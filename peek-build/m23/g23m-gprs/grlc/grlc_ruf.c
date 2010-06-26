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
|  Purpose :  This module implements local functions for service RU of
|             entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_RUF_C
#define GRLC_RUF_C
#endif

#define ENTITY_GRLC


/*==== INCLUDES =============================================================*/

#include <stdio.h>
#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_grlc.h"    /* to get cnf-definitions */
#include "mon_grlc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"
#include "grlc.h"        /* to get the global entity definitions */
#include "grlc_f.h"
#include "grlc_tms.h"
#include "grlc_rds.h"
#include "grlc_ruf.h"
#include <string.h>
#include "cl_rlcmac.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : ru_init
+------------------------------------------------------------------------------
| Description : The function ru_init() set init values in null state.
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_init ( void )
{ 
   UBYTE i;
   UBYTE j;
  
  TRACE_FUNCTION( "ru_init" );
  INIT_STATE(RU,RU_NULL);

  /*
   * UBYTE
   */  
  grlc_data->ru.nts               = 0xFF;
  grlc_data->ru.nts_max           = 0xFF;
  grlc_data->ru.ti                = 0xFF;
  grlc_data->ru.next_prim         = 0xFF;
  grlc_data->ru.active_prim       = 0xFF;
  grlc_data->ru.vs                = 0xFF;
  grlc_data->ru.va                = 0xFF;
  grlc_data->ru.bsn_ret           = 0xFF;
  grlc_data->ru.last_si_block     = 0xFF;
  grlc_data->ru.cv                = 0xFF;
  grlc_data->ru.N3104             = 0xFF;
  grlc_data->ru.N3104_MAX         = 0xFF;
  grlc_data->ru.block_status      = 0xFF;
  grlc_data->ru.poll_tn           = 0xFF;
  grlc_data->ru.count_cv_0        = 0xFF;
  grlc_data->ru.nr_nacked_blks    = 0xFF;
  grlc_data->ru.pdu_cnt           = 0xFF; 
  grlc_data->ru.pdu_sent          = 0xFF; 
  grlc_data->ru.pdu_rem           = 0xFF; 
  grlc_data->ru.pdu_boundaries    = 0xFF; 

  grlc_data->ru.write_pos_index   = 0xFF;
  
  /*
   * USHORT
   */
  grlc_data->ru.rlc_data_size     = 0xFFFF;
  grlc_data->ru.sdu_len           = 0xFFFF;
  grlc_data->ru.sdu_off           = 0xFFFF;
  grlc_data->ru.rlc_octet_cnt     = 0xFFFF;
  grlc_data->ru.tbc               = 0xFFFF;  
  grlc_data->ru.cnt_ts            = 0xFFFF;
  
  /*
   * BOOL
   */
  grlc_data->ru.tlli_cs_type      = FALSE;
  grlc_data->ru.cd_active         = TRUE;
  grlc_data->ru.first_usf         = FALSE;
  grlc_data->ru.release_tbf       = FALSE;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  grlc_data->ru.tbf_re_est        = FALSE;
#endif

  /*
   * Type
   */  
  grlc_data->ru.cs_type           = CS_ZERO;
  /*
   * structs
   */
  grlc_data->ru.pl_retrans_current.cnt = 0xFF;
  memset(grlc_data->ru.pl_retrans_current.blk, 0xFF, 4);
  /*
   * array
   */
  for (i=0;i<WIN_SIZE;i++)
    grlc_data->ru.vb[i & WIN_MOD] = VB_INVALID;
  /* 
   * global structs
   */
  for (i=0;i<NEXT_POLL_ARRAY_SIZE;i++)
  { 
    /*
     * set free list
     */
    grlc_data->next_poll_array[i].next      = i+1;
    grlc_data->next_poll_array[i].fn        = 0xFFFFFFFF;
    grlc_data->next_poll_array[i].cnt       = 0;
    for(j=0; j< POLL_TYPE_ARRAY_SIZE; j++)
      grlc_data->next_poll_array[i].poll_type[j] = CGRLC_POLL_NONE;
  }
  /* 
   * last free entry points to 0xff
   */ 
  grlc_data->next_poll_array[NEXT_POLL_ARRAY_SIZE-1].next = 0xFF;  
  /* 
   * indicates invalid paramters
   */
  grlc_data->poll_start_free = 0;
  grlc_data->poll_start_tbf  = 0xFF;   

} /* ru_init() */



/*

+------------------------------------------------------------------------------
| Function    : ru_within_window
+------------------------------------------------------------------------------
| Description : The function ru_within_window() checks if the bsn_i value is 
|               between high_value_i and low_value_i(modulo operating).  
|               The return value is result.
|               
|
| Parameters  : bsn_i          - chech this value
|               high_value_i   - right side of the window
|               low_value_i    - left side of the window
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL ru_within_window ( UBYTE bsn_i, UBYTE high_value_i,UBYTE low_value_i )
{ 
  BOOL result = FALSE;
  TRACE_FUNCTION( "ru_within_window" );
  
  if(
     ( high_value_i >= low_value_i  ) AND  /*   vs >= va  */
     ( bsn_i        >  low_value_i  ) AND  /*  ssn >  va  */
     ( bsn_i        <= high_value_i )      /*   vs >= ssn */
    )
  {
    result = TRUE;
  }
  else if(  ( high_value_i <  low_value_i  )   /*  va > vs  */
              AND
            (   (  ( bsn_i   > low_value_i  ) AND     /*  ssn > va  */
                   ( bsn_i   > high_value_i ))    OR  /*  ssn > vs  */   
                (  ( bsn_i   < low_value_i  ) AND     /*  ssn > va  */
                   ( bsn_i  <= high_value_i ))        /*  ssn <= vs  */
            )
    )
  {
    result = TRUE;
  }

 /* if(!result)
  {
    TRACE_EVENT_P3("BSN not in range: low=%d bsn=%d high=%d", low_value_i,bsn_i,high_value_i);
  }
 */

  return result;
} /* ru_within_window() */



/*
+------------------------------------------------------------------------------
| Function    : ru_recalc_rlc_oct_cnt
+------------------------------------------------------------------------------
| Description : The function ru_recalc_rlc_oct_cnt() the number of octetcs
|               during the TBF. It is needed for starting the countdown procedure 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL USHORT ru_recalc_rlc_oct_cnt ( void )
{ 
  USHORT  result;
  UBYTE   next_cnt;
  UBYTE   prev_cnt;
  UBYTE   rem_pdu_bound;
  ULONG   rest_of_pdu;
  UBYTE   last_pdu_bound;
  TRACE_FUNCTION( "ru_recalc_rlc_oct_cnt" );

  grlc_data->ru.next_prim = grlc_data->prim_queue[grlc_data->ru.active_prim].next;
  result                = grlc_data->ru.sdu_len;
  next_cnt              = grlc_data->ru.next_prim;
  if(grlc_data->ru.sdu_len)
    grlc_data->ru.pdu_rem  = 1;
  
  /*init calculate nr of pdu boundaries*/
  rest_of_pdu = grlc_data->ru.sdu_len%grlc_data->ru.rlc_data_size;
  if((rest_of_pdu EQ 0)                                  AND 
     ((next_cnt EQ 0xFF)                             OR      /* either no primitive in queue         */
       grlc_data->prim_queue[next_cnt].start_new_tbf))       /* or next primitive belongs to new tbf */
    rem_pdu_bound = 0; /*fills excatly in cv=0*/
  else if(rest_of_pdu EQ 0)
    rem_pdu_bound = 2; /*fills exxalty, more pdus rerq, therefore 2 bounds*/
  else
    rem_pdu_bound = 1; /*normal case*/
  last_pdu_bound = rem_pdu_bound;
  
#ifdef _SIMULATION_
  TRACE_EVENT_P4("pdu_len=%d,rest_of_pdu=%d,rem_pdu_bound=%d,last_pdu_bound=%d",
                                grlc_data->ru.sdu_len,
                                rest_of_pdu,
                                rem_pdu_bound,
                                last_pdu_bound );
#endif /* #ifdef _SIMULATION_ */

  prev_cnt = grlc_data->ru.active_prim;
  while((next_cnt < PRIM_QUEUE_SIZE_TOTAL) AND 
	    (grlc_data->prim_queue[next_cnt].start_new_tbf EQ 0)
         )
  {
    ULONG sdu_len;
    result   += grlc_data->prim_queue[next_cnt].prim_ptr->sdu.l_buf/8;
    sdu_len   = grlc_data->prim_queue[next_cnt].prim_ptr->sdu.l_buf/8;
    grlc_data->prim_queue[next_cnt].previous   = prev_cnt;
    grlc_data->prim_queue[next_cnt].cv_status  = FALSE;
    grlc_data->prim_queue[next_cnt].rlc_status = FALSE;
    grlc_data->prim_queue[next_cnt].last_bsn   = 0xFF;
 
    prev_cnt  = next_cnt;
    next_cnt  = grlc_data->prim_queue[next_cnt].next;
    grlc_data->ru.pdu_rem++;
    
    /*init calculate nr of pdu boundaries*/
    
    rest_of_pdu = ((sdu_len+last_pdu_bound+rest_of_pdu)%grlc_data->ru.rlc_data_size);
    if((rest_of_pdu EQ 0) AND ((next_cnt EQ 0xFF) OR grlc_data->prim_queue[next_cnt].start_new_tbf))
    {
      rem_pdu_bound += 0; /*fills excatly in cv=0*/
      last_pdu_bound = 0;
    }
    else if(rest_of_pdu EQ 0)
    {
      rem_pdu_bound += 2; /*fills exxalty, more pdus rerq, therefore 2 bounds*/
      last_pdu_bound = 2;
    }
    else
    {
      rem_pdu_bound += 1; /*normal case*/
      last_pdu_bound = 1;
    }    
#ifdef _SIMULATION_
  TRACE_EVENT_P4("pdu_len=%d,rest_of_pdu=%d,rem_pdu_bound=%d,last_pdu_bound=%d",
                                                                                sdu_len,
                                                                                rest_of_pdu,
                                                                                rem_pdu_bound,
                                                                                last_pdu_bound );
#endif /* #ifdef _SIMULATION_ */

  }
  grlc_data->ru.pdu_cnt         = grlc_data->ru.pdu_rem+grlc_data->ru.pdu_sent;
  grlc_data->ru.pdu_boundaries  = rem_pdu_bound;
#ifdef _SIMULATION_
  TRACE_EVENT_P4("pdu_cnt=%d,pdu_sent=%d,pdu_rem=%d,needed pdu_boundaries=%d",
                                                                            grlc_data->ru.pdu_cnt,
                                                                            grlc_data->ru.pdu_sent,
                                                                            grlc_data->ru.pdu_rem,
                                                                            grlc_data->ru.pdu_boundaries );
#endif /* #ifdef _SIMULATION_ */
    
  grlc_data->tbf_ctrl[grlc_data->ul_index].pdu_cnt     = grlc_data->ru.pdu_cnt;
  return result;
} /* ru_recalc_rlc_oct_cnt() */





/*
+------------------------------------------------------------------------------
| Function    : ru_get_next_sdu
+------------------------------------------------------------------------------
| Description : The function ru_get_next_sdu() gets the next GRLC_DATA_REQ from
|               the queue.  
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_get_next_sdu ( void )
{ 
  TRACE_FUNCTION( "ru_get_next_sdu" );

  grlc_data->ru.pdu_rem--;
  grlc_data->ru.pdu_sent++;
  grlc_data->prim_queue[grlc_data->ru.active_prim].last_bsn = grlc_data->ru.vs;

  if((grlc_data->prim_queue[grlc_data->ru.next_prim].start_new_tbf EQ 0) AND 
      grlc_data->ru.next_prim NEQ 0xFF)
  {
    grlc_data->ru.active_prim = grlc_data->prim_queue[grlc_data->ru.active_prim].next;
    grlc_data->ru.sdu_len    = grlc_data->prim_queue[grlc_data->ru.active_prim].prim_ptr->sdu.l_buf/8;
    grlc_data->ru.sdu_off    = grlc_data->prim_queue[grlc_data->ru.active_prim].prim_ptr->sdu.o_buf/8;
    grlc_data->prim_queue[grlc_data->ru.active_prim].rlc_status = TRUE;
    grlc_data->ru.next_prim = grlc_data->prim_queue[grlc_data->ru.active_prim].next;    
  }
  else
  {
    grlc_data->ru.sdu_len    = 0;
    grlc_data->ru.sdu_off    = 0;

  }

#ifdef _SIMULATION_
  TRACE_EVENT_P3("pdu_cnt=%d,pdu_sent=%d,pdu_rem=%d",
                                                    grlc_data->ru.pdu_cnt,
                                                    grlc_data->ru.pdu_sent,
                                                    grlc_data->ru.pdu_rem );
#endif /* #ifdef _SIMULATION_ */
 
} /* ru_get_next_sdu() */





