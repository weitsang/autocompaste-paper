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
    Mat image = cam.getImageFileInput("/Users/raghav/Desktop/4.png");
    
    // Initialise Page parameters
    Page page;
    page.setImage(image);
    
    // Process image
    Processor processor;
    processor.setPage(page);
    
    // Dilate image - remove blemishes
//    image = processor.dilateImage(0, 0);
    // Erode image - creates blobs in place of paragraphs
//    image = processor.erodeImage(0, 0);
    
    // Find white lines
    processor.findWhiteLines(image);
    
// For image 1
//    cv::line(image, cv::Point(0, 14), cv::Point(690, 14), cv::Scalar(0, 0, 200), 2, CV_AA);
//    cv::line(image, cv::Point(0, 48), cv::Point(690, 48), cv::Scalar(0, 0, 200), 2, CV_AA);
//    cv::line(image, cv::Point(0, 126), cv::Point(690, 126), cv::Scalar(0, 0, 200), 2, CV_AA);
//    cv::line(image, cv::Point(0, 576), cv::Point(690, 576), cv::Scalar(0, 0, 200), 2, CV_AA);
//    cv::line(image, cv::Point(0, 590), cv::Point(690, 590), cv::Scalar(0, 0, 200), 2, CV_AA);
//    cv::line(image, cv::Point(0, 624), cv::Point(690, 624), cv::Scalar(0, 0, 200), 2, CV_AA);

//  Image 2 and 3 are not white enough

//  For image 4
    cv::line(image, cv::Point(0, 61), cv::Point(847, 61), cv::Scalar(0, 0, 200), 2, CV_AA);
    cv::line(image, cv::Point(0, 104), cv::Point(847, 104), cv::Scalar(0, 0, 200), 2, CV_AA);
    cv::line(image, cv::Point(0, 133), cv::Point(847, 133), cv::Scalar(0, 0, 200), 2, CV_AA);
    cv::line(image, cv::Point(0, 152), cv::Point(847, 152), cv::Scalar(0, 0, 200), 2, CV_AA);
    cv::line(image, cv::Point(0, 188), cv::Point(847, 188), cv::Scalar(0, 0, 200), 2, CV_AA);
    
    imshow("Show", image);
//    cout << "Image size: " << image.size() << endl;

//    // Draw contours - draws boundaries around each letter, and makes the background black - less desirable
//    processor.drawContours(0, 0);
//    imshow("Borders", image);
    
//    // Cutting image(Not really) - draws boundaries around characters in a line - more desirable
//    // Read
//    Mat img1 = imread("/Users/raghav/Desktop/2.jpg");
//    
//    // Detect
//    vector<cv::Rect> letterBBoxes1 = processor.detectLetters(img1);
//    
//    // Display
//    for (int i = 0; i < letterBBoxes1.size(); i++) {
//        rectangle(img1, letterBBoxes1[i], cv::Scalar(0,255,0),3,8,0);
//    }
//
//    namedWindow("Contours", CV_WINDOW_AUTOSIZE);
//    imshow("Contours", img1);
    
//    // Cut image
//    vector<Mat> images = processor.cutImage(230, 110);
//    imshow("First small image", images[0]);
    cv::waitKey(0);

//    processor.prepareImageForOCR();
    
//    // Send to OCR
//    string output = processor.extractTextFromImage();
//    processor.replaceUnwantedCharactersWithSpace(output);
    
//    cout << output << endl;
//    processor.resizeImage(600, 400);
//    cout << processor.getPage().getImage().size().width << endl;
    return 0;
}

