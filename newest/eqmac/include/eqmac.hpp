#ifndef EQMAC_HPP
#define EQMAC_HPP

#include <string>
#include <vector>
#include <cmath>

#include <windows.h>

#include "memory.hpp"

#include "eqmac_structures.hpp"

const std::string everquest_title = "EQW beta 2.32";

const std::string everquest_graphics_dll_build_date = "Version 1.1.39 (DX8)  Last Build Date: Oct  8 2002 10:34:54";

#define EVERQUEST_GRAPHICS_DLL_BUILD_DATE      0x007F95D0
#define EVERQUEST_GRAPHICS_DLL_BUILD_DATE_SIZE 0x40

#define EVERQUEST_GRAPHICS_DLL_POINTER 0x007F9C50 // EQGfx_Dx8.DLL base address

#define EVERQUEST_IS_IN_GAME 0x00798550 // BYTE

#define EVERQUEST_IS_AUTO_ATTACK_ENABLED 0x007F6FFE // BYTE

#define EVERQUEST_StringTable_POINTER 0x007F9490

#define EVERQUEST_CEverQuest_POINTER 0x00809478 // POINTER to STRUCT
#define EVERQUEST_OFFSET_CEverQuest_GAME_STATE 0x5AC // DWORD

#define EVERQUEST_GAME_STATE_CHARACTER_SELECT 1
#define EVERQUEST_GAME_STATE_IN_GAME          5

#define EVERQUEST_CDisplay_POINTER 0x007F9510 // POINTER to STRUCT
#define EVERQUEST_OFFSET_CDisplay_IS_HOLDING_ITEM   0x040 // BYTE
#define EVERQUEST_OFFSET_CDisplay_IS_HOLDING_HOTKEY 0x042 // BYTE
#define EVERQUEST_OFFSET_CDisplay_TIMER             0x0C8 // DWORD ; 1000 milliseconds = 1 second

#define EVERQUEST_CHotButtonWnd_POINTER 0x0063D628 // POINTER to STRUCT

#define EVERQUEST_CLootWnd_POINTER 0x0063D65C // POINTER to STRUCT
#define EVERQUEST_OFFSET_CLootWnd_IS_OPEN   0x134 // BYTE
#define EVERQUEST_OFFSET_CLootWnd_ITEMS     0x1B8 // STRUCT of POINTER to STRUCT

#define EVERQUEST_CLootWnd_ITEMS_OFFSET 0x04

#define EVERQUEST_CLootWnd_ITEMS_MAX 30

#define EVERQUEST_CTradeWnd_POINTER 0x0063D668

#define EVERQUEST_CGiveWnd_POINTER 0x0063D678

#define EVERQUEST_OFFSET_ITEM_INFO_NAME             0x000 // STRING [0x40]
#define EVERQUEST_OFFSET_ITEM_INFO_LORE_NAME        0x040 // STRING [0x50]
#define EVERQUEST_OFFSET_ITEM_INFO_ID_FILE          0x090 // STRING [0x06]
#define EVERQUEST_OFFSET_ITEM_INFO_WEIGHT           0x0AE // BYTE ; multiply by 0.1 for actual decimal weight
#define EVERQUEST_OFFSET_ITEM_INFO_IS_NO_RENT       0x0AF // BYTE ; 255 = False
#define EVERQUEST_OFFSET_ITEM_INFO_IS_NO_DROP       0x0B0 // BYTE ; 255 = False
#define EVERQUEST_OFFSET_ITEM_INFO_SIZE             0x0B1 // BYTE ; EVERQUEST_ITEM_SIZE_x
#define EVERQUEST_OFFSET_ITEM_INFO_IS_CONTAINER     0x0B2 // BYTE ; 1 = True
#define EVERQUEST_OFFSET_ITEM_INFO_ID               0x0B4 // WORD
#define EVERQUEST_OFFSET_ITEM_INFO_ICON             0x0B6 // WORD
#define EVERQUEST_OFFSET_ITEM_INFO_EQUIP_SLOT       0x0B8 // DWORD
#define EVERQUEST_OFFSET_ITEM_INFO_EQUIPPABLE_SLOTS 0x0BC // DWORD
#define EVERQUEST_OFFSET_ITEM_INFO_COST             0x0C0 // DWORD

#define EVERQUEST_ITEM_INFO_NAME_SIZE      0x40
#define EVERQUEST_ITEM_INFO_LORE_NAME_SIZE 0x50
#define EVERQUEST_ITEM_INFO_ID_FILE_SIZE   0x06

#define EVERQUEST_ITEM_INFO_NULL 0x00000000

#define EVERQUEST_ITEM_SIZE_TINY   0
#define EVERQUEST_ITEM_SIZE_SMALL  1
#define EVERQUEST_ITEM_SIZE_MEDIUM 2
#define EVERQUEST_ITEM_SIZE_LARGE  3
#define EVERQUEST_ITEM_SIZE_GIANT  4

#define EVERQUEST_FONT_POINTER 0x0063D3B0 // POINTER to STRUCT

#define EVERQUEST_ZONE_ID 0x007B9648 // DWORD

