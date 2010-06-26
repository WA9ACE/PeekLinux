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
|  Purpose :  This module implements local functions for service RD of
|             entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_RDF_C
#define GRLC_RDF_C
#endif

#define ENTITY_GRLC

/*==== INCLUDES =============================================================*/

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
#include "grlc_tms.h" 
#include <string.h>      /* memcpy */
#include "grlc_f.h" 
#include "grlc_rdf.h" 
#include "grlc_meass.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : rd_init
+------------------------------------------------------------------------------
| Description : The function rd_init() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void rd_init ( void )
{ 
  TRACE_FUNCTION( "rd_init" );

  /*
   * UBYTE
   */
  grlc_data->rd.vq              = 0xFF;
  grlc_data->rd.vr              = 0xFF;
  grlc_data->rd.li_cnt          = 0xFF;
  grlc_data->rd.rlc_data_len    = 0xFF;
  grlc_data->rd.f_ack_ind       = 0xFF;
  grlc_data->rd.ssn             = 0xFF;      
        
  /*
   * USHORT
   */
  grlc_data->rd.pdu_len         = 0xFFFF;
  /*
   * BOOL
   */
  grlc_data->rd.pdu_complete        = TRUE;
  grlc_data->rd.channel_req         = FALSE;
  grlc_data->rd.ch_req_in_ack_prog  = FALSE;  
  grlc_data->rd.inSequence          = TRUE;
  /*
   * struct
   */
  grlc_data->rd.ptr_grlc               = NULL;
  grlc_data->rd.next_poll_block        = NEXT_POLL_BLOCK_NONE;
  
  INIT_STATE(RD,RD_NULL);

} /* rd_init() */





/*
+------------------------------------------------------------------------------
| Function    : rd_tbf_init
+------------------------------------------------------------------------------
| Description : The function rd_tbf_init() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void rd_tbf_init ( void )
{ 
  TRACE_FUNCTION( "rd_tbf_init" );

  /* 
   * UBYTE
   */
  grlc_data->rd.vq              = 0;
  grlc_data->rd.vr              = 0;
  grlc_data->rd.li_cnt          = 0;
  grlc_data->rd.ssn             = 0;      
  grlc_data->rd.last_bsn        = 0xFF;
  grlc_data->rd.bsn_pdu_start   = 0xFF; /*set to zero at receiving the first data block*/
  grlc_data->rd.cnt_sent_f_ack  = 0;
  grlc_data->rd.f_ack_ind       = 0;
  grlc_data->dl_tn_mask         = grlc_data->downlink_tbf.ts_mask;

  /*
   * USHORT
   */
  grlc_data->rd.pdu_len         = 0;

  /*
   * BOOL
   */
  grlc_data->rd.release_tbf        = FALSE;
  grlc_data->rd.pdu_complete       = TRUE;
  grlc_data->rd.ch_req_in_ack_prog = FALSE;  
  grlc_data->rd.v_next_tbf_params  = FALSE;
  grlc_data->rd.ignore_pdu         = FALSE;


  grlc_data->rd.fn_p_tbf_rel       = 0xFFFFFFFF;
  /*
   * struct
   */
  grlc_data->rd.rlc_mode               = grlc_data->downlink_tbf.rlc_mode;
  grlc_data->rd.cs_type                = CS_ZERO;
  /*
   * arrays
   */
  
  memset(grlc_data->rd.data_array, 0         , WIN_SIZE      * sizeof(grlc_data->rd.data_array[0]) );
  memset(grlc_data->rd.vn        , VN_INVALID, WIN_SIZE      * sizeof(grlc_data->rd.vn[0])         );
  memset(grlc_data->rd.li        , 0         , RD_LI_CNT_MAX * sizeof(grlc_data->rd.li[0])         );
  memset(grlc_data->rd.m         , 0         , RD_LI_CNT_MAX );

} /* rd_tbf_init() */



/*
+------------------------------------------------------------------------------
| Function    : rd_read_li_m_of_block
+------------------------------------------------------------------------------
| Description : The function rd_read_li_m_of_block() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL rd_read_li_m_of_block ( UBYTE * ptr_data_i, UBYTE e_bit_i )
{
  USHORT  len_sum;
  BOOL    result;

  TRACE_FUNCTION( "rd_read_li_m_of_block" );
  
  len_sum = 0;
  grlc_data->rd.li_cnt = 0;

  while(!(e_bit_i))
  {
    grlc_data->rd.li[grlc_data->rd.li_cnt] = 
               (ptr_data_i[grlc_data->rd.li_cnt] & 0xFC) >> 2;
    grlc_data->rd.m [grlc_data->rd.li_cnt] = 
               (ptr_data_i[grlc_data->rd.li_cnt] & 0x02) >> 1;
    e_bit_i  = (ptr_data_i[grlc_data->rd.li_cnt] & 0x01);
    len_sum += grlc_data->rd.li[grlc_data->rd.li_cnt];
    if((e_bit_i EQ 0) 
	      AND (!grlc_data->rd.m [grlc_data->rd.li_cnt]))
    {
	    /*SZML-RD/001*/
	    return FALSE;
    }
    else if(!(grlc_data->rd.li[grlc_data->rd.li_cnt])
	    AND (grlc_data->rd.m [grlc_data->rd.li_cnt]))
    {
	    TRACE_ERROR("dl block with li=0 and m=1: NOT ALLOWED");
	    return FALSE;
    }
    grlc_data->rd.li_cnt++;
  }

  if(grlc_data->rd.li_cnt > RD_LI_CNT_MAX)
  {
    TRACE_EVENT_P2("li_cnt=%d RD_LI_CNT_MAX=%d",grlc_data->rd.li_cnt,RD_LI_CNT_MAX);
    TRACE_ERROR("rd li_cnt bigger than RD_LI_CNT_MAX (=8)");
    TRACE_ASSERT( grlc_data->rd.li_cnt > RD_LI_CNT_MAX );
    return FALSE;
  }

  /* 
   * check if sum of LIs is longer than a rlc data block len 
   */
  len_sum += grlc_data->rd.li_cnt;
  if( (len_sum > grlc_data->rd.rlc_data_len) 
     OR 
      ((len_sum EQ grlc_data->rd.rlc_data_len) AND (grlc_data->rd.m [grlc_data->rd.li_cnt-1])) )
    result = FALSE;
  else
    result = TRUE;

  return result;

} /* rd_read_li_m_of_block() */



