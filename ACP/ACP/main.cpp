//
//  main.cpp
//  ACP
//
//  Created by Raghav Ramesh on 14/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include <iostream>
#include "Camera.h"
#include "Page.h"
#include "Processor.h"

int main(int argc, const char *argv[]) {
    
    // Image processing thread
    
    // Initialise camera
    Camera cam;
    cam.setWebcamDimensions(640, 360);
    
    // cv::imshow("Webcam", cam.getWebCamInput());
    // cv::imshow("Image", cam.getImageFileInput("/Users/raghav/Desktop/image.jpeg"));

    // Initialise Page parameters
    Page page;
    page.setSizeRes(cvSize(640, 360));
    page.setImage(cvCreateImage(page.getSizeRes(), IPL_DEPTH_8U, 1));
    page.setSummage(Mat::zeros(page.getSizeRes(), CV_8UC1));
    page.setGHeight(640);
    page.setGWidth(360);
    page.setNPasses(15);
    
    vector<Mat> cameraStack;
    
    while (1) {
        while (cameraStack.size() < 3) {
            cam.getWebCamInput();
            Processor processor;
            processor.setPage(page);
//            processor.resizeImage(, <#cv::Mat &dst#>, <#int width#>, <#int height#>);
            
        }
    }
    waitKey(0);
    
    return 0;
}

