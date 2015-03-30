#ifndef EQMAC_FUNCTIONS_H
#define EQMAC_FUNCTIONS_H

#include <cstdint>
#include <cstring>
#include <cmath>

#include "eqmac.h"

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

void EQ_READ_MEMORY_STRING(DWORD address, size_t size, char result[])
{
    char *buffer = new char[size + 1];

    for (size_t i = 0; i < size; i++)
    {
        buffer[i] = *(unsigned char*)(address + i);
    }

    result = buffer;

    delete[] buffer;
}

void EQ_WRITE_MEMORY_STRING(DWORD address, const char* value)
{
    size_t length = strlen(value);

    size_t j = 0;

    for (size_t i = 0; i < length; i++)
    {
        *(unsigned char*)(address + j) = value[i];
        j++;
    }

    *(unsigned char*)(address + j) = '\0';
}

// structures

EQVIEWPORT* EQ_OBJECT_pViewPort = (EQVIEWPORT*)EQ_STRUCTURE_VIEWPORT;
#define EQ_OBJECT_ViewPort (*EQ_OBJECT_pViewPort)

EQGUILDLIST* EQ_OBJECT_pGuildList = (EQGUILDLIST*)EQ_STRUCTURE_GUILD_LIST;
#define EQ_OBJECT_GuildList (*EQ_OBJECT_pGuildList)

EQCOMMANDLIST* EQ_OBJECT_pCommandList = (EQCOMMANDLIST*)EQ_STRUCTURE_COMMAND_LIST;
#define EQ_OBJECT_CommandList (*EQ_OBJECT_pCommandList)

// pointers

EQSPELLLIST** EQ_OBJECT_ppSpellList = (EQSPELLLIST**)EQ_POINTER_SPELL_MANAGER;
#define EQ_OBJECT_SpellList (*EQ_OBJECT_ppSpellList)

EQCAMERAINFO** EQ_OBJECT_ppCameraInfo = (EQCAMERAINFO**)EQ_POINTER_CAMERA_INFO;
#define EQ_OBJECT_CameraInfo (*EQ_OBJECT_ppCameraInfo)

EQCHARINFO** EQ_OBJECT_ppCharInfo = (EQCHARINFO**)EQ_POINTER_CHAR_INFO;
#define EQ_OBJECT_CharInfo (*EQ_OBJECT_ppCharInfo)

EQSPAWNINFO** EQ_OBJECT_ppSpawnsBegin = (EQSPAWNINFO**)EQ_POINTER_SPAWNS_BEGIN;
#define EQ_OBJECT_FirstSpawn (*EQ_OBJECT_ppSpawnsBegin)

EQGROUNDSPAWNINFO** EQ_OBJECT_ppGroundSpawnsBegin = (EQGROUNDSPAWNINFO**)EQ_POINTER_GROUND_SPAWNS_BEGIN;
#define EQ_OBJECT_FirstGroundSpawn (*EQ_OBJECT_ppGroundSpawnsBegin)

EQDOORSPAWNINFO** EQ_OBJECT_ppDoorSpawnsBegin = (EQDOORSPAWNINFO**)EQ_POINTER_DOOR_SPAWNS_BEGIN;
#define EQ_OBJECT_FirstDoorSpawn (*EQ_OBJECT_ppDoorSpawnsBegin)

EQSPAWNINFO** EQ_OBJECT_ppPlayerSpawnInfo = (EQSPAWNINFO**)EQ_POINTER_PLAYER_SPAWN_INFO;
#define EQ_OBJECT_PlayerSpawn (*EQ_OBJECT_ppPlayerSpawnInfo)

EQSPAWNINFO** EQ_OBJECT_ppTargetSpawnInfo = (EQSPAWNINFO**)EQ_POINTER_TARGET_SPAWN_INFO;
#define EQ_OBJECT_TargetSpawn (*EQ_OBJECT_ppTargetSpawnInfo)

EQCEVERQUEST** EQ_OBJECT_ppCEverquest = (EQCEVERQUEST**)EQ_POINTER_CEverQuest;
#define EQ_OBJECT_CEverQuest (*EQ_OBJECT_ppCEverquest)

EQCXWNDMANGER** EQ_OBJECT_ppCXWndManager = (EQCXWNDMANGER**)EQ_POINTER_CXWndManager;
#define EQ_OBJECT_CXWndManager (*EQ_OBJECT_ppCXWndManager)

EQCITEMDISPLAYWND** EQ_OBJECT_ppCItemDisplayWnd = (EQCITEMDISPLAYWND**)EQ_POINTER_CItemDisplayWnd;
#define EQ_OBJECT_CItemDisplayWnd (*EQ_OBJECT_ppCItemDisplayWnd)

EQCBUFFWINDOW** EQ_OBJECT_ppCBuffWindow = (EQCBUFFWINDOW**)EQ_POINTER_CBuffWindow;
#define EQ_OBJECT_CBuffWindow (*EQ_OBJECT_ppCBuffWindow)