/*
+------------------------------------------------------------------------------
| Function    : ru_calc_rlc_data_size
+------------------------------------------------------------------------------
| Description : The function ru_calc_rlc_data_size() calculates the Data size of
|               an RLC data block without header and TLLI. The size is described
|               in bytes.
|
| Parameters  : cs_type_i - Coding scheme, determines the size of an RLC data block
|                 
+------------------------------------------------------------------------------
*/
GLOBAL USHORT ru_calc_rlc_data_size ( T_CODING_SCHEME  cs_type_i, UBYTE ti_bit_i )
{ 
  USHORT  result=0;
  UBYTE   tlli_size;
  TRACE_FUNCTION( "ru_calc_rlc_data_size" );
  
  /*
   * check if tlli must be included in RLC data blocks
   */
  if (ti_bit_i EQ 0)
    tlli_size = 0; 
  else
  {
    tlli_size = TLLI_SIZE; 
    if(!(grlc_data->ru.tlli_cs_type))
      cs_type_i = CS_1; 
#ifdef REL99  
    if(grlc_data->pfi_support)
    {
      tlli_size++;
    } 
#endif
  }
  
  switch( cs_type_i)
  {
  case CS_1:
    result = 20;
    break;
  case CS_2:
    result = 30;
    break;
  case CS_3:
    result = 36;
    break;
  case CS_4:
    result = 50;
    break;
  default:
  if (!result)
  {
    TRACE_EVENT_P2("Unknown coding scheme, tsize=%d, t_cs=%d", 
                     tlli_size, grlc_data->ru.tlli_cs_type);
    result = 20; /* Going by CS1 coding scheme by default */
  }
    
    break;
  } 

  
  result -= tlli_size;
  
  return result;
} /* ru_calc_rlc_data_size() */


/*
+------------------------------------------------------------------------------
| Function    : ru_set_block_status
+------------------------------------------------------------------------------
| Description : The function ru_set_block_status() sets the block status.
|
| Parameters  : cs_type_i - Coding scheme, determines the block status
|                 
+------------------------------------------------------------------------------
*/
GLOBAL USHORT ru_set_block_status ( T_CODING_SCHEME  cs_type_i )
{ 
  USHORT  result=0;
  TRACE_FUNCTION( "ru_set_block_status" );

  if(!(grlc_data->ru.tlli_cs_type) AND 
     grlc_data->ru.ti)
     cs_type_i = CS_1; 

  switch( cs_type_i)
  {
  case CS_1:
    result = 2;
    break;
  case CS_2:
    result = 4;
    break;
  case CS_3:
    result = 5;
    break;
  case CS_4:
    result = 6;
    break;
  default:
    TRACE_EVENT_P3("Unknown CS cstype=%d t_cs=%d, ti=%d",cs_type_i,
                    grlc_data->ru.tlli_cs_type, grlc_data->ru.ti);
    
    break;
  } 
  return result;
} /* ru_set_block_status() */


/*
+------------------------------------------------------------------------------
| Function    : ru_get_cs_type
+------------------------------------------------------------------------------
| Description : The function ru_get_cs_type() returns the coding scheme 
|               for the requeseted block status.
|
| Parameters  : bs_i - requested block status
|                 
+------------------------------------------------------------------------------
*/
GLOBAL T_CODING_SCHEME  ru_get_cs_type ( USHORT  bs_i )
{ 
  T_CODING_SCHEME  result= CS_ZERO;
    ;
  TRACE_FUNCTION( "ru_get_cs_type" );
  

  switch( bs_i)
  {
  case 2:
    result = CS_1;
    break;
  case 4:
    result = CS_2;
    break;
  case 5:
    result = CS_3;
    break;
  case 6:
    result = CS_4;
    break;
  default:
    TRACE_ERROR("unknown block status");
    break;
  } 
  return result;
} /* ru_get_cs_type() */  
/*
+------------------------------------------------------------------------------
| Function    : ru_tbf_init
+------------------------------------------------------------------------------
| Description : The function ru_tbf_init() sets the parameters at tbf assignment
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_tbf_init ( void )
{ 
  UBYTE i;

  TRACE_FUNCTION( "ru_tbf_init" );

  grlc_data->ru.rlc_mode   = grlc_data->uplink_tbf.rlc_mode;
  grlc_data->ru.nts        = grlc_data->uplink_tbf.nts; 
  grlc_data->ru.nts_max    = grlc_data->uplink_tbf.nts;
  grlc_data->ru.va         = 0; 
  grlc_data->ul_tn_mask    = grlc_data->uplink_tbf.ts_mask;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  grlc_data->ru.tbf_re_est = FALSE;
#endif

  
  /*
   * initate l1 queue parameters
   */
  grlc_data->ru.write_pos_index   = 0;	
  for(i=0; i<MAX_UL_TN;i++)
    grlc_data->ru.ul_data[i].block_status = 0;


  if(grlc_data->ru.rlc_mode EQ CGRLC_RLC_MODE_ACK)
  {
    /* 
     * parameters only used in acknowledged mode
     */        
    grlc_data->ru.bsn_ret           = 0;     
    grlc_data->ru.last_si_block     = 0;     
    grlc_data->ru.N3104_MAX         = 3 * (grlc_data->uplink_tbf.bs_cv_max + 3 ) * grlc_data->ru.nts_max; 
    grlc_data->ru.nr_nacked_blks    = 0;
    grlc_data->ru.ti                = grlc_data->uplink_tbf.ti;
  }
  else if(grlc_data->ru.rlc_mode EQ CGRLC_RLC_MODE_UACK)
  {
    /* 
     * parameters only used in unacknowledged mode
     */
    grlc_data->ru.count_cv_0        = 0; 
    /* 
     * tlli is not send within data blocks
     */ 
    grlc_data->ru.ti                = 0; 
  }

  /* 
   * read the first pdu 
   */

  grlc_data->ru.active_prim        = grlc_data->prim_start_tbf;
  grlc_data->prim_queue[grlc_data->prim_start_tbf].previous = 0xFF;
  grlc_data->ru.next_prim          = grlc_data->prim_queue[grlc_data->ru.active_prim].next;
  grlc_data->prim_queue[grlc_data->prim_start_tbf].cv_status  = FALSE;
  grlc_data->prim_queue[grlc_data->prim_start_tbf].last_bsn   = 0xFF;
  grlc_data->ru.vs                = 0;
  grlc_data->ru.cv                = 55;
  /*
   * USHORT
   */
  grlc_data->ru.sdu_len           = grlc_data->prim_queue[grlc_data->ru.active_prim].prim_ptr->sdu.l_buf/8;
  grlc_data->ru.sdu_off           = grlc_data->prim_queue[grlc_data->ru.active_prim].prim_ptr->sdu.o_buf/8;
  grlc_data->prim_queue[grlc_data->ru.active_prim].rlc_status = TRUE;
  grlc_data->ru.pdu_sent          = 0;
  grlc_data->ru.cnt_ts            = 0;
  /*
   * ULONG
   */
  grlc_data->missed_poll_fn       = 1;

  /*
   * BOOL
   */
  grlc_data->ru.tlli_cs_type      = grlc_data->uplink_tbf.tlli_cs_type;
  grlc_data->ru.cd_active         = FALSE;
  grlc_data->ru.reorg_l1_needed	  = FALSE;
  grlc_data->ru.v_next_tbf_params = FALSE;
  grlc_data->ul_tfi_changed       = FALSE;
  grlc_data->ru.release_tbf       = FALSE;

  /*
   * Type
   */    
  grlc_data->ru.cs_type           = grlc_data->uplink_tbf.cs_type;
  grlc_data->ru.last_bsn          = LAST_BSN_NOT_BULIT;
  /*
   * struct
   */
  if(grlc_data->uplink_tbf.mac_mode EQ CGRLC_MAC_MODE_DA)
  {
     /*
      * Start t3164, when starting time is elapsed.
      * Only if contention resolution is needed and not fixed alloc is used.
      */
    grlc_data->ru.first_usf   = TRUE;
  }  
  grlc_data->ru.rlc_data_size = ru_calc_rlc_data_size(grlc_data->ru.cs_type, grlc_data->ru.ti);
  grlc_data->ru.block_status  = ru_set_block_status(grlc_data->ru.cs_type);

  /*
   * handle close ended TBF
   */
  if (grlc_data->uplink_tbf.rlc_db_granted )
  {
    ULONG rlc_oct_cnt ;
    
    UBYTE next = grlc_data->prim_queue[grlc_data->prim_start_tbf].next;

    rlc_oct_cnt = (grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_ptr->sdu.l_buf/8)+1;
    
    grlc_data->uplink_tbf.rlc_db_granted *= grlc_data->ru.rlc_data_size;

    while( (next < PRIM_QUEUE_SIZE)                       AND 
           (!grlc_data->prim_queue[next].start_new_tbf)   AND    
           ((rlc_oct_cnt+(grlc_data->prim_queue[next].prim_ptr->sdu.l_buf/8)+1) < grlc_data->uplink_tbf.rlc_db_granted))
    {
      rlc_oct_cnt += (grlc_data->prim_queue[next].prim_ptr->sdu.l_buf/8)+1;
      next         = grlc_data->prim_queue[next].next;
    }
    if(next < PRIM_QUEUE_SIZE)
      grlc_data->prim_queue[next].start_new_tbf = 1;

    TRACE_EVENT_P4("close ended TBF:  rlc_g=%ld rlc_oct =%ld data_size=%d next=%d"
                    ,grlc_data->uplink_tbf.rlc_db_granted
                    ,rlc_oct_cnt
                    ,grlc_data->ru.rlc_data_size
                    ,next);
  }

  grlc_data->ru.rlc_octet_cnt = ru_recalc_rlc_oct_cnt(); /*rlc data size needed*/
  grlc_data->ru.pl_retrans_current.cnt = 0;




  /* TRACE PARAMS*/
  grlc_data->ul_index = 0;        
  grlc_data->tbf_ctrl[grlc_data->ul_index].tbf_type    = TBF_TYPE_UL;
  grlc_data->tbf_ctrl[grlc_data->ul_index].tfi         = grlc_data->ul_tfi;        
  grlc_data->tbf_ctrl[grlc_data->ul_index].rlc_oct_cnt = 0;
  grlc_data->tbf_ctrl[grlc_data->ul_index].ret_bsn     = 0;
  grlc_data->tbf_ctrl[grlc_data->ul_index].ack_cnt     = 0;
  grlc_data->tbf_ctrl[grlc_data->ul_index].fbi         = 0;
} /* ru_tbf_init() */



/*
+------------------------------------------------------------------------------
| Function    : ru_set_T3198
+------------------------------------------------------------------------------
| Description : The function ru_set_T3198() starts the timer T3198 of the rlc
|               data block with bsn as input parameter. The timer is handled in
|               BS_CV_MAX block periods.
|
| Parameters  : bsn_i - bsn value of the data block
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_set_T3198 ( UBYTE bsn_i )
{ 
  USHORT block_periods;
  
  
  TRACE_FUNCTION( "ru_set_T3198" );


  block_periods = MAXIMUM(1,grlc_data->uplink_tbf.bs_cv_max);
  /*
   * set timer T3198 , 
   * the framenumber is set, at which the block is set to negative acknowledged
   */
  grlc_data->ru.rlc_data[bsn_i & WIN_MOD].T3198 = grlc_decode_tbf_start_rel(grlc_data->ul_fn, --block_periods);

#ifdef _SIMULATION_
    TRACE_EVENT_P4("SET T3198: bsn %d t3198=%ld c_fn=%ld bs_cv_max=%d"
                                                                      ,bsn_i
                                                                      ,grlc_data->ru.rlc_data[bsn_i & WIN_MOD].T3198
                                                                      ,grlc_data->ul_fn
                                                                      ,grlc_data->uplink_tbf.bs_cv_max);
#endif

} /* ru_set_T3198() */

