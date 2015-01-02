#include <string>
#include <sstream>

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

#define APPLICATION_NAME "EQMac Function Test"

DWORD WINAPI thread_function(LPVOID param)
{
    if (everquest_function_is_in_game() == false)
    {
        FreeLibraryAndExitThread(module_global, 0);
        return 0;
    }

    everquest_chat_helper_write_chat_text(APPLICATION_NAME " loaded.");
    Sleep(1000);

    int target_spawn_info = everquest_function_get_target_spawn_info();

    if (target_spawn_info != EVERQUEST_SPAWN_INFO_NULL)
    {
        everquest_function_trade_window_activate(target_spawn_info, true);
        Sleep(5000);
    }

    //unsigned short player_mana_max = everquest_function_get_player_mana_max();

    //std::stringstream mana_text;
    //mana_text << "Max Mana: " << player_mana_max;

    //everquest_chat_helper_write_chat_text(mana_text.str().c_str());

    //int target_spawn_info = everquest_function_get_target_spawn_info();
    //everquest_object_CEverQuest->LootCorpse((EQPlayer *)target_spawn_info, 1);

    //everquest_object_CLootWnd->RequestLootSlot(0, true);

    //everquest_object_CLootWnd->Deactivate();

    //everquest_function_warp_to_coordinates(-1001.81, -45.55, -7.47, 0);

    //everquest_function_set_player_position(EVERQUEST_STANDING_STATE_SITTING);

    // repop at bind point at PoK
    //everquest_object_CEverQuest->MoveToZone(EVERQUEST_ZONE_ID_POKNOWLEDGE, "repop to home at death", 1, 10);

    // gate to PoK
    //char* result = everquest_object_CEverQuest->MoveToZone(EVERQUEST_ZONE_ID_POKNOWLEDGE, "gate spell", 0, 6);

    // teleport to PoK
    //char* result = everquest_object_CEverQuest->MoveToZone("poknowledge", "MSG_TELEPORT_PC", 0, 1);

    /*
    int spawn_info = everquest_function_get_spawn_info_by_name("spider", true, true);

    if (spawn_info != EVERQUEST_SPAWN_INFO_NULL)
    {
        everquest_function_set_target_spawn_info(spawn_info);

        everquest_function_warp_to_spawn_info(spawn_info);

        everquest_function_adjust_player_distance_from_target(5.0, true);
    }
    */

    /*
    std::vector<everquest_spawn_t> spawns;
    std::vector<everquest_spawn_t>::iterator spawns_it;

    everquest_function_update_spawns(spawns);

    if (spawns.size())
    {
        std::sort(spawns.begin(), spawns.end(), everquest_sort_spawns_by_distance_t());

        for (spawns_it = spawns.begin() ; spawns_it != spawns.end(); ++spawns_it)
        {
            std::stringstream spawn_text;
            spawn_text << "Name: " << spawns_it->name << " " << "(" << "Distance:" << " " << spawns_it->distance << ")";

            everquest_chat_helper_write_chat_text(spawn_text.str().c_str());
        }
    }
    */

    /*
    int target_spawn_info = everquest_function_get_target_spawn_info();

    if (target_spawn_info != EVERQUEST_SPAWN_INFO_NULL)
    {
        everquest_function_loot_corpse(target_spawn_info);
        Sleep(1000);

        if (everquest_function_is_loot_window_empty() == true)
        {
            everquest_function_loot_window_close();
        }

        std::string item_name = "Silk";

        int slot_index = everquest_function_get_loot_slot_of_item_by_name(item_name, true);

        if (slot_index != -1)
        {
            bool loot_result = everquest_function_loot_all_items_by_name(item_name, true);
        }

        everquest_function_loot_window_close();
    }
    */

    /*
    std::vector<everquest_item_t> loot_items;
    std::vector<everquest_item_t>::iterator loot_items_it;

    everquest_function_update_loot_items(loot_items);

    if (loot_items.size())
    {
        for (loot_items_it = loot_items.begin() ; loot_items_it != loot_items.end(); ++loot_items_it)
        {
            everquest_chat_helper_write_chat_text(loot_items_it->name.c_str());
        }
    }
    */

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

            DisableThreadLibraryCalls(module);
            CreateThread(NULL, 0, &thread_load, NULL, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            everquest_chat_helper_remove_detour();
            break;
    }

    return TRUE;
}
