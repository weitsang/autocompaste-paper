//
//  Document.cpp
//  ACP
//
//  Created by Raghav Ramesh on 30/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include "Document.h"

Document::Document() {
    
}

void Document::processPage(page &currentPage){
	cv::Size size = currentPage.getFrame().size();
	Mat temp1 = Mat::zeros(size, currentPage.getFrame().channels());
	Mat temp2 = Mat::zeros(size, currentPage.getFrame().channels());


	double skewAngle = currentPage.computeSkew();
	currentPage.deskew(temp1, skewAngle);
	imwrite("deskew.jpg", temp1);

	
	currentPage.computeHoughLine(temp1, temp2);
	imwrite("hough.jpg", temp2);
	currentPage.computeContour(temp2);

}

bool Document::processLineLength(vector<int> Frame1, vector<int> Frame2){
	int lineCount = Frame1.size();
	double matchCount = 0.0;

	for (int i = 0; i < lineCount; i++){
		
		int largerLength = Frame1[i];
		int smallerLength = Frame2[i];

		if (Frame2[i] > Frame1[i]){
			largerLength = Frame2[i];
			smallerLength = Frame1[i];
		}

		double percentageError = 0.20*largerLength;
		double startRange = largerLength - percentageError;
		double endRange = largerLength + percentageError;

		if (smallerLength >= startRange && smallerLength <= endRange){
			matchCount++;
		}
	}

	double matchRatio = matchCount / lineCount;

	if (matchRatio >= 0.7){
		return true;
	}
	else
		return false;

}

bool Document :: checkFlip(page initialFrame, page nextFrame){


	processPage(initialFrame);

	
	processPage(nextFrame);

	

	double errorRatio;
	if (initialFrame.getWhitePixelCount() > nextFrame.getWhitePixelCount())
		errorRatio = nextFrame.getWhitePixelCount() / initialFrame.getWhitePixelCount();
	else
		errorRatio = initialFrame.getWhitePixelCount() / nextFrame.getWhitePixelCount();

	// pixel count match
	if (errorRatio < 0.70){
		return true;
	}

	//check for number of lines match
	if (initialFrame.getNumberOfLines() != nextFrame.getNumberOfLines()){
		return true;
	}

	vector<int> Frame1 = initialFrame.getLineLength();
	vector<int> Frame2 = nextFrame.getLineLength();

	if (!processLineLength(Frame1, Frame2)){
		return true;
	}
	
	return false;
}

void Document::checkIfPageHasBeenFlipped(Page previous, Page current) {
    
}


