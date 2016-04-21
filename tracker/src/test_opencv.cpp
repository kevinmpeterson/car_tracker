/**
	Copyright (c) 2016 Kevin Peterson

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <tracker/load_images.h>

int main(int argc, char** argv)
{
	DirectoryImageLoader* loader = new DirectoryImageLoader();

	if(argc <= 2)
	{
		std::cerr << "Usage: " << argv[0] << " <input_directory> <regexp>" << std::endl;
		return -1;
	}

	std::string inputDir(argv[1]);
	std::string regexp(argv[2]);

	if(!loader->initialize(inputDir, regexp))
	{
		std::cerr << "Error loading " << inputDir << ". Exiting." << std::endl;
		return -1;
	}

	cv::Mat currentFrame;
	cv::Mat prevFrame;

	cv::Mat currentFrameGray;
	cv::Mat prevFrameGray;

	cv::Mat displayImage;

	cv::Mat flow, flowMag, flowAngle;
	std::vector<cv::Mat> flowChannels;

	std::vector<cv::Point2f> trackedPoints[2];
	cv::Size subPixWinSize(10,10), winSize(31,31);
	int numberOfTrackedPoints = 1000;
	cv::TermCriteria termcrit(cv::TermCriteria::COUNT|cv::TermCriteria::EPS,20,0.03);

	cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("flow", cv::WINDOW_AUTOSIZE);

	if(loader->framesAvailable())
	{
		if(!loader->getNextFrame(prevFrame))
		{
			std::cout << "Could not get first frame" << std::endl;
			return -1;
		}
		cv::cvtColor(prevFrame, prevFrameGray, cv::COLOR_BGR2GRAY);
		cv::goodFeaturesToTrack(prevFrameGray, trackedPoints[0], numberOfTrackedPoints, 0.01, 10, cv::Mat(), 3, 0, 0.04);
		cv::cornerSubPix(prevFrameGray, trackedPoints[0], subPixWinSize, cv::Size(-1,-1), termcrit);
	}

	while(loader->framesAvailable())
	{
		if(!loader->getNextFrame(currentFrame))
		{
			std::cerr << "Error reading current frame" << std::endl;
			continue;
		}

		if(!trackedPoints[0].empty())
		{
			cv::cvtColor(currentFrame, currentFrameGray, cv::COLOR_BGR2GRAY);

			std::vector<uchar> status;
	        std::vector<float> err;
			cv::calcOpticalFlowPyrLK(
				prevFrameGray, currentFrameGray, trackedPoints[0], trackedPoints[1],
				status, err,
				winSize, 3, termcrit, 0, 0.001);

			currentFrame.copyTo(displayImage);
			for(int ptI = 0; ptI < trackedPoints[1].size(); ++ptI)
			{
				if(!status[ptI])
					continue;

				cv::circle(displayImage, trackedPoints[1][ptI], 3, cv::Scalar(0, 255, 0), -1, 8);
			}

			trackedPoints[0] = trackedPoints[1];

			cv::namedWindow("flow", cv::WINDOW_AUTOSIZE);
			cv::imshow("flow", displayImage);
		}



#if 0
		cv::calcOpticalFlowFarneback(prevFrameGray, currentFrameGray, flow, 0.125, 5, 5, 3, 5, 1.2, 0);

		cv::split(flow, flowChannels);
		cv::cartToPolar(flowChannels[0], flowChannels[1], flowMag, flowAngle);

		//cv::normalize(flowMag, flowMag, 0, 2000, cv::NORM_MINMAX);

		cv::namedWindow("flow", cv::WINDOW_AUTOSIZE);
		cv::imshow("flow", flowMag*10000);
#endif

		cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
		cv::imshow("image", currentFrame);

		cv::waitKey(1);

		currentFrame.copyTo(prevFrame);
		currentFrameGray.copyTo(prevFrameGray);
	}

	std::cout << "Hello World" << std::endl;
	return 0;
}
