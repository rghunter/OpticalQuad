/*
 * gst-OpticalProcessor.cpp
 *
 *  Created on: Jul 1, 2012
 *      Author: rghunter
 */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include "gstopticalquad.h"
#include <iostream>

#define WINDOW_SIZE 10
#define MAX_CORNERS 100
#define HISTORY 2
using namespace std;
using namespace cv;

deque<Mat> frame_buffer;
bool unDistort, first;
Mat outgoing_frame, frame_gray, save_image, fixed_image;
Mat frame_corrected;
Mat fixed_descriptor;
Mat cameraMatrix, distCoeffs;

vector<KeyPoint> keypoint_fixed;


gboolean init_frameprocessor(int width, int height)
{

	frame_gray.create(width,height,CV_8UC1);
	frame_corrected.create(width,height,CV_8UC3);
	first = true;

	FileStorage fs("calib.xml",FileStorage::READ);
	if(!fs.isOpened())
	{
		cout << "Could not open calibration file." << endl;
		unDistort = false;
		return FALSE;
	}else{
		fs["Camera_Matrix"] >> cameraMatrix;
		fs["Distortion_Coefficients"] >> distCoeffs;
		cout << "Camera Calibration Detected!" << endl;
		cout << "Camera Matrix: " << cameraMatrix << endl;
		cout << "Distortion Coefficients: " << distCoeffs << endl;
		fs.release();
		unDistort = true;
	}
	cout << "Called init_frameprocessor function" << endl;

	return TRUE;
}
void process_frame(IplImage *input, IplImage *output)
{
	SurfFeatureDetector detector(1000,8);
	FREAK extractor;
	Mat raw_frame(input);
	BruteForceMatcher <Hamming> matcher;
	vector<KeyPoint> keypoint;
	Mat descriptor;
	Mat output_frame;
	vector<DMatch> matches;

	if(first)
	{
		fixed_image = raw_frame.clone();
		detector.detect(raw_frame,keypoint_fixed);
		extractor.compute(raw_frame,keypoint_fixed,fixed_descriptor);
		first = false;
		return;
	}

	detector.detect(raw_frame,keypoint);
	extractor.compute(raw_frame,keypoint,descriptor);
	matcher.match(fixed_descriptor,descriptor,matches);
	//drawMatches(raw_frame,keypoint_fixed,raw_frame,keypoint,matches,output_frame);
	//imshow("output",output_frame);
	drawKeypoints(raw_frame,keypoint,raw_frame,Scalar(255,0,0));
	drawKeypoints(raw_frame,keypoint_fixed,raw_frame,Scalar(0,255,0),DrawMatchesFlags::DRAW_OVER_OUTIMG);

	*output = raw_frame;

	//memcpy(output->imageData,input->imageData,output->imageSize);
	return;
}
void cleanup_frameprocessor(void)
{

}
