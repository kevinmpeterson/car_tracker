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

#ifndef __LOAD_IMAGES_H__
#define __LOAD_IMAGES_H__

#include <boost/filesystem.hpp>
#include <opencv2/core/core.hpp>

/**
	This contains image loading routines for performing basic i/o. The goal is 
	to abstract frame loading so that the input source is transparent to the 
	caller following initialization.
*/

class ImageLoader {
public:
	virtual bool getNextFrame(cv::Mat& frame) = 0;
	virtual bool framesAvailable() = 0;
};

class DirectoryImageLoader : public ImageLoader {
public:
	DirectoryImageLoader();

	/*
		Initialize the loader with a list of files based on the directory and 
		the regular expression. The regexp specifies which files to include 
		and exclude. 
		
		directory specifies the input directory where the files are stored
		regexp specifies a standard regular expression used to filter the files 
			in the folder

		Returns true if the directory exists and the regexp matches 
		at least one file. Otherwise returns false.
	 */
	bool initialize(const std::string& directory, const std::string& regexp = ".*");
	bool getNextFrame(cv::Mat& frame);
	bool framesAvailable();

private:
	std::vector<boost::filesystem::path> fileList_;
	std::vector<boost::filesystem::path>::const_iterator fileItr_;
};

#endif
