#SingleInstance, Off
#Persistent

#Include, eqmac.ahk

Global psapi_dll_handle := 0

Global ini_file := "eqmac_options.ini"

Gui, Add, Text,, Process:
Gui, Add, DropDownList, vDropDownListProcesses w320 Choose1, Dynamic||
Gui, Add, Button, vButtonRefreshProcesses gButtonRefreshProcesses Default, Refresh &Processes
Gui, Add, Text,,
Gui, Add, Checkbox, vCheckboxSpeedHack, Speed Hack
Gui, Add, Text,, Speed Multiplier:
Gui, Add, DropDownList, vDropDownListSpeedModifier w320 Choose1, 1.0||1.25|1.5|1.75|2.0|4.0|6.0|8.0|10.0|20.0 ;|100.0
Gui, Add, Checkbox, vCheckboxScripts ym, Scripts
Gui, Add, ListView, vListViewScripts +HwndListViewHwndScripts VScroll Grid -Multi Checked NoSortHdr w640 h480, Enabled|Name|Description|Enable|Disable|

UpdateListViewColumnsScripts()
{
    Gui, ListView, ListViewScripts

    LV_ModifyCol()
    LV_ModifyCol(1, "AutoHdr")
    LV_ModifyCol(2, "AutoHdr")
    LV_ModifyCol(3, "AutoHdr")
    LV_ModifyCol(4, "AutoHdr")
    LV_ModifyCol(5, "AutoHdr")
}

