#include <string>
#include <sstream>

#include <windows.h>

#include "detours.h"
#pragma comment(lib, "detours.lib")

#include "vk_keys.h"

#include "cmemory.hpp"

#include "eqmac.hpp"
#include "eqmac_functions.hpp"

CMemory g_memory;

HMODULE g_module;

EQ_FUNCTION_TYPE_DrawNetStatus DrawNetStatus_REAL = NULL;

int __cdecl DrawNetStatus_DETOUR(int a1, unsigned short a2, unsigned short a3, unsigned short a4, unsigned short a5, int a6, unsigned short a7, unsigned long a8, long a9, unsigned long a10)
{
    DWORD fontArial14 = g_memory.ReadAny<DWORD>(EQ_POINTER_FONT_ARIAL14);

    DWORD worldSpaceToScreenSpaceCameraData = g_memory.ReadAny<DWORD>(EQ_POINTER_WORLD_SPACE_TO_SCREEN_SPACE_CAMERA_DATA);

    DWORD playerSpawn = g_memory.ReadAny<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);

    float playerY = g_memory.ReadAny<float>(playerSpawn + EQ_OFFSET_SPAWN_INFO_Y);
    float playerX = g_memory.ReadAny<float>(playerSpawn + EQ_OFFSET_SPAWN_INFO_X);
    float playerZ = g_memory.ReadAny<float>(playerSpawn + EQ_OFFSET_SPAWN_INFO_Z);

    EQLOCATION playerLocation = {playerY, playerX, playerZ};

    float resultX = 100.0f;
    float resultY = 100.0f;
    int result = EQGfx_Dx8__t3dWorldSpaceToScreenSpace(worldSpaceToScreenSpaceCameraData, &playerLocation, &resultX, &resultY);

    int screenX = (int)resultX;
    int screenY = (int)resultY;

    if (result == EQ_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
    {
        EQ_CLASS_CDisplay->WriteTextHD2("Fail", screenX, screenY, EQ_TEXT_COLOR_PINK, fontArial14);

        return DrawNetStatus_REAL(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    }

    char playerText[128] = "Player";

    EQ_CLASS_CDisplay->WriteTextHD2(playerText, screenX, screenY, EQ_TEXT_COLOR_PINK, fontArial14);

    return DrawNetStatus_REAL(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

DWORD WINAPI threadLoop(LPVOID param)
{
    while (1)
    {
        if (EQ_IsInGame(&g_memory) == false)
        {
            continue;
        }

        Sleep(1);
    }

    FreeLibraryAndExitThread(g_module, 0);
    return 0;
}

DWORD WINAPI threadLoad(LPVOID param)
{
    g_memory.EnableDebugPrivileges();
    g_memory.SetProcessById(GetCurrentProcessId());

    HINSTANCE graphicsDll = LoadLibraryA("EQGfx_Dx8.dll");
    if (!graphicsDll)
    {
        MessageBoxA(NULL, "Error: Failed to LoadLibrary EQGfx_Dx8.dll!", "EQMac ESP", MB_ICONERROR);

        FreeLibraryAndExitThread(g_module, 0);
        return 0;
    }

    EQGfx_Dx8__t3dWorldSpaceToScreenSpace = (EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dWorldSpaceToScreenSpace)GetProcAddress(graphicsDll, EQ_WORLD_SPACE_TO_SCREEN_SPACE_FUNCTION_NAME);
    if (!EQGfx_Dx8__t3dWorldSpaceToScreenSpace)
    {
        MessageBoxA(NULL, "Error: World to Screen function not found!", "EQMac ESP", MB_ICONERROR);

        FreeLibraryAndExitThread(g_module, 0);
        return 0;
    }

    DrawNetStatus_REAL = (EQ_FUNCTION_TYPE_DrawNetStatus)DetourFunction((PBYTE)EQ_FUNCTION_DrawNetStatus, (PBYTE)DrawNetStatus_DETOUR);

    CreateThread(NULL, 0, &threadLoop, NULL, 0, NULL);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    g_module = module;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(module);
            CreateThread(NULL, 0, &threadLoad, NULL, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            DetourRemove((PBYTE)DrawNetStatus_REAL, (PBYTE)DrawNetStatus_DETOUR);
            break;
    }

    return TRUE;
}
