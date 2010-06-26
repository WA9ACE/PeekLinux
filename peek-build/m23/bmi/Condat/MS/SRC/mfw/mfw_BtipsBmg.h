#ifndef DEF_MFW_BTIPS_BMG
#define DEF_MFW_BTIPS_BMG
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
 $File:       Mfw_BtipsBmg.h
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS BMG APPlication functionality.

********************************************************************************
 $History: Mfw_BtipsBmg.h

  26/06/07 Sasken original version

 $End

*******************************************************************************/


#define BT_MAX_DEVICE_LIST		10
#define MAX_RESPONSES   	10	//limit the number of Inquiry responses to 10

#include <me.h>
#include <Bttypes.h>

typedef struct _Bmg_DeviceList
{
	U8				myIndex;
	BD_ADDR 		bdAddr;
	UBYTE 			name[BT_MAX_REM_DEV_NAME];
}Bmg_DeviceList;

typedef struct _Bmg_ConnectedDeviceList
{
	U8				myIndex;
	BD_ADDR 		bdAddr;
	UBYTE 			name[BT_MAX_REM_DEV_NAME];
	U32   			ServiceId;
}Bmg_ConnectedDeviceList ;

typedef enum {
	BTIPS_GENERIC_DEVICE_SEARCH=0,
	BTIPS_A2DP_DEVICE_SEARCH,
	BTIPS_OPP_DEVICE_SEARCH,
	BTIPS_HSHF_DEVICE_SEARCH,
	BTIPS_DEVICE_SERVICES_SEARCH,
	BTIPS_CONNECTED_DEVICES_SEARCH	
}BTIPS_SEARCH_TYPES;

typedef enum{

	BTIPS_SDP_SERVICE_FTPS=0,
	BTIPS_SDP_SERVICE_OPPC,
	BTIPS_SDP_SERVICE_OPPS,
	BTIPS_SDP_SERVICE_VG,
	BTIPS_SDP_SERVICE_A2DP,
	  BTIPS_MAX_PROFILE_COUNT,

}BTIPS_SERVICE_TYPES;

BOOL mfw_btips_bmgDeviceDelete(const BD_ADDR *bdAddr);
BOOL mfw_btips_bmgSetPhoneVisibility(BOOL status);
BOOL mfw_btips_bmgGetPhoneVisibility(void);
BOOL mfw_btips_bmgSendPin (const BD_ADDR	*bdAddr, 
								const U8 				*pin,
								const U8 				len);
BOOL mfw_btips_bmgBond(const BD_ADDR 		*bdAddr,
							const U8 				*pin, 
							const U8 				len);

BOOL mfw_btips_bmgSetLocalDeviceName (U8 *name);
BOOL mfw_btips_bmgGetLocalDeviceName(U8 *name);
BOOL mfw_btips_bmgSetRemoteDeviceName( const BD_ADDR 	*bdAddr, U8 *name);
BOOL mfw_btips_bmgSetDeviceRecord( const BD_ADDR 	*bdAddr, U32 sdpSericesMask);
BOOL mfw_btips_bmg_getPairedDevices(Bmg_DeviceList *pairedDeviceList, U32 *count);
BOOL mfw_btips_bmg_getConnectedDevices(Bmg_ConnectedDeviceList *pairedDeviceList, U32 *count);
U32 mfw_btips_bmg_getConnectedDevicesCount(void);
BOOL mfw_btips_bmg_getConnectedDevicesForProfile(U32 profile_num, BD_ADDR 	*bdAddr,U8 *name, U32 *ServiceId);
BOOL mfw_btips_bmgDeviceSetConnectPermissionAllow(const BD_ADDR 	*bdAddr,BOOL permission);
BOOL mfw_btips_bmgDeviceSetConnectPermissionAsk(const BD_ADDR 	*bdAddr,BOOL permission);
U32 mfw_btips_bmgDeviceGetServices(const BD_ADDR 	*bdAddr);
BOOL mfw_btips_ftpsGetConnectedDevice (BD_ADDR *bdAddr, U8 *name);
BOOL mfw_btips_bmgGetDeviceState (const BD_ADDR *bdAddr, BtRemDevState *state);
BOOL mfw_btips_bmgSetDeviceNameInRecord(const BD_ADDR *bdAddr, UBYTE *name);
BOOL mfw_btips_bmg_DiscoverServices(const BD_ADDR 	*bdAddr, SdpServicesMask sdpServicesMask);


#endif


