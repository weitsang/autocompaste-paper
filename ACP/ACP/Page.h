//
//  Page.h
//  ACP
//
//  Created by Raghav Ramesh on 17/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#ifndef __ACP__Page__
#define __ACP__Page__

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


class Page {

private:
    IplImage *image; // Stores camera/image input
    Mat summage;
    int gWidth;
    int gHeight;
    int nPasses;
    CvSize sizeRes;

public:
    Page();
    
    void setImage(IplImage *image);
    void setSummage(Mat summage);
    void setGWidth(int value);
    void setGHeight(int value);
    void setNPasses(int value);
    void setSizeRes(CvSize value);
    
    CvSize getSizeRes();
    int getGWidth();
    int getGHeight();
    
    void resizeImage(Mat src, Mat &dst);
};

#endif /* defined(__ACP__Page__) */
