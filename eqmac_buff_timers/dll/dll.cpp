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

#define APPLICATION_NAME       "EQMac Buff Timers"
#define APPLICATION_CLASS_NAME "eqmac_buff_timers"
#define APPLICATION_DLL_NAME   "eqmac_buff_timers.dll"

#define EVERQUEST_CLIENT "eqgame.exe"
#define EVERQUEST_TITLE  "EQW beta 2.32"

#define WINDOW_HEIGHT 320
#define WINDOW_WIDTH  240

#define ID_TIMER_SET_PROCESS 10001
#define ID_TIMER_KEYS        10002

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

std::string buffs_file = "eqmac_buffs.txt";

std::map<int,std::string>buffs_list;
std::map<int,std::string>::iterator buffs_list_it;

void load_buffs()
{
    std::ifstream file(buffs_file.c_str());

    if (file.is_open())
    {
        std::string line;

        while (file.good())
        {
            std::getline(file, line);

            std::size_t found = line.find("#");
            if (found != std::string::npos)
            {
                continue;
            }

            std::vector<std::string>items;

            std::stringstream line_ss(line);

            std::string item;

            while (std::getline(line_ss, item, '^'))
            {
                if (items.size() > 1)
                {
                    int item_id;
                    std::istringstream(items.at(0)) >> item_id;

                    buffs_list.insert(std::pair<int,std::string>(item_id,items.at(1)));

                    items.clear();

                    break;
                }

                items.push_back(item);
            }
        }
    }
}

void timer_set_process(HWND hwnd)
{
    HWND foreground_hwnd = GetForegroundWindow();

    DWORD foreground_process_id;
    GetWindowThreadProcessId(foreground_hwnd, &foreground_process_id);

    if (foreground_process_id != GetCurrentProcessId())
    {
        return;
    }

    memory.set_process_by_id(GetCurrentProcessId());
}

void timer_keys(HWND hwnd)
{
    if (memory.get_process_id() == 0)
    {
        return;
    }

    if
    (
        (GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
        (GetAsyncKeyState(VK_ALT) & 0x8000) &&
        (GetAsyncKeyState(VK_BACK) & 0x8000)
    )
    {
        DestroyWindow(hwnd);
    }

    if
    (
        (GetAsyncKeyState(VK_ALT) & 0x8000) &&
        (GetAsyncKeyState(VK_B) & 0x8000)
    )
    {
        everquest_function_chat_write(APPLICATION_NAME ":", 10, 1);

        int char_spawn_info = everquest_get_char_info(memory);

        int buffs_structure_address = char_spawn_info + EVERQUEST_OFFSET_CHAR_INFO_BUFFS_STRUCTURE;

        bool buffs_found = false;

        int i = 0;
        for (i = 0; i < EVERQUEST_BUFFS_MAX; i++)
        {
            int buff_id = memory.read_any<WORD>(buffs_structure_address + EVERQUEST_OFFSET_BUFFS_STRUCTURE_SPELL_ID);

            if (buff_id == EVERQUEST_BUFF_SPELL_ID_NULL)
            {
                buffs_structure_address += EVERQUEST_BUFFS_STRUCTURE_BUFF_SIZE;

                continue;
            }
            else
            {
                buffs_found = true;
            }

            int buff_ticks = memory.read_any<DWORD>(buffs_structure_address + EVERQUEST_OFFSET_BUFFS_STRUCTURE_DURATION);

            if (buff_ticks == 0)
            {
                buffs_structure_address += EVERQUEST_BUFFS_STRUCTURE_BUFF_SIZE;

                continue;
            }

            int buff_total_seconds = buff_ticks * 6;

            int buff_hours = buff_total_seconds / 3600;

            buff_total_seconds = buff_total_seconds % 3600;

            int buff_minutes = buff_total_seconds / 60;

            int buff_seconds = buff_total_seconds % 60;

            std::stringstream buff_duration;

            //buff_duration << "Time: ";

            if (buff_hours > 0)
            {
                buff_duration << buff_hours << "h";
            }

            if (buff_minutes > 0)
            {
                if (buff_hours > 0)
                {
                    buff_duration << " ";
                }

                buff_duration << buff_minutes << "m";
            }

            if (buff_hours == 0 && buff_minutes == 0)
            {
                if (buff_seconds > 0)
                {
                    buff_duration << buff_seconds << "s";
                }
            }

            std::string buff_name = "*Unknown Buff";

            if (buffs_list.size() > 0)
            {
                for (buffs_list_it = buffs_list.begin(); buffs_list_it != buffs_list.end(); ++buffs_list_it)
                {
                    if (buff_id == buffs_list_it->first)
                    {
                        buff_name = buffs_list_it->second;

                        break;
                    }
                }
            }

            std::stringstream buff_text;
            buff_text << i + 1 << ":" << " " << buff_name << " " << "(" << buff_duration.str() << ")";

            everquest_function_chat_write((char*)buff_text.str().c_str(), 10, 1);

            buffs_structure_address += EVERQUEST_BUFFS_STRUCTURE_BUFF_SIZE;
        }

        if (buffs_found == false)
        {
            everquest_function_chat_write("You do not have any buffs.", 10, 1);
        }

        Sleep(1000);
    }
}

void on_create(HWND hwnd)
{
    SetTimer(hwnd, ID_TIMER_SET_PROCESS, 1, 0);
    SetTimer(hwnd, ID_TIMER_KEYS,        1, 0);

    load_buffs();

    everquest_function_chat_write(APPLICATION_NAME " loaded.", 10, 1);
}

void on_destroy(HWND hwnd)
{
    KillTimer(hwnd, ID_TIMER_SET_PROCESS);
    KillTimer(hwnd, ID_TIMER_KEYS);

    everquest_function_chat_write(APPLICATION_NAME " unloaded.", 10, 1);

    FreeLibraryAndExitThread((HINSTANCE)module, 0);

    CloseHandle(window_thread);

    PostQuitMessage(0);
}

void on_timer(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    switch (wparam)
    {
        case ID_TIMER_SET_PROCESS:
            timer_set_process(hwnd);
            break;

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
