#pragma once
#include "cv_calibration.h"

cv_calibration::cv_calibration() 
    : roiRect(0,0,0,0) {
}

cv_calibration::~cv_calibration() {
}

void cv_calibration::MAIN_ImageCalibration() {

    /*--- 변수---*/
    std::vector<cv::Mat> matImages, matImageResults; // src, dst
    std::vector< std::vector<cv::Point2f>> srcList_img2D, dstList_img2D;  // src, dst
    cv::Mat matImageGray, matImageBinary; // 연산용
    int nCutHalfTimes = 0;
    cv::Mat matImageView; // 확인용
    std::vector<std::vector<std::string>> vvCamImagePaths;
    std::vector<cv::Point3f> obj3D;
    std::vector<cv::Point2f> img2D;
    std::vector<std::vector<cv::Point3f>> list_obj3D;
    std::vector<std::vector<cv::Point2f>> list_img2D;
    /*TEST*/cv::Scalar red(0, 0, 255), green(0, 255, 0), blue(255, 0, 0);

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
                path = basePath1 + "\\" + std::to_string(j + 11) + ".jpg";
                vvCamImagePaths[i].push_back(path);
                path = "";
            }
        }
    }
    /*--- 연산 ---*/
    for (int cam = 0; cam < vvCamImagePaths.size(); cam++) {
        matImages.resize(vvCamImagePaths[cam].size());
        matImageResults.resize(vvCamImagePaths[cam].size());

        // 2D && 3D 좌표 세트 구하기 - 모든 이미지 in 각 카메라
        for (int imagePath = 0; imagePath < vvCamImagePaths[cam].size(); imagePath++) {

            // 작업 이미지 로드
            /*TEST*///std::string testpath = "C:\\Projects\\_DNC\\__calibration\\test\\12.jpg"; vvCamImagePaths[cam][imagePath] = testpath;
            matImages[imagePath] = cv::imread(vvCamImagePaths[cam][imagePath]);
            matImageResults[imagePath] = matImages[imagePath].clone();
            if (matImages[imagePath].empty()) continue;
            cv::cvtColor(matImages[imagePath], matImageGray, cv::COLOR_BGR2GRAY);

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
            for (int r = 9; r < 10; r += 2) {
                for (int c = 17; c < 18; c += 2) {
                    nCutHalfTimes = 0;
                    chessboard = cv::Size(c, r);
                    while (matImageGray.cols > 2000) {
                        nCutHalfTimes++;
                        cv::resize(matImageGray, matImageGray, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);
                        cv::resize(matImageResults[imagePath], matImageResults[imagePath], cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);
                    }
                    chessboardFound = cv::findChessboardCorners(matImageGray, chessboard, img2D, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);
                    std::cout << c << ", " << r << " end" << std::endl;
                    if (chessboardFound) {
                        std::cout << imagePath << " of corners has found!" << std::endl;
                        cv::cornerSubPix(matImageGray, img2D, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
                        break;
                    }
                }
                if (chessboardFound)
                    break;
                if (!chessboardFound && r == 9) {
                    std::cerr << imagePath << " of corners has not found!" << std::endl;
                    break; //not find!!
                }
            }

            /*TEST*/cv::drawChessboardCorners(matImageResults[imagePath], chessboard, img2D, chessboardFound);
            /*TEST*///cv::imwrite("C:\\Projects\\_DNC\\__calibration\\test\\" + std::to_string(imagePath + 10) + "_corners.jpg", matImageResults[imagePath]);

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

            matImageGray = cv::Mat::zeros(0, 0, CV_8UC3);
            /*TEST*///break; // 사진 1장만 테스트
        }

        // 카메라 캘리브레이션(이미지1장에 대해 검사 rotationVectors[image], translationVectors[image]) : 3D좌표 세트, 2D좌표 세트, 연산이미지크기, &카메라매트릭스(초점거리/중심좌표), &랜즈왜곡계수, &카메라회전벡터, &카메라위치벡터
        cv::Mat cameraMatrix, distortionCoefficient;
        std::vector<cv::Mat> rotationVectors, translationVectors;
        cv::calibrateCamera(list_obj3D, list_img2D, cv::Size(matImageResults[0].rows, matImageResults[0].cols), // TODO: 이미지사이즈 변수화하기
                            cameraMatrix, distortionCoefficient, rotationVectors, translationVectors);

        list_obj3D.clear();
        dstList_img2D = srcList_img2D = list_img2D;
        list_img2D.clear();

        // 캘리브레이션 적용
        cv::Mat matImageUndistorted, matImageTransformed;
        cv::Mat rt, rotationMatrix, perspectiveMatrix, perspectiveMatrix3x3;

        /*TEST오목볼록이미지만들기 */
        /*TEST*///cameraMatrix = (cv::Mat_<double>(3, 3) << 800, 0, matImage.cols / 2.0, 0, 800, matImage.rows / 2.0, 0, 0, 1); // 임의의 카메라 매트릭스 <왜곡용>!
        /*TEST*///distortionCoefficient = (cv::Mat_<double>(1, 5) << -0.1, -0.1, 0, 0, 0); // 핀쿠션 왜곡 계수
        /*TEST*///cv::undistort(matImageOriginal, matImageUndistorted, cameraMatrix, distortionCoefficient);
        /*TEST*///cv::imwrite("C:\\Projects\\_DNC\\__calibration\\checkerboard_ideal_18x10_pincushion.jpg", matImageUndistorted);
        /*TEST*///distortionCoefficient = (cv::Mat_<double>(1, 5) << 0.3, 0.1, 0, 0, 0); // 어안렌즈 왜곡 계수
        /*TEST*///cv::undistort(matImageOriginal, matImageUndistorted, cameraMatrix, distortionCoefficient);
        /*TEST*///cv::imwrite("C:\\Projects\\_DNC\\__calibration\\checkerboard_ideal_18x10_fisheye.jpg", matImageUndistorted);
        /*TEST*///cv::imshow("Undistorted Image", matImageUndistorted); cv::waitKey(0);

        /* TEST 원근이미지만들기 */
        /*TEST*///cameraMatrix = (cv::Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1); // 임의의 카메라 매트릭스
        /*TEST*///double angle = CV_PI / 180.0; // 1도 = 0.0174533 라디안
        /*TEST*///rotationVectors[cam] = (cv::Mat_<double>(3, 1) << 0 * angle, 0 * angle, 5 * angle); // 임의의 회전 벡터
        /*TEST*///translationVectors[cam] = (cv::Mat_<double>(3, 1) << 500, 500, 11111111111111111110); // 임의의 변환 벡터 <무의미>!

        for (int imagePath = 0; imagePath < vvCamImagePaths[cam].size(); imagePath++) {
            // 왜곡 보정
            cv::undistort(matImageResults[imagePath], matImageUndistorted, cameraMatrix, distortionCoefficient); // 회전벡터를 회전행렬로
            /*TEST*/cv::imwrite("C:\\Projects\\_DNC\\__calibration\\test\\" + std::to_string(imagePath + 11) + "_corners_undistort.jpg", matImageUndistorted);

            // 포인트 왜곡 보정
            cv::Mat matImage2Dpoints = matImageResults[imagePath].clone();
            for (size_t i = 0; i < srcList_img2D[imagePath].size(); ++i) { cv::circle(matImage2Dpoints, srcList_img2D[imagePath][i], 5, green, -1); }
            cv::undistortPoints(srcList_img2D[imagePath], dstList_img2D[imagePath], cameraMatrix, distortionCoefficient); // 정규화된 포인트 반환
            for (int i = 0; i < dstList_img2D[imagePath].size(); i++) {
                dstList_img2D[imagePath][i].x = dstList_img2D[imagePath][i].x * cameraMatrix.at<double>(0, 0) + cameraMatrix.at<double>(0, 2);
                dstList_img2D[imagePath][i].y = dstList_img2D[imagePath][i].y * cameraMatrix.at<double>(1, 1) + cameraMatrix.at<double>(1, 2);
            }
            for (size_t i = 0; i < srcList_img2D[imagePath].size(); ++i) { cv::circle(matImage2Dpoints, dstList_img2D[imagePath][i], 3, red, -1); }
            /*TEST*/cv::imwrite("C:\\Projects\\_DNC\\__calibration\\test\\" + std::to_string(imagePath + 11) + "_corners_undistortpoints.jpg", matImage2Dpoints);


            continue;


            /* TEST 원근이미지만들기 */
            /*TEST*///cameraMatrix = (cv::Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1); // 임의의 카메라 매트릭스
            /*TEST*///double angle = CV_PI / 180.0; // 1도 = 0.0174533 라디안
            /*TEST*///rotationVectors[cam] = (cv::Mat_<double>(3, 1) << 0 * angle, 0 * angle, 5 * angle); // 임의의 회전 벡터
            /*TEST*///translationVectors[cam] = (cv::Mat_<double>(3, 1) << 500, 500, 11111111111111111110); // 임의의 변환 벡터 <무의미>!

            // TODO: 원근 보정--------------------------------------------------------------------------------------------------------------------------------
            //cameraMatrix /= std::pow(2, nCutHalfTimes);
            //cameraMatrix.at<double>(2, 2) = 1.0;
            //for (int i = 0; i < translationVectors.size(); i++) {
            //    translationVectors[i] /= std::pow(2, nCutHalfTimes);
            //}

            /*TEST*/cameraMatrix = (cv::Mat_<double>(3, 3) <<
                                    1, 0, 0,
                                    0, 1, 0,
                                    0, 0, 1);
            rt = cv::Mat::eye(3, 4, CV_64F); //double
            cv::Rodrigues(rotationVectors[imagePath], rotationMatrix);
            ///*TEST*/rotationMatrix = (cv::Mat_<double>(3, 3) <<
            //                  1, 0, 0,
            //                  0, 1, 0,
            //                  0, 0, 1);
            rotationMatrix.copyTo(rt(cv::Rect(0, 0, 3, 3)));
            translationVectors[imagePath].copyTo(rt(cv::Rect(3, 0, 1, 3)));

            perspectiveMatrix = cameraMatrix * rt;
            perspectiveMatrix3x3 = perspectiveMatrix(cv::Rect(0, 0, 3, 3));
            perspectiveMatrix3x3.at<double>(2, 2) = 1.0;

            // 포인트 변환 적용
            cv::perspectiveTransform(srcList_img2D[imagePath], dstList_img2D[imagePath], perspectiveMatrix3x3);
            for (size_t i = 0; i < srcList_img2D[imagePath].size(); ++i) { cv::circle(matImage2Dpoints, dstList_img2D[imagePath][i], 3, blue, -1); }

            cv::warpPerspective(matImageUndistorted, matImageTransformed, perspectiveMatrix3x3, matImageUndistorted.size());
            matImageTransformed.empty();
            /*TEST*///cv::imwrite("C:\\Projects\\_DNC\\__calibration\\test\\"" + std::to_string(imagePath + 11) + "_corners_undistort_warpPerspective.jpg", matImageTransformed);
            /*TEST*///cv::hconcat(matImageResults[imagePath], matImageTransformed, matImageView);
            /*TEST*///cv::imshow("Undistorted Image", matImageUndistorted); cv::waitKey(0);
        }
        matImages.clear();
        matImageResults.clear();
    }
    vvCamImagePaths.clear();
    return;
}

