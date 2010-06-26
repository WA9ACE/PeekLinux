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
*   FILE NAME:      bmg_app.c
*
*   DESCRIPTION:	This file contains the implementation of the BMG application primitives (user actions).
*
*   AUTHOR:         Gili Friedman + Adi Ben Meir.
*
\*******************************************************************************/


/* GPF Includes */
#include "typedefs.h"
#include "vsi.h"
#include "../p_btt.h"

/* BTIPS Includes */
#include "me.h"
#include "osapi.h"
#include "../app_main.h"
#include "btl_common.h"
#include "bthal_btdrv.h"
#include "bthal_pm.h"
#include "bthal_os.h"
#include "bthal_common.h"
#include "bttypes.h"
#include "goep.h"
#include "bpp.h"
#include "Debug.h"

/* BTL includes */
#include "btl_bppsnd.h"
#include "btl_bmg.h"

/*Application includes*/
#include "lineparser.h"

/*BAT includes*/
#include "bat.h"

BOOL APP_BMG_SendPin (void);
BOOL APP_BMG_Search (void);
BOOL APP_BMG_StopSearch (void);
BOOL APP_BMG_SetLocalName (void);
BOOL APP_BMG_SetSecurityMode (void);
BOOL APP_BMG_AuthorizeDeviceReply (void);
BOOL APP_BMG_GetSecurityMode (void);
BOOL APP_BMG_CleanDeviceDB(void);
BOOL  APP_BMG_EnableTestMode(void);
BOOL APP_BMG_GetRemoteDeviceCod(void);
BOOL APP_BMG_GetDeviceRecord(void);
BOOL APP_BMG_AddDeviceRecord(void);
BOOL APP_BMG_GetRemoteDeviceName(void);

typedef BOOL (*FuncType)(void);

typedef struct
{
	char 		funcName[LINE_PARSER_MAX_STR_LEN];
	FuncType    funcPtr;
} _bmgCommands;


#define BMG_NUM_OF_BAT_INSTANCES		1

#define BMG_NUM_OF_BAT_CLIENTS		1

#define BMG_BAT_INST_BUF_SIZE		(BAT_INSTANCE_HEADER_SIZE * BMG_NUM_OF_BAT_INSTANCES)

#define BMG_BAT_CLIENTS_BUF_SIZE    (BAT_INSTANCE_SIZE + BAT_CLIENT_SIZE * BMG_NUM_OF_BAT_CLIENTS)

typedef struct _bmgBatAtCommand
{
	union
	{
		T_BAT_no_parameter noparam;
		T_BAT_cmd_at_d atd;
		T_BAT_cmd_set_plus_vts vts;		
		T_BAT_cmd_set_percent_chld chld;
		T_BAT_cmd_set_plus_ccwa ccwa;
		T_BAT_cmd_set_plus_cops cops;	
		T_BAT_cmd_set_plus_cpbs cpbs;
		T_BAT_cmd_set_plus_cpbr cpbr;
		T_BAT_cmd_set_plus_cpbf cpbf;
		T_BAT_cmd_set_plus_cscs cscs;
	}paramMem;
		
	T_BAT_cmd_send cmd;

}BmgBatAtCommands;

typedef struct _bmgBatModule
{
	/* Memory needed for BAT Library instance */
	unsigned int batInstBuf[(BMG_BAT_INST_BUF_SIZE + 3) / 4];

	/* Memory needed for BAT Clients */
	unsigned int batClientBuf[(BMG_BAT_CLIENTS_BUF_SIZE + 3) / 4];

	T_BAT_instance instance;

	T_BAT_config config;

	T_BAT_client client;

	BmgBatAtCommands atCmd;

}BmgBatModule;

static BmgBatModule batModuleData = {0};

static BOOL testModeEnabled = FALSE;

/************************************************************************************
 *
 * Internal functions declarations:
 *
 ***********************************************************************************/

static BtStatus AppBmgInitBat(void);

static BtStatus AppBmgDeinitBat(void);

static void AppBmgClientSigCb(T_BAT_client client, T_BAT_signal sig);

static const char* AppBmgBatRetToString(T_BAT_return retVal);

static void AppBmgSendCommand(void);

static int AppBmgClientRspCb(T_BAT_client client, T_BAT_cmd_response *rsp);

static void AppBmgNewBatInstanceSigCb(T_BAT_signal sig);

static void AppBmgCloseGSMRF(void);

static void AppBmgEnableTestMode(void);

static int AppBmgHandleOkRes();


