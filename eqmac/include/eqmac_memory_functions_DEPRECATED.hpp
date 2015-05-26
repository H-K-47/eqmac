#ifndef EQMAC_MEMORY_FUNCTIONS_HPP
#define EQMAC_MEMORY_FUNCTIONS_HPP

#include <cstdint>
#include <cstring>
#include <cmath>

#include <string>

#include <windows.h>

#include "cmemory.hpp"

bool EQ_IsInGame(CMemory* mem)
{
    BYTE result = mem->ReadAny<BYTE>(EQ_IS_IN_GAME);

    return (result != 0);
}

bool EQ_IsAutoAttackEnabled(CMemory* mem)
{
    BYTE result = mem->ReadAny<BYTE>(EQ_IS_AUTO_ATTACK_ENABLED);

    return (result != 0);
}

int EQ_GetZoneId(CMemory* mem)
{
    return mem->ReadAny<DWORD>(EQ_ZONE_ID);
}

EQCHARINFO EQ_GetCharInfo(CMemory* mem)
{
    return mem->ReadStructFromPointer<EQCHARINFO>(EQ_POINTER_CHAR_INFO);
}

std::string EQ_GetGuildNameById(CMemory* mem, int guildId)
{
    if (guildId == EQ_GUILD_ID_NULL)
    {
        return "Unknown Guild";
    }

    EQGUILDLIST guildList = mem->ReadAny<EQGUILDLIST>(EQ_STRUCTURE_GUILD_LIST);

    return guildList.Guild[guildId].Name;
}

#endif // EQMAC_MEMORY_FUNCTIONS_HPP
