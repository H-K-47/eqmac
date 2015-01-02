#include <string>
#include <fstream>

#include <windows.h>

#include "detours.h"
#pragma comment(lib, "detours.lib")

#include "vk_keys.h"

#include "memory.hpp"

#include "eqmac.hpp"
#include "eqmac_functions.hpp"
#include "eqmac_chat_helper.hpp"

memory mem;

HMODULE module_global;

#define APPLICATION_NAME "EQMac Detour Function Test"

std::fstream log_file;

EVERQUEST_FUNCTION_TYPE_CEverQuest__MoveToZone2 Real_CEverQuest__MoveToZone2 = NULL;

int __cdecl My_CEverQuest__MoveToZone2(int a1, char* a2, int a3, int a4)
{
    /*
    everquest_chat_helper_write_chat_text("CEverQuest__MoveToZone");

    everquest_chat_helper_write_chat_text("Zone ID:");
    everquest_chat_helper_write_chat_text(std::to_string(a1).c_str());

    everquest_chat_helper_write_chat_text("Text:");
    everquest_chat_helper_write_chat_text(a2);

    everquest_chat_helper_write_chat_text("Destination Type:");
    everquest_chat_helper_write_chat_text(std::to_string(a3).c_str());

    everquest_chat_helper_write_chat_text("Zone Request Reason:");
    everquest_chat_helper_write_chat_text(std::to_string(a4).c_str());
    */

    log_file.open("eqmac_detour_function_test.txt", std::fstream::in | std::fstream::out | std::fstream::app);

    log_file << "CEverQuest__MoveToZone\n";
    log_file << "Zone ID: "             << a1 << "\n";
    log_file << "Text: "                << a2 << "\n";
    log_file << "Destination Type: "    << a3 << "\n";
    log_file << "Zone Request Reason: " << a4 << "\n";
    log_file << "\n";

    log_file.close();

    return Real_CEverQuest__MoveToZone2(a1, a2, a3, a4);
}

DWORD WINAPI thread_function(LPVOID param)
{
    everquest_chat_helper_write_chat_text(APPLICATION_NAME " loaded.");
    Sleep(1000);

    while (1)
    {
        if (everquest_is_in_game(mem) == false)
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
        }

        Sleep(10);
    }

    everquest_chat_helper_write_chat_text(APPLICATION_NAME " unloaded.");
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

    CreateThread(NULL, 0, &thread_function, NULL, 0, NULL);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    module_global = module;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            everquest_chat_helper_detour_function();

            Real_CEverQuest__MoveToZone2 = (EVERQUEST_FUNCTION_TYPE_CEverQuest__MoveToZone2)DetourFunction((PBYTE)EVERQUEST_FUNCTION_CEverQuest__MoveToZone2, (PBYTE)My_CEverQuest__MoveToZone2);
            
            DisableThreadLibraryCalls(module);
            CreateThread(NULL, 0, &thread_load, NULL, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            everquest_chat_helper_remove_detour();

            DetourRemove((PBYTE)Real_CEverQuest__MoveToZone2, (PBYTE)My_CEverQuest__MoveToZone2);

            break;
    }

    return TRUE;
}
