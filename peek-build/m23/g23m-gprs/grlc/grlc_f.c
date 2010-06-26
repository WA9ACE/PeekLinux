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
| Purpose:     This module implements global functions for GRLC
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_F_C
#define GRLC_F_C
#endif

#define ENTITY_GRLC


/*==== INCLUDES =============================================================*/

#include <stdio.h>
#include <string.h>     /* to get definition of memcpy() */ 
#include <math.h>

#include "typedefs.h"   /* to get Condat data types                          */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros                            */
#include "ccdapi.h"     /* to get CCD API                                    */
#include "cnf_grlc.h"    /* to get cnf-definitions                            */
#include "mon_grlc.h"    /* to get mon-definitions                            */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"    /* to get message describtion                        */
#include "pcm.h"

#include "grlc.h"        /* to get the global entity definitions              */

#include "grlc_f.h"      /* to check own definitions         */
#include "grlc_tmf.h"    /* to get definition of tm_grlc_init()   */
#include "grlc_gfff.h"   /* to get definition of gff_init()  */
#include "grlc_rdf.h"    /* to get definition of rd_init()   */
#include "grlc_ruf.h"    /* to get definition of ru_init()   */
#include "grlc_rus.h"    
#include "grlc_rds.h"    
#include "grlc_tms.h" 
#include "grlc_tpcs.h"
#include "grlc_meass.h"
#include "cl_rlcmac.h"

/*==== CONST ================================================================*/

/*==== DIAGNOSTICS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== GLOBAL VARS ===========================================================*/
/*==== LOCAL MACROS =========================================================*/


/*==== FUNCTIONS PROTOTYPES =================================================*/

LOCAL void grlc_encode_dl_acknack   ( UBYTE * ptr_out );

LOCAL void grlc_encode_ul_dummy     ( UBYTE * ptr_out );

LOCAL void grlc_encode_pca          ( UBYTE * ptr_out );
  
LOCAL UBYTE grlc_decode_ul_acknack  ( UBYTE * ptr_blk );

LOCAL UBYTE grlc_ccd_error_handling ( UBYTE entity_i  );

LOCAL USHORT grlc_convert_11bit_2_etsi ( USHORT eleven_bit );

/*==== FUNCTIONS ============================================================*/
/*
+------------------------------------------------------------------------------
| Function    : grlc_encode_dl_acknack
+------------------------------------------------------------------------------
| Description : The function grlc_encode_dl_acknack() encodes the packet downlink
|               ack/nack block without CCD
|
| Parameters  : *ptr_out_i- ptr to the buffer where the air message will be placed
|
|
+------------------------------------------------------------------------------
*/
LOCAL void grlc_encode_dl_acknack ( UBYTE * ptr_out)
{
  MCAST (u_dl_ack,U_GRLC_DL_ACK); /* T_U_GRLC_DL_ACK */

  UBYTE i=0, bit=0,byte,bit_in_byte,j;
  
  TRACE_FUNCTION( "grlc_encode_dl_acknack" );

  /******************* mandatory elements   ****************************/

  /* MESSAGE TYPE 6 bit */
  ptr_out[0]  = u_dl_ack->msg_type << 2;
  bit +=6;
  /* DL TFI 5 bit */
  ptr_out[0] |= (u_dl_ack->dl_tfi >> 3) & 0x03;
  ptr_out[1]  = (u_dl_ack->dl_tfi << 5);
  bit +=5;
  /* ACK NACK DESCRIPTION */
  /* final ack indication 1 bit */
  ptr_out[1] |= (u_dl_ack->ack_nack_des.f_ack_ind << 4);
  bit +=1;
  /* ssn 7 bit */
  ptr_out[1] |= (u_dl_ack->ack_nack_des.ssn >> 3);
  ptr_out[2]  = (u_dl_ack->ack_nack_des.ssn << 5);
  bit +=7;
  for (i=0;i<64; i++)
  {
    byte           = bit / 8; /* byte pos         */
    bit_in_byte    = bit % 8; /* rel bit pos in the current byte */    
    ptr_out[byte] |= u_dl_ack->ack_nack_des.rbb[i] << (7-bit_in_byte);
    bit +=1;
  }
  /* CHANNEL REQUEST DESCRIPTION */
  /* valid flag*/
  byte           = bit / 8; 
  bit_in_byte    = bit % 8;
  ptr_out[byte] |= u_dl_ack->v_chan_req_des << (7-bit_in_byte);
  bit +=1;
  if(u_dl_ack->v_chan_req_des)
  {
    /* peak_thr_class */
    for(i=0;i<4;i++)
    {
      byte           = bit / 8; 
      bit_in_byte    = bit % 8;
      ptr_out[byte] |= (u_dl_ack->chan_req_des.peak_thr_class >> (3-i)) << (7-bit_in_byte);
      bit +=1;
    }     
    /* radio prio */
    for(i=0;i<2;i++)
    {
      byte           = bit / 8; 
      bit_in_byte    = bit % 8;
      ptr_out[byte] |= (u_dl_ack->chan_req_des.radio_prio >> (1-i)) << (7-bit_in_byte);
      bit +=1;
    }
    /* rlc_mode*/
    byte           = bit / 8; 
    bit_in_byte    = bit % 8;
    ptr_out[byte] |= (u_dl_ack->chan_req_des.rlc_mode) << (7-bit_in_byte);
    bit +=1;
    /* llc pdu type*/
    byte           = bit / 8; 
    bit_in_byte    = bit % 8;
    ptr_out[byte] |= (u_dl_ack->chan_req_des.llc_pdu_type) << (7-bit_in_byte);
    bit +=1;
    /* rlc_oct_cnt */
    for(i=0;i<16;i++)
    {
      byte           = bit / 8; 
      bit_in_byte    = bit % 8;
      ptr_out[byte] |= (u_dl_ack->chan_req_des.rlc_octet_cnt >> (15-i)) << (7-bit_in_byte);
      bit +=1;
    }
  }
  /* CHANNEL QUALITY REPORT */
  /* c_value */
  for(i=0;i<6;i++)
  {
    byte           = bit / 8; 
    bit_in_byte    = bit % 8;
    ptr_out[byte] |= (u_dl_ack->chan_qual_rep.c_value >> (5-i)) << (7-bit_in_byte);
    bit +=1;
  }
  /* rxqual */
  for(i=0;i<3;i++)
  {
    byte           = bit / 8; 
    bit_in_byte    = bit % 8;
    ptr_out[byte] |= (u_dl_ack->chan_qual_rep.rxqual >> (2-i)) << (7-bit_in_byte);
    bit +=1;
  }
  /* signvar */
  for(i=0;i<6;i++)
  {
    byte           = bit / 8; 
    bit_in_byte    = bit % 8;
    ptr_out[byte] |= (u_dl_ack->chan_qual_rep.signvar >> (5-i)) << (7-bit_in_byte);
    bit +=1;
  }
  /* c_value */
  for(j=0;j<8;j++)
  {
    UBYTE flag,value;

    switch(j)
    {
      case 0:
        flag  = u_dl_ack->chan_qual_rep.ilev.v_ilev0;
        value = u_dl_ack->chan_qual_rep.ilev.ilev0;
        break;
      case 1:
        flag  = u_dl_ack->chan_qual_rep.ilev.v_ilev1;
        value = u_dl_ack->chan_qual_rep.ilev.ilev1;
        break;
      case 2:
        flag  = u_dl_ack->chan_qual_rep.ilev.v_ilev2;
        value = u_dl_ack->chan_qual_rep.ilev.ilev2;
        break;
      case 3:
        flag  = u_dl_ack->chan_qual_rep.ilev.v_ilev3;
        value = u_dl_ack->chan_qual_rep.ilev.ilev3;
        break;
      case 4:
        flag  = u_dl_ack->chan_qual_rep.ilev.v_ilev4;
        value = u_dl_ack->chan_qual_rep.ilev.ilev4;
        break;
      case 5:
        flag  = u_dl_ack->chan_qual_rep.ilev.v_ilev5;
        value = u_dl_ack->chan_qual_rep.ilev.ilev5;
        break;
      case 6:
        flag  = u_dl_ack->chan_qual_rep.ilev.v_ilev6;
        value = u_dl_ack->chan_qual_rep.ilev.ilev6;
        break;
      case 7:
        flag  = u_dl_ack->chan_qual_rep.ilev.v_ilev7;
        value = u_dl_ack->chan_qual_rep.ilev.ilev7;
        break;
      default:
        TRACE_EVENT_P1("no valid j=%d value during grlc_encode_dl_acknack should not appear ",j);
        flag  = 0;
        value = 0;
        break;
    }
    byte           = bit / 8; 
    bit_in_byte    = bit % 8;
    ptr_out[byte] |= (flag) << (7-bit_in_byte);
    bit +=1;
    if(flag) 
    {
      for(i=0;i<4;i++)
      {
        byte           = bit / 8; 
        bit_in_byte    = bit % 8;
        ptr_out[byte] |= (value >> (3-i)) << (7-bit_in_byte);
        bit +=1;
      }
    }
  }

  /* RELEASE 99*/
#ifdef REL99
  byte           = bit / 8; 
  bit_in_byte    = bit % 8;
  ptr_out[byte] |= u_dl_ack->v_release_99_str_u_grlc_dl_ack << (7-bit_in_byte);
  bit +=1;

  if(u_dl_ack->v_release_99_str_u_grlc_dl_ack)
  {
    byte           = bit / 8; 
    bit_in_byte    = bit % 8;

    if(u_dl_ack->release_99_str_u_grlc_dl_ack.v_pfi)
    {
      ptr_out[byte] |= u_dl_ack->release_99_str_u_grlc_dl_ack.v_pfi << (7-bit_in_byte);
      bit +=1;
      for(i=0;i<6;i++)
      {
        byte           = bit / 8; 
        bit_in_byte    = bit % 8;
        ptr_out[byte] |= (u_dl_ack->release_99_str_u_grlc_dl_ack.pfi >> (6-i)) << (7-bit_in_byte);
        bit +=1;
      }

    }
    else
    {
      ptr_out[byte] |= u_dl_ack->release_99_str_u_grlc_dl_ack.v_pfi << (7-bit_in_byte);
      bit +=1;
    }
  }

  bit++;
#endif

  /* SPARE PADDINGS */
  byte           = bit / 8; 
  bit_in_byte    = bit % 8;

  if(bit_in_byte < 7)
  {
    UBYTE mask;

    mask= 0xff >> (bit_in_byte);
    mask &= 0x2B;
    ptr_out[byte] |= mask;
  }
  for(i=byte;i<22;i++)
  {
    byte++;
    ptr_out[byte] = 0x2B;

  }
} /* grlc_encode_dl_acknack() */
  
