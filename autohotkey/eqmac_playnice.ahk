#Persistent

#Include, eqmac.ahk

Global ini_file := "eqmac_playnice.ini"

Global sleep_time_foreground := 33

Global sleep_time_background := 100

EnableDebugPrivileges()

IniRead, sleep_time_foreground, %ini_file%, Options, SleepTimeForeground
IniRead, sleep_time_background, %ini_file%, Options, SleepTimeBackground

SetTimer, TimerPlayNice, 1

Return

PlayNice_Enable()
{
    WinGet, active_process_id, PID, A

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

        If (psapi_result && psapi_process_name)
        {
            If psapi_process_name = %EVERQUEST_CLIENT%
            {
                everquest_process_id := psapi_process_id

                everquest_process_handle := Memory_GetProcessHandle(everquest_process_id)

                 Memory_WriteNops(everquest_process_handle, EVERQUEST_ASM_PLAY_NICE_JUMP, EVERQUEST_ASM_PLAY_NICE_JUMP_SIZE)

                If (psapi_process_id = active_process_id)
                {
                    Memory_Write(everquest_process_handle, EVERQUEST_ASM_PLAY_NICE_SLEEP, sleep_time_foreground)
                }
                Else
                {
                    Memory_Write(everquest_process_handle, EVERQUEST_ASM_PLAY_NICE_SLEEP, sleep_time_background)
                }
            }
        }

        DllCall("CloseHandle", "Ptr", psapi_process_handle)
    }
}

PlayNice_Disable()
{
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

        If (psapi_result && psapi_process_name)
        {
            If psapi_process_name = %EVERQUEST_CLIENT%
            {
                everquest_process_id := psapi_process_id

                everquest_process_handle := Memory_GetProcessHandle(everquest_process_id)

                Memory_WriteBytes(everquest_process_handle, EVERQUEST_ASM_PLAY_NICE_JUMP, EVERQUEST_ASM_PLAY_NICE_JUMP_BYTES)
                Memory_Write(everquest_process_handle, EVERQUEST_ASM_PLAY_NICE_SLEEP, 30)
            }
        }

        DllCall("CloseHandle", "Ptr", psapi_process_handle)
    }
}

TimerPlayNice:
PlayNice_Enable()
Return

OnExit:

SetTimer, TimerPlayNice, Off

Sleep, 1000

PlayNice_Disable()

DllCall("FreeLibrary", "Ptr", psapi_dll_handle)

Memory_CloseHandle(everquest_process_handle)

ExitApp

Return
