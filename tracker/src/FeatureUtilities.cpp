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

#include <tracker/FeatureUtilities.h>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

FeatureProcessor::FeatureProcessor()
	: isValidForColor_(false)
{
}

FeatureProcessor::FeatureProcessor(bool isValidForColor)
	: isValidForColor_(isValidForColor_)
{
}

inline bool FeatureProcessor::getValidForColor() const
{
	return isValidForColor_;
}

ColorFeatureProcessor::ColorFeatureProcessor(
	const FeatureParams& params)
	: params_(params), FeatureProcessor()
{
}

void ColorFeatureProcessor::computeFeatureVector(
	const Mat& patch, 
	Mat& featureOutput) const
{
	// TODO: error check for 2 channels?
	// TODO: ensure zero to one scaling?
	if(patch.channels() == 3)
	{
		cvtColor(patch, featureOutput, CV_BGR2GRAY);
	}
	else
	{
		patch.copyTo(featureOutput);
	}

	featureOutput.convertTo(featureOutput, CV_32F);
	featureOutput = featureOutput / 255.0;

	// TODO: Change this to integral image? (see average_feature_region.m)
	resize(featureOutput, featureOutput, Size(0,0), 1./params_.cellSize, 1./params_.cellSize);
}