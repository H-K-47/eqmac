#SingleInstance, Ignore
#Persistent

#Include, eqmac.ahk

Global psapi_dll_handle := 0

SetTitleMatchMode, 2

CoordMode, Pixel

EnableDebugPrivileges()

psapi_dll_handle := DllCall("LoadLibrary", "Str", "Psapi.dll")

everquest_process_id := Memory_GetProcessID(EVERQUEST_CLIENT)

;MsgBox, everquest_process_id = %everquest_process_id%

If (everquest_process_id = 0)
{
    MsgBox, Error: everquest_process_id = 0
    ExitApp
    Return
}

everquest_process_handle := Memory_GetProcessHandle(everquest_process_id)

;MsgBox, everquest_process_handle = %everquest_process_handle%

If (everquest_process_handle = 0)
{
    MsgBox, Error: OpenProcess failed! Result: %A_LastError%
    ExitApp
    Return
}

SetTimer, TimerLootSpiderSilks, 100

SetTimer, TimerUpdateProcess, 100

Return

TimerLootSpiderSilks:

IfWinNotActive, %EVERQUEST_TITLE%
{
    Return
}

ImageSearch, image_x, image_y, 0, 0, A_ScreenWidth, A_ScreenHeight, %A_ScriptDir%\images\loot.bmp
Sleep, 100
If ErrorLevel = 0
{
    ImageSearch, image_x, image_y, 0, 0, A_ScreenWidth, A_ScreenHeight, %A_ScriptDir%\images\spidersilk.bmp
    Sleep, 100
    If ErrorLevel = 0
    {
        ;MouseMove, %image_x%, %image_y%

        Memory_Write(everquest_process_handle, EVERQUEST_MOUSE_X, image_x)
        Memory_Write(everquest_process_handle, EVERQUEST_MOUSE_Y, image_y)

        Sleep, 10

        Loop, 3
        {
            Click, Right
        }

        Sleep, 100

        Memory_Write(everquest_process_handle, EVERQUEST_MOUSE_X, A_ScreenWidth  / 2)
        Memory_Write(everquest_process_handle, EVERQUEST_MOUSE_Y, A_ScreenHeight / 2)
    }
    Else
    {
        SendInput, {Escape}
    }
}

Return

TimerUpdateProcess:
If WinNotActive, EverQuest
{
    Return
}

WinGet, active_process_id, PID, A

script_process_id := DllCall("GetCurrentProcessId")

If (active_process_id = script_process_id)
{
    Return
}

If (active_process_id = everquest_process_id)
{
    Return
}

psapi_size := 4096

psapi_size := VarSetCapacity(psapi_processes, psapi_size)
DllCall("Psapi.dll\EnumProcesses", "Ptr", &psapi_processes, "UInt", psapi_size, "UIntP", psapi_process)

Loop, % psapi_process // 4
{
    psapi_process_id := NumGet(psapi_processes, A_Index * 4, "UInt")

    psapi_process_handle := DllCall("OpenProcess", "UInt", 0x0010 | 0x0400, "Int", false, "UInt", psapi_process_id, "Ptr")

    If (!psapi_process_handle)
    {
        Continue
    }

    VarSetCapacity(psapi_process_name, psapi_size, 0)
    psapi_result := DllCall("Psapi.dll\GetModuleBaseName", "Ptr", psapi_process_handle, "Ptr", 0, "Str", psapi_process_name, "UInt", A_IsUnicode ? psapi_size // 2 : psapi_size)

    If (!psapi_result)
    {
        If psapi_result := DllCall("Psapi.dll\GetProcessImageFileName", "Ptr", psapi_process_handle, "Str", psapi_process_name, "UInt", A_IsUnicode ? psapi_size // 2 : psapi_size)
        {
            SplitPath, psapi_process_name, psapi_process_name
        }
    }

    DllCall("CloseHandle", "Ptr", psapi_process_handle)

    If (psapi_result && psapi_process_name)
    {
        If psapi_process_name = %EVERQUEST_CLIENT%
        {
            If (psapi_process_id = active_process_id)
            {
                everquest_process_id := psapi_process_id

                everquest_process_handle := Memory_GetProcessHandle(everquest_process_id)

                everquest_gfx_dll_address := everquest_GetGfxDllAddress()

                Break
            }
        }
    }
}

Return

GuiClose:
GuiEscape:

SetTimer, TimerUpdateProcess, Off

DllCall("FreeLibrary", "Ptr", psapi_dll_handle)
Memory_CloseHandle(everquest_process_handle)

ExitApp

Return
