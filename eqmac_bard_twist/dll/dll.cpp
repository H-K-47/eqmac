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

const std::string ini_file = "eqmac_bard_twist.ini";

#define APPLICATION_NAME       "EQMac Bard Twist"
#define APPLICATION_CLASS_NAME "eqmac_bard_twist"
#define APPLICATION_DLL_NAME   "eqmac_bard_twist.dll"

#define EVERQUEST_CLIENT "eqgame.exe"
#define EVERQUEST_TITLE  "EQW beta 2.32"

#define BARD_TWIST_MAX_SONGS 3

#define WINDOW_HEIGHT 320
#define WINDOW_WIDTH  240

#define ID_TIMER_KEYS       10001
#define ID_TIMER_BARD_TWIST 10002

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

#define VK_B 0x42

#define VK_ALT VK_MENU

memory memory;

HANDLE module;

HANDLE window_thread;

DWORD window_thread_id;

bool bard_twist_activated = false;

bool bard_twist_duck_to_deactivate = true;

void toggle_bool(bool &b)
{
    b = !b;
}

bool check_duck_to_deactivate()
{
    int player_spawn_info = everquest_get_player_spawn_info(memory);

    BYTE player_standing_state = memory.read_any<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE);

    if (player_standing_state == EVERQUEST_STANDING_STATE_DUCKING)
    {
        bard_twist_activated = false;

        return true;
    }

    return false;
}

bool check_if_sitting()
{
    int player_spawn_info = everquest_get_player_spawn_info(memory);

    BYTE player_standing_state = memory.read_any<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE);

    if (player_standing_state == EVERQUEST_STANDING_STATE_SITTING)
    {
        return true;
    }

    return false;
}

int fix_hot_button_number(int hot_button)
{
    if (hot_button > 0 && hot_button < 10)
    {
        return hot_button - 1;
    }
    else
    {
        return 9;
    }
}

void sleep_cast_time(float time)
{
    Sleep((DWORD)(time * 1000));
}

bool twist_hot_button(int hot_button)
{
    if (bard_twist_duck_to_deactivate == true)
    {
        if (check_duck_to_deactivate() == true)
        {
            return true;
        }
    }

    WORD player_casting_spell_id = everquest_get_player_casting_spell_id(memory);

    BYTE player_casting_spell_gem_number = everquest_get_player_casting_spell_gem_number(memory);

    if (player_casting_spell_id != EVERQUEST_CASTING_SPELL_ID_NULL && player_casting_spell_gem_number != EVERQUEST_CASTING_SPELL_GEM_NUMBER_SINGING)
    {
        return false;
    }

    int num_times_to_press_hotkey = 1;

    if (player_casting_spell_gem_number == EVERQUEST_CASTING_SPELL_GEM_NUMBER_SINGING)
    {
        num_times_to_press_hotkey = 2;
    }

    for (int i = 0; i < num_times_to_press_hotkey; i++)
    {
        everquest_function_do_hot_button(hot_button);
        Sleep(10);
    }

    Sleep(250);

    player_casting_spell_id = everquest_get_player_casting_spell_id(memory);

    if (player_casting_spell_id == EVERQUEST_CASTING_SPELL_ID_NULL)
    {
        return false;
    }

    return true;
}

