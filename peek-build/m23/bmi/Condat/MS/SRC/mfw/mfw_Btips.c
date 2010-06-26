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
 $File:       Mfw_Btips.c
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS BMG APPlication functionality.

********************************************************************************
 $History: Mfw_Btips.c

  26/06/07 Sasken original version

 $End

*******************************************************************************/




/*******************************************************************************

                                Include files

*******************************************************************************/

#define ENTITY_MFW
/* includes */

#include <string.h>


/* BTIPS Includes */
#include "me.h"
#include "osapi.h"
#include "btl_common.h"
#include "bthal_btdrv.h"
#include "bthal_fs.h"
#include "bthal_pm.h"
#include "bthal_config.h"
#include "bttypes.h"
#include "goep.h"
#include "bpp.h"
#include "Debug.h"

/* BTL includes */
#include "btl_bppsnd.h"
#include "btl_bmg.h"

#if FM_STACK == XA_ENABLED

#include "fms_api.h"
#endif

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

#include "Mfw_Btips.h"
#include "ffs/ffs_api.h"
/*------------------------------------------------------------------------
 *
 * Default BT Demo folder fsPath (i.e. \MfwBtDemo)
 */
U8  Bt_DefaultDemoDir[BT_FS_MAX_PATH_LENGTH];

#define hCommACI _ENTITY_PREFIXED(hCommACI)


#define MFW_BTIPS_RADIO_OFF 					0
#define MFW_BTIPS_RADIO_ON 					1
#define MFW_BTIPS_RADIO_IN_PROGRESS 			-1

EXTERN MfwHdr * current_mfw_elem;


/* Declaration of static variables */
static BOOL isBtRadioEnabled = MFW_BTIPS_RADIO_OFF;	/* To check at start-up */

static void btips_btlCallBack(const BtlEvent *event);
static void mfw_btips_btlCallBack(T_BTIPS_MMI_IND *btips_mmi_ind);
static void mfw_btips_callBack(T_BTIPS_MMI_IND *btips_mmi_ind );
static BOOL mfw_btips_createDemoRootDir(const BTHAL_U8 *fsPath);
BOOL mfw_btips_checkStatus(BtStatus btStatus);

void Btips_ffs_log_clear(void)
{
	ffs_remove((const char*)"/ffslog");
}
void Btips_ffs_log_message(char *LogMsg,...)
{  
#ifdef FFSLOG
      fd_t fd= -1;   
       char log_ffs_buf[256]; 
       va_list varpars;

 va_start (varpars, LogMsg);      
 vsprintf(log_ffs_buf,LogMsg,varpars);
 va_end (varpars);     
  fd = ffs_open((const char*)"/ffslog", FFS_O_APPEND | FFS_O_WRONLY );
        if(fd < 0) 
           fd=ffs_open((const char*)"/ffslog", FFS_O_CREATE |FFS_O_WRONLY );
        if(fd >= 0)
        {
           strcat(log_ffs_buf," \n ");
           ffs_write(fd, log_ffs_buf, strlen(log_ffs_buf));
           ffs_close(fd);
         }
#endif
}
#ifdef FF_MMI_BTIPS_APP

/************************************************************************
	             MFW - ACI Communication Module Start
************************************************************************/

