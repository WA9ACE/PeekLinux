/*******************************************************************************

					CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name:	Basic MMI
 $Project code:	BMI (6349)
 $Module:		Advice of Charge
 $File:		    MmiVoiceMemo.c
 $Revision:		1.0

 $Author:		Condat(UK)
 $Date:		    09/04/01

********************************************************************************

 Description:

    This provides the start code for the MMI

********************************************************************************
 $History: MmiAoc.c
	May 26, 2006 ER: OMAPS00079607 x0012849 :Jagannatha M
	Description: CPHS: Display 'charge/Timers' menu based on AOC ( Advice of charge )
	Solution:Made the following changes
		       1. Based on the value of Aoc_flag it will be decided to disply 'charge/Timers' menu or not.
		       
	June 16, 2005  REF: CRR 31267  x0021334
	Description: Handset ignore the initializtion of the PIN1/PIN2
	Fix:	Cheking is done to ascertain if PIN1/PIN2 are initialised. If not, appropriate
	       message is displayed to the user.
	
	09/04/01			Original Condat(UK) BMI version.
	May 25, 2004 REF: CRR MMI-SPR-17588		NISHIKANT KULKARNI	
	Issue description: On the SIM EF(ACMMax) is defined as 0xFFFFFF, so it should be possible to 
	enter 16777215, but only 999999 is accepted.
	Solution:  MAX_ENTER_LIMIT changed from 7 to 9. A check is done to verify that the entered 
	ACMmax value is less that 0xFFFFFF. Otherwise "Not Allowed" message is displayed.

 $End

*******************************************************************************/




/*******************************************************************************

                                Include files

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

#include "dspl.h"


#include "MmiMmi.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else /* NEW_EDITOR */
#include "MmiEditor.h"
#endif /* NEW_EDITOR */
#include "MmiLists.h"
#include "MmiIdle.h"
#include "MmiSoftKeys.h"
#include "Mmiicons.h"
#include "MmiMenu.h"
#include "MmiMain.h"
#include "MmiStart.h"
#include "MmiPins.h"

#include "mmiColours.h"

// June 16, 2005  REF: CRR 31267  x0021334
// Added this BOOL variable to keep track of PIN2 requests.
BOOL pin2Flag = FALSE;


/*******************************************************************************
	Local Data

*******************************************************************************/
#define	PIN_REQUEST_FOR_RATE			700
#define	SERVICES_ENTER_CURRENCY			701
#define	SERVICES_ONE_UNIT				702
#define	SERVICES_RATE_CHANGED			703
#define	SERVICES_CREDIT_REMAINING		704
#define	PIN_REQUEST_FOR_LIMIT_CHANGE	705
#define	PIN_REQUEST_FOR_LIMIT_CANCEL	706
#define	SERVICES_LIMIT_VIEW				707
#define	SERVICES_LIMIT_CHANGE			708
#define	SERVICES_LIMIT_CANCEL			709
#define	SERVICES_LIMIT_CHANGED			710
#define	SERVICES_LAST_CHARGE			711
#define	SERVICES_TOTAL_CHARGE			712
#define	SERVICES_RESET_CHARGE			713
#define	SERVICES_CHARGE_CHANGED			714
#define	SERVICES_OUT_OF_RANGE			715
#define	SERVICES_AOC_NOT_SUPPORTED		716
#define PIN_REQUEST_FOR_RESET_CHARGE  717

/*  May 25, 2004 REF: CRR MMI-SPR-17588		NISHIKANT KULKARNI
	MAX_ENTER_LIMIT changed from 7 to 9 to accept "Limit" values till 16777215 (0xFFFFFF) 
	Previously, with MAX_ENTER_LIMIT set to 7, maximum 999999 could be entered as limit.
	Effectively, the limit is MAX_ENTER_LIMIT - 1. The last character used for string termination.
	Also, the maximum value that can be entered (0xFFFFFF) is # defined to MAX_ENTER_VALUE
*/
/* Max. Enter of Character   */
#define MAX_ENTER_LIMIT      9	// xnkulkar SPR-17588
#define MAX_ENTER_CURRENCY   4
#define MAX_ENTER_UNIT       6
#define MAX_ENTER_VALUE		 0xFFFFFF	// xnkulkar SPR-17588
/*    */



typedef struct
{
	T_MMI_CONTROL			mmi_control;
	T_MFW_HND				service_win;        /* MFW win handler      */
	T_MFW_CM_AOC_INFO		aocInfo;
	USHORT					Identifier;
	char					edt_Buf[30]; 	  /* Editor buffer       */
} T_SERVICES;




LONG acm_last_call = 0; //accumulated call meter  from the last call
//later the value should save and read again from PCM

//x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
#ifdef FF_CPHS
static UBYTE Aoc_flag = FALSE;
#endif

/*******************************************************************************
	Local prototypes

*******************************************************************************/
T_MFW_HND	service_Init		(T_MFW_HND parent_window);
void		service_Exit		(T_MFW_HND own_window);
T_MFW_HND	service_create		(T_MFW_HND parent_window);
void		service_destroy		(T_MFW_HND own_window);
static int	service_win_cb		(T_MFW_EVENT event, T_MFW_WIN * win);
void		service_dialog_cb	(T_MFW_HND win, USHORT identifier, UBYTE reason);
void		service				(T_MFW_HND win, USHORT event, SHORT value, void * parameter);
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
void		service_loadEditDefault (T_AUI_EDITOR_DATA *editor_data);
#else /* NEW_EDITOR */
void		service_loadEditDefault (T_EDITOR_DATA *editor_data);
#endif /* NEW_EDITOR */
void		service_calc_balance   (T_MFW_HND win,char* line);
void		service_calculate	(T_MFW_HND win, char* line, long value);
USHORT		service_check_pin_for_aoc (void);
//ADDED BY RAVI-29-11-2005
extern T_MFW_HND	AUI_calc_Start(T_MFW_HND parent, T_AUI_EDITOR_DATA *editor_data);
// END RAVI

/*******************************************************************************
	Functions

*******************************************************************************/

/*******************************************************************************

 $Function:    	aoc_credit_remaining

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

int aoc_credit_remaining (MfwMnu* m, MfwMnuItem* i)
{
     T_MFW_HND win = mfw_parent(mfw_header());

	TRACE_FUNCTION("aoc_credit_remaining");

	// create new dynamic dialog for the AOC
    SEND_EVENT (service_Init (win), SERVICES_CREDIT_REMAINING, 0, 0);


	return 0;
}



/*******************************************************************************

 $Function:    	aoc_charge_rate

 $Description:	A security code editing screen is opened and Pin 2 requested.
				Once entered successfully, an alpha. editor is opened with the
				title "Enter currency".Once entered, an numeric editor is opened
				with the title "Enter price p. unit". When the user has finished
				entry the information screen " Rate changed" is displayed for
				3 second and the phone returns to the previous sub-menu
 $Returns:

 $Arguments:

*******************************************************************************/


