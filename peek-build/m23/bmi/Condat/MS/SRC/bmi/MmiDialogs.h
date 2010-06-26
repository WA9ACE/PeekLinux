//#error MmiDialogs includiert
#ifndef _DEF_MMI_DIALOGS_H_
#define _DEF_MMI_DIALOGS_H_
/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   MmiDialogs
 $File:       MmiDialogs.h
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/10/00

********************************************************************************

 Description:

  MMI Information dialogue handling.


********************************************************************************
 $History: MmiDialogs.h

	Apr 03, 2007    ER: OMAPS00122561  x0pleela
	Description:   [ACI] Phone lock feature has to be supported by ACI
	Solution: Phone Lock ER implementation

	Mar 11, 2006   REF:DR:OMAPS00061467  x0035544
	Description:   27.22.4.13.3 SET UP CALL (display of icons) fails.
	Solution: added the new structure T_SAT_IconData and defined its variable at the structures T_dialog_info
	and T_DISPLAY_DATA.

 	Feb 24, 2005 REF: CRR MMI-SPR-29102 xnkulkar
       Description: The phone resets when user presses the ''End All' key while the SMSs are being deleted. 
       Solution: As the user has already confirmed that all SMS messages are to be deleted, ''End All' key 
       		press is ignored till the delete operation is over.
          xrashmic 08 Feb, 2005 MMI-SPR-27853
        Moved the T_dialog_info structure from MmiDialogs.c to MmiDialogs.h
              

        xrashmic 08 Feb, 2005 MMI-SPR-27853
        Moved the T_dialog_info structure from MmiDialogs.c to MmiDialogs.h
              
	 Jun 04,2004 CRR:13601 xpradipg - SASKEN
	 Description: CFO interrogation display : Timer to long
	 Fix: Timer is enabled for the information dialog. It times out after 5 seconds from the last key
	        press.
  25/10/00      Original Condat(UK) BMI version.

 $End

*******************************************************************************/
#include "mfw_mfw.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#else /* NEW_EDITOR */
#include "mfw_edt.h"
#endif /* NEW_EDITOR */
#include "mfw_icn.h"

#include "MmiMmi.h" //for 'DialogsDummy'
#include "Mmiicons.h"

//#include "font_bitmaps.h"	//For t_font_bitmap definition

#define TIMER_EXIT  0x0BB8             /* Default SMS timer period - 3 secs.  */
                                       /* Information dialogue display times. */
#define ONE_SECS  0x03E8        /* 1000 milliseconds.     */
#define TWO_SECS  0x07D0        /* 2000 milliseconds.     */
#define THREE_SECS  0x0BB8             /* 3000 milliseconds.                  */
#define FOUR_SECS  (4000)
#define FIVE_SECS   0x1388             /* 5000 milliseconds.                  */
#define SIX_SECS  	(6000)
#define TEN_SECS    0x2710             /* 10000 milliseconds.                 */
#define TWO_MIN     0x1D4C0            /* 2 min                               */
#define FOREVER     0xFFFF             /* Infinite time period.               */

#define SMS_PROMPTX      5
#define SMS_PROMPTY     45
#define PROMPTX          7
#define PROMPTY         17
#define INFO_TEXTX      6
#define INFO_TEXTY      15
#define INFO_TEXTY2     26
#define INFO_TEXTY3     37
#define FRAMEX1          4
#define FRAMEY1         14
#define FRAMEX2         92
#define FRAMEY2         52

#define DLG_BORDER_VERTICAL_0		0x0000		//leave no border
#define DLG_BORDER_VERTICAL_1		0x0001		//leave 25% of bitmap to the left/right unused 
#define DLG_BORDER_VERTICAL_2		0x0002
#define DLG_BORDER_VERTICAL_3		0x0003
#define DLG_BORDER_VERTICAL			0x0003


#define DLG_BORDER_HORIZONTAL_0		0x0000
#define DLG_BORDER_HORIZONTAL_1		0x0004	//leave 25% of bitmap to the top/bottom unused
#define DLG_BORDER_HORIZONTAL_2		0x0008
#define DLG_BORDER_HORIZONTAL_3		0x000C
#define DLG_BORDER_HORIZONTAL		0x000C