/*
+------------------------------------------------------------------------------
| Function    : grlc_encode_ul_dummy
+------------------------------------------------------------------------------
| Description : The function grlc_encode_ul_dummy() encodes the packet uplink
|               dummy block without CCD
|
| Parameters  : *ptr_out_i- ptr to the buffer where the air message will be placed
|
|
+------------------------------------------------------------------------------
*/
LOCAL void grlc_encode_ul_dummy ( UBYTE * ptr_out)
{ 
  MCAST (ul_dummy,U_GRLC_UL_DUMMY); /* T_U_GRLC_UL_DUMMY */

  UBYTE i;
  
  TRACE_FUNCTION( "grlc_encode_ul_dummy" );

  /******************* mandatory elements   ****************************/

  ptr_out[0]  = ul_dummy->msg_type << 2;
  ptr_out[0] |= (UBYTE) (grlc_data->uplink_tbf.tlli >> 30);                  /* 1100 0000 0000 0000 0000 0000 0000 0000 */
  ptr_out[1]  = (UBYTE) ((grlc_data->uplink_tbf.tlli & 0x3FC00000) >> 22);   /* 0011 1111 1100 0000 0000 0000 0000 0000 */
  ptr_out[2]  = (UBYTE) ((grlc_data->uplink_tbf.tlli & 0x003FC000) >> 14);   /* 0000 0000 0011 1111 1100 0000 0000 0000 */
  ptr_out[3]  = (UBYTE) ((grlc_data->uplink_tbf.tlli & 0x00003FC0) >>  6);   /* 0000 0000 0000 0000 0011 1111 1100 0000 */ 
  ptr_out[4]  = (UBYTE) ((grlc_data->uplink_tbf.tlli & 0x0000003F) <<  2);   /* 0000 0000 0000 0000 0000 0000 0011 1111 */ 
  ptr_out[4] |= 0x01;  /* spare paddings */

  for(i=5;i<22;i++)
    ptr_out[i] = 0x2B;
} /* grlc_encode_ul_dummy() */

/*
+------------------------------------------------------------------------------
| Function    : grlc_encode_pca
+------------------------------------------------------------------------------
| Description : The function grlc_encode_pca() encodes the packet control 
|               acknowledgement block without CCD
|
| Parameters  : *ptr_out_i- ptr to the buffer where the air message will be placed
|
|
+------------------------------------------------------------------------------
*/
LOCAL void grlc_encode_pca ( UBYTE * ptr_out)
{ 
 MCAST(u_ctrl_ack,U_GRLC_CTRL_ACK); /* T_U_GRLC_CTRL_ACK */

  UBYTE i;
  
  TRACE_FUNCTION( "grlc_encode_pca" );

  /******************* mandatory elements   ****************************/

  ptr_out[0]  = u_ctrl_ack->msg_type << 2;
  ptr_out[0] |= (UBYTE) (grlc_data->uplink_tbf.tlli >> 30);                  /* 1100 0000 0000 0000 0000 0000 0000 0000 */
  ptr_out[1]  = (UBYTE) ((grlc_data->uplink_tbf.tlli & 0x3FC00000) >> 22);   /* 0011 1111 1100 0000 0000 0000 0000 0000 */
  ptr_out[2]  = (UBYTE) ((grlc_data->uplink_tbf.tlli & 0x003FC000) >> 14);   /* 0000 0000 0011 1111 1100 0000 0000 0000 */
  ptr_out[3]  = (UBYTE) ((grlc_data->uplink_tbf.tlli & 0x00003FC0) >>  6);   /* 0000 0000 0000 0000 0011 1111 1100 0000 */ 
  ptr_out[4]  = (UBYTE) ((grlc_data->uplink_tbf.tlli & 0x0000003F) <<  2);   /* 0000 0000 0000 0000 0000 0000 0011 1111 */ 
  ptr_out[4] |= u_ctrl_ack->pctrl_ack & 0x03;  

  for(i=5;i<22;i++)
    ptr_out[i] = 0x2B;
} /* grlc_encode_pca() */
/*
+------------------------------------------------------------------------------
| Function    : grlc_decode_ul_acknack
+------------------------------------------------------------------------------
| Description : The function grlc_decode_ul_acknack() decodes the packet uplink
|               ack/nack without CCD
|
| Parameters  : *ptr_blk- ptr to the air message
|
| Return value: returns decode status of air message:ccdOK,ccdWarning,ccdError 
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grlc_decode_ul_acknack ( UBYTE *ptr_blk  )
{ 
  MCAST(d_ul_ack,D_GRLC_UL_ACK);/* T_D_GRLC_UL_ACK */
  
  UBYTE i,j,k;
  UBYTE bit_pos;
#ifdef REL99
  UBYTE flag;
#endif
  
  UBYTE result =ccdOK;

  TRACE_FUNCTION( "grlc_decode_ul_acknack" );

  /******************* mandatory elements   ****************************/

  d_ul_ack->msg_type               = (ptr_blk[0] & 0xFC) >> 2 ;
  d_ul_ack->page_mode              = ptr_blk[0] & 0x03; 

  if((ptr_blk[1] & 0xC0)) /* distrubiton part error check */
  {
    TRACE_ERROR("P UL ACK: DISTRUBITION PART ERROR");
    TRACE_EVENT_P2("P UL ACK: DISTRUBITION PART ERROR byte = 0x2%x   res = 0x%2x",ptr_blk[1] ,ptr_blk[1] & 0xC0);
    return ccdError;
  }
  d_ul_ack->ul_tfi                 = (ptr_blk[1] & 0x3E) >> 1;

  if((ptr_blk[1] & 0x01)) /* message escape bit check */
  {
#ifdef REL99
    d_ul_ack->egprs_flag = TRUE;
#endif
    TRACE_ERROR("P UL ACK: MESSAGE ESCAPE ERROR");
    TRACE_EVENT_P2("P UL ACK: MESSAGE ESCAPE ERROR byte = 0x2%x   res = 0x%2x",ptr_blk[1] ,ptr_blk[1] & 0x01);
    return ccdError;
  }

  d_ul_ack->v_gprs_ul_ack_nack_info = TRUE;

  d_ul_ack->gprs_ul_ack_nack_info.chan_coding_cmd        = (ptr_blk[2] & 0xC0) >> 6;

#ifdef _SIMULATION_
  TRACE_EVENT_P2("tfi=%d ch_c_cmd=%d",d_ul_ack->ul_tfi,d_ul_ack->gprs_ul_ack_nack_info.chan_coding_cmd);
#endif /* _SIMULATION_ */
  /******************* Ack/Nack description  ****************************/
  d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.f_ack_ind = (ptr_blk[2] & 0x20) >> 5;                  
  d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.ssn       = (ptr_blk[2] << 2) & 0x7C;
  d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.ssn      |= (ptr_blk[3] >> 6) & 0x03;

  j= 3; /* inital byte of rbb field */
  k= 2; /* inital bit of rbb field  */

  for(i=0; i< 64;i++)
  {
    d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.rbb[i] = (ptr_blk[j] & (0x80>>k)) >> (7-k);
    k++;
    if(k EQ 8)
    {
      k=0;
      j++;
    }
  }
#ifdef _SIMULATION_
  TRACE_EVENT_P2("fai=%d ssn=%d",d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.f_ack_ind,d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.ssn);
#endif /* _SIMULATION_ */

  bit_pos = 91;          /* abs bit position */

  /******************* contention resolution tlli ***********************/
    
  j       = bit_pos / 8; /* byte pos         */
  k       = bit_pos % 8; /* rel bit pos      */

  d_ul_ack->gprs_ul_ack_nack_info.v_cr_tlli = (ptr_blk[j] >> (8-k)) & 0x01; 

  bit_pos++;

  if(d_ul_ack->gprs_ul_ack_nack_info.v_cr_tlli)
  {
    d_ul_ack->gprs_ul_ack_nack_info.cr_tlli.l_cr_tlli = 32;
    d_ul_ack->gprs_ul_ack_nack_info.cr_tlli.o_cr_tlli = 3;        
    d_ul_ack->gprs_ul_ack_nack_info.cr_tlli.b_cr_tlli[0] = ptr_blk[j];
    d_ul_ack->gprs_ul_ack_nack_info.cr_tlli.b_cr_tlli[1] = ptr_blk[j+1];
    d_ul_ack->gprs_ul_ack_nack_info.cr_tlli.b_cr_tlli[2] = ptr_blk[j+2];
    d_ul_ack->gprs_ul_ack_nack_info.cr_tlli.b_cr_tlli[3] = ptr_blk[j+3];
    d_ul_ack->gprs_ul_ack_nack_info.cr_tlli.b_cr_tlli[4] = ptr_blk[j+4];
    bit_pos+=32;
  }

#ifdef REL99
  /******************* packet timing advance ****************************/
  j       = bit_pos / 8; /* byte pos         */
  k       = bit_pos % 8; /* rel bit pos      */
  flag    = (ptr_blk[j] >> (8-k)) & 0x01; 
  bit_pos++;
  if(flag)
  {
  }
  /******************* power control params ****************************/
  j       = bit_pos / 8; /* byte pos         */
  k       = bit_pos % 8; /* rel bit pos      */
  flag    = (ptr_blk[j] >> (8-k)) & 0x01; 
  bit_pos++;
  if(flag)
  {
  }
  /*******************     Extension bits   ****************************/
  j       = bit_pos / 8; /* byte pos         */
  k       = bit_pos % 8; /* rel bit pos      */
  flag    = (ptr_blk[j] >> (8-k)) & 0x01; 
  bit_pos++;
  if(flag)
  {
  }
  /*******************     Fixed Alloc      ****************************/
  j       = bit_pos / 8; /* byte pos         */
  k       = bit_pos % 8; /* rel bit pos      */
  flag    = (ptr_blk[j] >> (8-k)) & 0x01; 
  bit_pos++;
  if(flag)
  {
  }
  /*******************     R99 FLAG           ****************************/
  j                       = bit_pos / 8; /* byte pos         */
  k                       = bit_pos % 8; /* rel bit pos      */
  bit_pos++;
  d_ul_ack->gprs_ul_ack_nack_info.v_release_99_str_d_ul_ack    = (ptr_blk[j] >> (8-k)) & 0x01; 
  /*******************     Extended PTA      ****************************/
  j       = bit_pos / 8; /* byte pos         */
  k       = bit_pos % 8; /* rel bit pos      */
  flag    = (ptr_blk[j] >> (8-k)) & 0x01; 
  bit_pos++;
  if(flag)
  {
  }
  /**********************************************************************/
  j                    = bit_pos / 8; /* byte pos         */
  k                    = bit_pos % 8; /* rel bit pos      */
  d_ul_ack->gprs_ul_ack_nack_info.release_99_str_d_ul_ack.tbf_est    = (ptr_blk[j] >> (8-k)) & 0x01; 

