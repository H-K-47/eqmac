Global everquest_process_handle := 0
Global everquest_process_id     := 0

Global EVERQUEST_CLIENT := "eqgame.exe"

Global EVERQUEST_TITLE := "EQW beta 2.32"

; eqgame.exe+AAAD1 - 74 0C - je eqgame.exe+AAADF
Global EVERQUEST_ASM_PLAY_NICE_JUMP       := 0x004AAAD1
Global EVERQUEST_ASM_PLAY_NICE_JUMP_BYTES := "74,0C"
Global EVERQUEST_ASM_PLAY_NICE_JUMP_SIZE  := 2

; eqgame.exe+AAAD3 - FF 35 68967900 - push [eqgame.exe+399668]
; eqgame.exe+AAAD9 - FF 15 5C315E00 - call dword ptr [eqgame.exe+1E315C] -> kernel32.Sleep
Global EVERQUEST_ASM_PLAY_NICE_SLEEP := 0x00799668 ; DWORD ; eqgame.exe+399668
