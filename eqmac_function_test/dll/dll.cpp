#define WINVER 0x0500

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>

#include <windows.h>
#include <psapi.h>

#include <winbase.h>

#include "memory.hpp"

#include "eqmac.hpp"

#define APPLICATION_NAME       "EQMac Function Test"
#define APPLICATION_CLASS_NAME "eqmac_function_test"
#define APPLICATION_DLL_NAME   "eqmac_function_test.dll"

#define EVERQUEST_CLIENT "eqgame.exe"
#define EVERQUEST_TITLE  "EQW beta 2.32"

memory memory;

HANDLE module;

HANDLE window_thread;

DWORD window_thread_id;

void do_function()
{
    everquest_function_chat_write_ex("EQMac Function Test loaded.");

    everquest_function_open_trade_window();

    everquest_function_chat_write_ex("EQMac Function Test unloaded.");

    FreeLibraryAndExitThread((HINSTANCE)module, 0);

    ExitThread(0);
}

extern "C" BOOL APIENTRY DllMain(HANDLE dll, DWORD reason, LPVOID reserved)
{
    module = dll;

    if (reason == DLL_PROCESS_ATTACH)
    {
        //MessageBox(NULL, "fdwReason == DLL_PROCESS_ATTACH", "DllMain", MB_OK | MB_ICONINFORMATION);

        memory.enable_debug_privileges();

        window_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)do_function, dll, 0, &window_thread_id);
    }

    if (reason == DLL_PROCESS_DETACH)
    {
        //MessageBox(NULL, "fdwReason == DLL_PROCESS_DETACH", "DllMain", MB_OK | MB_ICONINFORMATION);
    }

    return TRUE;
}