int aoc_charge_rate (MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win = mfw_parent(mfw_header());

	TRACE_FUNCTION("aoc_charge_rate");

	switch (service_check_pin_for_aoc ())
	{
		case MFW_SIM_PIN2_REQ:
			// June 16, 2005  REF: CRR 31267  x0021334
			pin2Flag = TRUE;
			// create new dynamic dialog for the AOC
		       SEND_EVENT (service_Init (win), PIN_REQUEST_FOR_RATE, 0, 0);
			break;

		case MFW_SIM_PIN_REQ:
		case MFW_SIM_NO_PIN:
			//pin 1 or no pin required, open now the editor for entering the currency
			SEND_EVENT (service_Init (win),  SERVICES_ENTER_CURRENCY, 0, 0);
			break;

	}

	return 0;
}



/*******************************************************************************

 $Function:    	aoc_view_limit

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


int aoc_view_limit (MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win = mfw_parent(mfw_header());

	TRACE_FUNCTION("aoc_view_limit");/*SPR2500*/


	// create new dynamic dialog for the AOC
    SEND_EVENT (service_Init (win), SERVICES_LIMIT_VIEW, 0, 0);

	return 0;
}


/*******************************************************************************

 $Function:    	aoc_change_limit

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


int aoc_change_limit (MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win = mfw_parent(mfw_header());

	TRACE_FUNCTION("aoc_change_limit");

      /*SPR#1904 - DS - Added call to service_check_pin_for_aoc() to check if
       *PIN2 entry is required.
       */
	switch (service_check_pin_for_aoc ())
	{
		case MFW_SIM_PIN2_REQ:
			// June 16, 2005  REF: CRR 31267  x0021334
			pin2Flag = TRUE;
			// create new dynamic dialog for the AOC
		       SEND_EVENT (service_Init (win), PIN_REQUEST_FOR_LIMIT_CHANGE, 0, 0);
			break;

		case MFW_SIM_PIN_REQ:
		case MFW_SIM_NO_PIN:
			//pin 1 or no pin required, open now the editor for entering the currency
			SEND_EVENT (service_Init (win),  SERVICES_LIMIT_CHANGE, 0, 0);
			break;

	}

	return 0;
}


/*******************************************************************************

 $Function:    	aoc_cancel_limit

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


int aoc_cancel_limit (MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win = mfw_parent(mfw_header());

	TRACE_FUNCTION("aoc_charge_rate");

      /*SPR#1904 - DS - Added call to service_check_pin_for_aoc() to check if
       *PIN2 entry is required.
       */
	switch (service_check_pin_for_aoc ())
	{
		case MFW_SIM_PIN2_REQ:
			// June 16, 2005  REF: CRR 31267  x0021334
			pin2Flag = TRUE;
			// create new dynamic dialog for the AOC
		       SEND_EVENT (service_Init (win), PIN_REQUEST_FOR_LIMIT_CANCEL, 0, 0);
			break;

		case MFW_SIM_PIN_REQ:
		case MFW_SIM_NO_PIN:
			//pin 1 or no pin required, open now the editor for entering the currency
			SEND_EVENT (service_Init (win),  SERVICES_LIMIT_CANCEL, 0, 0);
			break;

	}

	return 0;
}

/*******************************************************************************

 $Function:    	aoc_last_charge

 $Description:	Dispays the charge for the last call in the currency or in units
                if no currency information has been entered

 $Returns:

 $Arguments:

*******************************************************************************/


int aoc_last_charge (MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win = mfw_parent(mfw_header());

		TRACE_FUNCTION("aoc_last_charge"); /*SPR2500*/

	// create new dynamic dialog for the AOC
    SEND_EVENT (service_Init (win), SERVICES_LAST_CHARGE, 0, 0);


	return 0;
}

/*******************************************************************************

 $Function:    	aoc_total_charge

 $Description:	Displays the charge for all calls since the chage counter was last reset

 $Returns:

 $Arguments:

*******************************************************************************/


int aoc_total_charge (MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win = mfw_parent(mfw_header());

		TRACE_FUNCTION("aoc_total_charge");/*SPR2500*/


	// create new dynamic dialog for the AOC
    SEND_EVENT (service_Init (win), SERVICES_TOTAL_CHARGE, 0, 0);


	return 0;
}
/*******************************************************************************

 $Function:    	aoc_reset_charge

 $Description:	Resets the charge counter for all calls

 $Returns:

 $Arguments:

*******************************************************************************/


int aoc_reset_charge (MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win = mfw_parent(mfw_header());

		TRACE_FUNCTION("aoc_reset_charge");/*SPR2500*/


  switch (service_check_pin_for_aoc ())
  {
    case MFW_SIM_PIN2_REQ:
	// June 16, 2005  REF: CRR 31267  x0021334
	pin2Flag = TRUE;
      // create new dynamic dialog for the AOC
      SEND_EVENT (service_Init (win), PIN_REQUEST_FOR_RESET_CHARGE, 0, 0);
      break;

    case MFW_SIM_PIN_REQ:
    case MFW_SIM_NO_PIN:
      //pin 1 or no pin required, open now the editor for entering the currency
    SEND_EVENT (service_Init (win), SERVICES_RESET_CHARGE, 0, 0);

      break;
  }

	return 0;
}






/*******************************************************************************

 $Function:    	service_Init

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
T_MFW_HND service_Init (T_MFW_HND parent_window)
{
  return (service_create (parent_window));
}


/*******************************************************************************

 $Function:    	service_Exit

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
void service_Exit (T_MFW_HND own_window)
{

    T_MFW_WIN   * win_data     = ((T_MFW_HDR *)own_window)->data;
    T_SERVICES     * data         = (T_SERVICES *)win_data->user;

  	TRACE_FUNCTION("service_Exit");
	/*SPR 2500, check valid window pointer before destruction*/
	if ((own_window) &&
	     (win_data) &&
	     (data) &&
	     (data->service_win))
	{
		service_destroy (data->service_win);
	}
	else
	{
		TRACE_ERROR("service_Exit : Invalid Pointer passed");
	}

	return;

}

/*******************************************************************************

 $Function:    	service_create

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
T_MFW_HND service_create (T_MFW_HND parent_window)
{
  T_SERVICES *  data = (T_SERVICES *)ALLOC_MEMORY (sizeof (T_SERVICES));
  T_MFW_WIN   * win;


  TRACE_FUNCTION ("service_create()");

  data->service_win = win_create (parent_window, 0, 0, (T_MFW_CB)service_win_cb);

  if (data->service_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  data->mmi_control.dialog =  (T_DIALOG_FUNC)service;
  data->mmi_control.data   = data;
  win                      = ((T_MFW_HDR *)data->service_win)->data;
  win->user                = (void *) data;


  winShow(data->service_win);
  /*
   * return window handle
   */
  return data->service_win;
}

