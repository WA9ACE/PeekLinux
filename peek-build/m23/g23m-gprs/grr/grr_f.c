/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRR
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
| Purpose:     This module implements global functions for GRR
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_F_C
#define GRR_F_C
#endif

#define ENTITY_GRR

#ifdef _SIMULATION_

/*
 * Report warning 4005 as an error.
 * 
 * There are identical macro definitons in the GRR message and the RRGRR SAP 
 * document which should be aligned at all the time
 * (e.g. GPRS_RXLEV_ACCESS_MIN_INVALID, GPRS_MS_TXPWR_MAX_CCH, etc.)
 */
#pragma warning( error : 4005 )

#endif /* #ifdef _SIMULATION_ */

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
#include "cnf_grr.h"    /* to get cnf-definitions                            */
#include "mon_grr.h"    /* to get mon-definitions                            */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"    /* to get message describtion                        */
#include "pcm.h"

#include "grr.h"        /* to get the global entity definitions              */

#include "grr_f.h"      /* to check own definitions         */
#include "grr_ctrlf.h"  /* to get definition of ctrl_init() */
#include "grr_tcf.h"    /* to get definition of tc_init()   */
#include "grr_cpapf.h"  /* to get definition of cpap_init() */
#include "grr_psif.h"   /* to get definition of psi_init()  */
#include "grr_pgf.h"    /* to get definition of pg_init()   */
#include "grr_gfpf.h"   /* to get definition of gfp_init()  */
#include "grr_meass.h"  /* to get definition of meas_init() */
#include "grr_csf.h"    /* to get definition of cs_init()   */
#include "grr_tcs.h"
#include "grr_em.h"     /*for Engineering mode*/ 

/*==== CONST ================================================================*/
/*
 * Mask- and shift tables for use with macro GET_N_BITS(N)
 */
static const 
UBYTE maskTab1[] = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF };

static const 
UBYTE maskTab2[] = { 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01 };

static const 
UBYTE shiftTab[] = { 0x00, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
/*==== DIAGNOSTICS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/

static UBYTE* pBuf;     /* Global Var: pointer to air message data buffer    */
static int    startBit; /* Global Var: current pBuf decode offset            */
static int    bitLen;   /* Global Var: current remaining undecoded pbuf bits */
LOCAL T_LIST _local_dummy_list;
LOCAL T_f_range _local_rfl_contents;
/*==== GLOBAL VARS ===========================================================*/

/*==== LOCAL MACROS =========================================================*/
/*
 * GET_N_BITS reads next N (1..8) bits from UBYTE* pBuf, starting from integer
 * startBit. N is added to startBit and removed from bitLen after call to this 
 * macro.
 */
#define GET_N_BITS(N)                                                         \
          (( (startBit%8) + (N) <= 8)                                         \
           ? (pBuf[startBit/8] >> shiftTab[(startBit+(N))%8]) & maskTab1[N]   \
           : (pBuf[(startBit+(N))/8] >> shiftTab[(startBit+(N))%8]) +         \
           ((pBuf[startBit/8] & maskTab2[startBit%8]) << ((startBit+(N))%8))  \
          ); startBit += (N); bitLen -= (N)


/*==== FUNCTIONS PROTOTYPES =================================================*/
LOCAL BOOL grr_check_request_reference ( T_req_ref_p * req_ref_i );
LOCAL BOOL grr_check_glob_tfi ( T_glob_tfi * glob_tfi_i, UBYTE tn );
LOCAL BOOL grr_check_add_reject ( UBYTE tn );
LOCAL BOOL grr_check_add_1 ( T_add1 * add1_i,UBYTE tn );
LOCAL BOOL grr_check_add_2 ( T_add2 * add2_i,UBYTE tn );
LOCAL BOOL grr_check_add_3 ( T_add3 * add3_i,UBYTE tn );
LOCAL BOOL grr_check_add_4 ( T_add4 * add4_i,UBYTE tn );
LOCAL void grr_clean_up_seg_ctrl_blk_tbl ( void );
LOCAL void grr_align_seg_ctrl_blk_nxt ( void );

LOCAL UBYTE grr_decode_ie_tlli (BUF_tlli_value* tlli);
LOCAL UBYTE grr_decode_ie_pta (T_pta* pta);
LOCAL UBYTE grr_decode_ie_pwr_par (T_pwr_par* pwr_par);
LOCAL UBYTE grr_decode_ie_glob_tfi (T_glob_tfi* glob_tfi);
LOCAL UBYTE grr_decode_ie_meas_map (T_meas_map* meas_map);
LOCAL UBYTE grr_decode_ie_tbf_s_time (T_tbf_s_time* tbf_s_time);
LOCAL UBYTE grr_decode_ie_dyn_alloc_p (T_dyn_alloc_p* dyn_alloc_p);
LOCAL UBYTE grr_decode_ie_dyn_alloc_ts (T_dyn_alloc_ts* dyn_alloc_ts);
LOCAL UBYTE grr_decode_ie_freq_par (T_freq_par* freq_par);

LOCAL UBYTE grr_decode_dl_assignment (UBYTE* buf, int off, int len);
LOCAL UBYTE grr_decode_ul_assignment (UBYTE* buf, int off, int len);
LOCAL UBYTE grr_decode_ts_reconfig (UBYTE* buf, int off, int len);

LOCAL UBYTE grr_decode_pdch_release (UBYTE* buf, int off, int len);
LOCAL UBYTE grr_decode_polling_req (UBYTE* buf, int off, int len);
LOCAL UBYTE grr_decode_tbf_release_req (UBYTE* buf, int off, int len);

#ifdef REL99
LOCAL UBYTE grr_decode_ie_egprs_link_adpt_para(T_egprs_link_adpt_para *trgt);
LOCAL UBYTE  grr_decode_ie_compact_red_ma(T_compact_red_ma *trgt);
#endif

LOCAL void  grr_init_ms_data     ( void                                 );

/*
 * transmit power control 
 */
LOCAL void  grr_set_alpha_flags  ( BOOL              v_alpha,
                                   UBYTE             alpha              );

LOCAL void  grr_set_sngl_gamma   ( UBYTE             gamma,
                                   UBYTE             tn                 );

/*
 * NC and extended measurements
 */
LOCAL void  grr_init_rfreq_list  ( T_NC_RFREQ_LIST  *list               );

LOCAL void  grr_init_ncmeas_struct
                                 ( T_NCMEAS         *ncmeas,
                                   BOOL              is_cw              );

LOCAL void  grr_copy_em1_struct  ( T_XMEAS_EM1      *db_em1,
                                   T_em1            *ext_em1,
                                   BOOL              cpy_prm_set,
                                   UBYTE            *start_ext_lst_idx,
                                   UBYTE            *stop_ext_lst_idx   );

/*
 * database management
 */
LOCAL void  grr_init_db_srvc_param ( T_SC_DATABASE *db      );

LOCAL UBYTE grr_get_db_num         ( T_SC_DATABASE *db      );

/*
 * miscellaneous
 */
LOCAL void  grr_mrk_ext_lst_freq   ( T_EXT_FREQ_LIST *list  );

LOCAL UBYTE grr_ccd_error_handling ( UBYTE    entity_i      );

LOCAL void  grr_get_si_cell_alloc_list  ( T_LIST     *list  );

LOCAL BOOL  grr_get_psi_cell_alloc_list ( T_LIST     *list  );

LOCAL BOOL  grr_get_ms_alloc_list       ( T_LIST             *list,
                                          const T_gprs_ms_alloc_ie *ms_alloc );
/*==== FUNCTIONS ============================================================*/

/*
+------------------------------------------------------------------------------
| Function    : grr_check_request_reference
+------------------------------------------------------------------------------
| Description : The function grr_check_request_reference() compares the addresses
|               in req_ref_i with the entity data ent returns TRUE if the address
|               in req_ref_i is valid otherwise FALSE
|
| Parameters  : tlli_i - ptr to tlli buffer
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL grr_check_request_reference ( T_req_ref_p * req_ref_i )
{ 
  BOOL result = FALSE;
  UBYTE i,t1,t2,t3;
  USHORT acc_inf;

  t1 = req_ref_i->fn_mod.t1;
  t2 = req_ref_i->fn_mod.t2;
  t3 = req_ref_i->fn_mod.t3;
  acc_inf = req_ref_i->access_info;

  TRACE_FUNCTION( "grr_check_request_reference" );

  for(i=0;i<3;i++)
  {
    if( 
        (grr_data->req_ref[i].fn_mod.t1 EQ t1 )
        AND
        (grr_data->req_ref[i].fn_mod.t2 EQ t2 )
        AND
        (grr_data->req_ref[i].fn_mod.t3 EQ t3 )
        AND
        (grr_data->req_ref[i].access_info EQ acc_inf )
      )
    {
      result = TRUE;
      break;
    }
  }

  if(!result)
  {
    TRACE_EVENT("grr_check_request_reference failed");
  }

  return(result);
} /* grr_check_request_reference() */






/*
+------------------------------------------------------------------------------
| Function    : grr_check_glob_tfi();
+------------------------------------------------------------------------------
| Description : The function grr_check_glob_tfi() returns true if the TFI 
|               is correct.
|
| Parameters  : glob_tfi_i - pointer to global TFI structure
|               tn         - timeslot on which the message is received, this is
|                            required, because tfi is valid only for one 
|                            direction and the assigned PDCHs of a TBF(4.60 5.2)
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL grr_check_glob_tfi ( T_glob_tfi * glob_tfi_i, UBYTE tn )
{ 
  
  BOOL result = FALSE;
  TRACE_FUNCTION( "grr_check_glob_tfi" );


  if(glob_tfi_i->v_ul_tfi)
  {
    if((glob_tfi_i->ul_tfi EQ grr_data->uplink_tbf.tfi) AND
      ((0x80>>tn) & grr_data->uplink_tbf.ts_mask))
    {
      result = TRUE;
    }
  }
  else if(glob_tfi_i->v_dl_tfi)
  {
    if((glob_tfi_i->dl_tfi EQ grr_data->downlink_tbf.tfi) AND
      ((0x80>>tn) & grr_data->downlink_tbf.ts_mask))
    {
      result = TRUE;
    }
  }

  if(!result)
  {
    if(glob_tfi_i->v_ul_tfi)
    {
	  TRACE_FUNCTION("grr_check_glob_tfi failed: UL addressed");
#if 0
      TRACE_EVENT_P7("grr_check_glob_tfi failed: UL addressed tfi=%d, ul_tfi=%d, dl_tfi=%d || tn=%d ->mask=%x curr_mask=%x, st_mask=%x ",
                                        glob_tfi_i->ul_tfi,
                                        grr_data->uplink_tbf.tfi,
                                        grr_data->downlink_tbf.tfi,
                                        tn,
                                        0x80>>tn,
                                        grr_data->uplink_tbf.ts_mask,
                                        grr_data->uplink_tbf.ts_usage);
#endif
    }
    else if(glob_tfi_i->v_dl_tfi)
    {
	TRACE_FUNCTION("grr_check_glob_tfi failed: DL addressed");
#if 0
      TRACE_EVENT_P7("grr_check_glob_tfi failed: DL addressed tfi=%d, ul_tfi=%d, dl_tfi=%d|| tn=%d ->mask=%x curr_mask=%x, st_mask=%x ",
                                        glob_tfi_i->dl_tfi,
                                        grr_data->uplink_tbf.tfi,
                                        grr_data->downlink_tbf.tfi,
                                        tn,
                                        0x80>>tn,
                                        grr_data->downlink_tbf.ts_mask,
                                        grr_data->downlink_tbf.ts_usage);
#endif

    }
    else
    {
      TRACE_EVENT("grr_check_glob_tfi failed: NO addressed tfi ");
    }
  }

  return(result);
  
} /* grr_check_glob_tfi() */



/*
+------------------------------------------------------------------------------
| Function    : grr_check_add_reject
+------------------------------------------------------------------------------
| Description : The function grr_check_add_reject() checks the address in
|               addtional rejects in a Packet Access Reject.
|
| Parameters  : tn - timeslot: need to check if tfi is on assigned PDCH
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL grr_check_add_reject ( UBYTE tn )
{ 
  MCAST(access_rej,D_ACCESS_REJ);
  BOOL result = FALSE;

  TRACE_FUNCTION( "grr_check_add_reject" );

  if(access_rej->reject.v_tlli_value)
    result = grr_check_all_tlli(& access_rej->reject.tlli_value);
  else if (access_rej->reject.req_ref_tfi.v_req_ref_p)
    result = grr_check_request_reference( & access_rej->reject.req_ref_tfi.req_ref_p);
  else if (access_rej->reject.req_ref_tfi.v_glob_tfi)
    result = grr_check_glob_tfi(& access_rej->reject.req_ref_tfi.glob_tfi,tn);

  /* SZML-SGLBL/004 */

  return(result); 
  
} /* grr_check_add_reject() */



/*
+------------------------------------------------------------------------------
| Function    : grr_check_add_1
+------------------------------------------------------------------------------
| Description : The function grr_check_add_1() compares address (add1).
|
| Parameters  : add1_i - pointer to address structure to check
|               tn     - timeslot: need to check if tfi is on assigned PDCH
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL grr_check_add_1 ( T_add1 * add1_i,UBYTE tn )
{ 

  BOOL result = FALSE;
  TRACE_FUNCTION( "grr_check_add_1" );

  if(add1_i->v_glob_tfi) 
    result = grr_check_glob_tfi( & add1_i->glob_tfi,tn );
  else if (add1_i->v_tlli_value) 
    result = grr_check_all_tlli(& add1_i->tlli_value);

  return(result); 
  
} /* grr_check_add_1() */



/*
+------------------------------------------------------------------------------
| Function    : grr_check_add_2
+------------------------------------------------------------------------------
| Description : The function grr_check_add_2() checks address structure 2(add2).
|
| Parameters  : add2_i - pointer address structure to check
|               tn     - timeslot: need to check if tfi is on assigned PDCH
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL grr_check_add_2 ( T_add2 * add2_i,UBYTE tn )
{ 

  BOOL result = FALSE;
  TRACE_FUNCTION( "grr_check_add_2" );

  if(add2_i->v_glob_tfi) 
    result = grr_check_glob_tfi( & add2_i->glob_tfi,tn );
  else if (add2_i->v_tlli_value) 
    result = grr_check_all_tlli(& add2_i->tlli_value);
  else if (add2_i->v_tqi)
    result = (add2_i->tqi EQ grr_data->tqi);
  
  return(result); 
  
} /* grr_check_add_2() */


/*
+------------------------------------------------------------------------------
| Function    : grr_check_add_3
+------------------------------------------------------------------------------
| Description : The function grr_check_add_3() checks address structure 3(add3). 
|
| Parameters  : add3_i - address of address structure to check
|               tn     - timeslot: need to check if tfi is on assigned PDCH
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL grr_check_add_3 ( T_add3 * add3_i,UBYTE tn )
{ 
  BOOL result = FALSE;

  TRACE_FUNCTION( "grr_check_add_3" );

  if(add3_i->v_glob_tfi) 
    result = grr_check_glob_tfi( & add3_i->glob_tfi,tn );
  else if (add3_i->v_tlli_value) 
    result = grr_check_all_tlli(& add3_i->tlli_value);
  else if (add3_i->v_tqi)
    result = (add3_i->tqi EQ grr_data->tqi);
  else if (add3_i->v_req_ref_p)
    result = grr_check_request_reference( & add3_i->req_ref_p);

  return(result); 
  
} /* grr_check_add_3() */


/* The following function is added for handling address 4*/
/*
+------------------------------------------------------------------------------
| Function    : grr_check_add_4
+------------------------------------------------------------------------------
| Description : The function grr_check_add_4() checks address structure 4(add4).
|
| Parameters  : add4_i - address of address structure to check
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL grr_check_add_4 ( T_add4 * add4_i,UBYTE tn  )
{
  BOOL result = FALSE;

  TRACE_FUNCTION( "grr_check_add_4" );

  if(add4_i->v_glob_tfi)
    result = grr_check_glob_tfi( & add4_i->glob_tfi,tn );
  else if (add4_i->tqi_req_ref_p.v_tqi)
    result = (add4_i->tqi_req_ref_p.tqi EQ grr_data->tqi);
  else if (add4_i->tqi_req_ref_p.v_req_ref_p)
    result = grr_check_request_reference( & add4_i->tqi_req_ref_p.req_ref_p);

  return(result);

} /* grr_check_add_4() */

/*
+------------------------------------------------------------------------------
| Function    : grr_clean_up_seg_ctrl_blk_tbl
+------------------------------------------------------------------------------
| Description : The function grr_clean_up_seg_ctrl_blk_tbl() checks the table 
|               for old entries to be 'deleted'.
|
| Parameters  : no
|
+------------------------------------------------------------------------------
*/
LOCAL void grr_clean_up_seg_ctrl_blk_tbl ( void )
{ 
  ULONG fn_i,T3200_i;
  UBYTE i;
  TRACE_FUNCTION( "grr_clean_up_seg_ctrl_blk_tbl" );
  for(i = 0;i < SEG_CTRL_BLOCK_SIZE;i++)
  {
    if(0xFF NEQ grr_data->seg_ctrl_blk.blk[i].rti)
    {
      fn_i = grr_data->dl_fn;
      T3200_i = grr_data->seg_ctrl_blk.blk[i].T3200;
      if((grr_data->seg_ctrl_blk.blk[i].T3200 < 60000) AND (fn_i > (FN_MAX - 60000)))
      {
        T3200_i += FN_MAX;
      }
      else if((grr_data->seg_ctrl_blk.blk[i].T3200 > (FN_MAX - 60000)) AND (fn_i < 60000))
      {
        fn_i += FN_MAX;
      }
      if((T3200_i < fn_i) OR (T3200_i > (fn_i + 60000)))
      {
        grr_data->seg_ctrl_blk.blk[i].rti = 0xFF;
      }
    }
  }
} /* grr_clean_up_seg_ctrl_blk_tbl() */


/*
+------------------------------------------------------------------------------
| Function    : grr_align_seg_ctrl_blk_nxt
+------------------------------------------------------------------------------
| Description : The function grr_align_seg_ctrl_blk_nxt() checks the table for
|               the next field to use and sets grr_data->seg_ctrl_blk.next.
|
| Parameters  : no
|
+------------------------------------------------------------------------------
*/
LOCAL void grr_align_seg_ctrl_blk_nxt ( void )
{ 
  ULONG T3200_i1,T3200_i2;
  UBYTE i,n;

  TRACE_FUNCTION( "grr_align_seg_ctrl_blk_nxt" );
  grr_data->seg_ctrl_blk.next++;
  grr_data->seg_ctrl_blk.next %= SEG_CTRL_BLOCK_SIZE;
  if(0xFF NEQ grr_data->seg_ctrl_blk.blk[grr_data->seg_ctrl_blk.next].rti)
  {
    for(i = 0;i < SEG_CTRL_BLOCK_SIZE;i++)
    { /* is there an empty field */
      if(0xFF EQ grr_data->seg_ctrl_blk.blk[i].rti)
        break;
    }
    if(SEG_CTRL_BLOCK_SIZE > i)
    { /* there is an empty field */
      grr_data->seg_ctrl_blk.next = i;
    }
    else
    {/* there is no empty field -> find the oldest entry */
      for(i = 0,n = 0;i < SEG_CTRL_BLOCK_SIZE;i++)
      {
        T3200_i1 = grr_data->seg_ctrl_blk.blk[i].T3200;
        T3200_i2 = grr_data->seg_ctrl_blk.blk[n].T3200;
        if((T3200_i1 > (FN_MAX - 60000)) AND (T3200_i2 < 60000))
        {
          T3200_i2 += FN_MAX;
        }
        else if((T3200_i2 > (FN_MAX - 60000)) AND (T3200_i1 < 60000))
        {
          T3200_i1 += FN_MAX;
        }
        if(T3200_i1 < T3200_i2)
        {
          n = i;
        }
      }
      grr_data->seg_ctrl_blk.next = n;
    }
  }
} /* grr_align_seg_ctrl_blk_nxt() */

/*
+------------------------------------------------------------------------------
| Function    : grr_decode_ie_tlli
+------------------------------------------------------------------------------
| Description : This function decodes a compressed TLLI struct
|
| Parameters  : tlli     - BUF_tlli struct destination pointer
|
| Globals:      pBuf     - Ptr to the compressed TLLI buffer
|               startBit - Bit offset of the first TLLI bit
|               bitLen   - Lenght of the buffer in bits
|
| Returns     : ccdOK    - If no error is occured
|                        - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_ie_tlli (BUF_tlli_value* tlli)
{
  if (bitLen < 32)
    return ERR_MAND_ELEM_MISS;

  tlli->l_tlli_value = 32;
  tlli->o_tlli_value = startBit%8;

  tlli->b_tlli_value[0] = pBuf[startBit/8] & maskTab2[tlli->o_tlli_value];
  startBit += 8;

  tlli->b_tlli_value[1] = pBuf[startBit/8];
  startBit += 8;

  tlli->b_tlli_value[2] = pBuf[startBit/8];
  startBit += 8;

  tlli->b_tlli_value[3] = pBuf[startBit/8];
  startBit += 8;

  tlli->b_tlli_value[4] = pBuf[startBit/8] & ~(maskTab2[tlli->o_tlli_value]);

  bitLen   -= 32;

  return ccdOK;
}


/*
+------------------------------------------------------------------------------
| Function    : grr_decode_ie_pta
+------------------------------------------------------------------------------
| Description : This function decodes a compressed T_pta struct
|
| Parameters  : pta      - T_pta struct destination pointer
|
| Globals:      pBuf     - Ptr to the compressed T_pta buffer
|               startBit - Bit offset of the first T_pta bit
|               bitLen   - Lenght of the buffer in bits
|
| Returns     : ccdOK    - If no error is occured
|                        - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_ie_pta (T_pta* pta)
{
  int bit;                                    

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  bit = GET_N_BITS(1);                        
  if (bit == 1)                               /* TIMING_ADVANCED_VALUE:bit(6) */
  {
    if (bitLen < 6)
      return ERR_MAND_ELEM_MISS;              

    pta->v_ta_value = TRUE;                   
    pta->ta_value   = GET_N_BITS(6);
  }

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  pta->v_ta_index_tn = GET_N_BITS(1);
  if (pta->v_ta_index_tn == 1)                         /* TIMING_ADVANCED_INDEX:bit(4) */
  {                                           /* TI.A.-TIMESLOT_NUMBER:bit(3) */
    if (bitLen < 7)
      return ERR_MAND_ELEM_MISS;              

    pta->ta_index_tn.ta_index   = GET_N_BITS(4);
    pta->ta_index_tn.ta_tn      = GET_N_BITS(3);
  }

  return ccdOK;
}


/*
+------------------------------------------------------------------------------
| Function    : grr_decode_ie_pwr_par
+------------------------------------------------------------------------------
| Description : This function decodes a compressed T_pwr_par struct
|
| Parameters  : pwr_par      - T_pwr_par struct destination pointer
|
| Globals:      pBuf     - Ptr to the compressed T_pwr_par buffer
|               startBit - Bit offset of the first T_pwr_par bit
|               bitLen   - Lenght of the buffer in bits
|
| Returns     : ccdOK    - If no error is occured
|                        - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_ie_pwr_par (T_pwr_par* pwr_par)
{
  int i;
  int bit;                                    
                                              /* Power Control Parameters    */
  if (bitLen < 4)
    return ERR_MAND_ELEM_MISS;              

  pwr_par->alpha = GET_N_BITS(4);

  for (i=0; i<8; i++)
  {
    if (bitLen < 1)
      return ERR_MAND_ELEM_MISS;              

    bit = GET_N_BITS(1);

    if (bit == 1)
    {
      if (bitLen < 5)
        return ERR_MAND_ELEM_MISS;              

      pwr_par->gamma_tn[i].v_gamma = TRUE;    /* any values are allowed      */
      pwr_par->gamma_tn[i].gamma   = GET_N_BITS(5);
    }
  }

  return ccdOK;
}


/*
+------------------------------------------------------------------------------
| Function    : grr_decode_ie_glob_tfi
+------------------------------------------------------------------------------
| Description : This function decodes a compressed T_glob_tfi struct
|
| Parameters  : glob_tfi   - T_glob_tfi struct destination pointer
|
| Globals:      pBuf       - Ptr to the compressed T_glob_tfi buffer
|               startBit   - Bit offset of the first T_glob_tfi bit
|               bitLen     - Lenght of the buffer in bits
|
| Returns     : ccdOK    - If no error is occured
|                        - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_ie_glob_tfi (T_glob_tfi* glob_tfi)
{
  if (bitLen < 6)
    return ERR_MAND_ELEM_MISS;              
  
  glob_tfi->flag = GET_N_BITS(1);
  if (glob_tfi->flag == 0)                    /* UPLINK_TFI                  */
  {
    glob_tfi->v_ul_tfi = TRUE;
    glob_tfi->ul_tfi   = GET_N_BITS(5);
  }
  else                                        /* DOWNLINK_TFI                */
  {
    glob_tfi->v_dl_tfi = TRUE;
    glob_tfi->dl_tfi   = GET_N_BITS(5);
  }

  return ccdOK;
}


/*
+------------------------------------------------------------------------------
| Function    : grr_decode_ie_meas_map
+------------------------------------------------------------------------------
| Description : This function decodes a compressed T_meas_map struct
|
| Parameters  : meas_map   - T_meas_map struct destination pointer
|
| Globals:      pBuf       - Ptr to the compressed T_meas_map buffer
|               startBit   - Bit offset of the first T_meas_map bit
|               bitLen     - Lenght of the buffer in bits
|
| Returns     : ccdOK    - If no error is occured
|                        - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_ie_meas_map (T_meas_map* meas_map)
{
  USHORT h_byte;

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;              

  meas_map->meas_start_grr.flag = GET_N_BITS(1);
  if (meas_map->meas_start_grr.flag == 1)         /* Relative Frame Number Encod.*/
  {
    if (bitLen < 13)
      return ERR_MAND_ELEM_MISS;              

    meas_map->meas_start_grr.v_rel = TRUE;
                             h_byte = GET_N_BITS(5);
    meas_map->meas_start_grr.rel   = (h_byte << 8) + GET_N_BITS(8);
  }
  else                                        /* Absolute Frame Number Encod.*/
  {
    if (bitLen < 16)
      return ERR_MAND_ELEM_MISS;              

    meas_map->meas_start_grr.v_abs  = TRUE;
    meas_map->meas_start_grr.abs.t1 = GET_N_BITS(5);
    meas_map->meas_start_grr.abs.t3 = GET_N_BITS(6);
    meas_map->meas_start_grr.abs.t2 = GET_N_BITS(5);
  }

  if (bitLen < 13)
    return ERR_MAND_ELEM_MISS;

  meas_map->meas_inter  = GET_N_BITS(5);
  meas_map->meas_bitmap = GET_N_BITS(8);

  return ccdOK;
}


#ifdef REL99
/*
+------------------------------------------------------------------------------
| Function    : grr_decode_ie_egprs_link_adpt_para
+------------------------------------------------------------------------------
| Description : This function decodes the EGPRS link adaptation parameters
|
| Parameters  : trgt   - T_egprs_link_adpt_para destination pointer
|
| Globals:      pBuf       - Ptr to the compressed T_egprs_link_adpt_para buffer
|               startBit   - Bit offset of the first T_egprs_link_adpt_para bit
|               bitLen     - Lenght of the buffer in bits
|
| Returns     : ccdOK    - If no error is occured
|                        - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_ie_egprs_link_adpt_para(T_egprs_link_adpt_para *trgt)
{

  TRACE_FUNCTION("grr_decode_ie_egprs_link_adpt_para");

  if (bitLen < 8) /*5+2+1*/
    return ERR_MAND_ELEM_MISS;

  trgt->egprs_ws = GET_N_BITS(5);
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;

  trgt->lqm_mode = GET_N_BITS(2);
  trgt->v_bep_period2 = GET_N_BITS(1);
  if(trgt->v_bep_period2)
  {
    trgt->bep_period2 = GET_N_BITS(4);
  }
  return ccdOK;
}


/*
+------------------------------------------------------------------------------
| Function    : grr_decode_ie_compact_red_ma
+------------------------------------------------------------------------------
| Description : This function decodes the compact_red_ma parameters
|
| Parameters  : trgt   - T_compact_red_ma destination pointer
|
| Globals:      pBuf       - Ptr to the T_compact_red_ma buffer
|               startBit   - Bit offset of the first T_compact_red_ma bit
|               bitLen     - Lenght of the buffer in bits
|
| Returns     : ccdOK    - If no error is occured
|                        - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE  grr_decode_ie_compact_red_ma(T_compact_red_ma *trgt)
{
  UBYTE     i=0;

  TRACE_FUNCTION("grr_decode_ie_compact_red_ma");

  if (bitLen < 8) /*7(for length_reduced_bitmap)+1(for v_maio_2) */
    return ERR_MAND_ELEM_MISS;

  trgt->length_reduced_bitmap = GET_N_BITS(7);
  if(bitLen < trgt->c_reduced_ma_bitmap )
      return ERR_MAND_ELEM_MISS;

  for(i=0 ; i < trgt->c_reduced_ma_bitmap ;i++)
  {
    trgt->reduced_ma_bitmap[i] = GET_N_BITS(1);
  }
  trgt->v_maio_2 = GET_N_BITS(1);
  if(trgt->v_maio_2 == 1)
  {
    if (bitLen < 6)
      return ERR_MAND_ELEM_MISS;

    trgt->maio_2 = GET_N_BITS(6);
  }
  return ccdOK;
}

#endif


/*
+------------------------------------------------------------------------------
| Function    : grr_decode_ie_tbf_s_time
+------------------------------------------------------------------------------
| Description : This function decodes a compressed T_tbf_s_time struct
|
| Parameters  : tbf_s_time - T_tbf_s_time struct destination pointer
|
| Globals:      pBuf       - Ptr to the compressed T_tbf_s_time buffer
|               startBit   - Bit offset of the first T_tbf_s_time bit
|               bitLen     - Lenght of the buffer in bits
|
| Returns     : ccdOK    - If no error is occured
|                        - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_ie_tbf_s_time (T_tbf_s_time* tbf_s_time)
{
  USHORT h_byte;
                                              /* TBF Starting Time: Starting */
                                              /* frame number description    */
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;              

  tbf_s_time->flag = GET_N_BITS(1);
  if (tbf_s_time->flag == 1)                  /* Relative Frame Number Encod.*/
  {
    if (bitLen < 13)
      return ERR_MAND_ELEM_MISS;              
    
    tbf_s_time->v_rel = TRUE;

               h_byte = GET_N_BITS(5);
    tbf_s_time->rel   = (h_byte << 8) + GET_N_BITS(8);
  }
  else                                        /* Absolute Frame Number Encod.*/
  {
    if (bitLen < 16)
      return ERR_MAND_ELEM_MISS;              
    
    tbf_s_time->v_abs = TRUE;

    tbf_s_time->abs.t1 = GET_N_BITS(5);
    tbf_s_time->abs.t3 = GET_N_BITS(6);
    tbf_s_time->abs.t2 = GET_N_BITS(5);
  }

  return ccdOK;
}

/*
+------------------------------------------------------------------------------
| Function    : grr_decode_ie_dyn_alloc_p
+------------------------------------------------------------------------------
| Description : This function decodes a compressed Dynamic Allocation IE
|
| Parameters  : dyn_alloc_p - T_dyn_alloc_p struct destination pointer
|
| Globals:      pBuf        - Ptr to the compressed T_dyn_alloc_p buffer
|               startBit    - Bit offset of the first T_dyn_alloc_p bit
|               bitLen      - Lenght of the buffer in bits
|
| Returns     : ccdOK    - If no error is occured
|                        - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_ie_dyn_alloc_p (T_dyn_alloc_p* dyn_alloc_p)
{
  int i;
  int bit;                                    
  UBYTE  ret_code;


  if (bitLen < 2)                       
    return ERR_MAND_ELEM_MISS;

  dyn_alloc_p->xdyn_alloc = GET_N_BITS(1);

  dyn_alloc_p->flag2 = GET_N_BITS(1);
  if (dyn_alloc_p->flag2 == 1)                /* P0:bit(4), PR_MODE:bit(1)   */
  {
    if (bitLen < 5)
      return ERR_MAND_ELEM_MISS;

    dyn_alloc_p->v_p0      = TRUE;
    dyn_alloc_p->p0        = GET_N_BITS(4);
    dyn_alloc_p->v_pr_mode = TRUE;
    dyn_alloc_p->pr_mode   = GET_N_BITS(1);
  }

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;

  dyn_alloc_p->usf_grant = GET_N_BITS(1);     /* USF_GRANULARITY:bit(1)      */

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* UL_TFI_ASSIGNMENT : bit(5)  */
  {
    if (bitLen < 5)
      return ERR_MAND_ELEM_MISS;

    dyn_alloc_p->v_ul_tfi_assign = TRUE;
    dyn_alloc_p->ul_tfi_assign   = GET_N_BITS(5);
  }

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* RLC_DATA_B._GRANTED:bit(8)  */
  {
    if (bitLen < 8)
      return ERR_MAND_ELEM_MISS;

    dyn_alloc_p->v_rlc_db_granted = TRUE;
    dyn_alloc_p->rlc_db_granted   = GET_N_BITS(8);
  }

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* TBF Starting Time: Starting */
  {                                           /* frame number description    */
    ret_code = grr_decode_ie_tbf_s_time (&dyn_alloc_p->tbf_s_time);

    if (ret_code == ccdOK)
      dyn_alloc_p->v_tbf_s_time = TRUE;
    else
      return ret_code;
  }

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;

  dyn_alloc_p->flag = GET_N_BITS(1);
  if (dyn_alloc_p->flag == 0)                 /* Timeslot Allocation         */
  {
    for (i=0; i<8; i++)
    {
      if (bitLen < 1)
        return ERR_MAND_ELEM_MISS;

      bit = GET_N_BITS(1);
      if (bit == 1)
      {
        if (bitLen < 3)
          return ERR_MAND_ELEM_MISS;

        dyn_alloc_p->tn_alloc[i].v_usf = TRUE;
        dyn_alloc_p->tn_alloc[i].usf   = GET_N_BITS(3);
      }
    }

    dyn_alloc_p->v_tn_alloc = TRUE;
  }
  else                                        /* Timeslot Allocation with    */
  {                                           /*   Power Control Parameters  */
    if (bitLen < 4)
      return ERR_MAND_ELEM_MISS;

    dyn_alloc_p->tn_alloc_pwr.alpha = GET_N_BITS(4);

    for (i=0; i<8; i++)
    {
      if (bitLen < 1)
        return ERR_MAND_ELEM_MISS;

      bit = GET_N_BITS(1);
      if (bit == 1)
      {
        if (bitLen < 8)
          return ERR_MAND_ELEM_MISS;

        dyn_alloc_p->tn_alloc_pwr.usf_array[i].v_usf_g     = TRUE;
        dyn_alloc_p->tn_alloc_pwr.usf_array[i].usf_g.usf   = GET_N_BITS(3);
        dyn_alloc_p->tn_alloc_pwr.usf_array[i].usf_g.gamma = GET_N_BITS(5);
      }
    }

    dyn_alloc_p->v_tn_alloc_pwr = TRUE;
  }

  return ccdOK;
} /* grr_decode_ie_dyn_alloc_p */


/*
+------------------------------------------------------------------------------
| Function    : grr_decode_ie_dyn_alloc_ts
+------------------------------------------------------------------------------
| Description : This function decodes a compressed Dynamic Allocation IE
|
| Parameters  : dyn_alloc_ts - T_dyn_alloc_ts struct destination pointer
|
| Globals:      pBuf        - Ptr to the compressed T_dyn_alloc_p buffer
|               startBit    - Bit offset of the first T_dyn_alloc_p bit
|               bitLen      - Lenght of the buffer in bits
|
| Returns     : ccdOK    - If no error is occured
|                        - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_ie_dyn_alloc_ts (T_dyn_alloc_ts* dyn_alloc_ts)
{
  int i;
  int bit;                                    
  UBYTE  ret_code;


  if (bitLen < 2)                       
    return ERR_MAND_ELEM_MISS;

  dyn_alloc_ts->xdyn_alloc = GET_N_BITS(1);

  dyn_alloc_ts->flag2 = GET_N_BITS(1);
  if (dyn_alloc_ts->flag2 == 1)                /* P0:bit(4), PR_MODE:bit(1)   */
  {
    if (bitLen < 5)
      return ERR_MAND_ELEM_MISS;

    dyn_alloc_ts->v_p0      = TRUE;
    dyn_alloc_ts->p0        = GET_N_BITS(4);
    dyn_alloc_ts->v_pr_mode = TRUE;
    dyn_alloc_ts->pr_mode   = GET_N_BITS(1);
  }

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;

  dyn_alloc_ts->usf_grant = GET_N_BITS(1);     /* USF_GRANULARITY:bit(1)      */

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* RLC_DATA_B._GRANTED:bit(8)  */
  {
    if (bitLen < 8)
      return ERR_MAND_ELEM_MISS;

    dyn_alloc_ts->v_rlc_db_granted = TRUE;
    dyn_alloc_ts->rlc_db_granted   = GET_N_BITS(8);
  }

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* TBF Starting Time: Starting */
  {                                           /* frame number description    */
    ret_code = grr_decode_ie_tbf_s_time (&dyn_alloc_ts->tbf_s_time);

    if (ret_code == ccdOK)
      dyn_alloc_ts->v_tbf_s_time = TRUE;
    else
      return ret_code;
  }

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;

  dyn_alloc_ts->flag = GET_N_BITS(1);
  if (dyn_alloc_ts->flag == 0)                 /* Timeslot Allocation         */
  {
    for (i=0; i<8; i++)
    {
      if (bitLen < 1)
        return ERR_MAND_ELEM_MISS;

      bit = GET_N_BITS(1);
      if (bit == 1)
      {
        if (bitLen < 3)
          return ERR_MAND_ELEM_MISS;

        dyn_alloc_ts->tn_alloc[i].v_usf = TRUE;
        dyn_alloc_ts->tn_alloc[i].usf   = GET_N_BITS(3);
      }
    }

    dyn_alloc_ts->v_tn_alloc = TRUE;
  }
  else                                        /* Timeslot Allocation with    */
  {                                           /*   Power Control Parameters  */
    if (bitLen < 4)
      return ERR_MAND_ELEM_MISS;

    dyn_alloc_ts->tn_alloc_pwr.alpha = GET_N_BITS(4);

    for (i=0; i<8; i++)
    {
      if (bitLen < 1)
        return ERR_MAND_ELEM_MISS;

      bit = GET_N_BITS(1);
      if (bit == 1)
      {
        if (bitLen < 8)
          return ERR_MAND_ELEM_MISS;

        dyn_alloc_ts->tn_alloc_pwr.usf_array[i].v_usf_g     = TRUE;
        dyn_alloc_ts->tn_alloc_pwr.usf_array[i].usf_g.usf   = GET_N_BITS(3);
        dyn_alloc_ts->tn_alloc_pwr.usf_array[i].usf_g.gamma = GET_N_BITS(5);
      }
    }

    dyn_alloc_ts->v_tn_alloc_pwr = TRUE;
  }

  return ccdOK;
} /* grr_decode_ie_dyn_alloc_ts */


