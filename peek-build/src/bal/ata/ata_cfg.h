#ifndef __BTC_ATA_CFG_H_20080112__
#define __BTC_ATA_CFG_H_20080112__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "bal_def.h"

#ifndef FALSE
  #define FALSE 0
#endif

#ifndef TRUE
  #define TRUE 1
#endif

/* ATA memory handler */
//#define ATA_MALLOC(P,S)  { P = (void*)malloc(S);}
//#define ATA_MFREE(P) {free(P);}

/* For windows test */
/* #define OS_WINDOWS */

#if 0
/* ATA trace handler */
#ifdef OS_WINDOWS
#define ATA_TRACE(f) printf(f)
#define ATA_TRACE_P1(f,a1) printf(f,a1);
#define ATA_TRACE_P2(f,a1,a2) printf(f,a1,a2);
#else
#define ATA_TRACE(f) bal_trace(f)
#define ATA_TRACE_P1(f,a1) bal_printf(f,a1)
#define ATA_TRACE_P2(f,a1,a2)
#endif
#endif

#define ATA_MALLOC(s)  BalMalloc(s)
#define ATA_MFREE(p) BalFree(p)
 
#define ATA_TRACE(f) bal_trace(f)
#define ATA_TRACE_P1(f,a1) bal_printf(f,a1)
#define ATA_TRACE_P2(f,a1,a2) bal_printf(f,a1,a2)

/* ATA unit test enable */
/* #define ATA_UNIT_TEST */  

/* Define TI AT command process interface */
#define ATA_CFG_USE_ACI
//#define ATA_CFG_USE_DIO

#define ATA_GPRS

/* Define the AT command max lenght */
#define ATA_AT_CMD_MAX_LEN 160

#ifdef __cplusplus
}
#endif
#endif /* __BTC_ATA_CFG_H_20080112__ */
