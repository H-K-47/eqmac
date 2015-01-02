#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>

#include <boost/algorithm/string.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <boost/range/algorithm.hpp>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

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

#define APPLICATION_NAME "EQMac Coin Bot"

const std::string ini_file = "eqmac_coin_bot.ini";

//std::fstream log_file;

std::vector<everquest_spawn_t> everquest_spawns;
std::vector<everquest_spawn_t>::iterator everquest_spawns_it;

bool coin_bot_activated = false;

std::string aggro_npc_name = "A_Castrorum_Seniori01";
float aggro_npc_minimum_distance = 250;

std::string get_coin_npc_name = "Examiner_Malikin00";
float get_coin_npc_y;
float get_coin_npc_x;
float get_coin_npc_z;
float get_coin_npc_heading;
float get_coin_npc_warp_y;
float get_coin_npc_warp_x;
float get_coin_npc_warp_z;
float get_coin_npc_warp_heading;

std::string give_coin_npc_name = "Percontorius_Ontu00";
float give_coin_npc_y;
float give_coin_npc_x;
float give_coin_npc_z;
float give_coin_npc_heading;
float give_coin_npc_warp_y;
float give_coin_npc_warp_x;
float give_coin_npc_warp_z;
float give_coin_npc_warp_heading;
int give_coin_npc_mouse_x;
int give_coin_npc_mouse_y;

int give_button_mouse_x;
int give_button_mouse_y;

int hot_button_mouse_x;
int hot_button_mouse_y;

void load_config()
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);

    aggro_npc_name = pt.get<std::string>("AggroNPC.Name");
    aggro_npc_minimum_distance = pt.get<float>("AggroNPC.MinimumDistance");

    get_coin_npc_name         = pt.get<std::string>("GetCoinNPC.Name");
    get_coin_npc_y            = pt.get<float>("GetCoinNPC.Y");
    get_coin_npc_x            = pt.get<float>("GetCoinNPC.X");
    get_coin_npc_z            = pt.get<float>("GetCoinNPC.Z");
    get_coin_npc_heading      = pt.get<float>("GetCoinNPC.Heading");
    get_coin_npc_warp_y       = pt.get<float>("GetCoinNPC.WarpY");
    get_coin_npc_warp_x       = pt.get<float>("GetCoinNPC.WarpX");
    get_coin_npc_warp_z       = pt.get<float>("GetCoinNPC.WarpZ");
    get_coin_npc_warp_heading = pt.get<float>("GetCoinNPC.WarpHeading");
    //get_coin_npc_mouse_x      = pt.get<int>("GetCoinNPC.MouseX");
    //get_coin_npc_mouse_y      = pt.get<int>("GetCoinNPC.MouseY");

    give_coin_npc_name         = pt.get<std::string>("GiveCoinNPC.Name");
    give_coin_npc_y            = pt.get<float>("GiveCoinNPC.Y");
    give_coin_npc_x            = pt.get<float>("GiveCoinNPC.X");
    give_coin_npc_z            = pt.get<float>("GiveCoinNPC.Z");
    give_coin_npc_heading      = pt.get<float>("GiveCoinNPC.Heading");
    give_coin_npc_warp_y       = pt.get<float>("GiveCoinNPC.WarpY");
    give_coin_npc_warp_x       = pt.get<float>("GiveCoinNPC.WarpX");
    give_coin_npc_warp_z       = pt.get<float>("GiveCoinNPC.WarpZ");
    give_coin_npc_warp_heading = pt.get<float>("GiveCoinNPC.WarpHeading");
    give_coin_npc_mouse_x      = pt.get<int>("GiveCoinNPC.MouseX");
    give_coin_npc_mouse_y      = pt.get<int>("GiveCoinNPC.MouseY");

    give_button_mouse_x = pt.get<int>("GiveButton.MouseX");
    give_button_mouse_y = pt.get<int>("GiveButton.MouseY");

    hot_button_mouse_x = pt.get<int>("HotButton.MouseX");
    hot_button_mouse_y = pt.get<int>("HotButton.MouseY");
}

void toggle_bool(bool &b)
{
    b = !b;
}

void update_spawns()
{
    everquest_update_spawns(mem, everquest_spawns);
}

std::string get_target_name()
{
    std::string target_name = "null";

    int target_spawn_info = everquest_get_target_spawn_info(mem);

    if (target_spawn_info != EVERQUEST_SPAWN_INFO_NULL)
    {
        target_name = mem.read_string(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_NAME, EVERQUEST_SPAWN_INFO_NAME_SIZE);
    }

    return target_name;
}

void set_target_by_name(std::string target_name)
{
    update_spawns();

    foreach (everquest_spawn_t everquest_spawn, everquest_spawns)
    {
        if (everquest_spawn.name == target_name)
        {
            mem.write_any<DWORD>(EVERQUEST_TARGET_SPAWN_INFO_POINTER, everquest_spawn.address);

            break;
        }
    }
}

