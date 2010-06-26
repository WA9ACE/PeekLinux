/*----------------------------------------------------------------------------
|  Project :  3G PS
|  Module  :  SM
+-----------------------------------------------------------------------------
|             Copyright 2003 Texas Instruments.
|             All rights reserved. 
| 
|             This file is confidential and a trade secret of Texas 
|             Instruments .
|             The receipt of or possession of this file does not convey 
|             any rights to reproduce or disclose its contents or to 
|             manufacture, use, or sell anything it may describe, in 
|             whole, or in part, without the specific written consent of 
|             Texas Instruments. 
+-----------------------------------------------------------------------------
| Purpose:    This module implements the process body interface
|             for the User Plane Manager (SM) entity.
|             For design details, see:
|             8010.939 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef SM_PEI_C
#define SM_PEI_C
#endif

/*==== INCLUDES =============================================================*/

#include "sm.h"

#include "ccdapi.h"

#include "sm_sequencer.h"
#include "sm_dispatcher.h"

#include "mon_sm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/* Communication handles */
T_HANDLE            hCommACI   = VSI_ERROR;
T_HANDLE            hCommMM    = VSI_ERROR;
T_HANDLE            hCommUPM   = VSI_ERROR;
T_HANDLE            sm_handle;

static T_MONITOR sm_mon;
static BOOL first_access = TRUE;

/* Global entity data structure */
T_SM_DATA sm_data;

