/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   : cci_fbsf.c 
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
|  Purpose :  This module implements local functions for service FBS of
|             entity CCI.
+----------------------------------------------------------------------------- 
*/ 


#define CCI_FBSF_C

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/
#include <string.h>

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "cci.h"        /* to get the global entity definitions */
#include "llc.h"        /* to get the global entity definitions */

#include "cci_fbsf.h"
#include "llc_txp.h"
#include "llc_rxp.h"

#ifndef TI_PS_OP_CIPH_DRIVER

#ifdef _GEA_SIMULATION_
#include "cci_hw_sim.h"
#endif

#if !defined(_GEA_SIMULATION_) && !defined(LL_2to1)
#include "config/chipset.cfg"
#if (CHIPSET == 12) || (CHIPSET == 14)
#include "cci_gea_start.h"
#endif /* CHIPSET */
#endif /* ! _GEA_SIMULATION_ && !LL_2to1 */

/* local buffer for the copy of ciphering key */
static USHORT tmp_key[4];



/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/
#ifdef _GEA_SIMULATION_
USHORT* hw_reg = NULL;
#endif

/*==== LOCAL FUNCTION ===========================================================*/

LOCAL void ciph_get_result (T_CIPH_out_data *out_data, U8 *status);
LOCAL void ciph_init (void);
LOCAL void ciph_reset_hw (void);
LOCAL UBYTE ciph_fcs_check (UBYTE *fcs_start);
LOCAL void ciph_get_ciphered_data (T_CIPH_out_data *out_data);
LOCAL void ciph_get_deciphered_data (T_CIPH_out_data *out_data, U8 *status);
LOCAL void ciph_fill_ul_reg (T_CIPH_cipher_req_parms *cipher_req_parms,
                              T_CIPH_in_data_list *in_data_list);
LOCAL void ciph_fill_dl_reg (T_CIPH_cipher_req_parms *cipher_req_parms,
                              T_CIPH_in_data_list *in_data_list);

/*==== PRIVATE FUNCTIONS ====================================================*/

#ifdef LLC_TRACE_GEA_PARAM
LOCAL void cci_trace_gea_param( void );
#endif

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : ciph_cipher_req_sim
+------------------------------------------------------------------------------
| Description : Handles the primitive ciph_cipher_req. It is used to request 
|               ciphering or deciphering of a LLC PDU. 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ciph_cipher_req_sim (T_CIPH_cipher_req_parms *cipher_req_parms_ptr,
                             T_CIPH_in_data_list *in_data_ptr,
                             T_CIPH_out_data *out_data_ptr,
                             U8 *status) {
  
  USHORT volatile *cntl = cci_data->fbs.cntl_reg;
  int i = 0;
  ULONG cnt = 0;


  TRACE_FUNCTION( "ciph_cipher_req" );

  /* reset GEA (enables and then disables clock) */
  ciph_reset_hw();
  
  *cntl |= CL_ENABLE;

  /* fill UL or DL configuration and data registers */

  if (cci_data->fbs.ciph_params.direction == CIPH_UPLINK_DIR) {
    ciph_fill_ul_reg (cipher_req_parms_ptr, in_data_ptr );
#ifdef LL_2to1
    *cntl = (*cntl & UL_DL_UP) | START;
#else /* LL_2to1 */
#if (CHIPSET == 12) || (CHIPSET == 14)
    /* GEA work-around */
    *cntl = (*cntl & UL_DL_UP);
    gea_start();
#else
    *cntl = (*cntl & UL_DL_UP) | START;
#endif
#endif /* LL_2to1 */
  } else {
    ciph_fill_dl_reg (cipher_req_parms_ptr, in_data_ptr );
#ifdef LL_2to1
    *cntl |= (UL_DL_DOWN | START);
#else /* LL_2to1 */
#if (CHIPSET == 12) || (CHIPSET == 14)
    /* GEA work-around */
    *cntl |= (UL_DL_DOWN);
    gea_start();
#else
    *cntl |= (UL_DL_DOWN | START);
#endif
#endif /* LL_2to1 */
  }

  out_data_ptr->len = 0;
  for (i = 0; i < in_data_ptr->c_in_data; i++) {
    out_data_ptr->len += in_data_ptr->ptr_in_data[i].len;
  }

  if (cci_data->fbs.ciph_params.direction == CIPH_DOWNLINK_DIR) {
    out_data_ptr->len -= FCS_SIZE;
  } else {
    out_data_ptr->len += FCS_SIZE;
  }
  

#ifdef _GEA_SIMULATION_
  if (cci_data->fbs.ciph_params.direction == CIPH_UPLINK_DIR) {
    ciph_hw_sim_cipher(out_data_ptr->len);
  } else {
    ciph_hw_sim_decipher();
  }
#endif

  /* poll until data is processed */
  if(cci_data->fbs.cci_info_trace){
    while (*(cci_data->fbs.status_reg) & CHECK_WORKING){
      cnt += 1;
    };
    TRACE_EVENT_P1("INFO CCI: ciphering completed, counter: %d", cnt);
  } else {
    while (*(cci_data->fbs.status_reg) & CHECK_WORKING){ };
  }

  /* verify ciphering key, whether it has been corrupted */
  if(cci_data->fbs.cci_info_trace){
    if(*cci_data->fbs.kc_reg1 != tmp_key[0] ||
       *cci_data->fbs.kc_reg2 != tmp_key[1] ||
       *cci_data->fbs.kc_reg3 != tmp_key[2] ||
       *cci_data->fbs.kc_reg4 != tmp_key[3])
      {
        TRACE_ERROR("CCI ERROR: Ciphering key has been corrupted!!");
        TRACE_EVENT_P4("CCI: original key: %04x %04x %04x %04x %04x %04x",
        tmp_key[0], tmp_key[1], tmp_key[2], tmp_key[3]);
        TRACE_EVENT_P4("CCI: kc key: %04x %04x %04x %04x %04x %04x",
        *cci_data->fbs.kc_reg1, *cci_data->fbs.kc_reg2,
        *cci_data->fbs.kc_reg3, *cci_data->fbs.kc_reg4);
      }
  }

  /* and finally handle the result */
  ciph_get_result(out_data_ptr, status);

  /* disable clock until new frames are to process */
  *cntl &= CL_DISABLE;

}