#define EVERQUEST_ZONE_ID_FREPORTW       9   // West Freeport
#define EVERQUEST_ZONE_ID_COMMONS        21  // West Commonlands
#define EVERQUEST_ZONE_ID_ECOMMONS       22  // East Commonlands
#define EVERQUEST_ZONE_ID_CSHOME         26  // Sunset Home
#define EVERQUEST_ZONE_ID_SLEEPER        128 // Sleeper's Tomb
#define EVERQUEST_ZONE_ID_BAZAAR         151 // The Bazaar
#define EVERQUEST_ZONE_ID_NEXUS          152 // The Nexus
#define EVERQUEST_ZONE_ID_SSERU          159 // Sanctus Seru
#define EVERQUEST_ZONE_ID_TUTORIAL       183 // Tutorial Zone
#define EVERQUEST_ZONE_ID_LOAD           184 // Loading
#define EVERQUEST_ZONE_ID_LOAD2          185 // Loading
#define EVERQUEST_ZONE_ID_POKNOWLEDGE    202 // The Plane of Knowledge
#define EVERQUEST_ZONE_ID_POTRANQUILITY  203 // Plane of Tranquility
#define EVERQUEST_ZONE_ID_POTIMEA        219 // Plane of Time
#define EVERQUEST_ZONE_ID_POTIMEB        223 // Plane of Time

#define EVERQUEST_ZONE_INFO_STRUCTURE           0x00798784 // STRUCT
#define EVERQUEST_OFFSET_ZONE_INFO_PLAYER_NAME  0x0000 // STRING [0x40]
#define EVERQUEST_OFFSET_ZONE_INFO_SHORT_NAME   0x0040 // STRING [0x20]
#define EVERQUEST_OFFSET_ZONE_INFO_LONG_NAME    0x0060 // STRING [0x80]
#define EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Y 0x01E8 // FLOAT
#define EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_X 0x01EC // FLOAT
#define EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Z 0x01F0 // FLOAT

#define EVERQUEST_ZONE_INFO_PLAYER_NAME_SIZE 0x40
#define EVERQUEST_ZONE_INFO_SHORT_NAME_SIZE  0x20
#define EVERQUEST_ZONE_INFO_LONG_NAME_SIZE   0x80

#define EVERQUEST_WORLD_INFO_POINTER       0x007F9494 // POINTER to STRUCT
#define EVERQUEST_OFFSET_WORLD_INFO_HOUR   0x04 // BYTE
#define EVERQUEST_OFFSET_WORLD_INFO_MINUTE 0x05 // BYTE
#define EVERQUEST_OFFSET_WORLD_INFO_DAY    0x06 // BYTE
#define EVERQUEST_OFFSET_WORLD_INFO_MONTH  0x07 // BYTE
#define EVERQUEST_OFFSET_WORLD_INFO_YEAR   0x08 // BYTE

#define EVERQUEST_GROUND_SPAWN_INFO_POINTER                        0x007F949C // POINTER to STRUCT
#define EVERQUEST_OFFSET_GROUND_SPAWN_INFO_PREV_SPAWN_INFO_POINTER 0x00 // POINTER to STRUCT
#define EVERQUEST_OFFSET_GROUND_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER 0x04 // POINTER to STRUCT
#define EVERQUEST_OFFSET_GROUND_SPAWN_INFO_ID                      0x08 // WORD
#define EVERQUEST_OFFSET_GROUND_SPAWN_INFO_DROP_ID                 0x0C // WORD
#define EVERQUEST_OFFSET_GROUND_SPAWN_INFO_Z                       0x90 // FLOAT
#define EVERQUEST_OFFSET_GROUND_SPAWN_INFO_X                       0x94 // FLOAT
#define EVERQUEST_OFFSET_GROUND_SPAWN_INFO_Y                       0x98 // FLOAT
#define EVERQUEST_OFFSET_GROUND_SPAWN_INFO_NAME                    0x9C // STRING [0x40]

#define EVERQUEST_GROUND_SPAWN_INFO_NAME_SIZE 0x40

// ??
//#define EVERQUEST_DOORS_INFO_POINTER 0x007F94B8 // POINTER to STRUCT

// class EQ_Character
#define EVERQUEST_EQ_Character_POINTER                0x007F94E8 // POINTER to STRUCT
#define EVERQUEST_CHAR_INFO_POINTER                   EVERQUEST_EQ_Character_POINTER // POINTER to STRUCT
#define EVERQUEST_OFFSET_CHAR_INFO_NAME               0x0002 // STRING [0x40]
#define EVERQUEST_OFFSET_CHAR_INFO_MANA_CURRENT       0x009A // WORD
#define EVERQUEST_OFFSET_CHAR_INFO_IS_STUNNED         0x009E // BYTE
#define EVERQUEST_OFFSET_CHAR_INFO_BUFFS_STRUCTURE    0x0264 // STRUCT [0x0F] ; 15 buffs max
#define EVERQUEST_OFFSET_CHAR_INFO_STANDING_STATE     0x0B64 // BYTE ; EVERQUEST_STANDING_STATE_x
#define EVERQUEST_OFFSET_CHAR_INFO_PLATINUM           0x0B68 // DWORD
#define EVERQUEST_OFFSET_CHAR_INFO_GOLD               0x0B6C // DWORD
#define EVERQUEST_OFFSET_CHAR_INFO_SILVER             0x0B70 // DWORD
#define EVERQUEST_OFFSET_CHAR_INFO_COPPER             0x0B74 // DWORD
#define EVERQUEST_OFFSET_CHAR_INFO_BANK_PLATINUM      0x0B78 // DWORD
#define EVERQUEST_OFFSET_CHAR_INFO_BANK_GOLD          0x0B7C // DWORD
#define EVERQUEST_OFFSET_CHAR_INFO_BANK_SILVER        0x0B80 // DWORD
#define EVERQUEST_OFFSET_CHAR_INFO_BANK_COPPER        0x0B84 // DWORD
#define EVERQUEST_OFFSET_CHAR_INFO_SPAWN_INFO_POINTER 0x0D74 // POINTER to STRUCT

