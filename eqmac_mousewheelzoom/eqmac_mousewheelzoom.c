#include <windows.h>
#include <psapi.h>
#include <shellapi.h>

#include "resource.h"

#define APP_NAME      "EQMac Mouse Wheel Zoom"
#define APP_CLASSNAME "eqmac_mouse_wheel_zoom"

#define WINDOW_HEIGHT 640
#define WINDOW_WIDTH  480

#define WM_SHELLNOTIFY WM_USER+5

#define ID_TRAYICON 101

#define ID_TIMER_UPDATE 1001

HWND window_hwnd;

NOTIFYICONDATA tray_icon;

#define EQ_CLIENT "eqgame.exe"

#define EQ_MOUSE_WHEEL_ZOOM_PATCH_CHECK_ADDRESS 0x0055B2EC
#define EQ_MOUSE_WHEEL_ZOOM_PATCH_CHECK_VALUE 0x0C2474FF
#define EQ_MOUSE_WHEEL_ZOOM_PATCH_CHECK_SIZE 4

#define EQ_MOUSE_WHEEL_ZOOM_PATCH_JUMP1_ADDRESS 0x0055B2EC
#define EQ_MOUSE_WHEEL_ZOOM_PATCH_JUMP1_BYTES "\xE9\x0F\x71\x08\x00\x90\x90\x90\x90"
#define EQ_MOUSE_WHEEL_ZOOM_PATCH_JUMP1_SIZE 9

#define EQ_MOUSE_WHEEL_ZOOM_PATCH_CAVE1_ADDRESS 0x005E2400
#define EQ_MOUSE_WHEEL_ZOOM_PATCH_CAVE1_BYTES "\x8B\x44\x24\x0C\x89\x05\x04\x90\x79\x00\xFF\x74\x24\x0C\xE8\xAD\xCD\xFB\xFF\xE9\xDD\x8E\xF7\xFF"
#define EQ_MOUSE_WHEEL_ZOOM_PATCH_CAVE1_SIZE 24

#define EQ_MOUSE_WHEEL_ZOOM_PATCH_JUMP2_ADDRESS 0x0059F225
#define EQ_MOUSE_WHEEL_ZOOM_PATCH_JUMP2_BYTES "\xE9\xF6\x31\x04\x00"
#define EQ_MOUSE_WHEEL_ZOOM_PATCH_JUMP2_SIZE 5

#define EQ_MOUSE_WHEEL_ZOOM_PATCH_CAVE2_ADDRESS 0x005E2420
#define EQ_MOUSE_WHEEL_ZOOM_PATCH_CAVE2_BYTES "\x85\xFF\x0F\x85\x4F\x01\x00\x00\xA1\x78\x94\x80\x00\x8B\x80\xAC\x05\x00\x00\x83\xF8\x05\x0F\x85\x3B\x01\x00\x00\xA1\x04\x90\x79\x00\x85\xC0\x0F\x84\x2E\x01\x00\x00\x83\xF8\x78\x0F\x84\x05\x00\x00\x00\xE9\x1B\x00\x00\x00\x0F\xBE\x05\x68\xBE\x63\x00\x83\xF8\x02\x0F\x84\x2F\x00\x00\x00\x0F\x8F\x65\x00\x00\x00\xE9\x05\x01\x00\x00\x0F\xBE\x05\x68\xBE\x63\x00\x83\xF8\x00\x0F\x84\x6F\x00\x00\x00\x83\xF8\x02\x0F\x84\x66\x00\x00\x00\x0F\x8F\xDA\x00\x00\x00\xE9\xE1\x00\x00\x00\xA1\xCC\x94\x7F\x00\x8D\x80\x90\x00\x00\x00\xD9\x05\xC0\x96\x79\x00\xD8\x18\xDF\xE0\x9E\x0F\x86\x20\x00\x00\x00\xA1\xCC\x94\x7F\x00\x8D\x80\x90\x00\x00\x00\xD9\x05\xC0\x96\x79\x00\xD8\x20\xD8\x20\xD9\x1D\xC0\x96\x79\x00\xE9\xA5\x00\x00\x00\xC6\x05\x68\xBE\x63\x00\x00\xA1\xCC\x94\x7F\x00\x8D\x80\x90\x00\x00\x00\xD9\x00\xD9\x1D\xC0\x96\x79\x00\xE9\x86\x00\x00\x00\x0F\xBE\x05\x68\xBE\x63\x00\x83\xF8\x00\x0F\x84\x3A\x00\x00\x00\xE9\x00\x00\x00\x00\xD9\x05\xC0\x96\x79\x00\xD8\x1D\x38\x85\x5E\x00\xDF\xE0\x9E\x0F\x87\x3F\x00\x00\x00\xA1\xCC\x94\x7F\x00\x8D\x80\x90\x00\x00\x00\xD9\x05\xC0\x96\x79\x00\xD8\x00\xD8\x00\xD9\x1D\xC0\x96\x79\x00\xE9\x3C\x00\x00\x00\xC6\x05\x68\xBE\x63\x00\x02\xA1\xCC\x94\x7F\x00\x8D\x80\x90\x00\x00\x00\xD9\x00\xD9\x1D\xC0\x96\x79\x00\xE9\x1D\x00\x00\x00\xD9\x05\x38\x85\x5E\x00\xD9\x1D\xC0\x96\x79\x00\xE9\x0C\x00\x00\x00\xC6\x05\x68\xBE\x63\x00\x02\xE9\x00\x00\x00\x00\x8B\x46\x34\x39\xC7\xE9\xA9\xCC\xFB\xFF"
#define EQ_MOUSE_WHEEL_ZOOM_PATCH_CAVE2_SIZE 353

