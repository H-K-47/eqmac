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
#define USERCOLOR_ECHO_SAY 				0xFF +  52 //  52 - say echo
#define USERCOLOR_ECHO_TELL				0xFF +  53 //  53 - tell echo
#define USERCOLOR_ECHO_GROUP			0xFF +  54 //  54 - group echo
#define USERCOLOR_ECHO_GUILD			0xFF +  55 //  55 - guild echo
#define USERCOLOR_ECHO_OOC				0xFF +  56 //  56 - group echo
#define USERCOLOR_ECHO_AUCTION			0xFF +  57 //  57 - auction echo
#define USERCOLOR_ECHO_SHOUT			0xFF +  58 //  58 - shout echo
#define USERCOLOR_ECHO_EMOTE			0xFF +  59 //  59 - emote echo
#define USERCOLOR_ECHO_CHAT_1			0xFF +  60 //  60 - chat 1 echo
#define USERCOLOR_ECHO_CHAT_2			0xFF +  61 //  61 - chat 2 echo
#define USERCOLOR_ECHO_CHAT_3			0xFF +  62 //  62 - chat 3 echo
#define USERCOLOR_ECHO_CHAT_4			0xFF +  63 //  63 - chat 4 echo
#define USERCOLOR_ECHO_CHAT_5			0xFF +  64 //  64 - chat 5 echo
#define USERCOLOR_ECHO_CHAT_6			0xFF +  65 //  65 - chat 6 echo
#define USERCOLOR_ECHO_CHAT_7			0xFF +  66 //  66 - chat 7 echo
#define USERCOLOR_ECHO_CHAT_8			0xFF +  67 //  67 - chat 8 echo
#define USERCOLOR_ECHO_CHAT_9			0xFF +  68 //  68 - chat 9 echo
#define USERCOLOR_ECHO_CHAT_10			0xFF +  69 //  69 - chat 10 echo

#define DEITY_Bertoxxulous			201
#define DEITY_BrellSerilis			202
#define DEITY_CazicThule			203
#define DEITY_ErollisiMarr			204
#define DEITY_Bristlebane			205
#define DEITY_Innoruuk				206
#define DEITY_Karana				207
#define DEITY_MithanielMarr			208
#define DEITY_Prexus				209
#define DEITY_Quellious				210
#define DEITY_RallosZek				211
#define DEITY_RodcetNife			212
#define DEITY_SolusekRo				213
#define DEITY_TheTribunal			214
#define DEITY_Tunare				215
#define DEITY_Veeshan				216

#define ITEMTYPE_NORMAL		0
#define ITEMTYPE_PACK		1
#define ITEMTYPE_BOOK		2

#define COMP_NONE		0
#define COMP_EQ			1
#define COMP_NE			2
#define COMP_GT			3
#define COMP_LT			4
#define COMP_GE			5
#define COMP_LE			6
#define COMP_CONT		7
#define COMP_NOTCONT	8
#define COMP_BITAND		9
#define COMP_BITOR		10

#define COMP_TYPE_STRING	0
#define COMP_TYPE_NUMERIC	1
#define COMP_TYPE_BIT		2

#define CMD_MQ 1
#define CMD_EQ 2

#define LASTFIND_NOTFOUND	96
#define LASTFIND_PRIMARY	97

#define EVENT_CHAT			0
#define EVENT_TIMER			1
#define EVENT_CUSTOM		2
#define EVENT_EVAL			3
#define EVENT_EXEC			4
#define EVENT_PULSE			5
#define EVENT_SHUTDOWN		6
#define EVENT_BREAK			7
#define NUM_EVENTS			8

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


#define MAX_STRING            2048
#define MAX_MACROVARS	      100
#define MAX_MACROARRAYS	      100
#define MAX_STACKVARS	      10
#define MAX_LOCALVARS		  10
#define MAX_TIMERS			  40
#define MAX_ALERTS			  10
#define MAX_SPAWNMONITORS     10

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
	signed long HP;
	signed long Mana;
	signed long AC;

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

	union {
		BYTE StackCount;
        BYTE Charges;
    };

	BYTE EffectType;
	WORD SpellId;
} COMMON, *PCOMMON;

typedef struct _BOOK {
      BYTE Unknown0228[3];
      CHAR File[15];
} BOOK, *PBOOK;

