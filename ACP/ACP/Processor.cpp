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

Processor::Processor() {
    
}

void Processor::setPage(Page page) {
    this->page = page;
}

void Processor::resizeImage(int width, int height) {
    if (page.getImage().size().width != page.getWidth()) {
        cv::resize(page.getImage(), page.getImage(), page.getSizeRes(), 0, 0, cv::INTER_CUBIC);
    } else {
        page.getImage().copyTo(page.getImage());
    }
}

int Processor::prepareImageForOCR() {
    cv::GaussianBlur(page.getImage(), page.getImage(), Size(3, 3), 0, 0);
    cv::adaptiveThreshold(page.getImage(), page.getImage(), 255.f, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 5, 4.1f);
    return 1;
}

void Processor::rotateImageClockwise(double angle) {
    Size srcSize = page.getImage().size();
    Size dstSize(srcSize.height, srcSize.width);
    
    int len = std::max<int>(page.getImage().cols, page.getImage().rows);
    Point2f center(len/2., len/2.);
    Mat rotatedMat = cv::getRotationMatrix2D(center, angle, 1.0);
}

void Processor::initialiseTesseractAPI() {
    tessAPI = new tesseract::TessBaseAPI();
    if(tessAPI->Init(NULL, "eng")) {
        cout << "Could not initialise Tesseract." << endl;
    }
}

string Processor::extractTextFromImage() {
    initialiseTesseractAPI();
    string text;
    tessAPI->SetImage(page.getImage().data, page.getImage().cols, page.getImage().rows,
                      page.getImage().channels(), page.getImage().step);
    text = tessAPI->GetUTF8Text();
    tessAPI->End();
    return text;
}

void Processor::replaceUnwantedCharactersWithSpace(string text) {
    size_t nonalpha = text.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890!@#$%^&*()_+_,.;<> ");
    
    while(nonalpha!=std::string::npos) {
        text.at(nonalpha) = ' ';
        nonalpha = text.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890!@#$%^&*()_+_,.;<> ");
    }
}

// Unused
void Processor::displayImage(Mat image) {
    imshow("Result", image);
//    warpAffine(page.getImage(), page.getImage(), rotatedMat, dstSize);
}

void Processor::deskewImage(Mat image, double angle, Mat &rotated) {
    cv::bitwise_not(image, image);
    std::vector<cv::Point> points;
    cv::Mat_<uchar>::iterator iter = image.begin<uchar>();
    cv::Mat_<uchar>::iterator end = image.end<uchar>();
    
    for (; iter != end; ++iter)
        if (*iter)
            points.push_back(iter.pos());
    
    cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));
    cv::Mat rotatedMat = cv::getRotationMatrix2D(box.center, angle, 1);
    cv::warpAffine(image, rotated, rotatedMat, image.size(), cv::INTER_CUBIC);
    cv::Size boxSize = box.size;
    if (box.angle < -45.f)
        std::swap(boxSize.width, boxSize.height);
    
    cv::Mat cropped;
    cv::getRectSubPix(rotated, boxSize, box.center, rotated);
    cv::bitwise_not(rotated, rotated);
    std::cout << "Deskewing image.." << std::endl;
}