/*******************************************************************************

 $Function:		mfw_btips_sign_exec

 $Description:		This function sends the Mfw BTIPS events from the Mfw to the BMI.

 $Returns:		None

 $Arguments:		None

*******************************************************************************/
BOOL mfw_btips_sign_exec (T_MFW_HDR * cur_elem, T_MFW_EVENT event, T_MFW_BTIPS_PARA * para)
{
	TRACE_FUNCTION("mfw_btips_sign_exec()");

	while (cur_elem)
	{
		/*
		* event handler is available
		*/
			TRACE_FUNCTION_P1("mfw_btips_sign_exec - %x", event);
			if (cur_elem->type EQ MfwTyBtipsBmg)
			{
			T_MFW_BTIPS * btips_data;

			TRACE_FUNCTION("mfw_btips_sign_exec() cur_elem->type EQ MfwTyBtipsBmg");
			/*
			* handler is CM management handler
			*/
			btips_data = (T_MFW_BTIPS *)cur_elem->data;
			if (btips_data->emask & event)
			{
				/*
				* event is expected by the call back function
				*/
				btips_data->event = event;
				switch (event)
				{
					/*
					** BTIPS BMG Events
					*/
					case E_BTIPS_BMG_NAME_RESULT:
					case E_BTIPS_BMG_SEARCH_COMPLETE:
					case E_BTIPS_BMG_SEARCH_CANCELLED: 
					case E_BTIPS_BMG_BOND_COMPLETE:
					case E_BTIPS_BMG_BOND_CANCELLED:
					case E_BTIPS_BMG_DEVICE_SERVICES:
						if(para!=NULL)
							memcpy (&btips_data->para, para, sizeof (T_MFW_BTIPS_PARA));
						break; 
					default:
						return FALSE; 

				}

				/*
				* if call back defined, call it
				*/
				if (btips_data->handler)
				{					
					current_mfw_elem = cur_elem;

					if ((*(btips_data->handler)) (btips_data->event, (void *)&btips_data->para))
						return TRUE;
				}
			}
		}
		else if (cur_elem->type EQ MfwTyBtips)
		{
			T_MFW_BTIPS * btips_data;

			TRACE_FUNCTION("mfw_btips_sign_exec() cur_elem->type EQ MfwTyBtips");
		/*
			* handler is CM management handler
			*/
			btips_data = (T_MFW_BTIPS *)cur_elem->data;
			if (btips_data->emask & event)
			{
				/*
				* event is expected by the call back function
				*/
				btips_data->event = event;
				switch (event)
				{
					/*
					** BTIPS generic Events
					*/
					case E_BTIPS_POWERON_SUCCESS:
					case E_BTIPS_POWERON_FAILURE:
					case E_BTIPS_POWEROFF_SUCCESS:
					case E_BTIPS_BT_NOT_ON:
					case E_BTIPS_FATAL_ERROR:
					case E_BTIPS_INCOMING_PIN_REQ:
					case E_BTIPS_BMG_PAIRING_COMPLETE:
					case E_BTIPS_OPPS_PUSH_EVENT:
					case E_BTIPS_OPPS_PULL_EVENT:
					case E_BTIPS_FTPS_PUT_EVENT:
					case E_BTIPS_FTPS_GET_EVENT:
					case E_BTIPS_FTPS_DELETE_EVENT:
					case E_BTIPS_FTPS_COMPLETE_EVENT:	//sundeep
					case E_BTIPS_FTPS_ABORTED_EVENT:
					case E_BTIPS_FTPS_PUT_COMP_EVENT:
				//	case E_BTIPS_FTPS_GET_COMP_EVENT:
					case E_BTIPS_FTPS_CONNECT_EVENT:
					case E_BTIPS_FTPS_DISCONNECT_EVENT:
					case E_BTIPS_FTPS_DELETE_COMP_EVENT:	
					case E_BTIPS_VG_CONNECT_EVENT:	
					case E_BTIPS_VG_DISCONNECT_EVENT:
					case E_BTIPS_VG_AUDIO_CONNECT_EVENT:
					case E_BTIPS_VG_AUDIO_DISCONNECT_EVENT:
					case E_BTIPS_BMG_NAME_RESULT:
					case E_BTIPS_OPPS_PULL_COMP_EVENT:
					case E_BTIPS_OPPS_PUSH_COMP_EVENT:
						if(para!=NULL)
							memcpy (&btips_data->para, para, sizeof (T_MFW_BTIPS_PARA));
					
						break;
					default:
						return FALSE; 
				}

				/*
				* if call back defined, call it
				*/
				if (btips_data->handler)
				{					
					current_mfw_elem = cur_elem;

					if ((*(btips_data->handler)) (btips_data->event, (void *)&btips_data->para))
						return TRUE;
				}
			}
		}
		
		
		cur_elem = cur_elem->next;
	}
	return FALSE;
}

