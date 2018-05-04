#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

int n_boards = 0; //Will be set by input list
const int board_dt = 10;
int board_w;
int board_h;

void help(){
	printf("Calibration from disk. Call convention:\n\n"
		"  ch11_ex11_1_fromdisk board_w board_h image_list\n\n"
		"Where: board_{w,h} are the # of internal corners in the checkerboard\n"
		"       width (board_w) and height (board_h)\n"
		"       image_list is space separated list of path/filename of checkerboard\n"
		"       images\n\n"
		"Hit 'p' to pause/unpause, ESC to quit.  After calibration, press any other key to step through the images\n\n");
}

int main(int argc, char * argv[]) {

	char *chess_file = argv[1];
	char *image_file = argv[2];
	char *output_path = argv[3];
	for (int i = 0; i < argc; ++i)
		cout << argv[i] << endl;
	board_w = 7;//12;
	board_h = 7;//12;
	int board_n  = board_w * board_h;
	CvSize board_sz = cvSize( board_w, board_h );
	FILE *fptr = fopen(chess_file,"r");
	char names[2048];
	//COUNT THE NUMBER OF IMAGES:
	while(fscanf(fptr,"%s ",names)==1){
		n_boards++;
	}
	rewind(fptr);

	//cvNamedWindow( "Calibration", 0);
	//cvResizeWindow("Calibration", 960, 540);

	//ALLOCATE STORAGE
	CvMat* image_points      = cvCreateMat(n_boards*board_n,2,CV_32FC1);
	CvMat* object_points     = cvCreateMat(n_boards*board_n,3,CV_32FC1);
	CvMat* point_counts      = cvCreateMat(n_boards,1,CV_32SC1);
	CvMat* rotation  = cvCreateMat(3,1,CV_32FC1);
	CvMat* translation_vector  = cvCreateMat(3,1,CV_32FC1);
	CvMat* rotation_mat_final  = cvCreateMat(3,3,CV_32FC1);
	CvMat* intrinsic_matrix  = cvCreateMat(3,3,CV_32FC1);
	CvMat* distortion_coeffs = cvCreateMat(4,1,CV_32FC1);


	IplImage* image = 0;// = cvQueryFrame( capture );
	IplImage* gray_image = 0; //for subpixel
	CvPoint2D32f* corners = new CvPoint2D32f[ board_n ];
	int corner_count;
	int successes = 0;
	int step;

	for( int frame=0; frame<n_boards; frame++ ) {
		fscanf(fptr,"%s ",names);

		if(image){
			cvReleaseImage(&image);
			image = 0;
		}
		image = cvLoadImage( names);
		if(gray_image == 0  && image) //We'll need this for subpixel accurate stuff
			gray_image = cvCreateImage(cvGetSize(image),8,1);

		if(!image)
			printf("null image\n");

		int imageW = image->width;
		int imageH = image->height;
		int smallOne = std::min(imageH,imageW);
		int winSize = smallOne*(0.01);
		int found = cvFindChessboardCorners(
			image,
			board_sz,
			corners,
			&corner_count, 
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS
			);

		//Get Subpixel accuracy on those corners

		cvCvtColor(image, gray_image, CV_BGR2GRAY);
		cvFindCornerSubPix(gray_image, corners, corner_count, 
			cvSize(winSize,winSize),cvSize(-1,-1), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));  // win size is 11 * 11 origenal
		//Draw it

		//cvDrawChessboardCorners(image, board_sz, corners, corner_count, found);
		//cvShowImage( "Calibration", image );

		// If we got a good board, add it to our data
		//
		if( corner_count == board_n ) {
			step = successes*board_n;
			//	printf("Found = %d for %s\n",found,names);
			for( int i=step, j=0; j<board_n; ++i,++j ) {
				///         CV_MAT_ELEM(*image_points, CvPoint2D32f,0,i) = cvPoint2D32f(corners[j].x,corners[j].y);
				///         CV_MAT_ELEM(*object_points,CvPoint3D32f,0,i) = cvPoint3D32f(j/board_w, j%board_w, 0.0f);
				CV_MAT_ELEM(*image_points, float,i,0) = corners[j].x;
				CV_MAT_ELEM(*image_points, float,i,1) = corners[j].y;
				CV_MAT_ELEM(*object_points,float,i,0) = j/board_w;
				CV_MAT_ELEM(*object_points,float,i,1) = j%board_w;
				CV_MAT_ELEM(*object_points,float,i,2) = 0.0f;

			}
			//        CV_MAT_ELEM(*point_counts, int,0,successes) = board_n;
			CV_MAT_ELEM(*point_counts, int,successes,0) = board_n;		
			successes++;
		}

		//    if( successes == n_boards ) break;

		int c = cvWaitKey(15);
		if(c == 'p') {
			c = 0;
			while(c != 'p' && c != 27){
				c = cvWaitKey(250);
			}
		}
		if(c == 27)
			return 0;
	}
	printf("successes = %d, n_boards=%d\n",successes,n_boards);
	//ALLOCATE MATRICES ACCORDING TO HOW MANY IMAGES WE FOUND CHESSBOARDS ON
	///  CvMat* image_points2      = cvCreateMat(1,successes*board_n,CV_32FC2);
	///  CvMat* object_points2     = cvCreateMat(1,successes*board_n,CV_32FC3);
	///  CvMat* point_counts2      = cvCreateMat(1,successes,CV_32SC1);
	CvMat* object_points2     = cvCreateMat(successes*board_n,3,CV_32FC1);
	CvMat* image_points2      = cvCreateMat(successes*board_n,2,CV_32FC1);
	CvMat* point_counts2      = cvCreateMat(successes,1,CV_32SC1);
	//TRANSFER THE POINTS INTO THE CORRECT SIZE MATRICES
	for(int i = 0; i<successes*board_n; ++i){
		///      CV_MAT_ELEM(*image_points2, CvPoint2D32f,0,i)  = CV_MAT_ELEM(*image_points, CvPoint2D32f,0,i);
		///      CV_MAT_ELEM(*object_points2,CvPoint3D32f,0,i)  = CV_MAT_ELEM(*object_points,CvPoint3D32f,0,i);
		CV_MAT_ELEM(*image_points2, float,i,0) 	=	CV_MAT_ELEM(*image_points, float,i,0);
		CV_MAT_ELEM(*image_points2, float,i,1) 	= 	CV_MAT_ELEM(*image_points, float,i,1);
		CV_MAT_ELEM(*object_points2,float,i,0) = CV_MAT_ELEM(*object_points,float,i,0) ;
		CV_MAT_ELEM(*object_points2,float,i,1) = CV_MAT_ELEM(*object_points,float,i,1) ;
		CV_MAT_ELEM(*object_points2,float,i,2) = CV_MAT_ELEM(*object_points,float,i,2) ;

	} 
	for(int i=0; i<successes; ++i){
		///		CV_MAT_ELEM(*point_counts2,int,0, i) = CV_MAT_ELEM(*point_counts, int,0,i);
		CV_MAT_ELEM(*point_counts2,int,i, 0) = CV_MAT_ELEM(*point_counts, int,i,0);
	}
	cvReleaseMat(&object_points);
	cvReleaseMat(&image_points);
	cvReleaseMat(&point_counts);

	// cvWaitKey();//Now we have to reallocate the matrices
	// return 0;
	// At this point we have all of the chessboard corners we need.
	//

	// Initialize the intrinsic matrix such that the two focal
	// lengths have a ratio of 1.0
	//
	CV_MAT_ELEM( *intrinsic_matrix, float, 0, 0 ) = 1.0f;
	CV_MAT_ELEM( *intrinsic_matrix, float, 1, 1 ) = 1.0f;
	printf("cvCalibrateCamera2\n");
	cvCalibrateCamera2(
		object_points2,
		image_points2,
		point_counts2,
		cvGetSize( image ),
		intrinsic_matrix,
		distortion_coeffs,
		NULL,
		NULL,
		0//CV_CALIB_FIX_ASPECT_RATIO
		);




	cvFindExtrinsicCameraParams2(
		object_points2, image_points2,
		intrinsic_matrix,distortion_coeffs,
		rotation,translation_vector
		);


	// Save our work

	cvRodrigues2(translation_vector,rotation_mat_final);
	cvSave("rotation.xml",rotation_mat_final);
	cvSave("translation.xml",translation_vector);
	cvSave("Intrinsics.xml",intrinsic_matrix);
	cvSave("Distortion.xml",distortion_coeffs);
	// Load test
	CvMat *intrinsic = (CvMat*)cvLoad("Intrinsics.xml");
	CvMat *distortion = (CvMat*)cvLoad("Distortion.xml");

	// Build the undistort map which we will use for all 
	// subsequent frames.
	//
	IplImage* mapx = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );
	IplImage* mapy = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );
	printf("cvInitUndistortMap\n");
	cvInitUndistortMap(
		intrinsic,
		distortion,
		mapx,
		mapy
		);
	// Just run the camera to the screen, now only showing the undistorted
	// image.
	//
	rewind(fptr);
	//cvNamedWindow( "Undistort", 0 );
	//cvResizeWindow("Undistort", 960, 540);
	printf("\n\nPress any key to step through the images, ESC to quit\n\n");
	fclose(fptr);
	fptr = fopen(image_file,"r");
	while(fscanf(fptr,"%s ",names)==1){
		if(image){
			cvReleaseImage(&image);
			image = 0;
		}  
		std::string str(names);
		cout << "load " << str << endl;
		image = cvLoadImage(str.c_str());
		IplImage *t = cvCloneImage(image);
		//cvResizeWindow("Calibration", 960, 540);
		//cvShowImage( "Calibration", image );

		cvRemap( t, image, mapx, mapy );
		cvReleaseImage(&t);
		//cvResizeWindow("Undistort", 960, 540);
		//cvShowImage("Undistort", image);
		std::string outputname(output_path);
		outputname += str.substr(str.find_last_of('/'));
		cout << "save as " << outputname << endl;
		//outputname.insert(outputname.size()-4, "_u");
		cvSaveImage(outputname.c_str(),image);
		//if((cvWaitKey()&0x7F) == 27) break;  
	}

	return 0;
} 
