/*******************************************************************************

					CONDAT (UK)

********************************************************************************									      

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.				   
									      
********************************************************************************

 $Project name: Basic MMI						       
 $Project code: BMI (6349)							     
 $Module:		CPHS
 $File: 	    MmiCPHS.c
 $Revision:		1.0							  
									      
 $Author:		Condat(UK)							   
 $Date: 	    25/01/02							  
									       
********************************************************************************
									      
 Description

    This module provides the CPHS functionality

  
********************************************************************************
 $History: MmiCall.c
 	Nov 27, 2006 OMAPS00098359 a0393213(R.Prabakar)
 	Description: CPHS 4.2 : Call Divert doesn't work properly for Line 2

       Oct 11, 2006 REF:OMAPS00098287  x0039928
       Description : CPHS 4.2: MMI variable als_status is not updated proplery with the SIM contents.
       Solution     : als_status update is deffered until the idle screen is displayed.
       		      
       May 30, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)
       Description : CPHS Call Forwarding feature implementation
       Solution     : As part of this implementation, E_CPHS_SET_DV_STAT and E_CPHS_GET_DV_STAT in cphs_mfw_cb()
       		      are modified

	May 26, 2006 ER: OMAPS00079607 x0012849 :Jagannatha M
	Description: CPHS: Display 'charge/Timers' menu based on AOC ( Advice of charge ) and support info numbers.
	Solution:Made the following changes
		       1. Aoc_flag variable is defined and it will be set based on the CSP information.
		       2. New function info_num_msg_cb() added to support info numbers.
		       
	May 30, 2006  DR: OMAPS00070657 x0pleela
	Description: CPHS feature on Locosto-Lite
	Solution: For ALS feature,
			a) Defined a new macro "MAX_LINE_NAME_LEN"
			b) Defined new structure "T_ALS_DATA" for editing Line 1 and 2 names
			c) Defined new flag "Als_flag" to store the staus of ALS bit from CSP data
			d) Added new functions cphs_Line_Edit_cb(), cphs_edit_line_exec_cb(), 
				cphs_edit_line_create(), cphs_edit_line1(), cphs_edit_line2(), 
				mmi_cphs_get_als_info(), mmi_cphs_get_als_info(), mmi_cphs_set_als_value(),
				mmi_cphs_get_als_value(), mmi_get_als_selectedLine()
	
	Jun 14, 2005 REF: MMI-FIX-30439 x0018858
   	Description: The homezone/cityzone tags were not being displayed properly.
   	Solution: Modified to save the tags properly in caches and also made the 
	appropriate modifications to update the same properly during mobility.

	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX

    	Jun 23, 2004        	REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
  	Solution:			The CF state is maintained in flash. During bootup the CF icon in the idle screen
  					 is displayed depending upon its state in the ffs


//  May 31, 2004        REF: CRR 17291  xvilliva
//	Bug:	After power cycle the setting of the used line which have been made 
//			from the BMI is lost.
//	Fix:	1.The global g_pin2 is defined here.
//		2. sAT_PlusCLCK() is called while locking and unlocking ALS.
//		3. ALSPresent() is modified to call tAT_PercentALS() & qAT_PercentALS().
//		NOTE:- ALSPresent() was giving faulty status earlier.


  25/01/02			Original Condat(UK) BMI version. (MC)	
	   
 $End

*******************************************************************************/


/******************************************************************************
									      
				Include Files
									      
*******************************************************************************/

/* includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif
#include "mfw_sys.h"

#include "cus_aci.h"

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_tim.h"
#include "mfw_phb.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_lng.h"
#include "mfw_sat.h"
#include "mfw_kbd.h"
#include "mfw_nm.h"
#include "mfw_str.h"
#include "mfw_cphs.h"
#include "dspl.h"

#include "MmiMain.h"
#include "MmiDummy.h"
#include "MmiMmi.h"
#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiDialogs.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif
#include "MmiBookShared.h"
#include "mmiSmsMenu.h"
#include "Mmiicons.h"
#include "mmiColours.h"

#include "MmiCPHS.h"
// Jun 23 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
#include "mfw_ffs.h"
/* SPR759 - SH */
#ifdef MMI_HOMEZONE_ENABLED
#include "MmiHomezone.h"
#endif

/* 30th May,2006     OMAPS00079650     a0393213 (R.Prabakar)
    Description : CPHS Call Forwarding feature implementation*/
    
#include "MmiServices.h"
//x0pleela 08 June, 2006  DR:OMAPS00079607
//Including this header file to get the definition of getRoamingStatus()
#ifdef FF_CPHS
#include "MmiNetwork.h"
#endif



/******************************************************************************
									      
			       Definitions
									      
*******************************************************************************/
//#define CPHS_PROV_TEST /*Test to bypass the reading of the CPHS field from the SIM card*/
char g_pin2[MAX_PIN+1];//xvilliva SPR17291

typedef enum


{
	INFO_NUM_LIST,
	MAILBOX_LIST, 
	MAILBOX_LIST_CALL
}
	T_MENU_TYPE;
	

static MfwMnuAttr InfoNum_menuAttrib =
{
    &melody_menuArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu	    */
    (U8)-1, 				/* use default font	    */
    NULL,				/* with these items	    */
    0,					/* number of items     */
    COLOUR_LIST_XX, TxtNull, NULL, MNUATTRSPARE

};

typedef struct 
{
	 /* administrative data */

    T_MMI_CONTROL	mmi_control;
    T_MFW_HND		win;
    T_MFW_HND		parent_win;
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA	editor_data;	/* SPR#1428 - SH - New Editor data */
#else /* NEW_EDITOR */
	T_EDITOR_DATA		editor_data;
#endif /* NEW_EDITOR */

    /* associated handlers */

    T_MFW_HND		kbd;
    T_MFW_HND		kbd_long;
    T_MFW_HND		menu;
	T_MFW_HND		menu_tim;
	UBYTE				status_of_timer; /* use for the keypadlock */

	T_MFW_HND mailbox_win;
	T_MFW_CPHS_ENTRY cphs_voicemail_num;
	T_MFW_CPHS_VC_STAT* cphs_voicemail_status;
	T_MFW_CPHS_STATUS cphs_status;
	T_MFW_CPHS_OP_NAME* operator_name;

}  T_MMI_CPHS_DATA;


typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND	   numbers_win; 				   /* MFW win handler	   */
  T_MFW_HND	   parent;
  T_MFW_HND	   melody_handler;	   /* MFW sms handler	   */
  unsigned char      local_status;
  ListMenuData	   * menu_list_data;
} T_INFO_NUM_INFO;

#ifdef FF_CPHS_REL4  
typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND	   msp_win; 				   /* MFW win handler	   */
  T_MFW_HND	   parent;
  ListMenuData	   * menu_list_data;
} T_MSP_INFO;

typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND	   mwis_win; 				   /* MFW win handler	   */
  T_MFW_HND	   parent;
  ListMenuData	   * menu_list_data;
} T_MWIS_INFO;

#endif

//x0pleela 23 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
#define MAX_LINE_NAME_LEN (20+1)

typedef struct 
{
	 /* administrative data */

    T_MMI_CONTROL	mmi_control;
    T_MFW_HND		win;
    T_MFW_HND		parent_win;
    T_MFW_HND		    input_number_win;
#ifndef NEW_EDITOR
	MfwEdtAttr			edt_attr_name;
#endif /* NEW_EDITOR */
UBYTE				edt_buf_name[MAX_LINE_NAME_LEN];
}  T_ALS_DATA;


#endif

#define E_INIT 1



#define CPHS_TELESERVICES_BYTE_VALUE	0x06
#define ALS_MASK						0x80

#define CPHS_INFONUMS_BYTE_VALUE		0xC0
#define INFONUM_MASK					0xFF

//x0pleela 23 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
static UBYTE Als_flag; //Stores the status of ALS from CSP data
#endif

#ifdef FF_CPHS_REL4
static BOOL MSP_flag;
BOOL msp_register;
UBYTE msp_reg_profid;
UBYTE prof_sel;
BOOL interrogate= 0;
T_MFW_CPHS_MSP_INFO* msp_info;
BOOL mwis_flag = FALSE;
#endif




/******************************************************************************
									      
			       Static Global variables
									      
*******************************************************************************/

static T_MFW_HND cphs_wait_dialog_win = NULL;
static T_DISPLAY_DATA 	mmi_cphs_wait_dialog_data = {0};
static T_MFW_CPHS_ENTRY cphs_voicemail_num;
static T_MFW_CPHS_INFONUM_ENTRY current_info_num;
static T_MFW_CPHS_VC_NUM cphs_voicemail_numbers;
static T_MFW_CPHS_VC_STAT* cphs_voicemail_status;
static T_MFW_CPHS_STATUS cphs_status = CPHS_NotPresent;
static T_MFW_CPHS_OP_NAME operator_name;
static T_MFW_CPHS_DV_STAT call_forwarding_status;
#ifdef FF_CPHS_REL4
T_MFW_CPHS_REL4_DV_STAT call_forwarding_status_rel4;
static T_MFW_CPHS_REL4_MBI cphs_rel4_mbi; /*used to cache mbi info-should be updated immediately whenever mbi changes*/
static T_MFW_CPHS_REL4_MBDN cphs_rel4_mbdn; /*used to cache mbdn info-should be updated immediately whenever mbdn changes*/
static T_MFW_HND mailbox_waiting_dialog;
static T_MFW_CPHS_REL4_MAILBOX_TYPE mailbox_type; /*used to store the mailbox type for which an action is performed*/
#endif
static T_MFW_CPHS_CSP customer_service_profile;
static T_MFW_CPHS_INFONUM_LIST* info_numbers[4]= {NULL, NULL, NULL, NULL};
// Oct 11, 2006 REF:OMAPS00098287  x0039928
T_MFW_CPHS_ALS_STATUS als_status;
BOOL als_bootup;

static UBYTE info_num_level;
static USHORT info_num_index;

static T_MENU_TYPE type_of_list = INFO_NUM_LIST;

/******************************************************************************
									      
			       Function Prototypes
									      
*******************************************************************************/
int InfoNumbers2(void); // RAVI

int show_cphs_mb_number(T_MFW_CPHS_ENTRY* mb_number);
static void mailbox_edit_cb( T_MFW_HND win, USHORT Identifier,UBYTE reason);
static int cphs_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
#ifdef FF_CPHS_REL4
static int msp_cb (T_MFW_EVENT event, T_MFW_WIN * win);
#endif
int cphs_mfw_cb(T_MFW_EVENT event, void* para);
void cphs_dialog_cb(T_MFW_HND win, USHORT e,  SHORT identifier, void *parameter);
T_MFW_HND mmi_cphs_create(T_MFW_HND parent_window);
T_MFW_HND	edit_mailbox_start(T_MFW_HND parent_window);

static T_MFW_HND  buildInfoNumberMenu( MfwHnd parent_window);
void info_num_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
void InfonumList_cb(T_MFW_HND * Parent, ListMenuData * ListData);
static void InfoNum_destroy(MfwHnd own_window);
static int info_num_win_cb (MfwEvt e, MfwWin *w) ;
void info_num_display_cb(T_MFW_HND win, USHORT identifier, UBYTE reason);
EXTERN void  cphs_support_check(void);  // ADDED BY RAVI - 29-11-2005

//x0pleela 23 May, 2006  DR:OMAPS00070657
#ifdef FF_CPHS
void mmi_cphs_get_als_info( void );

/* x0045876, 14-Aug-2006 (WR - function declared implicitly)*/
EXTERN void setAOCflag(int aocValue);
#endif




/******************************************************************************
									      
			       Public functions
									      
*******************************************************************************/
/*******************************************************************************

 $Function:	mmi_cphs_init()

 $Description:	initiatialise cphs module
 
 $Returns:		mfw handle

 $Arguments:	none
		
*******************************************************************************/
T_MFW_HND mmi_cphs_init()
{	
	T_MFW_HND status;

	TRACE_FUNCTION("mmi_cphs_init()");
	cphs_init();			//initialise MFW CPHS module

	status = cphs_create(NULL, 0xFFFFF, cphs_mfw_cb); //create handler for MFW CPHS EVENTS
	
	info_num_level = 0;
	info_num_index = 1;


#ifndef FF_CPHS_REL4
#ifdef FF_CPHS
// Oct 11, 2006 REF:OMAPS00098287  x0039928
// Fix: als update is done in idle_draw_main_idle
//	mmi_cphs_get_als_info();
       als_bootup = TRUE;
#else
	cphs_get_als_info(&als_status);
#endif
#endif

	return status;
}

