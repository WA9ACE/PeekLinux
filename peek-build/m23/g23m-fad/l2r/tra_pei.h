/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  tra_pei.h
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
|  Purpose :  Definitions for the TRA Entity, for l2r_pei exclusively!
|             TRA
+----------------------------------------------------------------------------- 
*/ 

#ifndef TRA_PEI_H
#define TRA_PEI_H

#define ENTITY_L2R

/*
 * Constants
 */

/*
 * information for dti library
 */
#define TRA_DTI_UP_INSTANCE  0
#define TRA_DTI_UP_INTERFACE 1
#define TRA_DTI_UP_CHANNEL   0

#define TRA_DTI_BUF_SIZE  400  /* UL buffer size for max dti_data_ind */
#define MAX_SDU_SIZE 	  36   /* 14400/8/50	*/

/**********************************************************************************/
/*
 * process global data
 */

#ifndef NTRACE

/* for trace purposes */

#define SERVICE_NAME_DTI "DTI"
#define SERVICE_NAME_MGT "MGT"
#define SERVICE_NAME_UP  "UP"
#define SERVICE_NAME_DN  "DN"

#endif

#define DTI     dti.
#define MGT     mgt.
#define UP      up.
#define DN      dn.

/**********************************************************************************/
/*
 * data for process downlink
 */

#define TRA_DLR_DEPTH 6 /* ring buffer depth */

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *name;
  char *state_name;
#endif
  U8              sa;           /* last sa bit in this primitive  */
  U8              sb;           /* last sb bit in this primitive  */
  U8              x;            /* last x bit in this primitive   */
  T_DTI2_DATA_IND *prim;        /* NULL if no DTI2_DATA_IND allocated */
  T_desc2         *list_end;    /* NULL if no DTI2_DATA_IND allocated */
  T_RIBU_FD       *ribu;

  T_DTI2_DATA_IND *Brk_dti_data_ind; /* data indication for relaying break indication */
  } T_TRA_DN;

/**********************************************************************************/
/*
 * data for process uplink
 */

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *name;
  char *state_name;
#endif
  U8              sa;           /* last sa bit in this primitive  */
  U8              sb;           /* last sb bit in this primitive  */
  U8              x;            /* last x bit in this primitive   */
  UBYTE           req_frames;   /* amount of data RA is able to handle at next RA_DATA_REQ*/
  T_DTI2_DATA_REQ *Prim;        /* For DTI interface to hold last received primitive */
  USHORT          List_off;     /* offset to the remaining data to be sent */
  T_FD            fd;
  UBYTE           to_ra[MAX_SDU_SIZE]; /* data field to be handed to RA*/
} T_TRA_UP;

/**********************************************************************************/
/*
 * data for process management
 */

/**********************************************************************************/

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *name;
  char *state_name;
#endif
} T_SUB_STATE_TRA;

typedef struct
{
  T_SUB_STATE_TRA dti;  /* state variable for DTI library */
  T_SUB_STATE_TRA mgt;  /* process management */
  T_TRA_UP        up;   /* process uplink */
  T_TRA_DN        dn;   /* process downlink */
} T_TRA_DATA;

/*==== EXPORT =====================================================*/
/*
 * data base
 */

EXTERN T_TRA_DATA *tra_data;
EXTERN T_TRA_DATA *tra_data_first_elem;     /* always on element 0 of l2r_data_base */

/*
 *  Prototypes
 *
 *  MANAGEMENT primitive processing
 */

#ifdef OPTION_MULTITHREAD
  #define mgt_tra_activate_req   _ENTITY_PREFIXED(mgt_tra_activate_req)
  #define mgt_tra_deactivate_req _ENTITY_PREFIXED(mgt_tra_deactivate_req)
  #define mgt_tra_dti_req        _ENTITY_PREFIXED(mgt_tra_dti_req)
#endif

EXTERN void mgt_tra_activate_req   (T_TRA_ACTIVATE_REQ   *tra_activate_req);
EXTERN void mgt_tra_deactivate_req (T_TRA_DEACTIVATE_REQ *tra_deactivate_req);
EXTERN void mgt_tra_dti_req        (T_TRA_DTI_REQ        *tra_dti_req);

#ifdef OPTION_MULTITHREAD
  #define rcv_ra_ready_ind _ENTITY_PREFIXED(rcv_ra_ready_ind)
  #define rcv_ra_data_ind  _ENTITY_PREFIXED(rcv_ra_data_ind)
  #define rcv_ra_break_ind _ENTITY_PREFIXED(rcv_ra_break_ind)
#endif

EXTERN void rcv_ra_ready_ind (T_RA_READY_IND *ra_ready_ind);
EXTERN void rcv_ra_data_ind  (T_RA_DATA_IND  *ra_data_ind);
EXTERN void rcv_ra_break_ind (T_RA_BREAK_IND *ra_break_ind);

#ifdef OPTION_MULTITHREAD
#define sig_dti_tra_mgt_connection_opened_ind                   \
        _ENTITY_PREFIXED(sig_dti_tra_mgt_connection_opened_ind)
#define sig_dti_tra_mgt_connection_closed_ind                   \
        _ENTITY_PREFIXED(sig_dti_tra_mgt_connection_closed_ind)
#define sig_dti_tra_dn_tx_buffer_full_ind                       \
        _ENTITY_PREFIXED(sig_dti_tra_dn_tx_buffer_full_ind    )
#define sig_dti_tra_dn_tx_buffer_ready_ind                      \
        _ENTITY_PREFIXED(sig_dti_tra_dn_tx_buffer_ready_ind   )
#define sig_dti_tra_up_data_received_ind                        \
        _ENTITY_PREFIXED(sig_dti_tra_up_data_received_ind     )
#endif

/*
 * these functions are called by pei_sig_callback
 */
EXTERN void sig_dti_tra_mgt_connection_opened_ind();
EXTERN void sig_dti_tra_mgt_connection_closed_ind();
EXTERN void sig_dti_tra_dn_tx_buffer_full_ind();
EXTERN void sig_dti_tra_dn_tx_buffer_ready_ind();
EXTERN void sig_dti_tra_up_data_received_ind(T_DTI2_DATA_REQ *dti_data_req);

/*
 * Communication handles
 */

#ifdef OPTION_MULTITHREAD
  #define hCommCTRL _ENTITY_PREFIXED(hCommCTRL)
#ifdef _SIMULATION_
  #define hCommRA   _ENTITY_PREFIXED(hCommRA)
#endif
#endif

EXTERN T_HANDLE l2r_handle;
EXTERN T_HANDLE hCommCTRL; /* Controller Communication */

#ifdef _SIMULATION_
EXTERN T_HANDLE hCommRA; /* RA Communication */
#endif

#endif