class CXStr;
class CXWndManager;
class CSidlScreenWnd;
class CXWnd;
class CDisplay;
class CEverQuest;
class EQPlayer;
class EQ_Character;
class EQ_Item;
class EQ_Spell;
class CHotButtonWnd;
class CLootWnd;
class CTradeWnd;
class CItemDisplayWnd;
class CBuffWindow;

class CXStr
{
public:
    CXStr::~CXStr(void);
    CXStr::CXStr(char const *);
    void CXStr::operator+=(char const *);
    void CXStr::operator=(char const *);
};

class CXWndManager
{
public:
    int CXWndManager::DrawCursor(void) const;
    int CXWndManager::DrawWindows(void) const;
};

CXWndManager** EQ_CLASS_ppCXWndManager = (CXWndManager**)EQ_POINTER_CXWndManager;
#define EQ_CLASS_CXWndManager (*EQ_CLASS_ppCXWndManager)

class CSidlScreenWnd
{
public:
    //
};

class CXWnd
{
public:
    //
};

class CDisplay
{
public:
    void CDisplay::MoveLocalPlayerToSafeCoords(void);
    static int __cdecl CDisplay::WriteTextHD2(const char* text, int x, int y, int color, int font);
};

CDisplay** EQ_CLASS_ppCDisplay = (CDisplay**)EQ_POINTER_CDisplay;
#define EQ_CLASS_CDisplay (*EQ_CLASS_ppCDisplay)

class CEverQuest
{
public:
    void CEverQuest::InterpretCmd(class EQPlayer* spawn, char* text);
    void CEverQuest::dsp_chat(const char* text, short color, bool filtered);
    void CEverQuest::dsp_chat(const char* text);
    int __cdecl CEverQuest::LMouseDown(unsigned short x, unsigned short y);
    int __cdecl CEverQuest::LMouseUp(unsigned short x, unsigned short y);
    int __cdecl CEverQuest::RMouseDown(unsigned short x, unsigned short y);
    int __cdecl CEverQuest::RMouseUp(unsigned short x, unsigned short y);
    char* CEverQuest::trimName(char* spawnName);
    int CEverQuest::MoveToZone(char* zoneShortName, char* text, int destinationType, int zoneRequestReason);
    int CEverQuest::MoveToZone(int zoneId, char* text, int destinationType, int zoneRequestReason);
    int CEverQuest::LootCorpse(class EQPlayer* spawn, int unknown);
};

CEverQuest** EQ_CLASS_ppCEverQuest = (CEverQuest**)EQ_POINTER_CEverQuest;
#define EQ_CLASS_CEverQuest (*EQ_CLASS_ppCEverQuest)

class EQPlayer
{
public:
    void EQPlayer::ChangeHeight(float height);
    void EQPlayer::ChangePosition(uint8_t standingState);
    void EQPlayer::FacePlayer(class EQPlayer* spawn);
};

class EQ_Character
{
public:
    unsigned short EQ_Character::Max_Mana(void);
};

class EQ_Item
{
public:
    //
};

class EQ_Spell
{
public:
    //
};

EQ_Character** EQ_CLASS_ppEQ_Character = (EQ_Character**)EQ_POINTER_EQ_Character;
#define EQ_CLASS_EQ_Character (*EQ_CLASS_ppEQ_Character)

class CHotButtonWnd : public CSidlScreenWnd
{
public:
    void CHotButtonWnd::DoHotButton(unsigned short buttonIndex, bool allowAutoRightClick);
};

CHotButtonWnd** EQ_CLASS_ppCHotButtonWnd = (CHotButtonWnd**)EQ_POINTER_CHotButtonWnd;
#define EQ_CLASS_CHotButtonWnd (*EQ_CLASS_ppCHotButtonWnd)

class CLootWnd : public CSidlScreenWnd
{
public:
    void CLootWnd::Deactivate(void);
    void CLootWnd::RequestLootSlot(int slotIndex, bool autoLoot);
};

CLootWnd** EQ_CLASS_ppCLootWnd = (CLootWnd**)EQ_POINTER_CLootWnd;
#define EQ_CLASS_CLootWnd (*EQ_CLASS_ppCLootWnd)

class CTradeWnd : public CSidlScreenWnd
{
public:
    void CTradeWnd::Activate(class EQPlayer* spawn, bool isTargetNpc); // if (isTargetNpc == true) show Give Window
};

CTradeWnd** EQ_CLASS_ppCTradeWnd = (CTradeWnd**)EQ_POINTER_CTradeWnd;
#define EQ_CLASS_CTradeWnd (*EQ_CLASS_ppCTradeWnd)

class CItemDisplayWnd : public CSidlScreenWnd
{
public:
    void CItemDisplayWnd::SetItem(class EQ_Item* item, bool unknown);
    void CItemDisplayWnd::SetSpell(short spellId, bool hasDescription, int unknown);
};

