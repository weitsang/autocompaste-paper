//
//  Page.cpp
//  ACP
//
//  Created by Raghav Ramesh on 17/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include "Page.h"

    
void Page::setSummage(Mat summage) {
    this->summage = summage;
}

void Page::setImage(IplImage *image) {
    this->image = image;
}

void Page::setNPasses(int value) {
    this->nPasses = value;
}
    
void Page::setGHeight(int value) {
    this->gHeight = value;
}
    
void Page::setGWidth(int value) {
    this->gWidth = value;
}

void Page::setSizeRes(CvSize value) {
    this->sizeRes = value;
}

CvSize Page::getSizeRes() {
    return sizeRes;
}

int Page::getGWidth() {
    return this->gWidth;
}

int Page::getGHeight() {
    return this->gHeight;
}