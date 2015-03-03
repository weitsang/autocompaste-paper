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
    cout << data->processor.extractTextFromImage(data->image) << endl;
    
    pthread_exit(NULL);
}

int main(int argc, const char *argv[]) {
    
    Camera cam;
    Mat image = cam.getImageFileInput("/Users/raghav/Desktop/5.jpg");

    // Initialise Page parameters
    Page page;
    page.setImage(image);
    
    // Process image
    Processor processor;
    processor.setPage(page);
    
    vector<int> whiteLineLocations = processor.getSplittingLocations();

//    vector<Mat> cutImages = processor.cutImageGivenWhiteLineLocations(whiteLineLocations);
//    cout << "Does the vector contain anything? " << cutImages.size() << endl;
//    imshow("First cut image", cutImages[3]);
    imshow("image", image);
    waitKey();
    
//    threshold(image, image, 56, 255, THRESH_BINARY);
//    image = processor.dilateImage(0, 0);
    
//    processor.prepareImageForOCR();
    
//    // Send to OCR
//    string output = processor.extractTextFromImage();
//    processor.replaceUnwantedCharactersWithSpace(output);
    
//    cout << output << endl;
//    processor.resizeImage(600, 400);
//    cout << processor.getPage().getImage().size().width << endl;
    
    int numberOfThreads = (int)cutImages.size();
    struct threadData td[10];
    pthread_t threads[numberOfThreads];
    

    for (int i = 0; i < numberOfThreads; i++) {
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

