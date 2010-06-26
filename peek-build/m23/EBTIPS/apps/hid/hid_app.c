
/*******************************************************************************\
*                                                                         									  	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  	LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT			*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      hid_app.c
*
*   DESCRIPTION:    This file contains the HID Host application for the neptune
*				   platform.
*
*
*   AUTHOR:         Avraham HAMU
*
\*******************************************************************************/

#include "btl_config.h"
#include "osapi.h"
#if BTL_CONFIG_HIDH == BTL_CONFIG_ENABLED

/* GPF Includes */
#include "typedefs.h"
#include "vsi.h"
#include "../p_btt.h"

/* BTIPS Includes */
#include "me.h"

#include "../app_main.h"
#include "btl_common.h"
#include "bthal_btdrv.h"
#include "bthal_pm.h"
#include "bttypes.h"
#include "goep.h"
#include "bpp.h"
#include "debug.h"

/* BTL includes */
#include "btl_hid.h"
#include "lineparser.h"


#define MAX_DATA_LEN					(2048)
#define HID_COMMANDS_NUM 				26
#define MAX_CHANNEL_ID					(7)



/****************************************************************************
 *
 * Internal Function prototypes
 *
 ***************************************************************************/
void HIDA_ProcessUserAction(U8 *msg);
void HIDA_Init(void);
void HIDA_Deinit(void);
void APP_HIDH_Create(void);
void APP_HIDH_Destroy(void);
void APP_HIDH_Enable(void);
void APP_HIDH_Disable(void);
void APP_HIDH_Connect(void);
void APP_HIDH_HostQueryDevice(void);
void APP_HIDH_GetConnectedDevice(void);
void APP_HIDH_Disconnect(void);
void APP_HIDH_SetProtocol(void);
void APP_HIDH_GetProtocol(void);
void APP_HIDH_SetIdleRate(void);
void APP_HIDH_GetIdleRate(void);
void APP_HIDH_HardReset(void);
void APP_HIDH_SoftReset(void);
void APP_HIDH_Suspend(void);
void APP_HIDH_ExitSuspend(void);
void APP_HIDH_Unplug(void);
void APP_HIDH_SetReport(void);
void APP_HIDH_GetReport(void);
void APP_HIDH_SendInterrupt(void);
void APP_HID_SetSecurityLevel(void);
void APP_HID_GetSecurityLevel(void);
void APP_HID_GetNumChannels(void);
void APP_HID_SetNumChannels(void);


static void APP_HIDH_CallBack(const BtlHidEvent *event);
static char * pResultCode(HidResultCode result);
static char * pHidProtocol(HidProtocol Protocol);
static void printOuput(U16 max , U8  *data);
static U32 manageChannelId(BtlHidChannelId Channel, BOOL *ChannelIdIndex, BOOL channelOpened);


/****************************************************************************
 *
 * TxInterrupt
 *
 ****************************************************************************/

typedef struct _TxInterrupt{
    U8 data[MAX_DATA_LEN];
	HidInterrupt *interrupts;
} TxInterrupt;


/****************************************************************************
 *
 * TxTransaction
 *
 ****************************************************************************/
 
typedef struct _TxTransaction{
	union{
		    U8 data[MAX_DATA_LEN];
		}u;
	HidTransaction *transaction;
} TxTransaction;



typedef void (*FuncType)(void);

/****************************************************************************
 *
 * _hidCommands
 *
 ****************************************************************************/


typedef struct
{
	char 		funcName[LINE_PARSER_MAX_STR_LEN];
	FuncType    funcPtr;
} _hidCommands;

/* HID commands array */
static const  _hidCommands hidCommands[] =  { {"init",	    HIDA_Init}, 		       
												{"deinit",      	HIDA_Deinit}, 			
												{"create",       APP_HIDH_Create}, 		     	
												{"destroy",     APP_HIDH_Destroy},   
												{"enable",      APP_HIDH_Enable}, 		    
												{"disable",      APP_HIDH_Disable},		     	 
												{"connect",     APP_HIDH_Connect},		     	 
												{"disconnect", APP_HIDH_Disconnect},		     	 
												{"getcon",     APP_HIDH_GetConnectedDevice},		     	 
												{"query",     APP_HIDH_HostQueryDevice},	     	 
												{"setproto",     APP_HIDH_SetProtocol},		     	 
												{"getproto",     APP_HIDH_GetProtocol},
												{"setidle",     APP_HIDH_SetIdleRate},		     	 
												{"getidle",     APP_HIDH_GetIdleRate},		     	 
												{"hard",     APP_HIDH_HardReset},		     	 
												{"soft",     APP_HIDH_SoftReset},		     	 
												{"sus",     APP_HIDH_Suspend},		     	 
												{"exsus",     APP_HIDH_ExitSuspend},
												{"unplug",     APP_HIDH_Unplug},
												{"sreport",     APP_HIDH_SetReport},
												{"greport",     APP_HIDH_GetReport},
												{"sintr",     APP_HIDH_SendInterrupt},
												{"setsec",     APP_HID_SetSecurityLevel},	
												{"getsec",     APP_HID_GetSecurityLevel},	
												{"getchan",     APP_HID_GetNumChannels},	
												{"setchan",     APP_HID_SetNumChannels}	
											 };




