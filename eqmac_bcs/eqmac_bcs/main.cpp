#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cmath>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")

#define EQMACBCS_DEFAULT_PORT_NUMBER 29015

#define EQMACBCS_CLIENTS_MAX 32

#define EQMACBCS_RECV_MAX 4096
#define EQMACBCS_SEND_MAX 4096

#define EQMACBCS_TEXT_MAX 4096

int main(int argc , char *argv[])
{
    printf("EQMac Box Chat Server\n");

    unsigned short portNumber = EQMACBCS_DEFAULT_PORT_NUMBER;

    if (argc == 2)
    {
        if (strlen(argv[1]) > 0)
        {
            portNumber = atoi(argv[1]);

            if (portNumber <= 1024)
            {
                printf("Port number was less than 1024, using default port number instead.\n");

                portNumber = EQMACBCS_DEFAULT_PORT_NUMBER;
            }
        }
    }
    else if (argc == 3)
    {
        if (strlen(argv[1]) > 0 && strlen(argv[2]) > 0)
        {
            if (strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "-port") == 0)
            {
                portNumber = atoi(argv[2]);

                if (portNumber <= 1024)
                {
                    printf("Port number was less than 1024, using default port number instead.\n");

                    portNumber = EQMACBCS_DEFAULT_PORT_NUMBER;
                }
            }
        }
    }

    printf("Port: %d\n", portNumber);

    WSADATA wsaData;

    SOCKET clientSocket[EQMACBCS_CLIENTS_MAX];
    char clientSocketName[EQMACBCS_CLIENTS_MAX][EQMACBCS_TEXT_MAX];

    SOCKET listenSocket;
    SOCKET acceptSocket;
    SOCKET receiveSocket;
    SOCKET sendSocket;

    struct sockaddr_in server;
    struct sockaddr_in address;

    int addressLength = sizeof(struct sockaddr_in);

    fd_set fdSetRead;

    char recvBuffer[EQMACBCS_RECV_MAX];

    for (size_t i = 0 ; i < EQMACBCS_CLIENTS_MAX; i++)
    {
        clientSocket[i] = 0;

        memset(clientSocketName[i], 0, EQMACBCS_TEXT_MAX);
    }

    printf("Initialising Winsock...");

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        printf("\nWSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    printf("OK.\n");

    if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("socket failed. Error Code: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        return 1;
    }

    printf("Socket created.\n");

    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port        = htons(portNumber);

    if (bind(listenSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("bind failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Bind done.\n");

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("listen failed. Error Code: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        return 1;
    }

    printf("Waiting for incoming connections...\n");

    while (TRUE)
    {
        FD_ZERO(&fdSetRead);

        FD_SET(listenSocket, &fdSetRead);

        for (size_t i = 0 ; i < EQMACBCS_CLIENTS_MAX; i++)
        {
            receiveSocket = clientSocket[i];

            if (receiveSocket > 0)
            {
                FD_SET(receiveSocket , &fdSetRead);
            }
        }

        int selectResult = select(0, &fdSetRead, NULL, NULL, NULL);

        if (selectResult == SOCKET_ERROR)
        {
            printf("select failed. Error Code: %d\n" , WSAGetLastError());
            return 1;
        }

        if (FD_ISSET(listenSocket, &fdSetRead))
        {
            if ((acceptSocket = accept(listenSocket, (struct sockaddr*)&address, (int*)&addressLength)) < 0)
            {
                printf("accept failed.\n");
                return 1;
            }

            printf("Client connected. Socket: %d, IP: %s, Port: %d\n", acceptSocket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            const char* connectMessage = "Connected to EQMac Server|";
            send(acceptSocket, connectMessage, strlen(connectMessage), 0);

            for (size_t i = 0; i < EQMACBCS_CLIENTS_MAX; i++)
            {
                if (clientSocket[i] == 0)
                {
                    clientSocket[i] = acceptSocket;
                    printf("Client socket index: %d\n", i);
                    break;
                }
            }
        }

        for (size_t i = 0; i < EQMACBCS_CLIENTS_MAX; i++)
        {
            receiveSocket = clientSocket[i];

            if (FD_ISSET(receiveSocket, &fdSetRead))
            {
                getpeername(receiveSocket, (struct sockaddr*)&address, (int*)&addressLength);

                int recvResult = recv(receiveSocket, recvBuffer, EQMACBCS_RECV_MAX, 0);

                if (recvResult == SOCKET_ERROR)
                {
                    int errorCode = WSAGetLastError();

                    if (errorCode == WSAECONNRESET)
                    {
                        printf("Client disconnected unexpectedly. IP: %s, Port: %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                        const char* disconnectMessage = "Disconnected from EQMac Server|";
                        send(receiveSocket, disconnectMessage, strlen(disconnectMessage), 0);

                        closesocket(receiveSocket);
                        clientSocket[i] = 0;
                    }
                    else
                    {
                        printf("recv failed. Error Code: %d\n" , errorCode);
                    }
                }
                else if (recvResult == 0)
                {
                    printf("Client disconnected. IP: %s, Port: %d\n", inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

                    const char* disconnectMessage = "Disconnected from EQMac Server|";
                    send(receiveSocket, disconnectMessage, strlen(disconnectMessage), 0);

                    closesocket(receiveSocket);
                    clientSocket[i] = 0;
                }
                else
                {
                    recvBuffer[recvResult] = '\0';
                    printf("%s:%d - %s\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port), recvBuffer);

                    char receivedText[EQMACBCS_RECV_MAX];
                    strcpy_s(receivedText, recvBuffer);

                    //printf("receivedText: %s\n", receivedText);

                    char* token     = NULL;
                    char* tokenNext = NULL;

                    token = strtok_s(receivedText, "|", &tokenNext);
                    while (token != NULL)
                    {
                        //printf("token: %s\n", token);

                        size_t commandLength = 0;

                        if (strncmp(token, "Name: ", 6) == 0)
                        {
                            char name[128];
                            strcpy_s(name, token + 6);

                            //printf("name: %s\n", name);

                            for (size_t k = 0; k < EQMACBCS_CLIENTS_MAX; k++)
                            {
                                if (clientSocket[k] == receiveSocket)
                                {
                                    printf("Name: %s, linked to client socket %d from receive socket %d\n", name, clientSocket[k], receiveSocket);

                                    strcpy_s(clientSocketName[k], name);

                                    //acceptSocket = INVALID_SOCKET;

                                    break;
                                }
                            }
                        }

                        bool bBCT  = false;
                        bool bBCA  = false;
                        bool bBCAA = false;

                        bool bBroadcast = false;

                        if (strncmp(token, "/bct", 4) == 0)
                        {
                            commandLength = strlen("/bct") + 1;

                            bBCT  = true;
                            bBCA  = false;
                            bBCAA = false;

                            bBroadcast = true;
                        }
 
                        if (strncmp(token, "/bca", 4) == 0)
                        {
                            commandLength = strlen("/bca") + 1;

                            bBCT  = false;
                            bBCA  = true;
                            bBCAA = false;

                            bBroadcast = true;

                            if (strncmp(token, "/bcaa", 5) == 0)
                            {
                                commandLength = strlen("/bcaa") + 1;

                                bBCT  = false;
                                bBCA  = false;
                                bBCAA = true;

                                bBroadcast = true;
                            }
                        }

                        if (bBroadcast == true)
                        {
                            for (size_t j = 0; j < EQMACBCS_CLIENTS_MAX; j++)
                            {
                                if (clientSocket[j] == 0)
                                {
                                    continue;
                                }

                                if (bBCT == true)
                                {
                                    //printf("bBCT == true\n");

                                    if (strlen(clientSocketName[j]) == 0)
                                    {
                                        continue;
                                    }
                                }

                                sendSocket = clientSocket[j];

                                if (bBCT == false && bBCA == true && bBCAA == false && sendSocket == receiveSocket)
                                {
                                    //printf("bBCA == true, sendSocket == receiveSocket\n");

                                    continue;
                                }

                                char bcText[EQMACBCS_RECV_MAX] = {0};
                                strcpy_s(bcText, token + commandLength);

                                char name2[128];

                                char* token2     = NULL;
                                char* token2Next = NULL;

                                if (bBCT == true)
                                {
                                    token2 = strtok_s(bcText, " ", &token2Next);

                                    // token2 == Name
                                    strcpy_s(name2, token2);

                                    token2 = strtok_s(NULL, "", &token2Next);

                                    token2 = strtok_s(token2, ";", &token2Next);

                                    //printf("bBCT token2: %s\n", token2);
                                }
                                else
                                {
                                    token2 = strtok_s(bcText, ";", &token2Next);
                                }

                                //printf("name2: %s\n", name2);

                                if (bBCT == true)
                                {
                                    if (strlen(name2) == 0)
                                    {
                                        continue;
                                    }

                                    if (strcmp(name2, clientSocketName[j]) != 0)
                                    {
                                        //printf("strcmp(name2, clientSocketName[j]) != 0, %s != %s\n", name2, clientSocketName[j]);
                                        continue;
                                    }
                                }

                                while (token2 != NULL)
                                {
                                    //printf("token2: %s\n", token2);

                                    if (token2[0] != '/')
                                    {
                                         continue;
                                    }

                                    char commandText[EQMACBCS_SEND_MAX] = {0};
                                    sprintf_s(commandText, "Command: %s|", token2);

                                    //printf("commandText: %s\n", commandText);

                                    send(sendSocket, commandText, strlen(commandText), 0);

                                    token2 = strtok_s(NULL, ";", &token2Next);
                                }
                            }
                        }

                        token = strtok_s(NULL, "|", &tokenNext);
                    }
                }
            }
        }
    }

    closesocket(listenSocket);
    closesocket(acceptSocket);
    closesocket(receiveSocket);
    closesocket(sendSocket);

    for (size_t i = 0; i < EQMACBCS_CLIENTS_MAX; i++)
    {
        closesocket(clientSocket[i]);
    }

    WSACleanup();

    return 0;
}
