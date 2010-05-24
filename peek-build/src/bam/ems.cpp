/*=================================================================================
MODULE NAME : Ems.c
MODULE NAME : bui
	
	
	
GENERAL DESCRIPTION
	email server Msg and Signal handle
	
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
#include "bal_socket_api_ti.h"
#include "bal_def.h"
#include "msg.h"

/*====================================================================================
	FUNCTION: EMSMailFunc
	
	CREATE DATE: 2008-02-19
	AUTHOR: Randolph 
	
DESCRIPTION:
Handle the Msg received by the first Mailbox of Email server

ARGUMENTS PASSED:
uint32 MsgId, void* MsgDataP, uint32 MsgSize

RETURN VALUE:

	USED GLOBAL VARIABLES:

	
	USED STATIC VARIABLES:
	
CALL BY:
	EMSTask
	
IMPORTANT NOTES:


=====================================================================================*/
#ifdef EMO_SIM
extern "C" uint32 SimReadReg(void);
#endif
extern "C" void bal_set_network_cb(BAL_NETWORK_CB);
extern ExeSemaphoreT *bal_socket_recv_seamphore;

static void TCPSocketCallback(BAL_SOCKET_IND socketInd)
{
	bal_printf("ANDREY::Received socket notification: 0x%04x\n", socketInd);
}

void EMSInitFunc(void) 
{
	bal_printf("ANDREY::EMSInitFun(): Initializing network...");

	// initialize the network connection
	bal_set_profile("wap.cingular", "WAP@CINGULARGPRS.COM", "CINGULAR1");
	bal_set_network_cb((BAL_NETWORK_CB)TCPSocketCallback);
}

void EMSMailFunc(uint32 MsgId, void* MsgDataP, uint32 MsgSize)
{
	
	switch (MsgId)
	{ 
		case EM_S_SCHEDULER_INIT_APPT:
		{	
			emsMsg * pM = (emsMsg *)MsgDataP;
			MonPrintf("receive EM_S_SCHEDULER_INIT_APPT Msg:%d,%d,%d\r\n", pM->msgA, pM->msgB,MsgSize);
		}
			break;
		case EM_S_SCHEDULER_PREPARE_APPT:
			MonPrintf("receiver EM_S_SCHEDULER_PREPARE_APPT Msg");
			break;

		case EM_S_SOCKET:
		{
			bal_printf("ANDREY::Got socket request...");

			int fd = bal_socket(0 /*AF_INET*/, 0 /*SOCK_STREAM*/, 0 /*IPPROTO_TCP*/);

			emsMsg *sockReply = (emsMsg *)MsgDataP;

			memset(sockReply, 0, sizeof(emsMsg));
			sockReply->fd = fd;

			bal_printf("ANDREY::Dispatching socket reply...");
			BOSMsgSend(BOS_UI_ID, BOS_MAILBOX_1_ID, EM_S_SOCKET, (void *)sockReply, sizeof(emsMsg));
		}
		break;

		case EM_S_CONNECT:
		{
			bal_printf("ANDREY::Got connect request...");

			struct in_addr addr;
			struct sockaddr_in connect_in;

			emsMsg *connectReq = (emsMsg *)MsgDataP;

			memset(&connect_in, 0, sizeof(struct sockaddr_in));
			connect_in.sin_family = 0; /*AF_INET*/
			connect_in.sin_port = bal_htons(connectReq->port);
			connect_in.sin_addr.s_addr = bal_inet_addr(connectReq->host);

			connectReq->msgA = bal_connect(connectReq->fd, &connect_in, sizeof(connect_in));

			bal_printf("ANDREY::Dispatching connect reply...status: %d", connectReq->msgA);
			BOSMsgSend(BOS_UI_ID, BOS_MAILBOX_1_ID, EM_S_CONNECT, (void *)connectReq, sizeof(emsMsg));
		}
		break;

		case EM_S_RECV:
		{
			emsMsg *recvReq = (emsMsg *)MsgDataP;

			int readSize = recvReq->readSize;
			bal_printf("ANDREY::Got recv(%d, %x, %d)", recvReq->fd, recvReq->readBuffer, recvReq->readSize);
			recvReq->readSize = bal_recv(recvReq->fd, recvReq->readBuffer, recvReq->readSize, 0);

			bal_printf("ANDREY::Dispatching recv(%d, %x, %d) = %d", recvReq->fd, recvReq->readBuffer, readSize, recvReq->readSize);
			BOSMsgSend(BOS_UI_ID, BOS_MAILBOX_1_ID, EM_S_RECV, (void *)recvReq, sizeof(emsMsg));
		}
		break;

		case EM_S_PEEK:
		{
			bal_printf("ANDREY::Got peek request...");

			emsMsg *peekReq = (emsMsg *)MsgDataP;

#ifdef EMO_SIM
			int hasData = SimReadReg();
#else
			int hasData = *(int *)(((char *)0x007a9820) + 0x28);
#endif

			if (hasData > 0)
				peekReq->readSize = bal_recv(peekReq->fd, peekReq->readBuffer, peekReq->readSize, 0);
			else
				peekReq->readSize = -1;

			bal_printf("ANDREY::Dispatching recv reply...");
			BOSMsgSend(BOS_UI_ID, BOS_MAILBOX_1_ID, EM_S_PEEK, (void *)peekReq, sizeof(emsMsg));
		}
		break;

		case EM_S_SEND:
		{
			bal_printf("ANDREY::Got send request...");

			emsMsg *sendReq = (emsMsg *)MsgDataP;
			sendReq->writeSize = bal_send(sendReq->fd, sendReq->writeBuffer, sendReq->writeSize, 0);

			bal_printf("ANDREY::Dispatching send reply...");
			BOSMsgSend(BOS_UI_ID, BOS_MAILBOX_1_ID, EM_S_SEND, (void *)sendReq, sizeof(emsMsg));
		}
		break;

		case EM_S_CLOSE:
		{
			bal_printf("ANDREY::Got close request...");

			emsMsg *closeReq = (emsMsg *)MsgDataP;
			closeReq->msgA = bal_closesocket(closeReq->fd);

			bal_printf("ANDREY::Dispatching close reply...");
			BOSMsgSend(BOS_UI_ID, BOS_MAILBOX_1_ID, EM_S_CLOSE, (void *)closeReq, sizeof(emsMsg));
		}
		break;


		default:
		break;
	}
}