CItemDisplayWnd** EQ_CLASS_ppCItemDisplayWnd = (CItemDisplayWnd**)EQ_POINTER_CItemDisplayWnd;
#define EQ_CLASS_CItemDisplayWnd (*EQ_CLASS_ppCItemDisplayWnd)

class CBuffWindow : public CSidlScreenWnd
{
public:
    void CBuffWindow::RefreshBuffDisplay(void);
};

CBuffWindow** EQ_CLASS_ppCBuffWindow = (CBuffWindow**)EQ_POINTER_CBuffWindow;
#define EQ_CLASS_CBuffWindow (*EQ_CLASS_ppCBuffWindow)

/* CXStr */

// constructor
#define EQ_FUNCTION_CXStr__CXStr_char_const 0x00575F30
#ifdef EQ_FUNCTION_CXStr__CXStr_char_const
EQ_FUNCTION_AT_ADDRESS(CXStr::CXStr(char const *), EQ_FUNCTION_CXStr__CXStr_char_const);
#endif

// append
#define EQ_FUNCTION_CXStr__operator_plus_equal_char_const 0x00577310
#ifdef EQ_FUNCTION_CXStr__operator_plus_equal_char_const
EQ_FUNCTION_AT_ADDRESS(void CXStr::operator+=(char const *), EQ_FUNCTION_CXStr__operator_plus_equal_char_const);
#endif

// set
#define EQ_FUNCTION_CXStr__operator_equal_char_const 0x00576190
#ifdef EQ_FUNCTION_CXStr__operator_equal_char_const
EQ_FUNCTION_AT_ADDRESS(void CXStr::operator=(char const *), EQ_FUNCTION_CXStr__operator_equal_char_const);
#endif

/* CXWndManager */

#define EQ_FUNCTION_CXWndManager__DrawCursor 0x0059E320
#ifdef EQ_FUNCTION_CXWndManager__DrawCursor
EQ_FUNCTION_AT_ADDRESS(int CXWndManager::DrawCursor(void) const, EQ_FUNCTION_CXWndManager__DrawCursor);
#endif

#define EQ_FUNCTION_CXWndManager__DrawWindows 0x0059E000
#ifdef EQ_FUNCTION_CXWndManager__DrawWindows
EQ_FUNCTION_AT_ADDRESS(int CXWndManager::DrawWindows(void) const, EQ_FUNCTION_CXWndManager__DrawWindows);
#endif

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

#define EQ_FUNCTION_CEverQuest__InterpretCmd 0x0054572F
#ifdef EQ_FUNCTION_CEverQuest__InterpretCmd
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__InterpretCmd)(void* this_ptr, class EQPlayer* spawn, char* text);
EQ_FUNCTION_AT_ADDRESS(void CEverQuest::InterpretCmd(class EQPlayer*, char*), EQ_FUNCTION_CEverQuest__InterpretCmd);
#endif

#define EQ_FUNCTION_CEverQuest__dsp_chat 0x00537F99
#ifdef EQ_FUNCTION_CEverQuest__dsp_chat
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__dsp_chat)(void* this_ptr, const char* text, short textColor, bool b);
EQ_FUNCTION_AT_ADDRESS(void CEverQuest::dsp_chat(const char*, short, bool), EQ_FUNCTION_CEverQuest__dsp_chat);
#endif

#define EQ_FUNCTION_CEverQuest__dsp_chat2 0x005380FD
#ifdef EQ_FUNCTION_CEverQuest__dsp_chat2
EQ_FUNCTION_AT_ADDRESS(void CEverQuest::dsp_chat(const char*), EQ_FUNCTION_CEverQuest__dsp_chat2);
#endif

#define EQ_FUNCTION_CEverQuest__LMouseDown 0x005465AA
#ifdef EQ_FUNCTION_CEverQuest__LMouseDown
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__LMouseDown)(void* this_ptr, unsigned short x, unsigned short y);
EQ_FUNCTION_AT_ADDRESS(int __cdecl CEverQuest::LMouseDown(unsigned short, unsigned short), EQ_FUNCTION_CEverQuest__LMouseDown);
#endif

#define EQ_FUNCTION_CEverQuest__LMouseUp 0x00531614
#ifdef EQ_FUNCTION_CEverQuest__LMouseUp
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__LMouseUp)(void* this_ptr, unsigned short x, unsigned short y);
EQ_FUNCTION_AT_ADDRESS(int __cdecl CEverQuest::LMouseUp(unsigned short, unsigned short), EQ_FUNCTION_CEverQuest__LMouseUp);
#endif

#define EQ_FUNCTION_CEverQuest__RMouseDown 0x0054699D
#ifdef EQ_FUNCTION_CEverQuest__RMouseDown
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__RMouseDown)(void* this_ptr, unsigned short x, unsigned short y);
EQ_FUNCTION_AT_ADDRESS(int __cdecl CEverQuest::RMouseDown(unsigned short, unsigned short), EQ_FUNCTION_CEverQuest__RMouseDown);
#endif

