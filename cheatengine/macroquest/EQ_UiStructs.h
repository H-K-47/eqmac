/*****************************************************************************
    eqlib.dll: MacroQuest's extension DLL for EverQuest
    Copyright (C) 2002-2003 Plazmic

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License, version 2, as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
******************************************************************************/


// ***************************************************************************
// Structures
// ***************************************************************************

// 10-24-2003 Lax - Massive overhaul.
// See http://macroquest.sourceforge.net/phpBB2/viewtopic.php?t=3569 for
// standards on structures to make things easier on everyone!
// -- ONLY applies to EQ structs, not MQ structs like alerts, etc.


//5-15-2003   eqmule
typedef struct _CXSTR {
/*0x00*/   DWORD   Font;            // maybe, dont know.  04 = Window 01 = button
/*0x04*/   DWORD   MaxLength;
/*0x08*/   DWORD   Length;
/*0x0c*/   BOOL    Encoding; // 0: ASCII, 1:Unicode
/*0x10*/   PCRITICAL_SECTION pLock;
/*0x14*/   CHAR    Text[1]; // Stub, can be anywhere from Length to MaxLength (which is how much is malloc'd to this CXStr)
} CXSTR, *PCXSTR;


typedef struct _EQCURRENTSELECTION {
/*0x00*/   DWORD   Unknown;
/*0x04*/   DWORD   SelectedWnd;//address to selection in tree
/*0x08*/   DWORD   Unknownlodncrap;
/*0x0c*/   DWORD   NumberOfSlots;//merchantslots == 79
/*0x10*/   DWORD   TextureAnim;
/*0x14*/
} EQCURRENTSELECTION, *PEQCURRENTSELECTION;

typedef struct _EQSLOTLIST {
            DWORD   Parent;
            DWORD   InvSlots[372];//0-21 inv 22-30 bags
} EQSLOTLIST, *PEQSLOTLIST;



// Lax 10-24-2003
// Actual Size 0x110 10-9-2003
typedef struct _CSIDLWNDVFTABLE {
/*0000*/ LPVOID IsValid;
/*0004*/ LPVOID  vector_deleting_destructor;
/*0008*/ LPVOID  DrawNC;
/*000C*/ LPVOID  PostDraw;
/*0010*/ LPVOID  PostDraw2;
/*0014*/ LPVOID  DrawCursor;
/*0018*/ LPVOID  OnResize;
/*001C*/ LPVOID  PostDraw3;
/*0020*/ LPVOID  DrawBackground;
/*0024*/ LPVOID  DrawTooltip;
/*0028*/ LPVOID  GetMinimizedRect;
/*002C*/ LPVOID  DrawTitleBar;
/*0030*/ LPVOID  GetCursorToDisplay;
/*0034*/ LPVOID  HandleLButtonDown;
/*0038*/ LPVOID  HandleLButtonUp;
/*003C*/ LPVOID  HandleLButtonHeld;
/*0040*/ LPVOID  HandleLButtonUpAfterHeld;
/*0044*/ LPVOID  HandleRButtonDown;
/*0048*/ LPVOID  HandleRButtonUp;
/*004C*/ LPVOID  HandleRButtonHeld;
/*0050*/ LPVOID  HandleRButtonUpAfterHeld;
/*0054*/ LPVOID  HandleWheelButtonDown;
/*0058*/ LPVOID  HandleWheelButtonUp;
/*005C*/ LPVOID  HandleMouseMove;
/*0060*/ LPVOID  HandleWheelMove;
/*0064*/ LPVOID  HandleKeyboardMsg;
/*0068*/ LPVOID  OnDragDrop;
/*006C*/ LPVOID  GetDragDropCursor;
/*0070*/ LPVOID  QueryDropOK;
/*0074*/ LPVOID  OnClickStick;
/*0078*/ LPVOID  GetDragDropCursor2;
/*007C*/ LPVOID  QueryClickStickDropOK;
/*0080*/ LPVOID  WndNotification;
/*0084*/ LPVOID  ShowWindow;     // Activate   
/*0088*/ LPVOID  Deactivate;
/*008C*/ LPVOID  PostDraw4;
/*0090*/ LPVOID  OnMove;
/*0094*/ LPVOID  OnResize2;
/*0098*/ LPVOID  OnMinimizeBox;
/*009C*/ LPVOID  OnTileBox;
/*00A0*/ LPVOID  PostDraw5;
/*00A4*/ LPVOID  OnMove2;
/*00A8*/ LPVOID  OnMove3;
/*00AC*/ LPVOID  OnProcessFrame;
/*00B0*/ LPVOID  OnVScroll;
/*00B4*/ LPVOID  OnHScroll;
/*00B8*/ LPVOID  OnBroughtToTop;
/*00BC*/ LPVOID  OnMove4;
/*00C0*/ LPVOID  Unknown0x0c0;
/*00C4*/ LPVOID  Unknown0x0c4;
/*00C8*/ LPVOID  Unknown0x0c8;
/*00CC*/ LPVOID  OnMove5;
/*00D0*/ LPVOID  RequestDockInfo;
/*00D4*/ LPVOID  GetTooltip;
/*00D8*/ LPVOID  HitTest;
/*00DC*/ LPVOID  GetHitTestRect;
/*00E0*/ LPVOID  GetInnerRect;
/*00E4*/ LPVOID  GetClientRect;
/*00E8*/ LPVOID  GetMinSize;
/*00EC*/ LPVOID  GetUntileSize;
/*00F0*/ LPVOID  IsPointTransparent;
/*00F4*/ LPVOID  SetDrawTemplate;
/*00F8*/ LPVOID  SetWindowTextA;
/*00FC*/ LPVOID  SetVScrollPos;
/*0100*/ LPVOID  SetAttributesFromSidl;
/*0104*/ LPVOID DefineImages;
/*0108*/ LPVOID LoadIniInfo;
/*010C*/ LPVOID StoreIniInfo;
/*0110*/
} CSIDLWNDVFTABLE, *PCSIDLWNDVFTABLE;

