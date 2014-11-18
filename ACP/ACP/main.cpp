//
//  main.cpp
//  ACP
//
//  Created by Raghav Ramesh on 14/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include <iostream>
#include <tesseract/baseapi.h>
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
    page.setImageHeader(cvCreateImage(page.getSizeRes(), IPL_DEPTH_8U, 1));
    page.setSumOfImages(Mat::zeros(page.getSizeRes(), CV_8UC1));
    page.setHeight(640);
    page.setWidth(360);
    page.setNumOfFrames(15);
    
    vector<Mat> cameraStack;
    
    Processor processor;
    
    while (1) {
        while (cameraStack.size() < 3) {
            cam.getWebCamInput();

            processor.setPage(page);
            processor.resizeImage(page.getHeight(), page.getWidth());
            processor.prepareImageForOCR();
            processor.rotateImageClockwise(90);
            cameraStack.push_back(page.getImage());
        }
        
        imshow("Output", cameraStack.back());
        
        while(!cameraStack.empty()) {
            // Check if page has been flipped
            // computeSkew()
        }
    }
    
    // OCR Thread
    string output = processor.extractTextFromImage();
    processor.replaceUnwantedCharactersWithSpace(output);
    
    
    waitKey(0);
    
    return 0;
}

