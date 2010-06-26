/*******************************************************************************

					CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name:	Basic MMI
 $Project code:	BMI (6349)
 $Module:		Start
 $File:		    MmiDummy.c
 $Revision:		1.0

 $Author:		Condat(UK)
 $Date:		    25/10/00

********************************************************************************

 Description:



********************************************************************************
 $History: MmiDummy.c

	July 09, 2005   REF : MMI-22565 - a0876501
	Description:	IMEI CD byte calculation added
	Solution:	Byte alignment done for IMEI display.

	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg
	Description:	IMEI retreival  by posting a call to ACI sAT_xx API
	Solution:		The direct call to cl_get_imeisv() is replaced with the
					call to the callNUmber() which inturn calls sAT_Dn() and
					retrieves the IMEI info

*******************************************************************************

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
//NM
#include "gdi.h"
#include "audio.h"
#ifndef FF_2TO1_PS
#include "cl_imei.h"    /* IMEI common library */
#endif
//NM

#include "mfw_sys.h"
#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
#include "mfw_lng.h"
#include "mfw_icn.h"

#include "mon_aci.h"
#include "tok.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#else
#include "prim.h"
#endif



#include "message.h"
#include "prim.h"
#include "aci_cmh.h"
//NM

/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_mnu.h"
#include "mfw_sat.h"
#include "mfw_nm.h"
#include "mfw_tim.h"
#include "mfw_phb.h"
#include "mfw_sms.h"

#include "dspl.h"

#include "ksd.h"
#include "psa.h"

#include "MmiVersion.h"
#include "MmiMmi.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"

#include "MmiMain.h"
//	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg
#ifdef FF_MMI_ATC_MIGRATION
#define ATD_IMEI "*#06#"
#define IMEI_LEN 15
EXTERN void callNumber(UBYTE *number);
#endif


#ifdef NEPTUNE_BOARD
extern const UBYTE C_DEFAULT_IMEISV[CL_IMEI_SIZE];
#endif


//	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg
#ifdef FF_MMI_ATC_MIGRATION
char imei[IMEI_LEN+1];
#endif

//	July 09, 2005   REF : MMI-22565 - a0876501
extern UBYTE getCdByteFromImei(UBYTE *imei);


//Added by Muthu for PCM Replacemeny

#ifdef PCM_2_FFS
#define PSPDF_IMEI_ID      "/gsm/IMEI"          /* Int. Mobile Equipment Id */
#endif

/*******************************************************************************************
 *
 * Global definition of message function plus some dummies
 *
 *******************************************************************************************/

void mmi_send_event (T_MFW_HND win, USHORT event, USHORT value, void * parameter)
{

	if (win){
		T_MFW_WIN     * win_data  = ((T_MFW_HDR *)win)->data;
		  T_MMI_CONTROL * control  = (T_MMI_CONTROL *)win_data->user;

		if (control->dialog NEQ 0)
		    (control->dialog) (win, event, value, parameter);
	}
}

UBYTE idle_screen_has_focus()
{
  return TRUE;
}