void enable_debug_privileges();
int eq_should_apply_mouse_wheel_zoom_patch(HANDLE process_handle);
void eq_apply_mouse_wheel_zoom_patch(HANDLE process_handle);
void do_update();
void tray_icon_create(HWND hwnd);
void on_create(HWND hwnd);
void on_destroy(HWND hwnd);
void on_tray(HWND hwnd, WPARAM wparam, LPARAM lparam);
void on_timer(HWND hwnd, WPARAM wparam);
LRESULT CALLBACK window_proc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

void enable_debug_privileges()
{
    HANDLE token;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
    {
        TOKEN_PRIVILEGES tp;
        TOKEN_PRIVILEGES tpPrevious;

        DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);

        LUID luid;

        if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
        {
            tp.PrivilegeCount           = 1;
            tp.Privileges[0].Luid       = luid;
            tp.Privileges[0].Attributes = 0;

            AdjustTokenPrivileges
            (
                token,
                FALSE,
                &tp,
                sizeof(TOKEN_PRIVILEGES),
                &tpPrevious,
                &cbPrevious
            );

            tpPrevious.PrivilegeCount            = 1;
            tpPrevious.Privileges[0].Luid        = luid;
            tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
    
            AdjustTokenPrivileges
            (
                token,
                FALSE,
                &tpPrevious,
                cbPrevious,
                NULL,
                NULL
            );
        }
    }

    CloseHandle(token);
}

int eq_should_apply_mouse_wheel_zoom_patch(HANDLE process_handle)
{
    if (process_handle == NULL)
    {
        return 0;
    }

    DWORD check_value;
    ReadProcessMemory
    (
        process_handle,
        (LPVOID)EQ_MOUSE_WHEEL_ZOOM_PATCH_CHECK_ADDRESS,
        &check_value,
        EQ_MOUSE_WHEEL_ZOOM_PATCH_CHECK_SIZE,
        NULL
    );

    if (check_value == EQ_MOUSE_WHEEL_ZOOM_PATCH_CHECK_VALUE)
    {
        return 1;
    }

    return 0;
}

