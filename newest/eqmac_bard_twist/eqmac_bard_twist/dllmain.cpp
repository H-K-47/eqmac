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

#define APPLICATION_NAME "EQMac Bard Twist"

#define BARD_TWIST_MAX_SONGS 3

const std::string ini_file = "eqmac_bard_twist.ini";

bool bard_twist_activated = false;

bool bard_twist_duck_to_deactivate = true;

int bard_twist_number_of_songs = 3;

unsigned short bard_twist_song_1_hot_button = 1;
unsigned short bard_twist_song_2_hot_button = 2;
unsigned short bard_twist_song_3_hot_button = 3;

float bard_twist_song_1_cast_time = 3.0;
float bard_twist_song_2_cast_time = 3.0;
float bard_twist_song_3_cast_time = 3.0;

void load_config()
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);

    bard_twist_duck_to_deactivate = pt.get<bool>("BardTwist.bDuckToDeactivate", bard_twist_duck_to_deactivate);

    bard_twist_number_of_songs = pt.get<int>("BardTwist.iNumberOfSongs", bard_twist_number_of_songs);

    bard_twist_song_1_hot_button = pt.get<unsigned short>("BardTwist.iSong1HotButton", bard_twist_song_1_hot_button);
    bard_twist_song_2_hot_button = pt.get<unsigned short>("BardTwist.iSong2HotButton", bard_twist_song_2_hot_button);
    bard_twist_song_3_hot_button = pt.get<unsigned short>("BardTwist.iSong3HotButton", bard_twist_song_3_hot_button);

    bard_twist_song_1_cast_time = pt.get<float>("BardTwist.fSong1CastTime", bard_twist_song_1_cast_time);
    bard_twist_song_2_cast_time = pt.get<float>("BardTwist.fSong2CastTime", bard_twist_song_2_cast_time);
    bard_twist_song_3_cast_time = pt.get<float>("BardTwist.fSong3CastTime", bard_twist_song_3_cast_time);
}

bool check_if_player_is_ducking_to_deactivate()
{
    if (bard_twist_activated == false)
    {
        return true;
    }

    unsigned char player_standing_state = everquest_function_get_player_position();

    if (player_standing_state == EVERQUEST_STANDING_STATE_DUCKING)
    {
        bard_twist_activated = false;

        everquest_chat_helper_write_chat_text(APPLICATION_NAME " deactivated.");

        return true;
    }

    return false;
}

bool is_player_sitting()
{
    unsigned char player_standing_state = everquest_function_get_player_position();

    if (player_standing_state == EVERQUEST_STANDING_STATE_SITTING)
    {
        return true;
    }

    return false;
}

void sleep_time(float time)
{
    Sleep((DWORD)(time * 1000));
}

bool twist_hot_button(int hot_button)
{
    if (bard_twist_duck_to_deactivate == true)
    {
        if (check_if_player_is_ducking_to_deactivate() == true)
        {
            return true;
        }
    }

    unsigned short player_casting_spell_id = everquest_get_player_casting_spell_id(mem);

    unsigned char player_casting_spell_gem_number = everquest_get_player_casting_spell_gem_number(mem);

    if (player_casting_spell_id != EVERQUEST_CASTING_SPELL_ID_NULL && player_casting_spell_gem_number != EVERQUEST_CASTING_SPELL_GEM_NUMBER_SINGING)
    {
        return false;
    }

    int num_times_to_do_hot_button = 1;

    if (player_casting_spell_gem_number == EVERQUEST_CASTING_SPELL_GEM_NUMBER_SINGING)
    {
        num_times_to_do_hot_button = 2;
    }

    for (int i = 0; i < num_times_to_do_hot_button; i++)
    {
        everquest_function_do_hot_button(hot_button);
        Sleep(100);
    }

    Sleep(250);

    player_casting_spell_id = everquest_get_player_casting_spell_id(mem);

    if (player_casting_spell_id == EVERQUEST_CASTING_SPELL_ID_NULL)
    {
        return false;
    }

    return true;
}

