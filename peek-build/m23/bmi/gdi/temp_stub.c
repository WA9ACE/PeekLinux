#include "stdarg.h"
#include "rvf_api.h"
#include "rv_trace.h"
#include "rvm_use_id_list.h"
#include "nucleus.h"

void BalKeypadGetNotify(void)
{

}

void bal_trace(const char* fmt, ...) {
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
}

void bal_printf( const char* fmt, ...) {
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
}

//unsigned int mfwMmeDrvFlag;


void emo_printf( const char* fmt, ...) {
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
}
