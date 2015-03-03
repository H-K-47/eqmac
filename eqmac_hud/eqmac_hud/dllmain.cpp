#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>

#include <windows.h>

#include "detours.h"
#pragma comment(lib, "detours.lib")

#include "eqmac.hpp"
#include "eqmac_functions.hpp"

const char* g_applicationName = "EQMac HUD";

const char* g_applicationConfigFilename = "eqmac_hud.ini";

volatile int g_bExit = 0;

HMODULE g_module;

HANDLE g_handleThreadLoad;
HANDLE g_handleThreadLoop;

EQ_FUNCTION_TYPE_DrawNetStatus EQMACHUD_DrawNetStatus_REAL = NULL;

float g_mathPi = 3.14159265358979f;

int g_zoneId = 0;

int g_numLinesDeferred = 0;

int g_fontHeight = 10;

float g_elementOffset = 5.0f;

float g_buttonWidth  = 10.0f;
float g_buttonHeight = 10.0f;

DWORD g_buttonColorEnabled     = 0xFF00FF00;
DWORD g_buttonColorDisabled    = 0xFFFF0000;
DWORD g_buttonColorMinMaxClose = 0xFFFF00FF; // - + x

int g_buttonTextColorEnabled     = EQ_TEXT_COLOR_LIGHT_GREEN;
int g_buttonTextColorDisabled    = EQ_TEXT_COLOR_RED;
int g_buttonTextColorMinMaxClose = EQ_TEXT_COLOR_PINK;

float g_buttonExitX = 138.0;
float g_buttonExitY = 7.0f;

float g_buttonToggleMapX;
float g_buttonToggleMapY;

float g_buttonToggleEspX;
float g_buttonToggleEspY;

float g_buttonToggleEspDoorsX;
float g_buttonToggleEspDoorsY;

float g_buttonToggleEspDistanceX;
float g_buttonToggleEspDistanceY;

float g_buttonToggleBuffsX;
float g_buttonToggleBuffsY;

float g_buttonTogglePlayerInfoX;
float g_buttonTogglePlayerInfoY;

float g_buttonToggleTargetInfoX;
float g_buttonToggleTargetInfoY;

unsigned int g_mouseLeftClickTimer;
unsigned int g_mouseLeftClickDelay = 250; // delay in milliseconds

bool g_mapIsEnabled = true;

bool g_mapIsMinimized = false;
bool g_mapIsMaximized = false;

bool g_mapArrowIsEnabled        = true;
bool g_mapZoneInfoIsEnabled     = true;
bool g_mapHeightFilterIsEnabled = false;

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

float g_mapDefaultX = 5.0f;
float g_mapDefaultY = 55.0f;

float g_mapDefaultWidth  = 200.0f;
float g_mapDefaultHeight = 200.0f;

float g_mapX = g_mapDefaultX;
float g_mapY = g_mapDefaultY;

float g_mapWidth = g_mapDefaultWidth;
float g_mapHeight = g_mapDefaultHeight;

float g_mapMaximizedX = 5.0f;
float g_mapMaximizedY = 55.0f;

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

float g_mapArrowRadius = 20.0f;

float g_mapCenterLineSize = 5.0f;

DWORD g_mapBorderColor     = 0xFF00FF00;
DWORD g_mapArrowColor      = 0xFF00FF00;
DWORD g_mapLineColor       = 0xDEADBEEF; // DEADBEEF uses the map txt file line colors
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

float g_mapButtonZoomInX;
float g_mapButtonZoomInY;

float g_mapButtonZoomOutX;
float g_mapButtonZoomOutY;

float g_mapButtonZoom1X;
float g_mapButtonZoom1Y;

float g_mapButtonToggleLineColorX;
float g_mapButtonToggleLineColorY;

float g_mapButtonToggleHeightFilterX;
float g_mapButtonToggleHeightFilterY;

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

float g_mapButtonMinimizeX;
float g_mapButtonMinimizeY;

float g_mapButtonMaximizeX;
float g_mapButtonMaximizeY;

float g_mapButtonRestoreX;
float g_mapButtonRestoreY;

struct _EQMAPLINE* g_mapLineListHead = NULL;
struct _EQMAPPOINT* g_mapPointListHead = NULL;

bool g_espIsEnabled = true;

bool g_espShowYourselfIsEnabled     = true;
bool g_espShowPlayerIsEnabled       = true;
bool g_espShowPlayerCorpseIsEnabled = true;
bool g_espShowNpcIsEnabled          = true;
bool g_espShowNpcCorpseIsEnabled    = true;
bool g_espShowGroundSpawnIsEnabled  = true;
bool g_espShowDoorIsEnabled         = false;
bool g_espShowUniqueDoorIsEnabled   = true;

bool g_espDistanceIsEnabled = true;
float g_espDistance = 400.0f;

bool g_espDistancePlayerIsEnabled       = false;
bool g_espDistancePlayerCorpseIsEnabled = false;
bool g_espDistanceNpcIsEnabled          = true;
bool g_espDistanceNpcCorpseIsEnabled    = false;
bool g_espDistanceGroundSpawnIsEnabled  = true;
bool g_espDistanceDoorIsEnabled         = true;

int g_espPlayerTextColor       = EQ_TEXT_COLOR_RED;
int g_espPlayerCorpseTextColor = EQ_TEXT_COLOR_YELLOW;
int g_espNpcTextColor          = EQ_TEXT_COLOR_CYAN;
int g_espNpcCorpseTextColor    = EQ_TEXT_COLOR_YELLOW;
int g_espGroupMemberTextColor  = EQ_TEXT_COLOR_LIGHT_GREEN;
int g_espTargetTextColor       = EQ_TEXT_COLOR_PINK;
int g_espGameMasterTextColor   = EQ_TEXT_COLOR_PINK;
int g_espGroundSpawnTextColor  = EQ_TEXT_COLOR_WHITE;
int g_espDoorTextColor         = EQ_TEXT_COLOR_WHITE;

bool g_buffsIsEnabled = true;

float g_buffsX = 768.0f;
float g_buffsY = 30.0f;

int g_buffsTextColor = EQ_TEXT_COLOR_YELLOW;

bool g_playerInfoIsEnabled = true;

float g_playerInfoX = 768.0f;
float g_playerInfoY = 256.0f;

int g_playerInfoTextColor = EQ_TEXT_COLOR_CYAN;

bool g_targetInfoIsEnabled = true;

float g_targetInfoX = 768.0f;
float g_targetInfoY = 384.0f;

int g_targetInfoTextColor = EQ_TEXT_COLOR_PINK;

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
    if (pointX < rectX) return false;
    if (pointY < rectY) return false;
    if (pointX > (rectX + rectWidth)) return false;
    if (pointY > (rectY + rectHeight)) return false;

    return true;
}

bool EQMACHUD_FileExists(const char* filename)
{
    FILE *file;
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
    char bufferResult[255];
    char bufferDefault[255];

    sprintf_s(bufferDefault, "%f", defaultValue);

    GetPrivateProfileStringA(section, key, bufferDefault, bufferResult, 255, filename);

    float result = (float)atof(bufferResult);

    return result;
}

const char* EQMACHUD_ConfigReadString(const char* filename, const char* section, const char* key, const char* defaultValue)
{
    static char result[255];
    GetPrivateProfileStringA(section, key, defaultValue, result, 255, filename);
    return result;
}

