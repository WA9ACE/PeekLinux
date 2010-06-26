#ifndef _DEF_MMI_MMIWINDOW_H_
#define _DEF_MMI_MMIWINDOW_H_


/*******************************************************************************

					TI (Shanghai)

********************************************************************************                                                                              

 This software product is the property of TI (Shanghai) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	TISHMMI Project                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		    mmiwindow.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Yan Bin(bin-yan@ti.com)                                                          
 $Date:		    28/08/03                                                     
                                                                               
********************************************************************************
                                                                              
 Description:
 
    This module provides definitions of the types and
    constants which are shared across the MMS
    application modules.
   
********************************************************************************
 $History: mmiwindow.h

    CRR 23959: xpradipg 26 Aug 2004
    Description: List not updated after deleting the messages in Unsent/Inbox folder
    Solution: The list is refreshed everytime the user returns from the options menu
    
	28/08/03			Original TI(Shanghai) BMI version.	
	   
 $End

*******************************************************************************/
#undef GLOBAL_EXT
#if defined (MMI_MMIWINDOW_C)
	#define GLOBAL_EXT
#else
	#define GLOBAL_EXT extern
#endif

/*******************************************************************************
                                                                              
                                Include Files
                                                                              
*******************************************************************************/

#include "mfw_mfw.h"



/*******************************************************************************
                                                                              
                        MMI Window Message definition
                                                                              
*******************************************************************************/
#define MWM_CREATE			0x0010
#define MWM_DESTORY		0x0011
#define MWM_ONDRAW		0x0012
#define MWM_KEYBOARD		0x0012

#define MWM_SUSPEND		0x0013
#define MWM_RESUME		0x0014
//CRR 23959 - xpradipg 26 Aug 2004
//new event added to handle the menu redraw on returning from the options menu
#define MWM_REDRAW		0x0015

#define MWM_USER			0x8000

//CRR 23959 - xpradipg 26 Aug 2004
//Moved the structure definition from mmiwindow.c
typedef struct
{
	/* administrative data */
	T_MMI_CONTROL   mmi_control;
	T_MFW_HND       win;
	SHORT           id;
	T_MFW_HND       parent;

	/* associated handlers */

	T_MFW_HND       kbd;
	T_MFW_HND       kbd_long;
	T_MFW_HND       menu;

	T_MFW_HND       opt_win;
	T_MFW_HND        info_win;
	T_MFW_HND        list_win;

	ListMenuData *menu_list_data;
	UBYTE           option_select;  /* indicate what the user selected in the Optionsmenu*/
} T_MMIWINDOW;

/*******************************************************************************
                                                                              
                                Function Prototypes
                                                                              
*******************************************************************************/


GLOBAL_EXT T_MFW_HND  MMI_CreateWindow(T_MFW_HND 		hWinParent, 
											T_DIALOG_FUNC  	win_exec_cb,
											void * 			parameter,
											MfwCb 			kbd_cb,
											MfwMnuAttr *		pMnuAttr,	
											unsigned long 		dwStyle);

GLOBAL_EXT T_MFW_HND  MMI_ParentWindow(T_MFW_HND hWin);

GLOBAL_EXT T_MFW_HND  MMI_DestoryWindow(T_MFW_HND hWin);

extern void DummyFreeFunc(void *pMemBlock);


/*******************************************************************************
                                                                              
                              DUMMY DECLARATIONS	
                                                                              
*******************************************************************************/

//Dummy defines

// Dummy global variables



#endif /* _DEF_MMI_MMIWINDOW_H_ */





