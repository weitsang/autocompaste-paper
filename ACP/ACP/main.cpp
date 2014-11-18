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
    
    Processor processor;
    
    // OCR Thread
    string output = processor.extractTextFromImage();
    processor.replaceUnwantedCharactersWithSpace(output);
    
    
    waitKey(0);
    
    return 0;
}

