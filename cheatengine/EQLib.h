
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TESTLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TESTLIB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef EQLIB_EXPORTS
#define EQLIB_API __declspec(dllexport)
#else
#define EQLIB_API __declspec(dllimport)
#endif


// ***************************************************************************
// EQ Addresses
// ***************************************************************************
DWORD EQADDR_WRITECHATCOLOR=0;
DWORD EQADDR_GETRACEBYID=0;
DWORD EQADDR_GETCLASSBYID=0;
DWORD EQADDR_SCREENITEM=0;
DWORD EQADDR_SCREENSPAWN=0;
DWORD EQADDR_NEWUIINI=0;
DWORD EQADDR_MAXMANA=0;

DWORD EQADDR_LEFTCLICK=0;
DWORD EQADDR_RIGHTCLICK=0;
DWORD EQADDR_PACKLOCS=0;

DWORD EQADDR_CMDLIST=0;
DWORD EQADDR_CMDLIST_0=0;
DWORD EQADDR_CMDLIST_4=0;
DWORD EQADDR_CMDLIST_8=0;
DWORD EQADDR_CMDLIST_C=0;
DWORD EQADDR_CMDLIST_10_1=0;
DWORD EQADDR_CMDLIST_10_2=0;

DWORD EQADDR_ATTACK=0;

DWORD EQADDR_ZONING=0;
DWORD EQADDR_ZONELIST=0;
DWORD EQADDR_SPAWNLIST=0;
DWORD EQADDR_SPAWNTAIL=0;
DWORD EQADDR_CHAR=0;
DWORD EQADDR_CHAR_INFO=0;
DWORD EQADDR_TARGET=0;
DWORD EQADDR_ITEMS=0;
DWORD EQADDR_DOORS=0;
DWORD EQADDR_CLSITEMS=0;
DWORD EQADDR_CLSSPAWNS=0;
DWORD EQADDR_CLSMAIN=0;

DWORD EQADDR_GUILDLIST=0;

DWORD EQADDR_MOUSE=0;

DWORD EQADDR_DIMAIN=0;
DWORD EQADDR_DIKEYBOARD=0;
DWORD EQADDR_DIMOUSE=0;


// ***************************************************************************
// Defines
// ***************************************************************************
#define SPAWN_PLAYER                    0
#define SPAWN_NPC                       1
#define SPAWN_CORPSE                    2
#define SPAWN_ANY						3
#define SPAWN_PET						4

#define ITEM_NORMAL1                    0x0031
#define ITEM_NORMAL2                    0x0036
#define ITEM_NORMAL3                    0x315f
#define ITEM_NORMAL4                    0x3336
#define ITEM_NORMAL5                    0x0032
#define ITEM_NORMAL6                    0x0033
#define ITEM_NORMAL7                    0x0034
#define ITEM_NORMAL8                    0x0039
#define ITEM_CONTAINER                  0x7900
#define ITEM_CONTAINER_PLAIN            0x7953
#define ITEM_BOOK                       0x7379

#define COLOR_DEFAULT                   0x00 +   0
#define COLOR_DARKGREY                  0x00 +   1
#define COLOR_DARKGREEN                 0x00 +   2
#define COLOR_DARKBLUE                  0x00 +   3
#define COLOR_PURPLE                    0x00 +   5
#define COLOR_LIGHTGREY                 0x00 +   6

#define CONCOLOR_GREEN	                0x00 +  14
#define CONCOLOR_LIGHTBLUE	            0x00 +  18
#define CONCOLOR_BLUE	                0x00 +  16
#define CONCOLOR_BLACK	                0x00 +  20
#define CONCOLOR_YELLOW	                0x00 +  15
#define CONCOLOR_RED	                0x00 +  13

