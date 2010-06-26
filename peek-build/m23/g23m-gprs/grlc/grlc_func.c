/*
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRLC
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
|  Purpose :  This modul is part of the entity GRLC and implements all
|             functions according to 921bis.doc
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_FUNC_C
#define GRLC_FUNC_C
#endif


#define ENTITY_GRLC


/*==== INCLUDES =============================================================*/


#include <string.h>
#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"    /* to get air message definitions */
#include "grlc.h"        /* to get the global entity definitions */
#include "grlc_func.h"   /* to check the function */
#include "grlc_f.h"      /* to get the global entity definitions */
#include "grlc_tpcs.h"

/* patch to avoid overwriting of primitives in global variable QueueMsg[] */
#undef VSI_CALLER_SINGLE
#define VSI_CALLER_SINGLE 0
  

/*==== CONST ================================================================*/

/*==== DIAGNOSTICS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : maca_power_control
+------------------------------------------------------------------------------
| Description : The function maca_power_control() is described in 921bis.doc
|
| Parameters  : see 921bis.doc
|
+------------------------------------------------------------------------------
*/
GLOBAL void maca_power_control ( UWORD8  assignment_id,
                                 BOOL    crc_error,
                                 WORD8   bcch_level,
                                 UWORD16 radio_freq[MAC_BURST_PER_BLOCK],
                                 WORD8   burst_level[MAC_BURST_PER_BLOCK],
                                 UWORD8 *pch )
{
  UBYTE i; /* used for counting */

  tpc_get_pch( pch );

  grlc_data->func.mac_pwr_ctrl_ind.assignment_id = assignment_id;
  grlc_data->func.mac_pwr_ctrl_ind.crc_error     = ( crc_error ? GRLC_CRC_FAIL : GRLC_CRC_PASS );
  grlc_data->func.mac_pwr_ctrl_ind.bcch_level    = bcch_level;

  for( i = 0; i < MAC_BURST_PER_BLOCK; i++ )
  {
    grlc_data->func.mac_pwr_ctrl_ind.burst_level[i] = burst_level[i];
    grlc_data->func.mac_pwr_ctrl_ind.radio_freq [i] = radio_freq[i];
  }

  PSIGNAL( hCommGRLC, MAC_PWR_CTRL_IND, &grlc_data->func.mac_pwr_ctrl_ind );

  return;
} /* maca_power_control() */

#ifdef _TARGET_

UWORD32 lst_dl_frame_nr;


/*
+------------------------------------------------------------------------------
| Function    : rlc_uplink
+------------------------------------------------------------------------------
| Description : The function rlc_uplink() is described in 921bis.doc
|
| Parameters  : see 921bis.doc
|
+------------------------------------------------------------------------------
*/
GLOBAL void rlc_uplink ( UWORD8   assignment_id,
                         UWORD8   tx_data_no,
                         UWORD32  fn,
                         UWORD8   timing_advance_value,
                         T_ul_poll_resp    * ul_poll_response,
                         T_ul_data         * ul_data, 
                         BOOL      allocation_exhausted)
{
  UWORD8 i;
  API *pollptr = (API*)ul_poll_response;
  API *outptr = (API*)ul_data;


  grlc_data->last_ul_fn = fn;
  grlc_data->ul_fn_store[(grlc_data->ul_cnt_syn%CALL_ERROR_NR_OF_ST_FN)] = fn;

  if(grlc_data->ul_cnt_syn NEQ grlc_data->ul_cnt_asyn)
  {
    grlc_data->ul_fn_errors[(grlc_data->ul_cnt_syn%CALL_ERROR_NR_OF_ST_FN)] = fn;
    grlc_data->ul_fn_errors[(grlc_data->ul_call_errors%CALL_ERROR_NR_OF_ST_FN)] = fn;
    
    /* Increment the Call_Error and wraparrond if it exceed CALL_ERROR_NR_OF_ST_FN */
    grlc_data->ul_call_errors = ((grlc_data->ul_call_errors%CALL_ERROR_NR_OF_ST_FN)+1);    
  }
  
  grlc_data->func.mac_ready_ind.nts      = tx_data_no;
  grlc_data->func.mac_ready_ind.ta_value = timing_advance_value;

  /*****copy Poll blocks**************************************************/
  if(fn EQ grlc_data->next_poll_fn)
  {
    i=0;
    while(grlc_data->ul_poll_resp[i].block_status)
    {
      pollptr[i*15] = grlc_data->ul_poll_resp[i].block_status;    
      pollptr[i*15+1] = grlc_data->ul_poll_resp[i].tn;    
      memcpy( (UBYTE*)&pollptr[i*15+2],
              (UBYTE*)grlc_data->ul_poll_resp[i].ul_block,
              12 * sizeof(API) );                               /*lint !e419*/
      pollptr[15*(i+1)] = 0x0000; /* Poll None*/
      i++;
    }
    grlc_data->ul_poll_resp[0].block_status = 0;
    grlc_data->next_poll_fn = 2;
  }
  else
  {
    pollptr[0] = 0x0000; /* Poll None*/
  }

  
  /*****copy data blocks**************************************************/
  
  outptr[0] = 0;
  if(grlc_data->ta_value NEQ 0xFF)
  {
    for(i=0;i<tx_data_no;i++)
    {
      outptr[i*29] = grlc_data->ru.ul_data[i].block_status;
      memcpy( (UBYTE*)&outptr[1+(i*29)],
              (UBYTE*)grlc_data->ru.ul_data[i].ul_block,
              28 * sizeof(API) );                           /*lint !e419*/
      outptr[(i+1)*29] = 0;
    }   
  }

  /*call control of syn part*/
  grlc_data->ul_cnt_syn++;  

  return;
} /* rlc_uplink() */
 

