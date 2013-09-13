#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <boost/math/constants/constants.hpp>

#include <boost/range/algorithm.hpp>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include <windows.h>

#include "vk_keys.h"

#include "memory.hpp"

#include "eqmac.hpp"

const float PI = boost::math::constants::pi<float>();

const std::string ini_file = "eqmac_warp.ini";

#define APPLICATION_NAME       "EQMac Warp"
#define APPLICATION_CLASS_NAME "eqmac_warp"
#define APPLICATION_DLL_NAME   "eqmac_warp.dll"

#define EVERQUEST_CLIENT "eqgame.exe"
#define EVERQUEST_TITLE  "EQW beta 2.32"

bool exit_dll = false;

memory memory;

HMODULE module_global;

HANDLE thread_handle_load;
HANDLE thread_handle_update_zone;
HANDLE thread_handle_keys;
HANDLE thread_handle_auto_gate;
HANDLE thread_handle_warp_follow_target;

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

bool warp_follow_target_enabled   = true;
bool warp_follow_target_activated = false;

bool auto_gate_enabled                 = true;
int auto_gate_health_points_minimum    = 200;
float auto_gate_health_percent_minimum = 10;

void toggle_bool(bool &b)
{
    b = !b;
}

struct sort_spawns_by_distance_t
{
    bool operator() (const everquest_spawn_t& everquest_spawn1, const everquest_spawn_t& everquest_spawn2)
    {
        return everquest_spawn1.distance > everquest_spawn2.distance;
    }
};

int calculate_index_by_key()
{
    if (GetAsyncKeyState(VK_1))
    {
        return 1;
    }

    if (GetAsyncKeyState(VK_2))
    {
        return 2;
    }

    if (GetAsyncKeyState(VK_3))
    {
        return 3;
    }

    if (GetAsyncKeyState(VK_4))
    {
        return 4;
    }

    if (GetAsyncKeyState(VK_5))
    {
        return 5;
    }

    if (GetAsyncKeyState(VK_6))
    {
        return 6;
    }

    if (GetAsyncKeyState(VK_7))
    {
        return 7;
    }

    if (GetAsyncKeyState(VK_8))
    {
        return 8;
    }

    if (GetAsyncKeyState(VK_9))
    {
        return 9;
    }

    if (GetAsyncKeyState(VK_0))
    {
        return 10;
    }

    return 0;
}

void load_config()
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);

    warp_to_spawn_enabled                    = pt.get<bool>("WarpToSpawn.bEnabled", true);
    warp_to_spawn_do_target_spawn_after_warp = pt.get<bool>("WarpToSpawn.bDoTargetSpawnAfterWarp", true);
    warp_to_spawn_skip_corpses               = pt.get<bool>("WarpToSpawn.bSkipCorpses", true);

    warp_to_target_enabled = pt.get<bool>("WarpToTarget.bEnabled", true);

    warp_follow_target_enabled = pt.get<bool>("WarpFollowTarget.bEnabled", true);

    auto_gate_enabled                = pt.get<bool>("AutoGate.bEnabled", true);
    auto_gate_health_points_minimum  = pt.get<int>("AutoGate.iHealthPointsMinimum");
    auto_gate_health_percent_minimum = pt.get<float>("AutoGate.fHealthPercentMinimum");
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
    if (memory.get_process_id() == 0)
    {
        return;
    }

    everquest_update_spawns(memory, everquest_spawns);
}

void update_zone()
{
    if (memory.get_process_id() == 0)
    {
        return;
    }

    std::string zone_short_name = everquest_get_zone_short_name(memory);

    if (zone_short_name.size() == 0)
    {
        return;
    }

    boost::algorithm::to_lower(zone_short_name);

    if (zone_short_name == current_zone_short_name)
    {
        return;
    }

    current_zone_short_name.assign(zone_short_name);

    load_zone_config();
}

void wait_for_thread(HANDLE thread)
{
    if (thread == NULL)
    {
        return;
    }

    DWORD exit_code;
    GetExitCodeThread(thread, &exit_code);

    if (exit_code == STILL_ACTIVE)
    {
        TerminateThread(thread, 0);
        CloseHandle(thread);
    }
}

