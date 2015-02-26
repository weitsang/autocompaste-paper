//
//  Processor.cpp
//  ACP
//
//  Created by Raghav Ramesh on 22/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include "Processor.h"

using namespace cv;
using namespace std;

Processor::Processor() {
    
}


void Processor::setPage(Page page) {
    this->page = page;
}


Page Processor::getPage() {
    return page;
}


void Processor::resizeImage(int width, int height) {
    if (page.getImage().size().width != width) {
        cv::resize(page.getImage(), page.getImage(), page.getSizeRes(), 0, 0, INTER_CUBIC);
    } else {
        page.getImage().copyTo(page.getImage());
    }
}


void Processor::prepareImageForOCR() {
    cv::GaussianBlur(page.getImage(), page.getImage(), Size(3, 3), 0, 0);
    cv::adaptiveThreshold(page.getImage(), page.getImage(), 255.f, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 5, 4.1f);
}


void Processor::rotateImageClockwise(double angle) {
    Size srcSize = page.getImage().size();
    Size dstSize(srcSize.height, srcSize.width);
    
    int len = max<int>(page.getImage().cols, page.getImage().rows);
    Point2f center(len/2., len/2.);
    page.setImage(getRotationMatrix2D(center, angle, 1.0));
}


void Processor::initialiseTesseractAPI() {
    tessAPI = new tesseract::TessBaseAPI();
    if(tessAPI->Init(NULL, "eng")) {
        cout << "Could not initialise Tesseract." << endl;
    }
}


string Processor::extractTextFromImage() {
    initialiseTesseractAPI();
    string text;
    tessAPI->SetImage(page.getImage().data, page.getImage().cols, page.getImage().rows,
                      page.getImage().channels(), page.getImage().step);
    text = tessAPI->GetUTF8Text();
    tessAPI->End();
    return text;
}


void Processor::replaceUnwantedCharactersWithSpace(string text) {
    string allowedCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890!@#$%^&*()_+_,.;<> ";
    size_t nonalpha = text.find_first_not_of(allowedCharacters);
    
    while(nonalpha != string::npos) {
        text.at(nonalpha) = ' ';
        nonalpha = text.find_first_not_of(allowedCharacters);
    }
}


Mat Processor::erodeImage(int, void*) {
    int erosionType = MORPH_RECT;
    int erosionSize = 13;
    Mat element = getStructuringElement(erosionType, Size(2*erosionSize+1, 2*erosionSize+1), Point(erosionSize, erosionSize));
    
    cv::erode(this->getPage().getImage(), this->getPage().getImage(), element);
    return this->getPage().getImage();
}


Mat Processor::dilateImage(int, void* ) {
    int dilationType = MORPH_RECT;
    double dilationSize = 1.0;
    Mat element = getStructuringElement( dilationType,
                                        Size( 2*dilationSize, 2*dilationSize),
                                        Point( dilationSize, dilationSize ) );
    /// Apply the dilation operation
    dilate(this->getPage().getImage(), this->getPage().getImage(), element);
    return this->getPage().getImage();
}


vector<Mat> Processor::cutImageGivenWhiteLineLocations(vector<int> whiteLineLocations) {
    vector<Mat> images;
    Mat image = this->getPage().getImage();
    int width = image.size().width;
    sort(whiteLineLocations.begin(), whiteLineLocations.end());

    for (int i = 0, y = 0, height = 0; i < whiteLineLocations.size(); y = whiteLineLocations[i], i++) {
        height = whiteLineLocations[i] - y;
        Rect rect = Rect(0, y, width, height);
        images.push_back(Mat(image, rect));
    }
    return images;
}


vector<int> Processor::findWhiteLines(Mat img) {
    vector<int> whiteLines;
    map<int, int> actualWhiteLines;
    int highest = 0;
    
    for (int i = 0; i < img.rows-1; i++) {
        Scalar s = sum(Mat(img, Rect(0, i, img.cols - 1, 1)));
        Scalar s2 = sum(Mat(img, Rect(0, i+1, img.cols - 1, 1)));
        Scalar s3 = s - s2;
        
        if ((int)s3[0] == 0) {
            highest = (highest < (int)s[0]) ? (int)s[0] : highest;
            actualWhiteLines[i] = (int)s[0];
        }
    }
    
    // Iterate through map to eliminate non white lines and push only white lines
    typedef map<int, int>::iterator it_type;
    for(it_type iterator = actualWhiteLines.begin(); iterator != actualWhiteLines.end(); iterator++) {
        if (iterator->second == highest)
            whiteLines.push_back(iterator->first);
    }
    
    return whiteLines;
}