#define USERCOLOR_SAY					0xFF +   1 //  1  - Say
#define USERCOLOR_TELL					0xFF +   2 //  2  - Tell
#define USERCOLOR_GROUP					0xFF +   3 //  3  - Group
#define USERCOLOR_GUILD					0xFF +   4 //  4  - Guild
#define USERCOLOR_OOC					0xFF +   5 //  5  - OOC
#define USERCOLOR_AUCTION				0xFF +   6 //  6  - Auction
#define USERCOLOR_SHOUT					0xFF +   7 //  7  - Shout
#define USERCOLOR_EMOTE					0xFF +   8 //  8  - Emote
#define USERCOLOR_SPELLS				0xFF +   9 //  9  - Spells (meming, scribing, casting, etc.)
#define USERCOLOR_YOU_HIT_OTHER			0xFF +  10 //  10 - You hit other
#define USERCOLOR_OTHER_HIT_YOU			0xFF +  11 //  11 - Other hits you
#define USERCOLOR_YOU_MISS_OTHER		0xFF +  12 //  12 - You miss other
#define USERCOLOR_OTHER_MISS_YOU		0xFF +  13 //  13 - Other misses you
#define USERCOLOR_DUELS					0xFF +  14 //  14 - Some broadcasts (duels)
#define USERCOLOR_SKILLS				0xFF +  15 //  15 - Skills (ups, non-combat use, etc.)
#define USERCOLOR_DISCIPLINES			0xFF +  16 //  16 - Disciplines or special abilities
#define USERCOLOR_UNUSED001				0xFF +  17 //  17 - Unused at this time
#define USERCOLOR_DEFAULT				0xFF +  18 //  18 - Default text and stuff you type
#define USERCOLOR_UNUSED002				0xFF +  19 //  19 - Unused at this time
#define USERCOLOR_MERCHANT_OFFER		0xFF +  20 //  20 - Merchant Offer Price
#define USERCOLOR_MERCHANT_EXCHANGE		0xFF +  21 //  21 - Merchant Buy/Sell
#define USERCOLOR_YOUR_DEATH			0xFF +  22 //  22 - Your death message
#define USERCOLOR_OTHER_DEATH			0xFF +  23 //  23 - Others death message
#define USERCOLOR_OTHER_HIT_OTHER		0xFF +  24 //  24 - Other damage other
#define USERCOLOR_OTHER_MISS_OTHER		0xFF +  25 //  25 - Other miss other
#define USERCOLOR_WHO					0xFF +  26 //  26 - /who command
#define USERCOLOR_YELL					0xFF +  27 //  27 - yell for help
#define USERCOLOR_NON_MELEE				0xFF +  28 //  28 - Hit for non-melee
#define USERCOLOR_SPELL_WORN_OFF		0xFF +  29 //  29 - Spell worn off
#define USERCOLOR_MONEY_SPLIT			0xFF +  30 //  30 - Money splits
#define USERCOLOR_LOOT					0xFF +  21 //  31 - Loot message
#define USERCOLOR_RANDOM				0xFF +  32 //  32 - Dice Roll (/random)
#define USERCOLOR_OTHERS_SPELLS			0xFF +  33 //  33 - Others spells
#define USERCOLOR_SPELL_FAILURE			0xFF +  34 //  34 - Spell Failures (resists, fizzles, missing component, bad target, etc.)
#define USERCOLOR_CHAT_CHANNEL			0xFF +  35 //  35 - Chat Channel Messages
#define USERCOLOR_CHAT_1				0xFF +  36 //  36 - Chat Channel 1
#define USERCOLOR_CHAT_2				0xFF +  37 //  37 - Chat Channel 2
#define USERCOLOR_CHAT_3				0xFF +  38 //  38 - Chat Channel 3
#define USERCOLOR_CHAT_4				0xFF +  39 //  39 - Chat Channel 4
#define USERCOLOR_CHAT_5				0xFF +  40 //  40 - Chat Channel 5
#define USERCOLOR_CHAT_6				0xFF +  41 //  41 - Chat Channel 6
#define USERCOLOR_CHAT_7				0xFF +  42 //  42 - Chat Channel 7
#define USERCOLOR_CHAT_8				0xFF +  43 //  43 - Chat Channel 8
#define USERCOLOR_CHAT_9				0xFF +  44 //  44 - Chat Channel 9
#define USERCOLOR_CHAT_10				0xFF +  45 //  45 - Chat Channel 10
#define USERCOLOR_MELEE_CRIT			0xFF +  46 //  46 - Melee Crits
#define USERCOLOR_SPELL_CRIT			0xFF +  47 //  47 - Spell Crits
#define USERCOLOR_TOO_FAR_AWAY			0xFF +  48 //  48 - Too far away (melee)
#define USERCOLOR_NPC_RAMAGE			0xFF +  49 //  49 - NPC Rampage
#define USERCOLOR_NPC_FURRY				0xFF +  50 //  50 - NPC Furry
#define USERCOLOR_NPC_ENRAGE			0xFF +  51 //  51 - NPC Enrage