#define EQ_FUNCTION_CEverQuest__RMouseUp 0x00546B71
#ifdef EQ_FUNCTION_CEverQuest__RMouseUp
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__RMouseUp)(void* this_ptr, unsigned short x, unsigned short y);
EQ_FUNCTION_AT_ADDRESS(int __cdecl CEverQuest::RMouseUp(unsigned short, unsigned short), EQ_FUNCTION_CEverQuest__RMouseUp);
#endif

#define EQ_FUNCTION_CEverQuest__trimName 0x00537D39
#ifdef EQ_FUNCTION_CEverQuest__trimName
EQ_FUNCTION_AT_ADDRESS(char* CEverQuest::trimName(char*), EQ_FUNCTION_CEverQuest__trimName);
#endif

#define EQ_FUNCTION_CEverQuest__MoveToZone 0x005460B5
#ifdef EQ_FUNCTION_CEverQuest__MoveToZone
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__MoveToZone)(void* this_ptr, char*, char*, int, int);
EQ_FUNCTION_AT_ADDRESS(int CEverQuest::MoveToZone(char*, char*, int, int), EQ_FUNCTION_CEverQuest__MoveToZone);
#endif

#define EQ_FUNCTION_CEverQuest__MoveToZone2 0x00546081
#ifdef EQ_FUNCTION_CEverQuest__MoveToZone2
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__MoveToZone2)(void* this_ptr, int, char*, int, int);
EQ_FUNCTION_AT_ADDRESS(int CEverQuest::MoveToZone(int, char*, int, int), EQ_FUNCTION_CEverQuest__MoveToZone2);
#endif

#define EQ_FUNCTION_CEverQuest__LootCorpse 0x00547808
#ifdef EQ_FUNCTION_CEverQuest__LootCorpse
typedef signed int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__LootCorpse)(void* this_ptr, class EQPlayer*, int);
EQ_FUNCTION_AT_ADDRESS(int CEverQuest::LootCorpse(class EQPlayer*, int), EQ_FUNCTION_CEverQuest__LootCorpse);
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

/* CHotButtonWnd */

#define EQ_FUNCTION_CHotButtonWnd__DoHotButton 0x004209BD
#ifdef EQ_FUNCTION_CHotButtonWnd__DoHotButton
EQ_FUNCTION_AT_ADDRESS(void CHotButtonWnd::DoHotButton(unsigned short, bool), EQ_FUNCTION_CHotButtonWnd__DoHotButton);
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

/* CItemDisplayWnd */

#define EQ_FUNCTION_CItemDisplayWnd__SetItem 0x00423640
#ifdef EQ_FUNCTION_CItemDisplayWnd__SetItem
typedef int (__thiscall* EQ_FUNCTION_TYPE_CItemDisplayWnd__SetItem)(void* this_ptr, class EQ_Item* item, bool unknown);
EQ_FUNCTION_AT_ADDRESS(void CItemDisplayWnd::SetItem(class EQ_Item*, bool), EQ_FUNCTION_CItemDisplayWnd__SetItem);
#endif

#define EQ_FUNCTION_CItemDisplayWnd__SetSpell 0x00425957
#ifdef EQ_FUNCTION_CItemDisplayWnd__SetSpell
typedef int (__thiscall* EQ_FUNCTION_TYPE_CItemDisplayWnd__SetSpell)(void* this_ptr, int spellId, bool hasDescription, int unknown);
EQ_FUNCTION_AT_ADDRESS(void CItemDisplayWnd::SetSpell(short, bool, int), EQ_FUNCTION_CItemDisplayWnd__SetSpell);
#endif

/* CBuffWindow */

#define EQ_FUNCTION_CBuffWindow__SetBuffIcon 0x00409520

#define EQ_FUNCTION_CBuffWindow__RefreshBuffDisplay 0x00409334
#ifdef EQ_FUNCTION_CBuffWindow__RefreshBuffDisplay
typedef int (__thiscall* EQ_FUNCTION_TYPE_CBuffWindow__RefreshBuffDisplay)(void* this_ptr);
EQ_FUNCTION_AT_ADDRESS(void CBuffWindow::RefreshBuffDisplay(void), EQ_FUNCTION_CBuffWindow__RefreshBuffDisplay);
#endif

/* OTHER */

#define EQ_FUNCTION_DrawNetStatus 0x0054D3AE
#ifdef EQ_FUNCTION_DrawNetStatus
typedef int (__cdecl* EQ_FUNCTION_TYPE_DrawNetStatus)(int, unsigned short, unsigned short, unsigned short x, unsigned short y, int, unsigned short, unsigned long, long, unsigned long);
#endif

#define EQ_FUNCTION_HandleMouseWheel 0x0055B2E0
#ifdef EQ_FUNCTION_HandleMouseWheel
typedef int (__cdecl* EQ_FUNCTION_TYPE_HandleMouseWheel)(int mouseWheelDelta);
#endif