typedef struct _CONTAINER {
	struct _ITEMINFO *Contents[10];
	BYTE Combine;
	BYTE Slots;
	BYTE Open;
	BYTE SizeCapacity;
	BYTE WeightReduction;
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
	BYTE Type;
	BYTE Unknown0179;
	DWORD ItemNumber;
	DWORD IconNumber;
	DWORD EquipSlot;
	DWORD EquipableSlots;
	DWORD Cost;
	BYTE  Unknown0196[40];
	union { 
		COMMON    Common;
		CONTAINER Container;
		BOOK      Book;
	};
} ITEMINFO, *PITEMINFO;

typedef struct _SPELLBUFF
{
	WORD  Unknown0000;
	BYTE  Level;
	BYTE  Unknown0003;
	WORD  Unknown0004;
	WORD  SpellID;
	DWORD Duration;
	DWORD Unknown;
} SPELLBUFF, *PSPELLBUFF;

typedef struct _CHARINFO {
	BYTE Unknown0000; // 0
	BYTE Unknown0001; // 3
	CHAR Name[64];
	CHAR Lastname[70];
	DWORD Unknown0136;
	DWORD Race;
	DWORD Class;
	DWORD Gender;
	DWORD Level;
	DWORD Exp;
	DWORD Face;
	DWORD Mana;
	DWORD BaseHP;
	DWORD Stunned;
	DWORD BaseSTR;
	DWORD BaseSTA;
	DWORD BaseCHA;
	DWORD BaseDEX;
	DWORD BaseINT;
	DWORD BaseAGI;
	DWORD BaseWIS;
	BYTE Unknown0142[172];
	BYTE Language[25];
	BYTE Unknown0273[503];
	SPELLBUFF Buff[21];
	BYTE Unknown0730[1704];
	DWORD SpellBook[256];
	BYTE Unknown2342[1024];
	DWORD MemorizedSpells[8];
	BYTE Unknown2882[56];
	DWORD Plat;
	DWORD Gold;
	DWORD Silver;
	DWORD Copper;
	DWORD BankPlat;
	DWORD BankGold;
	DWORD BankSilver;
	DWORD BankCopper;
	BYTE Unknown2952[32];
	DWORD Skill[120];
	BYTE Unknown3224[132];
	DWORD AutoSplit;
	BYTE Unknown3348[96];
	struct _SPAWNINFO *pSpawn;
	PITEMINFO Inventory[30];
	PITEMINFO Cursor;
	BYTE Unknown3568[420];
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

	BYTE Unknown3652[3548];
	DWORD GuildID;
	BYTE Unknown4736[17];
	WORD Anon;					// not right loc
	BYTE GuildStatus;			// not right loc
	BYTE Unknown4756[452];
	DWORD AAExp;
	BYTE Unknown5208[428];
	DWORD AAPoints;	
	BYTE Unknown5689[2508];
	PITEMINFO Bank[8];
} CHARINFO, *PCHARINFO;

typedef struct _MODELINFONAME {
	DWORD Unknown0000;
	DWORD Unknown0004;
	PCHAR Name;
} MODELINFONAME, *PMODELINFONAME;

typedef struct _MODELINFO_GENERIC {
	DWORD Type;
	DWORD Unknown0004;
	DWORD Unknown0008;
	DWORD Unknown0012;
	DWORD Unknown0016;
} MODELINFO_GENERIC, *PMODELINFO_GENERIC;

typedef struct _MODELINFO_48 {
	MODELINFO_GENERIC Header;
    FLOAT Float1;
    FLOAT Float2;
    FLOAT Float3;
	PMODELINFONAME pModelName;
} MODELINFO_48, *PMODELINFO_48;

typedef struct _MODELINFO_51 {
	MODELINFO_GENERIC Header;
	PMODELINFONAME pFontName;
	PCHAR LabelText;
} MODELINFO_51, *PMODELINFO_51;

typedef struct _MODELINFO {
	DWORD Unknown0000;
	PCHAR NameDAG;
	PMODELINFO_GENERIC pModelInfo;
	struct _MODELINFO *pNext;
	PVOID pUnknown;
	PMODELINFONAME pTrack1;
	PMODELINFONAME pTrack2;
	FLOAT Float[40];
} MODELINFO, *PMODELINFO;

