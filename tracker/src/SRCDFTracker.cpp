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
#include <tracker/FeatureUtilities.h>
#include <tracker/ImageUtilities.h>

#include <iostream>

using namespace cv;
using namespace Eigen;

SRDCFParams::SRDCFParams()
{
	searchAreaScale = 2.0;
	cellSize = 1.0;
	labelSigma = 0.5; //1 / 16.0;
}

SRDCFTracker::SRDCFTracker()
{
	FeatureParams featureParams;
	featureParams.cellSize = params_.cellSize;

	ColorFeatureProcessor* cfp = new ColorFeatureProcessor(featureParams);
	featureProcessors_.push_back(cfp);
}

SRDCFTracker::~SRDCFTracker()
{
	for(int fI = 0; fI < featureProcessors_.size(); ++fI)
	{
		delete featureProcessors_[fI];
	}
}

void partitionFFT(const Mat& ftResponse, Mat& symmetric)
{
	Mat ret;
}

/**
  Convenience function to pack fft into real and imaginary planes
 */
Mat computeFFT(const Mat& input)
{
	Mat planes[] = {Mat_<float>(input), Mat::zeros(input.size(), CV_32F)};
	Mat complexI;
    merge(planes, 2, complexI);
    dft(complexI, complexI);

    return complexI;
}

int circularShift(const int& input, const int& shiftPoint, const int& maxVal)
{
	int ret;
	if(input < shiftPoint)
		ret = input;
	else
		ret = input - maxVal;

	return ret;
}

Mat SRDCFTracker::computeFeatureMatrix(const Mat& templ)
{
	//TODO: extend this beyond just the first feature set
	Mat features;
	featureProcessors_[0]->computeFeatureVector(templ, features);

	Mat windowedFeatures = features.mul(weightingWindow_);
	Mat featuresFT;

	featuresFT = computeFFT(windowedFeatures);
	//dft(windowedFeatures, featuresFT);

	return featuresFT;
}

Mat computeLabelMatrix(
	const double& labelSigma,
	const Size& outputSize)
{
	Mat labels;		// pixel space representation of the labels
	Mat labelsFT; 	// fourier domain representation of the labels

	labels = Mat::zeros(outputSize.height, outputSize.width, CV_32F);
	double scaledSigmaRSq = pow(labelSigma * outputSize.height, 2);
	double scaledSigmaCSq = pow(labelSigma * outputSize.width, 2);

	for(int col = 0; col < outputSize.width; ++col)
	{
		int colShifted = circularShift(col, outputSize.width/2, outputSize.width);
		double colShiftedScaled = (double)(colShifted * colShifted) / scaledSigmaCSq;
		for(int row = 0; row < outputSize.height; ++row)
		{
			int rowShifted = circularShift(row, outputSize.height/2, outputSize.height);
			double rowShiftedScaled = (double)(rowShifted * rowShifted) / scaledSigmaRSq;

			labels.at<float>(row, col) = 
				exp(-0.5 * (colShiftedScaled + rowShiftedScaled));
		}
	}

	//dft(labels, labelsFT);
	labelsFT = computeFFT(labels);

	return labelsFT;
}

/**
  unpackFFT unpacks the fft used by openCV into real and imaginary parts
  see http://docs.opencv.org/2.4/modules/core/doc/operations_on_arrays.html#dft
*/
void unpackFFT(const Mat& input, Mat& real, Mat& imag)
{
	std::cout << input.size() << std::endl;
	std::cout << input.channels() << std::endl;
	std::vector<Mat> unpackedFFT;
	split(input, unpackedFFT);

	Size sz = unpackedFFT[0].size();
	int numEntries = sz.width * sz.height;

	real = unpackedFFT[0].reshape(1, numEntries);
	imag = unpackedFFT[1].reshape(1, numEntries);

	std::cout << real.size() << "\t" << imag.size().width << "\t" << imag.size().height << std::endl;
}

SparseMat computeSparseFeatureMatrix(const Mat& featuresFT)
{
	// First compute the autocorrelation of the feature vector
	Mat autoCorr;
	mulSpectrums(featuresFT, featuresFT, autoCorr, 0, true);

	// Then build the sparse representation of the feature set 
	// (see Danelljan 2015 eq (8-10)
	const int dims[2] = {1, 1};
	SparseMat sparseFeatureMatrix = SparseMat(2, dims, CV_32FC1);
	Mat real, imag;

	unpackFFT(autoCorr, real, imag);
	// TODO: for next features, add block size

	// Build the diagonal matrix representing B*D(x)
	// This is a block diagonal matrix with one block per feture set
	// NOTE: currently only implemented for a single feature
	// 
	// Each row holds a two-entry transform of the complex data from the feature 
	// FFT. The per-row transform is a Re[i] - Im[i] and Re[i] + Im[i];
	int blockWidth = (real.size()).width * (real.size()).height;
	for(int diagI = 0; diagI < blockWidth; ++diagI)
	{
		int idxR1[] = {diagI, 				diagI};
		int idxR2[] = {diagI + blockWidth, 	diagI + blockWidth};
		int idxI1[] = {diagI,				diagI + blockWidth};
		int idxI2[] = {diagI + blockWidth, 	diagI};

		sparseFeatureMatrix.ref<float>(idxR1) =  real.at<float>(diagI, 1);
		sparseFeatureMatrix.ref<float>(idxR2) =  real.at<float>(diagI, 1);
		sparseFeatureMatrix.ref<float>(idxI1) = -imag.at<float>(diagI, 1);
		sparseFeatureMatrix.ref<float>(idxI2) =  imag.at<float>(diagI, 1);
	}

	return sparseFeatureMatrix;
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

	Rect initialTemplateROI = Rect(
		templateCenterX_ - initialTemplateSize_.width / 2,
		templateCenterY_ - initialTemplateSize_.height / 2,
		initialTemplateSize_.width,
		initialTemplateSize_.height);

	weightingWindow_ = hanning(
		initialTemplateSize_.height, 
		initialTemplateSize_.width);

	Mat templ = extractPatch(initialTemplateROI, image);
	Mat target = extractPatch(targetROI, image);

	// generate feature matrix
	Mat featuresFT = computeFeatureMatrix(templ);

	// generate label matrix
	Mat labelsFT = computeLabelMatrix(params_.labelSigma, initialTemplateSize_);
	SparseMat sparseFeatureMatrix = computeSparseFeatureMatrix(featuresFT);
	

	cv::namedWindow("template", cv::WINDOW_AUTOSIZE);
	cv::imshow("template", templ);

	cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
	cv::imshow("image", image);

	Mat real, imag;
	unpackFFT(featuresFT, real, imag);

	waitKey();

	std::cout << initialTemplateSize_ << std::endl;
}