// Lax 10-24-2003
// Actual Size 0x108  10-9-2003
typedef struct _CXWNDVFTABLE {
/*0000*/ LPVOID IsValid;
/*0004*/ LPVOID  vector_deleting_destructor;
/*0008*/ LPVOID  DrawNC;
/*000C*/ LPVOID  PostDraw;
/*0010*/ LPVOID  PostDraw2;
/*0014*/ LPVOID  DrawCursor;
/*0018*/ LPVOID  OnResize;
/*001C*/ LPVOID  PostDraw3;
/*0020*/ LPVOID  DrawBackground;
/*0024*/ LPVOID  DrawTooltip;
/*0028*/ LPVOID  GetMinimizedRect;
/*002C*/ LPVOID  DrawTitleBar;
/*0030*/ LPVOID  GetCursorToDisplay;
/*0034*/ LPVOID  HandleLButtonDown;
/*0038*/ LPVOID  HandleLButtonUp;
/*003C*/ LPVOID  HandleLButtonHeld;
/*0040*/ LPVOID  HandleLButtonUpAfterHeld;
/*0044*/ LPVOID  HandleRButtonDown;
/*0048*/ LPVOID  HandleRButtonUp;
/*004C*/ LPVOID  HandleRButtonHeld;
/*0050*/ LPVOID  HandleRButtonUpAfterHeld;
/*0054*/ LPVOID  HandleWheelButtonDown;
/*0058*/ LPVOID  HandleWheelButtonUp;
/*005C*/ LPVOID  HandleMouseMove;
/*0060*/ LPVOID  HandleWheelMove;
/*0064*/ LPVOID  HandleKeyboardMsg;
/*0068*/ LPVOID  OnDragDrop;
/*006C*/ LPVOID  GetDragDropCursor;
/*0070*/ LPVOID  QueryDropOK;
/*0074*/ LPVOID  OnClickStick;
/*0078*/ LPVOID  GetDragDropCursor2;
/*007C*/ LPVOID  QueryClickStickDropOK;
/*0080*/ LPVOID  WndNotification;
/*0084*/ LPVOID  ShowWindow;        
/*0088*/ LPVOID  Deactivate;
/*008C*/ LPVOID  PostDraw4;
/*0090*/ LPVOID  OnMove;
/*0094*/ LPVOID  OnResize2;
/*0098*/ LPVOID  OnMinimizeBox;
/*009C*/ LPVOID  OnTileBox;
/*00A0*/ LPVOID  PostDraw5;
/*00A4*/ LPVOID  OnMove2;
/*00A8*/ LPVOID  OnMove3;
/*00AC*/ LPVOID  OnProcessFrame;
/*00B0*/ LPVOID  OnVScroll;
/*00B4*/ LPVOID  OnHScroll;
/*00B8*/ LPVOID  OnBroughtToTop;
/*00BC*/ LPVOID  OnMove4;
/*00C0*/ LPVOID  Unknown0x0c0;
/*00C4*/ LPVOID  Unknown0x0c4;
/*00C8*/ LPVOID  Unknown0x0c8;
/*00CC*/ LPVOID  OnMove5;
/*00D0*/ LPVOID  RequestDockInfo;
/*00D4*/ LPVOID  GetTooltip;
/*00D8*/ LPVOID  HitTest;
/*00DC*/ LPVOID  GetHitTestRect;
/*00E0*/ LPVOID  GetInnerRect;
/*00E4*/ LPVOID  GetClientRect;
/*00E8*/ LPVOID  GetMinSize;
/*00EC*/ LPVOID  GetUntileSize;
/*00F0*/ LPVOID  IsPointTransparent;
/*00F4*/ LPVOID  SetDrawTemplate;
/*00F8*/ LPVOID  SetWindowTextA;
/*00FC*/ LPVOID  SetVScrollPos;
/*0100*/ LPVOID  SetAttributesFromSidl;
/*0104*/ LPVOID DefineImages;
/*0108*/
} CXWNDVFTABLE, *PCXWNDVFTABLE;