/* BMG commands array */
static const  _bmgCommands bmgCommands[] =  { {"sendpin",	    			APP_BMG_SendPin}, 		       
												{"search",      		APP_BMG_Search}, 			
												{"stopsearch",       	APP_BMG_StopSearch},
												{"setlocalname",       	APP_BMG_SetLocalName},
												{"setsecmode",       	APP_BMG_SetSecurityMode},
												{"authorize",       		APP_BMG_AuthorizeDeviceReply},
												{"getsecmode",       	APP_BMG_GetSecurityMode},
												{"cleandevicedb", 		APP_BMG_CleanDeviceDB},
												{"enabletestmode", 	APP_BMG_EnableTestMode},
												{"getRemDevCod", 		APP_BMG_GetRemoteDeviceCod},
												{"getDeviceRecord",	APP_BMG_GetDeviceRecord},
												{"addDeviceRecord",	APP_BMG_AddDeviceRecord},
												{"getRemoteName",	APP_BMG_GetRemoteDeviceName}
											};

static const U32 bmgNumOfCommands = (sizeof(bmgCommands) / sizeof(_bmgCommands));

#define BTL_BMG_MAX_PIN_LEN			16
#define BTL_BMG_SEARCH_ALL				0
#define BTL_BMG_SEARCH_AUDIO_MAJOR	1

/* Default local device name */
#define PME_APP_NAME						"Locosto Phone"



/* Internal functions prototypes */
void BmgaCallback (const BtlBmgEvent *);

static BtlBmgContext *bmgContext;

/*******************************************************************************\
*  FUNCTION:	BMGA_ProcessUserAction
*                                                                         
*  PARAMETERS:	msg - buffer holding primitive string
*                                                                         
*  DESCRIPTION:	Processes primitives for BMG Sample APP.
*                                                                         
*  RETURNS:		None.
\*******************************************************************************/
void BMGA_ProcessUserAction(U8 *msg)
{
	int i;
	char command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS lineStatus;

	lineStatus = LINE_PARSER_GetNextStr((U8 *)(command), LINE_PARSER_MAX_STR_LEN);
	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("Wrong BMG command. Can't parse line"));
		return;
	}

	/* Look for the correct function */
	for(i = 0; i < bmgNumOfCommands; i++)
	{
		if (OS_StriCmp(command, bmgCommands[i].funcName) == 0)
		{
			bmgCommands[i].funcPtr();
			return;
		}
	}
	Report(("Wrong BMG command"));
}



/*---------------------------------------------------------------------------
 *            APP_BMG_SendPin
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send PIN code.		
 *
 * Return:    TRUE if send PIN code is successful, FALSE otherwise.
 *
 */
BOOL APP_BMG_SendPin (void)
{
	U8 length,type,BD_array[BDADDR_NTOA_SIZE];
	BtStatus status = BT_STATUS_FAILED;
	BD_ADDR BdAddr;
	U8 PinArray [BTL_BMG_MAX_PIN_LEN + 1];

	Report(("APP_BMG_SendPin"));
	LINE_PARSER_GetNextU8( &type, FALSE);
	LINE_PARSER_GetNextStr(BD_array, 17);
	BdAddr = bdaddr_aton((const char *)(BD_array));
	LINE_PARSER_GetNextStr(PinArray, BTL_BMG_MAX_PIN_LEN);
	length = (U8)(OS_StrLen((const char *)(PinArray)));	
	status = BTL_BMG_PinReply(bmgContext, &BdAddr, (const U8 *)(PinArray), length, type);
	Report(("BMG : PIN code is: %s", PinArray));
	Report(("BMG :BTL_BMG_PinReply returned status: %s", pBT_Status(status)));
	if (status == BT_STATUS_SUCCESS)
	{
		return TRUE;
	}
		return FALSE;

}


BOOL AppBmgGetCodMajorValue(U8* CODMajorStr, U32 *CODMajor)
{
	BOOL	result;
	
	LINE_PARSER_ToLower(CODMajorStr);

	*CODMajor = COD_MAJOR_MISCELLANEOUS;
	result = TRUE;
	
	if (strcmp((const char*)CODMajorStr, "comp") == 0)
	{
		*CODMajor = COD_MAJOR_COMPUTER;
	}
	else if (strcmp((const char*)CODMajorStr, "phone") == 0)
	{
		*CODMajor = COD_MAJOR_PHONE;
	}
	else if (strcmp((const char*)CODMajorStr, "lan") == 0)
	{
		*CODMajor = COD_MAJOR_LAN_ACCESS_POINT;
	}
	else if (strcmp((const char*)CODMajorStr, "audio") == 0)
	{
		*CODMajor = COD_MAJOR_AUDIO;
	}
	else if (strcmp((const char*)CODMajorStr, "periph") == 0)
	{
		*CODMajor = COD_MAJOR_PERIPHERAL;
	}
	else if (strcmp((const char*)CODMajorStr, "imaging") == 0)
	{
		*CODMajor = COD_MAJOR_IMAGING;
	}
	else if (strcmp((const char*)CODMajorStr, "none") == 0)
	{
		*CODMajor = COD_MAJOR_MISCELLANEOUS;
	}
	else
	{
		result = FALSE;
	}

	return result;
}

