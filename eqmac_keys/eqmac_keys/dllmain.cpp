#include <cstdio>
#include <cstring>
#include <cmath>

#include <windows.h>

#include "vk_keys.h"

#include "eqmac.hpp"
#include "eqmac_functions.hpp"

HMODULE g_module;

HANDLE g_handleThreadLoad;
HANDLE g_handleThreadLoop;

const char* g_applicationName = "EQMac Keys";

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
    BYTE isInGame;

    isInGame = EQ_READ_MEMORY<BYTE>(EQ_IS_IN_GAME);

    if (isInGame == 1)
    {
        char loadedText[128];
        sprintf_s(loadedText, "%s loaded.", g_applicationName);

        EQ_CLASS_CEverQuest->dsp_chat(loadedText);
    }

    while (true)
    {
        isInGame = EQ_READ_MEMORY<BYTE>(EQ_IS_IN_GAME);

        if (isInGame == 0)
        {
            Sleep(1000);
            continue;
        }

        DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

        if ((currentTime - g_hotkeyTimer) < g_hotkeyDelay)
        {
            Sleep(10);
            continue;
        }

        if (EQMACKEYS_IsForegroundWindowCurrentProcessId() == true)
        {
            if
            (
                (KEYDOWN(VK_CONTROL)) &&
                (KEYDOWN(VK_ALT))     &&
                (KEYDOWN(VK_BACK))
            )
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

    isInGame = EQ_READ_MEMORY<BYTE>(EQ_IS_IN_GAME);

    if (isInGame == 1)
    {
        char unloadedText[128];
        sprintf_s(unloadedText, "%s unloaded.", g_applicationName);

        EQ_CLASS_CEverQuest->dsp_chat(unloadedText);
    }

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
            //WaitForSingleObject(g_handleThreadLoad, INFINITE);
            CloseHandle(g_handleThreadLoad);

            //WaitForSingleObject(g_handleThreadLoop, INFINITE);
            CloseHandle(g_handleThreadLoop);
            break;
    }

    return TRUE;
}