// Lax 10-24-2003
// Actual size 0x138, 10-9-2003
typedef struct _CXWND {
/*0x000*/   struct _CXWNDVFTABLE   *pvfTable;
/*0x004*/   DWORD   Unknown0x004;   // set to 0 in CXWnd::Refade
/*0x008*/   DWORD   Unknown0x008;
/*0x00c*/   DWORD   TimeMouseOver;         //how many ms the window has had the mouse over it...
/*0x010*/   DWORD   Unknown0x00c;      //
/*0x014*/   BYTE    Unknown0x010;      // set to 1 in CXWnd::Refade
/*0x015*/   BYTE    Unknown0x011;     // Faded?
/*0x016*/   BYTE    Locked; // -> 0x16
/*0x017*/   BYTE    Unknown0x013;
/*0x018*/   BYTE    Unknown0x014;
/*0x019*/   BYTE    Unknown0x015;
/*0x01a*/   BYTE    Unknown0x016;
/*0x01b*/   BYTE    Unknown0x017;
/*0x01c*/   BYTE    Unknown0x018[0x4];
/*0x020*/   struct  _CSIDLWND *pParentWindow;  // If this is NULL, coordinates are absolute...
/*0x024*/   struct  _CSIDLWND *pChildren;
/*0x028*/   struct  _CSIDLWND *pSiblings;  // its a tree.  // ->0x28
/*0x02c*/   BYTE    HasChildren;
/*0x02d*/   BYTE    HasSiblings; // ->0x2d
/*0x02e*/   BYTE    Unknown0x02a[0x2];
/*0x030*/   DWORD   Flags4;
/*0x034*/   DWORD   Xleft;          // left coordinate of the bounding box, relative to parent
/*0x038*/   DWORD   Ytop;          // top coordinate of the bounding box, relative to parent
/*0x03c*/   DWORD   Xright;       // right coordinate of the bounding box, relative to parent
/*0x040*/   DWORD   Ybottom;       // bottom coordinate of the bounding box, relative to parent
/*0x044*/   DWORD   Unknown0x40;   // screen left?
/*0x048*/   DWORD   Unknown0x44;   // screen top?
/*0x04c*/   DWORD   Unknown0x48;   // screen right?
/*0x050*/   DWORD   Unknown0x4c;   // screen bottom?
/*0x054*/   BYTE    Show;       // -> 0x54
/*0x055*/   BYTE    Enabled;
/*0x056*/   BYTE    Minimized;  // -> 0x56
/*0x057*/   BYTE    Unknown0x053; // ontilebox
/*0x058*/   BYTE    Open;
/*0x059*/   BYTE    Unknown0x055;
/*0x05a*/   BYTE    MouseOver;
/*0x05b*/   BYTE    Unknown0x057;
/*0x05c*/   DWORD   WindowStyle; // bit 1 - vertical scroll, bit 2 - horizontal scroll, bit 4 - title bar?, bit 8 - border
/*0x060*/   DWORD   Unknown0x05c;
/*0x064*/   struct _CXSTR  *WindowText;
/*0x068*/   struct _CXSTR  *Tooltip;
/*0x06c*/   DWORD   Unknown0x068; // CXWnd::SetLookLikeParent
/*0x070*/   DWORD   Unknown0x06c; // CXWnd::SetLookLikeParent
/*0x074*/   BYTE    Unknown0x070[0x14];
/*0x088*/   struct _CXSTR  *XMLToolTip;
/*0x08c*/   BYTE    Unknown0x088[0x18];
/*0x0a4*/   DWORD   ZLayer;
/*0x0a8*/   DWORD   Unknown0x0a4;
/*0x0ac*/   LPVOID  DrawTemplate;
/*0x0b0*/   BYTE    Unknown0x0ac[0x1c];
/*0x0cc*/   DWORD   Unknown0x0c8; // CXWnd::StartFade, CXWnd::Minimize
/*0x0d0*/   BYTE    Unknown0x0cc[0x10];
/*0x0e0*/   DWORD   FadeTickCount;
/*0x0e4*/   BYTE    Unknown0x0e0; // CXWnd::StartFade
/*0x0e5*/   BYTE    Unknown0x0e1; // CXWnd::StartFade
/*0x0e6*/   BYTE    Unknown0x0e2;
/*0x0e7*/   BYTE    Unknown0x0e3;
/*0x0e8*/   DWORD   Unknown0x0e4;// CXWnd::StartFade, CXWnd::Minimize
/*0x0ec*/   DWORD   VScrollMax; // -> 0xec
/*0x0f0*/   DWORD   VScrollPos; // -> 0xf0
/*0x0f4*/   DWORD   HScrollMax;
/*0x0f8*/   DWORD   HScrollPos;
/*0x0fc*/   BYTE    ValidCXWnd;
/*0x0fd*/   BYTE    Unused0x0f9[0x7];
/*0x104*/
} CXWND, *PCXWND;