/*
+------------------------------------------------------------------------------
| Function    : grr_decode_ie_freq_par
+------------------------------------------------------------------------------
| Description : This function decodes a compressed T_freq_par struct
|
| Parameters  : freq_par - T_freq_par struct destination pointer
|
| Globals:      pBuf     - Ptr to the compressed T_freq_par buffer
|               startBit - Bit offset of the first T_freq_par bit
|               bitLen   - Lenght of the buffer in bits
|
| Returns     : ccdOK    - If no error is occured
|                        - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_ie_freq_par (T_freq_par* freq_par)
{
  int    i;                                      
  int    bit;                                    
  USHORT h_byte;


  if (bitLen < 5)
    return ERR_MAND_ELEM_MISS;                

  freq_par->tsc   = GET_N_BITS(3);            /* TSC : bit (3)               */

  freq_par->flag  = GET_N_BITS(1);
  freq_par->flag2 = GET_N_BITS(1);

  if (freq_par->flag == 0)             
  {
    if (freq_par->flag2 == 0)                 /* ARFCN : bit(10)             */
    {
      if (bitLen < 10)
        return ERR_MAND_ELEM_MISS;

      freq_par->v_arfcn = TRUE;
                    h_byte   = GET_N_BITS(2);
      freq_par->arfcn   = (h_byte << 8) + GET_N_BITS(8);
    }
    else                                      /* Indirect encoding           */
    {
      if (bitLen < 11)
        return ERR_MAND_ELEM_MISS;

      freq_par->indi_encod.maio   = GET_N_BITS(6);
      freq_par->indi_encod.ma_num = GET_N_BITS(4);

      bit = GET_N_BITS(1);
      if (bit == 1)
      {
        if (bitLen < 3)
          return ERR_MAND_ELEM_MISS;

        freq_par->indi_encod.v_chamge_ma_sub   = TRUE;
        freq_par->indi_encod.chamge_ma_sub.cm1 = GET_N_BITS(2);

        bit = GET_N_BITS(1);
        if (bit == 1)
        {
          if (bitLen < 2)
            return ERR_MAND_ELEM_MISS;

          freq_par->indi_encod.chamge_ma_sub.v_cm2 = TRUE;
          freq_par->indi_encod.chamge_ma_sub.cm2   = GET_N_BITS(2);
        }
      }

      freq_par->v_indi_encod = TRUE;
    }
  }
  else
  {
    if (freq_par->flag2 == 0)                 /* Direct encoding 1           */
    {
      if (bitLen < 13)
        return ERR_MAND_ELEM_MISS;

      freq_par->di_encod1.maio = GET_N_BITS(6);
      
      freq_par->di_encod1.gprs_ms_alloc_ie.hsn = GET_N_BITS(6);

      bit = GET_N_BITS(1);                    
      if (bit == 1)                           /* RFL number list             */
      {
        i = 0;

        do {
          if (i >= MAX_RFL_NUM_LIST)
            return ERR_MAND_ELEM_MISS;

          if (bitLen < 5)
            return ERR_MAND_ELEM_MISS;

          freq_par->di_encod1.gprs_ms_alloc_ie.rfl_num_list[i++].
                                                     rfl_num = GET_N_BITS(4);
          bit = GET_N_BITS(1);

        } while (bit == 1);
        
        freq_par->di_encod1.gprs_ms_alloc_ie.c_rfl_num_list = i; 
        freq_par->di_encod1.gprs_ms_alloc_ie.v_rfl_num_list = TRUE;
      }

      if (bitLen < 1)
        return ERR_MAND_ELEM_MISS;

      freq_par->di_encod1.gprs_ms_alloc_ie.flag = GET_N_BITS(1);
      if (freq_par->di_encod1.gprs_ms_alloc_ie.flag == 0)
      {                                       /* MA_LENGTH, MA_BITMAP        */
        if (bitLen < 6)
          return ERR_MAND_ELEM_MISS;

        i = GET_N_BITS(6);
        freq_par->di_encod1.gprs_ms_alloc_ie.ma_struct.ma_len   = i;
        freq_par->di_encod1.gprs_ms_alloc_ie.ma_struct.c_ma_map = i + 1;

        if (bitLen < (i + 1))
          return ERR_MAND_ELEM_MISS;

        for (i=0; i<freq_par->di_encod1.gprs_ms_alloc_ie.ma_struct.c_ma_map; i++)
        {
          freq_par->di_encod1.gprs_ms_alloc_ie.ma_struct.ma_map[i] = GET_N_BITS(1);
        }

        freq_par->di_encod1.gprs_ms_alloc_ie.v_ma_struct = TRUE;
      }
      else                                    /* ARFCN index list            */
      {
        if (bitLen < 1)
          return ERR_MAND_ELEM_MISS;

        bit = GET_N_BITS(1);                    
        if (bit == 1)                         
        {
          i = 0;

          do {
            if (i >= MAX_ARFCN_LIST)
              return ERR_MAND_ELEM_MISS;

            if (bitLen < 7)
              return ERR_MAND_ELEM_MISS;

            freq_par->di_encod1.gprs_ms_alloc_ie.arfcn_index_list[i++].
                                                 arfcn_index = GET_N_BITS(6);
            bit = GET_N_BITS(1);

          } while (bit == 1);
      
          freq_par->di_encod1.gprs_ms_alloc_ie.c_arfcn_index_list = i; 
          freq_par->di_encod1.gprs_ms_alloc_ie.v_rfl_num_list     = TRUE;
        }

        freq_par->di_encod1.gprs_ms_alloc_ie.v_arfcn_index_list = TRUE;
      }

      freq_par->v_di_encod1 = TRUE;
    }
    else                                      /* Direct encoding 2           */
    {
      if (bitLen < 16)
        return ERR_MAND_ELEM_MISS;

      freq_par->di_encod2.maio        = GET_N_BITS(6);
      freq_par->di_encod2.hsn         = GET_N_BITS(6);
      freq_par->di_encod2.len_ma_list = GET_N_BITS(4);
      freq_par->di_encod2.c_ma_list   = freq_par->di_encod2.len_ma_list + 3;

      if (bitLen < (freq_par->di_encod2.c_ma_list << 3))
        return ERR_MAND_ELEM_MISS;

      for (i=0; i<freq_par->di_encod2.c_ma_list; i++)
      {
        freq_par->di_encod2.ma_list[i] = GET_N_BITS(8);
      }

      freq_par->v_di_encod2 = TRUE;
    }
  }

  return ccdOK;
}

/*
+------------------------------------------------------------------------------
| Function    : grr_decode_dl_assignment
+------------------------------------------------------------------------------
| Description : For performance reasons this function replaces the call to the
|               function ccd_decodeMsg. If no error occured, the D_DL_ASSIGN 
|               message is decoded into _decodeCtrlMsg.
|
| Parameters  : buf    - pointer to beginning of the D_DL_ASSIGN IE
|               len    - lenght of the buffer in bits
|               off    - bit offset of the first bit in the buffer
|
| Returns     : ccdOK  - if no error is occured
|                      - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_dl_assignment (UBYTE* buf, int off, int len)
{ 
  int    bit;                                    
  UBYTE  ret_code;

  MCAST(pMsg, D_DL_ASSIGN);

  /*-------------------------------------------------------------------------*/
  startBit = off;                             /* Initiate global data        */
  bitLen   = len;
  pBuf     = buf;

  memset (pMsg, 0, sizeof(T_D_DL_ASSIGN));    /* init destination struct     */
  /*-------------------------------------------------------------------------*/

  if (bitLen < 8)                             
    return ERR_MAND_ELEM_MISS;
  
  pMsg->msg_type  = GET_N_BITS(6);            /* MESSAGE_TYPE : bit(6)       */
  
  /* --- DISTRIBUTION CONTENTS --- */

  pMsg->page_mode = GET_N_BITS(2);            /* PAGE_MODE : bit(2)          */ 

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ccdOK;                             

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* PERSISTENCE_LEVEL: bit(4)*4 */
  {
    if (bitLen < 16)
      return ERR_MAND_ELEM_MISS;
    
    pMsg->v_pers_lev = TRUE;                  

    pMsg->pers_lev.plev[0] = GET_N_BITS(4);
    pMsg->pers_lev.plev[1] = GET_N_BITS(4);
    pMsg->pers_lev.plev[2] = GET_N_BITS(4);
    pMsg->pers_lev.plev[3] = GET_N_BITS(4);
  }

  if (bitLen <= 0)
    return ccdOK;                             

  /* --- ADDRESS INFORMATON --- */

  /*-------------------------------------------------------------------------*/
  pMsg->add1.flag = GET_N_BITS(1);
  if (pMsg->add1.flag == 0)                   /* Global TFI                  */
  {
    ret_code = grr_decode_ie_glob_tfi (&pMsg->add1.glob_tfi);

    if (ret_code == ccdOK)
      pMsg->add1.v_glob_tfi = TRUE;
    else
      return ret_code;
  }
  else
  {
    if (bitLen < 1)
      return ERR_MAND_ELEM_MISS;              

    bit = GET_N_BITS(1);
    if (bit == 0)                             /* TLLI                        */
    {
      ret_code = grr_decode_ie_tlli (&pMsg->add1.tlli_value);

      if(ret_code == ccdOK)
        pMsg->add1.v_tlli_value = TRUE;               
      else
        return ret_code;
    }
    else
    {
      return ERR_MAND_ELEM_MISS;              
    }
  }

  /*-------------------------------------------------------------------------*/

  /* --- MESSAGE ESCAPE --- */
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* 1 equals to Message Escape  */
    return ccdOK;                             /*   then we are ready         */

  /* --- NON-DISTRIBUTION CONTENTS --- */

  if (bitLen < 12)
    return ERR_MAND_ELEM_MISS;                

  pMsg->mac_mode = GET_N_BITS(2);             /* MAC_MODE : bit(2)           */
  pMsg->rlc_mode = GET_N_BITS(1);             /* RLC_MODE : bit(1)           */
  pMsg->ctrl_ack = GET_N_BITS(1);             /* CONTROL_ACK : bit(1)        */
  pMsg->ts_alloc = GET_N_BITS(8);             /* TIMESLOT_ALLOCATION : bit(8)*/

  /*-------------------------------------------------------------------------*/

  ret_code = grr_decode_ie_pta (&pMsg->pta);  /* Packet Timing Advanced IE   */
  
  if(ret_code != ccdOK)
    return ret_code;

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* P0:bit(4)                   */
  {                                           /* BTS_PWR_CTRL_MODE : bit(1)  */
                                              /* PR_MODE : bit(1)            */
    if (bitLen < 6)
      return ERR_MAND_ELEM_MISS;              

    pMsg->v_bts_pwr_ctrl       = TRUE;        
    pMsg->bts_pwr_ctrl.p0      = GET_N_BITS(4);
    pMsg->bts_pwr_ctrl.mode    = GET_N_BITS(1);   
    pMsg->bts_pwr_ctrl.pr_mode = GET_N_BITS(1);
  }

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* Frequency Parameters IE     */
  {
    ret_code = grr_decode_ie_freq_par (&pMsg->pda_trnc_grp.freq_par);

    if (ret_code == ccdOK)
      pMsg->pda_trnc_grp.v_freq_par = TRUE;
    else
      return ret_code;
  }

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* DOWNL.TFI_ASSIGNEMENT:bit(5)*/
  {
    if (bitLen < 5)
      return ERR_MAND_ELEM_MISS;              

    pMsg->pda_trnc_grp.v_dl_tfi_assign = TRUE;             
    pMsg->pda_trnc_grp.dl_tfi_assign   = GET_N_BITS(5);  
  }
  
  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* Power Control Parameters    */
  {
    ret_code = grr_decode_ie_pwr_par (&pMsg->pda_trnc_grp.pwr_par);

    if (ret_code == ccdOK)
      pMsg->pda_trnc_grp.v_pwr_par = TRUE;
    else
      return ret_code;
  }

  /*-------------------------------------------------------------------------*/
  bit = GET_N_BITS(1);
  if (bit == 1)                               /* TBF Starting Time: Starting */
  {                                           /* frame number description    */
    ret_code = grr_decode_ie_tbf_s_time (&pMsg->pda_trnc_grp.tbf_s_time);

    if (ret_code == ccdOK)
      pMsg->pda_trnc_grp.v_tbf_s_time = TRUE;
    else
      return ret_code;
  }

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;              

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* Measurement Mapping         */
  {                                       
    ret_code = grr_decode_ie_meas_map (&pMsg->pda_trnc_grp.meas_map);

    if (ret_code == ccdOK)
      pMsg->pda_trnc_grp.v_meas_map = TRUE;
    else
      return ret_code;
  }

#ifdef REL99
  /*Extending the Packet downlink assignment message decoding
   *for R99 additions
   */
  pMsg->pda_trnc_grp.v_release_99_str_pda = GET_N_BITS(1);
  
  if(pMsg->pda_trnc_grp.v_release_99_str_pda == 1)
  {
    bit = GET_N_BITS(1);
    if(bit == 1)
    {
      ret_code = grr_decode_ie_egprs_link_adpt_para(
        &pMsg->pda_trnc_grp.release_99_str_pda.egprs_link_adpt_para);
      if(ret_code == ccdOK)
        pMsg->pda_trnc_grp.release_99_str_pda.v_egprs_link_adpt_para = TRUE;
      else
        return ret_code;
    }
    if(bitLen < 1)
      return ERR_MAND_ELEM_MISS;
    bit = GET_N_BITS(1);
    if(bit == 1)
    {
      if(bitLen < 2)
        return ERR_MAND_ELEM_MISS;
      pMsg->pda_trnc_grp.release_99_str_pda.v_p_ext_ta = TRUE;
      pMsg->pda_trnc_grp.release_99_str_pda.p_ext_ta = GET_N_BITS(2);
    }

    if(bitLen < 1)
      return ccdOK;
    bit = GET_N_BITS(1);
    if(bit == 1)
    {

      ret_code = grr_decode_ie_compact_red_ma(&pMsg->pda_trnc_grp.release_99_str_pda.compact_red_ma);
      if(ret_code == ccdOK)
        pMsg->pda_trnc_grp.release_99_str_pda.v_compact_red_ma = TRUE;
      else
        return ret_code;
    }
  }
#endif

  /* --- PADDING BITS --- */ /* -> currently not decoded and/or validated */

  return ccdOK;
}
/*
+------------------------------------------------------------------------------
| Function    : grr_decode_pdch_release
+------------------------------------------------------------------------------
| Description : For performance reasons this function replaces the call to the
|               function ccd_decodeMsg. If no error occured, the D_PDCH_RELEASE 
|               message is decoded into _decodeCtrlMsg.
|
| Parameters  : buf    - pointer to beginning of the D_PDCH_RELEASE IE
|               len    - lenght of the buffer in bits
|               off    - bit offset of the first bit in the buffer
|
| Returns     : ccdOK  - if no error is occured
|                      - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_pdch_release (UBYTE* buf, int off, int len)
{ 
  MCAST(pMsg, D_PDCH_RELEASE);

  /*-------------------------------------------------------------------------*/
  startBit = off;                             /* Initiate global data        */
  bitLen   = len;
  pBuf     = buf;

  memset (pMsg, 0, sizeof(T_D_PDCH_RELEASE)); /* init destination struct     */
  /*-------------------------------------------------------------------------*/

  if (bitLen < 8)                             
    return ERR_MAND_ELEM_MISS;
  
  pMsg->msg_type  = GET_N_BITS(6);            /* MESSAGE_TYPE : bit(6)       */
  
  /* --- DISTRIBUTION CONTENTS --- */

  pMsg->page_mode = GET_N_BITS(2);            /* PAGE_MODE : bit(2)          */ 

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ccdOK;                             

  pMsg->v_ts_available = GET_N_BITS(1);       /* TIMESLOT AVAILABLE bit(1)  */

  if (bitLen <= 0)
    return ccdOK;                             

  pMsg->ts_available   = GET_N_BITS(8);       /* TIMESLOT MASK     bit(8)   */

  /* --- PADDING BITS --- */ /* -> currently not decoded and/or validated */

  return ccdOK;
}

/*
+------------------------------------------------------------------------------
| Function    : grr_decode_ul_assignment
+------------------------------------------------------------------------------
| Description : For performance reasons this function replaces the call to the
|               function ccd_decodeMsg. If no error occured, the D_UL_ASSIGN 
|               message is decoded into _decodeCtrlMsg.
|
| Parameters  : buf    - pointer to beginning of the D_DL_ASSIGN IE
|               len    - lenght of the buffer in bits
|               off    - bit offset of the first bit in the buffer
|
| Returns     : ccdOK  - if no error is occured
|                      - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_ul_assignment (UBYTE* buf, int off, int len)
{ 
  int    i;
  int    bit;                                    
  USHORT h_byte;
  UBYTE  ret_code;

  MCAST(pMsg, D_UL_ASSIGN);

  /*-------------------------------------------------------------------------*/
  startBit = off;                             /* Initiate global data        */
  bitLen   = len;
  pBuf     = buf;

  memset (pMsg, 0, sizeof(T_D_UL_ASSIGN));    /* init destination struct     */

  /*-------------------------------------------------------------------------*/

  if (bitLen < 8)                             
    return ERR_MAND_ELEM_MISS;
  
  pMsg->msg_type  = GET_N_BITS(6);            /* MESSAGE_TYPE : bit(6)       */
  
  /* --- DISTRIBUTION CONTENTS --- */

  pMsg->page_mode = GET_N_BITS(2);            /* PAGE_MODE : bit(2)          */ 

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ccdOK;                             

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* PERSISTENCE_LEVEL: bit(4)*4 */
  {
    if (bitLen < 16)
      return ERR_MAND_ELEM_MISS;
    
    pMsg->v_pers_lev = TRUE;                  

    pMsg->pers_lev.plev[0] = GET_N_BITS(4);
    pMsg->pers_lev.plev[1] = GET_N_BITS(4);
    pMsg->pers_lev.plev[2] = GET_N_BITS(4);
    pMsg->pers_lev.plev[3] = GET_N_BITS(4);
  }

  if (bitLen <= 0)
    return ccdOK;                             

  /*-------------------------------------------------------------------------*/

  /* --- ADDRESS INFORMATON --- */

  pMsg->add3.flag = GET_N_BITS(1);
  if (pMsg->add3.flag == 0)                   /* Global TFI                  */
  {
    ret_code = grr_decode_ie_glob_tfi (&pMsg->add3.glob_tfi);

    if (ret_code == ccdOK)
      pMsg->add3.v_glob_tfi = TRUE;
    else
      return ret_code;
  }
  else
  {
    if (bitLen < 1)
      return ERR_MAND_ELEM_MISS;              

    pMsg->add3.v_flag2 = TRUE;

    pMsg->add3.flag2 = GET_N_BITS(1);
    if (pMsg->add3.flag2 == 0)                /* TLLI                        */
    {
      if (bitLen < 32)
        return ERR_MAND_ELEM_MISS;

      ret_code = grr_decode_ie_tlli (&pMsg->add3.tlli_value);

      if(ret_code == ccdOK)
        pMsg->add3.v_tlli_value = TRUE;             
      else
        return ret_code;
    }
    else
    {
      if (bitLen < 1)
        return ERR_MAND_ELEM_MISS;              

      pMsg->add3.v_flag3 = TRUE;

      pMsg->add3.flag3 = GET_N_BITS(1);
      if (pMsg->add3.flag3 == 0)              /* TQI : bit(16)               */
      {
        if (bitLen < 16)
          return ERR_MAND_ELEM_MISS;              

        pMsg->add3.v_tqi = TRUE;             
                  h_byte = GET_N_BITS(8);
        pMsg->add3.tqi   = (h_byte << 8) + GET_N_BITS(8);
      }
      else                                    /* Packet Request Reference    */
      {
        if (bitLen < 27)
          return ERR_MAND_ELEM_MISS;              

        pMsg->add3.v_req_ref_p = TRUE;

                                  h_byte = GET_N_BITS(3);
        pMsg->add3.req_ref_p.access_info = (h_byte << 8) +  GET_N_BITS(8);

        pMsg->add3.req_ref_p.fn_mod.t1 = GET_N_BITS(5);
        pMsg->add3.req_ref_p.fn_mod.t3 = GET_N_BITS(6);
        pMsg->add3.req_ref_p.fn_mod.t2 = GET_N_BITS(5);
      }
    }
  }

  /*-------------------------------------------------------------------------*/

  /* --- MESSAGE ESCAPE --- */

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;

#ifdef REL99  
  pMsg->egprs_flag = GET_N_BITS(1);
  if (pMsg->egprs_flag == 1)
  {
    /* Extending the Packet uplink assignment message decoding 
	 * for EGPRS additions. We do not support EGPRS. So return
	 * doing nothing
	 */
    return ccdOK;
  }
#else
  bit = GET_N_BITS(1);
  if (bit == 1)                               /* 1 equals to Message Escape  */
    return ccdOK;                             /*   then we are ready         */
#endif

  /* When message egprs_flag is FALSE, the valid flags of all the 
   * corresponding conditional elements, is made TRUE here
   */
  pMsg->v_chan_coding_cmd = pMsg->v_tlli_chan_coding = TRUE;
  pMsg->v_pta = pMsg->v_flag = pMsg->v_flag2 = TRUE;
#ifdef REL99
  pMsg->v_release_99 = TRUE;
#endif

  /* --- NON-DISTRIBUTION CONTENTS --- */

  if (bitLen < 3)
    return ERR_MAND_ELEM_MISS;                

  pMsg->chan_coding_cmd  = GET_N_BITS(2);     /* CHANNEL_COD._COMMAND:bit(2) */

  pMsg->tlli_chan_coding = GET_N_BITS(1);     /* TLLI_BLOCK_CHAN_COD.:bit(1) */

  /*-------------------------------------------------------------------------*/
  ret_code = grr_decode_ie_pta (&pMsg->pta);  /* Packet Timing Advanced IE   */
  
  if(ret_code != ccdOK)
    return ret_code;

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* Frequency Parameters IE     */
  {
    ret_code = grr_decode_ie_freq_par (&pMsg->freq_par);

    if (ret_code == ccdOK)
      pMsg->v_freq_par = TRUE;
    else
      return ret_code;
  }

  /*-------------------------------------------------------------------------*/

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  pMsg->flag = GET_N_BITS(1);
  if (pMsg->flag == 0)                        
  {
    if (bitLen < 1)
      return ERR_MAND_ELEM_MISS;                

    pMsg->flag2 = GET_N_BITS(1);
    if (pMsg->flag2 == 1)                     /* 01: Dynamic Allocation IE   */
    {
      ret_code = grr_decode_ie_dyn_alloc_p (&pMsg->dyn_alloc_p);

      if (ret_code == ccdOK)
        pMsg->v_dyn_alloc_p = TRUE;
      else
        return ret_code;
    }
    else
    {
      /* ffs. */                              /* 00: for further extensions  */
    }
  }
  else
  {
    if (bitLen < 1)
      return ERR_MAND_ELEM_MISS;                

    pMsg->flag2 = GET_N_BITS(1);
    if (pMsg->flag2 == 0)                     /* 10: Single Block Allocation */
    {
      if (bitLen < 4)
        return ERR_MAND_ELEM_MISS;                

      pMsg->sin_alloc.tn = GET_N_BITS(3);

      if (bitLen < 1)
        return ERR_MAND_ELEM_MISS;

      bit = GET_N_BITS(1);
      if (bit == 1)
      {
        if (bitLen < 9)
          return ERR_MAND_ELEM_MISS;

        pMsg->sin_alloc.v_alf_gam = TRUE;
        pMsg->sin_alloc.alf_gam.alpha = GET_N_BITS(4);
        pMsg->sin_alloc.alf_gam.gamma = GET_N_BITS(5);
      }

      if (bitLen < 1)
        return ERR_MAND_ELEM_MISS;

      bit = GET_N_BITS(1);
      if (bit == 1)
      {
        if (bitLen < 6)
          return ERR_MAND_ELEM_MISS;

        pMsg->sin_alloc.v_bts_pwr_ctrl       = TRUE;
        pMsg->sin_alloc.bts_pwr_ctrl.p0      = GET_N_BITS(4);
        pMsg->sin_alloc.bts_pwr_ctrl.mode    = GET_N_BITS(1);
        pMsg->sin_alloc.bts_pwr_ctrl.pr_mode = GET_N_BITS(1);
      }

      ret_code = grr_decode_ie_tbf_s_time (&pMsg->sin_alloc.tbf_s_time);

      if (ret_code != ccdOK)
        return ret_code;

      pMsg->v_sin_alloc = TRUE;
    }
    else                                      /* 11: Fixed Allocation        */
    {
      if (bitLen < 1)
        return ERR_MAND_ELEM_MISS;

      bit = GET_N_BITS(1);
      if (bit == 1)
      {
        pMsg->f_alloc_ul.v_ul_tfi_assign = TRUE;
        pMsg->f_alloc_ul.ul_tfi_assign   = GET_N_BITS(5);
      }

      if (bitLen < 5)
        return ERR_MAND_ELEM_MISS;

      pMsg->f_alloc_ul.final_alloc = GET_N_BITS(1);;
      pMsg->f_alloc_ul.dl_ctrl_ts  = GET_N_BITS(3);;

      bit = GET_N_BITS(1);
      if (bit == 1)
      {
        if (bitLen < 6)
          return ERR_MAND_ELEM_MISS;

        pMsg->f_alloc_ul.v_bts_pwr_ctrl       = TRUE;
        pMsg->f_alloc_ul.bts_pwr_ctrl.p0      = GET_N_BITS(4);
        pMsg->f_alloc_ul.bts_pwr_ctrl.mode    = GET_N_BITS(1);
        pMsg->f_alloc_ul.bts_pwr_ctrl.pr_mode = GET_N_BITS(1);
      }

      pMsg->f_alloc_ul.flag = GET_N_BITS(1);
      if(pMsg->f_alloc_ul.flag == 0)
      {
        pMsg->f_alloc_ul.v_ts_alloc = TRUE;
        pMsg->f_alloc_ul.ts_alloc   = GET_N_BITS(8);
      }
      else
      {
        ret_code = grr_decode_ie_pwr_par (&pMsg->f_alloc_ul.pwr_par);

        if (ret_code == ccdOK)
          pMsg->f_alloc_ul.v_pwr_par = TRUE;
        else
          return ret_code;
      }

      if (bitLen < 1)
        return ERR_MAND_ELEM_MISS;

      pMsg->f_alloc_ul.half_dupelx = GET_N_BITS(1);

      ret_code = grr_decode_ie_tbf_s_time (&pMsg->f_alloc_ul.tbf_s_time);

      if (ret_code != ccdOK)
        return ret_code;

      bit = GET_N_BITS(1);
      if(bit == 0)
      {
        if (bitLen < 1)
          return ERR_MAND_ELEM_MISS;

        pMsg->f_alloc_ul.flag2 = GET_N_BITS(1);
        if (pMsg->f_alloc_ul.flag2 == 0)      /* with len Allocation Bitmap  */
        {
          if (bitLen < 8)
            return ERR_MAND_ELEM_MISS;

          pMsg->f_alloc_ul.block_struct.bl_o_bl_per = GET_N_BITS(1);
          pMsg->f_alloc_ul.block_struct.a_map_len   = GET_N_BITS(7);

          if (bitLen < pMsg->f_alloc_ul.block_struct.a_map_len)
            return ERR_MAND_ELEM_MISS;

          for (i=0; i<pMsg->f_alloc_ul.block_struct.a_map_len; i++)
          {
            pMsg->f_alloc_ul.block_struct.alloc_map[i] = GET_N_BITS(1);
          }

          pMsg->f_alloc_ul.block_struct.c_alloc_map = i;
          pMsg->f_alloc_ul.v_block_struct           = TRUE;
        }
        else                          /* without lenght of Allocation Bitmap */
        {                             /* Bitmap fills remainder of message   */
          pMsg->f_alloc_ul.v_alloc_map = TRUE;

          if (bitLen > 127)
            pMsg->f_alloc_ul.c_alloc_map = 127;
          else
            pMsg->f_alloc_ul.c_alloc_map = bitLen;

          for (i=0; i<pMsg->f_alloc_ul.c_alloc_map; i++)
          {
            pMsg->f_alloc_ul.alloc_map[i] = GET_N_BITS(1);
          }
        }
      }
      else
      {
         /* Message Escape */
      }

      pMsg->v_f_alloc_ul = TRUE;
    }
  }

#ifdef REL99
  /* Updation of packet uplink assignment message decoding for 
   * R99 extensions 
   */
  pMsg->release_99 = GET_N_BITS(1);
  if(pMsg->release_99 == 1)
  {
    if(bitLen < 2)
      return ERR_MAND_ELEM_MISS;
    pMsg->v_p_ext_ta = TRUE;
    pMsg->p_ext_ta = GET_N_BITS(2);
  }
#endif

  /* --- PADDING BITS --- */ /* -> currently not decoded and/or validated */
  return ccdOK;
}

/*
+------------------------------------------------------------------------------
| Function    : grr_decode_polling_req
+------------------------------------------------------------------------------
| Description : For performance reasons this function replaces the call to the
|               function ccd_decodeMsg. If no error occured, the D_POLLING_REQ 
|               message is decoded into _decodeCtrlMsg.
|
| Parameters  : buf    - pointer to beginning of the D_POLLING_REQ IE
|               len    - lenght of the buffer in bits
|               off    - bit offset of the first bit in the buffer
|
| Returns     : ccdOK  - if no error is occured
|                      - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_polling_req (UBYTE* buf, int off, int len)
{ 
  int    bit;                                    
  USHORT h_byte;
  UBYTE  ret_code;

  MCAST(pMsg, D_POLLING_REQ);

  /*-------------------------------------------------------------------------*/
  startBit = off;                             /* Initiate global data        */
  bitLen   = len;
  pBuf     = buf;

  memset (pMsg, 0, sizeof(T_D_POLLING_REQ));  /* init destination struct     */

  /*-------------------------------------------------------------------------*/

  if (bitLen < 8)                             
    return ERR_MAND_ELEM_MISS;
  
  pMsg->msg_type  = GET_N_BITS(6);            /* MESSAGE_TYPE : bit(6)       */
  
  /* --- DISTRIBUTION CONTENTS --- */

  pMsg->page_mode = GET_N_BITS(2);            /* PAGE_MODE : bit(2)          */ 


  /*-------------------------------------------------------------------------*/

  /* --- ADDRESS INFORMATON --- */

  pMsg->add2.flag = GET_N_BITS(1);
  if (pMsg->add2.flag == 0)                   /* Global TFI                  */
  {
    ret_code = grr_decode_ie_glob_tfi (&pMsg->add2.glob_tfi);

    if (ret_code == ccdOK)
      pMsg->add2.v_glob_tfi = TRUE;
    else
      return ret_code;
  }
  else
  {
    if (bitLen < 1)
      return ERR_MAND_ELEM_MISS;              

    pMsg->add2.v_flag2 = TRUE;

    pMsg->add2.flag2 = GET_N_BITS(1);
    if (pMsg->add2.flag2 == 0)                /* TLLI                        */
    {
      if (bitLen < 32)
        return ERR_MAND_ELEM_MISS;

      ret_code = grr_decode_ie_tlli (&pMsg->add2.tlli_value);

      if(ret_code == ccdOK)
        pMsg->add2.v_tlli_value = TRUE;             
      else
        return ret_code;
    }
    else
    {
      if (bitLen < 1)
        return ERR_MAND_ELEM_MISS;              

      bit = GET_N_BITS(1);
      if (bit == 0)                           /* TQI : bit(16)               */
      {
        if (bitLen < 16)
          return ERR_MAND_ELEM_MISS;              

        pMsg->add2.v_tqi = TRUE;             
                  h_byte = GET_N_BITS(8);
        pMsg->add2.tqi   = (h_byte << 8) + GET_N_BITS(8);
      }
      else 
      {
        return ERR_ADDR_INFO_PART;              
      }
    }
  }

  /*-------------------------------------------------------------------------*/

  /* --- TYPE OF ACK --- */

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  pMsg->ctrl_ack_type = GET_N_BITS(1);

  /* --- PADDING BITS --- */ /* -> currently not decoded and/or validated */

  return ccdOK;
}
/*
+------------------------------------------------------------------------------
| Function    : grr_decode_tbf_release_req
+------------------------------------------------------------------------------
| Description : For performance reasons this function replaces the call to the
|               function ccd_decodeMsg. If no error occured, the D_TBF_RELEASE 
|               message is decoded into _decodeCtrlMsg.
|
| Parameters  : buf    - pointer to beginning of the D_TBF_RELEASE IE
|               len    - lenght of the buffer in bits
|               off    - bit offset of the first bit in the buffer
|
| Returns     : ccdOK  - if no error is occured
|                      - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_tbf_release_req (UBYTE* buf, int off, int len)
{ 
  int    bit;                                    
  UBYTE  ret_code;

  MCAST(pMsg, D_TBF_RELEASE);

  /*-------------------------------------------------------------------------*/
  startBit = off;                             /* Initiate global data        */
  bitLen   = len;
  pBuf     = buf;

  memset (pMsg, 0, sizeof(T_D_TBF_RELEASE));  /* init destination struct     */

  /*-------------------------------------------------------------------------*/

  if (bitLen < 8)                             
    return ERR_MAND_ELEM_MISS;
  
  pMsg->msg_type  = GET_N_BITS(6);            /* MESSAGE_TYPE : bit(6)       */
  
  /* --- DISTRIBUTION CONTENTS --- */

  pMsg->page_mode = GET_N_BITS(2);            /* PAGE_MODE : bit(2)          */ 


  /*-------------------------------------------------------------------------*/

  /* --- ADDRESS INFORMATON --- */

  bit = GET_N_BITS(1);
  if (bit == 0)                               /* Global TFI                  */
  {
    ret_code = grr_decode_ie_glob_tfi (&pMsg->glob_tfi);

    if (ret_code NEQ ccdOK)
      return ret_code;
  }
  else
  {
    return ERR_ADDR_INFO_PART;              
  }

  /*-------------------------------------------------------------------------*/

  /* --- UPLINK/ DOWNLINK RELEASE VALUES --- */

  if (bitLen < 6)
    return ERR_MAND_ELEM_MISS;                

  pMsg->ul_release = GET_N_BITS(1);
  pMsg->dl_release = GET_N_BITS(1);
  pMsg->rel_cause  = GET_N_BITS(4);

  /* --- PADDING BITS --- */ /* -> currently not decoded and/or validated */

  return ccdOK;
}
/*
+------------------------------------------------------------------------------
| Function    : grr_decode_ts_reconfig
+------------------------------------------------------------------------------
| Description : For performance reasons this function replaces the call to the
|               function ccd_decodeMsg. If no error occured, the D_TS_RECONFIG
|               message is decoded into _decodeCtrlMsg.
|
| Parameters  : buf    - pointer to beginning of the D_TS_RECONFIG IE
|               len    - lenght of the buffer in bits
|               off    - bit offset of the first bit in the buffer
|
| Returns     : ccdOK  - if no error is occured
|                      - else CCD error code
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_decode_ts_reconfig (UBYTE* buf, int off, int len)
{ 
  int    i;
  int    bit;                                    
  UBYTE  ret_code;

  MCAST(pMsg, D_TS_RECONFIG);

  /*-------------------------------------------------------------------------*/
  startBit = off;                             /* Initiate global data        */
  bitLen   = len;
  pBuf     = buf;

  memset (pMsg, 0, sizeof(T_D_TS_RECONFIG));  /* init destination struct     */

  /*-------------------------------------------------------------------------*/

  if (bitLen < 8)                             
    return ERR_MAND_ELEM_MISS;
  
  pMsg->msg_type  = GET_N_BITS(6);            /* MESSAGE_TYPE : bit(6)       */
  
  /* --- DISTRIBUTION CONTENTS --- */

  pMsg->page_mode = GET_N_BITS(2);            /* PAGE_MODE : bit(2)          */ 

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ccdOK;                             

  /*-------------------------------------------------------------------------*/
  bit = GET_N_BITS(1);
  if(bit == 0)                                /* GLOBAL_TFI                  */
  {
    ret_code = grr_decode_ie_glob_tfi (&pMsg->glob_tfi);

    if (ret_code != ccdOK)
      return ret_code;
  }

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

#ifdef REL99
  pMsg->egprs_flag = GET_N_BITS(1);
  if (pMsg->egprs_flag == 1)
  {
    /*EGPRS is not supported. Just return */
    return ccdOK;
  }
#else
  bit = GET_N_BITS(1);
  if (bit == 1)                               /* 1 equals to Message Escape  */
    return ccdOK;                             /*   then we are ready         */
#endif



  /* When message egprs_flag is FALSE, the valid flags of all the 
   * corresponding conditional elements, is made TRUE here
   */
  pMsg->v_chan_coding_cmd = pMsg->v_gpta = pMsg->v_dl_rlc_mode = TRUE;
  pMsg->v_ctrl_ack = pMsg->v_dl_tn_alloc = pMsg->v_flag = TRUE;

#ifdef REL99
  pMsg->v_release_99 = TRUE;