/*******************************************************************************

 $Function:    	service_destroy

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
void service_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN   * win;
  T_SERVICES * data;

  TRACE_FUNCTION ("service_destroy()");

  if (own_window)
  {
    win  = ((T_MFW_HDR *)own_window)->data;
    data = (T_SERVICES *)win->user;

    if (data)
    {

      /*
       * Delete WIN handler
       */
      win_delete (data->service_win);
      data->service_win = 0;

	  FREE_MEMORY ((void *)data, sizeof (T_SERVICES));

    }
  }
}
/*******************************************************************************

 $Function:    	service_win_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
static int service_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
  /*
   * Top Window has no output
   */
  return 1;
}

/*******************************************************************************

 $Function:    	service_dialog_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
void service_dialog_cb(T_MFW_HND win, USHORT identifier, UBYTE reason)
{
	TRACE_FUNCTION ("service_dialog_cb()");

      switch (reason)
      {
        case INFO_KCD_LEFT:
          /* no break; */
        case INFO_TIMEOUT:
          /* no break; */
        case INFO_KCD_HUP:
          /* no break; */
        case INFO_KCD_RIGHT:
          /* no break; */
        case INFO_KCD_CLEAR:

			TRACE_EVENT_P1("identifier: %d", identifier);

			if (identifier EQ SERVICES_OUT_OF_RANGE)
			{
				TRACE_EVENT ("SERVICES_OUT_OF_RANGE");
				SEND_EVENT (win, SERVICES_ONE_UNIT, 0, 0);
			}
			else
			{
				TRACE_EVENT (" destroy ");
	            service_Exit(win);
			}
          break;
      }
}

/*******************************************************************************

 $Function:    	service_editor_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

static void service_editor_cb (T_MFW_HND win, USHORT Identifier, SHORT reason)
{
	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	T_SERVICES    * data     = (T_SERVICES *)win_data->user;


	UBYTE		PWD[16], i;
    int result;

	TRACE_FUNCTION ("service_editor_cb()");
	/*SPR 2500, check pointers valid*/
	if ((!win) ||
	     (!win_data) ||
	     (!data))
	{
		TRACE_ERROR("service_editor_cb : Invalid Pointer passed");
		return;
	}

	if ((!win) ||
	     (!win_data) ||
	     (!data))
	{
		TRACE_ERROR("service_editor_cb : Invalid Pointer passed");
		return;
	}

      memset(PWD, 0x00, 16);

	switch (reason)
	{
		case INFO_KCD_LEFT:
			switch (Identifier)
			{
				case SERVICES_ENTER_CURRENCY:
					TRACE_EVENT ("SERVICES_ENTER_CURRENCY");
					TRACE_EVENT ("pressed left");


                	// clear old current currency before overwrite it
                    memset(data->aocInfo.cur, 0x80, sizeof(data->aocInfo.cur));

                    if(strlen(data->edt_Buf) NEQ 0)
                    {
                        memcpy(data->aocInfo.cur, data->edt_Buf, MAX_ENTER_CURRENCY);
                    }

 					//termination from ASCII to GSM default
					for (i=0; i< MAX_ENTER_CURRENCY-1; i++)
					{
						if (data->aocInfo.cur[i] EQ '\0')
						data->aocInfo.cur[i] = 0x80;
					}


					// open the next editor for entering the price per unit
				    SEND_EVENT (win, SERVICES_ONE_UNIT, 0, 0);
					break;

				case SERVICES_ONE_UNIT:
					TRACE_EVENT ("SERVICES_ONE_UNIT");
					TRACE_EVENT ("pressed left");

                    // clear old current ppu before overwrite it
                    memset(data->aocInfo.ppu, '\0', sizeof(data->aocInfo.ppu));

	                if(strlen(data->edt_Buf) NEQ 0)
                    {
                        memcpy(data->aocInfo.ppu, data->edt_Buf, strlen(data->edt_Buf));
	                }


                    // Password not necessary, Pin2 scenario done
                                   memset(PWD, 0x00, 16);

     				// write cur and ppu in AOC
					result = cm_set_aoc_value(CM_AOC_PUCT, &data->aocInfo, PWD);


					TRACE_EVENT_P1("PPU : %s", data->aocInfo.ppu);
					TRACE_EVENT_P1("CUR : %s", data->aocInfo.cur);
					TRACE_EVENT_P1("result : %d", result);

					if(result EQ CM_OK)
					{
						SEND_EVENT (win, SERVICES_RATE_CHANGED, 0, 0);
					}
					else
					{
						SEND_EVENT (win, SERVICES_OUT_OF_RANGE, 0, 0);
					}

					break;

				case SERVICES_LIMIT_CHANGE:

					// string => long
                    data->aocInfo.acm_max = atol(data->edt_Buf);

					// Password not necessary, Pin2 scenario done
                    memset(PWD, 0x00, 16);

					//  May 25, 2004 REF: CRR MMI-SPR-17588		NISHIKANT KULKARNI

					//  If the ACMmax value entered by the user is more than 16777215 (0xFFFFFF), 
					//	display a "Not Allowed" message. 			
					if(data->aocInfo.acm_max > MAX_ENTER_VALUE)
					{
						// Send event to display a "Not Allowed" message.
						SEND_EVENT (win, SERVICES_AOC_NOT_SUPPORTED, 0, 0);
					}

					// Else set the ACMmax value in AoC.	
					else
					{
						result = cm_set_aoc_value(CM_AOC_ACMMAX, &data->aocInfo, PWD); /* save ACMmax in AOC */

						TRACE_EVENT_P1("%ld", data->aocInfo.acm_max);
						TRACE_EVENT_P1("result of set aoc %d", result);

						// Send event to display a "Limit Changed" message.
						SEND_EVENT (win, SERVICES_LIMIT_CHANGED, 0, 0);
					}

					break;
        case SERVICES_RESET_CHARGE:
			/* MC 12/04/02: Changed ACMMAX to ACM (should fix "broken" SIM problem)*/

					result = cm_set_aoc_value(CM_AOC_ACM, &data->aocInfo, PWD);	/* save ACMmax in AOC */

			}
			break;

	    case INFO_KCD_RIGHT:
	    case INFO_KCD_CLEAR:

			TRACE_EVENT ("INFO_KCD_RIGHT pressed");

			switch (Identifier)
			{

				case SERVICES_LIMIT_CHANGE:
				case SERVICES_ENTER_CURRENCY:

					//go back to the previous screen
					service_Exit (win);
					break;
				case SERVICES_ONE_UNIT:

					//go back to the previous screen
					SEND_EVENT (win, SERVICES_ENTER_CURRENCY, 0, 0);
					break;


				default:
					break;
			}
    default:
			break;
	}
}


