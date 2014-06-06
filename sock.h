
#ifndef sock_H_
#define sock_H_


#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_

#include <winsock2.h>
#include <windows.h>


#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

using namespace std;

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 4096
//#define DEFAULT_PORT "5566"


class sock
{
public:
	int connection(char* DEFAULT_PORT,string buf);
	string toACP(string message);


};



#endif