#ifndef EQMAC_HPP
#define EQMAC_HPP

#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>

#include <windows.h>

#include "cmemory.hpp"

const std::string EQ_TITLE = "EverQuest";
const std::string EQ_TITLE_EQW = "EQW beta 2.32";

#define EQ_IS_IN_GAME             0x00798550 // BYTE
#define EQ_IS_AUTO_ATTACK_ENABLED 0x007F6FFE // BYTE

#define EQ_POINTER_GRAPHICS_DLL 0x007F9C50 // EQGfx_Dx8.DLL base address

#define EQ_POINTER_StringTable 0x007F9490 // eqstr_xx.txt

#define EQ_POINTER_FONT_ARIAL14 0x0063D3B0

#define EQ_POINTER_CEverQuest 0x00809478
#define EQ_OFFSET_CEverQuest_GAME_STATE 0x5AC // DWORD

#define EQ_GAME_STATE_CHARACTER_SELECT 1
#define EQ_GAME_STATE_INGAME           5

#define EQ_POINTER_CDisplay 0x007F9510
#define EQ_OFFSET_CDisplay_IS_CURSOR_ITEM   0x040 // BYTE ; when you pick up and hold an item on your mouse cursor
#define EQ_OFFSET_CDisplay_IS_CURSOR_HOTKEY 0x042 // BYTE ; when you pick up and hold a hotkey button on your mouse cursor
#define EQ_OFFSET_CDisplay_TIMER            0x0C8 // DWORD ; global timer in milliseconds, 1000 milliseconds = 1 second

#define EQ_POINTER_CHotButtonWnd 0x0063D628

#define EQ_POINTER_CLootWnd 0x0063D65C
#define EQ_OFFSET_CLootWnd_IS_OPEN       0x134 // BYTE
#define EQ_OFFSET_CLootWnd_POINTER_ITEM1 0x1B8 // pointer to first item

#define EQ_CLootWnd_POINTER_ITEM_OFFSET 0x04 // offset between each item pointer

#define EQ_CLootWnd_ITEMS_MAX 30

#define EQ_POINTER_CTradeWnd 0x0063D668

#define EQ_POINTER_CGiveWnd 0x0063D678

#define EQ_OFFSET_ITEM_INFO_NAME             0x000 // STRING [0x40]
#define EQ_OFFSET_ITEM_INFO_LORE_NAME        0x040 // STRING [0x50]
#define EQ_OFFSET_ITEM_INFO_ID_FILE          0x090 // STRING [0x06]
#define EQ_OFFSET_ITEM_INFO_WEIGHT           0x0AE // BYTE ; multiply by 0.1 for actual decimal weight
#define EQ_OFFSET_ITEM_INFO_IS_NO_RENT       0x0AF // BYTE ; 255 = False
#define EQ_OFFSET_ITEM_INFO_IS_NO_DROP       0x0B0 // BYTE ; 255 = False
#define EQ_OFFSET_ITEM_INFO_SIZE             0x0B1 // BYTE ; EQ_ITEM_SIZE_x
#define EQ_OFFSET_ITEM_INFO_IS_CONTAINER     0x0B2 // BYTE ; 1 = True
#define EQ_OFFSET_ITEM_INFO_ID               0x0B4 // WORD
#define EQ_OFFSET_ITEM_INFO_ICON             0x0B6 // WORD
#define EQ_OFFSET_ITEM_INFO_EQUIP_SLOT       0x0B8 // DWORD
#define EQ_OFFSET_ITEM_INFO_EQUIPPABLE_SLOTS 0x0BC // DWORD
#define EQ_OFFSET_ITEM_INFO_COST             0x0C0 // DWORD
#define EQ_OFFSET_ITEM_INFO_IS_STACKABLE     0x0F6 // BYTE ; can have quantity more than 1
#define EQ_OFFSET_ITEM_INFO_QUANTITY         0x116 // BYTE ; count, amount

#define EQ_ITEM_INFO_NAME_SIZE      0x40
#define EQ_ITEM_INFO_LORE_NAME_SIZE 0x50
#define EQ_ITEM_INFO_ID_FILE_SIZE   0x06

#define EQ_ITEM_INFO_NULL 0x00000000

#define EQ_ITEM_SIZE_TINY   0
#define EQ_ITEM_SIZE_SMALL  1
#define EQ_ITEM_SIZE_MEDIUM 2
#define EQ_ITEM_SIZE_LARGE  3
#define EQ_ITEM_SIZE_GIANT  4

#define EQ_ZONE_ID 0x007B9648 // DWORD

#define EQ_ZONE_ID_FREPORTW       9   // West Freeport
#define EQ_ZONE_ID_COMMONS        21  // West Commonlands
#define EQ_ZONE_ID_ECOMMONS       22  // East Commonlands
#define EQ_ZONE_ID_CSHOME         26  // Sunset Home
#define EQ_ZONE_ID_FIELDOFBONE    78  // The Field of Bone
#define EQ_ZONE_ID_SLEEPER        128 // Sleeper's Tomb
#define EQ_ZONE_ID_BAZAAR         151 // The Bazaar
#define EQ_ZONE_ID_NEXUS          152 // The Nexus
#define EQ_ZONE_ID_SSERU          159 // Sanctus Seru
#define EQ_ZONE_ID_TUTORIAL       183 // Tutorial Zone
#define EQ_ZONE_ID_LOAD           184 // Loading
#define EQ_ZONE_ID_LOAD2          185 // Loading
#define EQ_ZONE_ID_POKNOWLEDGE    202 // The Plane of Knowledge
#define EQ_ZONE_ID_POTRANQUILITY  203 // Plane of Tranquility
#define EQ_ZONE_ID_POTIMEA        219 // Plane of Time
#define EQ_ZONE_ID_POTIMEB        223 // Plane of Time

