/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  RA_DLL
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
|  Purpose :  This Modul defines the L1 scheduler interface functions
|             for the component RA of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#ifndef RA_DLL_C
#define RA_DLL_C
#endif

#define ENTITY_RA

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
#include "pconst.cdg"
#include "custom.h"
#include "gsm.h"
#include "cnf_ra.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "ccdapi.h"
#include "ra.h"

/*==== CONST ======================================================*/
/*==== TYPES ======================================================*/
/*==== VARIABLES ==================================================*/

GLOBAL T_RA_DATA ra_data_base[1];

/*
 * state-controlled jump tables for each communication model
 */

LOCAL const T_VOID_FUNC tra_uplink_func[TRA_STATES] =
{
  tra_ul_null,           //  TRA_NULL           
  tra_ul_init_l1,        //  TRA_INIT_L1        
  tra_idle,              //  TRA_SYNCH_TCH_START
  tra_ul_sync_tch_found, //  TRA_SYNCH_TCH_FOUND
  tra_idle,              //  TRA_WAIT_SYNC_LOST 
  tra_idle,              //  TRA_DATA_TRANS_PRE1 
  tra_idle,              //  TRA_DATA_TRANS_PRE2 
  tra_idle,              //  TRA_DATA_TRANS_PRE3 
  tra_ul_data_trans      //  TRA_DATA_TRANS     
};     

LOCAL const T_VOID_FUNC tra_downlink_func[TRA_STATES] =
{
  tra_idle,                 //  TRA_NULL           
  tra_idle,                 //  TRA_INIT_L1        
  tra_dl_sync_tch_start,    //  TRA_SYNCH_TCH_START
  tra_idle,                 //  TRA_SYNCH_TCH_FOUND
  tra_dl_wait_sync_lost,    //  TRA_WAIT_SYNC_LOST 
  tra_dl_wait_for_connect,  //  TRA_DATA_TRANS_PRE1 
  tra_dl_wait_for_uart,     //  TRA_DATA_TRANS_PRE2 
  tra_dl_got_uart,          //  TRA_DATA_TRANS_PRE3
  tra_dl_data_trans         //  TRA_DATA_TRANS     
};

#ifdef FF_FAX

LOCAL const T_VOID_FUNC fax_uplink_func[FAX_STATES] =
{
  fax_ul_null,           // FAX_NULL           
  fax_ul_init_l1,        // FAX_INIT_L1        
  fax_idle,              // FAX_SYNCH_TCH_START
  fax_ul_sync_tch_found, // FAX_SYNCH_TCH_FOUND
  fax_idle,              // FAX_WAIT_SYNC_LOST 
  fax_idle,              // FAX_DATA_TRANS_PRE 
  fax_ul_data_trans,     // FAX_DATA_TRANS     
  fax_ul_modify          // FAX_MODIFY         
};

LOCAL const T_VOID_FUNC fax_downlink_func[FAX_STATES] =
{
  fax_idle,              // FAX_NULL           
  fax_idle,              // FAX_INIT_L1        
  fax_dl_sync_tch_start, // FAX_SYNCH_TCH_START
  fax_idle,              // FAX_SYNCH_TCH_FOUND
  fax_dl_wait_sync_lost, // FAX_WAIT_SYNC_LOST 
  fax_idle,              // FAX_DATA_TRANS_PRE 
  fax_dl_data_trans,     // FAX_DATA_TRANS     
  fax_idle               // FAX_MODIFY         
};

#endif /* FF_FAX */

LOCAL const T_VOID_FUNC rlp_uplink_func[RLP_STATES] =
{
  rlp_ul_null,      // RLP_NULL
  rlp_ul_init_l1,   // RLP_INIT_L1
  rlp_ul_data_trans // RLP_DATA_TRANS
};

LOCAL const T_VOID_FUNC rlp_downlink_func[RLP_STATES] =
{
  rlp_idle,         // RLP_NULL
  rlp_idle,         // RLP_INIT_L1
  rlp_dl_data_trans // RLP_DATA_TRANS
};

/*==== FUNCTIONS ==================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_DLL                   |
| STATE   : code                  ROUTINE : TimeOut                  |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL BOOL TimeOut(USHORT *timer)
{
  if (*timer NEQ 0) /* timer running */
  {
    if (*timer > ra_data->cycle_time)
      *timer -= ra_data->cycle_time;
    else
      *timer = 0;

#ifdef _SIMULATION_
    TRACE_EVENT_P1("RA_TIMER=%d", *timer);
#endif

    if (*timer EQ 0)
      return TRUE; /* timer expired */
    else
      return FALSE; /* timer running */
  }
  return FALSE; /* timer not running */
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_DLL                   |
| STATE   : code                  ROUTINE : dll_init                 |
+--------------------------------------------------------------------+

  PURPOSE : This function is called from the L2/3 in case of
            activation/deactivation of data services.
*/

