/*******************************************************************************\
*                                                                          									 	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT			*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      spp_app.c
*
*   DESCRIPTION:    This file contains the implementation of the SPP sample application.
*					
*
*   AUTHOR:         
*
\*******************************************************************************/

#include "btl_config.h"
#include "osapi.h"
#if BTL_CONFIG_SPP == BTL_CONFIG_ENABLED

//#include "ffs.h"
#include "btl_common.h"
#include "debug.h"

#include "../app_main.h"

#include "btl_spp.h"
#include "bthal_config.h"
#include "bthal_fs.h"


/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

/* SPP Application port states */
#define SPPAPP_PORT_STATE_NONE			            (0)
#define SPPAPP_PORT_STATE_CREATED		            (1)
#define SPPAPP_PORT_STATE_DISCONNECTED	            (2)
#define SPPAPP_PORT_STATE_CONNECTED		            (3)
#define SPPAPP_PORT_STATE_DISABLING		            (4)
#define SPPAPP_PORT_STATE_SENDIND		            (5)
#define SPPAPP_PORT_STATE_RECEIVING		            (6)

/* SPP Application specific data types */
#define SPP_APP_SETTINGS_BAUDRATE                   (1)
#define SPP_APP_SETTINGS_CONTROL                     (3)
#define SPP_APP_SETTINGS_XON_CHAR                   (4)
#define SPP_APP_SETTINGS_XOFF_CHAR                 (5)

#define	SPP_APP_SETTINGS_DATA_BITS				    (6)
#define	SPP_APP_SETTINGS_DATA_STOP_BITS			    (7)
#define	SPP_APP_SETTINGS_DATA_PARITY_BITS		    (8)
#define	SPP_APP_SETTINGS_DATA_PARITY_TYPE_BITS	    (9)

/* SPP Application reading file working modes */
#define SPP_APP_FILE_MODE_SINGLE_READ				(1)
#define SPP_APP_FILE_MODE_FULL_THROUGHPUT			(2)
#define SPP_APP_FILE_MODE_SINGLE_FULL_BUFFER_WRITE  (3)

/* SPP Application destination file states */
#define SPP_APP_DEST_FILE_NOT_OPENED                (1)
#define SPP_APP_DEST_FILE_OPENED                         (2)

/* Invalid value of port ID */
#define SPP_APP_PORT_NOT_SELECTED                   (0xff)

/* Constants related to SPP Application's buffer */
#define SPP_APP_BUFFER_LENGTH                          (512)
#define SPP_APP_BUFFER_FULL                               (1)
#define SPP_APP_END_OF_FILE                               (2)

/* Max number of Rx Sync packets - this value will be given to peer serial port
 * as amount of credits for RFCOMM credit based flow control */
#define SPP_APP_MAX_NUM_OF_RX_SYNC_PACKETS          (5)

/* SPP Application files states */
#define SPP_APP_FILE_NOT_OPEN                      		(1)
#define SPP_APP_SEND_FILE_READY                   	(2)
#define SPP_APP_WAIT_BUFFER_SENT                 	(3)
#define SPP_APP_WAIT_FOR_EMPTY_TX_BUFF      	(4)
#define SPP_APP_REACHED_FILE_END		          	(5)


/* Tx Rx settings */
//typedef U8 SppPortDataPathType;

#define SPP_APP_TX_S_RX_A                      	(9)
#define SPP_APP_TX_A_RX_S                  	(6)
#define SPP_APP_TX_S_RX_S                 	(5)
#define SPP_APP_TX_A_RX_A      			(10)

/********************************************************************************
 *
 * Internal Data Structures
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * SppAppFileStruct structure
 *
 *	   Represents internal data used while working with files.
 */
typedef struct _SppAppFileStruct 
{
	char    fileRxName[100];
	char    fileTxName[100];
   	U16 *fileTx;
    	U16    *fileRx;
	U8      fileStateTx; 
	U8      fileStateRx; 
	U16     dataSent;
	U16     buffLenTx; 
/*	U8      buffTx[SPP_APP_BUFFER_LENGTH];	*/
	U8      buffRx[SPP_APP_BUFFER_LENGTH];
	U16     bytesNumRead;
	U16     totalBytesRx;
	char    *ptrRx;

} SppAppFileStruct;

/*-------------------------------------------------------------------------------
 * SppAppCommonPort structure
 *
 *	   Represents internal data common for the server and for the client ports.
 */
typedef struct _SppAppCommonPort
{
	U8                      state;
	U8                      portId;
	const BtSecurityLevel   securityLevel;
	BtlSppPortContext       *portContext;
	const char              serviceName[SPP_SERVICE_NAME_MAX_SIZE];
    	BtlSppPortSettings      btlPortSettings;
	SppControlSignals       controlSignals;
	SppLineStatus           lineStatus;
    	U16                     rxCnt;
	SppAppFileStruct        fileStruct;
	BtSecurityLevel         secLevel;

} SppAppCommonPort;

/*-------------------------------------------------------------------------------
 * SppAppCommonPort structure
 *
 *	   Client port attributes.
 */
typedef struct _SppAppClientPort
{
    SppAppCommonPort     port;       /* Client port for that service */
    SdpServiceClassUuid  uuid;       /* UUID of service on device */
    BD_ADDR				 bdAddr;	 /* 48-bit address of the remote device */

} SppAppClientPort;

/*-------------------------------------------------------------------------------
 * SppAppServerPort structure
 *
 *	   Server port attributes.
 */
typedef SppAppCommonPort SppAppServerPort;

static U8 dataSent;



//==========================================



 /* The multiIsEnabled state flags whether the 
 * "Multiparty" checkbox is grayed out or not.
 */ 
static BOOL                 multiIsEnabled = FALSE;

static BtStatus status = BT_STATUS_FAILED;


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * sppAppServerPorts
 *
 *     Represents the default parameters of the server ports.
 */
