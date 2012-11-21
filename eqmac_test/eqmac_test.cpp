#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include <windows.h>
#include <psapi.h>

#include <boost/algorithm/string.hpp>

#include "memory.hpp"

#include "eqmac.hpp"

memory memory;

int main(int argc, char** argv)
{
    memory.enable_debug_privileges();

    HWND everquest_hwnd = FindWindow(NULL, everquest_title.c_str());

    memory.set_process_by_hwnd(everquest_hwnd);

    std::string zone_short_name = everquest_get_zone_short_name(memory);

    boost::algorithm::to_lower(zone_short_name);

    std::cout << "Zone Short Name: " << zone_short_name << std::endl;

    std::cout << "Zone Long Name: " << everquest_get_zone_long_name(memory) << std::endl;

    std::string player_name = memory.read_string(EVERQUEST_ZONE_INFO_STRUCTURE + EVERQUEST_OFFSET_ZONE_INFO_PLAYER_NAME, 0x40);

    std::cout << "Player Name: " << player_name << std::endl;

    int player_spawn_info = everquest_get_player_spawn_info(memory);

    float player_heading = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING);

    std::cout << "Player Heading: " << player_heading << std::endl;

    int num_spawns = 0;

    int spawn_info_address = player_spawn_info;

    int spawn_next_spawn_info = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER, 4);

    spawn_info_address = spawn_next_spawn_info;

    //spawn_prev_spawn_info = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_PREV_SPAWN_INFO_POINTER, 4);

    //spawn_info_address = spawn_prev_spawn_info;

    for (int i = 0; i < EVERQUEST_SPAWNS_MAX; i++)
    {
        spawn_next_spawn_info = memory.read_bytes(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER, 4);

        if (spawn_next_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            break;
        }

        std::string spawn_name = memory.read_string(spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NAME, 0x40);

        std::cout << "Spawn Name: " << spawn_name << " " << "(" << "0x" << std::hex << spawn_info_address << std::dec << ")" << std::endl;

        num_spawns++;

        spawn_info_address = spawn_next_spawn_info;
    }

    std::cout << "Number of Spawns: " << num_spawns << std::endl;

    memory.close();

    return 0;
}