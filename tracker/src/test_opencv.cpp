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
#include <eigen3/Eigen/Dense>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/eigen.hpp>

#include <tracker/load_images.h>
#include <tracker/SRCDFTracker.h>

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
	cv::namedWindow("image", cv::WINDOW_AUTOSIZE);

	cv::Rect roi(0, 0, 39, 63);
	SRCDFTracker tracker;
	if(loader->framesAvailable())
	{
		if(!loader->getNextFrame(currentFrame))
		{
			std::cerr << "Could not get first frame for initialization. Exiting." << std::endl;
			return -1;
		}

		tracker.initializeTrack(currentFrame, roi);
	}

	while(loader->framesAvailable())
	{
		if(!loader->getNextFrame(currentFrame))
		{
			std::cerr << "Error reading current frame" << std::endl;
			continue;
		}

		if(!currentFrame.data)
			std::cout << "No data..." << std::endl;

		cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
		cv::imshow("image", currentFrame);
		cv::waitKey(10);
	}

	std::cout << "Hello World" << std::endl;
	return 0;
}
