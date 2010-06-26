#ifndef _MMIBTIPS_OPPC_H_
#define _MMIBTIPS_OPPC_H_

/* =========================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================== */
/*
 $Project name: Basic Bluetooth OPP Client MMI
 $Project code:
 $Module:   Bluetooth OPP Client APPlication
 $File:       mmiBtipsOppc.h
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS BMG APPlication functionality.

********************************************************************************
 $History: mmiBtipsOppc.h

  26/06/07 Sasken original version

 $End

*******************************************************************************/


/* ====================================================== */
/*
* List Attribute
*/
/* ====================================================== */
static const MfwMnuAttr BtipsOppcList_Attrib =
{
    &menuArea,
    MNU_LEFT| MNU_LIST_ICONS| MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    NULL,                               /* with these items         */
    0,                                   /* number of items     */
    COLOUR_LIST_XX,
    TxtBtipsOppc,
    NULL,
    MNUATTRSPARE
};


typedef struct _T_MMI_Btips_Oppc_Win_data
{
	T_MMI_CONTROL          mmi_control;
	T_MFW_HND			parent;
	T_MFW_HND                 win;
	T_MFW_HND                 kbd;
	T_MFW_HND                 kbd_long;
	T_MFW_HND                 oppc_optionsMenu;
	
	
	T_VOID_FUNC              func_cb;
	T_VOID_FUNC	            	cancel_cb;
	UBYTE				oppc_num_file;
	UBYTE				oppc_file_count;
	ListCbFunc			oppc_list_cb;
	UBYTE				oppc_list_idx;
	UBYTE                     	IsMfwInit;
	ListMenuData * 		menu_list_data; 

}T_MMI_Btips_Oppc_Win_data;

typedef enum {
	BTIPS_OPPC_LIST_INIT=0,
	BTIPS_OPPC_LIST_DEINIT
}BTIPS_OPPC_WIN_EVENT;

#define OPPC_DEFAULT_PUSH_PULL_DIR			"/MfwBtDemo/"
#define OPPC_DEFAULT_PUSH_CARD				"/MfwBtDemo/default/mycard.vcf"
#define OPPC_VCF_EXT							".vcf"

/*Function Declarations*/
 int  				mmi_btips_oppcHandler(MfwMnu* m, MfwMnuItem* i);
int  				mmi_btips_oppcPullHandler(MfwMnu* m, MfwMnuItem* i);
int 				mmi_btips_oppcExchangeHandler(MfwMnu* m, MfwMnuItem* i);
static T_MFW_HND  mmi_btips_oppcBuildMenu( MfwHnd parent_window);
static int 			mmi_btips_oppcListWinCb (MfwEvt e, MfwWin *w)  ;
void 			mmi_btips_oppcListWinExecCb (T_MFW_HND win, USHORT event, 
										SHORT value, void * parameter);
static void 		mmi_btips_oppcListMenuCb(T_MFW_HND Parent, ListMenuData * ListData);
static void 		mmi_btips_oppcListMenuDestroy(MfwHnd window);
static MfwHnd 		mmi_btips_oppcShowInfoDlg(T_MFW_HND parent, int str1, int str2, 
	                                                                       T_VOID_FUNC callback);

int 				mmi_btips_oppcDeviceSearchWnd(int type);
#endif