/*
+------------------------------------------------------------------------------
| Function    : ru_send_mac_data_req
+------------------------------------------------------------------------------
| Description : The function ru_send_mac_data_req() sends the data block with
|               bsn_i. The claculated block is matched in the PL buffer  
|               (and MAC_DATA_REQ primitive, if SIMULATION is defined). 
|
| Parameters  : bsn_i - bsn value of the next to sent data block
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_send_mac_data_req ( UBYTE bsn_i )
{ 
  UBYTE   li_cnt;
  UBYTE   data_cnt;
  UBYTE   i;
  USHORT  l_tlli;
  UBYTE   len;
  USHORT  off;
  UBYTE   index;
  UBYTE   *ptr_temp;
  TRACE_FUNCTION( "ru_send_mac_data_req" );

#ifdef _SIMULATION_
  TRACE_EVENT_P2("BSN=%d mac_header=%x ",bsn_i, grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.mac);
#endif /* #ifdef _SIMULATION_ */

      

  {
    memset(&grlc_data->ru.ul_data[grlc_data->ru.write_pos_index],
           0, 
           sizeof(T_ul_data));

    grlc_data->ru.ul_data[grlc_data->ru.write_pos_index].block_status = grlc_data->ru.rlc_data[bsn_i & WIN_MOD].block_status;

    ptr_temp  = (UBYTE *) (grlc_data->ru.ul_data[grlc_data->ru.write_pos_index].ul_block);    
    /*
     * set MAC Header 
     */
    ptr_temp[0]  = grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.mac;  

    ptr_temp[1]  = (grlc_data->ul_tfi <<  1 |
                    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.ti);

    ptr_temp[2]  =  (bsn_i  <<  1 |
                    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.e_bit);                /*lint !e415*/
    /*
     * set Length Indicator field(s) if present
     */
    li_cnt = grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.li_cnt;
    for(i=3; i<(3+li_cnt);i++)
      ptr_temp[i] = grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.li_me[i-3];   /*lint !e661 !e662*/
    /*
     * set TLLI field, if present
     */
    if(grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.ti)
    {
      grlc_set_tlli( &l_tlli, &off, &(ptr_temp[3+li_cnt]), grlc_data->uplink_tbf.tlli );
      l_tlli /= 8; /* handled in bytes */

#ifdef REL99
      if (grlc_data->pfi_support)
      {
        ptr_temp[1] |= 0x40;
        ptr_temp[3+li_cnt+l_tlli] =(grlc_data->pfi_value << 1) | 0x01;/*04.60 7.1.2.6(initial pfi)*/ /*E bit = 1*/
        l_tlli++;
      } 
#endif

    }
    else
      l_tlli  = 0;
    /* 
     * copy LLC DATA
     */
    index    = 0;
    data_cnt = grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data_cnt;
    for(i=0;i<data_cnt;i++)
    {
      len = grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[i].l_buf / 8;
      off = grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[i].o_buf / 8;
      if(!grlc_test_mode_active())
      { /*
         * get regular data
         */
        memcpy(&(ptr_temp[3+li_cnt+l_tlli+index]),
                (grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[i].ptr_data->buf + off),
                 len);                                                        /*lint !e797*/
      }
      else
      {   
        /* 
         * test mode active, get test data
         */
        if(grlc_data->testmode.mode EQ CGRLC_TEST_RANDOM)
        {  
          /*
           *  CGRLC_TEST_RANDOM
           */
          if (!grlc_data->testmode.no_data_calculation)
          {
            /*
             *  First call in this block
             */
            grlc_prbs(COMPUTE_DATA, len, grlc_data->testmode.ptr_test_data);
            grlc_data->testmode.no_data_calculation = 1;
          }
          
          
          if (grlc_data->ru.nts EQ 1)
          { 
             /*
              * this block is finished, allow prbs data calculation aigain
              */
             grlc_data->testmode.no_data_calculation = 0;
          }  

          /*
           *  copy the prbs in 
           */
          memcpy(&(ptr_temp[3+li_cnt+l_tlli+index]), grlc_data->testmode.ptr_test_data, len); /*lint !e797*/        
        }
        else
        {
          /*
           *  CGRLC_LOOP
           */     
          /* 
           * If the downlink TBF is established on more than one timeslot, the MS shall transmit in 
           * the second uplink timeslot (if present) RLC/MAC blocks received on the second downlink 
           * timeslot, and shall transmit in the third uplink timeslot (if present) RLC/MAC blocks 
           * received in the third downlink timeslot and so on.
           * It is assumed that while transmitting L1 will transmit data blocks in the sequence
           * in which it has received blocks from GRLC.
           */
          if (grlc_data->downlink_tbf.nts > 1)
          {
            if (grlc_data->ru.write_pos_index EQ 0)
            {
              memcpy(&(ptr_temp[3+li_cnt+l_tlli+index]),grlc_data->testmode.rec_data[0].payload,len);
              TRACE_EVENT_P4("bsn =%d ready for send, cs_type=%ld, e_bit=%d, tn=%d"
                                        ,bsn_i
                                        ,grlc_data->ru.ul_data[grlc_data->ru.write_pos_index].block_status
                                        ,grlc_data->testmode.rec_data[0].e_bit
                                        ,grlc_data->ru.write_pos_index);
            }
            else
            {
              memcpy(&(ptr_temp[3+li_cnt+l_tlli+index]),grlc_data->testmode.rec_data[1].payload,len);
              TRACE_EVENT_P4("bsn =%d ready for send, cs_type=%ld, e_bit=%d, tn=%d"
                                        ,bsn_i
                                        ,grlc_data->ru.ul_data[grlc_data->ru.write_pos_index].block_status
                                        ,grlc_data->testmode.rec_data[1].e_bit
                                        ,grlc_data->ru.write_pos_index);
            }
          }
          else
          {
             memcpy(&(ptr_temp[3+li_cnt+l_tlli+index]),grlc_data->testmode.rec_data[0].payload,len);
             TRACE_EVENT_P3("bsn =%d ready for send, cs_type=%ld, e_bit=%d"
                                        ,bsn_i
                                        ,grlc_data->ru.ul_data[grlc_data->ru.write_pos_index].block_status
                                        ,grlc_data->testmode.rec_data[0].e_bit);
          }


        }
      }
      index += len;
    }
    if((grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.mac & 0x3C) EQ 0) /* countdown value is equal 0*/
    {
      /*
       * fill rest of last block with 2B
       */
      USHORT rlc_data_size;
      rlc_data_size = ru_calc_rlc_data_size( ru_get_cs_type(grlc_data->ru.rlc_data[bsn_i & WIN_MOD].block_status)
                      , grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.ti);
      len = rlc_data_size-index-li_cnt;
      memset(&(ptr_temp[3+li_cnt+l_tlli+index]),
              0x2B,
               len);                              /*lint !e797*/
    }

#ifdef _SIMULATION_
  {
    PALLOC(mac_data_req,MAC_DATA_REQ);
    memset(&(mac_data_req->ul_data),
           0, 
           sizeof(T_ul_data));
    memcpy(&(mac_data_req->ul_data),
           &(grlc_data->ru.ul_data[grlc_data->ru.write_pos_index]),
           sizeof(T_ul_data));                                    /*lint !e797*/
    PSEND(hCommL1,mac_data_req);
  }
  TRACE_EVENT_P1("wpi %d",grlc_data->ru.write_pos_index);
#else  /* #ifdef _SIMULATION_ */
  {
    TRACE_MEMORY_PRIM ( hCommGRLC, hCommL1, MAC_DATA_REQ,
                        &grlc_data->ru.ul_data[grlc_data->ru.write_pos_index],
                        sizeof(T_ul_data) );
  }
#endif /* #ifdef _SIMULATION_ */
      
    grlc_data->ru.write_pos_index++;
    
    /*
     * indicates the PL, there is no more block
     */
    grlc_data->ru.ul_data[grlc_data->ru.write_pos_index].block_status = 0;
  }
  grlc_data->ru.pl_retrans_current.blk[grlc_data->ru.pl_retrans_current.cnt] = bsn_i;
  grlc_data->ru.pl_retrans_current.cnt++;
  grlc_data->ru.nts--;
  grlc_data->ru.vb[bsn_i & WIN_MOD] = VB_PENDING_ACK;
} /* ru_send_mac_data_req() */


  

/*
+------------------------------------------------------------------------------
| Function    : ru_set_prim_queue
+------------------------------------------------------------------------------
| Description : The function ru_set_prim_queue() updates the primitive queue.
|               If the countdown procedure is started, all LLC PDUs which belongs
|               to the current tbf are signed with cv_status as true. In this case
|               these LLC PDUs can´t reorginized an more LLC PDU can sent during 
|               his TBF. Additionally received LLC PDUs are sent in a new TBF.
|               If the countdown procedure is stopped (i.e. change of cpding 
|               scheme) then this functions resets the cv_status to False.
|
| Parameters  : cd_state_i - if true, the countdown procedue is started, else
|               it is stopped
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_set_prim_queue ( BOOL cd_state_i )
{ 
  UBYTE next;
  TRACE_FUNCTION( "ru_set_prim_queue" );

  next = grlc_data->ru.next_prim;
  grlc_data->prim_queue[grlc_data->ru.active_prim].cv_status = cd_state_i;

  while( (grlc_data->prim_queue[next].start_new_tbf EQ 0) AND (next NEQ 0xFF) )
  {
    grlc_data->prim_queue[next].cv_status = cd_state_i;
    next = grlc_data->prim_queue[next].next;
  }

  
} /* ru_set_prim_queue() */




/*
+------------------------------------------------------------------------------
| Function    : ru_countdown_procedure
+------------------------------------------------------------------------------
| Description : The function ru_countdown_procedure() calculates the countdown
|               value. After Contention resoultion is succesfully completed or
|               at changing of the Coding scheme(indicateded in packet 
|               uplink ack/nack or packet timeslot reconfigure), the countdown 
|               value must be recalculated. 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE ru_countdown_procedure ( UBYTE ret_blocks )
{ 
  USHORT  x;
  UBYTE   result;
  TRACE_FUNCTION( "ru_countdown_procedure" );

  /*
   * ret_blocks is the number of blocks which has to retransmit
   * formular for cv calculation: x = round((TBC-bsn´-1)/nts)
   * here tbc = (TBC-bsn) so that
   *       x  = round((tbc-1)/nts)
   * nr of pdu bundaries is also considered for calculating tbc
   */
  
#ifdef _SIMULATION_
  TRACE_EVENT_P3("bs_cv_max=%d   rlc_oct_cnt=%d data_size=%d",grlc_data->uplink_tbf.bs_cv_max,grlc_data->ru.rlc_octet_cnt,grlc_data->ru.rlc_data_size);
#endif /* #ifdef _SIMULATION_ */


  if(grlc_data->testmode.mode EQ CGRLC_LOOP)
  {
    TRACE_EVENT("testmode B: cv value=15");
    return (15); 
  }

  grlc_data->ru.tbc = (grlc_data->ru.rlc_octet_cnt+grlc_data->ru.pdu_boundaries) / grlc_data->ru.rlc_data_size;
  if( (grlc_data->ru.rlc_octet_cnt+grlc_data->ru.pdu_boundaries) % grlc_data->ru.rlc_data_size) /*round upwards*/
    grlc_data->ru.tbc++;


  x   = (grlc_data->ru.tbc -1 + ret_blocks) / grlc_data->ru.nts_max; 

  if( (grlc_data->ru.tbc -1 + ret_blocks ) % grlc_data->ru.nts_max) /*round upwards*/
    x++;


  if(x > grlc_data->uplink_tbf.bs_cv_max)
  {
    result = 15;
    if(grlc_data->ru.cd_active)
    {
      grlc_data->ru.cd_active = FALSE;
      ru_set_prim_queue(grlc_data->ru.cd_active);
    }
  }
  else
  { 
    result = (UBYTE)x;
    if(!grlc_data->ru.cd_active AND 
      ((grlc_data->ru.state NEQ RU_WAIT_FOR_FIRST_CALL_ACK) OR (grlc_data->ru.state NEQ RU_WAIT_FOR_FIRST_CALL_UACK)))
    {
      grlc_data->ru.cd_active = TRUE;
      ru_set_prim_queue(grlc_data->ru.cd_active);
    }
  }

  return result;
  
} /* ru_countdown_procedure() */




/*
+------------------------------------------------------------------------------
| Function    : ru_update_vb
+------------------------------------------------------------------------------
| Description : The function ru_update_vb() refreshs the VB array field after
|               receiving a packet uplink ack/nack.
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_update_vb ( void )
{ 
  MCAST(d_ul_ack,D_GRLC_UL_ACK);
  UBYTE bsn, ssn, i;
  TRACE_FUNCTION( "ru_update_vb" );

  ssn = d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.ssn;   
  /* 
   * Check if ssn is valid
   */
  if(!ru_within_window(ssn,grlc_data->ru.vs,grlc_data->ru.va))
  {
    TRACE_EVENT_P4( "SSN OUTSIDE WINDOW: ssn=%d, vs=%d, va=%d f_ack_ind=%d",
                                ssn,
                                grlc_data->ru.vs,
                                grlc_data->ru.va, 
                                d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.f_ack_ind);
      return;
  }
  grlc_data->ru.nr_nacked_blks = 0;
  for(i = 1; i <= WIN_SIZE; i++)
  {
    bsn = (ssn - i) & 0x7F;  /*mod 128*/
    if(d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.rbb[WIN_SIZE-i])
    {
      if(grlc_data->ru.vb[bsn & WIN_MOD] NEQ VB_ACKED)
      {
        grlc_data->ru.vb[bsn & WIN_MOD] = VB_ACKED;
      }
    }
    else if(grlc_check_if_tbf_start_is_elapsed ( grlc_data->ru.rlc_data[bsn & WIN_MOD].T3198, grlc_data->ul_fn))
    {
      /*
       * timeout T3198
       */
      grlc_data->ru.vb[bsn & WIN_MOD] = VB_NACKED;
      grlc_data->ru.nr_nacked_blks++;
    }
    else
    {
      TRACE_EVENT_P3("BSN =%d not acked. T3198=%ld not expired (c_fn=%ld). vb not modified"
                                                                        ,bsn
                                                                        ,grlc_data->ru.rlc_data[bsn & WIN_MOD].T3198
                                                                        ,grlc_data->ul_fn);
    }
    if (bsn EQ grlc_data->ru.va)
      i = WIN_SIZE+1; /* break cobdition*/
  }

} /* ru_update_vb() */




