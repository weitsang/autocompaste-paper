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
    Mat image = cam.getImageFileInput("/Users/raghav/Desktop/11.png");

    // Initialise Page parameters
    Page page;
    page.setImage(image);
    
    // Process image
    Processor processor;
    processor.setPage(page);
    
    vector<int> whiteLineLocations = processor.getSplittingLocations();
    vector<Mat> cutImages = processor.cutImageGivenWhiteLineLocations(whiteLineLocations);
    
//    for (int i = 0; i < cutImages.size(); i++) {
//        cout << cutImages.size() << endl;
//        imshow("Image", cutImages[i]);
//        waitKey();
//    }
    // Send to OCR
//    string output = processor.extractTextFromImage(image);
//    cout << output << endl;


    int numberOfThreads = (int)cutImages.size();
    struct threadData td[22];
    pthread_t threads[22];

    for (int i = 1; i < numberOfThreads; i++) {
        td[i].id = i;
        td[i].processor = processor;
        td[i].image = cutImages[i];
        int statusFail = pthread_create(&threads[i], NULL, extractText, (void*)&td[i]);
        if (statusFail) {
            cout << "Error: Unable to create thread," << statusFail << endl;
            exit(-1);
        }
    }
    pthread_exit(NULL);
    
    return 0;
}

