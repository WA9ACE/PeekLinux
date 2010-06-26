/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************
 
 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		    MmiEm.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    23/09/02                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    MMI engineering mode handling
                        
********************************************************************************
 $History: MmiEm.c

    Apr 24, 2006    ERT: OMAPS00067603 x0043642
    Description: Engineering mode
    Solution:  Added new engineering mode functionalities to satisfy Neptune engineering mode
    requirements.


	23/09/02			Original Condat(UK) BMI version.	
	   
 $End




*******************************************************************************
                                                                              
                                Include Files
                                                                              
*******************************************************************************/
#define ENTITY_MFW

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

#include "prim.h"
#include "cus_aci.h"

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_lng.h"
#include "mfw_tim.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_phb.h"
#include "mfw_cm.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_sat.h"
#include "mfw_ss.h" /*for convert*/
#include "mfw_phb.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_cphs.h"
#include "mfw_sat.h"
#include "Mfw_band.h"
#include "mfw_ffs.h"

#include "dspl.h"

#include "MmiMmi.h"
#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiBand.h"
#include "MmiCPHS.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif
#include"MmiBookShared.h"


#include "Mfw_em.h"
#include "MmiEm.h"

#include "mmiColours.h"

#ifdef NEPTUNE_BOARD
#include "ffs.h"
#endif 

#define EM_UPDATE_RATE 1000

#ifdef NEPTUNE_BOARD
#define EM_EDITOR_SIZE 500 /*SPR 1757, increased size of editor buffer*/
#else
#define EM_EDITOR_SIZE 300 /*SPR 1757, increased size of editor buffer*/
#endif


#ifdef NEPTUNE_BOARD /*EngMode */
#define SAT_REQUEST_SENT 1
#define TIMER_ONE_SEC 1
#define TIMER_STOP 0

EXTERN T_ENG_INFO_REQ* Eng_Info_Req;
EXTERN T_ENG_INFO_STATIC_REQ* Eng_Info_Static_Req;

EXTERN unsigned long *picture_col;
EXTERN UBYTE RssiVal;

int EmRequest = NULL; 
#endif

/*
**************************************Datatypes****************************************
*/
typedef struct _tEmData_
{
    /* administrative data */

    T_MMI_CONTROL     mmi_control;
    T_MFW_HND         win;
    T_MFW_HND         parent_win;

    /* associated handlers */

    T_MFW_HND         kbd; /*keyboard handler*/
    T_MFW_HND         tim;	/*timer handler*/

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	T_ED_DATA *			editor;
	T_ED_ATTR			editor_attr;
#else /* NEW_EDITOR */
    T_MFW_HND         edt; /*mfw editor handler*/
    MfwEdtAttr		editor_data;						/*editor config data*/
#endif /* NEW_EDITOR */

    /* internal data */
    char    edtBuffer[EM_EDITOR_SIZE*2];/*SPR 1757*//*editor contentbuffer*/
	MfwEmDataType CurrentWindow;						/*type of data being displayed*/

} tEmData;


/*
**************************************Prototypes****************************************
*/
int Mmi_em_event_cb(T_MFW_EVENT event, T_MFW_HND para); //Handles events from the MFW
T_MFW_HND Mmi_em_create(T_MFW_HND parent_window, MfwEmDataType data_type); //Create a window to display the information, specifying the keyboard, display and timer handlers.
void Mmi_em_destroy(T_MFW_HND own_window); //Deallocate window data and delete window.
int Mmi_em_kbd_cb(T_MFW_EVENT event,T_MFW_KBD *  kc ); //Handles key presses in the display window
int Mmi_em_win_cb(T_MFW_EVENT event,T_MFW_WIN * win );// Handles the information display.
void Mmi_em_tim_cb(T_MFW_EVENT event,T_MFW_TIM * t); //Handles the timer events.  




/*
**************************************Public functions****************************************
*/


/*******************************************************************************

 $Function:    	mmi_em_init()

 $Description:	initialise Engineering Mode module
 
 $Returns:		None

 $Arguments:	None
 
*******************************************************************************/
void mmi_em_init(void)
{
	Mfw_em_init();

}

/*******************************************************************************

 $Function:    	mmi_em_enit()

 $Description:	exit Engineering Mode module
 
 $Returns:		None

 $Arguments:	None
 
*******************************************************************************/
void mmi_em_exit(void)
{
	Mfw_em_exit();
}

//#ifdef TI_PS_FF_EM
#if defined (TI_PS_FF_EM) || defined (MMI_EM_ENABLED)
/*******************************************************************************

 $Function:    	mmi_em_start_eng_mode_menus()

 $Description:	starts the emergency mode menu
 
 $Returns:		None

 $Arguments:	parent window
 
*******************************************************************************/
void mmi_em_start_eng_mode_menus(T_MFW_HND parent)
{	
	TRACE_EVENT("mmi_em_start_eng_mode_menus");
	bookMenuStart(parent, EngModeAttributes(), NULL);

}
#endif

#ifndef NEPTUNE_BOARD

/*******************************************************************************

 $Function:    	Mmi_em_display_mobile_info_data()

 $Description:	Called from menu; begins process to display Mobile Information
 
 $Returns:		status integer

 $Arguments:	menu and item (not used)
 
*******************************************************************************/