#endif



  /*-------------------------------------------------------------------------*/
  if (bitLen < 2)
    return ERR_MAND_ELEM_MISS;                

  pMsg->chan_coding_cmd = GET_N_BITS(2);      /* CHANNEL_COD._COMMAND:bit(2) */

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* TIMING_ADVANCED_VALUE       */
  {
    if (bitLen < 6)
      return ERR_MAND_ELEM_MISS;                

    pMsg->gpta.v_ta_value = TRUE;
    pMsg->gpta.ta_value   = GET_N_BITS(6);
  }

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  pMsg->gpta.flag = GET_N_BITS(1);
  if (pMsg->gpta.flag == 1)                   /* UPLINK_TIMING_ADVANCED      */
  {
    if (bitLen < 7)
      return ERR_MAND_ELEM_MISS;                

    pMsg->gpta.v_ul_ta_index = TRUE;
    pMsg->gpta.ul_ta_index   = GET_N_BITS(4);

    pMsg->gpta.v_ul_ta_tn    = TRUE;
    pMsg->gpta.ul_ta_tn      = GET_N_BITS(3);
  }

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  pMsg->gpta.flag2 = GET_N_BITS(1);
  if (pMsg->gpta.flag2 == 1)                  /* DOWNLINK_TIMING_ADVANCED    */
  {
    if (bitLen < 7)
      return ERR_MAND_ELEM_MISS;                

    pMsg->gpta.v_dl_ta_index = TRUE;
    pMsg->gpta.dl_ta_index   = GET_N_BITS(4);

    pMsg->gpta.v_dl_ta_tn    = TRUE;
    pMsg->gpta.dl_ta_tn      = GET_N_BITS(3);
  }

  /*-------------------------------------------------------------------------*/
  if (bitLen < 2)
    return ERR_MAND_ELEM_MISS;      
              
  pMsg->dl_rlc_mode = GET_N_BITS(1);
  pMsg->ctrl_ack    = GET_N_BITS(1);

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;      

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* DOWNLINK_TFI_ASSIGNMENT     */
  {
    if (bitLen < 5)
      return ERR_MAND_ELEM_MISS;      

    pMsg->v_dl_tfi = TRUE;
    pMsg->dl_tfi   = GET_N_BITS(5);
  }

  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;      

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* UPLINK_DFI_ASSIGNMENT       */
  {
    if (bitLen < 5)
      return ERR_MAND_ELEM_MISS;      

    pMsg->v_ul_tfi = TRUE;
    pMsg->ul_tfi   = GET_N_BITS(5);
  }

  /*-------------------------------------------------------------------------*/
  if (bitLen < 8)
    return ERR_MAND_ELEM_MISS;      

  pMsg->dl_tn_alloc = GET_N_BITS(8);          /* DOWNLINK_TIMESLOT_ALLOCATION*/

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  bit = GET_N_BITS(1);
  if (bit == 1)                               /* Frequency Parameters IE     */
  {
    ret_code = grr_decode_ie_freq_par (&pMsg->freq_par);

    if (ret_code == ccdOK)
      pMsg->v_freq_par = TRUE;
    else
      return ret_code;
  }

  /*-------------------------------------------------------------------------*/
  if (bitLen < 1)
    return ERR_MAND_ELEM_MISS;                

  pMsg->flag = GET_N_BITS(1);
  if (pMsg->flag == 0)                        /* Dynamic Allocation struct   */
  {
    ret_code = grr_decode_ie_dyn_alloc_ts (&pMsg->dyn_alloc_ts);

    if (ret_code == ccdOK)                      
      pMsg->v_dyn_alloc_ts = TRUE;
    else
      return ret_code;
  }
  else                                        /* Fixed Allocation struct     */
  {
    if (bitLen < 1)
      return ERR_MAND_ELEM_MISS;                

    pMsg->f_alloc_re.flag = GET_N_BITS(1);
    if (pMsg->f_alloc_re.flag == 0)           /* UPLINK_TIMESLOT_ALLOCATION  */
    {
      if (bitLen < 8)
        return ERR_MAND_ELEM_MISS;                

      pMsg->f_alloc_re.v_ul_ts_alloc = TRUE;
      pMsg->f_alloc_re.ul_ts_alloc   = GET_N_BITS(8);
    }
    else                                      /* Power Control Parameters IE */
    {
      ret_code = grr_decode_ie_pwr_par (&pMsg->f_alloc_re.pwr_par);

      if (ret_code == ccdOK)
        pMsg->f_alloc_re.v_pwr_par = TRUE;
      else
        return ret_code;
    }

    if (bitLen < 4)
      return ERR_MAND_ELEM_MISS;                

    pMsg->f_alloc_re.final_alloc = GET_N_BITS(1);
    pMsg->f_alloc_re.dl_ctrl_ts  = GET_N_BITS(3);

    bit = GET_N_BITS(1);
    if (bit == 1)
    {
      if (bitLen < 6)
        return ERR_MAND_ELEM_MISS;

      pMsg->f_alloc_re.v_bts_pwr_ctrl       = TRUE;
      pMsg->f_alloc_re.bts_pwr_ctrl.p0      = GET_N_BITS(4);
      pMsg->f_alloc_re.bts_pwr_ctrl.mode    = GET_N_BITS(1);
      pMsg->f_alloc_re.bts_pwr_ctrl.pr_mode = GET_N_BITS(1);
    }

    if (bitLen < 1)
      return ERR_MAND_ELEM_MISS;              

    bit = GET_N_BITS(1);
    if (bit == 1)                               /* Measurement Mapping         */
    {                                       
      ret_code = grr_decode_ie_meas_map (&pMsg->f_alloc_re.meas_map);

      if (ret_code == ccdOK)
        pMsg->f_alloc_re.v_meas_map = TRUE;
      else
        return ret_code;
    }
                                                /* TBF Starting Time           */
    ret_code = grr_decode_ie_tbf_s_time (&pMsg->f_alloc_re.tbf_s_time);

    if (ret_code != ccdOK)
      return ret_code;

    bit = GET_N_BITS(1);
    if(bit == 0)
    {
      if (bitLen < 1)
        return ERR_MAND_ELEM_MISS;

      pMsg->f_alloc_re.flag2 = GET_N_BITS(1);
      if (pMsg->f_alloc_re.flag2 == 0) /* with lenght of Allocation Bitmap */
      {
        if (bitLen < 8)
          return ERR_MAND_ELEM_MISS;

        pMsg->f_alloc_re.block_struct.bl_o_bl_per = GET_N_BITS(1);
        pMsg->f_alloc_re.block_struct.a_map_len   = GET_N_BITS(7);

        if (bitLen < pMsg->f_alloc_re.block_struct.a_map_len)
          return ERR_MAND_ELEM_MISS;

        for (i=0; i<pMsg->f_alloc_re.block_struct.a_map_len; i++)
        {
          pMsg->f_alloc_re.block_struct.alloc_map[i] = GET_N_BITS(1);
        }

        pMsg->f_alloc_re.block_struct.c_alloc_map = i;
        pMsg->f_alloc_re.v_block_struct           = TRUE;
      }
      else                          /* without lenght of Allocation Bitmap */
      {                             /* Bitmap fills remainder of message   */
        pMsg->f_alloc_re.v_alloc_map = TRUE;

        if (bitLen > 127)
          pMsg->f_alloc_re.c_alloc_map = 127;
        else
          pMsg->f_alloc_re.c_alloc_map = bitLen;

        for (i=0; i<pMsg->f_alloc_re.c_alloc_map; i++)
        {
          pMsg->f_alloc_re.alloc_map[i] = GET_N_BITS(1);
        }
      }
    }
    else
    {
       /* Message Escape */
    }

    pMsg->v_f_alloc_re = TRUE;
  }

#ifdef REL99
  /* Updation of packet time slot reconf message decoding for R99 
   * extensions
   */
  pMsg->release_99 = GET_N_BITS(1);
  if(pMsg->release_99 == 1)
  {
    if(bitLen < 2)
      return ERR_MAND_ELEM_MISS;
    pMsg->v_p_ext_ta = TRUE;
    pMsg->p_ext_ta = GET_N_BITS(2);
  }
#endif

  return ccdOK;
}

/*
+------------------------------------------------------------------------------
| Function    : grr_ccd_error_handling
+------------------------------------------------------------------------------
| Description : The function grr_ccd_error_handling() ...
|
| Parameters  : entity_i - the CCD was called for this entity
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_ccd_error_handling ( UBYTE entity_i  )
{ 
  UBYTE result = DELETE_MESSAGE;
  USHORT parlist [MAX_ERR_PAR];
  UBYTE first_error; 

  TRACE_FUNCTION( "grr_ccd_error_handling" );


  memset (parlist, 0, sizeof (parlist));
  
  first_error = ccd_getFirstError (entity_i, parlist);
  
  switch (first_error)
  {
    
  case ERR_PATTERN_MISMATCH:      /* A spare pattern does not match with  */
    /* the specified content                */
    /* Error params[0] = bitposition        */
    {
      MCAST(ptr,D_DL_ASSIGN);
      result = ptr->msg_type;
    }
    break;
    
  default:
    /* SZML-GLBL/010 */
    TRACE_ERROR( "Ctrl-Message will be deleted" );
    break;
  }
  
  return(result);

} /* grr_ccd_error_handling() */


/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : grr_check_ptmsi
+------------------------------------------------------------------------------
| Description : The function grr_check_ptmsi() checks the ptmsi and returns 
|               TRUE if the ptmsi is correct.
|
| Parameters  : ptmsi_i - ptr to ptmsi buffer
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_check_ptmsi ( BUF_ptmsi * ptmsi_i )
{ 
  ULONG ptmsi;
  BOOL result = FALSE;

  TRACE_FUNCTION( "grr_check_ptmsi" );
  
  ptmsi = grr_buffer2ulong( ptmsi_i );

  /*
   * if the new_pmsi is invalid the old should be onsidered s invalid too
   */
  if(grr_data->db.ms_id.new_ptmsi NEQ GMMRR_TMSI_INVALID)
  {
    if(grr_data->db.ms_id.old_ptmsi EQ ptmsi
    OR
    grr_data->db.ms_id.new_ptmsi EQ ptmsi)
    {
      result= TRUE;
    };
  }
  else
  {
    TRACE_EVENT_P2("NO valid PTMSI IN GRR PRESENT !!! old_ptmsi=%lx  new_ptmsi=%lx"
                                                            ,grr_data->db.ms_id.old_ptmsi
                                                            ,grr_data->db.ms_id.new_ptmsi);  
  }
  if(!result)
  {
    TRACE_EVENT_P5("OLD PTMSI: %LX, DESTINATION PTMSI: %LX   len=%d off=%d  NEW PTMSI=%lx"
                   ,grr_data->db.ms_id.old_ptmsi
                   ,ptmsi
                   ,ptmsi_i->l_ptmsi
                   ,ptmsi_i->o_ptmsi
                   ,grr_data->db.ms_id.new_ptmsi);
    
    TRACE_EVENT_P8("PTMSI BUF:buf[0 .. 7]=  %x %x %x %x %x %x %x %x "
                   ,ptmsi_i->b_ptmsi[0]
                   ,ptmsi_i->b_ptmsi[1]
                   ,ptmsi_i->b_ptmsi[2]
                   ,ptmsi_i->b_ptmsi[3]
                   ,ptmsi_i->b_ptmsi[4]
                   ,ptmsi_i->b_ptmsi[5]
                   ,ptmsi_i->b_ptmsi[6]
                   ,ptmsi_i->b_ptmsi[7]);

  }

  return(result);
} /* grr_check_ptmsi() */



/*
+------------------------------------------------------------------------------
| Function    : grr_check_tmsi
+------------------------------------------------------------------------------
| Description : The function grr_check_tmsi() checks the tmsi and returns 
|               TRUE if the tmsi is correct.
|
| Parameters  : tmsi_i - ptr to tmsi buffer
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_check_tmsi ( BUF_tmsi_field * tmsi_i )
{ 
  ULONG tmsi;
  BOOL result = FALSE;

  TRACE_FUNCTION( "grr_check_tmsi" );
  
  tmsi = grr_buffer2ulong( (BUF_ptmsi*) tmsi_i );
  
  if(tmsi EQ grr_data->db.ms_id.tmsi) result = TRUE;

  return(result);

} /* grr_check_tmsi() */




/*
+------------------------------------------------------------------------------
| Function    : grr_check_all_tlli
+------------------------------------------------------------------------------
| Description : The function grr_check_all_tlli() checks the tlli with all TLLI
|               GMM has assigned and returns TRUE if the tlli is between them. 
|
| Parameters  : tlli_i - ptr to tlli buffer
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_check_all_tlli ( BUF_tlli_value * tlli_i )
{ 
  ULONG tlli;
  BOOL result = FALSE;

  TRACE_FUNCTION( "grr_check_all_tlli" );
  
  tlli = grr_buffer2ulong( (BUF_ptmsi*)tlli_i );

  /*
   * if the new_tlli is invalid the old should be onsidered s invalid too
   */
  if(grr_data->db.ms_id.new_tlli NEQ GMMRR_TLLI_INVALID)
  {
    if(grr_data->db.ms_id.new_tlli EQ tlli)
    {
      grr_data->db.ms_id.received_tlli = grr_data->db.ms_id.new_tlli;
      result = TRUE;
    }
    else if (grr_data->db.ms_id.old_tlli EQ tlli)
    {
      
      grr_data->db.ms_id.received_tlli = grr_data->db.ms_id.old_tlli;
      result = TRUE;
    };
  }

  if(result)
  {
    tc_cgrlc_enable_req(CGRLC_QUEUE_MODE_DEFAULT,CGRLC_RA_DEFAULT,FALSE,CGRLC_ENAC_NORMAL);
  }
  else
  {
    TRACE_EVENT_P5("OLD TLLI: %LX, DESTINATION TLLI: %LX   len=%d off=%d  NEW TLLI=%lx"
                   ,grr_data->db.ms_id.old_tlli
                   ,tlli
                   ,tlli_i->l_tlli_value
                   ,tlli_i->o_tlli_value
                   ,grr_data->db.ms_id.new_tlli);
    
    TRACE_EVENT_P8("TLLI BUF:buf[0 .. 7]=  %x %x %x %x %x %x %x %x "
                   ,tlli_i->b_tlli_value[0]
                   ,tlli_i->b_tlli_value[1]
                   ,tlli_i->b_tlli_value[2]
                   ,tlli_i->b_tlli_value[3]
                   ,tlli_i->b_tlli_value[4]
                   ,tlli_i->b_tlli_value[5]
                   ,tlli_i->b_tlli_value[6]
                   ,tlli_i->b_tlli_value[7]);

  }

  return(result);

} /* grr_check_all_tlli() */



/*
+------------------------------------------------------------------------------
| Function    : grr_buffer2ulong
+------------------------------------------------------------------------------
| Description : The function grr_buffer2ulong() copy a 32-Bit-Buffer in a ULONG
|               variable
|               
|               SZML-GLBL/002
|
| Parameters  : ptmsi - pointer to buffer that contains the 32bit for the ULONG 
|
+------------------------------------------------------------------------------
*/
GLOBAL ULONG grr_buffer2ulong ( BUF_ptmsi * ptmsi)
{
  ULONG ul;
 
  UBYTE l, dummy; 
  USHORT i, ii;
  UBYTE off1, off2;
  
  TRACE_FUNCTION( "grr_buffer2ulong" ); 
  
  ul= 0;
  
  l = (UBYTE)ptmsi->l_ptmsi;
  
  off1 = ptmsi->o_ptmsi / 8;
  off2 = ptmsi->o_ptmsi % 8;
  
  dummy = 0;
  dummy = ptmsi->b_ptmsi[off1] << off2;
  
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
      dummy = ptmsi->b_ptmsi[off1] >> (8-l);
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
      ul |= ptmsi->b_ptmsi[off1];
      l -= 8;
      if(l EQ 0)
        return ul;
    }
  }
  while(TRUE);
}






/*
+------------------------------------------------------------------------------
| Function    : grr_decode_rr
+------------------------------------------------------------------------------
| Description : The function grr_decode_rr() calls the function ccd_decodeMsg.
|               After the call the decoded Message is in _decodeCtrlMsg.
|               
|
| Parameters  : msg_ptr_i - pointer to buffer that should be decoded
|
+------------------------------------------------------------------------------
*/

GLOBAL UBYTE grr_decode_rr (   T_MSGBUF  *  msg_ptr_i )
{ 
  UBYTE result;


  TRACE_FUNCTION( "grr_decode_rr" );

  
  
  result = ccd_decodeMsg ( CCDENT_RR, 
                  DOWNLINK,
                  msg_ptr_i,
                  _decodedMsg, 
                  NOT_PRESENT_8BIT);


  if ( result EQ ccdError)
  {
    TRACE_ERROR( "grr_decode_rr - decoding of RR message failed" );

    result = grr_ccd_error_handling(CCDENT_RR);
  }
  else
  {
    MCAST(ptr, D_SYS_INFO_13); /* get one message from the RR message-base*/
    result = ptr->msg_type; /*  this is the type of the decoded message*/
  }
  
  return(result);
  
} /* grr_decode_rr() */


  
/*
+------------------------------------------------------------------------------
| Function    : grr_calc_new_poll_pos
+------------------------------------------------------------------------------
| Description : The function grr_calc_new_poll_pos() calculates the fn of the 
|               new poll position
|
| Parameters  : fn_i    - framenumber
|               rrbp_i  - relative position
|
+------------------------------------------------------------------------------
*/
GLOBAL ULONG grr_calc_new_poll_pos ( ULONG fn_i, UBYTE rrbp_i )
{ 
  ULONG result=0;
  TRACE_FUNCTION( "grr_calc_new_poll_pos" );

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

} /* grr_calc_new_poll_pos() */






/*
+------------------------------------------------------------------------------
| Function    : grr_encode_ctrl
+------------------------------------------------------------------------------
| Description : The function grr_encode_ctrl() build a T_SDU buffer that 
|               contains the encode Ctrl Block ready to transmit. 
|
| Parameters  : ULONG ptr_in_i - ptr to the input structure
|               ULONG ptr_out_i - ptr to begin of output buffer
|               UBYTE r_bit_i - value of r_bit
+------------------------------------------------------------------------------
*/
GLOBAL void grr_encode_ctrl ( UBYTE * ptr_in_i, 
                              T_MSGBUF * ptr_out_i, 
                              UBYTE r_bit_i)
{ 
  TRACE_FUNCTION( "grr_encode_ctrl" );

  ptr_out_i->buf[0] = grr_get_ul_ctrl_block_header( r_bit_i );
  ptr_out_i->o_buf  = BIT_UL_CTRL_BLOCK_MAC_HEADER;
  ptr_out_i->l_buf  = BIT_UL_CTRL_BLOCK_CONTENTS;

  ccd_codeMsg ( CCDENT_GRR, 
                UPLINK,
                ptr_out_i,
                ptr_in_i, 
                NOT_PRESENT_8BIT);

  ptr_out_i->l_buf += ptr_out_i->o_buf;
  ptr_out_i->o_buf  = 0;

} /* grr_encode_ctrl() */


/*
+------------------------------------------------------------------------------
| Function    : grr_save_persistence_level
+------------------------------------------------------------------------------
| Description : The function grr_save_persistence_level() saves the persistence 
|               level values to the data_base. 
|
| Parameters  : ptr2persistence_level_i - pointer to received data
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_save_persistence_level ( T_pers_lev * ptr2persistence_level_i )
{ 
  UBYTE i ;
  TRACE_FUNCTION( "grr_save_persistence_level" );


  for(i=0; i<4; i++)
  {
     psc_db->prach.pers_lev.plev[i] = ptr2persistence_level_i->plev[i];
  }

} /* grr_save_persistence_level() */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_db_mode
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL T_DB_MODE grr_get_db_mode( void )
{ 
  TRACE_FUNCTION( "grr_get_db_mode" );

  return( grr_data->sc_db_mode );

}/* grr_get_db_mode */

/*
+------------------------------------------------------------------------------
| Function    : grr_set_db_ptr
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_set_db_ptr( T_DB_MODE mode )
{ 
  UBYTE old_mode = grr_data->sc_db_mode;

  TRACE_FUNCTION( "grr_set_db_ptr" );

  switch( mode )
  {
    case( DB_MODE_INIT ):
      grr_data->sc_db_mode = (UBYTE)mode;
      
      psc_db  = &grr_data->sc_db_1;
      posc_db = NULL;
      prsc_db = NULL;
      pcr_db  = &grr_data->sc_db_2;
      break;

    case( DB_MODE_CC_REQ ):
      if( grr_data->sc_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_data->sc_db_mode = (UBYTE)mode;

        if( psc_db EQ &grr_data->sc_db_1 )
        {
          psc_db  = &grr_data->sc_db_2;
          posc_db = &grr_data->sc_db_1;
        }
        else
        {
          psc_db  = &grr_data->sc_db_1;
          posc_db = &grr_data->sc_db_2;
        }

        prsc_db = NULL;
        pcr_db  = NULL;
      }
      break;

    case( DB_MODE_SWAP ):
      {
        T_SC_DATABASE *db;

        if( posc_db EQ psc_db )
        {
          posc_db = pcr_db;
        }
        else if( posc_db EQ pcr_db )
        {
          posc_db = psc_db;
        }
        
        if( prsc_db EQ psc_db )
        {
          prsc_db = pcr_db;
        }
        else if( prsc_db EQ pcr_db )
        {
          prsc_db = psc_db;
        }

        db     = psc_db;
        psc_db = pcr_db;
        pcr_db = db;
      }
      break;

    case( DB_MODE_CC_ACC ):
      if( grr_data->sc_db_mode EQ DB_MODE_CC_REQ )
      {
        grr_data->sc_db_mode = (UBYTE)mode;

        if( psc_db EQ &grr_data->sc_db_1 )
        {
          pcr_db = &grr_data->sc_db_2;
        }
        else
        {
          pcr_db = &grr_data->sc_db_1;
        }
      }
/*
      else
      {
        TRACE_ERROR( "Database mode: !DB_MODE_CC_REQ -> DB_MODE_CC_ACC" );
      }
*/
      break;

    case( DB_MODE_CC_REJ ):
      if( grr_data->sc_db_mode EQ DB_MODE_CC_REQ )
      {
        grr_data->sc_db_mode = (UBYTE)mode;

        if( psc_db EQ &grr_data->sc_db_1 )
        {
          psc_db  = &grr_data->sc_db_2;
          prsc_db = &grr_data->sc_db_1;
          pcr_db  = &grr_data->sc_db_1;
        }
        else
        {
          psc_db  = &grr_data->sc_db_1;
          prsc_db = &grr_data->sc_db_2;
          pcr_db  = &grr_data->sc_db_2;
        }

        posc_db = NULL;
      }
/*
      else
      {
        TRACE_ERROR( "Database mode: !DB_MODE_CC_REQ -> DB_MODE_CC_REJ" );
      }
*/
      break;

    default:
      TRACE_ASSERT( mode EQ DB_MODE_INIT   OR
                    mode EQ DB_MODE_CC_REQ OR
                    mode EQ DB_MODE_CC_ACC OR
                    mode EQ DB_MODE_CC_REJ OR
                    mode EQ DB_MODE_SWAP      );
      break;
  }

  TRACE_EVENT_P6( "mode: %d -> %d, database: psc = %d, posc = %d, prsc = %d, pcr = %d",
                  old_mode, mode,
                  grr_get_db_num( psc_db  ), grr_get_db_num( posc_db ),
                  grr_get_db_num( prsc_db ), grr_get_db_num( pcr_db  ) );

}/* grr_set_db_ptr */

/*
+------------------------------------------------------------------------------
| Function    : grr_init_db_srvc_param
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void grr_init_db_srvc_param ( T_SC_DATABASE *db )
{ 
  T_SC_DATABASE *original_db = psc_db;

  TRACE_FUNCTION( "grr_init_db_srvc_param" );

  psc_db = db;
  
  ctrl_init_params( );

  psi_init( );

  pg_init_params( );

  psc_db = original_db;
  
  grr_set_pbcch( FALSE );

}/* grr_init_db_srvc_param */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_db_num
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE grr_get_db_num ( T_SC_DATABASE *db )
{ 
  UBYTE db_num;

  TRACE_FUNCTION( "grr_get_db_num" );

  if     ( db EQ NULL               ) db_num = 0;
  else if( db EQ &grr_data->sc_db_1 ) db_num = 1;
  else if( db EQ &grr_data->sc_db_2 ) db_num = 2;
  else                                db_num = 3;

  return( db_num );

}/* grr_get_db_num */

/*
+------------------------------------------------------------------------------
| Function    : grr_init_db
+------------------------------------------------------------------------------
| Description : The function grr_init_db initializes the database in GRR_DATA.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_init_db ( T_SC_DATABASE *db )
{
  TRACE_FUNCTION( "grr_init_db" );
 
  db->cell_info_for_gmm.access_status              = GPRS_ACCESS_BARRED; 
  db->cell_info_for_gmm.cell_info.cell_env.rai.rac = GMMRR_RAC_INVALID;
  db->gprs_attach_is_running                       = FALSE;
  db->non_drx_timer_running                        = FALSE;
  db->non_drx_timer                                = DRX_NO;
  db->nc2_non_drx_period_running                   = FALSE;

  grr_init_db_srvc_param( db );

#ifdef REL99
  db->network_rel = BSS_NW_REL_97;     /*Default Network Release 97 */
#endif

}/* grr_init_db */

/*
+------------------------------------------------------------------------------
| Function    : grr_init
+------------------------------------------------------------------------------
| Description : The function grr_init initializes the entity GRR
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_init ( void )
{ 
  TRACE_FUNCTION( "grr_init" );
  
  /* initialize all GRR data */
  grr_data = &grr_data_base;
  memset( grr_data, 0, sizeof( T_GRR_DATA ) );

  /* initialize the pointers to the databases */
  grr_set_db_ptr( DB_MODE_INIT );

  /* initialize the serving cell databases */
  grr_init_db( psc_db );
  grr_init_db( pcr_db );


  /* 
   * call of service init functions 
   */
  ctrl_init();
  tc_init();
  cpap_init(); 
  psi_init();
  pg_init();
  meas_init( IM_MODE_TRANSFER );
  cs_grr_init();
  gfp_init();

  ccd_register(CCD_REENTRANT);
  
  grr_init_ms_data( );

  grr_data->db.ms_id.old_ptmsi    = GMMRR_TMSI_INVALID;               /* No valid PTMSI*/
  grr_data->db.ms_id.new_ptmsi    = GMMRR_TMSI_INVALID;               /* No valid PTMSI*/
  grr_data->db.ms_id.old_tlli     = GMMRR_TLLI_INVALID;
  grr_data->db.ms_id.new_tlli     = GMMRR_TLLI_INVALID;

  /*
   * SZML-GLBL/003
   */  
  /*set ms capabilty*/
  grr_data->ms_cap[0].Rx   = 1;
  grr_data->ms_cap[0].Tx   = 1;
  grr_data->ms_cap[0].Sum  = 2;
  grr_data->ms_cap[0].Ttb  = 2;
  grr_data->ms_cap[0].Tra  = 4;  /* Tra value for all classes */

  grr_data->ms_cap[1].Rx   = 2;
  grr_data->ms_cap[1].Tx   = 1;
  grr_data->ms_cap[1].Sum  = 3;
  grr_data->ms_cap[1].Ttb  = 2;
  grr_data->ms_cap[1].Tra  = 3;

  grr_data->ms_cap[2].Rx   = 2;
  grr_data->ms_cap[2].Tx   = 2;
  grr_data->ms_cap[2].Sum  = 3;
  grr_data->ms_cap[2].Ttb  = 2;
  grr_data->ms_cap[2].Tra  = 3;

  grr_data->ms_cap[3].Rx   = 3;
  grr_data->ms_cap[3].Tx   = 1;
  grr_data->ms_cap[3].Sum  = 4;
  grr_data->ms_cap[3].Ttb  = 1;
  grr_data->ms_cap[3].Tra  = 3;

  grr_data->ms_cap[4].Rx   = 2;
  grr_data->ms_cap[4].Tx   = 2;
  grr_data->ms_cap[4].Sum  = 4;
  grr_data->ms_cap[4].Ttb  = 1;
  grr_data->ms_cap[4].Tra  = 3;

  grr_data->ms_cap[5].Rx   = 3;
  grr_data->ms_cap[5].Tx   = 2;
  grr_data->ms_cap[5].Sum  = 4;
  grr_data->ms_cap[5].Ttb  = 1;
  grr_data->ms_cap[5].Tra  = 3;

  grr_data->ms_cap[6].Rx   = 3;
  grr_data->ms_cap[6].Tx   = 3;
  grr_data->ms_cap[6].Sum  = 4;
  grr_data->ms_cap[6].Ttb  = 1;
  grr_data->ms_cap[6].Tra  = 3;

  grr_data->ms_cap[7].Rx   = 4;
  grr_data->ms_cap[7].Tx   = 1;
  grr_data->ms_cap[7].Sum  = 5;
  grr_data->ms_cap[7].Ttb  = 1;
  grr_data->ms_cap[7].Tra  = 2;

  grr_data->ms_cap[8].Rx   = 3;
  grr_data->ms_cap[8].Tx   = 2;
  grr_data->ms_cap[8].Sum  = 5;
  grr_data->ms_cap[8].Ttb  = 1;
  grr_data->ms_cap[8].Tra  = 2;

  grr_data->ms_cap[9].Rx   = 4;
  grr_data->ms_cap[9].Tx   = 2;
  grr_data->ms_cap[9].Sum  = 5;
  grr_data->ms_cap[9].Ttb  = 1;
  grr_data->ms_cap[9].Tra  = 2;

  grr_data->ms_cap[10].Rx   = 4;
  grr_data->ms_cap[10].Tx   = 3;
  grr_data->ms_cap[10].Sum  = 5;
  grr_data->ms_cap[10].Ttb  = 1;
  grr_data->ms_cap[10].Tra  = 2;

  grr_data->ms_cap[11].Rx   = 4;
  grr_data->ms_cap[11].Tx   = 4;
  grr_data->ms_cap[11].Sum  = 5;
  grr_data->ms_cap[11].Ttb  = 1;
  grr_data->ms_cap[11].Tra  = 2;


  memset(&grr_data->ta_params, 0xFF, sizeof(T_TA_PARAMS));
  grr_data->ta_params.ta_valid = FALSE;
  grr_data->cc_running         = FALSE;

  grr_data->uplink_tbf.access_type = CGRLC_AT_NULL; /* NO CELL UPDATE NEED */

  grr_data->test_mode = CGRLC_NO_TEST_MODE;

  grr_data->l1_del_tbf_start_fn = GRR_INVALID_FN; 

  grr_set_pbcch( FALSE );

  grr_data->cell_res_status = TRUE;

#ifdef REL99

#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
  psc_db->band_indicator = NOT_PRESENT_8BIT;
#endif

  /* Initialize SGSN release to unknown value at Power ON */
  psc_db->sgsn_rel = PS_SGSN_UNKNOWN;

  cl_nwrl_set_sgsn_release(psc_db->sgsn_rel);

  /* Initialize the BSS release to R97 */
  psc_db->network_rel = BSS_NW_REL_97;
#endif

} /* grr_init() */

/*
+------------------------------------------------------------------------------
| Function    : grr_is_pbcch_present
+------------------------------------------------------------------------------
| Description : grr_is_pbcch_present returns whether the PBCCH is present or not
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/

GLOBAL BOOL grr_is_pbcch_present (void)
{
  
  return (psc_db->pbcch.bcch.pbcch_present);
}/*grr_is_pbcch_present*/





/*
+------------------------------------------------------------------------------
| Function    : grr_random_value
+------------------------------------------------------------------------------
| Description : This function generates a random value between 0 and max_i-1           
|               The function is current implemented by a random sequence.
|               It may replace by a other generation method in the future,
|               i.e. last bits of framenumber, systemtime or powervalue etc..
|
| Parameters  : max_i -  return_value is between 0 and (max_i-1)
|                        maximum value 256
|
+------------------------------------------------------------------------------
*/
GLOBAL ULONG grr_random_value (UBYTE index, ULONG max_i)
{
  /* The indexing is done to avoid unpredictable distortion of uniform */
  /* probability distribution of random values delivered to a specific */
  /* caller */
  static ULONG next[3] = {1,1,1};
  ULONG return_value;
#ifdef _TARGET_  
  static BOOL initflag[3] = {FALSE,FALSE,FALSE};
  T_TIME time_val;
#endif

  TRACE_FUNCTION( "grr_random_value" );
  
  if(index > 2)
  {
    TRACE_ERROR("Invalid inex in random value generation function");
    return(0);
  }

#ifdef _SIMULATION_
  if (grr_data->tc.res_random)
  {
    UBYTE i;
    TRACE_EVENT( "grr_data->tc.res_random is set" );
    for (i=0; i<3; i++)
    {
      next[i] = 1;
    }
    grr_data->tc.res_random = 0;
  }
#endif


#ifdef _TARGET_
  /* on target each random value sequence is initialized on its first usage */
  /* with system time to get different sequences for each mobile and each   */
  /* caller */
  if(initflag[index] EQ FALSE)
  {
    vsi_t_time (VSI_CALLER &time_val);
    next[index] = (ULONG)time_val;
    initflag[index] = TRUE;
  }
#endif

  next[index] = next[index] * 1103515245 + 12345;
  return_value = (next[index]/65536) % (max_i);

#ifndef _TARGET_
  TRACE_EVENT_P3("random index: %d max: %d value: %d",index, max_i, return_value);
#endif /* _TARGET_ */

  return(return_value);
}


/*
+------------------------------------------------------------------------------
| Function    : grr_decode_tbf_start_abs
|------------------------------------------------------------------------------
| Description : The function grr_decode_tbf_start_abs() translates the TBF- 
|               Starting-Time-Absolute into full frame number. Therefore the 
|               received frame number is needed in grr_data->dl_fn !! 
|
| Parameters  : ptr2tbf_abs - pointer to the tbf_abs structure to be decoded
|
+------------------------------------------------------------------------------
*/
GLOBAL ULONG grr_decode_tbf_start_abs(T_abs *ptr2tbf_abs)
{ 
  ULONG var1,var2,var3,result;
  UBYTE var4;
  TRACE_FUNCTION( "grr_decode_tbf_start_abs" );

  var3 = (ULONG)ptr2tbf_abs->t3;
  var2 = (ULONG)ptr2tbf_abs->t2;
  var1 = (ULONG)ptr2tbf_abs->t1;
        
  result = (51 * ((26+var3 - var2)%26) + var3 + 1326 * var1);
  /* see GSM 04.08 v 6.4.2 section 10.5.2.38 */
  /* T_abs delivers only the fn modulo 42432 so it has to be adjusted to the */
  /* current fn.*/
  result += (grr_data->dl_fn-(grr_data->dl_fn%42432));
  if(result < grr_data->dl_fn)
  {
    if((result + 10808) < grr_data->dl_fn)
    {
      result += 42432;
    }
  }
  else if(result > (grr_data->dl_fn + 31623))
  {
    if(result < 42432)
      result += 2715648;
    result -= 42432;
  }
  /* fn has to be aligned to first tdma frame in a block */
  var4 = (UBYTE)(result%13);
  if(var4 > 8)
    result += (13 - var4);
  else if(var4 > 4)
    result += (8 - var4);
  else if(var4 > 0)
    result += (4 - var4);
  if(FN_MAX <= result)
    result %= FN_MAX;
  return result;
}  /* grr_decode_tbf_start_abs */



/*
+------------------------------------------------------------------------------
| Function    : grr_decode_tbf_start_rel
|------------------------------------------------------------------------------
| Description : The function grr_decode_tbf_start_rel() translates the TBF- 
|               Starting-Time-Relative into full frame number. Therefore the 
|               received frame number is needed in start_fn !! 
|
| Parameters  : rel_pos - number in blocks added to current framenuber
|
+------------------------------------------------------------------------------
*/
GLOBAL ULONG grr_decode_tbf_start_rel(ULONG start_fn, USHORT rel_pos)
{ 
  ULONG result;

  TRACE_FUNCTION( "grr_decode_tbf_start_rel" );
  
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
}  /* grr_decode_tbf_start_rel */



/*
+------------------------------------------------------------------------------
| Function    : grr_calc_nr_of_set_bits
+------------------------------------------------------------------------------
| Description : this function calculates the number of set bits 
|               (for example timeslots) form value
|
| Parameters  : value - input value
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE grr_calc_nr_of_set_bits (  UBYTE value  )
{ 
  UBYTE tx_slots=0;
  UBYTE mask=128;
  UBYTE i;

  TRACE_FUNCTION( "grr_calc_nr_of_set_bits" );
        
  for(i=0; i<=7; i++)
  {
    if(value & mask)
      tx_slots++;
    mask>>=1;
  }
  /*
  if(tx_slots > 1)
  {
    TRACE_EVENT_P1("MULTISLOT: %d timeslots assigned ",tx_slots);
  }
  */
  return tx_slots;
} /* grr_calc_nr_of_set_bits() */

/*
+------------------------------------------------------------------------------
| Function    : grr_check_dist
+------------------------------------------------------------------------------
| Description : The function grr_check_dist() checks if high_i is bigger/equal 
|               than low_i(modulo calculation). 
|               The return value is true, if high_i is equal to low_i or 
|               bigger than low_i.
| Parameters  : high_i - expected high value
|               low_i  - expected low value
|               dist_i - max. allowed distance between high_i and low_i
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_check_dist ( ULONG high_i, ULONG low_i, ULONG dist_i)
{
  BOOL  result = FALSE;
  ULONG  real_dist;

  TRACE_FUNCTION( "grr_check_dist" );
    
  if (high_i >= low_i)
    real_dist = high_i - low_i;
  else
    real_dist = high_i + (FN_MAX-low_i);

  if (real_dist <= dist_i )
  {
     result = TRUE;
  }
  return result;
} /* grr_check_dist() */