/*
+------------------------------------------------------------------------------
| Function    : rlc_downlink
+------------------------------------------------------------------------------
| Description : The function rlc_downlink() is described in 921bis.doc
|
| Parameters  : see 921bis.doc
|
+------------------------------------------------------------------------------
*/
GLOBAL void rlc_downlink ( UWORD8        assignment_id,
                           UWORD32       fn,
                           T_dl_struct   *data_ptr,
                           UWORD8        rlc_blocks_sent,
                           UWORD8        last_poll_response)
{
  API * in_ptr = (API*) (data_ptr);
  BOOL send_psginal;
  UBYTE i,pos = 0, call_err = 0, crc_cnt=0;


  grlc_data->last_dl_fn = lst_dl_frame_nr; 
  lst_dl_frame_nr = fn;
  grlc_data->dl_fn_store[(grlc_data->dl_cnt_syn%CALL_ERROR_NR_OF_ST_FN)] = fn-4;

  if(grlc_data->dl_cnt_syn NEQ grlc_data->dl_cnt_asyn)
  {
    grlc_data->dl_fn_errors[(grlc_data->dl_cnt_syn%CALL_ERROR_NR_OF_ST_FN)] = fn-4;
    grlc_data->dl_call_errors++;    
  }

  grlc_data->dl_fn                           = fn-4;
  grlc_data->func.dl_blocks.fn               = fn-4;
  grlc_data->func.dl_blocks.data_ptr.rx_no   = 0;
  send_psginal = FALSE;
  if(grlc_data->dl_cnt_syn EQ grlc_data->dl_cnt_asyn)
  {
    for (i = 0; i < data_ptr->rx_no; i++) 
    {
      /* 
       * In Test mode B,if there is CRC error on the payload data the MS will,
       * where required by the USF, transmit the decoded payload data. The block
       * transmitted will be a valid uplink block format.
       */
      
      if(((in_ptr[31 * i + 1] & 0x0100) EQ 0x0000) OR (grlc_data->testmode.mode EQ CGRLC_LOOP) ) 
      {
        send_psginal = TRUE;
        grlc_data->func.dl_blocks.data_ptr.rx_no++;
#ifdef TI_PS_16BIT_CPY
        api_memcpy(&grlc_data->func.dl_blocks.data_ptr.dl_data[pos],
               &in_ptr[31 * i + 1],
               31 * sizeof(API) );                                  /*lint !e420*/
#else
        memcpy(&grlc_data->func.dl_blocks.data_ptr.dl_data[pos],
               &in_ptr[31 * i + 1],
               31 * sizeof(API) );                                  /*lint !e420*/
#endif
        pos++;
      }
      else        
      { /* CRC error */
        grlc_data->nr_of_crc_errors++;
        crc_cnt++;
      }
    }
  } 
  else
  { /* call error */
    grlc_data->dl_cnt_asyn = grlc_data->dl_cnt_syn;  
    call_err = 0xFF;
    TRACE_ERROR("DL CALL ERROR");
  }

  grlc_data->dl_cnt_syn++;
  
  if(send_psginal)
  { 
    PSIGNAL(hCommGRLC,MAC_DATA_IND,  (&grlc_data->func.mac_data_ind) ); 
  }
  else
  {
    grlc_data->dl_cnt_asyn++; /*no signal to GFF, inc asynchronus counter*/ 
    if(data_ptr->rx_no NEQ  crc_cnt)
    {
      TRACE_EVENT_P3("NO DL DATA:  rx_no= %d   crc_err=%d  call_err= %d",data_ptr->rx_no, crc_cnt,call_err);
    }
  }

  /* new interface */
  grlc_data->func.mac_ready_ind.rlc_blocks_sent  = rlc_blocks_sent;
  grlc_data->func.mac_ready_ind.last_poll_resp   = last_poll_response;
  grlc_data->func.mac_ready_ind.fn               = fn;
  grlc_data->ul_fn                               = fn; 

  /* adapte uplink framenumber from downlink framenumber */
    if((grlc_data->ul_fn % 13) EQ 12)
      grlc_data->ul_fn++;
    grlc_data->ul_fn %= FN_MAX;    

  PSIGNAL(hCommGRLC, MAC_READY_IND, &(grlc_data->func.mac_ready_ind));

  return;
} /* rlc_downlink() */


#endif /* _TARGET_ */