BOOL AppBmgGetLAPValue(U8* lapStr, U32 *lap)
{
	BOOL	result;
	
	LINE_PARSER_ToLower(lapStr);

	result = TRUE;
	
	if (strcmp((const char*)lapStr, "giac") == 0)
	{
		*lap = BT_IAC_GIAC;
	}
	if (strcmp((const char*)lapStr, "liac") == 0)
	{
		*lap = BT_IAC_LIAC;
	}
	else
	{
		result = FALSE;
	}

	return result;
}

/*---------------------------------------------------------------------------
 *            APP_BMG_Search
 *---------------------------------------------------------------------------
 *
 * Synopsis: Starts search process.	
 *
 * Return:    TRUE if search process is successfull, FALSE otherwise.
 *
 */
BOOL APP_BMG_Search (void)
{
	BtStatus status = BT_STATUS_FAILED;
	BtlBmgCodFilter CODFilter;
	BtlBmgCodFilter		*CODFilterPtr = 0;
	U8					lapStr[11];
	BtIac 				lap;
	BOOL				performNameRequest = FALSE;
	U8					maxResp = 0;
	LINE_PARSER_STATUS	lineParserStatus;
	U8					CODMajorStr[21];
	U32					CODMajor;

	Report(("APP_BMG_Search"));
	
	/* 
		Usage: -BMG Search [<max Resp>], [<Name Request>], [<Major>], [Services], [<LAP>]

		Default Values: 
		max Resp: 		0 (no limit)
		Name Request		true (perform name requst)
		COD Major:		None (Any)
		COD Service:		None (Any)			
		LAP: 			GIAC
		*/

	/* Max Responses */
	if (LINE_PARSER_AreThereMoreArgs() == TRUE)
	{
		lineParserStatus = LINE_PARSER_GetNextU8(&maxResp, FALSE);

		if (lineParserStatus != LINE_PARSER_STATUS_SUCCESS)
		{
			Report(("BMG :BTL_BMG_SearchByCod Invalid max Responses (%d)",status));
			return FALSE;
		}
	}
	else
	{
		maxResp = 0;
	}

	/* Perform Name Request */
	if (LINE_PARSER_AreThereMoreArgs() == TRUE)
	{
		lineParserStatus = LINE_PARSER_GetNextBool(&performNameRequest);

		if (lineParserStatus != LINE_PARSER_STATUS_SUCCESS)
		{
			Report(("BMG :BTL_BMG_SearchByCod Invalid Name Request Flag (%d)",status));
			return FALSE;
		}
	}
	else
	{
		performNameRequest = TRUE;
	}

	/* Assume default value would be used (no filter) */
	CODFilter.codFilterMask = BTL_BMG_SEARCH_COD_FILTER_NONE;
	
	/* COD Major */
	if (LINE_PARSER_AreThereMoreArgs() == TRUE)
	{
		lineParserStatus = LINE_PARSER_GetNextStr(CODMajorStr, 20);

		if (lineParserStatus != LINE_PARSER_STATUS_SUCCESS)
		{
			Report(("BMG :BTL_BMG_SearchByCod Invalid Major COD (%d)",status));
			return FALSE;
		}

		if (AppBmgGetCodMajorValue(CODMajorStr, &CODFilter.majorDeviceClass) == FALSE)
		{
			Report(("BMG :BTL_BMG_SearchByCod Invalid Major COD (%d)",status));
			return FALSE;
		}

		if (CODFilter.majorDeviceClass != COD_MAJOR_MISCELLANEOUS)
		{
			CODFilter.codFilterMask |= BTL_BMG_SEARCH_COD_FILTER_MAJOR_DEV_CLS;
		}
	}

	/* COD Service Classes */
	if (LINE_PARSER_AreThereMoreArgs() == TRUE)
	{
		lineParserStatus = LINE_PARSER_GetNextU32(&CODFilter.serviceClass, TRUE);

		if (lineParserStatus != LINE_PARSER_STATUS_SUCCESS)
		{
			Report(("BMG :BTL_BMG_SearchByCod Invalid COD Service Classes Value (%d)",status));
			return FALSE;
		}
		
		CODFilter.codFilterMask |= BTL_BMG_SEARCH_COD_FILTER_SERVICE_CLS;
	}

	if (CODFilter.codFilterMask != BTL_BMG_SEARCH_COD_FILTER_NONE)
	{
		CODFilterPtr = &CODFilter;
	}
	else
	{
		CODFilterPtr = 0;
	}
	
	/* LAP */
	if (LINE_PARSER_AreThereMoreArgs() == TRUE)
	{
		lineParserStatus = LINE_PARSER_GetNextStr(lapStr, 10);

		if (lineParserStatus != LINE_PARSER_STATUS_SUCCESS)
		{
			Report(("BMG :BTL_BMG_SearchByCod Invalid LAP (%d)",status));
			return FALSE;
		}

		if (AppBmgGetLAPValue(lapStr, &lap) == FALSE)
	{
			Report(("BMG :BTL_BMG_SearchByCod Invalid LAP (%d)",status));
			return FALSE;
		}
	}
	else
	{
		lap = BT_IAC_GIAC;
	}

	Report(("Performing Search:, maxResp:%d, %s Name Requests, %s, LAP:%s", 
			maxResp, 
			(performNameRequest ? "With" : "No"), 
			((CODFilterPtr != 0) ? "Use COD Filter" : "No COD Filter"),
			(lap == BT_IAC_GIAC) ? "GIAC" : "LIAC"));

	status = BTL_BMG_SearchByCod(bmgContext, 
					BTL_BMG_SEARCH_REGULAR, 
					lap, 
					BT_INQ_TIME_GAP100, 
									maxResp,
									CODFilterPtr, 
									performNameRequest, 
					SDP_SERVICE_NONE);

	Report(("BMG :BTL_BMG_SearchByCod returned status: %s", pBT_Status(status)));
			
	if (status == BT_STATUS_SUCCESS)
	{
		return TRUE;
	}
		return FALSE;

}