void mmi_cphs_refresh()
{	
//x0pleela 02 June, 2006  DR: OMAPS00079688
//Initialising CPHS
#ifdef FF_CPHS
	if (sAT_PercentCPHS(CMD_SRC_LCL,1) /* 1 => ACI_CPHS_INIT */
                  NEQ AT_EXCT)
      {
                TRACE_ERROR("Abnormal error Not able to turn on CPHS in ACI");
      }
#endif
	cphs_support_check();	//check if cphs supported
	/*retrieval of other values set off by callback function cphs_mfw_cb
	so when the CPHS status is retrieved, the operator name is then requested and 
	when it has returned, then the CSP is requested etc etec  */
		
}
/*******************************************************************************

 $Function:	cphs_E_plus_ALS_supported

 $Description:	returns TRUE if SIM is from E-plus, FALSE otherwise 
 
 $Returns:		status BOOL

 $Arguments:	none
		
*******************************************************************************/
BOOL cphs_E_plus_ALS_supported()
{	char IMSI[17];
	char MNC_MCC[6];
	char E_plus_MNC_MCC[] = "26203";
	int i=0;
	T_ACI_RETURN result;
//	char debug[40];  // RAVI
	
	result = qAT_PlusCIMI(CMD_SRC_LCL, IMSI);
	while (result != AT_CMPL  && i < 5)
	{
		i++;
	}
		
	strncpy(MNC_MCC, IMSI, 5);
	MNC_MCC[5] =0;//terminate string
	TRACE_EVENT("E-plus IMSI?");
	TRACE_EVENT(MNC_MCC);
	if (!strncmp(E_plus_MNC_MCC, MNC_MCC, 5)) //if e-plus IMSI
	{	
		return TRUE;

	}
	else
	{	TRACE_EVENT("Not e-plus IMSI");
		return FALSE;
	}

}
/*******************************************************************************

 $Function:	cphs_als_status

 $Description:	display which ALS line is enabled
 
 $Returns:		status int

 $Arguments:	menu, tag (not used)
		
*******************************************************************************/
int cphs_als_status(MfwMnu* m, MfwMnuItem* i)
{	T_MFW_HND		parent	     = mfwParent( mfw_header());
	UBYTE line;
	line = GetAlsLine();
#ifndef CPHS_PROV_TEST
	if (CphsPresent() == TRUE || cphs_E_plus_ALS_supported())
#endif
	{
		if (als_status.selectedLine != MFW_SERV_NotPresent)
			switch(line)
			{
				case (MFW_SERV_LINE1): ShowMessage(parent, TxtLine1, TxtActivated);break;
				case (MFW_SERV_LINE2): ShowMessage(parent, TxtLine2, TxtActivated);break;
				default: ShowMessage(parent, TxtReadError, TxtNull);break;
			}
		else
			ShowMessage(parent, TxtReadError, TxtNull);
	}
#ifndef CPHS_PROV_TEST
	else
		ShowMessage(parent, TxtNotSupported, TxtNull);
#else
		cphs_support_check();
#endif

	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:	       cphs_lock_als

 $Description:	       Lock ALS Line Selection from menu 
 
 $Returns:		status int

 $Arguments:	       menu, tag (not used)
		
*******************************************************************************/
int cphs_lock_als(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND		parent	     = mfwParent( mfw_header());
	T_MFW			retVal;

	TRACE_FUNCTION("cphs_lock_als");

#ifndef CPHS_PROV_TEST
	if (!ALSPresent(NULL, NULL, NULL))
#endif
	{
		//xvilliva SPR17291 
		//We call sAT_PlusCLCK(), to update the value in FFS. This is the value used by
		//ACI to estabilish a call on Line1 or Line2.
#ifndef FF_2TO1_PS 
            /*a0393213 warnings removal - CLCK_FAC_Al changed to FAC_Al as a result of ACI enum change*/
		sAT_PlusCLCK(CMD_SRC_LCL,FAC_Al,CLCK_MOD_Lock,g_pin2,
				((als_status.selectedLine == MFW_SERV_LINE1) ? CLASS_Vce : CLASS_AuxVce));//
#endif
		retVal = cphs_set_als_status(MFW_LINE_LOCKED);
	//x0pleela 11 June, 2006  DR:OMAPS00079692
	 //changed from CPHS_OK to MFW_CPHS_OK
		if (retVal == MFW_CPHS_OK)
		{
			als_status.status = MFW_LINE_LOCKED;
			ShowMessage(parent, TxtAlsLocked, TxtNull);
		}
	}
#ifndef CPHS_PROV_TEST
	else
		ShowMessage(parent, TxtNotSupported, TxtNull);
#endif

	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:	       cphs_unlock_als

 $Description:	       Unlock ALS Line Selection from menu 
 
 $Returns:		status int

 $Arguments:	       menu, tag (not used)
		
*******************************************************************************/
int cphs_unlock_als(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND		parent	     = mfwParent( mfw_header());
	T_MFW			retVal;

	TRACE_FUNCTION("cphs_unlock_als");

#ifndef CPHS_PROV_TEST
	if (!ALSPresent(NULL, NULL, NULL))
#endif
	{
		//xvilliva SPR17291
		//We call sAT_PlusCLCK(), to update the value in FFS. This is the value used by
		//ACI to estabilish a call on Line1 or Line2. 
#ifndef FF_2TO1_PS
             /*a0393213 warnings removal - CLCK_FAC_Al changed to FAC_Al as a result of ACI enum change*/
		sAT_PlusCLCK(CMD_SRC_LCL,FAC_Al,CLCK_MOD_Unlock,g_pin2,((als_status.selectedLine == MFW_SERV_LINE1) ? CLASS_Vce : CLASS_AuxVce));//
#endif
		retVal = cphs_set_als_status(MFW_LINE_UNLOCKED);
	//x0pleela 11 June, 2006  DR:OMAPS00079692
	 //changed from CPHS_OK to MFW_CPHS_OK
		if (retVal == MFW_CPHS_OK)
		{
			als_status.status = MFW_LINE_UNLOCKED;
			ShowMessage(parent, TxtAlsUnlocked, TxtNull);
		}
	}
#ifndef CPHS_PROV_TEST
	else
		ShowMessage(parent, TxtNotSupported, TxtNull);
#endif

	return MFW_EVENT_CONSUMED;
}

#ifdef FF_CPHS_REL4

/*******************************************************************************

 $Function:     item_flag_isMWISFilePresent

 $Description:  return 0 if MWIS file is present else 1
 
*******************************************************************************/
int item_flag_isMWISFilePresent(struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi)
{
  TRACE_FUNCTION("item_flag_isMBDNFilePresent");
  if(isMWISFilePresent())
  	return 0;
  else
  	return 1;
}

/*******************************************************************************

 $Function:     item_flag_isMBDNFileNotPresent

 $Description:  return 1 if MBDN file is present else 0
 
*******************************************************************************/
int item_flag_isMBDNFileNotPresent(struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi)
{
  TRACE_FUNCTION("item_flag_isMBDNFilePresent");
  if(isMBDNFilePresent())
  	return 1;
  else
  	return 0;
}
/*******************************************************************************

 $Function:     item_flag_isMBDNFilePresent

 $Description:  return 0 if MBDN file is present else 1
 
*******************************************************************************/

int item_flag_isMBDNFilePresent(struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi)
{
  TRACE_FUNCTION("item_flag_isMBDNFileNotPresent");
  if(isMBDNFilePresent())
  	return 0;
  else
  	return 1;
}


/*******************************************************************************

 $Function:	       mmi_msp_interrogate_done

 $Description:	       Interrogation is finished. 
 
 $Returns:		void

 $Arguments:	       void
		
*******************************************************************************/

void mmi_msp_interrogate_done(void)
{
	TRACE_FUNCTION("mmi_msp_interrogate_done()");
	
	/*a0393213 cphs rel4*/
	if(isCFISFilePresent())
	cphs_get_divert_status();

	/*invalidate the local cache of mbi and mbdn*/
	cphs_rel4_mbi.valid=0x00;
	cphs_rel4_mbdn.valid=0x00;
}

/*******************************************************************************

 $Function:	       mmi_msp_interrogate

 $Description:	       Start MSP interrogation
 
 $Returns:		void

 $Arguments:	       void
		
*******************************************************************************/

void mmi_msp_interrogate(void)
{
	char actstr[8];
	TRACE_FUNCTION("mmi_msp_interrogate()");
	
	interrogate = 1;
	strcpy(actstr, "*#59#");
       ss_execute_transaction((UBYTE*)actstr, 0);
}

/*******************************************************************************

 $Function:	       msp_cb

 $Description:	      Window event handler
 
 $Returns:		status int

 $Arguments:	       event, window handler
		
*******************************************************************************/

static int mwis_cb (MfwEvt e, MfwWin *w)    /* yyy window event handler */
{
	TRACE_EVENT ("mwis_cb()");
	
	switch (e)
	{
	case MfwWinVisible:  /* window is visible  */
		break;
	case MfwWinFocussed: /* input focus / selected	 */	
	case MfwWinDelete:   /* window will be deleted	 */   
		
	default:	    
		return MFW_EVENT_REJECTED;
	}
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:	       msp_cb

 $Description:	      Window event handler
 
 $Returns:		status int

 $Arguments:	       event, window handler
		
*******************************************************************************/

static int msp_cb (MfwEvt e, MfwWin *w)    /* yyy window event handler */
{
	TRACE_EVENT ("msp_cb()");
	
	switch (e)
	{
	case MfwWinVisible:  /* window is visible  */
		break;
	case MfwWinFocussed: /* input focus / selected	 */	
	case MfwWinDelete:   /* window will be deleted	 */   
		
	default:	    
		return MFW_EVENT_REJECTED;
	}
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:	       MSPPresent

 $Description:	       MSP is provisioned.
 
 $Returns:		status USHORT

 $Arguments:	       menu tag, attribute, item tag 
		
*******************************************************************************/

USHORT MSPPresent( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	TRACE_EVENT_P1("MSPPresent - %d ", msp_info->count);
	if(MSP_flag)
		return 0;
	else
		return 1;
}

/*******************************************************************************

 $Function:	       cphs_select_profile

 $Description:	       Select a profile for registration
 
 $Returns:		void

 $Arguments:	       profile id
		
*******************************************************************************/

void cphs_select_profile(int pf_id)
{
	char seq[6];

	TRACE_EVENT_P1("cphs_select_profile - %d", pf_id);
	strcpy(seq,"*59");
	switch(pf_id)
	{
		case 1:
			strcat(seq,"1");
			break;
		case 2:
			strcat(seq,"2");
			break;
		case 3:
			strcat(seq,"3");
			break;
		case 4:
			strcat(seq,"4");
			break;
		default:
			break;
	}
	strcat(seq,"#");

	msp_register = TRUE;
	msp_reg_profid = pf_id;
	sAT_Dn(CMD_SRC_LCL,seq,-1,-1,-1);
}

/*******************************************************************************

 $Function:	       msp_menu_destroy

 $Description:	       Destroy menu
 
 $Returns:		void

 $Arguments:	       window handler
		
*******************************************************************************/

static void msp_menu_destroy(MfwHnd window)
{
	T_MFW_WIN     * win_data  = ((T_MFW_HDR *)window)->data;
	T_MSP_INFO * data = (T_MSP_INFO *)win_data->user;

	TRACE_FUNCTION ("msp_menu_destroy");

	if (window == NULL)
	{
		return;
	}

	if (data)
	{
		win_delete(data->msp_win);
		
		/*
		** Free Memory
		*/
		mfwFree((void *)data, sizeof (T_MSP_INFO));
	}
}

/*******************************************************************************

 $Function:	       mwis_menu_destroy

 $Description:	       Destroy menu
 
 $Returns:		void

 $Arguments:	       window handler
		
*******************************************************************************/

static void mwis_menu_destroy(MfwHnd window)
{
	T_MFW_WIN     * win_data  = ((T_MFW_HDR *)window)->data;
	T_MWIS_INFO * data = (T_MWIS_INFO *)win_data->user;

	TRACE_FUNCTION ("mwis_menu_destroy");

	if (window == NULL)
	{
		return;
	}

	if (data)
	{
		win_delete(data->mwis_win);
		
		/*
		** Free Memory
		*/
		mfwFree((void *)data, sizeof (T_MWIS_INFO));
	}
}

/*******************************************************************************

 $Function:	       MwisList_cb

 $Description:	       Callback for MWIS list
 
 $Returns:		void

 $Arguments:	       parent window, list data
		
*******************************************************************************/

void MwisList_cb(T_MFW_HND * Parent, ListMenuData * ListData)
{
	T_MFW_WIN			*win_data	= ((T_MFW_HDR *)Parent)->data;
	T_MWIS_INFO *data		= (T_MWIS_INFO *)win_data->user;
	TRACE_FUNCTION ("MwisList_cb()");

	if ((ListData->Reason EQ LISTS_REASON_BACK) ||
	     (ListData->Reason EQ LISTS_REASON_CLEAR) ||
	     (ListData->Reason EQ LISTS_REASON_HANGUP))
	{
		/*
		** Destroy the window --- Not sure if this is required! ... leave it out for now
		*/
		listsDestroy(ListData->win);

		/*
		** Free the memory set aside for ALL the devices to be listed.
		*/
		if (data->menu_list_data != NULL)
		{
			FREE_MEMORY ((void *)data->menu_list_data->List, (U16)(4 * sizeof (T_MFW_MNU_ITEM)));
			FREE_MEMORY ((void *)data->menu_list_data, sizeof (ListMenuData));
		}

		mwis_menu_destroy(data->mwis_win);
		
	}
}

/*******************************************************************************

 $Function:	       mwis_exec_cb

 $Description:	       Dialog function to display the mwis list
 
 $Returns:		void

 $Arguments:	       window handler, event, value and parameter
		
*******************************************************************************/

void mwis_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
	T_MWIS_INFO * data = (T_MWIS_INFO*)win_data->user;
	T_MFW_CPHS_REL4_MWIS * mwis_ptr;
	int i;
	char temp[4][50];

	switch (event)
	{
	case E_INIT:

		TRACE_EVENT(">>>> mwis_exec_cb() Event:E_INIT");

		 /* initialization of administrative data */


		data->menu_list_data = (ListMenuData *)ALLOC_MEMORY(sizeof(ListMenuData));

		if(data->menu_list_data == 0)
		{
			TRACE_EVENT("Failed memory alloc 1 ");
			return;
		}
	
		data->menu_list_data->List = (T_MFW_MNU_ITEM *)ALLOC_MEMORY( 4 * sizeof(T_MFW_MNU_ITEM) );

		if(data->menu_list_data->List == 0)
		{
			TRACE_EVENT("Failed memory alloc 2");
			return;
		}

			mwis_ptr = get_mwis_stat(prof_sel);

			sprintf(temp[0],"Voice Mesage - %d ", mwis_ptr->voicemail);
			mnuInitDataItem(&data->menu_list_data->List[0]);
			data->menu_list_data->List[0].flagFunc = item_flag_none;
			data->menu_list_data->List[0].str  = temp[0];

			sprintf(temp[1],"Fax Message - %d ", mwis_ptr->fax);
			mnuInitDataItem(&data->menu_list_data->List[1]);
			data->menu_list_data->List[1].flagFunc = item_flag_none;
			data->menu_list_data->List[1].str  = temp[1];

			sprintf(temp[2],"Electronic Message - %d ", mwis_ptr->electronic);
			mnuInitDataItem(&data->menu_list_data->List[2]);
			data->menu_list_data->List[2].flagFunc = item_flag_none;
			data->menu_list_data->List[2].str  = temp[2];

			sprintf(temp[3],"Other Message - %d ", mwis_ptr->other);
			mnuInitDataItem(&data->menu_list_data->List[3]);
			data->menu_list_data->List[3].flagFunc = item_flag_none;
			data->menu_list_data->List[3].str  = temp[3];

		data->menu_list_data->ListLength = 4;
		data->menu_list_data->ListPosition = 0;
		data->menu_list_data->CursorPosition = 0;
		data->menu_list_data->SnapshotSize = 4;
		data->menu_list_data->Font = 0;
		data->menu_list_data->LeftSoftKey = TxtNull;
		data->menu_list_data->RightSoftKey = TxtSoftBack;
		data->menu_list_data->KeyEvents = KEY_RIGHT;
		data->menu_list_data->Reason = 0;
		data->menu_list_data->Strings = TRUE;
		data->menu_list_data->Attr   = &InfoNum_menuAttrib;
		data->menu_list_data->autoDestroy    = FALSE;
		
		/* Create the dynamic menu window */
	    listDisplayListMenu(win, data->menu_list_data,(ListCbFunc)MwisList_cb,0);

		break;
	default:
		break;
		}

}

/*******************************************************************************

 $Function:	       mmi_build_mwis_menu

 $Description:	       Create mwis menu
 
 $Returns:		window handler

 $Arguments:	       parent window handler
		
*******************************************************************************/

static T_MFW_HND mmi_build_mwis_menu( MfwHnd parent_window)
{
	T_MWIS_INFO *  data = (T_MWIS_INFO *)ALLOC_MEMORY (sizeof (T_MWIS_INFO));
	T_MFW_WIN  * win;
	TRACE_FUNCTION("mmi_build_mwis_menu");

    /*
     * Create window handler
     */

    data->mwis_win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)mwis_cb);
    if (data->mwis_win EQ NULL)
	{
	   return NULL;
	}

	TRACE_EVENT(">>>> mwis menu window created: " );
    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)mwis_exec_cb;
    data->mmi_control.data   = data;
    win 		     = ((T_MFW_HDR *)data->mwis_win)->data;
    win->user		     = (void *)data;
    data->parent	     = parent_window;
	winShow(data->mwis_win);
    /*
     * return window handle
     */

     return data->mwis_win;
}