#define DLG_INFO_LAYOUT_BOTTOM		0x0010	//display data on bottom of bitmap
#define DLG_INFO_LAYOUT_TOP			0x0020	//display data at top of bitmap
#define DLG_INFO_LAYOUT_LEFT		0x0040
#define DLG_INFO_LAYOUT_RIGHT		0x0080
#define DLG_INFO_LAYOUT_CENTRE		0x0000
#define DLG_INFO_LAYOUT_FILL		0x00F0	//move data to edges of available area


/*
 * common template definitions
 */
typedef void (*T_DIALOG_FUNC)(T_MFW_HND win, USHORT event, USHORT value, void * parameter);


typedef struct
{
  T_DIALOG_FUNC  dialog;
  void         * data;
} T_MMI_CONTROL;

#define ALLOC_MEMORY mfwAlloc
#define FREE_MEMORY  mfwFree

extern  void mmi_send_event (T_MFW_HND win, USHORT event, USHORT value, void * parameter);

#define SEND_EVENT(w,e,v,p) mmi_send_event(w,e,v,p)

/*
 * End of common template definitions
 */

 /*
 * Definitions from common dialog function
 */

//Berlin_UK int
/* kk 011102
  typedef void   (*T_VOID_FUNC)();
  typedef short  (*T_SHORT_FUNC)();
*/
//Berlin_UK int
//GW-SPR#762 wrap definitions
#define WRAP_STRING_1 (0x01)
#define WRAP_STRING_2 (0x02)

#define DLG_TYPE_NORMAL		0x01
#define DLG_TYPE_POPUP		0x02
//#define T_BITMAP t_font_bitmap - defined in Mmiicons.h

//x0035544 Feb 07, 2006 DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON
typedef struct
{
						/* SAT ICON DATA   */
	U8 width;				/* SAT Icon width  */
	U8 height;				/* SAT Icon height */
	char* dst;                           /* Icon data	   */
	BOOL selfExplanatory;
} T_SAT_IconData;
#endif

typedef struct
{
  char *       TextString;
  BOOL			SoftKeyStrings;		// SH - TRUE if we are providing strings for softkeys rather than IDs
  USHORT       LeftSoftKey;
  USHORT       RightSoftKey;
  char			*LSKString;			// SH - Text for customised softkey
  char			*RSKString;			// SH - Text for customised softkey
  ULONG        Time;
  T_MFW_EVENT  KeyEvents;
  USHORT       TextId;
  USHORT       Identifier;
  char *       TextString2;
  USHORT       TextId2;
  T_VOID_FUNC  Callback;
  UBYTE			WrapStrings;	// GW-SPR#762 - bits 0,1 set=wrap string 1,2
  //GW Added colour definition
  U32 			displayCol;

  int			dlgType;	//Type of popup
  T_BITMAP		*bgdBitmap; //Bitmap for background image
  MfwRect		*dlgPos;	//Position of dialog
  MfwIcnAttr*	icon_array;
  int			current_icon;
  UBYTE			number_of_frames;
  UBYTE			current_frame;	
  T_MFW_HND		animation_timer;
  int			anim_time;
 //x0035544 Feb 07, 2006 DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON
  T_SAT_IconData       IconData; //Sat icon information
#endif
} T_DISPLAY_DATA;

//xrashmic 08 Feb, 2005 MMI-SPR-27853
//Moved the sturcture from mmidialog.c to mmidialogs.h, need this structure to update the 
// dialog screen with the send progress information
typedef struct
{
  T_MMI_CONTROL   mmi_control;
  T_MFW_HND       parent_win;
  T_MFW_HND       info_win;
  T_MFW_HND       info_tim;
  T_MFW_HND       info_kbd;
  T_MFW_HND       info_kbd_long;
  T_MFW_HND       icon;  
	/*
   * animation fields
   */
  UBYTE			   number_of_frames;
  UBYTE			   current_frame;
  T_MFW_HND		   anim_time;
  MfwIcnAttr        *icon_array;  
  MfwIcnAttr        *current_icon;  
  T_MFW_HND       animation_timer;  

	/*
   * internal data
   */
  char          *TextString;
  USHORT		LeftSoftKey;
  USHORT		RightSoftKey;
  char			*LSKString;		// SH - text for left soft key
  char			*RSKString;		// SH - text for right soft key
  BOOL			SoftKeyStrings;		// SH - TRUE if using text rather than IDs for soft keys
  UBYTE			WrapStrings;	// GW - bits 0,1 set=wrap string 1,2
  ULONG        Time;
  T_MFW_EVENT  KeyEvents;
  USHORT       TextId;
  USHORT       Identifier;
  char *        TextString2;
  USHORT       TextId2;
  T_VOID_FUNC  Callback;

  U32		dlgCol;
  U32		dlgType;
  T_BITMAP	*dlgBgdBitmap;
  int		dlgSkFormat;
 //x0035544 Feb 07, 2006 DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON
  T_SAT_IconData IconData; //Sat icon information
#endif
} T_dialog_info;