#endif

  return(result);

} /* grlc_decode_ul_acknack() */

/*
+------------------------------------------------------------------------------
| Function    : grlc_ccd_error_handling
+------------------------------------------------------------------------------
| Description : The function grlc_ccd_error_handling() ...
|
| Parameters  : entity_i - the CCD was called for this entity
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grlc_ccd_error_handling ( UBYTE entity_i  )
{ 
  UBYTE result = DELETE_MESSAGE;
  USHORT parlist [MAX_ERR_PAR];
  UBYTE first_error; 

  TRACE_FUNCTION( "grlc_ccd_error_handling" );


  memset (parlist, 0, sizeof (parlist));
  
  first_error = ccd_getFirstError (entity_i, parlist);
  
  switch (first_error)
  {
    
  case ERR_PATTERN_MISMATCH:      /* A spare pattern does not match with  */
    /* the specified content                */
    /* Error params[0] = bitposition        */
    {
      MCAST(ptr,D_GRLC_UL_ACK);
      result = ptr->msg_type;
    }
    break;
    
  default:
    /* SZML-GLBL/010 */
    TRACE_ERROR( "Ctrl-Message will be deleted" );
    break;
  }
  
  return(result);

} /* grlc_ccd_error_handling() */


/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : grlc_buffer2ulong
+------------------------------------------------------------------------------
| Description : The function grlc_buffer2ulong() copy a 32-Bit-Buffer in a ULONG
|               variable
|               
|               SZML-GLBL/002
|
| Parameters  : ptmsi - pointer to buffer that contains the 32bit for the ULONG 
|
+------------------------------------------------------------------------------
*/
GLOBAL ULONG grlc_buffer2ulong ( BUF_cr_tlli *tlli)
{
  ULONG ul;
 
  UBYTE l, dummy; 
  USHORT i, ii;
  UBYTE off1, off2;
  
  TRACE_FUNCTION( "grlc_buffer2ulong "); 
  
  ul= 0;
  
  l = (UBYTE)tlli->l_cr_tlli;
  
  off1 = tlli->o_cr_tlli / 8;
  off2 = tlli->o_cr_tlli % 8;
  
  dummy = 0;
  dummy = tlli->b_cr_tlli[off1] << off2;
  
  if(l <= (8-off2))
  {
    dummy = dummy >> (8-l);
    ul |= dummy;
    return ul;
  }
  dummy = dummy >> off2;
  ul |= dummy;
  l -= (8-off2);
  
  do
  {
    off1++;
    
    if(l < 8)
    {
      dummy = tlli->b_cr_tlli[off1] >> (8-l);
      ii = 1;
      ul = ul << l;
      for(i=0; i< l; i++)
      {
        ul = ul | (dummy & ii);
        ii *= 2;
      }
      return ul;  
    }
    else
    {
      ul = ul << 8;
      ul |= tlli->b_cr_tlli[off1];
      l -= 8;
      if(l EQ 0)
        return ul;
    }
  }
  while(TRUE);
}



/*
+------------------------------------------------------------------------------
| Function    : grlc_delete_prim
+------------------------------------------------------------------------------
| Description : The function grlc_delete_prim() deletes the primitive that is   
|               pointed by the grlc_data->prim_start_tbf and sets the 
|               grlc_data->prim_start_tbf to the next entry in the tbf list.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_delete_prim ( void )
{ 
  TRACE_FUNCTION( "grlc_delete_prim" );

  /*
   * access type is reseted 
   */
  grlc_data->uplink_tbf.access_type = CGRLC_AT_NULL;

  if(grlc_data->prim_start_tbf < PRIM_QUEUE_SIZE_TOTAL)
  {
    UBYTE i;  

    /* 
     * get first entry from tbf list 
     */
    i = grlc_prim_get_first (&grlc_data->prim_start_tbf);


    if(i >= PRIM_QUEUE_SIZE)
    {
      TRACE_EVENT_P5("delete prim VOR i=%d  ps=%d, pf=%d,sps=%d,spf=%d",
                                                      i,
                                                      grlc_data->prim_start_tbf,
                                                      grlc_data->prim_start_free,
                                                      grlc_data->save_prim_start_tbf,
                                                      grlc_data->save_prim_start_free);
    }

    /*
     *   estimate new user data amount in queue
     */
    grlc_data->prim_user_data -= BYTELEN(grlc_data->prim_queue[i].prim_ptr->sdu.l_buf); 

    /* 
     * free primitive before reset old primitive entry
     */    
    PFREE ( grlc_data->prim_queue[i].prim_ptr );

    /* 
     * reset old primitive 
     */
    grlc_data->prim_queue[i].prim_ptr = NULL;
    grlc_data->prim_queue[i].prim_type = CGRLC_LLC_PRIM_TYPE_NULL;
    grlc_data->prim_queue[i].cv_status = FALSE;  
    grlc_data->prim_queue[i].rlc_status = FALSE;
    grlc_data->prim_queue[i].re_allocation = FALSE;
    grlc_data->prim_queue[i].start_new_tbf = FALSE;
    grlc_data->prim_queue[i].last_bsn = 0xff;
    grlc_data->prim_queue[i].previous = 0xff;

    /* 
     * put new entry at the end of free list 
     */

    if(i < PRIM_QUEUE_SIZE)
    {
      grlc_prim_put(&grlc_data->prim_start_free,i,END_OF_LIST);
    }
    else  if(grlc_data->gmm_procedure_is_running)
    {
     grlc_prim_put(&grlc_data->prim_start_free,i,END_OF_LIST);
     TRACE_EVENT_P5("delete prim %d AFTER PST=%d, PSF=%d,spst=%d,spsf=%d",
                                                      i,
                                                      grlc_data->prim_start_tbf,
                                                      grlc_data->prim_start_free,
                                                      grlc_data->save_prim_start_tbf,
                                                      grlc_data->save_prim_start_free);
    }

    /*
     * update LLC flow control state
     */
    if ((grlc_data->tm.send_grlc_ready_ind EQ PRIM_QUEUE_FULL) AND            /* PRIM QUEUE IS FULL */
         (
        (!grlc_data->gmm_procedure_is_running AND (i < PRIM_QUEUE_SIZE))  OR  /* LLC QUEUE IS ACTIVE, PDU FROM LLC QUEUE DELETED */
        (grlc_data->gmm_procedure_is_running AND !(i < PRIM_QUEUE_SIZE))))    /* GMM QUEUE IS ACTIVE, PDU FROM GMM QUEUE DELETED */
    {
      TRACE_EVENT_P3("Flow control activated gmm_q=%d,i=%d,ready=%d",grlc_data->gmm_procedure_is_running,i,grlc_data->tm.send_grlc_ready_ind);
      grlc_data->tm.send_grlc_ready_ind = SEND_A_GRLC_READY_IND;
    }

    grlc_data->grlc_data_req_cnt--;
  }
  else
  {
    TRACE_EVENT_P3("PST=%d PSF=%d PDU=%d: grlc_delete_prim"
                                                           ,grlc_data->prim_start_tbf
                                                           ,grlc_data->prim_start_free
                                                           ,grlc_data->grlc_data_req_cnt);
    return; 
  }


} /* grlc_delete_prim() */






  
/*
+------------------------------------------------------------------------------
| Function    : grlc_calc_new_poll_pos
+------------------------------------------------------------------------------
| Description : The function grlc_calc_new_poll_pos() calculates the fn of the 
|               new poll position
|
| Parameters  : fn_i    - framenumber
|               rrbp_i  - relative position
|
+------------------------------------------------------------------------------
*/
GLOBAL ULONG grlc_calc_new_poll_pos ( ULONG fn_i, UBYTE rrbp_i )
{ 
  ULONG result=0;
  TRACE_FUNCTION( "grlc_calc_new_poll_pos" );

  switch( rrbp_i )
  {
  case 0:
    result = (fn_i+13);
    break;
  case 1:
    if((fn_i+18)%13)
      result = (fn_i+17);
    else
      result = (fn_i+18);
    break;
  case 2:
    if(((fn_i+21)%13) EQ 8)
      result = (fn_i+21);
    else
      result = (fn_i+22);
    break;
  case 3:
    result = (fn_i+26);
    break;
  default:
    TRACE_ERROR( "unexpected rrbp  value" );
    break;
  } /* switch (rrbp_i) */
  result = result % 0x297000; 

  return result;

} /* grlc_calc_new_poll_pos() */




/*
+------------------------------------------------------------------------------
| Function    : grlc_get_new_poll_index
+------------------------------------------------------------------------------
| Description : The function grlc_get_new_poll_index()
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grlc_get_new_poll_index ( UBYTE * ptr_list_start_i )
{ 
  UBYTE result;
  TRACE_FUNCTION( "grlc_get_new_poll_index" );

  result = *ptr_list_start_i;                                                   
  /* set to new first entry */
  if (result NEQ 0xFF)
  {    
    *ptr_list_start_i = grlc_data->next_poll_array[*ptr_list_start_i].next;
    /* remove first entry from list  */
    grlc_data->next_poll_array[result].next = 0xff;    
  }
  else
  {
    TRACE_EVENT ("Poll array is full");
  }

  return(result);

} /* grlc_get_new_poll_index() */




