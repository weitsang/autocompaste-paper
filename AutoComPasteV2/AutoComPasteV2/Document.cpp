//
//  Document.cpp
//  AutoComPasteV2
//
//  Created by Raghav Ramesh on 5/9/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include "Document.h"


Document :: Document()
{
         
}
    
 bool Document :: EBookVersionExistsOnline(String sampleText) 
 {
	 // search online using the contents of the page and return a bool depending on whether the book exists or not.
 }

 bool Document :: hasFlipped(Page page1, Page page2)
 {
	 // can we use the API call from opencv directly?
 }

 void Document :: storeFlippedPage(Page page)
 {
	 previous = current;
	 current = page;
 }

 void Document :: deleteCache()
 {
	 // delete cache if we decide to store a cache for recognized pages
 }

 Page Document ::  downloadPageFromOnline(String sampleText)
 {
	 // need to implement for doing eBook part 
 }

 void Document :: downloadEBook()
 {
	 // To download the enitre book
	 // need to implement for doing eBook part
 }

 bool Document :: EBookHasBeenDownloaded(String sampleText)
 {
	 // check within the system files if the book already exists
	 // need to implement for eBook part
 }

 Page Document :: extractPageFromDownloadedEBook(String sampleText)
 {
	 // need to implement for eBook part
 }