void cv_calibration::TEST_cv_calibration() {
    std::vector<std::vector<std::string>> vvCamImagePaths;
    std::string basePath = "C:\\Projects\\_DNC\\__calibration\\test1\\";
    const int nCam = 4;

    //self-sample
    /*for (int sample1 = 1; sample1 < 13; sample1++) {
        vvCamImagePaths.emplace_back([basePath, sample1](std::vector<std::string> vStr) { vStr.emplace_back(basePath + "sample1_" + std::to_string(10 + sample1) + ".jpg"); return vStr; }(std::vector<std::string>()));
    }*/
    for (int sample1 = 0; sample1 < 4; sample1++) {
        vvCamImagePaths.emplace_back([basePath, sample1](std::vector<std::string> vStr) { vStr.emplace_back(basePath + "sample7_cam" + std::to_string(1 + sample1) + ".jpg"); return vStr; }(std::vector<std::string>()));
    }

    //laboratory-sample
    //vvCamImagePaths.emplace_back([basePath](std::vector<std::string> vStr) { vStr.emplace_back(basePath + "sample2_8mm.bmp"); return vStr; }(std::vector<std::string>()));
    //vvCamImagePaths.emplace_back([basePath](std::vector<std::string> vStr) { vStr.emplace_back(basePath + "sample3_12mm.bmp"); return vStr; }(std::vector<std::string>()));
    //vvCamImagePaths.emplace_back([basePath](std::vector<std::string> vStr) { vStr.emplace_back(basePath + "sample4_25mm.bmp"); return vStr; }(std::vector<std::string>()));
    //vvCamImagePaths.emplace_back([basePath](std::vector<std::string> vStr) { vStr.emplace_back(basePath + "sample5_50mm.bmp"); return vStr; }(std::vector<std::string>()));
    //vvCamImagePaths.emplace_back([basePath](std::vector<std::string> vStr) { vStr.emplace_back(basePath + "sample6_75mm.bmp"); return vStr; }(std::vector<std::string>()));

    //upper
    for (int nCam = 0; nCam < 4; nCam++) {
        std::vector<std::string> vTemp;
        for (int nTry = 0; nTry < 2; nTry++) {
            vTemp.emplace_back(basePath + "cutting_upper1_try" + std::to_string(nTry + 1) + "_cam" + std::to_string(nCam + 1) + ".jpg");
        }
        //vvCamImagePaths.emplace_back(vTemp);
    }
    
    //lower
    for (int nCam = 0; nCam < 4; nCam++) {
        std::vector<std::string> vTemp;
        for (int nTry = 0; nTry < 4; nTry++) {
            vTemp.emplace_back(basePath + "cutting_lower3_try" + std::to_string(nTry + 1) + "_cam" + std::to_string(nCam + 1) + ".jpg");
        }
        //vvCamImagePaths.emplace_back(vTemp);
    }

    std::vector<std::vector<cv::Mat>> vvMatImages{}, vvMatImageResults{}; // src, dst
    std::vector<std::vector<std::vector<cv::Point2f>>> vvvCamImagesCorners{}, vvvCamImagesCornersResults{};
    vvMatImages.resize(vvCamImagePaths.size());
    vvMatImageResults.resize(vvCamImagePaths.size());

    for (int cam = 0; cam < vvCamImagePaths.size(); cam++) {
        for (int img = 0; img < vvCamImagePaths[cam].size(); img++) {
            vvMatImages[cam].emplace_back(cv::imread(vvCamImagePaths[cam][img]));
            vvMatImageResults[cam].emplace_back(vvMatImages[cam][img].clone());
            if (vvMatImageResults[cam][img].empty()) continue;
        }
    }

    for (int cam = 0; cam < vvMatImageResults.size(); cam++) {
        vvvCamImagesCorners.clear();
        vvvCamImagesCornersResults.clear();
        vvvCamImagesCorners.resize(vvCamImagePaths.size());

        cv::Mat cameraMatrix{}, distortionCoefficient{};
        std::vector<cv::Mat> rotationVectors{}, translationVectors{};
        makeImageCalibration(vvMatImages[cam], vvvCamImagesCorners[cam], cameraMatrix, distortionCoefficient, rotationVectors, translationVectors);
        /*TEST*///continue;// 체스보드검출만 테스트

        for (int img = 0; img < vvMatImageResults[cam].size(); img++) {
            vvvCamImagesCornersResults = vvvCamImagesCorners;
            std::vector<cv::Point2f> srcImagesPoints, dstImagesPoints;
            undistortPoint(vvvCamImagesCorners[cam][img], vvvCamImagesCornersResults[cam][img], cameraMatrix, distortionCoefficient);
            undistortImage(vvMatImages[cam][img], vvMatImageResults[cam][img], cameraMatrix, distortionCoefficient);

            /*TEST*/for (int pt = 0; pt < vvvCamImagesCorners[cam][img].size(); ++pt) { 
                cv::circle(vvMatImages[cam][img], vvvCamImagesCorners[cam][img][pt], 16, cvRed, -1); }
            /*TEST*/for (int pt = 0; pt < vvvCamImagesCornersResults[cam][img].size(); ++pt) { 
                cv::circle(vvMatImages[cam][img], vvvCamImagesCornersResults[cam][img][pt], 12, cvGreen, -1); }
            /*TEST*///cv::imwrite(basePath + "result\\" + std::to_string(cam + 1) + "_" + std::to_string(img + 1) + "_corners_undistort.jpg", vvMatImageResults[cam][img]);
            /*TEST*///cv::imwrite(basePath + "result\\" + std::to_string(cam + 1) + "_" + std::to_string(img + 1) + "_corners_undistortpoints.jpg", vvMatImages[cam][img]);


            // TODO: 원근 보정--------------------------------------------------------------------------------------------------------------------------------
            struct TODO_Perspective
            {
                /* TEST 원근이미지만들기 */
            /*TEST*///cameraMatrix = (cv::Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1); // 임의의 카메라 매트릭스
            /*TEST*///double angle = CV_PI / 180.0; // 1도 = 0.0174533 라디안
            /*TEST*///rotationVectors[cam] = (cv::Mat_<double>(3, 1) << 0 * angle, 0 * angle, 5 * angle); // 임의의 회전 벡터
            /*TEST*///translationVectors[cam] = (cv::Mat_<double>(3, 1) << 500, 500, 11111111111111111110); // 임의의 변환 벡터 <무의미>!
            //cameraMatrix /= std::pow(2, nCutHalfTimes);
            //cameraMatrix.at<double>(2, 2) = 1.0;
            //for (int i = 0; i < translationVectors.size(); i++) {
            //    translationVectors[i] /= std::pow(2, nCutHalfTimes);
            //}

            /*TEST*//*cameraMatrix = (cv::Mat_<double>(3, 3) <<
                                    1, 0, 0,
                                    0, 1, 0,
                                    0, 0, 1);*/
                                    //rt = cv::Mat::eye(3, 4, CV_64F); //double
                                    //cv::Rodrigues(rotationVectors[imagePath], rotationMatrix);
                                    ///*TEST*/rotationMatrix = (cv::Mat_<double>(3, 3) <<
                                    //                  1, 0, 0,
                                    //                  0, 1, 0,
                                    //                  0, 0, 1);
                                    //rotationMatrix.copyTo(rt(cv::Rect(0, 0, 3, 3)));
                                    //translationVectors[imagePath].copyTo(rt(cv::Rect(3, 0, 1, 3)));

                                    //perspectiveMatrix = cameraMatrix * rt;
                                    //perspectiveMatrix3x3 = perspectiveMatrix(cv::Rect(0, 0, 3, 3));
                                    //perspectiveMatrix3x3.at<double>(2, 2) = 1.0;

                                    // 포인트 변환 적용
                                    //cv::perspectiveTransform(srcList_img2D[imagePath], dstList_img2D[imagePath], perspectiveMatrix3x3);
                                    //for (size_t i = 0; i < srcList_img2D[imagePath].size(); ++i) { cv::circle(matImage2Dpoints, dstList_img2D[imagePath][i], 3, blue, -1); }

                                    //cv::warpPerspective(matImageUndistorted, matImageTransformed, perspectiveMatrix3x3, matImageUndistorted.size());
                                    //matImageTransformed.empty();
                                    /*TEST*///cv::imwrite("C:\\Projects\\_DNC\\__calibration\\test\\"" + std::to_string(imagePath + 11) + "_corners_undistort_warpPerspective.jpg", matImageTransformed);
                                    /*TEST*///cv::hconcat(matImageResults[imagePath], matImageTransformed, matImageView);
                                    /*TEST*///cv::imshow("Undistorted Image", matImageUndistorted); cv::waitKey(0);
            };
        }
    }
}

