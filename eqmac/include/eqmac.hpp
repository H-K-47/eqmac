#ifndef EQMAC_HPP
#define EQMAC_HPP

#include <string>
#include <vector>
#include <cmath>

#include <windows.h>

#include "memory.hpp"

const std::string everquest_title = "EQW beta 2.32";

#define EVERQUEST_GRAPHICS_DLL_POINTER 0x007F9C50 // EQGfx_Dx8.DLL

#define EVERQUEST_IS_IN_GAME 0x00798550 // BYTE

#define EVERQUEST_IS_AUTO_ATTACK_ENABLED 0x007F6FFE // BYTE

#define EVERQUEST_ZONE_INFO_STRUCTURE           0x00798784 // STRUCT
#define EVERQUEST_OFFSET_ZONE_INFO_PLAYER_NAME  0x0000 // STRING [0x40]
#define EVERQUEST_OFFSET_ZONE_INFO_SHORT_NAME   0x0040 // STRING [0x20]
#define EVERQUEST_OFFSET_ZONE_INFO_LONG_NAME    0x0060 // STRING [0x80]
#define EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Y 0x01E8 // FLOAT
#define EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_X 0x01EC // FLOAT
#define EVERQUEST_OFFSET_ZONE_INFO_SAFE_POINT_Z 0x01F0 // FLOAT

#define EVERQUEST_WORLD_INFO_POINTER       0x007F9494 // POINTER to STRUCT
#define EVERQUEST_OFFSET_WORLD_INFO_HOUR   0x04 // BYTE
#define EVERQUEST_OFFSET_WORLD_INFO_MINUTE 0x05 // BYTE
#define EVERQUEST_OFFSET_WORLD_INFO_DAY    0x06 // BYTE
#define EVERQUEST_OFFSET_WORLD_INFO_MONTH  0x07 // BYTE
#define EVERQUEST_OFFSET_WORLD_INFO_YEAR   0x08 // BYTE

#define EVERQUEST_ITEMS_INFO_POINTER 0x007F94A0 // POINTER to STRUCT

#define EVERQUEST_CHAR_INFO_POINTER                   0x007F94E8 // POINTER to STRUCT
#define EVERQUEST_OFFSET_CHAR_INFO_NAME               0x0002 // STRING [0x40]
#define EVERQUEST_OFFSET_CHAR_INFO_IS_STUNNED         0x0098 // BYTE
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

#define EVERQUEST_SPAWN_INFO_BEGIN_POINTER 0x007F94E0 // POINTER to STRUCT

#define EVERQUEST_PLAYER_SPAWN_INFO_POINTER   0x007F94CC // POINTER to STRUCT
#define EVERQUEST_TARGET_SPAWN_INFO_POINTER   0x007F94EC // POINTER to STRUCT
#define EVERQUEST_MERCHANT_SPAWN_INFO_POINTER 0x007F94F8 // POINTER to STRUCT

#define EVERQUEST_SPAWN_INFO_NULL 0x00000000

#define EVERQUEST_OFFSET_SPAWN_INFO_NAME                    0x0001 // STRING [0x40]
#define EVERQUEST_OFFSET_SPAWN_INFO_Y                       0x0048 // FLOAT
#define EVERQUEST_OFFSET_SPAWN_INFO_X                       0x004C // FLOAT
#define EVERQUEST_OFFSET_SPAWN_INFO_Z                       0x0050 // FLOAT
#define EVERQUEST_OFFSET_SPAWN_INFO_HEADING                 0x0054 // FLOAT ; facing direction, rotation
#define EVERQUEST_OFFSET_SPAWN_INFO_MOVEMENT_SPEED          0x005C // FLOAT
#define EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER 0x0078 // POINTER to STRUCT
#define EVERQUEST_OFFSET_SPAWN_INFO_PREV_SPAWN_INFO_POINTER 0x007C // POINTER to STRUCT
#define EVERQUEST_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER      0x0084 // POINTER to STRUCT
#define EVERQUEST_OFFSET_SPAWN_INFO_CHAR_INFO_POINTER       0x0088 // POINTER to STRUCT
#define EVERQUEST_OFFSET_SPAWN_INFO_AVATAR_HEIGHT           0x008C // FLOAT
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
#define EVERQUEST_OFFSET_SPAWN_INFO_IS_LINKDEAD             0x0108 // BYTE
#define EVERQUEST_OFFSET_SPAWN_INFO_IS_AFK                  0x0122 // BYTE
#define EVERQUEST_OFFSET_SPAWN_INFO_LAST_NAME               0x012C // STRING [0x20]

#define EVERQUEST_SPAWNS_MAX 4096

