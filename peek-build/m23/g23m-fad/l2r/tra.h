/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  tra.h
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
|  Purpose :  Definitions for the Protocol Stack Entity
|             TRA
+----------------------------------------------------------------------------- 
*/ 

#ifndef TRA_H
#define TRA_H

/*
 * States of the entity process MGT
 */
#define MGT_IDLE            0
#define MGT_INACTIVE        1
#define MGT_ACTIVE          2

 /*
 * States of the entity processes UP & DN
 */
#define UP_INACTIVE 0
#define UP_IDLE     1
#define UP_WAIT     2
#define UP_SEND     3

#define DN_INACTIVE 0
#define DN_IDLE     1
#define DN_WAIT     2
#define DN_SEND     3

/*==== TYPES ======================================================*/

/*
* derived constants
*/

/* global user specific constants */

#define FL_INACTIVE 0
#define FL_ACTIVE   1

/*
* Bitoffset for encoding/decoding, taken from RLP
*/
#define ENCODE_OFFSET 0

typedef enum
{
  NONTRANSP = 0,
  TRANSP    = 1
} T_MODE;


/*lint -e767 macro 'ENTITY_DATA' was defined differently in another module */
#define ENTITY_DATA tra_data

/*
 *  MANAGEMENT signal processing
 */
#ifdef OPTION_MULTITHREAD
#define sig_mgt_tra_up_send_break_req _ENTITY_PREFIXED(sig_mgt_tra_up_send_break_req)
#define sig_mgt_tra_up_break_req      _ENTITY_PREFIXED(sig_mgt_tra_up_break_req)
#define sig_mgt_tra_dn_break_req      _ENTITY_PREFIXED(sig_mgt_tra_dn_break_req)

#define sig_any_tra_dn_send_break_req _ENTITY_PREFIXED(sig_any_tra_dn_send_break_req)
#define sig_up_tra_mgt_break_ind      _ENTITY_PREFIXED(sig_up_tra_mgt_break_ind)
#define sig_dn_tra_mgt_break_ind      _ENTITY_PREFIXED(sig_dn_tra_mgt_break_ind)
#endif

EXTERN void sig_mgt_tra_up_send_break_req(USHORT break_len);
EXTERN void sig_mgt_tra_up_break_req(void);
EXTERN void sig_mgt_tra_dn_break_req(void);

EXTERN void sig_any_tra_dn_send_break_req (void);

EXTERN void sig_up_tra_mgt_break_ind(USHORT break_len);
EXTERN void sig_dn_tra_mgt_break_ind(U8 sa, U8 sb, U8 flow, USHORT break_len);

/*
 *  MANAGEMENT procedures
 */

EXTERN void send_tra_deactivate_cnf(void);
EXTERN void send_tra_dti_cnf(T_TRA_DTI_REQ *tra_dti_req);

/*
 *  DN signal processing
 */

#ifdef OPTION_MULTITHREAD
#define sig_mgt_tra_dn_dti_conn_setup _ENTITY_PREFIXED(sig_mgt_tra_dn_dti_conn_setup)
#define sig_mgt_tra_dn_dti_conn_open  _ENTITY_PREFIXED(sig_mgt_tra_dn_dti_conn_open)
#define sig_mgt_tra_dn_dti_disc       _ENTITY_PREFIXED(sig_mgt_tra_dn_dti_disc)
#endif

EXTERN void sig_mgt_tra_dn_dti_conn_setup(void);
EXTERN void sig_mgt_tra_dn_dti_conn_open(void);
EXTERN void sig_mgt_tra_dn_dti_disc(void);

/*
 *  DN procedures
 */
#ifdef OPTION_MULTITHREAD
#define dl_init                       _ENTITY_PREFIXED(dl_init)
#define rbm_init                      _ENTITY_PREFIXED(rbm_init)
#define dl_send_data_ind              _ENTITY_PREFIXED(dl_send_data_ind )
#define dl_prep_data_ind              _ENTITY_PREFIXED(dl_prep_data_ind)
#endif

EXTERN void dl_init(T_TRA_DN *ddl);
EXTERN void rbm_init(T_TRA_DN *ddl);

EXTERN BOOL dl_send_data_ind (void);
EXTERN void dl_prep_data_ind (T_FD *pFD);

/*
 *  UP signal processing
 */
#ifdef OPTION_MULTITHREAD
#define sig_mgt_tra_up_dti_conn_setup _ENTITY_PREFIXED(sig_mgt_tra_up_dti_conn_setup)
#define sig_mgt_tra_up_dti_conn_open _ENTITY_PREFIXED(sig_mgt_tra_up_dti_conn_open)
#define sig_mgt_tra_up_dti_disc _ENTITY_PREFIXED(sig_mgt_tra_up_dti_disc)
#endif

EXTERN void sig_mgt_tra_up_dti_conn_setup(void);
EXTERN void sig_mgt_tra_up_dti_conn_open(void);
EXTERN void sig_mgt_tra_up_dti_disc(void);

/*
 *  UP procedures
 */
#ifdef OPTION_MULTITHREAD
#define ul_init                   _ENTITY_PREFIXED(ul_init          )
#define snd_data_to_RA            _ENTITY_PREFIXED(snd_data_to_RA   )
#define snd_break_to_RA           _ENTITY_PREFIXED(snd_break_to_RA  )
#define up_start_dti_flow         _ENTITY_PREFIXED(up_start_dti_flow)
#endif

EXTERN void ul_init(T_TRA_UP *dul);
EXTERN void snd_data_to_RA();
EXTERN void snd_break_to_RA(USHORT break_len);
EXTERN void up_start_dti_flow(void);

EXTERN T_MODE l2r_data_mode;

/*
* number of signals (max. signalNum which occurs in pei_signal)
*/

EXTERN DTI_HANDLE l2r_hDTI; /* DTI connection handle for DTI library      */

EXTERN T_FD *tra_get_next_FrameDesc(void);

#endif
