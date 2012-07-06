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
Mat outgoing_frame, frame_gray;
Mat corners[2];
int corner_count;
TermCriteria subPixCriteria;
Size subPixWindowSize;


void init_frameprocessor(int width, int height)
{
	frame_buffer.clear();
	corner_count = MAX_CORNERS;
	subPixWindowSize = Size(WINDOW_SIZE,WINDOW_SIZE);

	subPixCriteria.epsilon = 0.3;
	subPixCriteria.maxCount = 20;
	subPixCriteria.type = (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS);

	frame_gray.create(width,height,CV_8UC1);

	cout << "Called init_frameprocessor function" << endl;
}
void process_frame(IplImage *input, IplImage *output)
{

	Mat incoming_frame(input);
	Mat status, errors;

	//Size video_frame_size = incoming_frame.size();




	cvtColor(incoming_frame,frame_gray, CV_RGB2GRAY);

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
#if 0
#if 0
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
	line(incoming_frame,start,Point(video_frame_size.width/2+(x*vel*10.0),video_frame_size.height/2+(y*vel*10.0)),Scalar(255,0,0),3);
#endif
#endif

	*output = incoming_frame;

	//memcpy(output->imageData,input->imageData,output->imageSize);
	return;
}
void cleanup_frameprocessor(void)
{

}
