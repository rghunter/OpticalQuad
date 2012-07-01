/*
 * gst-OpticalProcessor.cpp
 *
 *  Created on: Jul 1, 2012
 *      Author: rghunter
 */

#include <opencv2/opencv.hpp>
#include "gstopticalquad.h"

using namespace cv;


void init_frameprocessor(void)
{


}
void process_frame(IplImage *input, IplImage *output)
{
	Mat incoming_frame(input);
	Mat outgoing_frame(output);
	cvtColor(outgoing_frame, incoming_frame, CV_RGB2GRAY);

	//memcpy(output->imageData,input->imageData,output->imageSize);
	return;
}
void cleanup_frameprocessor(void)
{

}