typedef struct _LIGHTINFO {
	DWORD Value0;
	DWORD Value1;
	DWORD Unknown0008;
	DWORD Unknown0012;
	PVOID pUnknown;
	FLOAT Y;
	FLOAT X;
	FLOAT Z;
	FLOAT Radius;
	DWORD Value2;
} LIGHTINFO, *PLIGHTINFO;

typedef struct _ACTORINFO {
	PVOID pCamera;
	PLIGHTINFO pDx8Light;		
	CHAR  ActorDef[20];
	BYTE  Unknown0162[9848];
	PVOID Unknown0644;
	PMODELINFO Model[20];
	BYTE  Unknown0728[352];
	DWORD LFG;
    DWORD Trader;

} ACTORINFO, *PACTORINFO;

#define MODEL_LABEL			0
#define MODEL_LABELINFO		1
#define MODEL_NULL2			2
#define MODEL_HELD_R		3
#define MODEL_HELD_L		4
#define MODEL_SHIELD		5
#define MODEL_NULL6			6
// ... useless armor stuff ...

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
	DWORD SpawnID;
	DWORD MasterID;
	DWORD HPMax;
	DWORD HPCurrent;
	WORD GuildID;
	BYTE Unknown0162[6];
	BYTE Type;
	BYTE Class;
	BYTE Unknown0172[2];
	DWORD Race;
	BYTE Gender;
	BYTE Level;
	BYTE Unknown0174[2];
	BYTE Charmed;
	BYTE StandState;
	BYTE Light;
	BYTE Unknown0179[93];
	BYTE UnkFlag0230;
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
	BYTE Unknown0285[7];
	WORD AARank;
	BYTE Unknown0294[6];
	CHAR Lastname[22];
	BYTE Unknown0298[12];
	WORD Deity;
} SPAWNINFO, *PSPAWNINFO;

#define STANDSTATE_STAND			0x64
#define STANDSTATE_BIND				0x69
#define STANDSTATE_SIT				0x6E
#define STANDSTATE_DUCK				0x6F
#define STANDSTATE_FEIGN			0x73
#define STANDSTATE_DEAD				0x78

#define MONITOR_SPAWN_X				1
#define MONITOR_SPAWN_Y				2
#define MONITOR_SPAWN_Z				4
#define MONITOR_SPAWN_HEADING		8
#define MONITOR_SPAWN_SPEEDRUN		16
#define MONITOR_SPAWN_HPCURRENT		32

typedef struct _SPAWNMONITORINFO {
	WORD SpawnID;
	FLOAT Y;
	FLOAT X;
	FLOAT Z;
    FLOAT Heading;
	FLOAT SpeedRun;
	DWORD HPCurrent;
	DWORD MonitorMask;
} SPAWNMONITORINFO, *PSPAWNMONITORINFO;

typedef struct _DOOR {
	PVOID pUnknown0000;
	struct _DOOR *pPrev;
	struct _DOOR *pNext;
    BYTE ID;
	CHAR Name[11];
	DWORD Unknown0024;
	DWORD Unknown0028;
	FLOAT DefaultY;
	FLOAT DefaultX;
	FLOAT DefaultZ;
	FLOAT DefaultHeading;
	FLOAT DoorAngle;
	FLOAT Y;
	FLOAT X;
	FLOAT Z;
	FLOAT Heading;
} DOOR, *PDOOR;

typedef struct _GROUNDITEM {
	struct _GROUNDITEM *pPrev;
	struct _GROUNDITEM *pNext;
	DWORD ID;
	DWORD DropID;
	DWORD Unknown;
	DWORD DxID;
	BYTE Unknown0024[176];
	DWORD Unknown2;
	FLOAT Heading;
	FLOAT Z;
	FLOAT X;
	FLOAT Y;
	CHAR Name[30];
} GROUNDITEM, *PGROUNDITEM;

typedef struct _ZONELIST {
	DWORD Header;
	CHAR Unknown0004[8];
	CHAR ShortName[129];
	CHAR LongName[129];
} ZONELIST, *PZONELIST;

