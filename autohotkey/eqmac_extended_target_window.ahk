#SingleInstance, Ignore
#Persistent

#Include, eqmac.ahk

Global psapi_dll_handle := 0

SetTitleMatchMode, 2

Gui, Font, s10, Courier New
Gui, Add, Text, ym, Filter:
Gui, Add, Edit, vEditFilter gEditFilter ym w320
Gui, Add, ListView, vListViewExtendedTarget gListViewExtendedTarget +HwndListViewHwndExtendedTarget VScroll Grid -Multi w400 h400 xm, Address|Name|HP|Distance

UpdateListViewColumnsExtendedTarget()
{
    Gui, ListView, ListViewTracking

    LV_ModifyCol()
    LV_ModifyCol(1, "AutoHdr")
    LV_ModifyCol(2, "150")
    LV_ModifyCol(3, "AutoHdr")
    LV_ModifyCol(4, "AutoHdr")

    LV_ModifyCol(1, "Integer") ; Address
    LV_ModifyCol(1, "Left")

    LV_ModifyCol(3, "Integer") ; HP
    LV_ModifyCol(3, "Left")

    LV_ModifyCol(4, "Integer") ; Distance
    LV_ModifyCol(4, "Right")

    LV_ModifyCol(3, "SortDesc") ; Sort by highest HP
}

UpdateListViewExtendedTarget()
{
    Gui, ListView, ListViewExtendedTarget
    LV_Delete()

    GuiControl, -Redraw, ListViewExtendedTarget

    target_spawn_info := everquest_GetTargetSpawnInfoPointer()

    player_spawn_info := everquest_GetPlayerSpawnInfoPointer()

    player_y := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y)
    player_x := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X)
    player_z := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z)

    spawn_info_address := player_spawn_info

    spawn_next_spawn_info := Memory_Read(everquest_process_handle,  spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER)

    spawn_info_address := spawn_next_spawn_info

    spawn_index := 1

    Loop, %EVERQUEST_SPAWNS_MAX%
    {
        spawn_next_spawn_info := Memory_Read(everquest_process_handle,  spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER)

        If (spawn_next_spawn_info = EVERQUEST_SPAWN_INFO_NULL)
        {
            Break
        }

        spawn_type := Memory_ReadEx(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_TYPE, 1)

        If (spawn_type = EVERQUEST_SPAWN_INFO_TYPE_CORPSE)
        {
            spawn_info_address := spawn_next_spawn_info

            Continue
        }

        spawn_name := Memory_ReadString(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NAME, 0x40)

        GuiControlGet, filter_text,, EditFilter

        StringLen, filter_text_length, filter_text

        If (filter_text_length > 0)
        {
            apply_filter := InStr(spawn_name, filter_text)

            If (apply_filter = 0)
            {
                spawn_info_address := spawn_next_spawn_info

                Continue
            }
        }

        spawn_hp_current := Memory_Read(everquest_process_handle,  spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HP_CURRENT)
        ;spawn_hp_max     := Memory_Read(everquest_process_handle,  spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HP_MAX)

        if (spawn_hp_current = 1)
        {
            ;spawn_info_address := spawn_next_spawn_info

            ;Continue
        }

        spawn_y := Memory_ReadFloat(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Y)
        spawn_x := Memory_ReadFloat(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_X)
        spawn_z := Memory_ReadFloat(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Z)

        spawn_distance := CalculateDistance(player_x, player_y, spawn_x, spawn_y)

        if (spawn_distance > 256)
        {
            ;spawn_info_address := spawn_next_spawn_info

            ;Continue
        }

        StringTrimRight, spawn_distance, spawn_distance, 7

        LV_Add("", spawn_info_address, spawn_name, spawn_hp_current, spawn_distance)

        If (target_spawn_info = spawn_info_address)
        {
            LV_Modify(LV_GetCount(), "Select")
        }

        spawn_info_address := spawn_next_spawn_info

        spawn_index++
    }

    UpdateListViewColumnsExtendedTarget()

    GuiControl, +Redraw, ListViewExtendedTarget
}

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

UpdateListViewExtendedTarget()
UpdateListViewColumnsExtendedTarget()

GuiControl, Focus, EditFilter

Gui, +AlwaysOnTop
Gui, Show, AutoSize Center, Extended Targets

SetTimer, TimerExtendedTarget, 1000

SetTimer, TimerUpdateProcess, 100

Return

~f::
Gui, ListView, ListViewExtendedTarget
LV_GetText(extended_target_address, 1, 1) ; first row, first column

If (extended_target_address != EVERQUEST_SPAWN_INFO_NULL)
{
    Memory_Write(everquest_process_handle, EVERQUEST_TARGET_SPAWN_INFO_POINTER, extended_target_address)
}
Return

TimerExtendedTarget:

UpdateListViewExtendedTarget()

Return

EditFilter:
Return

ListViewExtendedTarget:
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