/*******************************************************************************

 $Function:	       MspList_cb

 $Description:	       Callback for MSP list
 
 $Returns:		void

 $Arguments:	       parent window, list data
		
*******************************************************************************/

void MspList_cb(T_MFW_HND * Parent, ListMenuData * ListData)
{
	T_MFW_WIN			*win_data	= ((T_MFW_HDR *)Parent)->data;
	T_MSP_INFO *data		= (T_MSP_INFO *)win_data->user;
	TRACE_FUNCTION ("MspList_cb()");

	if ((ListData->Reason EQ LISTS_REASON_BACK) ||
	     (ListData->Reason EQ LISTS_REASON_CLEAR) ||
	     (ListData->Reason EQ LISTS_REASON_HANGUP))
	{
		/*
		** Destroy the window --- Not sure if this is required! ... leave it out for now
		*/
		listsDestroy(ListData->win);

		/*
		** Free the memory set aside for ALL the devices to be listed.
		*/
		if (data->menu_list_data != NULL)
		{
			FREE_MEMORY ((void *)data->menu_list_data->List, (U16)(msp_info->count * sizeof (T_MFW_MNU_ITEM)));
			FREE_MEMORY ((void *)data->menu_list_data, sizeof (ListMenuData));
		}

		msp_menu_destroy(data->msp_win);
		mwis_flag = FALSE;
	}
	else if(ListData->Reason EQ LISTS_REASON_SELECT)
	  {
		TRACE_EVENT_P1("list position is %d",ListData->ListPosition);
		if(mwis_flag == FALSE)
		cphs_select_profile(ListData->ListPosition);
		else
		{
			T_MFW_HND win;
			T_MFW_HND parent_win = mfw_parent(mfw_header());

			TRACE_FUNCTION("build_mwis_list()");

			mwis_flag = TRUE;
			prof_sel = ListData->ListPosition;
			win = mmi_build_mwis_menu(parent_win);
			if (win NEQ NULL)
			{
				SEND_EVENT (win, E_INIT, 0, 0);
			}
		}
	  }
}


/*******************************************************************************

 $Function:	       msp_exec_cb

 $Description:	       Dialog function to display the msp list
 
 $Returns:		void

 $Arguments:	       window handler, event, value and parameter
		
*******************************************************************************/

void msp_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
	T_MSP_INFO* data = (T_MSP_INFO*)win_data->user;
	int i;

	switch (event)
	{
	case E_INIT:

		TRACE_EVENT(">>>> msp_exec_cb() Event:E_INIT");

		 /* initialization of administrative data */


		data->menu_list_data = (ListMenuData *)ALLOC_MEMORY(sizeof(ListMenuData));

		if(data->menu_list_data == 0)
		{
			TRACE_EVENT("Failed memory alloc 1 ");
			return;
		}
	
		data->menu_list_data->List = (T_MFW_MNU_ITEM *)ALLOC_MEMORY( msp_info->count * sizeof(T_MFW_MNU_ITEM) );

		if(data->menu_list_data->List == 0)
		{
			TRACE_EVENT("Failed memory alloc 2");
			return;
		}

		for (i = 0; i < msp_info->count; i++)
		{
			mnuInitDataItem(&data->menu_list_data->List[i]);
			data->menu_list_data->List[i].flagFunc = item_flag_none;
			data->menu_list_data->List[i].str  = (char *)msp_info->profile[i].alpha;
		}

		data->menu_list_data->ListLength =msp_info->count;
		data->menu_list_data->ListPosition = 1;
		data->menu_list_data->CursorPosition = 1;
		data->menu_list_data->SnapshotSize = msp_info->count;
		data->menu_list_data->Font = 0;
		data->menu_list_data->LeftSoftKey = TxtSoftSelect;
		data->menu_list_data->RightSoftKey = TxtSoftBack;
		data->menu_list_data->KeyEvents = KEY_ALL;
		data->menu_list_data->Reason = 0;
		data->menu_list_data->Strings = TRUE;
		data->menu_list_data->Attr   = &InfoNum_menuAttrib;
		data->menu_list_data->autoDestroy    = FALSE;
		
		/* Create the dynamic menu window */
	    listDisplayListMenu(win, data->menu_list_data,(ListCbFunc)MspList_cb,0);

		break;
	default:
		break;
		}

}


/*******************************************************************************

 $Function:	       mmi_build_msp_menu

 $Description:	       Create MSP menu
 
 $Returns:		window handler

 $Arguments:	       parent window handler
		
*******************************************************************************/

static T_MFW_HND mmi_build_msp_menu( MfwHnd parent_window)
{
	T_MSP_INFO *  data = (T_MSP_INFO *)ALLOC_MEMORY (sizeof (T_MSP_INFO));
	T_MFW_WIN  * win;
	TRACE_FUNCTION("mmi_build_msp_menu");

    /*
     * Create window handler
     */

    data->msp_win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)msp_cb);
    if (data->msp_win EQ NULL)
	{
	   return NULL;
	}

	TRACE_EVENT(">>>> msp menu window created: " );
    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)msp_exec_cb;
    data->mmi_control.data   = data;
    win 		     = ((T_MFW_HDR *)data->msp_win)->data;
    win->user		     = (void *)data;
    data->parent	     = parent_window;
	winShow(data->msp_win);
    /*
     * return window handle
     */

     return data->msp_win;
}

/*******************************************************************************

 $Function:	       build_profile_list

 $Description:	       generate profile list
 
 $Returns:		status int

 $Arguments:	       menu, menu item
		
*******************************************************************************/

int build_profile_list(MfwMnu* m, MfwMnuItem* i)
{
		T_MFW_HND win;
		T_MFW_HND parent_win = mfw_parent(mfw_header());

		TRACE_FUNCTION("build_profile_list()");
		
		win = mmi_build_msp_menu(parent_win);
		if (win NEQ NULL)
		{
		SEND_EVENT (win, E_INIT, 0, 0);
		}
		return MFW_EVENT_CONSUMED;
}

int build_profile_list1(MfwMnu* m, MfwMnuItem* i)
{
		T_MFW_HND win;
		T_MFW_HND parent_win = mfw_parent(mfw_header());

		TRACE_FUNCTION("build_profile_list()");

		mwis_flag = TRUE;
		
		win = mmi_build_msp_menu(parent_win);
		if (win NEQ NULL)
		{
		SEND_EVENT (win, E_INIT, 0, 0);
		}
		return MFW_EVENT_CONSUMED;
}

#endif
/*******************************************************************************

 $Function:	cphs_line1_select

 $Description:	Enable ALS Line1 from menu 
 
 $Returns:		status int

 $Arguments:	menu, tag (not used)
		
*******************************************************************************/
int cphs_line1_select(MfwMnu* m, MfwMnuItem* i)
{	
	T_MFW_HND		parent	     = mfwParent( mfw_header());
	TRACE_FUNCTION("cphs_line1_select");
#ifndef CPHS_PROV_TEST
	if (!ALSPresent(NULL, NULL, NULL))
#endif
	{	cphs_select_line(MFW_SERV_LINE1);
		/*cphs_get_divert_status();*//*a0393213 - the call forward icon is now shown taking both the lines into consideration*/
	}
#ifndef CPHS_PROV_TEST
	else
		ShowMessage(parent, TxtNotSupported, TxtNull);
#endif
	return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:	cphs_line2_select

 $Description:	Enable ALS Line2 from menu 
 
 $Returns:		status int

 $Arguments:	menu, tag (not used)
		
*******************************************************************************/
int cphs_line2_select(MfwMnu* m, MfwMnuItem* i)
{	

	T_MFW_HND		parent	     = mfwParent( mfw_header());
	TRACE_FUNCTION("cphs_line2_select");
#ifndef CPHS_PROV_TEST
	if (!ALSPresent(NULL, NULL, NULL))
#endif
	{	
		cphs_select_line(MFW_SERV_LINE2);
		/*cphs_get_divert_status();*//*a0393213 - the call forward icon is now shown taking both the lines into consideration*/
	}
#ifndef CPHS_PROV_TEST
	else
		ShowMessage(parent, TxtNotSupported, TxtNull);
#endif
	return MFW_EVENT_CONSUMED;
}

//x0pleela 23 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
/*******************************************************************************

 $Function:    	cphs_Line_Edit_cb
 
 $Description:	Callback function for  Line rename editor

 $Returns:		none

 $Arguments:	win - current window
 				Identifier - unique id
 				reason - event cause
*******************************************************************************/

static void cphs_Line_Edit_cb(T_MFW_HND win, USHORT Identifier, SHORT reason)
{	
	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	T_ALS_DATA* data     = (T_ALS_DATA *)win_data->user;
      T_DISPLAY_DATA   display_info;
      
switch (reason)
	{
	       case INFO_KCD_ALTERNATELEFT:
		case INFO_KCD_LEFT:
                           	{
                           	if( Identifier == LINE1 )
	                           	strcpy(FFS_flashData.line1,(char *)data->edt_buf_name);
				else
					strcpy(FFS_flashData.line2,(char *)data->edt_buf_name);

                           	flash_write();


                           	dlg_initDisplayData_TextId(&display_info, TxtNull, TxtNull,   TxtDone, TxtNull, COLOUR_STATUS_WELCOME);
		
				dlg_initDisplayData_events( &display_info, NULL, TWO_SECS, KEY_ALL );

		  		info_dialog(win,&display_info);
		  		
		  #ifdef NEW_EDITOR
			AUI_edit_Destroy(data->input_number_win);
		#else /* NEW_EDITOR */
			editor_destroy(data->input_number_win);
		#endif /* NEW_EDITOR */
			data->input_number_win = 0;
		//dspl_ClearAll();

		
		winDelete(win);
		}
            break;
              case INFO_KCD_RIGHT:
		case INFO_KCD_HUP:
				{
		/* SPR#1428 - SH - New Editor changes */
		#ifdef NEW_EDITOR
			AUI_edit_Destroy(data->input_number_win);
		#else /* NEW_EDITOR */
			editor_destroy(data->input_number_win);
		#endif /* NEW_EDITOR */
			data->input_number_win = 0;
		//dspl_ClearAll();
		winDelete(win);
		
			}
			break;

		default:
			TRACE_EVENT("Err: Default");
			break;
                           
                           
                           }   
}

static void cphs_edit_line_exec_cb(T_MFW_HND win, USHORT event, SHORT identifier, void *parameter)
{	
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_ALS_DATA* data = (T_ALS_DATA *)win_data->user;
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
	T_EDITOR_DATA editor_data;
#endif
	if( identifier==LINE1 )
		strcpy((char *) data->edt_buf_name,(char*)MmiRsrcGetText(TxtLine1));
	else
		strcpy((char *) data->edt_buf_name,(char*)MmiRsrcGetText(TxtLine2));
		
	TRACE_FUNCTION ("cphs_edit_line_exec_cb()");

	switch (event)
	{
		case E_INIT:
			
	#ifdef NEW_EDITOR
			AUI_edit_SetDefault(&editor_data);
			AUI_edit_SetDisplay(&editor_data, PHNO_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
			AUI_edit_SetEvents(&editor_data, identifier, FALSE, FOREVER, (T_AUI_EDIT_CB)cphs_Line_Edit_cb);
			AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtDelete, TxtNull, NULL);
			AUI_edit_SetAltTextStr(&editor_data, 1, TxtSoftOK, TRUE, TxtSoftBack);
			AUI_edit_SetMode(&editor_data, ED_MODE_ALPHA, ED_CURSOR_NONE);
			AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII,(UBYTE *) data->edt_buf_name, MAX_LINE_NAME_LEN);
			data->input_number_win=AUI_edit_Start(data->win, &editor_data);
#else /* NEW_EDITOR */

			SmsSend_loadEditDefault(&editor_data);

			bookSetEditAttributes( PHNO_EDITOR, COLOUR_EDITOR_XX, 0, edtCurBar1, 0, 
			(char *) data->edt_data.NumberBuffer, /*MAX_SEARCH_CHAR*/NUMBER_LENGTH, &editor_data.editor_attr);
			editor_data.editor_attr.text     = (char *)data->edt_buf_name; 

			editor_data.editor_attr.size     = MAX_LINE_NAME_LEN; 

			editor_data.TextId               = TxtNull;  				 							  
			editor_data.LeftSoftKey          = TxtSoftOK;
			editor_data.AlternateLeftSoftKey = TxtSoftOK;
			editor_data.RightSoftKey         = TxtDelete;
			editor_data.Identifier           = identifier ; 
			editor_data.mode				 = E_EDIT_DIGITS_MODE;
			editor_data.Callback             = (T_EDIT_CB)cphs_Line_Edit_cb;
			editor_data.destroyEditor		 = FALSE;
			data->input_number_win =editor_start(data->win, &editor_data);  /* start the editor */ 


			/* SPR#1428 - SH - New Editor changes */
			
#endif
			
			break;
		default:
			TRACE_EVENT("Err: Default");
			break;
	}
}

/*******************************************************************************

 $Function:    	cphs_edit_line_create

 $Description:	Create a window for entering the text
 
 $Returns:		window handle

 $Arguments:	parent_window - parent window.
 				
*******************************************************************************/


static T_MFW_HND cphs_edit_line_create(MfwHnd parent_window)
{
	T_ALS_DATA* data = (T_ALS_DATA *)ALLOC_MEMORY (sizeof (T_ALS_DATA));
	T_MFW_WIN  * win;

	TRACE_FUNCTION ("cphs_edit_line_create()");

	// Create window handler
	data->win = win_create (parent_window, 0, E_WIN_VISIBLE, NULL);
	if (data->win EQ NULL)
	{
		return NULL;
	}

	// connect the dialog data to the MFW-window
	data->mmi_control.dialog = (T_DIALOG_FUNC)cphs_edit_line_exec_cb;
	data->mmi_control.data   = data;
	win                      = ((T_MFW_HDR *)data->win)->data;
	win->user                = (MfwUserDataPtr)data;
	data->parent_win             = parent_window;

	winShow(data->win);
	return data->win;
}

