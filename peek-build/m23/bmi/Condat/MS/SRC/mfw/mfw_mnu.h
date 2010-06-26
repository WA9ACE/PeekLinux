/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_mnu.h       $|
| $Author:: Le  $ CONDAT GmbH           $Revision:: 10              $|
| CREATED: 23.11.98                     $Modtime:: 4.01.00 13:30    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_MNU

   PURPOSE : Keyboard types & constants

   EXPORT  :

   TO DO   :

   $History:: mfw_mnu.h                                             $

   	June 05, 2006 REF:OMAPS00060424 x0045876
 	Description: Header Toggling
 	Solution: Implemented the toggling of header when displaying the length text in SELECT ITEM and SET UP MENU

	May 18, 2006    REF: DRT OMAPS00076438  xdeepadh	
	Description: Scrolling not implemented in Imageviewer
	Solution: The support for scrolling has been provided.

        Shashi Shekar B.S., a0876501, 16 Mar, 2006, OMAPS00061462
       Icon support for SetupMenu & Select item.
  
 *
 * ************************************************
 * User: xreddymn           Date: Dec-16-2004
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Added double line display for MFW list menu items
 *
 * *****************  Version 10  *****************
 * User: Le           Date: 6.01.00    Time: 9:23
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Alignment of MFW versions
 *
 * *****************  Version 2  *****************
 * User: Es           Date: 22.11.99   Time: 10:29
 * Updated in $/GSM/Condat/SND-MMI/MFW
 *
 * *****************  Version 1  *****************
 * User: Es           Date: 18.11.99   Time: 16:35
 * Created in $/GSM/Condat/SND-MMI/MFW
 * Initial
*/

#ifndef _DEF_MFW_MNU_H_
#define _DEF_MFW_MNU_H_

                                        /* MENU MODES               */
                                        /* ALIGNMENT                */
#define MNU_LEFT        0x0001          /* left aligned             */
#define MNU_RIGHT       0x0002          /* right aligned            */
#define MNU_CENTER      0x0003          /* centered                 */
#define MNU_ALIGN       0x0003          /* alignment mode mask      */
                                        /* MENU MODES HEADER              */
                                        /* ALIGNMENT                */
#define MNU_HDR_LEFT		0x0004          /* left aligned             */
#define MNU_HDR_RIGHT		0x0008          /* right aligned            */
#define MNU_HDR_CENTER		0x000C          /* centered                 */
#define MNU_HDR_ALIGN		0x000C          /* alignment mode mask      */

                                        /* DISPLAY                  */
#define MNU_OVERLAPPED  0x0010          /* location as specified    */
#define MNU_PAGED       0x0020          /* single item per page     */
#define MNU_LIST        0x0030          /* list items on page       */
#define MNU_LIST_ICONS  0x0040          /* list items on page with icon for each      */
#define MNU_PAGE_ICONS  0x0050          /*  page with multiple 2-state icons      */
#define MNU_LIST_2_LINE	0x0060          /* list with each item occupying two lines. xreddymn Dec-16-2004 */
#define MNU_LIST_COLOUR 	0x00F0		/* list items on page - each item is a different colour */
//May 18, 2006    REF: DRT OMAPS00076438  xdeepadh	
#define MNU_DISPLAY     0x00F0          /* display mode mask        */

                                        /* ITEM SELECTION           */
#define MNU_FRAMED      0x0100          /* selection by frame       */
#define MNU_CUR_ARROW   0x0200          /* selection by arrow       */
#define MNU_CUR_STAR    0x0300          /* selection by star        */
#define MNU_CUR_BLOCK   0x0400          /* selection by block       */
#define MNU_CUR_LINE    0x0500          /* selection by line        */
#define MNU_ITEMSEL     0x0f00          /* item select mode mask    */

#define MNU_HDRFORMAT_STR	0x8000		/* 0=Text ID, 1=Text string in supplied language */

                                        /* MENU EVENTS              */
#define E_MNU_VISIBLE   0x00000001      /* menu is displayed        */
#define E_MNU_EMPTY     0x00000002      /* menu is empty            */
#define E_MNU_SELECT    0x00000010      /* menu item selected       */
#define E_MNU_ESCAPE    0x00000020      /* menu escape              */
#define E_MNU_TOPPED    0x00000040      /* menu over top            */
#define E_MNU_BOTTOMED  0x00000080      /* menu under bottom        */
#define E_MNU_PREDRAW   0x00000100      /* item will be drawn       */
#define E_MNU_POSTDRAW  0x00000200      /* item has been drawn      */

                                        /* ITEM EXTENSION FLAGS     */
#define MNU_ITEM_HIDE       0x0001      /* item is hidden           */
#define MNU_ITEM_NOTIFY     0x0002      /* notify on draw           */
#define MNU_ITEM_ANIMA      0x0004      /* icon animation           */
#define MNU_ITEM_FONT       0x0008      /* use font (disabled, ...) */
#define MNU_ITEM_STATED     0x0010      /* item has status          */
#define MNU_ITEM_RADIO      0x0020      /* item is radio button     */
#define MNU_ITEM_HOTKEY     0x0040      /* item can have hotkey     */
#define MNU_ITEM_UNCHECKED	0x0080		  /* SPR#998 - SH -Item in checklist, unchecked */
#define MNU_ITEM_CHECKED	  0x0100	  /* SPR#998 - SH - Item in checklist, checked */



