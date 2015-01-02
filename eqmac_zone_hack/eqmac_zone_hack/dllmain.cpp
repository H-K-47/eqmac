#include <string>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <windows.h>

#include "detours.h"
#pragma comment(lib, "detours.lib")

#include "vk_keys.h"

#include "memory.hpp"

#include "eqmac.hpp"
#include "eqmac_functions.hpp"
#include "eqmac_chat_helper.hpp"

memory mem;

HMODULE module_global;

#define APPLICATION_NAME "EQMac Zone Hack"

const std::string ini_file = "eqmac_zone_hack.ini";

DWORD WINAPI thread_function(LPVOID param)
{
    if (everquest_function_is_in_game() == false)
    {
        FreeLibraryAndExitThread(module_global, 0);
        return 0;
    }

    int zone_id = EVERQUEST_ZONE_ID_POKNOWLEDGE;

    std::string zone_name = "poknowledge";

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);

    bool use_zone_id = pt.get<bool>("Options.UseZoneID", use_zone_id);

    zone_id = pt.get<int>("Zone.ID", zone_id);

    zone_name = pt.get<std::string>("Zone.Name", zone_name);

    if (use_zone_id == true)
    {
        std::stringstream zone_text;
        zone_text << APPLICATION_NAME << " is zoning to Zone ID: " << zone_id;

        everquest_chat_helper_write_chat_text(zone_text.str().c_str());
        Sleep(1000);

        everquest_object_CEverQuest->MoveToZone(zone_id, "repop to home at death", 1, 0);
    }
    else
    {
        std::stringstream zone_text;
        zone_text << APPLICATION_NAME << " is zoning to Zone Name: " << "'" << zone_name << "'";

        everquest_chat_helper_write_chat_text(zone_text.str().c_str());
        Sleep(1000);

        everquest_object_CEverQuest->MoveToZone(zone_name.c_str(), "repop to home at death", 1, 0);
    }

    Sleep(1000);

    // repop at bind point at PoK
    //everquest_object_CEverQuest->MoveToZone(EVERQUEST_ZONE_ID_POKNOWLEDGE, "repop to home at death", 1, 10);

    // gate to PoK
    //char* result = everquest_object_CEverQuest->MoveToZone(EVERQUEST_ZONE_ID_POKNOWLEDGE, "gate spell", 0, 6);

    // teleport to PoK
    //char* result = everquest_object_CEverQuest->MoveToZone("poknowledge", "MSG_TELEPORT_PC", 0, 1);

    FreeLibraryAndExitThread(module_global, 0);
    return 0;
}

DWORD WINAPI thread_load(LPVOID param)
{
    mem.enable_debug_privileges();
    mem.set_process_by_id(GetCurrentProcessId());

    if (mem.get_process_id() == 0)
    {
        FreeLibraryAndExitThread(module_global, 0);
        return 0;
    }

    CreateThread(NULL, 0, &thread_function, NULL, 0, NULL);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    module_global = module;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            everquest_chat_helper_detour_function();

            DisableThreadLibraryCalls(module);
            CreateThread(NULL, 0, &thread_load, NULL, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            everquest_chat_helper_remove_detour();
            break;
    }

    return TRUE;
}