/*
+------------------------------------------------------------------------------
| Function    : ru_calc_rlc_data_block
+------------------------------------------------------------------------------
| Description : The funcion ru_calc_rlc_data_block() builds a complete RLC
|               data block. 
|
| Parameters  : bsn_i - bsn value of the calculated RLC data block
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_calc_rlc_data_block ( UBYTE bsn_i )
{ 
  USHORT  rlc_len;
  UBYTE   li_cnt, data_cnt;
  TRACE_FUNCTION( "ru_calc_rlc_data_block" );
  
  /* 
   * calculate the countdown value, no retransmission
   */
  grlc_data->ru.cv = ru_countdown_procedure(0);
  grlc_data->ru.rlc_data[bsn_i & WIN_MOD].block_status = grlc_data->ru.block_status;  

  /*
   * set MAC header
   */
  grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.mac = (grlc_data->ru.cv  <<  2 |
                                             grlc_data->r_bit            );
  /*
   * set RLC header values
   */
  grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.ti      = grlc_data->ru.ti;

  /* 
   * data handling 
   */
  rlc_len = grlc_data->ru.rlc_data_size;
  data_cnt = 0;
  li_cnt   = 0;

  if(grlc_data->testmode.mode EQ CGRLC_LOOP)
  {
    grlc_data->ru.sdu_len       = 7777;                       /* random value */
    grlc_data->ru.sdu_off       = 0;
    rlc_len                    = 50;
    grlc_data->ru.rlc_octet_cnt = 7777;                       /* to be in line with sdu_len*/
    TRACE_EVENT("testmode B: get data from received downlink"); 
  }


    
  while ( (grlc_data->ru.sdu_len < rlc_len)  AND  (grlc_data->ru.sdu_len > 0) )
  { 
    /* 
     * end of pdu in block reached
     */
    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.li_me[li_cnt]= (UBYTE) grlc_data->ru.sdu_len;
    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.li_me[li_cnt] <<= 2;

    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].ptr_data  = 
                    &(grlc_data->prim_queue[grlc_data->ru.active_prim].prim_ptr->sdu);
    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].l_buf     = grlc_data->ru.sdu_len * 8;
    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].o_buf     = grlc_data->ru.sdu_off * 8;
    rlc_len -= grlc_data->ru.sdu_len + 1;
    grlc_data->ru.rlc_octet_cnt -= grlc_data->ru.sdu_len;
    grlc_data->ru.sdu_len       -= grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].l_buf/8;
    grlc_data->ru.sdu_off       += grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].l_buf/8;
    ru_get_next_sdu();
    if((grlc_data->ru.sdu_len > 0) AND rlc_len)
    {
      /*
       * one more sdu in queue
       */
      grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.li_me[li_cnt]   |=  0x02;
    }
    else
    {
      /* 
       * no more sdu in queue 
       */
      grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.li_me[li_cnt]   |=  0x01;
    }
    data_cnt++;
    li_cnt++;
    if(li_cnt > NR_OF_PDUS_PER_RLCMAC_BLOCK)
    {
      TRACE_EVENT_P2("li_cnt=%d NR_OF_PDUS_PER_RLCMAC_BLOCK=%d",li_cnt,NR_OF_PDUS_PER_RLCMAC_BLOCK);
      TRACE_ERROR("ru li_cnt bigger than RD_LI_CNT_MAX (=8)");
      TRACE_ASSERT( li_cnt > NR_OF_PDUS_PER_RLCMAC_BLOCK );
      return;
    }
  }
  
  if((grlc_data->ru.sdu_len EQ rlc_len) AND  (grlc_data->ru.sdu_len > 0) AND (grlc_data->ru.cv EQ 0))
  { 
    /*
     * end of pdu; rest of pdu match exactly into the rest of a rlc data block, last uplink block
     */
    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].ptr_data      = 
                       &(grlc_data->prim_queue[grlc_data->ru.active_prim].prim_ptr->sdu);       /*lint !e661*/

    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].l_buf         = grlc_data->ru.sdu_len *8;        /*lint !e661*/
    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].o_buf         = grlc_data->ru.sdu_off *8;        /*lint !e661*/
    data_cnt++;
    grlc_data->ru.rlc_octet_cnt -= rlc_len;
    ru_get_next_sdu();
  }
  else if((grlc_data->ru.sdu_len EQ rlc_len) AND  (grlc_data->ru.sdu_len > 0))
  { 
    /*
     * end of pdu; rest of pdu match exactly into the rest of a rlc data block,
     * split into two blocks, len set to zero, indicates that the end of pdu is not reached
     */
    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.li_me[li_cnt]  =  0x01; /*lint !e661*/

    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].ptr_data      = 
                       &(grlc_data->prim_queue[grlc_data->ru.active_prim].prim_ptr->sdu);       /*lint !e661*/
    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].l_buf         = (grlc_data->ru.sdu_len-1) *8;    /*lint !e661*/
    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].o_buf         = grlc_data->ru.sdu_off *8;        /*lint !e661*/
    data_cnt++;
    li_cnt++;
    rlc_len--;
    grlc_data->ru.sdu_len       -= rlc_len;
    grlc_data->ru.sdu_off       += rlc_len;
    grlc_data->ru.rlc_octet_cnt -= rlc_len;         
    if(li_cnt > NR_OF_PDUS_PER_RLCMAC_BLOCK)
    {
      TRACE_EVENT_P2("li_cnt=%d NR_OF_PDUS_PER_RLCMAC_BLOCK=%d",li_cnt,NR_OF_PDUS_PER_RLCMAC_BLOCK);
      TRACE_ERROR("ru2 li_cnt bigger than RD_LI_CNT_MAX (=8)");
      TRACE_ASSERT( li_cnt > NR_OF_PDUS_PER_RLCMAC_BLOCK );
      return;
    }
  }
  else   if( (grlc_data->ru.sdu_len > 0)  AND rlc_len)
  { 
    /* 
     * only a part of the sdu matches into the rlc  data block
     */
    if(grlc_data->testmode.mode EQ CGRLC_LOOP)
    {
      grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].ptr_data = NULL;                               /*lint !e661*/
      TRACE_EVENT("set data ptr for testmode B to NULL");
    }
    else
    {
      grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].ptr_data = 
                   &(grlc_data->prim_queue[grlc_data->ru.active_prim].prim_ptr->sdu);           /*lint !e661*/
    }
        grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].l_buf    = rlc_len * 8;                      /*lint !e661*/
    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data[li_cnt].o_buf    = grlc_data->ru.sdu_off * 8;            /*lint !e661*/
    data_cnt++;
    grlc_data->ru.sdu_len       -= rlc_len;
    grlc_data->ru.sdu_off       += rlc_len;
    grlc_data->ru.rlc_octet_cnt -= rlc_len;
  }
  /*
   * LI fied parameters are set
   */
  if(grlc_data->testmode.mode EQ CGRLC_LOOP)
  {
    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.e_bit = grlc_data->testmode.rec_data[0].e_bit;
  }
  else if(li_cnt)
  {
    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.e_bit            = 0;
    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.li_me[li_cnt-1] |= 0x01;
        grlc_data->ru.pdu_boundaries -= li_cnt;
#ifdef _SIMULATION_
  TRACE_EVENT_P2("li_cnt=%d,remaining pdu boundaries=%d",
                                                        li_cnt,
                                                        grlc_data->ru.pdu_boundaries);
#endif /* #ifdef _SIMULATION_ */

  }
  else
  {
    grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.e_bit = 1;
  }

  grlc_data->ru.rlc_data[bsn_i & WIN_MOD].header.li_cnt = li_cnt;
  grlc_data->ru.rlc_data[bsn_i & WIN_MOD].data_cnt      = data_cnt;
  


} /* ru_calc_rlc_data_block() */


/*
+------------------------------------------------------------------------------
| Function    : ru_handle_n3102
+------------------------------------------------------------------------------
| Description : The function ru_handle_n3102() handles the counter N3102.
|               If the input parameter ia PAN_INC, then the counter is 
|               incremented. If the input parameter ia PAN_DEC, then the 
|               counter is decremented. 
|
| Parameters  : pan_cnt_i - determines, if the counter is either in- or 
|               decremented
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_handle_n3102 ( T_PAN_CNT  pan_cnt_i )
{ 
  TRACE_FUNCTION( "ru_handle_n3102" );

  if((grlc_data->N3102 NEQ 0xFF)   AND 
     grlc_data->pan_struct.inc     AND
     grlc_data->pan_struct.dec     AND
     grlc_data->pan_struct.pmax)
  {

    switch( pan_cnt_i )
    {
    case PAN_INC:
      grlc_data->N3102 += grlc_data->pan_struct.inc;  
      grlc_data->N3102 = MINIMUM(grlc_data->N3102,grlc_data->pan_struct.pmax);
      
      /*TRACE_EVENT_P4( "INC N3102: inc=%d, dec=%d, pan_max=%d n3102=%d",
                      grlc_data->pan_struct.inc,
                      grlc_data->pan_struct.dec,
                      pgrlc_data->pan_struct.pmax,
                      grlc_data->N3102 );*/
      break;
    case PAN_DEC:
      if( grlc_data->N3102 > grlc_data->pan_struct.dec )
      {
        grlc_data->N3102 -= grlc_data->pan_struct.dec;

        TRACE_EVENT_P4( "DEC1 N3102: inc=%d, dec=%d, pan_max=%d n3102=%d",
                        grlc_data->pan_struct.inc,
                        grlc_data->pan_struct.dec,
                        grlc_data->pan_struct.pmax,
                        grlc_data->N3102 );

        sig_ru_tm_error_ra();
      }
      else
      {
        grlc_data->N3102 = 0;

        TRACE_EVENT_P4( "DEC2 N3102: inc=%d, dec=%d, pan_max=%d n3102=%d",
                        grlc_data->pan_struct.inc,
                        grlc_data->pan_struct.dec,
                        grlc_data->pan_struct.pmax,
                        grlc_data->N3102);
        sig_ru_tm_error_ra();
      }
      break;
    default:
      TRACE_ERROR("unknown type for pan_cnt_i");
      break;
    }
  }
  else if(pan_cnt_i EQ PAN_DEC)
  {
    TRACE_EVENT( "IGNORE N3102" );

    sig_ru_tm_error_ra();
  }
} /* ru_handle_n3102() */



/*
+------------------------------------------------------------------------------
| Function    : ru_calc_va
+------------------------------------------------------------------------------
| Description : The function ru_calc_va() claculates the block, that was
|               negatively acknowledged and must be retransmitted in the next
|               uplink block.
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE ru_calc_va ( void )
{ 
  UBYTE result;
  TRACE_FUNCTION( "ru_calc_va" );

  result = (grlc_data->ru.va) & 0x7F /*mod 128*/;
  
  while( (grlc_data->ru.vb[result & WIN_MOD] EQ VB_ACKED) AND
         (result NEQ grlc_data->ru.vs))
  {
    result = (result+1) & 0x7F /*mod 128*/;
  }  
  return result;
} /* ru_calc_va() */


/*
+------------------------------------------------------------------------------
| Function    : ru_set_next_bsn_ret()
+------------------------------------------------------------------------------
| Description : The function ru_set_next_bsn_ret sets the next valid bsn, 
|               which shall be transmitted as next retransmission 
|
| Parameters  :  - description of parameter dummy
|
+------------------------------------------------------------------------------
*/

GLOBAL UBYTE ru_set_next_bsn_ret ( void)
{ 
  UBYTE new_bsn_ret;
  TRACE_FUNCTION( "ru_set_next_bsn_ret" );

     
  new_bsn_ret = (grlc_data->ru.bsn_ret+1) & 0x7F; 
  while( (grlc_data->ru.vb[new_bsn_ret & WIN_MOD] NEQ VB_PENDING_ACK) AND
         (grlc_data->ru.vb[new_bsn_ret & WIN_MOD] NEQ VB_NACKED)      AND 
          new_bsn_ret NEQ grlc_data->ru.vs )
  {  
    new_bsn_ret = (new_bsn_ret+1) & 0x7F;    
  }
  if( new_bsn_ret EQ grlc_data->ru.vs )
  {
    grlc_data->ru.nr_nacked_blks = 0 ;
    new_bsn_ret = grlc_data->ru.va;      
  }

  return new_bsn_ret;

} /* ru_set_next_bsn_ret() */