#define EVERQUEST_SPAWN_INFO_TYPE_PLAYER 0
#define EVERQUEST_SPAWN_INFO_TYPE_NPC    1
#define EVERQUEST_SPAWN_INFO_TYPE_CORPSE 2

#define EVERQUEST_OFFSET_ACTOR_TIME_STAMP                    0x0058 // DWORD
#define EVERQUEST_OFFSET_ACTOR_TIME_STAMP_LAST_TICK          0x005C // DWORD
#define EVERQUEST_OFFSET_ACTOR_CASTING_SPELL_TIME_STAMP      0x00CC // DWORD
#define EVERQUEST_OFFSET_ACTOR_CASTING_SPELL_TIME_STAMP_EX   0x00E0 // DWORD
#define EVERQUEST_OFFSET_ACTOR_INFO_ANIMATION                0x0184 // BYTE
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_BOTH          0x0260 // DWORD
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_SECONDARY     0x0264 // DWORD
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_PRIMARY       0x0268 // DWORD
#define EVERQUEST_OFFSET_ACTOR_INFO_CASTING_SPELL_ID         0x027C // WORD
#define EVERQUEST_OFFSET_ACTOR_INFO_CASTING_SPELL_GEM_NUMBER 0x027E // BYTE
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_NOT_MOVING            0x032D // BYTE
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_LFG                   0x0438 // BYTE

#define EVERQUEST_CASTING_SPELL_ID_NULL 0xFFFF

#define EVERQUEST_CASTING_SPELL_GEM_NUMBER_SINGING 255

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
#define EVERQUEST_RACE_DRAKKIN   -1

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
#define EVERQUEST_CLASS_BERSERKER    -1

#define EVERQUEST_GUILD_NAMES_BEGIN 0x007F9C94
#define EVERQUEST_GUILD_NAME_SIZE 0x60

#define EVERQUEST_GUILD_NAMES_MAX 512

#define EVERQUEST_GUILD_ID_NULL 0xFFFF // WORD ; = 65535

#define EVERQUEST_STANDING_STATE_STANDING 0x64
#define EVERQUEST_STANDING_STATE_FROZEN   0x66 // mesmerised / feared ; You lose control of yourself!
#define EVERQUEST_STANDING_STATE_LOOTING  0x69 // looting or bind wound
#define EVERQUEST_STANDING_STATE_SITTING  0x6E
#define EVERQUEST_STANDING_STATE_DUCKING  0x6F // crouching
#define EVERQUEST_STANDING_STATE_FEIGNED  0x73 // feign death
#define EVERQUEST_STANDING_STATE_DEAD     0x78

// eqgame.exe+AAAD1 - 74 0C - je eqgame.exe+AAADF
#define EVERQUEST_ASM_PLAY_NICE_JUMP 0x004AAAD1
BYTE EVERQUEST_ASM_PLAY_NICE_JUMP_BYTES[] = {0x74, 0x0C};

// eqgame.exe+AAAD3 - FF 35 68967900 - push [eqgame.exe+399668]
// eqgame.exe+AAAD9 - FF 15 5C315E00 - call dword ptr [eqgame.exe+1E315C] -> kernel32.Sleep
#define EVERQUEST_ASM_PLAY_NICE_SLEEP_TIME 0x00799668 // DWORD ; eqgame.exe+399668

// call eqgame.exe+15833D
#define EVERQUEST_ASM_PLAY_NICE_SLEEP_IS_ENABLED 0x008063D0 // DWORD ; eqgame.exe+4063D0

#define EVERQUEST_FUNCTION_WARP_TO_SAFE_POINT       0x004B459C
#define EVERQUEST_FUNCTION_WARP_TO_SAFE_POINT_VALUE 0x007F9510

typedef void __stdcall _everquest_function_warp_to_safe_point(int value);
static _everquest_function_warp_to_safe_point *everquest_function_warp_to_safe_point = (_everquest_function_warp_to_safe_point *)EVERQUEST_FUNCTION_WARP_TO_SAFE_POINT;

#define EVERQUEST_FUNCTION_DO_HOT_BUTTON 0x004209BD

typedef void __stdcall _everquest_function_do_hot_button(int button_index);
static _everquest_function_do_hot_button *everquest_function_do_hot_button = (_everquest_function_do_hot_button *)EVERQUEST_FUNCTION_DO_HOT_BUTTON;

#define EVERQUEST_FUNCTION_CAST_SPELL 0x00000000

typedef void __stdcall _everquest_function_cast_spell(int gem_index, int spell_id);
static _everquest_function_cast_spell *everquest_function_cast_spell = (_everquest_function_cast_spell *)EVERQUEST_FUNCTION_CAST_SPELL;