/*
+------------------------------------------------------------------------------
| Function    : ciph_init_cipher_req_sim
+------------------------------------------------------------------------------
| Description : Handles the primitive ciph_init_cipher_req_sim.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ciph_init_cipher_req_sim (T_CIPH_init_cipher_req_parms *init_cipher_req_parms_ptr, 
                                 void (*read_data) (void))
{
  if (!cci_data->fbs.initialized){
    ciph_init();
    cci_data->fbs.initialized = TRUE;
  }
  cci_data->fbs.ciph_params = *init_cipher_req_parms_ptr;
}

/*
+------------------------------------------------------------------------------
| Function    : ciph_get_result
+------------------------------------------------------------------------------
| Description : Handles the function ciph_get_result. This function gets the 
|               ciphered data in uplink direction or deciphered data in 
|               downlink direction 
|
| Parameters  : 
+------------------------------------------------------------------------------
*/
LOCAL void ciph_get_result (T_CIPH_out_data *out_data, U8 *status)
{ 
  TRACE_FUNCTION( "ciph_get_result" );
  
  switch(cci_data->fbs.ciph_params.direction)
  {
    case CIPH_UPLINK_DIR:
      if( *(cci_data->fbs.status_reg) & CHECK_WORKING )
      {
        break;
      }
      else
      {
        ciph_get_ciphered_data (out_data);
      }
      break;

    case CIPH_DOWNLINK_DIR:
      if( *(cci_data->fbs.status_reg) & CHECK_WORKING )
      {
        break;
      }
      else
      {
        ciph_get_deciphered_data (out_data, status);
      }
      break;
    
    default:
      TRACE_ERROR( "TIMER unexpected" );
      break;
  }

} /* ciph_get_result() */


/*
+------------------------------------------------------------------------------
| Function    : ciph_init
+------------------------------------------------------------------------------
| Description : The function ciph_init() initializes the registers.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void ciph_init ( void )
{ 
  USHORT  *reg;

  TRACE_FUNCTION( "ciph_init" );


#ifdef _GEA_SIMULATION_

  TRACE_EVENT ("GEA simulation is used");

  /* 
   * initialization of registers - do it only once to handle restart of CCI 
   * 'hw_req' points to a set of 16 bit registers
   * 'fbs.simulated_reg_buffer' points to a 1596 bytes buffer for data
   */
  if (hw_reg == NULL)
  {
    MALLOC(hw_reg, sizeof(USHORT) * 26+1 );
  }
  memset(hw_reg, 0, sizeof(USHORT) * 26+1);

  if (cci_data->fbs.simulated_reg_buffer == NULL)
  {
    MALLOC(cci_data->fbs.simulated_reg_buffer, 1596);
  }
  memset(cci_data->fbs.simulated_reg_buffer, 0, 1596);
 
  reg = &hw_reg[0];

