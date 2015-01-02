#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstring>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <boost/math/constants/constants.hpp>

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

bool exit_dll = false;

bool thread_update_zone_is_done        = false;
bool thread_auto_gate_is_done          = false;
bool thread_warp_follow_target_is_done = false;

HANDLE thread_handle_load;
HANDLE thread_handle_update_zone;
HANDLE thread_handle_keys;
HANDLE thread_handle_auto_gate;
HANDLE thread_handle_warp_follow_target;

#define APPLICATION_NAME "EQMac Warp"

const std::string ini_file = "eqmac_warp.ini";

const float PI = boost::math::constants::pi<float>();

std::vector<everquest_spawn_t> everquest_spawns;
std::vector<everquest_spawn_t>::iterator everquest_spawns_it;

struct zone_t
{
    int id;

    std::string name;

    float safe_point_y;
    float safe_point_x;
    float safe_point_z;
};

zone_t zone;

std::string current_zone_short_name = "null";

struct warp_t
{
    int index;

    std::string description;

    float y;
    float x;
    float z;

    float heading;
};

std::vector<warp_t> warps;
std::vector<warp_t>::iterator warps_it;

struct warp_to_spawn_t
{
    int index;

    std::vector<std::string> names;
};

std::vector<warp_to_spawn_t> warps_to_spawns;
std::vector<warp_to_spawn_t>::iterator warps_to_spawns_it;

bool warp_to_spawn_enabled                    = true;
bool warp_to_spawn_do_target_spawn_after_warp = true;
bool warp_to_spawn_skip_corpses               = true;

bool warp_to_target_enabled = true;
std::string warp_to_target_name = "null";

bool warp_follow_target_enabled   = true;
bool warp_follow_target_activated = false;

bool auto_gate_enabled                 = true;
int auto_gate_health_points_minimum    = 250;
float auto_gate_health_percent_minimum = 40;

void load_config()
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);

    warp_to_spawn_enabled                    = pt.get<bool>("WarpToSpawn.bEnabled", warp_to_spawn_enabled);
    warp_to_spawn_do_target_spawn_after_warp = pt.get<bool>("WarpToSpawn.bDoTargetSpawnAfterWarp", warp_to_spawn_do_target_spawn_after_warp);
    warp_to_spawn_skip_corpses               = pt.get<bool>("WarpToSpawn.bSkipCorpses", warp_to_spawn_skip_corpses);

    warp_to_target_enabled = pt.get<bool>("WarpToTarget.bEnabled", warp_to_target_enabled);

    warp_to_target_name = pt.get<std::string>("WarpToTarget.sName", warp_to_target_name);

    warp_follow_target_enabled = pt.get<bool>("WarpFollowTarget.bEnabled", warp_follow_target_enabled);

    auto_gate_enabled                = pt.get<bool>("AutoGate.bEnabled", auto_gate_enabled);
    auto_gate_health_points_minimum  = pt.get<int>("AutoGate.iHealthPointsMinimum", auto_gate_health_points_minimum);
    auto_gate_health_percent_minimum = pt.get<float>("AutoGate.fHealthPercentMinimum", auto_gate_health_percent_minimum);
}

void set_clipboard_text(const char* text)
{
    size_t length = strlen(text) + 1;
    HGLOBAL hmem =  GlobalAlloc(GMEM_MOVEABLE, length);
    memcpy(GlobalLock(hmem), text, length);
    GlobalUnlock(hmem);

    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hmem);
    CloseClipboard();
}

int get_index_by_key()
{
    if (KEYDOWN(VK_1))
    {
        return 1;
    }

    if (KEYDOWN(VK_2))
    {
        return 2;
    }

    if (KEYDOWN(VK_3))
    {
        return 3;
    }

    if (KEYDOWN(VK_4))
    {
        return 4;
    }

    if (KEYDOWN(VK_5))
    {
        return 5;
    }

    if (KEYDOWN(VK_6))
    {
        return 6;
    }

    if (KEYDOWN(VK_7))
    {
        return 7;
    }

    if (KEYDOWN(VK_8))
    {
        return 8;
    }

    if (KEYDOWN(VK_9))
    {
        return 9;
    }

    if (KEYDOWN(VK_0))
    {
        return 10;
    }

    return 0;
}