/*
+------------------------------------------------------------------------------
| Function    : handle_ms_cap
+------------------------------------------------------------------------------
| Description : The function handle_ms_cap() checks if the ms capability is 
|               fulfilled or not 
| Parameters  : -
+------------------------------------------------------------------------------
*/
GLOBAL BOOL handle_ms_cap (UBYTE msg_type)
{
  UBYTE ms_class_index, dl_mask, ul_mask;
  BOOL check_ul;
  BOOL check_dl;

  TRACE_FUNCTION( "handle_ms_cap" );

  /*
   * use current slot mask if now new is assigned
   *
   */
  if(grr_data->downlink_tbf.ts_usage)
  {
    dl_mask = grr_data->downlink_tbf.ts_usage;
  }
  else
  {
    dl_mask = grr_data->downlink_tbf.ts_mask;
  }

    
  if(grr_data->uplink_tbf.ts_usage)
  {
    ul_mask = grr_data->uplink_tbf.ts_usage;
  }
  else
  {
    ul_mask = grr_data->uplink_tbf.ts_mask;
  }

  ms_class_index = grr_get_gprs_ms_class( ) - 1;

  switch(msg_type)
  {
    case UL_ASSIGNMENT:
      /* 
       * new uplink assignment received
       */
      check_ul = TRUE;
      if(grr_data->tbf_type EQ CGRLC_TBF_MODE_DL   OR
         grr_data->tbf_type EQ CGRLC_TBF_MODE_DL_UL  )
      {
        check_dl = TRUE;
      }
      else
      {
        check_dl = FALSE;
      }
      break;
    case DL_ASSIGNMENT:
      /* 
       * new uplink assignment received
       */
      check_dl = TRUE;
      if(grr_data->tbf_type EQ CGRLC_TBF_MODE_UL  OR
         grr_data->tbf_type EQ CGRLC_TBF_MODE_DL_UL   )
      {
        check_ul = TRUE;
      }
      else
      {
        check_ul = FALSE;
      }
      break;
    case UL_DL_ASSIGNMENT:
      /* 
       * new uplink and downlink assignment received
       */
      check_ul = TRUE;
      check_dl = TRUE;
      break;
    default:
      {
        TRACE_ERROR( "unexpected msg_type" );
        return FALSE;
      }
  }
  if (check_ul  AND
      ((grr_data->uplink_tbf.nts > grr_data->ms_cap[ms_class_index].Tx ) OR (!grr_data->uplink_tbf.nts)))
  {
    TRACE_ERROR( "uplink nts is bigger than allowed ms capability" );

    TRACE_EVENT_P8("MS CLASS ERROR 1: ul_mask=%x ul_nts=%d dl_mask=%x dl_nts=%d ms_class=%d Tx=%d Rx=%d sum=%d"
                                                                ,ul_mask
                                                                ,grr_data->uplink_tbf.nts
                                                                ,dl_mask
                                                                ,grr_data->downlink_tbf.nts
                                                                ,ms_class_index+1
                                                                ,grr_data->ms_cap[ms_class_index].Tx
                                                                ,grr_data->ms_cap[ms_class_index].Rx
                                                                ,grr_data->ms_cap[ms_class_index].Sum);
    return FALSE;
  }
  if (check_dl AND
      ((grr_data->downlink_tbf.nts > grr_data->ms_cap[ms_class_index].Rx) OR (!grr_data->downlink_tbf.nts)))
  {
    TRACE_ERROR( "downlink nts is bigger than allowed ms capability" );

    TRACE_EVENT_P8("MS CLASS ERROR 2:ul_mask=%x ul_nts=%d dl_mask=%x dl_nts=%d ms_class=%d Tx=%d Rx=%d sum=%d"
                                                                ,ul_mask
                                                                ,grr_data->uplink_tbf.nts
                                                                ,dl_mask
                                                                ,grr_data->downlink_tbf.nts
                                                                ,ms_class_index+1
                                                                ,grr_data->ms_cap[ms_class_index].Tx
                                                                ,grr_data->ms_cap[ms_class_index].Rx
                                                                ,grr_data->ms_cap[ms_class_index].Sum);
    return FALSE;
  }

  if (check_dl AND check_ul )
  {
    UBYTE help;

    if (((grr_data->uplink_tbf.nts + grr_data->downlink_tbf.nts)  > grr_data->ms_cap[ms_class_index].Sum) OR
         (!grr_data->uplink_tbf.nts) OR
         (!grr_data->downlink_tbf.nts))
    {
      TRACE_ERROR( "Sum bigger than allowed ms capability" );

      TRACE_EVENT_P8("MS CLASS ERROR 3:ul_mask=%x ul_nts=%d dl_mask=%x dl_nts=%d ms_class=%d Tx=%d Rx=%d sum=%d"
                                                                  ,ul_mask
                                                                  ,grr_data->uplink_tbf.nts
                                                                  ,dl_mask
                                                                  ,grr_data->downlink_tbf.nts
                                                                  ,ms_class_index+1
                                                                  ,grr_data->ms_cap[ms_class_index].Tx
                                                                  ,grr_data->ms_cap[ms_class_index].Rx
                                                                  ,grr_data->ms_cap[ms_class_index].Sum);
      return FALSE;
    }

    help = grr_calculate_Ttb(ul_mask,dl_mask);
    if (help < grr_data->ms_cap[ms_class_index].Ttb)
    {
      TRACE_ERROR("Ttb calculated not equal to the one of the multislot class ");
      TRACE_EVENT_P5("ul_mask=%x dl_mask=%x  ms_class=%d calc_ttb=%d ms_class_TTB=%d "
                                                                ,ul_mask
                                                                ,dl_mask
                                                                ,ms_class_index+1
                                                                ,help
                                                                ,grr_data->ms_cap[ms_class_index].Ttb);
      return FALSE;
    }
    help = grr_calculate_Tra(ul_mask,dl_mask);
    if ( help < grr_data->ms_cap[ms_class_index].Tra )
    {
      TRACE_ERROR("Tra calculated not equal to the one of the multislot class ");
      TRACE_EVENT_P5("ul_mask=%x dl_mask=%x  ms_class=%d calc_tra=%d ms_class_tra=%d "
                                                                ,ul_mask
                                                                ,dl_mask
                                                                ,ms_class_index+1
                                                                ,help
                                                                ,grr_data->ms_cap[ms_class_index].Tra);
      return FALSE;
    }
  }
  return TRUE;
} /* handle_ms_cap() */

/*
+------------------------------------------------------------------------------
| Function    : grr_copy_em1_struct
+------------------------------------------------------------------------------
| Description : The function grr_copy_em1_struct copies the em1 struct 
|               parameters.
|
| Parameters  : db_em1      - em1 structure in the GRR database
|               ext_em1     - em1 structure in the EXT measurement parameter
|               cpy_prm_set - indicates whether the new extended measurement 
|                             parameter overwrite the present ones due to a 
|                             messsage with higher sequence number
|
+------------------------------------------------------------------------------
*/
LOCAL void grr_copy_em1_struct( T_XMEAS_EM1 *db_em1,
                                T_em1       *ext_em1,
                                BOOL         cpy_prm_set,
                                UBYTE       *start_ext_lst_idx,
                                UBYTE       *stop_ext_lst_idx )
{
  UBYTE  i, j, m, n;

  TRACE_FUNCTION("grr_copy_em1_struct");

  if( cpy_prm_set )
  {
    /*
     * Copy EM1 struct parameters
     */
    db_em1->param.reporting_type = REP_TYPE_1;
    if(ext_em1->flag)
    {
      /*
       * check ext_reporting_type
       */
      switch(ext_em1->xrep_type)
      {
        case XREP_TYPE1:    /* 00 */
          db_em1->param.reporting_type = REP_TYPE_1;
          break;
        case XREP_TYPE2:    /* 01 */
          db_em1->param.reporting_type = REP_TYPE_2;
          db_em1->param.ncc_permitted  = ext_em1->ncc_permitted;
          break;
        case XREP_TYPE3:    /* 10 */
          db_em1->param.reporting_type = REP_TYPE_3;
          if(ext_em1->v_int_freq)
          {
            db_em1->param.int_frequency = ext_em1->int_freq;
          }
          else
          {
            db_em1->param.int_frequency = NOT_SET;
          }
          break;
        case XREP_RESERVED: /* 11 */
          db_em1->param.reporting_type = REP_TYPE_RES;
          break;
      }
    }

    /*
     * Check ext reporting period
     */
    db_em1->param.reporting_period = EXT_REP_PER_DEFAULT;
    if(ext_em1->v_xrep_per)
    {
      db_em1->param.reporting_period = ext_em1->xrep_per;
    }
  }

  /*
   * Copy extended frequency list Parameters
   */
  if(db_em1->list.number < RRGRR_MAX_ARFCN_EXT_MEAS)
  {
    /*
     * copy first EXT Frequency list structure
     */
    j                    = db_em1->list.number;
    *start_ext_lst_idx   = db_em1->list.number;
    db_em1->list.freq[j] = ext_em1->xfreq_list.start_freq;
    j++;

    /*
     * check remaining frequencies
     */
    for( i = 0;
         i < ext_em1->xfreq_list.nr_freq AND j < RRGRR_MAX_ARFCN_EXT_MEAS;
         i++ )
    {
      db_em1->list.freq[j] = 
        ( db_em1->list.freq[j-1] + ext_em1->xfreq_list.freq_diff_struct[i].freq_diff ) % 1024;
      j++;
    }

    /*
     * check whether EXT Frequency list structure is repeated
     */
    if(ext_em1->v_xfreq_list2)
    {
      for( m = 0; 
           m < ext_em1->c_xfreq_list2 AND j < RRGRR_MAX_ARFCN_EXT_MEAS;
           m++ )
      {
        db_em1->list.freq[j] = ext_em1->xfreq_list2[m].start_freq;
        j++;

        for( n = 0;
             n < ext_em1->xfreq_list2[m].nr_freq AND j < RRGRR_MAX_ARFCN_EXT_MEAS; 
             n++ )
        {
          db_em1->list.freq[j] =
            ( db_em1->list.freq[j-1] + ext_em1->xfreq_list2[m].freq_diff_struct[n].freq_diff ) % 1024;
          j++;
        }
      }
    }
    db_em1->list.number = j;
    *stop_ext_lst_idx   = db_em1->list.number - 1;
  }
  else
  {
    TRACE_ERROR("grr_copy_em1_struct EXT frequency list full");
  }
}/* grr_copy_em1_struct*/

/*
+------------------------------------------------------------------------------
| Function    : grr_prcs_xmeas_struct
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : db_xmeas    - extended measurement parameter in the GRR database
|               air_xmeas   - extended measurement parameter of the air interface
|                             message
|               cpy_prm_set - indicates whether the new extended measurement 
|                             parameter overwrite the present ones due to a 
|                             messsage with higher sequence number
|               new_idx     - sequence number of the message
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_prcs_xmeas_struct ( T_XMEAS     *db_xmeas, 
                                    T_xmeas_par *air_xmeas,
                                    BOOL         cpy_prm_set,
                                    UBYTE        new_inst_idx,
                                    UBYTE       *start_ext_lst_idx,
                                    UBYTE       *stop_ext_lst_idx )
{ 
  TRACE_FUNCTION( "grr_prcs_xmeas_struct" );
  
  /*
   * check ext meas order parameters:
   */
  switch(air_xmeas->xmeas_order)
  {
    case XMEAS_EM1:      /* 01 */
      db_xmeas->em_order_type = EM_EM1;
  
      if( cpy_prm_set )
      {
        db_xmeas->idx = new_inst_idx;
      }

      grr_copy_em1_struct( &db_xmeas->em1, &air_xmeas->em1, cpy_prm_set, 
                           start_ext_lst_idx, stop_ext_lst_idx );
      break;
    case XMEAS_RESET:    /* 11 */
      db_xmeas->em_order_type = EM_RESET;
      break;
    case XMEAS_EM0:      /* 00 */
    case XMEAS_RESERVED: /* 10 it is reseved but shall be interpreted as EM0 by the receiver */
      db_xmeas->em_order_type = EM_EM0;
      break;
  }
}/* grr_prcs_xmeas_struct */

/*
+------------------------------------------------------------------------------
| Function    : grr_init_xmeas_struct
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : xmeas  - pointer to extended measurement parameter
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_init_xmeas_struct( T_XMEAS *xmeas )
{ 
  TRACE_FUNCTION( "grr_init_xmeas_struct" );
  
  xmeas->idx                     = NOT_SET;
  xmeas->em_order_type           = EM_EMPTY;
  xmeas->em1.list.number         = 0;
  xmeas->em1.param.int_frequency = NOT_SET;
}/* grr_init_xmeas_struct */

/*
+------------------------------------------------------------------------------
| Function    : grr_prcs_nc_freq_list
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : db_list    - NC frequency list in the GRR database
|               v_air_list - valid flag for NC frequency list of the air
|                            interface message
|               air_list   - NC frequency list of the air interface message
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_prcs_nc_freq_list ( T_NC_LIST       *nc_list,
                                    T_NC_RFREQ_LIST *rfreq_list,
                                    BOOL             v_air_list,
                                    T_nc_freq_list  *air_list,
                                    T_INFO_TYPE      type,
                                    UBYTE            instance )
{ 
  UBYTE i, j; /* used for counting */

  TRACE_FUNCTION( "grr_prcs_nc_freq_list" );

  if( v_air_list EQ TRUE )
  {
    if( air_list->v_list_rf EQ TRUE )
    {
      /* process the list of removed frequencies */
      for( i = 0;
           i                  < air_list->list_rf.c_rfreq_index AND
           rfreq_list->number < MAX_NR_OF_NCELL; 
           i++ )
      {
        rfreq_list->idx[rfreq_list->number] = air_list->list_rf.rfreq_index[i];
        
        rfreq_list->number++;
      }      

      if( i < air_list->list_rf.c_rfreq_index )
      {
        TRACE_ERROR( "grr_prcs_nc_freq_list removed frequency list full" );
      }
    }

    if( air_list->v_list_af EQ TRUE )
    {
      UBYTE  number    = 0;
      USHORT last_freq;

      /* process the list of added frequencies */
      for( i = 0; i < air_list->c_list_af; i++ )
      {
        last_freq = 0;

        if( grr_store_cs_param
              ( nc_list,
                air_list->list_af[i].v_cs_par,
                &air_list->list_af[i].cs_par,
                type,
                instance,
                &number,
                &last_freq,
                air_list->list_af[i].start_freq,
                air_list->list_af[i].bsic ) EQ FALSE )
        {
          TRACE_EVENT( "grr_prcs_nc_freq_list: NC_MS list full" );

          return( v_air_list );
        }
        
        for( j = 0; j < air_list->list_af[i].nr_freq; j++ )
        {
          if( grr_store_cs_param
                ( nc_list,
                  air_list->list_af[i].afreq_s[j].v_cs_par,
                  &air_list->list_af[i].afreq_s[j].cs_par,
                  type,
                  instance,
                  &number,
                  &last_freq,
                  air_list->list_af[i].afreq_s[j].freq_diff_struct.freq_diff,
                  air_list->list_af[i].afreq_s[j].bsic ) EQ FALSE )
          {
            TRACE_EVENT( "grr_prcs_nc_freq_list: NC_MS list full" );

            return( v_air_list );
          }
        }
      }      
    }
  }

  return( v_air_list );
}/* grr_prcs_nc_freq_list */

/*
+------------------------------------------------------------------------------
| Function    : grr_prcs_nc_freq_final
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_prcs_nc_freq_final ( T_NC_LIST       *dest_list,
                                     T_NC_RFREQ_LIST *dest_rfreq,
                                     BOOL            *v_src_list_rfreq,
                                     T_NC_LIST       *src_list,
                                     T_NC_RFREQ_LIST *src_rfreq )
{ 
  BOOL  is_pbcch_present = grr_is_pbcch_present( );
  BOOL  is_identical;
  UBYTE i;

  TRACE_FUNCTION( "grr_prcs_nc_freq_final" );

  if( *v_src_list_rfreq EQ TRUE )
  {
    *v_src_list_rfreq = FALSE;

    /*
     * The change mark values should be increased in case 
     * at least one parameter has changed
     */
    if( dest_list->number  EQ src_list->number  AND
        dest_rfreq->number EQ src_rfreq->number     )
    {
      is_identical = TRUE;
    
      for( i = 0; i < dest_list->number AND is_identical; i++ )
      {
        if( is_pbcch_present )
        {
          is_identical =
            ( dest_list->info[i].arfcn EQ src_list->info[i].arfcn AND
              dest_list->info[i].bsic  EQ src_list->info[i].bsic      );
        }
        else
        {
          is_identical =
            ( dest_list->info[i].arfcn EQ src_list->info[i].arfcn );
        }
      }

      for( i = 0; i < dest_rfreq->number AND is_identical; i++ )
      {
        is_identical = ( dest_rfreq->idx[i] EQ src_rfreq->idx[i] );
      }
    }
    else
    {
      is_identical = FALSE;
    }

    if( !is_identical )
    {
      dest_list->chng_mrk.curr++;
    }

    /* copy remaining parameters except change mark */
    dest_list->number = src_list->number;

    memcpy( dest_list->info, src_list->info,
            sizeof( T_ncell_info ) * src_list->number );

    memcpy( dest_rfreq, src_rfreq, sizeof( T_NC_RFREQ_LIST ) );
  }
  else
  {
    if( dest_list->number NEQ 0 OR dest_rfreq->number NEQ 0 )
    {
      dest_list->number  = 0;
      dest_rfreq->number = 0;

      dest_list->chng_mrk.curr++;
    }
  }
}/* grr_prcs_nc_freq_final */

/*
+------------------------------------------------------------------------------
| Function    : grr_prcs_nc_param_struct
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : db_nc        - NC measurement parameter in the GRR database
|               air_nc       - NC measurement parameter of the air interface
|                              message
|               new_idx      - sequence number of the message
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_prcs_nc_param_struct ( T_NC_PARAM    *db_nc,
                                       T_nc_meas_par *ext_nc,
                                       UBYTE          new_idx )
{ 
  T_NC_ORDER ctrl_order;  /* NETWORK_CONTROL_ORDER */
  UBYTE      non_drx_per; /* NC_NON_DRX_PERIOD     */
  UBYTE      rep_per_i;   /* NC_REPORTING_PERIOD_I */
  UBYTE      rep_per_t;   /* NC_REPORTING_PERIOD_T */

  TRACE_FUNCTION( "grr_prcs_nc_param_struct" );

  /*
   * Store the network control order value in a temporary buffer
   */
  switch( ext_nc->ctrl_order )
  {
    case NCMEAS_NC0   : ctrl_order = NC_NC0;   break;
    case NCMEAS_NC1   : ctrl_order = NC_NC1;   break;
    case NCMEAS_NC2   : ctrl_order = NC_NC2;   break;
    case NCMEAS_RESET : 
    default           : ctrl_order = NC_RESET; break;
  }
 
  /*
   * Store the remaining parameters in temporary buffers
   */
  if(ext_nc->v_nc_meas_per)
  {
    non_drx_per = ext_nc->nc_meas_per.non_drx_per;
    rep_per_i   = ext_nc->nc_meas_per.rep_per_i;
    rep_per_t   = ext_nc->nc_meas_per.rep_per_t;
  }
  else
  {
    /*
     * Use default values, see 04.60 PSI5 Information Element Details
     */
    non_drx_per = NC_NON_DRX_PER_DEFAULT;
    rep_per_i   = NC_REP_PER_I_DEFAULT;
    rep_per_t   = NC_REP_PER_T_DEFAULT;
  }

  /*
   * The change mark values should be increased in case 
   * at least one parameter has changed
   */
  if( db_nc->ctrl_order  NEQ ctrl_order  OR
      db_nc->non_drx_per NEQ non_drx_per OR
      db_nc->rep_per_i   NEQ rep_per_i   OR
      db_nc->rep_per_t   NEQ rep_per_t      )
  {
    db_nc->chng_mrk.curr++;
  }

  db_nc->idx         = new_idx;
  db_nc->ctrl_order  = ctrl_order;
  db_nc->non_drx_per = non_drx_per;
  db_nc->rep_per_i   = rep_per_i;
  db_nc->rep_per_t   = rep_per_t;
}/* grr_prcs_nc_param_struct */

/*
+------------------------------------------------------------------------------
| Function    : grr_prcs_nc_param_final
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_prcs_nc_param_final ( T_NC_PARAM *dest_nc, 
                                      BOOL       *v_src_nc,
                                      T_NC_PARAM *src_nc )
{ 
  TRACE_FUNCTION( "grr_prcs_nc_param_final" );

  if( *v_src_nc EQ TRUE )
  {
    *v_src_nc = FALSE;

    /*
     * The change mark values should be increased in case 
     * at least one parameter has changed
     */
    if( dest_nc->ctrl_order  NEQ src_nc->ctrl_order  OR
        dest_nc->non_drx_per NEQ src_nc->non_drx_per OR
        dest_nc->rep_per_i   NEQ src_nc->rep_per_i   OR
        dest_nc->rep_per_t   NEQ src_nc->rep_per_t      )
    {
      dest_nc->chng_mrk.curr++;
    }

    /* Copy remaining parameters except change mark */
    dest_nc->idx         = src_nc->idx;
    dest_nc->ctrl_order  = src_nc->ctrl_order;
    dest_nc->non_drx_per = src_nc->non_drx_per;
    dest_nc->rep_per_i   = src_nc->rep_per_i;
    dest_nc->rep_per_t   = src_nc->rep_per_t;
  }
}/* grr_prcs_nc_param_final */

/*
+------------------------------------------------------------------------------
| Function    : grr_init_ncmeas_struct
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : ncmeas - pointer to NC measurement parameter
|               is_cw  - indicates whether the parameter pointer is related
|                        to cell wide or MS specific information
|
+------------------------------------------------------------------------------
*/
LOCAL void grr_init_ncmeas_struct( T_NCMEAS *ncmeas, BOOL is_cw )
{ 
  TRACE_FUNCTION( "grr_init_ncmeas_struct" );
  
  grr_init_nc_param( &ncmeas->param, is_cw );
  grr_init_nc_list( &ncmeas->list );
}/* grr_init_ncmeas_struct */

/*
+------------------------------------------------------------------------------
| Function    : grr_init_ncmeas_extd_struct
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : extd  - pointer to NC measurement extended parameter
|               is_cw - indicates whether the parameter pointer is related
|                       to cell wide or MS specific information
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_init_ncmeas_extd_struct ( T_NCMEAS_EXTENDED *extd, BOOL is_cw )
{ 
  TRACE_FUNCTION( "grr_init_ncmeas_extd_struct" );
  
  grr_init_ncmeas_struct( &extd->ncmeas, is_cw );
  grr_init_rfreq_list( &extd->rfreq );

#if defined (REL99) AND defined (TI_PS_FF_EMR)
  /* 
   * If PMO has not been received, PMO_USED shall be set to zero. 
   * Refer 3GPP TS 04.60 V8.17.0 (2002-12) Table 11.2.9d.2 
   */
  extd->pmo_ind = 0;
  extd->psi3_cm = NOT_SET;
  extd->ba_ind = NOT_SET;
#endif

}/* grr_init_ncmeas_extd_struct */

/*
+------------------------------------------------------------------------------
| Function    : grr_init_nc_list
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : list  - pointer to NC measurement list
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_init_nc_list( T_NC_LIST *list )
{ 
  TRACE_FUNCTION( "grr_init_nc_list" );
  
  list->number = 0;
}/* grr_init_nc_list */

/*
+------------------------------------------------------------------------------
| Function    : grr_init_nc_param
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : param - pointer to NC measurement parameter
|               is_cw - indicates whether the parameter pointer is related
|                       to cell wide or MS specific information
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_init_nc_param( T_NC_PARAM *param, BOOL is_cw )
{ 
  TRACE_FUNCTION( "grr_init_nc_param" );
  
  param->ctrl_order = ( is_cw EQ TRUE ? NC_NC0 : NC_EMPTY );
  param->idx        = NOT_SET;
}/* grr_init_nc_param */

/*
+------------------------------------------------------------------------------
| Function    : grr_init_rfreq_list
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : list  - pointer to NC measurement removed frequency list
|
+------------------------------------------------------------------------------
*/
LOCAL void grr_init_rfreq_list ( T_NC_RFREQ_LIST *list )
{ 
  TRACE_FUNCTION( "grr_init_rfreq_list" );
  
  list->number = 0;
}/* grr_init_rfreq_list */

/*
+------------------------------------------------------------------------------
| Function    : grr_sort_ext_lst_freq
+------------------------------------------------------------------------------
| Description : 
| Parameters  : 
+------------------------------------------------------------------------------
*/
GLOBAL void grr_sort_ext_lst_freq ( T_EXT_FREQ_LIST  *list,
                                    UBYTE             max_number,
                                    T_EXT_START_STOP *start_stop )
{
  UBYTE           i, j;     /* used for counting                    */
  T_EXT_FREQ_LIST tmp_list; /* temporary copy of EXT frequency list */

  tmp_list.number = 0;


  TRACE_FUNCTION( "grr_sort_ext_lst_freq" );

  for( i = 0; i < max_number; i++ )
  {
    if( start_stop[i].start NEQ RRGRR_INVALID_IDX AND 
        start_stop[i].stop  NEQ RRGRR_INVALID_IDX     )
    {
      for( j  = start_stop[i].start;
           j <= start_stop[i].stop;
           j++ )
      {
        tmp_list.freq[tmp_list.number] = list->freq[j];
        tmp_list.number++;
      }
    }
  }

  for( j = 0; j < tmp_list.number; j++ )
  {
    list->freq[j] = tmp_list.freq[j];
  }

  grr_mrk_ext_lst_freq( list );
} /* grr_sort_ext_lst_freq() */

/*
+------------------------------------------------------------------------------
| Function    : grr_mrk_ext_lst_freq
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
LOCAL void grr_mrk_ext_lst_freq ( T_EXT_FREQ_LIST *list )
{ 
  UBYTE i, j;

  TRACE_FUNCTION( "grr_mrk_ext_lst_freq" );

  if( list->number NEQ 0 )
  {
    for( i = 0; i < list->number - 1; i++ )
    {
      for( j = i + 1; 
           j < list->number AND IS_NOT_FLAGGED( list->freq[i],
                                                EM_VLDTY_ARFCN_MASK, 
                                                EM_NON_VLD_ARFCN );
           j++ )
      {
        if( list->freq[i] EQ list->freq[j] )
        {
          SET_FLAG( list->freq[i], EM_VLDTY_ARFCN_MASK, EM_NON_VLD_ARFCN, USHORT );       
        }
      }
    }
  }
}/* grr_mrk_ext_lst_freq */







/*
+------------------------------------------------------------------------------
| Function    : grr_create_channel_list
+------------------------------------------------------------------------------
| Description : The function grr_create_channel_list()  
|               Use a function from RR entity to extract frequencies from a frequency list structure
|               defined in 04.08
|               This function copies the frequencies into list. This function takes the arranging
|               of ARFCN into account cf. 12.10a GPRS Mobile Allocation in 04.60
| Parameters  : T_rfl*: pointer to the rfl received in PSI2 and UBYTE*list: pointer to 
|               the list similar to T_LIST
+------------------------------------------------------------------------------
*/
GLOBAL void grr_create_channel_list(T_rfl* rfl, UBYTE* list)
{
  T_f_range rfl_contents;
  UBYTE i;

  TRACE_FUNCTION( "grr_create_channel_list" );
  /*
   * Copy RFL contents into the T_LIST
   */
  memset (&rfl_contents, 0, sizeof(T_f_range));
  /* 
   * Length in bits
   */
  rfl_contents.l_f = (rfl->rfl_cont_len+3)*8;
  /*
   * Copy RFL content bits into the structure
   */
  for(i=0; i < rfl->c_rfl_cont; i++)
  {
    rfl_contents.b_f[i] = rfl->rfl_cont[i].flist;
  }
        
  /*
   * Use a function from RR
   * This function copies the frequencies into list. This function takes the arranging
   * of ARFCN into account cf. 12.10a GPRS Mobile Allocation in 04.60
   */
  for_create_channel_list (&rfl_contents, (T_LIST*)list);

}/* grr_create_channel_list */


/*
+------------------------------------------------------------------------------
| Function    : grr_get_si_cell_alloc_list
+------------------------------------------------------------------------------
| Description : 
|      
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void grr_get_si_cell_alloc_list( T_LIST *list )
{
#ifndef _TARGET_

  T_LIST cell_chan_des = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
    0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x00,0x00 }; 

#endif

  TRACE_FUNCTION( "grr_get_si_cell_alloc_list" );

  /*
   * Use cell allocation in SI1
   */

  /* Cell allocation is stored in rr at                                 */
  /* rr_data->sc_data.cd.cell_chan_desc for now use hazardous hack of   */
  /* access database of RR entity later make a new entity that replaces */
  /* RR and GRR or make a communication between RR and GRR to transfer  */
  /* list.                                                              */
  /* Used selfmade glumps hack function in RR to get cell allocation    */
  /* because getting typedefs from RR to use rr_data pointer is not     */
  /* that easy cause by name equalities in RR and GRR                   */

#ifndef _TARGET_

  srv_merge_list( list, &cell_chan_des );

#else
   
  /* TRACE_EVENT( "Use cell allocation in S1" ); */

  srv_get_cell_alloc_list( list );

#endif

} /* grr_get_si_cell_alloc_list */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_psi_cell_alloc_list
+------------------------------------------------------------------------------
| Description : 
|      
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL grr_get_psi_cell_alloc_list ( T_LIST *list )
{
  BOOL  rfl_not_found = FALSE;
  
  UBYTE n;
  UBYTE i;

  TRACE_FUNCTION( "grr_get_psi_cell_alloc_list" );

  for( n = 0; n < MAX_CELL_ALLOC; n++ )
  {
    if( psc_db->cell_alloc[n].rfl_num EQ NOT_SET )
    {
      if( n EQ 0 )
      {
#ifdef _SIMULATION_

        TRACE_ASSERT( FALSE );

#endif /* #ifdef _SIMULATION_ */

        rfl_not_found = TRUE;
      }
     
      break;
    }

    /* find rfl list with that number */
    i = 0;
   
    /* do not check the RFL received in an assignment message */
    while( i < MAX_RFL )
    {
      if( psc_db->rfl[i].num EQ psc_db->cell_alloc[n].rfl_num )
      {
        /* TRACE_EVENT_P1( "psc_db->rfl[i].num:%d", psc_db->rfl[i].num ); */
        break;
      }

      i++;
    }

    if( MAX_RFL > i )
    {
      srv_merge_list( list,                            /* output, result */ 
                      (T_LIST *)psc_db->rfl[i].list );
    }
    else
    {
#ifdef _SIMULATION_

      TRACE_ASSERT( FALSE );

#endif /* #ifdef _SIMULATION_ */

      rfl_not_found = TRUE;
      break;
    }
  }

  return( rfl_not_found EQ FALSE );

} /* grr_get_psi_cell_alloc_list */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_ms_alloc_list
+------------------------------------------------------------------------------
| Description : 
|      
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL grr_get_ms_alloc_list ( T_LIST             *list,
                                   const T_gprs_ms_alloc_ie *ms_alloc )
{
  BOOL  rfl_not_found = FALSE;
  
  UBYTE n;
  UBYTE i;

  TRACE_FUNCTION( "grr_get_ms_alloc_list" );

  for( n = 0; n < ms_alloc->c_rfl_num_list; n++ )
  {
    if( ms_alloc->rfl_num_list[n].rfl_num EQ NOT_SET )
    {
#ifdef _SIMULATION_

      TRACE_ASSERT( FALSE );

#endif /* #ifdef _SIMULATION_ */

      rfl_not_found = TRUE;
      break;
    }

    /* find rfl list with that number */
    i = 0;
    
    while( i <= MAX_RFL )
    {
      if( psc_db->rfl[i].num EQ ms_alloc->rfl_num_list[n].rfl_num )
      {
        break;
      }

      i++;
    }

    if( MAX_RFL >= i )
    {
      srv_merge_list( list,                            /* output, result */ 
                      (T_LIST *)psc_db->rfl[i].list );
    }
    else
    {
#ifdef _SIMULATION_

      TRACE_ASSERT( FALSE );

#endif /* #ifdef _SIMULATION_ */

      rfl_not_found = TRUE;
      break;
    }
  }

  return( rfl_not_found EQ FALSE );

} /* grr_get_ms_alloc_list */

/*
+------------------------------------------------------------------------------
| Function    : grr_create_freq_list
+------------------------------------------------------------------------------
| Description : The function grr_create_freq_list()  
|      
|               Get the according GPRS Mobile Allocation: Take into account MA_NUMBER
|               Write frequencies into frequency list
|      
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_create_freq_list( UBYTE          ma_num,
                                  UBYTE          maio,
                                  T_p_chan_sel  *chan_sel,
                                  T_p_freq_list *freq_list )
{
  BOOL                pbcch_is_present = grr_is_pbcch_present( );
  UBYTE               n                = 0;
  T_gprs_ms_alloc_ie *gp_ma            = NULL;
  
  T_LIST list1;
  USHORT list2[64];
  
  TRACE_FUNCTION( "grr_create_freq_list" );

  memset(&list1, 0, sizeof(T_LIST));
  memset(list2, NOT_PRESENT_16BIT, sizeof(list2));

  gp_ma = grr_get_gprs_ma (ma_num);
  
  if( gp_ma EQ NULL )
  {
    TRACE_EVENT("No valid GPRS_MA found");
    return FALSE;
  }

  /*
   * Copy hopping sequence number and mobile allocation index offset
   */
  chan_sel->p_rf_ch.arfcn = ( ( USHORT )gp_ma->hsn << 8 ) | ( USHORT )maio;

  /* Now we have the GPRS Mobile Allocation corresponding to the ma_num.
   * Get the referenced set of radio frequency lists for this
   * particular GPRS Mobile Allocation IE.
   * If RFL number list is not present in the IE, then cell allocation
   * would be returned by this function.
   */
  if(!grr_get_ref_list_or_cell_allocation(&list1,gp_ma,ma_num))
  {
     TRACE_ERROR("Could not get ref list");
     return FALSE;
  }

  /*
   * Copy ARFCN values into freq_list
   */
  srv_create_list (&list1, list2, 64, TRUE, 0);

  /*
   * Take MA_BITMAP or ARFCN_INDEX into account
   */
  grr_ma_filter_list( 
                      list2, /* input*/
                      freq_list->p_rf_chan_no.p_radio_freq, /* output*/
                      gp_ma
                     );

  /*
   * Get number of copied frequencies
   */
  if(!grr_validate_and_count_frequencies(freq_list->p_rf_chan_no.p_radio_freq,
                                                  &(freq_list->p_rf_chan_cnt)))
  {
    TRACE_ERROR( "grr_create_freq_list: validation failed/freq count zero" );
    return FALSE;
  }
  
  chan_sel->hopping        = 1;  
  
  /* Frequencies stored in freq_list are in form used by 3GPP(g23) standards.
   * convert them to the form used by L1.
   */
  for( n = 0; n < freq_list->p_rf_chan_cnt; n++ )     
  {
    freq_list->p_rf_chan_no.p_radio_freq[n] = 
                grr_g23_arfcn_to_l1( freq_list->p_rf_chan_no.p_radio_freq[n] );
  }

  return TRUE;

}/* grr_create_freq_list*/


/*
+------------------------------------------------------------------------------
| Function    : grr_ma_filter_list
+------------------------------------------------------------------------------
| Description : The function grr_ma_filter_list()  
|               takes MA_BITMAP or ARFCN_INDEX into account
|
|      
| Parameters  : list1 pointer to the input list
|               list2: pointer to the output list
|               gp_ma: pointer to the GPRS Mobile Allocation
+------------------------------------------------------------------------------
*/
GLOBAL void grr_ma_filter_list( 
                               USHORT* list1, /* input*/
                               USHORT* list2, /* output*/
                               const T_gprs_ms_alloc_ie* gp_ma
                               )
{
  UBYTE m,n,i,j;
  TRACE_FUNCTION("grr_ma_filter_list");

  if(gp_ma->v_ma_struct)
  {
    /*
     * MA_BITMAP is valid. First received bit corresponds to the last of T_LIST
     * and the last value of
     */
    i=0, j=0;
    for(n=gp_ma->ma_struct.c_ma_map; n NEQ 0; n--)
    {
      if(gp_ma->ma_struct.ma_map[n-1])
      {
        /*
         * Corresponding frequency is valid
         */
        list2[j] = list1[i];
        j++;
      }
      i++;
      if(list1[i] EQ NOT_PRESENT_16BIT)
        break;
    }
    list2[j] = NOT_PRESENT_16BIT;
  }
  else
  {
    /*
     * Check whether ARFCN list is valid or not
     */
    if(gp_ma->v_arfcn_index_list)
    {
      BOOL is_index_in_the_list=FALSE;
      /*
       * ARFCN Index list is valid
       */
      i=0, j=0;
      for(n=0; n < 64; n++)
      {
        for(m=0; m < gp_ma->c_arfcn_index_list; m++)
        {
          if(n EQ gp_ma->arfcn_index_list[m].arfcn_index)
          {
            /*
             * ARFCN_INDEX is in the list
             */
            is_index_in_the_list = TRUE;
            break;
          }
        }
        if(!is_index_in_the_list)
        {
          list2[j] = list1[i];
          j++;
        }
        is_index_in_the_list = FALSE;
        i++;
        
        if(list1[i] EQ NOT_PRESENT_16BIT)
          break;
      }
      list2[j] = NOT_PRESENT_16BIT;
    }
    else
    {
      /*
       * All radio frequencies are valid
       */
      /*list2 = list1;*/
      memcpy(list2, list1, 64*sizeof(USHORT));
    }
  }
}/* grr_ma_filter_list*/








/*
+------------------------------------------------------------------------------
| Function    : grr_calc_t_diff
+------------------------------------------------------------------------------
| Description : The function grr_calc_t_diff() calculates the difference 
|               between two time stamps taken into account an overflow of the
|               system ticks counter value.
|
| Parameters  : t_start - starting time
|               t_stop  - stopping time
|
+------------------------------------------------------------------------------
*/
GLOBAL T_TIME grr_calc_t_diff (T_TIME t_start, T_TIME t_stop)
{
  T_TIME t_diff;

  TRACE_FUNCTION("grr_calc_t_diff");
  
  if( t_start <= t_stop )
  {
    t_diff = t_stop - t_start;
  }
  else
  {
    t_diff = t_stop + ( ( ~ ( (T_TIME)0 ) ) - t_start ) + 1;
  }

  return( t_diff );
}/*grr_calc_t_diff()*/

/*
+------------------------------------------------------------------------------
| Function    : grr_prepare_db_for_new_cell
+------------------------------------------------------------------------------
| Description : The function grr_prepare_db_for_new_cell prepares the psi 
|               parameters and sc_db parameters like pbcch and pccch for
|               reading new SI13 and if needed PSI messages
|
| Parameters  : void
+------------------------------------------------------------------------------
*/
GLOBAL void grr_prepare_db_for_new_cell ( void )
{
  TRACE_FUNCTION("grr_prepare_db_for_new_cell");
   
  psi_init();

  /*initial PCCCH organization parameters */
  memset(&psc_db->pccch, 0, sizeof(T_pccch_org_par)); 

  /* Initial paging group*/
  memset(&psc_db->paging_group, 0, sizeof(T_PAGING_GROUP));

  psc_db->cell_info_for_gmm.cell_info.cell_env.rai.rac = GMMRR_RAC_INVALID;

  grr_set_pbcch( FALSE );

}/* grr_prepare_db_for_new_cell*/


