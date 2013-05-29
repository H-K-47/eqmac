#Persistent

#Include, eqmac.ahk

Global psapi_dll_handle := 0

Global ini_file := "eqmac_multiclient.ini"

Global WS_CAPTION := 0xC00000

Global PROCESS_ALL_ACCESS := 0x001F0FFF

Global cpu_core_index := 2

Global resolution_width  := 1680 ; 1920 ; 1280
Global resolution_height := 1050 ; 1080 ; 720

Global clients := Array()

EnableDebugPrivileges()

psapi_dll_handle := DllCall("LoadLibrary", "Str", "Psapi.dll")

IniRead, resolution_width,  %ini_file%, Resolution, Width
IniRead, resolution_height, %ini_file%, Resolution, Height

;IniWrite, TRUE, eqclient.ini, Defaults, WindowedMode
;IniWrite, %resolution_width%,  eqclient.ini, VideoMode, Width
;IniWrite, %resolution_height%, eqclient.ini, VideoMode, Height

GroupAdd, everquest, %EVERQUEST_TITLE%

Hotkey, IfWinActive, %EVERQUEST_TITLE%
Hotkey, XButton1, do_multi
Hotkey, XButton2, do_sit
Hotkey, MButton,  do_heal
Hotkey, F11, do_text

Return

; fix alt bug
~Up::
~Down::
~Left::
~Right::
~w::
~a::
~s::
~d::
SendInput, {Alt up}
Return

; press tilde to switch between client windows (EQWindows)
`::
GroupActivate, everquest
WinSet, AlwaysOnTop, On
WinSet, AlwaysOnTop, Off
SendInput, {Alt down}
SendInput, {Alt up}
Return

do_multi:
SendInput, ^0
Sleep, 100
Return

do_sit:
SendInput, ^5
Sleep, 100
Return

do_heal:
SendInput, ^8
Sleep, 100
Return

do_text:
SendInput, azxcvbnm1
Return

Numpad0::
NumpadIns::
GetClients()
Return

Numpad1::
NumpadEnd::
activate_client := clients[1]
WinActivate, ahk_pid %activate_client%
Return

Numpad2::
NumpadDown::
activate_client := clients[2]
WinActivate, ahk_pid %activate_client%
Return

Numpad3::
NumpadPgDn::
activate_client := clients[3]
WinActivate, ahk_pid %activate_client%
Return

Numpad4::
NumpadLeft::
activate_client := clients[4]
WinActivate, ahk_pid %activate_client%
Return

Numpad5::
NumpadClear::
activate_client := clients[5]
WinActivate, ahk_pid %activate_client%
Return

Numpad6::
NumpadRight::
activate_client := clients[6]
WinActivate, ahk_pid %activate_client%
Return

; press control+tilde for borderless fullscreen windowed mode
^`::
WinWait, %EVERQUEST_TITLE%
IfWinExist
{
    WinSet, Style, -%WS_CAPTION%
    WinMove, , , 0, 0, %resolution_width%, %resolution_height%
    WinSet, AlwaysOnTop, On
    WinSet, AlwaysOnTop, Off
}
Return

; press start+tilde to launch a new instance of the client
;#`::
;Run, C:\EQMac\eqw.exe
;Return

; press start+tilde to set each client's process affinity to a separate cpu core
#`::
SetProcessAffinityMask()
Return

SetProcessAffinityMask()
{
    psapi_size := 4096

    psapi_size := VarSetCapacity(psapi_processes, psapi_size)
    DllCall("Psapi.dll\EnumProcesses", "Ptr", &psapi_processes, "UInt", psapi_size, "UIntP", psapi_process)

    Loop, % psapi_process // 4
    {
        psapi_process_id := NumGet(psapi_processes, A_Index * 4, "UInt")

        psapi_process_handle := DllCall("OpenProcess", "UInt", 0x001F0FFF, "Int", false, "UInt", psapi_process_id, "Ptr")

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

        If (psapi_result && psapi_process_name)
        {
            If psapi_process_name = %EVERQUEST_CLIENT%
            {
                If (psapi_process_id = active_process_id)
                {
                    DllCall("SetProcessAffinityMask", "Ptr", psapi_process_handle, "Int", 1)
                }
                Else
                {
                    if (cpu_core_index > 8)
                    {
                        cpu_core_index := 2
                    }

                    DllCall("SetProcessAffinityMask", "Ptr", psapi_process_handle, "Int", cpu_core_index)

                    cpu_core_index := cpu_core_index * 2
                }
            }
        }

        DllCall("CloseHandle", "Ptr", psapi_process_handle)
    }
}

GetClients()
{
    clients = 

    clients := Array()

    psapi_size := 4096

    psapi_size := VarSetCapacity(psapi_processes, psapi_size)
    DllCall("Psapi.dll\EnumProcesses", "Ptr", &psapi_processes, "UInt", psapi_size, "UIntP", psapi_process)

    Loop, % psapi_process // 4
    {
        psapi_process_id := NumGet(psapi_processes, A_Index * 4, "UInt")

        psapi_process_handle := DllCall("OpenProcess", "UInt", 0x001F0FFF, "Int", false, "UInt", psapi_process_id, "Ptr")

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

        If (psapi_result && psapi_process_name)
        {
            If psapi_process_name = %EVERQUEST_CLIENT%
            {
                clients.Insert(psapi_process_id)
            }
        }

        DllCall("CloseHandle", "Ptr", psapi_process_handle)
    }
}

GuiClose:
GuiEscape:

DllCall("FreeLibrary", "Ptr", psapi_dll_handle)

ExitApp

Return
