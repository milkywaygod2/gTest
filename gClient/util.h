#pragma once
#include "pch.h"

namespace text {
	std::string wstring_to_string(const std::wstring& wstr);
	std::wstring string_to_wstring(const std::string& str);
}

namespace file {
	int countFolders(const std::string& path);
	int countFiles(const std::string& path);
}

namespace cv_ {
	void imshowWindow(const std::string& str, const cv::Mat& img);
}