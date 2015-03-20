#include <cstdio>
#include <cstring>
#include <cmath>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define EQMACCLIENT_IP_ADDRESS  "127.0.0.1"
#define EQMACCLIENT_PORT_NUMBER 29015

#define EQMACCLIENT_MAX_RECV 1024

int main(int argc , char *argv[])
{
    WSADATA wsaData;

    SOCKET connectSocket;

    char *recvBuffer;
    recvBuffer = (char*)malloc((EQMACCLIENT_MAX_RECV + 1) * sizeof(char));

    printf("EQMac Client\n");

    printf("Initialising Winsock...");

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        printf("\nWSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    printf("OK.\n");

    if ((connectSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("socket failed. Error Code: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    printf("Socket created.\n");

    struct hostent* host;
    if ((host = gethostbyname(EQMACCLIENT_IP_ADDRESS)) == NULL)
    {
        printf("gethostbyname failed.\n");
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    struct sockaddr_in address;
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    address.sin_port        = htons(EQMACCLIENT_PORT_NUMBER);

    while (TRUE)
    {
        if (connect(connectSocket, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
        {
            printf("connect failed.\n");
        }
        else
        {
            printf("Connected to server.\n");
            break;
        }

        if (connectSocket == INVALID_SOCKET)
        {
            printf("Unable to connect to server!\n");
        }

        printf("Trying again in 1 second...\n");

        Sleep(1000);
    }

    const char* connectMessage = "bca";

    if (send(connectSocket, connectMessage, (int)strlen(connectMessage), 0) == SOCKET_ERROR)
    {
        printf("send failed. Error Code: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    while (TRUE)
    {
        int recvResult = recv(connectSocket, recvBuffer, EQMACCLIENT_MAX_RECV, 0);

        if (recvResult > 0)
        {
            printf("Bytes received: %d\n", recvResult);

            recvBuffer[recvResult] = '\0';
            printf("Receive Buffer: %s\n", recvBuffer);
        }
    }

    if (shutdown(connectSocket, SD_BOTH) == SOCKET_ERROR)
    {
        printf("shutdown failed. Error Code: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    closesocket(connectSocket);
    WSACleanup();

    return 0;
}

/*
#include <stdlib.h>
#include <stdio.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "29015"

int __cdecl main(int argc, char **argv) 
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char *sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if ( iResult > 0 )
            printf("Bytes received: %d\n", iResult);
        else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while( iResult > 0 );

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
*/