/*
+------------------------------------------------------------------------------
| Function    : grlc_save_poll_pos
+------------------------------------------------------------------------------
| Description : The function grlc_save_poll_pos() 
|
| Parameters  : fn_i        - framenumber  
|               tn_i        - timeslot number
|               rrbp_i      - fn of the poll block|               
|               poll_type_i - kind of dl data
|               pctrl_ack_i - packet control ack value, needed fo p ctr ack msg
|               
|               
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_save_poll_pos ( ULONG fn_i, USHORT tn_i, UBYTE rrbp_i, UBYTE poll_type_i,UBYTE pctrl_ack_i)
{
  ULONG new_poll_pos;
  UBYTE i, next, help_index;

  TRACE_FUNCTION( "grlc_save_poll_pos" );

  if( tn_i >= POLL_TYPE_ARRAY_SIZE )
  {
    TRACE_EVENT_P5( "grlc_save_poll_pos: fn = %d, tn = %d, rrbp = %d, poll_type = %d, pctrl_ack = %d",
                    fn_i, tn_i, rrbp_i, poll_type_i, pctrl_ack_i );

    return;
  }

  if(0xFF NEQ rrbp_i)
  {
    new_poll_pos = grlc_calc_new_poll_pos(fn_i, rrbp_i);
  }
  else
  {
    new_poll_pos = fn_i;
  }

  /*TRACE_EVENT_P6("SAVE BEF: fn_i= %ld,rrbp=%ld,new_poll_pos=%ld, poll_type=%d,ps=%d ps_fn=%ld",
                                                fn_i,
                                                rrbp_i,
                                                new_poll_pos,
                                                poll_type_i,
                                                grlc_data->poll_start_tbf,
                                                grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn);

  */
  /* delete the poll position when it is older than 26 frames */
  while( grlc_data->poll_start_tbf NEQ 0xFF  AND 
         grlc_check_dist(new_poll_pos, 
                         grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn, 26) EQ FALSE)
  {
    /* move the expired poll position to the list of unused entries */
    TRACE_EVENT_P1("Remove expired poll at pst=%d",grlc_data->poll_start_tbf);
    help_index                                  = grlc_data->poll_start_tbf;
    grlc_data->poll_start_tbf                   = grlc_data->next_poll_array[help_index].next;
    grlc_data->next_poll_array[help_index].next = grlc_data->poll_start_free;
    grlc_data->poll_start_free                  = help_index;
    
    /* clear the poll position */
    grlc_data->next_poll_array[help_index].fn  = 0xFFFFFFFF;
    grlc_data->next_poll_array[help_index].cnt = 0;
    
    for( i = 0; i < POLL_TYPE_ARRAY_SIZE; i++ )
    {
      grlc_data->next_poll_array[help_index].poll_type[i] = CGRLC_POLL_NONE;      
    }
  }
  
  next       = 0xFF;
  help_index = grlc_data->poll_start_tbf;


  /*
   * find the position of the new fn
   */
  
  while( help_index NEQ 0xFF )
  {
    if( grlc_data->next_poll_array[help_index].fn EQ new_poll_pos )
    {
      next       = help_index;
      help_index = 0xFF;
    }
    else if( grlc_check_dist( new_poll_pos, grlc_data->next_poll_array[help_index].fn, 26 ) )
    {
      next       = help_index;
      help_index = grlc_data->next_poll_array[help_index].next;
    }
    else
    {
      help_index = 0xFF;
    }
  }

  /*
   * new_poll_pos is present in the poll array
   */
  if( next                                            NEQ 0xFF         AND
      grlc_data->next_poll_array[next].fn              EQ  new_poll_pos AND 
      grlc_data->next_poll_array[next].poll_type[tn_i] EQ  CGRLC_POLL_NONE        ) 
  {  
     /*
      * no collision 
      */
     grlc_data->next_poll_array[next].poll_type[tn_i] = poll_type_i;
     grlc_data->next_poll_array[next].cnt++;
     grlc_data->next_poll_array[next].ctrl_ack        = pctrl_ack_i;
  }
  else if( next                               NEQ 0xFF         AND
           grlc_data->next_poll_array[next].fn EQ  new_poll_pos     )

  { 
    /*
     * collision detected, if both pos were received with data blocks, the sent 
     * RLC/MAC block otherwise send packet control ack.
     */
    if ((poll_type_i NEQ CGRLC_POLL_DATA) 
         OR
        (grlc_data->next_poll_array[next].poll_type[tn_i] NEQ CGRLC_POLL_DATA))
    {
       grlc_data->next_poll_array[next].poll_type[tn_i] = CGRLC_POLL_COLLISION;
       TRACE_EVENT("collision detected: pca will sent");
    }
    else
    {
      TRACE_EVENT("collision detected: rlc/mac will be sent");
    }
    if(pctrl_ack_i NEQ 3)
      grlc_data->next_poll_array[next].ctrl_ack        = pctrl_ack_i;
  }
  else
  { 
    /*
     * fn does not exist in the poll list, included in poll list
     *
     * get new free index from the free list 
     */
    i = grlc_get_new_poll_index(&(grlc_data->poll_start_free));    
    if( i EQ 0xFF ) 
    { 
      TRACE_EVENT ("Poll array is full"); /*This should not happen */
      return;   
    }

    grlc_data->next_poll_array[i].cnt             = 1;
    grlc_data->next_poll_array[i].fn              = new_poll_pos;
    grlc_data->next_poll_array[i].poll_type[tn_i] = poll_type_i; 
    grlc_data->next_poll_array[i].ctrl_ack        = pctrl_ack_i;

    if( next EQ 0xFF )
    { 
      /* 
       * first entry in poll array 
       */
      grlc_data->next_poll_array[i].next    = grlc_data->poll_start_tbf;
      grlc_data->poll_start_tbf             = i;
    }
    else if( next < NEXT_POLL_ARRAY_SIZE )
    {
      /*
       * include in description list, is not first element 
       */
      grlc_data->next_poll_array[i].next            = grlc_data->next_poll_array[next].next;
      grlc_data->next_poll_array[next].next         = i;
    }
    else
    {
      TRACE_ASSERT( next < NEXT_POLL_ARRAY_SIZE );
    }
  }


  /*TRACE_EVENT_P6("SAVE AFTER: fn_i= %ld,rrbp=%ld,new_poll_pos=%ld, poll_type=%d,ps=%d ps_fn=%ld",
                                                fn_i,
                                                rrbp_i,
                                                new_poll_pos,
                                                poll_type_i,
                                                grlc_data->poll_start_tbf,
                                                grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn);
   */

} /* grlc_save_poll_pos() */






/*
+------------------------------------------------------------------------------
| Function    : grlc_encode_ctrl
+------------------------------------------------------------------------------
| Description : The function grlc_encode_ctrl() build a T_SDU buffer that 
|               contains the encode Ctrl Block ready to transmit. 
|
| Parameters  : ULONG ptr_in_i - ptr to the input structure
|               ULONG ptr_out_i - ptr to begin of output buffer
|               UBYTE r_bit_i - value of r_bit
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_encode_ctrl ( UBYTE * ptr_in_i, 
                              T_MSGBUF * ptr_out_i, 
                              UBYTE r_bit_i)
{ 
  TRACE_FUNCTION( "grlc_encode_ctrl" );

  ptr_out_i->buf[0] = 0x40 | r_bit_i;
  ptr_out_i->o_buf  = BIT_UL_CTRL_BLOCK_MAC_HEADER;
  ptr_out_i->l_buf  = BIT_UL_CTRL_BLOCK_CONTENTS;


  if(!grlc_data->grlc_wo_ccd)
  {
    ccd_codeMsg ( CCDENT_GRLC, 
                  UPLINK,
                  ptr_out_i,
                  ptr_in_i, 
                  NOT_PRESENT_8BIT);
  }
  else
  {
    switch(ptr_in_i[0]) /* msg_type */
    {
      case U_GRLC_UL_DUMMY_c:
        grlc_encode_ul_dummy(&ptr_out_i->buf[1]);
        break;
      case U_GRLC_CTRL_ACK_c:
        grlc_encode_pca(&ptr_out_i->buf[1]);
        break;
      case U_GRLC_DL_ACK_c:
        memset(&ptr_out_i->buf[1],0,22); /*lint !e419*/
        grlc_encode_dl_acknack(&ptr_out_i->buf[1]);
      break;
      default:
        ccd_codeMsg ( CCDENT_GRLC, 
                      UPLINK,
                      ptr_out_i,
                      ptr_in_i, 
                      NOT_PRESENT_8BIT);
        break;
    }
  }
  ptr_out_i->l_buf += ptr_out_i->o_buf;
  ptr_out_i->o_buf  = 0;

} /* grlc_encode_ctrl() */


/*
+------------------------------------------------------------------------------
| Function    : grlc_init
+------------------------------------------------------------------------------
| Description : The function grlc_init initializes the entity GRLC
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_init ( void )
{ 
  TRACE_FUNCTION( "grlc_init" );
  
  /* initialize all GRLC data */
  grlc_data = &grlc_data_base;
  memset( grlc_data, 0, sizeof( T_GRLC_DATA ) );

  grlc_data->grlc_data_req_cnt = 0;
  grlc_data->testmode.mode     = CGRLC_NO_TEST_MODE;

  grlc_data->meas.sq_restart = TRUE;
  
  /* 
   * call of service init functions 
   */
  tm_grlc_init();
  rd_init();
  ru_init();
  meas_grlc_init();
  tpc_grlc_init();
  gff_init();

  ccd_register(CCD_REENTRANT);  

#ifdef _SIMULATION_
  /* 
   * to make sure that the structure definitions of T_GRLC_DATA_REQ and 
   * T_GRLC_UNITDATA_REQ have the same layout.
   */
  {
    T_GRLC_DATA_REQ * ptr_grlc_data_req = (T_GRLC_DATA_REQ *)_decodedMsg;
    T_GRLC_UNITDATA_REQ * ptr_grlc_unitdata_req = (T_GRLC_UNITDATA_REQ *)_decodedMsg;

    if( &(ptr_grlc_data_req->grlc_qos)   NEQ &(ptr_grlc_unitdata_req->grlc_qos) AND
        &(ptr_grlc_data_req->radio_prio) NEQ &(ptr_grlc_unitdata_req->radio_prio) AND
        &(ptr_grlc_data_req->sdu)        NEQ &(ptr_grlc_unitdata_req->sdu) AND
        &(ptr_grlc_data_req->tlli)       NEQ &(ptr_grlc_unitdata_req->tlli) )
    {
      /*
       * In this case the primitive handling will not work correctly!!!
       */
      TRACE_ERROR("Fatal ERROR: T_GRLC_UNITDATA_REQ and T_GRLC_DATA_REQ are not equal!!");
    }
  } /* _SIMULATION_ */