#define EQ_STRUCTURE_ZONE_INFO 0x00798784 // STRUCT
#define EQ_OFFSET_ZONE_INFO_PLAYER_NAME  0x0000 // STRING [0x40]
#define EQ_OFFSET_ZONE_INFO_SHORT_NAME   0x0040 // STRING [0x20]
#define EQ_OFFSET_ZONE_INFO_LONG_NAME    0x0060 // STRING [0x80]
#define EQ_OFFSET_ZONE_INFO_SAFE_POINT_Y 0x01E8 // FLOAT
#define EQ_OFFSET_ZONE_INFO_SAFE_POINT_X 0x01EC // FLOAT
#define EQ_OFFSET_ZONE_INFO_SAFE_POINT_Z 0x01F0 // FLOAT

#define EQ_ZONE_INFO_PLAYER_NAME_SIZE 0x40
#define EQ_ZONE_INFO_SHORT_NAME_SIZE  0x20
#define EQ_ZONE_INFO_LONG_NAME_SIZE   0x80

#define EQ_POINTER_WORLD_INFO 0x007F9494
#define EQ_OFFSET_WORLD_INFO_HOUR   0x04 // BYTE
#define EQ_OFFSET_WORLD_INFO_MINUTE 0x05 // BYTE
#define EQ_OFFSET_WORLD_INFO_DAY    0x06 // BYTE
#define EQ_OFFSET_WORLD_INFO_MONTH  0x07 // BYTE
#define EQ_OFFSET_WORLD_INFO_YEAR   0x08 // BYTE

#define EQ_POINTER_GROUND_SPAWN_INFO 0x007F949C
#define EQ_OFFSET_GROUND_SPAWN_INFO_PREV_SPAWN_INFO_POINTER 0x00 // DWORD
#define EQ_OFFSET_GROUND_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER 0x04 // DWORD
#define EQ_OFFSET_GROUND_SPAWN_INFO_ID                      0x08 // WORD
#define EQ_OFFSET_GROUND_SPAWN_INFO_DROP_ID                 0x0C // WORD
#define EQ_OFFSET_GROUND_SPAWN_INFO_Z                       0x90 // FLOAT
#define EQ_OFFSET_GROUND_SPAWN_INFO_X                       0x94 // FLOAT
#define EQ_OFFSET_GROUND_SPAWN_INFO_Y                       0x98 // FLOAT
#define EQ_OFFSET_GROUND_SPAWN_INFO_NAME                    0x9C // STRING [0x40]

#define EQ_GROUND_SPAWN_INFO_NAME_SIZE 0x40

// ??
//#define EQ_DOORS_INFO_POINTER 0x007F94B8

// class EQ_Character
#define EQ_POINTER_CHAR_INFO 0x007F94E8
#define EQ_POINTER_EQ_Character EQ_POINTER_CHAR_INFO
#define EQ_OFFSET_CHAR_INFO_NAME               0x0002 // STRING [0x40]
#define EQ_OFFSET_CHAR_INFO_MANA               0x009A // WORD
#define EQ_OFFSET_CHAR_INFO_STUNNED_STATE      0x009E // BYTE ; EQ_STUNNED_STATE_x
#define EQ_OFFSET_CHAR_INFO_BUFF1              0x0264 // STRUCT ; first buff ; 15 buffs max
#define EQ_OFFSET_CHAR_INFO_STANDING_STATE     0x0B64 // BYTE ; EQ_STANDING_STATE_x
#define EQ_OFFSET_CHAR_INFO_PLATINUM           0x0B68 // DWORD
#define EQ_OFFSET_CHAR_INFO_GOLD               0x0B6C // DWORD
#define EQ_OFFSET_CHAR_INFO_SILVER             0x0B70 // DWORD
#define EQ_OFFSET_CHAR_INFO_COPPER             0x0B74 // DWORD
#define EQ_OFFSET_CHAR_INFO_BANK_PLATINUM      0x0B78 // DWORD
#define EQ_OFFSET_CHAR_INFO_BANK_GOLD          0x0B7C // DWORD
#define EQ_OFFSET_CHAR_INFO_BANK_SILVER        0x0B80 // DWORD
#define EQ_OFFSET_CHAR_INFO_BANK_COPPER        0x0B84 // DWORD
#define EQ_OFFSET_CHAR_INFO_SPAWN_INFO_POINTER 0x0D74 // DWORD

#define EQ_CHAR_INFO_NAME_SIZE 0x40

#define EQ_BUFF_SIZE 0x0A // size of buff struct
#define EQ_OFFSET_BUFF_UNKNOWN0000       0x00 // BYTE
#define EQ_OFFSET_BUFF_CASTER_LEVEL      0x01 // BYTE ; level of player who casted the buff
#define EQ_OFFSET_BUFF_MODIFIER          0x02 // BYTE ; divide by 10 to get Bard song modifier
#define EQ_OFFSET_BUFF_UNKNOWN0003       0x03 // BYTE ; damage shield = -1
#define EQ_OFFSET_BUFF_SPELL_ID          0x04 // WORD
#define EQ_OFFSET_BUFF_TICKS             0x06 // DWORD ; duration in ticks ; seconds = ticks * 3

#define EQ_BUFFS_MAX 15

#define EQ_BUFF_SPELL_ID_NULL 0xFFFF // WORD

#define EQ_BUFF_TYPE_DETRIMENTAL           0
#define EQ_BUFF_TYPE_BENEFICIAL            1
#define EQ_BUFF_TYPE_BENEFICIAL_GROUP_ONLY 2

#define EQ_POINTER_SPAWNS_BEGIN 0x007F9498

