//
// Created by haibao637 on 17-8-9.
//


// Copyright (c) 2012, 2013 Pierre MOULON.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


#include "i23dSFM/image/image.hpp"
#include "i23dSFM/sfm/sfm.hpp"

#include"mpi.h"
/// Feature/Regions & Image describer interfaces
#include "i23dSFM/features/features.hpp"
#include "nonFree/sift/SIFT_describer.hpp"
#include <cereal/archives/json.hpp>
#include "i23dSFM/system/timer.hpp"

#include "third_party/cmdLine/cmdLine.h"
#include "third_party/stlplus3/filesystemSimplified/file_system.hpp"
#include "third_party/progress/progress.hpp"

#include <cstdlib>
#include <fstream>
#include<deque>
#include<set>

#define SEND_SERVER 0

//#define RECV_SERVER 1
using namespace i23dSFM;
using namespace i23dSFM::image;
using namespace i23dSFM::features;
using namespace i23dSFM::sfm;
using namespace std;

features::EDESCRIBER_PRESET stringToEnum(const std::string &sPreset) {
    features::EDESCRIBER_PRESET preset;
    if (sPreset == "NORMAL")
        preset = features::NORMAL_PRESET;
    else if (sPreset == "HIGH")
        preset = features::HIGH_PRESET;
    else if (sPreset == "ULTRA")
        preset = features::ULTRA_PRESET;
    else
        preset = features::EDESCRIBER_PRESET(-1);
    return preset;
}

enum TAG {
    SIZE, FEATURE, DESCRIPTION, IMAGE_ID, IMAGE, IMAGE_ID_FEATURE_SIZE, THREAD_ID, FINISHED, CHECK_ID
};
using namespace i23dSFM::features;

/// - Compute view image description (feature & descriptor extraction)
/// - Export computed data
int main(int argc, char **argv) {
    CmdLine cmd;

    std::string sSfM_Data_Filename;
    std::string sOutDir = "";
    bool bUpRight = false;
    std::string sImage_Describer_Method = "SIFT";
    bool bForce = false;
    std::string sFeaturePreset = "";
    int client_nums = 1;
//    int REMOTE_SEND_SERVER;
    // required
    cmd.add(make_option('i', sSfM_Data_Filename, "input_file"));
    cmd.add(make_option('o', sOutDir, "outdir"));
    // Optional
    cmd.add(make_option('m', sImage_Describer_Method, "describerMethod"));
    cmd.add(make_option('u', bUpRight, "upright"));
    cmd.add(make_option('f', bForce, "force"));
    cmd.add(make_option('p', sFeaturePreset, "describerPreset"));
    cmd.add(make_option('n', client_nums, "client node numbers"));

    try {
        if (argc == 1) throw std::string("Invalid command line parameter.");
        cmd.process(argc, argv);
    } catch (const std::string &s) {

        std::cerr << "Usage: " << argv[0] << '\n'
                  << "[-i|--input_file] a SfM_Data file \n"
                  << "[-o|--outdir path] \n"
                  << "\n[Optional]\n"
                  << "[-f|--force] Force to recompute data\n"
                  << "[-m|--describerMethod]\n"
                  << "  (method to use to describe an image):\n"
                  << "   SIFT (default),\n"
                  << "   AKAZE_FLOAT: AKAZE with floating point descriptors,\n"
                  << "   AKAZE_MLDB:  AKAZE with binary descriptors\n"
                  << "[-u|--upright] Use Upright feature 0 or 1\n"
                  << "[-p|--describerPreset]\n"
                  << "  (used to control the Image_describer configuration):\n"
                  << "   NORMAL (default),\n"
                  << "   HIGH,\n"
                  << "   ULTRA: !!Can take long time!!\n"
                  << std::endl;

        std::cerr << s << std::endl;
        return EXIT_FAILURE;
    }





    // Feature extraction routines
    // For each View of the SfM_Data container:
    // - if regions file exist continue,
    // - if no file, compute features




    /*
     * mipi init
     *
     */

    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);//得到所有参加运算的进程的个数

    // unsigned  Running=1;

    //get the rank of the process
    int world_rank;
    //size_t size=world_size*3;
    Hash_Map<IndexT, string> sFeats;
    Hash_Map<IndexT, string> sDescs;

    Hash_Map<IndexT, string> sView_filenames;

    Hash_Map<IndexT, string> sMask_filenames;

    //shared_ptr<unsigned int>  feature_size(new unsigned int(world_size)); //保存每张图片的特征点数目
    SfM_Data sfm_data;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);//得到当前正在运行的进程的标识号