#endif

  grlc_data->t3164_to_cnt           = 0;
  grlc_data->ul_tfi_changed         = FALSE;
  grlc_data->uplink_tbf.access_type = CGRLC_AT_NULL; /* NO CELL UPDATE NEED */
  grlc_data->grlc_wo_ccd            = 0;             /* as default ccd used for air message handling */

  /*
   * Ready Timer state initialization 
   */
#ifdef FF_GRLC_4_TWO_2_ONE
  grlc_data->ready_timer.handling   = READY_TIMER_HANDLING_ENABLED;
#else
  grlc_data->ready_timer.handling   = READY_TIMER_HANDLING_DISABLED;
#endif

  grlc_data->ready_timer.state      = STANDBY_STATE; 
  grlc_data->ready_timer.value      = CGRLC_T3314_DEFAULT;

} /* grlc_init() */



/*
+------------------------------------------------------------------------------
| Function    : grlc_prim_put
+------------------------------------------------------------------------------
| Description : The function grlc_prim_put() put a Element (object_i) behind 
|               pos_i in the prim_queue.
|
| Parameters  : list_start_i  - address of the list should be manipulted  
|               object_i      - index of element that should be added to the list
|               pos_i         - index of the position behind that the object 
|                               should be added 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_prim_put(UBYTE * list_start_i, UBYTE object_i, UBYTE pos_i)
{ 
  TRACE_FUNCTION( "grlc_prim_put" );

  if(* list_start_i EQ 0xff)
  { /* no elements in the list */
    * list_start_i = object_i;
  }
  else
  { /* elements in the list */
    UBYTE i = * list_start_i;
    UBYTE j;
    BOOL put= FALSE;
    
    /* 
     * SZML-GLBL/004
     */
    for(j=0;j<PRIM_QUEUE_SIZE_TOTAL;j++)
    {
      if (grlc_data->prim_queue[i].next NEQ pos_i)
      {
        i = grlc_data->prim_queue[i].next;
      }
      else
      {
        /* 
         * put new object at pos_i of queue 
         */
        put = TRUE;
        grlc_data->prim_queue[object_i].next = grlc_data->prim_queue[i].next;
        grlc_data->prim_queue[i].next = object_i;
        break;
      }
    }
    if(!put)
      TRACE_EVENT_P3("PST=%d PSF=%d PDU=%d: grlc_prim_put failed"
                                                           ,grlc_data->prim_start_tbf
                                                           ,grlc_data->prim_start_free
                                                           ,grlc_data->grlc_data_req_cnt);

  }
} /* grlc_prim_put() */



/*
+------------------------------------------------------------------------------
| Function    : grlc_prim_get_first
+------------------------------------------------------------------------------
| Description : The function grlc_prim_get_first() removes the first element
|               and returns the index of the first element.
|
| Parameters  : list_start_i - address of the list that should be used
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE grlc_prim_get_first(UBYTE * list_start_i)
{ 
  UBYTE result;
  TRACE_FUNCTION( "grlc_prim_get_first" );

  result = *list_start_i;
                                                    
  /* 
   * set to new first entry 
   */
  *list_start_i = grlc_data->prim_queue[*list_start_i].next;

  /*
   * remove first entry from list 
   */
  grlc_data->prim_queue[result].next = 0xff;  

  return(result);

} /* grlc_prim_get_first() */


/*
+------------------------------------------------------------------------------
| Function    : grlc_set_packet_ctrl_ack
+------------------------------------------------------------------------------
| Description : The function grlc_set_packet_ctrl_ack() |
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE * grlc_set_packet_ctrl_ack(void)
{ 
  MCAST(u_ctrl_ack,U_GRLC_CTRL_ACK);
  UBYTE * result;
  TRACE_FUNCTION( "grlc_set_packet_ctrl_ack" );

  u_ctrl_ack->msg_type  = U_GRLC_CTRL_ACK_c;

  grlc_set_buf_tlli( &u_ctrl_ack->tlli_value, grlc_data->uplink_tbf.tlli );

  if(grlc_data->poll_start_tbf NEQ 0xFF)
    u_ctrl_ack->pctrl_ack = grlc_data->next_poll_array[grlc_data->poll_start_tbf].ctrl_ack;
  else /* response to IA poll */
    u_ctrl_ack->pctrl_ack = 3; 

  result                = _decodedMsg;

  return(result);

} /* grlc_set_packet_ctrl_ack() */



/*
+------------------------------------------------------------------------------
| Function    : grlc_send_access_burst
+------------------------------------------------------------------------------
| Description : The function grlc_send_access_burst() sents the poll as 
|               four access burst type
|
| Parameters  : tn_i : timeslot where the access burst shall send
|
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_send_access_burst(UBYTE tn_i)
{ 
  TRACE_FUNCTION( "grlc_send_access_burst" );

  

  if(grlc_data->poll_start_tbf NEQ 0xFF)
  {
    grlc_data->next_poll_array[grlc_data->poll_start_tbf].cnt--;
    grlc_data->next_poll_array[grlc_data->poll_start_tbf].poll_type[tn_i] = CGRLC_POLL_NONE;
    grlc_data->next_poll_fn = grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn;
  }
  grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index].tn = tn_i;
  if(grlc_data->ab_type EQ CGRLC_AB_11_BIT)
  {
    /*11 bit access burst*/
    USHORT elevenBit = 0x07E4+grlc_data->next_poll_array[grlc_data->poll_start_tbf].ctrl_ack;
    grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index].block_status = 8;
    grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index].ul_block[0]  = grlc_convert_11bit_2_etsi(elevenBit);
    /*
    TRACE_EVENT_P2("PCA AB_11_BIT sent fn_i= %ld PRACH11: %d"
                                              ,grlc_data->next_poll_fn
                                              ,grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index].ul_block[0]);
    */
  }
  else
  {
    /*8 bit access burst*/
    grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index].block_status = 7;
    grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index].ul_block[0]  = (0x7C+grlc_data->next_poll_array[grlc_data->poll_start_tbf].ctrl_ack);
    /*
    TRACE_EVENT_P1 ("PCA 8 bit access burst sent fn_i= %ld ",grlc_data->next_poll_fn);      
    */
  }
    
#ifdef _SIMULATION_
  {
    PALLOC(mac_poll_req,MAC_POLL_REQ);
    memset(mac_poll_req,0,
           sizeof(T_MAC_POLL_REQ));
    memcpy(&(mac_poll_req->ul_poll_resp),
           &(grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index]),
             sizeof(T_ul_poll_resp));
    PSEND(hCommL1,mac_poll_req);          
  }
#else  /* #ifdef _SIMULATION_ */
  {
    TRACE_MEMORY_PRIM ( hCommGRLC, hCommL1, MAC_POLL_REQ,
                        &grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index],
                        sizeof(T_ul_poll_resp) );
  }
#endif /* #ifdef _SIMULATION_ */

  TRACE_BINDUMP
    ( hCommGRLC, TC_USER4, 
      cl_rlcmac_get_msg_name( U_MSG_TYPE_CHANNEL_REQ_c, RLC_MAC_ROUTE_UL ),
      grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index].ul_block,
      RLC_MAC_MAX_LEN_CHANNEL_REQ ); /*lint !e569*/

  grlc_data->ul_poll_pos_index++;
  /* 
   * next poll block invalid
   */
  grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index].block_status = 0;    


} /* grlc_send_access_burst() */

/*
+------------------------------------------------------------------------------
| Function    : grlc_send_normal_burst
+------------------------------------------------------------------------------
| Description : The function grlc_send_normal_burst() sents the poll as 
|               normal burst type
|
| Parameters  : ptr_block_i : ptr to the ctrl message
| Parameters  : tn_i        : timeslot where the access burst shall send
| Parameters  : r_bit_i     : r bit needed for the mac header
|
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_send_normal_burst
                        (UBYTE * struct_data, UBYTE * encoded_data, UBYTE tn_i)
{
  
  TRACE_FUNCTION( "grlc_send_normal_burst" );

  if( (grlc_data->poll_start_tbf NEQ 0xFF)  AND
      (grlc_data->ta_value       EQ 0xFF) )
  {
    TRACE_EVENT_P2("No TA VALUE IN GRLC --> NB POLL NOT SENT  ta=%d poll_st_tbf=%d"
                                                                ,grlc_data->ta_value
                                                                ,grlc_data->poll_start_tbf);

    TRACE_ERROR("No TA VALUE IN GRLC --> NB POLL NOT SENT");

    grlc_data->next_poll_array[grlc_data->poll_start_tbf].cnt--;
    grlc_data->next_poll_array[grlc_data->poll_start_tbf].poll_type[tn_i] = CGRLC_POLL_NONE;
    return;
  }

  /*
   * either encoded data or structured data are passed by reference
   */


  if( struct_data NEQ NULL )
  {
    grlc_encode_ul_ctrl_block( ( UBYTE* )grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index].ul_block, struct_data );
  }
  else if( encoded_data NEQ NULL )
  {
    memcpy( ( UBYTE* )grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index].ul_block, 
            encoded_data,
            BYTE_UL_CTRL_BLOCK );
  }
  else
  {
    TRACE_ERROR( "grlc_send_normal_burst: no data available" );
  }

  grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index].tn = tn_i;
  grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index].block_status = 3;

  if(grlc_data->poll_start_tbf NEQ 0xFF)
  {
    grlc_data->next_poll_array[grlc_data->poll_start_tbf].cnt--;
    grlc_data->next_poll_array[grlc_data->poll_start_tbf].poll_type[tn_i] = CGRLC_POLL_NONE;
    grlc_data->next_poll_fn = grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn;
  }
  
#ifdef _SIMULATION_
  {
    PALLOC(mac_poll_req,MAC_POLL_REQ);
    memset(mac_poll_req,0,
           sizeof(T_MAC_POLL_REQ));
    memcpy(&(mac_poll_req->ul_poll_resp),
           &(grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index]),
             sizeof(T_ul_poll_resp));
    PSEND(hCommL1,mac_poll_req);          
  }