void thread_update_zone(HMODULE module)
{
    while (1)
    {

        if (exit_dll == true)
        {
            return;
        }

        if (memory.get_process_id() == 0)
        {
            exit_dll == true;
            return;
        }

        update_zone();

        Sleep(100);
    }
}

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
            exit_dll == true;
            break;
        }

        if (memory.is_foreground_window_current_process_id() == false)
        {
            Sleep(100);
            continue;
        }

        if
        (
            (GetAsyncKeyState(VK_CONTROL)) &&
            (GetAsyncKeyState(VK_ALT)) &&
            (GetAsyncKeyState(VK_BACK))
        )
        {
            if (warp_follow_target_activated == false)
            {
                exit_dll = true;
                break;
            }
        }

        if
        (
            (GetAsyncKeyState(VK_CONTROL)) &&
            (GetAsyncKeyState(VK_ALT)) &&
            (GetAsyncKeyState(VK_G))
        )
        {
            everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " is instant gating.");

            int player_spawn_info = everquest_get_player_spawn_info(memory);

            memory.write_any<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_TYPE, EVERQUEST_SPAWN_INFO_TYPE_CORPSE);

            Sleep(1000);
        }

        if
        (
            (GetAsyncKeyState(VK_CONTROL)) &&
            (GetAsyncKeyState(VK_ALT)) &&
            (GetAsyncKeyState(VK_F))
        )
        {
            if (warp_follow_target_enabled == true)
            {
                toggle_bool(warp_follow_target_activated);

                if (warp_follow_target_activated == true)
                {
                    everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " Follow Target activated.");
                }
                else
                {
                    everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " Follow Target deactivated.");
                }

                Sleep(1000);
            }
        }

        if (GetAsyncKeyState(VK_SUBTRACT))
        {
            bool warp_reverse = false;

            if (GetAsyncKeyState(VK_SHIFT))
            {
                warp_reverse = true;
            }
            else
            {
                warp_reverse = false;
            }

            int player_spawn_info = everquest_get_player_spawn_info(memory);

            float player_y = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
            float player_x = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
            float player_z = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

            if (warp_reverse == true)
            {
                player_z += 10;
            }
            else
            {
                player_z -= 10;
            }

            everquest_warp_to_coordinates(memory, player_y, player_x, player_z, -1);

            Sleep(1000);
        }

        if (GetAsyncKeyState(VK_DIVIDE))
        {
            int player_spawn_info = everquest_get_player_spawn_info(memory);

            float player_y = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
            float player_x = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
            float player_z = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

            float player_heading = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING);

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

            everquest_warp_to_coordinates(memory, player_y, player_x, player_z, -1);

            Sleep(1000);
        }

        if (GetAsyncKeyState(VK_F5))
        {
            current_zone_short_name = "null";

            Sleep(1000);
        }

        if (GetAsyncKeyState(VK_F12))
        {
            if (GetAsyncKeyState(VK_SHIFT))
            {
                everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " warps list:");

                if (warps.size() == 0)
                {
                    everquest_function_CEverQuest__dsp_chat("No warps found.");
                    Sleep(1000);

                    continue;
                }

                foreach (warp_t warp, warps)
                {
                    std::stringstream warp_text_ss;
                    warp_text_ss << warp.index << ": " << warp.description;

                    everquest_function_CEverQuest__dsp_chat((char*)warp_text_ss.str().c_str());
                }

                Sleep(1000);

                continue;
            }

            if (warps.size() == 0)
            {
                continue;
            }

            int warp_index = calculate_index_by_key();

            if (warp_index == 0)
            {
                continue;
            }

            foreach (warp_t warp, warps)
            {
                if (warp.index == warp_index)
                {
                    std::stringstream warp_text_ss;
                    warp_text_ss << APPLICATION_NAME << " is warping to " << warp.description << ".";

                    everquest_function_CEverQuest__dsp_chat((char*)warp_text_ss.str().c_str());

                    everquest_warp_to_coordinates(memory, warp.y, warp.x, warp.z, warp.heading);
                    break;
                }
            }

            Sleep(1000);
        }

        if (GetAsyncKeyState(VK_F8))
        {
            if (warp_to_spawn_enabled == false)
            {
                continue;
            }

            if (GetAsyncKeyState(VK_SHIFT))
            {
                everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " warp to spawn list:");

                if (warps_to_spawns.size() == 0)
                {
                    everquest_function_CEverQuest__dsp_chat("No warps to spawns found.");
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

                    everquest_function_CEverQuest__dsp_chat((char*)warp_text.c_str());
                }

                Sleep(1000);

                continue;
            }

            if (warps_to_spawns.size() == 0)
            {
                continue;
            }

            int warp_index = calculate_index_by_key();

            if (warp_index == 0)
            {
                continue;
            }

            update_spawns();

            std::sort(everquest_spawns.begin(), everquest_spawns.end(), sort_spawns_by_distance_t());

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
                                warp_z = everquest_spawn.z + 0.1;

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

            everquest_function_CEverQuest__dsp_chat((char*)warp_text_ss.str().c_str());

            everquest_warp_to_coordinates(memory, warp_y, warp_x, warp_z, warp_heading);

            if (warp_to_spawn_do_target_spawn_after_warp == true)
            {
                if (warp_target_address != EVERQUEST_SPAWN_INFO_NULL)
                {
                    memory.write_any<DWORD>(EVERQUEST_TARGET_SPAWN_INFO_POINTER, warp_target_address);
                }
            }

            Sleep(1000);
        }

        if (GetAsyncKeyState(VK_F7))
        {
            if (warp_to_target_enabled == false)
            {
                continue;
            }

            update_spawns();

            std::sort(everquest_spawns.begin(), everquest_spawns.end(), sort_spawns_by_distance_t());

            bool found_spawn = false;

            float warp_y = 0;
            float warp_x = 0;
            float warp_z = 0;

            float warp_heading = 0;

            int target_address = everquest_get_target_spawn_info(memory);

            std::string target_name;

            foreach (everquest_spawn_t everquest_spawn, everquest_spawns)
            {
                if (everquest_spawn.address == target_address)
                {
                    found_spawn = true;

                    target_name = everquest_spawn.name;

                    warp_y = everquest_spawn.y;
                    warp_x = everquest_spawn.x;
                    warp_z = everquest_spawn.z + 0.1;

                    warp_heading = everquest_spawn.heading;

                    break;
                }
            }

            if (found_spawn == false)
            {
                continue;
            }

            std::stringstream warp_text;
            warp_text << APPLICATION_NAME << " is warping to target: " << "'" << target_name << "'";

            everquest_function_CEverQuest__dsp_chat((char*)warp_text.str().c_str());

            everquest_warp_to_coordinates(memory, warp_y, warp_x, warp_z, warp_heading);

            Sleep(1000);
        }

        Sleep(100);
    }

    if (exit_dll == true)
    {
        everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " unloaded.");

        Sleep(1000);

        wait_for_thread(thread_handle_load);
        wait_for_thread(thread_handle_update_zone);
        wait_for_thread(thread_handle_auto_gate);
        wait_for_thread(thread_handle_warp_follow_target);

        FreeLibraryAndExitThread(module, 0);

        return;
    }
}