cv::Point2f cv_calibration::computeIntersection(const cv::Point2i& clockwise1, const cv::Point2i& clockwise2, const cv::Point2i& clockwise3, const cv::Point2i& clockwise4) {

    float x1 = clockwise1.x, y1 = clockwise1.y;
    float x2 = clockwise2.x, y2 = clockwise2.y;
    float x3 = clockwise3.x, y3 = clockwise3.y;
    float x4 = clockwise4.x, y4 = clockwise4.y;
    float intersectX, intersectY;
    float m1, b1, m2, b2;

    // 두 방정식 구하기
    (x3 - x1) == 0 ? m1 = 0 : m1 = (y3 - y1) / (x3 - x1); // y = m1(x) + b1 ... clockwise1 & clockwise3
    b1 = y1 - m1 * x1;

    (x4 - x2) == 0 ? m2 = 0 : m2 = (y4 - y2) / (x4 - x2); // y = m2(x) + b2 ... clockwise2 & clockwise4
    b2 = y2 - m2 * x2;

    // 예외처리 : 평행한 경우 (딱 두 케이스 있음, 곂치는 대각선 : 근데 이경우는 m이 0이 아님)
    if (m1 == m2 && m1 != 0 && m2 != 0) { 
        intersectX = -1; 
        intersectY = -1; 
    }
    else {
        // 예외처리 : 기울기가 0인 수직수평의 경우 
        // m1(x) + b1 = m2(x) + b2 ... (m1-m2)(x) = (b2-b1) 
        if (m1 == 0 && x3 == x1) { intersectX = x3 = x1; } else { intersectX = (b2 - b1) / (m1 - m2); }
        if (m2 == 0 && y4 == y2) { intersectY = y4 = y2; } else { intersectY = m2 * intersectX + b2; }

    }

    if (intersectX < 0 || intersectY < 0)
        intersectX = -1;

    return cv::Point2f(intersectX, intersectY);
}

int cv_calibration::getNearbyPoint(std::vector<std::pair<cv::Point2i, bool>> points, cv::Point2i point, int tolerance) {
    int idxNearbyPoint = -1; // false
    for (int i = points.size() - 1; i >= 0; --i) { // 최근검사부터 확인
        const std::pair<cv::Point2i, bool>& existedPoint = points[i];
        if (point.x >= existedPoint.first.x - tolerance &&
            point.x <= existedPoint.first.x + tolerance &&
            point.y >= existedPoint.first.y - tolerance &&
            point.y <= existedPoint.first.y + tolerance) {
            idxNearbyPoint = i;
            break;
        }
        else {
            // 현재 기준 새로운 점
        }
    }
    return idxNearbyPoint;
}

