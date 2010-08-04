/******************************************************************************
 * PWR task (pwr)
 * Adapted from FFS by Svend Kristian Lindholm, skl@ti.com
 * Idea, design and coding by Mads Meisner-Jensen, mmj@ti.com
 *
 * pwr tracing
 *
 * $Id: pwr_trace.h 1.1 Wed, 20 Aug 2003 10:22:37 +0200 skl $
 *
 ******************************************************************************/


#ifndef __LCC_TRACE_H__
#define __LCC_TRACE_H__

#include "lcc_cfg.h"

/******************************************************************************
 ** Common Tracing and logging
 *****************************************************************************/

int pwr_tr_query(int mask);


/******************************************************************************
 ** Target Tracing
 ******************************************************************************/

#define tw(contents)

#if TRACE_WANTED
    #define ttw(contents) contents
    // To distinguish from FFS trace system rename to pwr_ttr and pwr_str
    #define ttr pwr_ttr
    #define str pwr_str
#else
    #define ttw(contents)
    #define ttr
    #define str
#endif

#define tlw(contents)
#define NL
void pwr_ttr_init(unsigned int mask);
void pwr_ttr(unsigned trmask, char *format, ...);
void pwr_str(unsigned mask, char *string);

#define    TTrMem           0x00000001
#define    TTrMemLow     0x00000002

#define    TTrApi             0x00000010
#define    TTrTimer         0x00000020
#define    TTrTimerLow   0x00000040

#define    TTrInit        0x00000100
#define    TTrInitLow     0x00000200

#define    TTrCharge      0x00000400
#define    TTrChargeLow   0x00000800

#define    TTrEvent      0x00001000
#define    TTrEventLow    0x00002000

#define    TTrEnv         0x00020000
#define    TTrEnvLow      0x00040000

#define    TTrTask       0x00100000
#define    TTrTaskLow     0x00200000

#define    TTrTmpwr       0x08000000

#define    TTrTaskDelays  0x10000000
#define    TTrWarning     0x40000000
#define    TTrFatal      0x80000000
#define    TTrAll        0xFFFFFFFF 


#endif
