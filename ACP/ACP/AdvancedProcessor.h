//
//  AdvancedProcessor.h
//  ACP
//
//  Created by Raghav Ramesh on 14/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <OpenCL/opencl.h>
#include <tesseract/baseapi.h>
#include <opencv2/opencv.hpp>

class AdvancedProcessor {
    int processAcrossCores(int argc, char** argv);
};