#define EQ_POINTER_GROUND_SPAWNS_BEGIN 0x007F949C

// class EQPlayer
#define EQ_POINTER_PLAYER_SPAWN_INFO     0x007F94CC
#define EQ_POINTER_TARGET_SPAWN_INFO     0x007F94EC
#define EQ_POINTER_CONTROLLED_SPAWN_INFO 0x007F94E0

// targetted spawns
#define EQ_POINTER_TRADE_SPAWN_INFO      0x007F94C8
#define EQ_POINTER_MERCHANT_SPAWN_INFO   0x007F94F8
#define EQ_POINTER_BANKER_SPAWN_INFO     0x007F94FC
#define EQ_POINTER_CORPSE_SPAWN_INFO     0x007F9500
#define EQ_POINTER_GAMEMASTER_SPAWN_INFO 0x007F9504

#define EQ_SPAWN_INFO_NULL 0x00000000

#define EQ_OFFSET_SPAWN_INFO_NAME                    0x0001 // STRING [0x1E]
#define EQ_OFFSET_SPAWN_INFO_ZONE_ID                 0x0044 // DWORD
#define EQ_OFFSET_SPAWN_INFO_Y                       0x0048 // FLOAT
#define EQ_OFFSET_SPAWN_INFO_X                       0x004C // FLOAT
#define EQ_OFFSET_SPAWN_INFO_Z                       0x0050 // FLOAT
#define EQ_OFFSET_SPAWN_INFO_HEADING                 0x0054 // FLOAT ; facing direction, rotation
#define EQ_OFFSET_SPAWN_INFO_MOVEMENT_SPEED          0x005C // FLOAT
#define EQ_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER 0x0078
#define EQ_OFFSET_SPAWN_INFO_PREV_SPAWN_INFO_POINTER 0x007C
#define EQ_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER      0x0084
#define EQ_OFFSET_SPAWN_INFO_CHAR_INFO_POINTER       0x0088
#define EQ_OFFSET_SPAWN_INFO_CAMERA_HEIGHT           0x008C // FLOAT ; height from where your character's eyes see
#define EQ_OFFSET_SPAWN_INFO_MODEL_HEIGHT_OFFSET     0x0090 // FLOAT ; height above/below the ground
#define EQ_OFFSET_SPAWN_INFO_SPAWN_ID                0x0094 // WORD
#define EQ_OFFSET_SPAWN_INFO_PET_OWNER_SPAWN_ID      0x0096 // WORD
#define EQ_OFFSET_SPAWN_INFO_HP_MAX                  0x0098 // DWORD
#define EQ_OFFSET_SPAWN_INFO_HP_CURRENT              0x009C // DWORD
#define EQ_OFFSET_SPAWN_INFO_GUILD_ID                0x00A0 // WORD
#define EQ_OFFSET_SPAWN_INFO_TYPE                    0x00A8 // BYTE ; EQ_SPAWN_TYPE_x
#define EQ_OFFSET_SPAWN_INFO_CLASS                   0x00A9 // BYTE ; EQ_CLASS_x
#define EQ_OFFSET_SPAWN_INFO_RACE                    0x00AA // WORD ; EQ_RACE_x
#define EQ_OFFSET_SPAWN_INFO_LEVEL                   0x00AD // BYTE
#define EQ_OFFSET_SPAWN_INFO_HIDE_STATE              0x00AE // BYTE
#define EQ_OFFSET_SPAWN_INFO_STANDING_STATE          0x00B1 // BYTE ; EQ_STANDING_STATE_x
#define EQ_OFFSET_SPAWN_INFO_PRIMARY                 0x00C2 // WORD
#define EQ_OFFSET_SPAWN_INFO_SECONDARY               0x00C4 // WORD ; offhand
#define EQ_OFFSET_SPAWN_INFO_MODEL_HEIGHT            0x00F0 // FLOAT ; height / size / actor scale factor
#define EQ_OFFSET_SPAWN_INFO_IS_LINKDEAD             0x0108 // BYTE
#define EQ_OFFSET_SPAWN_INFO_IS_AFK                  0x011C // BYTE
#define EQ_OFFSET_SPAWN_INFO_LAST_NAME               0x012C // STRING [0x20]

#define EQ_SPAWNS_MAX 8192

#define EQ_SPAWN_INFO_NAME_SIZE      0x1E
#define EQ_SPAWN_INFO_LAST_NAME_SIZE 0x20

#define EQ_SPAWN_TYPE_PLAYER        0
#define EQ_SPAWN_TYPE_NPC           1
#define EQ_SPAWN_TYPE_NPC_CORPSE    2
#define EQ_SPAWN_TYPE_PLAYER_CORPSE 3

#define EQ_SPAWN_TARGET_TYPE_CAN_TARGET    0x00
#define EQ_SPAWN_TARGET_TYPE_CAN_TARGET2   0x01
#define EQ_SPAWN_TARGET_TYPE_CANNOT_TARGET 0x41

#define EQ_OFFSET_ACTOR_INFO_TIME_STAMP                  0x0058 // DWORD
#define EQ_OFFSET_ACTOR_INFO_TIME_STAMP_LAST_TICK        0x005C // DWORD
#define EQ_OFFSET_ACTOR_INFO_HEIGHT                      0x00B8 // FLOAT
#define EQ_OFFSET_ACTOR_INFO_CASTING_SPELL_TIME_STAMP    0x00CC // DWORD
#define EQ_OFFSET_ACTOR_INFO_CASTING_SPELL_TIME_STAMP_EX 0x00E0 // DWORD
#define EQ_OFFSET_ACTOR_INFO_ANIMATION                   0x0184 // BYTE
#define EQ_OFFSET_ACTOR_INFO_IS_HOLDING_BOTH             0x0260 // DWORD
#define EQ_OFFSET_ACTOR_INFO_IS_HOLDING_SECONDARY        0x0264 // DWORD
#define EQ_OFFSET_ACTOR_INFO_IS_HOLDING_PRIMARY          0x0268 // DWORD
#define EQ_OFFSET_ACTOR_INFO_CASTING_SPELL_ID            0x027C // WORD
#define EQ_OFFSET_ACTOR_INFO_CASTING_SPELL_GEM_NUMBER    0x027E // BYTE
#define EQ_OFFSET_ACTOR_INFO_IS_NOT_MOVING               0x032D // BYTE
#define EQ_OFFSET_ACTOR_INFO_IS_LFG                      0x0438 // BYTE