/*
+------------------------------------------------------------------------------
| Function    : ru_ret_bsn
+------------------------------------------------------------------------------
| Description : The function ru_ret_bsn() is called in RU_REL_ACK. In this state
|               all RLC data blocks are transmitted, but not acknowledged. If
|               more uplink PDCHs are available(i.e. the PL reads valid USF 
|               flags), then the blocks which currently where not acked are 
|               retransmitted. The bsn value of the next block is controled by
|               bsn_ret. 
|
| Parameters  : dummy 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_ret_bsn ( void )
{ 
  
  TRACE_FUNCTION( "ru_ret_bsn" );

 /* TRACE_EVENT_P3("BSN RET: bsn=%d, cv=%d vb=%d"
                                        ,grlc_data->ru.bsn_ret
                                        ,grlc_data->ru.rlc_data[grlc_data->ru.bsn_ret & WIN_MOD].header.cv
                                        ,grlc_data->ru.vb[grlc_data->ru.bsn_ret & WIN_MOD]);
  */
  ru_send_mac_data_req(grlc_data->ru.bsn_ret);
  grlc_data->ru.bsn_ret = ru_set_next_bsn_ret();

} /* ru_ret_bsn() */





/*
+------------------------------------------------------------------------------
| Function    : ru_change_of_cs
+------------------------------------------------------------------------------
| Description : The function ru_change_of_cs() handles the change of the coding
|               scheme. Folling values must be recalculated.
|                 1. rlc_data_len
|                 2. rlc_octet_cnt
|
| Parameters  : cs_type_i - new coding scheme
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_change_of_cs ( T_CODING_SCHEME  cs_type_i )
{ 
  TRACE_FUNCTION( "ru_change_of_cs" );

  grlc_data->ru.rlc_data_size = ru_calc_rlc_data_size(cs_type_i,grlc_data->ru.ti);
  grlc_data->ru.block_status  = ru_set_block_status(grlc_data->ru.cs_type);
  if(grlc_data->ru.last_bsn NEQ LAST_BSN_IS_SENT)
  {
    grlc_data->ru.rlc_octet_cnt   = ru_recalc_rlc_oct_cnt();
    grlc_data->ru.reorg_l1_needed = TRUE;
  }
  else
  {
    TRACE_EVENT_P3("No REORG dueto CS change requried: rlc_cnt=%ld l_bsn=%d cv=%d"
                                                                    ,grlc_data->ru.rlc_octet_cnt
                                                                    ,grlc_data->ru.last_bsn
                                                                    ,grlc_data->ru.cv);
  }
} /* ru_change_of_cs() */



/*
+------------------------------------------------------------------------------
| Function    : ru_contention_resolution
+------------------------------------------------------------------------------
| Description : The function ru_contention_resolution() checks at one phase 
|               access after receiving a packet uplink ack/nack message the 
|               contention resolution TLLI.
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL ru_contention_resolution ( void )
{ 
  MCAST(d_ul_ack,D_GRLC_UL_ACK);
  BOOL result = FALSE;

  TRACE_FUNCTION( "ru_contention_resolution" );

  if((grlc_data->ru.ti) AND (d_ul_ack->gprs_ul_ack_nack_info.v_cr_tlli) )
  {
    ULONG tlli;
    /* 
     * TLLI received and Contention resolution not yet processed 
     */
    tlli = grlc_buffer2ulong( &d_ul_ack->gprs_ul_ack_nack_info.cr_tlli );
    if(tlli  EQ grlc_data->uplink_tbf.tlli)
      result = TRUE;
    grlc_data->ru.N3104 = 0;
    vsi_t_stop(GRLC_handle,T3166);  
    if(result)
    { 
      /* 
       * correct TLLI received, contention resolution succesful 
       */
      grlc_data->ru.ti = 0;
      grlc_data->ru.rlc_data_size = ru_calc_rlc_data_size(grlc_data->ru.cs_type,grlc_data->ru.ti);
      grlc_data->ru.block_status  = ru_set_block_status(grlc_data->ru.cs_type);
      /* 
       * needed, because nr of pdu boundaries could be change
       */
      grlc_data->ru.rlc_octet_cnt = ru_recalc_rlc_oct_cnt(); 
      sig_ru_tm_cs();
      result = FALSE;
	  grlc_data->ru.reorg_l1_needed = TRUE;  
    }
    else
    {
      /* 
       * wrong TLLI received, contention resolution failed, TBF will be aborted
       */
      result = TRUE;
      TRACE_ERROR( "TLLI error occured" );
      sig_ru_tm_error_ra();
    }
  }
  else if(grlc_data->ru.ti EQ 0)
    result = FALSE;

  return result;
  
} /* ru_contention_resolution() */



/*
+------------------------------------------------------------------------------
| Function    : ru_delete_prims
+------------------------------------------------------------------------------
| Description : The function ru_delete_prims() deletes LLC PDUs from the 
|               primitive queue which are positivlely acknowledged(indirectly 
|               by the highest bsn value in the rrb field included in the 
|               packet uplink ack/nack).
|
| Parameters  : last_bsn_i - all primitives including last_bsn_i are deleted.
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_delete_prims ( UBYTE last_bsn_i )
{ 
  UBYTE cnt=0, prim_type=CGRLC_PRIM_TYPE_OTHER;
  BOOL  all_null_frames = TRUE;

  TRACE_FUNCTION( "ru_delete_prims" );
  /* 
   * check if last_bsn_i is outside window 
   */
  while( (grlc_data->prim_start_tbf < PRIM_QUEUE_SIZE_TOTAL) AND
         (grlc_data->prim_queue[grlc_data->prim_start_tbf].last_bsn NEQ 0xFF) AND 
         (ru_within_window( last_bsn_i,
                            grlc_data->ru.vs,
                            grlc_data->prim_queue[grlc_data->prim_start_tbf].last_bsn)))
  {
    cnt++;

    if( (prim_type NEQ CGRLC_PRIM_TYPE_GMM) AND 
        (grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_ptr->cause EQ GRLC_DTACS_MOBILITY_MANAGEMENT) )
    {
      prim_type = CGRLC_PRIM_TYPE_GMM;
    }

    if( (all_null_frames EQ TRUE) AND
        (grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_ptr->cause NEQ 
         GRLC_DTACS_CELL_NOTIFI_NULL_FRAME) )
    {
      /* This variable is set to FALSE in case there is at least ont not NULL frame */
      all_null_frames = FALSE;
    }

    sig_ru_tm_prim_delete();
  }

  if(cnt)
  {
    if( grlc_data->ready_timer.handling  EQ  READY_TIMER_HANDLING_ENABLED AND
        all_null_frames                  EQ  FALSE                        AND
        grlc_data->ready_timer.value     NEQ CGRLC_STANDBY                    )
    {
      if (grlc_data->ready_timer.value NEQ CGRLC_DEACTIVATED)
      {
        /* The Ready Timer will be restarted in case of at least one not NULL frame and 
           valid value (<>0 and <> 0xFFFFFFFF) */
        vsi_t_start(GRLC_handle,T3314, grlc_data->ready_timer.value );
      }

      grlc_enter_ready_state( );
    }

    if(   grlc_data->ready_timer.handling EQ READY_TIMER_HANDLING_DISABLED       OR
        ( grlc_data->ready_timer.handling EQ READY_TIMER_HANDLING_ENABLED  AND
          prim_type                       EQ CGRLC_PRIM_TYPE_GMM               )    )
    {
      PALLOC(cgrlc_trigger_ind,CGRLC_TRIGGER_IND); /* T_CGRLC_TRIGGER_IND */
      cgrlc_trigger_ind->prim_type = prim_type;
      PSEND(hCommGMM,cgrlc_trigger_ind);
    }
  }
 

 
#ifdef _SIMULATION_
  TRACE_EVENT_P5("%d PRIMS deleted: last_bsn=%d  vs=%d prim_start_tbf=%d, prim_cnt=%d ",
                   cnt,last_bsn_i,grlc_data->ru.vs,grlc_data->prim_start_tbf,grlc_data->grlc_data_req_cnt); 
  
  TRACE_EVENT_P3("pdu_cnt=%d,pdu_sent=%d,pdu_rem=%d",
                                                    grlc_data->ru.pdu_cnt,
                                                    grlc_data->ru.pdu_sent,
                                                    grlc_data->ru.pdu_rem );
#endif /* #ifdef _SIMULATION_ */  
} /* ru_delete_prims() */



/*
+------------------------------------------------------------------------------
| Function    : ru_handle_n3104
+------------------------------------------------------------------------------
| Description : The function ru_handle_n3104() controls the counter N3104. If
|               the counter reaches it maximum value, then the tbf is released.
|               The counter is ignored, if the contention resolution is 
|               successfully completed. Evry time at receiption a packet uplink
|               ack/nack without including TLLI the counter is incremented. 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL ru_handle_n3104 ( void)
{ 
  UBYTE result = FALSE;
  TRACE_FUNCTION( "ru_handle_n3104" );

  if(grlc_data->ru.ti)
  {
    grlc_data->ru.N3104 = (UBYTE)grlc_data->ru.cnt_ts;
    if(grlc_data->ru.N3104 EQ grlc_data->ru.N3104_MAX)
    {      
      result = TRUE;
      vsi_t_stop(GRLC_handle,T3166);
    }
  }
  return result;
  
} /* ru_handle_n3104() */



/*
+------------------------------------------------------------------------------
| Function    : ru_send_control_block
+------------------------------------------------------------------------------
| Description : The function ru_send_control_block() sends a control block
|               instead of RLC data block
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_send_control_block ( void )
{
  UBYTE index;

  TRACE_FUNCTION( "ru_send_control_block" );

#ifdef _SIMULATION_

  memset( &grlc_data->ru.ul_data[grlc_data->ru.write_pos_index],
          0, sizeof( T_ul_data ) );

#endif /* #ifdef _SIMULATION_ */

  grlc_data->ru.ul_data[grlc_data->ru.write_pos_index].block_status = 2;

  index = tm_cpy_ctrl_blk_to_buffer
            ( ( UBYTE* )grlc_data->ru.ul_data[grlc_data->ru.write_pos_index].ul_block );

#ifdef _SIMULATION_
  {
    PALLOC(mac_data_req,MAC_DATA_REQ);
    memset(&(mac_data_req->ul_data),
           0, 
           sizeof(T_ul_data));
    memcpy(&(mac_data_req->ul_data),
           &(grlc_data->ru.ul_data[grlc_data->ru.write_pos_index]),
           sizeof(T_ul_data));
    PSEND(hCommL1,mac_data_req);
  }
  TRACE_EVENT_P1("wpi %d",grlc_data->ru.write_pos_index);
#else  /* #ifdef _SIMULATION_ */
  {
    TRACE_MEMORY_PRIM ( hCommGRLC, hCommL1, MAC_DATA_REQ,
                        &grlc_data->ru.ul_data[grlc_data->ru.write_pos_index],
                        sizeof(T_ul_data) );
  }
#endif /* #ifdef _SIMULATION_ */

  {
    UBYTE* ul_block = ( UBYTE* )grlc_data->ru.ul_data[grlc_data->ru.write_pos_index].ul_block;

    TRACE_BINDUMP( hCommGRLC, TC_USER4,
                   cl_rlcmac_get_msg_name
                     ( ( UBYTE )( ul_block[1] >> 2 ), RLC_MAC_ROUTE_UL ),
                   ul_block, MAX_L2_FRAME_SIZE ); /*lint !e569*/
  }

  grlc_data->ru.write_pos_index++;
  grlc_data->ru.ul_data[grlc_data->ru.write_pos_index].block_status = 0;


  grlc_data->ru.pl_retrans_current.blk[grlc_data->ru.pl_retrans_current.cnt] = 
                                                 index + OFFSET_CTRL_BLOCK_IDX;
  grlc_data->ru.pl_retrans_current.cnt++;
  grlc_data->ru.nts--;  


} /* ru_send_control_block() */


/*
+------------------------------------------------------------------------------
| Function    : ru_stall_ind
+------------------------------------------------------------------------------
| Description : The function ru_stall_ind() handles the stall indication. 
|               If stall indication occurs, the the window is retransmitted if
|               uplink PDCHs are available.
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_stall_ind ( void)
{ 
  TRACE_FUNCTION( "ru_stall_ind" );

  /* set the si bit */
  grlc_data->ru.rlc_data[grlc_data->ru.last_si_block & WIN_MOD].header.mac |= 0x02;
  /* send block with set si bit */
  ru_send_mac_data_req(grlc_data->ru.last_si_block);  
  /* reset the si bit */
  grlc_data->ru.rlc_data[grlc_data->ru.last_si_block & WIN_MOD].header.mac &= 0xFD;

  grlc_data->ru.last_si_block = (grlc_data->ru.last_si_block+1)  & 0x7F;  
  if(grlc_data->ru.last_si_block EQ grlc_data->ru.vs)
    grlc_data->ru.last_si_block = grlc_data->ru.va ;

} /* ru_stall_ind() */


