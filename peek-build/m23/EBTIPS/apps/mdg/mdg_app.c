/*******************************************************************************\
*                                                                           	*
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
*   FILE NAME:      mdg_app.c
*
*   DESCRIPTION:    Implementation of Modem Data Gateway sample application for
*                       DUN.
*
*   AUTHOR:         V. Abram
*
\*******************************************************************************/


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "btl_config.h"
#include "osapi.h"
#if BTL_CONFIG_MDG == BTL_CONFIG_ENABLED


#include "bttypes.h"
#include "sec.h"
#include "spp.h"
#include "osapi.h"
#include "debug.h"
#include "lineparser.h"
#include "btl_mdg.h"
#include "../app_main.h"


/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

/* MDG Application states */
#define MDGA_SERVER_STATE_NONE			    (0)
#define MDGA_SERVER_STATE_ENABLED		    (1)
#define MDGA_SERVER_STATE_DISABLING		    (2)
#define MDGA_SERVER_STATE_DT_CONNECTED	    (3)
#define MDGA_SERVER_STATE_MODEM_CONNECTED	(4)
#define MDGA_SERVER_STATE_CONNECTED		    (5)

/* Security level of the MDG Application */
#define MDGA_SECURITY_LEVEL (BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT)

/* Max number of Rx Sync packets - this value will be given to peer serial port
 * as amount of credits for RFCOMM credit based flow control */
#define MDGA_MAX_NUM_OF_RX_SYNC_PACKETS     (5)


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Structure to keep track of server information */
typedef struct
{
	U8                      state;
	const BtSecurityLevel   securityLevel;
	BtlMdgServerContext     *serverContext;
    BtlMdgServerSettings    serverSettings;
	SppLineStatus           lineStatus;
} MdgaServer;


/********************************************************************************
 *
 * Function prototypes
 *
 *******************************************************************************/
void MDGA_Init(void);
void MDGA_Deinit(void);
void MDGA_Enable(U8 serverIndex);
void MDGA_Disable(U8 serverIndex);
void MDGA_GetSecurityLevel(U8 serverIndex);
void MDGA_SetSecurityLevel(U8 serverIndex, BtSecurityLevel *securityLevel);
void MDGA_GetComSettings(U8 serverIndex);
void MDGA_SetComSettings(U8 serverIndex, SppComSettings *comSettings);
void MDGA_GetConnectedDevice(U8 serverIndex);
void MDGA_TerminateDataCall(void);

static void MdgaBtlMdgCallback(const BtlMdgCallbackParms *parms);
static const char *MdgaEventName(BtlMdgServerEvent mdgEvent);
static const char *MdgaStateName(int state);


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

static MdgaServer mdgaServers[] =
{
    {
     MDGA_SERVER_STATE_NONE, MDGA_SECURITY_LEVEL, NULL,
     {BTL_SPP_SERVICE_TYPE_DUN,
      "BTIPS DUN", 
      {SPP_SERVER_PORT,
       BTL_SPP_SERVICE_TYPE_DUN,
       SPP_PORT_DATA_PATH_TX_SYNC | SPP_PORT_DATA_PATH_RX_ASYNC,
       MDGA_MAX_NUM_OF_RX_SYNC_PACKETS,
       {SPP_COM_SETTING_BAUDRATE_DEFAULT,
	   SPP_COM_SETTING_DATA_FORMAT_DEFAULT,
        SPP_COM_SETTING_FLOW_CONTROL_DEFAULT,
	   SPP_COM_SETTING_XON_CHAR_DEFAULT,
	   SPP_COM_SETTING_XOFF_CHAR_DEFAULT,
        0
      }
     },
     },
     0
    }
};

#define MDGA_NUM_SERVERS (sizeof(mdgaServers) / sizeof(MdgaServer))


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

 /*-------------------------------------------------------------------------------
 * MDGA_Init()
 *
 *		Initialize application module.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	None.
 *
 * Returns:
 *		None.
 */
