#ifndef __BAL_GPRS_CFG_H_20080225__
#define __BAL_GPRS_CFG_H_20080225__

#ifdef __cplusplus
extern "C" {
#endif

#define BAL_GPRS_DEBUG 1
#define BAL_MEM_MALLOC 1

#ifdef BAL_GPRS_DEBUG
    #include "../bal/bal_def.h"
    #define GPRS_TRACE(f)              bal_trace(f)
    #define GPRS_TRACE_P1(f,a1)        bal_printf(f,a1)
#else
    #define GPRS_TRACE(f)
#endif

#ifdef BAL_MEM_MALLOC
    #include "balapi.h"
    #define GPRS_MALLOC(s) BalMalloc(s)
    #define GPRS_FREE(p)   BalFree(p)
#endif


#ifdef __cplusplus
}
#endif
#endif //__BAL_GPRS_CFG_H_20080225__
