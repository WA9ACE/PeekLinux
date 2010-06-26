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
|  Purpose :  This module implements local functions for service GFF of
|             entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef  GRLC_GFFF_C
#define  GRLC_GFFF_C
#endif

#define ENTITY_GRLC

/*==== INCLUDES =============================================================*/
#include <string.h>
#include "typedefs.h"   /* to get Condat data types */
#include <stdio.h>
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"
#include "grlc.h"        
#include "grlc_f.h" 
#include "grlc_tms.h"   
#include "grlc_rus.h"   
#include "grlc_rds.h"   
#include "grlc_gfff.h"        
#include "cl_rlcmac.h"

/*==== CONST ================================================================*/

#define D_DL_DUMMY_c 0x25

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : gff_init
+------------------------------------------------------------------------------
| Description : The function gff_init() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void gff_init ( void )
{ 
  TRACE_FUNCTION( "gff_init" );

  grlc_data->gff.rlc_status = RLC_STATUS_NULL;
  grlc_data->ul_tfi         = 0xFF;
  grlc_data->dl_tfi         = 0xFF;

  grlc_data->next_poll_fn      = 1;
  grlc_data->ul_poll_pos_index = 0xFF;
  grlc_data->nr_of_crc_errors  = 0xFFFF;
  grlc_data->ta_value          = 0xFF;

  INIT_STATE(GFF,GFF_DEACTIVE); 
} /* gff_init() */



/*
+------------------------------------------------------------------------------
| Function    : gff_tbf_init
+------------------------------------------------------------------------------
| Description : The function gff_tbf_init() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void gff_tbf_init ( void )
{ 
  TRACE_FUNCTION( "gff_tbf_init" );
 
  /*
   * set segmented control block array
   */
  grlc_data->ul_poll_pos_index = 0;
  grlc_data->nr_of_crc_errors  = 0;


#if defined (_TARGET_)
  {
    UBYTE i;
    /* target debugging*/

    grlc_data->last_dl_fn = 1; 
    
    for (i=0; i < CALL_ERROR_NR_OF_ST_FN;i++)
    {
      grlc_data->ul_fn_store[i]  = 0;
      grlc_data->ul_fn_errors[i] = 0;
      grlc_data->dl_fn_store[i]  = 0;
      grlc_data->dl_fn_errors[i] = 0;
    }
    grlc_data->ul_cnt_syn        = 0;  /*for target, count calls in rlc uplink*/
    grlc_data->ul_cnt_asyn       = 0;  /*for target, count calls in gffp mac_ready_ind*/
    grlc_data->dl_cnt_syn        = 0;  /*for target, count calls in rlc downlink*/
    grlc_data->dl_cnt_asyn       = 0;  /*for target, count calls in gffp mac_data_ind*/
    grlc_data->ul_call_errors    = 0;  /*for target, count calls in rlc uplink during grlc is active*/
    grlc_data->dl_call_errors    = 0;  /*for target, count calls in rlc downlink during grlc is active*/
  }
#endif /* defined (_TARGET_) */

} /* gff_tbf_init() */



/*
+------------------------------------------------------------------------------
| Function    : gff_send_ctrl_block
+------------------------------------------------------------------------------
| Description : The function gff_send_ctrl_block() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void gff_send_ctrl_block (ULONG fn_i, UBYTE tn_i, UBYTE * ptr_ctrl_block_i )
{
  PALLOC(cgrlc_data_ind,CGRLC_DATA_IND); /*T_CGRLC_DATA_IND*/

  TRACE_FUNCTION( "gff_send_ctrl_block" );

  cgrlc_data_ind->fn = fn_i;
  cgrlc_data_ind->tn = tn_i;  

  memcpy( cgrlc_data_ind->data_array, 
          ptr_ctrl_block_i,
          CGRLC_MAX_CTRL_MSG_SIZE );

  PSEND(hCommGRR,cgrlc_data_ind);

}/* gff_send_ctrl_block() */