bool cv_calibration::searchCrossPoint(cv::Mat& roiImage, cv::Point2i& retPoint) {
    bool ret = false;
    uchar* img = roiImage.ptr<uchar>(0);
    const int nWidth = roiImage.cols;
    const int nHeight = roiImage.rows;

    
    int nDiff_absMax, nDiff;
    int nSlopeX, nSlopeY, nSlopeTh = 5;
    double dIsNoiseTh_StdDev = 1.0; // 68.26%
    std::vector<std::vector<cv::Scalar>> vvMeanStdDev(4, std::vector<cv::Scalar>(2));
    cv::Point2i ptUpperHorizontal, ptRightVertical, ptLowerHorizontal, ptLeftVertical;
    cv::Mat checkMatrix = (cv::Mat_<bool>(2, 2) <<
                   0, 0, 
                   0, 0);
    //double dMean_UpperHorizontal = cv::Scalar(cv::mean(roiImage.row(0)))[0];
    
    //분산검사
    cv::meanStdDev(roiImage.row(0),         vvMeanStdDev[0][0], vvMeanStdDev[0][1]); //가로상단
    cv::meanStdDev(roiImage.col(nWidth-1),  vvMeanStdDev[1][0], vvMeanStdDev[1][1]); //세로우측
    cv::meanStdDev(roiImage.row(nHeight-1), vvMeanStdDev[2][0], vvMeanStdDev[2][1]); //가로하단
    cv::meanStdDev(roiImage.col(0),         vvMeanStdDev[3][0], vvMeanStdDev[3][1]); //세로좌측

    double meanStdDev_Vertical = (vvMeanStdDev[0][1][0] + vvMeanStdDev[2][1][0]) / 2;
    double meanStdDev_Horizontal = (vvMeanStdDev[1][1][0] + vvMeanStdDev[3][1][0]) / 2;

    bool isRangeHorizontal, isRangeVertical;
    bool inrangeUH = meanStdDev_Vertical * 0.8 <= vvMeanStdDev[0][1][0] && vvMeanStdDev[0][1][0] <= meanStdDev_Vertical * 1.2;
    bool inrangeRV = meanStdDev_Horizontal * 0.8 <= vvMeanStdDev[1][1][0] && vvMeanStdDev[1][1][0] <= meanStdDev_Horizontal * 1.2;
    bool inrangeDH = meanStdDev_Vertical * 0.8 <= vvMeanStdDev[2][1][0] && vvMeanStdDev[2][1][0] <= meanStdDev_Vertical * 1.2;
    bool inrangeLV = meanStdDev_Horizontal * 0.8 <= vvMeanStdDev[3][1][0] && vvMeanStdDev[3][1][0] <= meanStdDev_Horizontal * 1.2;

    (inrangeUH && inrangeDH) ? isRangeHorizontal = false : isRangeHorizontal = true;
    (inrangeRV && inrangeLV) ? isRangeVertical = false : isRangeVertical = true;

    int exception = 0;

    //조밀한 변이 한변이라도 있다면 Cross이미지가 아님
    int cntOver200 = std::count_if(vvMeanStdDev.begin(), vvMeanStdDev.end(), [](const auto& v) { return v[0][0] >= 200; });
    if (vvMeanStdDev[0][1][0] < 10 ||
        vvMeanStdDev[1][1][0] < 10 ||
        vvMeanStdDev[2][1][0] < 10 ||
        vvMeanStdDev[3][1][0] < 10) 
    {
        exception++;
    }
    
    //4변중 2변이 200넘고 가로세로중앙이 한줄이라도 250이상이라면
    //TODO: 좀 더 스마트하게 개선할 필요성있음...
    if (cntOver200 >= 2) {
        std::vector<std::vector<cv::Scalar>> temp(2, std::vector<cv::Scalar>(2));
        cv::meanStdDev(roiImage.row(nHeight / 2), temp[0][0], temp[0][1]); //가로중앙
        cv::meanStdDev(roiImage.col(nWidth / 2), temp[1][0], temp[1][1]); //세로중앙
        int cntOver250 = std::count_if(temp.begin(), temp.end(), [](const auto& v) { return v[0][0] >= 250; });
        if (cntOver250 >= 1) {
            exception++;
        }
    }
    
    if (exception == 0) {
        /*TEST*//*if ((roiRect.x + roiRect.width) / roiRect.width - 1 == 35 && (roiRect.y + roiRect.height) / roiRect.height - 1 == 4) {
            roiRect.x += 0;
        }*/
            
        if (isRangeHorizontal) {

        }

        //정밀검사: 가로상단
        nDiff_absMax = 0, nDiff = 0, nSlopeX = 0, nSlopeY = 0;
        for (int x = 2; x < nWidth; x++) {
            nDiff = img[x] - img[x - 2];
            if (nDiff_absMax < std::abs(nDiff)) {
                nDiff_absMax = std::abs(nDiff);
                nSlopeX = x - 1;
                if (nDiff > nSlopeTh) {
                    checkMatrix.at<bool>(0, 0) = 0;
                    checkMatrix.at<bool>(0, 1) = 1;
                }
                else if (nDiff < -nSlopeTh) {
                    checkMatrix.at<bool>(0, 0) = 1;
                    checkMatrix.at<bool>(0, 1) = 0;
                }
                else {
                    nSlopeX = nWidth / 2;
                }
            }
        }
        ptUpperHorizontal = cv::Point2i(nSlopeX, 0);

        //정밀검사: 가로하단
        nDiff_absMax = 0, nDiff = 0, nSlopeX = 0, nSlopeY = 0;
        for (int x = 2; x < nWidth; x++) {
            nDiff = img[(nHeight - 1) * nWidth + x] - img[(nHeight - 1) * nWidth + (x - 2)];
            if (nDiff_absMax < std::abs(nDiff)) {
                nDiff_absMax = std::abs(nDiff);
                nSlopeX = x - 1;
                if (nDiff > nSlopeTh) {
                    checkMatrix.at<bool>(1, 0) = 0;
                    checkMatrix.at<bool>(1, 1) = 1;
                }
                else if (nDiff < -nSlopeTh) {
                    checkMatrix.at<bool>(1, 0) = 1;
                    checkMatrix.at<bool>(1, 1) = 0;
                }
                else {
                    nSlopeX = nWidth / 2;
                }
            }
        }
        ptLowerHorizontal = cv::Point2i(nSlopeX, nHeight);

        //정밀검사: 세로좌측
        nDiff_absMax = 0, nDiff = 0, nSlopeX = 0, nSlopeY = 0;
        for (int y = 2; y < nHeight; y++) {
            nDiff = img[0 + y * nWidth] - img[0 + (y - 2) * nWidth];
            if (nDiff_absMax < std::abs(nDiff)) {
                nDiff_absMax = std::abs(nDiff);
                nSlopeY = y - 1;
                if (nDiff > nSlopeTh) {
                    checkMatrix.at<bool>(0, 0) = 0;
                    checkMatrix.at<bool>(1, 0) = 1;
                }
                else if (nDiff < -nSlopeTh) {
                    checkMatrix.at<bool>(0, 0) = 1;
                    checkMatrix.at<bool>(1, 0) = 0;
                }
                else {
                    nSlopeY = nHeight / 2;
                }
            }
        }
        ptLeftVertical = cv::Point2i(0, nSlopeY);

        //정밀검사: 세로우측
        nDiff_absMax = 0, nDiff = 0, nSlopeX = 0, nSlopeY = 0;
        for (int y = 2; y < nHeight; y++) {
            nDiff = img[(nWidth - 1) + y * nWidth] - img[(nWidth - 1) + (y - 2) * nWidth];
            if (nDiff_absMax < std::abs(nDiff)) {
                nDiff_absMax = std::abs(nDiff);
                nSlopeY = y - 1;
                if (nDiff > nSlopeTh) {
                    checkMatrix.at<bool>(0, 1) = 0;
                    checkMatrix.at<bool>(1, 1) = 1;
                }
                else if (nDiff < -nSlopeTh) {
                    checkMatrix.at<bool>(0, 1) = 1;
                    checkMatrix.at<bool>(1, 1) = 0;
                }
                else {
                    nSlopeY = nHeight / 2;
                }
            }
        }
        ptRightVertical = cv::Point2i(nWidth, nSlopeY);

        //패턴시 교차점 계산
        bool pattern1 = (checkMatrix.at<bool>(0, 0) == 1 && checkMatrix.at<bool>(0, 1) == 0 &&
                         checkMatrix.at<bool>(1, 0) == 0 && checkMatrix.at<bool>(1, 1) == 1);

        bool pattern2 = (checkMatrix.at<bool>(0, 0) == 0 && checkMatrix.at<bool>(0, 1) == 1 &&
                         checkMatrix.at<bool>(1, 0) == 1 && checkMatrix.at<bool>(1, 1) == 0);

        if (pattern1 || pattern2) {
            cv::Point2f point = computeIntersection(ptUpperHorizontal, ptRightVertical, ptLowerHorizontal, ptLeftVertical);
            retPoint.x = round(point.x);
            retPoint.y = round(point.y);
            ret = true;
            /*TEST*/roiImage.ptr(0)[retPoint.y * roiImage.cols + retPoint.x] = 255;
        }
    }
    else {
        ret = false;
    }

    /*TEST*/if (exception > 0 && ret) {
    /*TEST*/std::cout << cv::mean(roiImage)[0] << std::endl;
    /*TEST*/std::cout << "↑ mean: " << vvMeanStdDev[0][0][0] << ", stddev: " << vvMeanStdDev[0][1][0] << std::endl << std::fixed << std::setprecision(1) << (meanStdDev_Vertical - vvMeanStdDev[0][1][0]) / meanStdDev_Vertical << "%" << std::endl;
    /*TEST*/std::cout << "→ mean: " << vvMeanStdDev[1][0][0] << ", stddev: " << vvMeanStdDev[1][1][0] << std::endl << std::fixed << std::setprecision(1) << (meanStdDev_Horizontal - vvMeanStdDev[1][1][0]) / meanStdDev_Horizontal << "%" << std::endl;
    /*TEST*/std::cout << "↓ mean: " << vvMeanStdDev[2][0][0] << ", stddev: " << vvMeanStdDev[2][1][0] << std::endl << std::fixed << std::setprecision(1) << (meanStdDev_Vertical - vvMeanStdDev[2][1][0]) / meanStdDev_Vertical << "%" << std::endl;
    /*TEST*/std::cout << "← mean: " << vvMeanStdDev[3][0][0] << ", stddev: " << vvMeanStdDev[3][1][0] << std::endl << std::fixed << std::setprecision(1) << (meanStdDev_Horizontal - vvMeanStdDev[3][1][0]) / meanStdDev_Horizontal << "%" << std::endl;
    /*TEST*/cv_::imshowWindow(std::to_string((roiRect.x + roiRect.width) / roiRect.width - 1) + "," + std::to_string((roiRect.y + roiRect.height) / roiRect.height - 1) + "_roiImage", roiImage);}

    return ret;
}

