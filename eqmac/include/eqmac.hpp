#ifndef EQMAC_HPP
#define EQMAC_HPP

#include <string>

#include "memory.hpp"

const std::string everquest_title = "EQW beta 2.32";

#define EVERQUEST_ZONE_INFO_STRUCTURE          0x00798784 // STRUCT
#define EVERQUEST_OFFSET_ZONE_INFO_PLAYER_NAME 0x000 // STRING [0x40]
#define EVERQUEST_OFFSET_ZONE_INFO_SHORT_NAME  0x040 // STRING [0x20]
#define EVERQUEST_OFFSET_ZONE_INFO_LONG_NAME   0x060 // STRING [0x80]

#define EVERQUEST_CHAR_INFO_POINTER                   0x007F94E8 // POINTER to STRUCT
#define EVERQUEST_OFFSET_CHAR_INFO_NAME               0x0002 // STRING [0x40]
#define EVERQUEST_OFFSET_CHAR_INFO_IS_STUNNED         0x0098 // BYTE
#define EVERQUEST_OFFSET_CHAR_INFO_STANDING_STATE     0x0B64 // BYTE ; EVERQUEST_STANDING_STATE_x
#define EVERQUEST_OFFSET_CHAR_INFO_SPAWN_INFO_POINTER 0x0D74 // POINTER to STRUCT

#define EVERQUEST_SPAWN_INFO_BEGIN_POINTER 0x00000000 // POINTER to STRUCT

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
#define EVERQUEST_OFFSET_SPAWN_INFO_TYPE                    0x00A8 // BYTE ; EVERQUEST_SPAWN_INFO_TYPE_x
#define EVERQUEST_OFFSET_SPAWN_INFO_CLASS                   0x00A9 // BYTE ; EVERQUEST_CLASS_x
#define EVERQUEST_OFFSET_SPAWN_INFO_RACE                    0x00AA // BYTE ; EVERQUEST_RACE_x
#define EVERQUEST_OFFSET_SPAWN_INFO_LEVEL                   0x00AD // DWORD
#define EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE          0x00B1 // BYTE ; EVERQUEST_STANDING_STATE_x
#define EVERQUEST_OFFSET_SPAWN_INFO_HP_MAX                  0x0098 // DWORD
#define EVERQUEST_OFFSET_SPAWN_INFO_HP_CURRENT              0x009C // DWORD
#define EVERQUEST_OFFSET_SPAWN_INFO_IS_LINKDEAD             0x0108 // BYTE
#define EVERQUEST_OFFSET_SPAWN_INFO_IS_AFK                  0x0122 // BYTE
#define EVERQUEST_OFFSET_SPAWN_INFO_LAST_NAME               0x012C // STRING [0x20]

#define EVERQUEST_SPAWNS_MAX 4096

#define EVERQUEST_SPAWN_INFO_TYPE_PLAYER 0
#define EVERQUEST_SPAWN_INFO_TYPE_NPC    1
#define EVERQUEST_SPAWN_INFO_TYPE_CORPSE 2

#define EVERQUEST_OFFSET_ACTOR_INFO_ANIMATION            0x0184 // BYTE
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_BOTH      0x0260 // DWORD
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_SECONDARY 0x0264 // DWORD
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_HOLDING_PRIMARY   0x0268 // DWORD
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_NOT_MOVING        0x032D // BYTE
#define EVERQUEST_OFFSET_ACTOR_INFO_IS_LFG               0x0438 // BYTE

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
#define EVERQUEST_CLASS_BERSERKER    16

#define EVERQUEST_FUNCTION_WARP_TO_SAFE_POINT       0x004B459C
#define EVERQUEST_FUNCTION_WARP_TO_SAFE_POINT_VALUE 0x007F9510

//typedef void __stdcall _everquest_function_warp_to_safe_point(int value);
//static _everquest_function_warp_to_safe_point *everquest_function_warp_to_safe_point = (_everquest_function_warp_to_safe_point *)EVERQUEST_FUNCTION_WARP_TO_SAFE_POINT;

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
    return memory.read_bytes(EVERQUEST_CHAR_INFO_POINTER, 4);
}

int everquest_get_player_spawn_info(memory &memory)
{
    return memory.read_bytes(EVERQUEST_PLAYER_SPAWN_INFO_POINTER, 4);
}

int everquest_get_player_actor_info(memory &memory)
{
    int player_spawn_info = everquest_get_player_spawn_info(memory);

    return memory.read_bytes(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER, 4);
}

int everquest_get_target_spawn_info(memory &memory)
{
    return memory.read_bytes(EVERQUEST_TARGET_SPAWN_INFO_POINTER, 4);
}

int everquest_get_target_actor_info(memory &memory)
{
    int target_spawn_info = everquest_get_target_spawn_info(memory);

    return memory.read_bytes(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER, 4);
}


int everquest_get_merchant_spawn_info(memory &memory)
{
    return memory.read_bytes(EVERQUEST_MERCHANT_SPAWN_INFO_POINTER, 4);
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

        default:
            class_short_name = "UNK";
            break;
    }

    return class_short_name;
}

#endif // EQMAC_HPP
