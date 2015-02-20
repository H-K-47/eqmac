#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <psapi.h>

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

HANDLE module;

HANDLE window_thread;

void timer_keys(HWND hwnd)
{
    if (GetAsyncKeyState(VK_5))
    {
        //MessageBox(NULL, "VK_5", "timer_keys", MB_OK | MB_ICONINFORMATION);

        HWND h = GetTopWindow(0);
        while (h)
        {
            DWORD process_id;
            DWORD thread_id = GetWindowThreadProcessId(h, &process_id);

            if (process_id == GetCurrentProcessId())
            {
                h = GetNextWindow(h , GW_HWNDNEXT);

                continue;
            }

            HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);

            if (process_handle != NULL)
            {
                HMODULE module;
                DWORD needed_ex;
                if (EnumProcessModules(process_handle, &module, sizeof(module), &needed_ex))
                {
                    char process_name[MAX_PATH] = {0};

                    GetModuleBaseName(process_handle, module, process_name, sizeof(process_name));

                    if (strcmp(process_name, EVERQUEST_CLIENT) == 0)
                    {
                        PostMessage(h, WM_KEYDOWN, VK_5, 0);
                        PostMessage(h, WM_KEYUP,   VK_5, 0);
                    }
                }
            }

            h = GetNextWindow(h , GW_HWNDNEXT);
        }

        //HWND everquest_window = FindWindow(NULL, EVERQUEST_TITLE);
        //PostMessage(everquest_window, WM_KEYDOWN, VK_5, 0);
        //PostMessage(everquest_window, WM_KEYUP,   VK_5, 0);
    }
}

void on_create(HWND hwnd)
{
    SetTimer(hwnd, ID_TIMER_KEYS, 100, 0);
}

void on_destroy(HWND hwnd)
{
    KillTimer(hwnd, ID_TIMER_KEYS);

    //DWORD exit_code;
    //GetExitCodeThread(window_thread, &exit_code);

    //FreeLibraryAndExitThread(module, exit_code);

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

DWORD WINAPI create_window()
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

BOOL APIENTRY DllMain(HANDLE h_module, DWORD fdw_reason, LPVOID lpv_reserved)
{
    module = h_module;

    if (fdw_reason == DLL_PROCESS_ATTACH)
    {
        //MessageBox(NULL, "fdwReason == DLL_PROCESS_ATTACH", "DllMain", MB_OK | MB_ICONINFORMATION);

        window_thread = CreateThread(NULL, 0, create_window, 0, 0, NULL);
    }

    if (fdw_reason == DLL_PROCESS_DETACH)
    {
        //MessageBox(NULL, "fdwReason == DLL_PROCESS_DETACH", "DllMain", MB_OK | MB_ICONINFORMATION);
    }

    return TRUE;
}