/*******************************************************************************

 $Function:		mfw_btips_signal

 $Description:		This function sends the Mfw btipsnts from the Mfw to the BMI.

 $Returns:		None

 $Arguments:		T_MFW_EVENT	: The event to be sent to the BMI
 				T_BTIPS_MMI_IND *			: Pointer to the Event data

*******************************************************************************/
void mfw_btips_signal(T_MFW_EVENT event, T_BTIPS_MMI_IND * para)
{
	UBYTE temp;
	temp = dspl_Enable(0);

	TRACE_FUNCTION("mfw_btips_signal()");

	if (mfwSignallingMethod EQ 0)
	{
		/*
		* focus is on a window
		*/
		if (mfwFocus)
		{
			/*
			* send event to sim management
			* handler if available
			*/
			if (mfw_btips_sign_exec (mfwFocus, event, para))
			{
				dspl_Enable(temp);
				return;
			}
		}

		/*
		* actual focussed window is not available
		* or has no network management registration
		* handler, then search all nodes from the root.
		*/
		if (mfwRoot)
			mfw_btips_sign_exec (mfwRoot, event, para);
	}
	else
	{
		MfwHdr * h = 0;

		/*
		* Focus set, then start here
		*/
		if (mfwFocus)
			h = mfwFocus;

		/*
		* Focus not set, then start root
		*/
		if (!h)
			h = mfwRoot;

		/*
		* No elements available, return
		*/
		while (h)
		{
			/*
			* Signal consumed, then return
			*/
			if (mfw_btips_sign_exec (h, event, para))
			{
				dspl_Enable(temp);
				return;
			}

			/*
			* All windows tried inclusive root
			*/
			if (h == mfwRoot)
			{
				dspl_Enable(temp);
				return;
			}

			/*
			* get parent window
			*/
			h = mfwParent(mfwParent(h));

			if (h)
				h = ((MfwWin * )(h->data))->elems;
		}
		
		mfw_btips_sign_exec (mfwRoot, event, para);
		
	}
	
	dspl_Enable(temp);

	return;
}


/*******************************************************************************

 $Function:		mfw_btips_create

 $Description:		This function  initialises an Mfw BTIPS entity and adds it to the Window Stack

 $Returns:		T_MFW_HND	: A handle for the entity

 $Arguments:		T_MFW_HND		: Parent Window Handle
 				T_MFW_EVENT	: Event Mask of the events to be handled
 				T_MFW_CB		: Callback function to handle the events

*******************************************************************************/
T_MFW_HND mfw_btips_create(T_MFW_HND hWin, T_MFW_EVENT event, MfwTyp type, T_MFW_CB cbfunc)
{
	T_MFW_HDR *hdr;
	T_MFW_BTIPS  *btips_para;

	TRACE_FUNCTION_P1("mfw_btips_create TYPE = %x", type);
	
	hdr = (T_MFW_HDR *) mfwAlloc(sizeof (T_MFW_HDR));
	btips_para = (T_MFW_BTIPS *) mfwAlloc(sizeof (T_MFW_BTIPS));

	if (!hdr OR !btips_para)
		return FALSE;

	/*
	* initialisation of the handler
	*/
	btips_para->emask   = event;
	btips_para->handler = cbfunc;

	hdr->data = btips_para;		
	hdr->type = type;		

	/*
	* installation of the handler
	*/
	return mfwInsert((T_MFW_HDR *)hWin, hdr);
}


/*******************************************************************************

 $Function:		mfw_btips_delete

 $Description:		This function clears down an Mfw BTIPS entity and removes it from the
 				Window Stack

 $Returns:		T_MFW_RES	: The result of the function

 $Arguments:		T_MFW_HND	: The Handle of the entity to be removed

*******************************************************************************/
T_MFW_RES mfw_btips_delete(T_MFW_HND hnd)
{
	TRACE_FUNCTION("mfw_btips_delete()");

	if (!hnd OR !((T_MFW_HDR *)hnd)->data)
		return MFW_RES_ILL_HND;

	if (!mfwRemove((T_MFW_HDR *)hnd))
		return MFW_RES_ILL_HND;

	mfwFree((U8 *)(((T_MFW_HDR *) hnd)->data),sizeof(T_MFW_BTIPS));
	mfwFree((U8 *)hnd,sizeof(T_MFW_HDR));

	return MFW_RES_OK;
}