#else

  TRACE_EVENT ("GEA hardware is used");

  reg = (USHORT *)START_ADRESS;

#endif

#if (BOARD == 61 OR BOARD == 71)	/* G-Sample or I-Sample*/
  
  /* 01 */ cci_data->fbs.cntl_reg      = reg++;       /* 00 */
  /* 02 */ cci_data->fbs.status_reg    = reg++;       /* 02 */
  /* 03 */ cci_data->fbs.status_irq_reg= reg++;       /* 04 */

  /* 04 */ cci_data->fbs.conf_ul_reg1  = reg++;       /* 06 */
  /* 05 */ cci_data->fbs.conf_ul_reg2  = reg++;       /* 08 */
  /* 06 */ cci_data->fbs.conf_ul_reg3  = reg++;       /* 0A */
  /* 07 */ cci_data->fbs.conf_ul_reg4  = reg++;       /* 0C */
  /* 08 */ cci_data->fbs.conf_ul_reg5  = reg++;       /* 0E */

  /* 09 */ cci_data->fbs.conf_dl_reg1  = reg++;       /* 10 */
  /* 10 */ cci_data->fbs.conf_dl_reg2  = reg++;       /* 12 */
  /* 11 */ cci_data->fbs.conf_dl_reg3  = reg++;       /* 14 */
  /* 12 */ cci_data->fbs.conf_dl_reg4  = reg++;       /* 16 */
  /* 13 */ cci_data->fbs.conf_dl_reg5  = reg++;       /* 18 */

  /* 14 */ cci_data->fbs.kc_reg1       = reg++;       /* 1A */
  /* 15 */ cci_data->fbs.kc_reg2       = reg++;       /* 1C */
  /* 16 */ cci_data->fbs.kc_reg3       = reg++;       /* 1E */
  /* 17 */ cci_data->fbs.kc_reg4       = reg++;       /* 20 */

  /* 22 */                               reg++;       /* 22 */
  /* 22 */                               reg++;       /* 24 */
  /* 22 */                               reg++;       /* 26 */
  /* 22 */                               reg++;       /* 28 */

  /* 18 */ cci_data->fbs.fcs_ul_reg1   = reg++;       /* 2A */
  /* 19 */ cci_data->fbs.fcs_ul_reg2   = reg++;       /* 2C */

  /* 20 */ cci_data->fbs.fcs_dl_reg1   = reg++;       /* 2E */
  /* 21 */ cci_data->fbs.fcs_dl_reg2   = reg++;       /* 30 */
                                            
  /* 22 */ cci_data->fbs.switch_reg    = reg++;       /* 32 */
                                            
  /* 25 */ cci_data->fbs.data16_reg    = reg++;       /* 34 */
  /* 26 */ cci_data->fbs.data8_reg     = (UBYTE*)reg; /* 36 */

  *cci_data->fbs.switch_reg = 0;

