#include "stdarg.h"
#include "rvf_api.h"
#include "rv_trace.h"
#include "rvm_use_id_list.h"

void BalKeypadGetNotify(void)
{

}

void bal_trace(const char* fmt, ...) {
        char buf[1024];
        va_list ap;
        va_start(ap, fmt);
        vsprintf(buf, fmt, ap);
        rvf_send_trace (buf,strlen(buf),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

        va_end(ap);
}

void bal_printf( const char* fmt, ...) {
        char buf[1024];
        va_list ap;
        va_start(ap, fmt);
        vsprintf(buf, fmt, ap);
        rvf_send_trace (buf,strlen(buf),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

        va_end(ap);
}

//unsigned int mfwMmeDrvFlag;


void emo_printf( const char* fmt, ...) {
	char buf[1024];
        va_list ap;
        va_start(ap, fmt);
        vsprintf(buf, fmt, ap);
  	rvf_send_trace (buf,strlen(buf),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )
        va_end(ap);
	TCCE_Task_Sleep(1);
}