// Lax 10-24-2003
// Actual size 0x138, 10-9-2003
typedef struct _CSIDLWND {
/*0x000*/   struct _CSIDLWNDVFTABLE   *pvfTable;
/*0x004*/   DWORD   Unknown0x004;   // set to 0 in CXWnd::Refade
/*0x008*/   DWORD   Unknown0x008;
/*0x00c*/   DWORD   TimeMouseOver;         //how many ms the window has had the mouse over it...
/*0x010*/   DWORD   Unknown0x010;      // 
/*0x014*/   BYTE    Unknown0x014;      // set to 1 in CXWnd::Refade
/*0x015*/   BYTE    Unknown0x015;     // Faded?
/*0x016*/   BYTE    Locked; // -> 0x16
/*0x017*/   BYTE    Unknown0x017;
/*0x018*/   BYTE    Unknown0x018;
/*0x019*/   BYTE    Unknown0x019;
/*0x01a*/   BYTE    Unknown0x01a;
/*0x01b*/   BYTE    Unknown0x01b;
/*0x01c*/   BYTE    Unknown0x01c[0x4];
/*0x020*/   struct  _CSIDLWND *pParentWindow;  // If this is NULL, coordinates are absolute...
/*0x024*/   struct  _CSIDLWND *pChildren;
/*0x028*/   struct  _CSIDLWND *pSiblings;  // its a tree.  // ->0x28
/*0x02c*/   BYTE    HasChildren;
/*0x02d*/   BYTE    HasSiblings; // ->0x2d
/*0x02e*/   BYTE    Unknown0x02e[0x2];
/*0x030*/   DWORD   Flags4;
/*0x034*/   RECT    Location;
/*0x044*/   DWORD   Unknown0x44;   // screen left? 
/*0x048*/   DWORD   Unknown0x48;   // screen top?
/*0x04c*/   DWORD   Unknown0x4c;   // screen right?
/*0x050*/   DWORD   Unknown0x50;   // screen bottom?
/*0x054*/   BYTE    Show;       // -> 0x54
/*0x055*/   BYTE    Enabled;
/*0x056*/   BYTE    Minimized;  // -> 0x56
/*0x057*/   BYTE    Unknown0x057; // ontilebox
/*0x058*/   BYTE    Open;
/*0x059*/   BYTE    Unknown0x059;
/*0x05a*/   BYTE    MouseOver;
/*0x05b*/   BYTE    Unknown0x05b;
/*0x05c*/   DWORD   WindowStyle; // bit 1 - vertical scroll, bit 2 - horizontal scroll, bit 4 - title bar?, bit 8 - border
/*0x060*/   DWORD   Unknown0x060;
/*0x064*/   struct _CXSTR  *WindowText;
/*0x068*/   struct _CXSTR  *Tooltip;
/*0x06c*/   DWORD   Unknown0x06c; // CXWnd::SetLookLikeParent
/*0x070*/   DWORD   Unknown0x070; // CXWnd::SetLookLikeParent
/*0x074*/   BYTE    Unknown0x074[0x14];
/*0x088*/   struct _CXSTR  *XMLToolTip;
/*0x08c*/   BYTE    Unknown0x08c[0x18];
/*0x0a4*/   DWORD   ZLayer;
/*0x0a8*/   DWORD   Unknown0x0a8;
/*0x0ac*/   LPVOID  DrawTemplate;
/*0x0b0*/   BYTE    Unknown0x0b0[0xc];
/*0x0bc*/   union {
					_ITEMINFO* Item;
					_CSIDLWND* InvDescription;
			};
/*0x0c0*/   BYTE    Unknown0x0c0[0xc];
/*0x0cc*/   DWORD   Unknown0x0cc; // CXWnd::StartFade, CXWnd::Minimize
/*0x0d0*/   BYTE    Unknown0x0d0[0x10];
/*0x0e0*/   DWORD   FadeTickCount;
/*0x0e4*/   BYTE    Unknown0x0e4; // CXWnd::StartFade
/*0x0e5*/   BYTE    Unknown0x0e5; // CXWnd::StartFade
/*0x0e6*/   BYTE    Unknown0x0e6; 
/*0x0e7*/   BYTE    Unknown0x0e7;
/*0x0e8*/   DWORD   Unknown0x0e8;// CXWnd::StartFade, CXWnd::Minimize
/*0x0ec*/   DWORD   VScrollMax; // -> 0xec
/*0x0f0*/   DWORD   VScrollPos; // -> 0xf0
/*0x0f4*/   DWORD   HScrollMax;
/*0x0f8*/   DWORD   HScrollPos;
/*0x0fc*/   BYTE    ValidCXWnd;
/*0x0fd*/   BYTE    Unused0x0f9[0x7];
/*0x104*/   union {
					struct _CXSTR  *SidlScreen; // --> 0x104
					DWORD   SlotID;
			};
/*0x108*/   LPVOID SidlPiece;   // CScreenPieceTemplate (important)  --> 0x108
/*0x10c*/   DWORD   Checked; // CRadioGroup
/*0x110*/   DWORD   TextureAnim; // used in CSidlScreenWnd::AddButtonToRadioGroup
/*0x114*/   DWORD   Unknown0x114;
/*0x118*/   DWORD   Selector;
/*0x11c*/   DWORD   PushToSelector;
/*0x120*/   BOOL   EnableINIStorage;
/*0x124*/   struct _CXSTR *INIStorageName;
/*0x128*/   DWORD   Unknown0x128;
/*0x12c*/   DWORD   Unknown0x12c;
/*0x130*/   BYTE    Unknown0x130[0x8];
/*0x138*/
} CSIDLWND, *PCSIDLWND;


