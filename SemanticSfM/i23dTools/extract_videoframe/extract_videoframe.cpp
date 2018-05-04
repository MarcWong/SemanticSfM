#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/ocl/ocl.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <vector>	
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;
using namespace cv::ocl;

bool createDetectorDescriptorMatcher( const string& detectorType, const string& descriptorType, const string& matcherType,	
	Ptr<FeatureDetector>& featureDetector,	
	Ptr<DescriptorExtractor>& descriptorExtractor,	
	Ptr<DescriptorMatcher>& descriptorMatcher )	
{	
	cout << "< Creating feature detector, descriptor extractor and descriptor matcher ..." << endl;	
	if (detectorType=="SIFT"||detectorType=="SURF")	
		initModule_nonfree();	
	featureDetector = FeatureDetector::create( detectorType );	
	descriptorExtractor = DescriptorExtractor::create( descriptorType );	
	descriptorMatcher = DescriptorMatcher::create( matcherType );	
	cout << ">" << endl;	
	bool isCreated = !( featureDetector.empty() || descriptorExtractor.empty() || descriptorMatcher.empty() );	
	if( !isCreated )	
		cout << "Can not create feature detector or descriptor extractor or descriptor matcher of given types." << endl << ">" << endl;	
	return isCreated;	
}	


bool refineMatchesWithHomography(const std::vector<cv::KeyPoint>& queryKeypoints,		
	const std::vector<cv::KeyPoint>& trainKeypoints,		 
	float reprojectionThreshold,		
	std::vector<cv::DMatch>& matches,		
	cv::Mat& homography	)	
{	
	const int minNumberMatchesAllowed = 4;		
	if (matches.size() < minNumberMatchesAllowed)		
		return false;		
	// Prepare data for cv::findHomography		
	std::vector<cv::Point2f> queryPoints(matches.size());		
	std::vector<cv::Point2f> trainPoints(matches.size());		
	for (size_t i = 0; i < matches.size(); i++)		
	{		
		queryPoints[i] = queryKeypoints[matches[i].queryIdx].pt;		
		trainPoints[i] = trainKeypoints[matches[i].trainIdx].pt;		
	}		
	// Find homography matrix and get inliers mask		
	std::vector<unsigned char> inliersMask(matches.size());		
	homography = cv::findHomography(queryPoints,		 
		trainPoints,		 
		CV_FM_RANSAC,		 
		reprojectionThreshold,		 
		inliersMask);		
	std::vector<cv::DMatch> inliers;		
	for (size_t i=0; i<inliersMask.size(); i++)		
	{		
		if (inliersMask[i])		
			inliers.push_back(matches[i]);		
	}		
	matches.swap(inliers);	
/*	Mat homoShow;	
	drawMatches(src,queryKeypoints,frameImg,trainKeypoints,matches,homoShow,Scalar::all(-1),CV_RGB(255,255,255),Mat(),2);			 
	imshow("homoShow",homoShow);*/	 
	return matches.size() > minNumberMatchesAllowed;	 

}	