#define EQ_FUNCTION_ProcessKeyDown 0x005257FA
#ifdef EQ_FUNCTION_ProcessKeyDown
typedef int (__cdecl* EQ_FUNCTION_TYPE_ProcessKeyDown)(int key);
#endif

#define EQ_FUNCTION_ProcessKeyUp 0x0052462A
#ifdef EQ_FUNCTION_ProcessKeyUp
typedef int (__cdecl* EQ_FUNCTION_TYPE_ProcessKeyUp)(int key);
#endif

#define EQ_FUNCTION_CastRay 0x004F20DB
#ifdef EQ_FUNCTION_CastRay
EQ_FUNCTION_AT_ADDRESS(int __cdecl EQ_CastRay(class EQPlayer* spawn, float y, float x, float z), EQ_FUNCTION_CastRay);
#endif

/* EQGfx_Dx8.DLL */

// world to screen function
typedef int (__cdecl* EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dWorldSpaceToScreenSpace)(int cameraDataPointer, EQLOCATION*, float* resultX, float* resultY);
EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dWorldSpaceToScreenSpace EQGfx_Dx8__t3dWorldSpaceToScreenSpace;

// draw line function
typedef int (__cdecl* EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dDeferLine)(EQLINE*, int argbColor);
EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dDeferLine EQGfx_Dx8__t3dDeferLine;

// draw rectangle function
typedef int (__cdecl* EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dDeferRect)(EQRECT*, int argbColor);
EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dDeferRect EQGfx_Dx8__t3dDeferRect;

// draw quad function (filled rectangle)
typedef int (__cdecl* EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dDeferQuad)(EQRECT*, int argbColor);
EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dDeferQuad EQGfx_Dx8__t3dDeferQuad;

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

void EQ_ToggleBool(bool& b)
{
    b = !b;
}

float EQ_CalculateDistance(float x1, float y1, float x2, float y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

float EQ_CalculateDistance3d(float x1, float y1, float z1, float x2, float y2, float z2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2));
}

void EQ_Rotate2d(float cx, float cy, float& x, float& y, float angle)
{
    float radians = angle * (EQ_PI / 256.0f); // 512 / 2 = 256

    float c = cosf(radians);
    float s = sinf(radians);

    float nx = (c * (x - cx)) - (s * (y - cy)) + cx;
    float ny = (s * (x - cx)) + (c * (y - cy)) + cy;

    x = nx;
    y = ny;
}

void EQ_CalculateItemCost(int cost, int& platinum, int& gold, int& silver, int& copper)
{
    // cost is in total copper value of item

    if (cost > 0)
    {
        platinum = cost / 1000;

        cost = cost % 1000;

        gold = cost / 100;

        cost = cost % 100;

        silver = cost / 10;

        cost = cost % 10;

        copper = cost;
    }
}

void EQ_GetItemCostString(int cost, char result[], size_t resultSize)
{
    int platinum = 0;
    int gold     = 0;
    int silver   = 0;
    int copper   = 0;

    EQ_CalculateItemCost(cost, platinum, gold, silver, copper);

    char costText[128] = {0};

    if (platinum > 0)
    {
        char platinumText[128];
        sprintf_s(platinumText, "%dp", platinum);

        strcat_s(costText, platinumText);
    }

    if (gold > 0)
    {
        if (platinum > 0)
        {
            strcat_s(costText, " ");
        }

        char goldText[128];
        sprintf_s(goldText, "%dg", gold);

        strcat_s(costText, goldText);
    }

    if (silver > 0)
    {
        if (platinum > 0 || gold > 0)
        {
            strcat_s(costText, " ");
        }

        char silverText[128];
        sprintf_s(silverText, "%ds", silver);

        strcat_s(costText, silverText);
    }

    if (copper > 0)
    {
        if (platinum > 0 || gold > 0 || silver > 0)
        {
            strcat_s(costText, " ");
        }

        char copperText[128];
        sprintf_s(copperText, "%dc", copper);

        strcat_s(costText, copperText);
    }

    strcpy_s(result, resultSize, costText);
}

void EQ_CalculateTickTime(int ticks, int& hours, int& minutes, int& seconds)
{
    if (ticks > 0)
    {
        seconds = ticks * 3;

        if (seconds > 0)
        {
            hours = seconds / (60 * 60);

            seconds = seconds - hours * (60 * 60);

            if (seconds > 0)
            {
                minutes = seconds / 60;

                seconds = seconds - minutes * 60;
            }
        }
    }
}

