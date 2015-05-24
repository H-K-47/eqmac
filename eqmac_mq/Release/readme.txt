EQMac MacroQuest

Features:
In-Game Map
Mouse Wheel Zooming
Client Switcher
ESP Hack
Speed Hack
Box Chat Server
Buff Timers
Bard Twist
Load/Save Spellsets
Loot All
Hide Corpses Looted
Auto Inventory
Lua Scripting
and more!

Installion:
Extract to your EQMac folder where eqgame.exe is located.
Use eqgame_inject_dll.exe to inject the eqmac_mq.dll into eqgame.exe while the game is running.
A batch file is provided to inject the DLL for you.
The maps folder is needed in order to draw the in-game map.
You can change various options by editing the eqmac_mq.ini configuration file.
You can make a zone specific configuration file by creating a ZONENAME.ini in the zoneconfigs folder. (example: c:\eqmac\zoneconfigs\poknowledge.ini)
You can make a character specific configuration file by creating a CHARACTERNAME.ini file in the charconfigs folder (example: c:\eqmac\charconfigs\Soandso.ini)

Missing DLLs?

Visual C++ Redistributable Packages for Visual Studio 2013
https://www.microsoft.com/en-us/download/details.aspx?id=40784
vcredist_x86.exe
vcredist_x64.exe
vcredist_arm.exe

Command List:

/unload
Unload MacroQuest

/version
Show Build Date

/configreload
Reload config files

/hudbuttons
Toggle the HUD buttons

/mousewheelzoom
Toggle Mouse Wheel Zoom

/mousewheelzoom #
Set Mouse Wheel Zoom speed

/clientswitcher
Toggle the Client Switcher

/clientswitcher #
/clientswitcher 1
/clientswitcher 2
/clientswitcher 3
Switch to client by number

/clientswitchernext
Switch to next client

/clientswitcherprevious
/clientswitcherprev
Switch to previous client

/clientswitcherrefresh
Refresh how many clients are loaded

/clientswitchersyncmouse
Toggle mouse sync

/clientswitcherbuttons
Toggle the Client Switcher hud buttons

/clientswitcherhotkeys
Toggle the Client Switcher hotkeys

TODO...

/bccmd autoconnect OR /bccmd autoreconnect
/bccmd connect OR /bccmd reconnect
/bccmd disconnect
/bccmd status
/bccmd help

/bca
/bcaa
/bct

/map
/mapreload
/mapbuttons
/mapmaximized OR /mapmaximize
/mapmaximized x y width height
/maplayerbase
/maplayer 0
/maplayer 1
/maplayer 2
/maplayer 3
/mapspawnclicktotarget
/maparrow
/mapzoneinfo
/mapheightfilter
/mapheightfilter low high
/mapheightfilterspawns
/maprotate
/mapspawns
/mappoints
/maplines
/maptargetline
/mapyourselfcorpseline
/mapreplaceblacklineswithwhitelines
/mapshowplayer
/mapshowpc
/mapshowplayercorpse
/mapshowpccorpse
/mapshownpc
/mapshownpccorpse
/mapspawndistance
/mapspawndistance #
/mapspawnfilternpc
/mapspawnfilternpc text,text,text,...
/mapzoomresetandcenter
/mapzoomreset
/mapcenter
/mapzoom #
/mapmousewheelzoom
/mapmousewheelzoommultiplier #    OR    /mapmousewheelzoom #
/maparrowradius #                 OR    /maparrow #
/mapcenterlinesize #              OR    /mapcenterline #
/map x y width height

/esp
/espcliptoviewport
/espshowyourself
/espshowplayer
/espshowpc
/espshowplayercorpse
/espshowpccorpse
/espshownpc
/espshownpccorpse
/espshowgroundspawns
/espshowdoorspawns
/espshowuniquedoorspawns
/espshowtext
/espshowtextonmousehover
/espdistance
/espdistance #
/espfilternpc text,text,text,...
/espskeletons
/espskeletonslineofsight OR /espskeletonslos
/espskeletonsdistance
/espskeletonsdistance #
/esptargetline
/esptargetlinedistance #

/healthbars
/healthbarscliptoviewport
/healthbarsdistance
/healthbarsdistance #
/healthbarsclicktotarget
/healthbars width height

/buffinfo
/buffinfo x y

/playerinfo
/playerinfo x y

/targetinfo
/targetinfo x y

/messagetext
/messagetext x y

/buffwindowtimers

/limitcpuusage
/limitcpuusage backgrounddelay foregrounddelay

/speed
/speed #
/speed sow
/speed run1
/speed run2
/speed run3

/skillhack

/twist
/twist # # # # # # # # # #
/twistdelay #

/kite
/kitedistance #

/nametext
/namecolors

/freecamera OR /freecam

/melee
/meleelist
/meleerange
/meleedelay #
/melee taunt
/melee bash
/melee kick
/melee disarm
/melee senseheading

/hacks
/hackslist
/hacks #

/savespellset name OR /savespells name
/loadspellset name OR /loadspells name

/logging

/target myself
/target player
/target pc
/target npc
/target corpse
/target npccorpse
/target playercorpse
/target pccorpse

/targetheight
/targetheight #

/zonegravity
/zonegravity #

/zoneclip #
/zoneminclip #
/zonemaxclip #

/facetarget OR /facetar
/bank
/banklist
/mana
/buffs
/autoinventory OR /autoinv
/drop
/dohotbutton #
/stopcast
/track
/stoptrack
/toggleinspect
/hidecorpses looted OR /hidecorpse looted

/lootall
/lootallnodrop
/lootallfilterid #,#,#,...
/lootallfiltername text,text,text,...

/casting "Spell Name"

/useitem #
0  = Left Ear
1  = Head
2  = Face
3  = Right Ear
4  = Neck
5  = Shoulders
6  = Arms
7  = Back
8  = Left Wrist
9  = Right Wrist
10 = Ranged
11 = Hands
12 = Primary
13 = Secondary
14 = Left Ring
15 = Right Ring
16 = Chest
17 = Legs
18 = Feet
19 = Waist
20 = Ammo
21 = Bag 1
22 = Bag 2
23 = Bag 3
24 = Bag 4
25 = Bag 5
26 = Bag 6
27 = Bag 7
28 = Bag 8

/beep

/warp
/warp y x z
/warptotarget
/warptoname spawnname

/zone zoneshortname

/script scriptname

/scripts
/scriptslist
/scripts #

/lua luastring

/dumpcommands
