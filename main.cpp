#include "stdafx.h"
#include "main.h"

#define M_PI 3.14159265358979323846

using namespace cv;
using namespace pad;
using namespace std;

Mat mattimage,temp,rotated;
Mat dst,cdst, prev_img;
float deskew_angle;
double as=0;
double *anglle= &as;
int stop=0;
unsigned nb_lines=0;
HANDLE angMutex,thread1,thread2,thread3;
char started=0;
bool progfail = 0;
vector<Mat> camera_Stack;


void deskew(Mat img, double angle,Mat &rotated)
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
    //cv::Mat rotated;
    cv::warpAffine(img, rotated, rot_mat, img.size(), cv::INTER_CUBIC);
    cv::Size box_size = box.size;
    if (box.angle < -45.)
    std::swap(box_size.width, box_size.height);

    cv::Mat cropped;
    cv::getRectSubPix(rotated, box_size, box.center, rotated);
    cv::bitwise_not(rotated, rotated);
    cout<<"deskewing"<<endl;
    //imshow("Rotated", cropped);
    
}

int state = 0;

// Thread 2
// Will terminate if new page detected

void OCR_t(void *param)
{

    tesseract::TessBaseAPI *api= new tesseract::TessBaseAPI();
    if(api->Init(NULL, "eng")) {
        printf("could not init tess\n");
    } 
    api->SetPageSegMode(tesseract::PSM_AUTO_OSD);
    string mytext; 
    Mat img2;

    double skew_angle = 0;

    printf("start");
    WaitForSingleObject(angMutex, INFINITE);
    printf("ing\n");
    mattimage.copyTo(img2);

    skew_angle=*anglle;


    ReleaseMutex(angMutex);
    OCRer myOcr;
            
    sock mySock;

    mySock.connection("5566","{\"name\":\"closeSourceDocument\",\"parameters\":[\"ocr\"]}");
            
        
    myOcr.OCR(img2);

            
    string buffer = myOcr.all_text;
    if(0) {
        for(set<string>::iterator setit=myOcr._dict_sent.begin(); setit!=myOcr._dict_sent.end();++setit) { 
            string temp = *setit;
            buffer +=  temp;
        }
    }

    // If the extracted text does not contain any of the following characters, replace with <space>
    size_t nonalpha = buffer.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890!@#$%^&*()_+_,.;<> ");
                
    while(nonalpha!=std::string::npos) {
        buffer.at(nonalpha) = ' ';
        nonalpha = buffer.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890!@#$%^&*()_+_,.;<> ");
    }
            
    // Send text to ACP
    if(mySock.connection("5566",mySock.toACP(buffer))) {
        state = 3;
        cout<<endl<<"****send fail************"<<endl;
    }

    printf("\n");

    stop=1;
    printf("\n finish\n");

    _endthread();

}

void imgpro_t(void *param)
{
    imgpros mypros;

    int a = 0;
    string filename = ".jpg";
    

    started = 0;

    mypros.init_camera(640, 360);   
    Mat temp;
    mypros.pimage = cvCreateImage(mypros.sizeres, IPL_DEPTH_8U, 1); // Creates an image header and allocates image data
    // Summing images to take average later
    mypros.summage = Mat::zeros(mypros.sizeres, CV_8UC1);

    vector<Mat> camera_stack;

    // Initialise imgpros object values
    // No of frames captured
    mypros.nPasses = 15;
    mypros.gHeight = 640;
    mypros.gWidth = 360;

    while(1) 
    {
        started = cvWaitKey(20);
        WaitForSingleObject(angMutex, INFINITE);
        
        while(camera_stack.size() < 3) {
            // Capture an image from webcam
            mypros.process_main();

            mypros.resizeimage(mypros.mattimage, mypros.mattimage, mypros.gHeight, mypros.gWidth);
            
            // Preparing for OCR
            mypros.process_image(mypros.mattimage);
            mypros.rotate(mypros.mattimage,mypros.mattimage,90);
        
            camera_stack.push_back(mypros.mattimage);

        }

        ReleaseMutex(angMutex);
        //namedWindow("Output", CV_WINDOW_OPENGL| CV_WINDOW_AUTOSIZE);    //for  cuda support
        imshow("Output", camera_stack.back());
        //cvWaitKey(20);
  
        while(!camera_stack.empty()) {
            // Check if page has been flipped or if there is a network error
            if(mypros.compute_skew(camera_stack.back())||state == 32) {
                
                WaitForSingleObject(angMutex, INFINITE);
                
                // High res for OCR
                mypros.init_camera(1920, 1080);
                mypros.process_main();
                
                // Transfer to thread2
                mypros.process_image(mypros.mattimage);
                // Rotates 90 due to the initial position of page under webcam
                mypros.rotate(mypros.mattimage, mattimage, 90);
                //imshow("highres", mattimage);

                // Low res
                mypros.init_camera(640, 360);
                ReleaseMutex(angMutex);
                cout << "o=ocr" << endl;
                TerminateThread(thread2, 0);

                CloseHandle(thread2);
                thread2 = (HANDLE)_beginthread(OCR_t, 0, NULL);
            }

            camera_stack.pop_back();

            // If network error, clear all images in the stack and set state to 0
            if(state == 32) {
                while(!mypros.sumofarray.empty()) {
                    mypros.sumofarray.pop_back();
                }
                while(!camera_stack.empty()) {
                    camera_stack.pop_back();
                }
                state = 0;
            }
        }
    }
}


int main(int argc, char* argv[])
{   
    angMutex = CreateMutex( NULL, FALSE, NULL); // win api
    // Image processing thread
    thread1=(HANDLE)_beginthread(imgpro_t, 0, NULL);

    cout << gpu::getCudaEnabledDeviceCount();

    cvWaitKey(300);

    
    // Infinite loop
    while(1) {}
    
    getchar();
    return(1);
}
