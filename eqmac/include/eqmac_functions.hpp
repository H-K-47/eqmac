#ifndef EQMAC_FUNCTIONS_HPP
#define EQMAC_FUNCTIONS_HPP

#include <cstdint>
#include <cstring>
#include <cmath>

#include "eqmac.hpp"

#define EQ_FUNCTION_AT_ADDRESS(function,offset) __declspec(naked) function\
{\
    __asm{mov eax, offset};\
    __asm{jmp eax};\
}

#define EQ_FUNCTION_AT_VARIABLE_ADDRESS(function,variable) __declspec(naked) function\
{\
    __asm{mov eax, [variable]};\
    __asm{jmp eax};\
}

#define EQ_FUNCTION_AT_VIRTUAL_ADDRESS(function,virtualoffset) __declspec(naked) function\
{\
    __asm{mov eax, [ecx]};\
    __asm{lea eax, [eax+virtualoffset]};\
    __asm{mov eax, [eax]};\
    __asm{jmp eax};\
}

template <class T>
T EQ_READ_MEMORY(DWORD address)
{
    T* buffer = (T*)address;
    return (*buffer);
}

template <class T>
void EQ_WRITE_MEMORY(DWORD address, T value)
{
    T* buffer = (T*)address;
    *buffer = value;
}

EQSPAWNINFO** ppSpawnsBegin = (EQSPAWNINFO**)EQ_POINTER_SPAWNS_BEGIN;
#define EQ_OBJECT_FirstSpawn (*ppSpawnsBegin)

EQGROUNDSPAWNINFO** ppGroundSpawnsBegin = (EQGROUNDSPAWNINFO**)EQ_POINTER_GROUND_SPAWNS_BEGIN;
#define EQ_OBJECT_FirstGroundSpawn (*ppGroundSpawnsBegin)

EQDOORSPAWNINFO** ppDoorSpawnsBegin = (EQDOORSPAWNINFO**)EQ_POINTER_DOOR_SPAWNS_BEGIN;
#define EQ_OBJECT_FirstDoorSpawn (*ppDoorSpawnsBegin)

EQSPAWNINFO** ppPlayerSpawnInfo = (EQSPAWNINFO**)EQ_POINTER_PLAYER_SPAWN_INFO;
#define EQ_OBJECT_PlayerSpawn (*ppPlayerSpawnInfo)

EQSPAWNINFO** ppTargetSpawnInfo = (EQSPAWNINFO**)EQ_POINTER_TARGET_SPAWN_INFO;
#define EQ_OBJECT_TargetSpawn (*ppTargetSpawnInfo)

//class CXWnd;
class CSidlScreenWnd;

class CDisplay;
class CEverQuest;
class EQ_Character;
class CHotButtonWnd;
class CLootWnd;
class CTradeWnd;

class EQPlayer;

class CSidlScreenWnd
{
public:
    CSidlScreenWnd::~CSidlScreenWnd(void);
    CSidlScreenWnd::CSidlScreenWnd() {};
};

class CDisplay
{
public:
    CDisplay::~CDisplay(void);
    CDisplay::CDisplay(struct HWND__*);
    void CDisplay::MoveLocalPlayerToSafeCoords(void);
    static int __cdecl CDisplay::WriteTextHD2(const char* text, int x, int y, int color, int font);
};

CDisplay** ppCDisplay = (CDisplay**)EQ_POINTER_CDisplay;
#define EQ_CLASS_CDisplay (*ppCDisplay)

class CEverQuest
{
public:
    CEverQuest::~CEverQuest(void);
    CEverQuest::CEverQuest(struct HWND__*);
    void CEverQuest::dsp_chat(const char* text, uint16_t color, bool filtered);
    void CEverQuest::dsp_chat(const char* text);
    char* CEverQuest::trimName(char*);
    int CEverQuest::MoveToZone(char* zoneShortName, char* text, int destinationType, int zoneRequestReason);
    int CEverQuest::MoveToZone(int zoneId, char* text, int destinationType, int zoneRequestReason);
    int CEverQuest::LootCorpse(class EQPlayer*, int unknown);
};

CEverQuest** ppCEverQuest = (CEverQuest**)EQ_POINTER_CEverQuest;
#define EQ_CLASS_CEverQuest (*ppCEverQuest)

class EQ_Character
{
public:
    EQ_Character::~EQ_Character(void);
    EQ_Character::EQ_Character(void);
    uint16_t EQ_Character::Max_Mana(void);
};

EQ_Character** ppEQ_Character = (EQ_Character**)EQ_POINTER_EQ_Character;
#define EQ_CLASS_EQ_Character (*ppEQ_Character)

class CHotButtonWnd : public CSidlScreenWnd
{
public:
    void CHotButtonWnd::DoHotButton(uint16_t buttonIndex, int allowAutoRightClick);
};

CHotButtonWnd** ppCHotButtonWnd = (CHotButtonWnd**)EQ_POINTER_CHotButtonWnd;
#define EQ_CLASS_CHotButtonWnd (*ppCHotButtonWnd)

class CLootWnd : public CSidlScreenWnd
{
public:
    void CLootWnd::Deactivate(void);
    void CLootWnd::RequestLootSlot(int slotIndex, bool autoLoot);
};

CLootWnd** ppCLootWnd = (CLootWnd**)EQ_POINTER_CLootWnd;
#define EQ_CLASS_CLootWnd (*ppCLootWnd)

class CTradeWnd : public CSidlScreenWnd
{
public:
    void CTradeWnd::Activate(class EQPlayer*, bool isTargetNpc); // if isTargetNpc == true, show Give Window
};

