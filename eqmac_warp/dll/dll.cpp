#define WINVER 0x0500

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
#include <psapi.h>

#include <winbase.h>

#include "memory.hpp"

#include "eqmac.hpp"

const std::string ini_file = "eqmac_warp.ini";

#define APPLICATION_NAME       "EQMac Warp"
#define APPLICATION_CLASS_NAME "eqmac_warp"
#define APPLICATION_DLL_NAME   "eqmac_warp.dll"

#define EVERQUEST_CLIENT "eqgame.exe"
#define EVERQUEST_TITLE  "EQW beta 2.32"

#define WINDOW_HEIGHT 320
#define WINDOW_WIDTH  240

#define ID_TIMER_KEYS      10001
#define ID_TIMER_AUTO_GATE 10002
#define ID_TIMER_FOLLOW    10003

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

#define VK_F 0x46
#define VK_G 0x47

#define VK_ALT VK_MENU

memory memory;

HANDLE module;

HANDLE window_thread;

DWORD window_thread_id;

std::vector<everquest_spawn_t> everquest_spawns;
std::vector<everquest_spawn_t>::iterator everquest_spawns_it;

bool warp_follow_activated = false;

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

std::string warp_to_spawn_name = "";
std::vector<std::string> warp_to_spawn_name_data;

void update_spawns()
{
    if (memory.get_process_id() == 0)
    {
        return;
    }

    everquest_update_spawns(memory, everquest_spawns);
}

