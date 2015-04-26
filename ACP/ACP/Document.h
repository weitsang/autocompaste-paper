//
//  Document.h
//  ACP
//
//  Created by Raghav Ramesh on 30/10/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#ifndef __ACP__Document__
#define __ACP__Document__

#include <iostream>
#include "Page.h"

class Document {
private:
    Page previous, current;
    
public:
    Document();
	
	void processPage(page &currentPage);
	bool processLineLength(vector<int> Frame1, vector<int> Frame2);
	bool checkFlip(page initialFrame, page nextFrame);
	
	
	
    void checkIfPageHasBeenFlipped(Page previous, Page current);
};

#endif /* defined(__ACP__Document__) */
