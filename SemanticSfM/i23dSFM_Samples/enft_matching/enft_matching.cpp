
// Copyright (c) 2017 pandagmz

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "i23dSFM/image/image.hpp"
#include "i23dSFM/image/image_warping.hpp"
#include "i23dSFM/features/features.hpp"
#include "i23dSFM/matching/regions_matcher.hpp"
#include "i23dSFM/multiview/solver_homography_kernel.hpp"
#include "i23dSFM/multiview/solver_fundamental_kernel.hpp"
#include "i23dSFM/multiview/conditioning.hpp"
#include "i23dSFM/robust_estimation/robust_estimator_ACRansac.hpp"
#include "i23dSFM/robust_estimation/robust_estimator_ACRansacKernelAdaptator.hpp"
#include "i23dSFM/sfm/sfm_data.hpp"
#include "i23dSFM/sfm/sfm_data_io.hpp"
#include "i23dSFM/sfm/pipelines/sfm_engine.hpp"
#include "i23dSFM/sfm/pipelines/sfm_features_provider.hpp"
#include "i23dSFM/sfm/pipelines/sfm_regions_provider.hpp"

#include "nonFree/sift/SIFT_describer.hpp"

#include "third_party/stlplus3/filesystemSimplified/file_system.hpp"
#include "third_party/cmdLine/cmdLine.h"

#include <string>
#include <iostream>
#include <set>
#include <unordered_set>

using namespace i23dSFM;
using namespace i23dSFM::image;
using namespace i23dSFM::matching;
using namespace i23dSFM::robust;
using namespace std;
using namespace i23dSFM::features;
using namespace i23dSFM::sfm;

