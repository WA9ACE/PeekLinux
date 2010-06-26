
#include "btl_config.h"
#include "osapi.h"
#if BTL_CONFIG_BTL_L2CAP == BTL_CONFIG_ENABLED

/* GPF Includes */
#include "typedefs.h"
#include "vsi.h"
#include "../p_btt.h"

#if BT_STACK == XA_ENABLED
#include <me.h>
#include <bttypes.h>
#include <sec.h>
#endif

#include <stdlib.h>
#include "conmgr.h"
#include "debug.h"
#include "../app_main.h"
#include "lineparser.h"
#include "l2cap.h"
#include "btl_l2cap.h"
#include "btl_config.h"
#include "btl_bmg.h"

#include "btl_log.h"

#define TARGET_PSM      			0x0025  /* PSM registered by the server */
#define SIZE_OF_1MB					(1024 * 1024 *1)

typedef enum _L2CAP_Role
{
	L2CAP_ROLE_NONE		= 0,
	L2CAP_ROLE_CLIENT	= 1,
	L2CAP_ROLE_SERVER	= 2
} L2CAP_Role;

typedef BOOL (*L2capFuncType)(void);

typedef struct _L2capCommand
{
	char	funcName[LINE_PARSER_MAX_STR_LEN];	
	L2capFuncType funcPtr;
} L2capCommand;

static BOOL L2CAPA_THROUGHPUT_Init(void);
static BOOL L2CAPA_THROUGHPUT_Deinit(void);
static BOOL L2CAPA_THROUGHPUT_Connect(void);
static BOOL L2CAPA_THROUGHPUT_Disconnect(void);
static BOOL L2CAPA_THROUGHPUT_Register(void);
static BOOL L2CAPA_THROUGHPUT_Deregister(void);
static BOOL L2CAPA_THROUGHPUT_SendData(void);
static BOOL L2CAPA_THROUGHPUT_SetTxDataLen(void);
static BOOL L2CAPA_THROUGHPUT_Abort(void);

static void BtlL2capAppCallBack(const BtlL2capEvent *event);
static void L2AppBtlBmgCallback(const BtlBmgEvent *bmgEvent);
static BOOL InitL2capData(void);

static const L2capCommand l2capAppCommands[] = 
{
	{"init", L2CAPA_THROUGHPUT_Init},
	{"deinit", L2CAPA_THROUGHPUT_Deinit},
	{"connect", L2CAPA_THROUGHPUT_Connect},
	{"disconnect", L2CAPA_THROUGHPUT_Disconnect},
	{"register", L2CAPA_THROUGHPUT_Register},
	{"deregister", L2CAPA_THROUGHPUT_Deregister},
	{"set_tx_data_len", L2CAPA_THROUGHPUT_SetTxDataLen},
	{"send_data", L2CAPA_THROUGHPUT_SendData},
	{"abort", L2CAPA_THROUGHPUT_Abort}
};

#define L2CAP_APP_NUM_OF_COMMANDS (sizeof(l2capAppCommands) /sizeof(L2capCommand))

/* Global variables */
static BtlL2capChannelSettings channelSettings;
static BtlL2capContext *activeL2capContext = 0;
static L2capConnSettings connSettings = {0};
static BtlL2capChannelId channelId = 0xffff;
static BtRemoteDevice *RemDev = 0;
static L2CAP_Role role;
static BtPacket ClientPkt;
static U8       TxData[L2CAP_MAXIMUM_MTU];
static L2capPsmValue psm = 0;
static BOOL serverRegisteredInd = FALSE;
static BOOL connectionInd = FALSE;
static BOOL sendInd = FALSE;
static U16	txDataLen = L2CAP_MAXIMUM_MTU;

static U32 startRxTime = 0;
static U32 lastRxTime = 0;
static U32 totalRxTime = 0;
static U32 lastRxDataLen = 0;
static U32 totalRxDataLen = 0;

static U32 startTxTime = 0;
static U32 lastTxTime = 0;
static U32 totalTxTime = 0;
static U32 lastTxDataLen = 0;
static U32 totalTxDataLen = 0;

/* When utilizing L2CAP Flow Control, the L2CAP User must provide a reassembly
 * buffer. The buffer must be as large as the maximum MTU stated in the PSM struct.
 */
#if L2CAP_FLOW_CONTROL == XA_ENABLED
static U8 ClientRxBuffer[L2CAP_MAXIMUM_MTU];
static U8 ServerRxBuffer[L2CAP_MAXIMUM_MTU];
#endif

