/*=================================================================================
MODULE NAME : amdb_task.cpp

MODULE NAME : bui
		
	
GENERAL DESCRIPTION
	amdb task entry.

BYD TECHFAITH Wireless Confidential Proprietary
(c) Copyright 2008 by BYD TECHFAITH Wireless. All Rights Reserved.
===================================================================================
	Revision History
	Modification Tracking
	Date Author Number Description of changes
	---------- -------------- --------- --------------------------------------
	2008-04-02 wupeng  create       

This task receive signal and Msg and handle it.

=================================================================================*/

/*=====================================================================================
	Include File Section
=====================================================================================*/
#include "sysdefs.h"
#include "stdio.h"
#include "bal_os.h"
#include "bal_def.h"
#include "amdb.h"
#include "nmea/nmea.h"
#include "DataObject.h"
#include "URL.h"

#define GPS_URI "system://local/gps"


/*******************************************************************************
*    Type Define Section
*******************************************************************************/
//the 1st Mailbox init and Msg handle function.
typedef struct
{
	void (*InitFunc)(void);
	void (*AmdbFunc)(uint32 MsgId, void * MsgBufferP, uint32 MsgSize);
}AmdbFuncsT;

/* Signal process function table */
typedef struct {
	BOSEventWaitT  SignalId;
	void           (*ProcessFunc)(void);
 } AmdbSignalFuncsT;


 
/*******************************************************************************
*    File Static Variable Define Section
*******************************************************************************/
//Email server function table.this can be modified,Max is 5
static const AmdbFuncsT AmdbFuncsTable[] = 
{
	{AmdbInitFunc,  AmdbFunc},
	{NULL, NULL}
};

//signal server function table.this can be modified,Max is 23
static const AmdbSignalFuncsT AmdbSignalFuncsTable[] = {
	{BOS_SIGNAL_1, AmdbSignalFunc},                  /* BOS_SIGNAL_1 */
 };

#define SIGNAL_TABLE_SIZE   1 



/*******************************************************************************
*    Global Variable Declare Section
*******************************************************************************/
//this use to init mailbox queue size,
//Amdb_S_TASK_MAIL_QUEUE_1 mean the 1st mailbox can handle 10 msg at one time,this can be modified.
extern "C" const MailQueueT AmdbQueueTable[] = {AMDB_S_TASK_MAIL_QUEUE_1, BOS_MAILBOX_1_ID};



/*====================================================================================
FUNCTION: AmdbTask
	
	CREATE DATE: 2008-04-02
	AUTHOR: wupeng 
	
DESCRIPTION:
	Amdb task entry.

ARGUMENTS PASSED:
none

RETURN VALUE:

	USED GLOBAL VARIABLES:
	USED STATIC VARIABLES:
	EMSMailFuncsTable
	EMSignalFuncsTable
	
CALL BY:
	core OS scheduler

IMPORTANT NOTES:
this the entry AMDB task, 
=====================================================================================*/

typedef enum {
    TR_BAUD_406250,
    TR_BAUD_115200,
    TR_BAUD_57600,
    TR_BAUD_38400,
    TR_BAUD_33900,
    TR_BAUD_28800,
    TR_BAUD_19200,
    TR_BAUD_14400,
    TR_BAUD_9600,
    TR_BAUD_4800,
    TR_BAUD_2400,
    TR_BAUD_1200,
    TR_BAUD_600,
    TR_BAUD_300,
    TR_BAUD_150,
    TR_BAUD_75
} T_tr_Baudrate;