#define EVERQUEST_FUNCTION_CHAT_WRITE 0x0000537F99

typedef void __stdcall _everquest_function_chat_write(char* text, int color = 10, int value = 1);
static _everquest_function_chat_write *everquest_function_chat_write = (_everquest_function_chat_write *)EVERQUEST_FUNCTION_CHAT_WRITE;

struct everquest_spawn_t
{
    int address;

    int count;

    int spawn_id;
    int owner_id;
    int guild_id;

    std::string name;
    std::string last_name;

    float x;
    float y;
    float z;

    float distance;
    float distance_z;

    float heading;

    float movement_speed;

    int standing_state;

    int type;
    int level;
    int race;
    int _class; // class is a keyword

    int hp;
    int hp_max;

    bool is_target;
    bool is_player_corpse;

    int is_holding_both;
    int is_holding_secondary;
    int is_holding_primary;
};

float everquest_calculate_distance(float x1, float y1, float x2, float y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

bool everquest_is_in_game(memory &memory)
{
    return memory.read_any<BYTE>(EVERQUEST_IS_IN_GAME);
}

bool everquest_is_auto_attack_enabled(memory &memory)
{
    return memory.read_any<BYTE>(EVERQUEST_IS_AUTO_ATTACK_ENABLED);
}

std::string everquest_get_zone_player_name(memory &memory)
{
    return memory.read_string(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_PLAYER_NAME, 0x40);
}

std::string everquest_get_zone_short_name(memory &memory)
{
    return memory.read_string(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_SHORT_NAME, 0x20);
}

std::string everquest_get_zone_long_name(memory &memory)
{
    return memory.read_string(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_LONG_NAME, 0x80);
}

int everquest_get_char_info(memory &memory)
{
    return memory.read_any<DWORD>(EVERQUEST_CHAR_INFO_POINTER);
}

int everquest_get_player_spawn_info(memory &memory)
{
    return memory.read_any<DWORD>(EVERQUEST_PLAYER_SPAWN_INFO_POINTER);
}

int everquest_get_player_actor_info(memory &memory)
{
    int player_spawn_info = everquest_get_player_spawn_info(memory);

    return memory.read_any<DWORD>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER);
}

int everquest_get_target_spawn_info(memory &memory)
{
    return memory.read_any<DWORD>(EVERQUEST_TARGET_SPAWN_INFO_POINTER);
}

int everquest_get_target_actor_info(memory &memory)
{
    int target_spawn_info = everquest_get_target_spawn_info(memory);

    return memory.read_any<DWORD>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER);
}


int everquest_get_merchant_spawn_info(memory &memory)
{
    return memory.read_any<DWORD>(EVERQUEST_MERCHANT_SPAWN_INFO_POINTER);
}

std::string everquest_get_race_short_name(int race)
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

        case EVERQUEST_RACE_DRAKKIN:
            race_short_name = "DRK";
            break;

        default:
            race_short_name = "UNK";
            break;
    }

    return race_short_name;
}