/*
+------------------------------------------------------------------------------
| Function    : gff_analyse_dl_data
+------------------------------------------------------------------------------
| Description : The function gff_analyse_dl_data() .... It returns whether
|               the downlink block was intended for that MS or not.
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL gff_analyse_dl_data (ULONG fn_i, T_dl_data * ptr_dl_data_i )
{ 

  BOOL        tfi_correct = FALSE;
  UBYTE       payload,rrbp,sp,tfi,*ptr_blk;

  TRACE_FUNCTION( "gff_analyse_dl_data" );

/*
 *  Layer 1 buffer
 *  B1 B0  --> ptr_dl_data_i->dl_block[0]
 *  B3 B2
 *
 * B0 is the mac header
 */


  ptr_blk = (UBYTE *) (&ptr_dl_data_i->dl_block[0]);


  payload = (ptr_blk[0] & 0xC0) >> 6;
  rrbp    = (ptr_blk[0] & 0x30) >> 4;
  sp      = (ptr_blk[0] & 0x08) >> 3;

  
  /* 
   * In Test mode B,if there is CRC error on the payload data the MS will,
   * where required by the USF, transmit the decoded payload data. The block
   * transmitted will be a valid uplink block format.
   */
  if(((ptr_dl_data_i->block_status & 0x0100) NEQ 0x0000) AND ((grlc_data->testmode.mode EQ CGRLC_LOOP)))
  {
    UBYTE       bsn,e_bit;
     
    bsn     = (ptr_blk[2] & 0xFE)>>1;
    e_bit   = (ptr_blk[2] & 0x01);
    
    TRACE_EVENT(" Bad CRC - In Testmode B ");

    sig_gff_rd_data(  fn_i, 
                        (UBYTE)ptr_dl_data_i->tn, 
                        ptr_dl_data_i->block_status,
                        rrbp,
                        sp,
                        bsn,
                        0,
                        e_bit,
                        ptr_blk);
  }
  else
  {
    if(payload EQ RLC_DATA_BLOCK) 
    {
      UBYTE       bsn,fbi,e_bit;
     
      tfi     = (ptr_blk[1] & 0x3E) >> 1;
      bsn     = (ptr_blk[2] & 0xFE)>>1;
      fbi     = (ptr_blk[1] & 0x01);
      e_bit   = (ptr_blk[2] & 0x01);
      ptr_blk = (UBYTE *) (&ptr_blk[3]);

      if((tfi EQ grlc_data->dl_tfi) AND
         ((0x80>>ptr_dl_data_i->tn) & grlc_data->dl_tn_mask))
      {
        tfi_correct = TRUE;
        if(sp)
        {
          grlc_save_poll_pos ( fn_i,(UBYTE)ptr_dl_data_i->tn,rrbp, CGRLC_POLL_DATA, 3);
        }
        sig_gff_rd_data(  fn_i, 
                          (UBYTE)ptr_dl_data_i->tn, 
                          ptr_dl_data_i->block_status,
                          rrbp,
                          sp,
                          bsn,
                          fbi,
                          e_bit,
                          ptr_blk);
  #ifdef _TARGET_
      
        TRACE_BINDUMP( hCommGRLC, TC_USER6,
                 		  "D_DATA_BLOCK including MAC header", &ptr_dl_data_i->dl_block[0],53);
  #endif /* _TARGET_ */

      }
      else
      {
        TRACE_EVENT_P6("wrong tfi in data block  ad.tfi=%d dl_tfi=%d  bsn=%d || tn=%d->mask=%x tn_mask=%x"
                                                                ,tfi
                                                                ,grlc_data->dl_tfi
                                                                ,bsn
                                                                ,ptr_dl_data_i->tn
                                                                ,(0x80>>ptr_dl_data_i->tn)
                                                                ,grlc_data->dl_tn_mask);
      }


    }
    else
    {
      /*
       * any rlc control message received.
       * check if it is a packet uplink ack/nack and if it is adressed to MS
       */
      ULONG tc_user        = TC_USER4;
      UBYTE trace_msg_type = D_MSG_TYPE_UNKNOWN_c;

      UBYTE msg_type=0,i=0;

      if( payload EQ CTRL_BLK_NO_OPT )
      {
        msg_type = trace_msg_type = ptr_blk[1]>>2; 
        i        = 2; /* Byte position of tfi in puan */
      }
      else if( payload EQ CTRL_BLK_OPT )
      {
        if( !(ptr_blk[1] & 0x01) )           /* no tfi octet present: ac   = 0 */
        {
          if( !(ptr_blk[1] & 0x80) )         /* not segmented       : rbsn = 0 */
          {
            if( (ptr_blk[1] & 0x02) )        /* not segmented       : fs   = 1 */ 
            {
              msg_type = trace_msg_type = ptr_blk[2]>>2; 
              i        = 3; /* Byte position of tfi in puan */
            }
            else                             /* segmented           : fs   = 0 */
            {
              trace_msg_type = ptr_blk[2]>>2; 
            }
          }
          else                               /* segmented           : rbsn = 1 */
          {
            trace_msg_type = D_MSG_TYPE_2ND_SEGMENT_c;
          }
        }
        else                                 /* tfi octet present   : ac   = 1 */
        {
          if( !(ptr_blk[1] & 0x80) )         /* not segmented       : rbsn = 0 */ 
          {
            if( (ptr_blk[1] & 0x02) )        /* not segmented       : fs   = 1 */
            {
              msg_type = trace_msg_type = ptr_blk[3]>>2;
              i        = 4; /* Byte position of tfi in puan */
            }
            else                             /* segmented           : fs   = 0 */
            {
              trace_msg_type = ptr_blk[3]>>2;
            }
          }
          else                               /* segmented           : rbsn = 1 */ 
          {
            trace_msg_type = D_MSG_TYPE_2ND_SEGMENT_c;
          }
        }
      }

      /*
       * send control message to GRR
       */
      if(msg_type NEQ D_DL_DUMMY_c ) /* No Dummy blocks sent to GRR , to primitive load between GRR and GRLC*/
      {  
        gff_send_ctrl_block(fn_i, (UBYTE)ptr_dl_data_i->tn,ptr_blk);
      }
  
      if( trace_msg_type EQ D_DL_DUMMY_c )
      {
        tc_user = TC_USER5;
      }

      TRACE_BINDUMP( hCommGRLC, tc_user,
                     cl_rlcmac_get_msg_name( trace_msg_type, RLC_MAC_ROUTE_DL ),
                     ptr_blk, MAX_L2_FRAME_SIZE ); /*lint !e569*/

      if(msg_type EQ D_GRLC_UL_ACK_c)
      {
        /*
         * tfi check for packet uplink ack nack
         */
        if(i EQ 4                AND                          /* tfi in header present       */
          !(ptr_blk[2] & 0x01)   AND                          /* D=0 packet uplink tfi add.  */
          (((ptr_blk[2]) & 0x3E)>>1) EQ grlc_data->ul_tfi)    /* header tfi addressed to MS  */
        {
          tfi = ((ptr_blk[2]) & 0x3E)>>1;
        }
        else
        {
          tfi = ((ptr_blk[i]) & 0x3E)>>1;  /* Take tfi from airmessage*/
        }

        if((grlc_data->ul_tfi EQ tfi) AND
           ((0x80>>ptr_dl_data_i->tn) & grlc_data->ul_tn_mask))
        {
          T_MSGBUF msg;
          UBYTE    data[22];

          memset(data,0xEE,22);

          msg.l_buf  = (23 - i + 1) * 8;
          msg.o_buf  = 0;
          msg.buf[0] = data[0];

          memcpy(msg.buf, &(ptr_blk[i-1]), (msg.l_buf)/8);


          if(sp)
          {
            grlc_save_poll_pos ( fn_i,(UBYTE)ptr_dl_data_i->tn,rrbp, CGRLC_POLL_UACK, 3);
          }
          grlc_decode_grlc (&msg);
          sig_gff_ru_ul_ack ( fn_i,(UBYTE)ptr_dl_data_i->tn,rrbp,sp); 
        } 
      }
    }
  }


  return( tfi_correct );
} /* gff_analyse_dl_data() */