void eq_apply_mouse_wheel_zoom_patch(HANDLE process_handle)
{
    if (process_handle == NULL)
    {
        return;
    }

    WriteProcessMemory
    (
        process_handle,
        (LPVOID)EQ_MOUSE_WHEEL_ZOOM_PATCH_CAVE1_ADDRESS,
        EQ_MOUSE_WHEEL_ZOOM_PATCH_CAVE1_BYTES,
        EQ_MOUSE_WHEEL_ZOOM_PATCH_CAVE1_SIZE,
        NULL
    );

    WriteProcessMemory
    (
        process_handle,
        (LPVOID)EQ_MOUSE_WHEEL_ZOOM_PATCH_JUMP1_ADDRESS,
        EQ_MOUSE_WHEEL_ZOOM_PATCH_JUMP1_BYTES,
        EQ_MOUSE_WHEEL_ZOOM_PATCH_JUMP1_SIZE,
        NULL
    );

    WriteProcessMemory
    (
        process_handle,
        (LPVOID)EQ_MOUSE_WHEEL_ZOOM_PATCH_CAVE2_ADDRESS,
        EQ_MOUSE_WHEEL_ZOOM_PATCH_CAVE2_BYTES,
        EQ_MOUSE_WHEEL_ZOOM_PATCH_CAVE2_SIZE,
        NULL
    );

    WriteProcessMemory
    (
        process_handle,
        (LPVOID)EQ_MOUSE_WHEEL_ZOOM_PATCH_JUMP2_ADDRESS,
        EQ_MOUSE_WHEEL_ZOOM_PATCH_JUMP2_BYTES,
        EQ_MOUSE_WHEEL_ZOOM_PATCH_JUMP2_SIZE,
        NULL
    );
}

void do_update()
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

        HMODULE module;
        DWORD needed_ex;
        if (EnumProcessModules(process_handle, &module, sizeof(module), &needed_ex))
        {
            char process_name[MAX_PATH] = {0};
            GetModuleBaseName(process_handle, module, process_name, sizeof(process_name));

            if (strcmp(process_name, EQ_CLIENT) == 0)
            {
                if (eq_should_apply_mouse_wheel_zoom_patch(process_handle) == 1)
                {
                    eq_apply_mouse_wheel_zoom_patch(process_handle);
                }
            }
        }

        CloseHandle(process_handle);
    }
}

void tray_icon_create(HWND hwnd)
{
    tray_icon.cbSize           = sizeof(NOTIFYICONDATA);
    tray_icon.hWnd             = hwnd;
    tray_icon.uID              = ID_TRAYICON;
    tray_icon.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    tray_icon.uCallbackMessage = WM_SHELLNOTIFY;
    //tray_icon.hIcon            = LoadIcon(NULL, IDI_WINLOGO);

    tray_icon.hIcon = (HICON)LoadImage
    (
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(ID_APP_ICON),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON),
        LR_SHARED
    );

    strcpy(tray_icon.szTip, APP_NAME);

    Shell_NotifyIcon(NIM_ADD, &tray_icon);
}

void on_create(HWND hwnd)
{
    enable_debug_privileges();

    tray_icon_create(hwnd);

    SetTimer(hwnd, ID_TIMER_UPDATE, 1000, (TIMERPROC)NULL);
}

void on_destroy(HWND hwnd)
{
    Shell_NotifyIcon(NIM_DELETE, &tray_icon);

    KillTimer(hwnd, ID_TIMER_UPDATE);

    PostQuitMessage(0);
}

void on_tray(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    if (wparam == ID_TRAYICON)
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
        case ID_TIMER_UPDATE:
            do_update();
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
    HWND application_window = FindWindow(NULL, APP_NAME);
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
    //wcx.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
    //wcx.hIconSm       = LoadIcon(NULL, IDI_WINLOGO);
    wcx.hIcon = (HICON)LoadImage
    (
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(ID_APP_ICON),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXICON),
        GetSystemMetrics(SM_CYICON),
        LR_SHARED
    );
    wcx.hIconSm = (HICON)LoadImage
    (
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(ID_APP_ICON),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON),
        LR_SHARED
    );
    wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    wcx.lpszMenuName  = NULL;
    wcx.lpszClassName = APP_CLASSNAME;

    RegisterClassEx(&wcx);

    window_hwnd = CreateWindowEx
    (
        WS_EX_DLGMODALFRAME,
        APP_CLASSNAME,
        APP_NAME,
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_HEIGHT, WINDOW_WIDTH,
        HWND_DESKTOP,
        NULL,
        hinstance,
        NULL
    );

    //ShowWindow(window_hwnd, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
