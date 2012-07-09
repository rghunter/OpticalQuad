/*
 * gst-OpticalProcessor.cpp
 *
 *  Created on: Jul 1, 2012
 *      Author: rghunter
 */

#include <opencv2/opencv.hpp>
#include "gstopticalquad.h"
#include <iostream>

#define WINDOW_SIZE 10
#define MAX_CORNERS 100
#define HISTORY 2
using namespace std;
using namespace cv;

deque<Mat> frame_buffer;
bool unDistort;
Mat outgoing_frame, frame_gray, save_image;
Mat frame_corrected;
Mat cameraMatrix, distCoeffs;
Mat corners[2];
int corner_count;
TermCriteria subPixCriteria;
Size subPixWindowSize;


gboolean init_frameprocessor(int width, int height)
{
	frame_buffer.clear();
	corner_count = MAX_CORNERS;
	subPixWindowSize = Size(WINDOW_SIZE,WINDOW_SIZE);

	subPixCriteria.epsilon = 0.3;
	subPixCriteria.maxCount = 20;
	subPixCriteria.type = (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS);

	frame_gray.create(width,height,CV_8UC1);
	frame_corrected.create(width,height,CV_8UC3);

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

	Mat raw_frame(input);

	Mat status, errors;

	if(unDistort)
	{
		undistort(raw_frame,frame_corrected,cameraMatrix,distCoeffs);
	}else{
		raw_frame.copyTo(frame_corrected);
	}

	Size video_frame_size = raw_frame.size();

	cvtColor(frame_corrected,frame_gray, CV_RGB2GRAY);

	frame_buffer.push_front(frame_gray.clone());

	if((int)frame_buffer.size() > HISTORY){
		frame_buffer.pop_back();
	}else{
		return;
	}

	goodFeaturesToTrack(frame_buffer[0],corners[0],corner_count,
						0.01, //quality
						10.0); //min seperation

	cornerSubPix(frame_buffer[0],corners[0],subPixWindowSize,Size(-1,-1),subPixCriteria);



	calcOpticalFlowPyrLK(frame_buffer[0],frame_buffer[1],corners[0],corners[1],status, errors);

#if 1
	Mat vectors = corners[0] - corners[1];
	float vel = 0;
	float x = 0;
	float y = 0;
	int len = 0;

	for(MatConstIterator_<Point2f> velocity_vector = vectors.begin<Point2f>(); velocity_vector != vectors.end<Point2f>(); ++velocity_vector)
	{
		vel += sqrt(pow((*velocity_vector).x,2)+pow((*velocity_vector).y,2));
		x += (*velocity_vector).x / vel;
		y += (*velocity_vector).y / vel;
		len++;
	}

	vel = vel / (float)len;
	y = y / (float)len;
	x = x / (float)len;

	Point start(video_frame_size.width/2,video_frame_size.height/2);
	line(frame_corrected,start,Point(video_frame_size.width/2+(x*vel*10.0),video_frame_size.height/2+(y*vel*10.0)),Scalar(255,0,0),3);

//	cout << y << x << endl;
#endif

	*output = frame_corrected;

	//memcpy(output->imageData,input->imageData,output->imageSize);
	return;
}
void cleanup_frameprocessor(void)
{

}