void thread_auto_gate(HMODULE module)
{
    while (1)
    {

        if (exit_dll == true)
        {
            return;
        }

        if (memory.get_process_id() == 0)
        {
            exit_dll == true;
            return;
        }

        if (auto_gate_enabled == false)
        {
            Sleep(100);
            continue;
        }

        int target_spawn_info = everquest_get_target_spawn_info(memory);

        if (target_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            Sleep(100);
            continue;
        }

        int player_spawn_info = everquest_get_player_spawn_info(memory);

        int player_hp_current = memory.read_any<DWORD>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HP_CURRENT);
        int player_hp_max     = memory.read_any<DWORD>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HP_MAX);

        float player_hp_percent = (player_hp_current * 100) / player_hp_max;

        if (player_hp_max <= auto_gate_health_points_minimum)
        {
            auto_gate_health_points_minimum = player_hp_max / auto_gate_health_percent_minimum;
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

        everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " is auto gating.");

        memory.write_any<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_TYPE, EVERQUEST_SPAWN_INFO_TYPE_CORPSE);

        Sleep(1000);
    }
}

void thread_warp_follow_target(HMODULE module)
{
    while(1)
    {

        if (exit_dll == true)
        {
            return;
        }

        if (memory.get_process_id() == 0)
        {
            exit_dll == true;
            return;
        }

        if (warp_follow_target_activated == false)
        {
            Sleep(100);
            continue;
        }

        int target_spawn_info = everquest_get_target_spawn_info(memory);

        if (target_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            Sleep(100);
            continue;
        }

        float target_y = memory.read_any<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
        float target_x = memory.read_any<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
        float target_z = memory.read_any<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

        float target_heading = memory.read_any<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING);

        everquest_warp_to_coordinates(memory, target_y, target_x, target_z + 0.1, target_heading);

        Sleep(1000);
    }
}

void thread_load(HMODULE module)
{
    memory.enable_debug_privileges();
    memory.set_process_by_id(GetCurrentProcessId());

    update_zone();

    load_config();
    load_zone_config();

    thread_handle_update_zone        = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_update_zone,        module, 0, NULL);
    thread_handle_keys               = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_keys,               module, 0, NULL);
    thread_handle_warp_follow_target = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_warp_follow_target, module, 0, NULL);
    thread_handle_auto_gate          = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_auto_gate,          module, 0, NULL);

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