/*
+------------------------------------------------------------------------------
| Function    : gff_handle_continious_ta
+------------------------------------------------------------------------------
| Description : store the new ta value, and pass it to L1
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void gff_handle_continious_ta ( void )
{

  TRACE_FUNCTION( "gff_handle_continious_ta" );

 
  if(grlc_data->func.mac_ready_ind.ta_value NEQ 0xFF)
  {
    /*
     * valid TA in L1
     */
    if(grlc_data->ta_value NEQ grlc_data->func.mac_ready_ind.ta_value )
    {
      PALLOC(cgrlc_ta_value_ind,CGRLC_TA_VALUE_IND);

      /*
       * Current TA in GRLC differs from the TA in L1: store in GRLC
       */
      grlc_data->ta_value           = grlc_data->func.mac_ready_ind.ta_value;
      cgrlc_ta_value_ind->ta_value  = grlc_data->ta_value;
      PSEND(hCommGRR,cgrlc_ta_value_ind);
    }
  }

} /* gff_handle_continious_ta */

/*
+------------------------------------------------------------------------------
| Function    : gff_clip_rxlev
+------------------------------------------------------------------------------
| Description : This function is used to clip received signal level values.
|
| Parameters  : clipp  - pointer to clipped received signal level values
|               rxlev  - pointer to received signal level values
|               number - number of received signal level values
|
+------------------------------------------------------------------------------
*/
GLOBAL void gff_clip_rxlev ( UBYTE *clipp, UBYTE *rxlev, UBYTE number )
{ 
  UBYTE i; /* used for counting */
  
  TRACE_FUNCTION( "gff_clip_rxlev" );

  for( i = 0; i < number; i++ )
  { 
    if( (signed char)( rxlev[i] ) <   MAC_RXLEV_MIN  AND
                       rxlev[i]   NEQ MAC_RXLEV_NONE     )
    {
      clipp[i] = MAC_RXLEV_MIN;
    }
    else if ( (signed char)( rxlev[i] ) > MAC_RXLEV_MAX )
    {
      clipp[i] = MAC_RXLEV_MAX;
    }
    else if( rxlev[i] EQ MAC_RXLEV_NONE )
    {
      clipp[i] = MAC_RXLEV_NONE;
    }
    else
    {
      clipp[i] = rxlev[i];
    }
  }
} /* gff_clip_rxlev() */
