#include <string>

#include <windows.h>

#include "vk_keys.h"

#include "memory.hpp"

#include "eqmac.hpp"
#include "eqmac_functions.hpp"

memory mem;

HMODULE module_global;

#define APPLICATION_NAME "EQMac Keys"

DWORD WINAPI thread_function(LPVOID param)
{
    everquest_function_write_text_to_chat(APPLICATION_NAME " loaded.");
    Sleep(1000);

    while (1)
    {
        if (everquest_function_is_in_game() == false)
        {
            Sleep(100);
            continue;
        }

        if (mem.is_foreground_window_current_process_id() == true)
        {
            if
            (
                (KEYDOWN(VK_CONTROL)) &&
                (KEYDOWN(VK_ALT)) &&
                (KEYDOWN(VK_BACK))
            )
            {
                break;
            }

            Sleep(10);
            continue;
        }

        if (KEYDOWN(VK_1) && KEYDOWN(VK_CONTROL))
        {
            everquest_function_do_hot_button(1);
            Sleep(1000);
        }

        if (KEYDOWN(VK_2) && KEYDOWN(VK_CONTROL))
        {
            everquest_function_do_hot_button(2);
            Sleep(1000);
        }

        if (KEYDOWN(VK_3) && KEYDOWN(VK_CONTROL))
        {
            everquest_function_do_hot_button(3);
            Sleep(1000);
        }

        if (KEYDOWN(VK_4) && KEYDOWN(VK_CONTROL))
        {
            everquest_function_do_hot_button(4);
            Sleep(1000);
        }

        if (KEYDOWN(VK_5) && KEYDOWN(VK_CONTROL))
        {
            everquest_function_do_hot_button(5);
            Sleep(1000);
        }

        if (KEYDOWN(VK_6) && KEYDOWN(VK_CONTROL))
        {
            everquest_function_do_hot_button(6);
            Sleep(1000);
        }

        if (KEYDOWN(VK_7) && KEYDOWN(VK_CONTROL))
        {
            everquest_function_do_hot_button(7);
            Sleep(1000);
        }

        if (KEYDOWN(VK_8) && KEYDOWN(VK_CONTROL))
        {
            everquest_function_do_hot_button(8);
            Sleep(1000);
        }

        if (KEYDOWN(VK_9) && KEYDOWN(VK_CONTROL))
        {
            everquest_function_do_hot_button(9);
            Sleep(1000);
        }

        if (KEYDOWN(VK_0) && KEYDOWN(VK_CONTROL))
        {
            everquest_function_do_hot_button(10);
            Sleep(1000);
        }

        Sleep(10);
    }

    everquest_function_write_text_to_chat(APPLICATION_NAME " unloaded.");
    Sleep(1000);

    FreeLibraryAndExitThread(module_global, 0);
    return 0;
}

DWORD WINAPI thread_load(LPVOID param)
{
    mem.enable_debug_privileges();
    mem.set_process_by_id(GetCurrentProcessId());

    if (mem.get_process_id() == 0)
    {
        FreeLibraryAndExitThread(module_global, 0);
        return 0;
    }

    HANDLE process_handle = GetCurrentProcess();
    SetPriorityClass(process_handle, BELOW_NORMAL_PRIORITY_CLASS);

    CreateThread(NULL, 0, &thread_function, NULL, 0, NULL);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    module_global = module;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(module);
            CreateThread(NULL, 0, &thread_load, NULL, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            FreeLibraryAndExitThread(module_global, 0);
            break;
    }

    return TRUE;
}
