#include <cstdio>
#include <cstring>
#include <cmath>

#include <windows.h>

#include "vk_keys.h"

#include "eqmac.h"
#include "eqmac_functions.h"

const char* g_applicationName = "EQMac Keys";

HMODULE g_module;

HANDLE g_handleThreadLoad;
HANDLE g_handleThreadLoop;

unsigned int g_hotkeyTimer = 0;
unsigned int g_hotkeyDelay = 500;

bool EQMACKEYS_IsForegroundWindowCurrentProcessId()
{
    HWND foregroundHwnd = GetForegroundWindow();

    DWORD foregroundProcessId;
    GetWindowThreadProcessId(foregroundHwnd, &foregroundProcessId);

    return (foregroundProcessId == GetCurrentProcessId());
}

DWORD WINAPI EQMACKEYS_ThreadLoop(LPVOID param)
{
    while (TRUE)
    {
        if (EQ_OBJECT_CEverQuest == NULL || EQ_OBJECT_CEverQuest->GameState != EQ_GAME_STATE_IN_GAME)
        {
            Sleep(1000);
            continue;
        }

        DWORD currentTime = EQ_ReadMemory<DWORD>(EQ_TIMER);

        if ((currentTime - g_hotkeyTimer) < g_hotkeyDelay)
        {
            Sleep(10);
            continue;
        }

        if (EQMACKEYS_IsForegroundWindowCurrentProcessId() == true)
        {
            if (KEYDOWN(VK_PAUSE))
            {
                break;
            }
        }
        else
        {
            if (KEYDOWN(VK_1) && KEYDOWN(VK_CONTROL))
            {
                EQ_CLASS_CHotButtonWnd->DoHotButton(0, 1);

                g_hotkeyTimer = currentTime;
            }

            if (KEYDOWN(VK_2) && KEYDOWN(VK_CONTROL))
            {
                EQ_CLASS_CHotButtonWnd->DoHotButton(1, 1);

                g_hotkeyTimer = currentTime;
            }

            if (KEYDOWN(VK_3) && KEYDOWN(VK_CONTROL))
            {
                EQ_CLASS_CHotButtonWnd->DoHotButton(2, 1);

                g_hotkeyTimer = currentTime;
            }

            if (KEYDOWN(VK_4) && KEYDOWN(VK_CONTROL))
            {
                EQ_CLASS_CHotButtonWnd->DoHotButton(3, 1);

                g_hotkeyTimer = currentTime;
            }

            if (KEYDOWN(VK_5) && KEYDOWN(VK_CONTROL))
            {
                EQ_CLASS_CHotButtonWnd->DoHotButton(4, 1);

                g_hotkeyTimer = currentTime;
            }

            if (KEYDOWN(VK_6) && KEYDOWN(VK_CONTROL))
            {
                EQ_CLASS_CHotButtonWnd->DoHotButton(5, 1);

                g_hotkeyTimer = currentTime;
            }

            if (KEYDOWN(VK_7) && KEYDOWN(VK_CONTROL))
            {
                EQ_CLASS_CHotButtonWnd->DoHotButton(6, 1);

                g_hotkeyTimer = currentTime;
            }

            if (KEYDOWN(VK_8) && KEYDOWN(VK_CONTROL))
            {
                EQ_CLASS_CHotButtonWnd->DoHotButton(7, 1);

                g_hotkeyTimer = currentTime;
            }

            if (KEYDOWN(VK_9) && KEYDOWN(VK_CONTROL))
            {
                EQ_CLASS_CHotButtonWnd->DoHotButton(8, 1);

                g_hotkeyTimer = currentTime;
            }

            if (KEYDOWN(VK_0) && KEYDOWN(VK_CONTROL))
            {
                EQ_CLASS_CHotButtonWnd->DoHotButton(9, 1);

                g_hotkeyTimer = currentTime;
            }
        }

        Sleep(10);
    }

    Sleep(1000);

    FreeLibraryAndExitThread(g_module, 0);
    return 0;
}

DWORD WINAPI EQMACKEYS_ThreadLoad(LPVOID param)
{
    g_handleThreadLoop = CreateThread(NULL, 0, &EQMACKEYS_ThreadLoop, NULL, 0, NULL);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    g_module = module;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(module);
            g_handleThreadLoad = CreateThread(NULL, 0, &EQMACKEYS_ThreadLoad, NULL, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