/*******************************************************************************

 $Function:    	createAOCDialog

 $Description:	Creates a dialog from 4 input parameters

 $Returns:

 $Arguments:

*******************************************************************************/
static void createAOCDialog( T_MFW_HND win, USHORT txtId1, USHORT txtId2, char *str2, USHORT event )
{
  	T_DISPLAY_DATA display_info;

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, txtId1, txtId2, COLOUR_AOC);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)service_dialog_cb, THREE_SECS, KEY_CLEAR | KEY_RIGHT | KEY_LEFT );
	display_info.Identifier			= event;
    display_info.TextString2    	= str2;
	info_dialog (win, &display_info);
}
/*******************************************************************************

 $Function:    	service

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
void service (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN    * win_data = ((T_MFW_HDR *) win)->data;
	T_SERVICES    * data     = (T_SERVICES *)win_data->user;

        /* CQ13428 - NDH - Use dynamically allocated memory to prevent MMI Stack Overflow */
#ifdef NEW_EDITOR
        /* SPR#1428 - SH - New Editor data */
	T_AUI_EDITOR_DATA *editor_data = (T_AUI_EDITOR_DATA *)ALLOC_MEMORY (sizeof (T_AUI_EDITOR_DATA));
#else
	T_EDITOR_DATA	*editor_data = (T_EDITOR_DATA *)ALLOC_MEMORY (sizeof (T_EDITOR_DATA));
