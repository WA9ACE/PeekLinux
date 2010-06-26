/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	                                                      
 $Project code:	                                                           
 $Module:		
 $File:		    MmiGprs.c
 $Revision:		                                                      
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		                                                          
                                                                               
********************************************************************************
                                                                              
 Description:
 	GPRS handling for MMI.
    
                        
********************************************************************************

 $History: MmiGprs.c

       Oct 18,2006 REF:DR OMAPS00099409 x0047075
       Description:TC2.1.5(BMI/MFW) - GPRS - GPRS Detach error
       Solution : The response P_CGREG_STAT_NOT_REG from the lower layer is now translated as GPRS_OFF by BMI.

       May 29,2006 REF: DR OMAPS00078997 x0043641
       Description: GPRS - DETACH (ERROR)
       Solution: A condition is added when not regeistered, no searching the gprs status
       is updated as not attached.

	Feb 24, 2006 REF:DR OMAPS00068976 x0035544
	Description: GPRS attach icon - MMI follow up issue of OMAPS00051034
	solution: a Check is added for the idle mode display and querry the GPRS status of 
	network registration and update the GPRS  state and attach icon display accordingly.
	
	15/05/2003 - SPR#1983 - SH - Updated to latest from 1.6.3 version.

	Dec 02, 2005 REF: DR OMAPS00048551    x0039928
   	Description: MMI Cloud displays ""GPRS Detached"" in place 
   	of ""GPRS Not Attached"" when unable to attach..
   	Solution: A condition is added to check if the request is for attach and then display 
   	"GPRS Not Attached" in case it fails.
	
	Dec 01, 2005 REF: DR OMAPS00048530   x0039928
   	Description: After the GPRS detach the MMI shows it is attached.
   	Solution: A check is made to display "GPRS Attached" only when an GPRS attach is
   	requested but not when GPRS detach is request.

	Jun 14, 2005 REF: MMI-FIX-30439 x0018858
   	Description: The homezone/cityzone tags were not being displayed properly.
   	Solution: Modified to save the tags properly in caches and also made the 
	appropriate modifications to update the same properly during mobility.
	   
 $End

*******************************************************************************/

#define ENTITY_MFW

/* Switch for tracing*/
#define TRACE_MMIGPRS

#ifdef TRACE_MMIGPRS
#define trace(x)			TRACE_EVENT(x)
#define tracefunction(x)	TRACE_FUNCTION(x)
#define trace_P1(x,a)		TRACE_EVENT_P1(x,a)
#define trace_P2(x,a,b)		TRACE_EVENT_P2(x,a,b)
#define trace_P3(x,a,b,c)	TRACE_EVENT_P3(x,a,b,c)
#define trace_P4(x,a,b,c,d)	TRACE_EVENT_P4(x,a,b,c,d)
#define trace_P5(x,a,b,c,d,e)   TRACE_EVENT_P5(x,a,b,c,d,e)
#define trace_P6(x,a,b,c,d,e,f)  TRACE_EVENT_P6(x,a,b,c,d,e,f)
#else
#define trace(x)			/* */
#define tracefunction(x)	/* */
#define trace_P1(x,a)		/* */
#define trace_P2(x,a,b)		/* */
#define trace_P3(x,a,b,c)	/* */
#define trace_P4(x,a,b,c,d)	/* */
#define trace_P5(x,a,b,c,d,e)   /* */
#define trace_P6(x,a,b,c,d,e,f)  /* */
#endif

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
/* New Editor changes */
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
#include "message.h"
#include "prim.h"
#include "aci_all.h"
#include "aci_cmh.h"  /* include types declared in ACI              */
#include "ati_cmd.h"
#include "aci_cmd.h"
#include "aci.h"
#include "dti_conn_mng.h"
#include "dti_cntrl_mng.h"
#include "gaci.h" /* include types declared in GACI             */
#include "gaci_cmh.h"

#ifdef FF_2TO1_PS
#include "aci_all.h"
#include "ati_cmd.h"
#include "aci_cmd.h"
#include "aci.h"
#include "dti_conn_mng.h"
#include "dti_cntrl_mng.h"
#include "gaci.h" /* include types declared in GACI             */
#endif
#include "mfw_ss.h" /*for convert*/
#include "mfw_cphs.h"
#include "mfw_sat.h"
#include "mfw_gprs.h"
#include "dspl.h"

#include "MmiGprs.h"
#include "MmiBookShared.h"

#include "Mmiicons.h"		/* Including this header allows us to access the global icon status, for the GPRS icon.*/
#include "mmiSmsBroadcast.h"  /* Including this to get the info_screen function */
/* New Editor changes.  Need editor for counter display */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif
#include "MmiBookUtils.h"
#include "MmiBookMenuWindow.h"		/* So we can use bookMenuStart() to create our own menus */

#include "mmiColours.h"

#ifdef NEPTUNE_BOARD
#define GPRS_ICON_SET    1
#define EDGE_ICON_SET    2
#endif

static T_MMI_GPRS_DATA *gprs_data=NULL;

/*******************************************************************************

 $Function:    	GPRS_Data

 $Description:	Initialise GPRS

 $Returns:		Handler for MFW GPRS events.

 $Arguments:	None.

*******************************************************************************/

T_MMI_GPRS_DATA *GPRS_Data(void)
{
	if (gprs_data==NULL)
	{
		TRACE_EVENT("***ERROR*** MMI GPRS not initialised properly");
	}
	
	return gprs_data;
}

/*******************************************************************************

 $Function:    	GPRS_Init

 $Description:	Initialise GPRS

 $Returns:		Handler for MFW GPRS events.

 $Arguments:	None.

*******************************************************************************/

void GPRS_Init(void)
{
	T_MMI_GPRS_DATA *data;
	
	tracefunction("GPRS_Init()");

	gprs_init();

	/* Allocate memory for mmi data */
	data = (T_MMI_GPRS_DATA *)ALLOC_MEMORY(sizeof(T_MMI_GPRS_DATA));
	gprs_data = data;
	memset(data, 0, sizeof(T_MMI_GPRS_DATA));

	data->mfw_gprs = gprs_create(0, E_MFW_GPRS_ALL_EVENTS, (MfwCb)GPRS_MfwCb);
	data->cid = GPRS_CID_OMITTED;
	data->uplink_counter = 0;
	data->downlink_counter = 0;
	data->sms_service = SMS_SERVICE_OMITTED;
	data->display = FALSE;
	data->notify_win = NULL;
	data->callback = NULL;
	data->reg_status = GPRS_OFF;
	
	return;
}


/*******************************************************************************

 $Function:    	GPRS_SetCallback

 $Description:	Specify a callback function, where GPRS connection information will
 				be sent, or NULL to switch off notification.

 $Returns:		None.

 $Arguments:	win - window handler, or NULL
 				callback - callback function, or NULL

*******************************************************************************/

void GPRS_SetCallback(T_MFW_HND win, T_GPRS_CB callback)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();

	tracefunction("GPRS_SetCallback()");

	if (!data)
	{
		return;
	}

	data->notify_win = win;
	data->callback = callback;

	return;
}


/*******************************************************************************

 $Function:    	GPRS_Exit

 $Description:	Exit GPRS

 $Returns:		None.

 $Arguments:	None.

*******************************************************************************/

