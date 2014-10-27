//
//  Camera.cpp
//  ACP
//
//  Created by Raghav Ramesh on 14/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include "Camera.h"

Camera::Camera() {
	webCam.release();
	webCam.open(0);
    
	if (!webCam.isOpened()) {
		printf("Unable to find webcam.\n");
		getchar();
	}
}

void Camera :: setWebcamDimensions(int width, int height) {
    webCam.set(CV_CAP_PROP_FRAME_WIDTH, width);
    webCam.set(CV_CAP_PROP_FRAME_HEIGHT, height);
}

Mat Camera :: getWebCamInput() {
    webCam >> inputImage;
    inputImage = inputImage;
    return inputImage;
}

Mat Camera :: getImageFileInput(string filepath) {
	inputImage = imread(filepath);
	return inputImage;
}