#define EQ_CASTING_SPELL_ID_NULL 0xFFFF

#define EQ_CASTING_SPELL_GEM_NUMBER_SINGING 255

// class EQ_Main
#define EQ_POINTER_OLD_UI_INFO 0x007F9574
#define EQ_POINTER_EQ_Main EQ_POINTER_OLD_UI_INFO
#define EQ_OFFSET_OLD_UI_INFO_STATE               0x00E88 // BYTE ; EQ_OLD_UI_STATE_x
#define EQ_OFFSET_OLD_UI_INFO_IS_SIDE_WINDOW_OPEN 0x5F314 // DWORD ; trade window, loot window, bank window, merchant window

#define EQ_MOUSE_X_EX 0x008092E8 // DWORD
#define EQ_MOUSE_Y_EX 0x008092EC // DWORD

#define EQ_MOUSE_X 0x00798580 // WORD
#define EQ_MOUSE_Y 0x00798582 // WORD

#define EQ_MOUSE_CLICK_STATE 0x00798614 // DWORD

#define EQ_MOUSE_CLICK_STATE_LEFT  1       //0x01000001
#define EQ_MOUSE_CLICK_STATE_RIGHT 1677612 //0x00010100
#define EQ_MOUSE_CLICK_STATE_BOTH  0x01010101

#define EQ_MOUSE_BUTTON_IS_HELD_DOWN 0x00798628 // DWORD

#define EQ_MOUSE_LOOK_STATE 0x007985EA // DWORD

#define EQ_MOUSE_LOOK_STATE_FALSE 0x00010000
#define EQ_MOUSE_LOOK_STATE_TRUE  0x00010001

#define EQ_RACE_UNKNOWN   0
#define EQ_RACE_HUMAN     1
#define EQ_RACE_BARBARIAN 2
#define EQ_RACE_ERUDITE   3
#define EQ_RACE_WOOD_ELF  4
#define EQ_RACE_HIGH_ELF  5
#define EQ_RACE_DARK_ELF  6
#define EQ_RACE_HALF_ELF  7
#define EQ_RACE_DWARF     8
#define EQ_RACE_TROLL     9
#define EQ_RACE_OGRE      10
#define EQ_RACE_HALFLING  11
#define EQ_RACE_GNOME     12
#define EQ_RACE_IKSAR     128
#define EQ_RACE_VAH_SHIR  130
#define EQ_RACE_FROGLOK   330

#define EQ_CLASS_UNKNOWN                  0
#define EQ_CLASS_WARRIOR                  1
#define EQ_CLASS_CLERIC                   2
#define EQ_CLASS_PALADIN                  3
#define EQ_CLASS_RANGER                   4
#define EQ_CLASS_SHADOWKNIGHT             5
#define EQ_CLASS_DRUID                    6
#define EQ_CLASS_MONK                     7
#define EQ_CLASS_BARD                     8
#define EQ_CLASS_ROGUE                    9
#define EQ_CLASS_SHAMAN                   10
#define EQ_CLASS_NECROMANCER              11
#define EQ_CLASS_WIZARD                   12
#define EQ_CLASS_MAGICIAN                 13
#define EQ_CLASS_ENCHANTER                14
#define EQ_CLASS_BEASTLORD                15
#define EQ_CLASS_BANKER                   16
#define EQ_CLASS_WARRIOR_GUILDMASTER      17 // EQ_CLASS_WARRIOR + 16
#define EQ_CLASS_CLERIC_GUILDMASTER       18
#define EQ_CLASS_PALADIN_GUILDMASTER      19
#define EQ_CLASS_RANGER_GUILDMASTER       20
#define EQ_CLASS_SHADOWKNIGHT_GUILDMASTER 21
#define EQ_CLASS_DRUID_GUILDMASTER        22
#define EQ_CLASS_MONK_GUILDMASTER         23
#define EQ_CLASS_BARD_GUILDMASTER         24
#define EQ_CLASS_ROGUE_GUILDMASTER        25
#define EQ_CLASS_SHAMAN_GUILDMASTER       26
#define EQ_CLASS_NECROMANCER_GUILDMASTER  27
#define EQ_CLASS_WIZARD_GUILDMASTER       28
#define EQ_CLASS_MAGICIAN_GUILDMASTER     29
#define EQ_CLASS_ENCHANTER_GUILDMASTER    30
#define EQ_CLASS_BEASTLORD_GUILDMASTER    31 // EQ_CLASS_BEASTLORD + 16
#define EQ_CLASS_MERCHANT                 32
#define EQ_CLASS_GUILDMASTER_BEGIN        17
#define EQ_CLASS_GUILDMASTER_END          31

#define EQ_OFFSET_CLASS_GUILDMASTER 16 // EQ_CLASS_x + 16 = EQ_CLASS_x_GUILDMASTER

#define EQ_STRUCTURE_GUILD_LIST 0x007F9C94
#define EQ_GUILDS_MAX 512

#define EQ_GUILD_ID_NULL 0xFFFF // WORD

