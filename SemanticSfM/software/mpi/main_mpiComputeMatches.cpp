
// Copyright (c) 2012, 2013 Pierre MOULON.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "i23dSFM/sfm/sfm_data.hpp"
#include "i23dSFM/sfm/sfm_data_io.hpp"
#include "i23dSFM/sfm/pipelines/sfm_engine.hpp"
#include "i23dSFM/sfm/pipelines/sfm_features_provider.hpp"
#include "i23dSFM/sfm/pipelines/sfm_regions_provider.hpp"

/// Generic Image Collection image matching
#include "i23dSFM/matching_image_collection/Matcher_Regions_AllInMemory.hpp"
#include "i23dSFM/matching_image_collection/Matcher.hpp"
#include "i23dSFM/matching/matcher_cascade_hashing.hpp"
#include "i23dSFM/matching/indMatchDecoratorXY.hpp"
#include "i23dSFM/matching/matching_filters.hpp"
#include "i23dSFM/matching_image_collection/GeometricFilter.hpp"
#include "i23dSFM/matching_image_collection/F_ACRobust.hpp"
#include "i23dSFM/matching_image_collection/E_ACRobust.hpp"
#include "i23dSFM/matching_image_collection/H_ACRobust.hpp"
#include "i23dSFM/matching/pairwiseAdjacencyDisplay.hpp"
#include "i23dSFM/matching/indMatch_utils.hpp"
#include "i23dSFM/system/timer.hpp"

#include "i23dSFM/graph/graph.hpp"
#include "i23dSFM/stl/stl.hpp"
#include "third_party/cmdLine/cmdLine.h"
#include "third_party/stlplus3/filesystemSimplified/file_system.hpp"

#include <cstdlib>
#include <fstream>
#include"mpi.h"

using namespace i23dSFM;
using namespace i23dSFM::cameras;
using namespace i23dSFM::matching;
using namespace i23dSFM::robust;
using namespace i23dSFM::sfm;
using namespace i23dSFM::matching_image_collection;
using namespace std;
#define SEND_SERVER 0
//#define RECV_SERVER 1
enum TAG {
    IMAGE_ID,
    PAIR_INFO,
    DESCRIPTION,
    MATCHED_SIZE,
    CHECK_ID,
    MATCH_RESULT,
    THREAD_ID
};
enum EGeometricModel {
    FUNDAMENTAL_MATRIX = 0,
    ESSENTIAL_MATRIX = 1,
    HOMOGRAPHY_MATRIX = 2
};

enum EPairMode {
    PAIR_EXHAUSTIVE = 0,
    PAIR_CONTIGUOUS = 1,
    PAIR_FROM_FILE = 2
};