bool EQMACHUD_LoadConfig(const char* filename)
{
    if (EQMACHUD_FileExists(filename) == false)
    {
        return false;
    }

    // HUD

    g_mouseLeftClickDelay = EQMACHUD_ConfigReadInt(filename, "HUD", "iMouseLeftClickDelay", g_mouseLeftClickDelay);

    g_buttonExitX = EQMACHUD_ConfigReadFloat(filename, "HUD", "fButtonExitX", g_buttonExitX);
    g_buttonExitY = EQMACHUD_ConfigReadFloat(filename, "HUD", "fButtonExitY", g_buttonExitY);

    // Map

    g_mapIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bEnabled", g_mapIsEnabled);

    g_mapIsMinimized = EQMACHUD_ConfigReadBool(filename, "Map", "bIsMinimized", g_mapIsMinimized);
    g_mapIsMaximized = EQMACHUD_ConfigReadBool(filename, "Map", "bIsMaximized", g_mapIsMaximized);

    g_mapArrowIsEnabled        = EQMACHUD_ConfigReadBool(filename, "Map", "bArrow",        g_mapArrowIsEnabled);
    g_mapZoneInfoIsEnabled     = EQMACHUD_ConfigReadBool(filename, "Map", "bZoneInfo",     g_mapZoneInfoIsEnabled);
    g_mapHeightFilterIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bHeightFilter", g_mapHeightFilterIsEnabled);

    g_mapSpawnsIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bSpawns", g_mapSpawnsIsEnabled);
    g_mapPointsIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bPoints", g_mapPointsIsEnabled);
    g_mapLinesIsEnabled  = EQMACHUD_ConfigReadBool(filename, "Map", "bLines",  g_mapLinesIsEnabled);

    g_mapTargetLineIsEnabled         = EQMACHUD_ConfigReadBool(filename, "Map", "bTargetLine",         g_mapTargetLineIsEnabled);
    g_mapYourselfCorpseLineIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bYourselfCorpseLine", g_mapYourselfCorpseLineIsEnabled);

    g_mapReplaceBlackLinesWithWhiteLinesIsEnabled = EQMACHUD_ConfigReadBool(filename, "Map", "bReplaceBlackLinesWithWhiteLines", g_mapReplaceBlackLinesWithWhiteLinesIsEnabled);

    // TODO //

    g_mapDefaultX = EQMACHUD_ConfigReadFloat(filename, "Map", "fDefaultX", g_mapDefaultX);
    g_mapDefaultY = EQMACHUD_ConfigReadFloat(filename, "Map", "fDefaultY", g_mapDefaultY);

    g_mapDefaultWidth  = EQMACHUD_ConfigReadFloat(filename, "Map", "fDefaultWidth",  g_mapDefaultWidth);
    g_mapDefaultHeight = EQMACHUD_ConfigReadFloat(filename, "Map", "fDefaultHeight", g_mapDefaultHeight);

    g_mapMaximizedX = EQMACHUD_ConfigReadFloat(filename, "Map", "fMaximizedX", g_mapMaximizedX);
    g_mapMaximizedY = EQMACHUD_ConfigReadFloat(filename, "Map", "fMaximizedY", g_mapMaximizedY);

    g_mapMaximizedWidth  = EQMACHUD_ConfigReadFloat(filename, "Map", "fMaximizedWidth",  g_mapMaximizedWidth);
    g_mapMaximizedHeight = EQMACHUD_ConfigReadFloat(filename, "Map", "fMaximizedHeight", g_mapMaximizedHeight);

    // ESP

    g_espIsEnabled = EQMACHUD_ConfigReadBool(filename, "ESP", "bEnabled", g_espIsEnabled);

    // Buffs

    g_buffsIsEnabled = EQMACHUD_ConfigReadBool(filename, "Buffs", "bEnabled", g_buffsIsEnabled);

    // Player Info

    g_playerInfoIsEnabled = EQMACHUD_ConfigReadBool(filename, "Player Info", "bEnabled", g_playerInfoIsEnabled);

    // Target Info

    g_targetInfoIsEnabled = EQMACHUD_ConfigReadBool(filename, "Target Info", "bEnabled", g_targetInfoIsEnabled);

    return true;
}

struct _EQMAPLINE* EQMACHUD_MapLineList_Create(EQMAPLINE* mapLine)
{
    g_mapLineListHead = (struct _EQMAPLINE*)malloc(sizeof(struct _EQMAPLINE));

    if (g_mapLineListHead == NULL)
    {
        return NULL;
    }

    g_mapLineListHead->Next = NULL;

    g_mapLineListHead->X1    = mapLine->X1;
    g_mapLineListHead->Y1    = mapLine->Y1;
    g_mapLineListHead->Z1    = mapLine->Z1;
    g_mapLineListHead->X2    = mapLine->X2;
    g_mapLineListHead->Y2    = mapLine->Y2;
    g_mapLineListHead->Z2    = mapLine->Z2;
    g_mapLineListHead->R     = mapLine->R;
    g_mapLineListHead->G     = mapLine->G;
    g_mapLineListHead->B     = mapLine->B;
    g_mapLineListHead->Layer = mapLine->Layer;

    return g_mapLineListHead;
}

struct _EQMAPLINE* EQMACHUD_MapLineList_Add(EQMAPLINE* mapLine)
{
    if (g_mapLineListHead == NULL)
    {
        return (EQMACHUD_MapLineList_Create(mapLine));
    }

    struct _EQMAPLINE *current = g_mapLineListHead;

    if (current == NULL)
    {
        return NULL;
    }

    while (current->Next != NULL)
    {
        current = current->Next;
    }

    current->Next = (struct _EQMAPLINE*)malloc(sizeof(struct _EQMAPLINE));
    current->Next->Next = NULL;

    current->Next->X1    = mapLine->X1;
    current->Next->Y1    = mapLine->Y1;
    current->Next->Z1    = mapLine->Z1;
    current->Next->X2    = mapLine->X2;
    current->Next->Y2    = mapLine->Y2;
    current->Next->Z2    = mapLine->Z2;
    current->Next->R     = mapLine->R;
    current->Next->G     = mapLine->G;
    current->Next->B     = mapLine->B;
    current->Next->Layer = mapLine->Layer;

    return current;
}

void EQMACHUD_MapLineList_Destroy()
{
    struct _EQMAPLINE *current = g_mapLineListHead;

    if (current == NULL)
    {
        return;
    }

    struct _EQMAPLINE *next;

    while (current->Next != NULL)
    {
        next = current->Next;
        free(current);
        current = next;
    }

    g_mapLineListHead = NULL;
}

struct _EQMAPPOINT* EQMACHUD_MapPointList_Create(EQMAPPOINT* mapPoint)
{
    g_mapPointListHead = (struct _EQMAPPOINT*)malloc(sizeof(struct _EQMAPPOINT));

    if (g_mapPointListHead == NULL)
    {
        return NULL;
    }

    g_mapPointListHead->Next = NULL;

    g_mapPointListHead->X     = mapPoint->X;
    g_mapPointListHead->Y     = mapPoint->Y;
    g_mapPointListHead->Z     = mapPoint->Z;
    g_mapPointListHead->R     = mapPoint->R;
    g_mapPointListHead->G     = mapPoint->G;
    g_mapPointListHead->B     = mapPoint->B;
    g_mapPointListHead->Size  = mapPoint->Size;
    g_mapPointListHead->Layer = mapPoint->Layer;

    strcpy_s(g_mapPointListHead->Text, mapPoint->Text);

    return g_mapPointListHead;
}

struct _EQMAPPOINT* EQMACHUD_MapPointList_Add(EQMAPPOINT* mapPoint)
{
    if (g_mapPointListHead == NULL)
    {
        return (EQMACHUD_MapPointList_Create(mapPoint));
    }

    struct _EQMAPPOINT *current = g_mapPointListHead;

    if (current == NULL)
    {
        return NULL;
    }

    while (current->Next != NULL)
    {
        current = current->Next;
    }