#else

  /* 01 */ cci_data->fbs.cntl_reg      = reg++;       /* 00 */
  /* 02 */ cci_data->fbs.status_reg    = reg++;       /* 02 */
  /* 03 */ cci_data->fbs.status_irq_reg= reg++;       /* 04 */

  /* 04 */ cci_data->fbs.conf_ul_reg1  = reg++;       /* 06 */
  /* 05 */ cci_data->fbs.conf_ul_reg2  = reg++;       /* 08 */
  /* 06 */ cci_data->fbs.conf_ul_reg3  = reg++;       /* 0A */
  /* 07 */ cci_data->fbs.conf_ul_reg4  = reg++;       /* 0C */
  /* 08 */ cci_data->fbs.conf_ul_reg5  = reg++;       /* 0E */

  /* 09 */ cci_data->fbs.conf_dl_reg1  = reg++;       /* 10 */
  /* 10 */ cci_data->fbs.conf_dl_reg2  = reg++;       /* 12 */
  /* 11 */ cci_data->fbs.conf_dl_reg3  = reg++;       /* 14 */
  /* 12 */ cci_data->fbs.conf_dl_reg4  = reg++;       /* 16 */
  /* 13 */ cci_data->fbs.conf_dl_reg5  = reg++;       /* 18 */

  /* 14 */ cci_data->fbs.kc_reg1       = reg++;       /* 1A */
  /* 15 */ cci_data->fbs.kc_reg2       = reg++;       /* 1C */
  /* 16 */ cci_data->fbs.kc_reg3       = reg++;       /* 1E */
  /* 17 */ cci_data->fbs.kc_reg4       = reg++;       /* 20 */

  /* 18 */ cci_data->fbs.fcs_ul_reg1   = reg++;       /* 22 */
  /* 19 */ cci_data->fbs.fcs_ul_reg2   = reg++;       /* 24 */

  /* 20 */ cci_data->fbs.fcs_dl_reg1   = reg++;       /* 26 */
  /* 21 */ cci_data->fbs.fcs_dl_reg2   = reg++;       /* 28 */
                                            
  /* 22 */                               reg++;       /* 2A */
  /* 23 */                               reg++;       /* 2C */
  /* 24 */                               reg++;       /* 2E */
                                            
  /* 25 */ cci_data->fbs.data16_reg    = reg++;       /* 30 */
  /* 26 */ cci_data->fbs.data8_reg     = (UBYTE*)reg; /* 32 */
#endif /* Board 61 or 71*/

#ifdef _GEA_SIMULATION_
  cci_data->fbs.simulated_reg = cci_data->fbs.simulated_reg_buffer;
#endif

#ifndef _GEA_SIMULATION_
  TRACE_EVENT ("Received FCS will be verified");
#endif

  /* finaly reset the HW */
  ciph_reset_hw();

  /* initialize flag for CCI info trace */
  cci_data->fbs.cci_info_trace = FALSE;
  /* initialize freed partition counter  */
  cci_data->fbs.cci_freed_partition = 0;

} /* ciph_init() */


/*
+------------------------------------------------------------------------------
| Function    : ciph_reset_hw
+------------------------------------------------------------------------------
| Description : Resets the hardware 
|
+------------------------------------------------------------------------------
*/
LOCAL void ciph_reset_hw ( void )
{ 
  USHORT volatile *cntl = cci_data->fbs.cntl_reg;

  TRACE_FUNCTION( "ciph_reset_hw" );

  /* making the clock enable */
  *cntl |= CL_ENABLE;

  /* start reset in both directions and disable interrupt */
  *cntl &= ~(RESET_UL | RESET_DL | IT_ENABLE);

#ifdef _GEA_SIMULATION_

  *cci_data->fbs.status_reg &= NOT_WORKING;

  /* simulate reset complete */
  *cntl |= RESET_UL;
  *cntl |= RESET_DL;

#endif

  /* wait until reset is ready (both bits back high) */
  while ((*cntl & (RESET_UL|RESET_DL)) != (RESET_UL|RESET_DL))
    ;

  /* disable clock until new frames are to process */
  *cntl &= CL_DISABLE;

} /* ciph_reset_hw() */

/*
+------------------------------------------------------------------------------
| Function    : ciph_fcs_check
+------------------------------------------------------------------------------
| Description : The function compares the 3 bytes FCS for downlink direction
|
| Parameters  : *fcs_start - pointer to beginning of fcs field
|
| Returns     : CCI_FCS_PASSED - if equal
|               CCI_FCS_FAILED - if NOT equal
+------------------------------------------------------------------------------
*/
LOCAL UBYTE ciph_fcs_check (UBYTE* fcs_start)
{ 
  TRACE_FUNCTION( "ciph_fcs_check" );

#ifdef _CHECK_RECEIVED_FCS_
#ifdef _GEA_SIMULATION_
  /*
   * Compare FCS, taking byte ordering of FCS registers into account (Intel
   * format: high-/lowbyte swaped)
   */
  if (* fcs_start    == (UBYTE) (*cci_data->fbs.fcs_dl_reg1 & 0x00FF)       &&
      *(fcs_start+1) == (UBYTE)((*cci_data->fbs.fcs_dl_reg1 & 0xFF00) >> 8) &&
      *(fcs_start+2) == (UBYTE) (*cci_data->fbs.fcs_dl_reg2 & 0x00FF)        )
  {
    /* clear bit to indicate FCS is ok */
    *cci_data->fbs.status_reg &= ~(FCS_FALSE_BIT);
  }
  else
  {
    /* set bit to indicate FCS is false */
    *cci_data->fbs.status_reg |= FCS_FALSE_BIT;
  }

  /*
   * Check, if FCS false bit is set
   */
  if (*cci_data->fbs.status_reg & FCS_FALSE_BIT)
  {
    TRACE_EVENT("Info: Received FCS is wrong");
    return CIPH_FCS_ERROR;
  }
#endif /* _GEA_SIMULATION_ */
#endif /* _CHECK_RECEIVED_FCS_ */


#ifndef _GEA_SIMULATION_
  /*
   * Check, if FCS false bit is set
   */
  if (*cci_data->fbs.status_reg & FCS_FALSE_BIT)
  {
    TRACE_0_INFO("Received FCS is wrong");
    return CIPH_FCS_ERROR;
  }
#endif /* _GEA_SIMULATION_ */

  return CIPH_CIPH_PASS;

} /* ciph_fcs_check() */