static SppAppServerPort sppAppServerPorts[] = {
    {SPPAPP_PORT_STATE_NONE, 0, SPP_SECURITY_SETTINGS_DEFAULT, 0, "Dial-up Networking",
        {SPP_SERVER_PORT,
         BTL_SPP_SERVICE_TYPE_DUN,
         SPP_PORT_DATA_PATH_TX_SYNC | SPP_PORT_DATA_PATH_RX_ASYNC,
         SPP_APP_MAX_NUM_OF_RX_SYNC_PACKETS,
		     {SPP_COM_SETTING_BAUDRATE_DEFAULT,
		      SPP_COM_SETTING_DATA_FORMAT_DEFAULT,
		      SPP_COM_SETTING_FLOW_CONTROL_DEFAULT,
		      SPP_COM_SETTING_XON_CHAR_DEFAULT,
		      SPP_COM_SETTING_XOFF_CHAR_DEFAULT,
		      0}},
		{0, 0}, 0, 0, 
		{"/SPP/dataFromClientDialUp.txt", {0}, 0, 0, SPP_APP_FILE_NOT_OPEN ,SPP_APP_DEST_FILE_NOT_OPENED , 0, 0, {0}},
		BSL_NO_SECURITY},
    {SPPAPP_PORT_STATE_NONE, 1, SPP_SECURITY_SETTINGS_DEFAULT, 0, "FAX",
        {SPP_SERVER_PORT,
         BTL_SPP_SERVICE_TYPE_FAX,
         SPP_PORT_DATA_PATH_TX_SYNC | SPP_PORT_DATA_PATH_RX_ASYNC,
         SPP_APP_MAX_NUM_OF_RX_SYNC_PACKETS,
		     {SPP_COM_SETTING_BAUDRATE_DEFAULT,
		      SPP_COM_SETTING_DATA_FORMAT_DEFAULT,
		      SPP_COM_SETTING_FLOW_CONTROL_DEFAULT,
		      SPP_COM_SETTING_XON_CHAR_DEFAULT,
		      SPP_COM_SETTING_XOFF_CHAR_DEFAULT,
              0}},
		{0, 0}, 0, 0, 
		{"/SPP/dataFromClientFax.txt", {0}, 0, 0, SPP_APP_FILE_NOT_OPEN ,SPP_APP_DEST_FILE_NOT_OPENED , 0, 0, {0}}, 
		BSL_NO_SECURITY},
    {SPPAPP_PORT_STATE_NONE, 2, SPP_SECURITY_SETTINGS_DEFAULT, 0, "Serial Port 1",
        {SPP_SERVER_PORT,
         BTL_SPP_SERVICE_TYPE_SPP,
         SPP_PORT_DATA_PATH_TX_SYNC | SPP_PORT_DATA_PATH_RX_ASYNC,
         SPP_APP_MAX_NUM_OF_RX_SYNC_PACKETS,
		    {SPP_COM_SETTING_BAUDRATE_DEFAULT,
		     SPP_COM_SETTING_DATA_FORMAT_DEFAULT,
		     SPP_COM_SETTING_FLOW_CONTROL_DEFAULT,
		     SPP_COM_SETTING_XON_CHAR_DEFAULT,
		     SPP_COM_SETTING_XOFF_CHAR_DEFAULT,
		     0}},
		{0, 0}, 0, 0, 
		{"/SPP/dataFromClientSerial1.txt", {0}, 0, 0, SPP_APP_FILE_NOT_OPEN ,SPP_APP_DEST_FILE_NOT_OPENED , 0, 0, {0}},
		BSL_NO_SECURITY},
	{SPPAPP_PORT_STATE_NONE, 3, SPP_SECURITY_SETTINGS_DEFAULT, 0, "Serial Port 2",
        {SPP_SERVER_PORT,
         BTL_SPP_SERVICE_TYPE_SPP,
         SPP_PORT_DATA_PATH_TX_SYNC | SPP_PORT_DATA_PATH_RX_ASYNC,
         SPP_APP_MAX_NUM_OF_RX_SYNC_PACKETS,
		    {SPP_COM_SETTING_BAUDRATE_DEFAULT,
		     SPP_COM_SETTING_DATA_FORMAT_DEFAULT,
		     SPP_COM_SETTING_FLOW_CONTROL_DEFAULT,
		     SPP_COM_SETTING_XON_CHAR_DEFAULT,
		     SPP_COM_SETTING_XOFF_CHAR_DEFAULT,
		     0}},
		{0, 0}, 0, 0, 
		{"/SPP/dataFromClientSerial2.txt", {0}, 0, 0, SPP_APP_FILE_NOT_OPEN ,SPP_APP_DEST_FILE_NOT_OPENED , 0, 0, {0}},
		BSL_NO_SECURITY}	
};

#define SPP_APP_NUM_SERVERS     (sizeof(sppAppServerPorts)/sizeof(SppAppServerPort))

/*-------------------------------------------------------------------------------
 * sppAppClientPorts
 *
 *     Represents the default parameters of the client ports.
 */