/*
+------------------------------------------------------------------------------
| Function    : ru_new_data
+------------------------------------------------------------------------------
| Description : The function ru_new_data() calculates the next in sequence data
|               and transmits it.
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_new_data ( void)
{ 
  TRACE_FUNCTION( "ru_new_data" );

  ru_calc_rlc_data_block(grlc_data->ru.vs);
  ru_send_mac_data_req(grlc_data->ru.vs);
  grlc_data->ru.rlc_data[grlc_data->ru.vs & WIN_MOD].cnt_pl_trans = 0;  /*first try to transmit*/
  grlc_data->ru.vs = (grlc_data->ru.vs+1) & 0x7F /*mod 128*/;
} /* ru_new_data() */




/*
+------------------------------------------------------------------------------
| Function    : ru_reorg_l1
+------------------------------------------------------------------------------
| Description : The function ru_reorg_l1() recalculates data blocks which are
|				passed to l1 but wasn´t sent due to usf only after succesfull 
|				contention resolution or change of coding scheme.
|
| Parameters  : sent_blks_i - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_reorg_l1 ( UBYTE sent_blks_i )
{ 
  UBYTE cnt_reog_blks; /* number of reorged blks */
  UBYTE reorged_blks = 0; /*counts the number of modified blocks*/
  UBYTE i, bsn;

  TRACE_FUNCTION( "ru_reorg_l1" );


  cnt_reog_blks = grlc_data->ru.nts_max - sent_blks_i;

  for (i=0; i < cnt_reog_blks; i++)
  {
	  UBYTE index;

	  index = grlc_data->ru.pl_retrans_current.cnt - 1 - i;
	  bsn   = grlc_data->ru.pl_retrans_current.blk[index];
	  if(bsn <= BSN_MAX)
	  {	
#ifdef _SIMULATION_
    TRACE_EVENT_P1("bsn %d",bsn);
#endif
      if(!grlc_data->ru.rlc_data[bsn & WIN_MOD].cnt_pl_trans)
      {/* not transmitted over physical link */
        UBYTE cnt_data_parts;
        reorged_blks++;
        grlc_data->ru.vs--;
        grlc_data->ru.vs      = grlc_data->ru.vs & 0x7F;

        cnt_data_parts              =  grlc_data->ru.rlc_data[bsn & WIN_MOD].data_cnt - 1;
        grlc_data->ru.sdu_len       +=  (grlc_data->ru.rlc_data[bsn & WIN_MOD].data[cnt_data_parts].l_buf/8);
        grlc_data->ru.sdu_off        =  (grlc_data->ru.rlc_data[bsn & WIN_MOD].data[cnt_data_parts].o_buf/8);
        grlc_data->ru.rlc_octet_cnt +=  (grlc_data->ru.rlc_data[bsn & WIN_MOD].data[cnt_data_parts].l_buf/8);

        if(cnt_data_parts)
          grlc_data->ru.sdu_len = 0; /* pdu bound in block*/

        while(cnt_data_parts--)
        {
          grlc_data->ru.sdu_len       += (grlc_data->ru.rlc_data[bsn & WIN_MOD].data[cnt_data_parts].l_buf/8);
          grlc_data->ru.sdu_off        = grlc_data->ru.rlc_data[bsn & WIN_MOD].data[cnt_data_parts].o_buf/8;
          grlc_data->ru.rlc_octet_cnt += (grlc_data->ru.rlc_data[bsn & WIN_MOD].data[cnt_data_parts].l_buf/8);
          grlc_data->ru.next_prim       = grlc_data->ru.active_prim;
          grlc_data->ru.active_prim     = grlc_data->prim_queue[grlc_data->ru.active_prim].previous;
          grlc_data->prim_queue[grlc_data->ru.active_prim].last_bsn = grlc_data->prim_queue[grlc_data->ru.next_prim].last_bsn;
          TRACE_EVENT("parts of pdu present");
        }
        /*
         * handle pdu parametes
         */        
        if((grlc_data->ru.rlc_data[bsn & WIN_MOD].header.mac & 0x3C) EQ 0) /* countdown value is equal 0*/                    
        {
          grlc_data->ru.pdu_rem  += grlc_data->ru.rlc_data[bsn & WIN_MOD].data_cnt;
          grlc_data->ru.pdu_sent -= grlc_data->ru.rlc_data[bsn & WIN_MOD].data_cnt;
        }
        else if(grlc_data->ru.rlc_data[bsn & WIN_MOD].header.li_cnt AND
               ((grlc_data->ru.rlc_data[bsn & WIN_MOD].header.li_me[grlc_data->ru.rlc_data[bsn & WIN_MOD].header.li_cnt-1] & 0xFC) EQ 0)) 
        {
          grlc_data->ru.pdu_rem  += grlc_data->ru.rlc_data[bsn & WIN_MOD].data_cnt;
          grlc_data->ru.pdu_sent -= grlc_data->ru.rlc_data[bsn & WIN_MOD].data_cnt;
        }
        else
        {
          grlc_data->ru.pdu_rem  += grlc_data->ru.rlc_data[bsn & WIN_MOD].header.li_cnt;
          grlc_data->ru.pdu_sent -= grlc_data->ru.rlc_data[bsn & WIN_MOD].header.li_cnt;
        }
        if(grlc_data->ru.state EQ RU_REL_ACK)
        {
          SET_STATE(RU,RU_ACK);
        }
        else if(grlc_data->ru.state EQ RU_REL_UACK)
        {
          SET_STATE(RU,RU_UACK);
        }
        /*
         * Stop Countdown procedure, if bsn was the first block with CV NEQ 15 
         */
        if(grlc_data->ru.cd_active  AND
           (grlc_data->ru.rlc_data[((--bsn) & 0x7F) & WIN_MOD].header.mac & 0x3C) EQ 15)
        {
          TRACE_EVENT_P4("stop CNT DWN during reorg bsn-1 =%d bs_cv_max=%d,ru.cv=%d rlc_oc_ctn=%ld"
                                                           ,((bsn--) & 0x7F)
                                                           ,grlc_data->uplink_tbf.bs_cv_max
                                                           ,grlc_data->ru.cv
                                                           ,grlc_data->ru.rlc_octet_cnt);
          grlc_data->ru.cd_active = FALSE;
          ru_set_prim_queue(grlc_data->ru.cd_active);
        }

      }
      else
      {
        /*
         * block was once transmitted over physical link
         */
        reorged_blks++;
        /*
        TRACE_EVENT("block was transmitted over pl link, do not modify");
        */
      }
	  }
	  else
	  {
		  TRACE_EVENT("CTRL BLK in L1 queue: reorg not needed");
	  }
  }
  if(reorged_blks)
  {
    grlc_data->ru.write_pos_index       -= reorged_blks;
    grlc_data->ru.pl_retrans_current.cnt -= reorged_blks;
    memset(&grlc_data->ru.pl_retrans_current.blk[grlc_data->ru.pl_retrans_current.cnt],
           0xFF,
           reorged_blks);
  }
  ru_recalc_rlc_oct_cnt();

} /* ru_reorg_l1() */



/*
+------------------------------------------------------------------------------
| Function    : ru_del_prim_in_uack_mode
+------------------------------------------------------------------------------
| Description : The function ru_del_prim_in_uack_mode() deletes primitvies in 
|               rlc unackknowledged mode
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_del_prim_in_uack_mode ( UBYTE rlc_blocks_sent_i)
{ 
  UBYTE highest_bsn = BSN_MAX + 1; /* this represents the highest bsn sent on air */
  UBYTE  counter = 0;
  TRACE_FUNCTION( "ru_del_prim_in_uack_mode" );
  
  for(counter = rlc_blocks_sent_i;counter > 0;counter--)
  {
    
    highest_bsn = grlc_data->ru.pl_retrans_current.blk[counter-1];
    if( highest_bsn < (BSN_MAX + 1) )
    {
      /* data block has been transmitted on air, valid bsn found. */
      highest_bsn=(highest_bsn+1) & BSN_MAX;
      if(grlc_data->ru.cv NEQ 0)
      {
        ru_delete_prims(highest_bsn);
      }
      else if((grlc_data->ru.cv EQ 0) AND (grlc_data->ru.count_cv_0 EQ 4))
      {
        ru_delete_prims(highest_bsn);
      }
      break;
    }
    /* if highest_bsn is greater than BSN_MAX then it is a control block. we should check in 
     * next radio block
     */
  }
} /* ru_del_prim_in_uack_mode() */


/*
+------------------------------------------------------------------------------
| Function    : ru_handle_timers
+------------------------------------------------------------------------------
| Description : The function ru_handle_timers handles the following timers:
|                T3164, T3166, T3180, T3182, T3198
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_handle_timers ( UBYTE rlc_blocks_sent_i)
{
  UBYTE i;
  TRACE_FUNCTION( "ru_handle_timers" );

  /*
   * start timter T3198, l1 has transmitted a block
   */
  for(i=0 ; i < rlc_blocks_sent_i;i++)
  {
    if (grlc_data->ru.pl_retrans_current.blk[i] <= BSN_MAX )
      ru_set_T3198(grlc_data->ru.pl_retrans_current.blk[i]);
  }
  /*
   * first usf is read: stop T3164; wait for first ack/nack: start T3166
   */
  if( rlc_blocks_sent_i AND grlc_data->ru.first_usf)     
  {
    vsi_t_stop(GRLC_handle,T3164);
    grlc_data->ru.first_usf = FALSE;
    grlc_data->t3164_to_cnt = 0; 
  }
  if(grlc_data->ru.ti AND rlc_blocks_sent_i AND (!grlc_data->ru.cnt_ts))
  { 
    /* 
     * Only started in phase access when first data block was sent
     * in case of fixed allocation, after sending the first rlc data block
     * T3166 must be started(Wait for the first acknowledge).
     */
    vsi_t_start(GRLC_handle,T3166,T3166_VALUE);
  }
  /* 
   * start T3182 if at stall indication or
   * start T3182 if final data block was sent or
   * restart T3180 if a data block was sent, only in dynamic allocation
   */
  if (rlc_blocks_sent_i AND grlc_data->uplink_tbf.mac_mode EQ CGRLC_MAC_MODE_DA)
  {
    vsi_t_stop(GRLC_handle,T3180);
    vsi_t_start(GRLC_handle,T3180,T3180_VALUE);
  }
  if(    grlc_data->ru.last_bsn EQ LAST_BSN_STALL_CONDITION
     AND (grlc_t_status( T3182 ) EQ 0))
  {
    vsi_t_start(GRLC_handle,T3182,T3182_VALUE);
    TRACE_EVENT_P6("SI:T3182 is started: vs=%d va=%d dl_fn=%ld rlc_bs=%d bsn[0]=%d bsn[1]=%d"
                                                  ,grlc_data->ru.vs
                                                  ,grlc_data->ru.va
                                                  ,grlc_data->dl_fn
                                                  ,rlc_blocks_sent_i
                                                  ,grlc_data->ru.pl_retrans_current.blk[0]
                                                  ,grlc_data->ru.pl_retrans_current.blk[1]);
  }
  else if(    (grlc_data->ru.last_bsn EQ LAST_BSN_IS_SENT)
          AND (grlc_t_status( T3182 ) EQ 0) /* timer is not running */
          AND (!grlc_data->ru.nr_nacked_blks)) 
  {
    vsi_t_start(GRLC_handle,T3182,T3182_VALUE);
    TRACE_EVENT_P3("T3182 started : vs=%d va=%d  last_bsn=%d "
                                                        ,grlc_data->ru.vs
                                                        ,grlc_data->ru.va
                                                        ,grlc_data->ru.last_bsn);
  }
/*  else
  {
    TRACE_EVENT_P6("NO TIMER START: vs=%d va=%d  last_bsn=%d t3182=%ld t3180=%d rlc_blocks_sent_i=%d"
                                                        ,grlc_data->ru.vs
                                                        ,grlc_data->ru.va
                                                        ,grlc_data->ru.last_bsn
                                                        ,grlc_t_status( T3182 )
                                                        ,grlc_t_status( T3180 )
                                                        ,rlc_blocks_sent_i);
  }
  */
} /* ru_handle_timers() */




