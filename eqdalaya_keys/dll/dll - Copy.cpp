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

#include "eqdalaya.hpp"

#define APPLICATION_NAME       "EQDalaya Keys"
#define APPLICATION_CLASS_NAME "eqdalaya_keys"
#define APPLICATION_DLL_NAME   "eqdalaya_keys.dll"

#define EVERQUEST_CLIENT "eqgame.exe"
#define EVERQUEST_TITLE  "EverQuest"

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

#define VK_ALT VK_MENU

memory memory;

HMODULE module_global;

void timer_keys(HWND hwnd)
{
    if (memory.get_process_id() == 0)
    {
        return;
    }

    if (memory.is_foreground_window_current_process_id() == true)
    {
        if
        (
            (GetAsyncKeyState(VK_CONTROL)) &&
            (GetAsyncKeyState(VK_ALT)) &&
            (GetAsyncKeyState(VK_BACK))
        )
        {
            DestroyWindow(hwnd);
        }

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
    memory.set_process_by_id(GetCurrentProcessId());

    SetTimer(hwnd, ID_TIMER_KEYS, 1, 0);

    everquest_function_chat_write_ex(APPLICATION_NAME " loaded.");
}

void on_destroy(HWND hwnd, HMODULE module)
{
    KillTimer(hwnd, ID_TIMER_KEYS);

    everquest_function_chat_write_ex(APPLICATION_NAME " unloaded.");

    FreeLibraryAndExitThread(module, 0);
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
            on_destroy(hwnd, module_global);
            break;

       case WM_TIMER:
            on_timer(hwnd, wparam, lparam);
            break;

        default:
            return DefWindowProc(hwnd, umsg, wparam, lparam);
    }

    return 0;
}

extern "C" DWORD WINAPI create_window(HMODULE module)
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

extern "C" BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    module_global = module;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            memory.enable_debug_privileges();
            DisableThreadLibraryCalls(module);
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)create_window, module, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