#define EVERQUEST_CHAR_INFO_NAME_SIZE 0x40

#define EVERQUEST_BUFFS_STRUCTURE_BUFF_SIZE 0x0A // 0x10
#define EVERQUEST_OFFSET_BUFFS_STRUCTURE_UNKNOWN_0x00     0x00
#define EVERQUEST_OFFSET_BUFFS_STRUCTURE_CASTER_LEVEL     0x01 // BYTE ; level of player who casted the buff
#define EVERQUEST_OFFSET_BUFFS_STRUCTURE_MODIFIER         0x02 // BYTE ; divide by 10 to get Bard song modifier
#define EVERQUEST_OFFSET_BUFFS_STRUCTURE_IS_DAMAGE_SHIELD 0x03 // BYTE ; damage shield = -1
#define EVERQUEST_OFFSET_BUFFS_STRUCTURE_SPELL_ID         0x04 // WORD
#define EVERQUEST_OFFSET_BUFFS_STRUCTURE_DURATION         0x06 // DWORD ; seconds = duration * 6
//#define EVERQUEST_OFFSET_BUFFS_STRUCTURE_DAMAGE_ABSORB    0x0C // DWORD

#define EVERQUEST_BUFFS_MAX 15

#define EVERQUEST_BUFF_SPELL_ID_NULL 0xFFFF

#define EVERQUEST_BUFF_TYPE_DETRIMENTAL           0
#define EVERQUEST_BUFF_TYPE_BENEFICIAL            1
#define EVERQUEST_BUFF_TYPE_BENEFICIAL_GROUP_ONLY 2

// class EQPlayer
#define EVERQUEST_PLAYER_SPAWN_INFO_POINTER     0x007F94CC // POINTER to STRUCT
#define EVERQUEST_TARGET_SPAWN_INFO_POINTER     0x007F94EC // POINTER to STRUCT
#define EVERQUEST_CONTROLLED_SPAWN_INFO_POINTER 0x007F94E0 // POINTER to STRUCT

#define EVERQUEST_ACTIVE_TRADE_SPAWN_INFO_POINTER      0x007F94C8 // POINTER to STRUCT
#define EVERQUEST_ACTIVE_MERCHANT_SPAWN_INFO_POINTER   0x007F94F8 // POINTER to STRUCT
#define EVERQUEST_ACTIVE_BANKER_SPAWN_INFO_POINTER     0x007F94FC // POINTER to STRUCT
#define EVERQUEST_ACTIVE_CORPSE_SPAWN_INFO_POINTER     0x007F9500 // POINTER to STRUCT
#define EVERQUEST_ACTIVE_GAMEMASTER_SPAWN_INFO_POINTER 0x007F9504 // POINTER to STRUCT

#define EVERQUEST_SPAWN_INFO_NULL 0x00000000

#define EVERQUEST_OFFSET_SPAWN_INFO_NAME                    0x0001 // STRING [0x40]
#define EVERQUEST_OFFSET_SPAWN_INFO_ZONE_ID                 0x0044 // DWORD
#define EVERQUEST_OFFSET_SPAWN_INFO_Y                       0x0048 // FLOAT
#define EVERQUEST_OFFSET_SPAWN_INFO_X                       0x004C // FLOAT
#define EVERQUEST_OFFSET_SPAWN_INFO_Z                       0x0050 // FLOAT
#define EVERQUEST_OFFSET_SPAWN_INFO_HEADING                 0x0054 // FLOAT ; facing direction, rotation
#define EVERQUEST_OFFSET_SPAWN_INFO_MOVEMENT_SPEED          0x005C // FLOAT
#define EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER 0x0078 // POINTER to STRUCT
#define EVERQUEST_OFFSET_SPAWN_INFO_PREV_SPAWN_INFO_POINTER 0x007C // POINTER to STRUCT
#define EVERQUEST_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER      0x0084 // POINTER to STRUCT
#define EVERQUEST_OFFSET_SPAWN_INFO_CHAR_INFO_POINTER       0x0088 // POINTER to STRUCT
#define EVERQUEST_OFFSET_SPAWN_INFO_CAMERA_HEIGHT           0x008C // FLOAT ; height from where your character's eyes see
#define EVERQUEST_OFFSET_SPAWN_INFO_MODEL_HEIGHT_OFFSET     0x0090 // FLOAT ; height above/below the ground
#define EVERQUEST_OFFSET_SPAWN_INFO_SPAWN_ID                0x0094 // WORD
#define EVERQUEST_OFFSET_SPAWN_INFO_OWNER_ID                0x0096 // WORD
#define EVERQUEST_OFFSET_SPAWN_INFO_HP_MAX                  0x0098 // DWORD
#define EVERQUEST_OFFSET_SPAWN_INFO_HP_CURRENT              0x009C // DWORD
#define EVERQUEST_OFFSET_SPAWN_INFO_GUILD_ID                0x00A0 // WORD
#define EVERQUEST_OFFSET_SPAWN_INFO_TYPE                    0x00A8 // BYTE ; EVERQUEST_SPAWN_INFO_TYPE_x
#define EVERQUEST_OFFSET_SPAWN_INFO_CLASS                   0x00A9 // BYTE ; EVERQUEST_CLASS_x
#define EVERQUEST_OFFSET_SPAWN_INFO_RACE                    0x00AA // BYTE ; EVERQUEST_RACE_x
#define EVERQUEST_OFFSET_SPAWN_INFO_LEVEL                   0x00AD // BYTE
#define EVERQUEST_OFFSET_SPAWN_INFO_HIDE_STATE              0x00AE // BYTE
#define EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE          0x00B1 // BYTE ; EVERQUEST_STANDING_STATE_x
#define EVERQUEST_OFFSET_SPAWN_INFO_PRIMARY                 0x00C2 // WORD
#define EVERQUEST_OFFSET_SPAWN_INFO_SECONDARY               0x00C4 // WORD ; offhand
#define EVERQUEST_OFFSET_SPAWN_INFO_MODEL_HEIGHT            0x00F0 // FLOAT ; height / size / actor scale factor
#define EVERQUEST_OFFSET_SPAWN_INFO_IS_LINKDEAD             0x0108 // BYTE
#define EVERQUEST_OFFSET_SPAWN_INFO_IS_AFK                  0x0122 // BYTE
#define EVERQUEST_OFFSET_SPAWN_INFO_LAST_NAME               0x012C // STRING [0x20]