/*******************************************************************************

 $Function:	cphs_edit_line1

 $Description:	edit line name from menu 
 
 $Returns:		status int

 $Arguments:	menu, tag (not used)
		
*******************************************************************************/
int  cphs_edit_line1(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	T_MFW_HND win = cphs_edit_line_create(parent_win);
	SEND_EVENT(win,E_INIT,LINE1,0);
	/* a0393213 (WR - missing return statement at end of non-void function "cphs_edit_line1")*/
	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:	cphs_edit_line2

 $Description:	edit line name from menu 
 
 $Returns:		status int

 $Arguments:	menu, tag (not used)
		
*******************************************************************************/
int  cphs_edit_line2(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	T_MFW_HND win = cphs_edit_line_create(parent_win);
	SEND_EVENT(win,E_INIT,LINE2,0);
	/* a0393213 (WR - missing return statement at end of non-void function "cphs_edit_line2")*/
	return MFW_EVENT_CONSUMED;
}
#endif  /*FF_CPHS*/

/*******************************************************************************

 $Function:	menu_cphs_call_mailbox_number

 $Description:	edit mailbox numbers from menu 
 
 $Returns:		status int

 $Arguments:	menu, tag (not used)
		
*******************************************************************************/
int menu_cphs_call_mailbox_number(MfwMnu* m, MfwMnuItem* i)
{   T_MFW_HND		parent	     = mfwParent( mfw_header());
	T_MFW_CPHS_INFO configuration;
	
	TRACE_FUNCTION("menu_cphs_call_mailbox_number");
	
	type_of_list = MAILBOX_LIST_CALL;
	cphs_config(&configuration);
	/***************************Go-lite Optimization changes Start***********************/
	//Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P2( "CPHS? %d Mailbox config info: %d", configuration.mbNum, CphsPresent());
	/***************************Go-lite Optimization changes end***********************/
	if (CphsMailboxPresent()==TRUE )
		cphs_get_mailbox();
	else
		M_callVoice(m, i);
    return MFW_EVENT_CONSUMED;		
}
/*******************************************************************************

 $Function:	menu_cphs_edit_mailbox_number

 $Description:	edit mailbox numbers from menu 
 
 $Returns:		status int

 $Arguments:	menu, tag (not used)
		
*******************************************************************************/
int menu_cphs_edit_mailbox_number(MfwMnu* m, MfwMnuItem* i)
{   T_MFW_HND		parent	     = mfwParent( mfw_header());
	T_MFW_CPHS_INFO configuration;

	TRACE_FUNCTION("menu_cphs_edit_mailbox_number");
	type_of_list = MAILBOX_LIST;
	
	cphs_config(&configuration);
	/***************************Go-lite Optimization changes Start***********************/
	//Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P2( "CPHS? %d Mailbox config info: %d", configuration.mbNum, CphsPresent());
	/***************************Go-lite Optimization changes end***********************/
	if (CphsMailboxPresent()==TRUE )
		cphs_get_mailbox();
	else
		M_exeVoice(m, i);
    return MFW_EVENT_CONSUMED;		
}


#ifdef FF_CPHS_REL4
/*******************************************************************************

 $Function:	menu_cphs_rel4_show_mailbox_number

 $Description:	menu handler function for any item in MailBox menu
 		
*******************************************************************************/
int menu_cphs_rel4_show_mailbox_number(MfwMnu* m, MfwMnuItem* i)
{
  if(cphs_rel4_mbi.valid==1)
  	{
  	/*already cached - no need to read SIM*/
       	sendCPHSSignal(E_CPHS_REL4_GET_MBI,&cphs_rel4_mbi );
  	}
  else
  	{
  	/*local copy not valid - read from SIM*/
         UBYTE msp_id=mfw_cphs_get_reg_profile();
         T_DISPLAY_DATA   display_info;
         TRACE_FUNCTION("menu_cphs_rel4_show_mailbox_number");
         switch (m->lCursor[m->level])
         	{
         	case 0:
       		TRACE_EVENT("voice selected");
       		mailbox_type=T_MFW_CPHS_REL4_MAILBOX_VOICE;
       		break;
       	case 1:
       		TRACE_EVENT("fax selected");
       		mailbox_type=T_MFW_CPHS_REL4_MAILBOX_FAX;
       		break;
       	case 2:
       		TRACE_EVENT("electronic selected");
       		mailbox_type=T_MFW_CPHS_REL4_MAILBOX_ELECTRONIC;
       		break;		
       	case 3:
       		TRACE_EVENT("other selected");
       		mailbox_type=T_MFW_CPHS_REL4_MAILBOX_OTHER;
       		break;			
       	default:
       		TRACE_ERROR("menu index out of range");
       		break;
         	}  	
          qAT_PercentMBI(CMD_SRC_LCL,msp_id);
		  
	    dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtPleaseWait, TxtNull, COLOUR_STATUS_WAIT);
	    dlg_initDisplayData_events( &display_info, NULL, FOREVER, 0 );

          mailbox_waiting_dialog=info_dialog (mfw_parent(mfwFocus), &display_info);
  
  	}
}
#endif
/*******************************************************************************

 $Function:	InfoNumbers

 $Description:	displays information number list from CPHS 

 $Returns:		MFW_EVENT_CONSUMED always

 $Arguments:	m, menu, i, item
 
*******************************************************************************/

int InfoNumbers( MfwMnu *m, MfwMnuItem *i )
{
	T_MFW_HND win;
    T_MFW_HND parent_win = mfw_parent(mfw_header());
	T_MFW_CPHS_INFO config_info;
	
	cphs_config(&config_info);
	/***************************Go-lite Optimization changes Start***********************/
//Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P1("Info Num config info: %d", config_info.iNum);
	/***************************Go-lite Optimization changes end***********************/
	if (CphsPresent() && config_info.iNum&0x03)
	{	if (info_numbers[0] != NULL&& info_numbers[0]->count != 0)
		{	type_of_list = INFO_NUM_LIST;
			info_num_level = 1;
			win = buildInfoNumberMenu(parent_win);
			
			if (win NEQ NULL)
			{
				SEND_EVENT (win, E_INIT, 0, 0);
			}
		}
		else
			ShowMessage(parent_win, TxtEmptyList, TxtNull);
	}
	else
		ShowMessage(parent_win, TxtNotSupported, TxtNull);
	
	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:	CphsRequestOperatorName()

 $Description:	returns operator name data structure from CPHS SIM data

 $Returns:		see above

 $Arguments:	none
 
*******************************************************************************/
T_MFW_CPHS_OP_NAME* CphsRequestOperatorName()
{	
	return &operator_name;
}

/*******************************************************************************

 $Function:	CphsPresent()

 $Description:	indicates whether CPHS sim or not

 $Returns:		TRUE if it is, FALSe if it isn't
 $Arguments:	none
 
*******************************************************************************/
UBYTE CphsPresent()
{
	//x0pleela 11 June, 2006  DR:OMAPS00079692
	 //changed from CPHS_OK to MFW_CPHS_OK
	if (cphs_status == MFW_CPHS_OK)
		return TRUE;
	else
	 {	 if (cphs_status == CPHS_NotPresent)
			return FALSE;
		else /*MC 21/05/02, CONQ5999->6007 Helps diagnose initialisation probs*/
			return CPHS_ERR;
	}
}
/*******************************************************************************

 $Function:	CphsMailboxPresent

 $Description:	indicates whether a CPHS SIM which supports mailbox numbers

 $Returns:		TRUE if it is, FALSe if it isn't
 $Arguments:	none
 
*******************************************************************************/
USHORT CphsMailboxPresent( /*struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi*/ )
{	T_MFW_CPHS_INFO config_info;

	TRACE_FUNCTION("CphsMailboxPresent");
	cphs_config(&config_info);

	if (config_info.mbNum &0x03 && CphsPresent())
		return /*0*/TRUE;
	else
		return /*1*/FALSE;

}
/*******************************************************************************

 $Function:	ALSPresent

 $Description:	indicates whether a CPHS SIM supports ALS by checking CSP

 $Returns:		0 if it does, 1 if it doesn't
 $Arguments:	menu tag, menu attribute tag, menu item tag (none of them used, 
				this is merely the format for menu display/suppression functions)
 
*******************************************************************************/
USHORT ALSPresent( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{ 
	//xvilliva SPR17291 - The pre-existing code was returning ALS not supported 
	//		      even if it the SIM supported. Changed the code to call 
	//		      tAT_PercentALS() and qAT_PercentALS(). 		

#ifndef FF_CPHS
	T_ACI_ALS_MOD ALSmode = (T_ACI_ALS_MOD)0;  // RAVI
#endif

//x0pleela 23 May, 2006  DR: OMAP00070657
//Returning the status of ALS based on CSP data
#ifdef FF_CPHS
	if( mmi_cphs_get_als_value() )
		return 0;
	else
		return 1;
#else

	tAT_PercentALS( CMD_SRC_NONE, &ALSmode );
	qAT_PercentALS( CMD_SRC_NONE, &ALSmode );

	// ALSmode = 1;//xvilliva  SPR17291 - IMP: Hardcoded for our SIMS - got to test on ALS sim.

	return   (!ALSmode);//returns 0 (if line1 or line2) returns 1 if not supported.
#endif
}

/*******************************************************************************

 $Function:	       CphsALSUnlocked

 $Description:	       Indicates whether a CPHS SIM's support of ALS has been locked by the User

 $Returns:		0 if it hasn't, 1 if it has
 $Arguments:	       menu tag, menu attribute tag, menu item tag (none of them used, 
				this is merely the format for menu display/suppression functions)
 
*******************************************************************************/
USHORT CphsALSUnlocked( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	USHORT retVal = CphsALSLocked(m, ma, mi);

	if (retVal == 0)
		retVal = 1;
	else
		retVal = 0;
	
	return (retVal);
}
/*******************************************************************************

 $Function:	       CphsALSUnlocked

 $Description:	       Indicates whether a CPHS SIM's support of ALS has been locked by the User

 $Returns:		0 if it has, 1 if it hasn't
 $Arguments:	       menu tag, menu attribute tag, menu item tag (none of them used, 
				this is merely the format for menu display/suppression functions)
 
*******************************************************************************/
USHORT CphsALSLocked( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	USHORT retVal;
	
	if (als_status.status == MFW_LINE_LOCKED)
		retVal = 0;
	else
		retVal = 1;
	
	return (retVal);
}
/*******************************************************************************

 $Function:	InfoNumbersPresent

 $Description:	indicates whether a CPHS SIM has Info Numbers by checking CSP

 $Returns:		0 if it does, 1 if it doesn't
 $Arguments:	none
 
*******************************************************************************/
USHORT InfoNumbersPresent( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{	T_MFW_CPHS_INFO config_info;
	
	int i,len;
	cphs_config(&config_info);
	/***************************Go-lite Optimization changes Start***********************/
	//Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P1("InfoNum config: %d", config_info.iNum);
	/***************************Go-lite Optimization changes end***********************/
	len = strlen((char*)customer_service_profile.csp); /* x0039928 - Lint warning fix */
	for(i=0; i<len; i+=2)	
	{
		if (customer_service_profile.csp[i] ==CPHS_INFONUMS_BYTE_VALUE)
			if (customer_service_profile.csp[(i+1)] & INFONUM_MASK)
				return 0; //ALS is supported
	}
	return 1;
}
/*******************************************************************************

 $Function:	GetCphsVoicemailStatus

 $Description:	Asks MFW to check voicemail status on CPHS SIM
				cphs_mfw_cb handles callback from this
 $Returns:		status of line passed as parameter
 $Arguments:	line
 
*******************************************************************************/
T_MFW_CFLAG_STATUS GetCphsVoicemailStatus(UBYTE line)
{	
	cphs_get_mailbox_status (); 
	TRACE_EVENT("Requesting voicemail status");
	
	/*SPR 2653, removed unneccessary code*/
	/*requesting the mailbox status will trigger an event from the MFW if
	CPHS is supported and that event will cause a voicemail indication window if a 
	voicemail flag has been set in the SIM*/
		return MFW_CFLAG_NotPresent;


}
/*******************************************************************************

 $Function:	GetCphsDivertStatus

 $Description:	get call forwarding status of specified line
				cphs_mfw_cb handles callback from this
 $Returns:		status of line passed as parameter
 $Arguments:	line
 
*******************************************************************************/

T_MFW_CFLAG_STATUS GetCphsDivertStatus(UBYTE line)
{	
	switch (line)
	{
		case MFW_SERV_LINE1:	return call_forwarding_status.line1; //break;  // RAVI
		case MFW_SERV_LINE2:		return call_forwarding_status.line2; // break; // RAVI
		case MFW_SERV_FAX:		return call_forwarding_status.fax; // break; // RAVI
		case MFW_SERV_DATA:		return call_forwarding_status.data; // break; // RAVI
		default:		return MFW_CFLAG_NotPresent;
	}
}

/*******************************************************************************

 $Function:	GetCphsRel4DivertStatus

 $Description:	get current call forwarding status 
 $Arguments:	none
 
*******************************************************************************/
#ifdef FF_CPHS_REL4
T_MFW_CPHS_REL4_DV_STAT GetCphsRel4DivertStatus()
{	
	TRACE_FUNCTION("GetCphsRel4DivertStatus()");
	return call_forwarding_status_rel4;
}


/*******************************************************************************

 $Function:	isCFISFilePresent

 $Description:	says whether the file is allocated and activated or not
 $Arguments:	none
 $Returns	    : TRUE - if present
 			FALSE - if absent
 
*******************************************************************************/
BOOL isCFISFilePresent()
{
	TRACE_FUNCTION("isCFISFilePresent()");
	if(sim_serv_table_check((UBYTE)SRV_No_55) == ALLOCATED_AND_ACTIVATED)
	{
		TRACE_EVENT("CFIS PRESENT");
		TRUE;
	}
	else
	{
		TRACE_EVENT("CFIS NOT PRESENT");
		FALSE;
	}
}

/*******************************************************************************

 $Function:	isMWISFilePresent

 $Description:	says whether the file is allocated and activated or not
 $Arguments:	none
 $Returns	    : TRUE - if present
 			FALSE - if absent
 
*******************************************************************************/
BOOL isMWISFilePresent(void)
{
	TRACE_FUNCTION("isMWISFilePresent()");
	if(sim_serv_table_check((UBYTE)SRV_No_54) == ALLOCATED_AND_ACTIVATED)
	{
		TRACE_EVENT("MWIS PRESENT");
		TRUE;
	}
	else
	{
		TRACE_EVENT("MWIS NOT PRESENT");
		FALSE;
	}
		
}

/*******************************************************************************

 $Function:	isMBDNFilePresent

 $Description:	says whether the file is allocated and activated or not
 $Arguments:	none
 $Returns	    : TRUE - if present
 			FALSE - if absent
 
*******************************************************************************/
BOOL isMBDNFilePresent(void)
{
	TRACE_FUNCTION("isMWISFilePresent()");
	if(sim_serv_table_check((UBYTE)SRV_No_53) == ALLOCATED_AND_ACTIVATED)
	{
		TRACE_EVENT("MBDN PRESENT");
		TRUE;
	}
	else
	{
		TRACE_EVENT("MBDN NOT PRESENT");
		FALSE;
	}
		
}
#endif

/*******************************************************************************

 $Function:	GetAlsLine
 $Description:	gets current als line
 $Returns:		current als line
 $Arguments:	line
 
*******************************************************************************/
UBYTE GetAlsLine()
{	
	return als_status.selectedLine;
}

/*******************************************************************************

 $Function:	ShowMessage
 $Description:	Displays screen with passed text ids
 $Returns:		nothing
 $Arguments:	parent window, 2 text ids
 
*******************************************************************************/
void ShowMessage(T_MFW_HND win, USHORT TextId1, USHORT TextId2)
{

	T_DISPLAY_DATA Dialog;

	/* Initialise the dialog control block with default information
	*/
	dlg_initDisplayData_TextId( &Dialog, TxtNull,  TxtNull, TextId1, TextId2 , COLOUR_STATUS);
	dlg_initDisplayData_events( &Dialog, NULL, THREE_SECS, KEY_LEFT| KEY_RIGHT|KEY_CLEAR );

	/* Show the dialog
	*/
	info_dialog( win, &Dialog );
}


/*******************************************************************************

 $Function:	mmi_cphs_wait_dialog
 $Description:	Displays the "Pls. wait" Dialog
 $Returns:	Nothing
 $Arguments:	parent window
 
*******************************************************************************/
void mmi_cphs_wait_dialog(T_MFW_HND win)
{

	cphs_wait_dialog_win = NULL;
	memset( (void *)&mmi_cphs_wait_dialog_data, 0, sizeof(T_DISPLAY_DATA) );

	/* Initialise the dialog control block with default information
	*/
	dlg_initDisplayData_TextId( &mmi_cphs_wait_dialog_data, TxtNull,  TxtNull, TxtPleaseWait, NULL , COLOUR_STATUS);
	dlg_initDisplayData_events( &mmi_cphs_wait_dialog_data, NULL, FOREVER, KEY_MAX );

	/* Show the dialog */
	cphs_wait_dialog_win = info_dialog( win, &mmi_cphs_wait_dialog_data );
}


/*******************************************************************************

 $Function:	mmi_cphs_wait_dialog_destroy
 $Description:	Deletes the "Pls. wait" Dialog
 $Returns:	Nothing
 $Arguments:	None
 
*******************************************************************************/
void mmi_cphs_wait_dialog_destroy(void)
{
	if ( cphs_wait_dialog_win != NULL )
	{
		SEND_EVENT( cphs_wait_dialog_win, DIALOG_DESTROY, 0, &mmi_cphs_wait_dialog_data );
	}
	
	cphs_wait_dialog_win = NULL;
	memset( (void *)&mmi_cphs_wait_dialog_data, 0, sizeof(T_DISPLAY_DATA) );
}


/******************************************************************************
									      
			     Private functions
									      
*******************************************************************************/

T_MFW_HND	edit_mailbox_start(T_MFW_HND parent_window)
{
		T_MFW_HND		win	      = mmi_cphs_create(parent_window);
		TRACE_FUNCTION("calc_start()");
	    if (win NEQ NULL)
		{
		SEND_EVENT(win, MAILBOX, NULL, NULL);
	       
		} 

	   return win;
} 





T_MFW_HND mmi_cphs_create(T_MFW_HND parent)
{	T_MMI_CPHS_DATA     * data = (T_MMI_CPHS_DATA *)ALLOC_MEMORY (sizeof (T_MMI_CPHS_DATA ));
	T_MFW_WIN  * mailbox_win;

	if (data EQ NULL)
	{
		return NULL;
	}

	// Create window handler
	data->mailbox_win = win_create (parent, 0, E_WIN_VISIBLE, (MfwCb)cphs_win_cb);
	if (data->mailbox_win EQ NULL)
	{
		
		return NULL;
	}
	// connect the dialog data to the MFW-window
	data->mmi_control.dialog = (T_DIALOG_FUNC)cphs_dialog_cb;
	data->mmi_control.data	 = data;
	mailbox_win			 = ((T_MFW_HDR *)data->mailbox_win)->data;
	mailbox_win->user		 = (void *)data;
	data->parent_win	 = parent;
	data->win = mailbox_win;
	return data->mailbox_win;

}

void mailbox_destroy(MfwHnd own_window)
{
  T_MFW_WIN * win_data;
  T_MMI_CPHS_DATA   * data = NULL; 

  if (own_window)
	{	

	win_data = ((T_MFW_HDR *)own_window)->data;
		if (win_data != NULL) //PATCH TB
			data = (T_MMI_CPHS_DATA *)win_data->user;

	  if (data)
		{
		TRACE_EVENT ("mailbox_destroy()");
			
			win_delete (data->win);
		  // Free Memory
			FREE_MEMORY ((void *)data, sizeof (T_MMI_CPHS_DATA));
	
		}
	  else
		{
			TRACE_EVENT ("mailbox_destroy() called twice");
		}
	}
}

void cphs_dialog_cb(T_MFW_HND win, USHORT e,  SHORT identifier, void *parameter)
{	TRACE_EVENT("cphs_dialog_cb()");
	switch(e)
	{	case (MAILBOX): 
			cphs_get_mailbox();break;
		default: break;
	}
}

static int cphs_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
    T_MMI_CPHS_DATA	* data = (T_MMI_CPHS_DATA *)win->user;
	
    TRACE_FUNCTION ("cphs_win_cb()");

    if(data EQ 0)
	return 1;
    switch (event)
    {
	case E_WIN_VISIBLE:
		if (win->flags & E_WIN_VISIBLE)
		{
		    /* Print the information dialog */
		    /* Clear Screen */
		    dspl_ClearAll();
		}
	   default:
		return 0;
	}
}
/*******************************************************************************

 $Function:	show_cphs_mb_numbers
 $Description:	Show the list of mailbox numbers
 $Returns:		nothing
 $Arguments:	none
 
*******************************************************************************/
void show_cphs_mb_numbers()
{
	T_MFW_HND win;
    T_MFW_HND parent_win= mfw_parent(mfw_header());

	TRACE_FUNCTION("show_cphs_mb_numbers");

	{
		win = buildInfoNumberMenu(parent_win);

		if (win NEQ NULL)
		{
			SEND_EVENT (win, E_INIT, 0, 0);
		}
	}
}
static UBYTE* temp_ed;

/*******************************************************************************

 $Function:	show_cphs_mb_number
 $Description:	Show the passed mailbox number in editor
 $Returns:		status int
 $Arguments:	mailbox number entry
 
*******************************************************************************/

int show_cphs_mb_number(T_MFW_CPHS_ENTRY* mb_number)
{	T_MFW_HND parent_win = mfw_parent(mfw_header());
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA	editor_data;	/* SPR#1428 - SH - New Editor data*/
#else
	T_EDITOR_DATA		editor_data;
#endif
	char temp_number[MFW_CPHS_VM_NUMBER_MAXLEN - 10];

TRACE_FUNCTION("show_cphs_mb_number");

	memset( temp_number, 0, (MFW_CPHS_VM_NUMBER_MAXLEN - 10) );

	//if it's an international number without a '+' 
	if (mb_number->ton == MFW_TON_INTERNATIONAL && mb_number->number[0] != '+')
	{	strcat(temp_number, "+");	//put a + in front of number
		strncat(temp_number, (char*)mb_number->number, (PHB_MAX_LEN - 1) );
		strcpy((char*)mb_number->number, temp_number);
	}
/* SPR#1428 - SH - New Editor changes */

#ifdef NEW_EDITOR
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)mb_number->number, PHB_MAX_LEN - 1);
	AUI_edit_SetMode(&editor_data, 0, ED_CURSOR_UNDERLINE);
	AUI_edit_SetDisplay(&editor_data, 0, COLOUR_EDITOR, EDITOR_FONT);
	AUI_edit_SetEvents(&editor_data, 0, TRUE, FOREVER, (T_AUI_EDIT_CB)mailbox_edit_cb);
	AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtDelete, TxtNull, NULL);

	temp_ed = (UBYTE *)mb_number->number;

	AUI_edit_Start(parent_win, &editor_data);