/*
+------------------------------------------------------------------------------
| Function    : rd_out_grlc_data_ind
+------------------------------------------------------------------------------
| Description : The function rd_out_grlc_data_ind() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void rd_out_grlc_data_ind( void )
{ 
  T_GRLC_DATA_IND  *prim_ptr;
  TRACE_FUNCTION( "rd_out_grlc_data_ind" );

  if(grlc_data->rd.ignore_pdu OR
     grlc_test_mode_active())
  {
    /*
     * this pdu is not passed to LLC because MAX_LLC_PDU_SIZE was exceed or testmode is active
     */
    grlc_data->rd.ignore_pdu = FALSE;

    TRACE_EVENT_P4("TARGET PDU END reached at will be ignored: len=%ld, bsn=pdu_start=%d vr = %d testmode=%d "
                                                                                ,grlc_data->rd.pdu_len
                                                                                ,grlc_data->rd.bsn_pdu_start
                                                                                ,grlc_data->rd.vr
                                                                                ,grlc_test_mode_active());

    rd_free_desc_list_partions();
    return;
  }

  if(grlc_data->rd.rlc_mode EQ CGRLC_RLC_MODE_ACK)
  {
    PALLOC_DESC (grlc_data_ind, GRLC_DATA_IND);//lint !e413
    prim_ptr  = grlc_data_ind;
  }
  else
  {
    PALLOC_DESC (grlc_unitdata_ind, GRLC_UNITDATA_IND);//lint !e413
    prim_ptr  = (T_GRLC_DATA_IND*)grlc_unitdata_ind;
  }

  memcpy(  prim_ptr,
           &grlc_data->rd.grlc_data_ind,
           sizeof(T_GRLC_DATA_IND) );
  PSEND(hCommLLC, prim_ptr);

  grlc_data->rd.grlc_data_ind.desc_list.first           = NULL;
  grlc_data->rd.pdu_complete                            = TRUE;
  grlc_data->tbf_ctrl[grlc_data->dl_index].rlc_oct_cnt += grlc_data->rd.pdu_len;
  grlc_data->tbf_ctrl[grlc_data->dl_index].pdu_cnt++;

} /* rd_out_grlc_data_ind() */
  
#ifdef _SIMULATION_

/*
+------------------------------------------------------------------------------
| Function    : rd_out_grlc_data_ind_test
+------------------------------------------------------------------------------
| Description : The function rd_out_grlc_data_ind_test() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void rd_out_grlc_data_ind_test( void )
{ 
  T_NEXT_ARRAY          *ptr_next_array = NULL;
  T_NEXT_ARRAY          *ptr_help = NULL;
  UBYTE                 *ptr_pos  = NULL,cnt=0;
  USHORT                sdu_len_in_bits;
  T_GRLC_DATA_IND_TEST   *prim_ptr = NULL;

  TRACE_FUNCTION( "rd_out_grlc_data_ind_test" );

   
   if(grlc_data->rd.ignore_pdu OR 
     grlc_test_mode_active())
  {
    /*
     * this pdu is not passed to LLC because MAX_LLC_PDU_SIZE was exceed or testmode is active
     */
    grlc_data->rd.ignore_pdu = FALSE;

    TRACE_EVENT_P4("SIMULATION PDU END reached at will be ignored: len=%ld, bsn=pdu_start=%d vr = %d testmode=%d"
                                                                                ,grlc_data->rd.pdu_len
                                                                                ,grlc_data->rd.bsn_pdu_start
                                                                                ,grlc_data->rd.vr
                                                                                ,grlc_test_mode_active());
    rd_free_desc_list_partions();
    return;
  }
  sdu_len_in_bits = grlc_data->rd.pdu_len * 8;

  if(grlc_data->rd.rlc_mode EQ CGRLC_RLC_MODE_ACK)
  {
    PALLOC_SDU (grlc_data_ind_test, GRLC_DATA_IND_TEST, sdu_len_in_bits);
    prim_ptr  = grlc_data_ind_test;
  }
  else
  {
    PALLOC_SDU (grlc_unitdata_ind_test, GRLC_UNITDATA_IND_TEST, sdu_len_in_bits);
    prim_ptr  = (T_GRLC_DATA_IND_TEST*)grlc_unitdata_ind_test;
  }
  {
    /* 
     * copy from description list to test primitive
     */  
    prim_ptr->tlli      = grlc_data->downlink_tbf.tlli;
    prim_ptr->sdu.l_buf = grlc_data->rd.pdu_len * 8;
    prim_ptr->sdu.o_buf = 0;
    ptr_next_array      = 
      (T_NEXT_ARRAY*)grlc_data->rd.grlc_data_ind.desc_list.first;
    ptr_pos = prim_ptr->sdu.buf;
    cnt=0;
    do
    {
      cnt++;
      memcpy(ptr_pos,
            ptr_next_array->data,
            ptr_next_array->len );
      ptr_pos = &ptr_pos[ptr_next_array->len];
      ptr_next_array = (T_NEXT_ARRAY*)ptr_next_array->next;
    }
    while(ptr_next_array NEQ NULL);
    PSEND(hCommLLC, prim_ptr);
    TRACE_EVENT_P1("SEND PARTIONS =%d",cnt);

  }

  grlc_data->tbf_ctrl[grlc_data->dl_index].rlc_oct_cnt += grlc_data->rd.pdu_len;
  grlc_data->tbf_ctrl[grlc_data->dl_index].pdu_cnt++;


  /* delete blocks which are sent to LLC: only possible in test environment
   * usally, deleting of the description list is task of LLC
   */
   rd_free_desc_list_partions();

} /* rd_out_grlc_data_ind_test() */  