/*---------------------------------------------------------------------------
 *            APP_BMG_StopSearch
 *---------------------------------------------------------------------------
 *
 * Synopsis: Stops search process.		
 *
 * Return:    TRUE if stop search process is successfull, FALSE otherwise.
 *
 */
BOOL APP_BMG_StopSearch (void)
{
	BtStatus status = BT_STATUS_FAILED;
	status = BTL_BMG_CancelSearch(bmgContext);
	Report(("BMG :BTL_BMG_CancelSearch returned status: %s", pBT_Status(status)));
	if (status == BT_STATUS_SUCCESS)
	{
		return TRUE;
	}
		return FALSE;
}

/*---------------------------------------------------------------------------
 *            APP_BMG_CleanDeviceDB
 *---------------------------------------------------------------------------
 *
 * Synopsis: Cleans device DB		
 *
 * Return:    TRUE if stop clean process is successfull, FALSE otherwise.
 *
 */
BOOL APP_BMG_CleanDeviceDB(void)
{
	BtStatus status = BT_STATUS_FAILED;
	status = BTL_BMG_CleanDeviceDB(bmgContext);
	Report(("BMG :BTL_BMG_CleanDeviceDB returned status: %s", pBT_Status(status)));
	if (status == BT_STATUS_SUCCESS)
	{
		return TRUE;
	}
		return FALSE;
}


/*---------------------------------------------------------------------------
 *            APP_BMG_SetLocalName
 *---------------------------------------------------------------------------
 *
 * Synopsis: Set local name of our device.		
 *
 * Return:    TRUE if setting local name is successfull, FALSE otherwise.
 *
 */
BOOL APP_BMG_SetLocalName (void)
{
	U8 length,name[BTL_BMG_MAX_LOCAL_NAME_LEN + 1];
	BtStatus status = BT_STATUS_FAILED;

	LINE_PARSER_GetNextStr(name, BTL_BMG_MAX_LOCAL_NAME_LEN);
	length = (U8)(OS_StrLen((const char *)(name)));
	status = BTL_BMG_SetLocalDeviceName(bmgContext, (U8 *)(name), (length+1));
	Report(("BMG :BTL_BMG_SetLocalDeviceName returned status: %s", pBT_Status(status)));
	if (status == BT_STATUS_SUCCESS)
	{
		Report(("BMG :New name is %s", (char *)(name)));
		return TRUE;
	}
		return FALSE;
}


/*---------------------------------------------------------------------------
 *            APP_BMG_SetSecurityMode
 *---------------------------------------------------------------------------
 *
 * Synopsis: Set the security level of our device.		
 *
 * Return:    TRUE if setting the security level is successfull, FALSE otherwise.
 *
 */
BOOL APP_BMG_SetSecurityMode (void)
{
	BtStatus status = BT_STATUS_FAILED;
	U8 SecLevel;

	LINE_PARSER_GetNextU8(&SecLevel, FALSE);
	status = BTL_BMG_SetSecurityMode(bmgContext, (SecLevel+1));
	Report(("BMG :BTL_BMG_SetSecurityMode returned status: %s", pBT_Status(status)));
	if (status == BT_STATUS_SUCCESS)
	{
		return TRUE;
	}
		return FALSE;
}


/*---------------------------------------------------------------------------
 *            APP_BMG_SetSecurityMode
 *---------------------------------------------------------------------------
 *
 * Synopsis: Get the security level of our device.		
 *
 * Return:    TRUE if getting the security level is successfull, FALSE otherwise.
 *
 */
