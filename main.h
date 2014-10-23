#ifndef _CAMINST_H_
#define _CAMINST_H_

#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_
#undef min
#undef max

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>


//#include <opencv2/video/tracking.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>


#include <cuda.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <process.h>
#include <iostream>
#include <string>
#include <conio.h>

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <math.h>
#include <queue>
#include <conio.h>
#include "Imgpros.h"
//#include "GImgpros.h"
#include "OCRer.h"
#include "sock.h"

#define NOMINMAX
#include <winsock2.h>
#include <windows.h>


#define M_PI 3.14159265358979323846



using namespace cv;
//using namespace pad;
using namespace cv::gpu;
using namespace std;


class cam_Inst{
private:
    IplImage* image;                    //input image from webcam stored here 
    // cvLoadImage provides a pointer to an IplImage, which means it creates an IplImage when it loads it and returns you it's emplacement.
    IplImage* pimage;

    Mat mattimage,temp,rotated;
    Mat dst,cdst, prev_img;
    float deskew_angle;
    double as;
    double *anglle;
    int stop;
    unsigned nb_lines;
    //HANDLE angMutex,thread1,thread2;
    char started;


public:
    cam_Inst();
    int run_avg(vector<int> &myqueue, int size);
    bool ConnectToHost(int PortNo, char* IPAddress);
    void CloseConnection();
    int process_image(Mat image);
    void featuredetect(Mat img1,Mat img2 );
    void featuredetect();
    void featuredetect2(Mat img1, Mat img2);
    void compute_skew3(Mat src,double &angle);
    void compute_skew2(Mat img,double angle);
    void compute_skew(Mat src,double &angle);
    void deskew(Mat img, double angle,Mat &rotated);
    int init_camera(CvCapture *&p_Webcam,int width,int height);
    //void angle_t(void *param);
    //void skew_t(void *param);
    //void imgpro_t(void *param);

    int* get_boundingbox(IplImage* img_input);
    Mat ROIimage(IplImage * img1,int *boundary);
};



#endif
