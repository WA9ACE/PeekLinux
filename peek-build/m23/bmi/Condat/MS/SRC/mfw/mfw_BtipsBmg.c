/* =========================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================== */
/*
 $Project name: Basic Bluetooth MMI
 $Project code:
 $Module:   Bluetooth BMG MFW
 $File:       Mfw_Btips.c
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS BMG APPlication functionality.

********************************************************************************
 $History: Mfw_Btips.c

  26/06/07 Sasken original version

 $End

*******************************************************************************/




/*******************************************************************************

                                Include files

*******************************************************************************/

#define ENTITY_MFW
/* includes */

#include <string.h>
#ifdef FF_MMI_BTIPS_APP

/* BTIPS Includes */
#include "me.h"
#include "osapi.h"
#include "btl_common.h"
#include "bthal_btdrv.h"
#include "bthal_pm.h"
#include "bttypes.h"
#include "goep.h"
#include "bpp.h"
#include "Debug.h"

/* BTL includes */
#include "btl_bmg.h"

#endif
#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_icn.h"	/* included for mfw_mnu.h */
#include "mfw_mnu.h"
#include "mfw_tim.h"
#include "mfw_kbd.h"
#include "mfw_sat.h" /* included for MmiDummy.h, included for MmiMenu.h */

#include "Mfw_Btips.h"
#include "Mfw_BtipsBmg.h"


#ifdef FF_MMI_BTIPS_APP

#define BTL_BMG_MAX_PIN_LEN			16
#define BTL_BMG_SEARCH_ALL				0
#define BTL_BMG_SEARCH_AUDIO_MAJOR	1
#define PME_APP_NAME 					"Locosto Phone"



//extern BtlA2dpContext 	  *btlA2dpContext ;
//extern BtlFtpsContext	 *ftpsContext;
/* Internal functions prototypes */
void btips_bmgCallback (const BtlBmgEvent *);
extern UBYTE g_device_name[BT_MAX_REM_DEV_NAME];
extern BOOL g_Outgoing;

static BtlBmgContext *mfwBtipsBmgContext;

/*---------------------------------------------------------------------------
 *            mfw_btips_bmgGetDeviceName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send PIN code.		
 *
 * Return:    TRUE if name is available. Else convert the BD_ADDR itself to a string.
 *
 */
BOOL mfw_btips_bmgGetDeviceName(const BD_ADDR *bdAddr, U8 *name)
{
	BtStatus btStatus;
	BtlBmgDeviceRecord record;
	TRACE_FUNCTION("mfw_btips_bmgGetDeviceName");
	btStatus = BTL_BMG_FindDeviceRecord(mfwBtipsBmgContext, bdAddr, &record);
	if(BT_STATUS_SUCCESS == btStatus)
	{

		strcpy(name, record.name);
	}
	else
	{
		bdaddr_ntoa(bdAddr, name);
	}
	return TRUE;
}
//26-07-07
/*---------------------------------------------------------------------------
 *            mfw_btips_bmgSetDeviceRecord
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send PIN code.		
 *
 * Return:    TRUE if name is available. Else convert the BD_ADDR itself to a string.
 *
 */
BOOL mfw_btips_bmgSetDeviceNameInRecord(const BD_ADDR *bdAddr, UBYTE *name)
{
	BtStatus btStatus;
	BtlBmgDeviceRecord record;
	TRACE_FUNCTION("mfw_btips_bmgGetDeviceName");
	btStatus = BTL_BMG_FindDeviceRecord(mfwBtipsBmgContext, bdAddr, &record);
	if(BT_STATUS_SUCCESS == btStatus)
	{

		OS_MemCopy(record.name, name, BT_MAX_REM_DEV_NAME);
		BTL_BMG_SetDeviceRecord(mfwBtipsBmgContext, &record);

	}
	else
	{
		bdaddr_ntoa(bdAddr, name);
	}
	return TRUE;
}


BOOL mfw_btips_bmgGetDeviceState (const BD_ADDR *bdAddr, BtRemDevState *state)
{
	TRACE_FUNCTION("mfw_btips_bmgGetDeviceState");
	BTL_BMG_GetRemDevState (mfwBtipsBmgContext, bdAddr, state);
	return TRUE;
}

/*---------------------------------------------------------------------------
 *            mfw_btips_bmgSendPin
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send PIN code.		
 *
 * Return:    TRUE if send PIN code is successful, FALSE otherwise.
 *
 */
BOOL mfw_btips_bmgSendPin (const BD_ADDR	*bdAddr, 
								const U8 				*pin,
								const U8 				len)
{
	BtStatus btStatus;
	
	btStatus = BTL_BMG_PinReply(mfwBtipsBmgContext, bdAddr, pin, len, BPT_SAVE_TRUSTED);
	return mfw_btips_checkStatus(btStatus);
}

/*---------------------------------------------------------------------------
 *            mfw_btips_bmgSendHciCommand
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send an HCI Command.		
 *
 * Return:    TRUE if command was sent successfully, FALSE otherwise.
 *
 */
BOOL mfw_btips_bmgSendHciCommand (const U16 		hciCommand,
											const U8 			parmsLen, 
											const U8 			*parms, 
											const U8 			event)
{
	BtStatus btStatus;

	TRACE_FUNCTION_P1("mfw_btips_bmgSendHciCommand %x", hciCommand);
	btStatus = BTL_BMG_SendHciCommand(mfwBtipsBmgContext, hciCommand, parmsLen, parms, event);
	return mfw_btips_checkStatus(btStatus);
}