void MDGA_Init()
{
    BtStatus status;
   	U32 idx;
    MdgaServer *server = &mdgaServers[0];

    Report(("MDGA_Init"));

    /* Create and enable all MDG servers */
    for (idx=0; idx<MDGA_NUM_SERVERS; idx++, server++)
    {
        /* Create MDG server */
        status = BTL_MDG_Create(0,
                                MdgaBtlMdgCallback,
                                &server->securityLevel,
                                &server->serverContext);
        if (BT_STATUS_SUCCESS != status)
        {
	        Report(("MDGA: BTL_MDG_Create() failed for server %s, status %s",
                    server->serverSettings.serviceName,
                    pBT_Status(status)));
        }
        else
        {
            /* Enable MDG server */
            status = BTL_MDG_Enable(server->serverContext,
                                    &server->serverSettings);

            if (BT_STATUS_SUCCESS != status)
            {
	            Report(("MDGA: BTL_MDG_Enable() failed for server %s, status %s",
                        server->serverSettings.serviceName,
                        pBT_Status(status)));
            }
            else
            {
                server->state = MDGA_SERVER_STATE_ENABLED;
            }
        }
    }
}

/*-------------------------------------------------------------------------------
 * MDGA_Deinit()
 *
 *		Deinitialize application module.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	None.
 *
 * Returns:
 *		None.
 */
void MDGA_Deinit()
{
    BtStatus status;
   	U32 idx;
    MdgaServer *server = &mdgaServers[0];

    Report(("MDGA_Deinit"));

    /* Disable and destroy all MDG servers */
    for (idx=0; idx<MDGA_NUM_SERVERS; idx++, server++)
    {
        /* Check whether the server was enabled and not in DISABLING state */
        if ((server->state >= MDGA_SERVER_STATE_ENABLED) &&
            (MDGA_SERVER_STATE_DISABLING != server->state))
        {
            /* Disable MDG server */
            status = BTL_MDG_Disable(server->serverContext);

            if (BT_STATUS_SUCCESS == status)
            {
                /* Destroy MDG server */
                status = BTL_MDG_Destroy(&server->serverContext);

                if (BT_STATUS_SUCCESS != status)
                {
                    Report(("MDGA: BTL_MDG_Destroy() failed for server %s, status %s",
                            server->serverSettings.serviceName,
                            pBT_Status(status)));
                }
                else
                {
                    server->state = MDGA_SERVER_STATE_NONE;
                }
            }
            else if ((BT_STATUS_PENDING == status) || (BT_STATUS_IN_PROGRESS == status))
            {
                /* Continue disabling server in MdgaBtlMdgCallback() */
                server->state = MDGA_SERVER_STATE_DISABLING;
            }
            else
            {
                Report(("MDGA: BTL_MDG_Disable() failed, status %s", pBT_Status(status)));
            }
        }
    }
}

/*-------------------------------------------------------------------------------
 * MDGA_Enable()
 *
 *		Processes command 'enable'.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	serverIndex - index of the server.
 *
 * Returns:
 *		None.
 */
void MDGA_Enable(U8 serverIndex)
{
	BtStatus status;
   	MdgaServer *server;

   	server = &mdgaServers[serverIndex];

    /* Check validity of the command */
    if (server->state >= MDGA_SERVER_STATE_ENABLED)
    {
	    Report(("MDGA: server %s is already enabled",
                server->serverSettings.serviceName));
        return;
    }

    /* Create MDG server */
    status = BTL_MDG_Create(0,
                            MdgaBtlMdgCallback,
                            &server->securityLevel,
                            &server->serverContext);
    if (BT_STATUS_SUCCESS != status)
    {
        Report(("MDGA: BTL_MDG_Create() failed, status %s",
                pBT_Status(status)));
    }
    else
    {
        /* Enable MDG server */
        status = BTL_MDG_Enable(server->serverContext,
                                &server->serverSettings);

        if (BT_STATUS_SUCCESS != status)
        {
            Report(("MDGA: BTL_MDG_Enable() failed, status %s",
                    pBT_Status(status)));
        }
        else
        {
            server->state = MDGA_SERVER_STATE_ENABLED;
            UI_DISPLAY(("MDGA: server %s enabled",
                        server->serverSettings.serviceName));
        }
    }

    return;
}

/*-------------------------------------------------------------------------------
 * MDGA_Disable()
 *
 *		Processes command 'enable'.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	serverIndex - index of the server.
 *
 * Returns:
 *		None.
 */