    current->Next = (struct _EQMAPPOINT*)malloc(sizeof(struct _EQMAPPOINT));
    current->Next->Next = NULL;

    current->Next->X     = mapPoint->X;
    current->Next->Y     = mapPoint->Y;
    current->Next->Z     = mapPoint->Z;
    current->Next->R     = mapPoint->R;
    current->Next->G     = mapPoint->G;
    current->Next->B     = mapPoint->B;
    current->Next->Size  = mapPoint->Size;
    current->Next->Layer = mapPoint->Layer;

    strcpy_s(current->Next->Text, mapPoint->Text);

    return current;
}

void EQMACHUD_MapPointList_Destroy()
{
    struct _EQMAPPOINT *current = g_mapPointListHead;

    if (current == NULL)
    {
        return;
    }

    struct _EQMAPPOINT *next;

    while (current->Next != NULL)
    {
        next = current->Next;
        free(current);
        current = next;
    }

    g_mapPointListHead = NULL;
}

bool EQMACHUD_LoadMap(const char* filename)
{
    EQMACHUD_MapLineList_Destroy();
    EQMACHUD_MapPointList_Destroy();

    FILE* file;
    errno_t fileErrorNumber = fopen_s(&file, filename, "r");

    if (fileErrorNumber == 1)
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

            char lineType;

            int lineR;
            int lineG;
            int lineB;

            int result =
                sscanf
                (
                    line,
                    "%c %f, %f, %f, %f, %f, %f, %d, %d, %d",
                    &lineType,
                    &mapLine.X1, &mapLine.Y1, &mapLine.Z1,
                    &mapLine.X2, &mapLine.Y2, &mapLine.Z2,
                    &lineR,      &lineG,      &lineB
                );

            mapLine.R = lineR;
            mapLine.G = lineG;
            mapLine.B = lineB;

            if (result == 10)
            {
                EQMACHUD_MapLineList_Add(&mapLine);
            }
        }
        else if (line[0] == 'P')
        {
            EQMAPPOINT mapPoint;

            char lineType;

            int pointR;
            int pointG;
            int pointB;

            int pointSize;

            char pointText[128];

            int result =
                sscanf
                (
                    line,
                    "%c %f, %f, %f, %d, %d, %d, %d, %s",
                    &lineType,
                    &mapPoint.X, &mapPoint.Y, &mapPoint.Z,
                    &pointR,     &pointG,     &pointB,
                    &pointSize,
                    pointText
                );

            mapPoint.R = pointR;
            mapPoint.G = pointG;
            mapPoint.B = pointB;

            mapPoint.Size = pointSize;

            size_t i;
            for (i = 0; pointText[i] != '\0'; i++)
            {
                if (pointText[i] == '_')
                {
                    pointText[i] = ' ';
                }
            }

            strcpy_s(mapPoint.Text, pointText);

            if (result == 9)
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

    g_buttonToggleMapX = g_buttonExitX + g_buttonWidth + g_elementOffset;
    g_buttonToggleMapY = g_buttonExitY;

    buttonIndex += 1.0f;

    g_buttonToggleEspX = g_buttonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_buttonToggleEspY = g_buttonExitY;

    buttonIndex += 1.0f;

    g_buttonToggleEspDoorsX = g_buttonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_buttonToggleEspDoorsY = g_buttonExitY;

    buttonIndex += 1.0f;

    g_buttonToggleEspDistanceX = g_buttonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_buttonToggleEspDistanceY = g_buttonExitY;

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

    // map buttons

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

    g_mapButtonToggleHeightFilterX = g_mapMaxX + g_elementOffset;
    g_mapButtonToggleHeightFilterY = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

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

    g_mapButtonMinimizeX = g_mapMaxX - g_buttonWidth - g_elementOffset - g_buttonWidth;
    g_mapButtonMinimizeY = g_mapY - g_buttonHeight - g_elementOffset;

    g_mapButtonMaximizeX = g_mapMaxX - g_buttonWidth;
    g_mapButtonMaximizeY = g_mapY - g_buttonHeight - g_elementOffset;

    g_mapButtonRestoreX = g_mapX;
    g_mapButtonRestoreY = g_mapY;
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

void EQMACHUD_MapSetZoom(float zoom)
{
    g_mapZoom = zoom;
}

void EQMACHUD_MapToggleMinimized()
{
    EQ_ToggleBool(g_mapIsMinimized);
}

void EQMACHUD_MapToggleMaximized()
{
    EQ_ToggleBool(g_mapIsMaximized);

    EQMACHUD_GuiRecalculateCoordinates();
}

void EQMACHUD_MapToggleHeightFilter()
{
    EQ_ToggleBool(g_mapHeightFilterIsEnabled);
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

void EQMACHUD_ToggleEspShowDoor()
{
    EQ_ToggleBool(g_espShowDoorIsEnabled);
}

void EQMACHUD_ToggleEspDistance()
{
    EQ_ToggleBool(g_espDistanceIsEnabled);
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

void EQMACHUD_DoUpdateZone()
{
    DWORD zoneId = EQ_READ_MEMORY<DWORD>(EQ_ZONE_ID);

    EQZONEINFO* zoneInfo = (EQZONEINFO*)EQ_STRUCTURE_ZONE_INFO;

    if (g_zoneId != zoneId && zoneId != 0)
    {
        char mapFilename[128];
        sprintf_s(mapFilename, ".\\maps\\%s.txt", zoneInfo->ShortName);

        if (EQMACHUD_LoadMap(mapFilename) == true)
        {
            g_zoneId = zoneId;
        }
        else
        {
            EQMACHUD_MapLineList_Destroy();
            EQMACHUD_MapPointList_Destroy();
        }

        char zoneConfigFilename[128];
        sprintf_s(zoneConfigFilename, ".\\maps\\%s.ini", zoneInfo->ShortName);

        EQMACHUD_LoadConfig(zoneConfigFilename);

        EQMACHUD_GuiRecalculateCoordinates();
    }
}

void EQMACHUD_DoButtonToolTipTextAndMouseClick()
{
    DWORD fontArial14 = EQ_READ_MEMORY<DWORD>(EQ_POINTER_FONT_ARIAL14);

    WORD mouseX = EQ_READ_MEMORY<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_READ_MEMORY<WORD>(EQ_MOUSE_Y);

    DWORD mouseClickState = EQ_READ_MEMORY<DWORD>(EQ_MOUSE_CLICK_STATE);

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
        strcpy_s(buttonText, "Exit ");
        strcat_s(buttonText, g_applicationName);

        EQ_CLASS_CDisplay->WriteTextHD2
        (
            buttonText,
            (int)(g_buttonExitX),
            (int)(g_buttonExitY + g_buttonHeight + g_elementOffset),
            g_buttonTextColorMinMaxClose,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            g_bExit = 1;
        }
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
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Map",
            (int)(g_buttonToggleMapX),
            (int)(g_buttonToggleMapY + g_buttonHeight + g_elementOffset),
            g_buttonTextColorEnabled,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
            {
                g_mouseLeftClickTimer = currentTime;

                EQMACHUD_ToggleMap();

                if (g_mapIsEnabled == true)
                {
                    EQ_CLASS_CEverQuest->dsp_chat("-> Map enabled.");
                }
                else
                {
                    EQ_CLASS_CEverQuest->dsp_chat("-> Map disabled.");
                }
            }
        }
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
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "ESP",
            (int)(g_buttonToggleEspX),
            (int)(g_buttonToggleEspY + g_buttonHeight + g_elementOffset),
            g_buttonTextColorEnabled,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
            {
                g_mouseLeftClickTimer = currentTime;

                EQMACHUD_ToggleEsp();

                if (g_espIsEnabled == true)
                {
                    EQ_CLASS_CEverQuest->dsp_chat("-> ESP enabled.");
                }
                else
                {
                    EQ_CLASS_CEverQuest->dsp_chat("-> ESP disabled.");
                }
            }
        }
    }

    if
    (
        EQMACHUD_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_buttonToggleEspDoorsX, (int)g_buttonToggleEspDoorsY,
            (int)g_buttonWidth, (int)g_buttonHeight
        )
        == true
    )
    {
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "ESP Doors",
            (int)(g_buttonToggleEspDoorsX),
            (int)(g_buttonToggleEspDoorsY + g_buttonHeight + g_elementOffset),
            g_buttonTextColorEnabled,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
            {
                g_mouseLeftClickTimer = currentTime;

                EQMACHUD_ToggleEspShowDoor();

                if (g_espShowDoorIsEnabled == true)
                {
                    EQ_CLASS_CEverQuest->dsp_chat("-> ESP Doors enabled.");
                }
                else
                {
                    EQ_CLASS_CEverQuest->dsp_chat("-> ESP Doors disabled.");
                }
            }
        }
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
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "ESP Distance",
            (int)(g_buttonToggleEspDistanceX),
            (int)(g_buttonToggleEspDistanceY + g_buttonHeight + g_elementOffset),
            g_buttonTextColorEnabled,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
            {
                g_mouseLeftClickTimer = currentTime;

                EQMACHUD_ToggleEspDistance();

                if (g_espDistanceIsEnabled == true)
                {
                    EQ_CLASS_CEverQuest->dsp_chat("-> ESP Distance enabled.");
                }
                else
                {
                    EQ_CLASS_CEverQuest->dsp_chat("-> ESP Distance disabled.");
                }
            }
        }
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
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Buffs",
            (int)(g_buttonToggleBuffsX),
            (int)(g_buttonToggleBuffsY + g_buttonHeight + g_elementOffset),
            g_buttonTextColorEnabled,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
            {
                g_mouseLeftClickTimer = currentTime;

                EQMACHUD_ToggleBuffs();

                if (g_buffsIsEnabled == true)
                {
                    EQ_CLASS_CEverQuest->dsp_chat("-> Buffs enabled.");
                }
                else
                {
                    EQ_CLASS_CEverQuest->dsp_chat("-> Buffs disabled.");
                }
            }
        }
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
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Player Info",
            (int)(g_buttonTogglePlayerInfoX),
            (int)(g_buttonTogglePlayerInfoY + g_buttonHeight + g_elementOffset),
            g_buttonTextColorEnabled,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
            {
                g_mouseLeftClickTimer = currentTime;

                EQMACHUD_TogglePlayerInfo();

                if (g_playerInfoIsEnabled == true)
                {
                    EQ_CLASS_CEverQuest->dsp_chat("-> Player Info enabled.");
                }
                else
                {
                    EQ_CLASS_CEverQuest->dsp_chat("-> Player Info disabled.");
                }
            }
        }
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
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Target Info",
            (int)(g_buttonToggleTargetInfoX),
            (int)(g_buttonToggleTargetInfoY + g_buttonHeight + g_elementOffset),
            g_buttonTextColorEnabled,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
            {
                g_mouseLeftClickTimer = currentTime;

                EQMACHUD_ToggleTargetInfo();

                if (g_targetInfoIsEnabled == true)
                {
                    EQ_CLASS_CEverQuest->dsp_chat("-> Target Info enabled.");
                }
                else
                {
                    EQ_CLASS_CEverQuest->dsp_chat("-> Target Info disabled.");
                }
            }
        }
    }

    if (g_mapIsMinimized == false)
    {
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
            EQ_CLASS_CDisplay->WriteTextHD2
            (
                "Zoom In",
                (int)(g_mapButtonZoomInX + g_buttonWidth + g_elementOffset),
                (int)(g_mapButtonZoomInY - 2.0f),
                g_buttonTextColorEnabled,
                fontArial14
            );

            if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
            {
                EQMACHUD_MapZoomIn();
            }
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
            EQ_CLASS_CDisplay->WriteTextHD2
            (
                "Zoom Out",
                (int)(g_mapButtonZoomOutX + g_buttonWidth + g_elementOffset),
                (int)(g_mapButtonZoomOutY - 2.0f),
                g_buttonTextColorEnabled,
                fontArial14
            );

            if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
            {
                EQMACHUD_MapZoomOut();
            }
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
            EQ_CLASS_CDisplay->WriteTextHD2
            (
                "Zoom 1x",
                (int)(g_mapButtonZoom1X + g_buttonWidth + g_elementOffset),
                (int)(g_mapButtonZoom1Y - 2.0f),
                g_buttonTextColorEnabled,
                fontArial14
            );

            if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
            {
                EQMACHUD_MapSetZoom(1.0f);
            }
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
            EQ_CLASS_CDisplay->WriteTextHD2
            (
                "Height Filter",
                (int)(g_mapButtonToggleHeightFilterX + g_buttonWidth + g_elementOffset),
                (int)(g_mapButtonToggleHeightFilterY - 2.0f),
                g_buttonTextColorEnabled,
                fontArial14
            );

            if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
            {
                DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

                if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
                {
                    g_mouseLeftClickTimer = currentTime;

                    EQMACHUD_MapToggleHeightFilter();

                    if (g_mapHeightFilterIsEnabled == true)
                    {
                        EQ_CLASS_CEverQuest->dsp_chat("-> Map Height Filter enabled.");
                    }
                    else
                    {
                        EQ_CLASS_CEverQuest->dsp_chat("-> Map Height Filter disabled.");
                    }
                }
            }
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
            EQ_CLASS_CDisplay->WriteTextHD2
            (
                "Line Color",
                (int)(g_mapButtonToggleLineColorX + g_buttonWidth + g_elementOffset),
                (int)(g_mapButtonToggleLineColorY - 2.0f),
                g_buttonTextColorEnabled,
                fontArial14
            );

            if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
            {
                DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

                if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
                {
                    g_mouseLeftClickTimer = currentTime;

                    EQMACHUD_MapToggleLineColor();
                }
            }
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
            EQ_CLASS_CDisplay->WriteTextHD2
            (
                "Zone Info",
                (int)(g_mapButtonToggleZoneInfoX + g_buttonWidth + g_elementOffset),
                (int)(g_mapButtonToggleZoneInfoY - 2.0f),
                g_buttonTextColorEnabled,
                fontArial14
            );

            if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
            {
                DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

                if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
                {
                    g_mouseLeftClickTimer = currentTime;

                    EQMACHUD_MapToggleZoneInfo();

                    if (g_mapZoneInfoIsEnabled == true)
                    {
                        EQ_CLASS_CEverQuest->dsp_chat("-> Map Zone Info enabled.");
                    }
                    else
                    {
                        EQ_CLASS_CEverQuest->dsp_chat("-> Map Zone Info disabled.");
                    }
                }
            }
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
            EQ_CLASS_CDisplay->WriteTextHD2
            (
                "Spawns",
                (int)(g_mapButtonToggleSpawnsX + g_buttonWidth + g_elementOffset),
                (int)(g_mapButtonToggleSpawnsY - 2.0f),
                g_buttonTextColorEnabled,
                fontArial14
            );

            if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
            {
                DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

                if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
                {
                    g_mouseLeftClickTimer = currentTime;

                    EQMACHUD_MapToggleSpawns();

                    if (g_mapSpawnsIsEnabled == true)
                    {
                        EQ_CLASS_CEverQuest->dsp_chat("-> Map Spawns enabled.");
                    }
                    else
                    {
                        EQ_CLASS_CEverQuest->dsp_chat("-> Map Spawns disabled.");
                    }
                }
            }
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
            EQ_CLASS_CDisplay->WriteTextHD2
            (
                "Points (Labels)",
                (int)(g_mapButtonTogglePointsX + g_buttonWidth + g_elementOffset),
                (int)(g_mapButtonTogglePointsY - 2.0f),
                g_buttonTextColorEnabled,
                fontArial14
            );

            if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
            {
                DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

                if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
                {
                    g_mouseLeftClickTimer = currentTime;

                    EQMACHUD_MapTogglePoints();

                    if (g_mapPointsIsEnabled == true)
                    {
                        EQ_CLASS_CEverQuest->dsp_chat("-> Map Points enabled.");
                    }
                    else
                    {
                        EQ_CLASS_CEverQuest->dsp_chat("-> Map Points disabled.");
                    }
                }
            }
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
            EQ_CLASS_CDisplay->WriteTextHD2
            (
                "Lines",
                (int)(g_mapButtonToggleLinesX + g_buttonWidth + g_elementOffset),
                (int)(g_mapButtonToggleLinesY - 2.0f),
                g_buttonTextColorEnabled,
                fontArial14
            );

            if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
            {
                DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

                if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
                {
                    g_mouseLeftClickTimer = currentTime;

                    EQMACHUD_MapToggleLines();

                    if (g_mapLinesIsEnabled == true)
                    {
                        EQ_CLASS_CEverQuest->dsp_chat("-> Map Lines enabled.");
                    }
                    else
                    {
                        EQ_CLASS_CEverQuest->dsp_chat("-> Map Lines disabled.");
                    }
                }
            }
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
            EQ_CLASS_CDisplay->WriteTextHD2
            (
                "Spawn Distance",
                (int)(g_mapButtonToggleSpawnDistanceX + g_buttonWidth + g_elementOffset),
                (int)(g_mapButtonToggleSpawnDistanceY - 2.0f),
                g_buttonTextColorEnabled,
                fontArial14
            );

            if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
            {
                DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

                if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
                {
                    g_mouseLeftClickTimer = currentTime;

                    EQMACHUD_MapToggleSpawnDistance();

                    if (g_mapSpawnDistanceIsEnabled == true)
                    {
                        EQ_CLASS_CEverQuest->dsp_chat("-> Map Spawn Distance enabled.");
                    }
                    else
                    {
                        EQ_CLASS_CEverQuest->dsp_chat("-> Map Spawn Distance disabled.");
                    }
                }
            }
        }

        if
        (
            EQMACHUD_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonMinimizeX, (int)g_mapButtonMinimizeY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_CLASS_CDisplay->WriteTextHD2
            (
                "Minimize",
                (int)(g_mapButtonMinimizeX),
                (int)(g_mapButtonMinimizeY - g_elementOffset) - g_fontHeight,
                g_buttonTextColorMinMaxClose,
                fontArial14
            );

            if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
            {
                DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

                if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
                {
                    g_mouseLeftClickTimer = currentTime;

                    EQMACHUD_MapToggleMinimized();
                }
            }
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

            EQ_CLASS_CDisplay->WriteTextHD2
            (
                buttonText,
                (int)(g_mapButtonMaximizeX),
                (int)(g_mapButtonMaximizeY - g_elementOffset) - g_fontHeight,
                g_buttonTextColorMinMaxClose,
                fontArial14
            );

            if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
            {
                DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

                if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
                {
                    g_mouseLeftClickTimer = currentTime;

                    EQMACHUD_MapToggleMaximized();
                }
            }
        }
    } //if (g_mapIsMinimized == false)
    else
    {
        if
        (
            EQMACHUD_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonRestoreX, (int)g_mapButtonRestoreY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_CLASS_CDisplay->WriteTextHD2
            (
                "Restore",
                (int)(g_mapButtonRestoreX + g_buttonWidth + g_elementOffset),
                (int)(g_mapButtonRestoreY - 2.0f),
                g_buttonTextColorMinMaxClose,
                fontArial14
            );

            if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
            {
                DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

                if ((currentTime - g_mouseLeftClickTimer) > g_mouseLeftClickDelay)
                {
                    g_mouseLeftClickTimer = currentTime;

                    EQMACHUD_MapToggleMinimized();
                }
            }
        }
    }
}

