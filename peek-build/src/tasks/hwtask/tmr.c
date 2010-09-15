#define ENTITY_MFW

#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"

//#include "mfw_mfw.h"
//#include "mfw_sys.h"
#include "drv_tmr.h"
#include "l4_tim.h"
#include "tmr.h"

#include "cus_aci.h"

static void (*sig) (void) = 0;          /* timer signaling function */

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_tmr            |
| STATE   : code                        ROUTINE : tmrTimer           |
+--------------------------------------------------------------------+

  PURPOSE : forward timer signal

*/

int mfw_timeout (USHORT t)
{
    if (sig AND t EQ MFW_TIMER)
    {
      sig();
      return 1;
    }
    return 0;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_tmr            |
| STATE   : code                        ROUTINE : tmrInit            |
+--------------------------------------------------------------------+

  PURPOSE : initialize timer driver

*/

int tmrInit (void (*s)(void))
{
    sig = s;

    TRACE_FUNCTION("tmrInit()");

    if (!s)
        return 0;

    return 1;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_tmr            |
| STATE   : code                        ROUTINE : tmrExit            |
+--------------------------------------------------------------------+

  PURPOSE : finalize timer driver

*/

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

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_tmr            |
| STATE   : code                        ROUTINE : tmrStart           |
+--------------------------------------------------------------------+

  PURPOSE : start timer

*/

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

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_tmr            |
| STATE   : code                        ROUTINE : tmrStop            |
+--------------------------------------------------------------------+

  PURPOSE : stop timer

*/

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

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_tmr            |
| STATE   : code                        ROUTINE : tmrLeft            |
+--------------------------------------------------------------------+

  PURPOSE : report left time

*/

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