/*
+------------------------------------------------------------------------------
| Function    : grr_is_non_drx_mode
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_is_non_drx_mode ( void )
{
  TRACE_FUNCTION( "grr_is_non_drx_mode" );

  return( grr_data->ms.split_pg_cycle   EQ  GMMRR_NO_DRX       OR
          ( grr_data->ms.split_pg_cycle NEQ GMMRR_NO_DRX AND 
            grr_is_non_drx_period( )    EQ  TRUE             )    );

}/* grr_is_non_drx_mode */

/*
+------------------------------------------------------------------------------
| Function    : grr_is_non_drx_period
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : Input: void
|               Return: Sends TRUE if one of the three NON DRX period is valid,
|                       otherwise FALSE;
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_is_non_drx_period ( void )
{
  TRACE_FUNCTION( "grr_is_non_drx_period" );

  return( psc_db->gprs_attach_is_running     OR
          psc_db->non_drx_timer_running      OR
          psc_db->nc2_non_drx_period_running    );

}/* grr_is_non_drx_period */

/*
+------------------------------------------------------------------------------
| Function    : grr_handle_non_drx_period
+------------------------------------------------------------------------------
| Description : Handles the timer of non DRX mode
|
| Parameters  : non_drx_type          - type of non DRX mode
|               non_drx_timer_running - indicates whether non DRX period 
|                                       is active or not
+------------------------------------------------------------------------------
*/
GLOBAL void grr_handle_non_drx_period ( T_NON_DRX_TYPE non_drx_type,
                                        BOOL           non_drx_timer_running )
{ 
  TRACE_FUNCTION( "grr_handle_non_drx_period" );
  
  switch( non_drx_type )
  {
    case TRANSFER_NDRX:
      psc_db->non_drx_timer_running      = non_drx_timer_running;
      break;

    case GMM_NDRX:
      psc_db->gprs_attach_is_running     = non_drx_timer_running;
      break;

    case NC2_NDRX:
      psc_db->nc2_non_drx_period_running = non_drx_timer_running;
      break;
  }

  pg_non_drx( );

}/*grr_handle_non_drx_period*/ 

/*
+------------------------------------------------------------------------------
| Function    : grr_store_g_pwr_par
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_store_g_pwr_par ( T_g_pwr_par *g_pwr_par )
{ 
  TRACE_FUNCTION( "grr_store_g_pwr_par" );
  
  psc_db->v_g_pwr_par = TRUE;

  psc_db->g_pwr_par.alpha           = CLIP_ALPHA( g_pwr_par->alpha );
  psc_db->g_pwr_par.t_avg_w         = CLIP_T_AVG( g_pwr_par->t_avg_w );
  psc_db->g_pwr_par.t_avg_t         = CLIP_T_AVG( g_pwr_par->t_avg_t );
  psc_db->g_pwr_par.pb              = g_pwr_par->pb;
  psc_db->g_pwr_par.pc_meas_chan    = g_pwr_par->pc_meas_chan;
  psc_db->g_pwr_par.imeas_chan_list = g_pwr_par->imeas_chan_list;
  psc_db->g_pwr_par.n_avg_i         = g_pwr_par->n_avg_i;

  grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param = TRUE;

} /* grr_store_g_pwr_par */

/*
+------------------------------------------------------------------------------
| Function    : grr_store_type_pwr_par
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_store_type_pwr_par ( T_pwr_par *pwr_par, BOOL use_prev )
{ 
  UBYTE i; /* used for counting */

  TRACE_FUNCTION( "grr_store_type_pwr_par" );
  
  grr_set_alpha_flags( TRUE, pwr_par->alpha );

  for( i = 0; i < 8; i++ )
  {
    /*
     * if the information element is not present for certain previously 
     * allocated timeslots, the MS shall continue to use the previous
     * power on these timeslots
     */
    if( pwr_par->gamma_tn[i].v_gamma OR !use_prev )
    {
      psc_db->pwr_par.gamma_tn[i].v_gamma = pwr_par->gamma_tn[i].v_gamma;
      psc_db->pwr_par.gamma_tn[i].gamma   = pwr_par->gamma_tn[i].gamma;
    }
  }

  grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param = TRUE;

} /* grr_store_type_pwr_par */

/*
+------------------------------------------------------------------------------
| Function    : grr_store_type_tn_alloc_pwr
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_store_type_tn_alloc_pwr ( T_tn_alloc_pwr *pwr_par )
{ 
  UBYTE i; /* used for counting */

  TRACE_FUNCTION( "grr_store_type_tn_alloc_pwr" );
  
  grr_set_alpha_flags( TRUE, pwr_par->alpha );

  for( i = 0; i < 8; i++ )
  {
    psc_db->pwr_par.gamma_tn[i].v_gamma = pwr_par->usf_array[i].v_usf_g;
    psc_db->pwr_par.gamma_tn[i].gamma   = pwr_par->usf_array[i].usf_g.gamma;
  }

  grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param = TRUE;
  
} /* grr_store_type_tn_alloc_pwr */

/*
+------------------------------------------------------------------------------
| Function    : grr_store_type_dyn_alloc
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_store_type_dyn_alloc ( T_dyn_alloc *pwr_par )
{ 
  UBYTE i; /* used for counting */

  TRACE_FUNCTION( "grr_store_type_dyn_alloc" );
  
  grr_set_alpha_flags( pwr_par->v_alpha, pwr_par->alpha );

  if( pwr_par->v_usf_gamma_csn1 EQ TRUE )
  {
    for( i = 0; i < 8; i++ )
    {
      psc_db->pwr_par.gamma_tn[i].v_gamma = pwr_par->usf_gamma_csn1[i].v_usf_gamma;
      psc_db->pwr_par.gamma_tn[i].gamma   = pwr_par->usf_gamma_csn1[i].usf_gamma.gamma;
    }
  }

  grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param = TRUE;

} /* grr_store_type_dyn_alloc */

/*
+------------------------------------------------------------------------------
| Function    : grr_store_type_alf_gam
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_store_type_alf_gam ( T_alf_gam *pwr_par, UBYTE tn )
{ 
  TRACE_FUNCTION( "grr_store_type_alf_gam" );
  
  grr_set_alpha_flags( TRUE, pwr_par->alpha );
  grr_set_sngl_gamma ( pwr_par->gamma, tn );

  grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param = TRUE;

} /* grr_store_type_alf_gam */

/*
+------------------------------------------------------------------------------
| Function    : grr_store_type_pck_upl_ass_ia
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_store_type_pck_upl_ass_ia ( T_tfi_ass_alloc *pwr_par,
                                            UBYTE             tn       )
{ 
  TRACE_FUNCTION( "grr_store_type_pck_upl_ass_ia" );
  
  grr_set_alpha_flags( pwr_par->v_alpha, pwr_par->alpha );
  grr_set_sngl_gamma ( pwr_par->gamma, tn );

  grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param = TRUE;

} /* grr_store_type_pck_upl_ass_ia */

/*
+------------------------------------------------------------------------------
| Function    : grr_store_type_pck_snbl_ass_ia
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_store_type_pck_snbl_ass_ia ( T_sngl_block_alloc *pwr_par,
                                            UBYTE             tn       )
{ 
  TRACE_FUNCTION( "grr_store_type_pck_snbl_ass_ia" );
  
  grr_set_alpha_flags( pwr_par->v_alpha, pwr_par->alpha );
  grr_set_sngl_gamma ( pwr_par->gamma, tn );

} /* grr_store_type_pck_snbl_ass_ia */


/*
+------------------------------------------------------------------------------
| Function    : grr_store_type_tfi_ass_rlc
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_store_type_tfi_ass_rlc ( T_tfi_ass_rlc *pwr_par,
                                         UBYTE          tn       )
{ 
  TRACE_FUNCTION( "grr_store_type_tfi_ass_rlc" );
  
  grr_set_alpha_flags( pwr_par->v_alpha, pwr_par->alpha );
  grr_set_sngl_gamma ( pwr_par->gamma, tn );

  grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param = TRUE;

} /* grr_store_type_tfi_ass_rlc */


/*
+------------------------------------------------------------------------------
| Function    : grr_store_type_alpha_gamma
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_store_type_alpha_gamma ( T_alpha_gamma *pwr_par,
                                         UBYTE          tn       )
{ 
  TRACE_FUNCTION( "grr_store_type_alpha_gamma" );
  
  grr_set_alpha_flags( TRUE, pwr_par->alpha );
  grr_set_sngl_gamma ( pwr_par->gamma, tn );

  grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param = TRUE;

} /* grr_store_type_alpha_gamma */

/*
+------------------------------------------------------------------------------
| Function    : grr_store_type_pwr_ctrl
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_store_type_pwr_ctrl ( T_pwr_ctrl *pwr_par )
{ 
  UBYTE i; /* used for counting */

  TRACE_FUNCTION( "grr_store_type_pwr_ctrl" );
  
  grr_set_alpha_flags( TRUE, pwr_par->alpha );

  for( i = 0; i < 8; i++ )
  {
    psc_db->pwr_par.gamma_tn[i].v_gamma = pwr_par->tagged_gamma[i].v_gamma;
    psc_db->pwr_par.gamma_tn[i].gamma   = pwr_par->tagged_gamma[i].gamma;
  }

  grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param = TRUE;

} /* grr_store_type_pwr_ctrl */



/*
+------------------------------------------------------------------------------
| Function    : grr_set_alpha_flags
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void grr_set_alpha_flags ( BOOL v_alpha, UBYTE alpha )
{ 
  TRACE_FUNCTION( "grr_set_alpha_flags" );
  
  if( v_alpha )
  {
    psc_db->pwr_par.v_alpha = TRUE;
    psc_db->pwr_par.alpha   = CLIP_ALPHA( alpha );
  }

  psc_db->v_pwr_par = TRUE;

} /* grr_set_alpha_flags */

/*
+------------------------------------------------------------------------------
| Function    : grr_set_sngl_gamma
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void grr_set_sngl_gamma ( UBYTE gamma, UBYTE tn )
{ 
  UBYTE i; /* used for counting */

  TRACE_FUNCTION( "grr_set_sngl_gamma" );
  
  for( i = 0; i < 8; i++ )
  {
    psc_db->pwr_par.gamma_tn[i].v_gamma = FALSE;
  }

  psc_db->pwr_par.gamma_tn[tn].v_gamma = TRUE;
  psc_db->pwr_par.gamma_tn[tn].gamma   = gamma;

} /* grr_set_sngl_gamma */

/*
+------------------------------------------------------------------------------
| Function    : grr_set_tbf_cfg_req_param
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_set_tbf_cfg_req_param
                                ( T_MPHP_ASSIGNMENT_REQ *tbf_cfg_req )
{ 
  TRACE_FUNCTION( "grr_set_tbf_cfg_req_param" );

  tbf_cfg_req->assign_id          = 0;
  grr_data->cs.last_assignment_id = tbf_cfg_req->assign_id; 
  tbf_cfg_req->m_class            = grr_get_gprs_ms_class( );
  tbf_cfg_req->if_meas_enable     = meas_im_get_permit( );
  tbf_cfg_req->pc_meas_chan       = psc_db->g_pwr_par.pc_meas_chan;
 
  if( psc_db->gprs_cell_opt.ab_type EQ AB_8_BIT )
  {
    tbf_cfg_req->burst_type = AB_8_BIT;
  }
  else
  {
    tbf_cfg_req->burst_type = AB_11_BIT;
  }
} /* grr_set_tbf_cfg_req_param */

/*
+------------------------------------------------------------------------------
| Function    : grr_init_ms_data
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void grr_init_ms_data ( void )
{ 
  TRACE_FUNCTION( "grr_init_ms_data" );

#ifdef _TARGET_
  grr_data->ms.reverts_NMO_III = TRUE;
  grr_data->ms.tbf_mon_ccch    = FALSE; /* Target: Not allowed*/
#else
  grr_data->ms.reverts_NMO_III = FALSE;
  grr_data->ms.tbf_mon_ccch    = TRUE; /* WIN 32: allowed*/
#endif
} /* grr_init_ms_data */




/*
+------------------------------------------------------------------------------
| Function    : grr_update_pacch
+------------------------------------------------------------------------------
| Description : The function grr_update_pacch updates the power reduction and 
|               access burst type in case of change and being in transfer mode
|
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_update_pacch( void )
{ 
  TRACE_FUNCTION( "grr_update_pacch" );

  {
    PALLOC(update_psi, MPHP_UPDATE_PSI_PARAM_REQ);
    update_psi->pb         = psc_db->g_pwr_par.pb;
    update_psi->burst_type = psc_db->gprs_cell_opt.ab_type;    
    PSEND(hCommL1, update_psi);
  }

} /* grr_update_pacch */


/*
+------------------------------------------------------------------------------
| Function    : grr_imsi_mod
+------------------------------------------------------------------------------
| Description : The function grr_imsi_mod() returns the imsi modulo 1000
|
| Parameters  : in: NONE out: USHORT imsimod
|
+------------------------------------------------------------------------------
*/

GLOBAL USHORT grr_imsi_mod ()
{
  UBYTE i= grr_data->db.ms_id.imsi.number_of_digits;
  TRACE_FUNCTION("grr_imsi_mod ");
  
  return ((SHORT)grr_data->db.ms_id.imsi.digit[i - 3] * 100 +
        (SHORT)grr_data->db.ms_id.imsi.digit[i - 2] * 10 +
        (SHORT)grr_data->db.ms_id.imsi.digit[i - 1]);

}

/*
+------------------------------------------------------------------------------
| Function    : grr_set_buf_tlli
+------------------------------------------------------------------------------
| Description : The function grr_set_buf_tlli() fills the TLLI buffer.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_set_buf_tlli ( BUF_tlli_value *buf_tlli_o, ULONG tlli_i )
{
  TRACE_FUNCTION( "grr_set_buf_tlli" );

  grr_set_tlli( &buf_tlli_o->l_tlli_value, 
                &buf_tlli_o->o_tlli_value,
                &buf_tlli_o->b_tlli_value[0],
                tlli_i );

} /* grr_set_buf_tlli */


/*
+------------------------------------------------------------------------------
| Function    : grr_set_tlli
+------------------------------------------------------------------------------
| Description : The function grr_set_tlli() fills the TLLI buffer.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_set_tlli
                ( USHORT *l_tlli, USHORT *o_tlli, UBYTE *b_tlli, ULONG tlli )
{
  TRACE_FUNCTION( "grr_set_tlli" );

  *l_tlli   = 32;
  *o_tlli   = 0;
  
  b_tlli[0] = (UBYTE)((tlli >> 24) & 0x000000ff);
  b_tlli[1] = (UBYTE)((tlli >> 16) & 0x000000ff);
  b_tlli[2] = (UBYTE)((tlli >> 8 ) & 0x000000ff);
  b_tlli[3] = (UBYTE)((tlli      ) & 0x000000ff);

  /* unused byte must be set to 0x00, otherwise CCD has some problems */
  b_tlli[4] = 0;

} /* grr_set_tlli */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_tlli
+------------------------------------------------------------------------------
| Description : The function grr_get_tlli() returns the TLLI.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL ULONG grr_get_tlli ( void )
{
  TRACE_FUNCTION( "grr_get_tlli" );

  return( grr_data->db.ms_id.new_tlli );

} /* grr_get_tlli */

/*
+------------------------------------------------------------------------------
| Function    : grr_encode_ul_ctrl_block
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_encode_ul_ctrl_block ( UBYTE *ul_ctrl_block, 
                                       UBYTE *ul_ctrl_data )
{
  T_CTRLBUF enc_block;

  TRACE_FUNCTION( "grr_encode_ul_ctrl_block" );

  grr_encode_ctrl( ul_ctrl_data, ( T_MSGBUF* )&enc_block , grr_data->r_bit );
  memcpy( ul_ctrl_block, enc_block.buf, BYTELEN( enc_block.l_buf ) );    

} /* grr_encode_ul_ctrl_block */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_ul_ctrl_block_header
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE grr_get_ul_ctrl_block_header ( UBYTE r_bit )
{
  TRACE_FUNCTION( "grr_get_ul_ctrl_block_header" );

  /* 
   * set Payload Type and R Bit
   */
  return( ( CTRL_BLK_NO_OPT << 6 ) | r_bit );

} /* grr_get_ul_ctrl_block_header */

/*
+------------------------------------------------------------------------------
| Function    : grr_check_if_tbf_start_is_elapsed
+------------------------------------------------------------------------------
| Description : The function grr_check_if_tbf_start_is_elapsed() checks if 
|               tbf starting time is elapsed or not, modulo calculation is 
|               needed
| Parameters  : start_fn    - tbf starting time
|               current_fn  - current frame number
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_check_if_tbf_start_is_elapsed ( ULONG start_fn, ULONG current_fn)
{
  BOOL  result = FALSE;
  ULONG d1;
  /* ULONG d2; */
  /* FN_MAX=0x297000 == 2715648 ==125463 seconds(4.62ms per frame)
   * the starting time is within  current_fn-10808 and current_fn+31623
   * modulo operation must be taken in account
   */
  TRACE_FUNCTION( "grr_check_if_tbf_start_is_elapsed" );

  /*
   * handle maximum distance for tbf starting time
   */

  if(start_fn EQ 0xFFFFFFFF)
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
    TRACE_EVENT_P2(" case 1: st time elapsed st_fn=%ld  c_fn=%ld",start_fn,current_fn);
  }
  else if((start_fn >= current_fn) AND
          (FN_MAX-start_fn+current_fn) <= d1)
  {
    result = TRUE;
    TRACE_EVENT_P2("case 2: st time elapsed st_fn=%ld  c_fn=%ld",start_fn,current_fn);
  }
  else
  {
    TRACE_EVENT_P2("case 3: WAIT FOR ST TIME st_fn=%ld  c_fn=%ld",start_fn,current_fn);
  }

    

  return result;
} /* grr_check_if_tbf_start_is_elapsed() */

/*
+------------------------------------------------------------------------------
| Function    : grr_calculate_Tra
+------------------------------------------------------------------------------
| Description : The function has to calculate for mobile type 1 (ms_class 1-12)
|               the minimum timeslots between end of last previous downlink  
|               timeslot and the  next uplink or two consecutive uplinks.
|
| Parameters  : ul_usage : uplink timeslot mask
|               dl_usage : downlink timeslot mask
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE grr_calculate_Tra (UBYTE ul_usage, UBYTE dl_usage)
{
  UBYTE mask=0x80, z_mask=0;
  UBYTE Tx[8],Rx[8],ul_seted_slot,dl_seted_slot,j,i,tra,rc_dl_usage;

  TRACE_FUNCTION( "grr_calculate_Tra" );

  memset( Tx, 0, sizeof( Tx ) );
  memset( Rx, 0, sizeof( Rx ) );


  ul_seted_slot = 0;
  dl_seted_slot = 0;

  /*
   * detect position of uplink seted slots and the number of slots
   */
  if(ul_usage NEQ z_mask)
  {
    for(j=0;j<8;j++)
    {
      if(mask & ul_usage)
      {
        Tx[ul_seted_slot]=j+1;
        ul_seted_slot++;
      }
      mask>>=1;
    }
  }
  /*
   * detect position of downlink seted slots and the number of slots
   * and shift them for 3 positions(delay between uplink and downlink)
   */  
  mask        = 0x80; /* set again, it is reused*/
  rc_dl_usage = grr_reconstruct_dl_usage(dl_usage);
  if(rc_dl_usage NEQ z_mask)
  {
    for(i=0;i<=7;i++)
    {
      if(mask & rc_dl_usage)
      {
        Rx[dl_seted_slot]=i+1;
        dl_seted_slot++;
      }
      mask>>=1;
    }
  }
  tra=0xFF; /* set to invalid value */
  /*
   * calculate tra
   */
  for(i=0;i<ul_seted_slot;i++)
  {
    for(j=0;j<dl_seted_slot;j++)
    {
      if (Rx[j] EQ Tx[i])
      { /* tranmission and receiving at the same time not allowed*/
        tra=0;
        TRACE_EVENT_P6("OVERLAP TRA  Rx[%d]=%d  Tx[%d]=%d ul_mask=%x dl_mask=%x"
                                  ,j
                                  ,Rx[j]
                                  ,i
                                  ,Tx[i]
                                  ,ul_usage
                                  ,dl_usage);
        return tra;
      }
      else if(Rx[j] > Tx[i])
      {
        if(tra > (Rx[j] - Tx[i]-1))
        {
          tra = Rx[j] - Tx[i]-1;
        }
      }
      else
      {
        if(tra > (Rx[j]+8 - Tx[i]-1))
        {
          tra = Rx[j]+8 - Tx[i]-1;
        }
      }
    }      
  }
  return tra;
} /*grr_calculate_Tra */

/*
+------------------------------------------------------------------------------
| Function    : grr_calculate_Ttb
+------------------------------------------------------------------------------
| Description : The function has to calculate for mobile type 1 (ms_class 1-12)
|               the minimum timeslot between end of last previous uplink  
|               timeslot and the first next downlink or two cnsecutive Downlinks.
|
| Parameters  : ul_usage : uplink timeslot mask
|               dl_usage : downlink timeslot mask
|
+------------------------------------------------------------------------------
*/

GLOBAL UBYTE grr_calculate_Ttb (UBYTE ul_usage, UBYTE dl_usage)
{
  UBYTE mask=0x80, z_mask=0;
  UBYTE Tx[8],Rx[8],ul_seted_slot,dl_seted_slot,j,i,ttb;

  TRACE_FUNCTION( "grr_calculate_Ttb" );

  memset( Tx, 0, sizeof( Tx ) );
  memset( Rx, 0, sizeof( Rx ) );
 
  ul_seted_slot = 0;
  dl_seted_slot = 0;

  /*
   * detecte position of uplink seted slots and the number of slots  
   * uplink slots gets an offset of 3 positions(Air interface).
   */
  if(ul_usage NEQ z_mask)
  {
    for(j=0;j<8;j++)
    {
      if(mask & ul_usage)
      {
        Tx[ul_seted_slot]=j+1+3;
        ul_seted_slot++;
      }
      mask>>=1;
    }
  }
  /* 
   * detecte position of downlink seted slots and the number of slots 
   */
  mask=0x80;
  if(dl_usage NEQ z_mask)
  {
    for(i=0;i<=7;i++)
    {
      if(mask & dl_usage)
      {
        Rx[dl_seted_slot]=i+1;
        dl_seted_slot++;
      }
      mask>>=1;
    }
  }
  ttb=0xFF; /* set to invalid value */
  /*
   * calculate ttb
   */
  for(i=0;i<dl_seted_slot;i++)
  {
    for(j=0;j<ul_seted_slot;j++)
    {
      if ((Tx[j] EQ Rx[i])  OR
          !(Tx[j] - Rx[i]-8))
      { /* tranmission and receiving at the same time not allowed*/
        ttb=0;
        TRACE_EVENT_P6("OVERLAP TTB    Tx[%d]=%d  Rx[%d]=%d ul_mask=%x dl_mask=%x"
                                    ,j
                                    ,Tx[j]
                                    ,i
                                    ,Rx[i]
                                    ,ul_usage
                                    ,dl_usage);
        return ttb;
      }
      else if(Tx[j] > Rx[i])
      {
        if((Tx[j] - Rx[i] > 8) AND
           (ttb > (Tx[j] - Rx[i]-8-1)))
        {
          ttb  = Tx[j] - Rx[i]-8-1;
        }
        else if(ttb > (Tx[j] - Rx[i]-1))
        {
          ttb  = Tx[j] - Rx[i]-1;          
        }
      }
      else if(ttb > (Tx[j]+8 - Rx[i]-1) )
      {
        ttb = Tx[j]+8 - Rx[i]-1;
      }
    }
  }
  return ttb;
} /* grr_calculate_Ttb */


/*
+------------------------------------------------------------------------------
| Function    : grr_reconstruct_dl_usage
+------------------------------------------------------------------------------
| Description : this function converts the downlink slot mask. The first 3 MSB
|               are switched to the first LSB bits.
|               B0 is the MSB, B7 is the LSB
|               dl_usage (input)     : B0 B1 B2 B3 B4 B5 B6 B7
|               new_dl_usage (output): B3 B4 B5 B6 B7 B0 B1 B2
|
| Parameters  : dl_usage: this is timeslot mask for downlink
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE grr_reconstruct_dl_usage(UBYTE dl_usage)
{
  UBYTE help, new_dl_usage;

  TRACE_FUNCTION( "grr_reconstruct_dl_usage" );

  help         = 0xE0;
  help        &= dl_usage;
  dl_usage   <<= 3;
  help       >>= 5;
  new_dl_usage = dl_usage | help;

  return new_dl_usage;
}/* grr_reconstruct_dl_usage */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_gprs_ms_class
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE grr_get_gprs_ms_class ( void )
{
  T_rf_cap *rf_cap         = rr_csf_get_rf_capability( );
  UBYTE     gprs_ms_class;

  TRACE_FUNCTION( "grr_get_gprs_ms_class" );

  if( rf_cap NEQ NULL )
  {
    gprs_ms_class = rf_cap->rf_ms.gprs_ms_class;
  }
  else
  {
    gprs_ms_class = MSLOT_CLASS_1;

    TRACE_ERROR( "grr_get_gprs_ms_class: RF capabilities invalid" );
  }

  return( gprs_ms_class );

} /* grr_get_gprs_ms_class */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_next_ncell_param
+------------------------------------------------------------------------------
| Description : 
| Parameters  : 
+------------------------------------------------------------------------------
*/
GLOBAL T_ncell_info* grr_get_next_ncell_param ( UBYTE       max_number,
                                                T_NC_LIST  *list,
                                                T_INFO_SRC  info_src )
{
  UBYTE         i;
  T_ncell_info *ncell_info_max;
  T_ncell_info *ncell_info_next;

  TRACE_FUNCTION( "grr_get_next_ncell_param" );

  /*
   * looking for the next free entry or in case there is no more free entry
   * available, looking for the highest indexed entry which will be deleted
   */
  if( list->number < max_number )
  {
    ncell_info_max = &list->info[list->number];
  }
  else
  {
    ncell_info_max = &list->info[0];

    for( i = 1; i < max_number; i++ )
    {
      ncell_info_next = &list->info[i];

      if( ncell_info_next->info_src > ncell_info_max->info_src )
      {
        ncell_info_max = ncell_info_next;
      }
    }

    TRACE_ERROR( "grr_get_next_ncell_param: Number of NCELLs > max_number" );

    if( ncell_info_max->info_src < info_src )
    {
      TRACE_EVENT( "grr_get_next_ncell_param: Keep NCELL list" );

      ncell_info_max = NULL;
    }
    else if( ncell_info_max->info_src EQ info_src )
    {
      TRACE_ERROR( "grr_get_next_ncell_param: Two identical NCELL" );

      ncell_info_max = NULL;
    }
    else
    {
      TRACE_EVENT( "grr_get_next_ncell_param: Change NCELL list" );
    }
  }

  return( ncell_info_max );

}/* grr_get_next_ncell_param */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_next_bigger_ncell_param
+------------------------------------------------------------------------------
| Description : 
| Parameters  : 
+------------------------------------------------------------------------------
*/
GLOBAL T_ncell_info* grr_get_next_bigger_ncell_param ( T_NC_LIST  *list,
                                                       T_INFO_SRC  info_src )
{
  UBYTE         i;
  T_INFO_SRC    info_src_max;
  T_ncell_info *ncell_info_next;
  T_ncell_info *ncell_info_bigger = NULL;

  TRACE_FUNCTION( "grr_get_next_bigger_ncell_param" );

  if( list->number NEQ 0 )
  {
    /*
     * all indeces should be set to unused in case info_src EQ 0,
     * which marks the start of a new search process 
     */    
    if( info_src EQ 0 )
    {
      for( i = 0; i < list->number; i++ )
      {
        list->info[i].index = 0xFF;
      }
    }

    i            = 0;
    info_src_max = ~((T_INFO_SRC)0);

    do
    {
      ncell_info_next = &list->info[i];

      TRACE_ASSERT( info_src               NEQ ncell_info_next->info_src OR 
                    ncell_info_next->index NEQ 0xFF                         );


      if( info_src                  < ncell_info_next->info_src AND 
          ncell_info_next->info_src < info_src_max                  )
      {
        info_src_max      = ncell_info_next->info_src;
        ncell_info_bigger = ncell_info_next;
      }

      i++;
    }
    while( i < list->number );
  }

  return( ncell_info_bigger );

}/* grr_get_next_bigger_ncell_param */

/*
+------------------------------------------------------------------------------
| Function    : grr_store_cs_param
+------------------------------------------------------------------------------
| Description : 
| Parameters  : ncell_info     - neighbour cell information
|               v_cs_par       - valid falg for cell selection parameter
|               cs_par         - cell selection parameter
|               info_src       - source of neighbour cell information
|               arfcn          - absolute radio frequency channel number
|               bsic           - base station identity code
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_store_cs_param ( T_NC_LIST    *nc_list,
                                 BOOL          v_cs_par,
                                 T_cs_par     *cs_par,
                                 T_INFO_TYPE   type,
                                 UBYTE         instance,
                                 UBYTE        *number,
                                 USHORT       *freq,
                                 USHORT        freq_diff,
                                 UBYTE         bsic )
{
  T_INFO_SRC    info_src=0;
  T_ncell_info *ncell_info;

  TRACE_FUNCTION( "grr_store_cs_param" );

  NC_SET_TYPE    ( info_src, type     );
  NC_SET_INSTANCE( info_src, instance );
  NC_SET_NUMBER  ( info_src, *number  );

  ncell_info = grr_get_next_ncell_param( MAX_NR_OF_NCELL,
                                         nc_list, 
                                         info_src );

  if( ncell_info EQ NULL )
  {
    return( FALSE );
  }

  (*number)++;
  nc_list->number++;

  *freq = ( *freq + freq_diff ) % 1024;

  ncell_info->info_src = info_src;
  ncell_info->arfcn    = *freq;
  ncell_info->bsic     = bsic;

  ncell_info->v_cr_par = v_cs_par;

  if( v_cs_par EQ TRUE )
  {
    ncell_info->cr_par.same_ra_scell = cs_par->same_ra_scell;
    ncell_info->cr_par.exc_acc       = cs_par->exc_acc;
    ncell_info->cr_par.cell_ba       = cs_par->cell_ba;

#if !defined (NTRACE)

    if( grr_data->cs.v_crp_trace EQ TRUE )
    {
      TRACE_EVENT_P3( "grr_store_cs_param: cell barred status %d %d, info_src %x",
                      ncell_info->arfcn, ncell_info->cr_par.cell_ba, info_src );
    }

#endif /* #if !defined (NTRACE) */

    SET_GPRS_RXLEV_ACCESS_MIN
      ( ncell_info->cr_par.cr_par_1.cr_pow_par.gprs_rxlev_access_min,
        cs_par->v_cs_par_s1, cs_par->cs_par_s1.gprs_rxlev_access_min );

    SET_GPRS_MS_TXPWR_MAX_CCH
      ( ncell_info->cr_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch,
        cs_par->v_cs_par_s1, cs_par->cs_par_s1.txpwr_max_cch );

    SET_GPRS_TEMP_OFFSET
      ( ncell_info->cr_par.cr_offset.gprs_temp_offset,
        cs_par->v_cs_par_s2, cs_par->cs_par_s2.gprs_temp_offset );

    SET_GPRS_PENALTY_TIME
      ( ncell_info->cr_par.cr_offset.gprs_penalty_time,
        cs_par->v_cs_par_s2, cs_par->cs_par_s2.gprs_penalty_time );

    SET_GPRS_RESEL_OFF
      ( ncell_info->cr_par.gprs_resel_off,
        cs_par->v_gprs_resel_off, cs_par->gprs_resel_off );

    ncell_info->cr_par.cr_par_1.v_hcs_par = cs_par->v_hcs_par;

    SET_GPRS_HCS_THR
      ( ncell_info->cr_par.cr_par_1.hcs_par.gprs_hcs_thr,
        cs_par->v_hcs_par, cs_par->hcs_par.gprs_hcs_thr );

    SET_GPRS_PRIO_CLASS
      ( ncell_info->cr_par.cr_par_1.hcs_par.gprs_prio_class,
        cs_par->v_hcs_par, cs_par->hcs_par.gprs_prio_class );

    grr_store_si13_pbcch_location( &ncell_info->cr_par,
                                   cs_par->v_si13_pbcch,
                                   &cs_par->si13_pbcch );
  }

  return( TRUE );

}/* grr_store_cs_param */

/*
+------------------------------------------------------------------------------
| Function    : grr_store_si13_pbcch_locaction
+------------------------------------------------------------------------------
| Description : 
| Parameters  : 
+------------------------------------------------------------------------------
*/
GLOBAL void grr_store_si13_pbcch_location ( T_CR_PAR     *cr_par,
                                            BOOL          v_si13_pbcch,
                                            T_si13_pbcch *si13_pbcch )
{
  TRACE_FUNCTION( "grr_store_si13_pbcch_locaction" );

  cr_par->v_si13_pbcch = v_si13_pbcch;

  if( v_si13_pbcch )
  {
    cr_par->si13_pbcch.v_si13_location = si13_pbcch->v_si13_loc;

    if( si13_pbcch->v_si13_loc )
    {
      cr_par->si13_pbcch.si13_location = si13_pbcch->si13_loc;
    }
    else
    {
      cr_par->si13_pbcch.pbcch_location     =
        si13_pbcch->si13_pbcch_s1.pbcch_loc;
      cr_par->si13_pbcch.psi1_repeat_period =
        si13_pbcch->si13_pbcch_s1.psi1_rep_per + 1;
    }
  }
}/* grr_store_si13_pbcch_locaction */

/*
+------------------------------------------------------------------------------
| Function    : grr_restore_cs_param
+------------------------------------------------------------------------------
| Description : 
| Parameters  : 
+------------------------------------------------------------------------------
*/
GLOBAL void grr_restore_cs_param ( T_ncell_info *curr_info,
                                   T_ncell_info *prev_info,
                                   UBYTE         curr_idx )
{
  TRACE_FUNCTION( "grr_restore_cs_param" );

  curr_info->index = curr_idx;

  if( grr_is_pbcch_present( ) )
  {
    if( curr_info->cr_par.cr_par_1.cr_pow_par.gprs_rxlev_access_min EQ 
        GPRS_RXLEV_ACCESS_MIN_INVALID                                  )
    {
      if( prev_info NEQ NULL )
      {
        curr_info->cr_par.cr_par_1.cr_pow_par.gprs_rxlev_access_min =
          prev_info->cr_par.cr_par_1.cr_pow_par.gprs_rxlev_access_min;
      }
      else
      {
        curr_info->cr_par.cr_par_1.cr_pow_par.gprs_rxlev_access_min =
          psc_db->scell_par.cr_par_1.cr_pow_par.gprs_rxlev_access_min;
      }
    }

    if( curr_info->cr_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch EQ 
        GPRS_MS_TXPWR_MAX_CCH_INVALID                                  )
    {
      if( prev_info NEQ NULL )
      {
        curr_info->cr_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch =
          prev_info->cr_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch;
      }
      else
      {
        curr_info->cr_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch =
          psc_db->scell_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch;
      }
    }

    if( curr_info->cr_par.cr_offset.gprs_temp_offset EQ 
        GPRS_TEMPORARY_OFFSET_INVALID                   )
    {
      if( prev_info NEQ NULL )
      {
        curr_info->cr_par.cr_offset.gprs_temp_offset =
          prev_info->cr_par.cr_offset.gprs_temp_offset;
      }
      else
      {
        TRACE_ERROR( "gprs_temp_offset not present in 1st NCELL");

        curr_info->cr_par.cr_offset.gprs_temp_offset =
          GPRS_TEMPORARY_OFFSET_00_DB;
      }
    }

    if( curr_info->cr_par.cr_offset.gprs_penalty_time EQ 
        GPRS_PENALTY_TIME_INVALID                        )
    {
      if( prev_info NEQ NULL )
      {
        curr_info->cr_par.cr_offset.gprs_penalty_time =
          prev_info->cr_par.cr_offset.gprs_penalty_time;
      }
      else
      {
        TRACE_ERROR( "gprs_penalty_time not present in 1st NCELL");

        curr_info->cr_par.cr_offset.gprs_penalty_time =
          GPRS_PENALTY_TIME_MAX;
      }
    }

    if( psc_db->scell_par.cr_par_1.v_hcs_par EQ FALSE )
    {
      /*
       * If the HCS struct is omitted for the serving cell,
       * HCS is not used and the HCS parameters for the other
       * cells shall be neglected i.e the HCS signal strength
       * threshold shall be set to infinity for all cells.
       */
      curr_info->cr_par.cr_par_1.v_hcs_par = FALSE;
    }
    else
    {
      if( curr_info->cr_par.cr_par_1.v_hcs_par EQ FALSE )
      {
        if( prev_info NEQ NULL )
        {
          curr_info->cr_par.cr_par_1.v_hcs_par = 
            prev_info->cr_par.cr_par_1.v_hcs_par;

          if( curr_info->cr_par.cr_par_1.hcs_par.gprs_prio_class EQ
              GPRS_PRIORITY_CLASS_INVALID                           )
          {
            curr_info->cr_par.cr_par_1.hcs_par.gprs_prio_class =
              prev_info->cr_par.cr_par_1.hcs_par.gprs_prio_class;
          }

          if( curr_info->cr_par.cr_par_1.hcs_par.gprs_hcs_thr EQ 
              GPRS_HCS_THR_INVALID                               )
          {
            curr_info->cr_par.cr_par_1.hcs_par.gprs_hcs_thr =
              prev_info->cr_par.cr_par_1.hcs_par.gprs_hcs_thr;
          }
        }
        else
        {
          curr_info->cr_par.cr_par_1.v_hcs_par = 
            psc_db->scell_par.cr_par_1.v_hcs_par;

          if( curr_info->cr_par.cr_par_1.hcs_par.gprs_prio_class EQ
              GPRS_PRIORITY_CLASS_INVALID                           )
          {
            curr_info->cr_par.cr_par_1.hcs_par.gprs_prio_class =
              psc_db->scell_par.cr_par_1.hcs_par.gprs_prio_class;
          }

          if( curr_info->cr_par.cr_par_1.hcs_par.gprs_hcs_thr EQ 
              GPRS_HCS_THR_INVALID                               )
          {
            curr_info->cr_par.cr_par_1.hcs_par.gprs_hcs_thr =
              psc_db->scell_par.cr_par_1.hcs_par.gprs_hcs_thr;
          }
        }
      }
    }
  }
}/* grr_restore_cs_param */