void GPRS_Exit(void)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();

	tracefunction("GPRS_Exit()");

	if (data)
	{	
		/* Delete any remaining please wait window (just in case!)*/
		GPRS_PleaseWaitDestroy();

		/* Delete the MFW module */
		if (data->mfw_gprs)
		{
			gprs_delete(data->mfw_gprs);
		}
		
		/* Delete MMI data */
		FREE_MEMORY((void *)data, sizeof(T_MMI_GPRS_DATA));
		gprs_data = NULL;
	}
	
	gprs_exit();
	return;
}


/*******************************************************************************

 $Function:    	GPRS_Status

 $Description:	Return status of GPRS.

 $Returns:		GPRS_OFF, GPRS_NOT_ATTACHED, GPRS_ATTACHED

 $Arguments:	None.

*******************************************************************************/

int GPRS_Status(void)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
	T_MFW_GPRS *gprs;
	int result;
	
#ifdef NEPTUNE_BOARD
	int iStatus = 0;
	int iDisplayIcn = 0;
#endif

	tracefunction("GPRS_Status()");

	if (!data)
	{
		return GPRS_OFF;
	}
	
	/* Keeps track of last status value, so we only need to call callback when a change occurs.
	 * Default state is GPRS OFF */

	iconsDeleteState(iconIdGPRSOn);
	
#ifdef NEPTUNE_BOARD
	/* OMAPS00074454 - For Edge Icon Display */
      iconsDeleteState(iconIdEdgeOn);
	/* END */
#endif	
	result = GPRS_OFF;

	/* Check if searching or attached */

	if (data->mfw_gprs)
	{
	 	gprs = ((T_MFW_HDR *)data->mfw_gprs)->data;
//x0035544 Feb 23, 2006 DR:OMAPS00068976
        /* Check for the idle mode and querry the status of network registration 
         *and update the GPRS  state  and attach icon display accordingly */
		if (gprs && (idleIsFocussed() ||data->menu_win == NULL) ) 
		{	
#ifdef NEPTUNE_BOARD                         
		   iDisplayIcn = mfw_gprs_edge_status (&iStatus); 

                 /* Status is obtained from AT command and assigned. */
   		   gprs->data.Reg_state.p_reg_State = (T_P_CGREG_STAT) iStatus;

		   if ((gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_SEARCHING) &&
		       (iDisplayIcn == GPRS_ICON_SET))
		   {
		  	result = GPRS_SEARCHING;
		   }

		   else if((gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_SEARCHING) && 
		       	 (iDisplayIcn == EDGE_ICON_SET))
   		   {
		  	result = EDGE_SEARCHING;
		   }
			
		   /* Check regState and iDisplayIcn For display of GPRS*/
		  else if ((gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_REG_HOME || 
		   	         gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_REG_ROAM ) && 
				  (iDisplayIcn == GPRS_ICON_SET))
		  {
		 	iconsSetState(iconIdGPRSOn);
			result = GPRS_ATTACHED;
		  }

       	  /* Check regState and iDisplayIcn For display of EDGE*/
     		  else if ((gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_REG_HOME || 
     			     gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_REG_ROAM ) && 
     			     (iDisplayIcn == EDGE_ICON_SET))
     		  {
     		       iconsSetState(iconIdEdgeOn);	
     			result = EDGE_ATTACHED;
     		  }
                else if ((gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_NO_CELL || 
			    gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_LIMITED ||
			    gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_UNKN || 
			    gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_REG_DEN  ) &&
			    (iDisplayIcn == EDGE_ICON_SET))
                {
			iconsDeleteState(iconIdGPRSOn );
			result = GPRS_ERROR;                     
                }
                else if ((gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_NO_CELL || 
				    gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_LIMITED ||
				    gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_UNKN || 
				    gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_REG_DEN ) &&
				    (iDisplayIcn == EDGE_ICON_SET))
                {
				iconsDeleteState(iconIdEdgeOn );
				result = EDGE_ERROR;                     
                }
                /* May 29, 2006 Ref bug OMAPS00078997 */
                else if (gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_NOT_REG )
                {
                       iconsDeleteState(iconIdGPRSOn );
                       iconsDeleteState(iconIdEdgeOn );
                       result = GPRS_OFF;
                }
		  else
		  {
                          iconsDeleteState(iconIdGPRSOn);
		            iconsDeleteState(iconIdEdgeOn);
		  }
					 
#else

			gprs->data.Reg_state.p_reg_State = p_gprs_status();

			trace_P1("GPRS_Status: p_regState = %d", gprs->data.Reg_state.p_reg_State);

			if (gprs->data.Reg_state.p_reg_State ==P_CGREG_STAT_SEARCHING)
			{
				result = GPRS_SEARCHING;
			}

			/* Check regState rather than attached to find GPRS status */

			else if (gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_REG_HOME || 
				     gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_REG_ROAM )
			{
				iconsSetState(iconIdGPRSOn);
				result = GPRS_ATTACHED;
			}
			else if(gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_NO_CELL || 
				    gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_LIMITED || 
				    gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_UNKN || 
				    gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_REG_DEN)
				     
			{
				iconsDeleteState(iconIdGPRSOn );
				result = GPRS_ERROR;
			}
                     //Oct 18,2006 REF:DR OMAPS00099409 x0047075
                    // Description:TC2.1.5(BMI/MFW) - GPRS - GPRS Detach error
                    //Solution : The response P_CGREG_STAT_NOT_REG from the lower layer is now translated as GPRS_OFF by BMI.
			else if (gprs->data.Reg_state.p_reg_State == P_CGREG_STAT_NOT_REG )
                    {
                       iconsDeleteState(iconIdGPRSOn );
                       result = GPRS_OFF;
                     }
#endif 
				
		}
		else if (gprs )
		{

#ifdef NEPTUNE_BOARD
			iDisplayIcn = mfw_gprs_edge_status (&iStatus);

                      /* Status is obtained from AT command and assigned. */
			/* x0045876, 14-Aug-2006 (WR - enumerated type mixed with another type) */
			/* gprs->data.Reg_state.regState = iStatus; */
   		       gprs->data.Reg_state.regState = (T_CGREG_STAT) iStatus;

			if ((gprs->data.Reg_state.regState==CGREG_STAT_SEARCHING) && 
			     (iDisplayIcn == GPRS_ICON_SET))
			{
				result = GPRS_SEARCHING;
			}

			else if((gprs->data.Reg_state.regState==CGREG_STAT_SEARCHING) && 
				    (iDisplayIcn == EDGE_ICON_SET))
			{
				result = EDGE_SEARCHING;
			}
			
			/* Check regState and iDisplayIcn For display of GPRS*/
			else if ((gprs->data.Reg_state.regState==CGREG_STAT_REG_HOME || 
				       gprs->data.Reg_state.regState==CGREG_STAT_REG_ROAM) && 
				       (iDisplayIcn == GPRS_ICON_SET))
			{
				iconsSetState(iconIdGPRSOn);
				result = GPRS_ATTACHED;
			}

			/* Check regState and iDisplayIcn For display of EDGE*/
			else if ((gprs->data.Reg_state.regState==CGREG_STAT_REG_HOME || 
				       gprs->data.Reg_state.regState==CGREG_STAT_REG_ROAM) && 
				      (iDisplayIcn == EDGE_ICON_SET))
			{
 			       iconsSetState(iconIdEdgeOn);	
				result = EDGE_ATTACHED;
			}
#else

			//x0035544 Feb 23, 2006 DR:OMAPS00068976
			gprs->data.Reg_state.regState = gprs_status();

			trace_P1("GPRS_Status: regState = %d", gprs->data.Reg_state.regState);
			//x0035544 Feb 23, 2006 DR:OMAPS00068976
			if (gprs->data.Reg_state.regState==CGREG_STAT_SEARCHING)
			{
				result = GPRS_SEARCHING;
			}

			/* Check regState rather than attached to find GPRS status */
			//x0035544 Feb 23, 2006 DR:OMAPS00068976
			else if (gprs->data.Reg_state.regState==CGREG_STAT_REG_HOME ||
				     gprs->data.Reg_state.regState ==CGREG_STAT_REG_ROAM)
			{
				iconsSetState(iconIdGPRSOn);
				result = GPRS_ATTACHED;
			}
#endif			
		}
	}
	
	/* If status has changed, notify */
	if (data->callback!=NULL && result!=data->reg_status)
	{
		data->callback(result);
	}

	data->reg_status = result;

	return result;
}

