#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>

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

#define APPLICATION_NAME "EQMac Silk Bot"

bool silk_bot_activated = false;

float silk_bot_melee_distance = 5.0;

DWORD silk_bot_loot_corpse_delay = 5000;

std::vector<std::string> silk_bot_loot_mobs;
std::vector<std::string>::iterator silk_bot_loot_mobs_it;

std::vector<std::string> silk_bot_loot_items;
std::vector<std::string>::iterator silk_bot_loot_items_it;

std::vector<std::string> silk_bot_trash_mobs;
std::vector<std::string>::iterator silk_bot_trash_mobs_it;

int random_number(int i)
{
    std::srand(unsigned(std::time(0)));

    return std::rand() % i;
}

DWORD WINAPI thread_function(LPVOID param)
{
    if (everquest_function_is_in_game() == false)
    {
        FreeLibraryAndExitThread(module_global, 0);
        return 0;
    }

    everquest_chat_helper_write_chat_text(APPLICATION_NAME " loaded.");
    Sleep(1000);

    everquest_function_set_target_spawn_info(EVERQUEST_SPAWN_INFO_NULL);

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

            if
            (
                (KEYDOWN(VK_CONTROL)) &&
                (KEYDOWN(VK_ALT)) &&
                (KEYDOWN(VK_P))
            )
            {
                everquest_toggle_bool(silk_bot_activated);

                if (silk_bot_activated == true)
                {
                    everquest_chat_helper_write_chat_text(APPLICATION_NAME " activated.");
                }
                else
                {
                    everquest_chat_helper_write_chat_text(APPLICATION_NAME " deactivated.");
                }

                Sleep(1000);
            }
        }

        if (silk_bot_activated == false)
        {
            Sleep(100);
            continue;
        }

        int zone_id = everquest_function_get_zone_id();

        if (zone_id == EVERQUEST_ZONE_ID_POKNOWLEDGE || zone_id == EVERQUEST_ZONE_ID_BAZAAR)
        {
            Sleep(100);
            continue;
        }

        if (zone_id == EVERQUEST_ZONE_ID_FREPORTW)
        {
            everquest_function_warp_to_coordinates(37.05, 805, -24.87, 128);
            Sleep(100);
            continue;
        }

        if (zone_id == EVERQUEST_ZONE_ID_COMMONS)
        {
            everquest_function_warp_to_coordinates(9.98, -1655, -72.55, 384);
            Sleep(100);
            continue;
        }

        if (zone_id != EVERQUEST_ZONE_ID_ECOMMONS)
        {
            Sleep(100);
            continue;
        }

        int player_spawn_info = everquest_function_get_player_spawn_info();
        int target_spawn_info = everquest_function_get_target_spawn_info();

        if (target_spawn_info == player_spawn_info)
        {
            everquest_function_set_target_spawn_info(EVERQUEST_SPAWN_INFO_NULL);
            continue;
        }

        if (target_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            /*
            int num_player_spawns = 0;

            std::vector<everquest_spawn_t> spawns;
            std::vector<everquest_spawn_t>::iterator spawns_it;

            everquest_function_update_spawns(spawns);

            if (spawns.size())
            {
                for (spawns_it = spawns.begin() ; spawns_it != spawns.end(); spawns_it++)
                {
                    if (spawns_it->type == EVERQUEST_SPAWN_INFO_TYPE_PLAYER)
                    {
                        if (spawns_it->level >= 20)
                        {
                            num_player_spawns++;
                        }
                    }
                }
            }

            if (num_player_spawns > 0)
            {
                Sleep(100);
                continue;
            }
            */

            int spawn_info = everquest_function_get_spawn_info_by_name("spider", true, true, true);

            if (spawn_info == EVERQUEST_SPAWN_INFO_NULL)
            {
                std::random_shuffle(silk_bot_loot_mobs.begin(), silk_bot_loot_mobs.end(), random_number);

                for (silk_bot_loot_mobs_it = silk_bot_loot_mobs.begin(); silk_bot_loot_mobs_it != silk_bot_loot_mobs.end(); silk_bot_loot_mobs_it++)
                {
                    spawn_info = everquest_function_get_spawn_info_by_name(*silk_bot_loot_mobs_it, true, true, true);

                    if (spawn_info != EVERQUEST_SPAWN_INFO_NULL)
                    {
                        break;
                    }
                }

                if (spawn_info == EVERQUEST_SPAWN_INFO_NULL)
                {
                    for (silk_bot_trash_mobs_it = silk_bot_trash_mobs.begin(); silk_bot_trash_mobs_it != silk_bot_trash_mobs.end(); silk_bot_trash_mobs_it++)
                    {
                        spawn_info = everquest_function_get_spawn_info_by_name(*silk_bot_trash_mobs_it, true, true, true);

                        if (spawn_info != EVERQUEST_SPAWN_INFO_NULL)
                        {
                            break;
                        }
                    }
                }
            }

            if (spawn_info != EVERQUEST_SPAWN_INFO_NULL)
            {
                unsigned char spawn_type = everquest_function_read_memory<BYTE>(spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_TYPE);

                if (spawn_type != EVERQUEST_SPAWN_INFO_TYPE_CORPSE)
                {
                    everquest_function_set_target_spawn_info(spawn_info);
                }

                continue;
            }
        }
        else
        {
            target_spawn_info = everquest_function_get_target_spawn_info();

            everquest_function_face_spawn_info(target_spawn_info);

            unsigned char target_type = everquest_function_read_memory<BYTE>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_TYPE);

            if (target_type == EVERQUEST_SPAWN_INFO_TYPE_PLAYER)
            {
                everquest_function_set_target_spawn_info(EVERQUEST_SPAWN_INFO_NULL);
                continue;
            }

            float target_movement_speed = everquest_function_read_memory<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_MOVEMENT_SPEED);

            float target_distance = everquest_function_get_target_distance();

            if (target_movement_speed != 0.0 && target_distance > silk_bot_melee_distance)
            {
                everquest_function_warp_to_spawn_info(target_spawn_info);
                Sleep(100);
            }

            if (target_movement_speed == 0.0 && target_distance > (float)(silk_bot_melee_distance * 2.0))
            {
                everquest_function_warp_to_spawn_info(target_spawn_info);
                Sleep(100);
            }

            if (target_type != EVERQUEST_SPAWN_INFO_TYPE_CORPSE)
            {
                everquest_function_set_auto_attack(true);

                if (target_movement_speed == 0.0)
                {
                    float target_distance = everquest_function_get_target_distance();

                    if (target_distance < (float)(silk_bot_melee_distance - 1.0))
                    {
                        everquest_function_adjust_player_distance_from_target((float)(silk_bot_melee_distance - 1.0), true);
                    }
                }
            }
            else
            {
                Sleep(100);

                everquest_function_set_auto_attack(false);

                std::string target_name = everquest_function_read_memory_string(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_NAME, EVERQUEST_SPAWN_INFO_NAME_SIZE);

                bool loot_mob_found = false;

                for (silk_bot_loot_mobs_it = silk_bot_loot_mobs.begin(); silk_bot_loot_mobs_it != silk_bot_loot_mobs.end(); silk_bot_loot_mobs_it++)
                {
                    std::size_t found = target_name.find(*silk_bot_loot_mobs_it);

                    if (found != std::string::npos)
                    {
                        loot_mob_found = true;
                        break;
                    }
                }

                if (loot_mob_found == true)
                {
                    everquest_chat_helper_write_chat_text(APPLICATION_NAME " --> Looting target.");

                    everquest_function_loot_corpse(target_spawn_info);
                    Sleep(silk_bot_loot_corpse_delay);

                    if (everquest_function_is_loot_window_open() == false)
                    {
                        everquest_chat_helper_write_chat_text(APPLICATION_NAME " --> Error: The loot window did not open.");

                        everquest_function_set_target_spawn_info(EVERQUEST_SPAWN_INFO_NULL);
                        continue;
                    }

                    if (everquest_function_is_loot_window_empty() == true)
                    {
                        everquest_chat_helper_write_chat_text(APPLICATION_NAME " --> Closing loot window because it was empty.");

                        everquest_function_loot_window_close();
                    }

                    everquest_chat_helper_write_chat_text(APPLICATION_NAME " --> Looting items from corpse.");

                    for (silk_bot_loot_items_it = silk_bot_loot_items.begin(); silk_bot_loot_items_it != silk_bot_loot_items.end(); silk_bot_loot_items_it++)
                    {
                        int slot_index = -1;
                
                        slot_index = everquest_function_get_loot_slot_of_item_by_name(*silk_bot_loot_items_it, false);

                        if (slot_index != -1)
                        {
                            bool loot_result = everquest_function_loot_all_items_by_name(*silk_bot_loot_items_it, false);
                        }
                    }

                    Sleep(1000);

                    everquest_chat_helper_write_chat_text(APPLICATION_NAME " --> Closing loot window.");

                    everquest_function_loot_window_close();
                }
                {
                    everquest_chat_helper_write_chat_text(APPLICATION_NAME " --> Skipping looting target.");
                }

                everquest_function_set_target_spawn_info(EVERQUEST_SPAWN_INFO_NULL);
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

    silk_bot_loot_mobs.push_back("spider");
    silk_bot_loot_mobs.push_back("cat");
    silk_bot_loot_mobs.push_back("lion");
    silk_bot_loot_mobs.push_back("wolf");
    silk_bot_loot_mobs.push_back("bear");
    silk_bot_loot_mobs.push_back("kodiak");

    silk_bot_loot_items.push_back("Spider Silk");
    silk_bot_loot_items.push_back("Spiderling Silk");

    silk_bot_loot_items.push_back("Superb Cat Pelt");
    silk_bot_loot_items.push_back("High Quality Cat Pelt");
    silk_bot_loot_items.push_back("Medium Quality Cat Pelt");
    silk_bot_loot_items.push_back("Low Quality Cat Pelt");

    silk_bot_loot_items.push_back("Superb Lion Skin");
    silk_bot_loot_items.push_back("High Quality Lion Skin");
    silk_bot_loot_items.push_back("Medium Quality Lion Skin");
    silk_bot_loot_items.push_back("Low Quality Lion Skin");

    silk_bot_loot_items.push_back("Superb Cougarskin");
    silk_bot_loot_items.push_back("High Quality Cougarskin");
    silk_bot_loot_items.push_back("Medium Quality Cougarskin");
    silk_bot_loot_items.push_back("Low Quality Cougarskin");

    silk_bot_loot_items.push_back("Superb Bear Skin");
    silk_bot_loot_items.push_back("High Quality Bear Skin");
    silk_bot_loot_items.push_back("Medium Quality Bear Skin");
    silk_bot_loot_items.push_back("Low Quality Bear Skin");

    silk_bot_loot_items.push_back("Superb Wolf Skin");
    silk_bot_loot_items.push_back("High Quality Wolf Skin");
    silk_bot_loot_items.push_back("Medium Quality Wolf Skin");
    silk_bot_loot_items.push_back("Low Quality Wolf Skin");

    silk_bot_trash_mobs.push_back("decaying_skeleton");
    silk_bot_trash_mobs.push_back("a_fire_beetle");
    silk_bot_trash_mobs.push_back("orc_pawn");
    silk_bot_trash_mobs.push_back("a_moss_snake");
    silk_bot_trash_mobs.push_back("a_leaf_scarab");
    silk_bot_trash_mobs.push_back("wolf");
    silk_bot_trash_mobs.push_back("cat");
    silk_bot_trash_mobs.push_back("lion");
    silk_bot_trash_mobs.push_back("bear");

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