#else  /* #ifdef _SIMULATION_ */
  {
    TRACE_MEMORY_PRIM ( hCommGRLC, hCommL1, MAC_POLL_REQ,
                        &grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index],
                        sizeof(T_ul_poll_resp) );
  }
#endif /* #ifdef _SIMULATION_ */

  {
    UBYTE* ul_block = ( UBYTE* )grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index].ul_block;

    TRACE_BINDUMP( hCommGRLC, TC_USER4,
                   cl_rlcmac_get_msg_name
                     ( ( UBYTE )( ul_block[1] >> 2 ), RLC_MAC_ROUTE_UL ),
                   ul_block, MAX_L2_FRAME_SIZE ); /*lint !e569*/
  }

  grlc_data->ul_poll_pos_index++;
  /* 
   * next poll block invalid
   */
  grlc_data->ul_poll_resp[grlc_data->ul_poll_pos_index].block_status = 0;    

  /*
  TRACE_EVENT_P1 ("Normal burst sent at fn_i= %ld ",grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn);
  */
} /* grlc_send_normal_burst() */

/*
+------------------------------------------------------------------------------
| Function    : grlc_del_sent_poll
+------------------------------------------------------------------------------
| Description : The function grlc_del_sent_poll() deletes the poll pos which was 
|               sent to L1
|
| Parameters  

|
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_del_sent_poll(void )
{ 
  UBYTE     help_index;
  UBYTE     next_index;

  TRACE_FUNCTION( "grlc_del_sent_poll" );
   
  
  if( grlc_data->next_poll_array[grlc_data->poll_start_tbf].cnt EQ 0 )
  {
    help_index = grlc_data->poll_start_tbf;
    grlc_data->poll_start_tbf = grlc_data->next_poll_array[grlc_data->poll_start_tbf].next;
    next_index = grlc_data->poll_start_free; 
    while(grlc_data->next_poll_array[next_index].next NEQ 0xFF)
    {
       next_index = grlc_data->next_poll_array[next_index].next;
    }
    grlc_data->next_poll_array[next_index].next = help_index;
    grlc_data->next_poll_array[help_index].next = 0xFF;
  }



} /* grlc_del_sent_poll() */



/*
+------------------------------------------------------------------------------
| Function    : grlc_decode_tbf_start_rel
|------------------------------------------------------------------------------
| Description : The function grlc_decode_tbf_start_rel() translates the TBF- 
|               Starting-Time-Relative into full frame number. Therefore the 
|               received frame number is needed in start_fn !! 
|
| Parameters  : rel_pos - number in blocks added to current framenuber
|
+------------------------------------------------------------------------------
*/
GLOBAL ULONG grlc_decode_tbf_start_rel(ULONG start_fn, USHORT rel_pos)
{ 
  ULONG result;

  TRACE_FUNCTION( "grlc_decode_tbf_start_rel" );
  
  result = 4+4*rel_pos + start_fn + rel_pos/3;
  
  if ((12 EQ (result%13))  OR
       (7 EQ (result%13))  OR
       (3 EQ (result%13)))
  {
    result += 1;  
  }
  if(FN_MAX <= result)
  {
    result %= FN_MAX;
  }

  return result;
}  /* grlc_decode_tbf_start_rel */



/*
+------------------------------------------------------------------------------
| Function    : grlc_get_sdu_len_and_used_ts
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : * sdu_len - len of the current sdu in progress
|               * used_ul_resources - used uplink resources in fixed alloc mode
|
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_get_sdu_len_and_used_ts (  T_RLC_VALUES * values  )
{ 
  TRACE_FUNCTION( "grlc_get_sdu_len_and_used_ts" );
   
  switch( GET_STATE( RU ) )
  {
    case RU_NULL:
      values->sdu_len = grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_ptr->sdu.l_buf/8;
      values->cnt_ts  = 0;
      break;
    default:
      values->sdu_len = grlc_data->ru.sdu_len;
      values->cnt_ts  = grlc_data->ru.cnt_ts;          
      break;
  }
 /* grlc_get_sdu_len_and_used_ts() */
}



/*
+------------------------------------------------------------------------------
| Function    : grlc_check_dist
+------------------------------------------------------------------------------
| Description : The function grlc_check_dist() checks if high_i is bigger/equal 
|               than low_i(modulo calculation). 
|               The return value is true, if high_i is equal to low_i or 
|               bigger than low_i.
| Parameters  : high_i - expected high value
|               low_i  - expected low value
|               dist_i - max. allowed distance between high_i and low_i
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grlc_check_dist ( ULONG high_i, ULONG low_i, ULONG dist_i)
{
  BOOL  result = FALSE;
  ULONG  real_dist;

  TRACE_FUNCTION( "grlc_check_dist" );
    
  if (high_i >= low_i)
    real_dist = high_i - low_i;
  else
    real_dist = high_i + (FN_MAX-low_i);

  if (real_dist <= dist_i )
  {
     result = TRUE;
  }
  return result;
} /* grlc_check_dist() */



/*
+------------------------------------------------------------------------------
| Function    : grlc_handle_poll_pos
+------------------------------------------------------------------------------
| Description : The function grlc_handle_poll_pos()  
|                
| Parameters  : -
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_handle_poll_pos (ULONG current_fn)
{
  UBYTE     help_index;
  UBYTE     next_index;
  TRACE_FUNCTION( "grlc_handle_poll_pos" );
  
  while ( (grlc_data->poll_start_tbf NEQ 0xFF) AND
          (grlc_check_dist(current_fn,grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn,26)) AND 
          (current_fn NEQ grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn))
  {
    TRACE_EVENT_P4("current_fn= %ld  missed poll_fn=%ld  ps_tbf=%d  cnt=%d",
                                  current_fn,
                                  grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn,
                                  grlc_data->poll_start_tbf,
                                  grlc_data->next_poll_array[grlc_data->poll_start_tbf].cnt);
    /*
     * delete elememnt from tbf_list if all pollType = POLL_NONE, and add to free list
     */
    grlc_data->missed_poll_fn = grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn;
    grlc_data->next_poll_array[grlc_data->poll_start_tbf].cnt = 0;
    help_index = grlc_data->poll_start_tbf;
    grlc_data->poll_start_tbf = grlc_data->next_poll_array[grlc_data->poll_start_tbf].next;
    next_index = grlc_data->poll_start_free;
    while(grlc_data->next_poll_array[next_index].next NEQ 0xFF)
    {
       next_index = grlc_data->next_poll_array[next_index].next;
    }
    grlc_data->next_poll_array[next_index].next = help_index;
    grlc_data->next_poll_array[help_index  ].next = 0xFF;
    TRACE_ERROR( "Poll Position missed" );
  }
} /* grlc_handle_poll_pos() */


/*
+------------------------------------------------------------------------------
| Function    : grlc_send_rem_poll_pos
+------------------------------------------------------------------------------
| Description : The function grlc_send_rem_poll_pos()  
|                
| Parameters  : -
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_send_rem_poll_pos (ULONG current_fn)
{ 
  ULONG delta_fn = 0;
  UBYTE *ptr_block=NULL;

  
  TRACE_FUNCTION( "grlc_send_rem_poll_pos" );

  if (grlc_data->poll_start_tbf EQ 0xFF)
    return;



#ifdef _TARGET_      
  delta_fn = grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn;
  if(current_fn EQ FN_MAX-5)
    delta_fn += 5;
  else
    delta_fn -= current_fn;
#endif

#ifdef _SIMULATION_
  if(current_fn EQ grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn)        
    delta_fn    = 4;      
  else      
    delta_fn    = 0;      
#endif

  
  if( (delta_fn EQ 4) OR 
      (delta_fn EQ 5) )
  {
    UBYTE tn=0;
    UBYTE index;
    while(grlc_data->next_poll_array[grlc_data->poll_start_tbf].cnt
           AND (tn < 8))
    {
      switch(grlc_data->next_poll_array[grlc_data->poll_start_tbf].poll_type[tn])
      {
         case CGRLC_POLL_CTRL:
         case CGRLC_POLL_UACK:
         case CGRLC_POLL_COLLISION:
           if(grlc_data->burst_type EQ CGRLC_BURST_TYPE_NB)
           {
             ptr_block = grlc_set_packet_ctrl_ack();
             grlc_send_normal_burst(ptr_block, NULL, tn);
           }
           else
             grlc_send_access_burst(tn);
           break;
         case CGRLC_POLL_RES_AB:
           grlc_send_access_burst(tn);
           break;
         case CGRLC_POLL_RES_NB:
           ptr_block = grlc_set_packet_ctrl_ack();
           grlc_send_normal_burst(ptr_block, NULL, tn);
           break;
         case CGRLC_POLL_DATA:
           if( grlc_data->tbf_type EQ TBF_TYPE_UL )
           {
             /*
              * no downlink active , send ctrl block or pca 
              */
             if( tm_get_num_ctrl_blck( ) NEQ 0 )
             {
               ptr_block = tm_get_ctrl_blk( &index, TRUE );
               grlc_send_normal_burst(NULL, ptr_block, tn);
               grlc_data->rd.next_poll_block = NEXT_POLL_BLOCK_DL_DATA;
             }
             else if(grlc_data->burst_type EQ CGRLC_BURST_TYPE_NB)
             {
               ptr_block = grlc_set_packet_ctrl_ack();
               grlc_send_normal_burst(ptr_block, NULL, tn);
             }
             else
               grlc_send_access_burst(tn);
           }
           break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
         case CGRLC_POLL_RE_ASS: /* TBF re assignment on PACCH */
    
            if(grlc_data->burst_type EQ CGRLC_BURST_TYPE_NB)
            {
               T_U_GRLC_RESOURCE_REQ resource_req;
              
               tm_build_res_req( &resource_req,R_BUILD_2PHASE_ACCESS);
               grlc_send_normal_burst((UBYTE *)&resource_req, NULL, tn);
            }
            else /* PCA CTRL ACK 00 */
            {
               grlc_data->next_poll_array[grlc_data->poll_start_tbf].ctrl_ack = 0;
               grlc_send_access_burst(tn);
            }

           break;
#endif
      }
      tn++;
    }
    grlc_del_sent_poll();
  }  
}/* grlc_send_rem_poll_pos() */



