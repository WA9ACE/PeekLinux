/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  
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
|  Purpose :  Definitions for llc_uf.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_UF_H
#define LLC_UF_H


/*==== XID DEFINITIONS ======================================================*/

/*
 * XID parameter types
 */
#define XID_VERSION           0
#define XID_IOV_UI            1
#define XID_IOV_I             2
#define XID_T200              3
#define XID_N200              4
#define XID_N201_U            5
#define XID_N201_I            6
#define XID_MD                7
#define XID_MU                8
#define XID_KD                9
#define XID_KU                10
#define XID_LAYER_3           11
#define XID_RESET             12

#define XID_MAX_TYPE          XID_RESET

/*
 * XID parameter lengths in octets
 */
#define XID_VERSION_LEN       1
#define XID_IOV_UI_LEN        4
#define XID_IOV_I_LEN         4
#define XID_T200_LEN          2
#define XID_N200_LEN          1
#define XID_N201_U_LEN        2
#define XID_N201_I_LEN        2
#define XID_MD_LEN            2
#define XID_MU_LEN            2
#define XID_KD_LEN            1
#define XID_KU_LEN            1
/* no default length for layer 3 parameters */
#define XID_RESET_LEN         0

/*
 * XID parameter header lengths in octets
 */
#define XID_VERSION_HDR_LEN   1
#define XID_IOV_UI_HDR_LEN    2
#define XID_IOV_I_HDR_LEN     2
#define XID_T200_HDR_LEN      1
#define XID_N200_HDR_LEN      1
#define XID_N201_U_HDR_LEN    1
#define XID_N201_I_HDR_LEN    1
#define XID_MD_HDR_LEN        1
#define XID_MU_HDR_LEN        1
#define XID_KD_HDR_LEN        1
#define XID_KU_HDR_LEN        1
#define XID_LAYER_3_HDR_LEN   2
#define XID_RESET_HDR_LEN     1

/*
 * XID parameter ranges
 */
#define XID_VERSION_MIN       0u
#define XID_VERSION_MAX       15u
#define XID_IOV_UI_MIN        0u
#define XID_IOV_UI_MAX        4294967295uL        /* actually: 2 ^ 32 - 1 */
#define XID_IOV_I_MIN         0u
#define XID_IOV_I_MAX         4294967295uL        /* actually: 2 ^ 32 - 1 */
#define XID_T200_MIN          1u                   /* units: 0.1 seconds */
#define XID_T200_MAX          4095u
#define XID_N200_MIN          1u
#define XID_N200_MAX          15u
#ifdef _SIMULATION_
#ifdef MULTI_LAYER
                                                  /* units: octets */
#define XID_N201_U_MIN        (llc_data->current_sapi EQ LL_SAPI_1 ? 400u :    \
                               llc_data->current_sapi EQ LL_SAPI_7 ? 270u : 59u)
#else
#define XID_N201_U_MIN        (llc_data->current_sapi EQ LL_SAPI_1 ? 400u :    \
                               llc_data->current_sapi EQ LL_SAPI_7 ? 270u : 140u)

#endif /* MULTI_LAYER */
#else
                                                  /* units: octets */
#define XID_N201_U_MIN        (llc_data->current_sapi EQ LL_SAPI_1 ? 400u :    \
                               llc_data->current_sapi EQ LL_SAPI_7 ? 270u : 140u)
#endif /* SIMULATION */
#define XID_N201_U_MAX        1520u
#ifdef _SIMULATION_
#ifdef MULTI_LAYER
#define XID_N201_I_MIN        59u                 /* units: octets */
#else
#define XID_N201_I_MIN        140u                 /* units: octets */

#endif /* MULTI_LAYER */
#else
#define XID_N201_I_MIN        140u                 /* units: octets */
#endif /* SIMULATION */
#define XID_N201_I_MAX        1520u
#define XID_MD_OFF            0u                   
#define XID_MD_MIN            9u                   /* units: 16 octets */
#define XID_MD_MAX            24320u
#define XID_MU_OFF            0u                   
#define XID_MU_MIN            9u                   /* units: 16 octets */
#define XID_MU_MAX            24320u
#define XID_KD_MIN            1u                   /* units: frames */
#define XID_KD_MAX            255u
#define XID_KU_MIN            1u                   /* units: frames */
#define XID_KU_MAX            255u