#define EQ_GENDER_MALE   0x00
#define EQ_GENDER_FEMALE 0x01

#define EQ_DEITY_BERTOXXULOUS  0x00C9
#define EQ_DEITY_BRELL_SERILIS 0x00CA
#define EQ_DEITY_CAZIC_THULE   0x00CB
#define EQ_DEITY_INNORUK       0x00CE
#define EQ_DEITY_KARANA        0x00CF
#define EQ_DEITY_QUELLIOUS     0x00D2
#define EQ_DEITY_RALLOS_ZEK    0x00D3
#define EQ_DEITY_THE_TRIBUNAL  0x00D6
#define EQ_DEITY_TUNARE        0x00D7
#define EQ_DEITY_AGNOSTIC      0x018C

// EQPlayer::ChangePosition(BYTE standingState)
#define EQ_STANDING_STATE_STANDING 0x64
#define EQ_STANDING_STATE_FROZEN   0x66 // stunned / mesmerized / feared ; You lose control of yourself!
#define EQ_STANDING_STATE_LOOTING  0x69 // looting or binding wounds
#define EQ_STANDING_STATE_SITTING  0x6E
#define EQ_STANDING_STATE_DUCKING  0x6F // crouching
#define EQ_STANDING_STATE_FEIGNED  0x73 // feign death
#define EQ_STANDING_STATE_DEAD     0x78

#define EQ_STUNNED_STATE_FALSE       0x00
#define EQ_STUNNED_STATE_TRUE        0x01
#define EQ_STUNNED_STATE_UNCONSCIOUS 0x03
#define EQ_STUNNED_STATE_DEAD        0x04

#define EQ_LEVITATION_STATE_FALSE 0x00
#define EQ_LEVITATION_STATE_TRUE  0x02
#define EQ_LEVITATION_STATE_NPC   0x03

#define EQ_ANONYMOUS_STATE_FALSE    0x00
#define EQ_ANONYMOUS_STATE_TRUE     0x01
#define EQ_ANONYMOUS_STATE_ROLEPLAY 0x02

#define EQ_HEADING_MAX 512 // yaw

#define EQ_SKILL_ONE_HAND_BLUNT 0
#define EQ_SKILL_TRACKING       53

#define EQ_TEXT_COLOR_BLACK       0x00
#define EQ_TEXT_COLOR_GRAY        0x01
#define EQ_TEXT_COLOR_GREEN       0x02
#define EQ_TEXT_COLOR_GRAY2       0x03
#define EQ_TEXT_COLOR_BLUE        0x04
#define EQ_TEXT_COLOR_PINK        0x05
#define EQ_TEXT_COLOR_GRAY3       0x06
#define EQ_TEXT_COLOR_WHITE       0x07
#define EQ_TEXT_COLOR_GRAY4       0x08
#define EQ_TEXT_COLOR_GRAY5       0x09
#define EQ_TEXT_COLOR_WHITE2      0x0A
#define EQ_TEXT_COLOR_GRAY6       0x0B
#define EQ_TEXT_COLOR_LIGHT_GRAY  0x0C
#define EQ_TEXT_COLOR_RED         0x0D
#define EQ_TEXT_COLOR_LIGHT_GREEN 0x0E
#define EQ_TEXT_COLOR_YELLOW      0x0F
#define EQ_TEXT_COLOR_LIGHT_BLUE  0x10
#define EQ_TEXT_COLOR_GRAY7       0x11
#define EQ_TEXT_COLOR_CYAN        0x12
#define EQ_TEXT_COLOR_GRAY8       0x13
#define EQ_TEXT_COLOR_BLACK2      0x14

#define EQ_WORLD_SPACE_TO_SCREEN_SPACE_FUNCTION_NAME "t3dWorldSpaceToScreenSpace" // EQGfx_Dx8.t3dWorldSpaceToScreenSpace

#define EQ_POINTER_WORLD_SPACE_TO_SCREEN_SPACE_CAMERA_DATA 0x0063B924 // pass this to function as argument1

#define EQ_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE 0xFFFF3D3E // world space to screen space failed because the location is not on screen

/****************************************************************************************************/

typedef struct _EQLOCATION
{
    float Y;
    float X;
    float Z;
} EQLOCATION, *PEQLOCATION;

typedef struct _EQLINE
{
    float X1;
    float Y1;
    float Z1 = 1.0;
    float X2;
    float Y2;
    float Z2 = 1.0;
} EQLINE, *PEQLINE;

typedef struct _EQARGBCOLOR
{
    union
    {
        struct
        {
            BYTE B;
            BYTE G;
            BYTE R;
            BYTE A;
        };
        DWORD ARGB;
    };
} EQARGBCOLOR, *PEQARGBCOLOR;

typedef struct _EQZONEINFO
{
/* 0x0000 */ CHAR PlayerName[40];
/* 0x0040 */ CHAR ShortName[20];
/* 0x0060 */ CHAR LongName[80];
} EQZONEINFO, *PEQZONEINFO;

