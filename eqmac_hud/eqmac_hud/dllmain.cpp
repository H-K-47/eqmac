#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>

#include <windows.h>

#include "detours.h"
#pragma comment(lib, "detours.lib")

#include "eqmac.hpp"
#include "eqmac_functions.hpp"

const char* g_applicationName    = "EQMac HUD";
const char* g_applicationExeName = "eqmac_hud";

bool g_bLoaded = false;

volatile int g_bExit = 0;

HMODULE g_module;

HANDLE g_handleThreadLoad;
HANDLE g_handleThreadLoop;

EQ_FUNCTION_TYPE_CEverQuest__LMouseDown EQMACHUD_REAL_CEverQuest__LMouseDown = NULL;
EQ_FUNCTION_TYPE_CEverQuest__LMouseUp   EQMACHUD_REAL_CEverQuest__LMouseUp   = NULL;

EQ_FUNCTION_TYPE_HandleMouseWheel EQMACHUD_REAL_HandleMouseWheel = NULL;

EQ_FUNCTION_TYPE_CEverQuest__dsp_chat EQMACHUD_REAL_CEverQuest__dsp_chat = NULL;

EQ_FUNCTION_TYPE_DrawNetStatus EQMACHUD_REAL_DrawNetStatus = NULL;

float g_mathPi = 3.14159265358979f;

int g_zoneId = 0;

int g_numDeferred2dItems = 0;

int g_fontHeight = 14;

float g_elementOffset = 5.0f;

bool g_writeTextToChatWindowIsEnabled = true;

unsigned int g_previousExperience = 0;

float g_buttonWidth  = 10.0f;
float g_buttonHeight = 10.0f;

DWORD g_buttonColorEnabled     = 0xFF00FF00;
DWORD g_buttonColorDisabled    = 0xFFFF0000;
DWORD g_buttonColorMinMaxClose = 0xFFFF00FF; // - + x

int g_buttonTextColorEnabled     = EQ_TEXT_COLOR_LIGHT_GREEN;
int g_buttonTextColorDisabled    = EQ_TEXT_COLOR_RED;
int g_buttonTextColorMinMaxClose = EQ_TEXT_COLOR_PINK;

char g_buttonTextColorEnabled_string[32];
char g_buttonTextColorDisabled_string[32];
char g_buttonTextColorMinMaxClose_string[32];

float g_buttonExitX = 138.0;
float g_buttonExitY = 7.0f;

float g_buttonReloadConfigX;
float g_buttonReloadConfigY;

float g_buttonToggleMapX;
float g_buttonToggleMapY;

float g_buttonToggleEspX;
float g_buttonToggleEspY;

float g_buttonToggleEspTextX;
float g_buttonToggleEspTextY;

float g_buttonToggleEspGroundSpawnX;
float g_buttonToggleEspGroundSpawnY;

float g_buttonToggleEspDoorSpawnX;
float g_buttonToggleEspDoorSpawnY;

float g_buttonToggleEspDistanceX;
float g_buttonToggleEspDistanceY;

float g_buttonToggleEspFilterNpcX;
float g_buttonToggleEspFilterNpcY;

float g_buttonToggleHealthBarsX;
float g_buttonToggleHealthBarsY;

float g_buttonToggleBuffsX;
float g_buttonToggleBuffsY;

float g_buttonTogglePlayerInfoX;
float g_buttonTogglePlayerInfoY;

float g_buttonToggleTargetInfoX;
float g_buttonToggleTargetInfoY;

bool g_mapIsEnabled = true;

bool g_mapIsMaximized = false;

unsigned int g_mapNumLayers = 3;

bool g_mapLayer0IsEnabled = true;
bool g_mapLayer1IsEnabled = true;
bool g_mapLayer2IsEnabled = true;
bool g_mapLayer3IsEnabled = true;

bool g_mapSpawnClickToTargetIsEnabled = true;

bool g_mapArrowIsEnabled        = true;
bool g_mapZoneInfoIsEnabled     = true;
bool g_mapHeightFilterIsEnabled = false;
bool g_mapRotateIsEnabled       = false;

bool g_mapSpawnsIsEnabled = true;
bool g_mapPointsIsEnabled = true;
bool g_mapLinesIsEnabled  = true;

bool g_mapTargetLineIsEnabled         = true;
bool g_mapYourselfCorpseLineIsEnabled = true;

bool g_mapReplaceBlackLinesWithWhiteLinesIsEnabled = false;

bool g_mapShowPlayerIsEnabled       = true;
bool g_mapShowPlayerCorpseIsEnabled = true;
bool g_mapShowNpcIsEnabled          = true;
bool g_mapShowNpcCorpseIsEnabled    = true;

bool g_mapSpawnDistanceIsEnabled = true;
float g_mapSpawnDistance = 400.0f;

bool g_mapSpawnDistancePlayerIsEnabled       = false;
bool g_mapSpawnDistancePlayerCorpseIsEnabled = false;
bool g_mapSpawnDistanceNpcIsEnabled          = true;
bool g_mapSpawnDistanceNpcCorpseIsEnabled    = false;

bool g_mapSpawnFilterNpcIsEnabled = false;
char g_mapSpawnFilterNpc[4096];

float g_mapDefaultX = 5.0f;
float g_mapDefaultY = 64.0f;

float g_mapDefaultWidth  = 200.0f;
float g_mapDefaultHeight = 200.0f;

float g_mapX = g_mapDefaultX;
float g_mapY = g_mapDefaultY;

float g_mapWidth  = g_mapDefaultWidth;
float g_mapHeight = g_mapDefaultHeight;

float g_mapMaximizedX = 5.0f;
float g_mapMaximizedY = 64.0f;

float g_mapMaximizedWidth  = 640.0f;
float g_mapMaximizedHeight = 480.0f;

float g_mapMinX;
float g_mapMaxX;

float g_mapMinY;
float g_mapMaxY;

float g_mapOriginX;
float g_mapOriginY;

float g_mapHeightFilterLow  = 10.0f; // z axis
float g_mapHeightFilterHigh = 10.0f; // z axis

float g_mapZoom           = 1.0f;
float g_mapZoomMultiplier = 0.05f; // zoom speed
float g_mapZoomDefault    = 1.0f;
float g_mapZoomMinimum    = 10.0f;
float g_mapZoomMaximum    = 0.01f;

float g_mapMouseWheelZoomMultiplier = 0.5f; // mouse wheel zoom speed

float g_mapArrowRadius = 20.0f;

float g_mapCenterLineSize = 5.0f;

DWORD g_mapBorderColor     = 0xFF00FF00;
DWORD g_mapArrowColor      = 0xFF00FF00;
DWORD g_mapLineColor       = 0xDEADBEEF; // DEADBEEF uses the map text file line colors
DWORD g_mapTargetLineColor = 0xFFFF00FF;
DWORD g_mapCorpseLineColor = 0xFFFFFF00;

int g_mapDefaultTextColor      = EQ_TEXT_COLOR_LIGHT_GREEN;
int g_mapZoneInfoTextColor     = EQ_TEXT_COLOR_LIGHT_GREEN;
int g_mapPointTextColor        = EQ_TEXT_COLOR_LIGHT_GRAY;
int g_mapPlayerTextColor       = EQ_TEXT_COLOR_RED;
int g_mapPlayerCorpseTextColor = EQ_TEXT_COLOR_YELLOW;
int g_mapNpcTextColor          = EQ_TEXT_COLOR_CYAN;
int g_mapNpcCorpseTextColor    = EQ_TEXT_COLOR_YELLOW;
int g_mapGroupMemberTextColor  = EQ_TEXT_COLOR_LIGHT_GREEN;
int g_mapTargetTextColor       = EQ_TEXT_COLOR_PINK;
int g_mapGameMasterTextColor   = EQ_TEXT_COLOR_PINK;

char g_mapDefaultTextColor_string[32];
char g_mapZoneInfoTextColor_string[32];
char g_mapPointTextColor_string[32];
char g_mapPlayerTextColor_string[32];
char g_mapPlayerCorpseTextColor_string[32];
char g_mapNpcTextColor_string[32];
char g_mapNpcCorpseTextColor_string[32];
char g_mapGroupMemberTextColor_string[32];
char g_mapTargetTextColor_string[32];
char g_mapGameMasterTextColor_string[32];

float g_mapButtonReloadMapX;
float g_mapButtonReloadMapY;

float g_mapButtonCopyTargetMapLocationToClipboardX;
float g_mapButtonCopyTargetMapLocationToClipboardY;

float g_mapButtonToggleLayer0X;
float g_mapButtonToggleLayer0Y;

float g_mapButtonToggleLayer1X;
float g_mapButtonToggleLayer1Y;

float g_mapButtonToggleLayer2X;
float g_mapButtonToggleLayer2Y;

float g_mapButtonToggleLayer3X;
float g_mapButtonToggleLayer3Y;

float g_mapButtonMaximizeX;
float g_mapButtonMaximizeY;

float g_mapButtonZoomInX;
float g_mapButtonZoomInY;

float g_mapButtonZoomOutX;
float g_mapButtonZoomOutY;

float g_mapButtonZoom1X;
float g_mapButtonZoom1Y;

float g_mapButtonToggleLineColorX;
float g_mapButtonToggleLineColorY;

float g_mapButtonToggleArrowX;
float g_mapButtonToggleArrowY;

float g_mapButtonToggleHeightFilterX;
float g_mapButtonToggleHeightFilterY;

float g_mapButtonToggleRotateX;
float g_mapButtonToggleRotateY;

float g_mapButtonToggleZoneInfoX;
float g_mapButtonToggleZoneInfoY;

float g_mapButtonToggleSpawnsX;
float g_mapButtonToggleSpawnsY;

float g_mapButtonTogglePointsX;
float g_mapButtonTogglePointsY;

float g_mapButtonToggleLinesX;
float g_mapButtonToggleLinesY;

float g_mapButtonToggleSpawnDistanceX;
float g_mapButtonToggleSpawnDistanceY;

float g_mapButtonToggleSpawnFilterNpcX;
float g_mapButtonToggleSpawnFilterNpcY;

struct _EQMAPLINE*  g_mapLineListBegin  = NULL;
struct _EQMAPPOINT* g_mapPointListBegin = NULL;

bool g_espIsEnabled = true;

bool g_espShowYourselfIsEnabled          = true;
bool g_espShowPlayerIsEnabled            = true;
bool g_espShowPlayerCorpseIsEnabled      = true;
bool g_espShowNpcIsEnabled               = true;
bool g_espShowNpcCorpseIsEnabled         = true;
bool g_espShowGroundSpawnIsEnabled       = true;
bool g_espShowDoorSpawnIsEnabled         = false;
bool g_espShowDoorUniqueSpawnIsEnabled   = true; // POK books, POK portals, etc

bool g_espShowTextIsEnabled = true; // show/hide all ESP text

bool g_espShowTextOnMouseHover = true;

bool g_espShowTextDistanceIsEnabled           = true;
bool g_espShowTextLevelIsEnabled              = true;
bool g_espShowTextYourselfHpIsEnabled         = true;
bool g_espShowTextYourselfManaIsEnabled       = true;
bool g_espShowTextIsGameMasterIsEnabled       = true;
bool g_espShowTextIsPlayerKillIsEnabled       = true;
bool g_espShowTextIsAwayFromKeyboardIsEnabled = true;
bool g_espShowTextPlayerIsLinkDeadIsEnabled   = true;
bool g_espShowTextPlayerIsFeignDeathIsEnabled = true;
bool g_espShowTextPlayerGuildIsEnabled        = true;
bool g_espShowTextPlayerClassIsEnabled        = true;
bool g_espShowTextPlayerHpIsEnabled           = true;
bool g_espShowTextNpcUniqueClassIsEnabled     = true; // merchant, banker, guildmaster
bool g_espShowTextNpcHpIsEnabled              = true;

bool g_espDistanceIsEnabled = true;
float g_espDistance = 400.0f;

bool g_espDistancePlayerIsEnabled       = false;
bool g_espDistancePlayerCorpseIsEnabled = false;
bool g_espDistanceNpcIsEnabled          = true;
bool g_espDistanceNpcCorpseIsEnabled    = false;
bool g_espDistanceGroundSpawnIsEnabled  = true;
bool g_espDistanceDoorSpawnIsEnabled    = true;

bool g_espFilterNpcIsEnabled = false;
char g_espFilterNpc[4096];

int g_espPlayerTextColor       = EQ_TEXT_COLOR_RED;
int g_espPlayerCorpseTextColor = EQ_TEXT_COLOR_YELLOW;
int g_espNpcTextColor          = EQ_TEXT_COLOR_CYAN;
int g_espNpcCorpseTextColor    = EQ_TEXT_COLOR_YELLOW;
int g_espGroupMemberTextColor  = EQ_TEXT_COLOR_LIGHT_GREEN;
int g_espTargetTextColor       = EQ_TEXT_COLOR_PINK;
int g_espGameMasterTextColor   = EQ_TEXT_COLOR_PINK;
int g_espGroundSpawnTextColor  = EQ_TEXT_COLOR_WHITE;
int g_espDoorSpawnTextColor    = EQ_TEXT_COLOR_WHITE;

char g_espPlayerTextColor_string[32];
char g_espPlayerCorpseTextColor_string[32];
char g_espNpcTextColor_string[32];
char g_espNpcCorpseTextColor_string[32];
char g_espGroupMemberTextColor_string[32];
char g_espTargetTextColor_string[32];
char g_espGameMasterTextColor_string[32];
char g_espGroundSpawnTextColor_string[32];
char g_espDoorSpawnTextColor_string[32];

bool g_buffsIsEnabled = true;

float g_buffsX = 768.0f;
float g_buffsY = 32.0f;

int g_buffsTextColor = EQ_TEXT_COLOR_YELLOW;

char g_buffsTextColor_string[32];

bool g_playerInfoIsEnabled = true;

float g_playerInfoX = 768.0f;
float g_playerInfoY = 256.0f;

int g_playerInfoTextColor = EQ_TEXT_COLOR_CYAN;

char g_playerInfoTextColor_string[32];

bool g_targetInfoIsEnabled = true;

float g_targetInfoX = 768.0f;
float g_targetInfoY = 384.0f;

int g_targetInfoTextColor = EQ_TEXT_COLOR_PINK;

char g_targetInfoTextColor_string[32];

bool g_messageTextIsEnabled = true;

bool g_messageTextIsCentered = true;

bool g_messageTextGainedExperienceIsEnabled = true;

char g_messageText[128];
int g_messageTextColor = EQ_TEXT_COLOR_WHITE;

float g_messageTextX = 512.0f;
float g_messageTextY = 256.0f;

unsigned int g_messageTextTimer = 0;
unsigned int g_messageTextDelay = 10000; // delay in milliseconds

bool g_itemDisplayTextIsEnabled = true;

bool g_itemDisplayTextItemValueIsEnabled = true;

bool g_healthBarsIsEnabled = true;

bool g_healthBarsClickToTargetIsEnabled = true;

float g_healthBarsWidth  = 50.0f;
float g_healthBarsHeight = 10.0f;

#define EQMACHUD_HEALTH_BAR_DEFAULT_BACKGROUND_COLOR 0xC8000000
#define EQMACHUD_HEALTH_BAR_DEFAULT_FOREGROUND_COLOR 0xC8FF0000

DWORD g_healthBarsBackgroundColor       = 0xC8000000;
DWORD g_healthBarsForegroundPlayerColor = 0xC800FF00;
DWORD g_healthBarsForegroundNpcColor    = 0xC8FF0000;
DWORD g_healthBarsForegroundTargetColor = 0xC8FF00FF;

bool g_healthBarsDistanceIsEnabled = true;
float g_healthBarsDistance = 400.0f;

// Cohen-Sutherland algorithm
// http://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
// need this to clip lines within the map window rectangle
#define EQMACHUD_LINECLIP_INSIDE 0 // 0000
#define EQMACHUD_LINECLIP_LEFT   1 // 0001
#define EQMACHUD_LINECLIP_RIGHT  2 // 0010
#define EQMACHUD_LINECLIP_BOTTOM 4 // 0100
#define EQMACHUD_LINECLIP_TOP    8 // 1000

int EQMACHUD_GetLineClipValue(float x, float y, float minX, float minY, float maxX, float maxY)
{
    int value = EQMACHUD_LINECLIP_INSIDE;
 
    if (x < minX)
    {
        value |= EQMACHUD_LINECLIP_LEFT;
    }
    else if (x > maxX)
    {
        value |= EQMACHUD_LINECLIP_RIGHT;
    }

    if (y < minY)
    {
        value |= EQMACHUD_LINECLIP_BOTTOM;
    }
    else if (y > maxY)
    {
        value |= EQMACHUD_LINECLIP_TOP;
    }
 
    return value;
}

bool EQMACHUD_DoClipLine(EQLINE* line, float minX, float minY, float maxX, float maxY)
{
    bool bDrawLine = false;

    // don't clip lines into the rectangle, offset by 1 pixel
    minX = minX + 1.0f;
    minY = minY + 1.0f;
    maxX = maxX - 1.0f;
    maxY = maxY - 1.0f;

    int lineClipValue1 = EQMACHUD_GetLineClipValue(line->X1, line->Y1, minX, minY, maxX, maxY);
    int lineClipValue2 = EQMACHUD_GetLineClipValue(line->X2, line->Y2, minX, minY, maxX, maxY);

    while (true)
    {
        if (!(lineClipValue1 | lineClipValue2))
        {
            bDrawLine = true;
            break;
        }
        else if (lineClipValue1 & lineClipValue2)
        {
            break;
        }
        else
        {
            float x;
            float y;
 
            int lineClipValueOut = lineClipValue1 ? lineClipValue1 : lineClipValue2;
 
            if (lineClipValueOut & EQMACHUD_LINECLIP_TOP)
            {
                x = line->X1 + (line->X2 - line->X1) * (maxY - line->Y1) / (line->Y2 - line->Y1);
                y = maxY;
            }
            else if (lineClipValueOut & EQMACHUD_LINECLIP_BOTTOM)
            {
                x = line->X1 + (line->X2 - line->X1) * (minY - line->Y1) / (line->Y2 - line->Y1);
                y = minY;
            }
            else if (lineClipValueOut & EQMACHUD_LINECLIP_RIGHT)
            {
                y = line->Y1 + (line->Y2 - line->Y1) * (maxX - line->X1) / (line->X2 - line->X1);
                x = maxX;
            }
            else if (lineClipValueOut & EQMACHUD_LINECLIP_LEFT)
            {
                y = line->Y1 + (line->Y2 - line->Y1) * (minX - line->X1) / (line->X2 - line->X1);
                x = minX;
            }
 
            if (lineClipValueOut == lineClipValue1)
            {
                line->X1 = x;
                line->Y1 = y;
                lineClipValue1 = EQMACHUD_GetLineClipValue(line->X1, line->Y1, minX, minY, maxX, maxY);
            }
            else
            {
                line->X2 = x;
                line->Y2 = y;
                lineClipValue2 = EQMACHUD_GetLineClipValue(line->X2, line->Y2, minX, minY, maxX, maxY);
            }
        }
    }

    return bDrawLine;
}

bool EQMACHUD_IsPointInsideRectangle(int pointX, int pointY, int rectX, int rectY, int rectWidth, int rectHeight)
{
    if (pointX < rectX)                return false;
    if (pointY < rectY)                return false;
    if (pointX > (rectX + rectWidth))  return false;
    if (pointY > (rectY + rectHeight)) return false;

    return true;
}

void EQMACHUD_Rotate2D(float cx, float cy, float& x, float& y, float angle)
{
    float radians = angle * (g_mathPi / 256.0f);

    float c = cosf(radians);
    float s = sinf(radians);

    float nx = (c * (x - cx)) - (s * (y - cy)) + cx;
    float ny = (s * (x - cx)) + (c * (y - cy)) + cy;

    x = nx;
    y = ny;
}

bool EQMACHUD_FileExists(const char* filename)
{
    FILE* file;
    errno_t fileErrorNumber = fopen_s(&file, filename, "r");

    if (file)
    {
        fclose(file);
    }

    return (fileErrorNumber == 0);
}

int EQMACHUD_ConfigReadInt(const char* filename, const char* section, const char* key, int defaultValue)
{
    return GetPrivateProfileIntA(section, key, defaultValue, filename);
}

bool EQMACHUD_ConfigReadBool(const char* filename, const char* section, const char* key, int defaultValue)
{
    int result = GetPrivateProfileIntA(section, key, defaultValue, filename);

    return (result != 0);
}

float EQMACHUD_ConfigReadFloat(const char* filename, const char* section, const char* key, float defaultValue)
{
    char bufferDefault[255];
    sprintf_s(bufferDefault, "%f", defaultValue);

    char bufferResult[255];
    GetPrivateProfileStringA(section, key, bufferDefault, bufferResult, 255, filename);

    float result = (float)atof(bufferResult);

    return result;
}

