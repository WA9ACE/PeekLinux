/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_win.h       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 8               $|
| CREATED: 21.09.98                     $Modtime:: 23.03.00 9:08    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_WIN

   PURPOSE : window types & constants

   EXPORT  :

   TO DO   :

   $History:: mfw_win.h                                             $
 * 
 * *****************  Version 8  *****************
 * User: Es           Date: 23.03.00   Time: 14:42
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Added 'winUnhide()': no update(), no event.
 * 
 * *****************  Version 7  *****************
 * User: Kk           Date: 17.01.00   Time: 8:56
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * added simple check functions for easy checking if win is visible,
 * focussed, closed
 * 
 * *****************  Version 6  *****************
 * User: Le           Date: 6.01.00    Time: 9:23
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Alignment of MFW versions
 * 
 * *****************  Version 3  *****************
 * User: Xam          Date: 12/15/99   Time: 4:06p
 * Updated in $/GSM/Condat/SND-MMI/MFW
 * Added constant MfwWinClosed.
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
 * *****************  Version 5  *****************
 * User: Es           Date: 6.07.99    Time: 12:37
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 4  *****************
 * User: Es           Date: 17.02.99   Time: 20:01
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

#ifndef _DEF_MFW_WIN_H_
#define _DEF_MFW_WIN_H_


typedef struct MfwWinAttrTag            /* WINDOW ATTRIBUTES        */
{
    MfwRect win;                        /* window position and size */
    MfwRect view;                       /* view position and size   */
    U16 bgColor;                        /* background color         */
} MfwWinAttr;

typedef unsigned long *MfwUserDataPtr ;

typedef struct MfwWinTag                /* WINDOW CONTROL BLOCK     */
{
    MfwEvt mask;                        /* selection of events      */
    MfwEvt flags;                       /* current event            */
    MfwCb handler;                      /* event handler            */
    MfwWinAttr *attr;                   /* window attributes        */
    MfwHdr *elems;                      /* window elements          */
    void *user;                         /* user specific data       */
} MfwWin;

                                        /* WINDOW FLAGS             */
#define MfwWinVisible       1           /* window is visible        */
#define MfwWinFocussed      2           /* input focus / selected   */
#define MfwWinDelete        4           /* window will be deleted   */
#define MfwWinClosed        256         /* Child window closed ??   */
/* xreddymn Aug-09-2004, MMI-SPR-23964 (TII_MMS33) 
 * Implemented suspend and resume functionality for MFW windows
 */
#define MfwWinSuspend       8           /* window is suspended      */
#define MfwWinResume        16          /* window is resumed        */
                                        /* PROTOTYPES               */
MfwRes winInit (U16 *sx, U16 *sy);
MfwRes winExit (void);
MfwHnd winCreate (MfwHnd w, MfwWinAttr *a, MfwEvt e, MfwCb f);
MfwRes winDelete (MfwHnd w);
MfwRes winShow (MfwHnd w);
MfwRes winHide (MfwHnd w);
MfwRes winUnhide (MfwHnd w);
MfwHnd winFocus (MfwHnd w);
U8 winAutoFocus (U8 useit);
MfwRes winClear (MfwHnd win);
MfwRes winUpdate (MfwWin *w);
MfwHnd winNext (MfwHnd winCur);
MfwHnd winPrev (MfwHnd winCur);

int	winIsVisible(MfwHnd w);
int	winIsFocussed(MfwHnd w);
int	winIsClosed(MfwHnd w);

void mfw_traceWinHdrs(void);

#endif

