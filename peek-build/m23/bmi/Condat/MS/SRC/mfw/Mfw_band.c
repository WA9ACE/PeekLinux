/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW                   $Workfile:: Mfw_band.c           $|
| $Author:: MC/SH                          $Revision::  1              $|
| STATE  : code                                                       |
+--------------------------------------------------------------------+

   MODULE  : MFW_BAND

   PURPOSE : AT%BAND functionality for MFW


   $History: Mfw_band.c                                              $

*/


#define ENTITY_MFW


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif

/* BEGIN ADD: Sumit : Req ID: : 31-Mar-2005*/
#ifndef NEPTUNE_BOARD
/* END ADD: Sumit : Req ID: : 31-Mar-2005*/
#include "ffs/ffs.h"
/* BEGIN ADD: Sumit : Req ID: : 31-Mar-2005*/
#else
#include "ffs.h"
#endif
/* END ADD: Sumit : Req ID: : 31-Mar-2005*/


/* SPR#1470 - SH - these includes are now outside of
 * FAX_AND_DATA switch */
#include "message.h"
#include "prim.h"
#include "aci_cmh.h"

#if defined (FAX_AND_DATA)
/*#include "message.h"
#include "prim.h"
#include "aci_cmh.h"  SPR#1470 - SH */
#include "aci_fd.h"
#endif

#ifdef GPRS
#include "gprs.h"
#endif
#include "dti_conn_mng.h"

#include "phb.h"
#include "psa.h"
#include "cmh.h"
#include "cmh_phb.h"
#include "psa.h"
#include "psa_sim.h"
#include "cus_aci.h"
#include "cmh_mm.h"
#include "cmh_sim.h"

#include "Mfw_band.h"


/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_BAND          |
| STATE  : code                         ROUTINE: band_radio_mode_switch|
+--------------------------------------------------------------------+


   PURPOSE :   Set radio band and mode

*/

T_MFW_BAND_RET band_radio_mode_switch(T_MFW_BAND_MODE mode, UBYTE band)
{
	T_ACI_RETURN ret;

	TRACE_FUNCTION("band_radio_mode_switch");
      /*a0393213 compiler warnings removal - explicit typecasting done*/
	ret = sAT_PercentBAND(CMD_SRC_LCL, (T_ACI_BAND_MODE)mode, band); /*radio mode set without errors*/

	TRACE_EVENT_P1("Band: %d", band);
	TRACE_EVENT_P1("Return val: %d", ret);

	switch(ret)
	{
		case (AT_BUSY):
			return MFW_BAND_Busy;
//			break;  // RAVI
		case (AT_FAIL):
			return MFW_BAND_Fail;
//			break;   // RAVI
		case (AT_CMPL):
			return MFW_BAND_OK;
//			break;  // RAVI
		case (AT_EXCT):
			return MFW_BAND_InProgress;
//			break;   // RAVI
		default:
			return MFW_BAND_Fail;
//			break;     // RAVI
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_BAND          |
| STATE  : code                         ROUTINE: band_get_radio_modes  |
+--------------------------------------------------------------------+


   PURPOSE :   Get list of possible bands

*/

T_MFW_BAND_RET band_get_radio_modes(T_MFW_BAND_MODE* maxMode, UBYTE *band)
{
	T_ACI_RETURN ret;

	ret = tAT_PercentBAND(CMD_SRC_LCL, (T_ACI_BAND_MODE*)maxMode, (UBYTE*)band);

	if (ret == AT_CMPL)
		return MFW_BAND_OK;
	else
		return MFW_BAND_Fail;
}

/*
+--------------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_BAND                |
| STATE  : code                         ROUTINE: band_get_current_radio_mode  |
+---------------------------------------------------------------------------+


   PURPOSE :   Get current band setting

*/
T_MFW_BAND_RET band_get_current_radio_mode(T_MFW_BAND_MODE* mode, UBYTE* band)
{
	T_ACI_RETURN ret;

	ret = qAT_PercentBAND(CMD_SRC_LCL, (T_ACI_BAND_MODE*)mode, (UBYTE*)band);

	if (ret == AT_CMPL)
		return MFW_BAND_OK;
	else
		return MFW_BAND_Fail;

}