CTradeWnd** ppCTradeWnd = (CTradeWnd**)EQ_POINTER_CTradeWnd;
#define EQ_CLASS_CTradeWnd (*ppCTradeWnd)

class EQPlayer
{
public:
    EQPlayer::~EQPlayer(void);
    EQPlayer::EQPlayer(class EQPlayer*, unsigned char, unsigned int, unsigned char, char*);
    void EQPlayer::ChangeHeight(float height);
    void EQPlayer::ChangePosition(uint8_t);
    void EQPlayer::FacePlayer(class EQPlayer*);
};

/* CDisplay */

#define EQ_FUNCTION_CDisplay__MoveLocalPlayerToSafeCoords 0x004B459C
#ifdef EQ_FUNCTION_CDisplay__MoveLocalPlayerToSafeCoords
EQ_FUNCTION_AT_ADDRESS(void CDisplay::MoveLocalPlayerToSafeCoords(void), EQ_FUNCTION_CDisplay__MoveLocalPlayerToSafeCoords);
#endif

#define EQ_FUNCTION_CDisplay__WriteTextHD2 0x004AA5AA
#ifdef EQ_FUNCTION_CDisplay__WriteTextHD2
typedef int (__cdecl* EQ_FUNCTION_TYPE_CDisplay__WriteTextHD2)(const char* text, int x, int y, int color, int font);
EQ_FUNCTION_AT_ADDRESS(int CDisplay::WriteTextHD2(const char*, int, int, int, int), EQ_FUNCTION_CDisplay__WriteTextHD2);
#endif

/* CEverQuest */

#define EQ_FUNCTION_CEverQuest__dsp_chat 0x00537F99
#ifdef EQ_FUNCTION_CEverQuest__dsp_chat
EQ_FUNCTION_AT_ADDRESS(void CEverQuest::dsp_chat(const char*,uint16_t,bool), EQ_FUNCTION_CEverQuest__dsp_chat);
#endif

#define EQ_FUNCTION_CEverQuest__dsp_chat2 0x005380FD
#ifdef EQ_FUNCTION_CEverQuest__dsp_chat2
EQ_FUNCTION_AT_ADDRESS(void CEverQuest::dsp_chat(const char*), EQ_FUNCTION_CEverQuest__dsp_chat2);
#endif

#define EQ_FUNCTION_CEverQuest__trimName 0x00537D39
#ifdef EQ_FUNCTION_CEverQuest__trimName
EQ_FUNCTION_AT_ADDRESS(char* CEverQuest::trimName(char*), EQ_FUNCTION_CEverQuest__trimName);
#endif

#define EQ_FUNCTION_CEverQuest__MoveToZone 0x005460B5
#ifdef EQ_FUNCTION_CEverQuest__MoveToZone
typedef int (__cdecl* EQ_FUNCTION_TYPE_CEverQuest__MoveToZone)(char*, char*, int, int);
EQ_FUNCTION_AT_ADDRESS(int CEverQuest::MoveToZone(char*, char*, int, int), EQ_FUNCTION_CEverQuest__MoveToZone);
#endif

#define EQ_FUNCTION_CEverQuest__MoveToZone2 0x00546081
#ifdef EQ_FUNCTION_CEverQuest__MoveToZone2
typedef int (__cdecl* EQ_FUNCTION_TYPE_CEverQuest__MoveToZone2)(int, char*, int, int);
EQ_FUNCTION_AT_ADDRESS(int CEverQuest::MoveToZone(int, char*, int, int), EQ_FUNCTION_CEverQuest__MoveToZone2);
#endif

#define EQ_FUNCTION_CEverQuest__LootCorpse 0x00547808
#ifdef EQ_FUNCTION_CEverQuest__LootCorpse
typedef signed int (__cdecl* EQ_FUNCTION_TYPE_CEverQuest__LootCorpse)(class EQPlayer*, int);
EQ_FUNCTION_AT_ADDRESS(int CEverQuest::LootCorpse(class EQPlayer *, int), EQ_FUNCTION_CEverQuest__LootCorpse);
#endif

/* CHotButtonWnd */

#define EQ_FUNCTION_CHotButtonWnd__DoHotButton 0x004209BD
#ifdef EQ_FUNCTION_CHotButtonWnd__DoHotButton
EQ_FUNCTION_AT_ADDRESS(void CHotButtonWnd::DoHotButton(uint16_t, int), EQ_FUNCTION_CHotButtonWnd__DoHotButton);
#endif

/* CLootWnd */

#define EQ_FUNCTION_CLootWnd__Deactivate 0x0042651F
#ifdef EQ_FUNCTION_CLootWnd__Deactivate
EQ_FUNCTION_AT_ADDRESS(void CLootWnd::Deactivate(void), EQ_FUNCTION_CLootWnd__Deactivate);
#endif

#define EQ_FUNCTION_CLootWnd__RequestLootSlot 0x00426B02
#ifdef EQ_FUNCTION_CLootWnd__RequestLootSlot
EQ_FUNCTION_AT_ADDRESS(void CLootWnd::RequestLootSlot(int, bool), EQ_FUNCTION_CLootWnd__RequestLootSlot);
#endif

/* CTradeWnd */

#define EQ_FUNCTION_CTradeWnd__Activate 0x004392C9
#ifdef EQ_FUNCTION_CTradeWnd__Activate
EQ_FUNCTION_AT_ADDRESS(void CTradeWnd::Activate(class EQPlayer*, bool), EQ_FUNCTION_CTradeWnd__Activate);
#endif

