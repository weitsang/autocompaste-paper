//
//  Processor.cpp
//  AutoComPasteV2
//
//  Created by Raghav Ramesh on 5/9/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include "Processor.h"

public class Processor {
    """Displays the image in the specified window"""
	virtual void displayImageInWindow(Mat src) = 0;
    
	"""Applies affine transformation the src image contained in a Mat object given the angle of rotation"""
	virtual void rotateClockwise(Mat src, Mat &dst, double angle) = 0;
	
	"""Stores frames for averaging"""
	virtual bool storeFramesForAveraging(Mat src, int size, vector<Mat> &Tarray) = 0;
    
	"""Returns a Mat object with an image of a different resolution"""
	virtual void resizeImage(int width, int height) = 0;
    
	"""Calculates and draws lines with hough transform"""
	virtual void applyHoughTransform(Mat src, Mat &dst, int threshold, unsigned int *nb, int thickness, double &Fangle, double acute) = 0;
    
	"""To calculate skew angle of text in image"""
	virtual int computeSkewTextInImage(const Mat src) = 0;
    
	"""Finds and stores large isolated blobs within an image"""
	virtual void findAndStoreBlobs(const cv::Mat &binary, std::vector<std::vector<cv::Point2i>> &blobs) = 0;
    
	"""Returns ratio of white content to black"""
	virtual double getWhiteContentRatio(Mat img) = 0;
    
	"""Fills the image with to contrast lines"""
	virtual void fillHolesWithSpecifiedColor(cv::Mat &input) = 0;
};