void load_zone_config()
{
    zone.id           = 0;
    zone.name         = "null";
    zone.safe_point_y = 0.0;
    zone.safe_point_x = 0.0;
    zone.safe_point_z = 0.0;

    warps.clear();
    warps_to_spawns.clear();

    if (current_zone_short_name == "null")
    {
        return;
    }

    std::stringstream zone_ini_file;
    zone_ini_file << "warps/" << current_zone_short_name << ".ini";

    std::fstream file;
    file.open(zone_ini_file.str().c_str(), std::ios::in);

    if (!file.is_open())
    {
        return;
    }

    file.close();

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(zone_ini_file.str(), pt);

    zone.id           = pt.get<int>("Zone.ID");
    zone.name         = pt.get<std::string>("Zone.Name");
    zone.safe_point_y = pt.get<float>("Zone.SafePointY");
    zone.safe_point_x = pt.get<float>("Zone.SafePointX");
    zone.safe_point_z = pt.get<float>("Zone.SafePointZ");

    int i = 0;

    for (i = 0; i < 10; i++)
    {
        int index = i + 1;

        std::stringstream key;
        key << "Warps." << index;

        std::string warps_value = pt.get<std::string>(key.str());

        if (warps_value.size())
        {
            std::vector<std::string> warps_data;
            boost::split(warps_data, warps_value, boost::is_any_of(","));

            if (warps_data.size() == 5)
            {
                warp_t warp;

                warp.index       = index;
                warp.description = warps_data.at(0);
                warp.y           = boost::lexical_cast<float>(warps_data.at(1));
                warp.x           = boost::lexical_cast<float>(warps_data.at(2));
                warp.z           = boost::lexical_cast<float>(warps_data.at(3));
                warp.heading     = boost::lexical_cast<float>(warps_data.at(4));

                warps.push_back(warp);
            }
        }
    }

    for (i = 0; i < 10; i++)
    {
        int index = i + 1;

        std::stringstream key;
        key << "WarpToSpawn." << index;

        std::string warp_to_spawn_value = pt.get<std::string>(key.str());

        if (warp_to_spawn_value.size())
        {
            std::vector<std::string> warp_to_spawn_data;
            boost::split(warp_to_spawn_data, warp_to_spawn_value, boost::is_any_of(","));

            if (warp_to_spawn_data.size())
            {
                warp_to_spawn_t warp_to_spawn;

                warp_to_spawn.index = index;
                warp_to_spawn.names = warp_to_spawn_data;

                warps_to_spawns.push_back(warp_to_spawn);
            }
        }
    }
}

void update_spawns()
{
    everquest_function_update_spawns(everquest_spawns);
}

void update_zone()
{
    std::string zone_short_name = everquest_function_get_zone_short_name();

    if (zone_short_name.size() == 0)
    {
        return;
    }

    if (zone_short_name == current_zone_short_name)
    {
        return;
    }

    current_zone_short_name.assign(zone_short_name);

    load_zone_config();
}