BOOL APP_BMG_GetSecurityMode (void)
{
	BtStatus status = BT_STATUS_FAILED;
	BtSecurityMode CurrentSecLevel;

	status = BTL_BMG_GetSecurityMode(bmgContext, &CurrentSecLevel);
	Report(("BMG :APP_BMG_GetSecurityMode returned status: %s", pBT_Status(status)));
	if (status == BT_STATUS_SUCCESS)
	{
		Report(("BMG : Current security mode is: %d", (CurrentSecLevel-1)));
		return TRUE;
	}
		return FALSE;
}



/*---------------------------------------------------------------------------
 *            APP_BMG_AuthorizeDeviceReply
 *---------------------------------------------------------------------------
 *
 * Synopsis: Reply for authorization request of another device.		
 *
 * Return:    TRUE if Autorization reply is successfull, FALSE otherwise.
 *
 */
BOOL APP_BMG_AuthorizeDeviceReply (void)
{
	BtStatus status = BT_STATUS_FAILED;
	U8 AuthorizationType,BD_array[BDADDR_NTOA_SIZE];
	BD_ADDR BdAddr;
	
	LINE_PARSER_GetNextStr(BD_array, 17);
	BdAddr = bdaddr_aton((const char *)(BD_array));
	LINE_PARSER_GetNextU8(&AuthorizationType, FALSE);
	status = BTL_BMG_AuthorizeDeviceReply(bmgContext, &BdAddr, (BtlBmgAuthorizationType) (AuthorizationType));
	if (status == BT_STATUS_SUCCESS)
	{
		return TRUE;
	}
		return FALSE;
}

/*---------------------------------------------------------------------------
 *            APP_BMG_EnableTestMode
 *---------------------------------------------------------------------------
 *
 * Synopsis: Switches OFF the GSM RF and Enables test mode.		
 *
 * Return:    TRUE always
 *
 */

BOOL APP_BMG_EnableTestMode(void)
{
	testModeEnabled = TRUE;
	
	/*Switches OFF GSM RF*/
	AppBmgCloseGSMRF();

	return TRUE;	
}

BOOL APP_BMG_GetRemoteDeviceCod(void)
{
	BtStatus				status;
	LINE_PARSER_STATUS 	lineStatus;
	U8 					bdArray[BDADDR_NTOA_SIZE + 1];
	BD_ADDR 			bdAddr;
	BtClassOfDevice		cod;

	/* Handle  BD Address Argument */
	
	lineStatus = LINE_PARSER_GetNextStr(bdArray, BDADDR_NTOA_SIZE);
	
	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BMG: Failed Reading BD Address Argument(%d)", lineStatus));
		return;
	}

	bdAddr = bdaddr_aton((const char *)(bdArray));

	status = BTL_BMG_GetRemDevCod(bmgContext, &bdAddr, &cod);

	if (status ==  BT_STATUS_SUCCESS)
	{
		Report(("BMG: COD (%s): %x", bdaddr_ntoa(&(bdAddr), (char*)bdArray), cod));
	}
	else
	{
		Report(("BMG: BTL_BMG_GetRemDevCod Failed (%s)", pBT_Status(status)));
	}

	return TRUE;
}

BOOL APP_BMG_GetDeviceRecord(void)
{
	BtStatus				status;
	LINE_PARSER_STATUS 	lineStatus;
	U8 					bdArray[BDADDR_NTOA_SIZE + 1];
	BD_ADDR 			bdAddr;
	BtlBmgDeviceRecord	record;

	/* Handle  BD Address Argument */
	
	lineStatus = LINE_PARSER_GetNextStr(bdArray, BDADDR_NTOA_SIZE);
	
	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BMG: Failed Reading BD Address Argument(%d)", lineStatus));
		return;
	}

	bdAddr = bdaddr_aton((const char *)(bdArray));

	status = BTL_BMG_FindDeviceRecord(bmgContext, &bdAddr, &record);

	if (status == BT_STATUS_DEVICE_NOT_FOUND)
	{
		Report(("BMG: %s Not Found in Device DB", bdaddr_ntoa(&(bdAddr), (char*)bdArray)));
	}
	else if (status ==  BT_STATUS_SUCCESS)
	{
		Report(("BMG: Record Data For %s", bdaddr_ntoa(&(bdAddr), (char*)bdArray)));
		Report(("BMG: Name: |%s|", record.name));		
		Report(("BMG: COD: %x", record.classOfDevice));		
		Report(("BMG: %s Link Key", (record.keyType == KEY_NOT_DEFINED) ? ("-- NO -- ") : ("-- HAS -- ")));
		Report(("BMG: PIN Len: %d", (U32)record.pinLen));
		Report(("BMG: %s Device", (record.trusted == TRUE) ? ("TRUSTED") : ("NOT TRUSTED")));
		Report(("BMG: Trusted Per Service: %x", record.trustedPerService));
		Report(("BMG: Services: %x", record.services));
		Report(("BMG: psMode: %d, psRepMode: %d", (U32)record.psi.psMode, (U32)record.psi.psRepMode));
	}
	else
	{
		Report(("BMG: BTL_BMG_FindDeviceRecord Failed (%s)", pBT_Status(status)));
	}

	return TRUE;
}

