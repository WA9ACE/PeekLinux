#ifndef DEF_MFW_BTIPS
#define DEF_MFW_BTIPS
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
 $File:       Mfw_Btips.h
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS BMG APPlication functionality.

********************************************************************************
 $History: Mfw_Btips.h

  26/06/07 Sasken original version

 $End

*******************************************************************************/

#define MFW_BTIPS_SUCCESS	1
#define MFW_BTIPS_FAILED	0

#define BT_FS_MAX_PATH_LENGTH		256
#define BT_MAX_REM_DEV_NAME 248
#define BT_DEMO_FOLDER_PATH      "/MfwBtDemo"
/* Macro for sending events to UI - meanwhile, trace message to PCO Viewer */
void mfw_BtipsReport(char *format,...);
#define mfw_BtipsReport(f) vsi_o_event_ttrace(f);

#define BT_MAX_REM_DEV_NAME 248
#define BT_MAX_OBJECT_NAME_LEN 256


typedef U8 MfwBtMainEvent;

typedef U8 BtEventType;

typedef U8 MfwFtpsEventType;
typedef U8 MfwVgEventType;

typedef struct _mfwBtBmgEvent
{
	BtEventType eventType; /* BtEventType - ME Event Type*/
	BtErrorCode   errCode;     /* BtErrorCode - Error code */
	BD_ADDR            bdAddr;     /* Device Address */
	union
	{
		U8	deviceName[BT_MAX_REM_DEV_NAME + 1];
		/* Results  for BTEVENT_DISCOVER_SERVICES_RESULT event */
		SdpServicesMask discoveredServicesMask;
	}p;
	
}MfwBtBmgEvent;

typedef struct _mfwBtOppsEvent
{
	BtEventType mfwOppsEventType; /* BtEventType - ME Event Type*/
	union
	{
		struct 
		{
			U8	deviceName[BT_MAX_REM_DEV_NAME + 1];
			U8	oppObjectName[80];
		}opps;
	}p;
}MfwBtOppsEvent;
typedef struct _mfwBtVgEvent
{
	MfwVgEventType event;
	U8	deviceName[BT_MAX_REM_DEV_NAME + 1];
	BD_ADDR bdAddr;
}MfwBtVgEvent;

typedef struct _mfwBtFtpsEvent
{
	MfwFtpsEventType event;
	union
	{
		struct
		{
			U8	deviceName[BT_MAX_REM_DEV_NAME + 1];
			BD_ADDR bdAddr;
		}ftpConnect;
		struct
		{
			U8	deviceName[BT_MAX_REM_DEV_NAME + 1];
			BD_ADDR bdAddr;
		}ftpDisconnect;
		struct
		{
			U16 percent;
		}ftpPercentComplete;
		struct
		{
			U8 objectName[BT_MAX_OBJECT_NAME_LEN];
		}ftpComplete;
		struct
		{
			U8	deviceName[BT_MAX_REM_DEV_NAME + 1];
			BD_ADDR bdAddr;
			U8 objectName[BT_MAX_OBJECT_NAME_LEN];
		}ftpGetRequest;
		struct
		{
			U8	deviceName[BT_MAX_REM_DEV_NAME + 1];
			BD_ADDR bdAddr;
			U8 objectName[BT_MAX_OBJECT_NAME_LEN];
		}ftpPutRequest;
		struct
		{
			U8	deviceName[BT_MAX_REM_DEV_NAME + 1];
			BD_ADDR bdAddr;
			U8 objectName[BT_MAX_OBJECT_NAME_LEN];
		}ftpDeleteRequest;

	}p;
	
}MfwBtFtpsEvent;

typedef struct _mfwBtMdgEvent
{
	BtEventType mfwMdgEventType; /* BtEventType - ME Event Type*/
	union
	{
		struct 
		{
			U8	deviceName[BT_MAX_REM_DEV_NAME + 1];
		}mdg;
	}p;
}MfwBtMdgEvent;