void EQMACHUD_ConfigReadString(const char* filename, const char* section, const char* key, const char* defaultValue, char result[], size_t resultSize)
{
    GetPrivateProfileStringA(section, key, defaultValue, result, resultSize, filename);
}

bool EQMACHUD_LoadConfig(const char* filename)
{
    if (EQMACHUD_FileExists(filename) == false)
    {
        return false;
    }

    // HUD

    g_writeTextToChatWindowIsEnabled = EQMACHUD_ConfigReadBool(filename, "HUD", "bWriteTextToChatWindow", g_writeTextToChatWindowIsEnabled);

    g_buttonExitX = EQMACHUD_ConfigReadFloat(filename, "HUD", "fButtonExitX", g_buttonExitX);
    g_buttonExitY = EQMACHUD_ConfigReadFloat(filename, "HUD", "fButtonExitY", g_buttonExitY);

    g_buttonColorEnabled     = EQMACHUD_ConfigReadInt(filename, "HUD", "argbButtonColorEnabled",     g_buttonColorEnabled);
    g_buttonColorDisabled    = EQMACHUD_ConfigReadInt(filename, "HUD", "argbButtonColorDisabled",    g_buttonColorDisabled);
    g_buttonColorMinMaxClose = EQMACHUD_ConfigReadInt(filename, "HUD", "argbButtonColorMinMaxClose", g_buttonColorMinMaxClose);

    EQMACHUD_ConfigReadString(filename, "HUD", "sButtonTextColorEnabled",     "Light Green", g_buttonTextColorEnabled_string,     sizeof(g_buttonTextColorEnabled_string));
    EQMACHUD_ConfigReadString(filename, "HUD", "sButtonTextColorDisabled",    "Red",         g_buttonTextColorDisabled_string,    sizeof(g_buttonTextColorDisabled_string));
    EQMACHUD_ConfigReadString(filename, "HUD", "sButtonTextColorMinMaxClose", "Pink",        g_buttonTextColorMinMaxClose_string, sizeof(g_buttonTextColorMinMaxClose_string));
    g_buttonTextColorEnabled     = EQ_GetTextColorIdByName(g_buttonTextColorEnabled_string);
    g_buttonTextColorDisabled    = EQ_GetTextColorIdByName(g_buttonTextColorDisabled_string);
    g_buttonTextColorMinMaxClose = EQ_GetTextColorIdByName(g_buttonTextColorMinMaxClose_string);

    // Map

    g_mapIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bEnabled", g_mapIsEnabled);

    g_mapIsMaximized = EQMACHUD_ConfigReadBool(filename, "Map", "bIsMaximized", g_mapIsMaximized);

    g_mapLayer0IsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bLayer0", g_mapLayer0IsEnabled);
    g_mapLayer1IsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bLayer1", g_mapLayer1IsEnabled);
    g_mapLayer2IsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bLayer2", g_mapLayer2IsEnabled);
    g_mapLayer3IsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bLayer3", g_mapLayer3IsEnabled);

    g_mapSpawnClickToTargetIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bSpawnClickToTarget", g_mapSpawnClickToTargetIsEnabled);

    g_mapArrowIsEnabled        = EQMACHUD_ConfigReadBool(filename, "Map", "bArrow",        g_mapArrowIsEnabled);
    g_mapZoneInfoIsEnabled     = EQMACHUD_ConfigReadBool(filename, "Map", "bZoneInfo",     g_mapZoneInfoIsEnabled);
    g_mapHeightFilterIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bHeightFilter", g_mapHeightFilterIsEnabled);
    g_mapRotateIsEnabled       = EQMACHUD_ConfigReadBool(filename, "Map", "bRotate",       g_mapRotateIsEnabled);

    g_mapSpawnsIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bSpawns", g_mapSpawnsIsEnabled);
    g_mapPointsIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bPoints", g_mapPointsIsEnabled);
    g_mapLinesIsEnabled  = EQMACHUD_ConfigReadBool(filename, "Map", "bLines",  g_mapLinesIsEnabled);

    g_mapTargetLineIsEnabled         = EQMACHUD_ConfigReadBool(filename, "Map", "bTargetLine",         g_mapTargetLineIsEnabled);
    g_mapYourselfCorpseLineIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bYourselfCorpseLine", g_mapYourselfCorpseLineIsEnabled);

    g_mapReplaceBlackLinesWithWhiteLinesIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bReplaceBlackLinesWithWhiteLines", g_mapReplaceBlackLinesWithWhiteLinesIsEnabled);

    g_mapShowPlayerIsEnabled       = EQMACHUD_ConfigReadBool(filename, "Map", "bShowPlayer",       g_mapShowPlayerIsEnabled);
    g_mapShowPlayerCorpseIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bShowPlayerCorpse", g_mapShowPlayerCorpseIsEnabled);
    g_mapShowNpcIsEnabled          = EQMACHUD_ConfigReadBool(filename, "Map", "bShowNpc",          g_mapShowNpcIsEnabled);
    g_mapShowNpcCorpseIsEnabled    = EQMACHUD_ConfigReadBool(filename, "Map", "bShowNpcCorpse",    g_mapShowNpcCorpseIsEnabled);

    g_mapSpawnDistanceIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bSpawnDistance", g_mapSpawnDistanceIsEnabled);
    g_mapSpawnDistance = EQMACHUD_ConfigReadFloat(filename, "Map", "fSpawnDistance", g_mapSpawnDistance);

    g_mapSpawnDistancePlayerIsEnabled       = EQMACHUD_ConfigReadBool(filename, "Map", "bSpawnDistancePlayer",       g_mapSpawnDistancePlayerIsEnabled);
    g_mapSpawnDistancePlayerCorpseIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bSpawnDistancePlayerCorpse", g_mapSpawnDistancePlayerCorpseIsEnabled);
    g_mapSpawnDistanceNpcIsEnabled          = EQMACHUD_ConfigReadBool(filename, "Map", "bSpawnDistanceNpc",          g_mapSpawnDistanceNpcIsEnabled);
    g_mapSpawnDistanceNpcCorpseIsEnabled    = EQMACHUD_ConfigReadBool(filename, "Map", "bSpawnDistanceNpcCorpse",    g_mapSpawnDistanceNpcCorpseIsEnabled);

    g_mapSpawnFilterNpcIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bSpawnFilterNpc", g_mapSpawnFilterNpcIsEnabled);
    EQMACHUD_ConfigReadString(filename, "Map", "sSpawnFilterNpc", "", g_mapSpawnFilterNpc, sizeof(g_mapSpawnFilterNpc));

    g_mapDefaultX = EQMACHUD_ConfigReadFloat(filename, "Map", "fDefaultX", g_mapDefaultX);
    g_mapDefaultY = EQMACHUD_ConfigReadFloat(filename, "Map", "fDefaultY", g_mapDefaultY);

    g_mapDefaultWidth  = EQMACHUD_ConfigReadFloat(filename, "Map", "fDefaultWidth",  g_mapDefaultWidth);
    g_mapDefaultHeight = EQMACHUD_ConfigReadFloat(filename, "Map", "fDefaultHeight", g_mapDefaultHeight);

    g_mapMaximizedX = EQMACHUD_ConfigReadFloat(filename, "Map", "fMaximizedX", g_mapMaximizedX);
    g_mapMaximizedY = EQMACHUD_ConfigReadFloat(filename, "Map", "fMaximizedY", g_mapMaximizedY);

    g_mapMaximizedWidth  = EQMACHUD_ConfigReadFloat(filename, "Map", "fMaximizedWidth",  g_mapMaximizedWidth);
    g_mapMaximizedHeight = EQMACHUD_ConfigReadFloat(filename, "Map", "fMaximizedHeight", g_mapMaximizedHeight);

    g_mapHeightFilterLow  = EQMACHUD_ConfigReadFloat(filename, "Map", "fHeightFilterLow",  g_mapHeightFilterLow);
    g_mapHeightFilterHigh = EQMACHUD_ConfigReadFloat(filename, "Map", "fHeightFilterHigh", g_mapHeightFilterHigh);

    g_mapZoom           = EQMACHUD_ConfigReadFloat(filename, "Map", "fZoom",           g_mapZoom);
    g_mapZoomMultiplier = EQMACHUD_ConfigReadFloat(filename, "Map", "fZoomMultiplier", g_mapZoomMultiplier);
    g_mapZoomMinimum    = EQMACHUD_ConfigReadFloat(filename, "Map", "fZoomMinimum",    g_mapZoomMinimum);
    g_mapZoomMaximum    = EQMACHUD_ConfigReadFloat(filename, "Map", "fZoomMaximum",    g_mapZoomMaximum);

    g_mapMouseWheelZoomMultiplier = EQMACHUD_ConfigReadFloat(filename, "Map", "fMouseWheelZoomMultiplier", g_mapMouseWheelZoomMultiplier);

    g_mapArrowRadius = EQMACHUD_ConfigReadFloat(filename, "Map", "fArrowRadius", g_mapArrowRadius);

    g_mapCenterLineSize = EQMACHUD_ConfigReadFloat(filename, "Map", "fCenterLineSize", g_mapCenterLineSize);

    g_mapBorderColor     = EQMACHUD_ConfigReadInt(filename, "Map", "argbBorderColor",       g_mapBorderColor);
    g_mapArrowColor      = EQMACHUD_ConfigReadInt(filename, "Map", "argbArrowColor",        g_mapArrowColor);
    g_mapLineColor       = EQMACHUD_ConfigReadInt(filename, "Map", "argbLineColor",         g_mapLineColor);
    g_mapTargetLineColor = EQMACHUD_ConfigReadInt(filename, "Map", "argbTargetLineColor",   g_mapTargetLineColor);
    g_mapCorpseLineColor = EQMACHUD_ConfigReadInt(filename, "Map", "argbCorpseLineColor",   g_mapCorpseLineColor);

    EQMACHUD_ConfigReadString(filename, "Map", "sDefaultTextColor",      "Light Green", g_mapDefaultTextColor_string,      sizeof(g_mapDefaultTextColor_string));
    EQMACHUD_ConfigReadString(filename, "Map", "sZoneInfoTextColor",     "Light Green", g_mapZoneInfoTextColor_string,     sizeof(g_mapZoneInfoTextColor_string));
    EQMACHUD_ConfigReadString(filename, "Map", "sPointTextColor",        "Light Gray",  g_mapPointTextColor_string,        sizeof(g_mapPointTextColor_string));
    EQMACHUD_ConfigReadString(filename, "Map", "sPlayerTextColor",       "Red",         g_mapPlayerTextColor_string,       sizeof(g_mapPlayerTextColor_string));
    EQMACHUD_ConfigReadString(filename, "Map", "sPlayerCorpseTextColor", "Yellow",      g_mapPlayerCorpseTextColor_string, sizeof(g_mapPlayerCorpseTextColor_string));
    EQMACHUD_ConfigReadString(filename, "Map", "sNpcTextColor",          "Cyan",        g_mapNpcTextColor_string,          sizeof(g_mapNpcTextColor_string));
    EQMACHUD_ConfigReadString(filename, "Map", "sNpcCorpseTextColor",    "Yellow",      g_mapNpcCorpseTextColor_string,    sizeof(g_mapNpcCorpseTextColor_string));
    EQMACHUD_ConfigReadString(filename, "Map", "sGroupMemberTextColor",  "Light Green", g_mapGroupMemberTextColor_string,  sizeof(g_mapGroupMemberTextColor_string));
    EQMACHUD_ConfigReadString(filename, "Map", "sTargetTextColor",       "Pink",        g_mapTargetTextColor_string,       sizeof(g_mapTargetTextColor_string));
    EQMACHUD_ConfigReadString(filename, "Map", "sGameMasterTextColor",   "Pink",        g_mapGameMasterTextColor_string,   sizeof(g_mapGameMasterTextColor_string));
    g_mapDefaultTextColor      = EQ_GetTextColorIdByName(g_mapDefaultTextColor_string);
    g_mapZoneInfoTextColor     = EQ_GetTextColorIdByName(g_mapZoneInfoTextColor_string);
    g_mapPointTextColor        = EQ_GetTextColorIdByName(g_mapPointTextColor_string);
    g_mapPlayerTextColor       = EQ_GetTextColorIdByName(g_mapPlayerTextColor_string);
    g_mapPlayerCorpseTextColor = EQ_GetTextColorIdByName(g_mapPlayerCorpseTextColor_string);
    g_mapNpcTextColor          = EQ_GetTextColorIdByName(g_mapNpcTextColor_string);
    g_mapNpcCorpseTextColor    = EQ_GetTextColorIdByName(g_mapNpcCorpseTextColor_string);
    g_mapGroupMemberTextColor  = EQ_GetTextColorIdByName(g_mapGroupMemberTextColor_string);
    g_mapTargetTextColor       = EQ_GetTextColorIdByName(g_mapTargetTextColor_string);
    g_mapGameMasterTextColor   = EQ_GetTextColorIdByName(g_mapGameMasterTextColor_string);

    // ESP

    g_espIsEnabled = EQMACHUD_ConfigReadBool(filename, "ESP", "bEnabled", g_espIsEnabled);

    g_espShowYourselfIsEnabled        = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowYourself",        g_espShowYourselfIsEnabled);
    g_espShowPlayerIsEnabled          = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowPlayer",          g_espShowPlayerIsEnabled);
    g_espShowPlayerCorpseIsEnabled    = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowPlayerCorpse",    g_espShowPlayerCorpseIsEnabled);
    g_espShowNpcIsEnabled             = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowNpc",             g_espShowNpcIsEnabled);
    g_espShowNpcCorpseIsEnabled       = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowNpcCorpse",       g_espShowNpcCorpseIsEnabled);
    g_espShowGroundSpawnIsEnabled     = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowGroundSpawn",     g_espShowGroundSpawnIsEnabled);
    g_espShowDoorSpawnIsEnabled       = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowDoorSpawn",       g_espShowDoorSpawnIsEnabled);
    g_espShowDoorUniqueSpawnIsEnabled = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowDoorUniqueSpawn", g_espShowDoorUniqueSpawnIsEnabled);

    g_espShowTextIsEnabled = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowText", g_espShowTextIsEnabled);

    g_espShowTextOnMouseHover = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextOnMouseHover", g_espShowTextOnMouseHover);

    g_espShowTextDistanceIsEnabled           = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextDistance",           g_espShowTextDistanceIsEnabled);
    g_espShowTextLevelIsEnabled              = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextLevel",              g_espShowTextLevelIsEnabled);
    g_espShowTextYourselfHpIsEnabled         = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextYourselfHp",         g_espShowTextYourselfHpIsEnabled);
    g_espShowTextYourselfManaIsEnabled       = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextYourselfMana",       g_espShowTextYourselfManaIsEnabled);
    g_espShowTextIsGameMasterIsEnabled       = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextIsGameMaster",       g_espShowTextIsGameMasterIsEnabled);
    g_espShowTextIsPlayerKillIsEnabled       = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextIsPlayerKill",       g_espShowTextIsPlayerKillIsEnabled);
    g_espShowTextIsAwayFromKeyboardIsEnabled = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextIsAwayFromKeyboard", g_espShowTextIsAwayFromKeyboardIsEnabled);
    g_espShowTextPlayerIsLinkDeadIsEnabled   = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextPlayerIsLinkDead",   g_espShowTextPlayerIsLinkDeadIsEnabled);
    g_espShowTextPlayerIsFeignDeathIsEnabled = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextPlayerIsFeignDeath", g_espShowTextPlayerIsFeignDeathIsEnabled);
    g_espShowTextPlayerGuildIsEnabled        = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextPlayerGuild",        g_espShowTextPlayerGuildIsEnabled);
    g_espShowTextPlayerClassIsEnabled        = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextPlayerClass",        g_espShowTextPlayerClassIsEnabled);
    g_espShowTextPlayerHpIsEnabled           = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextPlayerHp",           g_espShowTextPlayerHpIsEnabled);
    g_espShowTextNpcUniqueClassIsEnabled     = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextNpcUniqueClass",     g_espShowTextNpcUniqueClassIsEnabled);
    g_espShowTextNpcHpIsEnabled              = EQMACHUD_ConfigReadBool(filename, "ESP", "bShowTextNpcHp",              g_espShowTextNpcHpIsEnabled);

    g_espDistanceIsEnabled = EQMACHUD_ConfigReadBool(filename, "ESP", "bDistance", g_espDistanceIsEnabled);
    g_espDistance = EQMACHUD_ConfigReadFloat(filename, "ESP", "fDistance", g_espDistance);

    g_espDistancePlayerIsEnabled       = EQMACHUD_ConfigReadBool(filename, "ESP", "bDistancePlayer",       g_espDistancePlayerIsEnabled);
    g_espDistancePlayerCorpseIsEnabled = EQMACHUD_ConfigReadBool(filename, "ESP", "bDistancePlayerCorpse", g_espDistancePlayerCorpseIsEnabled);
    g_espDistanceNpcIsEnabled          = EQMACHUD_ConfigReadBool(filename, "ESP", "bDistanceNpc",          g_espDistanceNpcIsEnabled);
    g_espDistanceNpcCorpseIsEnabled    = EQMACHUD_ConfigReadBool(filename, "ESP", "bDistanceNpcCorpse",    g_espDistanceNpcCorpseIsEnabled);
    g_espDistanceGroundSpawnIsEnabled  = EQMACHUD_ConfigReadBool(filename, "ESP", "bDistanceGroundSpawn",  g_espDistanceGroundSpawnIsEnabled);
    g_espDistanceDoorSpawnIsEnabled    = EQMACHUD_ConfigReadBool(filename, "ESP", "bDistanceDoorSpawn",    g_espDistanceDoorSpawnIsEnabled);

    g_espFilterNpcIsEnabled = EQMACHUD_ConfigReadBool(filename, "ESP", "bFilterNpc", g_espFilterNpcIsEnabled);
    EQMACHUD_ConfigReadString(filename, "ESP", "sFilterNpc", "", g_espFilterNpc, sizeof(g_espFilterNpc));

    EQMACHUD_ConfigReadString(filename, "ESP", "sPlayerTextColor",       "Red",         g_espPlayerTextColor_string,       sizeof(g_espPlayerTextColor_string));
    EQMACHUD_ConfigReadString(filename, "ESP", "sPlayerCorpseTextColor", "Yellow",      g_espPlayerCorpseTextColor_string, sizeof(g_espPlayerCorpseTextColor_string));
    EQMACHUD_ConfigReadString(filename, "ESP", "sNpcTextColor",          "Cyan",        g_espNpcTextColor_string,          sizeof(g_espNpcTextColor_string));
    EQMACHUD_ConfigReadString(filename, "ESP", "sNpcCorpseTextColor",    "Yellow",      g_espNpcCorpseTextColor_string,    sizeof(g_espNpcCorpseTextColor_string));
    EQMACHUD_ConfigReadString(filename, "ESP", "sGroupMemberTextColor",  "Light Green", g_espGroupMemberTextColor_string,  sizeof(g_espGroupMemberTextColor_string));
    EQMACHUD_ConfigReadString(filename, "ESP", "sTargetTextColor",       "Pink",        g_espTargetTextColor_string,       sizeof(g_espTargetTextColor_string));
    EQMACHUD_ConfigReadString(filename, "ESP", "sGameMasterTextColor",   "Pink",        g_espGameMasterTextColor_string,   sizeof(g_espGameMasterTextColor_string));
    EQMACHUD_ConfigReadString(filename, "ESP", "sGroundSpawnTextColor",  "White",       g_espGroundSpawnTextColor_string,  sizeof(g_espGroundSpawnTextColor_string));
    EQMACHUD_ConfigReadString(filename, "ESP", "sDoorSpawnTextColor",    "White",       g_espDoorSpawnTextColor_string,    sizeof(g_espDoorSpawnTextColor_string));
    g_espPlayerTextColor       = EQ_GetTextColorIdByName(g_espPlayerTextColor_string);
    g_espPlayerCorpseTextColor = EQ_GetTextColorIdByName(g_espPlayerCorpseTextColor_string);
    g_espNpcTextColor          = EQ_GetTextColorIdByName(g_espNpcTextColor_string);
    g_espNpcCorpseTextColor    = EQ_GetTextColorIdByName(g_espNpcCorpseTextColor_string);
    g_espGroupMemberTextColor  = EQ_GetTextColorIdByName(g_espGroupMemberTextColor_string);
    g_espTargetTextColor       = EQ_GetTextColorIdByName(g_espTargetTextColor_string);
    g_espGameMasterTextColor   = EQ_GetTextColorIdByName(g_espGameMasterTextColor_string);
    g_espGroundSpawnTextColor  = EQ_GetTextColorIdByName(g_espGroundSpawnTextColor_string);
    g_espDoorSpawnTextColor    = EQ_GetTextColorIdByName(g_espDoorSpawnTextColor_string);

    // HealthBars

    g_healthBarsIsEnabled = EQMACHUD_ConfigReadBool(filename, "HealthBars", "bEnabled", g_healthBarsIsEnabled);

    g_healthBarsClickToTargetIsEnabled = EQMACHUD_ConfigReadBool(filename, "HealthBars", "bClickToTarget", g_healthBarsClickToTargetIsEnabled);

    g_healthBarsWidth  = EQMACHUD_ConfigReadFloat(filename, "HealthBars", "fWidth",  g_healthBarsWidth);
    g_healthBarsHeight = EQMACHUD_ConfigReadFloat(filename, "HealthBars", "fHeight", g_healthBarsHeight);

    g_healthBarsBackgroundColor       = EQMACHUD_ConfigReadInt(filename, "HealthBars", "argbBackgroundColor",       g_healthBarsBackgroundColor);
    g_healthBarsForegroundPlayerColor = EQMACHUD_ConfigReadInt(filename, "HealthBars", "argbForegroundPlayerColor", g_healthBarsForegroundPlayerColor);
    g_healthBarsForegroundNpcColor    = EQMACHUD_ConfigReadInt(filename, "HealthBars", "argbForegroundNpcColor",    g_healthBarsForegroundNpcColor);
    g_healthBarsForegroundTargetColor = EQMACHUD_ConfigReadInt(filename, "HealthBars", "argbForegroundTargetColor", g_healthBarsForegroundTargetColor);

    g_healthBarsDistanceIsEnabled = EQMACHUD_ConfigReadBool(filename, "HealthBars", "bDistance", g_healthBarsDistanceIsEnabled);
    g_healthBarsDistance = EQMACHUD_ConfigReadFloat(filename, "HealthBars", "fDistance", g_healthBarsDistance);

    // MessageText

    g_messageTextIsEnabled = EQMACHUD_ConfigReadBool(filename, "MessageText", "bEnabled", g_messageTextIsEnabled);

    g_messageTextIsCentered = EQMACHUD_ConfigReadBool(filename, "MessageText", "bCentered", g_messageTextIsCentered);

    g_messageTextX = EQMACHUD_ConfigReadFloat(filename, "MessageText", "fX", g_messageTextX);
    g_messageTextY = EQMACHUD_ConfigReadFloat(filename, "MessageText", "fY", g_messageTextY);

    g_messageTextDelay = EQMACHUD_ConfigReadInt(filename, "MessageText", "iDelay", g_messageTextDelay);

    g_messageTextGainedExperienceIsEnabled = EQMACHUD_ConfigReadBool(filename, "MessageText", "bGainedExperience", g_messageTextGainedExperienceIsEnabled);

    // ItemDisplayText

    g_itemDisplayTextIsEnabled = EQMACHUD_ConfigReadBool(filename, "ItemDisplayText", "bEnabled", g_itemDisplayTextIsEnabled);

    g_itemDisplayTextItemValueIsEnabled = EQMACHUD_ConfigReadBool(filename, "ItemDisplayText", "bItemValue", g_itemDisplayTextItemValueIsEnabled);

    // Buffs

    g_buffsIsEnabled = EQMACHUD_ConfigReadBool(filename, "Buffs", "bEnabled", g_buffsIsEnabled);

    g_buffsX = EQMACHUD_ConfigReadFloat(filename, "Buffs", "fX", g_buffsX);
    g_buffsY = EQMACHUD_ConfigReadFloat(filename, "Buffs", "fY", g_buffsY);

    EQMACHUD_ConfigReadString(filename, "Buffs", "sTextColor", "Yellow", g_buffsTextColor_string, sizeof(g_buffsTextColor_string));
    g_buffsTextColor = EQ_GetTextColorIdByName(g_buffsTextColor_string);
    
    // PlayerInfo

    g_playerInfoIsEnabled = EQMACHUD_ConfigReadBool(filename, "PlayerInfo", "bEnabled", g_playerInfoIsEnabled);

    g_playerInfoX = EQMACHUD_ConfigReadFloat(filename, "PlayerInfo", "fX", g_playerInfoX);
    g_playerInfoY = EQMACHUD_ConfigReadFloat(filename, "PlayerInfo", "fY", g_playerInfoY);

    EQMACHUD_ConfigReadString(filename, "PlayerInfo", "sTextColor", "Cyan", g_playerInfoTextColor_string, sizeof(g_playerInfoTextColor_string));
    g_playerInfoTextColor = EQ_GetTextColorIdByName(g_playerInfoTextColor_string);

    // TargetInfo

    g_targetInfoIsEnabled = EQMACHUD_ConfigReadBool(filename, "TargetInfo", "bEnabled", g_targetInfoIsEnabled);

    g_targetInfoX = EQMACHUD_ConfigReadFloat(filename, "TargetInfo", "fX", g_targetInfoX);
    g_targetInfoY = EQMACHUD_ConfigReadFloat(filename, "TargetInfo", "fY", g_targetInfoY);

    EQMACHUD_ConfigReadString(filename, "TargetInfo", "sTextColor", "Pink", g_targetInfoTextColor_string, sizeof(g_targetInfoTextColor_string));
    g_targetInfoTextColor = EQ_GetTextColorIdByName(g_targetInfoTextColor_string);

    return true;
}