#else /* NEW_EDITOR */
	editor_attr_init(&editor_data.editor_attr, 0, edtCurBar1, NULL, (char*)mb_number->number, PHB_MAX_LEN-1, COLOUR_EDITOR);
	editor_data_init(&editor_data, (T_EDIT_CB)mailbox_edit_cb, TxtSoftOK, TxtDelete, NULL, 1, DIGITS_MODE, FOREVER);
	editor_data.Identifier		=  0; 
	editor_data.hide				= FALSE;
	editor_data.destroyEditor		= TRUE; 
	editor_data.TextString		= NULL; /*MC SPR 1257, stops glitch in Chinese*/ 


	editor_start(parent_win,&editor_data);	/* start the editor */ 
#endif /* NEW_EDITOR */
	return 1; 
}

#ifdef FF_CPHS_REL4
/*******************************************************************************

 $Function:	show_cphs_rel4_mb_number
 $Description:	Show the passed cphs rel4 mailbox number in editor
 $Returns:		status int
 $Arguments:	mailbox number entry
 
*******************************************************************************/

int show_cphs_rel4_mb_number(T_MFW_MBDN* mb_number)
{	T_MFW_HND parent_win = mfw_parent(mfw_header());
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA	editor_data;	/* SPR#1428 - SH - New Editor data*/
#else
	T_EDITOR_DATA		editor_data;
#endif
	char temp_number[MAX_MB_NUM_LEN];

TRACE_FUNCTION("show_cphs_mb_number");

	memset(temp_number, 0, MAX_MB_NUM_LEN);

	//if it's an international number without a '+' 
	if (mb_number->type.ton == MFW_TON_INTERNATIONAL && mb_number->number[0] != '+')
	{	strcat(temp_number, "+");	//put a + in front of number
		strcat(temp_number, (char*)mb_number->number);
		strcpy((char*)mb_number->number, temp_number);
	}
/* SPR#1428 - SH - New Editor changes */

#ifdef NEW_EDITOR
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)mb_number->number, MAX_MB_NUM_LEN-1);
	AUI_edit_SetMode(&editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);
	AUI_edit_SetDisplay(&editor_data, 0, COLOUR_EDITOR, EDITOR_FONT);
	AUI_edit_SetEvents(&editor_data, 0, TRUE, FOREVER, NULL);
	switch(mailbox_type)
		{
		case T_MFW_CPHS_REL4_MAILBOX_VOICE:
			AUI_edit_SetTextStr(&editor_data, TxtSoftOK, NULL, TxtVoice, NULL);
			break;
		case T_MFW_CPHS_REL4_MAILBOX_FAX:
			AUI_edit_SetTextStr(&editor_data, TxtSoftOK, NULL, TxtFax, NULL);
			break;
		case T_MFW_CPHS_REL4_MAILBOX_ELECTRONIC:
			AUI_edit_SetTextStr(&editor_data, TxtSoftOK, NULL, TxtElectronic, NULL);
			break;
		case T_MFW_CPHS_REL4_MAILBOX_OTHER:
			AUI_edit_SetTextStr(&editor_data, TxtSoftOK, NULL, TxtOther, NULL);
			break;			
		}
	AUI_edit_Start(parent_win, &editor_data);
#else /* NEW_EDITOR */
	editor_attr_init(&editor_data.editor_attr, 0, edtCurNone, NULL, (char*)mb_number->number, MAX_MB_NUM_LEN-1, COLOUR_EDITOR);
	editor_data_init(&editor_data, NULL, TxtSoftOK, NULL, NULL, 1, DIGITS_MODE, FOREVER);
	editor_data.Identifier		=  0; 
	editor_data.hide				= FALSE;
	editor_data.destroyEditor		= TRUE; 
	editor_data.TextString		= NULL; /*MC SPR 1257, stops glitch in Chinese*/ 


	editor_start(parent_win,&editor_data);	/* start the editor */ 
#endif /* NEW_EDITOR */
// ADDED BY RAVI - 29-11-2005
#if (BOARD == 61)
	return 1; 
#endif
// END RAVI
}
#endif

/*******************************************************************************

 $Function:	mailbox_edit_cb
 $Description:	callback for mailbox editor screen
 $Returns:		nothing
 $Arguments:	window, identifier, reason
 
*******************************************************************************/
static void mailbox_edit_cb( T_MFW_HND win, USHORT Identifier,UBYTE reason)
{
	T_MFW set_mailbox_errcode = MFW_CPHS_OK;	
	UBYTE write_error_number[MFW_CPHS_VM_NUMBER_MAXLEN]; 

	TRACE_FUNCTION("mailbox_edit_cb()");
	switch (reason )
    {
		case INFO_KCD_LEFT:
		{	
			memcpy( (void *)&write_error_number[0], (void *)&cphs_voicemail_num.number[0], MFW_CPHS_VM_NUMBER_MAXLEN );
			memset ( (void *)&cphs_voicemail_num.number[0], 0, MFW_CPHS_VM_NUMBER_MAXLEN );
			memcpy ( (void *)&cphs_voicemail_num.number[0], (void *)temp_ed, MFW_CPHS_VM_NUMBER_MAXLEN );
			
			set_mailbox_errcode = cphs_set_mailbox(&cphs_voicemail_num);

			if ( set_mailbox_errcode == MFW_CPHS_OK )
			{
				mmi_cphs_wait_dialog(win); /* Display the "Pls. wait" Dialog as SIM Write consumes quite some time */
			}
			else
			{				
				memcpy( (void *)&cphs_voicemail_numbers.entries[cphs_voicemail_num.list_index].number[0], 
					    (void *)&write_error_number[0], MFW_CPHS_VM_NUMBER_MAXLEN);
			}
			
			break;
		}
		
			
		default:
		{
		    /* otherwise no action to be performed  */
			break;
		}
	}
}