BOOL APP_BMG_AddDeviceRecord(void)
{
	BtStatus				status;
	LINE_PARSER_STATUS 	lineStatus;
	U8 					bdArray[BDADDR_NTOA_SIZE + 1];
	BD_ADDR 			bdAddr;
	BtlBmgDeviceRecord	record;

	/* Handle  BD Address Argument */
	
	lineStatus = LINE_PARSER_GetNextStr(bdArray, BDADDR_NTOA_SIZE);
	
	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BMG: Failed Reading BD Address Argument(%d)", lineStatus));
		return;
	}

	bdAddr = bdaddr_aton((const char *)(bdArray));

	status = BTL_BMG_FindDeviceRecord(bmgContext, &bdAddr, &record);

	if (status == BT_STATUS_SUCCESS)
	{
		Report(("BMG: %s Already Exists in Device DB", bdaddr_ntoa(&(bdAddr), (char*)bdArray)));
	}
	else if (status ==  BT_STATUS_NOT_FOUND)
	{
		status = BTL_BMG_InitDeviceRecord(bmgContext, &record);

		if (status != BT_STATUS_SUCCESS)
		{
			Report(("BMG: BTL_BMG_InitDeviceRecord Failed (%s)", pBT_Status(status)));
		}

		record.bdAddr = bdAddr;

		status = BTL_BMG_SetDeviceRecord(bmgContext, &record);

		if (status != BT_STATUS_SUCCESS)
		{
			Report(("BMG: BTL_BMG_SetDeviceRecord Failed (%s)", pBT_Status(status)));
		}

		Report(("BMG: Successfully Added %s to the Device DB", bdaddr_ntoa(&(bdAddr), (char*)bdArray)));
	}
	else
	{
		Report(("BMG: BTL_BMG_FindDeviceRecord Failed (%s)", pBT_Status(status)));
	}

	return TRUE;
}

BOOL APP_BMG_GetRemoteDeviceName(void)
{
	BtStatus				status;
	LINE_PARSER_STATUS 	lineStatus;
	U8 					bdArray[BDADDR_NTOA_SIZE + 1];
	BD_ADDR 			bdAddr;

	/* Handle  BD Address Argument */
	
	lineStatus = LINE_PARSER_GetNextStr(bdArray, BDADDR_NTOA_SIZE);
	
	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BMG: Failed Reading BD Address Argument(%d)", lineStatus));
		return;
	}

	bdAddr = bdaddr_aton((const char *)(bdArray));

	status = BTL_BMG_GetRemoteDeviceName(bmgContext, &bdAddr);

	if (status != BT_STATUS_PENDING)
	{
		Report(("BMG: BTL_BMG_GetRemoteDeviceName Failed (%s) ", pBT_Status(status)));
	}
	else 
	{
		Report(("BMG: Waiting for Remote Name Result"));
	}

	return TRUE;
}

static BtStatus AppBmgDeinitBat(void)
{
	T_BAT_return retVal;
	BtStatus status = BT_STATUS_SUCCESS;

	retVal = bat_delete(batModuleData.instance);

	if (BAT_OK != retVal)
	{
		Report(("APP BMG: bat_delete() failed: %s", AppBmgBatRetToString(retVal)));
		status = BT_STATUS_FAILED;
	}

	retVal = bat_deinit();
	if (BAT_OK != retVal)
	{
		Report(("APP BMG: bat_deinit() failed: %s", AppBmgBatRetToString(retVal)));
		status = BT_STATUS_FAILED;
	}

	return status;
}

static void AppBmgClientSigCb(T_BAT_client client, T_BAT_signal sig)
{
	Report(("APP BMG: AppBmgClientSigCb() sig = %d", sig));

	switch (sig)
	{
		case BAT_OPEN_CLIENT_SUCCEED:
			break;

		case BAT_OPEN_CLIENT_FAIL:
			Report(("APP BMG: Failed to create new client"));
			if (BAT_OK != bat_close(batModuleData.client))
			{
				Report(("APP BMG: bat_close() for BAT client failed"));
			}
			if (BT_STATUS_SUCCESS != AppBmgDeinitBat())
			{
				Report(("APP BM: AppBmgDeinitBat failed"));
			}
			break;

		case BAT_READY_RESOURCE:
			/* AT Commands can now be sent */
			break;

		default:
			break;
	}
}

