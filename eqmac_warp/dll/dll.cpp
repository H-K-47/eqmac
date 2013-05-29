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

#define ID_TIMER_KEYS 10001

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

#define VK_G 0x47

#define VK_ALT VK_MENU

typedef void __stdcall _everquest_function_warp_to_safe_point(int value);
static _everquest_function_warp_to_safe_point *everquest_function_warp_to_safe_point = (_everquest_function_warp_to_safe_point *)EVERQUEST_FUNCTION_WARP_TO_SAFE_POINT;

memory memory;

HANDLE module;

HANDLE window_thread;

DWORD window_thread_id;

struct map_spawn_t
{
    int address;

    int count;

    int spawn_id;
    int owner_id;

    std::string name;
    std::string last_name;

    float x, y, z;

    float distance;
    float distance_z;

    float heading;

    float movement_speed;

    int standing_state;

    int type;
    int level;
    int race;
    int _class; // class is a keyword

    int hp;
    int hp_max;

    bool is_target;
    bool is_player_corpse;

    int is_holding_both;
    int is_holding_secondary;
    int is_holding_primary;
};

std::vector<map_spawn_t> map_spawns;
std::vector<map_spawn_t>::iterator map_spawns_it;

struct sort_spawns_by_distance_t
{
    bool operator() (const map_spawn_t& map_spawn1, const map_spawn_t& map_spawn2)
    {
        return map_spawn1.distance > map_spawn2.distance;
    }
};

std::string warp_to_spawn_name = "";
std::vector<std::string> warp_to_spawn_name_data;

float calculate_distance(float x1, float y1, float x2, float y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void update_spawns()
{
    if (memory.get_process_id() == 0)
    {
        return;
    }

    map_spawns.clear();

    int player_spawn_info = everquest_get_player_spawn_info(memory);
    int target_spawn_info = everquest_get_target_spawn_info(memory);

    float player_y = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
    float player_x = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
    float player_z = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

    int spawn_info_address = player_spawn_info;

    int spawn_next_spawn_info = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER, 4);

    spawn_info_address = spawn_next_spawn_info;

    for (int i = 0; i < EVERQUEST_SPAWNS_MAX; i++)
    {
        spawn_next_spawn_info = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER, 4);

        if (spawn_next_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            break;
        }

        int spawn_actor_info = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER, 4);

        map_spawn_t map_spawn;

        map_spawn.address = spawn_info_address;

        map_spawn.spawn_id = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_SPAWN_ID, 2);
        map_spawn.owner_id = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_OWNER_ID, 2);

        map_spawn.name      = memory.read_string(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NAME,      0x40);
        map_spawn.last_name = memory.read_string(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_LAST_NAME, 0x20);

        map_spawn.y = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Y);
        map_spawn.x = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_X);
        map_spawn.z = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Z);

        map_spawn.distance = calculate_distance(player_x, player_y, map_spawn.x, map_spawn.y);

        map_spawn.distance_z = map_spawn.z - player_z;

        map_spawn.heading = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HEADING);

        map_spawn.movement_speed = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_MOVEMENT_SPEED);

        map_spawn.standing_state = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE);

        map_spawn.type   = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_TYPE);
        map_spawn.level  = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_LEVEL);
        map_spawn.race   = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_RACE);
        map_spawn._class = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_CLASS);

        map_spawn.hp     = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HP_CURRENT, 4);
        map_spawn.hp_max = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HP_MAX,     4);

        map_spawn.is_target = (spawn_info_address == target_spawn_info ? true : false);

        map_spawn.is_player_corpse = false;

        if (map_spawn.type == EVERQUEST_SPAWN_INFO_TYPE_CORPSE)
        {
            std::string player_name = memory.read_string(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_NAME, 0x40);

            std::size_t found = map_spawn.name.find(player_name);

            if (found != std::string::npos)
            {
                map_spawn.is_player_corpse = true;
            }
        }

        map_spawn.is_holding_both      = memory.read_bytes(spawn_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_BOTH,      4);
        map_spawn.is_holding_secondary = memory.read_bytes(spawn_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_SECONDARY, 4);
        map_spawn.is_holding_primary   = memory.read_bytes(spawn_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_PRIMARY,   4);

        map_spawns.push_back(map_spawn);

        spawn_info_address = spawn_next_spawn_info;
    }
}

