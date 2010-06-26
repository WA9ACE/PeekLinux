/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_tim.c       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 12              $|
| CREATED: 21.09.98                     $Modtime:: 2.03.00 11:49    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_TIM

   PURPOSE : timer handling functions

   EXPORT  :

   TO DO   :

   $History:: mfw_tim.c                                             $
 * 
 * *****************  Version 12  *****************
 * User: Es           Date: 3.03.00    Time: 12:10
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * timStart(), timSignal() etc.: safer handling of simultaneous
 * timeouts; additional chain in timer control block.
 *
 * *****************  Version 11  *****************
 * User: Nm           Date: 18.02.00   Time: 13:57
 * Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 10  *****************
 * User: Nm           Date: 18.02.00   Time: 12:34
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * change the name timSetup to
 * timSetTime
 *
 * *****************  Version 9  *****************
 * User: Nm           Date: 18.02.00   Time: 12:21
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * add the function timSetup()
 *
 * *****************  Version 8  *****************
 * User: Es           Date: 14.06.99   Time: 12:14
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 7  *****************
 * User: Es           Date: 1.04.99    Time: 17:07
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * removed lots of traces
 *
 * *****************  Version 6  *****************
 * User: Es           Date: 18.02.99   Time: 17:01
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 5  *****************
 * User: Es           Date: 17.02.99   Time: 19:11
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 4  *****************
 * User: Es           Date: 27.01.99   Time: 15:06
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
#include "drv_tmr.h"
#include "mfw_tim.h"


static MfwTim *timRoot = 0;             /* list of running clocks   */
static int timTimeoutCount;             /* overrun counter          */
static int timTimeoutBusy;              /* overrun marker           */
static int timTimerPrecMs;              /* minimum timer intervall  */
static MfwTim *ActiveTOut = NULL;          /* list of timeouts to be processed on this cycle*/
/* PATCH PMC 000721: use another pointer /
         NDH 16/4/2003 : Make it static for use with timDelete also */
static MfwTim *timSavedNext = NULL;
/* END PATCH PMC 000721: use another pointer */

static void timInsert (MfwTim *t);
static void timRemove (MfwTim *t);
static void timAdjust (S32 t);
static int timFind (MfwTim *t);
static int timCommand (U32 cmd, void *h);

MfwHdr * current_mfw_elem;

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timInit            |
+--------------------------------------------------------------------+

  PURPOSE : initialize timer handler

*/