static SppAppClientPort sppAppClientPorts[] = {
    {{SPPAPP_PORT_STATE_NONE, 0, SPP_SECURITY_SETTINGS_DEFAULT, 0, "Dial-up Networking",
        {SPP_CLIENT_PORT,
         BTL_SPP_SERVICE_TYPE_DUN,
         SPP_PORT_DATA_PATH_TX_SYNC | SPP_PORT_DATA_PATH_RX_ASYNC,
         SPP_APP_MAX_NUM_OF_RX_SYNC_PACKETS,
		     {SPP_COM_SETTING_BAUDRATE_DEFAULT,
		      SPP_COM_SETTING_DATA_FORMAT_DEFAULT,
		      SPP_COM_SETTING_FLOW_CONTROL_DEFAULT,
		      SPP_COM_SETTING_XON_CHAR_DEFAULT,
		      SPP_COM_SETTING_XOFF_CHAR_DEFAULT,
              0}},
		{0,0}, 0, 0, 
		{"/SPP/dataFromServerDialUp.txt", {0}, 0, 0, SPP_APP_FILE_NOT_OPEN ,SPP_APP_DEST_FILE_NOT_OPENED , 0, 0, {0}},
		BSL_NO_SECURITY},
    	SC_DIALUP_NETWORKING}, 
    {{SPPAPP_PORT_STATE_NONE, 1, SPP_SECURITY_SETTINGS_DEFAULT, 0, "FAX",
        {SPP_CLIENT_PORT,
         BTL_SPP_SERVICE_TYPE_FAX,
         SPP_PORT_DATA_PATH_TX_SYNC | SPP_PORT_DATA_PATH_RX_ASYNC,
         SPP_APP_MAX_NUM_OF_RX_SYNC_PACKETS,
             {SPP_COM_SETTING_BAUDRATE_DEFAULT,
		      SPP_COM_SETTING_DATA_FORMAT_DEFAULT,
		      SPP_COM_SETTING_FLOW_CONTROL_DEFAULT,
		      SPP_COM_SETTING_XON_CHAR_DEFAULT,
		      SPP_COM_SETTING_XOFF_CHAR_DEFAULT,
              0}},
		{0,0}, 0, 0,
		{"/SPP/dataFromServerFax.txt", {0}, 0, 0, SPP_APP_FILE_NOT_OPEN ,SPP_APP_DEST_FILE_NOT_OPENED , 0, 0, {0}},
		BSL_NO_SECURITY},
     	SC_FAX}, 
    {{SPPAPP_PORT_STATE_NONE, 2, SPP_SECURITY_SETTINGS_DEFAULT, 0, "Serial Port 1",
        {SPP_CLIENT_PORT,
         BTL_SPP_SERVICE_TYPE_SPP,
         SPP_PORT_DATA_PATH_TX_SYNC | SPP_PORT_DATA_PATH_RX_ASYNC,
         SPP_APP_MAX_NUM_OF_RX_SYNC_PACKETS,
		    {SPP_COM_SETTING_BAUDRATE_DEFAULT,
		     SPP_COM_SETTING_DATA_FORMAT_DEFAULT,
		     SPP_COM_SETTING_FLOW_CONTROL_DEFAULT,
		     SPP_COM_SETTING_XON_CHAR_DEFAULT,
		     SPP_COM_SETTING_XOFF_CHAR_DEFAULT,
             0}},
		{0,0}, 0, 0,
		{"/SPP/dataFromServerSerial1.txt", {0}, 0, 0, SPP_APP_FILE_NOT_OPEN ,SPP_APP_DEST_FILE_NOT_OPENED , 0, 0, {0}},
		BSL_NO_SECURITY},
		SC_SERIAL_PORT}, 
	{{SPPAPP_PORT_STATE_NONE, 3, SPP_SECURITY_SETTINGS_DEFAULT, 0, "Serial Port 2",
        {SPP_CLIENT_PORT,
         BTL_SPP_SERVICE_TYPE_SPP,
         SPP_PORT_DATA_PATH_TX_SYNC | SPP_PORT_DATA_PATH_RX_ASYNC,
         SPP_APP_MAX_NUM_OF_RX_SYNC_PACKETS,
		    {SPP_COM_SETTING_BAUDRATE_DEFAULT,
		     SPP_COM_SETTING_DATA_FORMAT_DEFAULT,
		     SPP_COM_SETTING_FLOW_CONTROL_DEFAULT,
		     SPP_COM_SETTING_XON_CHAR_DEFAULT,
		     SPP_COM_SETTING_XOFF_CHAR_DEFAULT,
             0}},
		{0,0}, 0, 0,
		{"/SPP/dataFromServerSerial2.txt", {0}, 0, 0, SPP_APP_FILE_NOT_OPEN ,SPP_APP_DEST_FILE_NOT_OPENED , 0, 0, {0}},
		BSL_NO_SECURITY},
		SC_SERIAL_PORT}	
};

#define SPP_APP_NUM_CLIENTS     (sizeof(sppAppClientPorts)/sizeof(SppAppClientPort))

/* Application's state */
static BOOL sppAppExiting = FALSE;

/* External function client uses to check if service is on the remote device. */
//static BtDeviceContext *dev;

/* Number of open ports */
static U8 sppAppNumOfOpenPorts;

/* Number of connected ports */
static U8 sppAppNumOfConnectedPorts = 0;

/* Currently selected client port */
static SppAppClientPort *sppAppSelectedClient = 0 ;

/* Currently selected server port */
static SppAppServerPort *sppAppSelectedServer = 0 ;







/********************************************************************************
 *
 * External function declarations
 *
 *******************************************************************************/
void SPPA_Init(void);
BOOL SPPA_Deinit(void);
void SPPA_EnableClient(BtlSppServiceType index ,SppPortDataPathType txRxSettings,U8 *serviceName);
 void SPPA_DisableClient(BtlSppServiceType index);
 void SPPA_EnableServer(BtlSppServiceType index,SppPortDataPathType txRxSettings,U8 *serviceName);
 void SPPA_DisableServer(BtlSppServiceType index);
void SPPA_Connect(BD_ADDR *bd_addr,BtlSppServiceType index);
void SPPA_WriteDataS(BtlSppServiceType index);
void SPPA_WriteDataC(BtlSppServiceType index);

void SPPA_DisconnectS(BtlSppServiceType index);
void SPPA_DisconnectC(BtlSppServiceType index);

void SPPA_Exit();


/*******************************************************************************
 *
 * Internal Function prototypes
 *
 ******************************************************************************/

static void SppAppBtlCallback(const BtlSppPortEvent *event);
static void SppAppFlushRelevantBuffers(SppAppCommonPort *commonPort);

const char *pSPP_Event(SppEvent sppEvent);
const char *pSPP_Settings(U8 settingsType, U8 settingsValue);
static void GetStateStr(U8 state, char * stateStr);



/*---------------------------------------------------------------------------
 *            SPPA_Init
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX server for the OPP application 
 *            and the Object Store.
 *
 * Return:    void
 *
 */
void SPPA_Init(void)
{
	U8 i;
	BtStatus    status;
	char stateStr[15];

	Report(("SPP_APP : SPPA_Init  Function entered"));
	
	/* Create all clients */
	for (i=0; i<SPP_APP_NUM_CLIENTS; i++)
    {
	/* Create BTL SPP context */
		Report(("SPP_APP : SPPA_Init  trying to create Client port\n"));
		status = BTL_SPP_Create(0,
				  				SppAppBtlCallback,
								&sppAppClientPorts[i].port.secLevel,
								&sppAppClientPorts[i].port.portContext);


		if (BT_STATUS_SUCCESS != status)
		{
			Report(("SPP_APP : BT_STATUS NO SUCCESS in creating Client port\n"));
			return;
		}

		else
		{
			sppAppClientPorts[i].port.state = SPPAPP_PORT_STATE_CREATED;
			Report(("SPP_APP : A Client Port Was Created"));
			
		}
	}

	/* Create all servers */
	for (i=0; i<SPP_APP_NUM_SERVERS; i++) 
	{
		Report(("SPP_APP : SPPA_Init  trying to create Server port\n"));
		status = BTL_SPP_Create(0,
					  			SppAppBtlCallback,
								&sppAppServerPorts[i].secLevel,
								&sppAppServerPorts[i].portContext);

		if (BT_STATUS_SUCCESS != status)
		{
			Report(("SPP_APP : BT_STATUS NO SUCCESS in creating Server port\n"));
			return;
		}

		else
		{
			sppAppServerPorts[i].state = SPPAPP_PORT_STATE_CREATED;
			Report(("SPP_APP : A Server Port Was Created"));
		}
		Report(("SPP_APP : BTL_SPP_Create() [server] returned %s.",pBT_Status(status)));
	}
	Report(("SPP_APP : 4 client ports and 4 server ports were created with security level 0\n"));

	GetStateStr(SPPAPP_PORT_STATE_CREATED, stateStr);

}