void timer_keys(HWND hwnd)
{
    if (memory.get_process_id() == 0)
    {
        return;
    }

    if (memory.is_foreground_window_current_process_id() == false)
    {
        return;
    }

    if
    (
        (GetAsyncKeyState(VK_CONTROL)) &&
        (GetAsyncKeyState(VK_ALT)) &&
        (GetAsyncKeyState(VK_BACK))
    )
    {
        DestroyWindow(hwnd);
    }

    if
    (
        (GetAsyncKeyState(VK_CONTROL)) &&
        (GetAsyncKeyState(VK_ALT)) &&
        (GetAsyncKeyState(VK_G))
    )
    {
        everquest_function_chat_write_ex(APPLICATION_NAME " is instant gating.");

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
        toggle_bool(warp_follow_activated);

        if (warp_follow_activated == true)
        {
            everquest_function_chat_write_ex(APPLICATION_NAME " Follow activated.");
        }
        else
        {
            everquest_function_chat_write_ex(APPLICATION_NAME " Follow deactivated.");
        }

        Sleep(1000);
    }

    if (GetAsyncKeyState(VK_F12))
    {
        bool warp_point_ex = false;

        if (GetAsyncKeyState(VK_SHIFT))
        {
            warp_point_ex = true;
        }
        else
        {
            warp_point_ex = false;
        }

        int player_spawn_info = everquest_get_player_spawn_info(memory);

        std::string zone_short_name = everquest_get_zone_short_name(memory);

        boost::algorithm::to_lower(zone_short_name);

        std::stringstream zone_ini;
        zone_ini << "warps/" << zone_short_name << ".ini";

        std::fstream file;
        file.open(zone_ini.str().c_str(), std::ios::in);

        if (!file.is_open())
        {
            return;
        }

        file.close();

        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(zone_ini.str(), pt);

        std::string warp_description;
        float warp_y, warp_x, warp_z, warp_heading;

        if (warp_point_ex == true)
        {
            warp_description = pt.get<std::string>("WarpPointEx.Description");

            warp_y = pt.get<float>("WarpPointEx.Y");
            warp_x = pt.get<float>("WarpPointEx.X");
            warp_z = pt.get<float>("WarpPointEx.Z");

            warp_heading = pt.get<float>("WarpPointEx.Heading");
        }
        else
        {
            warp_description = pt.get<std::string>("WarpPoint.Description");

            warp_y = pt.get<float>("WarpPoint.Y");
            warp_x = pt.get<float>("WarpPoint.X");
            warp_z = pt.get<float>("WarpPoint.Z");

            warp_heading = pt.get<float>("WarpPoint.Heading");
        }

        std::stringstream warp_text;
        warp_text << APPLICATION_NAME << " is warping to " << warp_description << ".";

        everquest_function_chat_write_ex((char*)warp_text.str().c_str());

        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Y, warp_y);
        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_X, warp_x);
        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Z, warp_z);

        memory.write_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING, warp_heading);

        everquest_function_warp_to_safe_point(EVERQUEST_FUNCTION_WARP_TO_SAFE_POINT_VALUE);

        Sleep(1000);
    }

    if (GetAsyncKeyState(VK_F8))
    {
        bool warp_to_spawn_ex = false;

        if (GetAsyncKeyState(VK_SHIFT))
        {
            warp_to_spawn_ex = true;
        }
        else
        {
            warp_to_spawn_ex = false;
        }

        int player_spawn_info = everquest_get_player_spawn_info(memory);

        std::fstream file;
        file.open(ini_file.c_str(), std::ios::in);

        if (!file.is_open())
        {
            return;
        }

        file.close();

        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(ini_file, pt);

        bool warp_to_spawn_enabled = pt.get<bool>("WarpToSpawn.bEnabled", false);

        if (warp_to_spawn_enabled == false)
        {
            return;
        }

        bool warp_to_spawn_do_target = pt.get<bool>("WarpToSpawn.bDoTarget", false);

        bool warp_to_spawn_skip_corpses = pt.get<bool>("WarpToSpawn.bSkipCorpses", true);

        std::string warp_to_spawn_name_type = "WarpToSpawn.sName";

        if (warp_to_spawn_ex == true)
        {
            warp_to_spawn_name_type = "WarpToSpawn.sNameEx";
            
        }

        warp_to_spawn_name = pt.get<std::string>(warp_to_spawn_name_type, warp_to_spawn_name);

        if (warp_to_spawn_name.size())
        {
            boost::split(warp_to_spawn_name_data, warp_to_spawn_name, boost::is_any_of(","));
        }
        else
        {
            return;
        }

        update_spawns();

        std::sort(everquest_spawns.begin(), everquest_spawns.end(), sort_spawns_by_distance_t());

        bool found_spawn = false;

        float warp_y = 0;
        float warp_x = 0;
        float warp_z = 0;

        float warp_heading = 0;

        int warp_target_address = EVERQUEST_SPAWN_INFO_NULL;

        foreach (everquest_spawn_t everquest_spawn, everquest_spawns)
        {
            foreach(std::string warp_to_spawn_name_data_value, warp_to_spawn_name_data)
            {
                if (warp_to_spawn_skip_corpses == true)
                {
                    if (everquest_spawn.type == EVERQUEST_SPAWN_INFO_TYPE_CORPSE)
                    {
                        continue;
                    }
                }

                std::size_t found = everquest_spawn.name.find(warp_to_spawn_name_data_value);

                if (found != std::string::npos)
                {
                    found_spawn = true;

                    warp_y = everquest_spawn.y;
                    warp_x = everquest_spawn.x;
                    warp_z = everquest_spawn.z + 1.0;

                    warp_heading = everquest_spawn.heading;

                    warp_target_address = everquest_spawn.address;

                    break;
                }
            }
        }

        if (found_spawn == false)
        {
            return;
        }

        everquest_function_chat_write_ex(APPLICATION_NAME " is warping to spawn.");

        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Y, warp_y);
        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_X, warp_x);
        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Z, warp_z);

        memory.write_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING, warp_heading);

        everquest_function_warp_to_safe_point(EVERQUEST_FUNCTION_WARP_TO_SAFE_POINT_VALUE);

        if (warp_to_spawn_do_target == true)
        {
            if (warp_target_address != EVERQUEST_SPAWN_INFO_NULL)
            {
                Sleep(100);

                memory.write_any<DWORD>(EVERQUEST_TARGET_SPAWN_INFO_POINTER, warp_target_address);
            }
        }

        Sleep(1000);
    }

    if (GetAsyncKeyState(VK_F7))
    {
        int player_spawn_info = everquest_get_player_spawn_info(memory);

        std::fstream file;
        file.open(ini_file.c_str(), std::ios::in);

        if (!file.is_open())
        {
            return;
        }

        file.close();

        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(ini_file, pt);

        bool warp_to_target_enabled = pt.get<bool>("WarpToTarget.bEnabled", false);

        if (warp_to_target_enabled == false)
        {
            return;
        }

        update_spawns();

        std::sort(everquest_spawns.begin(), everquest_spawns.end(), sort_spawns_by_distance_t());

        bool found_spawn = false;

        float warp_y = 0;
        float warp_x = 0;
        float warp_z = 0;

        float warp_heading = 0;

        int target_address = everquest_get_target_spawn_info(memory);

        foreach (everquest_spawn_t everquest_spawn, everquest_spawns)
        {
            if (everquest_spawn.address == target_address)
            {
                found_spawn = true;

                warp_y = everquest_spawn.y;
                warp_x = everquest_spawn.x;
                warp_z = everquest_spawn.z + 1.0;

                warp_heading = everquest_spawn.heading;

                break;
            }
        }

        if (found_spawn == false)
        {
            return;
        }

        everquest_function_chat_write_ex(APPLICATION_NAME " is warping to target.");

        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Y, warp_y);
        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_X, warp_x);
        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Z, warp_z);

        memory.write_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING, warp_heading);

        everquest_function_warp_to_safe_point(EVERQUEST_FUNCTION_WARP_TO_SAFE_POINT_VALUE);

        Sleep(1000);
    }
}

