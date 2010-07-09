/*=================================================================================
	MODULE NAME : Emailstask.c

	MODULE NAME : bui


	GENERAL DESCRIPTION
	Email server task entry.

	BYD TECHFAITH Wireless Confidential Proprietary
	(c) Copyright 2008 by BYD TECHFAITH Wireless. All Rights Reserved.
	===================================================================================
	Revision History
	Modification Tracking
	Date Author Number Description of changes
	---------- -------------- --------- --------------------------------------
	2008-02-19 Randolph Wang  create       

	This task receive signal and Msg and handle it.

	=================================================================================*/

/*=====================================================================================
	Include File Section
	=====================================================================================*/
#include "sysdefs.h"
#include "bal_os.h"
#include "bal_def.h"
#include "ems.h"


/*******************************************************************************
 *    Type Define Section
 *******************************************************************************/
//the 1st Mailbox init and Msg handle function.
typedef struct
{
	void (*InitFunc)(void);
	void (*MailFunc)(uint32 MsgId, void * MsgBufferP, uint32 MsgSize);
}EMSMailFuncsT;

/* Signal process function table */
typedef struct {
	BOSEventWaitT  SignalId;
	void           (*ProcessFunc)(void);
} EMSignalFuncsT;



/*******************************************************************************
 *    File Static Variable Define Section
 *******************************************************************************/
//Email server function table.this can be modified,Max is 5
static const EMSMailFuncsT EMSMailFuncsTable[] = 
{
	{EMSInitFunc,  EMSMailFunc},
	{NULL, NULL}
};

//signal server function table.this can be modified,Max is 23
static const EMSignalFuncsT EMSignalFuncsTable[] = {
	{BOS_SIGNAL_1, EMSSignalFunc},                  /* BOS_SIGNAL_1 */
};

#define SIGNAL_TABLE_SIZE   1 //( sizeof(EMSignalFuncsTable) / sizeof(EMSignalFuncsT[0]) )



/*******************************************************************************
 *    Global Variable Declare Section
 *******************************************************************************/
//this use to init mailbox queue size,
//EM_S_TASK_MAIL_QUEUE_1 mean the 1st mailbox can handle 10 msg at one time,this can be modified.
const MailQueueT EMSMailQueueTable[] = {EM_S_TASK_MAIL_QUEUE_1, BOS_MAILBOX_1_ID};



/*====================================================================================
FUNCTION: EMSTask

CREATE DATE: 2008-02-19
AUTHOR: Randolph 

DESCRIPTION:
EMail server task entry.

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
this the entry EMail Server task, 
=====================================================================================*/

void EMSTask(uint32 argc, void *argv) 
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
	while (BalStatusGet() == FALSE)
	{
		//the function below is same as NU_Sleep(100);
		BOSEventWait( BOS_EM_S_ID, BOS_SIGNAL_FALSE, BOS_MESSAGE_FALSE, BOSCalMsec(100));
	}

	//call mailbox init handle
	for (MailBoxId = 0; MailBoxId < EM_S_MAX_MAILBOXES; MailBoxId++) 
		if(EMSMailFuncsTable[MailBoxId].InitFunc)
			EMSMailFuncsTable[MailBoxId].InitFunc();

	while (TRUE)
	{
		// wait signal and Msg
		EvtStatus = BOSEventWait(BOS_EM_S_ID, BOS_SIGNAL_TRUE, BOS_MESSAGE_TRUE, BOS_TIMEOUT_FALSE);

		//receive end signal
		if ((EvtStatus & BOS_SIGNAL_TYPE) && (EvtStatus & EM_S_END_SIGNAL))
		{
			//Email task will exit,Maybe this should be called when poweroff.
			return;
		}

		//receive signal
		if (EvtStatus & BOS_SIGNAL_TYPE)
		{
			for(i = 0; i < SIGNAL_TABLE_SIZE; i++)
			{
				if(EMSignalFuncsTable[i].SignalId & EvtStatus)
				{
					if(EMSignalFuncsTable[i].ProcessFunc)
					{
						//call signal handle
						EMSignalFuncsTable[i].ProcessFunc();
					}
				}
			}
		}

		//receive Msg		
		if(EvtStatus & BOS_MESSAGE_TYPE)
		{
			for(MailBoxIndex=BOS_MAILBOX_1,MailBoxId = BOS_MAILBOX_1_ID; MailBoxId<EM_S_MAX_MAILBOXES; MailBoxId++)
			{
				if(EvtStatus & MailBoxIndex)
				{
					//get the Msg value
					MsgStatus = BOSMsgRead(BOS_EM_S_ID, (BOSMailboxIdT)MailBoxId, &MsgId, &MsgBufferP, &MsgSize);
					if(MsgStatus)
					{

						if(EMSMailFuncsTable[MailBoxId].MailFunc != NULL)
						{
							//call Msg handle
							EMSMailFuncsTable[MailBoxId].MailFunc(MsgId,MsgBufferP,MsgSize);
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
