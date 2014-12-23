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
    
    Camera cam;
    Mat image = cam.getImageFileInput("/Users/raghav/Desktop/2.jpg");
    
    // Erode paragraphs
    
    
    // Initialise Page parameters
    Page page;
    page.setImage(image);
    
    // Process image
    Processor processor;
    processor.setPage(page);
    processor.prepareImageForOCR();
    processor.erodeImage(0, 0);
    processor.drawContours(0, 0);
    cv::waitKey(0);
    
    // Send to OCR
    string output = processor.extractTextFromImage();
    processor.replaceUnwantedCharactersWithSpace(output);
    
    cout << output << endl;
    
    return 0;
}

