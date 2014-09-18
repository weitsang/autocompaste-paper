//
//  Camera.cpp
//  AutoComPasteV2
//
//  Created by Raghav Ramesh on 5/9/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include "Camera.h"

int Camera::initCameraFrameSize(int width, int height)
{
	webCam.release();
	webCam.open(0);

	if (!webCam.isOpened())  // check if we succeeded
	{
		printf("cannot find webcam ...\n");
		getchar();
		return(-1);
	}

	webCam.set(CV_CAP_PROP_FRAME_WIDTH, width);
	webCam.set(CV_CAP_PROP_FRAME_HEIGHT, height);

	return 1;
}

Mat Camera :: getWebCamInput()
{
	inputImage;
	webCam >> inputImage;

	return inputImage;
}

Mat Camera :: getImageFileInput()
{
	inputImage = imread("C:\\Users\\Dell\\Desktop\\edwin.png"); // Just for testing purpose change the file location accordingly to read the image 
	return inputImage;
}
