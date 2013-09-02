#SingleInstance, Ignore
#Persistent

#Include, eqmac.ahk

SetTitleMatchMode, 2

CoordMode, Pixel

EnableDebugPrivileges()

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

Memory_Write(everquest_process_handle, EVERQUEST_MOUSE_X, 640 / 2)
Memory_Write(everquest_process_handle, EVERQUEST_MOUSE_Y, 480 / 2)

Sleep, 5000

SetTimer, TimerBot, 1000

Return

ClickLocation(x, y)
{
    Memory_Write(everquest_process_handle, EVERQUEST_MOUSE_X, x)
    Memory_Write(everquest_process_handle, EVERQUEST_MOUSE_Y, y)
    Sleep, 100

    Loop, 5
    {
    Click, 10
    Sleep, 100
    }

    Memory_Write(everquest_process_handle, EVERQUEST_MOUSE_X, 640 / 2)
    Memory_Write(everquest_process_handle, EVERQUEST_MOUSE_Y, 480 / 2)
    Sleep, 100
}

FindImage(image_name)
{
    ImageSearch, found_x, found_y, 0, 0, A_ScreenWidth, A_ScreenHeight, %A_ScriptDir%\images\%image_name%.bmp
    Sleep, 100
    If ErrorLevel = 0
    {
        Return, True
    }

    Return, False
}

ClickImage(image_name)
{
    ImageSearch, found_x, found_y, 0, 0, A_ScreenWidth, A_ScreenHeight, %A_ScriptDir%\images\%image_name%.bmp
    Sleep, 100
    If ErrorLevel = 0
    {
        ClickLocation(found_x, found_y)
    }
}

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

TimerBot:
IfWinNotActive, %EVERQUEST_TITLE%
{
    Return
}

num_pc := Bot_GetNumberPC()

;MsgBox, num_pc: %num_pc%

If (num_pc > 0)
{
    ;ToolTip, There are other players in the zone, 0, 0

    SendInput, {Shift down}
    Sleep, 100
    SendInput, {LShift}{F12}
    Sleep, 100
    SendInput, {Shift up}
    Sleep, 100
    Sleep, 1000

    Return
}

zone_short_name := Memory_ReadString(everquest_process_handle, EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SHORT_NAME, 0x20)

IfNotInString, zone_short_name, sseru
{
    ;ToolTip, You are not in Sanctus Seru, 0, 0

    Return
}

target_spawn_info := everquest_GetTargetSpawnInfoPointer()

If (target_spawn_info != EVERQUEST_SPAWN_INFO_NULL)
{
    target_name := Memory_ReadString(everquest_process_handle, target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_NAME, 0x40)

    IfInString, target_name, Malikin
    {
        ;ToolTip, Malikin, 0, 0

        SendInput, {1}
        Sleep, 1000

        If FindImage("coin_on_mouse") = True
        {
            SendInput, {Shift down}
            Sleep, 100
            SendInput, {LShift}{F12}
            Sleep, 100
            SendInput, {Shift up}
            Sleep, 100
            Sleep, 1000

            SendInput, {2}
            Sleep, 1000
        }
    }

    IfInString, target_name, Ontu
    {
        ;ToolTip, Ontu, 0, 0

        If FindImage("give_coin") = True
        {
            ;If FindImage("give_button_ui") = True
            ;{
                ;ClickImage("give_button")
                ;Sleep, 1000
            ;}

            ;If FindImage("give_button_ui2") = True
            ;{
                ;ClickImage("give_button2")
                ;Sleep, 1000
            ;}

            ClickLocation(60, 390) ; location of the Give button on screen when giving coin
            Sleep, 1000
        }
        Else
        {
            If FindImage("coin_on_mouse") = True
            {
                ClickLocation(332, 192) ; location of Ontu NPC on screen when giving coin
                Sleep, 1000
            }
            Else
            {
                Loop, 5
                {
                    SendInput, {Escape}
                    Sleep, 100
                }
                Sleep, 1000
            }
        }
    }
}
Else
{
    ;ToolTip, No Target, 0, 0

    SendInput, {F12}
    Sleep, 1000

    SendInput, {1}
    Sleep, 1000
}

Return

GuiClose:
GuiEscape:

Memory_CloseHandle(everquest_process_handle)

ExitApp

Return
