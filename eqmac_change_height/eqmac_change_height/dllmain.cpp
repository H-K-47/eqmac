#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

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

std::vector<everquest_spawn_t> everquest_spawns;
std::vector<everquest_spawn_t>::iterator everquest_spawns_it;

#define APPLICATION_NAME "EQMac Change Height"

const std::string ini_file = "eqmac_change_height.ini";

bool change_height_use_minimum = false;
bool change_height_use_maximum = false;

float change_height_minimum = 1.0;
float change_height_maximum = 5.0;

void load_config()
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);

    change_height_use_minimum = pt.get<bool>("ChangeHeight.bUseMinimum", change_height_use_minimum);
    change_height_use_maximum = pt.get<bool>("ChangeHeight.bUseMaximum", change_height_use_maximum);

    change_height_minimum = pt.get<float>("ChangeHeight.fMinimumHeight", change_height_minimum);
    change_height_maximum = pt.get<float>("ChangeHeight.fMaximumHeight", change_height_maximum);
}

DWORD WINAPI thread_function(LPVOID param)
{
    everquest_chat_helper_write_chat_text(APPLICATION_NAME " loaded.");
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
        }

        if
        (
            (change_height_minimum > 0) &&
            (change_height_maximum > 0)
        )
        {
            float player_height = everquest_function_get_player_height();

            if (change_height_use_minimum == true)
            {
                if (player_height < change_height_minimum)
                {
                    everquest_function_set_player_height(change_height_minimum);
                }
            }

            if (change_height_use_maximum == true)
            {
                if (player_height > change_height_maximum)
                {
                    everquest_function_set_player_height(change_height_maximum);
                }
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

    load_config();

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

            DisableThreadLibraryCalls(module);
            CreateThread(NULL, 0, &thread_load, NULL, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            everquest_chat_helper_remove_detour();
            break;
    }

    return TRUE;
}
