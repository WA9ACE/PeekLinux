/*=================================================================================

	HEADER NAME : amdb.h
	MODULE NAME : BUI
	
	PRE-INCLUDE FILES DESCRIPTION
	
	
GENERAL DESCRIPTION
	AMDB Msg and Signal handle

BYD TECHFAITH Wireless Confidential Proprietary
(c) Copyright 2008 by BYD TECHFAITH Wireless. All Rights Reserved.
===================================================================================
Revision History
Modification Tracking
Date Author Number Description of changes
---------- ------------ --------- --------------------------------------------
2008-04-02 wupeng create		

	
=================================================================================*/
/*******************************************************************************
*    Multi-Include-Prevent Section
*******************************************************************************/
#ifndef AMDB_H
#define AMDB_H

/*******************************************************************************
*    Include File Section
*******************************************************************************/
#include "sysdefs.h"

/*******************************************************************************
*    Macro Define Section
*******************************************************************************/
//AMDB only have one Mailbox,this can be modified.Max is 5
#define AMDB_S_MAX_MAILBOXES	1

//send this signal to end AMDB task.
#define AMDB_S_END_SIGNAL		BOS_SIGNAL_20

/*******************************************************************************
*    Type Define Section
*******************************************************************************/
typedef enum
{
  AMDB_S_SCHEDULER_INIT_APPT,
  AMDB_S_SCHEDULER_PREPARE_APPT
}AMDBMsgIdT;


/*******************************************************************************
*    Prototype Declare Section
*******************************************************************************/
void AmdbInitFunc(void);
void AmdbFunc(uint32 MsgId, void* MsgDataP, uint32 MsgSize);
void AmdbSignalFunc(void);

#endif
/*****************************************************************************
* End of File
*****************************************************************************/
