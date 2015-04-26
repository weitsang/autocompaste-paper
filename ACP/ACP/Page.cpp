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

Mat Page::getImage() {
    return this->image;
}

void Page::setSizeRes(CvSize value) {
    this->sizeRes = value;
}

CvSize Page::getSizeRes() {
    return sizeRes;
}

void Page::setNumOfFrames(int count) {
    this->numOfFrames = count;
}

void Page::setSumOfImages(Mat sumOfImages) {
    this->sumOfImages = sumOfImages;
}

double page::getWhitePixelCount(){

	return whitePixelCount;
}

Mat page::getFrame(){

	return Frame;
}

int page::getNumberOfLines(){

	return numberOfLines;
}

vector<int> page::getLineLength(){
	return lineLength;

}
vector<int> page::getWhiteSpaceHeight(){
	return whiteSpaceHeight;
}