#define EVERQUEST_SPAWNS_MAX 8192

#define EVERQUEST_SPAWN_INFO_NAME_SIZE      0x40
#define EVERQUEST_SPAWN_INFO_LAST_NAME_SIZE 0x20

#define EVERQUEST_SPAWN_INFO_TYPE_PLAYER 0
#define EVERQUEST_SPAWN_INFO_TYPE_NPC    1
#define EVERQUEST_SPAWN_INFO_TYPE_CORPSE 2

#define EVERQUEST_OFFSET_ACTOR_INFO_TIME_STAMP                  0x0058 // DWORD
#define EVERQUEST_OFFSET_ACTOR_INFO_TIME_STAMP_LAST_TICK        0x005C // DWORD
#define EVERQUEST_OFFSET_ACTOR_INFO_HEIGHT                      0x00B8 // FLOAT
#define EVERQUEST_OFFSET_ACTOR_INFO_CASTING_SPELL_TIME_STAMP    0x00CC // DWORD
#define EVERQUEST_OFFSET_ACTOR_INFO_CASTING_SPELL_TIME_STAMP_EX 0x00E0 // DWORD
#define EVERQUEST_OFFSET_ACTOR_INFO_ANIMATION                   0x0184 // BYTE
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_BOTH             0x0260 // DWORD
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_SECONDARY        0x0264 // DWORD
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_PRIMARY          0x0268 // DWORD
#define EVERQUEST_OFFSET_ACTOR_INFO_CASTING_SPELL_ID            0x027C // WORD
#define EVERQUEST_OFFSET_ACTOR_INFO_CASTING_SPELL_GEM_NUMBER    0x027E // BYTE
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_NOT_MOVING               0x032D // BYTE
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_LFG                      0x0438 // BYTE

#define EVERQUEST_CASTING_SPELL_ID_NULL 0xFFFF

#define EVERQUEST_CASTING_SPELL_GEM_NUMBER_SINGING 255

#define EVERQUEST_OLD_UI_INFO_POINTER 0x007F9574
#define EVERQUEST_OFFSET_OLD_UI_INFO_STATE               0x00E88 // BYTE ; EVERQUEST_OLD_UI_STATE_x
#define EVERQUEST_OFFSET_OLD_UI_INFO_IS_SIDE_WINDOW_OPEN 0x5F314 // DWORD ; trade window, loot window, bank window, merchant window

#define EVERQUEST_MOUSE_X 0x008092E8 // DWORD
#define EVERQUEST_MOUSE_Y 0x008092EC // DWORD

#define EVERQUEST_MOUSE_CLICK_STATE 0x00798614 // DWORD

#define EVERQUEST_MOUSE_CLICK_STATE_LEFT  1       //0x01000001
#define EVERQUEST_MOUSE_CLICK_STATE_RIGHT 1677612 //0x00010100
#define EVERQUEST_MOUSE_CLICK_STATE_BOTH  0x01010101

#define EVERQUEST_MOUSE_BUTTON_IS_HELD_DOWN 0x00798628 // DWORD

#define EVERQUEST_MOUSE_LOOK_STATE 0x007985EA // DWORD

#define EVERQUEST_MOUSE_LOOK_STATE_FALSE 0x00010000
#define EVERQUEST_MOUSE_LOOK_STATE_TRUE  0x00010001

#define EVERQUEST_RACE_UNKNOWN   0
#define EVERQUEST_RACE_HUMAN     1
#define EVERQUEST_RACE_BARBARIAN 2
#define EVERQUEST_RACE_ERUDITE   3
#define EVERQUEST_RACE_WOOD_ELF  4
#define EVERQUEST_RACE_HIGH_ELF  5
#define EVERQUEST_RACE_DARK_ELF  6
#define EVERQUEST_RACE_HALF_ELF  7
#define EVERQUEST_RACE_DWARF     8
#define EVERQUEST_RACE_TROLL     9
#define EVERQUEST_RACE_OGRE      10
#define EVERQUEST_RACE_HALFLING  11
#define EVERQUEST_RACE_GNOME     12
#define EVERQUEST_RACE_IKSAR     128
#define EVERQUEST_RACE_VAH_SHIR  130
#define EVERQUEST_RACE_FROGLOK   330
//#define EVERQUEST_RACE_DRAKKIN