static const char* AppBmgBatRetToString(T_BAT_return retVal)
{
	switch (retVal)
	{
	case BAT_OK:
		return  "BAT_OK";
	case BAT_BUSY_RESOURCE:
		return "BAT_BUSY_RESOURCE";
	case BAT_ERROR:
		return "BAT_ERROR";
	default:
		break;
	}
	return "Unrecognized BAT return value";
}

/*---------------------------------------------------------------------------
 *            AppBmgSendCommand
 *---------------------------------------------------------------------------
 *
 * Synopsis: Sends AT command to the BAT. Only one AT command can be send using this BAT 	
 *			interface. 
 *
 * Return:    TRUE always
 *
 */
static void AppBmgSendCommand(void)
{
	T_BAT_return res;

	res = bat_send(batModuleData.client, &batModuleData.atCmd.cmd);

	switch (res)
	{
	case BAT_OK:
	case BAT_BUSY_RESOURCE:
		break;

	case BAT_ERROR:
		Report(("APP BMG: bat_send failed"));
	default:
		break;
	}
}

static int AppBmgHandleOkRes()
{
	/*GSM Clock switched OFF*/
	Report(("APP BMG: GSM Clock Switched OFF"));

	if(testModeEnabled == TRUE)
	{
		/*Enables Test Mode*/
		AppBmgEnableTestMode();
	}

	return TRUE;
}

static int AppBmgClientRspCb(T_BAT_client client, T_BAT_cmd_response *rsp)
{
	int retVal = BAT_OK;

	Report(("APP BMG: AppBmgClientRspCb. response is %d", rsp->ctrl_response));
	
	switch(rsp->ctrl_response)
	{

	case BAT_RES_AT_OK:
		retVal = AppBmgHandleOkRes();
		break;
	case BAT_RES_AT_NO_CARRIER_FINAL:
	case BAT_RES_AT_BUSY:
	case BAT_RES_AT_NO_ANSWER:
	case BAT_RES_PLUS_CME_ERROR:
	case BAT_RES_PLUS_EXT_ERROR:
		Report(("APP BMG: AT command Response: FAILED"));
		break;
			
	default:
		break;
	}

	return retVal;
}


static void AppBmgNewBatInstanceSigCb(T_BAT_signal sig)
{
	T_BAT_return retVal;

	if (BAT_NEW_INSTANCE_SUCCEED != sig)
	{
		Report(("APP BMG: BAT new instance failed"));
		return;
	}

	/*Opens a BAT client*/
	retVal = bat_open(batModuleData.instance, &batModuleData.client, AppBmgClientRspCb, AppBmgClientSigCb);
	if (BAT_OK != retVal)
	{
		Report(("APP BMG: bat_open failed: %s", AppBmgBatRetToString(retVal)));
		bat_close(batModuleData.client);
	}
}


static BtStatus AppBmgInitBat(void)
{
	T_BAT_return retVal;
	BtStatus     status = BT_STATUS_FAILED;
	unsigned int sleepCounter = 0;
	T_BAT_config *config;

	config = &batModuleData.config;
	config->adapter.gdd_if = gdd_func_dio;
	config->adapter.cap.dio_cap.mtu_size = 300; /*GDD_DIO_MTU_SIZE*/
	config->device = DEVICE_PACKET;
	config->l2p.protocol_id = L2P_SP_PSI;

	retVal = bat_init((void *)batModuleData.batInstBuf, BMG_NUM_OF_BAT_INSTANCES);
	if (BAT_OK != retVal)
	{
		Report(("APP BMG: bat_init() returned: %s", AppBmgBatRetToString(retVal)));
		return BT_STATUS_FAILED;
	}

	while ((retVal = bat_new(&batModuleData.instance, (void *)batModuleData.batClientBuf, BMG_NUM_OF_BAT_CLIENTS,
				     &batModuleData.config, AppBmgNewBatInstanceSigCb)) != BAT_OK)
	{
		sleepCounter++;
		BTHAL_OS_Sleep(100);

		/* Wait at most 10 sec */
		if (sleepCounter >= 300)
			break;
	}

	if (BAT_OK != retVal)
	{
		Report(("APP BMG: bat_new() returned: %s", AppBmgBatRetToString(retVal)));
		bat_deinit();
		return BT_STATUS_FAILED;
	}

	return BT_STATUS_SUCCESS;
}


static void AppBmgCloseGSMRF(void)
{
	T_BAT_cmd_set_plus_cfun param;

	param.fun = BAT_CFUN_FUN_MIN;
	param.rst = BAT_CFUN_RST_NO_RESET;
	
	batModuleData.atCmd.cmd.ctrl_params = BAT_CMD_SET_PLUS_CFUN;
	batModuleData.atCmd.cmd.params.ptr_set_plus_cfun = &param;

	AppBmgSendCommand();
}


