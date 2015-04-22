//
//  main.cpp
//  ACP
//
//  Created by Raghav Ramesh on 14/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include <iostream>
#include <tesseract/baseapi.h>
#include "Camera.h"
#include "Page.h"
#include "Processor.h"
#include <OpenCL/opencl.h>
#include "AdvancedProcessor.h"

struct threadData {
    int id;
    Processor processor;
    Mat image;
};

void *extractText(void *threadArg) {
    struct threadData *data;
    data = (struct threadData *) threadArg;
    string text = data->processor.extractTextFromImage(data->image);
    cout << text << endl;
    pthread_exit(NULL);
}

int main(int argc, const char *argv[]) {
    
    Camera cam;
    // File name to be changed depending on the machine
    Mat image = cam.getImageFileInput("/Users/raghav/Desktop/12.png");

    // Initialise Page parameters
    Page page;
    page.setImage(image);
    
    // Process image
    Processor processor;
    processor.setPage(page);
    
    // 1. To test without speedup, comment out one of the two methods to obtain correct results
    // Send to OCR
//    string output = processor.extractTextFromImage(image);
//    cout << output << endl;
    // end of 1.
    
    
    // 2. To test with speedup
    vector<int> whiteLineLocations = processor.getSplittingLocations();
    vector<Mat> cutImages = processor.cutImageGivenWhiteLineLocations(whiteLineLocations);


    int numberOfThreads = (int)cutImages.size();
    struct threadData td[50];
    pthread_t threads[50];

    for (int i = 1; i < numberOfThreads; i++) {
        td[i].id = i;
        td[i].processor = processor;
        td[i].image = cutImages[i];
        int statusFail = pthread_create(&threads[i], NULL, extractText, (void*)&td[i]);
        if (statusFail) {
            cout << "Error: Unable to create thread." << statusFail << endl;
            exit(-1);
        }
    }
    pthread_exit(NULL);
    // end of 2.
    
    return 0;
}

