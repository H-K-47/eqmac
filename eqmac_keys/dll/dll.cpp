#include <windows.h>

#include "vk_keys.h"

#include "memory.hpp"

#include "eqmac.hpp"

#define APPLICATION_NAME       "EQMac Keys"
#define APPLICATION_CLASS_NAME "eqmac_keys"
#define APPLICATION_DLL_NAME   "eqmac_keys.dll"

#define EVERQUEST_CLIENT "eqgame.exe"
#define EVERQUEST_TITLE  "EQW beta 2.32"

bool exit_dll = false;

memory memory;

HMODULE module_global;

HANDLE thread_handle_load;
HANDLE thread_handle_keys;

void thread_keys(HMODULE module)
{
    while (1)
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

            Sleep(100);
            continue;
        }

        if (GetAsyncKeyState(VK_CONTROL))
        {
            if (GetAsyncKeyState(VK_1))
            {
                everquest_function_CHotButtonWnd__DoHotButton(0, 1);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_2))
            {
                everquest_function_CHotButtonWnd__DoHotButton(1, 1);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_3))
            {
                everquest_function_CHotButtonWnd__DoHotButton(2, 1);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_4))
            {
                everquest_function_CHotButtonWnd__DoHotButton(3, 1);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_5))
            {
                everquest_function_CHotButtonWnd__DoHotButton(4, 1);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_6))
            {
                everquest_function_CHotButtonWnd__DoHotButton(5, 1);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_7))
            {
                everquest_function_CHotButtonWnd__DoHotButton(6, 1);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_8))
            {
                everquest_function_CHotButtonWnd__DoHotButton(7, 1);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_9))
            {
                everquest_function_CHotButtonWnd__DoHotButton(8, 1);
                Sleep(1000);
            }

            if (GetAsyncKeyState(VK_0))
            {
                everquest_function_CHotButtonWnd__DoHotButton(9, 1);
                Sleep(1000);
            }
        }

        Sleep(100);
    }

    if (exit_dll == true)
    {
        everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " unloaded.");

        Sleep(1000);

        FreeLibraryAndExitThread(module_global, 0);

        return;
    }
}

void thread_load(HMODULE module)
{
    memory.enable_debug_privileges();
    memory.set_process_by_id(GetCurrentProcessId());

    thread_handle_keys = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_keys, module, 0, NULL);

    everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " loaded.");

    return;
}

extern "C" BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    module_global = module;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(module);
            thread_handle_load = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_load, module, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
