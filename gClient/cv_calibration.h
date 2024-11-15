#pragma once
#include "pch.h"
enum dir {
	EAST,
	WEST,
	SOUTH,
	NORTH,
	END
};

class cv_calibration
{
public:
	cv_calibration();
	~cv_calibration();

private:

public:
	cv::Rect roiRect, roiRectDuplication;
	cv::Scalar cvRed{ 0, 0, 255 }, cvGreen{ 0, 255, 0 }, cvBlue{ 255, 0, 0 };

public:
	void MAIN_ImageCalibration();
	void TEST_cv_calibration();
	
	// 크로스검색
	cv::Point2f computeIntersection(const cv::Point2i& clockwise1, const cv::Point2i& clockwise2, const cv::Point2i& clockwise3, const cv::Point2i& clockwise4); 
	int getNearbyPoint(std::vector<std::pair<cv::Point2i, bool/*duplication*/>> points , cv::Point2i point, int tolerance);
	bool searchCrossPoint(cv::Mat& roiImage, cv::Point2i& retPoint);
	void sortVecPoint(std::vector<cv::Point2i>& points, std::vector<std::vector<cv::Point2i>>& usingSquared_points = std::vector<std::vector<cv::Point2i>>());

	// 체스판재구성
	int getLengthX(const std::vector<int>& lengthDir) { lengthDir[EAST] + lengthDir[WEST]; }
	int getLengthY(const std::vector<int>& lengthDir) { lengthDir[SOUTH] + lengthDir[NORTH]; }
	bool isAllTrue(const std::vector<bool>& vBool) { return std::all_of(vBool.begin(), vBool.end(), [](bool ret) { return ret; }); }
	bool pioneerOnestep(const std::vector<cv::Point2i>& points, int& stdPointIdx, const int& stdScale, const int& nWidth, const int& roiScale, const dir& direction);
	void makeCheckboard(cv::Mat& grayImageAbstracted, cv::Size& chessboardCornersizes);

	// 메인
	void makeImageCalibration(const std::vector<cv::Mat>& images, 
							  std::vector<std::vector<cv::Point2f>>& imagesCorners,
							  cv::Mat& cameraMatrix, 
							  cv::Mat& distortionCoefficient, 
							  std::vector<cv::Mat>& rotationVectors, 
							  std::vector<cv::Mat>& translationVectors);
	void undistortImage(const cv::Mat& srcImage, cv::Mat& dstImage,
						const cv::Mat& cameraMatrix,
						const cv::Mat& distortionCoefficient);
	void undistortPoint(const std::vector<cv::Point2f>& srcImagesPoints, std::vector<cv::Point2f>& dstImagesPoints, 
					  const cv::Mat& cameraMatrix, 
					  const cv::Mat& distortionCoefficient);
	void transformPoint(const std::vector<cv::Point2f>& srcImagesPoints, std::vector<cv::Point2f>& dstImagesPoints, 
						const cv::Mat& cameraMatrix, 
						const std::vector<cv::Mat>& rotationVectors, 
						const std::vector<cv::Mat>& translationVectors);
};