/*
+------------------------------------------------------------------------------
| Function    : ciph_get_ciphered_data
+------------------------------------------------------------------------------
| Description : The function ciph_get_ciphered_data() copies the result of the ciphered 
|               data to destination buffer.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
LOCAL void ciph_get_ciphered_data (T_CIPH_out_data *out_data)
{  
  int    frame16, 
         frame8, 
         i;
  USHORT *sdu_data16;
  UBYTE  *sdu_data8;

  TRACE_FUNCTION( "ciph_get_ciphered_data" );

  i       = out_data->len - FCS_SIZE;

  frame16 = i / 2; 
  frame8  = i % 2; 

#ifdef _GEA_SIMULATION_
  cci_data->fbs.simulated_reg = cci_data->fbs.simulated_reg_buffer;
#endif /* _GEA_SIMULATION_ */

  sdu_data16 = (USHORT*)out_data->buf;

#if (BOARD == 61 OR BOARD == 71)	/* G-Sample or I-Sample */
  *cci_data->fbs.switch_reg = 1;

#ifdef LLC_TRACE_GEA_PARAM
  cci_trace_gea_param();
#endif

#endif

  for (i=0; i<frame16; i++)
  {

#ifdef _GEA_SIMULATION_
    ciph_reg16_read_sim();
#endif

    *sdu_data16 = *cci_data->fbs.data16_reg;

    sdu_data16++;
  }
   
  sdu_data8 = (UBYTE*)sdu_data16;

  if (frame8 != 0)
  {

#ifdef _GEA_SIMULATION_
    ciph_reg8_read_sim();
#endif

   *sdu_data8 = *cci_data->fbs.data8_reg;

    sdu_data8++;
  }

#if (BOARD == 61 OR BOARD == 71)	/* G-Sample or I-Sample */
  *cci_data->fbs.switch_reg = 0;
#endif

  /*
   * Copy FCS to destination sdu, taking byte ordering of FCS registers
   * into account, e.g.:
   *   reg1   reg2       sdu
   *   29 FC  xx 36  ->  FC 29 36
   */
  *sdu_data8 = (UBYTE) (*cci_data->fbs.fcs_ul_reg1 & 0x00FF);
  sdu_data8++;
  *sdu_data8 = (UBYTE)((*cci_data->fbs.fcs_ul_reg1 & 0xFF00) >> 8);
  sdu_data8++;
  *sdu_data8 = (UBYTE) (*cci_data->fbs.fcs_ul_reg2 & 0x00FF);

} /* ciph_get_ciphered_data() */

/*
+------------------------------------------------------------------------------
| Function    : ciph_get_deciphered_data
+------------------------------------------------------------------------------
| Description : The function ciph_get_deciphered_data() .... 
|
| Parameters  : T_CCI_DECIPHER_CNF *decipher_cnf
|
+------------------------------------------------------------------------------
*/
LOCAL void ciph_get_deciphered_data (T_CIPH_out_data *out_data, U8 *status)
{  
  int    i,
         frame8,
         frame16;
  USHORT *sdu_data16;
  UBYTE  *sdu_data8;


  TRACE_FUNCTION( "ciph_get_deciphered_data" );

  i       = out_data->len;

  frame16 = i / 2;
  frame8  = i % 2;

#ifdef _GEA_SIMULATION_
  cci_data->fbs.simulated_reg = cci_data->fbs.simulated_reg_buffer;
#endif

  sdu_data16 = (USHORT*)out_data->buf;

#if (BOARD == 61 OR BOARD == 71)	/* G-Sample or I-Sample */
  *cci_data->fbs.switch_reg = 1;
#endif

  for (i=0; i<frame16; i++)
  {

#ifdef _GEA_SIMULATION_
    ciph_reg16_read_sim();
#endif

    *sdu_data16 = *cci_data->fbs.data16_reg;

    sdu_data16++;
  }
  
  sdu_data8 = (UBYTE*)sdu_data16;

  if (frame8 != 0)
  {

#ifdef _GEA_SIMULATION_
    ciph_reg8_read_sim();
#endif

    *sdu_data8 = *cci_data->fbs.data8_reg;

    sdu_data8++;
  }

#if (BOARD == 61 OR BOARD == 71)	/* G-Sample or I-Sample */
  *cci_data->fbs.switch_reg = 0;
#endif

  /*
   * Get result from FCS calculation
   */
  *status = ciph_fcs_check(sdu_data8);

} /* ciph_get_deciphered_data() */



