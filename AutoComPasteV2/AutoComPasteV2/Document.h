//
//  Document.h
//  AutoComPasteV2
//
//  Created by Raghav Ramesh on 5/9/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#ifndef __AutoComPasteV2__Document__
#define __AutoComPasteV2__Document__

#include <iostream>

class Document {

private:
	Page current;
	Page previous;

public: 

	//Initiate identifiers
	Document();

	//Returns true if an e-book version of the document is available online
	bool EBookVersionExistOnline();
	
	//Returns true if a page in the document as been flipped
	bool hasFlipped(Page page1, Page page2);

	//Store page temporarily
	void storeFlippedPage(Page page));
	
	//Delete previously stored image(s)
	void deleteCache();

	//If eBook exists, download specified page from ebook
	Page downloadPageFromOnline(String sampleText);

	//If eBook exists, download eBook
	void downloadEBook();

	//Returns true if the ebook has been downloaded
	bool EBookHasBeenDownloaded(String sampleText);

	//If eBook has already been downloaded
	Page extractPageFromDownloadedEBook(String sampleText);
}

#endif /* defined(__AutoComPasteV2__Document__) */
