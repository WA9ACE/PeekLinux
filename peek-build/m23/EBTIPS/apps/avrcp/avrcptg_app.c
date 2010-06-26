/*******************************************************************************\
*                                                                       		*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      avrcptg_app.c
*
*   DESCRIPTION:	This file contains the implementation of the AVRCP Target role application primitives (user actions).
*
*   AUTHOR:         Alon Cheifetz.
*
\*******************************************************************************/

#include "btl_config.h"
#include "osapi.h"
#if BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED


#if BT_STACK == XA_ENABLED
#include "sec.h"		
#endif

#include "avrcp.h"		
		
#include "../app_main.h"


/* BTL includes */
#include "btl_avrcptg.h"
#include "lineparser.h"	

static BtlAvrcptgContext	*avrcptgaContext = 0;
static BtSecurityLevel securityLevel = BSL_NO_SECURITY;
static  U16 Category = (AVRCP_FEATURES_CATEGORY_1 | AVRCP_FEATURES_CATEGORY_2);
static BD_ADDR bdAddr;

extern int avrcpChannelId2A2dpStreamId[BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT];

/****************************************************************************
 *
 * External Function prototypes
 *
 ***************************************************************************/
extern void APP_A2DP_SendAvrcpOperation(U32 channelId, AvrcpPanelOperation avrcpPanelOp);
extern void APP_A2DP_UpdateAvrcpConnection(U32 channelId, BD_ADDR *bdAdd, BOOL isChannelConnected);


/****************************************************************************
 *
 * Internal Function prototypes
 *
 ***************************************************************************/
static void AvrcptgaCallBack(const BtlAvrcptgEvent *event);
void APP_AVRCPTG_UpdateAvrcpConnection(U32 streamId, BD_ADDR *bdAdd, BOOL isChannelConnected);

void AVRCPTGA_Init(void);
void AVRCPTGA_Deinit(void);

void avrcptgaCreate(void);
void avrcptgaDestroy(void);
void avrcptgaEnable(void);
void avrcptgaDisable(void);
void avrcptgaSetSecurityLevel(void);
void avrcptgaGetSecurityLevel(void);
void avrcptgaConnect(void);
void avrcptgaDisconnect(void);
void avrcptgaGetConnectedDevice(void);

static U8 *pRcpCtype(U8 msgCtype);
static U8 *pRcpResponse(U8 msgResponse);
static U8 *pRcpOpCode(U8 msgOpCode);
static U8 *pRcpCmd(AvrcpPanelOperation msgCmd);

/****************************************************************************
 *
 * RAM data
 *
 ****************************************************************************/

static AvrcpCmdFrame        cmdFrame;
static AvrcpRspFrame        rspFrame;


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*---------------------------------------------------------------------------
 *            AVRCPTGA_Init
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Init the AVRCPTG module.
 *
 * Return:    TRUE if initialized, FALSE otherwise
 *
 */
void AVRCPTGA_Init(void)
{
	BtStatus status=0;
	
	status = BTL_AVRCPTG_Create(0, AvrcptgaCallBack, 0, &avrcptgaContext);
	
    Report(("BTL_AVRCPTG_Create() returned %s.", pBT_Status(status)));

	status = BTL_AVRCPTG_Enable(avrcptgaContext, (const BtlUtf8 *)"Neptune_Avrcptg", Category);
	
    Report(("BTL_AVRCPTG_Enable() returned %s.", pBT_Status(status)));
	
}

/*---------------------------------------------------------------------------
 *            AVRCPTGA_Deinit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinit the AVRCPTG module.
 *
 * Return:    TRUE if initialized, FALSE otherwise
 *
 */
void AVRCPTGA_Deinit(void)
{
	BtStatus status;
	
	status = BTL_AVRCPTG_Disable(avrcptgaContext);
	
    Report(("BTL_AVRCPTG_Disable() returned %s.", pBT_Status(status)));
	
	status = BTL_AVRCPTG_Destroy(&avrcptgaContext);
	
    Report(("BTL_AVRCPTG_Destroy() returned %s.", pBT_Status(status)));
	
}


/*---------------------------------------------------------------------------
 *            avrcptgaCreate
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Allocates a unique AVRCPTG context.
 *
 * Return:    TRUE if initialized, FALSE otherwise
 *
 */
void avrcptgaCreate(void)
{
	BtStatus status;

	status = BTL_AVRCPTG_Create(0, AvrcptgaCallBack, &securityLevel, &avrcptgaContext);
	
    Report(("BTL_AVRCPTG_Create() returned %s.", pBT_Status(status)));
	
}