/*******************************************************************************

 $Function:	cphs_mfw_cb
 $Description:	Callback handler for mfw_cphs events
 $Returns:		status int
 $Arguments:	event, parameter pointer
 
*******************************************************************************/
int cphs_mfw_cb(T_MFW_EVENT event, void* para)
{	T_MFW_HND parent = mfwParent( mfw_header());
	
//   T_MFW_CPHS_VC_NUM* number_list =(T_MFW_CPHS_VC_NUM* )para;  // RAVI

//	T_MFW_CPHS_PARA* parameter = (T_MFW_CPHS_PARA*)para;  // RAVI
//	T_MFW_CPHS_STATUS temp;  // RAVI
	TRACE_FUNCTION("cphs_mfw_cb");
	switch(event)
	{	case E_CPHS_IND: 
			TRACE_EVENT("Getting cphs status");
			if(para != NULL)
			{	cphs_status =(* (T_MFW_CPHS_STATUS*)para);
				//x0pleela 11 June, 2006  DR:OMAPS00079692
				 //changed from CPHS_OK to MFW_CPHS_OK
				if (cphs_status != MFW_CPHS_OK)
				{
					/***************************Go-lite Optimization changes Start***********************/
					//Aug 16, 2004    REF: CRR 24323   Deepa M.D	
					TRACE_EVENT_P1("cphs not supported, apparently%d", cphs_status);
					/***************************Go-lite Optimization changes end***********************/
				}
				cphs_operator_name();
			}
			break;	      
		case E_CPHS_OP_NAME:
                     //x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
			TRACE_EVENT("case: E_CPHS_OP_NAME");
			if(para != NULL)
			{	
			       int csp_status; //x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 - No need to include this under FF_CPHS flag. 
				memcpy(&operator_name, para, sizeof(T_MFW_CPHS_OP_NAME));
				TRACE_EVENT("operator name callback");				
									     
			if (operator_name.longName.data[0] == NULL)
				TRACE_EVENT("Blank long name");

			if (operator_name.shortName.data[0] == NULL)
				TRACE_EVENT("Blank short name");
                      //x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
			csp_status = cphs_get_csp();
                      //x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
                      // In case if cphs_get_csp returns less than 'MFW_CPHS_OK' then either CPHS is not present or CPS is not allocated or activated in serice table.
                     #ifdef FF_CPHS
					 	//x0pleela 11 June, 2006  DR:OMAPS00079692
	 			//changed from CPHS_OK to MFW_CPHS_OK
			if( csp_status < MFW_CPHS_OK)
			{
 		           TRACE_EVENT("E_CPHS_OP_NAME : AOC is set to TRUE");
                         setAOCflag(TRUE);  
			}
			#endif
			}
 		       break;   
		case E_CPHS_GET_VC_NUM:
		{	
			if (para!= NULL)
			{	
			memcpy(&cphs_voicemail_numbers, para, sizeof(T_MFW_CPHS_VC_NUM));
			/***************************Go-lite Optimization changes Start***********************/
			//Aug 16, 2004    REF: CRR 24323   Deepa M.D
			TRACE_EVENT_P1("Voicemail No %s", cphs_voicemail_numbers.entries[0].number);
			/***************************Go-lite Optimization changes end***********************/
				show_cphs_mb_numbers();
			}
			
		}
		break;
		#ifdef FF_CPHS_REL4
		case E_CPHS_REL4_GET_MBI:
			if(para!=NULL)
				{
				/*update local cache*/
					memcpy(&cphs_rel4_mbi,para,sizeof(T_MFW_CPHS_REL4_MBI));
				/*get mbdn*/
				switch(mailbox_type)
					{
					case T_MFW_CPHS_REL4_MAILBOX_VOICE:
						if(cphs_rel4_mbdn.valid & 0x01) 
							cphs_mfw_cb(E_CPHS_REL4_GET_MBDN,&(cphs_rel4_mbdn.mailbox_numbers[0]));
						else
							cphs_rel4_get_mailbox_no( cphs_rel4_mbi.voice);
						break;
					case T_MFW_CPHS_REL4_MAILBOX_FAX:
						if(cphs_rel4_mbdn.valid & 0x02)
							cphs_mfw_cb(E_CPHS_REL4_GET_MBDN,&(cphs_rel4_mbdn.mailbox_numbers[1]));
						else
							cphs_rel4_get_mailbox_no(cphs_rel4_mbi.fax);
						break;
					case T_MFW_CPHS_REL4_MAILBOX_ELECTRONIC:
						if(cphs_rel4_mbdn.valid & 0x04)
							cphs_mfw_cb(E_CPHS_REL4_GET_MBDN,&(cphs_rel4_mbdn.mailbox_numbers[2]));
						else
							cphs_rel4_get_mailbox_no(cphs_rel4_mbi.electronic);
						break;
					case T_MFW_CPHS_REL4_MAILBOX_OTHER:
						if(cphs_rel4_mbdn.valid & 0x08)
							cphs_mfw_cb(E_CPHS_REL4_GET_MBDN,&(cphs_rel4_mbdn.mailbox_numbers[3]));
						else
							cphs_rel4_get_mailbox_no( cphs_rel4_mbi.other);
						break;
					}				
				}
			break;
		case E_CPHS_REL4_GET_MBDN:
			/*destroy the dialog showing waiting*/
			SEND_EVENT(mailbox_waiting_dialog, DIALOG_DESTROY, 0, 0);			
			if(para!=NULL)
				{

				/*update local cache and show mailbox number*/
				switch(mailbox_type)
					{
					case T_MFW_CPHS_REL4_MAILBOX_VOICE:
							memcpy(&cphs_rel4_mbdn.mailbox_numbers[0],para,sizeof(T_MFW_MBDN));
							cphs_rel4_mbdn.valid|=0x01;
							show_cphs_rel4_mb_number(&cphs_rel4_mbdn.mailbox_numbers[0]);
						break;
					case T_MFW_CPHS_REL4_MAILBOX_FAX:
							memcpy(&cphs_rel4_mbdn.mailbox_numbers[1],para,sizeof(T_MFW_MBDN));
							cphs_rel4_mbdn.valid|=0x02;
							show_cphs_rel4_mb_number(&cphs_rel4_mbdn.mailbox_numbers[1]);
						break;
					case T_MFW_CPHS_REL4_MAILBOX_ELECTRONIC:
							memcpy(&cphs_rel4_mbdn.mailbox_numbers[2],para,sizeof(T_MFW_MBDN));
							cphs_rel4_mbdn.valid|=0x04;
							show_cphs_rel4_mb_number(&cphs_rel4_mbdn.mailbox_numbers[2]);
						break;
					case T_MFW_CPHS_REL4_MAILBOX_OTHER:
 						       memcpy(&cphs_rel4_mbdn.mailbox_numbers[3],para,sizeof(T_MFW_MBDN));
							cphs_rel4_mbdn.valid|=0x08;
							show_cphs_rel4_mb_number(&cphs_rel4_mbdn.mailbox_numbers[3]);
						break;
					}
												
				}
			break;
		#endif
		 case E_CPHS_SET_VC_NUM:   
		{		//result of writing mailbox number
			UBYTE write_result;
			if(para != NULL)
			{	write_result = *(UBYTE*)para;
				mmi_cphs_wait_dialog_destroy(); /* Delete the "Pls. wait" screen */  
				if (write_result == MFW_SIMOP_WRITE_OK)
					ShowMessage(parent, TxtSaved, TxtNull);
				else
					ShowMessage(parent, TxtWriteError, TxtNull);//display "Write Error"
			}
		 }
		 break;
			 case E_CPHS_GET_VC_STAT:  
			 {	
				MfwHnd	idle_win =idle_get_window();
				// UBYTE line;  // RAVI

			 	/*a0393213 cphs rel4 - the parameter sent are of different type for CPHS Rel4 and non-CPHS Rel4*/			 	  
				#ifdef FF_CPHS_REL4
			 	if(!isMWISFilePresent() )
			 	{
			 	#endif
				if(para != NULL)
				{	cphs_voicemail_status = (T_MFW_CPHS_VC_STAT*)para;

					TRACE_EVENT("Getting voicemail status");
					if (cphs_voicemail_status->line1 == MFW_CFLAG_SET)
					{
					//x0pleela 01 June, 2006  DR: OMAPS00079692
					//Commented below line to avoid displaying a popup msg showing the line # 
					//Idle screen will be updated with the voice mail icon
    					       //ShowMessage(idle_win, TxtVoicemail, TxtLine1);

    					       /* SPR#1929 - DS - Set voicemail icon on idle screen. Port of 1960 */
    					       iconsSetState(iconIdVoice);
    				      }

				
					if (cphs_voicemail_status->line2 == MFW_CFLAG_SET)
					{	
					//x0pleela 01 June, 2006  DR: OMAPS00079692
					//Commented below line to avoid displaying a popup msg showing the line # 
					//Idle screen will be updated with the voice mail icon
						//ShowMessage(idle_win, TxtVoicemail, TxtLine2);

						/* SPR#1929 - DS - Set voicemail icon on idle screen. Port of 1960 */
    					      iconsSetState(iconIdVoice);
					}

					 /* SPR#1929 - DS - Remove voicemail icon from idle screen if 
                                  * there is no voicemail waiting on either line1/line2. Port of 1960.
                                  */
					if ( (cphs_voicemail_status->line1 != MFW_CFLAG_SET) && 
					     (cphs_voicemail_status->line2 != MFW_CFLAG_SET) )
    				      {
    				            iconsDeleteState(iconIdVoice);
    				      }
				}
				#ifdef FF_CPHS_REL4
				}
				else
				{
					if(is_message_waiting())
					{						 
						iconsSetState(iconIdVoice);						
					}
					else
					{
						iconsDeleteState(iconIdVoice);
					}

				}
				#endif

			 }
			 break;
			case E_CPHS_SET_VC_STAT:  
		 
			{		//result of resetting mailbox status
				// UBYTE write_result;  // RAVI

				TRACE_EVENT("Mailbox status set");
				//Update our copy of the mailbox status
				GetCphsVoicemailStatus(0);
			}
			break;
			 case E_CPHS_GET_DV_STAT:
			 {	
			 	/*a0393213 cphs rel4 - the parameter sent are of different type for CPHS Rel4 and non-CPHS Rel4*/			 	  
				#ifdef FF_CPHS_REL4
			 	if(!isCFISFilePresent() )
			 	{
			 	#endif
					UBYTE line;
					//check for divert on active line
					line = GetAlsLine();
					
					TRACE_EVENT_P4("cphs_mfw_cb():E_CPHS_GET_DV_STAT %d %x %x %x", ((T_MFW_CPHS_DV_STAT*)para)->result,
						                   ((T_MFW_CPHS_DV_STAT*)para)->line1,((T_MFW_CPHS_DV_STAT*)para)->line2,line); 

					/* 30th May,2006     OMAPS00079650     a0393213 (R.Prabakar)
					    Description : CPHS Call Forwarding feature implementation
					    Solution     : call_forwarding_status is updated. The data is not written into flash as it would be done
					    in E_CPHS_SET_DV_STAT. However the present code is not disturbed*/
	    
					#ifdef FF_CPHS
						if(((T_MFW_CPHS_DV_STAT*)para)->result == MFW_SIMOP_READ_ERR)
						{

						if(FFS_flashData.CFState==TRUE)
							{
							if (line == MFW_SERV_LINE2) 
							   ((T_MFW_CPHS_DV_STAT*)para)->line2=MFW_CFLAG_SET;
							else
								((T_MFW_CPHS_DV_STAT*)para)->line1=MFW_CFLAG_SET;
								
							}
						else
							{
							if (line == MFW_SERV_LINE2)
							   ((T_MFW_CPHS_DV_STAT*)para)->line2=MFW_CFLAG_NOTSet;
							else
								((T_MFW_CPHS_DV_STAT*)para)->line1=MFW_CFLAG_NOTSet;
							}
						}
					#endif
					if(para != NULL)
						memcpy(&call_forwarding_status, para, sizeof(T_MFW_CPHS_DV_STAT));
					{

						//check if current outgoing line diverted
					/*a0393213 - the call forward icon is now shown taking both the lines into consideration*/
					/*if (line == MFW_SERV_LINE2)
							DivertStatus = GetCphsDivertStatus(MFW_SERV_LINE2);
						else//if (line == MFW_SERV_LINE1)
						DivertStatus = GetCphsDivertStatus(MFW_SERV_LINE1);*/

						//set the call forwarding icon 
					if ((GetCphsDivertStatus(MFW_SERV_LINE1)==MFW_CFLAG_SET) || 
					    (GetCphsDivertStatus(MFW_SERV_LINE2)==MFW_CFLAG_SET))
						{
							iconsSetState(iconIdCallFor);
							#ifndef FF_CPHS
								//  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
								//              Setting the Current state of CF in Flash						
				                         FFS_flashData.CFState=TRUE;
							#endif
						}
						//gid rid of call forwarding icon 
					if ((GetCphsDivertStatus(MFW_SERV_LINE1)==MFW_CFLAG_NOTSet) && 
					    (GetCphsDivertStatus(MFW_SERV_LINE2)==MFW_CFLAG_NOTSet))
						{
							iconsDeleteState(iconIdCallFor); 
							#ifndef FF_CPHS
								//  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
								//              Setting the Current state of CF in Flash
				                        FFS_flashData.CFState=FALSE;
							#endif
						}
						
				            #ifndef FF_CPHS
							//  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
							//              Writng the Current state of CF into ffs	
							flash_write();
					      #endif
					}
					#ifdef FF_CPHS_REL4
					 	}
						else
						{
							/*store info in call_forwarding_status_rel4*/
							memcpy(&call_forwarding_status_rel4, para, sizeof(T_MFW_CPHS_REL4_DV_STAT));
							if(((T_MFW_CPHS_REL4_DV_STAT*)para)->voice==MFW_CFLAG_SET)
								iconsSetState(iconIdCallFor);
							else
								iconsDeleteState(iconIdCallFor);
						}
						if(isMWISFilePresent())
							cphs_rel4_get_mailbox_status();
					#endif
			 }
			 break; 
			 case E_CPHS_SET_DV_STAT:    
			 {		
			 	//result of setting divert status
				// UBYTE write_result;  // RAVI
				/* 30th May,2006     OMAPS00079650     a0393213 (R.Prabakar)
				    Description : CPHS Call Forwarding feature implementation
				    Solution     : CF status is written into flash if sim write had failed*/
				#ifdef FF_CPHS_REL4
				if(!isCFISFilePresent() )
					{
				#endif
					#ifdef FF_CPHS
	                			if(para != NULL)
						{	
					   		UBYTE write_result;
					    		write_result = *(UBYTE*)para;
							if (write_result == MFW_SIMOP_WRITE_OK)
								TRACE_EVENT("cphs_mfw_cb:Call forward flag set in sim");
							if (write_result == MFW_SIMOP_WRITE_ERR)
								{
								TRACE_EVENT("cphs_mfw_cb:Unable to set call forward flag in sim");
								
								if(ss_data.cfu_services&0x01)
									{
									FFS_flashData.CFState=TRUE;
									}
								else
									{
									FFS_flashData.CFState=FALSE;
									}
								flash_write();
								}
						}
					#endif
										
						cphs_get_divert_status(); //refresh divert status
				#ifdef FF_CPHS_REL4
					}
				else/*a0393213 cphs rel4 supported*/
					{
					if(call_forwarding_status_rel4.voice==MFW_CFLAG_SET)
						iconsSetState(iconIdCallFor);
					else
						iconsDeleteState(iconIdCallFor);						
					}
				#endif

			 }
			 break;
			 case E_CPHS_GET_ALS_STATUS:  
				{	if(para != NULL)
						memcpy(&als_status, para, sizeof(T_MFW_CPHS_ALS_STATUS));
				}
				break;
			 case E_CPHS_SET_LINE: 
			 {		//result of set ALS line
				UBYTE write_result;
				if(para != NULL)
				{	write_result = *(UBYTE*)para;
					if (write_result == MFW_SIMOP_WRITE_OK)
						{
						TRACE_EVENT("cphs_mfw_cb() : ALS status written into flash");
						ShowMessage(parent, TxtChanged, TxtNull);
						}
					if (write_result == MFW_SIMOP_WRITE_ERR)
						{
						TRACE_EVENT("cphs_mfw_cb() : ALS status not written into flash");
						ShowMessage(parent, TxtWriteError, TxtNull);//display "Write Error"
						}
				}
#ifdef FF_CPHS
				mmi_cphs_get_als_info();
#else
				cphs_get_als_info(&als_status);//refresh current line
#endif				
				/*cphs_get_divert_status(); //refresh divert status*//*a0393213 - the call forward icon is now shown taking both the lines into consideration*/
			 }
			 break;
			 case E_CPHS_SET_ALS_STATUS: 
			 {		//result of setting ALS status
				UBYTE write_result;
				if(para != NULL)
				{	write_result = *(UBYTE*)para;
					if (write_result == MFW_SIMOP_WRITE_OK)
						ShowMessage(parent, TxtSaved, TxtNull);
					if (write_result == MFW_SIMOP_WRITE_ERR)
						ShowMessage(parent, TxtWriteError, TxtNull);//display "Write Error"
				}
			 }
			 break;
			 case E_CPHS_GET_CSP:
			 { 
                            #ifdef FF_CPHS
                            //x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
                            int i;
                            #endif					  
			      TRACE_EVENT("Reached E_CPHS_GET_CSP");
			 	if(para != NULL)
				{	
					memcpy(&customer_service_profile, para, sizeof(T_MFW_CPHS_CSP));

					 /***************************Go-lite Optimization changes Start***********************/
					 //Aug 16, 2004    REF: CRR 24323   Deepa M.D
					 TRACE_EVENT_P7("CSP: %x,%x,%x,%x,%x,%x,%x", customer_service_profile.csp[0], customer_service_profile.csp[1], 
						 customer_service_profile.csp[2], customer_service_profile.csp[3], customer_service_profile.csp[4], customer_service_profile.csp[5],
						 customer_service_profile.csp[6]);
					 TRACE_EVENT_P3("%x,%x,%x", customer_service_profile.csp[7], customer_service_profile.csp[8], customer_service_profile.csp[9]);
					 TRACE_EVENT_P7("CSP: %x,%x,%x,%x,%x,%x,%x", customer_service_profile.csp[10], customer_service_profile.csp[11], 
						 customer_service_profile.csp[12], customer_service_profile.csp[13], customer_service_profile.csp[14], customer_service_profile.csp[15],
						 customer_service_profile.csp[16]);
					 TRACE_EVENT_P3("%x,%x,%x", customer_service_profile.csp[17], customer_service_profile.csp[18], customer_service_profile.csp[19]);
					 /***************************Go-lite Optimization changes end***********************/

                                   //x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
			              #ifdef FF_CPHS
			              TRACE_EVENT_P2("AOC : Service group code =%d  value =%d", 
			                  customer_service_profile.csp[4], customer_service_profile.csp[5]);
                                    // No need to check if Aoc_flag is already set to TRUE.
                        
				 	 /*check for Byte 1 service group code =03 and Byte2 bit 6 =1*/
					 for (i=0;i<20; i++)
					 {
					 //check for AOC bit
					 	if( customer_service_profile.csp[i] EQ 0x03)
				 		{
							if(i == 19) break;  /* x0039928 - Lint warning fix */
							i++;
				 			if( customer_service_profile.csp[i] & 0x20)
				 			{
             					              TRACE_EVENT("E_CPHS_GET_CSP: AOC flag is SET in CPHS ");
                                                        setAOCflag(TRUE); 
				 				break;
				 			}
				 			else {
                                                     TRACE_EVENT("E_CPHS_GET_CSP: AOC flag is NOT SET in CPHS ");
                                                     setAOCflag(FALSE);  													 
                                                 }
				 		}
						//check for ALS bit
						else if( customer_service_profile.csp[i] == 0x06)
					  	{
   					  		if(i == 19) break;  /* x0039928 - Lint warning fix */
						    i++;
						    if( customer_service_profile.csp[i] & 0x80 )
						    {
							mmi_cphs_set_als_value( TRUE);
						      break;
						    }
						    else
						      mmi_cphs_set_als_value( FALSE);
				  		}
						else
							i++;	
				 	 }
					
				       #endif
				       cphs_get_info_num(1, info_num_index);
				}

			 }break;  			 
			 case E_CPHS_GET_INFO_NUM:
			 {	
				if(para != NULL)
				{	
					memcpy(&current_info_num, para, sizeof( T_MFW_CPHS_INFONUM_ENTRY));

					TRACE_EVENT("Getting info number");
					if (current_info_num.result !=MFW_SIMOP_READ_ERR)
						{	T_DISPLAY_DATA Dialog;
							TRACE_EVENT("Displaying num");
							/* Initialise the dialog control block with default information
							*/
							dlg_initDisplayData_TextStr( &Dialog, TxtSoftCall,  TxtSoftBack, (char*)current_info_num.alpha.data, (char*)current_info_num.number, COLOUR_STATUS);
							dlg_initDisplayData_events( &Dialog, (T_VOID_FUNC) info_num_display_cb, FOREVER, KEY_LEFT| KEY_RIGHT|KEY_CLEAR );

							/* Show the dialog
							*/
							info_dialog( parent, &Dialog );
						}
					}
				 }
			 
			 break; 
			 case E_CPHS_GET_INFO_LIST:
				{ 
					if (para == NULL)
					{	
						TRACE_EVENT("Invalid info num list");
					}
					else
					{	//populate next pointer 
					       //RAVI - 29-11-2005 
					      /* x0045876, 14-Aug-2006 (WR - assignment in condition)*/
					       info_numbers[info_num_level] = (T_MFW_CPHS_INFONUM_LIST*) ALLOC_MEMORY(sizeof(T_MFW_CPHS_INFONUM_LIST));
						if (info_numbers[info_num_level])
							memcpy(info_numbers[info_num_level], para, sizeof(T_MFW_CPHS_INFONUM_LIST));

						
							TRACE_EVENT("Getting info numbers!!");
							if (info_numbers[info_num_level]->count != 0)
							{
								/***************************Go-lite Optimization changes Start***********************/
								//Aug 16, 2004    REF: CRR 24323   Deepa M.D
								TRACE_EVENT_P1("Valid info num list, count:%d",info_numbers[info_num_level]->count);
								/***************************Go-lite Optimization changes end***********************/
								if (info_num_level)
								{	info_num_level++;
									InfoNumbers2();
									
								}
							}
							else
								info_numbers[info_num_level]=NULL;
					}
				}
			 break;
			 case E_CPHS_GET_SIM_FIELD:
				{	T_MFW_CPHS_READ_SIM_ENTRY temp;

					memcpy(&temp, para, sizeof(T_MFW_CPHS_READ_SIM_ENTRY));
					if (temp.data_len >0)
						//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
						//begin 30439
						//temp.exchange_data[temp.data_len-1] = 0;
						//end 30439
					/***************************Go-lite Optimization changes Start***********************/
					//Aug 16, 2004    REF: CRR 24323   Deepa M.D
					TRACE_EVENT_P3("Sim Read, Field: %d (Record: %d) Errcode: %d",temp.requested_field, temp.record, temp.sim_result);
					/***************************Go-lite Optimization changes end***********************/
					/* SPR759 - SH - Read Sim for homezone, calls appropriate functions */
					#ifdef MMI_HOMEZONE_ENABLED	
						if (temp.requested_field>=HZ_SIM_PARAMETERS
							&& temp.requested_field < (HZ_SIM_CELL_CACHE+HZ_ZONES_MAX) )
						{
							homezoneReadSimCb(temp.requested_field, temp.record, temp.sim_result);
						}
					#endif
				}
			 break;

			  case E_CPHS_SET_SIM_FIELD:
				{
				T_MFW_CPHS_READ_SIM_ENTRY temp;

					memcpy(&temp, para, sizeof(T_MFW_CPHS_READ_SIM_ENTRY));
					if (temp.data_len >0)
						temp.exchange_data[temp.data_len-1] = 0;
					/***************************Go-lite Optimization changes Start***********************/
					//Aug 16, 2004    REF: CRR 24323   Deepa M.D
					TRACE_EVENT_P2("Sim Write, Requested_field: %d Errcode: %d",temp.requested_field, temp.sim_result);
					/***************************Go-lite Optimization changes end***********************/
					/* SPR759 - SH - Read Sim for homezone, calls appropriate functions */
					#ifdef MMI_HOMEZONE_ENABLED
						if (temp.requested_field>=HZ_SIM_PARAMETERS
							&& temp.requested_field < (HZ_SIM_CELL_CACHE+HZ_ZONES_MAX) )
						{
							homezoneWriteSimCb(temp.requested_field, temp.record, temp.sim_result);
						}
					#endif
				}
				break;
#ifdef FF_CPHS_REL4				
			    case E_CPHS_MSP_IND:
				{
					if(para != NULL)
					{
						msp_info = mfw_cphs_get_msp();
						TRACE_EVENT_P1("E_CPHS_MSP_IND - mspcount %d",msp_info->count);
						if(msp_info->count < 2)
						{
						MSP_flag = FALSE;
						#ifdef FF_CPHS
							mmi_cphs_get_als_info();
						#else
							cphs_get_als_info(&als_status);
						#endif
						return TRUE;
						}
						else
						{
							MSP_flag = TRUE;
							mfw_cphs_set_default_profile(1);
							mfw_cphs_set_reg_profile(1);
							mmi_msp_interrogate();
						}
					}
			    	}
				break;
#endif				
			 default: break;
				}
	

	return TRUE;
}