void timer_keys(HWND hwnd)
{
    memory.set_process_by_id(GetCurrentProcessId());

    if (memory.get_process_id() == 0)
    {
        //MessageBox(hwnd, "Error: Process ID not found!", APPLICATION_NAME, MB_OK | MB_ICONERROR);
        //DestroyWindow(hwnd);

        return;
    }

    int player_spawn_info = everquest_get_player_spawn_info(memory);

    BYTE player_class = memory.read_any<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_CLASS);

    if (player_class != EVERQUEST_CLASS_BARD)
    {
        //MessageBox(hwnd, "Error: You are not a Bard!", APPLICATION_NAME, MB_OK | MB_ICONINFORMATION);
        //DestroyWindow(hwnd);

        return;
    }

    if (bard_twist_duck_to_deactivate == true)
    {
        check_duck_to_deactivate();
    }

    if
    (
        (GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
        (GetAsyncKeyState(VK_ALT) & 0x8000) &&
        (GetAsyncKeyState(VK_B) & 0x8000)
    )
    {
        toggle_bool(bard_twist_activated);

        Sleep(1000);
    }

    if
    (
        (GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
        (GetAsyncKeyState(VK_ALT) & 0x8000) &&
        (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    )
    {
        DestroyWindow(hwnd);
    }
}

void timer_bard_twist(HWND hwnd)
{
    if (bard_twist_activated == false)
    {
        return;
    }

    if (check_if_sitting() == true)
    {
        return;
    }

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);

    bool bard_twist_enabled = pt.get<bool>("BardTwist.bEnabled", false);

    if (bard_twist_enabled == false)
    {
        return;
    }

    bard_twist_duck_to_deactivate = pt.get<bool>("BardTwist.bDuckToDeactivate", true);

    if (bard_twist_duck_to_deactivate == true)
    {
        if (check_duck_to_deactivate() == true)
        {
            return;
        }
    }

    int bard_twist_number_of_songs = pt.get<int>("BardTwist.iNumberOfSongs");

    if (bard_twist_number_of_songs <= 0 || bard_twist_number_of_songs > BARD_TWIST_MAX_SONGS)
    {
        return;
    }

    int bard_twist_song_1_hot_button   = pt.get<int>("BardTwist.iSong1HotButton");
    float bard_twist_song_1_cast_time  = pt.get<float>("BardTwist.fSong1CastTime");

    int bard_twist_song_2_hot_button   = pt.get<int>("BardTwist.iSong2HotButton");
    float bard_twist_song_2_cast_time  = pt.get<float>("BardTwist.fSong2CastTime");

    int bard_twist_song_3_hot_button   = pt.get<int>("BardTwist.iSong3HotButton");
    float bard_twist_song_3_cast_time  = pt.get<float>("BardTwist.fSong3CastTime");

    bard_twist_song_1_hot_button = fix_hot_button_number(bard_twist_song_1_hot_button);
    bard_twist_song_2_hot_button = fix_hot_button_number(bard_twist_song_2_hot_button);
    bard_twist_song_3_hot_button = fix_hot_button_number(bard_twist_song_3_hot_button);

    bool result = false;

    while (result == false)
    {
        if (check_if_sitting() == true)
        {
            break;
        }

        result = twist_hot_button(bard_twist_song_1_hot_button);

        Sleep(1);
    }

    if (bard_twist_activated == false)
    {
        return;
    }

    sleep_cast_time(bard_twist_song_1_cast_time);

    if (bard_twist_number_of_songs > 1)
    {
        result = false;

        while (result == false)
        {
            if (check_if_sitting() == true)
            {
                break;
            }

            result = twist_hot_button(bard_twist_song_2_hot_button);

            Sleep(1);
        }

        if (bard_twist_activated == false)
        {
            return;
        }

        sleep_cast_time(bard_twist_song_2_cast_time);
    }

    if (bard_twist_number_of_songs > 2)
    {
        result = false;

        while (result == false)
        {
            if (check_if_sitting() == true)
            {
                break;
            }

            result = twist_hot_button(bard_twist_song_3_hot_button);

            Sleep(1);
        }

        if (bard_twist_activated == false)
        {
            return;
        }

        sleep_cast_time(bard_twist_song_3_cast_time);
    }

    Sleep(1);
}

void on_create(HWND hwnd)
{
    SetTimer(hwnd, ID_TIMER_KEYS,       1, 0);
    SetTimer(hwnd, ID_TIMER_BARD_TWIST, 1, 0);
}

void on_destroy(HWND hwnd)
{
    KillTimer(hwnd, ID_TIMER_KEYS);
    KillTimer(hwnd, ID_TIMER_BARD_TWIST);

    DWORD exit_code;
    GetExitCodeThread(window_thread, &exit_code);

    FreeLibraryAndExitThread((HINSTANCE)module, exit_code);

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

        case ID_TIMER_BARD_TWIST:
            timer_bard_twist(hwnd);
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
