#include "stdafx.h"
#include "sock.h"
#include <iostream>
#include <cstring>
#include <string>

int i = 0;

// Replace with c++ JSON formater 
// Formats text to JSON format manually
string sock::toACP(string message)
{

    string header = "{\"name\":\"processRawText\", \"parameters\":[\"";
    header += message + "\", 'HARDCOPY', 'ocr']}";
    return header;
}

// Create socket for TCP connection
// Uses winsock
int sock::connection(char* DEFAULT_PORT, string sendbus)
{
    // Check if sent successful
    char pass[16] = "{\"status\":0}\r\n";
    
    pass[14] = 0;
    pass[15] = 10;

    int argc = 2;
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    //std::cout<<sendbus<<endl;

    char *sendbuf = new char[2048];
    strncpy(sendbuf, sendbus.c_str(), sendbus.size() + 1);
    sendbuf[sendbus.size()] = 0;
    std::cout<<sendbuf;
    char recvbuf[16];
    int iResult;
    int recvbuflen = 16;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != 0) 
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
    if(iResult != 0) 
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if(iResult == SOCKET_ERROR) 
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if(ConnectSocket == INVALID_SOCKET) 
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) 
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if(iResult == SOCKET_ERROR) 
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if(iResult > 0) 
        {
            printf("Bytes received: %d\n", iResult);
            printf("%s", recvbuf);
            if (recvbuf[10] != '0')
            {
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }
            else
                printf("**successful \n");
        }
        else if(iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while(iResult > 0);

    // Cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
