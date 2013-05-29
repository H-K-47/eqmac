#SingleInstance, Ignore
#Persistent

#Include, eqmac.ahk

Global psapi_dll_handle := 0

Global target_address := EVERQUEST_SPAWN_INFO_NULL

SetTitleMatchMode, 2

CoordMode, Pixel

EnableDebugPrivileges()

psapi_dll_handle := DllCall("LoadLibrary", "Str", "Psapi.dll")

Global player_y_prev := 0
Global player_x_prev := 0

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

SetTimer, TimerBotCheckIdle, 15000

SetTimer, TimerBotSurvive, 100

SetTimer, TimerBotUpdateTarget, 1

SetTimer, TimerBot, 1000

SetTimer, TimerUpdateProcess, 100

Return

Bot_GetNumberPC()
{
    num_pc := 0

    player_spawn_info := everquest_GetPlayerSpawnInfoPointer()

    spawn_info_address := player_spawn_info

    spawn_next_spawn_info := Memory_Read(everquest_process_handle,  spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER)

    spawn_info_address := spawn_next_spawn_info

    Loop, %EVERQUEST_SPAWNS_MAX%
    {
        spawn_next_spawn_info := Memory_Read(everquest_process_handle,  spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER)

        If (spawn_next_spawn_info = EVERQUEST_SPAWN_INFO_NULL)
        {
            Break
        }

        spawn_type := Memory_ReadEx(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_TYPE, 1)

        If (spawn_type = EVERQUEST_SPAWN_INFO_TYPE_PLAYER)
        {
            num_pc++
        }

        spawn_info_address := spawn_next_spawn_info
    }

    Return, num_pc
}

TimerBotCheckIdle:
IfWinNotActive, %EVERQUEST_TITLE%
{
    Return
}

num_pc := Bot_GetNumberPC()

;MsgBox, num_pc: %num_pc%

If (num_pc > 0)
{
    Return
}

zone_short_name := Memory_ReadString(everquest_process_handle, EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SHORT_NAME, 0x20)

IfInString, zone_short_name, bazaar
{
    Return
}

player_spawn_info := everquest_GetPlayerSpawnInfoPointer()

player_y := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y)
player_x := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X)

If (player_y = player_y_prev)
{
    If (player_x = player_x_prev)
    {
        Loop, 5
        {
            SendInput, {Escape}
        }

        Sleep, 100

        SendInput, {F8}

        Sleep, 1000

        Return
    }
}

player_y_prev := player_y
player_x_prev := player_x

Return

TimerBotSurvive:
IfWinNotActive, %EVERQUEST_TITLE%
{
    Return
}

zone_short_name := Memory_ReadString(everquest_process_handle, EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SHORT_NAME, 0x20)

IfInString, zone_short_name, bazaar
{
    Return
}

player_spawn_info := everquest_GetPlayerSpawnInfoPointer()

player_hp     := Memory_Read(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HP_CURRENT)
player_hp_max := Memory_Read(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HP_MAX)

If (player_hp > 0)
{
    If (player_hp < (player_hp_max / 2))
    {
        SendInput, {F12}

        Sleep, 1000

        ExitApp
    }
}

Return

TimerBotUpdateTarget:
IfWinNotActive, %EVERQUEST_TITLE%
{
    Return
}

num_pc := Bot_GetNumberPC()

;MsgBox, num_pc: %num_pc%

If (num_pc > 0)
{
    Return
}

zone_short_name := Memory_ReadString(everquest_process_handle, EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SHORT_NAME, 0x20)

IfInString, zone_short_name, bazaar
{
    Return
}

target_spawn_info := everquest_GetTargetSpawnInfoPointer()

If (target_spawn_info != EVERQUEST_SPAWN_INFO_NULL)
{
    target_address := target_spawn_info
}
Else
{
    If (target_address != EVERQUEST_SPAWN_INFO_NULL)
    {
        Memory_Write(everquest_process_handle, EVERQUEST_TARGET_SPAWN_INFO_POINTER, target_address)
    }
}

Return

TimerBot:
IfWinNotActive, %EVERQUEST_TITLE%
{
    Return
}

num_pc := Bot_GetNumberPC()

;MsgBox, num_pc: %num_pc%

If (num_pc > 0)
{
    Return
}