float get_target_distance_by_name(std::string target_name)
{
    update_spawns();

    foreach (everquest_spawn_t everquest_spawn, everquest_spawns)
    {
        if (everquest_spawn.name == target_name)
        {
            return everquest_spawn.distance;
        }
    }

    return -1;
}

float get_target_distance_z_by_name(std::string target_name)
{
    update_spawns();

    foreach (everquest_spawn_t everquest_spawn, everquest_spawns)
    {
        if (everquest_spawn.name == target_name)
        {
            return everquest_spawn.distance_z;

            break;
        }
    }

    return -1;
}

bool check_duck_to_deactivate()
{
    int player_spawn_info = everquest_get_player_spawn_info(mem);

    unsigned char player_standing_state = mem.read_any<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE);

    if (player_standing_state == EVERQUEST_STANDING_STATE_DUCKING)
    {
        coin_bot_activated = false;

        everquest_chat_helper_write_chat_text(APPLICATION_NAME " deactivated.");

        return true;
    }

    return false;
}

bool check_if_sitting()
{
    int player_spawn_info = everquest_get_player_spawn_info(mem);

    unsigned char player_standing_state = mem.read_any<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE);

    if (player_standing_state == EVERQUEST_STANDING_STATE_SITTING)
    {
        return true;
    }

    return false;
}