typedef struct _SPELLLIST {
	DWORD ID;
	FLOAT Range;
	BYTE Unknown0004[12];
	DWORD CastTime;
	DWORD RecoveryTime;
	DWORD RecastTime;
	DWORD DurationType;
	DWORD DurationValue1;
	DWORD DurationValue2;
	DWORD Mana;
	DWORD Value1;
	BYTE Unknown0046[183];
	BYTE Level[15];
	BYTE Unknown0244[38];
	PCHAR Name;
	PCHAR Target;
	PCHAR szUnknown1;
	PCHAR szUnknown2;
	PCHAR szUnknown3;
	PCHAR CastOnYou;
	PCHAR CastOnOther;
	PCHAR WearOff;
	PVOID Effects;
} SPELLLIST, *PSPELLLIST;

typedef struct _SWHOSORT {
	CHAR szName[MAX_STRING];
	CHAR szLine[MAX_STRING];
	BYTE Level;
	DWORD SpawnID;
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
	DWORD Alert;
	DWORD NoAlert;
} SWHOFILTER, *PSWHOFILTER;

typedef struct _CONNECTION {
	SOCKET socket;
	BOOL Auth;
	struct _CONNECTION *pNext;
} CONNECTION, *PCONNECTION;

typedef struct _CHATBUF {
	CHAR szText[MAX_STRING];
	struct _CHATBUF *pNext;
} CHATBUF, *PCHATBUF;

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
	DWORD LocalizedStringID; 
	PCHAR szName;
	PCHAR szLocalized;
	VOID  (__cdecl *fAddress)(PSPAWNINFO, PCHAR);
	DWORD Restriction;
	DWORD Category;
} CMDLIST, *PCMDLIST;

typedef struct _MACROBLOCK { 
	CHAR Line[MAX_STRING];
	struct _MACROBLOCK *pNext;
	struct _MACROBLOCK *pPrev;
} MACROBLOCK, *PMACROBLOCK;

typedef struct _ALERT { 
	CHAR Name[MAX_STRING];
	CHAR Class[MAX_STRING];
	CHAR Race[MAX_STRING];
	BYTE Type;
	DOUBLE Radius;
	DOUBLE ZRadius;
	DWORD MinLevel;
	DWORD MaxLevel;
	struct _ALERT *pNext;
} ALERT, *PALERT;