void EQMACHUD_DoExitRowButtons()
{
    DWORD fontArial14 = EQ_READ_MEMORY<DWORD>(EQ_POINTER_FONT_ARIAL14);

    int buttonColor = g_buttonColorMinMaxClose;
    int buttonTextColor = g_buttonTextColorMinMaxClose;

    EQ_DrawRectangle(g_buttonExitX, g_buttonExitY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("x", (int)(g_buttonExitX + 3.0f), (int)(g_buttonExitY - 2.0f), buttonTextColor, fontArial14);

    g_numLinesDeferred += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleMapX, g_buttonToggleMapY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("m", (int)(g_buttonToggleMapX + 2.0f), (int)(g_buttonToggleMapY - 2.0f), buttonTextColor, fontArial14);

    g_numLinesDeferred += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleEspX, g_buttonToggleEspY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("e", (int)(g_buttonToggleEspX + 3.0f), (int)(g_buttonToggleEspY - 2.0f), buttonTextColor, fontArial14);

    g_numLinesDeferred += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espDistanceIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleEspDistanceX, g_buttonToggleEspDistanceY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("d", (int)(g_buttonToggleEspDistanceX + 3.0f), (int)(g_buttonToggleEspDistanceY - 2.0f), buttonTextColor, fontArial14);

    g_numLinesDeferred += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espShowDoorIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleEspDoorsX, g_buttonToggleEspDoorsY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("o", (int)(g_buttonToggleEspDoorsX + 3.0f), (int)(g_buttonToggleEspDoorsY - 2.0f), buttonTextColor, fontArial14);

    g_numLinesDeferred += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_buffsIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleBuffsX, g_buttonToggleBuffsY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("b", (int)(g_buttonToggleBuffsX + 3.0f), (int)(g_buttonToggleBuffsY - 2.0f), buttonTextColor, fontArial14);

    g_numLinesDeferred += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_playerInfoIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonTogglePlayerInfoX, g_buttonTogglePlayerInfoY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("p", (int)(g_buttonTogglePlayerInfoX + 3.0f), (int)(g_buttonTogglePlayerInfoY - 3.0f), buttonTextColor, fontArial14);

    g_numLinesDeferred += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_targetInfoIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_buttonToggleTargetInfoX, g_buttonToggleTargetInfoY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("t", (int)(g_buttonToggleTargetInfoX + 5.0f), (int)(g_buttonToggleTargetInfoY - 2.0f), buttonTextColor, fontArial14);

    g_numLinesDeferred += 4;
}

void EQMACHUD_DoMap()
{
    DWORD fontArial14 = EQ_READ_MEMORY<DWORD>(EQ_POINTER_FONT_ARIAL14);

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
        (int)(g_mapY - g_elementOffset) - g_fontHeight,
        g_mapDefaultTextColor,
        fontArial14
    );

    if (g_mapIsMinimized == true)
    {
        int buttonColor = g_buttonColorMinMaxClose;
        int buttonTextColor = g_buttonTextColorMinMaxClose;

        EQ_DrawRectangle(g_mapButtonRestoreX, g_mapButtonRestoreY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("+", (int)(g_mapButtonRestoreX + 3.0f), (int)(g_mapButtonRestoreY - 2.0f), buttonTextColor, fontArial14);

        g_numLinesDeferred += 4;
    }
    else
    {
        EQ_DrawRectangle(g_mapX, g_mapY, g_mapWidth, g_mapHeight, g_mapBorderColor);

        g_numLinesDeferred += 4;

        int buttonColor = g_buttonColorEnabled;
        int buttonTextColor = g_buttonTextColorEnabled;

        EQ_DrawRectangle(g_mapButtonZoomInX, g_mapButtonZoomInY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("+", (int)(g_mapButtonZoomInX + 3.0f), (int)(g_mapButtonZoomInY - 2.0f), buttonTextColor, fontArial14);

        g_numLinesDeferred += 4;

        EQ_DrawRectangle(g_mapButtonZoomOutX, g_mapButtonZoomOutY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("-", (int)(g_mapButtonZoomOutX + 4.0f), (int)(g_mapButtonZoomOutY - 3.0f), buttonTextColor, fontArial14);

        g_numLinesDeferred += 4;

        EQ_DrawRectangle(g_mapButtonZoom1X, g_mapButtonZoom1Y, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("1", (int)(g_mapButtonZoom1X + 4.0f), (int)(g_mapButtonZoom1Y - 2.0f), buttonTextColor, fontArial14);

        g_numLinesDeferred += 4;

        EQ_DrawRectangle(g_mapButtonToggleLineColorX, g_mapButtonToggleLineColorY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("c", (int)(g_mapButtonToggleLineColorX + 3.0f), (int)(g_mapButtonToggleLineColorY - 2.0f), buttonTextColor, fontArial14);

        g_numLinesDeferred += 4;

        buttonColor = g_buttonColorEnabled;
        buttonTextColor = g_buttonTextColorEnabled;

        if (g_mapHeightFilterIsEnabled == false)
        {
            buttonColor = g_buttonColorDisabled;
            buttonTextColor = g_buttonTextColorDisabled;
        }

        EQ_DrawRectangle(g_mapButtonToggleHeightFilterX, g_mapButtonToggleHeightFilterY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("h", (int)(g_mapButtonToggleHeightFilterX + 3.0f), (int)(g_mapButtonToggleHeightFilterY - 2.0f), buttonTextColor, fontArial14);

        g_numLinesDeferred += 4;

        buttonColor = g_buttonColorEnabled;
        buttonTextColor = g_buttonTextColorEnabled;

        if (g_mapZoneInfoIsEnabled == false)
        {
            buttonColor = g_buttonColorDisabled;
            buttonTextColor = g_buttonTextColorDisabled;
        }

        EQ_DrawRectangle(g_mapButtonToggleZoneInfoX, g_mapButtonToggleZoneInfoY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("z", (int)(g_mapButtonToggleZoneInfoX + 3.0f), (int)(g_mapButtonToggleZoneInfoY - 2.0f), buttonTextColor, fontArial14);

        g_numLinesDeferred += 4;

        buttonColor = g_buttonColorEnabled;
        buttonTextColor = g_buttonTextColorEnabled;

        if (g_mapSpawnsIsEnabled == false)
        {
            buttonColor = g_buttonColorDisabled;
            buttonTextColor = g_buttonTextColorDisabled;
        }

        EQ_DrawRectangle(g_mapButtonToggleSpawnsX, g_mapButtonToggleSpawnsY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("s", (int)(g_mapButtonToggleSpawnsX + 3.0f), (int)(g_mapButtonToggleSpawnsY - 2.0f), buttonTextColor, fontArial14);

        g_numLinesDeferred += 4;

        buttonColor = g_buttonColorEnabled;
        buttonTextColor = g_buttonTextColorEnabled;

        if (g_mapPointsIsEnabled == false)
        {
            buttonColor = g_buttonColorDisabled;
            buttonTextColor = g_buttonTextColorDisabled;
        }

        EQ_DrawRectangle(g_mapButtonTogglePointsX, g_mapButtonTogglePointsY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("p", (int)(g_mapButtonTogglePointsX + 3.0f), (int)(g_mapButtonTogglePointsY - 3.0f), buttonTextColor, fontArial14);

        g_numLinesDeferred += 4;

        buttonColor = g_buttonColorEnabled;
        buttonTextColor = g_buttonTextColorEnabled;

        if (g_mapLinesIsEnabled == false)
        {
            buttonColor = g_buttonColorDisabled;
            buttonTextColor = g_buttonTextColorDisabled;
        }

        EQ_DrawRectangle(g_mapButtonToggleLinesX, g_mapButtonToggleLinesY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("l", (int)(g_mapButtonToggleLinesX + 5.0f), (int)(g_mapButtonToggleLinesY - 2.0f), buttonTextColor, fontArial14);

        g_numLinesDeferred += 4;

        buttonColor = g_buttonColorEnabled;
        buttonTextColor = g_buttonTextColorEnabled;

        if (g_mapSpawnDistanceIsEnabled == false)
        {
            buttonColor = g_buttonColorDisabled;
            buttonTextColor = g_buttonTextColorDisabled;
        }

        EQ_DrawRectangle(g_mapButtonToggleSpawnDistanceX, g_mapButtonToggleSpawnDistanceY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("d", (int)(g_mapButtonToggleSpawnDistanceX + 3.0f), (int)(g_mapButtonToggleSpawnDistanceY - 2.0f), buttonTextColor, fontArial14);

        g_numLinesDeferred += 4;

        buttonColor = g_buttonColorMinMaxClose;
        buttonTextColor = g_buttonTextColorMinMaxClose;

        EQ_DrawRectangle(g_mapButtonMinimizeX, g_mapButtonMinimizeY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("-", (int)(g_mapButtonMinimizeX + 4.0f), (int)(g_mapButtonMinimizeY - 3.0f), buttonTextColor, fontArial14);

        g_numLinesDeferred += 4;

        EQ_DrawRectangle(g_mapButtonMaximizeX, g_mapButtonMaximizeY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("+", (int)(g_mapButtonMaximizeX + 3.0f), (int)(g_mapButtonMaximizeY - 2.0f), buttonTextColor, fontArial14);

        g_numLinesDeferred += 4;

        if (g_mapLinesIsEnabled == true)
        {
            struct _EQMAPLINE *mapLine = g_mapLineListHead;

            while (mapLine)
            {
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

                // only use 75% of the maximum allowed number of deferred objects
                // the game needs to reserve some to draw stuff
                // this also leaves some extra to draw the player arrow, target line, etc
                if (g_numLinesDeferred > (int)(EQ_GRAPHICS_DLL_DEFERRED_OBJECTS_MAX * 0.75f))
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

                    g_numLinesDeferred += 1;
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

                bool bDrawLine = EQMACHUD_DoClipLine(&line, g_mapMinX, g_mapMinY, g_mapMaxX, g_mapMaxY);

                if (bDrawLine == true)
                {
                    EQGfx_Dx8__t3dDeferLine(&line, g_mapTargetLineColor);

                    g_numLinesDeferred += 1;
                }
            }
        }

        if (g_mapPointsIsEnabled == true && g_mapIsMaximized == true)
        {
            struct _EQMAPPOINT *mapPoint = g_mapPointListHead;

            while (mapPoint)
            {
                int pointX = (int)(((mapPoint->X * g_mapZoom) + g_mapOriginX) + (playerSpawn->X * g_mapZoom));
                int pointY = (int)(((mapPoint->Y * g_mapZoom) + g_mapOriginY) + (playerSpawn->Y * g_mapZoom));

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

                EQ_CLASS_CDisplay->WriteTextHD2(pointText, pointX, pointY, g_mapPointTextColor, fontArial14);

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
                    bool applyDistance = false;

                    if (g_mapSpawnDistancePlayerIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_PLAYER)
                    {
                        applyDistance = true;
                    }

                    if (g_mapSpawnDistancePlayerCorpseIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_PLAYER_CORPSE)
                    {
                        applyDistance = true;
                    }

                    if (g_mapSpawnDistanceNpcIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_NPC)
                    {
                        applyDistance = true;
                    }

                    if (g_mapSpawnDistanceNpcCorpseIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_NPC_CORPSE)
                    {
                        applyDistance = true;
                    }

                    // target is always visible
                    if (spawn == targetSpawn)
                    {
                        applyDistance = false;
                    }

                    if (applyDistance == true && spawnDistance > g_mapSpawnDistance)
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

                int mapSpawnX = (int)(((-spawn->X * g_mapZoom) + g_mapOriginX) + (playerSpawn->X * g_mapZoom));
                int mapSpawnY = (int)(((-spawn->Y * g_mapZoom) + g_mapOriginY) + (playerSpawn->Y * g_mapZoom));

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

                if (g_mapIsMaximized == false)
                {
                    EQ_CLASS_CDisplay->WriteTextHD2("*", mapSpawnX, mapSpawnY, textColor, fontArial14);
                }
                else
                {
                    const char* spawnName = EQ_CLASS_CEverQuest->trimName(spawn->Name);

                    char spawnText[128];
                    sprintf_s(spawnText, "+ %s", spawnName);

                    EQ_CLASS_CDisplay->WriteTextHD2(spawnText, mapSpawnX, mapSpawnY, textColor, fontArial14);
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

                        bool bDrawLine = EQMACHUD_DoClipLine(&line, g_mapMinX, g_mapMinY, g_mapMaxX, g_mapMaxY);

                        if (bDrawLine == true)
                        {
                            EQGfx_Dx8__t3dDeferLine(&line, g_mapCorpseLineColor);

                            g_numLinesDeferred += 1;
                        }
                    }

                    spawn = spawn->Next;
                }
            }
        } // if (g_mapSpawnsIsEnabled == true)

        if (g_mapArrowIsEnabled == true)
        {
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

            float playerHeadingC = playerSpawn->Heading;

            playerHeadingC = playerHeadingC - 128.0f;

            if (playerHeadingC < 0.0f)
            {
                playerHeadingC = playerHeadingC + 512.0f;
            }

            float playerHeadingRadiansC = playerHeadingC * g_mathPi / 256.0f;

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

            float playerHeadingRadiansB = playerHeadingB * g_mathPi / 256.0f;

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

            float playerHeadingRadiansA = playerHeadingA * 3.14159265358979f / 256.0f;

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
            char zoneText[128];
            sprintf_s(zoneText, "Zone: %s (ID: %d)", zoneInfo->ShortName, zoneId);
            EQ_CLASS_CDisplay->WriteTextHD2(zoneText, (int)g_mapX, (int)(g_mapMaxY + g_elementOffset), g_mapZoneInfoTextColor, fontArial14);

            if (g_mapIsMaximized == true)
            {
                char locationText[128];
                sprintf_s(locationText, "Location: %.2f, %.2f, %.2f", playerLocation.Y, playerLocation.X, playerLocation.Z);
                EQ_CLASS_CDisplay->WriteTextHD2(locationText, (int)g_mapX, (int)(g_mapMaxY + g_elementOffset + g_elementOffset) + g_fontHeight, g_mapZoneInfoTextColor, fontArial14);
            }
        }
    }
}

void EQMACHUD_DoEsp()
{
    DWORD fontArial14 = EQ_READ_MEMORY<DWORD>(EQ_POINTER_FONT_ARIAL14);

    DWORD worldSpaceToScreenSpaceCameraData = EQ_READ_MEMORY<DWORD>(EQ_POINTER_T3D_WORLD_SPACE_TO_SCREEN_SPACE_CAMERA_DATA);

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
            if (g_espDistanceDoorIsEnabled == true && spawnDistance > g_espDistance)
            {
                doorSpawn = doorSpawn->Next;
                continue;
            }
        }

        float resultX = 0.0f;
        float resultY = 0.0f;
        int result = EQGfx_Dx8__t3dWorldSpaceToScreenSpace(worldSpaceToScreenSpaceCameraData, &spawnLocation, &resultX, &resultY);

        if (result != EQ_T3D_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
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
                if (g_espShowUniqueDoorIsEnabled == true)
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
                if (g_espShowDoorIsEnabled == true)
                {
                    sprintf_s(spawnText, "+ Door: %s", spawnName);
                }
                else
                {
                    doorSpawn = doorSpawn->Next;
                    continue;
                }
            }

            char distanceText[32];
            sprintf_s(distanceText, " (%d)", (int)spawnDistance);
            strcat_s(spawnText, distanceText);

            EQ_CLASS_CDisplay->WriteTextHD2(spawnText, screenX, screenY, g_espDoorTextColor, fontArial14);
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

        if (result != EQ_T3D_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
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

            char distanceText[32];
            sprintf_s(distanceText, " (%d)", (int)spawnDistance);
            strcat_s(spawnText, distanceText);

            EQ_CLASS_CDisplay->WriteTextHD2(spawnText, screenX, screenY, g_espGroundSpawnTextColor, fontArial14);
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
            bool applyDistance = false;

            if (g_espDistancePlayerIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_PLAYER)
            {
                applyDistance = true;
            }

            if (g_espDistancePlayerCorpseIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_PLAYER_CORPSE)
            {
                applyDistance = true;
            }

            if (g_espDistanceNpcIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_NPC)
            {
                applyDistance = true;
            }

            if (g_espDistanceNpcCorpseIsEnabled == true && spawn->Type == EQ_SPAWN_TYPE_NPC_CORPSE)
            {
                applyDistance = true;
            }

            // target is always visible
            if (spawn == targetSpawn)
            {
                applyDistance = false;
            }

            if (applyDistance == true && spawnDistance > g_espDistance)
            {
                spawn = spawn->Next;
                continue;
            }
        }

        float resultX = 0.0f;
        float resultY = 0.0f;
        int result = EQGfx_Dx8__t3dWorldSpaceToScreenSpace(worldSpaceToScreenSpaceCameraData, &spawnLocation, &resultX, &resultY);

        if (result != EQ_T3D_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
        {
            int screenX = (int)resultX;
            int screenY = (int)resultY;

            if (g_espShowYourselfIsEnabled == true && spawn == playerSpawn)
            {
                int playerTextY = screenY;

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

                        playerTextY = playerTextY + g_fontHeight + 1;
                    }
                }

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

                        playerTextY = playerTextY + g_fontHeight + 1;
                    }
                }

                spawn = spawn->Next;
                continue;
            }

            const char* spawnName = EQ_CLASS_CEverQuest->trimName(spawn->Name);

            char spawnText[128];
            sprintf_s(spawnText, "+ %s", spawnName);

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

            if (spawn->IsGameMaster == 1)
            {
                textColor = g_espGameMasterTextColor;

                strcat_s(spawnText, " GM");
            }

            if (spawn->IsPlayerKill == 1)
            {
                strcat_s(spawnText, " PVP");
            }

            if (spawn->IsAwayFromKeyboard == 1)
            {
                strcat_s(spawnText, " AFK");
            }

            if (spawn->Type == EQ_SPAWN_TYPE_PLAYER)
            {
                if (spawn->IsLinkDead == 1)
                {
                    strcat_s(spawnText, " LD");
                }

                if (spawn->StandingState == EQ_STANDING_STATE_FEIGNED)
                {
                    strcat_s(spawnText, " FD");
                }
            }

            char levelText[32];
            sprintf_s(levelText, " L%d", spawn->Level);
            strcat_s(spawnText, levelText);

            if (spawn->Type == EQ_SPAWN_TYPE_PLAYER)
            {
                char classText[32];
                sprintf_s(classText, " %s", EQ_GetClassShortName(spawn->Class));
                strcat_s(spawnText, classText);
            }

            char distanceText[32];
            sprintf_s(distanceText, " (%d)", (int)spawnDistance);
            strcat_s(spawnText, distanceText);

            EQ_CLASS_CDisplay->WriteTextHD2(spawnText, screenX, screenY, textColor, fontArial14);

            if (spawn->Type == EQ_SPAWN_TYPE_PLAYER)
            {
                if (spawn->GuildId != EQ_GUILD_ID_NULL)
                {
                    screenY = screenY + g_fontHeight + 1;

                    char guildText[128];
                    sprintf_s(guildText, "<%s>", EQ_GetGuildNameById(spawn->GuildId));

                    EQ_CLASS_CDisplay->WriteTextHD2(guildText, screenX, screenY, textColor, fontArial14);
                }

                if (spawn->HpCurrent < 100)
                {
                    screenY = screenY + g_fontHeight + 1;

                    char hpText[128];
                    sprintf_s(hpText, "HP: %d%%", spawn->HpCurrent);

                    EQ_CLASS_CDisplay->WriteTextHD2(hpText, screenX, screenY, textColor, fontArial14);
                }
            }
            else if (spawn->Type == EQ_SPAWN_TYPE_NPC)
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
                    screenY = screenY + g_fontHeight + 1;

                    char classText[128];
                    sprintf_s(classText, "(%s)", EQ_STRING_CLASS_NAME[spawn->Class]);

                    EQ_CLASS_CDisplay->WriteTextHD2(classText, screenX, screenY, textColor, fontArial14);
                }

                if (spawn->HpCurrent < 100)
                {
                    screenY = screenY + g_fontHeight + 1;

                    char hpText[128];
                    sprintf_s(hpText, "HP: %d%%", spawn->HpCurrent);

                    EQ_CLASS_CDisplay->WriteTextHD2(hpText, screenX, screenY, textColor, fontArial14);
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

    int buffY = (int)(g_buffsY) + g_fontHeight + 1;

    for (size_t i = 0; i < EQ_BUFFS_MAX; i++)
    {
        int spellId = charInfo->Buffs[i].SpellId;

        if (spellId == EQ_SPELL_ID_NULL)
        {
            continue;
        }

        const char* spellName = EQ_OBJECT_SpellList->Spell[spellId]->Name;

        int buffTicks = charInfo->Buffs[i].Ticks;

        int buffSeconds = buffTicks * 3;

        int buffHours = buffSeconds / (60 * 60);

        buffSeconds = buffSeconds - buffHours * (60 * 60);

        int buffMinutes = buffSeconds / 60;

        buffSeconds = buffSeconds - buffMinutes * 60;

        char buffText[128];
        sprintf_s(buffText, "%02d: (%02d:%02d:%02d) %s", i + 1, buffHours, buffMinutes, buffSeconds, spellName);

        EQ_CLASS_CDisplay->WriteTextHD2(buffText, (int)g_buffsX, (int)buffY, g_buffsTextColor, fontArial14);

        buffY = buffY + g_fontHeight + 1;
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

    int playerTextY = (int)(g_playerInfoY) + g_fontHeight + 1;

    int hpCurrent = playerSpawn->HpCurrent;
    int hpMax = playerSpawn->HpMax;

    if (hpCurrent > 0 && hpMax > 0)
    {
        int hpPercent = (hpCurrent * 100) / hpMax;

        char hpText[128];
        sprintf_s(hpText, "HP: %d/%d (%d%%)", hpCurrent, hpMax, hpPercent);

        EQ_CLASS_CDisplay->WriteTextHD2(hpText, (int)g_playerInfoX, (int)playerTextY, g_playerInfoTextColor, fontArial14);

        playerTextY = playerTextY + g_fontHeight + 1;
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

            playerTextY = playerTextY + g_fontHeight + 1;
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

    int targetTextY = (int)(g_targetInfoY) + g_fontHeight + 1;

    const char* raceShortName = EQ_GetRaceShortName(targetSpawn->Race);

    const char* classShortName = EQ_GetClassShortName(targetSpawn->Class);

    char levelRaceClassText[128];
    sprintf_s(levelRaceClassText, "L%d %s %s", targetSpawn->Level, raceShortName, classShortName);

    EQ_CLASS_CDisplay->WriteTextHD2(levelRaceClassText, (int)g_targetInfoX, (int)targetTextY, g_targetInfoTextColor, fontArial14);

    targetTextY = targetTextY + g_fontHeight + 1;

    if (targetSpawn->GuildId != EQ_GUILD_ID_NULL)
    {
        char guildText[128];
        sprintf_s(guildText, "<%s>", EQ_GetGuildNameById(targetSpawn->GuildId));

        EQ_CLASS_CDisplay->WriteTextHD2(guildText, (int)g_targetInfoX, (int)targetTextY, g_targetInfoTextColor, fontArial14);

        targetTextY = targetTextY + g_fontHeight + 1;
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

        targetTextY = targetTextY + g_fontHeight + 1;
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

            targetTextY = targetTextY + g_fontHeight + 1;
        }
    }
}

int __cdecl EQMACHUD_DrawNetStatus_DETOUR(int a1, unsigned short a2, unsigned short a3, unsigned short a4, unsigned short a5, int a6, unsigned short a7, unsigned long a8, long a9, unsigned long a10)
{
    g_numLinesDeferred = 0;

    EQMACHUD_DoUpdateZone();

    EQMACHUD_DoButtonToolTipTextAndMouseClick();

    if (g_espIsEnabled == true && (g_mapIsMaximized == false || g_mapIsMinimized == true))
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

    EQMACHUD_DoExitRowButtons();

    return EQMACHUD_DrawNetStatus_REAL(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

DWORD WINAPI EQMACHUD_ThreadLoop(LPVOID param)
{
    while (g_bExit == 0)
    {
        Sleep(1000);
    }

    DetourRemove((PBYTE)EQMACHUD_DrawNetStatus_REAL, (PBYTE)EQMACHUD_DrawNetStatus_DETOUR);

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

    EQGfx_Dx8__t3dWorldSpaceToScreenSpace = (EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dWorldSpaceToScreenSpace)GetProcAddress(graphicsDll, EQ_T3D_WORLD_SPACE_TO_SCREEN_SPACE_FUNCTION_NAME);
    if (!EQGfx_Dx8__t3dWorldSpaceToScreenSpace)
    {
        MessageBoxA(NULL, "Error: World to screen function not found!", g_applicationName, MB_ICONERROR);

        FreeLibraryAndExitThread(g_module, 0);
        return 0;
    }

    EQGfx_Dx8__t3dDeferLine = (EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dDeferLine)GetProcAddress(graphicsDll, EQ_T3D_DEFER_LINE_FUNCTION_NAME);
    if (!EQGfx_Dx8__t3dDeferLine)
    {
        MessageBoxA(NULL, "Error: Draw line function not found!", g_applicationName, MB_ICONERROR);

        FreeLibraryAndExitThread(g_module, 0);
        return 0;
    }

    char configFilename[128];
    sprintf_s(configFilename, ".\\%s", g_applicationConfigFilename);

    EQMACHUD_LoadConfig(configFilename);

    EQMACHUD_GuiRecalculateCoordinates();

    g_handleThreadLoop = CreateThread(NULL, 0, &EQMACHUD_ThreadLoop, NULL, 0, NULL);

    EQMACHUD_DrawNetStatus_REAL = (EQ_FUNCTION_TYPE_DrawNetStatus)DetourFunction((PBYTE)EQ_FUNCTION_DrawNetStatus, (PBYTE)EQMACHUD_DrawNetStatus_DETOUR);

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
