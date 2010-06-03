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
#include "balfsiapi.h"
#include "ems.h"
#include "bal_socket_api_ti.h"
#include "bal_def.h"
#include "msg.h"

#include "bal_gprs.h"
#include "psa.h"
#include "p_ppp.h"
#include "cmh.h"
#include "cmh_sm.h"

#include "markup.h"
#include "Debug.h"
#include "p_malloc.h"

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
extern "C" void flash_led();

static void TCPSocketCallback(BAL_SOCKET_IND socketInd, void* data)
{
	emo_printf("ANDREY::Received socket notification: 0x%04x\n", socketInd);
	switch (socketInd & 0xff)
	{
		case BAL_SOCK_IP_ADDR_IND:
		{
			T_NAS_ip ip;
			cmhSM_get_pdp_addr_for_CGPADDR(1, &ip);

			U8 *octets = ip.ip_address.ipv4_addr.a4;
			emo_printf("Received IP address: %d.%d.%d.%d", octets[0], octets[1], octets[2], octets[3]);
			flash_led();
		}
		break;

		case BAL_SOCK_UDP_DATA_IND:
		{
			emo_printf("Received UDP notification, 0x%08x (%s)", data, (data ? (char *)data : ""));
			flash_led();
		}
		break;

		case BAL_SOCK_RESUME_IND:
		case BAL_SOCKET_NETWORK_STATUS_IND:
		{
			emo_printf("Network status change to: %d", (socketInd >> 8) & 0xff);
			flash_led();
		}
		break;

		case BAL_SOCK_NEED_HARD_RESET_IND:
		{
			emo_printf("Bad stuff happened! Need reset");
		}
		break;

		case BAL_SOCK_TIMEOUT_IND:
		case BAl_SOCKET_NETWORK_LOST_IND:
		case BAL_SOCK_CONN_CLOSED_IND:
		case BAL_SOCK_ERROR_IND:
		case BAL_SOCK_BAERER_CLOSED_IND:
		default:
			break;
	}
}

#define NETWORK_CONFIG "/Peek/peek.cfg"

bool ReadNetworkConfig(char *&apn, char *&user, char *&pass)
{
	BalFsiHandleT handle = NULL;
	BalFsiResultT ret = FSI_ERR_UNKNOWN;

	ret = BalFsiFileOpen(&handle, NETWORK_CONFIG, FSI_FILE_OPEN_READ_EXIST);
	if (ret != FSI_SUCCESS)
	{
		emo_printf("Failed to open file: %s", NETWORK_CONFIG);
		return false;
	}

	BalFsiFileAttribT attr;
	ret = BalFsiGetFileHandleAttrib(handle, &attr);
	if (ret != FSI_SUCCESS)
	{
		emo_printf("Failed to get attributes for file: %s", NETWORK_CONFIG);
		BalFsiFileClose(handle);
		return false;
	}

	CMarkup markup;
	uint32 nLen = attr.Size;
	char *contents = (char *)p_malloc((nLen + 2) * sizeof(char));
	memset(contents, 0, nLen + 2);

	ret = BalFsiFileRead(contents, 1, &nLen, handle);
	if (ret != FSI_SUCCESS)
	{
		emo_printf("Failed to read from file: %s", NETWORK_CONFIG);
		BalFsiFileClose(handle);
		p_free(contents);
		return false;
	}

	if (!markup.LoadFromString(contents))
	{
		emo_printf("Failed to parse file: %s", NETWORK_CONFIG);
		p_free(contents);
		BalFsiFileClose(handle);
		return false;
	}

	markup.ResetPos();
	if (markup.FindElem("Peek"))
	{
		if (markup.FindChildElem("apn"))
		{
			const char *str = markup.GetChildData();
			apn = (char *)p_malloc(strlen(str) + 1);
			strcpy(apn, str);
		}
		else
			emo_printf("Failed to find apn in %s", NETWORK_CONFIG);

		if (markup.FindChildElem("username"))
		{
			const char *str = markup.GetChildData();
			user = (char *)p_malloc(strlen(str) + 1);
			strcpy(user, str);
		}
		else
			emo_printf("Failed to find username in %s", NETWORK_CONFIG);

		if (markup.FindChildElem("password"))
		{
			const char *str = markup.GetChildData();
			pass = (char *)p_malloc(strlen(str) + 1);
			strcpy(pass, str);
		}
		else
			emo_printf("Failed to find password in %s", NETWORK_CONFIG);
	}
	else
	{
		emo_printf("Malformed config file: %s", NETWORK_CONFIG);
	}

	p_free(contents);
	BalFsiFileClose(handle);
	return true;
}

void EMSInitFunc(void) 
{
	bal_printf("ANDREY::EMSInitFun(): Initializing network...");

	// initialize the network connection
	char *apn = NULL, *user = NULL, *pass = NULL;
	if (!ReadNetworkConfig(apn, user, pass))
		return;

	if (apn && user && pass)
	{
		//bal_set_profile("wap.cingular", "WAP@CINGULARGPRS.COM", "CINGULAR1");
		bal_set_profile(apn, user, pass);
		bal_set_network_cb((BAL_NETWORK_CB)TCPSocketCallback);
	}
	else
		emo_printf("Failed to get network info from config %s, apn: %s, user: %s, pass: %s", 
			NETWORK_CONFIG, apn ? apn : "NULL", user ? user : "NULL", pass ? pass : "NULL");

	if (apn) p_free(apn);
	if (user) p_free(user);
	if (pass) p_free(pass);
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