void EQMACHUD_CopyTargetMapLocationToClipboard()
{
    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    if (targetSpawn)
    {
        char locationText[256];
        sprintf_s
        (
            locationText,
            "P %.4f, %.4f, %.4f, %d, %d, %d, %d, %s",
            -targetSpawn->X, -targetSpawn->Y, targetSpawn->Z,
            0, 0, 0,
            2,
            targetSpawn->Name
        );

        const size_t len = sizeof(locationText);

        HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, len);
        memcpy(GlobalLock(mem), locationText, len);

        GlobalUnlock(mem);

        OpenClipboard(0);
        EmptyClipboard();
        SetClipboardData(CF_TEXT, mem);
        CloseClipboard();
    }
}

struct _EQMAPLINE* EQMACHUD_MapLineList_Create(EQMAPLINE* mapLine)
{
    g_mapLineListBegin = (struct _EQMAPLINE*)malloc(sizeof(struct _EQMAPLINE));

    if (g_mapLineListBegin == NULL)
    {
        return NULL;
    }

    g_mapLineListBegin->Next = NULL;

    g_mapLineListBegin->X1    = mapLine->X1;
    g_mapLineListBegin->Y1    = mapLine->Y1;
    g_mapLineListBegin->Z1    = mapLine->Z1;
    g_mapLineListBegin->X2    = mapLine->X2;
    g_mapLineListBegin->Y2    = mapLine->Y2;
    g_mapLineListBegin->Z2    = mapLine->Z2;
    g_mapLineListBegin->R     = mapLine->R;
    g_mapLineListBegin->G     = mapLine->G;
    g_mapLineListBegin->B     = mapLine->B;
    g_mapLineListBegin->Layer = mapLine->Layer;

    return g_mapLineListBegin;
}

struct _EQMAPLINE* EQMACHUD_MapLineList_Add(EQMAPLINE* mapLine)
{
    if (g_mapLineListBegin == NULL)
    {
        return (EQMACHUD_MapLineList_Create(mapLine));
    }

    struct _EQMAPLINE *node = g_mapLineListBegin;

    if (node == NULL)
    {
        return NULL;
    }

    while (node->Next != NULL)
    {
        node = node->Next;
    }

    node->Next = (struct _EQMAPLINE*)malloc(sizeof(struct _EQMAPLINE));
    node->Next->Next = NULL;

    node->Next->X1    = mapLine->X1;
    node->Next->Y1    = mapLine->Y1;
    node->Next->Z1    = mapLine->Z1;
    node->Next->X2    = mapLine->X2;
    node->Next->Y2    = mapLine->Y2;
    node->Next->Z2    = mapLine->Z2;
    node->Next->R     = mapLine->R;
    node->Next->G     = mapLine->G;
    node->Next->B     = mapLine->B;
    node->Next->Layer = mapLine->Layer;

    return node;
}

void EQMACHUD_MapLineList_Destroy()
{
    struct _EQMAPLINE *node = g_mapLineListBegin;

    if (node == NULL)
    {
        return;
    }

    struct _EQMAPLINE *next;

    while (node->Next != NULL)
    {
        next = node->Next;
        free(node);
        node = next;
    }

    g_mapLineListBegin = NULL;
}

struct _EQMAPPOINT* EQMACHUD_MapPointList_Create(EQMAPPOINT* mapPoint)
{
    g_mapPointListBegin = (struct _EQMAPPOINT*)malloc(sizeof(struct _EQMAPPOINT));

    if (g_mapPointListBegin == NULL)
    {
        return NULL;
    }

    g_mapPointListBegin->Next = NULL;

    g_mapPointListBegin->X     = mapPoint->X;
    g_mapPointListBegin->Y     = mapPoint->Y;
    g_mapPointListBegin->Z     = mapPoint->Z;
    g_mapPointListBegin->R     = mapPoint->R;
    g_mapPointListBegin->G     = mapPoint->G;
    g_mapPointListBegin->B     = mapPoint->B;
    g_mapPointListBegin->Size  = mapPoint->Size;
    g_mapPointListBegin->Layer = mapPoint->Layer;

    strcpy_s(g_mapPointListBegin->Text, mapPoint->Text);

    return g_mapPointListBegin;
}

struct _EQMAPPOINT* EQMACHUD_MapPointList_Add(EQMAPPOINT* mapPoint)
{
    if (g_mapPointListBegin == NULL)
    {
        return (EQMACHUD_MapPointList_Create(mapPoint));
    }

    struct _EQMAPPOINT *node = g_mapPointListBegin;

    if (node == NULL)
    {
        return NULL;
    }

    while (node->Next != NULL)
    {
        node = node->Next;
    }

    node->Next = (struct _EQMAPPOINT*)malloc(sizeof(struct _EQMAPPOINT));
    node->Next->Next = NULL;

    node->Next->X     = mapPoint->X;
    node->Next->Y     = mapPoint->Y;
    node->Next->Z     = mapPoint->Z;
    node->Next->R     = mapPoint->R;
    node->Next->G     = mapPoint->G;
    node->Next->B     = mapPoint->B;
    node->Next->Size  = mapPoint->Size;
    node->Next->Layer = mapPoint->Layer;

    strcpy_s(node->Next->Text, mapPoint->Text);

    return node;
}

void EQMACHUD_MapPointList_Destroy()
{
    struct _EQMAPPOINT *node = g_mapPointListBegin;

    if (node == NULL)
    {
        return;
    }

    struct _EQMAPPOINT *next;

    while (node->Next != NULL)
    {
        next = node->Next;
        free(node);
        node = next;
    }

    g_mapPointListBegin = NULL;
}

bool EQMACHUD_LoadMap(const char* filename, int layer)
{
    FILE* file;
    errno_t fileErrorNumber = fopen_s(&file, filename, "r");

    if (fileErrorNumber != 0)
    {
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strlen(line) == 0)
        {
            continue;
        }

        size_t ln = strlen(line) - 1;
        if (line[ln] == '\n' || line[ln] == '\r')
        {
            line[ln] = '\0';
        }

        if (line[0] == 'L')
        {
            EQMAPLINE mapLine;

            mapLine.Layer = (BYTE)layer;

            char lineType;

            int lineR;
            int lineG;
            int lineB;

            int result =
                sscanf_s
                (
                    line,
                    "%c %f, %f, %f, %f, %f, %f, %d, %d, %d",
                    &lineType, (rsize_t)1,
                    &mapLine.X1, &mapLine.Y1, &mapLine.Z1,
                    &mapLine.X2, &mapLine.Y2, &mapLine.Z2,
                    &lineR,      &lineG,      &lineB
                );

            mapLine.R = lineR;
            mapLine.G = lineG;
            mapLine.B = lineB;

            if (result == 10) // the number of values for a line in the map text file
            {
                EQMACHUD_MapLineList_Add(&mapLine);
            }
        }
        else if (line[0] == 'P')
        {
            EQMAPPOINT mapPoint;

            mapPoint.Layer = (BYTE)layer;

            char lineType;

            int pointR;
            int pointG;
            int pointB;

            int pointSize;

            char pointText[128];

            int result =
                sscanf_s
                (
                    line,
                    "%c %f, %f, %f, %d, %d, %d, %d, %s",
                    &lineType, (rsize_t)1,
                    &mapPoint.X, &mapPoint.Y, &mapPoint.Z,
                    &pointR,     &pointG,     &pointB,
                    &pointSize,
                    pointText, (rsize_t)sizeof(pointText)
                );

            mapPoint.R = pointR;
            mapPoint.G = pointG;
            mapPoint.B = pointB;

            mapPoint.Size = pointSize;

            for (size_t i = 0; pointText[i] != '\0'; i++)
            {
                if (pointText[i] == '_')
                {
                    pointText[i] = ' ';
                }
            }

            strcpy_s(mapPoint.Text, pointText);

            if (result == 9) // the number of values for a point in the map text file
            {
                EQMACHUD_MapPointList_Add(&mapPoint);
            }
        }
    }

    if (file)
    {
        fclose(file);
    }

    return true;
}

void EQMACHUD_GuiRecalculateCoordinates()
{
    // important to update this in order for maximize/restore to work properly
    if (g_mapIsMaximized == false)
    {
        g_mapX = g_mapDefaultX;
        g_mapY = g_mapDefaultY;

        g_mapWidth  = g_mapDefaultWidth;
        g_mapHeight = g_mapDefaultHeight;
    }
    else
    {
        g_mapX = g_mapMaximizedX;
        g_mapY = g_mapMaximizedY;

        g_mapWidth  = g_mapMaximizedWidth;
        g_mapHeight = g_mapMaximizedHeight;
    }

    g_mapMinX = g_mapX;
    g_mapMaxX = g_mapX + g_mapWidth;

    g_mapMinY = g_mapY;
    g_mapMaxY = g_mapY + g_mapHeight;

    g_mapOriginX = g_mapX + (g_mapWidth  * 0.5f);
    g_mapOriginY = g_mapY + (g_mapHeight * 0.5f);

    float buttonIndex;

    // exit row buttons

    buttonIndex = 1.0f;

    g_buttonReloadConfigX = g_buttonExitX + g_buttonWidth + g_elementOffset;
    g_buttonReloadConfigY = g_buttonExitY;

    buttonIndex += 1.0f;

    g_buttonToggleMapX = g_buttonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_buttonToggleMapY = g_buttonExitY;

    buttonIndex += 1.0f;

    g_buttonToggleEspX = g_buttonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_buttonToggleEspY = g_buttonExitY;

    buttonIndex += 1.0f;

    g_buttonToggleEspTextX = g_buttonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_buttonToggleEspTextY = g_buttonExitY;

    buttonIndex += 1.0f;

    g_buttonToggleEspGroundSpawnX = g_buttonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_buttonToggleEspGroundSpawnY = g_buttonExitY;

    buttonIndex += 1.0f;

    g_buttonToggleEspDoorSpawnX = g_buttonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_buttonToggleEspDoorSpawnY = g_buttonExitY;

    buttonIndex += 1.0f;

    g_buttonToggleEspDistanceX = g_buttonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_buttonToggleEspDistanceY = g_buttonExitY;

    buttonIndex += 1.0f;

    g_buttonToggleEspFilterNpcX = g_buttonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_buttonToggleEspFilterNpcY = g_buttonExitY;

    buttonIndex += 1.0f;

    g_buttonToggleHealthBarsX = g_buttonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_buttonToggleHealthBarsY = g_buttonExitY;

    buttonIndex += 1.0f;

    g_buttonToggleBuffsX = g_buttonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_buttonToggleBuffsY = g_buttonExitY;

    buttonIndex += 1.0f;

    g_buttonTogglePlayerInfoX = g_buttonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_buttonTogglePlayerInfoY = g_buttonExitY;

    buttonIndex += 1.0f;

    g_buttonToggleTargetInfoX = g_buttonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_buttonToggleTargetInfoY = g_buttonExitY;

    buttonIndex += 1.0f;

    // map right column buttons

    g_mapButtonZoomInX = g_mapMaxX + g_elementOffset;
    g_mapButtonZoomInY = g_mapY;

    buttonIndex = 1.0f;

    g_mapButtonZoomOutX = g_mapMaxX + g_elementOffset;
    g_mapButtonZoomOutY = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

    buttonIndex += 1.0f;

    g_mapButtonZoom1X = g_mapMaxX + g_elementOffset;
    g_mapButtonZoom1Y = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

    buttonIndex += 1.0f;

    g_mapButtonToggleLineColorX = g_mapMaxX + g_elementOffset;
    g_mapButtonToggleLineColorY = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

    buttonIndex += 1.0f;

    g_mapButtonToggleArrowX = g_mapMaxX + g_elementOffset;
    g_mapButtonToggleArrowY = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

    buttonIndex += 1.0f;

    g_mapButtonToggleHeightFilterX = g_mapMaxX + g_elementOffset;
    g_mapButtonToggleHeightFilterY = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

    buttonIndex += 1.0f;

    g_mapButtonToggleRotateX = g_mapMaxX + g_elementOffset;
    g_mapButtonToggleRotateY = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

    buttonIndex += 1.0f;

    g_mapButtonToggleZoneInfoX = g_mapMaxX + g_elementOffset;
    g_mapButtonToggleZoneInfoY = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

    buttonIndex += 1.0f;

    g_mapButtonToggleSpawnsX = g_mapMaxX + g_elementOffset;
    g_mapButtonToggleSpawnsY = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

    buttonIndex += 1.0f;

    g_mapButtonTogglePointsX = g_mapMaxX + g_elementOffset;
    g_mapButtonTogglePointsY = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

    buttonIndex += 1.0f;

    g_mapButtonToggleLinesX = g_mapMaxX + g_elementOffset;
    g_mapButtonToggleLinesY = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

    buttonIndex += 1.0f;

    g_mapButtonToggleSpawnDistanceX = g_mapMaxX + g_elementOffset;
    g_mapButtonToggleSpawnDistanceY = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

    buttonIndex += 1.0f;

    g_mapButtonToggleSpawnFilterNpcX = g_mapMaxX + g_elementOffset;
    g_mapButtonToggleSpawnFilterNpcY = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

    buttonIndex += 1.0f;

    // map top row buttons

    g_mapButtonMaximizeX = g_mapMaxX - g_buttonWidth;
    g_mapButtonMaximizeY = g_mapY - g_buttonHeight - g_elementOffset;

    buttonIndex = 1.0f;

    g_mapButtonToggleLayer3X = g_mapButtonMaximizeX - ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_mapButtonToggleLayer3Y = g_mapY - g_buttonHeight - g_elementOffset;

    buttonIndex += 1.0f;

    g_mapButtonToggleLayer2X = g_mapButtonMaximizeX - ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_mapButtonToggleLayer2Y = g_mapY - g_buttonHeight - g_elementOffset;

    buttonIndex += 1.0f;

    g_mapButtonToggleLayer1X = g_mapButtonMaximizeX - ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_mapButtonToggleLayer1Y = g_mapY - g_buttonHeight - g_elementOffset;

    buttonIndex += 1.0f;

    g_mapButtonToggleLayer0X = g_mapButtonMaximizeX - ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_mapButtonToggleLayer0Y = g_mapY - g_buttonHeight - g_elementOffset;

    buttonIndex += 1.0f;

    g_mapButtonCopyTargetMapLocationToClipboardX = g_mapButtonMaximizeX - ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_mapButtonCopyTargetMapLocationToClipboardY = g_mapY - g_buttonHeight - g_elementOffset;

    buttonIndex += 1.0f;

    g_mapButtonReloadMapX = g_mapButtonMaximizeX - ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_mapButtonReloadMapY = g_mapY - g_buttonHeight - g_elementOffset;

    buttonIndex += 1.0f;
}

void EQMACHUD_MapZoomOut()
{
    g_mapZoom -= g_mapZoom * g_mapZoomMultiplier;

    if (g_mapZoom <= g_mapZoomMaximum)
    {
        g_mapZoom = g_mapZoomMaximum;
    }
}

void EQMACHUD_MapZoomIn()
{
    g_mapZoom += g_mapZoom * g_mapZoomMultiplier;

    if (g_mapZoom >= g_mapZoomMinimum)
    {
        g_mapZoom = g_mapZoomMinimum;
    }
}

void EQMACHUD_MapMouseWheelZoomOut()
{
    g_mapZoom -= g_mapZoom * g_mapMouseWheelZoomMultiplier;

    if (g_mapZoom <= g_mapZoomMaximum)
    {
        g_mapZoom = g_mapZoomMaximum;
    }
}

void EQMACHUD_MapMouseWheelZoomIn()
{
    g_mapZoom += g_mapZoom * g_mapMouseWheelZoomMultiplier;

    if (g_mapZoom >= g_mapZoomMinimum)
    {
        g_mapZoom = g_mapZoomMinimum;
    }
}

void EQMACHUD_MapSetZoom(float zoom)
{
    g_mapZoom = zoom;
}

void EQMACHUD_MapToggleMaximized()
{
    EQ_ToggleBool(g_mapIsMaximized);

    EQMACHUD_GuiRecalculateCoordinates();
}

void EQMACHUD_MapToggleLayer0()
{
    EQ_ToggleBool(g_mapLayer0IsEnabled);
}