void thread_keys(HANDLE module)
{
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
                //exit_dll = true;
                //break;
            }

            if
            (
                (KEYDOWN(VK_CONTROL)) &&
                (KEYDOWN(VK_ALT)) &&
                (KEYDOWN(VK_G))
            )
            {
                everquest_chat_helper_write_chat_text(APPLICATION_NAME " is gating to bind point.");

                everquest_function_gate_to_bind_point();

                Sleep(1000);
            }

            if
            (
                (KEYDOWN(VK_CONTROL)) &&
                (KEYDOWN(VK_ALT)) &&
                (KEYDOWN(VK_F))
            )
            {
                if (warp_follow_target_enabled == true)
                {
                    everquest_toggle_bool(warp_follow_target_activated);

                    if (warp_follow_target_activated == true)
                    {
                        everquest_chat_helper_write_chat_text(APPLICATION_NAME " Follow Target activated.");
                    }
                    else
                    {
                        everquest_chat_helper_write_chat_text(APPLICATION_NAME " Follow Target deactivated.");
                    }

                    Sleep(1000);
                }
            }

            if (KEYDOWN(VK_W) && KEYDOWN(VK_SHIFT))
            {
                int player_spawn_info = everquest_function_get_player_spawn_info();

                float player_y = everquest_function_read_memory<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
                float player_x = everquest_function_read_memory<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
                float player_z = everquest_function_read_memory<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

                float player_heading = everquest_function_read_memory<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING);

                player_heading = (player_heading * 360) / 512; // convert from 512 degrees to 360 degrees

                player_heading -= 90; // rotate by -90 degrees

                if (player_heading < 0)
                {
                    player_heading += 360;
                }

                float player_heading_radians = player_heading * PI / 180; // convert degrees to radians

                float sub_y = sin(player_heading_radians);
                float add_x = cos(player_heading_radians);

                float multiplier = 10;

                player_y -= sub_y * multiplier;
                player_x += add_x * multiplier;

                everquest_function_warp_to_coordinates(player_y, player_x, player_z, -1);

                Sleep(100);
            }

            if (KEYDOWN(VK_S) && KEYDOWN(VK_SHIFT))
            {
                int player_spawn_info = everquest_function_get_player_spawn_info();

                float player_y = everquest_function_read_memory<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
                float player_x = everquest_function_read_memory<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
                float player_z = everquest_function_read_memory<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

                player_z += 10;

                everquest_function_warp_to_coordinates(player_y, player_x, player_z, -1);

                Sleep(100);
            }

            if (KEYDOWN(VK_F1))
            {
                int player_spawn_info = everquest_function_get_player_spawn_info();

                float player_y = everquest_function_read_memory<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
                float player_x = everquest_function_read_memory<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
                float player_z = everquest_function_read_memory<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

                float player_heading = everquest_function_read_memory<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING);

                std::stringstream player_location;
                player_location << "loc" << "," << player_y << "," << player_x << "," << player_z << "," << player_heading;

                set_clipboard_text(player_location.str().c_str());

                Sleep(1000);
            }

            if (KEYDOWN(VK_F2))
            {
                int zone_id = everquest_function_get_zone_id();

                std::string zone_short_name = everquest_function_get_zone_short_name();
                std::string zone_long_name  = everquest_function_get_zone_long_name();

                float zone_safe_point_y = everquest_function_read_memory<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Y);
                float zone_safe_point_x = everquest_function_read_memory<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_X);
                float zone_safe_point_z = everquest_function_read_memory<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Z);

                std::stringstream zone_info;
                zone_info << "[Zone]"                           << "\r\n";
                zone_info << "ID="         << zone_id           << "\r\n";
                zone_info << "Name="       << zone_long_name    << "\r\n";
                zone_info << "ShortName="  << zone_short_name   << "\r\n";
                zone_info << "SafePointX=" << zone_safe_point_y << "\r\n";
                zone_info << "SafePointY=" << zone_safe_point_x << "\r\n";
                zone_info << "SafePointZ=" << zone_safe_point_z;

                set_clipboard_text(zone_info.str().c_str());

                Sleep(1000);
            }

            if (KEYDOWN(VK_F5))
            {
                current_zone_short_name = "null";

                load_config();

                Sleep(1000);
            }

            if (KEYDOWN(VK_F7))
            {
                if (warp_to_target_enabled == false)
                {
                    Sleep(100);
                    continue;
                }

                int target_spawn_info = everquest_function_get_target_spawn_info();

                if (target_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
                {
                    Sleep(100);
                    continue;
                }

                std::string target_name = everquest_function_read_memory_string(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_NAME, EVERQUEST_SPAWN_INFO_NAME_SIZE);

                float target_y = everquest_function_read_memory<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
                float target_x = everquest_function_read_memory<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
                float target_z = everquest_function_read_memory<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

                float target_heading = everquest_function_read_memory<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING);

                std::stringstream target_location;
                target_location << target_name << "," << target_y << "," << target_x << "," << target_z << "," << target_heading;

                set_clipboard_text(target_location.str().c_str());

                std::stringstream warp_text;
                warp_text << APPLICATION_NAME << " is warping to target: " << "'" << target_name << "'";

                everquest_chat_helper_write_chat_text(warp_text.str().c_str());

                everquest_function_warp_to_coordinates(target_y, target_x, target_z, target_heading);

                Sleep(100);
            }

            if (KEYDOWN(VK_F8))
            {
                if (warp_to_spawn_enabled == false)
                {
                    Sleep(100);
                    continue;
                }

                if (KEYDOWN(VK_SHIFT))
                {
                    everquest_chat_helper_write_chat_text(APPLICATION_NAME " warp to spawn list:");

                    if (warps_to_spawns.size() == 0)
                    {
                        everquest_chat_helper_write_chat_text("No warps to spawns found.");

                        Sleep(1000);
                        continue;
                    }

                    foreach (warp_to_spawn_t warp_to_spawn, warps_to_spawns)
                    {
                        std::stringstream warp_text_ss;
                        warp_text_ss << warp_to_spawn.index << ": ";

                        foreach (std::string warp_to_spawn_name, warp_to_spawn.names)
                        {
                            warp_text_ss << "'" << warp_to_spawn_name << "'" << ", ";
                        }

                        std::string warp_text = warp_text_ss.str();

                        if (warp_text.substr(warp_text.length() - 2, 2) == ", ")
                        {
                            warp_text = warp_text.substr(0, warp_text.length() - 2);
                        }

                        everquest_chat_helper_write_chat_text(warp_text.c_str());
                    }

                    Sleep(1000);
                    continue;
                }

                if (warps_to_spawns.size() == 0)
                {
                    Sleep(100);
                    continue;
                }

                int warp_index = get_index_by_key();

                if (warp_index == 0)
                {
                    Sleep(100);
                    continue;
                }

                update_spawns();

                std::sort(everquest_spawns.begin(), everquest_spawns.end(), everquest_sort_spawns_by_distance_t());

                bool found_spawn = false;

                std::string warp_name = "null";

                float warp_y = 0.0;
                float warp_x = 0.0;
                float warp_z = 0.0;

                float warp_heading = 0.0;

                int warp_target_address = EVERQUEST_SPAWN_INFO_NULL;

                foreach (warp_to_spawn_t warp_to_spawn, warps_to_spawns)
                {
                    if (warp_to_spawn.index == warp_index)
                    {
                        foreach (everquest_spawn_t everquest_spawn, everquest_spawns)
                        {
                            if (warp_to_spawn_skip_corpses == true)
                            {
                                if (everquest_spawn.type == EVERQUEST_SPAWN_INFO_TYPE_CORPSE)
                                {
                                    continue;
                                }
                            }

                            foreach (std::string warp_to_spawn_name, warp_to_spawn.names)
                            {
                                std::size_t found = everquest_spawn.name.find(warp_to_spawn_name);

                                if (found != std::string::npos)
                                {
                                    found_spawn = true;

                                    warp_name = everquest_spawn.name;

                                    warp_y = everquest_spawn.y;
                                    warp_x = everquest_spawn.x;
                                    warp_z = everquest_spawn.z;

                                    warp_heading = everquest_spawn.heading;

                                    warp_target_address = everquest_spawn.address;

                                    break;
                                }
                            }

                            if (found_spawn == true)
                            {
                                break;
                            }
                        }
                    }

                    if (found_spawn == true)
                    {
                        break;
                    }
                }

                if (found_spawn == false)
                {
                    continue;
                }

                std::stringstream warp_text_ss;
                warp_text_ss << APPLICATION_NAME << " is warping to spawn: " << "'" << warp_name << "'";

                everquest_chat_helper_write_chat_text(warp_text_ss.str().c_str());

                everquest_function_warp_to_coordinates(warp_y, warp_x, warp_z, warp_heading);

                if (warp_to_spawn_do_target_spawn_after_warp == true)
                {
                    if (warp_target_address != EVERQUEST_SPAWN_INFO_NULL)
                    {
                        everquest_function_write_memory<DWORD>(EVERQUEST_TARGET_SPAWN_INFO_POINTER, warp_target_address);
                    }
                }

                Sleep(1000);
            }

            if (KEYDOWN(VK_F12))
            {
                if (KEYDOWN(VK_SHIFT))
                {
                    everquest_chat_helper_write_chat_text(APPLICATION_NAME " warps list:");

                    if (warps.size() == 0)
                    {
                        everquest_chat_helper_write_chat_text("No warps found.");

                        Sleep(1000);
                        continue;
                    }

                    foreach (warp_t warp, warps)
                    {
                        std::stringstream warp_text_ss;
                        warp_text_ss << warp.index << ": " << warp.description;

                        everquest_chat_helper_write_chat_text(warp_text_ss.str().c_str());
                    }

                    Sleep(1000);
                    continue;
                }

                if (warps.size() == 0)
                {
                    Sleep(100);
                    continue;
                }

                int warp_index = get_index_by_key();

                if (warp_index == 0)
                {
                    Sleep(100);
                    continue;
                }

                foreach (warp_t warp, warps)
                {
                    if (warp.index == warp_index)
                    {
                        std::stringstream warp_text_ss;
                        warp_text_ss << APPLICATION_NAME << " is warping to " << warp.description << ".";

                        everquest_chat_helper_write_chat_text(warp_text_ss.str().c_str());

                        everquest_function_warp_to_coordinates(warp.y, warp.x, warp.z, warp.heading);
                        break;
                    }
                }

                Sleep(1000);
            }
        }

        Sleep(10);
    }

    if (exit_dll == true)
    {
        while (1)
        {
            if
            (
                (thread_update_zone_is_done          == true) &&
                (thread_auto_gate_is_done            == true) &&
                (thread_warp_follow_target_is_done   == true)
            )
            {
                Sleep(100);
                break;
            }

            Sleep(10);
        }

        everquest_chat_helper_write_chat_text(APPLICATION_NAME " unloaded.");
        Sleep(1000);

        FreeLibraryAndExitThread(module_global, 0);
        return;
    }
}

