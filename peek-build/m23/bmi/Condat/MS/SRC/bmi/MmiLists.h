#ifndef _DEF_MMI_LISTS_H_
#define _DEF_MMI_LISTS_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		    MmiLists.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the root mofule for the basic MMI
                        
********************************************************************************
 $History: MmiLists.h

 	May 18, 2006    REF: DRT OMAPS00076438  xdeepadh	
	Description: Scrolling not implemented in Imageviewer
	Solution: The support for scrolling has been provided.

       May 03, 2006    REF: DRT OMAPS00076439  xdeepadh
	Description: Image viewer back not working 
	Solution: The Preview List will be repopulated, when the list window is 
	resumed.
	
	 Dec 22, 2005    REF: ENH  xdeepadh
	Description: Image Viewer Application
	Solution: Implemeted the Image Viewer  to view the jpeg images

       xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
       Using the MenuSelect Key for requesting the help info in STK menu.
       
	March 01, 2005    REF: ENH 29313    Deepa M.D
	Isolating colour build related changes from MMI LITE flag.
 	The proper  Feature flag  COLORDISPLAY was used for all color 
   	related changes.
	
	   
 $End

*******************************************************************************/
#include "mfw_mnu.h" //for mfwMnuAttr

#define LISTS_SCROLL_UP   0
#define LISTS_SCROLL_DOWN 1

#define LISTS_FAIL 0
#define LISTS_OK   1

#define LISTS_MAX_ITEMS 10

#define SNAPSHOT_SIZE_2 2
#define SNAPSHOT_SIZE_4 4
#define SNAPSHOT_SIZE_6 6

#define LISTS_REASON_NULL     0
#define LISTS_REASON_SELECT   1
#define LISTS_REASON_BACK     2
#define LISTS_REASON_TIMEOUT   3
#define LISTS_REASON_CLEAR     4
#define LISTS_REASON_SCROLL     5
#define LISTS_REASON_HANGUP 6
#define LISTS_REASON_SEND 7
#define LISTS_REASON_CALL 8
//	Dec 22, 2005    REF: ENH  xdeepadh -Imageviewer
#define LISTS_REASON_SCROLL_UP     9
#define LISTS_REASON_SCROLL_DOWN     10
#define LISTS_REASON_RESUME   11
#define LISTS_REASON_SUSPEND   12
 #define LISTS_REASON_VISIBLE	13
 #define LISTS_REASON_DRAWCOMPLETE	14


/*Event which can be sent to list window*/
#define E_LIST_DESTROY 3 /*SPR 2132, destroy window*/

typedef enum
{
    E_START_MELODY,    
#ifdef FF_MMI_RINGTONE   /* Events for Ring tone handling */  
    E_STOP_MELODY,
    E_SUSPEND_MELODY,
    E_VISIBLE_MELODY
#else
    E_STOP_MELODY    /*a0393213 warnings removal- trailing comma is nonstandard*/
#endif    
} e_melody_events;



typedef struct
{
	MfwHnd		win;
    MfwMnuAttr  *Attr;            /* Menu attributes                    */
    MfwMnuItem  *List;		  /* List menu text strings.            */
    UBYTE        ListLength;	  /* Number of entries in list menu.    */
    UBYTE        ListPosition;	  /* True cursor position in list menu. */
    UBYTE        CursorPosition;  /* Cursor position in list snapshot.  */
    UBYTE        SnapshotSize;    /* Number of list entries to view.    */
    U8           Font;            /* List menu font.                    */
    BOOL         Strings;         /* TRUE = string display; FALSE = IDs */
    int          LeftSoftKey;     /* Left softkey text.                 */
    int          RightSoftKey;    /* Right softkey text.                */
    MfwEvt	 KeyEvents;       /* Keyboard events to be handled.     */
    UBYTE        Reason;          /* User defined exit reason.          */
    UBYTE 		autoDestroy;	
    UBYTE       selectKey;  //xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
//March 01, 2005    REF: ENH 29313    Deepa M.D
// Isolating colour build related changes from MMI LITE flag.
// The proper  Feature flag  COLORDISPLAY was used for all color 
//related changes.
#ifdef COLOURDISPLAY 
    U32			listCol; 
#else
    U8			listCol;/*SPR 2686*/
#endif
  MfwHnd       mfw_iv_handle;
  int lineHeight;
  BOOL block_keys;
} ListMenuData;

typedef void (*ListCbFunc)(T_MFW_HND * Parent, ListMenuData * ListData);

typedef struct
{
    T_MMI_CONTROL   mmi_control;
    T_MFW_HND winLists;
    SHORT           id;
    T_MFW_HND Parent;
    T_MFW_HND kbdLists;
    T_MFW_HND kbdLists_long;
    T_MFW_HND MenuLists;
    MfwMnuAttr MenuListsAttr;
    ListCbFunc Callback;
    UBYTE   ExitReason;
    ListMenuData MenuData;
} ListWinInfo;
typedef struct
{
    T_MMI_CONTROL   mmi_control;
    T_MFW_HND winLists;
    SHORT           id;
    T_MFW_HND Parent;
    T_MFW_HND kbdLists;
    T_MFW_HND kbdLists_long;
    T_MFW_HND MenuLists;
    MfwMnuAttr MenuListsAttr;
    ListCbFunc Callback;
    UBYTE   ExitReason;
    ListMenuData *MenuData;
} ListIconsWinInfo;
/* PROTOTYPES               */

T_MFW listDisplayListMenu (MfwHnd Parent, ListMenuData *Data, ListCbFunc cbFunc, int inUnicode);
T_MFW listIconsDisplayListMenu (MfwHnd Parent, ListMenuData *Data, ListCbFunc cbFunc,int inUnicode);
void listsIconsDestroy(T_MFW_HND win);
void listsDestroy(T_MFW_HND win);
#endif /* _DEF_MMI_LISTS_H_ */
