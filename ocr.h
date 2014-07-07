#ifndef OCRER_H
#define OCRER_H

#include <iostream>
#include <set>
#include <vector>
#include <tesseract/baseapi.h>
#include <tesseract/resultiterator.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>


#define MAX_N_IMGS 10                       // max number of images to average
// next 3 related to feature extraction
#define FEATURE_QUALITY .2f                 // feature points (corners) with quality measure <
                                            // 0.2 * (best corner's quality measure) are rejected
#define N_CORNERS 200                       // max number of features (corners) to look for in images
#define MIN_DISTANCE 4.f                    // min possible Euclidean distance between the corners
#define MOTION_P_THRESHOLD .2f              // if >20% features are not found in the next image, consider motion


namespace pad {
    using namespace std;
    using namespace tesseract;
    using namespace cv;

    class OCRer {
    public:
        OCRer();
        ~OCRer();
        void process_image(unsigned char *img_data,
                           int bytes_per_pixel, int bytes_per_line,
                           int left, int top, int width, int height);
        void process_image(Mat img);

        void OCR(Mat img);
        string OCR_text();
        set<string>        _dict_words;

        set<string>        _sentences;
        set<string>        _dict_sent;
    string all_text;
    private:
        TessBaseAPI       *_tessapi;
        
        set<string>        _live_words;
        set<char*>        _all_words;
        Mat       _average_img;
        Mat       _last_img;
        int       _n_imgs;
        int       _last_n_features;
        bool      _motion;
        char* ocr_text;
        
    };
}

#endif
