//
//  Processor.h
//  ACP
//
//  Created by Raghav Ramesh on 22/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#ifndef __ACP__Processor__
#define __ACP__Processor__

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include "Page.h"


using namespace cv;
using namespace std;


class Processor {
private:
    Page page;
    tesseract::TessBaseAPI *tessAPI;
public:
    Processor();
    void setPage(Page newPage);
    void resizeImage(int width, int height);
    int prepareImageForOCR();
    void rotateImageClockwise(double angle);
    
    void displayImage(Mat image);
    void displayImageAfterRotation(Mat image, double angle);
    void deskewImage(Mat img, double angle, Mat &rotated);
    
    void initialiseTesseractAPI();
    string extractTextFromImage();
    void replaceUnwantedCharactersWithSpace(string text);
};
#endif /* defined(__ACP__Processor__) */