BOOL mfw_btips_bmgBond(const BD_ADDR 		*bdAddr,
							const U8 				*pin, 
							const U8 				len)
{
	BtStatus btStatus;
	btStatus = BTL_BMG_Bond(mfwBtipsBmgContext, bdAddr, pin, len, BPT_SAVE_TRUSTED);
	return mfw_btips_checkStatus(btStatus);
}

BOOL mfw_btips_bmgCancelBond(const BD_ADDR 		*bdAddr)
{
	BtStatus btStatus;
	TRACE_FUNCTION("mfw_btips_bmgCancelBond");
	btStatus = BTL_BMG_CancelBond(mfwBtipsBmgContext, bdAddr);
	return mfw_btips_checkStatus(btStatus);

}
BOOL mfw_btips_bmgGetCodMajorValue(U8* CODMajorStr, U32 *CODMajor)
{
	BOOL	result;
	#if 0
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
#endif
	return result;
}

BOOL mfw_btips_bmgGetLAPValue(U8* lapStr, U32 *lap)
{
	BOOL	result;
	#if 0
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
#endif
	return result;
}

/*---------------------------------------------------------------------------
 *            mfw_btips_bmgSearch
 *---------------------------------------------------------------------------
 *
 * Synopsis: Starts search process.	
 *
 * Return:    TRUE if search process is successfull, FALSE otherwise.
 *
 */
BOOL mfw_btips_bmgCancelSearch (void)
{
	BtStatus btStatus = BT_STATUS_FAILED;

	TRACE_FUNCTION ("mfw_btips_bmgCancelSearch()");

	btStatus = BTL_BMG_CancelSearch(mfwBtipsBmgContext);
	TRACE_FUNCTION_P1("MFW_BTIPS_BMG :BTL_BMG_CancelSearch returned btStatus: %s", pBT_Status(btStatus));
			
	return mfw_btips_checkStatus(btStatus);
}

/*---------------------------------------------------------------------------
 *            mfw_btips_bmgSearch
 *---------------------------------------------------------------------------
 *
 * Synopsis: Starts search process.	
 *
 * Return:    TRUE if search process is successfull, FALSE otherwise.
 *
 */
BOOL mfw_btips_bmgSearch (BOOL searchType)
{
	BtStatus status = BT_STATUS_FAILED;
	BtlBmgCodFilter CODFilter;
	BtlBmgCodFilter		*CODFilterPtr = 0;
//	U8					lapStr[11];
	BOOL				performNameRequest = TRUE;
	U8					maxResp = 0;
//	U8					CODMajorStr[21];
//	U32					CODMajor;

	TRACE_FUNCTION_P1 ("mfw_btips_bmgSearch TYPE = %d", searchType);
	
	switch(searchType)
	{	
		case BTIPS_GENERIC_DEVICE_SEARCH:		
		status = BTL_BMG_SearchByCod(mfwBtipsBmgContext, 
						BTL_BMG_SEARCH_REGULAR, 
						BT_IAC_GIAC, 
						BT_INQ_TIME_GAP100, 
						MAX_RESPONSES,
						0, 
						performNameRequest, 
						SDP_SERVICE_NONE);
			break;
		case BTIPS_A2DP_DEVICE_SEARCH:
			CODFilter.codFilterMask = BTL_BMG_SEARCH_COD_FILTER_SERVICE_CLS|BTL_BMG_SEARCH_COD_FILTER_SERVICE_CLS;
			CODFilter.serviceClass = COD_RENDERING|COD_AUDIO;
			CODFilter.majorDeviceClass = COD_MAJOR_AUDIO;
			status = BTL_BMG_SearchByCod(mfwBtipsBmgContext, 
							BTL_BMG_SEARCH_REGULAR, 
							BT_IAC_GIAC, 
							BT_INQ_TIME_GAP100, 
							MAX_RESPONSES,
							&CODFilter, 
							performNameRequest, 
							SDP_SERVICE_NONE);
			break;

		case BTIPS_OPP_DEVICE_SEARCH:
			CODFilter.codFilterMask = BTL_BMG_SEARCH_COD_FILTER_SERVICE_CLS;
			CODFilter.serviceClass = COD_OBJECT_TRANSFER;
			status = BTL_BMG_SearchByCod(mfwBtipsBmgContext, 
							BTL_BMG_SEARCH_REGULAR, 
							BT_IAC_GIAC, 
							BT_INQ_TIME_GAP100, 
							MAX_RESPONSES,
							&CODFilter, 
							performNameRequest, 
							SDP_SERVICE_NONE);

			break;
		case BTIPS_HSHF_DEVICE_SEARCH:
		CODFilter.codFilterMask = BTL_BMG_SEARCH_COD_FILTER_MAJOR_DEV_CLS;
		CODFilter.majorDeviceClass = COD_MAJOR_AUDIO;
		status = BTL_BMG_SearchByCod(mfwBtipsBmgContext, 
						BTL_BMG_SEARCH_REGULAR, 
						BT_IAC_GIAC, 
						BT_INQ_TIME_GAP100, 
						MAX_RESPONSES,
							&CODFilter, 
						performNameRequest, 
						SDP_SERVICE_NONE);
			break;

	}


	mfw_BtipsReport(("MFW_BTIPS_BMG :BTL_BMG_SearchByCod returned status: %s", pBT_Status(status)));
	return mfw_btips_checkStatus(status);
}