void MDGA_Disable(U8 serverIndex)
{
	BtStatus status;
   	MdgaServer *server = &mdgaServers[serverIndex];

    /* Check validity of the command */
    if (server->state < MDGA_SERVER_STATE_ENABLED)
    {
	    Report(("MDGA: server %s is not enabled",
                server->serverSettings.serviceName));
        return;
    }

    /* Disable MDG server */
    status = BTL_MDG_Disable(server->serverContext);

    if (BT_STATUS_SUCCESS == status)
    {
        /* Destroy MDG server */
        status = BTL_MDG_Destroy(&server->serverContext);

        if (BT_STATUS_SUCCESS != status)
        {
            Report(("MDGA: BTL_MDG_Destroy() failed, status %s",
                    pBT_Status(status)));
        }
        else
        {
            server->state = MDGA_SERVER_STATE_NONE;
	        UI_DISPLAY(("MDGA: server %s disabled",
                        server->serverSettings.serviceName));
        }
    }
    else if ((BT_STATUS_PENDING == status) || (BT_STATUS_IN_PROGRESS == status))
    {
        /* Continue disabling server in MdgaBtlMdgCallback() */
        server->state = MDGA_SERVER_STATE_DISABLING;
	    UI_DISPLAY(("MDGA: disabling server %s",
                    server->serverSettings.serviceName));
    }
    else
    {
        Report(("MDGA: BTL_MDG_Disable() failed, status %s", pBT_Status(status)));
    }

    return;
} 

/*-------------------------------------------------------------------------------
 * MDGA_GetSecurityLevel()
 *
 *		Processes command 'getsec'.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	serverIndex - index of the server.
 *
 * Returns:
 *		None.
 */
void MDGA_GetSecurityLevel(U8 serverIndex)
{
	BtStatus status;
	BtSecurityLevel securityLevel;
   	MdgaServer *server = &mdgaServers[serverIndex];
 
	status = BTL_MDG_GetSecurityLevel(server->serverContext, &securityLevel);

	if (BT_STATUS_SUCCESS == status)
	{
		UI_DISPLAY(("UI_EVENT: MDGA got security level %d for server %s",
			        securityLevel,
			        server->serverSettings.serviceName));
	}
	else
	{
		Report(("MDGA: BTL_MDG_GetSecurityLevel() failed for server %s, status %s",
			     server->serverSettings.serviceName,
			     pBT_Status(status)));
	}

	return;
}

/*-------------------------------------------------------------------------------
 * MDGA_SetSecurityLevel()
 *
 *		Processes command 'setsec'.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	serverIndex - index of the server.
 *
 * Returns:
 *		None.
 */
void MDGA_SetSecurityLevel(U8 serverIndex, BtSecurityLevel *securityLevel)
{
	BtStatus status;
   	MdgaServer *server = &mdgaServers[serverIndex];
 
	status = BTL_MDG_SetSecurityLevel(server->serverContext, securityLevel);

	if (BT_STATUS_SUCCESS == status)
	{
		UI_DISPLAY(("UI_EVENT: MDGA set security level %d for server %s",
			        *securityLevel,
			        server->serverSettings.serviceName));
	}
	else
	{
		Report(("MDGA: BTL_MDG_SetSecurityLevel() failed for server %s, status %s",
			     server->serverSettings.serviceName,
			     pBT_Status(status)));
	}

	return;
}

/*-------------------------------------------------------------------------------
 * MDGA_GetComSettings()
 *
 *		Processes command 'getcomset'.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	serverIndex - index of the server.
 *
 * Returns:
 *		None.
 */
void MDGA_GetComSettings(U8 serverIndex)
{
	BtStatus status;
	SppComSettings comSettings;
   	MdgaServer *server = &mdgaServers[serverIndex];
 
	status = BTL_MDG_GetComSettings(server->serverContext, &comSettings);

	if (BT_STATUS_SUCCESS == status)
	{
		UI_DISPLAY(("UI_EVENT: MDGA got com settings for server %s -", server->serverSettings.serviceName));
		UI_DISPLAY(("UI_EVENT: MDGA baudrate 0x%02x, data format 0x%02x, flow control 0x%02x, XON 0x%02x, XOFF 0x%02x",
				    comSettings.baudRate,
				    comSettings.dataFormat,
				    comSettings.flowControl,
				    comSettings.xonChar,
				    comSettings.xoffChar));
	}
	else
	{
		Report(("MDGA: BTL_MDG_GetComSettings() failed for server %s, status %s",
			     server->serverSettings.serviceName,
			     pBT_Status(status)));
	}

	return;
}

/*-------------------------------------------------------------------------------
 * MDGA_SetComSettings()
 *
 *		Processes command 'setcomset'.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	serverIndex - index of the server.
 *
 * Returns:
 *		None.
 */