/*
 * Conditions for checking correct sense of negotiation.
 */
#define XID_SENSE_UP          >
#define XID_SENSE_DOWN        <


/*==== FUNCTION DECLARATIONS ================================================*/

EXTERN void   u_init                   (void);
EXTERN void   u_init_sapi              (void);

EXTERN void   u_build_u_frame          
(
#ifdef LL_DESC
  T_desc_list3 *desc_list3,
#else
  T_sdu *sdu, 
#endif
  T_BIT cr_bit, 
  UBYTE sapi, 
  T_BIT pf_bit, 
  T_COMMAND command
);

EXTERN BOOL   u_check_xid              (T_sdu *sdu, 
                                        T_BIT cr_bit, 
                                        T_COMMAND command);
EXTERN void   u_eval_xid               (T_BIT cr_bit,
                                        BOOL *reset_received,
                                        BOOL *xid_ind);

EXTERN void   u_insert_xid             
(
#ifdef LL_DESC
  T_desc_list3 *desc_list3,
#else
  T_sdu *sdu,
#endif
  T_BIT cr_bit
);

EXTERN void   u_insert_frmr_information 
(
#ifndef LL_DESC
  T_sdu *sdu,
#else
  T_desc_list3 *desc_list3,
#endif
  T_LL_UNITDATA_IND *frame,
  T_PDU_TYPE pdu_type,
  USHORT ctrl_length,
  T_FRAME_NUM vs,
  T_FRAME_NUM vr,
  T_BIT cr_bit,
  UBYTE reason
);

EXTERN void   u_send_sabm              (void);
EXTERN void   u_send_disc              (void);

#ifdef REL99 
  EXTERN void   sig_tx_u_send_null       (UBYTE cause);
#endif /* REL99 */

EXTERN void   u_send_ua                (T_BIT pf_bit,
                                        BOOL include_xid);
EXTERN void   u_send_dm                (T_BIT pf_bit);
EXTERN void   u_send_frmr              (T_LL_UNITDATA_IND *frame,
                                        T_PDU_TYPE pdu_type,
                                        USHORT ctrl_length,
                                        T_FRAME_NUM vs,
                                        T_FRAME_NUM vr,
                                        T_BIT cr_bit,
                                        UBYTE reason);
#ifdef CC_CONCEPT
EXTERN void   u_send_llgmm_status_ind  (USHORT error_cause);
EXTERN void   u_send_ll_status_ind     (USHORT cause);
EXTERN void   u_send_ll_release_ind    (USHORT cause);
#else
EXTERN void   u_send_llgmm_status_ind  (UBYTE error_cause);
EXTERN void   u_send_ll_status_ind     (UBYTE cause);
EXTERN void   u_send_ll_release_ind    (UBYTE cause);
#endif

EXTERN void   u_send_ll_release_cnf    (void);
EXTERN void   u_send_ll_xid_cnf        (void);
EXTERN void   u_send_ll_establish_ind  (void);
EXTERN void   u_send_ll_establish_cnf  (void);
EXTERN void   u_send_ll_xid_ind        (void);
EXTERN void   u_send_xid               (T_BIT cr_bit);
EXTERN void   u_tag_xid_parameters     (T_BIT cr_bit, BOOL send_ack_para);
EXTERN void   u_handle_optimization    (void);

EXTERN void   u_handle_mX_zero_rsp     (USHORT mX_req, USHORT* n201_i, 
                                        USHORT mX,     USHORT  mX_max,    
                                        UBYTE* kX,     UBYTE   kX_min, 
                                        UBYTE  kX_type );
EXTERN void   u_check_requested_xid    (void);

#endif /* !LLC_UF_H */
