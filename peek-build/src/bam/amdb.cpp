/*=================================================================================
MODULE NAME : Amdb.cpp
MODULE NAME : bui
	
	
	
GENERAL DESCRIPTION
	Amdb Msg and Signal handle
	
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
#include "amdb.h"
#include "bal_os.h"
#include "bal_socket_api_ti.h"




typedef struct _emsMsg
{
	int msgA;
	int msgB;
}emsMsg;




/*====================================================================================
	FUNCTION: AmdbMailFunc
	
	CREATE DATE: 2008-04-02
	AUTHOR: wupeng 
	
DESCRIPTION:
Handle the Msg received by the first Mailbox of Email server

ARGUMENTS PASSED:
uint32 MsgId, void* MsgDataP, uint32 MsgSize

RETURN VALUE:

	USED GLOBAL VARIABLES:
	USED STATIC VARIABLES:
	
CALL BY:
	AmdbTask
	
IMPORTANT NOTES:

=====================================================================================*/
void AmdbFunc(uint32 MsgId, void* MsgDataP, uint32 MsgSize)
{

	switch (MsgId)
	{ 
		case AMDB_S_SCHEDULER_INIT_APPT:
		{	
			emsMsg * pM = (emsMsg *)MsgDataP;
			MonPrintf("receive AMDB_S_SCHEDULER_INIT_APPT Msg:%d,%d,%d\r\n", pM->msgA, pM->msgB,MsgSize);
		}
			break;
		case AMDB_S_SCHEDULER_PREPARE_APPT:
			MonPrintf("receiver AMDB_S_SCHEDULER_PREPARE_APPT Msg");

			break;
		default:
			break;
	}
}

static void network_cb(BAL_SOCKET_IND ind) {

	switch(ind) {
        	case BAL_SOCK_IP_ADDR_IND:
                	break;
        	case BAL_SOCK_UDP_DATA_IND:
                	break;
        	case BAL_SOCK_NEED_HARD_RESET_IND:
               		break;
        	case BAL_SOCKET_NETWORK_STATUS_IND:
                	break;
        	case BAL_SOCK_RESUME_IND:
                	break;
        	case BAl_SOCKET_NETWORK_LOST_IND:
        	case BAL_SOCK_CONN_CLOSED_IND:
        	case BAL_SOCK_ERROR_IND:
        	case BAL_SOCK_TIMEOUT_IND:
        	case BAL_SOCK_BAERER_CLOSED_IND:
                	break;

    	  default:
        	break;
	}
}

extern "C" void bal_set_network_cb(BAL_NETWORK_CB);

/*====================================================================================
	FUNCTION: AmdbFunc
	
	CREATE DATE: 2008-04-02
	AUTHOR: wupeng 
	
DESCRIPTION:
Amdb mailbox init

ARGUMENTS PASSED:


RETURN VALUE:

	USED GLOBAL VARIABLES:

	USED STATIC VARIABLES:
	
CALL BY:
	AmdbTask
	
IMPORTANT NOTES:

=====================================================================================*/
void  AmdbInitFunc(void)
{
//	bal_set_network_cb(network_cb);
}


/*====================================================================================
	FUNCTION: AmdbSignalFunc
	
	CREATE DATE: 2008-04-02
	AUTHOR: wupeng
	
DESCRIPTION:
Handle the Signal  

ARGUMENTS PASSED:


RETURN VALUE:

	USED GLOBAL VARIABLES:

	
	USED STATIC VARIABLES:
	
CALL BY:
	AmdbTask
	
IMPORTANT NOTES:

=====================================================================================*/
void AmdbSignalFunc(void)
{
	//   TODO: Add BOS_SIGNAL_1 handle process here
}


#if 1 /*example*/
/*****************************************************
*
*	!!!!this below is send msg example
*
******************************************************/
extern "C"
{
void AmdbtestsendSignal()
{
	//send to Email server task signal 1
	BOSSignalSet(BOS_AMDB_S_ID, BOS_SIGNAL_1);
}


void AmdbtestsendMsg()
{
	emsMsg *testMsg = NULL;
	testMsg = (emsMsg *)BOSMsgBufferGet(sizeof(emsMsg));
	/*
		check the BOSMsgBufferGet() return value to avoid NULL pointer!
		2008-5-8 ZhangXi
	*/
	if(!testMsg){
		MonPrintf("Alert!!! There has no MSG buffer!--Line:%d,in file %s\r\n", __LINE__, __FILE__);	
		return;
	}
	testMsg->msgA=1;
	testMsg->msgB=2;
	//send to Email server task,the 1st mailbox, msg Id = M_S_SCHEDULER_INIT_APPT
	BOSMsgSend( BOS_AMDB_S_ID, BOS_MAILBOX_1_ID, AMDB_S_SCHEDULER_INIT_APPT, (void *)testMsg, sizeof(emsMsg)  );	
}

}
#endif