//5-15-2003   eqmule
//10-30-2003 plazmic - converted to CSIDLWND
typedef struct _EQMERCHWINDOW {
/*0x000*/   struct _CSIDLWND Wnd;
/*0x138*/   BYTE Unknown0x138[0x10];
/*0x148*/   PCONTENTS   ItemDesc[0x50];   //the mainwindow has pointers directly to the items in the slots...
/*0x288*/   DWORD   Unknown5;
/*0x28c*/   DWORD   SelectedSlotID;
/*0x290*/   DWORD   AddressToPointerForSelectedItem;//
/*0x294*/   DWORD   Unknown6;
/*0x298*/   DWORD   ItemIconwnd;
/*0x29c*/   DWORD   Unknown7;
/*0x2a0*/   DWORD   SellBuyButton;
/*0x2a4*/   struct _CSIDLWND   *SlotsHandles[0x50];
/*0x3e4*/   DWORD   DoneButton;
/*0x3e8*/   DWORD   WindowFace;
/*0x3ec*/
} EQMERCHWINDOW, *PEQMERCHWINDOW;


// 10-27-2003 Lax
typedef struct _EQCHATMGR
{
/*0x000*/ struct _EQCHATWINDOW* ChatWnd[0x20];
/*0x080*/ DWORD NumWindows;
/*0x084*/ DWORD ActiveWindow; // CChatManager::GetActiveChatWindow
/*0x088*/ DWORD LockedWindow; // CChatManager::GetActiveChatWindow
/*0x08c*/ DWORD Unknown0x08c;
/*0x090*/ struct _EQCHATWINDOW* ChannelMap[0x20];   // channel map
/*0x110*/ BYTE  Unknown0x110[0x28];
/*0x138*/ DWORD Unknown0x138;
/*0x13c*/ DWORD Unknown0x13c;
/*0x140*/ DWORD Unknown0x140;
/*0x144*/ LPVOID ChatMenu;
/*0x148*/ DWORD Unknown0x148;
/*0x14c*/ DWORD Unknown0x14c;
/*0x150*/ DWORD Unknown0x150;
/*0x154*/ DWORD Unknown0x154;
/*0x158*/ LPVOID MeleeMenu;
/*0x15c*/ DWORD Unknown0x15c;
/*0x160*/ LPVOID SpellMenu;
/*0x164*/ DWORD Unknown0x164;
/*0x168*/ DWORD Unknown0x168;
/*0x16c*/ LPVOID ChannelMenu;
/*0x170*/ LPVOID ChatMenu2;
/*0x174*/ LPVOID FilterMenu;
/*0x178*/ DWORD Unknown0x178;
/*0x17c*/ DWORD Unknown0x17c;
/*0x180*/ DWORD Unknown0x180;
/*0x184*/ LPVOID HitsModeMenu;
/*0x188*/ LPVOID MeleeMenu2;
/*0x18c*/ LPVOID HitsModeMenu2;
/*0x190*/ DWORD Unknown0x190;
/*0x194*/ LPVOID HitsModeMenu3;
/*0x198*/ DWORD Unknown0x198;
/*0x19c*/ LPVOID HitsModeMenu4;
/*0x1a0*/ DWORD Unknown0x1a0;
/*0x1a4*/ LPVOID HitsModeMenu5;
/*0x1a8*/ DWORD Unknown0x1a8;
/*0x1ac*/ LPVOID HitsModeMenu6;
/*0x1b0*/ DWORD Unknown0x1b0;
/*0x1b4*/ DWORD Unknown0x1b4; // CChatManager__UpdateTellMenus
/*0x1b8*/ DWORD Unknown0x1b8;
/*0x1bc*/ DWORD Unknown0x1bc; // CChatManager__UpdateTellMenus
/*0x1c0*/ DWORD Unknown0x1c0;
/*0x1c4*/ DWORD Unknown0x1c4; // CChatManager__UpdateTellMenus // raid
/*0x1c8*/ DWORD Unknown0x1c8;
/*0x1cc*/ DWORD Unknown0x1cc; // CChatManager__UpdateTellMenus
/*0x1d0*/ DWORD Unknown0x1d0; // CChatManager__UpdateTellMenus
/*0x1d4*/ DWORD Unknown0x1d4; // CChatManager__UpdateTellMenus
/*0x1d8*/ DWORD Unknown0x1d8;
/*0x1dc*/ DWORD Unknown0x1dc;
/*0x1e0*/ DWORD Unknown0x1e0;
/*0x1e4*/ BYTE  Unknown0x1e4[0x1c];
/*0x200*/ DWORD Unknown0x200;
/*0x204*/ DWORD Unknown0x204;
/*0x208*/
} EQCHATMGR, *PEQCHATMGR;