zone_short_name := Memory_ReadString(everquest_process_handle, EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SHORT_NAME, 0x20)

IfInString, zone_short_name, bazaar
{
    Return
}

target_spawn_info := everquest_GetTargetSpawnInfoPointer()

If (target_spawn_info != EVERQUEST_SPAWN_INFO_NULL)
{
    target_address := target_spawn_info
}
Else
{
    If (target_address != EVERQUEST_SPAWN_INFO_NULL)
    {
        Memory_Write(everquest_process_handle, EVERQUEST_TARGET_SPAWN_INFO_POINTER, target_address)
    }
}

target_spawn_info := everquest_GetTargetSpawnInfoPointer()

If (target_spawn_info != EVERQUEST_SPAWN_INFO_NULL)
{
    target_name := Memory_ReadString(everquest_process_handle, target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_NAME, 0x40)

    IfInString, target_name, corpse
    {
        num_pc := Bot_GetNumberPC()

        ;MsgBox, num_pc: %num_pc%

        If (num_pc > 0)
        {
            Return
        }

        SendInput, {F7}

        Sleep, 1000

        Loop, 5
        {
            SendInput, {4}
        }

        Sleep, 5000

        player_spawn_info := everquest_GetPlayerSpawnInfoPointer()

        player_standing_state := Memory_ReadEx(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE, 1)

        If (player_standing_state != EVERQUEST_STANDING_STATE_LOOTING)
        {
            Memory_Write(everquest_process_handle, EVERQUEST_TARGET_SPAWN_INFO_POINTER, EVERQUEST_SPAWN_INFO_NULL)

            target_address := EVERQUEST_SPAWN_INFO_NULL

            Sleep, 1000

            Loop
            {
                num_pc := Bot_GetNumberPC()

                ;MsgBox, num_pc: %num_pc%

                If (num_pc > 0)
                {
                    Return
                }

                SendInput, {F8}

                Sleep, 1000

                target_spawn_info := everquest_GetTargetSpawnInfoPointer()

                If (target_spawn_info = EVERQUEST_SPAWN_INFO_NULL)
                {
                    num_pc := Bot_GetNumberPC()

                    ;MsgBox, num_pc: %num_pc%

                    If (num_pc > 0)
                    {
                        Return
                    }

                    SendInput, {Shift down}
                    Sleep, 100
                    SendInput, {LShift}{F8}
                    Sleep, 100
                    SendInput, {Shift up}
                    Sleep, 100

                    Sleep, 1000

                    target_spawn_info := everquest_GetTargetSpawnInfoPointer()

                    If (target_spawn_info = EVERQUEST_SPAWN_INFO_NULL)
                    {
                        Continue
                    }
                    Else
                    {
                        target_address := target_spawn_info

                        Break
                    }
                }
                Else
                {
                    target_address := target_spawn_info

                    Break
                }
            }
        }
    }
}

is_attacking := Memory_Read(everquest_process_handle, EVERQUEST_IS_ATTACKING_READ_ONLY)

If (is_attacking = 0)
{
    Memory_Write(everquest_process_handle, EVERQUEST_IS_ATTACKING_TOGGLE, EVERQUEST_IS_ATTACKING_TOGGLE_TRUE)
}

player_spawn_info := everquest_GetPlayerSpawnInfoPointer()
target_spawn_info := everquest_GetTargetSpawnInfoPointer()

If (target_spawn_info != EVERQUEST_SPAWN_INFO_NULL)
{
    player_y := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y)
    player_x := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X)

    target_y := Memory_ReadFloat(everquest_process_handle, target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y)
    target_x := Memory_ReadFloat(everquest_process_handle, target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X)

    target_distance := CalculateDistance(player_x, player_y, target_x, target_y)

    ;ToolTip, Target Distance: %target_distance%, 0, 32

    If (target_distance > 8)
    {
        Send {Up down}

        Sleep, 100

        Send {Up up}

        Sleep, 10

        num_pc := Bot_GetNumberPC()

        ;MsgBox, num_pc: %num_pc%

        If (num_pc > 0)
        {
            Return
        }

        SendInput, {F7}

        Sleep, 1000
    }

    If (target_distance < 8)
    {
        Send {Down down}

        Sleep, 10

        Send {Down up}
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