/*******************************************************************************

 $Function:    	GPRS_ResultScreen

 $Description:	Displays a result and returns GPRS to idle status

 $Returns:		None.

 $Arguments:	parent_win - the parent window
 				TextId1	- the first text string to be shown
 				TextId2  - the second text string to be shown
 
 [afo 03/10/03] Moved function to an earlier location in the file and changed 
			the return type to void. We were taking liberties with C which we 
			got away with using the TI compiler but which Visual C flagged as 
			an error.
			
			We seem to be a bit slack on function scope and return values in 
			some cases in this file.

*******************************************************************************/

static void GPRS_ResultScreen(T_MFW_HND parent_win, USHORT TextId1, USHORT TextId2)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
	
	tracefunction("GPRS_ResultScreen()");

	if (!data)
	{
		return;
	}
	
	GPRS_PleaseWaitDestroy();	
	
	info_screen(parent_win, TextId1, TextId2, NULL);

	data->display = FALSE;

	return;
}

/*******************************************************************************

 $Function:    	GPRS_Attach

 $Description:	Attempts an attach to the GPRS network.

 $Returns:		MMI_GPRS_STATUS_ATTACHING or MMI_GPRS_STATUS_IDLE

 $Arguments:	win - Parent window - if this is provided, status dialogues will be shown

*******************************************************************************/

int GPRS_Attach(T_MFW_HND win)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();

	tracefunction("GPRS_Attach()");

	if (!data)
	{
		/* x0045876, 14-Aug-2006 (WR - non-void function "GPRS_Attach" should return a value) */
		/* return; */
		return 0;
	}
	
	data->status = MMI_GPRS_STATUS_ATTACHING;

	if (win)
	{
		GPRS_PleaseWait(win, TxtPleaseWait, NULL);
	}

	/* Perform the attach */
	
	if (gprs_attach(CGATT_STATE_ATTACHED)==MFW_RES_OK)
	{
		trace("GPRSAttach executing, awaiting confirmation.");
	}
	else
	{
		/* Show result */
		if (win)
		{
			GPRS_ResultScreen(win, TxtFailed, TxtNull);
		}
		data->status = MMI_GPRS_STATUS_IDLE;
	}

	return data->status;
}


/*******************************************************************************

 $Function:    	GPRS_Detach

 $Description:	Detach from the GPRS network.

 $Returns:		MMI_GPRS_STATUS_DETACHING or MMI_GPRS_STATUS_IDLE

 $Arguments:	None

*******************************************************************************/

int GPRS_Detach(T_MFW_HND win)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();

	tracefunction("GPRS_Detach()");

	if (!data)
	{
		/* x0045876, 14-Aug-2006 (WR - non-void function "GPRS_Detach" should return a value) */
		/* return; */
		return 0;
	}

	data->status = MMI_GPRS_STATUS_DETACHING;

	if (win)
	{
		GPRS_PleaseWait(win, TxtPleaseWait, NULL);
	}
	
	if (gprs_attach(CGATT_STATE_DETACHED)==MFW_RES_OK)
	{
		trace("GPRSDetach executing, awaiting confirmation.");
	}
	else
	{
		if (win)
		{
			GPRS_ResultScreen(win, TxtFailed, TxtNull);
		}
		data->status = MMI_GPRS_STATUS_IDLE;
	}

	return data->status;
}


/*******************************************************************************

 $Function:    	GPRS_ContextDefine

 $Description:	Define a PDP context

 $Returns:		TRUE if successful, FALSE otherwise

 $Arguments:	cid - Context ID
 				IPAddress - IP address of context
 				APN - Access Point Name of context

*******************************************************************************/
#ifdef FF_2TO1_PS
int GPRS_ContextDefine(SHORT cid, char *IPAddress, char *APN)
{
	static T_PDP_CONTEXT context;
	
	strcpy(context.pdp_apn, APN);
	strcpy(context.pdp_type, "IP");
	strcpy((char*)context.pdp_addr.ip_address.ipv4_addr.a4, (char*)IPAddress);

  	context.pdp_addr.ctrl_ip_address = NAS_is_ipv4;

  	/* x0045876, 14-Aug-2006 (WR - integer conversion resulted in a change of sign) */
	/*
	context.d_comp = CGDCONT_D_COMP_OMITTED;
	context.h_comp = CGDCONT_H_COMP_OMITTED;
	*/
	context.d_comp = (U8) CGDCONT_D_COMP_OMITTED;
	context.h_comp = (U8) CGDCONT_H_COMP_OMITTED;
	
//	memset(&context.qos,0,sizeof(T_PS_qos));
//	memset(&context.min_qos,0,sizeof(T_PS_qos));

	if (gprs_definePDPContext(cid, &context)!=MFW_RES_OK)
	{
		return FALSE;
	}

	return TRUE;
}
#else
int GPRS_ContextDefine(SHORT cid, char *IPAddress, char *APN)
{
	static T_PDP_CONTEXT context;
	
	strcpy(context.pdp_apn, APN);
	strcpy(context.pdp_type, "IP");
	strcpy((char*)context.pdp_addr.ip_address.ipv4_addr.a4, (char*)IPAddress);
	context.pdp_addr.ctrl_ip_address = NAS_is_ipv4;
	context.d_comp = (U8)CGDCONT_D_COMP_OMITTED;
	context.h_comp = (U8)CGDCONT_H_COMP_OMITTED;
//	memset(&context.qos,0,sizeof(T_QOS));
//	memset(&context.min_qos,0,sizeof(T_QOS));

	if (gprs_definePDPContext(cid, &context)!=MFW_RES_OK)
	{
		return FALSE;
	}

	return TRUE;
}
#endif


/*******************************************************************************

 $Function:    	GPRS_ContextActivate

 $Description:	Activates a PDP context

 $Returns:		TRUE if successful, FALSE otherwise

 $Arguments:	cid - Context ID

*******************************************************************************/

int GPRS_ContextActivate(SHORT cid)
{
	SHORT cids[2];

	tracefunction("GPRS_ContextActivate()");
	
	cids[0] = cid;
	cids[1] = 0;
	
	if (gprs_contextActivation(CGACT_STATE_ACTIVATED, cids)==MFW_RES_OK)
	{
		trace("GPRS_ContextActivate: Failed.");
 		return FALSE;
	}

	trace("GPRS_ContextActivate: Executing, awaiting confirmation.");
	return TRUE;
}


