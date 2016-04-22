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

#ifndef __FEATURE_UTILITIES_H__
#define __FEATURE_UTILITIES_H__

#include <opencv2/core/core.hpp>

struct FeatureParams
{
	double cellSize;
};

class FeatureProcessor
{
public:
	FeatureProcessor();

	virtual void computeFeatureVector(const cv::Mat& patch, cv::Mat& featureVector) const = 0;
	inline bool getValidForColor() const;

private:
	FeatureProcessor(bool isValidForColor_);
	bool isValidForColor_;
};

class HOGFeatureProcessor : public FeatureProcessor
{
public:
	HOGFeatureProcessor();
	void computeFeatureVector(const cv::Mat& patch, cv::Mat& featureVector) const;
};

class ColorFeatureProcessor : public FeatureProcessor
{
public:
	ColorFeatureProcessor(
		const FeatureParams& params);
	void computeFeatureVector(const cv::Mat& patch, cv::Mat& featureVector) const;

private:
	FeatureParams params_;
};

#endif
