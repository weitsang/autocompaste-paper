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
    // Image parameters
    Mat image;
    int imageWidth;
    int imageHeight;
    Mat sumOfImages;
    int numOfFrames;
    CvSize sizeRes;

public:
    Page();
    
    void setImage(Mat image);
    Mat getImage();
    
    void setSizeRes(CvSize value);
    CvSize getSizeRes();
    
    void setSumOfImages(Mat sumOfImages);
    void setNumOfFrames(int count);
    
    void resizeImage(Mat src, Mat &dst);
};

#endif /* defined(__ACP__Page__) */
