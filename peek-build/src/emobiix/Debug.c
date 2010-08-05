#include "stdarg.h"
#include "rvf_api.h"
#include "rv_trace.h"
#include "rvm_use_id_list.h"
#include "nucleus.h"

unsigned int mfwMmeDrvFlag;

#ifndef SIMULATOR
void emo_printf( const char* fmt, ...) {
#ifndef EMO_PROD
        char buf[10000];
        va_list ap;
        NU_TASK *tp;

        va_start(ap, fmt);
        tp =  TCC_Current_Task_Pointer();

        if(!tp) {
                vsprintf(buf, fmt, ap);
        } else {
                sprintf(buf, "Task [%s] ", tp->tc_name);
                vsprintf(buf+strlen(buf), fmt, ap);
        }
        rvf_send_trace (buf,strlen(buf)+1,NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )
        va_end(ap);
        TCCE_Task_Sleep(2);
#endif
}
#endif
