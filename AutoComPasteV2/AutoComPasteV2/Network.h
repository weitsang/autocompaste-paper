//
//  Network.h
//  AutoComPasteV2
//
//  Created by Raghav Ramesh on 5/9/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#ifndef __AutoComPasteV2__Network__
#define __AutoComPasteV2__Network__

#include <iostream>

"""Set the ACP header with given message"""
string setACPHeaderWithMessage(string message);

"""Create connection and test until one transmission succeeds"""
createConnectionWithACP(char* defaultPort);

"""Receive until peer closes connection"""
receiveDataFromACP();

"""Cleans up and closes connection"""
closeConnection();

"""Storing the recognized text from page to the database"""
sendTextToACP(string message)

"""To check if the data has been sent or not"""
bool sendError()

#endif /* defined(__AutoComPasteV2__Network__) */
