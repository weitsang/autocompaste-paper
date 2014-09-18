//
//  Page.h
//  AutoComPasteV2
//
//  Created by Raghav Ramesh on 5/9/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#ifndef __AutoComPasteV2__Page__
#define __AutoComPasteV2__Page__

#include <iostream>

class Page {
	Processor processor;
public:

	//Initiate identifiers
	Page();

	//Displays the image after rotation
	void displayAfterRotation();

	//To rotate the image to compensate for skewing
	void deskewImage(Mat img, double angle, Mat &rotated);

	//Handles phase 1 of image processing - applying morphological operations
	int prepareImageForOCR(Mat image);
	
	//Set CPU/GPU
	void setProcessor(Processor p);

	//Extracts text from image and stores as a string
	void recogniseTextUsingOCR(Mat img); 

	//Returns string result of OCR
	string OCRToString();
}

#endif /* defined(__AutoComPasteV2__Page__) */
