//
//  Main.h
//  AutoComPasteV2
//
//  Created by Raghav Ramesh on 5/9/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#ifndef __AutoComPasteV2__Main__
#define __AutoComPasteV2__Main__

#include <iostream>

// Begin image processing thread
Camera cam;
cam.initCamera(640, 360);

Document doc = new Document();
while(1) {
    // Get input from webcam or image file
    Mat image = cam.getWebCamInput();
    // Mat image = cam.getImageFileInput();
    
    bool isImageInput;
    
    Page page = new Page();
    
    // If using CPU
    page.setProcessor(new CPUProcessor(image));
    // Else if using GPU
    page.setProcessor(new GPUProcessor(image));
    
    doc.storeFlippedPage(page);
    
    Networks nwk = new Networks();
    
    page.resizeImage(cam.gHeight, cam.gWidth);
    page.prepareImageforOCR();
    page.rotateClockwise(90);
    
    if(imageIsFromCamera) {
        if(doc.hasFlipped(cam.image) || sendError()) {
            doc.deleteCache();
            page.OCR(cam.image);
            string sampleText = page.OCRtoString();
            string pageText;
            if(doc.EBookHasBeenDownloaded(sampleText)) {
                pageText = doc.extractPageFromDownloadedEBook(sampleText);
            } else if(doc.EBookVersionExistOnline(text)) {
                doc.downloadEBook();
                pageText = doc.extractTextFromDownloadedEbook(sampleText);
            } 
            sendTextToACP(pageText);
        }
    }		
}

#endif /* defined(__AutoComPasteV2__Main__) */
