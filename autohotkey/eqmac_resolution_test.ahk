#Include, eqmac.ahk

Global WS_CAPTION := 0xC00000

WinWait, %EVERQUEST_TITLE%
IfWinExist
{
    WinActivateBottom, %EVERQUEST_TITLE%

    WinSet, Style, -%WS_CAPTION%

    WinMove, , , 0, 0, 1280, 720

    WinSet, AlwaysOnTop, On
    WinSet, AlwaysOnTop, Off
}
Return
