#ifndef __KPD_ROT_H__
#define __KPD_ROT_H__

// temperory 15s for debug
#define ROT_TIMER_ID			0x01
#define ROT_TIMER_VAlUE		15000   // ms  //it is to  control the rolling speed and changing direction.
#define ROT_TIMER_INTV			RVF_MS_TO_TICKS(ROT_TIMER_VAlUE)

#define KPD_ROT_LISRSAVE_LENGTH			10

#define KPD_ROT_NULL	0x00
#define KPD_ROT_A		0x01
#define KPD_ROT_B		0x02
#define KPD_ROT_AB		0x03


//#define ROT_DEBUG
#ifndef ROT_DEBUG
#define ROT_TRACE(string)
#define ROT_TRACE_PARAM(string, param)
#define ROT_TRACE_SEND(ch)
#define ROT_TRACE_MARK(ch)
#else
#define ROT_TRACE(string)\
	rvf_send_trace ("ROT: "string,(sizeof("ROT: "string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )
#define ROT_TRACE_PARAM(string, param)\
	rvf_send_trace ("ROT: "string,(sizeof("ROT: "string)-1),(UINT32)param,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )
#define ROT_TRACE_SEND(ch)	TRACE_UART_SEND(ch)	
#define ROT_TRACE_MARK(ch)	TRACE_UART_MARK(ch)
#endif



typedef struct {
    NU_HISR  hisr;
#ifndef HISR_STACK_SHARING
#if (LOCOSTO_LITE)
    char     hisr_stack[288];
#else
    char     hisr_stack[512];
#endif
#endif
} rot_hisr;

typedef enum{
	KPD_ROT_IDLE,
	KPD_ROT_STATE_1,
	KPD_ROT_STATE_2,
	KPD_ROT_STATE_3,
	KPD_ROT_STATE_4
} KPD_ROT_STATE_T;

typedef enum{
	KPD_ROT_CW,
	KPD_ROT_CCW,
	KPD_ROT_UNKNOWN
} KPD_ROT_DIRECTION_T;

typedef void (*ROT_CB_T)(KPD_ROT_DIRECTION_T direction);



void kpd_rot_timer_expired(UINT32 id);
void rot_hisr_entry();
void kpd_rot_init();
void kpd_rot_process(UINT16 event);

#endif 

