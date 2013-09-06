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

#include "eqdalaya.hpp"

#define APPLICATION_NAME "EQDalaya Keys"

#define VK_0 0x30
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39

#define VK_ALT VK_MENU

memory memory;

bool exit_dll = false;

void thread_function(HMODULE module)
{
    memory.enable_debug_privileges();
    memory.set_process_by_id(GetCurrentProcessId());

    everquest_function_chat_write_ex(APPLICATION_NAME " loaded.");

    while(1)
    {

        if (exit_dll == true)
        {
            break;
        }

        if (memory.get_process_id() == 0)
        {
            continue;
        }

        if (memory.is_foreground_window_current_process_id() == true)
        {
            if
            (
                (GetAsyncKeyState(VK_CONTROL)) &&
                (GetAsyncKeyState(VK_ALT)) &&
                (GetAsyncKeyState(VK_BACK))
            )
            {
                exit_dll = true;
                break;
            }

            continue;
        }

        if (GetAsyncKeyState(VK_CONTROL))
        {
            if (GetAsyncKeyState(VK_1))
            {
                everquest_function_do_hot_button(0);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_2))
            {
                everquest_function_do_hot_button(1);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_3))
            {
                everquest_function_do_hot_button(2);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_4))
            {
                everquest_function_do_hot_button(3);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_5))
            {
                everquest_function_do_hot_button(4);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_6))
            {
                everquest_function_do_hot_button(5);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_7))
            {
                everquest_function_do_hot_button(6);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_8))
            {
                everquest_function_do_hot_button(7);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_9))
            {
                everquest_function_do_hot_button(8);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_0))
            {
                everquest_function_do_hot_button(9);
                Sleep(1000);
            }
        }

        Sleep(1);
    }

    if (exit_dll == true)
    {
        everquest_function_chat_write_ex(APPLICATION_NAME " unloaded.");

        Sleep(1000);

        FreeLibraryAndExitThread(module, 0);
        ExitThread(0);
    }
}

extern "C" BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(module);
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_function, module, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