#define COMP_NONE	0
#define COMP_EQ		1
#define COMP_NE		2
#define COMP_GT		3
#define COMP_LT		4
#define COMP_GE		5
#define COMP_LE		6
#define COMP_CONT   7

#define LASTFIND_NOTFOUND	96
#define LASTFIND_PRIMARY	97

#define EVENT_CHAT			0
#define EVENT_TIMER			1
#define EVENT_CUSTOM		2
#define NUM_EVENTS			3

#define CHAT_SAY			1
#define CHAT_TELL			2
#define CHAT_OOC			4
#define CHAT_SHOUT			8
#define CHAT_AUC			16
#define CHAT_GUILD			32
#define CHAT_GROUP			64
#define CHAT_CHAT			128
#define CHATEVENT(x)	(gEventChat & x)

#define FILTERSKILL_ALL		 0
#define FILTERSKILL_INCREASE 1
#define FILTERSKILL_NONE	 2

#define FILTERMACRO_ALL		 0
#define FILTERMACRO_ENHANCED 1
#define FILTERMACRO_NONE	 2


#define MAX_STRING            1024
#define MAX_MACROVARS	      100
#define MAX_MACROARRAYS	      20
#define MAX_STACKVARS	      5
#define MAX_TIMERS			  20
#define MAX_ALERTS			  10

// ***************************************************************************
// Structures
// ***************************************************************************
typedef struct _COMMON {
	signed char STR;
	signed char STA;
	signed char CHA;
	signed char DEX;
	signed char INT;
	signed char AGI;
	signed char WIS;
	signed char SvMagic;
	signed char SvFire;
	signed char SvCold;
	signed char SvDisease;
	signed char SvPoison;
	signed short HP;
	signed short Mana;
	signed short AC;

    BYTE Unknown0246[2];
    BYTE Light;
    BYTE Delay;
    BYTE Damage;
    BYTE Unknown0251;
    BYTE Range;
    BYTE Skill;
    BYTE Magic;
    BYTE CastingLevel0;
    BYTE Material;
    BYTE Unknown0258[3];
    DWORD Color;
    BYTE Unknown0264[2];
    WORD SpellId0;
    WORD Classes;
    BYTE Unknown0270[2];
    WORD Races;
    BYTE Unknown0274[2];
	BYTE Stackable;
    BYTE CastingLevel;

	union {// different meanings depending on an unknown indicator
		BYTE StackCount;
        BYTE Charges;
    };

	BYTE EffectType;
	WORD SpellId;
	BYTE Unknown0282[10];
	WORD SkillModId;
	signed char SkillModPercent;
	BYTE Unknown0315[41];

} COMMON, *PCOMMON;

typedef struct _BOOK {
      BYTE Unknown0228[3];
      CHAR File[15];
	  BYTE Unknown0246[15];
} BOOK, *PBOOK;

typedef struct _CONTAINER {
	struct _ITEMINFO *Contents[10];
	BYTE Combine;
	signed char Slots;
	BYTE Open;
	BYTE SizeCapacity;
	BYTE WeightReduction;
    BYTE Unknown0273[3];
} CONTAINER, *PCONTAINER;

