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
public:
    Processor();
    void setPage(Page newPage);
    void resizeImage(Mat src, Mat&dst, int width, int height);
    int prepareImageForOCR(Mat image);
    void rotateImageClockwise(Mat src, Mat &dst, double angle);
    void displayImage(Mat image);
    void displayImageAfterRotation(Mat image, double angle);
    void deskewImage(Mat img, double angle, Mat &rotated);
};
#endif /* defined(__ACP__Processor__) */