#endif /* _SIMULATION_ */




/*
+------------------------------------------------------------------------------
| Function    : rd_send_grlc_data_ind
+------------------------------------------------------------------------------
| Description : The function rd_send_grlc_data_ind() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void rd_send_grlc_data_ind ( UBYTE bsn_i )
{ 
  T_NEXT_ARRAY  *ptr_block;
  UBYTE         pdu_cnt;
  UBYTE         compl_pdu;
  TRACE_FUNCTION( "rd_send_grlc_data_ind" );
  
  
  pdu_cnt   = grlc_data->rd.data_array[bsn_i & WIN_MOD].pdu_cnt;

  if(grlc_data->rd.data_array[bsn_i & WIN_MOD].pdu_complete)
    compl_pdu   = grlc_data->rd.data_array[bsn_i & WIN_MOD].pdu_cnt;
  else if (pdu_cnt)
    compl_pdu   = grlc_data->rd.data_array[bsn_i & WIN_MOD].pdu_cnt -1;
  else
    compl_pdu   = 0;

  if ((ptr_block = grlc_data->rd.data_array[bsn_i & WIN_MOD].first) EQ NULL)
  {
    TRACE_EVENT_P4("NO DATA IND first empty bsn=%d bsn_mod=%d vr=%d vq=%d", bsn_i, bsn_i&WIN_MOD,grlc_data->rd.vr,grlc_data->rd.vq);
    return;
  }
  
  do
  {
    if(grlc_data->rd.pdu_complete)
    {
      grlc_data->rd.ptr_grlc                       = ptr_block;
      grlc_data->rd.grlc_data_ind.desc_list.first  = (ULONG) grlc_data->rd.ptr_grlc;
      grlc_data->rd.pdu_len                        = ptr_block->len;
      grlc_data->rd.bsn_pdu_start                  = bsn_i;
    }
    else
    {
      grlc_data->rd.pdu_len        += ptr_block->len;
      grlc_data->rd.ptr_grlc->next  = (ULONG*) ptr_block;
      grlc_data->rd.ptr_grlc        = (T_NEXT_ARRAY*) grlc_data->rd.ptr_grlc->next;
    }
    grlc_data->rd.pdu_complete    = FALSE;
 
    if(grlc_data->rd.pdu_len > MAX_LLC_PDU_SIZE)
    {
      TRACE_EVENT_P5("PDU SIZE TO BIG = %ld  pdu_st=%d vr=%d compl_pdu=%d pdu_cnt=%d"
                                                              ,grlc_data->rd.pdu_len
                                                              ,grlc_data->rd.bsn_pdu_start
                                                              ,grlc_data->rd.vr
                                                              ,compl_pdu
                                                              ,pdu_cnt);
      if(!compl_pdu)
      {
        /* no pdu in data block */
        grlc_data->rd.ignore_pdu    = TRUE;
        
        /* free only linked partions */
        rd_free_desc_list_partions();

        return;
      }
      else
      {
        /* pdu boundary in data block */
        ptr_block = (T_NEXT_ARRAY*) ptr_block->next;
        grlc_data->rd.ptr_grlc->next = NULL;

        /*
         * free only linked partions
         */
        rd_free_desc_list_partions();
        
        compl_pdu--;       
      }

    }
    else if( compl_pdu )
    {

      TRACE_EVENT_P5("GRLC_DATA_IND len=%ld bsn_start=%d bsn_end=%d las_len=%d dl_fn=%ld"





















                                                                              ,grlc_data->rd.pdu_len
                                                                              ,grlc_data->rd.bsn_pdu_start
                                                                              ,bsn_i
                                                                              ,ptr_block->len
                                                                              ,grlc_data->dl_fn);    
      ptr_block                                      = (T_NEXT_ARRAY*) ptr_block->next;
      grlc_data->rd.ptr_grlc->next                   = NULL;
      grlc_data->rd.grlc_data_ind.desc_list.list_len = grlc_data->rd.pdu_len;
      grlc_data->rd.grlc_data_ind.tlli               = grlc_data->downlink_tbf.tlli;

      #ifdef _TARGET_
      {
          rd_out_grlc_data_ind();
      }
      #     endif   
          
#     ifdef _SIMULATION_
      {
        rd_out_grlc_data_ind_test();
      }
#     endif      
      grlc_data->rd.pdu_complete = TRUE;
      compl_pdu--;
    }
    if(!pdu_cnt)
      pdu_cnt = 0;
    else
      pdu_cnt--;
  }
  while(pdu_cnt);

  grlc_data->rd.pdu_complete = grlc_data->rd.data_array[bsn_i & WIN_MOD].pdu_complete;

  /*
   * block combined, first element reseted
   */
  grlc_data->rd.data_array[bsn_i & WIN_MOD].first = NULL;

} /* rd_send_grlc_data_ind() */



