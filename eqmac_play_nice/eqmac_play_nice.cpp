#define WINVER 0x0500

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <windows.h>
#include <psapi.h>
#include <shellapi.h>

#include <winbase.h>

#include "resource.h"

#include "memory.hpp"

#include "eqmac.hpp"

const std::string ini_file = "eqmac_play_nice.ini";

int sleep_time_foreground = 10;
int sleep_time_background = 100;

#define APPLICATION_NAME       "EQMac Play Nice"
#define APPLICATION_CLASS_NAME "eqmac_play_nice"

#define EVERQUEST_CLIENT "eqgame.exe"
#define EVERQUEST_TITLE  "EQW beta 2.32"

#define WINDOW_HEIGHT 320
#define WINDOW_WIDTH  240

#define WM_SHELLNOTIFY WM_USER+5

#define ID_TRAY_ICON 101

#define ID_TIMER_PLAY_NICE 1001

NOTIFYICONDATA tray_icon;

memory memory;

DWORD get_active_window_process_id()
{
    HWND active_window = GetForegroundWindow();

    DWORD process_id;
    GetWindowThreadProcessId(active_window, &process_id);

    return process_id;
}

void play_nice_read_ini()
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);

    sleep_time_foreground = pt.get<int>("Options.SleepTimeForeground");
    sleep_time_background = pt.get<int>("Options.SleepTimeBackground");
}

void play_nice_enable()
{
    DWORD processes[1024];
    DWORD needed;
    if (!EnumProcesses(processes, sizeof(processes), &needed))
    {
        return;
    }

    DWORD num_processes = needed / sizeof(DWORD);

    unsigned int i = 0;
    for (i = 0; i < num_processes; i++)
    {
        if (processes[i] == 0)
        {
            continue;
        }

        HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processes[i]);

        if (process_handle == NULL)
        {
            continue;
        }

        HMODULE hmod;
        DWORD needed_ex;
        if (EnumProcessModules(process_handle, &hmod, sizeof(hmod), &needed_ex))
        {
            char process_name[MAX_PATH] = {0};

            GetModuleBaseName(process_handle, hmod, process_name, sizeof(process_name));

            if (strcmp(process_name, "eqgame.exe") == 0)
            {
                DWORD process_id_active = get_active_window_process_id();

                memory.set_process_by_id(processes[i]);

                memory.write_any<BYTE>(EVERQUEST_ASM_PLAY_NICE_SLEEP_IS_ENABLED, 1);

                if (processes[i] == process_id_active)
                {
                    memory.write_any<DWORD>(EVERQUEST_ASM_PLAY_NICE_SLEEP_TIME, sleep_time_foreground);
                }
                else
                {
                    memory.write_any<DWORD>(EVERQUEST_ASM_PLAY_NICE_SLEEP_TIME, sleep_time_background);
                }
            }
        }

        CloseHandle(process_handle);
    }
}

void play_nice_disable()
{
    DWORD processes[1024];
    DWORD needed;
    if (!EnumProcesses(processes, sizeof(processes), &needed))
    {
        return;
    }

    DWORD num_processes = needed / sizeof(DWORD);

    unsigned int i = 0;
    for (i = 0; i < num_processes; i++)
    {
        if (processes[i] == 0)
        {
            continue;
        }

        HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processes[i]);

        if (process_handle == NULL)
        {
            continue;
        }

        HMODULE hmod;
        DWORD needed_ex;
        if (EnumProcessModules(process_handle, &hmod, sizeof(hmod), &needed_ex))
        {
            char process_name[MAX_PATH] = {0};

            GetModuleBaseName(process_handle, hmod, process_name, sizeof(process_name));

            if (strcmp(process_name, "eqgame.exe") == 0)
            {
                memory.set_process_by_id(processes[i]);

                memory.write_any<DWORD>(EVERQUEST_ASM_PLAY_NICE_SLEEP_TIME, 0);
                memory.write_any<BYTE>(EVERQUEST_ASM_PLAY_NICE_SLEEP_IS_ENABLED, 0);
            }
        }

        CloseHandle(process_handle);
    }
}

void tray_icon_create(HWND hwnd)
{
    tray_icon.cbSize           = sizeof(NOTIFYICONDATA);
    tray_icon.hWnd             = hwnd;
    tray_icon.uID              = ID_TRAY_ICON;
    tray_icon.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    tray_icon.uCallbackMessage = WM_SHELLNOTIFY;
    //tray_icon.hIcon          = LoadIcon(NULL, IDI_WINLOGO);
    tray_icon.hIcon            = (HICON)LoadImage
    (
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(ID_APPLICATION_ICON),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON),
        LR_SHARED
    );

    strcpy(tray_icon.szTip, APPLICATION_NAME);

    Shell_NotifyIcon(NIM_ADD, &tray_icon);
}

void on_create(HWND hwnd)
{
    memory.enable_debug_privileges();

    tray_icon_create(hwnd);

    play_nice_read_ini();

    SetTimer(hwnd, ID_TIMER_PLAY_NICE, 1, (TIMERPROC)NULL);
}

void on_destroy(HWND hwnd)
{
    Shell_NotifyIcon(NIM_DELETE, &tray_icon);

    KillTimer(hwnd, ID_TIMER_PLAY_NICE);

    play_nice_disable();

    PostQuitMessage(0);
}

void on_tray(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    if (wparam == ID_TRAY_ICON)
    {
        if (lparam == WM_RBUTTONUP)
        {
            DestroyWindow(hwnd);
        }
    }
}

void on_timer(HWND hwnd, WPARAM wparam)
{
    switch (wparam) 
    { 
        case ID_TIMER_PLAY_NICE:
            play_nice_enable();
            break;
    }
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
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

        case WM_SHELLNOTIFY:
            on_tray(hwnd, wparam, lparam);
            break;

        case WM_TIMER:
            on_timer(hwnd, wparam);
            break;

        default:
            return DefWindowProc(hwnd, umsg, wparam, lparam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR cmdline, int cmdshow)
{
    HWND application_window = FindWindow(NULL, APPLICATION_NAME);
    if (application_window != NULL)
    {
        return 0;
    }

    WNDCLASSEX wcx;
    wcx.cbSize        = sizeof(WNDCLASSEX);
    wcx.style         = 0;
    wcx.lpfnWndProc   = window_proc;
    wcx.cbClsExtra    = 0;
    wcx.cbWndExtra    = 0;
    wcx.hInstance     = hinstance;
    //wcx.hIcon       = LoadIcon(NULL, IDI_WINLOGO);
    //wcx.hIconSm     = LoadIcon(NULL, IDI_WINLOGO);
    wcx.hIcon         = (HICON)LoadImage
    (
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(ID_APPLICATION_ICON),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXICON),
        GetSystemMetrics(SM_CYICON),
        LR_SHARED
    );
    wcx.hIconSm       = (HICON)LoadImage
    (
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(ID_APPLICATION_ICON),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON),
        LR_SHARED
    );
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
        hinstance,
        NULL
    );

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