void thread_update_zone(HANDLE module)
{
    while (1)
    {
        if (exit_dll == true)
        {
            break;
        }

        if (everquest_function_is_in_game() == false)
        {
            Sleep(100);
            continue;
        }

        update_zone();

        Sleep(100);
    }

    thread_update_zone_is_done = true;
    return;
}

void thread_auto_gate(HANDLE module)
{
    while (1)
    {
         if (exit_dll == true)
        {
            break;
        }

        if (everquest_function_is_in_game() == false)
        {
            Sleep(100);
            continue;
        }

        if (auto_gate_enabled == false)
        {
            Sleep(100);
            continue;
        }

        int target_spawn_info = everquest_function_get_target_spawn_info();

        if (target_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            Sleep(100);
            continue;
        }

        int player_spawn_info = everquest_function_get_player_spawn_info();

        int player_hp_current = everquest_function_read_memory<DWORD>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HP_CURRENT);
        int player_hp_max     = everquest_function_read_memory<DWORD>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HP_MAX);

        float player_hp_percent = (float)((player_hp_current * 100) / player_hp_max);

        if (player_hp_max <= auto_gate_health_points_minimum)
        {
            auto_gate_health_points_minimum = (int)(player_hp_max / auto_gate_health_percent_minimum);
        }

        if (player_hp_current > auto_gate_health_points_minimum)
        {
            Sleep(100);
            continue;
        }

        if (player_hp_percent > auto_gate_health_percent_minimum)
        {
            Sleep(100);
            continue;
        }

        if (auto_gate_health_points_minimum <= 0)
        {
            Sleep(100);
            continue;
        }

        if (auto_gate_health_percent_minimum <= 0.0)
        {
            Sleep(100);
            continue;
        }

        warp_follow_target_activated = false;

        everquest_chat_helper_write_chat_text(APPLICATION_NAME " is automatically gating to bind point.");

        everquest_function_gate_to_bind_point();

        everquest_function_set_target_spawn_info(EVERQUEST_SPAWN_INFO_NULL);

        Sleep(1000);
    }

    thread_auto_gate_is_done = true;
    return;
}

