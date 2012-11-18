#ifndef EQMAC_HPP
#define EQMAC_HPP

const std::string everquest_title = "EQW beta 2.32";

#define EVERQUEST_ZONE_INFO_STRUCTURE          0x00798784 // STRUCT
#define EVERQUEST_OFFSET_ZONE_INFO_PLAYER_NAME 0x000 // STRING [0x40]
#define EVERQUEST_OFFSET_ZONE_INFO_SHORT_NAME  0x040 // STRING [0x20]
#define EVERQUEST_OFFSET_ZONE_INFO_LONG_NAME   0x060 // STRING [0x80]

#define EVERQUEST_CHAR_INFO_POINTER               0x007F94E8 // POINTER to STRUCT
#define EVERQUEST_OFFSET_CHAR_INFO_NAME           0x0002 // STRING [0x40]
#define EVERQUEST_OFFSET_CHAR_INFO_STANDING_STATE 0x0B64 // BYTE ; EVERQUEST_STANDING_STATE_x

#define EVERQUEST_PLAYER_SPAWN_INFO_POINTER   0x007F94CC // POINTER to STRUCT
#define EVERQUEST_TARGET_SPAWN_INFO_POINTER   0x007F94EC // POINTER to STRUCT
#define EVERQUEST_MERCHANT_SPAWN_INFO_POINTER 0x007F94F8 // POINTER to STRUCT

#define EVERQUEST_SPAWN_INFO_NULL 0x00000000

#define EVERQUEST_OFFSET_SPAWN_INFO_NAME                    0x0001 // STRING [0x40]
#define EVERQUEST_OFFSET_SPAWN_INFO_LAST_NAME               0xFFFF // STRING [0x20] ; title for NPCs
#define EVERQUEST_OFFSET_SPAWN_INFO_Y                       0x0048 // FLOAT
#define EVERQUEST_OFFSET_SPAWN_INFO_X                       0x004C // FLOAT
#define EVERQUEST_OFFSET_SPAWN_INFO_Z                       0x0050 // FLOAT
#define EVERQUEST_OFFSET_SPAWN_INFO_HEADING                 0x0054 // FLOAT ; facing direction, rotation
#define EVERQUEST_OFFSET_SPAWN_INFO_MOVEMENT_SPEED          0x005C // FLOAT
#define EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER 0x0078 // POINTER to STRUCT
#define EVERQUEST_OFFSET_SPAWN_INFO_PREV_SPAWN_INFO_POINTER 0x007C // POINTER to STRUCT
#define EVERQUEST_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER      0x0084 // POINTER to STRUCT
#define EVERQUEST_OFFSET_SPAWN_INFO_CHAR_INFO_POINTER       0x0088 // POINTER to STRUCT
#define EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE          0x00B1 // BYTE ; EVERQUEST_STANDING_STATE_x

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

int everquest_get_target_spawn_info(memory &memory)
{
    return memory.read_bytes(EVERQUEST_TARGET_SPAWN_INFO_POINTER, 4);
}

int everquest_get_merchant_spawn_info(memory &memory)
{
    return memory.read_bytes(EVERQUEST_MERCHANT_SPAWN_INFO_POINTER, 4);
}

#endif // EQMAC_HPP
