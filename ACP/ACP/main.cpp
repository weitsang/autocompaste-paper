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
#include <OpenCL/opencl.h>
//#include "AdvancedProcessor.cpp"

int main(int argc, const char *argv[]) {
    
    Camera cam;
    Mat image = cam.getImageFileInput("/Users/raghav/Desktop/4.png");

    // Initialise Page parameters
    Page page;
    page.setImage(image);
    
    // Process image
    Processor processor;
    processor.setPage(page);
    
    vector<int> whiteLineLocations = processor.getSplittingLocations();
    vector<Mat> cutImages = processor.cutImageGivenWhiteLineLocations(whiteLineLocations);
    // Cut image
//    vector<Mat> images = processor.cutImage(230, 110);
    cout << "Does the vector contain anything? " << cutImages.size() << endl;
    
    imshow("First cut image", cutImages[3]);
    
    waitKey();
    
//    threshold(image, image, 56, 255, THRESH_BINARY);
//    image = processor.dilateImage(0, 0);
    
    
//    processor.prepareImageForOCR();
    
//    // Send to OCR
//    string output = processor.extractTextFromImage();
//    processor.replaceUnwantedCharactersWithSpace(output);
    
//    cout << output << endl;
//    processor.resizeImage(600, 400);
//    cout << processor.getPage().getImage().size().width << endl;
    return 0;
}