typedef struct _ITEMINFO {
	CHAR Name[64];
	CHAR LoreName[80];
	CHAR IDFile[6];
	BYTE Unknown0150[24];
	BYTE Weight;
	BYTE NoRent;
	BYTE NoDrop;
	BYTE Size;
	BYTE IsContainer;
	BYTE Unknown0179;
	WORD ItemNumber;
	WORD IconNumber;
	DWORD EquipSlot;
	DWORD EquipableSlots;
	DWORD Cost;
	BYTE  Unknown0196[32];
	union { 
		COMMON    Common;
		CONTAINER Container;
		BOOK      Book;
	};
} ITEMINFO, *PITEMINFO;

typedef struct _CHARINFO {
	BYTE Unknown0000; // 0
	BYTE Unknown0001; // 3
	CHAR Name[64];
	CHAR Lastname[70];
	WORD Unknown0136;
	WORD Race;
	WORD Class;
	WORD Gender;
	WORD Level;
	//
	WORD Unknown0146;
	DWORD Exp;
	WORD Face;
	WORD Mana;
	WORD BaseHP;
	WORD Unknown0158;
	WORD BaseSTR;
	WORD BaseSTA;
	WORD BaseCHA;
	WORD BaseDEX;
	WORD BaseINT;
	WORD BaseAGI;
	WORD BaseWIS;
	BYTE Unknown0142[2692];
	WORD MemorizedSpells[8];
	BYTE Unknown2882[18];
	DWORD Unknown2900;
	DWORD Unknown2904;
	DWORD Unknown2908;
	DWORD Unknown2912;
	DWORD Unknown2916;
	DWORD Plat;
	DWORD Gold;
	DWORD Silver;
	DWORD Copper;
	DWORD BankPlat;
	DWORD BankGold;
	DWORD BankSilver;
	DWORD BankCopper;
	BYTE Unknown2952[32];
	WORD Skill[74];
	BYTE Unknown3132[312];
	struct _SPAWNINFO *pSpawn;
	PITEMINFO Inventory[30];
	DWORD Unknown3568;
	DWORD ZoneBoundId;
	DWORD ZoneBirthId;
	DWORD ZoneOtherId[3];
	FLOAT ZoneBoundX;
	FLOAT ZoneBirthX;
	FLOAT ZoneOtherX[3];
	FLOAT ZoneBoundY;
	FLOAT ZoneBirthY;
	FLOAT ZoneOtherY[3];
	FLOAT ZoneBoundZ;
	FLOAT ZoneBirthZ;
	FLOAT ZoneOtherZ[3];
	BYTE Unknown3652[4512];
	PITEMINFO Bank[8];
} CHARINFO, *PCHARINFO;

typedef struct _ACTORINFO {
	DWORD Unknown0000;
	DWORD Unknown0004;
	CHAR  ActorDef[20];
	BYTE Unknown00028[948];
	DWORD LFG;
    DWORD UnkFlag;

} ACTORINFO, *PACTORINFO;

typedef struct _SPAWNINFO {
	BYTE Unknown0000;      // 3?
	CHAR Name[30];
	BYTE Unknown0031[37];
	DWORD Zone;
	FLOAT Y;
	FLOAT X;
	FLOAT Z;
	FLOAT Heading;
	FLOAT Unknown0068;
	FLOAT SpeedRun;
	FLOAT SpeedY;
	FLOAT SpeedX;
	FLOAT SpeedZ;
	FLOAT SpeedHeading;
	FLOAT Unknown0088;
	FLOAT CameraAngle; // 128=up -128=down
	struct _SPAWNINFO *pPrev;
	struct _SPAWNINFO *pNext;
	PVOID pUnknown0108;    // Unknown... pointer? always 0000000
	PACTORINFO pActorInfo;
	PCHARINFO pCharInfo;
	FLOAT CameraHeightOffset;
	FLOAT ModelHeightOffset;
	WORD SpawnID;
	WORD MasterID;
	DWORD HPMax;
	DWORD HPCurrent;
	WORD GuildID;
	BYTE Unknown0162[6];  // Unknown
	BYTE Type;
	BYTE Class;
	WORD Race;
	BYTE Gender;
	BYTE Level;
	BYTE Unknown0174[78];
	BYTE UnkFlag001;
	BYTE Unknown0231[11];
	BYTE Linkdead;
	BYTE Guide;
	BYTE Unknown0244;
	BYTE GM;
	BYTE VisType;
	BYTE Unknown0247[7];
	DWORD Anon;
	BYTE Unknown0257[4];
	BYTE AFK;
	BYTE Unknown0261[15];
	CHAR Lastname[22];
} SPAWNINFO, *PSPAWNINFO;

