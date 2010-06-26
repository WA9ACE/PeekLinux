/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_mnu.c       $|
| $Author:: Kk  $ CONDAT GmbH           $Revision:: 18              $|
| CREATED: 24.11.98                     $Modtime:: 24.02.00 8:07    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_MNU
 
   PURPOSE : menu handling functions

   EXPORT  :

   TO DO   :

   $History:: mfw_mnu.c                                             $

   	June 05, 2006 REF:OMAPS00060424 x0045876
 	Description: Header Toggling
 	Solution: Implemented the toggling of header when displaying the length text in SELECT ITEM and SET UP MENU

	May 18, 2006    REF: DRT OMAPS00076438  xdeepadh	
	Description: Scrolling not implemented in Imageviewer
	Solution: The support for scrolling has been provided.

 
       May 03, 2006    REF: DRT OMAPS00076439  xdeepadh
	Description: Image viewer back not working 
	Solution: The Preview List will be repopulated, when the list window is 
	resumed.

       Shashi Shekar B.S., a0876501, 16 Mar, 2006, OMAPS00061462
       Icon support for SetupMenu & Select item.
   
 *
 * ************************************************
	


	Dec 22, 2005    REF: ENH  xdeepadh
	Description: Image Viewer Application
	Solution: Implemeted the Image Viewer  to view the jpeg images
 
 * User: xreddymn           Date: Sep-30-2005
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Added header display for menus on monochrome
 * screens for MMI-SPR-33781
 *
 * ************************************************
 * User: xreddymn           Date: Dec-16-2004
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Added double line display for MFW list menu items
 *
 * *****************  Version 18  *****************
 * User: Kk           Date: 28.02.00   Time: 10:33
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * font usage corrected for page and list menus. free menus still
 * need rework.
 *
 * *****************  Version 17  *****************
 * User: Es           Date: 17.01.00   Time: 17:37
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * menu mode handling bug (uses only first level settings)
|
| *****************  Version 16  *****************
| User: Le           Date: 6.01.00    Time: 9:23
| Updated in $/GSM/Condat/MS/SRC/MFW
| Alignment of MFW versions
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

#include <stdio.h>
#include <string.h>

#define ENTITY_MFW

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#include "mfw_sys.h"
#include "mfw_mfw.h"
#include "mfw_kbd.h"
#include "gdi.h"
#include "dspl.h"
#include "mfw_lng.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_icn.h"
#include "mfw_mnu.h"


#include "message.h"
#include "prim.h"
#include "aci_cmh.h"

#include "mfw_mmi.h"

/* START: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
#include "mfw_tim.h"
#include "ATBCommon.h"
#include "ATBDisplay.h"

#define MNU_TOGGLE_TIMEOUT 3000

static USHORT g_title_pos;			
static USHORT g_title_next_pos;	
MfwHnd g_title_timer;

extern MfwHnd g_win;	
/* END: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */

static int menuIsEmpty (MfwMnu *m, MfwMnuAttr *ma);
static int countVisible (MfwMnu *m, MfwMnuAttr *ma, int start);
static void drawPageMenu (MfwMnu *mnu);
static void drawListMenu (MfwMnu *m);
static void drawIconsListMenu(MfwMnu *m);
static void drawFreeMenu (MfwMnu *m);
//May 18, 2006    REF: DRT OMAPS00076438  xdeepadh	
static int mnuCommand (U32 cmd, void *h);

/* 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
static int Menu_edit_TitleTimerCb(MfwEvt e, MfwTim *t);

#define TIME_TRACE_EVENT 

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuInit            |
+--------------------------------------------------------------------+

  PURPOSE : initialize menu handler

*/

