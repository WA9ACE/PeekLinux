/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	                                                      
 $Project code:	                                                           
 $Module:		
 $File:		    MmiGprs.h
 $Revision:		                                                      
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		                                                          
                                                                               
********************************************************************************
                                                                              
 Description:
 	Header file for GPRS handling for MMI.
    
                        
********************************************************************************

 $History: MmiGprs.h

	15/05/2003 - SPR#1983 - SH - Updated to latest from 1.6.3 version.
	   
 $End

*******************************************************************************/

#ifndef _MMI_GPRS_H_
#define _MMI_GPRS_H_

#ifndef WIN32	// EF GPRS testing enabled for windows
#undef TEST_MMIGPRS  	/* GPRS testing, gives extra menu functions */
#endif

/*******************************************************************************

 GLOBAL DEFINITIONS

*******************************************************************************/

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

#if (BOARD == 61)
/* EDGE states */
enum
{
	EDGE_NOTHING = -1,			/* Nothing - no event to send */
	EDGE_OFF = 0,				/* EDGE is not attached */
	EDGE_SEARCHING,
	EDGE_ATTACHED,				/* EDGE is attached */
	EDGE_CONTEXT_ACTIVATE,		/* PDP context is activated */
	EDGE_CONTEXT_DEACTIVATE,	/* PDP context is deactivated */
	EDGE_ERROR					/* General EDGE error */
};
#endif

typedef enum
{
	MMI_GPRS_STATUS_IDLE,
	MMI_GPRS_STATUS_ATTACHING,
	MMI_GPRS_STATUS_DETACHING,
	MMI_GPRS_STATUS_DATA_COUNTER
}E_MMI_GPRS_STATUS;

/* SMS options */

enum
{
	SMS_SERVICE_GPRS = 0,
	SMS_SERVICE_CS,
	SMS_SERVICE_GPRS_PREF,
	SMS_SERVICE_CS_PREF,
	SMS_SERVICE_OMITTED = 0xFF
};

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

/* Callback function */

typedef void (*T_GPRS_CB) (int);// Earlier argument not used

void GPRS_Init(void);
void GPRS_Exit(void);
void GPRS_SetCallback(T_MFW_HND win, T_GPRS_CB callback);

/* GPRS functionality */

int GPRS_Status(void);
int GPRS_Attach(T_MFW_HND win);
int GPRS_Detach(T_MFW_HND win);
int GPRS_ContextDefine(SHORT cid, char *IPAddress, char *APN);
int GPRS_ContextActivate(SHORT cid);
int GPRS_ContextDeactivate(SHORT cid);
int GPRS_SetQOS(SHORT cid, UBYTE preced, UBYTE delay, UBYTE relclass, UBYTE peak, UBYTE mean);
int GPRS_SetMinQOS(SHORT cid, UBYTE preced, UBYTE delay, UBYTE relclass, UBYTE peak, UBYTE mean);
UBYTE GPRS_SmsService(void);
UBYTE GPRS_SetSmsService(UBYTE service);

/* MMI functionality */

int GPRS_MenuAttach(MfwMnu* m, MfwMnuItem* i);
int GPRS_MenuDetach(MfwMnu* m, MfwMnuItem* i);
int GPRS_MenuIncomingDataCount(MfwMnu* m, MfwMnuItem* i);
int GPRS_MenuOutgoingDataCount(MfwMnu* m, MfwMnuItem* i);
int GPRS_MenuSelectSms(MfwMnu* menu, MfwMnuItem* item);
int GPRS_MenuSelectSms_set(MfwMnu* menu, MfwMnuItem* item);


/*******************************************************************************

 LOCAL DATA TYPES

*******************************************************************************/

#define ULONG_SIZE_IN_DECIMAL		10		/* Max no. of decimal digits a ULONG can have */
#define KILOBYTE					1024	/* No of bytes in a kilobyte, used for data counter calculation. */
#define PLEASEWAIT_TIMEOUT			35000	/* Time for "please wait" window to time out */

/* Counter types */

typedef enum
{
	MMI_GPRS_COUNTER_UPLINK,
	MMI_GPRS_COUNTER_DOWNLINK
} T_MMI_GPRS_COUNTER;

/* GPRS data */

typedef struct
{
	T_MFW_HND		mfw_gprs;				/* Provide a handle to the GPRS information. */
	UBYTE			cid;					/* The context id.  Starts at 1 then increments. */
	UBYTE			reg_status;				/* Registration status */
	MfwHnd 			pleasewait_win;			/* Handle for the Please Wait window */
	MfwHnd 			edit_win;				/* Handle for editor window */
	MfwHnd			menu_win;				/* Handler for menu windows */
	char			*counterstring;			/* String for counter digits */
	ULONG			uplink_counter;			/* Value of uplink counter */
	ULONG			downlink_counter;		/* Value of downlink counter */
	USHORT			counter_type;			/* TRUE if uplink counter has been requested */
	UBYTE			sms_service;				/* Type of SMS service */
	BOOL			display;				/* Whether to display "OK" windows or not */
	USHORT			status;					/* Current status */
	T_MFW_HND		notify_win;				/* Window to which to send notification */
	T_GPRS_CB		callback;				/* MMI provided callback function */
}
T_MMI_GPRS_DATA;


/*******************************************************************************

 LOCAL FUNCTION PROTOTYPES

*******************************************************************************/

static T_MFW_HND GPRS_PleaseWait(T_MFW_HND parent_win, USHORT TextId1, USHORT TextId2);
static void GPRS_PleaseWaitCb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
static void GPRS_PleaseWaitDestroy(void);
static void GPRS_ShowCounter(T_MFW_HND win);
static void GPRS_ShowCounterCb(T_MFW_HND win, USHORT identifier, USHORT reason);
static void GPRS_ResetCounter(T_MFW_HND win, UBYTE identifier, UBYTE reason);
static void GPRS_DestroyCounter(void);
static void GPRS_UpdateCounter(void);
static int GPRS_MfwCb(T_MFW_EVENT event, void* para);
#endif