/*
+------------------------------------------------------------------------------
| Function    : grr_handle_ta
+------------------------------------------------------------------------------
| Description : This function handles the timing advance of the MS
| Parameters  : v_tav:    valid flag ta_value
|               tav:      ta value
|               v_ul_tai: valid flag dl ta_index
|               ul_tai:   dl ta index
|               ul_tatn:  dl ta timeslot number
|               v_dl_tai: valid flag dl ta_index
|               dl_tai:   dl ta index
|               dl_tatn:  dl ta timeslot number
|               ptr2ta:   pointer to the ta structue, which is passed to l1
+------------------------------------------------------------------------------
*/
GLOBAL void grr_handle_ta ( UBYTE v_tav,
                            UBYTE tav,
                            UBYTE v_ul_tai, 
                            UBYTE ul_tai,
                            UBYTE ul_tatn, 
                            UBYTE v_dl_tai, 
                            UBYTE dl_tai,
                            UBYTE dl_tatn, 
                            T_p_timing_advance *ptr2ta)
{
  BOOL use_dl_par = FALSE;
  TRACE_FUNCTION( "grr_handle_ta" );

  ptr2ta->ta_value             = 0xFF;
  ptr2ta->ta_index             = 0xFF;
  ptr2ta->tn                   = 0xFF;

  /*TRACE_EVENT_P1("grr_data->tbf_type: %d", grr_data->tbf_type);*/
  switch(grr_data->tbf_type)
  {
    case CGRLC_TBF_MODE_DL:
    case CGRLC_TBF_MODE_DL_UL:
      /*use DL TA parameters*/
      use_dl_par = TRUE;
      break;
    case CGRLC_TBF_MODE_NULL:
    case CGRLC_TBF_MODE_UL:
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case CGRLC_TBF_MODE_2PA:
#endif
    default:
      /*future use*/
      break;
  }
  /*
   * TA-Value, if present
   */
  if(v_tav) /* New TA value Received */
  { 
    /*  
    TRACE_EVENT("New TA value Received");
    */
    if(tav NEQ grr_data->ta_params.ta_value )
    {
      /*
       * inform grlc that new ta is received
       */

      PALLOC(cgrlc_ta_value_req, CGRLC_TA_VALUE_REQ);
      cgrlc_ta_value_req->ta_value = tav;
      PSEND(hCommGRLC, cgrlc_ta_value_req);
    }
    ptr2ta->ta_value             = tav;
    grr_data->ta_params.ta_value = tav;
    grr_data->ta_params.ta_valid = TRUE;
  }
  else if(grr_data->ta_params.ta_valid) /* use old TA value */
  {
    /*
    TRACE_EVENT("Use old TA value in IA or PUL on (P)AGCH");
    */
    ptr2ta->ta_value = grr_data->ta_params.ta_value;
  }
  else
  {
    /*TRACE_EVENT("No valid TA in neither in message nor stored TA: check TAI");*/
    grr_data->ta_params.ta_value = 0xFF;
    grr_data->ta_params.ta_valid = FALSE;
  }

  /*
   * If TAI present the MS shall use "continuos TA"
   */
  if(v_dl_tai EQ 0)
  {
    /* Reset the tai & ta_tn */
    if(grr_data->ta_params.dl_ta_i NEQ 0xFF)
    {
      TRACE_EVENT_P4("TA_INFO: Switch off DL CTA dl_tai=%d dl_ta_tn=%d ul_tai=%d, ul_ta_tn=%d",
      grr_data->ta_params.dl_ta_i,
      grr_data->ta_params.dl_ta_tn,
      grr_data->ta_params.ul_ta_i,
      grr_data->ta_params.ul_ta_tn);
    }
    grr_data->ta_params.dl_ta_i  = 0xFF;
    grr_data->ta_params.dl_ta_tn = 0xFF;
  }
  else if(v_dl_tai EQ 1)
  {
    /* TRACE_EVENT("DL TAI: cont TA: if UL TAI is also valid, we prefer to use DL TAI"); */
    grr_data->ta_params.dl_ta_i  = dl_tai;
    grr_data->ta_params.dl_ta_tn = dl_tatn;
    ptr2ta->ta_index             = dl_tai;
    ptr2ta->tn                   = dl_tatn;
  }

  if(v_ul_tai EQ 0)
  {
    /* Reset the tai & ta_tn */
    if(grr_data->ta_params.ul_ta_i NEQ 0xFF)
    {
      TRACE_EVENT_P7("TA_INFO: Switch off UL CTA ul_tai=%d ul_ta_tn=%d dl_tai=%d dl_ta_tn=%d v_dl_tai=%d new_dl_tai=%d new_dl_ta_tn=%d",
      grr_data->ta_params.ul_ta_i,
      grr_data->ta_params.ul_ta_tn,
      grr_data->ta_params.dl_ta_i,
      grr_data->ta_params.dl_ta_tn,
      v_dl_tai,
      dl_tai,
      dl_tatn);
    }
    grr_data->ta_params.ul_ta_i  = 0xFF;
    grr_data->ta_params.ul_ta_tn = 0xFF;

    if(use_dl_par AND grr_data->ta_params.dl_ta_i NEQ 0xFF)
    {
      ptr2ta->ta_index             = grr_data->ta_params.dl_ta_i;
      ptr2ta->tn                   = grr_data->ta_params.dl_ta_tn;
    }
  }
  else if(v_ul_tai EQ 1)
  {
    grr_data->ta_params.ul_ta_i  = ul_tai;
    grr_data->ta_params.ul_ta_tn = ul_tatn;
    if(use_dl_par AND grr_data->ta_params.dl_ta_i NEQ 0xFF)
    {
      /* TRACE_EVENT("Conc TBF: use DL TA for UL"); */
      ptr2ta->ta_index             = grr_data->ta_params.dl_ta_i;
      ptr2ta->tn                   = grr_data->ta_params.dl_ta_tn;
    }
    else
    {
      /* TRACE_EVENT("Single TBF or TBF_Type is not set to concurrent: UL TAI: cont TA"); */
      ptr2ta->ta_index             = ul_tai;
      ptr2ta->tn                   = ul_tatn;
    }
  }
  /*
   * store values ,which are passed to layer 1
   */
  grr_data->ta_params.l1_ta_value = ptr2ta->ta_value;
  grr_data->ta_params.l1_ta_i     = ptr2ta->ta_index;
  grr_data->ta_params.l1_ta_tn    = ptr2ta->tn;
/*

  {
    ULONG trace[4];

    trace[0]  = ul_tai   <<  0;
    trace[0] |= v_ul_tai <<  8;
    trace[0] |= tav      << 16;
    trace[0] |= v_tav    << 24;

    trace[1]  = dl_tatn  <<  0;
    trace[1] |= dl_tai   <<  8;
    trace[1] |= v_dl_tai << 16;
    trace[1] |= ul_tatn  << 24;

    trace[2]  = grr_data->ta_params.ul_ta_tn <<  0;
    trace[2] |= grr_data->ta_params.ul_ta_i  <<  8;
    trace[2] |= grr_data->ta_params.ta_value << 16;
    trace[2] |= grr_data->ta_params.ta_valid << 24;

    trace[3]  = grr_data->ta_params.l1_ta_i     <<  0;
    trace[3] |= grr_data->ta_params.l1_ta_value <<  8;
    trace[3] |= grr_data->ta_params.dl_ta_tn    << 16;
    trace[3] |= grr_data->ta_params.dl_ta_i     << 24;

    TRACE_EVENT_P5( "TA_PARAM_1: %08X%08X %08X%08X%02X",
                    trace[0], trace[1], trace[2], trace[3],
                    grr_data->ta_params.l1_ta_tn );
  }
*/
}/* grr_handle_ta */

/*---------------------- GET FROM PPC-----------------------------------------*/
/*
+------------------------------------------------------------------------------
| Function    : grr_decode_grr
+------------------------------------------------------------------------------
| Description : The function grr_decode_grr() calls the function ccd_decodeMsg.
|               After the call the decoded Message is in _decodeCtrlMsg.
|
| Parameters  : msg_ptr_i - pointer to buffer that should be decoded
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE grr_decode_grr (T_MSGBUF  *  msg_ptr_i)
{ 
  UBYTE result;
  UBYTE msg_type = msg_ptr_i->buf[0] >> 2;;

  TRACE_FUNCTION( "grr_decode_grr" );

  /*
   * Offset must be zero, else code to get msg_type is illegal
   */
  TRACE_ASSERT (msg_ptr_i->o_buf==0);

#ifdef _SIMULATION_
  /*
   * If one of the following asserts fail, you have to verify the message
   * decoders.
   */
#ifdef REL99
  TRACE_ASSERT (sizeof(T_D_DL_ASSIGN)   == 0x02E0); /*lint !e774*/
  TRACE_ASSERT (sizeof(T_D_UL_ASSIGN)   == 0x0410); /*lint !e774*/
  TRACE_ASSERT (sizeof(T_D_TS_RECONFIG) == 0x03D8); /*lint !e774*/
#else
  TRACE_ASSERT (sizeof(T_D_DL_ASSIGN)   == 0x024C); /*lint !e774*/
  TRACE_ASSERT (sizeof(T_D_UL_ASSIGN)   == 0x040C); /*lint !e774*/
  TRACE_ASSERT (sizeof(T_D_TS_RECONFIG) == 0x03D4); /*lint !e774*/
#endif
#endif


  switch (msg_type)
  {
    case D_DL_ASSIGN:
      result = grr_decode_dl_assignment (msg_ptr_i->buf, msg_ptr_i->o_buf, msg_ptr_i->l_buf);
      break;

    case D_UL_ASSIGN:
      result = grr_decode_ul_assignment (msg_ptr_i->buf, msg_ptr_i->o_buf, msg_ptr_i->l_buf);
      break;

    case D_TS_RECONFIG:
      result = grr_decode_ts_reconfig (msg_ptr_i->buf, msg_ptr_i->o_buf, msg_ptr_i->l_buf);
      break;
    case D_PDCH_RELEASE:
      result = grr_decode_pdch_release (msg_ptr_i->buf, msg_ptr_i->o_buf, msg_ptr_i->l_buf);
      break;
    case D_POLLING_REQ:
      result = grr_decode_polling_req (msg_ptr_i->buf, msg_ptr_i->o_buf, msg_ptr_i->l_buf);
      break;
    case D_TBF_RELEASE:
      result = grr_decode_tbf_release_req (msg_ptr_i->buf, msg_ptr_i->o_buf, msg_ptr_i->l_buf);
      break;
     
    default:
      result = ccd_decodeMsg (CCDENT_GRR, DOWNLINK, msg_ptr_i, _decodedMsg, NOT_PRESENT_8BIT);
      break;
  }


  if ( result EQ ccdError ) switch (msg_type)
  {
    case D_DL_ASSIGN:
    case D_UL_ASSIGN:
    case D_TS_RECONFIG:
      /* This should not be the final version of error handling */
      return DELETE_MESSAGE;

    default:
      return grr_ccd_error_handling( CCDENT_GRR );
  }

  return msg_type;

} /* grr_decode_grr() */


/*
+------------------------------------------------------------------------------
| Function    : grr_decode_rlcmac
+------------------------------------------------------------------------------
| Description : The function grr_decode_rlcmac() 
|
| Parameters  : ptr_msg_i     - pointer to rlcmac-ctrl message
|               ptr_header_i  - pointer to mac header of the rlc block
+------------------------------------------------------------------------------
*/
GLOBAL void grr_decode_rlcmac ( UBYTE * ptr_msg_i, T_D_HEADER * ptr_d_header_i )
{   
  UBYTE rrbp, sp, payload;
  TRACE_FUNCTION( "grr_decode_rlcmac" );
  /*MAC Header*/
  payload      = (ptr_msg_i[0] & 0xC0) >> 6;
  rrbp         = (ptr_msg_i[0] & 0x30) >> 4;
  sp           = (ptr_msg_i[0] & 0x08) >> 3;
  ptr_d_header_i->payload  = payload;
  /*if ctrl block with optional parameter*/
  if(payload EQ CTRL_BLK_OPT )
  {
    ptr_d_header_i->d_ctrl.rrbp         = rrbp;
    ptr_d_header_i->d_ctrl.sp           = sp;
    ptr_d_header_i->d_ctrl.rbsn         = (ptr_msg_i[1] & 0x80) >> 7;
    ptr_d_header_i->d_ctrl.rti          = (ptr_msg_i[1] & 0x7C) >> 2;
    ptr_d_header_i->d_ctrl.fs           = (ptr_msg_i[1] & 0x02) >> 1;
    ptr_d_header_i->d_ctrl.ac           = (ptr_msg_i[1] & 0x01);
    if(ptr_d_header_i->d_ctrl.ac EQ 1)
    {
      ptr_d_header_i->d_ctrl.msg_type     = ptr_msg_i[3] >> 2;
      ptr_d_header_i->d_ctrl.tfi          = (ptr_msg_i[2] & 0x3E) >> 1;
      ptr_d_header_i->d_ctrl.d            = (ptr_msg_i[2] & 0x01);
      ptr_d_header_i->ptr_block           = &ptr_msg_i[3];
    }
    else
    {
      ptr_d_header_i->d_ctrl.msg_type     = ptr_msg_i[2] >> 2;
      ptr_d_header_i->ptr_block           = &ptr_msg_i[2];
    }

    TRACE_EVENT_P4("optional header received: 0x%x 0x%x 0x%x 0x%x",ptr_msg_i[0],ptr_msg_i[1],ptr_msg_i[2],ptr_msg_i[3]);
  }
  else if(payload EQ CTRL_BLK_NO_OPT )
    /*if ctrl block with optional parameter*/
  {
    ptr_d_header_i->d_ctrl.msg_type     = ptr_msg_i[1] >> 2;
    ptr_d_header_i->d_ctrl.rrbp         = rrbp;
    ptr_d_header_i->d_ctrl.sp           = sp;
    ptr_d_header_i->ptr_block           = &ptr_msg_i[1];
  }
  else
  {
    TRACE_ERROR("unknown payload type");
    TRACE_EVENT_P5("pt=%d FIRST 5 BYTES: 0x%x 0x%x 0x%x 0x%x",payload,ptr_msg_i[0],ptr_msg_i[1],ptr_msg_i[2],ptr_msg_i[3]);
  }

} /* grr_decode_rlcmac() */



/*
+------------------------------------------------------------------------------
| Function    : grr_handle_rlcmac_header
+------------------------------------------------------------------------------
| Description : The function grr_handle_rlcmac_header() 
|
| Parameters  : mode_i - possible values: 
|                        PACKET_MODE_PIM/PACKET_MODE_PAM and 
|                         (that means message was received over MPHP)
|                        PACKET_MODE_PTM
|                         (that means message was received over RLCMAC)
|               remark: the reason for introduce mode_i is lost? 
|               ptr_header_i - pointer to mac header of the rlc block
|               fn_i - frame number
|               
+------------------------------------------------------------------------------
*/
GLOBAL T_MSGBUF * grr_handle_rlcmac_header ( T_PACKET_MODE   mode_i, 
                                             T_D_HEADER    * ptr_header_i,
                                             ULONG           fn_i )
{ 

  T_MSGBUF * result = (T_MSGBUF *)(& grr_data->ctrl_msg);

  UBYTE i;
  UBYTE frame_periods;

  TRACE_FUNCTION( "grr_handle_rlcmac_header" );

  if(ptr_header_i->payload EQ CTRL_BLK_NO_OPT)
  {
    /*
     * not segmented control block
     */
    result->l_buf = 22*8;
    result->o_buf = 0;
    memcpy(result->buf, ptr_header_i->ptr_block, (result->l_buf)/8);
    ptr_header_i->pctrl_ack = 3;
  } 
  else if (ptr_header_i->payload EQ CTRL_BLK_OPT)
  { 
    /*
     * segmented control block
     */
    if(ptr_header_i->d_ctrl.ac)
      result->l_buf = (22-2)*8;
    else
      result->l_buf = (22-1)*8;
    
    if(ptr_header_i->d_ctrl.fs AND !ptr_header_i->d_ctrl.rbsn)
    {
      result->o_buf  = 0;
      memcpy(result->buf, ptr_header_i->ptr_block, (result->l_buf)/8);
      ptr_header_i->pctrl_ack = 2;
      /*
      TRACE_EVENT("optional rlc mac header without segmentation");
      */
    }
    else if(ptr_header_i->d_ctrl.fs)
    {
      /*
       * check if the first part is received, otherwise delete
       */
      grr_clean_up_seg_ctrl_blk_tbl();
/*      for(i=grr_data->seg_ctrl_blk.next-1;i NEQ grr_data->seg_ctrl_blk.next;((i--)%8))*/
      for(i = 0;i < SEG_CTRL_BLOCK_SIZE ;i++)
      {
        if(ptr_header_i->d_ctrl.rti EQ grr_data->seg_ctrl_blk.blk[i].rti)
        { 
          /*
           * a part of the recently received control block is saved, 
           * reassabling is possible 
           */
          if(!(ptr_header_i->d_ctrl.rbsn))
          { /*
             * received part is the first part of the segmented block
             * copy recently received as first part
             */
            memcpy(result->buf, ptr_header_i->ptr_block, (result->l_buf)/8);
            /*
             * copy previous saved as second part 
             */
            memcpy(result->buf+(result->l_buf)/8,grr_data->seg_ctrl_blk.blk[i].ctrl_blk.buf, 
                                        (grr_data->seg_ctrl_blk.blk[i].ctrl_blk.l_buf)/8);
          }
          else
          { 
            /*
             * received part is the second part of the segmented block
             * copy saved part as first part
             */
            memcpy(result->buf, grr_data->seg_ctrl_blk.blk[i].ctrl_blk.buf, 
                          (grr_data->seg_ctrl_blk.blk[i].ctrl_blk.l_buf)/8);
            /*
             * copy recently received as second part
             */
            memcpy(result->buf+(grr_data->seg_ctrl_blk.blk[i].ctrl_blk.l_buf)/8, 
                                     ptr_header_i->ptr_block, (result->l_buf)/8);
          }
          result->l_buf += grr_data->seg_ctrl_blk.blk[i].ctrl_blk.l_buf;
          result->o_buf  = 0;
          ptr_header_i->pctrl_ack = 3;
          grr_data->seg_ctrl_blk.blk[i].rti = 0xFF;
          /*
           * break condition: saved part was found 
           */
          break; 
        }
        else
        {
          if(i EQ SEG_CTRL_BLOCK_SIZE)
          {
            result = NULL;  
            TRACE_ERROR("final segment but no identical rti values");
          }
        }
      }
    }
    else
    {
      /*
       * save the first part of a control block
       */
      if(0xff EQ grr_data->seg_ctrl_blk.next)
        grr_data->seg_ctrl_blk.next = 0; /* for safety only (former glumpshack) */
      if(0xFF NEQ grr_data->seg_ctrl_blk.blk[grr_data->seg_ctrl_blk.next].rti)
      {
        grr_clean_up_seg_ctrl_blk_tbl();
        grr_align_seg_ctrl_blk_nxt();
      }
      
      /*
       * calculate the timeout value for T3200 in untis of frames 
       */       
      if( mode_i                  EQ PACKET_MODE_PIM AND
          grr_is_non_drx_period( )           )
      {
        frame_periods = 4 * MAXIMUM( 1, psc_db->gprs_cell_opt.bs_cv_max );
      }
      else
      {
        USHORT drx_p;
        drx_p = meas_im_get_drx_period_frames( );
        frame_periods = MAXIMUM( 1,drx_p );
      }

      grr_data->seg_ctrl_blk.blk[grr_data->seg_ctrl_blk.next].rbsn  = 
                                         ptr_header_i->d_ctrl.rbsn;
      grr_data->seg_ctrl_blk.blk[grr_data->seg_ctrl_blk.next].rti   = 
                                         ptr_header_i->d_ctrl.rti;
      grr_data->seg_ctrl_blk.blk[grr_data->seg_ctrl_blk.next].T3200 = 
        ( fn_i + 4 * frame_periods ) % FN_MAX;
      grr_data->seg_ctrl_blk.blk[grr_data->seg_ctrl_blk.next].ctrl_blk.l_buf = 
                                                               result->l_buf;
      grr_data->seg_ctrl_blk.blk[grr_data->seg_ctrl_blk.next].ctrl_blk.o_buf = 0;
      memcpy(grr_data->seg_ctrl_blk.blk[grr_data->seg_ctrl_blk.next].ctrl_blk.buf, 
                                        ptr_header_i->ptr_block, (result->l_buf/8));
      
/*      (grr_data->seg_ctrl_blk.next++)%8; */
      grr_clean_up_seg_ctrl_blk_tbl();
      grr_align_seg_ctrl_blk_nxt();

      if(ptr_header_i->d_ctrl.rbsn)
        ptr_header_i->pctrl_ack = 1; 
      else
        ptr_header_i->pctrl_ack = 2; 
      result = NULL;
    }    
  }
  else
    {
      TRACE_ERROR(" payload type is not a control block ");
    }
  return(result);

} /* grr_handle_rlcmac_header() */


/*
+------------------------------------------------------------------------------
| Function    : grr_check_address
+------------------------------------------------------------------------------
| Description : The function grr_check_address() this function checks the 
|               message of the message in _decodeCtrlMsg. The Function returns 
|               TRUE if the address is correct.    
|
| Parameters  : msg_type_i - the airmessage type defined in M_GRR.val
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_check_address ( UBYTE msg_type_i,UBYTE tn_i )
{ 

  BOOL result = FALSE;
  TRACE_FUNCTION( "grr_check_address" );

  switch( msg_type_i )
    {
    case D_ACCESS_REJ_c:
      result = grr_check_add_reject(tn_i);
      break;

    case D_QUEUING_NOT_c:
      {
        MCAST(queuing_not,D_QUEUING_NOT);
        result = grr_check_request_reference(&(queuing_not->req_ref_p));
      }
      break;

    case D_TBF_RELEASE_c:
      {
        MCAST(tbf_release,D_TBF_RELEASE);
        result = grr_check_glob_tfi(&(tbf_release->glob_tfi),tn_i );
      }
      break;

    case D_TS_RECONFIG_c:
      {
        MCAST(d_ts_reconfig,D_TS_RECONFIG);
        result = grr_check_glob_tfi( &(d_ts_reconfig->glob_tfi),tn_i );
      }
      break;

    case D_PAGING_REQ_c:
      result = TRUE;
        /*grr_check_page_add();*/
      break;

    case D_UL_ACK_c:
      {
        MCAST(ul_ack,D_UL_ACK);
        if((grr_data->uplink_tbf.tfi EQ ul_ack->ul_tfi) AND
           ((0x80>>tn_i) & grr_data->uplink_tbf.ts_mask))
        {
          result = TRUE;
        }
        else
        {
          TRACE_EVENT_P6("ul_ack adress failed add_tfi=%d, ul_tfi=%d, || tn=%d ->mask=%x curr_mask=%x, st_mask=%x ",
                                        ul_ack->ul_tfi,
                                        grr_data->uplink_tbf.tfi,
                                        tn_i,
                                        0x80>>tn_i,
                                        grr_data->uplink_tbf.ts_mask,
                                        grr_data->uplink_tbf.ts_usage);

        }
      }
      break;

    case D_CELL_CHAN_ORDER_c:
      {
        MCAST(d_cell_chan_order,D_CELL_CHAN_ORDER);
        result = grr_check_add_1( &(d_cell_chan_order->add1),tn_i );
      }
      break;

    case D_MEAS_ORDER_c:
      {
        MCAST(d_meas_order,D_MEAS_ORDER);
        result = grr_check_add_1( &(d_meas_order->add1),tn_i );
      }
      break;

    case D_DL_ASSIGN_c:
      {
        MCAST(d_dl_assign,D_DL_ASSIGN);
        result = grr_check_add_1( &(d_dl_assign->add1),tn_i );
      }
      break;

    case D_POLLING_REQ_c:
      {
        MCAST(d_polling_req,D_POLLING_REQ);
        result = grr_check_add_2( &(d_polling_req->add2),tn_i );
      }
      break;

    case D_CTRL_PWR_TA_c:
      {
        MCAST(d_ctrl_pwr_ta,D_CTRL_PWR_TA);
        result = grr_check_add_4( &(d_ctrl_pwr_ta->add4),tn_i );
      }
      break;

    case D_UL_ASSIGN_c:
      {
        MCAST(d_ul_assign,D_UL_ASSIGN);
        result = grr_check_add_3( &(d_ul_assign->add3),tn_i );
      }
      break;

      /*
       * the following messages do not have a address field 
       * therefore the result is always TRUE
       */
    case PSI_1_c:            
    case PSI_2_c:                
    case PSI_3_c:               
    case PSI_3_BIS_c:
#if defined (REL99) AND defined (TI_PS_FF_EMR)
    case PSI_3_TER_c:
#endif
    case PSI_4_c:           
    case PSI_5_c:
#ifdef REL99
    case PSI_8_c:               
#endif
    case PSI_13_c:               
    case D_DL_DUMMY_c:   
    case D_PDCH_RELEASE_c:
    case D_PRACH_PAR_c:       
      result = TRUE;
      break;

    default:
      TRACE_ERROR("grr_check_address with invalid message type");
      break;
  } /* switch (msg_type_i) */
  
  return(result); 
  
} /* grr_check_address() */


/*
+------------------------------------------------------------------------------
| Function    : grr_increase_dsc
+------------------------------------------------------------------------------
| Description : The function grr_increase_dsc increases dsc by 1
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/          
GLOBAL void grr_increase_dsc ( void )
{
  TRACE_FUNCTION( "grr_increase_dsc" );
  
  grr_data->pg.dsc++;

  if(grr_data->pg.dsc > grr_data->pg.initial_dsc)
  {
    grr_data->pg.dsc = grr_data->pg.initial_dsc;
  }

  GRR_EM_SET_DSC_VAL(grr_data->pg.dsc);
  
#ifdef _SIMULATION_
  TRACE_EVENT_P1( "DSC: %d", grr_data->pg.dsc );
#endif /* #ifdef _SIMULATION_ */
}/* grr_increase_dsc*/

/*
+------------------------------------------------------------------------------
| Function    : grr_decrease_dsc
+------------------------------------------------------------------------------
| Description : The function grr_decrease_dsc decreases dsc by 4
|
| Parameters  : return BOOL: indicates whether downlink siganlling failure 
|               occured or not
|
+------------------------------------------------------------------------------
*/          
GLOBAL BOOL grr_decrease_dsc ( void )
{
  TRACE_FUNCTION( "grr_decrease_dsc" );
  
  if( grr_data->pg.dsc <= 4 )
  {
    grr_data->pg.dsc  = 0;
  }
  else
  {
    grr_data->pg.dsc -= 4;
  }

  GRR_EM_SET_DSC_VAL(grr_data->pg.dsc);

#ifdef _SIMULATION_
  TRACE_EVENT_P1( "DSC: %d", grr_data->pg.dsc );
#endif /* #ifdef _SIMULATION_ */

  return( grr_data->pg.dsc EQ 0 );
}/* grr_decrease_dsc*/

/*
+------------------------------------------------------------------------------
| Function    : grr_t_status
+------------------------------------------------------------------------------
| Description : This function returns the remaining time in milliseconds.
|               A value of 0L is returned in case the timer is not existing or
|               not running.
|
| Parameters  : t_index : timer index
|
+------------------------------------------------------------------------------
*/
GLOBAL T_TIME grr_t_status( USHORT t_index )
{
  T_TIME t_time = 0L;
  
  TRACE_FUNCTION( "grr_t_status" );

  vsi_t_status( GRR_handle, t_index, &t_time );

  return( t_time );
} /* grr_t_status */ 

/*
+------------------------------------------------------------------------------
| Function    : grr_get_nc_mval
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL T_NC_MVAL* grr_get_nc_mval ( USHORT arfcn, UBYTE bsic, UBYTE *idx )
{
  UBYTE      i;
  T_NC_MVAL *nc_mval = NULL;

  TRACE_FUNCTION( "grr_get_nc_mval" );

  for( i = 0; i < MAX_NR_OF_NC_MVAL AND nc_mval EQ NULL; i++ )
  {
    nc_mval = &grr_data->db.nc_mval_list.nc_mval[i];
    *idx    = i;

    if( bsic EQ RRGRR_INVALID_BSIC )
    {
      if( nc_mval->arfcn NEQ arfcn )
      {
        nc_mval = NULL;
      }
    }
    else
    {
      if( nc_mval->arfcn NEQ arfcn OR nc_mval->sync_info.bsic NEQ bsic )
      {
        nc_mval = NULL;
      }
    }
  }

  return( nc_mval );
} /* grr_get_nc_mval */ 

/*
+------------------------------------------------------------------------------
| Function    : grr_get_ncell_info
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL T_ncell_info* grr_get_ncell_info ( USHORT arfcn, UBYTE bsic )
{
  UBYTE         i;
  T_ncell_info *ncell_info = NULL;

  TRACE_FUNCTION( "grr_get_ncell_info" );

  for( i = 0; i < grr_data->db.nc_ref_lst.number AND ncell_info EQ NULL; i++ )
  {
    ncell_info = grr_data->db.nc_ref_lst.info[i];

    if( bsic EQ RRGRR_INVALID_BSIC )
    {
      if( ncell_info->arfcn NEQ arfcn )
      {
        ncell_info = NULL;
      }
    }
    else
    {
      if( ncell_info->arfcn NEQ arfcn OR ncell_info->bsic NEQ bsic )
      {
        ncell_info = NULL;
      }
    }
  }

  return( ncell_info );
} /* grr_get_ncell_info */ 

/*
+------------------------------------------------------------------------------
| Function    : grr_set_pbcch
+------------------------------------------------------------------------------
| Description : This function sets pbcch presence. It is no more handled 
|               in PSI only.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_set_pbcch ( BOOL pbcch_presence )
{
  TRACE_FUNCTION( "grr_set_pbcch" );

  psc_db->pbcch.bcch.pbcch_present = pbcch_presence;

#if !defined (NTRACE)

  if( grr_data->cs.v_crp_trace EQ TRUE )
  {
    TRACE_EVENT_P3( "database: psc = %d, PBCCH presence: %d %d",
                    grr_get_db_num( psc_db ), 
                    grr_data->sc_db_1.pbcch.bcch.pbcch_present, 
                    grr_data->sc_db_2.pbcch.bcch.pbcch_present );
  }

#endif /* #if !defined (NTRACE) */

} /* grr_set_pbcch */

/*
+------------------------------------------------------------------------------
| Function    : grr_set_pg_nmo
+------------------------------------------------------------------------------
| Description : This function sets NMO for paging
|               If NMO II and PBCCH present, then assume paging coordination
|               is used. The MS will act as NMO I mobile with PBCCH
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_set_pg_nmo(void)
{
  if( psc_db->pbcch.bcch.pbcch_present
      AND 
      psc_db->cell_info_for_gmm.cell_info.net_mode EQ GMMRR_NET_MODE_II )
  {
    grr_data->pg.nmo = GMMRR_NET_MODE_I;
    TRACE_EVENT("Network is NMO II, but with PBCCH. The MS will act as NMO I (paging coordination)");
  }
  else
    grr_data->pg.nmo = psc_db->cell_info_for_gmm.cell_info.net_mode;
}/*grr_set_pg_nmo*/

/*
+------------------------------------------------------------------------------
| Function    : grr_set_freq_par
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_set_freq_par ( T_p_frequency_par *freq_par )
{
  TRACE_FUNCTION( "grr_set_freq_par" );

  freq_par->p_chan_sel = grr_data->tc.freq_set.freq_par.p_chan_sel;

  if( grr_data->tc.freq_set.freq_par.p_chan_sel.hopping )
  {
    freq_par->p_freq_list = grr_data->tc.freq_set.freq_par.p_freq_list;
  }
  else
  {
    memset( &freq_par->p_freq_list, 0, sizeof( freq_par->p_freq_list ) );
  }
} /* grr_set_freq_par */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_pccch_freq_par
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_get_pccch_freq_par ( UBYTE          pccch_group, 
                                     T_p_chan_sel  *chan_sel,
                                     T_p_freq_list *freq_list )
{
  BOOL result = FALSE;

  TRACE_FUNCTION( "grr_get_pccch_freq_par" );

  if( pccch_group > psc_db->paging_group.kc - 1 )
  {
    TRACE_ERROR( "pccch_group > psc_db->paging_group.kc - 1" );

    return( result );
  }

  if( psc_db->paging_group.pccch[pccch_group].is_static )
  {
    chan_sel->hopping       = 0;
    chan_sel->p_rf_ch.arfcn = psc_db->paging_group.pccch[pccch_group].arfcn;

    result = TRUE;
  }
  else
  {
    result = 
      grr_create_freq_list(  psc_db->paging_group.pccch[pccch_group].ma_num,
                             psc_db->paging_group.pccch[pccch_group].maio,
                             chan_sel,
                             freq_list );
  }

  return( result );
} /* grr_get_pccch_freq_par */

/*
+------------------------------------------------------------------------------
| Function    : grr_cgrlc_pwr_ctrl_cnf
+------------------------------------------------------------------------------
| Description : Handles the primitive CGRLC_PWR_CTRL_CNF
|
| Parameters  : cgrlc_pwr_ctrl_cnf - Ptr to primitive payload  
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_cgrlc_pwr_ctrl_cnf ( T_CGRLC_PWR_CTRL_CNF *cgrlc_pwr_ctrl_cnf )
{ 
  TRACE_FUNCTION( "grr_cgrlc_pwr_ctrl_cnf" );

  PFREE( cgrlc_pwr_ctrl_cnf );

} /* grr_cgrlc_pwr_ctrl_cnf() */

/*
+------------------------------------------------------------------------------
| Function    : grr_cgrlc_pwr_ctrl_req
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_cgrlc_pwr_ctrl_req( BOOL v_c_value )
{
  TRACE_FUNCTION( "grr_cgrlc_pwr_ctrl_req" );

  {
    PALLOC( cgrlc_pwr_ctrl_req, CGRLC_PWR_CTRL_REQ );

    grr_prcs_pwr_ctrl( &cgrlc_pwr_ctrl_req->pwr_ctrl, v_c_value );

    if( cgrlc_pwr_ctrl_req->pwr_ctrl.v_pwr_ctrl_param      EQ FALSE AND
        cgrlc_pwr_ctrl_req->pwr_ctrl.v_glbl_pwr_ctrl_param EQ FALSE AND
        cgrlc_pwr_ctrl_req->pwr_ctrl.v_freq_param          EQ FALSE AND
        cgrlc_pwr_ctrl_req->pwr_ctrl.v_c_value             EQ FALSE     )
    {
      PFREE( cgrlc_pwr_ctrl_req );
    }
    else
    {
      PSEND( hCommGRLC, cgrlc_pwr_ctrl_req );
    }
  }
} /* grr_cgrlc_pwr_ctrl_req() */

/*
+------------------------------------------------------------------------------
| Function    : grr_prcs_pwr_ctrl
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_prcs_pwr_ctrl ( T_CGRLC_pwr_ctrl *pwr_ctrl, BOOL v_c_value )
{
  USHORT pdch_arfcn;

  TRACE_FUNCTION( "grr_prcs_pwr_ctrl" );

  if( grr_data->tc.v_freq_set )
  {
    /*
     * Apply the ARFCN value obtained from the assignment message.
     *
     * When getting the ARFCN value, reconvert it to the absolute
     * ARFCN value, since grr_data->tc has L1 mapped ARFCN values.
     */
    if( grr_data->tc.freq_set.freq_par.p_chan_sel.hopping )
    {
      /* If hopping is enabled, then take the first ARFCN value from the assignment */
      pdch_arfcn = 
        grr_l1_arfcn_to_g23
          ( grr_data->tc.freq_set.freq_par.p_freq_list.p_rf_chan_no.p_radio_freq[0] );
    }
    else 
    {
      /* If hopping is disabled, then take the ARFCN value from the assignment */
      pdch_arfcn = 
        grr_l1_arfcn_to_g23
          ( grr_data->tc.freq_set.freq_par.p_chan_sel.p_rf_ch.arfcn );
    }
  }
  else
  {
    pdch_arfcn = psc_db->pbcch.bcch.arfcn;
  }
   
  if( grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param )
  {
    T_CGRLC_pwr_ctrl_param *pwr_ctrl_param = &pwr_ctrl->pwr_ctrl_param;

    if( psc_db->v_pwr_par EQ TRUE AND psc_db->pwr_par.v_alpha EQ TRUE )
    {
      pwr_ctrl_param->alpha = psc_db->pwr_par.alpha;
    }
    else
    {
      pwr_ctrl_param->alpha = CGRLC_ALPHA_INVALID;
    }
        
    {
      UBYTE i;

      for( i = 0; i < CGRLC_MAX_TIMESLOTS; i++ )
      {
        if( psc_db->v_pwr_par                   EQ TRUE AND
            psc_db->pwr_par.gamma_tn[i].v_gamma EQ TRUE     )
        {
          pwr_ctrl_param->gamma_ch[i] = psc_db->pwr_par.gamma_tn[i].gamma;
        }
        else
        {
          pwr_ctrl_param->gamma_ch[i] = CGRLC_GAMMA_INVALID;
        }
      }
    }
  }

  if( grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param )
  {
    T_CGRLC_glbl_pwr_ctrl_param *glbl_pwr_ctrl_param =
                                                &pwr_ctrl->glbl_pwr_ctrl_param;
    
    T_MS_PWR_CAP mspc;

    glbl_pwr_ctrl_param->alpha        = psc_db->g_pwr_par.alpha;
    glbl_pwr_ctrl_param->t_avg_t      = psc_db->g_pwr_par.t_avg_t;
    glbl_pwr_ctrl_param->pb           = psc_db->g_pwr_par.pb;
    glbl_pwr_ctrl_param->pc_meas_chan = psc_db->g_pwr_par.pc_meas_chan;
    glbl_pwr_ctrl_param->pwr_max      = grr_get_pms_max
                                          (  pdch_arfcn,
                                             grr_data->meas.pwr_offset,
                                             psc_db->scell_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch,
                                            &mspc );
  }

  if( grr_data->pwr_ctrl_valid_flags.v_freq_param )
  {
    T_CGRLC_freq_param *freq = &pwr_ctrl->freq_param;

    T_MS_PWR_CAP mspc;

    grr_get_ms_pwr_cap(  pdch_arfcn, grr_data->meas.pwr_offset, &mspc );
    
    freq->bcch_arfcn   = grr_g23_arfcn_to_l1( psc_db->pbcch.bcch.arfcn );
    freq->pdch_hopping = ( grr_data->tc.v_freq_set                           AND 
                           grr_data->tc.freq_set.freq_par.p_chan_sel.hopping     );
    freq->pdch_band    = mspc.band_ind;
  }
      
  if( v_c_value )
  {
    if( grr_data->pwr_ctrl_valid_flags.v_c_value )
    {
      meas_c_get_c_value( &pwr_ctrl->c_value );
    }

    pwr_ctrl->v_c_value           = grr_data->pwr_ctrl_valid_flags.v_c_value;
    grr_data->pwr_ctrl_valid_flags.v_c_value           = FALSE;
  }
  else
  {
    pwr_ctrl->v_c_value           = v_c_value;
  }

  pwr_ctrl->v_pwr_ctrl_param      = grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param;
  pwr_ctrl->v_glbl_pwr_ctrl_param = grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param;
  pwr_ctrl->v_freq_param          = grr_data->pwr_ctrl_valid_flags.v_freq_param;

  grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param      = FALSE;
  grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param = FALSE;
  grr_data->pwr_ctrl_valid_flags.v_freq_param          = FALSE;

} /* grr_prcs_pwr_ctrl */

