#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cmath>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <tlhelp32.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

//#define DIRECTINPUT_VERSION 0x0800
//#include "dinput.h"
//#pragma comment(lib, "dinput.lib")

#include "detours.h"
#pragma comment(lib, "detours.lib")

#include "eqmac.h"
#include "eqmac_functions.h"

const char* g_applicationName    = "EQMac MacroQuest";
const char* g_applicationExeName = "eqmac_mq";

bool g_bLoaded = false;

volatile int g_bExit = 0;

HWND g_hwnd;

HMODULE g_module;

HANDLE g_handleThreadLoad;
HANDLE g_handleThreadLoop;

WSADATA g_wsaData;
SOCKET g_connectSocket;
fd_set g_fdSetRead;

EQ_FUNCTION_TYPE_CEverQuest__LMouseDown EQMACMQ_REAL_CEverQuest__LMouseDown = NULL;
EQ_FUNCTION_TYPE_CEverQuest__LMouseUp   EQMACMQ_REAL_CEverQuest__LMouseUp   = NULL;

EQ_FUNCTION_TYPE_HandleMouseWheel EQMACMQ_REAL_HandleMouseWheel = NULL;

EQ_FUNCTION_TYPE_ProcessKeyDown EQMACMQ_REAL_ProcessKeyDown = NULL;
EQ_FUNCTION_TYPE_ProcessKeyUp   EQMACMQ_REAL_ProcessKeyUp   = NULL;

EQ_FUNCTION_TYPE_ProcessMovementKeys EQMACMQ_REAL_ProcessMovementKeys = NULL;

EQ_FUNCTION_TYPE_CEverQuest__InterpretCmd EQMACMQ_REAL_CEverQuest__InterpretCmd = NULL;

EQ_FUNCTION_TYPE_CEverQuest__dsp_chat EQMACMQ_REAL_CEverQuest__dsp_chat = NULL;

EQ_FUNCTION_TYPE_DrawNetStatus EQMACMQ_REAL_DrawNetStatus = NULL;

EQ_FUNCTION_TYPE_CItemDisplayWnd__SetItem  EQMACMQ_REAL_CItemDisplayWnd__SetItem  = NULL;
EQ_FUNCTION_TYPE_CItemDisplayWnd__SetSpell EQMACMQ_REAL_CItemDisplayWnd__SetSpell = NULL;

EQ_FUNCTION_TYPE_CBuffWindow__RefreshBuffDisplay  EQMACMQ_REAL_CBuffWindow__RefreshBuffDisplay = NULL;

EQ_FUNCTION_TYPE_CDisplay__SetNameSpriteState EQMACMQ_REAL_CDisplay__SetNameSpriteState = NULL;
EQ_FUNCTION_TYPE_CDisplay__SetNameSpriteTint  EQMACMQ_REAL_CDisplay__SetNameSpriteTint  = NULL;

EQ_FUNCTION_TYPE_EQ_Character__eqspa_movement_rate EQMACMQ_REAL_EQ_Character__eqspa_movement_rate = NULL;

EQ_FUNCTION_TYPE_EQ_Character__CastSpell EQMACMQ_REAL_EQ_Character__CastSpell = NULL;

EQ_FUNCTION_TYPE_EQ_Character__UseSkill EQMACMQ_REAL_EQ_Character__UseSkill = NULL;

EQ_FUNCTION_TYPE_AutoInventory EQMACMQ_REAL_AutoInventory = NULL;

EQ_FUNCTION_TYPE_CSpellBookWnd__StartSpellMemorization EQMACMQ_REAL_CSpellBookWnd__StartSpellMemorization = NULL;
EQ_FUNCTION_TYPE_CSpellBookWnd__FinishMemorizing       EQMACMQ_REAL_CSpellBookWnd__FinishMemorizing       = NULL;

EQ_FUNCTION_TYPE_CLootWnd__Deactivate EQMACMQ_REAL_CLootWnd__Deactivate = NULL;

bool g_killSwitchIsEnabled = true;
bool g_loggingIsEnabled = false;
bool g_standWhenMovingIsEnabled = true;
bool g_standWhenCastSpellIsEnabled = true;
bool g_closeLootWindowWhenMovingIsEnabled = true;
bool g_hideCorpseLootedIsEnabled = false;

bool g_boxChatIsEnabled = true;

bool g_boxChatAutoConnectIsEnabled = true;

bool g_boxChatIsConnected          = false;
bool g_boxChatSendConnectedMessage = false;

bool g_boxChatShowConnectedMessage    = false;
bool g_boxChatShowDisconnectedMessage = false;

unsigned int g_boxChatKeepAliveTimer = 0;
unsigned int g_boxChatKeepAliveDelay = 30000; // 30 seconds

#define EQMACMQ_BOX_CHAT_SERVER_IP_ADDRESS "127.0.0.1"
#define EQMACMQ_BOX_CHAT_SERVER_PORT       "29015"

#define EQMACMQ_BOX_CHAT_RECV_MAX 4096
#define EQMACMQ_BOX_CHAT_SEND_MAX 4096

#define EQMACMQ_BOX_CHAT_TEXT_MAX 4096

#define EQMACMQ_BOX_CHAT_COMMANDS_MAX 32

char g_boxChatServerIpAddress[128];
char g_boxChatServerPort[128];

char g_boxChatRecvBuffer[EQMACMQ_BOX_CHAT_RECV_MAX] = {0};

char g_boxChatSendCommandList[EQMACMQ_BOX_CHAT_COMMANDS_MAX][EQMACMQ_BOX_CHAT_TEXT_MAX] = {0};

char g_boxChatInterpretCommandList[EQMACMQ_BOX_CHAT_COMMANDS_MAX][EQMACMQ_BOX_CHAT_TEXT_MAX] = {0};

char g_boxChatInterpretCommandListEx[EQMACMQ_BOX_CHAT_COMMANDS_MAX][EQMACMQ_BOX_CHAT_TEXT_MAX] = {0};

bool g_ignoreKeysIsEnabled = true;

unsigned int g_ignoreKeysTimer = 0;
unsigned int g_ignoreKeysDelay = 10;

int g_zoneId = 0;

int g_numDeferred2dItems = 0;

float g_maxDeferred2dItemsPercent = 0.75f; // use 3000/4000 max

// based on font Arial 14 pointer
int g_fontHeight                  = 14;
int g_fontHeightPixels            = 10;
int g_fontHeightEmptyPixelsTop    = 3;
int g_fontHeightEmptyPixelsBottom = 1;

float g_elementOffset = 5.0f;

DWORD g_eqProcesses[32];
unsigned int g_eqProcessesCount = 0;

bool g_slashCommandsIsEnabled = true;

bool g_mouseWheelZoomIsEnabled = true;
float g_mouseWheelZoomMultiplier = 1.0f;

bool g_mouseIsDragging = false;

float g_mouseDraggingBeginX = 0.0f;
float g_mouseDraggingBeginY = 0.0f;

bool g_writeTextToChatWindowIsEnabled = true;

unsigned int g_playerLevelCurrent       = 0;
unsigned int g_playerLevelPrevious      = 0;
unsigned int g_playerExperienceCurrent  = 0;
unsigned int g_playerExperiencePrevious = 0;

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

bool g_hudButtonsIsEnabled = true;

float g_hudButtonsX = 138.0;
float g_hudButtonsY = 5.0f;

float g_hudButtonExitX;
float g_hudButtonExitY;

float g_hudButtonReloadConfigX;
float g_hudButtonReloadConfigY;

float g_hudButtonToggleClientSwitcherX;
float g_hudButtonToggleClientSwitcherY;

float g_hudButtonToggleMapX;
float g_hudButtonToggleMapY;

float g_hudButtonToggleEspX;
float g_hudButtonToggleEspY;

float g_hudButtonToggleEspTextX;
float g_hudButtonToggleEspTextY;

float g_hudButtonToggleEspSkeletonsX;
float g_hudButtonToggleEspSkeletonsY;

float g_hudButtonToggleEspGroundSpawnsX;
float g_hudButtonToggleEspGroundSpawnsY;

float g_hudButtonToggleEspDoorSpawnsX;
float g_hudButtonToggleEspDoorSpawnsY;

float g_hudButtonToggleEspDistanceX;
float g_hudButtonToggleEspDistanceY;

float g_hudButtonToggleEspFilterNpcX;
float g_hudButtonToggleEspFilterNpcY;

float g_hudButtonToggleHealthBarsX;
float g_hudButtonToggleHealthBarsY;

float g_hudButtonToggleBuffInfoX;
float g_hudButtonToggleBuffInfoY;

float g_hudButtonTogglePlayerInfoX;
float g_hudButtonTogglePlayerInfoY;

float g_hudButtonToggleTargetInfoX;
float g_hudButtonToggleTargetInfoY;

bool g_mapIsEnabled = true;

bool g_mapIsMaximized = false;

unsigned int g_mapNumLayers = 3;

bool g_mapLayer0IsEnabled = true;
bool g_mapLayer1IsEnabled = true;
bool g_mapLayer2IsEnabled = true;
bool g_mapLayer3IsEnabled = true;

bool g_mapSpawnClickToTargetIsEnabled = false;

bool g_mapArrowIsEnabled              = true;
bool g_mapZoneInfoIsEnabled           = true;
bool g_mapHeightFilterIsEnabled       = false;
bool g_mapHeightFilterSpawnsIsEnabled = false;
bool g_mapRotateIsEnabled             = false;

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
char g_mapSpawnFilterNpc[4096] = {0};

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

float g_mapOffsetX = 0.0f;
float g_mapOffsetY = 0.0f;

float g_mapHeightFilterLow  = 10.0f; // z axis
float g_mapHeightFilterHigh = 10.0f; // z axis

float g_mapZoom           = 1.0f;
float g_mapZoomMultiplier = 0.05f; // zoom speed
float g_mapZoomDefault    = 1.0f;
float g_mapZoomMinimum    = 10.0f;
float g_mapZoomMaximum    = 0.01f;

bool g_mapMouseWheelZoomIsEnabled = true;
float g_mapMouseWheelZoomMultiplier = 0.5f; // mouse wheel zoom speed

float g_mapArrowRadius = 20.0f;

float g_mapCenterLineSize = 5.0f;

DWORD g_mapBorderColor     = 0xFF00FF00;
DWORD g_mapBackgroundColor = 0x00000000;
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

bool g_mapButtonsIsEnabled = true;

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

float g_mapButtonResetZoomAndCenterX;
float g_mapButtonResetZoomAndCenterY;

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

bool g_espClipToViewPortIsEnabled = true;

bool g_espSkeletonsIsEnabled = false; // like StickFigures=1 in eqclient.ini

bool g_espSkeletonsLineOfSightIsEnabled = false; // require the spawn to be out of line of sight to be drawn

bool g_espSkeletonsDistanceIsEnabled = true;
float g_espSkeletonsDistance = 200.0f;

int g_espSkeletonsDefaultLineColor     = 0xFFFF8000;
int g_espSkeletonsPlayerLineColor      = 0xFF800000;
int g_espSkeletonsPlayerCorpseLineColor= 0xFF808000;
int g_espSkeletonsNpcLineColor         = 0xFF008080;
int g_espSkeletonsNpcCorpseLineColor   = 0xFF808000;
int g_espSkeletonsGroupMemberLineColor = 0xFF008000;
int g_espSkeletonsTargetLineColor      = 0xFF800080;
int g_espSkeletonsGameMasterLineColor  = 0xFF800080;

bool g_espShowYourselfIsEnabled         = true;
bool g_espShowPlayerIsEnabled           = true;
bool g_espShowPlayerCorpseIsEnabled     = true;
bool g_espShowNpcIsEnabled              = true;
bool g_espShowNpcCorpseIsEnabled        = true;
bool g_espShowGroundSpawnsIsEnabled     = true;
bool g_espShowDoorSpawnsIsEnabled       = false;
bool g_espShowUniqueDoorSpawnsIsEnabled = true; // POK books, POK portals, etc

bool g_espShowTextIsEnabled = true; // show/hide all ESP text

bool g_espShowTextOnMouseHoverIsEnabled = true;

bool g_espShowTextDistanceIsEnabled           = true;
bool g_espShowTextLevelIsEnabled              = true;
bool g_espShowTextYourselfHpIsEnabled         = true;
bool g_espShowTextYourselfManaIsEnabled       = true;
bool g_espShowTextIsGameMasterIsEnabled       = true;
bool g_espShowTextIsPlayerKillIsEnabled       = true;
bool g_espShowTextIsAwayFromKeyboardIsEnabled = true;
bool g_espShowTextIsLookingForGroupIsEnabled  = true;
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
char g_espFilterNpc[4096] = {0};

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

bool g_buffInfoIsEnabled = true;

float g_buffInfoX = 768.0f;
float g_buffInfoY = 32.0f;

int g_buffInfoTextColor            = EQ_TEXT_COLOR_YELLOW;
int g_buffInfoBeneficialTextColor  = EQ_TEXT_COLOR_YELLOW;
int g_buffInfoDetrimentalTextColor = EQ_TEXT_COLOR_RED;

char g_buffInfoTextColor_string[32];
char g_buffInfoBeneficialTextColor_string[32];
char g_buffInfoDetrimentalTextColor_string[32];

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

bool g_messageTextOnChatWindowIsEnabled       = true;
bool g_messageTextOnGainedExperienceIsEnabled = true;

char g_messageText[128];
int g_messageTextColor = EQ_TEXT_COLOR_WHITE;

float g_messageTextX = 512.0f;
float g_messageTextY = 256.0f;

unsigned int g_messageTextTimer = 0;
unsigned int g_messageTextDelay = 5000; // delay in milliseconds

bool g_itemDisplayWindowTextIsEnabled = true;

bool g_itemDisplayWindowTextItemsIsEnabled  = true;
bool g_itemDisplayWindowTextSpellsIsEnabled = true;

bool g_buffWindowTimersIsEnabled = true;

bool g_healthBarsIsEnabled = true;

bool g_healthBarsClipToViewPortIsEnabled = true;

bool g_healthBarsClickToTargetIsEnabled = false;

float g_healthBarsWidth  = 50.0f;
float g_healthBarsHeight = 10.0f;

#define EQMACMQ_HEALTH_BAR_DEFAULT_BACKGROUND_COLOR 0xC8000000
#define EQMACMQ_HEALTH_BAR_DEFAULT_FOREGROUND_COLOR 0xC8FF0000
#define EQMACMQ_HEALTH_BAR_DEFAULT_MANA_COLOR       0xC800FFFF

DWORD g_healthBarsBackgroundColor       = 0xC8000000;
DWORD g_healthBarsForegroundPlayerColor = 0xC800FF00;
DWORD g_healthBarsForegroundNpcColor    = 0xC8FF0000;
DWORD g_healthBarsForegroundTargetColor = 0xC8FF00FF;
DWORD g_healthBarsManaColor             = 0xC800FFFF;

bool g_healthBarsDistanceIsEnabled = true;
float g_healthBarsDistance = 400.0f;

bool g_clientSwitcherIsEnabled = true;

bool g_clientSwitcherSyncMouseIsEnabled = true;

bool g_clientSwitcherButtonsIsEnabled = true;

bool g_clientSwitcherHotkeysIsEnabled = true;

bool g_clientSwitcherHotkeyTildeIsEnabled   = false;
bool g_clientSwitcherHotkeyNumpad0IsEnabled = true;
bool g_clientSwitcherHotkeyNumpad1IsEnabled = true;
bool g_clientSwitcherHotkeyNumpad2IsEnabled = true;
bool g_clientSwitcherHotkeyNumpad3IsEnabled = true;
bool g_clientSwitcherHotkeyNumpad4IsEnabled = true;
bool g_clientSwitcherHotkeyNumpad5IsEnabled = true;
bool g_clientSwitcherHotkeyNumpad6IsEnabled = true;

DWORD g_clientSwitcherUpdateProcessesTimer = 0;
DWORD g_clientSwitcherUpdateProcessesDelay = 1000;

float g_clientSwitcherButtonRefreshX;
float g_clientSwitcherButtonRefreshY;

float g_clientSwitcherButton1X;
float g_clientSwitcherButton1Y;

float g_clientSwitcherButton2X;
float g_clientSwitcherButton2Y;

float g_clientSwitcherButton3X;
float g_clientSwitcherButton3Y;

bool g_hacksIsEnabled = true;

#define EQMACMQ_HACKS_MAX 256

typedef struct _EQMACMQHACK
{
    struct _EQMACMQHACK* Next;
    DWORD Index;
    CHAR Name[128];
    CHAR Filename[1024];
    CHAR Description[1024];
    BOOL IsEnabled;
} EQMACMQHACK, *PEQMACMQHACK;

struct _EQMACMQHACK* g_hackListBegin = NULL;

bool g_nameTextIsEnabled = true;

bool g_nameTextNpcLevelAndClassIsEnabled = true;

bool g_nameColorsIsEnabled = true;

bool g_nameColorsBankerIsEnabled       = true;
bool g_nameColorsMerchantIsEnabled     = true;
bool g_nameColorsGuildmasterIsEnabled  = true;
bool g_nameColorsPlayerCorpseIsEnabled = false;
bool g_nameColorsNpcCorpseIsEnabled    = false;

int g_nameColorsBanker       = 0xFF0080;
int g_nameColorsMerchant     = 0xFFFF00;
int g_nameColorsGuildmaster  = 0xFF8000;
int g_nameColorsPlayerCorpse = 0x808080;
int g_nameColorsNpcCorpse    = 0x808080;

bool g_limitCpuUsageIsEnabled = true;

int g_limitCpuUsageForegroundDelay = 1;
int g_limitCpuUsageBackgroundDelay = 10;

bool g_speedHackIsEnabled = true;

float g_speedHackModifier = 0.0f;

bool g_bardTwistIsEnabled = false;

int g_bardTwistSongIndex = 0;

const int g_bardTwistSongNumbersMax = 10;

int g_bardTwistSongNumbers[g_bardTwistSongNumbersMax] = {-1};

unsigned int g_bardTwistTimer = 0;
unsigned int g_bardTwistDelay = 3300; // 3.3 seconds ; 3 second cast time, 0.3 seconds to account for latency

bool g_freeCameraIsEnabled = false;

float g_freeCameraMultiplier = 1.0f;

bool g_meleeIsEnabled = false;

bool g_meleeRangeIsEnabled = false;

unsigned int g_meleeTimer = 0;
unsigned int g_meleeDelay = 1000;

bool g_meleeSkillTauntIsEnabled = false;
bool g_meleeSkillKickIsEnabled = false;
bool g_meleeSkillBashIsEnabled = false;
bool g_meleeSkillSlamIsEnabled = false;
bool g_meleeSkillDisarmIsEnabled = false;
bool g_meleeSkillSenseHeadingIsEnabled = false;

char g_loadSpellsetName[128] = {0};
bool g_loadSpellsetIsInProgress = false;

bool g_lootAllIsInProgress = false;

bool g_lootAllNoDropIsEnabled = false;

bool g_lootAllFilterIdIsEnabled = false;
char g_lootAllFilterId[4096] = {0};

bool g_lootAllFilterNameIsEnabled = false;
char g_lootAllFilterName[4096] = {0};

unsigned int g_lootAllCounter = 0;

unsigned int g_lootAllTimer = 0;
unsigned int g_lootAllDelay = 1000;

bool g_skillHackIsEnabled = true;

unsigned int g_skillHackSwimming = 200;
unsigned int g_skillHackTracking = 200;
unsigned int g_skillHackSenseHeading = 200;

// Cohen-Sutherland algorithm
// http://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
// need this to clip lines within the map window rectangle
#define EQMACMQ_LINECLIP_INSIDE 0 // 0000
#define EQMACMQ_LINECLIP_LEFT   1 // 0001
#define EQMACMQ_LINECLIP_RIGHT  2 // 0010
#define EQMACMQ_LINECLIP_BOTTOM 4 // 0100
#define EQMACMQ_LINECLIP_TOP    8 // 1000

// function prototypes

void EQMACMQ_EnableDebugPrivileges();
bool EQMACMQ_IsForegroundWindowCurrentProcessId();

void EQMACMQ_DoUnload();

void EQMACMQ_ReloadConfig();
void EQMACMQ_ReloadMap();
void EQMACMQ_DoLoadConfig();
void EQMACMQ_DoLoadMap();

void EQMACMQ_DoHacksLoad(bool load);
void EQMACMQ_DoFreeCamera(bool enable);

int __fastcall EQMACMQ_DETOUR_CDisplay__SetNameSpriteState(void* this_ptr, void* not_used, class EQPlayer* a1, bool a2);
int __fastcall EQMACMQ_DETOUR_CDisplay__SetNameSpriteTint(void* this_ptr, void* not_used, class EQPlayer* a1);

void EQMACMQ_Log(const char* text)
{
    if (g_loggingIsEnabled == false)
    {
        return;
    }

    FILE* file;
    errno_t fileErrorNumber = fopen_s(&file, "eqmac_mq.log", "a");

    if (file)
    {
        fprintf(file, "[%s] %s\n", __TIME__, text);
        fclose(file);
    }
}

void EQMACMQ_EnableDebugPrivileges()
{
    HANDLE token;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
    {
        TOKEN_PRIVILEGES tp;
        TOKEN_PRIVILEGES tpPrevious;

        DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);

        LUID luid;

        if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
        {
            tp.PrivilegeCount           = 1;
            tp.Privileges[0].Luid       = luid;
            tp.Privileges[0].Attributes = 0;

            AdjustTokenPrivileges
            (
                token,
                FALSE,
                &tp,
                sizeof(TOKEN_PRIVILEGES),
                &tpPrevious,
                &cbPrevious
            );

            tpPrevious.PrivilegeCount            = 1;
            tpPrevious.Privileges[0].Luid        = luid;
            tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
    
            AdjustTokenPrivileges
            (
                token,
                FALSE,
                &tpPrevious,
                cbPrevious,
                NULL,
                NULL
            );
        }
    }

    CloseHandle(token);
}

bool EQMACMQ_IsForegroundWindowCurrentProcessId()
{
    HWND foregroundHwnd = GetForegroundWindow();

    DWORD foregroundProcessId;
    GetWindowThreadProcessId(foregroundHwnd, &foregroundProcessId);

    return (foregroundProcessId == GetCurrentProcessId());
}

void EQMACMQ_UpdateEqProcesses()
{
    memset(g_eqProcesses, 0, sizeof(g_eqProcesses));

    unsigned int eqProcessIndex = 0;

    DWORD processes[1024];
    DWORD needed;
    if (!EnumProcesses(processes, sizeof(processes), &needed))
    {
        return;
    }

    DWORD numProcesses = needed / sizeof(DWORD);

    for (DWORD i = 0; i < numProcesses; i++)
    {
        if (processes[i] != 0)
        {
            HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processes[i]);

            if (processHandle != NULL)
            {
                HMODULE module;
                DWORD neededEx;
                if (EnumProcessModules(processHandle, &module, sizeof(module), &neededEx))
                {
                    char processName[128] = {0};
                    GetModuleBaseNameA(processHandle, module, processName, sizeof(processName));

                    if (strcmp(processName, EQ_STRING_PROCESS_NAME) == 0)
                    {
                        g_eqProcesses[eqProcessIndex] = processes[i];

                        AllowSetForegroundWindow(processes[i]);

                        eqProcessIndex++;
                    }
                }
            }

            CloseHandle(processHandle);
        }

        if (eqProcessIndex >= 3)
        {
            break;
        }
    }

    g_eqProcessesCount = 0;

    for (DWORD i = 0; i < sizeof(g_eqProcesses); i++)
    {
        if (g_eqProcesses[i] == 0)
        {
            break;
        }

         g_eqProcessesCount++;
    }
}

DWORD EQMACMQ_GetModuleBaseAddress(const wchar_t* moduleName)
{
    DWORD moduleBaseAddress = 0;

    DWORD processId = GetCurrentProcessId();

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);

    if (snapshot != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 ModuleEntry32 = {0};
        ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

        if (Module32First(snapshot, &ModuleEntry32))
        {
            do
            {
                if (wcscmp(ModuleEntry32.szModule, moduleName) == 0)
                {
                    moduleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                    break;
                }
            } while (Module32Next(snapshot, &ModuleEntry32));
        }

        CloseHandle(snapshot);
    }

    return moduleBaseAddress;
}

bool EQMACMQ_DoBoxChatConnect()
{
    closesocket(g_connectSocket);
    //WSACleanup();

    //if (WSAStartup(MAKEWORD(2,2), &g_wsaData) != 0)
    //{
        //return false;
    //}

    struct addrinfo *addrinfo_result = NULL;
    struct addrinfo *addrinfo_ptr    = NULL;
    struct addrinfo addrinfo_hints;

    ZeroMemory(&addrinfo_hints, sizeof(addrinfo_hints));
    addrinfo_hints.ai_family   = AF_INET;
    addrinfo_hints.ai_socktype = SOCK_STREAM;
    addrinfo_hints.ai_protocol = IPPROTO_TCP;

    if (strlen(g_boxChatServerIpAddress) == 0)
    {
        strncpy_s(g_boxChatServerIpAddress, sizeof(g_boxChatServerIpAddress), EQMACMQ_BOX_CHAT_SERVER_IP_ADDRESS, _TRUNCATE);
    }

    unsigned int portNumber = atoi(g_boxChatServerPort);

    // restricted port numbers, use default
    if (portNumber <= 1024 || strlen(g_boxChatServerPort) == 0)
    {
        strncpy_s(g_boxChatServerPort, sizeof(g_boxChatServerPort), EQMACMQ_BOX_CHAT_SERVER_PORT, _TRUNCATE);
    }

    if (getaddrinfo(g_boxChatServerIpAddress, g_boxChatServerPort, &addrinfo_hints, &addrinfo_result) != 0)
    {
        //WSACleanup();
        return false;
    }

    addrinfo_ptr = addrinfo_result;

    g_connectSocket = socket(addrinfo_ptr->ai_family, addrinfo_ptr->ai_socktype, addrinfo_ptr->ai_protocol);

    if (g_connectSocket == INVALID_SOCKET)
    {
        freeaddrinfo(addrinfo_result);
        //WSACleanup();
        return false;
    }

    if (connect(g_connectSocket, addrinfo_ptr->ai_addr, (int)addrinfo_ptr->ai_addrlen) != SOCKET_ERROR)
    {
        if (g_connectSocket != INVALID_SOCKET)
        {
            freeaddrinfo(addrinfo_result);
            return true;
        }
    }

    freeaddrinfo(addrinfo_result);

    closesocket(g_connectSocket);
    //WSACleanup();

    return false;
}

BOOL CALLBACK EQMACMQ_UpdateHwndProc(HWND hwnd, LPARAM lparam)
{
    if (lparam == 0)
    {
        return TRUE;
    }

    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);

    if (processId == lparam)
    {
        g_hwnd = hwnd;

        return FALSE;
    }

    g_hwnd = NULL;

    return TRUE;
}

void EQMACMQ_UpdateHwnd()
{
    EnumWindows(EQMACMQ_UpdateHwndProc, GetCurrentProcessId());
}

BOOL CALLBACK EQMACMQ_DoClientSwitchProc(HWND hwnd,LPARAM lparam)
{
    if (lparam == 0)
    {
        return TRUE;
    }

    char windowText[1024];
    GetWindowTextA(hwnd, windowText, sizeof(windowText));

    if (strstr(windowText, EQ_STRING_WINDOW_TITLE) == NULL)
    {
        if (strstr(windowText, EQ_STRING_WINDOW_TITLE_EQW) == NULL)
        {
            return TRUE;
        }
    }

    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);

    if (processId == lparam)
    {
        if (g_clientSwitcherSyncMouseIsEnabled == true)
        {
            WORD mouseX = EQ_ReadMemory<WORD>(EQ_MOUSE_X);
            WORD mouseY = EQ_ReadMemory<WORD>(EQ_MOUSE_Y);

            HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, processId);

            if (processHandle != NULL)
            {
                WriteProcessMemory(processHandle, (LPVOID)EQ_MOUSE_X_REAL, &mouseX, sizeof(mouseX), 0);
                WriteProcessMemory(processHandle, (LPVOID)EQ_MOUSE_Y_REAL, &mouseY, sizeof(mouseY), 0);
            }

            CloseHandle(processHandle);
        }

        ShowWindow(hwnd, SW_SHOW);
        SetForegroundWindow(hwnd);

        return FALSE;
    }

    return TRUE;
}

void EQMACMQ_DoClientSwitch(int processIndex)
{
    EnumWindows(EQMACMQ_DoClientSwitchProc, g_eqProcesses[processIndex]);
}

void EQMACMQ_DoClientSwitchPrevious()
{
    if (g_eqProcessesCount < 2)
    {
        return;
    }

    DWORD currentProcessId = GetCurrentProcessId();

    int processIndex = 0;

    for (DWORD i = 0; i < sizeof(g_eqProcesses); i++)
    {
        if (g_eqProcesses[i] == currentProcessId)
        {
            processIndex = i;
            break;
        }
    }

    if (processIndex == 0)
    {
        if (g_eqProcesses[2] > 0)
        {
            processIndex = 2;
        }
        else
        {
            processIndex = 1;
        }
    }
    else if (processIndex == 1)
    {
        if (g_eqProcesses[0] > 0)
        {
            processIndex = 0;
        }
        else
        {
            processIndex = 2;
        }
    }
    else if (processIndex == 2)
    {
        if (g_eqProcesses[1] > 0)
        {
            processIndex = 1;
        }
        else
        {
            processIndex = 0;
        }
    }

    EQMACMQ_DoClientSwitch(processIndex);
}

void EQMACMQ_DoClientSwitchNext()
{
    if (g_eqProcessesCount < 2)
    {
        return;
    }

    DWORD currentProcessId = GetCurrentProcessId();

    int processIndex = 0;

    for (DWORD i = 0; i < sizeof(g_eqProcesses); i++)
    {
        if (g_eqProcesses[i] == currentProcessId)
        {
            processIndex = i;
            break;
        }
    }

    if (processIndex == 0)
    {
        if (g_eqProcesses[1] > 0)
        {
            processIndex = 1;
        }
        else
        {
            processIndex = 2;
        }
    }
    else if (processIndex == 1)
    {
        if (g_eqProcesses[2] > 0)
        {
            processIndex = 2;
        }
        else
        {
            processIndex = 0;
        }
    }
    else if (processIndex == 2)
    {
        if (g_eqProcesses[0] > 0)
        {
            processIndex = 0;
        }
        else
        {
            processIndex = 1;
        }
    }

    EQMACMQ_DoClientSwitch(processIndex);
}

int EQMACMQ_GetLineClipValue(float x, float y, float minX, float minY, float maxX, float maxY)
{
    int value = EQMACMQ_LINECLIP_INSIDE;
 
    if (x < minX)
    {
        value |= EQMACMQ_LINECLIP_LEFT;
    }
    else if (x > maxX)
    {
        value |= EQMACMQ_LINECLIP_RIGHT;
    }

    if (y < minY)
    {
        value |= EQMACMQ_LINECLIP_BOTTOM;
    }
    else if (y > maxY)
    {
        value |= EQMACMQ_LINECLIP_TOP;
    }
 
    return value;
}

bool EQMACMQ_DoClipLine(EQLINE* line, float minX, float minY, float maxX, float maxY)
{
    bool bDrawLine = false;

    // don't clip lines into the rectangle, offset by 1 pixel
    minX = minX + 1.0f;
    minY = minY + 1.0f;
    maxX = maxX - 1.0f;
    maxY = maxY - 1.0f;

    int lineClipValue1 = EQMACMQ_GetLineClipValue(line->X1, line->Y1, minX, minY, maxX, maxY);
    int lineClipValue2 = EQMACMQ_GetLineClipValue(line->X2, line->Y2, minX, minY, maxX, maxY);

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
 
            if (lineClipValueOut & EQMACMQ_LINECLIP_TOP)
            {
                x = line->X1 + (line->X2 - line->X1) * (maxY - line->Y1) / (line->Y2 - line->Y1);
                y = maxY;
            }
            else if (lineClipValueOut & EQMACMQ_LINECLIP_BOTTOM)
            {
                x = line->X1 + (line->X2 - line->X1) * (minY - line->Y1) / (line->Y2 - line->Y1);
                y = minY;
            }
            else if (lineClipValueOut & EQMACMQ_LINECLIP_RIGHT)
            {
                y = line->Y1 + (line->Y2 - line->Y1) * (maxX - line->X1) / (line->X2 - line->X1);
                x = maxX;
            }
            else if (lineClipValueOut & EQMACMQ_LINECLIP_LEFT)
            {
                y = line->Y1 + (line->Y2 - line->Y1) * (minX - line->X1) / (line->X2 - line->X1);
                x = minX;
            }
 
            if (lineClipValueOut == lineClipValue1)
            {
                line->X1 = x;
                line->Y1 = y;
                lineClipValue1 = EQMACMQ_GetLineClipValue(line->X1, line->Y1, minX, minY, maxX, maxY);
            }
            else
            {
                line->X2 = x;
                line->Y2 = y;
                lineClipValue2 = EQMACMQ_GetLineClipValue(line->X2, line->Y2, minX, minY, maxX, maxY);
            }
        }
    }

    return bDrawLine;
}

bool EQMACMQ_IsPointInsideRectangle(int pointX, int pointY, int rectX, int rectY, int rectWidth, int rectHeight)
{
    if (pointX < rectX)                return false;
    if (pointY < rectY)                return false;
    if (pointX > (rectX + rectWidth))  return false;
    if (pointY > (rectY + rectHeight)) return false;

    return true;
}

bool EQMACMQ_IsDeferred2dItemsAtMax()
{
    if (g_numDeferred2dItems > (int)(EQ_GRAPHICS_DLL_DEFERRED_2D_ITEMS_MAX * g_maxDeferred2dItemsPercent))
    {
        return true;
    }

    return false;
}

bool EQMACMQ_FileExists(const char* filename)
{
    FILE* file;
    errno_t fileErrorNumber = fopen_s(&file, filename, "r");

    if (file)
    {
        fclose(file);
    }

    return (fileErrorNumber == 0);
}

int EQMACMQ_ConfigReadInt(const char* filename, const char* section, const char* key, int defaultValue)
{
    return GetPrivateProfileIntA(section, key, defaultValue, filename);
}

bool EQMACMQ_ConfigReadBool(const char* filename, const char* section, const char* key, int defaultValue)
{
    int result = GetPrivateProfileIntA(section, key, defaultValue, filename);

    return (result != 0);
}

float EQMACMQ_ConfigReadFloat(const char* filename, const char* section, const char* key, float defaultValue)
{
    char bufferDefault[128];
    _snprintf_s(bufferDefault, sizeof(bufferDefault), _TRUNCATE, "%f", defaultValue);

    char bufferResult[128];
    GetPrivateProfileStringA(section, key, bufferDefault, bufferResult, sizeof(bufferResult), filename);

    float result = (float)atof(bufferResult);

    return result;
}

void EQMACMQ_ConfigReadString(const char* filename, const char* section, const char* key, const char* defaultValue, char result[], size_t resultSize)
{
    GetPrivateProfileStringA(section, key, defaultValue, result, resultSize, filename);
}

bool EQMACMQ_LoadConfig(const char* filename)
{
    if (EQMACMQ_FileExists(filename) == false)
    {
        return false;
    }

    // Options

    g_loggingIsEnabled = EQMACMQ_ConfigReadBool(filename, "Options", "bLogging", g_loggingIsEnabled);

    g_maxDeferred2dItemsPercent = EQMACMQ_ConfigReadFloat(filename, "Options", "fMaxDeferred2dItemsPercent", g_maxDeferred2dItemsPercent);

    g_killSwitchIsEnabled = EQMACMQ_ConfigReadBool(filename, "Options", "bKillSwitch", g_killSwitchIsEnabled);

    g_slashCommandsIsEnabled = EQMACMQ_ConfigReadBool(filename, "Options", "bSlashCommands", g_slashCommandsIsEnabled);

    g_writeTextToChatWindowIsEnabled = EQMACMQ_ConfigReadBool(filename, "Options", "bWriteTextToChatWindow", g_writeTextToChatWindowIsEnabled);

    g_standWhenMovingIsEnabled = EQMACMQ_ConfigReadBool(filename, "Options", "bStandWhenMoving", g_standWhenMovingIsEnabled);

    g_standWhenCastSpellIsEnabled = EQMACMQ_ConfigReadBool(filename, "Options", "bStandWhenCastSpell", g_standWhenCastSpellIsEnabled);

    g_closeLootWindowWhenMovingIsEnabled = EQMACMQ_ConfigReadBool(filename, "Options", "bCloseLootWindowWhenMoving", g_closeLootWindowWhenMovingIsEnabled);

    // IgnoreKeys

    g_ignoreKeysIsEnabled = EQMACMQ_ConfigReadBool(filename, "IgnoreKeys", "bEnabled", g_ignoreKeysIsEnabled);

    // BoxChat

    g_boxChatIsEnabled = EQMACMQ_ConfigReadBool(filename, "BoxChat", "bEnabled", g_boxChatIsEnabled);

    g_boxChatAutoConnectIsEnabled = EQMACMQ_ConfigReadBool(filename, "BoxChat", "bAutoConnect", g_boxChatAutoConnectIsEnabled);

    EQMACMQ_ConfigReadString(filename, "BoxChat", "sServerIpAddress", EQMACMQ_BOX_CHAT_SERVER_IP_ADDRESS, g_boxChatServerIpAddress, sizeof(g_boxChatServerIpAddress));
    EQMACMQ_ConfigReadString(filename, "BoxChat", "sServerPort",      EQMACMQ_BOX_CHAT_SERVER_PORT,       g_boxChatServerPort,      sizeof(g_boxChatServerPort));

    g_boxChatKeepAliveDelay = EQMACMQ_ConfigReadInt(filename, "BoxChat", "iKeepAliveDelay", g_boxChatKeepAliveDelay);

    // HUD

    g_hudButtonsIsEnabled = EQMACMQ_ConfigReadBool(filename, "HUD", "bButtons", g_hudButtonsIsEnabled);

    g_hudButtonsX = EQMACMQ_ConfigReadFloat(filename, "HUD", "fButtonsX", g_hudButtonsX);
    g_hudButtonsY = EQMACMQ_ConfigReadFloat(filename, "HUD", "fButtonsY", g_hudButtonsY);

    g_buttonColorEnabled     = EQMACMQ_ConfigReadInt(filename, "HUD", "argbButtonColorEnabled",     g_buttonColorEnabled);
    g_buttonColorDisabled    = EQMACMQ_ConfigReadInt(filename, "HUD", "argbButtonColorDisabled",    g_buttonColorDisabled);
    g_buttonColorMinMaxClose = EQMACMQ_ConfigReadInt(filename, "HUD", "argbButtonColorMinMaxClose", g_buttonColorMinMaxClose);

    EQMACMQ_ConfigReadString(filename, "HUD", "sButtonTextColorEnabled",     "Light Green", g_buttonTextColorEnabled_string,     sizeof(g_buttonTextColorEnabled_string));
    EQMACMQ_ConfigReadString(filename, "HUD", "sButtonTextColorDisabled",    "Red",         g_buttonTextColorDisabled_string,    sizeof(g_buttonTextColorDisabled_string));
    EQMACMQ_ConfigReadString(filename, "HUD", "sButtonTextColorMinMaxClose", "Pink",        g_buttonTextColorMinMaxClose_string, sizeof(g_buttonTextColorMinMaxClose_string));
    g_buttonTextColorEnabled     = EQ_GetTextColorIdByName(g_buttonTextColorEnabled_string);
    g_buttonTextColorDisabled    = EQ_GetTextColorIdByName(g_buttonTextColorDisabled_string);
    g_buttonTextColorMinMaxClose = EQ_GetTextColorIdByName(g_buttonTextColorMinMaxClose_string);

    // MouseWheelZoom

    g_mouseWheelZoomIsEnabled = EQMACMQ_ConfigReadBool(filename, "MouseWheelZoom", "bEnabled", g_mouseWheelZoomIsEnabled);

    g_mouseWheelZoomMultiplier = EQMACMQ_ConfigReadFloat(filename, "MouseWheelZoom", "fMultiplier", g_mouseWheelZoomMultiplier);

    // ClientSwitcher

    g_clientSwitcherIsEnabled = EQMACMQ_ConfigReadBool(filename, "ClientSwitcher", "bEnabled", g_clientSwitcherIsEnabled);

    g_clientSwitcherSyncMouseIsEnabled = EQMACMQ_ConfigReadBool(filename, "ClientSwitcher", "bSyncMouse", g_clientSwitcherSyncMouseIsEnabled);

    g_clientSwitcherButtonsIsEnabled = EQMACMQ_ConfigReadBool(filename, "ClientSwitcher", "bButtons", g_clientSwitcherButtonsIsEnabled);

    g_clientSwitcherHotkeysIsEnabled = EQMACMQ_ConfigReadBool(filename, "ClientSwitcher", "bHotkeys", g_clientSwitcherHotkeysIsEnabled);

    g_clientSwitcherHotkeyTildeIsEnabled   = EQMACMQ_ConfigReadBool(filename, "ClientSwitcher", "bHotkeyTilde",   g_clientSwitcherHotkeyTildeIsEnabled);
    g_clientSwitcherHotkeyNumpad0IsEnabled = EQMACMQ_ConfigReadBool(filename, "ClientSwitcher", "bHotkeyNumpad0", g_clientSwitcherHotkeyNumpad0IsEnabled);
    g_clientSwitcherHotkeyNumpad1IsEnabled = EQMACMQ_ConfigReadBool(filename, "ClientSwitcher", "bHotkeyNumpad1", g_clientSwitcherHotkeyNumpad1IsEnabled);
    g_clientSwitcherHotkeyNumpad2IsEnabled = EQMACMQ_ConfigReadBool(filename, "ClientSwitcher", "bHotkeyNumpad2", g_clientSwitcherHotkeyNumpad2IsEnabled);
    g_clientSwitcherHotkeyNumpad3IsEnabled = EQMACMQ_ConfigReadBool(filename, "ClientSwitcher", "bHotkeyNumpad3", g_clientSwitcherHotkeyNumpad3IsEnabled);
    g_clientSwitcherHotkeyNumpad4IsEnabled = EQMACMQ_ConfigReadBool(filename, "ClientSwitcher", "bHotkeyNumpad4", g_clientSwitcherHotkeyNumpad4IsEnabled);
    g_clientSwitcherHotkeyNumpad5IsEnabled = EQMACMQ_ConfigReadBool(filename, "ClientSwitcher", "bHotkeyNumpad5", g_clientSwitcherHotkeyNumpad5IsEnabled);
    g_clientSwitcherHotkeyNumpad6IsEnabled = EQMACMQ_ConfigReadBool(filename, "ClientSwitcher", "bHotkeyNumpad6", g_clientSwitcherHotkeyNumpad6IsEnabled);

    // LimitCpuUsage

    g_limitCpuUsageIsEnabled = EQMACMQ_ConfigReadBool(filename, "LimitCpuUsage", "bEnabled", g_limitCpuUsageIsEnabled);

    g_limitCpuUsageForegroundDelay = EQMACMQ_ConfigReadInt(filename, "LimitCpuUsage", "iForegroundDelay", g_limitCpuUsageForegroundDelay);
    g_limitCpuUsageBackgroundDelay = EQMACMQ_ConfigReadInt(filename, "LimitCpuUsage", "iBackgroundDelay", g_limitCpuUsageBackgroundDelay);

    // Map

    g_mapIsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bEnabled", g_mapIsEnabled);

    g_mapIsMaximized = EQMACMQ_ConfigReadBool(filename, "Map", "bIsMaximized", g_mapIsMaximized);

    g_mapButtonsIsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bButtons", g_mapButtonsIsEnabled);

    g_mapLayer0IsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bLayer0", g_mapLayer0IsEnabled);
    g_mapLayer1IsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bLayer1", g_mapLayer1IsEnabled);
    g_mapLayer2IsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bLayer2", g_mapLayer2IsEnabled);
    g_mapLayer3IsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bLayer3", g_mapLayer3IsEnabled);

    g_mapSpawnClickToTargetIsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bSpawnClickToTarget", g_mapSpawnClickToTargetIsEnabled);

    g_mapArrowIsEnabled        = EQMACMQ_ConfigReadBool(filename, "Map", "bArrow",        g_mapArrowIsEnabled);
    g_mapZoneInfoIsEnabled     = EQMACMQ_ConfigReadBool(filename, "Map", "bZoneInfo",     g_mapZoneInfoIsEnabled);
    g_mapHeightFilterIsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bHeightFilter", g_mapHeightFilterIsEnabled);
    g_mapRotateIsEnabled       = EQMACMQ_ConfigReadBool(filename, "Map", "bRotate",       g_mapRotateIsEnabled);

    g_mapSpawnsIsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bSpawns", g_mapSpawnsIsEnabled);
    g_mapPointsIsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bPoints", g_mapPointsIsEnabled);
    g_mapLinesIsEnabled  = EQMACMQ_ConfigReadBool(filename, "Map", "bLines",  g_mapLinesIsEnabled);

    g_mapTargetLineIsEnabled         = EQMACMQ_ConfigReadBool(filename, "Map", "bTargetLine",         g_mapTargetLineIsEnabled);
    g_mapYourselfCorpseLineIsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bYourselfCorpseLine", g_mapYourselfCorpseLineIsEnabled);

    g_mapReplaceBlackLinesWithWhiteLinesIsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bReplaceBlackLinesWithWhiteLines", g_mapReplaceBlackLinesWithWhiteLinesIsEnabled);

    g_mapShowPlayerIsEnabled       = EQMACMQ_ConfigReadBool(filename, "Map", "bShowPlayer",       g_mapShowPlayerIsEnabled);
    g_mapShowPlayerCorpseIsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bShowPlayerCorpse", g_mapShowPlayerCorpseIsEnabled);
    g_mapShowNpcIsEnabled          = EQMACMQ_ConfigReadBool(filename, "Map", "bShowNpc",          g_mapShowNpcIsEnabled);
    g_mapShowNpcCorpseIsEnabled    = EQMACMQ_ConfigReadBool(filename, "Map", "bShowNpcCorpse",    g_mapShowNpcCorpseIsEnabled);

    g_mapSpawnDistanceIsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bSpawnDistance", g_mapSpawnDistanceIsEnabled);
    g_mapSpawnDistance = EQMACMQ_ConfigReadFloat(filename, "Map", "fSpawnDistance", g_mapSpawnDistance);

    g_mapSpawnDistancePlayerIsEnabled       = EQMACMQ_ConfigReadBool(filename, "Map", "bSpawnDistancePlayer",       g_mapSpawnDistancePlayerIsEnabled);
    g_mapSpawnDistancePlayerCorpseIsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bSpawnDistancePlayerCorpse", g_mapSpawnDistancePlayerCorpseIsEnabled);
    g_mapSpawnDistanceNpcIsEnabled          = EQMACMQ_ConfigReadBool(filename, "Map", "bSpawnDistanceNpc",          g_mapSpawnDistanceNpcIsEnabled);
    g_mapSpawnDistanceNpcCorpseIsEnabled    = EQMACMQ_ConfigReadBool(filename, "Map", "bSpawnDistanceNpcCorpse",    g_mapSpawnDistanceNpcCorpseIsEnabled);

    g_mapSpawnFilterNpcIsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bSpawnFilterNpc", g_mapSpawnFilterNpcIsEnabled);
    EQMACMQ_ConfigReadString(filename, "Map", "sSpawnFilterNpc", "", g_mapSpawnFilterNpc, sizeof(g_mapSpawnFilterNpc));

    g_mapDefaultX = EQMACMQ_ConfigReadFloat(filename, "Map", "fDefaultX", g_mapDefaultX);
    g_mapDefaultY = EQMACMQ_ConfigReadFloat(filename, "Map", "fDefaultY", g_mapDefaultY);

    g_mapDefaultWidth  = EQMACMQ_ConfigReadFloat(filename, "Map", "fDefaultWidth",  g_mapDefaultWidth);
    g_mapDefaultHeight = EQMACMQ_ConfigReadFloat(filename, "Map", "fDefaultHeight", g_mapDefaultHeight);

    g_mapMaximizedX = EQMACMQ_ConfigReadFloat(filename, "Map", "fMaximizedX", g_mapMaximizedX);
    g_mapMaximizedY = EQMACMQ_ConfigReadFloat(filename, "Map", "fMaximizedY", g_mapMaximizedY);

    g_mapMaximizedWidth  = EQMACMQ_ConfigReadFloat(filename, "Map", "fMaximizedWidth",  g_mapMaximizedWidth);
    g_mapMaximizedHeight = EQMACMQ_ConfigReadFloat(filename, "Map", "fMaximizedHeight", g_mapMaximizedHeight);

    g_mapHeightFilterLow  = EQMACMQ_ConfigReadFloat(filename, "Map", "fHeightFilterLow",  g_mapHeightFilterLow);
    g_mapHeightFilterHigh = EQMACMQ_ConfigReadFloat(filename, "Map", "fHeightFilterHigh", g_mapHeightFilterHigh);

    g_mapZoom           = EQMACMQ_ConfigReadFloat(filename, "Map", "fZoom",           g_mapZoom);
    g_mapZoomMultiplier = EQMACMQ_ConfigReadFloat(filename, "Map", "fZoomMultiplier", g_mapZoomMultiplier);
    g_mapZoomMinimum    = EQMACMQ_ConfigReadFloat(filename, "Map", "fZoomMinimum",    g_mapZoomMinimum);
    g_mapZoomMaximum    = EQMACMQ_ConfigReadFloat(filename, "Map", "fZoomMaximum",    g_mapZoomMaximum);

    g_mapMouseWheelZoomIsEnabled = EQMACMQ_ConfigReadBool(filename, "Map", "bMouseWheelZoom", g_mapMouseWheelZoomIsEnabled);
    g_mapMouseWheelZoomMultiplier = EQMACMQ_ConfigReadFloat(filename, "Map", "fMouseWheelZoomMultiplier", g_mapMouseWheelZoomMultiplier);

    g_mapArrowRadius = EQMACMQ_ConfigReadFloat(filename, "Map", "fArrowRadius", g_mapArrowRadius);

    g_mapCenterLineSize = EQMACMQ_ConfigReadFloat(filename, "Map", "fCenterLineSize", g_mapCenterLineSize);

    g_mapBorderColor     = EQMACMQ_ConfigReadInt(filename, "Map", "argbBorderColor",       g_mapBorderColor);
    g_mapBackgroundColor = EQMACMQ_ConfigReadInt(filename, "Map", "argbBackgroundColor",   g_mapBackgroundColor);
    g_mapArrowColor      = EQMACMQ_ConfigReadInt(filename, "Map", "argbArrowColor",        g_mapArrowColor);
    g_mapLineColor       = EQMACMQ_ConfigReadInt(filename, "Map", "argbLineColor",         g_mapLineColor);
    g_mapTargetLineColor = EQMACMQ_ConfigReadInt(filename, "Map", "argbTargetLineColor",   g_mapTargetLineColor);
    g_mapCorpseLineColor = EQMACMQ_ConfigReadInt(filename, "Map", "argbCorpseLineColor",   g_mapCorpseLineColor);

    EQMACMQ_ConfigReadString(filename, "Map", "sDefaultTextColor",      "Light Green", g_mapDefaultTextColor_string,      sizeof(g_mapDefaultTextColor_string));
    EQMACMQ_ConfigReadString(filename, "Map", "sZoneInfoTextColor",     "Light Green", g_mapZoneInfoTextColor_string,     sizeof(g_mapZoneInfoTextColor_string));
    EQMACMQ_ConfigReadString(filename, "Map", "sPointTextColor",        "Light Gray",  g_mapPointTextColor_string,        sizeof(g_mapPointTextColor_string));
    EQMACMQ_ConfigReadString(filename, "Map", "sPlayerTextColor",       "Red",         g_mapPlayerTextColor_string,       sizeof(g_mapPlayerTextColor_string));
    EQMACMQ_ConfigReadString(filename, "Map", "sPlayerCorpseTextColor", "Yellow",      g_mapPlayerCorpseTextColor_string, sizeof(g_mapPlayerCorpseTextColor_string));
    EQMACMQ_ConfigReadString(filename, "Map", "sNpcTextColor",          "Cyan",        g_mapNpcTextColor_string,          sizeof(g_mapNpcTextColor_string));
    EQMACMQ_ConfigReadString(filename, "Map", "sNpcCorpseTextColor",    "Yellow",      g_mapNpcCorpseTextColor_string,    sizeof(g_mapNpcCorpseTextColor_string));
    EQMACMQ_ConfigReadString(filename, "Map", "sGroupMemberTextColor",  "Light Green", g_mapGroupMemberTextColor_string,  sizeof(g_mapGroupMemberTextColor_string));
    EQMACMQ_ConfigReadString(filename, "Map", "sTargetTextColor",       "Pink",        g_mapTargetTextColor_string,       sizeof(g_mapTargetTextColor_string));
    EQMACMQ_ConfigReadString(filename, "Map", "sGameMasterTextColor",   "Pink",        g_mapGameMasterTextColor_string,   sizeof(g_mapGameMasterTextColor_string));
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

    g_espIsEnabled = EQMACMQ_ConfigReadBool(filename, "ESP", "bEnabled", g_espIsEnabled);

    g_espClipToViewPortIsEnabled = EQMACMQ_ConfigReadBool(filename, "ESP", "bClipToViewPort", g_espClipToViewPortIsEnabled);

    g_espSkeletonsIsEnabled = EQMACMQ_ConfigReadBool(filename, "ESP", "bSkeletons", g_espSkeletonsIsEnabled);

    g_espSkeletonsLineOfSightIsEnabled = EQMACMQ_ConfigReadBool(filename, "ESP", "bSkeletonsLineOfSight", g_espSkeletonsLineOfSightIsEnabled);

    g_espSkeletonsDistanceIsEnabled = EQMACMQ_ConfigReadBool(filename, "ESP", "bSkeletonsDistance", g_espSkeletonsDistanceIsEnabled);
    g_espSkeletonsDistance = EQMACMQ_ConfigReadFloat(filename, "ESP", "fSkeletonsDistance", g_espSkeletonsDistance);

    g_espSkeletonsDefaultLineColor      = EQMACMQ_ConfigReadInt(filename, "ESP", "argbSkeletonsDefaultLineColor",      g_espSkeletonsDefaultLineColor);
    g_espSkeletonsPlayerLineColor       = EQMACMQ_ConfigReadInt(filename, "ESP", "argbSkeletonsPlayerLineColor",       g_espSkeletonsPlayerLineColor);
    g_espSkeletonsPlayerCorpseLineColor = EQMACMQ_ConfigReadInt(filename, "ESP", "argbSkeletonsPlayerCorpseLineColor", g_espSkeletonsPlayerCorpseLineColor);
    g_espSkeletonsNpcLineColor          = EQMACMQ_ConfigReadInt(filename, "ESP", "argbSkeletonsNpcLineColor",          g_espSkeletonsNpcLineColor);
    g_espSkeletonsNpcCorpseLineColor    = EQMACMQ_ConfigReadInt(filename, "ESP", "argbSkeletonsNpcCorpseLineColor",    g_espSkeletonsNpcCorpseLineColor);
    g_espSkeletonsGroupMemberLineColor  = EQMACMQ_ConfigReadInt(filename, "ESP", "argbSkeletonsGroupMemberLineColor",  g_espSkeletonsGroupMemberLineColor);
    g_espSkeletonsTargetLineColor       = EQMACMQ_ConfigReadInt(filename, "ESP", "argbSkeletonsTargetLineColor",       g_espSkeletonsTargetLineColor);
    g_espSkeletonsGameMasterLineColor   = EQMACMQ_ConfigReadInt(filename, "ESP", "argbSkeletonsGameMasterLineColor",   g_espSkeletonsGameMasterLineColor);

    g_espShowYourselfIsEnabled         = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowYourself",         g_espShowYourselfIsEnabled);
    g_espShowPlayerIsEnabled           = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowPlayer",           g_espShowPlayerIsEnabled);
    g_espShowPlayerCorpseIsEnabled     = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowPlayerCorpse",     g_espShowPlayerCorpseIsEnabled);
    g_espShowNpcIsEnabled              = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowNpc",              g_espShowNpcIsEnabled);
    g_espShowNpcCorpseIsEnabled        = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowNpcCorpse",        g_espShowNpcCorpseIsEnabled);
    g_espShowGroundSpawnsIsEnabled     = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowGroundSpawns",     g_espShowGroundSpawnsIsEnabled);
    g_espShowDoorSpawnsIsEnabled       = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowDoorSpawns",       g_espShowDoorSpawnsIsEnabled);
    g_espShowUniqueDoorSpawnsIsEnabled = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowUniqueDoorSpawns", g_espShowUniqueDoorSpawnsIsEnabled);

    g_espShowTextIsEnabled = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowText", g_espShowTextIsEnabled);

    g_espShowTextOnMouseHoverIsEnabled = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextOnMouseHover", g_espShowTextOnMouseHoverIsEnabled);

    g_espShowTextDistanceIsEnabled           = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextDistance",           g_espShowTextDistanceIsEnabled);
    g_espShowTextLevelIsEnabled              = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextLevel",              g_espShowTextLevelIsEnabled);
    g_espShowTextYourselfHpIsEnabled         = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextYourselfHp",         g_espShowTextYourselfHpIsEnabled);
    g_espShowTextYourselfManaIsEnabled       = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextYourselfMana",       g_espShowTextYourselfManaIsEnabled);
    g_espShowTextIsGameMasterIsEnabled       = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextIsGameMaster",       g_espShowTextIsGameMasterIsEnabled);
    g_espShowTextIsPlayerKillIsEnabled       = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextIsPlayerKill",       g_espShowTextIsPlayerKillIsEnabled);
    g_espShowTextIsLookingForGroupIsEnabled  = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextIsLookingForGroup",  g_espShowTextIsLookingForGroupIsEnabled);
    g_espShowTextIsAwayFromKeyboardIsEnabled = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextIsAwayFromKeyboard", g_espShowTextIsAwayFromKeyboardIsEnabled);
    g_espShowTextPlayerIsLinkDeadIsEnabled   = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextPlayerIsLinkDead",   g_espShowTextPlayerIsLinkDeadIsEnabled);
    g_espShowTextPlayerIsFeignDeathIsEnabled = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextPlayerIsFeignDeath", g_espShowTextPlayerIsFeignDeathIsEnabled);
    g_espShowTextPlayerGuildIsEnabled        = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextPlayerGuild",        g_espShowTextPlayerGuildIsEnabled);
    g_espShowTextPlayerClassIsEnabled        = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextPlayerClass",        g_espShowTextPlayerClassIsEnabled);
    g_espShowTextPlayerHpIsEnabled           = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextPlayerHp",           g_espShowTextPlayerHpIsEnabled);
    g_espShowTextNpcUniqueClassIsEnabled     = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextNpcUniqueClass",     g_espShowTextNpcUniqueClassIsEnabled);
    g_espShowTextNpcHpIsEnabled              = EQMACMQ_ConfigReadBool(filename, "ESP", "bShowTextNpcHp",              g_espShowTextNpcHpIsEnabled);

    g_espDistanceIsEnabled = EQMACMQ_ConfigReadBool(filename, "ESP", "bDistance", g_espDistanceIsEnabled);
    g_espDistance = EQMACMQ_ConfigReadFloat(filename, "ESP", "fDistance", g_espDistance);

    g_espDistancePlayerIsEnabled       = EQMACMQ_ConfigReadBool(filename, "ESP", "bDistancePlayer",       g_espDistancePlayerIsEnabled);
    g_espDistancePlayerCorpseIsEnabled = EQMACMQ_ConfigReadBool(filename, "ESP", "bDistancePlayerCorpse", g_espDistancePlayerCorpseIsEnabled);
    g_espDistanceNpcIsEnabled          = EQMACMQ_ConfigReadBool(filename, "ESP", "bDistanceNpc",          g_espDistanceNpcIsEnabled);
    g_espDistanceNpcCorpseIsEnabled    = EQMACMQ_ConfigReadBool(filename, "ESP", "bDistanceNpcCorpse",    g_espDistanceNpcCorpseIsEnabled);
    g_espDistanceGroundSpawnIsEnabled  = EQMACMQ_ConfigReadBool(filename, "ESP", "bDistanceGroundSpawn",  g_espDistanceGroundSpawnIsEnabled);
    g_espDistanceDoorSpawnIsEnabled    = EQMACMQ_ConfigReadBool(filename, "ESP", "bDistanceDoorSpawn",    g_espDistanceDoorSpawnIsEnabled);

    g_espFilterNpcIsEnabled = EQMACMQ_ConfigReadBool(filename, "ESP", "bFilterNpc", g_espFilterNpcIsEnabled);
    EQMACMQ_ConfigReadString(filename, "ESP", "sFilterNpc", "", g_espFilterNpc, sizeof(g_espFilterNpc));

    EQMACMQ_ConfigReadString(filename, "ESP", "sPlayerTextColor",       "Red",         g_espPlayerTextColor_string,       sizeof(g_espPlayerTextColor_string));
    EQMACMQ_ConfigReadString(filename, "ESP", "sPlayerCorpseTextColor", "Yellow",      g_espPlayerCorpseTextColor_string, sizeof(g_espPlayerCorpseTextColor_string));
    EQMACMQ_ConfigReadString(filename, "ESP", "sNpcTextColor",          "Cyan",        g_espNpcTextColor_string,          sizeof(g_espNpcTextColor_string));
    EQMACMQ_ConfigReadString(filename, "ESP", "sNpcCorpseTextColor",    "Yellow",      g_espNpcCorpseTextColor_string,    sizeof(g_espNpcCorpseTextColor_string));
    EQMACMQ_ConfigReadString(filename, "ESP", "sGroupMemberTextColor",  "Light Green", g_espGroupMemberTextColor_string,  sizeof(g_espGroupMemberTextColor_string));
    EQMACMQ_ConfigReadString(filename, "ESP", "sTargetTextColor",       "Pink",        g_espTargetTextColor_string,       sizeof(g_espTargetTextColor_string));
    EQMACMQ_ConfigReadString(filename, "ESP", "sGameMasterTextColor",   "Pink",        g_espGameMasterTextColor_string,   sizeof(g_espGameMasterTextColor_string));
    EQMACMQ_ConfigReadString(filename, "ESP", "sGroundSpawnTextColor",  "White",       g_espGroundSpawnTextColor_string,  sizeof(g_espGroundSpawnTextColor_string));
    EQMACMQ_ConfigReadString(filename, "ESP", "sDoorSpawnTextColor",    "White",       g_espDoorSpawnTextColor_string,    sizeof(g_espDoorSpawnTextColor_string));
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

    g_healthBarsIsEnabled = EQMACMQ_ConfigReadBool(filename, "HealthBars", "bEnabled", g_healthBarsIsEnabled);

    g_healthBarsClipToViewPortIsEnabled = EQMACMQ_ConfigReadBool(filename, "HealthBars", "bClipToViewPort", g_healthBarsClipToViewPortIsEnabled);

    g_healthBarsClickToTargetIsEnabled = EQMACMQ_ConfigReadBool(filename, "HealthBars", "bClickToTarget", g_healthBarsClickToTargetIsEnabled);

    g_healthBarsWidth  = EQMACMQ_ConfigReadFloat(filename, "HealthBars", "fWidth",  g_healthBarsWidth);
    g_healthBarsHeight = EQMACMQ_ConfigReadFloat(filename, "HealthBars", "fHeight", g_healthBarsHeight);

    g_healthBarsBackgroundColor       = EQMACMQ_ConfigReadInt(filename, "HealthBars", "argbBackgroundColor",       g_healthBarsBackgroundColor);
    g_healthBarsForegroundPlayerColor = EQMACMQ_ConfigReadInt(filename, "HealthBars", "argbForegroundPlayerColor", g_healthBarsForegroundPlayerColor);
    g_healthBarsForegroundNpcColor    = EQMACMQ_ConfigReadInt(filename, "HealthBars", "argbForegroundNpcColor",    g_healthBarsForegroundNpcColor);
    g_healthBarsForegroundTargetColor = EQMACMQ_ConfigReadInt(filename, "HealthBars", "argbForegroundTargetColor", g_healthBarsForegroundTargetColor);
    g_healthBarsManaColor             = EQMACMQ_ConfigReadInt(filename, "HealthBars", "argbManaColor",             g_healthBarsManaColor);

    g_healthBarsDistanceIsEnabled = EQMACMQ_ConfigReadBool(filename, "HealthBars", "bDistance", g_healthBarsDistanceIsEnabled);
    g_healthBarsDistance = EQMACMQ_ConfigReadFloat(filename, "HealthBars", "fDistance", g_healthBarsDistance);

    // MessageText

    g_messageTextIsEnabled = EQMACMQ_ConfigReadBool(filename, "MessageText", "bEnabled", g_messageTextIsEnabled);

    g_messageTextIsCentered = EQMACMQ_ConfigReadBool(filename, "MessageText", "bCentered", g_messageTextIsCentered);

    g_messageTextX = EQMACMQ_ConfigReadFloat(filename, "MessageText", "fX", g_messageTextX);
    g_messageTextY = EQMACMQ_ConfigReadFloat(filename, "MessageText", "fY", g_messageTextY);

    g_messageTextDelay = EQMACMQ_ConfigReadInt(filename, "MessageText", "iDelay", g_messageTextDelay);

    g_messageTextOnChatWindowIsEnabled       = EQMACMQ_ConfigReadBool(filename, "MessageText", "bOnChatWindow",       g_messageTextOnChatWindowIsEnabled);
    g_messageTextOnGainedExperienceIsEnabled = EQMACMQ_ConfigReadBool(filename, "MessageText", "bOnGainedExperience", g_messageTextOnGainedExperienceIsEnabled);

    // ItemDisplayWindowText

    g_itemDisplayWindowTextIsEnabled = EQMACMQ_ConfigReadBool(filename, "ItemDisplayWindowText", "bEnabled", g_itemDisplayWindowTextIsEnabled);

    g_itemDisplayWindowTextItemsIsEnabled  = EQMACMQ_ConfigReadBool(filename, "ItemDisplayWindowText", "bItems",  g_itemDisplayWindowTextItemsIsEnabled);
    g_itemDisplayWindowTextSpellsIsEnabled = EQMACMQ_ConfigReadBool(filename, "ItemDisplayWindowText", "bSpells", g_itemDisplayWindowTextSpellsIsEnabled);

    // BuffWindowTimers

    g_buffWindowTimersIsEnabled = EQMACMQ_ConfigReadBool(filename, "BuffWindowTimers", "bEnabled", g_buffWindowTimersIsEnabled);

    // BuffInfo

    g_buffInfoIsEnabled = EQMACMQ_ConfigReadBool(filename, "BuffInfo", "bEnabled", g_buffInfoIsEnabled);

    g_buffInfoX = EQMACMQ_ConfigReadFloat(filename, "BuffInfo", "fX", g_buffInfoX);
    g_buffInfoY = EQMACMQ_ConfigReadFloat(filename, "BuffInfo", "fY", g_buffInfoY);

    EQMACMQ_ConfigReadString(filename, "BuffInfo", "sTextColor",            "Yellow", g_buffInfoTextColor_string,            sizeof(g_buffInfoTextColor_string));
    EQMACMQ_ConfigReadString(filename, "BuffInfo", "sBeneficialTextColor",  "Yellow", g_buffInfoBeneficialTextColor_string,  sizeof(g_buffInfoBeneficialTextColor_string));
    EQMACMQ_ConfigReadString(filename, "BuffInfo", "sDetrimentalTextColor", "Red",    g_buffInfoDetrimentalTextColor_string, sizeof(g_buffInfoDetrimentalTextColor_string));
    g_buffInfoTextColor            = EQ_GetTextColorIdByName(g_buffInfoTextColor_string);
    g_buffInfoBeneficialTextColor  = EQ_GetTextColorIdByName(g_buffInfoBeneficialTextColor_string);
    g_buffInfoDetrimentalTextColor = EQ_GetTextColorIdByName(g_buffInfoDetrimentalTextColor_string);

    // PlayerInfo

    g_playerInfoIsEnabled = EQMACMQ_ConfigReadBool(filename, "PlayerInfo", "bEnabled", g_playerInfoIsEnabled);

    g_playerInfoX = EQMACMQ_ConfigReadFloat(filename, "PlayerInfo", "fX", g_playerInfoX);
    g_playerInfoY = EQMACMQ_ConfigReadFloat(filename, "PlayerInfo", "fY", g_playerInfoY);

    EQMACMQ_ConfigReadString(filename, "PlayerInfo", "sTextColor", "Cyan", g_playerInfoTextColor_string, sizeof(g_playerInfoTextColor_string));
    g_playerInfoTextColor = EQ_GetTextColorIdByName(g_playerInfoTextColor_string);

    // TargetInfo

    g_targetInfoIsEnabled = EQMACMQ_ConfigReadBool(filename, "TargetInfo", "bEnabled", g_targetInfoIsEnabled);

    g_targetInfoX = EQMACMQ_ConfigReadFloat(filename, "TargetInfo", "fX", g_targetInfoX);
    g_targetInfoY = EQMACMQ_ConfigReadFloat(filename, "TargetInfo", "fY", g_targetInfoY);

    EQMACMQ_ConfigReadString(filename, "TargetInfo", "sTextColor", "Pink", g_targetInfoTextColor_string, sizeof(g_targetInfoTextColor_string));
    g_targetInfoTextColor = EQ_GetTextColorIdByName(g_targetInfoTextColor_string);

    // NameText

    g_nameTextIsEnabled = EQMACMQ_ConfigReadBool(filename, "NameText", "bEnabled", g_nameTextIsEnabled);

    g_nameTextNpcLevelAndClassIsEnabled = EQMACMQ_ConfigReadBool(filename, "NameText", "bNpcLevelAndClass", g_nameTextNpcLevelAndClassIsEnabled);

    // NameColors

    g_nameColorsIsEnabled = EQMACMQ_ConfigReadBool(filename, "NameColors", "bEnabled", g_nameColorsIsEnabled);

    g_nameColorsBankerIsEnabled       = EQMACMQ_ConfigReadBool(filename, "NameColors", "bBanker",       g_nameColorsBankerIsEnabled);
    g_nameColorsMerchantIsEnabled     = EQMACMQ_ConfigReadBool(filename, "NameColors", "bMerchant",     g_nameColorsMerchantIsEnabled);
    g_nameColorsGuildmasterIsEnabled  = EQMACMQ_ConfigReadBool(filename, "NameColors", "bGuildmaster",  g_nameColorsGuildmasterIsEnabled);
    g_nameColorsPlayerCorpseIsEnabled = EQMACMQ_ConfigReadBool(filename, "NameColors", "bPlayerCorpse", g_nameColorsPlayerCorpseIsEnabled);
    g_nameColorsNpcCorpseIsEnabled    = EQMACMQ_ConfigReadBool(filename, "NameColors", "bNpcCorpse",    g_nameColorsNpcCorpseIsEnabled);

    g_nameColorsBanker       = EQMACMQ_ConfigReadInt(filename, "NameColors", "rgbBanker",       g_nameColorsBanker);
    g_nameColorsMerchant     = EQMACMQ_ConfigReadInt(filename, "NameColors", "rgbMerchant",     g_nameColorsMerchant);
    g_nameColorsGuildmaster  = EQMACMQ_ConfigReadInt(filename, "NameColors", "rgbGuildmaster",  g_nameColorsGuildmaster);
    g_nameColorsPlayerCorpse = EQMACMQ_ConfigReadInt(filename, "NameColors", "rgbPlayerCorpse", g_nameColorsPlayerCorpse);
    g_nameColorsNpcCorpse    = EQMACMQ_ConfigReadInt(filename, "NameColors", "rgbNpcCorpse",    g_nameColorsNpcCorpse);

    // Hacks

    g_hacksIsEnabled = EQMACMQ_ConfigReadBool(filename, "Hacks", "bEnabled", g_hacksIsEnabled);

    // SpeedHack

    g_speedHackIsEnabled = EQMACMQ_ConfigReadBool(filename, "SpeedHack", "bEnabled", g_speedHackIsEnabled);

    g_speedHackModifier = EQMACMQ_ConfigReadFloat(filename, "SpeedHack", "fModifier", g_speedHackModifier);

    // SkillHack

    g_skillHackIsEnabled = EQMACMQ_ConfigReadBool(filename, "SkillHack", "bEnabled", g_skillHackIsEnabled);

    g_skillHackSwimming = EQMACMQ_ConfigReadInt(filename, "SkillHack", "iSwimming", g_skillHackSwimming);
    g_skillHackTracking = EQMACMQ_ConfigReadInt(filename, "SkillHack", "iTracking", g_skillHackTracking);
    g_skillHackSenseHeading = EQMACMQ_ConfigReadInt(filename, "SkillHack", "iSenseHeading", g_skillHackSenseHeading);

    // BardTwist

    g_bardTwistDelay = EQMACMQ_ConfigReadInt(filename, "BardTwist", "iDelay", g_bardTwistDelay);

    // LootAll

    g_lootAllDelay = EQMACMQ_ConfigReadInt(filename, "LootAll", "iDelay", g_lootAllDelay);

    g_lootAllNoDropIsEnabled = EQMACMQ_ConfigReadBool(filename, "LootAll", "bNoDrop", g_lootAllNoDropIsEnabled);

    g_lootAllFilterIdIsEnabled = EQMACMQ_ConfigReadBool(filename, "LootAll", "bFilterId", g_lootAllFilterIdIsEnabled);

    EQMACMQ_ConfigReadString(filename, "LootAll", "sFilterId", "", g_lootAllFilterId, sizeof(g_lootAllFilterId));

    g_lootAllFilterNameIsEnabled = EQMACMQ_ConfigReadBool(filename, "LootAll", "bFilterName", g_lootAllFilterNameIsEnabled);

    EQMACMQ_ConfigReadString(filename, "LootAll", "sFilterName", "", g_lootAllFilterName, sizeof(g_lootAllFilterName));

    // FreeCamera

    g_freeCameraMultiplier = EQMACMQ_ConfigReadFloat(filename, "FreeCamera", "fMultiplier", g_freeCameraMultiplier);

    // Melee

    g_meleeIsEnabled = EQMACMQ_ConfigReadBool(filename, "Melee", "bEnabled", g_meleeIsEnabled);

    g_meleeRangeIsEnabled = EQMACMQ_ConfigReadBool(filename, "Melee", "bRange", g_meleeRangeIsEnabled);

    g_meleeDelay = EQMACMQ_ConfigReadInt(filename, "Melee", "iDelay", g_meleeDelay);

    g_meleeSkillTauntIsEnabled = EQMACMQ_ConfigReadBool(filename, "Melee", "bTaunt", g_meleeSkillTauntIsEnabled);
    g_meleeSkillKickIsEnabled = EQMACMQ_ConfigReadBool(filename, "Melee", "bKick", g_meleeSkillKickIsEnabled);
    g_meleeSkillBashIsEnabled = EQMACMQ_ConfigReadBool(filename, "Melee", "bBash", g_meleeSkillBashIsEnabled);
    g_meleeSkillSlamIsEnabled = EQMACMQ_ConfigReadBool(filename, "Melee", "bSlam", g_meleeSkillSlamIsEnabled);
    g_meleeSkillDisarmIsEnabled = EQMACMQ_ConfigReadBool(filename, "Melee", "bDisarm", g_meleeSkillDisarmIsEnabled);
    g_meleeSkillSenseHeadingIsEnabled = EQMACMQ_ConfigReadBool(filename, "Melee", "bSenseHeading", g_meleeSkillSenseHeadingIsEnabled);

    return true;
}

void EQMACMQ_CopyTargetMapLocationToClipboard()
{
    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    if (targetSpawn == NULL)
    {
        return;
    }

    char locationText[256];
    _snprintf_s
    (
        locationText, sizeof(locationText), _TRUNCATE,
        "P %.4f, %.4f, %.4f, %d, %d, %d, %d, %s",
        -targetSpawn->X, -targetSpawn->Y, targetSpawn->Z,
        0, 0, 0,
        2,
        targetSpawn->Name
    );

    const size_t length = sizeof(locationText);

    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, length);
    memcpy(GlobalLock(mem), locationText, length);
    GlobalUnlock(mem);

    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, mem);
    CloseClipboard();

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_CLASS_CEverQuest->dsp_chat("Target's map location copied to clipboard.");
    }
}

struct _EQMACMQHACK* EQMACMQ_HackList_Create(EQMACMQHACK* hack)
{
    g_hackListBegin = (struct _EQMACMQHACK*)malloc(sizeof(struct _EQMACMQHACK));

    if (g_hackListBegin == NULL)
    {
        return NULL;
    }

    g_hackListBegin->Next = NULL;

    g_hackListBegin->Index = hack->Index;

    strncpy_s(g_hackListBegin->Name,        sizeof(g_hackListBegin->Name),        hack->Name,        _TRUNCATE);
    strncpy_s(g_hackListBegin->Filename,    sizeof(g_hackListBegin->Filename),    hack->Filename,    _TRUNCATE);
    strncpy_s(g_hackListBegin->Description, sizeof(g_hackListBegin->Description), hack->Description, _TRUNCATE);

    g_hackListBegin->IsEnabled = hack->IsEnabled;

    return g_hackListBegin;
}

struct _EQMACMQHACK* EQMACMQ_HackList_Add(EQMACMQHACK* hack)
{
    if (g_hackListBegin == NULL)
    {
        return (EQMACMQ_HackList_Create(hack));
    }

    struct _EQMACMQHACK* node = g_hackListBegin;

    if (node == NULL)
    {
        return NULL;
    }

    while (node->Next != NULL)
    {
        node = node->Next;
    }

    node->Next = (struct _EQMACMQHACK*)malloc(sizeof(struct _EQMACMQHACK));
    node->Next->Next = NULL;

    node->Next->Index = hack->Index;

    strncpy_s(node->Next->Name,        sizeof(node->Next->Name),        hack->Name,        _TRUNCATE);
    strncpy_s(node->Next->Filename,    sizeof(node->Next->Filename),    hack->Filename,    _TRUNCATE);
    strncpy_s(node->Next->Description, sizeof(node->Next->Description), hack->Description, _TRUNCATE);

    node->Next->IsEnabled = hack->IsEnabled;

    return node;
}

void EQMACMQ_HackList_Destroy()
{
    struct _EQMACMQHACK* node = g_hackListBegin;

    if (node == NULL)
    {
        return;
    }

    struct _EQMACMQHACK* next;

    while (node->Next != NULL)
    {
        next = node->Next;
        free(node);
        node = next;
    }

    g_hackListBegin = NULL;
}

struct _EQMAPLINE* EQMACMQ_MapLineList_Create(EQMAPLINE* mapLine)
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

struct _EQMAPLINE* EQMACMQ_MapLineList_Add(EQMAPLINE* mapLine)
{
    if (g_mapLineListBegin == NULL)
    {
        return (EQMACMQ_MapLineList_Create(mapLine));
    }

    struct _EQMAPLINE* node = g_mapLineListBegin;

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

void EQMACMQ_MapLineList_Destroy()
{
    struct _EQMAPLINE* node = g_mapLineListBegin;

    if (node == NULL)
    {
        return;
    }

    struct _EQMAPLINE* next;

    while (node->Next != NULL)
    {
        next = node->Next;
        free(node);
        node = next;
    }

    g_mapLineListBegin = NULL;
}

struct _EQMAPPOINT* EQMACMQ_MapPointList_Create(EQMAPPOINT* mapPoint)
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

    strncpy_s(g_mapPointListBegin->Text, sizeof(g_mapPointListBegin->Text), mapPoint->Text, _TRUNCATE);

    return g_mapPointListBegin;
}

struct _EQMAPPOINT* EQMACMQ_MapPointList_Add(EQMAPPOINT* mapPoint)
{
    if (g_mapPointListBegin == NULL)
    {
        return (EQMACMQ_MapPointList_Create(mapPoint));
    }

    struct _EQMAPPOINT* node = g_mapPointListBegin;

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

    strncpy_s(node->Next->Text, sizeof(node->Next->Text), mapPoint->Text, _TRUNCATE);

    return node;
}

void EQMACMQ_MapPointList_Destroy()
{
    struct _EQMAPPOINT* node = g_mapPointListBegin;

    if (node == NULL)
    {
        return;
    }

    struct _EQMAPPOINT* next;

    while (node->Next != NULL)
    {
        next = node->Next;
        free(node);
        node = next;
    }

    g_mapPointListBegin = NULL;
}

bool EQMACMQ_LoadMap(const char* filename, int layer)
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
                EQMACMQ_MapLineList_Add(&mapLine);
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

            strncpy_s(mapPoint.Text, sizeof(mapPoint.Text), pointText, _TRUNCATE);

            if (result == 9) // the number of values for a point in the map text file
            {
                EQMACMQ_MapPointList_Add(&mapPoint);
            }
        }
    }

    if (file)
    {
        fclose(file);
    }

    return true;
}

void EQMACMQ_GuiRecalculateCoordinates()
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

    g_mapOffsetX = 0.0f;
    g_mapOffsetY = 0.0f;

    g_mapMinX = g_mapX;
    g_mapMaxX = g_mapX + g_mapWidth;

    g_mapMinY = g_mapY;
    g_mapMaxY = g_mapY + g_mapHeight;

    g_mapOriginX = g_mapX + (g_mapWidth  * 0.5f);
    g_mapOriginY = g_mapY + (g_mapHeight * 0.5f);

    float buttonIndex;

    // hud buttons

    buttonIndex = 1.0f;

    g_hudButtonExitX = g_hudButtonsX;
    g_hudButtonExitY = g_hudButtonsY;

    g_hudButtonReloadConfigX = g_hudButtonExitX + g_buttonWidth + g_elementOffset;
    g_hudButtonReloadConfigY = g_hudButtonExitY;

    buttonIndex += 1.0f;

    g_hudButtonToggleClientSwitcherX = g_hudButtonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_hudButtonToggleClientSwitcherY = g_hudButtonExitY;

    buttonIndex += 1.0f;

    g_hudButtonToggleMapX = g_hudButtonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_hudButtonToggleMapY = g_hudButtonExitY;

    buttonIndex += 1.0f;

    g_hudButtonToggleEspX = g_hudButtonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_hudButtonToggleEspY = g_hudButtonExitY;

    buttonIndex += 1.0f;

    g_hudButtonToggleEspTextX = g_hudButtonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_hudButtonToggleEspTextY = g_hudButtonExitY;

    buttonIndex += 1.0f;

    g_hudButtonToggleEspSkeletonsX = g_hudButtonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_hudButtonToggleEspSkeletonsY = g_hudButtonExitY;

    buttonIndex += 1.0f;

    g_hudButtonToggleEspGroundSpawnsX = g_hudButtonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_hudButtonToggleEspGroundSpawnsY = g_hudButtonExitY;

    buttonIndex += 1.0f;

    g_hudButtonToggleEspDoorSpawnsX = g_hudButtonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_hudButtonToggleEspDoorSpawnsY = g_hudButtonExitY;

    buttonIndex += 1.0f;

    g_hudButtonToggleEspDistanceX = g_hudButtonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_hudButtonToggleEspDistanceY = g_hudButtonExitY;

    buttonIndex += 1.0f;

    g_hudButtonToggleEspFilterNpcX = g_hudButtonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_hudButtonToggleEspFilterNpcY = g_hudButtonExitY;

    buttonIndex += 1.0f;

    g_hudButtonToggleHealthBarsX = g_hudButtonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_hudButtonToggleHealthBarsY = g_hudButtonExitY;

    buttonIndex += 1.0f;

    g_hudButtonToggleBuffInfoX = g_hudButtonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_hudButtonToggleBuffInfoY = g_hudButtonExitY;

    buttonIndex += 1.0f;

    g_hudButtonTogglePlayerInfoX = g_hudButtonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_hudButtonTogglePlayerInfoY = g_hudButtonExitY;

    buttonIndex += 1.0f;

    g_hudButtonToggleTargetInfoX = g_hudButtonExitX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_hudButtonToggleTargetInfoY = g_hudButtonExitY;

    buttonIndex += 1.0f;

    // map right column buttons

    g_mapButtonZoomInX = g_mapMaxX + g_elementOffset;
    g_mapButtonZoomInY = g_mapY;

    buttonIndex = 1.0f;

    g_mapButtonZoomOutX = g_mapMaxX + g_elementOffset;
    g_mapButtonZoomOutY = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

    buttonIndex += 1.0f;

    g_mapButtonResetZoomAndCenterX = g_mapMaxX + g_elementOffset;
    g_mapButtonResetZoomAndCenterY = g_mapY + ((g_buttonHeight + g_elementOffset) * buttonIndex);

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

void EQMACMQ_ReloadConfig()
{
    EQMACMQ_DoLoadConfig();

    EQMACMQ_GuiRecalculateCoordinates();

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_CLASS_CEverQuest->dsp_chat("Config reloaded.");
    }
}

void EQMACMQ_ReloadMap()
{
    EQMACMQ_DoLoadMap();

    EQMACMQ_GuiRecalculateCoordinates();

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_CLASS_CEverQuest->dsp_chat("Map reloaded.");
    }
}

void EQMACMQ_ClientSwitcherRefresh()
{
    EQMACMQ_UpdateEqProcesses();

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_CLASS_CEverQuest->dsp_chat("Client Switcher refreshed.");

        if (g_eqProcesses[0] > 0)
        {
            EQ_CLASS_CEverQuest->dsp_chat("Client 1 found.");
        }

        if (g_eqProcesses[1] > 0)
        {
            EQ_CLASS_CEverQuest->dsp_chat("Client 2 found.");
        }

        if (g_eqProcesses[2] > 0)
        {
            EQ_CLASS_CEverQuest->dsp_chat("Client 3 found.");
        }
    }
}

void EQMACMQ_MapZoomOut()
{
    g_mapZoom -= g_mapZoom * g_mapZoomMultiplier;

    if (g_mapZoom <= g_mapZoomMaximum)
    {
        g_mapZoom = g_mapZoomMaximum;
    }
}

void EQMACMQ_MapZoomIn()
{
    g_mapZoom += g_mapZoom * g_mapZoomMultiplier;

    if (g_mapZoom >= g_mapZoomMinimum)
    {
        g_mapZoom = g_mapZoomMinimum;
    }
}

void EQMACMQ_MapMouseWheelZoomOut()
{
    g_mapZoom -= g_mapZoom * g_mapMouseWheelZoomMultiplier;

    if (g_mapZoom <= g_mapZoomMaximum)
    {
        g_mapZoom = g_mapZoomMaximum;
    }
}

void EQMACMQ_MapMouseWheelZoomIn()
{
    g_mapZoom += g_mapZoom * g_mapMouseWheelZoomMultiplier;

    if (g_mapZoom >= g_mapZoomMinimum)
    {
        g_mapZoom = g_mapZoomMinimum;
    }
}

void EQMACMQ_MapSetZoom(float zoom)
{
    g_mapZoom = zoom;
}

void EQMACMQ_MapResetZoom()
{
    g_mapZoom = 1.0f;
}

void EQMACMQ_MapCenter()
{
    g_mapOffsetX = 0.0f;
    g_mapOffsetY = 0.0f;
}

void EQMACMQ_ToggleHudButtons()
{
    EQ_ToggleBool(g_hudButtonsIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("HUD Buttons", g_hudButtonsIsEnabled);
    }
}

void EQMACMQ_ToggleMapButtons()
{
    EQ_ToggleBool(g_mapButtonsIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Buttons", g_mapButtonsIsEnabled);
    }
}

void EQMACMQ_ToggleMapMaximized()
{
    EQ_ToggleBool(g_mapIsMaximized);

    EQMACMQ_GuiRecalculateCoordinates();
}

void EQMACMQ_ToggleMapLayer0()
{
    EQ_ToggleBool(g_mapLayer0IsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Base Layer", g_mapLayer0IsEnabled);
    }
}

void EQMACMQ_ToggleMapLayer1()
{
    EQ_ToggleBool(g_mapLayer1IsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Layer 1", g_mapLayer1IsEnabled);
    }
}

void EQMACMQ_ToggleMapLayer2()
{
    EQ_ToggleBool(g_mapLayer2IsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Layer 2", g_mapLayer2IsEnabled);
    }
}

void EQMACMQ_ToggleMapLayer3()
{
    EQ_ToggleBool(g_mapLayer3IsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Layer 3", g_mapLayer3IsEnabled);
    }
}

void EQMACMQ_ToggleMapArrow()
{
    EQ_ToggleBool(g_mapArrowIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Arrow", g_mapArrowIsEnabled);
    }
}

void EQMACMQ_ToggleMapHeightFilter()
{
    EQ_ToggleBool(g_mapHeightFilterIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Height Filter", g_mapHeightFilterIsEnabled);

        if (g_mapHeightFilterIsEnabled == true)
        {
            EQ_WriteFloatVarToChat("Low",  g_mapHeightFilterLow);
            EQ_WriteFloatVarToChat("High", g_mapHeightFilterHigh);
        }
    }
}

void EQMACMQ_ToggleMapHeightFilterSpawns()
{
    EQ_ToggleBool(g_mapHeightFilterSpawnsIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Height Filter Spawns", g_mapHeightFilterSpawnsIsEnabled);
    }
}

void EQMACMQ_ToggleMapRotate()
{
    EQ_ToggleBool(g_mapRotateIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Rotate", g_mapRotateIsEnabled);
    }

    g_mapOffsetX = 0.0f;
    g_mapOffsetY = 0.0f;
}

void EQMACMQ_ToggleMapZoneInfo()
{
    EQ_ToggleBool(g_mapZoneInfoIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Zone Info", g_mapZoneInfoIsEnabled);
    }
}

void EQMACMQ_ToggleMapSpawnClickToTarget()
{
    EQ_ToggleBool(g_mapSpawnClickToTargetIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Spawn Click To Target", g_mapSpawnClickToTargetIsEnabled);
    }
}

void EQMACMQ_ToggleMapMouseWheelZoom()
{
    EQ_ToggleBool(g_mapMouseWheelZoomIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Mouse Wheel Zoom", g_mapMouseWheelZoomIsEnabled);
    }
}

void EQMACMQ_ToggleMapSpawns()
{
    EQ_ToggleBool(g_mapSpawnsIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Spawns", g_mapSpawnsIsEnabled);
    }
}

void EQMACMQ_ToggleMapPoints()
{
    EQ_ToggleBool(g_mapPointsIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Points", g_mapPointsIsEnabled);
    }
}

void EQMACMQ_ToggleMapLines()
{
    EQ_ToggleBool(g_mapLinesIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Lines", g_mapLinesIsEnabled);
    }
}

void EQMACMQ_ToggleMapTargetLine()
{
    EQ_ToggleBool(g_mapTargetLineIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Target Line", g_mapTargetLineIsEnabled);
    }
}

void EQMACMQ_ToggleMapYourselfCorpseLine()
{
    EQ_ToggleBool(g_mapYourselfCorpseLineIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Yourself Corpse Line", g_mapYourselfCorpseLineIsEnabled);
    }
}

void EQMACMQ_ToggleMapReplaceBlackLinesWithWhiteLines()
{
    EQ_ToggleBool(g_mapReplaceBlackLinesWithWhiteLinesIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Replace Black Lines With White Lines", g_mapReplaceBlackLinesWithWhiteLinesIsEnabled);
    }
}

void EQMACMQ_ToggleMapShowPlayer()
{
    EQ_ToggleBool(g_mapShowPlayerIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Show Player", g_mapShowPlayerIsEnabled);
    }
}

void EQMACMQ_ToggleMapShowPlayerCorpse()
{
    EQ_ToggleBool(g_mapShowPlayerCorpseIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Show Player Corpse", g_mapShowPlayerCorpseIsEnabled);
    }
}

void EQMACMQ_ToggleMapShowNpc()
{
    EQ_ToggleBool(g_mapShowNpcIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Show NPC", g_mapShowNpcIsEnabled);
    }
}

void EQMACMQ_ToggleMapShowNpcCorpse()
{
    EQ_ToggleBool(g_mapShowNpcCorpseIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Show NPC Corpse", g_mapShowNpcCorpseIsEnabled);
    }
}

void EQMACMQ_ToggleMapSpawnDistance()
{
    EQ_ToggleBool(g_mapSpawnDistanceIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Spawn Distance", g_mapSpawnDistanceIsEnabled);

        if (g_mapSpawnDistanceIsEnabled == true)
        {
            EQ_WriteFloatVarToChat("Distance", g_mapSpawnDistance);
        }
    }
}

void EQMACMQ_ToggleMapSpawnFilterNpc()
{
    EQ_ToggleBool(g_mapSpawnFilterNpcIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map Spawn Filter NPC", g_mapSpawnFilterNpcIsEnabled);

        if (g_mapSpawnFilterNpcIsEnabled == true)
        {
            EQ_WriteStringVarToChat("Filter", g_mapSpawnFilterNpc);
        }
    }
}

void EQMACMQ_ToggleMapLineColor()
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

void EQMACMQ_ToggleBoxChatAutoConnect()
{
    EQ_ToggleBool(g_boxChatAutoConnectIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Box Chat Auto Connect", g_boxChatAutoConnectIsEnabled);
    }
}

void EQMACMQ_ToggleClientSwitcher()
{
    EQ_ToggleBool(g_clientSwitcherIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Client Switcher", g_clientSwitcherIsEnabled);
    }
}

void EQMACMQ_ToggleClientSwitcherSyncMouse()
{
    EQ_ToggleBool(g_clientSwitcherSyncMouseIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Client Switcher Sync Mouse", g_clientSwitcherSyncMouseIsEnabled);
    }
}

void EQMACMQ_ToggleClientSwitcherButtons()
{
    EQ_ToggleBool(g_clientSwitcherButtonsIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Client Switcher Buttons", g_clientSwitcherButtonsIsEnabled);
    }
}

void EQMACMQ_ToggleClientSwitcherHotkeys()
{
    EQ_ToggleBool(g_clientSwitcherHotkeysIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Client Switcher Hotkeys", g_clientSwitcherHotkeysIsEnabled);
    }
}

void EQMACMQ_ToggleMouseWheelZoom()
{
    EQ_ToggleBool(g_mouseWheelZoomIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Mouse Wheel Zoom", g_mouseWheelZoomIsEnabled);
    }
}

void EQMACMQ_ToggleMap()
{
    EQ_ToggleBool(g_mapIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Map", g_mapIsEnabled);
    }
}

void EQMACMQ_ToggleEsp()
{
    EQ_ToggleBool(g_espIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP", g_espIsEnabled);
    }
}

void EQMACMQ_ToggleEspClipToViewPort()
{
    EQ_ToggleBool(g_espClipToViewPortIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Clip To Viewport", g_espClipToViewPortIsEnabled);
    }
}

void EQMACMQ_ToggleEspSkeletons()
{
    EQ_ToggleBool(g_espSkeletonsIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Skeletons", g_espSkeletonsIsEnabled);

        if (g_espSkeletonsIsEnabled == true && g_espSkeletonsDistanceIsEnabled == true)
        {
            EQ_WriteFloatVarToChat("Distance", g_espSkeletonsDistance);
        }
    }
}

void EQMACMQ_ToggleEspSkeletonsDistance()
{
    EQ_ToggleBool(g_espSkeletonsDistanceIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Skeletons Distance", g_espSkeletonsDistanceIsEnabled);

        if (g_espSkeletonsIsEnabled == true && g_espSkeletonsDistanceIsEnabled == true)
        {
            EQ_WriteFloatVarToChat("Distance", g_espSkeletonsDistance);
        }
    }
}

void EQMACMQ_ToggleEspSkeletonsLineOfSight()
{
    EQ_ToggleBool(g_espSkeletonsLineOfSightIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Skeletons Line Of Sight", g_espSkeletonsLineOfSightIsEnabled);
    }
}

void EQMACMQ_ToggleEspShowText()
{
    EQ_ToggleBool(g_espShowTextIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Show Text", g_espShowTextIsEnabled);
    }
}

void EQMACMQ_ToggleEspShowTextOnMouseHover()
{
    EQ_ToggleBool(g_espShowTextOnMouseHoverIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Show Text On Mouse Hover", g_espShowTextIsEnabled);
    }
}

void EQMACMQ_ToggleEspShowYourself()
{
    EQ_ToggleBool(g_espShowYourselfIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Show Yourself", g_espShowYourselfIsEnabled);
    }
}

void EQMACMQ_ToggleEspShowPlayer()
{
    EQ_ToggleBool(g_espShowPlayerIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Show Player", g_espShowPlayerIsEnabled);
    }
}

void EQMACMQ_ToggleEspShowPlayerCorpse()
{
    EQ_ToggleBool(g_espShowPlayerCorpseIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Show Player Corpse", g_espShowPlayerCorpseIsEnabled);
    }
}

void EQMACMQ_ToggleEspShowNpc()
{
    EQ_ToggleBool(g_espShowNpcIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Show NPC", g_espShowNpcIsEnabled);
    }
}

void EQMACMQ_ToggleEspShowNpcCorpse()
{
    EQ_ToggleBool(g_espShowNpcCorpseIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Show NPC Corpse", g_espShowNpcCorpseIsEnabled);
    }
}

void EQMACMQ_ToggleEspShowGroundSpawns()
{
    EQ_ToggleBool(g_espShowGroundSpawnsIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Show Ground Spawns", g_espShowGroundSpawnsIsEnabled);
    }
}

void EQMACMQ_ToggleEspShowDoorSpawns()
{
    EQ_ToggleBool(g_espShowDoorSpawnsIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Show Door Spawns", g_espShowDoorSpawnsIsEnabled);
    }
}

void EQMACMQ_ToggleEspShowUniqueDoorSpawns()
{
    EQ_ToggleBool(g_espShowUniqueDoorSpawnsIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Show Unique Door Spawns", g_espShowUniqueDoorSpawnsIsEnabled);
    }
}

void EQMACMQ_ToggleEspDistance()
{
    EQ_ToggleBool(g_espDistanceIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Distance", g_espDistanceIsEnabled);

        if (g_espIsEnabled == true && g_espDistanceIsEnabled == true)
        {
            EQ_WriteFloatVarToChat("Distance", g_espDistance);
        }
    }
}

void EQMACMQ_ToggleEspFilterNpc()
{
    EQ_ToggleBool(g_espFilterNpcIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("ESP Filter NPC", g_espFilterNpcIsEnabled);

        if (g_espFilterNpcIsEnabled == true)
        {
            EQ_WriteStringVarToChat("Filter", g_espFilterNpc);
        }
    }
}

void EQMACMQ_ToggleHealthBars()
{
    EQ_ToggleBool(g_healthBarsIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Health Bars", g_healthBarsIsEnabled);
    }
}

void EQMACMQ_ToggleHealthBarsClipToViewPort()
{
    EQ_ToggleBool(g_healthBarsClipToViewPortIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Health Bars Clip To Viewport", g_healthBarsClipToViewPortIsEnabled);
    }
}

void EQMACMQ_ToggleHealthBarsDistance()
{
    EQ_ToggleBool(g_healthBarsDistanceIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Health Bars Distance", g_healthBarsDistanceIsEnabled);
    }
}

void EQMACMQ_ToggleHealthBarsClickToTarget()
{
    EQ_ToggleBool(g_healthBarsClickToTargetIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Health Bars Click To Target", g_healthBarsClickToTargetIsEnabled);
    }
}

void EQMACMQ_ToggleBuffInfo()
{
    EQ_ToggleBool(g_buffInfoIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Buff Info", g_buffInfoIsEnabled);
    }
}

void EQMACMQ_TogglePlayerInfo()
{
    EQ_ToggleBool(g_playerInfoIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Player Info", g_playerInfoIsEnabled);
    }
}

void EQMACMQ_ToggleTargetInfo()
{
    EQ_ToggleBool(g_targetInfoIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Target Info", g_targetInfoIsEnabled);
    }
}

void EQMACMQ_ToggleMessageText()
{
    EQ_ToggleBool(g_messageTextIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Message Text", g_messageTextIsEnabled);
    }
}

void EQMACMQ_ToggleBuffWindowTimers()
{
    EQ_ToggleBool(g_buffWindowTimersIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Buff Window Timers", g_buffWindowTimersIsEnabled);
    }
}

void EQMACMQ_ToggleHacks()
{
    EQ_ToggleBool(g_hacksIsEnabled);

    EQMACMQ_DoHacksLoad(g_hacksIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Hacks", g_hacksIsEnabled);
    }
}

void EQMACMQ_ToggleLimitCpuUsage()
{
    EQ_ToggleBool(g_limitCpuUsageIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Limit CPU Usage", g_limitCpuUsageIsEnabled);
    }
}

void EQMACMQ_ToggleNameText()
{
    EQ_ToggleBool(g_nameTextIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Name Text", g_nameTextIsEnabled);
    }
}

void EQMACMQ_ToggleNameColors()
{
    EQ_ToggleBool(g_nameColorsIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Name Colors", g_nameColorsIsEnabled);
    }
}

void EQMACMQ_ToggleSpeedHack()
{
    EQ_ToggleBool(g_speedHackIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Speed Hack", g_speedHackIsEnabled);

        if (g_speedHackIsEnabled == true)
        {
            EQ_WriteFloatVarToChat("Speed Modifier", g_speedHackModifier);
        }
    }
}

void EQMACMQ_ToggleSkillHack()
{
    EQ_ToggleBool(g_skillHackIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Skill Hack", g_skillHackIsEnabled);
    }
}

void EQMACMQ_ToggleHideCorpseLooted()
{
    EQ_ToggleBool(g_hideCorpseLootedIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Hide Corpse Looted", g_hideCorpseLootedIsEnabled);
    }
}

void EQMACMQ_ToggleInspect()
{
    bool isInspectEnabled = EQ_IsInspectEnabled();

    if (isInspectEnabled == true)
    {
        isInspectEnabled = false;

        EQ_WriteMemory<BYTE>(EQ_IS_INSPECT_ENABLED, 0);
    }
    else
    {
        isInspectEnabled = true;

        EQ_WriteMemory<BYTE>(EQ_IS_INSPECT_ENABLED, 1);
    }

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Inspect", isInspectEnabled);
    }
}

void EQMACMQ_ToggleBardTwist()
{
    EQ_ToggleBool(g_bardTwistIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Bard Twist", g_bardTwistIsEnabled);
    }
}

void EQMACMQ_ToggleFreeCamera()
{
    EQ_ToggleBool(g_freeCameraIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Free Camera", g_freeCameraIsEnabled);

        if (g_freeCameraIsEnabled == true)
        {
            EQ_WriteStringVarToChat("Free Camera", "Use the arrows keys and mouse look to direct the camera.");
        }
    }

    EQMACMQ_DoFreeCamera(g_freeCameraIsEnabled);
}

void EQMACMQ_ToggleMelee()
{
    EQ_ToggleBool(g_meleeIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Melee", g_meleeIsEnabled);
    }
}

void EQMACMQ_ToggleMeleeRange()
{
    EQ_ToggleBool(g_meleeRangeIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Melee Range", g_meleeRangeIsEnabled);
    }
}

void EQMACMQ_ToggleLootAllNoDrop()
{
    EQ_ToggleBool(g_lootAllNoDropIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Loot All NO DROP", g_lootAllNoDropIsEnabled);
    }
}

void EQMACMQ_ToggleLootAllFilterId()
{
    EQ_ToggleBool(g_lootAllFilterIdIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Loot All Filter By ID", g_lootAllFilterIdIsEnabled);
    }
}

void EQMACMQ_ToggleLootAllFilterName()
{
    EQ_ToggleBool(g_lootAllFilterNameIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Loot All Filter By Name", g_lootAllFilterNameIsEnabled);
    }
}

void EQMACMQ_ToggleLogging()
{
    EQ_ToggleBool(g_loggingIsEnabled);

    if (g_writeTextToChatWindowIsEnabled == true)
    {
        EQ_WriteBoolVarToChat("Logging", g_loggingIsEnabled);
    }
}

void EQMACMQ_DoUnload()
{
    if (g_writeTextToChatWindowIsEnabled == true)
    {
        if (EQ_IsInGame() == true)
        {
            char unloadedText[128];
            _snprintf_s(unloadedText, sizeof(unloadedText), _TRUNCATE, "%s unloaded.", g_applicationName);

            EQ_CLASS_CEverQuest->dsp_chat(unloadedText);
        }
    }

    g_bExit = 1;
}

void EQMACMQ_DoLoadMap()
{
    DWORD zoneId = EQ_ReadMemory<DWORD>(EQ_ZONE_ID);

    if (zoneId != 0)
    {
        EQMACMQ_MapLineList_Destroy();
        EQMACMQ_MapPointList_Destroy();

        char mapFilename[1024];
        _snprintf_s(mapFilename, sizeof(mapFilename), _TRUNCATE, ".\\maps\\%s.txt", EQ_OBJECT_ZoneInfo.ShortName);

        EQMACMQ_LoadMap(mapFilename, 0); // layer 0 is the base layer

        size_t mapLayer;
        for (mapLayer = 1; mapLayer < (g_mapNumLayers + 1); mapLayer++)
        {
            char mapFilenameEx[1024];
            _snprintf_s(mapFilenameEx, sizeof(mapFilenameEx), _TRUNCATE, ".\\maps\\%s_%d.txt", EQ_OBJECT_ZoneInfo.ShortName, mapLayer);

            EQMACMQ_LoadMap(mapFilenameEx, mapLayer);
        }
    }
}

void EQMACMQ_DoLoadConfig()
{
    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    DWORD zoneId = EQ_ReadMemory<DWORD>(EQ_ZONE_ID);

    // default config file
    char configFilename[1024];
    _snprintf_s(configFilename, sizeof(configFilename), _TRUNCATE, ".\\%s.ini", g_applicationExeName);

    EQMACMQ_LoadConfig(configFilename);

    // character specific config file
    if (charInfo)
    {
        char charConfigFilename[1024];
        _snprintf_s(charConfigFilename, sizeof(charConfigFilename), _TRUNCATE, ".\\charconfigs\\%s.ini", charInfo->Name);

        EQMACMQ_LoadConfig(charConfigFilename);
    }

    // zone specific config file
    if (zoneId != 0)
    {
        char zoneConfigFilename[1024];
        _snprintf_s(zoneConfigFilename, sizeof(zoneConfigFilename), _TRUNCATE, ".\\zoneconfigs\\%s.ini", EQ_OBJECT_ZoneInfo.ShortName);

        EQMACMQ_LoadConfig(zoneConfigFilename);
    }
}

void EQMACMQ_DoUpdateZone()
{
    DWORD zoneId = EQ_ReadMemory<DWORD>(EQ_ZONE_ID);

    if (g_zoneId != zoneId && zoneId != 0)
    {
        g_zoneId = zoneId;

        g_playerLevelCurrent       = 0;
        g_playerLevelPrevious      = 0;
        g_playerExperienceCurrent  = 0;
        g_playerExperiencePrevious = 0;

        EQMACMQ_DoLoadMap();
        EQMACMQ_DoLoadConfig();

        EQMACMQ_GuiRecalculateCoordinates();
    }
}

void EQMACMQ_DoHackEnable(const char* filename, bool enable)
{
    for (size_t i = 0; i < EQMACMQ_HACKS_MAX; i++)
    {
        char addressKey[128];
        _snprintf_s(addressKey, sizeof(addressKey), _TRUNCATE, "Address%d", i);

        char addressString[128];
        EQMACMQ_ConfigReadString(filename, "Hack", addressKey, "", addressString, sizeof(addressString));

        if (strlen(addressString) == 0)
        {
            continue;
        }

        DWORD baseAddress = 0;

        if (strstr(addressString, "eqgame.exe") != NULL)
        {
            baseAddress = EQ_BASE_ADDRESS;
        }
        else if (strstr(addressString, "eqgfx_dx8.dll") != NULL)
        {
            baseAddress = EQ_ReadMemory<DWORD>(EQ_POINTER_GRAPHICS_DLL);
        }

        if (baseAddress != 0)
        {
            char* token     = NULL;
            char* tokenNext = NULL;

            token = strtok_s(addressString, "+", &tokenNext);

            if (token != NULL)
            {
                token = strtok_s(NULL, "+", &tokenNext);
            }

            DWORD offset = strtoul(token, NULL, 16);

            _snprintf_s(addressString, sizeof(addressString), _TRUNCATE, "0x%08X", baseAddress + offset);
        }

        DWORD address = strtoul(addressString, NULL, 16);

        if (address == 0)
        {
            continue;
        }

        char valueKey[128];

        if (enable == true)
        {
            _snprintf_s(valueKey, sizeof(valueKey), _TRUNCATE, "Enable%d", i);
        }
        else
        {
            _snprintf_s(valueKey, sizeof(valueKey), _TRUNCATE, "Disable%d", i);
        }

        char valueString[128];
        EQMACMQ_ConfigReadString(filename, "Hack", valueKey, "", valueString, sizeof(valueString));

        if (strlen(valueString) == 0)
        {
            continue;
        }

        BYTE value[256];

        size_t valueSize = 0;

        char* token     = NULL;
        char* tokenNext = NULL;

        token = strtok_s(valueString, " ", &tokenNext);

        while (token != NULL)
        {
            value[valueSize] = (BYTE)strtoul(token, NULL, 16);

            valueSize++;

            token = strtok_s(NULL, " ", &tokenNext);
        }

        DWORD oldProtection;
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, valueSize, PAGE_EXECUTE_READWRITE, &oldProtection);

        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, value, valueSize, 0);

        DWORD tempProtection;
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, valueSize, oldProtection, &tempProtection);

        if (g_loggingIsEnabled == true)
        {
            char logText[1024];
            _snprintf_s(logText, sizeof(logText), _TRUNCATE, "%s | WriteProcessMemory, Address: 0x%08X, First Value: 0x%X, Size: %d", __FUNCTION__, address, *value, valueSize);
            EQMACMQ_Log(logText);
        }

        struct _EQMACMQHACK* hack = g_hackListBegin;

        while (hack)
        {
            if (strcmp(hack->Filename, filename) == 0)
            {
                hack->IsEnabled = enable;
                break;
            }

            hack = hack->Next;
        }
    }
}

void EQMACMQ_DoHacksLoad(bool load)
{
    EQMACMQ_HackList_Destroy();

    char configFilename[1024];
    _snprintf_s(configFilename, sizeof(configFilename), _TRUNCATE, ".\\%s.ini", g_applicationExeName);

    for (size_t i = 0; i < EQMACMQ_HACKS_MAX; i++)
    {
        char enabledKey[128];
        _snprintf_s(enabledKey, sizeof(enabledKey), _TRUNCATE, "bEnabled%d", i);

        bool enabled = EQMACMQ_ConfigReadBool(configFilename, "Hacks", enabledKey, false);

        char nameKey[128];
        _snprintf_s(nameKey, sizeof(nameKey), _TRUNCATE, "sName%d", i);

        char name[128];
        EQMACMQ_ConfigReadString(configFilename, "Hacks", nameKey, "", name, sizeof(name));

        if (strlen(name) == 0)
        {
            continue;
        }

        char hackFilename[1024];
        _snprintf_s(hackFilename, sizeof(hackFilename), _TRUNCATE, ".\\hacks\\%s.ini", name);

        char hackName[1024];
        EQMACMQ_ConfigReadString(hackFilename, "Hack", "Name", "No name", hackName, sizeof(hackName));

        char hackDescription[1024];
        EQMACMQ_ConfigReadString(hackFilename, "Hack", "Description", "No description", hackDescription, sizeof(hackDescription));

        if (load == true)
        {
            EQMACMQHACK hack;

            hack.Index = i + 1;

            strncpy_s(hack.Name,        sizeof(hack.Name),        hackName,        _TRUNCATE);
            strncpy_s(hack.Filename,    sizeof(hack.Filename),    hackFilename,    _TRUNCATE);
            strncpy_s(hack.Description, sizeof(hack.Description), hackDescription, _TRUNCATE);

            hack.IsEnabled = enabled;

            EQMACMQ_HackList_Add(&hack);

            if (hack.IsEnabled == TRUE)
            {
                EQMACMQ_DoHackEnable(hackFilename, true);
            }
        }
        else
        {
            EQMACMQ_DoHackEnable(hackFilename, false);
        }
    }
}

void EQMACMQ_DoSaveSpellset(const char* name)
{
    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    if (charInfo == NULL)
    {
        return;
    }

    char filename[1024];
    _snprintf_s(filename, sizeof(filename), _TRUNCATE, ".\\spellsets\\%s.ini", name);

    FILE* file;
    errno_t fileErrorNumber = fopen_s(&file, filename, "w");

    if (file)
    {
        fprintf(file, "[Spellset]\n");

        for (size_t i = 0; i < EQ_NUM_SPELL_GEMS; i++)
        {
            int spellId = charInfo->MemorizedSpell[i];

            if (spellId != EQ_SPELL_ID_NULL)
            {
                fprintf(file, "%d=%d\n", i, spellId);

                const char* spellName = EQ_OBJECT_SpellList->Spell[spellId]->Name;

                if (strlen(spellName) != 0 && spellName != NULL)
                {
                    fprintf(file, "# %s\n", spellName);
                }
            }
        }

        fclose(file);
    }
}

void EQMACMQ_DoLoadSpellset(const char* name)
{
    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    if (charInfo == NULL)
    {
        return;
    }

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    if (playerSpawn == NULL)
    {
        return;
    }

    bool bMemorize = false;

    char filename[1024];
    _snprintf_s(filename, sizeof(filename), _TRUNCATE, ".\\spellsets\\%s.ini", name);

    FILE* file;
    errno_t fileErrorNumber = fopen_s(&file, filename, "r");

    if (file)
    {
        for (size_t i = 0; i < EQ_NUM_SPELL_GEMS; i++)
        {
            if (charInfo->MemorizedSpell[i] != EQ_SPELL_ID_NULL)
            {
                continue;
            }

            char gemIndexText[8];
            _snprintf_s(gemIndexText, sizeof(gemIndexText), _TRUNCATE, "%d", i);

            int spellId = EQMACMQ_ConfigReadInt(filename, "Spellset", gemIndexText, EQ_SPELL_ID_NULL);

            if (spellId != EQ_SPELL_ID_NULL)
            {
                if (EQ_CLASS_CSpellBookWnd->GetSpellMemTicksLeft() == 0)
                {
                    int spellBookIndex = EQ_GetSpellBookSpellIndexBySpellId(spellId);

                    if (spellBookIndex != -1)
                    {
                        EQ_CLASS_CSpellBookWnd->Activate();
                        EQ_CLASS_CSpellBookWnd->StartSpellMemorization(spellBookIndex, i, 0);

                        bMemorize = true;
                    }
                }
            }
        }

        fclose(file);
    }

    if (bMemorize == false || playerSpawn->StandingState == EQ_STANDING_STATE_STANDING)
    {
        memset(g_loadSpellsetName, 0, sizeof(g_loadSpellsetName));

        g_loadSpellsetIsInProgress = false;
    }
}

void EQMACMQ_DoLootAll()
{
    if (g_lootAllIsInProgress == false)
    {
        return;
    }

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    if (playerSpawn == NULL)
    {
        g_lootAllCounter = 0;
        g_lootAllIsInProgress = false;
        return;
    }

    if (playerSpawn->StandingState != EQ_STANDING_STATE_LOOTING)
    {
        if (g_lootAllCounter > 1)
        {
            g_lootAllCounter = 0;
            g_lootAllIsInProgress = false;
        }
        return;
    }

    PEQSPAWNINFO corpseSpawn = (PEQSPAWNINFO)EQ_OBJECT_CorpseSpawn;

    if (corpseSpawn == NULL)
    {
        if (g_lootAllCounter > 1)
        {
            g_lootAllCounter = 0;
            g_lootAllIsInProgress = false;
        }
        return;
    }

    bool bFoundLoot = false;

    bool bFoundNoDrop = false;

    if (EQ_OBJECT_CLootWnd && EQ_OBJECT_CLootWnd->CSidlWnd.EQWnd.IsOpen == 1 && EQ_OBJECT_CLootWnd->CSidlWnd.EQWnd.IsVisible == 1)
    {
        for (size_t i = 0; i < EQ_NUM_LOOT_WINDOW_ITEMS; i++)
        {
            if (EQ_OBJECT_CLootWnd->Item[i] != NULL)
            {
                if (EQ_OBJECT_CLootWnd->Item[i]->NoDrop == 0x00)
                {
                    bFoundNoDrop = true;

                    if (g_lootAllNoDropIsEnabled == false)
                    {
                        continue;
                    }
                }

                if (g_lootAllFilterIdIsEnabled == true && strlen(g_lootAllFilterId) > 0)
                {
                    bool bIdFound = false;

                    char filter[4096];
                    strncpy_s(filter, sizeof(filter), g_lootAllFilterId, _TRUNCATE);

                    char* token     = NULL;
                    char* tokenNext = NULL;

                    token = strtok_s(filter, ",", &tokenNext);

                    while (token != NULL)
                    {
                        int id = atoi(token);

                        if (EQ_OBJECT_CLootWnd->Item[i]->Id == id)
                        {
                            bIdFound = true;
                            break;
                        }

                        token = strtok_s(NULL, ",", &tokenNext);
                    }

                    if (bIdFound == false)
                    {
                        continue;
                    }
                }

                if (g_lootAllFilterNameIsEnabled == true && strlen(g_lootAllFilterName) > 0)
                {
                    bool bNameFound = false;

                    char filter[4096];
                    strncpy_s(filter, sizeof(filter), g_lootAllFilterName, _TRUNCATE);

                    char* token     = NULL;
                    char* tokenNext = NULL;

                    token = strtok_s(filter, ",", &tokenNext);

                    while (token != NULL)
                    {
                        if (strcmp(EQ_OBJECT_CLootWnd->Item[i]->Name, token) == 0)
                        {
                            bNameFound = true;
                            break;
                        }

                        token = strtok_s(NULL, ",", &tokenNext);
                    }

                    if (bNameFound == false)
                    {
                        continue;
                    }
                }

                bFoundLoot = true;

                EQ_CLASS_CLootWnd->RequestLootSlot(i, true);

                break;
            }
        }
    }

    if (g_lootAllNoDropIsEnabled == false && bFoundNoDrop == true)
    {
        bFoundLoot = true;
    }

    if (bFoundLoot == false)
    {
        g_lootAllCounter++;

        if (g_lootAllCounter > 1)
        {
            g_lootAllCounter = 0;
            g_lootAllIsInProgress = false;

            EQ_CLASS_CLootWnd->Deactivate();
        }
    }
}

void EQMACMQ_DoSkillHack()
{
    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    if (charInfo == NULL)
    {
        return;
    }

    unsigned int swimming = charInfo->Skill[EQ_SKILL_SWIMMING];

    if (swimming < g_skillHackSwimming)
    {
        charInfo->Skill[EQ_SKILL_SWIMMING] = g_skillHackSwimming;
    }

    unsigned int tracking = charInfo->Skill[EQ_SKILL_TRACKING];

    if (tracking < g_skillHackTracking)
    {
        charInfo->Skill[EQ_SKILL_TRACKING] = g_skillHackTracking;
    }

    unsigned int senseHeading = charInfo->Skill[EQ_SKILL_SENSE_HEADING];

    if (senseHeading < g_skillHackSenseHeading)
    {
        charInfo->Skill[EQ_SKILL_SENSE_HEADING] = g_skillHackSenseHeading;
    }
}

void EQMACMQ_DoFreeCamera(bool enable)
{
    DWORD baseAddress = EQ_ReadMemory<DWORD>(EQ_POINTER_EQGfx_Dx8__t3dSetCameraLocation);

    if (enable == true)
    {
        DWORD address = 0;
        DWORD oldProtection = 0;
        DWORD tempProtection = 0;

        address = baseAddress + 0x4B; // Camera Y
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 2, PAGE_EXECUTE_READWRITE, &oldProtection);
        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, "\x90\x90", 2, 0);
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 2, oldProtection, &tempProtection);

        address = baseAddress + 0x50; // Camera X
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 3, PAGE_EXECUTE_READWRITE, &oldProtection);
        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, "\x90\x90\x90", 3, 0);
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 3, oldProtection, &tempProtection);

        address = baseAddress + 0x56; // Camera Z
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 3, PAGE_EXECUTE_READWRITE, &oldProtection);
        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, "\x90\x90\x90", 3, 0);
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 3, oldProtection, &tempProtection);

        EQ_WriteMemory<DWORD>(EQ_CAMERA_VIEW, EQ_CAMERA_VIEW_FIRST_PERSON);
    }
    else
    {
        DWORD address = 0;
        DWORD oldProtection = 0;
        DWORD tempProtection = 0;

        address = baseAddress + 0x4B; // Camera Y
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 2, PAGE_EXECUTE_READWRITE, &oldProtection);
        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, "\x89\x01", 2, 0);
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 2, oldProtection, &tempProtection);

        address = baseAddress + 0x50; // Camera X
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 3, PAGE_EXECUTE_READWRITE, &oldProtection);
        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, "\x89\x41\x04", 3, 0);
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 3, oldProtection, &tempProtection);

        address = baseAddress + 0x56; // Camera Z
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 3, PAGE_EXECUTE_READWRITE, &oldProtection);
        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, "\x89\x51\x08", 3, 0);
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 3, oldProtection, &tempProtection);
    }
}

void EQMACMQ_DoFreeCameraKeys()
{
    if (EQMACMQ_IsForegroundWindowCurrentProcessId() == false)
    {
        return;
    }

    if (GetAsyncKeyState(VK_UP))
    {
        float cameraY = EQ_OBJECT_CameraInfo->Y;
        float cameraX = EQ_OBJECT_CameraInfo->X;
        float cameraZ = EQ_OBJECT_CameraInfo->Z;

        float cameraHeading = EQ_OBJECT_CameraInfo->Heading;
        float cameraPitch   = EQ_OBJECT_CameraInfo->Pitch;

        cameraHeading = cameraHeading - 128.0f;

        if (cameraHeading < 0.0f)
        {
            cameraHeading = cameraHeading + 512.0f;
        }

        float cameraHeadingRadians = cameraHeading * EQ_PI / 256.0f;

        cameraPitch = cameraPitch + fabs(EQ_CAMERA_PITCH_DEFAULT); // remove negative offset from camera pitch

        float cameraPitchRadians = cameraPitch * EQ_PI / 256.0f;

        float addY = sinf(cameraHeadingRadians);
        float addX = cosf(cameraHeadingRadians);
        float addZ = sinf(cameraPitchRadians);

        if (fabs(cameraPitch) < 90.0f)
        {
            cameraY -= addY * g_freeCameraMultiplier;
            cameraX += addX * g_freeCameraMultiplier;
        }

        cameraZ += addZ * g_freeCameraMultiplier;

        EQ_OBJECT_CameraInfo->Y = cameraY;
        EQ_OBJECT_CameraInfo->X = cameraX;
        EQ_OBJECT_CameraInfo->Z = cameraZ;
    }

    if (GetAsyncKeyState(VK_DOWN))
    {
        float cameraY = EQ_OBJECT_CameraInfo->Y;
        float cameraX = EQ_OBJECT_CameraInfo->X;
        float cameraZ = EQ_OBJECT_CameraInfo->Z;

        float cameraHeading = EQ_OBJECT_CameraInfo->Heading;
        float cameraPitch   = EQ_OBJECT_CameraInfo->Pitch;

        cameraHeading = cameraHeading - 128.0f;

        if (cameraHeading < 0.0f)
        {
            cameraHeading = cameraHeading + 512.0f;
        }

        float cameraHeadingRadians = cameraHeading * EQ_PI / 256.0f;

        cameraPitch = cameraPitch + fabs(EQ_CAMERA_PITCH_DEFAULT); // remove negative offset from camera pitch

        float cameraPitchRadians = cameraPitch * EQ_PI / 256.0f;

        float addY = sinf(cameraHeadingRadians);
        float addX = cosf(cameraHeadingRadians);
        float addZ = sinf(cameraPitchRadians);

        if (fabs(cameraPitch) < 90.0f)
        {
            cameraY += addY * g_freeCameraMultiplier;
            cameraX -= addX * g_freeCameraMultiplier;
        }

        cameraZ -= addZ * g_freeCameraMultiplier;

        EQ_OBJECT_CameraInfo->Y = cameraY;
        EQ_OBJECT_CameraInfo->X = cameraX;
        EQ_OBJECT_CameraInfo->Z = cameraZ;
    }

    if (GetAsyncKeyState(VK_LEFT))
    {
        float cameraY = EQ_OBJECT_CameraInfo->Y;
        float cameraX = EQ_OBJECT_CameraInfo->X;
        float cameraZ = EQ_OBJECT_CameraInfo->Z;

        float cameraHeading = EQ_OBJECT_CameraInfo->Heading;
        float cameraPitch   = EQ_OBJECT_CameraInfo->Pitch;

        if (cameraHeading < 0.0f)
        {
            cameraHeading = cameraHeading + 512.0f;
        }

        float cameraHeadingRadians = cameraHeading * EQ_PI / 256.0f;

        cameraPitch = cameraPitch + fabs(EQ_CAMERA_PITCH_DEFAULT); // remove negative offset from camera pitch

        float cameraPitchRadians = cameraPitch * EQ_PI / 256.0f;

        float addY = sinf(cameraHeadingRadians);
        float addX = cosf(cameraHeadingRadians);
        float addZ = sinf(cameraPitchRadians);

        if (fabs(cameraPitch) < 90.0f)
        {
            cameraY -= addY * g_freeCameraMultiplier;
            cameraX += addX * g_freeCameraMultiplier;
        }

        cameraZ += addZ * g_freeCameraMultiplier;

        EQ_OBJECT_CameraInfo->Y = cameraY;
        EQ_OBJECT_CameraInfo->X = cameraX;
        EQ_OBJECT_CameraInfo->Z = cameraZ;
    }

    if (GetAsyncKeyState(VK_RIGHT))
    {
        float cameraY = EQ_OBJECT_CameraInfo->Y;
        float cameraX = EQ_OBJECT_CameraInfo->X;
        float cameraZ = EQ_OBJECT_CameraInfo->Z;

        float cameraHeading = EQ_OBJECT_CameraInfo->Heading;
        float cameraPitch   = EQ_OBJECT_CameraInfo->Pitch;

        cameraHeading = cameraHeading + 256.0f;

        if (cameraHeading > 512.0f)
        {
            cameraHeading = cameraHeading - 512.0f;
        }

        float cameraHeadingRadians = cameraHeading * EQ_PI / 256.0f;

        cameraPitch = cameraPitch + fabs(EQ_CAMERA_PITCH_DEFAULT); // remove negative offset from camera pitch

        float cameraPitchRadians = cameraPitch * EQ_PI / 256.0f;

        float addY = sinf(cameraHeadingRadians);
        float addX = cosf(cameraHeadingRadians);
        float addZ = sinf(cameraPitchRadians);

        if (fabs(cameraPitch) < 90.0f)
        {
            cameraY -= addY * g_freeCameraMultiplier;
            cameraX += addX * g_freeCameraMultiplier;
        }

        cameraZ += addZ * g_freeCameraMultiplier;

        EQ_OBJECT_CameraInfo->Y = cameraY;
        EQ_OBJECT_CameraInfo->X = cameraX;
        EQ_OBJECT_CameraInfo->Z = cameraZ;
    }
}

bool EQMACMQ_DoButtonMouseLeftHeldDown()
{
    // prevent hud buttons from working if a game gui window or element overlaps
    if (EQ_IsMouseHoveringOverCXWnd() == true)
    {
        return true;
    }

    WORD mouseX = EQ_ReadMemory<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_ReadMemory<WORD>(EQ_MOUSE_Y);

    DWORD mouseClickState = EQ_ReadMemory<DWORD>(EQ_MOUSE_CLICK_STATE);

    if (mouseClickState != EQ_MOUSE_CLICK_STATE_LEFT)
    {
        return true;
    }

    if (EQ_IsKeyPressedControl() == true)
    {
        if (g_mapIsEnabled == true && g_mapButtonsIsEnabled == true)
        {
            if
            (
                EQMACMQ_IsPointInsideRectangle
                (
                    mouseX, mouseY,
                    (int)g_mapX, (int)g_mapY,
                    (int)g_mapWidth, (int)g_mapHeight
                )
                == true
            )
            {
                if (g_mapRotateIsEnabled == false)
                {
                    EQ_OBJECT_CXWndManager->MouseIcon = EQ_MOUSE_ICON_SIZE_ALL;

                    if (g_mouseIsDragging == true)
                    {
                        float addX = ((float)mouseX - g_mouseDraggingBeginX);
                        float addY = ((float)mouseY - g_mouseDraggingBeginY);

                        if (g_mapZoom > 0.0f)
                        {
                            addX = addX / g_mapZoom;
                            addY = addY / g_mapZoom;
                        }

                        g_mapOffsetX += addX;
                        g_mapOffsetY += addY;

                        g_mouseDraggingBeginX = mouseX;
                        g_mouseDraggingBeginY = mouseY;
                    }
                }

                return true;
            }
        }
    }

    if (g_mapIsEnabled == true && g_mapButtonsIsEnabled == true)
    {
        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonZoomInX, (int)g_mapButtonZoomInY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_MapZoomIn();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonZoomOutX, (int)g_mapButtonZoomOutY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_MapZoomOut();
            return true;
        }
    }

    return false;
}

bool EQMACMQ_DoButtonMouseLeftDown()
{
    // prevent hud buttons from working if a game gui window or element overlaps
    if (EQ_IsMouseHoveringOverCXWnd() == true)
    {
        g_mouseIsDragging = false;

        return true;
    }

    WORD mouseX = EQ_ReadMemory<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_ReadMemory<WORD>(EQ_MOUSE_Y);

    g_mouseIsDragging = true;
    g_mouseDraggingBeginX = mouseX;
    g_mouseDraggingBeginY = mouseY;

    if (g_mapIsEnabled == true && g_mapButtonsIsEnabled == true)
    {
        if
        (
            EQMACMQ_IsPointInsideRectangle
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
            EQMACMQ_IsPointInsideRectangle
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
    }

    return false;
}

bool EQMACMQ_DoButtonMouseLeftUp()
{
    g_mouseIsDragging = false;

    // prevent hud buttons from working if a game gui window or element overlaps
    if (EQ_IsMouseHoveringOverCXWnd() == true)
    {
        return true;
    }

    WORD mouseX = EQ_ReadMemory<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_ReadMemory<WORD>(EQ_MOUSE_Y);

    if (EQ_IsKeyPressedControl() == true)
    {
        if (g_mapIsEnabled == true && g_mapButtonsIsEnabled == true)
        {
            if
            (
                EQMACMQ_IsPointInsideRectangle
                (
                    mouseX, mouseY,
                    (int)g_mapX, (int)g_mapY,
                    (int)g_mapWidth, (int)g_mapHeight
                )
                == true
            )
            {
                return true;
            }
        }
    }

    if (g_hudButtonsIsEnabled == true)
    {
        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonExitX, (int)g_hudButtonExitY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_DoUnload();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonReloadConfigX, (int)g_hudButtonReloadConfigY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ReloadConfig();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleClientSwitcherX, (int)g_hudButtonToggleClientSwitcherY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleClientSwitcher();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleMapX, (int)g_hudButtonToggleMapY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMap();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleEspX, (int)g_hudButtonToggleEspY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleEsp();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleEspTextX, (int)g_hudButtonToggleEspTextY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleEspShowText();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleEspSkeletonsX, (int)g_hudButtonToggleEspSkeletonsY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleEspSkeletons();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleEspGroundSpawnsX, (int)g_hudButtonToggleEspGroundSpawnsY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleEspShowGroundSpawns();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleEspDoorSpawnsX, (int)g_hudButtonToggleEspDoorSpawnsY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleEspShowDoorSpawns();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleEspDistanceX, (int)g_hudButtonToggleEspDistanceY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleEspDistance();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleEspFilterNpcX, (int)g_hudButtonToggleEspFilterNpcY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleEspFilterNpc();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleHealthBarsX, (int)g_hudButtonToggleHealthBarsY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleHealthBars();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleBuffInfoX, (int)g_hudButtonToggleBuffInfoY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleBuffInfo();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonTogglePlayerInfoX, (int)g_hudButtonTogglePlayerInfoY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_TogglePlayerInfo();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleTargetInfoX, (int)g_hudButtonToggleTargetInfoY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleTargetInfo();
            return true;
        }
    }

    if (g_mapIsEnabled == true && g_mapButtonsIsEnabled == true)
    {
        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonZoomInX, (int)g_mapButtonZoomInY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_MapZoomIn();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonZoomOutX, (int)g_mapButtonZoomOutY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_MapZoomOut();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonResetZoomAndCenterX, (int)g_mapButtonResetZoomAndCenterY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_MapResetZoom();
            EQMACMQ_MapCenter();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonToggleArrowX, (int)g_mapButtonToggleArrowY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapArrow();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonToggleHeightFilterX, (int)g_mapButtonToggleHeightFilterY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapHeightFilter();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonToggleRotateX, (int)g_mapButtonToggleRotateY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapRotate();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonToggleLineColorX, (int)g_mapButtonToggleLineColorY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapLineColor();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonToggleZoneInfoX, (int)g_mapButtonToggleZoneInfoY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapZoneInfo();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonToggleSpawnsX, (int)g_mapButtonToggleSpawnsY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapSpawns();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonTogglePointsX, (int)g_mapButtonTogglePointsY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapPoints();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonToggleLinesX, (int)g_mapButtonToggleLinesY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapLines();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonToggleSpawnDistanceX, (int)g_mapButtonToggleSpawnDistanceY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapSpawnDistance();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonToggleSpawnFilterNpcX, (int)g_mapButtonToggleSpawnFilterNpcY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapSpawnFilterNpc();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonReloadMapX, (int)g_mapButtonReloadMapY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ReloadMap();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonCopyTargetMapLocationToClipboardX, (int)g_mapButtonCopyTargetMapLocationToClipboardY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_CopyTargetMapLocationToClipboard();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonToggleLayer0X, (int)g_mapButtonToggleLayer0Y,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapLayer0();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonToggleLayer1X, (int)g_mapButtonToggleLayer1Y,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapLayer1();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonToggleLayer2X, (int)g_mapButtonToggleLayer2Y,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapLayer2();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonToggleLayer3X, (int)g_mapButtonToggleLayer3Y,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapLayer3();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonMaximizeX, (int)g_mapButtonMaximizeY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ToggleMapMaximized();
            return true;
        }
    }

    if (g_clientSwitcherIsEnabled == true && g_clientSwitcherButtonsIsEnabled == true)
    {
        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_clientSwitcherButtonRefreshX, (int)g_clientSwitcherButtonRefreshY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQMACMQ_ClientSwitcherRefresh();
            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_clientSwitcherButton1X, (int)g_clientSwitcherButton1Y,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            if (g_eqProcesses[0] > 0)
            {
                EQMACMQ_DoClientSwitch(0);
            }

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_clientSwitcherButton2X, (int)g_clientSwitcherButton2Y,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            if (g_eqProcesses[1] > 0)
            {
                EQMACMQ_DoClientSwitch(1);
            }

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_clientSwitcherButton3X, (int)g_clientSwitcherButton3Y,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            if (g_eqProcesses[2] > 0)
            {
                EQMACMQ_DoClientSwitch(2);
            }

            return true;
        }
    }

    return false;
}

bool EQMACMQ_DoButtonToolTipText()
{
    // prevent hud buttons from working if a game gui window or element overlaps
    if (EQ_IsMouseHoveringOverCXWnd() == true)
    {
        return true;
    }

    WORD mouseX = EQ_ReadMemory<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_ReadMemory<WORD>(EQ_MOUSE_Y);

    if (g_hudButtonsIsEnabled == true)
    {
        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonExitX, (int)g_hudButtonExitY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            char buttonText[128];
            _snprintf_s(buttonText, sizeof(buttonText), _TRUNCATE, "Exit %s", g_applicationName);

            EQ_DrawTooltipText
            (
                buttonText,
                (int)(g_hudButtonExitX),
                (int)(g_hudButtonExitY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonReloadConfigX, (int)g_hudButtonReloadConfigY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "Reload Config",
                (int)(g_hudButtonReloadConfigX),
                (int)(g_hudButtonReloadConfigY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleClientSwitcherX, (int)g_hudButtonToggleClientSwitcherY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "Client Switcher",
                (int)(g_hudButtonToggleClientSwitcherX),
                (int)(g_hudButtonToggleClientSwitcherY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleMapX, (int)g_hudButtonToggleMapY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "Map",
                (int)(g_hudButtonToggleMapX),
                (int)(g_hudButtonToggleMapY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleEspX, (int)g_hudButtonToggleEspY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "ESP",
                (int)(g_hudButtonToggleEspX),
                (int)(g_hudButtonToggleEspY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleEspTextX, (int)g_hudButtonToggleEspTextY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "ESP Text",
                (int)(g_hudButtonToggleEspTextX),
                (int)(g_hudButtonToggleEspTextY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleEspSkeletonsX, (int)g_hudButtonToggleEspSkeletonsY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "ESP Skeletons",
                (int)(g_hudButtonToggleEspSkeletonsX),
                (int)(g_hudButtonToggleEspSkeletonsY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleEspGroundSpawnsX, (int)g_hudButtonToggleEspGroundSpawnsY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "ESP Ground Spawns",
                (int)(g_hudButtonToggleEspGroundSpawnsX),
                (int)(g_hudButtonToggleEspGroundSpawnsY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleEspDoorSpawnsX, (int)g_hudButtonToggleEspDoorSpawnsY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "ESP Door Spawns",
                (int)(g_hudButtonToggleEspDoorSpawnsX),
                (int)(g_hudButtonToggleEspDoorSpawnsY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleEspDistanceX, (int)g_hudButtonToggleEspDistanceY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "ESP Distance",
                (int)(g_hudButtonToggleEspDistanceX),
                (int)(g_hudButtonToggleEspDistanceY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleEspFilterNpcX, (int)g_hudButtonToggleEspFilterNpcY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "ESP Filter NPC",
                (int)(g_hudButtonToggleEspFilterNpcX),
                (int)(g_hudButtonToggleEspFilterNpcY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleHealthBarsX, (int)g_hudButtonToggleHealthBarsY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "Health Bars",
                (int)(g_hudButtonToggleHealthBarsX),
                (int)(g_hudButtonToggleHealthBarsY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleBuffInfoX, (int)g_hudButtonToggleBuffInfoY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "Buff Info",
                (int)(g_hudButtonToggleBuffInfoX),
                (int)(g_hudButtonToggleBuffInfoY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonTogglePlayerInfoX, (int)g_hudButtonTogglePlayerInfoY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "Player Info",
                (int)(g_hudButtonTogglePlayerInfoX),
                (int)(g_hudButtonTogglePlayerInfoY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_hudButtonToggleTargetInfoX, (int)g_hudButtonToggleTargetInfoY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "Target Info",
                (int)(g_hudButtonToggleTargetInfoX),
                (int)(g_hudButtonToggleTargetInfoY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }
    }

    if (g_mapIsEnabled == true && g_mapButtonsIsEnabled == true)
    {
        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_mapButtonResetZoomAndCenterX, (int)g_mapButtonResetZoomAndCenterY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "Reset Zoom and Center",
                (int)(g_mapButtonResetZoomAndCenterX + g_buttonWidth + g_elementOffset),
                (int)(g_mapButtonResetZoomAndCenterY - 2.0f),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
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
                strncpy_s(buttonText, sizeof(buttonText), "Maximize", _TRUNCATE);
            }
            else
            {
                strncpy_s(buttonText, sizeof(buttonText), "Restore", _TRUNCATE);
            }

            EQ_DrawTooltipText
            (
                buttonText,
                (int)(g_mapButtonMaximizeX),
                (int)(g_mapButtonMaximizeY - g_elementOffset) - (g_fontHeight - 2),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }
    }

    if (g_clientSwitcherIsEnabled == true && g_clientSwitcherButtonsIsEnabled == true)
    {
        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_clientSwitcherButtonRefreshX, (int)g_clientSwitcherButtonRefreshY,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            EQ_DrawTooltipText
            (
                "Refresh Clients",
                (int)(g_clientSwitcherButtonRefreshX),
                (int)(g_clientSwitcherButtonRefreshY + g_buttonHeight + g_elementOffset),
                EQ_POINTER_FONT_ARIAL14
            );

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_clientSwitcherButton1X, (int)g_clientSwitcherButton1Y,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            if (g_eqProcesses[0] > 0)
            {
                EQ_DrawTooltipText
                (
                    "Switch to Client 1",
                    (int)(g_clientSwitcherButton1X),
                    (int)(g_clientSwitcherButton1Y + g_buttonHeight + g_elementOffset),
                    EQ_POINTER_FONT_ARIAL14
                );
            }

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_clientSwitcherButton2X, (int)g_clientSwitcherButton2Y,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            if (g_eqProcesses[1] > 0)
            {
                EQ_DrawTooltipText
                (
                    "Switch to Client 2",
                    (int)(g_clientSwitcherButton2X),
                    (int)(g_clientSwitcherButton2Y + g_buttonHeight + g_elementOffset),
                    EQ_POINTER_FONT_ARIAL14
                );
            }

            return true;
        }

        if
        (
            EQMACMQ_IsPointInsideRectangle
            (
                mouseX, mouseY,
                (int)g_clientSwitcherButton3X, (int)g_clientSwitcherButton3Y,
                (int)g_buttonWidth, (int)g_buttonHeight
            )
            == true
        )
        {
            if (g_eqProcesses[2] > 0)
            {
                EQ_DrawTooltipText
                (
                    "Switch to Client 3",
                    (int)(g_clientSwitcherButton3X),
                    (int)(g_clientSwitcherButton3Y + g_buttonHeight + g_elementOffset),
                    EQ_POINTER_FONT_ARIAL14
                );
            }

            return true;
        }
    }

    return false;
}

void EQMACMQ_DoHudButtons()
{
    DWORD fontArial14 = EQ_ReadMemory<DWORD>(EQ_POINTER_FONT_ARIAL14);

    int buttonColor = g_buttonColorMinMaxClose;
    int buttonTextColor = g_buttonTextColorMinMaxClose;

    EQ_DrawRectangle(g_hudButtonExitX, g_hudButtonExitY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("x", (int)(g_hudButtonExitX + 3.0f), (int)(g_hudButtonExitY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    EQ_DrawRectangle(g_hudButtonReloadConfigX, g_hudButtonReloadConfigY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("r", (int)(g_hudButtonReloadConfigX + 4.0f), (int)(g_hudButtonReloadConfigY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_clientSwitcherIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_hudButtonToggleClientSwitcherX, g_hudButtonToggleClientSwitcherY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("c", (int)(g_hudButtonToggleClientSwitcherX + 3.0f), (int)(g_hudButtonToggleClientSwitcherY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_mapIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_hudButtonToggleMapX, g_hudButtonToggleMapY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("m", (int)(g_hudButtonToggleMapX + 2.0f), (int)(g_hudButtonToggleMapY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_hudButtonToggleEspX, g_hudButtonToggleEspY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("e", (int)(g_hudButtonToggleEspX + 3.0f), (int)(g_hudButtonToggleEspY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espShowTextIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_hudButtonToggleEspTextX, g_hudButtonToggleEspTextY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("s", (int)(g_hudButtonToggleEspTextX + 3.0f), (int)(g_hudButtonToggleEspTextY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espSkeletonsIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_hudButtonToggleEspSkeletonsX, g_hudButtonToggleEspSkeletonsY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("k", (int)(g_hudButtonToggleEspSkeletonsX + 4.0f), (int)(g_hudButtonToggleEspSkeletonsY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espDistanceIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_hudButtonToggleEspDistanceX, g_hudButtonToggleEspDistanceY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("d", (int)(g_hudButtonToggleEspDistanceX + 3.0f), (int)(g_hudButtonToggleEspDistanceY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espFilterNpcIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_hudButtonToggleEspFilterNpcX, g_hudButtonToggleEspFilterNpcY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("f", (int)(g_hudButtonToggleEspFilterNpcX + 4.0f), (int)(g_hudButtonToggleEspFilterNpcY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espShowGroundSpawnsIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_hudButtonToggleEspGroundSpawnsX, g_hudButtonToggleEspGroundSpawnsY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("g", (int)(g_hudButtonToggleEspGroundSpawnsX + 3.0f), (int)(g_hudButtonToggleEspGroundSpawnsY - 3.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_espShowDoorSpawnsIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_hudButtonToggleEspDoorSpawnsX, g_hudButtonToggleEspDoorSpawnsY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("o", (int)(g_hudButtonToggleEspDoorSpawnsX + 3.0f), (int)(g_hudButtonToggleEspDoorSpawnsY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_healthBarsIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_hudButtonToggleHealthBarsX, g_hudButtonToggleHealthBarsY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("h", (int)(g_hudButtonToggleHealthBarsX + 3.0f), (int)(g_hudButtonToggleHealthBarsY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_buffInfoIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_hudButtonToggleBuffInfoX, g_hudButtonToggleBuffInfoY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("b", (int)(g_hudButtonToggleBuffInfoX + 3.0f), (int)(g_hudButtonToggleBuffInfoY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_playerInfoIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_hudButtonTogglePlayerInfoX, g_hudButtonTogglePlayerInfoY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("p", (int)(g_hudButtonTogglePlayerInfoX + 3.0f), (int)(g_hudButtonTogglePlayerInfoY - 3.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_targetInfoIsEnabled == false)
    {
        buttonColor = g_buttonColorDisabled;
        buttonTextColor = g_buttonTextColorDisabled;
    }

    EQ_DrawRectangle(g_hudButtonToggleTargetInfoX, g_hudButtonToggleTargetInfoY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("t", (int)(g_hudButtonToggleTargetInfoX + 5.0f), (int)(g_hudButtonToggleTargetInfoY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;
}

void EQMACMQ_DoClientSwitcherButtons()
{
    DWORD resolutionWidth = EQ_ReadMemory<DWORD>(EQ_RESOLUTION_WIDTH);

    float totalWidth = g_buttonWidth + g_elementOffset + (float)((g_buttonWidth * g_eqProcessesCount) + (g_elementOffset + (g_eqProcessesCount - 1)));

    g_clientSwitcherButtonRefreshX = (resolutionWidth * 0.5f) - (totalWidth * 0.5f);
    g_clientSwitcherButtonRefreshY = 5.0f;

    g_clientSwitcherButtonRefreshX = floorf(g_clientSwitcherButtonRefreshX);

    float buttonIndex = 1.0f;

    g_clientSwitcherButton1X = g_clientSwitcherButtonRefreshX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_clientSwitcherButton1Y = g_clientSwitcherButtonRefreshY;

    buttonIndex += 1.0f;

    g_clientSwitcherButton2X = g_clientSwitcherButtonRefreshX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_clientSwitcherButton2Y = g_clientSwitcherButtonRefreshY;

    buttonIndex += 1.0f;

    g_clientSwitcherButton3X = g_clientSwitcherButtonRefreshX + ((g_buttonWidth + g_elementOffset) * buttonIndex);
    g_clientSwitcherButton3Y = g_clientSwitcherButtonRefreshY;

    buttonIndex += 1.0f;

    DWORD fontArial14 = EQ_ReadMemory<DWORD>(EQ_POINTER_FONT_ARIAL14);

    int buttonColor = g_buttonColorMinMaxClose;
    int buttonTextColor = g_buttonTextColorMinMaxClose;

    EQ_DrawRectangle(g_clientSwitcherButtonRefreshX, g_clientSwitcherButtonRefreshY, g_buttonWidth, g_buttonHeight, buttonColor);
    EQ_CLASS_CDisplay->WriteTextHD2("r", (int)(g_clientSwitcherButtonRefreshX + 4.0f), (int)(g_clientSwitcherButtonRefreshY - 2.0f), buttonTextColor, fontArial14);

    g_numDeferred2dItems += 4;

    buttonColor = g_buttonColorEnabled;
    buttonTextColor = g_buttonTextColorEnabled;

    if (g_eqProcessesCount > 0)
    {
        EQ_DrawRectangle(g_clientSwitcherButton1X, g_clientSwitcherButton1Y, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("1", (int)(g_clientSwitcherButton1X + 3.0f), (int)(g_clientSwitcherButton1Y - 2.0f), buttonTextColor, fontArial14);

        g_numDeferred2dItems += 4;
    }

    if (g_eqProcessesCount > 1)
    {
        EQ_DrawRectangle(g_clientSwitcherButton2X, g_clientSwitcherButton2Y, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("2", (int)(g_clientSwitcherButton2X + 3.0f), (int)(g_clientSwitcherButton2Y - 2.0f), buttonTextColor, fontArial14);

        g_numDeferred2dItems += 4;
    }

    if (g_eqProcessesCount > 2)
    {
        EQ_DrawRectangle(g_clientSwitcherButton3X, g_clientSwitcherButton3Y, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("3", (int)(g_clientSwitcherButton3X + 3.0f), (int)(g_clientSwitcherButton3Y - 2.0f), buttonTextColor, fontArial14);

        g_numDeferred2dItems += 4;
    }
}

void EQMACMQ_DoMap()
{
    DWORD fontArial14 = EQ_ReadMemory<DWORD>(EQ_POINTER_FONT_ARIAL14);

    WORD mouseX = EQ_ReadMemory<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_ReadMemory<WORD>(EQ_MOUSE_Y);

    DWORD mouseClickState = EQ_ReadMemory<DWORD>(EQ_MOUSE_CLICK_STATE);

    DWORD zoneId = EQ_ReadMemory<DWORD>(EQ_ZONE_ID);

    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    PEQSPAWNINFO spawn = (PEQSPAWNINFO)EQ_OBJECT_FirstSpawn;

    PEQGROUNDSPAWNINFO groundSpawn = (PEQGROUNDSPAWNINFO)EQ_OBJECT_FirstGroundSpawn;

    PEQDOORSPAWNINFO doorSpawn = (PEQDOORSPAWNINFO)EQ_OBJECT_FirstDoorSpawn;

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    EQGROUPLIST* groupList = (EQGROUPLIST*)EQ_STRUCTURE_GROUP_LIST;

    EQLOCATION playerLocation = {playerSpawn->Y, playerSpawn->X, playerSpawn->Z};

    float mapPlayerX = (((-playerSpawn->X * g_mapZoom) + g_mapOriginX) + ((playerSpawn->X + g_mapOffsetX) * g_mapZoom));
    float mapPlayerY = (((-playerSpawn->Y * g_mapZoom) + g_mapOriginY) + ((playerSpawn->Y + g_mapOffsetY) * g_mapZoom));

    EQ_CLASS_CDisplay->WriteTextHD2
    (
        "Map",
        (int)g_mapX,
        (int)(g_mapY - g_elementOffset) - g_fontHeightPixels,
        g_mapDefaultTextColor,
        fontArial14
    );

    EQ_DrawRectangle(g_mapX, g_mapY, g_mapWidth, g_mapHeight, g_mapBorderColor);

    if (g_mapBackgroundColor != 0x00000000)
    {
        EQ_DrawRectangle(g_mapX + 1.0f, g_mapY + 1.0f, g_mapWidth - 1.0f, g_mapHeight - 1.0f, g_mapBackgroundColor, true);
    }

    g_numDeferred2dItems += 4;

    if (g_mapButtonsIsEnabled == true)
    {
        int buttonColor = g_buttonColorEnabled;
        int buttonTextColor = g_buttonTextColorEnabled;

        EQ_DrawRectangle(g_mapButtonZoomInX, g_mapButtonZoomInY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("+", (int)(g_mapButtonZoomInX + 3.0f), (int)(g_mapButtonZoomInY - 2.0f), buttonTextColor, fontArial14);

        g_numDeferred2dItems += 4;

        EQ_DrawRectangle(g_mapButtonZoomOutX, g_mapButtonZoomOutY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("-", (int)(g_mapButtonZoomOutX + 4.0f), (int)(g_mapButtonZoomOutY - 3.0f), buttonTextColor, fontArial14);

        g_numDeferred2dItems += 4;

        EQ_DrawRectangle(g_mapButtonResetZoomAndCenterX, g_mapButtonResetZoomAndCenterY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2("*", (int)(g_mapButtonResetZoomAndCenterX + 4.0f), (int)(g_mapButtonResetZoomAndCenterY - 0.0f), buttonTextColor, fontArial14);

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
            strncpy_s(mapButtonMaximizeText, sizeof(mapButtonMaximizeText), "+", _TRUNCATE);
            mapButtonMaximizeTextOffsetX = 3.0;
            mapButtonMaximizeTextOffsetY = 2.0;
        }
        else
        {
            strncpy_s(mapButtonMaximizeText, sizeof(mapButtonMaximizeText), "-", _TRUNCATE);
            mapButtonMaximizeTextOffsetX = 4.0;
            mapButtonMaximizeTextOffsetY = 2.0;
        }

        EQ_DrawRectangle(g_mapButtonMaximizeX, g_mapButtonMaximizeY, g_buttonWidth, g_buttonHeight, buttonColor);
        EQ_CLASS_CDisplay->WriteTextHD2(mapButtonMaximizeText, (int)(g_mapButtonMaximizeX + mapButtonMaximizeTextOffsetX), (int)(g_mapButtonMaximizeY - mapButtonMaximizeTextOffsetY), buttonTextColor, fontArial14);

        g_numDeferred2dItems += 4;
    }

    if (g_mapLinesIsEnabled == true)
    {
        struct _EQMAPLINE* mapLine = g_mapLineListBegin;

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
            line.X1 = ((mapLine->X1 * g_mapZoom) + g_mapOriginX) + ((playerSpawn->X + g_mapOffsetX) * g_mapZoom);
            line.Y1 = ((mapLine->Y1 * g_mapZoom) + g_mapOriginY) + ((playerSpawn->Y + g_mapOffsetY) * g_mapZoom);
            line.Z1 = 1.0f;
            line.X2 = ((mapLine->X2 * g_mapZoom) + g_mapOriginX) + ((playerSpawn->X + g_mapOffsetX) * g_mapZoom);
            line.Y2 = ((mapLine->Y2 * g_mapZoom) + g_mapOriginY) + ((playerSpawn->Y + g_mapOffsetY) * g_mapZoom);
            line.Z2 = 1.0f;

            if (g_mapRotateIsEnabled == true)
            {
                EQ_Rotate2d(g_mapOriginX, g_mapOriginY, line.X1, line.Y1, playerSpawn->Heading);
                EQ_Rotate2d(g_mapOriginX, g_mapOriginY, line.X2, line.Y2, playerSpawn->Heading);
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
            bool bDrawLine = EQMACMQ_DoClipLine(&line, g_mapMinX, g_mapMinY, g_mapMaxX, g_mapMaxY);

            // only use a percentage of the maximum allowed number of deferred 2D items
            // the game and this application need to reserve some to draw other stuff
            if (EQMACMQ_IsDeferred2dItemsAtMax() == true)
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

                    mapLineColor = argbColor.Color;

                    // replace black lines with white lines for visibility
                    if
                    (
                        g_mapReplaceBlackLinesWithWhiteLinesIsEnabled == true ||
                        g_mapBackgroundColor == 0xFF000000                    ||

                        (int)g_mapX                 < EQ_OBJECT_ViewPort.X1 ||
                        (int)g_mapY                 < EQ_OBJECT_ViewPort.Y1 ||
                        (int)(g_mapX + g_mapWidth)  > EQ_OBJECT_ViewPort.X2 ||
                        (int)(g_mapY + g_mapHeight) > EQ_OBJECT_ViewPort.Y2
                    )
                    {
                        if (mapLineColor == 0xFF000000)
                        {
                            mapLineColor = 0xFFFFFFFF;
                        }
                    }

                    // if map background is white, replace white lines with black lines
                    if (g_mapBackgroundColor == 0xFFFFFFFF)
                    {
                        if (mapLineColor == 0xFFFFFFFF)
                        {
                            mapLineColor = 0xFF000000;
                        }
                    }
                }

                EQGfx_Dx8__t3dDeferLine(&line, mapLineColor);

                g_numDeferred2dItems++;
            }

            mapLine = mapLine->Next;
        }
    } // if (g_mapLinesIsEnabled == true)

    if (g_mapTargetLineIsEnabled == true)
    {
        if (targetSpawn && targetSpawn != playerSpawn)
        {
            EQLINE line;
            line.X1 = mapPlayerX;
            line.Y1 = mapPlayerY;
            line.Z1 = 1.0f;
            line.X2 = ((-targetSpawn->X * g_mapZoom) + g_mapOriginX) + ((playerSpawn->X + g_mapOffsetX) * g_mapZoom);
            line.Y2 = ((-targetSpawn->Y * g_mapZoom) + g_mapOriginY) + ((playerSpawn->Y + g_mapOffsetY) * g_mapZoom);
            line.Z2 = 1.0f;

            if (g_mapRotateIsEnabled == true)
            {
                EQ_Rotate2d(g_mapOriginX, g_mapOriginY, line.X1, line.Y1, playerSpawn->Heading);
                EQ_Rotate2d(g_mapOriginX, g_mapOriginY, line.X2, line.Y2, playerSpawn->Heading);
            }

            bool bDrawLine = EQMACMQ_DoClipLine(&line, g_mapMinX, g_mapMinY, g_mapMaxX, g_mapMaxY);

            if (EQMACMQ_IsDeferred2dItemsAtMax() == true)
            {
                bDrawLine = false;
            }

            if (bDrawLine == true)
            {
                EQGfx_Dx8__t3dDeferLine(&line, g_mapTargetLineColor);

                g_numDeferred2dItems++;
            }
        }
    }

    if (g_mapPointsIsEnabled == true && g_mapIsMaximized == true)
    {
        struct _EQMAPPOINT* mapPoint = g_mapPointListBegin;

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

            float pointX = (((mapPoint->X * g_mapZoom) + g_mapOriginX) + ((playerSpawn->X + g_mapOffsetX) * g_mapZoom));
            float pointY = (((mapPoint->Y * g_mapZoom) + g_mapOriginY) + ((playerSpawn->Y + g_mapOffsetY) * g_mapZoom));

            if (g_mapRotateIsEnabled == true)
            {
                EQ_Rotate2d(g_mapOriginX, g_mapOriginY, pointX, pointY, playerSpawn->Heading);
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
            _snprintf_s(pointText, sizeof(pointText), _TRUNCATE, "+ %s", mapPoint->Text);

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

            if (g_mapHeightFilterIsEnabled == true && g_mapHeightFilterSpawnsIsEnabled == true)
            {
                if (g_mapHeightFilterLow > 0.0f)
                {
                    if (spawn->Z < (playerSpawn->Z - g_mapHeightFilterLow))
                    {
                        spawn = spawn->Next;
                        continue;
                    }
                }

                if (g_mapHeightFilterHigh > 0.0f)
                {
                    if (spawn->Z > (playerSpawn->Z + g_mapHeightFilterLow))
                    {
                        spawn = spawn->Next;
                        continue;
                    }
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

            float mapSpawnX = (((-spawn->X * g_mapZoom) + g_mapOriginX) + ((playerSpawn->X + g_mapOffsetX) * g_mapZoom));
            float mapSpawnY = (((-spawn->Y * g_mapZoom) + g_mapOriginY) + ((playerSpawn->Y + g_mapOffsetY) * g_mapZoom));

            if (g_mapRotateIsEnabled == true)
            {
                EQ_Rotate2d(g_mapOriginX, g_mapOriginY, mapSpawnX, mapSpawnY, playerSpawn->Heading);
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
                strncpy_s(filter, sizeof(filter), g_mapSpawnFilterNpc, _TRUNCATE);

                char* filterToken     = NULL;
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
                _snprintf_s(spawnText, sizeof(spawnText), "+ %s", spawnName, _TRUNCATE);

                EQ_CLASS_CDisplay->WriteTextHD2(spawnText, (int)mapSpawnX, (int)mapSpawnY, textColor, fontArial14);
            }

            if (EQMACMQ_IsPointInsideRectangle(mouseX, mouseY, (int)(mapSpawnX + 1) - 4, (int)(mapSpawnY + 6) - 4, 8, 8) == true)
            {
                if (EQ_IsMouseHoveringOverCXWnd() == false)
                {
                    if (g_mapSpawnClickToTargetIsEnabled == true)
                    {
                        if (mouseClickState == EQ_MOUSE_CLICK_STATE_LEFT && EQ_IsKeyPressedControl() == false)
                        {
                            EQ_OBJECT_TargetSpawn = spawn;
                        }
                    }

                    if (g_mapIsMaximized == false)
                    {
                        char spawnText[128];
                        _snprintf_s(spawnText, sizeof(spawnText), _TRUNCATE, "%s", spawnName);

                        int textWidth = EQ_GetFontTextWidth(EQ_POINTER_FONT_ARIAL14, spawnText);

                        textWidth = textWidth + 1;

                        EQ_DrawRectangle((float)(mouseX + EQ_MOUSE_CURSOR_WIDTH + 1), (float)mouseY, (float)textWidth, (float)g_fontHeight, 0x80000000, true);

                        EQ_CLASS_CDisplay->WriteTextHD2(spawnText, (int)(mouseX + EQ_MOUSE_CURSOR_WIDTH + 1), (int)mouseY, textColor, fontArial14);
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

                if (strstr(spawn->Name, playerSpawn->Name) != NULL)
                {
                    EQLINE line;
                    line.X1 = mapPlayerX;
                    line.Y1 = mapPlayerY;
                    line.Z1 = 1.0f;
                    line.X2 = ((-spawn->X * g_mapZoom) + g_mapOriginX) + ((playerSpawn->X + g_mapOffsetX) * g_mapZoom);
                    line.Y2 = ((-spawn->Y * g_mapZoom) + g_mapOriginY) + ((playerSpawn->Y + g_mapOffsetY) * g_mapZoom);
                    line.Z2 = 1.0f;

                    if (g_mapRotateIsEnabled == true)
                    {
                        EQ_Rotate2d(g_mapOriginX, g_mapOriginY, line.X1, line.Y1, playerSpawn->Heading);
                        EQ_Rotate2d(g_mapOriginX, g_mapOriginY, line.X2, line.Y2, playerSpawn->Heading);
                    }

                    bool bDrawLine = EQMACMQ_DoClipLine(&line, g_mapMinX, g_mapMinY, g_mapMaxX, g_mapMaxY);

                    if (EQMACMQ_IsDeferred2dItemsAtMax() == true)
                    {
                        bDrawLine = false;
                    }

                    if (bDrawLine == true)
                    {
                        EQGfx_Dx8__t3dDeferLine(&line, g_mapCorpseLineColor);

                        g_numDeferred2dItems++;
                    }
                }

                spawn = spawn->Next;
            }
        }
    }

    bool bDrawCenterAndArrow = true;

    if
    (
        mapPlayerX >= g_mapMaxX ||
        mapPlayerY >= g_mapMaxY ||
        mapPlayerX <= g_mapMinX ||
        mapPlayerY <= g_mapMinY
    )
    {
        bDrawCenterAndArrow = false;
    }

    if (bDrawCenterAndArrow == true)
    {
        EQLINE mapCenterLineHorizontal;
        mapCenterLineHorizontal.X1 = mapPlayerX - g_mapCenterLineSize;
        mapCenterLineHorizontal.Y1 = mapPlayerY;
        mapCenterLineHorizontal.Z1 = 1.0f;
        mapCenterLineHorizontal.X2 = mapPlayerX + g_mapCenterLineSize;
        mapCenterLineHorizontal.Y2 = mapPlayerY;
        mapCenterLineHorizontal.Z2 = 1.0f;

        EQLINE mapCenterLineVertical;
        mapCenterLineVertical.X1 = mapPlayerX;
        mapCenterLineVertical.Y1 = mapPlayerY  - g_mapCenterLineSize;
        mapCenterLineVertical.Z1 = 1.0f;
        mapCenterLineVertical.X2 = mapPlayerX;
        mapCenterLineVertical.Y2 = mapPlayerY  + g_mapCenterLineSize;
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

            float playerHeadingRadiansC = playerHeadingC * (EQ_PI / 256.0f);

            float arrowAddCX = cosf(playerHeadingRadiansC);
            arrowAddCX = arrowAddCX * g_mapArrowRadius;

            float arrowAddCY = sinf(playerHeadingRadiansC);
            arrowAddCY = arrowAddCY * g_mapArrowRadius;

            float arrowCX = -playerSpawn->X - arrowAddCX;
            float arrowCY = -playerSpawn->Y + arrowAddCY;

            float playerHeadingB = playerHeadingC;

            playerHeadingB = playerHeadingB - 32.0f;

            if (playerHeadingB < 0.0f)
            {
                playerHeadingB = playerHeadingB + 512.0f;
            }

            float playerHeadingRadiansB = playerHeadingB * (EQ_PI / 256.0f);

            float arrowAddBX = cosf(playerHeadingRadiansB);
            arrowAddBX = arrowAddBX * (g_mapArrowRadius * 0.5f);

            float arrowAddBY = sinf(playerHeadingRadiansB);
            arrowAddBY = arrowAddBY * (g_mapArrowRadius * 0.5f);

            float arrowBX = arrowCX + arrowAddBX;
            float arrowBY = arrowCY - arrowAddBY;

            float playerHeadingA = playerHeadingC;

            playerHeadingA = playerHeadingA + 32.0f;

            if (playerHeadingA > 512.0f)
            {
                playerHeadingA = playerHeadingA - 512.0f;
            }

            float playerHeadingRadiansA = playerHeadingA * (EQ_PI / 256.0f);

            float arrowAddAX = cosf(playerHeadingRadiansA);
            arrowAddAX = arrowAddAX * (g_mapArrowRadius * 0.5f);

            float arrowAddAY = sinf(playerHeadingRadiansA);
            arrowAddAY = arrowAddAY * (g_mapArrowRadius * 0.5f);

            float arrowAX = arrowCX + arrowAddAX;
            float arrowAY = arrowCY - arrowAddAY;

            float mapArrowCX = (arrowCX + mapPlayerX) + playerSpawn->X;
            float mapArrowCY = (arrowCY + mapPlayerY) + playerSpawn->Y;

            float mapArrowBX = (arrowBX + mapPlayerX) + playerSpawn->X;
            float mapArrowBY = (arrowBY + mapPlayerY) + playerSpawn->Y;

            float mapArrowAX = (arrowAX + mapPlayerX) + playerSpawn->X;
            float mapArrowAY = (arrowAY + mapPlayerY) + playerSpawn->Y;

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
            mapArrowLineC.X1 = mapPlayerX;
            mapArrowLineC.Y1 = mapPlayerY;
            mapArrowLineC.Z1 = 1.0f;
            mapArrowLineC.X2 = mapArrowCX;
            mapArrowLineC.Y2 = mapArrowCY;
            mapArrowLineC.Z2 = 1.0f;

            EQGfx_Dx8__t3dDeferLine(&mapArrowLineA, g_mapArrowColor);
            EQGfx_Dx8__t3dDeferLine(&mapArrowLineB, g_mapArrowColor);
            EQGfx_Dx8__t3dDeferLine(&mapArrowLineC, g_mapArrowColor);
        }
    }

    if (g_mapZoneInfoIsEnabled == true)
    {
        float textY = g_mapMaxY + g_elementOffset;

        char zoneText[128];
        _snprintf_s(zoneText, sizeof(zoneText), _TRUNCATE, "Zone: %s (ID: %d)", EQ_OBJECT_ZoneInfo.ShortName, zoneId);
        EQ_CLASS_CDisplay->WriteTextHD2(zoneText, (int)g_mapX, (int)textY, g_mapZoneInfoTextColor, fontArial14);

        textY = textY + (g_fontHeight - g_fontHeightEmptyPixelsTop);

        if (g_mapIsMaximized == true)
        {
            char locationText[128];
            _snprintf_s(locationText, sizeof(locationText), _TRUNCATE, "Location: %.2f, %.2f, %.2f", playerLocation.Y, playerLocation.X, playerLocation.Z);
            EQ_CLASS_CDisplay->WriteTextHD2(locationText, (int)g_mapX, (int)textY, g_mapZoneInfoTextColor, fontArial14);

            textY = textY + (g_fontHeight - g_fontHeightEmptyPixelsTop);
        }

        if (g_mapRotateIsEnabled == true)
        {
            char headingText[128];
            _snprintf_s(headingText, sizeof(headingText), _TRUNCATE, "Heading: %s", EQ_GetCardinalDirectionByHeading(playerSpawn->Heading));
            EQ_CLASS_CDisplay->WriteTextHD2(headingText, (int)g_mapX, (int)textY, g_mapZoneInfoTextColor, fontArial14);

            textY = textY + (g_fontHeight - g_fontHeightEmptyPixelsTop);
        }
    }
}

void EQMACMQ_DoEspSkeletonsDrawLineBetweenDag(PEQDAGINFO dag1, PEQDAGINFO dag2, int lineColor)
{
    if (dag1 == NULL || dag2 == NULL)
    {
        return;
    }

    if (EQMACMQ_IsDeferred2dItemsAtMax() == true)
    {
        return;
    }

    int viewPortX1 = EQ_OBJECT_ViewPort.X1;
    int viewPortY1 = EQ_OBJECT_ViewPort.Y1;
    int viewPortX2 = EQ_OBJECT_ViewPort.X2;
    int viewPortY2 = EQ_OBJECT_ViewPort.Y2;

    DWORD worldSpaceToScreenSpaceCameraData = EQ_ReadMemory<DWORD>(EQ_POINTER_WORLD_SPACE_TO_SCREEN_SPACE_CAMERA_DATA);

    EQLOCATION location1 = {dag1->Y, dag1->X, dag1->Z};
    EQLOCATION location2 = {dag2->Y, dag2->X, dag2->Z};

    float result1X = 0.0f;
    float result1Y = 0.0f;
    int result1 = EQGfx_Dx8__t3dWorldSpaceToScreenSpace(worldSpaceToScreenSpaceCameraData, &location1, &result1X, &result1Y);

    if (result1 == EQ_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
    {
        return;
    }

    int screen1X = (int)(result1X + viewPortX1);
    int screen1Y = (int)(result1Y + viewPortY1);

    if
    (
        EQMACMQ_IsPointInsideRectangle
        (
            screen1X, screen1Y,
            viewPortX1,  viewPortY1,
            viewPortX2 - viewPortX1,
            viewPortY2 - viewPortY1
        )
        == false
    )
    {
        return;
    }

    float result2X = 0.0f;
    float result2Y = 0.0f;
    int result2 = EQGfx_Dx8__t3dWorldSpaceToScreenSpace(worldSpaceToScreenSpaceCameraData, &location2, &result2X, &result2Y);

    if (result2 == EQ_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
    {
        return;
    }

    int screen2X = (int)(result2X + viewPortX1);
    int screen2Y = (int)(result2Y + viewPortY1);

    if
    (
        EQMACMQ_IsPointInsideRectangle
        (
            screen2X, screen2Y,
            viewPortX1,  viewPortY1,
            viewPortX2 - viewPortX1,
            viewPortY2 - viewPortY1
        )
        == false
    )
    {
        return;
    }

    EQLINE line;
    line.X1 = (float)screen1X;
    line.Y1 = (float)screen1Y;
    line.Z1 = 1.0f;
    line.X2 = (float)screen2X;
    line.Y2 = (float)screen2Y;
    line.Z2 = 1.0f;

    EQGfx_Dx8__t3dDeferLine(&line, lineColor);

    g_numDeferred2dItems++;
}

void EQMACMQ_DoEspSkeletonsDraw(PEQDAGINFO dag, int lineColor)
{
    // note: this is a recursive function, it calls itself

    if (dag == NULL)
    {
        return;
    }

    if (dag->NumChildren > 0)
    {
        for (size_t i = 0; i < dag->NumChildren; i++)
        {
            if (strstr(dag->Children->Child[i]->Name, "POINT") == NULL)
            {
                EQMACMQ_DoEspSkeletonsDrawLineBetweenDag(dag, dag->Children->Child[i], lineColor);
            }

            EQMACMQ_DoEspSkeletonsDraw(dag->Children->Child[i], lineColor);
        }
    }
}

void EQMACMQ_DoEsp()
{
    DWORD fontArial14 = EQ_ReadMemory<DWORD>(EQ_POINTER_FONT_ARIAL14);

    WORD mouseX = EQ_ReadMemory<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_ReadMemory<WORD>(EQ_MOUSE_Y);

    DWORD mouseClickState = EQ_ReadMemory<DWORD>(EQ_MOUSE_CLICK_STATE);

    DWORD worldSpaceToScreenSpaceCameraData = EQ_ReadMemory<DWORD>(EQ_POINTER_WORLD_SPACE_TO_SCREEN_SPACE_CAMERA_DATA);

    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    PEQSPAWNINFO spawn = (PEQSPAWNINFO)EQ_OBJECT_FirstSpawn;

    PEQGROUNDSPAWNINFO groundSpawn = (PEQGROUNDSPAWNINFO)EQ_OBJECT_FirstGroundSpawn;

    PEQDOORSPAWNINFO doorSpawn = (PEQDOORSPAWNINFO)EQ_OBJECT_FirstDoorSpawn;

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    EQGROUPLIST* groupList = (EQGROUPLIST*)EQ_STRUCTURE_GROUP_LIST;

    EQLOCATION playerLocation = {playerSpawn->Y, playerSpawn->X, playerSpawn->Z};

    if (g_espSkeletonsIsEnabled == true)
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

            if (g_espSkeletonsDistanceIsEnabled == true)
            {
                if (spawnDistance > g_espSkeletonsDistance)
                {
                    spawn = spawn->Next;
                    continue;
                }
            }

            if (g_espSkeletonsLineOfSightIsEnabled == true)
            {
                if (EQ_CastRay((EQPlayer*)playerSpawn, spawn->Y, spawn->X, spawn->Z) == 1)
                {
                    spawn = spawn->Next;
                    continue;
                }
            }

            if (g_espClipToViewPortIsEnabled == true && (EQ_OBJECT_ViewPort.X1 != 0 || EQ_OBJECT_ViewPort.Y1 != 0))
            {
                int viewPortX1 = EQ_OBJECT_ViewPort.X1;
                int viewPortY1 = EQ_OBJECT_ViewPort.Y1;
                int viewPortX2 = EQ_OBJECT_ViewPort.X2;
                int viewPortY2 = EQ_OBJECT_ViewPort.Y2;

                float resultX = 0.0f;
                float resultY = 0.0f;
                int result = EQGfx_Dx8__t3dWorldSpaceToScreenSpace(worldSpaceToScreenSpaceCameraData, &spawnLocation, &resultX, &resultY);

                if (result != EQ_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
                {
                    int screenX = (int)(resultX + viewPortX1);
                    int screenY = (int)(resultY + viewPortY1);

                    if
                    (
                        EQMACMQ_IsPointInsideRectangle
                        (
                            screenX, screenY,
                            viewPortX1,  viewPortY1,
                            viewPortX2 - viewPortX1,
                            viewPortY2 - viewPortY1
                        )
                        == false
                    )
                    {
                        spawn = spawn->Next;
                        continue;
                    }
                }
            }

            PEQDAGINFO dagRoot = spawn->ActorInfo->ModelInfo->DagRoot;

            if (dagRoot && dagRoot->NumChildren > 0)
            {
                PEQDAGINFO dagFirst = dagRoot->Children->Child[0];

                if (dagFirst == NULL)
                {
                    spawn = spawn->Next;
                    continue;
                }

                int lineColor = g_espSkeletonsDefaultLineColor;

                switch (spawn->Type)
                {
                    case EQ_SPAWN_TYPE_PLAYER:
                        lineColor = g_espSkeletonsPlayerLineColor;
                        break;

                    case EQ_SPAWN_TYPE_PLAYER_CORPSE:
                        lineColor = g_espSkeletonsPlayerCorpseLineColor;
                        break;

                    case EQ_SPAWN_TYPE_NPC:
                        lineColor = g_espSkeletonsNpcLineColor;
                        break;

                    case EQ_SPAWN_TYPE_NPC_CORPSE:
                        lineColor = g_espSkeletonsNpcCorpseLineColor;
                        break;
                }

                for (size_t i = 0; i < EQ_GROUP_MEMBERS_MAX; i++)
                {
                    if (spawn == groupList->GroupMember[i])
                    {
                        lineColor = g_espSkeletonsGroupMemberLineColor;
                        break;
                    }
                }

                if (spawn == targetSpawn)
                {
                    lineColor = g_espSkeletonsTargetLineColor;
                }

                if (spawn->IsGameMaster == 1)
                {
                    lineColor = g_espSkeletonsGameMasterLineColor;;
                }

                EQMACMQ_DoEspSkeletonsDraw(dagFirst, lineColor);
            }

            spawn = spawn->Next;
        }
    }

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
            int viewPortX1 = EQ_OBJECT_ViewPort.X1;
            int viewPortY1 = EQ_OBJECT_ViewPort.Y1;
            int viewPortX2 = EQ_OBJECT_ViewPort.X2;
            int viewPortY2 = EQ_OBJECT_ViewPort.Y2;

            int screenX = (int)(resultX + viewPortX1);
            int screenY = (int)(resultY + viewPortY1);

            if (g_espClipToViewPortIsEnabled == true && (EQ_OBJECT_ViewPort.X1 != 0 || EQ_OBJECT_ViewPort.Y1 != 0))
            {
                if
                (
                    EQMACMQ_IsPointInsideRectangle
                    (
                        screenX, screenY,
                        viewPortX1,  viewPortY1,
                        viewPortX2 - viewPortX1,
                        viewPortY2 - viewPortY1
                    )
                    == false
                )
                {
                    doorSpawn = doorSpawn->Next;
                    continue;
                }
            }

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
                if (g_espShowUniqueDoorSpawnsIsEnabled == true)
                {
                    _snprintf_s(spawnText, sizeof(spawnText), _TRUNCATE, "+ %s", doorDescription);
                }
                else
                {
                    doorSpawn = doorSpawn->Next;
                    continue;
                }
            }
            else
            {
                if (g_espShowDoorSpawnsIsEnabled == true)
                {
                    _snprintf_s(spawnText, sizeof(spawnText), _TRUNCATE, "+ Door: %s", spawnName);
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
                _snprintf_s(distanceText, sizeof(distanceText), _TRUNCATE, " (%d)", (int)spawnDistance);
                strncat_s(spawnText, sizeof(spawnText), distanceText, _TRUNCATE);
            }

            if (g_espShowTextIsEnabled == false)
            {
                EQ_CLASS_CDisplay->WriteTextHD2("+", screenX, screenY, g_espDoorSpawnTextColor, fontArial14);
            }
            else
            {
                EQ_CLASS_CDisplay->WriteTextHD2(spawnText, screenX, screenY, g_espDoorSpawnTextColor, fontArial14);
            }

            if (g_espShowTextIsEnabled == false && g_espShowTextOnMouseHoverIsEnabled == true)
            {
                if (EQMACMQ_IsPointInsideRectangle(mouseX, mouseY, (int)(screenX + 1) - 4, (int)(screenY + 6) - 4, 8, 8) == true)
                {
                    if (EQ_IsMouseHoveringOverCXWnd() == false)
                    {
                        // remove the "+ "
                        if (strlen(spawnText) > 2)
                        {
                            memmove(spawnText, spawnText + 2, strlen(spawnText));
                        }

                        int textWidth = EQ_GetFontTextWidth(EQ_POINTER_FONT_ARIAL14, spawnText);

                        textWidth = textWidth + 1;

                        EQ_DrawRectangle((float)(mouseX + EQ_MOUSE_CURSOR_WIDTH + 1), (float)mouseY, (float)textWidth, (float)g_fontHeight, 0x80000000, true);

                        EQ_CLASS_CDisplay->WriteTextHD2(spawnText, (int)(mouseX + EQ_MOUSE_CURSOR_WIDTH + 1), (int)mouseY, g_espDoorSpawnTextColor, fontArial14);
                    }
                }
            }
        }

        doorSpawn = doorSpawn->Next;
    }

    groundSpawn = (PEQGROUNDSPAWNINFO)EQ_OBJECT_FirstGroundSpawn;

    while (g_espShowGroundSpawnsIsEnabled == true && groundSpawn)
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
            int viewPortX1 = EQ_OBJECT_ViewPort.X1;
            int viewPortY1 = EQ_OBJECT_ViewPort.Y1;
            int viewPortX2 = EQ_OBJECT_ViewPort.X2;
            int viewPortY2 = EQ_OBJECT_ViewPort.Y2;

            int screenX = (int)(resultX + viewPortX1);
            int screenY = (int)(resultY + viewPortY1);

            if (g_espClipToViewPortIsEnabled == true && (EQ_OBJECT_ViewPort.X1 != 0 || EQ_OBJECT_ViewPort.Y1 != 0))
            {
                if
                (
                    EQMACMQ_IsPointInsideRectangle
                    (
                        screenX, screenY,
                        viewPortX1,  viewPortY1,
                        viewPortX2 - viewPortX1,
                        viewPortY2 - viewPortY1
                    )
                    == false
                )
                {
                    groundSpawn = groundSpawn->Next;
                    continue;
                }
            }

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
            _snprintf_s(spawnText, sizeof(spawnText), _TRUNCATE, "+ %s", spawnName);

            if (g_espShowTextDistanceIsEnabled == true)
            {
                char distanceText[32];
                _snprintf_s(distanceText, sizeof(distanceText), _TRUNCATE, " (%d)", (int)spawnDistance);
                strncat_s(spawnText, sizeof(spawnText), distanceText, _TRUNCATE);
            }

            if (g_espShowTextIsEnabled == false)
            {
                EQ_CLASS_CDisplay->WriteTextHD2("+", screenX, screenY, g_espGroundSpawnTextColor, fontArial14);
            }
            else
            {
                EQ_CLASS_CDisplay->WriteTextHD2(spawnText, screenX, screenY, g_espGroundSpawnTextColor, fontArial14);
            }

            if (g_espShowTextIsEnabled == false && g_espShowTextOnMouseHoverIsEnabled == true)
            {
                if (EQMACMQ_IsPointInsideRectangle(mouseX, mouseY, (int)(screenX + 1) - 4, (int)(screenY + 6) - 4, 8, 8) == true)
                {
                    if (EQ_IsMouseHoveringOverCXWnd() == false)
                    {
                        // remove the "+ "
                        if (strlen(spawnText) > 2)
                        {
                            memmove(spawnText, spawnText + 2, strlen(spawnText));
                        }

                        int textWidth = EQ_GetFontTextWidth(EQ_POINTER_FONT_ARIAL14, spawnText);

                        textWidth = textWidth + 1;

                        EQ_DrawRectangle((float)(mouseX + EQ_MOUSE_CURSOR_WIDTH + 1), (float)mouseY, (float)textWidth, (float)g_fontHeight, 0x80000000, true);

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
            int viewPortX1 = EQ_OBJECT_ViewPort.X1;
            int viewPortY1 = EQ_OBJECT_ViewPort.Y1;
            int viewPortX2 = EQ_OBJECT_ViewPort.X2;
            int viewPortY2 = EQ_OBJECT_ViewPort.Y2;

            int screenX = (int)(resultX + viewPortX1);
            int screenY = (int)(resultY + viewPortY1);

            if (g_espClipToViewPortIsEnabled == true && (EQ_OBJECT_ViewPort.X1 != 0 || EQ_OBJECT_ViewPort.Y1 != 0))
            {
                if
                (
                    EQMACMQ_IsPointInsideRectangle
                    (
                        screenX, screenY,
                        viewPortX1,  viewPortY1,
                        viewPortX2 - viewPortX1,
                        viewPortY2 - viewPortY1
                    )
                    == false
                )
                {
                    spawn = spawn->Next;
                    continue;
                }
            }

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
                            _snprintf_s(hpText, sizeof(hpText), _TRUNCATE, "HP: %d/%d (%d%%)", hpCurrent, hpMax, hpPercent);

                            EQ_CLASS_CDisplay->WriteTextHD2(hpText, screenX, playerTextY, EQ_TEXT_COLOR_RED, fontArial14);

                            playerTextY = playerTextY + (g_fontHeight - g_fontHeightEmptyPixelsTop);
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
                            _snprintf_s(manaText, sizeof(manaText), _TRUNCATE, "MANA: %d/%d (%d%%)", manaCurrent, manaMax, manaPercent);

                            EQ_CLASS_CDisplay->WriteTextHD2(manaText, screenX, playerTextY, EQ_TEXT_COLOR_CYAN, fontArial14);

                            playerTextY = playerTextY + (g_fontHeight - g_fontHeightEmptyPixelsTop);
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
                strncpy_s(filter, sizeof(filter), g_mapSpawnFilterNpc, _TRUNCATE);

                char* filterToken     = NULL;
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
            _snprintf_s(spawnText, sizeof(spawnText), _TRUNCATE, "+ %s", spawnName);

            if (g_espShowTextIsGameMasterIsEnabled == true)
            {
                if (spawn->IsGameMaster == 1)
                {
                    textColor = g_espGameMasterTextColor;

                    strncat_s(spawnText, sizeof(spawnText), " GM", _TRUNCATE);
                }
            }

            if (g_espShowTextIsPlayerKillIsEnabled == true)
            {
                if (spawn->IsPlayerKill == 1)
                {
                    strncat_s(spawnText, sizeof(spawnText), " PVP", _TRUNCATE);
                }
            }

            if (g_espShowTextIsLookingForGroupIsEnabled == true)
            {
                if (spawn->ActorInfo->IsLookingForGroup == 1)
                {
                    strncat_s(spawnText, sizeof(spawnText), " LFG", _TRUNCATE);
                }
            }

            if (g_espShowTextIsAwayFromKeyboardIsEnabled == true)
            {
                if (spawn->IsAwayFromKeyboard == 1)
                {
                    strncat_s(spawnText, sizeof(spawnText), " AFK", _TRUNCATE);
                }
            }

            if (spawn->Type == EQ_SPAWN_TYPE_PLAYER)
            {
                if (g_espShowTextPlayerIsLinkDeadIsEnabled == true)
                {
                    if (spawn->IsLinkDead == 1)
                    {
                        strncat_s(spawnText, sizeof(spawnText), " LD", _TRUNCATE);
                    }
                }

                if (g_espShowTextPlayerIsFeignDeathIsEnabled == true)
                {
                    if (spawn->StandingState == EQ_STANDING_STATE_FEIGNED)
                    {
                        strncat_s(spawnText, sizeof(spawnText), " FD", _TRUNCATE);
                    }
                }
            }

            if (g_espShowTextLevelIsEnabled == true)
            {
                char levelText[32];
                _snprintf_s(levelText, sizeof(levelText), _TRUNCATE, " L%d", spawn->Level);
                strncat_s(spawnText, sizeof(spawnText), levelText, _TRUNCATE);
            }

            if (g_espShowTextPlayerClassIsEnabled == true)
            {
                if (spawn->Type == EQ_SPAWN_TYPE_PLAYER)
                {
                    char classText[32];
                    _snprintf_s(classText, sizeof(classText), _TRUNCATE, " %s", EQ_GetClassShortName(spawn->Class));
                    strncat_s(spawnText, sizeof(spawnText), classText, _TRUNCATE);
                }
            }

            if (g_espShowTextDistanceIsEnabled == true)
            {
                char distanceText[32];
                _snprintf_s(distanceText, sizeof(distanceText), _TRUNCATE, " (%d)", (int)spawnDistance);
                strncat_s(spawnText, sizeof(spawnText), distanceText, _TRUNCATE);
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
                            screenY = screenY + (g_fontHeight - g_fontHeightEmptyPixelsTop);

                            char guildText[128];
                            _snprintf_s(guildText, sizeof(guildText), _TRUNCATE, "<%s>", EQ_GetGuildNameById(spawn->GuildId));

                            EQ_CLASS_CDisplay->WriteTextHD2(guildText, screenX, screenY, textColor, fontArial14);
                        }
                    }

                    if (g_espShowTextPlayerHpIsEnabled == true)
                    {
                        if (spawn->HpCurrent < 100)
                        {
                            screenY = screenY + (g_fontHeight - g_fontHeightEmptyPixelsTop);

                            char hpText[128];
                            _snprintf_s(hpText, sizeof(hpText), _TRUNCATE, "HP: %d%%", spawn->HpCurrent);

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
                            screenY = screenY + (g_fontHeight - g_fontHeightEmptyPixelsTop);

                            char classText[128];
                            _snprintf_s(classText, sizeof(classText), _TRUNCATE, "(%s)", EQ_STRING_CLASS_NAME[spawn->Class]);

                            EQ_CLASS_CDisplay->WriteTextHD2(classText, screenX, screenY, textColor, fontArial14);
                        }
                    }

                    if (g_espShowTextNpcHpIsEnabled == true)
                    {
                        if (spawn->HpCurrent < 100)
                        {
                            screenY = screenY + (g_fontHeight - g_fontHeightEmptyPixelsTop);

                            char hpText[128];
                            _snprintf_s(hpText, sizeof(hpText), _TRUNCATE, "HP: %d%%", spawn->HpCurrent);

                            EQ_CLASS_CDisplay->WriteTextHD2(hpText, screenX, screenY, textColor, fontArial14);
                        }
                    }
                }
            }

            if (g_espShowTextIsEnabled == false && g_espShowTextOnMouseHoverIsEnabled == true)
            {
                if (EQMACMQ_IsPointInsideRectangle(mouseX, mouseY, (int)(screenX + 1) - 4, (int)(screenY + 6) - 4, 8, 8) == true)
                {
                    if (EQ_IsMouseHoveringOverCXWnd() == false)
                    {
                        // remove the "+ "
                        if (strlen(spawnText) > 2)
                        {
                            memmove(spawnText, spawnText + 2, strlen(spawnText));
                        }

                        int textWidth = EQ_GetFontTextWidth(EQ_POINTER_FONT_ARIAL14, spawnText);

                        textWidth = textWidth + 1;

                        EQ_DrawRectangle((float)(mouseX + EQ_MOUSE_CURSOR_WIDTH + 1), (float)mouseY, (float)textWidth, (float)g_fontHeight, 0x80000000, true);

                        EQ_CLASS_CDisplay->WriteTextHD2(spawnText, (int)mouseX + EQ_MOUSE_CURSOR_WIDTH + 1, (int)mouseY, textColor, fontArial14);
                    }
                }
            }
        }

        spawn = spawn->Next;
    }
}

void EQMACMQ_DoBuffInfo()
{
    DWORD fontArial14 = EQ_ReadMemory<DWORD>(EQ_POINTER_FONT_ARIAL14);

    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    EQ_CLASS_CDisplay->WriteTextHD2("Buffs", (int)g_buffInfoX, (int)g_buffInfoY, g_buffInfoTextColor, fontArial14);

    int buffY = (int)(g_buffInfoY) + (g_fontHeight - g_fontHeightEmptyPixelsTop);

    for (size_t i = 0; i < EQ_NUM_BUFFS; i++)
    {
        int spellId = charInfo->Buff[i].SpellId;

        if (spellId == EQ_SPELL_ID_NULL)
        {
            continue;
        }

        const char* spellName = EQ_OBJECT_SpellList->Spell[spellId]->Name;

        if (strlen(spellName) == 0 || spellName == NULL)
        {
            continue;
        }

        int buffTicks = charInfo->Buff[i].Ticks;

        if (buffTicks == 0)
        {
            continue;
        }

        int buffHours   = 0;
        int buffMinutes = 0;
        int buffSeconds = 0;

        EQ_CalculateTickTime(buffTicks, buffHours, buffMinutes, buffSeconds);

        int textColor = g_buffInfoTextColor;

        int buffType = EQ_OBJECT_SpellList->Spell[spellId]->BuffType;

        if (buffType == EQ_BUFF_TYPE_DETRIMENTAL)
        {
            textColor = g_buffInfoDetrimentalTextColor;
        }
        else
        {
            textColor = g_buffInfoBeneficialTextColor;
        }

        char buffText[128];
        _snprintf_s(buffText, sizeof(buffText), _TRUNCATE, "%02d: (%02d:%02d:%02d) %s", i + 1, buffHours, buffMinutes, buffSeconds, spellName);

        EQ_CLASS_CDisplay->WriteTextHD2(buffText, (int)g_buffInfoX, (int)buffY, textColor, fontArial14);

        buffY = buffY + (g_fontHeight - g_fontHeightEmptyPixelsTop);
    }
}

void EQMACMQ_DoPlayerInfo()
{
    DWORD fontArial14 = EQ_ReadMemory<DWORD>(EQ_POINTER_FONT_ARIAL14);

    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    if (playerSpawn == NULL)
    {
        return;
    }

    EQ_CLASS_CDisplay->WriteTextHD2("Player", (int)g_playerInfoX, (int)g_playerInfoY, g_playerInfoTextColor, fontArial14);

    int playerTextY = (int)(g_playerInfoY) + (g_fontHeight - g_fontHeightEmptyPixelsTop);

    int hpCurrent = playerSpawn->HpCurrent;
    int hpMax     = playerSpawn->HpMax;

    if (hpCurrent > 0 && hpMax > 0)
    {
        int hpPercent = (hpCurrent * 100) / hpMax;

        char hpText[128];
        _snprintf_s(hpText, sizeof(hpText), _TRUNCATE, "HP: %d/%d (%d%%)", hpCurrent, hpMax, hpPercent);

        EQ_CLASS_CDisplay->WriteTextHD2(hpText, (int)g_playerInfoX, (int)playerTextY, g_playerInfoTextColor, fontArial14);

        playerTextY = playerTextY + (g_fontHeight - g_fontHeightEmptyPixelsTop);
    }

    if (charInfo)
    {
        int manaCurrent = charInfo->Mana;
        int manaMax     = EQ_CLASS_EQ_Character->Max_Mana();

        if (manaCurrent > 0 && manaMax > 0)
        {
            int manaPercent = (manaCurrent * 100) / manaMax;

            char manaText[128];
            _snprintf_s(manaText, sizeof(manaText), _TRUNCATE, "MANA: %d/%d (%d%%)", manaCurrent, manaMax, manaPercent);

            EQ_CLASS_CDisplay->WriteTextHD2(manaText, (int)g_playerInfoX, (int)playerTextY, g_playerInfoTextColor, fontArial14);

            playerTextY = playerTextY + (g_fontHeight - g_fontHeightEmptyPixelsTop);
        }
    }
}

void EQMACMQ_DoTargetInfo()
{
    DWORD fontArial14 = EQ_ReadMemory<DWORD>(EQ_POINTER_FONT_ARIAL14);

    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    if (targetSpawn == NULL)
    {
        return;
    }

    const char* targetName = EQ_CLASS_CEverQuest->trimName(targetSpawn->Name);

    char targetText[128];
    _snprintf_s(targetText, sizeof(targetText), _TRUNCATE, "Target: %s", targetName);

    if (targetSpawn->IsPlayerKill == 1)
    {
        strncat_s(targetText, sizeof(targetText), " PVP", _TRUNCATE);
    }

    if (targetSpawn->ActorInfo->IsLookingForGroup == 1)
    {
        strncat_s(targetText, sizeof(targetText), " LFG", _TRUNCATE);
    }

    if (targetSpawn->IsAwayFromKeyboard == 1)
    {
        strncat_s(targetText, sizeof(targetText), " AFK", _TRUNCATE);
    }

    if (targetSpawn->Type == EQ_SPAWN_TYPE_PLAYER)
    {
        if (targetSpawn->IsLinkDead == 1)
        {
            strncat_s(targetText, sizeof(targetText), " LD", _TRUNCATE);
        }

        if (targetSpawn->StandingState == EQ_STANDING_STATE_FEIGNED)
        {
            strncat_s(targetText, sizeof(targetText), " FD", _TRUNCATE);
        }
    }

    EQ_CLASS_CDisplay->WriteTextHD2(targetText, (int)g_targetInfoX, (int)g_targetInfoY, g_targetInfoTextColor, fontArial14);

    int targetTextY = (int)(g_targetInfoY) + (g_fontHeight - g_fontHeightEmptyPixelsTop);

    const char* raceShortName = EQ_GetRaceShortName(targetSpawn->Race);

    const char* classShortName = EQ_GetClassShortName(targetSpawn->Class);

    char levelRaceClassText[128];
    _snprintf_s(levelRaceClassText, sizeof(levelRaceClassText), _TRUNCATE, "L%d %s %s", targetSpawn->Level, raceShortName, classShortName);

    EQ_CLASS_CDisplay->WriteTextHD2(levelRaceClassText, (int)g_targetInfoX, (int)targetTextY, g_targetInfoTextColor, fontArial14);

    targetTextY = targetTextY + (g_fontHeight - g_fontHeightEmptyPixelsTop);

    if (targetSpawn->GuildId != EQ_GUILD_ID_NULL)
    {
        char guildText[128];
        _snprintf_s(guildText, sizeof(guildText), _TRUNCATE, "<%s>", EQ_GetGuildNameById(targetSpawn->GuildId));

        EQ_CLASS_CDisplay->WriteTextHD2(guildText, (int)g_targetInfoX, (int)targetTextY, g_targetInfoTextColor, fontArial14);

        targetTextY = targetTextY + (g_fontHeight - g_fontHeightEmptyPixelsTop);
    }

    if (targetSpawn->HpCurrent < 100)
    {
        char hpText[128];

        if (targetSpawn == playerSpawn)
        {
            int hpCurrent = targetSpawn->HpCurrent;
            int hpMax     = targetSpawn->HpMax;

            if (hpCurrent > 0 && hpMax > 0)
            {
                int hpPercent = (hpCurrent * 100) / hpMax;

                _snprintf_s(hpText, sizeof(hpText), _TRUNCATE, "HP: %d/%d (%d%%)", hpCurrent, hpMax, hpPercent);
            }
        }
        else
        {
            int hpCurrent = targetSpawn->HpCurrent;

            if (hpCurrent > 0)
            {
                _snprintf_s(hpText, sizeof(hpText), _TRUNCATE, "HP: %d%%", targetSpawn->HpCurrent);
            }
        }

        EQ_CLASS_CDisplay->WriteTextHD2(hpText, (int)g_targetInfoX, (int)targetTextY, g_targetInfoTextColor, fontArial14);

        targetTextY = targetTextY + (g_fontHeight - g_fontHeightEmptyPixelsTop);
    }

    if (targetSpawn == playerSpawn && charInfo)
    {
        int manaCurrent = charInfo->Mana;
        int manaMax     = EQ_CLASS_EQ_Character->Max_Mana();

        if (manaCurrent > 0 && manaMax > 0)
        {
            int manaPercent = (manaCurrent * 100) / manaMax;

            char manaText[128];
            _snprintf_s(manaText, sizeof(manaText), _TRUNCATE, "MANA: %d/%d (%d%%)", manaCurrent, manaMax, manaPercent);

            EQ_CLASS_CDisplay->WriteTextHD2(manaText, (int)g_targetInfoX, (int)targetTextY, g_targetInfoTextColor, fontArial14);

            targetTextY = targetTextY + (g_fontHeight - g_fontHeightEmptyPixelsTop);
        }

        float experiencePercent = (float)((charInfo->Experience * 100.0f) / EQ_EXPERIENCE_MAX);

        char experienceText[128];
        _snprintf_s(experienceText, sizeof(experienceText), _TRUNCATE, "EXP: %d/%d (%.0f%%)", charInfo->Experience, EQ_EXPERIENCE_MAX, experiencePercent);

        EQ_CLASS_CDisplay->WriteTextHD2(experienceText, (int)g_targetInfoX, (int)targetTextY, g_targetInfoTextColor, fontArial14);

        targetTextY = targetTextY + (g_fontHeight - g_fontHeightEmptyPixelsTop);
    }
}

void EQMACMQ_DoHealthBars()
{
    WORD mouseX = EQ_ReadMemory<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_ReadMemory<WORD>(EQ_MOUSE_Y);

    DWORD mouseClickState = EQ_ReadMemory<DWORD>(EQ_MOUSE_CLICK_STATE);

    DWORD worldSpaceToScreenSpaceCameraData = EQ_ReadMemory<DWORD>(EQ_POINTER_WORLD_SPACE_TO_SCREEN_SPACE_CAMERA_DATA);

    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    EQLOCATION playerLocation = {playerSpawn->Y, playerSpawn->X, playerSpawn->Z};

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

        if (hpPercent == 0.0f || hpPercent == 1.0f)
        {
            spawn = spawn->Next;
            continue;
        }

        float manaPercent = 0.0f;

        if (charInfo)
        {
            int manaCurrent = charInfo->Mana;
            int manaMax     = EQ_CLASS_EQ_Character->Max_Mana();

            if (manaCurrent > 0 && manaMax > 0 && manaCurrent < manaMax)
            {
                manaPercent = (float)manaCurrent / (float)manaMax;
            }
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

        DWORD healthBarForegroundColor = EQMACMQ_HEALTH_BAR_DEFAULT_FOREGROUND_COLOR;

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

        DWORD healthBarManaColor = EQMACMQ_HEALTH_BAR_DEFAULT_MANA_COLOR;

        if (spawn == playerSpawn)
        {
            healthBarManaColor = g_healthBarsManaColor;
        }

        float resultX = 0.0f;
        float resultY = 0.0f;
        int result = EQGfx_Dx8__t3dWorldSpaceToScreenSpace(worldSpaceToScreenSpaceCameraData, &healthBarLocation, &resultX, &resultY);

        if (result != EQ_WORLD_SPACE_TO_SCREEN_SPACE_RESULT_FAILURE)
        {
            int viewPortX1 = EQ_OBJECT_ViewPort.X1;
            int viewPortY1 = EQ_OBJECT_ViewPort.Y1;
            int viewPortX2 = EQ_OBJECT_ViewPort.X2;
            int viewPortY2 = EQ_OBJECT_ViewPort.Y2;

            float screenX = (float)(resultX + viewPortX1);
            float screenY = (float)(resultY + viewPortY1);

            screenX = screenX - g_healthBarsWidth * 0.5f;

            if (g_healthBarsClipToViewPortIsEnabled == true && (EQ_OBJECT_ViewPort.X1 != 0 || EQ_OBJECT_ViewPort.Y1 != 0))
            {
                if
                (
                    EQMACMQ_IsPointInsideRectangle
                    (
                        (int)screenX, (int)screenY,
                        viewPortX1,  viewPortY1,
                        viewPortX2 - viewPortX1,
                        viewPortY2 - viewPortY1
                    )
                    == false
                )
                {
                    spawn = spawn->Next;
                    continue;
                }
            }

            EQ_DrawRectangle(screenX, screenY, g_healthBarsWidth, g_healthBarsHeight, g_healthBarsBackgroundColor, true);
            EQ_DrawRectangle(screenX + 1.0f, screenY + 1.0f, (g_healthBarsWidth - 2.0f) * hpPercent, g_healthBarsHeight - 2.0f, healthBarForegroundColor, true);

            if (spawn == playerSpawn)
            {
                if (manaPercent > 0.0f)
                {
                    EQ_DrawRectangle(screenX, screenY + g_healthBarsHeight, g_healthBarsWidth, g_healthBarsHeight * 0.5f, g_healthBarsBackgroundColor, true);
                    EQ_DrawRectangle(screenX + 1.0f, screenY + 1.0f + g_healthBarsHeight, (g_healthBarsWidth - 2.0f) * manaPercent, (g_healthBarsHeight * 0.5f) - 2.0f, healthBarManaColor, true);
                 }
             }

            if (g_healthBarsClickToTargetIsEnabled == true)
            {
                if (EQMACMQ_IsPointInsideRectangle(mouseX, mouseY, (int)screenX, (int)screenY, (int)g_healthBarsWidth, (int)g_healthBarsHeight) == true)
                {
                    if (EQ_IsMouseHoveringOverCXWnd() == false)
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

void EQMACMQ_DoMessageText()
{
    if (strlen(g_messageText) == 0)
    {
        return;
    }

    DWORD currentTime = EQ_ReadMemory<DWORD>(EQ_TIMER);

    if (currentTime < g_messageTextTimer)
    {
        DWORD fontArial16 = EQ_ReadMemory<DWORD>(EQ_POINTER_FONT_ARIAL16);

        int textX = (int)g_messageTextX;

        if (g_messageTextIsCentered == true)
        {
            int textWidth = EQ_GetFontTextWidth(EQ_POINTER_FONT_ARIAL16, g_messageText);

            textX = textX - (int)(textWidth * 0.5f);
        }

        EQ_CLASS_CDisplay->WriteTextHD2(g_messageText, (int)textX, (int)g_messageTextY, g_messageTextColor, fontArial16);
    }
}

void EQMACMQ_DoMessageTextGeneric(const char* text, int textColor)
{
    strncpy_s(g_messageText, sizeof(g_messageText), text, _TRUNCATE);

    g_messageTextColor = textColor;

    DWORD currentTime = EQ_ReadMemory<DWORD>(EQ_TIMER);

    g_messageTextTimer = currentTime + g_messageTextDelay;
}

void EQMACMQ_DoMessageTextGainedExperience()
{
    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    if (charInfo == NULL)
    {
        return;
    }

    g_playerLevelCurrent = charInfo->Level;
    g_playerExperienceCurrent = charInfo->Experience;

    if (g_playerLevelPrevious == 0)
    {
        g_playerLevelPrevious = g_playerLevelCurrent;
    }

    if (g_playerExperiencePrevious == 0)
    {
        g_playerExperiencePrevious = g_playerExperienceCurrent;
    }

    unsigned int differenceExperience = 0;

    bool bLostExperience = false;

    if ((g_playerExperienceCurrent > g_playerExperiencePrevious) || (g_playerExperienceCurrent > 0 && g_playerExperiencePrevious == 0))
    {
        // gained experience
        if (g_playerLevelCurrent == g_playerLevelPrevious)
        {
            differenceExperience = g_playerExperienceCurrent - g_playerExperiencePrevious;
        }
        // level down
        //else if (g_playerLevelCurrent < g_playerLevelPrevious)
        //{
            //differenceExperience = (EQ_EXPERIENCE_MAX - g_playerExperienceCurrent) + g_playerExperiencePrevious;

            //bLostExperience = true;
        //}
    }
    else if (g_playerExperienceCurrent < g_playerExperiencePrevious)
    {
        // lost experience
        //if (g_playerLevelCurrent == g_playerLevelPrevious)
        //{
            //differenceExperience = g_playerExperiencePrevious - g_playerExperienceCurrent;

            //bLostExperience = true;
        //}
        // level up
        if (g_playerLevelCurrent > g_playerLevelPrevious)
        {
            differenceExperience = (EQ_EXPERIENCE_MAX - g_playerExperiencePrevious) + g_playerExperienceCurrent;
        }
    }

    if (differenceExperience > 0)
    {
        float differenceExperiencePercent = (float)((differenceExperience * 100.0f) / EQ_EXPERIENCE_MAX);

        if (bLostExperience == false)
        {
            _snprintf_s(g_messageText, sizeof(g_messageText), _TRUNCATE, "You gained %d experience! (%.0f%%)", differenceExperience, differenceExperiencePercent);
        }
        else
        {
            _snprintf_s(g_messageText, sizeof(g_messageText), _TRUNCATE, "You lost %d experience! (%.0f%%)", differenceExperience, differenceExperiencePercent);
        }

        g_messageTextColor = EQ_TEXT_COLOR_YELLOW;

        DWORD currentTime = EQ_ReadMemory<DWORD>(EQ_TIMER);

        g_messageTextTimer = currentTime + g_messageTextDelay;

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            EQ_CLASS_CEverQuest->dsp_chat(g_messageText, EQ_TEXT_COLOR_YELLOW, true);

            float experiencePercent = (float)((charInfo->Experience * 100.0f) / EQ_EXPERIENCE_MAX);

            char experienceText[128];
            _snprintf_s(experienceText, sizeof(experienceText), _TRUNCATE, "You have %d/%d experience. (%.0f%%)", charInfo->Experience, EQ_EXPERIENCE_MAX, experiencePercent);

            EQ_CLASS_CEverQuest->dsp_chat(experienceText, EQ_TEXT_COLOR_YELLOW, true);
        }
    }

    g_playerLevelPrevious = g_playerLevelCurrent;

    g_playerExperiencePrevious = g_playerExperienceCurrent;
}

void EQMACMQ_DoBardTwist()
{
    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    if (playerSpawn == NULL)
    {
        return;
    }

    if (playerSpawn->Class != EQ_CLASS_BARD)
    {
        return;
    }

    if (playerSpawn->StandingState != EQ_STANDING_STATE_STANDING)
    {
        return;
    }

    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    if (charInfo == NULL)
    {
        return;
    }

    if (g_bardTwistSongNumbers[0] == -1)
    {
        return;
    }

    DWORD currentTime = EQ_ReadMemory<DWORD>(EQ_TIMER);

    if (currentTime < (g_bardTwistTimer + g_bardTwistDelay))
    {
        return;
    }
    else
    {
        g_bardTwistTimer = currentTime;
    }

    if (g_bardTwistSongIndex >= g_bardTwistSongNumbersMax)
    {
        g_bardTwistSongIndex = 0;
    }

    int gem = g_bardTwistSongNumbers[g_bardTwistSongIndex];

    if (gem == -1)
    {
        g_bardTwistSongIndex = 0;

        gem = g_bardTwistSongNumbers[g_bardTwistSongIndex];
    }

    if (gem != -1)
    {
        int spellId = charInfo->MemorizedSpell[g_bardTwistSongIndex];

        if (spellId != EQ_SPELL_ID_NULL)
        {
            EQ_CLASS_EQ_Character->StopSpellCast(0);
            EQ_CLASS_EQ_Character->CastSpell(gem, spellId, NULL, -1);
        }
    }

    g_bardTwistSongIndex++;
}

void EQMACMQ_DoMelee()
{
    if (EQ_IsAutoAttackEnabled() == false)
    {
        return;
    }

    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    if (playerSpawn == NULL)
    {
        return;
    }

    if (playerSpawn->StandingState != EQ_STANDING_STATE_STANDING)
    {
        return;
    }

    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    if (targetSpawn == NULL)
    {
        return;
    }

    if (targetSpawn->Type == EQ_SPAWN_TYPE_NPC_CORPSE || targetSpawn->Type == EQ_SPAWN_TYPE_PLAYER_CORPSE)
    {
        return;
    }

    DWORD currentTime = EQ_ReadMemory<DWORD>(EQ_TIMER);

    if (currentTime < (g_meleeTimer + g_meleeDelay))
    {
        return;
    }
    else
    {
        g_meleeTimer = currentTime;
    }

    if (g_meleeRangeIsEnabled == true)
    {
        float targetSpawnDistance = EQ_CalculateDistance(playerSpawn->X, playerSpawn->Y, targetSpawn->X, targetSpawn->Y);

        float targetMeleeDistance = EQ_get_melee_range((EQPlayer*)playerSpawn, (EQPlayer*)targetSpawn);

        if (g_loggingIsEnabled == true)
        {
            char logText[1024];
            _snprintf_s(logText, sizeof(logText), _TRUNCATE, "%s | targetSpawnDistance: %f, targetMeleeDistance: %f", __FUNCTION__, targetSpawnDistance, targetMeleeDistance);
            EQMACMQ_Log(logText);
        }

        if (targetSpawnDistance > targetMeleeDistance)
        {
            if (g_loggingIsEnabled == true)
            {
                char logText[1024];
                _snprintf_s(logText, sizeof(logText), _TRUNCATE, "%s | skipping melee because melee range is enabled and targetSpawnDistance > targetMeleeDistance", __FUNCTION__, targetSpawnDistance, targetMeleeDistance);
                EQMACMQ_Log(logText);
            }

            return;
        }
    }

    if (g_meleeSkillTauntIsEnabled == true)
    {
        EQ_CLASS_EQ_Character->UseSkill(EQ_SKILL_TAUNT, (EQPlayer*)targetSpawn);
    }

    if (g_meleeSkillKickIsEnabled == true)
    {
        EQ_CLASS_EQ_Character->UseSkill(EQ_SKILL_KICK, (EQPlayer*)targetSpawn);
    }

    if (g_meleeSkillBashIsEnabled == true)
    {
        EQ_CLASS_EQ_Character->UseSkill(EQ_SKILL_BASH, (EQPlayer*)targetSpawn);
    }

    if (g_meleeSkillSlamIsEnabled == true)
    {
        EQ_CLASS_EQ_Character->UseSkill(EQ_SKILL_SLAM, (EQPlayer*)targetSpawn);
    }

    if (g_meleeSkillDisarmIsEnabled == true)
    {
        EQ_CLASS_EQ_Character->UseSkill(EQ_SKILL_DISARM, (EQPlayer*)targetSpawn);
    }

    if (g_meleeSkillSenseHeadingIsEnabled == true)
    {
        EQ_CLASS_EQ_Character->UseSkill(EQ_SKILL_SENSE_HEADING, (EQPlayer*)targetSpawn);
    }
}

void EQMACMQ_DoBankList()
{
    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    if (charInfo == NULL)
    {
        return;
    }

    EQ_CLASS_CEverQuest->dsp_chat("Bank:");

    for (size_t i = 0; i < EQ_NUM_INVENTORY_BANK_SLOTS; i++)
    {
        PEQITEMINFO item = (PEQITEMINFO)charInfo->InventoryBankItem[i];

        if (item == NULL)
        {
            continue;
        }

        char bankText[128];
        _snprintf_s
        (
            bankText, sizeof(bankText), _TRUNCATE,
            "%02d: %s",
            i + 1, item->Name
        );

        if (item->IsContainer == 0)
        {
            if (item->Common.IsStackable == 1 && item->Common.StackCount > 1)
            {
                char stackCountText[128];
                _snprintf_s
                (
                    stackCountText, sizeof(stackCountText), _TRUNCATE,
                    " x%d",
                    item->Common.StackCount
                );

                strncat_s(bankText, sizeof(bankText), stackCountText, _TRUNCATE);
            }
        }

        EQ_CLASS_CEverQuest->dsp_chat(bankText);

        if (item->IsContainer == 1)
        {
            for (size_t j = 0; j < EQ_NUM_CONTAINER_SLOTS; j++)
            {
                PEQITEMINFO containerItem = (PEQITEMINFO)charInfo->InventoryBankItem[i]->Container.Item[j];

                if (containerItem == NULL)
                {
                    continue;
                }

                char bankTextEx[128];
                _snprintf_s
                (
                    bankTextEx, sizeof(bankTextEx), _TRUNCATE,
                    "- %s",
                    containerItem->Name
                );

                if (containerItem->IsContainer == 0)
                {
                    if (containerItem->Common.IsStackable == 1 && containerItem->Common.StackCount > 1)
                    {
                        char stackCountTextEx[128];
                        _snprintf_s
                        (
                            stackCountTextEx, sizeof(stackCountTextEx), _TRUNCATE,
                            " x%d",
                            containerItem->Common.StackCount
                        );

                        strncat_s(bankTextEx, sizeof(bankTextEx), stackCountTextEx, _TRUNCATE);
                    }
                }

                EQ_CLASS_CEverQuest->dsp_chat(bankTextEx);
            }
        }
    }
}

void EQMACMQ_DoMouseWheelZoom(int mouseWheelDelta)
{
    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

    if (playerSpawn == NULL)
    {
        return;
    }

    DWORD cameraView = EQ_ReadMemory<DWORD>(EQ_CAMERA_VIEW);

    FLOAT cameraThirdPersonZoom = 0.0f;

    FLOAT cameraThirdPersonZoomMax = EQ_ReadMemory<FLOAT>(EQ_CAMERA_VIEW_THIRD_PERSON_ZOOM_MAX);

    float zoom = 0.0f;

    DWORD zoomAddress = NULL;

    if (cameraView == EQ_CAMERA_VIEW_THIRD_PERSON2)
    {
        cameraThirdPersonZoom = EQ_ReadMemory<FLOAT>(EQ_CAMERA_VIEW_THIRD_PERSON2_ZOOM);

        zoomAddress = EQ_CAMERA_VIEW_THIRD_PERSON2_ZOOM;
    }
    else if (cameraView == EQ_CAMERA_VIEW_THIRD_PERSON3)
    {
        cameraThirdPersonZoom = EQ_ReadMemory<FLOAT>(EQ_CAMERA_VIEW_THIRD_PERSON3_ZOOM);

        zoomAddress = EQ_CAMERA_VIEW_THIRD_PERSON3_ZOOM;
    }
    else if (cameraView == EQ_CAMERA_VIEW_THIRD_PERSON4)
    {
        cameraThirdPersonZoom = EQ_ReadMemory<FLOAT>(EQ_CAMERA_VIEW_THIRD_PERSON4_ZOOM);

        zoomAddress = EQ_CAMERA_VIEW_THIRD_PERSON4_ZOOM;
    }

    if (mouseWheelDelta == EQ_MOUSE_WHEEL_DELTA_UP)
    {
        if
        (
            cameraView == EQ_CAMERA_VIEW_THIRD_PERSON2 ||
            cameraView == EQ_CAMERA_VIEW_THIRD_PERSON3 ||
            cameraView == EQ_CAMERA_VIEW_THIRD_PERSON4
        )
        {
            if (cameraThirdPersonZoom <= playerSpawn->ModelHeightOffset)
            {
                if (cameraView == EQ_CAMERA_VIEW_THIRD_PERSON2)
                {
                    EQ_WriteMemory<DWORD>(EQ_CAMERA_VIEW, EQ_CAMERA_VIEW_FIRST_PERSON);
                }
                else
                {
                    if (zoomAddress != NULL)
                    {
                        zoom = playerSpawn->ModelHeightOffset;

                        EQ_WriteMemory<FLOAT>(zoomAddress, zoom);
                    }
                }
            }
            else
            {
                if (zoomAddress != NULL)
                {
                    zoom = cameraThirdPersonZoom - (playerSpawn->ModelHeightOffset * g_mouseWheelZoomMultiplier);

                    if (zoom < playerSpawn->ModelHeightOffset)
                    {
                        zoom = playerSpawn->ModelHeightOffset;
                    }

                    EQ_WriteMemory<FLOAT>(zoomAddress, zoom);
                }
            }
        }
    }
    else if (mouseWheelDelta == EQ_MOUSE_WHEEL_DELTA_DOWN)
    {
        if (cameraView == EQ_CAMERA_VIEW_FIRST_PERSON)
        {
            zoom = playerSpawn->ModelHeightOffset;

            EQ_WriteMemory<FLOAT>(EQ_CAMERA_VIEW_THIRD_PERSON2_ZOOM, zoom);

            EQ_WriteMemory<DWORD>(EQ_CAMERA_VIEW, EQ_CAMERA_VIEW_THIRD_PERSON2);
        }
        else if
        (
            cameraView == EQ_CAMERA_VIEW_THIRD_PERSON2 ||
            cameraView == EQ_CAMERA_VIEW_THIRD_PERSON3 ||
            cameraView == EQ_CAMERA_VIEW_THIRD_PERSON4
        )
        {
            if (zoomAddress != NULL)
            {
                zoom = cameraThirdPersonZoom + (playerSpawn->ModelHeightOffset * g_mouseWheelZoomMultiplier);

                if (zoom > cameraThirdPersonZoomMax)
                {
                    zoom = cameraThirdPersonZoomMax;
                }

                EQ_WriteMemory<FLOAT>(zoomAddress, zoom);
            }
        }
    }
}

int __fastcall EQMACMQ_DETOUR_CEverQuest__LMouseDown(void* this_ptr, void* not_used, unsigned short a1, unsigned short a2)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_CEverQuest__LMouseDown(this_ptr, a1, a2);
    }

    if (EQ_IsInGame() == true && EQ_IsNetStatusEnabled() == true)
    {
        if (EQMACMQ_DoButtonMouseLeftDown() == true)
        {
            return EQMACMQ_REAL_CEverQuest__LMouseDown(this_ptr, 0xFFFF, 0xFFFF);
        }
    }

    return EQMACMQ_REAL_CEverQuest__LMouseDown(this_ptr, a1, a2);
}

int __fastcall EQMACMQ_DETOUR_CEverQuest__LMouseUp(void* this_ptr, void* not_used, unsigned short a1, unsigned short a2)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_CEverQuest__LMouseUp(this_ptr, a1, a2);
    }

    if (EQ_IsInGame() == true && EQ_IsNetStatusEnabled() == true)
    {
        if (EQMACMQ_DoButtonMouseLeftUp() == true)
        {
            return EQMACMQ_REAL_CEverQuest__LMouseUp(this_ptr, 0xFFFF, 0xFFFF);
        }
    }

    return EQMACMQ_REAL_CEverQuest__LMouseUp(this_ptr, a1, a2);
}

int __cdecl EQMACMQ_DETOUR_HandleMouseWheel(int a1)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_HandleMouseWheel(a1);
    }

    if (EQ_IsInGame() == false)
    {
        return EQMACMQ_REAL_HandleMouseWheel(a1);
    }

    if (EQ_IsNetStatusEnabled() == false)
    {
        return EQMACMQ_REAL_HandleMouseWheel(a1);
    }

    if (EQ_IsMouseHoveringOverCXWnd() == true)
    {
        return EQMACMQ_REAL_HandleMouseWheel(a1);
    }

    int mouseWheelDelta = a1;

    WORD mouseX = EQ_ReadMemory<WORD>(EQ_MOUSE_X);
    WORD mouseY = EQ_ReadMemory<WORD>(EQ_MOUSE_Y);

    if
    (
        EQMACMQ_IsPointInsideRectangle
        (
            mouseX, mouseY,
            (int)g_mapX,     (int)g_mapY,
            (int)g_mapWidth, (int)g_mapHeight
        )
        == true
        &&
        g_mapIsEnabled == true
    )
    {
        if (g_mapMouseWheelZoomIsEnabled == true)
        {
            if (mouseWheelDelta == EQ_MOUSE_WHEEL_DELTA_UP)
            {
                EQMACMQ_MapMouseWheelZoomIn();
            }
            else if (mouseWheelDelta == EQ_MOUSE_WHEEL_DELTA_DOWN)
            {
                EQMACMQ_MapMouseWheelZoomOut();
            }
        }
    }
    else
    {
        if (g_mouseWheelZoomIsEnabled == true)
        {
            EQMACMQ_DoMouseWheelZoom(mouseWheelDelta);
        }
    }

    return EQMACMQ_REAL_HandleMouseWheel(a1);
}

int __cdecl EQMACMQ_DETOUR_ProcessMovementKeys(int a1)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_ProcessMovementKeys(a1);
    }

    if (a1 > 0xFF)
    {
        return EQMACMQ_REAL_ProcessKeyDown(EQ_KEY_NULL);
    }

    int key = a1;

    //char keyText[128];
    //_snprintf_s(keyText, sizeof(keyText), _TRUNCATE, "ProcessMovementKeys: %d", key);
    //EQ_CLASS_CEverQuest->dsp_chat(keyText);

    if (g_freeCameraIsEnabled == true)
    {
        if (EQ_IsNotTypingInChat() == true)
        {
            if (EQ_IsKeyPressedControl() == false && EQ_IsKeyPressedAlt() == false && EQ_IsKeyPressedShift() == false)
            {
                if (key == EQ_KEY_UP_ARROW || key == EQ_KEY_DOWN_ARROW || key == EQ_KEY_LEFT_ARROW || key == EQ_KEY_RIGHT_ARROW)
                {
                    return EQMACMQ_REAL_ProcessMovementKeys(EQ_KEY_NULL);
                }
            }
        }
    }

    return EQMACMQ_REAL_ProcessMovementKeys(a1);
}

int __cdecl EQMACMQ_DETOUR_ProcessKeyDown(int a1)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_ProcessKeyDown(a1);
    }

    if (a1 > 0xFF)
    {
        return EQMACMQ_REAL_ProcessKeyDown(EQ_KEY_NULL);
    }

    if (g_ignoreKeysIsEnabled == true)
    {
        if (g_ignoreKeysTimer < g_ignoreKeysDelay)
        {
            return EQMACMQ_REAL_ProcessKeyDown(EQ_KEY_NULL);
        }
    }

    int key = a1;

    //char keyText[128];
    //_snprintf_s(keyText, sizeof(keyText), _TRUNCATE, "ProcessKeyDown: %d", key);
    //EQ_CLASS_CEverQuest->dsp_chat(keyText);

    if (g_freeCameraIsEnabled == true)
    {
        if (EQ_IsNotTypingInChat() == true)
        {
            if (EQ_IsKeyPressedControl() == false && EQ_IsKeyPressedAlt() == false && EQ_IsKeyPressedShift() == false)
            {
                if (key == EQ_KEY_UP_ARROW || key == EQ_KEY_DOWN_ARROW || key == EQ_KEY_LEFT_ARROW || key == EQ_KEY_RIGHT_ARROW)
                {
                    return EQMACMQ_REAL_ProcessKeyDown(EQ_KEY_NULL);
                }
            }
        }
    }

    if (g_closeLootWindowWhenMovingIsEnabled == true)
    {
        if (EQ_OBJECT_CLootWnd && EQ_OBJECT_CLootWnd->CSidlWnd.EQWnd.IsOpen == 1 && EQ_OBJECT_CLootWnd->CSidlWnd.EQWnd.IsVisible == 1)
        {
            if (EQ_IsNotTypingInChat() == true)
            {
                if (EQ_IsKeyPressedControl() == false && EQ_IsKeyPressedAlt() == false && EQ_IsKeyPressedShift() == false)
                {
                    if
                    (
                        key == EQ_KEY_UP_ARROW || key == EQ_KEY_DOWN_ARROW || key == EQ_KEY_LEFT_ARROW || key == EQ_KEY_RIGHT_ARROW ||
                        key == EQ_KEY_W        || key == EQ_KEY_S          || key == EQ_KEY_A          || key == EQ_KEY_D
                    )
                    {
                        EQ_CLASS_CLootWnd->Deactivate();
                    }
                }
            }
        }
    }

    if (g_standWhenMovingIsEnabled == true)
    {
        if (EQ_IsNotTypingInChat() == true)
        {
            if (EQ_IsKeyPressedControl() == false && EQ_IsKeyPressedAlt() == false && EQ_IsKeyPressedShift() == false)
            {
                if
                (
                    key == EQ_KEY_UP_ARROW || key == EQ_KEY_DOWN_ARROW || key == EQ_KEY_LEFT_ARROW || key == EQ_KEY_RIGHT_ARROW ||
                    key == EQ_KEY_W        || key == EQ_KEY_S          || key == EQ_KEY_A          || key == EQ_KEY_D
                )
                {
                    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

                    if (playerSpawn != NULL)
                    {
                        if (playerSpawn->StandingState == EQ_STANDING_STATE_SITTING)
                        {
                            ((EQPlayer*)playerSpawn)->ChangePosition(EQ_STANDING_STATE_STANDING);
                        }
                    }
                }
            }
        }
    }

    bool bHotkey = false;

    if (g_clientSwitcherIsEnabled == true && g_clientSwitcherHotkeysIsEnabled == true)
    {
        if
        (
            (g_clientSwitcherHotkeyTildeIsEnabled   == true && key == EQ_KEY_TILDE)   ||
            (g_clientSwitcherHotkeyNumpad0IsEnabled == true && key == EQ_KEY_NUMPAD0) ||
            (g_clientSwitcherHotkeyNumpad1IsEnabled == true && key == EQ_KEY_NUMPAD1) ||
            (g_clientSwitcherHotkeyNumpad2IsEnabled == true && key == EQ_KEY_NUMPAD2) ||
            (g_clientSwitcherHotkeyNumpad3IsEnabled == true && key == EQ_KEY_NUMPAD3) ||
            (g_clientSwitcherHotkeyNumpad4IsEnabled == true && key == EQ_KEY_NUMPAD4) ||
            (g_clientSwitcherHotkeyNumpad5IsEnabled == true && key == EQ_KEY_NUMPAD5) ||
            (g_clientSwitcherHotkeyNumpad6IsEnabled == true && key == EQ_KEY_NUMPAD6)
        )
        {
            bHotkey = true;
        }
    }

    if (bHotkey == true)
    {
        return EQMACMQ_REAL_ProcessKeyDown(EQ_KEY_NULL);
    }

    return EQMACMQ_REAL_ProcessKeyDown(a1);
}

int __cdecl EQMACMQ_DETOUR_ProcessKeyUp(int a1)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_ProcessKeyUp(a1);
    }

    if (a1 > 0xFF)
    {
        return EQMACMQ_REAL_ProcessKeyUp(EQ_KEY_NULL);
    }

    if (g_ignoreKeysIsEnabled == true)
    {
        if (g_ignoreKeysTimer < g_ignoreKeysDelay)
        {
            return EQMACMQ_REAL_ProcessKeyUp(EQ_KEY_NULL);
        }
    }

    int key = a1;

    //char keyText[128];
    //_snprintf_s(keyText, sizeof(keyText), _TRUNCATE, "ProcessKeyUp: %d", key);
    //EQ_CLASS_CEverQuest->dsp_chat(keyText);

    if (g_freeCameraIsEnabled == true)
    {
        if (EQ_IsNotTypingInChat() == true)
        {
            if (EQ_IsKeyPressedControl() == false && EQ_IsKeyPressedAlt() == false && EQ_IsKeyPressedShift() == false)
            {
                if (key == EQ_KEY_UP_ARROW || key == EQ_KEY_DOWN_ARROW || key == EQ_KEY_LEFT_ARROW || key == EQ_KEY_RIGHT_ARROW)
                {
                    return EQMACMQ_REAL_ProcessKeyUp(EQ_KEY_NULL);
                }
            }
        }
    }

    bool bHotkey = false;

    if (g_clientSwitcherIsEnabled == true && g_clientSwitcherHotkeysIsEnabled == true)
    {
        if (g_clientSwitcherHotkeyTildeIsEnabled == true && key == EQ_KEY_TILDE)
        {
            EQMACMQ_DoClientSwitchNext();

            bHotkey = true;
        }

        if (g_clientSwitcherHotkeyNumpad0IsEnabled == true && key == EQ_KEY_NUMPAD0)
        {
            EQMACMQ_UpdateEqProcesses();

            bHotkey = true;
        }

        if (g_clientSwitcherHotkeyNumpad1IsEnabled == true && key == EQ_KEY_NUMPAD1)
        {
            EQMACMQ_DoClientSwitch(0);

            bHotkey = true;
        }

        if (g_clientSwitcherHotkeyNumpad2IsEnabled == true && key == EQ_KEY_NUMPAD2)
        {
            EQMACMQ_DoClientSwitch(1);

            bHotkey = true;
        }

        if (g_clientSwitcherHotkeyNumpad3IsEnabled == true && key == EQ_KEY_NUMPAD3)
        {
            EQMACMQ_DoClientSwitch(2);

            bHotkey = true;
        }

        if (g_clientSwitcherHotkeyNumpad4IsEnabled == true && key == EQ_KEY_NUMPAD4)
        {
            EQMACMQ_DoClientSwitchPrevious();

            bHotkey = true;
        }

        if (g_clientSwitcherHotkeyNumpad5IsEnabled == true && key == EQ_KEY_NUMPAD5)
        {
            EQMACMQ_UpdateEqProcesses();

            bHotkey = true;
        }

        if (g_clientSwitcherHotkeyNumpad6IsEnabled == true && key == EQ_KEY_NUMPAD6)
        {
            EQMACMQ_DoClientSwitchNext();

            bHotkey = true;
        }
    }

    if (bHotkey == true)
    {
        return EQMACMQ_REAL_ProcessKeyUp(EQ_KEY_NULL);
    }

    return EQMACMQ_REAL_ProcessKeyUp(a1);
}

int __cdecl EQMACMQ_DETOUR_AutoInventory(PEQCHARINFO a1, EQITEMINFO** a2, short a3)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_AutoInventory(a1, a2, a3);
    }

/*
    FILE* file = fopen("eqmac_mq-debug-AutoInventory.txt", "a");
    fprintf(file, "a1: 0x%X\n", a1);
    fprintf(file, "a2: 0x%X\n", a2);
    fprintf(file, "a3: %d\n", a3);
    fprintf(file, "\n\n");
    fclose(file);
*/

    return EQMACMQ_REAL_AutoInventory(a1, a2, a3);
}

int __fastcall EQMACMQ_DETOUR_CSpellBookWnd__StartSpellMemorization(void* this_ptr, void* not_used, int a1, int a2, bool a3)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_CSpellBookWnd__StartSpellMemorization(this_ptr, a1, a2, a3);
    }

/*
    FILE* file = fopen("eqmac_mq-debug-CSpellBookWnd__StartSpellMemorization.txt", "a");
    fprintf(file, "a1: %d\n", a1);
    fprintf(file, "a2: %d\n", a2);
    fprintf(file, "a3: %d\n", a3);
    fprintf(file, "a1: 0x%X\n", a1);
    fprintf(file, "a2: 0x%X\n", a2);
    fprintf(file, "\n\n");
    fclose(file);
*/

    return EQMACMQ_REAL_CSpellBookWnd__StartSpellMemorization(this_ptr, a1, a2, a3);
}

int __fastcall EQMACMQ_DETOUR_CSpellBookWnd__FinishMemorizing(void* this_ptr, void* not_used, int a1, int a2)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_CSpellBookWnd__FinishMemorizing(this_ptr, a1, a2);
    }

    int result = EQMACMQ_REAL_CSpellBookWnd__FinishMemorizing(this_ptr, a1, a2);

/*
    FILE* file = fopen("eqmac_mq-debug-CSpellBookWnd__FinishMemorizing.txt", "a");
    fprintf(file, "a1: %d\n", a1);
    fprintf(file, "a2: %d\n", a2);
    fprintf(file, "a1: 0x%X\n", a1);
    fprintf(file, "a2: 0x%X\n", a2);
    fprintf(file, "\n\n");
    fclose(file);
*/

    if (g_loadSpellsetIsInProgress == true)
    {
        if (strlen(g_loadSpellsetName) > 0)
        {
            EQMACMQ_DoLoadSpellset(g_loadSpellsetName);
        }
    }

    return result;
}

int __fastcall EQMACMQ_DETOUR_CLootWnd__Deactivate(void* this_ptr, void* not_used)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_CLootWnd__Deactivate(this_ptr);
    }

    int result = EQMACMQ_REAL_CLootWnd__Deactivate(this_ptr);

    if (g_hideCorpseLootedIsEnabled == true)
    {
        if (EQ_OBJECT_CorpseSpawn)
        {
            PEQSPAWNINFO corpseSpawn = (PEQSPAWNINFO)EQ_OBJECT_CorpseSpawn;

            if (corpseSpawn != NULL)
            {
                corpseSpawn->ActorInfo->IsInvisible = 1;
            }
        }
    }

    return result;
}

int __fastcall EQMACMQ_DETOUR_CItemDisplayWnd__SetItem(void* this_ptr, void* not_used, EQ_Item* a1, bool a2)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_CItemDisplayWnd__SetItem(this_ptr, a1, a2);
    }

    int result = EQMACMQ_REAL_CItemDisplayWnd__SetItem(this_ptr, a1, a2);

    if (g_itemDisplayWindowTextIsEnabled == false || g_itemDisplayWindowTextItemsIsEnabled == false)
    {
        return result;
    }

    if (a1 == NULL)
    {
        return result;
    }

    PEQITEMINFO item = (PEQITEMINFO)a1;

    if (item && EQ_OBJECT_CItemDisplayWnd->DisplayText)
    {
        char itemFilename[1024];
        _snprintf_s(itemFilename, sizeof(itemFilename), _TRUNCATE, ".\\items\\%d.txt", item->Id);

        FILE* file;
        errno_t fileErrorNumber = fopen_s(&file, itemFilename, "rb");

        if (file)
        {
            fseek(file, 0, SEEK_END);
            long fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);

            if (fileSize > 0)
            {
                char* fileContents = (char*)malloc(fileSize + 1);
                fread(fileContents, fileSize, 1, file);
                fileContents[fileSize] = 0;

                EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, "<BR>");
                EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, fileContents);
                EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, "<BR>");

                free(fileContents);
            }

            fclose(file);
        }

        EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, "<BR><c \"#FF00FF\">");

        char itemIdText[128];
        _snprintf_s(itemIdText, sizeof(itemIdText), _TRUNCATE, "ID: %d (0x%08X)<BR>", item->Id, item->Id);

        EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, itemIdText);

        if (item->Cost > 0)
        {
            char itemCostString[128];
            EQ_GetItemCostString(item->Cost, itemCostString, sizeof(itemCostString));

            char itemCostText[128];
            _snprintf_s(itemCostText, sizeof(itemCostText), _TRUNCATE, "Value: %s<BR>", itemCostString);

            EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, itemCostText);
        }

        EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, "</c>");
    }

    return result;
}


int __fastcall EQMACMQ_DETOUR_CItemDisplayWnd__SetSpell(void* this_ptr, void* not_used, short a1, bool a2, int a3)
{
    // a1 = spellId

    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_CItemDisplayWnd__SetSpell(this_ptr, a1, a2, a3);
    }

    int result = EQMACMQ_REAL_CItemDisplayWnd__SetSpell(this_ptr, a1, a2, a3);

    if (g_itemDisplayWindowTextIsEnabled == false || g_itemDisplayWindowTextSpellsIsEnabled == false)
    {
        return result;
    }

    if (a1 == NULL)
    {
        return result;
    }

    PEQSPELLINFO spell = EQ_OBJECT_SpellList->Spell[a1];

    if (spell && EQ_OBJECT_CItemDisplayWnd->DisplayText)
    {
        char spellText[128];

        if (spell->RecastTime > 0)
        {
            float spellRecastTime = (float)(spell->RecastTime / 1000);

            memset(spellText, 0, sizeof(spellText));
            _snprintf_s(spellText, sizeof(spellText), _TRUNCATE, "Recast Time: %.1f<BR>", spellRecastTime);
            EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, spellText);
        }

        if (spell->RecoveryTime > 0)
        {
            float spellRecoveryTime = (float)(spell->RecoveryTime / 1000);

            memset(spellText, 0, sizeof(spellText));
            _snprintf_s(spellText, sizeof(spellText), _TRUNCATE, "Recovery Time: %.1f<BR>", spellRecoveryTime);
            EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, spellText);
        }

        if (spell->Range > 0.0f)
        {
            memset(spellText, 0, sizeof(spellText));
            _snprintf_s(spellText, sizeof(spellText), _TRUNCATE, "Range: %.1f<BR>", spell->Range);
            EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, spellText);
        }

        if (spell->AoeRange > 0.0f)
        {
            memset(spellText, 0, sizeof(spellText));
            _snprintf_s(spellText, sizeof(spellText), _TRUNCATE, "AOE Range: %.1f<BR>", spell->AoeRange);
            EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, spellText);
        }

        if (spell->Duration > 0)
        {
            char spellDurationTimeText[128];
            EQ_GetTickTimeString(spell->Duration, spellDurationTimeText, sizeof(spellDurationTimeText));

            memset(spellText, 0, sizeof(spellText));
            _snprintf_s(spellText, sizeof(spellText), _TRUNCATE, "Duration: %s<BR>", spellDurationTimeText);
            EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, spellText);
        }

        if (spell->AoeDuration > 0)
        {
            char spellAoeDurationTimeText[128];
            EQ_GetTickTimeString(spell->AoeDuration, spellAoeDurationTimeText, sizeof(spellAoeDurationTimeText));

            memset(spellText, 0, sizeof(spellText));
            _snprintf_s(spellText, sizeof(spellText), _TRUNCATE, "AOE Duration: %s<BR>", spellAoeDurationTimeText);
            EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, spellText);
        }

        if (spell->DurationFormula > 0)
        {
            memset(spellText, 0, sizeof(spellText));
            _snprintf_s(spellText, sizeof(spellText), _TRUNCATE, "Duration Formula: %d<BR>", spell->DurationFormula);
            EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, spellText);
        }

        char spellFilename[1024];
        _snprintf_s(spellFilename, sizeof(spellFilename), _TRUNCATE, ".\\spells\\%d.txt", spell->Id);

        FILE* file;
        errno_t fileErrorNumber = fopen_s(&file, spellFilename, "rb");

        if (file)
        {
            fseek(file, 0, SEEK_END);
            long fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);

            if (fileSize > 0)
            {
                char* fileContents = (char*)malloc(fileSize + 1);
                fread(fileContents, fileSize, 1, file);
                fileContents[fileSize] = 0;

                EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, "<BR>");
                EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, fileContents);
                EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, "<BR>");

                free(fileContents);
            }

            fclose(file);
        }

        EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, "<BR><c \"#FF00FF\">");

        memset(spellText, 0, sizeof(spellText));
        _snprintf_s(spellText, sizeof(spellText), _TRUNCATE, "ID: %d (0x%08X)<BR>", spell->Id, spell->Id);
        EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, spellText);

        EQ_CXStr_Append(&EQ_OBJECT_CItemDisplayWnd->DisplayText, "</c>");
    }

    return result;
};

int __fastcall EQMACMQ_DETOUR_CBuffWindow__RefreshBuffDisplay(void* this_ptr, void* not_used)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_CBuffWindow__RefreshBuffDisplay(this_ptr);
    }

    int result = EQMACMQ_REAL_CBuffWindow__RefreshBuffDisplay(this_ptr);

    if (g_buffWindowTimersIsEnabled == false)
    {
        return result;
    }

    PEQCBUFFWINDOW buffWindow = (PEQCBUFFWINDOW)this_ptr;

    PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

    if (charInfo == NULL)
    {
        return result;
    }

    for (size_t i = 0; i < EQ_NUM_BUFFS; i++)
    {
        int spellId = charInfo->Buff[i].SpellId;

        if (spellId == EQ_SPELL_ID_NULL)
        {
            continue;
        }

        int buffTicks = charInfo->Buff[i].Ticks;

        if (buffTicks == 0)
        {
            continue;
        }

        char buffTickTimeText[128];
        EQ_GetTickTimeString(buffTicks, buffTickTimeText, sizeof(buffTickTimeText));

        char buffTimeText[128];
        _snprintf_s(buffTimeText, sizeof(buffTimeText), _TRUNCATE, " (%s)", buffTickTimeText);

        PEQCSIDLWND buffButtonWnd = (PEQCSIDLWND)buffWindow->BuffButtonWnd[i];

        if (buffButtonWnd)
        {
            if (buffButtonWnd->EQWnd.ToolTipText)
            {
                EQ_CXStr_Append(&buffButtonWnd->EQWnd.ToolTipText, buffTimeText);
            }
        }
    }

    return result;
};

int __fastcall EQMACMQ_DETOUR_CDisplay__SetNameSpriteState(void* this_ptr, void* not_used, class EQPlayer* a1, bool a2)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_CDisplay__SetNameSpriteState(this_ptr, a1, a2);
    }

    int result = EQMACMQ_REAL_CDisplay__SetNameSpriteState(this_ptr, a1, a2);

    if (g_nameTextIsEnabled == false)
    {
        return result;
    }

    if (EQ_IsInGame() == false)
    {
        return result;
    }

    //if (EQ_IsShowNpcNamesEnabled() == false)
    //{
        //return result;
    //}

    PEQSPAWNINFO spawn = (PEQSPAWNINFO)a1;

    if (spawn == NULL)
    {
        return result;
    }

    if (spawn->ActorInfo->DagHeadPoint == NULL)
    {
        return result;
    }

    if (spawn->ActorInfo->DagHeadPoint->StringSprite == NULL)
    {
        return result;
    }

    if (spawn->ActorInfo->DagHeadPoint->StringSprite->Text == NULL)
    {
        return result;
    }

    char nameText[128];
    strncpy_s(nameText, sizeof(nameText), spawn->ActorInfo->DagHeadPoint->StringSprite->Text, _TRUNCATE);

    if (spawn->Type == EQ_SPAWN_TYPE_NPC)
    {
        if (g_nameTextNpcLevelAndClassIsEnabled == true)
        {
            char extraText[128];
            _snprintf_s(extraText, sizeof(extraText), _TRUNCATE, "\n(%d %s)", spawn->Level, EQ_GetClassName(spawn->Class));

            strncat_s(nameText, sizeof(nameText), extraText, _TRUNCATE);

            DWORD fontTexture = EQ_GetStringSpriteFontTexture();

            EQ_CLASS_CDisplay->ChangeDagStringSprite(spawn->ActorInfo->DagHeadPoint, fontTexture, nameText);

            EQMACMQ_DETOUR_CDisplay__SetNameSpriteTint(this_ptr, not_used, a1);
        }
    }

    return result;
};

int __fastcall EQMACMQ_DETOUR_CDisplay__SetNameSpriteTint(void* this_ptr, void* not_used, class EQPlayer* a1)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_CDisplay__SetNameSpriteTint(this_ptr, a1);
    }

    int result = EQMACMQ_REAL_CDisplay__SetNameSpriteTint(this_ptr, a1);

    if (g_nameColorsIsEnabled == false)
    {
        return result;
    }

    if (EQ_IsInGame() == false)
    {
        return result;
    }

    PEQSPAWNINFO spawn = (PEQSPAWNINFO)a1;

    if (spawn == NULL)
    {
        return result;
    }

    if (spawn->ActorInfo->DagHeadPoint == NULL)
    {
        return result;
    }

    if (spawn->ActorInfo->DagHeadPoint->StringSprite == NULL)
    {
        return result;
    }

    if (spawn->Type == EQ_SPAWN_TYPE_NPC)
    {
        if (spawn->Class == EQ_CLASS_BANKER)
        {
            if (g_nameColorsBankerIsEnabled == true)
            {
                spawn->ActorInfo->DagHeadPoint->StringSprite->Color = EQ_GetRgbColorFromInt(g_nameColorsBanker);
            }
        }
        else if (spawn->Class == EQ_CLASS_MERCHANT)
        {
            if (g_nameColorsMerchantIsEnabled == true)
            {
                spawn->ActorInfo->DagHeadPoint->StringSprite->Color = EQ_GetRgbColorFromInt(g_nameColorsMerchant);
            }
        }
        else if (spawn->Class >= EQ_CLASS_GUILDMASTER_BEGIN && spawn->Class <= EQ_CLASS_GUILDMASTER_END)
        {
            if (g_nameColorsGuildmasterIsEnabled == true)
            {
                spawn->ActorInfo->DagHeadPoint->StringSprite->Color = EQ_GetRgbColorFromInt(g_nameColorsGuildmaster);
            }
        }
    }
    else if (spawn->Type == EQ_SPAWN_TYPE_NPC_CORPSE)
    {
        if (g_nameColorsNpcCorpseIsEnabled == true)
        {
            spawn->ActorInfo->DagHeadPoint->StringSprite->Color = EQ_GetRgbColorFromInt(g_nameColorsNpcCorpse);
        }
    }
    else if (spawn->Type == EQ_SPAWN_TYPE_PLAYER_CORPSE)
    {
        if (g_nameColorsPlayerCorpseIsEnabled == true)
        {
            spawn->ActorInfo->DagHeadPoint->StringSprite->Color = EQ_GetRgbColorFromInt(g_nameColorsPlayerCorpse);
        }
    }

    PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

    if (targetSpawn == NULL)
    {
        return result;
    }

    if (spawn != targetSpawn)
    {
        return result;
    }

    static int flash;

    int r = spawn->ActorInfo->DagHeadPoint->StringSprite->Color.R;
    int g = spawn->ActorInfo->DagHeadPoint->StringSprite->Color.G;
    int b = spawn->ActorInfo->DagHeadPoint->StringSprite->Color.B;

    int rr = flash * ((255 - r) / 10);
    int gg = flash * ((255 - g) / 10);
    int bb = flash * ((255 - b) / 10);

    int rrr = 0;

    if (255 - rr >= 0)
    {
        rrr = 255 - rr;
    }

    int ggg = 255 - gg;

    if (255 - gg < 0)
    {
        ggg = 0;
    }

    int bbb = 255 - bb;

    if (255 - bb < 0)
    {
        bbb = 0;
    }

    spawn->ActorInfo->DagHeadPoint->StringSprite->Color.R = rrr;
    spawn->ActorInfo->DagHeadPoint->StringSprite->Color.G = ggg;
    spawn->ActorInfo->DagHeadPoint->StringSprite->Color.B = bbb;

    if (flash == 10)
    {
        flash = 0;
    }
    else
    {
        flash++;
    }

    return result;
}

int __fastcall EQMACMQ_DETOUR_EQ_Character__eqspa_movement_rate(void* this_ptr, void* not_used, short a1)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_EQ_Character__eqspa_movement_rate(this_ptr, a1);
    }

    int result = EQMACMQ_REAL_EQ_Character__eqspa_movement_rate(this_ptr, a1);

    if (g_speedHackIsEnabled == true)
    {
        PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

        if (playerSpawn != NULL)
        {
            if (playerSpawn->ActorInfo->MovementSpeedModifier < g_speedHackModifier)
            {
                playerSpawn->ActorInfo->MovementSpeedModifier = g_speedHackModifier;
            }
        }
    }

    return result;
}

int __fastcall EQMACMQ_DETOUR_EQ_Character__CastSpell(void* this_ptr, void* not_used, unsigned char a1, short a2, EQITEMINFO** a3, short a4)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_EQ_Character__CastSpell(this_ptr, a1, a2, a3, a4);
    }

/*
    int result = EQMACMQ_REAL_EQ_Character__CastSpell(this_ptr, a1, a2, a3, a4);

    FILE* file = fopen("eqmac_mq-debug-EQ_Character__CastSpell.txt", "a");
    fprintf(file, "a1: %d\n", a1);
    fprintf(file, "a2: %d\n", a2);
    fprintf(file, "a3: 0x%X\n", a3);
    fprintf(file, "a4: %d\n", a4);
    fprintf(file, "result: %d\n", result);
    fprintf(file, "\n\n");
    fclose(file);

    return result;
*/

    if (g_standWhenCastSpellIsEnabled == true)
    {
        PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

        if (playerSpawn != NULL)
        {
            if (playerSpawn->StandingState == EQ_STANDING_STATE_SITTING)
            {
                ((EQPlayer*)playerSpawn)->ChangePosition(EQ_STANDING_STATE_STANDING);
            }
        }
    }

    return EQMACMQ_REAL_EQ_Character__CastSpell(this_ptr, a1, a2, a3, a4);
}

int __fastcall EQMACMQ_DETOUR_EQ_Character__UseSkill(void* this_ptr, void* not_used, unsigned char a1, class EQPlayer* a2)
{
    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_EQ_Character__UseSkill(this_ptr, a1, a2);
    }

/*
    int result = EQMACMQ_REAL_EQ_Character__UseSkill(this_ptr, a1, a2);

    FILE* file = fopen("eqmac_mq-debug-EQ_Character__UseSkill.txt", "a");
    fprintf(file, "a1: %d\n", a1);
    if (a2 != NULL)
    {
        fprintf(file, "a2: 0x%X\n", a2);
    }
    fprintf(file, "\n\n");
    fclose(file);

    return result;
*/

    return EQMACMQ_REAL_EQ_Character__UseSkill(this_ptr, a1, a2);
}

int __fastcall EQMACMQ_DETOUR_CEverQuest__InterpretCmd(void* this_ptr, void* not_used, class EQPlayer* a1, char* a2)
{
    //FILE* file = fopen("eqmac_mq-debug-CEverQuest__InterpretCmd.txt", "a");
    //fprintf(file, "CEverQuest::InterpretCmd: %s\n", a2);
    //fclose(file);

    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, a1, a2);
    }

    if (a1 == NULL || a2 == NULL)
    {
        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, a1, a2);
    }

    if (strlen(a2) == 0)
    {
        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // double slashes not needed, convert "//" to "/" by removing first character
    if (strncmp(a2, "//", 2) == 0)
    {
        memmove(a2, a2 + 1, strlen(a2));
    }

    // box chat
    if (g_boxChatIsEnabled == true)
    {
        if (strlen(a2) > 0)
        {
            for (size_t i = 0; i < EQMACMQ_BOX_CHAT_COMMANDS_MAX; i++)
            {
                if (strlen(g_boxChatSendCommandList[i]) == 0)
                {
                    strncpy_s(g_boxChatSendCommandList[i], sizeof(g_boxChatSendCommandList[i]), a2, _TRUNCATE);
                    break;
                }
            }
        }

        if (strncmp(a2, "/bc", 3) == 0)
        {
            if (strcmp(a2, "/bccmd autoconnect") == 0 || strcmp(a2, "/bccmd autoreconnect") == 0)
            {
                EQMACMQ_ToggleBoxChatAutoConnect();
            }

            if (strcmp(a2, "/bccmd connect") == 0 || strcmp(a2, "/bccmd reconnect") == 0)
            {
                if (EQMACMQ_DoBoxChatConnect() == true)
                {
                    g_boxChatIsConnected = true;
                    g_boxChatSendConnectedMessage = true;
                    g_boxChatShowConnectedMessage = true;
                }
            }
            else if (strncmp(a2, "/bccmd connect ", 15) == 0)
            {
                char command[128];
                char command2[128];

                char ipAddress[128];
                char port[128];

                int result = sscanf_s(a2, "%s %s %s %s", command, sizeof(command), command2, sizeof(command2), ipAddress, sizeof(ipAddress), port, sizeof(port));

                if (result == 4)
                {
                    strncpy_s(g_boxChatServerIpAddress, sizeof(g_boxChatServerIpAddress), ipAddress, _TRUNCATE);
                    strncpy_s(g_boxChatServerPort,      sizeof(g_boxChatServerPort),      port,      _TRUNCATE);

                    EQ_WriteStringVarToChat("Box Chat Server IP Address", g_boxChatServerIpAddress);
                    EQ_WriteStringVarToChat("Box Chat Server Port",       g_boxChatServerPort);

                    if (EQMACMQ_DoBoxChatConnect() == true)
                    {
                        g_boxChatIsConnected = true;
                        g_boxChatSendConnectedMessage = true;
                        g_boxChatShowConnectedMessage = true;
                    }
                }
            }

            if (strcmp(a2, "/bccmd disconnect") == 0)
            {
                closesocket(g_connectSocket);

                g_boxChatIsConnected = false;
                g_boxChatShowDisconnectedMessage = true;
            }

            if (strcmp(a2, "/bccmd status") == 0)
            {
                char statusText[128];
                _snprintf_s(statusText, sizeof(statusText), _TRUNCATE, "Box Chat Status: %s", g_boxChatIsConnected ? "Connected" : "Disconnected");

                EQ_CLASS_CEverQuest->dsp_chat(statusText);
            }

            if (strcmp(a2, "/bccmd help") == 0)
            {
                EQ_CLASS_CEverQuest->dsp_chat("Box Chat Help");
                EQ_CLASS_CEverQuest->dsp_chat("Connect to the box chat server:");
                EQ_CLASS_CEverQuest->dsp_chat("- /bccmd connect");
                EQ_CLASS_CEverQuest->dsp_chat("Disconnect from the box chat server:");
                EQ_CLASS_CEverQuest->dsp_chat("- /bccmd disconnect");
                EQ_CLASS_CEverQuest->dsp_chat("Toggle automatically connecting to the box chat server:");
                EQ_CLASS_CEverQuest->dsp_chat("- /bccmd autoconnect");
                EQ_CLASS_CEverQuest->dsp_chat("Show connection status with the box chat server:");
                EQ_CLASS_CEverQuest->dsp_chat("- /bccmd status");
                EQ_CLASS_CEverQuest->dsp_chat("Send commands to a specific client by character name:");
                EQ_CLASS_CEverQuest->dsp_chat("- /bct ToonName /command1;/command2");
                EQ_CLASS_CEverQuest->dsp_chat("Send commands to all connected clients, excluding yourself:");
                EQ_CLASS_CEverQuest->dsp_chat("- /bca /command1;/command2");
                EQ_CLASS_CEverQuest->dsp_chat("Send commands to all connected clients, including yourself:");
                EQ_CLASS_CEverQuest->dsp_chat("- /bcaa /command1;/command2");
            }

            return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
        }
    }

    if (g_slashCommandsIsEnabled == false)
    {
        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, a1, a2);
    }

    // unload
    if (strcmp(a2, "/unload") == 0)
    {
        EQMACMQ_DoUnload();

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // config
    if (strncmp(a2, "/config", 7) == 0)
    {
        if (strcmp(a2, "/configreload") == 0)
        {
            EQMACMQ_ReloadConfig();
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // hud
    if (strncmp(a2, "/hud", 4) == 0)
    {
        if (strcmp(a2, "/hudbuttons") == 0)
        {
            EQMACMQ_ToggleHudButtons();
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // mouse wheel zoom
    if (strncmp(a2, "/mousewheelzoom", 15) == 0)
    {
        if (strcmp(a2, "/mousewheelzoom") == 0)
        {
            EQMACMQ_ToggleMouseWheelZoom();
        }
        else
        {
            char command[128];

            float zoomMultiplier = 0.0f;

            int result = sscanf_s(a2, "%s %f", command, sizeof(command), &zoomMultiplier);

            if (result == 2)
            {
                g_mouseWheelZoomMultiplier = zoomMultiplier;

                EQ_WriteFloatVarToChat("Mouse Wheel Zoom Multiplier", g_mouseWheelZoomMultiplier);
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // client switcher
    if (strncmp(a2, "/clientswitcher", 15) == 0)
    {
        if (strcmp(a2, "/clientswitcher on") == 0)
        {
            if (g_clientSwitcherIsEnabled == false)
            {
                EQMACMQ_ToggleClientSwitcher();
            }
        }
        else if (strcmp(a2, "/clientswitcher off") == 0)
        {
            if (g_clientSwitcherIsEnabled == true)
            {
                EQMACMQ_ToggleClientSwitcher();
            }
        }
        else if (strcmp(a2, "/clientswitcher 1") == 0)
        {
            EQMACMQ_DoClientSwitch(0);
        }
        else if (strcmp(a2, "/clientswitcher 2") == 0)
        {
            EQMACMQ_DoClientSwitch(1);
        }
        else if (strcmp(a2, "/clientswitcher 3") == 0)
        {
            EQMACMQ_DoClientSwitch(2);
        }
        else if (strcmp(a2, "/clientswitchernext") == 0)
        {
            EQMACMQ_DoClientSwitchNext();
        }
        else if (strcmp(a2, "/clientswitcherprevious") == 0 || strcmp(a2, "/clientswitcherprev") == 0)
        {
            EQMACMQ_DoClientSwitchPrevious();
        }
        else if (strcmp(a2, "/clientswitcherrefresh") == 0)
        {
            EQMACMQ_ClientSwitcherRefresh();
        }
        else if (strcmp(a2, "/clientswitchersyncmouse") == 0)
        {
            EQMACMQ_ToggleClientSwitcherSyncMouse();
        }
        else if (strcmp(a2, "/clientswitcherbuttons") == 0)
        {
            EQMACMQ_ToggleClientSwitcherButtons();
        }
        else if (strcmp(a2, "/clientswitcherhotkeys") == 0)
        {
            EQMACMQ_ToggleClientSwitcherHotkeys();
        }
        else
        {
            EQMACMQ_ToggleClientSwitcher();
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // map
    if (strncmp(a2, "/map", 4) == 0)
    {
        if (strcmp(a2, "/map") == 0)
        {
            EQMACMQ_ToggleMap();
        }
        else if (strcmp(a2, "/map on") == 0)
        {
            if (g_mapIsEnabled == false)
            {
                EQMACMQ_ToggleMap();
            }
        }
        else if (strcmp(a2, "/map off") == 0)
        {
            if (g_mapIsEnabled == true)
            {
                EQMACMQ_ToggleMap();
            }
        }
        else if (strcmp(a2, "/mapreload") == 0)
        {
            EQMACMQ_ReloadMap();
        }
        else if (strcmp(a2, "/mapbuttons") == 0)
        {
            EQMACMQ_ToggleMapButtons();
        }
        else if (strcmp(a2, "/mapmaximized") == 0 || strcmp(a2, "/mapmaximize") == 0)
        {
            EQMACMQ_ToggleMapMaximized();
        }
        else if (strncmp(a2, "/mapmaximized ", 14) == 0)
        {
            char command[128];

            float x = 0.0f;
            float y = 0.0f;

            float width  = 0.0f;
            float height = 0.0f;

            int result = sscanf_s(a2, "%s %f %f %f %f", command, sizeof(command), &x, &y, &width, &height);

            if (result > 2)
            {
                g_mapMaximizedX = x;
                g_mapMaximizedY = y;

                EQ_WriteFloatVarToChat("Map Maximized X", g_mapMaximizedX);
                EQ_WriteFloatVarToChat("Map Maximized Y", g_mapMaximizedY);

                if (result > 4)
                {
                    g_mapMaximizedWidth  = width;
                    g_mapMaximizedHeight = height;

                    EQ_WriteFloatVarToChat("Map Maximized Width",  g_mapMaximizedWidth);
                    EQ_WriteFloatVarToChat("Map Maximized Height", g_mapMaximizedHeight);
                }

                EQMACMQ_GuiRecalculateCoordinates();
            }
        }
        else if (strcmp(a2, "/maplayer 0") == 0 || strcmp(a2, "/maplayerbase") == 0)
        {
            EQMACMQ_ToggleMapLayer0();
        }
        else if (strcmp(a2, "/maplayer 1") == 0)
        {
            EQMACMQ_ToggleMapLayer1();
        }
        else if (strcmp(a2, "/maplayer 2") == 0)
        {
            EQMACMQ_ToggleMapLayer2();
        }
        else if (strcmp(a2, "/maplayer 3") == 0)
        {
            EQMACMQ_ToggleMapLayer3();
        }
        else if (strcmp(a2, "/mapspawnclicktotarget") == 0)
        {
            EQMACMQ_ToggleMapSpawnClickToTarget();
        }
        else if (strcmp(a2, "/maparrow") == 0)
        {
            EQMACMQ_ToggleMapArrow();
        }
        else if (strcmp(a2, "/mapzoneinfo") == 0)
        {
            EQMACMQ_ToggleMapZoneInfo();
        }
        else if (strcmp(a2, "/mapheightfilter") == 0)
        {
            EQMACMQ_ToggleMapHeightFilter();
        }
        else if (strncmp(a2, "/mapheightfilter ", 17) == 0)
        {
            char command[128];

            float low  = 0.0f;
            float high = 0.0f;

            int result = sscanf_s(a2, "%s %f %f", command, sizeof(command), &low, &high);

            if (result == 3)
            {
                g_mapHeightFilterLow  = low;
                g_mapHeightFilterHigh = high;

                EQ_WriteFloatVarToChat("Map Height Filter Low",  g_mapHeightFilterLow);
                EQ_WriteFloatVarToChat("Map Height Filter High", g_mapHeightFilterHigh);
            }
        }
        else if (strcmp(a2, "/mapheightfilterspawns") == 0)
        {
            EQMACMQ_ToggleMapHeightFilterSpawns();
        }
        else if (strcmp(a2, "/maprotate") == 0)
        {
            EQMACMQ_ToggleMapRotate();
        }
        else if (strcmp(a2, "/mapspawns") == 0)
        {
            EQMACMQ_ToggleMapSpawns();
        }
        else if (strcmp(a2, "/mappoints") == 0)
        {
            EQMACMQ_ToggleMapPoints();
        }
        else if (strcmp(a2, "/maplines") == 0)
        {
            EQMACMQ_ToggleMapLines();
        }
        else if (strcmp(a2, "/maptargetline") == 0)
        {
            EQMACMQ_ToggleMapTargetLine();
        }
        else if (strcmp(a2, "/mapyourselfcorpseline") == 0)
        {
            EQMACMQ_ToggleMapYourselfCorpseLine();
        }
        else if (strcmp(a2, "/mapreplaceblacklineswithwhitelines") == 0)
        {
            EQMACMQ_ToggleMapReplaceBlackLinesWithWhiteLines();
        }
        else if (strcmp(a2, "/mapshowplayer") == 0 || strcmp(a2, "/mapshowpc") == 0)
        {
            EQMACMQ_ToggleMapShowPlayer();
        }
        else if (strcmp(a2, "/mapshowplayercorpse") == 0 || strcmp(a2, "/mapshowpccorpse") == 0)
        {
            EQMACMQ_ToggleMapShowPlayerCorpse();
        }
        else if (strcmp(a2, "/mapshownpc") == 0)
        {
            EQMACMQ_ToggleMapShowNpc();
        }
        else if (strcmp(a2, "/mapshownpccorpse") == 0)
        {
            EQMACMQ_ToggleMapShowNpcCorpse();
        }
        else if (strcmp(a2, "/mapspawndistance") == 0)
        {
            EQMACMQ_ToggleMapSpawnDistance();
        }
        else if (strncmp(a2, "/mapspawndistance ", 18) == 0)
        {
            char command[128];

            float distance = 0.0f;

            int result = sscanf_s(a2, "%s %f", command, sizeof(command), &distance);

            if (result == 2)
            {
                g_mapSpawnDistance = distance;

                EQ_WriteFloatVarToChat("Map Spawn Distance", g_mapSpawnDistance);
            }
        }
        else if (strcmp(a2, "/mapspawnfilternpc") == 0)
        {
            EQMACMQ_ToggleMapSpawnFilterNpc();
        }
        else if (strncmp(a2, "/mapspawnfilternpc ", 19) == 0)
        {
            char command[128];

            char filter[4096];

            int result = sscanf_s(a2, "%s %s", command, sizeof(command), filter, sizeof(filter));

            if (result == 2)
            {
                strncpy_s(g_mapSpawnFilterNpc, sizeof(g_mapSpawnFilterNpc), filter, _TRUNCATE);

                EQ_WriteStringVarToChat("Map Spawn Filter NPC", g_mapSpawnFilterNpc);
            }
        }
        else if (strcmp(a2, "/mapzoomresetandcenter") == 0)
        {
            EQMACMQ_MapResetZoom();
            EQMACMQ_MapCenter();

            EQ_CLASS_CEverQuest->dsp_chat("Map Zoom Reset and Centered.");
        }
        else if (strcmp(a2, "/mapzoomreset") == 0)
        {
            EQMACMQ_MapResetZoom();

            EQ_CLASS_CEverQuest->dsp_chat("Map Zoom Reset.");
        }
        else if (strcmp(a2, "/mapcenter") == 0)
        {
            EQMACMQ_MapCenter();

            EQ_CLASS_CEverQuest->dsp_chat("Map Centered.");
        }
        else if (strncmp(a2, "/mapzoom ", 9) == 0)
        {
            char command[128];

            float zoom = 0.0f;

            int result = sscanf_s(a2, "%s %f", command, sizeof(command), &zoom);

            if (result == 2)
            {
                g_mapZoom = zoom;

                EQ_WriteFloatVarToChat("Map Zoom", g_mapZoom);
            }
        }
        else if (strcmp(a2, "/mapmousewheelzoom") == 0)
        {
            EQMACMQ_ToggleMapMouseWheelZoom();
        }
        else if (strncmp(a2, "/mapmousewheelzoom ", 19) == 0 || strncmp(a2, "/mapmousewheelzoommultiplier ", 29) == 0)
        {
            char command[128];

            float zoomMultiplier = 0.0f;

            int result = sscanf_s(a2, "%s %f", command, sizeof(command), &zoomMultiplier);

            if (result == 2)
            {
                g_mapMouseWheelZoomMultiplier = zoomMultiplier;

                EQ_WriteFloatVarToChat("Map Mouse Wheel Zoom Multiplier", g_mapMouseWheelZoomMultiplier);
            }
        }
        else if (strncmp(a2, "/maparrowradius ", 16) == 0 || strncmp(a2, "/maparrow ", 10) == 0)
        {
            char command[128];

            float radius = 0.0f;

            int result = sscanf_s(a2, "%s %f", command, sizeof(command), &radius);

            if (result == 2)
            {
                g_mapArrowRadius = radius;

                EQ_WriteFloatVarToChat("Map Arrow Radius", g_mapArrowRadius);
            }
        }
        else if (strncmp(a2, "/mapcenterlinesize ", 19) == 0 || strncmp(a2, "/mapcenterline ", 15) == 0)
        {
            char command[128];

            float size = 0.0f;

            int result = sscanf_s(a2, "%s %f", command, sizeof(command), &size);

            if (result == 2)
            {
                g_mapCenterLineSize = size;

                EQ_WriteFloatVarToChat("Map Center Line Size", g_mapCenterLineSize);
            }
        }
        else
        {
            if (strncmp(a2, "/map ", 5) == 0)
            {
                char command[128];

                float x = 0.0f;
                float y = 0.0f;

                float width  = 0.0f;
                float height = 0.0f;

                int result = sscanf_s(a2, "%s %f %f %f %f", command, sizeof(command), &x, &y, &width, &height);

                if (result > 2)
                {
                    g_mapDefaultX = x;
                    g_mapDefaultY = y;

                    EQ_WriteFloatVarToChat("Map X", g_mapDefaultX);
                    EQ_WriteFloatVarToChat("Map Y", g_mapDefaultY);

                    if (result > 4)
                    {
                        g_mapDefaultWidth  = width;
                        g_mapDefaultHeight = height;

                    EQ_WriteFloatVarToChat("Map Width",  g_mapDefaultWidth);
                    EQ_WriteFloatVarToChat("Map Height", g_mapDefaultHeight);
                    }

                    EQMACMQ_GuiRecalculateCoordinates();
                }
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // esp
    if (strncmp(a2, "/esp", 4) == 0)
    {
        if (strcmp(a2, "/esp on") == 0)
        {
            if (g_espIsEnabled == false)
            {
                EQMACMQ_ToggleEsp();
            }
        }
        else if (strcmp(a2, "/esp off") == 0)
        {
            if (g_espIsEnabled == true)
            {
                EQMACMQ_ToggleEsp();
            }
        }
        else if (strcmp(a2, "/espcliptoviewport") == 0)
        {
            EQMACMQ_ToggleEspClipToViewPort();
        }
        else if (strcmp(a2, "/espshowyourself") == 0)
        {
            EQMACMQ_ToggleEspShowYourself();
        }
        else if (strcmp(a2, "/espshowplayer") == 0 || strcmp(a2, "/espshowpc") == 0)
        {
            EQMACMQ_ToggleEspShowPlayer();
        }
        else if (strcmp(a2, "/espshowplayercorpse") == 0 || strcmp(a2, "/espshowpccorpse") == 0)
        {
            EQMACMQ_ToggleEspShowPlayerCorpse();
        }
        else if (strcmp(a2, "/espshownpc") == 0)
        {
            EQMACMQ_ToggleEspShowNpc();
        }
        else if (strcmp(a2, "/espshownpccorpse") == 0)
        {
            EQMACMQ_ToggleEspShowNpcCorpse();
        }
        else if (strcmp(a2, "/espshowgroundspawns") == 0)
        {
            EQMACMQ_ToggleEspShowGroundSpawns();
        }
        else if (strcmp(a2, "/espshowdoorspawns") == 0)
        {
            EQMACMQ_ToggleEspShowDoorSpawns();
        }
        else if (strcmp(a2, "/espshowuniquedoorspawns") == 0)
        {
            EQMACMQ_ToggleEspShowUniqueDoorSpawns();
        }
        else if (strcmp(a2, "/espshowtext") == 0)
        {
            EQMACMQ_ToggleEspShowText();
        }
        else if (strcmp(a2, "/espshowtextonmousehover") == 0)
        {
            EQMACMQ_ToggleEspShowTextOnMouseHover();
        }
        else if (strcmp(a2, "/espdistance") == 0)
        {
            EQMACMQ_ToggleEspDistance();
        }
        else if (strncmp(a2, "/espdistance ", 13) == 0)
        {
            char command[128];

            float distance = 0.0f;

            int result = sscanf_s(a2, "%s %f", command, sizeof(command), &distance);

            if (result == 2)
            {
                g_espDistance = distance;

                EQ_WriteFloatVarToChat("ESP Distance", g_espDistance);
            }
        }
        else if (strcmp(a2, "/espfilternpc") == 0)
        {
            EQMACMQ_ToggleEspFilterNpc();
        }
        else if (strncmp(a2, "/espfilternpc ", 14) == 0)
        {
            char command[128];

            char filter[4096];

            int result = sscanf_s(a2, "%s %s", command, sizeof(command), filter, sizeof(filter));

            if (result == 2)
            {
                strncpy_s(g_espFilterNpc, sizeof(g_espFilterNpc), filter, _TRUNCATE);

                EQ_WriteStringVarToChat("ESP Filter NPC", g_espFilterNpc);
            }
        }
        else if (strcmp(a2, "/espskeletons") == 0)
        {
            EQMACMQ_ToggleEspSkeletons();
        }
        else if (strcmp(a2, "/espskeletonslineofsight") == 0 || strcmp(a2, "/espskeletonslos") == 0)
        {
            EQMACMQ_ToggleEspSkeletonsLineOfSight();
        }
        else if (strcmp(a2, "/espskeletonsdistance") == 0)
        {
            EQMACMQ_ToggleEspSkeletonsDistance();
        }
        else if (strncmp(a2, "/espskeletonsdistance ", 22) == 0)
        {
            char command[128];

            float distance = 0.0f;

            int result = sscanf_s(a2, "%s %f", command, sizeof(command), &distance);

            if (result == 2)
            {
                g_espSkeletonsDistance = distance;

                EQ_WriteFloatVarToChat("ESP Skeletons Distance", g_espSkeletonsDistance);
            }
        }
        else
        {
            EQMACMQ_ToggleEsp();
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // health bars
    if (strncmp(a2, "/healthbars", 11) == 0)
    {
        if (strcmp(a2, "/healthbars") == 0)
        {
            EQMACMQ_ToggleHealthBars();
        }
        else if (strcmp(a2, "/healthbars on") == 0)
        {
            if (g_healthBarsIsEnabled == false)
            {
                EQMACMQ_ToggleHealthBars();
            }
        }
        else if (strcmp(a2, "/healthbars off") == 0)
        {
            if (g_healthBarsIsEnabled == true)
            {
                EQMACMQ_ToggleHealthBars();
            }
        }
        else if (strcmp(a2, "/healthbarscliptoviewport") == 0)
        {
            EQMACMQ_ToggleHealthBarsClipToViewPort();
        }
        else if (strcmp(a2, "/healthbarsdistance") == 0)
        {
            EQMACMQ_ToggleHealthBarsDistance();
        }
        else if (strncmp(a2, "/healthbarsdistance ", 20) == 0)
        {
            char command[128];

            float distance = 0.0f;

            int result = sscanf_s(a2, "%s %f", command, sizeof(command), &distance);

            if (result == 2)
            {
                g_healthBarsDistance = distance;

                EQ_WriteFloatVarToChat("Health Bars Distance", g_healthBarsDistance);
            }
        }
        else if (strcmp(a2, "/healthbarsclicktotarget") == 0)
        {
            EQMACMQ_ToggleHealthBarsClickToTarget();
        }
        else
        {
            if (strncmp(a2, "/healthbars ", 12) == 0)
            {
                char command[128];

                float width  = 0.0f;
                float height = 0.0f;

                int result = sscanf_s(a2, "%s %f %f", command, sizeof(command), &width, &height);

                if (result == 3)
                {
                    g_healthBarsWidth  = width;
                    g_healthBarsHeight = height;

                    EQ_WriteFloatVarToChat("Health Bars Width",  g_healthBarsWidth);
                    EQ_WriteFloatVarToChat("Health Bars Height", g_healthBarsHeight);
                }
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // buff info
    if (strncmp(a2, "/buffinfo", 9) == 0)
    {
        if (strcmp(a2, "/buffinfo") == 0)
        {
            EQMACMQ_ToggleBuffInfo();
        }
        else if (strcmp(a2, "/buffinfo on") == 0)
        {
            if (g_buffInfoIsEnabled == false)
            {
                EQMACMQ_ToggleBuffInfo();
            }
        }
        else if (strcmp(a2, "/buffinfo off") == 0)
        {
            if (g_buffInfoIsEnabled == true)
            {
                EQMACMQ_ToggleBuffInfo();
            }
        }
        else
        {
            if (strncmp(a2, "/buffinfo ", 10) == 0)
            {
                char command[128];

                float x = 0.0f;
                float y = 0.0f;

                int result = sscanf_s(a2, "%s %f %f", command, sizeof(command), &x, &y);

                if (result == 3)
                {
                    g_buffInfoX = x;
                    g_buffInfoY = y;

                    EQ_WriteFloatVarToChat("Buff Info X", g_buffInfoX);
                    EQ_WriteFloatVarToChat("Buff Info Y", g_buffInfoY);
                }
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // player info
    if (strncmp(a2, "/playerinfo", 11) == 0)
    {
        if (strcmp(a2, "/playerinfo") == 0)
        {
            EQMACMQ_TogglePlayerInfo();
        }
        else if (strcmp(a2, "/playerinfo on") == 0)
        {
            if (g_playerInfoIsEnabled == false)
            {
                EQMACMQ_TogglePlayerInfo();
            }
        }
        else if (strcmp(a2, "/playerinfo off") == 0)
        {
            if (g_playerInfoIsEnabled == true)
            {
                EQMACMQ_TogglePlayerInfo();
            }
        }
        else
        {
            if (strncmp(a2, "/playerinfo ", 12) == 0)
            {
                char command[128];

                float x = 0.0f;
                float y = 0.0f;

                int result = sscanf_s(a2, "%s %f %f", command, sizeof(command), &x, &y);

                if (result == 3)
                {
                    g_playerInfoX = x;
                    g_playerInfoY = y;

                    EQ_WriteFloatVarToChat("Player Info X", g_playerInfoX);
                    EQ_WriteFloatVarToChat("Player Info Y", g_playerInfoY);
                }
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // target info
    if (strncmp(a2, "/targetinfo", 11) == 0)
    {
        if (strcmp(a2, "/targetinfo") == 0)
        {
            EQMACMQ_ToggleTargetInfo();
        }
        else if (strcmp(a2, "/targetinfo on") == 0)
        {
            if (g_targetInfoIsEnabled == false)
            {
                EQMACMQ_ToggleTargetInfo();
            }
        }
        else if (strcmp(a2, "/targetinfo off") == 0)
        {
            if (g_targetInfoIsEnabled == true)
            {
                EQMACMQ_ToggleTargetInfo();
            }
        }
        else
        {
            if (strncmp(a2, "/targetinfo ", 12) == 0)
            {
                char command[128];

                float x = 0.0f;
                float y = 0.0f;

                int result = sscanf_s(a2, "%s %f %f", command, sizeof(command), &x, &y);

                if (result == 3)
                {
                    g_targetInfoX = x;
                    g_targetInfoY = y;

                    EQ_WriteFloatVarToChat("Target Info X", g_targetInfoX);
                    EQ_WriteFloatVarToChat("Target Info Y", g_targetInfoY);
                }
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // message text
    if (strncmp(a2, "/messagetext", 12) == 0)
    {
        if (strcmp(a2, "/messagetext") == 0)
        {
            EQMACMQ_ToggleMessageText();
        }
        else
        {
            char command[128];

            float x = 0.0f;
            float y = 0.0f;

            int result = sscanf_s(a2, "%s %f %f", command, sizeof(command), &x, &y);

            if (result == 3)
            {
                g_messageTextX = x;
                g_messageTextY = y;

                EQ_WriteFloatVarToChat("Message Text X", g_messageTextX);
                EQ_WriteFloatVarToChat("Message Text Y", g_messageTextY);
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // buff window timers
    if (strcmp(a2, "/buffwindowtimers") == 0)
    {
        EQMACMQ_ToggleBuffWindowTimers();

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // limit cpu usage
    if (strncmp(a2, "/limitcpuusage", 14) == 0)
    {
        if (strcmp(a2, "/limitcpuusage") == 0)
        {
            EQMACMQ_ToggleLimitCpuUsage();
        }
        else if (strncmp(a2, "/limitcpuusage ", 15) == 0)
        {
            char command[128];

            int foregroundDelay = 1;
            int backgroundDelay = 10;

            int result = sscanf_s(a2, "%s %d %d", command, sizeof(command), &foregroundDelay, &backgroundDelay);

            if (result == 3)
            {
                g_limitCpuUsageForegroundDelay = foregroundDelay;
                g_limitCpuUsageBackgroundDelay = backgroundDelay;

                EQ_WriteIntVarToChat("Foreground Delay", g_limitCpuUsageForegroundDelay);
                EQ_WriteIntVarToChat("Background Delay", g_limitCpuUsageBackgroundDelay);
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // speed hack
    if (strncmp(a2, "/speed", 6) == 0)
    {
        if (strcmp(a2, "/speed") == 0)
        {
            EQMACMQ_ToggleSpeedHack();
        }
        else if (strcmp(a2, "/speed sow") == 0)
        {
            g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_SPIRIT_OF_WOLF;

            EQ_WriteStringVarToChat("Speed Modifier", "Spirit of Wolf");
        }
        else if (strcmp(a2, "/speed run1") == 0)
        {
            g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN1;

            EQ_WriteStringVarToChat("Speed Modifier", "AA Run 1");
        }
        else if (strcmp(a2, "/speed run2") == 0)
        {
            g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN2;

            EQ_WriteStringVarToChat("Speed Modifier", "AA Run 2");
        }
        else if (strcmp(a2, "/speed run3") == 0)
        {
            g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN3;

            EQ_WriteStringVarToChat("Speed Modifier", "AA Run 3");
        }
        else if (strncmp(a2, "/speed ", 7) == 0)
        {
            char command[128];

            float modifier = 0.0f;

            int result = sscanf_s(a2, "%s %f", command, sizeof(command), &modifier);

            if (result == 2)
            {
                g_speedHackModifier = modifier;

                EQ_WriteFloatVarToChat("Speed Modifier", g_speedHackModifier);
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // skill hack
    if (strncmp(a2, "/skillh", 7) == 0)
    {
        EQMACMQ_ToggleSkillHack();

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // bard twist
    if (strncmp(a2, "/twist", 6) == 0)
    {
        if (strcmp(a2, "/twist") == 0)
        {
            EQMACMQ_ToggleBardTwist();
        }
        else if (strcmp(a2, "/twist on") == 0)
        {
            if (g_bardTwistIsEnabled == false)
            {
                EQMACMQ_ToggleBardTwist();
            }
        }
        else if (strcmp(a2, "/twist off") == 0)
        {
            if (g_bardTwistIsEnabled == true)
            {
                EQMACMQ_ToggleBardTwist();
            }
        }
        else if (strncmp(a2, "/twist ", 7) == 0)
        {
            memset(g_bardTwistSongNumbers, -1, sizeof(g_bardTwistSongNumbers));

            char command[128];

            int songNumbers[g_bardTwistSongNumbersMax] = {-1};

            int result = sscanf_s
            (
                a2, "%s %d %d %d %d %d %d %d %d %d %d", command, sizeof(command),
                &songNumbers[0],
                &songNumbers[1],
                &songNumbers[2],
                &songNumbers[3],
                &songNumbers[4],
                &songNumbers[5],
                &songNumbers[6],
                &songNumbers[7],
                &songNumbers[8],
                &songNumbers[9]
            );

            if (result > 1)
            {
                char twistText[128];
                strncpy_s(twistText, sizeof(twistText), "Twist Songs: ", _TRUNCATE);

                for (int i = 0; i < result - 1; i++)
                {
                    if (songNumbers[i] == 0 || songNumbers[i] > EQ_NUM_SPELL_GEMS)
                    {
                        continue;
                    }

                    g_bardTwistSongNumbers[i] = songNumbers[i] - 1;

                    char songNumberString[8];
                    _itoa_s(songNumbers[i], songNumberString, sizeof(songNumberString), 10);

                    strncat_s(twistText, sizeof(twistText), songNumberString, _TRUNCATE);
                    strncat_s(twistText, sizeof(twistText), " ",              _TRUNCATE);
                }

                EQ_CLASS_CEverQuest->dsp_chat(twistText);
            }
        }
        else if (strncmp(a2, "/twistdelay ", 12) == 0)
        {
            char command[128];

            unsigned int delay = 3300;

            int result = sscanf_s(a2, "%s %d", command, sizeof(command), &delay);

            if (result == 2)
            {
                g_bardTwistDelay = delay;

                EQ_WriteIntVarToChat("Twist Delay", g_bardTwistDelay);
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // name text
    if (strcmp(a2, "/nametext") == 0)
    {
        EQMACMQ_ToggleNameText();

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // name colors
    if (strcmp(a2, "/namecolors") == 0)
    {
        EQMACMQ_ToggleNameColors();

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // free camera
    if (strncmp(a2, "/freecam", 8) == 0)
    {
        if (strcmp(a2, "/freecamera") == 0)
        {
            EQMACMQ_ToggleFreeCamera();
        }
        else if (strncmp(a2, "/freecamera ", 12) == 0)
        {
            char command[128];

            float multiplier = 1.0;

            int result = sscanf_s(a2, "%s %f", command, sizeof(command), &multiplier);

            if (result == 2)
            {
                g_freeCameraMultiplier = multiplier;

                EQ_WriteFloatVarToChat("Free Camera Multiplier", g_freeCameraMultiplier);
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // melee
    if (strncmp(a2, "/melee", 6) == 0)
    {
        if (strcmp(a2, "/melee") == 0)
        {
            EQMACMQ_ToggleMelee();
        }
        else if (strcmp(a2, "/meleelist") == 0)
        {
            EQ_CLASS_CEverQuest->dsp_chat("Melee:");
            EQ_WriteBoolVarToChat("Melee Skill Taunt", g_meleeSkillTauntIsEnabled);
            EQ_WriteBoolVarToChat("Melee Skill Kick", g_meleeSkillKickIsEnabled);
            EQ_WriteBoolVarToChat("Melee Skill Bash", g_meleeSkillBashIsEnabled);
            EQ_WriteBoolVarToChat("Melee Skill Slam", g_meleeSkillSlamIsEnabled);
            EQ_WriteBoolVarToChat("Melee Skill Disarm", g_meleeSkillDisarmIsEnabled);
            EQ_WriteBoolVarToChat("Melee Skill Sense Heading", g_meleeSkillSenseHeadingIsEnabled);
        }
        else if (strcmp(a2, "/meleerange") == 0)
        {
            EQMACMQ_ToggleMeleeRange();
        }
        else if (strncmp(a2, "/meleedelay ", 12) == 0)
        {
            char command[128];

            int delay = 1000;

            int result = sscanf_s(a2, "%s %d", command, sizeof(command), &delay);

            if (result == 2)
            {
                g_meleeDelay = delay;

                EQ_WriteIntVarToChat("Melee Delay", g_meleeDelay);
            }
        }
        else if (strncmp(a2, "/melee ", 7) == 0)
        {
            char command[128];

            char skill[128];

            int result = sscanf_s(a2, "%s %s", command, sizeof(command), skill, sizeof(skill));

            if (result == 2)
            {
                if (strcmp(skill, "taunt") == 0)
                {
                    EQ_ToggleBool(g_meleeSkillTauntIsEnabled);
                    EQ_WriteBoolVarToChat("Melee Skill Taunt", g_meleeSkillTauntIsEnabled);
                }
                else if (strcmp(skill, "kick") == 0)
                {
                    EQ_ToggleBool(g_meleeSkillKickIsEnabled);
                    EQ_WriteBoolVarToChat("Melee Skill Kick", g_meleeSkillKickIsEnabled);
                }
                else if (strcmp(skill, "bash") == 0)
                {
                    EQ_ToggleBool(g_meleeSkillBashIsEnabled);
                    EQ_WriteBoolVarToChat("Melee Skill Bash", g_meleeSkillBashIsEnabled);
                }
                else if (strcmp(skill, "slam") == 0)
                {
                    EQ_ToggleBool(g_meleeSkillSlamIsEnabled);
                    EQ_WriteBoolVarToChat("Melee Skill Slam", g_meleeSkillSlamIsEnabled);
                }
                else if (strcmp(skill, "disarm") == 0)
                {
                    EQ_ToggleBool(g_meleeSkillDisarmIsEnabled);
                    EQ_WriteBoolVarToChat("Melee Skill Disarm", g_meleeSkillDisarmIsEnabled);
                }
                else if (strcmp(skill, "senseheading") == 0)
                {
                    EQ_ToggleBool(g_meleeSkillSenseHeadingIsEnabled);
                    EQ_WriteBoolVarToChat("Melee Skill Sense Heading", g_meleeSkillSenseHeadingIsEnabled);
                }
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // hacks
    if (strncmp(a2, "/hack", 5) == 0)
    {
        if (strcmp(a2, "/hacks") == 0)
        {
            EQMACMQ_ToggleHacks();
        }
        else if (strncmp(a2, "/hacks ", 7) == 0)
        {
            char command[128];

            int index = 0;

            int result = sscanf_s(a2, "%s %d", command, sizeof(command), &index);

            if (result == 2)
            {
                struct _EQMACMQHACK* hack = g_hackListBegin;

                while (hack)
                {
                    if (hack->Index == index)
                    {
                        EQMACMQ_DoHackEnable(hack->Filename, !(hack->IsEnabled));

                        char hackText[128];
                        _snprintf_s
                        (
                            hackText, sizeof(hackText), _TRUNCATE,
                            "%d: %s | %s (%s)",
                            hack->Index,
                            hack->IsEnabled ? "On" : "Off",
                            hack->Name,
                            hack->Description
                        );

                        EQ_CLASS_CEverQuest->dsp_chat(hackText);

                        break;
                    }

                    hack = hack->Next;
                }
            }
        }
        else if (strcmp(a2, "/hackslist") == 0)
        {
            EQ_CLASS_CEverQuest->dsp_chat("Hacks:");

            struct _EQMACMQHACK* hack = g_hackListBegin;

            while (hack)
            {
                char hackText[128];
                _snprintf_s
                (
                    hackText, sizeof(hackText), _TRUNCATE,
                    "%d: %s | %s (%s)",
                    hack->Index,
                    hack->IsEnabled ? "On" : "Off",
                    hack->Name,
                    hack->Description
                );

                EQ_CLASS_CEverQuest->dsp_chat(hackText);

                hack = hack->Next;
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // save spell set
    if (strncmp(a2, "/savespellset ", 14) == 0 || strncmp(a2, "/savespells ", 12) == 0)
    {
        char command[128];

        char name[128];

        int result = sscanf_s(a2, "%s %s", command, sizeof(command), name, sizeof(name));

        if (result == 2 && strlen(name) > 0)
        {
            EQMACMQ_DoSaveSpellset(name);

            EQ_WriteStringVarToChat("Save Spellset", name);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // load spell set
    if (strncmp(a2, "/loadspellset ", 14) == 0 || strncmp(a2, "/loadspells ", 12) == 0 || strncmp(a2, "/memspellset ", 13) == 0 || strncmp(a2, "/memspells ", 11) == 0)
    {
        char command[128];

        char name[128];

        int result = sscanf_s(a2, "%s %s", command, sizeof(command), name, sizeof(name));

        if (result == 2 && strlen(name) > 0)
        {
            char filename[1024];
            _snprintf_s(filename, sizeof(filename), _TRUNCATE, ".\\spellsets\\%s.ini", name);

            if (EQMACMQ_FileExists(filename) == true)
            {
                strncpy_s(g_loadSpellsetName, sizeof(g_loadSpellsetName), name, _TRUNCATE);
                g_loadSpellsetIsInProgress = true;

                EQMACMQ_DoLoadSpellset(name);

                EQ_WriteStringVarToChat("Load Spellset", name);
            }
            else
            {
                EQ_WriteStringVarToChat("Load Spellset", "File not found!");
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // logging
    if (strcmp(a2, "/logging") == 0)
    {
        EQMACMQ_ToggleLogging();

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // targeting
    if (strcmp(a2, "/target myself") == 0)
    {
        PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

        if (playerSpawn)
        {
            EQ_TargetSpawn(playerSpawn);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    if (strcmp(a2, "/target player") == 0 || strcmp(a2, "/target pc") == 0)
    {
        PEQSPAWNINFO nearestSpawn = EQ_GetNearestSpawn(EQ_SPAWN_TYPE_PLAYER);

        if (nearestSpawn)
        {
            EQ_TargetSpawn(nearestSpawn);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    if (strcmp(a2, "/target npc") == 0)
    {
        PEQSPAWNINFO nearestSpawn = EQ_GetNearestSpawn(EQ_SPAWN_TYPE_NPC);

        if (nearestSpawn)
        {
            EQ_TargetSpawn(nearestSpawn);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    if (strcmp(a2, "/target corpse") == 0)
    {
        PEQSPAWNINFO nearestSpawn = EQ_GetNearestSpawn(EQ_SPAWN_TYPE_ANY_CORPSE);

        if (nearestSpawn)
        {
            EQ_TargetSpawn(nearestSpawn);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    if (strcmp(a2, "/target npccorpse") == 0)
    {
        PEQSPAWNINFO nearestSpawn = EQ_GetNearestSpawn(EQ_SPAWN_TYPE_NPC_CORPSE);

        if (nearestSpawn)
        {
            EQ_TargetSpawn(nearestSpawn);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    if (strcmp(a2, "/target playercorpse") == 0 || strcmp(a2, "/target pccorpse") == 0)
    {
        PEQSPAWNINFO nearestSpawn = EQ_GetNearestSpawn(EQ_SPAWN_TYPE_PLAYER_CORPSE);

        if (nearestSpawn)
        {
            EQ_TargetSpawn(nearestSpawn);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    if (strcmp(a2, "/target mycorpse") == 0)
    {
        PEQSPAWNINFO myCorpse = EQ_GetMyCorpse();

        if (myCorpse)
        {
            EQ_TargetSpawn(myCorpse);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // target height / size
    if (strncmp(a2, "/targetheight", 13) == 0)
    {
        if (strcmp(a2, "/targetheight") == 0)
        {
            PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

            if (targetSpawn)
            {
                ((EQPlayer*)targetSpawn)->ChangeHeight(5.0f);

                EQ_WriteFloatVarToChat("Target Height", 5.0f);
            }
        }
        else if (strncmp(a2, "/targetheight ", 14) == 0)
        {
            PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

            if (targetSpawn)
            {
                char command[128];

                float height = 0.0f;

                int result = sscanf_s(a2, "%s %f", command, sizeof(command), &height);

                if (result == 2)
                {
                    if (height > 0.0f)
                    {
                        ((EQPlayer*)targetSpawn)->ChangeHeight(height);

                        EQ_WriteFloatVarToChat("Target Height", height);
                    }
                }
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // zone gravity
    if (strcmp(a2, "/zonegravity") == 0)
    {
        EQ_OBJECT_ZoneInfo.Gravity = (float)EQ_ZONE_GRAVITY_DEFAULT;

        EQ_WriteFloatVarToChat("Zone Gravity set to Default", (float)EQ_ZONE_GRAVITY_DEFAULT);

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }
    else if (strncmp(a2, "/zonegravity ", 13) == 0)
    {
        char command[128];

        float gravity = 0.0f;

        int result = sscanf_s(a2, "%s %f", command, sizeof(command), &gravity);

        if (result == 2)
        {
            if (gravity > 0.0f)
            {
                EQ_OBJECT_ZoneInfo.Gravity = gravity;

                EQ_WriteFloatVarToChat("Zone Gravity", gravity);
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // zone clip
    if (strncmp(a2, "/zoneclip ", 10) == 0)
    {
        char command[128];

        float clip = 0.0f;

        int result = sscanf_s(a2, "%s %f", command, sizeof(command), &clip);

        if (result == 2)
        {
            if (clip > 0.0f)
            {
                EQ_OBJECT_ZoneInfo.MinClip = clip;
                EQ_OBJECT_ZoneInfo.MaxClip = clip;

                EQ_WriteFloatVarToChat("Zone Minimum And Maximum Clip", clip);
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // zone min clip
    if (strncmp(a2, "/zoneminclip ", 13) == 0)
    {
        char command[128];

        float clip = 0.0f;

        int result = sscanf_s(a2, "%s %f", command, sizeof(command), &clip);

        if (result == 2)
        {
            if (clip > 0.0f)
            {
                EQ_OBJECT_ZoneInfo.MinClip = clip;

                EQ_WriteFloatVarToChat("Zone Minimum Clip", clip);
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // zone max clip
    if (strncmp(a2, "/zonemaxclip ", 13) == 0)
    {
        char command[128];

        float clip = 0.0f;

        int result = sscanf_s(a2, "%s %f", command, sizeof(command), &clip);

        if (result == 2)
        {
            if (clip > 0.0f)
            {
                EQ_OBJECT_ZoneInfo.MaxClip = clip;

                EQ_WriteFloatVarToChat("Zone Maximum Clip", clip);
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // face target
    if (strncmp(a2, "/facetar", 8) == 0)
    {
        PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;
        PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

        if (playerSpawn && targetSpawn)
        {
            ((EQPlayer*)playerSpawn)->FacePlayer((EQPlayer*)targetSpawn);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // bank info
    if (strcmp(a2, "/bank") == 0)
    {
        PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

        if (charInfo)
        {
            char bankText[128];
            _snprintf_s
            (
                bankText, sizeof(bankText), _TRUNCATE,
                "You have %dp %dg %ds %dc in the bank.",
                charInfo->BankPlatinum,
                charInfo->BankGold,
                charInfo->BankSilver,
                charInfo->BankCopper
            );

            EQ_CLASS_CEverQuest->dsp_chat(bankText);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // bank list
    if (strcmp(a2, "/banklist") == 0)
    {
        EQMACMQ_DoBankList();

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // mana info
    if (strcmp(a2, "/mana") == 0)
    {
        PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

        if (charInfo)
        {
            int manaCurrent = charInfo->Mana;
            int manaMax     = EQ_CLASS_EQ_Character->Max_Mana();

            char manaText[128];
            _snprintf_s
            (
                manaText, sizeof(manaText), _TRUNCATE,
                "You have %d / %d mana.",
                manaCurrent,
                manaMax
            );

            EQ_CLASS_CEverQuest->dsp_chat(manaText);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // buff info
    if (strcmp(a2, "/buffs") == 0)
    {
        EQ_CLASS_CEverQuest->dsp_chat("Buffs:");

        PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

        if (charInfo)
        {
            for (size_t i = 0; i < EQ_NUM_BUFFS; i++)
            {
                int spellId = charInfo->Buff[i].SpellId;

                if (spellId == EQ_SPELL_ID_NULL)
                {
                    continue;
                }

                const char* spellName = EQ_OBJECT_SpellList->Spell[spellId]->Name;

                if (strlen(spellName) == 0 || spellName == NULL)
                {
                    continue;
                }

                int buffTicks = charInfo->Buff[i].Ticks;

                if (buffTicks == 0)
                {
                    continue;
                }

                int buffHours   = 0;
                int buffMinutes = 0;
                int buffSeconds = 0;

                EQ_CalculateTickTime(buffTicks, buffHours, buffMinutes, buffSeconds);

                char buffText[128];
                _snprintf_s(buffText, sizeof(buffText), _TRUNCATE, "%02d: (%02d:%02d:%02d) %s", i + 1, buffHours, buffMinutes, buffSeconds, spellName);

                EQ_CLASS_CEverQuest->dsp_chat(buffText);
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // autoinventory
    if (strncmp(a2, "/autoinv", 8) == 0)
    {
        PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

        if (charInfo && charInfo->CursorItem != NULL)
        {
            EQ_AutoInventory(charInfo, &charInfo->CursorItem, 0);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // do hotbutton
    if (strncmp(a2, "/dohotbutton ", 13) == 0)
    {
        char command[128];

        unsigned int button = 0;

        int result = sscanf_s(a2, "%s %d", command, sizeof(command), &button);

        if (result == 2)
        {
            if (button > 0 && button <= EQ_NUM_HOTBUTTONS_TOTAL)
            {
                EQ_CLASS_CHotButtonWnd->DoHotButton(button - 1, false);
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // stop cast
    if (strcmp(a2, "/stopcast") == 0)
    {
        PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;

        if (charInfo != NULL)
        {
            EQ_CLASS_EQ_Character->StopSpellCast(0);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // track
    if (strcmp(a2, "/track") == 0)
    {
        EQ_CLASS_CTrackingWnd->Activate();

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // stop tracking
    if (strncmp(a2, "/stoptrack", 10) == 0)
    {
        WORD isHidingOrTracking = EQ_ReadMemory<WORD>(EQ_IS_HIDING_OR_TRACKING);

        if (isHidingOrTracking == EQ_IS_HIDING_OR_TRACKING_EQUALS_TRACKING)
        {
            EQ_WriteMemory<WORD>(EQ_IS_HIDING_OR_TRACKING, EQ_IS_HIDING_OR_TRACKING_EQUALS_NONE);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // toggle inspect
    if (strcmp(a2, "/toggleinspect") == 0)
    {
        EQMACMQ_ToggleInspect();

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // hide corpses looted
    if (_stricmp(a2, "/hidecorpses looted") == 0 || _stricmp(a2, "/hidecorpse looted") == 0)
    {
        EQMACMQ_ToggleHideCorpseLooted();

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // loot all
    if (strncmp(a2, "/lootall", 8) == 0)
    {
        if (strcmp(a2, "/lootall") == 0)
        {
            PEQCHARINFO charInfo = (PEQCHARINFO)EQ_OBJECT_CharInfo;
            PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;
            PEQSPAWNINFO targetSpawn = (PEQSPAWNINFO)EQ_OBJECT_TargetSpawn;

            if (charInfo != NULL && playerSpawn != NULL && targetSpawn != NULL)
            {
                if (playerSpawn->StandingState != EQ_STANDING_STATE_LOOTING)
                {
                    if (targetSpawn->Type == EQ_SPAWN_TYPE_NPC_CORPSE || targetSpawn->Type == EQ_SPAWN_TYPE_PLAYER_CORPSE)
                    {
                        EQ_CLASS_CEverQuest->LootCorpse((EQPlayer*)targetSpawn, 0);
                    }
                }
            }

            g_lootAllCounter = 0;
            g_lootAllIsInProgress = true;
        }
        else if (strcmp(a2, "/lootallnodrop") == 0)
        {
            EQMACMQ_ToggleLootAllNoDrop();
        }
        else if (strcmp(a2, "/lootallfilterid") == 0)
        {
            EQMACMQ_ToggleLootAllFilterId();
        }
        else if (strcmp(a2, "/lootallfiltername") == 0)
        {
            EQMACMQ_ToggleLootAllFilterName();
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // casting "Spell Name"
    if (strncmp(a2, "/casting", 8) == 0)
    {
        if (strncmp(a2, "/casting \"", 10) == 0)
        {
            if (strlen(a2) > 9 && a2[9] == '"' && a2[strlen(a2) - 1] == '"')
            {
                char command[128];

                char spellName[128];

                //char item[128];

                int result = sscanf_s(a2, "%s \"%[^\"]\"", command, sizeof(command), spellName, sizeof(spellName)); //item, sizeof(item)

                if (result == 2)
                {
                    int spellId = EQ_GetSpellIdBySpellName(spellName);

                    if (spellId != EQ_SPELL_ID_NULL)
                    {
                        int spellGem = EQ_GetSpellGemBySpellId(spellId);

                        if (spellGem != EQ_SPELL_ID_NULL)
                        {
                            EQ_CLASS_EQ_Character->StopSpellCast(0);
                            EQ_CLASS_EQ_Character->CastSpell(spellGem, spellId, NULL, -1);
                        }
                    }
                }

                // else if (result == 3) // item
            }
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // beep
    if (strcmp(a2, "/beep") == 0)
    {
        MessageBeep(MB_OK);

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    // dump commands
    if (strcmp(a2, "/dumpcommands") == 0)
    {
        FILE* file;
        errno_t fileErrorNumber = fopen_s(&file, "eqmac_mq-CommandList.txt", "a");

        if (file)
        {
            for (size_t i = 0; i < EQ_NUM_COMMANDS; i++)
            {
                if (EQ_OBJECT_CommandList.Command[i].Name != NULL && strlen(EQ_OBJECT_CommandList.Command[i].Name) > 0)
                {
                    fprintf
                    (
                        file,
                        "%d: %s (Function Address: 0x%08X, Restriction: %d, Category: %d)\n",
                        i,
                        EQ_OBJECT_CommandList.Command[i].Name,
                        EQ_OBJECT_CommandList.Command[i].FunctionAddress,
                        EQ_OBJECT_CommandList.Command[i].Restriction,
                        EQ_OBJECT_CommandList.Command[i].Category
                    );
                }
            }

            fclose(file);
        }

        return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, NULL, NULL);
    }

    return EQMACMQ_REAL_CEverQuest__InterpretCmd(this_ptr, a1, a2);
}

int __fastcall EQMACMQ_DETOUR_CEverQuest__dsp_chat(void* this_ptr, void* not_used, const char* a1, short a2, bool a3)
{
    // a1 = text
    // a2 = textColor

    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_CEverQuest__dsp_chat(this_ptr, a1, a2, a3);
    }

    if (g_messageTextOnChatWindowIsEnabled == true)
    {
        if
        (
            strcmp(a1, "Your spell fizzles!")                      == 0 ||
            strcmp(a1, "Your spell did not take hold.")            == 0 ||
            strcmp(a1, "Your target is out of range, get closer!") == 0 ||
            strcmp(a1, "You are too fatigued to jump!")            == 0 ||
            strcmp(a1, "YOU were injured by falling!")             == 0 ||
            strcmp(a1, "You can't reach that, get closer.")        == 0 ||
            strncmp(a1, "Your target resisted", 20)                == 0
        )
        {
            EQMACMQ_DoMessageTextGeneric(a1, EQ_TEXT_COLOR_RED);
        }
    }

    if (g_bardTwistIsEnabled == true)
    {
        if
        (
            strcmp(a1, "You miss a note, bringing your song to a close!") == 0 ||
            strcmp(a1, "You must first select a target for this spell!")  == 0
        )
        {
            g_bardTwistTimer = EQ_ReadMemory<DWORD>(EQ_TIMER) - g_bardTwistDelay;
        }
    }

    return EQMACMQ_REAL_CEverQuest__dsp_chat(this_ptr, a1, a2, a3);
}

int __cdecl EQMACMQ_DETOUR_DrawNetStatus(int a1, unsigned short a2, unsigned short a3, unsigned short a4, unsigned short a5, int a6, unsigned short a7, unsigned long a8, long a9, unsigned long a10)
{
    // a4 = x
    // a5 = y

/*
        FILE* file = fopen("eqmac_mq-debug-DrawNetStatus.txt", "a");
        fprintf(file, "a1: 0x%X\n", a1);
        fprintf(file, "a2: %d\n", a2);
        fprintf(file, "a3: %d\n", a3);
        fprintf(file, "a4 x: %d\n", a4);
        fprintf(file, "a5 y: %d\n", a5);
        fprintf(file, "a6: 0x%X\n", a6);
        fprintf(file, "a7: %d\n", a7);
        fprintf(file, "a8: 0x%X\n", a8);
        fprintf(file, "a9: 0x%X\n", a9);
        fprintf(file, "a10: 0x%X\n", a10);
        fprintf(file, "\n\n");
        fclose(file);
*/

    if (g_bExit == 1)
    {
        return EQMACMQ_REAL_DrawNetStatus(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    }

    // kill switch
    if (GetAsyncKeyState(VK_PAUSE))
    {
        if (g_killSwitchIsEnabled == true)
        {
            EQMACMQ_DoUnload();
        }
    }

    if (g_bLoaded == false)
    {
        g_bLoaded = true;

        EQMACMQ_UpdateEqProcesses();

        EQ_WriteMemory<BYTE>(EQ_IS_NET_STATUS_ENABLED, 1);

        if (g_writeTextToChatWindowIsEnabled == true)
        {
            if (EQ_IsInGame() == true)
            {
                char loadedText[128];
                _snprintf_s(loadedText, sizeof(loadedText), _TRUNCATE, "%s loaded.", g_applicationName);

                EQ_CLASS_CEverQuest->dsp_chat(loadedText);
            }
        }
    }

    DWORD currentTime = EQ_ReadMemory<DWORD>(EQ_TIMER);

    if (g_ignoreKeysIsEnabled == true)
    {
        if (EQMACMQ_IsForegroundWindowCurrentProcessId() == false)
        {
            g_ignoreKeysTimer = 0;
        }
        else
        {
            g_ignoreKeysTimer++;

            if (g_ignoreKeysTimer > 10000)
            {
                g_ignoreKeysTimer = 1;
            }
        }
    }

    if (g_limitCpuUsageIsEnabled == true)
    {
        if (EQMACMQ_IsForegroundWindowCurrentProcessId() == false)
        {
            Sleep(g_limitCpuUsageBackgroundDelay);
        }
        else
        {
            Sleep(g_limitCpuUsageForegroundDelay);
        }
    }

    if (g_boxChatIsEnabled == true)
    {
        if (g_boxChatShowConnectedMessage == true)
        {
            if (g_writeTextToChatWindowIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("Box Chat connected.");
            }

            g_boxChatShowConnectedMessage = false;
        }

        if (g_boxChatShowDisconnectedMessage == true)
        {
            if (g_writeTextToChatWindowIsEnabled == true)
            {
                EQ_CLASS_CEverQuest->dsp_chat("Box Chat disconnected.");
            }

            g_boxChatShowDisconnectedMessage = false;
        }

        for (size_t i = 0; i < EQMACMQ_BOX_CHAT_COMMANDS_MAX; i++)
        {
            if (strlen(g_boxChatInterpretCommandListEx[i]) > 0)
            {
                if (g_writeTextToChatWindowIsEnabled == true)
                {
                    char boxChatText[128];
                    _snprintf_s(boxChatText, sizeof(boxChatText), _TRUNCATE, "Box Chat: %s", g_boxChatInterpretCommandListEx[i]);

                    EQ_CLASS_CEverQuest->dsp_chat(boxChatText);
                }

                if (EQ_IsInGame() == true)
                {
                    PEQSPAWNINFO playerSpawn = (PEQSPAWNINFO)EQ_OBJECT_PlayerSpawn;

                    if (playerSpawn)
                    {
                        EQ_CLASS_CEverQuest->InterpretCmd((EQPlayer*)playerSpawn, g_boxChatInterpretCommandListEx[i]);
                    }
                }

                memset(g_boxChatInterpretCommandListEx[i], 0, EQMACMQ_BOX_CHAT_TEXT_MAX);
            }
        }
    }

    g_numDeferred2dItems = 0;

    if (EQ_IsNetStatusEnabled() == false)
    {
        g_messageTextTimer = 0;
    }

    EQMACMQ_DoUpdateZone();

    if (g_lootAllIsInProgress == true)
    {
        DWORD currentTime = EQ_ReadMemory<DWORD>(EQ_TIMER);

        if (currentTime < (g_lootAllTimer + g_lootAllDelay))
        {
            //
        }
        else
        {
            EQMACMQ_DoLootAll();

            g_lootAllTimer = currentTime;
        }
    }

    if (g_freeCameraIsEnabled == true)
    {
        if (EQ_IsNotTypingInChat() == true)
        {
            EQMACMQ_DoFreeCameraKeys();
        }
    }

    if (g_skillHackIsEnabled == true)
    {
        EQMACMQ_DoSkillHack();
    }

    if (g_bardTwistIsEnabled == true)
    {
        EQMACMQ_DoBardTwist();
    }

    if (g_meleeIsEnabled == true)
    {
        EQMACMQ_DoMelee();
    }

    if (g_healthBarsIsEnabled == true)
    {
        EQMACMQ_DoHealthBars();
    }

    EQMACMQ_DoButtonMouseLeftHeldDown();

    if (g_espIsEnabled == true && g_mapIsMaximized == false)
    {
        EQMACMQ_DoEsp();
    }

    if (g_playerInfoIsEnabled == true)
    {
        EQMACMQ_DoPlayerInfo();
    }

    if (g_targetInfoIsEnabled == true)
    {
        EQMACMQ_DoTargetInfo();
    }

    if (g_buffInfoIsEnabled == true)
    {
        EQMACMQ_DoBuffInfo();
    }

    if (g_mapIsEnabled == true)
    {
        EQMACMQ_DoMap();
    }

    if (g_messageTextIsEnabled == true)
    {
        EQMACMQ_DoMessageText();
    }

    if (g_messageTextOnGainedExperienceIsEnabled == true)
    {
        EQMACMQ_DoMessageTextGainedExperience();
    }

    if (g_clientSwitcherIsEnabled == true && g_clientSwitcherButtonsIsEnabled == true)
    {
        EQMACMQ_DoClientSwitcherButtons();
    }

    if (g_hudButtonsIsEnabled == true)
    {
        EQMACMQ_DoHudButtons();
    }

    // redraw the cursor so that the HUD is underneath it
    DWORD mouseClickState = EQ_ReadMemory<DWORD>(EQ_MOUSE_CLICK_STATE);

    if (mouseClickState != EQ_MOUSE_CLICK_STATE_RIGHT) // hide the cursor while mouse looking
    {
        EQ_CLASS_CXWndManager->DrawCursor();
    }

    EQMACMQ_DoButtonToolTipText();

    return EQMACMQ_REAL_DrawNetStatus(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

DWORD WINAPI EQMACMQ_ThreadLoop(LPVOID param)
{
    while (g_bExit == 0)
    {
        if (g_boxChatIsEnabled == true)
        {
            if (g_boxChatAutoConnectIsEnabled == true)
            {
                while (g_boxChatIsConnected == false)
                {
                    if (EQMACMQ_DoBoxChatConnect() == true)
                    {
                        g_boxChatIsConnected = true;
                        g_boxChatSendConnectedMessage = true;
                        g_boxChatShowConnectedMessage = true;
                        break;
                    }

                    if (g_bExit == 1)
                    {
                        break;
                    }

                    Sleep(1000);
                }
            }

            if (g_boxChatSendConnectedMessage == true)
            {
                char connectMessage[128];

                if (EQ_IsInGame() == true)
                {
                    _snprintf_s(connectMessage, sizeof(connectMessage), _TRUNCATE, "Name: %s|", EQ_OBJECT_PlayerSpawn->Name);
                }
                else
                {
                    _snprintf_s(connectMessage, sizeof(connectMessage), _TRUNCATE, "Unknown Player connected|");
                }

                if (send(g_connectSocket, connectMessage, (int)strlen(connectMessage), 0) == SOCKET_ERROR)
                {
                    g_boxChatIsConnected = false;
                    g_boxChatShowDisconnectedMessage = true;
                }

                g_boxChatSendConnectedMessage = false;
            }

            if (EQ_IsInGame() == false)
            {
                Sleep(1000);
                continue;
            }

            if (g_boxChatIsConnected == true && g_connectSocket != INVALID_SOCKET)
            {
                DWORD currentTime = EQ_ReadMemory<DWORD>(EQ_TIMER);

                if (currentTime > (g_boxChatKeepAliveTimer + g_boxChatKeepAliveDelay))
                {
                    char keepAliveMessage[128];
                    strncpy_s(keepAliveMessage, sizeof(keepAliveMessage), "Keep Alive|", _TRUNCATE);

                    if (send(g_connectSocket, keepAliveMessage, (int)strlen(keepAliveMessage), 0) == SOCKET_ERROR)
                    {
                        g_boxChatIsConnected = false;
                        g_boxChatShowDisconnectedMessage = true;
                    }

                    g_boxChatKeepAliveTimer = currentTime;
                }

                FD_ZERO(&g_fdSetRead);

                FD_SET(g_connectSocket, &g_fdSetRead);

                timeval tv;
                tv.tv_sec  = 0;
                tv.tv_usec = 0;

                int selectResult = select(0, &g_fdSetRead, NULL, NULL, &tv);

                if (selectResult != SOCKET_ERROR)
                {
                    if (FD_ISSET(g_connectSocket, &g_fdSetRead))
                    {
                        memset(g_boxChatRecvBuffer, 0, EQMACMQ_BOX_CHAT_RECV_MAX);

                        int recvResult = recv(g_connectSocket, g_boxChatRecvBuffer, EQMACMQ_BOX_CHAT_RECV_MAX, 0);

                        if (recvResult > 0)
                        {
                            //EQ_CLASS_CEverQuest->dsp_chat(g_boxChatRecvBuffer);

                            char receivedText[EQMACMQ_BOX_CHAT_RECV_MAX];
                            strncpy_s(receivedText, sizeof(receivedText), g_boxChatRecvBuffer, _TRUNCATE);

                            char* token     = NULL;
                            char* tokenNext = NULL;

                            token = strtok_s(receivedText, "|", &tokenNext);
                            while (token != NULL)
                            {
                                if (strncmp(token, "Command: ", 9) == 0)
                                {
                                    char commandText[EQMACMQ_BOX_CHAT_TEXT_MAX] = {0};
                                    strncpy_s(commandText, sizeof(commandText), token, _TRUNCATE);

                                    char* token2     = NULL;
                                    char* token2Next = NULL;

                                    token2 = strtok_s(commandText, ";", &token2Next);
                                    while (token2 != NULL)
                                    {
                                        char cmdText[EQMACMQ_BOX_CHAT_TEXT_MAX];
                                        strncpy_s(cmdText, sizeof(cmdText), token2 + 9, _TRUNCATE);

                                        if (cmdText[0] == '/' && strstr(cmdText, ";") == NULL)
                                        {
                                            for (size_t i = 0; i < EQMACMQ_BOX_CHAT_COMMANDS_MAX; i++)
                                            {
                                                if (strlen(g_boxChatInterpretCommandList[i]) == 0)
                                                {
                                                    strncpy_s(g_boxChatInterpretCommandList[i], sizeof(g_boxChatInterpretCommandList[i]), cmdText, _TRUNCATE);
                                                    break;
                                                }
                                            }
                                        }

                                        token2 = strtok_s(NULL, ";", &token2Next);
                                    }
                                }

                                token = strtok_s(NULL, "|", &tokenNext);
                            }
                        }
                    }
                }

                for (size_t i = 0; i < EQMACMQ_BOX_CHAT_COMMANDS_MAX; i++)
                {
                    if (strlen(g_boxChatSendCommandList[i]) > 0)
                    {
                        char sendText[EQMACMQ_BOX_CHAT_TEXT_MAX];
                        _snprintf_s(sendText, sizeof(sendText), _TRUNCATE, "%s|", g_boxChatSendCommandList[i]);

                        if (send(g_connectSocket, sendText, (int)strlen(sendText), 0) == SOCKET_ERROR)
                        {
                            g_boxChatIsConnected = false;
                            g_boxChatShowDisconnectedMessage = true;
                        }

                        memset(g_boxChatSendCommandList[i], 0, EQMACMQ_BOX_CHAT_TEXT_MAX);
                    }
                }
            }

            for (size_t i = 0; i < EQMACMQ_BOX_CHAT_COMMANDS_MAX; i++)
            {
                if (strlen(g_boxChatInterpretCommandList[i]) > 0)
                {
                    //EQ_CLASS_CEverQuest->dsp_chat(g_boxChatInterpretCommandList[i]);

                    if (strlen(g_boxChatInterpretCommandList[i]) > 7 && strncmp(g_boxChatInterpretCommandList[i], "/pause ", 7) == 0)
                    {
                        char sleepText[128];
                        strncpy_s(sleepText, sizeof(sleepText), g_boxChatInterpretCommandList[i] + 7, _TRUNCATE);

                        int sleepTime = atoi(sleepText);

                        if (sleepTime > 0)
                        {
                            Sleep((int)(sleepTime * 1000));
                        }

                        memset(g_boxChatInterpretCommandList[i], 0, EQMACMQ_BOX_CHAT_TEXT_MAX);

                        continue;
                    }

                    for (size_t j = 0; j < EQMACMQ_BOX_CHAT_COMMANDS_MAX; j++)
                    {
                        if (strlen(g_boxChatInterpretCommandListEx[j]) == 0)
                        {
                            strncpy_s(g_boxChatInterpretCommandListEx[j], sizeof(g_boxChatInterpretCommandListEx[j]), g_boxChatInterpretCommandList[i], _TRUNCATE);
                            break;
                        }
                    }

                    memset(g_boxChatInterpretCommandList[i], 0, EQMACMQ_BOX_CHAT_TEXT_MAX);

                    Sleep(1000);
                }
            }
        }

        Sleep(100);
    }

    Sleep(100);

    if (shutdown(g_connectSocket, SD_BOTH) == SOCKET_ERROR)
    {
        closesocket(g_connectSocket);
        //WSACleanup();
    }

    closesocket(g_connectSocket);
    //WSACleanup();

    EQMACMQ_DoHacksLoad(false);

    EQMACMQ_DoFreeCamera(false);

    DetourRemove((PBYTE)EQMACMQ_REAL_CEverQuest__LMouseDown, (PBYTE)EQMACMQ_DETOUR_CEverQuest__LMouseDown);
    DetourRemove((PBYTE)EQMACMQ_REAL_CEverQuest__LMouseUp,   (PBYTE)EQMACMQ_DETOUR_CEverQuest__LMouseUp);

    DetourRemove((PBYTE)EQMACMQ_REAL_HandleMouseWheel, (PBYTE)EQMACMQ_DETOUR_HandleMouseWheel);

    DetourRemove((PBYTE)EQMACMQ_REAL_ProcessKeyDown, (PBYTE)EQMACMQ_DETOUR_ProcessKeyDown);
    DetourRemove((PBYTE)EQMACMQ_REAL_ProcessKeyUp,   (PBYTE)EQMACMQ_DETOUR_ProcessKeyUp);

    //DetourRemove((PBYTE)EQMACMQ_REAL_ProcessMovementKeys, (PBYTE)EQMACMQ_DETOUR_ProcessMovementKeys);

    DetourRemove((PBYTE)EQMACMQ_REAL_CEverQuest__InterpretCmd, (PBYTE)EQMACMQ_DETOUR_CEverQuest__InterpretCmd);

    DetourRemove((PBYTE)EQMACMQ_REAL_CEverQuest__dsp_chat, (PBYTE)EQMACMQ_DETOUR_CEverQuest__dsp_chat);

    DetourRemove((PBYTE)EQMACMQ_REAL_DrawNetStatus, (PBYTE)EQMACMQ_DETOUR_DrawNetStatus);

    DetourRemove((PBYTE)EQMACMQ_REAL_CDisplay__SetNameSpriteState, (PBYTE)EQMACMQ_DETOUR_CDisplay__SetNameSpriteState);
    DetourRemove((PBYTE)EQMACMQ_REAL_CDisplay__SetNameSpriteTint,  (PBYTE)EQMACMQ_DETOUR_CDisplay__SetNameSpriteTint);

    DetourRemove((PBYTE)EQMACMQ_REAL_EQ_Character__eqspa_movement_rate, (PBYTE)EQMACMQ_DETOUR_EQ_Character__eqspa_movement_rate);

    DetourRemove((PBYTE)EQMACMQ_REAL_EQ_Character__CastSpell, (PBYTE)EQMACMQ_DETOUR_EQ_Character__CastSpell);

    DetourRemove((PBYTE)EQMACMQ_REAL_EQ_Character__UseSkill, (PBYTE)EQMACMQ_DETOUR_EQ_Character__UseSkill);

    //DetourRemove((PBYTE)EQMACMQ_REAL_AutoInventory, (PBYTE)EQMACMQ_DETOUR_AutoInventory);

    DetourRemove((PBYTE)EQMACMQ_REAL_CItemDisplayWnd__SetItem,  (PBYTE)EQMACMQ_DETOUR_CItemDisplayWnd__SetItem);
    DetourRemove((PBYTE)EQMACMQ_REAL_CItemDisplayWnd__SetSpell, (PBYTE)EQMACMQ_DETOUR_CItemDisplayWnd__SetSpell);

    DetourRemove((PBYTE)EQMACMQ_REAL_CBuffWindow__RefreshBuffDisplay, (PBYTE)EQMACMQ_DETOUR_CBuffWindow__RefreshBuffDisplay);

    DetourRemove((PBYTE)EQMACMQ_REAL_CSpellBookWnd__StartSpellMemorization,  (PBYTE)EQMACMQ_DETOUR_CSpellBookWnd__StartSpellMemorization);
    DetourRemove((PBYTE)EQMACMQ_REAL_CSpellBookWnd__FinishMemorizing,        (PBYTE)EQMACMQ_DETOUR_CSpellBookWnd__FinishMemorizing);

    DetourRemove((PBYTE)EQMACMQ_REAL_CLootWnd__Deactivate, (PBYTE)EQMACMQ_DETOUR_CLootWnd__Deactivate);

    Sleep(1000);

    FreeLibraryAndExitThread(g_module, 0);
    return 0;
}

DWORD WINAPI EQMACMQ_ThreadLoad(LPVOID param)
{
    EQMACMQ_EnableDebugPrivileges();

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

    char eqClientIniNewUi[128];
    EQMACMQ_ConfigReadString(".\\eqclient.ini", "Defaults", "NewUI", "TRUE", eqClientIniNewUi, sizeof(eqClientIniNewUi));

    if (_stricmp(eqClientIniNewUi, "FALSE") == 0)
    {
        MessageBoxA(NULL, "Error: Classic UI / Velious UI not supported!\nSet NewUI=TRUE in eqclient.ini to use the Luclin UI.", g_applicationName, MB_ICONERROR);

        FreeLibraryAndExitThread(g_module, 0);
        return 0;
    }

    EQMACMQ_DoLoadConfig();

    EQMACMQ_GuiRecalculateCoordinates();

    if (g_hacksIsEnabled == true)
    {
        EQMACMQ_DoHacksLoad(true);
    }

    g_handleThreadLoop = CreateThread(NULL, 0, &EQMACMQ_ThreadLoop, NULL, 0, NULL);

    EQMACMQ_REAL_CEverQuest__LMouseDown = (EQ_FUNCTION_TYPE_CEverQuest__LMouseDown)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__LMouseDown, (PBYTE)EQMACMQ_DETOUR_CEverQuest__LMouseDown);
    EQMACMQ_REAL_CEverQuest__LMouseUp   = (EQ_FUNCTION_TYPE_CEverQuest__LMouseUp)  DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__LMouseUp,   (PBYTE)EQMACMQ_DETOUR_CEverQuest__LMouseUp);

    EQMACMQ_REAL_HandleMouseWheel = (EQ_FUNCTION_TYPE_HandleMouseWheel)DetourFunction((PBYTE)EQ_FUNCTION_HandleMouseWheel, (PBYTE)EQMACMQ_DETOUR_HandleMouseWheel);

    EQMACMQ_REAL_ProcessKeyDown = (EQ_FUNCTION_TYPE_ProcessKeyDown)DetourFunction((PBYTE)EQ_FUNCTION_ProcessKeyDown, (PBYTE)EQMACMQ_DETOUR_ProcessKeyDown);
    EQMACMQ_REAL_ProcessKeyUp   = (EQ_FUNCTION_TYPE_ProcessKeyUp)  DetourFunction((PBYTE)EQ_FUNCTION_ProcessKeyUp,   (PBYTE)EQMACMQ_DETOUR_ProcessKeyUp);

    //EQMACMQ_REAL_ProcessMovementKeys = (EQ_FUNCTION_TYPE_ProcessMovementKeys)DetourFunction((PBYTE)EQ_FUNCTION_ProcessMovementKeys, (PBYTE)EQMACMQ_DETOUR_ProcessMovementKeys);

    EQMACMQ_REAL_CEverQuest__InterpretCmd = (EQ_FUNCTION_TYPE_CEverQuest__InterpretCmd)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__InterpretCmd, (PBYTE)EQMACMQ_DETOUR_CEverQuest__InterpretCmd);

    EQMACMQ_REAL_CEverQuest__dsp_chat = (EQ_FUNCTION_TYPE_CEverQuest__dsp_chat)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__dsp_chat, (PBYTE)EQMACMQ_DETOUR_CEverQuest__dsp_chat);

    EQMACMQ_REAL_DrawNetStatus = (EQ_FUNCTION_TYPE_DrawNetStatus)DetourFunction((PBYTE)EQ_FUNCTION_DrawNetStatus, (PBYTE)EQMACMQ_DETOUR_DrawNetStatus);

    EQMACMQ_REAL_CDisplay__SetNameSpriteState = (EQ_FUNCTION_TYPE_CDisplay__SetNameSpriteState)DetourFunction((PBYTE)EQ_FUNCTION_CDisplay__SetNameSpriteState, (PBYTE)EQMACMQ_DETOUR_CDisplay__SetNameSpriteState);
    EQMACMQ_REAL_CDisplay__SetNameSpriteTint  = (EQ_FUNCTION_TYPE_CDisplay__SetNameSpriteTint) DetourFunction((PBYTE)EQ_FUNCTION_CDisplay__SetNameSpriteTint,  (PBYTE)EQMACMQ_DETOUR_CDisplay__SetNameSpriteTint);

    EQMACMQ_REAL_EQ_Character__eqspa_movement_rate = (EQ_FUNCTION_TYPE_EQ_Character__eqspa_movement_rate)DetourFunction((PBYTE)EQ_FUNCTION_EQ_Character__eqspa_movement_rate, (PBYTE)EQMACMQ_DETOUR_EQ_Character__eqspa_movement_rate);

    EQMACMQ_REAL_EQ_Character__CastSpell = (EQ_FUNCTION_TYPE_EQ_Character__CastSpell)DetourFunction((PBYTE)EQ_FUNCTION_EQ_Character__CastSpell, (PBYTE)EQMACMQ_DETOUR_EQ_Character__CastSpell);

    EQMACMQ_REAL_EQ_Character__UseSkill = (EQ_FUNCTION_TYPE_EQ_Character__UseSkill)DetourFunction((PBYTE)EQ_FUNCTION_EQ_Character__UseSkill, (PBYTE)EQMACMQ_DETOUR_EQ_Character__UseSkill);

    //EQMACMQ_REAL_AutoInventory = (EQ_FUNCTION_TYPE_AutoInventory)DetourFunction((PBYTE)EQ_FUNCTION_AutoInventory, (PBYTE)EQMACMQ_DETOUR_AutoInventory);

    EQMACMQ_REAL_CItemDisplayWnd__SetItem  = (EQ_FUNCTION_TYPE_CItemDisplayWnd__SetItem) DetourFunction((PBYTE)EQ_FUNCTION_CItemDisplayWnd__SetItem,  (PBYTE)EQMACMQ_DETOUR_CItemDisplayWnd__SetItem);
    EQMACMQ_REAL_CItemDisplayWnd__SetSpell = (EQ_FUNCTION_TYPE_CItemDisplayWnd__SetSpell)DetourFunction((PBYTE)EQ_FUNCTION_CItemDisplayWnd__SetSpell, (PBYTE)EQMACMQ_DETOUR_CItemDisplayWnd__SetSpell);

    EQMACMQ_REAL_CBuffWindow__RefreshBuffDisplay = (EQ_FUNCTION_TYPE_CBuffWindow__RefreshBuffDisplay)DetourFunction((PBYTE)EQ_FUNCTION_CBuffWindow__RefreshBuffDisplay, (PBYTE)EQMACMQ_DETOUR_CBuffWindow__RefreshBuffDisplay);

    EQMACMQ_REAL_CSpellBookWnd__StartSpellMemorization = (EQ_FUNCTION_TYPE_CSpellBookWnd__StartSpellMemorization)DetourFunction((PBYTE)EQ_FUNCTION_CSpellBookWnd__StartSpellMemorization, (PBYTE)EQMACMQ_DETOUR_CSpellBookWnd__StartSpellMemorization);
    EQMACMQ_REAL_CSpellBookWnd__FinishMemorizing       = (EQ_FUNCTION_TYPE_CSpellBookWnd__FinishMemorizing)      DetourFunction((PBYTE)EQ_FUNCTION_CSpellBookWnd__FinishMemorizing,       (PBYTE)EQMACMQ_DETOUR_CSpellBookWnd__FinishMemorizing);

    EQMACMQ_REAL_CLootWnd__Deactivate = (EQ_FUNCTION_TYPE_CLootWnd__Deactivate)DetourFunction((PBYTE)EQ_FUNCTION_CLootWnd__Deactivate, (PBYTE)EQMACMQ_DETOUR_CLootWnd__Deactivate);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    g_module = module;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(module);
            g_handleThreadLoad = CreateThread(NULL, 0, &EQMACMQ_ThreadLoad, NULL, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