/// Compute corresponding features between a series of views:
/// - Load view images description (regions: features & descriptors)
/// - Compute putative local feature matches (descriptors matching)
/// - Compute geometric coherent feature matches (robust model estimation from putative matches)
/// - Export computed data
int main(int argc, char **argv) {
    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);//得到所有参加运算的进程的个数
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);//得到当前正在运行的进程的标识号
    const int RECV_SERVER=world_size-1;
    SfM_Data sfm_data;
    std::string sSfM_Data_Filename;
    std::string sMatchesDirectory = "";
    std::string sGeometricModel = "f";
    float fDistRatio = 0.8f;
    int iMatchingVideoMode = -1;
    std::string sPredefinedPairList = "";
    bool bUpRight = false;
    std::string sNearestMatchingMethod = "MPI";
    bool bForce = false;
    bool bGuided_matching = false;
    int imax_iteration = 2048;
    std::shared_ptr<Regions_Provider> regions_provider = std::make_shared<Regions_Provider>();
    PairWiseMatches map_PutativesMatches;
    EGeometricModel eGeometricModelToCompute = FUNDAMENTAL_MATRIX;
    std::string sGeometricMatchesFilename = "";
    std::vector<std::string> vec_fileNames;
    Pair_Set pairs;

    const size_t dimension = 128;
    // std::unique_ptr<Matcher> collectionMatcher;
    if (world_rank == SEND_SERVER || world_rank == RECV_SERVER) {
        CmdLine cmd;


        //required
        cmd.add(make_option('i', sSfM_Data_Filename, "input_file"));
        cmd.add(make_option('o', sMatchesDirectory, "out_dir"));
        // Options
        cmd.add(make_option('r', fDistRatio, "ratio"));
        cmd.add(make_option('g', sGeometricModel, "geometric_model"));
        cmd.add(make_option('v', iMatchingVideoMode, "video_mode_matching"));
        cmd.add(make_option('l', sPredefinedPairList, "pair_list"));
        cmd.add(make_option('n', sNearestMatchingMethod, "nearest_matching_method"));
        cmd.add(make_option('f', bForce, "force"));
        cmd.add(make_option('m', bGuided_matching, "guided_matching"));
        cmd.add(make_option('I', imax_iteration, "max_iteration"));

        try {
            if (argc == 1) throw std::string("Invalid command line parameter.");
            cmd.process(argc, argv);
        } catch (const std::string &s) {
            std::cerr << "Usage: " << argv[0] << '\n'
                      << "[-i|--input_file] a SfM_Data file\n"
                      << "[-o|--out_dir path] output path where computed are stored\n"
                      << "\n[Optional]\n"
                      << "[-f|--force] Force to recompute data]\n"
                      << "[-r|--ratio] Distance ratio to discard non meaningful matches\n"
                      << "   0.8: (default).\n"
                      << "[-g|--geometric_model]\n"
                      << "  (pairwise correspondences filtering thanks to robust model estimation):\n"
                      << "   f: (default) fundamental matrix,\n"
                      << "   e: essential matrix,\n"
                      << "   h: homography matrix.\n"
                      << "[-v|--video_mode_matching]\n"
                      << "  (sequence matching with an overlap of X images)\n"
                      << "   X: with match 0 with (1->X), ...]\n"
                      << "   2: will match 0 with (1,2), 1 with (2,3), ...\n"
                      << "   3: will match 0 with (1,2,3), 1 with (2,3,4), ...\n"
                      << "[-l]--pair_list] file\n"
                      << "[-n|--nearest_matching_method]\n"
                      << "  AUTO: auto choice from regions type,\n"
                      << "  For Scalar based regions descriptor:\n"
                      << "    BRUTEFORCEL2: L2 BruteForce matching,\n"
                      << "    ANNL2: L2 Approximate Nearest Neighbor matching,\n"
                      << "    CASCADEHASHINGL2: L2 Cascade Hashing matching.\n"
                      << "    FASTCASCADEHASHINGL2: (default)\n"
                      << "      L2 Cascade Hashing with precomputed hashed regions\n"
                      << "     (faster than CASCADEHASHINGL2 but use more memory).\n"
                      << "  For Binary based descriptor:\n"
                      << "    BRUTEFORCEHAMMING: BruteForce Hamming matching.\n"
                      << "[-m|--guided_matching]\n"
                      << "  use the found model to improve the pairwise correspondences."
                      << std::endl;

            std::cerr << s << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << " You called : " << "\n"
                  << argv[0] << "\n"
                  << "--input_file " << sSfM_Data_Filename << "\n"
                  << "--out_dir " << sMatchesDirectory << "\n"
                  << "Optional parameters:" << "\n"
                  << "--force " << bForce << "\n"
                  << "--ratio " << fDistRatio << "\n"
                  << "--geometric_model " << sGeometricModel << "\n"
                  << "--video_mode_matching " << iMatchingVideoMode << "\n"
                  << "--pair_list " << sPredefinedPairList << "\n"
                  << "--nearest_matching_method " << sNearestMatchingMethod << "\n"
                  << "--guided_matching " << bGuided_matching << std::endl;

        EPairMode ePairmode = (iMatchingVideoMode == -1) ? PAIR_EXHAUSTIVE : PAIR_CONTIGUOUS;

        if (sPredefinedPairList.length()) {
            ePairmode = PAIR_FROM_FILE;
            if (iMatchingVideoMode > 0) {
                std::cerr << "\nIncompatible options: --videoModeMatching and --pairList" << std::endl;
                return EXIT_FAILURE;
            }
        }

        if (sMatchesDirectory.empty() || !stlplus::is_folder(sMatchesDirectory)) {
            std::cerr << "\nIt is an invalid output directory" << std::endl;
            return EXIT_FAILURE;
        }


        switch (sGeometricModel[0]) {
            case 'f':
            case 'F':
                eGeometricModelToCompute = FUNDAMENTAL_MATRIX;
                sGeometricMatchesFilename = "matches.f.txt";
                break;
            case 'e':
            case 'E':
                eGeometricModelToCompute = ESSENTIAL_MATRIX;
                sGeometricMatchesFilename = "matches.e.txt";
                break;
            case 'h':
            case 'H':
                eGeometricModelToCompute = HOMOGRAPHY_MATRIX;
                sGeometricMatchesFilename = "matches.h.txt";
                break;
            default:
                std::cerr << "Unknown geometric model" << std::endl;
                return EXIT_FAILURE;
        }

        // -----------------------------
        // - Load SfM_Data Views & intrinsics data
        // a. Compute putative descriptor matches
        // b. Geometric filtering of putative matches
        // + Export some statistics
        // -----------------------------

        //---------------------------------------
        // Read SfM Scene (image view & intrinsics data)
        //---------------------------------------

        if (!Load(sfm_data, sSfM_Data_Filename, ESfM_Data(VIEWS | INTRINSICS))) {
            std::cerr << std::endl
                      << "The input SfM_Data file \"" << sSfM_Data_Filename << "\" cannot be read." << std::endl;
            return EXIT_FAILURE;
        }

        //---------------------------------------
        // Load SfM Scene regions
        //---------------------------------------
        // Init the regions_type from the image describer file (used for image regions extraction)
        using namespace i23dSFM::features;
        const std::string sImage_describer = stlplus::create_filespec(sMatchesDirectory, "image_describer", "json");
        std::unique_ptr<Regions> regions_type = Init_region_type_from_file(sImage_describer);
        if (!regions_type) {
            std::cerr << "Invalid: "
                      << sImage_describer << " regions type file." << std::endl;
            return EXIT_FAILURE;
        }

        //---------------------------------------
        // a. Compute putative descriptor matches
        //    - Descriptor matching (according user method choice)
        //    - Keep correspondences only if NearestNeighbor ratio is ok
        //---------------------------------------

        // Load the corresponding view regions
        if (!regions_provider->load(sfm_data, sMatchesDirectory, regions_type)) {
            std::cerr << std::endl << "Invalid regions." << std::endl;
            return EXIT_FAILURE;
        }



        // Build some alias from SfM_Data Views data:
        // - List views as a vector of filenames & image sizes

        std::vector<std::pair<size_t, size_t> > vec_imagesSize;
        {
            vec_fileNames.reserve(sfm_data.GetViews().size());
            vec_imagesSize.reserve(sfm_data.GetViews().size());
            for (Views::const_iterator iter = sfm_data.GetViews().begin();
                 iter != sfm_data.GetViews().end();
                 ++iter) {
                const View *v = iter->second.get();
                vec_fileNames.push_back(stlplus::create_filespec(sfm_data.s_root_path,
                                                                 v->s_Img_path));
                vec_imagesSize.push_back(std::make_pair(v->ui_width, v->ui_height));
            }
        }

        std::cout << std::endl << " - PUTATIVE MATCHES - " << std::endl;
        // If the matches already exists, reload them
        if (!bForce && stlplus::file_exists(sMatchesDirectory + "/matches.putative.txt")) {
            PairedIndMatchImport(sMatchesDirectory + "/matches.putative.txt", map_PutativesMatches);
            std::cout << "\t PREVIOUS RESULTS LOADED" << std::endl;
        }
        else // Compute the putative matches
        {
            std::cout << "Uses: ";
            switch (ePairmode) {
                case PAIR_EXHAUSTIVE:
                    std::cout << "exhaustive pairwise matching" << std::endl;
                    break;
                case PAIR_CONTIGUOUS:
                    std::cout << "sequence pairwise matching" << std::endl;
                    break;
                case PAIR_FROM_FILE:
                    std::cout << "user defined pairwise matching" << std::endl;
                    break;
            }


            // Perform the matching

            system::Timer timer;
            {
                // From matching mode compute the pair list that have to be matched:

                switch (ePairmode) {
                    case PAIR_EXHAUSTIVE:
                        pairs = exhaustivePairs(sfm_data.GetViews().size());
                        break;
                    case PAIR_CONTIGUOUS:
                        pairs = contiguousWithOverlap(sfm_data.GetViews().size(), iMatchingVideoMode);
                        break;
                    case PAIR_FROM_FILE:
                        if (!loadPairs(sfm_data.GetViews().size(), sPredefinedPairList, pairs)) {
                            return EXIT_FAILURE;
                        };
                        break;
                }

                // Photometric matching of putative pairs
                for (auto it : map_PutativesMatches) {
                    if (pairs.find(it.first) != pairs.end()) {
                        //std::cout << "Pair " << it.first.first << " " << it.first.second << " has been loaded" << std::endl;
                        pairs.erase(it.first);
                    }
                }


            }

            //collectionMatcher->Match(sfm_data, regions_provider, pairs, map_PutativesMatches);

            // std::cout << "Task (Regions Matching) done in (s): " << timer.elapsed() << std::endl;
        }


    }
    if (world_rank == SEND_SERVER) {

        deque<int> thread_pool;
        thread_pool.clear();
        /*
         * ０：发送线程
         * １：接收线程
         */
        for (int i = 1; i < world_size-1; ++i) {

            thread_pool.push_back(i);

        }
        unique_ptr<MPI_Request> recv_thread_requests(new MPI_Request[thread_pool.size()]);
        unique_ptr<MPI_Status> recv_thread_statuses(new MPI_Status[thread_pool.size()]);
        // for(auto p:pairs){



        std::set<IndexT> used_index;
        // Sort pairs according the first index to minimize later memory swapping
        typedef std::map<IndexT, std::vector<IndexT> > Map_vectorT;
        Map_vectorT map_Pairs;
        for (Pair_Set::const_iterator iter = pairs.begin(); iter != pairs.end(); ++iter) {
            map_Pairs[iter->first].push_back(iter->second);
            used_index.insert(iter->first);
            used_index.insert(iter->second);
        }
        for (auto map_pair:map_Pairs) {

            cout << "当前线程池状态:";
            for (auto thread : thread_pool) {
                cout << thread << " ";
            }
            cout << endl;
            if (thread_pool.empty()) {
                cout << "等待空闲线程" << endl;

                int response = 0;
                MPI_Waitany(world_size - 2, recv_thread_requests.get(), &response, recv_thread_statuses.get());

                //unsigned  recover=recv_thread_statuses.get()[response].MPI_SOURCE;
                cout << response << " 回收的线程:" << response +1 << endl;
                thread_pool.push_front(response +1);


            }
            int current_thread = thread_pool.front();
            thread_pool.pop_front();


            /*
             * 1:  主图片id　和下一次传输的大小
             * 2:  图片id 图片特征数量
             * 3:  图片特征
             */
            unsigned int img_id_size[2];
            img_id_size[0] = map_pair.first;
            // contains self
            img_id_size[1] = map_pair.second.size() + 1;
//            cout << "发送主图片ｉｄ，图片数量:" << img_id_size[0] << " " << img_id_size[1] << endl;
            MPI_Send(img_id_size, 2, MPI_UNSIGNED, current_thread, IMAGE_ID, MPI_COMM_WORLD);
            vector<unsigned int> img_infos;
            vector<unsigned char> img_descs;
            int img_descs_count = 0;
            img_infos.push_back(map_pair.first);
            img_infos.push_back(regions_provider.get()->regions_per_view[map_pair.first].get()->RegionCount());
            img_descs_count += regions_provider.get()->regions_per_view[map_pair.first].get()->RegionCount();
            for (auto item:map_pair.second) {
                img_infos.push_back(item);
//                cout<<"图片信息："<<item<<" "<<regions_provider.get()->regions_per_view[item].get()->RegionCount()<<endl;
                img_infos.push_back(regions_provider.get()->regions_per_view[item].get()->RegionCount());
                img_descs_count += regions_provider.get()->regions_per_view[item].get()->RegionCount();
            }
//            cout << "发送主图片ｉｄ和图片特征数目:" << endl;

            MPI_Send(img_infos.data(), img_infos.size(), MPI_UNSIGNED, current_thread, PAIR_INFO, MPI_COMM_WORLD);


            img_descs.resize(img_descs_count * dimension);
            auto ptr = img_descs.data();
            for (int i = 0; i < img_id_size[1]; ++i) {
                memcpy(ptr, regions_provider.get()->regions_per_view[img_infos[2*i]].get()->DescriptorRawData(),
                       sizeof(unsigned char) * img_infos[2*i + 1] * dimension);
                ptr += img_infos[2*i + 1] * dimension;
            }
//            cout << "发送主图片ｉｄ和图片特征描述符:" << endl;
            MPI_Send(img_descs.data(), img_descs_count * dimension, MPI_UNSIGNED_CHAR, current_thread, DESCRIPTION,
                     MPI_COMM_WORLD);
            MPI_Irecv(NULL, 0, MPI_UNSIGNED, RECV_SERVER, THREAD_ID, MPI_COMM_WORLD,
                      &(recv_thread_requests.get()[current_thread - 1]));


        }








        //  }

        unsigned int buf[] = {-1, 0};
        for (int i = 1; i < world_size-1; ++i) {

            cout << "Finalize thread:" << i << endl;

            MPI_Send(buf, 2, MPI_UNSIGNED, i, IMAGE_ID, MPI_COMM_WORLD);

        }
//        unsigned int local_finished[]={0,0};
//        MPI_Send(&local_finished,2,MPI_INT,RECV_SERVER,IMAGE_ID_FEATURE_SIZE,MPI_COMM_WORLD);
        cout << "send server ended!" << endl;
    }
    if (world_rank != SEND_SERVER && world_rank != RECV_SERVER) {
        typedef Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> BaseMat;


        // Init the cascade hasher
        CascadeHasher cascade_hasher;
        vector<unsigned int> result;
        cascade_hasher.Init(128);
        while (true) {
            unsigned int img_id_size[2];
            MPI_Status status;
            MPI_Recv(img_id_size, 2, MPI_UNSIGNED, SEND_SERVER, IMAGE_ID, MPI_COMM_WORLD, &status);

            cout << "接收主图片ｉｄ，从图片数量:" << img_id_size[0] << " " << img_id_size[1] << endl;
            if (img_id_size[0] == -1) {
                cout << "接收到停止信号" << endl;
                MPI_Send(img_id_size, 2, MPI_UNSIGNED, RECV_SERVER, MATCHED_SIZE, MPI_COMM_WORLD);

                break;
            }

            result.clear();
            //所有图片的ｉｄ 和特征数量
            vector<int> pairs_infos(img_id_size[1] * 2);
            vector<int> steps;
            MPI_Recv(pairs_infos.data(), img_id_size[1] * 2, MPI_UNSIGNED, SEND_SERVER, PAIR_INFO, MPI_COMM_WORLD,
                     &status);

            int total_des_size = 0;

            for (int i = 1; i < pairs_infos.size(); i += 2) {
                steps.push_back(total_des_size);
                total_des_size += pairs_infos[i];

            }
            cout<<"步长:";

            for(int i=0;i<steps.size();++i){
                cout<<steps[i]<<" ";
            }

            cout<<endl;
//            cout << "接收主图片ｉｄ和图片特征数目:" << total_des_size << endl;
            vector<unsigned char> buf(total_des_size * dimension);
            unsigned char *des_ptr = buf.data();
            MPI_Recv(buf.data(), buf.size(), MPI_UNSIGNED_CHAR, SEND_SERVER, DESCRIPTION, MPI_COMM_WORLD, &status);
//            cout << "接收主图片ｉｄ和图片特征描述:" << total_des_size * dimension << endl;
            typedef unsigned char ScalarT;
            typedef Eigen::Matrix<ScalarT, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> BaseMat;



            // Init the cascade hasher
            CascadeHasher cascade_hasher;
            cascade_hasher.Init(dimension);


            std::map<IndexT, HashedDescriptions> hashed_base_;

            // Compute the zero mean descriptor that will be used for hashing (one for all the image regions)
            Eigen::VectorXf zero_mean_descriptor;
            {
                Eigen::MatrixXf matForZeroMean;
                for (int i = 0; i < img_id_size[1]; ++i) {

                    const ScalarT *tabI =
                            reinterpret_cast<const ScalarT *>(des_ptr+steps[i]*dimension);

                    if (i == 0) {
                        matForZeroMean.resize(img_id_size[1], dimension);
                        matForZeroMean.fill(0.0f);
                    }
                    if (pairs_infos[2 * i + 1] > 0) {
                        Eigen::Map<BaseMat> mat_I((ScalarT *) tabI, pairs_infos[2 * i + 1], dimension);
                        matForZeroMean.row(i) = CascadeHasher::GetZeroMeanDescriptor(mat_I);
                    }

                }
                zero_mean_descriptor = CascadeHasher::GetZeroMeanDescriptor(matForZeroMean);
            }
            des_ptr = buf.data();
            // Index the input regions
#ifdef I23DSFM_USE_OPENMP
#pragma omp parallel for schedule(dynamic)
#endif

            for (int i = 0; i < img_id_size[1]; ++i) {

                const ScalarT *tabI =
                        reinterpret_cast<const ScalarT *>(des_ptr+steps[i] * dimension);




                Eigen::Map<BaseMat> mat_I((ScalarT *) tabI, pairs_infos[i * 2 + 1], dimension);
                const HashedDescriptions hashed_description = cascade_hasher.CreateHashedDescriptions(mat_I,
                                                                                                      zero_mean_descriptor);
#ifdef I23DSFM_USE_OPENMP
#pragma omp critical
#endif
                {
                    hashed_base_[pairs_infos[i * 2]] = std::move(hashed_description);
                }


            }
            des_ptr = buf.data();
            // Perform matching between all the pairs
            //for (int i = 0; i < 1; i++) {
            const IndexT I = pairs_infos[0];
//                if (pairs_infos[1] == 0) {
//
//                    continue;
//                }

            //const std::vector<features::PointFeature> pointFeaturesI = regionsI.GetRegionsPositions();
            const ScalarT *tabI = des_ptr;

            Eigen::Map<BaseMat> mat_I((ScalarT*)tabI, pairs_infos[1], dimension);


#ifdef I23DSFM_USE_OPENMP
#pragma omp parallel for schedule(dynamic)
#endif
            for (int j = 1; j < img_id_size[1]; ++j) {
                const IndexT J = pairs_infos[j * 2];


                cout<<I<<J<<"开始匹配"<<endl;
                // Matrix representation of the query input data;
                const ScalarT *tabJ = reinterpret_cast<const ScalarT *>(des_ptr+steps[j]*dimension);

                Eigen::Map<BaseMat> mat_J((ScalarT *) tabJ, pairs_infos[j * 2 + 1], dimension);

                IndMatches pvec_indices;
                typedef typename Accumulator<ScalarT>::Type ResultType;
                std::vector<ResultType> pvec_distances;
                pvec_distances.reserve(pairs_infos[j * 2 + 1] * 2);
                pvec_indices.reserve(pairs_infos[j * 2 + 1] * 2);

                // Match the query descriptors to the database
                cascade_hasher.Match_HashedDescriptions<BaseMat, ResultType>(
                        hashed_base_[J], mat_J,
                        hashed_base_[I], mat_I,
                        &pvec_indices, &pvec_distances);

                std::vector<int> vec_nn_ratio_idx;
                // Filter the matches using a distance ratio test:
                //   The probability that a match is correct is determined by taking
                //   the ratio of distance from the closest neighbor to the distance
                //   of the second closest.
                matching::NNdistanceRatio(
                        pvec_distances.begin(), // distance start
                        pvec_distances.end(),   // distance end
                        2, // Number of neighbor in iterator sequence (minimum required 2)
                        vec_nn_ratio_idx, // output (indices that respect the distance Ratio)
                        Square(fDistRatio));

                matching::IndMatches vec_putative_matches;
                vec_putative_matches.reserve(vec_nn_ratio_idx.size());
                for (size_t k = 0; k < vec_nn_ratio_idx.size(); ++k) {
                    const size_t index = vec_nn_ratio_idx[k];
                    vec_putative_matches.emplace_back(pvec_indices[index * 2]._j, pvec_indices[index * 2]._i);
                }

                // Remove duplicates
                matching::IndMatch::getDeduplicated(vec_putative_matches);

                // Remove matches that have the same (X,Y) coordinates



#ifdef I23DSFM_USE_OPENMP
#pragma omp critical
#endif
                {

                    if (!vec_putative_matches.empty()) {

                        result.push_back(I);
                        result.push_back(J);
                        result.push_back(vec_putative_matches.size());
                        cout << I << " " << J << " : " << vec_putative_matches.size() << endl;
                        for (auto match:vec_putative_matches) {
                            result.push_back(match._i);
                            result.push_back(match._j);
                        }
                        //map_PutativesMatches.insert(make_pair(make_pair(I, J), std::move(vec_putative_matches)));

                    }
                }
            }
            //}

            unsigned int match_id_size[] = {img_id_size[0], result.size()};

            MPI_Send(match_id_size, 2, MPI_UNSIGNED, RECV_SERVER, MATCHED_SIZE, MPI_COMM_WORLD);

            MPI_Send(result.data(), result.size(), MPI_UNSIGNED, RECV_SERVER, MATCH_RESULT, MPI_COMM_WORLD);


        }

        cout<<"remote thread "<< world_rank<<" ended!"<<endl;
    }
    if (world_rank == RECV_SERVER) {
        set<int> received;

        /*
         * 接收数据
         */
        unsigned int match_id_size[2];
        MPI_Status status;
        int SOURCE_TAG;
        while (received.size() < world_size - 2) {
            MPI_Recv(match_id_size, 2, MPI_UNSIGNED, MPI_ANY_SOURCE, MATCHED_SIZE, MPI_COMM_WORLD, &status);
            SOURCE_TAG = status.MPI_SOURCE;
            if (match_id_size[0] == -1) {
                received.insert(SOURCE_TAG);
                cout << "已结束线程数" << received.size() << endl;

                continue;
            }


            vector<unsigned int> match_result(match_id_size[1]);

            MPI_Recv(match_result.data(), match_id_size[1], MPI_UNSIGNED, SOURCE_TAG, MATCH_RESULT, MPI_COMM_WORLD,
                     &status);

            int i = 0;
            while (i < match_id_size[1]) {
                IndMatches vec_putative_matches;
                int I = match_result[i], J = match_result[i + 1];
                int size = match_result[i + 2];
                for (int j = 0; j < size; ++j) {
                    vec_putative_matches.emplace_back(match_result[i + 3 + 2 * j], match_result[i + 3 + 2 * j + 1]);
                }
                auto regionsJ = regions_provider.get()->regions_per_view.at(J).get();
                const std::vector<features::PointFeature> pointFeaturesJ = regionsJ->GetRegionsPositions();

                auto regionsI = regions_provider.get()->regions_per_view.at(I).get();
                const std::vector<features::PointFeature> pointFeaturesI = regionsI->GetRegionsPositions();

                matching::IndMatchDecorator<float> matchDeduplicator(vec_putative_matches,
                                                                     pointFeaturesI, pointFeaturesJ);
                matchDeduplicator.getDeduplicated(vec_putative_matches);
                map_PutativesMatches.insert(make_pair(make_pair(I, J), std::move(vec_putative_matches)));
                i += 3 + size * 2;
            }

            MPI_Send(NULL, 0, MPI_UNSIGNED, SEND_SERVER, THREAD_ID, MPI_COMM_WORLD);
        }
        cout << "初始匹配完成" << endl;
        std::ofstream file(std::string(sMatchesDirectory + "/matches.putative.txt").c_str());
        if (file.is_open())
            PairedIndMatchToStream(map_PutativesMatches, file);
        file.close();
        //---------------------------------------
        //-- Export putative matches
//        //---------------------------------------
//           std::ofstream file(std::string(sMatchesDirectory + "/matches.putative.txt").c_str());
//        if (file.is_open())
//            PairedIndMatchToStream(map_PutativesMatches, file);
//        file.close();

        //-- export putative matches Adjacency matrix
        PairWiseMatchingToAdjacencyMatrixSVG(vec_fileNames.size(),
                                             map_PutativesMatches,
                                             stlplus::create_filespec(sMatchesDirectory, "PutativeAdjacencyMatrix",
                                                                      "svg"));
        //-- export view pair graph once putative graph matches have been computed
        {
            std::set<IndexT> set_ViewIds;
            std::transform(sfm_data.GetViews().begin(), sfm_data.GetViews().end(),
                           std::inserter(set_ViewIds, set_ViewIds.begin()), stl::RetrieveKey());
            graph::indexedGraph putativeGraph(set_ViewIds, getPairs(map_PutativesMatches));
            graph::exportToGraphvizData(
                    stlplus::create_filespec(sMatchesDirectory, "putative_matches"),
                    putativeGraph.g);
        }
        //---------------------------------------
        // b. Geometric filtering of putative matches
        //    - AContrario Estimation of the desired geometric model
        //    - Use an upper bound for the a contrario estimated threshold
        //---------------------------------------

        std::unique_ptr<ImageCollectionGeometricFilter> filter_ptr(
                new ImageCollectionGeometricFilter(&sfm_data, regions_provider));

        if (filter_ptr) {
            system::Timer timer;
            std::cout << std::endl << " - Geometric filtering - " << std::endl;

            PairWiseMatches map_GeometricMatches;
            switch (eGeometricModelToCompute) {
                case HOMOGRAPHY_MATRIX: {
                    const bool bGeometric_only_guided_matching = true;
                    filter_ptr->Robust_model_estimation(GeometricFilter_HMatrix_AC(4.0, imax_iteration),
                                                        map_PutativesMatches, bGuided_matching,
                                                        bGeometric_only_guided_matching ? -1.0 : 0.6);
                    map_GeometricMatches = filter_ptr->Get_geometric_matches();
                }
                    break;
                case FUNDAMENTAL_MATRIX: {
                    filter_ptr->Robust_model_estimation(GeometricFilter_FMatrix_AC(4.0, imax_iteration),
                                                        map_PutativesMatches, bGuided_matching);
                    map_GeometricMatches = filter_ptr->Get_geometric_matches();
                }
                    break;
                case ESSENTIAL_MATRIX: {
                    filter_ptr->Robust_model_estimation(GeometricFilter_EMatrix_AC(4.0, imax_iteration),
                                                        map_PutativesMatches, bGuided_matching);
                    map_GeometricMatches = filter_ptr->Get_geometric_matches();

                    //-- Perform an additional check to remove pairs with poor overlap
                    std::vector<PairWiseMatches::key_type> vec_toRemove;
                    for (PairWiseMatches::const_iterator iterMap = map_GeometricMatches.begin();
                         iterMap != map_GeometricMatches.end(); ++iterMap) {
                        const size_t putativePhotometricCount = map_PutativesMatches.find(
                                iterMap->first)->second.size();
                        const size_t putativeGeometricCount = iterMap->second.size();
                        const float ratio = putativeGeometricCount / (float) putativePhotometricCount;
                        if (putativeGeometricCount < 50 || ratio < .3f) {
                            // the pair will be removed
                            vec_toRemove.push_back(iterMap->first);
                        }
                    }
                    //-- remove discarded pairs
                    for (std::vector<PairWiseMatches::key_type>::const_iterator
                                 iter = vec_toRemove.begin(); iter != vec_toRemove.end(); ++iter) {
                        map_GeometricMatches.erase(*iter);
                    }
                }
                    break;
            }


            //---------------------------------------
            //-- Export geometric filtered matches
            //---------------------------------------
            std::ofstream file(string(sMatchesDirectory + "/" + sGeometricMatchesFilename).c_str());
            if (file.is_open())
                PairedIndMatchToStream(map_GeometricMatches, file);
            file.close();

            std::cout << "Task done in (s): " << timer.elapsed() << std::endl;

            //-- export Adjacency matrix
            std::cout << "\n Export Adjacency Matrix of the pairwise's geometric matches"
                      << std::endl;
            PairWiseMatchingToAdjacencyMatrixSVG(vec_fileNames.size(),
                                                 map_GeometricMatches,
                                                 stlplus::create_filespec(sMatchesDirectory, "GeometricAdjacencyMatrix",
                                                                          "svg"));

            //-- export view pair graph once geometric filter have been done
            {
                std::set<IndexT> set_ViewIds;
                std::transform(sfm_data.GetViews().begin(), sfm_data.GetViews().end(),
                               std::inserter(set_ViewIds, set_ViewIds.begin()), stl::RetrieveKey());
                graph::indexedGraph putativeGraph(set_ViewIds, getPairs(map_GeometricMatches));
                graph::exportToGraphvizData(
                        stlplus::create_filespec(sMatchesDirectory, "geometric_matches"),
                        putativeGraph.g);
            }
        }
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