void EQMACHUD_MapToggleLayer1()
{
    EQ_ToggleBool(g_mapLayer1IsEnabled);
}

void EQMACHUD_MapToggleLayer2()
{
    EQ_ToggleBool(g_mapLayer2IsEnabled);
}

void EQMACHUD_MapToggleLayer3()
{
    EQ_ToggleBool(g_mapLayer3IsEnabled);
}

void EQMACHUD_MapToggleArrow()
{
    EQ_ToggleBool(g_mapArrowIsEnabled);
}

void EQMACHUD_MapToggleHeightFilter()
{
    EQ_ToggleBool(g_mapHeightFilterIsEnabled);
}

void EQMACHUD_MapToggleRotate()
{
    EQ_ToggleBool(g_mapRotateIsEnabled);
}

void EQMACHUD_MapToggleZoneInfo()
{
    EQ_ToggleBool(g_mapZoneInfoIsEnabled);
}

void EQMACHUD_MapToggleSpawns()
{
    EQ_ToggleBool(g_mapSpawnsIsEnabled);
}

void EQMACHUD_MapTogglePoints()
{
    EQ_ToggleBool(g_mapPointsIsEnabled);
}

void EQMACHUD_MapToggleLines()
{
    EQ_ToggleBool(g_mapLinesIsEnabled);
}

void EQMACHUD_MapToggleSpawnDistance()
{
    EQ_ToggleBool(g_mapSpawnDistanceIsEnabled);
}

void EQMACHUD_MapToggleSpawnFilterNpc()
{
    EQ_ToggleBool(g_mapSpawnFilterNpcIsEnabled);
}

void EQMACHUD_MapToggleLineColor()
{
    if (g_mapLineColor == 0xDEADBEEF)
    {
        g_mapLineColor = 0xFFFFFFFF;
    }
    else if (g_mapLineColor == 0xFFFFFFFF)
    {
        g_mapLineColor = 0xFF000000;
    }
    else if (g_mapLineColor == 0xFF000000)
    {
        g_mapLineColor = 0xFF00FF00;
    }
    else if (g_mapLineColor == 0xFF00FF00)
    {
        g_mapLineColor = 0xFFFFFF00;
    }
    else if (g_mapLineColor == 0xFFFFFF00)
    {
        g_mapLineColor = 0xFF00FFFF;
    }
    else if (g_mapLineColor == 0xFF00FFFF)
    {
        g_mapLineColor = 0xFFFF00FF;
    }
    else if (g_mapLineColor == 0xFFFF00FF)
    {
        g_mapLineColor = 0xFFFF0000;
    }
    else if (g_mapLineColor == 0xFFFF0000)
    {
        g_mapLineColor = 0xDEADBEEF;
    }
    else
    {
        g_mapLineColor = 0xDEADBEEF;
    }
}

void EQMACHUD_ToggleMap()
{
    EQ_ToggleBool(g_mapIsEnabled);
}

void EQMACHUD_ToggleEsp()
{
    EQ_ToggleBool(g_espIsEnabled);
}

void EQMACHUD_ToggleEspShowText()
{
    EQ_ToggleBool(g_espShowTextIsEnabled);
}

void EQMACHUD_ToggleEspShowGroundSpawn()
{
    EQ_ToggleBool(g_espShowGroundSpawnIsEnabled);
}

void EQMACHUD_ToggleEspShowDoorSpawn()
{
    EQ_ToggleBool(g_espShowDoorSpawnIsEnabled);
}

void EQMACHUD_ToggleEspDistance()
{
    EQ_ToggleBool(g_espDistanceIsEnabled);
}

void EQMACHUD_ToggleEspFilterNpc()
{
    EQ_ToggleBool(g_espFilterNpcIsEnabled);
}

void EQMACHUD_ToggleHealthBars()
{
    EQ_ToggleBool(g_healthBarsIsEnabled);
}

void EQMACHUD_ToggleBuffs()
{
    EQ_ToggleBool(g_buffsIsEnabled);
}

void EQMACHUD_TogglePlayerInfo()
{
    EQ_ToggleBool(g_playerInfoIsEnabled);
}

void EQMACHUD_ToggleTargetInfo()
{
    EQ_ToggleBool(g_targetInfoIsEnabled);
}

void EQMACHUD_DoLoadMap()
{
    DWORD zoneId = EQ_READ_MEMORY<DWORD>(EQ_ZONE_ID);

    EQZONEINFO* zoneInfo = (EQZONEINFO*)EQ_STRUCTURE_ZONE_INFO;

    if (zoneId != 0)
    {
        EQMACHUD_MapLineList_Destroy();
        EQMACHUD_MapPointList_Destroy();

        char mapFilename[128];
        sprintf_s(mapFilename, ".\\maps\\%s.txt", zoneInfo->ShortName);

        EQMACHUD_LoadMap(mapFilename, 0); // layer 0 is the base layer

        size_t mapLayer;
        for (mapLayer = 1; mapLayer < (g_mapNumLayers + 1); mapLayer++)
        {
            char mapFilenameEx[128];
            sprintf_s(mapFilenameEx, ".\\maps\\%s_%d.txt", zoneInfo->ShortName, mapLayer);

            EQMACHUD_LoadMap(mapFilenameEx, mapLayer);
        }
    }
}

void EQMACHUD_DoLoadConfig()
{
    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    DWORD zoneId = EQ_READ_MEMORY<DWORD>(EQ_ZONE_ID);

    EQZONEINFO* zoneInfo = (EQZONEINFO*)EQ_STRUCTURE_ZONE_INFO;

    // default config file
    char configFilename[128];
    sprintf_s(configFilename, ".\\%s.ini", g_applicationExeName);

    EQMACHUD_LoadConfig(configFilename);

    // character specific config file
    if (charInfo)
    {
        char charConfigFilename[128];
        sprintf_s(charConfigFilename, ".\\%s-%s.ini", g_applicationExeName, charInfo->Name);

        EQMACHUD_LoadConfig(charConfigFilename);
    }

    // zone specific config file
    if (zoneId != 0)
    {
        char zoneConfigFilename[128];
        sprintf_s(zoneConfigFilename, ".\\maps\\%s.ini", zoneInfo->ShortName);

        EQMACHUD_LoadConfig(zoneConfigFilename);
    }
}

void EQMACHUD_DoUpdateZone()
{
    DWORD zoneId = EQ_READ_MEMORY<DWORD>(EQ_ZONE_ID);

    EQZONEINFO* zoneInfo = (EQZONEINFO*)EQ_STRUCTURE_ZONE_INFO;

    if (g_zoneId != zoneId && zoneId != 0)
    {
        g_zoneId = zoneId;

        EQMACHUD_DoLoadMap();
        EQMACHUD_DoLoadConfig();

        EQMACHUD_GuiRecalculateCoordinates();
    }
}

bool EQMACHUD_DoButtonMouseLeftHeldDown()
{
    // prevent hud buttons from working if a game gui window or element overlaps
    DWORD mouseOverGuiElement = EQ_READ_MEMORY<DWORD>(EQ_POINT_CXWND_MOUSE_OVER_GUI_ELEMENT);
    if (mouseOverGuiElement != 0x00000000)
    {
        return true;
    }

    WORD mouseX = EQ_READ_MEMORY<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_READ_MEMORY<WORD>(EQ_MOUSE_Y);

    DWORD mouseClickState = EQ_READ_MEMORY<DWORD>(EQ_MOUSE_CLICK_STATE);

    if (mouseClickState != EQ_MOUSE_CLICK_STATE_LEFT)
    {
        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonZoomInX, (int)g_mapButtonZoomInY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapZoomIn();
        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonZoomOutX, (int)g_mapButtonZoomOutY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapZoomOut();
        return true;
    }

    return false;
}

bool EQMACHUD_DoButtonMouseLeftDown()
{
    // prevent hud buttons from working if a game gui window or element overlaps
    DWORD mouseOverGuiElement = EQ_READ_MEMORY<DWORD>(EQ_POINT_CXWND_MOUSE_OVER_GUI_ELEMENT);
    if (mouseOverGuiElement != 0x00000000)
    {
        return true;
    }

    WORD mouseX = EQ_READ_MEMORY<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_READ_MEMORY<WORD>(EQ_MOUSE_Y);

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonZoomInX, (int)g_mapButtonZoomInY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonZoomOutX, (int)g_mapButtonZoomOutY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        return true;
    }

    return false;
}

bool EQMACHUD_DoButtonMouseLeftUp()
{
    // prevent hud buttons from working if a game gui window or element overlaps
    DWORD mouseOverGuiElement = EQ_READ_MEMORY<DWORD>(EQ_POINT_CXWND_MOUSE_OVER_GUI_ELEMENT);
    if (mouseOverGuiElement != 0x00000000)
    {
        return true;
    }

    WORD mouseX = EQ_READ_MEMORY<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_READ_MEMORY<WORD>(EQ_MOUSE_Y);

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonExitX, (int)g_buttonExitY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        if (g_writeTextToChatWindowIsEnabled == true)
        {
            BYTE isInGame = EQ_READ_MEMORY<BYTE>(EQ_IS_IN_GAME);

            if (isInGame == 1)
            {
                char unloadedText[128];
                sprintf_s(unloadedText, "%s exiting...", g_applicationName);

                EQ_CLASS_CEverQuest->dsp_chat(unloadedText);
            }
        }

        g_bExit = 1;
        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonReloadConfigX, (int)g_buttonReloadConfigY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_DoLoadConfig();

        EQMACHUD_GuiRecalculateCoordinates();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            EQ_CLASS_CEverQuest->dsp_chat("-> Config reloaded.");
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleMapX, (int)g_buttonToggleMapY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_ToggleMap();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_mapIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleEspX, (int)g_buttonToggleEspY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_ToggleEsp();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_espIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> ESP enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> ESP disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleEspTextX, (int)g_buttonToggleEspTextY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_ToggleEspShowText();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_espShowTextIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> ESP Text enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> ESP Text disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleEspGroundSpawnX, (int)g_buttonToggleEspGroundSpawnY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_ToggleEspShowGroundSpawn();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_espShowGroundSpawnIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> ESP Ground Spawns enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> ESP Ground Spawns disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleEspDoorSpawnX, (int)g_buttonToggleEspDoorSpawnY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_ToggleEspShowDoorSpawn();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_espShowDoorSpawnIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> ESP Door Spawns enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> ESP Door Spawns disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleEspDistanceX, (int)g_buttonToggleEspDistanceY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_ToggleEspDistance();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_espDistanceIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> ESP Distance enabled.");

                char distanceText[128];
                sprintf_s(distanceText, "Distance: %.1f", g_espDistance);

                EQ_CLASS_CEverQuest->dsp_chat(distanceText);
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> ESP Distance disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleEspFilterNpcX, (int)g_buttonToggleEspFilterNpcY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_ToggleEspFilterNpc();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_espFilterNpcIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> ESP Filter NPC enabled.");

                if (strlen(g_espFilterNpc) < 128)
                {
                    EQ_CLASS_CEverQuest->dsp_chat(g_espFilterNpc);
                }
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> ESP Filter NPC disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleHealthBarsX, (int)g_buttonToggleHealthBarsY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_ToggleHealthBars();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_healthBarsIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Health Bars enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Health Bars disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleBuffsX, (int)g_buttonToggleBuffsY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_ToggleBuffs();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_buffsIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Buffs enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Buffs disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonTogglePlayerInfoX, (int)g_buttonTogglePlayerInfoY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_TogglePlayerInfo();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_playerInfoIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Player Info enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Player Info disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleTargetInfoX, (int)g_buttonToggleTargetInfoY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_ToggleTargetInfo();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_targetInfoIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Target Info enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Target Info disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonZoomInX, (int)g_mapButtonZoomInY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapZoomIn();
        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonZoomOutX, (int)g_mapButtonZoomOutY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapZoomOut();
        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonZoom1X, (int)g_mapButtonZoom1Y,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {

        EQMACHUD_MapSetZoom(1.0f);
        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleArrowX, (int)g_mapButtonToggleArrowY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapToggleArrow();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_mapArrowIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Arrow enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Arrow disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleHeightFilterX, (int)g_mapButtonToggleHeightFilterY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapToggleHeightFilter();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_mapHeightFilterIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Height Filter enabled.");

                char heightFilterText[128];

                sprintf_s(heightFilterText, "Low: %.1f", g_mapHeightFilterLow);

                EQ_CLASS_CEverQuest->dsp_chat(heightFilterText);

                sprintf_s(heightFilterText, "High: %.1f", g_mapHeightFilterHigh);

                EQ_CLASS_CEverQuest->dsp_chat(heightFilterText);
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Height Filter disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleRotateX, (int)g_mapButtonToggleRotateY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapToggleRotate();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_mapRotateIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Rotate enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Rotate disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleLineColorX, (int)g_mapButtonToggleLineColorY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapToggleLineColor();
        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleZoneInfoX, (int)g_mapButtonToggleZoneInfoY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapToggleZoneInfo();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_mapZoneInfoIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Zone Info enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Zone Info disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleSpawnsX, (int)g_mapButtonToggleSpawnsY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapToggleSpawns();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_mapSpawnsIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Spawns enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Spawns disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonTogglePointsX, (int)g_mapButtonTogglePointsY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapTogglePoints();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_mapPointsIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Points enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Points disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleLinesX, (int)g_mapButtonToggleLinesY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapToggleLines();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_mapLinesIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Lines enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Lines disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleSpawnDistanceX, (int)g_mapButtonToggleSpawnDistanceY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapToggleSpawnDistance();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_mapSpawnDistanceIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Spawn Distance enabled.");

                char distanceText[128];
                sprintf_s(distanceText, "Distance: %.1f", g_mapSpawnDistance);

                EQ_CLASS_CEverQuest->dsp_chat(distanceText);
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Spawn Distance disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleSpawnFilterNpcX, (int)g_mapButtonToggleSpawnFilterNpcY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapToggleSpawnFilterNpc();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_mapSpawnFilterNpcIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Spawn Filter NPC enabled.");

                if (strlen(g_mapSpawnFilterNpc) < 128)
                {
                    EQ_CLASS_CEverQuest->dsp_chat(g_mapSpawnFilterNpc);
                }
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Spawn Filter NPC disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonReloadMapX, (int)g_mapButtonReloadMapY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_DoLoadMap();

        EQMACHUD_GuiRecalculateCoordinates();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            EQ_CLASS_CEverQuest->dsp_chat("-> Map reloaded.");
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonCopyTargetMapLocationToClipboardX, (int)g_mapButtonCopyTargetMapLocationToClipboardY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_CopyTargetMapLocationToClipboard();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            EQ_CLASS_CEverQuest->dsp_chat("-> Target's map location copied to clipboard.");
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleLayer0X, (int)g_mapButtonToggleLayer0Y,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapToggleLayer0();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_mapLayer0IsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Base Layer enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Base Layer disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleLayer1X, (int)g_mapButtonToggleLayer1Y,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapToggleLayer1();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_mapLayer1IsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Layer 1 enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Layer 1 disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleLayer2X, (int)g_mapButtonToggleLayer2Y,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapToggleLayer2();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_mapLayer2IsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Layer 2 enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Layer 2 disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleLayer3X, (int)g_mapButtonToggleLayer3Y,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapToggleLayer3();

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (g_mapLayer3IsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Layer 3 enabled.");
            }
            else
            {
                EQ_CLASS_CEverQuest->dsp_chat("-> Map Layer 3 disabled.");
            }
        }

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonMaximizeX, (int)g_mapButtonMaximizeY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQMACHUD_MapToggleMaximized();
        return true;
    }

    return false;
}

