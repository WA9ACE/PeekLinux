#ifndef _MMIBTIPS_BMG_H_
#define _MMIBTIPS_BMG_H_

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
 $Module:   Bluetooth BMG APPlication
 $File:       MmiBtipsBmg.h
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS BMG APPlication functionality.

********************************************************************************
 $History: MmiBtipsBmg.h

  26/06/07 Sasken original version

 $End

*******************************************************************************/
#define BTL_BMG_MAX_PIN_LENGTH 17
#define BT_MAX_REM_DEV_NAME 	248
#define BTIPS_MAX_SERVICES_BUFFER	200	//Buffer for the AUI_EDITOR to display the services supported by a particluar paired device

#define T_MFW_BTIPS_SEARCH_CB BtipsSearchCb
typedef int (*BtipsSearchCb) (BD_ADDR);  /* MFW event handler        */

/* ====================================================== */
/*
* List Attribute
*/
/* ====================================================== */
static const MfwMnuAttr BtipsDeviceList_Attrib =
{
    &menuArea,
    MNU_LEFT| MNU_LIST_ICONS| MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    NULL,                               /* with these items         */
    0,                                   /* number of items     */
    COLOUR_LIST_XX,
#ifdef FF_MMI_BTIPS_APP
    TxtBtipsApp,
#endif
    NULL,
    MNUATTRSPARE
};
//Structure to access buffer for AUI Editor
typedef struct
{

	char * edt_buf;
} T_MMI_BTIPS_SERVICES_STRUCT;

/*---------------------------------------------------------------------------
 * BondState type
 *
 *     MMI Bluetooth states for bond process
 */
typedef U8 BondState;

#define BOND_NOT_STARTED     0x00 /* No request from user*/
#define BOND_USER_REQUESTED    0x01 /* User has requested but the process is not  started*/
#define BOND_PIN_TAKEN 0x02 /*Pin was taken from user and send to peer device, no response yet  */
#define BOND_CANCELLED   0x03 /* User cancelled the request*/
#define PIN_REQUEST_RECEIVED   0x04 /* Response received from peer device for the sent PIN request*/
/* End of BondState */

/*---------------------------------------------------------------------------
 * InquiryState type
 *
 *     MMI Bluetooth states for Inquiry process
 */
typedef U8 InquiryState;

#define INQUIRY_NOT_STARTED     	0x00 /* No on-going inquiryprocess*/
#define INQUIRY_STARTED		 0x01 /*Inquiry is on-going, neither cancelled nor complete*/
#define INQUIRY_CANCELLATION_STARTED   0x02 /* User requested to cancel the inquiry*/
#define INQUIRY_CANCELLATION_COMPLETE 0x03 /*Cancellation complete*/
#define INQUIRY_COMPLETE			0x04 /*Inquiry comlpete*/
/* End of InquiryState */

#define SHOW_NO_LIST		0 // this is to keep the window alive, to pop up the pinkey window for bond
#define SHOW_PAIRED_LIST 	1
#define SHOW_SEARCH_LIST 	2
#define SHOW_NONE_EXIT 	3//Exit now