void MDGA_SetComSettings(U8 serverIndex, SppComSettings *comSettings)
{
	BtStatus status;
   	MdgaServer *server = &mdgaServers[serverIndex];

	status = BTL_MDG_SetComSettings(server->serverContext, comSettings);

	if (BT_STATUS_SUCCESS == status)
	{
		UI_DISPLAY(("UI_EVENT: MDGA set com settings for server %s -", server->serverSettings.serviceName));
		UI_DISPLAY(("UI_EVENT: MDGA baudrate 0x%02x, data format 0x%02x, flow control 0x%02x, XON 0x%02x, XOFF 0x%02x, parm mask 0x%04x",
				    comSettings->baudRate,
				    comSettings->dataFormat,
				    comSettings->flowControl,
				    comSettings->xonChar,
				    comSettings->xoffChar,
				    comSettings->parmMask));
	}
	else if ((BT_STATUS_PENDING != status) && (BT_STATUS_IN_PROGRESS != status))
	{
		Report(("MDGA: BTL_MDG_GetComSettings() failed for server %s, status %s",
			     server->serverSettings.serviceName,
			     pBT_Status(status)));
	}

	return;
}

/*-------------------------------------------------------------------------------
 * MDGA_GetConnectedDevice()
 *
 *		Processes command 'getcondev'.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	serverIndex - index of the server.
 *
 * Returns:
 *		None.
 */
void MDGA_GetConnectedDevice(U8 serverIndex)
{
	BtStatus status;
	BD_ADDR bdAddr;
	char addr[BDADDR_NTOA_SIZE];
   	MdgaServer *server = &mdgaServers[serverIndex];
 
	status = BTL_MDG_GetConnectedDevice(server->serverContext, &bdAddr);

	if (BT_STATUS_SUCCESS == status)
	{
		UI_DISPLAY(("UI_EVENT: MDGA server %s is connected to %s",
			       server->serverSettings.serviceName,
			       bdaddr_ntoa((const BD_ADDR *)&bdAddr, addr)));
	}
	else
	{
		Report(("MDGA: MdgaGetConnectedDevice() failed for server %s, status %s",
			     server->serverSettings.serviceName,
			     pBT_Status(status)));
	}

	return;
}

/*-------------------------------------------------------------------------------
 * MDGA_TerminateDataCall()
 *
 *		Processes command 'termdatacall'.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	None.
 *
 * Returns:
 *		None.
 */
void MDGA_TerminateDataCall()
{
    return;
}

/*-------------------------------------------------------------------------------
 * MdgaBtlMdgCallback()
 *
 *		Callback function for receiving BTL MDG events.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	parms - received event.
 *
 * Returns:
 *		None.
 */
static void MdgaBtlMdgCallback(const BtlMdgCallbackParms *parms)
{
	BtStatus status;
    MdgaServer *server;
	int idx;
	
	/* Find a server according to given BTL MDG context */
	for (idx = 0, server=&mdgaServers[0]; idx<MDGA_NUM_SERVERS; idx++, server++)
	{
		if (server->serverContext == parms->serverContext)
		{
			break;
		}
	}

	/* Check whether we found a port in array of clients */
	if (MDGA_NUM_SERVERS == idx)
	{

		/* Server is not found */
		Report(("MDGA: server's context is unknown"));
		return;
	}

	/* Display received event and server's service name */
	UI_DISPLAY(("UI_EVENT: MDGA received event %s for server %s",
                MdgaEventName(parms->event),
                server->serverSettings.serviceName));

	/* Change port's state according to event */
	switch(parms->event)
	{
		case BTL_MDG_SERVER_EVENT_ENABLED:
			server->state = MDGA_SERVER_STATE_ENABLED;
			break;

		case BTL_MDG_SERVER_EVENT_DISABLED:
            if (MDGA_SERVER_STATE_DISABLING == server->state)
            {
            /* Destroy MDG server */
            status = BTL_MDG_Destroy(&server->serverContext);

                if (BT_STATUS_SUCCESS != status)
                {
                    Report(("MDGA: BTL_MDG_Destroy() failed, status %s",
                            pBT_Status(status)));
                }
                else
                {
                    server->state = MDGA_SERVER_STATE_NONE;
	                UI_DISPLAY(("MDGA: server %s disabled",
                                server->serverSettings.serviceName));
                }
            }
            break;

		case BTL_MDG_SERVER_EVENT_DT_CONNECTED:
            if (MDGA_SERVER_STATE_MODEM_CONNECTED == server->state)
            {
			    server->state = MDGA_SERVER_STATE_CONNECTED;
            }
            else
            {
			    server->state = MDGA_SERVER_STATE_DT_CONNECTED;
            }
			break;

		case BTL_MDG_SERVER_EVENT_DT_DISCONNECTED:
            if (MDGA_SERVER_STATE_CONNECTED == server->state)
            {
			    server->state = MDGA_SERVER_STATE_MODEM_CONNECTED;
            }
            else if (MDGA_SERVER_STATE_DISABLING != server->state)
            {
			    server->state = MDGA_SERVER_STATE_ENABLED;
            }
			break;

		case BTL_MDG_SERVER_EVENT_MODEM_CONNECTED:
            if (MDGA_SERVER_STATE_DT_CONNECTED == server->state)
            {
			    server->state = MDGA_SERVER_STATE_CONNECTED;
            }
            else
            {
			    server->state = MDGA_SERVER_STATE_DT_CONNECTED;
            }
			break;

		case BTL_MDG_SERVER_EVENT_MODEM_DISCONNECTED:
            if (MDGA_SERVER_STATE_CONNECTED == server->state)
            {
			    server->state = MDGA_SERVER_STATE_DT_CONNECTED;
            }
            else if (MDGA_SERVER_STATE_DISABLING != server->state)
            {
			    server->state = MDGA_SERVER_STATE_ENABLED;
            }
			break;

		default:
			break;
	}

	/* Display server's new state */
	Report(("MDGA: %s server's state %s",
           server->serverSettings.serviceName,
           MdgaStateName(server->state)));
}