/*******************************************************************************

 $Function:	int InfoNumbers2( )
 $Description:	Displays info number sub-menus
 $Returns:		status int
 $Arguments:	nothing
 
*******************************************************************************/
//for sub-menus
int InfoNumbers2( )
{
	T_MFW_HND win;
    T_MFW_HND parent_win = mfw_parent(mfw_header());
    
	TRACE_FUNCTION("InfoNumbers2");
	
	if (info_numbers != NULL)
	{	type_of_list = INFO_NUM_LIST;
		win = buildInfoNumberMenu(parent_win);

		if (win NEQ NULL)
		{
			SEND_EVENT (win, E_INIT, 0, 0);
		}
	}
	
	return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:	buildInfoNumberMenu
 $Description:	Creates window for Information numbers
 $Returns:		window created
 $Arguments:	parent window
 
*******************************************************************************/
static T_MFW_HND  buildInfoNumberMenu( MfwHnd parent_window)
{
	T_INFO_NUM_INFO *  data = (T_INFO_NUM_INFO *)ALLOC_MEMORY (sizeof (T_INFO_NUM_INFO));
	T_MFW_WIN  * win;
	TRACE_FUNCTION("buildInfoNumberMenu");

    /*
     * Create window handler
     */

    data->numbers_win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)info_num_win_cb);
    if (data->numbers_win EQ NULL)
	{
	   return NULL;
	}

	TRACE_EVENT(">>>> Info num window created: " );
    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)info_num_exec_cb;
    data->mmi_control.data   = data;
    win 		     = ((T_MFW_HDR *)data->numbers_win)->data;
    win->user		     = (void *)data;
    data->parent	     = parent_window;
	winShow(data->numbers_win);
    /*
     * return window handle
     */

     return data->numbers_win;
}


//x0012849: Jagannatha  01 June, 2006  ER: OMAPS00079607 -To Support AcC and Info numbers
/*******************************************************************************
 $Function:	info_num_msg_cb
 $Description:	Dialogue callback function of the information Numbers message window 
 $Returns:		none
 $Arguments:	win - window handler, dialogue id, key pressed				
 
*******************************************************************************/
#ifdef FF_CPHS
void info_num_msg_cb(T_MFW_HND win, USHORT identifier, UBYTE reason)
{
    
    T_MFW_HND parent = mfwParent( mfw_header());    
    TRACE_FUNCTION ("info_num_msg_cb");
    switch (reason) 
    {
        case INFO_KCD_LEFT: 
            // This code is not executed for the case - If mobile is in roaming and service is Network specific.
            if (info_numbers[info_num_level-1]->entry[identifier].entryStat & 0x80) //if bit 8 set/*a0393213 warnings removal - Dialog.Identifier changed to identifier*/
            {	
                TRACE_EVENT("CPHS INFO:Should be number entry");
                if (info_num_level < 4)
            	      cphs_select_info_num(info_num_index);
            }
            else
            {	
                 if (info_num_level < 4)
                 {	
                     TRACE_EVENT("CPHS INFO:Should be dir entry");
                     cphs_get_info_num(info_num_level+1, info_num_index+1);
                 } 
            }
            break;

        default: 
            break;
    }
}
#endif
/*******************************************************************************
 $Function:	info_num_display_cb

 $Description:	Dialogue callback function of the information Numbers display window
 
 $Returns:		none

 $Arguments:	win - window handler, dialogue id, key pressed
				
 
*******************************************************************************/

void info_num_display_cb(T_MFW_HND win, USHORT identifier, UBYTE reason)
{

   // char buf [40];  // RAVI
    
	TRACE_FUNCTION ("info_num_display_cb");

      switch (reason) 
      {
	case INFO_KCD_LEFT: callNumber((UBYTE*)current_info_num.number);
	default: break;  // RAVI - Deafault corrected to default.
	
      }
}
/*******************************************************************************

 $Function:	info_num_exec_cb

 $Description:	Exec callback function of the information Numbers window
 
 $Returns:		none

 $Arguments:	win - window handler
				event - mfw event
				parameter - optional data.
 
*******************************************************************************/
void info_num_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN	   * win_data = ((T_MFW_HDR *) win)->data;
    T_INFO_NUM_INFO   * data = (T_INFO_NUM_INFO *)win_data->user;
	UBYTE i;
	UBYTE numberOfEntries;
