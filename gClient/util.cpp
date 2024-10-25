#include "util.h"

std::string text::wstring_to_string(const std::wstring& wstr) {
	int bufferSize = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	std::string str(bufferSize, 0);
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], bufferSize, NULL, NULL);
	return str;
}

std::wstring text::string_to_wstring(const std::string& str) {
	int bufferSize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	std::wstring wstr(bufferSize, 0);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], bufferSize);
	return wstr;
}

int file::countFolders(const std::string& path) {
	int folderCount = 0;
	WIN32_FIND_DATA findFileData;

	#ifdef UNICODE
	std::wstring wpath(path.begin(), path.end());
	HANDLE hFind = FindFirstFile((wpath + L"\\*").c_str(), &findFileData);
	#else
	HANDLE hFind = FindFirstFile((path + "\\*").c_str(), &findFileData);
	#endif // UNICODE

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				#ifdef UNICODE
				std::wstring folderName = findFileData.cFileName;
				if (folderName != L"." && folderName != L"..") ++folderCount;
				#else
				std::string folderName = findFileData.cFileName;
				if (folderName != "." && folderName != "..") ++folderCount;
				#endif // UNICODE
			}
		} while (FindNextFile(hFind, &findFileData) != 0);
		FindClose(hFind);
	}
	return folderCount;
}

int file::countFiles(const std::string& path) {
	int fileCount = 0;
	WIN32_FIND_DATA findFileData;

	#ifdef UNICODE
	std::wstring wpath(path.begin(), path.end());
	HANDLE hFind = FindFirstFile((wpath + L"\\*").c_str(), &findFileData);
	#else
	HANDLE hFind = FindFirstFile((path + "\\*").c_str(), &findFileData);
	#endif // UNICODE

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				++fileCount;
			}
		} while (FindNextFile(hFind, &findFileData) != 0);
		FindClose(hFind);
	}
	return fileCount;
}

void cv_::imshowWindow(const std::string& str, const cv::Mat& img) {
	int screenHeight = GetSystemMetrics(SM_CYSCREEN) * 0.85;
	double hscale = static_cast<double>(screenHeight) / img.cols;
	cv::Mat imgShow;
	cv::resize(img, imgShow, cv::Size(), hscale, hscale, cv::INTER_LINEAR);
	cv::imshow(str, imgShow);
}