typedef struct _DOOR {
	BYTE Unknown0000;
	BYTE Unknown0001[3];
	struct _DOOR *pPrev;
	struct _DOOR *pNext;
    BYTE Unknown0012;
	CHAR Name[11];
	DWORD Unknown0024;
	DWORD Unknown0028;
	FLOAT Y;
	FLOAT X;
	FLOAT Z;
	FLOAT Heading;
	FLOAT Y1;
	FLOAT X1;
	FLOAT Z1;
	FLOAT Heading1;
	FLOAT Heading0;
} DOOR, *PDOOR;

typedef struct _GROUNDITEM {
	struct _GROUNDITEM *pPrev;
	struct _GROUNDITEM *pNext;
	WORD ID;
	BYTE Unknown0010[2];
	WORD DropID;
	BYTE Unknown0014[6];
	DWORD DxID;
	FLOAT Y;
	FLOAT X;
	FLOAT Z;
	FLOAT Heading;
	CHAR Name[30];
} GROUNDITEM, *PGROUNDITEM;

typedef struct _ZONELIST {
	DWORD Header;
	CHAR Unknown0004[8];
	CHAR ShortName[129];
	CHAR LongName[129];
} ZONELIST, *PZONELIST;

typedef struct _SWHOSORT {
	CHAR szName[MAX_STRING];
	CHAR szLine[MAX_STRING];
	BYTE Level;
	WORD SpawnID;
	FLOAT Distance;
} SWHOSORT, *PSWHOSORT;

typedef struct _SWHOFILTER {
	CHAR szFilter[MAX_STRING];
	BYTE Type;
	BYTE MinLevel;
	BYTE MaxLevel;
	FLOAT Radius;
	DWORD Guild;
	BOOL LFG;
} SWHOFILTER, *PSWHOFILTER;

typedef struct _MOUSEINFO {
	DWORD X;
	DWORD Y;
	DWORD SpeedX;
	DWORD SpeedY;
} MOUSEINFO, *PMOUSEINFO;

typedef struct _SPELLINFO {
	CHAR Name[32];
	CHAR Target[32];
} SPELLINFO, *PSPELLINFO;

typedef struct _PACKLOC {
	DWORD X;
	DWORD Y;
} PACKLOC, *PPACKLOC;

typedef struct _CMDLIST {
	DWORD CmdID; /* New */
	PCHAR szName;
	PCHAR szName2;
	VOID  (__stdcall *fAddress)(PSPAWNINFO, PCHAR);
	WORD Restriction;
	WORD Category;
} CMDLIST, *PCMDLIST;

typedef struct _MACROBLOCK { 
	CHAR Line[MAX_STRING];
	struct _MACROBLOCK *pNext;
	struct _MACROBLOCK *pPrev;
} MACROBLOCK, *PMACROBLOCK;

typedef struct _ALERT { 
	CHAR Name[MAX_STRING];
	BYTE Type;
	FLOAT Radius;
	DWORD MinLevel;
	DWORD MaxLevel;
	struct _ALERT *pNext;
} ALERT, *PALERT;

typedef struct _MACROSTACK {
	PMACROBLOCK Location;
	struct _MACROSTACK *pNext;
	LONG Return;
	CHAR StackStr[MAX_STACKVARS][MAX_STRING];
} MACROSTACK, *PMACROSTACK;

typedef struct _KEYPRESS {
	WORD KeyId;
	BOOL Pressed;
	struct _KEYPRESS *pNext;
} KEYPRESS, *PKEYPRESS;