/*---------------------------------------------------------------------------
 *            OPPSA_Deinit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Server protocol.
 *
 * Return:    BOOL
 *
 */
BOOL SPPA_Deinit(void)
{
    SppAppCommonPort *commonPort;
    BtStatus status;
    BOOL allPortsDestroyed;
    U8 i;

    allPortsDestroyed = TRUE;

    /* Check all the client and the server ports */
    for (i=0; i<SPP_APP_NUM_CLIENTS+SPP_APP_NUM_SERVERS; i++)
    {
    
        if (i < SPP_APP_NUM_CLIENTS)
        {
            commonPort = &sppAppClientPorts[i].port;
        }
        else
        {
            commonPort = &sppAppServerPorts[i-SPP_APP_NUM_CLIENTS];
        }
    
        switch(commonPort->state)
        {
            case SPPAPP_PORT_STATE_DISABLING:
                /* Nothing to do - wait for the port to be disabled */
                allPortsDestroyed = FALSE;
                break;

            case SPPAPP_PORT_STATE_CONNECTED:
            case SPPAPP_PORT_STATE_SENDIND:
            case SPPAPP_PORT_STATE_RECEIVING:
            case SPPAPP_PORT_STATE_DISCONNECTED:
                /* Disable the port. If it is connected, it will be
                 * disconnected automatically */
                status = BTL_SPP_Disable(commonPort->portContext);

                if (BT_STATUS_SUCCESS == status)
                {
                    /* Update the port's state */
                    commonPort->state = SPPAPP_PORT_STATE_CREATED;

                    /* Go to the next case - destroy the port */
                }
                else
                {
                    /* Update the port's state */
                    commonPort->state = SPPAPP_PORT_STATE_DISABLING;

                    allPortsDestroyed = FALSE;
                    break;
                }

            case SPPAPP_PORT_STATE_CREATED:
                /* Destroy the port */
                status = BTL_SPP_Destroy(&commonPort->portContext);

                if (BT_STATUS_SUCCESS == status)
                {
                    /* Update the port's state */
                    commonPort->state = SPPAPP_PORT_STATE_NONE;
                }
                else
                {
                    /* Nothing to do with bad status - only report it */
                    Report(("SPP_APP : SPPA_Deinit: BTL_SPP_Destroy() failed, status %s",
                            pBT_Status(status)));
                }
                break;

            default:
                break;
        }
    }
   return (allPortsDestroyed);
}
/*---------------------------------------------------------------------------
 *            SPPA_EnableClient()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  
 *
 * Return:    void
 *
 */
  void SPPA_EnableClient(BtlSppServiceType index ,SppPortDataPathType txRxSettings,U8 *serviceName)
{
	Report(("SPP_APP : Enabeling Client Type=%d TxRx=%d serviceName=%s",index,txRxSettings,serviceName));		        
	
    if (index < BTL_SPP_SERVICE_TYPE_DUN)
	{
		Report(("SPP_APP : SPPA_EnableClient :Can't enable context. Select an active client first"));
		return;
	}

	sppAppSelectedClient = &sppAppClientPorts[index];
			        
      if (txRxSettings != 0)
      {
		sppAppSelectedClient->port.btlPortSettings.dataPathTypes = txRxSettings;
       }
				      
	 /* If we are enabling SPP */
	if(BTL_SPP_SERVICE_TYPE_SPP == sppAppSelectedClient->port.btlPortSettings.serviceType)
        {
		if(*serviceName != '\0')
        	{
        		/*Report(( "SPP_APP : The name of service to connect the client to: %s",
                              sppAppSelectedClient->port.serviceName));
	
			status = BTL_SPP_Enable(sppAppSelectedClient->port.portContext,
									        sppAppSelectedClient->port.serviceName,
									        &sppAppSelectedClient->port.btlPortSettings); 	
			strcpy((char *)sppAppSelectedClient->port.serviceName, serviceName);*/
        	}
	}

	status = BTL_SPP_Enable(sppAppSelectedClient->port.portContext,
									        0,
									        &sppAppSelectedClient->port.btlPortSettings); 
	Report(( "SPP_APP : BTL_SPP_Enable returned %s", pBT_Status(status)));
	 if (BT_STATUS_FAILED == status)
        {
		return;
         }
	sppAppSelectedClient->port.state = SPPAPP_PORT_STATE_DISCONNECTED;
       sppAppSelectedClient->port.fileStruct.fileStateRx = SPP_APP_DEST_FILE_NOT_OPENED;
       sppAppSelectedClient->port.fileStruct.fileStateTx = SPP_APP_DEST_FILE_NOT_OPENED;
			       
}
/*---------------------------------------------------------------------------
 *            SPPA_DisableClient()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  
 *
 * Return:    void
 *
 */
 void SPPA_DisableClient(BtlSppServiceType index)
{
	if (index < BTL_SPP_SERVICE_TYPE_DUN)
	{
		Report(("SPP_APP : SPPA_DisableClient :Can't disable context. Select an enabled client first"));
		return;
	}
	 sppAppSelectedClient = &sppAppClientPorts[index];
	if (SPPAPP_PORT_STATE_CONNECTED == sppAppSelectedClient->port.state)
	{
		Report(("SPP_APP : SPPA_DisableClient :Please disconnect first"));
		return;
	}
	status = BTL_SPP_Disable(sppAppSelectedClient->port.portContext);
	Report(("SPP_APP : SPPA_DisableClient :BTL_SPP_Disable returned %s",pBT_Status(status)));
	if (BT_STATUS_FAILED == status)
		return;
	sppAppSelectedClient->port.state = SPPAPP_PORT_STATE_CREATED;


}
/*---------------------------------------------------------------------------
 *            SPPA_EnableServer()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  
 *
 * Return:    void
 *
 */
 void SPPA_EnableServer(BtlSppServiceType index,SppPortDataPathType txRxSettings,U8 *serviceName)
{
	Report(("SPP_APP : Enabeling Server Type=%d TxRx=%d serviceName=%s",index,txRxSettings,serviceName));
	if (index < BTL_SPP_SERVICE_TYPE_DUN)
	{
		Report(("SPP_APP : SPPA_EnableServer :Can't enable context. Select an active server first"));
		return;
	}
			
	sppAppSelectedServer = &sppAppServerPorts[index];
	if (txRxSettings != 0)
       {
		sppAppSelectedServer->btlPortSettings.dataPathTypes = txRxSettings;
       }
       if(BTL_SPP_SERVICE_TYPE_SPP ==     sppAppSelectedServer->btlPortSettings.serviceType)
       {
       	if(*serviceName != '\0')
        	{	
	      		strcpy((char *)sppAppSelectedServer->serviceName, serviceName);
				
       	}
	}
	 status = BTL_SPP_Enable(sppAppSelectedServer->portContext,
									        (const BtlUtf8 *)sppAppSelectedServer->serviceName,
									        &sppAppSelectedServer->btlPortSettings);
	Report(("SPP_APP : SPPA_EnableServer :BTL_SPP_Enable returned %s",pBT_Status(status))); 
	if (BT_STATUS_FAILED == status)
		return;
	 sppAppSelectedServer->state = SPPAPP_PORT_STATE_DISCONNECTED;
        sppAppSelectedServer->fileStruct.fileStateRx = SPP_APP_DEST_FILE_NOT_OPENED;
        sppAppSelectedServer->fileStruct.fileStateTx = SPP_APP_DEST_FILE_NOT_OPENED;
	
	
}
/*---------------------------------------------------------------------------
 *            
 *---------------------------------------------------------------------------
 *
 * Synopsis:  
 *
 * Return:    void
 *
 */
 void SPPA_DisableServer(BtlSppServiceType index)
{
	if (index < BTL_SPP_SERVICE_TYPE_DUN)
	{
		Report(("SPP_APP : SPPA_EnableServer :Can't disable context. Select an active server first"));
		return;
	}
			
	sppAppSelectedServer = &sppAppServerPorts[index];
	

	 status = BTL_SPP_Disable(sppAppSelectedServer->portContext);
	Report(("SPP_APP : SPPA_EnableServer :BTL_SPP_Disable returned %s",pBT_Status(status)));
	if (BT_STATUS_FAILED == status)
		return;
	  sppAppSelectedServer->state = SPPAPP_PORT_STATE_CREATED;
}
/*---------------------------------------------------------------------------
 *            SPPA_Connect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  
 *
 * Return:    void
 *
 */