DWORD WINAPI thread_function(LPVOID param)
{
    everquest_chat_helper_write_chat_text(APPLICATION_NAME " loaded.");
    Sleep(1000);

    while (1)
    {
        if (everquest_function_is_in_game() == false)
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
                //break;
            }

            if
            (
                (KEYDOWN(VK_CONTROL)) &&
                (KEYDOWN(VK_ALT)) &&
                (KEYDOWN(VK_B))
            )
            {
                int player_spawn_info = everquest_function_get_player_spawn_info();

                unsigned char player_class = everquest_function_read_memory<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_CLASS);

                if (player_class != EVERQUEST_CLASS_BARD)
                {
                    everquest_chat_helper_write_chat_text(APPLICATION_NAME " Error: You are NOT a Bard.");
                    Sleep(1000);
                    continue;
                }

                everquest_toggle_bool(bard_twist_activated);

                if (bard_twist_activated == true)
                {
                    everquest_chat_helper_write_chat_text(APPLICATION_NAME " activated.");

                    std::stringstream bard_twist_text;
                    bard_twist_text.precision(2);

                    bard_twist_text << "Number of Songs: " << bard_twist_number_of_songs;

                    everquest_chat_helper_write_chat_text(bard_twist_text.str().c_str());

                    bard_twist_text.str("");

                    bard_twist_text << "Hot Buttons: "
                        << bard_twist_song_1_hot_button << " (" << bard_twist_song_1_cast_time << " sec)" << ", "
                        << bard_twist_song_2_hot_button << " (" << bard_twist_song_2_cast_time << " sec)" << ", "
                        << bard_twist_song_3_hot_button << " (" << bard_twist_song_3_cast_time << " sec)";

                    everquest_chat_helper_write_chat_text(bard_twist_text.str().c_str());

                    bard_twist_text.str("");
                }
                else
                {
                    everquest_chat_helper_write_chat_text(APPLICATION_NAME " deactivated.");
                }

                Sleep(1000);
            }
        }

        if (bard_twist_activated == false)
        {
            Sleep(10);
            continue;
        }

        if (is_player_sitting() == true)
        {
            Sleep(10);
            continue;
        }

        if (bard_twist_duck_to_deactivate == true)
        {
            if (check_if_player_is_ducking_to_deactivate() == true)
            {
                Sleep(10);
                continue;
            }
        }

        if (bard_twist_number_of_songs <= 0 || bard_twist_number_of_songs > BARD_TWIST_MAX_SONGS)
        {
            Sleep(10);
            continue;
        }

        bool result = false;

        while (result == false)
        {
            if (is_player_sitting() == true)
            {
                break;
            }

            if (bard_twist_duck_to_deactivate == true)
            {
                if (check_if_player_is_ducking_to_deactivate() == true)
                {
                    break;
                }
            }

            result = twist_hot_button(bard_twist_song_1_hot_button);

            Sleep(1);
        }

        if (bard_twist_activated == false)
        {
            Sleep(10);
            continue;
        }

        sleep_time(bard_twist_song_1_cast_time);

        if (bard_twist_number_of_songs > 1)
        {
            result = false;

            while (result == false)
            {
                if (is_player_sitting() == true)
                {
                    break;
                }

                if (bard_twist_duck_to_deactivate == true)
                {
                    if (check_if_player_is_ducking_to_deactivate() == true)
                    {
                        break;
                    }
                }

                result = twist_hot_button(bard_twist_song_2_hot_button);

                Sleep(1);
            }

            if (bard_twist_activated == false)
            {
                Sleep(10);
                continue;
            }

            sleep_time(bard_twist_song_2_cast_time);
        }

        if (bard_twist_number_of_songs > 2)
        {
            result = false;

            while (result == false)
            {
                if (is_player_sitting() == true)
                {
                    break;
                }

                if (bard_twist_duck_to_deactivate == true)
                {
                    if (check_if_player_is_ducking_to_deactivate() == true)
                    {
                        break;
                    }
                }

                result = twist_hot_button(bard_twist_song_3_hot_button);

                Sleep(1);
            }

            if (bard_twist_activated == false)
            {
                Sleep(10);
                continue;
            }

            sleep_time(bard_twist_song_3_cast_time);
        }

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