/*******************************************************************************

 $Function:    	GPRS_ContextDeactivate

 $Description:	Dectivates a PDP context

 $Returns:		TRUE if successful, FALSE otherwise

 $Arguments:	cid - Context ID

*******************************************************************************/

int GPRS_ContextDeactivate(SHORT cid)
{
	SHORT cids[2];

	tracefunction("GPRS_ContextDeactivate()");
	
	cids[0] = cid;
	cids[1] = 0;
	
	if (gprs_contextActivation(CGACT_STATE_DEACTIVATED, cids)==MFW_RES_OK)
	{
		trace("GPRS_ContextDeactivate: Failed.");
 		return FALSE;
	}

	trace("GPRS_ContextDeactivate: Executing, awaiting confirmation.");
	return TRUE;
}


/*******************************************************************************

 $Function:    	GPRS_SetQOS

 $Description:	Set quality of service

 $Returns:		TRUE if succeeded, FALSE if failed

 $Arguments:	cid - context ID
 				preced...etc. - quality of service parameters

*******************************************************************************/

#ifdef FF_2TO1_PS
int GPRS_SetQOS(SHORT cid, UBYTE preced, UBYTE delay, UBYTE relclass, UBYTE peak, UBYTE mean)
{
	T_PS_qos QOS;

	tracefunction("GPRS_SetQOS()");

	QOS.qos_r97.preced = preced;
	QOS.qos_r97.delay = delay;
	QOS.qos_r97.relclass = relclass;
	QOS.qos_r97.peak = peak;
	QOS.qos_r97.mean = mean;
	
	if (gprs_setQOS(cid, &QOS)!=MFW_RES_OK)
	{
		trace("GPRS_MenuSetQOS: Failed");
		return FALSE;
	}

	trace("GPRS_MenuSetQOS: OK");
	
	return TRUE;
}
#else
int GPRS_SetQOS(SHORT cid, UBYTE preced, UBYTE delay, UBYTE relclass, UBYTE peak, UBYTE mean)
{
	T_PS_qos QOS;

	tracefunction("GPRS_SetQOS()");

	QOS.qos_r97.preced = preced;
	QOS.qos_r97.delay = delay;
	QOS.qos_r97.relclass = relclass;
	QOS.qos_r97.peak = peak;
	QOS.qos_r97.mean = mean;
	
	
	if (gprs_setQOS(cid, &QOS)!=MFW_RES_OK)
	{
		trace("GPRS_MenuSetQOS: Failed");
		return FALSE;
	}

	trace("GPRS_MenuSetQOS: OK");
	
	return TRUE;
}
#endif

/*******************************************************************************

 $Function:    	GPRS_SetMinQOS

 $Description:	Set minimum quality of service

 $Returns:		TRUE if succeeded, FALSE if failed

 $Arguments:	cid - context ID
 				preced...etc. - quality of service parameters

*******************************************************************************/

#ifdef FF_2TO1_PS
int GPRS_SetMinQOS(SHORT cid, UBYTE preced, UBYTE delay, UBYTE relclass, UBYTE peak, UBYTE mean)
{
	T_PS_qos QOS;

	tracefunction("GPRS_SetMinQOS()");

	QOS.qos_r97.preced = preced;
	QOS.qos_r97.delay = delay;
	QOS.qos_r97.relclass = relclass;
	QOS.qos_r97.peak = peak;
	QOS.qos_r97.mean = mean;
	
	if (gprs_setQOSMin(cid, &QOS)!=MFW_RES_OK)
	{
		trace("GPRS_MenuSetMinQOS: Failed");
		return FALSE;
	}

	trace("GPRS_MenuSetMinQOS: OK");
	
	return TRUE;
}
#else
int GPRS_SetMinQOS(SHORT cid, UBYTE preced, UBYTE delay, UBYTE relclass, UBYTE peak, UBYTE mean)
{
	T_PS_qos QOS;

	tracefunction("GPRS_SetMinQOS()");

	QOS.qos_r97.preced = preced;
	QOS.qos_r97.delay = delay;
	QOS.qos_r97.relclass = relclass;
	QOS.qos_r97.peak = peak;
	QOS.qos_r97.mean = mean;
	
	if (gprs_setQOSMin(cid, &QOS)!=MFW_RES_OK)
	{
		trace("GPRS_MenuSetMinQOS: Failed");
		return FALSE;
	}

	trace("GPRS_MenuSetMinQOS: OK");
	
	return TRUE;
}
#endif

/*******************************************************************************

 $Function:    	GPRS_SmsService

 $Description:	Return SMS Service type

 $Returns:		SMS_SERVICE_xxx

 $Arguments:	None.

*******************************************************************************/

UBYTE GPRS_SmsService(void)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();

	tracefunction("GPRS_SmsService");

	if (!data)
	{
		return SMS_SERVICE_OMITTED;
	}
	
	return data->sms_service;
}


/*******************************************************************************

 $Function:    	GPRS_SetSmsService

 $Description:	Set SMS Service type

 $Returns:		TRUE if set proceeded OK

 $Arguments:	None.

*******************************************************************************/

UBYTE GPRS_SetSmsService(UBYTE service)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
	T_CGSMS_SERVICE ACISmsService;
	UBYTE old_service = 0; /* Warning Correction - 29-11-2005 */
	tracefunction("GPRS_SetSmsService");
	trace_P1("Service: %d", service);
	
	switch(service)
	{
		case SMS_SERVICE_GPRS:
			ACISmsService = CGSMS_SERVICE_GPRS;
			break;
		case SMS_SERVICE_CS:
			ACISmsService = CGSMS_SERVICE_CS;
			break;
		case SMS_SERVICE_GPRS_PREF:
			ACISmsService = CGSMS_SERVICE_GPRS_PREFERRED;
			break;
		case SMS_SERVICE_CS_PREF:
			ACISmsService = CGSMS_SERVICE_CS_PREFERRED;
			break;
		default:
			ACISmsService = CGSMS_SERVICE_CS;
			break;
	}

	if (data)
	{
		old_service = data->sms_service;
		data->sms_service = service;
	}
	
	if (gprs_setServiceSMS(ACISmsService) != MFW_RES_OK)
	{
		trace("gprs_setServiceSMS failed");
		/* Failed - set service back to old value */
		if (data)
		{
			data->sms_service = old_service;
		}
		return FALSE;
	}
	
	return TRUE;
}


/*******************************************************************************

 $Function:    	GPRS_MfwCb

 $Description:	Callback function for MFW GPRS events.

 $Returns:

 $Arguments:	Event number and generic parameter.

*******************************************************************************/

