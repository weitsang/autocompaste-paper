//
//  Camera.h
//  ACP
//
//  Created by Raghav Ramesh on 14/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#ifndef __ACP__Camera__
#define __ACP__Camera__

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


class Camera {
    
private:
	VideoCapture camera;
	Mat inputImage;
    
public:
	Camera();
    void setCameraDimensions(int width, int height);
	Mat getCameraInput();
	Mat getImageFileInput(string filepath);
};

#endif /* defined(__ACP__Camera__) */
