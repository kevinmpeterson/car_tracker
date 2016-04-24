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
**/

#include <opencv2/highgui/highgui.hpp>

#include <tracker/SRCDFTracker.h>
#include <iostream>

using namespace cv;
using namespace Eigen;


VectorXd hanning1d(const int& numPoints)
{
	VectorXd result = VectorXd::Zero(numPoints);

	for(int wI = 0; wI < numPoints; ++wI)
	{
		result(wI) = 0.5 * (1 - cos(2 * M_PI * (wI / (double)numPoints)));
	}

	return result;
}

MatrixXd hanning(const int& numRows, const int& numCols)
{
	MatrixXd result;

	MatrixXd hannCol =  hanning1d(numRows);
	MatrixXd hannRow = (hanning1d(numCols)).transpose();

	result = hannCol * hannRow;

	return result;
}

SRDCFParams::SRDCFParams()
{
	searchAreaScale = 2.0;
	cellSize = 1.0;
}

SRDCFTracker::SRDCFTracker()
{
}


Rect cropROI(const Rect& roi, const Size& imSize)
{
	Rect outROI = roi;

	if(roi.x < 0)
	{
		outROI.x = 0;
		outROI.width = roi.width + roi.x;
	}

	if(roi.y < 0)
	{
		outROI.y = 0;
		outROI.height = roi.height + roi.y;
	}

	if(outROI.x + outROI.width > imSize.width)
	{
		outROI.width = imSize.width - outROI.x;
	}

	if(outROI.y + outROI.height > imSize.height)
	{
		outROI.height = imSize.height - outROI.y;
	}

	return outROI;
}


void SRDCFTracker::initializeTrack(
	const Mat& image,
	const Rect& targetROI)
{
	templateScaleFactor_ = 1.0;

	templateCenterX_ = targetROI.x + targetROI.width / 2;
	templateCenterY_ = targetROI.y + targetROI.height / 2;

	std::cout << targetROI << std::endl;
	std::cout << templateCenterX_ << ", " << templateCenterY_ << std::endl;
	double searchAreaFactor = params_.searchAreaScale;

	initialTemplateSize_ = cv::Size(
		targetROI.width * searchAreaFactor,
		targetROI.height * searchAreaFactor);

	templateSize_ = cv::Size(
		initialTemplateSize_.width * templateScaleFactor_, 
		initialTemplateSize_.height * templateScaleFactor_);

	weightingWindow_ = hanning(
		initialTemplateSize_.height, 
		initialTemplateSize_.width);

	Rect initialTemplateROI = Rect(
		templateCenterX_ - initialTemplateSize_.width / 2,
		templateCenterY_ - initialTemplateSize_.height / 2,
		initialTemplateSize_.width,
		initialTemplateSize_.height);

	Rect initTemplROICropped = cropROI(initialTemplateROI, image.size());

	std::cout << initialTemplateROI << std::endl;
	std::cout << initTemplROICropped << std::endl;
	std::cout << targetROI << std::endl;

	Mat templ = image(initTemplROICropped);
	Mat target = image(targetROI);

	cv::namedWindow("template", cv::WINDOW_AUTOSIZE);
	cv::imshow("template", templ);

	cv::namedWindow("target", cv::WINDOW_AUTOSIZE);
	cv::imshow("target", target);

	cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
	cv::imshow("image", image);

	cv::namedWindow("hanning", cv::WINDOW_AUTOSIZE);
	cv::imshow("hanning", templ);

	waitKey();
	/*
	templateROI = Rect(targetROI.x, targetROI.y);
	*/
	/*
	Mat trackBox = image(roi);
	Mat pixels = image(templateROI);
	*/
	std::cout << initialTemplateSize_ << std::endl;
}
