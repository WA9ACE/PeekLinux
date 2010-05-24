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
#include "bal_os.h"
#include "bal_def.h"
#include "amdb.h"



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
  
	//wait until BAL is Ready.
	while (BalStatusGet()==FALSE)
	{
	    	//the function below is same as NU_Sleep(100);
    		BOSEventWait( BOS_AMDB_S_ID, BOS_SIGNAL_FALSE, BOS_MESSAGE_FALSE, BOSCalMsec(100) );
  	}

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