#endif
  //	T_MFW_SIM_PIN_STATUS status;  // RAVI

	UBYTE					i, result;
       UBYTE                                PWD[16];
    static char line[MAX_LINE];

	TRACE_FUNCTION("service()");

       if (!editor_data)
        {
            TRACE_ERROR("service : Failed to allocate memory for the Editor Data");
            return;
        }
	if ((!win) ||
	     (!win_data) ||
	     (!data))
	{
		TRACE_ERROR("service : Invalid Pointer passed");
              /* CQ13428 - NDH - Use dynamically allocated memory to prevent MMI Stack Overflow */
#ifdef NEW_EDITOR
              FREE_MEMORY((void *)editor_data, sizeof (T_AUI_EDITOR_DATA));
#else
              FREE_MEMORY((void *)editor_data, sizeof (T_EDITOR_DATA));
#endif
		return;
	}

      memset(PWD, 0x00, 16);
	switch (event)
	{

		case SERVICES_CREDIT_REMAINING:

            memset(line,'\0',sizeof(line));

			//Get the ACM MAX
            cm_get_aoc_value(CM_AOC_ACMMAX,&data->aocInfo);

			//Get ACM
            cm_get_aoc_value(CM_AOC_ACM,&data->aocInfo);

			//Get currency and price per unit
            cm_get_aoc_value(CM_AOC_PUCT,&data->aocInfo);


			TRACE_EVENT_P1("ACMMAX: %ld", data->aocInfo.acm_max);
			TRACE_EVENT_P1("ACM: %ld", data->aocInfo.acm);
			TRACE_EVENT_P1("PPU:%s", data->aocInfo.ppu);
			TRACE_EVENT_P1("CUR:%s", data->aocInfo.cur);


            if ((data->aocInfo.acm_max NEQ '\0') && (data->aocInfo.acm NEQ '\0'))
            {
                /* special case: It is not possible to have ACM > ACMMAX   */
                /* this case the mobile has to disconnect */
                if(data->aocInfo.acm > data->aocInfo.acm_max)
                {
                    strcpy(line, "????");
                }
                else
                {
                    /* calculate the credit with ACMAX and PPU  */
                    TRACE_EVENT ("CREDIT :ACMMAX !=0 and PPU !=0");
                    service_calc_balance (win,(char*)line);

                }
            }
            if ((data->aocInfo.acm_max EQ '\0') && (data->aocInfo.acm NEQ '\0'))
            {
                /* calculate the cost with ACM and PPU  */
                TRACE_EVENT ("CREDIT :ACMMAX ==0 and PPU !=0");
                service_calculate (win, (char*)line, data->aocInfo.acm);

            }
            if ((data->aocInfo.acm_max EQ '\0') && (data->aocInfo.acm NEQ '\0') )
            {
                /* show only ACM */
                TRACE_EVENT ("CREDIT :ACMMAX ==0 and PPU ==0");
                sprintf(line, "%ld", data->aocInfo.acm); /*   */
            }
             if ((data->aocInfo.acm_max NEQ '\0') && (data->aocInfo.acm NEQ '\0'))
            {
                /*  show only ACMAX  */
                TRACE_EVENT ("CREDIT :ACMMAX !=0 and PPU ==0");
                sprintf(line, "%ld", data->aocInfo.acm_max - data->aocInfo.acm); /*   */
            }

			if (data->aocInfo.acm EQ '\0')
           		strcpy ((char *)line, "0");

			//from ASCII to GSM default
			for (i=0; i< MAX_LINE; i++)
			{
				if (line[i] EQ (char)0x80 OR line[i] EQ (char)0xFF )   /* x0039928 - Lint warning fix */
					line[i] = '\0';
			}


            if (data->aocInfo.acm_max EQ 0)
            {
				createAOCDialog( win,  TxtCost, TxtNull, line, event);
            }
            else
            {
				createAOCDialog( win,  TxtBalance, TxtNull, line, event);
            }
			break;




		case PIN_REQUEST_FOR_RATE:
			//save the status
		    data->Identifier = SERVICES_ENTER_CURRENCY;

			//PIN2 is requested to change the Rate
            pin2_check (data->service_win);

			break;

		case PIN_REQUEST_FOR_LIMIT_CHANGE:

			//PIN2 is requested to change the Rate
            pin2_check (data->service_win);

			//save the status
		    data->Identifier = SERVICES_LIMIT_CHANGE;
			break;

		case PIN_REQUEST_FOR_LIMIT_CANCEL:

			//PIN2 is requested to change the Rate
            pin2_check (data->service_win);

			//save the status
		    data->Identifier = SERVICES_LIMIT_CANCEL;
			break;

    case PIN_REQUEST_FOR_RESET_CHARGE:
      //save the status
        data->Identifier = SERVICES_RESET_CHARGE;

      //PIN2 is requested to change the Rate
            pin2_check (data->service_win);

      break;


		//it calls from MMI PINS
        case PIN2_OK:
			TRACE_EVENT("PIN2_OK");

			switch (data->Identifier)
			{
				case SERVICES_ENTER_CURRENCY:

						//pin was ok, open now the editor for entering the currency
						SEND_EVENT (win, data->Identifier, 0, 0);
					break;
				case SERVICES_LIMIT_CHANGE:

						//pin was ok, open now the editor for entering the currency
						SEND_EVENT (win, data->Identifier, 0, 0);
					break;
				case SERVICES_LIMIT_CANCEL:

						//pin was ok, open now the editor for entering the currency
						SEND_EVENT (win, data->Identifier, 0, 0);
					break;
        case SERVICES_RESET_CHARGE:

            //pin was ok, open now the editor for entering the currency
            SEND_EVENT (win, data->Identifier, 0, 0);
          break;



			}
			break;

        case  PIN2_ABORT:
			createAOCDialog( win, TxtFailed, TxtNull, line, event );
		    break;

        case SERVICES_ENTER_CURRENCY:


        	//save the status
		    data->Identifier = SERVICES_ENTER_CURRENCY;


			//PUCT (currency and price per unit) is requested
	        result = cm_get_aoc_value(CM_AOC_PUCT, &data->aocInfo);

			TRACE_EVENT_P1("result %d", result);

			//fill up editor-sttribut with default
			service_loadEditDefault (editor_data);

			//clear the editor-buffer
			memset(data->edt_Buf,'\0',sizeof(data->edt_Buf));

			// let to show the current currency, before the user change it */
		    memcpy(data->edt_Buf, data->aocInfo.cur, MAX_ENTER_CURRENCY);


			for (i=0; i< MAX_ENTER_CURRENCY-1; i++)
			{
				if (data->edt_Buf[i] EQ (char)0x80 OR data->edt_Buf[i] EQ (char)0xFF )
					data->edt_Buf[i] = '\0';
			}

/* SPR#1428 - SH - New Editor changes */

#ifdef NEW_EDITOR
			AUI_edit_SetBuffer(editor_data, ATB_DCS_ASCII, (UBYTE *)data->edt_Buf, MAX_ENTER_CURRENCY);
			AUI_edit_SetTextStr(editor_data, TxtSoftOK, TxtDelete, TxtEnterCurrency, NULL);
			AUI_edit_SetEvents(editor_data, data->Identifier, TRUE, FOREVER, (T_AUI_EDIT_CB)service_editor_cb);
			AUI_edit_SetMode(editor_data, ED_MODE_ALPHA, ED_CURSOR_BAR);

			AUI_edit_Start(data->service_win, editor_data);
#else /* NEW_EDITOR */
			editor_data->editor_attr.text	= data->edt_Buf;  /* buffer to be edited */
			editor_data->editor_attr.size	= MAX_ENTER_CURRENCY;/* limit to 3 letters */
			editor_data->LeftSoftKey			= TxtSoftOK;
			editor_data->RightSoftKey		= TxtDelete;
			editor_data->TextId				= TxtEnterCurrency;
			editor_data->Identifier			= data->Identifier;
			editor_data->mode				= ALPHA_MODE;
			editor_data->AlternateLeftSoftKey     = TxtNull;
			editor_data->min_enter		    = 1;

			/* create the dialog handler */
			editor_start(data->service_win, editor_data);  /* start the editor */
#endif /* NEW_EDITOR */
			break;



        case SERVICES_ONE_UNIT:

			//save the status
		    data->Identifier = SERVICES_ONE_UNIT;

			//fill up editor-sttribut with default
			service_loadEditDefault (editor_data);

			//clear the editor-buffer
			memset(data->edt_Buf,'\0',sizeof(data->edt_Buf));

           /* let to show the current price per unit, before the user change it */
            memcpy(data->edt_Buf, data->aocInfo.ppu, sizeof(data->aocInfo.ppu));

			for (i=0; i< MAX_ENTER_UNIT-1; i++)
			{
				if (data->edt_Buf[i] EQ (char)0x80 OR data->edt_Buf[i] EQ (char)0xFF )
					data->edt_Buf[i] = '\0';
			}

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_SetBuffer(editor_data, ATB_DCS_ASCII, (UBYTE *)data->edt_Buf, MAX_ENTER_UNIT);
			AUI_edit_SetTextStr(editor_data, TxtSoftOK, TxtDelete, Txt1Unit, NULL);
			AUI_edit_SetEvents(editor_data, data->Identifier, TRUE, FOREVER, (T_AUI_EDIT_CB)service_editor_cb);
			AUI_edit_SetMode(editor_data, 0, ED_CURSOR_BAR);

			AUI_calc_Start(data->service_win, editor_data);
#else /* NEW_EDITOR */
			editor_data->editor_attr.text	= data->edt_Buf;  /* buffer to be edited */
			editor_data->editor_attr.size	= MAX_ENTER_UNIT;/* limit to 5 numerics */
			editor_data->LeftSoftKey			= TxtSoftOK;
			editor_data->RightSoftKey		= TxtDelete;
			editor_data->TextId				= Txt1Unit;
			editor_data->Identifier			= data->Identifier;
			editor_data->mode				= CALC_MODE;
			editor_data->AlternateLeftSoftKey     = TxtNull;
			editor_data->min_enter		    = 1;

			/* create the dialog handler */
			editor_start(data->service_win, editor_data);  /* start the editor */
#endif /* NEW_EDITOR */
			break;


		case SERVICES_RATE_CHANGED:
			createAOCDialog( win,  TxtRate, TxtChanged, NULL, event);
			break;

		case SERVICES_LIMIT_VIEW:

            memset(line,'\0',sizeof(line));

            cm_get_aoc_value(CM_AOC_ACMMAX,&data->aocInfo);  /* Get the ACM MAX     */
            cm_get_aoc_value(CM_AOC_PUCT,&data->aocInfo);    /* Get currency and price per unit */

            TRACE_EVENT_P1("ACMMAX: %ld", data->aocInfo.acm_max);
            TRACE_EVENT_P1("ACM: %ld", data->aocInfo.acm);
            TRACE_EVENT_P1("PPU:%s", data->aocInfo.ppu);
            TRACE_EVENT_P1("CUR:%s", data->aocInfo.cur);

            if((strlen((char*)data->aocInfo.ppu) EQ 0) && data->aocInfo.acm_max NEQ '\0')
            {
                sprintf(line, "%ld", data->aocInfo.acm_max);     /* Show only ACMmax  */
	            TRACE_EVENT ("get only :ACMMAX ; PPU ==0 and ACMMAX!=0");
            }
            if((strlen((char*)data->aocInfo.ppu) NEQ 0) && data->aocInfo.acm_max NEQ '\0')
            {
				service_calculate (win, (char*)line, data->aocInfo.acm_max);

                TRACE_EVENT ("calculate ACMMAX * PPU; PPU !=0 and ACMMAX!=0");
            }

			for (i=0; i< MAX_LINE; i++)
			{
				if (line[i] EQ (char)0x80 OR line[i] EQ (char)0xFF )
					line[i] = '\0';
			}

            if(data->aocInfo.acm_max NEQ '\0')
            {
				createAOCDialog( win,  TxtLimitEq, TxtNull, line, event);
            }
            else
            {
				createAOCDialog( win,  TxtNo, TxtLimit, NULL, event);
            }

			  /*
			   * Call Info Screen
			   */

			break;

		case SERVICES_LIMIT_CHANGE:

			//fill up editor-sttribut with default
			service_loadEditDefault (editor_data);

			//clear the editor-buffer
			memset(data->edt_Buf,'\0',sizeof(data->edt_Buf));

			data->Identifier = SERVICES_LIMIT_CHANGE; /*SPR 2500*/


			data->Identifier = SERVICES_LIMIT_CHANGE;

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR

			AUI_edit_SetBuffer(editor_data, ATB_DCS_ASCII, (UBYTE *)data->edt_Buf, MAX_ENTER_LIMIT);
			AUI_edit_SetTextStr(editor_data, TxtSoftOK, TxtDelete, TxtEnterLimit, NULL);
			AUI_edit_SetEvents(editor_data, data->Identifier, TRUE, FOREVER, (T_AUI_EDIT_CB)service_editor_cb);
			AUI_edit_SetMode(editor_data, 0, ED_CURSOR_BAR);

			AUI_calc_Start(data->service_win, editor_data);
#else /* NEW_EDITOR */
			editor_data->editor_attr.text	= data->edt_Buf;  /* buffer to be edited */
			editor_data->editor_attr.size	= MAX_ENTER_LIMIT; /* limit to 6 terms of units */
			editor_data->LeftSoftKey			= TxtSoftOK;
			editor_data->RightSoftKey		= TxtDelete;
			editor_data->TextId				= TxtEnterLimit;
			editor_data->Identifier			= data->Identifier;
			editor_data->mode				= CALC_MODE;
			editor_data->AlternateLeftSoftKey     = TxtNull;
			editor_data->min_enter		    = 1;

			/* create the dialog handler */
			editor_start(data->service_win, editor_data);  /* start the editor */
#endif /* NEW_EDITOR */

			break;
		case SERVICES_LIMIT_CANCEL:

					// clear the limit
                    data->aocInfo.acm_max = 0;

					// Password not necessary, Pin2 scenario done
                    memset(PWD, 0x00, 16);

			cm_set_aoc_value(CM_AOC_ACMMAX, &data->aocInfo, PWD);	/* save ACMmax in AOC */

			TRACE_EVENT_P1("%ld", data->aocInfo.acm_max);

                    SEND_EVENT (win, SERVICES_LIMIT_CHANGED, 0, 0);

			break;

		case SERVICES_LIMIT_CHANGED:
			createAOCDialog( win,  TxtLimit, TxtChanged, NULL, event);

			break;

		case SERVICES_LAST_CHARGE:

            cm_get_aoc_value(CM_AOC_PUCT,&data->aocInfo);    /* Get currency and price per unit */
            cm_get_aoc_value(CM_AOC_PUCT,&data->aocInfo);    /* Get currency and price per unit */

            if ( strlen((char*)data->aocInfo.ppu) EQ 0 )
            {
                TRACE_EVENT ("get only :ACM ; PPU ==0 and ACM!=0");

                /* show only the units ACM from last call */
                sprintf(line, "%ld", acm_last_call);
            }
            else
            {
                TRACE_EVENT ("get only :ACM ; PPU !=0 and ACM!=0");

                /* add currency */
                sprintf(line, "%ld %s", acm_last_call, data->aocInfo.cur);
            }

            if (acm_last_call EQ '\0')
               strcpy ((char *)line, "0");

			createAOCDialog( win,  TxtLastCharge, TxtNull, line, event);
            break;

		case SERVICES_TOTAL_CHARGE:

            memset(line,'\0',sizeof(line));

            cm_get_aoc_value(CM_AOC_ACM,&data->aocInfo);  /* Get the ACM      */
            cm_get_aoc_value(CM_AOC_PUCT,&data->aocInfo);    /* Get currency and price per unit */


		    /* nm test only  set the default  */

			TRACE_EVENT_P1("ACM: %ld", data->aocInfo.acm);
			TRACE_EVENT_P1("PPU:%s", data->aocInfo.ppu);
			TRACE_EVENT_P1("CUR:%s", data->aocInfo.cur);


            if((strlen((char*)data->aocInfo.ppu) EQ 0) && data->aocInfo.acm NEQ '\0')
            {
				//show only the units ACM
                sprintf(line, "%ld", data->aocInfo.acm);
                TRACE_EVENT ("get only :ACM ; PPU ==0 and ACM!=0");

            }
            if((strlen((char*)data->aocInfo.ppu) NEQ 0) && data->aocInfo.acm NEQ '\0')
            {

                /* Ref bug omaps00076109 */
                #ifdef NEPTUNE_BOARD
                    sprintf(line, "%ld %s", data->aocInfo.acm, data->aocInfo.cur);
                #else
                    service_calculate (win, (char*)line, data->aocInfo.acm);
                #endif
                
                TRACE_EVENT ("calculate ACM * PPU; PPU !=0 and ACM!=0");
            }

			if (data->aocInfo.acm EQ '\0')
           		strcpy ((char *)line, "0");

			createAOCDialog( win,  TxtTotalCharge, TxtNull, line, event);

			break;

		case SERVICES_RESET_CHARGE:

			// clear the charge counter for all calls
            data->aocInfo.acm = 0;
			acm_last_call	 = 0;

			(void)cm_set_aoc_value(CM_AOC_ACM, &data->aocInfo, PWD);		/* save ACM in AOC */

			TRACE_EVENT_P1("%ld", data->aocInfo.acm);

            SEND_EVENT (win, SERVICES_CHARGE_CHANGED, 0, 0);

			break;

		case SERVICES_CHARGE_CHANGED:
			createAOCDialog( win,  TxtResetCharge, TxtNull, NULL, event);
			break;
		case SERVICES_OUT_OF_RANGE:

				TRACE_EVENT("SERVICES_OUT_OF_RANGE");
			createAOCDialog( win,  TxtOutofRange, TxtNull, NULL, event);
 			break;

		case SERVICES_AOC_NOT_SUPPORTED:
				TRACE_EVENT("SERVICES_NOT_ALLOWED");
			createAOCDialog( win,  TxtNotAllowed, TxtNull, NULL, event);
 			break;


	}

     /* CQ13428 - NDH - Use dynamically allocated memory to prevent MMI Stack Overflow */