/*
+------------------------------------------------------------------------------
| Function    : rd_check_window_size
+------------------------------------------------------------------------------
| Description : The function rd_check_window_size() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL rd_check_window_size ( UBYTE bsn_i )
{ 
  BOOL  result;

  TRACE_FUNCTION( "rd_check_window_size" );

  if(grlc_data->rd.rlc_mode EQ CGRLC_RLC_MODE_ACK)
  {
    if( bsn_i >= grlc_data->rd.ssn )
    {
      if((bsn_i - grlc_data->rd.ssn) < WIN_SIZE)
      {
        result = TRUE;
      }
      else
      {
        result = FALSE;
      }
    }
    else 
    {
      if((bsn_i + 128 - grlc_data->rd.ssn) < WIN_SIZE)
      {
        result = TRUE;
      }
      else
      {
        result = FALSE;
      }
    }
  }
  else if(grlc_data->rd.rlc_mode EQ CGRLC_RLC_MODE_UACK)
  {
    if( bsn_i >= grlc_data->rd.vr )
    {
      if((bsn_i - grlc_data->rd.vr) < WIN_SIZE)
      {
        result = TRUE;
      }
      else
      {
        result = FALSE;
      }
    }
    else 
    {
      if((bsn_i + 128 - grlc_data->rd.vr) < WIN_SIZE)
      {
        result = TRUE;
      }
      else
      {
        result = FALSE;
      }
    }
  }
  else
  {
    result =FALSE;
    TRACE_EVENT_P4("unknown rlc mode in rd_check_window_size: rlc_mode=%d, bsn_i=%d,ssn=%d,vr=%d "
                                                                                                  ,grlc_data->rd.rlc_mode
                                                                                                  ,bsn_i
                                                                                                  ,grlc_data->rd.ssn
                                                                                                  ,grlc_data->rd.vr);
  }


  return result;
} /* rd_check_window_size() */



/*
+------------------------------------------------------------------------------
| Function    : rd_save_block
+------------------------------------------------------------------------------
| Description : The function rd_save_block() .... 
|
| Parameters  : bsn_i             - block sequence number of the saved block
|               *ptr_data_block_i - pointer the data field of the rlc data 
|                                   block without header and li field
|               fbi_i             - final block indication bit of the received
|                                   data block
|
+------------------------------------------------------------------------------
*/
GLOBAL void rd_save_block ( UBYTE bsn_i, UBYTE * ptr_data_block_i, UBYTE fbi_i)
{ 
  T_NEXT_ARRAY  *ptr_temp=NULL;
  UBYTE         block_nr;
  UBYTE         i;
  UBYTE         *ptr_data;
  USHORT        len;  
  TRACE_FUNCTION( "rd_save_block" );

  if(!(grlc_data->rd.li_cnt))
  {
    /*
     * only a part of a pdu
     */
    MALLOC(ptr_temp, sizeof(T_NEXT_ARRAY));
    ptr_temp->next = NULL;
    ptr_temp->len  = grlc_data->rd.rlc_data_len;
    memcpy((ptr_temp->data),(ptr_data_block_i),(ptr_temp->len));
    grlc_data->rd.data_array[bsn_i & WIN_MOD].first = ptr_temp;
    if(fbi_i)
    {
      grlc_data->rd.data_array[bsn_i & WIN_MOD].pdu_cnt = 1;
      grlc_data->rd.data_array[bsn_i & WIN_MOD].pdu_complete =TRUE;
    }
    else
    {
      grlc_data->rd.data_array[bsn_i & WIN_MOD].pdu_cnt = 0;
      grlc_data->rd.data_array[bsn_i & WIN_MOD].pdu_complete =FALSE;
    }

  }
  else
  {
    /* 
     * for the first part 
     */
    MALLOC(ptr_temp, sizeof(T_NEXT_ARRAY));
    if(grlc_data->rd.li[0] EQ 0 )
      ptr_temp->len  = grlc_data->rd.rlc_data_len-1;
    else
      ptr_temp->len  = grlc_data->rd.li[0];

    ptr_data = ptr_data_block_i;
    memcpy((ptr_temp->data),
           (ptr_data),
           (ptr_temp->len));
    grlc_data->rd.data_array[bsn_i & WIN_MOD].first = ptr_temp;            
    block_nr = 1;
    while((block_nr <= grlc_data->rd.li_cnt) AND (grlc_data->rd.m[block_nr-1]))
    {
      ptr_data       = &(ptr_data[ptr_temp->len]);
      MALLOC(ptr_temp->next, sizeof(T_NEXT_ARRAY));
      ptr_temp       = (T_NEXT_ARRAY *)ptr_temp->next;
      /*
       * check if it islast pdu in rlc data block
       */
      if((block_nr EQ grlc_data->rd.li_cnt)
		      OR (grlc_data->rd.li[block_nr] EQ 0))
      {
        /*
         * last block, len is REST
         */
        len = 0;
        for(i=0; i < grlc_data->rd.li_cnt; i++)
          len += grlc_data->rd.li[i];
        ptr_temp->len  = grlc_data->rd.rlc_data_len - grlc_data->rd.li_cnt - len;
      }
      else
      {
        /* 
         * not last part 
         */
        ptr_temp->len  = grlc_data->rd.li[block_nr];
      }
      memcpy((ptr_temp->data),
             (ptr_data),
             (ptr_temp->len));
      block_nr++;
    }    
    ptr_temp->next = NULL;
    if(grlc_data->rd.m[grlc_data->rd.li_cnt-1] EQ 0)
	    grlc_data->rd.data_array[bsn_i & WIN_MOD].pdu_cnt = grlc_data->rd.li_cnt;
	  else
	    grlc_data->rd.data_array[bsn_i & WIN_MOD].pdu_cnt = grlc_data->rd.li_cnt + 1;

	  if(fbi_i
		  OR 
		  ((grlc_data->rd.li[grlc_data->rd.li_cnt-1])
		    AND (grlc_data->rd.m[grlc_data->rd.li_cnt-1] EQ 0)))
	    grlc_data->rd.data_array[bsn_i & WIN_MOD].pdu_complete = TRUE;
	  else
	    grlc_data->rd.data_array[bsn_i & WIN_MOD].pdu_complete = FALSE;

  }
  
} /* rd_save_block() */