/*
+------------------------------------------------------------------------------
| Function    : grlc_test_mode_active
+------------------------------------------------------------------------------
| Description : This functions returns 0 if the GPRS test mode is not activated. 
|               Otherwise a value greater then 0. 
|               
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/

GLOBAL UBYTE grlc_test_mode_active ()
{
  if(grlc_data->testmode.mode NEQ CGRLC_NO_TEST_MODE)
    return TRUE;
  else
    return FALSE;

}/* grlc_test_mode_active*/


/*
+------------------------------------------------------------------------------
| Function    : grlc_prbs
+------------------------------------------------------------------------------
| Description : 
|               This function generates a pseodo random bis sequence. 
|               The implementation is related to CCITT O.151 Okt. 92 chapter 2.1  .
|               
|               This functions generates the next length_i bytes of a 32767-bit 
|               pseudo-random test sequence if action_i is set to COMPUTE_DATA
|               and writes the data at address ptr_i.
|            
|               The function stores the position where it stops to calculate.
|               In case of action_i EQ INITIALIZE the function erase his history.
|               On its next call the function continues with its intial values
|               
| Parameters  : UBYTE action_i (INITIALIZE or COMPUTE_DATA)
|               and only
|               UBYTE length_i (number of data bytes which shall callculated and copied)
|               UBYTE * out_i  (location where this dada have to be placed, only valid)  
|
+------------------------------------------------------------------------------
*/

GLOBAL void grlc_prbs(UBYTE action_i, UBYTE length_i, UBYTE * ptr_i)
{
  TRACE_FUNCTION( "grlc_prbs" );

  switch(action_i)
  {
  case INITIALIZE:

#define FEED_BACK_MASK 0x6000    
    /* 
     * 14th and 15 stage will be feeded back 
     */


    /*
     * Initialize the prbs generation
     *
     * This value is the value of the shift register 8 cycles 
     * before shift register value 0x7fff
     */
    grlc_data->testmode.prbs_shift_reg = 0x55ff;
    break;
  case COMPUTE_DATA:
    {
      /*
       * get prbs computing values
       */
      USHORT i,reg = grlc_data->testmode.prbs_shift_reg;

      UBYTE  * out_ptr = ptr_i;

      for(i=0; i < (8 * length_i); i++)
      {
        /*
         * write a byte in output if neccessary
         */
        if(i % 8 EQ 0) 
        { 
          *out_ptr++ = (UBYTE)(0x00ff&reg);
        }
        
        
        {
          USHORT temp = reg & FEED_BACK_MASK;
          /*
           *  shift the register and but new data in
           */
          reg = reg << 1;
          if ( (temp EQ FEED_BACK_MASK) OR (temp EQ 0) )
          {
            /* 
             * put a "0" in 
             */;
          }
          else
          {
            /* 
             * put a "1" in
             */
            reg +=1;       
          }
        }   /* for(i=0; i < (8 * length_i); i++) */
      }      
      /*
       * store prbs computing values
       */
      grlc_data->testmode.prbs_shift_reg = reg;
    }
    break;

  default:
    break;
  }
  
  return;

}/*grlc_prbs*/
  







/*
+------------------------------------------------------------------------------
| Function    : grlc_trace_tbf_par
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_trace_tbf_par ( UBYTE tbf_index )
{ 
  TRACE_FUNCTION( "grlc_trace_tbf_par" );
 
  TRACE_EVENT_P9("tbf_type=%d;start_fn=%ld (%ld);end_fn=%ld(%ld);rlc_oct_cnt=%d; pdu_cnt=%ld;vs_vr=%d;va_vq=%d ",
                                                    grlc_data->tbf_ctrl[tbf_index].tbf_type,
                                                    grlc_data->tbf_ctrl[tbf_index].start_fn,
                                                    grlc_data->tbf_ctrl[tbf_index].start_fn%42432,
                                                    grlc_data->tbf_ctrl[tbf_index].end_fn,
                                                    grlc_data->tbf_ctrl[tbf_index].end_fn%42432,
                                                    grlc_data->tbf_ctrl[tbf_index].rlc_oct_cnt,
                                                    grlc_data->tbf_ctrl[tbf_index].pdu_cnt,
                                                    grlc_data->tbf_ctrl[tbf_index].vs_vr,
                                                    grlc_data->tbf_ctrl[tbf_index].va_vq);

  TRACE_EVENT_P9("cnt_ts=%d;ack_cnt=%d;fbi=%d;ret_bsn=%d;N_ACC=%d,tlli=%lx rem_ul_data=%d,PST=%d,PSF=%d",                                  
                                                    grlc_data->tbf_ctrl[tbf_index].cnt_ts,
                                                    grlc_data->tbf_ctrl[tbf_index].ack_cnt,
                                                    grlc_data->tbf_ctrl[tbf_index].fbi,
                                                    grlc_data->tbf_ctrl[tbf_index].ret_bsn,
                                                    grlc_data->tm.n_acc_req_procedures,
                                                    grlc_data->uplink_tbf.tlli,
                                                    grlc_data->grlc_data_req_cnt,
                                                    grlc_data->prim_start_tbf,
                                                    grlc_data->prim_start_free);

} /* grlc_trace_tbf_par */




/*
+------------------------------------------------------------------------------
| Function    : grlc_set_buf_tlli
+------------------------------------------------------------------------------
| Description : The function grlc_set_buf_tlli() fills the TLLI buffer.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_set_buf_tlli ( BUF_tlli_value *buf_tlli_o, ULONG tlli_i )
{
  TRACE_FUNCTION( "grlc_set_buf_tlli" );

  grlc_set_tlli( &buf_tlli_o->l_tlli_value, 
                &buf_tlli_o->o_tlli_value,
                &buf_tlli_o->b_tlli_value[0],
                tlli_i );

} /* grlc_set_buf_tlli */


/*
+------------------------------------------------------------------------------
| Function    : grlc_set_tlli
+------------------------------------------------------------------------------
| Description : The function grlc_set_tlli() fills the TLLI buffer.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_set_tlli
                ( USHORT *l_tlli, USHORT *o_tlli, UBYTE *b_tlli, ULONG tlli )
{
  TRACE_FUNCTION( "grlc_set_tlli" );

  *l_tlli   = 32;
  *o_tlli   = 0;
  
  b_tlli[0] = (UBYTE)((tlli >> 24) & 0x000000ff);
  b_tlli[1] = (UBYTE)((tlli >> 16) & 0x000000ff);
  b_tlli[2] = (UBYTE)((tlli >> 8 ) & 0x000000ff);
  b_tlli[3] = (UBYTE)((tlli      ) & 0x000000ff);

  /* unused byte must be set to 0x00, otherwise CCD has some problems */
  b_tlli[4] = 0;

} /* grlc_set_tlli */


/*
+------------------------------------------------------------------------------
| Function    : grlc_encode_ul_ctrl_block
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_encode_ul_ctrl_block ( UBYTE *ul_ctrl_block, 
                                       UBYTE *ul_ctrl_data )
{
  T_CTRLBUF enc_block;

  TRACE_FUNCTION( "grlc_encode_ul_ctrl_block" );

  grlc_encode_ctrl( ul_ctrl_data, ( T_MSGBUF* )&enc_block , grlc_data->r_bit );
  memcpy( ul_ctrl_block, enc_block.buf, BYTELEN( enc_block.l_buf ) );    

} /* grlc_encode_ul_ctrl_block */






/*
+------------------------------------------------------------------------------
| Function    : grlc_check_if_tbf_start_is_elapsed
+------------------------------------------------------------------------------
| Description : The function grlc_check_if_tbf_start_is_elapsed() checks if 
|               tbf starting time is elapsed or not, modulo calculation is 
|               needed
| Parameters  : start_fn    - tbf starting time
|               current_fn  - current frame number
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grlc_check_if_tbf_start_is_elapsed ( ULONG start_fn, ULONG current_fn)
{
  BOOL  result = FALSE;
  ULONG d1;
  /* ULONG d2; */
  /* FN_MAX=0x297000 == 2715648 ==125463 seconds(4.62ms per frame)
   * the starting time is within  current_fn-10808 and current_fn+31623
   * modulo operation must be taken in account
   */
  TRACE_FUNCTION( "grlc_check_if_tbf_start_is_elapsed" );

  /*
   * handle maximum distance for tbf starting time
   */

  if(start_fn EQ CGRLC_STARTING_TIME_NOT_PRESENT)  /*lint !e650*/
  {
    result =TRUE;
    return result;
  }
  d1 = 10808;
  /* d2 = 31623; */
  if( (start_fn <= current_fn)     AND
      ((current_fn-start_fn) <= d1))
  {
    result = TRUE;
    /*TRACE_EVENT_P2(" case 1: st time elapsed st_fn=%ld  c_fn=%ld",start_fn,current_fn);*/
  }
  else if((start_fn >= current_fn) AND
          (FN_MAX-start_fn+current_fn) <= d1)
  {
    result = TRUE;
   /* TRACE_EVENT_P2("case 2: st time elapsed st_fn=%ld  c_fn=%ld",start_fn,current_fn);*/
  }
/*  else
  {
    TRACE_EVENT_P2("case 3: WAIT FOR ST TIME st_fn=%ld  c_fn=%ld",start_fn,current_fn);
  }
*/    

  return result;
} /* grlc_check_if_tbf_start_is_elapsed() */



/*
+------------------------------------------------------------------------------
| Function    : grlc_decode_grlc
+------------------------------------------------------------------------------
| Description : The function grlc_decode_grlc() calls the function ccd_decodeMsg.
|               After the call the decoded Message is in _decodeCtrlMsg.
|
| Parameters  : msg_ptr_i - pointer to buffer that should be decoded
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE grlc_decode_grlc (T_MSGBUF  *  msg_ptr_i)
{ 
  UBYTE result;
  UBYTE msg_type = msg_ptr_i->buf[0] >> 2;;

  TRACE_FUNCTION( "grlc_decode_grlc" );

  /*
   * Offset must be zero, else code to get msg_type is illegal
   */
  TRACE_ASSERT (msg_ptr_i->o_buf==0);



  switch (msg_type)
  {
    case D_UL_ACK:
      if(!grlc_data->grlc_wo_ccd)
      {
        result = ccd_decodeMsg (CCDENT_GRLC, DOWNLINK, msg_ptr_i, _decodedMsg, NOT_PRESENT_8BIT);
      }
      else
      {
        result = grlc_decode_ul_acknack(msg_ptr_i->buf);
      }
      break;
    default:
      TRACE_ERROR(" not Packet ul ack decoded: should nor happen ");
      result = DELETE_MESSAGE;
      break;
  }


  if ( result EQ ccdError )
  {
    return grlc_ccd_error_handling( CCDENT_GRLC );
  }

  return msg_type;

} /* grlc_decode_grlc() */