std::unique_ptr<Image_describer> image_describer(new SIFT_Image_describer(SiftParams(-1)));
std::map<IndexT, std::unique_ptr<features::Regions> > regions_perImage;
std::map<IndexT, Image<unsigned char> > image_list;
/*
const string data_folder = "/windows/DataSet/CAB/";//"/windows/DataSet/test4/";
int N = 312;
int NAMEBASE = 10435;
const string prefix = "P10";
const string suffix = ".JPG";
*/
std::vector<IndMatch> EnftMatch(IndexT ja, IndexT jb, set<IndMatch> &matchIdx)
{
  //std::cout << "Pair " << ja << " and " << jb << " start matching : " << std::endl;
  Image<unsigned char> imageL, imageR;
  imageL = image_list[ja];
  imageR = image_list[jb];
  const SIFT_Regions* regionsL = dynamic_cast<SIFT_Regions*>(regions_perImage.at(ja).get());
  const SIFT_Regions* regionsR = dynamic_cast<SIFT_Regions*>(regions_perImage.at(jb).get());

  const PointFeatures
    featsL = regions_perImage.at(ja)->GetRegionsPositions(),
    featsR = regions_perImage.at(jb)->GetRegionsPositions();

  std::vector<IndMatch> vec_PutativeMatches, bak_PutativeMatches;
  //-- Perform matching -> find Nearest neighbor, filtered with Distance ratio
    // Find corresponding points
  matching::DistanceRatioMatch(
    0.8, matching::BRUTE_FORCE_L2,
    *regions_perImage.at(ja).get(),
    *regions_perImage.at(jb).get(),
    vec_PutativeMatches);

  //-- Fundamental robust estimation
  Mat F_xL(2, vec_PutativeMatches.size());
  Mat F_xR(2, vec_PutativeMatches.size());

  for (size_t k = 0; k < vec_PutativeMatches.size(); ++k)  {
    const PointFeature & imaL = featsL[vec_PutativeMatches[k]._i];
    const PointFeature & imaR = featsR[vec_PutativeMatches[k]._j];
    F_xL.col(k) = imaL.coords().cast<double>();
    F_xR.col(k) = imaR.coords().cast<double>();
  }
  std::vector<size_t> F_vec_inliers;
  typedef ACKernelAdaptor<
    i23dSFM::fundamental::kernel::SevenPointSolver,
    i23dSFM::fundamental::kernel::SymmetricEpipolarDistanceError,
    UnnormalizerT,
    Mat3>
    F_KernelType;

  F_KernelType F_kernel(
    F_xL, imageL.Width(), imageL.Height(),
    F_xR, imageR.Width(), imageR.Height(),
    true); // configure as point to line error model.

  Mat3 F;
  std::pair<double,double> F_ACRansacOut = ACRANSAC(F_kernel, F_vec_inliers, 1024, &F,
    Square(4.0), // Upper bound of authorized threshold
    false);
  if(F_vec_inliers.size() <= F_KernelType::MINIMUM_SAMPLES *2.5)
  {
    //std::cout << "No fundamental matrix found!" << std::endl;
    return bak_PutativeMatches;
  }

  int times = 0;
  std::vector<Mat3> vec_H;//multi H matrix
  std::vector<Image<unsigned char> > vec_WarpImage;
  std::vector<IndMatch> vec_EnftMatches;
  bak_PutativeMatches = vec_PutativeMatches;//all matches by enft method
  vec_PutativeMatches.clear();
  for(size_t i : F_vec_inliers)
    vec_PutativeMatches.push_back(bak_PutativeMatches[i]);
  bak_PutativeMatches = vec_PutativeMatches;
  //for ( size_t i = 0; i < F_vec_inliers.size(); ++i)  {
  //  vec_EnftMatches.push_back(vec_PutativeMatches[F_vec_inliers[i]]);
  //}

  while(times++ < 6)//(vec_PutativeMatches.size() > 0)
  // Homography geometry filtering of putative matches
  {
  	//std::cout << "Putative matches size : " << vec_PutativeMatches.size() << std::endl;
	  unordered_set<size_t> unstOutliers;
	  for(size_t i = 0; i < vec_PutativeMatches.size(); i++)
		  unstOutliers.insert(i);
    //A. get back interest point and send it to the robust estimation framework
    Mat xL(2, vec_PutativeMatches.size());
    Mat xR(2, vec_PutativeMatches.size());

    for (size_t k = 0; k < vec_PutativeMatches.size(); ++k)  {
      const PointFeature & imaL = featsL[vec_PutativeMatches[k]._i];
      const PointFeature & imaR = featsR[vec_PutativeMatches[k]._j];
      xL.col(k) = imaL.coords().cast<double>();
      xR.col(k) = imaR.coords().cast<double>();
    }

    //-- Homography robust estimation
    std::vector<size_t> vec_inliers;
    typedef ACKernelAdaptor<
      i23dSFM::homography::kernel::FourPointSolver,
      i23dSFM::homography::kernel::AsymmetricError,
      UnnormalizerI,
      Mat3>
      KernelType;

    KernelType kernel(
      xL, imageL.Width(), imageL.Height(),
      xR, imageR.Width(), imageR.Height(),
      false); // configure as point to point error model.

    Mat3 H;
    std::pair<double,double> ACRansacOut = ACRANSAC(kernel, vec_inliers, 1024, &H,
      std::numeric_limits<double>::infinity(),
      false);
    const double & thresholdH = ACRansacOut.first;

    // Check the homography support some point to be considered as valid
    if (vec_inliers.size() > KernelType::MINIMUM_SAMPLES *2.5) {

      //std::cout << "\nFound a homography under the confidence threshold of: "
      //  << thresholdH << " pixels\n\twith: " << vec_inliers.size() << " inliers"
      //  << " from: " << vec_PutativeMatches.size()
      //  << " putatives correspondences"
      //  << std::endl;

      //Show homography validated point and compute residuals
      std::vector<double> vec_residuals(vec_inliers.size(), 0.0);
      for ( size_t i = 0; i < vec_inliers.size(); ++i)  {
	  	  unstOutliers.erase(vec_inliers[i]);
        const SIOPointFeature & LL = regionsL->Features()[vec_PutativeMatches[vec_inliers[i]]._i];
        const SIOPointFeature & RR = regionsR->Features()[vec_PutativeMatches[vec_inliers[i]]._j];
        const Vec2f L = LL.coords();
        const Vec2f R = RR.coords();
        // residual computation
        vec_residuals[i] = std::sqrt(KernelType::ErrorT::Error(H,
                                       LL.coords().cast<double>(),
                                       RR.coords().cast<double>()));
        vec_EnftMatches.push_back(vec_PutativeMatches[vec_inliers[i]]);
        //double lx = L[0], ly = L[1], rx = R[0], ry = R[1];
        //image::ApplyH_AndCheckOrientation(H, lx, ly);
        //std::cout << lx << " " << ly << " "
        //          << rx << " " << ry << std::endl;
      }
      // Display some statistics of reprojection errors
      float dMin, dMax, dMean, dMedian;
      minMaxMeanMedian<float>(vec_residuals.begin(), vec_residuals.end(),
                            dMin, dMax, dMean, dMedian);

      //std::cout << std::endl
      //  << "Homography matrix estimation, residuals statistics:" << "\n"
      //  << "\t-- Residual min:\t" << dMin << std::endl
      //  << "\t-- Residual median:\t" << dMedian << std::endl
      //  << "\t-- Residual max:\t "  << dMax << std::endl
      //  << "\t-- Residual mean:\t " << dMean << std::endl
		  //  << H << std::endl;
      vec_H.push_back(H);
      Image<unsigned char> wImage(imageL.Width(), imageL.Height());
      image::Warp(imageR, H, wImage);
      vec_WarpImage.push_back(wImage);
      //WriteImage("unwarped.png", imageL);
      //WriteImage(("warped" + std::to_string(times) + ".png").c_str(), wImage);

      std::vector<IndMatch> vec_NextMatches;
      for(size_t i : unstOutliers)
      {
        vec_NextMatches.push_back(vec_PutativeMatches[i]);
      }
      vec_PutativeMatches = vec_NextMatches;
    }
    else  {
      //std::cout << "ACRANSAC was unable to estimate a rigid homography"
      //  << std::endl;
      break;
    }
  }
  
  //vec_EnftMatches.clear();
  //vec_EnftMatches.insert(vec_EnftMatches.end(), F_vec_inliers.begin(), F_vec_inliers.end());
  //std::cout << "init enft matches count : " << vec_EnftMatches.size() << std::endl;
//store the features that have been matched upon
  unordered_set<IndexT> matchedL, matchedR;
  for(auto m : vec_EnftMatches)
  {
    matchedL.insert(m._i);
    matchedR.insert(m._j);
  }
  //for every feature in L image, find the best match in R image by enft
  for(IndexT j = 0; j < regionsR->Descriptors().size(); ++j)
  {
    if(matchedR.find(j) != matchedR.end())
      continue;
    //int dis_Hx = 4, dis_Fx = 2;//the range to find match
    //int dim = 128;//the dimension of descriptor
    int matchedIndex = -1;//L's index matched with R's j
    //double minDescDist = 128 * 256 * 256;//min sift descriptor distance
    int W = 5, lWl;
    double le = 1 / 40.0, lh = 1 / 1000.0;
    double E, Ei, Ee, Eh;//E for energy, in the eq(1) : E = Ei + |W| * le * Ee + |W| * lh * Eh;
    //std::cout << j << std::endl;
    for(IndexT i = 0; i < regionsL->Descriptors().size(); ++i)
    {
      if(matchedL.find(i) != matchedL.end())
        continue;
      for(IndexT k = 0; k < vec_H.size(); ++k)
      {
        Mat3 &H = vec_H[k];
        Image<unsigned char> &wimageR = vec_WarpImage[k];
        double lx = regionsL->Features()[i].x(),
               ly = regionsL->Features()[i].y(),
               rx = regionsR->Features()[j].x(),
               ry = regionsR->Features()[j].y();
               
        image::ApplyH_AndCheckOrientation(H, lx, ly);
        //if(!wimageR.Contains(ly, lx))
        //  continue;

        //calc energy h
        Eh = (lx - rx) * (lx - rx)
           + (ly - ry) * (ly - ry);
        if(Eh > 15 * 15)
        {
          //std::cout << Eh << "over point distance" << std::endl;
          continue;
        }
        //std::cout << Eh << " point passed" << std::endl;
        //calc energy e
        Ee = i23dSFM::fundamental::kernel::SymmetricEpipolarDistanceError::Error(
          F, 
          regionsL->Features()[i].coords().cast<double>(), 
          regionsR->Features()[j].coords().cast<double>()
          );
        Ee *= Ee;
        if(Ee > 5 * 5)
        {
          //std::cout << Ee << "over line distance" << std::endl;
          continue;
        }
        //std::cout << Ee << " line passed" << std::endl;
        //calc energy i
        Ei = 0;
        lWl = 11 * 11;
        for(int dx = -W; dx <= W; ++dx)
          for(int dy = -W; dy <= W; ++dy)
          {
            if(!imageL.Contains(ly + dy, lx + dx) || !wimageR.Contains(ry + dy, rx + dx))
            {
              --lWl;
              continue;
            }
            Ei += (imageL(ly + dy, lx + dx) - wimageR(ry + dy, rx + dx)) / 256.0
                * (imageL(ly + dy, lx + dx) - wimageR(ry + dy, rx + dx)) / 256.0;
          }
        
        //find the minimum energy E
        if((-1 == matchedIndex) || (E > Ei + lWl * le * Ee + lWl * lh * Eh))
        {
          //std::cout << "|W| : " << lWl << std::endl;
          E = Ei + lWl * le * Ee + lWl * lh * Eh;
          matchedIndex = i;
        }
        /*std::cout << "Debug Mode :"
                  << " E : " << E
                  << " Ei : " << Ei
                  << " Ee : " << Ee
                  << " Eh : " << Eh
                  << std::endl;*/
      }
    }
    if(matchedIndex != -1)
    {
      vec_EnftMatches.push_back(IndMatch(matchedIndex, j));
      //std::cout << "ENFT matches : " << matchedIndex << " " << j << std::endl;
    }
  }
  //std::cout << "calc enft matches count : " << vec_EnftMatches.size() << std::endl;
  //fmatch << ja << " " << jb << " " << std::endl
  //       << vec_EnftMatches.size() << std::endl;
  assert(matchIdx.empty());
  for(IndexT k = 0; k < vec_EnftMatches.size(); ++k)
  {
    //fmatch << vec_EnftMatches[k]._i << " " << vec_EnftMatches[k]._j << std::endl;
    matchIdx.insert(vec_EnftMatches[k]);
  }
  return bak_PutativeMatches;
}

