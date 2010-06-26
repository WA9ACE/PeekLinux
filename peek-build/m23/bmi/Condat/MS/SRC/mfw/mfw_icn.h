/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_icn.h       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 5               $|
| CREATED: 23.11.98                     $Modtime:: 23.03.00 8:58    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_ICN

   PURPOSE : icon types & constants

   EXPORT  :

   TO DO   :

   $History:: mfw_icn.h                                             $

       Shashi Shekar B.S., a0876501, 16 Mar, 2006, OMAPS00061462
       Icon support for SetupMenu & Select item.
  
 *
 * *****************  Version 5  *****************
 * March 01, 2005    REF: ENH 29313    Deepa M.D
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Isolating colour build related changes from MMI LITE flag.
 *The proper  Feature flag  COLORDISPLAY was used for all color 
   related changes.
 * *****************  Version 5  *****************
 * User: Es           Date: 23.03.00   Time: 14:41
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Added icnUnhide(); removed 'update()' from 'hide()' and 'unhide()'
 *
 * *****************  Version 4  *****************
 * User: Le           Date: 6.01.00    Time: 9:23
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Alignment of MFW versions
 *
 * *****************  Version 4  *****************
 * User: Rm           Date: 12/03/99   Time: 10:16a
 * Updated in $/GSM/Condat/SND-MMI/MFW
 * new parameter by icnCycle
 *
 * *****************  Version 3  *****************
 * User: Es           Date: 24.11.99   Time: 11:55
 * Updated in $/GSM/Condat/SND-MMI/MFW
 * improved 'icnHide()' (really hides the icon now).
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

#ifndef _DEF_MFW_ICN_H_
#define _DEF_MFW_ICN_H_


                                        /* ICON EVENTS              */
#define E_ICN_VISIBLE       0x00000001  /* icon is displayed        */

typedef struct MfwIcnAttrTag            /* icon attribute structure */
{
    MfwRect area;                       /* icon area                */
    U8 nIcons;                          /* number of icons          */
    U32 iconCol;					/* index into colours to be used */
    U8 icnType;						/* 0=b+w, 2=256 col, 3=32bit col */
    char *icons;                        /* icon bitmaps             */
// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	U8 selfExplanatory;				/* Flag to show associated Text or not */
#endif
} MfwIcnAttr;

typedef struct MfwIcnTag                /* icon control block       */
{
    MfwEvt mask;                        /* selection of events      */
    MfwEvt flags;                       /* current signaled event   */
    MfwCb handler;                      /* event handler            */
    MfwIcnAttr *attr;                   /* icon attributes          */
    U8 index;                           /* cycle index              */
//March 01, 2005    REF: ENH 29313    Deepa M.D
// Isolating colour build related changes from MMI LITE flag.
// The proper  Feature flag  COLORDISPLAY was used for all color 
//related changes.
    #ifdef COLOURDISPLAY
    U32 icnFgdColour;
    U32 icnBgdColour;
    #else
    /*SPR 2686*/
    U8 icnFgdColour;
    U8 icnBgdColour;

    #endif
} MfwIcn;

MfwHnd icnCreate (MfwHnd w, MfwIcnAttr *a, MfwEvt e, MfwCb f);

MfwRes icnInit (void);
MfwRes icnExit (void);

MfwRes icnDelete (MfwHnd i);
MfwRes icnShow (MfwHnd i);
MfwRes icnHide (MfwHnd i);
MfwRes icnUnhide (MfwHnd i);
MfwRes icnUpdate (MfwIcn *i);
MfwRes icnCycle (MfwHnd i,int o);

#endif

