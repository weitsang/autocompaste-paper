//
//  Page.cpp
//  ACP
//
//  Created by Raghav Ramesh on 17/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include "Page.h"

Page::Page() {
    
}

void Page::setImage(Mat image) {
    this->image = image;
}

void Page::setImageHeader(IplImage *imageHeader) {
    this->imageHeader = imageHeader;
}

void Page::setHeight(int value) {
    this->height = value;
}

void Page::setWidth(int value) {
    this->width = value;
}

void Page::setNumOfFrames(int count) {
    this->numOfFrames = count;
}

void Page::setSumOfImages(Mat sumOfImages) {
    this->sumOfImages = sumOfImages;
}

void Page::setSizeRes(CvSize value) {
    this->sizeRes = value;
}


CvSize Page::getSizeRes() {
    return sizeRes;
}

Mat Page::getImage() {
    return this->image;
}

int Page::getWidth() {
    return this->width;
}

int Page::getHeight() {
    return this->height;
}