UpdateListViewScripts()
{
    GuiControl, -Redraw, ListViewScripts

    Gui, ListView, ListViewScripts
    LV_Delete()

    FileRead, options_file, %ini_file%

    found_scripts := False

    Loop, Parse, options_file, `n, `r
    {
        ;MsgBox, %A_LoopField%

        If A_LoopField = 
        {
            Continue
        }

        IfInString, A_LoopField, #
        {
            Continue
        }

        IfInString, A_LoopField, //
        {
            Continue
        }

        IfInString, A_LoopField, [Scripts]
        {
            found_scripts := True
            
            Continue
        }

        IfInString, A_LoopField, [/Scripts]
        {
            found_scripts := False
            
            Continue
        }

        If (found_scripts)
        {
            StringSplit, script_data, A_LoopField, :

            script_file_path = %A_ScriptDir%\scripts\%script_data2%.ini

            IfExist, %script_file_path%
            {
                script_is_enabled = %script_data1%

                IniRead, script_name, %script_file_path%, Script, Name

                IniRead, script_description, %script_file_path%, Script, Description

                FileRead, script_file, %script_file_path%

                found_enable  := False
                found_disable := False

                instructions_enable = 
                instructions_disable = 

                Loop, Parse, script_file, `n, `r
                {
                    ;MsgBox, %A_LoopField%

                    If A_LoopField = 
                    {
                        Continue
                    }

                    IfInString, A_LoopField, #
                    {
                        Continue
                    }

                    IfInString, A_LoopField, //
                    {
                        Continue
                    }

                    IfInString, A_LoopField, [Enable]
                    {
                        found_enable  := True
                        found_disable := False

                        Continue
                    }

                    IfInString, A_LoopField, [Disable]
                    {
                        found_enable  := False
                        found_disable := True

                        Continue
                    }

                    If (found_enable)
                    {
                        instructions_enable = %instructions_enable%%A_LoopField%^
                    }

                    If (found_disable)
                    {
                        instructions_disable = %instructions_disable%%A_LoopField%^
                    }
                }

                StringRight, find_text, instructions_enable, 1
                If find_text = ^
                {
                    StringTrimRight, instructions_enable, instructions_enable, 1
                }

                StringRight, find_text, instructions_disable, 1
                If find_text = ^
                {
                    StringTrimRight, instructions_disable, instructions_disable, 1
                }

                LV_Add("Check" . %script_is_enabled%, "", script_name, script_description, instructions_enable, instructions_disable)
            }
        }
    }

    GuiControl, +Redraw, ListViewScripts
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

SetTimer, TimerUpdateProcess, 100

IniRead, scripts, %ini_file%, Options, Scripts
If (scripts > -1)
{
    GuiControl,, CheckboxScripts, %scripts%
}

IniRead, speed_modifier, %ini_file%, SpeedHack, Modifier
If (speed_modifier > -1)
{
    GuiControl, ChooseString, DropDownListSpeedModifier, %speed_modifier%
}

UpdateListViewScripts()
UpdateListViewColumnsScripts()

SetTimer, TimerSpeedHack, 1

SetTimer, TimerScripts, 1

SetTimer, TimerUpdateTitle, 100

Gui, Show, Center, o

Return

TimerSpeedHack:
GuiControlGet, is_checked,, CheckboxSpeedHack
If (is_checked = 0)
{
    Memory_WriteFloat(everquest_process_handle, EVERQUEST_SPEED_HACK_SPEED_MODIFIER, 1.0)

    Return
}

GuiControlGet, speed_modifier,, DropDownListSpeedModifier

Memory_WriteFloat(everquest_process_handle, EVERQUEST_SPEED_HACK_SPEED_MODIFIER, speed_modifier)

Return

TimerScripts:
GuiControlGet, is_checked,, CheckboxScripts
If (is_checked = 0)
{
    Return
}

Gui, ListView, ListViewScripts

Loop % LV_GetCount()
{
    ; http://l.autohotkey.net/docs/commands/ListView.htm#bifRow
    Gui +LastFound
    SendMessage, 4140, A_Index - 1, 0xF000, SysListView321
    is_checked := (ErrorLevel >> 12) - 1

    LV_GetText(script_name,        A_Index, 2) ; Name
    LV_GetText(script_description, A_Index, 3) ; Description

    If (is_checked = 1)
    {
        script_instructions_column := 4 ; Enable
    }
    Else
    {
        script_instructions_column := 5 ; Disable
    }

    LV_GetText(script_instructions, A_Index, script_instructions_column)

    ;MsgBox, row_number = %A_Index%`nis_checked = %is_checked%`nscript_name = %script_name%`nscript_description = %script_description%`nscript_instructions = %script_instructions%

    StringSplit, script_instructions, script_instructions, ^

    Loop, %script_instructions0%
    {
        script_instruction := script_instructions%A_Index%
    
        ;MsgBox, script_instruction = %script_instruction%
    
        StringSplit, script_instruction_data, script_instruction, :

        IfEqual, script_instruction_data1, pointer
        {
            script_instruction_pointer = %script_instruction_data2%

            script_instruction_pointer = 0x%script_instruction_pointer% ; pre-append 0x

            ;MsgBox, script_instruction_pointer = %script_instruction_pointer%

            script_instruction_address := Memory_Read(everquest_process_handle, script_instruction_pointer)

            ;MsgBox, script_instruction_address = %script_instruction_address%

            script_instruction_offsets = %script_instruction_data4%

            num_offsets := 0
            Loop, Parse, script_instruction_offsets, `,
            {
                num_offsets += 1
            }

            ;MsgBox, num_offsets = %num_offsets%

            StringSplit, script_instruction_offsets, script_instruction_offsets, `,

            current_offset := 1
            Loop, %script_instruction_offsets0%
            {
                script_instruction_offset := script_instruction_offsets%A_Index%

                script_instruction_offset = 0x%script_instruction_offset% ; pre-append 0x

                script_instruction_address += script_instruction_offset

                ;MsgBox, script_instruction_address = %script_instruction_address%

                If (current_offset = num_offsets)
                {
                    Break
                }

                script_instruction_address := Memory_Read(everquest_process_handle, script_instruction_address)

                ;MsgBox, script_instruction_address = %script_instruction_address%

                current_offset := current_offset + 1
            }

            script_instruction_type    = %script_instruction_data5%
            script_instruction_value   = %script_instruction_data6%

            ;Break
        }
        Else ; no pointers
        {
            script_instruction_address = %script_instruction_data1%
            script_instruction_type    = %script_instruction_data2%
            script_instruction_value   = %script_instruction_data3%

            script_instruction_address = 0x%script_instruction_address% ; pre-append 0x
        }

        ;MsgBox, script_instruction_address = %script_instruction_address%`nscript_instruction_type = %script_instruction_type%`nscript_instruction_value = %script_instruction_value%

        If script_instruction_type = nops
        {
            Memory_WriteNops(everquest_process_handle, script_instruction_address, script_instruction_value)
        }

        If script_instruction_type = bytes
        {
            Memory_WriteBytes(everquest_process_handle, script_instruction_address, script_instruction_value)
        }

        If script_instruction_type = byte
        {
            Memory_WriteEx(everquest_process_handle, script_instruction_address, script_instruction_value, 1)
        }

        If script_instruction_type = word
        {
            Memory_WriteEx(everquest_process_handle, script_instruction_address, script_instruction_value, 2)
        }

        If script_instruction_type = dword
        {
            Memory_Write(everquest_process_handle, script_instruction_address, script_instruction_value)
        }

        If script_instruction_type = float
        {
            Memory_WriteFloat(everquest_process_handle, script_instruction_address, script_instruction_value)
        }
    }
}

Return

ButtonRefreshProcesses:
GuiControl,, DropDownListProcesses, |

GuiControl,, DropDownListProcesses, Dynamic

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
            GuiControl,, DropDownListProcesses, %psapi_process_id%:%psapi_process_name%
        }
    }
}

GuiControl, ChooseString, DropDownListProcesses, Dynamic
Return

TimerUpdateTitle:
script_process_id := DllCall("GetCurrentProcessId")

player_name := everquest_GetZonePlayerName()

WinSetTitle, ahk_pid %script_process_id%,,%player_name%
Return

TimerUpdateProcess:
GuiControlGet, processes_text,, DropDownListProcesses

IfNotEqual, processes_text, Dynamic
{
    StringSplit, processes_text_vars, processes_text, :

    selected_process_id = %processes_text_vars1%

    everquest_process_id := selected_process_id

    everquest_process_handle := Memory_GetProcessHandle(everquest_process_id)

    Return
}

IfWinNotActive, %EVERQUEST_TITLE%
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

Gui, ListView, ListViewScripts

Loop % LV_GetCount()
{
    LV_Modify(A_Index, "-Check") ; disable all scripts
}

Gosub, TimerScripts

DllCall("FreeLibrary", "Ptr", psapi_dll_handle)

Memory_CloseHandle(everquest_process_handle)

ExitApp

Return