vector<int> Processor::getSplittingLocations() {
    vector<int> splittingLocations;
    Mat image = this->getPage().getImage();
    vector<int> whiteLines = findWhiteLines(image);
    stack<int> splitter;
    
    splitter.push(whiteLines[0]);
    for (int i = 0; i<whiteLines.size(); i++) {
        if (whiteLines[i+1] - whiteLines[i] > 1) {
            splitter.push(whiteLines[i]);
            splitter.push(whiteLines[i+1]);
        }
    }
    splitter.push(whiteLines[whiteLines.size()-1]);
    
    int a, b;
    while(!splitter.empty()) {
        a = splitter.top();
        splitter.pop();
        b = splitter.top();
        splitter.pop();
        splittingLocations.push_back((a+b)/2);
    }
    return splittingLocations;
}



// Unused
vector<Mat> Processor::cutImage(int x_coord, int y_coord) {
    Mat bigImage = page.getImage();
    Mat smallImage = Mat(bigImage, Rect(0, 0, x_coord, y_coord));
    
    Size smallSize(x_coord, y_coord);
    vector<Mat> smallImages;
    
    for (int y = 0; y < bigImage.rows; y += smallSize.height) {
        for (int x = 0; x < bigImage.cols; x += smallSize.width) {
            Rect rect = Rect(x, y, smallSize.width, smallSize.height);
            smallImages.push_back(Mat(bigImage, rect));
        }
    }
    return smallImages;
}


vector<Rect> Processor::detectLetters(cv::Mat img) {
    std::vector<cv::Rect> boundRect;
    cv::Mat img_gray, img_sobel, img_threshold, element;
    cvtColor(img, img_gray, CV_BGR2GRAY);
    cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
    element = getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3) );
    cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick
    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(img_threshold, contours, 0, 1);
    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
        if (contours[i].size()>100)
        {
            cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
            cv::Rect appRect( boundingRect( cv::Mat(contours_poly[i]) ));
            if (appRect.width>appRect.height)
                boundRect.push_back(appRect);
        }
    return boundRect;
}


void Processor::drawContours(int, void *) {
    Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    int thresh = 100;
    RNG rng(12345);
    
    /// Detect edges using Threshold
    threshold(this->getPage().getImage(), threshold_output, thresh, 255, THRESH_BINARY);
    /// Find contours
    findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    
    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point>> contours_poly( contours.size() );
    vector<Rect> boundRect(contours.size());
    vector<Point2f>center(contours.size());
    vector<float>radius(contours.size());
    
    for(int i = 0; i < contours.size(); i++) {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i]) );
        minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
    }
    
    
    /// Draw polygonal contour + bonding rects + circles
    Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
        cv::Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        cv::drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        cout << "Color:" << color << endl;
        //        circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
    }
    
    /// Show in a window
    namedWindow("Contours", CV_WINDOW_AUTOSIZE);
    imshow("Contours", drawing);
}


void Processor::displayImage(Mat image) {
    imshow("Result", image);
//    warpAffine(page.getImage(), page.getImage(), rotatedMat, dstSize);
}


void Processor::deskewImage(Mat image, double angle, Mat &rotated) {
    bitwise_not(image, image);
    vector<Point> points;
    Mat_<uchar>::iterator iter = image.begin<uchar>();
    Mat_<uchar>::iterator end = image.end<uchar>();
    
    for (; iter != end; ++iter)
        if (*iter)
            points.push_back(iter.pos());
    
    RotatedRect box = minAreaRect(Mat(points));
    Mat rotatedMat = getRotationMatrix2D(box.center, angle, 1);
    warpAffine(image, rotated, rotatedMat, image.size(), INTER_CUBIC);
    Size boxSize = box.size;
    if (box.angle < -45.f)
        swap(boxSize.width, boxSize.height);
    
    Mat cropped;
    getRectSubPix(rotated, boxSize, box.center, rotated);
    bitwise_not(rotated, rotated);
    cout << "Deskewing image.." << endl;
}