/*
+------------------------------------------------------------------------------
| Function    : rd_comp_rec_par
+------------------------------------------------------------------------------
| Description : The function rd_comp_rec_par() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void rd_comp_rec_par ( UBYTE bsn_i )
{ 
  UBYTE vq_help;
  TRACE_FUNCTION( "rd_comp_rec_par" );

  if(grlc_data->rd.rlc_mode EQ CGRLC_RLC_MODE_ACK)
  {
    if( ( grlc_data->rd.vr >= grlc_data->rd.ssn)  && 
        ( (bsn_i >= grlc_data->rd.vr ) || 
          (bsn_i < grlc_data->rd.ssn) ) )
      grlc_data->rd.vr = (bsn_i+1) & 0x7F;
    else if( ( grlc_data->rd.vr < grlc_data->rd.ssn)  && 
             ( (bsn_i >= grlc_data->rd.vr ) && 
               (bsn_i < grlc_data->rd.ssn) ) )
      grlc_data->rd.vr = (bsn_i+1) & 0x7F;
    
    grlc_data->rd.vn[bsn_i & WIN_MOD] = VN_RECEIVED;
    vq_help = grlc_data->rd.vq;
    while( (grlc_data->rd.vn[vq_help & WIN_MOD]  EQ VN_RECEIVED) AND 
            vq_help NEQ grlc_data->rd.vr)
    {
      vq_help = (vq_help+1) & 0x7F;
    }
    grlc_data->rd.vq = vq_help;
  }
  else if(grlc_data->rd.rlc_mode EQ CGRLC_RLC_MODE_UACK)
  {
    if(grlc_data->rd.vr EQ bsn_i)
    {
      grlc_data->rd.vr         = (1+grlc_data->rd.vr) % 128;
      grlc_data->rd.vq         = (1+grlc_data->rd.vq) % 128;
      grlc_data->rd.inSequence = TRUE;
    }
    else
    {
      grlc_data->rd.vr         = bsn_i;
      grlc_data->rd.inSequence = FALSE;    
    }
  }
  else
    TRACE_ERROR(" unknown RLC Mode during dl tbf in rd_comp_rec_par");


  
} /* rd_comp_rec_par() */




/*
+------------------------------------------------------------------------------
| Function    : rd_check_fbi
+------------------------------------------------------------------------------
| Description : The function rd_check_fbi() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE rd_check_fbi ( UBYTE fbi_i, UBYTE sp , ULONG fn , UBYTE rrbp  )
{ 
  TRACE_FUNCTION( "rd_check_fbi" );
 
  /*
   * mark the bsn with fbi=1
   */
  if(fbi_i)
  {
    grlc_data->rd.last_bsn = grlc_data->rd.vr;
    if(!sp)
    {
      TRACE_EVENT_P1("NO SP BUT FINAL DATA BLOCK t3192=%d",grlc_data->downlink_tbf.t3192_val);
    }
  }


  /*
   * if last bsn is left window element, than tbf is going to be released
   */
  if(grlc_data->rd.last_bsn EQ grlc_data->rd.vq AND
     !grlc_data->rd.f_ack_ind) /* to avoid retransmission */ 
  {
    PALLOC(prim,CGRLC_T3192_STARTED_IND);
    PSEND(hCommGRR,prim);
    if(grlc_data->rd.rlc_mode EQ CGRLC_RLC_MODE_ACK )
    {
      SET_STATE(RD,RD_REL_ACK);
    }
    else
    {
      SET_STATE(RD,RD_REL_UACK);
    }
    
    grlc_data->tbf_ctrl[grlc_data->dl_index].fbi = 1;

    if(sp)
    {
      grlc_data->rd.fn_p_tbf_rel = grlc_decode_tbf_start_rel(fn,(USHORT)(rrbp+3));
    }
    return 1;
  }
  else
  {
    return 0;
  }
}/* rd_check_fbi() */




