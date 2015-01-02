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

#define APPLICATION_NAME "EQMac Skill Up"

const std::string ini_file = "eqmac_skill_up.ini";

bool skill_up_activated = false;

int skill_up_text_time_current  = 0;
int skill_up_text_time_previous = 0;

int skill_up_hot_button_time_current  = 0;
int skill_up_hot_button_time_previous = 0;

unsigned short skill_up_hot_button = 1;

bool skill_up_sit_and_stand_to_meditate = true;

float skill_up_mana_percent_to_sit   = 40;
float skill_up_mana_percent_to_stand = 100;

void load_config()
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);

    skill_up_hot_button = pt.get<unsigned short>("SkillUp.iHotButton", skill_up_hot_button);

    skill_up_sit_and_stand_to_meditate = pt.get<bool>("SkillUp.bSitAndStandToMeditate", skill_up_sit_and_stand_to_meditate);

    skill_up_mana_percent_to_sit   = pt.get<float>("SkillUp.fManaPercentToSit",   skill_up_mana_percent_to_sit);
    skill_up_mana_percent_to_stand = pt.get<float>("SkillUp.fManaPercentToStand", skill_up_mana_percent_to_stand);
}

DWORD WINAPI thread_function(LPVOID param)
{
    everquest_chat_helper_write_chat_text(APPLICATION_NAME " loaded.");
    Sleep(1000);

    while (1)
    {
        if (everquest_is_in_game(mem) == false)
        {
            Sleep(100);
            continue;
        }

        if (mem.is_foreground_window_current_process_id() == true)
        {
            if
            (
                (KEYDOWN(VK_CONTROL)) &&
                (KEYDOWN(VK_ALT))     &&
                (KEYDOWN(VK_BACK))
            )
            {
                break;
            }

            if
            (
                (KEYDOWN(VK_CONTROL)) &&
                (KEYDOWN(VK_ALT)) &&
                (KEYDOWN(VK_Z))
            )
            {
                everquest_toggle_bool(skill_up_activated);

                if (skill_up_activated == true)
                {
                    everquest_chat_helper_write_chat_text(APPLICATION_NAME " activated.");
                }
                else
                {
                    everquest_chat_helper_write_chat_text(APPLICATION_NAME " deactivated.");
                }

                Sleep(1000);
            }
        }

        if (skill_up_activated == false)
        {
            Sleep(100);
            continue;
        }

        if (skill_up_hot_button < 0)
        {
            Sleep(100);
            continue;
        }

        if (skill_up_sit_and_stand_to_meditate == true)
        {
            unsigned short player_mana_current = everquest_function_get_player_mana_current();
            unsigned short player_mana_max     = everquest_function_get_player_mana_max();

            float player_mana_percent = everquest_function_get_player_mana_percent();

            skill_up_text_time_current = everquest_function_get_CDisplay_timer();

            if ((skill_up_text_time_current - skill_up_text_time_previous) < 10000)
            {
                //
            }
            else
            {
                std::stringstream skill_up_text;
                skill_up_text.precision(0);
                skill_up_text << "Mana: " << player_mana_current << " / " << player_mana_max << " " << "(" << player_mana_percent << "%" << ")";

                everquest_chat_helper_write_chat_text(skill_up_text.str().c_str());

                skill_up_text_time_previous = skill_up_text_time_current;
            }

            if (player_mana_percent >= skill_up_mana_percent_to_stand)
            {
                everquest_function_set_player_position(EVERQUEST_STANDING_STATE_STANDING);
            }

            if (player_mana_percent <= skill_up_mana_percent_to_sit)
            {
                everquest_function_set_player_position(EVERQUEST_STANDING_STATE_SITTING);
            }
        }
        else
        {
            everquest_function_set_player_position(EVERQUEST_STANDING_STATE_STANDING);
        }

        if (skill_up_sit_and_stand_to_meditate == true)
        {
            if (everquest_function_get_player_position() == EVERQUEST_STANDING_STATE_SITTING)
            {
                Sleep(100);
                continue;
            }
        }

        int player_actor_info = everquest_get_player_actor_info(mem);

        unsigned short player_casting_spell_id = mem.read_any<WORD>(player_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_CASTING_SPELL_ID);

        if (player_casting_spell_id != EVERQUEST_CASTING_SPELL_ID_NULL)
        {
            Sleep(100);
            continue;
        }

        skill_up_hot_button_time_current = everquest_function_get_CDisplay_timer();

        if ((skill_up_hot_button_time_current - skill_up_hot_button_time_previous) < 1000)
        {
            Sleep(100);
            continue;
        }

        everquest_function_do_hot_button(skill_up_hot_button);

        skill_up_hot_button_time_previous = skill_up_hot_button_time_current;

        Sleep(10);
    }

    everquest_chat_helper_write_chat_text(APPLICATION_NAME " unloaded.");
    Sleep(1000);

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

    load_config();

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
