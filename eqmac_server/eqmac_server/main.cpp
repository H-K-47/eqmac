#include <cstdio>
#include <cstring>
#include <cmath>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define EQMACSERVER_PORT_NUMBER 29015

#define EQMACSERVER_CLIENTS_MAX 32

#define EQMACSERVER_RECV_MAX 4096

#define EQMACCLIENT_COMMAND_TEXT_MAX 4096

int main(int argc , char *argv[])
{
    WSADATA wsaData;

    SOCKET clientSocket[EQMACSERVER_CLIENTS_MAX];
    SOCKET listenSocket;
    SOCKET acceptSocket;
    SOCKET receiveSocket;
    SOCKET sendSocket;

    struct sockaddr_in server;
    struct sockaddr_in address;

    int addressLength = sizeof(struct sockaddr_in);

    fd_set fdSetRead;

    char recvBuffer[EQMACSERVER_RECV_MAX];

    printf("EQMac Server\n");

    for (size_t i = 0 ; i < EQMACSERVER_CLIENTS_MAX; i++)
    {
        clientSocket[i] = 0;
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
    server.sin_port        = htons(EQMACSERVER_PORT_NUMBER);

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

        for (size_t i = 0 ; i < EQMACSERVER_CLIENTS_MAX; i++)
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

            const char* connectMessage = "Connected to EQMac Server.";
            send(acceptSocket, connectMessage, strlen(connectMessage), 0);

            for (size_t i = 0; i < EQMACSERVER_CLIENTS_MAX; i++)
            {
                if (clientSocket[i] == 0)
                {
                    clientSocket[i] = acceptSocket;
                    printf("Client socket index: %d\n", i);
                    break;
                }
            }
        }

        for (size_t i = 0; i < EQMACSERVER_CLIENTS_MAX; i++)
        {
            receiveSocket = clientSocket[i];

            if (FD_ISSET(receiveSocket, &fdSetRead))
            {
                getpeername(receiveSocket, (struct sockaddr*)&address, (int*)&addressLength);

                int recvResult = recv(receiveSocket, recvBuffer, EQMACSERVER_RECV_MAX, 0);

                if (recvResult == SOCKET_ERROR)
                {
                    int errorCode = WSAGetLastError();

                    if (errorCode == WSAECONNRESET)
                    {
                        printf("Client disconnected unexpectedly. IP: %s, Port: %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                        const char* disconnectMessage = "Disconnected from EQMac Server.";
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

                    const char* disconnectMessage = "Disconnected from EQMac Server.";
                    send(receiveSocket, disconnectMessage, strlen(disconnectMessage), 0);

                    closesocket(receiveSocket);
                    clientSocket[i] = 0;
                }
                else
                {
                    recvBuffer[recvResult] = '\0';
                    printf("%s:%d - %s\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port), recvBuffer);

                    if (strncmp(recvBuffer, "/bca", 4) == 0)
                    {
                        size_t commandLength = strlen("/bca") + 1;

                        bool bBroadcastAllIncludingClient = false;

                        if (strncmp(recvBuffer, "/bcaa", 5) == 0)
                        {
                            bBroadcastAllIncludingClient = true;

                            commandLength = strlen("/bcaa") + 1;
                        }

                        for (size_t j = 0; j < EQMACSERVER_CLIENTS_MAX; j++)
                        {
                            if (clientSocket[j] == 0)
                            {
                                continue;
                            }

                            sendSocket = clientSocket[j];

                            if (bBroadcastAllIncludingClient == false && sendSocket == receiveSocket)
                            {
                                continue;
                            }

                            char bcText[EQMACCLIENT_COMMAND_TEXT_MAX] = {0};
                            strcpy_s(bcText, recvBuffer);

                            char* token     = NULL;
                            char* tokenNext = NULL;

                            token = strtok_s(bcText + commandLength, ";", &tokenNext);
                            while (token != NULL)
                            {
                                //printf("token: %s\n", token);

                                if (token[0] != '/')
                                {
                                     continue;
                                }

                                char commandText[EQMACCLIENT_COMMAND_TEXT_MAX] = {0};
                                sprintf_s(commandText, "Command: %s;", token);

                                //printf("commandText: %s\n", commandText);

                                send(sendSocket, commandText, strlen(commandText), 0);

                                token = strtok_s(NULL, ";", &tokenNext);
                            }
                        }
                    }
                }
            }
        }
    }

    closesocket(listenSocket);
    closesocket(acceptSocket);
    closesocket(receiveSocket);
    closesocket(sendSocket);

    for (size_t i = 0; i < EQMACSERVER_CLIENTS_MAX; i++)
    {
        closesocket(clientSocket[i]);
    }

    WSACleanup();

    return 0;
}