int Mmi_em_display_mobile_info_data(MfwMnu* m, MfwMnuItem* i)
{	T_MFW_HND		parent	     = mfwParent( mfw_header());
	TRACE_EVENT_P1("Mmi_em_display_mobile_info(), parent win %d", parent);
	TRACE_EVENT_P1("Memory left:%d", mfwCheckMemoryLeft());
	Mmi_em_create(parent, EM_MOBILE_INFO);
	
	return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:    	Mmi_em_display_serving_cell_params()

 $Description:	Called from menu; begins process to display Serving Cell parameters
 
 $Returns:		status integer

 $Arguments:	menu and item (not used)
 
*******************************************************************************/

int Mmi_em_display_serving_cell_params(MfwMnu* m, MfwMnuItem* i)
{	T_MFW_HND		parent	     = mfwParent( mfw_header());
	TRACE_EVENT_P1("MMiEm, Memory left:%d", mfwCheckMemoryLeft());
	Mmi_em_create(parent, EM_SERVING_CELL_PARAMS);
	
	return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:    	Mmi_em_display_neighbour_cell_params()

 $Description:	Called from menu; begins process to display Neighbouring Cell parameters
 
 $Returns:		status integer

 $Arguments:	menu and item (not used)
 
*******************************************************************************/ 
int Mmi_em_display_neighbour_cell_params(MfwMnu* m, MfwMnuItem* i)
{	T_MFW_HND		parent	     = mfwParent( mfw_header());
	TRACE_EVENT_P1("MMiEm, Memory left:%d", mfwCheckMemoryLeft());
	Mmi_em_create(parent, EM_NEIGHBOURING_CELL_PARAMS);
	
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	Mmi_em_display_location_params()

 $Description:	Called from menu; begins process to display Locationparameters
 
 $Returns:		status integer

 $Arguments:	menu and item (not used)
 
*******************************************************************************/ 
int Mmi_em_display_location_params(MfwMnu* m, MfwMnuItem*i )
{	T_MFW_HND		parent	     = mfwParent( mfw_header());

	Mmi_em_create(parent, EM_LOCATION_PARAMS);
	
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	Mmi_em_display_ciph_hop_DTX_params()

 $Description:	Called from menu; begins process to display Ciphering, hopping and
 discontinuous transmission parameters
 
 $Returns:		status integer

 $Arguments:	menu and item (not used)
 
*******************************************************************************/
int Mmi_em_display_ciph_hop_DTX_params(MfwMnu* m, MfwMnuItem* i)
{	T_MFW_HND		parent	     = mfwParent( mfw_header());
	TRACE_EVENT_P1("MMiEm, Memory left:%d", mfwCheckMemoryLeft());
	Mmi_em_create(parent, EM_CIPH_HOP_DTX_PARAMS);
	
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	Mmi_em_display_GPRS_params()

 $Description:	Called from menu; begins process to display GPRS specific parameters
 
 $Returns:		status integer

 $Arguments:	menu and item (not used)
 
*******************************************************************************/

int Mmi_em_display_GPRS_params(MfwMnu* m, MfwMnuItem* i)
{	T_MFW_HND		parent	     = mfwParent( mfw_header());
	TRACE_EVENT_P1("MMiEm, Memory left:%d", mfwCheckMemoryLeft());
	Mmi_em_create(parent, EM_GPRS_PARAMS);
	
	return MFW_EVENT_CONSUMED;
}

#else    /* if NEPTUNE_BOARD */

int Mmi_em_start(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       parent       = mfwParent( mfw_header());
    TRACE_EVENT_P1("Mmi_em_display_mobile_info(), parent win %d", parent);
    TRACE_EVENT_P1("Memory left:%d", mfwCheckMemoryLeft());

    TRACE_EVENT_P1("Selected item: %d", m->lCursor[m->level]);
    
    Mmi_em_create(parent, m->lCursor[m->level]);

    if(!EmRequest)
    {
        sAT_PercentESINFO(CMD_SRC_LCL,TIMER_ONE_SEC);
        sAT_PercentEINFO(CMD_SRC_LCL,TIMER_ONE_SEC);
    }
    EmRequest = SAT_REQUEST_SENT;
    
    return MFW_EVENT_CONSUMED;
}


#endif  /* ifndef NEPTUNE_BOARD */
/*
************************************Internal Functions**************************************
*/

/*******************************************************************************

 $Function:    	Mmi_em_create()

 $Description:	Creates emergency mode display window
 
 $Returns:		window handle

 $Arguments:	parent window, kind of data to be displayed
 
*******************************************************************************/

T_MFW_HND Mmi_em_create(T_MFW_HND parent_window, MfwEmDataType data_type)
{
    tEmData     * data = (tEmData *)ALLOC_MEMORY (sizeof (tEmData ));
    T_MFW_WIN  * win;

    if (data EQ NULL)
    {
        return NULL;
    }

    // Create window handler
    data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (MfwCb)Mmi_em_win_cb);
    if (data->win EQ NULL)
    {
        return NULL;
    }
    TRACE_EVENT_P1("Mmi_em_create(), data->win %d", data->win);
    //no window event handler needed
    data->mmi_control.dialog = (T_DIALOG_FUNC)NULL;
    data->mmi_control.data   = data;

    data->kbd      = kbd_create (data->win, KEY_ALL,         (T_MFW_CB)Mmi_em_kbd_cb);

#ifndef NEPTUNE_BOARD
    /*Mobile Info is the only data screen which doesn't need to be updated every second*/
    if (data_type != EM_MOBILE_INFO)
    {
        data->tim		= tim_create(data->win, EM_UPDATE_RATE, (T_MFW_CB)Mmi_em_tim_cb);
        timStart(data->tim);//start timer
    }
#endif

    /*populate the user data*/
    win                      = ((T_MFW_HDR *)data->win)->data;
    win->user                = (void *)data;
    data->parent_win         = parent_window;

    /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
    /*SPR 1757, removed code for Chinese special case*/
    AUI_edit_SetAttr(&data->editor_attr, EM_CREATE_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT, ED_MODE_READONLY, ED_CURSOR_NONE, ATB_DCS_ASCII, (UBYTE *)data->edtBuffer,EM_EDITOR_SIZE);
    data->editor = ATB_edit_Create(&data->editor_attr,0);
    /*Clear editor buffer*/
    memset(data->edtBuffer, 0, sizeof(EM_EDITOR_SIZE*2));
    ATB_edit_Init(data->editor);
#else /* NEW_EDITOR */
    /*SPR 1757, removed code for Chinese special case*/
    bookSetEditAttributes(EM_CREATE_EDITOR,COLOUR_EDITOR_XX,0,edtCurNone,0,data->edtBuffer,EM_EDITOR_SIZE*2,&data->editor_data);
    data->edt = edtCreate(data->win, &data->editor_data, NULL, NULL);
    edtUnhide( data->edt );
    /*Clear editor buffer*/
    memset(data->edtBuffer, 0, sizeof(EM_EDITOR_SIZE*2));
#endif /* NEW_EDITOR */

    /*Make sure we store the kind of window we want to show*/
    data->CurrentWindow = data_type;

    /*bind the MFW event handler to any events from the MFW Eng Mode module*/
    Mfw_em_create(data->win, 0xFF,  (MfwCb)Mmi_em_event_cb);
    Mfw_em_get_data(data_type);/*ask MFW for the required data*/
    win_show(data->win);/*show the screen*/
    return data->win;

}

/*******************************************************************************

 $Function:    	Mmi_em_event_cb()

 $Description:	Handles events coming from the MFW 
 
 $Returns:		status integer

 $Arguments:	event type, pointer to data
 
*******************************************************************************/
int Mmi_em_event_cb(T_MFW_EVENT event,void* para)
{ 
    T_MFW_HND       win  = mfw_parent (mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    tEmData * data = (tEmData *)win_data->user;

#ifndef NEPTUNE_BOARD
    MFW_EM_Mobile_Info* mob_info;
    MFW_EM_Serving_Cell_Parameters* sc_info;
    MFW_EM_Neighbour_Cell_Parameters* nc_info;
    MFW_EM_Location_Parameters* loc_info;
    MFW_EM_Ciph_hop_DTX_Parameters* ciph_hop_dtx_info;
    MFW_EM_GPRS_Parameter* gprs_info;
    char * DTX_Status;
#else
    fd_t fd;
    char file[10];
    char timeAndDate[25];
#endif

    char *temp_buffer=NULL;
    
#ifdef NEW_EDITOR
    T_ATB_TEXT temp_text;/*SPR 1757*/
#endif 
    TRACE_EVENT_P1("Mmi_em_event_cb(), event:%d", event);
    temp_buffer=(char*)malloc(EM_EDITOR_SIZE*2*sizeof(char)); /*a0393213 lint warnings removal - dynamically allocated*/
    if(temp_buffer==NULL)
    	{
    	TRACE_ERROR("Mmi_em_event_cb():No memory");
	return MFW_EVENT_CONSUMED;
    	}

    memset(temp_buffer, 0, EM_EDITOR_SIZE*2);

#ifdef NEPTUNE_BOARD
    strcpy(file, "/NOR/Em.txt");
    fd = ffs_open(file, FFS_O_CREATE|FFS_O_WRONLY|FFS_O_APPEND);
    if(fd < 0 )
    {
        TRACE_EVENT("Error in Openning File");
	  free(temp_buffer);
        return FALSE;
    }

    sprintf(timeAndDate, "%s  %s\n", mfw_td_get_clock_str(), mfw_td_get_date_str());
    ffs_write(fd, timeAndDate, strlen(timeAndDate));
                
#endif                


    /*if event matches current window type*/
    if (event == data->CurrentWindow)
    {
        switch (event)
        {
#ifndef NEPTUNE_BOARD         /*EngMode */
        
            case EM_MOBILE_INFO:
            {
                            mob_info = (MFW_EM_Mobile_Info*)para;/*cast parameter to appropriate type*/
                            /*copy data to editor buffer*/
                            /*MC, SPR 1554 Added all SW versions to editor string*/ 
                            sprintf(temp_buffer, "SIM_VERSION:%s CC_VERSION:%s SS_VERSION:%s SMS_VERSION:%s MM_VERSION:%s RR_VERSION:%s DL_VERSION:%s ALR_VERSION:%s  IMEI:%s IMSI:%s TMSI:%u", 
                            mob_info->SIM_version, mob_info->CC_version,  mob_info->SS_version,
                            mob_info->SMS_version, mob_info->MM_version, mob_info->RR_version,
                            mob_info->DL_version, mob_info->ALR_version,mob_info->IMEI, mob_info->IMSI,
                            mob_info->TMSI);
                            /* SPR#1428 - SH - New Editor - string has changed, update word wrap */
#ifdef NEW_EDITOR
				/*SPR 1757 Insert temp buffer into editor*/
				temp_text.data = (UBYTE*)temp_buffer;
				temp_text.dcs = ATB_DCS_ASCII;
				ATB_string_Length(&temp_text);
				ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
				ATB_edit_ClearAll(data->editor);
				ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
				ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
				ATB_edit_Refresh(data->editor);
#else /* !NEW_EDITOR */
				strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
#endif
				win_show(data->win);/*show data*/
			}
			break;
#endif
            
			case EM_SERVING_CELL_PARAMS:
			{
#ifndef NEPTUNE_BOARD                
				sc_info = (MFW_EM_Serving_Cell_Parameters*)para;/*cast parameter to appropriate type*/
				sprintf(temp_buffer, "ARFCN:%d RSSI:%d RXQ:%d RLT:%d C1:%d C2:%d LAC:%d BSIC:%d TAV:%d",\
          sc_info->arfcn, sc_info->RSSI, sc_info->RXQ, sc_info->RLT, sc_info->C1, sc_info->C2,\
          sc_info->LAC, sc_info->BSIC, sc_info->TAV);
#else
            sprintf (temp_buffer, "AFRCN = %u   Cell ID = %u \nMCC<>: = %u    %u    %u\nMNC<>: %u    %u    %u\nLAC<> = %u    %u\nRAC = %u\nMultiframe period = %u \nTMSI<> = %u   %u   %u   %u \nMax retransmission = %u  \nCall re establishment status flag = %u \nMax o/p pw on common control channel = %u \nMinimum signal level for access = %u",
                Eng_Info_Req->info_rr.gsm_info.gsm_info_idle.pccch_info.pccch_channel.gsm_channel_static.arfcn, 
                Eng_Info_Req->info_rr.gsm_info.gsm_info_idle.gsm_cell_id,
                Eng_Info_Static_Req->info_mm.mm_cmd_info.user_selected_plmn.mcc[0],
                Eng_Info_Static_Req->info_mm.mm_cmd_info.user_selected_plmn.mcc[1],
                Eng_Info_Static_Req->info_mm.mm_cmd_info.user_selected_plmn.mcc[2],
                Eng_Info_Static_Req->info_mm.mm_cmd_info.user_selected_plmn.mnc[0],
                Eng_Info_Static_Req->info_mm.mm_cmd_info.user_selected_plmn.mnc[1],
                Eng_Info_Static_Req->info_mm.mm_cmd_info.user_selected_plmn.mnc[2],
                Eng_Info_Static_Req->info_mm.mm_sim_info.last_registered_cs_lac.lac_element[0],
                Eng_Info_Static_Req->info_mm.mm_sim_info.last_registered_cs_lac.lac_element[1],
                Eng_Info_Req->info_mm.mm_reg_ps_info.registered_rac,
                Eng_Info_Req->info_rr.gsm_info.gsm_info_idle.ccch_info.bs_pa_mfrms,
                Eng_Info_Req->info_mm.mm_reg_cs_info.tmsi[0],
                Eng_Info_Req->info_mm.mm_reg_cs_info.tmsi[1],
                Eng_Info_Req->info_mm.mm_reg_cs_info.tmsi[2],
                Eng_Info_Req->info_mm.mm_reg_cs_info.tmsi[3],
                Eng_Info_Req->info_rr.gsm_info.gsm_info_idle.ccch_info.max_retrans,
                Eng_Info_Req->info_rr.gsm_info.gsm_info_idle.call_reestablishment_allowed,
                Eng_Info_Req->info_rr.gsm_bba_cell[0].gsm_cell_info.gsm_cell_info_bcch.ms_txpwr_max_cch,
                Eng_Info_Req->info_rr.gsm_bba_cell[0].gsm_cell_info.gsm_cell_info_bcch.rxlev_access_min
                );

                ffs_write(fd, "Serving cell info\n",20 );
                ffs_write(fd, temp_buffer, strlen(temp_buffer));

#endif
    
	/* SPR#1428 - SH - New Editor - string has changed, update word wrap */
#ifdef NEW_EDITOR
				/*SPR 1757 Insert temp buffer into editor*/
				temp_text.data = (UBYTE*)temp_buffer;
				temp_text.dcs = ATB_DCS_ASCII;
				ATB_string_Length(&temp_text);
				ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
				ATB_edit_ClearAll(data->editor);
				ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
				ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
				ATB_edit_Refresh(data->editor);
#else /* !NEW_EDITOR */
				strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
#endif
				win_show(data->win);
			}
			break;
            
			case EM_NEIGHBOURING_CELL_PARAMS:	
			{
#ifndef NEPTUNE_BOARD                
                    nc_info = (MFW_EM_Neighbour_Cell_Parameters*)para;/*cast parameter to appropriate type*/
				/*copy data to editor buffer*/
				sprintf(temp_buffer, "NUM:%d\nARFCN:RSSI:C2:LAC:BSIC\n %5d%5d%5d%6d%5d\n %5d%5d%5d%6d%5d\n %5d%5d%5d%6d%5d\n %5d%5d%5d%6d%5d\n %5d%5d%5d%6d%5d\n %5d%5d%5d%6d%5d\n SC%4d%5d%5d%6d%5d",\
          nc_info->NUM,\
          nc_info->arfcn[0], nc_info->RSSI[0], nc_info->C2[0], nc_info->LAC_NC[0], nc_info->BSIC_NC[0],\
          nc_info->arfcn[1], nc_info->RSSI[1], nc_info->C2[1], nc_info->LAC_NC[1], nc_info->BSIC_NC[1],\
          nc_info->arfcn[2], nc_info->RSSI[2], nc_info->C2[2], nc_info->LAC_NC[2], nc_info->BSIC_NC[2],\
          nc_info->arfcn[3], nc_info->RSSI[3], nc_info->C2[3], nc_info->LAC_NC[3], nc_info->BSIC_NC[3],\
          nc_info->arfcn[4], nc_info->RSSI[4], nc_info->C2[4], nc_info->LAC_NC[4], nc_info->BSIC_NC[4],\
          nc_info->arfcn[5], nc_info->RSSI[5], nc_info->C2[5], nc_info->LAC_NC[5], nc_info->BSIC_NC[5],\
          nc_info->arfcn[6], nc_info->RSSI[6], nc_info->C2[6], nc_info->LAC_NC[6], nc_info->BSIC_NC[6]);
	/* SPR#1428 - SH - New Editor - string has changed, update word wrap */
				/*show the window*/
				win_show(data->win);

#else /* NEPTUNE_BOARD*/
                sprintf (temp_buffer, "NEIGHBOURING CELL\nNUM : %d\nBCCH <>  %d  \nBSIC <>  %d  %d  %d  %d  %d  %d \nRXLEV <>  %d  %d  %d  %d  %d  %d\nC1 <>  %d  %d  %d  %d  %d  %d\nC2 <>  %d  %d  %d  %d  %d  %d\nC31 <>  %d  %d  %d  %d  %d  %d\nC32 <>  %d  %d  %d  %d  %d  %d\nARFCN <>  %d  %d  %d  %d  %d  %d\nFRAME OFFSET <>  %d  %d  %d  %d  %d  %d\nTIME ALIGN <>  %d  %d  %d  %d  %d  %d",
                        Eng_Info_Req->info_rr.c_gsm_bba_cell,
                        Eng_Info_Req->info_rr.gsm_info.gsm_info_active.gsm_info_dedicated.bcch_channel,
                        

                        Eng_Info_Req->info_rr.gsm_bba_cell[1].gsm_cell_info.gsm_cell_info_pbcch.bsic_match,
                        Eng_Info_Req->info_rr.gsm_bba_cell[2].gsm_cell_info.gsm_cell_info_pbcch.bsic_match,
                        Eng_Info_Req->info_rr.gsm_bba_cell[3].gsm_cell_info.gsm_cell_info_pbcch.bsic_match,
                        Eng_Info_Req->info_rr.gsm_bba_cell[4].gsm_cell_info.gsm_cell_info_pbcch.bsic_match,
                        Eng_Info_Req->info_rr.gsm_bba_cell[5].gsm_cell_info.gsm_cell_info_pbcch.bsic_match,
                        Eng_Info_Req->info_rr.gsm_bba_cell[6].gsm_cell_info.gsm_cell_info_pbcch.bsic_match,
                        Eng_Info_Req->info_rr.gsm_bba_cell[1].rxlev,
                        Eng_Info_Req->info_rr.gsm_bba_cell[2].rxlev,
                        Eng_Info_Req->info_rr.gsm_bba_cell[3].rxlev,
                        Eng_Info_Req->info_rr.gsm_bba_cell[4].rxlev,
                        Eng_Info_Req->info_rr.gsm_bba_cell[5].rxlev,
                        Eng_Info_Req->info_rr.gsm_bba_cell[6].rxlev,
                        Eng_Info_Req->info_rr.gsm_bba_cell[1].gsm_cell_info.gsm_cell_info_bcch.c1,
                        Eng_Info_Req->info_rr.gsm_bba_cell[2].gsm_cell_info.gsm_cell_info_bcch.c1,
                        Eng_Info_Req->info_rr.gsm_bba_cell[3].gsm_cell_info.gsm_cell_info_bcch.c1,
                        Eng_Info_Req->info_rr.gsm_bba_cell[4].gsm_cell_info.gsm_cell_info_bcch.c1,
                        Eng_Info_Req->info_rr.gsm_bba_cell[5].gsm_cell_info.gsm_cell_info_bcch.c1,
                        Eng_Info_Req->info_rr.gsm_bba_cell[6].gsm_cell_info.gsm_cell_info_bcch.c1,
                        Eng_Info_Req->info_rr.gsm_bba_cell[1].gsm_cell_info.gsm_cell_info_bcch.c2,
                        Eng_Info_Req->info_rr.gsm_bba_cell[2].gsm_cell_info.gsm_cell_info_bcch.c2,
                        Eng_Info_Req->info_rr.gsm_bba_cell[3].gsm_cell_info.gsm_cell_info_bcch.c2,
                        Eng_Info_Req->info_rr.gsm_bba_cell[4].gsm_cell_info.gsm_cell_info_bcch.c2,
                        Eng_Info_Req->info_rr.gsm_bba_cell[5].gsm_cell_info.gsm_cell_info_bcch.c2,
                        Eng_Info_Req->info_rr.gsm_bba_cell[6].gsm_cell_info.gsm_cell_info_bcch.c2,
                        Eng_Info_Req->info_rr.gsm_bba_cell[1].gsm_cell_info.gsm_cell_info_pbcch.c31,
                        Eng_Info_Req->info_rr.gsm_bba_cell[2].gsm_cell_info.gsm_cell_info_pbcch.c31,
                        Eng_Info_Req->info_rr.gsm_bba_cell[3].gsm_cell_info.gsm_cell_info_pbcch.c31,
                        Eng_Info_Req->info_rr.gsm_bba_cell[4].gsm_cell_info.gsm_cell_info_pbcch.c31,
                        Eng_Info_Req->info_rr.gsm_bba_cell[5].gsm_cell_info.gsm_cell_info_pbcch.c31,
                        Eng_Info_Req->info_rr.gsm_bba_cell[6].gsm_cell_info.gsm_cell_info_pbcch.c31,
                        Eng_Info_Req->info_rr.gsm_bba_cell[1].gsm_cell_info.gsm_cell_info_pbcch.c32,
                        Eng_Info_Req->info_rr.gsm_bba_cell[2].gsm_cell_info.gsm_cell_info_pbcch.c32,
                        Eng_Info_Req->info_rr.gsm_bba_cell[3].gsm_cell_info.gsm_cell_info_pbcch.c32,
                        Eng_Info_Req->info_rr.gsm_bba_cell[4].gsm_cell_info.gsm_cell_info_pbcch.c32,
                        Eng_Info_Req->info_rr.gsm_bba_cell[5].gsm_cell_info.gsm_cell_info_pbcch.c32,
                        Eng_Info_Req->info_rr.gsm_bba_cell[6].gsm_cell_info.gsm_cell_info_pbcch.c32,
                        Eng_Info_Req->info_rr.gsm_bba_cell[1].arfcn,
                        Eng_Info_Req->info_rr.gsm_bba_cell[2].arfcn,
                        Eng_Info_Req->info_rr.gsm_bba_cell[3].arfcn,
                        Eng_Info_Req->info_rr.gsm_bba_cell[4].arfcn,
                        Eng_Info_Req->info_rr.gsm_bba_cell[5].arfcn,
                        Eng_Info_Req->info_rr.gsm_bba_cell[6].arfcn,
                        Eng_Info_Req->info_rr.gsm_bba_cell[1].gsm_sync_info.fn_offset,
                        Eng_Info_Req->info_rr.gsm_bba_cell[2].gsm_sync_info.fn_offset,
                        Eng_Info_Req->info_rr.gsm_bba_cell[3].gsm_sync_info.fn_offset,
                        Eng_Info_Req->info_rr.gsm_bba_cell[4].gsm_sync_info.fn_offset,
                        Eng_Info_Req->info_rr.gsm_bba_cell[5].gsm_sync_info.fn_offset,
                        Eng_Info_Req->info_rr.gsm_bba_cell[6].gsm_sync_info.fn_offset,
                        Eng_Info_Req->info_rr.gsm_bba_cell[1].gsm_sync_info.time_align,
                        Eng_Info_Req->info_rr.gsm_bba_cell[2].gsm_sync_info.time_align,
                        Eng_Info_Req->info_rr.gsm_bba_cell[3].gsm_sync_info.time_align,
                        Eng_Info_Req->info_rr.gsm_bba_cell[4].gsm_sync_info.time_align,
                        Eng_Info_Req->info_rr.gsm_bba_cell[5].gsm_sync_info.time_align,
                        Eng_Info_Req->info_rr.gsm_bba_cell[6].gsm_sync_info.time_align
                        );
                        ffs_write(fd, "Neighbouring cell info\n",25 );
                        ffs_write(fd, temp_buffer, strlen(temp_buffer));

#endif
#ifdef NEW_EDITOR
				/*SPR 1757 Insert temp buffer into editor*/
				temp_text.data = (UBYTE*)temp_buffer;
				temp_text.dcs = ATB_DCS_ASCII;
				ATB_string_Length(&temp_text);
				ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
				ATB_edit_ClearAll(data->editor);
				ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
				ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
				ATB_edit_Refresh(data->editor);
#else /* !NEW_EDITOR */
				strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
#endif			
                        win_show(data->win);
			}
			break;

#ifndef NEPTUNE_BOARD                
                    case EM_LOCATION_PARAMS:
			{
                        loc_info = (MFW_EM_Location_Parameters*)para;/*cast parameter to appropriate type*/
				/*copy data to editor buffer*/
				/*MC, SPR 1554 Removed Cell id from editor string*/ 
				sprintf(temp_buffer, "LUP:%d MCC:%s MNC:%s LAC:%d ", loc_info->LUP, loc_info->MCC, loc_info->MNC, loc_info->LAC);
	/* SPR#1428 - SH - New Editor - string has changed, update word wrap */
#ifdef NEW_EDITOR
				/*SPR 1757 Insert temp buffer into editor*/
				temp_text.data = (UBYTE*)temp_buffer;
				temp_text.dcs = ATB_DCS_ASCII;
				ATB_string_Length(&temp_text);
				ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
				ATB_edit_ClearAll(data->editor);
				ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
				ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
				ATB_edit_Refresh(data->editor);
#else /* !NEW_EDITOR */
				strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
#endif	
				/*show the window*/
				win_show(data->win);
			}
			break;
#endif

#ifndef NEPTUNE_BOARD                
			case EM_CIPH_HOP_DTX_PARAMS:
			{
                            ciph_hop_dtx_info = (MFW_EM_Ciph_hop_DTX_Parameters*)para;/*cast parameter to appropriate type*/
				/*convert DTX status to string*/ 
				if (ciph_hop_dtx_info->DTX_status == FALSE)
					DTX_Status = "Off";
				else
					DTX_Status = "On";
				//copy data to editor
				sprintf(temp_buffer, "STATUS:%d HSN:%d DTX:%s ", ciph_hop_dtx_info->ciph_status, ciph_hop_dtx_info->HSN, DTX_Status);
				/*show the window*/
				/* SPR#1428 - SH - New Editor - string has changed, update word wrap */
#ifdef NEW_EDITOR
				/*SPR 1757 Insert temp buffer into editor*/
				temp_text.data = (UBYTE*)temp_buffer;
				temp_text.dcs = ATB_DCS_ASCII;
				ATB_string_Length(&temp_text);
				ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
				ATB_edit_ClearAll(data->editor);
				ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
				ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
				ATB_edit_Refresh(data->editor);
#else /* !NEW_EDITOR */
				strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
#endif
				win_show(data->win);
			}
			break;
#endif

#ifndef NEPTUNE_BOARD                
			case EM_GPRS_PARAMS:
			{
                            gprs_info = (MFW_EM_GPRS_Parameter*)para;//cast parameter to appropriate type
				/*MC, SPR 1554 Changed ediotr string to show Network Mode of Operation*/ 
				sprintf(temp_buffer, "NMO:%d  NDTS:%d  RAC:%d  C31:%d  C32:%d",\
          gprs_info->NMO, gprs_info->NDTS, gprs_info->RAC, gprs_info->C31, gprs_info->C32);
				//show the window
				/* SPR#1428 - SH - New Editor - string has changed, update word wrap */
#ifdef NEW_EDITOR
				/*SPR 1757 Insert temp buffer into editor*/
				temp_text.data = (UBYTE*)temp_buffer;
				temp_text.dcs = ATB_DCS_ASCII;
				ATB_string_Length(&temp_text);
				ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
				ATB_edit_ClearAll(data->editor);
				ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
				ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
				ATB_edit_Refresh(data->editor);
#else /* !NEW_EDITOR */
				strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
#endif
				win_show(data->win);
			}
			break;
#endif
            

#ifdef NEPTUNE_BOARD
            case EM_SERVING_CELL_RF_PARAMS:
                sprintf (temp_buffer, "RSSI = %u\nDTX = %d  \nC1 = %u \nC2 = %u \nCELL-RESELECT HYSTERESIS = %u \nC32 = %d\nRXLEV (FULL) = %u \nRXLEV(SUB) = %u \nRXQUAL(FULL) = %u \nRXQUAL(SUB) = %u \nTXPOWER Timing advance = %u\nSignal variance =%u",
                RssiVal,
                Eng_Info_Req->info_rr.gsm_info.gsm_info_active.gsm_info_dedicated.codec_info.dtx_used,
                Eng_Info_Req->info_rr.gsm_bba_cell[0].gsm_cell_info.gsm_cell_info_bcch.c1,
                Eng_Info_Req->info_rr.gsm_bba_cell[0].gsm_cell_info.gsm_cell_info_bcch.c2,
                Eng_Info_Req->info_rr.gsm_bba_cell[0].gsm_cell_info.gsm_cell_info_bcch.cell_reselect_hysteresis,
                Eng_Info_Req->info_rr.gsm_bba_cell[0].gsm_cell_info.gsm_cell_info_pbcch.c32,
                Eng_Info_Req->info_rr.gsm_info.gsm_info_active.gsm_info_dedicated.rxlev_full,
                Eng_Info_Req->info_rr.gsm_info.gsm_info_active.gsm_info_dedicated.rxlev_sub,
                Eng_Info_Req->info_rr.gsm_info.gsm_info_active.gsm_info_dedicated.rxqual_full,
                Eng_Info_Req->info_rr.gsm_info.gsm_info_active.gsm_info_dedicated.rxqual_sub,
                Eng_Info_Req->info_grlc.channel_quality.timing_advance,
                Eng_Info_Req->info_grlc.channel_quality.signal_variance
                );

                ffs_write(fd, "Serving cell RF info\n",25 );
                ffs_write(fd, temp_buffer, strlen(temp_buffer));
                        /* SPR#1428 - SH - New Editor - string has changed, update word wrap */
                #ifdef NEW_EDITOR
                    /*SPR 1757 Insert temp buffer into editor*/
                        temp_text.data = (UBYTE*)temp_buffer;
                        temp_text.dcs = ATB_DCS_ASCII;
                        ATB_string_Length(&temp_text);
                        ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
                        ATB_edit_ClearAll(data->editor);
                        ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
                        ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
                        ATB_edit_Refresh(data->editor);
                #else /* !NEW_EDITOR */
                        strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
                #endif
                        win_show(data->win);/*show data*/

                break;

            case EM_SERVING_CHANNEL_PARAMS:

                sprintf (temp_buffer, "BCCH = %u \n HSN = %u \nBSIC = %u \n MAIO = %u \nARFCN = %u \nMobile allocation:\n MA index offset = %u\nMA number =%u",
                    Eng_Info_Req->info_rr.gsm_info.gsm_info_active.gsm_info_dedicated.bcch_channel,
                    Eng_Info_Req->info_rr.gsm_info.gsm_info_idle.pccch_info.pccch_channel.gsm_channel_hopping.hsn,
                    Eng_Info_Req->info_rr.gsm_bba_cell[0].gsm_cell_info.gsm_cell_info_pbcch.bsic_match,
                    Eng_Info_Req->info_rr.gsm_info.gsm_info_idle.pccch_info.pccch_channel.gsm_channel_hopping.maio,
                    Eng_Info_Req->info_rr.gsm_bba_cell[0].arfcn,
                    Eng_Info_Req->info_rr.gsm_info.gsm_info_idle.pccch_info.pccch_channel.gsm_channel_hopping.maio,
                    Eng_Info_Req->info_rr.gsm_info.gsm_info_idle.pccch_info.pccch_channel.gsm_channel_hopping.ma_number
                    );

                ffs_write(fd, "Serving Channel info\n",25 );
                ffs_write(fd, temp_buffer, strlen(temp_buffer));
                        /* SPR#1428 - SH - New Editor - string has changed, update word wrap */
                #ifdef NEW_EDITOR
                    /*SPR 1757 Insert temp buffer into editor*/
                        temp_text.data = (UBYTE*)temp_buffer;
                        temp_text.dcs = ATB_DCS_ASCII;
                        ATB_string_Length(&temp_text);
                        ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
                        ATB_edit_ClearAll(data->editor);
                        ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
                        ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
                        ATB_edit_Refresh(data->editor);
                #else /* !NEW_EDITOR */
                        strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
                #endif
                        win_show(data->win);/*show data*/

                break;

            case EM_AMR_PARAMS:

                sprintf (temp_buffer, "Threshold = %u\nHysteresis = %u\n Link quality estimate:\nMean bit error probability = %u\nVariation coefficient of BEP =%u\n\nBEP period = %u\nLQM mode =%u",
                    Eng_Info_Req->info_rr.gsm_bba_cell[0].gsm_cell_info.gsm_cell_info_pbcch.hcs_thr,
                    Eng_Info_Req->info_rr.gsm_bba_cell[0].gsm_cell_info.gsm_cell_info_bcch.cell_reselect_hysteresis,
                    Eng_Info_Req->info_grlc.channel_quality.bep_lqm.bep_lqm_gmsk.mean_bep,
                    Eng_Info_Req->info_grlc.channel_quality.bep_lqm.bep_lqm_gmsk.cv_bep,
                    Eng_Info_Req->info_grlc.channel_quality.bep_lqm.bep_period,
                    Eng_Info_Req->info_grlc.channel_quality.bep_lqm.lqm_mode
                    );

                ffs_write(fd, "Serving AMR info\n",20 );
                ffs_write(fd, temp_buffer, strlen(temp_buffer));                

                        /* SPR#1428 - SH - New Editor - string has changed, update word wrap */
                #ifdef NEW_EDITOR
                    /*SPR 1757 Insert temp buffer into editor*/
                        temp_text.data = (UBYTE*)temp_buffer;
                        temp_text.dcs = ATB_DCS_ASCII;
                        ATB_string_Length(&temp_text);
                        ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
                        ATB_edit_ClearAll(data->editor);
                        ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
                        ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
                        ATB_edit_Refresh(data->editor);
                #else /* !NEW_EDITOR */
                        strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
                #endif
                        win_show(data->win);/*show data*/

                break;

            case EM_SNDCP_PARAMS:

                    sprintf (temp_buffer, "Header compression status = %u %u %u %u\n Data compression status = %u %u %u %u",
                        Eng_Info_Req->info_sm.connection_info_sm[0].compression_header,
                        Eng_Info_Req->info_sm.connection_info_sm[1].compression_header,
                        Eng_Info_Req->info_sm.connection_info_sm[2].compression_header,
                        Eng_Info_Req->info_sm.connection_info_sm[3].compression_header,
                        Eng_Info_Req->info_sm.connection_info_sm[0].compression_data,
                        Eng_Info_Req->info_sm.connection_info_sm[1].compression_data,
                        Eng_Info_Req->info_sm.connection_info_sm[2].compression_data,
                        Eng_Info_Req->info_sm.connection_info_sm[3].compression_data
                        );

                    ffs_write(fd, "Serving SNDCP info\n",21 );
                    ffs_write(fd, temp_buffer, strlen(temp_buffer));                    

                        /* SPR#1428 - SH - New Editor - string has changed, update word wrap */
                #ifdef NEW_EDITOR
                    /*SPR 1757 Insert temp buffer into editor*/
                        temp_text.data = (UBYTE*)temp_buffer;
                        temp_text.dcs = ATB_DCS_ASCII;
                        ATB_string_Length(&temp_text);
                        ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
                        ATB_edit_ClearAll(data->editor);
                        ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
                        ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
                        ATB_edit_Refresh(data->editor);
                #else /* !NEW_EDITOR */
                        strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
                #endif
                        win_show(data->win);/*show data*/
                        break;

                
                case EM_USERLEVEL_PARAMS:

                    sprintf (temp_buffer, "Application Data throughput\n Data rate = %u   %u\n data_rate_retx = %u  %u",
                        Eng_Info_Req->info_grlc.rlc_mac_statistic[0].throughput.data_rate,
                        Eng_Info_Req->info_grlc.rlc_mac_statistic[1].throughput.data_rate,
                        Eng_Info_Req->info_grlc.rlc_mac_statistic[0].throughput.data_rate_retx,
                        Eng_Info_Req->info_grlc.rlc_mac_statistic[1].throughput.data_rate_retx
                        );
                ffs_write(fd, "Serving user level info\n",26 );
                ffs_write(fd, temp_buffer, strlen(temp_buffer));
                        /* SPR#1428 - SH - New Editor - string has changed, update word wrap */
                #ifdef NEW_EDITOR
                    /*SPR 1757 Insert temp buffer into editor*/
                        temp_text.data = (UBYTE*)temp_buffer;
                        temp_text.dcs = ATB_DCS_ASCII;
                        ATB_string_Length(&temp_text);
                        ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
                        ATB_edit_ClearAll(data->editor);
                        ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
                        ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
                        ATB_edit_Refresh(data->editor);
                #else /* !NEW_EDITOR */
                        strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
                #endif
                        win_show(data->win);/*show data*/
                    break;


            case EM_LLCTRACING_PARAMS:

                    sprintf (temp_buffer, "LLC Data throughput UL/DL\n throughput UL: \nData rate= %u\ndata_rate_retx=%u\nthroughput DL\nData rate = %u\nndata_rate_retx = %u",
                        Eng_Info_Req->info_llc.throughput_ul.data_rate,
                        Eng_Info_Req->info_llc.throughput_ul.data_rate_retx,
                        Eng_Info_Req->info_llc.throughput_dl.data_rate,
                        Eng_Info_Req->info_llc.throughput_dl.data_rate_retx
                        );
                        ffs_write(fd, "Serving LLC tracing info\n",26 );
                        ffs_write(fd, temp_buffer, strlen(temp_buffer));

                        /* SPR#1428 - SH - New Editor - string has changed, update word wrap */
                #ifdef NEW_EDITOR
                    /*SPR 1757 Insert temp buffer into editor*/
                        temp_text.data = (UBYTE*)temp_buffer;
                        temp_text.dcs = ATB_DCS_ASCII;
                        ATB_string_Length(&temp_text);
                        ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
                        ATB_edit_ClearAll(data->editor);
                        ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
                        ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
                        ATB_edit_Refresh(data->editor);
                #else /* !NEW_EDITOR */
                        strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
                #endif
                        win_show(data->win);/*show data*/
                break;

            case EM_RLC_MAC_PARAMS:

                    sprintf (temp_buffer, "RLC/MAC Data throughput\n Data through-put value in bits/s = %u  %u\nData through-put value of retransmitted bytes in bits/s = %u  %u",
                        Eng_Info_Req->info_grlc.rlc_mac_statistic[0].throughput.data_rate,
                        Eng_Info_Req->info_grlc.rlc_mac_statistic[1].throughput.data_rate,
                        Eng_Info_Req->info_grlc.rlc_mac_statistic[0].throughput.data_rate_retx,
                        Eng_Info_Req->info_grlc.rlc_mac_statistic[1].throughput.data_rate_retx
                        );
                    ffs_write(fd, "Serving RLC/MAC info\n",23 );
                    ffs_write(fd, temp_buffer, strlen(temp_buffer));

                        /* SPR#1428 - SH - New Editor - string has changed, update word wrap */
                #ifdef NEW_EDITOR
                    /*SPR 1757 Insert temp buffer into editor*/
                        temp_text.data = (UBYTE*)temp_buffer;
                        temp_text.dcs = ATB_DCS_ASCII;
                        ATB_string_Length(&temp_text);
                        ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
                        ATB_edit_ClearAll(data->editor);
                        ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
                        ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
                        ATB_edit_Refresh(data->editor);
                #else /* !NEW_EDITOR */
                        strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
                #endif
                        win_show(data->win);/*show data*/
                break;

            case EM_GMM_INFO_PARAMS:

                    sprintf (temp_buffer, "Attach state = %u \nSM information (per PDP context) \nNo of active primary PDP contexts= %u\nNo of active secondary PDP context= %u Radio priority =%u\n\nDelay class = %d\nReliability class =%d\nPeak throughput = %d\nPrecedence class = %d\nMean throughput =%d\n\nIPv4/v6 address\nipv4 = %d\nipv6 = %d",
                        Eng_Info_Req->info_sm.attached,
                        Eng_Info_Req->info_sm.pdp_context_count_primary,
                        Eng_Info_Req->info_sm.pdp_context_count_secondary,
                        Eng_Info_Req->info_sm.connection_info_sm[2].radio_prio,
                        Eng_Info_Static_Req->info_sm.context_info[0].qos.qos_r97.delay,
                        Eng_Info_Static_Req->info_sm.context_info[0].qos.qos_r97.relclass,
                        Eng_Info_Static_Req->info_sm.context_info[0].qos.qos_r97.peak,
                        Eng_Info_Static_Req->info_sm.context_info[0].qos.qos_r97.preced,
                        Eng_Info_Static_Req->info_sm.context_info[0].qos.qos_r97.mean,
                        Eng_Info_Static_Req->info_sm.context_info[0].ip_address.ipv4_addr.aelement[0],
                        Eng_Info_Static_Req->info_sm.context_info[0].ip_address.ipv6_addr.aelement[0]
                        );
                    ffs_write(fd, "Serving GMM info\n",20 );
                    ffs_write(fd, temp_buffer, strlen(temp_buffer));
                        /* SPR#1428 - SH - New Editor - string has changed, update word wrap */
                #ifdef NEW_EDITOR
                    /*SPR 1757 Insert temp buffer into editor*/
                        temp_text.data = (UBYTE*)temp_buffer;
                        temp_text.dcs = ATB_DCS_ASCII;
                        ATB_string_Length(&temp_text);
                        ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
                        ATB_edit_ClearAll(data->editor);
                        ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
                        ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
                        ATB_edit_Refresh(data->editor);
                #else /* !NEW_EDITOR */
                        strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
                #endif
                        win_show(data->win);/*show data*/

                break;

            case EM_CALL_STATUS_PARAMS:

                    sprintf (temp_buffer, "CALL STATUS\nTransaction id = %u\nDirection = %u \nStatecc = %u \nOn hold = %u",
                        Eng_Info_Req->info_cc.connection_info_cc[0].transaction_id,
                        Eng_Info_Req->info_cc.connection_info_cc[0].direction,
                        Eng_Info_Req->info_cc.connection_info_cc[0].state_cc,
                        Eng_Info_Req->info_cc.connection_info_cc[0].on_hold
                        );
                    ffs_write(fd, "Serving call status info\n",27 );
                    ffs_write(fd, temp_buffer, strlen(temp_buffer));
                        /* SPR#1428 - SH - New Editor - string has changed, update word wrap */
                #ifdef NEW_EDITOR
                    /*SPR 1757 Insert temp buffer into editor*/
                        temp_text.data = (UBYTE*)temp_buffer;
                        temp_text.dcs = ATB_DCS_ASCII;
                        ATB_string_Length(&temp_text);
                        ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
                        ATB_edit_ClearAll(data->editor);
                        ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
                        ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
                        ATB_edit_Refresh(data->editor);
                #else /* !NEW_EDITOR */
                        strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
                #endif
                        win_show(data->win);/*show data*/

                break;

            case EM_LAYER_MESSAGE_PARAMS:

                    sprintf (temp_buffer, "LAYER MESSAGE\nLogical Channel: \narfcn=%d\n\nMobile allocation index offset=%d\nHopping sequence number =%d\nvalid-flag=%d\nMobile allocation number = %d",
                        Eng_Info_Req->info_rr.gsm_info.gsm_info_idle.pccch_info.pccch_channel.gsm_channel_static.arfcn,
                        Eng_Info_Req->info_rr.gsm_info.gsm_info_idle.pccch_info.pccch_channel.gsm_channel_hopping.maio,
                        Eng_Info_Req->info_rr.gsm_info.gsm_info_idle.pccch_info.pccch_channel.gsm_channel_hopping.hsn,
                        Eng_Info_Req->info_rr.gsm_info.gsm_info_idle.pccch_info.pccch_channel.gsm_channel_hopping.v_ma_number,
                        Eng_Info_Req->info_rr.gsm_info.gsm_info_idle.pccch_info.pccch_channel.gsm_channel_hopping.ma_number
                        );
                    ffs_write(fd, "Serving Layer message\n",25 );
                    ffs_write(fd, temp_buffer, strlen(temp_buffer));
                        /* SPR#1428 - SH - New Editor - string has changed, update word wrap */
                #ifdef NEW_EDITOR
                    /*SPR 1757 Insert temp buffer into editor*/
                        temp_text.data = (UBYTE*)temp_buffer;
                        temp_text.dcs = ATB_DCS_ASCII;
                        ATB_string_Length(&temp_text);
                        ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
                        ATB_edit_ClearAll(data->editor);
                        ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
                        ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
                        ATB_edit_Refresh(data->editor);
                #else /* !NEW_EDITOR */
                        strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
                #endif
                        win_show(data->win);/*show data*/
                break;

            case EM_WCDMA_LAYER_MESSAGE_PARAMS:

                    sprintf (temp_buffer, "%s",
                        "WCDMA LAYER MESSAGE"
                        );
                    ffs_write(fd, "Serving WCDMA Layer info\n",32 );
                    ffs_write(fd, temp_buffer, strlen(temp_buffer));
                        /* SPR#1428 - SH - New Editor - string has changed, update word wrap */
                #ifdef NEW_EDITOR
                    /*SPR 1757 Insert temp buffer into editor*/
                        temp_text.data = (UBYTE*)temp_buffer;
                        temp_text.dcs = ATB_DCS_ASCII;
                        ATB_string_Length(&temp_text);
                        ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
                        ATB_edit_ClearAll(data->editor);
                        ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
                        ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
                        ATB_edit_Refresh(data->editor);
                #else /* !NEW_EDITOR */
                        strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
                #endif
                        win_show(data->win);/*show data*/
                break;

            case EM_3G_INFO_PARAMS:

                sprintf (temp_buffer, "%s",
                    "3G INFO"
                    );
                ffs_write(fd, temp_buffer, strlen(temp_buffer));
                        /* SPR#1428 - SH - New Editor - string has changed, update word wrap */
                #ifdef NEW_EDITOR
                    /*SPR 1757 Insert temp buffer into editor*/
                        temp_text.data = (UBYTE*)temp_buffer;
                        temp_text.dcs = ATB_DCS_ASCII;
                        ATB_string_Length(&temp_text);
                        ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
                        ATB_edit_ClearAll(data->editor);
                        ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
                        ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
                        ATB_edit_Refresh(data->editor);
                #else /* !NEW_EDITOR */
                        strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
                #endif
                        win_show(data->win);/*show data*/
                break;

            case EM_PACKET_DATA_PARAMS:

                sprintf (temp_buffer, "PACKET DATA\nGMM State = %d\n ",
                        Eng_Info_Req->info_mm.mm_reg_ps_info.gmm_main_state
                        );
                ffs_write(fd, "Serving packet data info\n",27 );
                ffs_write(fd, temp_buffer, strlen(temp_buffer));
                        /* SPR#1428 - SH - New Editor - string has changed, update word wrap */
                #ifdef NEW_EDITOR
                    /*SPR 1757 Insert temp buffer into editor*/
                        temp_text.data = (UBYTE*)temp_buffer;
                        temp_text.dcs = ATB_DCS_ASCII;
                        ATB_string_Length(&temp_text);
                        ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
                        ATB_edit_ClearAll(data->editor);
                        ATB_edit_InsertString(data->editor, &temp_text);			/* Insert name string */
                        ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */
                        ATB_edit_Refresh(data->editor);
                #else /* !NEW_EDITOR */
                        strncpy(data->edtBuffer, temp_buffer, EM_EDITOR_SIZE);/*SPR 1757*/
                #endif
                        win_show(data->win);/*show data*/

                break;
            
#endif   /*ifndef NEPTUNE BOARD */
		default:
			free(temp_buffer);
			return MFW_EVENT_REJECTED; 
		}
	}
	else
		{
		free(temp_buffer);
		return MFW_EVENT_REJECTED;
		}

#ifdef NEPTUNE_BOARD
    ffs_write(fd, "\n****************\n\n",20 );
    ffs_close(fd);
#endif
    free(temp_buffer);
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:    	Mmi_em_destroy()
 $Description:	Delete the window
 $Returns:		none
 $Arguments:	window handle 
*******************************************************************************/
void Mmi_em_destroy(T_MFW_HND own_window)
{
  T_MFW_WIN     * win  = ((T_MFW_HDR *)own_window)->data;
  tEmData * data = (tEmData *)win->user;

  TRACE_EVENT ("Mmi_em_destroy()");

  if (own_window == NULL)
  {
	TRACE_EVENT ("Error :- Called with NULL Pointer");
	return;
  }
  if (data)
  {
    /*
     * Exit TIMER & KEYBOARD Handle
     */
    kbd_delete (data->kbd);
    tim_delete (data->tim);
    /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	ATB_edit_Destroy(data->editor);
#else
	edt_delete(data->edt);
#endif
    /*
     * Delete WIN Handler
     */
    win_delete (data->win);
    /*
     * Free Memory
     */
    FREE_MEMORY ((void *)data, sizeof (tEmData));
  }
  TRACE_EVENT_P1("MMiEm END, Memory left:%d", mfwCheckMemoryLeft());
}

/*******************************************************************************

 $Function:    	Mmi_em_kbd_cb()

 $Description:	Keyboard handler
 
 $Returns:		status int

 $Arguments:	event, keyboard data
 
*******************************************************************************/
int Mmi_em_kbd_cb(T_MFW_EVENT event,T_MFW_KBD *  kc )
{
    T_MFW_HND       win  = mfw_parent (mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    tEmData * data = (tEmData *)win_data->user;
#ifdef NEPTUNE_BOARD /*EngMode */
    char strTemp[4];
    int nCopy;

    char pic[9];
    U16 *p;
    fd_t fd;
    char file[15];

    static char command[10];

    if(strlen(command) > 8)
    {
        memset(command, '\0', sizeof(command));
    }
#endif
        
    TRACE_EVENT_P2("Mmi_em_kbd_cb, key:%d, win;%d", kc->code, data->win);
  
    switch(kc->code)
    {
    
        case KCD_MNUUP:/*scroll up */
        /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
        ATB_edit_MoveCursor(data->editor, ctrlUp, TRUE);
#else /* NEW_EDITOR */
        edtChar(data->edt,ecUp);
#endif /* NEW_EDITOR */
            break;
        
        case KCD_MNUDOWN:/*scroll down*/
        /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
            ATB_edit_Char(data->editor,ctrlDown, TRUE);
#else /* NEW_EDITOR */
            edtChar(data->edt,ecDown);
#endif /* NEW_EDITOR */
 
            break;
#ifdef NEPTUNE_BOARD /*EngMode */
                case KCD_0:
                    strcat(command, "0");
                    break;
                case KCD_1:
                    strcat(command, "1");
                    break;
                case KCD_2:
                    strcat(command, "2");
                    break;
                case KCD_3:
                    strcat(command, "3");
                    break;
                case KCD_4:
                    strcat(command, "4");
                    break;
                case KCD_5:
                    strcat(command, "5");
                    break;
                case KCD_6:
                    strcat(command, "6");
                    break;
                case KCD_7:
                    strcat(command, "7");
                    break;
                case KCD_8:
                    strcat(command, "8");
                    break;
                case KCD_9:
                    strcat(command, "9");
                    break;
                case KCD_STAR:
                    strcat(command, "*");
                    break;
                case KCD_HASH:
                    strcat(command, "#");
                    if(!strcmp(command,"###1234#"))
                    {
                        TRACE_EVENT_P1("Handle capture, command :%s", command);                

                        strcpy(file, "/NOR/pic");
//                        strcpy(strTemp, "/NOR/pic");
                        memset(strTemp, '\0', sizeof(strTemp));
                        nCopy = 0;
                        while ( (fd = (int) ffs_open(  file, FFS_O_RDONLY )) > 0 )
                        {

                                nCopy++;
                                sprintf(strTemp, "%d", nCopy);
                                strcat(file, strTemp);
                                ffs_close((fd_t) fd);
                        }
                        TRACE_EVENT_P1("nCopy:%d", nCopy); 
                        sprintf( strTemp, "%d", nCopy);
//                        strcat( file, strTemp );
                        TRACE_EVENT_P1("file:%s", file); 
                        fd = ffs_open(file, FFS_O_CREATE|FFS_O_WRONLY|FFS_O_TRUNC);
                        ffs_write(fd, picture_col, 320*240);
                        ffs_close(fd);

                        memset(command, '\0', sizeof(command));

                    }
                    
                    break;
#endif

        case KCD_HUP:	/*destroy window*/
        case KCD_RIGHT:
#ifdef NEPTUNE_BOARD            
            EmRequest = NULL;
            sAT_PercentEINFO(CMD_SRC_LCL,TIMER_STOP);
            sAT_PercentESINFO(CMD_SRC_LCL,TIMER_STOP);
            Mmi_em_destroy(data->win);
            memset(command, '\0', sizeof(command));
#else
            Mmi_em_destroy(data->win);
#endif
            break;
        default:
            break;
    }	
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	Mmi_em_win_cb()

 $Description:	Display handler
 
 $Returns:		status int

 $Arguments:	event, window handle
 
*******************************************************************************/
int Mmi_em_win_cb(T_MFW_EVENT event,T_MFW_WIN * win )
{
    tEmData* data = (tEmData *)win->user;

    TRACE_EVENT_P1("EM window: %d", win);
    TRACE_EVENT_P1("Mmi_em_win_cb(), data->CurrentWindow %d", data->CurrentWindow);

    switch( event )
    {
        case MfwWinVisible:
            /*clear screen*/
            dspl_ClearAll();

       /*Show editor content*/
       /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
            ATB_edit_Show(data->editor);
#else /* NEW_EDITOR */
            edtShow(data->edt);
#endif /* NEW_EDITOR */

            displaySoftKeys(TxtNull, TxtSoftBack); /*show the softkeys*/
            break;
        default:
            return MFW_EVENT_PASSED;
        /*break;*/
    }

    return MFW_EVENT_CONSUMED;

}

/*******************************************************************************

 $Function:    	Mmi_em_tim_cb()

 $Description:	Timer handler (every second)
 
 $Returns:		none

 $Arguments:	event, timer data
 
*******************************************************************************/
void Mmi_em_tim_cb(T_MFW_EVENT event,T_MFW_TIM * t)
{
    T_MFW_HND win  = mfw_parent (mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    tEmData * data = (tEmData *)win_data->user;
    TRACE_EVENT_P1("Mmi_em_tim_cb(), win:%d", data->win);
    /*Request the data from MFW again*/
    Mfw_em_get_data(data->CurrentWindow);
    /*restart timer*/
    timStart(data->tim);
}
