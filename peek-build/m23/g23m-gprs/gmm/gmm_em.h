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
|  Purpose : Engineering Mode (EM) Declarations + Macros
|
+-----------------------------------------------------------------------------
*/


#ifndef GMM_EM_H
#define GMM_EM_H


#ifdef FF_EM_MODE
/*also defined in rr_em.h*/

/*
 * buffer size has been chosen to store ten state events for now.
 */
#define EM_GMM_BUFFER_SIZE             5

/*
 * EM_MAX_GMM_EVENTS defines maximum number of event traces for the engineering 
 * mode. The number is increased by one to ensure that the event numbers 
 * defined in the corresponding document are the same as in the sources.
 */
#define EM_MAX_GMM_EVENTS              2

/*
 *  The offset is used to indicate the source entity the event trace is from.
 *  L1/ALR = 0x00, DL = 0x2D, GRLC = 0x33, RR = 0x37, MM = 0x64, CC = 0x78, 
 *  GMM=0xAA, GRR=0xAC, SS = 0xAF, SMS = 0xBE, SIM = E1
 */
#define GMM_OFFSET                   0xAA

#define TRACE_EVENT_EM_P8(s,a1,a2,a3,a4,a5,a6,a7,a8)  TRACE_USER_CLASS_P8(TC_USER8,s,a1,a2,a3,a4,a5,a6,a7,a8)
#define GMM_EM_NULL_STATE  0x0
#define GMM_EM_DEREG_STATE 0x1
#define GMM_EM_REG_INIT_STATE 0x2
#define GMM_EM_REG_STATE 0x3
#define GMM_EM_RAU_INIT_STATE 0x4
#define GMM_EM_DEREG_INIT_STATE 0x5

#define GMM_EM_NORMAL_SRVC_SUBSTATE 0x0
#define GMM_EM_NO_CELL_SUBSTATE 0x1
#define GMM_EM_SUSPENDED_SUBSTATE 0x2
#define GMM_EM_ATTEMPT_TO_ATT_SUBSTATE 0x3
#define GMM_EM_ATT_NEEDED_SUBSTATE 0x4
#define GMM_EM_STATUS_IND_SUBSTATE 0x5
#define GMM_EM_IMSI_DETACH_INIT_SUBSTATE 0x6
#define GMM_EM_ATTEMPT_TO_UPDATE_MM_SUBSTATE 0x7
#define GMM_EM_ATTEMPT_TO_UPDATE_SUBSTATE 0x8
#define GMM_EM_UPDATE_NEEDED_SUBSTATE 0x9
#define GMM_EM_POWER_OFF_SUBSTATE 0xA

#define GMM_V_1          (1+GMM_OFFSET)

#define TRACE_EVENT_EM_P2(s,a1,a2)              TRACE_USER_CLASS_P2(TC_USER8,s,a1,a2)
#define GMM_V_1          (1+GMM_OFFSET)

EXTERN void em_gmm_pco_trace_req    ( T_EM_PCO_TRACE_REQ    *em_pco_trace_req);

EXTERN BOOL gmm_v[EM_MAX_GMM_EVENTS];

EXTERN UBYTE em_gmm_event_buffer[EM_GMM_BUFFER_SIZE];
EXTERN UBYTE em_gmm_buffer_write;

EXTERN void em_gmm_sc_gprs_info_req ( T_EM_SC_GPRS_INFO_REQ *em_sc_gprs_info_req);
EXTERN void em_gmm_sc_gprs_info_cnf ( T_EM_SC_GPRS_INFO_CNF *em_sc_gprs_info_cnf);
EXTERN void em_gmm_info_req         ( T_EM_GMM_INFO_REQ     *em_gmm_info_req);
EXTERN void em_gmm_grlc_info_cnf    ( T_EM_GRLC_INFO_CNF    *em_grlc_info_cnf);
EXTERN void em_gmm_grlc_info_req    ( T_EM_GRLC_INFO_REQ    *em_grlc_info_req);

#define EM_GMM_SET_STATE(s) \
  {\
    UBYTE gmm_main_state, gmm_sub_state;\
    em_gmm_map_state(s,&gmm_main_state,&gmm_sub_state);\
    if (gmm_v[1])\
    {\
      gmm_v[1] = em_write_buffer_4 (GMM_V_1,gmm_main_state,gmm_sub_state);\
    } \
  }
EXTERN void em_gmm_throughput_info_req ( T_EM_THROUGHPUT_INFO_REQ *em_throughput_info_req);
EXTERN void em_gmm_throughput_info_cnf ( T_EM_THROUGHPUT_INFO_CNF *em_throughput_info_cnf);
EXTERN void em_gmm_grr_event_req ( T_EM_GRR_EVENT_REQ *em_grr_event_req);
EXTERN void em_gmm_grlc_event_req( T_EM_GRLC_EVENT_REQ *em_grlc_event_req);
EXTERN void em_gmm_data_ind ( T_EM_DATA_IND *em_data_ind);


EXTERN       void em_init_gmm_event_trace    (void);

#ifdef OPTION_MULTITHREAD
  #define em_write_buffer_4     _ENTITY_PREFIXED(em_write_buffer_4)
  #define check_write_index     _ENTITY_PREFIXED(check_write_index)
#endif /*OPTION_MULTITHREAD*/

EXTERN UBYTE em_write_buffer_4 (UBYTE event_no, UBYTE value1, UBYTE value2);

EXTERN void em_gmm_event_req (T_EM_GMM_EVENT_REQ *em_gmm_event_req);


#else
#define EM_GMM_SET_STATE                                 /* Event 1*/

#endif /* FF_EM_MODE */

#endif /* GMM_EM_H */
