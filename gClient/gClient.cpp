#pragma once
#include "pch.h"

int main() {

    /*--- 변수---*/
    cv::Mat matImage, matImageGray, matImageBinary;
    std::vector<std::vector<std::string>> vvCamImagePaths;
    std::vector<cv::Point3f> obj3D;
    std::vector<cv::Point2f> img2D;
    std::vector<std::vector<cv::Point3f>> list_obj3D;
    std::vector<std::vector<cv::Point2f>> list_img2D;

    /*--- 데이터 ---*/ 
    // TODO: 캘리브레이션 이미지 경로 동적 추가 자동화
    /*int nCamIdx = countFolders(basePath1);
    std::vector<int> nImages;
    for (int cam = 1; cam <= nCamIdx; ++cam) {
        nImages[nCamIdx] = countFiles(basePath1);
        for (int i = 1; i <= nImages[cam]; ++i) {
            std::string path = "cam" + std::to_string(nCamIdx) + "/image" + std::to_string(i) + ".jpg";
            vvCamImagePaths[cam].push_back(path);
        }
    }*/

    std::string path;
    int nCam = 0, nImages = 0;
    int mode = 0;
    if (mode == 1) {
        std::string basePath1 = "C:\\Projects\\_DNC\\_image\\dnc_AreaCam25x4_checkerboard\\0724\\cutting\\UpperCuttingZone1\\25M\\pattern_film";
        nCam = 4, nImages = 2;
        vvCamImagePaths.resize(nCam);
        for (int i = 0; i < nCam; i++) {
            for (int j = 0; j < nImages; j++) {
                path = basePath1 + "\\Sur_F081" + std::to_string(j + 71) + "_Cam" + std::to_string(i + 1) + "_C00000_NULL_CODE" + ".jpg";
                vvCamImagePaths[i].push_back(path);
                path = "";
            }
        }
    }
    else if (mode == 2) {
        std::string basePath2 = "C:\\Projects\\_DNC\\_image\\dnc_AreaCam25x4_checkerboard\\0724\\cutting\\UpperCuttingZone1\\25M\\glass_ruler";
        nCam = 4, nImages = 3;
        for (int i = 0; i < nCam; i++) {
            vvCamImagePaths.resize(nCam);
            for (int j = 0; j < nImages; j++) {
                path = basePath2 + "\\Sur_F081" + std::to_string(j + 79) + "_Cam" + std::to_string(i + 1) + "_C00000_NULL_CODE" + ".jpg";
                vvCamImagePaths[i].push_back(path);
                path = "";
            }
        }
    }
    else if (mode == 0/*TEST*/) {
        std::string basePath1 = "C:\\Projects\\_DNC\\__calibration\\test";
        nCam = 1, nImages = 12;
        vvCamImagePaths.resize(nCam);
        for (int i = 0; i < nCam; i++) {
            for (int j = 0; j < nImages; j++) {
                path = basePath1 + "\\" + std::to_string(j + 10) + ".jpg";
                vvCamImagePaths[i].push_back(path);
                path = "";
            }
        }
    }
    /*--- 연산 ---*/
    for (int cam = 0; cam < vvCamImagePaths.size(); cam++) {

        // 2D && 3D 좌표 세트 구하기 - 모든 이미지 in 각 카메라
        for (int imagePath = 0; imagePath < vvCamImagePaths[cam].size(); imagePath++) {

            // 작업 이미지 로드
            /*TEST*///std::string imagePath = "C:\\Projects\\_DNC\\__calibration\\checkerboard_ideal_18x10_fisheye.jpg";
            matImage = cv::imread(vvCamImagePaths[cam][imagePath]);
            if (matImage.empty()) continue;
            cv::cvtColor(matImage, matImageGray, cv::COLOR_BGR2GRAY);

            // TODO: 체커보드 이미지 추상화 (규격확인)
            cv::Size chessboard;
            
            //cv::threshold(matImageGray, matImageGray, 110, 255, cv::THRESH_BINARY);
            //cv::adaptiveThreshold(matImageGray, matImageBinary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 251, 2);
            //cv::adaptiveThreshold(matImageGray, matImageBinary, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 111, 2);//!
            //cv::adaptiveThreshold(matImageGray, matImageBinary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 111, 2);//!
            //cv::adaptiveThreshold(matImageGray, matImageBinary, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 51, 2);
            //cv::adaptiveThreshold(matImageGray, matImageBinary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 51, 2);

            //cv::resize(matImageGray, matImageGray, cv::Size(), 0.1, 0.1, cv::INTER_LINEAR);
            //cv::equalizeHist(matImageGray, matImageGray);
            //cv::GaussianBlur(matImageGray, matImageGray, cv::Size(5, 5), 0);
            //cv::adaptiveThreshold(matImageGray, matImageGray, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 251, 2);
            //cv::morphologyEx(matImageGray, matImageGray, cv::MORPH_CLOSE, cv::Mat());

            // Image상의 2D 포인트 구하기 
            bool chessboardFound = false;
            for (int r = 7; r < 10; r += 2) {
                for (int c = 15; c < 18; c += 2) {
                    chessboard = cv::Size(c, r);
                    while (matImageGray.cols > 2000) {
                    cv::resize(matImageGray, matImageGray, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);
                    cv::resize(matImage, matImage, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);
                    }                    
                    chessboardFound = cv::findChessboardCorners(matImageGray, chessboard, img2D, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);
                    std::cout << c << ", " << r << " end" << std::endl;
                    if (chessboardFound) {
                        cv::cornerSubPix(matImageGray, img2D, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
                        break;
                    }
                }
                if (chessboardFound)
                    break;
                if (!chessboardFound && r == 9) {
                    break; //not find!!
                }
            }

            /*TEST*/
            /*TEST*/cv::Mat matImage2Dpoints = matImageGray.clone();
            /*TEST*/cv::Scalar green(0, 255, 0);
            /*TEST*/cv::drawChessboardCorners(matImage, chessboard, img2D, chessboardFound);
            /*TEST*/for (size_t i = 0; i < img2D.size(); ++i) { cv::circle(matImage2Dpoints, img2D[i], 5, green, -1); }
            
            /*TEST*///cv::imwrite("C:\\Projects\\_DNC\\__calibration\\test\\" + std::to_string(imagePath + 10) + "_corners.jpg", matImage);
            /*TEST*///cv_::imshowWindow("2D points on Checker-board", matImage2Dpoints);
            /*TEST*///cv::waitKey(0);


            list_img2D.push_back(img2D);
            img2D.clear();  // 2D 포인트 초기화

            // 실제 Object 3D 포인트
            for (int r = 0; r < chessboard.height; r++) {
                for (int c = 0; c < chessboard.width; c++) {
                    obj3D.push_back(cv::Point3f(c, r, 0));
                }
            }
            list_obj3D.push_back(obj3D);
            obj3D.clear();  // 3D 포인트 초기화
            /*TEST*///break;
        }

        // 카메라 캘리브레이션 : 3D좌표 세트, 2D좌표 세트, 연산이미지크기, &카메라매트릭스(초점거리/중심좌표), &랜즈왜곡계수, &카메라회전벡터, &카메라위치벡터
        cv::Mat cameraMatrix, distortionCoefficient;
        std::vector<cv::Mat> rotationVectors, translationVectors;
        cv::calibrateCamera(list_obj3D, list_img2D, cv::Size(matImageGray.rows, matImageGray.cols),
                            cameraMatrix, distortionCoefficient, rotationVectors, translationVectors);

        // 왜곡 보정
        cv::Mat matImageUndistorted, matImageOriginal = matImage;

        /*TEST오목볼록이미지만들기 */
        /*TEST*///cameraMatrix = (cv::Mat_<double>(3, 3) << 800, 0, matImage.cols / 2.0, 0, 800, matImage.rows / 2.0, 0, 0, 1); // 임의의 카메라 매트릭스 <왜곡용>!
        /*TEST*///distortionCoefficient = (cv::Mat_<double>(1, 5) << -0.1, -0.1, 0, 0, 0); // 핀쿠션 왜곡 계수
        /*TEST*///cv::undistort(matImageOriginal, matImageUndistorted, cameraMatrix, distortionCoefficient);
        /*TEST*///cv::imwrite("C:\\Projects\\_DNC\\__calibration\\checkerboard_ideal_18x10_pincushion.jpg", matImageUndistorted);
        /*TEST*///distortionCoefficient = (cv::Mat_<double>(1, 5) << 0.3, 0.1, 0, 0, 0); // 어안렌즈 왜곡 계수
        /*TEST*///cv::undistort(matImageOriginal, matImageUndistorted, cameraMatrix, distortionCoefficient);
        /*TEST*///cv::imwrite("C:\\Projects\\_DNC\\__calibration\\checkerboard_ideal_18x10_fisheye.jpg", matImageUndistorted);
        /*TEST*///cv::imshow("Undistorted Image", matImageUndistorted);
        /*TEST*///cv::waitKey(0);

        cv::undistort(matImageOriginal, matImageUndistorted, cameraMatrix, distortionCoefficient); // 회전벡터를 회전행렬로

        
        
        /* TEST 삐뚠이미지만들기 */
        /*TEST*/cameraMatrix = (cv::Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1); // 임의의 카메라 매트릭스
        /*TEST*/double angle = CV_PI / 180.0; // 1도 = 0.0174533 라디안
        /*TEST*/rotationVectors[cam] = (cv::Mat_<double>(3, 1) << 0 * angle, 0 * angle, 5 * angle); // 임의의 회전 벡터
        /*TEST*/translationVectors[cam] = (cv::Mat_<double>(3, 1) << 500, 500, 11111111111111111110); // 임의의 변환 벡터 <무의미>!

        // TODO: 원근 보정
        cv::Mat matImageTransformed, rotationMatrix;
        cv::Rodrigues(rotationVectors[cam], rotationMatrix);

        cv::Mat rt = cv::Mat::eye(3, 4, CV_64F);
        rotationMatrix.copyTo(rt(cv::Rect(0, 0, 3, 3)));
        translationVectors[cam].copyTo(rt(cv::Rect(3, 0, 1, 3)));
        cv::Mat perspectiveMatrix = cameraMatrix * rt;
        cv::Mat perspectiveMatrix3x3 = perspectiveMatrix(cv::Rect(0, 0, 3, 3));
        perspectiveMatrix3x3.at<double>(2, 2) = 1;

        cv::warpPerspective(matImageUndistorted, matImageTransformed, perspectiveMatrix3x3, matImageUndistorted.size());
        /*TEST*/cv::imwrite("C:\\Projects\\_DNC\\__calibration\\checkerboard_ideal_18x10_perspective.jpg", matImageTransformed);

        /*TEST*/cv::imshow("Projected Points", matImageOriginal);
        /*TEST*/cv::imshow("Projected Points", matImageTransformed);
        /*TEST*/cv::waitKey(0);
    }

    

	while(true) {

	}
	return 0;
}