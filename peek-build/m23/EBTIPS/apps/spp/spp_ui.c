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
*   FILE NAME:      spp_ui.c
*
*   DESCRIPTION:    This file contains the implementation of the SPP sample. 
*					
*
*   AUTHOR:        
*
\*******************************************************************************/

#include "btl_config.h"
#include "osapi.h"

#if BTL_CONFIG_SPP == BTL_CONFIG_ENABLED


#include "btl_common.h"
#include "debug.h"
#include "../app_main.h"
#include "btl_spp.h"
#include "btl_config.h"
#include "lineparser.h"


/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/



/*******************************************************************************
 *
 * Macro definitions
 *
 ******************************************************************************/
#define SPPA_FILE_NAME_MAX_LEN	255
#define SPPA_BD_ADDR_LEN	17

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/


 /*-------------------------------------------------------------------------------
 * OppsaUiCommandId type
 *
 *     Defines the OPPS user commands
 */
typedef U8 SppaUiCommandId;

#define SPPA_UI_ENABLE_CLIENT               		  0
#define SPPA_UI_DISABLE_CLIENT             		  1
#define SPPA_UI_ENABLE_SERVER             		  2
#define SPPA_UI_DISABLE_SERVER                     	  3
#define SPPA_UI_CONNECT_CLIENT                       4
#define SPPA_UI_DISCONNECT_CLIENT                 5
#define SPPA_UI_DISCONNECT_SERVER                 6       
//#define SPPA_UI_SEND_FILE_S                 7    
#define SPPA_UI_WRITE_DATA_S        			8
#define SPPA_UI_WRITE_DATA_C                 	9
#define SPPA_UI_GET_SECURITY_C			10
#define SPPA_UI_GET_SECURITY_S			11
#define SPPA_UI_SHOW_LOCAL_SETTINGS 		12
#define SPPA_UI_SLOW_BAUD_S				13
#define SPPA_UI_SLOW_BAUD_C				14
#define SPPA_UI_SEND_FILE_S				15
#define SPPA_UI_SEND_FILE_C				16
#define SPPA_UI_CLOSE_FILE_S				17
#define SPPA_UI_CLOSE_FILE_C				18
#define SPPA_UI_FAST_BOUD_RATE_S			19
#define SPPA_UI_FAST_BOUD_RATE_C			20
#define SPPA_UI_SET_SECURITY_C				21
#define SPPA_UI_SET_SECURITY_S				22
#define SPPA_UI_EXIT						23


typedef struct _SppaUiCommand
{
	char 	            cmdName[LINE_PARSER_MAX_STR_LEN];
	SppaUiCommandId    cmdId;
} SppaUiCommand;


/********************************************************************************
 *
 * Function prototypes
 *
 *******************************************************************************/

void SPPA_UI_ProcessUserAction(U8 *msg);
BOOL lineParserStatusSucces(LINE_PARSER_STATUS stat);


/********************************************************************************
 *
 * External functions
 *
 *******************************************************************************/
extern  void SPPA_Init(void);
extern  BOOL SPPA_Deinit(void);
extern  void SPPA_EnableClient(BtlSppServiceType index ,SppPortDataPathType txRxSettings,U8 *serviceName);
extern  void SPPA_DisableClient(BtlSppServiceType index);
extern  void SPPA_EnableServer(BtlSppServiceType index,SppPortDataPathType txRxSettings,U8 *serviceName);
extern  void SPPA_DisableServer(BtlSppServiceType index);
extern  void SPPA_Connect(BD_ADDR *bd_addr,BtlSppServiceType index);
extern  void SPPA_WriteDataS(BtlSppServiceType index);
extern  void SPPA_WriteDataC(BtlSppServiceType index);
//extern  void SPPA_SendFileS(BtlSppServiceType index,U8 *fileName);		
//extern  void SPPA_SendFileC(BtlSppServiceType index,U8 *fileName);
//extern  void SPPA_CloseFileS(BtlSppServiceType index);
//extern  void SPPA_CloseFileC(BtlSppServiceType index);
extern  void SPPA_DisconnectS(BtlSppServiceType index);
extern  void SPPA_DisconnectC(BtlSppServiceType index);
//extern  void SPPA_SetSecurityC(BtlSppServiceType index ,const BtSecurityLevel *securityLevel);
//extern  void SPPA_SetSecurityS(BtlSppServiceType index ,const BtSecurityLevel *securityLevel);
//extern  void SPPA_GetSecurityC(BtlSppServiceType index);
//extern  void SPPA_GetSecurityS(BtlSppServiceType index);
//extern  void SPPA_ShowLocalSettings(BtlSppServiceType index);
//extern  void SPPA_FastBaudRateC(BtlSppServiceType index);
//extern  void SPPA_FastBaudRateS(BtlSppServiceType index);
//extern  void SPPA_SlowBaudC(BtlSppServiceType index);
//extern  void SPPA_SlowBaudS(BtlSppServiceType index);
extern  void SPPA_Exit();


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/


