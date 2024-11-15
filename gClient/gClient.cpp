#pragma once
#include "gClient.h"

int main() {
    std::shared_ptr<Data> pData = std::make_shared<Data>();

    //TEST
	    pData->cv_calibrationObj.TEST_cv_calibration();
	try {

	}
	catch (const std::exception& e) {
		e.what();
	}


    return 0;
}