#ifdef NEW_EDITOR
     FREE_MEMORY((void *)editor_data, sizeof (T_AUI_EDITOR_DATA));
#else
     FREE_MEMORY((void *)editor_data, sizeof (T_EDITOR_DATA));
#endif
     return;
}


/*******************************************************************************

 $Function:    	service_loadEditDefault

 $Description:	fill up editor-sttribut with default

 $Returns:

 $Arguments:

*******************************************************************************/
#ifdef NEW_EDITOR
/* SPR#1428 - SH - New Editor changes */
void service_loadEditDefault (T_AUI_EDITOR_DATA *editor_data)
{
		TRACE_FUNCTION ("service_loadEditDefault()");

		AUI_edit_SetDefault(editor_data);
		AUI_edit_SetDisplay(editor_data, ZONE_FULL_SK_TITLE, COLOUR_EDITOR, EDITOR_FONT);
		AUI_edit_SetEvents(editor_data, 0, TRUE, FOREVER, (T_AUI_EDIT_CB)service_editor_cb);
		AUI_edit_SetTextStr(editor_data, TxtSoftSelect, TxtSoftBack, TxtNull, NULL);
		AUI_edit_SetMode(editor_data, 0, ED_CURSOR_UNDERLINE);
		AUI_edit_SetAltTextStr(editor_data, 1, TxtNull, FALSE, TxtNull);
		return;
}
#else /* NEW_EDITOR */
void service_loadEditDefault (T_EDITOR_DATA *editor_data)
{
		TRACE_FUNCTION ("service_loadEditDefault()");

		editor_attr_init(&editor_data->editor_attr, ZONE_SMALL_EDITOR, edtCurBar1, NULL, NULL, 0, COLOUR_EDITOR);
	    editor_data->editor_attr.font     = 0;

		editor_data_init( editor_data, (T_EDIT_CB)service_editor_cb, TxtSoftSelect, TxtSoftBack, TxtNull, 1, DIGITS_MODE, FOREVER);
		editor_data->hide			    = FALSE;
		editor_data->destroyEditor	    = TRUE;
		editor_data->AlternateLeftSoftKey   = TxtNull;
}
#endif /* NEW_EDITOR */