void cv_calibration::sortVecPoint(std::vector<cv::Point2i>& points, std::vector<std::vector<cv::Point2i>>& _requiring_squared_points) {
    const int vSizeForAtomsOnLine = 5;
    std::vector<cv::Point2i> vTemp{}, vRet{};
    _requiring_squared_points.clear();

    // y오름차순 정렬
    std::sort(points.begin(), points.end(), [](const cv::Point2i& a, const cv::Point2i& b) { return a.y < b.y; });

    // 포인트정보 Mat화
    int numofPoints = points.size();
    cv::Mat pointsMat(2, numofPoints, CV_32S), windowX;
    cv::Scalar meanX, stddevX, meanY, stddevY;
    int* pMat = pointsMat.ptr<int>(0); // 첫 번째 행의 포인터 (x 값)

    for (int i = 0; i < numofPoints; ++i) {
        pMat[i + numofPoints * 0] = points[i].x;
        pMat[i + numofPoints * 1] = points[i].y;
    }
    /*for (int i = 0; i < numofPoints; ++i) {
        pMat[i + numofPoints * 2] = checkerboardpoints[i].first.x;
        pMat[i + numofPoints * 3] = checkerboardpoints[i].first.y;
    }*/

    // 포인트정보 xy매트릭스화
    for (int vPoint = 0; vPoint + vSizeForAtomsOnLine - 1 < numofPoints; vPoint++) {
        windowX = pointsMat.row(1).colRange(vPoint, vPoint + vSizeForAtomsOnLine);
        cv::meanStdDev(windowX, meanX, stddevX);

        /*TEST*///std::cout << stddevX[0] << std::endl;

        if (stddevX[0] > 10) {
            for (int i = 0; i < vSizeForAtomsOnLine - 1; i++) {
                vTemp.emplace_back(points[vPoint + i]);
            }

            std::sort(vTemp.begin(), vTemp.end(), [](const cv::Point2i& a, const cv::Point2i& b) { return a.x < b.x; });

            for (auto& it : vTemp) { vRet.emplace_back(it); }
            _requiring_squared_points.emplace_back(vTemp);
            vTemp.clear();
            vPoint += (vSizeForAtomsOnLine - 1 - 1); ////추가분 증가(순회분 - 첫회분) - 라인체인지시 다음 반복자 상승분
        }
        else {
            vTemp.emplace_back(points[vPoint]);
        }

        if (vPoint + vSizeForAtomsOnLine == numofPoints) {
            for (int i = 1; i < vSizeForAtomsOnLine; i++) {
                vTemp.emplace_back(points[vPoint + i]);
            }

            std::sort(vTemp.begin(), vTemp.end(), [](const cv::Point2i& a, const cv::Point2i& b) { return a.x < b.x; });

            for (auto& it : vTemp) { vRet.emplace_back(it); }
            _requiring_squared_points.emplace_back(vTemp);
            vTemp.clear();
        }
    }
    points.clear();
    points = vRet;
}

bool cv_calibration::pioneerOnestep(const std::vector<cv::Point2i>& points, int& stdPointIdx, const int& stdScale, const int& nWidth, const int& roiScale, const dir& direction) {
    bool ret = false;
    const cv::Point2i& stdPoint = points[stdPointIdx];
    const float searchRatioX = 0.5, searchRatioY = 0.5;
    const int searchRangeForVertical = nWidth * 1.5 / roiScale + 1;
    const int searchRangeForHorizontal = 4;
    
    switch (direction) {
    case EAST:
        //동 1칸 순회 검색
        for (int i = 1; i < searchRangeForHorizontal + 1; i++) {
            if ((stdPointIdx + i) >= points.size()) {
                break;
            }
            else {
                if (points[stdPointIdx + i].x < stdPoint.x + stdScale * (1 + searchRatioX) &&
                    points[stdPointIdx + i].x > stdPoint.x + stdScale * (0 + searchRatioX) &&
                    points[stdPointIdx + i].y < stdPoint.y + stdScale * (0 + searchRatioY) &&
                    points[stdPointIdx + i].y > stdPoint.y - stdScale * (0 + searchRatioY)) {
                    stdPointIdx = stdPointIdx + i;
                    ret = true;
                    break;
                }
            }
        }
        break;
    case WEST:
        //서 1칸 순회 검색
        for (int i = 1; i < searchRangeForHorizontal + 1; i++) {
            if ((stdPointIdx - i) < 0) {
                break;
            }
            else {
                if (points[stdPointIdx - i].x < stdPoint.x - stdScale * (0 + searchRatioX) &&
                    points[stdPointIdx - i].x > stdPoint.x - stdScale * (1 + searchRatioX) &&
                    points[stdPointIdx - i].y < stdPoint.y + stdScale * (0 + searchRatioY) &&
                    points[stdPointIdx - i].y > stdPoint.y - stdScale * (0 + searchRatioY)) {
                    stdPointIdx = stdPointIdx - i;
                    ret = true;
                    break;
                }
            }
        }
        break;
    case SOUTH:
        //남 1칸 순회 검색
        for (int i = 1; i < searchRangeForVertical; i++) {
            if ((stdPointIdx + i) >= points.size()) {
                break;
            }
            else {
                if (points[stdPointIdx + i].x < stdPoint.x + stdScale * (0 + searchRatioX) &&
                    points[stdPointIdx + i].x > stdPoint.x - stdScale * (0 + searchRatioX) &&
                    points[stdPointIdx + i].y < stdPoint.y + stdScale * (1 + searchRatioY) &&
                    points[stdPointIdx + i].y > stdPoint.y + stdScale * (0 + searchRatioY)) {
                    stdPointIdx = stdPointIdx + i;
                    ret = true;
                    break;
                }
            }
        }
        break;
    case NORTH:
        //북 1칸 순회 검색
        for (int i = 1; i < searchRangeForVertical; i++) {
            if ((stdPointIdx - i) < 0) {
                break;
            }
            else {
                if (points[stdPointIdx - i].x < stdPoint.x + stdScale * (0 + searchRatioX) &&
                    points[stdPointIdx - i].x > stdPoint.x - stdScale * (0 + searchRatioX) &&
                    points[stdPointIdx - i].y < stdPoint.y - stdScale * (0 + searchRatioY) &&
                    points[stdPointIdx - i].y > stdPoint.y - stdScale * (1 + searchRatioY)) {
                    stdPointIdx = stdPointIdx - i;
                    ret = true;
                    break;
                }
            }
        }
        break;
    default:
        break;
    }

    return ret;
}