typedef struct _DIKEYID {
	CHAR szName[32];
	WORD Id;
} DIKEYID, *PDIKEYID;

typedef struct _ITEMDB {
	struct _ITEMDB *pNext;
	WORD ID;
	CHAR szName[256];
} ITEMDB, *PITEMDB;

typedef struct _DEFINE {
	struct _DEFINE *pNext;
	CHAR szName[MAX_STRING];
	CHAR szReplace[MAX_STRING];
} DEFINE, *PDEFINE;

typedef struct _EVENTLIST {
	struct _EVENTLIST *pNext;
	CHAR szName[MAX_STRING];
	CHAR szMatch[MAX_STRING];
	PMACROBLOCK pEventFunc;
} EVENTLIST, *PEVENTLIST;

typedef struct _EVENTSTACK {
	struct _EVENTSTACK *pNext;
	DWORD Type;
	PEVENTLIST pEventList;
	CHAR EventStr[MAX_STACKVARS][MAX_STRING];
} EVENTSTACK, *PEVENTSTACK;

// ***************************************************************************
// String arrays
// ***************************************************************************
PCHAR szHeading[] = {
	"south",				//0
	"south by southeast",	//1
	"southeast",			//2
	"east by southeast",	//3
	"east",					//4
	"east by northeast",	//5
	"northeast",			//6
	"north by northeast",	//7
	"north",				//8
	"north by northwest",	//9
	"northwest",			//10
	"west by northwest",	//11
	"west",					//12
	"west by southwest",	//13
	"southwest",			//14
	"south by southwest"	//15
};

PCHAR szHeadingShort[] = {
	"S",	//0
	"SSE",	//1
	"SE",	//2
	"ESE",	//3
	"E",	//4
	"ENE",	//5
	"NE",	//6
	"NNE",	//7
	"N",	//8
	"NNW",	//9
	"NW",	//10
	"WNW",	//11
	"W",	//12
	"WSW",	//13
	"SW",	//14
	"SSW"	//15
};

PCHAR szSize[] = {
	"Tiny",
	"Small",
	"Normal",
	"Large",
	"Giant",
	"Huge", //?
	"Unknown" //?
};

PCHAR szSpawnType[] = {
	"player",
	"NPC",
	"corpse",
	"any",
	NULL // SuperWhoTarget
};

PCHAR szGender[] = {
	"male",
	"female",
	"neuter",
	"unknown"
};

PCHAR szSkills[] = {
	#include "skills.h"
	NULL
};

PCHAR szCombineTypes[] = {
	#include "combines.h"
	NULL
};

// ***************************************************************************
// Everquest function typedefs
// ***************************************************************************
typedef DWORD	(__stdcall *fEQWriteChatColor)(PCHAR, DWORD);
typedef PCHAR	(__stdcall *fEQGetStringByID)(WORD);
typedef VOID	(__stdcall *fEQCommand)(PSPAWNINFO, PCHAR);
typedef VOID	(__stdcall *fEQClick)(DWORD, DWORD);
typedef HRESULT	(__stdcall *fGetDeviceState)(THIS_ DWORD, LPVOID);
typedef DWORD	(__stdcall *fEQScreenItem)(DWORD, DWORD, DWORD);
typedef DWORD	(__stdcall *fEQScreenSpawn)(DWORD, DWORD);
typedef PCHAR   (__stdcall *fEQNewUIINI)(VOID);