/*---------------------------------------------------------------------------
 *            avrcptgaDestroy
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Releases a AVRCPTG context.
 *
 * Return:    TRUE if initialized, FALSE otherwise
 *
 */
void avrcptgaDestroy(void)
{
	BtStatus status;

	status = BTL_AVRCPTG_Destroy(&avrcptgaContext);
	
    Report(("avrcptgaDestroy() returned %s.", pBT_Status(status)));
	
}

/*---------------------------------------------------------------------------
 *            avrcptgaEnable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enable AVRCPTG and register AVRCP TG SDP record.
 *
 * Return:    TRUE if initialized, FALSE otherwise
 *
 */
void avrcptgaEnable(void)
{
	BtStatus status;
		
	status = BTL_AVRCPTG_Enable(avrcptgaContext, (const BtlUtf8 *)"Avrcptg_Neptune", Category);
		
    Report(("avrcptgaEnable() returned %s.", pBT_Status(status)));
	
}

/*---------------------------------------------------------------------------
 *            avrcptgaDisable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disable AVRCPTG.
 *
 * Return:    TRUE if initialized, FALSE otherwise
 *
 */
void avrcptgaDisable(void)
{
	BtStatus status;
		
	status = BTL_AVRCPTG_Disable(avrcptgaContext);
		
    Report(("AVRCPTGA_Disable() returned %s.", pBT_Status(status)));
	
}


/*---------------------------------------------------------------------------
 *            AVRCPTGA_SetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets security level for the given AVRCPTG context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
void avrcptgaSetSecurityLevel(void)
{
	BtStatus status;
	BOOL retVal = FALSE;
	U8 newSecurityLevel = securityLevel;
	U8 Level;

	if (LINE_PARSER_GetNextU8(&Level, FALSE) != LINE_PARSER_STATUS_SUCCESS)
	{
	    Report(("AVRCPTGA_SetSecurityLevel() error with Level %d.", Level));
		return ;
	}

	if (newSecurityLevel & (Level))
	{
		newSecurityLevel &= (~(Level));
		retVal = FALSE;
	}
	else
	{
		newSecurityLevel |= Level;
		retVal = TRUE;
	}
	
	status = BTL_AVRCPTG_SetSecurityLevel(avrcptgaContext, &Level);
	
    Report(("AVRCPTGA_SetSecurityLevel() returned %s.", pBT_Status(status)));

	if (status == BT_STATUS_SUCCESS)
		securityLevel = newSecurityLevel;

}

/*---------------------------------------------------------------------------
 *            avrcptgaGetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Gets security level for the given AVRCPTG context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
void avrcptgaGetSecurityLevel(void)
{
	BtStatus status;
	BtSecurityLevel level;
	
	status = BTL_AVRCPTG_GetSecurityLevel(avrcptgaContext, &level);
	
    Report(("BTL_AVRCPTG_GetSecurityLevel() returned %s.",pBT_Status(status)));

	if(level & BSL_AUTHENTICATION_IN)
	{
			Report(("Security level: BSL_AUTHENTICATION_IN"));
	}
	if(level & BSL_AUTHORIZATION_IN)
	{
			Report(("Security level: BSL_AUTHORIZATION_IN"));
	}
	if(level & BSL_ENCRYPTION_IN)
	{
			Report(("Security level: BSL_ENCRYPTION_IN"));
	}
	if(level & BSL_AUTHENTICATION_OUT)
	{
		Report(("Security level: BSL_AUTHENTICATION_OUT"));
	}
	if(level & BSL_AUTHORIZATION_OUT)
	{
		Report(("Security level: BSL_AUTHORIZATION_OUT"));
	}
	if(level & BSL_ENCRYPTION_OUT)
	{
		Report(("Security level: BSL_ENCRYPTION_OUT"));
	}
	if(level & BSL_BROADCAST_IN)
	{
		Report(("Security level: BSL_BROADCAST_IN"));
	}
}

/*---------------------------------------------------------------------------
 *            avrcptgaConnect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiates a connection to a remote AVRCP CT.
 *
 */