#define EVERQUEST_CLASS_UNKNOWN      0
#define EVERQUEST_CLASS_WARRIOR      1
#define EVERQUEST_CLASS_CLERIC       2
#define EVERQUEST_CLASS_PALADIN      3
#define EVERQUEST_CLASS_RANGER       4
#define EVERQUEST_CLASS_SHADOWKNIGHT 5
#define EVERQUEST_CLASS_DRUID        6
#define EVERQUEST_CLASS_MONK         7
#define EVERQUEST_CLASS_BARD         8
#define EVERQUEST_CLASS_ROGUE        9
#define EVERQUEST_CLASS_SHAMAN       10
#define EVERQUEST_CLASS_NECROMANCER  11
#define EVERQUEST_CLASS_WIZARD       12
#define EVERQUEST_CLASS_MAGICIAN     13
#define EVERQUEST_CLASS_ENCHANTER    14
#define EVERQUEST_CLASS_BEASTLORD    15
//#define EVERQUEST_CLASS_BERSERKER
#define EVERQUEST_CLASS_BANKER       16
#define EVERQUEST_CLASS_WARRIOR_GUILDMASTER      17 // EVERQUEST_CLASS_WARRIOR + 16
#define EVERQUEST_CLASS_CLERIC_GUILDMASTER       18
#define EVERQUEST_CLASS_PALADIN_GUILDMASTER      19
#define EVERQUEST_CLASS_RANGER_GUILDMASTER       20
#define EVERQUEST_CLASS_SHADOWKNIGHT_GUILDMASTER 21
#define EVERQUEST_CLASS_DRUID_GUILDMASTER        22
#define EVERQUEST_CLASS_MONK_GUILDMASTER         23
#define EVERQUEST_CLASS_BARD_GUILDMASTER         24
#define EVERQUEST_CLASS_ROGUE_GUILDMASTER        25
#define EVERQUEST_CLASS_SHAMAN_GUILDMASTER       26
#define EVERQUEST_CLASS_NECROMANCER_GUILDMASTER  27
#define EVERQUEST_CLASS_WIZARD_GUILDMASTER       28
#define EVERQUEST_CLASS_MAGICIAN_GUILDMASTER     29
#define EVERQUEST_CLASS_ENCHANTER_GUILDMASTER    30
#define EVERQUEST_CLASS_BEASTLORD_GUILDMASTER    31 // EVERQUEST_CLASS_BEASTLORD + 16
#define EVERQUEST_CLASS_GUILDMASTER_BEGIN        EVERQUEST_CLASS_WARRIOR_GUILDMASTER
#define EVERQUEST_CLASS_GUILDMASTER_END          EVERQUEST_CLASS_BEASTLORD_GUILDMASTER
#define EVERQUEST_CLASS_MERCHANT     32

#define EVERQUEST_OFFSET_CLASS_GUILDMASTER 16 // EVERQUEST_CLASS_x + 16

#define EVERQUEST_GUILD_NAMES_BEGIN 0x007F9C94
#define EVERQUEST_GUILD_NAME_SIZE 0x60

#define EVERQUEST_GUILD_NAMES_MAX 512

#define EVERQUEST_GUILD_ID_NULL 0xFFFF // WORD ; = 65535

// EQPlayer::ChangePosition((uchar))
#define EVERQUEST_STANDING_STATE_STANDING 0x64
#define EVERQUEST_STANDING_STATE_FROZEN   0x66 // mesmerised / feared ; You lose control of yourself!
#define EVERQUEST_STANDING_STATE_LOOTING  0x69 // looting or bind wound
#define EVERQUEST_STANDING_STATE_SITTING  0x6E
#define EVERQUEST_STANDING_STATE_DUCKING  0x6F // crouching
#define EVERQUEST_STANDING_STATE_FEIGNED  0x73 // feign death
#define EVERQUEST_STANDING_STATE_DEAD     0x78

#define EVERQUEST_HEADING_MAX 512

// eqgame.exe+AAAD1 - 74 0C - je eqgame.exe+AAADF
#define EVERQUEST_ASM_PLAY_NICE_JUMP 0x004AAAD1
BYTE EVERQUEST_ASM_PLAY_NICE_JUMP_BYTES[] = {0x74, 0x0C};

// eqgame.exe+AAAD3 - FF 35 68967900 - push [eqgame.exe+399668]
// eqgame.exe+AAAD9 - FF 15 5C315E00 - call dword ptr [eqgame.exe+1E315C] -> kernel32.Sleep
#define EVERQUEST_ASM_PLAY_NICE_SLEEP_TIME 0x00799668 // DWORD ; eqgame.exe+399668

// call eqgame.exe+15833D
#define EVERQUEST_ASM_PLAY_NICE_SLEEP_IS_ENABLED 0x008063D0 // DWORD ; eqgame.exe+4063D0

void everquest_toggle_bool(bool &b)
{
    b = !b;
}