// Actual size 0x248 10-9-2003
// 10-27-2003 Lax
typedef struct _EQCHATWINDOW
{
/*0x000*/ struct _CSIDLWND Wnd;
/*0x138*/ DWORD  Unknown0x138;
/*0x13c*/ LPVOID ChatManager; // --> 0x13c
/*0x140*/ struct _CSIDLWND* InputWnd;
/*0x144*/ struct _CSIDLWND* OutputWnd;
/*0x148*/ DWORD Unknown0x140;
/*0x14c*/ DWORD Unknown0x144;
/*0x150*/ BYTE Unknown0x148;
/*0x151*/ BYTE Unknown0x149[0x3f];
/*0x18c*/ DWORD Unknown0x188;
/*0x190*/ DWORD Unknown0x18c;
/*0x194*/ struct _CXSTR *CommandHistory[0x28]; // ->0x198
/*0x238*/ DWORD Unknown0x230; // CChatWindow::HistoryBack/forward .. maybe total or current history lines
/*0x23c*/ DWORD Unknown0x234; // CChatWindow::HistoryBack/forward .. maybe total or current history lines
/*0x240*/ DWORD FontSize; //
/*0x244*/ DWORD Unknown0x23C;
/*0x248*/
} EQCHATWINDOW, *PEQCHATWINDOW;

