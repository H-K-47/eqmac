#define WINVER 0x0500

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>

#include <windows.h>
#include <psapi.h>

#include <winbase.h>

#include "memory.hpp"

#include "eqmac.hpp"

const std::string ini_file = "eqmac_keys.ini";

#define APPLICATION_NAME       "EQMac Keys"
#define APPLICATION_CLASS_NAME "eqmac_keys"
#define APPLICATION_DLL_NAME   "eqmac_keys.dll"

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

typedef void __stdcall _everquest_function_do_hot_button(int button_index);
static _everquest_function_do_hot_button *everquest_function_do_hot_button = (_everquest_function_do_hot_button *)EVERQUEST_FUNCTION_DO_HOT_BUTTON;

memory memory;

HANDLE module;

HANDLE window_thread;

DWORD window_thread_id;

void timer_keys(HWND hwnd)
{
    HWND foreground_hwnd = GetForegroundWindow();

    DWORD foreground_process_id;
    GetWindowThreadProcessId(foreground_hwnd, &foreground_process_id);

    if (foreground_process_id == GetCurrentProcessId())
    {
        return;
    }

    if (GetAsyncKeyState(VK_CONTROL))
    {
        if (GetAsyncKeyState(VK_1))
        {
            everquest_function_do_hot_button(0);

            Sleep(1000);
        }

        if (GetAsyncKeyState(VK_2))
        {
            everquest_function_do_hot_button(1);

            Sleep(1000);
        }

        if (GetAsyncKeyState(VK_3))
        {
            everquest_function_do_hot_button(2);

            Sleep(1000);
        }

        if (GetAsyncKeyState(VK_4))
        {
            everquest_function_do_hot_button(3);

            Sleep(1000);
        }

        if (GetAsyncKeyState(VK_5))
        {
            everquest_function_do_hot_button(4);

            Sleep(1000);
        }

        if (GetAsyncKeyState(VK_6))
        {
            everquest_function_do_hot_button(5);

            Sleep(1000);
        }

        if (GetAsyncKeyState(VK_7))
        {
            everquest_function_do_hot_button(6);

            Sleep(1000);
        }

        if (GetAsyncKeyState(VK_8))
        {
            everquest_function_do_hot_button(7);

            Sleep(1000);
        }

        if (GetAsyncKeyState(VK_9))
        {
            everquest_function_do_hot_button(8);

            Sleep(1000);
        }

        if (GetAsyncKeyState(VK_0))
        {
            everquest_function_do_hot_button(9);

            Sleep(1000);
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