MfwRes timInit (void)
{
    void timTimeout (void);

    timTimeoutCount = 0;
    timTimeoutBusy = 0;
    timRoot = 0;
    mfwCommand[MfwTypTim] = (MfwCb) timCommand;
    tmrInit(timTimeout);
    tmrStart(1);
    timTimerPrecMs = tmrStop();

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timExit            |
+--------------------------------------------------------------------+

  PURPOSE : finalize timer handler

*/

MfwRes timExit (void)
{
    tmrExit();
    mfwCommand[MfwTypTim] = 0;

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timCreate          |
+--------------------------------------------------------------------+

  PURPOSE : create timer control

*/

MfwHnd timCreate (MfwHnd w, S32 t, MfwCb f)
{
    MfwHdr *hdr = (MfwHdr *) mfwAlloc(sizeof(MfwHdr));
    MfwTim *tim = (MfwTim *) mfwAlloc(sizeof(MfwTim));
    MfwHdr *insert_status =0;

    if (!hdr || !tim)
   	 {
    	TRACE_ERROR("ERROR: timCreate() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));

   		if(tim)
   			mfwFree((U8*)tim,sizeof(MfwTim));
   		
	   	return 0;
     }

    tim->time = t;
    tim->left = 0;
    tim->handler = f;
    tim->next = 0;
    // PATCH LE 06.06.00
    // store mfw header address
    tim->mfwHeader = hdr;	/* SPR#1597 - SH - Change mfw_header to mfwHeader */
    // END PATCH LE 06.06.00

    hdr->data = tim;
    hdr->type = MfwTypTim;

    insert_status= mfwInsert(w,hdr);
    if(!insert_status)
	{
  		TRACE_ERROR("ERROR: timCreate() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)tim,sizeof(MfwTim));
		return 0;
  	}
    return insert_status;
  	
    
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timDelete          |
+--------------------------------------------------------------------+

  PURPOSE : delete timer control block

*/

MfwRes timDelete (MfwHnd h)
{
    MfwRes res;

    if (!h)
        return MfwResIllHnd;

    if (((MfwHdr *) h)->type != MfwTypTim)
        return MfwResIllHnd;

    res = (mfwRemove(h)) ? MfwResOk : MfwResIllHnd;

    timStop(h);

    mfwFree(((MfwHdr *) h)->data,sizeof(MfwTim));
    mfwFree(h,sizeof(MfwHdr));

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timStart           |
+--------------------------------------------------------------------+

  PURPOSE : start timer

*/

MfwRes timStart (MfwHnd h)
{
    MfwTim *tc;
    S32 left, diff;

    if (!h)
        return MfwResIllHnd;

    if (((MfwHdr *) h)->type != MfwTypTim)
        return MfwResIllHnd;

    tc = ((MfwHdr *) h)->data;

    if (tc->time <= 0)
        return MfwResErr;

    if (tc->time < timTimerPrecMs)
        tc->time = timTimerPrecMs;

    tc->left = tc->time;
    left = tmrStop();                   /* get systimer left time   */
    timRemove(tc);                      /* remove, if running       */
    if (left <= tc->left)
        tc->left -= left;               /* adjust for next timer    */
    else
    {
        diff = left - tc->left;         /* correction value         */
        left = tc->left;                /* new timeout              */
        tc->left = 0;                   /* this is the first        */
        timAdjust(diff);                /* correct other timers     */
    }
    timInsert(tc);
    if (!left)                          /* no timer was running     */
    {
        left = timRoot->left;
        tc = timRoot;
        while (tc)
        {
            tc->left -= left;           /* adjust time left entry   */
            tc = tc->next;
        }
    }
    tmrStart(left);                     /* restart timer            */

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timSetup           |
+--------------------------------------------------------------------+

  PURPOSE : Stop the current timer and reload it with the new timeout.


*/

MfwRes timSetTime (MfwHnd h,S32 t)
{
    MfwTim *tc;

    if (!h)
        return MfwResIllHnd;

    if (((MfwHdr *) h)->type != MfwTypTim)
        return MfwResIllHnd;

    timStop (h);                        /* stop the current timer */

    tc = ((MfwHdr *) h)->data;
    tc->time = t;                        /* load with new timeout */
    tc->left = 0;


    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timStop            |
+--------------------------------------------------------------------+

  PURPOSE : stop timer

*/

MfwRes timStop (MfwHnd h)
{
    MfwTim *tc;

    if (!h)
        return MfwResIllHnd;

    if (((MfwHdr *) h)->type != MfwTypTim)
        return MfwResIllHnd;

    tc = ((MfwHdr *) h)->data;
    tc->left = 0;

    timRemove(tc);

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timTime            |
+--------------------------------------------------------------------+

  PURPOSE : get timers remaining time

*/

S32 timTime (MfwHnd h)
{
    MfwTim *tc;

    if (!h)
        return 0;

    if (((MfwHdr *) h)->type != MfwTypTim)
        return 0;

    tc = ((MfwHdr *) h)->data;

    if (timFind(tc))
        return (tmrLeft() + tc->left);

    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timSignal          |
+--------------------------------------------------------------------+

  PURPOSE : thread context time out

*/

void timSignal (void)
{
    S32 tout;
    MfwTim *tc;
	UBYTE temp;

    if (!timRoot)
        return;

    ActiveTOut = timRoot;
	ActiveTOut->next2 = 0;

    while (timRoot && timRoot->left == 0)
    {
        timRoot->left = -1;             /* flag timeout             */
        timRoot->next2 = timRoot->next; /* setup timeout chain      */
        timRoot = timRoot->next;        /* remove element           */

        /* SPR#2029 - DS - Ensure second timer exists. Port of HLE fix. */
	  if (timRoot != 0)
            timRoot->next2 = 0;
    }

    if (timRoot)
    {
        tout = timRoot->left;
        tc = timRoot;
        while (tc)
        {
            tc->left -= tout;           /* adjust time left entry   */
            tc = tc->next;
        }
        tmrStart(tout);                 /* start next session       */
    }

    while (ActiveTOut && ActiveTOut->left < 0)          /* signal timout handlers   */
    {
        ActiveTOut->left = 0;

/* PATCH PMC 000721: save the next pointer because the memory associated with 
 * to may be released in the timer handler function.
 */
        timSavedNext = ActiveTOut->next2;
/* END PATCH PMC 000721 */
		
        if (ActiveTOut->handler)
        {
        // PATCH LE 06.06.00
        // store current mfw elem
          current_mfw_elem = ActiveTOut->mfwHeader;	 /* SPR#1597 - SH - Change mfw_header to mfwHeader */
        // END PATCH LE 06.06.00

		/* NM, p011b */	
		temp = dspl_Enable(0);
		/* p011b end */	
		
		  (void)((*(ActiveTOut->handler))(ActiveTOut->time,ActiveTOut));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
		

		/* NM, p011c */
		dspl_Enable(temp);
		/* p011c end */	  
		
        }
/* PATCH PMC 000721: use the SavedNext pointer to set ActiveTOut */
		ActiveTOut = timSavedNext;
    /* cq18182 pointer cleared here, this fix is only temporary as it seems to fix the current problem, however further investigation
	  is required as to why the timSavedNext pointer was not being cleared. 10-03-04 MZ. */
		timSavedNext = NULL;
/* END PATCH PMC 000721 */
    }

	/* cq18182 add check and clear the pointer 10-03-04 MZ.*/
	if(ActiveTOut != NULL)
		ActiveTOut = NULL;

}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timTimeout         |
+--------------------------------------------------------------------+

  PURPOSE : thread context time out

*/

void timTimeout (void)
{
    timTimeoutCount++;

    if (timTimeoutBusy)
        return;

    timTimeoutBusy = 1;
    while (timTimeoutCount)
    {
        timTimeoutCount--;
        timSignal();
    }
    timTimeoutBusy = 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timInsert          |
+--------------------------------------------------------------------+

  PURPOSE : insert timer into ordered list

*/

static void timInsert (MfwTim *t)
{
    MfwTim *prev, *curr;

    if (!timRoot)
    {
        timRoot = t;
        t->next = 0;  
	 t->next2 = 0; /* cq18182 initialise the pointer 10-03-04 MZ. */
        return;
    }
    if (t->left < timRoot->left)
    {
        t->next = timRoot;
        t->next2 = 0;  /* cq18182 initialise the pointer 10-03-04 MZ. */
        timRoot = t;
        return;
    }
    prev = timRoot;
    curr = timRoot->next;
    while (curr && t->left >= curr->left)
    {
        prev = curr;
        curr = curr->next;
    }
    prev->next = t;
    t->next = curr;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timRemove          |
+--------------------------------------------------------------------+

  PURPOSE : remove timer from list

*/

static void timRemove (MfwTim *t)
{
    MfwTim *prev, *curr;
	MfwTim *timCheck;
	int		doneFlag = 0;

	if (t == 0)
        return;
	
    while (timRoot == t)
        timRoot = t->next;
 
    if (timRoot)
	{
    prev = timRoot;
    curr = timRoot->next;

    while (curr)
    {
        if (curr == t)
            prev->next = curr->next;
        else
            prev = curr;
        curr = curr->next;
    }
}

    /*
    ** Ensure that the timer being deleted is not on the ActiveTOut List
    */
    if (timSavedNext)
	{
		if (timSavedNext == t)
		{
			timSavedNext = timSavedNext->next2;
		}
		else
		{
			timCheck = timSavedNext;
			if(timCheck == NULL || (ULONG)timCheck > 33554432)
			{     /* cq18182 Additional traces to trap pointer overflow. 10-03-04 MZ. */
    	      			TRACE_EVENT_P1("ERROR: timCheck invalid 0x%08x - mfw_tim.c(593), quit the function", timCheck);
		        	return;
		       }
			

		    while (timCheck != 0 && !doneFlag)
		    {

			if(timCheck == NULL || (ULONG)timCheck > 33554432)
			{  /* cq18182 Additional traces to trap pointer overflow. 10-03-04 MZ. */
    	      			TRACE_EVENT_P1("ERROR: timCheck invalid 0x%08x - mfw_tim.c(603), quit the function", timCheck);
		        	return;
		       }
			
		    	if (timCheck->next2 == t)
		    	{
		    		timCheck->next2 = t->next2;

		    		doneFlag = (int)1;
		    	}
		    	else
		    	{
		    		if(timCheck == NULL || (ULONG)timCheck > 33554432)
				{    /* cq18182 Additional traces to trap pointer overflow. 10-03-04 MZ. */
    	      				TRACE_EVENT_P1("ERROR: timCheck invalid 0x%08x - mfw_tim.c(617), quit the function", timCheck);
		        		return;
		       	}
		    		timCheck = timCheck->next2;
					
		    	}
		   	}
		}
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timAdjust          |
+--------------------------------------------------------------------+

  PURPOSE : adjust all timers in list

*/

static void timAdjust (S32 t)
{
    MfwTim *tc;

    tc = timRoot;
    while (tc)
    {
        tc->left += t;
        tc = tc->next;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timFind            |
+--------------------------------------------------------------------+

  PURPOSE : find timer in running list

*/

static int timFind (MfwTim *t)
{
    MfwTim *tc;

    tc = timRoot;
    while (tc)
    {
        if (tc == t)
            return 1;
        tc = tc->next;
    }

    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TIM            |
| STATE   : code                        ROUTINE : timCommand         |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

static int timCommand (U32 cmd, void *h)
{
    switch (cmd)
    {
        case MfwCmdDelete:              /* delete me                */
            if (!h)
                return 0;
            timDelete(h);
            return 1;
        default:
            break;
    }

    return 0;
}

