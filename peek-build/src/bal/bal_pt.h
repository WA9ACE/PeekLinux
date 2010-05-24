#ifndef __BAL_PT_20090115_H__
#define __BAL_PT_20090115_H__

#ifdef VIN_PERFORMANCE_TEST
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
void bal_printf( const char* fmt, ...);
unsigned long NU_Retrieve_Clock(void);
#ifdef __cplusplus
}
#endif //__cplusplus

#define PerformanceTraceStart(f) \
do \
{ \
    unsigned long Interval     = 0; \
    unsigned long StartTicks   = NU_Retrieve_Clock();

#define PerformanceTraceEnd(f) \
    Interval = NU_Retrieve_Clock() - StartTicks; \
		bal_printf("[PerformanceTrace:] Function Name: [%s] Run Time: [%5d Ticks] [%4.0f MS]\n", \
               f, Interval, Interval/0.22); \
}while(0);
#else //VIN_PERFORMANCE_TEST
#define PerformanceTraceStart(f)
#define PerformanceTraceEnd(f)
#endif //VIN_PERFORMANCE_TEST
#endif //__BAL_PT_20090115_H__

