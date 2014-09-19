//
//  Page.cpp
//  AutoComPasteV2
//
//  Created by Raghav Ramesh on 5/9/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include "Page.h"

void Page :: displayAfterRotation()
{
	imshow("Page under analysis", pageImage);
}

void Page :: deskewImage(Mat img, double angle, Mat &rotated)
{
	cv::bitwise_not(img, img);
	std::vector<cv::Point> points;
	cv::Mat_<uchar>::iterator it = img.begin<uchar>();
	cv::Mat_<uchar>::iterator end = img.end<uchar>();
	for (; it != end; ++it)
	if (*it)
		points.push_back(it.pos());

	cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));
	cv::Mat rot_mat = cv::getRotationMatrix2D(box.center, angle, 1);
	cv::warpAffine(img, rotated, rot_mat, img.size(), cv::INTER_CUBIC);
	cv::Size box_size = box.size;
	if (box.angle < -45.)
		std::swap(box_size.width, box_size.height);

	cv::Mat cropped;
	cv::getRectSubPix(rotated, box_size, box.center, rotated);
	cv::bitwise_not(rotated, rotated);
	cout << "deskewing" << endl;
}

int Page :: prepareImageForOCR(Mat image)
{
	GaussianBlur(image, image, Size(3, 3), 0, 0);
	adaptiveThreshold(image, image, 255.f, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 5, 4.1f);
	return 1;
}

void Page :: recogniseTextUsingOCR(Mat img)
{
	TessBaseAPI = *tessapi
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

	if (api->Init(NULL, "eng"))	
	{
		printf("could not init tess\n");
	}

	api->SetPageSegMode(tesseract::PSM_AUTO_OSD);

	cv::waitKey(100);

	tessapi->SetImage((const unsigned char*)img.data,img.cols, img.rows,img.channels(), img.step);
	tessapi->Recognize(NULL);

	ResultIterator *ri = tessapi->GetIterator();
	
	if (ri != NULL)
	{
		textResult = "";
		while (ri->Next(RIL_TEXTLINE)) 
		{
			textResult += ri->GetUTF8Text(RIL_TEXTLINE);
		}

		delete ri;

	}

}

string Page :: OCRToString()
{
	return textResult;
}