extern "C" {
void UA_Init (uint32 uart_id,
              T_tr_Baudrate baudrate,
              void (callback_function (void)));

void UA_WriteString (uint32 uart_id,
                     char *buffer);

uint32 UA_ReadNChars (uint32 uart_id,
                           char *buffer,
                           uint32 chars_to_read);

static char lat[100];
static char lon[100];

static DataObject* GPS;
static DataObjectField* GPS_LAT;
static DataObjectField* GPS_LONG;
static void gps_set_location(char *lat, char *lon)
{
        if (!GPS)
                return;
        GPS_LAT->field.string = lat;
        GPS_LONG->field.string = lon;
        dataobject_setValue(GPS, "lat", GPS_LAT);
        dataobject_setValue(GPS, "long", GPS_LONG);
}

void gps_init()
{
        URL *url = url_parse(GPS_URI, URL_ALL);

        GPS = dataobject_construct(url, 1);
        GPS_LAT = dataobjectfield_string("");
        GPS_LONG = dataobjectfield_string("");
        dataobject_setValue(GPS, "lat", GPS_LAT);
        dataobject_setValue(GPS, "long", GPS_LONG);
}

static void serial_read_cb(void) {

    int it;
    nmeaINFO info;
    nmeaPARSER parser;
    nmeaPOS dpos;
    char locbuf[1024];

    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

    memset(locbuf, 0, 1024);

    if(UA_ReadNChars(0, locbuf, 1024) > 0) {
    	nmea_parse(&parser, locbuf, 1024, &info);
    	nmea_info2pos(&info, &dpos);
	sprintf(lat, "%f", dpos.lat);
	sprintf(lon, "%f", dpos.lon);
	if(GPS) {
		gps_set_location(lat, lon);
	}
    	//emo_printf("Lat: %f, Lon: %f, Sig: %d, Fix: %d\n", dpos.lat, dpos.lon, info.sig, info.fix);
    }
    nmea_parser_destroy(&parser);
}
}

extern "C" void AmdbTask(uint32 /*argc*/, void * /*argv*/) 
{
	BOSEventWaitT EvtStatus;
	bool          MsgStatus;
	uint32        MsgId;
	uint32        MsgSize;
	void          *MsgBufferP;
	uint8         MailBoxId;
	BOSEventWaitT MailBoxIndex;
	int i;
  	GPS = NULL;
	//wait until BAL is Ready.
	while (BalStatusGet()==FALSE)
	{
	    	//the function below is same as NU_Sleep(100);
    		BOSEventWait( BOS_AMDB_S_ID, BOS_SIGNAL_FALSE, BOS_MESSAGE_FALSE, BOSCalMsec(100) );
  	}

	UA_Init(0, TR_BAUD_600, &serial_read_cb); // Switch Baud to 4800 for GPS / Bluetooth

  	for (MailBoxId = 0; MailBoxId < AMDB_S_MAX_MAILBOXES; MailBoxId++) 
  	{
		if(AmdbFuncsTable[MailBoxId].InitFunc)
		{
			//call mailbox init handle
			AmdbFuncsTable[MailBoxId].InitFunc();
		}
  	}

	while(TRUE)
	{
		//wait signal and Msg
		EvtStatus = BOSEventWait(BOS_AMDB_S_ID, BOS_SIGNAL_TRUE, BOS_MESSAGE_TRUE, BOS_TIMEOUT_FALSE);
		//receive end signal

		if ((EvtStatus & BOS_SIGNAL_TYPE) && (EvtStatus & AMDB_S_END_SIGNAL))
		{
			//Email task will exit,Maybe this should be called when poweroff.
			return;
		}
		//receive signal
		if (EvtStatus & BOS_SIGNAL_TYPE)
		{
			for(i = 0; i < SIGNAL_TABLE_SIZE; i++)
			{
				if(AmdbSignalFuncsTable[i].SignalId & EvtStatus)
				{
					if(AmdbSignalFuncsTable[i].ProcessFunc)
					{
						//call signal handle
						AmdbSignalFuncsTable[i].ProcessFunc();
					}
				}
			}
		}

		//receive Msg		
		if(EvtStatus & BOS_MESSAGE_TYPE)
		{

			for(MailBoxIndex=BOS_MAILBOX_1,MailBoxId = BOS_MAILBOX_1_ID; MailBoxId<AMDB_S_MAX_MAILBOXES; MailBoxId++)
			{
				if(EvtStatus & MailBoxIndex)
				{

					//get the Msg value
					MsgStatus = BOSMsgRead(BOS_AMDB_S_ID, (BOSMailboxIdT)MailBoxId, &MsgId, &MsgBufferP, &MsgSize);
					if(MsgStatus)
					{
	
	
						if(AmdbFuncsTable[MailBoxId].AmdbFunc != NULL)
						{
		
							//call Msg handle
							AmdbFuncsTable[MailBoxId].AmdbFunc(MsgId,MsgBufferP,MsgSize);
						}
	
						if (MsgBufferP != NULL)
						{
			
							//free the Msg buffer.
							BOSMsgBufferFree(MsgBufferP);
							MsgBufferP = NULL;
						}
					}
				}
				MailBoxIndex = (BOSEventWaitT)(MailBoxIndex << 1);
			}
		}
	}
}/*end of EMSTask*/