static int GPRS_MfwCb(T_MFW_EVENT event, void* para)
{

	T_MMI_GPRS_DATA	*data = GPRS_Data();
	T_MFW_HND 			win			= mfwParent( mfw_header());
	T_MFW_GPRS_DATA		*gprs_data	= (T_MFW_GPRS_DATA *)para;				// Structure that stores data from mfw
	USHORT				textId;
	int result;
	
	tracefunction("GPRS_MfwCb()");

	if (!data)
	{
		return FALSE; /* Warning Correction - provide return value */
	}

	/* Result of GPRS_NOTHING will not be sent */
	
	result = GPRS_NOTHING;

	/* Find GPRS status */
	
	if (idleIsFocussed())
	{
		/* Update idle screen icons to show any change in GPRS status */
		iconsShow();
	}
	else
	{
		/* iconsShow calls GPRS_Status; if we don't call that, call this on its own */
		GPRS_Status();
	}
	
	switch(event)
	{
		case E_MFW_GPRS_S_CNTXT:
			trace("GPRS_MfwCb: E_MFW_GPRS_S_CNTXT");
			break;

		case E_MFW_GPRS_S_ATT:
			trace("GPRS_MfwCb: E_MFW_GPRS_S_ATT");
			
		// Dec 01, 2005 REF: DR OMAPS00048530   x0039928
		// Fix : Set the current status to attaching or detaching depending on the request.
			if (data->reg_status!= GPRS_ATTACHED)
				data->status = MMI_GPRS_STATUS_ATTACHING;
			else
				data->status = MMI_GPRS_STATUS_DETACHING;
			
			if (data->display)
			{
				if (data->reg_status!= GPRS_ATTACHED)
				{
					/* Previous please wait will be destroyed and this new message shown */
					GPRS_PleaseWait(win, TxtGPRS, TxtAttaching);
				}
				else
				{
					/* MZ cq10952  Info string */
					GPRS_PleaseWait(win, TxtGPRS, TxtDetaching);
				}
			}
			break;

		case E_MFW_GPRS_S_ACT:
			trace("GPRS_MfwCb: E_MFW_GPRS_S_ACT");

			/* Notify of context activation */
			result = GPRS_CONTEXT_ACTIVATE;			
			break;

		case E_MFW_GPRS_R_ACT:
			trace("GPRS_MfwCb: E_MFW_GPRS_R_ACT");

			/* Notify of context activation */
			result = GPRS_CONTEXT_ACTIVATE;
			break;

		case E_MFW_GPRS_S_DATA:
			trace("GPRS_MfwCb: E_MFW_GPRS_S_DATA");
			break;

		case E_MFW_GPRS_R_DATA:
			trace("GPRS_MfwCb: E_MFW_GPRS_R_DATA");
			break;

		case E_MFW_GPRS_S_QOS:
			trace("GPRS_MfwCb: E_MFW_GPRS_S_QOS");
			break;

		case E_MFW_GPRS_R_QOS:
			trace("GPRS_MfwCb: E_MFW_GPRS_R_QOS");
			break;

		case E_MFW_GPRS_S_QOS_MIN:
			trace("GPRS_MfwCb: E_MFW_GPRS_S_QOS_MIN");
			break;

		case E_MFW_GPRS_S_CLASS:
			trace("GPRS_MfwCb: E_MFW_GPRS_S_CLASS");
			break;

		case E_MFW_GPRS_S_PDPADDR:
			trace("GPRS_MfwCb: E_MFW_GPRS_S_PDPADDR");
			break;

		case E_MFW_GPRS_S_AUTORESP:
			trace("GPRS_MfwCb: E_MFW_GPRS_S_AUTORESP");
			break;

		case E_MFW_GPRS_S_ANS:
			trace("GPRS_MfwCb: E_MFW_GPRS_S_ANS");
			break;

		case E_MFW_GPRS_R_ANS:
			trace("GPRS_MfwCb: E_MFW_GPRS_R_ANS");
			break;

		case E_MFW_GPRS_S_EREP:
			trace("GPRS_MfwCb: E_MFW_GPRS_S_EREPG");
			break;

		case E_MFW_GPRS_R_EREP_RJ:
			trace("GPRS_MfwCb: E_MFW_GPRS_R_EREP_RJ");
			
			/* Notify of context deactivation */

			result = GPRS_CONTEXT_DEACTIVATE;
			break;

		case E_MFW_GPRS_R_EREP_ATT:
			trace("GPRS_MfwCb: E_MFW_GPRS_R_EREP_ATT");

			/* Attach status has changed, display message as appropriate */
					
			if (data->display)
			{
				if (data->reg_status==GPRS_ATTACHED)
					textId = TxtAttached;
				else
					textId = TxtNotAttached;
				
				/* Show result */	
				GPRS_ResultScreen(win, TxtGPRS, textId);
			}
			data->status = MMI_GPRS_STATUS_IDLE;

			break;

		case E_MFW_GPRS_R_EREP_ACT:
			trace("GPRS_MfwCb: E_MFW_GPRS_R_EREP_ACT");

			/* Notify of context activation */
			result = GPRS_CONTEXT_ACTIVATE;
			break;

		case E_MFW_GPRS_R_EREP_DEACT:
			trace("GPRS_MfwCb: E_MFW_GPRS_R_EREP_DEACT");

			/* Notify of context deactivation */
			result = GPRS_CONTEXT_DEACTIVATE;
			break;

		case E_MFW_GPRS_R_EREP_CLASS:
			trace("GPRS_MfwCb: E_MFW_GPRS_R_EREP_CLASS");
			break;

		case E_MFW_GPRS_R_REG:
			trace("GPRS_MfwCb: E_MFW_GPRS_R_REG");
			//x0035544 Feb 23, 2006 DR:OMAPS00068976
			trace_P1("regState: %d", gprs_data->Reg_state.p_reg_State);

			switch (data->reg_status)
			{
				case GPRS_ATTACHED:
#ifdef MMI_HOMEZONE_ENABLED
					//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
					//begin 30439
					/* Cell reselection for homezone */
					homezoneUpdate(gprs_data->lac, gprs_data->ci, 0);
					//end 30439
#endif
	// Dec 01, 2005 REF: DR OMAPS00048530   x0039928
	// Fix : Display GPRS attached only if the request is for attaching.
					if(data->status == MMI_GPRS_STATUS_ATTACHING)	
					{
						/* Show result */
						if (data->display)
						{
							GPRS_ResultScreen(win, TxtGPRS, TxtAttached);
						}
					}
					data->status = MMI_GPRS_STATUS_IDLE;
					break;
				
				case GPRS_OFF:
#ifdef MMI_HOMEZONE_ENABLED
					//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
					//begin 30439
					/* Cell reselection for homezone */
					homezoneUpdate(0,0,0);
					//end 30439
#endif
					// Dec 02, 2005 REF: DR OMAPS00048551    x0039928
					// Fix : Display GPRS not attached if gprs attach fails
					// and the current status is attaching.
					if(data->status == MMI_GPRS_STATUS_ATTACHING)
					{
						/* Show result */
						if (data->display)
						{
							GPRS_ResultScreen(win, TxtGPRS, TxtNotAttached);
						}
					}
					else
					{
						/* Show result */
						if (data->display)
						{
							GPRS_ResultScreen(win, TxtGPRS, TxtDetached);
						}
					}
					data->status = MMI_GPRS_STATUS_IDLE;
					break;

				case GPRS_SEARCHING:       /* not registered, searching */
#ifdef MMI_HOMEZONE_ENABLED
					//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
					//begin 30439
					/* Cell reselection for homezone */
					homezoneUpdate(0, 0, 0);
					//end 30439
#endif
					data->status = MMI_GPRS_STATUS_ATTACHING;
					if (data->display)
					{
						GPRS_PleaseWait(win, TxtGPRS, TxtSearching);	/* SPR#1986 */
					}
					break;

				default:
					data->status = MMI_GPRS_STATUS_IDLE;
					/* Show result */
					if (data->display)
					{
						GPRS_ResultScreen(win, TxtGPRS, TxtError);
					}
					break;	
			}
			
			break;

		case E_MFW_GPRS_S_SMSSERV:
			trace("GPRS_MfwCb: E_MFW_GPRS_S_SMSSERV");
			trace_P1("Service: %d", data->sms_service);

			data->status = MMI_GPRS_STATUS_IDLE;
								
			/* Display "Using <service>" */

			if (data->display)
    		{
    		    USHORT TxtId2, TxtId1 = TxtUsing;
    		    switch(data->sms_service)
    		    {
                    case SMS_SERVICE_GPRS:           /* GPRS */
                        TxtId2 = TxtGPRS;
                        break;
                    case SMS_SERVICE_CS:             /* circuit switched */
                        TxtId2 = TxtCSD;
                        break;
                    case SMS_SERVICE_GPRS_PREF: /* GPRS preferred */
                        TxtId2 = TxtGPRSPreferred;
                        break;
                    case SMS_SERVICE_CS_PREF:   /* circuit switched preferred */
                        TxtId2 = TxtCSDPreferred;
                        break;
                    default:                           /* Shouldn't happen, but... */
                        TxtId1 = TxtFailed;
                        TxtId2 = TxtNull;
                        break;
    			}
    			
				/* Show result */	
				GPRS_ResultScreen(win, TxtId1, TxtId2);
    		}
			break;

		case E_MFW_GPRS_OK:
			trace("GPRS_MfwCb: E_MFW_GPRS_OK");
			break;

		case E_MFW_GPRS_ERROR:
			trace("GPRS_MfwCb: E_MFW_GPRS_ERROR");

			data->status = MMI_GPRS_STATUS_IDLE;
								
			/* Display "GPRS Error" */

			if (data->display)
			{
				/* Delete please wait window */
				GPRS_PleaseWaitDestroy();		
			
				/* Show result */	
				GPRS_ResultScreen(win, TxtGPRS, TxtFailed);
			}

			/* Notify of error */
			result = GPRS_ERROR;
			break;

		case E_MFW_GPRS_CONNECT:
			trace("GPRS_MfwCb: E_MFW_GPRS_CONNECT");

			data->status = MMI_GPRS_STATUS_IDLE;
			
			if (data->display)
			{
				/* Delete please wait window */
				GPRS_PleaseWaitDestroy();		
				
				/* Display "CONNECTED" window */
				info_screen(win, TxtGPRS, TxtConnected, NULL);
			}
			break;

		case E_MFW_GPRS_ERR_CONNECT:
			trace("GPRS_MfwCb: E_MFW_GPRS_ERR_CONNECT");

			data->status = MMI_GPRS_STATUS_IDLE;
								
			/* Display "GPRS No Answer" */
			if (data->display)
			{
				/* Show result */	
				GPRS_ResultScreen(win, TxtGPRS, TxtNoAnswer);
			}
			break;

		case E_MFW_GPRS_COUNTER:
			trace("GPRS_MfwCb: E_MFW_GPRS_COUNTER");

			/* Add values returned from ACI
			 * to total counter */
			 
			data->uplink_counter += (ULONG)(gprs_data->counter.octets_uplink);
			data->downlink_counter += (ULONG)(gprs_data->counter.octets_downlink);

			/* Only display counter if user has requested it */
								
			if (data->display)
			{
				data->display = FALSE;
				/* Delete please wait window */
				GPRS_PleaseWaitDestroy();
				
				GPRS_ShowCounter(win);
			}
			break;

		default:
			trace_P1("GPRS_MfwCb: event = %d", event);
			break;
	}

	/* Send a notification if appropriate */
	
	if (data->callback!=NULL && result!=GPRS_NOTHING)
	{
		data->callback(result);
	}
	
	return TRUE;
}