/*
+------------------------------------------------------------------------------
| Function    : ciph_fill_ul_reg
+------------------------------------------------------------------------------
| Description : The function ciph_fill_ul_reg() fills the uplink registers. 
|
| Parameters  : 
+------------------------------------------------------------------------------
*/
LOCAL void ciph_fill_ul_reg ( T_CIPH_cipher_req_parms *cipher_req_parms,
                              T_CIPH_in_data_list *in_data_list )
{ 
  int    i, j; 
  int    frame8;
  UBYTE  *desc_data8;
  U16 len = 0;

  TRACE_FUNCTION( "ciph_fill_ul_reg" );

  /* 
   * setting the protected mode variable 
   */
  if (cipher_req_parms->gprs_parameters.pm EQ CIPH_PM_PROTECTED)    
  {
    *cci_data->fbs.conf_ul_reg1 |= PROTECTED;
  }
  else
  {   
    *cci_data->fbs.conf_ul_reg1 &= NON_PROTECTED;
  }
    
  /* 
   * FCS is always calculated
   */
  *cci_data->fbs.conf_ul_reg1 |= FCS_COMPUTED; 

  /*
   * direction is uplink 
   */
  /* Direction muss noch in init function gesetzt werden */
  *cci_data->fbs.conf_ul_reg1 &= D_UL;

  if (cci_data->fbs.ciph_params.algo NEQ CIPH_EA0) 
  {
    *cci_data->fbs.conf_ul_reg1 |= ENCRYPT;
         
    if(cci_data->fbs.ciph_params.algo EQ CIPH_EA1) 
    {
      *cci_data->fbs.conf_ul_reg1 &= GEA_FIRST;
    }
    else
    if(cci_data->fbs.ciph_params.algo EQ CIPH_EA2)
    {
      *cci_data->fbs.conf_ul_reg1 |= GEA_SECOND;
    }
    else
    {
      TRACE_ERROR ("Illegal ciphering_algorithm");
    }

    /* 
     * if the condition is CCI_CIPHER_NO_ALGORITHM we don't use subsequent statements 
     */
    *cci_data->fbs.kc_reg1 = (USHORT)((USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[1]) << 8)+
                             (USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[0]);
    *cci_data->fbs.kc_reg2 = (USHORT)((USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[3]) << 8)+ 
                             (USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[2]);
    *cci_data->fbs.kc_reg3 = (USHORT)((USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[5]) << 8)+ 
                             (USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[4]);
    *cci_data->fbs.kc_reg4 = (USHORT)((USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[7]) << 8)+ 
                             (USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[6]);

    /* store ciphering key in the local buffer for later verification */
    if(cci_data->fbs.cci_info_trace){
      tmp_key[0] = *cci_data->fbs.kc_reg1;
      tmp_key[1] = *cci_data->fbs.kc_reg2;
      tmp_key[2] = *cci_data->fbs.kc_reg3;
      tmp_key[3] = *cci_data->fbs.kc_reg4;
    }

    /* 
     * FRAME DEPENDENT CIPHERING INPUT entity is ULONG in the primitive 
     */
    *cci_data->fbs.conf_ul_reg4 = (USHORT)(cipher_req_parms->gprs_parameters.ciphering_input);
    *cci_data->fbs.conf_ul_reg5 = (USHORT)(cipher_req_parms->gprs_parameters.ciphering_input >> 16);

    /*
     * it is assumed that MSB is first 16 bit and LSB part is last 16 bits 
     */ 
  }
  else 
  {     
    /* 
     * in this case we don't fill the registers that contain ciphering keys
     */
    *cci_data->fbs.conf_ul_reg1 &= NO_ENCRYPT;
  }

  /*
   * Enter LLC-PDU size in bytes
   */
  for (i = 0; i < in_data_list->c_in_data; i++) {
    len += in_data_list->ptr_in_data[i].len;
  }
  *cci_data->fbs.conf_ul_reg2 = (USHORT)len;
  /*
   * conf_ul_reg3 is seperated into 2 parts of UBYTE:
   * -> lowbyte   = N202
   * -> hightbyte = LLC-PDU header size in bytes (indicate the offset of information)
   */ 
  *cci_data->fbs.conf_ul_reg3 = (USHORT)((cipher_req_parms->gprs_parameters.header_size << 8) | CIPH_N202);

#ifdef _GEA_SIMULATION_
  cci_data->fbs.simulated_reg = cci_data->fbs.simulated_reg_buffer;
#endif

  *cci_data->fbs.conf_ul_reg1 &= NO_INPUT_SHIFT;

#if (BOARD == 61 OR BOARD == 71)	/* G-Sample or I-Sample */
  *cci_data->fbs.switch_reg = 1;

#ifdef LLC_TRACE_GEA_PARAM  
  cci_trace_gea_param();
#endif

#endif

  for (i = 0; i < in_data_list->c_in_data; i++) {
    desc_data8 = (U8*)in_data_list->ptr_in_data[i].buf;
    frame8 = in_data_list->ptr_in_data[i].len;
    for (j=0; j < frame8; j++)
    {
      *cci_data->fbs.data8_reg = *desc_data8;

  #ifdef _GEA_SIMULATION_
      ciph_reg8_write_sim();
  #endif
      desc_data8++;
    }
  }

#if (BOARD == 61 OR BOARD == 71)	/* G-Sample or I-Sample */
  *cci_data->fbs.switch_reg = 0;
#endif
} /* ciph_fill_ul_reg() */