void timer_auto_gate(HWND hwnd)
{
    if (memory.get_process_id() == 0)
    {
        return;
    }

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);

    bool auto_gate_enabled = pt.get<bool>("AutoGate.bEnabled", false);

    if (auto_gate_enabled == false)
    {
        return;
    }

    int target_spawn_info = everquest_get_target_spawn_info(memory);

    if (target_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
    {
        return;
    }

    int player_spawn_info = everquest_get_player_spawn_info(memory);

    int player_hp_current = memory.read_any<DWORD>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HP_CURRENT);
    int player_hp_max     = memory.read_any<DWORD>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HP_MAX);

    float player_hp_percent = (player_hp_current * 100) / player_hp_max;

    int   auto_gate_health_points_minimum  = pt.get<int>("AutoGate.iHealthPointsMinimum");
    float auto_gate_health_percent_minimum = pt.get<float>("AutoGate.fHealthPercentMinimum");

    if (player_hp_current > auto_gate_health_points_minimum)
    {
        return;
    }

    if (player_hp_max <= auto_gate_health_points_minimum)
    {
        return;
    }

    if (player_hp_percent > auto_gate_health_percent_minimum)
    {
        return;
    }

    if (auto_gate_health_points_minimum <= 0)
    {
        return;
    }

    if (auto_gate_health_percent_minimum <= 0.0)
    {
        return;
    }

    everquest_function_chat_write_ex(APPLICATION_NAME " is auto gating.");

    memory.write_any<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_TYPE, EVERQUEST_SPAWN_INFO_TYPE_CORPSE);

    Sleep(1000);
}

void timer_follow(HWND hwnd)
{
    if (memory.get_process_id() == 0)
    {
        return;
    }

    if (warp_follow_activated == false)
    {
        return;
    }

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);

    std::string follow_name = pt.get<std::string>("WarpFollow.sName", "");

    if (follow_name.size() == 0)
    {
        return;
    }

    int player_spawn_info = everquest_get_player_spawn_info(memory);

    std::string player_name = memory.read_string(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_NAME, EVERQUEST_SPAWN_INFO_NAME_SIZE);

    if (follow_name == player_name)
    {
        return;
    }

    update_spawns();

    bool found_spawn = false;

    float warp_y = 0;
    float warp_x = 0;
    float warp_z = 0;

    float warp_heading = 0;

    foreach (everquest_spawn_t everquest_spawn, everquest_spawns)
    {
        if (everquest_spawn.name == follow_name)
        {
            found_spawn = true;

            warp_y = everquest_spawn.y;
            warp_x = everquest_spawn.x;
            warp_z = everquest_spawn.z + 1.0;

            warp_heading = everquest_spawn.heading;

            break;
        }
    }

    if (found_spawn == false)
    {
        return;
    }

    everquest_function_chat_write_ex(APPLICATION_NAME " is warp following.");

    memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Y, warp_y);
    memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_X, warp_x);
    memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Z, warp_z);

    memory.write_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING, warp_heading);

    everquest_function_warp_to_safe_point(EVERQUEST_FUNCTION_WARP_TO_SAFE_POINT_VALUE);

    Sleep(1000);
}