/*
+------------------------------------------------------------------------------
| Function    : ru_handle_nts
+------------------------------------------------------------------------------
| Description : The function ru_handle_nts recalutates the parameter 
|               grlc_data->ru.nts in case of layer 1 queue reorganisation
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/

GLOBAL void ru_handle_nts (UBYTE rlc_blocks_sent_i)
{ 
  UBYTE i;
  TRACE_FUNCTION( "ru_handle_nts" );


  if( grlc_data->testmode.mode EQ CGRLC_LOOP      AND 
      (rlc_blocks_sent_i < grlc_data->ru.nts_max)     )
  {
    TRACE_EVENT_P2("TESTMODE B REORG L1: blk_sent = %d nts=nts_max=%d",rlc_blocks_sent_i,grlc_data->ru.nts_max);
    grlc_data->ru.reorg_l1_needed = TRUE; 
  }
  

  grlc_data->ru.nts = rlc_blocks_sent_i;
  
  /*
   * check whether there is a reorganisation of the tansmit queue needed
   * due to pending control blocks 
   */
  if( tm_get_num_ctrl_blck( ) NEQ 0 )
  {
    grlc_data->ru.reorg_l1_needed = TRUE;
  }


  if(   grlc_data->ru.reorg_l1_needed   
    AND (rlc_blocks_sent_i < grlc_data->ru.nts_max ) 
    AND grlc_data->ru.pl_retrans_current.cnt
    AND grlc_data->ru.pl_retrans_current.blk[0] NEQ 0xFF) /* dummy blocks in queue*/
  {    
    ru_reorg_l1(rlc_blocks_sent_i);
    grlc_data->ru.nts = grlc_data->ru.nts_max; 
  }
  else if (!grlc_data->ru.pl_retrans_current.cnt)
  { /* no data block in queue, put max nuber in queue*/
    grlc_data->ru.nts = grlc_data->ru.nts_max;
  }
  /********otherwise check if a positive acknowledged block is queue**********/
  else
  {
    for(i=0; i < grlc_data->ru.pl_retrans_current.cnt;i++)
    {
      if(grlc_data->ru.pl_retrans_current.blk[i] <= BSN_MAX AND 
         (grlc_data->ru.vb[grlc_data->ru.pl_retrans_current.blk[i] & WIN_MOD] EQ VB_ACKED))
      {
        grlc_data->ru.nts = (grlc_data->ru.nts_max -i);
        grlc_data->ru.pl_retrans_current.cnt -= (grlc_data->ru.nts - rlc_blocks_sent_i);
        memset(&grlc_data->ru.pl_retrans_current.blk[i],
             0xFF,
             grlc_data->ru.nts);
        grlc_data->ru.write_pos_index -= grlc_data->ru.nts - rlc_blocks_sent_i;
        break;
      }
    }
  }
  /***********************check if a bsn_ret block is in queue*****************/ 
  for(i=0; i < grlc_data->ru.pl_retrans_current.cnt;i++)
  {
    if(grlc_data->ru.pl_retrans_current.blk[i] EQ grlc_data->ru.bsn_ret)
    {
      grlc_data->ru.bsn_ret = ru_set_next_bsn_ret();    
    }
  }
  /****************************************************************************/ 
  grlc_data->ru.reorg_l1_needed = FALSE;



} /* ru_handle_nts() */


/*
+------------------------------------------------------------------------------
| Function    : ru_check_pl_ret
+------------------------------------------------------------------------------
| Description : handles the restart of timer T3198, and first call of RU 
|               (to stop T3164)
|
| Parameters  : rlc_blocks_sent_i - number of needed blocks by PL(is equal to 
|               the number of sent blocks in the previous radio block)
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_check_pl_ret ( UBYTE rlc_blocks_sent_i)
{
  UBYTE i;
  UBYTE bsn;

  TRACE_FUNCTION( "ru_check_pl_ret" );

    
  /*
   * handle the sent blocks
   */
  for(i=0;i<rlc_blocks_sent_i;i++)
  {
	  bsn = grlc_data->ru.pl_retrans_current.blk[i];
    /*
     * chek if the range is OK
     */
    if(  bsn <= BSN_MAX )          /* data block was sent */
    {  
      /*
       * trace parametes: 1. retranmission counter, byte counter 
       */
      grlc_data->ru.rlc_data[bsn & WIN_MOD].cnt_pl_trans++;
      if(grlc_data->ru.rlc_data[bsn & WIN_MOD].cnt_pl_trans > 1) 
        grlc_data->tbf_ctrl[grlc_data->ul_index].ret_bsn++;
      else
      {
        UBYTE j;
        for(j=0;j<grlc_data->ru.rlc_data[bsn & WIN_MOD].data_cnt;j++)
          grlc_data->tbf_ctrl[grlc_data->ul_index].rlc_oct_cnt +=grlc_data->ru.rlc_data[bsn & WIN_MOD].data[j].l_buf/8;
      }
      /* 
       * a retransmitted block was sent 
       */
      if(bsn EQ grlc_data->ru.bsn_ret)
      {
        grlc_data->ru.bsn_ret = ru_set_next_bsn_ret(); 
      }
      /*
       * last bsn is sent
       */
      if((grlc_data->ru.rlc_data[bsn & WIN_MOD].header.mac & 0x3C) EQ 0) /* countdown value is equal 0*/ 
      {
        grlc_data->ru.last_bsn = LAST_BSN_IS_SENT;
      }
      else if(bsn EQ ((grlc_data->ru.va+WIN_SIZE-1) & 0x7F)
              AND (grlc_data->ru.last_bsn NEQ LAST_BSN_STALL_CONDITION))
      {
        /*
         * stall indication detected
         */
        grlc_data->ru.last_bsn = LAST_BSN_STALL_CONDITION;
        TRACE_EVENT_P6("NEXT DATA STALL INDICATION bsn=%d va=%d vs=%d  cnt_ts=%d last_bsn=%ld dl_fn=%ld"
                                                          ,bsn
                                                          ,grlc_data->ru.va
                                                          ,grlc_data->ru.vs
                                                          ,grlc_data->ru.cnt_ts
                                                          ,grlc_data->ru.last_bsn
                                                          ,grlc_data->dl_fn);
      }
    }
    else if( bsn >= OFFSET_CTRL_BLOCK_IDX AND bsn < 0xFF ) /* control block was sent */
    {
      sig_ru_tm_ctrl_blk_sent( (UBYTE)( bsn - OFFSET_CTRL_BLOCK_IDX ) );
    } 
    else if (bsn EQ 0xFF)
    {
      /* nothing to do : dummy block sent*/
    }
    else /*invalid block was sent, should not appear */
    {
      TRACE_ERROR("INVALID bsn range neither data or control block");
      TRACE_EVENT("INVALID bsn range neither data or control block");
      TRACE_EVENT_P1("bsn = %d ", bsn);
    }    
  }
  ru_handle_timers(rlc_blocks_sent_i);
  /******************delete sent blocks from pl_retrans_current**************/
  memcpy(grlc_data->ru.pl_retrans_current.blk,
          &grlc_data->ru.pl_retrans_current.blk[rlc_blocks_sent_i],
           (grlc_data->ru.nts_max - rlc_blocks_sent_i));
  grlc_data->ru.pl_retrans_current.cnt -= rlc_blocks_sent_i;
  grlc_data->ru.cnt_ts += rlc_blocks_sent_i;

} /* ru_check_pl_ret() */






/*
+------------------------------------------------------------------------------
| Function    : ru_send_ul_dummy_block
+------------------------------------------------------------------------------
| Description : The function ru_send_ul_dummy_block() sends a uplink dummy 
|               control block instead of RLC data block
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_send_ul_dummy_block ( void )
{ 
  MCAST (ul_dummy,U_GRLC_UL_DUMMY);

  TRACE_FUNCTION( "ru_send_ul_dummy_block" );


  memset(&grlc_data->ru.ul_data[grlc_data->ru.write_pos_index],
          0, 
          sizeof(T_ul_data));

  grlc_data->ru.ul_data[grlc_data->ru.write_pos_index].block_status = 2;

  /*
   * set uplink dummy block
   */
  ul_dummy->msg_type = U_GRLC_UL_DUMMY_c;

  grlc_set_buf_tlli( &ul_dummy->tlli_value, grlc_data->uplink_tbf.tlli );
  grlc_encode_ul_ctrl_block( ( UBYTE* ) grlc_data->ru.ul_data[grlc_data->ru.write_pos_index].ul_block,
                            ( UBYTE* )ul_dummy );

#ifdef _SIMULATION_
  {
    PALLOC(mac_data_req,MAC_DATA_REQ);
    memset(&(mac_data_req->ul_data),
           0, 
           sizeof(T_ul_data));
    memcpy(&(mac_data_req->ul_data),
           &(grlc_data->ru.ul_data[grlc_data->ru.write_pos_index]),
           sizeof(T_ul_data));
    PSEND(hCommL1,mac_data_req);
  }
  TRACE_EVENT_P1("wpi %d",grlc_data->ru.write_pos_index);
#else  /* #ifdef _SIMULATION_ */
  {
    TRACE_MEMORY_PRIM ( hCommGRLC, hCommL1, MAC_DATA_REQ,
                        &grlc_data->ru.ul_data[grlc_data->ru.write_pos_index],
                        sizeof(T_ul_data) );
  }
#endif /* #ifdef _SIMULATION_ */

  {
    UBYTE* ul_block = ( UBYTE* )grlc_data->ru.ul_data[grlc_data->ru.write_pos_index].ul_block;
  
    TRACE_BINDUMP( hCommGRLC, TC_USER5,
                   cl_rlcmac_get_msg_name
                     ( ( UBYTE )( ul_block[1] >> 2 ), RLC_MAC_ROUTE_UL ),
                   ul_block, MAX_L2_FRAME_SIZE ); /*lint !e569*/
  }

  grlc_data->ru.write_pos_index++;
  grlc_data->ru.ul_data[grlc_data->ru.write_pos_index].block_status = 0;

  grlc_data->ru.pl_retrans_current.blk[grlc_data->ru.pl_retrans_current.cnt] = 255;
  grlc_data->ru.pl_retrans_current.cnt++;
  grlc_data->ru.nts--;  
} /* ru_send_ul_dummy_block() */




/*
+------------------------------------------------------------------------------
| Function    : ru_handle_stall_ind
+------------------------------------------------------------------------------
| Description : The function ru_handle_stall_ind() checks, if there was a stall
|               condition, and if it is canceled.
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_handle_stall_ind ( void )
{ 
  TRACE_FUNCTION( "ru_handle_stall_ind" );

  if(grlc_data->ru.last_bsn EQ LAST_BSN_STALL_CONDITION)
  {
    if(!(grlc_data->ru.vs EQ ((grlc_data->ru.va + WIN_SIZE) & 0x7F)))
    {/*stall condition eliminated*/
      grlc_data->ru.reorg_l1_needed = TRUE;  /* remove stalled blocks from data queue */      
      vsi_t_stop(GRLC_handle,T3182);
      ru_handle_n3102(PAN_INC);
      if(grlc_data->ru.rlc_mode EQ CGRLC_RLC_MODE_UACK)
      { /*resume with data transfer after stall indication in rlc unacknowledged problem*/
        grlc_data->ru.last_bsn = LAST_BSN_RESUME_UACK_MODE_AFTER_SI; 
      } 
      else
      {
        grlc_data->ru.last_bsn = LAST_BSN_NOT_BULIT;
      }
    }
  }
  else
  {
    ru_handle_n3102(PAN_INC);
    grlc_data->ru.last_si_block = grlc_data->ru.va;
  }
} /* ru_handle_stall_ind() */


