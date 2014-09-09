//
//  Document.h
//  AutoComPasteV2
//
//  Created by Raghav Ramesh on 5/9/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#ifndef __AutoComPasteV2__Document__
#define __AutoComPasteV2__Document__

#include <iostream>

"""Displays the image in the specified window"""
virtual void display(Mat src);

"""Applies affine transformation the src image contained in a Mat object given the angle of rotation"""
virtual void rotate(Mat src, Mat &dst, double angle);

"""Stores frames for averaging"""
virtual bool storeFramesForAveraging(Mat src, int size, vector<Mat> &Tarray);

"""Returns a Mat object with an image of a different resolution"""
virtual void resizeImage(int width, int height);

"""Calculates and draws lines with hough transform"""
virtual void applyHoughTransform(Mat src, Mat &dst, int threshold, unsigned int *nb, int thickness, double &Fangle, double acute);

"""To calculate the skew angle of text in an image"""
virtual int computeSkewOfTextInImage(const Mat src);

"""Finds and stores large isolated blobs within an image"""
virtual void findAndStoreBlobs(const cv::Mat &binary, std::vector<std::vector<cv::Point2i>> &blobs);

"""Returns ratio of white content to black"""
virtual double getWhiteContentRatio(Mat img);

"""Fills the image with to contrast lines"""
virtual void fillHolesWithSpecifiedColor(cv::Mat &input);

#endif /* defined(__AutoComPasteV2__Document__) */