GLOBAL void dll_init(void)
{
  ra_data = &ra_data_base[0];
  memset (ra_data, 0, sizeof(T_RA_DATA));
  memset ((char *)ra_data->shm.shm_addr, 0, sizeof (ra_data->shm.shm_addr));
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_DLL                   |
| STATE   : code                  ROUTINE : dll_data_ul              |
+--------------------------------------------------------------------+

  PURPOSE : This function is called from the L1 scheduler every 20ms.
            This function writes the control words
            and/or the uplink data into the shared memory area
            which is the interface between the DSP layer1
            and the MCU layer2. This function is used for all kind
            of data transmission (FAX/TRANSPARENTDATA/RLP)
*/

GLOBAL void dll_data_ul
            (
              USHORT *_ul_buffer_address,
              USHORT *_d_ra_conf,
              USHORT *_d_ra_act,
              USHORT *_d_ra_test,
              USHORT *_d_ra_statu,
              USHORT *_d_fax
            )
{
  if (ra_data->activated EQ FALSE)
  {
    return;
  }

  switch (ra_data->model)
  {
    case MODEL_RLP:
      if (ra_data->shm.shm_addr[a_ntu] EQ NULL)
      {
        ra_data->shm.shm_addr[a_ntu]      = _ul_buffer_address;
        ra_data->shm.shm_addr[d_ra_conf]  = _d_ra_conf;
        ra_data->shm.shm_addr[d_ra_act]   = _d_ra_act;
      }
      rlp_uplink_func[ra_data->ker.state]();
      break;

    case MODEL_TRANS:
      if (ra_data->shm.shm_addr[a_rau] EQ NULL)
      {
        ra_data->shm.shm_addr[a_rau]      = _ul_buffer_address;
        ra_data->shm.shm_addr[d_ra_conf]  = _d_ra_conf;
        ra_data->shm.shm_addr[d_ra_act]   = _d_ra_act;
        ra_data->shm.shm_addr[d_ra_statu] = _d_ra_statu;
        ra_data->shm.shm_addr[d_ra_test]  = _d_ra_test;
      }
      tra_uplink_func[ra_data->ker.state]();
      break;

#ifdef FF_FAX
    case MODEL_FAX:
      if (ra_data->shm.shm_addr[a_faxu] EQ NULL)
      {
        ra_data->shm.shm_addr[a_faxu]     = _ul_buffer_address;
        ra_data->shm.shm_addr[d_ra_conf]  = _d_ra_conf;
        ra_data->shm.shm_addr[d_ra_act]   = _d_ra_act;
        ra_data->shm.shm_addr[d_ra_statu] = _d_ra_statu;
        ra_data->shm.shm_addr[d_fax]      = _d_fax;
      }
      fax_uplink_func[ra_data->ker.state]();
      break;
#endif /* FF_FAX */

    default:
      return;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_DLL                   |
| STATE   : code                  ROUTINE : dll_data_dl              |
+--------------------------------------------------------------------+

  PURPOSE : This function is called from the L1 scheduler every 20ms
            (in TCH/9.6F). This function should read out the
            indiaction words and/or the downlink data from the shared
            memory area which is the interface between the DSP layer1
            and the MCU layer2 and send a primitive to the entity
            which is active in the selected data mode.
            This function is used for all kind
            of data transmission (FAX/TRANSPARENTDATA/RLP)
*/

GLOBAL void dll_data_dl(USHORT *_dl_buffer_address, USHORT *_d_ra_act, USHORT *_d_ra_statd)
{
  if (ra_data->activated EQ FALSE)
  {
    return;
  }

  switch (ra_data->model)
  {
  case MODEL_RLP:
    if (ra_data->shm.shm_addr[a_ntd] EQ NULL)
    {
      ra_data->shm.shm_addr[a_ntd]      = _dl_buffer_address;
      ra_data->shm.shm_addr[d_ra_act]   = _d_ra_act;
    }
    rlp_downlink_func[ra_data->ker.state]();
    break;

  case MODEL_TRANS:
    if (ra_data->shm.shm_addr[a_rad] EQ NULL)
    {
      ra_data->shm.shm_addr[a_rad]      = _dl_buffer_address;
      ra_data->shm.shm_addr[d_ra_act]   = _d_ra_act;
      ra_data->shm.shm_addr[d_ra_statd] = _d_ra_statd;
    }
    tra_downlink_func[ra_data->ker.state]();
    break;

#ifdef FF_FAX
  case MODEL_FAX:
    if (ra_data->shm.shm_addr[a_faxd] EQ NULL)
    {
      ra_data->shm.shm_addr[a_faxd]     = _dl_buffer_address;
      ra_data->shm.shm_addr[d_ra_act]   = _d_ra_act;
      ra_data->shm.shm_addr[d_ra_statd] = _d_ra_statd;
    }
    fax_downlink_func[ra_data->ker.state]();
    break;
#endif /* FF_FAX */

  default:
    return;
  }
}

