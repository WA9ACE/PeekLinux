#ifndef _MMIBTIPS_A2DP_H_
#define _MMIBTIPS_A2DP_H_

/* =========================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================== */
/*
 $Project name: Basic Bluetooth A2DP MMI
 $Project code:
 $Module:   Bluetooth A2DP APPlication
 $File:       MmiBtipsA2dp.h
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS BMG APPlication functionality.

********************************************************************************
 $History: MmiBtipsA2dp.h

  26/06/07 Sasken original version

 $End

*******************************************************************************/


/* ====================================================== */
/*
* List Attribute
*/
/* ====================================================== */
static const MfwMnuAttr BtipsA2dpList_Attrib =
{
    &menuArea,
    MNU_LEFT| MNU_LIST_ICONS| MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    NULL,                               /* with these items         */
    0,                                   /* number of items     */
    COLOUR_LIST_XX,
    TxtBtipsA2dp,
    NULL,
    MNUATTRSPARE
};


typedef struct _T_MMI_Btips_A2dp_Win_data
{
	T_MMI_CONTROL          mmi_control;
	T_MFW_HND			parent;
	T_MFW_HND                 win;
	T_MFW_HND                 kbd;
	T_MFW_HND                 kbd_long;
	T_MFW_HND                 a2dp_optionsMenu;
	
	
	T_VOID_FUNC              func_cb;
	T_VOID_FUNC	            	cancel_cb;
	UBYTE				a2dp_num_file;
	UBYTE				a2dp_file_count;
	ListCbFunc			a2dp_list_cb;
	UBYTE				a2dp_list_idx;
	UBYTE                     	IsMfwInit;
	ListMenuData * 		menu_list_data; 
	BOOL				deviceSearchStarted;

}T_MMI_Btips_A2dp_Win_data;

typedef enum {
	BTIPS_A2DP_LIST_INIT=0,
	BTIPS_A2DP_LIST_DEINIT
}BTIPS_A2DP_WIN_EVENT;

/*Function Declarations*/
 int  mmi_btips_a2dpHandler(MfwMnu* m, MfwMnuItem* i);
void mmi_btips_a2dpExecCb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
int mmi_btips_a2dpDeviceSearchCallback(BD_ADDR bdAddr);

#endif