/*
+------------------------------------------------------------------------------
|  Function     :  btipsNotifyMFW
+------------------------------------------------------------------------------
|  Description  :  Posts the FM Radio events to the MMI task 
|
|  Parameters   :  T_FMRADIO_EVENT fmradioevent
|                         void context - not used now
|
|  Return       :  TRUE/FALSE
+------------------------------------------------------------------------------
*/
void btipsNotifyMFW (T_BTIPS_MMI_IND * event)
{
#if defined (NEW_FRAME)
	EXTERN T_HANDLE hCommACI;
#else
	EXTERN T_VSI_CHANDLE hCommACI;
#endif

	PALLOC(btips_mmi_ind,BTIPS_MMI_IND);
      TRACE_FUNCTION_P1 ("btipsNotifyMFW %x", event->mfwBtipsEventType);
	  /*Copy required data to btips_mmi_ind*/
	btips_mmi_ind->mfwBtipsEventType = event->mfwBtipsEventType;
	btips_mmi_ind->data = event->data;

#if defined (NEW_FRAME)
	PSENDX(ACI,btips_mmi_ind);
#else
#if defined (_TMS470)
	vsi_c_send("",hCommACI,D2P(btips_mmi_ind),
	        sizeof(T_PRIM_HEADER)+sizeof(T_BTIPS_MMI_IND));
#else
	PSEND(ACI,btips_mmi_ind);
#endif
#endif
 	return;
 }
/*
+------------------------------------------------------------------------------
|  Function     :  btipsPrimHandler
+------------------------------------------------------------------------------
|  Description  :  This fn is the primitive handler for BTIPS MFW application
|
|  Parameters   :  opc-opc code
|                           data - data
|
|
|  Return       :  TRUE/FALSE
+------------------------------------------------------------------------------
*/
GLOBAL BOOL btipsPrimHandler (USHORT opc, void * data)
{	
	TRACE_FUNCTION_P1 ("btipsPrimHandler %x", opc);

	if (opc EQ BTIPS_MMI_IND)
	{
		T_BTIPS_MMI_IND *btips_mmi_ind = (T_BTIPS_MMI_IND *)data;
		mfw_btips_callBack(btips_mmi_ind);
		PFREE(btips_mmi_ind);
		return TRUE;
	}
  	return FALSE;  
}

/*This function will be invoked in the ACI context and from here the signals will be
passed on to the respective mfw_btips_xxxx_callback()*/
static void mfw_btips_callBack(T_BTIPS_MMI_IND *btips_mmi_ind )
{
/*Todo: implement function pointer callback*/
	switch(btips_mmi_ind->mfwBtipsEventType)
	{
		case MFW_BTIPS_MAIN_EVENT:
			mfw_btips_btlCallBack(btips_mmi_ind);
			break;
		case MFW_BTIPS_BMG_EVENT:
			mfw_btips_bmgCallback(btips_mmi_ind);
			break;

		case MFW_BTIPS_OPPS_EVENT:
			mfw_btips_oppsCallback(btips_mmi_ind);
			break;

		case MFW_BTIPS_FTPS_EVENT:
			mfw_btips_ftpsCallback(btips_mmi_ind);
			break;

		case MFW_BTIPS_VG_EVENT:
			mfw_btips_vgCallback(btips_mmi_ind);
			break;
			
		case MFW_BTIPS_OPPC_EVENT:
			mfw_btips_oppcCallback(btips_mmi_ind);
			break;
			
		case MFW_BTIPS_MDG_EVENT:
			mfw_btips_mdgCallback(btips_mmi_ind);
			break;

		default:
			break;
	}
}
/************************************************************************
	             MFW - ACI Communication Module End
************************************************************************/

/*---------------------------------------------------------------------------
 *            mfw_btips_init
 *---------------------------------------------------------------------------
 *
 * Synopsis:  To return the current status of BT radio.		
 *
 * Return:    TRUE if send PIN code is successful, FALSE otherwise.
 *
 */