float everquest_calculate_distance(float x1, float y1, float x2, float y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

bool everquest_is_in_game(memory &mem)
{
    unsigned char result = mem.read_any<BYTE>(EVERQUEST_IS_IN_GAME);

    if (result == 0)
    {
        return false;
    }

    return true;
}

bool everquest_is_graphics_dll_build_date_correct(memory &mem)
{
    bool is_correct = false;

    std::string date = mem.read_string(EVERQUEST_GRAPHICS_DLL_BUILD_DATE, EVERQUEST_GRAPHICS_DLL_BUILD_DATE_SIZE);

    if (date == everquest_graphics_dll_build_date)
    {
        is_correct = true;
    }

    return is_correct;
}

bool everquest_is_auto_attack_enabled(memory &mem)
{
    unsigned char result = mem.read_any<BYTE>(EVERQUEST_IS_AUTO_ATTACK_ENABLED);

    if (result == 0)
    {
        return false;
    }

    return true;
}

int everquest_get_zone_id(memory &mem)
{
    return mem.read_any<DWORD>(EVERQUEST_ZONE_ID);
}

std::string everquest_get_zone_player_name(memory &mem)
{
    return mem.read_string(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_PLAYER_NAME, EVERQUEST_ZONE_INFO_PLAYER_NAME_SIZE);
}

std::string everquest_get_zone_short_name(memory &mem)
{
    std::string name = mem.read_string(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SHORT_NAME, EVERQUEST_ZONE_INFO_SHORT_NAME_SIZE);

    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    return name;
}

std::string everquest_get_zone_long_name(memory &mem)
{
    return mem.read_string(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_LONG_NAME, EVERQUEST_ZONE_INFO_LONG_NAME_SIZE);
}

int everquest_get_char_info(memory &mem)
{
    return mem.read_any<DWORD>(EVERQUEST_CHAR_INFO_POINTER);
}

int everquest_get_ground_spawn_info(memory &mem)
{
    return mem.read_any<DWORD>(EVERQUEST_GROUND_SPAWN_INFO_POINTER);
}

int everquest_get_old_ui_info(memory &mem)
{
    return mem.read_any<DWORD>(EVERQUEST_OLD_UI_INFO_POINTER);
}

int everquest_get_CDisplay(memory &mem)
{
    return mem.read_any<DWORD>(EVERQUEST_CDisplay_POINTER);
}

int everquest_get_CEverQuest(memory &mem)
{
    return mem.read_any<DWORD>(EVERQUEST_CEverQuest_POINTER);
}

int everquest_get_game_state(memory &mem)
{
    int CEverQuest_info = everquest_get_CEverQuest(mem);

    return mem.read_any<DWORD>(CEverQuest_info + EVERQUEST_OFFSET_CEverQuest_GAME_STATE);
}

int everquest_get_player_spawn_info(memory &mem)
{
    return mem.read_any<DWORD>(EVERQUEST_PLAYER_SPAWN_INFO_POINTER);
}

int everquest_get_player_actor_info(memory &mem)
{
    int player_spawn_info = everquest_get_player_spawn_info(mem);

    return mem.read_any<DWORD>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER);
}

int everquest_get_target_spawn_info(memory &mem)
{
    return mem.read_any<DWORD>(EVERQUEST_TARGET_SPAWN_INFO_POINTER);
}

int everquest_get_target_actor_info(memory &mem)
{
    int target_spawn_info = everquest_get_target_spawn_info(mem);

    return mem.read_any<DWORD>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER);
}

std::string everquest_get_race_short_name(unsigned char race)
{
    std::string race_short_name;

    switch (race)
    {
        case EVERQUEST_RACE_UNKNOWN:
            race_short_name = "UNK";
            break;

        case EVERQUEST_RACE_HUMAN:
            race_short_name = "HUM";
            break;

        case EVERQUEST_RACE_BARBARIAN:
            race_short_name = "BAR";
            break;

        case EVERQUEST_RACE_ERUDITE:
            race_short_name = "ERU";
            break;

        case EVERQUEST_RACE_WOOD_ELF:
            race_short_name = "ELF";
            break;

        case EVERQUEST_RACE_HIGH_ELF:
            race_short_name = "HIE";
            break;

        case EVERQUEST_RACE_DARK_ELF:
            race_short_name = "DEF";
            break;

        case EVERQUEST_RACE_HALF_ELF:
            race_short_name = "HEF";
            break;

        case EVERQUEST_RACE_DWARF:
            race_short_name = "DWF";
            break;

        case EVERQUEST_RACE_TROLL:
            race_short_name = "TRL";
            break;

        case EVERQUEST_RACE_OGRE:
            race_short_name = "OGR";
            break;

        case EVERQUEST_RACE_HALFLING:
            race_short_name = "HFL";
            break;

        case EVERQUEST_RACE_GNOME:
            race_short_name = "GNM";
            break;

        case EVERQUEST_RACE_IKSAR:
            race_short_name = "IKS";
            break;

        case EVERQUEST_RACE_VAH_SHIR:
            race_short_name = "VAH";
            break;

        case EVERQUEST_RACE_FROGLOK:
            race_short_name = "FRG";
            break;

        //case EVERQUEST_RACE_DRAKKIN:
            //race_short_name = "DRK";
            //break;

        default:
            race_short_name = "UNK";
            break;
    }

    return race_short_name;
}

