//********************************************************//
// CUDA SIFT extractor by Marten Björkman aka Celebrandil //
//              celle @ csc.kth.se                       //
//********************************************************//

#include <iostream>
#include <cmath>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "cudaImage.h"
#include "cudaSift.h"
using namespace std;
int ImproveHomography(SiftData &data, float *homography, int numLoops, float minScore, float maxAmbiguity, float thresh);
vector<pair<int, int> > PrintMatchData(SiftData &siftData1, SiftData &siftData2);
void MatchAll(SiftData &siftData1, SiftData &siftData2, float *homography);

///////////////////////////////////////////////////////////////////////////////
// Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
  int devNum = 0;
  if (argc>1)
    devNum = std::atoi(argv[1]);

  // Read images using OpenCV
  cv::Mat limg, rimg;
  int i, j, id;
  int N = 1;
  //std::string path = "/data/DataSet/DJI/20160520_9/sele/";
  std::string path = "/data/DataSet/mengli/car/test4/";
  SiftData siftData[N];
  id = 0;
  //ofstream ofile((path + "gpurecon/matches/matches.putative.txt").c_str());
  for(i = 0; i < N; i++)
      InitSiftData(siftData[i], 2310000, true, true);
  //std::string list[] = {"0000", "0001", "0002", "0003", "0004", "0005", "0006", "0007", "0008", "0009", "0010", "0011", "0012", "0013", "0014", "0015", "0016", "0017", "0018", "0019", "0020", "0021", "0022", "0023", "0024", "0025", "0026", "0027", "0028", "0029"};
  for(i = 100001; i <= 100229; i++){
    std::string name;
    std::stringstream ss;
    ss.clear();
    ss << i;
    ss >> name;
    name = name.substr(1);
    //name = "IMG_" + name;
  cv::imread(path + name + ".jpg", 0).convertTo(limg, CV_32FC1);
  std::cout << path + name + ".JPG" << std::endl;
  //cv::imread("/home/gaomz/datatest/100_7101.JPG", 0).convertTo(rimg, CV_32FC1);
  unsigned int w = limg.cols;
  unsigned int h = limg.rows;
  std::cout << "Image size = (" << w << "," << h << ")" << std::endl;
  if((w == 0) || (h == 0))
      continue;

  // Perform some initial blurring (if needed)
//  cv::GaussianBlur(limg, limg, cv::Size(5,5), 1.0);
//  cv::GaussianBlur(rimg, rimg, cv::Size(5,5), 1.0);

  // Initial Cuda images and download images to device
  std::cout << "Initializing data..." << std::endl;
  InitCuda(devNum);
  CudaImage img1;//, img2;
  img1.Allocate(w, h, iAlignUp(w, 128), false, NULL, (float*)limg.data);
  //img2.Allocate(w, h, iAlignUp(w, 128), false, NULL, (float*)rimg.data);
  img1.Download();
  //img2.Download();

  // Extract Sift features from images
//  SiftData siftData1, siftData2;
  float initBlur = 0.0f;
  //float thresh = 4.0f;
  float thresh = 0.04f;
  //InitSiftData(siftData2, 50000, true, true);
  //for (int i=0;i<1;i++) {
  ExtractSift(siftData[id % N], img1, 6, initBlur, thresh, 0.0f);
    //ExtractSift(siftData2, img2, 6, initBlur, thresh, 0.0f);
  //}
  //  std::cout << path + "gputest/matches/" + list[i] << std::endl;
  PrintSiftData(siftData[id % N], path + "gpurecon/matches/" + name);
  //id++;
  //FreeSiftData(siftData[id]);

//return 0;
  //for(i = 0; i < 229; i++)
/*
  if(id >= N - 1)
      for(j = 1; j < N; j++)
      {
          int k = id - j, maxm;
          cout << id << " " << k << endl;
          vector<pair<int, int> > match;
          MatchSiftData(siftData[id % N], siftData[k % N]);
          MatchSiftData(siftData[k % N], siftData[id % N]);
          match = PrintMatchData(siftData[id % N], siftData[k % N]);
          //if(j == 1)
          //    maxm = match.size();
          //else if(match.size() * 3 < maxm)
          //    continue;
           ofile << k << " " << id << endl << match.size() << endl;
           for(k = 0; k < match.size(); k++)
               ofile << match[k].first << " " << match[k].second << endl;
           //SiftPoint *sift1 = siftData[i].h_data, *sift2 = siftData[j].h_data;
           //for(k = 0; k < siftData[i].numPts; k++)
           //    if(k == sift2[sift1[k].match].match)
           //        ofile << k << " " << sift1[k].match << endl;
      }
*/
  id++;
  }
  for(i = 0; i < N; i++)
    FreeSiftData(siftData[i]);
  return 0;
  // Match Sift features and find a homography
  /*
  for (int i=0;i<1;i++)
    MatchSiftData(siftData1, siftData2);
  float homography[9];
  int numMatches;
  FindHomography(siftData1, homography, &numMatches, 10000, 0.00f, 0.80f, 5.0);
  int numFit = ImproveHomography(siftData1, homography, 5, 0.00f, 0.80f, 3.0);

  // Print out and store summary data
  PrintMatchData(siftData1, siftData2, img1);
#if 0
  PrintSiftData(siftData1);
  MatchAll(siftData1, siftData2, homography);
#endif
  std::cout << "Number of original features: " <<  siftData1.numPts << " " << siftData2.numPts << std::endl;
  std::cout << "Number of matching features: " << numFit << " " << numMatches << " " << 100.0f*numMatches/std::min(siftData1.numPts, siftData2.numPts) << "%" << std::endl;
  cv::imwrite("data/limg_pts.pgm", limg);

  // Free Sift data from device
  FreeSiftData(siftData1);
  FreeSiftData(siftData2);*/
}