/* EQPlayer */

#define EQ_FUNCTION_EQPlayer__ChangeHeight 0x0050C160
#ifdef EQ_FUNCTION_EQPlayer__ChangeHeight
EQ_FUNCTION_AT_ADDRESS(void EQPlayer::ChangeHeight(float), EQ_FUNCTION_EQPlayer__ChangeHeight);
#endif

#define EQ_FUNCTION_EQPlayer__ChangePosition 0x0050BE3C
#ifdef EQ_FUNCTION_EQPlayer__ChangePosition
EQ_FUNCTION_AT_ADDRESS(void EQPlayer::ChangePosition(uint8_t), EQ_FUNCTION_EQPlayer__ChangePosition);
#endif

#define EQ_FUNCTION_EQPlayer__FacePlayer 0x00508120
#ifdef EQ_FUNCTION_EQPlayer__FacePlayer
EQ_FUNCTION_AT_ADDRESS(void EQPlayer::FacePlayer(class EQPlayer*), EQ_FUNCTION_EQPlayer__FacePlayer);
#endif

/* EQ_Character */

#define EQ_FUNCTION_EQ_Character__Max_Mana 0x004B9483
#ifdef EQ_FUNCTION_EQ_Character__Max_Mana
EQ_FUNCTION_AT_ADDRESS(unsigned short EQ_Character::Max_Mana(void), EQ_FUNCTION_EQ_Character__Max_Mana);
#endif

/* OTHER */

#define EQ_FUNCTION_DrawNetStatus 0x0054D3AE
#ifdef EQ_FUNCTION_DrawNetStatus
typedef int (__cdecl* EQ_FUNCTION_TYPE_DrawNetStatus)(int, unsigned short, unsigned short, unsigned short, unsigned short, int, unsigned short, unsigned long, long, unsigned long);
#endif

// world to screen function
typedef int (__cdecl* EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dWorldSpaceToScreenSpace)(int, EQLOCATION*, float*, float*);
EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dWorldSpaceToScreenSpace EQGfx_Dx8__t3dWorldSpaceToScreenSpace;

// draw line function
typedef int (__cdecl* EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dDeferLine)(EQLINE*, int); // int color ; 0xAARRGGBB
EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dDeferLine EQGfx_Dx8__t3dDeferLine;

// EQWorldData::GetFullZoneName
// call 00523E49

// StringTable::getString((ulong,bool *))
//#define EQ_FUNCTION_StringTable__getString 0x00550EFE

//#define EQ_FUNCTION___get_melee_range 0x004F3898

//#define EQ_FUNCTION___CastRay 0x004F20DB

/*

// EQ_Character::CastSpell((uchar,short,EQ_Item **,short))
#define EQ_FUNCTION_EQ_Character__CastSpell 0x004C483B

#ifdef EQ_FUNCTION_EQ_Character__CastSpell
typedef void (__stdcall *_everquest_function_EQ_Character__CastSpell)(unsigned char gem_index, short spell_id);
_everquest_function_EQ_Character__CastSpell everquest_function_EQ_Character__CastSpell = (_everquest_function_EQ_Character__CastSpell)EQ_FUNCTION_EQ_Character__CastSpell;
#endif
*/

char* EQ_GetGuildNameById(int guildId)
{
    if (guildId == EQ_GUILD_ID_NULL)
    {
        return "Unknown Guild";
    }

    EQGUILDLIST* guildList = (EQGUILDLIST*)EQ_STRUCTURE_GUILD_LIST;

    return guildList->Guild[guildId].Name;
}

void EQ_DrawRectangle(float x, float y, float width, float height, int color)
{
    EQLINE lineTop;
    lineTop.X1 = x;
    lineTop.Y1 = y;
    lineTop.Z1 = 1.0f;
    lineTop.X2 = x + width;
    lineTop.Y2 = y;
    lineTop.Z2 = 1.0f;

    EQLINE lineLeft;
    lineLeft.X1 = x;
    lineLeft.Y1 = y;
    lineLeft.Z1 = 1.0f;
    lineLeft.X2 = x;
    lineLeft.Y2 = y + height;
    lineLeft.Z2 = 1.0f;

    EQLINE lineRight;
    lineRight.X1 = x + width;
    lineRight.Y1 = y;
    lineRight.Z1 = 1.0f;
    lineRight.X2 = x + width;
    lineRight.Y2 = y + height;
    lineRight.Z2 = 1.0f;

    EQLINE lineBottom;
    lineBottom.X1 = x;
    lineBottom.Y1 = y + height;
    lineBottom.Z1 = 1.0f;
    lineBottom.X2 = x + width;
    lineBottom.Y2 = y + height;
    lineBottom.Z2 = 1.0f;

    EQGfx_Dx8__t3dDeferLine(&lineTop,    color);
    EQGfx_Dx8__t3dDeferLine(&lineLeft,   color);
    EQGfx_Dx8__t3dDeferLine(&lineRight,  color);
    EQGfx_Dx8__t3dDeferLine(&lineBottom, color);
}