// class EQ_Character
typedef struct _EQCHARINFO
{
/* 0x0000 */ BYTE Unknown0000;
/* 0x0001 */ BYTE Unknown0001;
/* 0x0002 */ CHAR Name[64]; // [0x40]
/* 0x0042 */ CHAR LastName[70]; // [0x46] ; surname or title
/* 0x0088 */ WORD Gender; // EQ_GENDER_x
/* 0x008A */ WORD Race; // EQ_RACE_x
/* 0x008C */ WORD Class; // EQ_CLASS_x
/* 0x008E */ WORD Unknown008E;
/* 0x0090 */ WORD Level;
/* 0x0092 */ WORD Unknown0092;
/* 0x0094 */ DWORD Experience; // EXP
/* 0x0098 */ WORD Face;
/* 0x009A */ WORD Mana;
/* 0x009C */ WORD BaseHP;
/* 0x009E */ WORD StunnedState; // EQ_STUNNED_STATE_x
/* 0x00A0 */ WORD BaseSTR;
/* 0x00A2 */ WORD BaseSTA;
/* 0x00A4 */ WORD BaseCHA;
/* 0x00A6 */ WORD BaseDEX;
/* 0x00A8 */ WORD BaseINT;
/* 0x00AA */ WORD BaseAGI;
/* 0x00AC */ WORD BaseWIS;
/* 0x00AE */ BYTE Unknown00AE[1];
/* 0x0732 */ WORD SpellBook[250]; // [0xFA]
/* 0x0A36 */ BYTE Unknown082A[524]; // [0x20C]
/* 0x0A3E */ WORD MemorizedSpells[8]; // [0x08]
/* 0x0A50 */ BYTE Unknown0A3E[18];
/* 0x0A54 */ DWORD Unknown0A50;
/* 0x0A58 */ DWORD Unknown2904;
/* 0x0A5C */ DWORD Unknown2908;
/* 0x0A60 */ DWORD Unknown2912;
	DWORD Unknown2916;
////////////////////////////////////////////////
/* 0x0B64 */ BYTE StandingState; // EQ_STANDING_STATE_x
/* 0x0B68 */ DWORD Platinum;
/* 0x0B6C */ DWORD Gold;
/* 0x0B70 */ DWORD Silver;
/* 0x0B74 */ DWORD Copper;
/* 0x0B78 */ DWORD BankPlatinum;
/* 0x0B7C */ DWORD BankGold;
/* 0x0B80 */ DWORD BankSilver;
/* 0x0B84 */ DWORD BankCopper;
} EQCHARINFO, *PEQCHARINFO;

typedef struct _EQACTORINFO
{
} EQACTORINFO, *PEQACTORINFO;

// sizeof EQSPAWNINFO 0x168
typedef struct _EQSPAWNINFO
{
/* 0x0000 */ BYTE Unknown0000; // always equals 0x03
/* 0x0001 */ CHAR Name[30]; // [0x1E]
/* 0x001F */ BYTE Unknown001F[37];
/* 0x0044 */ DWORD ZoneId; // EQ_ZONE_ID_x
/* 0x0048 */ FLOAT Y;
/* 0x004C */ FLOAT X;
/* 0x0050 */ FLOAT Z;
/* 0x0054 */ FLOAT Heading; // camera view left/right, yaw
/* 0x0058 */ FLOAT Unknown0058;
/* 0x005C */ FLOAT MovementSpeed;
/* 0x0060 */ FLOAT MovementSpeedY;
/* 0x0064 */ FLOAT MovementSpeedX;
/* 0x0068 */ FLOAT MovementSpeedZ;
/* 0x006C */ FLOAT MovementSpeedHeading;
/* 0x0070 */ FLOAT Unknown0070;
/* 0x0074 */ FLOAT Pitch; // camera view up/down
/* 0x0078 */ struct _EQSPAWNINFO* Prev;
/* 0x007C */ struct _EQSPAWNINFO* Next;
/* 0x0080 */ PVOID Unknown0080;
/* 0x0084 */ PEQACTORINFO ActorInfo; // EQACTORINFO
/* 0x0088 */ PEQCHARINFO CharInfo; // EQCHARINFO
/* 0x008C */ FLOAT CameraHeightOffset;
/* 0x0090 */ FLOAT ModelHeightOffset;
/* 0x0094 */ WORD SpawnId;
/* 0x0096 */ WORD PetOwnerSpawnId; // spawn id of the owner of this pet spawn
/* 0x0098 */ DWORD HpMax;
/* 0x009C */ DWORD HpCurrent;
/* 0x00A0 */ WORD GuildId;
/* 0x00A2 */ BYTE Unknown00A2[6];
/* 0x00A8 */ BYTE Type; // EQ_SPAWN_TYPE_x
/* 0x00A9 */ BYTE Class; // EQ_CLASS_x
/* 0x00AA */ WORD Race;  // EQ_RACE_x
/* 0x00AC */ BYTE Gender; // EQ_GENDER_x
/* 0x00AD */ BYTE Level;
/* 0x00AE */ BYTE IsHidden; // 0 = Visible, 1 = Invisible
/* 0x00AF */ BYTE IsSnared; // 0 = Normal Movement Speed, 1 = Slow Movement Speed
/* 0x00B0 */ BYTE IsPlayerKill; // PVP flagged with red name by Priest of Discord
/* 0x00B1 */ BYTE StandingState; // EQ_STANDING_STATE_x
/* 0x00B2 */ BYTE LightType; // EQ_LIGHT_TYPE_x
/* 0x00B3 */ BYTE Unknown00B3;
/* 0x00B4 */ WORD EquipmentMaterialType[7]; // EQ_EQUIPMENT_MATERIAL_TYPE_x ; Head,Chest,Arms,Wrist,Hands,Legs,Feet
/* 0x00C2 */ WORD EquipmentPrimaryItemType; // EQ_EQUIPMENT_ITEM_TYPE_x ; Primary
/* 0x00C4 */ WORD EquipmentSecondaryItemType; // EQ_EQUIPMENT_ITEM_TYPE_x ; Secondary
/* 0x00C6 */ WORD Unknown00C6;
/* 0x00C8 */ BYTE Unknown00C8[36];
/* 0x00EC */ BYTE Unknown00EC;
/* 0x00F0 */ FLOAT Height; // model height or size/scale (shrink, grow, etc)
/* 0x00F4 */ FLOAT Unknown00F4;
/* 0x00F8 */ FLOAT Unknown00F8;
/* 0x00FC */ FLOAT Unknown00FC;
/* 0x0100 */ FLOAT MovementBackwardSpeedMultiplier; // modifies how fast you move backwards (Default = 0.4600000083)
/* 0x0104 */ FLOAT MovementForwardSpeedMultiplier; // modifies how fast you move forwards (Default = 0.6999999881)
/* 0x0108 */ BYTE IsLinkDead; // LD
/* 0x0109 */ BYTE IsGameMaster; // GM
/* 0x010A */ WORD LevitationState; // EQ_LEVITATION_STATE_x
/* 0x010C */ DWORD TargetType; // EQ_SPAWN_TARGET_TYPE_x
/* 0x0110 */ DWORD Unknown0110;
/* 0x0114 */ DWORD AnonymousState; // EQ_ANONYMOUS_STATE_x, /anonymous and /roleplay
/* 0x0118 */ DWORD Unknown0118;
/* 0x011C */ DWORD IsAwayFromKeyboard; // AFK
/* 0x0120 */ BYTE Unknown0120[12];
/* 0x012C */ CHAR LastName[22]; // surname or title
/* 0x0142 */ BYTE Unknown0142[10];
/* 0x014C */ WORD GuildRank;
/* 0x014E */ WORD Deity; // EQ_DEITY_x
/* 0x0150 */ BYTE Unknown0150;
/* 0x0151 */ BYTE Unknown0151[6];
/* 0x0157 */ BYTE Unknown0157[5];
/* 0x015C */ DWORD Unknown015C;
/* 0x0160 */ DWORD Unknown0160;
/* 0x0164 */ DWORD Unknown0164;
} EQSPAWNINFO, *PEQSPAWNINFO;