void MatchAll(SiftData &siftData1, SiftData &siftData2, float *homography)
{
#ifdef MANAGEDMEM
  SiftPoint *sift1 = siftData1.m_data;
  SiftPoint *sift2 = siftData2.m_data;
#else
  SiftPoint *sift1 = siftData1.h_data;
  SiftPoint *sift2 = siftData2.h_data;
#endif
  int numPts1 = siftData1.numPts;
  int numPts2 = siftData2.numPts;
  int numFound = 0;
  for (int i=0;i<numPts1;i++) {
    float *data1 = sift1[i].data;
    std::cout << i << ":" << sift1[i].scale << ":" << (int)sift1[i].orientation << std::endl;
    bool found = false;
    for (int j=0;j<numPts2;j++) {
      float *data2 = sift2[j].data;
      float sum = 0.0f;
      for (int k=0;k<128;k++)
	sum += data1[k]*data2[k];
      float den = homography[6]*sift1[i].xpos + homography[7]*sift1[i].ypos + homography[8];
      float dx = (homography[0]*sift1[i].xpos + homography[1]*sift1[i].ypos + homography[2]) / den - sift2[j].xpos;
      float dy = (homography[3]*sift1[i].xpos + homography[4]*sift1[i].ypos + homography[5]) / den - sift2[j].ypos;
      float err = dx*dx + dy*dy;
      if (err<100.0f)
	found = true;
      if (err<100.0f || j==sift1[i].match) {
	if (j==sift1[i].match && err<100.0f)
	  std::cout << " *";
	else if (j==sift1[i].match)
	  std::cout << " -";
	else if (err<100.0f)
	  std::cout << " +";
	else
	  std::cout << "  ";
	std::cout << j << ":" << sum << ":" << (int)sqrt(err) << ":" << sift2[j].scale << ":" << (int)sift2[j].orientation << std::endl;
      }
    }
    std::cout << std::endl;
    if (found)
      numFound++;
  }
  std::cout << "Number of founds: " << numFound << std::endl;
}

vector<pair<int, int> > PrintMatchData(SiftData &siftData1, SiftData &siftData2)
{
  int numPts = siftData1.numPts;
  vector<pair<int, int> > match;
#ifdef MANAGEDMEM
  SiftPoint *sift1 = siftData1.m_data;
  SiftPoint *sift2 = siftData2.m_data;
#else
  SiftPoint *sift1 = siftData1.h_data;
  SiftPoint *sift2 = siftData2.h_data;
#endif
  //std::cout << std::setprecision(3);
  for (int j=0;j<numPts;j++)
      if(sift2[sift1[j].match].match == j)
          match.push_back(make_pair(j, sift1[j].match));
  return match;
//    int k = sift1[j].match;
    /*if (true || sift1[j].match_error<5) {
      float dx = sift2[k].xpos - sift1[j].xpos;
      float dy = sift2[k].ypos - sift1[j].ypos;
#if 1
      if (false && sift1[j].xpos>550 && sift1[j].xpos<600) {
	std::cout << "pos1=(" << (int)sift1[j].xpos << "," << (int)sift1[j].ypos << ") ";
	std::cout << j << ": " << "score=" << sift1[j].score << "  ambiguity=" << sift1[j].ambiguity << "  match=" << k << "  ";
	std::cout << "scale=" << sift1[j].scale << "  ";
	std::cout << "error=" << (int)sift1[j].match_error << "  ";
	std::cout << "orient=" << (int)sift1[j].orientation << "," << (int)sift2[k].orientation << "  ";
	std::cout << " delta=(" << (int)dx << "," << (int)dy << ")" << std::endl;
      }
#endif
#if 1
      int len = (int)(fabs(dx)>fabs(dy) ? fabs(dx) : fabs(dy));
      for (int l=0;l<len;l++) {
	int x = (int)(sift1[j].xpos + dx*l/len);
	int y = (int)(sift1[j].ypos + dy*l/len);
	h_img[y*w+x] = 255.0f;
      }
#endif
    }
#if 1
    int x = (int)(sift1[j].xpos+0.5);
    int y = (int)(sift1[j].ypos+0.5);
    int s = std::min(x, std::min(y, std::min(w-x-2, std::min(h-y-2, (int)(1.41*sift1[j].scale)))));
    int p = y*w + x;
    p += (w+1);
    for (int k=0;k<s;k++)
      h_img[p-k] = h_img[p+k] = h_img[p-k*w] = h_img[p+k*w] = 0.0f;
    p -= (w+1);
    for (int k=0;k<s;k++)
      h_img[p-k] = h_img[p+k] = h_img[p-k*w] =h_img[p+k*w] = 255.0f;
#endif
  }
  std::cout << std::setprecision(6);
  */
}