static void AppBmgEnableTestMode(void)
{
	BtStatus status = BT_STATUS_FAILED;

	/*Enables Testmode*/
	status = BTL_BMG_EnableTestMode(bmgContext);	
	if (status == BT_STATUS_FAILED)
	{	
		Report(("APP BMG: BTL_BMG_EnableTestMode Failed"));
	}
	else
	{
		Report(("APP_BMG: BTL_BMG_EnableTestMode Success"));
	}

	testModeEnabled = FALSE;
}


/*---------------------------------------------------------------------------
 *            BMGA_Init
 *---------------------------------------------------------------------------
 *
 * Synopsis: BMG application initialization.		
 *
 * Return:    NONE.
 *
 */
void BMGA_Init (void)
{
	BtStatus  status = BT_STATUS_FAILED;
	
	status = BTL_BMG_Create(0, BmgaCallback, &bmgContext);
	Assert(status == BT_STATUS_SUCCESS);

	status = BTL_BMG_SetEventForwardingMode(bmgContext, BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
	Assert(status == BT_STATUS_SUCCESS);
	
#if 0
	status = BTL_BMG_SetClassOfDevice(bmgContext, (COD_MAJOR_PHONE | COD_MINOR_PHONE_CELLULAR));
	Assert(status == BT_STATUS_SUCCESS);
#endif
	
	status = AppBmgInitBat();
	Assert(status == BT_STATUS_SUCCESS);
	
	Report(("BMGA_Init() complete.\n"));
}

/*---------------------------------------------------------------------------
 *            BMGA_Deinit
 *---------------------------------------------------------------------------
 *
 * Synopsis: BMG application deinitialization.		
 *
 * Return:    NONE.
 *
 */
void BMGA_Deinit(void)
{
	BtStatus status = BT_STATUS_FAILED;

	status = AppBmgDeinitBat();
	Assert(status == BT_STATUS_SUCCESS);

	
	status = BTL_BMG_Destroy(&bmgContext);
	Assert(status == BT_STATUS_SUCCESS);
		
	Report(("BMGA_Deinit() complete.\n"));
}

/*---------------------------------------------------------------------------
 *            BmgaCallback
 *---------------------------------------------------------------------------
 *
 * Synopsis: Callback function to handle BMG profile events.	
 *
 * Return:    NONE.
 *
 */
void BmgaCallback (const BtlBmgEvent *bmgEvent)
{
	const BtEvent *event = bmgEvent->event;
	BtStatus status = BT_STATUS_FAILED;
	char  bdAddr[BDADDR_NTOA_SIZE];
	char *name;
	switch (event->eType)
	{
	case BTEVENT_HCI_INITIALIZED:
		Report(("BmgaCallback: BTEVENT_HCI_INITIALIZED"));
		break;
		
	case BTEVENT_PIN_REQ:
		if (event->errCode == BT_STATUS_SUCCESS)
		{
			/* Prompt the user for a PIN code */
			UI_DISPLAY(("Please Enter PIN Code for %s", bdaddr_ntoa(&event->p.remDev->bdAddr, bdAddr)));
		}
		else
		{
			/* Cancel the PIN code request */
			Report ((("BMG: Pin code couldn't be displayed because of an error")));
		}
		break;

	case BTEVENT_AUTHORIZATION_REQ:
		Report(("Authorization request received from %s",bdaddr_ntoa(&event->p.remDev->bdAddr, bdAddr)));
		break;
		
	case BTEVENT_INQUIRY_RESULT:							
		UI_DISPLAY(("Inquiry result arrived, BD address: %s", bdaddr_ntoa(&event->p.inqResult.bdAddr, bdAddr)));	
		break;
		
	case BTEVENT_INQUIRY_COMPLETE:
	case BTEVENT_INQUIRY_CANCELED:
		UI_DISPLAY(("Inquiry %s ", 
			((event->eType == BTEVENT_INQUIRY_COMPLETE) ? ("completed") : ("canceled"))));
		break;
		
	case BTEVENT_NAME_RESULT:
		if (event->errCode == BEC_NO_ERROR)
		{
			if (event->p.meToken->p.name.io.out.len == 0)
			{
				name =  "Unknown";
			}
			else
			{
				name = (char *)(event->p.meToken->p.name.io.out.name);
			}

			UI_DISPLAY(("Received name %s for device %s", 
				name,
				bdaddr_ntoa(&event->p.meToken->p.name.bdAddr, bdAddr)));
			
		}
		else
		{
			/* Record that the name request failed. Next time we'll skip it. */
            		Report(("BMG: Name request for %s failed, status %s.\n", 
            		bdaddr_ntoa(&event->p.meToken->p.name.bdAddr, bdAddr), 
            				pHC_Status(event->errCode)));
		}
				
	default:
		break;
	}
}