#define SERVICESMASK_NOT_EMPTY 	1
#define SERVICESMASK_EMPTY			0
typedef struct _T_MMI_Btips_Bmg_DevicesWin_data
{
	T_MMI_CONTROL	mmi_control;
	T_MFW_HND		win;
	T_MFW_HND		mfwwin;	
	T_MFW_HND		parent;
	T_MFW_HND         	kbd;
	T_MFW_HND		bondCancellationWindow;	
	SHORT          		id;

	/*Editor for Pincode request*/
	T_MFW_HND   	pinCode_editor;
	U8				pinCode[4];

	/*Create a list for displaying devices*/
	ListMenuData 		  *menu_list_data;
	T_MFW_HND            list_win;//Hanlder for the list
	U32				  listCount;
	BOOL			  listID;	//for Inquiry list = SHOW_SEARCH_LIST, for paired list = SHOW_PAIRED_LIST
	
	/*Create a list for displaying connected devices*/
	ListMenuData 		  *menu_list_connected_data;
	T_MFW_HND            list_connected_win;//Hanlder for the list
	U32				  listCountConnectedDevices;
	U32				  noConnectedDevices;
	Bmg_ConnectedDeviceList connected_deviceList[BT_MAX_DEVICE_LIST];
	//BOOL			  listID;	//for Inquiry list = SHOW_SEARCH_LIST, for paired list = SHOW_PAIRED_LIST

	/*Global Data related to BTIPS Application Statemachine*/
	BOOL			searchType;
	InquiryState		inquiryState;
	BondState 		bondState;
	U32 				noPairedDevices;
	U32 				noInquiredDevices;
	U32 				currentListIndex;
	Bmg_DeviceList	deviceList[BT_MAX_DEVICE_LIST];
BtipsSearchCb	searchCB;

}T_MMI_Btips_Bmg_DevicesWin_data;

typedef enum {
	BTIPS_BMG_PAIRED_DEVICES_INIT=0,
	BTIPS_BMG_INQUIRY_RESULT_DEVICES_INIT,
	BTIPS_BMG_BONDING_REQUEST_INIT,
	BTIPS_BMG_BONDING_REQUEST_SENT,
	BTIPS_BMG_DEVICES_LIST_WIN_DEINIT,
	BTIPS_BMG_DEVICES_EXIT_WIN,
}BTIPS_BMG_DEVICES_WIN_EVENT;

typedef enum {
	BTIPS_BMG_CONNECTED_DEVICES_EXIT_WIN=0,
}BTIPS_BMG_CONNECTED_DEVICES_WIN_EVENT;
/*********************************************************************
				BMG Editor Data for PhoneName Editor , PinCode Editor
*********************************************************************/
typedef enum {
	BTIPS_BMG_LOCAL_EDITOR_INIT=0,
	BTIPS_BMG_REMOTE_EDITOR_INIT,
	BTIPS_BMG_EDITOR_CANCEL
}BTIPS_BMG_PHONENAME_EDITOR_EVENT;

typedef struct _T_MMI_Btips_Bmg_PhoneName_Win_data
{
	T_MMI_CONTROL	mmi_control;
	T_MFW_HND		win;
	T_MFW_HND		mfwwin;	
	T_MFW_HND		parent;
	T_MFW_HND         	kbd;
	SHORT          		id;
	T_MFW_HND   	phoneName_editor;
	UBYTE 			phoneNameBuffer[BT_MAX_REM_DEV_NAME]; /* pointer of text and number */
	BOOL			local_device;	//TRUE indicates Local device and FALSE indicates Remote Device
}	T_MMI_Btips_Bmg_PhoneName_Win_data;

typedef struct _T_MMI_Btips_Bmg_PinCode_Win_data
{
	T_MMI_CONTROL	mmi_control;
	T_MFW_HND		win;
	T_MFW_HND		mfwwin;	
	T_MFW_HND		parent;
	T_MFW_HND         	kbd;
	SHORT          		id;
	T_MFW_HND   	pinCode_editor;
	UBYTE 			pinCodeBuffer[16]; /* pointer of text and number *///sundeep changed from 4
}	T_MMI_Btips_Bmg_PinCode_Win_data;
/*********************************************************************
				BMG Editor END
*********************************************************************/

T_MFW_HND mmi_btips_bmg_pinCode_editor_create(MfwHnd parent_window);
T_MFW_HND mmi_btips_bmg_devices_win_create(T_MFW_HND parent, BOOL searchType, T_MFW_BTIPS_SEARCH_CB searchCB);
T_MFW_HND mmi_btips_bmg_services_editor_create(T_MFW_HND parent_win, SdpServicesMask sdpServicesMask);
#endif