/*
template <class T>
T everquest_function_read_memory(DWORD address);

template <class T>
void everquest_function_write_memory(DWORD address, T value);

std::string everquest_function_read_memory_string(DWORD address, int size);

bool everquest_function_is_in_game();
int everquest_function_get_char_info();
int everquest_function_get_player_spawn_info();
int everquest_function_get_player_actor_info();
int everquest_function_get_target_spawn_info();
int everquest_function_get_target_actor_info();
void everquest_function_set_target_spawn_info(int spawn_info);
int everquest_function_get_CDisplay();
int everquest_function_get_CEverQuest();
int everquest_function_get_CLootWnd();
int everquest_function_get_CDisplay_timer();
void everquest_function_update_spawns(std::vector<everquest_spawn_t> &spawns);
void everquest_function_update_loot_items(std::vector<everquest_item_t> &items);
void everquest_function_write_text_to_chat(const char *text);
void everquest_function_gate_to_bind_point();
int everquest_function_get_zone_id();
std::string everquest_function_get_zone_short_name();
std::string everquest_function_get_zone_long_name();
void everquest_function_set_auto_attack(bool enabled);
void everquest_function_warp_to_coordinates(float y, float x, float z, float heading);
void everquest_function_warp_to_spawn_info(int spawn_info, bool use_heading);
void everquest_function_adjust_player_distance_from_target(float distance, bool backwards);
float everquest_function_get_target_distance();
void everquest_function_set_player_height(float height);
float everquest_function_get_player_height();
void everquest_function_set_player_position(unsigned char position);
unsigned char everquest_function_get_player_position();
void everquest_function_face_spawn_info(int spawn_info);
void everquest_function_do_hot_button(short button_index);
unsigned short everquest_function_get_player_mana_max();
unsigned short everquest_function_get_player_mana_current();
float everquest_function_get_player_mana_percent();
int everquest_function_get_spawn_info_by_name(std::string spawn_name, bool partial_match, bool sort_by_distance, bool ignore_corpse);
bool everquest_function_is_loot_window_open();
bool everquest_function_is_loot_window_empty();
void everquest_function_loot_corpse(int spawn_info);
void everquest_function_loot_slot(int slot_index);
void everquest_function_loot_window_close();
void everquest_function_loot_all_items();
int everquest_function_get_loot_slot_of_item_by_name(std::string item_name, bool partial_match);
bool everquest_function_loot_all_items_by_name(std::string item_name, bool partial_match);
void everquest_function_trade_window_activate(int spawn_info, bool is_target_npc);

template <class T>
T everquest_function_read_memory(DWORD address)
{
    return *(T*)address;
}

template <class T>
void everquest_function_write_memory(DWORD address, T value)
{
    *(T*)address = value;
}

std::string everquest_function_read_memory_string(DWORD address, int size)
{
    char *buffer = new char[size + 1];

    for (int i = 0; i < size; i++)
    {
        buffer[i] = *(unsigned char*)(address + i);
    }

    std::string result = buffer;

    delete[] buffer;

    return result;
}

bool everquest_function_is_in_game()
{
    unsigned char result = everquest_function_read_memory<BYTE>(EQ_IS_IN_GAME);

    if (result == 0)
    {
        return false;
    }

    return true;
}

int everquest_function_get_char_info()
{
    return everquest_function_read_memory<DWORD>(EQ_CHAR_INFO_POINTER);
}

int everquest_function_get_player_spawn_info()
{
    return everquest_function_read_memory<DWORD>(EQ_PLAYER_SPAWN_INFO_POINTER);
}

int everquest_function_get_player_actor_info()
{
    int player_spawn_info = everquest_function_get_player_spawn_info();

    return everquest_function_read_memory<DWORD>(player_spawn_info + EQ_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER);
}

int everquest_function_get_target_spawn_info()
{
    return everquest_function_read_memory<DWORD>(EQ_TARGET_SPAWN_INFO_POINTER);
}

int everquest_function_get_target_actor_info()
{
    int target_spawn_info = everquest_function_get_target_spawn_info();

    if (target_spawn_info == EQ_SPAWN_INFO_NULL)
    {
        return 0;
    }

    return everquest_function_read_memory<DWORD>(target_spawn_info + EQ_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER);
}

void everquest_function_set_target_spawn_info(int spawn_info)
{
    everquest_function_write_memory<DWORD>(EQ_TARGET_SPAWN_INFO_POINTER, spawn_info);
}

int everquest_function_get_CDisplay()
{
    return everquest_function_read_memory<DWORD>(EQ_CDisplay_POINTER);
}

int everquest_function_get_CEverQuest()
{
    return everquest_function_read_memory<DWORD>(EQ_CEverQuest_POINTER);
}

int everquest_function_get_CLootWnd()
{
    return everquest_function_read_memory<DWORD>(EQ_CLootWnd_POINTER);
}

int everquest_function_get_CDisplay_timer()
{
    int CDisplay_info = everquest_function_get_CDisplay();

    return everquest_function_read_memory<DWORD>(CDisplay_info + EQ_OFFSET_CDisplay_TIMER);
}

void everquest_function_update_spawns(std::vector<everquest_spawn_t> &spawns)
{
    spawns.clear();

    int player_spawn_info = everquest_function_get_player_spawn_info();
    int target_spawn_info = everquest_function_get_target_spawn_info();

    float player_y = everquest_function_read_memory<float>(player_spawn_info + EQ_OFFSET_SPAWN_INFO_Y);
    float player_x = everquest_function_read_memory<float>(player_spawn_info + EQ_OFFSET_SPAWN_INFO_X);
    float player_z = everquest_function_read_memory<float>(player_spawn_info + EQ_OFFSET_SPAWN_INFO_Z);

    int spawn_info_address = player_spawn_info;

    int spawn_next_spawn_info = everquest_function_read_memory<DWORD>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER);

    spawn_info_address = spawn_next_spawn_info;

    for (int i = 0; i < EQ_SPAWNS_MAX; i++)
    {
        spawn_next_spawn_info = everquest_function_read_memory<DWORD>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER);

        if (spawn_next_spawn_info == EQ_SPAWN_INFO_NULL)
        {
            break;
        }

        int spawn_actor_info = everquest_function_read_memory<DWORD>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER);

        everquest_spawn_t spawn;

        spawn.address = spawn_info_address;

        spawn.spawn_id = everquest_function_read_memory<WORD>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_SPAWN_ID);
        spawn.owner_id = everquest_function_read_memory<WORD>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_OWNER_ID);
        spawn.guild_id = everquest_function_read_memory<WORD>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_GUILD_ID);

        spawn.name      = everquest_function_read_memory_string(spawn_info_address + EQ_OFFSET_SPAWN_INFO_NAME,      EQ_SPAWN_INFO_NAME_SIZE);
        spawn.last_name = everquest_function_read_memory_string(spawn_info_address + EQ_OFFSET_SPAWN_INFO_LAST_NAME, EQ_SPAWN_INFO_LAST_NAME_SIZE);

        spawn.y = everquest_function_read_memory<float>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_Y);
        spawn.x = everquest_function_read_memory<float>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_X);
        spawn.z = everquest_function_read_memory<float>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_Z);

        spawn.distance = everquest_calculate_distance(player_x, player_y, spawn.x, spawn.y);

        spawn.distance_z = abs(spawn.z - player_z);

        spawn.heading = everquest_function_read_memory<float>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_HEADING);

        spawn.movement_speed = everquest_function_read_memory<float>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_MOVEMENT_SPEED);

        spawn.standing_state = everquest_function_read_memory<BYTE>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_STANDING_STATE);

        spawn.type   = everquest_function_read_memory<BYTE>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_TYPE);
        spawn.level  = everquest_function_read_memory<BYTE>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_LEVEL);
        spawn.race   = everquest_function_read_memory<BYTE>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_RACE);
        spawn._class = everquest_function_read_memory<BYTE>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_CLASS);

        spawn.hp     = everquest_function_read_memory<DWORD>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_HP_CURRENT);
        spawn.hp_max = everquest_function_read_memory<DWORD>(spawn_info_address + EQ_OFFSET_SPAWN_INFO_HP_MAX);

        spawn.is_target = (spawn_info_address == target_spawn_info ? true : false);

        spawn.is_player_corpse = false;

        if (spawn.type == EQ_SPAWN_INFO_TYPE_CORPSE)
        {
            std::string player_name = everquest_function_read_memory_string(player_spawn_info + EQ_OFFSET_SPAWN_INFO_NAME, EQ_SPAWN_INFO_NAME_SIZE);

            std::size_t found = spawn.name.find(player_name);

            if (found != std::string::npos)
            {
                spawn.is_player_corpse = true;
            }
        }

        spawn.is_holding_both      = everquest_function_read_memory<DWORD>(spawn_actor_info + EQ_OFFSET_ACTOR_INFO_IS_HOLDING_BOTH);
        spawn.is_holding_secondary = everquest_function_read_memory<DWORD>(spawn_actor_info + EQ_OFFSET_ACTOR_INFO_IS_HOLDING_SECONDARY);
        spawn.is_holding_primary   = everquest_function_read_memory<DWORD>(spawn_actor_info + EQ_OFFSET_ACTOR_INFO_IS_HOLDING_PRIMARY);

        spawns.push_back(spawn);

        spawn_info_address = spawn_next_spawn_info;
    }
}

void everquest_function_update_loot_items(std::vector<everquest_item_t> &items)
{
    items.clear();

    if (everquest_function_is_loot_window_open == false)
    {
        return;
    }

    int CLootWnd_info = everquest_function_get_CLootWnd();

    for (int i = 0; i < EQ_CLootWnd_ITEMS_MAX; i++)
    {
        int item_info_address = everquest_function_read_memory<DWORD>(CLootWnd_info + EQ_OFFSET_CLootWnd_ITEMS + (i * EQ_CLootWnd_ITEMS_OFFSET));

        if (item_info_address == EQ_ITEM_INFO_NULL)
        {
            continue;
        }

        everquest_item_t item;

        item.loot_window_slot = i;

        item.address = item_info_address;

        item.name      = everquest_function_read_memory_string(item_info_address + EQ_OFFSET_ITEM_INFO_NAME,      EQ_ITEM_INFO_NAME_SIZE);
        item.lore_name = everquest_function_read_memory_string(item_info_address + EQ_OFFSET_ITEM_INFO_LORE_NAME, EQ_ITEM_INFO_LORE_NAME_SIZE);
        item.id_file   = everquest_function_read_memory_string(item_info_address + EQ_OFFSET_ITEM_INFO_ID_FILE,   EQ_ITEM_INFO_ID_FILE_SIZE);

        unsigned char item_weight = everquest_function_read_memory<BYTE>(item_info_address + EQ_OFFSET_ITEM_INFO_WEIGHT);
        item.weight = (float)(item_weight * 0.1);

        item.size = everquest_function_read_memory<BYTE>(item_info_address + EQ_OFFSET_ITEM_INFO_SIZE);

        unsigned char item_is_no_rent = everquest_function_read_memory<BYTE>(item_info_address + EQ_OFFSET_ITEM_INFO_IS_NO_RENT);
        if (item_is_no_rent == 255)
        {
            item.is_no_rent = false;
        }
        else
        {
            item.is_no_rent = true;
        }

        unsigned char item_is_no_drop = everquest_function_read_memory<BYTE>(item_info_address + EQ_OFFSET_ITEM_INFO_IS_NO_DROP);
        if (item_is_no_drop == 255)
        {
            item.is_no_drop = false;
        }
        else
        {
            item.is_no_drop = true;
        }

        unsigned char item_is_container = everquest_function_read_memory<BYTE>(item_info_address + EQ_OFFSET_ITEM_INFO_IS_CONTAINER);
        if (item_is_container == 0)
        {
            item.is_container = false;
        }
        else
        {
            item.is_container = true;
        }

        item.id   = everquest_function_read_memory<WORD>(item_info_address + EQ_OFFSET_ITEM_INFO_ID);
        item.icon = everquest_function_read_memory<WORD>(item_info_address + EQ_OFFSET_ITEM_INFO_ICON);

        item.equip_slot       = everquest_function_read_memory<DWORD>(item_info_address + EQ_OFFSET_ITEM_INFO_EQUIP_SLOT);
        item.equippable_slots = everquest_function_read_memory<DWORD>(item_info_address + EQ_OFFSET_ITEM_INFO_EQUIPPABLE_SLOTS);

        item.cost = everquest_function_read_memory<DWORD>(item_info_address + EQ_OFFSET_ITEM_INFO_COST);

        items.push_back(item);
    }
}

void everquest_function_write_text_to_chat(const char *text)
{
    everquest_object_CEverQuest->dsp_chat(text);
}

void everquest_function_gate_to_bind_point()
{
    int player_spawn_info = everquest_function_get_player_spawn_info();

    everquest_function_write_memory<BYTE>(player_spawn_info + EQ_OFFSET_SPAWN_INFO_TYPE, EQ_SPAWN_INFO_TYPE_CORPSE);
}

int everquest_function_get_zone_id()
{
    return everquest_function_read_memory<DWORD>(EQ_ZONE_ID);
}

std::string everquest_function_get_zone_short_name()
{
    std::string name = everquest_function_read_memory_string(EQ_ZONE_INFO_STRUCTURE + EQ_OFFSET_ZONE_INFO_SHORT_NAME, EQ_ZONE_INFO_SHORT_NAME_SIZE);

    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    return name;
}

std::string everquest_function_get_zone_long_name()
{
    return everquest_function_read_memory_string(EQ_ZONE_INFO_STRUCTURE + EQ_OFFSET_ZONE_INFO_LONG_NAME, EQ_ZONE_INFO_LONG_NAME_SIZE);
}

void everquest_function_set_auto_attack(bool enabled)
{
    everquest_function_write_memory<BYTE>(EQ_IS_AUTO_ATTACK_ENABLED, (unsigned char)enabled);
}

void everquest_function_warp_to_coordinates(float y, float x, float z, float heading = -1.0)
{
    if (heading >= 0 && heading <= EQ_HEADING_MAX)
    {
        int player_spawn_info = everquest_function_get_player_spawn_info();

        everquest_function_write_memory<float>(player_spawn_info + EQ_OFFSET_SPAWN_INFO_HEADING, heading);
    }

    float original_y = everquest_function_read_memory<float>(EQ_ZONE_INFO_STRUCTURE + EQ_OFFSET_ZONE_INFO_SAFE_POINT_Y);
    float original_x = everquest_function_read_memory<float>(EQ_ZONE_INFO_STRUCTURE + EQ_OFFSET_ZONE_INFO_SAFE_POINT_Y);
    float original_z = everquest_function_read_memory<float>(EQ_ZONE_INFO_STRUCTURE + EQ_OFFSET_ZONE_INFO_SAFE_POINT_Y);

    everquest_function_write_memory<float>(EQ_ZONE_INFO_STRUCTURE + EQ_OFFSET_ZONE_INFO_SAFE_POINT_Y, y);
    everquest_function_write_memory<float>(EQ_ZONE_INFO_STRUCTURE + EQ_OFFSET_ZONE_INFO_SAFE_POINT_X, x);
    everquest_function_write_memory<float>(EQ_ZONE_INFO_STRUCTURE + EQ_OFFSET_ZONE_INFO_SAFE_POINT_Z, z);

    everquest_object_CDisplay->MoveLocalPlayerToSafeCoords();

    everquest_function_write_memory<float>(EQ_ZONE_INFO_STRUCTURE + EQ_OFFSET_ZONE_INFO_SAFE_POINT_Y, original_y);
    everquest_function_write_memory<float>(EQ_ZONE_INFO_STRUCTURE + EQ_OFFSET_ZONE_INFO_SAFE_POINT_X, original_x);
    everquest_function_write_memory<float>(EQ_ZONE_INFO_STRUCTURE + EQ_OFFSET_ZONE_INFO_SAFE_POINT_Z, original_z);
}

void everquest_function_warp_to_spawn_info(int spawn_info, bool use_heading = true)
{
    if (spawn_info == EQ_SPAWN_INFO_NULL)
    {
        return;
    }

    float spawn_y = everquest_function_read_memory<float>(spawn_info + EQ_OFFSET_SPAWN_INFO_Y);
    float spawn_x = everquest_function_read_memory<float>(spawn_info + EQ_OFFSET_SPAWN_INFO_X);
    float spawn_z = everquest_function_read_memory<float>(spawn_info + EQ_OFFSET_SPAWN_INFO_Z);

    float spawn_heading = -1.0;
    
    if (use_heading == true)
    {
        spawn_heading = everquest_function_read_memory<float>(spawn_info + EQ_OFFSET_SPAWN_INFO_HEADING);
    }

    everquest_function_warp_to_coordinates(spawn_y, spawn_x, spawn_z, spawn_heading);
}

void everquest_function_adjust_player_distance_from_target(float distance, bool backwards = true)
{
    int target_spawn_info = everquest_function_get_target_spawn_info();

    if (target_spawn_info == EQ_SPAWN_INFO_NULL)
    {
        return;
    }

    int player_spawn_info = everquest_function_get_player_spawn_info();

    if (target_spawn_info == player_spawn_info)
    {
        return;
    }

    float movement_speed = 0.0;

    while (1)
    {
        float target_distance = everquest_function_get_target_distance();

        if (target_distance == -1.0)
        {
            return;
        }

        bool distance_is_met = false;

        if (backwards == true)
        {
            movement_speed = -1.0;

            distance_is_met = (target_distance <= distance);
        }
        else
        {
            movement_speed = 1.0;

            distance_is_met = (target_distance >= distance);
        }

        if (distance_is_met == true)
        {
            everquest_function_write_memory<float>(player_spawn_info + EQ_OFFSET_SPAWN_INFO_MOVEMENT_SPEED, movement_speed);
        }
        else
        {
            break;
        }
    }

    everquest_function_write_memory<float>(player_spawn_info + EQ_OFFSET_SPAWN_INFO_MOVEMENT_SPEED, 0.0);
}

float everquest_function_get_target_distance()
{
    int target_spawn_info = everquest_function_get_target_spawn_info();

    if (target_spawn_info == EQ_SPAWN_INFO_NULL)
    {
        return -1.0;
    }

    float target_y = everquest_function_read_memory<float>(target_spawn_info + EQ_OFFSET_SPAWN_INFO_Y);
    float target_x = everquest_function_read_memory<float>(target_spawn_info + EQ_OFFSET_SPAWN_INFO_X);

    int player_spawn_info = everquest_function_get_player_spawn_info();

    if (player_spawn_info == EQ_SPAWN_INFO_NULL)
    {
        return -1.0;
    }
 
    float player_y = everquest_function_read_memory<float>(player_spawn_info + EQ_OFFSET_SPAWN_INFO_Y);
    float player_x = everquest_function_read_memory<float>(player_spawn_info + EQ_OFFSET_SPAWN_INFO_X);

    return everquest_calculate_distance(player_x, player_y, target_x, target_y);
}

void everquest_function_set_player_height(float height)
{
    if (height <= 0)
    {
        return;
    }

    int player_spawn_info = everquest_function_get_player_spawn_info();

    ((EQPlayer*)player_spawn_info)->ChangeHeight(height);
}

float everquest_function_get_player_height()
{
    int player_spawn_info = everquest_function_get_player_spawn_info();

    return everquest_function_read_memory<float>(player_spawn_info + EQ_OFFSET_SPAWN_INFO_MODEL_HEIGHT);
}

void everquest_function_set_player_position(unsigned char position)
{
    if (position <= 0)
    {
        return;
    }

    int player_spawn_info = everquest_function_get_player_spawn_info();

    ((EQPlayer*)player_spawn_info)->ChangePosition(position);
}

unsigned char everquest_function_get_player_position()
{
    int player_spawn_info = everquest_function_get_player_spawn_info();

    return everquest_function_read_memory<BYTE>(player_spawn_info + EQ_OFFSET_SPAWN_INFO_STANDING_STATE);
}

void everquest_function_face_spawn_info(int spawn_info)
{
    int player_spawn_info = everquest_function_get_player_spawn_info();

    ((EQPlayer*)player_spawn_info)->FacePlayer((EQPlayer*)spawn_info);
}

void everquest_function_do_hot_button(short button_index)
{
    if (button_index < 0)
    {
        return;
    }

    if (button_index > 0 && button_index < 10)
    {
        button_index = button_index - 1;
    }
    else
    {
        button_index = 9;
    }

    everquest_object_CHotButtonWnd->DoHotButton(button_index, 1);
}

unsigned short everquest_function_get_player_mana_max()
{
    return everquest_object_EQ_Character->Max_Mana();
}

unsigned short everquest_function_get_player_mana_current()
{
    int char_info = everquest_function_get_char_info();

    unsigned short player_mana_current = everquest_function_read_memory<WORD>(char_info + EQ_OFFSET_CHAR_INFO_MANA_CURRENT);

    return player_mana_current;
}

float everquest_function_get_player_mana_percent()
{
    unsigned short player_mana_current = everquest_function_get_player_mana_current();

    unsigned short player_mana_max = everquest_function_get_player_mana_max();

    return (float)((player_mana_current * 100) / player_mana_max);
}

int everquest_function_get_spawn_info_by_name(std::string spawn_name, bool partial_match = true, bool sort_by_distance = true, bool ignore_corpse = true)
{
    if (spawn_name.size() == 0)
    {
        return EQ_SPAWN_INFO_NULL;
    }

    std::vector<everquest_spawn_t> spawns;
    std::vector<everquest_spawn_t>::iterator spawns_it;

    everquest_function_update_spawns(spawns);

    if (spawns.size() == 0)
    {
        return EQ_SPAWN_INFO_NULL;
    }

    if (sort_by_distance == true)
    {
        std::sort(spawns.begin(), spawns.end(), everquest_sort_spawns_by_distance_t());
    }

    int spawn_info = EQ_SPAWN_INFO_NULL;

    for (spawns_it = spawns.begin() ; spawns_it != spawns.end(); ++spawns_it)
    {
        if (ignore_corpse == true)
        {
            if (spawns_it->type == EQ_SPAWN_INFO_TYPE_CORPSE)
            {
                continue;
            }
        }

        if (partial_match == true)
        {
            std::size_t found = spawns_it->name.find(spawn_name);

            if (found != std::string::npos)
            {
                spawn_info = spawns_it->address;
                break;
            }
        }
        else
        {
            if (spawns_it->name == spawn_name)
            {
                spawn_info = spawns_it->address;
                break;
            }
        }
    }

    return spawn_info;
}

void everquest_function_loot_window_close()
{
    everquest_object_CLootWnd->Deactivate();
}

bool everquest_function_is_loot_window_open()
{
    int CLootWnd_info = everquest_function_get_CLootWnd();

    unsigned char is_loot_window_open = everquest_function_read_memory<BYTE>(CLootWnd_info + EQ_OFFSET_CLootWnd_IS_OPEN);

    if (is_loot_window_open == 0)
    {
        return false;
    }

    return true;
}

bool everquest_function_is_loot_window_empty()
{
    if (everquest_function_is_loot_window_open == false)
    {
        return true;
    }

    int CLootWnd_info = everquest_function_get_CLootWnd();

    for (int i = 0; i < EQ_CLootWnd_ITEMS_MAX; i++)
    {
        int item_info_address = everquest_function_read_memory<DWORD>(CLootWnd_info + EQ_OFFSET_CLootWnd_ITEMS + (i * EQ_CLootWnd_ITEMS_OFFSET));

        if (item_info_address != EQ_ITEM_INFO_NULL)
        {
            return false;
        }
    }

    return true;
}

void everquest_function_loot_corpse(int spawn_info)
{
    if (spawn_info == EQ_SPAWN_INFO_NULL)
    {
        return;
    }

    unsigned char spawn_type = everquest_function_read_memory<BYTE>(spawn_info + EQ_OFFSET_SPAWN_INFO_TYPE);

    if (spawn_type != EQ_SPAWN_INFO_TYPE_CORPSE)
    {
        return;
    }

    if (everquest_function_is_loot_window_open() == true)
    {
        return;
    }

    everquest_object_CEverQuest->LootCorpse((EQPlayer *)spawn_info, 1);
}

void everquest_function_loot_slot(int slot_index)
{
    if (slot_index < 0)
    {
        return;
    }

    everquest_object_CLootWnd->RequestLootSlot(slot_index, true);
    Sleep(100);
}

void everquest_function_loot_all_items()
{
    if (everquest_function_is_loot_window_open == false)
    {
        return;
    }

    if (everquest_function_is_loot_window_empty() == true)
    {
        everquest_function_loot_window_close();
        return;
    }

    int CLootWnd_info = everquest_function_get_CLootWnd();

    for (int i = 0; i < EQ_CLootWnd_ITEMS_MAX; i++)
    {
        int item_info_address = everquest_function_read_memory<DWORD>(CLootWnd_info + EQ_OFFSET_CLootWnd_ITEMS + (i * EQ_CLootWnd_ITEMS_OFFSET));

        if (item_info_address == EQ_ITEM_INFO_NULL)
        {
            continue;
        }

        everquest_function_loot_slot(i);
    }
}

int everquest_function_get_loot_slot_of_item_by_name(std::string item_name, bool partial_match)
{
    if (item_name.size() == 0)
    {
        return -1;
    }

    if (everquest_function_is_loot_window_open == false)
    {
        return -1;
    }

    if (everquest_function_is_loot_window_empty() == true)
    {
        everquest_function_loot_window_close();
        return -1;
    }

    std::vector<everquest_item_t> loot_items;
    std::vector<everquest_item_t>::iterator loot_items_it;

    everquest_function_update_loot_items(loot_items);

    if (loot_items.size() == 0)
    {
        return -1;
    }

    int slot_index = -1;

    for (loot_items_it = loot_items.begin() ; loot_items_it != loot_items.end(); ++loot_items_it)
    {
        if (partial_match == true)
        {
            std::size_t found = loot_items_it->name.find(item_name);

            if (found != std::string::npos)
            {
                slot_index = loot_items_it->loot_window_slot;
                break;
            }
        }
        else
        {
            if (loot_items_it->name == item_name)
            {
                slot_index = loot_items_it->loot_window_slot;
                break;
            }
        }
    }

    return slot_index;
}

bool everquest_function_loot_all_items_by_name(std::string item_name, bool partial_match = true)
{
    if (item_name.size() == 0)
    {
        return false;
    }

    if (everquest_function_is_loot_window_open == false)
    {
        return false;
    }

    if (everquest_function_is_loot_window_empty() == true)
    {
        everquest_function_loot_window_close();
        return false;
    }

    std::vector<everquest_item_t> loot_items;
    std::vector<everquest_item_t>::iterator loot_items_it;

    everquest_function_update_loot_items(loot_items);

    if (loot_items.size() == 0)
    {
        return false;
    }

    bool result = false;

    for (loot_items_it = loot_items.begin() ; loot_items_it != loot_items.end(); ++loot_items_it)
    {
        bool found_match = false;

        if (partial_match == true)
        {
            std::size_t found = loot_items_it->name.find(item_name);

            if (found != std::string::npos)
            {
                found_match = true;
                result      = true;
            }
        }
        else
        {
            if (loot_items_it->name == item_name)
            {
                found_match = true;
                result      = true;
            }
        }

        if (found_match == true)
        {
            everquest_function_loot_slot(loot_items_it->loot_window_slot);;
        }
    }

    return result;
}

void everquest_function_trade_window_activate(int spawn_info, bool is_target_npc)
{
    everquest_object_CTradeWnd->Activate((EQPlayer *)spawn_info, is_target_npc);
}

*/

#endif // EQMAC_FUNCTIONS_HPP
