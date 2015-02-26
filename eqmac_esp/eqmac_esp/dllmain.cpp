#include <cstdint>
#include <cstring>
#include <cmath>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <windows.h>

#include "detours.h"
#pragma comment(lib, "detours.lib")

#include "eqmac.hpp"
#include "eqmac_functions.hpp"

volatile int g_bExit = 0;

HMODULE g_module;

HANDLE g_handleThreadLoad;
HANDLE g_handleThreadLoop;

EQ_FUNCTION_TYPE_DrawNetStatus EQMACESP_DrawNetStatus_REAL = NULL;

int g_fontHeight = 10;

bool g_espIsEnabled = true;

float g_espDistance = 400.0f;

int g_mapZoneId = 0;

float g_mapX = 5.0f;
float g_mapY = 30.0f;

float g_mapWidth  = 200.0f;
float g_mapHeight = 200.0f;

float g_mapX_default = 5.0f;
float g_mapY_default = 30.0f;

float g_mapWidth_default  = 200.0f;
float g_mapHeight_default = 200.0f;

float g_mapX_alternate = 5.0f;
float g_mapY_alternate = 30.0f;

float g_mapWidth_alternate  = 640.0f;
float g_mapHeight_alternate = 480.0f;

float g_mapOriginX = g_mapX + (g_mapWidth  * 0.5f);
float g_mapOriginY = g_mapY + (g_mapHeight * 0.5f);

bool g_mapArrowIsEnabled = true;
bool g_mapSizeIsDefault = true;
bool g_mapZoneInfoIsEnabled = false;
bool g_mapHeightFilterIsEnabled = false;
bool g_mapSpawnsIsEnabled = true;
bool g_mapPointsIsEnabled = true;
bool g_mapLinesIsEnabled = true;

float g_mapHeightFilterLow  = 10.0; // z axis
float g_mapHeightFilterHigh = 10.0; // z axis

float g_mapZoom           = 0.5f;
float g_mapZoomMultiplier = 0.1f;
float g_mapZoomDefault    = 1.0f;
float g_mapZoomMinimum    = 10.0f;
float g_mapZoomMaximum    = 0.01f;

float g_mapArrowRadius = 25.0f;

float g_mapCenterLineSize = 5.0f;

DWORD g_mapBorderColor = 0xFF00FF00;
DWORD g_mapArrowColor  = 0xFF00FF00;
DWORD g_mapLineColor   = 0xFFFFFFFF;

DWORD g_mapButtonColorEnabled  = 0xFF00FF00;
DWORD g_mapButtonColorDisabled = 0xFFFF0000;
DWORD g_mapButtonColorExit     = 0xFFFF00FF;

float g_mapElementOffset = 5.0f;

float g_mapButtonWidth  = 10.0f;
float g_mapButtonHeight = 10.0f;

float g_mapButtonZoomInX = g_mapX + g_mapWidth + g_mapElementOffset;
float g_mapButtonZoomInY = g_mapY;

float g_mapButtonZoomOutX = g_mapX + g_mapWidth + g_mapElementOffset;
float g_mapButtonZoomOutY = g_mapY + g_mapButtonHeight + g_mapElementOffset;

float g_mapButtonZoom1X = g_mapX + g_mapWidth + g_mapElementOffset;
float g_mapButtonZoom1Y = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 2.0f);

float g_mapButtonToggleMapSizeX = g_mapX + g_mapWidth + g_mapElementOffset;
float g_mapButtonToggleMapSizeY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 3.0f);

float g_mapButtonToggleLineColorX = g_mapX + g_mapWidth + g_mapElementOffset;
float g_mapButtonToggleLineColorY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 4.0f);

float g_mapButtonToggleHeightFilterX = g_mapX + g_mapWidth + g_mapElementOffset;
float g_mapButtonToggleHeightFilterY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 5.0f);

float g_mapButtonToggleZoneInfoX = g_mapX + g_mapWidth + g_mapElementOffset;
float g_mapButtonToggleZoneInfoY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 6.0f);

float g_mapButtonToggleEspX = g_mapX + g_mapWidth + g_mapElementOffset;
float g_mapButtonToggleEspY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 7.0f);

float g_mapButtonToggleSpawnsX = g_mapX + g_mapWidth + g_mapElementOffset;
float g_mapButtonToggleSpawnsY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 8.0f);

float g_mapButtonTogglePointsX = g_mapX + g_mapWidth + g_mapElementOffset;
float g_mapButtonTogglePointsY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 9.0f);

float g_mapButtonToggleLinesX = g_mapX + g_mapWidth + g_mapElementOffset;
float g_mapButtonToggleLinesY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 10.0f);

float g_mapButtonExitX = g_mapX + g_mapWidth + g_mapElementOffset;
float g_mapButtonExitY = g_mapY + g_mapHeight - g_mapButtonHeight;

std::vector<EQMAPLINE>  g_mapLineList;
std::vector<EQMAPPOINT> g_mapPointList;

int g_mapNumLines = 0;

int g_mapTimerLeftClick;

// Cohen-Sutherland algorithm
// http://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
// need this to clip lines within the map window rectangle
#define LINECLIP_INSIDE 0 // 0000
#define LINECLIP_LEFT   1 // 0001
#define LINECLIP_RIGHT  2 // 0010
#define LINECLIP_BOTTOM 4 // 0100
#define LINECLIP_TOP    8 // 1000

int EQMACESP_GetLineClipValue(float x, float y, float minX, float minY, float maxX, float maxY)
{
    int value = LINECLIP_INSIDE;
 
    if (x < minX)
    {
        value |= LINECLIP_LEFT;
    }
    else if (x > maxX)
    {
        value |= LINECLIP_RIGHT;
    }

    if (y < minY)
    {
        value |= LINECLIP_BOTTOM;
    }
    else if (y > maxY)
    {
        value |= LINECLIP_TOP;
    }
 
    return value;
}