/*---------------------------------------------------------------------------
 *            mfw_btips_bmgGetLocalDeviceName
 *---------------------------------------------------------------------------
 *
 * Synopsis: Extract the currently set local name 	
 * 
 * Return:   
 *
 */
BOOL mfw_btips_bmgGetLocalDeviceName(U8 *name)
{
	BtStatus btStatus;
	U8 len = 0;
	btStatus = BTL_BMG_GetLocalDeviceName(mfwBtipsBmgContext, name, &len);
	return mfw_btips_checkStatus(btStatus);
	
}


/*---------------------------------------------------------------------------
 *            mfw_btips_bmgSetLocalDeviceName
 *---------------------------------------------------------------------------
 *
 * Synopsis: Set local name (Send a null terminated string) of our device.		
 *
 * Return:    TRUE if setting local name is successfull, FALSE otherwise.
 *
 */
BOOL mfw_btips_bmgSetLocalDeviceName (U8 *name)
{
	U8 len = 0;

	BtStatus btStatus = BT_STATUS_FAILED;
	len = (U8)(OS_StrLen((const char *)(name)));
	name[len]=0;
	btStatus = BTL_BMG_SetLocalDeviceName(mfwBtipsBmgContext, (U8 *)(name), len+1);
	return  mfw_btips_checkStatus(btStatus);
}

/*---------------------------------------------------------------------------
 *            mfw_btips_bmgSetPhoneVisibility
 *---------------------------------------------------------------------------
 *
 * Synopsis: Set Accessibility of the BT device.		
 *
 * Return:    .
 *
 */
BOOL mfw_btips_bmgSetPhoneVisibility(BOOL status)
{
	BtStatus btStatus;

	BtAccessibleMode modeNC;
	//BtAccessModeInfo infoNC;
	BtAccessibleMode modeC;
	//BtAccessModeInfo infoC;

	TRACE_FUNCTION("mfw_btips_bmgSetPhoneVisibility");

	btStatus = BTL_BMG_GetAccessibleMode(mfwBtipsBmgContext, 
									&modeNC,
									0,
									&modeC, 
									0);

	TRACE_EVENT_P2("BTL_BMG_GetAccessibleMode %d, %d", modeNC, modeC);
	
	if(TRUE == status)
	{
		modeNC = BAM_GENERAL_ACCESSIBLE;
		modeC = BAM_GENERAL_ACCESSIBLE;
	}
	else
	{
		modeNC = BAM_CONNECTABLE_ONLY;
		modeC = BAM_CONNECTABLE_ONLY;
	}

	btStatus = BTL_BMG_SetAccessibleMode(mfwBtipsBmgContext, 
									&modeNC,
									0,
									&modeC, 
									0);

	TRACE_EVENT_P2("BTL_BMG_SetAccessibleMode %d, %d", modeNC, modeC);

	return mfw_btips_checkStatus(btStatus);

}

/*---------------------------------------------------------------------------
 *            mfw_btips_bmgSetPhoneVisibility
 *---------------------------------------------------------------------------
 *
 * Synopsis: Set Accessibility of the BT device.		
 *
 * Return:    .
 *
 */
BOOL mfw_btips_bmgGetPhoneVisibility(void)
{
	BtStatus btStatus;

	BtAccessibleMode modeNC;

	BtAccessibleMode modeC;


	TRACE_FUNCTION("mfw_btips_bmgSetPhoneVisibility");

	btStatus = BTL_BMG_GetAccessibleMode(mfwBtipsBmgContext, 
									&modeNC,
									0,
									&modeC, 
									0);
	
	if(modeNC == BAM_GENERAL_ACCESSIBLE)
		return TRUE;
	
	return FALSE;
	
}
/*---------------------------------------------------------------------------
 *            mfw_btips_bmgDeviceDelete
 *---------------------------------------------------------------------------
 *
 * Synopsis: Delete a record with the given "bdAddr" from the device database.		
 *
 * Return:    .
 *
 */
BOOL mfw_btips_bmgDeviceDelete(const BD_ADDR *bdAddr)
{
	BtStatus btStatus;

 	btStatus = BTL_BMG_DeleteDeviceRecord(mfwBtipsBmgContext, bdAddr);
	 mfw_BtipsReport(("BTL_BMG_GetNumberOfDeviceRecords() returned %s.\n", pBT_Status(btStatus)));
	return mfw_btips_checkStatus(btStatus);
}

/*---------------------------------------------------------------------------
 *            mfw_btips_bmg_getPairedDevices
 *---------------------------------------------------------------------------
 *
 * Synopsis: Enumerate the device database. 		
 *
 * Return:    .
 *
 */
