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
#include <tracker/ImageUtilities.h>

using namespace cv;
using namespace Eigen;

Mat hanning1d(const int& numPoints)
{
	Mat result = Mat::zeros(numPoints, 1, CV_32F);

	for(int wI = 0; wI < numPoints; ++wI)
	{
		result.at<float>(wI, 1) = 0.5 * (1 - cos(2 * M_PI * (wI / (double)numPoints)));
	}

	return result;
}

Mat hanning(const int& numRows, const int& numCols)
{
	Mat result;

	Mat hannCol =  hanning1d(numRows);
	Mat hannRow = (hanning1d(numCols)).t();

	result = hannCol * hannRow;
	
	return result;
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

Mat extractPatch(const Rect& roi, const Mat& image)
{
	Mat output = Mat::zeros(roi.height, roi.width, image.type());

	Rect croppedROI = cropROI(roi, image.size());
	Rect outputROI(
		croppedROI.x - roi.x, croppedROI.y - roi.y, 
		croppedROI.width, croppedROI.height);

	Mat croppedOutputPatch = output(outputROI);
	Mat croppedInputPatch = image(croppedROI);
	croppedInputPatch.copyTo(croppedOutputPatch);

	return output;
}