bool EQMACESP_IsPointInsideRectangle(int pointX, int pointY, int rectX, int rectY, int rectWidth, int rectHeight)
{
    if (pointX < rectX) return false;
    if (pointY < rectY) return false;
    if (pointX > (rectX + rectWidth)) return false;
    if (pointY > (rectY + rectHeight)) return false;

    return true;
}

void EQMACESP_StringReplaceAll(std::string& subject, const std::string& search, const std::string& replace)
{
    std::size_t position = 0;
    while ((position = subject.find(search, position)) != std::string::npos)
    {
         subject.replace(position, search.length(), replace);
         position += replace.length();
    }
}

bool EQMACESP_ParseMap(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
    {
        return false;
    }

    g_mapLineList.clear();
    g_mapPointList.clear();

    std::string line;
    while (std::getline(file, line))
    {
        if (line.size() == 0)
        {
            continue;
        }

        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

        char lineType = line.at(0);

        line = line.substr(1);

        std::vector<std::string> lineTokens;
        std::istringstream lineStream(line);
        std::string lineToken;
        while (std::getline(lineStream, lineToken, ','))
        {
            lineTokens.push_back(lineToken);
        }

        if (lineTokens.size() == 0)
        {
            continue;
        }

        if (lineType == 'L')
        {
            if (lineTokens.size() != 9)
            {
                continue;
            }

            EQMAPLINE mapLine;

            mapLine.X1 = std::stof(lineTokens.at(0));
            mapLine.Y1 = std::stof(lineTokens.at(1));
            mapLine.Z1 = std::stof(lineTokens.at(2));

            mapLine.X2 = std::stof(lineTokens.at(3));
            mapLine.Y2 = std::stof(lineTokens.at(4));
            mapLine.Z2 = std::stof(lineTokens.at(5));

            mapLine.R = std::stoi(lineTokens.at(6));
            mapLine.G = std::stoi(lineTokens.at(7));
            mapLine.B = std::stoi(lineTokens.at(8));

            g_mapLineList.push_back(mapLine);
        }
        else if (lineType == 'P')
        {
            if (lineTokens.size() != 8)
            {
                continue;
            }

            EQMAPPOINT mapPoint;

            mapPoint.X = std::stof(lineTokens.at(0));
            mapPoint.Y = std::stof(lineTokens.at(1));
            mapPoint.Z = std::stof(lineTokens.at(2));

            mapPoint.R = std::stoi(lineTokens.at(3));
            mapPoint.G = std::stoi(lineTokens.at(4));
            mapPoint.B = std::stoi(lineTokens.at(5));

            mapPoint.Size = std::stoi(lineTokens.at(6));

            std::string mapPointText = lineTokens.at(7);
            EQMACESP_StringReplaceAll(mapPointText, "_", " ");
            strcpy_s(mapPoint.Text, mapPointText.c_str());

            g_mapPointList.push_back(mapPoint);
        }
    }

    return true;
}

void EQMACESP_MapRecalculateCoordinates()
{
    g_mapOriginX = g_mapX + (g_mapWidth  * 0.5f);
    g_mapOriginY = g_mapY + (g_mapHeight * 0.5f);

    g_mapButtonZoomInX = g_mapX + g_mapWidth + g_mapElementOffset;
    g_mapButtonZoomInY = g_mapY;

    g_mapButtonZoomOutX = g_mapX + g_mapWidth + g_mapElementOffset;
    g_mapButtonZoomOutY = g_mapY + g_mapButtonHeight + g_mapElementOffset;

    g_mapButtonZoom1X = g_mapX + g_mapWidth + g_mapElementOffset;
    g_mapButtonZoom1Y = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 2.0f);

    g_mapButtonToggleMapSizeX = g_mapX + g_mapWidth + g_mapElementOffset;
    g_mapButtonToggleMapSizeY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 3.0f);

    g_mapButtonToggleLineColorX = g_mapX + g_mapWidth + g_mapElementOffset;
    g_mapButtonToggleLineColorY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 4.0f);

    g_mapButtonToggleHeightFilterX = g_mapX + g_mapWidth + g_mapElementOffset;
    g_mapButtonToggleHeightFilterY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 5.0f);

    g_mapButtonToggleZoneInfoX = g_mapX + g_mapWidth + g_mapElementOffset;
    g_mapButtonToggleZoneInfoY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 6.0f);

    g_mapButtonToggleEspX = g_mapX + g_mapWidth + g_mapElementOffset;
    g_mapButtonToggleEspY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 7.0f);

    g_mapButtonToggleSpawnsX = g_mapX + g_mapWidth + g_mapElementOffset;
    g_mapButtonToggleSpawnsY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 8.0f);

    g_mapButtonTogglePointsX = g_mapX + g_mapWidth + g_mapElementOffset;
    g_mapButtonTogglePointsY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 9.0f);

    g_mapButtonToggleLinesX = g_mapX + g_mapWidth + g_mapElementOffset;
    g_mapButtonToggleLinesY = g_mapY + ((g_mapButtonHeight + g_mapElementOffset) * 10.0f);

    g_mapButtonExitX = g_mapX + g_mapWidth + g_mapElementOffset;
    g_mapButtonExitY = g_mapY + g_mapHeight - g_mapButtonHeight;
}

void EQMACESP_MapZoomOut()
{
    g_mapZoom -= g_mapZoom * g_mapZoomMultiplier;

    if (g_mapZoom <= g_mapZoomMaximum)
    {
        g_mapZoom = g_mapZoomMaximum;
    }
}