// Actual size 0x1dc old
// Lax 10-24-2003
typedef struct _EQBUFFWINDOW
{
/*0x000*/ struct _CSIDLWND Wnd;
/*0x138*/ BYTE  Unknown0x138[0x4c];
/*0x184*/ DWORD pBuff[0x0f];    // CButton*
/*0x1C0*/ DWORD Unknown0x1C0;
/*0x1C4*/ DWORD Unknown0x1C4;
/*0x1C8*/ DWORD Unknown0x1C8;
/*0x1CC*/ DWORD Unknown0x1CC;
/*0x1D0*/ DWORD FirstEffectSlot; //0-15
/*0x1D4*/ DWORD LastEffectSlot;  //0-15
/*0x1D8*/ DWORD MaxBuffs;  // either 0x0F or 0x06  (normal or shortbuffs)
/*0x1DC*/
} EQBUFFWINDOW, *PEQBUFFWINDOW;

// onetimehero 09-17-03
// ContainerWindow
// Actual Size 0x17C old
typedef struct _EQCONTAINERWINDOW {
/*0x000*/ struct _CSIDLWND Wnd;
/*0x138*/ struct _CONTENTS*   pContents;     // Pointer to the contents of the container;
                                                 // Matches the pointer in CHARINFO.Inventory/Bank/World
/*0x13c*/ struct _CSIDLWND*   pSlots[0x0a];
/*0x164*/ struct _CSIDLWND*   pCombine;
/*0x168*/ struct _CSIDLWND*   pDone;
/*0x16c*/ struct _CSIDLWND*   pIcon;
/*0x170*/ struct _CSIDLWND*   pUnknown;
/*0x174*/ struct _CSIDLWND*   pLabel;
/*0x178*/ BYTE Unknown0x178[4];
/*0x17c*/
} EQCONTAINERWINDOW, *PEQCONTAINERWINDOW;

// Actual Size 0x74  old
typedef struct _EQ_CONTAINERWND_MANAGER {
/*0x000*/   DWORD pvfTable; // NOT based on CXWnd.  Contains only destructor
/*0x004*/   PEQCONTAINERWINDOW pPCContainers[0x19];  // All open containers, including World, in order of opening...
/*0x068*/   PCONTENTS   pWorldContents;            // Pointer to the contents of the world container.  If NULL, world container isn't open;
/*0x06c*/   DWORD dwWorldContainerID;            // ID of container in zone, starts at one (zero?) and goes up.
/*0x070*/   DWORD dwTimeSpentWithWorldContainerOpen;  // Cumulative counter?
/*0x074*/
} EQ_CONTAINERWND_MANAGER, *PEQ_CONTAINERWND_MANAGER;

typedef struct _POINT3 {
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
} POINT3, *PPOINT3;

// plazmic 08-17-03
// Map Window
typedef struct _MAPLABEL { // sizeof() = 0x34
    struct _MAPLABEL *pNext;
    struct _MAPLABEL *pPrev;
    POINT3 Location;
    ARGBCOLOR Color;
    DWORD Size; //1-3;
    PCHAR Label;
    DWORD Layer;    //0-3;
    DWORD Width;
    DWORD Height;
    DWORD unk_0x2c;
    DWORD unk_0x30;
} MAPLABEL, *PMAPLABEL;

typedef struct _MAPLINE { // sizeof() = 0x28
    struct _MAPLINE *pNext;
    struct _MAPLINE *pPrev;
    POINT3 Start;
    POINT3 End;
    ARGBCOLOR Color;
    DWORD Layer;    //0-3;
} MAPLINE, *PMAPLINE;

typedef struct _EQLOOTWINDOW {
/*0x000*/ struct _CSIDLWND Wnd;
/*0x138*/ BYTE  Unknown0x138[0x0c];
/*0x144*/ BYTE  Unknown0x144[0x80];
/*0x1c4*/ PCONTENTS   ItemDesc[0x1e]; //there can only be 30 items on a corpse since that equals 22 inv slots plus 8 bags...
/*0x23c*/
} EQLOOTWINDOW, *PEQLOOTWINDOW;

typedef struct _EQNOTESWINDOW {
/*0x000*/ struct _CSIDLWND Wnd;
/*0x138*/ BYTE  Unknown0x138[0x08];
/*0x140*/ struct _CSIDLWND *pEditWnd;
} EQNOTESWINDOW, *PEQNOTESWINDOW;

// Lax 10-29-2003 
// Actual size 0x16C 
typedef struct _EQITEMWINDOW 
{ 
/*0x000*/ struct _CSIDLWND Wnd; 
/*0x138*/ BYTE Unknown0x138[0x4]; 
/*0x13c*/ struct _CSIDLWND *DisplayWnd; 
/*0x140*/ BYTE Unknown0x140[0x8]; 
/*0x148*/ BYTE Unknown0x148; 
/*0x149*/ BYTE Unknown0x149[0x3]; 
/*0x14C*/ PCXSTR ItemInfo; 
/*0x150*/ PVOID Unknown0x150; // PCXSTR "item display" ??    _CSIDLWND ?? 
/*0x154*/ DWORD Unknown0x154; // possibly PCXSTR of information as on charms 
/*0x158*/ BYTE  Unknown0x158; 
/*0x15C*/ PVOID TextureAnim; 
/*0x160*/ PVOID TextureAnim2; 
/*0x164*/ DWORD Unknown0x164[2]; 
/*0x16C*/ 
} EQITEMWINDOW, *PEQITEMWINDOW; 

