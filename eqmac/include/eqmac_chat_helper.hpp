#ifndef EQMAC_CHAT_HELPER_HPP
#define EQMAC_CHAT_HELPER_HPP

#include "eqmac.hpp"
#include "eqmac_functions.hpp"

bool everquest_chat_helper_write_chat_text_is_enabled = false;

std::string everquest_chat_helper_chat_text_to_write;

void everquest_chat_helper_write_chat_text(const char *text)
{
    if (everquest_chat_helper_write_chat_text_is_enabled == true)
    {
        while (everquest_chat_helper_write_chat_text_is_enabled != false)
        {
            Sleep(1);
        }
    }

    everquest_chat_helper_chat_text_to_write = text;

    everquest_chat_helper_write_chat_text_is_enabled = true;

    //Sleep(100);
}

EVERQUEST_FUNCTION_TYPE_DrawNetStatus Real_DrawNetStatus = NULL;

int __cdecl My_DrawNetStatus(int a1, unsigned short a2, unsigned short a3, unsigned short a4, unsigned short a5, int a6, unsigned short a7, unsigned long a8, long a9, unsigned long a10)
{
    while (everquest_chat_helper_write_chat_text_is_enabled == true)
    {
        everquest_object_CEverQuest->dsp_chat(everquest_chat_helper_chat_text_to_write.c_str());

        everquest_chat_helper_write_chat_text_is_enabled = false;
    }

    return Real_DrawNetStatus(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

void everquest_chat_helper_detour_function()
{
    Real_DrawNetStatus = (EVERQUEST_FUNCTION_TYPE_DrawNetStatus)DetourFunction((PBYTE)EVERQUEST_FUNCTION_DrawNetStatus, (PBYTE)My_DrawNetStatus);
}

void everquest_chat_helper_remove_detour()
{
    DetourRemove((PBYTE)Real_DrawNetStatus, (PBYTE)My_DrawNetStatus);
}

#endif // EQMAC_CHAT_HELPER_HPP