/*******************************************************************************

 $Function:    	GPRS_MenuAttach

 $Description:	Attempts an attach to the GPRS network.

 $Returns:		TRUE if proceeding, FALSE if failed

 $Arguments:	Standard menu option parameters

*******************************************************************************/

int GPRS_MenuAttach(MfwMnu* m, MfwMnuItem* i)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
	T_MFW_HND win = mfw_parent(mfw_header());
/*	int gprsStatus; */ /* Warning Correction */
	
	tracefunction("GPRS_MenuAttach()");

	if (!data)
	{
		return FALSE;
	}

	if (!data->mfw_gprs)
	{
		data->mfw_gprs = gprs_create(0, E_MFW_GPRS_ALL_EVENTS, (MfwCb)GPRS_MfwCb);
		/* Set CID to 0; on increment will become 1. */
		data->cid = GPRS_CID_OMITTED;
	}

	if (data->reg_status==(int)GPRS_ATTACHED)
	{
		/* Show result */	
		GPRS_ResultScreen(win, TxtAlreadyAttached, TxtAttached);
		return FALSE;
	}

	GPRS_Attach(win);

	return TRUE;
}


/*******************************************************************************

 $Function:    	GPRS_MenuDetach

 $Description:	Attempts a detach from the GPRS network.

 $Returns:		TRUE if proceeding, FALSE if failed

 $Arguments:	Standard menu option parameters

*******************************************************************************/

int GPRS_MenuDetach(MfwMnu* m, MfwMnuItem* i)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
	T_MFW_HND win = mfw_parent(mfw_header());
/*	int gprsStatus; */ /* Warning Correction */

	tracefunction("GPRS_MenuDetach()");

	if (!data)
	{
		return FALSE;
	}
	
	/* Check to make sure gprs handle exists */

	if (!data->mfw_gprs)
	{
		/* Show result */	
		GPRS_ResultScreen(win, TxtNotAttached, TxtNull);
		return FALSE;
	}

	if (data->reg_status==(int)GPRS_OFF)
	{
		GPRS_ResultScreen(win, TxtNotAttached, TxtNull);
		return FALSE;
	}

	if (data->reg_status==(int)GPRS_SEARCHING)
	{
		GPRS_ResultScreen(win, TxtGPRS, TxtNotAvailable);
		return FALSE;
	}

	/* Try to detach */

	GPRS_Detach(win);
	
	return TRUE;
}


/*******************************************************************************

 $Function:    	GPRS_MenuIncomingDataCount

 $Description:	Show data counter for last incoming call

 $Returns:		TRUE if proceeding, FALSE if failed

 $Arguments:	Standard menu option parameters

*******************************************************************************/

int GPRS_MenuIncomingDataCount(MfwMnu* m, MfwMnuItem* i)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
	T_MFW_HND		win = mfw_parent(mfw_header());
	T_MFW_RES		result;

	tracefunction("GPRS_MenuIncomingDataCount()");

	if (!data)
	{
		return FALSE;
	}
	
	data->counter_type = MMI_GPRS_COUNTER_DOWNLINK;	/* SPR#1875 - SH - Now DOWNLINK */
	data->display = TRUE;
	data->status = MMI_GPRS_STATUS_DATA_COUNTER;
	GPRS_PleaseWait(win, TxtPleaseWait, NULL); /* SPR#1986 */
	
	/* If a data call is in progress, counter is now reset every
	 * time we check it - MMI keeps track of total.
	 * If a data call is not in progress, display current total from MMI */
	 
	result = gprs_counter(TRUE);

	switch(result)
	{
		/* Waiting for rAT_PercentSNCNT callback, don't update display yet */
		
		case MFW_RES_OK:		
			trace("gprs_counter executing, awaiting confirmation.");
			break;

		/* We're not connected - display current total */
		
		case MfwResDone:
			GPRS_PleaseWaitDestroy();
			data->display = FALSE;
			GPRS_ShowCounter(win);
			break;

		/* Error has occurred */
		
		default:
			GPRS_PleaseWaitDestroy();
			data->display = FALSE;
			info_screen(win, TxtGPRS,TxtError, NULL);
			break;
	}
	
	return TRUE;
}