typedef struct _MACROSTACK {
	PMACROBLOCK Location;
	struct _MACROSTACK *pNext;
	LONG Return;
	CHAR StackStr[MAX_STACKVARS][MAX_STRING];
	CHAR LocalStr[MAX_LOCALVARS][MAX_STRING];
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
	DWORD ID;
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

typedef struct _ALIAS {
	struct _ALIAS *pNext;
	CHAR szName[MAX_STRING];
	CHAR szCommand[MAX_STRING];
} ALIAS, *PALIAS;

typedef struct _EVENTSTACK {
	struct _EVENTSTACK *pNext;
	DWORD Type;
	PEVENTLIST pEventList;
	CHAR EventStr[MAX_STACKVARS][MAX_STRING];
} EVENTSTACK, *PEVENTSTACK;

typedef struct _TIMER {
	ULONG Original;
	ULONG Current;
} TIMER, *PTIMER;

typedef struct _FILTER {
	struct _FILTER *pNext;
	CHAR FilterText[MAX_STRING];
	DWORD Length;
	PBOOL pEnabled;
} FILTER, *PFILTER;

typedef struct _PANEL_HSLIDER_ITEMS {
	BYTE Unknown0052[444];
	PITEMINFO *pItem;
	WORD Current;
	WORD Remaining;
} PANEL_HSLIDER_ITEMS, *PPANEL_HSLIDER_ITEMS;

typedef struct _PANEL_HSLIDER_MONEY {
	BYTE Unknown0052[412];
	DWORD *pMax;
	DWORD Unknown0472;
	DWORD Current;
} PANEL_HSLIDER_MONEY, *PPANEL_HSLIDER_MONEY;

typedef struct _PANEL_PACK {
	BYTE Unknown0052[320];
	PITEMINFO pItem;
} PANEL_PACK, *PPANEL_PACK;

typedef struct _PANEL {
	LPVOID lpVtbl;
	DWORD Unknown0004;
	DWORD Unknown0008;
	DWORD Unknown0012;
	DWORD Visible;
	DWORD Unknown0020;
	DWORD Unknown0024;
	DWORD Unknown0028;
	DWORD Unknown0032;
	RECT Area;
	union { // 0x34
		PANEL_PACK Pack;
		PANEL_HSLIDER_ITEMS HSliderItems;
		PANEL_HSLIDER_MONEY HSliderMoney;
	};
} PANEL, *PPANEL;


// ***************************************************************************
// Everquest function typedefs
// ***************************************************************************
typedef DWORD	(__stdcall *fEQWriteChatColor)(THIS_ PCHAR, DWORD, DWORD);
typedef PCHAR	(__stdcall *fEQGetStringByID)(DWORD);
typedef VOID	(__cdecl *fEQCommand)(PSPAWNINFO, PCHAR);
typedef VOID	(__stdcall *fEQClick)(THIS_ DWORD, DWORD);
typedef HRESULT	(__stdcall *fGetDeviceState)(THIS_ DWORD, LPVOID);
typedef DWORD	(__stdcall *fEQScreenItem)(DWORD, DWORD, DWORD);
typedef DWORD	(__stdcall *fEQScreenSpawn)(DWORD, DWORD);
typedef PCHAR   (__stdcall *fEQNewUIINI)(VOID);

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
VOID		DebugSpewAlways			(PCHAR szFormat, ...);
PCHAR		GetNextArg				(PCHAR szLine, DWORD dwNumber = 1);
PCHAR		GetArg					(PCHAR szDest, PCHAR szSrc, DWORD dwNumber, BOOL LeaveQuotes = FALSE, BOOL ToParen = FALSE);
PSPAWNINFO	GetClosestAlert			(PSPAWNINFO pChar,DWORD List, DWORD* pdwCount);
VOID		FreeAlerts				(DWORD List);
DWORD		ConColor				(WORD CharLevel, WORD SpawnLevel);
DWORD WINAPI InsertCommands			(LPVOID lpParameter);
VOID		FailIf					(PSPAWNINFO pChar, PCHAR szCommand, BOOL All=FALSE);
VOID		WriteChatColor			(PCHAR szText, DWORD Color);
VOID		UpdateMonitoredSpawns	(VOID);
PCHAR		GetModel				(PSPAWNINFO pSpawn, DWORD Slot);
PSPAWNINFO	GetPet					(DWORD OwnerID);
BOOL		IfCompare				(PCHAR szCond);

// ***************************************************************************
// Exports
// ***************************************************************************
extern "C" EQLIB_API VOID  SuperWho       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  SuperWhoTarget (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Help           (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Identify       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Location       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Where          (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Skills         (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MacroPause     (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Skills         (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Click          (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  SendKey        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Press          (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Cleanup        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MouseTo        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Macro          (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Face           (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Target         (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  FindItem       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Merchant       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Breakin        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Echo           (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  For            (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Next           (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Call           (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Return         (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  If             (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Goto           (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  EndMacro       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  VarCalc        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  VarSet         (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MyVarAdd       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MyVarSub       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MyVarAnd       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MyVarOr        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  VarRShift      (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  VarLShift      (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MemSpell       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Unload         (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Delay          (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Alert          (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  DoEvents       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  CharInfo       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MacroLog       (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Filter         (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  DebugSpewFile  (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MacroBeep      (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Items          (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  ItemTarget     (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Doors          (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  DoorTarget     (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  ZapVars        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  SetError		  (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  ListMacros	  (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  MQMsgBox		  (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Alias		  (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  DoAbility	  (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  Cast			  (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  ListConnections(PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  HSlider        (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  ZoneDelay	  (PSPAWNINFO, PCHAR);
extern "C" EQLIB_API VOID  TelnetServer_AddToChatQueue(PCHAR);
extern "C" EQLIB_API VOID  InjectEnable   ();
extern "C" EQLIB_API VOID  InjectDisable  ();

#ifdef USE_PERL
extern "C" EQLIB_API VOID  PerlEvalCommand(PSPAWNINFO, PCHAR);

typedef enum _ENGINE_STATES {
	ENGINE_STOPPED,
	ENGINE_INITIALIZING,
	ENGINE_RUNNING,
	ENGINE_SHUTTING_DOWN
} ENGINE_STATES;

void DbgOut(char *szBuffer);
void DbgOutF(char *pszFormat, ...);

class CCritSecLock {
public:
	CCritSecLock(LPCRITICAL_SECTION _pCritSec) { pCritSec = _pCritSec; EnterCriticalSection(pCritSec); }
	~CCritSecLock() { LeaveCriticalSection(pCritSec); }
private:
	LPCRITICAL_SECTION pCritSec;
};
#endif //USE_PERL