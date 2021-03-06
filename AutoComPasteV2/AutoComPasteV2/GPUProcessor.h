//
//  GPUProcessor.h
//  AutoComPasteV2
//
//  Created by Raghav Ramesh on 5/9/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#ifndef __AutoComPasteV2__GPUProcessor__
#define __AutoComPasteV2__GPUProcessor__

#include <iostream>

"""Displays the image in the specified window"""
void displayImageInWindow(GpuMat src);

"""Applies affine transformation the src image contained in a GpuMat object given the angle of rotation"""
void rotateClockwise(double angle);

"""Stores frames for averaging"""
bool storeFramesForAveraging(GpuMat src, int size, vector<GpuMat> &Tarray);

"""Returns a GpuMat object with an image of a different resolution"""
void resizeImage(int width, int height);

"""Calculates and draws lines with hough transform"""
void applyHoughTransform(GpuMat src, GpuMat &dst, int threshold, unsigned int *nb, int thickness, double &Fangle, double acute);

"""To calculate the skew angle of text in an image"""
int computeSkewTextInImage(GpuMat src);

#endif /* defined(__AutoComPasteV2__GPUProcessor__) */
