#include <cstring>

#include <windows.h>

#include "detours.h"
#pragma comment(lib, "detours.lib")

#include "eqmac.hpp"
#include "eqmac_functions.hpp"

HMODULE g_module;

EQ_FUNCTION_TYPE_DrawNetStatus DrawNetStatus_REAL = NULL;

int __cdecl DrawNetStatus_DETOUR(int a1, unsigned short a2, unsigned short a3, unsigned short a4, unsigned short a5, int a6, unsigned short a7, unsigned long a8, long a9, unsigned long a10)
{
    BYTE isInGame = EQ_READ_MEMORY<BYTE>(EQ_IS_IN_GAME);

    if (isInGame == 0)
    {
        return DrawNetStatus_REAL(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    }

    DWORD fontArial14 = EQ_READ_MEMORY<DWORD>(EQ_POINTER_FONT_ARIAL14);

    DWORD worldSpaceToScreenSpaceCameraData = EQ_READ_MEMORY<DWORD>(EQ_POINTER_WORLD_SPACE_TO_SCREEN_SPACE_CAMERA_DATA);

    PEQSPAWNINFO spawn = (PEQSPAWNINFO)EQ_OBJECT_FirstSpawn;

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    while (spawn)
    {
        if (spawn == playerSpawn)
        {
            spawn = spawn->Next;
            continue;
        }

        EQLOCATION playerLocation = {EQ_OBJECT_PlayerSpawn->Y, EQ_OBJECT_PlayerSpawn->X, EQ_OBJECT_PlayerSpawn->Z}; 

        EQLOCATION spawnLocation = {spawn->Y, spawn->X, spawn->Z};

        DWORD spawnType = spawn->Type;

        float spawnDistance = EQ_CalculateDistance(playerLocation.X, playerLocation.Y, spawnLocation.X, spawnLocation.Y);

        if (spawnDistance > 400.0f && spawnType == EQ_SPAWN_TYPE_NPC)
        {
            spawn = spawn->Next;
            continue;
        }

        float resultX = 0.0f;
        float resultY = 0.0f;
        int result = EQGfx_Dx8__t3dWorldSpaceToScreenSpace(worldSpaceToScreenSpaceCameraData, &spawnLocation, &resultX, &resultY);

        if (result != EQ_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
        {
            int screenX = (int)resultX;
            int screenY = (int)resultY;

            const char* spawnName = EQ_CLASS_CEverQuest->trimName(spawn->Name);

            char spawnText[128];
            sprintf_s(spawnText, "+ %s", spawnName);

            DWORD textColor = EQ_TEXT_COLOR_WHITE;

            switch (spawnType)
            {
                case EQ_SPAWN_TYPE_PLAYER:
                    textColor = EQ_TEXT_COLOR_RED;
                    break;

                case EQ_SPAWN_TYPE_NPC:
                    textColor = EQ_TEXT_COLOR_CYAN;
                    break;

                case EQ_SPAWN_TYPE_NPC_CORPSE:
                case EQ_SPAWN_TYPE_PLAYER_CORPSE:
                    textColor = EQ_TEXT_COLOR_YELLOW;
                    break;
            }

            if (spawn == targetSpawn)
            {
                textColor = EQ_TEXT_COLOR_PINK;
            }

            if (spawn->IsGameMaster == 1)
            {
                textColor = EQ_TEXT_COLOR_PINK;

                strcat_s(spawnText, " GM");
            }

            if (spawn->IsPlayerKill == 1)
            {
                strcat_s(spawnText, " PVP");
            }

            if (spawn->IsAwayFromKeyboard == 1)
            {
                strcat_s(spawnText, " AFK");
            }

            if (spawn->IsLinkDead == 1 && spawnType == EQ_SPAWN_TYPE_PLAYER)
            {
                strcat_s(spawnText, " LD");
            }

            char distanceText[32];
            sprintf_s(distanceText, " (%d)", (int)spawnDistance);

            strcat_s(spawnText, distanceText);

            EQ_CLASS_CDisplay->WriteTextHD2(spawnText, screenX, screenY, textColor, fontArial14);
        }

        spawn = spawn->Next;
    }

    return DrawNetStatus_REAL(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

/*
DWORD WINAPI threadLoop(LPVOID param)
{
    while (1)
    {
        //

        Sleep(1);
    }

    FreeLibraryAndExitThread(g_module, 0);
    return 0;
}
*/

DWORD WINAPI threadLoad(LPVOID param)
{
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

    //CreateThread(NULL, 0, &threadLoop, NULL, 0, NULL);

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