//	ListWinInfo	 * mnu_data = (ListWinInfo *)parameter;   // RAVI


	MmiRsrcGetText(TxtLine1);
	MmiRsrcGetText(TxtLine2);
	MmiRsrcGetText(TxtFax);
	MmiRsrcGetText(TxtData);
    TRACE_FUNCTION (">>>>> info_num_exec_cb()");
	
    switch (event)
	{
	case E_INIT:

		TRACE_EVENT(">>>> info_num_exec_cb() Event:E_INIT");

		 /* initialization of administrative data */


		data->menu_list_data = (ListMenuData *)ALLOC_MEMORY(sizeof(ListMenuData));

		if(data->menu_list_data == 0)
		{
			TRACE_EVENT("Failed memory alloc 1 ");
			return;
		}
		if (type_of_list == MAILBOX_LIST || type_of_list == MAILBOX_LIST_CALL)
			{	numberOfEntries = cphs_voicemail_numbers.count;
				if (numberOfEntries > 4)
					numberOfEntries = 4;
			}
		else
			numberOfEntries = info_numbers[info_num_level-1]->count;
		
		data->menu_list_data->List = (T_MFW_MNU_ITEM *)ALLOC_MEMORY( 4 * sizeof(T_MFW_MNU_ITEM) );

		if(data->menu_list_data->List == 0)
		{
			TRACE_EVENT("Failed memory alloc 2");
			return;
		}
		
		for (i = 0; i < numberOfEntries ; i++)
		{
			mnuInitDataItem(&data->menu_list_data->List[i]);
			if (type_of_list == MAILBOX_LIST || type_of_list == MAILBOX_LIST_CALL)
				data->menu_list_data->List[i].str  = (char *)cphs_voicemail_numbers.entries[i].alpha.data;
			else
				data->menu_list_data->List[i].str  = (char *)info_numbers[info_num_level-1]->entry[i].alpha.data;
			data->menu_list_data->List[i].flagFunc = item_flag_none; 
		}

		data->menu_list_data->ListLength =numberOfEntries;
		data->menu_list_data->ListPosition = 1;
		data->menu_list_data->CursorPosition = 1;
		data->menu_list_data->SnapshotSize = numberOfEntries;
		data->menu_list_data->Font = 0;
		if (type_of_list == MAILBOX_LIST_CALL)
			data->menu_list_data->LeftSoftKey = TxtSoftCall;
		else
			data->menu_list_data->LeftSoftKey = TxtSoftSelect;
		data->menu_list_data->RightSoftKey = TxtSoftBack;
		data->menu_list_data->KeyEvents = KEY_ALL;
		data->menu_list_data->Reason = 0;
		data->menu_list_data->Strings = TRUE;
		data->menu_list_data->Attr   = &InfoNum_menuAttrib;
		data->menu_list_data->autoDestroy    = FALSE;
		
		
		/* Create the dynamic menu window */
	    listDisplayListMenu(win, data->menu_list_data,(ListCbFunc)InfonumList_cb,0);
		  
		break;

	case LISTS_REASON_VISIBLE:
	case LISTS_REASON_RESUME:
	{
		dspl_ClearAll();	// sbh - clear screen before update
	
		if (type_of_list == MAILBOX_LIST)
		{	
			softKeys_displayId( TxtSoftSelect, TxtSoftBack, 0, COLOUR_LIST_SUBMENU);
		}
		else if (type_of_list == MAILBOX_LIST_CALL)
		{
			softKeys_displayId( TxtSoftCall, TxtSoftBack, 0, COLOUR_LIST_SUBMENU);			
		}
	
		break;
	}

	default:
	    break;
	}
}
/*******************************************************************************

 $Function:	InfonumList_cb

 $Description:	Callback function for the info num list.
 
 $Returns:		none
 
 $Arguments:	Parent - parent window.
				ListData - Menu item list
*******************************************************************************/

void InfonumList_cb(T_MFW_HND * Parent, ListMenuData * ListData)
{
    T_MFW_WIN	    * win_data = ((T_MFW_HDR *)Parent)->data;
    T_INFO_NUM_INFO * data     = (T_INFO_NUM_INFO *)win_data->user;	 
	
//	char debug[40];   // RAVI

TRACE_FUNCTION("InfonumList_cb");
	if ((ListData->Reason EQ LISTS_REASON_BACK) || (ListData->Reason EQ LISTS_REASON_CLEAR))
	{
		  listsDestroy(ListData->win);
   	          InfoNum_destroy(data->numbers_win);
	    info_num_level--;
	}
	else if(ListData->Reason EQ LISTS_REASON_SELECT)
	{
		if (type_of_list == MAILBOX_LIST || type_of_list == MAILBOX_LIST_CALL) //get the correct voicemail num
		{	
			cphs_voicemail_numbers.entries[ListData->ListPosition].list_index = ListData->ListPosition;
				
			if (type_of_list == MAILBOX_LIST)
			{	/*If we're editing*/
				TRACE_FUNCTION_P2("List Posn2: %d, MBN: %s", ListData->ListPosition, cphs_voicemail_numbers.entries[ListData->ListPosition].number );
				show_cphs_mb_number(&cphs_voicemail_numbers.entries[ListData->ListPosition]);
				memcpy(&cphs_voicemail_num, &cphs_voicemail_numbers.entries[ListData->ListPosition], sizeof(T_MFW_CPHS_ENTRY));
			}
			else
			{	
				char temp_number[MFW_CPHS_VM_NUMBER_MAXLEN];
			
				memcpy(&cphs_voicemail_num, &cphs_voicemail_numbers.entries[ListData->ListPosition], sizeof(T_MFW_CPHS_ENTRY));
				memset(temp_number, 0, MFW_CPHS_VM_NUMBER_MAXLEN);
				
				/*If we're calling the number*/ 
				//if it's an international number without a '+' 
				if (cphs_voicemail_num.ton == MFW_TON_INTERNATIONAL && cphs_voicemail_num.number[0] != '+')
				{	
					strcat(temp_number, "+");	//put a + in front of number
					strncat(temp_number, (char*)cphs_voicemail_num.number, strlen((char*)cphs_voicemail_num.number));					
					memset((void*)cphs_voicemail_num.number, 0, MFW_CPHS_VM_NUMBER_MAXLEN);
					strncpy((char*)cphs_voicemail_num.number, temp_number, strlen((char *)temp_number));
				}
				
				callNumber(cphs_voicemail_num.number);

			}

			
		}
		else
		{	
#ifdef FF_CPHS
			T_DISPLAY_DATA Dialog;
                     //x0012849: Jagannatha  01 June, 2006  ER: OMAPS00079607 -To Support AcC and Info numbers
                     UBYTE inRoaming;
                     inRoaming = getRoamingStatus();
                     TRACE_EVENT_P1("Roaming status = %d", inRoaming);
#endif					 
                    //get the number/directory
				info_num_index = info_numbers[info_num_level-1]->entry[ListData->ListPosition].index;
				/***************************Go-lite Optimization changes Start***********************/
				//Aug 16, 2004    REF: CRR 24323   Deepa M.D
				TRACE_EVENT_P3("Index selected: %d, Level, %d, entryStat:%d",info_num_index, info_num_level, 
					info_numbers[info_num_level-1]->entry[ListData->ListPosition].entryStat); 
				/***************************Go-lite Optimization changes end***********************/
                            //x0012849: Jagannatha  01 June, 2006  ER: OMAPS00079607 -To Support AcC and Info numbers
                            //0x20 in Binary  0010 0000  - If bit 6 is set to 1 that indicates that service is specific to network.
#ifdef FF_CPHS
				if( (current_info_num.entryStat & 0x20) && inRoaming )
				{
					dlg_initDisplayData_TextStr( &Dialog, TxtSoftOK,  TxtSoftBack, (char*)MmiRsrcGetText(TxtNotAvailable), NULL, COLOUR_STATUS);
					dlg_initDisplayData_events( &Dialog, (T_VOID_FUNC) info_num_msg_cb, THREE_SECS, KEY_RIGHT|KEY_CLEAR );
					Dialog.Identifier = (USHORT)ListData->ListPosition;
					info_dialog( Parent, &Dialog );
				}
                            //x0012849: Jagannatha  01 June, 2006  ER: OMAPS00079607 -To Support AcC and Info numbers
                            //0x10 in Binary  0001 0000  - If bit 5 is set to 1 that indicates that service is charged at premium rate.
				else if(current_info_num.entryStat & 0x10)
				{
					dlg_initDisplayData_TextStr( &Dialog, TxtSoftOK,  TxtSoftBack, (char*)"Charging will be done at a", (char*)"Premium Rate", COLOUR_STATUS);
					dlg_initDisplayData_events( &Dialog, (T_VOID_FUNC) info_num_msg_cb, FOREVER, KEY_LEFT|KEY_RIGHT|KEY_CLEAR );
					Dialog.Identifier = (USHORT)ListData->ListPosition;
					info_dialog( Parent, &Dialog );
				}
				else
				{
#endif				
					if (info_numbers[info_num_level-1]->entry[ListData->ListPosition].entryStat & 0x80) //if bit 8 set
					{	
						TRACE_EVENT("CPHS INFO:Should be number entry");
						if (info_num_level < 4)
							cphs_select_info_num(info_num_index);
					}
					else
					{	
						if (info_num_level < 4)
						{		
							TRACE_EVENT("CPHS INFO:Should be dir entry");
							cphs_get_info_num(info_num_level+1, info_num_index+1);
						}
					}
#ifdef FF_CPHS
				}
#endif
					
		}	
			   
	}

	
}

/*******************************************************************************

 $Function:	InfoNum_destroy

 $Description:	Destroy the info num window.
			
 $Returns:		none

 $Arguments:	own_window- current window
*******************************************************************************/

static void InfoNum_destroy(MfwHnd own_window)
{
    T_MFW_WIN * win_data;
    T_INFO_NUM_INFO	* data;
	UBYTE numberOfEntries;
	
    TRACE_FUNCTION ("InfoNum_destroy()");
	
    if (own_window)
	{
	    win_data = ((T_MFW_HDR *)own_window)->data;
	    data = (T_INFO_NUM_INFO *)win_data->user;

	    if (data)
		{
		    /*
		     * Delete WIN handler
		     */ 
		    win_delete (data->numbers_win);

		    /*	   
		     * Free Memory
		     */
		     if ( ( type_of_list == MAILBOX_LIST ) || ( type_of_list == MAILBOX_LIST_CALL ) )
		    {	numberOfEntries = cphs_voicemail_numbers.count;
				if (numberOfEntries > 4)
					numberOfEntries = 4;
			}
		     else
				numberOfEntries =info_numbers[info_num_level-1]->count;
			if (data->menu_list_data != NULL)
			{	//int i; // RAVI
				FREE_MEMORY ((void *)data->menu_list_data->List, numberOfEntries * sizeof (T_MFW_MNU_ITEM));
			    FREE_MEMORY ((void *)data->menu_list_data, sizeof (ListMenuData));
			    if (type_of_list == INFO_NUM_LIST && info_num_level > 1)//if info numbers, 
				{	FREE_MEMORY((void*)info_numbers[info_num_level-1], sizeof(T_MFW_CPHS_INFONUM_LIST));
				info_numbers[info_num_level-1] = NULL;
			    }
			}
			FREE_MEMORY ((void *)data, sizeof (T_INFO_NUM_INFO));

		}
	    else
		{
		    TRACE_EVENT ("InfoNum_destroy() called twice");
		}
	}
}

/*******************************************************************************

 $Function:	info_num_win_cb

 $Description:	Window callback function for the info numbers window.
 
 $Returns:		none

 $Arguments:	w - mfw window handler
				e - mfw event
				
*******************************************************************************/

static int info_num_win_cb (MfwEvt e, MfwWin *w)    /* yyy window event handler */
{
	TRACE_EVENT ("info_num_win_cb()");
	
	switch (e)
	{
		case MfwWinVisible:  /* window is visible  */
			break;
			
	case MfwWinFocussed: /* input focus / selected	 */	
	case MfwWinDelete:   /* window will be deleted	 */   
		
	default:	    
		return MFW_EVENT_REJECTED;
	}
	return MFW_EVENT_CONSUMED;
}



/*******************************************************************************

 $Function:	mmi_cphs_get_als_info

 $Description:	Returns the current Line enabled
 
 $Returns:		none

 $Arguments:	
				
*******************************************************************************/
//x0pleela 23 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
void mmi_cphs_get_als_info()
{
	int nALS_line_data;

	nALS_line_data= mfw_cphs_get_als_info();
	TRACE_EVENT_P1("mmi_cphs_get_als_info() : %d",nALS_line_data);
	switch (nALS_line_data)
    	{
	      	case ALS_MOD_SPEECH:
			als_status.selectedLine = MFW_SERV_LINE1;
			break;
			
	      case ALS_MOD_AUX_SPEECH:
			als_status.selectedLine = MFW_SERV_LINE2;
			break;
	      default:
			als_status.selectedLine = MFW_SERV_NotPresent;
			break;
    }
}


/*******************************************************************************

 $Function:	mmi_cphs_set_als_value

 $Description:	Set the Als flag based on the CSP data read from CPHS enabled SIM
 
 $Returns:		none

 $Arguments:	
				
*******************************************************************************/
//x0pleela 29 May, 2006  DR: OMAPS00070657

void mmi_cphs_set_als_value( UBYTE als_val)
{
	TRACE_FUNCTION("mmi_cphs_set_als_value()");
	Als_flag = als_val;
}

/*******************************************************************************

 $Function:	mmi_cphs_get_als_value

 $Description:	Returns the Als flag value
 
 $Returns:		none

 $Arguments:	
				
*******************************************************************************/
//x0pleela 29 May, 2006  DR: OMAPS00070657

UBYTE  mmi_cphs_get_als_value( void )
{
	TRACE_EVENT_P1("mmi_cphs_get_als_value() %d",Als_flag);
	return Als_flag;
}

#ifdef FF_CPHS_REL4
UBYTE  mmi_cphs_get_msp_value( void )
{
	TRACE_EVENT_P1("mmi_cphs_get_msp_value - MSP_flag - %d", MSP_flag);
	return MSP_flag;
}
#endif
/*******************************************************************************

 $Function:	mmi_get_als_selectedLine

 $Description:	Returns the current Line enabled
 
 $Returns:		none

 $Arguments:	
				
*******************************************************************************/
//x0pleela 30 May, 2006  DR: OMAPS00070657
T_MFW_LINE_INDEX mmi_get_als_selectedLine( void)
{
	return als_status.selectedLine;
}
#endif /* FF_CPHS */