bool is_target_null()
{
    int target_spawn_info = everquest_get_target_spawn_info(mem);

    if (target_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
    {
        return true;
    }

    return false;
}

void clear_target()
{
    mem.write_any<DWORD>(EVERQUEST_TARGET_SPAWN_INFO_POINTER, EVERQUEST_SPAWN_INFO_NULL);
}

void move_mouse_to_top_left_of_screen()
{
    mem.write_any<DWORD>(EVERQUEST_MOUSE_X, 0);
    mem.write_any<DWORD>(EVERQUEST_MOUSE_Y, 0);
}

void move_mouse_to_center_of_screen()
{
    mem.write_any<DWORD>(EVERQUEST_MOUSE_X, 640 / 2);
    mem.write_any<DWORD>(EVERQUEST_MOUSE_Y, 480 / 2);
}

void mouse_click(int x, int y)
{
    mem.write_any<DWORD>(EVERQUEST_MOUSE_X, x);
    mem.write_any<DWORD>(EVERQUEST_MOUSE_Y, y);

    Sleep(100);

    mem.write_any<DWORD>(EVERQUEST_MOUSE_CLICK_STATE, EVERQUEST_MOUSE_CLICK_STATE_LEFT);

    Sleep(100);

    move_mouse_to_top_left_of_screen();
}

bool is_mouse_cursor_holding_item()
{
    int CDisplay_info = everquest_get_CDisplay(mem);

    unsigned char result = mem.read_any<BYTE>(CDisplay_info + EVERQUEST_OFFSET_CDisplay_IS_HOLDING_ITEM);

    if (result == 0)
    {
        return false;
    }

    return true;
}

bool is_trade_window_open()
{
    int old_ui_info = everquest_get_old_ui_info(mem);

    int result = mem.read_any<DWORD>(old_ui_info + EVERQUEST_OFFSET_OLD_UI_INFO_IS_TRADE_WINDOW_OPEN);

    if (result == 0)
    {
        return false;
    }

    return true;
}

void set_movement_updates(bool enabled)
{
    if (enabled == true)
    {
        mem.write_any<WORD>(0x0053EAC0, 0x9090);
    }
    else
    {
        mem.write_any<WORD>(0x0053EAC0, 0x2E75);
    }
}

bool is_aggro_npc_within_distance()
{
    return false;

    bool is_within_distance = false;

    update_spawns();

    foreach (everquest_spawn_t everquest_spawn, everquest_spawns)
    {
        if (everquest_spawn.name == aggro_npc_name)
        {
            float aggro_npc_distance_from_npc = everquest_calculate_distance(get_coin_npc_x, get_coin_npc_y, everquest_spawn.x, everquest_spawn.y);

            if (aggro_npc_distance_from_npc <= aggro_npc_minimum_distance)
            {
                is_within_distance = true;
                break;
            }
        }
    }

    return is_within_distance;
}

void warp_to_give_coin_npc()
{
    //return;

    everquest_function_warp_to_coordinates(give_coin_npc_warp_y, give_coin_npc_warp_x, give_coin_npc_warp_z, give_coin_npc_warp_heading);

    int player_spawn_info = everquest_get_player_spawn_info(mem);

    float player_y = mem.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
    float player_x = mem.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
    
    float distance = everquest_calculate_distance(player_x, player_y, give_coin_npc_warp_x, give_coin_npc_warp_y);

    if (distance > 1)
    {
        warp_to_give_coin_npc();
    }

    //while (get_target_distance_by_name(give_coin_npc_name) > 100)
    //{
        //warp_to_give_coin_npc();
    //}

    if (get_target_distance_z_by_name(give_coin_npc_name) > 3)
    {
        warp_to_give_coin_npc();
        Sleep(1000);
    }
}

void warp_to_get_coin_npc()
{
    //return;

    //if (is_aggro_npc_within_distance() == true)
    //{
        //warp_to_give_coin_npc();
        //return;
    //}

    everquest_function_warp_to_coordinates(get_coin_npc_warp_y, get_coin_npc_warp_x, get_coin_npc_warp_z, get_coin_npc_warp_heading);

    int player_spawn_info = everquest_get_player_spawn_info(mem);

    float player_y = mem.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
    float player_x = mem.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
    
    float distance = everquest_calculate_distance(player_x, player_y, get_coin_npc_warp_x, get_coin_npc_warp_y);

    if (distance > 1)
    {
        warp_to_get_coin_npc();
    }

    if (get_target_distance_z_by_name(get_coin_npc_name) > 3)
    {
        warp_to_get_coin_npc();
        Sleep(1000);
    }
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

            if
            (
                (KEYDOWN(VK_CONTROL)) &&
                (KEYDOWN(VK_ALT)) &&
                (KEYDOWN(VK_Z))
            )
            {
                toggle_bool(coin_bot_activated);

                //if (is_aggro_npc_within_distance() == true)
                //{
                    //coin_bot_activated = false;

                    //everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " aggro NPC is within distance!");
                //}

                if (coin_bot_activated == true)
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

        if (coin_bot_activated == false)
        {
            Sleep(100);
            continue;
        }

        if (check_if_sitting() == true)
        {
            Sleep(100);
            continue;
        }

        if (check_duck_to_deactivate() == true)
        {
            Sleep(100);
            continue;
        }

        int zone_id = everquest_get_zone_id(mem);

        if (zone_id != EVERQUEST_ZONE_ID_SSERU)
        {
            Sleep(100);
            continue;
        }

        int player_spawn_info = everquest_get_player_spawn_info(mem);

        int player_hp_current = mem.read_any<DWORD>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HP_CURRENT);
        int player_hp_max     = mem.read_any<DWORD>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HP_MAX);

        float player_hp_percent = (float)((player_hp_current * 100) / player_hp_max);

        if (player_hp_current != player_hp_max || player_hp_percent < 90)
        {
            warp_to_give_coin_npc();
            continue;
        }

        clear_target();

        move_mouse_to_top_left_of_screen();

        //if (is_aggro_npc_within_distance() == true)
        //{
            //warp_to_give_coin_npc();
            //continue;
        //}

        if (is_target_null() == true)
        {
            if (is_mouse_cursor_holding_item() == true)
            {
                set_target_by_name(give_coin_npc_name);
                warp_to_give_coin_npc();
            }
            else
            {
                set_target_by_name(get_coin_npc_name);
                warp_to_get_coin_npc();
            }
        }

        if (get_target_name() == give_coin_npc_name)
        {
            if (is_mouse_cursor_holding_item() == false)
            {
                if (is_trade_window_open() == false)
                {
                    set_target_by_name(get_coin_npc_name);
                    warp_to_get_coin_npc();
                }
            }
        }

        if (get_target_name() == get_coin_npc_name)
        {
            while (is_mouse_cursor_holding_item() == false)
            {
                if (is_aggro_npc_within_distance() == true)
                {
                    warp_to_give_coin_npc();
                    break;
                }

                if (check_duck_to_deactivate() == true)
                {
                    break;
                }

                if (get_target_name() == give_coin_npc_name)
                {
                    if (is_mouse_cursor_holding_item() == false)
                    {
                        if (is_trade_window_open() == false)
                        {
                            set_target_by_name(get_coin_npc_name);
                            warp_to_get_coin_npc();
                            break;
                        }
                    }
                }

                mouse_click(hot_button_mouse_x, hot_button_mouse_y);
                Sleep(100);
            }

            if (is_mouse_cursor_holding_item() == true)
            {
                set_target_by_name(give_coin_npc_name);
                warp_to_give_coin_npc();
            }
        }

        if (get_target_name() == give_coin_npc_name)
        {
            while (is_mouse_cursor_holding_item() == true)
            {
                if (check_duck_to_deactivate() == true)
                {
                    break;
                }

                if (is_trade_window_open() == false)
                {
                    mouse_click(give_coin_npc_mouse_x, give_coin_npc_mouse_y);
                    Sleep(100);
                }
            }

            if (is_mouse_cursor_holding_item() == false)
            {
                while (is_trade_window_open() == true)
                {
                    if (check_duck_to_deactivate() == true)
                    {
                        break;
                    }

                    mouse_click(give_button_mouse_x, give_button_mouse_y);
                    Sleep(100);
                }

                if (is_trade_window_open() == false)
                {
                    clear_target();
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