bool matchingDescriptor(const vector<KeyPoint>& queryKeyPoints,const vector<KeyPoint>& trainKeyPoints,	
	const Mat& queryDescriptors,const Mat& trainDescriptors,	 
	Ptr<DescriptorMatcher>& descriptorMatcher,	
	Mat& homo, bool enableRatioTest = true)	
{	
	vector< vector<DMatch> > m_knnMatches;	
	vector<DMatch>m_Matches;	

	if (enableRatioTest)	
	{	
		cout<<"KNN Matching"<<endl;	
		const float minRatio = 1.f / 1.5f;	
		descriptorMatcher->knnMatch(queryDescriptors,trainDescriptors,m_knnMatches,2);	
		for (size_t i=0; i<m_knnMatches.size(); i++)	
		{	
			const cv::DMatch& bestMatch = m_knnMatches[i][0];	
			const cv::DMatch& betterMatch = m_knnMatches[i][1];	
			float distanceRatio = bestMatch.distance / betterMatch.distance;	
			if (distanceRatio < minRatio)	
			{	
				m_Matches.push_back(bestMatch);	
			}	
		}	

	}	
	else	
	{	
		cout<<"Cross-Check"<<endl;	
		Ptr<cv::DescriptorMatcher> BFMatcher(new cv::BFMatcher(cv::NORM_HAMMING, true));	
		BFMatcher->match(queryDescriptors,trainDescriptors, m_Matches );	
	}	
	//Mat homo;	
	float homographyReprojectionThreshold = 1.0;	
	bool homographyFound = refineMatchesWithHomography(	
		queryKeyPoints,trainKeyPoints,homographyReprojectionThreshold,m_Matches,homo);	

	if (!homographyFound)	
	{
		//fout << "No homo" << endl;
		return false;	
	}
	else	
	{	
		if (m_Matches.size()>10)
		{
			return true;	
		}
		return false;
	}	


}	
long findBestFrame(string filename, VideoCapture &capture, long frameToStart)	
{	
	Mat src, frameImg;	
	ofstream fout;
	int width;	
	int height;	
	bool ifFound;
	vector<Point> srcCorner(4);	
	vector<Point> dstCorner(4);	
	src = imread(filename,0);	
	width = src.cols;	
	height = src.rows;	
	string detectorType = "SIFT";	
	string descriptorType = "SIFT";	
	string matcherType = "FlannBased";	

	Ptr<FeatureDetector> featureDetector;	
	Ptr<DescriptorExtractor> descriptorExtractor;	
	Ptr<DescriptorMatcher> descriptorMatcher;	
	if( !createDetectorDescriptorMatcher( detectorType, descriptorType, matcherType, featureDetector, descriptorExtractor, descriptorMatcher ) )	
	{	
		cout<<"Creat Detector Descriptor Matcher False!"<<endl;	
		return -1;	
	}	
	//Intial: read the pattern img keyPoint	
	vector<KeyPoint> queryKeypoints;	
	Mat queryDescriptor;	
	featureDetector->detect(src,queryKeypoints);	
	descriptorExtractor->compute(src,queryKeypoints,queryDescriptor);	
	cout << "number of features : " << queryKeypoints.size() << endl;

	vector<KeyPoint> trainKeypoints;	
	Mat trainDescriptor;	

	Mat frame,grayFrame;
	long currentFrame;
	currentFrame = frameToStart;
	ifFound = false;
        int lastFeatureNumber = 0;
        int dupFrame = 0;
	
	while(!ifFound)
	{
		currentFrame += 1;
		cout << "currentFrame : " << currentFrame << endl;
		capture.set(CV_CAP_PROP_POS_FRAMES, currentFrame);
		capture.read(frame);	
		if (frame.empty())
			return -1;
		else
		{
			frame.copyTo(frameImg);
			grayFrame.zeros(frame.rows,frame.cols,CV_8UC1);
			cvtColor(frame,grayFrame,CV_BGR2GRAY);	
			trainKeypoints.clear();	
			trainDescriptor.setTo(0);	
			featureDetector->detect(grayFrame,trainKeypoints);	
			cout << "number of features : " << trainKeypoints.size() << endl;
			if(trainKeypoints.size() == lastFeatureNumber)
		              dupFrame += 1;
			else
		        {
				lastFeatureNumber = trainKeypoints.size();
				dupFrame = 0;
			}	
			if(dupFrame >= 60)
			{
				cout << "The media end is broken!" << endl;
				exit(0);
			}

			
			//清晰度阈值
			//if(trainKeypoints.size() < 2000)
			//	continue;
			
			if(trainKeypoints.size()!=0)	
			{	
				descriptorExtractor->compute(grayFrame,trainKeypoints,trainDescriptor);	
				Mat homo;
				bool isFound = matchingDescriptor(queryKeypoints,trainKeypoints,queryDescriptor,trainDescriptor,descriptorMatcher,homo);	
				Mat_<double> homo_ = homo;
				if(isFound)
				{
					std::vector<Point2f> srcPoint(1), dstPoint(1);
					int inPoints = 0;
					for(int c = 0; c < src.cols; c++)
						for(int r = 0; r < src.rows; r++)
						{
							srcPoint[0] = cvPoint(c, r);
							perspectiveTransform(srcPoint, dstPoint, homo);
							if(dstPoint[0].x >= 0 && dstPoint[0].y >= 0 && dstPoint[0].x <= src.cols && dstPoint[0].y <= src.rows)
								++inPoints;
						}
					double overlapRate = (double)inPoints / (src.cols * src.rows);
					cout << "Overlap area:" << overlapRate << endl;

					//judge frame homo && overlap rate
					cout << "current homo : " << fabs(homo_[2][1]) << endl;
					if(fabs(homo_[2][1]) > 1e-4 && overlapRate <= 0.618)
					{
						ifFound = true;
					}
				}
				else
				{
					cout << "current homo doesn't exist" << endl;
					ifFound = true;
				}
			}	
		}
	}
	return currentFrame;
}
int main(int argc, char *argv[])
{
	if(argc < 4)
		return -1;
	string moviePath, outPath;
	moviePath = argv[1];
	outPath = argv[2];
	//打开视频文件：其实就是建立一个VideoCapture结构
	VideoCapture capture(moviePath);//"D:\\pandagmz\\VideoFrame\\DJI02082.MP4");
	//检测是否正常打开:成功打开时，isOpened返回ture
	if(!capture.isOpened())
	{
		cout<<"fail to open!"<<endl;
		return -1;
	}
	long nMainFrame = atoi(argv[3]);
	int i;
	Mat frame;
	for(i = 0; i < 600; i++)
	{
		cout << "MainFrame : " << nMainFrame << endl;
		capture.set(CV_CAP_PROP_POS_FRAMES, nMainFrame);
		capture.read(frame);
		string path = outPath;//"D:\\pandagmz\\VideoFrame\\";
		stringstream ss;
		string str;
		ss << nMainFrame;
		ss >> str;
		path = path + str + ".jpg";
		imwrite(path,frame);
		nMainFrame = findBestFrame(path, capture, nMainFrame);
		if(nMainFrame < 0)
			break;
	}
	capture.release();
	return 0;
}