/*
+------------------------------------------------------------------------------
| Function    : rd_set_acknack
+------------------------------------------------------------------------------
| Description : The function rd_set_acknack() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE* rd_set_acknack ( void )
{ 
  MCAST (u_dl_ack,U_GRLC_DL_ACK);
  UBYTE * ptr_block;
  UBYTE i;
  UBYTE index;
  UBYTE help,neg_ack=0;
  ULONG rbb1=0,rbb2=0,dummy1=0,dummy2=0;

  TRACE_FUNCTION( "rd_set_acknack" );

  
  if(grlc_data->rd.channel_req AND (grlc_data->tbf_type EQ TBF_TYPE_DL))
  {
    u_dl_ack->v_chan_req_des = 1;
    u_dl_ack->chan_req_des   = grlc_data->chan_req_des;    
    grlc_data->rd.channel_req = 0;
    grlc_data->rd.ch_req_in_ack_prog = TRUE;
	 /* TRACE_EVENT_P6("channel req des in dl ack nack:ptp=%d,rp=%d,rlc_mode=%d,llc_pt=%d,rlc_oc=%d, pst=%d",
                                  grlc_data->chan_req_des.peak_thr_class,
                                  grlc_data->chan_req_des.radio_prio,
                                  grlc_data->chan_req_des.rlc_mode,
                                  grlc_data->chan_req_des.llc_pdu_type,
                                  grlc_data->chan_req_des.rlc_octet_cnt,
                                  grlc_data->prim_start_tbf);*/
    if(grlc_data->prim_start_tbf >= PRIM_QUEUE_SIZE_TOTAL)
    {
      TRACE_EVENT_P3("PST=%d PSF=%d PDU=%d: rd_set_acknack"
                                                           ,grlc_data->prim_start_tbf
                                                           ,grlc_data->prim_start_free
                                                           ,grlc_data->grlc_data_req_cnt);
    }

  }
  else
  {
    u_dl_ack->v_chan_req_des = 0;
    grlc_data->rd.channel_req = 0; 
  }
  
  u_dl_ack->msg_type               = U_GRLC_DL_ACK_c;
  u_dl_ack->dl_tfi                 = grlc_data->dl_tfi;
  u_dl_ack->ack_nack_des.f_ack_ind = grlc_data->rd.f_ack_ind;
  u_dl_ack->ack_nack_des.ssn       = grlc_data->rd.vr;
  memset(u_dl_ack->ack_nack_des.rbb, 1, WIN_SIZE );
  for(i=0; i< WIN_SIZE; i++)
  {
    if(grlc_data->rd.vr EQ grlc_data->rd.ssn OR grlc_data->rd.rlc_mode EQ CGRLC_RLC_MODE_UACK)
      break;
    index = (grlc_data->rd.vr-1-i) & 0x7F;
    if(grlc_data->rd.vn[index & WIN_MOD] NEQ VN_RECEIVED)
    {
      u_dl_ack->ack_nack_des.rbb[WIN_SIZE-1-i] = 0; 
      neg_ack++;
    }
    if(grlc_data->rd.ssn EQ index )      
    {
      help  =  grlc_data->rd.ssn;
      while(help NEQ grlc_data->rd.vq)
      {
        grlc_data->rd.vn[help & WIN_MOD] = VN_INVALID;
        help = (help + 1) & 0x7F; 
      }
      break;
    }
  }

  /* TRACE ONLY if there is channel desc or negative ack or the final ack*/
  if(u_dl_ack->v_chan_req_des OR
     u_dl_ack->ack_nack_des.f_ack_ind OR
     neg_ack)
  {
    if(neg_ack AND !u_dl_ack->ack_nack_des.f_ack_ind)
    {
      for(i=0; i<32;i++)
      {
        dummy1 = u_dl_ack->ack_nack_des.rbb[WIN_SIZE-1-i];
        dummy2 = u_dl_ack->ack_nack_des.rbb[WIN_SIZE-1-i-32];
        rbb1  += dummy1 <<i;
        rbb2  += dummy2 <<i;
      }
      TRACE_EVENT_P7("dl_ack:p_fn=%ld,CD=%d  ssn=%d nacks=%d vq=%d rbb2=%lx rbb1=%lx",
                                          grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn,
                                          u_dl_ack->v_chan_req_des,
                                          u_dl_ack->ack_nack_des.ssn,
                                          neg_ack,                                         
                                          grlc_data->rd.vq,
                                          rbb2,
                                          rbb1); 
    }
    else
    {
      rbb1 = 0xFFFFFFFF;
      rbb2 = 0xFFFFFFFF;
      TRACE_EVENT_P6("dl_ack:p_fn=%ld,CD=%d fbi=%d ssn=%d nacks=%d vq=%d",
                                          grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn,
                                          u_dl_ack->v_chan_req_des,
                                          u_dl_ack->ack_nack_des.f_ack_ind,
                                          u_dl_ack->ack_nack_des.ssn,
                                          neg_ack,                                         
                                          grlc_data->rd.vq);
    }
  }
  else
  {
    rbb1 = 0xFFFFFFFF;
    rbb2 = 0xFFFFFFFF;  
  }

  u_dl_ack->chan_qual_rep.c_value = 0;
  u_dl_ack->chan_qual_rep.rxqual  = 0;
  u_dl_ack->chan_qual_rep.signvar = 0;

  {
    /* processing channel quality report */

    /* processing of C value */
    u_dl_ack->chan_qual_rep.c_value = meas_grlc_c_get_value( );

    /* processing of RXQUAL value */
    u_dl_ack->chan_qual_rep.rxqual  = meas_sq_get_rxqual_value( );

    /* processing of signal variance */
    u_dl_ack->chan_qual_rep.signvar = meas_sv_get_value( );

    /* processing of relative interference levels */
    meas_int_get_rel_i_level( &u_dl_ack->chan_qual_rep.ilev );
  } 