typedef struct _EQGROUNDSPAWNINFO
{
/* 0x0000 */ struct _EQGROUNDSPAWNINFO* Prev;
/* 0x0004 */ struct _EQGROUNDSPAWNINFO* Next;
/* 0x0008 */ WORD Id;
/* 0x000A */ BYTE Unknown000A[2];
/* 0x000C */ WORD DropId;
/* 0x000E */ BYTE Unknown000E[2];
/* 0x0010 */ WORD Unknown0010;
/* 0x0012 */ BYTE Unknown0012[2];
/* 0x0014 */ DWORD Unknown0014; // pointer
/* 0x0018 */ BYTE Unknown0018[116];
/* 0x008C */ FLOAT Heading;
/* 0x0090 */ FLOAT Z;
/* 0x0094 */ FLOAT Y;
/* 0x0098 */ FLOAT X;
/* 0x009C */ CHAR Name[30];
} EQGROUNDSPAWNINFO, *PEQGROUNDSPAWNINFO;

// sizeof EQGUILDINFO 0x96
typedef struct _EQGUILDINFO
{
/* 0x0000 */ CHAR Name[32];
/* 0x0032 */ BYTE Unknown0032[64];
} EQGUILDINFO, *PEQGUILDINFO;

typedef struct _EQGUILDLIST
{
    EQGUILDINFO Guild[512];
} EQGUILDLIST, *PEQGUILDLIST;

/****************************************************************************************************/

void EQ_ToggleBool(bool &b)
{
    b = !b;
}