BOOL mfw_btips_bmg_getPairedDevices(Bmg_DeviceList *pairedDeviceList, U32 *count)
{
	BtStatus btStatus;
	U32 		index, numOfDevicesDB;
    	BtlBmgDeviceRecord deviceRecord;
	*count = 0;

	TRACE_FUNCTION ("mfw_btips_bmg_getPairedDevices()");

	  /* Checks if the DB is empty or not*/
	 btStatus= BTL_BMG_GetNumberOfDeviceRecords(mfwBtipsBmgContext,  &numOfDevicesDB);
	 mfw_BtipsReport(("BTL_BMG_GetNumberOfDeviceRecords() returned %s.\n", pBT_Status(btStatus)));
	if(numOfDevicesDB!=0)
	{
	/*Current limitation is to have a display of only BT_MAX_DEVICE_LIST devices*/
		for (index=0;index <numOfDevicesDB && index <= BT_MAX_DEVICE_LIST; index++)
		{
			btStatus=BTL_BMG_EnumDeviceRecords(mfwBtipsBmgContext,  index, &deviceRecord);
			mfw_BtipsReport(("BTL_BMG_EnumDeviceRecords() returned %s for %d: %s", pBT_Status(btStatus), index, deviceRecord.name));
			if (btStatus == BT_STATUS_SUCCESS)
			{
				OS_MemCopy((U8*)&pairedDeviceList[index].bdAddr, (U8*)&deviceRecord.bdAddr, sizeof(BD_ADDR));
				strcpy(pairedDeviceList[index].name, deviceRecord.name);
				pairedDeviceList[index].myIndex = index;
			}
		}
		*count = index;
		return TRUE;
	}
	return mfw_btips_checkStatus(btStatus);
}



/*---------------------------------------------------------------------------
 *            mfw_btips_bmg_getConnectedDevices
 *---------------------------------------------------------------------------
 *
 * Synopsis: Enumerate the device database. 		
 *
 * Return:    .
 *
 */
BOOL mfw_btips_bmg_getConnectedDevices(Bmg_ConnectedDeviceList *ConnectedDeviceList, U32 *count)
{
	BtStatus btStatus;
	U32 		index;//, numOfDevicesDB;
//    	BtlBmgDeviceRecord deviceRecord;
	U32 profile_count=0;
	BD_ADDR bdAddr;

	char name[BT_MAX_REM_DEV_NAME];
	U32 ServiceID;
	*count = 0;

	TRACE_FUNCTION ("mfw_btips_bmg_getConnectedDevices()");

	for (index=0;index <BTIPS_MAX_PROFILE_COUNT; index++)
	{	
		btStatus = mfw_btips_bmg_getConnectedDevicesForProfile(index,&bdAddr,name,&ServiceID);
		if (btStatus)
		{
			memset(&ConnectedDeviceList[*count].name,0,BT_MAX_REM_DEV_NAME*sizeof(UBYTE));
			memset(&ConnectedDeviceList[*count].bdAddr,0,sizeof(BD_ADDR));
			OS_MemCopy((U8*)&ConnectedDeviceList[*count].bdAddr,(U8*)&bdAddr, sizeof(BD_ADDR));
			strcpy(ConnectedDeviceList[*count].name, name);
			ConnectedDeviceList[*count].myIndex = *count;
			ConnectedDeviceList[*count].ServiceId= ServiceID;
			ServiceID = 0;
			(*count)++;
		}
		
	}	
	
	return mfw_btips_checkStatus(btStatus);
}

U32 mfw_btips_bmg_getConnectedDevicesCount(void)
{
	BtStatus btStatus;
	U32 		index, count=0;
	BD_ADDR bdAddr;

	char name[BT_MAX_REM_DEV_NAME];
	U32 ServiceID;


	TRACE_FUNCTION ("mfw_btips_bmg_getConnectedDevicesCount()");

	for (index=0;index <BTIPS_MAX_PROFILE_COUNT; index++)
	{	
		btStatus = mfw_btips_bmg_getConnectedDevicesForProfile(index,&bdAddr,name,&ServiceID);

		if (btStatus)
		{
			count++;
		}	
	}
	return count;
}

BOOL mfw_btips_bmg_getConnectedDevicesForProfile(U32 profile_num,  BD_ADDR *bdAddr, U8 *name, U32 *ServiceID)
{


	BOOL status;
	
	switch(profile_num)
		{

			case BTIPS_SDP_SERVICE_A2DP:
				status = mfw_btips_a2dpGetConnectedDevice(bdAddr, name);
				if(TRUE ==status)
					*ServiceID = BTIPS_SDP_SERVICE_A2DP;
				TRACE_EVENT_P1("mfw_btips_bmg_getConnectedDevicesForProfile A2DP  name =%s ",name);
				break;

		
			case BTIPS_SDP_SERVICE_VG:
				status=mfw_btips_vgGetConnectedDevice(bdAddr, name);
				if(TRUE ==status)
					*ServiceID = BTIPS_SDP_SERVICE_VG;

				TRACE_EVENT_P1("mfw_btips_bmg_getConnectedDevicesForProfile OPPC  name =%s ",name);
				break;

	

			case BTIPS_SDP_SERVICE_OPPC:
				status=mfw_btips_oppcGetConnectedDevice(bdAddr, name);
				if(TRUE ==status)
					*ServiceID = BTIPS_SDP_SERVICE_OPPC;
	
				TRACE_EVENT_P1("mfw_btips_bmg_getConnectedDevicesForProfile OPPC  name =%s ",name);
				break;

			case BTIPS_SDP_SERVICE_OPPS:
				
				status=mfw_btips_oppsGetConnectedDevice(bdAddr, name);
				if(TRUE ==status)
					*ServiceID = BTIPS_SDP_SERVICE_OPPS;

				TRACE_EVENT_P1("mfw_btips_bmg_getConnectedDevicesForProfile OPPS name =%s ",name);
				break;


			case BTIPS_SDP_SERVICE_FTPS:
								
				status=mfw_btips_ftpsGetConnectedDevice(bdAddr, name);
					if(TRUE ==status)
					*ServiceID = BTIPS_SDP_SERVICE_FTPS;
		
				TRACE_EVENT_P1("mfw_btips_bmg_getConnectedDevicesForProfile FTPS  name =%s ",name);
				break;


			default:
				break;
		}
	return status;
}