void mfw_btips_init(void)
{

//	BTHAL_OS_Sleep(100);  
    /* Initialize BTIPS basic components: BT stack, BTL layer... */
	TRACE_FUNCTION(("mfw_btips_init: Starting BTL_Init"));	
       OS_StrCpy(Bt_DefaultDemoDir, BT_DEMO_FOLDER_PATH);
	if (TRUE != mfw_btips_createDemoRootDir(Bt_DefaultDemoDir))
       {
                Report(("Fatal Error: Could not create BT root folder\n"));;
       }
	if (BT_STATUS_SUCCESS != BTL_Init(btips_btlCallBack))
	{
		mfw_BtipsReport(("Fatal Error: Could not initialize the BTIPS basic components\n"));
	}

    /* Initialize application modules */
	mfw_btips_bmgInit();
	mfw_btips_a2dpInit();
	mfw_btips_ftpsInit();
	mfw_btips_vgInit();
	mfw_btips_oppsInit();
	mfw_btips_oppcInit();
mfw_btips_mdgInit();
	
	#if FM_STACK == XA_ENABLED
 	Assert(FM_STATUS_SUCCESS == FM_Init());
	#endif
	

}

/*---------------------------------------------------------------------------
 *            mfw_btips_getRadioStatus
 *---------------------------------------------------------------------------
 *
 * Synopsis:  To return the current status of BT radio.		
 *
 * Return:    TRUE if send PIN code is successful, FALSE otherwise.
 *
 */
BOOL mfw_btips_getRadioStatus (void)
{
	return isBtRadioEnabled;
}


/*---------------------------------------------------------------------------
 *            mfw_btips_radioOn
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enable BT radio.		
 *
 * Return:    TRUE if Enabling BT radio is successful, FALSE otherwise.
 *
 */
BOOL mfw_btips_radioOn (void)
{
	BtStatus btStatus;
	
	TRACE_FUNCTION("mfw_btips_radioOn()");
	
	Btips_ffs_log_clear();

	if (MFW_BTIPS_RADIO_OFF == isBtRadioEnabled)
	{
		isBtRadioEnabled = MFW_BTIPS_RADIO_IN_PROGRESS;
		btStatus = BTL_RadioOn();
		Assert(MFW_BTIPS_SUCCESS == mfw_btips_checkStatus(btStatus));
	}
	else
	{
		TRACE_EVENT_P1("mfw_btips_radioOn -BT radio is already enabled %d", isBtRadioEnabled);
	}
}


/*---------------------------------------------------------------------------
 *            mfw_btips_radioOff
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disable BT radio.		
 *
 * Return:    TRUE if Disabling BT radio is successful, FALSE otherwise.
 *
 */
BOOL mfw_btips_radioOff (void)
{
	BtStatus btStatus;

	TRACE_FUNCTION("mfw_btips_radioOff()");

	if (MFW_BTIPS_RADIO_ON == isBtRadioEnabled)
	{
		isBtRadioEnabled = MFW_BTIPS_RADIO_IN_PROGRESS;
		btStatus = BTL_RadioOff();
		Assert(MFW_BTIPS_SUCCESS == mfw_btips_checkStatus(btStatus));
	}
	else
	{
		mfw_BtipsReport(("APP MAIN: BT radio is already disabled"));
	}
}

/*---------------------------------------------------------------------------
 *            btips_btlCallBack
 *---------------------------------------------------------------------------
 *
 * Synopsis:  BTL Main application callback in BTS Context		
 *
 * Return:    
 *
 */