/* OPPS commands array */
static const SppaUiCommand sppaUiCommands[] =  
{  
   	{"enablec",	        	SPPA_UI_ENABLE_CLIENT}, 		       
	{"disablec",          		SPPA_UI_DISABLE_CLIENT}, 			
	{"enables",          		SPPA_UI_ENABLE_SERVER}, 		     	
	{"disables",          		SPPA_UI_DISABLE_SERVER},   
	{"connectc",         		SPPA_UI_CONNECT_CLIENT}, 		    
	{"disconnectc",         	SPPA_UI_DISCONNECT_CLIENT},		     	 
	{"disconnects",      		SPPA_UI_DISCONNECT_SERVER},		     	 
	{"writes",          		SPPA_UI_WRITE_DATA_S},
    	{"writec",          		SPPA_UI_WRITE_DATA_C},
    	{"getsecurityc",           	SPPA_UI_GET_SECURITY_C},
    	{"getsecuritys",           	SPPA_UI_GET_SECURITY_S},
    	{"showsettings",           SPPA_UI_SHOW_LOCAL_SETTINGS},
    	{"slowbauds",         	SPPA_UI_SLOW_BAUD_S},
    	{"slowbaudc",         	SPPA_UI_SLOW_BAUD_C},
    	{"sendfiles",          		SPPA_UI_SEND_FILE_S},
    	{"sendfilec",          		SPPA_UI_SEND_FILE_C},
    	{"closefiles",          	SPPA_UI_CLOSE_FILE_S},
    	{"closefilec",          	SPPA_UI_CLOSE_FILE_C},
   	{"fastbauds",          	SPPA_UI_FAST_BOUD_RATE_S},
    	{"fastbaudc",          	SPPA_UI_FAST_BOUD_RATE_C}, 
    	{"setsecurityc",          	SPPA_UI_SET_SECURITY_C},
    	{"setsecuritys",          	SPPA_UI_SET_SECURITY_S},
    	{"exit",          			SPPA_UI_EXIT}
};

#define SPPA_COMMANDS_NUM (sizeof(sppaUiCommands) / sizeof(SppaUiCommand))



/*******************************************************************************
 *
 * Internal Function prototypes
 *
 ******************************************************************************/



/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/


/*------------------------------------------------------------------------------
 *            OPPSA_UI_ProcessUserAction
 *------------------------------------------------------------------------------
 *
 * Synopsis:  Processes primitives for the OPP server application
 *
 * Return:    void
 *
 */