/*******************************************************************************

 $Function:    	service_calc_balance

 $Description:	Calculate the Credit remaining

 $Returns:

 $Arguments:

*******************************************************************************/
void service_calc_balance (T_MFW_HND win,char* line)
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
	T_SERVICES    * data     = (T_SERVICES *)win_data->user;


    char line2[MAX_LINE];
    char *pdest,*src,*dst,*POINT;
    LONG result;
    int  ch = '.';
    UBYTE counter=0;
	if ((!win) ||
	     (!win_data) ||
	     (!data))
	{
		TRACE_ERROR("service_calc_balance : Invalid Pointer passed");
		return;
	}

    /*SPR 2500, chack validity of pointers before continuing*/
	if ((!win) ||
	     (!win_data) ||
	     (!data))
	{
		TRACE_ERROR("service_calc_balance : Invalid Pointer passed");
		return;
	}

    memset(line2,'\0',sizeof(line2));

    cm_get_aoc_value(CM_AOC_ACMMAX,&data->aocInfo);  /* Get the ACM MAX     */
    cm_get_aoc_value(CM_AOC_ACM,&data->aocInfo);     /* Get ACM  */
    cm_get_aoc_value(CM_AOC_PUCT,&data->aocInfo);    /* Get currency and price per unit */



    /* Search for '.' */
    pdest =(char *) strchr((char*) data->aocInfo.ppu, ch );

    if( pdest EQ NULL )                              /* '.' not founded  */
    {
        result = atol((char*)data->aocInfo.ppu );                /* STRING => LONG   */
        result= result *(data->aocInfo.acm_max - data->aocInfo.acm); /* (ACMmax-ACM) * PPU */
        sprintf(line, "%ld %s", result,data->aocInfo.cur); /* LONG => STRING   */
    }
    else                                        /* '.' found  */
    {
    /*
    * '.' found, *pdest EQ '.'
        */
        counter = strlen(pdest + 1); /* store the decimal place */

        src = (char*)data->aocInfo.ppu;
        dst = line;
        do                           /* remove the '.'   */
        {
            if (*src NEQ '.')
            {
                *dst++ = *src;
            }
        } while (*src++ NEQ '\0');
        /* STRING => LONG   */
        result= (atol(line))*(data->aocInfo.acm_max - data->aocInfo.acm); /* (ACMmax - ACM) * PPU */
        sprintf(line, "%ld", result);        /* LONG => STRING   */

        if(strlen(line) < counter)
        {
            src = line;
            dst = line2;
            *dst++ = '0';
            *dst++ = '.';
            counter = counter - strlen(line);
            do                               /* fill up with '0'   */
            {
                *dst++ = '0';
            } while (--counter NEQ 0);

            memcpy (dst,src,sizeof(src));
            sprintf(line, "%s %s", line2,data->aocInfo.cur);     /* add the currency */
            return;
        }
        src = line;
        dst = line2;
        POINT= src + strlen(src) - counter;
        do                                  /* set the '.'   */
        {
            if (src EQ POINT)
            {
                *dst++ = '.';
            }
            *dst++ = *src++;

        } while (*src NEQ '\0');

        if (line2[0] EQ '.')
            sprintf(line, "0%s %s", line2,data->aocInfo.cur);     /* add the currency */

        if (line2[0] NEQ '.')
            sprintf(line, "%s %s", line2,data->aocInfo.cur);     /* add the currency */

    }
    return;
}


/*******************************************************************************

 $Function:    	service_calculate

 $Description:	Calculate the value with price per unit and currency.

 				"value * PPU" @ currency

 $Returns:

 $Arguments:

*******************************************************************************/


