#ifndef __BAL_GPRS_API_H_20080225__
#define __BAL_GPRS_API_H_20080225__

#ifdef __cplusplus
extern "C" {
#endif

#include "typedefs.h"

/* GPRS states */
enum
{
	GPRS_NOTHING = -1,			/* Nothing - no event to send */
	GPRS_OFF = 0,				/* GPRS is not attached */
	GPRS_SEARCHING,
	GPRS_ATTACHED,				/* GPRS is attached */
	GPRS_CONTEXT_ACTIVATE,		/* PDP context is activated */
	GPRS_CONTEXT_DEACTIVATE,	/* PDP context is deactivated */
	GPRS_ERROR					/* General GPRS error */
};

/* SMS options */
typedef enum
{
	SMS_SERVICE_GPRS = 0,
	SMS_SERVICE_CS,
	SMS_SERVICE_GPRS_PREF,
	SMS_SERVICE_CS_PREF,
	SMS_SERVICE_OMITTED = 0xFF
} BAL_SMS_SERVICE_TYPE;

/* Quality of service (QOS) parameters */
enum
{
	QOS_PRECED_HIGH = 1,
	QOS_PRECED_NORMAL,
	QOS_PRECED_LOW
};

enum
{
	QOS_BEST = 1
};


typedef void (*T_BAL_GPRS_CB) (int);

typedef struct
{
	UBYTE			cid;					/* The context id.  Starts at 1 then increments. */
	UBYTE			reg_status;				/* Registration status */
	char			*counterstring;			/* String for counter digits */
	ULONG			uplink_counter;			/* Value of uplink counter */
	ULONG			downlink_counter;		/* Value of downlink counter */
	USHORT			counter_type;			/* TRUE if uplink counter has been requested */
	UBYTE			sms_service;			/* Type of SMS service */
	USHORT			status;					/* Current status */
	T_BAL_GPRS_CB	callback;				/* MMI provided callback function */
} T_BAL_GPRS_API_DATA;


void GPRS_Init(void);
void GPRS_Exit(void);
void GPRS_SetCallback(T_BAL_GPRS_CB callback);

/* GPRS functionality */

int GPRS_Status(void);
int GPRS_Attach(void);
int GPRS_Detach(void);
int GPRS_ContextDefine(SHORT cid, char *IPAddress, char *apn);
int GPRS_ContextActivate(SHORT cid);
int GPRS_ContextDeactivate(SHORT cid);
int GPRS_SetQOS(SHORT cid, UBYTE preced, UBYTE delay, UBYTE relclass, UBYTE peak, UBYTE mean);
int GPRS_SetMinQOS(SHORT cid, UBYTE preced, UBYTE delay, UBYTE relclass, UBYTE peak, UBYTE mean);

/* For sms */
int GPRS_SmsService(void);
int GPRS_SetSmsService(BAL_SMS_SERVICE_TYPE service);


#ifdef __cplusplus
}
#endif
#endif //__BAL_GPRS_API_H_20080225__