/*
+------------------------------------------------------------------------------
| Function    : grlc_activate_tfi
+------------------------------------------------------------------------------
| Description : 
|              grlc_activate_tfi modifies tfi´s if tbf starting time is reached
|
|              
| Parameters  : fn_i is the current framenuber in tbf mode
|
+------------------------------------------------------------------------------
*/          

GLOBAL void grlc_activate_tfi (ULONG fn_i)
{

  TRACE_FUNCTION( "grlc_activate_tfi" );


  /* 1. check if starting time is reached */
  switch(grlc_data->tfi_change)
  {
    case TFI_CHANGE_UL:
      if(grlc_check_if_tbf_start_is_elapsed ( grlc_data->ul_tbf_start_time, ((fn_i+5)%FN_MAX)))
      {
        grlc_data->tfi_change      = TFI_CHANGE_NULL;
        grlc_data->ul_tfi          = grlc_data->start_fn_ul_tfi;
        grlc_data->start_fn_ul_tfi = 0xFF;        
        TRACE_EVENT_P2("UL TFI CHANGE St reached st_fn=%ld c_fn=%ld",grlc_data->ul_tbf_start_time,fn_i );
        grlc_data->ul_tbf_start_time = CGRLC_STARTING_TIME_NOT_PRESENT;
        grlc_data->tbf_ctrl[grlc_data->ul_index].tfi = grlc_data->ul_tfi;
        grlc_data->ul_tfi_changed  = TRUE;
      }
      break;
    case TFI_CHANGE_DL:
      if(grlc_check_if_tbf_start_is_elapsed ( grlc_data->dl_tbf_start_time, fn_i))
      {
        grlc_data->tfi_change      = TFI_CHANGE_NULL;
        grlc_data->dl_tfi          = grlc_data->start_fn_dl_tfi;
        grlc_data->start_fn_dl_tfi = 0xFF;
        TRACE_EVENT_P2("DL TFI CHANGE St reached st_fn=%ld c_fn=%ld",grlc_data->dl_tbf_start_time,fn_i );
        grlc_data->dl_tbf_start_time = CGRLC_STARTING_TIME_NOT_PRESENT;
        grlc_data->tbf_ctrl[grlc_data->dl_index].tfi = grlc_data->dl_tfi;
      }
      break;
    case TFI_CHANGE_ALL:
      if(grlc_check_if_tbf_start_is_elapsed ( grlc_data->ul_tbf_start_time, ((fn_i+5)%FN_MAX)))
      {
        grlc_data->tfi_change      = TFI_CHANGE_DL;
        grlc_data->ul_tfi          = grlc_data->start_fn_ul_tfi;
        grlc_data->start_fn_ul_tfi = 0xFF;
        TRACE_EVENT_P2("UL TFI CHANGE(ALL) St reached st_fn=%ld c_fn=%ld",grlc_data->ul_tbf_start_time,fn_i );
        grlc_data->ul_tbf_start_time = CGRLC_STARTING_TIME_NOT_PRESENT;
        grlc_data->tbf_ctrl[grlc_data->ul_index].tfi = grlc_data->ul_tfi;
        grlc_data->tbf_ctrl[grlc_data->dl_index].tfi = grlc_data->dl_tfi;
        grlc_data->ul_tfi_changed  = TRUE;
      }
      if(grlc_check_if_tbf_start_is_elapsed ( grlc_data->dl_tbf_start_time, fn_i))
      {
        if (grlc_data->tfi_change EQ TFI_CHANGE_DL)
          grlc_data->tfi_change      = TFI_CHANGE_NULL;
        else
          grlc_data->tfi_change      = TFI_CHANGE_UL;
        grlc_data->dl_tfi          = grlc_data->start_fn_dl_tfi;
        grlc_data->start_fn_dl_tfi = 0xFF;
        TRACE_EVENT_P2("DL TFI CHANGE(ALL) St reached st_fn=%ld c_fn=%ld",grlc_data->dl_tbf_start_time,fn_i );
        grlc_data->dl_tbf_start_time = CGRLC_STARTING_TIME_NOT_PRESENT;
      }
      break;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : grlc_t_status
+------------------------------------------------------------------------------
| Description : This function returns the remaining time in milliseconds.
|               A value of 0L is returned in case the timer is not existing or
|               not running.
|
| Parameters  : t_index : timer index
|
+------------------------------------------------------------------------------
*/
GLOBAL T_TIME grlc_t_status( USHORT t_index )
{
  T_TIME t_time = 0L;
  
  TRACE_FUNCTION( "grlc_t_status" );

  vsi_t_status( GRLC_handle, t_index, &t_time );

  return( t_time );
} /* grlc_t_status */ 

/*
+------------------------------------------------------------------------------
| Function    : grlc_enter_standby_state
+------------------------------------------------------------------------------
| Description : This function is called in case the STANDBY state should be
|               entered.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_enter_standby_state ( void )
{
  TRACE_FUNCTION( "grlc_enter_standby_state" );

  if( grlc_data->ready_timer.handling EQ READY_TIMER_HANDLING_ENABLED )
  {
    if( grlc_data->ready_timer.state EQ READY_STATE )
    {
      /* The ready_timer.state is set to STANDBY_STATE just in case we are not already in this state.
         The CGRLC_STANDBY_STATE_IND primitives are sent if we move from READY_STATE to STANDBY_STATE only.  
      */
      if( grlc_data->ready_timer.value NEQ CGRLC_DEACTIVATED )
      {
        /* If the timer T3314 expires while we are in READY_STATE but the timer is deactivated then
           the transition to STANDBY_STATE will never occur
        */
        /* If the primitive CGRLC_FORCE_TO_STANDBY_REQ is received when we are in Ready State but
           the timer is deactivated then the transition to STANDBY_STATE will never occur
        */
        grlc_data->ready_timer.state = STANDBY_STATE;
        vsi_t_stop( GRLC_handle, T3314 );

        {
          PALLOC(cgrlc_standby_state_ind,CGRLC_STANDBY_STATE_IND); /* T_CGRLC_STANDBY_STATE_IND sent to GMM */
          PSEND(hCommGMM,cgrlc_standby_state_ind);
        }

        {
          PALLOC(cgrlc_standby_state_ind,CGRLC_STANDBY_STATE_IND); /* T_CGRLC_STANDBY_STATE_IND sent to GRR */
          PSEND(hCommGRR,cgrlc_standby_state_ind);
        }
      }
      else
      {
        TRACE_EVENT( "grlc_enter_standby_state: MS enters STANDBY state while T3314 is deactivated" );
      }
    }
    else
    {
      TRACE_EVENT( "grlc_enter_standby_state: MS is already in STANDBY state" );
    }
  }
} /* grlc_enter_standby_state */ 

/*
+------------------------------------------------------------------------------
| Function    : grlc_enter_ready_state
+------------------------------------------------------------------------------
| Description : This function is called in case the READY state should be
|               entered.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_enter_ready_state ( void )
{
  TRACE_FUNCTION( "grlc_enter_ready_state" );

  if (grlc_data->ready_timer.state EQ STANDBY_STATE)
  {
    /* The ready_timer.state is set to READY_STATE just in case we are not already in this state.
       The CGRLC_READY_STATE_IND primitives are sent only if we move from STANDBY_STATE to READY_STATE */

    /* If we receive the primitive CGRLC_READY_TIMER_CONFIG_REQ with timer value CGRLC_DEACTIVATED when 
       we are in CGRLC_STANDBY, the state will immediately switch to READY and the CGRLC_READY_STATE_IND
       primitives will be sent */
    grlc_data->ready_timer.state = READY_STATE;      

    {
      PALLOC(cgrlc_ready_state_ind,CGRLC_READY_STATE_IND); /* T_CGRLC_READY_STATE_IND  sent to GMM */
      PSEND(hCommGMM,cgrlc_ready_state_ind);
    }

    {
      PALLOC(cgrlc_ready_state_ind,CGRLC_READY_STATE_IND); /* T_CGRLC_READY_STATE_IND  sent to GRR */
      PSEND(hCommGRR,cgrlc_ready_state_ind);
    }
  }
  else
  {
    TRACE_EVENT( "grlc_enter_ready_state: MS is already in READY state" );
  }
} /* grlc_enter_ready_state */ 
/*
+------------------------------------------------------------------------------
| Function    : grlc_convert_11bit_2_etsi
+------------------------------------------------------------------------------
| Description : Converts the 11 bit access burst value into ETSI format
|
| Parameters  : In:  eleven bit value
|               Out: converted eleven bit
|
+------------------------------------------------------------------------------
*/
LOCAL USHORT grlc_convert_11bit_2_etsi ( USHORT eleven_bit )
{
  USHORT etsi11bit;
  USHORT dummy1 = 0, dummy2 = 0;

  TRACE_FUNCTION( "grlc_convert_11bit_2_etsi" );

  /*
   *  11 Bit access burst
   * b: bit
   * b10 b9 b8 b7 b6 b5 b4 b3 b2 b1 b0
   * should be sent to the network -according 04.60 and 0404- in the
   * following 16-bit format:
   * 0 0 0 0 0 b2 b1 b0 b10 b9 b8 b7 b6 b5 b4 b3
   */

  /*
   * get b2 b1 b0
   */
  dummy1 = 0x0007 & eleven_bit;

  /*
   * shift it 8 bits to left
   */
  dummy1 = ( dummy1 << 8 );

  /*
   * get b10 b9 b8 b7 b6 b5 b4 b3
   */
  dummy2 = 0xFFF8 & eleven_bit;

  /*
   * shift it 3 bits to right
   */
  dummy2 = ( dummy2 >> 3 );

  /*
   * compose dummy1 and dummy2 to the target 16-bit format
   */
  etsi11bit = dummy1 | dummy2;
  
  return etsi11bit;

} /* grlc_convert_11bit_2_etsi() */ 