// ***************************************************************************
// EQ Functions
// ***************************************************************************
fEQCommand		  cmdHelp        =  NULL;
fEQCommand		  cmdWho         =  NULL;
fEQCommand		  cmdWhoTarget   =  NULL;
fEQCommand		  cmdLocation    =  NULL;
fEQCommand		  cmdFace        =  NULL;
fEQCommand		  cmdTarget      =  NULL;
fEQCommand		  cmdCharInfo    =  NULL;
fEQCommand		  cmdFilter      =  NULL;
fEQClick          LeftClick      =  NULL;
fEQClick          RightClick     =  NULL;
fEQWriteChatColor WriteChatColor =  NULL;
fEQGetStringByID  GetRaceByID    =  NULL;
fEQGetStringByID  GetClassByID   =  NULL;
fEQScreenItem	  ScreenItem     =  NULL;
fEQScreenSpawn	  ScreenSpawn    =  NULL;
fEQNewUIINI		  NewUIINI		 =  NULL;
// ***************************************************************************
// Prototypes
// ***************************************************************************
VOID		OverwriteTable			(DWORD Address);
VOID		Pulse					(VOID);
VOID		HookChat				(BOOL Patch);
VOID		HookDInput				(BOOL Patch);
BOOL		TakeControlOfCommandList(VOID);
VOID		ReleaseCommandList		(VOID);
BOOL		Include					(PCHAR szFile);
PCHAR		GetFullZone				(DWORD ZoneID);
PCHAR		ParseMacroParameter		(PSPAWNINFO pChar, PCHAR szOriginal);
VOID		DoCommand				(PSPAWNINFO pChar, PCHAR szLine);
PCHAR		CleanupName				(PCHAR szName, BOOL Article = TRUE);
VOID		SwapSWho				(PSWHOSORT pSWho1, PSWHOSORT pSWho2);
VOID		SortSWho				(PSWHOSORT pSWhoSort, DWORD SpawnCount, BOOL JustDistance = FALSE);
VOID		SuperWhoFindPets		(PSPAWNINFO pChar, WORD SpawnID);
VOID		SuperWhoDisplay			(PSPAWNINFO pChar, PSWHOFILTER pFilter, PSPAWNINFO pTarget, WORD Padding = 0, DWORD Color = USERCOLOR_WHO);
FLOAT		DistanceToSpawn			(PSPAWNINFO pChar, PSPAWNINFO pSpawn);
FLOAT		EstimatedDistanceToSpawn(PSPAWNINFO pChar, PSPAWNINFO pSpawn);
BOOL		AddMacroLine			(PCHAR szLine);
VOID		DebugSpew				(PCHAR szFormat, ...);
PCHAR		GetNextArg				(PCHAR szLine, DWORD dwNumber = 1);
PCHAR		GetArg					(PCHAR szDest, PCHAR szSrc, DWORD dwNumber, BOOL LeaveQuotes = FALSE, BOOL ToParen = FALSE);
PSPAWNINFO	GetClosestAlert			(PSPAWNINFO pChar,DWORD List);
VOID		FreeAlerts				(DWORD List);
DWORD		ConColor				(WORD CharLevel, WORD SpawnLevel);
DWORD WINAPI InsertCommands			(LPVOID lpParameter);
VOID		FailIf					(PSPAWNINFO pChar, PCHAR szCommand, BOOL All=FALSE);


// ***************************************************************************
// Exports
// ***************************************************************************
extern "C" EQLIB_API VOID  SuperWho      (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  SuperWhoTarget(PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Help          (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Identify      (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Location      (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Where         (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Skills        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MacroPause    (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Skills        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Commands      (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Click         (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  SendKey       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Press         (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Cleanup       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MouseTo       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Macro         (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Face          (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Target        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  FindItem      (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Breakin       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Echo          (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  For           (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Next          (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Call          (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Return        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  If            (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Goto          (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  EndMacro      (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  VarCalc       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  VarSet        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  VarAdd        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  VarSub        (PSPAWNINFO, PCHAR);
//extern "C" EQLIB_API VOID  CameraHeight  (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Unload        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Delay         (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Alert         (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  DoEvents      (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  CharInfo      (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MacroLog      (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Filter        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MacroBeep     (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Items         (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  ItemTarget    (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  ZapVars       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  SetError		 (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  ListMacros	 (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MQMsgBox		 (PSPAWNINFO, PCHAR);

extern "C" EQLIB_API VOID  InjectEnable  ();
extern "C" EQLIB_API VOID  InjectDisable ();

#ifdef CMD_PACKLOC
extern "C" EQLIB_API VOID  PackLoc       (PSPAWNINFO, PCHAR);
#endif
