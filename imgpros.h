// Define IMGPROS macro
#ifndef IMGPROS_H_
#define IMGPROS_H_

#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_
#include "stdafx.h"

// Import OpenCV libraries 
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/gpu/gpu.hpp>

// Import Tesseract API 
#include <tesseract\baseapi.h>
#include <leptonica\allheaders.h>

// Import standard C++ headers
#include <iostream>
#include <string>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <math.h>
#include <queue>
#include <omp.h>



#define M_PI 3.14159265358979323846


using namespace cv;
//using namespace pad;
using namespace std;
using namespace cv::gpu;

class imgpros{

public:
    bool movement;
    int blankscrn;
    int startocr;
    int prntcounter;

    GpuMat gsummage;
    GpuMat zeros;
    Mat summage;
    double startt;
    CvSize sizeres;
    IplImage *image;                    //input image from webcam stored here
    IplImage *pimage;
    Mat mattimage,orig;
    GpuMat gmattimage;
    CvCapture* p_Webcam;

    VideoCapture webCam;

    double angle;
    unsigned int *nb_lines;
    vector<Mat> sumofarray;
    vector<Mat> difarray;

    vector<GpuMat> sumofgarray;
    vector<GpuMat> difgarray;


    int gWidth;
    int gHeight;
    int nPasses;

    imgpros();

    void process_main();
    int init_camera(int width,int height);
    int process_image(Mat image);
    void deskew(Mat img, double angle,Mat &rotated);
    void display();
    
    void display(Mat src);
    void rotate(Mat src,Mat &dst,double angle);
    void callines(Mat src, Mat &dst,int thresh,unsigned int *nb,int thickness,double &Fangle,double acute);
    int compute_skew(const Mat src);
    void resizeimage(Mat src,Mat &dst,int width,int height);
    bool imagesum(Mat src,int size,vector<Mat> &Tarray);
    
    void cvFillHoles(cv::Mat &input);
    double howwhite(Mat img);
    void FindBlobs(const cv::Mat &binary, std::vector < std::vector<cv::Point2i> > &blobs);

    //gpu
    void display(GpuMat src);
    void rotate(GpuMat src,GpuMat &dst,double angle);
    void callines(GpuMat src, GpuMat &dst,int thresh,unsigned int *nb,int thickness,double &Fangle,double acute);
    int compute_skewg(GpuMat src);
    void resizeimage(GpuMat src,GpuMat &dst,int width,int height);
    bool imagesum(GpuMat src,int size,vector<GpuMat> &Tarray);
    

};


#endif