void cv_calibration::makeCheckboard(cv::Mat& grayImageAbstracted, cv::Size& chessboardCornersizes) {
    uchar* img = grayImageAbstracted.ptr<uchar>(0);
    cv::Mat showImage = grayImageAbstracted.clone();
    uchar* imgshow = showImage.ptr<uchar>(0);
    const int nWidth = grayImageAbstracted.cols;
    const int nHeight = grayImageAbstracted.rows;
    const int nWhite = 255;
    const int nBlack = 0;
    cv::Scalar cvWhite(255);
    cv::Scalar cvBlack(0);
    
    cv::Point centerPoint(nWidth/2, nHeight/2);
    int centerPointPixelIndex = centerPoint.y * nWidth + centerPoint.x;


    /*------------------------------------1.크로스포인트 검출------------------------------------*/
    const double dRoiAccuracy = 0.02;
    int roiScale = nWidth * dRoiAccuracy > nHeight * dRoiAccuracy ? nWidth * dRoiAccuracy : nHeight * dRoiAccuracy;
    const float overlayScale = 0.85f;
    int idxNearbyPoint, toleranceNearbyPoint;
    
    roiRect.x = -roiScale * overlayScale;
    roiRect.y = -roiScale * overlayScale;
    roiRect.width = roiScale;
    roiRect.height = roiScale;
    cv::Mat roiImage;
    cv::Point2i retpoint;
    std::vector<std::pair<cv::Point2i, bool/*duplication*/>> checkerboardpoints;

    for (int y = 0; y < (nHeight / (roiScale * overlayScale) - 2); y++) {
        roiRect.x = -roiScale * overlayScale;
        roiRect.y += roiScale * overlayScale;
        for (int x = 0; x < (nWidth / (roiScale * overlayScale) - 2); x++) {
            /*TEST*///std::cout << x << ", " << y << std::endl;
            /*TEST*/cv::rectangle(showImage, roiRect, cv::Scalar(125, 125, 125), 1);

            roiRect.x += roiScale * overlayScale;
            roiImage = grayImageAbstracted(roiRect).clone();
            retpoint.x = 0;
            retpoint.y = 0;
            /*TEST*/if (checkerboardpoints.size() == 43) {
            /*TEST*/    retpoint.x = retpoint.x;
            /*TEST*/}
            if (searchCrossPoint(roiImage, retpoint)) {
                retpoint.x += roiRect.x;
                retpoint.y += roiRect.y;
                /*TEST*/cv::circle(showImage, retpoint, 33, 255, 33);
                // 검사된 점과 유사한 위치의 점이 있는 지 순회 - 인덱싱을 반환
                toleranceNearbyPoint = round((1 - overlayScale) * roiScale);
                idxNearbyPoint = getNearbyPoint(checkerboardpoints, retpoint, toleranceNearbyPoint);
                if (idxNearbyPoint == -1) {
                    // 유사점 없음
                    checkerboardpoints.emplace_back(retpoint, false);
                    /*TEST*/imgshow[(retpoint.y) * nWidth + (retpoint.x)] = nBlack;
                }
                else {
                    // 0 ~ 유사점 인덱스반환
                    if (!checkerboardpoints[idxNearbyPoint].second) {
                        // 재검사 수행
                        (roiRectDuplication.x = checkerboardpoints[idxNearbyPoint].first.x - roiScale / 2) < 0 ? roiRectDuplication.x = 0 : 0;
                        (roiRectDuplication.y = checkerboardpoints[idxNearbyPoint].first.y - roiScale / 2) < 0 ? roiRectDuplication.y = 0 : 0;
                        roiRectDuplication.width = roiScale;
                        roiRectDuplication.height = roiScale;
                        roiImage = grayImageAbstracted(roiRectDuplication).clone();
                        if (searchCrossPoint(roiImage, retpoint)) {
                            retpoint.x += roiRectDuplication.x;
                            retpoint.y += roiRectDuplication.y;
                            checkerboardpoints[idxNearbyPoint].first = retpoint;
                            checkerboardpoints[idxNearbyPoint].second = true;
                            /*TEST*/imgshow[(retpoint.y) * nWidth + (retpoint.x)] = nWhite;
                        }
                        else {
                            std::cout << "nonsense - check the logic of <searchCrossPoint>" << std::endl;
                        }
                    }
                    else {
                        // 재검사를 이미한 점 (수정하지 않음)
                    }
                }
            }
        }
    }
    /*TEST*/for (auto& pt : checkerboardpoints) {
    /*TEST*/    cv::circle(showImage, pt.first, 1, 255, 2);
    /*TEST*/}
    /*TEST*///cv_::imshowWindow("processing", showImage);

    /*------------------------------------2.체커보드재구성------------------------------------*/
    /*TEST*/showImage = grayImageAbstracted.clone();
    std::vector<cv::Point2i> crossPoints;
    for (auto& it : checkerboardpoints) { 
        crossPoints.emplace_back(it.first); 
    }
    sortVecPoint(crossPoints);
    checkerboardpoints.clear();

    int idxMiddle = crossPoints.size() * 2 / 3;
    bool found = false;
    for (int j = 0; j < roiScale * 10; j += 20) {
        for (int i = crossPoints.size() / 3; i < crossPoints.size() * 2 / 3; i++) {
            if (crossPoints[i].x < nWidth / 2 + j &&
                crossPoints[i].x > nWidth / 2 - j &&
                crossPoints[i].y < nHeight / 2 + j &&
                crossPoints[i].y > nHeight / 2 - j) {
                idxMiddle = i;
                found = true;
                break;
            }
        }
        if (found)
            break;
    }

    cv::Point2i middlePoint = crossPoints[idxMiddle];
    cv::Rect stableArea(middlePoint.x, middlePoint.y, 0, 0);

    const int vSizeForAtomsOnLine = 5;
    int estimatedScale = 0;
    for (int i = -2; i < vSizeForAtomsOnLine - 2; i++) { estimatedScale += (crossPoints[idxMiddle + i].x - crossPoints[idxMiddle + i - 1].x); }
    estimatedScale /= vSizeForAtomsOnLine;

    std::vector<std::vector<cv::Point2i>> yxMatrix(0);
    std::vector<cv::Point2i> vRow(0),vRowTemp(0);

    dir dirOnestep = (dir)0;
    int stdPointIdx, stdPointIdxTemp;
    std::pair<int, int> idxMiddle_yxMatrix(0, 0);
    std::vector<std::pair<int, int>> lengthStdPointIdxDir = { {+0, idxMiddle}, {-0, idxMiddle}, {+0, idxMiddle}, {-0, idxMiddle} }; // +동(시작) -서 +남 -북
    std::pair<std::vector<bool>, std::vector<int>> pioneerOneside; // 개별 확장성공여부 & 개별 포인트 인덱스
    std::vector<bool> pioneerClosedDir = {false, false, false, false};

    vRow.emplace_back(middlePoint);
    while (!isAllTrue(pioneerClosedDir)) {
        // 로직을 반복
        if (!pioneerClosedDir[dirOnestep]) {

            // onestep
            stdPointIdx = lengthStdPointIdxDir[dirOnestep].second;
            pioneerOneside.first.emplace_back(pioneerOnestep(crossPoints, stdPointIdx, estimatedScale, nWidth, roiScale, dirOnestep));
            pioneerOneside.second.emplace_back(stdPointIdx);

            if (pioneerOneside.first[0]) {
                lengthStdPointIdxDir[dirOnestep].second = stdPointIdx;

                // backupstep
                switch (dirOnestep) {
                case EAST: // 남북확장
                    stdPointIdxTemp = stdPointIdx;
                    for (int i = 0; i < std::abs(lengthStdPointIdxDir[SOUTH].first); i++) {
                        pioneerOneside.first.emplace_back(pioneerOnestep(crossPoints, stdPointIdx, estimatedScale, nWidth, roiScale, SOUTH));
                        pioneerOneside.second.emplace_back(stdPointIdx);
                    }
                    stdPointIdx = stdPointIdxTemp;
                    for (int i = 0; i < std::abs(lengthStdPointIdxDir[NORTH].first); i++) {
                        pioneerOneside.first.emplace_back(pioneerOnestep(crossPoints, stdPointIdx, estimatedScale, nWidth, roiScale, NORTH));
                        pioneerOneside.second.emplace_back(stdPointIdx);
                    }
                    break;
                case WEST: // 남북확장
                    stdPointIdxTemp = stdPointIdx;
                    for (int i = 0; i < std::abs(lengthStdPointIdxDir[SOUTH].first); i++) {
                        pioneerOneside.first.emplace_back(pioneerOnestep(crossPoints, stdPointIdx, estimatedScale, nWidth, roiScale, SOUTH));
                        pioneerOneside.second.emplace_back(stdPointIdx);
                    }
                    stdPointIdx = stdPointIdxTemp;
                    for (int i = 0; i < std::abs(lengthStdPointIdxDir[NORTH].first); i++) {
                        pioneerOneside.first.emplace_back(pioneerOnestep(crossPoints, stdPointIdx, estimatedScale, nWidth, roiScale, NORTH));
                        pioneerOneside.second.emplace_back(stdPointIdx);
                    }
                    break;
                case SOUTH: // 동서확장
                    stdPointIdxTemp = stdPointIdx;
                    for (int i = 0; i < std::abs(lengthStdPointIdxDir[EAST].first); i++) {
                        pioneerOneside.first.emplace_back(pioneerOnestep(crossPoints, stdPointIdx, estimatedScale, nWidth, roiScale, EAST));
                        pioneerOneside.second.emplace_back(stdPointIdx);
                    }
                    stdPointIdx = stdPointIdxTemp; 
                    for (int i = 0; i < std::abs(lengthStdPointIdxDir[WEST].first); i++) {
                        pioneerOneside.first.emplace_back(pioneerOnestep(crossPoints, stdPointIdx, estimatedScale, nWidth, roiScale, WEST));
                        pioneerOneside.second.emplace_back(stdPointIdx);
                    }
                    break;
                case NORTH: // 동서확장
                    stdPointIdxTemp = stdPointIdx;
                    for (int i = 0; i < std::abs(lengthStdPointIdxDir[EAST].first); i++) {
                        pioneerOneside.first.emplace_back(pioneerOnestep(crossPoints, stdPointIdx, estimatedScale, nWidth, roiScale, EAST));
                        pioneerOneside.second.emplace_back(stdPointIdx);
                    }
                    stdPointIdx = stdPointIdxTemp;
                    for (int i = 0; i < std::abs(lengthStdPointIdxDir[WEST].first); i++) {
                        pioneerOneside.first.emplace_back(pioneerOnestep(crossPoints, stdPointIdx, estimatedScale, nWidth, roiScale, WEST));
                        pioneerOneside.second.emplace_back(stdPointIdx);
                    }
                    break;
                default:
                    break;
                }

                if (isAllTrue(pioneerOneside.first)) {
                    // 확장
                    switch (dirOnestep) {
                    case EAST: lengthStdPointIdxDir[dirOnestep].first++; break;
                    case WEST: lengthStdPointIdxDir[dirOnestep].first--; break;
                    case SOUTH:lengthStdPointIdxDir[dirOnestep].first++; break;
                    case NORTH:lengthStdPointIdxDir[dirOnestep].first--; break;
                    default: break;
                    }
                    for (auto& it : pioneerOneside.second) {
                        vRow.emplace_back(crossPoints[it]);
                        /*TEST*/cv::circle(showImage, crossPoints[it], 33, 255, 33);
                    }
                    pioneerOneside.first.clear();
                    pioneerOneside.second.clear();
                }
                else {
                    // 해당 사이드는 확장종결
                    pioneerClosedDir[dirOnestep] = true;
                    pioneerOneside.first.clear();
                    pioneerOneside.second.clear();
                }
            }
            else {
                // 해당 사이드는 확장종결
                pioneerClosedDir[dirOnestep] = true;
                pioneerOneside.first.clear();
                pioneerOneside.second.clear();
            }
        }
        //동서남북순회
        dirOnestep == NORTH ? dirOnestep = EAST : dirOnestep = (dir)((int)dirOnestep + 1);
    }

    sortVecPoint(vRow, yxMatrix);
    crossPoints.clear();
    /*TEST*/for (auto& pt : vRow) { cv::circle(showImage, pt, 33, 255, 33); }

    /*------------------------------------3.체커보드추상화------------------------------------*/
    // xy매트릭스 정보
    int nRow(0), nCol(0);
    nRow = yxMatrix.size();
    nCol = yxMatrix[0].size();

    // 그리기
    std::vector<cv::Point2i> polySquare(4), polySquareNext(4);
    bool isWhiteStart, isWhiteNext;
    std::vector<std::pair<std::vector<cv::Point2i>, bool>> checkAreas;

    cv::Mat mask = cv::Mat::zeros(grayImageAbstracted.size(), CV_8UC1);
    cv::bitwise_not(mask, mask);
    showImage = mask.clone();

    for (int r = 0; r < (nRow - 1); r++) {
        polySquare = { yxMatrix[r + 0][0], yxMatrix[r + 0][1], yxMatrix[r + 1][1], yxMatrix[r + 1][0] };
        mask = cv::Mat::zeros(grayImageAbstracted.size(), CV_8UC1);
        cv::fillConvexPoly(mask, polySquare, cv::Scalar(255));
        int meanValue = cv::mean(grayImageAbstracted, mask)[0];

        polySquareNext = { yxMatrix[r + 0][1], yxMatrix[r + 0][2], yxMatrix[r + 1][2], yxMatrix[r + 1][1] };
        mask = cv::Mat::zeros(grayImageAbstracted.size(), CV_8UC1);
        cv::fillConvexPoly(mask, polySquareNext, cv::Scalar(255));
        int meanValueNext = cv::mean(grayImageAbstracted, mask)[0];

        if (meanValue > meanValueNext) {
            isWhiteStart = true;
            checkAreas.emplace_back(make_pair(polySquare, isWhiteStart));
            cv::fillConvexPoly(showImage, polySquare, cvWhite);

            isWhiteNext = false;
            checkAreas.emplace_back(make_pair(polySquareNext, isWhiteNext));
            cv::fillConvexPoly(showImage, polySquareNext, cvBlack);
        }
        else {
            isWhiteStart = false;
            checkAreas.emplace_back(make_pair(polySquare, isWhiteStart));
            cv::fillConvexPoly(showImage, polySquare, cvBlack);

            isWhiteNext = true;
            checkAreas.emplace_back(make_pair(polySquareNext, isWhiteNext));
            cv::fillConvexPoly(showImage, polySquareNext, cvWhite);
        }

        for (int c = 0 + 2; c < (nCol - 1); c++) {
            isWhiteNext = !isWhiteNext;

            polySquare = { yxMatrix[r + 0][c + 0], yxMatrix[r + 0][c + 1], yxMatrix[r + 1][c + 1], yxMatrix[r + 1][c + 0] };
            checkAreas.emplace_back(make_pair(polySquare, isWhiteNext));
            cv::fillConvexPoly(showImage, polySquare, isWhiteNext ? cvWhite : cvBlack);

        }
    }
    grayImageAbstracted = showImage.clone();
    chessboardCornersizes.width = nCol - 1;
    chessboardCornersizes.height = nRow - 1;
    
    //채운 이미지와 nRow-1, nCol-1을 반환

}