void SPPA_UI_ProcessUserAction(U8 *msg)
{
	U8 portType = 0;
	U8 idx;
	U8 command[LINE_PARSER_MAX_STR_LEN];
    	SppaUiCommandId cmdId = 0;
       U8	BdArray[SPPA_BD_ADDR_LEN];
	BD_ADDR  bdAddr;  	
	U8 txRxSettings = 1;	//defualt Tx_s Rx_A
	U8 serviceName[SPP_SERVICE_NAME_MAX_SIZE] ;
	U8 fullPathFileName[SPPA_FILE_NAME_MAX_LEN] ;
	U8 secLevel = 0;
	
	if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(command, LINE_PARSER_MAX_STR_LEN))
	{
		Report(("SPPA_UI: Wrong SPP command. Can't parse line"));
		return;
	}

    /* Look for the correct function */
	for(idx = 0; idx < SPPA_COMMANDS_NUM; idx++)
	{
		if (OS_StrCmp((const char *)command, sppaUiCommands[idx].cmdName) == 0)
		{
			cmdId = sppaUiCommands[idx].cmdId;
			break;
		}
	}
	if(cmdId == SPPA_UI_EXIT){
		SPPA_Exit();
		return;
	}
    if (idx == SPPA_COMMANDS_NUM)
    {
        Report(("SPPA_UI: [command] not found"));	
	    return;
    }

	Report(("SPPA_UI: Received action - \"%s\".", sppaUiCommands[idx].cmdName));
	if (!lineParserStatusSucces(LINE_PARSER_GetNextU8(&portType, FALSE)))
	{   
		return;
	}
	
    	switch (cmdId)    
    	{

        case SPPA_UI_ENABLE_CLIENT:
		if (!lineParserStatusSucces(LINE_PARSER_GetNextU8(&txRxSettings, FALSE)))
		{   
			//return;
		}
		if (!lineParserStatusSucces( LINE_PARSER_GetNextStr(serviceName, SPP_SERVICE_NAME_MAX_SIZE-1)) )
	        {   
		        Report(("SPPA_UI_ENABLE_CLIENT: no service name given "));	
		        //break;
	        }
		
            	SPPA_EnableClient(portType, txRxSettings, serviceName);
		break;

	case SPPA_UI_DISABLE_CLIENT:
		
            	SPPA_DisableClient(portType);	
		break;

	case SPPA_UI_ENABLE_SERVER:
		if (!lineParserStatusSucces(LINE_PARSER_GetNextU8(&txRxSettings, FALSE) ))
		{   
			Report(("Please select Client Or Server "));	
			//return;
		}
		if (!lineParserStatusSucces(LINE_PARSER_GetNextStr(serviceName, SPP_SERVICE_NAME_MAX_SIZE) ))
	        {   
		        Report(("SPPA_UI_ENABLE_CLIENT: no service name given "));	
		        //break;
	        }
		
            	SPPA_EnableServer(portType, txRxSettings, serviceName);
		break;
       case SPPA_UI_DISABLE_SERVER:
		
            	SPPA_DisableServer(portType);	
		break;	
        case SPPA_UI_CONNECT_CLIENT:

		if(!lineParserStatusSucces(LINE_PARSER_GetNextStr(BdArray, SPPA_BD_ADDR_LEN)))
		{
			Report(("SPPA_UI_CONNECT_CLIENT: bdAddr"));
			break;
		}
		bdAddr = bdaddr_aton((const char *)(BdArray));
		SPPA_Connect(&bdAddr,portType);
            break;

        case SPPA_UI_DISCONNECT_CLIENT:
		
            	SPPA_DisconnectC(portType);		
		break;

        case SPPA_UI_DISCONNECT_SERVER:
          
            	SPPA_DisconnectS(portType);	
		break;

        case SPPA_UI_WRITE_DATA_S:
           	
          	SPPA_WriteDataS(portType);
            break;

        case SPPA_UI_WRITE_DATA_C:
		
          SPPA_WriteDataC(portType);
           
          break;
        	
        case SPPA_UI_GET_SECURITY_C:
		
          //SPPA_GetSecurityC(portType);	

          	break;
        case SPPA_UI_GET_SECURITY_S:
          	//SPPA_GetSecurityS(portType);	
		break;
	 case SPPA_UI_SHOW_LOCAL_SETTINGS:
		//SPPA_ShowLocalSettings(portType);
		break;
	 case SPPA_UI_SLOW_BAUD_C:
		//SPPA_SlowBaudC(portType);
		break;
	case SPPA_UI_SLOW_BAUD_S:
		//SPPA_SlowBaudS(portType);
		break;		
	 case SPPA_UI_FAST_BOUD_RATE_C:
		//SPPA_FastBaudRateC(portType);
		break;
	case SPPA_UI_FAST_BOUD_RATE_S:
		//SPPA_FastBaudRateS(portType);
		break;	
	case SPPA_UI_SEND_FILE_C:
		
		if (!lineParserStatusSucces( LINE_PARSER_GetNextStr(fullPathFileName, SPPA_FILE_NAME_MAX_LEN)) )
	        {   
		        Report(("SPPA_UI_SEND_FILE_C: no file path name given "));	
		        //break;
	        }
		//SPPA_SendFileC(portType, "/SPP/test.txt"/*fullPathFileName*/);
		break;
	case SPPA_UI_SEND_FILE_S:
		if (!lineParserStatusSucces( LINE_PARSER_GetNextStr(fullPathFileName, SPPA_FILE_NAME_MAX_LEN) ))
	        {   
		        Report(("SPPA_UI_SEND_FILE_S: no file path name given "));	
		        //break;
	        }
		//SPPA_SendFileS(portType, "/SPP/test.txt"/*fullPathFileName*/);
		break;
	case SPPA_UI_SET_SECURITY_C:
		if (!lineParserStatusSucces(LINE_PARSER_GetNextU8(&secLevel, FALSE) ))
		{   
			Report(("Security level was not given "));	
			return;
		}
		//SPPA_SetSecurityC(portType, &secLevel);
		break;
	case SPPA_UI_SET_SECURITY_S:
		if (!lineParserStatusSucces( LINE_PARSER_GetNextU8(&secLevel, FALSE)) )
		{   
			Report(("Security level was not given "));			
			return;
		}
		//SPPA_SetSecurityS(portType, &secLevel);
		break;	
	case SPPA_UI_CLOSE_FILE_S:
		//SPPA_CloseFileS(portType);
		break;
	case SPPA_UI_CLOSE_FILE_C:
		//SPPA_CloseFileC(portType);
		break;	
      default:
            Report(("SPPA_UI: Mistake in commands table"));
            break;
    } 
}

BOOL lineParserStatusSucces(LINE_PARSER_STATUS stat)
{
	switch(stat)
	{
	case LINE_PARSER_STATUS_SUCCESS:
		return TRUE;
	case LINE_PARSER_STATUS_FAILED:
		Report(("lineParserStatusSucces : FAILED"));
		return FALSE;
	case LINE_PARSER_STATUS_ARGUMENT_TOO_LONG:
		Report(("lineParserStatusSucces : ARGUMENT_TOO_LONG"));
		return FALSE;	
	case LINE_PARSER_STATUS_NO_MORE_ARGUMENTS:
		Report(("lineParserStatusSucces : NO_MORE_ARGUMENTS"));
		return FALSE;
	default:
		Report(("lineParserStatusSucces : no such status"));
		return FALSE;
	}
}

#else /* BTL_CONFIG_SPP == BTL_CONFIG_ENABLED */

void SPPA_UI_ProcessUserAction(U8 *msg)
{
	msg=msg;
	Report(("SPP_APP is disabled via BTL_CONFIG."));
}





#endif /* BTL_CONFIG_SPP == BTL_CONFIG_ENABLED */

