/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-PS
|  Modul   :  DRV_VMD
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  This Module defines the voice memo and recognition
|             driver interface for the G23 protocol stack.
|             
|             This driver is used to control all voice memo and 
|             voice recognition related procedure.
+----------------------------------------------------------------------------- 
*/ 

#ifndef DRV_VMD_C
#define DRV_VMD_C
/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "prim.h"
#include "gsm.h"
#include "tok.h"
#include "gdi.h"
#include "vmd.h"
#ifdef TI_PS_HCOMM_CHANGE
#include "cl_hComm_handle.h"
#endif
/*==== EXPORT =====================================================*/
/*==== VARIABLES ==================================================*/

drv_SignalCB_Type       vmd_signal_callback = NULL;

#ifdef TI_PS_HCOMM_CHANGE
#else
#if defined (NEW_FRAME)
EXTERN T_HANDLE  hCommL1;
#else
EXTERN T_VSI_CHANDLE  hCommL1;
#endif
#endif

/*==== FUNCTIONS ==================================================*/

/*==== TYPES ======================================================*/

typedef struct T_MMI_VM_RECORD_REQ_V
{
  UBYTE    dtx_used;
  USHORT   d_k_x1_t0;
  USHORT   d_k_x1_t1;
  USHORT   d_k_x1_t2;
  USHORT   d_pe_rep;
  USHORT   d_pe_off;
  USHORT   d_se_off;
  USHORT   d_bu_off;
  USHORT   d_t0_on;
  USHORT   d_t0_off;
  USHORT   d_t1_on;
  USHORT   d_t1_off;
  USHORT   d_t2_on;
  USHORT   d_t2_off;
  USHORT * start_address;
  USHORT * stop_address;
} T_MMI_VM_RECORD_REQ_V;

typedef struct T_MMI_VM_PLAY_REQ_V
{
  USHORT * start_address;
} T_MMI_VM_PLAY_REQ_V;

typedef struct T_MMI_VM_END_PLAY_IND
{
  USHORT * stop_address;
} T_MMI_VM_END_PLAY_IND;

typedef struct T_MMI_VM_END_RECORD_IND
{
  USHORT * stop_address;
} T_MMI_VM_END_RECORD_IND;

typedef struct T_MMI_VM_END_PLAY_REQ
{
  UBYTE dummy;
} T_MMI_VM_END_PLAY_REQ;

typedef struct T_MMI_VM_END_RECORD_REQ
{
  UBYTE dummy;
} T_MMI_VM_END_RECORD_REQ;
  
typedef struct 
{
  USHORT  param1;
  USHORT  param2;
  USHORT  param3;
  USHORT  param4;
  USHORT  param5;
}
T_MMI_SR_REQ_V;

typedef struct 
{
  UBYTE   error_code;
  UBYTE   index;
} T_MMI_SR_IND_V;

#define MMI_VM_RECORD_REQ_V          8 
#define MMI_VM_PLAY_REQ_V            9
#define MMI_VM_END_RECORD_REQ      10

#define MMI_SR_REQ_V                 110
#define MMI_SR_IND_V                 119

#define MMI_VM_END_PLAY_REQ        114
#define MMI_VM_END_PLAY_IND        115
#define MMI_VM_END_RECORD_IND      116

LOCAL void vmd_mmi_vm_end_record_ind (T_MMI_VM_END_RECORD_IND * vmd_parameter);
LOCAL void vmd_mmi_vm_end_play_ind (T_MMI_VM_END_PLAY_IND * vmd_parameter);
LOCAL void vmd_mmi_sr_end_ind (T_MMI_SR_IND_V * vmd_parameter);

/*==== CONSTANTS ==================================================*/
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_VMD                    |
| STATE   : code                ROUTINE : vmd_init                   |
+--------------------------------------------------------------------+

  PURPOSE : The function initializes the driver´s internal data.
            The function returns DRV_OK in case of a successful
            completition. The function returns DRV_INITIALIZED if
            the driver has already been initialized and is ready to
            be used or is already in use. In case of an initialization
            failure, which means the that the driver cannot be used,
            the function returns DRV_INITFAILURE.

*/