float EQ_CalculateDistance(float x1, float y1, float x2, float y2)
{
    return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

bool EQ_IsInGame(CMemory* mem)
{
    BYTE result = mem->ReadAny<BYTE>(EQ_IS_IN_GAME);

    return (result != 0);
}

bool EQ_IsAutoAttackEnabled(CMemory* mem)
{
    BYTE result = mem->ReadAny<BYTE>(EQ_IS_AUTO_ATTACK_ENABLED);

    return (result != 0);
}

DWORD EQ_GetZoneId(CMemory* mem)
{
    return mem->ReadAny<DWORD>(EQ_ZONE_ID);
}

EQCHARINFO EQ_GetCharInfo(CMemory* mem)
{
    return mem->ReadStructFromPointer<EQCHARINFO>(EQ_POINTER_CHAR_INFO);
}

//EQCEVERQUEST EQ_GetCEverQuest(CMemory* mem)
//{
    //return mem->ReadStructFromPointer<DWORD>(EQ_CEverQuest_POINTER);
//}

//DWORD EQ_GetGameState(CMemory* mem)
//{
    //DWORD CEverQuest = EQ_GetCEverQuest(mem);

    //return mem->ReadAny<DWORD>(CEverQuest + EQ_OFFSET_CEverQuest_GAME_STATE);
//}

char* EQ_GetRaceShortName(int race)
{
    char* raceShortName;

    switch (race)
    {
        case EQ_RACE_UNKNOWN:
            raceShortName = "UNK";
            break;

        case EQ_RACE_HUMAN:
            raceShortName = "HUM";
            break;

        case EQ_RACE_BARBARIAN:
            raceShortName = "BAR";
            break;

        case EQ_RACE_ERUDITE:
            raceShortName = "ERU";
            break;

        case EQ_RACE_WOOD_ELF:
            raceShortName = "ELF";
            break;

        case EQ_RACE_HIGH_ELF:
            raceShortName = "HIE";
            break;

        case EQ_RACE_DARK_ELF:
            raceShortName = "DEF";
            break;

        case EQ_RACE_HALF_ELF:
            raceShortName = "HEF";
            break;

        case EQ_RACE_DWARF:
            raceShortName = "DWF";
            break;

        case EQ_RACE_TROLL:
            raceShortName = "TRL";
            break;

        case EQ_RACE_OGRE:
            raceShortName = "OGR";
            break;

        case EQ_RACE_HALFLING:
            raceShortName = "HFL";
            break;

        case EQ_RACE_GNOME:
            raceShortName = "GNM";
            break;

        case EQ_RACE_IKSAR:
            raceShortName = "IKS";
            break;

        case EQ_RACE_VAH_SHIR:
            raceShortName = "VAH";
            break;

        case EQ_RACE_FROGLOK:
            raceShortName = "FRG";
            break;

        default:
            raceShortName = "UNK";
            break;
    }

    return raceShortName;
}

char* EQ_GetClassShortName(int _class)
{
    char* classShortName;

    switch (_class)
    {
        case EQ_CLASS_UNKNOWN:
            classShortName = "UNK";
            break;

        case EQ_CLASS_WARRIOR:
        case EQ_CLASS_WARRIOR_GUILDMASTER:
            classShortName = "WAR";
            break;

        case EQ_CLASS_CLERIC:
        case EQ_CLASS_CLERIC_GUILDMASTER:
            classShortName = "CLR";
            break;

        case EQ_CLASS_PALADIN:
        case EQ_CLASS_PALADIN_GUILDMASTER:
            classShortName = "PAL";
            break;

        case EQ_CLASS_RANGER:
        case EQ_CLASS_RANGER_GUILDMASTER:
            classShortName = "RNG";
            break;

        case EQ_CLASS_SHADOWKNIGHT:
        case EQ_CLASS_SHADOWKNIGHT_GUILDMASTER:
            classShortName = "SHD";
            break;

        case EQ_CLASS_DRUID:
        case EQ_CLASS_DRUID_GUILDMASTER:
            classShortName = "DRU";
            break;

        case EQ_CLASS_MONK:
        case EQ_CLASS_MONK_GUILDMASTER:
            classShortName = "MNK";
            break;

        case EQ_CLASS_BARD:
        case EQ_CLASS_BARD_GUILDMASTER:
            classShortName = "BRD";
            break;

        case EQ_CLASS_ROGUE:
        case EQ_CLASS_ROGUE_GUILDMASTER:
            classShortName = "ROG";
            break;

        case EQ_CLASS_SHAMAN:
        case EQ_CLASS_SHAMAN_GUILDMASTER:
            classShortName = "SHM";
            break;

        case EQ_CLASS_NECROMANCER:
        case EQ_CLASS_NECROMANCER_GUILDMASTER:
            classShortName = "NEC";
            break;

        case EQ_CLASS_WIZARD:
        case EQ_CLASS_WIZARD_GUILDMASTER:
            classShortName = "WIZ";
            break;

        case EQ_CLASS_MAGICIAN:
        case EQ_CLASS_MAGICIAN_GUILDMASTER:
            classShortName = "MAG";
            break;

        case EQ_CLASS_ENCHANTER:
        case EQ_CLASS_ENCHANTER_GUILDMASTER:
            classShortName = "ENC";
            break;

        case EQ_CLASS_BEASTLORD:
        case EQ_CLASS_BEASTLORD_GUILDMASTER:
            classShortName = "BST";
            break;

        case EQ_CLASS_BANKER:
            classShortName = "BANKER";
            break;

        case EQ_CLASS_MERCHANT:
            classShortName = "MERCHANT";
            break;

        default:
            classShortName = "UNK";
            break;
    }

    return classShortName;
}

char* EQ_GetStandingStateString(int standingState)
{
    char* standingStateString;

    switch (standingState)
    {
        case EQ_STANDING_STATE_STANDING:
            standingStateString = "Standing";
            break;

        case EQ_STANDING_STATE_FROZEN:
            standingStateString = "Mesmerized / Feared";
            break;

        case EQ_STANDING_STATE_LOOTING:
            standingStateString = "Looting / Binding Wounds";
            break;

        case EQ_STANDING_STATE_SITTING:
            standingStateString = "Sitting";
            break;

        case EQ_STANDING_STATE_DUCKING:
            standingStateString = "Ducking";
            break;

        case EQ_STANDING_STATE_FEIGNED:
            standingStateString = "Feigning Death";
            break;

        case EQ_STANDING_STATE_DEAD:
            standingStateString = "Dead";
            break;

        default:
            standingStateString = "Unknown";
            break;
    }

    return standingStateString;
}

char* EQ_GetGroundSpawnName(char* spawnActorDefinitionName)
{
    char* spawnName = spawnActorDefinitionName;

    if (spawnActorDefinitionName == "IT63_ACTORDEF")
    {
        spawnName = "Dropped Item";
    }

    if (spawnActorDefinitionName == "IT64_ACTORDEF")
    {
        spawnName = "Dropped Bag";
    }

    if (spawnActorDefinitionName == "IT66_ACTORDEF")
    {
        spawnName = "Forge";
    }

    if (spawnActorDefinitionName == "IT69_ACTORDEF")
    {
        spawnName = "Oven";
    }

    if (spawnActorDefinitionName == "IT70_ACTORDEF")
    {
        spawnName = "Brew Barrel";
    }

    if (spawnActorDefinitionName == "IT73_ACTORDEF")
    {
        spawnName = "Kiln";
    }

    if (spawnActorDefinitionName == "IT74_ACTORDEF")
    {
        spawnName = "Pottery Wheel";
    }

    if (spawnActorDefinitionName == "IT128_ACTORDEF")
    {
        spawnName = "Sewing Kit";
    }

    return spawnName;
}

char* EQ_GetGuildNameById(CMemory* mem, int guildId)
{
    if (guildId == EQ_GUILD_ID_NULL)
    {
        return "Unknown Guild";
    }

    EQGUILDLIST guildList = mem->ReadAny<EQGUILDLIST>(EQ_STRUCTURE_GUILD_LIST);

    return guildList.Guild[guildId].Name;
}

#endif // EQMAC_HPP
