#include <cstdio>
#include <cstring>
#include <cmath>

#include <windows.h>

#include "detours.h"
#pragma comment(lib, "detours.lib")

#include "eqmac.h"
#include "eqmac_functions.h"

const char* g_applicationName = "EQMac Mouse Wheel Zoom";

volatile int g_bExit = 0;

HMODULE g_module;

HANDLE g_handleThreadLoad;
HANDLE g_handleThreadLoop;

EQ_FUNCTION_TYPE_ProcessKeyDown EQMACMOUSEWHEELZOOM_REAL_ProcessKeyDown = NULL;
EQ_FUNCTION_TYPE_ProcessKeyUp   EQMACMOUSEWHEELZOOM_REAL_ProcessKeyUp   = NULL;

EQ_FUNCTION_TYPE_HandleMouseWheel EQMACMOUSEWHEELZOOM_REAL_HandleMouseWheel = NULL;

int __cdecl EQMACMOUSEWHEELZOOM_DETOUR_ProcessKeyDown(int a1)
{
    if (g_bExit == 1)
    {
        return EQMACMOUSEWHEELZOOM_REAL_ProcessKeyDown(a1);
    }

    int key = a1;

    //char keyText[128];
    //sprintf_s(keyText, "ProcessKeyDown: %d", key);
    //EQ_CLASS_CEverQuest->dsp_chat(keyText);

    return EQMACMOUSEWHEELZOOM_REAL_ProcessKeyDown(a1);
}

int __cdecl EQMACMOUSEWHEELZOOM_DETOUR_ProcessKeyUp(int a1)
{
    if (g_bExit == 1)
    {
        return EQMACMOUSEWHEELZOOM_REAL_ProcessKeyUp(a1);
    }

    int key = a1;

    //char keyText[128];
    //sprintf_s(keyText, "ProcessKeyUp: %d", key);
    //EQ_CLASS_CEverQuest->dsp_chat(keyText);

    if (key == EQ_KEY_PAUSE_BREAK)
    {
        g_bExit = 1;

        return EQMACMOUSEWHEELZOOM_REAL_ProcessKeyUp(EQ_KEY_NULL);
    }

    return EQMACMOUSEWHEELZOOM_REAL_ProcessKeyUp(a1);
}

int __cdecl EQMACMOUSEWHEELZOOM_DETOUR_HandleMouseWheel(int a1)
{
    if (g_bExit == 1)
    {
        return EQMACMOUSEWHEELZOOM_REAL_HandleMouseWheel(a1);
    }

    int mouseWheelDelta = a1;

    if (EQ_OBJECT_CEverQuest->GameState == EQ_GAME_STATE_IN_GAME)
    {
        DWORD mouseOverWindow = EQ_READ_MEMORY<DWORD>(EQ_CXWND_MANAGER_MOUSE_HOVER_WINDOW);

        if (mouseOverWindow == 0x00000000)
        {
            DWORD cameraView = EQ_READ_MEMORY<DWORD>(EQ_CAMERA_VIEW);

            FLOAT cameraThirdPersonZoom = 0.0f;

            FLOAT cameraThirdPersonZoomMax = EQ_READ_MEMORY<FLOAT>(EQ_CAMERA_VIEW_THIRD_PERSON_ZOOM_MAX);

            float zoom = 0.0f;

            DWORD zoomAddress = NULL;

            if (cameraView == EQ_CAMERA_VIEW_THIRD_PERSON2)
            {
                cameraThirdPersonZoom = EQ_READ_MEMORY<FLOAT>(EQ_CAMERA_VIEW_THIRD_PERSON2_ZOOM);

                zoomAddress = EQ_CAMERA_VIEW_THIRD_PERSON2_ZOOM;
            }
            else if (cameraView == EQ_CAMERA_VIEW_THIRD_PERSON3)
            {
                cameraThirdPersonZoom = EQ_READ_MEMORY<FLOAT>(EQ_CAMERA_VIEW_THIRD_PERSON3_ZOOM);

                zoomAddress = EQ_CAMERA_VIEW_THIRD_PERSON3_ZOOM;
            }
            else if (cameraView == EQ_CAMERA_VIEW_THIRD_PERSON4)
            {
                cameraThirdPersonZoom = EQ_READ_MEMORY<FLOAT>(EQ_CAMERA_VIEW_THIRD_PERSON4_ZOOM);

                zoomAddress = EQ_CAMERA_VIEW_THIRD_PERSON4_ZOOM;
            }

            if (mouseWheelDelta == EQ_MOUSE_WHEEL_DELTA_UP)
            {
                if
                (
                    cameraView == EQ_CAMERA_VIEW_THIRD_PERSON2 ||
                    cameraView == EQ_CAMERA_VIEW_THIRD_PERSON3 ||
                    cameraView == EQ_CAMERA_VIEW_THIRD_PERSON4
                )
                {
                    if (cameraThirdPersonZoom < EQ_OBJECT_PlayerSpawn->ModelHeightOffset)
                    {
                        EQ_WRITE_MEMORY<DWORD>(EQ_CAMERA_VIEW, EQ_CAMERA_VIEW_FIRST_PERSON);
                    }
                    else
                    {
                        if (zoomAddress != NULL)
                        {
                            zoom = cameraThirdPersonZoom - EQ_OBJECT_PlayerSpawn->ModelHeightOffset;

                            EQ_WRITE_MEMORY<FLOAT>(zoomAddress, zoom);
                        }
                    }
                }
            }
            else if (mouseWheelDelta == EQ_MOUSE_WHEEL_DELTA_DOWN)
            {
                if (cameraView == EQ_CAMERA_VIEW_FIRST_PERSON)
                {
                    zoom = EQ_OBJECT_PlayerSpawn->ModelHeightOffset;

                    EQ_WRITE_MEMORY<FLOAT>(EQ_CAMERA_VIEW_THIRD_PERSON2_ZOOM, zoom);

                    EQ_WRITE_MEMORY<DWORD>(EQ_CAMERA_VIEW, EQ_CAMERA_VIEW_THIRD_PERSON2);
                }
                else if
                (
                    cameraView == EQ_CAMERA_VIEW_THIRD_PERSON2 ||
                    cameraView == EQ_CAMERA_VIEW_THIRD_PERSON3 ||
                    cameraView == EQ_CAMERA_VIEW_THIRD_PERSON4
                )
                {
                    if (zoomAddress != NULL)
                    {
                        zoom = cameraThirdPersonZoom + EQ_OBJECT_PlayerSpawn->ModelHeightOffset;

                        if (zoom > cameraThirdPersonZoomMax)
                        {
                            zoom = cameraThirdPersonZoomMax;
                        }

                        EQ_WRITE_MEMORY<FLOAT>(zoomAddress, zoom);
                    }
                }
            }
        }
    }

    return EQMACMOUSEWHEELZOOM_REAL_HandleMouseWheel(a1);
}

