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

#include <iterator>
#include <algorithm>
#include <tracker/load_images.h>
#include <boost/regex.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace boost::filesystem;

DirectoryImageLoader::DirectoryImageLoader()
: ImageLoader()
{
}

bool DirectoryImageLoader::initialize(
	const std::string& directory, 
	const std::string& regexp)
{
	std::vector<path> localFileList;

	try {
		if(!exists(directory))
		{
			std::cerr << directory << " does not exist" << std::endl;
			return false;
		}
		if(!is_directory(directory))
		{
			std::cerr << directory << " is not a directory" << std::endl;
			return false;
		}

		path p(directory.c_str());
		boost::smatch what;

		boost::regex my_filter;
		my_filter = boost::regex(regexp);

		for(directory_iterator dirItr = directory_iterator(p); dirItr != directory_iterator(); ++dirItr)
		{
			if(!boost::regex_match(dirItr->path().filename().string(), what, my_filter))
				continue;

			localFileList.push_back(*dirItr);

		}
		//copy(directory_iterator(p), directory_iterator(), back_inserter(localFileList));
	}
	catch (const filesystem_error& ex)
	{
		std::cerr << ex.what() << std::endl;
		return false;
	}

	if(localFileList.size() > 0)
	{
		std::sort(localFileList.begin(), localFileList.end());
		fileList_ = localFileList;
		fileItr_ = fileList_.begin();
	}
	else
	{
		std::cerr << "No files found in " << directory << std::endl;
		return false;
	}

	return true;
}

bool DirectoryImageLoader::getNextFrame(cv::Mat& frame) 
{
	if(fileList_.size() == 0 || fileItr_ == fileList_.end())
		return false;

	std::cout << fileItr_->string() << std::endl;

	frame = cv::imread((fileItr_->string()));
	++fileItr_;

	if(!frame.data)
		return false;
	
	return true;
}

bool DirectoryImageLoader::framesAvailable()
{
	if(fileList_.size() == 0 || fileItr_ == fileList_.end())
		return false;

	return true;
}