void SPPA_Connect(BD_ADDR *bd_addr,BtlSppServiceType index)
{
	if (index < BTL_SPP_SERVICE_TYPE_DUN)
	{
		Report(("SPP_APP : Can't disconnect. Select an active server first"));
		return;
	}
	sppAppSelectedClient = &sppAppClientPorts[index];
	if (0 != sppAppSelectedClient->port.portContext) {
		OS_MemCopy((U8 *)&sppAppSelectedClient->bdAddr.addr,
			   							(U8 *)bd_addr,
							  			 BD_ADDR_SIZE);

		if (SPPAPP_PORT_STATE_DISCONNECTED == sppAppSelectedClient->port.state)
		{
			

			if (BT_STATUS_SUCCESS ==
				(status = BTL_SPP_Connect(sppAppSelectedClient->port.portContext,
													  &sppAppSelectedClient->bdAddr))) 
			{
				sppAppSelectedClient->port.state = SPPAPP_PORT_STATE_CONNECTED;
				sppAppSelectedClient->port.rxCnt = 0;
				sppAppNumOfOpenPorts++;
				Report(("SPP_APP : SPPA_ConnectC :Client Connect\n"));
			}
			else if ((BT_STATUS_PENDING == status) || (BT_STATUS_IN_PROGRESS == status)) {
				Report(("SPPA_ConnectC :BTL_SPP_Connect() status for %s: %s\n", 
				sppAppSelectedClient->port.serviceName, pBT_Status(status)));
			}
			else 
			{
				Report(("SPP_APP : SPPA_ConnectC :BTL_SPP_Connect() failed for %s: status %s\n", 
				sppAppSelectedClient->port.serviceName, pBT_Status(status)));
			}
		}
		else
					{
						Report(("SPP_APP : SPPA_ConnectC :Selected port %s is not in IDLE state\n", 
							sppAppSelectedClient->port.serviceName));
		}
	}
}
/*---------------------------------------------------------------------------
 *            
 *---------------------------------------------------------------------------
 *
 * Synopsis:  
 *
 * Return:    void
 *
 */
void SPPA_WriteDataS(BtlSppServiceType index)
{
	U16 writtenBytes;
	
	SppAppCommonPort *commonPort;
	Report(("SPP_APP : Entered : SPPA_WriteDataS"));
	if (index < BTL_SPP_SERVICE_TYPE_DUN)
	{
		Report(("SPP_APP : Can't disconnect. Select an active server first"));
		return;
	}
	 sppAppSelectedServer = &sppAppServerPorts[index];
	
	/* Find a port according to given BTL SPP context */
	
	if (sppAppSelectedServer->btlPortSettings.dataPathTypes &
                                                            SPP_PORT_DATA_PATH_TX_SYNC)
       {
           status = BTL_SPP_WriteSync(sppAppSelectedServer->portContext,
							                                (const U8 *)"Hello World!",  12,
							                                &writtenBytes);
        }
        else
        {
             status = BTL_SPP_WriteAsync(sppAppSelectedServer->portContext,
							                                (const U8 *)"Hello World!", 12);
        }
        if (BT_STATUS_SUCCESS == status)
        {
           Report ((  "SPP_APP : %d byets written to client \n",writtenBytes));
        }
        else
        {
           Report (( "SPP_APP : Writing failed, status %s\n", pBT_Status(status)));
        }
	Report(("SPP_APP : Exited : SPPA_WriteDataS")); 
}