static void btips_btlCallBack(const BtlEvent *event)
{
	T_BTIPS_MMI_IND	btips_mmi_ind;
	
	TRACE_FUNCTION_P1("btips_btlCallBack %d", event->type);
	btips_mmi_ind.mfwBtipsEventType = MFW_BTIPS_MAIN_EVENT;
	switch (event->type)
	{
		case BTL_EVENT_RADIO_OFF_COMPLETE:
			isBtRadioEnabled = MFW_BTIPS_RADIO_OFF;
			btips_mmi_ind.data.btlBtEvent = BTL_EVENT_RADIO_OFF_COMPLETE;
			btipsNotifyMFW(&btips_mmi_ind);
			break;
	
		case BTL_EVENT_RADIO_ON_COMPLETE:
			// Display Bluetooth Icon on the opening/default main window
			isBtRadioEnabled = MFW_BTIPS_RADIO_ON;
			btips_mmi_ind.data.btlBtEvent = BTL_EVENT_RADIO_ON_COMPLETE;
			btipsNotifyMFW(&btips_mmi_ind);
			break;
	}
}

/*---------------------------------------------------------------------------
 *            mfw_btips_radioOff
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disable BT radio.		
 *
 * Return:    TRUE if Disabling BT radio is successful, FALSE otherwise.
 *
 */

static void mfw_btips_btlCallBack(T_BTIPS_MMI_IND * para)
{
	MfwBtMainEvent event	= para->data.btlBtEvent;
	
	TRACE_FUNCTION_P1("mfw_btips_btlCallBack %d", event);

	switch (event)
	{
		case BTL_EVENT_RADIO_OFF_COMPLETE:
			mfw_btips_signal(E_BTIPS_POWEROFF_SUCCESS, NULL);
			break;
	
		case BTL_EVENT_RADIO_ON_COMPLETE:
			// Display Bluetooth Icon on the opening/default main window
			mfw_btips_signal(E_BTIPS_POWERON_SUCCESS, NULL);
			break;
	
	};	
}

BOOL mfw_btips_checkStatus(BtStatus btStatus)
{
	BOOL mfwBtStatus = 0;
	TRACE_EVENT_P1("mfw_btips_checkStatus : %s", pBT_Status(btStatus));
	switch(btStatus)
		{
		case BT_STATUS_SUCCESS:
		case BT_STATUS_PENDING:
		case BT_STATUS_IN_PROGRESS:
			mfwBtStatus = MFW_BTIPS_SUCCESS;
			break;

		case BTEVENT_HCI_FATAL_ERROR:
			mfwBtStatus = MFW_BTIPS_FAILED;
			TRACE_ERROR("mfw_btips_checkStatus - Fatal Error");
			mfw_btips_signal(E_BTIPS_FATAL_ERROR, NULL);		
			break;
			
		case BT_STATUS_INVALID_PARM:
			mfwBtStatus = MFW_BTIPS_FAILED;
			TRACE_ERROR("mfw_btips_checkStatus - Invalid Params");
			break;
		
		case BT_STATUS_DEVICE_NOT_FOUND:			
			mfwBtStatus = MFW_BTIPS_FAILED;
			TRACE_ERROR("mfw_btips_checkStatus - BT_STATUS_DEVICE_NOT_FOUND");
			break;
			
		case BT_STATUS_HCI_INIT_ERR:
			mfwBtStatus = MFW_BTIPS_FAILED;
			TRACE_ERROR("mfw_btips_checkStatus - BT_STATUS_HCI_INIT_ERR");
			mfw_btips_signal(E_BTIPS_FATAL_ERROR, NULL);		
			break;
			
		default:
			mfwBtStatus = MFW_BTIPS_SUCCESS; //For the time being
			break;
		}
		return mfwBtStatus;
}

static BOOL mfw_btips_createDemoRootDir(const BTHAL_U8 *fsPath)
{
    static char path[BTHAL_FS_MAX_PATH_LENGTH] = {'\0'};
    
    BtFsStatus status;
    BthalFsStat fileStat; 

    if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Stat((const BTHAL_U8 *)fsPath, &fileStat))
    {
       /* First, create the root directory */
        status = BTHAL_FS_Mkdir((const BTHAL_U8 *)fsPath);
        if ((status != BT_STATUS_HAL_FS_ERROR_EXISTS) && (status != BT_STATUS_HAL_FS_SUCCESS)) 
	 {
	 	Report(("Error: Unable to create Demo root dir %s\n", path)); 
	       return FALSE;
	 }
    
      
    }

    return TRUE;
}

#endif