/*
+------------------------------------------------------------------------------
| Function    : grr_set_cell_info_service
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_set_cell_info_service( void )
{
  TRACE_FUNCTION( "grr_set_cell_info_service" );

  if     ( psc_db->cell_info_for_gmm.gprs_service  EQ GPRS_SERVICE_NONE       )
  {
    psc_db->cell_info_for_gmm.cell_info.service_state = GMMRR_SERVICE_NONE;
  }
  else if( psc_db->cell_info_for_gmm.gprs_service  EQ GPRS_SERVICE_FULL   AND
           psc_db->cell_info_for_gmm.access_status EQ GPRS_ACCESS_ALLOWED     )
  {
    psc_db->cell_info_for_gmm.cell_info.service_state = GMMRR_SERVICE_FULL;
  }
  else
  {
    psc_db->cell_info_for_gmm.cell_info.service_state = GMMRR_SERVICE_LIMITED;
  }
} /* grr_set_cell_info_service */

/*
+------------------------------------------------------------------------------
| Function    : grr_is_packet_idle_mode
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_is_packet_idle_mode( void )
{
  /*
   * The entity GRR as a whole is in packet idle mode in case the three state
   * machines below are in packet idle mode in detail.
   *
   * There are state transitions, in which one of these state machines is in
   * packet idle mode, but the others are in different states (e.g. during
   * processing of RRGRR_STOP_TASK_REQ/CNF or MPHP_ASSIGNMENT_REQ/CON). That's
   * why all three states shall be considered.
   */

  return( GET_STATE( CTRL_GLBL ) EQ GLBL_PCKT_MODE_IDLE AND
          GET_STATE( TC        ) EQ TC_PIM              AND 
          GET_STATE( CPAP      ) EQ CPAP_IDLE               );

} /* grr_is_packet_idle_mode */

/*
+------------------------------------------------------------------------------
| Function    : grr_clip_rxlev
+------------------------------------------------------------------------------
| Description : This function is used to clip received signal level values.
|
| Parameters  : clipp  - pointer to clipped received signal level values
|               rxlev  - pointer to received signal level values
|               number - number of received signal level values
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_clip_rxlev ( UBYTE *clipp, UBYTE *rxlev, UBYTE number )
{ 
  UBYTE i; /* used for counting */
  
  TRACE_FUNCTION( "grr_clip_rxlev" );

  for( i = 0; i < number; i++ )
  { 
    if( (signed char)( rxlev[i] ) <   CGRLC_RXLEV_MIN  AND
                       rxlev[i]   NEQ CGRLC_RXLEV_NONE     )
    {
      clipp[i] = CGRLC_RXLEV_MIN;
    }
    else if ( (signed char)( rxlev[i] ) > CGRLC_RXLEV_MAX )
    {
      clipp[i] = CGRLC_RXLEV_MAX;
    }
    else if( rxlev[i] EQ CGRLC_RXLEV_NONE )
    {
      clipp[i] = CGRLC_RXLEV_NONE;
    }
    else
    {
      clipp[i] = rxlev[i];
    }
  }
} /* grr_clip_rxlev() */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_pms_max
+------------------------------------------------------------------------------
| Description : This function is used to ...
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE grr_get_pms_max ( USHORT        arfcn,
                               UBYTE         pwr_offset,
                               UBYTE         gprs_ms_txpwr_max_cch,
                               T_MS_PWR_CAP *mspc                   )
{ 
  UBYTE pms_max; /* maximum output power applied by the MS */
  
  TRACE_FUNCTION( "grr_get_pms_max" ); 

  grr_get_ms_pwr_cap( arfcn, pwr_offset, mspc );

  pms_max = mspc->pwr_offset + mspc->p_control[gprs_ms_txpwr_max_cch];
  pms_max = MINIMUM( pms_max, mspc->p[mspc->ms_power] );

  return( pms_max );

} /* grr_get_pms_max() */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_ms_txpwr_max_cch
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE grr_get_ms_txpwr_max_cch ( USHORT arfcn,
                                        UBYTE  pwr_offset,
                                        UBYTE  gprs_ms_txpwr_max_cch )
{ 
  T_MS_PWR_CAP mspc;    /* MS power capabilities                  */
  UBYTE        pms_max; /* maximum output power applied by the MS */

  TRACE_FUNCTION( "grr_get_ms_txpwr_max_cch" );

  pms_max = grr_get_pms_max( arfcn, pwr_offset, gprs_ms_txpwr_max_cch, &mspc );

  return( grr_get_pcl( mspc.p_control, pms_max ) );

} /* grr_get_ms_txpwr_max_cch() */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_ms_pwr_cap
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_get_ms_pwr_cap ( USHORT        arfcn,
                                 UBYTE         pow_offset,
                                 T_MS_PWR_CAP *mspc        )
{ 
  TRACE_FUNCTION( "grr_get_ms_pwr_cap" );
  
  switch( std )
  {
    case STD_850:
      grr_get_ms_pwr_cap_gsm900
        ( grr_get_power_class( IDX_PWRCLASS_850 ), mspc );
      break;

    case STD_900:
    case STD_EGSM:
      grr_get_ms_pwr_cap_gsm900
        ( grr_get_power_class( IDX_PWRCLASS_900 ), mspc );
      break;

    case STD_1900:
      grr_get_ms_pwr_cap_pcs1900
        ( grr_get_power_class( IDX_PWRCLASS_1900 ), pow_offset, mspc );
      break;

    case STD_1800:
      grr_get_ms_pwr_cap_dcs1800
        ( grr_get_power_class( IDX_PWRCLASS_1800 ), pow_offset, mspc );
      break;

    case STD_DUAL:
      /*
       * For dualband mobiles the calculation depends on the channel number
       */
      if( arfcn < LOW_CHANNEL_1800 )
      {
        /*
         * All GSM 900
         */
        grr_get_ms_pwr_cap_gsm900
          ( grr_get_power_class( IDX_PWRCLASS_900 ), mspc );
      }
      else
      {
        /*
         * All DCS 1800 channels
         */
        grr_get_ms_pwr_cap_dcs1800
          ( grr_get_power_class( IDX_PWRCLASS_1800 ), pow_offset, mspc );
      }
      break;

    case STD_DUAL_EGSM:
      if( arfcn < LOW_CHANNEL_1800  OR
          arfcn > HIGH_CHANNEL_1800    )
      {
        /*
         * All GSM 900 and E-GSM channels
         */
        grr_get_ms_pwr_cap_gsm900
          ( grr_get_power_class( IDX_PWRCLASS_900 ), mspc );
      }
      else
      {
        /*
         * All DCS 1800 channels
         */
        grr_get_ms_pwr_cap_dcs1800
          ( grr_get_power_class( IDX_PWRCLASS_1800 ), pow_offset, mspc );
      }
      break;

    case STD_DUAL_US:
      if( arfcn < LOW_CHANNEL_1900 )
      {
        /*
         * All GSM 850 channels
         */
        grr_get_ms_pwr_cap_gsm900
          ( grr_get_power_class( IDX_PWRCLASS_850 ), mspc );
      }
      else
      {
        /*
         * All PCS 1900 channels
         */
        grr_get_ms_pwr_cap_pcs1900
          ( grr_get_power_class( IDX_PWRCLASS_1900 ), pow_offset, mspc );
      }
      break;

#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
      case STD_850_1800:
      if( arfcn < LOW_CHANNEL_1800 )
      {
        /*
         * All GSM 850 channels
         */
        grr_get_ms_pwr_cap_gsm900
          ( grr_get_power_class( IDX_PWRCLASS_850 ), mspc );
      }
      else
      {
        /*
         * All DCS 1800 channels
         */
        grr_get_ms_pwr_cap_dcs1800
          ( grr_get_power_class( IDX_PWRCLASS_1800 ), pow_offset, mspc );
      }
      break;

      case STD_900_1900:
      if( arfcn >= LOW_CHANNEL_1900  AND
          arfcn <= HIGH_CHANNEL_1900 )
      {
        /*
         * All PCS 1900 channels
         */
        grr_get_ms_pwr_cap_pcs1900
          ( grr_get_power_class( IDX_PWRCLASS_1900 ), pow_offset, mspc );

      }
      else
      {
        /*
         * All GSM 900 and E-GSM channels
         */
        grr_get_ms_pwr_cap_gsm900
          ( grr_get_power_class( IDX_PWRCLASS_900 ), mspc );
      }
      break;
        
      case STD_850_900_1800:
      if( arfcn >= LOW_CHANNEL_1800  AND
          arfcn <= HIGH_CHANNEL_1800 )
      {
        /*
         * All DCS 1800 channels
         */
        grr_get_ms_pwr_cap_dcs1800
          ( grr_get_power_class( IDX_PWRCLASS_1800 ), pow_offset, mspc );

      }
      else
      {
        if((arfcn >= LOW_CHANNEL_850) AND (arfcn <= HIGH_CHANNEL_850))
        {
          /*
           * All GSM 850 channels
           */
          grr_get_ms_pwr_cap_gsm900
            ( grr_get_power_class( IDX_PWRCLASS_850 ), mspc );
        }
        else
        {
          /*
           * All GSM 900 and E-GSM channels
           */
          grr_get_ms_pwr_cap_gsm900
            ( grr_get_power_class( IDX_PWRCLASS_900 ), mspc );
        }
      }
      break;

      case STD_850_900_1900:
      if( arfcn >= LOW_CHANNEL_1900  AND
          arfcn <= HIGH_CHANNEL_1900 )
      {
        /*
         * All PCS 1900 channels
         */
        grr_get_ms_pwr_cap_pcs1900
          ( grr_get_power_class( IDX_PWRCLASS_1900 ), pow_offset, mspc );

      }
      else
      {
        if((arfcn >= LOW_CHANNEL_850) AND (arfcn <= HIGH_CHANNEL_850))
        {
          /*
           * All GSM 850 channels
           */
          grr_get_ms_pwr_cap_gsm900
            ( grr_get_power_class( IDX_PWRCLASS_850 ), mspc );
        }
        else
        {
          /*
           * All GSM 900 and E-GSM channels
           */
          grr_get_ms_pwr_cap_gsm900
            ( grr_get_power_class( IDX_PWRCLASS_900 ), mspc );
        }
      }
      break;

      default :
        TRACE_EVENT_P1 (" std : %d",std);
        break;
#endif
  }
} /* grr_get_ms_pwr_cap */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_ms_pwr_cap_gsm900
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_get_ms_pwr_cap_gsm900 ( UBYTE         rf_pow_cap,
                                        T_MS_PWR_CAP *mspc        )
{ 
  TRACE_FUNCTION( "grr_get_ms_pwr_cap_gsm900" );

  /*
   * The table for the power class conversion in GSM 900
   */
  mspc->p          = p_gsm;

  /*
   * The table for the GPRS_MAX_TXPWR_CCCH conversion in GSM 900
   */
  mspc->p_control  = p_control_gsm;

  /*
   * The MS power class is defined in classmark 2 of the non-volatile
   * memory data for GSM 900.
   */
  mspc->ms_power   = rf_pow_cap - 1;

  /*
   * For a power class 3 mobile in the DCS or PCS frequency 
   * standard an additional power offset can be defined
   *
   * Note: This parameter is only available for the serving cell
   *
   */
  mspc->pwr_offset = 0;

  /*
   * Maximum allowed output power in the cell
   */
  mspc->pwr_max    = p_control_gsm[MAX_PCL_GSM900];
   
  /*
   * Minimum required output power in the cell 
   */
  mspc->pwr_min    = p_control_gsm[MIN_PCL_GSM900];

  mspc->gamma_0    = GAMMA_0_GSM900;

  mspc->band_ind   = CGRLC_GSM_900;
} /* grr_get_ms_pwr_cap_gsm900 */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_ms_pwr_cap_dcs1800
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_get_ms_pwr_cap_dcs1800 ( UBYTE         rf_pow_cap,
                                         UBYTE         pow_offset,
                                         T_MS_PWR_CAP *mspc        )
{ 
  TRACE_FUNCTION( "grr_get_ms_pwr_cap_dcs1800" );

  mspc->p          = p_dcs;
  mspc->p_control  = p_control_dcs;
  mspc->ms_power   = rf_pow_cap - 1;
  mspc->pwr_offset = ( ( rf_pow_cap EQ POWER_CLASS_3 ) ? 2 * pow_offset : 0 );
  mspc->pwr_max    = p_control_dcs[MAX_PCL_DCS1800];
  mspc->pwr_min    = p_control_dcs[MIN_PCL_DCS1800];
  mspc->gamma_0    = GAMMA_0_DCS1800;
  mspc->band_ind   = CGRLC_DCS_1800;
} /* grr_get_ms_pwr_cap_dcs1800 */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_ms_pwr_cap_pcs1900
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_get_ms_pwr_cap_pcs1900 ( UBYTE         rf_pow_cap,
                                         UBYTE         pow_offset,
                                         T_MS_PWR_CAP *mspc        )
{ 
  TRACE_FUNCTION( "grr_get_ms_pwr_cap_pcs1900" );

  mspc->p          = p_pcs;
  mspc->p_control  = p_control_pcs;
  mspc->ms_power   = rf_pow_cap - 1;
  mspc->pwr_offset = ( ( rf_pow_cap EQ POWER_CLASS_3 ) ? 2 * pow_offset : 0 );
  mspc->pwr_max    = p_control_pcs[MAX_PCL_PCS1900];
  mspc->pwr_min    = p_control_pcs[MIN_PCL_PCS1900];
  mspc->gamma_0    = GAMMA_0_PCS1900;
  mspc->band_ind   = CGRLC_PCS_1900;
} /* grr_get_ms_pwr_cap_pcs1900 */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_power_class
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE grr_get_power_class ( UBYTE index )
{
  T_rf_cap *rf_cap      = rr_csf_get_rf_capability( );
  UBYTE     power_class;

  TRACE_FUNCTION( "grr_get_power_class" );

  if( rf_cap NEQ NULL )
  {
    power_class = rf_cap->rf_power.pow_class4[index].pow_class;
  }
  else
  {
    power_class = POWER_CLASS_1;

    TRACE_ERROR( "grr_get_power_class: RF capabilities invalid" );
  }

  return( power_class );

} /* grr_get_power_class */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_pcl
+------------------------------------------------------------------------------
| Description : This function is used to convert the nominal output power to 
|               the power control level
|
| Parameters  : p_ctrl - pointer to table of power control levels
|               nop    - nominal output power
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE grr_get_pcl ( UBYTE const *p_ctrl,
                           SHORT        nop )
{ 
  UBYTE pcl = 0; /* power control level */

  TRACE_FUNCTION( "grr_get_pcl" ); 

  /* get power control level */
  while( nop < (SHORT)p_ctrl[pcl] - 1 OR nop >= (SHORT)p_ctrl[pcl] + 1 ) pcl++;

  return( pcl );

} /* grr_get_pcl() */

/*
+------------------------------------------------------------------------------
| Function    : grr_validate_ma_num_in_freq_par
+------------------------------------------------------------------------------
| Description : Validates MA_NUM in indirect encoding.
|               This validation is required only when
|               freq parameters are received with indirect
|               encoding.
|               Change mark received with Frequency parameters is compared
|               with the stored change mark for PSI2/PSI13/SI13 to determine
|               if the mobile allocation stored in these messages could be used.
|
|               Frequency parameters received in an assignment message could refer
|               to GPRS mobile allocation received in a previous assignment message
|               by using MA_NUM = 15 in indirect encoding. Frequency parameters
|               received in non-assignment message( For example PSI 8 or PSI 14)
|               cannot refer to GPRS mobile received in a previous message.
|               This validation is done using the second parameter.
|
|               This function should be used before using the GPRS mobile allocation
|               referred in freq_par.
| Parameters  : freq_par - points to the frequency parameter structure
|               received in system information/some assignment message.
|
|               msg_type - identifies the type of message in which freq par
|                          was received. Two values are possible.
|                          GRR_FREQ_PARAM_RECEIVED_IN_ASSIGNMENT 0
|                          GRR_FREQ_PARAM_RECEIVED_IN_NON_ASSIGNMENT 1
| Return value : True - if validation sucessful.
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_validate_ma_num_in_freq_par(const T_freq_par *freq_par,UBYTE msg_type)
{
  BOOL result;

  TRACE_FUNCTION("grr_validate_ma_num_in_freq_par");

  if(MA_NUMBER_4_ASSIGNMENT EQ freq_par->indi_encod.ma_num)
    {
      if(GRR_FREQ_PARAM_RECEIVED_IN_ASSIGNMENT NEQ msg_type )
        {
          TRACE_ERROR("Frequency parameter in a non assignment message");
          TRACE_ERROR("cannot have ma_num==MA_NUMBER_4_ASSIGNMENT");
          result = FALSE;
        }
      else if( (MA_NUMBER_4_ASSIGNMENT EQ psc_db->gprs_ms_alloc_in_assignment.ma_num) AND
               (!freq_par->indi_encod.v_chamge_ma_sub)                )
      {
        result = TRUE;
      }
      else
      {
        TRACE_ERROR("mobile allocation in assignment wrong");
        result = FALSE;
      }
    }
    else if(MA_NUMBER_4_PSI13_OR_CELL_ALLOC EQ freq_par->indi_encod.ma_num)
    {
      if( ( (freq_par->indi_encod.v_chamge_ma_sub      ) AND
            (freq_par->indi_encod.chamge_ma_sub.cm1 EQ
             psc_db->psi13_params.si13_change_mark                 )
          ) OR
          ( (freq_par->indi_encod.chamge_ma_sub.v_cm2  ) AND
            (freq_par->indi_encod.chamge_ma_sub.cm2 EQ
             psc_db->psi13_params.si13_change_mark                 )
          ) OR
          (
            !freq_par->indi_encod.v_chamge_ma_sub     AND
            !freq_par->indi_encod.chamge_ma_sub.v_cm2
          ))
      {
        result = TRUE;
      }
      else
      {
        TRACE_ERROR("wrong si13 change mark value");
        result = FALSE;
      }
    }
    else
    {
      if( ( (freq_par->indi_encod.v_chamge_ma_sub      ) AND
            (freq_par->indi_encod.chamge_ma_sub.cm1 EQ
             psc_db->psi2_params.psi2_change_mark                  )
          ) OR
          ( (freq_par->indi_encod.chamge_ma_sub.v_cm2  ) AND
            (freq_par->indi_encod.chamge_ma_sub.cm2 EQ
             psc_db->psi2_params.psi2_change_mark                  )
          ) OR
          (
            !freq_par->indi_encod.v_chamge_ma_sub     AND
            !freq_par->indi_encod.chamge_ma_sub.v_cm2
          ))
      {
        result = TRUE;
      }
      else
      {
        TRACE_ERROR("wrong psi2 change mark value");
        TRACE_EVENT_P5("psi2cm:%d vcm1:%d cm1:%d vcm2:%d cm2:%d",
          psc_db->psi2_params.psi2_change_mark,
          freq_par->indi_encod.v_chamge_ma_sub,
          freq_par->indi_encod.chamge_ma_sub.cm1,
          freq_par->indi_encod.chamge_ma_sub.v_cm2,
          freq_par->indi_encod.chamge_ma_sub.cm2);
        result = FALSE;
      }
    }
    return result;
} /* grr_validate_ma_num_in_freq_par */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_gprs_ma
+------------------------------------------------------------------------------
| Description : ma_num identifies the GPRS mobile allocation received
|               in PSI 2, PSI 13 or the previous assignment message.
|               This function returns the matching pointer to the
|               GPRS Mobile allocation IE.
|
| Parameters  : ma_num - 0 to 13 - gprs mobile allocation received in PSI 2
|                             14 gprs mobile allocation received in Si13/PSI13
|                             15 GPRS mobile allocation received in previous
|                                assignment message.
| Return value : Pointer to appropriate GPRS MA stored in psc db.
+------------------------------------------------------------------------------
*/
GLOBAL T_gprs_ms_alloc_ie* grr_get_gprs_ma(UBYTE ma_num)
{
  USHORT n = 0;
  T_gprs_ms_alloc_ie *gp_ma = NULL;
  TRACE_FUNCTION( "grr_get_gprs_ma" );
  if(ma_num > MA_NUMBER_4_ASSIGNMENT)
  {
    TRACE_ERROR("ma_num > MA_NUMBER_4_ASSIGNMENT");
    return NULL;
  }

  /*
   * Get the GPRS Mobile Allocation IE corresponding to the ma_num
   */
  switch(ma_num)
  {
    /* MA_NUMBER used to reference a MA received in a previous assignent */
    case MA_NUMBER_4_ASSIGNMENT:
      if( psc_db->gprs_ms_alloc_in_assignment.ma_num NEQ NOT_SET )
      {
        gp_ma = &psc_db->gprs_ms_alloc_in_assignment.gprs_ms_alloc_ie;
      }
      break;

    /* MA_NUMBER used to reference a Mobile Allocation
     * (MA) received in PSI13, which may referenced from
     * an assignment message or which referes to the cell
     * allocation defined for the cell in SI1 or PSI2
     */
    case MA_NUMBER_4_PSI13_OR_CELL_ALLOC:

      /*lint -fallthrough*/

    /* GPRS Mobile Allocations received in PSI2 and/or PSI13 values 0...13 */
    default:
      for(n = 0; n < MAX_GPRS_MS_ALLOC; n++)
      {
        if( psc_db->gprs_ms_alloc_in_psi2_psi13[n].ma_num EQ ma_num)
        {
          gp_ma = &psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie;
          break;
        }
      }
      break;
  }
   return gp_ma;
} /* grr_get_gprs_ma */

/*
+------------------------------------------------------------------------------
| Function    : grr_cnv_freq_para_in_assignment_direct_enc_2
+------------------------------------------------------------------------------
| Description : This function decodes the frequency parametes received
|               in direct encoding2 form. It constructs the Mobile allocation
|               list as required by TI L1 MPHP primitives.
|
| Parameters  :
|
| NOTE: THIS FUNCTION USES THE LOCAL TEMPORARY ARRAYS - _local_rfl_contents AND
|      _local_dummy_list. These are use by grr_cnv_freq_para_in_psi8_direct_enc_2()
|       also. These two functions are placed in the same object module to avoid
|       the need for two copies of temporary arrays.
+------------------------------------------------------------------------------
*/

GLOBAL BOOL grr_cnv_freq_para_in_assignment_direct_enc_2
             (T_p_frequency_par *frequency_par,T_freq_par *freq_par)
{
   TRACE_FUNCTION("grr_cnv_freq_para_in_assignment_direct_enc_2");
   /* decode and store rfl list */
    /* clean RFL content  */
    memset (&_local_rfl_contents, 0, sizeof(T_f_range));
    /* Length in bits */
    _local_rfl_contents.l_f = (freq_par->di_encod2.len_ma_list+3)*8;
    /* Copy RFL content bits into the structure */
    memcpy(_local_rfl_contents.b_f, freq_par->di_encod2.ma_list,
      freq_par->di_encod2.len_ma_list+3);

    /*
     * Use a function from RR
     * This function copies the frequencies into list. This function takes the arranging
     * of ARFCN into account cf. 12.10a GPRS Mobile Allocation in 04.60
     */
    for_create_channel_list (&_local_rfl_contents, &_local_dummy_list);

    memcpy(psc_db->rfl[MAX_RFL].list, _local_dummy_list.b_f,
      sizeof(psc_db->rfl[MAX_RFL].list));

/*    grr_create_channel_list(freq_par->di_encod2.ma_list, psc_db->rfl[MAX_RFL].list);*/

    psc_db->gprs_ms_alloc_in_assignment.gprs_ms_alloc_ie.hsn =
                                                      freq_par->di_encod2.hsn;
    psc_db->gprs_ms_alloc_in_assignment.gprs_ms_alloc_ie.v_rfl_num_list = TRUE;
    psc_db->gprs_ms_alloc_in_assignment.gprs_ms_alloc_ie.c_rfl_num_list = 1;
    psc_db->gprs_ms_alloc_in_assignment.gprs_ms_alloc_ie.v_ma_struct = FALSE;
    psc_db->gprs_ms_alloc_in_assignment.gprs_ms_alloc_ie.v_arfcn_index_list = FALSE;

    /* set up gprs_ms_alloc_in_assignment struct in database */
    psc_db->gprs_ms_alloc_in_assignment.ma_num = MA_NUMBER_4_ASSIGNMENT;
    psc_db->gprs_ms_alloc_in_assignment.gprs_ms_alloc_ie.rfl_num_list[0].rfl_num =
    psc_db->rfl[MAX_RFL].num                   = RFL_NUMBER_4_DIRECT_ENCODING_2;

    return(grr_create_freq_list(  psc_db->gprs_ms_alloc_in_assignment.ma_num,
                                  freq_par->di_encod2.maio,
                                  &frequency_par->p_chan_sel,
                                  &frequency_par->p_freq_list ));
}/* grr_cnv_freq_para_in_assignment_direct_enc_2 */

#ifdef REL99

/*
+------------------------------------------------------------------------------
| Function    : grr_cnv_freq_para_in_psi8_direct_enc_2
+------------------------------------------------------------------------------
| Description : This function decodes the Frequency parameters in direct encoding 2
|               The output of this function is in form which could be used by
|               MPH_* primitives in RR. This The mobile allocation list
|               generated by this function is different from the ones used by
|               MPHP_* primitives.
|               CBCH parameters are passed to RR and RR passes to ALR and ALR
|               converts these frequencies to TI L1 form and passes in MPHC_*
|               primitive to L1.
|               The caller of this function should make sure that the type of
|               encoding used in freq_par is of direct encoding 2.
| Parameters  : cbch_req - output.
|               freq_par - input
| NOTE: THIS FUNCTION USES THE LOCAL TEMPORARY ARRAYS - _local_rfl_contents AND
|       _local_dummy_list. These are use by grr_cnv_freq_para_in_assignment_direct_enc_2()
|       also.These two functions are placed in the same object module to avoid
|       the need for two copies of temporary arrays.
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_cnv_freq_para_in_psi8_direct_enc_2(T_cbch *cbch_req,const T_freq_par *freq_par)
{
    USHORT count = 0;
    TRACE_FUNCTION( "grr_cnv_freq_para_in_psi8_direct_enc_2");
    /* decode and store rfl list */
    /* clean RFL content  */
    memset (&_local_rfl_contents, 0, sizeof(T_f_range));
    /* Length in bits */
    _local_rfl_contents.l_f = (freq_par->di_encod2.len_ma_list+3)*8;
    /* Copy RFL content bits into the structure */
    memcpy(_local_rfl_contents.b_f, freq_par->di_encod2.ma_list,
      freq_par->di_encod2.len_ma_list+3);

    /*
     * Use a function from RR
     * This function copies the frequencies into list. This function takes the arranging
     * of ARFCN into account cf. 12.10a GPRS Mobile Allocation in 04.60
     */
    for_create_channel_list (&_local_rfl_contents, &_local_dummy_list);

    /* Local dummy list represents the mobile allocation.
     * This list need not be filtered with grr_ma_filter_list
     * as ALL arfcns in this list are part of mobile allocation.
     * But we need to validate this list.
     */
    srv_create_list(&_local_dummy_list,cbch_req->ma, 64 , TRUE ,0);

    if(!grr_validate_and_count_frequencies(cbch_req->ma,&count))
    {
      TRACE_ERROR( "grr_cnv_freq_para_in_psi8_direct_enc_2:validation failed/freq count zero" );
      return FALSE;
    }
    cbch_req->ma[count] = NOT_PRESENT_16BIT;
    cbch_req->maio = freq_par->di_encod2.maio;
    cbch_req->hsn = freq_par->di_encod2.hsn ;
    return TRUE;
}/* grr_cnv_freq_para_in_psi8_direct_enc_2 */
#endif