DWORD WINAPI EQMACMOUSEWHEELZOOM_ThreadLoop(LPVOID param)
{
    while (g_bExit == 0)
    {
        Sleep(1000);
    }

    DetourRemove((PBYTE)EQMACMOUSEWHEELZOOM_REAL_ProcessKeyDown, (PBYTE)EQMACMOUSEWHEELZOOM_DETOUR_ProcessKeyDown);
    DetourRemove((PBYTE)EQMACMOUSEWHEELZOOM_REAL_ProcessKeyUp,   (PBYTE)EQMACMOUSEWHEELZOOM_DETOUR_ProcessKeyUp);

    DetourRemove((PBYTE)EQMACMOUSEWHEELZOOM_REAL_HandleMouseWheel, (PBYTE)EQMACMOUSEWHEELZOOM_DETOUR_HandleMouseWheel);

    FreeLibraryAndExitThread(g_module, 0);
    return 0;
}

DWORD WINAPI EQMACMOUSEWHEELZOOM_ThreadLoad(LPVOID param)
{
    EQMACMOUSEWHEELZOOM_REAL_ProcessKeyDown = (EQ_FUNCTION_TYPE_ProcessKeyDown)DetourFunction((PBYTE)EQ_FUNCTION_ProcessKeyDown, (PBYTE)EQMACMOUSEWHEELZOOM_DETOUR_ProcessKeyDown);
    EQMACMOUSEWHEELZOOM_REAL_ProcessKeyUp   = (EQ_FUNCTION_TYPE_ProcessKeyUp)  DetourFunction((PBYTE)EQ_FUNCTION_ProcessKeyUp,   (PBYTE)EQMACMOUSEWHEELZOOM_DETOUR_ProcessKeyUp);

    EQMACMOUSEWHEELZOOM_REAL_HandleMouseWheel = (EQ_FUNCTION_TYPE_HandleMouseWheel)DetourFunction((PBYTE)EQ_FUNCTION_HandleMouseWheel, (PBYTE)EQMACMOUSEWHEELZOOM_DETOUR_HandleMouseWheel);

    g_handleThreadLoop = CreateThread(NULL, 0, &EQMACMOUSEWHEELZOOM_ThreadLoop, NULL, 0, NULL);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    g_module = module;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(module);
            g_handleThreadLoad = CreateThread(NULL, 0, &EQMACMOUSEWHEELZOOM_ThreadLoad, NULL, 0, NULL);
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