std::string everquest_get_class_short_name(int _class)
{
    std::string class_short_name;

    switch (_class)
    {
        case EVERQUEST_CLASS_UNKNOWN:
            class_short_name = "UNK";
            break;

        case EVERQUEST_CLASS_WARRIOR:
            class_short_name = "WAR";
            break;

        case EVERQUEST_CLASS_CLERIC:
            class_short_name = "CLR";
            break;

        case EVERQUEST_CLASS_PALADIN:
            class_short_name = "PAL";
            break;

        case EVERQUEST_CLASS_RANGER:
            class_short_name = "RNG";
            break;

        case EVERQUEST_CLASS_SHADOWKNIGHT:
            class_short_name = "SHD";
            break;

        case EVERQUEST_CLASS_DRUID:
            class_short_name = "DRU";
            break;

        case EVERQUEST_CLASS_MONK:
            class_short_name = "MNK";
            break;

        case EVERQUEST_CLASS_BARD:
            class_short_name = "BRD";
            break;

        case EVERQUEST_CLASS_ROGUE:
            class_short_name = "ROG";
            break;

        case EVERQUEST_CLASS_SHAMAN:
            class_short_name = "SHM";
            break;

        case EVERQUEST_CLASS_NECROMANCER:
            class_short_name = "NEC";
            break;

        case EVERQUEST_CLASS_WIZARD:
            class_short_name = "WIZ";
            break;

        case EVERQUEST_CLASS_MAGICIAN:
            class_short_name = "MAG";
            break;

            case EVERQUEST_CLASS_ENCHANTER:
            class_short_name = "ENC";
            break;

        case EVERQUEST_CLASS_BEASTLORD:
            class_short_name = "BST";
            break;

        case EVERQUEST_CLASS_BERSERKER:
            class_short_name = "BER";
            break;

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

std::string everquest_get_guild_name(memory &memory, int guild_id)
{
    if (guild_id == EVERQUEST_GUILD_ID_NULL)
    {
        return "No Guild";
    }

    int guild_name_address = EVERQUEST_GUILD_NAMES_BEGIN;

    for (int i = 0; i < EVERQUEST_GUILD_NAMES_MAX; i++)
    {
        if (i == guild_id)
        {
            std::string guild_name = memory.read_string(guild_name_address, EVERQUEST_GUILD_NAME_SIZE);

            return guild_name;
        }

        guild_name_address = guild_name_address + EVERQUEST_GUILD_NAME_SIZE;
    }

    return "Unknown Guild";
}

WORD everquest_get_player_casting_spell_id(memory &memory)
{
    int player_actor_info = everquest_get_player_actor_info(memory);

    return memory.read_any<WORD>(player_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_CASTING_SPELL_ID);
}

BYTE everquest_get_player_casting_spell_gem_number(memory &memory)
{
    int player_actor_info = everquest_get_player_actor_info(memory);

    return memory.read_any<BYTE>(player_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_CASTING_SPELL_GEM_NUMBER);
}

void everquest_update_spawns(memory &memory, std::vector<everquest_spawn_t> &everquest_spawns)
{
    everquest_spawns.clear();

    int player_spawn_info = everquest_get_player_spawn_info(memory);
    int target_spawn_info = everquest_get_target_spawn_info(memory);

    float player_y = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
    float player_x = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);
    float player_z = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z);

    int spawn_info_address = player_spawn_info;

    int spawn_next_spawn_info = memory.read_any<DWORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER);

    spawn_info_address = spawn_next_spawn_info;

    for (int i = 0; i < EVERQUEST_SPAWNS_MAX; i++)
    {
        spawn_next_spawn_info = memory.read_any<DWORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER);

        if (spawn_next_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            break;
        }

        int spawn_actor_info = memory.read_any<DWORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER);

        everquest_spawn_t everquest_spawn;

        everquest_spawn.address = spawn_info_address;

        everquest_spawn.spawn_id = memory.read_any<WORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_SPAWN_ID);
        everquest_spawn.owner_id = memory.read_any<WORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_OWNER_ID);
        everquest_spawn.guild_id = memory.read_any<WORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_GUILD_ID);

        everquest_spawn.name      = memory.read_string(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NAME,      0x40);
        everquest_spawn.last_name = memory.read_string(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_LAST_NAME, 0x20);

        everquest_spawn.y = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Y);
        everquest_spawn.x = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_X);
        everquest_spawn.z = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Z);

        everquest_spawn.distance = everquest_calculate_distance(player_x, player_y, everquest_spawn.x, everquest_spawn.y);

        everquest_spawn.distance_z = everquest_spawn.z - player_z;

        everquest_spawn.heading = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HEADING);

        everquest_spawn.movement_speed = memory.read_any<float>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_MOVEMENT_SPEED);

        everquest_spawn.standing_state = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE);

        everquest_spawn.type   = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_TYPE);
        everquest_spawn.level  = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_LEVEL);
        everquest_spawn.race   = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_RACE);
        everquest_spawn._class = memory.read_any<BYTE>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_CLASS);

        everquest_spawn.hp     = memory.read_any<DWORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HP_CURRENT);
        everquest_spawn.hp_max = memory.read_any<DWORD>(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HP_MAX);

        everquest_spawn.is_target = (spawn_info_address == target_spawn_info ? true : false);

        everquest_spawn.is_player_corpse = false;

        if (everquest_spawn.type == EVERQUEST_SPAWN_INFO_TYPE_CORPSE)
        {
            std::string player_name = memory.read_string(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_NAME, 0x40);

            std::size_t found = everquest_spawn.name.find(player_name);

            if (found != std::string::npos)
            {
                everquest_spawn.is_player_corpse = true;
            }
        }

        everquest_spawn.is_holding_both      = memory.read_any<DWORD>(spawn_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_BOTH);
        everquest_spawn.is_holding_secondary = memory.read_any<DWORD>(spawn_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_SECONDARY);
        everquest_spawn.is_holding_primary   = memory.read_any<DWORD>(spawn_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_PRIMARY);

        everquest_spawns.push_back(everquest_spawn);

        spawn_info_address = spawn_next_spawn_info;
    }
}

#endif // EQMAC_HPP