void L2CAPA_THROUGHPUT_ProcessUserAction(U8 *msg)
{
	U32 idx;
	char command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS status;
	BOOL success = FALSE;

	status = LINE_PARSER_GetNextStr((U8*)command, LINE_PARSER_MAX_STR_LEN);
	if (LINE_PARSER_STATUS_SUCCESS != status)
	{
		Report(("L2CAP_TP APP: command parsing failed"));
		return;
	}

	for(idx = 0; idx < L2CAP_APP_NUM_OF_COMMANDS; ++idx)
	{
		if (0 == OS_StrCmp(command, l2capAppCommands[idx].funcName))
		{
			success = l2capAppCommands[idx].funcPtr();
		}
	}
	
	if (FALSE == success)
	{
		Report(("L2CAP_TP APP: user action failed"));
	}
}

BOOL L2CAPA_THROUGHPUT_Init(void)
{
	BOOL sts;
	
	if (0 != activeL2capContext)
	{
		Report(("L2CAP_TP APP L2CAPA_THROUGHPUT_Init: Application was already initialized"));
		return FALSE;
	}
		
	sts = InitL2capData();

	BTL_LOG_ERROR(("L2CAP_TP APP status after InitL2capData() is %d", sts));

	return sts;
}

BOOL L2CAPA_THROUGHPUT_Deinit(void)
{
	if (0 == activeL2capContext)
	{
		BTL_LOG_ERROR(("L2CAP_TP APP: Application was already deinitialized"));
		return FALSE;
	}
	
	if (TRUE == serverRegisteredInd)
	{
		Report(("L2CAP_TP APP: Please deregister first."));
		return FALSE;
	}
	if (TRUE == connectionInd)
	{
		Report(("L2CAP_TP APP: Please disconnect first."));
		return FALSE;
	}

	if (BT_STATUS_SUCCESS != BTL_L2CAP_Destroy(&activeL2capContext))
	{
		Report(("L2CAP_TP APP: Error after BTL_L2CAP_Destroy()."));
		return FALSE;
	}		

	activeL2capContext = 0;

	Report(("L2CAP_TP APP: L2CAPA_THROUGHPUT_Deinit() return TRUE."));
	
	return TRUE;
}

static BOOL L2CAPA_THROUGHPUT_Register()
{
	BtStatus status;
	
	/* Check we didn't registered yet */
	if (psm != 0)
	{
		Report(("L2CAP_TP APP: Error in L2CAPA_THROUGHPUT_Register. Psm is registered"));
		return FALSE;
	}

	/* Verify it is free */
	if (role != L2CAP_ROLE_NONE)
	{
		Report(("L2CAP_TP APP: Error in L2CAPA_THROUGHPUT_Register. Not free role"));
		return FALSE;
	}
	
	/* set PSM */
	psm = TARGET_PSM;
	
	/* Get selected PSM. */
	status = BTL_L2CAP_RegisterPsm(activeL2capContext, &channelSettings, &psm);
	BTL_LOG_ERROR(("L2CAP_TP Server: BTL_L2CAP_RegisterPsm() Returned %s", pBT_Status(status)));

	if (status == BT_STATUS_SUCCESS)
    {
    	/* Mark as a server. */
        role = L2CAP_ROLE_SERVER;
		serverRegisteredInd = TRUE;
		return TRUE;
	}

	return FALSE;
}