#if 0

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
	status = BTL_BMG_SetSecurityMode(mfwBtipsBmgContext, (SecLevel+1));
	mfw_BtipsReport(("MFW_BTIPS_BMG :BTL_BMG_SetSecurityMode returned status: %s", pBT_Status(status)));
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

	status = BTL_BMG_GetSecurityMode(mfwBtipsBmgContext, &CurrentSecLevel);
	mfw_BtipsReport(("MFW_BTIPS_BMG :APP_BMG_GetSecurityMode returned status: %s", pBT_Status(status)));
	if (status == BT_STATUS_SUCCESS)
	{
		mfw_BtipsReport(("MFW_BTIPS_BMG : Current security mode is: %d", (CurrentSecLevel-1)));
		return TRUE;
	}
		return FALSE;
}

#endif

/*---------------------------------------------------------------------------
 *            APP_BMG_AuthorizeDeviceReply
 *---------------------------------------------------------------------------
 *
 * Synopsis: Reply for authorization request of another device.		
 *
 * Return:    TRUE if Autorization reply is successfull, FALSE otherwise.
 *
 */
BOOL mfw_btips_bmgAuthorizeDeviceReply (BOOL auth, BD_ADDR *BdAddr)
{
	BtStatus btStatus = BT_STATUS_FAILED;
	U8 AuthorizationType;//, BD_array[BDADDR_NTOA_SIZE];
	AuthorizationType = auth? BTL_BMG_AUTHORIZED_NOT_TRUSTED: BTL_BMG_NOT_AUTHORIZED;
//	btStatus = BTL_BMG_AuthorizeDeviceReply(mfwBtipsBmgContext, &BdAddr, (BtlBmgAuthorizationType) (AuthorizationType));
	if (btStatus == BT_STATUS_SUCCESS)
	{
		return TRUE;
	}
		return FALSE;
}


/*---------------------------------------------------------------------------
 *            mfw_btips_bmgInit
 *---------------------------------------------------------------------------
 *
 * Synopsis: BMG application initialization.		
 *
 * Return:    NONE.
 *
 */