void EQ_GetTickTimeString(int ticks, char result[], size_t resultSize)
{
    int hours   = 0;
    int minutes = 0;
    int seconds = 0;

    EQ_CalculateTickTime(ticks, hours, minutes, seconds);

    char timeText[128] = {0};

    if (hours > 0)
    {
        char hoursText[128];
        sprintf_s(hoursText, "%dh", hours);

        strcat_s(timeText, hoursText);
    }

    if (minutes > 0)
    {
        if (hours > 0)
        {
            strcat_s(timeText, " ");
        }

        char minutesText[128];
        sprintf_s(minutesText, "%dm", minutes);

        strcat_s(timeText, minutesText);
    }

    if (seconds > 0)
    {
        if (hours > 0 || minutes > 0)
        {
            strcat_s(timeText, " ");
        }

        char secondsText[128];
        sprintf_s(secondsText, "%ds", seconds);

        strcat_s(timeText, secondsText);
    }

    strcpy_s(result, resultSize, timeText);
}

const char* EQ_KEYVALUESTRINGLIST_GetValueByKey(const char* list[][2], size_t listSize, char key[])
{
    for (size_t i = 0; i < listSize; i++)
    {
        if (strcmp(list[i][0], key) == 0)
        {
            return list[i][1]; // return Value if Key is found
        }
    }

    return NULL;
}

int EQ_GetTextColorIdByName(const char* name)
{
    for (size_t i = 0; i < EQ_STRINGSIZE_TEXT_COLOR_NAME; i++)
    {
        if (strcmp(EQ_STRING_TEXT_COLOR_NAME[i], name) == 0)
        {
            return i;
        }
    }

    return EQ_TEXT_COLOR_WHITE;
}

const char* EQ_GetRaceName(int race)
{
    const char* name = NULL;

    switch (race)
    {
        case EQ_RACE_IKSAR:
            name = "Iksar";
            break;

        case EQ_RACE_VAH_SHIR:
            name = "Vah Shir";
            break;

        case EQ_RACE_FROGLOK:
            name = "Froglok";
            break;
    }

    if (race < (int)EQ_STRINGSIZE_RACE_NAME)
    {
        name = EQ_STRING_RACE_NAME[race];
    }

    if (name == NULL)
    {
        return "Unknown";
    }

    return name;
}

const char* EQ_GetRaceShortName(int race)
{
    const char* name = NULL;

    switch (race)
    {
        case EQ_RACE_IKSAR:
            name = "IKS";
            break;

        case EQ_RACE_VAH_SHIR:
            name = "VAH";
            break;

        case EQ_RACE_FROGLOK:
            name = "FRG";
            break;
    }

    if (race < (int)EQ_STRINGSIZE_RACE_SHORT_NAME)
    {
        name = EQ_STRING_RACE_SHORT_NAME[race];
    }

    if (name == NULL)
    {
        return "UNK";
    }

    return name;
}

const char* EQ_GetClassName(int _class)
{
    return EQ_STRING_CLASS_NAME[_class];
}

const char* EQ_GetClassShortName(int _class)
{
    return EQ_STRING_CLASS_SHORT_NAME[_class];
}

const char* EQ_GetStandingStateString(int standingState)
{
    const char* standingStateString = NULL;

    switch (standingState)
    {
        case EQ_STANDING_STATE_STANDING:
            standingStateString = "Standing";
            break;

        case EQ_STANDING_STATE_FROZEN:
            standingStateString = "Mesmerized / Feared";
            break;

        case EQ_STANDING_STATE_LOOTING:
            standingStateString = "Looting / Binding Wounds";
            break;

        case EQ_STANDING_STATE_SITTING:
            standingStateString = "Sitting";
            break;

        case EQ_STANDING_STATE_DUCKING:
            standingStateString = "Ducking";
            break;

        case EQ_STANDING_STATE_FEIGNED:
            standingStateString = "Feigning Death";
            break;

        case EQ_STANDING_STATE_DEAD:
            standingStateString = "Dead";
            break;
    }

    if (standingStateString == NULL)
    {
        return "Unknown";
    }

    return standingStateString;
}

const char* EQ_GetCardinalDirectionByHeading(float heading)
{
    const char* direction = NULL;

    heading = roundf(heading);

    if (heading >= 0.0f && heading <= 31.0f)
    {
        direction = "North";
    }
    else if (heading >= 32.0f && heading <= 95.0f)
    {
        direction = "North West";
    }
    else if (heading >= 96.0f && heading <= 159.0f)
    {
        direction = "West";
    }
    else if (heading >= 160.0f && heading <= 223.0f)
    {
        direction = "South West";
    }
    else if (heading >= 224.0f && heading <= 287.0f)
    {
        direction = "South";
    }
    else if (heading >= 288.0f && heading <= 351.0f)
    {
        direction = "South East";
    }
    else if (heading >= 352.0f && heading <= 415.0f)
    {
        direction = "East";
    }
    else if (heading >= 416.0f && heading <= 479.0f)
    {
        direction = "North East";
    }
    else if (heading >= 480.0f && heading <= 512.0f)
    {
        direction = "North";
    }

    return direction;
}

void EQ_CXStr_Append(PEQCXSTR* cxstr, PCHAR text)
{
    CXStr *temp = (CXStr*)cxstr;

    (*temp) += text;

    cxstr = (PEQCXSTR*)temp;
}

