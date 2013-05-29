#Include, eqmac.ahk

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

SetFormat, IntegerFast, H

everquest_gfx_dll_address := everquest_GetGfxDllAddress()

MsgBox, everquest_gfx_dll_address: %everquest_gfx_dll_address%

everquest_eqgame_dll_address := everquest_GetDllAddress("eqgame.dll")

MsgBox, everquest_eqgame_dll_address: %everquest_eqgame_dll_address%