/* Air interface message coding/decoding buffer */
U8 _decodedMsg [MAX_MSTRUCT_LEN_SM];

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
| Function		: pei_primitive
+------------------------------------------------------------------------------
| Description	:  This function is called by the frame when a primitive is
|		   received and needs to be processed.
|
|                      |                      |
|                      |                      |                UPPER LAYER
|                      v                      v
|  +--------------(SMREG SAP)--------------(SM SAP)---------------+
|  |                                                              |
|  |                              SM                              |
|  |                                                              |
|  +-------------------------------^------------------------------+
|                                  |
|                                  |                           LOWER LAYER
|                             (MMPM SAP)
|
| Parameters  :	prim      - Pointer to the received primitive
|
| Return      :	PEI_OK    - function succeeded
|               PEI_ERROR - function failed
+------------------------------------------------------------------------------
*/
static short pei_primitive (void *primitive)
{
  (void)TRACE_FUNCTION("SM pei_primitive");

  if (primitive != NULL)
  {
    T_PRIM_HEADER *prim;
    U32 opc;

    opc = P_OPC(primitive);

/*@i1@*/ (void)PTRACE_IN (opc);

    prim = (T_PRIM_HEADER *)&((T_PRIM *)primitive)->data;

    /* Memory supervision ? */
    VSI_PPM_RCV (primitive);

    switch (opc)
    {
      /* SMREG SAP */
      case SMREG_PDP_ACTIVATE_REQ:
        sm_disp_smreg_pdp_activate_req((T_SMREG_PDP_ACTIVATE_REQ *)(prim));
        PFREE(prim);
        break;

      case SMREG_PDP_ACTIVATE_SEC_REQ:
        sm_disp_smreg_pdp_activate_sec_req((T_SMREG_PDP_ACTIVATE_SEC_REQ *)(prim));
        PFREE(prim);
        break;

      case SMREG_PDP_ACTIVATE_REJ_RES:
        sm_disp_smreg_pdp_activate_rej_res((T_SMREG_PDP_ACTIVATE_REJ_RES *)(prim));
        PFREE(prim);
        break;

      case SMREG_PDP_DEACTIVATE_REQ:
        sm_disp_smreg_pdp_deactivate_req((T_SMREG_PDP_DEACTIVATE_REQ *)(prim));
        PFREE(prim);
        break;

      case SMREG_PDP_MODIFY_REQ:
        sm_disp_smreg_pdp_modify_req((T_SMREG_PDP_MODIFY_REQ *)(prim));
        PFREE(prim);
        break;


      /* SM SAP */
      case SM_ACTIVATE_RES:
        sm_disp_sm_activate_res((T_SM_ACTIVATE_RES *)(prim));
        PFREE(prim);
        break;

      case SM_DEACTIVATE_RES:
        sm_disp_sm_deactivate_res((T_SM_DEACTIVATE_RES *)(prim));
        PFREE(prim);
        break;

      case SM_MODIFY_RES:
        sm_disp_sm_modify_res((T_SM_MODIFY_RES *)(prim));
        PFREE(prim);
        break;

      case SM_STATUS_REQ:
        sm_disp_sm_status_req((T_SM_STATUS_REQ *)(prim));
        PFREE(prim);
        break;

      /* MMPM SAP */
      case MMPM_ATTACH_IND:
        sm_disp_mmpm_attach_ind((T_MMPM_ATTACH_IND *)(prim));
        PFREE(prim);
        break;

      case MMPM_DETACH_IND:
        sm_disp_mmpm_detach_ind((T_MMPM_DETACH_IND *)(prim));
        PFREE(prim);
        break;

      case MMPM_UNITDATA_IND:
        sm_disp_mmpm_unitdata_ind((T_MMPM_UNITDATA_IND *)(prim));
        PFREE(prim);
        break;

      case MMPM_SUSPEND_IND:
        sm_disp_mmpm_suspend_ind((T_MMPM_SUSPEND_IND *)(prim));
        PFREE(prim);
        break;

      case MMPM_RESUME_IND:
        sm_disp_mmpm_resume_ind((T_MMPM_RESUME_IND *)(prim));
        PFREE(prim);
        break;

      default:
        /* forward sys primitives to the environment */
        if ( (opc & SYS_MASK) != 0) {
          (void)vsi_c_primitive(VSI_CALLER primitive);
          return (short)PEI_OK;
        } else {
          PFREE(prim);
          return (short)PEI_ERROR;
        }
    } /* switch */
  } /* if (prim != NULL) */

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function	: pei_init
+------------------------------------------------------------------------------
| Description	: This function is called by the frame. It is used
|                 to initialise the entity.
|
| Parameters	: handle            - task handle
|
| Return	: PEI_OK            - entity initialised
|		  PEI_ERROR         - entity not (yet) initialised
+------------------------------------------------------------------------------
*/
static short pei_init (T_HANDLE handle)
{
  (void)TRACE_FUNCTION ("SM pei_init");

  /*
   * Initialize task handle
   */
  sm_handle = handle;
  /*
   * Open communication channels
   */
  if (hCommACI < VSI_OK)
  {
    if ((hCommACI = vsi_c_open(VSI_CALLER ACI_NAME)) < VSI_OK)
    {
      return (short)PEI_ERROR;
    }
  }

  if (hCommMM < VSI_OK)
  {
/*#ifdef SM_EDGE  */
    if ((hCommMM = vsi_c_open(VSI_CALLER GMM_NAME)) < VSI_OK)
/*#else */ /* #ifdef SM_EDGE */
/*  if ((hCommMM = vsi_c_open(VSI_CALLER MM_NAME)) < VSI_OK)
  #endif */ /*#ifdef SM_EDGE*/
     {
      return (short)PEI_ERROR;
     }
  }

  if (hCommUPM < VSI_OK)
  {
    if ((hCommUPM = vsi_c_open (VSI_CALLER UPM_NAME)) < VSI_OK)
    {
      return (short)PEI_ERROR;
    }
  }

  /* Clear static entity data store */
  memset(&sm_data, 0, sizeof(sm_data));

  /* Initialize multi-entity state machine */
  sm_sequencer_init();

  /*
   * Initialize entity data (call init functions)
   */
  (void)ccd_init();

  return (short)PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : pei_timeout
+------------------------------------------------------------------------------
| Description : This function is called by the frame when a timer has expired.
|
| Parameters  : index             - timer index
|
| Return      : PEI_OK            - timeout processed
|               PEI_ERROR         - timeout not processed
|
+------------------------------------------------------------------------------
*/

static short pei_timeout (U16 index)
{
  (void)TRACE_FUNCTION("pei_timeout");

  /*
   * Process timeout
   */
  sm_disp_pei_timeout(index);

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function	: pei_exit
+------------------------------------------------------------------------------
| Description	: This function is called by the frame when the entity
|                 is terminated. All open resources are freed.
|
| Parameters	: -
|
| Return	: PEI_OK            - exit sucessful
|		  PEI_ERROR         - exit not successful
+------------------------------------------------------------------------------
*/
static short pei_exit (void)
{
  int nsapi;
  (void)TRACE_FUNCTION ("SM pei_exit");

  /*
   * Clean up entity data
   */

  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++) {
    int                       index = sm_nsapi_to_index((U16)nsapi);

    if (sm_data.sm_context_array[index] != NULL) {
      /* sm_free_context_data_by_nsapi also sets pointer to NULL */
      sm_free_context_data_by_nsapi(nsapi);
    }

    if (sm_data.sm_pending_mt_array[index] != NULL) {
      /* sm_free_pending_mt_context_by_index also sets pointer to NULL */
      sm_free_pending_mt_context_by_index((U16)index);
    }
  }

  sm_sequencer_exit();

  /* Exit CCD to clean up data stores */
  (void)ccd_exit();

  /*
   * Close communication channels
   */
  (void)vsi_c_close(VSI_CALLER hCommACI);
  hCommACI = VSI_ERROR;

  (void)vsi_c_close(VSI_CALLER hCommMM);
  hCommMM = VSI_ERROR;

  (void)vsi_c_close(VSI_CALLER hCommUPM);
  hCommUPM = VSI_ERROR;

  return (short)PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function		: pei_config
+------------------------------------------------------------------------------
| Description	: This function is called by the frame when a primitive is
|                 received indicating dynamic configuration.
|
|                 This function is not used in this entity.
|
| Parameters	:	handle            - Communication handle
|
| Return      : PEI_OK            - sucessful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
static short pei_config (char *inString)
{
  (void)TRACE_FUNCTION ("SM pei_config");
  (void)TRACE_FUNCTION (inString);

#ifdef DEBUG
  if ( strcmp(inString, "DUMP_STATE") == 0) {
    sm_dump_state();
  }
#endif

  return (short)PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function	: pei_monitor
+------------------------------------------------------------------------------
| Description	: This function is called by the frame in case sudden entity
|                 specific data is requested (e.g. entity Version).
|
| Parameters	:  out_monitor       - return the address of the data to be
|                                      monitoredCommunication handle
|
| Return	:  PEI_OK            - sucessful (address in out_monitor is valid)
|	           PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
/*@-compdef@*/ /*@-mods@*/
static short pei_monitor (void **out_monitor) /*@globals undef sm_mon@*/
{
  (void)TRACE_FUNCTION ("SM pei_monitor");

  /*
   * Version = "0.S" (S = Step).
   */
  sm_mon.version = "SM 0.1";
  *out_monitor = &sm_mon;

  return PEI_OK;
}
/*@=compdef@*/ /*@=mods@*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function		: pei_create
+------------------------------------------------------------------------------
| Description	:  This function is called by the frame when the process is 
|                  created.
|
| Parameters	:  out_name          - Pointer to the buffer in which to locate
|                                      the name of this entity
|
| Return	:  PEI_OK            - entity created successfuly
|		   PEI_ERROR         - entity could not be created
|
+------------------------------------------------------------------------------
*/
short pei_create (T_PEI_INFO **info)
{
/*@-nullassign@*/
static T_PEI_INFO pei_info =
              {
/*@i1@*/       SM_NAME,       /* name */
               {              /* pei-table */
                 pei_init,
                 pei_exit,
                 pei_primitive,
                 pei_timeout,
                 NULL,        /* no pei_signal function */
                 NULL,        /* no pei_run function */
                 pei_config,
                 pei_monitor
               },
               (U32)2048,     /* stack size */ 
               (U16)10,       /* queue entries */
               (U16)165,      /* priority (1->low, 255->high) */
               (U16)SM_TIMER_MAX,  /* number of timers */
               (U8)0x03       /* flags: bit 0   active(0) body/passive(1) */
              };              /*        bit 1   com by copy(0)/reference(1) */
/*@=nullassign@*/

  (void)TRACE_FUNCTION ("SM pei_create");

  /*
   * Close Resources if open
   */
  if (first_access)
  {
    first_access = FALSE;
  } else {
    (void)pei_exit();
  }

  /*
   * Export startup configuration data
   */
  *info = &pei_info;

  return PEI_OK;
}

/*==== END OF FILE ==========================================================*/