/*---------------------------------------------------------------------------
 *           Report(("Entered : ")); 
 *---------------------------------------------------------------------------
 *
 * Synopsis:  
 *
 * Return:    void
 *
 */
void SPPA_WriteDataC(BtlSppServiceType index)
{
	U16 writtenBytes;
	Report(("SPP_APP : Entered : SPPA_WriteDataC"));
	if (index < BTL_SPP_SERVICE_TYPE_DUN)
	{
		Report(("SPP_APP : Can't disconnect. Select an active server first"));
	return;
	}
	sppAppSelectedClient = &sppAppClientPorts[index];
	
	if (sppAppSelectedClient->port.btlPortSettings.dataPathTypes &
                                                                SPP_PORT_DATA_PATH_TX_SYNC)
	{
	         status = BTL_SPP_WriteSync(sppAppSelectedClient->port.portContext,
		 	            (const U8 *)"Hello World!", 12, &writtenBytes);
	} else
	{
	     status = BTL_SPP_WriteAsync(sppAppSelectedClient->port.portContext,
				  (const U8 *)"Hello World!", 12);
	}
 	if (status == BT_STATUS_SUCCESS) 
	{
		Report (("SPP_APP : %d byets written to server ",
                                   writtenBytes));
	}
	else
	{
		Report(( "SPP_APP : Writing failed, status %s\n",
                                  pBT_Status(status)));
	}
	Report(("SPP_APP : Exited : SPPA_WriteDataC")); 		    
}


/*---------------------------------------------------------------------------
 *            SPPA_DisconnectS
 *---------------------------------------------------------------------------
 *
 * Synopsis:  
 *
 * Return:    void
 *
 */
void SPPA_DisconnectS(BtlSppServiceType index)
{
	if (index < BTL_SPP_SERVICE_TYPE_DUN)
	{
		Report(("SPP_APP : Can't disconnect. Select an active server first"));
	return;
	}
	sppAppSelectedServer = &sppAppServerPorts[index];
	Report(("SPP_APP : BTL_SPP_Disconnect returned %s", pBT_Status(BTL_SPP_Disconnect(sppAppSelectedServer->portContext))));
}

/*---------------------------------------------------------------------------
 *            SPPA_DisconnectC
 *---------------------------------------------------------------------------
 *
 * Synopsis: 
 *
 * Return:    void
 *
 */
void SPPA_DisconnectC(BtlSppServiceType index)
{
	if (index < BTL_SPP_SERVICE_TYPE_DUN)
	{
		Report(("SPP_APP : Can't close file. Select an active client first"));
		return;
	}

	sppAppSelectedClient = &sppAppClientPorts[index];
	Report(("SPP_APP : BTL_SPP_Disconnect returned %s", pBT_Status(BTL_SPP_Disconnect(sppAppSelectedClient->port.portContext))));

}

 
 /*---------------------------------------------------------------------------
 *            OPPSA_Destroy
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Releases a OPPS context (previously allocated with BTL_OPPS_Create).
 *
 * Return:    void
 *
 */
 
void SPPA_Exit()
{
	if (FALSE == SPPA_Deinit())
				    {
                        /* Continue destroying contexts after shutting down all
                         * ports in SppAppBtlSppCallback() function */
                        sppAppExiting = TRUE;
	}
}
 
/*---------------------------------------------------------------------------
 *             SppAppBtlCallback() 
 *--------------------------------------------------------------------------- 
 *
 *  Synopsis:    Callback function to receive BTL SPP events.
 *
 * Parameters:
 *		event - event information received from BTL SPP module
 *
 * Returns:
 *      None.
 */