void thread_warp_follow_target(HANDLE module)
{
    while (1)
    {
        if (exit_dll == true)
        {
            break;
        }

        if (everquest_function_is_in_game() == false)
        {
            Sleep(100);
            continue;
        }

        if (warp_follow_target_enabled == false)
        {
            Sleep(100);
            continue;
        }

        if (warp_follow_target_activated == false)
        {
            Sleep(100);
            continue;
        }

        int target_spawn_info = EVERQUEST_SPAWN_INFO_NULL;

        if (warp_to_target_name == "null" || warp_to_target_name.size() == 0)
        {
            target_spawn_info = everquest_function_get_target_spawn_info();
        }
        else
        {
            update_spawns();

            foreach (everquest_spawn_t everquest_spawn, everquest_spawns)
            {
                if (everquest_spawn.name == warp_to_target_name)
                {
                    target_spawn_info = everquest_spawn.address;
                    break;
                }
            }
        }

        if (target_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            Sleep(100);
            continue;
        }

        int player_spawn_info = everquest_function_get_player_spawn_info();

        std::string player_name = everquest_function_read_memory_string(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_NAME, EVERQUEST_SPAWN_INFO_NAME_SIZE);

        if (player_name == warp_to_target_name)
        {
            Sleep(100);
            continue;
        }

        unsigned char player_class = everquest_function_read_memory<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_CLASS);

        if (player_class != EVERQUEST_CLASS_BARD)
        {
            int player_actor_info = everquest_function_get_player_actor_info();

            unsigned short player_casting_spell_id = everquest_function_read_memory<WORD>(player_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_CASTING_SPELL_ID);

            if (player_casting_spell_id != EVERQUEST_CASTING_SPELL_ID_NULL)
            {
                Sleep(100);
                continue;
            }
        }

        float player_y = everquest_function_read_memory<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
        float player_x = everquest_function_read_memory<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);

        float target_y = everquest_function_read_memory<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
        float target_x = everquest_function_read_memory<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
        float target_z = everquest_function_read_memory<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

        float target_heading = everquest_function_read_memory<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING);

        unsigned char target_type = everquest_function_read_memory<BYTE>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_TYPE);

        if (target_type == EVERQUEST_SPAWN_INFO_TYPE_PLAYER)
        {
            int target_actor_info = everquest_function_read_memory<DWORD>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER);

            unsigned char target_is_not_moving = everquest_function_read_memory<BYTE>(target_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_NOT_MOVING);

            if (target_is_not_moving > 0)
            {
                float target_distance = everquest_calculate_distance(player_x, player_y, target_x, target_y);

                if (target_distance < 10.0)
                {
                    Sleep(100);
                    continue;
                }
            }
        }

        everquest_function_warp_to_coordinates(target_y, target_x, target_z, target_heading);

        Sleep(100);
    }

    thread_warp_follow_target_is_done = true;
    return;
}

void thread_load(HANDLE module)
{
    mem.enable_debug_privileges();
    mem.set_process_by_id(GetCurrentProcessId());

    if (mem.get_process_id() == 0)
    {
        FreeLibraryAndExitThread(module_global, 0);
        return;
    }

    load_config();

    thread_handle_keys               = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_keys,               module_global, 0, NULL);
    thread_handle_update_zone        = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_update_zone,        module_global, 0, NULL);
    thread_handle_auto_gate          = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_auto_gate,          module_global, 0, NULL);
    thread_handle_warp_follow_target = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_warp_follow_target, module_global, 0, NULL);

    everquest_chat_helper_write_chat_text(APPLICATION_NAME " loaded.");

    return;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    module_global = module;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            everquest_chat_helper_detour_function();

            DisableThreadLibraryCalls(module);
            thread_handle_load = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_load, module_global, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            everquest_chat_helper_remove_detour();
            break;
    }

    return TRUE;
}