typedef U8 MfwBtOppcEvent;
typedef U8 MfwBtA2dpEvent;
/*mfwBtipsEventType modules*/
#define MFW_BTIPS_MAIN_EVENT	0
#define MFW_BTIPS_BMG_EVENT	1
#define MFW_BTIPS_OPPS_EVENT	2
#define MFW_BTIPS_OPPC_EVENT	3
#define MFW_BTIPS_FTPS_EVENT	4
#define MFW_BTIPS_VG_EVENT	5
#define MFW_BTIPS_MDG_EVENT	6

#define MFW_BTIPS_FTPS_PUT_EVENT			1
#define MFW_BTIPS_FTPS_GET_EVENT			2
#define MFW_BTIPS_FTPS_DELETE_EVENT		3
#define MFW_BTIPS_FTPS_COMPLETE_EVENT	4	//vamsee
#define MFW_BTIPS_FTPS_ABORTED_EVENT	5	//vamsee
#define MFW_BTIPS_FTPS_PUT_COMP_EVENT  6
//#define MFW_BTIPS_FTPS_GET_COMP_EVENT  7
#define MFW_BTIPS_FTPS_DELETE_COMP_EVENT  8
#define MFW_BTIPS_FTPS_CONNECT_EVENT    9
#define MFW_BTIPS_FTPS_DISCONNECT_EVENT    10


#define MFW_BTIPS_VG_CONNECT_EVENT				1
#define MFW_BTIPS_VG_DISCONNECT_EVENT			2
#define MFW_BTIPS_VG_AUDIO_CONNECT_EVENT		3
#define MFW_BTIPS_VG_AUDIO_DISCONNECT_EVENT	4

#define MFW_BTIPS_OPPS_PUSH_EVENT	1
#define MFW_BTIPS_OPPS_PULL_EVENT	2
#define MFW_BTIPS_OPPS_PUSH_COMP_EVENT	6
#define MFW_BTIPS_OPPS_PULL_COMP_EVENT	4
//#define MFW_BTIPS_OPPS_DISCONNECT_EVENT	4

typedef struct _T_BTIPS_MMI_IND
{
	U8 mfwBtipsEventType;
	union
		{
			MfwBtMainEvent btlBtEvent;
			MfwBtBmgEvent bmgBtEvent;
			 /*Give yr own required Structure definition for the data that is to be carried 
			 from BTS context to the MMI (ACI) context*/
			MfwBtOppsEvent oppsBtEvent;
			MfwBtOppcEvent oppcBtEvent;
			MfwBtFtpsEvent ftpsBtEvent;
			MfwBtVgEvent vgBtEvent;
			MfwBtMdgEvent mdgBtEvent;
			MfwBtA2dpEvent a2dpBtEvent;

			/*.....Event types can be defined for other modules*/
		}data;
	
}T_BTIPS_MMI_IND;

/******************BTIPS Generic  EVENTS************************/
#define E_BTIPS_POWERON_SUCCESS		(T_MFW_EVENT)0x00000001
#define E_BTIPS_POWERON_FAILURE		(T_MFW_EVENT)0x00000002
#define E_BTIPS_POWEROFF_SUCCESS		(T_MFW_EVENT)0x00000004
#define E_BTIPS_BMG_PAIRING_COMPLETE	(T_MFW_EVENT)0x00000008
#define E_BTIPS_INCOMING_PIN_REQ		(T_MFW_EVENT)0x00000010
#define E_BTIPS_BT_NOT_ON				(T_MFW_EVENT)0x00000020
#define E_BTIPS_FATAL_ERROR			(T_MFW_EVENT)0x00000040
#define E_BTIPS_FTPS_PUT_EVENT		(T_MFW_EVENT)0x00000080
#define E_BTIPS_FTPS_GET_EVENT		(T_MFW_EVENT)0x00000100
#define E_BTIPS_FTPS_COMPLETE_EVENT 			(T_MFW_EVENT)0x00400000		//sundeep
#define E_BTIPS_FTPS_ABORTED_EVENT			(T_MFW_EVENT)0x00800000		//vamsee
#define E_BTIPS_FTPS_PUT_COMP_EVENT			(T_MFW_EVENT)0x01000000		//sundeep
//#define E_BTIPS_FTPS_GET_COMP_EVENT			(T_MFW_EVENT)0x02000000		//sundeep
#define E_BTIPS_FTPS_DELETE_COMP_EVENT		(T_MFW_EVENT)0x04000000		//sundeep
#define E_BTIPS_FTPS_CONNECT_EVENT			(T_MFW_EVENT)0x20000000		//sundeep
#define E_BTIPS_FTPS_DISCONNECT_EVENT		(T_MFW_EVENT)0x40000000		//sundeep


