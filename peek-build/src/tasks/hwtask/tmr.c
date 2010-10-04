#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"

#include "drv_tmr.h"
#include "l4_tim.h"
#include "tmr.h"

#include "cus_aci.h"

static void (*sig) (void) = 0;          /* timer signaling function */

int mfw_timeout (USHORT t)
{
    if (sig AND t EQ MFW_TIMER)
    {
      sig();
      return 1;
    }
    return 0;
}


int tmrInit (void (*s)(void))
{
    sig = s;

    TRACE_FUNCTION("tmrInit()");

    if (!s)
        return 0;

    return 1;
}

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

void tmrStart (ULONG ms)
{
  if (sig)
  {
    if (ms < 5)                         /* at least one tick        */
      ms = 5;                           /* set to minimum           */
	TIMER_START(aci_handle, MFW_TIMER, ms);
  }
}

ULONG tmrStop (void)
{
  T_TIME left;                         /* VSI timer value          */

  if (sig)
  {
    vsi_t_status(VSI_CALLER MFW_TIMER, &left);
	TIMER_STOP(aci_handle, MFW_TIMER);
    return (left);
  }
  return 0;
}

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