//    REMOTE_SEND_SERVER = world_size - client_nums - 1;
//    assert(REMOTE_SEND_SERVER > 0);
    std::unique_ptr<Image_describer> image_describer;
    unique_ptr<unsigned int> views_info = nullptr;//保存图片的大小信息
    int image_nums = 0;

    if (world_rank == SEND_SERVER) {
        std::cout << " You called : " << std::endl
                  << argv[0] << std::endl
                  << "--input_file " << sSfM_Data_Filename << std::endl
                  << "--outdir " << sOutDir << std::endl
                  << "--describerMethod " << sImage_Describer_Method << std::endl
                  << "--upright " << bUpRight << std::endl
                  << "--describerPreset " << (sFeaturePreset.empty() ? "NORMAL" : sFeaturePreset) << std::endl
                  << "--force " << bForce << std::endl
                  << "--[-n] the client node numbers " << client_nums << std::endl;
    }


    const std::string sImage_describer = stlplus::create_filespec(sOutDir, "image_describer", "json");
    if (!bForce && stlplus::is_file(sImage_describer) && (world_rank == SEND_SERVER )) {
        // Dynamically load the image_describer from the file (will restore old used settings)
        std::ifstream stream(sImage_describer.c_str());
        if (!stream.is_open())
            return false;

        try {
            cereal::JSONInputArchive archive(stream);
            archive(cereal::make_nvp("image_describer", image_describer));
        }
        catch (const cereal::Exception &e) {
            std::cerr << e.what() << std::endl
                      << "Cannot dynamically allocate the Image_describer interface." << std::endl;
            return EXIT_FAILURE;
        }
    } else {
        // Create the desired Image_describer method.
        // Don't use a factory, perform direct allocation
        if (sImage_Describer_Method == "SIFT") {
            image_describer.reset(new SIFT_Image_describer(SiftParams(), !bUpRight));
        } else if (sImage_Describer_Method == "AKAZE_FLOAT") {
            image_describer.reset(new AKAZE_Image_describer(AKAZEParams(AKAZEConfig(), AKAZE_MSURF), !bUpRight));
        } else if (sImage_Describer_Method == "AKAZE_MLDB") {
            image_describer.reset(new AKAZE_Image_describer(AKAZEParams(AKAZEConfig(), AKAZE_MLDB), !bUpRight));
        }
        //image_describer.reset(new AKAZE_Image_describer(AKAZEParams(AKAZEConfig(), AKAZE_LIOP), !bUpRight));
        if (!image_describer) {
            std::cerr << "Cannot create the designed Image_describer:"
                      << sImage_Describer_Method << "." << std::endl;
            return EXIT_FAILURE;
        } else {
            if (!sFeaturePreset.empty())
                if (!image_describer->Set_configuration_preset(stringToEnum(sFeaturePreset))) {
                    std::cerr << "Preset configuration failed." << std::endl;
                    return EXIT_FAILURE;
                }
        }

        if (world_rank == SEND_SERVER)
            // Export the used Image_describer and region type for:
            // - dynamic future regions computation and/or loading
        {
            std::ofstream stream(sImage_describer.c_str());
            if (!stream.is_open())
                return false;

            cereal::JSONOutputArchive archive(stream);
            archive(cereal::make_nvp("image_describer", image_describer));
            std::unique_ptr<Regions> regionsType;
            image_describer->Allocate(regionsType);
            archive(cereal::make_nvp("regions_type", regionsType));
        }
    }


    //unsigned size=0;
    /*
     *
     * change of mipi patch
     */
    //主进程
    if (world_rank == SEND_SERVER ) {

        if (sOutDir.empty()) {
            std::cerr << "\nIt is an invalid output directory" << std::endl;
            return EXIT_FAILURE;
        }

        // Create output dir
        if (!stlplus::folder_exists(sOutDir)) {
            if (!stlplus::folder_create(sOutDir)) {
                std::cerr << "Cannot create output directory" << std::endl;
                return EXIT_FAILURE;
            }
        }

        //---------------------------------------
        // a. Load input scene
        //---------------------------------------

        if (!Load(sfm_data, sSfM_Data_Filename, ESfM_Data(VIEWS | INTRINSICS))) {
            std::cerr << std::endl
                      << "The input file \"" << sSfM_Data_Filename << "\" cannot be read" << std::endl;
            return false;
        }

        for (auto iter:sfm_data.views) {
            auto view = iter.second;
            const std::string sView_filename = stlplus::create_filespec(sfm_data.s_root_path,
                                                                        view->s_Img_path);

            sView_filenames[view->id_view] = sView_filename;
            const std::string sFeat = stlplus::create_filespec(sOutDir,
                                                               stlplus::basename_part(sView_filename), "feat");
            sFeats[view->id_view] = sFeat;
            const std::string sDesc = stlplus::create_filespec(sOutDir,
                                                               stlplus::basename_part(sView_filename), "desc");

            sDescs[view->id_view] = sDesc;
            const std::string sMask_filename = stlplus::create_filespec(sfm_data.s_root_path + "mask/",
                                                                        stlplus::basename_part(sView_filename), "msk");

            sMask_filenames[view->id_view] = sMask_filename;
        }

        //views_info.resize(size);
        //mpi 先广播图片信息，ID,高,宽

        for (auto iterViews:sfm_data.GetViews()) {

            const View *view = iterViews.second.get();
            if (bForce || !stlplus::file_exists(sFeats[view->id_view]) ||
                !stlplus::file_exists(sDescs[view->id_view])) {
                image_nums++;
                //

            }

        }


    }


    MPI_Bcast(&image_nums, 1, MPI_INT, SEND_SERVER, MPI_COMM_WORLD);
    if (image_nums == 0) {

        return 0;
    }
    // cout<<"bcast size ended"<<endl;
    // MPI_Barrier(MPI_COMM_WORLD);

    views_info.reset(new unsigned int[image_nums * 3]);
    cout << "image_nums:" << image_nums << endl;
    assert(views_info != nullptr);
    if (world_rank == SEND_SERVER) {
        int i = 0;
        for (auto iterViews:sfm_data.GetViews()) {

            const View *view = iterViews.second.get();
            if (bForce || !stlplus::file_exists(sFeats[view->id_view]) ||
                !stlplus::file_exists(sDescs[view->id_view])) {

                //
                views_info.get()[i++] = (iterViews.first);
                views_info.get()[i++] = (view->ui_height);
                views_info.get()[i++] = (view->ui_width);
            }

        }
    }

    MPI_Bcast(views_info.get(), image_nums * 3, MPI_UNSIGNED, SEND_SERVER, MPI_COMM_WORLD);
    // b. Init the image_describer
    // - retrieve the used one in case of pre-computed features
    // - else create the desired one


    if (world_rank == SEND_SERVER) {
        cout << "总进程数:" << world_size << endl;
        cout << "需要sift提取的图片数目" << image_nums << endl;
        cout << "本地服务线程ID" << SEND_SERVER << endl ;

        system::Timer timer;


        // sFeats.resize(sfm_data.GetViews().size());
        //打包数据

        deque<int> thread_pool;
        thread_pool.clear();
        for (int i = 1; i < world_size; ++i) {

            thread_pool.push_back(i);

        }



//            for(Views::const_iterator iterViews = sfm_data.views.begin();
//                iterViews != sfm_data.views.end();
//                ++iterViews, ++my_progress_bar)
        unique_ptr<MPI_Request> recv_thread_requests(new MPI_Request[thread_pool.size()]);
        unique_ptr<MPI_Status> recv_thread_statuses(new MPI_Status[thread_pool.size()]);
        //unique_ptr<int> recv_thread_id(new int[thread_pool.size()]);
        //unique_ptr<unsigned int> threads(new unsigned int[world_size-2]);
//            unique_ptr<Image<unsigned  char>> imageGrays(new Image<unsigned char>[image_nums]);
        Image<unsigned char> imageGray;
        for (int index = 0; index < image_nums; ++index) {
            unsigned id_view = views_info.get()[3 * index];
//                auto iter :sfm_data.GetViews();
//                auto view=iter.second;
            if (thread_pool.empty()) {
                cout << "等待空闲线程" << endl;
//
                int response = 0;
                MPI_Waitany(world_size - 2, recv_thread_requests.get(), &response,
                            recv_thread_statuses.get());

                //unsigned  recover=recv_thread_statuses.get()[response].MPI_SOURCE;
                cout << response << " 回收的线程:" << (response + 1) << endl;

                thread_pool.push_back((response + 1));


            }


            //If features or descriptors file are missing, compute them
            if (bForce || !stlplus::file_exists(sFeats[id_view]) || !stlplus::file_exists(sDescs[id_view])) {



                //MPI_Send(&this_thread,imageGray.size(),MPI_INT,id_view+1,OMP_THREAD,MPI_COMM_WORLD);
                //MPI_Isend(imageGray.data(),imageGray.size(),MPI_UNSIGNED_CHAR,id_view+1,IMAGE,MPI_COMM_WORLD,)
                //打包数据
                //cout << "发送　" << id_view << " 的图片数据" << endl;
                //+2　表示跳过服务进程，
                //没隔一轮检测一次



                //当线程池不为空时，

                if (!thread_pool.empty()) {
                    cout << "线程池状态：";
                    for (auto x: thread_pool) {
                        cout << x << " ";
                    }
                    cout << endl;
                    int current_thread = thread_pool.front();
                    thread_pool.pop_front();
                    //给当前线程发送图片id
//                    cout << id_view << "-> " << current_thread << "　发送数据" << endl;
//
                    auto img_str = sView_filenames[id_view];
                    auto name_len = img_str.size() + 1;
                    MPI_Send(&name_len, 1, MPI_UNSIGNED, current_thread, IMAGE_ID, MPI_COMM_WORLD);
                    MPI_Send(img_str.c_str(), name_len, MPI_UNSIGNED_CHAR, current_thread, IMAGE,
                             MPI_COMM_WORLD);

                    //MPI_Request request;
                    //MPI_Status status;

                    //threads.get()[current_thread]=(current_thread);

                    //当前进程－１
                    MPI_Irecv(NULL, 0, MPI_UNSIGNED, current_thread, THREAD_ID, MPI_COMM_WORLD,
                              &(recv_thread_requests.get()[current_thread - 1]));
                }


                // MPI_Isend(imageGray->data(),imageGray->size(),MPI_UNSIGNED_CHAR,id_view+1,IMAGE,MPI_COMM_WORLD,requests.get()+index);



            }
        }

//        MPI_Status recv_thread_status;
//        MPI_Recv(NULL,0,MPI_UNSIGNED,RECV_SERVER,FINISHED,MPI_COMM_WORLD,&recv_thread_status);
//        unsigned  finished=-1;
//        for(int i=1;i<world_size-1;++i){
//            if(world_rank==RECV_SERVER||world_rank==REMOTE_SEND_SERVER){
//                continue;
//            }
//            cout<<"Finalize thread:"<<i<<endl;
//            MPI_Send(&finished,1,MPI_UNSIGNED,i,IMAGE_ID,MPI_COMM_WORLD);
//        }

        //MPI_Waitall(image_nums,requests.data(),statuses.data());
        // MPI_Waitall(world_size/client_nums-3,recv_thread_requests.get(),recv_thread_statuses.get());
        unsigned finished = -1;
        for (int i = 1; i < world_size; ++i) {

            cout << "Finalize thread:" << i << endl;
            MPI_Send(&finished, 1, MPI_UNSIGNED, i, IMAGE_ID, MPI_COMM_WORLD);

        }
//        unsigned int local_finished[]={0,0};
//        MPI_Send(&local_finished,2,MPI_INT,RECV_SERVER,IMAGE_ID_FEATURE_SIZE,MPI_COMM_WORLD);
        cout << "send server ended!" << endl;
    }
    if (world_rank != SEND_SERVER) {

        //进程结束标志
        std::unique_ptr<Regions> regions;
//            MPI_Request  request;
//            MPI_Irecv(&Running,1,MPI_UNSIGNED,RECV_SERVER,FINISHED,MPI_COMM_WORLD,&request);


//            MPI_Bcast(&image_nums,1,MPI_UNSIGNED,ROOT,MPI_COMM_WORLD);
//            cout<<"成功接收　图片数目信息:"<<image_nums<<endl;
//            //接受广播信息
//
//            MPI_Bcast(views_info.get(),world_size*3,MPI_UNSIGNED,ROOT,MPI_COMM_WORLD);




        Hash_Map<unsigned, pair<unsigned, unsigned >> views_size;
        assert(views_info != nullptr);

        for (int i = 0; i < image_nums; ++i) {
            //cout<<"图片id:"<<views_info.get()[3*i]<<endl;
            views_size[views_info.get()[3 * i]] = make_pair(views_info.get()[3 * i + 1], views_info.get()[3 * i + 2]);
        }

        while (true) {


            vector<float> points;
            unsigned int buf[2];

            unsigned h, w, img_size;
            //un MPI_Send(&id_view,1,MPI_UNSIGNED,current_thread,IMAGE_ID,MPI_COMM_WORLD);
            MPI_Status status;
            vector<unsigned char> rec_buff;
            Image<unsigned char> imageGray;
            Image<unsigned char> *im = &imageGray;
            unsigned file_name_len;
            MPI_Recv(&file_name_len, 1, MPI_UNSIGNED, SEND_SERVER, IMAGE_ID, MPI_COMM_WORLD, &status);

            unique_ptr<char> file_name(new char[file_name_len]);


            cout << file_name_len << "->" << world_rank << endl;
            if (file_name_len == -1) {
                cout <<world_rank<< "接收到停止信号" << endl;

                break;
            }

            int source_id = status.MPI_SOURCE;

            //实际所需的进程数，因为如果多次运行，可能已经有图片提取好特征点了。所以实际进程会小于图片总数
//            if (views_size.find(view_id) != views_size.end()) {
            //解包

//                h = views_size[view_id].first;
//                w = views_size[view_id].second;
//                img_size = h * w;
//                cout<<"接收　"<<view_id<<" -> "<<world_rank<<" 的图片数据"<<" 大小为"<<imageGray.size()<<endl;
            //MPI_Recv(&this_thread,imageGray.size(),MPI_INT,ROOT,OMP_THREAD,MPI_COMM_WORLD,status);

//                rec_buff.resize(img_size);
//                memset(rec_buff.data(), 0, img_size * sizeof(unsigned char));

            MPI_Recv(file_name.get(), file_name_len, MPI_UNSIGNED_CHAR, SEND_SERVER, IMAGE, MPI_COMM_WORLD, &status);

            if (!ReadImage(file_name.get(), &imageGray))
                continue;

            image_describer->Describe(imageGray, regions);
            assert(regions != nullptr);
//                cout<<view_id<<" regions info:"<<regions.get()->Type_id()<<endl;
            // auto point_num=regions->RegionCount();
//                cout << "提取　" << view_id << " ->" << world_rank << "的特征点" << endl;
//
            //cout<<"发送　"<<view_id<<" 的特征点数目"<<endl;

            //MPI_Send(&view_id,1,MPI_UNSIGNED,RECV_SERVER,IMAGE_ID,MPI_COMM_WORLD);


            SIFT_Regions *regionsCasted = dynamic_cast<SIFT_Regions *>(regions.get());
            const std::string sFeat = stlplus::create_filespec(sOutDir,
                                                               stlplus::basename_part(file_name.get()), "feat");
            const std::string sDesc = stlplus::create_filespec(sOutDir,
                                                               stlplus::basename_part(file_name.get()), "desc");
            image_describer->Save(regions.get(), sFeat, sDesc);

//                cout<<"发送　"<<view_id<<" 的特征点"<<endl;



            MPI_Send(0, NULL, MPI_UNSIGNED, SEND_SERVER, THREAD_ID, MPI_COMM_WORLD);

            //  image_describer->Save(regions.get(), sFeat, sDesc);
            //打包



//            }

        }


        cout << world_rank << " sift node ended!" << endl;
    }






    MPI_Finalize();
    /*
     * end of mipi patch
     */

    //       std::cout << "Task done in (s): " << timer.elapsed() << std::endl;
    cout << "hello world" << endl;
    return EXIT_SUCCESS;
}
