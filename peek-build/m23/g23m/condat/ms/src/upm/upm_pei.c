/*----------------------------------------------------------------------------
|  Project :  3G PS
|  Module  :  UPM
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
|             for the User Plane Manager (UPM) entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef UPM_PEI_C
#define UPM_PEI_C
#endif

/*==== INCLUDES =============================================================*/

#include "upm.h"

#include <pei.h>
#include <custom.h>

#include "upm_dispatcher.h"

#include "mon_upm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

static T_MONITOR upm_mon;
static BOOL first_access = TRUE;

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
| Function              : pei_primitive
+------------------------------------------------------------------------------
| Description   :  This function is called by the frame when a primitive is
|                  received and needs to be processed.
|
|                                 |
|                                 |                                 UPPER LAYER
|                                 v
|  +---------------------------(DTI SAP)--------------------------+
|  |                                                              |
|  |                             UPM                              |
|  |                                                              |
|  +-^---------^---------^---------^--------^---------^---------^-+
|    |         |         |         |        |         |         |
|    |         |         |         |        |         |         |   LOWER LAYER
| (SM SAP) (MMPM SAP) (RR SAP) (RRC SAP) (SN SAP) (PDCP SAP) (MEM SAP)
|
| Parameters  : prim      - Pointer to the received primitive
|
| Return      : PEI_OK    - function succeeded
|               PEI_ERROR - function failed
+------------------------------------------------------------------------------
*/
static short pei_primitive (void *primitive)
     /*@globals upm_data@*/
{
  (void)TRACE_FUNCTION("UPM pei_primitive");

  if (primitive != NULL)
  {
    T_PRIM_HEADER *prim;
    U32 opc;

    opc = P_OPC(primitive);

/*@i1@*/(void)PTRACE_IN (opc);

    prim = (T_PRIM_HEADER *)&((T_PRIM *)primitive)->data;

    /* Memory supervision ? */
    VSI_PPM_RCV (primitive);

    switch (opc)
    {
#ifndef UPM_WITHOUT_USER_PLANE
      /* DTI SAP */
#ifdef DTI2
      case DTI2_CONNECT_REQ:
        upm_disp_dti_connect_req((T_DTI2_CONNECT_REQ *)(prim));
        /* PFREE is called in the DTILIB */
        break;

      case DTI2_CONNECT_CNF:
        upm_disp_dti_connect_cnf ((T_DTI2_CONNECT_CNF *)(prim));
        /* PFREE is called in the DTILIB */
        break;

      case DTI2_CONNECT_IND:
        upm_disp_dti_connect_ind ((T_DTI2_CONNECT_IND *)(prim));
        /* PFREE is called in the DTILIB */
        break;

      case DTI2_CONNECT_RES:
        upm_disp_dti_connect_res((T_DTI2_CONNECT_RES *)(prim));
        /* PFREE is called in the DTILIB */
        break;

      case DTI2_DISCONNECT_IND:
        upm_disp_dti_disconnect_ind((T_DTI2_DISCONNECT_IND *)(prim));
        /* PFREE is called in the DTILIB */
        break;

      case DTI2_DISCONNECT_REQ:
        upm_disp_dti_disconnect_req((T_DTI2_DISCONNECT_REQ *)(prim));
        /* PFREE is called in the DTILIB */
        break;
#endif /* DTI2 */

      case DTI2_GETDATA_REQ:
        upm_disp_dti_getdata_req((T_DTI2_GETDATA_REQ *)(prim));
        /* PFREE is called in the DTILIB */
        break;

      case DTI2_READY_IND:
        upm_disp_dti_ready_ind ((T_DTI2_READY_IND *)(prim));
        /* PFREE is called in the DTILIB */
        break;

      case DTI2_DATA_REQ:
        upm_disp_dti_data_req((T_DTI2_DATA_REQ *)(prim));
        /* PFREE is called in the upm_sig_callback function in upm_dti_handler.c */
        break;

      case DTI2_DATA_IND:
        upm_disp_dti_data_ind ((T_DTI2_DATA_IND *)(prim));
        /* PFREE is called in the upm_sig_callback function in upm_dti_handler.c */
        break;

#ifdef DEBUG
      case DTI2_DATA_TEST_REQ:
        upm_disp_dti_data_test_req((T_DTI2_DATA_TEST_REQ *)(prim));
        /* PFREE is called in the DTILIB */
        break;
      case DTI2_DATA_TEST_IND:
        upm_disp_dti_data_test_ind((T_DTI2_DATA_TEST_IND *)(prim));
        /* PFREE is called in the DTILIB */
        break;
#endif /* DEBUG */

#endif /* UPM_WITHOUT_USER_PLANE */

      /* SM SAP */
      case SM_ACTIVATE_STARTED_IND:
        upm_disp_sm_activate_started_ind((T_SM_ACTIVATE_STARTED_IND *)(prim));
        PFREE(prim);
        break;

      case SM_ACTIVATE_IND:
        upm_disp_sm_activate_ind((T_SM_ACTIVATE_IND *)(prim));
        PFREE(prim);
        break;

      case SM_DEACTIVATE_IND:
        upm_link_dispatch_sm_deactivate_ind((T_SM_DEACTIVATE_IND *)(prim));
        PFREE(prim);
        break;

      case SM_MODIFY_IND:
        upm_disp_sm_modify_ind((T_SM_MODIFY_IND *)(prim));
        PFREE(prim);
        break;

#ifndef UPM_WITHOUT_USER_PLANE
      /* MMPM SAP */
      case MMPM_SUSPEND_IND:
        upm_disp_mmpm_suspend_ind((T_MMPM_SUSPEND_IND *)(prim));
        PFREE(prim);
        break;

      case MMPM_RESUME_IND:
        upm_disp_mmpm_resume_ind((T_MMPM_RESUME_IND *)(prim));
        PFREE(prim);
        break;
#endif /* UPM_WITHOUT_USER_PLANE */

#ifdef TI_UMTS
      case MMPM_REESTABLISH_CNF:
        upm_disp_mmpm_reestablish_cnf((T_MMPM_REESTABLISH_CNF *)(prim));
        PFREE(prim);
        break;

      case MMPM_REESTABLISH_REJ:
        upm_disp_mmpm_reestablish_rej((T_MMPM_REESTABLISH_REJ *)(prim));
        PFREE(prim);
        break;
#endif /* TI_UMTS */
#ifdef TI_GPRS
      case MMPM_SEQUENCE_IND:
        upm_disp_mmpm_sequence_ind((T_MMPM_SEQUENCE_IND *)(prim));
        PFREE(prim);
        break;
#endif /* TI_GPRS */
#ifdef TI_DUAL_MODE
      case MMPM_RAT_CHANGE_COMPLETED_IND:
        upm_disp_mmpm_rat_change_completed_ind((T_MMPM_RAT_CHANGE_COMPLETED_IND *)(prim));
        PFREE(prim);
        break;

      case MMPM_RAT_CHANGE_IND:
        upm_disp_mmpm_rat_change_ind((T_MMPM_RAT_CHANGE_IND *)(prim));
        PFREE(prim);
        break;
#endif /* TI_DUAL_MODE */

#ifdef TI_UMTS
      /* PDCP SAP */
      case PDCP_DATA_IND:
        upm_disp_pdcp_data_ind((T_PDCP_DATA_IND *)(prim));
        PFREE(prim);
        break;

#ifdef DEBUG
      case PDCP_TEST_DATA_IND:
        upm_disp_pdcp_test_data_ind((T_PDCP_TEST_DATA_IND *)(prim));
        PFREE(prim);
        break;
#endif /* DEBUG */

#ifdef TI_DUAL_MODE
      case PDCP_AM_GET_PENDING_PDU_CNF:
        upm_disp_pdcp_get_pending_pdu_cnf((T_PDCP_AM_GET_PENDING_PDU_CNF *)(prim));
        PFREE(prim);
        break;

#ifdef DEBUG
      case PDCP_TEST_AM_GET_PENDING_PDU_CNF:
        upm_disp_pdcp_test_am_get_pending_pdu_cnf((T_PDCP_TEST_AM_GET_PENDING_PDU_CNF *)(prim));
        PFREE(prim);
        break;
#endif

      /* RRC SAP */
      case RRC_MOVE_USER_DATA_IND:
        upm_disp_rrc_move_user_data_ind((T_RRC_MOVE_USER_DATA_IND *)(prim));
        PFREE(prim);
        break;
#endif /* TI_DUAL_MODE */
      case RRC_RAB_ESTABLISH_IND:
        upm_disp_rrc_rab_establish_ind((T_RRC_RAB_ESTABLISH_IND *)(prim));
        PFREE(prim);
        break;
      case RRC_RAB_ESTABLISH_COMPLETE_IND:
        upm_disp_rrc_rab_establish_complete_ind((T_RRC_RAB_ESTABLISH_COMPLETE_IND *)(prim));
        PFREE(prim);
        break;
      case RRC_RAB_RELEASE_IND:
        upm_disp_rrc_rab_release_ind((T_RRC_RAB_RELEASE_IND *)(prim));
        PFREE(prim);
        break;

      /* MEM SAP */
      case MEM_READY_IND:
        upm_disp_mem_ready_ind((T_MEM_READY_IND *)(prim));
        PFREE(prim);
        break;
#endif /* TI_UMTS */

#ifdef TI_GPRS
      /* SN SAP */
      case SN_ACTIVATE_CNF:
        upm_disp_sn_activate_cnf((T_SN_ACTIVATE_CNF *)(prim));
        PFREE(prim);
        break;

      case SN_COUNT_CNF:
        upm_disp_sn_count_cnf((T_SN_COUNT_CNF *)(prim));
        PFREE(prim);
        break;

      case SN_MODIFY_CNF:
        upm_disp_sn_modify_cnf((T_SN_MODIFY_CNF *)(prim));
        PFREE(prim);
        break;

      case SN_DEACTIVATE_CNF:
        upm_disp_sn_deactivate_cnf((T_SN_DEACTIVATE_CNF *)(prim));
        PFREE(prim);
        break;

      case SN_STATUS_IND:
        /* May affect multiple contexts.  Special dispatch handling. */
        upm_sndcp_dispatch_sn_status_ind((T_SN_STATUS_IND *)(prim));
        PFREE(prim);
        break;

      case SN_SEQUENCE_CNF:
        /* May affect multiple contexts.  Special dispatch handling. */
        upm_sndcp_dispatch_sn_sequence_cnf((T_SN_SEQUENCE_CNF *)(prim));
        PFREE(prim);
        break;

#ifdef TI_DUAL_MODE
      case SN_GET_PENDING_PDU_CNF:
        upm_disp_sn_get_pending_pdu_cnf((T_SN_GET_PENDING_PDU_CNF *)(prim));
        PFREE(prim);
        break;

#ifdef DEBUG
      case SN_TEST_GET_PENDING_PDU_CNF:
        upm_disp_sn_test_get_pending_pdu_cnf((T_SN_TEST_GET_PENDING_PDU_CNF *)(prim));
        PFREE(prim);
        break;
#endif /* DEBUG */

      /* RR SAP */
      case RR_MOVE_USER_DATA_IND:
        upm_disp_rr_move_user_data_ind((T_RR_MOVE_USER_DATA_IND *)(prim));
        PFREE(prim);
        break;
#endif /* TI_DUAL_MODE */
#endif /* TI_GPRS */

      /* UPM SAP */
      case UPM_COUNT_REQ:
        upm_disp_upm_count_req((T_UPM_COUNT_REQ *)(prim));
        PFREE(prim);
        break;

#ifndef UPM_WITHOUT_USER_PLANE
      case UPM_DTI_REQ:
        upm_disp_upm_dti_req((T_UPM_DTI_REQ *)(prim));
        PFREE(prim);
        break;
#endif /* UPM_WITHOUT_USER_PLANE */

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
| Function      : pei_init
+------------------------------------------------------------------------------
| Description   : This function is called by the frame. It is used
|                 to initialise the entity.
|
| Parameters    : handle            - task handle
|
| Return        : PEI_OK            - entity initialised
|                 PEI_ERROR         - entity not (yet) initialised
+------------------------------------------------------------------------------
*/
static short pei_init (T_HANDLE handle)
  /*@globals undef upm_data@*/
  /*@modifies upm_data@*/
{
  (void)TRACE_FUNCTION ("UPM pei_init");

  /* Clear static entity data store */
  memset(&upm_data, 0, sizeof(upm_data));

  /*
   * Initialize task handles
   */
  upm_data.upm_handle = handle;
  hCommACI   = VSI_ERROR;
  hCommSM    = VSI_ERROR;
  hCommMM    = VSI_ERROR;
#ifdef TI_UMTS
  hCommPDCP  = VSI_ERROR;
  hCommRRC   = VSI_ERROR;
#endif /* TI_UMTS */
#ifdef TI_GPRS
  hCommSNDCP = VSI_ERROR;
  hCommRR    = VSI_ERROR;
#endif /* TI_GPRS */

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

  if (hCommSM < VSI_OK)
  {
    if ((hCommSM = vsi_c_open(VSI_CALLER SM_NAME)) < VSI_OK)
    {
      return (short)PEI_ERROR;
    }
  }

  if (hCommMM < VSI_OK)
  {
/*#ifdef UPM_EDGE */
    if ((hCommMM = vsi_c_open(VSI_CALLER GMM_NAME)) < VSI_OK)
/*#else */ /*#ifdef UPM_EDGE*/
/*    if ((hCommMM = vsi_c_open(VSI_CALLER MM_NAME)) < VSI_OK)
#endif */ /*#ifdef UPM_EDGE*/
    {
      return (short)PEI_ERROR;
    }
  }


#ifdef TI_UMTS
  if (hCommPDCP < VSI_OK)
  {
    if ((hCommPDCP = vsi_c_open(VSI_CALLER PDCP_NAME)) < VSI_OK)
    {
      return (short)PEI_ERROR;
    }
  }

  if (hCommRRC < VSI_OK)
  {
    if ((hCommRRC = vsi_c_open(VSI_CALLER RRC_NAME)) < VSI_OK)
    {
      return (short)PEI_ERROR;
    }
  }
#endif /* TI_UMTS */

#ifdef TI_GPRS
  if (hCommSNDCP < VSI_OK)
  {
    if ((hCommSNDCP = vsi_c_open (VSI_CALLER SNDCP_NAME)) < VSI_OK)
    {
      return (short)PEI_ERROR;
    }
  }

  if (hCommRR < VSI_OK)
  {
    if ((hCommRR = vsi_c_open (VSI_CALLER RR_NAME)) < VSI_OK)
    {
      return (short)PEI_ERROR;
    }
  }
#endif /* TI_GPRS */

  /*
   * Initialize entity data (call init functions)
   */

#ifdef TI_UMTS
  mem_init();
#endif /* TI_UMTS */

  /*
   * Initialize DTI
   */
#ifndef UPM_WITHOUT_USER_PLANE
  assert (upm_hDTI == NULL);
  upm_hDTI = dti_init((U8)NAS_SIZE_NSAPI, upm_data.upm_handle,
                      DTI_DEFAULT_OPTIONS, upm_sig_callback);
#endif /* UPM_WITHOUT_USER_PLANE */

  return (short)PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function      : pei_exit
+------------------------------------------------------------------------------
| Description   : This function is called by the frame when the entity
|                 is terminated. All open resources are freed.
|
| Parameters    : -
|
| Return        : PEI_OK            - exit sucessful
|                 PEI_ERROR         - exit not successful
+------------------------------------------------------------------------------
*/
static short pei_exit (void)
{
  int nsapi;
  (void)TRACE_FUNCTION ("UPM pei_exit");

  /*
   * Clean up entity data
   */

  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++) {
    struct T_CONTEXT_DATA *ptr_context_data;

    ptr_context_data = upm_get_context_data_from_nsapi(nsapi);
    if (ptr_context_data != NULL) {
#ifdef TI_GPRS
      upm_sndcp_control_exit        (ptr_context_data);
#endif /* TI_GPRS */
#ifdef TI_UMTS
      upm_rab_control_exit          (ptr_context_data);
#endif
      upm_link_control_exit         (ptr_context_data);
#ifndef UPM_WITHOUT_USER_PLANE
      upm_downlink_data_control_exit(ptr_context_data);
      upm_uplink_data_control_exit  (ptr_context_data);
      upm_dti_control_exit          (ptr_context_data);
#endif /* UPM_WITHOUT_USER_PLANE */

#ifdef DEBUG
 #ifdef TI_UMTS
      upm_free_mem_buffer(ptr_context_data);
 #endif
#endif
      upm_free_context_data(nsapi);
    }
  }
#ifdef TI_DUAL_MODE
  upm_rat_control_exit();
#endif

#ifndef UPM_WITHOUT_USER_PLANE
  /*
   * Clean-up DTILIB
   */
  dti_deinit(upm_hDTI);

  /* Disable forced DTI neighbor routing */
#ifdef DEBUG
  upm_disp_force_neighbor(NULL);
#endif
#endif /* UPM_WITHOUT_USER_PLANE */

  /*
   * Close communication channels
   */
  (void)vsi_c_close(VSI_CALLER hCommACI);
  hCommACI   = VSI_ERROR;

  (void)vsi_c_close(VSI_CALLER hCommSM);
  hCommSM    = VSI_ERROR;

  (void)vsi_c_close(VSI_CALLER hCommMM);
  hCommMM    = VSI_ERROR;

#ifdef TI_UMTS
  (void)vsi_c_close(VSI_CALLER hCommPDCP);
  hCommPDCP  = VSI_ERROR;

  (void)vsi_c_close(VSI_CALLER hCommRRC);
  hCommRRC   = VSI_ERROR;
#endif /* TI_UMTS */

#ifdef TI_GPRS
  (void)vsi_c_close(VSI_CALLER hCommSNDCP);
  hCommSNDCP = VSI_ERROR;

  (void)vsi_c_close(VSI_CALLER hCommRR);
  hCommRR    = VSI_ERROR;
#endif /* TI_GPRS */

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function              : pei_config
+------------------------------------------------------------------------------
| Description   : This function is called by the frame when a primitive is
|                 received indicating dynamic configuration.
|
|                 This function is not used in this entity.
|
| Parameters    :       handle            - Communication handle
|
| Return      : PEI_OK            - sucessful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
static short pei_config (char *inString)
{
#ifdef DEBUG
  const size_t neighbor_strlen = strlen("NEIGHBOR_ENTITY");
#endif
  (void)TRACE_FUNCTION ("UPM pei_config");
  (void)TRACE_FUNCTION (inString);

#ifdef DEBUG

#ifdef TI_UMTS
  if ( strcmp(inString, "MEM_FLOW_CONTROL ON") == 0)
  {
    upm_disp_adjust_mem_flow_control(TRUE);
  }
  else if ( strcmp(inString, "MEM_FLOW_CONTROL OFF") == 0)
  {
    upm_disp_adjust_mem_flow_control(FALSE);
  }
  else if ( strncmp(inString, "NEIGHBOR_ENTITY", neighbor_strlen) == 0)
  {
    if (strncmp(&inString[neighbor_strlen + 1], "CLEAR", sizeof("CLEAR")) != 0)
    {
      upm_disp_force_neighbor(&inString[neighbor_strlen + 1]);
    } else {
      upm_disp_force_neighbor(NULL);
    }
  } else
#endif /* TI_UMTS */
  if ( strcmp(inString, "LL_ENTITY_TEST") == 0)
  {
    upm_activate_ll_entity_test();
  }
#endif /* DEBUG */

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function      : pei_monitor
+------------------------------------------------------------------------------
| Description   : This function is called by the frame in case sudden entity
|                 specific data is requested (e.g. entity Version).
|
| Parameters    :  out_monitor       - return the address of the data to be
|                                      monitoredCommunication handle
|
| Return        :  PEI_OK            - sucessful (address in out_monitor is valid)
|                  PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
/*@-compdef@*/ /*@-mods@*/
static short pei_monitor (void **out_monitor) /*@globals undef upm_mon@*/
{
  (void)TRACE_FUNCTION ("UPM pei_monitor");

  /*
   * Version = "0.S" (S = Step).
   */
  upm_mon.version = "UPM 0.1";
  *out_monitor = &upm_mon;

  return PEI_OK;
}
/*@=compdef@*/ /*@=mods@*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function              : pei_create
+------------------------------------------------------------------------------
| Description   :  This function is called by the frame when the process is 
|                  created.
|
| Parameters    :  out_name          - Pointer to the buffer in which to locate
|                                      the name of this entity
|
| Return        :  PEI_OK            - entity created successfuly
|                  PEI_ERROR         - entity could not be created
|
+------------------------------------------------------------------------------
*/
short pei_create (T_PEI_INFO **info)
     /*@globals first_access@*/ /*@modifies first_access@*/
{
/*@-nullassign@*/
static T_PEI_INFO pei_info =
              {
/*@i1@*/       UPM_NAME,      /* name */
               {              /* pei-table */
                 pei_init,
                 pei_exit,
                 pei_primitive,
                 NULL,        /* no pei_timeout function */
                 NULL,        /* no pei_signal function */
                 NULL,        /* no pei_run function */
                 pei_config,
                 pei_monitor
               },
               (U32)1024,     /* stack size */
               (U16)20,       /* queue entries */
               (U16)216,      /* priority (1->low, 255->high) */
               (U16)0,        /* number of timers */
               (U8)0x03       /* flags: bit 0   active(0) body/passive(1) */
              };              /*        bit 1   com by copy(0)/reference(1) */
/*@+nullassign@*/

  (void)TRACE_FUNCTION ("UPM pei_create");

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