/*******************************************************************************

 $Function:    	GPRS_MenuOutgoingDataCount

 $Description:	Show data counter for last incoming call

 $Returns:		TRUE if proceeding, FALSE if failed

 $Arguments:	Standard menu option parameters

*******************************************************************************/

int GPRS_MenuOutgoingDataCount(MfwMnu* m, MfwMnuItem* i)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
	T_MFW_HND		win = mfw_parent(mfw_header());
	T_MFW_RES		result;

	tracefunction("GPRS_MenuOutgoingDataCount()");

	if (!data)
	{
		return FALSE;
	}
	
	data->counter_type = MMI_GPRS_COUNTER_UPLINK;	/* SPR#1875 - SH - Now UPLINK */
	data->display = TRUE;
	data->status = MMI_GPRS_STATUS_DATA_COUNTER;
	GPRS_PleaseWait(win, TxtPleaseWait, NULL); /* SPR#1986 */

	/* If a data call is in progress, counter is now reset every
	 * time we check it - MMI keeps track of total.
	 * If a data call is not in progress, display current total from MMI */
	 
	result = gprs_counter(TRUE);

	switch(result)
	{
		/* Waiting for rAT_PercentSNCNT callback, don't update display yet */
		
		case MFW_RES_OK:
			trace("gprs_counter executing, awaiting confirmation.");
			break;

		/* We're not connected - display current total */
		
		case MfwResDone:
			GPRS_PleaseWaitDestroy();
			data->display = FALSE;
			GPRS_ShowCounter(win);
			break;

		/* Error has occurred */
		
		default:
			GPRS_PleaseWaitDestroy();
			data->display = FALSE;
			info_screen(win, TxtGPRS,TxtError, NULL);
			break;
	}
	
	return TRUE;
}



/*******************************************************************************

 $Function:    	GPRS_MenuSelectSms

 $Description:	Sets cursor position for this menu based on the current settings

 $Returns:		TRUE if proceeding, FALSE if failed

 $Arguments:	menu		- pointer to the current menu
 				item		- pointer to the current menu item

*******************************************************************************/

int GPRS_MenuSelectSms(MfwMnu* menu, MfwMnuItem* item)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
	T_MFW_HND win	= mfwParent( mfw_header()); /* GPRS menu window */
/*	T_MFW_GPRS *gprs; */  /* Warning Correction */

	tracefunction("GPRS_MenuSelectSms()");

	if (!data)
	{
		return FALSE;
	}

	data->menu_win = bookMenuStart(win, SMSServiceAttributes(), 0);

	SEND_EVENT(data->menu_win, DEFAULT_OPTION, NULL, &data->sms_service); // Add our own callback

	return TRUE;
}


/*******************************************************************************

 $Function:    	GPRS_MenuSelectSms_set

 $Description:	Send SMS by GPRS/CSD

 $Returns:		TRUE if proceeding, FALSE if failed

 $Arguments:	menu		- pointer to the current menu
 				item		- pointer to the current menu item

*******************************************************************************/

int GPRS_MenuSelectSms_set(MfwMnu* menu, MfwMnuItem* item)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
	T_MFW_HND win = mfwParent( mfw_header());	 /* GPRS menu window */

	tracefunction("GPRS_MenuSelectSms_set()");
	trace_P1("Menu level: %d", menu->lCursor[menu->level]);

	if (!data)
	{
		return FALSE;
	}

	if (!data->mfw_gprs)
	{
		TRACE_EVENT("** No MFW gprs **");
		return FALSE;
	}
	
	data->display = TRUE;

	if(!GPRS_SetSmsService(menu->lCursor[menu->level]))
	{
		info_screen(win, TxtFailed, NULL, NULL);
	}
	
	return TRUE;
}


/*******************************************************************************

 $Function:    	GPRS_PleaseWait

 $Description:	Creates a "Please Wait" message that will hang around until an "OK" or
 				"Error" message is received by GPRS_MfwCb

 $Returns:		Pointer to the created window

 $Arguments:	parent_win - the parent window
 				TextId1	- the first text string to be shown
 				TextId2  - the second text string to be shown

*******************************************************************************/

static T_MFW_HND GPRS_PleaseWait(T_MFW_HND parent_win, USHORT TextId1, USHORT TextId2)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
	T_DISPLAY_DATA display_info;

	tracefunction("GPRS_PleaseWait()");

	if (!data)
	{
		return NULL;
	}
	
	/* Delete any previous window */
	GPRS_PleaseWaitDestroy();

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtCancel, TextId1, TextId2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)GPRS_PleaseWaitCb, PLEASEWAIT_TIMEOUT, KEY_HUP | KEY_RIGHT );

	data->display = TRUE;							/* Allow OK and Fail messages to appear. */
	data->menu_win = parent_win;					/* Store parent window */

	/* Show information screen, storing window pointer */
	data->pleasewait_win = info_dialog(parent_win,&display_info);

	return data->pleasewait_win;
}


/*******************************************************************************

 $Function:    	GPRS_PleaseWaitCb

 $Description:	Please wait window callback function.

 $Returns:		None

 $Arguments:	win - the parent window
 				identifier - not used
 				reason - the reason for the callback

*******************************************************************************/

static void GPRS_PleaseWaitCb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
	
	tracefunction("GPRS_PleaseWaitCb()");

	if (!data)
	{
		return;
	}
	
	/* Dialog already destroyed by this point */

	data->pleasewait_win = NULL;
	
	switch(reason)
	{
		case INFO_TIMEOUT:
		
			/* Delete window */
			
			info_screen(data->menu_win, TxtTimedOut, NULL, NULL);
			data->status = MMI_GPRS_STATUS_IDLE;
			break;
		
		case INFO_KCD_HUP:
		case INFO_KCD_RIGHT:
		
			/*Cancel current operation */
			
			switch(data->status)
			{
				case MMI_GPRS_STATUS_ATTACHING:
				case MMI_GPRS_STATUS_DETACHING:
					gprs_attach_abort();
					data->status = MMI_GPRS_STATUS_IDLE;
					data->display = FALSE;
					break;
				case MMI_GPRS_STATUS_DATA_COUNTER:
					gprs_counter_abort();
					data->status = MMI_GPRS_STATUS_IDLE;
					data->display = FALSE;
					break;
			}
			break;
	}
	
	return;
}


/*******************************************************************************

 $Function:    	GPRS_PleaseWaitDestroy

 $Description:	Check to see if the "Please Wait" window is present, and destroys it
 				if so.

 $Returns:		None.

 $Arguments:	None.

*******************************************************************************/

static void GPRS_PleaseWaitDestroy(void)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
	
	tracefunction("GPRS_PleaseWaitDestroy()");

	if (!data)
	{
		return;
	}
	
	if (data->pleasewait_win)
	{
		SEND_EVENT(data->pleasewait_win, DIALOG_DESTROY, NULL, NULL);
		data->pleasewait_win = NULL;
	}
	
	return;
}


/*******************************************************************************

 $Function:    	GPRS_ShowCounter

 $Description:	Generic counter displaying function

 $Returns:

 $Arguments:	Standard menu option parameters

*******************************************************************************/

