#ifndef DEF_MFW_BTIPS_VG
#define DEF_MFW_BTIPS_VG
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
 $File:       Mfw_BtipsVg.h
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS VG APPlication functionality.

********************************************************************************
 $History: Mfw_BtipsVg.h

  26/06/07 Sasken original version

 $End

*******************************************************************************/

/* Maximum files to be displayed in the MMI */

#define MFW_VG_APP_AT_STR_LEN	100


BOOL mfw_btips_vgConnect (BD_ADDR bdAddr);

BOOL mfw_btips_vgDisconnect (BD_ADDR bdAddr);

BOOL mfw_btips_vgIsDeviceConnected (BD_ADDR bdAddr);

BOOL mfw_btips_vgTransferAudioToPhone (void);	

BOOL mfw_btips_vgTransferAudioToPhone (void);

BOOL mfw_btips_vgIsAudioOnPhone (void);

BOOL mfw_btips_vgIsAudioOnHF (void);

BOOL mfw_btips_vgIsHandOverPossible (void);
BOOL mfw_btips_vgGetConnectedDevice (BD_ADDR *bdAddr, U8 *name);

#endif