//	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg
//	this function is no more referenced hence omit it
#ifdef FF_MMI_ATC_MIGRATION
/*******************************************************************************

 $Function:    	mmi_imei_retrieve

 $Description:	This function invokes sAT_Dn() to retrieve the IMEI and store it
 				in the global variabel "imei".

 $Returns:

 $Arguments:
*******************************************************************************/
void mmi_imei_retrieve()
{
	callNumber((UBYTE*)ATD_IMEI);
}
#else
/*******************************************************************************

 $Function:    	get_imei

 $Description:	This function invokes the operation for key sequence
				command for getting the IMEI.

 $Returns:

 $Arguments:
*******************************************************************************/
int get_imei(char *imei)
{
#ifdef FF_2TO1_PS
  UBYTE retVal;                     /* holds return value */

/* x0045876, 14-Aug-2006 (WR - "version" was declared but never referenced) */
#ifndef PCM_2_FFS
  UBYTE version;                        /* holds version */
#endif
  
#ifdef PCM_2_FFS  
  UBYTE buf[8];   // Sizeof ( T_PSPDF_IMEI )
#else
  UBYTE buf[SIZE_EF_IMEI];
#endif
  
  TRACE_FUNCTION ("get_imei()");

#ifdef PCM_2_FFS
	/* OMAPS00059546 : If there is no IMEI file, copying the default imei 
		has been added */
  	retVal = ffs_file_read((UBYTE *)PSPDF_IMEI_ID, buf, sizeof(T_PSPDF_IMEI) );
	if (retVal != sizeof(T_PSPDF_IMEI))
	  {
	    TRACE_ERROR("CL FFS read ERROR (IMEI) -> Default values used");
	    memcpy(buf, C_DEFAULT_IMEISV, sizeof(T_PSPDF_IMEI));
	  }
#else
  retVal = pcm_ReadFile ((UBYTE *)EF_IMEI_ID, SIZE_EF_IMEI, buf, &version);
  if( retVal EQ PCM_INVALID_SIZE OR retVal EQ PCM_INVALID_FILE )
    return( FALSE );
#endif

#else
	UBYTE buf[CL_IMEI_SIZE];

	TRACE_FUNCTION ("get_imei()");

  cl_get_imeisv(CL_IMEI_SIZE, buf, CL_IMEI_GET_STORED_IMEI);
#endif  

/* OMAPS00059546 : BCD Conversion for IMEI has been added for NEPTUNE */
#ifdef NEPTUNE_BOARD
  imei[0]  = ( buf [0] & 0x0F) +0x30;  
  imei[1]  = ((buf [0] >> 4) & 0x0F)+0x30;
  imei[2]  = ( buf [1] & 0x0F)+0x30;
  imei[3]  = ((buf [1] >> 4) & 0x0F)+0x30;
  imei[4]  = ( buf [2] & 0x0F)+0x30;
  imei[5]  = ((buf [2] >> 4) & 0x0F)+0x30;
  imei[6]  = ( buf [3] & 0x0F)+0x30;
  imei[7]  = ((buf [3] >> 4) & 0x0F)+0x30;
  imei[8]  = ( buf [4] & 0x0F)+0x30;
  imei[9]  = ((buf [4] >> 4) & 0x0F)+0x30;
  imei[10] = ( buf [5] & 0x0F)+0x30;
  imei[11] = ((buf [5] >> 4) & 0x0F)+0x30;
  imei[12] = ( buf [6] & 0x0F)+0x30;
  imei[13] = ((buf [6] >> 4) & 0x0F)+0x30;
  imei[14] = ((getCdByteFromImei(buf)) & 0x0F)+0x30;
  imei[15] = ( buf [7] & 0x0F)+0x30;
  imei[16] = ((buf [7] >> 4) & 0x0F)+0x30;
  imei[17] = 0;
#else
  imei[0]  = ((buf [0] >> 4) & 0x0F)+0x30;
  imei[1]  = ( buf [0] & 0x0F) +0x30;
  imei[2]  = ((buf [1] >> 4) & 0x0F)+0x30;
  imei[3]  = ( buf [1] & 0x0F)+0x30;
  imei[4]  = ((buf [2] >> 4) & 0x0F)+0x30;
  imei[5]  = ( buf [2] & 0x0F)+0x30;
  imei[6]  = ((buf [3] >> 4) & 0x0F)+0x30;
  imei[7]  = ( buf [3] & 0x0F)+0x30;
  imei[8]  = ((buf [4] >> 4) & 0x0F)+0x30;
  imei[9]  = ( buf [4] & 0x0F)+0x30;
  imei[10] = ((buf [5] >> 4) & 0x0F)+0x30;
  imei[11] = ( buf [5] & 0x0F)+0x30;
  imei[12] = ((buf [6] >> 4) & 0x0F)+0x30;
  imei[13] = ( buf [6] & 0x0F)+0x30;
//	July 09, 2005   REF : MMI-22565 - a0876501
//	For calculating the Cd byte
  imei[14] = ((getCdByteFromImei(buf)) & 0x0F)+0x30;
  imei[15] = (( buf [7] >> 4) & 0x0F)+0x30;
  imei[16] = ( buf [7] & 0x0F)+0x30;
  imei[17] = 0;
#endif

  TRACE_EVENT_P8("MMI INFO IMEI: TAC %1c%1c%1c%1c%1c%1c%1c%1c",
                  imei[0], imei[1], imei[2], imei[3],
                  imei[4], imei[5], imei[6], imei[7]);
  TRACE_EVENT_P6("MMI INFO IMEI: SNR %1c%1c%1c%1c%1c%1c",
                  imei[8],  imei[9], imei[10], imei[11],
                  imei[12], imei[13]);
  TRACE_EVENT_P4("MMI INFO IMEI: SV CD %1c%1c%1c%1c", imei[14], imei[15], imei[16],imei[17]);

  return( TRUE );
}
#endif

char* get_build_user()
{
	return BUILD_USER;
}

char* get_build_date()
{
	return BUILD_DATE;
}

char* get_build_time()
{
	return BUILD_TIME;
}

/************************ End of functions *************************************************/

#ifndef FF_WAP
void rAT_WAP_start_gprs_login(void)
{
	//Dummy function - WAP disabled/not compiled
}
#endif
/* END OF DUMMY FUNCTIONS */