void mfw_btips_bmgInit (void)
{
	BtStatus  btStatus = BT_STATUS_FAILED;
	
	btStatus = BTL_BMG_Create(0, btips_bmgCallback, &mfwBtipsBmgContext);
	Assert(btStatus == BT_STATUS_SUCCESS);

	btStatus = BTL_BMG_SetEventForwardingMode(mfwBtipsBmgContext, BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
	Assert(btStatus == BT_STATUS_SUCCESS);
	
#if 0
	btStatus = BTL_BMG_SetClassOfDevice(mfwBtipsBmgContext, (COD_MAJOR_PHONE | COD_MINOR_PHONE_CELLULAR));
	Assert(btStatus == BT_STATUS_SUCCESS);
#endif
	
	mfw_BtipsReport(("BMGA_Init() complete.\n"));
}

/*---------------------------------------------------------------------------
 *            mfw_btips_bmgDeinit
 *---------------------------------------------------------------------------
 *
 * Synopsis: BMG application deinitialization.		
 *
 * Return:    NONE.
 *
 */
void mfw_btips_bmgDeinit(void)
{
	BtStatus btStatus = BT_STATUS_FAILED;

	btStatus = BTL_BMG_Destroy(&mfwBtipsBmgContext);
	Assert(btStatus == BT_STATUS_SUCCESS);
		
	mfw_BtipsReport(("BMGA_Deinit() complete.\n"));
}

/*---------------------------------------------------------------------------
 *            btips_bmgCallback
 *---------------------------------------------------------------------------
 *
 * Synopsis: Callback function to handle BMG profile events invoked in BTS context.	
 *
 * Return:    NONE.
 *
 */
void btips_bmgCallback (const BtlBmgEvent *bmgEvent)
{
	const BtEvent *event = bmgEvent->event;
	BtStatus btStatus = BT_STATUS_FAILED;
	char  bdAddr[BDADDR_NTOA_SIZE];
	char *name;
	T_BTIPS_MMI_IND	btips_mmi_ind;
	
	TRACE_FUNCTION_P1("btips_bmgCallback: %s", pME_Event(event->eType));
	btips_mmi_ind.mfwBtipsEventType = MFW_BTIPS_BMG_EVENT;
	btips_mmi_ind.data.bmgBtEvent.eventType = event->eType;
	btips_mmi_ind.data.bmgBtEvent.errCode =event->errCode;

	switch (event->eType)
	{ 

	case BTEVENT_LINK_CONNECT_IND:
		btStatus = BTL_BMG_GetRemoteDeviceName(mfwBtipsBmgContext, &event->p.remDev->bdAddr);
		break;
	case BTEVENT_HCI_INITIALIZED:
		btStatus = BTL_BMG_SetLocalDeviceName(mfwBtipsBmgContext, (U8 *)PME_APP_NAME, (U8)(sizeof(PME_APP_NAME)));		
		mfw_BtipsReport(("MFW_BTIPS_BMG :BTL_BMG_SetLocalDeviceName returned status: %s", pBT_Status(btStatus)));

		btStatus = BTL_BMG_SetDefaultLinkPolicy(mfwBtipsBmgContext, ((BLP_MASTER_SLAVE_SWITCH|BLP_HOLD_MODE|BLP_SNIFF_MODE|BLP_PARK_MODE|BLP_SCATTER_MODE)&(~BLP_MASK)), ((BLP_MASTER_SLAVE_SWITCH|BLP_HOLD_MODE|BLP_SNIFF_MODE|BLP_PARK_MODE|BLP_SCATTER_MODE)&(~BLP_MASK)));
		mfw_BtipsReport(("MFW_BTIPS_BMG :BTL_BMG_SetDefaultLinkPolicy returned status: %s", pBT_Status(btStatus)));

		btStatus = BTL_BMG_SetSecurityMode(mfwBtipsBmgContext, BSM_SEC_LEVEL_2);
		mfw_BtipsReport(("MFW_BTIPS_BMG :BTL_BMG_SetSecurityMode returned status: %s", pBT_Status(btStatus)));
		
		btipsNotifyMFW(btips_mmi_ind);
		break;
		
	case BTEVENT_HCI_DEINITIALIZED:		
		btipsNotifyMFW(btips_mmi_ind);
		break;
		

	case BTEVENT_HCI_FAILED:		
		btipsNotifyMFW(&btips_mmi_ind);
		break;

	case BTEVENT_PAIRING_COMPLETE:
		TRACE_EVENT_P2("Pairing with %s complete. status %s.\n", bdaddr_ntoa( &event->p.remDev->bdAddr, bdAddr), pHC_Status(event->errCode));
		OS_MemCopy((U8*)&btips_mmi_ind.data.bmgBtEvent.bdAddr, (U8*)&event->p.remDev->bdAddr, sizeof(BD_ADDR));
		btipsNotifyMFW(&btips_mmi_ind);
		break;

	case BTEVENT_BOND_RESULT:
			if (event->errCode != BEC_NO_ERROR) 
			{
				TRACE_EVENT_P2("Bond attempt to %s failed. Reason %s.", bdaddr_ntoa(&event->p.bdAddr, bdAddr), pHC_Status(event->errCode));
			}
			else 
			{
				TRACE_EVENT_P1("Bond to %s ended successfully", bdaddr_ntoa(&event->p.bdAddr, bdAddr));
			}

		OS_MemCopy((U8*)&btips_mmi_ind.data.bmgBtEvent.bdAddr, (U8*)&event->p.bdAddr, sizeof(BD_ADDR));

		btipsNotifyMFW(&btips_mmi_ind);
		break;

	case BTEVENT_BOND_CANCELLED:
		btipsNotifyMFW(&btips_mmi_ind);
		break;
		
	case BTEVENT_PIN_REQ:
		if (event->errCode == BT_STATUS_SUCCESS)
		{
			T_BTIPS_MMI_IND btipsMmiInd = {0};
			BtlBmgDeviceRecord record;


			btStatus=BTL_BMG_GetRemoteDeviceName(mfwBtipsBmgContext, &event->p.remDev->bdAddr);
						TRACE_EVENT_P1("BTEVENT_PIN_REQ btStatus %d",btStatus);
			/* Prompt the user for a PIN code */
			mfw_BtipsReport(("Please Enter PIN Code for %s", bdaddr_ntoa(&event->p.remDev->bdAddr, bdAddr)));
		}
		else
		{
			/* Cancel the PIN code request */
			mfw_BtipsReport ((("BMG: Pin code couldn't be displayed because of an error")));
		}
		OS_MemCopy((U8*)&btips_mmi_ind.data.bmgBtEvent.bdAddr, (U8*)&event->p.remDev->bdAddr, sizeof(BD_ADDR));
		btipsNotifyMFW(&btips_mmi_ind);
		break;

	case BTEVENT_AUTHORIZATION_REQ:
		TRACE_EVENT_P1("Authorization request received from %s",bdaddr_ntoa(&event->p.remDev->bdAddr, bdAddr));
		break;
		
	case BTEVENT_INQUIRY_RESULT:							
		mfw_BtipsReport(("Inquiry result arrived, BD address: %s", bdaddr_ntoa(&event->p.inqResult.bdAddr, bdAddr)));	
		break;

	case BTEVENT_SEARCH_COMPLETE:		
	case BTEVENT_SEARCH_CANCELLED:
		TRACE_EVENT_P1("Search %s ", ((event->eType == BTEVENT_SEARCH_COMPLETE) ? ("completed") : ("cancelled")));
		btipsNotifyMFW(&btips_mmi_ind);
		break;
		
	case BTEVENT_INQUIRY_COMPLETE:
	case BTEVENT_INQUIRY_CANCELED:
		TRACE_EVENT_P1("Inquiry %s ", ((event->eType == BTEVENT_INQUIRY_COMPLETE) ? ("completed") : ("cancelled")));
		//btipsNotifyMFW(&btips_mmi_ind);
		break;
		
	case BTEVENT_NAME_RESULT:

		if (event->errCode == BEC_NO_ERROR)
		{
			OS_MemCopy((U8*)&btips_mmi_ind.data.bmgBtEvent.bdAddr, (U8*)&event->p.meToken->p.name.bdAddr, sizeof(BD_ADDR));
			if (event->p.meToken->p.name.io.out.len == 0)
			{
				name =  "Unknown";						
				strcpy(btips_mmi_ind.data.bmgBtEvent.p.deviceName,  bdaddr_ntoa(&event->p.meToken->p.name.bdAddr, bdAddr));
			}
			else
			{
				name = (char *)(event->p.meToken->p.name.io.out.name);
				if (name)
				{
				}
				else
				{
					TRACE_EVENT("BTEVENT_NAME_RESULT Name = NULL");
				}
				strcpy(btips_mmi_ind.data.bmgBtEvent.p.deviceName,  name);
			}
			TRACE_EVENT_P2("Received name %s for device %s", name, bdaddr_ntoa(&event->p.meToken->p.name.bdAddr, bdAddr));
			btipsNotifyMFW(&btips_mmi_ind);
		}
		else
		{
			/* Record that the name request failed. Next time we'll skip it. */
            		mfw_BtipsReport(("BMG: Name request for %s failed, status %s.\n", 
            		bdaddr_ntoa(&event->p.meToken->p.name.bdAddr, bdAddr), 
            				pHC_Status(event->errCode)));
		}
		break;
		case BTEVENT_DISCOVER_SERVICES_RESULT :
			btips_mmi_ind.data.bmgBtEvent.p.discoveredServicesMask = event->p.discoveredServices.discoveredServicesMask;
			bdaddr_ntoa(&event->p.discoveredServices.bdAddr, bdAddr);
			OS_MemCopy((U8*)&btips_mmi_ind.data.bmgBtEvent.bdAddr , (U8*)&event->p.discoveredServices.bdAddr, sizeof(BD_ADDR));
			btips_mmi_ind.data.bmgBtEvent.p.discoveredServicesMask = event->p.discoveredServices.discoveredServicesMask;
			TRACE_EVENT_P2("Discover Services %x for %s",event->p.discoveredServices.discoveredServicesMask, bdAddr);
			btipsNotifyMFW(&btips_mmi_ind);
			break;
	default:
		break;
	}
}


/************************************************************************
	             BMG in ACI Context Module Start
************************************************************************/
void mfw_btips_bmgCallback( T_BTIPS_MMI_IND * para)
{
	MfwBtBmgEvent bmgBtEvent = para->data.bmgBtEvent;
	
	TRACE_FUNCTION_P1("mfw_btips_bmgCallback - %s", pME_Event(bmgBtEvent.eventType));
	
	switch(bmgBtEvent.eventType)
	{
		case BTEVENT_HCI_DEINITIALIZED:
			break;
			
		case BTEVENT_HCI_INITIALIZED:
			mfw_btips_bmgSetPhoneVisibility(TRUE);
			break;
		
		case BTEVENT_HCI_FAILED:			
			mfw_btips_signal(E_BTIPS_POWERON_FAILURE, NULL);
			break;
			
		case BTEVENT_PAIRING_COMPLETE:
			mfw_btips_signal(E_BTIPS_BMG_PAIRING_COMPLETE, para);
			break;
			
		case BTEVENT_BOND_RESULT:
			mfw_btips_signal(E_BTIPS_BMG_BOND_COMPLETE, para);
			break;	

		case BTEVENT_BOND_CANCELLED:
			mfw_btips_signal(E_BTIPS_BMG_BOND_CANCELLED, NULL);
		break;

		case BTEVENT_PIN_REQ:
			if (bmgBtEvent.errCode == BT_STATUS_SUCCESS)
			{
			mfw_btips_signal(E_BTIPS_INCOMING_PIN_REQ, para);
			}
			break;
			
		case BTEVENT_NAME_RESULT:
			
			TRACE_EVENT_P1("Received name for device %s", bmgBtEvent.p.deviceName);
			mfw_btips_signal(E_BTIPS_BMG_NAME_RESULT, para);
			break;
			
		case BTEVENT_SEARCH_COMPLETE:
			mfw_btips_signal(E_BTIPS_BMG_SEARCH_COMPLETE, NULL);
			break;
		case BTEVENT_SEARCH_CANCELLED:
			mfw_btips_signal(E_BTIPS_BMG_SEARCH_CANCELLED, NULL);
			break;

		case BTEVENT_INQUIRY_COMPLETE:
			break;
			
		case BTEVENT_INQUIRY_CANCELED:
			break;
		case BTEVENT_DISCOVER_SERVICES_RESULT:
			TRACE_FUNCTION("mfw_btips_bmgCallback BTEVENT_DISCOVER_SERVICES_RESULT");
			mfw_btips_signal(E_BTIPS_BMG_DEVICE_SERVICES, para);
			break;
			
		default:
		break;
	}
}
BOOL mfw_btips_bmgDeviceSetConnectPermissionAllow(const BD_ADDR 		*bdAddr,BOOL permission )
{

	BtlBmgDeviceRecord record;

	BtStatus  btStatus = BT_STATUS_FAILED;
	
	TRACE_FUNCTION("mfw_btips_bmgDeviceSetConnectPermissionAllow()");
	
	btStatus = BTL_BMG_FindDeviceRecord(mfwBtipsBmgContext, bdAddr, &record);
	TRACE_EVENT_P1("BTL_BMG_FindDeviceRecord in mfw_btips_bmgDeviceSetConnectPermissionAllow returned - %d", btStatus);
											
	if(BT_STATUS_SUCCESS == btStatus)											
		{
			record.trusted = TRUE;
			record.trustedPerService= SDP_SERVICE_OPP|SDP_SERVICE_DUN|SDP_SERVICE_FTP|SDP_SERVICE_A2DP|SDP_SERVICE_HFP; //Not sure if this is the way to set the bits in the mask field

			btStatus = BTL_BMG_SetDeviceRecord(mfwBtipsBmgContext, &record);
			
			TRACE_EVENT_P1("BTL_BMG_SetDeviceRecord in mfw_btips_bmgDeviceSetConnectPermissionAllow returned - %d", btStatus);
		}
			
		return mfw_btips_checkStatus(btStatus);
	
	
											
}

BOOL mfw_btips_bmgDeviceSetConnectPermissionAsk(const BD_ADDR 	*bdAddr,BOOL permission)
{

	BtlBmgDeviceRecord record;

	BtStatus  btStatus = BT_STATUS_FAILED;
	
	TRACE_FUNCTION("mfw_btips_bmgDeviceSetConnectPermissionAsk()");

	
	btStatus = BTL_BMG_FindDeviceRecord(mfwBtipsBmgContext, bdAddr,&record);

	TRACE_EVENT_P1("BTL_BMG_FindDeviceRecord in mfw_btips_bmgDeviceSetConnectPermissionAsk returned - %d", btStatus);

	
	if(BT_STATUS_SUCCESS == btStatus)
		{
			record.trusted = FALSE;
			
			record.trustedPerService= 0; //Not sure if this is the way to set the bits in the mask field

			btStatus = BTL_BMG_SetDeviceRecord(mfwBtipsBmgContext, &record);

			TRACE_EVENT_P1("BTL_BMG_SetDeviceRecord in mfw_btips_bmgDeviceSetConnectPermissionAsk returned - %d", btStatus);
				
		}

	return mfw_btips_checkStatus(btStatus);
		

}


BOOL mfw_btips_bmgSetRemoteDeviceName( const BD_ADDR 	*bdAddr, U8 *name)
{

	BtStatus  btStatus = BT_STATUS_FAILED;
	
	BtlBmgDeviceRecord record;
	
	TRACE_FUNCTION("mfw_btips_bmgSetRemoteDeviceName()");

	btStatus = BTL_BMG_FindDeviceRecord(mfwBtipsBmgContext, bdAddr,&record);
	
	if(BT_STATUS_SUCCESS == btStatus)
	{
		strcpy(record.name, name);
		btStatus = BTL_BMG_SetDeviceRecord(mfwBtipsBmgContext, &record);
	}
	return  mfw_btips_checkStatus(btStatus);
}

	//vamsee
/* Get the Services Supported by the Remote Device*/
U32 mfw_btips_bmgDeviceGetServices(const BD_ADDR 	*bdAddr)
{

	BtlBmgDeviceRecord  record;
	BtStatus 	   btStatus = BT_STATUS_FAILED;
	TRACE_FUNCTION("mfw_btips_bmgDeviceGetServices()");
	btStatus = BTL_BMG_FindDeviceRecord(mfwBtipsBmgContext,  bdAddr, &record);
	TRACE_EVENT_P1("Record Services %x",record.services);	
	return record.services;
}

BOOL mfw_btips_bmg_DiscoverServices(const BD_ADDR 	*bdAddr, SdpServicesMask sdpServicesMask)
{
	BtStatus btStatus;

	TRACE_EVENT_P1("mfw_btips_bmg_DiscoverServices Discover %x",sdpServicesMask);

	btStatus = BTL_BMG_DiscoverServices(mfwBtipsBmgContext, bdAddr,  sdpServicesMask);
	return  mfw_btips_checkStatus(btStatus);
}

BOOL mfw_btips_bmgSetDeviceRecord( const BD_ADDR 	*bdAddr, U32 sdpSericesMask)
{


	BtStatus  btStatus = BT_STATUS_FAILED;
	
	BtlBmgDeviceRecord record;
	
	TRACE_FUNCTION("mfw_btips_bmgSetDeviceRecord()");

	btStatus = BTL_BMG_FindDeviceRecord(mfwBtipsBmgContext, bdAddr,&record);
	
	if(BT_STATUS_SUCCESS == btStatus)
	{
		TRACE_EVENT_P1("mfw_btips_bmgSetDeviceRecord() %x",sdpSericesMask);
		
		record.services = sdpSericesMask;

		TRACE_EVENT_P1("Record Services %x",record.services);
		
		btStatus = BTL_BMG_SetDeviceRecord(mfwBtipsBmgContext, &record);
	}
	return  mfw_btips_checkStatus(btStatus);
}

/************************************************************************
	              BMG in ACI Context Module End
************************************************************************/




#endif

