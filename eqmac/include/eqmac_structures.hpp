#ifndef EQMAC_STRUCTURES_HPP
#define EQMAC_STRUCTURES_HPP

#include <string>
#include <algorithm>

struct everquest_item_t
{
    int loot_window_slot;

    int address;

    std::string name;
    std::string lore_name;
    std::string id_file;

    float weight;

    bool is_no_rent;
    bool is_no_drop;

    unsigned char size;

    bool is_container;

    unsigned short id;
    unsigned short icon;

    int equip_slot;
    int equippable_slots;

    int cost;
};

struct everquest_spawn_t
{
    int address;

    int count;

    unsigned short spawn_id;
    unsigned short owner_id;
    unsigned short guild_id;

    std::string name;
    std::string last_name;

    float y;
    float x;
    float z;

    float distance;
    float distance_z;

    float heading;

    float movement_speed;

    unsigned char standing_state;

    unsigned char type;
    unsigned char level;
    unsigned char race;
    unsigned char _class; // class is a keyword

    int hp;
    int hp_max;

    bool is_target;
    bool is_player_corpse;

    int is_holding_both;
    int is_holding_secondary;
    int is_holding_primary;
};

struct everquest_ground_spawn_t
{
    int address;

    unsigned short id;
    unsigned short drop_id;

    std::string name;

    float y;
    float x;
    float z;

    float distance;
    float distance_z;
};

struct everquest_sort_spawns_by_distance_t
{
    bool operator() (const everquest_spawn_t& everquest_spawn1, const everquest_spawn_t& everquest_spawn2)
    {
        return everquest_spawn1.distance < everquest_spawn2.distance;
    }
};

#endif // EQMAC_STRUCTURES_HPP