#define E_BTIPS_FTPS_DELETE_EVENT				(T_MFW_EVENT)0x00000200
#define E_BTIPS_VG_CONNECT_EVENT				(T_MFW_EVENT)0x00000400
#define E_BTIPS_VG_DISCONNECT_EVENT			(T_MFW_EVENT)0x00000800
#define E_BTIPS_VG_AUDIO_CONNECT_EVENT		(T_MFW_EVENT)0x00001000
#define E_BTIPS_VG_AUDIO_DISCONNECT_EVENT	(T_MFW_EVENT)0x00002000

/******************BMG EVENTS*********************************/
#define E_BTIPS_BMG_NAME_RESULT			(T_MFW_EVENT)0x00004000
#define E_BTIPS_BMG_SEARCH_COMPLETE		(T_MFW_EVENT)0x00008000
#define E_BTIPS_BMG_SEARCH_CANCELLED		(T_MFW_EVENT)0x00010000
#define E_BTIPS_BMG_BOND_COMPLETE		(T_MFW_EVENT)0x00020000
#define E_BTIPS_BMG_DEVICE_SERVICES 		(T_MFW_EVENT)0x00040000
#define E_BTIPS_OPPS_PUSH_EVENT			(T_MFW_EVENT)0x00080000
#define E_BTIPS_OPPS_PULL_EVENT			(T_MFW_EVENT)0x00100000
#define E_BTIPS_BMG_BOND_CANCELLED		(T_MFW_EVENT)0x00200000

//#if 0
#define E_BTIPS_OPPS_PULL_COMP_EVENT	(T_MFW_EVENT)0x08000000
#define E_BTIPS_OPPS_PUSH_COMP_EVENT 	(T_MFW_EVENT)0x10000000
//#endif

#define E_BTIPS_ALL_SERVICES	   	(T_MFW_EVENT)0xFFFFFFFF

typedef T_BTIPS_MMI_IND T_MFW_BTIPS_PARA;

typedef struct
{
    T_MFW_EVENT      emask;         /* events of interest */
    T_MFW_EVENT      event;          /* current event */
    T_MFW_CB           handler;
    T_MFW_BTIPS_PARA  para;
} T_MFW_BTIPS;


void Btips_ffs_log_message(char *LogMsg,...);


#ifdef FF_MMI_BTIPS_APP
#define BTIPS_MMI_IND 	0x1107 //TISH: 0x110 this value is same as Camera primiive.
GLOBAL BOOL btipsPrimHandler (USHORT opc, void * data);
#endif//BT-MMI - FF_MMI_BTIPS_APP

BOOL mfw_btips_checkStatus(BtStatus btStatus);

void mfw_btips_signal(T_MFW_EVENT event, T_BTIPS_MMI_IND * para);
int mmi_btips_app_root_cb(T_MFW_EVENT evnt, void *para);
void mfw_btips_ftpsAcceptObjectRequest (BOOL flag);
void mfw_btips_ftpsDisconnect (void);
	
#endif