int main(int argc, char ** argv) {

  CmdLine cmd;
  int nNear = 0;
  std::string sSfM_Data_Filename;
  std::string sOutDir = "";
  bool bForce = false;
  string sPairList = "";

  cmd.add( make_option('i', sSfM_Data_Filename, "input_file") );
  cmd.add( make_option('o', sOutDir, "outdir") );
  cmd.add( make_option('n', nNear, "number_of_near") );
  cmd.add( make_option('f', bForce, "force") );
  cmd.add( make_option('p', sPairList, "pair_list"));

  try{
    cmd.process(argc, argv);
  }
  catch(const std::string &s)
  {
    std::cerr << s << std::endl;
    return EXIT_FAILURE;
  }

  SfM_Data sfm_data;
  if (!Load(sfm_data, sSfM_Data_Filename, ESfM_Data(VIEWS|INTRINSICS))) {
    std::cerr << std::endl
      << "The input file \""<< sSfM_Data_Filename << "\" cannot be read" << std::endl;
    return false;
  }
/*  const string jpg_file = data_folder + prefix;
	const string mat_file = data_folder + "enft/matches/" + prefix;
  assert(system(("mkdir " + data_folder + "enft").c_str()) >= 0);
  assert(system(("mkdir " + data_folder + "enft/matches").c_str()) >= 0);
	
  
  int imgCnt = 0;
  for(IndexT j = 0; j < N; j++)
  {
    Image<unsigned char> image;
    string s;
    stringstream ss;
    ss << j + NAMEBASE;
    ss >> s;
    //s = s.substr(1);
    if(0 == ReadImage((jpg_file + s + suffix).c_str(), &image))
    {
      //std::cout << "doesn't exist!" << std::endl;
      continue;
    }
    std::cout << "Read image : " << jpg_file + s + suffix << std::endl;
    image_list[imgCnt] = image;
    image_describer->Describe(image, regions_perImage[imgCnt]);
    image_describer->Save(regions_perImage[imgCnt].get(), mat_file + s + ".feat", mat_file + s + ".desc");
    ++imgCnt;
  }
  */
    Image<unsigned char> imageGray;
    C_Progress_display my_progress_bar( sfm_data.GetViews().size(),
      std::cout, "\n- EXTRACT FEATURES -\n" );
    for(Views::const_iterator iterViews = sfm_data.views.begin();
        iterViews != sfm_data.views.end();
        ++iterViews, ++my_progress_bar)
    {
      const View * view = iterViews->second.get();
      //std::cout << view->id_view << std::endl;
      const std::string sView_filename = stlplus::create_filespec(sfm_data.s_root_path,
        view->s_Img_path);
      const std::string sFeat = stlplus::create_filespec(sOutDir,
        stlplus::basename_part(sView_filename), "feat");
      const std::string sDesc = stlplus::create_filespec(sOutDir,
        stlplus::basename_part(sView_filename), "desc");
      if (!ReadImage(sView_filename.c_str(), &imageGray))
        continue;
      image_list[view->id_view] = imageGray;
      if (bForce || !stlplus::file_exists(sFeat) || !stlplus::file_exists(sDesc))
      {
        image_describer->Describe(imageGray, regions_perImage[view->id_view]);
        image_describer->Save(regions_perImage[view->id_view].get(), sFeat, sDesc);
      }
      else
      {
        regions_perImage[view->id_view].reset(new features::SIFT_Regions());
        image_describer->Load(regions_perImage[view->id_view].get(), sFeat, sDesc);
      }
    }
  //Image<RGBColor> image;
  //const string jpg_filenameL = "/cdata/DataSet/DJI/beida/high/DJI_0154.JPG";//stlplus::folder_up(string(THIS_SOURCE_DIR))
  //    + "/imageData/StanfordMobileVisualSearch/Ace_0.png";
  //const string jpg_filenameR = "/cdata/DataSet/DJI/beida/high/DJI_0155.JPG";//stlplus::folder_up(string(THIS_SOURCE_DIR))
  //    + "/imageData/StanfordMobileVisualSearch/Ace_1.png";
  //assert(image_list.size() == imgCnt);
  vector<pair<int, int> > imgPair;
  if(sPairList == "")
  {
    if(nNear != 0)
    {
      for(IndexT ja = 0; ja < image_list.size(); ++ja)
        for(IndexT jb = ja + 1; jb < ja + nNear; ++jb)
          imgPair.push_back(make_pair(ja, jb % image_list.size()));
    }
    else
    {
      for(IndexT ja = 0; ja + 1 < image_list.size(); ++ja)
        for(IndexT jb = ja + 1; jb < image_list.size(); ++jb)
          imgPair.push_back(make_pair(ja, jb));
    }
  }
  else
  {
    ifstream ifPair(sPairList);
    IndexT ja, jb;
    while(ifPair >> ja >> jb)
      imgPair.push_back(make_pair(ja, jb));
  }
  std::cout << "Total pairs : " << imgPair.size() << std::endl;
  IndexT iCurPair = 0;

  fstream fmatch;
  if(!stlplus::file_exists(sOutDir + "/matches.putative.txt"))
    fmatch.open(stlplus::create_filespec(sOutDir, "matches.putative.txt"), std::fstream::out);
  else
  {
    set<pair<int, int> > usPair;
    for(auto p : imgPair)
      usPair.insert(p);
    fmatch.open(stlplus::create_filespec(sOutDir, "matches.putative.txt"), std::fstream::in);
    int ia, ib;
    while(fmatch >> ia >> ib)
    {
      int iPair, numPair, x, y;
      usPair.erase(make_pair(ia, ib));
      fmatch >> numPair;
      for(iPair = 0; iPair < numPair; ++iPair)
        fmatch >> x >> y;
    }
    fmatch.close();
    imgPair.clear();
    for(auto p : usPair)
      imgPair.push_back(p);
    fmatch.open(stlplus::create_filespec(sOutDir, "matches.putative.txt"), std::fstream::app);
  }
  
  #pragma omp parallel for
  for(int iter = 0; iter < imgPair.size(); iter++)      
  {
    IndexT ja = imgPair[iter].first, jb = imgPair[iter].second;
    set<IndMatch> matchFore, matchBack;
    vector<IndMatch> retab, retba;
    retab = EnftMatch(ja, jb, matchFore);
    retba = EnftMatch(jb, ja, matchBack);
    if((retab.size() == 0) || (retba.size() == 0))
      continue;
    vector<IndMatch> matchIdx;
    for(IndMatch idm : matchFore)
    {
      IndMatch mdi;
      mdi._i = idm._j;
      mdi._j = idm._i;
      if(matchBack.find(mdi) != matchBack.end())
        matchIdx.push_back(idm);
    }
    if(matchIdx.size() < retab.size())
      matchIdx = retab;
    if(matchIdx.size() < retba.size())
    {
      for(IndMatch idm : retba)
        idm = idm.swap();
      matchIdx = retba;
    }
    #pragma omp critical
    {
      //std::cout << "calc " << ja << " " << jb << " forward F " << retab << " backward F " << retba << std::endl
      //          << "merge " << ja << " " << jb << " forward and backward, remained matches count : " << matchIdx.size() << std::endl;
      std::cout << "[" << ++iCurPair << "/" << imgPair.size() << "] "
                << ja << " and " << jb << " have : " << matchIdx.size()
                << " matches from ab : " << retab.size() << " matches, ba " << retba.size() << " matches." << std::endl;
      fmatch << ja << " " << jb << " " << std::endl
            << matchIdx.size() << std::endl;
      for(IndMatch idm : matchIdx)
      {
        fmatch << idm << std::endl;
      }
    }
  }
  fmatch.close();
  return 0;
}