void service_calculate (T_MFW_HND win, char* line, long value)
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
	T_SERVICES      * data     = (T_SERVICES *)win_data->user;

    char line2[MAX_LINE];
    char *pdest,*src,*dst,*POINT;
    LONG result;
    int  ch = '.';
    UBYTE counter=0;
   
    /*SPR2500, check validity of pinters before continuing*/
	if ((!win) ||
	     (!win_data) ||
	     (!data))
	{
		TRACE_ERROR("service_calculate : Invalid Pointer passed");
		return;
	}

    memset(line2,'\0',sizeof(line2));

    cm_get_aoc_value(CM_AOC_ACM,&data->aocInfo);     /* Get ACM  */
    cm_get_aoc_value(CM_AOC_PUCT,&data->aocInfo);    /* Get currency and price per unit */


    /* Search for '.' */
    pdest =(char *) strchr((char*) data->aocInfo.ppu, ch );
    if( pdest EQ NULL )                              /* '.' not founded  */
    {
        result = atol((char*)data->aocInfo.ppu );                /* STRING => LONG   */
        result= result * value;            /* value * PPU */
        sprintf(line, "%ld %s", result,data->aocInfo.cur); /* LONG => STRING   */
    }
    else                                        /* '.' found  */
    {
        /*
         * '.' found, *pdest EQ '.'
         */
        counter = strlen(pdest + 1); /* store the decimal place */

        src = (char*)data->aocInfo.ppu;
        dst = line;
        do                           /* remove the '.'   */
        {
            if (*src NEQ '.')
            {
                *dst++ = *src;
            }
        } while (*src++ NEQ '\0');

        /* STRING => LONG   */
        result= atol(line) * value;        /* value * PPU     */
        sprintf(line, "%ld", result);        /* LONG => STRING   */

        if(strlen(line) < counter)
        {
            src = line;
            dst = line2;
            *dst++ = '0';
            *dst++ = '.';
            counter = counter - strlen(line);
            do                               /* fill up with '0'   */
            {
                *dst++ = '0';
            } while (--counter NEQ 0);

            memcpy (dst,src,sizeof(src));
            sprintf(line, "%s %s", line2,data->aocInfo.cur);     /* add the currency */
            return;
        }
        src = line;
        dst = line2;
        POINT= src + strlen(src) - counter;
        do                                  /* set the '.'   */
        {
            if (src EQ POINT)
            {
                *dst++ = '.';
            }
            *dst++ = *src++;

        } while (*src NEQ '\0');

        if (line2[0] EQ '.')
            sprintf(line, "0%s %s", line2,data->aocInfo.cur);     /* add the currency */

        if (line2[0] NEQ '.')
            sprintf(line, "%s %s", line2,data->aocInfo.cur);     /* add the currency */

    }
    return;
}

//x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
#ifdef FF_CPHS
/*******************************************************************************

 $Function:    	setAOCflag

 $Description: set the value to Aoc_flag variable

 $Returns:

 $Arguments: TRUE or FALSE

*******************************************************************************/
void setAOCflag(UBYTE aocValue)
{
    Aoc_flag = aocValue;
}


/*******************************************************************************

 $Function:    	getAOCflag

 $Description: Rturns the value of variable Aoc_flag.

 $Returns: value of Aoc_flag.

 $Arguments:

*******************************************************************************/
UBYTE getAOCflag(void)
{
    return Aoc_flag;
}   
#endif


/*******************************************************************************

 $Function:    	service_check_pin_for_aoc

 $Description: this function check the PIN 2 requirement & availability of AOC

 $Returns:

 $Arguments:

*******************************************************************************/
USHORT service_check_pin_for_aoc (void)
{
	T_MFW_AOC_ACCESS access;

	/* check PIN 2 requirement & availability of AOC */
	sim_configuration (NULL, NULL, NULL, NULL, NULL, &access);

	TRACE_EVENT_P3("acm:%d acmmax:%d,puct:%d", access.access_acm,access.access_acmmax,access.access_puct);
      #ifdef FF_CPHS
      //x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
      //The 'Charge/Timers' menu should be displayed if CPHS is not supported.
	if (CphsPresent() != TRUE)
	{
           TRACE_EVENT("CPHS is not present in this SIM");
           setAOCflag(TRUE);  
	}
      TRACE_EVENT_P1("Values: Aoc_flag = %d  - 1 Indicates AOC is set in the CSP", getAOCflag());
      TRACE_EVENT_P1("status : cphs_status = %d ", CphsPresent());
      #endif
	  
	if (access.access_acm     EQ MFW_SIM_PIN2 &&
	   access.access_acmmax EQ MFW_SIM_PIN2 &&
	   access.access_puct     EQ MFW_SIM_PIN2)
	{
		TRACE_EVENT("PIN 2 required");
              //x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
              #ifdef FF_CPHS
		if (getAOCflag())
			return MFW_SIM_PIN2_REQ;
		else
			return MFW_SIM_NO_AOC;
		#else
		return MFW_SIM_PIN2_REQ;
              #endif
	}
	else if (access.access_acm     EQ MFW_SIM_PIN1 &&
	       access.access_acmmax EQ MFW_SIM_PIN1 &&
	       access.access_puct     EQ MFW_SIM_PIN1)
	{
		TRACE_EVENT("PIN 1 required");
              //x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
              #ifdef FF_CPHS
		if (getAOCflag())
			return MFW_SIM_PIN_REQ;
		else
			return MFW_SIM_NO_AOC;	
		#else
		return MFW_SIM_PIN_REQ;		
              #endif
	}
	else if (access.access_acm     EQ MFW_SIM_NO_PIN &&
	       access.access_acmmax EQ MFW_SIM_NO_PIN &&
	       access.access_puct     EQ MFW_SIM_NO_PIN)
	{
		TRACE_EVENT("No PIN  required");
              //x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
              #ifdef FF_CPHS
		if (getAOCflag())
			return MFW_SIM_NO_PIN;
		else
			return MFW_SIM_NO_AOC;
		#else
		return MFW_SIM_NO_PIN;
              #endif
	}
	else if (access.access_acm     EQ MFW_SIM_UNKNOWN &&
	       access.access_acmmax EQ MFW_SIM_UNKNOWN &&
	       access.access_puct     EQ MFW_SIM_UNKNOWN)
	{
		TRACE_EVENT("AOC not supported");
		return MFW_SIM_FAILURE;
	}
       //x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
       #ifdef FF_CPHS
	else
	{
		TRACE_EVENT("AOC Else part");
		if (getAOCflag())
			return MFW_SIM_NO_PIN;
		else
			return MFW_SIM_NO_AOC;
	}
	#endif
	
}

/*******************************************************************************

 $Function:    	service_check_aoc

 $Description:   If there is no support for AOC on SIM card
 			   the item "Charge Timers" will not Appear on the menulist

 $Returns:

 $Arguments:

*******************************************************************************/
USHORT service_check_aoc (struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi)
{
	TRACE_FUNCTION("service_check_aoc");

	switch (service_check_pin_for_aoc ())
	{
		case MFW_SIM_PIN2_REQ:
		case MFW_SIM_PIN_REQ:
		case MFW_SIM_NO_PIN:
			TRACE_EVENT("AOC item visible");
			return 0;
			/*break;*/

		case MFW_SIM_FAILURE:
              //x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
	       #ifdef FF_CPHS
		case MFW_SIM_NO_AOC:
			TRACE_EVENT("service_check_aoc - AOC item not visible");
             #endif
			return 1;
			/*break;*/
	}
	return 1;
}


