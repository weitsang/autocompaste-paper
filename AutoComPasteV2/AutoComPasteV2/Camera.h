//
//  Camera.h
//  AutoComPasteV2
//
//  Created by Raghav Ramesh on 5/9/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#ifndef __AutoComPasteV2__Camera__
#define __AutoComPasteV2__Camera__

#include <iostream>

"""Initiate identifiers"""
Camera();

"""Returns a Mat object of a frame captured from webcam and converts to colour space - Corresponds to process_main() in imgpros"""
Mat getWebCamInput();

"""Returns a Mat object of an image from a file"""
Mat getImageFileInput();

"""Initialise webcam with frame width and height"""
int initCameraFrameSize(int width, int height);

#endif /* defined(__AutoComPasteV2__Camera__) */
