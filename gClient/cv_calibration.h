#pragma once
#include "pch.h"

class cv_calibration
{
public:
	cv_calibration() {}
	~cv_calibration() {}

private:

public:
	void distort(cv::Mat src, cv::Mat dst, cv::Mat cameraMatrix, cv::Mat distortionCoefficient);
	void transform(std::vector<cv::Point2f> obj3D, 
				   cv::Mat rotationVector, 
				   cv::Mat translationVector, 
				   cv::Mat cameraMatrix, 
				   cv::Mat distortionCoefficient);
};