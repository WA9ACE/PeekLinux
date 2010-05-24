/*=================================================================================

	HEADER NAME : Ems.h
	MODULE NAME : BUI
	
	PRE-INCLUDE FILES DESCRIPTION
	
	
GENERAL DESCRIPTION
	email server Msg and Signal handle

BYD TECHFAITH Wireless Confidential Proprietary
(c) Copyright 2008 by BYD TECHFAITH Wireless. All Rights Reserved.
===================================================================================
Revision History
Modification Tracking
Date Author Number Description of changes
---------- ------------ --------- --------------------------------------------
2008-02-19 Randolph Wang create		

	
=================================================================================*/
/*******************************************************************************
*    Multi-Include-Prevent Section
*******************************************************************************/
#ifndef EMS_H
#define EMS_H

/*******************************************************************************
*    Include File Section
*******************************************************************************/
#include "sysdefs.h"

/*******************************************************************************
*    Macro Define Section
*******************************************************************************/
//Email Server only have one Mailbox,this can be modified.Max is 5
#define EM_S_MAX_MAILBOXES	1

//send this signal to end Email server task.
#define EM_S_END_SIGNAL		BOS_SIGNAL_20



/*******************************************************************************
*    Prototype Declare Section
*******************************************************************************/
void EMSInitFunc(void);
void EMSMailFunc(uint32 MsgId, void* MsgDataP, uint32 MsgSize);
void EMSSignalFunc(void);

#endif

