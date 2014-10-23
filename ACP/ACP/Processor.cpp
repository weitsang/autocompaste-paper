//
//  Processor.cpp
//  ACP
//
//  Created by Raghav Ramesh on 22/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include "Processor.h"

using namespace cv;
using namespace std;


void Processor::setPage(Page page) {
    this->page = page;
}

void Processor::resizeImage(cv::Mat src, cv::Mat &dst, int width, int height) {
    if (src.size().width != page.getGWidth()) {
        cv::resize(src, dst, page.getSizeRes(), 0, 0, cv::INTER_CUBIC);
    } else {
        src.copyTo(dst);
    }
}

int Processor::prepareImageForOCR(cv::Mat image) {
    cv::GaussianBlur(image, image, Size(3, 3), 0, 0);
    cv::adaptiveThreshold(image, image, 255.f, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 5, 4.1f);
    return 1;
}

void Processor::rotateImageClockwise(cv::Mat src, cv::Mat &dst, double angle) {
    Size srcSize = src.size();
    Size dstSize(srcSize.height, srcSize.width);
    
    int len = std::max<int>(src.cols, src.rows);
    Point2f center(len/2., len/2.);
    Mat rotatedMat = cv::getRotationMatrix2D(center, angle, 1.0);
    warpAffine(src, dst, rotatedMat, dstSize);
}

void Processor::displayImage(Mat image) {
    imshow("Result", image);
}

void Processor::deskewImage(Mat img, double angle, Mat &rotated) {
    cv::bitwise_not(img, img);
    std::vector<cv::Point> points;
    cv::Mat_<uchar>::iterator iter = img.begin<uchar>();
    cv::Mat_<uchar>::iterator end = img.end<uchar>();
    
    for (; iter != end; ++iter)
        if (*iter)
            points.push_back(iter.pos());
    
    cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));
    cv::Mat rotatedMat = cv::getRotationMatrix2D(box.center, angle, 1);
    cv::warpAffine(img, rotated, rotatedMat, img.size(), cv::INTER_CUBIC);
    cv::Size boxSize = box.size;
    if (box.angle < -45.)
        std::swap(boxSize.width, boxSize.height);
    
    cv::Mat cropped;
    cv::getRectSubPix(rotated, boxSize, box.center, rotated);
    cv::bitwise_not(rotated, rotated);
    std::cout << "deskewing" << std::endl;
}