GLOBAL UBYTE vmd_init (drv_SignalCB_Type in_SignalCBPtr)
{
  vmd_signal_callback   = in_SignalCBPtr;     /* store call-back function */

  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_VMD                    |
| STATE   : code                ROUTINE : vmd_exit                   |
+--------------------------------------------------------------------+

  PURPOSE : The function is called when the driver functionality is
            not longer required.
*/

GLOBAL void vmd_exit (void)
{
  vmd_signal_callback = NULL;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)   MODULE  : DRV_VMD                        |
| STATE   : code            ROUTINE : vmd_start_voice_memo_recording |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to start voice memo recording. 
            The needed parameters are defined in the structure
            vmd_start_record_type.
            
*/

GLOBAL UBYTE vmd_start_voice_memo_recording (vmd_start_record_type * vmd_parameter)
{
  PALLOC (mmi_vm_record_req, MMI_VM_RECORD_REQ_V);

  /*
   * copy parameters to send to layer 1
   */
  mmi_vm_record_req->start_address = vmd_parameter->start_address;
  mmi_vm_record_req->stop_address  = vmd_parameter->stop_address; 
  mmi_vm_record_req->dtx_used      = vmd_parameter->dtx_used;

  switch (vmd_parameter->tone_id)
  {
    /* case ....
       add here the parameter to generate a 
       tone during dedicated mode.
       break;
     */
    case 0:   /* no tone in idle mode */
      mmi_vm_record_req->d_k_x1_t0 = 0;
      mmi_vm_record_req->d_k_x1_t1 = 0;
      mmi_vm_record_req->d_k_x1_t2 = 0;
      mmi_vm_record_req->d_pe_rep  = 0;
      mmi_vm_record_req->d_pe_off  = 0;
      mmi_vm_record_req->d_se_off  = 0;
      mmi_vm_record_req->d_bu_off  = 0;
      mmi_vm_record_req->d_t0_on   = 0;
      mmi_vm_record_req->d_t0_off  = 0;
      mmi_vm_record_req->d_t1_on   = 0;
      mmi_vm_record_req->d_t1_off  = 0;
      mmi_vm_record_req->d_t2_on   = 0;
      mmi_vm_record_req->d_t2_off  = 0;
      break;
  }


  PSENDX (L1, mmi_vm_record_req);

  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)   MODULE  : DRV_VMD                        |
| STATE   : code            ROUTINE : vmd_stop_voice_memo_recording  |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to stop voice memo recording. 
            
*/

GLOBAL UBYTE vmd_stop_voice_memo_recording ()
{
  PALLOC (mmi_vm_end_record_req, MMI_VM_END_RECORD_REQ);

  PSENDX (L1, mmi_vm_end_record_req);

  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)   MODULE  : DRV_VMD                        |
| STATE   : code            ROUTINE : vmd_start_voice_memo_playing   |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to start voice memo playing. 
            The needed parameters are defined in the structure
            vmd_start_play_type.
            
*/

GLOBAL UBYTE vmd_start_voice_memo_playing (vmd_start_play_type * vmd_parameter)
{

  PALLOC (mmi_vm_play_req, MMI_VM_PLAY_REQ_V);

  /*
   * copy parameters to send to layer 1
   */
  mmi_vm_play_req->start_address = vmd_parameter->start_address;
  
  PSENDX (L1, mmi_vm_play_req);

  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)   MODULE  : DRV_VMD                        |
| STATE   : code            ROUTINE : vmd_stop_voice_memo_playing    |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to stop voice memo playing. 
            
*/

GLOBAL UBYTE vmd_stop_voice_memo_playing ()
{
  PALLOC (mmi_vm_end_play_req, MMI_VM_END_PLAY_REQ);

  PSENDX (L1, mmi_vm_end_play_req);

  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)   MODULE  : DRV_VMD                        |
| STATE   : code            ROUTINE : vmd_start_recognition          |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to a speech recognition procedure.
            The needed parameters are defined in the structure
            vmd_start_recognition_req_type.
            
*/

GLOBAL UBYTE vmd_start_recognition (vmd_speech_recognition_req_type * vmd_parameter)
{
  PALLOC (mmi_sr_req, MMI_SR_REQ_V);

  /*
   * copy parameters to send to layer 1
   *
   * NOTE: parameter usage is not clear (missing documents)
   */
  mmi_sr_req->param1 = vmd_parameter->i_speaker;
  mmi_sr_req->param2 = vmd_parameter->i_action;
  mmi_sr_req->param3 = vmd_parameter->i_param1;
  mmi_sr_req->param4 = vmd_parameter->i_param2;
  
  PSENDX (L1, mmi_sr_req);

  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)   MODULE  : DRV_VMD                        |
| STATE   : code            ROUTINE : vmd_mmi_vm_end_record_ind      |
+--------------------------------------------------------------------+

  PURPOSE : This function is called after reception of the
            MMI_VM_END_RECORD_IND primitive from layer 1.
            
*/

LOCAL void vmd_mmi_vm_end_record_ind (T_MMI_VM_END_RECORD_IND * vmd_parameter)
{
  drv_SignalID_Type signal_params;

  signal_params.SignalType  = VMD_END_RECORD;

#if defined (NEW_FRAME)

  signal_params.SignalType = 0;
//nm
//  signal_params.UserData    =  vmd_parameter;

#else
  signal_params.SignalValue = 0;
//nm
//  signal_params.UserData    =  vmd_parameter;
#endif

  if (vmd_signal_callback NEQ NULL)
    (*vmd_signal_callback)(&signal_params);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)   MODULE  : DRV_VMD                        |
| STATE   : code            ROUTINE : vmd_mmi_vm_end_play_ind        |
+--------------------------------------------------------------------+

  PURPOSE : This function is called after reception of the
            MMI_VM_END_PLAY_IND primitive from layer 1.
            
*/

LOCAL void vmd_mmi_vm_end_play_ind (T_MMI_VM_END_PLAY_IND * vmd_parameter)
{
  drv_SignalID_Type signal_params;

  signal_params.SignalType  = VMD_END_PLAY;

#if defined (NEW_FRAME)
  signal_params.SignalType = 0;
//nm
//  signal_params.UserData    = vmd_parameter;

#else
  signal_params.SignalValue = 0;
//nm
//  signal_params.UserData    = vmd_parameter;
#endif

  if (vmd_signal_callback NEQ NULL)
    (*vmd_signal_callback)(&signal_params);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)   MODULE  : DRV_VMD                        |
| STATE   : code            ROUTINE : vmd_mmi_sr_ind                 |
+--------------------------------------------------------------------+

  PURPOSE : This function is called after reception of the
            MMI_SR_IND primitive from layer 1.
            
*/

LOCAL void vmd_mmi_sr_end_ind (T_MMI_SR_IND_V * vmd_parameter)
{
  drv_SignalID_Type signal_params;

  signal_params.SignalType  = VMD_SPEECH_RECOGNITION;

#if defined (NEW_FRAME)
  signal_params.SignalType = 0;
//nm
//  signal_params.UserData    = vmd_parameter;

#else
  signal_params.SignalValue = 0;
//nm
//  signal_params.UserData    = vmd_parameter;
#endif

  if (vmd_signal_callback NEQ NULL)
    (*vmd_signal_callback)(&signal_params);
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)   MODULE  : DRV_VMD                        |
| STATE   : code            ROUTINE : vmd_primitive                  |
+--------------------------------------------------------------------+

  PURPOSE : This function is called from the CST pei module after 
            reception of a primitive from layer 1. If it is a 
            VMD primitive it is distributed and the primitive is
            freed, else nothing happens.
            
*/

GLOBAL UBYTE vmd_primitive (USHORT opc, void * data)
{
  switch (opc)
  {
    case MMI_VM_END_RECORD_IND:
      vmd_mmi_vm_end_record_ind ((T_MMI_VM_END_RECORD_IND *)data);
      PFREE (data);
      return TRUE;

    case MMI_VM_END_PLAY_IND:
      vmd_mmi_vm_end_play_ind ((T_MMI_VM_END_PLAY_IND *)data);
      PFREE (data);
      return TRUE;

    case MMI_SR_IND_V:
      vmd_mmi_sr_end_ind ((T_MMI_SR_IND_V *)data);
      PFREE (data);
      return TRUE;

  }
  return FALSE;
}

#endif