void on_create(HWND hwnd)
{
    memory.set_process_by_id(GetCurrentProcessId());

    SetTimer(hwnd, ID_TIMER_KEYS,      1, 0);
    SetTimer(hwnd, ID_TIMER_AUTO_GATE, 1, 0);
    SetTimer(hwnd, ID_TIMER_FOLLOW,    1, 0);

    everquest_function_chat_write_ex(APPLICATION_NAME " loaded.");
}

void on_destroy(HWND hwnd)
{
    KillTimer(hwnd, ID_TIMER_KEYS);
    KillTimer(hwnd, ID_TIMER_AUTO_GATE);
    KillTimer(hwnd, ID_TIMER_FOLLOW);

    everquest_function_chat_write_ex(APPLICATION_NAME " unloaded.");

    FreeLibraryAndExitThread((HINSTANCE)module, 0);

    ExitThread(0);

    PostQuitMessage(0);
}

void on_timer(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    switch (wparam)
    {
        case ID_TIMER_KEYS:
            timer_keys(hwnd);
            break;

        case ID_TIMER_AUTO_GATE:
            timer_auto_gate(hwnd);
            break;

        case ID_TIMER_FOLLOW:
            timer_follow(hwnd);
            break;
    }
}

LRESULT CALLBACK window_procedure(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        case WM_CREATE:
            on_create(hwnd);
            break;

        case WM_DESTROY:
            on_destroy(hwnd);
            break;

       case WM_TIMER:
            on_timer(hwnd, wparam, lparam);
            break;

        default:
            return DefWindowProc(hwnd, umsg, wparam, lparam);
    }

    return 0;
}

extern "C" DWORD WINAPI create_window()
{
    WNDCLASSEX wcx;
    wcx.cbSize        = sizeof(WNDCLASSEX);
    wcx.style         = 0;
    wcx.lpfnWndProc   = window_procedure;
    wcx.cbClsExtra    = 0;
    wcx.cbWndExtra    = 0;
    wcx.hInstance     = GetModuleHandle(NULL);
    wcx.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
    wcx.hIconSm       = LoadIcon(NULL, IDI_WINLOGO);
    wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    wcx.lpszMenuName  = NULL;
    wcx.lpszClassName = APPLICATION_CLASS_NAME;

    RegisterClassEx(&wcx);

    HWND hwnd;
    hwnd = CreateWindowEx
    (
        WS_EX_DLGMODALFRAME,
        APPLICATION_CLASS_NAME,
        APPLICATION_NAME,
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_HEIGHT, WINDOW_WIDTH,
        HWND_DESKTOP,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    //ShowWindow(hwnd, SW_SHOW);
    //UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

extern "C" BOOL APIENTRY DllMain(HANDLE dll, DWORD reason, LPVOID reserved)
{
    module = dll;

    if (reason == DLL_PROCESS_ATTACH)
    {
        //MessageBox(NULL, "fdwReason == DLL_PROCESS_ATTACH", "DllMain", MB_OK | MB_ICONINFORMATION);

        memory.enable_debug_privileges();

        window_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)create_window, dll, 0, &window_thread_id);
    }

    if (reason == DLL_PROCESS_DETACH)
    {
        //MessageBox(NULL, "fdwReason == DLL_PROCESS_DETACH", "DllMain", MB_OK | MB_ICONINFORMATION);
    }

    return TRUE;
}
