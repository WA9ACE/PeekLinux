#ifndef _MMIBTIPSGLOBAL_H_
#define _MMIBTIPSGLOBAL_H_

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
/*******************************************************************************
                                                                              
                                Public methods
                                                                              
*******************************************************************************/
USHORT	mmi_btipsRadioOnStatus(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi);
USHORT	mmi_btipsRadioOffStatus(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi);
 int  mmi_btipsRadioOn(MfwMnu* m, MfwMnuItem* i);
GLOBAL int  mmi_btipsRadioOff(void);
int mmi_btips_bmgDevices(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_bmgPhoneName(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_bmgSetVisibility(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_bmgConnectedDevices(MfwMnu* m,MfwMnuItem* i);
int mmi_btips_bmgHidePhone(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_bmgShowPhone(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_bmgSetConnectPermissionAllow(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_bmgSetConnectPermissionAsk(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_bmgDeviceDelete(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_bmgDeviceDisconnect(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_bmgDeviceSetRemotename(MfwMnu* m, MfwMnuItem* i);
USHORT mmi_btips_bmgIsVisible( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
USHORT mmi_btips_bmgIsInvisible( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
int mmi_btips_vgIncomingCallInHandsfree(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_vgIncomingCallInPhone(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_vgMyHandsfree(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_VgConnect(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_vgIsDeviceConnected (struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi);
int mmi_btips_VgDisconnect(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_vgIsDeviceDisconnected (struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi);
int  mmi_btips_a2dpHandler(MfwMnu* m, MfwMnuItem* i);
int  mmi_btips_oppcHandler(MfwMnu* m, MfwMnuItem* i);
int  mmi_btips_oppcPullHandler(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_oppcExchangeHandler(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_a2dpPlayOnSingleDevice(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_a2dpPlayOnMultipleDevice(MfwMnu* m, MfwMnuItem* i);
GLOBAL int mmi_btips_services(MfwMnu* m, MfwMnuItem* i);
int mmi_btips_a2dpDeviceSearchWnd();
USHORT	mmi_Btips_is_audio_on_phone (struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, 
					  struct MfwMnuItemTag *mi);
USHORT	mmi_Btips_is_audio_on_handsfree (struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, 
					  struct MfwMnuItemTag *mi);
USHORT	mmi_Btips_is_audio_handover_possible (struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, 
					  struct MfwMnuItemTag *mi);
int mmi_BtipsTransferAudioToPhone (MfwMnu* m, MfwMnuItem* i);
int mmi_BtipsTransferAudioToHF (MfwMnu* m, MfwMnuItem* i);
int mmi_BtipsAudioHandOver (MfwMnu* m, MfwMnuItem* i);
int mmi_BtipsMyHandsfree(MfwMnu* m, MfwMnuItem* i);
#endif //_MMIBTIPSGLOBAL_H_