/*-------------------------------------------------------------------------------
 * MdgaEventName()
 *
 *		Gets event's name.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	mdgEvent - event.
 *
 * Returns:
 *		String with event's name.
 */
static const char *MdgaEventName(BtlMdgServerEvent mdgEvent)
{
    switch (mdgEvent)
    {
        case BTL_MDG_SERVER_EVENT_INITIALIZED:
            return "BTL_MDG_SERVER_EVENT_INITIALIZED";

        case BTL_MDG_SERVER_EVENT_DEINITIALIZED:
            return "BTL_MDG_SERVER_EVENT_DEINITIALIZED";

        case BTL_MDG_SERVER_EVENT_ENABLED:
            return "BTL_MDG_SERVER_EVENT_ENABLED";

        case BTL_MDG_SERVER_EVENT_DISABLED:
            return "BTL_MDG_SERVER_EVENT_DISABLED";

        case BTL_MDG_SERVER_EVENT_DT_CONNECTED:
            return "BTL_MDG_SERVER_EVENT_DT_CONNECTED";

        case BTL_MDG_SERVER_EVENT_DT_DISCONNECTED:
            return "BTL_MDG_SERVER_EVENT_DT_DISCONNECTED";

        case BTL_MDG_SERVER_EVENT_MODEM_CONNECTED:
            return "BTL_MDG_SERVER_EVENT_MODEM_CONNECTED";

        case BTL_MDG_SERVER_EVENT_MODEM_DISCONNECTED:
            return "BTL_MDG_SERVER_EVENT_MODEM_DISCONNECTED";

        default:
            return "MDGA: server received unknown event";
    }
}

/*-------------------------------------------------------------------------------
 * MdgaStateName()
 *
 *		Gets state's name.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	state - server's state.
 *
 * Returns:
 *		String with state's name.
 */
static const char *MdgaStateName(int state)
{
    switch (state)
    {
        case MDGA_SERVER_STATE_NONE:
            return "MDGA_SERVER_STATE_NONE";

        case MDGA_SERVER_STATE_ENABLED:
            return "MDGA_SERVER_STATE_ENABLED";

        case MDGA_SERVER_STATE_DISABLING:
            return "MDGA_SERVER_STATE_DISABLING";

        case MDGA_SERVER_STATE_DT_CONNECTED:
            return "MDGA_SERVER_STATE_DT_CONNECTED";

        case MDGA_SERVER_STATE_MODEM_CONNECTED:
            return "MDGA_SERVER_STATE_MODEM_CONNECTED";

        case MDGA_SERVER_STATE_CONNECTED:
            return "MDGA_SERVER_STATE_CONNECTED";

        default:
            return "MDG server unknown state";
    }
}


#else /* BTL_CONFIG_MDG == BTL_CONFIG_ENABLED */



void MDGA_Init(void)
{
	
	Report(("MDGA_Init -BTL_CONFIG_MDG is disabled"));
	

}


void MDGA_Deinit(void)
{
    
	Report(("MDGA_Deinit -BTL_CONFIG_MDG is disabled"));
}



#endif /* BTL_CONFIG_MDG == BTL_CONFIG_ENABLED */

