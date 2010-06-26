/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_tim.h       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 8               $|
| CREATED: 21.09.98                     $Modtime:: 2.03.00 11:50    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_TIM

   PURPOSE : Timer types & constants

   EXPORT  :

   TO DO   :

   $History:: mfw_tim.h                                             $
 * 
 * *****************  Version 8  *****************
 * User: Es           Date: 3.03.00    Time: 12:10
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * timStart(), timSignal() etc.: safer handling of simultaneous
 * timeouts; additional chain in timer control block.
 *
 * *****************  Version 7  *****************
 * User: Nm           Date: 18.02.00   Time: 12:34
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * change the name timSetup to
 * timSetTime
 *
 * *****************  Version 6  *****************
 * User: Nm           Date: 18.02.00   Time: 12:21
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * add timSetup()
 *
 * *****************  Version 5  *****************
 * User: Es           Date: 14.06.99   Time: 12:14
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

#ifndef _DEF_MFW_TIM_H_
#define _DEF_MFW_TIM_H_

typedef struct MfwTimTag                /* TIMER CONTROL BLOCK      */
{
    S32 time;                           /* timeout time             */
    S32 left;                           /* time left before timeout */
    MfwCb handler;                      /* event handler            */
    struct MfwTimTag *next;             /* next in active chain     */
    struct MfwTimTag *next2;            /* next in timeout chain    */
    // PATCH LE 06.06.00
    // add pointer to MFW header element
    MfwHdr * mfwHeader;                /* Associated header.  SPR#1597 - SH - Change mfw_header to mfwHeader */
    // END PATCH LE 06.06.00
} MfwTim;

                                        /* PROTOTYPES               */
MfwRes timInit (void);
MfwRes timExit (void);
MfwHnd timCreate (MfwHnd w, S32 t, MfwCb f);
MfwRes timDelete (MfwHnd h);
MfwRes timStart (MfwHnd tim);
MfwRes timStop (MfwHnd tim);
MfwRes timSetTime (MfwHnd tim, S32 t);
S32 timTime (MfwHnd h);
void timSignal (void);

/* Added to remove warning */
EXTERN UBYTE dspl_Enable (UBYTE   in_Enable);

#endif