static void GPRS_ShowCounter(T_MFW_HND win)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA	editor_data;
#else
	T_EDITOR_DATA	editor_data;
#endif
/*	T_MFW_GPRS		*gprs;  */ /* Warning Correction */
	USHORT			titleId;

	tracefunction("GPRS_ShowCounter()");

	if (!data)
	{
		return;
	}
	
	/* Destroy old editor if it exists */
	
	if (data->edit_win)
	{
		GPRS_DestroyCounter();
	}

	/* Set appropriate title string (corrected) */
	
	switch(data->counter_type)
	{
		case MMI_GPRS_COUNTER_DOWNLINK:
			titleId = TxtIncomingData;
			break;
		
		case MMI_GPRS_COUNTER_UPLINK:
			titleId = TxtOutgoingData;
			break;

		default:
			titleId = TxtNull;
			break;
	}

	data->counterstring = (char *)ALLOC_MEMORY(ULONG_SIZE_IN_DECIMAL);
	GPRS_UpdateCounter();

	/* New Editor */
#ifdef NEW_EDITOR
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetDisplay(&editor_data, ZONE_FULL_SK_TITLE, COLOUR_EDITOR_XX, EDITOR_FONT);
	AUI_edit_SetEvents(&editor_data, data->counter_type, FALSE, FOREVER, (T_AUI_EDIT_CB)GPRS_ShowCounterCb);
	AUI_edit_SetTextStr(&editor_data, TxtReset, TxtSoftBack, titleId, NULL);
	AUI_edit_SetMode(&editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)data->counterstring, ULONG_SIZE_IN_DECIMAL);

	data->edit_win = AUI_edit_Start(win, &editor_data);
#else /* NEW_EDITOR */
	editor_data.TextString			= NULL;
	editor_data.LeftSoftKey			= TxtSoftOK;
	editor_data.RightSoftKey		= TxtReset;
	editor_data.AlternateLeftSoftKey	= NULL;
	editor_data.Callback			= (T_EDIT_CB)GPRS_ShowCounterCb;
	editor_data.Identifier			= data->counter_type;
	editor_data.hide				= FALSE;
	editor_data.mode				= READ_ONLY_MODE;
	editor_data.timeout           	= FOREVER;
	editor_data.min_enter			= 0;
	editor_data.destroyEditor		= FALSE;
	editor_data.editor_attr.win.px	= 0;
	editor_data.editor_attr.win.py	= Mmi_layout_line(1)+4;
	editor_data.editor_attr.win.sx	= SCREEN_SIZE_X;
	editor_data.editor_attr.win.sy	= Mmi_layout_line(2)+4;
	editor_data.editor_attr.edtCol	= COLOUR_EDITOR_XX;
	editor_data.editor_attr.font	= 0;
	editor_data.editor_attr.mode	= edtCurNone;
	editor_data.editor_attr.controls	= 0;
    editor_data.editor_attr.size	= ULONG_SIZE_IN_DECIMAL;
	editor_data.TextId				= titleId;

	editor_data.editor_attr.text = data->counterstring;

	/* Create the dialog handler */
	
	data->edit_win = editor_start(win, &editor_data);  /* start the editor */
#endif /* NEW_EDITOR */

	return;
}


/*******************************************************************************

 $Function:    	GPRS_ShowCounterCb

 $Description:	Change text string for the counter we've just changed

 $Returns:		nothing.

 $Arguments:	win		- the editor window
 				identifier - the type of counter we're updating
 				reason	- the event that caused the callback

*******************************************************************************/

static void GPRS_ShowCounterCb(T_MFW_HND win, USHORT identifier, USHORT reason)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
	T_DISPLAY_DATA display_info;
	
	tracefunction("GPRS_ShowCounterCb()");

	if (!data)
	{
		return;
	}
	
	switch(reason)
	{
		/* Prompt whether to reset */
		 
		case INFO_KCD_LEFT:
			dlg_initDisplayData_TextId(&display_info, TxtCancel, TxtReset, TxtDataCounterReset, TxtNull, COLOUR_STATUS);
			dlg_initDisplayData_events(&display_info, (T_VOID_FUNC) GPRS_ResetCounter, TEN_SECS, KEY_HUP|KEY_CLEAR|KEY_LEFT|KEY_RIGHT );

		    info_dialog(win, &display_info);
			break;

		/* Press hangup key or select OK
		 * Calls GPRS_DestroyCounter to destroy window */
		case INFO_KCD_RIGHT:
		case INFO_KCD_HUP:
			GPRS_DestroyCounter();
			break;
	}

	return;
}


/*******************************************************************************

 $Function:    	GPRS_ResetCounter

 $Description:	Callback from dialogue prompting whether to reset the data counter

 $Returns:		None

 $Arguments:	Standard callback parameters
 
*******************************************************************************/

static void GPRS_ResetCounter(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();

	tracefunction("GPRS_ResetCounter()");

	if (!data)
	{
		return;
	}
	
	switch(reason)
	{
		/* RSK - Reset counter */
		
		case INFO_KCD_RIGHT:
			if (data->counter_type==MMI_GPRS_COUNTER_UPLINK)
			{
				data->uplink_counter = 0;
			}
			else
			{
				data->downlink_counter = 0;
			}
					
			GPRS_ShowCounter(win);
			break;
			
		/* LSK or HUP - cancel without resetting */
		
		case INFO_KCD_LEFT:
		case INFO_KCD_HUP:
			break;
	}
	
	return;
}


/*******************************************************************************

 $Function:    	GPRS_DestroyCounter

 $Description:	Destroy counter display

 $Returns:		None

 $Arguments:	None
 
*******************************************************************************/

static void GPRS_DestroyCounter()
{
	T_MMI_GPRS_DATA *data = GPRS_Data();

	tracefunction("GPRS_DestroyCounter()");
	
	if (!data)
	{
		return;
	}
	
	if (data->edit_win)
	{

	/* New Editor */
	
#ifdef NEW_EDITOR
		AUI_edit_Destroy(data->edit_win);
#else
		editor_destroy(data->edit_win);
#endif
		FREE_MEMORY((void *)data->counterstring, ULONG_SIZE_IN_DECIMAL);
		data->counterstring = NULL;
		data->edit_win = NULL;
	}
	
	return;
}

/*******************************************************************************

 $Function:    	GPRS_UpdateCounter

 $Description:	Change text string for the counter we've just changed

 $Returns:		None

 $Arguments:	None

*******************************************************************************/

static void GPRS_UpdateCounter(void)
{
	T_MMI_GPRS_DATA *data = GPRS_Data();
	ULONG counter;

	tracefunction("GPRS_UpdateCounter()");

	if (!data)
	{
		return;
	}
	
	counter = 0;
	
	switch(data->counter_type)
	{
		case MMI_GPRS_COUNTER_UPLINK:
			counter = data->uplink_counter;
			break;
		
		case MMI_GPRS_COUNTER_DOWNLINK:
			counter = data->downlink_counter;
			break;
	}

	if (counter<1024)
	{
		sprintf(data->counterstring, "%d bytes",counter);		// Counter is <1K, display bytes
	}
	else
	{
		counter = counter/KILOBYTE;
		if (counter<1024)
			sprintf(data->counterstring, "%dKB",counter);		// Counter is <1M, display Kbytes
		else
		{
			counter = counter/KILOBYTE;
			sprintf(data->counterstring, "%dMB",counter);		// Display Mbytes
		}
	}

	return;
}

