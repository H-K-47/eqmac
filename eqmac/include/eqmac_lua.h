#ifndef EQMAC_LUA_H
#define EQMAC_LUA_H

#include <cstdint>
#include <cstring>
#include <cmath>

#include "lua.hpp"

#include "eqmac.h"
#include "eqmac_functions.h"

lua_State* EQ_LUA_STATE;

static int EQ_LUA_WriteStringToChat(lua_State* L)
{
    const char* text = luaL_checkstring(L, 1);

    if (text != NULL && strlen(text) > 0)
    {
        EQ_WriteStringToChat(text);
    }

    return 0;
}

static int EQ_LUA_WriteStringToChatWithColor(lua_State* L)
{
    const char* text = luaL_checkstring(L, 1);

    int color = luaL_checkinteger(L, 2);

    if (text != NULL && strlen(text) > 0)
    {
        EQ_WriteStringToChatWithColor(text, color);
    }

    return 0;
}

static int EQ_LUA_DrawText(lua_State* L)
{
    const char* text = luaL_checkstring(L, 1);

    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    int color = luaL_checkinteger(L, 4);

    if (text != NULL && strlen(text) > 0)
    {
        EQ_DrawText(text, x, y, color);
    }

    return 0;
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
            if (EQ_IsInGame() == true)
            {
                EQ_WriteStringVarToChat("Script Error", (char*)lua_tostring(L, -1));
            }

            lua_pop(L, 1);

            return false;
        }
    }
    else
    {
        //if (EQ_IsInGame() == true)
        //{
            //EQ_WriteStringVarToChat("Script Error function not found", (char*)functionName);
        //}

        return false;
    }

    return true;
}

static const struct luaL_Reg EQ_LUA_LIBRARY[] =
{
    {"WriteStringToChat",          EQ_LUA_WriteStringToChat},
    {"WriteStringToChatWithColor", EQ_LUA_WriteStringToChatWithColor},
    {"DrawText",                   EQ_LUA_DrawText},
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