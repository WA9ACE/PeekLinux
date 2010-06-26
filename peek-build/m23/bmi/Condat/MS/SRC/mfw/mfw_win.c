/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_win.c       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 21              $|
| CREATED: 21.09.98                     $Modtime:: 23.03.00 9:08    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_WIN

   PURPOSE : window handling functions

   EXPORT  :

   TO DO   :

   $History:: mfw_win.c                                             $

    Oct 09, 2006 DR: OMAPS00095308 - xreddymn
    Description: mfwFocus is NULL when window callback function for MfwWinResume
    is invoked in winDelete.
    Solution: After deleting a window and all its components, set mfwFocus to
    the previous window, before calling window callback function for
    MfwWinResume.

	Mar 30, 2005	REF: CRR 29986	xpradipg
	Description: Optimisation 1: Removal of unused variables and dynamically
	allocate/ deallocate mbndata
	Solution: removed the unused varialbe mfw_twh_strlen
  
 * textMessage
 * *****************  Version 21  *****************
 * User: Es           Date: 23.03.00   Time: 14:42
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Added 'winUnhide()': no update(), no event.
 *
 * *****************  Version 20  *****************
 * User: Kk           Date: 14.03.00   Time: 15:52
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * changed update sequence
 *
 * *****************  Version 19  *****************
 * User: Es           Date: 3.03.00    Time: 12:04
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * winUpdate(): first update windows children, then! call event
 * handler
 *
 * *****************  Version 18  *****************
 * User: Es           Date: 18.02.00   Time: 17:46
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * danger in winUpdate
 *
 * *****************  Version 17  *****************
 * User: Nm           Date: 17.02.00   Time: 9:23
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * updated winIsFocussed
 *
 * *****************  Version 16  *****************
 * User: Kk           Date: 17.01.00   Time: 8:56
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * added simple check functions for easy checking if win is visible,
 * focussed, closed
 *
 * *****************  Version 15  *****************
 * User: Kk           Date: 10.01.00   Time: 13:46
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * some changes done by ES
 *
 * *****************  Version 3  *****************
 * User: Rm           Date: 12/22/99   Time: 4:34p
 * Updated in $/GSM/Condat/SND-MMI/MFW
 *
 * *****************  Version 2  *****************
 * User: Es           Date: 22.11.99   Time: 10:30
 * Updated in $/GSM/Condat/SND-MMI/MFW
 *
 * *****************  Version 1  *****************
 * User: Es           Date: 18.11.99   Time: 16:35
 * Created in $/GSM/Condat/SND-MMI/MFW
 * Initial
 *
 * *****************  Version 13  *****************
 * User: Es           Date: 6.07.99    Time: 12:37
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 11  *****************
 * User: Es           Date: 14.04.99   Time: 17:34
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * moved to CST
|
| *****************  Version 10  *****************
| User: Le           Date: 14.04.99   Time: 9:51
| Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 9  *****************
 * User: Es           Date: 1.04.99    Time: 17:07
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * removed lots of traces
 *
 * *****************  Version 8  *****************
 * User: Es           Date: 20.02.99   Time: 19:24
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 7  *****************
 * User: Es           Date: 20.02.99   Time: 18:22
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 6  *****************
 * User: Es           Date: 20.02.99   Time: 15:37
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 5  *****************
 * User: Es           Date: 18.02.99   Time: 17:01
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 4  *****************
 * User: Es           Date: 17.02.99   Time: 19:11
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 3  *****************
 * User: Es           Date: 14.01.99   Time: 17:19
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 2  *****************
 * User: Es           Date: 23.12.98   Time: 16:19
 * Updated in $/GSM/DEV/MS/SRC/MFW
*/


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

#include "mfw_mfw.h"
#include "mfw_sys.h"
#include "gdi.h"
#include "dspl.h"
#include "mfw_win.h"

static MfwHdr *autoFocus;               /* focus of toplevel window */
static U8 winUseAutoFocus = 0;          /* automatic focussing flag */
static int winCommand (U32 cmd, void *h);



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winInit            |
+--------------------------------------------------------------------+

  PURPOSE : initialize window handler

*/

