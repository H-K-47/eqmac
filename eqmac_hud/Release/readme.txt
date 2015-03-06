EQMac HUD

Extract to your EQMac folder where eqgame.exe is located.
Use eqgame_inject_dll.exe to inject the eqmac_hud.dll into eqgame.exe while the game is running.
A batch file is provided to inject the DLL for you.
The maps folder is needed in order to draw the in-game map.
You can change some HUD options by editing the eqmac_hud.ini configuration file.
You can make a zone specific configuration file by creating a ZONENAME.ini in the maps folder. (example: c:\eqmac\maps\poknowledge.ini)
You can make a character specific configuration file by creating a eqmac_hud-CHARACTERNAME.ini file in your EQMac folder (example: c:\eqmac\eqmac_hud-Soandso.ini)

Map
Draws an in-game map.

ESP
Draws the names of spawns through walls.

Buffs
Draws buff names and buff timers in hh:mm::ss format.

Player/Target Info
Draws spawn level, race, class, hp, mana information.

Missing DLLs?

Visual C++ Redistributable Packages for Visual Studio 2013
https://www.microsoft.com/en-us/download/details.aspx?id=40784

vcredist_x86.exe
vcredist_x64.exe
vcredist_arm.exe