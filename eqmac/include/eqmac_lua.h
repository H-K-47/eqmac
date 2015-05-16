#ifndef EQMAC_LUA_H
#define EQMAC_LUA_H

#include <cstdint>
#include <cstring>
#include <cmath>

#include "lua.hpp"

#include "eqmac.h"
#include "eqmac_functions.h"

lua_State* EQ_LUA_STATE;

void EQ_LUA_Log(const char* text)
{
    FILE* file;
    errno_t fileErrorNumber = fopen_s(&file, "eqmac_mq-lua.log", "a");

    if (file)
    {
        fprintf(file, "[%s] %s\n", __TIME__, text);
        fclose(file);
    }
}

static int EQ_LUA_IsNotTypingInChat(lua_State* L)
{
    bool result = EQ_IsNotTypingInChat();

    lua_pushboolean(L, result);

    return 1;
}

static int EQ_LUA_WriteStringToChat(lua_State* L)
{
    const char* text = luaL_checkstring(L, 1);

    if (text == NULL || strlen(text) == 0)
    {
        return 0;
    }

    EQ_WriteStringToChat(text);

    return 0;
}

static int EQ_LUA_WriteStringToChatWithColor(lua_State* L)
{
    const char* text = luaL_checkstring(L, 1);

    int color = luaL_checkinteger(L, 2);

    if (text == NULL || strlen(text) == 0)
    {
        return 0;
    }

    EQ_WriteStringToChatWithColor(text, color);

    return 0;
}

static int EQ_LUA_DrawText(lua_State* L)
{
    const char* text = luaL_checkstring(L, 1);

    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    int color = luaL_checkinteger(L, 4);

    if (text == NULL || strlen(text) == 0)
    {
        return 0;
    }

    EQ_DrawText(text, x, y, color);

    return 0;
}

static int EQ_LUA_SetMousePosition(lua_State* L)
{
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);

    EQ_SetMousePosition(x, y);

    return 0;
}

static int EQ_LUA_InterpretCommand(lua_State* L)
{
    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    if (playerSpawn == NULL)
    {
        return 0;
    }

    const char* text = luaL_checkstring(L, 1);

    if (text == NULL || strlen(text) == 0)
    {
        return 0;
    }

    EQ_CLASS_CEverQuest->InterpretCmd((EQPlayer*)playerSpawn, (char*)text);

    return 0;
}

static int EQ_LUA_GetCharInfo(lua_State* L)
{
    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    if (charInfo == NULL)
    {
        return 0;
    }

    lua_newtable(L);

    lua_pushstring(L, "Platinum");
    lua_pushinteger(L, charInfo->Platinum);
    lua_settable(L, -3);

    lua_pushstring(L, "Gold");
    lua_pushinteger(L, charInfo->Gold);
    lua_settable(L, -3);

    lua_pushstring(L, "Silver");
    lua_pushinteger(L, charInfo->Silver);
    lua_settable(L, -3);

    lua_pushstring(L, "Copper");
    lua_pushinteger(L, charInfo->Copper);
    lua_settable(L, -3);

    return 1;
}

bool EQ_LUA_GetSpawnInfo(lua_State* L, PEQSPAWNINFO spawnInfo)
{
    if (spawnInfo == NULL)
    {
        return false;
    }

    lua_newtable(L);

    lua_pushstring(L, "Name");
    lua_pushstring(L, spawnInfo->Name);
    lua_settable(L, -3);

    lua_pushstring(L, "Level");
    lua_pushinteger(L, spawnInfo->Level);
    lua_settable(L, -3);

    return true;
}

static int EQ_LUA_GetPlayerSpawnInfo(lua_State* L)
{
    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    if (playerSpawn == NULL)
    {
        return 0;
    }

    if (EQ_LUA_GetSpawnInfo(L, playerSpawn) == false)
    {
        return 0;
    }

    return 1;
}

static int EQ_LUA_GetTargetSpawnInfo(lua_State* L)
{
    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    if (targetSpawn == NULL)
    {
        return 0;
    }

    if (EQ_LUA_GetSpawnInfo(L, targetSpawn) == false)
    {
        return 0;
    }

    return 1;
}

bool EQ_LUA_DoScript(const char* filename)
{
    if (EQ_LUA_STATE == NULL)
    {
        return false;
    }

    int error = luaL_dofile(EQ_LUA_STATE, filename);

    if (error != 0)
    {
        EQ_LUA_Log(lua_tostring(EQ_LUA_STATE, -1));

        if (EQ_IsInGame() == true)
        {
            EQ_WriteStringVarToChat("Script Error", (char*)lua_tostring(EQ_LUA_STATE, -1));
        }

        lua_pop(EQ_LUA_STATE, 1);

        return false;
    }

    return true;
}

bool EQ_LUA_DoScriptFunction(lua_State* L, const char* filename, const char* functionName)
{
    if (L == NULL)
    {
        return false;
    }

    int error = luaL_dofile(L, filename);

    if (error != 0)
    {
        EQ_LUA_Log(lua_tostring(L, -1));

        if (EQ_IsInGame() == true)
        {
            EQ_WriteStringVarToChat("Script Error", (char*)lua_tostring(L, -1));
        }

        lua_pop(L, 1);

        return false;
    }

    lua_getglobal(L, functionName);

    if (!lua_isnil(L, -1))
    {
        int error = lua_pcall(L, 0, LUA_MULTRET, 0);

        if (error != 0)
        {
            EQ_LUA_Log(lua_tostring(L, -1));

            if (EQ_IsInGame() == true)
            {
                EQ_WriteStringVarToChat("Script Error", (char*)lua_tostring(L, -1));
            }

            lua_pop(L, 1);

            return false;
        }
    }

    return true;
}

bool EQ_LUA_DoString(const char* text)
{
    if (EQ_LUA_STATE == NULL)
    {
        return false;
    }

    int error = luaL_dostring(EQ_LUA_STATE, text);

    if (error != 0)
    {
        EQ_LUA_Log(lua_tostring(EQ_LUA_STATE, -1));

        if (EQ_IsInGame() == true)
        {
            EQ_WriteStringVarToChat("Lua Error", (char*)lua_tostring(EQ_LUA_STATE, -1));
        }

        lua_pop(EQ_LUA_STATE, 1);

        return false;
    }

    return true;
}

static const struct luaL_Reg EQ_LUA_LIBRARY[] =
{
    {"IsNotTypingInChat",          EQ_LUA_IsNotTypingInChat},
    {"WriteStringToChat",          EQ_LUA_WriteStringToChat},
    {"WriteStringToChatWithColor", EQ_LUA_WriteStringToChatWithColor},
    {"DrawText",                   EQ_LUA_DrawText},
    {"SetMousePosition",           EQ_LUA_SetMousePosition},
    {"InterpretCommand",           EQ_LUA_InterpretCommand},
    {"GetCharInfo",                EQ_LUA_GetCharInfo},
    {"GetPlayerSpawnInfo",         EQ_LUA_GetPlayerSpawnInfo},
    {"GetTargetSpawnInfo",         EQ_LUA_GetTargetSpawnInfo},
    {NULL, NULL}
};

void EQ_LUA_Open()
{
    EQ_LUA_STATE = luaL_newstate();

    luaL_openlibs(EQ_LUA_STATE);

    luaL_register(EQ_LUA_STATE, "eq", EQ_LUA_LIBRARY);
}

void EQ_LUA_Close()
{
    lua_close(EQ_LUA_STATE);
}

#endif // EQMAC_LUA_H