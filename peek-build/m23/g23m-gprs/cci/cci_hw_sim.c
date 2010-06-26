/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  cci_hw_sim.c
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
|  Purpose :  This module implements hardware simulation functions for
|             entity CCI.
+----------------------------------------------------------------------------- 
*/ 


#define CCI_HW_SIM_C

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/
#include <string.h>     /* to get memmove */

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

#include "cci_hw_sim.h"

#include "llc_f.h"      /* to get llc_build_crc24() */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

#ifdef _GEA_SIMULATION_


/*
+------------------------------------------------------------------------------
| Function    : ciph_hw_sim_cipher
+------------------------------------------------------------------------------
| Description : This functionn simulates the hardware during ciphering process    
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ciph_hw_sim_cipher (U16 len)
{
  /*
   * first move data one byte, if requested
   */

  if( *cci_data->fbs.conf_ul_reg1 & INPUT_SHIFT )
  {
    memmove (cci_data->fbs.simulated_reg_buffer,   /* dest */
             cci_data->fbs.simulated_reg_buffer+1, /* src  */
             len);
  }

  {
#ifdef _SIM_CALC_FCS_
    ULONG             fcs;

  /*
   * Build FCS (function returns already inversed CRC), store result in fcs.
   */
  fcs = llc_build_crc24 (cci_data->fbs.simulated_reg_buffer,
                         len);
  /*
   * Copy FCS to HW registers, taking byte ordering of FCS registers
   * into account, e.g.:
   * fcs              reg1   reg2
   * xx 36 29 FC  ->  29 FC  xx 36
   */
  *cci_data->fbs.fcs_ul_reg1 = (USHORT) (fcs & 0x0000FFFFL);
  *cci_data->fbs.fcs_ul_reg2 = (USHORT)((fcs & 0x00FF0000L) >> 16);
#else
  /*
   * Set FCS to all zeroes in simulation.
   */
  *cci_data->fbs.fcs_ul_reg1 = 0x0000;
  *cci_data->fbs.fcs_ul_reg2 = 0x0000;
#endif /* _SIM_CALC_FCS_ */
  }
  /*
   * HW simulation: set status register to indicate finished work and emulate
   * timer afterwards.
   */
  *cci_data->fbs.status_reg &= NOT_WORKING;

} /* ciph_hw_sim_cipher */

 /*
+------------------------------------------------------------------------------
| Function    : ciph_hw_sim_decipher
+------------------------------------------------------------------------------
| Description : This functionn simulates the hardware during deciphering process    
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ciph_hw_sim_decipher ( void )
{
  /*
   * set all registers to indicate finished work.
   * FCS will be checked later.
   */
  *cci_data->fbs.fcs_dl_reg1 &= 0x0000;
  *cci_data->fbs.fcs_dl_reg2 &= 0x0000;

  *cci_data->fbs.status_reg  &= NOT_WORKING;

} /* ciph_hw_sim_decipher */



/*
+------------------------------------------------------------------------------
| Function    : ciph_reg16_write_sim
+------------------------------------------------------------------------------
| Description : This function simulates the hardware writing process on windows 
|               environment
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ciph_reg16_write_sim ( void )
{ 
  UBYTE *data = (UBYTE *)cci_data->fbs.data16_reg;;

  /*
   * To support any alignment the copy process must be done in two steps 
   * by the use of 8 bit char pointers
   */
  *cci_data->fbs.simulated_reg = *data;
  cci_data->fbs.simulated_reg++;

  data++;

  *cci_data->fbs.simulated_reg = *data;
  cci_data->fbs.simulated_reg++;
  
} /* ciph_reg16_write_sim */




/*
+------------------------------------------------------------------------------
| Function    : ciph_reg8_write_sim
+------------------------------------------------------------------------------
| Description : This function simulates the 8 bit hardware writing process 
|               by the use of 8 bit register.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ciph_reg8_write_sim ( void )
{ 
  *cci_data->fbs.simulated_reg = *cci_data->fbs.data8_reg;
  cci_data->fbs.simulated_reg++;

} /* ciph_reg8_write_sim() */


/*
+------------------------------------------------------------------------------
| Function    : ciph_reg16_read_sim
+------------------------------------------------------------------------------
| Description : This function simulates the 16 bit hardware reading process 
|               by the use of a 16 bit register
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ciph_reg16_read_sim ( void )
{   
  UBYTE *data = (UBYTE *)cci_data->fbs.data16_reg;
  
  /*
   * To support any alignment the copy process must be done in two steps 
   * by the use of 8 bit char pointers
   */
  *data = *cci_data->fbs.simulated_reg;
  cci_data->fbs.simulated_reg++;

  data++;  

  *data = *cci_data->fbs.simulated_reg;
  cci_data->fbs.simulated_reg++;

} /* ciph_reg16_read_sim*/


/*
+------------------------------------------------------------------------------
| Function    : ciph_reg8_read_sim
+------------------------------------------------------------------------------
| Description : This function simulates the 8 bit hardware reading process 
|               by the use of 8 bit register.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ciph_reg8_read_sim ( void )
{   
  *cci_data->fbs.data8_reg = *cci_data->fbs.simulated_reg;
  cci_data->fbs.simulated_reg++;

} /* ciph_reg8_read_sim*/



#endif /* _GEA_SIMULATION */