void EQMACESP_MapZoomIn()
{
    g_mapZoom += g_mapZoom * g_mapZoomMultiplier;

    if (g_mapZoom >= g_mapZoomMinimum)
    {
        g_mapZoom = g_mapZoomMinimum;
    }
}

void EQMACESP_MapZoom1()
{
    g_mapZoom = 1.0f;
}

void EQMACESP_MapToggleSize()
{
    EQ_ToggleBool(g_mapSizeIsDefault);

    if (g_mapSizeIsDefault == true)
    {
        g_mapX = g_mapX_default;
        g_mapY = g_mapY_default;

        g_mapWidth  = g_mapWidth_default;
        g_mapHeight = g_mapWidth_default;
    }
    else
    {
        g_mapX = g_mapX_alternate;
        g_mapY = g_mapY_alternate;

        g_mapWidth  = g_mapWidth_alternate;
        g_mapHeight = g_mapHeight_alternate;
    }

    EQMACESP_MapRecalculateCoordinates();
}


void EQMACESP_MapToggleHeightFilter()
{
    EQ_ToggleBool(g_mapHeightFilterIsEnabled);
}

void EQMACESP_MapToggleEsp()
{
    EQ_ToggleBool(g_espIsEnabled);
}

void EQMACESP_MapToggleZoneInfo()
{
    EQ_ToggleBool(g_mapZoneInfoIsEnabled);
}

void EQMACESP_MapToggleSpawns()
{
    EQ_ToggleBool(g_mapSpawnsIsEnabled);
}

void EQMACESP_MapTogglePoints()
{
    EQ_ToggleBool(g_mapPointsIsEnabled);
}

void EQMACESP_MapToggleLines()
{
    EQ_ToggleBool(g_mapLinesIsEnabled);
}

void EQMACESP_MapToggleLineColor()
{
    if (g_mapLineColor == 0xFFFFFFFF)
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
        g_mapLineColor = 0xFFFFFFFF;
    }
}