#define MAX_LVL         100             /* maximum of menu level    */
#define UNUSED          -1              /* unused menu level        */

// Shashi Shekar B.S., a0876501, 16-Jan-2006, OMAPS00061460
#ifdef FF_MMI_SAT_ICON
#define TITLE_ICON_WIDTH			10
#define TITLE_ICON_HEIGHT			16
#define LINE_ICON_WIDTH			10
extern const unsigned char SATIconQuestionMark[];
#endif

/* SPR#1983 - SH - These are the different text encodings for menus */

typedef enum
{
	MNU_LIST_LANGUAGE_DEFAULT	= 0,
	MNU_LIST_LANGUAGE_UNICODE,
	MNU_LIST_LANGUAGE_ASCII
} MfwMnuListLanguageType;

struct MfwMnuItemTag; /*a0393213 compiler warning removal - removed typedef*/
struct MfwMnuAttrTag;  /*a0393213 compiler warning removal - removed typedef*/
struct MfwMnuTag;       /*a0393213 compiler warning removal - removed typedef*/

                                        /* menu function            */
typedef int (*MenuFunc)(struct MfwMnuTag* m, struct MfwMnuItemTag* i);
                                        /* item flag function       */
typedef U16 (*FlagFunc)(struct MfwMnuTag *m, struct MfwMnuAttrTag *a,
                                             struct MfwMnuItemTag *i);


typedef struct MfwItmExtPredrawTag      /* item ext: predraw        */
{
    char *str;                          /* string to be drawn       */
    MfwIcnAttr *icon;                   /* icon to be drawn         */
} MfwItmExtPredraw;

typedef struct MfwMnuItemTag            /* menu item                */
{
    MfwIcnAttr * icon;                  /* item with icon control   */
    void *exta;                         /* item extension data      */
    MfwHnd x;//edit;                        /* Must be removed ES!!     */
    char* str;                          /* item with textId (GW)     */
    struct MfwMnuAttrTag *menu;         /* item with submenu        */
    MenuFunc func;                      /* item with handler        */
    FlagFunc flagFunc;                  /* item flag function       */
} MfwMnuItem;

typedef struct MfwMnuAttrTag            /* menu attributes          */
{
    MfwRect *area;                      /* menu area                */
    U16 mode;                           /* menu modes               */
    U8 font;                            /* default menu font        */
    MfwMnuItem *items;                  /* menu items               */
    U8 nItems;                          /* number of menu items     */
    U32 mnuColour;				//GW index as to what the colour is going to be
    int  hdrId;					//GW Added
    MfwIcnAttr *bgdBitmap;		//GW Added for background for menus
   int unused;					//GW Added and removed definition
   
   /* 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
   BOOL header_toggle;
   
 // Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	MfwIcnAttr *icon;				/* Icon to be displayed in the header.*/
#endif
} MfwMnuAttr;
//If 'unused' is to be used, remove the definition below and add the required definition into the code
#define MNUATTRSPARE 0
typedef struct MfwMnuTag                /* menu control block       */
{
    MfwEvt mask;                        /* selection of events      */
    MfwEvt flags;                       /* current event            */
    MfwCb handler;                      /* menu event handler       */
    MfwMnuAttr *attr;                   /* menu attributes          */
    MfwMnuAttr *curAttr;                /* current attributes       */
    MfwHnd useStrID;                    /* GW 0=Text is passed as ID's ,   1=txt is strings     */
    MfwHnd useDefLang;					/* GW 0=Set text display to ascii, 1=txt is in language default*/
    U8 level;                           /* current level            */
	U8 lShift[MAX_LVL];					/* shift scrolling			*/
    U8 lCursor [MAX_LVL];               /* level cursor position    */
    U8 scrollMode;						
    int nLines;							/* Number of lines that ftn on screen */
      int lineHeight;					/* Height per line */
    UBYTE textDCS;		/* specify the data type of text (ASCII, Unicode etc) */
} MfwMnu;

MfwRes mnuInit (void);
MfwRes mnuExit (void);
MfwHnd mnuCreate (MfwHnd w, MfwMnuAttr *a, MfwEvt e, MfwCb Mnu);
MfwRes mnuDelete (MfwHnd m);
MfwHnd mnuLang (MfwHnd m, MfwHnd l);
MfwHnd mnuStrType (MfwHnd m, MfwHnd l); //GW
MfwRes mnuShow (MfwHnd m);
MfwRes mnuHide (MfwHnd m);
MfwRes mnuUnhide (MfwHnd m);//GW added 29/11/01
MfwRes mnuUpdate (MfwMnu *m);
MfwRes mnuUp (MfwHnd m);
MfwRes mnuDown (MfwHnd m);
MfwRes mnuEscape (MfwHnd m);
MfwRes mnuSelect (MfwHnd m);
MfwRes mnuDone (MfwHnd m);
MfwRes mnuKeyAction (MfwHnd m, U8 keycode);
MfwRes mnuChooseVisibleItem(MfwHnd m,U8 nItem);
U8 mnuCountVisibleItems(MfwHnd m);
U8 mnuCountCurrentVisibleItem(MfwHnd m);
void mnuScrollMode (MfwHnd menu,U8 mode);

void mnuInitDataItem(MfwMnuItem* mnuItem);
int mnuIdentify(struct MfwMnuAttrTag *attr, struct MfwMnuItemTag *item); /*SPR#998 - SH - added*/
EXTERN UBYTE dspl_Enable (UBYTE   in_Enable);
#endif