std::string everquest_get_class_short_name(unsigned char _class)
{
    std::string class_short_name;

    switch (_class)
    {
        case EVERQUEST_CLASS_UNKNOWN:
            class_short_name = "UNK";
            break;

        case EVERQUEST_CLASS_WARRIOR:
        case EVERQUEST_CLASS_WARRIOR_GUILDMASTER:
            class_short_name = "WAR";
            break;

        case EVERQUEST_CLASS_CLERIC:
        case EVERQUEST_CLASS_CLERIC_GUILDMASTER:
            class_short_name = "CLR";
            break;

        case EVERQUEST_CLASS_PALADIN:
        case EVERQUEST_CLASS_PALADIN_GUILDMASTER:
            class_short_name = "PAL";
            break;

        case EVERQUEST_CLASS_RANGER:
        case EVERQUEST_CLASS_RANGER_GUILDMASTER:
            class_short_name = "RNG";
            break;

        case EVERQUEST_CLASS_SHADOWKNIGHT:
        case EVERQUEST_CLASS_SHADOWKNIGHT_GUILDMASTER:
            class_short_name = "SHD";
            break;

        case EVERQUEST_CLASS_DRUID:
        case EVERQUEST_CLASS_DRUID_GUILDMASTER:
            class_short_name = "DRU";
            break;

        case EVERQUEST_CLASS_MONK:
        case EVERQUEST_CLASS_MONK_GUILDMASTER:
            class_short_name = "MNK";
            break;

        case EVERQUEST_CLASS_BARD:
        case EVERQUEST_CLASS_BARD_GUILDMASTER:
            class_short_name = "BRD";
            break;

        case EVERQUEST_CLASS_ROGUE:
        case EVERQUEST_CLASS_ROGUE_GUILDMASTER:
            class_short_name = "ROG";
            break;

        case EVERQUEST_CLASS_SHAMAN:
        case EVERQUEST_CLASS_SHAMAN_GUILDMASTER:
            class_short_name = "SHM";
            break;

        case EVERQUEST_CLASS_NECROMANCER:
        case EVERQUEST_CLASS_NECROMANCER_GUILDMASTER:
            class_short_name = "NEC";
            break;

        case EVERQUEST_CLASS_WIZARD:
        case EVERQUEST_CLASS_WIZARD_GUILDMASTER:
            class_short_name = "WIZ";
            break;

        case EVERQUEST_CLASS_MAGICIAN:
        case EVERQUEST_CLASS_MAGICIAN_GUILDMASTER:
            class_short_name = "MAG";
            break;

        case EVERQUEST_CLASS_ENCHANTER:
        case EVERQUEST_CLASS_ENCHANTER_GUILDMASTER:
            class_short_name = "ENC";
            break;

        case EVERQUEST_CLASS_BEASTLORD:
        case EVERQUEST_CLASS_BEASTLORD_GUILDMASTER:
            class_short_name = "BST";
            break;

        //case EVERQUEST_CLASS_BERSERKER:
            //class_short_name = "BER";
            //break;

        case EVERQUEST_CLASS_BANKER:
            class_short_name = "BANKER";
            break;

        case EVERQUEST_CLASS_MERCHANT:
            class_short_name = "MERCHANT";
            break;

        default:
            class_short_name = "UNK";
            break;
    }

    return class_short_name;
}

std::string everquest_get_standing_state_name(unsigned char standing_state)
{
    std::string standing_state_name;

    switch (standing_state)
    {
        case EVERQUEST_STANDING_STATE_STANDING:
            standing_state_name = "Standing";
            break;

        case EVERQUEST_STANDING_STATE_FROZEN:
            standing_state_name = "Mesmerised / Feared";
            break;

        case EVERQUEST_STANDING_STATE_LOOTING:
            standing_state_name = "Looting / Bind Wound";
            break;

        case EVERQUEST_STANDING_STATE_SITTING:
            standing_state_name = "Sitting";
            break;

        case EVERQUEST_STANDING_STATE_DUCKING:
            standing_state_name = "Ducking";
            break;

        case EVERQUEST_STANDING_STATE_FEIGNED:
            standing_state_name = "Feign Death";
            break;

        case EVERQUEST_STANDING_STATE_DEAD:
            standing_state_name = "Dead";
            break;

        default:
            standing_state_name = "Unknown";
            break;
    }

    return standing_state_name;
}

std::string everquest_get_ground_spawn_name(std::string spawn_actor_definition_name)
{
    std::string spawn_name = spawn_actor_definition_name;

    if (spawn_actor_definition_name == "IT63_ACTORDEF")
    {
        spawn_name = "Dropped Item";
    }

    if (spawn_actor_definition_name == "IT64_ACTORDEF")
    {
        spawn_name = "Dropped Bag";
    }

    if (spawn_actor_definition_name == "IT66_ACTORDEF")
    {
        spawn_name = "Forge";
    }

    if (spawn_actor_definition_name == "IT69_ACTORDEF")
    {
        spawn_name = "Oven";
    }

    if (spawn_actor_definition_name == "IT70_ACTORDEF")
    {
        spawn_name = "Brew Barrel";
    }

    if (spawn_actor_definition_name == "IT73_ACTORDEF")
    {
        spawn_name = "Kiln";
    }

    if (spawn_actor_definition_name == "IT74_ACTORDEF")
    {
        spawn_name = "Pottery Wheel";
    }

    if (spawn_actor_definition_name == "IT128_ACTORDEF")
    {
        spawn_name = "Sewing Kit";
    }

    return spawn_name;
}

std::string everquest_get_guild_name(memory &mem, short guild_id)
{
    if (guild_id == EVERQUEST_GUILD_ID_NULL)
    {
        return "No Guild";
    }

    int guild_name_address = EVERQUEST_GUILD_NAMES_BEGIN;

    for (short i = 0; i < EVERQUEST_GUILD_NAMES_MAX; i++)
    {
        if (i == guild_id)
        {
            std::string guild_name = mem.read_string(guild_name_address, EVERQUEST_GUILD_NAME_SIZE);

            return guild_name;
        }

        guild_name_address = guild_name_address + EVERQUEST_GUILD_NAME_SIZE;
    }

    return "Unknown Guild";
}

unsigned short everquest_get_player_casting_spell_id(memory &mem)
{
    int player_actor_info = everquest_get_player_actor_info(mem);

    return mem.read_any<WORD>(player_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_CASTING_SPELL_ID);
}

