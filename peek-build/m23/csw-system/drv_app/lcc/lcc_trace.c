/******************************************************************************
 * PWR task (pwr)
 * Adapted from FFS by Svend Kristian Lindholm, skl@ti.com
 * Idea, design and coding by Mads Meisner-Jensen, mmj@ti.com
 *
 * pwr tracing
 *
 * $Id: pwr_trace.c 1.1 Wed, 20 Aug 2003 10:22:37 +0200 skl $
 *
 ******************************************************************************/

#include "lcc/lcc_trace.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "rvf/rvf_api.h"
#include "rv/rv_general.h"
#include "rvm/rvm_use_id_list.h"
#include "rv/rv_defined_swe.h"

/******************************************************************************
 * Target Tracing
 *****************************************************************************/

static unsigned int pwr_mask = TTrFatal | TTrWarning;

void pwr_ttr_init(unsigned int mask)
{
    pwr_mask = mask | TTrFatal | TTrWarning;
}

void pwr_ttr(unsigned int mask, char *format, ...)
{
    va_list args;
    static char buf[256];

    if (pwr_mask & mask)
    {
        // build string ala tr() then call str()
        va_start(args, format);
        vsprintf(buf, format, args);
        str(mask, buf);
        va_end(args);
    }
}

void pwr_str(unsigned mask, char *string)
{
    if (pwr_mask & mask) {
#if (_RVF == 1)
        rvf_send_trace(string, strlen(string), NULL_PARAM,
                       RV_TRACE_LEVEL_WARNING, LCC_USE_ID);
        //rvf_delay(5);
#endif
    }
}

/******************************************************************************
 ** Common Tracing and logging
 *****************************************************************************/

int pwr_tr_query(int mask)
{
    return (pwr_mask & mask);
}
