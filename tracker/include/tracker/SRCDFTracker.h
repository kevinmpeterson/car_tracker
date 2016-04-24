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
	SRDCFTracker implements a Spatially Regularized Correlation Filters for Visual 
	Tracking [1]. This code is based off of the MATLAB implementation by Hager [2]. 


	[1] http://www.cvl.isy.liu.se/research/objrec/visualtracking/regvistrack/SRDCF_ICCV15.pdf
	[2] http://www.cvl.isy.liu.se/research/objrec/visualtracking/regvistrack/SRDCF_code.zip
 */

#ifndef __SRDCF_TRACKER_H__
#define __SRDCF_TRACKER_H__

#include <vector>
#include <opencv2/core/core.hpp>
#include <eigen3/Eigen/Dense>
#include <tracker/FeatureUtilities.h>

struct SRDCFParams
{
	double searchAreaScale;
	double cellSize;

	SRDCFParams();
};

class SRDCFTracker
{
public:
	SRDCFTracker();
	~SRDCFTracker();

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

	Eigen::MatrixXd weightingWindow_;	// cosine weighting window for regularization
	SRDCFParams params_;		// parameters for the filter

	cv::Size initialTemplateSize_; 	// Size of the target window at initialization
	cv::Size templateSize_;  		// Current size of the template
	double templateScaleFactor_; 	// Multiplier to determine size of the search region

	double templateCenterX_;
	double templateCenterY_;


	std::vector< FeatureProcessor* > featureProcessors_;
};

#endif