#if !defined (NTRACE)

  if( grlc_data->meas.v_im_trace NEQ 0 )
  {
    TRACE_EVENT_P3( "rd_set_acknack: %d %d %d",
                    u_dl_ack->chan_qual_rep.c_value,
                    u_dl_ack->chan_qual_rep.rxqual,
                    u_dl_ack->chan_qual_rep.signvar );
  }

#endif /* #if !defined (NTRACE) */

#ifdef REL99

  u_dl_ack->v_release_99_str_u_grlc_dl_ack = 1;
  
  if (grlc_data->pfi_support AND u_dl_ack->v_chan_req_des )
  {
    u_dl_ack->release_99_str_u_grlc_dl_ack.v_pfi = 1;
    u_dl_ack->release_99_str_u_grlc_dl_ack.pfi = grlc_data->pfi_value;  
  }
  else
  {
    u_dl_ack->release_99_str_u_grlc_dl_ack.v_pfi = 0;
  }

#endif


  grlc_data->rd.ssn   = grlc_data->rd.vq;  
  ptr_block = (_decodedMsg);


  return ptr_block;
} /* rd_set_acknack() */


/*
+------------------------------------------------------------------------------
| Function    : rd_calc_rlc_data_len
+------------------------------------------------------------------------------
| Description : The function rd_calc_rlc_data_len() calculates the Data size of
|               an RLC data block depending on coding scheme. 
|               The size is described in bytes.
|               
| Parameters  : block_status_i - includes the Coding scheme which 
|               determines the size of an RLC data block
|                 
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE rd_calc_rlc_data_len ( USHORT  block_status_i )
{ 
  UBYTE result=0;
  TRACE_FUNCTION( "rd_calc_rlc_data_len" );
  
  /*
   * the coding scheme is only in the first four bits
   */
  block_status_i = block_status_i & 0x000F; 

  switch( block_status_i)
  {
  case 2:
    grlc_data->rd.cs_type  = CS_1;
    break;
  case 4:
    grlc_data->rd.cs_type  = CS_2;
    break;
  case 5:
    grlc_data->rd.cs_type  = CS_3;
    break;
  case 6:
    grlc_data->rd.cs_type  = CS_4;
    break;
  default:
    TRACE_EVENT("No Coding Scheme in RLC data block defined, old CS is used");
    break;
  }
  
  
  switch( grlc_data->rd.cs_type)
  {
  /*
   * NO CS defined in all previously received RLC data blocks, 
   * therefore default CS_1 is used
   */
  case CS_ZERO: 
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
    TRACE_ERROR("unknown Coding Scheme");
    break;
  }

  return result;
} /* rd_calc_rlc_data_len() */



/*
+------------------------------------------------------------------------------
| Function    : rd_fill_blocks
+------------------------------------------------------------------------------
| Description : The function rd_fill_blocks() fills not received but needed 
|               RLC data blocks with the value Zero.
|
| Parameters  : bsn_i - bsn value of the recently received RLC data block, 
|               which is not inSequence(not equal VR at receiving the block)
|
+------------------------------------------------------------------------------
*/
GLOBAL void rd_fill_blocks ( UBYTE bsn_i )
{ 
  T_NEXT_ARRAY * ptr_temp=NULL;
  TRACE_FUNCTION( "rd_fill_blocks" );

  do
  {
    MALLOC(ptr_temp, sizeof(T_NEXT_ARRAY));
    ptr_temp->next = NULL;
    ptr_temp->len  = grlc_data->rd.rlc_data_len;
    memset(ptr_temp->data,0,ptr_temp->len);
    grlc_data->rd.data_array[grlc_data->rd.vq & WIN_MOD].first = ptr_temp;
    grlc_data->rd.data_array[grlc_data->rd.vq & WIN_MOD].pdu_cnt = 0;
    grlc_data->rd.data_array[grlc_data->rd.vq & WIN_MOD].pdu_complete =FALSE;
    rd_send_grlc_data_ind(grlc_data->rd.vq);
    grlc_data->rd.vq = (1+grlc_data->rd.vq) % 128; /*modulo 128*/
  }
  while(grlc_data->rd.vq NEQ bsn_i);

  grlc_data->rd.vq         = (1+grlc_data->rd.vq) % 128; /*modulo 128*/
  grlc_data->rd.vr         = grlc_data->rd.vq;
  grlc_data->rd.inSequence = TRUE;



} /* rd_fill_blocks() */