void avrcptgaConnect(void)
{

	U8 					BdArray[17];
	BtStatus    		status;
	BD_ADDR 			BdAddr;
	LINE_PARSER_STATUS  ParserStatus;
	BtlAvrcptgChannelId channelId;
	ParserStatus = LINE_PARSER_GetNextStr(BdArray, 17);


	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("Avrcptga BD addr. Can't connect."));
		return;
	}
	
	BdAddr = bdaddr_aton((const char *)(BdArray));
	
	status = BTL_AVRCPTG_Connect(avrcptgaContext, &BdAddr,&channelId);

	Report(("BTL_AVRCPTG_Connect returned %s", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            avrcptgDisconnect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Terminates a connection with a remote AVRCP device.
 *
 */
void avrcptgaDisconnect(void)
{
	BtStatus status;
	U32 channelId;

	if (LINE_PARSER_GetNextU32(&channelId, FALSE) == LINE_PARSER_STATUS_SUCCESS)
	{
		status = BTL_AVRCPTG_Disconnect(avrcptgaContext, channelId);
	}
	else
	{
	    Report(("avrcptgDisconnect() error with channelId %d.", channelId));
		return ;
	}
		
    Report(("avrcptgDisconnect() returned %s.", pBT_Status(status)));
}

/*---------------------------------------------------------------------------
 *            avrcptgGetConnectedDevice
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function returns the connected device.
 *
 */
void avrcptgaGetConnectedDevice(void)
{
	BtStatus status;
	U32 channelId;
	LINE_PARSER_STATUS ParserStatus;

	BD_ADDR 	bdAddr;
	char 		addr[BDADDR_NTOA_SIZE];
		
	if (LINE_PARSER_GetNextU32(&channelId, FALSE) == LINE_PARSER_STATUS_SUCCESS)
	{
		status = BTL_AVRCPTG_GetConnectedDevice(avrcptgaContext, channelId, &bdAddr);

		switch(status)
		{
		case BT_STATUS_NO_CONNECTION: 
			UI_DISPLAY(("avrcptgaGetConnectedDevice: no connection on channel %x.", channelId));
			break;
		case BT_STATUS_SUCCESS: 
			UI_DISPLAY(("avrcptgaGetConnectedDevice: Connected to %s.", bdaddr_ntoa(&bdAddr, addr)));
			break;
	    default:
			Report(("avrcptgaGetConnectedDevice - switch failed at default "));
	        break;
	    }
	}
	else
	{
	    Report(("avrcptgGetConnectedDevice() error with channelId %d.", channelId));
		return ;
	}
		
    Report(("avrcptgGetConnectedDevice() returned %s.", pBT_Status(status)));
}


void APP_AVRCPTG_UpdateAvrcpConnection(U32 streamId, BD_ADDR *bdAdd, BOOL isChannelConnected)
{
	U32 channelId;
	BD_ADDR currBdAdd;
	BtStatus status;

	for(channelId = 0; channelId < BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		if((isChannelConnected == FALSE) && (avrcpChannelId2A2dpStreamId[channelId] == streamId))
		{
			avrcpChannelId2A2dpStreamId[channelId] = (-1); 
			return;
		}
		
		status = BTL_AVRCPTG_GetConnectedDevice(avrcptgaContext, channelId, &currBdAdd);

		if(status == BT_STATUS_SUCCESS)
		{
			/* connected device was found, compare BD address */
			if(OS_MemCmp(&currBdAdd, sizeof(BD_ADDR), bdAdd, sizeof(BD_ADDR)))
			{
				avrcpChannelId2A2dpStreamId[channelId] = streamId;  
				return;
			}
		}   
	}
}


/*---------------------------------------------------------------------------
 *            AvrcptgaCallBack
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes Avrcptg profile events.
 *
 * Return:    void
 *
 */
static void AvrcptgaCallBack(const BtlAvrcptgEvent *event)
{
	AvrcpChannel *Channel = event->avrcptgEvent->channel;
	AvrcpCallbackParms *Parms = event->avrcptgEvent;
	BtlAvrcptgChannelId	channelId = event->channelId;
	static U16 opTotLen;

    switch (Parms->event) {
    case AVRCP_EVENT_CONNECT_IND:
		BTL_AVRCPTG_ConnectResponse(avrcptgaContext, channelId, TRUE);
		UI_DISPLAY(("AVRCPTG UI: Incoming Connection on channel %x was accepted", channelId));
        break;

    case AVRCP_EVENT_CONNECT:
        /* Transport connected */
		UI_DISPLAY(("AVRCPTG UI: Connection established on channel %x", channelId));
		
		/*update A2DP app AVRCP channel was connected */
		APP_A2DP_UpdateAvrcpConnection(channelId, &(Parms->p.remDev->bdAddr), TRUE);
        break;

    case AVRCP_EVENT_DISCONNECT:
        /* Transport disconnected */
        UI_DISPLAY(("AVRCPTG UI: Disconnected channel %x", channelId));

		/*update A2DP app AVRCP channel was disconnected */
		APP_A2DP_UpdateAvrcpConnection(channelId, &(Parms->p.remDev->bdAddr), FALSE);
        break;

    case AVRCP_EVENT_COMMAND:
        /* Command, process and display */
        UI_DISPLAY(("AVRCPTG UI: Rx Ctype: %s, OpCode: %s, len: %d on channel %x", pRcpCtype(Parms->p.cmdFrame->ctype),pRcpOpCode(Parms->p.cmdFrame->opcode), Parms->p.cmdFrame->operandLen, channelId));

        opTotLen = Parms->p.cmdFrame->operandLen;
        if (opTotLen) 
        {
	        UI_DISPLAY(("AVRCPTG UI: Operands, tot: %d on channel %x", opTotLen ,channelId));
        }

        /* Set up response */
        rspFrame.subunitType = Parms->p.cmdFrame->subunitType;
        rspFrame.subunitId = Parms->p.cmdFrame->subunitId;
        rspFrame.opcode = Parms->p.cmdFrame->opcode;
        break;

    case AVRCP_EVENT_RESPONSE:
        /* Response */
        UI_DISPLAY(("AVRCPTG UI: Rx Response: %s, OpCode: %s, len: %d on channel %x", pRcpResponse(Parms->p.rspFrame->response), pRcpOpCode(Parms->p.rspFrame->opcode), Parms->p.rspFrame->operandLen , channelId));

        opTotLen = Parms->p.rspFrame->operandLen;
        if (opTotLen) 
        {
	        UI_DISPLAY(("AVRCPTG UI: Operands, tot: %d on channel %x", opTotLen ,channelId));
        }
        break;

    case AVRCP_EVENT_TX_DONE:
        /* Frame sent */
        UI_DISPLAY(("AVRCPTG UI: Frame sent, OpCode: %s on channel %x", pRcpOpCode(Parms->p.cmdFrame->opcode), channelId));
        break;

    case AVRCP_EVENT_OPERANDS:
        /* Operands (pretend it's cmdFrame since both have the same layout) */
        opTotLen += opTotLen = Parms->p.cmdFrame->operandLen;
        UI_DISPLAY(("AVRCPTG UI: More Operands, tot: %d on channel %x", opTotLen, channelId));
        break;

#if AVRCP_PANEL_SUBUNIT == XA_ENABLED 
    case AVRCP_EVENT_PANEL_PRESS:
    	UI_DISPLAY(("AVRCPTG UI: Pressing %s on channel %x\n", pRcpCmd(Parms->p.panelInd.operation), channelId));
		APP_A2DP_SendAvrcpOperation(channelId, Parms->p.panelInd.operation); /* sent the event to A2DP application */

    	break;
    	
    case AVRCP_EVENT_PANEL_HOLD:
    	UI_DISPLAY(("AVRCPTG UI: Holding %s on channel %x\n", pRcpCmd(Parms->p.panelInd.operation), channelId));
    	break;

    case AVRCP_EVENT_PANEL_RELEASE:
    	UI_DISPLAY(("AVRCPTG UI: Releasing %s on channel %x\n", pRcpCmd(Parms->p.panelInd.operation), channelId));
        break;
#endif /* AVRCP_PANEL_SUBUNIT == XA_ENABLED */

    case AVRCP_BSC_EVENT_RECOMMENDED_PAUSE:
        /* Bluetooth System Coordinator module recommended to pause A2DP stream */
        UI_DISPLAY(("AVRCPTG UI: Incoming call - BSC recommends to pause Media Player"));
        break;
        
    case AVRCP_BSC_EVENT_RECOMMENDED_RESUME_PLAYING:
        /* Bluetooth System Coordinator module recommended to resume playing A2DP stream */
        UI_DISPLAY(("AVRCPTG UI: Incoming call finished - BSC recommends to resume playing Media Player"));
        break;
		
    default:
		Report(("AVRCPTG callback - switch failed at default "));
        break;
    }
}

static U8 *pRcpCtype(U8 msgCtype)
{
    switch (msgCtype) {
    case AVRCP_CTYPE_CONTROL:
        return (U8 *)"Control";
    

    case AVRCP_CTYPE_STATUS:
        return (U8 *)"Status";
    

    case AVRCP_CTYPE_SPECIFIC_INQUIRY:
        return (U8 *)"Specific Inquiry";
    

    case AVRCP_CTYPE_NOTIFY:
        return (U8 *)"Notify";
    

    case AVRCP_CTYPE_GENERAL_INQUIRY:
        return (U8 *)"General Inquiry";
    

    default:
        return (U8 *)"Unknown";
    
    }
}

static U8 *pRcpResponse(U8 msgResp)
{
    switch (msgResp) {
    case AVRCP_RESPONSE_NOT_IMPLEMENTED:
        return (U8 *)"Not Implemented";


    case AVRCP_RESPONSE_ACCEPTED:
        return (U8 *)"Accepted";


    case AVRCP_RESPONSE_REJECTED:
        return (U8 *)"Rejected";


    case AVRCP_RESPONSE_IN_TRANSITION:
        return (U8 *)"In Transition";


    case AVRCP_RESPONSE_IMPLEMENTED_STABLE:
        return (U8 *)"Implemented Stable";


    case AVRCP_RESPONSE_CHANGED:
        return (U8 *)"Changed";


    case AVRCP_RESPONSE_INTERIM:
        return (U8 *)"Interim";


    default:
        return (U8 *)"Unknown";

    }
}

static U8 *pRcpOpCode(U8 msgOpCode)
{
    switch (msgOpCode) {
    case AVRCP_OPCODE_VENDOR_DEPENDENT:
        return (U8 *)"Vendor Dependent";
   

    case AVRCP_OPCODE_UNIT_INFO:
        return (U8 *)"Unit Info";
   

    case AVRCP_OPCODE_SUBUNIT_INFO:
        return (U8 *)"Subunit Info";
   

    case AVRCP_OPCODE_PASS_THROUGH:
        return (U8 *)"Pass Through";
   

    default:
        return (U8 *)"Unknown";
   
    }
}


static U8 *pRcpCmd(AvrcpPanelOperation msgCmd)
{
    switch (msgCmd) {

    case AVRCP_POP_SELECT:
        return (U8 *)"AVRCP_SELECT";


    case AVRCP_POP_UP:
        return (U8 *)"AVRCP_UP";


    case AVRCP_POP_DOWN:
        return (U8 *)"AVRCP_DOWN";


    case AVRCP_POP_LEFT:
        return (U8 *)"AVRCP_LEFT";


    case AVRCP_POP_RIGHT:
        return (U8 *)"AVRCP_RIGHT";


    case AVRCP_POP_RIGHT_UP:
        return (U8 *)"AVRCP_RIGHT_UP";


    case AVRCP_POP_RIGHT_DOWN:
        return (U8 *)"AVRCP_RIGHT_DOWN";


    case AVRCP_POP_LEFT_UP:
        return (U8 *)"AVRCP_LEFT_UP";


    case AVRCP_POP_LEFT_DOWN:
        return (U8 *)"AVRCP_LEFT_DOWN";


    case AVRCP_POP_ROOT_MENU:
        return (U8 *)"AVRCP_ROOT_MENU";


    case AVRCP_POP_SETUP_MENU:
        return (U8 *)"AVRCP_SETUP_MENU";


    case AVRCP_POP_CONTENTS_MENU:
        return (U8 *)"AVRCP_CONTENTS_MENU";


    case AVRCP_POP_FAVORITE_MENU:
        return (U8 *)"AVRCP_FAVORITE_MENU";


    case AVRCP_POP_EXIT:
        return (U8 *)"AVRCP_EXIT";


    case AVRCP_POP_0:
        return (U8 *)"AVRCP_0";


    case AVRCP_POP_1:
        return (U8 *)"AVRCP_1";


    case AVRCP_POP_2:
        return (U8 *)"AVRCP_2";


    case AVRCP_POP_3:
        return (U8 *)"AVRCP_3";


    case AVRCP_POP_4:
        return (U8 *)"AVRCP_4";


    case AVRCP_POP_5:
        return (U8 *)"AVRCP_5";


    case AVRCP_POP_6:
        return (U8 *)"AVRCP_6";


    case AVRCP_POP_7:
        return (U8 *)"AVRCP_7";


    case AVRCP_POP_8:
        return (U8 *)"AVRCP_8";


    case AVRCP_POP_9:
        return (U8 *)"AVRCP_9";


    case AVRCP_POP_DOT:
        return (U8 *)"AVRCP_DOT";


    case AVRCP_POP_ENTER:
        return (U8 *)"AVRCP_ENTER";


    case AVRCP_POP_CLEAR:
        return (U8 *)"AVRCP_CLEAR";


    case AVRCP_POP_CHANNEL_UP:
        return (U8 *)"AVRCP_CHANNEL_UP";


    case AVRCP_POP_CHANNEL_DOWN:
        return (U8 *)"AVRCP_CHANNEL_DOWN";


    case AVRCP_POP_PREVIOUS_CHANNEL:
        return (U8 *)"AVRCP_PREVIOUS_CHANNEL";


    case AVRCP_POP_SOUND_SELECT:
        return (U8 *)"AVRCP_SOUND_SELECT";


    case AVRCP_POP_INPUT_SELECT:
        return (U8 *)"AVRCP_INPUT_SELECT";


    case AVRCP_POP_DISPLAY_INFO:
        return (U8 *)"AVRCP_DISPLAY_INFO";


    case AVRCP_POP_HELP:
        return (U8 *)"AVRCP_HELP";


    case AVRCP_POP_PAGE_UP:
        return (U8 *)"AVRCP_PAGE_UP";


    case AVRCP_POP_PAGE_DOWN:
        return (U8 *)"AVRCP_PAGE_DOWN";


    case AVRCP_POP_POWER:
        return (U8 *)"AVRCP_POWER";


    case AVRCP_POP_VOLUME_UP:
        return (U8 *)"AVRCP_VOLUME_UP";


    case AVRCP_POP_VOLUME_DOWN:
        return (U8 *)"AVRCP_VOLUME_DOWN";


    case AVRCP_POP_MUTE:
        return (U8 *)"AVRCP_MUTE";


    case AVRCP_POP_PLAY:
        return (U8 *)"AVRCP_PLAY";


    case AVRCP_POP_STOP:
        return (U8 *)"AVRCP_STOP";


    case AVRCP_POP_PAUSE:
        return (U8 *)"AVRCP_PAUSE";
     

    case AVRCP_POP_RECORD:
        return (U8 *)"AVRCP_RECORD";
        

    case AVRCP_POP_REWIND:
        return (U8 *)"AVRCP_REWIND";
        

    case AVRCP_POP_FAST_FORWARD:
        return (U8 *)"AVRCP_FAST_FORWARD";
        

    case AVRCP_POP_EJECT:
        return (U8 *)"AVRCP_EJECT";
        

    case AVRCP_POP_FORWARD:
        return (U8 *)"AVRCP_FORWARD";
        

    case AVRCP_POP_BACKWARD:
        return (U8 *)"AVRCP_BACKWARD";
        

    case AVRCP_POP_ANGLE:
        return (U8 *)"AVRCP_ANGLE";
        

    case AVRCP_POP_SUBPICTURE:
        return (U8 *)"AVRCP_SUBPICTURE";
        

    case AVRCP_POP_F1:
        return (U8 *)"AVRCP_F1";
        

    case AVRCP_POP_F2:
        return (U8 *)"AVRCP_F2";
        

    case AVRCP_POP_F3:
        return (U8 *)"AVRCP_F3";
        

    case AVRCP_POP_F4:
        return (U8 *)"AVRCP_F4";
        

    case AVRCP_POP_F5:
        return (U8 *)"AVRCP_F5";
        

    case AVRCP_POP_VENDOR_UNIQUE:
        return (U8 *)"AVRCP_VENDOR_UNIQUE";
        

    case AVRCP_POP_NEXT_GROUP:
        return (U8 *)"AVRCP_NEXT_GROUP";
        

    case AVRCP_POP_PREV_GROUP:
        return (U8 *)"AVRCP_PREV_GROUP";
        

    case AVRCP_POP_RESERVED:
        return (U8 *)"AVRCP_RESERVED";
        


    default:
        return (U8 *)"Unknown";
        
    }
}

#else /* BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED */

void AVRCPTGA_Init(void)
{

    Report(("AVRCPTG UI: AVRCPTGA_Init -BTL_CONFIG_AVRCPTG is disabled "));

}

void AVRCPTGA_Deinit(void)
{

    Report(("AVRCPTG UI: AVRCPTGA_Deinit -BTL_CONFIG_AVRCPTG is disabled "));

}


#endif /* BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED */



