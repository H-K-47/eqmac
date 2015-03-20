#include <cstdio>
#include <cstring>
#include <cmath>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "detours.h"
#pragma comment(lib, "detours.lib")

#include "eqmac.h"
#include "eqmac_functions.h"

const char* g_applicationName = "EQMac Client";

volatile int g_bExit = 0;

volatile int g_bInterpretCmd = 0;

HMODULE g_module;

HANDLE g_handleThreadLoad;
HANDLE g_handleThreadLoop;

EQ_FUNCTION_TYPE_CEverQuest__InterpretCmd EQMACCLIENT_REAL_InterpretCmd = NULL;

#define EQMACCLIENT_IP_ADDRESS  "127.0.0.1"
#define EQMACCLIENT_PORT_NUMBER 29015

#define EQMACCLIENT_RECV_MAX 4096

#define EQMACCLIENT_COMMANDS_MAX 32
#define EQMACCLIENT_COMMAND_TEXT_MAX 4096

WSADATA g_wsaData;

SOCKET g_connectSocket;

fd_set g_fdSetRead;

char g_recvBuffer[EQMACCLIENT_RECV_MAX] = {0};

char g_commandText[EQMACCLIENT_COMMAND_TEXT_MAX] = {0};

char g_commandList[EQMACCLIENT_COMMANDS_MAX][EQMACCLIENT_COMMAND_TEXT_MAX] = {0};

int __fastcall EQMACCLIENT_DETOUR_InterpretCmd(void* this_ptr, void* not_used, class EQPlayer* a1, char* a2)
{
    if (g_bExit == 1)
    {
        return EQMACCLIENT_REAL_InterpretCmd(this_ptr, a1, a2);
    }

    if (strlen(a2) > 0)
    {
        memset(g_commandText, 0, EQMACCLIENT_COMMAND_TEXT_MAX);
        strcpy_s(g_commandText, a2);

        g_bInterpretCmd = 1;
    }

    //FILE* file = fopen("eqmac_client-debug.txt", "a");
    //fprintf(file, "CEverQuest::InterpretCmd: %s\n", a2);
    //fclose(file);

    // do not let these /bc commands leak to the EverQuest server
    // return NULL
    if
    (
        strncmp(a2, "/bc",               3) == 0 ||
        strncmp(a2, "/bct",              4) == 0 ||
        strncmp(a2, "/bca",              4) == 0 ||
        strncmp(a2, "/bcaa",             5) == 0 ||
        strncmp(a2, "/bccmd",            6) == 0 ||
        strncmp(a2, "/bccmd help",      11) == 0 ||
        strncmp(a2, "/bccmd quit",      11) == 0
    )
    {
        if (strncmp(a2, "/bccmd help", 11) == 0)
        {
            //
        }

        if (strncmp(a2, "/bccmd quit", 11) == 0)
        {
            g_bExit = 1;
        }

        return EQMACCLIENT_REAL_InterpretCmd(this_ptr, NULL, NULL);
    }

    return EQMACCLIENT_REAL_InterpretCmd(this_ptr, a1, a2);
}

DWORD WINAPI EQMACCLIENT_ThreadLoop(LPVOID param)
{
    if (WSAStartup(MAKEWORD(2,2), &g_wsaData) != 0)
    {
        return 0;
    }

    if ((g_connectSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        closesocket(g_connectSocket);
        WSACleanup();
        return 0;
    }

    struct hostent* host;
    if ((host = gethostbyname(EQMACCLIENT_IP_ADDRESS)) == NULL)
    {
        closesocket(g_connectSocket);
        WSACleanup();
        return 0;
    }

    struct sockaddr_in address;
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    address.sin_port        = htons(EQMACCLIENT_PORT_NUMBER);

    while (TRUE)
    {
        if (GetAsyncKeyState(VK_PAUSE))
        {
            g_bExit = 1;
            break;
        }

        if (connect(g_connectSocket, (struct sockaddr*)&address, sizeof(address)) != SOCKET_ERROR)
        {
            if (g_connectSocket != INVALID_SOCKET)
            {
                const char* connectMessage = "Hello! I just connected to the server.";
                send(g_connectSocket, connectMessage, (int)strlen(connectMessage), 0);

                break;
            }
        }

        Sleep(1000);
    }

    while (g_bExit == 0)
    {
        if (GetAsyncKeyState(VK_PAUSE))
        {
            g_bExit = 1;
        }

        if (EQ_OBJECT_CEverQuest->GameState != EQ_GAME_STATE_IN_GAME)
        {
            Sleep(1000);
            continue;
        }

        //if (g_bInterpretCmd == 0)
        {
            FD_ZERO(&g_fdSetRead);

            FD_SET(g_connectSocket, &g_fdSetRead);

            timeval tv;
            tv.tv_sec  = 0;
            tv.tv_usec = 0;

            int selectResult = select(0, &g_fdSetRead, NULL, NULL, &tv);

            if (selectResult != SOCKET_ERROR)
            {
                if (FD_ISSET(g_connectSocket, &g_fdSetRead))
                {
                    memset(g_recvBuffer, 0, EQMACCLIENT_RECV_MAX);

                    int recvResult = recv(g_connectSocket, g_recvBuffer, EQMACCLIENT_RECV_MAX, 0);

                    if (recvResult > 0)
                    {
                        //EQ_CLASS_CEverQuest->dsp_chat(g_recvBuffer);

                        if (strncmp(g_recvBuffer, "Command: ", 9) == 0)
                        {
                            char commandText[EQMACCLIENT_COMMAND_TEXT_MAX] = {0};
                            strcpy_s(commandText, g_recvBuffer);

                            char* token     = NULL;
                            char* tokenNext = NULL;

                            token = strtok_s(commandText, ";", &tokenNext);
                            while (token != NULL)
                            {
                                char cmdText[EQMACCLIENT_COMMAND_TEXT_MAX];
                                strcpy_s(cmdText, token + 9);

                                if (cmdText[0] == '/' && strstr(cmdText, ";") == NULL)
                                {
                                    for (size_t i = 0; i < EQMACCLIENT_COMMANDS_MAX; i++)
                                    {
                                        if (strlen(g_commandList[i]) == 0)
                                        {
                                            strcpy_s(g_commandList[i], cmdText);
                                            break;
                                        }
                                    }
                                }

                                token = strtok_s(NULL, ";", &tokenNext);
                            }
                        }
                    }
                }
            }
        }

        if (g_bInterpretCmd == 1)
        {
            if (g_connectSocket != INVALID_SOCKET)
            {
                if (send(g_connectSocket, g_commandText, (int)strlen(g_commandText), 0) == SOCKET_ERROR)
                {
                    g_bExit = 1;
                }
            }

            g_bInterpretCmd = 0;
        }

        for (size_t i = 0; i < EQMACCLIENT_COMMANDS_MAX; i++)
        {
            if (strlen(g_commandList[i]) > 0)
            {
                if (strlen(g_commandList[i]) > 7 && strncmp(g_commandList[i], "/pause ", 7) == 0)
                {
                    char sleepText[128];
                    strncpy_s(sleepText, g_commandList[i] + 7, strlen(g_commandList[i]) - 1);

                    int sleepTime = atoi(sleepText);

                    if (sleepTime > 0)
                    {
                        Sleep((int)(sleepTime * 1000));
                    }

                    continue;
                }

                PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

                EQ_CLASS_CEverQuest->InterpretCmd((EQPlayer*)playerSpawn, g_commandList[i]);

                memset(g_commandList[i], 0, EQMACCLIENT_COMMAND_TEXT_MAX);

                Sleep(1000);
            }
        }

        Sleep(100);
    }

    if (shutdown(g_connectSocket, SD_BOTH) == SOCKET_ERROR)
    {
        closesocket(g_connectSocket);
        WSACleanup();
    }

    closesocket(g_connectSocket);
    WSACleanup();

    DetourRemove((PBYTE)EQMACCLIENT_REAL_InterpretCmd, (PBYTE)EQMACCLIENT_DETOUR_InterpretCmd);

    FreeLibraryAndExitThread(g_module, 0);
    return 0;
}

DWORD WINAPI EQMACCLIENT_ThreadLoad(LPVOID param)
{
    EQMACCLIENT_REAL_InterpretCmd = (EQ_FUNCTION_TYPE_CEverQuest__InterpretCmd)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__InterpretCmd, (PBYTE)EQMACCLIENT_DETOUR_InterpretCmd);

    g_handleThreadLoop = CreateThread(NULL, 0, &EQMACCLIENT_ThreadLoop, NULL, 0, NULL);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    g_module = module;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(module);
            g_handleThreadLoad = CreateThread(NULL, 0, &EQMACCLIENT_ThreadLoad, NULL, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            //WaitForSingleObject(g_handleThreadLoad, INFINITE);
            CloseHandle(g_handleThreadLoad);

            //WaitForSingleObject(g_handleThreadLoop, INFINITE);
            CloseHandle(g_handleThreadLoop);
            break;
    }

    return TRUE;
}