static void SppAppBtlCallback(const BtlSppPortEvent *event)
{
	SppAppCommonPort *commonPort;
	U8 i;
	U8 textBoxId;
	char stateStr[15];
	
	
	/* Find a port according to given BTL SPP context */
	for (i=0, commonPort=&sppAppServerPorts[0]; i<SPP_APP_NUM_SERVERS; i++, commonPort++)
	{
		if (commonPort->portContext == event->portContext)
		{
			break;
		}
	}


	/* Check whether we found a port in array of clients */
	if (SPP_APP_NUM_SERVERS == i)
	{
		for (i=0; i<SPP_APP_NUM_CLIENTS; i++)
		{
			commonPort = &(sppAppClientPorts[i].port);
			if (commonPort->portContext == event->portContext)
			{
				break;
			}
		}

		if (SPP_APP_NUM_CLIENTS == i)
		{
			/* Port is not found */
			Report(("SPP_APP : Port's context is unknown"));
			return;
		}
	}
	/* Change port's state according to event */
	switch(event->parms->event)
	{
		case SPP_EVENT_ACL_CREATION_FAILED:
			Report(("SPP_APP : Connection Failed SPP_EVENT_ACL_CREATION_FAILED"));
		case SPP_EVENT_SDP_QUERY_FAILED:
			
			Report(("SPP_APP : Connection Failed SPP_EVENT_SDP_QUERY_FAILED"));
			
			break;

		case SPP_EVENT_OPEN:
			Report(("SPP_APP : SPP_EVENT_OPEN"));
			commonPort->state = SPPAPP_PORT_STATE_CONNECTED;
			//GetStateStr(commonPort->state, stateStr);
			
			if (SPP_CLIENT_PORT == commonPort->btlPortSettings.portType)
			{
				sppAppNumOfConnectedPorts++;
				Report(("SPP_APP : Client connected"));
			}
			else
			{
				sppAppNumOfConnectedPorts++;
				Report(("SPP_APP : Server connected"));
			}
			
			//SendDlgItemMessage(AppWnd, textBoxId, WM_SETTEXT, 0, (LPARAM)stateStr);

			break;

		case SPP_EVENT_CLOSED:
			Report(("SPP_APP : SPP_EVENT_CLOSED"));
			sppAppNumOfOpenPorts--;
			commonPort->state = SPPAPP_PORT_STATE_DISCONNECTED;
			//GetStateStr(commonPort->state, stateStr);
			
			if (SPP_CLIENT_PORT == commonPort->btlPortSettings.portType)
			{
				sppAppNumOfConnectedPorts--;
				/* Flush relevant buffers, if the port was in data transfer
		                 * state */
		              SppAppFlushRelevantBuffers(commonPort);
		                
				/* Update the port's state and GUI */
		             commonPort->state = SPPAPP_PORT_STATE_DISCONNECTED;
		             //SppAppSetRelevantBoxesClient(commonPort);
				Report(("SPP_APP : Client connected"));
			}
			else
			{
				sppAppNumOfConnectedPorts--;

				/* Flush relevant buffers, if the port was in data transfer
                		 * state */
               		 SppAppFlushRelevantBuffers(commonPort);

			   	 /* Update the port's state and GUI */
               		 commonPort->state = SPPAPP_PORT_STATE_DISCONNECTED;
				//SppAppSetRelevantBoxesServer(commonPort);
				//textBoxId = GetServerStateTextBox(i);
				Report(("SPP_APP : Client connected"));
			}
			
			//SendDlgItemMessage(AppWnd, textBoxId, WM_SETTEXT, 0, (LPARAM)stateStr);

			Report(("SPP_APP : Client connected"));
			break;


#if RF_SEND_CONTROL == XA_ENABLED
		case SPP_EVENT_PORT_NEG_CNF:
			/* Peer confirmed port negotiation */
			
			Report(("SPP_APP : Peer confirmed port params"));
			
			if (event->parms->p.portSettings.parmMask == RF_PARM_BAUDRATE)
			{
				Report(("SPP_APP : Peer confirmed baud rate %s\n", pSPP_Settings(SPP_APP_SETTINGS_BAUDRATE, 
																		 event->parms->p.portSettings.baudRate))); 
				
			}
			break;
#endif /* RF_SEND_CONTROL enabled */

		case SPP_EVENT_PORT_NEG_IND:
			Report(("SPP_APP : Peer  is willing to change params"));
			break;
		case SPP_EVENT_TX_DATA_COMPLETE:
		{
			Report(( "SPP_APP : SPP_EVENT_TX_DATA_COMPLETE"));


			break;
		
        	} /* End case SPP_EVENT_TX_DATA_COMPLETE */
	case SPP_EVENT_RX_DATA_IND: 
	{
		BtStatus status;
            	U16 size;
		U8 data[12];
		Report(( "SPP_APP : SPP_EVENT_RX_DATA_IND"));
		Report(( "SPP_APP : %d bytes received from client",
              event->parms->p.rxDataInfo.rxDataLen));

		commonPort->state = SPPAPP_PORT_STATE_RECEIVING;
            
		do
		{
              	if(commonPort->btlPortSettings.dataPathTypes & SPP_PORT_DATA_PATH_RX_ASYNC)
                	{
				status = BTL_SPP_ReadAsync(event->portContext, 
								           commonPort->fileStruct.buffRx, 
								           sizeof(commonPort->fileStruct.buffRx), 
								           &commonPort->fileStruct.bytesNumRead);

				commonPort->fileStruct.ptrRx = (char *)commonPort->fileStruct.buffRx;
				strcpy((char *)data, commonPort->fileStruct.buffRx);
				if (BT_STATUS_SUCCESS != status)
				{
				    Report(("SPP_APP : BTL_SPP_ReadAsync() failure, status %s",
                                pBT_Status(status)));
				 }
			    }
                else if (commonPort->btlPortSettings.dataPathTypes & SPP_PORT_DATA_PATH_RX_SYNC)
                {
			commonPort->fileStruct.ptrRx = (char *)event->parms->p.rxDataInfo.rxData;
                     commonPort->fileStruct.bytesNumRead = event->parms->p.rxDataInfo.rxDataLen;
			strncpy((char *)data, commonPort->fileStruct.ptrRx,commonPort->fileStruct.bytesNumRead);
			*(data+commonPort->fileStruct.bytesNumRead) = '\0';
		  }
			
		  if (commonPort->fileStruct.bytesNumRead > 0)
		  {
		  	Report(("SPP_APP : The Data recieved : %s",data));		
					
		}
		else{
		}  
	 } while ((commonPort->btlPortSettings.dataPathTypes & SPP_PORT_DATA_PATH_RX_ASYNC) &&
                     (commonPort->fileStruct.bytesNumRead > 0));
            break;
        } /* End case SPP_EVENT_RX_DATA_IND */
	case SPP_EVENT_DISABLED:
		commonPort->state = SPPAPP_PORT_STATE_CREATED;
		if (TRUE == sppAppExiting)
           	 {
			    if (TRUE == SPPA_Deinit())
			    {
        		        sppAppExiting = FALSE;
			    }
            	}
		break;
	default:
		break;
	}
}
 
/*---------------------------------------------------------------------------
 *            SppAppSendFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  
 *
 * Return:    void
 *
 */
static void SppAppFlushRelevantBuffers(SppAppCommonPort *commonPort)
{
    BtStatus status;

    if (SPPAPP_PORT_STATE_SENDIND == commonPort->state)
    {
        status = BTL_SPP_Flush(commonPort->portContext, SPP_FLUSH_TYPE_TX);

        if (BT_STATUS_SUCCESS != status)
        {
            Report(( "SPP_APP : Flush of Tx data failed, status %s",
                      pBT_Status(status)));
        }
    }
    else if ((SPPAPP_PORT_STATE_RECEIVING == commonPort->state) &&
             (commonPort->btlPortSettings.dataPathTypes & SPP_PORT_DATA_PATH_RX_ASYNC))
    {
        status = BTL_SPP_Flush(commonPort->portContext, SPP_FLUSH_TYPE_RX);

        if (BT_STATUS_SUCCESS != status)
        {
            Report(("SPP_APP : Flush of Rx Async data failed, status %s",
                      pBT_Status(status)));
        }
    }
}

 
/*
 *---------------------------------------------------------------------------
 *            General Purpose Debug functions
 *---------------------------------------------------------------------------
 */