void EQ_CXStr_Set(PEQCXSTR* cxstr, PCHAR text) 
{ 
    CXStr *temp = (CXStr*)cxstr;

    (*temp) = text;

    cxstr = (PEQCXSTR*)temp;
}

char* EQ_GetGuildNameById(int guildId)
{
    if (guildId == EQ_GUILD_ID_NULL)
    {
        return "Unknown Guild";
    }

    return EQ_OBJECT_GuildList.Guild[guildId].Name;
}

void EQ_WriteIntVarToChat(const char* name, int value)
{
    char text[128];
    sprintf_s(text, _TRUNCATE, "%s: %d", name, value);

    EQ_CLASS_CEverQuest->dsp_chat(text);
}

void EQ_WriteHexVarToChat(const char* name, int value)
{
    char text[128];
    sprintf_s(text, _TRUNCATE, "%s: 0x%08X", value);

    EQ_CLASS_CEverQuest->dsp_chat(text);
}

void EQ_WriteFloatVarToChat(const char* name, float value)
{
    char text[128];
    sprintf_s(text, _TRUNCATE, "%s: %.1f", name, value);

    EQ_CLASS_CEverQuest->dsp_chat(text);
}

void EQ_WriteBoolVarToChat(const char* name, bool& b)
{
    char text[128];
    sprintf_s(text, _TRUNCATE, "%s: %s", name, b ? "On" : "Off");

    EQ_CLASS_CEverQuest->dsp_chat(text);
}

void EQ_WriteStringVarToChat(const char* name, char value[])
{
    char text[128];
    sprintf_s(text, _TRUNCATE, "%s: %s", name, value);

    EQ_CLASS_CEverQuest->dsp_chat(text);
}

void EQ_DrawRectangle(float x, float y, float width, float height, int color, bool filled = false)
{
    EQRECT rect;

    // top left
    rect.X1 = x;
    rect.Y1 = y;
    rect.Z1 = 1.0f;

    // top right
    rect.X2 = x + width;
    rect.Y2 = y;
    rect.Z2 = 1.0f;

    // bottom right
    rect.X3 = x + width;
    rect.Y3 = y + height;
    rect.Z3 = 1.0f;

    // bottom left
    rect.X4 = x;
    rect.Y4 = y + height;
    rect.Z4 = 1.0f;

    if (filled == false)
    {
        EQGfx_Dx8__t3dDeferRect(&rect, color);
    }
    else
    {
        EQGfx_Dx8__t3dDeferQuad(&rect, color);
    }
}

int EQ_GetFontTextWidth(DWORD fontPointer, char text[])
{
    unsigned int textLength = strlen(text);

    if (textLength == 0)
    {
        return 0;
    }

    int* fontGlyphArray = NULL;

    if (fontPointer == EQ_POINTER_FONT_ARIAL10)
    {
        fontGlyphArray = EQ_FONT_GLYPH_SIZES_ARIAL10;
    }
    else if (fontPointer == EQ_POINTER_FONT_ARIAL12)
    {
        fontGlyphArray = EQ_FONT_GLYPH_SIZES_ARIAL12;
    }
    else if (fontPointer == EQ_POINTER_FONT_ARIAL14)
    {
        fontGlyphArray = EQ_FONT_GLYPH_SIZES_ARIAL14;
    }
    else if (fontPointer == EQ_POINTER_FONT_ARIAL15)
    {
        fontGlyphArray = EQ_FONT_GLYPH_SIZES_ARIAL15;
    }
    else if (fontPointer == EQ_POINTER_FONT_ARIAL16)
    {
        fontGlyphArray = EQ_FONT_GLYPH_SIZES_ARIAL16;
    }
    else if (fontPointer == EQ_POINTER_FONT_ARIAL20)
    {
        fontGlyphArray = EQ_FONT_GLYPH_SIZES_ARIAL20;
    }

    if (fontGlyphArray == NULL)
    {
        return 0;
    }

    int width = 0;

    for (size_t i = 0; i < textLength; i++)
    {
        int asciiIndex = (int)text[i];

        if (asciiIndex > 128)
        {
            continue;
        }

        width += fontGlyphArray[asciiIndex];
    }

    return width;
}