/*
+------------------------------------------------------------------------------
| Function    : ciph_fill_dl_reg
+------------------------------------------------------------------------------
| Description : The function ciph_fill_dl_reg() 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void ciph_fill_dl_reg (T_CIPH_cipher_req_parms *cipher_req_parms,
                             T_CIPH_in_data_list *in_data_list )
{ 
  ULONG    i, j;
  ULONG    frame8;
  UBYTE    *desc_data8;
  ULONG    len = 0;
  
  TRACE_FUNCTION( "ciph_fill_dl_reg" );

  /* 
   * setting the protected mode variable
   */
  if(cipher_req_parms->gprs_parameters.pm EQ CIPH_PM_PROTECTED)    
  {
    *cci_data->fbs.conf_dl_reg1 |= PROTECTED;
  } 
  else
  {   
    *cci_data->fbs.conf_dl_reg1 &= NON_PROTECTED;
  }

  /* 
   * FCS is always calculated
   */
  *cci_data->fbs.conf_dl_reg1 |= FCS_COMPUTED; 

  /*
   * data always copied aligned independent 
   */
  *cci_data->fbs.conf_dl_reg1 &= NO_INPUT_SHIFT;

  /*
   * direction is downlink 
   */
  *cci_data->fbs.conf_dl_reg1 |= D_DL; 

  if(cci_data->fbs.ciph_params.algo NEQ CIPH_EA0)
  {
    *cci_data->fbs.conf_dl_reg1 |= ENCRYPT; 

    if(cci_data->fbs.ciph_params.algo EQ CIPH_EA1)
    {
      *cci_data->fbs.conf_dl_reg1 &= GEA_FIRST;
    }     
    else
    if(cci_data->fbs.ciph_params.algo EQ CIPH_EA2)
    {
      *cci_data->fbs.conf_dl_reg1 |= GEA_SECOND;
    }
    else
    {
      TRACE_ERROR ("Illegal ciphering_algorithm");
    }
   
    *cci_data->fbs.kc_reg1 = 
    (USHORT)((USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[1]) << 8) +
    (USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[0]);
    
    *cci_data->fbs.kc_reg2 = 
    (USHORT)((USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[3]) << 8) +
    (USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[2]);
    
    *cci_data->fbs.kc_reg3 = 
    (USHORT)((USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[5]) << 8) +
    (USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[4]);
    
    *cci_data->fbs.kc_reg4 = 
    (USHORT)((USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[7]) << 8) +
    (USHORT)(cci_data->fbs.ciph_params.ptr_ck->ck_element[6]);

    /* store ciphering key in local buffer for later verification */
    if(cci_data->fbs.cci_info_trace){
      tmp_key[0] = *cci_data->fbs.kc_reg1;
      tmp_key[1] = *cci_data->fbs.kc_reg2;
      tmp_key[2] = *cci_data->fbs.kc_reg3;
      tmp_key[3] = *cci_data->fbs.kc_reg4;
    }

    /* 
     * FRAME DEPENDENT CIPHERING INPUT  entity is ULONG in the primitive
     */
    *cci_data->fbs.conf_dl_reg4 = (USHORT)(cipher_req_parms->gprs_parameters.ciphering_input);
    *cci_data->fbs.conf_dl_reg5 = (USHORT)(cipher_req_parms->gprs_parameters.ciphering_input >> 16);
  }
  else 
  {
    /*
     * in this case we don't fill the registers that contain ciphering keys 
     */
    *cci_data->fbs.conf_dl_reg1 &= NO_ENCRYPT; 
  }

  /*
   * Write length of LLC-PDU in bytes including FCS
   */
  for (i = 0; i < in_data_list->c_in_data; i++) {
    len += in_data_list->ptr_in_data[i].len;
  }

  *cci_data->fbs.conf_dl_reg2 = (USHORT)len;

  /* 
   * conf_dl_reg3 is seperated into 2 parts of UBYTE:
   * -> lowbyte   = N202
   * -> hightbyte = LLC-PDU header size in bytes (indicate the offset of information)
   */ 
  *cci_data->fbs.conf_dl_reg3 = (USHORT)((cipher_req_parms->gprs_parameters.header_size << 8) | CIPH_N202);


