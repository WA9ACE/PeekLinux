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
 $File:       MmiBtipsBmg.c
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS BMG APPlication functionality.

********************************************************************************
 $History: MmiBtipsBmg.c

  26/06/07 Sasken original version

 $End

*******************************************************************************/




/*******************************************************************************

                                Include files

*******************************************************************************/

#define ENTITY_MFW

#include <string.h>
#include "me.h"

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
#include "MmiBlkLangDB.h"
#include "mmiColours.h"
#include "MmiDialogs.h"
#include "MmiDummy.h" /* included for MmiMenu.h */
#include "MmiLists.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "AUIEditor.h"
#include "MmiMain.h"
#include "Mmiicons.h"
#include "MmiWindow.h"
#include "MmiCall.h"
#include "mmiBookShared.h"

#include "mfw_BtipsBmg.h"
#include "mfw_BtipsVg.h"
#include "mmiBtipsBmg.h"
#include "mmiBtips.h"


static BD_ADDR selectedDevice;
T_MFW_HND vgSrchWin;
T_MFW_HND vgConDisconnectWin;

#ifdef FF_MMI_BTIPS_APP

int mmi_btips_VgConnect(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	mfw_btips_vgConnect (selectedDevice);
	//win_delete(parent_win);
	return MFW_EVENT_CONSUMED;	
}

USHORT mmi_btips_vgIsDeviceConnected()//(struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi)
{
	if (mfw_btips_vgIsDeviceConnected (selectedDevice) == TRUE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int mmi_btips_VgDisconnect(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent (mfw_header ( ));
	mfw_btips_vgDisconnect (selectedDevice);
	//win_delete(parent_win);
	return MFW_EVENT_CONSUMED;	
}

USHORT mmi_btips_vgIsDeviceDisconnected()//(struct MfwMnuTag *m, struct MfwMnuAttrTag *ma,  struct MfwMnuItemTag *mi)
{
	if (mfw_btips_vgIsDeviceConnected (selectedDevice) == TRUE)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
	return MFW_EVENT_CONSUMED;	
}


int mmi_BtipsVgSearchCb (BD_ADDR bdAddr)
{
	static char szDevName[128];
	T_MFW_HND parent = mfwParent(mfw_header());
	selectedDevice = bdAddr;

	mfw_btips_bmgGetDeviceName (&bdAddr, szDevName);
	TRACE_FUNCTION_P1 ("mmi_BtipsVgSearchCb with Dev Name %s",  szDevName);
	mmi_btips_app_show_text (0, szDevName, "Selected", NULL);
	//SEND_EVENT (vgSrchWin, BTIPS_BMG_DEVICES_EXIT_WIN, 0, (void *)NULL);
#ifdef FF_MMI_FMRADIO_APP	
	if(FALSE == mfw_fmRadio_isFmOverBT())
	{
			vgConDisconnectWin = bookMenuStart (parent, (MfwMnuAttr *)btipsVgSelectedDeviceMenuAttributes(),0);
	}
	
	else
	{
#endif
		/*This request is from FM Radio MMI*/
		if (FALSE == mfw_btips_vgIsDeviceConnected(selectedDevice))
		{
			TRACE_FUNCTION ("mmi_BtipsVgSearchCb: FMOverBT, now make SLC");
			mfw_btips_vgConnect(selectedDevice);
		}
		else if(mfw_btips_vgIsAudioOnPhone())	
		{
			TRACE_FUNCTION ("mmi_BtipsVgSearchCb: FMOverBT, SLC UP, now create AUDIO connection");
			mfw_btips_vgTransferAudioToHF();
		}
		else
		{
			TRACE_FUNCTION ("mmi_BtipsVgSearchCb: FMOverBT, SLC UP, Audio UP, Now Disconnect SLC");
			mfw_btips_vgDisconnect(selectedDevice);
		}
//	}	
	//SEND_EVENT (vgSrchWin, BTIPS_BMG_DEVICES_EXIT_WIN, 0, (void *)NULL);//sundeep
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	mmi_BtipsMyHandsfree

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_BtipsMyHandsfree(MfwMnu* m, MfwMnuItem* i)
{
/*
Create a new window to 
1. List out the current paired Handsfree Device
2. Give an option to search for new Handsfree Devices
*/
	T_MFW_HND parent_win = mfwParent(mfw_header());

	TRACE_FUNCTION("mmi_BtipsMyHandsfree");
	
	vgSrchWin = mmi_btips_bmg_devices_win_create (parent_win, BTIPS_HSHF_DEVICE_SEARCH, mmi_BtipsVgSearchCb);

	return MFW_EVENT_CONSUMED;
}

int mmi_BtipsTransferAudioToPhone (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent (mfw_header ( ));

	mmi_btips_app_show_info (parent_win, TxtBtipsAudioOnPhone, TxtSelected, ONE_SECS,NULL);
	
	mfw_btips_vgTransferAudioToPhone ( );

	return MFW_EVENT_CONSUMED;	
}

int mmi_BtipsTransferAudioToHF (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent (mfw_header ( ));

	mmi_btips_app_show_info (parent_win, TxtBtipsAudioOnHeadset, TxtSelected, ONE_SECS,NULL);

	mfw_btips_vgTransferAudioToHF ( );	

	return MFW_EVENT_CONSUMED;	
}

int mmi_BtipsAudioHandOver (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent (mfw_header ( ));

	mmi_btips_app_show_info (parent_win, TxtBtipsAudioSwitchToOtherHeadset, TxtSelected, ONE_SECS,NULL);
//	mfw_btips_vgTransferAudioHandOver ( );		

	return MFW_EVENT_CONSUMED;	

}

USHORT	mmi_Btips_is_audio_on_phone (struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, 
					  struct MfwMnuItemTag *mi)
{
	if (mfw_btips_vgIsAudioOnPhone ( ) == TRUE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

USHORT	mmi_Btips_is_audio_on_handsfree (struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, 
					  struct MfwMnuItemTag *mi)
{
	if (mfw_btips_vgIsAudioOnHF ( ) == TRUE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

USHORT	mmi_Btips_is_audio_handover_possible (struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, 
					  struct MfwMnuItemTag *mi)
{
	if (mfw_btips_vgIsHandOverPossible ( ) == TRUE)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}

}
#endif
