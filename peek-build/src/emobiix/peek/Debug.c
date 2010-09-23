#include "stdarg.h"
#include "stdio.h"
#include "rvf_api.h"
#include "rv_trace.h"
#include "rvm_use_id_list.h"
#include "nucleus.h"

typedef long off_t;

unsigned int mfwMmeDrvFlag;
static  char buf[10000] = {0};

void emo_printf( const char* fmt, ...) {
#ifndef EMO_PROD
        va_list ap;
        NU_TASK *tp;

        va_start(ap, fmt);
        tp =  TCC_Current_Task_Pointer();

        if(!tp) {
                vsprintf(buf, fmt, ap);
        } else {
                sprintf(buf, "[%s] ", tp->tc_name);
                vsprintf(buf+strlen(buf), fmt, ap);
        }
		buf[64] = '\0';
        rvf_send_trace (buf,strlen(buf)+1,NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )
        va_end(ap);
//        TCCE_Task_Sleep(2);
#endif
}

void emo_fprintf(FILE *file, const char* fmt, ...) {
#ifndef EMO_PROD
        va_list ap;
        NU_TASK *tp;

        va_start(ap, fmt);
        tp =  TCC_Current_Task_Pointer();

        if(!tp) {
                vsprintf(buf, fmt, ap);
        } else {
                sprintf(buf, "[%s] ", tp->tc_name);
                vsprintf(buf+strlen(buf), fmt, ap);
        }
		buf[64] = '\0';
        rvf_send_trace (buf,strlen(buf)+1,NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )
        va_end(ap);
//        TCCE_Task_Sleep(2);
#endif
}

void bal_trace(char *msg) {
	//emo_printf("%s", msg);
}

void bal_printf(const char* fmt, ...) {
#ifndef EMO_PROD
/* only used for kpd stuff and we know its working
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
*/
#endif
}

/* Default streams for stdlib */
int DStreamopen(const char *path, unsigned flags, int llv_fd) {

	emo_printf("DStreamopen()");

	return 0;
}

int DStreamclose(int dev_fd) {
	return 0;
}

int DStreamread(int dev_fd, char *buf, unsigned count) {
	return 0;
}

int DStreamwrite(int dev_fd, const char *buf, unsigned count) {
	emo_printf("Stream write: fd %d - buf %s - count %d", dev_fd, buf, count);
	return count;
}

int DStreamunlink(const char *path) {
	return -1;
}

int DStreamrename(const char *old_name, const char *new_name) {
	return -1;
}

off_t DStreamlseek(int dev_fd, off_t offset, int origin) {
	return -1;
}