void EQ_DrawTooltipText(char text[], int x, int y, DWORD fontPointer)
{
    unsigned int textLength = strlen(text);

    if (textLength == 0)
    {
        return;
    }

    DWORD font = EQ_READ_MEMORY<DWORD>(fontPointer);

    int fontHeight = 0;

    if (fontPointer == EQ_POINTER_FONT_ARIAL10)
    {
        fontHeight = 10;
    }
    else if (fontPointer == EQ_POINTER_FONT_ARIAL12)
    {
        fontHeight = 12;
    }
    else if (fontPointer == EQ_POINTER_FONT_ARIAL14)
    {
        fontHeight = 14;
    }
    else if (fontPointer == EQ_POINTER_FONT_ARIAL15)
    {
        fontHeight = 15;
    }
    else if (fontPointer == EQ_POINTER_FONT_ARIAL16)
    {
        fontHeight = 16;
    }
    else if (fontPointer == EQ_POINTER_FONT_ARIAL20)
    {
        fontHeight = 20;
    }

    if (fontHeight == 0)
    {
        return;
    }

    int textWidth = EQ_GetFontTextWidth(fontPointer, text);

    if (textWidth == 0)
    {
        return;
    }

    textWidth = textWidth + 1;

    int textX = x - 1;

    EQ_DrawRectangle((float)textX, (float)y, (float)textWidth, (float)fontHeight, EQ_TOOLTIP_TEXT_BACKGROUND_COLOR, true);

    EQ_CLASS_CDisplay->WriteTextHD2(text, x, y, EQ_TEXT_COLOR_WHITE, font);
}

void EQ_ApplyClassicUiDrawOffset(int& x, int& y)
{
    BYTE uiState = EQ_READ_MEMORY<BYTE>(EQ_UI_STATE);

    if (uiState == EQ_UI_STATE_CLASSIC)
    {
        DWORD resolutionWidth  = EQ_READ_MEMORY<DWORD>(EQ_RESOLUTION_WIDTH);
        DWORD resolutionHeight = EQ_READ_MEMORY<DWORD>(EQ_RESOLUTION_HEIGHT);

        if (resolutionWidth > EQ_CLASSIC_UI_WIDTH || resolutionHeight > EQ_CLASSIC_UI_HEIGHT)
        {
            int offsetX = (int)((resolutionWidth  - EQ_CLASSIC_UI_WIDTH)  * 0.5f);
            int offsetY = (int)((resolutionHeight - EQ_CLASSIC_UI_HEIGHT) * 0.5f);

            x = x + offsetX;
            y = y + offsetY;
        }
    }
}

void EQ_TargetSpawn(PEQSPAWNINFO spawn)
{
    if (spawn)
    {
        EQ_WRITE_MEMORY<PEQSPAWNINFO>(EQ_POINTER_TARGET_SPAWN_INFO, spawn);
    }
}

PEQSPAWNINFO EQ_GetMyCorpse()
{
    PEQSPAWNINFO spawn = (PEQSPAWNINFO)EQ_OBJECT_FirstSpawn;

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    while (spawn)
    {
        if (spawn->Type != EQ_SPAWN_TYPE_PLAYER_CORPSE)
        {
            spawn = spawn->Next;
            continue;
        }

        if (strstr(spawn->Name, playerSpawn->Name) != NULL)
        {
            return spawn;
        }

        spawn = spawn->Next;
    }

    return NULL;
}

PEQSPAWNINFO EQ_GetNearestSpawn(int spawnType)
{
    PEQSPAWNINFO spawn = (PEQSPAWNINFO)EQ_OBJECT_FirstSpawn;

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    //PEQSPAWNINFO nearestSpawn = (PEQSPAWNINFO)malloc(sizeof(EQSPAWNINFO));

    int spawnId = 0;

    float shortestDistance = 0.0f;

    while (spawn)
    {
        if (spawn->Type != spawnType && spawnType != EQ_SPAWN_TYPE_ANY_CORPSE)
        {
            spawn = spawn->Next;
            continue;
        }

        if (spawnType == EQ_SPAWN_TYPE_ANY_CORPSE)
        {
            if (spawn->Type != EQ_SPAWN_TYPE_NPC_CORPSE && spawn->Type != EQ_SPAWN_TYPE_PLAYER_CORPSE)
            {
                spawn = spawn->Next;
                continue;
            }
        }

        if (spawn == playerSpawn)
        {
            spawn = spawn->Next;
            continue;
        }

        float spawnDistance = EQ_CalculateDistance(playerSpawn->X, playerSpawn->Y, spawn->X, spawn->Y);

/*
        if (spawnDistance > 400.0f)
        {
            spawn = spawn->Next;
            continue;
        }
*/

        if (shortestDistance == 0.0f)
        {
            shortestDistance = spawnDistance;
        }

        if (spawnDistance <= shortestDistance)
        {
            shortestDistance = spawnDistance;

            spawnId = spawn->SpawnId;
        }

        spawn = spawn->Next;
    }

    spawn = (PEQSPAWNINFO)EQ_OBJECT_FirstSpawn;

    while (spawn)
    {
        if (spawn->SpawnId == spawnId)
        {
            return spawn;
        }

        spawn = spawn->Next;
    }

    return NULL;
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

            size_t found = spawn.name.find(player_name);

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
            size_t found = spawns_it->name.find(spawn_name);

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
            size_t found = loot_items_it->name.find(item_name);

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
            size_t found = loot_items_it->name.find(item_name);

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

#endif // EQMAC_FUNCTIONS_H
