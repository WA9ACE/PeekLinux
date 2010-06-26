#ifndef _MMIBTIPS_H_
#define _MMIBTIPS_H_

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
#include "Bttypes.h"

typedef struct _MMI_Btips_data
{
	 int ipBtipsHandle;
	/*Editor for Pincode request*/
	T_MFW_HND   	pinCode_editor;
	/*Global Data related to BTIPS Application Statemachine*/
	U8	incomingPinRequestState; //TRUE for incoming and FALSE for bond request(out-going)
	BD_ADDR		bd_addr_incomingPinReq;
	U8				deviceName[BT_MAX_REM_DEV_NAME + 1]; //of the current Bonding (IN/OUT) device
}	MMI_Btips_data;

/*******************************************************************************
                                                                              
                                Public methods
                                                                              
*******************************************************************************/

T_MFW_HND mfw_btips_create(T_MFW_HND hWin, T_MFW_EVENT event, MfwTyp type, T_MFW_CB cbfunc);
GLOBAL MfwHnd mmi_btips_app_show_info(T_MFW_HND parent, int str1, int str2,int timer, T_VOID_FUNC callback);
MfwHnd mmi_btips_app_show_text(T_MFW_HND parent, char * str1, char * str2, T_VOID_FUNC callback);
T_MFW_HND mmi_btips_app_ask_info(T_MFW_HND parent, char * str1, char * str2, T_VOID_FUNC callback);

#endif