const char *pSPP_Event(SppEvent sppEvent)
{
    switch (sppEvent) {
    case SPP_EVENT_OPEN_IND:
        return "SPP_EVENT_OPEN_IND";
    case SPP_EVENT_OPEN:
        return "SPP_EVENT_OPEN";
    case SPP_EVENT_CLOSE_IND:
        return "SPP_EVENT_CLOSE_IND";
    case SPP_EVENT_CLOSED:
        return "SPP_EVENT_CLOSED";
    case SPP_EVENT_RX_DATA_IND:
        return "SPP_EVENT_RX_DATA_IND";
    //case SPP_EVENT_TX_BUF_IND:
       // return "SPP_EVENT_TX_BUF_IND";
#if RF_SEND_TEST == XA_ENABLED
    case SPP_EVENT_TEST_CNF:
        return "SPP_EVENT_TEST_CNF";
#endif /* RF_SEND_TEST enabled */
    case SPP_EVENT_PORT_NEG_IND:
        return "SPP_EVENT_PORT_NEG_IND";
    case SPP_EVENT_PORT_NEG_CNF:
        return "SPP_EVENT_PORT_NEG_CNF";
    case SPP_EVENT_PORT_STATUS_IND:
        return "SPP_EVENT_PORT_STATUS_IND";
    case SPP_EVENT_PORT_STATUS_CNF:
        return "SPP_EVENT_PORT_STATUS_CNF";
    case SPP_EVENT_MODEM_STATUS_IND:
        return "SPP_EVENT_MODEM_STATUS_IND";
    case SPP_EVENT_MODEM_STATUS_CNF:
        return "SPP_EVENT_MODEM_STATUS_CNF";
    case SPP_EVENT_LINE_STATUS_IND:
        return "SPP_EVENT_LINE_STATUS_IND";
    case SPP_EVENT_LINE_STATUS_CNF:
        return "SPP_EVENT_LINE_STATUS_CNF";
    case SPP_EVENT_FLOW_OFF_IND:
        return "SPP_EVENT_FLOW_OFF_IND";
    case SPP_EVENT_FLOW_ON_IND:
        return "SPP_EVENT_FLOW_ON_IND";
    case SPP_EVENT_SDP_QUERY_FAILED:
        return "SPP_EVENT_SDP_QUERY_FAILED";
    }
    return "UNKNOWN";
}


const char *pSPP_Settings(U8 settingsType, U8 settingsValue)
{
	const char * baudDescriptors[] = {"2400","4800","7200","9600","19200","38400","57600","115200","230400"};
	const char * dataFormatDescriptors[] = {"5 Data bits","6 Data bits","7 Data bits","8 Data bits"};
	const char * stopBitsDescriptors[] = {"1 bit","1 bit 1 5"};
	const char * parityDescriptors[] = {"NONE","ON"};
	const char * parityTypeDescriptors[] = {"ODD","EVEN","MARK","TYPE_SPACE"};
	
		
	switch (settingsType) {
    
		case SPP_APP_SETTINGS_BAUDRATE:
			return baudDescriptors[settingsValue];
		
		case SPP_APP_SETTINGS_DATA_BITS:
			return dataFormatDescriptors[(settingsValue & SPP_COM_SETTING_MASK_DATA_BITS) >> 0];
		
		case SPP_APP_SETTINGS_DATA_STOP_BITS:
			return stopBitsDescriptors[(settingsValue & SPP_COM_SETTING_MASK_STOP_BITS) >> 2];
		
		case SPP_APP_SETTINGS_DATA_PARITY_BITS:
			return parityDescriptors[(settingsValue & SPP_COM_SETTING_MASK_PARITY) >> 3];
		
		case SPP_APP_SETTINGS_DATA_PARITY_TYPE_BITS:
			return parityTypeDescriptors[(settingsValue & SPP_COM_SETTING_MASK_PARITY_TYPE) >> 4];
		
    }
    return "SPP_APP : NO SUCH SETTING TYPE";

}

/*---------------------------------------------------------------------------
 *            GetStateStr()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Changes the given string according to the given integer state
 */

static void GetStateStr(U8 state, char * stateStr)
{
	switch (state)
	{
	
		case SPPAPP_PORT_STATE_NONE:
			OS_StrCpy(stateStr, "none");
			break;
		case SPPAPP_PORT_STATE_CREATED:
			OS_StrCpy(stateStr, "Created");
			break;
		case SPPAPP_PORT_STATE_DISCONNECTED:
			OS_StrCpy(stateStr, "Enabled");
			break;
		case SPPAPP_PORT_STATE_CONNECTED:
			OS_StrCpy(stateStr, "Connected");
			break;
		case SPPAPP_PORT_STATE_DISABLING:
			OS_StrCpy(stateStr, "Disabling");
			break;
		case SPPAPP_PORT_STATE_SENDIND:
			OS_StrCpy(stateStr, "Sending");
			break;
		case SPPAPP_PORT_STATE_RECEIVING:
			OS_StrCpy(stateStr, "Receiving");
			break;
		default: 
			OS_StrCpy(stateStr, "Wrong State");
			break;

	}
}

#if 0
/*---------------------------------------------------------------------------
 * AppReport()
 *
 *  Prints info messages in the application's window.
 */
static void AppReport(SppAppCommonPort *commonPort, char *format,...)
{
    char            buffer[200]; /* Output Buffer */
    int             index;
    static SppAppCommonPort *prevCommonPort = NULL;

    /* Check whether 'commonPort' parameter is relevant and whether it has been
     * changed since previous message in order to display port's type and name */
    if ((NULL != commonPort ) && (commonPort != prevCommonPort))
    {
        /* Save 'commonPort' value */
        prevCommonPort = commonPort;

        if (SPP_CLIENT_PORT == commonPort->btlPortSettings.portType)
        {
            strcpy(buffer, "--------------------[Client ");
        }
        else
        {
            strcpy(buffer, "--------------------[Server ");
        }

        switch(commonPort->portId)
        {
            case 0:
                strcat(buffer, "Dial-up]--------------------");
                break;

            case 1:
                strcat(buffer, "Fax]--------------------");
                break;

            case 2:
                strcat(buffer, "Serial1]--------------------");
                break;

            case 3:
                strcat(buffer, "Serial2]--------------------");
                break;

            default:
                break;
        }

    }

    /* Skim off accidental newlines characters */
    if (buffer[index-1] == '\n')
    {
        if (buffer[index-2] == '.')
        {
            buffer[index-1] = 0;
            index--;
        } else buffer[index-1] = '.';
    }

     /* Now write the text out to the listbox. */
   Report((buffer));
}
#endif

#else /* BTL_CONFIG_SPP == BTL_CONFIG_ENABLED */


void SPPA_Init(void)
{
	
	Report(("SPPA_Init -BTL_CONFIG_SPP is disabled"));
	

}


BOOL SPPA_Deinit(void)
{
    
	Report(("SPPA_Deinit -BTL_CONFIG_SPP is disabled"));
	return TRUE;
}



#endif /* BTL_CONFIG_SPP == BTL_CONFIG_ENABLED */