/*
+------------------------------------------------------------------------------
| Function    : rd_calc_delta_fn
+------------------------------------------------------------------------------
| Description : The function rd_calc_delta_fn() calculates delta_fn. It is 
|               needed at receiving mac_ready_ind to send a poll block.
|               In Target Enviroment, the poll block(Ack/nack or Control block)
|               must be calculated one radio block earlier then the send time.
|               This is needed due to the functional interface.
| Parameters  : fn_i - framenumber for the uplink call 
|               
|
+------------------------------------------------------------------------------
*/
GLOBAL ULONG rd_calc_delta_fn ( ULONG fn_i )
{ 
  ULONG result;
  TRACE_FUNCTION( "rd_calc_delta_fn" );

  #ifdef _TARGET_
  {
    if(fn_i EQ FN_MAX-5)
      result = grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn + 5;
    else
      result = grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn - fn_i;
  }
#endif

#ifdef _SIMULATION_
  {
    if(fn_i EQ grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn)
      result = 4; /*valid: send block*/
    else
      result = 0; /*no poll sending*/
  }
#endif

  return result;


} /* rd_calc_delta_fn() */

/*
+------------------------------------------------------------------------------
| Function    : rd_free_desc_list_partions
+------------------------------------------------------------------------------
| Description : The function rd_free_desc_list_partions() frees the partions,
|               which are linked for a candidate LLC pdu. PDU is stored as a 
|               description list, but not passed to LLC because of incomplete.
| Parameters  :  
|               
+------------------------------------------------------------------------------
*/
GLOBAL void rd_free_desc_list_partions ( void )
{
  UBYTE          cnt  = 0;
  T_NEXT_ARRAY  *help = NULL;

  TRACE_FUNCTION( "rd_free_desc_list_partions" );

  help = (T_NEXT_ARRAY*)grlc_data->rd.grlc_data_ind.desc_list.first;

  grlc_data->rd.grlc_data_ind.desc_list.first = NULL;

  while(help != NULL)
  {
    T_NEXT_ARRAY  *ptr = (T_NEXT_ARRAY*) help->next;
    MFREE(help);
    help = ptr;
    cnt++;
  }


  #ifdef _SIMULATION_
  TRACE_EVENT_P1("freed partion (linked)  : cnt=%d",cnt);
#endif /* _SIMULATION_*/


  grlc_data->rd.pdu_complete  = TRUE;
  grlc_data->rd.pdu_len       = 0;


} /* rd_free_desc_list_partions() */

/*
+------------------------------------------------------------------------------
| Function    : rd_free_database_partions
+------------------------------------------------------------------------------
| Description : The function rd_free_database_partions() frees the partions,
|               which are stored in the RD database. They are not linked to a
|               LLC pdu because data blocks are not received in sequence. 
|               Partions are removed from left window size (vq) up to the 
|               right window size (vr).
| Parameters  :  
|               
+------------------------------------------------------------------------------
*/
GLOBAL void rd_free_database_partions ( void )
{
  USHORT bsn;

  UBYTE  cnt=0;

  T_NEXT_ARRAY  *help = NULL;

  TRACE_FUNCTION( "rd_free_database_partions" );

  bsn = grlc_data->rd.vq;  

  while (bsn NEQ grlc_data->rd.vr) 
  {
    help = grlc_data->rd.data_array[bsn & WIN_MOD].first; 
#ifdef _SIMULATION_
    TRACE_EVENT_P2("candiate bsn=%d  vn=%d",bsn,grlc_data->rd.vn[bsn & WIN_MOD]);
#endif /* _SIMULATION_*/

    while (help != NULL) 
    {
      grlc_data->rd.data_array[bsn & WIN_MOD].first = 
        (T_NEXT_ARRAY *)grlc_data->rd.data_array[bsn & WIN_MOD].first->next;

      MFREE(help);

      cnt++;
#ifdef _SIMULATION_
      TRACE_EVENT_P1("element free : bsn=%d",bsn);
#endif /* _SIMULATION_*/
      if(grlc_data->rd.data_array[bsn & WIN_MOD].pdu_cnt)
		    help = grlc_data->rd.data_array[bsn & WIN_MOD].first;
	    else
		    help = NULL;
	  }
    grlc_data->rd.data_array[bsn & WIN_MOD].pdu_complete = FALSE;
    grlc_data->rd.data_array[bsn & WIN_MOD].pdu_cnt      = 0xFF;
    grlc_data->rd.data_array[bsn & WIN_MOD].first        = NULL;   
    
    bsn = (bsn + 1) & 0x7F;
  }
  TRACE_EVENT_P2("rd_free_database_partions after:  bsn=%d  cnt=%d ", bsn,  cnt);


} /* rd_free_database_partions() */


/*
+------------------------------------------------------------------------------
| Function    : rd_cgrlc_st_time_ind 
+------------------------------------------------------------------------------
| Description : The function rd_cgrlc_st_time_ind () informs higher layers
|               that the starting time is elapsed
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
GLOBAL void rd_cgrlc_st_time_ind  ( void )
{ 
  PALLOC(cgrlc_starting_time_ind,CGRLC_STARTING_TIME_IND); /* T_CGRLC_STARTING_TIME_IND */

  TRACE_FUNCTION( "rd_cgrlc_st_time_ind " );

  cgrlc_starting_time_ind->tbf_mode = CGRLC_TBF_MODE_DL;
  cgrlc_starting_time_ind->tfi      = grlc_data->dl_tfi;
 
  PSEND(hCommGRR,cgrlc_starting_time_ind);

  grlc_data->dl_tn_mask = grlc_data->downlink_tbf.ts_mask;
  

} /* rd_cgrlc_st_time_ind () */