int __cdecl EQMACESP_DrawNetStatus_DETOUR(int a1, unsigned short a2, unsigned short a3, unsigned short a4, unsigned short a5, int a6, unsigned short a7, unsigned long a8, long a9, unsigned long a10)
{
    g_mapNumLines = 0;

    DWORD fontArial14 = EQ_READ_MEMORY<DWORD>(EQ_POINTER_FONT_ARIAL14);

    DWORD worldSpaceToScreenSpaceCameraData = EQ_READ_MEMORY<DWORD>(EQ_POINTER_T3D_WORLD_SPACE_TO_SCREEN_SPACE_CAMERA_DATA);

    PEQSPAWNINFO spawn = (PEQSPAWNINFO)EQ_OBJECT_FirstSpawn;

    PEQGROUNDSPAWNINFO groundSpawn = (PEQGROUNDSPAWNINFO)EQ_OBJECT_FirstGroundSpawn;

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    EQGROUPLIST* groupList = (EQGROUPLIST*)EQ_STRUCTURE_GROUP_LIST;

    EQLOCATION playerLocation = {EQ_OBJECT_PlayerSpawn->Y, EQ_OBJECT_PlayerSpawn->X, EQ_OBJECT_PlayerSpawn->Z};

    DWORD zoneId = EQ_READ_MEMORY<DWORD>(EQ_ZONE_ID);

    EQZONEINFO* zoneInfo = (EQZONEINFO*)EQ_STRUCTURE_ZONE_INFO;

    if (g_mapZoneId != zoneId)
    {
        std::stringstream mapFilename;
        mapFilename << "maps/" << zoneInfo->ShortName << ".txt";

        if (EQMACESP_ParseMap(mapFilename.str()) == true)
        {
            g_mapZoneId = zoneId;
        }
        else
        {
            g_mapLineList.clear();
            g_mapPointList.clear();
        }
    }

    WORD mouseX = EQ_READ_MEMORY<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_READ_MEMORY<WORD>(EQ_MOUSE_Y);

    DWORD mouseClickState = EQ_READ_MEMORY<DWORD>(EQ_MOUSE_CLICK_STATE);

    if
    (
        EQMACESP_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonZoomInX, (int)g_mapButtonZoomInY,
            (int)g_mapButtonWidth, (int)g_mapButtonHeight
        )
        == true
    )
    {
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Zoom In",
            (int)(g_mapButtonZoomInX + g_mapButtonWidth + g_mapElementOffset),
            (int)(g_mapButtonZoomInY - 2.0f),
            EQ_TEXT_COLOR_LIGHT_GREEN,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            EQMACESP_MapZoomIn();
        }
    }

    if
    (
        EQMACESP_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonZoomOutX, (int)g_mapButtonZoomOutY,
            (int)g_mapButtonWidth, (int)g_mapButtonHeight
        )
        == true
    )
    {
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Zoom Out",
            (int)(g_mapButtonZoomOutX + g_mapButtonWidth + g_mapElementOffset),
            (int)(g_mapButtonZoomOutY - 2.0f),
            EQ_TEXT_COLOR_LIGHT_GREEN,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            EQMACESP_MapZoomOut();
        }
    }

    if
    (
        EQMACESP_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonZoom1X, (int)g_mapButtonZoom1Y,
            (int)g_mapButtonWidth, (int)g_mapButtonHeight
        )
        == true
    )
    {
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Zoom 1x",
            (int)(g_mapButtonZoom1X + g_mapButtonWidth + g_mapElementOffset),
            (int)(g_mapButtonZoom1Y - 2.0f),
            EQ_TEXT_COLOR_LIGHT_GREEN,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            EQMACESP_MapZoom1();
        }
    }

    if
    (
        EQMACESP_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleMapSizeX, (int)g_mapButtonToggleMapSizeY,
            (int)g_mapButtonWidth, (int)g_mapButtonHeight
        )
        == true
    )
    {
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Map Size",
            (int)(g_mapButtonToggleMapSizeX + g_mapButtonWidth + g_mapElementOffset),
            (int)(g_mapButtonToggleMapSizeY - 2.0f),
            EQ_TEXT_COLOR_LIGHT_GREEN,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            // half a second between mouse clicks to prevent spam
            if ((currentTime - g_mapTimerLeftClick) > 500)
            {
                g_mapTimerLeftClick = currentTime;

                EQMACESP_MapToggleSize();
            }
        }
    }

    if
    (
        EQMACESP_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleHeightFilterX, (int)g_mapButtonToggleHeightFilterY,
            (int)g_mapButtonWidth, (int)g_mapButtonHeight
        )
        == true
    )
    {
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Height Filter",
            (int)(g_mapButtonToggleHeightFilterX + g_mapButtonWidth + g_mapElementOffset),
            (int)(g_mapButtonToggleHeightFilterY - 2.0f),
            EQ_TEXT_COLOR_LIGHT_GREEN,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            // half a second between mouse clicks to prevent spam
            if ((currentTime - g_mapTimerLeftClick) > 500)
            {
                g_mapTimerLeftClick = currentTime;

                EQMACESP_MapToggleHeightFilter();

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
        EQMACESP_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleLineColorX, (int)g_mapButtonToggleLineColorY,
            (int)g_mapButtonWidth, (int)g_mapButtonHeight
        )
        == true
    )
    {
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Line Color",
            (int)(g_mapButtonToggleLineColorX + g_mapButtonWidth + g_mapElementOffset),
            (int)(g_mapButtonToggleLineColorY - 2.0f),
            EQ_TEXT_COLOR_LIGHT_GREEN,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            // half a second between mouse clicks to prevent spam
            if ((currentTime - g_mapTimerLeftClick) > 500)
            {
                g_mapTimerLeftClick = currentTime;

                EQMACESP_MapToggleLineColor();
            }
        }
    }

    if
    (
        EQMACESP_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleZoneInfoX, (int)g_mapButtonToggleZoneInfoY,
            (int)g_mapButtonWidth, (int)g_mapButtonHeight
        )
        == true
    )
    {
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Zone Info",
            (int)(g_mapButtonToggleZoneInfoX + g_mapButtonWidth + g_mapElementOffset),
            (int)(g_mapButtonToggleZoneInfoY - 2.0f),
            EQ_TEXT_COLOR_LIGHT_GREEN,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            // half a second between mouse clicks to prevent spam
            if ((currentTime - g_mapTimerLeftClick) > 500)
            {
                g_mapTimerLeftClick = currentTime;

                EQMACESP_MapToggleZoneInfo();

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
        EQMACESP_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleEspX, (int)g_mapButtonToggleEspY,
            (int)g_mapButtonWidth, (int)g_mapButtonHeight
        )
        == true
    )
    {
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "ESP",
            (int)(g_mapButtonToggleEspX + g_mapButtonWidth + g_mapElementOffset),
            (int)(g_mapButtonToggleEspY - 2.0f),
            EQ_TEXT_COLOR_LIGHT_GREEN,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            // half a second between mouse clicks to prevent spam
            if ((currentTime - g_mapTimerLeftClick) > 500)
            {
                g_mapTimerLeftClick = currentTime;

                EQMACESP_MapToggleEsp();

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
        EQMACESP_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleSpawnsX, (int)g_mapButtonToggleSpawnsY,
            (int)g_mapButtonWidth, (int)g_mapButtonHeight
        )
        == true
    )
    {
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Spawns",
            (int)(g_mapButtonToggleSpawnsX + g_mapButtonWidth + g_mapElementOffset),
            (int)(g_mapButtonToggleSpawnsY - 2.0f),
            EQ_TEXT_COLOR_LIGHT_GREEN,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            // half a second between mouse clicks to prevent spam
            if ((currentTime - g_mapTimerLeftClick) > 500)
            {
                g_mapTimerLeftClick = currentTime;

                EQMACESP_MapToggleSpawns();

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
        EQMACESP_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonTogglePointsX, (int)g_mapButtonTogglePointsY,
            (int)g_mapButtonWidth, (int)g_mapButtonHeight
        )
        == true
    )
    {
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Points",
            (int)(g_mapButtonTogglePointsX + g_mapButtonWidth + g_mapElementOffset),
            (int)(g_mapButtonTogglePointsY - 2.0f),
            EQ_TEXT_COLOR_LIGHT_GREEN,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            // half a second between mouse clicks to prevent spam
            if ((currentTime - g_mapTimerLeftClick) > 500)
            {
                g_mapTimerLeftClick = currentTime;

                EQMACESP_MapTogglePoints();

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
        EQMACESP_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonToggleLinesX, (int)g_mapButtonToggleLinesY,
            (int)g_mapButtonWidth, (int)g_mapButtonHeight
        )
        == true
    )
    {
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Lines",
            (int)(g_mapButtonToggleLinesX + g_mapButtonWidth + g_mapElementOffset),
            (int)(g_mapButtonToggleLinesY - 2.0f),
            EQ_TEXT_COLOR_LIGHT_GREEN,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            DWORD currentTime = EQ_READ_MEMORY<DWORD>(EQ_TIMER);

            // half a second between mouse clicks to prevent spam
            if ((currentTime - g_mapTimerLeftClick) > 500)
            {
                g_mapTimerLeftClick = currentTime;

                EQMACESP_MapToggleLines();

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
        EQMACESP_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapButtonExitX, (int)g_mapButtonExitY,
            (int)g_mapButtonWidth, (int)g_mapButtonHeight
        )
        == true
    )
    {
        EQ_CLASS_CDisplay->WriteTextHD2
        (
            "Exit",
            (int)(g_mapButtonExitX + g_mapButtonWidth + g_mapElementOffset),
            (int)(g_mapButtonExitY - 2.0f),
            EQ_TEXT_COLOR_PINK,
            fontArial14
        );

        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT)
        {
            g_bExit = 1;
        }
    }

    if (g_espIsEnabled == true && g_mapSizeIsDefault == true)
    {

    while (groundSpawn)
    {
        EQLOCATION spawnLocation = {groundSpawn->Y, groundSpawn->X, groundSpawn->Z};

        float spawnDistance = EQ_CalculateDistance(playerLocation.X, playerLocation.Y, spawnLocation.X, spawnLocation.Y);

        if (spawnDistance > g_espDistance)
        {
            groundSpawn = groundSpawn->Next;
            continue;
        }

        float resultX = 0.0f;
        float resultY = 0.0f;
        int result = EQGfx_Dx8__t3dWorldSpaceToScreenSpace(worldSpaceToScreenSpaceCameraData, &spawnLocation, &resultX, &resultY);

        if (result != EQ_T3D_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
        {
            int screenX = (int)resultX;
            int screenY = (int)resultY;

            const char* spawnName = EQ_GetGroundSpawnName(groundSpawn->Name);

            char spawnText[128];
            sprintf_s(spawnText, "+ %s", spawnName);

            char distanceText[32];
            sprintf_s(distanceText, " (%d)", (int)spawnDistance);
            strcat_s(spawnText, distanceText);

            EQ_CLASS_CDisplay->WriteTextHD2(spawnText, screenX, screenY, EQ_TEXT_COLOR_WHITE, fontArial14);
        }

        groundSpawn = groundSpawn->Next;
    }

    while (spawn)
    {
        EQLOCATION spawnLocation = {spawn->Y, spawn->X, spawn->Z};

        float spawnDistance = EQ_CalculateDistance(playerLocation.X, playerLocation.Y, spawnLocation.X, spawnLocation.Y);

        if (spawnDistance > g_espDistance && spawn->Type == EQ_SPAWN_TYPE_NPC)
        {
            spawn = spawn->Next;
            continue;
        }

        float resultX = 0.0f;
        float resultY = 0.0f;
        int result = EQGfx_Dx8__t3dWorldSpaceToScreenSpace(worldSpaceToScreenSpaceCameraData, &spawnLocation, &resultX, &resultY);

        if (result != EQ_T3D_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
        {
            int screenX = (int)resultX;
            int screenY = (int)resultY;

            if (spawn == playerSpawn)
            {
                int spawnHpPercent = (spawn->HpCurrent * 100) / spawn->HpMax;

                if (spawnHpPercent < 100)
                {
                    char hpText[128];
                    sprintf_s(hpText, "HP: %d/%d (%d%%)", spawn->HpCurrent, spawn->HpMax, spawnHpPercent);

                    EQ_CLASS_CDisplay->WriteTextHD2(hpText, screenX, screenY + g_fontHeight + 1, EQ_TEXT_COLOR_LIGHT_GREEN, fontArial14);
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
                    textColor = EQ_TEXT_COLOR_RED;
                    break;

                case EQ_SPAWN_TYPE_NPC:
                    textColor = EQ_TEXT_COLOR_CYAN;
                    break;

                case EQ_SPAWN_TYPE_NPC_CORPSE:
                case EQ_SPAWN_TYPE_PLAYER_CORPSE:
                    textColor = EQ_TEXT_COLOR_YELLOW;
                    break;
            }

            for (std::size_t i = 0; i < EQ_GROUP_MEMBERS_MAX; i++)
            {
                if (spawn == groupList->GroupMember[i])
                {
                    textColor = EQ_TEXT_COLOR_LIGHT_GREEN;
                    break;
                }
            }

            if (spawn == targetSpawn)
            {
                textColor = EQ_TEXT_COLOR_PINK;
            }

            if (spawn->IsGameMaster == 1)
            {
                textColor = EQ_TEXT_COLOR_PINK;

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

            if (spawn->IsLinkDead == 1 && spawn->Type == EQ_SPAWN_TYPE_PLAYER)
            {
                strcat_s(spawnText, " LD");
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

    } // if (g_espIsEnabled == true)

    EQ_DrawRectangle(g_mapX, g_mapY, g_mapWidth, g_mapHeight, g_mapBorderColor);

    g_mapNumLines += 4;

    int mapButtonColor = g_mapButtonColorEnabled;
    int mapButtonTextColor = EQ_TEXT_COLOR_LIGHT_GREEN;

    EQ_DrawRectangle(g_mapButtonZoomInX, g_mapButtonZoomInY, g_mapButtonWidth, g_mapButtonHeight, mapButtonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("+", (int)(g_mapButtonZoomInX + 3.0f), (int)(g_mapButtonZoomInY - 2.0f), mapButtonTextColor, fontArial14);

    g_mapNumLines += 4;

    EQ_DrawRectangle(g_mapButtonZoomOutX, g_mapButtonZoomOutY, g_mapButtonWidth, g_mapButtonHeight, mapButtonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("-", (int)(g_mapButtonZoomOutX + 4.0f), (int)(g_mapButtonZoomOutY - 3.0f), mapButtonTextColor, fontArial14);

    g_mapNumLines += 4;

    EQ_DrawRectangle(g_mapButtonZoom1X, g_mapButtonZoom1Y, g_mapButtonWidth, g_mapButtonHeight, mapButtonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("1", (int)(g_mapButtonZoom1X + 4.0f), (int)(g_mapButtonZoom1Y - 2.0f), mapButtonTextColor, fontArial14);

    g_mapNumLines += 4;

    EQ_DrawRectangle(g_mapButtonToggleMapSizeX, g_mapButtonToggleMapSizeY, g_mapButtonWidth, g_mapButtonHeight, mapButtonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("m", (int)(g_mapButtonToggleMapSizeX + 2.0f), (int)(g_mapButtonToggleMapSizeY - 2.0f), mapButtonTextColor, fontArial14);

    g_mapNumLines += 4;

    EQ_DrawRectangle(g_mapButtonToggleLineColorX, g_mapButtonToggleLineColorY, g_mapButtonWidth, g_mapButtonHeight, mapButtonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("c", (int)(g_mapButtonToggleLineColorX + 3.0f), (int)(g_mapButtonToggleLineColorY - 2.0f), mapButtonTextColor, fontArial14);

    g_mapNumLines += 4;

    mapButtonColor = g_mapButtonColorEnabled;
    mapButtonTextColor = EQ_TEXT_COLOR_LIGHT_GREEN;

    if (g_mapHeightFilterIsEnabled == false)
    {
        mapButtonColor = g_mapButtonColorDisabled;
        mapButtonTextColor = EQ_TEXT_COLOR_RED;
    }

    EQ_DrawRectangle(g_mapButtonToggleHeightFilterX, g_mapButtonToggleHeightFilterY, g_mapButtonWidth, g_mapButtonHeight, mapButtonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("h", (int)(g_mapButtonToggleHeightFilterX + 3.0f), (int)(g_mapButtonToggleHeightFilterY - 2.0f), mapButtonTextColor, fontArial14);

    g_mapNumLines += 4;

    mapButtonColor = g_mapButtonColorEnabled;
    mapButtonTextColor = EQ_TEXT_COLOR_LIGHT_GREEN;

    if (g_mapZoneInfoIsEnabled == false)
    {
        mapButtonColor = g_mapButtonColorDisabled;
        mapButtonTextColor = EQ_TEXT_COLOR_RED;
    }

    EQ_DrawRectangle(g_mapButtonToggleZoneInfoX, g_mapButtonToggleZoneInfoY, g_mapButtonWidth, g_mapButtonHeight, mapButtonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("z", (int)(g_mapButtonToggleZoneInfoX + 3.0f), (int)(g_mapButtonToggleZoneInfoY - 2.0f), mapButtonTextColor, fontArial14);

    g_mapNumLines += 4;

    mapButtonColor = g_mapButtonColorEnabled;
    mapButtonTextColor = EQ_TEXT_COLOR_LIGHT_GREEN;

    if (g_espIsEnabled == false)
    {
        mapButtonColor = g_mapButtonColorDisabled;
        mapButtonTextColor = EQ_TEXT_COLOR_RED;
    }

    EQ_DrawRectangle(g_mapButtonToggleEspX, g_mapButtonToggleEspY, g_mapButtonWidth, g_mapButtonHeight, mapButtonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("e", (int)(g_mapButtonToggleEspX + 3.0f), (int)(g_mapButtonToggleEspY - 2.0f), mapButtonTextColor, fontArial14);

    g_mapNumLines += 4;

    mapButtonColor = g_mapButtonColorEnabled;
    mapButtonTextColor = EQ_TEXT_COLOR_LIGHT_GREEN;

    if (g_mapSpawnsIsEnabled == false)
    {
        mapButtonColor = g_mapButtonColorDisabled;
        mapButtonTextColor = EQ_TEXT_COLOR_RED;
    }

    EQ_DrawRectangle(g_mapButtonToggleSpawnsX, g_mapButtonToggleSpawnsY, g_mapButtonWidth, g_mapButtonHeight, mapButtonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("s", (int)(g_mapButtonToggleSpawnsX + 3.0f), (int)(g_mapButtonToggleSpawnsY - 2.0f), mapButtonTextColor, fontArial14);

    g_mapNumLines += 4;

    mapButtonColor = g_mapButtonColorEnabled;
    mapButtonTextColor = EQ_TEXT_COLOR_LIGHT_GREEN;

    if (g_mapPointsIsEnabled == false)
    {
        mapButtonColor = g_mapButtonColorDisabled;
        mapButtonTextColor = EQ_TEXT_COLOR_RED;
    }

    EQ_DrawRectangle(g_mapButtonTogglePointsX, g_mapButtonTogglePointsY, g_mapButtonWidth, g_mapButtonHeight, mapButtonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("p", (int)(g_mapButtonTogglePointsX + 3.0f), (int)(g_mapButtonTogglePointsY - 3.0f), mapButtonTextColor, fontArial14);

    g_mapNumLines += 4;

    mapButtonColor = g_mapButtonColorEnabled;
    mapButtonTextColor = EQ_TEXT_COLOR_LIGHT_GREEN;

    if (g_mapLinesIsEnabled == false)
    {
        mapButtonColor = g_mapButtonColorDisabled;
        mapButtonTextColor = EQ_TEXT_COLOR_RED;
    }

    EQ_DrawRectangle(g_mapButtonToggleLinesX, g_mapButtonToggleLinesY, g_mapButtonWidth, g_mapButtonHeight, mapButtonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("l", (int)(g_mapButtonToggleLinesX + 5.0f), (int)(g_mapButtonToggleLinesY - 2.0f), mapButtonTextColor, fontArial14);

    g_mapNumLines += 4;

    mapButtonColor = g_mapButtonColorExit;
    mapButtonTextColor = EQ_TEXT_COLOR_PINK;

    EQ_DrawRectangle(g_mapButtonExitX, g_mapButtonExitY, g_mapButtonWidth, g_mapButtonHeight, mapButtonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("x", (int)(g_mapButtonExitX + 3.0f), (int)(g_mapButtonExitY - 2.0f), mapButtonTextColor, fontArial14);

    g_mapNumLines += 4;

    if (g_mapLinesIsEnabled == true)
    {

    for (const EQMAPLINE &mapLine : g_mapLineList)
    {
        if (g_mapHeightFilterIsEnabled == true)
        {
            if (g_mapHeightFilterLow > 0.0f)
            {
                if
                (
                    mapLine.Z1 < (playerSpawn->Z - g_mapHeightFilterLow) ||
                    mapLine.Z2 < (playerSpawn->Z - g_mapHeightFilterLow)
                )
                {
                    continue;
                }
            }

            if (g_mapHeightFilterHigh > 0.0f)
            {
                if
                (
                    mapLine.Z1 > (playerSpawn->Z + g_mapHeightFilterHigh) ||
                    mapLine.Z2 > (playerSpawn->Z + g_mapHeightFilterHigh)
                )
                {
                    continue;
                }
            }
        }

        EQLINE line;
        line.X1 = ((mapLine.X1 * g_mapZoom) + g_mapOriginX) + (playerSpawn->X * g_mapZoom);
        line.Y1 = ((mapLine.Y1 * g_mapZoom) + g_mapOriginY) + (playerSpawn->Y * g_mapZoom);
        line.Z1 = 1.0f;
        line.X2 = ((mapLine.X2 * g_mapZoom) + g_mapOriginX) + (playerSpawn->X * g_mapZoom);
        line.Y2 = ((mapLine.Y2 * g_mapZoom) + g_mapOriginY) + (playerSpawn->Y * g_mapZoom);
        line.Z2 = 1.0f;

        float minX = g_mapX;
        float maxX = g_mapX + g_mapWidth;

        float minY = g_mapY;
        float maxY = g_mapY + g_mapHeight;

        if
        (
            line.X1 > maxX &&
            line.X2 > maxX &&
            line.X1 < minX &&
            line.X2 < minX
        )
        {
            continue;
        }

        if
        (
            line.Y1 > maxY &&
            line.Y2 > maxY &&
            line.Y1 < minY &&
            line.Y2 < minY
        )
        {
            continue;
        }

        // don't clip lines into the rectangle, offset by 1 pixel
        minX = minX + 1.0f;
        minY = minY + 1.0f;
        maxX = maxX - 1.0f;
        maxY = maxY - 1.0f;

        // Cohen-Sutherland algorithm
        // clip the map lines to the rectangle

        bool bDrawLine = false;

        int lineClipValue1 = EQMACESP_GetLineClipValue(line.X1, line.Y1, minX, minY, maxX, maxY);
        int lineClipValue2 = EQMACESP_GetLineClipValue(line.X2, line.Y2, minX, minY, maxX, maxY);

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
 
                if (lineClipValueOut & LINECLIP_TOP)
                {
                    x = line.X1 + (line.X2 - line.X1) * (maxY - line.Y1) / (line.Y2 - line.Y1);
                    y = maxY;
                }
                else if (lineClipValueOut & LINECLIP_BOTTOM)
                {
                    x = line.X1 + (line.X2 - line.X1) * (minY - line.Y1) / (line.Y2 - line.Y1);
                    y = minY;
                }
                else if (lineClipValueOut & LINECLIP_RIGHT)
                {
                    y = line.Y1 + (line.Y2 - line.Y1) * (maxX - line.X1) / (line.X2 - line.X1);
                    x = maxX;
                }
                else if (lineClipValueOut & LINECLIP_LEFT)
                {
                    y = line.Y1 + (line.Y2 - line.Y1) * (minX - line.X1) / (line.X2 - line.X1);
                    x = minX;
                }
 
                if (lineClipValueOut == lineClipValue1)
                {
                    line.X1 = x;
                    line.Y1 = y;
                    lineClipValue1 = EQMACESP_GetLineClipValue(line.X1, line.Y1, minX, minY, maxX, maxY);
                }
                else
                {
                    line.X2 = x;
                    line.Y2 = y;
                    lineClipValue2 = EQMACESP_GetLineClipValue(line.X2, line.Y2, minX, minY, maxX, maxY);
                }
            }
        }

        // only use 75% of the maximum allowed number of deferred objects
        // the game needs to reserve some to draw stuff
        // this also leaves some extra to draw the player arrow
        if (g_mapNumLines > (int)(EQ_GRAPHICS_DLL_DEFERRED_OBJECTS_MAX * 0.75f))
        {
            bDrawLine = false;
        }

        if (bDrawLine == true)
        {
            EQGfx_Dx8__t3dDeferLine(&line, g_mapLineColor);

            g_mapNumLines += 1;
        }
    }

    } // if (g_mapLinesIsEnabled == true)

    if (g_mapPointsIsEnabled == true && g_mapSizeIsDefault == false)
    {
        for (const EQMAPPOINT &mapPoint : g_mapPointList)
        {
            int pointX = (int)(((mapPoint.X * g_mapZoom) + g_mapOriginX) + (playerSpawn->X * g_mapZoom));
            int pointY = (int)(((mapPoint.Y * g_mapZoom) + g_mapOriginY) + (playerSpawn->Y * g_mapZoom));

            if
            (
                pointX >= (g_mapX + g_mapWidth)  ||
                pointY >= (g_mapY + g_mapHeight) ||
                pointX <= g_mapX                 ||
                pointY <= g_mapY
            )
            {
                continue;
            }

            char pointText[128];
            sprintf_s(pointText, "+ %s", mapPoint.Text);

            EQ_CLASS_CDisplay->WriteTextHD2(pointText, pointX, pointY, EQ_TEXT_COLOR_LIGHT_GRAY, fontArial14);
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

        EQLOCATION spawnLocation = {spawn->Y, spawn->X, spawn->Z};

        float spawnDistance = EQ_CalculateDistance(playerLocation.X, playerLocation.Y, spawnLocation.X, spawnLocation.Y);

        if (spawnDistance > g_espDistance && spawn->Type == EQ_SPAWN_TYPE_NPC)
        {
            spawn = spawn->Next;
            continue;
        }

        DWORD textColor = EQ_TEXT_COLOR_WHITE;

        switch (spawn->Type)
        {
            case EQ_SPAWN_TYPE_PLAYER:
                textColor = EQ_TEXT_COLOR_RED;
                break;

            case EQ_SPAWN_TYPE_NPC:
                textColor = EQ_TEXT_COLOR_CYAN;
                break;

            case EQ_SPAWN_TYPE_NPC_CORPSE:
            case EQ_SPAWN_TYPE_PLAYER_CORPSE:
                textColor = EQ_TEXT_COLOR_YELLOW;
                break;
        }

        for (std::size_t i = 0; i < EQ_GROUP_MEMBERS_MAX; i++)
        {
            if (spawn == groupList->GroupMember[i])
            {
                textColor = EQ_TEXT_COLOR_LIGHT_GREEN;
                break;
            }
        }

        if (spawn == targetSpawn)
        {
            textColor = EQ_TEXT_COLOR_PINK;
        }

        int mapSpawnX = (int)(((-spawn->X * g_mapZoom) + g_mapOriginX) + (playerSpawn->X * g_mapZoom));
        int mapSpawnY = (int)(((-spawn->Y * g_mapZoom) + g_mapOriginY) + (playerSpawn->Y * g_mapZoom));

        if
        (
            mapSpawnX >= (g_mapX + g_mapWidth)  ||
            mapSpawnY >= (g_mapY + g_mapHeight) ||
            mapSpawnX <= g_mapX                 ||
            mapSpawnY <= g_mapY
        )
        {
            spawn = spawn->Next;
            continue;
        }

        if (g_mapSizeIsDefault == true)
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

    float playerHeadingRadiansC = playerHeadingC * 3.14159265358979f / 256.0f;

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

    float playerHeadingRadiansB = playerHeadingB * 3.14159265358979f / 256.0f;

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
        EQ_CLASS_CDisplay->WriteTextHD2(zoneText, (int)g_mapX, (int)(g_mapY + g_mapHeight + g_mapElementOffset), EQ_TEXT_COLOR_LIGHT_GREEN, fontArial14);
    }

    return EQMACESP_DrawNetStatus_REAL(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

DWORD WINAPI EQMACESP_ThreadLoop(LPVOID param)
{
    while (g_bExit == 0)
    {
        //
    }

    DetourRemove((PBYTE)EQMACESP_DrawNetStatus_REAL, (PBYTE)EQMACESP_DrawNetStatus_DETOUR);

    FreeLibraryAndExitThread(g_module, 0);
    return 0;
}

DWORD WINAPI EQMACESP_ThreadLoad(LPVOID param)
{
    HINSTANCE graphicsDll = LoadLibraryA(EQ_GRAPHICS_DLL_NAME);
    if (!graphicsDll)
    {
        MessageBoxA(NULL, "Error: Failed to LoadLibrary EQGfx_Dx8.dll!", "EQMac ESP", MB_ICONERROR);

        FreeLibraryAndExitThread(g_module, 0);
        return 0;
    }

    EQGfx_Dx8__t3dWorldSpaceToScreenSpace = (EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dWorldSpaceToScreenSpace)GetProcAddress(graphicsDll, EQ_T3D_WORLD_SPACE_TO_SCREEN_SPACE_FUNCTION_NAME);
    if (!EQGfx_Dx8__t3dWorldSpaceToScreenSpace)
    {
        MessageBoxA(NULL, "Error: World to Screen function not found!", "EQMac ESP", MB_ICONERROR);

        FreeLibraryAndExitThread(g_module, 0);
        return 0;
    }

    EQGfx_Dx8__t3dDeferLine = (EQ_FUNCTION_TYPE_EQGfx_Dx8__t3dDeferLine)GetProcAddress(graphicsDll, EQ_T3D_DEFER_LINE_FUNCTION_NAME);
    if (!EQGfx_Dx8__t3dDeferLine)
    {
        MessageBoxA(NULL, "Error: Draw Line function not found!", "EQMac ESP", MB_ICONERROR);

        FreeLibraryAndExitThread(g_module, 0);
        return 0;
    }

    EQMACESP_DrawNetStatus_REAL = (EQ_FUNCTION_TYPE_DrawNetStatus)DetourFunction((PBYTE)EQ_FUNCTION_DrawNetStatus, (PBYTE)EQMACESP_DrawNetStatus_DETOUR);

    g_handleThreadLoop = CreateThread(NULL, 0, &EQMACESP_ThreadLoop, NULL, 0, NULL);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    g_module = module;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(module);
            g_handleThreadLoad = CreateThread(NULL, 0, &EQMACESP_ThreadLoad, NULL, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            WaitForSingleObject(EQMACESP_ThreadLoad, INFINITE);
            CloseHandle(g_handleThreadLoad);

            WaitForSingleObject(EQMACESP_ThreadLoop, INFINITE);
            CloseHandle(g_handleThreadLoop);
            break;
    }

    return TRUE;
}