// Actual Size 0x22c 10-9-2003
typedef struct _EQMAPWINDOW {
/*0x000*/ struct _CSIDLWND Wnd;
/*0x138*/ BYTE Unknown0x138[0x28];
/*0x160*/ CHAR shortzonename[0x20];
/*0x180*/ BYTE Unknown0x180[0x70];
/*0x1F0*/ PMAPLINE pLines;
/*0x1F4*/ PMAPLABEL pLabels;
/*0x1F8*/ BYTE Unknown0x1f8[0x34];
/*0x22c*/
} EQMAPWINDOW, *PEQMAPWINDOW;




// onetimehero 09-17-03
// Spell Window
typedef struct _EQCASTSPELLWINDOW {
/*0x000*/ struct _CSIDLWND Wnd;
/*0x138*/ BYTE Unknown0x134[0x0c];
/*0x144*/  struct _EQCASTSPELLGEM   *SpellSlots[0x8];
/*0x164*/  BYTE    Unknown0x160[0x8];
/*0x16c*/  DWORD   spellicon;//if this is equal to FFFFFFFF there is no spell memmed in this slot...
/*0x170*/  DWORD   spellstate;// 1 = cast in progress or refreshtime not met 2 means we ducked or aborted cast, 0 means its ok to cast
/*0x174*/
} EQCASTSPELLWINDOW, *PEQCASTSPELLWINDOW;

// onetimehero 09-17-03
// Individual Gems
// Actual size 0x188 10-9-2003
typedef struct _EQCASTSPELLGEM {
/*0x000*/ struct _CSIDLWND Wnd;
/*0x138*/ BYTE Unknown0x138[0x0c];
/*0x144*/         BYTE   Unknown0x144[0x20];
/*0x164*/         BYTE   Unknown0x164[0x8];
/*0x16c*/         DWORD   spellicon;//if this is equal to FFFFFFFF there is no spell memmed in this slot...
/*0x170*/         DWORD   spellstate;// 1 = cast in progress or refreshtime not met 2 means we ducked or aborted cast, 0 means its ok to cast
/*0x174*/
} EQCASTSPELLGEM, *PEQCASTSPELLGEM;

// Actual size 0x1c4 10-9-2003
typedef struct _EQHOTBUTTONWINDOW {
/*0x000*/ struct _CSIDLWND Wnd;
/*0x138*/ BYTE Unknown0x138[0x10];
/*0x148*/ DWORD   HotButtonWndView;//0 to 9 for the different views
/*0x14c*/ struct _CSIDLWND   *HotButtons[0x0a];//these will change when you switch page...
/*0x174*/
} EQHOTBUTTONWINDOW, *PEQHOTBUTTONWINDOW;

typedef struct _LOOTCORPSE {
/*0x000*/   BYTE    Unknown0x000;   // 03 seems very common (for NPC anyway)
/*0x001*/   CHAR    Name[0x4b];
/*0x04c*/   FLOAT   Timer;  //Uncertain the equation used here, but fairly certain it's the timer
/*0x050*/   FLOAT   X;
/*0x054*/   BYTE    Unknown0x054[0x4];
/*0x058*/   FLOAT   Y;
/*0x05c*/   FLOAT   Z;
/*0x060*/   BYTE    Unknown0x060[0x24];
/*0x084*/   DWORD   Flags;
/*0x088*/   BYTE    Unknown0x088[0x4];
/*0x08c*/   DWORD   Unknown0x08c;   //256 seems very common
/*0x090*/
// I think there might be pointers at 0x0e0..but no clue what to
} LOOTCORPSE, *PLOOTCORPSE;

typedef struct _SPELLFAVORITE {
/*0x000*/   DWORD   SpellId[8];
/*0x020*/   CHAR    Name[25];
/*0x039*/   BYTE    Byte_39;
/*0x03A*/   BYTE    Byte_3A;
/*0x03B*/   BYTE    Byte_3B;
/*0x03C*/
} SPELLFAVORITE, *PSPELLFAVORITE;