void timer_keys(HWND hwnd)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);

    bool auto_gate_enabled = pt.get<bool>("AutoGate.bEnabled", false);

    if (auto_gate_enabled == true)
    {
        int player_spawn_info = everquest_get_player_spawn_info(memory);

        int player_hp_current = memory.read_bytes(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HP_CURRENT, 4);
        int player_hp_max     = memory.read_bytes(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HP_MAX,     4);

        float player_hp_percent = (player_hp_current * 100) / player_hp_max;

        int   auto_gate_health_points_minimum  = pt.get<float>("AutoGate.iHealthPointsMinimum");
        float auto_gate_health_percent_minimum = pt.get<float>("AutoGate.fHealthPercentMinimum");

        bool auto_gate_ready = false;

        if (player_hp_current < auto_gate_health_points_minimum)
        {
            if (player_hp_max > auto_gate_health_points_minimum)
            {
                auto_gate_ready = true;
            }
        }

        if (player_hp_percent < auto_gate_health_percent_minimum)
        {
            auto_gate_ready = true;
        }

        if (auto_gate_health_points_minimum <= 0)
        {
            auto_gate_ready = false;
        }

        if (auto_gate_health_percent_minimum <= 0.0)
        {
            auto_gate_ready = false;
        }

        int target_spawn_info = everquest_get_target_spawn_info(memory);

        if (target_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            auto_gate_ready = false;
        }

        if (auto_gate_ready == true)
        {
            memory.write_any<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_TYPE, EVERQUEST_SPAWN_INFO_TYPE_CORPSE);
        }
    }

    HWND foreground_hwnd = GetForegroundWindow();

    DWORD foreground_process_id;
    GetWindowThreadProcessId(foreground_hwnd, &foreground_process_id);

    if (foreground_process_id != GetCurrentProcessId())
    {
        return;
    }

    if (GetAsyncKeyState(VK_F12))
    {
        //MessageBox(NULL, "VK_F12", "timer_keys", MB_OK | MB_ICONINFORMATION);

        memory.set_process_by_id(foreground_process_id);

        if (memory.get_process_id() == 0)
        {
            return;
        }

        bool warp_point_ex = false;

        if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
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

        float warp_y, warp_x, warp_z, warp_heading;

        if (warp_point_ex == true)
        {
            warp_y = pt.get<float>("WarpPointEx.Y");
            warp_x = pt.get<float>("WarpPointEx.X");
            warp_z = pt.get<float>("WarpPointEx.Z");

            warp_heading = pt.get<float>("WarpPointEx.Heading");
        }
        else
        {
            warp_y = pt.get<float>("WarpPoint.Y");
            warp_x = pt.get<float>("WarpPoint.X");
            warp_z = pt.get<float>("WarpPoint.Z");

            warp_heading = pt.get<float>("WarpPoint.Heading");
        }

        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Y, warp_y);
        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_X, warp_x);
        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Z, warp_z);

        memory.write_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING, warp_heading);

        everquest_function_warp_to_safe_point(EVERQUEST_FUNCTION_WARP_TO_SAFE_POINT_VALUE);

        Sleep(1000);
    }

    if (GetAsyncKeyState(VK_F8))
    {
        //MessageBox(NULL, "VK_F8", "timer_keys", MB_OK | MB_ICONINFORMATION);

        memory.set_process_by_id(foreground_process_id);

        if (memory.get_process_id() == 0)
        {
            return;
        }

        bool warp_to_spawn_ex = false;

        if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
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

        std::sort(map_spawns.begin(), map_spawns.end(), sort_spawns_by_distance_t());

        bool found_spawn = false;

        float warp_y = 0;
        float warp_x = 0;
        float warp_z = 0;

        float warp_heading = 0;

        int warp_target_address = EVERQUEST_SPAWN_INFO_NULL;

        foreach (map_spawn_t map_spawn, map_spawns)
        {

            foreach(std::string warp_to_spawn_name_data_value, warp_to_spawn_name_data)
            {
                if (warp_to_spawn_skip_corpses == true)
                {
                    if (map_spawn.type == EVERQUEST_SPAWN_INFO_TYPE_CORPSE)
                    {
                        continue;
                    }
                }

                std::size_t found = map_spawn.name.find(warp_to_spawn_name_data_value);

                if (found != std::string::npos)
                {
                    found_spawn = true;

                    warp_y = map_spawn.y;
                    warp_x = map_spawn.x;
                    warp_z = map_spawn.z + 1.0;

                    warp_heading = map_spawn.heading;

                    warp_target_address = map_spawn.address;

                    break;
                }
            }
        }

        if (found_spawn == false)
        {
            return;
        }

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

                memory.write_bytes(EVERQUEST_TARGET_SPAWN_INFO_POINTER, warp_target_address, 4);
            }
        }

        Sleep(1000);
    }

    if (GetAsyncKeyState(VK_F7))
    {
        //MessageBox(NULL, "VK_F7", "timer_keys", MB_OK | MB_ICONINFORMATION);

        memory.set_process_by_id(foreground_process_id);

        if (memory.get_process_id() == 0)
        {
            return;
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

        bool warp_to_target_enabled = pt.get<bool>("WarpToTarget.bEnabled", false);

        if (warp_to_target_enabled == false)
        {
            return;
        }

        update_spawns();

        std::sort(map_spawns.begin(), map_spawns.end(), sort_spawns_by_distance_t());

        bool found_spawn = false;

        float warp_y = 0;
        float warp_x = 0;
        float warp_z = 0;

        float warp_heading = 0;

        int target_address = everquest_get_target_spawn_info(memory);

        foreach (map_spawn_t map_spawn, map_spawns)
        {
            if (map_spawn.address == target_address)
            {
                found_spawn = true;

                warp_y = map_spawn.y;
                warp_x = map_spawn.x;
                warp_z = map_spawn.z + 1.0;

                warp_heading = map_spawn.heading;

                break;
            }
        }

        if (found_spawn == false)
        {
            return;
        }

        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Y, warp_y);
        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_X, warp_x);
        memory.write_any<float>(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Z, warp_z);

        memory.write_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING, warp_heading);

        everquest_function_warp_to_safe_point(EVERQUEST_FUNCTION_WARP_TO_SAFE_POINT_VALUE);

        Sleep(1000);
    }

    if (GetAsyncKeyState(VK_G))
    {
        memory.set_process_by_id(foreground_process_id);

        if (memory.get_process_id() == 0)
        {
            return;
        }

        if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
        {
            if (GetAsyncKeyState(VK_ALT) & 0x8000)
            {
                int player_spawn_info = everquest_get_player_spawn_info(memory);

                memory.write_any<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_TYPE, EVERQUEST_SPAWN_INFO_TYPE_CORPSE);
            }
        }
    }
}

void on_create(HWND hwnd)
{
    SetTimer(hwnd, ID_TIMER_KEYS, 100, 0);
}

void on_destroy(HWND hwnd)
{
    KillTimer(hwnd, ID_TIMER_KEYS);

    DWORD exit_code;
    GetExitCodeThread(window_thread, &exit_code);

    //FreeLibraryAndExitThread((HINSTANCE)module, exit_code);

    CloseHandle(window_thread);

    PostQuitMessage(0);
}

void on_timer(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    switch (wparam)
    {
        case ID_TIMER_KEYS:
            timer_keys(hwnd);
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

        window_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)create_window, 0, 0, &window_thread_id);
    }

    if (reason == DLL_PROCESS_DETACH)
    {
        //MessageBox(NULL, "fdwReason == DLL_PROCESS_DETACH", "DllMain", MB_OK | MB_ICONINFORMATION);
    }

    return TRUE;
}