MfwRes mnuInit (void)
{
    mfwCommand[MfwTypMnu] = (MfwCb) mnuCommand;

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuExit            |
+--------------------------------------------------------------------+

  PURPOSE : finalize menu handler

*/

MfwRes mnuExit (void)
{
    mfwCommand[MfwTypMnu] = 0;

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuCreate          |
+--------------------------------------------------------------------+

  PURPOSE : create menu control
  GW 05/10/01 - Changed single value 'lng' to 2 values indicating if ID's should be 
				used (useStrID) and if the strings are ascii or unicode (uesDefLang).

*/

MfwHnd mnuCreate (MfwHnd w, MfwMnuAttr *a, MfwEvt e, MfwCb f)
{
    MfwHdr *hdr = (MfwHdr *) mfwAlloc(sizeof(MfwHdr));
    MfwMnu *mnu = (MfwMnu *) mfwAlloc(sizeof(MfwMnu));
	MfwHdr *insert_status =0;
	
    if (!hdr || !mnu)
    {
    	TRACE_ERROR("ERROR: mnuCreate() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));
   		if(mnu)
   			mfwFree((U8*)mnu,sizeof(MfwMnu));	
   		
	   	return 0;
  	}

    mnu->mask = e;
    mnu->flags = 0;
    mnu->handler = f;
    mnu->attr = a;
    mnu->curAttr = a;

    mnu->useStrID = 0;
    mnu->useDefLang = 0;
    /* SPR#1983 - SH - Stores character type, ASCII/UNICODE */
    mnu->textDCS = MNU_LIST_LANGUAGE_DEFAULT;
    mnu->level = 0;
    memset(mnu->lCursor,UNUSED,sizeof(mnu->lCursor));
    memset(mnu->lShift,1,sizeof(mnu->lShift));
    mnu->lCursor[0] = 0;
    mnu->scrollMode = 1;
    hdr->data = mnu;
    hdr->type = MfwTypMnu;

    insert_status = mfwInsert(w,hdr);
    if(!insert_status)
	{
  		TRACE_ERROR("ERROR: mnuCreate() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)mnu ,sizeof(MfwMnu));
		return 0;
  	}
    return insert_status;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuDelete          |
+--------------------------------------------------------------------+

  PURPOSE : delete menu control

*/

MfwRes mnuDelete (MfwHnd m)
{
    MfwRes res;

    if (!m)
        return MfwResIllHnd;

    mnuHide(m);
    res = (mfwRemove(m)) ? MfwResOk : MfwResIllHnd;

    mfwFree(((MfwHdr *) m)->data,sizeof(MfwMnu));
    mfwFree(m,sizeof(MfwHdr));

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuLang            |
+--------------------------------------------------------------------+

  PURPOSE : set / get whether the string is associated with an ID or if it is null-terminated

  GW 05/10/01 - Changed single value 'lng' to 2 values indicating if ID's should be 
				used (useStrID) and if the strings are ascii or unicode (uesDefLang).
*/

MfwHnd mnuLang (MfwHnd m, MfwHnd l)
{
    MfwHnd ol;
    MfwMnu *mnu;

    if( (mnu = mfwControl(m)) == 0)
        return (MfwHnd) MfwResIllHnd;

    ol = mnu->useStrID;
    mnu->useStrID = l;
    mnu->useDefLang = l;

	/* SPR#1983 - SH - Set character type appropriately */
   	if (l)
   	{
   		mnu->textDCS = MNU_LIST_LANGUAGE_DEFAULT;
   	}
   	else
   	{
   		mnu->textDCS = MNU_LIST_LANGUAGE_ASCII;
   	}	

    return ol;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuStrType         |
+--------------------------------------------------------------------+

  PURPOSE : set / get language in use (1=use default ascii/unicode, 0=assume ascii)

  GW 05/10/01 - Changed single value 'lng' to 2 values indicating if ID's should be 
				used (useStrID) and if the strings are ascii or unicode (uesDefLang).
*/
MfwHnd mnuStrType (MfwHnd m, MfwHnd l)
{
    MfwHnd ol;
    MfwMnu *mnu;

    if( (mnu = mfwControl(m)) == 0)
        return (MfwHnd) MfwResIllHnd;

    ol = mnu->useDefLang;
    mnu->useDefLang = l;

	/* SPR#1983 - SH  - Set character type appropriately*/
   	if (l)
   	{
   		mnu->textDCS = MNU_LIST_LANGUAGE_UNICODE;
   	}
   	else
   	{
   		mnu->textDCS = MNU_LIST_LANGUAGE_DEFAULT;
   	}
   	
    return ol;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuDCSType         |
+--------------------------------------------------------------------+

  PURPOSE : SPR#1983 - SH - Added.
  			Set the character type of the menu text
*/

UBYTE mnuDCSType (MfwHnd m, UBYTE listLanguageType)
{
    UBYTE ol;
    MfwMnu *mnu;

    if( (mnu = mfwControl(m)) == 0)
        return NULL;

    ol = mnu->textDCS;
    mnu->textDCS = listLanguageType;
   	
    return ol;
}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuShow            |
+--------------------------------------------------------------------+

  PURPOSE : show menu

*/

MfwRes mnuShow (MfwHnd m)
{
    MfwMnu *mnu;

    if( (mnu = mfwControl(m)) == 0)
        return MfwResIllHnd;

    mnu->flags |= E_MNU_VISIBLE;
    mnuUpdate(mnu);

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuHide            |
+--------------------------------------------------------------------+

  PURPOSE : hide menu

*/

MfwRes mnuHide (MfwHnd m)
{
    MfwMnu *mnu;
//	U8 dsplOld;  

    if ((mnu = mfwControl(m)) == 0)
        return MfwResIllHnd;

    mnu->flags &= ~E_MNU_VISIBLE;
    if (mnu->handler)
        if (mnu->mask & E_MNU_VISIBLE)
            (void)(mnu->handler(E_MNU_VISIBLE,mnu));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuUnhide          |
+--------------------------------------------------------------------+

  PURPOSE : unhide menu (without drawing)

*/

MfwRes mnuUnhide (MfwHnd m)
{
    MfwMnu *mnu;
//	U8 dsplOld;  

    if ((mnu = mfwControl(m)) == 0)
        return MfwResIllHnd;

    mnu->flags |= E_MNU_VISIBLE;
    if (mnu->handler)
        if (mnu->mask & E_MNU_VISIBLE)
            (void)(mnu->handler(E_MNU_VISIBLE,mnu));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuUpdate          |
+--------------------------------------------------------------------+

  PURPOSE : draw menu

*/

MfwRes mnuUpdate (MfwMnu *m)
{

//    U8 dsplOld;  
	

    if (!m)
        return MfwResIllHnd;
    if (!(m->flags & E_MNU_VISIBLE))
        return MfwResOk;

    if (m->handler)
        if (m->mask & E_MNU_VISIBLE)
            (void)(m->handler(E_MNU_VISIBLE,m));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/

	/*JVJE Update to fit the menu to a visible item */
	{
		MfwMnuAttr *ma = m->curAttr;
		int index= m->lCursor[m->level];
	//	int iloop;   
		U16 retVal;

		 if (ma->items[index].flagFunc)
			 retVal = ma->items[index].flagFunc(m,ma,&(ma->items[index]));
		 else
		 	retVal = 0;

		while (retVal & MNU_ITEM_HIDE)
	    {
			index++;
			if (index>=ma->nItems)
				index =0;

			 if (ma->items[index].flagFunc)
				 retVal = ma->items[index].flagFunc(m,ma,&(ma->items[index]));
			 else
			 	retVal = 0;

	    } 
		m->lCursor[m->level]=index;	    
	}
	
    if ((m->curAttr->mode & MNU_DISPLAY) == MNU_OVERLAPPED)
        drawFreeMenu(m);
//May 18, 2006    REF: DRT OMAPS00076438  xdeepadh		
    else if ((m->curAttr->mode & MNU_DISPLAY) == MNU_PAGED)
        drawPageMenu(m);
    else if ((m->curAttr->mode & MNU_DISPLAY) == MNU_LIST)
        drawListMenu(m);
    else if ((m->curAttr->mode & MNU_DISPLAY) ==MNU_LIST_ICONS)
	drawIconsListMenu(m);
    /* xreddymn Dec-16-2004 MMI-SPR-27384: Two line height list display */
    else if ((m->curAttr->mode & MNU_DISPLAY) == MNU_LIST_2_LINE)
        drawListMenu(m);
    else if ((m->curAttr->mode & MNU_DISPLAY) == MNU_LIST_COLOUR)
        drawListMenu(m);
   if (m->handler)
        if (m->mask & E_MNU_POSTDRAW)
            (void)(m->handler(E_MNU_POSTDRAW,m));

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuUp              |
+--------------------------------------------------------------------+

  PURPOSE : one step up

*/

MfwRes mnuUp (MfwHnd m)
{
    MfwMnu *mnu;
    MfwMnuAttr *ca;
    MfwMnuItem *ci;
//	U8 uMode;  
	
    if ((mnu = mfwControl(m)) == 0)
        return MfwResIllHnd;


    ca = mnu->curAttr;
    do
    {
        if (mnu->lCursor[mnu->level] == 0)
        {
            mnu->lCursor[mnu->level] = ca->nItems - 1;
            mnu->flags |= E_MNU_TOPPED;
            if (mnu->handler)
                if (mnu->mask & E_MNU_TOPPED)
                    (void)(mnu->handler(E_MNU_TOPPED,mnu));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
        }
        else
        {
            mnu->lCursor[mnu->level] -= (U8) 1;
            mnu->flags &= ~E_MNU_TOPPED;
        }
        ci = ca->items + mnu->lCursor[mnu->level];
    } while (ci->flagFunc(mnu,ca,ci) & MNU_ITEM_HIDE);
	
	if (mnu->scrollMode) /* Simple scrolling */
	{
		U8 shift = mnu->lShift[mnu->level];
		U8 index = countVisible(mnu,ca,mnu->lCursor[mnu->level]);
		U8 visibleItems = countVisible(mnu,ca,ca->nItems); 
		int nLines = mnu->nLines;
	    		
		if (visibleItems<nLines)
			nLines = visibleItems;
		if (shift>1)
			shift--;
		mnu->lShift[mnu->level] = shift;		

	}
    mnuUpdate(mnu);
    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuDown            |
+--------------------------------------------------------------------+

  PURPOSE : one step down

*/

MfwRes mnuDown (MfwHnd m)
{
    MfwMnu *mnu;
    MfwMnuAttr *ca;
    MfwMnuItem *ci;


    if ((mnu = mfwControl(m)) == 0)
        return MfwResIllHnd;
    ca = mnu->curAttr;
    do
    {
        if (mnu->lCursor[mnu->level] >= mnu->curAttr->nItems - 1)
        {
            mnu->lCursor[mnu->level] = 0;
            mnu->flags |= E_MNU_BOTTOMED;
            if (mnu->handler)
                if (mnu->mask & E_MNU_BOTTOMED)
                    (void)(mnu->handler(E_MNU_BOTTOMED,mnu));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
        }
        else
        {
            mnu->lCursor[mnu->level] += (U8) 1;
            mnu->flags &= ~E_MNU_BOTTOMED;
        }
        ci = ca->items + mnu->lCursor[mnu->level];
    } while (ci->flagFunc(mnu,ca,ci) & MNU_ITEM_HIDE);

	if (mnu->scrollMode) /* Simple scrolling */
	{
		U8 shift = mnu->lShift[mnu->level];
		U8 index = countVisible(mnu,ca,mnu->lCursor[mnu->level]);
		U8 visibleItems = countVisible(mnu,ca,ca->nItems); 
		int nLines = mnu->nLines;
		
		if (visibleItems<nLines)
			nLines = visibleItems;
		if (shift<nLines)
			shift++;
		mnu->lShift[mnu->level] = shift;		

	}
    mnuUpdate(mnu);
    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuSelect          |
+--------------------------------------------------------------------+

  PURPOSE : select a menu item; if there is a submenu, go one level
            down

*/

MfwRes mnuSelect (MfwHnd m)
{
    MfwMnu *mnu;
    MfwMnuAttr *submenu, *ca;
    MenuFunc func;
//	U8 dsplOld;  

    if ((mnu = mfwControl(m)) == 0)
        return MfwResIllHnd;
    
    ca = mnu->curAttr;
    submenu = ca->items[mnu->lCursor[mnu->level]].menu;
    func = ca->items[mnu->lCursor[mnu->level]].func;

    if (func)                        /* perform submenu          */
    {
        (void)(func(mnu,&ca->items[mnu->lCursor[mnu->level]]));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
    }

    if (submenu)                      /* call menu function       */
    {
        if (menuIsEmpty(mnu,submenu))
        {
            mnu->flags |= E_MNU_EMPTY;
            if (mnu->handler && mnu->mask & E_MNU_EMPTY)
                (void)(mnu->handler(E_MNU_EMPTY,mnu));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
            mnu->flags &= ~E_MNU_EMPTY;
        }
        else
        {
            dspl_Clear(ca->area->px, ca->area->py,
                        (U16) (ca->area->sx+ca->area->px-1),
                        (U16) (ca->area->sy+ca->area->py-1));
            mnu->level++;
            mnu->lCursor[mnu->level] = 0;
            ca = mnu->curAttr = submenu;
            if (ca->items->flagFunc(mnu,ca,ca->items) & MNU_ITEM_HIDE)
                mnuDown(m);
            else
                mnuUpdate(mnu);
        }
	    return MfwResOk;								/* we do not have to check for !submenu */
    }

    if (!func && !submenu)                                /* inform mmi               */
    {
        mnu->flags |= E_MNU_SELECT;
        if (mnu->handler && mnu->mask & E_MNU_SELECT)
            (void)(mnu->handler(E_MNU_SELECT,mnu));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
    }
    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuEscape          |
+--------------------------------------------------------------------+

  PURPOSE : go back one level or leave the menu

*/

MfwRes mnuEscape (MfwHnd m)
{
    MfwMnu *mnu;
    MfwMnuAttr *attr;
    U8 lvl;
//	U8 dsplOld;  

    if ((mnu = mfwControl(m)) == 0)
        return MfwResIllHnd;

    if (mnu->level != 0)
    {
        mnu->lCursor[mnu->level] = (U8)UNUSED; /*a0393213 removing compiler warnings - UNUSED type casted*/
        mnu->lShift[mnu->level] = 1;
        mnu->level--;
        lvl = 0;                        /* set level 0              */
        attr = mnu->attr;               /* set start point          */
        while (lvl != mnu->level)
        {
            attr = attr->items[mnu->lCursor[lvl]].menu;
            lvl++;
        }
        mnu->curAttr = attr;
        mnuUpdate(mnu);
    }
    else
    {
        mnu->flags |= E_MNU_ESCAPE;
        if (mnu->handler)
            if (mnu->mask & E_MNU_ESCAPE)
                (void)(mnu->handler(E_MNU_ESCAPE,mnu));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
    }
    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuDone            |
+--------------------------------------------------------------------+

  PURPOSE : reset menu

*/

MfwRes mnuDone (MfwHnd m)
{
    MfwMnu *mnu;

    if ((mnu = mfwControl(m)) == 0)
        return MfwResIllHnd;

    mnu->curAttr = mnu->attr;
    mnu->level = 0;
    memset(mnu->lCursor,UNUSED,sizeof(mnu->lCursor));
    memset(mnu->lShift,1,sizeof(mnu->lShift));
    mnu->lCursor[0] = 0;

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuKeyAction       |
+--------------------------------------------------------------------+

  PURPOSE : perform direct jumping if there was pressed KCD_1
            through KCD_9; in this case function returns 1;
            otherwise function returns 0

*/

MfwRes mnuKeyAction (MfwHnd m, U8 keycode)
{
    MfwMnu *mnu;
    U8 directAccess;

    if ((mnu = mfwControl(m)) == 0)
        return MfwResIllHnd;

    switch (keycode)
    {
        case KCD_1: directAccess = 0; break;
        case KCD_2: directAccess = 1; break;
        case KCD_3: directAccess = 2; break;
        case KCD_4: directAccess = 3; break;
        case KCD_5: directAccess = 4; break;
        case KCD_6: directAccess = 5; break;
        case KCD_7: directAccess = 6; break;
        case KCD_8: directAccess = 7; break;
        case KCD_9: directAccess = 8; break;
        default: return MfwResOk; /*a0393213 compiler warning removal --- 0 converted into MfwResOk*/
    }

    if (directAccess >= mnu->curAttr->nItems)
        return MfwResOk;  /*a0393213 compiler warning removal --- 0 converted into MfwResOk*/

    mnu->lCursor[mnu->level] = directAccess;
    mnuShow(m);
    mnuSelect(m);

    return MfwResDone;  /*a0393213 compiler warning removal --- 1 converted into MfwResDone*/
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuChooseItem      |
+--------------------------------------------------------------------+

  PURPOSE : Chooses the nItemst Item.

*/

MfwRes mnuChooseVisibleItem(MfwHnd m,U8 nItem)
{

    MfwMnu *mnu;
	int i;

    if ((mnu = mfwControl(m)) == 0)
        return MfwResIllHnd;

/*a0393213 compiler warning removal - commented*/
/*	if (nItem<0)
		return MfwResIllHnd;*/
	for (i = 0; i < mnu->curAttr->nItems; i++)
    {
		if (!(mnu->curAttr->items[i].flagFunc(mnu,
                                              mnu->curAttr,
                                            &(mnu->curAttr->items[i]))
                & MNU_ITEM_HIDE))
		{
			if (!nItem)
				{
					mnu->lCursor[mnu->level] = i;
					mnuUpdate(mnu);
					return MfwResOk;
				}
			else
				nItem--;
		}
	}
	return MfwResIllHnd;

}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : menuIsEmpty        |
+--------------------------------------------------------------------+

  PURPOSE : check for empty submenu

*/

static int menuIsEmpty (MfwMnu *m, MfwMnuAttr *ma)
{
    int i;

    for (i = 0; i < ma->nItems; i++)
        if (!(ma->items[i].flagFunc(m,ma,&(ma->items[i]))
                                            & MNU_ITEM_HIDE))
            return 0;

    return 1;
}





/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : countVisible       |
+--------------------------------------------------------------------+

  PURPOSE : count visible items up to current item

*/

static int countVisible (MfwMnu *m, MfwMnuAttr *ma, int start)
{
    int i, count;

    for (i = 0, count = 0; i < start; i++)
        if (!(ma->items[i].flagFunc(m,ma,&(ma->items[i]))
                                            & MNU_ITEM_HIDE))
            count++;

    return count;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : countVisibleItems  |
+--------------------------------------------------------------------+

  PURPOSE : count visible items in the current branch

*/

U8 mnuCountVisibleItems (MfwHnd m)
{
    MfwMnu *mnu;

	if ((mnu = mfwControl(m)) == 0)
        return 0;

	return (U8)countVisible(mnu, mnu->curAttr,mnu->curAttr->nItems);
}


/*
+--------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU                  |
| STATE   : code                        ROUTINE : countCurrentVisibleItem  |
+--------------------------------------------------------------------------+

  PURPOSE : returns the position of the current Item
*/

U8 mnuCountCurrentVisibleItem(MfwHnd m)
{
    MfwMnu *mnu;
	U8 i;
	U8 pos=0;

	if ((mnu = mfwControl(m)) == 0)
        return 0;

	for (i = 0; i < mnu->curAttr->nItems; i++)
    {
		if ((mnu->lCursor[mnu->level]) == i)
			return pos;
		if (!(mnu->curAttr->items[i].flagFunc(mnu,
                                              mnu->curAttr,
                                            &(mnu->curAttr->items[i]))
                & MNU_ITEM_HIDE))
			pos++;
	}
	return pos;

}




/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : checkPredraw       |
+--------------------------------------------------------------------+

  PURPOSE : check / generate predraw event

*/

static int checkPredraw (MfwMnu *mnu, MfwMnuItem *mi,
                                char **t, MfwIcnAttr **icn)
{
    int res = 0;

    if (mi->flagFunc(mnu,mnu->curAttr,mi) & MNU_ITEM_NOTIFY)
    {
        mnu->flags |= E_MNU_PREDRAW;
        if (mnu->handler)
            if (mnu->mask & E_MNU_PREDRAW)
            {
                if (mi->exta)
                {
                    ((MfwItmExtPredraw*)(mi->exta))->str = *t;
                    ((MfwItmExtPredraw*)(mi->exta))->icon = *icn;
                }
                res = mnu->handler(E_MNU_PREDRAW,mi);
                if (mi->exta)
                {
                    *t = ((MfwItmExtPredraw*)(mi->exta))->str;
                    *icn = ((MfwItmExtPredraw*)(mi->exta))->icon;
                }
            }
        mnu->flags &= ~E_MNU_PREDRAW;
    }

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : checkPostdraw      |
+--------------------------------------------------------------------+

  PURPOSE : check / generate postdraw event

*/

static int checkPostdraw (MfwMnu *mnu, MfwMnuItem *mi)
{
    int res = 0;

    if (mi->flagFunc(mnu,mnu->curAttr,mi) & MNU_ITEM_NOTIFY)
    {
        mnu->flags |= E_MNU_POSTDRAW;
        if (mnu->handler)
            if (mnu->mask & E_MNU_POSTDRAW)
                res = mnu->handler(E_MNU_POSTDRAW,mi);
        mnu->flags &= ~E_MNU_POSTDRAW;
    }

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : checkStatus        |
+--------------------------------------------------------------------+

  PURPOSE : check / insert item status (string MUST be in RAM)

  SPR998 - SH - Changed to allow checkbox menu items.
*/

static int checkStatus (MfwMnu *m, MfwMnuAttr *ma, MfwMnuItem *mi,
                                                              char *menuString)
{
	char *e;                            /* ES!! UNICODE             */
	int index = 0;   
	/*a0393213 compiler warning removal - variable unicode removed*/
	U16 flagResult;

	flagResult = mi->flagFunc(m,ma,mi);
    switch (flagResult)
    {
    	case MNU_ITEM_STATED:
		    e = mi->exta;
		    if (!e)
		        return 0;
		    while (menuString[index])
		    {
		        if (menuString[index] == '%')                  /* ES!! : UNICODE           */
		        {
		            switch (menuString[index+1])
		            {
		                case 'c':               /* single char              */
		                    strcpy(&menuString[index+1],&menuString[index+2]);
		                    menuString[index] = *e;
		                    e++;
		                    break;
		            }
		        }
		        index++;
		    }
		    break;

		/* SPR998 - SH - Checked or unchecked option.
		 * Don't need to do anything here. */
		
		case MNU_ITEM_UNCHECKED:
		case MNU_ITEM_CHECKED:
			break;
			
		default:
			break;
	}

    return flagResult;	/* SPR#998 - SH - Now returns item type */
}


static void mnuDrawHeader(MfwMnuAttr *mnuAttr, MfwRect* areaLeft, char *hdrString)
{
	int mode = mnuAttr->mode;
	MfwRect* mnuArea = mnuAttr->area;
	int mnuColour =  mnuAttr->mnuColour;
	char* txt=hdrString;
	int nPixels;
	int xPos,yPos,sy,txtXpos;  // Removed txtLen, sx

	/* START: 05-June 2006, x0045876 (OMAPS00060424 - Header Toggle) */
	dspl_DevCaps disData;
	
	T_DS_TEXTFORMAT  format;
	
	T_ATB_TEXT title;
	T_ATB_TEXT newTitle;

	USHORT	titleWidth;
	/*a0393213 warning removal-variable titleHeight removed*/
	USHORT	textIndex;
	USHORT	lastSpace;
	USHORT	character;
	/* END: 05-June 2006, x0045876 (OMAPS00060424 - Header Toggle) */

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	USHORT		titleIconWidth = 0;
	USHORT 		iconX, iconY;
#endif

	xPos = mnuArea->px;
	yPos = mnuArea->py;
	sy = res_getTitleHeight();
	resources_setTitleColour(mnuColour);
       dspl_Clear(xPos,yPos,(U16)(mnuArea->sx+xPos-1),(U16)(mnuArea->sy+yPos-1));
	
	if ((mode & MNU_HDR_ALIGN) == MNU_HDR_LEFT)
	{
		txtXpos = xPos;
	}
	else
	{
		/* START: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
		if ((mode & MNU_HDR_ALIGN) == MNU_HDR_RIGHT)
		{
			nPixels = dspl_GetTextExtent( txt, 0);
			txtXpos =  xPos + mnuArea->sx - nPixels;
		}
		else
		{	//Display in middle
			/* START: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
			if (mnuAttr->header_toggle)
			{
				title.dcs = ATB_DCS_ASCII;
				title.data = (UBYTE*)txt;/*a0393213 warnings removal-typecasting done*/
				title.len = ATB_string_Length(&title);
					
				ATB_display_SetFormatAttr(&format, 0, FALSE);
				titleWidth = ATB_display_StringWidth(&title, &format)+2;
				/*titleHeight = ATB_display_StringHeight(&title, &format);*//*a0393213 warning removal-titleHeight assigned but never used*/

				dspl_GetDeviceCaps(&disData);

				if (titleWidth>(disData.Width-2))
				{
					if (!g_title_timer)
					{
						g_title_timer = timCreate(g_win, MNU_TOGGLE_TIMEOUT, (MfwCb) Menu_edit_TitleTimerCb);
						g_title_pos = 0;
						g_title_next_pos = 0;
						timStart(g_title_timer);
					}

					titleWidth = 0;
					textIndex = g_title_pos;
					lastSpace = 0;
	
					while (titleWidth<(disData.Width -2) && textIndex<title.len)
					{
						character = ATB_string_GetChar(&title, textIndex);
						titleWidth+=ATB_display_GetCharWidth(character, &format);
						textIndex++;
						if (character==UNICODE_SPACE)
							lastSpace = textIndex;
					}

					if (textIndex==title.len)
						g_title_next_pos = 0;
					else
					{
						if (lastSpace>0)
							textIndex = lastSpace;
						g_title_next_pos = textIndex;
					}

					newTitle.len = textIndex-g_title_pos;
					newTitle.data = (UBYTE *)mfwAlloc((newTitle.len+1)*ATB_string_Size(&title));
					newTitle.dcs = title.dcs;

					for (textIndex=0; textIndex<newTitle.len; textIndex++)
					{
						ATB_string_SetChar(&newTitle, textIndex, ATB_string_GetChar(&title, g_title_pos+textIndex));
					}
					ATB_string_SetChar(&newTitle, newTitle.len, UNICODE_EOLN);	
					ATB_display_Text(0,0,&format, &newTitle);
					mfwFree((UBYTE *)newTitle.data, (newTitle.len+1)*ATB_string_Size(&title));

					areaLeft->px = xPos;	
					areaLeft->sx = mnuArea->sx;
					areaLeft->py = yPos+sy;	
					areaLeft->sy = mnuArea->sy-sy;
	
					resources_restoreMnuColour();
					return;
				
				}
				else
				{
					nPixels = dspl_GetTextExtent( txt, 0);
					txtXpos =  xPos + (mnuArea->sx - nPixels)/2;
				}
			}
			else
			{
				nPixels = dspl_GetTextExtent( txt, 0);
				txtXpos =  xPos + (mnuArea->sx - nPixels)/2;
			}
			/* END: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
		}
	}

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
       if (mnuAttr->icon != NULL)
	{
		if (mnuAttr->icon->area.sx > TITLE_ICON_WIDTH) 
		{
			titleIconWidth = TITLE_ICON_WIDTH;
		}
		else
		{
			titleIconWidth = mnuAttr->icon->area.sx;
		}
	}			
	else
	{
		titleIconWidth = 0;
	}

	if(titleIconWidth)
		txtXpos = txtXpos + titleIconWidth + 1;

    if (mnuAttr->icon != NULL)
    {
		if(mnuAttr->icon->selfExplanatory == FALSE)
		{
			if (txt[0] != (char)0x80) /*a0393213 lint warnings removal - typecast done*/
				dspl_ScrText (txtXpos, yPos, txt, 0);
			else
				dspl_TextOut(txtXpos, yPos, DSPL_TXTATTR_CURRENT_MODE, txt );
		}	
    }
    else
    {
			if (txt[0] != (char)0x80) /*a0393213 lint warnings removal - typecast done*/
				dspl_ScrText (txtXpos, yPos, txt, 0);
			else
				dspl_TextOut(txtXpos, yPos, DSPL_TXTATTR_CURRENT_MODE, txt );
    }
#else
	if (txt[0] != 0x80)
		dspl_ScrText (txtXpos, yPos, txt, 0);
	else
		dspl_TextOut(txtXpos, yPos, DSPL_TXTATTR_CURRENT_MODE, txt );
#endif	

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
    if (mnuAttr->icon != NULL)
	{
	    if (mnuAttr->icon->icons != NULL)
	    {
			if ((mnuAttr->icon->area.sx > TITLE_ICON_WIDTH) || (mnuAttr->icon->area.sy > TITLE_ICON_HEIGHT))
			{
				/* iconY calculates place holder to place the icon 
					at the center of the screen. */
				   iconX = 1;
				   iconY = 1+ ((sy-2) / 2) - (10 / 2);
				   dspl_BitBlt2(iconX, iconY, 8, 
						10, (void*)SATIconQuestionMark, 0, BMP_FORMAT_256_COLOUR);
			}
			else
			{
			        dspl_BitBlt2(mnuAttr->icon->area.px, mnuAttr->icon->area.py,
			   	                mnuAttr->icon->area.sx, mnuAttr->icon->area.sy,
			                    mnuAttr->icon->icons, 0, mnuAttr->icon->icnType);
			}
	    	}
    	}
#endif

	areaLeft->px = xPos;	
	areaLeft->sx = mnuArea->sx;
	areaLeft->py = yPos+sy;	
	areaLeft->sy = mnuArea->sy-sy;
#ifdef MFW_DEBUG_DISPLAY_SIZE
    dspl_DrawRect(xPos,yPos,(xPos+mnuArea->sx-1),(yPos+sy-1));
#endif    
	resources_restoreMnuColour();
}

// Sep 30, 2005  REF: CRR MMI-SPR-33781  xreddymn
// Description: Menu header not displayed for Golite
// Solution: Added functionality to display menu header
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuDrawHeaderBW    |
+--------------------------------------------------------------------+

  PURPOSE : Displays menu header (title) on monochrome displays.
  Code is based on mnuDrawHeader. See also mnuDrawHeader.
*/
static void mnuDrawHeaderBW(MfwMnuAttr *mnuAttr, MfwRect* areaLeft, char *hdrString)
{
	int     mode = mnuAttr->mode;
	MfwRect *mnuArea = mnuAttr->area;
	int     mnuColour = mnuAttr->mnuColour;
	char    *txt = hdrString;
	int     nPixels;
	int     xPos, yPos,  sy,  txtXpos; /*a0393213 compiler warning removal, removed sx and txtLen*/

	xPos = mnuArea->px;
	yPos = mnuArea->py;
	sy = res_getTitleHeight();
	resources_setTitleColour(mnuColour);
	dspl_Clear(xPos, yPos, (U16)(mnuArea->sx + xPos - 1), (U16)(mnuArea->sy + yPos - 1));
	dspl_DrawLine(xPos, (U16)(sy + yPos - 2), (U16)(mnuArea->sx + xPos - 1), (U16)(sy + yPos - 2));
	if ((mode & MNU_HDR_ALIGN) == MNU_HDR_LEFT)
	{	//Position is correct
		txtXpos = xPos;
	}
	else
	{
		nPixels = dspl_GetTextExtent( txt, 0);
		if ((mode & MNU_HDR_ALIGN) == MNU_HDR_RIGHT)
		{
			txtXpos = xPos + mnuArea->sx - nPixels;
		}
		else
		{	//Display in middle
			txtXpos = xPos + (mnuArea->sx - nPixels)/2;
		}
	}
	if (txt[0] != (char)0x80) /*a0393213 lint warnings removal-typecast done*/
		dspl_ScrText (txtXpos, yPos, txt, 0);
	else
		dspl_TextOut(txtXpos, yPos, DSPL_TXTATTR_CURRENT_MODE, txt);
	// Remaining area is calculated so that it does not touch
	// or overlap with the menu header
	areaLeft->px = xPos;
	areaLeft->sx = mnuArea->sx;
	areaLeft->py = yPos+sy + 1;
	areaLeft->sy = mnuArea->sy - sy - 1;
	resources_restoreMnuColour();
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : drawPageMenu       |
+--------------------------------------------------------------------+

  PURPOSE : draws menu in page mode

  GW 05/10/01 - Changed single value 'lng' to 2 values indicating if ID's should be 
				used (useStrID) and if the strings are ascii or unicode (uesDefLang).

*/
void fastCopyBitmap(int startX, 	int startY, 	// start position of bitmap
						int bmpSx,	int bmpSy,		//size of bitmap
						char*	srcBitmap,
						int posX,   	int posY,   	// start of area to be copied into
						int sx,     	int sy,     	// size of area to be copied into 
						U32 bgd_col,	int bmptype);

static void drawPageMenu (MfwMnu *mnu)
{
    MfwMnuAttr *ma;                     /* menu attributes          */
    //U16 th, tw, tl;                     /* text height, width, len  */ 
    U16 ax, ay, aw, ah;  // , x;              /* menu area dimensions     */
//    int tLines;                         /* lines in item text       */  
//    int nLines;                         /* lines in menu area       */ 
    char *txt;                // Removed *t2, save;          /* item text & clip saver   */
    MfwIcnAttr * icn;                    /* item icon                */
    U8 oldFont = (U8)-1;                    /* save previous font       */ /*a0393213 compiler warnings removal - explicit casting done*/
    MfwMnuItem *mi;                     /* current item             */
  //  char  *t;      // Removed tt [64]             /* temp item string         */


    ma = mnu->curAttr;
    if (ma->font != (U8) -1)
        oldFont = dspl_SelectFontbyID(ma->font); /* setup font */

    ax = ma->area->px; ay = ma->area->py;
    aw = ma->area->sx; ah = ma->area->sy;

    mi = ma->items + mnu->lCursor[mnu->level];
    icn = mi->icon;
	txt =  mi->str;
 	resources_setColour(ma->mnuColour);

    if ((ma->mode & MNU_HDRFORMAT_STR)==0)  /* header is a text ID */
		txt =  (char*)MmiRsrcGetText((int)txt);  
 	checkPredraw(mnu,mi,&txt,&icn);     /* ES!!                     */
    {
    	MfwRect areaLeft;
    	mnuDrawHeader(ma,&areaLeft,txt);
    	ax = areaLeft.px; ay = areaLeft.py;
    	aw = areaLeft.sx; ah = areaLeft.sy;
    }
	if ((dspl_getDisplayType()==DSPL_COLOUR) &&  (ma->bgdBitmap != NULL))
	{
		MfwRect *bmpArea = &ma->bgdBitmap->area;
		if ((ma->bgdBitmap->icons != NULL) &&  (bmpArea->sx >0) && (bmpArea->sy >0))
		{
			fastCopyBitmap(		(int)bmpArea->px, (int)bmpArea->py, (int)bmpArea->sx, (int)bmpArea->sy,
									ma->bgdBitmap->icons,
									(int)ax,(int)ay, (int)aw, (int)ah, 
									dspl_GetBgdColour(),
									ma->bgdBitmap->icnType);			
		}
		else
		{
		    dspl_Clear(ax,ay,(U16)(ax+aw-1),(U16)(ay+ah-1));
		}			
	}
	else
	{
	    dspl_Clear(ax,ay,(U16)(ax+aw-1),(U16)(ay+ah-1));
	}
		
    

    if (icn != NULL)
    {                                   /* show associated icon     */
	dspl_BitBlt2(icn->area.px,icn->area.py,
                    icn->area.sx,icn->area.sy,
                    icn->icons,0,icn->icnType);

    	}
    if (oldFont != (U8) -1)
        dspl_SelectFontbyID(oldFont);   /* restore previous font    */

    checkPostdraw(mnu,mi);              /* ES!!                     */

#ifdef MFW_DEBUG_DISPLAY_SIZE
    dspl_DrawRect(ax,ay,(ax+aw-1),(ay+ah-1));
#endif    
	resources_restoreColour();
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : drawListMenu       |
+--------------------------------------------------------------------+

  PURPOSE : draws menu in list mode

  GW 05/10/01 - Changed single value 'lng' to 2 values indicating if ID's should be 
				used (useStrID) and if the strings are ascii or unicode (uesDefLang).
*/
#define MAX_STR_CHAR 64
static void drawListMenu (MfwMnu *mnu)
{
    MfwMnuAttr *ma;                     /* menu attributes          */
    U16 fh, fw;                         /* font height & width      */
    U16 ax, ay, aw, ah;                 /* menu area dimensions     */
    U16 x, x2=0, EOL_position=0, hfh=0;
    int nLines;
    int i;
    int nIdx;
    int nTextLen;
    MfwIcnAttr * icn;
//    int fit;    
    char *txtStr;                          /* item text                */
    char txt[MAX_STR_CHAR];
    U8 oldFont =(U8) -1;                    /* save previous font       */ /*a0393213 compiler warnings removal - explicit type casting done*/
	int simpleScrolling;
	int status;							/* SPR#998 - SH - Status for each menu item */
	const USHORT checkbox_width = 7;	/* SPR#998 - SH - Checkbox width in pixels */
	const USHORT checkbox_height = 7;	/* SPR#998 - SH - Checkbox height in pixels */
	const USHORT checkbox_spacing_left = 1;	/* SPR#998 - SH - Gap between left edge of menu and checkbox */
	const USHORT checkbox_spacing_right = 2; /* SPR#998 - SH - Gap between right edge of checkbox and menu text */
	USHORT checkbox_x;		/* SPR#998 - SH - X position of checkbox */
	USHORT checkbox_y;		 /* SPR#998 - SH - Y position of checkbox */

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	USHORT		titleIconWidth = 0;
	 /*a0393213 compiler warnings removal - iconX, iconY removed*/
#endif

	TRACE_EVENT("drawListMenu() start");	 /*a0393213 compiler warnings removal - TIME_TRACE_EVENT changed to TRACE_EVENT*/
    ma = mnu->curAttr;

	/* SPR#1983 - SH - Ignore language type now */
	
    if (ma->font != (U8) -1)
        oldFont = dspl_SelectFontbyID(ma->font); /* setup font */

	/* xreddymn Dec-16-2004 MMI-SPR-27384: Two line height list display */
	if((ma->mode & MNU_DISPLAY)== MNU_LIST_2_LINE)
	{
		hfh = dspl_GetFontHeight(); 	/* height of one line */
		fh = hfh<<1;					/* height of two lines */
	}
	else
		fh = dspl_GetFontHeight();
    fw = dspl_GetTextExtent("X",1); // ES!! only for normal fonts !
    
    //Display header info (if required)
    if ((ma->hdrId != 0) && (dspl_getDisplayType()==DSPL_COLOUR))
    {
    	MfwRect areaLeft;
    	char *hdrStr;
	    if ((ma->mode & MNU_HDRFORMAT_STR)==0)  /* header is a text ID */
			hdrStr =  MmiRsrcGetText(ma->hdrId);  
    	else
    		hdrStr = (char*)(ma->hdrId);
    	mnuDrawHeader(ma,&areaLeft,hdrStr);
    	ax = areaLeft.px; ay = areaLeft.py;
    	aw = areaLeft.sx; ah = areaLeft.sy;
    }
    // Sep 30, 2005  REF: CRR MMI-SPR-33781  xreddymn
    // Description: Menu header not displayed for Golite
    // Solution: Added functionality to display menu header
    // Display a different header in case of monochrome screens
    else if((ma->hdrId != 0) && (DSPL_BW == dspl_getDisplayType()))
    {
    	MfwRect areaLeft;
    	char *hdrStr;
    	if ((ma->mode & MNU_HDRFORMAT_STR) == 0)  /* header is a text ID */
    		hdrStr = MmiRsrcGetText(ma->hdrId);
    	else
    		hdrStr = (char*)(ma->hdrId);
    	mnuDrawHeaderBW(ma, &areaLeft, hdrStr);
    	ax = areaLeft.px; ay = areaLeft.py;
    	aw = areaLeft.sx; ah = areaLeft.sy;
    }
    else
    {
    	ax = ma->area->px; ay = ma->area->py;
    	aw = ma->area->sx; ah = ma->area->sy;
    }
    resources_setColour(ma->mnuColour);
    dspl_Clear(ax,ay,(U16)(aw+ax-1),(U16)(ah+ay-1));
    nLines = ah / fh;
	mnu->nLines = nLines; //Store the number of lines we are drawing for scrolling up/down
    {
//    	TRACE_EVENT_P5(" ax:%d ay:%d aw:%d ah:%d lines:%d",ax,ay,aw,ah, nLines);
    } 	
	if ((!mnu->scrollMode)||(countVisible(mnu,ma,ma->nItems)<=nLines)) /* Simple scrolling */
		simpleScrolling = TRUE;
	else
		simpleScrolling = FALSE;

	if (simpleScrolling)
	{
	    nIdx = (countVisible(mnu,ma,mnu->lCursor[mnu->level])/nLines)*nLines;
	}
	else
	{
		int count=mnu->lShift[mnu->level];
		int index= mnu->lCursor[mnu->level]+1;
		int nVisibles = countVisible(mnu,ma,ma->nItems);

		while (ma->items[index-1].flagFunc(mnu,ma,&(ma->items[index-1])) & MNU_ITEM_HIDE)
	    {
			index++;
        } 

		while (count>0){
			if (index<=0)
				index = ma->nItems-1;
			else
				index--;
	        if (!(ma->items[index].flagFunc(mnu,ma,&(ma->items[index]))& MNU_ITEM_HIDE))
				count--;								
		}
		if (nVisibles<nLines)
			nLines = nVisibles;
	    nIdx = index;	
	}

    for (i = 0; i < nLines; nIdx++)     /* ES!! not so nice...      */
    {
    	if (nIdx >= ma->nItems)
    	{
			if (simpleScrolling)
				break;
			else
				nIdx=0;				
		}
	
        if (ma->items[nIdx].flagFunc(mnu,ma,&(ma->items[nIdx]))
	                                            & MNU_ITEM_HIDE)
            continue;
	    if (mnu->useStrID)                   /* use language handler     */
			txtStr = (char*)MmiRsrcGetText((int)ma->items[nIdx].str);   //JVJE
    	else
            txtStr = ma->items[nIdx].str;
		icn = ma->items[nIdx].icon;

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
       if (icn != NULL)
	{
		if (icn->area.sx > TITLE_ICON_WIDTH) 
		{
			titleIconWidth = TITLE_ICON_WIDTH;
		}
		else
		{
			titleIconWidth = icn->area.sx;
		}
	}			
	else
	{
		titleIconWidth = 0;
	}
#endif

        checkPredraw(mnu,ma->items+nIdx,&txtStr,&icn); /* ES!!         */
		memcpy(txt,txtStr,MAX_STR_CHAR);
    	status = checkStatus(mnu,ma,ma->items+nIdx,txt); /* SPR#998 - SH - Store the resulting status  */

	    if ((mnu->useStrID) &&
	       ((ma->mode & MNU_DISPLAY) == MNU_LIST_COLOUR))
	    {
			resources_setColourMnuItem( (int)ma->items[nIdx].str );
	    }

		/* xreddymn Dec-16-2004 MMI-SPR-27384: For two line menu item display */
		if((ma->mode & MNU_DISPLAY)== MNU_LIST_2_LINE)
		{
			EOL_position=0;
#ifdef NO_ASCIIZ
			if(txt[0] == (char)0x80 || mnu->textDCS==MNU_LIST_LANGUAGE_UNICODE) /*a0393213 lint warnings removal - typecast done*/
 			{
				while((((U16*)txt)[EOL_position]!='\n') && (((U16*)txt)[EOL_position]!='\0')) EOL_position++;
				if(((U16*)txt)[EOL_position]=='\0')	/* Less than two lines to display */
					EOL_position=0;
				else									/* Two lines to display, break each line with a '\0' */
				{
					((U16*)txt)[EOL_position]='\0';
					EOL_position++;
				}
			}
			else
#endif
			{
				while((txt[EOL_position]!='\n') && (txt[EOL_position]!='\0')) EOL_position++;
				if(txt[EOL_position]=='\0')				/* Less than two lines to display */
					EOL_position=0;
				else									/* Two lines to display, break each line with a '\0' */
				{
					txt[EOL_position]='\0';
					EOL_position++;
				}
			}
			if(EOL_position>0)
			{
#ifdef NO_ASCIIZ
				if(txt[0] == (char)0x80 || mnu->textDCS==MNU_LIST_LANGUAGE_UNICODE) /*a0393213 lint warnings removal - typecast done*/
					nTextLen = dspl_GetTextExtent((char*)(txt+(EOL_position<<1)),(U16)dspl_str_length(txt));
				else
#endif
					nTextLen = dspl_GetTextExtent((char*)(txt+EOL_position),(U16)dspl_str_length(txt));

				/* SPR#998 - SH - If list is a checkbox list, include space for checkbox in width */	
				if (status==MNU_ITEM_CHECKED || status==MNU_ITEM_UNCHECKED)
				{
					nTextLen+=(checkbox_spacing_left+checkbox_width+checkbox_spacing_right);
				}
				/* end of SPR#998 */
			
		        if ((ma->mode & MNU_ALIGN) == MNU_CENTER)
			        x2 = (U16) (ax + (aw - nTextLen) / 2);
		        else if ((ma->mode & MNU_ALIGN) == MNU_RIGHT)
			        x2 = (U16) (ax + aw - nTextLen);
		        else
			        x2 = (U16) (ax + fw);        /* MNU_LEFT (default)       */
				if ((ma->mode & MNU_ITEMSEL) == MNU_CUR_LINE)
					x2 =ax;
			}
		}

        nTextLen = dspl_GetTextExtent(txt,(U16)dspl_str_length(txt));

		/* SPR#998 - SH - If list is a checkbox list, include space for checkbox in width */
		
		if (status==MNU_ITEM_CHECKED || status==MNU_ITEM_UNCHECKED)
		{
			nTextLen+=(checkbox_spacing_left+checkbox_width+checkbox_spacing_right);
		}

		/* end of SPR#998 */
		
        if ((ma->mode & MNU_ALIGN) == MNU_CENTER)
	        x = (U16) (ax + (aw - nTextLen) / 2);
        else if ((ma->mode & MNU_ALIGN) == MNU_RIGHT)
	        x = (U16) (ax + aw - nTextLen);
        else
	        x = (U16) (ax + fw);        /* MNU_LEFT (default)       */
		if ((ma->mode & MNU_ITEMSEL) == MNU_CUR_LINE)
			x =ax;

		/* SPR#998 - SH - Draw checkboxes if required */

		if (status==MNU_ITEM_CHECKED || status==MNU_ITEM_UNCHECKED)
		{	
			checkbox_x = x+checkbox_spacing_left;
			checkbox_y = ay+i*fh+(fh-checkbox_height)/2;
			
			if (status==MNU_ITEM_CHECKED)
			{
				/* NOTE: for some reason, a filled rect must have x2, y2 +1 of the same size
				 * of unfilled rect...compare parameters here and below */
				dspl_DrawFilledRect(checkbox_x, checkbox_y, checkbox_x+checkbox_width+1,
					checkbox_y+checkbox_height+1);
			}
			else
			{
				dspl_DrawRect(checkbox_x, checkbox_y, checkbox_x+checkbox_width,
					checkbox_y+checkbox_height);
			}
			x+=(checkbox_spacing_left+checkbox_width+checkbox_spacing_right); /* Text box now starts just to the right of checkbox */

			/* xreddymn Dec-16-2004 MMI-SPR-27384: For two line menu item display */
			x2+=(checkbox_spacing_left+checkbox_width+checkbox_spacing_right);
		}

		/* end of SPR#998 */
		
		{      
#ifdef NO_ASCIIZ
/*MC, use universal height rather than smaller chinese font height*/
/*MC, SPR1526, send  whole string to dspl_TextOut, rather than missing out unicode tag*/
/* SPR#1983 - SH - If unicode is set, send text as unicode */

			if (txt[0] == (char)0x80 || mnu->textDCS==MNU_LIST_LANGUAGE_UNICODE) /*a0393213 lint warnings removal - typecast done*/
			{
// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
				if (icn != NULL)
				{
					if(icn->selfExplanatory == FALSE)
					dspl_TextOut(titleIconWidth + 1 + x,(U16)ay+i*fh/*MC, 1319*/,DSPL_TXTATTR_UNICODE,txt);
				}
				else
					dspl_TextOut(x,(U16)ay+i*fh/*MC, 1319*/,DSPL_TXTATTR_UNICODE,txt);
#else
				dspl_TextOut(x,(U16)ay+i*fh/*MC, 1319*/,DSPL_TXTATTR_UNICODE,txt);
#endif
				if(EOL_position>0) /* xreddymn Dec-16-2004 MMI-SPR-27384: two line display */
				{
					dspl_TextOut(x2,(U16)ay+i*fh+hfh/*MC, 1319*/,DSPL_TXTATTR_UNICODE,(char*)(txt+(EOL_position<<1)));

				}
			}
			else
#endif
			{
// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
				if (icn != NULL)
				{
					if(icn->selfExplanatory == FALSE)
					dspl_TextOut(titleIconWidth + 1 + x,(U16)(ay+i*fh),DSPL_TXTATTR_CURRENT_MODE,txt);
				}
				else
					dspl_TextOut(x,(U16)(ay+i*fh),DSPL_TXTATTR_CURRENT_MODE,txt);
#else			
				dspl_TextOut(x,(U16)(ay+i*fh),DSPL_TXTATTR_CURRENT_MODE,txt);
#endif
				if(EOL_position>0) /* xreddymn Dec-16-2004 MMI-SPR-27384: two line display */
				{
					dspl_TextOut(x2,(U16)(ay+i*fh+hfh),DSPL_TXTATTR_CURRENT_MODE,(char*)(txt+EOL_position));
				}
			}	
		}

	    if (nIdx == mnu->lCursor[mnu->level])
        {
	        if ((ma->mode & MNU_ITEMSEL) == MNU_FRAMED)
            dspl_DrawRect(ax,(U16)(ay+i*fh),(U16)(aw-2+ax),
                                                (U16)(fh-1+ay+i*fh));
	        else if ((ma->mode & MNU_ITEMSEL) == MNU_CUR_ARROW)
            	dspl_TextOut(ax,(U16)(ay+i*fh),DSPL_TXTATTR_CURRENT_MODE,">");
            else if ((ma->mode & MNU_ITEMSEL) == MNU_CUR_STAR)
	            dspl_TextOut(ax,(U16)(ay+i*fh),DSPL_TXTATTR_CURRENT_MODE,"*");
            else if ((ma->mode & MNU_ITEMSEL) == MNU_CUR_BLOCK)
	            dspl_TextOut(ax,(U16)(ay+i*fh),
                            DSPL_TXTATTR_CURRENT_MODE|DSPL_TXTATTR_INVERS," ");
	        else if ((ma->mode & MNU_ITEMSEL) == MNU_CUR_LINE)
            {
				if (dspl_getDisplayType()==DSPL_BW)
				{
	       			dspl_DrawFilledColRect(x,ay+i*fh,aw,ay+(i+1)*fh,0x00FFFFFF);
#ifdef NO_ASCIIZ
					/* SPR#1983 - SH - If unicode is set, send text as unicode */
					if (txt[0] == (char)0x80 || mnu->textDCS==MNU_LIST_LANGUAGE_UNICODE) /*a0393213 lint warnings removal - typecast done*/
					{
						dspl_TextOut(x,(U16)ay+i*fh/*MC, 1319*/,DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,txt);
						if(EOL_position>0) /* xreddymn Dec-16-2004 MMI-SPR-27384: two line display */
						{
							dspl_TextOut(x2,(U16)ay+i*fh+hfh/*MC, 1319*/,DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)(txt+(EOL_position<<1)));
						}
					}
					else
#endif
					{
						dspl_TextOut(x,(U16)(ay+i*fh),DSPL_TXTATTR_CURRENT_MODE|DSPL_TXTATTR_INVERS,txt);
						if(EOL_position>0) /* xreddymn Dec-16-2004 MMI-SPR-27384: two line display */
						{
							dspl_TextOut(x2,(U16)(ay+i*fh+hfh),DSPL_TXTATTR_CURRENT_MODE|DSPL_TXTATTR_INVERS,(char*)(txt+EOL_position));
						}
					}
				}
				else
				{
					resources_setHLColour(ma->mnuColour);
	       			dspl_DrawFilledBgdRect(x,ay+i*fh,aw,ay+(i+1)*fh);
#ifdef NO_ASCIIZ
					/* SPR#1983 - SH - If unicode is set, send text as unicode */
					if (txt[0] == (char)0x80 || mnu->textDCS==MNU_LIST_LANGUAGE_UNICODE) /*a0393213 lint warnings removal-typecast done*/
					{
// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
				if (icn != NULL)
				{
						if(icn->selfExplanatory == FALSE)
						dspl_TextOut(titleIconWidth + x,(U16)ay+i*fh/*MC, 1319*/,DSPL_TXTATTR_UNICODE,txt);
				}
				else
					dspl_TextOut(x,(U16)ay+i*fh/*MC, 1319*/,DSPL_TXTATTR_UNICODE,txt);
#else
						dspl_TextOut(x,(U16)ay+i*fh/*MC, 1319*/,DSPL_TXTATTR_UNICODE,txt);
#endif
						if(EOL_position>0) /* xreddymn Dec-16-2004 MMI-SPR-27384: two line display */
						{
							dspl_TextOut(x2,(U16)ay+i*fh+hfh/*MC, 1319*/,DSPL_TXTATTR_UNICODE,(char*)(txt+(EOL_position<<1)));
						}
					}
					else
#endif
					{
// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
				if (icn != NULL)
				{
						if(icn->selfExplanatory == FALSE)
						dspl_TextOut(titleIconWidth + x,(U16)(ay+i*fh),DSPL_TXTATTR_CURRENT_MODE,txt);
				}
				else
					dspl_TextOut(x,(U16)(ay+i*fh),DSPL_TXTATTR_CURRENT_MODE,txt);
#else
						dspl_TextOut(x,(U16)(ay+i*fh),DSPL_TXTATTR_CURRENT_MODE,txt);
#endif
						if(EOL_position>0) /* xreddymn Dec-16-2004 MMI-SPR-27384: two line display */
						{
							dspl_TextOut(x2,(U16)(ay+i*fh+hfh),DSPL_TXTATTR_CURRENT_MODE,(char*)(txt+EOL_position));
						}
					}
					resources_restoreMnuColour();

				}
			}
            if (icn != NULL)// && icn->nIcons > nIdx)
	        {
// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
/* There is a problem when this icon is displayed. 
The last parameter was not taken into consideration & a default '0' was been set. This is corrected now.*/
/*    	        dspl_BitBlt2(icn->area.px, icn->area.py,
        	                icn->area.sx, icn->area.sy,
                            icn->icons, 0, icn->icnType);*/

		if ((icn->area.sx > LINE_ICON_WIDTH) || (icn->area.sy > fh))
		{
			   dspl_BitBlt2(icn->area.px, icn->area.py,
		   	                icn->area.sx, icn->area.sy, (void*)SATIconQuestionMark, 0, BMP_FORMAT_256_COLOUR);
		}
		else
		{
		        dspl_BitBlt2(icn->area.px, icn->area.py,
		   	                icn->area.sx, icn->area.sy,
		                    icn->icons, 0, icn->icnType);
		}
#else
    	        dspl_BitBlt2(icn->area.px,icn->area.py,
        	                icn->area.sx,icn->area.sy,
                            icn->icons,(U16)nIdx,0);
#endif
	        }
    	}
        i++;
	    checkPostdraw(mnu,ma->items+nIdx); /* ES!!                  */
    }	

    if (oldFont != (U8) -1)
        dspl_SelectFontbyID(oldFont);   /* restore previous font    */

    /* SPR#1983 - SH - No longer have to change text type back */
    
    resources_restoreColour();
#ifdef MFW_DEBUG_DISPLAY_SIZE
    dspl_DrawRect(ax,ay,(ax+aw-1),(ay+ah-1));
#endif    
    TRACE_EVENT("drawListMenu() end"); /*a0393213 warnings removal-TIME_TRACE_EVENT changed to TRACE_EVENT*/

}


static void drawIconsListMenu(MfwMnu *mnu)
{
    MfwMnuAttr *ma;                     /* menu attributes          */
    U16 fh, fw;                         /* font height & width      */
    U16 ax, ay, aw, ah;                 /* menu area dimensions     */
    U16 x;
    int nLines;
    int i;
    int nIdx;
    int nTextLen;
    MfwIcnAttr * icn;
     char *txtStr;                          /* item text                */
    char txt[MAX_STR_CHAR];
    U8 oldFont = (U8)-1;                    /* save previous font       */ /* x0020906 - Type Cast - 14-08-2006 */
 	int simpleScrolling;
	int icon_w=0;
	int icon_h=0;
	TRACE_FUNCTION("drawIconsListMenu()");	/* Replace TIME_TRACE_EVENT to TRACE_FUNCTION - x0020906 - Warning Correction */
    ma = mnu->curAttr;
	/* SPR#1983 - SH - Ignore language type now */
	
    if (ma->font != (U8) -1)
        oldFont = dspl_SelectFontbyID(ma->font); /* setup font */

	fh = dspl_GetFontHeight();
    fw = dspl_GetTextExtent("X",1); // ES!! only for normal fonts !
    
    //Display header info (if required)
    if ((ma->hdrId != 0) && (dspl_getDisplayType()==DSPL_COLOUR))
    {
    	MfwRect areaLeft;
    	char *hdrStr;
	    if ((ma->mode & MNU_HDRFORMAT_STR)==0)  /* header is a text ID */
			hdrStr =  MmiRsrcGetText(ma->hdrId);  
    	else
    		hdrStr = (char*)(ma->hdrId);
    	mnuDrawHeader(ma,&areaLeft,hdrStr);
    	ax = areaLeft.px; ay = areaLeft.py;
    	aw = areaLeft.sx; ah = areaLeft.sy;
    }
    // Sep 30, 2005  REF: CRR MMI-SPR-33781  xreddymn
    // Description: Menu header not displayed for Golite
    // Solution: Added functionality to display menu header
    // Display a different header in case of monochrome screens
    else if((ma->hdrId != 0) && (DSPL_BW == dspl_getDisplayType()))
    {
    	MfwRect areaLeft;
    	char *hdrStr;
    	if ((ma->mode & MNU_HDRFORMAT_STR) == 0)  /* header is a text ID */
    		hdrStr = MmiRsrcGetText(ma->hdrId);
    	else
    		hdrStr = (char*)(ma->hdrId);
    	mnuDrawHeaderBW(ma, &areaLeft, hdrStr);
    	ax = areaLeft.px; ay = areaLeft.py;
    	aw = areaLeft.sx; ah = areaLeft.sy;
    }
    else
    {
    	ax = ma->area->px; ay = ma->area->py;
    	aw = ma->area->sx; ah = ma->area->sy;
    }
    resources_setColour(ma->mnuColour);
    dspl_Clear(ax,ay,(U16)(aw+ax-1),(U16)(ah+ay-1));
    //nLines = ah / fh;
	if(ma->items[0].icon != NULL)
	{
		icon_w=ma->items[0].icon->area.sx+10;
	    	icon_h = ma->items[0].icon->area.sy;
	}
	else
	{
		icon_w = 0;
		icon_h = fh;
	}
     nLines=ah/icon_h;
	mnu->nLines= nLines;
	icon_h=ah/nLines;
	mnu->lineHeight = icon_h;
	mnu->nLines = nLines; //Store the number of lines we are drawing for scrolling up/down
	if ((!mnu->scrollMode)||(countVisible(mnu,ma,ma->nItems)<=nLines)) /* Simple scrolling */
		simpleScrolling = TRUE;
	else
		simpleScrolling = FALSE;

	if (simpleScrolling)
	{
	    nIdx = (countVisible(mnu,ma,mnu->lCursor[mnu->level])/nLines)*nLines;
	}
	else
	{
		int count=mnu->lShift[mnu->level];
		int index= mnu->lCursor[mnu->level]+1;
		int nVisibles = countVisible(mnu,ma,ma->nItems);

		while (ma->items[index-1].flagFunc(mnu,ma,&(ma->items[index-1])) & MNU_ITEM_HIDE)
	    {
			index++;
        } 

		while (count>0){
			if (index<=0)
				index = ma->nItems-1;
			else
				index--;
	        if (!(ma->items[index].flagFunc(mnu,ma,&(ma->items[index]))& MNU_ITEM_HIDE))
				count--;								
		}
		if (nVisibles<nLines)
			nLines = nVisibles;
	    nIdx = index;	
	}
	if(ma->items[0].icon != NULL)
	{
        ma->items[0].icon->area.px=ax;
        ma->items[0].icon->area.py=ay;
	}
    for (i = 0; i < nLines; nIdx++)     /* ES!! not so nice...      */
    {
    	if (nIdx >= ma->nItems)
    	{
			if (simpleScrolling)
				break;
			else
				nIdx=0;				
		}
        if (ma->items[nIdx].flagFunc(mnu,ma,&(ma->items[nIdx]))
	                                            & MNU_ITEM_HIDE)
            continue;
	    if (mnu->useStrID)                   /* use language handler     */
			txtStr = (char*)MmiRsrcGetText((int)ma->items[nIdx].str);   //JVJE
    	else
            txtStr = ma->items[nIdx].str;
		icn = ma->items[nIdx].icon;

        checkPredraw(mnu,ma->items+nIdx,&txtStr,&icn); /* ES!!         */
		memcpy(txt,txtStr,MAX_STR_CHAR);
    	checkStatus(mnu,ma,ma->items+nIdx,txt); /* SPR#998 - SH - Store the resulting status  */

	    if ((mnu->useStrID) &&
	       ((ma->mode & MNU_DISPLAY) == MNU_LIST_COLOUR))
	    {
			resources_setColourMnuItem( (int)ma->items[nIdx].str );
	    }

        nTextLen = dspl_GetTextExtent(txt,(U16)dspl_str_length(txt));
		
        if ((ma->mode & MNU_ALIGN) == MNU_CENTER)
	        x = (U16) (ax + (aw - nTextLen) / 2);
        else if ((ma->mode & MNU_ALIGN) == MNU_RIGHT)
	        x = (U16) (ax + aw - nTextLen);
        else
	        x = (U16) (ax + fw);        /* MNU_LEFT (default)       */
		if ((ma->mode & MNU_ITEMSEL) == MNU_CUR_LINE)
			x =ax;

		/* SPR#998 - SH - Draw checkboxes if required */

		

		/* end of SPR#998 */
		
		{      
#ifdef NO_ASCIIZ
/*MC, use universal height rather than smaller chinese font height*/
/*MC, SPR1526, send  whole string to dspl_TextOut, rather than missing out unicode tag*/
/* SPR#1983 - SH - If unicode is set, send text as unicode */

			if (txt[0] == (char)0x80 || mnu->textDCS==MNU_LIST_LANGUAGE_UNICODE)
			{
 				dspl_TextOut(x+icon_w,(U16)ay+i*icon_h/*MC, 1319*/,DSPL_TXTATTR_UNICODE,txt); 				
			}
			else
#endif
			{
 			
				dspl_TextOut(x+icon_w,(U16)(ay+i*icon_h),DSPL_TXTATTR_CURRENT_MODE,txt); 				
			}	
		}

	    if (nIdx == mnu->lCursor[mnu->level])
        {
	        if ((ma->mode & MNU_ITEMSEL) == MNU_FRAMED)
            dspl_DrawRect(ax,(U16)(ay+i*icon_h),(U16)(aw-2+ax),
                                                (U16)(icon_h-1+ay+i*icon_h));
	        else if ((ma->mode & MNU_ITEMSEL) == MNU_CUR_ARROW)
            	dspl_TextOut(ax+icon_w,(U16)(ay+i*icon_h),DSPL_TXTATTR_CURRENT_MODE,">");
            else if ((ma->mode & MNU_ITEMSEL) == MNU_CUR_STAR)
	            dspl_TextOut(ax+icon_w,(U16)(ay+i*icon_h),DSPL_TXTATTR_CURRENT_MODE,"*");
            else if ((ma->mode & MNU_ITEMSEL) == MNU_CUR_BLOCK)
	            dspl_TextOut(ax+icon_w,(U16)(ay+i*icon_h),
                            DSPL_TXTATTR_CURRENT_MODE|DSPL_TXTATTR_INVERS," ");
	        else if ((ma->mode & MNU_ITEMSEL) == MNU_CUR_LINE)
            {
				if (dspl_getDisplayType()==DSPL_BW)
				{
	       			dspl_DrawFilledColRect(x,ay+i*icon_h,aw,ay+(i+1)*icon_h,0x00FFFFFF);
#ifdef NO_ASCIIZ
					/* SPR#1983 - SH - If unicode is set, send text as unicode */
					if (txt[0] == (char)0x80 || mnu->textDCS==MNU_LIST_LANGUAGE_UNICODE)
					{
						dspl_TextOut(x+icon_w,(U16)ay+i*icon_h/*MC, 1319*/,DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,txt);
					}
					else
#endif
					{
						dspl_TextOut(x+icon_w,(U16)(ay+i*icon_h),DSPL_TXTATTR_CURRENT_MODE|DSPL_TXTATTR_INVERS,txt);
					}
				}
				else
				{
					resources_setHLColour(ma->mnuColour);
	       			dspl_DrawFilledBgdRect(x,ay+i*icon_h,aw,ay+(i+1)*icon_h);
#ifdef NO_ASCIIZ
					/* SPR#1983 - SH - If unicode is set, send text as unicode */
					if (txt[0] == (char)0x80 || mnu->textDCS==MNU_LIST_LANGUAGE_UNICODE)
					{
 
						dspl_TextOut(x+icon_w,(U16)ay+i*icon_h/*MC, 1319*/,DSPL_TXTATTR_UNICODE,txt);
					}
					else
#endif
					{
 
						dspl_TextOut(x+icon_w,(U16)(ay+i*icon_h),DSPL_TXTATTR_CURRENT_MODE,txt);

					}
					resources_restoreMnuColour();

				}
			}
            if (icn != NULL)// && icn->nIcons > nIdx)
	        {
                 dspl_BitBlt2(ax,ay+i*icon_h,
        	                icn->area.sx,icn->area.sy,
                            icn->icons,0,icn->icnType);           
 	        }
    	}
		else
		{
		if (icn != NULL){
				dspl_BitBlt2(ax,ay+i*icon_h,
        	                icn->area.sx,icn->area.sy,
                            icn->icons,0,icn->icnType);      		                    }
    	}
        i++;
	    checkPostdraw(mnu,ma->items+nIdx); /* ES!!                  */
    }	

    if (oldFont != (U8) -1)
        dspl_SelectFontbyID(oldFont);   /* restore previous font    */

    
    resources_restoreColour();
#ifdef MFW_DEBUG_DISPLAY_SIZE
    dspl_DrawRect(ax,ay,(ax+aw-1),(ay+ah-1));
#endif    

}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : drawFreeMenu       |
+--------------------------------------------------------------------+

  PURPOSE : draws menu in overlapped mode

*/
static void drawFreeMenu (MfwMnu *mnu)
{
// ES!! to be revised
//kk needs font entry
    int nFontHeight;
    int i, x;
    int nIdx;
    int nTextLen;
    MfwIcnAttr * icn;

    MfwMnuAttr *wa;                     /* working attribute        */
    wa = mnu->curAttr;
    dspl_Clear(wa->area->px,wa->area->py,
                (U16)(wa->area->sx+wa->area->px-1),
                (U16)(wa->area->sy+wa->area->py-1));
    nFontHeight = dspl_GetFontHeight();
    nTextLen = dspl_GetTextExtent(wa->items[mnu->lCursor[mnu->level]].str,
        (U16)dspl_str_length(wa->items[mnu->lCursor[mnu->level]].str));

    if ((wa->mode & MNU_ALIGN) == MNU_CENTER)
        x = wa->area->px + (wa->area->sx - nTextLen) / 2;
    else if ((wa->mode & MNU_ALIGN) == MNU_RIGHT)
        x = wa->area->px + wa->area->sx - nTextLen;
    else
        x = wa->area->px;
 
    dspl_TextOut((U16)x,(U16)(wa->area->py+wa->area->sy-nFontHeight),DSPL_TXTATTR_CURRENT_MODE,
                 wa->items[mnu->lCursor[mnu->level]].str);

    for (i = 0; i < wa->nItems; i++)
    {
        nIdx = (mnu->lCursor[mnu->level] + i + 1) % wa->nItems;
        icn = wa->items[nIdx].icon;

        if (icn != NULL)
            dspl_BitBlt(icn->area.px,icn->area.py,
                        icn->area.sx,icn->area.sy,
                        0/*icn->usIndex*/,icn->icons,0);
    }
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuCommand         |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

static int mnuCommand (U32 cmd, void *h)
{
    switch (cmd)
    {
        case MfwCmdDelete:              /* delete me                */
            if (!h)
                return 0;
            mnuDelete(h);
            return 1;
        case MfwCmdUpdate:              /* repaint                  */
            if (!h || ((MfwHdr *) h)->type != MfwTypMnu)
                return 0;
            mnuUpdate(((MfwHdr *) h)->data);
            return 1;
        default:
            break;
    }

    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuScrollMode      |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

void mnuScrollMode (MfwHnd menu,U8 mode)
{
	MfwMnu *mnu;
	
    if ((mnu = mfwControl(menu)) == 0)
        return;
	mnu->scrollMode = mode;	
}


void mnuInitDataItem(MfwMnuItem* mnuItem)
{
	mnuItem->icon = 0;
	mnuItem->exta = 0;
	mnuItem->menu = 0;
	mnuItem->func = 0;	
    mnuItem->str = NULL;                       
    mnuItem->flagFunc = 0; 

}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : mnuIdentify           |
+--------------------------------------------------------------------+

  PURPOSE : SPR998 - SH - Identify menu option.  Returns the index of the option, or -1
  if option is not found.

*/

int mnuIdentify(struct MfwMnuAttrTag *attr, struct MfwMnuItemTag *item)
{
	int index;
	int currentOption;
	
	/* Identify current menu option */

	currentOption = -1;
	
	for (index=0; index < attr->nItems; index++)
	{
		if (&attr->items[index]==item)
			currentOption = index;
	}

	return currentOption;
}


void MMI_TRACE_P1(char* str, int err)
{
	TRACE_EVENT_P2("%s:%d",str,err);
}

/* START: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
/*******************************************************************************

 $Function:    	Menu_edit_TitleTimerCb

 $Description:	Callback function for the Menu title timer.  Recalculates title_pos so that
 				the next part of the string is displayed.
 
 $Returns:	None.

 $Arguments:	event	- the event type
 			timer	- the timer
 
*******************************************************************************/
static int Menu_edit_TitleTimerCb (MfwEvt e, MfwTim *t)
{	
	TRACE_FUNCTION("Menu_edit_TitleTimerCb");

	if (g_title_next_pos!=g_title_pos)
		g_title_pos = g_title_next_pos;
	winShow(g_win);
	timStart(g_title_timer);
	
    	return 1;
}
/* END: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */

