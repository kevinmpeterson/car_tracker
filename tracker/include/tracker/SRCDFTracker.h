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

/**
	SRCDFTracker implements a Spatially Regularized Correlation Filters for Visual 
	Tracking [1]. This code is based off of the MATLAB implementation by Hager [2]. 


	[1] http://www.cvl.isy.liu.se/research/objrec/visualtracking/regvistrack/SRDCF_ICCV15.pdf
	[2] http://www.cvl.isy.liu.se/research/objrec/visualtracking/regvistrack/SRDCF_code.zip
 */

#ifndef __SRDCF_TRACKER_H__
#define __SRCDF_TRACKER_H__

#include <opencv2/core/core.hpp>
#include <eigen3/Eigen/Dense>

Eigen::MatrixXd hanning(const int& numRows, const int& numCols);

class SRCDFTracker
{
public:
	SRCDFTracker();

	/**
		initializeTrack initializes a filter track from an image and a given region
		
		Inputs are an image and a region within that image that contains the object 
		to track.

		This function doesn't have outputs.
	 */
	void initializeTrack(
		const cv::Mat& image,
		const cv::Rect& roi);

private:
	
	Eigen::MatrixXd weightingWindow_;

};

#endif