static BOOL L2CAPA_THROUGHPUT_SetTxDataLen()
{
	char txDataLenStr[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS parserStatus;
	U16 dataLen;
	
	parserStatus = LINE_PARSER_GetNextStr((U8*)txDataLenStr, LINE_PARSER_MAX_STR_LEN);
	if (LINE_PARSER_STATUS_SUCCESS != parserStatus)
	{
		Report(("L2CAP_TP APP: Tx Data Length Parsing failed"));
		return FALSE;
	}

	dataLen = (U16)atoi(txDataLenStr);
	if (0 == dataLen)
	{
		Report(("L2CAP_TP APP: Invalid Tx Data Length - '%s'", txDataLenStr));
		return FALSE;
	}

	if (dataLen > L2CAP_MAXIMUM_MTU)
	{
		Report(("L2CAP_TP APP: Tx Data Length (%d) is bigger then max (%d). Using default (%d)",
			dataLen, L2CAP_MAXIMUM_MTU, L2CAP_MAXIMUM_MTU));
		txDataLen = L2CAP_MAXIMUM_MTU;
		return FALSE;
	}

	txDataLen = dataLen;

	Report(("L2CAP_TP APP: Tx Data Length was set to '%d'", txDataLen));

	return TRUE;
}

static BOOL L2CAPA_THROUGHPUT_Deregister()
{
	BtStatus status;
	
	/* Check we are registered */
	if (0 == psm)
	{
		Report(("L2CAP_TP APP: Error in L2CAPA_THROUGHPUT_Deregister. Psm is not registered"));
		return FALSE;
	}

	/* Verify it is server */
	if (role != L2CAP_ROLE_SERVER)
	{
		Report(("L2CAP_TP APP: Error in L2CAPA_THROUGHPUT_Deregister. Not a server"));
		return FALSE;
	}
	
	status = BTL_L2CAP_DeregisterPsm(activeL2capContext);
    Report(("L2CAP_TP Server: BTL_L2CAP_DeregisterPsm() returned %s", pBT_Status(status)));

	if (BT_STATUS_SUCCESS == status)
	{
		/* Mark as a not used. */
		psm = 0;
		role = L2CAP_ROLE_NONE;
		serverRegisteredInd = FALSE;
		return TRUE;
	}

	return FALSE;
}

static BOOL L2CAPA_THROUGHPUT_Connect()
{
	BtStatus status;
	BD_ADDR addr;
	char addrString[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS parserStatus;
	
	if (role != L2CAP_ROLE_NONE)
	{
		Report(("L2CAP_TP APP: Can't connect. Server app or connected client"));
		return FALSE;
	}

	parserStatus = LINE_PARSER_GetNextStr((U8*)addrString, LINE_PARSER_MAX_STR_LEN);
	if (LINE_PARSER_STATUS_SUCCESS != parserStatus)
	{
		Report(("L2CAP_TP APP: BD Address parsing failed"));
		return FALSE;
	}

	addr = bdaddr_aton(addrString);

	status = BTL_L2CAP_Connect(activeL2capContext,
                           	   &addr,
                           	   TARGET_PSM,
                           	   &connSettings,
                           	   &channelSettings,
                           	   &channelId);

	BTL_LOG_ERROR(("L2CAP_TP APP: status after BTL_L2CAP_Connect() %d.", status));

	if ((BT_STATUS_PENDING == status) || (BT_STATUS_SUCCESS == status))
	{
		role = L2CAP_ROLE_CLIENT;
		return TRUE;
	}

	return FALSE;
}

static BOOL L2CAPA_THROUGHPUT_Disconnect()
{
	BtStatus status = BTL_L2CAP_Disconnect(activeL2capContext, channelId);
	
	BTL_LOG_ERROR(("L2CAP_TP APP Client: BTL_L2CAP_Disconnect() Returned %s", channelId, pBT_Status(status)));

	if (BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status)
	{
		return TRUE;
	}

	return FALSE;
}

static BOOL L2CAPA_THROUGHPUT_Abort()
{
	BTL_LOG_ERROR(("L2CAP_TP Client: Send data abort"));
	sendInd = FALSE;
	return TRUE;
}

static BOOL L2CAPA_THROUGHPUT_SendData()
{
	BtStatus status;
	
	sendInd = TRUE;
	
    if (ClientPkt.flags & BTP_FLAG_INUSE) 
	{
		Report(("L2CAP_TP Client: Cannot Send Packet because it is In Use."));
		sendInd = FALSE;
		return FALSE;
	}
	BTHAL_OS_GetSystemTime(&startTxTime);
	lastTxTime = startTxTime;
	totalTxTime = 0;
	totalTxDataLen = 0;
	
	status = BTL_L2CAP_SendData(activeL2capContext, channelId, TxData, txDataLen, (BtPacket **)&ClientPkt);
	if (status != BT_STATUS_PENDING)
	{
		Report(("L2CAP_TP: Error after BTL_L2CAP_SendData(). status is %d", status));
		sendInd = FALSE;
		return FALSE;
	}

	return TRUE;
}

static void L2AppBtlBmgCallback(const BtlBmgEvent *bmgEvent)
{
	const BtEvent *Event = bmgEvent->event;
		
    switch (Event->eType) {
    case BTEVENT_LINK_CONNECT_IND:
    case BTEVENT_LINK_CONNECT_CNF:
        if (Event->errCode == BEC_NO_ERROR) {
			RemDev = Event->p.remDev;
            Report(("L2CAP_TP APP: ACL connection established."));
        } else {
            Report(("L2CAP_TP APP: ACL connection failed, reason = %s.", pHC_Status(Event->errCode)));
            RemDev = 0;
        }
        break;
    case BTEVENT_LINK_DISCONNECT:
            Report(("L2CAP_TP APP: ACL connection Disconnected."));
        RemDev = 0;
        break;
    }
}

static void BtlL2capAppCallBack(const BtlL2capEvent *event)
{
	BtlL2capChannelConfig channelConfig;
	BtStatus status;
  	char addr[BDADDR_NTOA_SIZE];
	double dTotLen;
	double dTotTime;
	
	if (event->l2capContext == 0)
	{
		Report(("L2CAP_TP APP: NULL event context"));
		return;
	}
	
	if (event->l2capContext != activeL2capContext)
	{
		Report(("L2CAP_TP APP: Context mismatch. Event context doesn't match active"));
		return;
	}

  	/* Create a displayable BD address for some events. */
	addr[0] = '\0';
	if (event->l2capEvent->aclLink != NULL)
  	{
    	/* Relevant event? --> create a displayable address. */
		if ((event->l2capEvent->event == L2EVENT_CONNECT_IND) ||
		    (event->l2capEvent->event == L2EVENT_CONNECTED)   ||
		    (event->l2capEvent->event == L2EVENT_DISCON_IND)  ||
		    (event->l2capEvent->event == L2EVENT_DISCONNECTED)||
			(event->l2capEvent->event == L2EVENT_CONNECTION_FAILURE))
    	{  
		  	bdaddr_ntoa(&(event->l2capEvent->aclLink->bdAddr), addr);
    	}
  	}

  	switch (event->l2capEvent->event)
  	{
    case L2EVENT_CONNECT_IND:
      	channelId = event->channelId;
      
        /* Auto-accept connection */
#if L2CAP_FLOW_CONTROL == XA_ENABLED
        connSettings.reasmBuffer = ServerRxBuffer;
#endif
        status = BTL_L2CAP_ConnectResponse(activeL2capContext, event->channelId, event->l2capEvent->status, &connSettings);
        BTL_LOG_ERROR(("L2CAP_TP APP: Channel %d: RX: CONNECT_IND from \"%s\". Auto accepted, result = %s.", event->channelId, addr, pBT_Status(status)));
      	break;
      
    case L2EVENT_CONNECTED:
		status = BTL_L2CAP_GetChannelConfig(activeL2capContext, event->channelId, &channelConfig);
      	BTL_LOG_ERROR(("L2CAP_TP APP: Channel %d: RX: CONNECTED with \"%s\", tx MTU = %d", event->channelId, addr, channelConfig.txMtu));
		connectionInd = TRUE;
      	break;

    case L2EVENT_DISCON_IND:
      	BTL_LOG_ERROR(("L2CAP_TP APP: Channel %d: RX: DISCON_IND from \"%s\".", event->channelId, addr));
      	break;

    case L2EVENT_DISCONNECTED:
      	BTL_LOG_ERROR(("L2CAP_TP APP: Channel %d: RX: DISCONNECTED from \"%s\".", event->channelId, addr));
		channelId = BTL_L2CAP_ILLEGAL_CHANNEL_ID;
		connectionInd = FALSE;
		if (L2CAP_ROLE_CLIENT == role)
		{
			role = L2CAP_ROLE_NONE;
		}
      	break;

    case L2EVENT_DATA_IND:
		if (totalRxDataLen == 0)
		{
			BTHAL_OS_GetSystemTime(&startRxTime);
			lastRxTime = startRxTime;
			totalRxTime = 0;
			totalRxDataLen = 0;
		}

		totalRxDataLen += event->l2capEvent->dataLen;
		lastRxDataLen += event->l2capEvent->dataLen;

		if (lastRxDataLen >= SIZE_OF_1MB)
		{
			BTHAL_OS_GetSystemTime(&totalRxTime);
			dTotLen = (totalRxDataLen * 8) / 1024.0;
			dTotTime = (totalRxTime-startRxTime) / 1000.0;

			BTL_LOG_ERROR(("Total kbits: %.2f, Total time (s): %.2f", dTotLen, dTotTime)); 
			BTL_LOG_ERROR(("L2CAP Average Rx rate is %.2f kbps", dTotLen/dTotTime));
			BTL_LOG_ERROR(("L2CAP last %lu ms Rx rate is %.2f kbps", totalRxTime-lastRxTime,
				(((8.00 * lastRxDataLen / 1024) / ((totalRxTime-lastRxTime) / 1000.0)))));
			lastRxTime = totalRxTime;
			lastRxDataLen = 0;
		}
      	break;
		
    case L2EVENT_PACKET_HANDLED:		
		if (TRUE == sendInd)
		{
    		if (ClientPkt.flags & BTP_FLAG_INUSE) 
			{
				Report(("L2CAP_TP Client: Cannot Send Packet because it is In Use."));
				sendInd = FALSE;
				break;
			}
		totalTxDataLen += txDataLen;
		lastTxDataLen += txDataLen;

		if (lastTxDataLen >= SIZE_OF_1MB)
		{
			BTHAL_OS_GetSystemTime(&totalTxTime);
			dTotLen = (totalTxDataLen * 8) / 1024.0;
			dTotTime = (totalTxTime-startTxTime) / 1000.0;

			BTL_LOG_ERROR(("Total kbits: %.2f, Total time (s): %.2f", dTotLen, dTotTime)); 
			BTL_LOG_ERROR(("L2CAP Average Tx rate is %.2f kbps", dTotLen/dTotTime));
			BTL_LOG_ERROR(("L2CAP last %lu ms Tx rate is %.2f kbps", totalTxTime-lastTxTime,
				(((8.00 * lastTxDataLen / 1024) / ((totalTxTime-lastTxTime) / 1000.0)))));
			lastTxTime = totalTxTime;
			lastTxDataLen = 0;
		}

			status = BTL_L2CAP_SendData(activeL2capContext, channelId, TxData, txDataLen, (BtPacket **)&ClientPkt);
			if (status != BT_STATUS_PENDING)
			{
				Report(("L2CAP_TP: Error after BTL_L2CAP_SendData(). status is %d", status));
				sendInd = FALSE;
				break;
			}
		}
      	break;
	
    case L2EVENT_COMPLETE:
      	Report(("L2CAP_TP APP: Channel -: RX: COMPLETE, status = %s", pBT_Status(event->l2capEvent->status)));
      	break;

    case L2EVENT_CONFIGURING:
      	Report(("L2CAP_TP APP: Channel %d: RX: CONFIGURING, status = %s", event->channelId, pBT_Status(event->l2capEvent->status)));
      	break;


    case L2CAP_EVENT_DISABLED:
		if (psm != 0)
		{
			psm = 0;
			role = L2CAP_ROLE_NONE;
			serverRegisteredInd = FALSE;
		}
      	Report(("L2CAP_TP APP: RX: EVENT_DISABLED, PSM is now deregistered"));
      	break;

	case L2EVENT_CONNECTION_FAILURE:
		sendInd = FALSE;
      	BTL_LOG_ERROR(("L2CAP_TP APP: Failed to connect to %s.", addr));
	  	BTL_LOG_ERROR(("L2CAP_TP APP: Channel %d: RX: DISCONNECTED from \"%s\".", event->channelId, addr));
      break;			

    default:
      	Report(("L2CAP_TP APP: Channel %d: RX: unknown L2CAP event: %d.", event->channelId, event->l2capEvent->event));
      	break;
  	}
}

/*
 * Initialize all global (test) data.
 */
static BOOL InitL2capData(void)
{
	U16             i;
	BtStatus status;
  	BtSecurityLevel securityLevel = BSL_NO_SECURITY;

    status = BTL_L2CAP_Create(0, BtlL2capAppCallBack, &securityLevel, PROT_L2CAP, &activeL2capContext);
	Report(("L2CAP_TP APP: status after BTL_L2CAP_Create() is %d", status));
	if (BT_STATUS_SUCCESS != status)
	{
		return FALSE;
	}

	txDataLen = L2CAP_MAXIMUM_MTU;
	
	OS_MemSet((U8 *)&ClientPkt, 0, sizeof(BtPacket));

	channelId = BTL_L2CAP_ILLEGAL_CHANNEL_ID;
	
  	/* Fill Tx-data package with a defined pattern.*/
  	for (i = 0; i < txDataLen; i++)
  	{
    	TxData[i] = (U8)i;
  	}

  	/* Init channelConfig for initiating (client) connections, */
  	channelSettings.localMtu = L2CAP_MAXIMUM_MTU;
  	channelSettings.minRemoteMtu = 256;

	role = L2CAP_ROLE_NONE;

	return TRUE;
}


#else /* BTL_CONFIG_BTL_L2CAP == BTL_CONFIG_ENABLED */

void L2CAPA_THROUGHPUT_ProcessUserAction(U8 *msg)
{
	msg=msg;
	Report(("L2CAP_TP_APP is disabled via BTL_CONFIG."));
}

#endif /* BTL_CONFIG_BTL_L2CAP == BTL_CONFIG_ENABLED */