typedef struct
{
  USHORT       TitleTextId;
  char *       TextString;
#ifdef NEW_EDITOR	/* SPR#1428 - SH - New Editor changes */
  T_ED_ATTR		EditorAttrScroll;
#else /* NEW_EDITOR */
  MfwEdtAttr   	EdtAttrScroll;
#endif /* NEW_EDITOR */
  USHORT       LeftSoftKey;
  USHORT       RightSoftKey;
  ULONG        Time;
  T_MFW_EVENT  KeyEvents;
  USHORT       Identifier;
  T_VOID_FUNC  Callback;
} T_SCROLL_DISPLAY_DATA;
/*
 * Function expected from Info Dialog
 */
#define INFO_TIMEOUT   1
#define INFO_KCD_LEFT  2
#define INFO_KCD_RIGHT 3
#define INFO_KCD_HUP   4
#define INFO_KCD_UP    5
#define INFO_KCD_DOWN  6
#define INFO_KCD_0_9   7
#define INFO_KCD_ALL   8
#define INFO_KCD_CLEAR 9
#define INFO_KCD_OFFHOOK 10
#define INFO_KCD_CALL 11

#define PROMPT_LENGTH    16

//x0pleela 02 Apr, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK

T_MFW_HND phlock_info_dialog (T_MFW_HND        parent_win,T_DISPLAY_DATA * display_info);
#endif //FF_PHONE_LOCK

extern  T_MFW_HND info_dialog (T_MFW_HND        parent_win,
                               T_DISPLAY_DATA * display_info);

T_MFW_HND info_dialog_softkeystrings (T_MFW_HND parent_win, T_DISPLAY_DATA * display_info);

extern T_MFW_HND mmi_dialog_information_screen(T_MFW_HND parent_win, int TxtId,char* text,
          T_VOID_FUNC call_back, USHORT identifier);
extern T_MFW_HND mmi_dialog_information_screen_forever(MfwHnd win,int TextId, char* TextStr, int colour);

void dlg_zeroDisplayData( T_DISPLAY_DATA *displayInfo);
void dlg_initDisplayData_TextId( T_DISPLAY_DATA *displayInfo, int lsk, int rsk, int str1, int str2, int colId);
void dlg_initDisplayData_TextStr( T_DISPLAY_DATA *displayInfo, int lsk, int rsk,char *str1, char *str2, int colId);
void dlg_initDisplayData_events( T_DISPLAY_DATA *displayInfo, T_VOID_FUNC callback, int timer, T_MFW_EVENT keyEvents);

/*GB*/
void mmi_dialogs_insert_icon (T_MFW_HND win_dialog ,MfwIcnAttr *icon);
void mmi_dialogs_insert_animation (T_MFW_HND win_dialog ,S32 anim_time ,MfwIcnAttr *icon ,UBYTE number_of_frames);

void mmi_dialogs_insert_animation_new (T_DISPLAY_DATA *animateInfo , S32 anim_time ,MfwIcnAttr* icon ,UBYTE number_of_frames);
//Jun 04,2004 CRR:13601 xpradipg - SASKEN
//declaration for the function definition
extern void InfoDialogTimRestart(T_MFW_HND win);

// Feb 24, 2005 REF: CRR MMI-SPR-29102 xnkulkar
// Prototype declaration for the function 'mmi_dialog_information_screen_delete_all()'
T_MFW_HND mmi_dialog_information_screen_delete_all(MfwHnd win,int TextId, char* TextStr, int colour);

/*
 * Internal events
 */
#define DIALOG_INIT    200
#define DIALOG_DESTROY 201

// c018 rsa
typedef int DisplayData; // ??? rsa  needed!?
typedef int DialogInfo; // ??? rsa  needed!?

typedef enum
            {
             DIALOGS_REASON_NULL = DialogsDummy,
             DIALOGS_REASON_TIMER,
             DIALOGS_REASON_LEFTKEY,
             DIALOGS_REASON_RIGHTKEY,
             DIALOGS_REASON_CLEARKEY
            }DialogReasons;

// end c018 rsa
#endif