#ifdef _GEA_SIMULATION_
  cci_data->fbs.simulated_reg = cci_data->fbs.simulated_reg_buffer;
#endif

#if (BOARD == 61 OR BOARD == 71)	/* G-Sample or I-Sample */
  *cci_data->fbs.switch_reg = 1;
#endif

  for (i = 0; i < in_data_list->c_in_data; i++) {
    desc_data8 = (UBYTE*)in_data_list->ptr_in_data[i].buf;
    frame8 = in_data_list->ptr_in_data[i].len;

    for (j=0; j < frame8; j++){
      *cci_data->fbs.data8_reg = desc_data8[j];
#ifdef _GEA_SIMULATION_
      ciph_reg8_write_sim();
#endif
    }
  }

#if (BOARD == 61 OR BOARD == 71)	/* G-Sample or I-Sample */
  *cci_data->fbs.switch_reg = 0;
#endif

} /* ciph_fill_dl_reg() */

#if (BOARD == 61 OR BOARD == 71)	/* G-Sample or I-Sample */

#ifdef LLC_TRACE_GEA_PARAM
LOCAL void cci_trace_gea_param( void )
{
  TRACE_EVENT_P9( "cci_trace_gea_param_1 %04X %04X %04X %04X %04X %04X %04X %04X %04X",
                  *cci_data->fbs.cntl_reg      ,
                  *cci_data->fbs.status_reg    ,
                  *cci_data->fbs.status_irq_reg,
                  *cci_data->fbs.conf_ul_reg1  ,
                  *cci_data->fbs.conf_ul_reg2  ,
                  *cci_data->fbs.conf_ul_reg3  ,
                  *cci_data->fbs.conf_ul_reg4  ,
                  *cci_data->fbs.conf_ul_reg5  ,
                  *cci_data->fbs.conf_dl_reg1   );

  TRACE_EVENT_P9( "cci_trace_gea_param_2 %04X %04X %04X %04X %04X %04X %04X %04X %04X",
                  *cci_data->fbs.conf_dl_reg2  ,
                  *cci_data->fbs.conf_dl_reg3  ,
                  *cci_data->fbs.conf_dl_reg4  ,
                  *cci_data->fbs.conf_dl_reg5  ,
                  *cci_data->fbs.fcs_ul_reg1   ,
                  *cci_data->fbs.fcs_ul_reg2   ,
                  *cci_data->fbs.fcs_dl_reg1   ,
                  *cci_data->fbs.fcs_dl_reg2   ,
                  *cci_data->fbs.switch_reg     );
}
#endif
#endif
#endif /* TI_PS_OP_CIPH_DRIVER */