void cv_calibration::makeImageCalibration(const std::vector<cv::Mat>& images, std::vector<std::vector<cv::Point2f>>& imagesCorners, cv::Mat& cameraMatrix, cv::Mat& distortionCoefficient, std::vector<cv::Mat>& rotationVectors, std::vector<cv::Mat>& translationVectors) {

    /*--- 변수---*/
    std::vector<cv::Mat> matImageResults; // dst
    cv::Mat matImageGray, matImageBinary, matImageAbstracted; // 연산용
    int nCutHalfTimes = 0;
    cv::Mat matImageView; // 확인용
    std::vector<cv::Point3f> obj3D;
    std::vector<cv::Point2f> img2D;
    std::vector<std::vector<cv::Point3f>> list_obj3D;
    std::vector<std::vector<cv::Point2f>> list_img2D;
    /*TEST*/cv::Scalar red(0, 0, 255), green(0, 255, 0), blue(255, 0, 0);

    /*--- 연산 ---*/
    matImageResults.resize(images.size());

    // 2D && 3D 좌표 세트 구하기 - 모든 이미지 in 각 카메라
    for (int i = 0; i < images.size(); i++) {

        matImageResults[i] = images[i].clone();
        if (matImageResults[i].empty()) continue;
        
        // 체커보드 이미지 추상화 & 규격확인
        cv::cvtColor(images[i], matImageGray, cv::COLOR_BGR2GRAY);
        cv::Size chessboardCornersizes;
        matImageAbstracted = matImageGray.clone();
        makeCheckboard(matImageAbstracted, chessboardCornersizes);
        /*TEST*///continue;

        // Image상의 2D 포인트 구하기 
        bool chessboardFound = false;
        const int howmanytimes = 2;
        for (int r = chessboardCornersizes.height - 1; r > chessboardCornersizes.height - 2 * howmanytimes; r -= 2) {
            for (int c = chessboardCornersizes.width - 1; c > chessboardCornersizes.width - 2 * howmanytimes; c -= 2) {
                nCutHalfTimes = 0;
                chessboardCornersizes = cv::Size(c, r);
                while (matImageGray.cols > 2000000) { // TODO: 압축연산기능
                    nCutHalfTimes++;
                    cv::resize(matImageGray, matImageGray, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);
                    cv::resize(matImageResults[i], matImageResults[i], cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);
                }
                chessboardFound = cv::findChessboardCorners(matImageAbstracted, chessboardCornersizes, img2D, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);
                std::cout << c << ", " << r << " end" << std::endl;
                /*TEST*/cv::drawChessboardCorners(matImageResults[i], chessboardCornersizes, img2D, chessboardFound);
                if (chessboardFound) {
                    /*TEST*///std::cout << i << " of corners has found!" << std::endl;
                    cv::cornerSubPix(matImageGray, img2D, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));

                    /*TEST*/cv::drawChessboardCorners(matImageResults[i], chessboardCornersizes, img2D, chessboardFound);
                    /*TEST*///cv::imwrite("C:\\Projects\\_DNC\\__calibration\\4-2abstract" + std::to_string(i + 10) + ".jpg", matImageResults[i]);
                    break;
                }
            }
            if (chessboardFound)
                break;
            if (!chessboardFound && r == 9) {
                std::cerr << i << " of corners has not found!" << std::endl;
                break; //not find!!
            }
        }
        list_img2D.push_back(img2D);
        img2D.clear();  // 2D 포인트 초기화

        // 실제 Object 3D 포인트
        for (int r = 0; r < chessboardCornersizes.height; r++) {
            for (int c = 0; c < chessboardCornersizes.width; c++) {
                obj3D.push_back(cv::Point3f(c, r, 0));
            }
        }
        list_obj3D.push_back(obj3D);
        obj3D.clear();  // 3D 포인트 초기화

        matImageGray = cv::Mat::zeros(0, 0, CV_8UC3);
        /*TEST*///break; // 사진 1장만 테스트
    }
    /*TEST*///return; // 체스보드검출만 테스트

    // 카메라 캘리브레이션(이미지1장에 대해 검사 rotationVectors[image], translationVectors[image]) 
    // : 3D좌표 세트, 2D좌표 세트, 연산이미지크기, &카메라매트릭스(초점거리/중심좌표), &랜즈왜곡계수, &카메라회전벡터, &카메라위치벡터
    cv::calibrateCamera(list_obj3D, list_img2D, cv::Size(matImageResults[0].rows, matImageResults[0].cols), // TODO: 이미지사이즈 변수화하기
                        cameraMatrix, distortionCoefficient, rotationVectors, translationVectors);

    imagesCorners = list_img2D;
    list_obj3D.clear();
    list_img2D.clear();

    matImageResults.clear();
    
    return;
}

