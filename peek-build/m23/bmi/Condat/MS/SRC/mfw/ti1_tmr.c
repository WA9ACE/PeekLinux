/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: ti1_tmr.c       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 8               $|
| CREATED: 28.01.99                     $Modtime:: 18.02.00 17:57   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : TI1_TMR

   PURPOSE : timer driver interface (TI1 VERSION)
             uses only one system timer (programmed in milliseconds)

   EXPORT  :

   TO DO   :

   $History:: ti1_tmr.c                                             $
 * 
 *Sep 19, 2006  DRT OMAPS00091093   x0047075(Archana)
 *	Description: optimizing of the timer related vsi interface
 *	Solution:Replacing the Vsi_t_start ,Vsi_t_stop with TIMER_START and TIMER_STOP
 *****************  Version 8  *****************
 * User: Es           Date: 18.02.00   Time: 17:58
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * tmrStart(): bug in inner 'if'-clause
 * 
 * *****************  Version 7  *****************
 * User: Es           Date: 6.07.99    Time: 12:41
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 6  *****************
 * User: Es           Date: 14.06.99   Time: 12:15
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 5  *****************
 * User: Es           Date: 1.04.99    Time: 17:07
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * removed lots of traces
 *
 * *****************  Version 4  *****************
 * User: Es           Date: 17.02.99   Time: 19:11
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 3  *****************
 * User: Es           Date: 14.02.99   Time: 20:57
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 2  *****************
 * User: Es           Date: 11.02.99   Time: 16:43
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 1  *****************
 * User: Es           Date: 9.02.99    Time: 14:54
 * Created in $/GSM/DEV/MS/SRC/MFW
 * TI display & keyboard interface for MFW
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
#include "l4_tim.h"
#include "ti1_tmr.h"

#include "cus_aci.h"

#if !defined (NEW_FRAME)
void aci_flush_fifo (T_VSI_THANDLE timer);
#endif

static void (*sig) (void) = 0;          /* timer signaling function */
#if !defined (NEW_FRAME)
static T_VSI_THANDLE timer = 0;         /* VSI timer handle         */
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_tmr            |
| STATE   : code                        ROUTINE : tmrTimer           |
+--------------------------------------------------------------------+

  PURPOSE : forward timer signal

*/
#if defined (NEW_FRAME)

int mfw_timeout (USHORT t)
{
    if (sig AND t EQ MFW_TIMER)
    {
      sig();
      return 1;
    }
    return 0;
}

#else

int mfw_timeout (T_VSI_THANDLE t)
{
    if (sig && t == timer)
    {
        sig();
        return 1;
    }

    return 0;
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_tmr            |
| STATE   : code                        ROUTINE : tmrInit            |
+--------------------------------------------------------------------+

  PURPOSE : initialize timer driver

*/

#if defined (NEW_FRAME)

int tmrInit (void (*s)(void))
{
    sig = s;

    TRACE_FUNCTION("tmrInit()");

    if (!s)
        return 0;

    return 1;
}

#else

int tmrInit (void (*s)(void))
{
    sig = s;

    TRACE_FUNCTION("tmrInit()");

    if (!s)
        return 0;

    timer = vsi_t_open(VSI_CALLER "mfwTimer");
    if (timer < VSI_OK)
    {
        TRACE_ERROR("TI1_TMR_INIT: No Timer");
        sig = 0;
        return 0;
    }

    return 1;
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_tmr            |
| STATE   : code                        ROUTINE : tmrExit            |
+--------------------------------------------------------------------+

  PURPOSE : finalize timer driver

*/

#if defined (NEW_FRAME)

int tmrExit (void)
{
    TRACE_FUNCTION("tmrExit()");

    if (sig)
    {
      tmrStop();
    }
    sig = 0;

    return 1;
}

#else

int tmrExit (void)
{
    TRACE_FUNCTION("tmrExit()");

    if (sig)
    {
        tmrStop();
        vsi_t_close(VSI_CALLER timer);
    }
    timer = 0;
    sig = 0;

    return 1;
}

#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_tmr            |
| STATE   : code                        ROUTINE : tmrStart           |
+--------------------------------------------------------------------+

  PURPOSE : start timer

*/
#if defined (NEW_FRAME)

void tmrStart (ULONG ms)
{
  if (sig)
  {
    if (ms < 5)                         /* at least one tick        */
      ms = 5;                           /* set to minimum           */
// Sep 19, 2006  DRT OMAPS00091093   x0047075(Archana)
//Replacing Vsi_t_start with TIMER_START
TIMER_START(aci_handle, MFW_TIMER, ms);
  }
}

#else

void tmrStart (U32 ms)
{
    T_VSI_TVALUE toTime;                /* VSI timer value          */

    if (sig && timer >= VSI_OK)
    {
        toTime = (ms * TMR_PREC) / TMR_TICK;
        if (ms && !toTime)
            toTime = 1;                 /* set to minimum           */

// Sep 19, 2006  DRT OMAPS00091093   x0047075(Archana)
//Replacing Vsi_t_start with TIMER_START
       TIMER_START(VSI_CALLER timer,toTime);
    }
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_tmr            |
| STATE   : code                        ROUTINE : tmrStop            |
+--------------------------------------------------------------------+

  PURPOSE : stop timer

*/

#if defined (NEW_FRAME)

ULONG tmrStop (void)
{
  T_TIME left;                         /* VSI timer value          */

  if (sig)
  {
    vsi_t_status(VSI_CALLER MFW_TIMER, &left);
// Sep 19, 2006  DRT OMAPS00091093   x0047075(Archana)
//Replacing Vsi_t_stop with TIMER_STOP
TIMER_STOP(aci_handle, MFW_TIMER);
    return (left);
  }
  return 0;
}

#else

U32 tmrStop (void)
{
    T_VSI_TVALUE left;                  /* VSI timer value          */

    if (sig && timer >= VSI_OK)
    {
        vsi_t_status(VSI_CALLER timer,&left);
// Sep 19, 2006  DRT OMAPS00091093   x0047075(Archana)
//Replacing Vsi_t_stop with TIMER_STOP
TIMER_STOP(VSI_CALLER timer);
        aci_flush_fifo(timer);
        return ((left * TMR_TICK) / TMR_PREC);
    }

    return 0;
}

#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_tmr            |
| STATE   : code                        ROUTINE : tmrLeft            |
+--------------------------------------------------------------------+

  PURPOSE : report left time

*/

#if defined (NEW_FRAME)

ULONG tmrLeft (void)
{
    T_TIME left;                  /* VSI timer value          */

    if (sig)
    {
      vsi_t_status(VSI_CALLER MFW_TIMER, &left);
      return (left);
    }
    return 0;
}

#else

U32 tmrLeft (void)
{
    T_VSI_TVALUE left;                  /* VSI timer value          */

    if (sig && timer >= VSI_OK)
    {
        vsi_t_status(VSI_CALLER timer,&left);
        return ((left * TMR_TICK) / TMR_PREC);
    }

    return 0;
}

#endif