/*
+------------------------------------------------------------------------------
| Function    : ru_handle_tbf_start_in_ptm
+------------------------------------------------------------------------------
| Description : The function ru_handle_tbf_start_in_ptm() modifies the parameters
|               for the tbf at reaching the starting time
|
| Parameters  : rlc_blocks_sent_i - number of blocks sent in current in radio block
|               return value is set, if only nts is decrased and not all blocks are sent
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_handle_tbf_start_in_ptm ( UBYTE rlc_blocks_sent_i )
{
  UBYTE  ctrl_blk_active_idx;
  BOOL   realloc_prr_allowed = FALSE;
  TRACE_FUNCTION( "ru_handle_tbf_start_in_ptm" );
  /*
   * reassignment of uplink and tbf starting time elapsed, received either with packet uplink assignment
   * or packet timeslot reconfigure
   */

  
  if(grlc_data->ul_tfi_changed)
  {
    if((rlc_blocks_sent_i < grlc_data->ru.nts_max)  AND                    /* nr of locks which are not sent */
       (grlc_data->ru.cs_type EQ grlc_data->ru.next_tbf_params.cs_type) AND /* coding scheme has not changed  */
       (grlc_data->ru.nts_max <= grlc_data->ru.next_tbf_params.nts))        /* nts increased or equal */
    {
      UBYTE i, bsn[4],cnt;
      /* 
       * change the tfi for blocks which are in l1 queue 
       */
      cnt    = grlc_data->ru.pl_retrans_current.cnt;
      bsn[0] = grlc_data->ru.pl_retrans_current.blk[0];
      bsn[1] = grlc_data->ru.pl_retrans_current.blk[1];
      grlc_data->ru.pl_retrans_current.cnt = 0;
      for (i = 0; i< cnt;i++)
      {
        if(bsn[i] <= BSN_MAX)
        {
          grlc_data->ru.write_pos_index--;      
          ru_send_mac_data_req(bsn[i]);
          TRACE_EVENT_P1("bsn %d modified while tfi is changed",bsn[i]);
        }  
        else
        {
          TRACE_EVENT_P1("bsn %d :TFI FOR CTRL BLOCK CHANGED. no modification",bsn[i]);
        }
      }
    }
    grlc_data->ul_tfi_changed  = FALSE;
  }



  grlc_data->ru.tlli_cs_type = grlc_data->ru.next_tbf_params.tlli_cs_type;
  if(grlc_data->ru.cs_type NEQ grlc_data->ru.next_tbf_params.cs_type)
  {
    TRACE_EVENT_P2("UL assign: CS changed from %d to %d ",grlc_data->ru.cs_type,grlc_data->ru.next_tbf_params.cs_type);
    grlc_data->ru.cs_type      = grlc_data->ru.next_tbf_params.cs_type;
    ru_change_of_cs(grlc_data->ru.cs_type);        
  }
  if(grlc_data->ru.nts_max < grlc_data->ru.next_tbf_params.nts)
  {
    TRACE_EVENT_P6("nts increased from %d to %d, tfi=%d,cnt_ts=%d,vs=%d,va=%d",
                                      grlc_data->ru.nts_max,
                                      grlc_data->ru.next_tbf_params.nts,
                                      grlc_data->ul_tfi,
                                      grlc_data->ru.cnt_ts,
                                      grlc_data->ru.vs,
                                      grlc_data->ru.va);
    grlc_data->ru.nts = (grlc_data->ru.next_tbf_params.nts - grlc_data->ru.nts_max);
    grlc_data->ru.nts_max = grlc_data->ru.next_tbf_params.nts;

    while( grlc_data->ru.nts              AND
           tm_get_num_ctrl_blck( ) NEQ 0  AND 
           ru_ctrl_blk_selection_allowed() )

    { /* 
       * next uplink block is a control block, 
       *  check if countdown procedure is statred or not
       */
      ru_send_control_block( );
    }
    while(grlc_data->ru.nts)
    { 
      ctrl_blk_active_idx = ru_peek_for_ctrl_blk();
      if ((ctrl_blk_active_idx EQ 0xFF) OR
           realloc_prr_allowed EQ TRUE)     /*No control block , form data block*/
      { 
        while(grlc_data->ru.nts AND grlc_data->ru.sdu_len)
        {
          ru_new_data();
        }
        while(grlc_data->ru.nts)
        {
          ru_ret_bsn();
        }
        realloc_prr_allowed = FALSE;
        break;
      }
      else
      {
        
        TRACE_EVENT_P1("reallocation of llc pdu (index)=%d",ctrl_blk_active_idx);
        
        /* if already one PRR in L1 Buffer , replace it with new PRR */
        if (grlc_data->ru.pl_retrans_current.blk[0]
          EQ (BLK_INDEX_TM + OFFSET_CTRL_BLOCK_IDX))
        {      
          grlc_data->ru.write_pos_index--;
          grlc_data->ru.pl_retrans_current.cnt--;
          grlc_data->ru.nts++;  
          TRACE_EVENT("prr in l1 buffer queue,replace with new prr");
        }
        sig_ru_tm_end_of_pdu(ctrl_blk_active_idx);    
        ru_send_control_block();    
        realloc_prr_allowed = TRUE;      
      }
    }
    if(grlc_data->ru.cv EQ 0 AND
       grlc_data->ru.rlc_mode EQ CGRLC_RLC_MODE_ACK )
    {
      SET_STATE(RU,RU_REL_ACK);
    }
    else if(grlc_data->ru.rlc_mode EQ CGRLC_RLC_MODE_ACK)
    {
      SET_STATE(RU,RU_ACK);
    }
    else if(grlc_data->ru.cv EQ 0 AND
       grlc_data->ru.rlc_mode EQ CGRLC_RLC_MODE_UACK )
    {
      SET_STATE(RU,RU_REL_UACK);
    }
    else if(grlc_data->ru.rlc_mode EQ CGRLC_RLC_MODE_UACK)
    {
      SET_STATE(RU,RU_UACK);
    }
    else
    {
      TRACE_EVENT_P2("RLC MODE ??? cv=%d rlc_mode=%d",grlc_data->ru.cv,grlc_data->ru.rlc_mode);
    }
  }
  else if(grlc_data->ru.nts_max > grlc_data->ru.next_tbf_params.nts)
  {
    /* Reorg layer 1 */
    ru_reorg_l1((UBYTE)(rlc_blocks_sent_i));   
    TRACE_EVENT_P7("nts decrased from %d to %d tfi=%d,cnt_ts=%d,va=%d,vs=%d blk_s=%d  ",
                                                                grlc_data->ru.nts_max,
                                                                grlc_data->ru.next_tbf_params.nts,
                                                                grlc_data->ul_tfi,
                                                                grlc_data->ru.cnt_ts,
                                                                grlc_data->ru.va,
                                                                grlc_data->ru.vs,
                                                                rlc_blocks_sent_i);
    grlc_data->ru.nts_max = grlc_data->ru.next_tbf_params.nts;
  }
  else
  {
    TRACE_EVENT_P6("ul tbf reassignment with tfi=%d,cnt_ts=%d,va=%d,vs=%d,nts_max=%d tbf_st_time=%ld",
                                                                grlc_data->ul_tfi,
                                                                grlc_data->ru.cnt_ts,
                                                                grlc_data->ru.va,
                                                                grlc_data->ru.vs,
                                                                grlc_data->ru.nts_max,
                                                                grlc_data->ul_tbf_start_time);
  }
  grlc_data->ru.v_next_tbf_params = FALSE;
  
} /* ru_handle_tbf_start_in_ptm() */


/*
+------------------------------------------------------------------------------
| Function    : ru_switch_ul_buffer 
+------------------------------------------------------------------------------
| Description : The function ru_switch_ul_buffer () .... 
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_switch_ul_buffer  ( UBYTE rlc_blocks_sent_i )
{ 

  UBYTE i;

  TRACE_FUNCTION( "ru_switch_ul_buffer " );

  /*
   * switch uplink buffer, it means, rlc blocks which are not sent
   * by PL are switched to the top of the ul buffer
   */
  if (rlc_blocks_sent_i)
  {
    i = rlc_blocks_sent_i;
    while((i < MAX_UL_TN) AND (grlc_data->ru.ul_data[i].block_status NEQ 0))
    {
      grlc_data->ru.ul_data[i - rlc_blocks_sent_i] = grlc_data->ru.ul_data[i]; 
      i++;
    }
    grlc_data->ru.write_pos_index -= rlc_blocks_sent_i;
  } 
} /* ru_switch_ul_buffer () */

/*
+------------------------------------------------------------------------------
| Function    : ru_cgrlc_st_time_ind 
+------------------------------------------------------------------------------
| Description : The function ru_cgrlc_st_time_ind () informs higher layers
|               that the starting time is elapsed
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_cgrlc_st_time_ind  ( void )
{ 
  PALLOC(cgrlc_starting_time_ind,CGRLC_STARTING_TIME_IND); /* T_CGRLC_STARTING_TIME_IND */

  TRACE_FUNCTION( "ru_cgrlc_st_time_ind " );

  cgrlc_starting_time_ind->tbf_mode = CGRLC_TBF_MODE_UL;
  cgrlc_starting_time_ind->tfi      = grlc_data->ul_tfi;
 
  PSEND(hCommGRR,cgrlc_starting_time_ind);

  grlc_data->ul_tn_mask = grlc_data->uplink_tbf.ts_mask;
  

} /* ru_cgrlc_st_time_ind () */

/*
+------------------------------------------------------------------------------
| Function    : ru_ctrl_blk_selection_allowed
+------------------------------------------------------------------------------
| Description : This function gets called from sig_gff_ru_mac_ready_ind handler 
|               and also from the function which handles ul reassignment
|               (ru_handle_tbf_start_in_ptm). In mac ready indicate handler 
|               this is calld only when ru is in rel_ack state. 
|               This function returns true or false according to the following
|               table. The table applies for uack mode also.
|               When this function returns true, then control block should be
|               selected for transmission by RU.
|               When it returns False, then control block is selected by RD.
|               
|                  ru_rel_ack state          rd_rel_ack         - TRUE
|                  ru_ack                    rd_ack             - TRUE
|                  ru_ack                    rd_rel_ack         - TRUE
|                  ru_rel_ack                rd_ack             - FALSE
|                  ru_ack                    NO DL TBF(rd_null) - TRUE
|                  ru_rel_ack                NO DL TBF(rd_null) - TRUE
|               
|               This function should be called only in PTM.
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
GLOBAL BOOL ru_ctrl_blk_selection_allowed()
{
  BOOL ru_in_release_mode = FALSE, ctrl_blk_allowed = TRUE, dl_release_started = TRUE;

  if(grlc_data->tbf_type EQ TBF_TYPE_CONC)
  {
    /* In ru_send_pca stete, ctrl blocks are not sent. No need to 
     * check for that here.
     */
    if((GET_STATE(RU) EQ RU_REL_ACK) OR (GET_STATE(RU) EQ RU_REL_UACK))
    {
      ru_in_release_mode = TRUE;
    }
  
    sig_ru_rd_get_downlink_release_state(&dl_release_started);
     
    /* rd_rel_state would be true if fbi=1 has been received
     * in downlink.
     */
    if(ru_in_release_mode AND (dl_release_started EQ FALSE) )
    {
      ctrl_blk_allowed = FALSE;
    }
  }
  return ctrl_blk_allowed;
}

/*
+------------------------------------------------------------------------------
| Function    : ru_peek_for_ctrl_blk
+------------------------------------------------------------------------------
| Description : The function would peek to see if there is reallocation set,
|               then it returns TRUE, so that PRR can be constructed. 
|               Also if the rlc block has more than one llc pdu with re-allocation
|               set , then prr os sent for the latest llc pdu. 
| Parameters  : None.
|
+------------------------------------------------------------------------------
*/

#ifndef CF_FAST_EXEC

GLOBAL UBYTE ru_peek_for_ctrl_blk()
{
  BOOL    ctrlblk_found = FALSE; 
  USHORT  rlc_len;
  USHORT  sdu_len;
  UBYTE   active_prim,next_prim,active_prr_idx = 0xFF;

  TRACE_FUNCTION( "ru_peek_for_ctrl_blk" );
  
  sdu_len     = grlc_data->ru.sdu_len;
  active_prim = grlc_data->ru.active_prim;
  next_prim   = grlc_data->ru.next_prim;
  rlc_len     = grlc_data->ru.rlc_data_size;
  
  
  while ( (sdu_len < rlc_len)  AND  (sdu_len > 0) )
  {  
    rlc_len -= sdu_len + 1; /* 1 is for length indicator */    
    
    if (!grlc_data->ru.cd_active)
    {
      ctrlblk_found = ru_peek_next_sdu(&sdu_len,&active_prim,&next_prim);
      active_prr_idx = ctrlblk_found ? active_prim:0xFF;
    }
    else
    {
#ifdef _SIMULATION_
      TRACE_EVENT("countdown in progress and extended tbf not supported,cant build PRR");
#endif
      return active_prr_idx;
    }        
 
  }   
  
  return (ctrlblk_found ? active_prr_idx : 0xFF);
}/*ru_peek_for_ctrl_blk*/
#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : ru_peek_next_sdu
+------------------------------------------------------------------------------
| Description : The function would peek to see if there is reallocation set,
|               for the current llc PDU.
|               
| Parameters  : None.
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL BOOL ru_peek_next_sdu(USHORT *sdu_len,UBYTE *active_prim,UBYTE *next_prim)
{   
  BOOL prr_build = FALSE;

  TRACE_FUNCTION( "ru_peek_next_sdu");  
  
  if( *next_prim NEQ 0xFF  AND
     (grlc_data->prim_queue[*next_prim].start_new_tbf EQ 0))
  {
    *active_prim = grlc_data->prim_queue[*active_prim].next;
    /*  This signal allows TM to initiate resource re-allocation
    *  if required
    */
    prr_build = grlc_data->prim_queue[*active_prim].re_allocation;          
    
    *sdu_len  = grlc_data->prim_queue[*active_prim].prim_ptr->sdu.l_buf/8;
    *next_prim = grlc_data->prim_queue[*active_prim].next;  
  }
  else
  {
   *sdu_len  = 0;   
  }  
  return prr_build;
}/*ru_peek_next_sdu*/
#endif /* CF_FAST_EXEC */