bool EQMACHUD_DoButtonToolTipText()
{
    // prevent hud buttons from working if a game gui window or element overlaps
    DWORD mouseOverGuiElement = EQ_READ_MEMORY<DWORD>(EQ_POINT_CXWND_MOUSE_OVER_GUI_ELEMENT);
    if (mouseOverGuiElement != 0x00000000)
    {
        return true;
    }

    WORD mouseX = EQ_READ_MEMORY<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_READ_MEMORY<WORD>(EQ_MOUSE_Y);

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonExitX, (int)g_buttonExitY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        char buttonText[128];
        sprintf_s(buttonText, "Exit %s", g_applicationName);

        EQ_DrawTooltipText
        (
            buttonText,
            (int)(g_buttonExitX),
            (int)(g_buttonExitY + g_buttonHeight + g_elementOffset),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonReloadConfigX, (int)g_buttonReloadConfigY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Reload Config",
            (int)(g_buttonReloadConfigX),
            (int)(g_buttonReloadConfigY + g_buttonHeight + g_elementOffset),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleMapX, (int)g_buttonToggleMapY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Map",
            (int)(g_buttonToggleMapX),
            (int)(g_buttonToggleMapY + g_buttonHeight + g_elementOffset),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleEspX, (int)g_buttonToggleEspY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "ESP",
            (int)(g_buttonToggleEspX),
            (int)(g_buttonToggleEspY + g_buttonHeight + g_elementOffset),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleEspTextX, (int)g_buttonToggleEspTextY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "ESP Text",
            (int)(g_buttonToggleEspTextX),
            (int)(g_buttonToggleEspTextY + g_buttonHeight + g_elementOffset),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleEspGroundSpawnX, (int)g_buttonToggleEspGroundSpawnY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "ESP Ground Spawns",
            (int)(g_buttonToggleEspGroundSpawnX),
            (int)(g_buttonToggleEspGroundSpawnY + g_buttonHeight + g_elementOffset),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleEspDoorSpawnX, (int)g_buttonToggleEspDoorSpawnY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "ESP Door Spawns",
            (int)(g_buttonToggleEspDoorSpawnX),
            (int)(g_buttonToggleEspDoorSpawnY + g_buttonHeight + g_elementOffset),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleEspDistanceX, (int)g_buttonToggleEspDistanceY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "ESP Distance",
            (int)(g_buttonToggleEspDistanceX),
            (int)(g_buttonToggleEspDistanceY + g_buttonHeight + g_elementOffset),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleEspFilterNpcX, (int)g_buttonToggleEspFilterNpcY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "ESP Filter NPC",
            (int)(g_buttonToggleEspFilterNpcX),
            (int)(g_buttonToggleEspFilterNpcY + g_buttonHeight + g_elementOffset),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleHealthBarsX, (int)g_buttonToggleHealthBarsY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Health Bars",
            (int)(g_buttonToggleHealthBarsX),
            (int)(g_buttonToggleHealthBarsY + g_buttonHeight + g_elementOffset),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleBuffsX, (int)g_buttonToggleBuffsY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Buffs",
            (int)(g_buttonToggleBuffsX),
            (int)(g_buttonToggleBuffsY + g_buttonHeight + g_elementOffset),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonTogglePlayerInfoX, (int)g_buttonTogglePlayerInfoY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Player Info",
            (int)(g_buttonTogglePlayerInfoX),
            (int)(g_buttonTogglePlayerInfoY + g_buttonHeight + g_elementOffset),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleTargetInfoX, (int)g_buttonToggleTargetInfoY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Target Info",
            (int)(g_buttonToggleTargetInfoX),
            (int)(g_buttonToggleTargetInfoY + g_buttonHeight + g_elementOffset),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonZoomInX, (int)g_mapButtonZoomInY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Zoom In",
            (int)(g_mapButtonZoomInX + g_buttonWidth + g_elementOffset),
            (int)(g_mapButtonZoomInY - 2.0f),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonZoomOutX, (int)g_mapButtonZoomOutY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Zoom Out",
            (int)(g_mapButtonZoomOutX + g_buttonWidth + g_elementOffset),
            (int)(g_mapButtonZoomOutY - 2.0f),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonZoom1X, (int)g_mapButtonZoom1Y,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Zoom 1x",
            (int)(g_mapButtonZoom1X + g_buttonWidth + g_elementOffset),
            (int)(g_mapButtonZoom1Y - 2.0f),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleArrowX, (int)g_mapButtonToggleArrowY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Arrow",
            (int)(g_mapButtonToggleArrowX + g_buttonWidth + g_elementOffset),
            (int)(g_mapButtonToggleArrowY - 2.0f),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleHeightFilterX, (int)g_mapButtonToggleHeightFilterY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Height Filter",
            (int)(g_mapButtonToggleHeightFilterX + g_buttonWidth + g_elementOffset),
            (int)(g_mapButtonToggleHeightFilterY - 2.0f),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleRotateX, (int)g_mapButtonToggleRotateY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Rotate",
            (int)(g_mapButtonToggleRotateX + g_buttonWidth + g_elementOffset),
            (int)(g_mapButtonToggleRotateY - 2.0f),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleLineColorX, (int)g_mapButtonToggleLineColorY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Line Color",
            (int)(g_mapButtonToggleLineColorX + g_buttonWidth + g_elementOffset),
            (int)(g_mapButtonToggleLineColorY - 2.0f),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleZoneInfoX, (int)g_mapButtonToggleZoneInfoY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Zone Info",
            (int)(g_mapButtonToggleZoneInfoX + g_buttonWidth + g_elementOffset),
            (int)(g_mapButtonToggleZoneInfoY - 2.0f),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleSpawnsX, (int)g_mapButtonToggleSpawnsY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Spawns",
            (int)(g_mapButtonToggleSpawnsX + g_buttonWidth + g_elementOffset),
            (int)(g_mapButtonToggleSpawnsY - 2.0f),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonTogglePointsX, (int)g_mapButtonTogglePointsY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Points (Labels)",
            (int)(g_mapButtonTogglePointsX + g_buttonWidth + g_elementOffset),
            (int)(g_mapButtonTogglePointsY - 2.0f),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleLinesX, (int)g_mapButtonToggleLinesY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Lines",
            (int)(g_mapButtonToggleLinesX + g_buttonWidth + g_elementOffset),
            (int)(g_mapButtonToggleLinesY - 2.0f),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleSpawnDistanceX, (int)g_mapButtonToggleSpawnDistanceY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Spawn Distance",
            (int)(g_mapButtonToggleSpawnDistanceX + g_buttonWidth + g_elementOffset),
            (int)(g_mapButtonToggleSpawnDistanceY - 2.0f),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleSpawnFilterNpcX, (int)g_mapButtonToggleSpawnFilterNpcY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Spawn Filter NPC",
            (int)(g_mapButtonToggleSpawnFilterNpcX + g_buttonWidth + g_elementOffset),
            (int)(g_mapButtonToggleSpawnFilterNpcY - 2.0f),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonReloadMapX, (int)g_mapButtonReloadMapY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Reload Map",
            (int)(g_mapButtonReloadMapX),
            (int)(g_mapButtonReloadMapY - g_elementOffset) - (g_fontHeight - 2),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonCopyTargetMapLocationToClipboardX, (int)g_mapButtonCopyTargetMapLocationToClipboardY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Copy Target's Map Location to Clipboard",
            (int)(g_mapButtonCopyTargetMapLocationToClipboardX),
            (int)(g_mapButtonCopyTargetMapLocationToClipboardY - g_elementOffset) - (g_fontHeight - 2),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleLayer0X, (int)g_mapButtonToggleLayer0Y,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Base Layer",
            (int)(g_mapButtonToggleLayer0X),
            (int)(g_mapButtonToggleLayer0Y - g_elementOffset) - (g_fontHeight - 2),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleLayer1X, (int)g_mapButtonToggleLayer1Y,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Layer 1",
            (int)(g_mapButtonToggleLayer1X),
            (int)(g_mapButtonToggleLayer1Y - g_elementOffset) - (g_fontHeight - 2),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleLayer2X, (int)g_mapButtonToggleLayer2Y,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Layer 2",
            (int)(g_mapButtonToggleLayer2X),
            (int)(g_mapButtonToggleLayer2Y - g_elementOffset) - (g_fontHeight - 2),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleLayer3X, (int)g_mapButtonToggleLayer3Y,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_DrawTooltipText
        (
            "Layer 3",
            (int)(g_mapButtonToggleLayer3X),
            (int)(g_mapButtonToggleLayer3Y - g_elementOffset) - (g_fontHeight - 2),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonMaximizeX, (int)g_mapButtonMaximizeY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        char buttonText[128];
        if (g_mapIsMaximized == false)
        {
            strcpy_s(buttonText, "Maximize");
        }
        else
        {
            strcpy_s(buttonText, "Restore");
        }

        EQ_DrawTooltipText
        (
            buttonText,
            (int)(g_mapButtonMaximizeX),
            (int)(g_mapButtonMaximizeY - g_elementOffset) - (g_fontHeight - 2),
            EQ_TEXT_COLOR_WHITE,
            EQ_POINTER_FONT_ARIAL14
        );

        return true;
    }

    return false;
}

void EQMACHUD_DoExitRowButtons()
{
    DWORD fontArial14 = EQ_READ_MEMORY<DWORD>(EQ_POINTER_FONT_ARIAL14);

    int buttonColor = g_buttonColorMinMaxClose;
    int buttonTextColor = g_buttonTextColorMinMaxClose;

    EQ_DrawRectangle(g_buttonExitX, g_buttonExitY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("x", (int)(g_buttonExitX + 3.0f), (int)(g_buttonExitY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    EQ_DrawRectangle(g_buttonReloadConfigX, g_buttonReloadConfigY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("r", (int)(g_buttonReloadConfigX + 4.0f), (int)(g_buttonReloadConfigY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleMapX, g_buttonToggleMapY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("m", (int)(g_buttonToggleMapX + 2.0f), (int)(g_buttonToggleMapY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleEspX, g_buttonToggleEspY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("e", (int)(g_buttonToggleEspX + 3.0f), (int)(g_buttonToggleEspY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espShowTextIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleEspTextX, g_buttonToggleEspTextY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("s", (int)(g_buttonToggleEspTextX + 3.0f), (int)(g_buttonToggleEspTextY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espDistanceIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleEspDistanceX, g_buttonToggleEspDistanceY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("d", (int)(g_buttonToggleEspDistanceX + 3.0f), (int)(g_buttonToggleEspDistanceY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espFilterNpcIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleEspFilterNpcX, g_buttonToggleEspFilterNpcY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("f", (int)(g_buttonToggleEspFilterNpcX + 4.0f), (int)(g_buttonToggleEspFilterNpcY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espShowGroundSpawnIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleEspGroundSpawnX, g_buttonToggleEspGroundSpawnY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("g", (int)(g_buttonToggleEspGroundSpawnX + 3.0f), (int)(g_buttonToggleEspGroundSpawnY - 3.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espShowDoorSpawnIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleEspDoorSpawnX, g_buttonToggleEspDoorSpawnY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("o", (int)(g_buttonToggleEspDoorSpawnX + 3.0f), (int)(g_buttonToggleEspDoorSpawnY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_healthBarsIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleHealthBarsX, g_buttonToggleHealthBarsY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("h", (int)(g_buttonToggleHealthBarsX + 3.0f), (int)(g_buttonToggleHealthBarsY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_buffsIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleBuffsX, g_buttonToggleBuffsY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("b", (int)(g_buttonToggleBuffsX + 3.0f), (int)(g_buttonToggleBuffsY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_playerInfoIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonTogglePlayerInfoX, g_buttonTogglePlayerInfoY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("p", (int)(g_buttonTogglePlayerInfoX + 3.0f), (int)(g_buttonTogglePlayerInfoY - 3.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_targetInfoIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleTargetInfoX, g_buttonToggleTargetInfoY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("t", (int)(g_buttonToggleTargetInfoX + 5.0f), (int)(g_buttonToggleTargetInfoY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;
}

void EQMACHUD_DoMap()
{
    DWORD fontArial14 = EQ_READ_MEMORY<DWORD>(EQ_POINTER_FONT_ARIAL14);

    WORD mouseX = EQ_READ_MEMORY<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_READ_MEMORY<WORD>(EQ_MOUSE_Y);

    DWORD mouseClickState = EQ_READ_MEMORY<DWORD>(EQ_MOUSE_CLICK_STATE);

    DWORD mouseOverGuiElement = EQ_READ_MEMORY<DWORD>(EQ_POINT_CXWND_MOUSE_OVER_GUI_ELEMENT);

    DWORD zoneId = EQ_READ_MEMORY<DWORD>(EQ_ZONE_ID);

    EQZONEINFO* zoneInfo = (EQZONEINFO*)EQ_STRUCTURE_ZONE_INFO;

    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    PEQSPAWNINFO spawn = (PEQSPAWNINFO)EQ_OBJECT_FirstSpawn;

    PEQGROUNDSPAWNINFO groundSpawn = (PEQGROUNDSPAWNINFO)EQ_OBJECT_FirstGroundSpawn;

    PEQDOORSPAWNINFO doorSpawn = (PEQDOORSPAWNINFO)EQ_OBJECT_FirstDoorSpawn;

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    EQGROUPLIST* groupList = (EQGROUPLIST*)EQ_STRUCTURE_GROUP_LIST;

    EQLOCATION playerLocation = {EQ_OBJECT_PlayerSpawn->Y, EQ_OBJECT_PlayerSpawn->X, EQ_OBJECT_PlayerSpawn->Z};

    EQLOCATION targetLocation = {EQ_OBJECT_TargetSpawn->Y, EQ_OBJECT_TargetSpawn->X, EQ_OBJECT_TargetSpawn->Z};

    EQ_CLASS_CDisplay->WriteTextHD2
    (
        "Map",
        (int)g_mapX,
        (int)(g_mapY - g_elementOffset) - (g_fontHeight - 4),
        g_mapDefaultTextColor,
        fontArial14
    );

    EQ_DrawRectangle(g_mapX, g_mapY, g_mapWidth, g_mapHeight, g_mapBorderColor);

    g_numDeferred2dItems += 4;

    int buttonColor = g_buttonColorEnabled;
    int buttonTextColor = g_buttonTextColorEnabled;

    EQ_DrawRectangle(g_mapButtonZoomInX, g_mapButtonZoomInY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("+", (int)(g_mapButtonZoomInX + 3.0f), (int)(g_mapButtonZoomInY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    EQ_DrawRectangle(g_mapButtonZoomOutX, g_mapButtonZoomOutY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("-", (int)(g_mapButtonZoomOutX + 4.0f), (int)(g_mapButtonZoomOutY - 3.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    EQ_DrawRectangle(g_mapButtonZoom1X, g_mapButtonZoom1Y, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("1", (int)(g_mapButtonZoom1X + 3.0f), (int)(g_mapButtonZoom1Y - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    EQ_DrawRectangle(g_mapButtonToggleLineColorX, g_mapButtonToggleLineColorY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("c", (int)(g_mapButtonToggleLineColorX + 3.0f), (int)(g_mapButtonToggleLineColorY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapArrowIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_mapButtonToggleArrowX, g_mapButtonToggleArrowY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("a", (int)(g_mapButtonToggleArrowX + 3.0f), (int)(g_mapButtonToggleArrowY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapHeightFilterIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_mapButtonToggleHeightFilterX, g_mapButtonToggleHeightFilterY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("h", (int)(g_mapButtonToggleHeightFilterX + 3.0f), (int)(g_mapButtonToggleHeightFilterY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapRotateIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_mapButtonToggleRotateX, g_mapButtonToggleRotateY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("r", (int)(g_mapButtonToggleRotateX + 4.0f), (int)(g_mapButtonToggleRotateY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapZoneInfoIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_mapButtonToggleZoneInfoX, g_mapButtonToggleZoneInfoY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("z", (int)(g_mapButtonToggleZoneInfoX + 3.0f), (int)(g_mapButtonToggleZoneInfoY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapSpawnsIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_mapButtonToggleSpawnsX, g_mapButtonToggleSpawnsY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("s", (int)(g_mapButtonToggleSpawnsX + 3.0f), (int)(g_mapButtonToggleSpawnsY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapPointsIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_mapButtonTogglePointsX, g_mapButtonTogglePointsY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("p", (int)(g_mapButtonTogglePointsX + 3.0f), (int)(g_mapButtonTogglePointsY - 3.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapLinesIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_mapButtonToggleLinesX, g_mapButtonToggleLinesY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("l", (int)(g_mapButtonToggleLinesX + 5.0f), (int)(g_mapButtonToggleLinesY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapSpawnDistanceIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_mapButtonToggleSpawnDistanceX, g_mapButtonToggleSpawnDistanceY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("d", (int)(g_mapButtonToggleSpawnDistanceX + 3.0f), (int)(g_mapButtonToggleSpawnDistanceY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapSpawnFilterNpcIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_mapButtonToggleSpawnFilterNpcX, g_mapButtonToggleSpawnFilterNpcY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("f", (int)(g_mapButtonToggleSpawnFilterNpcX + 4.0f), (int)(g_mapButtonToggleSpawnFilterNpcY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    EQ_DrawRectangle(g_mapButtonReloadMapX, g_mapButtonReloadMapY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("r", (int)(g_mapButtonReloadMapX + 4.0f), (int)(g_mapButtonReloadMapY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    EQ_DrawRectangle(g_mapButtonCopyTargetMapLocationToClipboardX, g_mapButtonCopyTargetMapLocationToClipboardY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("c", (int)(g_mapButtonCopyTargetMapLocationToClipboardX + 3.0f), (int)(g_mapButtonCopyTargetMapLocationToClipboardY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapLayer0IsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_mapButtonToggleLayer0X, g_mapButtonToggleLayer0Y, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("b", (int)(g_mapButtonToggleLayer0X + 3.0f), (int)(g_mapButtonToggleLayer0Y - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapLayer1IsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_mapButtonToggleLayer1X, g_mapButtonToggleLayer1Y, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("1", (int)(g_mapButtonToggleLayer1X + 3.0f), (int)(g_mapButtonToggleLayer1Y - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapLayer2IsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_mapButtonToggleLayer2X, g_mapButtonToggleLayer2Y, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("2", (int)(g_mapButtonToggleLayer2X + 3.0f), (int)(g_mapButtonToggleLayer2Y - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapLayer3IsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_mapButtonToggleLayer3X, g_mapButtonToggleLayer3Y, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("3", (int)(g_mapButtonToggleLayer3X + 3.0f), (int)(g_mapButtonToggleLayer3Y - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorMinMaxClose;
    buttonTextColor = g_buttonTextColorMinMaxClose;

    char mapButtonMaximizeText[8];
    float mapButtonMaximizeTextOffsetX;
    float mapButtonMaximizeTextOffsetY;
    if (g_mapIsMaximized == false)
    {
        strcpy_s(mapButtonMaximizeText, "+");
        mapButtonMaximizeTextOffsetX = 3.0;
        mapButtonMaximizeTextOffsetY = 2.0;
    }
    else
    {
        strcpy_s(mapButtonMaximizeText, "-");
        mapButtonMaximizeTextOffsetX = 4.0;
        mapButtonMaximizeTextOffsetY = 2.0;
    }

    EQ_DrawRectangle(g_mapButtonMaximizeX, g_mapButtonMaximizeY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2(mapButtonMaximizeText, (int)(g_mapButtonMaximizeX + mapButtonMaximizeTextOffsetX), (int)(g_mapButtonMaximizeY - mapButtonMaximizeTextOffsetY), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    if (g_mapLinesIsEnabled == true)
    {
        struct _EQMAPLINE *mapLine = g_mapLineListBegin;

        while (mapLine)
        {
            if (g_mapLayer0IsEnabled == false && mapLine->Layer == 0)
            {
                mapLine = mapLine->Next;
                continue;
            }

            if (g_mapLayer1IsEnabled == false && mapLine->Layer == 1)
            {
                mapLine = mapLine->Next;
                continue;
            }

            if (g_mapLayer2IsEnabled == false && mapLine->Layer == 2)
            {
                mapLine = mapLine->Next;
                continue;
            }

            if (g_mapLayer3IsEnabled == false && mapLine->Layer == 3)
            {
                mapLine = mapLine->Next;
                continue;
            }

            if (g_mapHeightFilterIsEnabled == true)
            {
                if (g_mapHeightFilterLow > 0.0f)
                {
                    if
                    (
                        mapLine->Z1 < (playerSpawn->Z - g_mapHeightFilterLow) ||
                        mapLine->Z2 < (playerSpawn->Z - g_mapHeightFilterLow)
                    )
                    {
                        mapLine = mapLine->Next;
                        continue;
                    }
                }

                if (g_mapHeightFilterHigh > 0.0f)
                {
                    if
                    (
                        mapLine->Z1 > (playerSpawn->Z + g_mapHeightFilterHigh) ||
                        mapLine->Z2 > (playerSpawn->Z + g_mapHeightFilterHigh)
                    )
                    {
                        mapLine = mapLine->Next;
                        continue;
                    }
                }
            }

            EQLINE line;
            line.X1 = ((mapLine->X1 * g_mapZoom) + g_mapOriginX) + (playerSpawn->X * g_mapZoom);
            line.Y1 = ((mapLine->Y1 * g_mapZoom) + g_mapOriginY) + (playerSpawn->Y * g_mapZoom);
            line.Z1 = 1.0f;
            line.X2 = ((mapLine->X2 * g_mapZoom) + g_mapOriginX) + (playerSpawn->X * g_mapZoom);
            line.Y2 = ((mapLine->Y2 * g_mapZoom) + g_mapOriginY) + (playerSpawn->Y * g_mapZoom);
            line.Z2 = 1.0f;

            if (g_mapRotateIsEnabled == true)
            {
                EQMACHUD_Rotate2D(g_mapOriginX, g_mapOriginY, line.X1, line.Y1, playerSpawn->Heading);
                EQMACHUD_Rotate2D(g_mapOriginX, g_mapOriginY, line.X2, line.Y2, playerSpawn->Heading);
            }

            if
            (
                line.X1 > g_mapMaxX &&
                line.X2 > g_mapMaxX &&
                line.X1 < g_mapMinX &&
                line.X2 < g_mapMinX
            )
            {
                mapLine = mapLine->Next;
                continue;
            }

            if
            (
                line.Y1 > g_mapMaxY &&
                line.Y2 > g_mapMaxY &&
                line.Y1 < g_mapMinY &&
                line.Y2 < g_mapMinY
            )
            {
                mapLine = mapLine->Next;
                continue;
            }

            // Cohen-Sutherland algorithm
            // clip the map lines to the rectangle
            bool bDrawLine = EQMACHUD_DoClipLine(&line, g_mapMinX, g_mapMinY, g_mapMaxX, g_mapMaxY);

            // only use 75% of the maximum allowed number of deferred 2D items
            // the game needs to reserve some to draw stuff
            // this application also needs to reserve some
            if (g_numDeferred2dItems > (int)(EQ_GRAPHICS_DLL_DEFERRED_2D_ITEMS_MAX * 0.75f))
            {
                bDrawLine = false;
            }

            if (bDrawLine == true)
            {
                int mapLineColor = g_mapLineColor;

                if (mapLineColor == 0xDEADBEEF)
                {
                    EQARGBCOLOR argbColor;
                    argbColor.A = 0xFF;
                    argbColor.R = mapLine->R;
                    argbColor.G = mapLine->G;
                    argbColor.B = mapLine->B;

                    mapLineColor = argbColor.ARGB;

                    // replace black lines with white lines for visibility
                    if (g_mapReplaceBlackLinesWithWhiteLinesIsEnabled == true)
                    {
                        if (mapLineColor == 0xFF000000)
                        {
                            mapLineColor = 0xFFFFFFFF;
                        }
                    }
                }

                EQGfx_Dx8__t3dDeferLine(&line, mapLineColor);

                g_numDeferred2dItems += 1;
            }

            mapLine = mapLine->Next;
        }
    } // if (g_mapLinesIsEnabled == true)

    if (g_mapTargetLineIsEnabled == true)
    {
        if (targetSpawn && targetSpawn != playerSpawn)
        {
            EQLINE line;
            line.X1 = g_mapOriginX;
            line.Y1 = g_mapOriginY;
            line.Z1 = 1.0f;
            line.X2 = ((-targetSpawn->X * g_mapZoom) + g_mapOriginX) + (playerSpawn->X * g_mapZoom);
            line.Y2 = ((-targetSpawn->Y * g_mapZoom) + g_mapOriginY) + (playerSpawn->Y * g_mapZoom);
            line.Z2 = 1.0f;

            if (g_mapRotateIsEnabled == true)
            {
                EQMACHUD_Rotate2D(g_mapOriginX, g_mapOriginY, line.X1, line.Y1, playerSpawn->Heading);
                EQMACHUD_Rotate2D(g_mapOriginX, g_mapOriginY, line.X2, line.Y2, playerSpawn->Heading);
            }

            bool bDrawLine = EQMACHUD_DoClipLine(&line, g_mapMinX, g_mapMinY, g_mapMaxX, g_mapMaxY);

            if (bDrawLine == true)
            {
                EQGfx_Dx8__t3dDeferLine(&line, g_mapTargetLineColor);

                g_numDeferred2dItems += 1;
            }
        }
    }

    if (g_mapPointsIsEnabled == true && g_mapIsMaximized == true)
    {
        struct _EQMAPPOINT *mapPoint = g_mapPointListBegin;

        while (mapPoint)
        {
            if (g_mapLayer0IsEnabled == false && mapPoint->Layer == 0)
            {
                mapPoint = mapPoint->Next;
                continue;
            }

            if (g_mapLayer1IsEnabled == false && mapPoint->Layer == 1)
            {
                mapPoint = mapPoint->Next;
                continue;
            }

            if (g_mapLayer2IsEnabled == false && mapPoint->Layer == 2)
            {
                mapPoint = mapPoint->Next;
                continue;
            }

            if (g_mapLayer3IsEnabled == false && mapPoint->Layer == 3)
            {
                mapPoint = mapPoint->Next;
                continue;
            }

            float pointX = (((mapPoint->X * g_mapZoom) + g_mapOriginX) + (playerSpawn->X * g_mapZoom));
            float pointY = (((mapPoint->Y * g_mapZoom) + g_mapOriginY) + (playerSpawn->Y * g_mapZoom));

            if (g_mapRotateIsEnabled == true)
            {
                EQMACHUD_Rotate2D(g_mapOriginX, g_mapOriginY, pointX, pointY, playerSpawn->Heading);
            }

            if
            (
                pointX >= g_mapMaxX ||
                pointY >= g_mapMaxY ||
                pointX <= g_mapMinX ||
                pointY <= g_mapMinY
            )
            {
                mapPoint = mapPoint->Next;
                continue;
            }

            char pointText[128];
            sprintf_s(pointText, "+ %s", mapPoint->Text);

            EQ_CLASS_CDisplay->WriteTextHD2(pointText, (int)pointX, (int)pointY, g_mapPointTextColor, fontArial14);

            mapPoint = mapPoint->Next;
        }
    }

    if (g_mapSpawnsIsEnabled == true)
    {
        spawn = (PEQSPAWNINFO)EQ_OBJECT_FirstSpawn;

        while (spawn)
        {
            if (spawn == playerSpawn)
            {
                spawn = spawn->Next;
                continue;
            }

            if (spawn != targetSpawn)
            {
                if (g_mapShowPlayerIsEnabled == false && spawn->Type == EQ_SPAWN_TYPE_PLAYER)
                {
                    spawn = spawn->Next;
                    continue;
                }

                if (g_mapShowPlayerCorpseIsEnabled == false && spawn->Type == EQ_SPAWN_TYPE_PLAYER_CORPSE)
                {
                    spawn = spawn->Next;
                    continue;
                }

                if (g_mapShowNpcIsEnabled == false && spawn->Type == EQ_SPAWN_TYPE_NPC)
                {
                    spawn = spawn->Next;
                    continue;
                }

                if (g_mapShowNpcCorpseIsEnabled == false && spawn->Type == EQ_SPAWN_TYPE_NPC_CORPSE)
                {
                    spawn = spawn->Next;
                    continue;
                }
            }

            EQLOCATION spawnLocation = {spawn->Y, spawn->X, spawn->Z};

            float spawnDistance = EQ_CalculateDistance(playerLocation.X, playerLocation.Y, spawnLocation.X, spawnLocation.Y);

            if (g_mapSpawnDistanceIsEnabled == true)
            {
                bool bApplyDistance = false;

                if (g_mapSpawnDistancePlayerIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_PLAYER)
                {
                    bApplyDistance = true;
                }

                if (g_mapSpawnDistancePlayerCorpseIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_PLAYER_CORPSE)
                {
                    bApplyDistance = true;
                }

                if (g_mapSpawnDistanceNpcIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_NPC)
                {
                    bApplyDistance = true;
                }

                if (g_mapSpawnDistanceNpcCorpseIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_NPC_CORPSE)
                {
                    bApplyDistance = true;
                }

                // target is always visible
                if (spawn == targetSpawn)
                {
                    bApplyDistance = false;
                }

                // gamemaster is always visible
                if (spawn->IsGameMaster == 1)
                {
                    bApplyDistance = false;
                }

                if (bApplyDistance == true && spawnDistance > g_mapSpawnDistance)
                {
                    spawn = spawn->Next;
                    continue;
                }
            }

            DWORD textColor = EQ_TEXT_COLOR_WHITE;

            switch (spawn->Type)
            {
                case EQ_SPAWN_TYPE_PLAYER:
                    textColor = g_mapPlayerTextColor;
                    break;

                case EQ_SPAWN_TYPE_PLAYER_CORPSE:
                    textColor = g_mapPlayerCorpseTextColor;
                    break;

                case EQ_SPAWN_TYPE_NPC:
                    textColor = g_mapNpcTextColor;
                    break;

                case EQ_SPAWN_TYPE_NPC_CORPSE:
                    textColor = g_mapNpcCorpseTextColor;
                    break;
            }

            for (size_t i = 0; i < EQ_GROUP_MEMBERS_MAX; i++)
            {
                if (spawn == groupList->GroupMember[i])
                {
                    textColor = g_mapGroupMemberTextColor;
                    break;
                }
            }

            if (spawn == targetSpawn)
            {
                textColor = g_mapTargetTextColor;
            }

            if (spawn->IsGameMaster == 1)
            {
                textColor = g_mapGameMasterTextColor;
            }

            float mapSpawnX = (((-spawn->X * g_mapZoom) + g_mapOriginX) + (playerSpawn->X * g_mapZoom));
            float mapSpawnY = (((-spawn->Y * g_mapZoom) + g_mapOriginY) + (playerSpawn->Y * g_mapZoom));

            if (g_mapRotateIsEnabled == true)
            {
                EQMACHUD_Rotate2D(g_mapOriginX, g_mapOriginY, mapSpawnX, mapSpawnY, playerSpawn->Heading);
            }

            if
            (
                mapSpawnX >= g_mapMaxX ||
                mapSpawnY >= g_mapMaxY ||
                mapSpawnX <= g_mapMinX ||
                mapSpawnY <= g_mapMinY
            )
            {
                spawn = spawn->Next;
                continue;
            }

            const char* spawnName = EQ_CLASS_CEverQuest->trimName(spawn->Name);

            if (g_mapSpawnFilterNpcIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_NPC)
            {
                bool bFilter = true;

                char filter[4096];
                strcpy_s(filter, g_mapSpawnFilterNpc);

                char* filterToken;
                char* filterTokenNext = NULL;

                filterToken = strtok_s(filter, ",", &filterTokenNext);
                while (filterToken != NULL)
                {
                    if (strstr(spawnName, filterToken) != NULL)
                    {
                         bFilter = false;
                    }

                    filterToken = strtok_s(NULL, ",", &filterTokenNext);
                }

                if (bFilter == true)
                {
                    spawn = spawn->Next;
                    continue;
                }
            }

            if (g_mapIsMaximized == false)
            {
                EQ_CLASS_CDisplay->WriteTextHD2("*", (int)mapSpawnX, (int)mapSpawnY, textColor, fontArial14);
            }
            else
            {
                char spawnText[128];
                sprintf_s(spawnText, "+ %s", spawnName);

                EQ_CLASS_CDisplay->WriteTextHD2(spawnText, (int)mapSpawnX, (int)mapSpawnY, textColor, fontArial14);
            }

            if (EQMACHUD_IsPointInsideRectangle(mouseX, mouseY, (int)(mapSpawnX + 1) - 4, (int)(mapSpawnY + 6) - 4, 8, 8) == true)
            {
                if (mouseOverGuiElement == 0x00000000)
                {
                    if (g_mapSpawnClickToTargetIsEnabled == true)
                    {
                        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
                        {
                            EQ_OBJECT_TargetSpawn = spawn;
                        }
                    }

                    if (g_mapIsMaximized == false)
                    {
                        char spawnText[128];
                        sprintf_s(spawnText, "%s", spawnName);

                        EQ_CLASS_CDisplay->WriteTextHD2(spawnText, (int)mouseX + EQ_MOUSE_CURSOR_WIDTH + 1, (int)mouseY, textColor, fontArial14);
                    }
                }
            }

            spawn = spawn->Next;
        }

        if (g_mapYourselfCorpseLineIsEnabled == true)
        {
            spawn = (PEQSPAWNINFO)EQ_OBJECT_FirstSpawn;

            while (spawn)
            {
                if (spawn == playerSpawn)
                {
                    spawn = spawn->Next;
                    continue;
                }

                if (spawn->Type != EQ_SPAWN_TYPE_PLAYER_CORPSE)
                {
                    spawn = spawn->Next;
                    continue;
                }

                if (std::strstr(spawn->Name, playerSpawn->Name) != NULL)
                {
                    EQLINE line;
                    line.X1 = g_mapOriginX;
                    line.Y1 = g_mapOriginY;
                    line.Z1 = 1.0f;
                    line.X2 = ((-spawn->X * g_mapZoom) + g_mapOriginX) + (playerSpawn->X * g_mapZoom);
                    line.Y2 = ((-spawn->Y * g_mapZoom) + g_mapOriginY) + (playerSpawn->Y * g_mapZoom);
                    line.Z2 = 1.0f;

                    if (g_mapRotateIsEnabled == true)
                    {
                        EQMACHUD_Rotate2D(g_mapOriginX, g_mapOriginY, line.X1, line.Y1, playerSpawn->Heading);
                        EQMACHUD_Rotate2D(g_mapOriginX, g_mapOriginY, line.X2, line.Y2, playerSpawn->Heading);
                    }

                    bool bDrawLine = EQMACHUD_DoClipLine(&line, g_mapMinX, g_mapMinY, g_mapMaxX, g_mapMaxY);

                    if (bDrawLine == true)
                    {
                        EQGfx_Dx8__t3dDeferLine(&line, g_mapCorpseLineColor);

                        g_numDeferred2dItems += 1;
                    }
                }

                spawn = spawn->Next;
            }
        }
    } // if (g_mapSpawnsIsEnabled == true)

    EQLINE mapCenterLineHorizontal;
    mapCenterLineHorizontal.X1 = g_mapOriginX - g_mapCenterLineSize;
    mapCenterLineHorizontal.Y1 = g_mapOriginY;
    mapCenterLineHorizontal.Z1 = 1.0f;
    mapCenterLineHorizontal.X2 = g_mapOriginX + g_mapCenterLineSize;
    mapCenterLineHorizontal.Y2 = g_mapOriginY;
    mapCenterLineHorizontal.Z2 = 1.0f;

    EQLINE mapCenterLineVertical;
    mapCenterLineVertical.X1 = g_mapOriginX;
    mapCenterLineVertical.Y1 = g_mapOriginY  - g_mapCenterLineSize;
    mapCenterLineVertical.Z1 = 1.0f;
    mapCenterLineVertical.X2 = g_mapOriginX;
    mapCenterLineVertical.Y2 = g_mapOriginY  + g_mapCenterLineSize;
    mapCenterLineVertical.Z2 = 1.0f;

    EQGfx_Dx8__t3dDeferLine(&mapCenterLineHorizontal, g_mapArrowColor);
    EQGfx_Dx8__t3dDeferLine(&mapCenterLineVertical,   g_mapArrowColor);

    if (g_mapArrowIsEnabled == true && g_mapRotateIsEnabled == false)
    {
        float playerHeadingC = playerSpawn->Heading;

        playerHeadingC = playerHeadingC - 128.0f;

        if (playerHeadingC < 0.0f)
        {
            playerHeadingC = playerHeadingC + 512.0f;
        }

        float playerHeadingRadiansC = playerHeadingC * (g_mathPi / 256.0f);

        float arrowAddCX = std::cosf(playerHeadingRadiansC);
        arrowAddCX = arrowAddCX * g_mapArrowRadius;

        float arrowAddCY = std::sinf(playerHeadingRadiansC);
        arrowAddCY = arrowAddCY * g_mapArrowRadius;

        float arrowCX = -playerSpawn->X - arrowAddCX;
        float arrowCY = -playerSpawn->Y + arrowAddCY;

        float playerHeadingB = playerHeadingC;

        playerHeadingB = playerHeadingB - 32.0f;

        if (playerHeadingB < 0.0f)
        {
            playerHeadingB = playerHeadingB + 512.0f;
        }

        float playerHeadingRadiansB = playerHeadingB * (g_mathPi / 256.0f);

        float arrowAddBX = std::cosf(playerHeadingRadiansB);
        arrowAddBX = arrowAddBX * (g_mapArrowRadius * 0.5f);

        float arrowAddBY = std::sinf(playerHeadingRadiansB);
        arrowAddBY = arrowAddBY * (g_mapArrowRadius * 0.5f);

        float arrowBX = arrowCX + arrowAddBX;
        float arrowBY = arrowCY - arrowAddBY;

        float playerHeadingA = playerHeadingC;

        playerHeadingA = playerHeadingA + 32.0f;

        if (playerHeadingA > 512.0f)
        {
            playerHeadingA = playerHeadingA - 512.0f;
        }

        float playerHeadingRadiansA = playerHeadingA * (g_mathPi / 256.0f);

        float arrowAddAX = std::cosf(playerHeadingRadiansA);
        arrowAddAX = arrowAddAX * (g_mapArrowRadius * 0.5f);

        float arrowAddAY = std::sinf(playerHeadingRadiansA);
        arrowAddAY = arrowAddAY * (g_mapArrowRadius * 0.5f);

        float arrowAX = arrowCX + arrowAddAX;
        float arrowAY = arrowCY - arrowAddAY;

        float mapArrowCX = (arrowCX + g_mapOriginX) + playerSpawn->X;
        float mapArrowCY = (arrowCY + g_mapOriginY) + playerSpawn->Y;

        float mapArrowBX = (arrowBX + g_mapOriginX) + playerSpawn->X;
        float mapArrowBY = (arrowBY + g_mapOriginY) + playerSpawn->Y;

        float mapArrowAX = (arrowAX + g_mapOriginX) + playerSpawn->X;
        float mapArrowAY = (arrowAY + g_mapOriginY) + playerSpawn->Y;

        EQLINE mapArrowLineA;
        mapArrowLineA.X1 = mapArrowCX;
        mapArrowLineA.Y1 = mapArrowCY;
        mapArrowLineA.Z1 = 1.0f;
        mapArrowLineA.X2 = mapArrowAX;
        mapArrowLineA.Y2 = mapArrowAY;
        mapArrowLineA.Z2 = 1.0f;

        EQLINE mapArrowLineB;
        mapArrowLineB.X1 = mapArrowCX;
        mapArrowLineB.Y1 = mapArrowCY;
        mapArrowLineB.Z1 = 1.0f;
        mapArrowLineB.X2 = mapArrowBX;
        mapArrowLineB.Y2 = mapArrowBY;
        mapArrowLineB.Z2 = 1.0f;

        EQLINE mapArrowLineC;
        mapArrowLineC.X1 = g_mapOriginX;
        mapArrowLineC.Y1 = g_mapOriginY;
        mapArrowLineC.Z1 = 1.0f;
        mapArrowLineC.X2 = mapArrowCX;
        mapArrowLineC.Y2 = mapArrowCY;
        mapArrowLineC.Z2 = 1.0f;

        EQGfx_Dx8__t3dDeferLine(&mapArrowLineA, g_mapArrowColor);
        EQGfx_Dx8__t3dDeferLine(&mapArrowLineB, g_mapArrowColor);
        EQGfx_Dx8__t3dDeferLine(&mapArrowLineC, g_mapArrowColor);
    } // if (g_mapArrowIsEnabled == true)

    if (g_mapZoneInfoIsEnabled == true)
    {
        float textY = g_mapMaxY + g_elementOffset;

        char zoneText[128];
        sprintf_s(zoneText, "Zone: %s (ID: %d)", zoneInfo->ShortName, zoneId);
        EQ_CLASS_CDisplay->WriteTextHD2(zoneText, (int)g_mapX, (int)textY, g_mapZoneInfoTextColor, fontArial14);

        textY = textY + (g_fontHeight - 3);

        if (g_mapIsMaximized == true)
        {
            char locationText[128];
            sprintf_s(locationText, "Location: %.2f, %.2f, %.2f", playerLocation.Y, playerLocation.X, playerLocation.Z);
            EQ_CLASS_CDisplay->WriteTextHD2(locationText, (int)g_mapX, (int)textY, g_mapZoneInfoTextColor, fontArial14);

            textY = textY + (g_fontHeight - 3);
        }

        if (g_mapRotateIsEnabled == true)
        {
            char headingText[128];
            sprintf_s(headingText, "Heading: %s", EQ_GetCardinalDirectionByHeading(playerSpawn->Heading));
            EQ_CLASS_CDisplay->WriteTextHD2(headingText, (int)g_mapX, (int)textY, g_mapZoneInfoTextColor, fontArial14);

            textY = textY + (g_fontHeight - 3);
        }
    }
}

void EQMACHUD_DoEsp()
{
    DWORD fontArial14 = EQ_READ_MEMORY<DWORD>(EQ_POINTER_FONT_ARIAL14);

    WORD mouseX = EQ_READ_MEMORY<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_READ_MEMORY<WORD>(EQ_MOUSE_Y);

    DWORD mouseClickState = EQ_READ_MEMORY<DWORD>(EQ_MOUSE_CLICK_STATE);

    DWORD mouseOverGuiElement = EQ_READ_MEMORY<DWORD>(EQ_POINT_CXWND_MOUSE_OVER_GUI_ELEMENT);

    DWORD worldSpaceToScreenSpaceCameraData = EQ_READ_MEMORY<DWORD>(EQ_POINTER_WORLD_SPACE_TO_SCREEN_SPACE_CAMERA_DATA);

    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    PEQSPAWNINFO spawn = (PEQSPAWNINFO)EQ_OBJECT_FirstSpawn;

    PEQGROUNDSPAWNINFO groundSpawn = (PEQGROUNDSPAWNINFO)EQ_OBJECT_FirstGroundSpawn;

    PEQDOORSPAWNINFO doorSpawn = (PEQDOORSPAWNINFO)EQ_OBJECT_FirstDoorSpawn;

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    EQGROUPLIST* groupList = (EQGROUPLIST*)EQ_STRUCTURE_GROUP_LIST;

    EQLOCATION playerLocation = {EQ_OBJECT_PlayerSpawn->Y, EQ_OBJECT_PlayerSpawn->X, EQ_OBJECT_PlayerSpawn->Z};

    EQLOCATION targetLocation = {EQ_OBJECT_TargetSpawn->Y, EQ_OBJECT_TargetSpawn->X, EQ_OBJECT_TargetSpawn->Z};

    doorSpawn = (PEQDOORSPAWNINFO)EQ_OBJECT_FirstDoorSpawn;

    while (doorSpawn)
    {
        EQLOCATION spawnLocation = {doorSpawn->Y, doorSpawn->X, doorSpawn->Z};

        float spawnDistance = EQ_CalculateDistance(playerLocation.X, playerLocation.Y, spawnLocation.X, spawnLocation.Y);

        if (g_espDistanceIsEnabled == true)
        {
            if (g_espDistanceDoorSpawnIsEnabled == true && spawnDistance > g_espDistance)
            {
                doorSpawn = doorSpawn->Next;
                continue;
            }
        }

        float resultX = 0.0f;
        float resultY = 0.0f;
        int result = EQGfx_Dx8__t3dWorldSpaceToScreenSpace(worldSpaceToScreenSpaceCameraData, &spawnLocation, &resultX, &resultY);

        if (result != EQ_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
        {
            int screenX = (int)resultX;
            int screenY = (int)resultY;

            const char* spawnName = doorSpawn->Name;

            char spawnText[128];

            const char* doorDescription =
                EQ_KEYVALUESTRINGLIST_GetValueByKey
                (
                    EQ_KEYVALUESTRINGLIST_DOOR_SPAWN_NAME_DESCRIPTION,
                    EQ_KEYVALUESTRINGLISTSIZE_DOOR_SPAWN_NAME_DESCRIPTION,
                    doorSpawn->Name
                );

            if (doorDescription != NULL)
            {
                if (g_espShowDoorUniqueSpawnIsEnabled == true)
                {
                    sprintf_s(spawnText, "+ %s", doorDescription);
                }
                else
                {
                    doorSpawn = doorSpawn->Next;
                    continue;
                }
            }
            else
            {
                if (g_espShowDoorSpawnIsEnabled == true)
                {
                    sprintf_s(spawnText, "+ Door: %s", spawnName);
                }
                else
                {
                    doorSpawn = doorSpawn->Next;
                    continue;
                }
            }

            if (g_espShowTextDistanceIsEnabled == true)
            {
                char distanceText[32];
                sprintf_s(distanceText, " (%d)", (int)spawnDistance);
                strcat_s(spawnText, distanceText);
            }

            if (g_espShowTextIsEnabled == false)
            {
                EQ_CLASS_CDisplay->WriteTextHD2("+", screenX, screenY, g_espDoorSpawnTextColor, fontArial14);
            }
            else
            {
                EQ_CLASS_CDisplay->WriteTextHD2(spawnText, screenX, screenY, g_espDoorSpawnTextColor, fontArial14);
            }

            if (g_espShowTextIsEnabled == false && g_espShowTextOnMouseHover == true)
            {
                if (EQMACHUD_IsPointInsideRectangle(mouseX, mouseY, (int)(screenX + 1) - 4, (int)(screenY + 6) - 4, 8, 8) == true)
                {
                    if (mouseOverGuiElement == 0x00000000)
                    {
                        EQ_CLASS_CDisplay->WriteTextHD2(spawnText, (int)mouseX + EQ_MOUSE_CURSOR_WIDTH + 1, (int)mouseY, g_espDoorSpawnTextColor, fontArial14);
                    }
                }
            }
        }

        doorSpawn = doorSpawn->Next;
    }

    groundSpawn = (PEQGROUNDSPAWNINFO)EQ_OBJECT_FirstGroundSpawn;

    while (g_espShowGroundSpawnIsEnabled == true && groundSpawn)
    {
        EQLOCATION spawnLocation = {groundSpawn->Y, groundSpawn->X, groundSpawn->Z};

        float spawnDistance = EQ_CalculateDistance(playerLocation.X, playerLocation.Y, spawnLocation.X, spawnLocation.Y);

        if (g_espDistanceIsEnabled == true) 
        {
            if (g_espDistanceGroundSpawnIsEnabled == true && spawnDistance > g_espDistance)
            {
                groundSpawn = groundSpawn->Next;
                continue;
            }
        }

        float resultX = 0.0f;
        float resultY = 0.0f;
        int result = EQGfx_Dx8__t3dWorldSpaceToScreenSpace(worldSpaceToScreenSpaceCameraData, &spawnLocation, &resultX, &resultY);

        if (result != EQ_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
        {
            int screenX = (int)resultX;
            int screenY = (int)resultY;

            const char* spawnName =
                EQ_KEYVALUESTRINGLIST_GetValueByKey
                (
                    EQ_KEYVALUESTRINGLIST_GROUND_SPAWN_NAME_DESCRIPTION,
                    EQ_KEYVALUESTRINGLISTSIZE_GROUND_SPAWN_NAME_DESCRIPTION,
                    groundSpawn->Name
                );

            if (spawnName == NULL)
            {
                spawnName = groundSpawn->Name;
            }

            char spawnText[128];
            sprintf_s(spawnText, "+ %s", spawnName);

            if (g_espShowTextDistanceIsEnabled == true)
            {
                char distanceText[32];
                sprintf_s(distanceText, " (%d)", (int)spawnDistance);
                strcat_s(spawnText, distanceText);
            }

            if (g_espShowTextIsEnabled == false)
            {
                EQ_CLASS_CDisplay->WriteTextHD2("+", screenX, screenY, g_espGroundSpawnTextColor, fontArial14);
            }
            else
            {
                EQ_CLASS_CDisplay->WriteTextHD2(spawnText, screenX, screenY, g_espGroundSpawnTextColor, fontArial14);
            }

            if (g_espShowTextIsEnabled == false && g_espShowTextOnMouseHover == true)
            {
                if (EQMACHUD_IsPointInsideRectangle(mouseX, mouseY, (int)(screenX + 1) - 4, (int)(screenY + 6) - 4, 8, 8) == true)
                {
                    if (mouseOverGuiElement == 0x00000000)
                    {
                        EQ_CLASS_CDisplay->WriteTextHD2(spawnText, (int)mouseX + EQ_MOUSE_CURSOR_WIDTH + 1, (int)mouseY, g_espGroundSpawnTextColor, fontArial14);
                    }
                }
            }
        }

        groundSpawn = groundSpawn->Next;
    }

    spawn = (PEQSPAWNINFO)EQ_OBJECT_FirstSpawn;

    while (spawn)
    {
        if (spawn != targetSpawn)
        {
            if (g_espShowPlayerIsEnabled == false && spawn->Type == EQ_SPAWN_TYPE_PLAYER)
            {
                spawn = spawn->Next;
                continue;
            }

            if (g_espShowPlayerCorpseIsEnabled == false && spawn->Type == EQ_SPAWN_TYPE_PLAYER_CORPSE)
            {
                spawn = spawn->Next;
                continue;
            }

            if (g_espShowNpcIsEnabled == false && spawn->Type == EQ_SPAWN_TYPE_NPC)
            {
                spawn = spawn->Next;
                continue;
            }

            if (g_espShowNpcCorpseIsEnabled == false && spawn->Type == EQ_SPAWN_TYPE_NPC_CORPSE)
            {
                spawn = spawn->Next;
                continue;
            }
        }

        EQLOCATION spawnLocation = {spawn->Y, spawn->X, spawn->Z};

        float spawnDistance = EQ_CalculateDistance(playerLocation.X, playerLocation.Y, spawnLocation.X, spawnLocation.Y);

        if (g_espDistanceIsEnabled == true)
        {
            bool bApplyDistance = false;

            if (g_espDistancePlayerIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_PLAYER)
            {
                bApplyDistance = true;
            }

            if (g_espDistancePlayerCorpseIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_PLAYER_CORPSE)
            {
                bApplyDistance = true;
            }

            if (g_espDistanceNpcIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_NPC)
            {
                bApplyDistance = true;
            }

            if (g_espDistanceNpcCorpseIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_NPC_CORPSE)
            {
                bApplyDistance = true;
            }

            // target is always visible
            if (spawn == targetSpawn)
            {
                bApplyDistance = false;
            }

            // gamemaster is always visible
            if (spawn->IsGameMaster == 1)
            {
                bApplyDistance = false;
            }

            if (bApplyDistance == true && spawnDistance > g_espDistance)
            {
                spawn = spawn->Next;
                continue;
            }
        }

        float resultX = 0.0f;
        float resultY = 0.0f;
        int result = EQGfx_Dx8__t3dWorldSpaceToScreenSpace(worldSpaceToScreenSpaceCameraData, &spawnLocation, &resultX, &resultY);

        if (result != EQ_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
        {
            int screenX = (int)resultX;
            int screenY = (int)resultY;

            if (g_espShowYourselfIsEnabled == true && spawn == playerSpawn)
            {
                int playerTextY = screenY;

                if (g_espShowTextYourselfHpIsEnabled == true)
                {
                    int hpCurrent = spawn->HpCurrent;
                    int hpMax = spawn->HpMax;

                    if (hpCurrent > 0 && hpMax > 0)
                    {
                        int hpPercent = (hpCurrent * 100) / hpMax;

                        if (hpPercent < 100)
                        {
                            char hpText[128];
                            sprintf_s(hpText, "HP: %d/%d (%d%%)", hpCurrent, hpMax, hpPercent);

                            EQ_CLASS_CDisplay->WriteTextHD2(hpText, screenX, playerTextY, EQ_TEXT_COLOR_RED, fontArial14);

                            playerTextY = playerTextY + (g_fontHeight - 3);
                        }
                    }
                }

                if (g_espShowTextYourselfManaIsEnabled == true)
                {
                    if (charInfo)
                    {
                        int manaCurrent = charInfo->Mana;
                        int manaMax = EQ_CLASS_EQ_Character->Max_Mana();

                        if (manaCurrent > 0 && manaMax > 0 && manaCurrent < manaMax)
                        {
                            int manaPercent = (manaCurrent * 100) / manaMax;

                            char manaText[128];
                            sprintf_s(manaText, "MANA: %d/%d (%d%%)", manaCurrent, manaMax, manaPercent);

                            EQ_CLASS_CDisplay->WriteTextHD2(manaText, screenX, playerTextY, EQ_TEXT_COLOR_CYAN, fontArial14);

                            playerTextY = playerTextY + (g_fontHeight - 3);
                        }
                    }
                }

                spawn = spawn->Next;
                continue;
            }

            const char* spawnName = EQ_CLASS_CEverQuest->trimName(spawn->Name);

            if (g_espFilterNpcIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_NPC)
            {
                bool bFilter = true;

                char filter[4096];
                strcpy_s(filter, g_mapSpawnFilterNpc);

                char* filterToken;
                char* filterTokenNext = NULL;

                filterToken = strtok_s(filter, ",", &filterTokenNext);
                while (filterToken != NULL)
                {
                    if (strstr(spawnName, filterToken) != NULL)
                    {
                         bFilter = false;
                    }

                    filterToken = strtok_s(NULL, ",", &filterTokenNext);
                }

                if (bFilter == true)
                {
                    spawn = spawn->Next;
                    continue;
                }
            }

            DWORD textColor = EQ_TEXT_COLOR_WHITE;

            switch (spawn->Type)
            {
                case EQ_SPAWN_TYPE_PLAYER:
                    textColor = g_espPlayerTextColor;
                    break;

                case EQ_SPAWN_TYPE_PLAYER_CORPSE:
                    textColor = g_espPlayerCorpseTextColor;
                    break;

                case EQ_SPAWN_TYPE_NPC:
                    textColor = g_espNpcTextColor;
                    break;

                case EQ_SPAWN_TYPE_NPC_CORPSE:
                    textColor = g_espNpcCorpseTextColor;
                    break;
            }

            for (size_t i = 0; i < EQ_GROUP_MEMBERS_MAX; i++)
            {
                if (spawn == groupList->GroupMember[i])
                {
                    textColor = g_espGroupMemberTextColor;
                    break;
                }
            }

            if (spawn == targetSpawn)
            {
                textColor = g_espTargetTextColor;
            }

            char spawnText[128];
            sprintf_s(spawnText, "+ %s", spawnName);

            if (g_espShowTextIsGameMasterIsEnabled == true)
            {
                if (spawn->IsGameMaster == 1)
                {
                    textColor = g_espGameMasterTextColor;

                    strcat_s(spawnText, " GM");
                }
            }

            if (g_espShowTextIsPlayerKillIsEnabled == true)
            {
                if (spawn->IsPlayerKill == 1)
                {
                    strcat_s(spawnText, " PVP");
                }
            }

            if (g_espShowTextIsAwayFromKeyboardIsEnabled == true)
            {
                if (spawn->IsAwayFromKeyboard == 1)
                {
                    strcat_s(spawnText, " AFK");
                }
            }

            if (spawn->Type == EQ_SPAWN_TYPE_PLAYER)
            {
                if (g_espShowTextPlayerIsLinkDeadIsEnabled == true)
                {
                    if (spawn->IsLinkDead == 1)
                    {
                        strcat_s(spawnText, " LD");
                    }
                }

                if (g_espShowTextPlayerIsFeignDeathIsEnabled == true)
                {
                    if (spawn->StandingState == EQ_STANDING_STATE_FEIGNED)
                    {
                        strcat_s(spawnText, " FD");
                    }
                }
            }

            if (g_espShowTextLevelIsEnabled == true)
            {
                char levelText[32];
                sprintf_s(levelText, " L%d", spawn->Level);
                strcat_s(spawnText, levelText);
            }

            if (g_espShowTextPlayerClassIsEnabled == true)
            {
                if (spawn->Type == EQ_SPAWN_TYPE_PLAYER)
                {
                    char classText[32];
                    sprintf_s(classText, " %s", EQ_GetClassShortName(spawn->Class));
                    strcat_s(spawnText, classText);
                }
            }

            if (g_espShowTextDistanceIsEnabled == true)
            {
                char distanceText[32];
                sprintf_s(distanceText, " (%d)", (int)spawnDistance);
                strcat_s(spawnText, distanceText);
            }

            if (g_espShowTextIsEnabled == false)
            {
                EQ_CLASS_CDisplay->WriteTextHD2("+", screenX, screenY, textColor, fontArial14);
            }
            else
            {
                EQ_CLASS_CDisplay->WriteTextHD2(spawnText, screenX, screenY, textColor, fontArial14);

                if (spawn->Type == EQ_SPAWN_TYPE_PLAYER)
                {
                    if (g_espShowTextPlayerGuildIsEnabled == true)
                    {
                        if (spawn->GuildId != EQ_GUILD_ID_NULL)
                        {
                            screenY = screenY + (g_fontHeight - 3);

                            char guildText[128];
                            sprintf_s(guildText, "<%s>", EQ_GetGuildNameById(spawn->GuildId));

                            EQ_CLASS_CDisplay->WriteTextHD2(guildText, screenX, screenY, textColor, fontArial14);
                        }
                    }

                    if (g_espShowTextPlayerHpIsEnabled == true)
                    {
                        if (spawn->HpCurrent < 100)
                        {
                            screenY = screenY + (g_fontHeight - 3);

                            char hpText[128];
                            sprintf_s(hpText, "HP: %d%%", spawn->HpCurrent);

                            EQ_CLASS_CDisplay->WriteTextHD2(hpText, screenX, screenY, textColor, fontArial14);
                        }
                    }
                }
                else if (spawn->Type == EQ_SPAWN_TYPE_NPC)
                {
                    if (g_espShowTextNpcUniqueClassIsEnabled == true)
                    {
                        if
                        (
                            spawn->Class == EQ_CLASS_BANKER   ||
                            spawn->Class == EQ_CLASS_MERCHANT ||
                            (
                                spawn->Class >= EQ_CLASS_GUILDMASTER_BEGIN &&
                                spawn->Class <= EQ_CLASS_GUILDMASTER_END
                            )
                        )
                        {
                            screenY = screenY + (g_fontHeight - 3);

                            char classText[128];
                            sprintf_s(classText, "(%s)", EQ_STRING_CLASS_NAME[spawn->Class]);

                            EQ_CLASS_CDisplay->WriteTextHD2(classText, screenX, screenY, textColor, fontArial14);
                        }
                    }

                    if (g_espShowTextNpcHpIsEnabled == true)
                    {
                        if (spawn->HpCurrent < 100)
                        {
                            screenY = screenY + (g_fontHeight - 3);

                            char hpText[128];
                            sprintf_s(hpText, "HP: %d%%", spawn->HpCurrent);

                            EQ_CLASS_CDisplay->WriteTextHD2(hpText, screenX, screenY, textColor, fontArial14);
                        }
                    }
                }
            }

            if (g_espShowTextIsEnabled == false && g_espShowTextOnMouseHover == true)
            {
                if (EQMACHUD_IsPointInsideRectangle(mouseX, mouseY, (int)(screenX + 1) - 4, (int)(screenY + 6) - 4, 8, 8) == true)
                {
                    if (mouseOverGuiElement == 0x00000000)
                    {
                        EQ_CLASS_CDisplay->WriteTextHD2(spawnText, (int)mouseX + EQ_MOUSE_CURSOR_WIDTH + 1, (int)mouseY, textColor, fontArial14);
                    }
                }
            }
        }

        spawn = spawn->Next;
    }
}

void EQMACHUD_DoBuffs()
{
    DWORD fontArial14 = EQ_READ_MEMORY<DWORD>(EQ_POINTER_FONT_ARIAL14);

    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    EQ_CLASS_CDisplay->WriteTextHD2("Buffs", (int)g_buffsX, (int)g_buffsY, g_buffsTextColor, fontArial14);

    int buffY = (int)(g_buffsY) + (g_fontHeight - 3);

    for (size_t i = 0; i < EQ_BUFFS_MAX; i++)
    {
        int spellId = charInfo->Buffs[i].SpellId;

        if (spellId == EQ_SPELL_ID_NULL)
        {
            continue;
        }

        const char* spellName = EQ_OBJECT_SpellList->Spell[spellId]->Name;

        int buffTicks = charInfo->Buffs[i].Ticks;

        if (buffTicks == 0)
        {
            continue;
        }

        int buffHours   = 0;
        int buffMinutes = 0;
        int buffSeconds = 0;

        buffSeconds = buffTicks * 3;

        if (buffSeconds > 0)
        {
            buffHours = buffSeconds / (60 * 60);

            buffSeconds = buffSeconds - buffHours * (60 * 60);

            if (buffSeconds > 0)
            {
                buffMinutes = buffSeconds / 60;

                buffSeconds = buffSeconds - buffMinutes * 60;
            }
        }

        char buffText[128];
        sprintf_s(buffText, "%02d: (%02d:%02d:%02d) %s", i + 1, buffHours, buffMinutes, buffSeconds, spellName);

        EQ_CLASS_CDisplay->WriteTextHD2(buffText, (int)g_buffsX, (int)buffY, g_buffsTextColor, fontArial14);

        buffY = buffY + (g_fontHeight - 3);
    }
}

void EQMACHUD_DoPlayerInfo()
{
    DWORD fontArial14 = EQ_READ_MEMORY<DWORD>(EQ_POINTER_FONT_ARIAL14);

    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    if (playerSpawn == NULL)
    {
        return;
    }

    EQ_CLASS_CDisplay->WriteTextHD2("Player", (int)g_playerInfoX, (int)g_playerInfoY, g_playerInfoTextColor, fontArial14);

    int playerTextY = (int)(g_playerInfoY) + (g_fontHeight - 3);

    int hpCurrent = playerSpawn->HpCurrent;
    int hpMax = playerSpawn->HpMax;

    if (hpCurrent > 0 && hpMax > 0)
    {
        int hpPercent = (hpCurrent * 100) / hpMax;

        char hpText[128];
        sprintf_s(hpText, "HP: %d/%d (%d%%)", hpCurrent, hpMax, hpPercent);

        EQ_CLASS_CDisplay->WriteTextHD2(hpText, (int)g_playerInfoX, (int)playerTextY, g_playerInfoTextColor, fontArial14);

        playerTextY = playerTextY + (g_fontHeight - 3);
    }

    if (charInfo)
    {
        int manaCurrent = charInfo->Mana;
        int manaMax = EQ_CLASS_EQ_Character->Max_Mana();

        if (manaCurrent > 0 && manaMax > 0)
        {
            int manaPercent = (manaCurrent * 100) / manaMax;

            char manaText[128];
            sprintf_s(manaText, "MANA: %d/%d (%d%%)", manaCurrent, manaMax, manaPercent);

            EQ_CLASS_CDisplay->WriteTextHD2(manaText, (int)g_playerInfoX, (int)playerTextY, g_playerInfoTextColor, fontArial14);

            playerTextY = playerTextY + (g_fontHeight - 3);
        }
    }
}

void EQMACHUD_DoTargetInfo()
{
    DWORD fontArial14 = EQ_READ_MEMORY<DWORD>(EQ_POINTER_FONT_ARIAL14);

    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    if (targetSpawn == NULL)
    {
        return;
    }

    const char* targetName = EQ_CLASS_CEverQuest->trimName(targetSpawn->Name);

    char targetText[128];
    sprintf_s(targetText, "Target: %s", targetName);

    if (targetSpawn->IsPlayerKill == 1)
    {
        strcat_s(targetText, " PVP");
    }

    if (targetSpawn->IsAwayFromKeyboard == 1)
    {
        strcat_s(targetText, " AFK");
    }

    if (targetSpawn->Type == EQ_SPAWN_TYPE_PLAYER)
    {
        if (targetSpawn->IsLinkDead == 1)
        {
            strcat_s(targetText, " LD");
        }

        if (targetSpawn->StandingState == EQ_STANDING_STATE_FEIGNED)
        {
            strcat_s(targetText, " FD");
        }
    }

    EQ_CLASS_CDisplay->WriteTextHD2(targetText, (int)g_targetInfoX, (int)g_targetInfoY, g_targetInfoTextColor, fontArial14);

    int targetTextY = (int)(g_targetInfoY) + (g_fontHeight - 3);

    const char* raceShortName = EQ_GetRaceShortName(targetSpawn->Race);

    const char* classShortName = EQ_GetClassShortName(targetSpawn->Class);

    char levelRaceClassText[128];
    sprintf_s(levelRaceClassText, "L%d %s %s", targetSpawn->Level, raceShortName, classShortName);

    EQ_CLASS_CDisplay->WriteTextHD2(levelRaceClassText, (int)g_targetInfoX, (int)targetTextY, g_targetInfoTextColor, fontArial14);

    targetTextY = targetTextY + (g_fontHeight - 3);

    if (targetSpawn->GuildId != EQ_GUILD_ID_NULL)
    {
        char guildText[128];
        sprintf_s(guildText, "<%s>", EQ_GetGuildNameById(targetSpawn->GuildId));

        EQ_CLASS_CDisplay->WriteTextHD2(guildText, (int)g_targetInfoX, (int)targetTextY, g_targetInfoTextColor, fontArial14);

        targetTextY = targetTextY + (g_fontHeight - 3);
    }

    if (targetSpawn->HpCurrent < 100)
    {
        char hpText[128];

        if (targetSpawn == playerSpawn)
        {
            int hpCurrent = targetSpawn->HpCurrent;
            int hpMax = targetSpawn->HpMax;

            if (hpCurrent > 0 && hpMax > 0)
            {
                int hpPercent = (hpCurrent * 100) / hpMax;

                sprintf_s(hpText, "HP: %d/%d (%d%%)", hpCurrent, hpMax, hpPercent);
            }
        }
        else
        {
            int hpCurrent = targetSpawn->HpCurrent;

            if (hpCurrent > 0)
            {
                sprintf_s(hpText, "HP: %d%%", targetSpawn->HpCurrent);
            }
        }

        EQ_CLASS_CDisplay->WriteTextHD2(hpText, (int)g_targetInfoX, (int)targetTextY, g_targetInfoTextColor, fontArial14);

        targetTextY = targetTextY + (g_fontHeight - 3);
    }

    if (targetSpawn == playerSpawn && charInfo)
    {
        int manaCurrent = charInfo->Mana;
        int manaMax = EQ_CLASS_EQ_Character->Max_Mana();

        if (manaCurrent > 0 && manaMax > 0)
        {
            int manaPercent = (manaCurrent * 100) / manaMax;

            char manaText[128];
            sprintf_s(manaText, "MANA: %d/%d (%d%%)", manaCurrent, manaMax, manaPercent);

            EQ_CLASS_CDisplay->WriteTextHD2(manaText, (int)g_targetInfoX, (int)targetTextY, g_targetInfoTextColor, fontArial14);

            targetTextY = targetTextY + (g_fontHeight - 3);
        }
    }
}

void EQMACHUD_DoHealthBars()
{
    WORD mouseX = EQ_READ_MEMORY<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_READ_MEMORY<WORD>(EQ_MOUSE_Y);

    DWORD mouseClickState = EQ_READ_MEMORY<DWORD>(EQ_MOUSE_CLICK_STATE);

    DWORD mouseOverGuiElement = EQ_READ_MEMORY<DWORD>(EQ_POINT_CXWND_MOUSE_OVER_GUI_ELEMENT);

    DWORD worldSpaceToScreenSpaceCameraData = EQ_READ_MEMORY<DWORD>(EQ_POINTER_WORLD_SPACE_TO_SCREEN_SPACE_CAMERA_DATA);

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    EQLOCATION playerLocation = {EQ_OBJECT_PlayerSpawn->Y, EQ_OBJECT_PlayerSpawn->X, EQ_OBJECT_PlayerSpawn->Z};

    PEQSPAWNINFO spawn = (PEQSPAWNINFO)EQ_OBJECT_FirstSpawn;

    while (spawn)
    {
        EQLOCATION spawnLocation = {spawn->Y, spawn->X, spawn->Z};

        float spawnDistance = EQ_CalculateDistance(playerLocation.X, playerLocation.Y, spawnLocation.X, spawnLocation.Y);

        if (g_healthBarsDistanceIsEnabled == true)
        {
            if (spawnDistance > g_healthBarsDistance && spawn != targetSpawn)
            {
                spawn = spawn->Next;
                continue;
            }
        }

        if (spawn->Type == EQ_SPAWN_TYPE_PLAYER_CORPSE || spawn->Type == EQ_SPAWN_TYPE_NPC_CORPSE)
        {
            spawn = spawn->Next;
            continue;
        }

        int hpCurrent = spawn->HpCurrent;
        int hpMax     = spawn->HpMax;

        if (hpCurrent <= 0 || hpCurrent >= hpMax)
        {
            spawn = spawn->Next;
            continue;
        }

        float hpPercent = 0.0f;

        if (hpCurrent > 0 && hpMax > 0)
        {
            hpPercent = (float)hpCurrent / (float)hpMax;
        }

        if (hpPercent == 0.0 || hpPercent == 1.0)
        {
            spawn = spawn->Next;
            continue;
        }

        float spawnZOffset = 0.0f;

        if
        (
            spawn->StandingState == EQ_STANDING_STATE_SITTING ||
            spawn->StandingState == EQ_STANDING_STATE_DUCKING ||
            spawn->StandingState == EQ_STANDING_STATE_LOOTING ||
            spawn->StandingState == EQ_STANDING_STATE_FEIGNED
        )
        {
            spawnZOffset = spawn->CameraHeightOffset * 0.5f;
        }
        else
        {
            spawnZOffset = spawn->CameraHeightOffset;
        }

        EQLOCATION healthBarLocation =
        {
            spawn->Y,
            spawn->X,
            spawn->Z + spawnZOffset
        };

        DWORD healthBarForegroundColor = EQMACHUD_HEALTH_BAR_DEFAULT_FOREGROUND_COLOR;

        if (spawn->Type == EQ_SPAWN_TYPE_PLAYER)
        {
            healthBarForegroundColor = g_healthBarsForegroundPlayerColor;
        }
        else if (spawn->Type == EQ_SPAWN_TYPE_NPC)
        {
            healthBarForegroundColor = g_healthBarsForegroundNpcColor;
        }

        if (spawn == targetSpawn)
        {
            healthBarForegroundColor = g_healthBarsForegroundTargetColor;
        }

        float resultX = 0.0f;
        float resultY = 0.0f;
        int result = EQGfx_Dx8__t3dWorldSpaceToScreenSpace(worldSpaceToScreenSpaceCameraData, &healthBarLocation, &resultX, &resultY);

        if (result != EQ_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
        {
            float screenX = resultX;
            float screenY = resultY;

            screenX = screenX - g_healthBarsWidth * 0.5f;

            EQ_DrawRectangle(screenX, screenY, g_healthBarsWidth, g_healthBarsHeight, g_healthBarsBackgroundColor, true);
            EQ_DrawRectangle(screenX + 1.0f, screenY + 1.0f, (g_healthBarsWidth - 2.0f) * hpPercent, g_healthBarsHeight - 2.0f, healthBarForegroundColor, true);

            if (g_healthBarsClickToTargetIsEnabled == true)
            {
                if (EQMACHUD_IsPointInsideRectangle(mouseX, mouseY, (int)screenX, (int)screenY, (int)g_healthBarsWidth, (int)g_healthBarsHeight) == true)
                {
                    if (mouseOverGuiElement == 0x00000000)
                    {
                        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
                        {
                            EQ_OBJECT_TargetSpawn = spawn;
                        }
                    }
                }
            }
        }

        spawn = spawn->Next;
        continue;
    }
}
    
void EQMACHUD_DoItemDisplayText()
{
    DWORD mouseOverGuiElement = EQ_READ_MEMORY<DWORD>(EQ_POINT_CXWND_MOUSE_OVER_GUI_ELEMENT);

    if (mouseOverGuiElement == 0x00000000)
    {
        return;
    }

    PEQCITEMDISPLAYWND itemDisplayWnd = (PEQCITEMDISPLAYWND)EQ_OBJECT_CItemDisplayWnd;

    if (itemDisplayWnd->IsVisible == 0)
    {
        return;
    }

    //DWORD fontArial14 = EQ_READ_MEMORY<DWORD>(EQ_POINTER_FONT_ARIAL14);

    WORD mouseX = EQ_READ_MEMORY<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_READ_MEMORY<WORD>(EQ_MOUSE_Y);

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            itemDisplayWnd->X1, itemDisplayWnd->Y1,
            itemDisplayWnd->X2 - itemDisplayWnd->X1,
            itemDisplayWnd->Y2 - itemDisplayWnd->Y1
        )
        == false
    )
    {
        return;
    }

    if (g_itemDisplayTextItemValueIsEnabled == true && itemDisplayWnd->Item.Cost > 0)
    {
        int itemCost = itemDisplayWnd->Item.Cost;

        int itemCostPlatinum;
        int itemCostGold;
        int itemCostSilver;
        int itemCostCopper;

        if (itemCost > 0)
        {
            itemCostPlatinum = itemCost / 1000;

            itemCost = itemCost % 1000;

            itemCostGold = itemCost / 100;

            itemCost = itemCost % 100;

            itemCostSilver = itemCost / 10;

            itemCost = itemCost % 10;

            itemCostCopper = itemCost;
        }

        char itemCostText[128];
        strcpy_s(itemCostText, " Item Value: ");

        if (itemCostPlatinum > 0)
        {
            char platinumText[128];
            sprintf_s(platinumText, "%dp ", itemCostPlatinum);

            strcat_s(itemCostText, platinumText);
        }

        if (itemCostGold > 0)
        {
            char goldText[128];
            sprintf_s(goldText, "%dg ", itemCostGold);

            strcat_s(itemCostText, goldText);
        }

        if (itemCostSilver > 0)
        {
            char silverText[128];
            sprintf_s(silverText, "%ds ", itemCostSilver);

            strcat_s(itemCostText, silverText);
        }

        if (itemCostCopper > 0)
        {
            char copperText[128];
            sprintf_s(copperText, "%dc ", itemCostCopper);

            strcat_s(itemCostText, copperText);
        }

        EQ_DrawTooltipText(itemCostText, mouseX + EQ_MOUSE_CURSOR_WIDTH + 1, mouseY, EQ_TEXT_COLOR_WHITE, EQ_POINTER_FONT_ARIAL14);
    }
}

void EQMACHUD_DoMessageText()
{
    if (strlen(g_messageText) == 0)
    {
        return;
    }

    DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

    if (currentTime < g_messageTextTimer)
    {
        DWORD fontArial16 = EQ_READ_MEMORY<DWORD>(EQ_POINTER_FONT_ARIAL16);

        int textX = (int)g_messageTextX;

        if (g_messageTextIsCentered == true)
        {
            int textWidth = EQ_GetFontTextWidth(EQ_POINTER_FONT_ARIAL16, g_messageText);

            textX = textX - (int)(textWidth * 0.5f);
        }

        EQ_CLASS_CDisplay->WriteTextHD2(g_messageText, (int)textX, (int)g_messageTextY, g_messageTextColor, fontArial16);
    }
}

void EQMACHUD_DoMessageTextGainedExperience()
{
    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    if (charInfo == NULL)
    {
        return;
    }

    unsigned int currentExperience = charInfo->Experience;

    if (currentExperience > g_previousExperience)
    {
        unsigned int gainedExperience = currentExperience - g_previousExperience;

        float gainedExperiencePercent = (float)((gainedExperience * 100) / EQ_EXPERIENCE_MAX);

        g_previousExperience = currentExperience;

        sprintf_s(g_messageText, "You gained %d experience! (%.2f%%)", gainedExperience, gainedExperiencePercent);

        g_messageTextColor = EQ_TEXT_COLOR_YELLOW;

        DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

        g_messageTextTimer = currentTime + g_messageTextDelay;

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            EQ_CLASS_CEverQuest->dsp_chat(g_messageText, EQ_TEXT_COLOR_YELLOW, true);
        }
    }
}

int __cdecl EQMACHUD_DETOUR_CEverQuest__LMouseDown(unsigned short a1, unsigned short a2)
{
    if (g_bExit == 1)
    {
        return EQMACHUD_REAL_CEverQuest__LMouseDown(a1, a2);
    }

    BYTE isInGame = EQ_READ_MEMORY<BYTE>(EQ_IS_IN_GAME);

    if (isInGame == 1)
    {
        BYTE isNetStatusEnabled = EQ_READ_MEMORY<BYTE>(EQ_IS_NET_STATUS_ENABLED);

        if (isNetStatusEnabled == 1)
        {
            if (EQMACHUD_DoButtonMouseLeftDown() == true)
            {
                return EQMACHUD_REAL_CEverQuest__LMouseDown(0xFFFF, 0xFFFF);
            }
        }
    }

    return EQMACHUD_REAL_CEverQuest__LMouseDown(a1, a2);
}

int __cdecl EQMACHUD_DETOUR_CEverQuest__LMouseUp(unsigned short a1, unsigned short a2)
{
    if (g_bExit == 1)
    {
        return EQMACHUD_REAL_CEverQuest__LMouseUp(a1, a2);
    }

    BYTE isInGame = EQ_READ_MEMORY<BYTE>(EQ_IS_IN_GAME);

    if (isInGame == 1)
    {
        BYTE isNetStatusEnabled = EQ_READ_MEMORY<BYTE>(EQ_IS_NET_STATUS_ENABLED);

        if (isNetStatusEnabled == 1)
        {
            if (EQMACHUD_DoButtonMouseLeftUp() == true)
            {
                return EQMACHUD_REAL_CEverQuest__LMouseUp(0xFFFF, 0xFFFF);
            }
        }
    }

    return EQMACHUD_REAL_CEverQuest__LMouseUp(a1, a2);
}

int __cdecl EQMACHUD_DETOUR_HandleMouseWheel(int a1)
{
    if (g_bExit == 1)
    {
        return EQMACHUD_REAL_HandleMouseWheel(a1);
    }

    int mouseWheelDelta = a1;

    BYTE isInGame = EQ_READ_MEMORY<BYTE>(EQ_IS_IN_GAME);

    if (isInGame == 1)
    {
        BYTE isNetStatusEnabled = EQ_READ_MEMORY<BYTE>(EQ_IS_NET_STATUS_ENABLED);

        if (isNetStatusEnabled == 1)
        {
            DWORD mouseOverGuiElement = EQ_READ_MEMORY<DWORD>(EQ_POINT_CXWND_MOUSE_OVER_GUI_ELEMENT);

            if (mouseOverGuiElement == 0x00000000)
            {
                WORD mouseX = EQ_READ_MEMORY<WORD>(EQ_MOUSE_X);
                WORD mouseY = EQ_READ_MEMORY<WORD>(EQ_MOUSE_Y);

                if
                (
                    EQMACHUD_IsPointInsideRectangle
                    (
                        mouseX, mouseY,
                        (int)g_mapX,     (int)g_mapY,
                        (int)g_mapWidth, (int)g_mapHeight
                    )
                    == true
                )
                {
                    if (mouseWheelDelta == EQ_MOUSE_WHEEL_DELTA_UP)
                    {
                        EQMACHUD_MapMouseWheelZoomIn();
                    }
                    else if (mouseWheelDelta == EQ_MOUSE_WHEEL_DELTA_DOWN)
                    {
                        EQMACHUD_MapMouseWheelZoomOut();
                    }
                }
            }
        }
    }

    return EQMACHUD_REAL_HandleMouseWheel(a1);
}

int __cdecl EQMACHUD_DETOUR_CEverQuest__dsp_chat(const char* a1, short a2, bool a3)
{
    if (g_messageTextGainedExperienceIsEnabled == true)
    {
        if (strcmp(a1, "You gain experience!!") == 0)
        {
            EQMACHUD_DoMessageTextGainedExperience();
        }
    }

    return EQMACHUD_REAL_CEverQuest__dsp_chat(a1, a2, a3);
}

int __cdecl EQMACHUD_DETOUR_DrawNetStatus(int a1, unsigned short a2, unsigned short a3, unsigned short a4, unsigned short a5, int a6, unsigned short a7, unsigned long a8, long a9, unsigned long a10)
{
    if (g_bExit == 1)
    {
        return EQMACHUD_REAL_DrawNetStatus(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    }

    if (g_bLoaded == false)
    {
        g_bLoaded = true;

        EQ_WRITE_MEMORY<BYTE>(EQ_IS_NET_STATUS_ENABLED, 1);

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            BYTE isInGame = EQ_READ_MEMORY<BYTE>(EQ_IS_IN_GAME);

            if (isInGame == 1)
            {
                char loadedText[128];
                sprintf_s(loadedText, "%s loaded.", g_applicationName);

                EQ_CLASS_CEverQuest->dsp_chat(loadedText);
            }
        }
    }

    g_numDeferred2dItems = 0;

    BYTE isNetStatusEnabled = EQ_READ_MEMORY<BYTE>(EQ_IS_NET_STATUS_ENABLED);

    if (isNetStatusEnabled == 0)
    {
        g_messageTextTimer = 0;
    }

    EQMACHUD_DoUpdateZone();

    if (g_healthBarsIsEnabled == true)
    {
        EQMACHUD_DoHealthBars();
    }

    EQMACHUD_DoButtonMouseLeftHeldDown();

    if (g_espIsEnabled == true && g_mapIsMaximized == false)
    {
        EQMACHUD_DoEsp();
    }

    if (g_playerInfoIsEnabled == true)
    {
        EQMACHUD_DoPlayerInfo();
    }

    if (g_targetInfoIsEnabled == true)
    {
        EQMACHUD_DoTargetInfo();
    }

    if (g_buffsIsEnabled == true)
    {
        EQMACHUD_DoBuffs();
    }

    if (g_mapIsEnabled == true)
    {
        EQMACHUD_DoMap();
    }

    if (g_itemDisplayTextIsEnabled == true)
    {
        EQMACHUD_DoItemDisplayText();
    }

    if (g_messageTextIsEnabled == true)
    {
        EQMACHUD_DoMessageText();
    }

    EQMACHUD_DoExitRowButtons();

    EQMACHUD_DoButtonToolTipText();

    return EQMACHUD_REAL_DrawNetStatus(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

DWORD WINAPI EQMACHUD_ThreadLoop(LPVOID param)
{
    while (g_bExit == 0)
    {
        Sleep(1000);
    }

    DetourRemove((PBYTE)EQMACHUD_REAL_CEverQuest__LMouseDown, (PBYTE)EQMACHUD_DETOUR_CEverQuest__LMouseDown);
    DetourRemove((PBYTE)EQMACHUD_REAL_CEverQuest__LMouseUp,   (PBYTE)EQMACHUD_DETOUR_CEverQuest__LMouseUp);

    DetourRemove((PBYTE)EQMACHUD_REAL_HandleMouseWheel, (PBYTE)EQMACHUD_DETOUR_HandleMouseWheel);

    DetourRemove((PBYTE)EQMACHUD_REAL_CEverQuest__dsp_chat, (PBYTE)EQMACHUD_DETOUR_CEverQuest__dsp_chat);

    DetourRemove((PBYTE)EQMACHUD_REAL_DrawNetStatus, (PBYTE)EQMACHUD_DETOUR_DrawNetStatus);

    FreeLibraryAndExitThread(g_module, 0);
    return 0;
}

DWORD WINAPI EQMACHUD_ThreadLoad(LPVOID param)
{
    HINSTANCE graphicsDll = LoadLibraryA(EQ_STRING_GRAPHICS_DLL_NAME);
    if (!graphicsDll)
    {
        MessageBoxA(NULL, "Error: Failed to load graphics DLL!", g_applicationName, MB_ICONERROR);

        FreeLibraryAndExitThread(g_module, 0);
        return 0;
    }

    EQGfx_Dx8__t3dWorldSpaceToScreenSpace = (EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dWorldSpaceToScreenSpace)GetProcAddress(graphicsDll, EQ_GRAPHICS_DLL_FUNCTION_NAME_t3dWorldSpaceToScreenSpace);
    if (!EQGfx_Dx8__t3dWorldSpaceToScreenSpace)
    {
        MessageBoxA(NULL, "Error: World to screen function not found!", g_applicationName, MB_ICONERROR);

        FreeLibraryAndExitThread(g_module, 0);
        return 0;
    }

    EQGfx_Dx8__t3dDeferLine = (EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dDeferLine)GetProcAddress(graphicsDll, EQ_GRAPHICS_DLL_FUNCTION_NAME_t3dDeferLine);
    if (!EQGfx_Dx8__t3dDeferLine)
    {
        MessageBoxA(NULL, "Error: Draw line function not found!", g_applicationName, MB_ICONERROR);

        FreeLibraryAndExitThread(g_module, 0);
        return 0;
    }

    EQGfx_Dx8__t3dDeferRect = (EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dDeferRect)GetProcAddress(graphicsDll, EQ_GRAPHICS_DLL_FUNCTION_NAME_t3dDeferRect);
    if (!EQGfx_Dx8__t3dDeferRect)
    {
        MessageBoxA(NULL, "Error: Draw rectangle function not found!", g_applicationName, MB_ICONERROR);

        FreeLibraryAndExitThread(g_module, 0);
        return 0;
    }

    EQGfx_Dx8__t3dDeferQuad = (EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dDeferQuad)GetProcAddress(graphicsDll, EQ_GRAPHICS_DLL_FUNCTION_NAME_t3dDeferQuad);
    if (!EQGfx_Dx8__t3dDeferQuad)
    {
        MessageBoxA(NULL, "Error: Draw quad function not found!", g_applicationName, MB_ICONERROR);

        FreeLibraryAndExitThread(g_module, 0);
        return 0;
    }

    EQMACHUD_DoLoadConfig();

    EQMACHUD_GuiRecalculateCoordinates();

    g_handleThreadLoop = CreateThread(NULL, 0, &EQMACHUD_ThreadLoop, NULL, 0, NULL);

    EQMACHUD_REAL_CEverQuest__LMouseDown = (EQ_FUNCTION_TYPE_CEverQuest__LMouseDown)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__LMouseDown, (PBYTE)EQMACHUD_DETOUR_CEverQuest__LMouseDown);
    EQMACHUD_REAL_CEverQuest__LMouseUp   = (EQ_FUNCTION_TYPE_CEverQuest__LMouseUp)  DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__LMouseUp,   (PBYTE)EQMACHUD_DETOUR_CEverQuest__LMouseUp);

    EQMACHUD_REAL_HandleMouseWheel = (EQ_FUNCTION_TYPE_HandleMouseWheel)DetourFunction((PBYTE)EQ_FUNCTION_HandleMouseWheel, (PBYTE)EQMACHUD_DETOUR_HandleMouseWheel);

    EQMACHUD_REAL_CEverQuest__dsp_chat = (EQ_FUNCTION_TYPE_CEverQuest__dsp_chat)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__dsp_chat, (PBYTE)EQMACHUD_DETOUR_CEverQuest__dsp_chat);

    EQMACHUD_REAL_DrawNetStatus = (EQ_FUNCTION_TYPE_DrawNetStatus)DetourFunction((PBYTE)EQ_FUNCTION_DrawNetStatus, (PBYTE)EQMACHUD_DETOUR_DrawNetStatus);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    g_module = module;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(module);
            g_handleThreadLoad = CreateThread(NULL, 0, &EQMACHUD_ThreadLoad, NULL, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            //WaitForSingleObject(g_handleThreadLoad, INFINITE);
            CloseHandle(g_handleThreadLoad);

            //WaitForSingleObject(g_handleThreadLoop, INFINITE);
            CloseHandle(g_handleThreadLoop);
            break;
    }

    return TRUE;
}