/*
+------------------------------------------------------------------------------
| Function    : grr_validate_and_count_frequencies
+------------------------------------------------------------------------------
| Description : This function ensures that all the frequencies in the list
|               are of same band. It returns the cout of frequencies in the list.
|               NOT_PRESENT_16BIT represents the end of list.
| Parameters  : Array representing the frequency list
|               ptr_cnt - output, returns the count.
| Return value :TRUE/FALSE
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_validate_and_count_frequencies(USHORT radio_freq_list[MPHP_NUMC_MA],USHORT* ptr_cnt)
{
  USHORT n = 0;
  TRACE_FUNCTION("grr_validate_and_count_frequencies");
  switch( std )
    {
      case STD_900: /* frequencies must between 1 and 124 */

       for(n=0, *ptr_cnt=0; n < 64; n++)
       {
         if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
           break;

         if(( radio_freq_list[n] >= LOW_CHANNEL_900 ) AND
            ( radio_freq_list[n] <= HIGH_CHANNEL_900))
         {
            (*ptr_cnt)++;
         }
         else
         {
           TRACE_EVENT_P1( "STD_900:Frequency %d not in the band!", radio_freq_list[n]);
           return FALSE;
         }
       }

        break;

      case STD_EGSM:

       for(n=0, *ptr_cnt=0; n < 64; n++)
       {
         if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
           break;

         if((radio_freq_list[n] <= HIGH_CHANNEL_900) OR
           ((radio_freq_list[n] >= LOW_CHANNEL_EGSM) AND
            (radio_freq_list[n] <= HIGH_CHANNEL_EGSM - 1)))
         {
           (*ptr_cnt)++;
         }
         else
         {
           TRACE_EVENT_P1( "STD_EGSM: Frequency %d not in the band!", radio_freq_list[n]);
           return FALSE;
         }
       }
        break;

      case STD_1900:

       for(n=0, *ptr_cnt=0; n < 64; n++)
       {
         if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
           break;

         if(( radio_freq_list[n] >= LOW_CHANNEL_1900 ) AND
            ( radio_freq_list[n] <= HIGH_CHANNEL_1900))
         {
            (*ptr_cnt)++;
         }
         else
         {
           TRACE_EVENT_P1( "STD_1900: Frequency %d not in the band!", radio_freq_list[n]);
           return FALSE;
         }
       }
        break;

      case STD_1800:

       for(n=0, *ptr_cnt=0; n < 64; n++)
       {
         if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
           break;

         if(( radio_freq_list[n] >= LOW_CHANNEL_1800 ) AND
           (  radio_freq_list[n] <= HIGH_CHANNEL_1800))
         {
           (*ptr_cnt)++;
         }
         else
         {
           TRACE_EVENT_P1( "STD_1800: Frequency %d not in the band!", radio_freq_list[n]);
           return FALSE;
         }
       }
       break;

      case STD_850:
       for(n=0, *ptr_cnt=0; n < 64; n++)
       {
         if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
           break;

         if(( radio_freq_list[n] >= LOW_CHANNEL_850 ) AND
            ( radio_freq_list[n] <= HIGH_CHANNEL_850))
         {
           (*ptr_cnt)++;
         }
         else
         {
           TRACE_EVENT_P1( "STD_850: Frequency %d not in the band!", radio_freq_list[n]);
           return FALSE;
         }
       }
       break;

      case STD_DUAL:

        if (( radio_freq_list[0] >= LOW_CHANNEL_900 ) AND
           (radio_freq_list[0] <= HIGH_CHANNEL_900))
        {
          for(n=0, *(ptr_cnt)=0; n < 64; n++)
          {
            if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
            break;

            if(( radio_freq_list[n] >= LOW_CHANNEL_900 ) AND
            (radio_freq_list[n] <= HIGH_CHANNEL_900))
            {
              *(ptr_cnt)+=1;
            }
            else
            {
              TRACE_EVENT_P1( "STD_DUAL: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
            }
          }
        }
        else
        if (( radio_freq_list[0] >= LOW_CHANNEL_1800 ) AND
           (radio_freq_list[0] <= HIGH_CHANNEL_1800))
        {
          for(n=0, *(ptr_cnt)=0; n < 64; n++)
          {
            if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
            break;

            if(( radio_freq_list[n] >= LOW_CHANNEL_1800 ) AND
               (radio_freq_list[n] <= HIGH_CHANNEL_1800))
            {
              *(ptr_cnt)+=1;
            }
            else
            {
              TRACE_EVENT_P1( "STD_DUAL: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
            }
          }
        }
        else
        {
              TRACE_EVENT_P1( "STD_DUAL: Frequency %d not in the band!", radio_freq_list[0]);
              return FALSE;
        }
        break;

      case STD_DUAL_EGSM:

         if((radio_freq_list[0] <= HIGH_CHANNEL_900) OR
           ((radio_freq_list[0] >= LOW_CHANNEL_EGSM) AND
           (radio_freq_list[0] <= HIGH_CHANNEL_EGSM - 1)))
         {
           for(n=0, *(ptr_cnt)=0; n < 64; n++)
           {
             if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
             break;

             if((radio_freq_list[n] <= HIGH_CHANNEL_900) OR
             ((radio_freq_list[n] >= LOW_CHANNEL_EGSM) AND
             (radio_freq_list[n] <= HIGH_CHANNEL_EGSM - 1)))
             {
               *(ptr_cnt)+=1;
             }
             else
             {
               TRACE_EVENT_P1( "STD_DUAL_EGSM: Frequency %d not in the band!", radio_freq_list[n]);
               return FALSE;
             }
           }
         }
         else
         if (( radio_freq_list[0] >= LOW_CHANNEL_1800 ) AND
           (radio_freq_list[0] <= HIGH_CHANNEL_1800))
         {
           for(n=0, *(ptr_cnt)=0; n < 64; n++)
           {
             if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
             break;

             if(( radio_freq_list[n] >= LOW_CHANNEL_1800 ) AND
             (radio_freq_list[n] <= HIGH_CHANNEL_1800))
             {
               *(ptr_cnt)+=1;
             }
             else
             {
              TRACE_EVENT_P1( "STD_DUAL_EGSM: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
             }
           }
         }
         else
         {
             TRACE_EVENT_P1( "STD_DUAL_EGSM: Frequency %d not in the band!", radio_freq_list[0]);
             return FALSE;
         }
         break;

      case STD_DUAL_US:
        if (( radio_freq_list[0] >= LOW_CHANNEL_850 ) AND
           (radio_freq_list[0] <= HIGH_CHANNEL_850))
        {
          for(n=0, *(ptr_cnt)=0; n < 64; n++)
          {
            if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
            break;

            if(( radio_freq_list[n] >= LOW_CHANNEL_850 ) AND
            (radio_freq_list[n] <= HIGH_CHANNEL_850))
            {
              *(ptr_cnt)+=1;
            }
            else
            {
              TRACE_EVENT_P1( "STD_DUAL_US: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
            }
          }
        }
        else
        if (( radio_freq_list[0] >= LOW_CHANNEL_1900 ) AND
           (radio_freq_list[0] <= HIGH_CHANNEL_1900))
        {
          for(n=0, *(ptr_cnt)=0; n < 64; n++)
          {
            if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
            break;

            if(( radio_freq_list[n] >= LOW_CHANNEL_1900 ) AND
               ( radio_freq_list[n] <= HIGH_CHANNEL_1900))
            {
              (*ptr_cnt)++;
            }
            else
            {
              TRACE_EVENT_P1( "STD_DUAL_US: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
            }
          }
        }
        else
        {
              TRACE_EVENT_P1( "STD_DUAL_US: Frequency %d not in the band!", radio_freq_list[0]);
              return FALSE;
        }
        break;

#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
      case STD_850_1800:
        if (( radio_freq_list[0] >= LOW_CHANNEL_850 ) AND
            ( radio_freq_list[0] <= HIGH_CHANNEL_850))
        {
          for(n=0, *(ptr_cnt)=0; n < 64; n++)
          {
            if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
            break;

            if(( radio_freq_list[n] >= LOW_CHANNEL_850 ) AND
               ( radio_freq_list[n] <= HIGH_CHANNEL_850))
            {
              *(ptr_cnt)+=1;
            }
            else
            {
              TRACE_EVENT_P1( "STD_850_1800: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
            }
          }
        }
        else
        if (( radio_freq_list[0] >= LOW_CHANNEL_1800 ) AND
            ( radio_freq_list[0] <= HIGH_CHANNEL_1800))
        {
          for(n=0, *(ptr_cnt)=0; n < 64; n++)
          {
            if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
            break;

            if(( radio_freq_list[n] >= LOW_CHANNEL_1800 ) AND
               ( radio_freq_list[n] <= HIGH_CHANNEL_1800))
            {
              (*ptr_cnt)++;
            }
            else
            {
              TRACE_EVENT_P1( "STD_850_1800: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
            }
          }
        }
        else
        {
          TRACE_EVENT_P1( "STD_850_1800: Frequency %d not in the band!", radio_freq_list[0]);
          return FALSE;
        }
        break;

      case STD_900_1900:
         if((radio_freq_list[n] <= HIGH_CHANNEL_900) OR
           ((radio_freq_list[n] >= LOW_CHANNEL_EGSM) AND
            (radio_freq_list[n] <= HIGH_CHANNEL_EGSM - 1)))
        {
          for(n=0, *(ptr_cnt)=0; n < 64; n++)
          {
            if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
            break;

            if((radio_freq_list[n] <= HIGH_CHANNEL_900) OR
              ((radio_freq_list[n] >= LOW_CHANNEL_EGSM) AND
               (radio_freq_list[n] <= HIGH_CHANNEL_EGSM - 1)))
            {
              *(ptr_cnt)+=1;
            }
            else
            {
              TRACE_EVENT_P1( "STD_900_1900: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
            }
          }
        }
        else
        if (( radio_freq_list[0] >= LOW_CHANNEL_1900 ) AND
            ( radio_freq_list[0] <= HIGH_CHANNEL_1900))
        {
          for(n=0, *(ptr_cnt)=0; n < 64; n++)
          {
            if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
            break;

            if(( radio_freq_list[n] >= LOW_CHANNEL_1900 ) AND
               ( radio_freq_list[n] <= HIGH_CHANNEL_1900))
            {
              (*ptr_cnt)++;
            }
            else
            {
              TRACE_EVENT_P1( "STD_900_1900: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
            }
          }
        }
        else
        {
              TRACE_EVENT_P1( "STD_900_1900: Frequency %d not in the band!", radio_freq_list[0]);
              return FALSE;
        }
        break;
        
      case STD_850_900_1800:
        if (( radio_freq_list[0] >= LOW_CHANNEL_850 ) AND
            ( radio_freq_list[0] <= HIGH_CHANNEL_850))
        {
          for(n=0, *(ptr_cnt)=0; n < 64; n++)
          {
            if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
            break;

            if(( radio_freq_list[n] >= LOW_CHANNEL_850 ) AND
               ( radio_freq_list[n] <= HIGH_CHANNEL_850))
            {
              *(ptr_cnt)+=1;
            }
            else
            {
              TRACE_EVENT_P1( "STD_850_900_1800: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
            }
          }
        }
        else
         if((radio_freq_list[n] <= HIGH_CHANNEL_900) OR
           ((radio_freq_list[n] >= LOW_CHANNEL_EGSM) AND
            (radio_freq_list[n] <= HIGH_CHANNEL_EGSM - 1)))
        {
          for(n=0, *(ptr_cnt)=0; n < 64; n++)
          {
            if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
            break;

            if((radio_freq_list[n] <= HIGH_CHANNEL_900) OR
              ((radio_freq_list[n] >= LOW_CHANNEL_EGSM) AND
               (radio_freq_list[n] <= HIGH_CHANNEL_EGSM - 1)))
            {
              (*ptr_cnt)++;
            }
            else
            {
              TRACE_EVENT_P1( "STD_850_900_1800: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
            }
          }
        }
        else
        if (( radio_freq_list[0] >= LOW_CHANNEL_1800 ) AND
            ( radio_freq_list[0] <= HIGH_CHANNEL_1800))
        {
          for(n=0, *(ptr_cnt)=0; n < 64; n++)
          {
            if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
            break;

            if(( radio_freq_list[n] >= LOW_CHANNEL_1800 ) AND
               ( radio_freq_list[n] <= HIGH_CHANNEL_1800))
            {
              (*ptr_cnt)++;
            }
            else
            {
              TRACE_EVENT_P1( "STD_850_900_1800: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
            }
          }
        }
        else
        {
              TRACE_EVENT_P1( "STD_850_900_1800: Frequency %d not in the band!", radio_freq_list[0]);
              return FALSE;
        }
        break;

      case STD_850_900_1900:
        if (( radio_freq_list[0] >= LOW_CHANNEL_850 ) AND
            ( radio_freq_list[0] <= HIGH_CHANNEL_850))
        {
          for(n=0, *(ptr_cnt)=0; n < 64; n++)
          {
            if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
            break;

            if(( radio_freq_list[n] >= LOW_CHANNEL_850 ) AND
               ( radio_freq_list[n] <= HIGH_CHANNEL_850))
            {
              *(ptr_cnt)+=1;
            }
            else
            {
              TRACE_EVENT_P1( "STD_850_900_1900: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
            }
          }
        }
        else
         if((radio_freq_list[n] <= HIGH_CHANNEL_900) OR
           ((radio_freq_list[n] >= LOW_CHANNEL_EGSM) AND
            (radio_freq_list[n] <= HIGH_CHANNEL_EGSM - 1)))
        {
          for(n=0, *(ptr_cnt)=0; n < 64; n++)
          {
            if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
            break;

            if((radio_freq_list[n] <= HIGH_CHANNEL_900) OR
              ((radio_freq_list[n] >= LOW_CHANNEL_EGSM) AND
               (radio_freq_list[n] <= HIGH_CHANNEL_EGSM - 1)))
            {
              (*ptr_cnt)++;
            }
            else
            {
              TRACE_EVENT_P1( "STD_850_900_1900: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
            }
          }
        }
        else
        if (( radio_freq_list[0] >= LOW_CHANNEL_1900 ) AND
            ( radio_freq_list[0] <= HIGH_CHANNEL_1900))
        {
          for(n=0, *(ptr_cnt)=0; n < 64; n++)
          {
            if(radio_freq_list[n] EQ NOT_PRESENT_16BIT)
            break;

            if(( radio_freq_list[n] >= LOW_CHANNEL_1900 ) AND
               ( radio_freq_list[n] <= HIGH_CHANNEL_1900))
            {
              (*ptr_cnt)++;
            }
            else
            {
              TRACE_EVENT_P1( "STD_850_900_1900: Frequency %d not in the band!", radio_freq_list[n]);
              return FALSE;
            }
          }
        }
        else
        {
              TRACE_EVENT_P1( "STD_850_900_1900: Frequency %d not in the band!", radio_freq_list[0]);
              return FALSE;
        }
        break;
#endif

      default:
#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
        TRACE_ASSERT( std EQ STD_850       OR
                      std EQ STD_900       OR
                      std EQ STD_EGSM      OR
                      std EQ STD_1900      OR
                      std EQ STD_1800      OR
                      std EQ STD_DUAL      OR
                      std EQ STD_DUAL_EGSM OR
                      std EQ STD_DUAL_US   OR  
                      std EQ STD_850_1800  OR 
                      std EQ STD_900_1900  OR 
                      std EQ STD_850_900_1800 OR 
                      std EQ STD_850_900_1900 );
#else
        TRACE_ASSERT( std EQ STD_850       OR
                      std EQ STD_900       OR
                      std EQ STD_EGSM      OR
                      std EQ STD_1900      OR
                      std EQ STD_1800      OR
                      std EQ STD_DUAL      OR
                      std EQ STD_DUAL_EGSM OR
                      std EQ STD_DUAL_US      );
#endif
        break;
    }

  memset(&radio_freq_list[n], 0, (128-2*n)); /*lint !e669*/
  /*set values back to zero, neeeded by windows test cases*/

  *(ptr_cnt) = MINIMUM( MPHP_NUMC_MA, *ptr_cnt );

  if( *ptr_cnt EQ 0 )
  {
    TRACE_ERROR( "grr_validate_and_count_frequencies: freq count EQ 0" );

    return FALSE;
  }
  return TRUE;
}/* grr_validate_and_count_frequencies */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_ref_list_or_cell_allocation
+------------------------------------------------------------------------------
| Description : Determining the frequency list to be used for a hopping channel
|               involvs following steps.
|               1. Determining the referenced set of reference frequency lists
|                  or cell allocation.( See 3GPP 04.60 12.10a)
|               2. Filtering this list based on MA_BITMAP and ARFCN index list.
|
|               This function performs the first step. ie it generates
|               the referenced set of referncelists or Cell allocation list
|               depending on the presence of RFL number list in
|               the GPRS Mobile allocation IE.
| Parameters  : list1 - output, This is list will hold the referenced set of
|               reference freq list or Cell allocation depending on the
|               contents of gp_ma passed.
|               gp_ma - pointer to gprs mobile allocation IE
|               ma_num - identifies the MA_NUM in freq parameters IE.
|                        3GPP 04.60 Sec 5.5.1.7
|
| Return value : TRUE/FALSE
+------------------------------------------------------------------------------
*/
GLOBAL BOOL grr_get_ref_list_or_cell_allocation(T_LIST *list1,const T_gprs_ms_alloc_ie *gp_ma,UBYTE ma_num)
{
  BOOL                pbcch_is_present = grr_is_pbcch_present( );
  /*
   * Check RFL number list. If NOT exists, use cell allocation (CA)
   */
  TRACE_FUNCTION(" grr_get_ref_list_or_cell_allocation ");
  if(!gp_ma->v_rfl_num_list)
  {
    /*
     * Use CA defined in PSI2 or in SI1 depending on the table below
     *
     *  Case | PBCCH   | PSI2     | RFL_NUMBERs | Action
     *       | present | complete | all found   |
     *  -----+---------+----------+-------------+-----------
     *
     * 0 <= MA_NUMBER <= 15
     *
     *  -----+---------+----------+-------------+-----------
     *    A  |    y    |    y     |      y      | Use CA PSI
     *  -----+---------+----------+-------------+-----------
     *    B  |    y    |    y     |      n      | Use CA SI
     *  -----+---------+----------+-------------+-----------
     *    C  |    y    |    n     |      x      | Use CA SI
     *  -----+---------+----------+-------------+-----------
     *    D  |    n    |    x     |      x      | Use CA SI
     *  -----+---------+----------+-------------+-----------
     *
     * y = yes, n = no, x = don't case
     *
     */
    if( pbcch_is_present                 EQ TRUE       AND
        psc_db->state_of_PSI[PSI2].state EQ RECEIPT_OK AND
        psc_db->v_cell_alloc             EQ TRUE           )
    {
      /*
       * Use CA in PSI2. RFLs define a CA.
       * If needed, we have to merge frequencies defined in different RFL's into one list
       */

      /* Implementation of case A and B */

      if( grr_get_psi_cell_alloc_list( list1 ) EQ FALSE )
      {
        /* Implementation of case B */

        TRACE_ERROR( "grr_create_freq_list: grr_get_psi_cell_alloc_list( ) EQ FALSE in case B" );

        grr_get_si_cell_alloc_list( list1 );
      }
    }
    else
    {
      /* Implementation of case C and D */

      if( pbcch_is_present EQ TRUE )
      {
        TRACE_EVENT( "grr_create_freq_list: PSI2 not OK in case C and D" );
      }

      grr_get_si_cell_alloc_list( list1 );
    }
  }
  else
  {
    /*
     * Use MA depending on the table below
     *
     * Case | PBCCH   | PSI2     | RFL_NUMBERs | 0 <= RFL_NUMBER <= 15 | RFL_NUMBER == 16 | Action
     *      | present | complete | all found   |                       |                  |
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *
     * 0 <= MA_NUMBER <= 13
     *
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *   E  |    y    |    y     |      y      |           x           |         x        | Use list
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *   F  |    y    |    y     |      n      |           x           |         x        | Reject list
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *   G  |    y    |    n     |      x      |           x           |         x        | Reject list
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *   H  |    n    |    x     |      x      |           x           |         x        | Reject list
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *
     * MA_NUMBER == 14
     *
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *   I  |    y    |    y     |      y      |           x           |         x        | Use list
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *   J  |    y    |    y     |      n      |           x           |         x        | Use CA PSI
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *   K  |    y    |    n     |      x      |           x           |         x        | Use CA SI
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *   L  |    n    |    x     |      x      |           x           |         x        | Use CA SI
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *
     * MA_NUMBER == 15
     *
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *   M  |    y    |    y     |      y      |           x           |         x        | Use list
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *   N  |    y    |    y     |      n      |           x           |         x        | Reject list
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *   O  |    y    |    n     |      x      |           y           |         n        | Reject list
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *   P  |    y    |    n     |      x      |           n           |         y        | Use list
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *   Q  |    n    |    x     |      x      |           y           |         n        | Reject list
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *   R  |    n    |    x     |      x      |           n           |         y        | Use list
     * -----+---------+----------+-------------+-----------------------+------------------+------------
     *
     * y = yes, n = no, x = don't case
     *
     */

    /*
     * Use the defined RFL in GPRS MA
     * If needed, we have to merge frequencies defined in different RFL's into one list
     */
    if( ma_num EQ MA_NUMBER_4_ASSIGNMENT )
    {
      if( pbcch_is_present                 EQ TRUE       AND
          psc_db->state_of_PSI[PSI2].state EQ RECEIPT_OK     )
      {
        /* Implementation of case M and N */

        if( grr_get_ms_alloc_list( list1, gp_ma ) EQ FALSE )
        {
          /* Implementation of case N */

          TRACE_ERROR( "grr_create_freq_list: grr_get_ms_alloc_list( ) EQ FALSE in case N" );

          return FALSE;
        }
      }
      else
      {
        if( gp_ma->c_rfl_num_list          EQ 1                              AND
            gp_ma->rfl_num_list[0].rfl_num EQ RFL_NUMBER_4_DIRECT_ENCODING_2     )
        {
          /* Implementation of case P and R */

          if( grr_get_ms_alloc_list( list1, gp_ma ) EQ FALSE )
          {
            TRACE_ERROR( "grr_create_freq_list: grr_get_ms_alloc_list( ) EQ FALSE in case P and R" );

            return FALSE;
          }
        }
        else
        {
          /* Implementation of case O and Q */

          if( pbcch_is_present EQ TRUE )
          {
            TRACE_ERROR( "grr_create_freq_list: PSI2 not OK in case O and Q" );
          }
          else
          {
            TRACE_ERROR( "grr_create_freq_list: no PBCCH present and error in case O and Q" );
          }

          return FALSE;
        }
      }
    }
    else if( ma_num EQ MA_NUMBER_4_PSI13_OR_CELL_ALLOC )
    {
      if( pbcch_is_present                 EQ TRUE       AND
          psc_db->state_of_PSI[PSI2].state EQ RECEIPT_OK     )
      {
        /* Implementation of case I and J */

        if( grr_get_ms_alloc_list( list1, gp_ma ) EQ FALSE )
        {
          /* Implementation of case J */

          TRACE_ERROR( "grr_create_freq_list: grr_get_ms_alloc_list( ) EQ FALSE in case J" );

          if( grr_get_psi_cell_alloc_list( list1 ) EQ FALSE )
          {
            TRACE_ERROR( "grr_create_freq_list: grr_get_psi_cell_alloc_list( ) EQ FALSE in case J" );

            grr_get_si_cell_alloc_list( list1 );
          }
        }
      }
      else
      {
        /* Implementation of case K and L */

        if( pbcch_is_present EQ TRUE )
        {
          TRACE_ERROR( "grr_create_freq_list: PSI2 not OK in case K and L" );
        }

        grr_get_si_cell_alloc_list( list1 );
      }
    }
    else
    {
      if( pbcch_is_present                 EQ TRUE       AND
          psc_db->state_of_PSI[PSI2].state EQ RECEIPT_OK     )
      {
        /* Implementation of case E and F */

        if( grr_get_ms_alloc_list( list1, gp_ma ) EQ FALSE )
        {
          /* Implementation of case F */

          TRACE_ERROR( "grr_create_freq_list: grr_get_ms_alloc_list( ) EQ FALSE in case F" );

          return FALSE;
        }
      }
      else
      {
        /* Implementation of case G and H */

        if( pbcch_is_present EQ TRUE )
        {
          TRACE_ERROR( "grr_create_freq_list: PSI2 not OK in case G and H" );
        }

        return FALSE;
      }
    }
  }
  return TRUE;
}/* grr_get_ref_list_or_cell_allocation  */

#ifdef REL99
/*
+------------------------------------------------------------------------------
| Function    : grr_get_time_to_send_poll
+------------------------------------------------------------------------------
| Description : This function retrun the duration in milliseconds to transmit a 
|               poll.The basic for the calucation is based on RRBP obtained in 
|               the downlink message. The calculation is mentioned in the function.
| Parameters  : rrbp
|
+------------------------------------------------------------------------------
*/
GLOBAL T_TIME grr_get_time_to_send_poll(UBYTE rrbp)
{
   /*
   * This calculation is based on the following calculation
   * t_ms = (Number of frame to transmit Poll)*(Frame Duration in millseconds)
   * Number of frame to transmit Poll:- are 13,18,22,26 for rrbp 0,1,2,3 respectively
   * Frame Duration in millseconds:-is 4.615millseconds
   * eg . t_ms[0] = 13*4.615 =60,
   */
   T_TIME t_ms[] = {60, 83,102,120};
   return (t_ms[rrbp]);
}

#ifdef TI_PS_FF_EMR

/*
+------------------------------------------------------------------------------
| Function    : grr_init_enh_param
+------------------------------------------------------------------------------
| Description : This function is used to initialize ENH parameters
|
| Parameters  : param - pointer to ENH measurement parameter
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_init_enh_param( T_GRR_ENH_PARA *param, BOOL rep_prio_pres )
{ 
  TRACE_FUNCTION( "grr_init_enh_param" );
  
  param->idx        = NOT_SET;
  param->rept_type  = REPORT_TYPE_REP;
  if(rep_prio_pres)
  {
    param->gprs_rept_prio_desc.num_cells = 0;
  }
  
}/* grr_init_enh_param */

/*
+------------------------------------------------------------------------------
| Function    : grr_init_enh_cell_list
+------------------------------------------------------------------------------
| Description : This function is used to initialize ENH cell list
|
| Parameters  : Nil
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_init_enh_cell_list( void )
{ 
  TRACE_FUNCTION( "grr_init_enh_cell_list" );

  grr_data->db.cnt_enh_cell_list = 0;

  grr_data->db.sorted_enh_cell_list.num_valid = 0;

}/* grr_init_enh_cell_list */

/*
+------------------------------------------------------------------------------
| Function    : grr_prcs_enh_param_cw_temp
+------------------------------------------------------------------------------
| Description : This function is used to copy the enhanced parameters received 
|               in PSI5. Here it is copied into a temporary storage until all
|               the instances are properly received.
|
| Parameters  : db_enh  - ENH measurement parameter in the GRR database
|               air_enh - ENH measurement parameter of the air interface
|                         message
|               new_idx - sequence number of the message
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_prcs_enh_param_cw_temp ( T_GRR_ENH_PARA  *db_enh,
                                         T_enh_rep_param_struct *air_enh,
                                         UBYTE  new_idx )
{ 
  UBYTE n;
  
  TRACE_FUNCTION( "grr_prcs_enh_param_cw_temp" );

  if(db_enh->idx EQ NOT_SET)
  {
    /* Store default values for optional enhanced measurement paramaters */
    db_enh->ncc_permitted = NCC_PERMITTED_DEFAULT;
    db_enh->multiband_rep = GRR_MULTIBAND_REPORTING_0;
    db_enh->servingband_rep = SERVING_BAND_REPORTING_DEFAULT;
    db_enh->scale_order = SCALE_0dB;
    for(n = 0; n < MAX_NUM_BANDS; n++)
    {
      db_enh->enh_rep_data[n].rep_threshold = REP_THRESHOLD_DEF;
      db_enh->enh_rep_data[n].rep_offset = REP_OFFSET_0;
    }
  }    
  
  /* Update Enhanced Measurement parameters */
  db_enh->rept_type = air_enh->reporting_type;
  db_enh->rep_rate = air_enh->reporting_rate;
  db_enh->inv_bsic_enabled = air_enh->invalid_bsic_rep;
  
  if( (air_enh->v_ncc_permitted) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
  {
    db_enh->ncc_permitted = air_enh->ncc_permitted;
  }

  /* Update GPRS measurement parameters */
  if(air_enh->v_gprs_meas_par_report)
  {
    if( (air_enh->gprs_meas_par_report.v_multi_band_rep) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->multiband_rep = air_enh->gprs_meas_par_report.multi_band_rep;
    }

    if( (air_enh->gprs_meas_par_report.v_serv_cell_rep) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->servingband_rep = 
      air_enh->gprs_meas_par_report.serv_cell_rep;
    }

    if( (air_enh->gprs_meas_par_report.v_scale_ord) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->scale_order = 
      air_enh->gprs_meas_par_report.scale_ord;
    }

    if( (air_enh->gprs_meas_par_report.v_report_900_grr) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->enh_rep_data[0].rep_threshold = 
      air_enh->gprs_meas_par_report.report_900_grr.rep_thres_900;
      db_enh->enh_rep_data[0].rep_offset = 
      air_enh->gprs_meas_par_report.report_900_grr.rep_offset_900;
    }

    if( (air_enh->gprs_meas_par_report.v_report_1800_grr) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->enh_rep_data[1].rep_threshold = 
      air_enh->gprs_meas_par_report.report_1800_grr.rep_thres_1800;
      db_enh->enh_rep_data[1].rep_offset = 
      air_enh->gprs_meas_par_report.report_1800_grr.rep_offset_1800;
    }

    if( (air_enh->gprs_meas_par_report.v_report_400_grr) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->enh_rep_data[2].rep_threshold = 
      air_enh->gprs_meas_par_report.report_400_grr.rep_thres_400;
      db_enh->enh_rep_data[2].rep_offset = 
      air_enh->gprs_meas_par_report.report_400_grr.rep_offset_400;
    }

    if( (air_enh->gprs_meas_par_report.v_report_1900_grr) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->enh_rep_data[3].rep_threshold = 
      air_enh->gprs_meas_par_report.report_1900_grr.rep_thres_1900;
      db_enh->enh_rep_data[3].rep_offset = 
      air_enh->gprs_meas_par_report.report_1900_grr.rep_offset_1900;
    }

    if( (air_enh->gprs_meas_par_report.v_report_850_grr) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->enh_rep_data[4].rep_threshold = 
      air_enh->gprs_meas_par_report.report_850_grr.rep_thres_850;
      db_enh->enh_rep_data[4].rep_offset = 
      air_enh->gprs_meas_par_report.report_850_grr.rep_offset_850;
    }
  }    

  db_enh->idx = new_idx;
}/* grr_prcs_enh_param_cw_temp */

/*
+------------------------------------------------------------------------------
| Function    : grr_prcs_enh_param_pmo
+------------------------------------------------------------------------------
| Description : This function is used to copy temporarily the enhanced 
|               measurement parameters from PMO.
|
| Parameters  : db_enh  - ENH measurement parameter in the GRR database
|               air_enh - ENH measurement parameter of the air interface
|                         message
|               new_idx - sequence number of the message
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_prcs_enh_param_pmo ( T_GRR_ENH_PARA  *db_enh,
                                     T_enh_meas_param_pmo  *air_enh,  
                                     UBYTE  new_idx, UBYTE *pmo_ind )
{
  UBYTE n;
  UBYTE num_cells = db_enh->gprs_rept_prio_desc.num_cells;
  UBYTE number_cells = air_enh->gprs_rep_prio_cell_desc.number_cells;

  TRACE_FUNCTION( "grr_prcs_enh_param_pmo" );

  if(db_enh->idx EQ NOT_SET)
  {
    /* Store default values for optional enhanced measurement paramaters */
    db_enh->ncc_permitted = NCC_PERMITTED_DEFAULT;
    db_enh->multiband_rep = GRR_MULTIBAND_REPORTING_0;
    db_enh->servingband_rep = SERVING_BAND_REPORTING_DEFAULT;
    db_enh->scale_order = SCALE_0dB;
    for(n = 0; n < MAX_NUM_BANDS; n++)
    {
      db_enh->enh_rep_data[n].rep_threshold = REP_THRESHOLD_DEF;
      db_enh->enh_rep_data[n].rep_offset = REP_OFFSET_0;
    }
    for(n = 0; n < MAX_NR_OF_GSM_NC; n++)
    {
      db_enh->gprs_rept_prio_desc.rept_prio[n] = NORMAL_PRIO;
    }
  }    
  
  /* Update Enhanced Measurement parameters */
  
  *pmo_ind = air_enh->ba_psi3_str.pmo_ind_used;
  db_enh->rept_type = air_enh->reporting_type;
  db_enh->rep_rate = air_enh->reporting_rate;
  db_enh->inv_bsic_enabled = air_enh->invalid_bsic_rep;

  /* GPRS Report priority can be received in only one instance since start 
    index is not given for mapping to BA as in RTD and BSIC mapping in SI */
  if((air_enh->v_gprs_rep_prio_cell_desc) AND (number_cells NEQ 0))
  {
    if(number_cells > MAX_NR_OF_GSM_NC)
    {
      num_cells = MAX_NR_OF_GSM_NC;
    }
    else
    {
      num_cells = number_cells;
    }

    for(n = 0; n < num_cells; n++)
    {
      db_enh->gprs_rept_prio_desc.rept_prio[n] = 
        air_enh->gprs_rep_prio_cell_desc.rep_prio[n];
    }
  }

  /* Update GPRS measurement parameters */
  if(air_enh->v_gprs_meas_par_desc_meas)
  {
    if( (air_enh->gprs_meas_par_desc_meas.v_multi_band_rep) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->multiband_rep = 
        air_enh->gprs_meas_par_desc_meas.multi_band_rep;
    }

    if( (air_enh->gprs_meas_par_desc_meas.v_serv_cell_rep) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->servingband_rep = 
      air_enh->gprs_meas_par_desc_meas.serv_cell_rep;
    }

    db_enh->scale_order = air_enh->gprs_meas_par_desc_meas.scale_ord;

    if( (air_enh->gprs_meas_par_desc_meas.v_report_900_grr) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->enh_rep_data[0].rep_threshold = 
      air_enh->gprs_meas_par_desc_meas.report_900_grr.rep_thres_900;
      db_enh->enh_rep_data[0].rep_offset = 
      air_enh->gprs_meas_par_desc_meas.report_900_grr.rep_offset_900;
    }

    if( (air_enh->gprs_meas_par_desc_meas.v_report_1800_grr) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->enh_rep_data[1].rep_threshold = 
      air_enh->gprs_meas_par_desc_meas.report_1800_grr.rep_thres_1800;
      db_enh->enh_rep_data[1].rep_offset =
      air_enh->gprs_meas_par_desc_meas.report_1800_grr.rep_offset_1800;
    }

    if( (air_enh->gprs_meas_par_desc_meas.v_report_400_grr) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->enh_rep_data[2].rep_threshold = 
      air_enh->gprs_meas_par_desc_meas.report_400_grr.rep_thres_400;
      db_enh->enh_rep_data[2].rep_offset =
      air_enh->gprs_meas_par_desc_meas.report_400_grr.rep_offset_400;
    }

    if( (air_enh->gprs_meas_par_desc_meas.v_report_1900_grr) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->enh_rep_data[3].rep_threshold = 
      air_enh->gprs_meas_par_desc_meas.report_1900_grr.rep_thres_1900;
      db_enh->enh_rep_data[3].rep_offset =
      air_enh->gprs_meas_par_desc_meas.report_1900_grr.rep_offset_1900;
    }

    if( (air_enh->gprs_meas_par_desc_meas.v_report_850_grr) AND
      ((db_enh->idx EQ NOT_SET) OR (new_idx > db_enh->idx)) )
    {
      db_enh->enh_rep_data[4].rep_threshold = 
      air_enh->gprs_meas_par_desc_meas.report_850_grr.rep_thres_850;
      db_enh->enh_rep_data[4].rep_offset =
      air_enh->gprs_meas_par_desc_meas.report_850_grr.rep_offset_850;
    }
  }
  
  db_enh->idx = new_idx;
}/* grr_prcs_enh_param_pmo */

/*
+------------------------------------------------------------------------------
| Function    : grr_prcs_enh_param_pcco
+------------------------------------------------------------------------------
| Description : This function is used to copy the enhanced measurement 
|               parameters from PCCO.
|
| Parameters  : db      - Network directed cell database
|               air_enh - ENH measurement parameter of the air interface
|                         message
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_prcs_enh_param_pcco ( T_SC_DATABASE  *db,
                                      T_enh_meas_param_pcco *air_enh )
{
  UBYTE n;
  T_GRR_ENH_PARA *db_enh = &db->enh_ms;
  UBYTE max_num_ncells = db_enh->gprs_rept_prio_desc.num_cells;

  TRACE_FUNCTION( "grr_prcs_enh_param_pcco" );

  if(db_enh->idx EQ NOT_SET)
  {
    /* Store default values for optional enhanced measurement paramaters */
    db_enh->ncc_permitted = NCC_PERMITTED_DEFAULT;
    db_enh->multiband_rep = GRR_MULTIBAND_REPORTING_0;
    db_enh->servingband_rep = SERVING_BAND_REPORTING_DEFAULT;
    db_enh->scale_order = SCALE_0dB;
    for(n = 0; n < MAX_NUM_BANDS; n++)
    {
      db_enh->enh_rep_data[n].rep_threshold = REP_THRESHOLD_DEF;
      db_enh->enh_rep_data[n].rep_offset = REP_OFFSET_0;
    }
  }    
  
  /* Update Enhanced Measurement parameters */

  db->nc_ms.psi3_cm = air_enh->ba_psi3_str.psi3_cm;
  db->nc_ms.ba_ind  = air_enh->ba_psi3_str.ba_ind_used;  
  db->nc_ms.pmo_ind = air_enh->ba_psi3_str.pmo_ind_used;
  db_enh->rept_type = air_enh->reporting_type;
  db_enh->rep_rate = air_enh->reporting_rate;
  db_enh->inv_bsic_enabled = air_enh->invalid_bsic_rep;

  if((air_enh->v_gprs_rep_prio_cell_desc) AND 
  (air_enh->gprs_rep_prio_cell_desc.number_cells NEQ 0))
  {

    if(air_enh->gprs_rep_prio_cell_desc.number_cells > MAX_NR_OF_NCELL)
    {
      max_num_ncells = MAX_NR_OF_NCELL;
    }
    else
    {
      max_num_ncells = air_enh->gprs_rep_prio_cell_desc.number_cells;
    }
    
    for(n = 0; n < max_num_ncells; n++)
    {
      db_enh->gprs_rept_prio_desc.rept_prio[n] = 
      air_enh->gprs_rep_prio_cell_desc.rep_prio[n];
    }
    
  }
  /* Update GPRS measurement parameters */
  if(air_enh->v_gprs_meas_par_desc_meas)
  {
    if(air_enh->gprs_meas_par_desc_meas.v_multi_band_rep)
    {
      db_enh->multiband_rep = 
        air_enh->gprs_meas_par_desc_meas.multi_band_rep;
    }

    if(air_enh->gprs_meas_par_desc_meas.v_serv_cell_rep)
    {
      db_enh->servingband_rep = 
      air_enh->gprs_meas_par_desc_meas.serv_cell_rep;
    }

    db_enh->scale_order = air_enh->gprs_meas_par_desc_meas.scale_ord;

    if(air_enh->gprs_meas_par_desc_meas.v_report_900_grr)
    {
      db_enh->enh_rep_data[0].rep_threshold = 
      air_enh->gprs_meas_par_desc_meas.report_900_grr.rep_thres_900;
      db_enh->enh_rep_data[0].rep_offset = 
      air_enh->gprs_meas_par_desc_meas.report_900_grr.rep_offset_900;
    }

    if(air_enh->gprs_meas_par_desc_meas.v_report_1800_grr)
    {
      db_enh->enh_rep_data[1].rep_threshold = 
      air_enh->gprs_meas_par_desc_meas.report_1800_grr.rep_thres_1800;
      db_enh->enh_rep_data[1].rep_offset =
      air_enh->gprs_meas_par_desc_meas.report_1800_grr.rep_offset_1800;
    }

    if(air_enh->gprs_meas_par_desc_meas.v_report_400_grr)
    {
      db_enh->enh_rep_data[2].rep_threshold = 
      air_enh->gprs_meas_par_desc_meas.report_400_grr.rep_thres_400;
      db_enh->enh_rep_data[2].rep_offset =
      air_enh->gprs_meas_par_desc_meas.report_400_grr.rep_offset_400;
    }

    if(air_enh->gprs_meas_par_desc_meas.v_report_1900_grr)
    {
      db_enh->enh_rep_data[3].rep_threshold = 
      air_enh->gprs_meas_par_desc_meas.report_1900_grr.rep_thres_1900;
      db_enh->enh_rep_data[3].rep_offset =
      air_enh->gprs_meas_par_desc_meas.report_1900_grr.rep_offset_1900;
    }

    if(air_enh->gprs_meas_par_desc_meas.v_report_850_grr)
    {
      db_enh->enh_rep_data[4].rep_threshold = 
      air_enh->gprs_meas_par_desc_meas.report_850_grr.rep_thres_850;
      db_enh->enh_rep_data[4].rep_offset =
      air_enh->gprs_meas_par_desc_meas.report_850_grr.rep_offset_850;
    }
  }    

  db_enh->idx = 0;
}/* grr_prcs_enh_param_pcco */

/*
+------------------------------------------------------------------------------
| Function    : grr_copy_enh_and_nc_params_from_si2qtr
+------------------------------------------------------------------------------
| Description : This function is meant to copy the ENH and NC parameters that
|               are received by SI2 quater, when PBCCH is not present
|
| Parameters  : Pointer to RRGRR_SI2QUATER_IND message contents
|                
+------------------------------------------------------------------------------
*/
GLOBAL void grr_copy_enh_and_nc_params_from_si2qtr(T_RRGRR_SI2QUATER_IND 
                                                   *rrgrr_si2quater_ind)
{
  UBYTE n;
  T_enh_para_struct *enh_para_struct = 
    &(rrgrr_si2quater_ind->enh_para_struct);

  T_nc_para_struct *nc_para_struct = &(rrgrr_si2quater_ind->nc_para_struct);
  
  TRACE_FUNCTION( "grr_copy_enh_nc_params" );
  
  /* Copy Enhanced Measurement parameters only if report type indicates ENH */
  if(rrgrr_si2quater_ind->rep_type EQ ENHANCED_MEAS)
  {
    /* Copy GSM Neighbour Cell list, if present. Otherwise, only ENH params have 
       changed. Use old NC list and new ENH params */
    for(n = 0; n < enh_para_struct->num_valid_cells; n++)
    {
      /* Do not include the serving cell in the GSM Neighbour Cell list */
      if ( (enh_para_struct->enh_cell_list[n].arfcn EQ psc_db->pbcch.bcch.arfcn)
            AND 
           (enh_para_struct->enh_cell_list[n].bsic EQ psc_db->pbcch.bcch.bsic) )
      {
        continue;
      }
      psc_db->nc_ba_bcch_cw.info[n].index = n;
      psc_db->nc_ba_bcch_cw.info[n].arfcn = 
        enh_para_struct->enh_cell_list[n].arfcn;
      psc_db->nc_ba_bcch_cw.info[n].bsic = 
        enh_para_struct->enh_cell_list[n].bsic;
      psc_db->enh_cw.gprs_rept_prio_desc.rept_prio[n] =
        enh_para_struct->enh_cell_list[n].rep_priority;
    }

     psc_db->nc_ba_bcch_cw.number = n;
    
    /* Copy Enhanced Measurement parameters */
    psc_db->enh_cw.rept_type = rrgrr_si2quater_ind->rep_type;
    psc_db->enh_cw.rep_rate = enh_para_struct->rep_rate;
    psc_db->enh_cw.inv_bsic_enabled = enh_para_struct->inv_bsic_enabled;
    psc_db->enh_cw.ncc_permitted = enh_para_struct->ncc_permitted;
    psc_db->enh_cw.multiband_rep = enh_para_struct->multiband_rep;
    psc_db->enh_cw.servingband_rep = enh_para_struct->servingband_rep;
    psc_db->enh_cw.scale_order = enh_para_struct->scale_order;
    for(n = 0; n < MAX_NUM_BANDS; n++)
    {
      psc_db->enh_cw.enh_rep_data[n].rep_offset = 
        enh_para_struct->enh_rep_data[n].rep_offset;
      psc_db->enh_cw.enh_rep_data[n].rep_threshold = 
        enh_para_struct->enh_rep_data[n].rep_threshold;
    }
  }
  
  /* Copy NC Measurement parameters if received by SI2 quater */
  if(nc_para_struct->nco NEQ NC_EMPTY)
  {
    T_nc_meas_par nc_meas_par;
    
    nc_meas_par.ctrl_order = nc_para_struct->nco;
    nc_meas_par.v_nc_meas_per = nc_para_struct->is_valid;
    if(nc_meas_par.v_nc_meas_per)
    {
      nc_meas_par.nc_meas_per.non_drx_per = nc_para_struct->nc_non_drx;
      nc_meas_par.nc_meas_per.rep_per_i = nc_para_struct->nc_rep_per_i;
      nc_meas_par.nc_meas_per.rep_per_t = nc_para_struct->nc_rep_per_t;
    }
    
    grr_prcs_nc_param_struct ( &psc_db->nc_cw.param, &nc_meas_par, 0 );
  }

  cs_build_nc_ref_list( psc_db, FALSE );
  
  if( 
    psc_db->nc_cw.param.chng_mrk.prev        NEQ
    psc_db->nc_cw.param.chng_mrk.curr
    )
  {
    if( cs_is_meas_reporting( ) EQ FALSE )
    {
      cs_cancel_meas_report( );
    }
    
    cs_process_t3158( );
    
    psc_db->nc_cw.param.chng_mrk.prev = psc_db->nc_cw.param.chng_mrk.curr;
  }
} /* grr_copy_enh_and_nc_params_from_si2qtr */

/*
+------------------------------------------------------------------------------
| Function    : grr_get_psi3_cm
+------------------------------------------------------------------------------
| Description : This function returns the value of PSI3 change mark. The PSI3
|               change mark is received on PSI3, PSI3 bis and on optional 
|               PSI3 ter message
|
| Parameters  : Nil
|                
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE grr_get_psi3_cm(void)
{
  UBYTE psi3_cm = NOT_SET;

  TRACE_FUNCTION("grr_get_psi3_cm");

  if(psc_db->psi3_params.psi3_change_mark EQ
     psc_db->psi3bis_params.psi3bis_change_mark) 
  {
    psi3_cm = psc_db->psi3_params.psi3_change_mark;
    
    if(psc_db->psi3ter_params.psi3ter_change_mark NEQ NOT_SET AND
      psc_db->psi3ter_params.psi3ter_change_mark NEQ psi3_cm)
    {
      psi3_cm = NOT_SET;
    }
  }
  return(psi3_cm);
} /* grr_get_psi3_cm */


/*
+------------------------------------------------------------------------------
| Function    : grr_init_ba_bcch_nc_list
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : list  - pointer to NC measurement list
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_init_ba_bcch_nc_list(T_SC_DATABASE *db)
{ 
  TRACE_FUNCTION( "grr_init_ba_bcch_nc_list" );
  
  db->nc_ba_bcch_cw.number = 0;
  db->ba_ind = 0; /* BA-IND of BA-BCCH */
}/* grr_init_ba_bcch_nc_list */

#endif 

#endif