/*====================================================================================
	FUNCTION: EMSMailFunc
	
	CREATE DATE: 2008-02-19
	AUTHOR: Randolph 
	
DESCRIPTION:
email mailbox init

ARGUMENTS PASSED:


RETURN VALUE:

	USED GLOBAL VARIABLES:

	USED STATIC VARIABLES:
	
CALL BY:
	EMSTask
	
IMPORTANT NOTES:

=====================================================================================*/

/*====================================================================================
	FUNCTION: EMSSignalFunc
	
	CREATE DATE: 2008-02-19
	AUTHOR: Randolph 
	
DESCRIPTION:
Handle the Signal received by the first Mailbox of Email server

ARGUMENTS PASSED:


RETURN VALUE:

	USED GLOBAL VARIABLES:

	
	USED STATIC VARIABLES:
	
CALL BY:
	EMSTask
	
IMPORTANT NOTES:

=====================================================================================*/
void EMSSignalFunc(void)
{
	bal_printf("ANDREY::Received a signal!");

	//   TODO: Add BOS_SIGNAL_1 handle process here
}

/*****************************************************
*
*	!!!!this below is send msg example
*
******************************************************/
extern "C"
{
void EMStestsendSignal()
{
	//send to Email server task signal 1
	BOSSignalSet(BOS_EM_S_ID, BOS_SIGNAL_1);
}


void EMStestsendMsg()
{
	emsMsg *testMsg = NULL;
	testMsg = (emsMsg *)BOSMsgBufferGet(sizeof(emsMsg));

		//check the BOSMsgBufferGet() return value to avoid NULL pointer!
	  //	2008-5-8 ZhangXi
	
	if(!testMsg){
		MonPrintf("Alert!!! There has no MSG buffer!\r\n");
		return;
	}
	testMsg->msgA=1;
	testMsg->msgB=2;
	//send to Email server task,the 1st mailbox, msg Id = M_S_SCHEDULER_INIT_APPT
	BOSMsgSend( BOS_EM_S_ID, BOS_MAILBOX_1_ID, EM_S_SCHEDULER_INIT_APPT, (void *)testMsg, sizeof(emsMsg)  );	
}

}