unsigned char everquest_get_player_casting_spell_gem_number(memory &mem)
{
    int player_actor_info = everquest_get_player_actor_info(mem);

    return mem.read_any<BYTE>(player_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_CASTING_SPELL_GEM_NUMBER);
}

void everquest_update_spawns(memory &mem, std::vector<everquest_spawn_t> &spawns)
{
    spawns.clear();

    int player_spawn_info = everquest_get_player_spawn_info(mem);
    int target_spawn_info = everquest_get_target_spawn_info(mem);

    float player_y = mem.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
    float player_x = mem.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
    float player_z = mem.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

    int spawn_info_address = player_spawn_info;

    int spawn_next_spawn_info = mem.read_any<DWORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER);

    spawn_info_address = spawn_next_spawn_info;

    for (int i = 0; i < EVERQUEST_SPAWNS_MAX; i++)
    {
        spawn_next_spawn_info = mem.read_any<DWORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER);

        if (spawn_next_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            break;
        }

        int spawn_actor_info = mem.read_any<DWORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER);

        everquest_spawn_t spawn;

        spawn.address = spawn_info_address;

        spawn.spawn_id = mem.read_any<WORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_SPAWN_ID);
        spawn.owner_id = mem.read_any<WORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_OWNER_ID);
        spawn.guild_id = mem.read_any<WORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_GUILD_ID);

        spawn.name      = mem.read_string(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NAME,      EVERQUEST_SPAWN_INFO_NAME_SIZE);
        spawn.last_name = mem.read_string(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_LAST_NAME, EVERQUEST_SPAWN_INFO_LAST_NAME_SIZE);

        spawn.y = mem.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Y);
        spawn.x = mem.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_X);
        spawn.z = mem.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Z);

        spawn.distance = everquest_calculate_distance(player_x, player_y, spawn.x, spawn.y);

        spawn.distance_z = abs(spawn.z - player_z);

        spawn.heading = mem.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HEADING);

        spawn.movement_speed = mem.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_MOVEMENT_SPEED);

        spawn.standing_state = mem.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE);

        spawn.type   = mem.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_TYPE);
        spawn.level  = mem.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_LEVEL);
        spawn.race   = mem.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_RACE);
        spawn._class = mem.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_CLASS);

        spawn.hp     = mem.read_any<DWORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HP_CURRENT);
        spawn.hp_max = mem.read_any<DWORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HP_MAX);

        spawn.is_target = (spawn_info_address == target_spawn_info ? true : false);

        spawn.is_player_corpse = false;

        if (spawn.type == EVERQUEST_SPAWN_INFO_TYPE_CORPSE)
        {
            std::string player_name = mem.read_string(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_NAME, EVERQUEST_SPAWN_INFO_NAME_SIZE);

            std::size_t found = spawn.name.find(player_name);

            if (found != std::string::npos)
            {
                spawn.is_player_corpse = true;
            }
        }

        spawn.is_holding_both      = mem.read_any<DWORD>(spawn_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_BOTH);
        spawn.is_holding_secondary = mem.read_any<DWORD>(spawn_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_SECONDARY);
        spawn.is_holding_primary   = mem.read_any<DWORD>(spawn_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_PRIMARY);

        spawns.push_back(spawn);

        spawn_info_address = spawn_next_spawn_info;
    }
}

void everquest_update_ground_spawns(memory &mem, std::vector<everquest_ground_spawn_t> &ground_spawns)
{
    ground_spawns.clear();

    int spawn_info = everquest_get_ground_spawn_info(mem);

    if (spawn_info == 0)
    {
        return;
    }

    int spawn_info_address = spawn_info;

    //int spawn_next_spawn_info = mem.read_any<DWORD>(spawn_info_address + EVERQUEST_OFFSET_GROUND_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER);

    //spawn_info_address = spawn_next_spawn_info;

    int player_spawn_info = everquest_get_player_spawn_info(mem);

    float player_y = mem.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
    float player_x = mem.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
    float player_z = mem.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

    for (int i = 0; i < EVERQUEST_SPAWNS_MAX; i++)
    {
        int spawn_next_spawn_info = mem.read_any<DWORD>(spawn_info_address + EVERQUEST_OFFSET_GROUND_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER);

        if (spawn_next_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            break;
        }

        everquest_ground_spawn_t ground_spawn;

        ground_spawn.address = spawn_info_address;

        ground_spawn.id      = mem.read_any<WORD>(spawn_info_address + EVERQUEST_OFFSET_GROUND_SPAWN_INFO_ID);
        ground_spawn.drop_id = mem.read_any<WORD>(spawn_info_address + EVERQUEST_OFFSET_GROUND_SPAWN_INFO_DROP_ID);
        
        ground_spawn.name = mem.read_string(spawn_info_address + EVERQUEST_OFFSET_GROUND_SPAWN_INFO_NAME, EVERQUEST_GROUND_SPAWN_INFO_NAME_SIZE);
        
        ground_spawn.y = mem.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_GROUND_SPAWN_INFO_Y);
        ground_spawn.x = mem.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_GROUND_SPAWN_INFO_X);
        ground_spawn.z = mem.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_GROUND_SPAWN_INFO_Z);

        ground_spawn.distance = everquest_calculate_distance(player_x, player_y, ground_spawn.x, ground_spawn.y);

        ground_spawn.distance_z = abs(ground_spawn.z - player_z);

        ground_spawns.push_back(ground_spawn);

        spawn_info_address = spawn_next_spawn_info;
    }
}

#endif // EQMAC_HPP