static BtlHidContext	*hidContext = 0;
static BD_ADDR 		bdAddr;
static TxTransaction 	trans={0};
static TxInterrupt 		inter={0};
static BtSecurityLevel	securityLevel = BSL_NO_SECURITY;
static BtlHidChannelId mainChannel=0;
static U32 NumChannelId=0;
static BOOL ChannelIdIndex[7]={0};
static U32 TypeOfDevice[MAX_CHANNEL_ID]={0};


/*******************************************************************************\
*  FUNCTION:	HIDA_UiStreamToAction
*                                                                         
*  PARAMETERS:	msg - Parameters of actions.
*                                                                         
*  DESCRIPTION:	Processes primitives for the HIDH application
*                                                                         
*  RETURNS:		None.
\*******************************************************************************/
void HIDA_ProcessUserAction(U8 *msg)
{	

	U8 i;
	char command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS status;

	status = LINE_PARSER_GetNextStr((U8 *)command, LINE_PARSER_MAX_STR_LEN);

	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("Wrong HID command. Can't parse line"));
		return;
	}

	/* Look for the correct function */
	for(i = 0; i < HID_COMMANDS_NUM; i++)
	{
		if (OS_StrCmp(command, hidCommands[i].funcName) == 0)
		{
			hidCommands[i].funcPtr();
			return;
		}
	}
		Report(("Wrong HID command"));
}


/*---------------------------------------------------------------------------
 *            HIDA_Init
 *---------------------------------------------------------------------------
 *
 * Synopsis:   Initialize the HID Module.
 *
 * Return:      void
 *
 */