MfwRes winInit (U16 *sx, U16 *sy)
{
    dspl_DevCaps displayData;

    dspl_Init();                        /* init display driver      */

    displayData.DisplayType = DSPL_TYPE_GRAPHIC;
    dspl_SetDeviceCaps(&displayData);
    dspl_GetDeviceCaps(&displayData);
    *sx = displayData.Width;
    *sy = displayData.Height;
    winUseAutoFocus = 0;

    mfwCommand[MfwTypWin] = (MfwCb) winCommand;

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winExit            |
+--------------------------------------------------------------------+

  PURPOSE : finalize window handler

*/

MfwRes winExit (void)
{
    mfwCommand[MfwTypWin] = 0;
    dspl_Exit();                        /* finit display driver     */

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winCreate          |
+--------------------------------------------------------------------+

  PURPOSE : create window control

*/

MfwHnd winCreate (MfwHnd w, MfwWinAttr *a, MfwEvt e, MfwCb f)
{


    MfwHdr *hdr = (MfwHdr *) mfwAlloc(sizeof(MfwHdr));
    MfwWin *win = (MfwWin *) mfwAlloc(sizeof(MfwWin));
    MfwHdr *eoc = (MfwHdr *) mfwAlloc(sizeof(MfwHdr));
    MfwHdr *insert_status =0;

    /* xreddymn Aug-09-2004, MMI-SPR-23964 (TII_MMS33) 
     * Implemented suspend and resume functionality for MFW windows
     * The following code posts MfwWinSuspend event
     */
    MfwWin *ee;
    MfwHnd window;
    window = mfwParent(mfwFocus);
    if(window)
    {
    	ee = (MfwWin*) ((MfwHdr *) window)->data;
    	if(ee && (((MfwHdr *) window)->type == MfwTypWin))
    	{
    		if (ee->handler && ee->mask & MfwWinSuspend)
    		{
	    		/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
    			(void)(ee->handler(MfwWinSuspend,ee));
    		}
    	}
    }
    if (!hdr || !win || !eoc)
    	{
    	TRACE_ERROR("ERROR: winCreate() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));

   		if(win)
   			mfwFree((U8*)win,sizeof(MfwWin));

   		if(eoc)
   			mfwFree((U8*)eoc,sizeof(MfwHdr));
   		
	   	return 0;
    }

    eoc->next = hdr;                    /* setup eoc element        */
    eoc->type = MfwTypMax;
    eoc->data = 0;

    win->mask = e;                      /* setup window control     */
    win->flags = 0;
    win->handler = f;
    win->attr = a;
    win->elems = eoc;
    win->user = 0;

    hdr->data = win;                    /* setup header element     */
    hdr->type = MfwTypWin;

    if (mfwSignallingMethod == 0)
        insert_status = mfwInsert(w,hdr);
    else
	{
        insert_status = mfwInsert(mfwParent(mfwFocus),hdr);
        mfwSetFocus(eoc);
	}

	/* Release memory if handler installation failed. */
	if(!insert_status)
	{
  		TRACE_ERROR("ERROR: winCreate() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)win,sizeof(MfwWin));
   		mfwFree((U8*)eoc,sizeof(MfwHdr)); 
		return 0;
  	}
  	
  	return insert_status;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winDelete          |
+--------------------------------------------------------------------+

  PURPOSE : delete window control

*/

MfwRes winDelete (MfwHnd w)
{
    MfwCb f;
    MfwHdr *e, *t, *r;
    MfwRes res;

    /*NM, p012a*/
    MfwWin *parent_controlBlock;
    MfwHdr  *parent_win;
	/*    U8 uMode;
	 p012a end*/

	TRACE_FUNCTION("win_delete()");

    if (!w)
        return MfwResIllHnd;
    if (((MfwHdr *) w)->type != MfwTypWin)
        return MfwResIllHnd;            /* element is not a window  */

    e = ((MfwHdr *) w)->data;
    if (!e)
        return MfwResErr;

    ((MfwWin *) e)->flags |= MfwWinDelete;
    if (((MfwWin *) e)->handler)
        if (((MfwWin *) e)->mask & MfwWinDelete)
            (void)(((MfwWin *) e)->handler(MfwWinDelete,e));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/

    e = ((MfwWin *) e)->elems;
    if (!e)
        return MfwResErr;

    if (mfwFocus == e)                  /* was focus window         */
        mfwSetFocus(0);                   /* no focus anymore         */



    while (e && e->type != MfwTypMax)   /* try to delete all elems  */
    {
        t = e->next;
        if (e->data)
        {
            if (mfwSignallingMethod != 0 && e->type == MfwTypWin)
            {
                r = (MfwHdr*) mfwParent(w);
                mfwRemove(e);           /* move up child window     */
                /* xreddymn OMAPS00095308 Oct-09-2006
                 * Check if r is valid.
                 */                
                if (r) 
                {
                mfwAppend(&(((MfwWin*)(r->data))->elems),e);
            }
            else
            {
                    mfwAppend(0,e);
                }
            }
            else
            {
                f = mfwCommand[e->type];
                if (f)
                    (void)(f(MfwCmdDelete,e));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
            }
        }
        e = t;
    }
    res = (mfwRemove(w)) ? MfwResOk : MfwResIllHnd;


	/*NM, p012d 
 	old: after deleting the focus-win start from root to top
   	new: after deleting the focus-win just give the new focus to 
   	      his parent-win (faster)
   	*/
   if (mfwSignallingMethod != 0 && mfwRoot && (!mfwFocus))
    {
	   parent_win = (MfwHdr*) mfwParent(w);

   		if (((MfwHdr *) parent_win)->type == MfwTypWin)
		{
		    parent_controlBlock = ((MfwHdr *) parent_win)->data;      

    /* xreddymn Aug-09-2004, MMI-SPR-23964 (TII_MMS33)
     * Implemented suspend and resume functionality for MFW windows
     * The following code posts MfwWinResume event
     */
    		if (parent_controlBlock->handler && parent_controlBlock->mask & MfwWinResume)
    		{
                    /* xreddymn OMAPS00095308 Oct-09-2006
                     * Set new value for mfwFocus before invoking the window
                     * handler for MfwWinResume.
                     */
                    mfwFocus = (MfwHdr*)((MfwWin*)parent_controlBlock)->elems;
                    (void)(parent_controlBlock->handler(MfwWinResume, parent_controlBlock));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
                }

	        winUpdate(parent_controlBlock);
	        mfwSetFocus(autoFocus);
		}

	}



    mfwFree((MfwHnd) e,sizeof(MfwHdr));
    mfwFree(((MfwHdr *) w)->data,sizeof(MfwWin));
    mfwFree(w,sizeof(MfwHdr));

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winShow            |
+--------------------------------------------------------------------+

  PURPOSE : show window (put in front of visibles)

*/
//TISH current_mfw_elem should not changed after winShow
EXTERN MfwHdr * current_mfw_elem;

MfwRes winShow (MfwHnd w)
{
    MfwWin *win;
	MfwHdr * temp_current_mfw_elem;

	TRACE_FUNCTION("winShow()");

    if (!w)
        return MfwResIllHnd;            /* window does not exist    */
	
    if (((MfwHdr *) w)->type != MfwTypWin)
        return MfwResIllHnd;            /* element is not a window  */
	
    win = ((MfwHdr *) w)->data;         /* get control block        */

	temp_current_mfw_elem = current_mfw_elem;
    mfwAppend(mfwRemove(w),w);          /* to front (draw at last)  */
	current_mfw_elem = temp_current_mfw_elem;
    win->flags |= MfwWinVisible;        /* window is visible        */

    winUpdate(win);                     /* draw window elements     */
    if (winUseAutoFocus)                /* automatic focussing      */
        mfwSetFocus(autoFocus);           /* determined by update()   */

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winHide            |
+--------------------------------------------------------------------+

  PURPOSE : hide window (and redraw remaining)

*/

MfwRes winHide (MfwHnd w)
{
    MfwHdr *r;
    MfwWin *win;

    if (!w)
        return MfwResIllHnd;            /* element does not exist   */
    if (((MfwHdr *) w)->type != MfwTypWin)
        return MfwResIllHnd;            /* element is not a window  */

    win = ((MfwHdr *) w)->data;
    win->flags &= ~MfwWinVisible;       /* window is not visible    */
    if (win->handler)                   /* call event handler       */
        if (win->mask & MfwWinVisible)
            (void)(win->handler(MfwWinVisible,win));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
    r = mfwRoot;                        /* the beginning            */
    autoFocus = 0;                      /* reset focus              */
    while (r->type != MfwTypMax)        /* more links in chain      */
    {
        if (r->type == MfwTypWin)       /* link is a window         */
            winUpdate(r->data);
        r = r->next;
    }
    if (winUseAutoFocus)                /* automatic focussing      */
        mfwSetFocus(autoFocus);           /* determined by update()   */

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winUnhide          |
+--------------------------------------------------------------------+

  PURPOSE : unhide window (without redrawing, no event)

*/

MfwRes winUnhide (MfwHnd w)
{
    MfwWin *win;

    if (!w)
        return MfwResIllHnd;            /* element does not exist   */
    if (((MfwHdr *) w)->type != MfwTypWin)
        return MfwResIllHnd;            /* element is not a window  */

    win = ((MfwHdr *) w)->data;
    win->flags |= MfwWinVisible;        /* window will be visible   */

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winFocus           |
+--------------------------------------------------------------------+

  PURPOSE : assign input/event focus to window

*/

MfwHnd winFocus (MfwHnd w)
{
    MfwWin *wNew, *wOld = 0;

    if (winUseAutoFocus)                /* automatic focussing      */
        return 0;                       /* ES!! only for testing !! */
    if (!w)
    {
        w = mfwFocus;
        mfwSetFocus(0);                   /* delete focus             */
        return w;                       /* return old focus         */
    }

    if (((MfwHdr *) w)->type != MfwTypWin)
        return 0;                       /* element is not a window  */

    if (mfwFocus)
    {
        while (mfwFocus->type != MfwTypMax)
            mfwSetFocus(mfwFocus->next);  /* search focus root        */
        mfwSetFocus(mfwFocus->next);      /* the focus window         */
        if (mfwFocus->type != MfwTypWin)
        {
            mfwSetFocus(0);               /* serious error:           */
            return 0;                   /* element is not a window  */
        }
        wOld = mfwFocus->data;          /* window control block     */
        wOld->flags &= ~MfwWinFocussed; /* input focus / deselected */
    }

    wNew = ((MfwHdr *) w)->data;
    w = mfwFocus;                       /* save old focus           */
    mfwSetFocus(wNew->elems);             /* focus on window elements */
    wNew->flags |= MfwWinFocussed;      /* input focus / selected   */

    if (wNew->handler)                  /* call new event handler   */
        if (wNew->mask & MfwWinFocussed)
            (void)(wNew->handler(MfwWinFocussed,wNew));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
    if (wOld && wOld->handler)          /* call old event handler   */
        if (wOld->mask & MfwWinFocussed)
            (void)(wOld->handler(MfwWinFocussed,wOld));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/

    return w;                           /* return old focus         */
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winAutoFocus       |
+--------------------------------------------------------------------+

  PURPOSE : set auto focus mode

*/

U8 winAutoFocus (U8 useit)
{
    U8 winUseAutoFocusOld = winUseAutoFocus; /* save previous state */

    winUseAutoFocus = useit;

    return winUseAutoFocusOld;          /* return previous state    */
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winClear           |
+--------------------------------------------------------------------+

  PURPOSE : clear window

*/

MfwRes winClear (MfwHnd w)
{
    MfwWin *win;

    if (!w)
    {
        dspl_ClearAll();                /* clear screen             */
        return MfwResOk;
    }

    win = ((MfwHdr *) w)->data;         /* clear window area        */
    dspl_Clear(win->attr->win.px,win->attr->win.py,
               (U16) (win->attr->win.sx+win->attr->win.px-2),
               (U16) (win->attr->win.sy+win->attr->win.py-2));

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winUpdate          |
+--------------------------------------------------------------------+

  PURPOSE : draw all visible windows

*/

MfwRes winUpdate (MfwWin *w)
{
    MfwCb f = 0;                        /* modules command handler  */
    MfwHdr *e;                          /* windows elements         */

//	TRACE_FUNCTION("winUpdate()");

    if (!w)
        return MfwResIllHnd;

    if (mfwSignallingMethod == 0)
    {
        if (!(w->flags & MfwWinVisible))
            return MfwResOk;                /* hidden window            */

        if (w->handler)                     /* THEN: call event handler */
            if (w->mask & MfwWinVisible)
                (void)(w->handler(MfwWinVisible,w));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
        e = w->elems;                       /* FIRST: update children   */
        autoFocus = e;                      /* gets focus of top window */
        while (e->type != MfwTypMax)        /* window has elements      */
        {
            if (e->type > MfwTypNone && e->type < MfwTypMax)
            {
                f = mfwCommand[e->type];
                if (f)                      /* cmd function implemented */
                    (void)(f(MfwCmdUpdate,e));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
            }
            e = e->next;
        }
    }
    else
    {
        if (w->handler && (w->flags & MfwWinVisible))
            if (w->mask & MfwWinVisible){
                (void)(w->handler(MfwWinVisible,w));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
            }

        e = w->elems;                       /* FIRST: update children   */
        autoFocus = e;                      /* gets focus of top window */
        while (e && (e->type != MfwTypMax))        /* OMAPS00145866 Added Null pointer check by Sameer  */
        {
            if (e->type > MfwTypNone && e->type < MfwTypMax)
            {
                f = mfwCommand[e->type];
                if (f && ((w->flags & MfwWinVisible)
                                || (e->type == MfwTypWin)))
                    (void)(f(MfwCmdUpdate,e));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
            }
            e = e->next;
        }
    }

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winNext            |
+--------------------------------------------------------------------+

  PURPOSE : show next window (for testing purposes only)

*/

MfwHnd winNext (MfwHnd winCur)
{
    MfwHdr *cw;                         /* current window pointer   */

    if (!winCur)
        return 0;

    cw = ((MfwWin *) (((MfwHdr *) winCur)->data))->elems;
    while (cw->type != MfwTypWin)
    {
        if (cw->type == MfwTypMax)
        {
            cw = cw->next;
            if (!cw)
                cw = mfwRoot;
            else
                cw = cw->next;          /* windows successor        */
        }
        else
            cw = cw->next;
    }

    return cw;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winPrev            |
+--------------------------------------------------------------------+

  PURPOSE : show previous window (for testing purposes only)

*/

MfwHnd winPrev (MfwHnd winCur)
{
    MfwHnd wo, wc;

    wo = wc = winNext(winCur);
    while (wc != winCur)
    {
        wo = wc;
        wc = winNext(wc);
    }

    return wo;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winCommand         |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

static int winCommand (U32 cmd, void *h)
{
    switch (cmd)
    {
        case MfwCmdDelete:              /* delete me                */
            if (!h)
                return 0;
            winDelete(h);
            return 1;
        case MfwCmdUpdate:              /* repaint                  */
            if (!h || ((MfwHdr *) h)->type != MfwTypWin)
                return 0;
            winUpdate(((MfwHdr *) h)->data);
            return 1;
        default:
            break;
    }

    return 0;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winIsVisible       |
+--------------------------------------------------------------------+

  PURPOSE : check if window is visible

*/
int winIsVisible(MfwHnd w)
{
    MfwWin *win;

    if (!w)
        return 0;                       /* no handle                */

    if (((MfwHdr *) w)->type != MfwTypWin)
        return 0;                       /* element is not a window  */

    win = ((MfwHdr *) w)->data;

    return (win->flags & MfwWinVisible);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winIsFocussed      |
+--------------------------------------------------------------------+

  PURPOSE : check if window is focussed

*/
int winIsFocussed(MfwHnd w)
{
    MfwWin *win;

    if (!w)
        return 0;                       /* no handle                */

    if (((MfwHdr *) w)->type != MfwTypWin)
        return 0;                       /* element is not a window  */

    win = ((MfwHdr *) w)->data;

    return (win->elems == mfwFocus);

}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : winIcClosed        |
+--------------------------------------------------------------------+

  PURPOSE : check if window is closed

*/
int winIsClosed(MfwHnd w)
{
    MfwWin *win;

    if (!w)
        return 0;                       /* no handle                */

    if (((MfwHdr *) w)->type != MfwTypWin)
        return 0;                       /* element is not a window  */

    win = ((MfwHdr *) w)->data;

    return (win->flags & MfwWinClosed);
}

MfwHdr* getHdr( MfwHnd win)
{
	return((MfwHdr *) win);
}
MfwWin* getWinData( MfwHnd win)
{
	MfwHdr* winHdr;
	winHdr = getHdr(win);
	return (winHdr->data);
}

#define TWH_MAX_STR_LEN 90
#define TWH_MAX_SINGLE_SIZE	21		// This is the length of the Level, Address, Type and Callback in hex plus a zero terminator

static unsigned char trc_lvl;
static char mfw_twh_str[TWH_MAX_STR_LEN];

//	Mar 30, 2005	REF: CRR 29986	xpradipg
//	remove the definition since its not used
#ifndef FF_MMI_OPTIM
static unsigned int mfw_twh_strlen;
#endif

static void mfw_twhNext(MfwHdr *next);
static char *mfw_twhType(MfwTyp type);
static void mfw_twhOut(void *ptr, MfwTyp typ);
static void mfw_twhCb(void *ptr);
static void mfw_twh_str_purge(void);

#include <string.h>

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_WIN            |
| STATE   : code                        ROUTINE : mfw_traceWinHdrs   |
+--------------------------------------------------------------------+

  PURPOSE	: This function will trace out the entire window tree.
  			: EXTREME care should be taken using this function as it will cause a
  			: massive overhead on the CPU and processing WILL be affected!!!!

*/
void mfw_traceWinHdrs(void)
{
	TRACE_FUNCTION(">>>>> mfw_traceWinHdrs");

	trc_lvl = 0;

	TRACE_EVENT("mfwRoot");

	if (mfwRoot)
		mfw_twhNext(mfwRoot);

	/*
	** Trace out the last remaining line (there will always be one)
	*/
	mfw_twh_str_purge();

	TRACE_FUNCTION("<<<<< mfw_traceWinHdrs");
	return;
}

static void mfw_twhNext(MfwHdr *next)
{
	while (next)
	{
		if (next->type != MfwTypMax)
		{
			/*
			** Trace out this address, and its Type
			*/
			mfw_twhOut(next, next->type);

			/*
			** If this type is a Window, then it may have sub-elements
			** Trace them out before continuing the tree on this level
			*/
			if (next->type == MfwTypWin)
			{
				MfwWin *win_ptr = (MfwWin *)next->data;

				if (win_ptr)
				{
					MfwHdr *win_elems = win_ptr->elems;

					/*
					** Add the Callback Address before doing the sub-elements
					*/
					mfw_twhCb((void*)win_ptr->handler);
					trc_lvl++;
					mfw_twhNext(win_elems);
					trc_lvl--;
				}
			}

			if (next->next == NULL)
			{
				mfw_twhOut(0, MfwTypNone); /*a0393213 compiler warnings removal - 0 changed to MfwTypNone*/
			}

			next = next->next;
		}
		else
		{
			/*
			** Trace out this address, and its Type
			*/
			mfw_twhOut(next, next->type);

			next = NULL;
		}
	}
		
	return;
}

static char *mfw_twhType(MfwTyp type)
{
	switch (type)
	{
    	case MfwTypNone:                         /* no element (first !!)    */
    		return("MfwTypNone");
//    		break;  

    	case MfwTypWin:                          /* window                   */
    		return("MfwTypWin");
//    		break;  

		case MfwTypEdt:                          /* editor                   */
			return("MfwTypEdt");
//			break;  

    	case MfwTypMnu:                          /* menu handler             */
    		return("MfwTypMnu");
//    		break;  

		case MfwTypIcn:                          /* icon handler             */
			return("MfwTypIcn");
//			break;  

		case MfwTypKbd:                          /* keyboard                 */
			return("MfwTypKbd");
//			break;  

		case MfwTypTim:                          /* timer                    */
			return("MfwTypTim");
//			break;  

		case MfwTypMme:                          /* misc. mobile equipment   */
			return("MfwTypMme");
//			break;  

		case MfwTypVmm:                          /* MDL virtual machine      */
			return("MfwTypVmm");
//			break;  

		case MfwTypNm:                           /* network                  */
			return("MfwTypNm");
//			break;  

		case MfwTypSim:                          /* SIM card                 */
			return("MfwTypSim");
//			break;  

		case MfwTypCm:                           /* call handling            */
			return("MfwTypCm");
//			break;  

		case MfwTypPhb:                          /* phonebook handling       */
			return("MfwTypPhb");
//			break;  

		case MfwTypSms:                          /* short message handling   */
			return("MfwTypSms");
//			break;  

		case MfwTypSs:                           /* supl. services handling  */
			return("MfwTypSs");
//			break;  

		case MfwTypSat:                          /* SIM Toolkit handling     */
			return("MfwTypSat");
//			break;  

		case MfwTypLng:                          /* language handling        */
			return("MfwTypLng");
//			break;  

		case MfwTypInp:                          /* input handling           */
			return("MfwTypInp");
//			break; 

		case MfwTypNode:                         /* node element             */
			return("MfwTypNode");
//			break; 

		case MfwTypCphs:                         /* CPHS element             */
			return("MfwTypCphs");
//			break; 

		case MfwTypGprs:                         /* GPRS handling            */
			return("MfwTypGprs");
//			break;  

		case MfwTypEm:							/*MC, SPR 1209 Eng Mode handling*/
			return("MfwTypEm");
//			break; 

#ifdef BT_INTERFACE
		case MfwTypBt:                           /* bluetooth handling       */
			return("MfwTypBt");
//			break;  
#endif

#ifdef BTE_MOBILE
		case MfwTypBte:                           /* bluetooth handling       */
			return("MfwTypBte");
//			break;		
#endif

		case MfwTypMax:                          /* end of types (last !!)   */
			return("MfwTypMax");
//			break;  

		default:
			return("Not Known");
	}
}

static void mfw_twhOut(void *ptr, MfwTyp typ)
{
	unsigned char i;
	char	fmt_str[30];

	memset(fmt_str, 0, 30);
	
#if 0
	/* The code here will output the information in hex format which will need
    ** more decoding, but which may be necessary if the amount of information
    ** being output causes problems in the BMI. Change the above #if to be '#if 1'
    ** to use this format
    */
	sprintf(fmt_str,"%02x%08lx%02x", trc_lvl, ptr, typ);

	/*
	** Determine whether a new string is required
	*/
	if (mfw_twh_strlen > (TWH_MAX_STR_LEN - TWH_MAX_SINGLE_SIZE))
	{
		/*
		** Need to output the previous trace and start a new line
		*/
		mfw_twh_str_purge();
	}
		
	strcat(mfw_twh_str, fmt_str);

	mfw_twh_strlen += 12;
#else

	memset(mfw_twh_str, 0, TWH_MAX_STR_LEN);
	
	for (i=0; i<trc_lvl; i++)
	{
		strcat(mfw_twh_str, "   ");
	}

	strcat(mfw_twh_str, "-> ");
	
	sprintf(fmt_str,"N:%08p T:%s", ptr, mfw_twhType(typ));/*a0393213 lint warnings removal-format specifier %08lx changed to %p*/

	strcat(mfw_twh_str, fmt_str);

	if (typ != MfwTypWin)
	{
		/*
		** Trace out the details - There will be no callback
		*/
		TRACE_EVENT(mfw_twh_str);
	}
#endif
	return;
}

static void mfw_twhCb(void *ptr)
{
	char	fmt_str[20];

	memset(fmt_str, 0, 20);
#if 0
	/* The code here will output the information in hex format which will need
    ** more decoding, but which may be necessary if the amount of information
    ** being output causes problems in the BMI. Change the above #if to be '#if 1'
    ** to use this format
    */
	/*
	** No Need to test whether there is enough space for the callback ...already checked in mfw_twhOut()
	*/
	sprintf(fmt_str,"%08lx", ptr);
		
	strcat(mfw_twh_str, fmt_str);

	mfw_twh_strlen += 8;
#else
	/*
	** No Need to test whether there is enough space for the callback ...already checked in mfw_twhOut()
	*/
	sprintf(fmt_str," Cb:%08p", ptr);/*a0393213 lint warnings removal-format specifier %08lx changed to %p*/
		
	strcat(mfw_twh_str, fmt_str);

	TRACE_EVENT(mfw_twh_str);
#endif
	return;
}

static void mfw_twh_str_purge(void)
{
#if 0
	/* The code here is needed when using the hex format output which will need
    ** more decoding, but which may be necessary if the amount of information
    ** being output causes problems in the BMI. Change the above #if to be '#if 1'
    ** to use this format
    */
	TRACE_EVENT(mfw_twh_str);

	memset(mfw_twh_str, 0, TWH_MAX_STR_LEN);
	mfw_twh_strlen = 0;

	vsi_t_sleep(VSI_CALLER 10);
#endif
	return;
}

