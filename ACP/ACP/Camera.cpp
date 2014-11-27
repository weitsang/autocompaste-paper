//
//  Camera.cpp
//  ACP
//
//  Created by Raghav Ramesh on 14/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include "Camera.h"

Camera::Camera() {
	camera.release();
	camera.open(0);
    
	if (!camera.isOpened()) {
		printf("Unable to find video capture device..\n");
	}
}

void Camera :: setCameraDimensions(int width, int height) {
    camera.set(CV_CAP_PROP_FRAME_WIDTH, width);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT, height);
}

Mat Camera :: getCameraInput() {
    camera >> inputImage;
    return inputImage;
}

Mat Camera :: getImageFileInput(string filepath) {
	inputImage = imread(filepath, CV_LOAD_IMAGE_GRAYSCALE);
	return inputImage;
}