void HIDA_Init(void)
{
	BtStatus status;
	status = BTL_HIDH_Create(0, APP_HIDH_CallBack, 0, &hidContext);
	Report(("APP_HIDH_Create() returned %s.",pBT_Status(status)));
	status = BTL_HIDH_Enable(hidContext, 0, 0);	
    	Report(("APP_HIDH_Enable() returned %s.",pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            HIDA_Deinit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the HID Module.
 *
 * Return:    void
 *
 */
void HIDA_Deinit(void)
{
	BtStatus status;
	status = BTL_HIDH_Disable(hidContext);
    	Report(("APP_HIDH_Disable() returned %s.",pBT_Status(status)));
	status =  BTL_HIDH_Destroy(&hidContext);
    	Report(("APP_HIDH_Destroy() returned %s.",pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_Create
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Allocates a unique HID context.		
 *
 * Return:    void
 *
 */
void APP_HIDH_Create()
{
	BtStatus status;
	LINE_PARSER_STATUS parserStatus;

	if (TRUE == LINE_PARSER_AreThereMoreArgs() )
	{
		parserStatus = LINE_PARSER_GetNextU8(&securityLevel, TRUE);
		
		if (parserStatus != LINE_PARSER_STATUS_SUCCESS)
		{
			Report(("HID: Can't read security level. Ignoring command"));
			return ;
		}
	}
	status = BTL_HIDH_Create(0, APP_HIDH_CallBack, &securityLevel, &hidContext);
	Report(("BTL_PBAPS_Create() returned %s.",pBT_Status(status)));
	UI_DISPLAY(("APP_HIDH_Create returned %s", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_Destroy
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Releases a HID context .		
 *
 * Return:    void
 *
 */
void APP_HIDH_Destroy(void)
{
	BtStatus status;
	status = BTL_HIDH_Destroy(&hidContext);
	UI_DISPLAY(("APP_HIDH_Destroy returned %s", pBT_Status(status)));	
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_Enable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enable the HID module.
 *
 * Return:    void
 *
 */
void APP_HIDH_Enable(void)
{
	BtStatus status;
	status = BTL_HIDH_Enable(hidContext, 0, 0);
	UI_DISPLAY(("APP_HIDH_Enable returned %s", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_Disable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disable the HID module.
 *
 * Return:    void
 *
 */
void APP_HIDH_Disable(void)
{
	BtStatus status;
	status = BTL_HIDH_Disable(hidContext);
    	UI_DISPLAY(("APP_HIDH_Disable returned %s", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_Connect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiates a connection to a remote HID Device. .
 *
 * Return:    void
 *
 */
void APP_HIDH_Connect()
{
	BtStatus status;
	U8	      BdArray[17];
	LINE_PARSER_STATUS  ParserStatus;
	
	ParserStatus = LINE_PARSER_GetNextStr(BdArray, 17);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("HID wrong BD addr. Can't connect."));
		return;
	}
	
	bdAddr = bdaddr_aton((const char *)(BdArray));
	status = BTL_HIDH_Connect(hidContext, &bdAddr);
    	UI_DISPLAY(("APP_HIDH_Connect returned %s", pBT_Status(status)));
}

/*---------------------------------------------------------------------------
 *            APP_HIDH_HostQueryDevice
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Queries the Device for its SDP database entry.
 *
 * Return:    void
 *
 */
void APP_HIDH_HostQueryDevice(void)
{
	BtStatus status;
	U8	      BdArray[17];
	LINE_PARSER_STATUS  ParserStatus;

	ParserStatus = LINE_PARSER_GetNextStr(BdArray, 17);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("HID wrong BD addr. Can't connect."));
		return;
	}
	
	bdAddr = bdaddr_aton((const char *)(BdArray));
	status = BTL_HIDH_HostQueryDevice(hidContext, mainChannel, &bdAddr);
    	UI_DISPLAY(("APP_HIDH_HostQueryDevice returned %s", pBT_Status(status)));
}

/*---------------------------------------------------------------------------
 *            APP_HIDH_GetConnectedDevice
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function returns the connected device.
 *
 * Return:    void
 *
 */
void APP_HIDH_GetConnectedDevice(void)
{
	BtStatus status;
	char addr[BDADDR_NTOA_SIZE];
	
	status = BTL_HIDH_GetConnectedDevice(hidContext, mainChannel, &bdAddr);
	Report(("BTL_HID: Connection opened with BD_Addr %s.", bdaddr_ntoa(&bdAddr,addr)));
    	UI_DISPLAY(("APP_HIDH_GetConnectedDevice returned %s", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_Disconnect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Terminates a connection with a remote HID device.
 *
 * Return:    void
 *
 */
void APP_HIDH_Disconnect(void)
{
	BtStatus status;
	status = BTL_HIDH_Disconnect(hidContext, mainChannel);
    	UI_DISPLAY(("APP_HIDH_Disconnect returned %s", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_SetProtocol
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends the current protocol to the HID device.
 *
 * Return:    void
 *
 */
void APP_HIDH_SetProtocol(void)
{
	BtStatus status;
	HidProtocol reportProtocol ;
	U8 typeProto[30]={0};
	
	if (LINE_PARSER_GetNextStr(typeProto, LINE_PARSER_MAX_STR_LEN) != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("APP_HIDH_SetProtocol() failed "));	
		return;
	}
	else
	{
		if (OS_StrCmp( "report\0", (const char *)typeProto) == 0)
			{
				reportProtocol = 1;
			}
		else
			{
				if (OS_StrCmp( "boot\0", (const char *)typeProto) == 0)
				{
					reportProtocol = 0;
				}
				else
				{
					Report(("APP_HIDH_SetProtocol() wrong parameter "));
				}
			}
		status = BTL_HIDH_SetProtocol(hidContext, mainChannel, reportProtocol, &(trans.transaction));
    		UI_DISPLAY(("APP_HIDH_SetProtocol returned %s", pBT_Status(status)));
	}
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_GetProtocol
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends a protocol request to the HID device.
 *
 * Return:    void
 *
 */
void APP_HIDH_GetProtocol(void)
{
	BtStatus status;
	status = BTL_HIDH_GetProtocol(hidContext, mainChannel, &(trans.transaction));
    	UI_DISPLAY(("APP_HIDH_GetProtocol returned %s", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_SetIdleRate
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends the idle rate to the HID device.
 *
 * Return:    void
 *
 */
void APP_HIDH_SetIdleRate(void )
{
	BtStatus status;
	HidIdleRate reportIdleRate=0;

	if (LINE_PARSER_GetNextU8(&reportIdleRate, FALSE) != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("APP_HIDH_SetIdleRate() failed "));	
		return;
	}
	else
	{
		status = BTL_HIDH_SetIdleRate(hidContext,mainChannel, reportIdleRate, &(trans.transaction));
    		UI_DISPLAY(("APP_HIDH_SetIdleRate returned %s", pBT_Status(status)));
	}
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_GetIdleRate
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends an idle rate status request to the HID device.
 *
 * Return:    void
 *
 */
void APP_HIDH_GetIdleRate(void)
{
	BtStatus status;
	status = BTL_HIDH_GetIdleRate(hidContext, mainChannel, &(trans.transaction));
    	UI_DISPLAY(("APP_HIDH_GetIdleRate returned %s", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_HardReset
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends a "HardReset" operation control to the remote device.
 *
 * Return:    void
 *
 */
void APP_HIDH_HardReset(void)
{
	BtStatus status;
	status = BTL_HIDH_HardReset(hidContext, mainChannel, &(trans.transaction));
    	UI_DISPLAY(("APP_HIDH_HardReset returned %s", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_SoftReset
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends a "SoftReset" operation control to the remote device.
 *
 * Return:    void
 *
 */
void APP_HIDH_SoftReset(void)
{
	BtStatus status;
	status = BTL_HIDH_SoftReset(hidContext, mainChannel, &(trans.transaction));
    	UI_DISPLAY(("APP_HIDH_SoftReset returned %s", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_Suspend
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends a "Suspend" operation control to the remote device
 *
 * Return:    void
 *
 */
void APP_HIDH_Suspend(void)
{
	BtStatus status;
	status = BTL_HIDH_Suspend(hidContext, mainChannel, &(trans.transaction));
    	UI_DISPLAY(("APP_HIDH_Suspend returned %s", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_ExitSuspend
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends an "Exit Suspend" operation control to the remote device.
 *
 * Return:    void
 *
 */
void APP_HIDH_ExitSuspend(void)
{
	BtStatus status;
	status = BTL_HIDH_ExitSuspend(hidContext, mainChannel, &(trans.transaction));
    	UI_DISPLAY(("APP_HIDH_ExitSuspend returned %s", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_Unplug
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends a "Unplug" operation control to the remote device.
 *
 * Return:    void
 *
 */
void APP_HIDH_Unplug(void)
{
	BtStatus status;
	status = BTL_HIDH_Unplug(hidContext, mainChannel, &(trans.transaction));
    	UI_DISPLAY(("APP_HIDH_Unplug returned %s", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_SetReport
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends a report to the HID device.
 *
 * Return:    void
 *
 */
void APP_HIDH_SetReport(void)
{
	BtStatus status;
	U8 string[30]={0};
	U8 dataTrans[30]={0};
	U16 dataLenTrans;
	HidReportType reportTypeTrans;

	if ((LINE_PARSER_GetNextStr(dataTrans, LINE_PARSER_MAX_STR_LEN) == LINE_PARSER_STATUS_SUCCESS))
		{
		if ((LINE_PARSER_GetNextStr(string, LINE_PARSER_MAX_STR_LEN) == LINE_PARSER_STATUS_SUCCESS))
			{
				if (OS_StrCmp( "other\0", (const char *)string) == 0)
							{
								reportTypeTrans = 0;
							}
						else
							{
								if (OS_StrCmp( "input\0",(const char *) string) == 0)
								{
									reportTypeTrans = 1;
								}
								else
								{
									if (OS_StrCmp( "output\0", (const char *)string) == 0)
									{
										reportTypeTrans = 2;
									}
									else
									{
										if (OS_StrCmp( "feature\0", (const char *)string) == 0)
										{
											reportTypeTrans = 3;
										}
										else
										{
											Report(("APP_HIDH_SetReport()  parameter 2 not valid"));
										}
									}
								}
							}
						
					dataLenTrans=OS_StrLen((const char *)dataTrans);
					if(dataLenTrans < MAX_DATA_LEN)
						{
							BTHAL_UTILS_MemCopy(trans.u.data, dataTrans, dataLenTrans);
							status = BTL_HIDH_SetReport(hidContext,mainChannel, trans.u.data, dataLenTrans, reportTypeTrans, &(trans.transaction));
		    					UI_DISPLAY(("APP_HIDH_SetReport returned %s", pBT_Status(status)));
						}
					else UI_DISPLAY(("APP_HIDH_SetReport can't be executed"));
			}
		else Report(("Can't parse the 2nd args"));	
		}
	else Report(("Can't parse the 1st args"));	
}


/*---------------------------------------------------------------------------
 *            APP_HIDH_GetReport
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends an report request to the HID device..
 *
 * Return:    void
 *
 */
void APP_HIDH_GetReport(void)
{
	BtStatus status;
	U8 string[30]={0};
	U16 bufferSize;
	HidReportType reportType;
	BOOL useId;
	U8 reportId;

	if ((LINE_PARSER_GetNextU16(&bufferSize, FALSE) == LINE_PARSER_STATUS_SUCCESS))
		{
		if((LINE_PARSER_GetNextStr(string, LINE_PARSER_MAX_STR_LEN) == LINE_PARSER_STATUS_SUCCESS))
			{
			if((LINE_PARSER_GetNextBool(&useId) == LINE_PARSER_STATUS_SUCCESS))
				{
				if((LINE_PARSER_GetNextU8(&reportId, FALSE) == LINE_PARSER_STATUS_SUCCESS))
					{
						if (OS_StrCmp( "other\0", (const char *)string) == 0)
							{
								reportType = 0;
							}
						else
							{
								if (OS_StrCmp( "input\0", (const char *)string) == 0)
								{
									reportType = 1;
								}
								else
								{
									if (OS_StrCmp( "output\0", (const char *)string) == 0)
									{
										reportType = 2;
									}
									else
									{
										if (OS_StrCmp( "feature\0", (const char *)string) == 0)
										{
											reportType = 3;
										}
										else
										{
											Report(("APP_HIDH_GetReport()  parameter 2 not valid"));
										}
									}
								}
							}
						status = BTL_HIDH_GetReport(hidContext,mainChannel, bufferSize, reportType, useId, reportId, &(trans.transaction));
			    			UI_DISPLAY(("APP_HIDH_GetReport returned %s", pBT_Status(status)));
					}
				else Report(("Can't parse the 4th args"));	
				}
			else Report(("Can't parse the 3rd args"));	
			}
		else Report(("Can't parse the 2nd args"));	
		}
	else Report(("Can't parse the 1st args"));		
}

/*---------------------------------------------------------------------------
 *            APP_HIDH_SendInterrupt
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends an interrupt (report) to the remote device .
 *
 * Return:    void
 *
 */
void APP_HIDH_SendInterrupt(void)
{
	BtStatus status;
	U8 string[30]={0};
	U8 data[30]={0};
	U16 dataLen;
	HidReportType reportType;
	
	if ((LINE_PARSER_GetNextStr(data, LINE_PARSER_MAX_STR_LEN) == LINE_PARSER_STATUS_SUCCESS))
		{
			if((LINE_PARSER_GetNextStr(string, LINE_PARSER_MAX_STR_LEN) == LINE_PARSER_STATUS_SUCCESS))
				{
				if (OS_StrCmp( "other\0", (const char *)string) == 0)
							{
								reportType = 0;
							}
						else
							{
								if (OS_StrCmp( "input\0", (const char *)string) == 0)
								{
									reportType = 1;
								}
								else
								{
									if (OS_StrCmp( "output\0", (const char *)string) == 0)
									{
										reportType = 2;
									}
									else
									{
										if (OS_StrCmp( "feature\0", (const char *)string) == 0)
										{
											reportType = 3;
										}
										else
										{
											Report(("APP_HIDH_SendInterrupt()  parameter 2 not valid"));
										}
									}
								}
							}
						
				dataLen=OS_StrLen((const char *)data);
				if(dataLen < MAX_DATA_LEN)
					{
					 	BTHAL_UTILS_MemCopy(inter.data, data, dataLen);
						status = BTL_HIDH_SendInterrupt(hidContext, mainChannel, inter.data, dataLen, reportType, &(inter.interrupts));
					}
				else UI_DISPLAY(("APP_HIDH_SendInterrupt can't be executed"));
				}
			else Report(("Can't parse the 2nd args"));
		}
	else Report(("Can't parse the 1st args"));	
}



/*---------------------------------------------------------------------------
 *            APP_HID_SetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets security level for the given HID context.
 *
 * Return:    void
 *
 */
void APP_HID_SetSecurityLevel()
{
	BtStatus status;
	BOOL retVal = FALSE;
	BtSecurityLevel newSecurityLevel = securityLevel;
	LINE_PARSER_STATUS parserStatus;
	
	parserStatus = LINE_PARSER_GetNextU8(&newSecurityLevel, TRUE);
	
	if (parserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("HID: Can't read security level. Ignoring command"));
		return ;
	}
	
	status = BTL_HID_SetSecurityLevel(hidContext, &newSecurityLevel);
	Report(("BTL_HID_SetSecurityLevel returned :%s",pBT_Status(status)));
	UI_DISPLAY(("APP_HIDH_SetSecurityLevel returned %s", pBT_Status(status)));
}

/*---------------------------------------------------------------------------
 *            APP_Hid_GetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Gets security level for the given PBAPS context.
 *
 * Return:    void
 *
 */
void APP_HID_GetSecurityLevel(void)
{
	BtStatus status;
	BtSecurityLevel level;
	
	status = BTL_HID_GetSecurityLevel(hidContext, &level);
	
    	UI_DISPLAY(("APP_HIDH_GetSecurityLevel returned %s", pBT_Status(status)));

	switch (level)
	{
		case 0:
			UI_DISPLAY(("Security level: BSL_NO_SECURITY"));
			break;
		case 1:
			UI_DISPLAY(("Security level: BSL_AUTHENTICATION_IN"));
			break;
		case 2:
			UI_DISPLAY(("Security level: BSL_AUTHORIZATION_IN"));
			break;
		case 4:
			UI_DISPLAY(("Security level: BSL_ENCRYPTION_IN"));
			break;
		case 3:
			UI_DISPLAY(("Security level: BSL_AUTHENTICATION_IN \n and BSL_AUTHORIZATION_IN"));
			break;
		case 5:
			UI_DISPLAY(("Security level: BSL_AUTHENTICATION_IN \n and BSL_ENCRYPTION_IN"));
			break;
		case 6:
			UI_DISPLAY(("Security level: BSL_AUTHORIZATION_IN \n and BSL_ENCRYPTION_IN"));
			break;
		case 7:
			UI_DISPLAY(("Security level: BSL_AUTHENTICATION_IN \n and BSL_AUTHORIZATION_IN \n and BSL_ENCRYPTION_IN"));
			break;
		default:
			UI_DISPLAY(("Security level Defined: %d", level));
			break;
			
	}
}


/*---------------------------------------------------------------------------
 *            APP_Hid_GetNumChannels
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function isn't a part of the HID API
 *			 Display the number of channels opened
 *
 * Return:    void
 *
 */
void APP_HID_GetNumChannels(void)
{
	NumChannelId=manageChannelId(mainChannel, ChannelIdIndex, TRUE);
	UI_DISPLAY(("  Channels ID opened : %d", NumChannelId));
}


/*---------------------------------------------------------------------------
 *            APP_Hid_GetNumChannels
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function isn't a part of the HID API
 *			Set the active channel
 *
 * Return:    void
 *
 */
void APP_HID_SetNumChannels(void)
{
	if (LINE_PARSER_GetNextU32(&mainChannel, FALSE) != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("APP_HID_SetNumChannels() failed "));	
		return;
	}
	else
	{
		UI_DISPLAY(("  Active channel is : %d", mainChannel));
	}
}

/*---------------------------------------------------------------------------
 *            APP_HIDH_CallBack
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes HID  events.
 *
 * Return:    void
 *
 */
static void APP_HIDH_CallBack(const BtlHidEvent *event)
{
       HidTransaction *trans;
	BtStatus status;	   
	U8 updateChannelID[40]={0};
	HidCallbackParms *parms = (HidCallbackParms *) event->hidEvent;

	mainChannel=event->channelId;  
	Report(("APP_HIDH_CallBack: Channel ID reveived:%d",mainChannel));
	   
    	switch (parms->event) 
    		{
		    case HIDEVENT_OPEN_IND:
			if (parms->status == BT_STATUS_FAILED){
		            UI_DISPLAY(("Could Not accept the incoming connection"));
		        } else {
		            UI_DISPLAY(("Open request received"));
		        }
		        break;
		    case HIDEVENT_OPEN:
		        UI_DISPLAY(("App - Connection opened"));
				
			 /* Checks the number of opened channels ID*/
	 		 NumChannelId=manageChannelId(event->channelId,ChannelIdIndex, TRUE);
	 		 sprintf((char *)updateChannelID, "Channels ID opened : %d", NumChannelId);
        		 UI_DISPLAY(((const char *)updateChannelID));
			
		        break;
		    case HIDEVENT_CLOSE_IND:
		        UI_DISPLAY(("Close indication received"));
		        break;
		    case HIDEVENT_CLOSED:
		        UI_DISPLAY(("Connection closed"));

			 /* Checks the number of opened channels ID*/
	 		 NumChannelId=manageChannelId(event->channelId, ChannelIdIndex, FALSE);
	 		 if(NumChannelId != 0)
	 		 {
	 			sprintf((char *)updateChannelID, "  Channels ID opened : %d", NumChannelId);
        			UI_DISPLAY(((const char *)updateChannelID));
	 		 }
	 		 else
	 			UI_DISPLAY(("  No channel ID opened"));
		        break;
		    case HIDEVENT_QUERY_CNF:
			if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_DEVICE_SUBCLASS) {
           			 TypeOfDevice[mainChannel] = parms->ptrs.queryRsp->deviceSubclass;
        		}
				
		        UI_DISPLAY(("Received SDP Query Response"));
		        if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_DEVICE_RELEASE) {
		            UI_DISPLAY(("    Device Release: %04X", parms->ptrs.queryRsp->deviceRelease));
		        }
		        if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_PARSER_VERSION) {
		            UI_DISPLAY(("    Parser Version: %04X", parms->ptrs.queryRsp->parserVersion));
		        }
		        if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_DEVICE_SUBCLASS) {
		            UI_DISPLAY(("    Device Subclass: %02X", parms->ptrs.queryRsp->deviceSubclass));
		        }
		        if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_COUNTRY_CODE) {
		            UI_DISPLAY(("    Country Code: %02X", parms->ptrs.queryRsp->countryCode));
		        }
		        if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_VIRTUAL_CABLE) {
		            if (parms->ptrs.queryRsp->virtualCable) {
		                UI_DISPLAY(("    Virtual Cable Support"));
		            } else {
		                UI_DISPLAY(("    No Virtual Cable Support"));
		            }
		        }
		        if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_RECONNECT_INIT) {
		            if (parms->ptrs.queryRsp->reconnect) {
		                UI_DISPLAY(("    Initiates Reconnect"));
		            } else {
		                UI_DISPLAY(("    Does Not Initiates Reconnect"));
		            }
		        }
		        if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_DESCRIPTOR_LIST) {
		            UI_DISPLAY(("    Descriptor Data Len: %04X", parms->ptrs.queryRsp->descriptorLen));
		        }
		        if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_SDP_DISABLE) {
		            if (parms->ptrs.queryRsp->sdpDisable) {
		                UI_DISPLAY(("    SDP Disabled During Conn"));
		            } else {
		                UI_DISPLAY(("    SDP Available During Conn"));
		            }
		        }
		        if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_BATTERY_POWER) {
		            if (parms->ptrs.queryRsp->batteryPower) {
		                UI_DISPLAY(("    Battery Powered"));
		            } else {
		                UI_DISPLAY(("    Power Supply"));
		            }
		        }
		        if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_REMOTE_WAKE) {
		            if (parms->ptrs.queryRsp->remoteWakeup) {
		                UI_DISPLAY(("    Supports Remote Wakeup"));
		            } else {
		                UI_DISPLAY(("    Does Not Support Remote Wakeup"));
		            }
		        }
		        if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_PROFILE_VERSION) {
		            UI_DISPLAY(("    Profile Version: %04X", parms->ptrs.queryRsp->profileVersion));
		        }
		        if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_SUPERV_TIMEOUT) {
		            UI_DISPLAY(("    Supervision Timeout: %04X", parms->ptrs.queryRsp->supervTimeout));
		        }
		        if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_NORM_CONNECTABLE) {
		            if (parms->ptrs.queryRsp->normConnectable) {
		                UI_DISPLAY(("    Normally Connectable"));
		            } else {
		                UI_DISPLAY(("    Not Normally Connectable"));
		            }
		        }
		        if (parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_BOOT_DEVICE) {
		            if (parms->ptrs.queryRsp->bootDevice) {
		                UI_DISPLAY(("    Boot Device"));
		            } else {
		                UI_DISPLAY(("    No Boot Protocol"));
		            }
		        }
		        break;
		    case HIDEVENT_TRANSACTION_IND:
		        UI_DISPLAY(("Transaction Received:"));
		        trans = parms->ptrs.trans;
		        switch (trans->type) {
		        case HID_TRANS_CONTROL:
		            UI_DISPLAY(("    Control:"));
		            if (trans->parm.control == HID_CTRL_VIRTUAL_CABLE_UNPLUG) {
		                UI_DISPLAY(("        Control Function Received: VIRTUAL CABLE UNPLUG"));
		            } else {
		                UI_DISPLAY(("        Unknown Control Function"));
		            }
		        }
		        break;
		    case HIDEVENT_TRANSACTION_RSP:
		        UI_DISPLAY(("Transaction Response"));
		        trans = parms->ptrs.trans;
		        switch (trans->type) {
		        case HID_TRANS_GET_REPORT_RSP:
		            UI_DISPLAY(("    Get Report Response, Report data len = %d", 
		                       trans->parm.report->dataLen));
		            break;
		        case HID_TRANS_GET_PROTOCOL_RSP:
		            UI_DISPLAY(("    Get Protocol Response, Protocol = %s",
		                       pHidProtocol(trans->parm.protocol)));
		            break;
		        case HID_TRANS_GET_IDLE_RATE_RSP:
		            UI_DISPLAY(("    Get Idle Rate Response, Idle Rate = %d",
		                       trans->parm.idleRate));
		            break;
		        }
		        break;
		    case HIDEVENT_TRANSACTION_COMPLETE:
		        UI_DISPLAY(("Transaction complete, status = %s", 
		                   pBT_Status(parms->status)));
		        trans = parms->ptrs.trans;
		        switch (trans->type) {
		        case HID_TRANS_GET_REPORT:
		            UI_DISPLAY(("    Get Report, result = %s", 
		                       pResultCode(trans->resultCode)));
		            break;
		        case HID_TRANS_GET_PROTOCOL:
		            UI_DISPLAY(("    Get Protocol, result = %s", 
		                       pResultCode(trans->resultCode)));
		            break;
		        case HID_TRANS_GET_IDLE_RATE:
		            UI_DISPLAY(("    Get Idle Rate, result = %s", 
		                       pResultCode(trans->resultCode)));
		            break;
		        case HID_TRANS_SET_REPORT:
		            UI_DISPLAY(("    Set Report, result = %s", 
		                       pResultCode(trans->resultCode)));
		            break;
		        case HID_TRANS_SET_PROTOCOL:
		            UI_DISPLAY(("    Set Protocol, result = %s", 
		                       pResultCode(trans->resultCode)));
		            break;
		        case HID_TRANS_SET_IDLE_RATE:
		            UI_DISPLAY(("    Set Idle Rate, result = %s", 
		                       pResultCode(trans->resultCode)));
		            break;
		        }
		        break;
		    case HIDEVENT_INTERRUPT:
		        switch(TypeOfDevice[mainChannel])
	 		 {
				 case 64:
				 UI_DISPLAY(("Kbd:%X",TypeOfDevice[mainChannel]));
				 break;
				 case 128:
				 UI_DISPLAY(("Mouse:%X",TypeOfDevice[mainChannel]));
				 break;
				 case 192:
				 UI_DISPLAY(("Combo:%X",TypeOfDevice[mainChannel]));
				 break;
				 default:
				 UI_DISPLAY(("Undetermined device:%X",TypeOfDevice[mainChannel]));
				 break;
	 		 }
			 printOuput(parms->ptrs.intr->dataLen, parms->ptrs.intr->data);
		        break;
		    case HIDEVENT_INTERRUPT_COMPLETE:
		        UI_DISPLAY(("Interrupt complete"));
		        
		}
    }

static char *pHidProtocol(HidProtocol Protocol)
{
    switch (Protocol) {
    case HID_PROTOCOL_REPORT:
        return "HID_PROTOCOL_REPORT";
    case HID_PROTOCOL_BOOT:
        return "HID_PROTOCOL_BOOT";
    default:
        return "UNKNOWN";
    }
}

static char * pResultCode(HidResultCode result)
{
    switch (result) {
    case HID_RESULT_SUCCESS:
        return "HID_RESULT_SUCCESS";
    case HID_RESULT_NOT_READY:
        return "HID_RESULT_NOT_READY";
    case HID_RESULT_INVALID_REPORT_ID:
        return "HID_RESULT_INVALID_REPORT_ID";
    case HID_RESULT_UNSUPPORTED_REQUEST:
        return "HID_RESULT_UNSUPPORTED_REQUEST";
    case HID_RESULT_INVALID_PARAMETER:
        return "HID_RESULT_INVALID_PARAMETER";
    case HID_RESULT_UNKNOWN:
        return "HID_RESULT_UNKNOWN";
    case HID_RESULT_FATAL:
        return "HID_RESULT_FATAL";
    default:
        return "UNKNOWN";
    }

}

static void printOuput(U16 max , U8  *data)
{
	int cpt=0;
	U8 outPut[100]={0};
	U8 outPut2[100]={0};
	for(cpt=0; cpt <max; cpt++)
		{
		sprintf((char *)outPut, "%X", data[cpt]);
		strcat( outPut2, outPut);
		}
	UI_DISPLAY(("Data received :%s", outPut2));
}

static U32 manageChannelId(BtlHidChannelId Channel, BOOL *ChannelIdIndex, BOOL channelOpened)
{
	U32 index=0;
	U32 numItems=0;
	if(Channel<MAX_CHANNEL_ID)
	{
		*(ChannelIdIndex+Channel)=channelOpened;
	}
	
	for(index=0; index<MAX_CHANNEL_ID; index++)
	{
		if(*(ChannelIdIndex+index)==TRUE)
			numItems++;
	}
	return numItems;
}

#else  /*BTL_CONFIG_HIDH == BTL_CONFIG_ENABLED*/

void HIDA_Init(void)
{
	Report(("HIDA_Init -BTL_CONFIG_HIDH is disabled "));
}
void HIDA_Deinit(void)
{
	Report(("HIDA_Deinit  - BTL_CONFIG_HIDH is disabled"));
}

void HIDA_ProcessUserAction(U8 *msg)
{
	msg=msg;
	Report(("HID_APP is disabled via BTL_CONFIG."));

}



#endif /* BTL_CONFIG_HIDH == BTL_CONFIG_ENABLED */