void cv_calibration::undistortImage(const cv::Mat& srcImage, cv::Mat& dstImage, const cv::Mat& cameraMatrix, const cv::Mat& distortionCoefficient) {

    cv::undistort(srcImage, dstImage, cameraMatrix, distortionCoefficient);
}

void cv_calibration::undistortPoint(const std::vector<cv::Point2f>& srcImagesPoints, std::vector<cv::Point2f>& dstImagesPoints, const cv::Mat& cameraMatrix, const cv::Mat& distortionCoefficient) {
    
    cv::undistortPoints(srcImagesPoints, dstImagesPoints, cameraMatrix, distortionCoefficient); // 정규화된 포인트 반환
    for (int i = 0; i < dstImagesPoints.size(); i++) {
        dstImagesPoints[i].x = dstImagesPoints[i].x * cameraMatrix.at<double>(0, 0) + cameraMatrix.at<double>(0, 2);
        dstImagesPoints[i].y = dstImagesPoints[i].y * cameraMatrix.at<double>(1, 1) + cameraMatrix.at<double>(1, 2);
    }
    /*TEST*///cv::Scalar red(0, 0, 255), green(0, 255, 0), blue(255, 0, 0);
    /*TEST*///for (size_t i = 0; i < srcList_img2D[imagePath].size(); ++i) { cv::circle(matImage2Dpoints, dstList_img2D[imagePath][i], 3, red, -1); }
    /*TEST*///cv::imwrite("C:\\Projects\\_DNC\\__calibration\\test\\" + std::to_string(imagePath + 11) + "_corners_undistortpoints.jpg", matImage2Dpoints);    
}
