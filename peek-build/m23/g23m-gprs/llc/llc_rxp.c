/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  
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
|  Purpose :  This modul is part of the entity LLC and implements all 
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (RX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_RXP_C
#define LLC_RXP_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */

#include "llc_rxf.h"    /* to get local RX functions */
#include "llc_us.h"     /* to get signal interface to U */
#include "llc_uirxs.h"  /* to get signal interface to UIRX */
#include "llc_irxs.h"   /* to get signal interface to IRX */
 
#ifdef _SIMULATION_
#include <string.h>     /* to get memcpy() */
#endif

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

#ifdef _SIMULATION_
LOCAL void rx_copy_test_primitive_data  (T_GRLC_UNITDATA_IND_TEST 
                                         *grlc_unitdata_ind_test, 
                                         T_GRLC_UNITDATA_IND *grlc_unitdata_ind);
#endif

#ifndef CF_FAST_EXEC

GLOBAL void rx_grlc_xdata_ind             (T_GRLC_UNITDATA_IND *grlc_unitdata_ind);

#endif /* CF_FAST_EXEC */


/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : rx_grlc_data_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive GRLC_DATA_IND
|
| Parameters  : *grlc_data_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void rx_grlc_data_ind ( T_GRLC_DATA_IND *grlc_data_ind )
{ 
  TRACE_FUNCTION( "grlc_data_ind" );
  
  switch( GET_STATE( RX ) )
  {
    case RX_TLLI_ASSIGNED:
    {
      /*
       * Both primitives are treated the same way and contain the same 
       * information.
       */
      PPASS (grlc_data_ind, grlc_unitdata_ind, GRLC_UNITDATA_IND);

      /*
       * Primitive is handled in rx_grlc_xdata_ind().
       */
      rx_grlc_xdata_ind (grlc_unitdata_ind);
      break;
    }
    default:
      PFREE_DESC (grlc_data_ind);
      TRACE_ERROR( "GRLC_DATA_IND unexpected" );
      break;
  }

} /* rx_grlc_data_ind() */

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : rx_grlc_unitdata_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive GRLC_UNITDATA_IND
|
| Parameters  : *grlc_unitdata_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void rx_grlc_unitdata_ind ( T_GRLC_UNITDATA_IND *grlc_unitdata_ind )
{ 
  TRACE_FUNCTION( "grlc_unitdata_ind" );
  
  switch( GET_STATE( RX ) )
  {
    case RX_TLLI_ASSIGNED:
      /*
       * Primitive is handled in rx_grlc_xdata_ind().
       */
      rx_grlc_xdata_ind (grlc_unitdata_ind);
      break;
    default:
      PFREE_DESC (grlc_unitdata_ind);
      TRACE_ERROR( "GRLC_UNITDATA_IND unexpected" );
      break;
  }

} /* rx_grlc_unitdata_ind() */

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : rx_cci_decipher_cnf
+------------------------------------------------------------------------------
| Description : Handles the primitive CCI_DECIPHER_CNF. 
|               Note: The type LL_UNITDATA_IND is used instead to avoid PPASS
.
|
| Parameters  : *ll_unitdata_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
/*#if defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   defined(LL_2to1) */

GLOBAL void rx_cci_decipher_cnf ( T_LL_UNITDATA_IND *ll_unitdata_ind)
{
  UBYTE             cipher;
  T_PDU_TYPE        frame_type;
  T_COMMAND         command;
  T_BIT             cr_bit;
  T_BIT             pf_bit;
  T_FRAME_NUM       nr;
  T_FRAME_NUM       ns;
  BOOL              frame_ok;
  UBYTE             frame_rej;
  USHORT            frame_rej_ctrl_length;
  USHORT            ctrl_len;


  TRACE_FUNCTION( "rx_cci_decipher_cnf" );

  cipher  = ll_unitdata_ind->cipher;

  switch( GET_STATE( RX ) )
  {
    case RX_TLLI_ASSIGNED:
      /* variable sapi is "misused" for fcs_check value */
      if (ll_unitdata_ind->sapi EQ CCI_FCS_PASSED)
      {

        {
          /*
           * Label INTERPRET
           */

          rx_interpret_frame (&ll_unitdata_ind->sdu, &ll_unitdata_ind->sapi, 
            &frame_type, &command, &cr_bit, &pf_bit, &nr, &ns, &frame_ok, 
            &frame_rej, &frame_rej_ctrl_length, cipher);

          SWITCH_LLC (ll_unitdata_ind->sapi);

          /*
           * In case of I-frames check, if the information field exceeds N201-I
           */
          if (frame_type == I_FRAME)
          {
            ctrl_len = I_CTRL_MIN_OCTETS;

            /*
             * Add sizeof SACK-Bitmap, if necessarry (add K+1)
             */
            if (command == I_SACK)
            {
              ctrl_len += (ll_unitdata_ind->sdu.buf[(ll_unitdata_ind->sdu.o_buf/8)+4] 
                            & 0x1F) + 1;
            }
            
            if (BYTELEN(ll_unitdata_ind->sdu.l_buf) > *(llc_data->n201_i) + ctrl_len)
            {
              frame_ok  = FALSE;
              frame_rej = FRMR_W2;

              TRACE_0_INFO("Received frame violates N201-I: send FRMR");
            }
          }

          /*
           * frame_ok includes: correct frame length, PD bit == 0, SAPI valid, 
           * FCS correct, known PDU type.
           */
          if (frame_ok EQ TRUE)
          {
            rx_strip_llc_header (&ll_unitdata_ind->sdu, frame_type, command);

            /*
             * Label S_DATA
             */

            switch (frame_type)
            {
              case U_FRAME:
                /*
                 * Label U_FRAME
                 */
        
                /*
                 * Service u_frames receives the SDU within the allocated UNITDATA
                 * primitive.
                 */
                sig_rx_u_data_ind (ll_unitdata_ind, command, cr_bit, pf_bit);
                break;
              case UI_FRAME:
                /*
                 * Label UI_FRAME
                 */
        
                sig_rx_uirx_data_ind (ll_unitdata_ind, nr);
                break;
              case S_FRAME:
                /*
                 * No break. S frames and I frames are treated the same way.
                 */
              case I_FRAME:
                /*
                 * Label I_FRAME
                 */

                sig_rx_irx_data_ind (ll_unitdata_ind, 
                                     command, frame_type, cr_bit, pf_bit, ns, nr);
                break;
              default:
                PFREE (ll_unitdata_ind);
                TRACE_ERROR ("unknown frame type");
                break;
            }
          }
          else /* frame_ok EQ FALSE */
          {
            /*
             * Check if frame rejection condition occurred, and if U has to be
             * informed.
             */
            if (frame_rej EQ FRAME_NOT_REJ)
            {
              TRACE_0_INFO("Frame ignored due to decode problem");
              PFREE (ll_unitdata_ind);
            }
            else /* W1 bit and/or W3 bit set */
            {
              /*
               * Inform U of the frame rejection condition.
               */
              TRACE_0_INFO("Frame rejected due to decode problem");
              sig_rx_u_frmr_ind (ll_unitdata_ind, frame_type, frame_rej_ctrl_length,
                cr_bit, frame_rej);
            }
          }
        } /* end of validity range of ll_unitdata_ind */
      }
      else /* fcs_check EQ CCI_FCS_FAILED */
      {
#ifdef TRACE_EVE
        UBYTE sapi;

        rx_interpret_frame (&ll_unitdata_ind->sdu, &sapi, 
          &frame_type, &command, &cr_bit, &pf_bit, &nr, &ns, &frame_ok, 
          &frame_rej, &frame_rej_ctrl_length, cipher);
#endif
        TRACE_0_INFO("Frame discarded due to FCS");
        PFREE (ll_unitdata_ind);
      }
      break;
    default:
      PFREE(ll_unitdata_ind);
      TRACE_ERROR( "CCI_DECIPHER_CNF unexpected" );
      break;
  }

} /* rx_cci_decipher_cnf() */

/*#endif */ /* CF_FAST_EXEC || _SIMULATION_ */


/*
+------------------------------------------------------------------------------
| Function    : rx_grlc_data_ind_test
+------------------------------------------------------------------------------
| Description : Handles the primitive GRLC_DATA_IND_TEST
|               NOTE: This is only necessary in simulation environment.
|
| Parameters  : *grlc_data_ind_test - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifdef _SIMULATION_
GLOBAL void rx_grlc_data_ind_test ( T_GRLC_DATA_IND_TEST *grlc_data_ind_test )
{
  TRACE_FUNCTION( "grlc_data_ind_test" );
  
  switch( GET_STATE( RX ) )
  {
    case RX_TLLI_ASSIGNED:
    {
      /*
       * Allocate a "normal" GRLC_UNITDATA_IND primitive and copy the data
       * of the test primitive GRLC_DATA_IND_TEST (sdu) to 
       * GRLC_UNITDATA_IND (desc_list).
       */
      PALLOC_DESC (grlc_unitdata_ind, GRLC_UNITDATA_IND);

      grlc_unitdata_ind->tlli = grlc_data_ind_test->tlli;
      rx_copy_test_primitive_data ((T_GRLC_UNITDATA_IND_TEST *)grlc_data_ind_test,
        grlc_unitdata_ind);

      /*
       * Free the received test primitive.
       */
      PFREE (grlc_data_ind_test);

      /*
       * Primitive is handled in rx_grlc_xdata_ind().
       */
      rx_grlc_xdata_ind (grlc_unitdata_ind);
      break;
    }
    default:
      PFREE (grlc_data_ind_test);
      TRACE_ERROR( "GRLC_DATA_IND_TEST unexpected" );
      break;
  }

} /* rx_grlc_data_ind_test() */
#endif /* _SIMULATION_ */


/*
+------------------------------------------------------------------------------
| Function    : rx_grlc_unitdata_ind_test
+------------------------------------------------------------------------------
| Description : Handles the primitive GRLC_UNITDATA_IND_TEST
|               NOTE: This is only necessary in simulation environment.
|
| Parameters  : *grlc_unitdata_ind_test - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifdef _SIMULATION_
GLOBAL void rx_grlc_unitdata_ind_test ( T_GRLC_UNITDATA_IND_TEST 
                                      *grlc_unitdata_ind_test )
{
  TRACE_FUNCTION( "grlc_unitdata_ind_test" );
  
  switch( GET_STATE( RX ) )
  {
    case RX_TLLI_ASSIGNED:
    {
      /*
       * Allocate a "normal" GRLC_UNITDATA_IND primitive and copy the data
       * of the test primitive GRLC_UNITDATA_IND_TEST (sdu) to 
       * GRLC_UNITDATA_IND (desc_list).
       */
      PALLOC_DESC (grlc_unitdata_ind, GRLC_UNITDATA_IND);

      grlc_unitdata_ind->tlli = grlc_unitdata_ind_test->tlli;
      rx_copy_test_primitive_data (grlc_unitdata_ind_test, grlc_unitdata_ind);

      /*
       * Free the received test primitive.
       */
      PFREE (grlc_unitdata_ind_test);

      /*
       * Primitive is handled in rx_grlc_xdata_ind().
       */
      rx_grlc_xdata_ind (grlc_unitdata_ind);
      break;
    }
    default:
      PFREE (grlc_unitdata_ind_test);
      TRACE_ERROR( "GRLC_UNITDATA_IND_TEST unexpected" );
      break;
  }

} /* rx_grlc_unitdata_ind_test() */
#endif /* _SIMULATION_ */


/*
+------------------------------------------------------------------------------
| Function    : rx_copy_test_primitive_data
+------------------------------------------------------------------------------
| Description : Copies the data of a TEST primitive (sdu) to a normal 
|               primitive (desc_list).
|               ATTENTION: All other parameters of the primitives are left 
|               untouched and are not copied by this function!
|
| Parameters  : *grlc_unitdata_ind_test  - source primitive
|               *grlc_unitdata_ind       - destination primitive
|
+------------------------------------------------------------------------------
*/
#ifdef _SIMULATION_
LOCAL void rx_copy_test_primitive_data (T_GRLC_UNITDATA_IND_TEST 
                                        *grlc_unitdata_ind_test, 
                                        T_GRLC_UNITDATA_IND *grlc_unitdata_ind)
{
#define FRAG_LEN    80 /* value + 6 must fit in an pool with lots of entries */

  T_sdu             *sdu;
  T_desc            *desc;
  T_desc            *last_desc = NULL;
  int               sdu_index;
  int               length;


  sdu = &grlc_unitdata_ind_test->sdu;

  /*
   * Begin at the first relevant octet.
   */
  sdu_index = sdu->o_buf/8;

  /*
   * Initialise descriptor list length.
   */
  grlc_unitdata_ind->desc_list.list_len = 0;


  /*
   * Copy complete SDU to descriptor list using descriptors of max. 10 bytes.
   */
  while (sdu_index < sdu->l_buf/8)
  {
    /*
     * Calculate length of descriptor data (= length of remaining sdu buffer
     * with a maximum of FRAG_LEN)
     */
    length = (sdu_index+FRAG_LEN < sdu->l_buf/8) ? FRAG_LEN 
                                                 : (sdu->l_buf/8 - sdu_index);

    /*
     * Allocate the necessary size for the data descriptor. The size is 
     * calculated as follows:
     * - take the size of a descriptor structure
     * - subtract one because of the array buffer[1] to get the size of
     *   descriptor control information
     * - add number of octets of descriptor data
     */
    MALLOC (desc, (USHORT)(sizeof(T_desc) - 1 + length));

    /*
     * Fill descriptor control information.
     */
    desc->next  = (ULONG)NULL;
    desc->len   = length;

    /*
     * Add length of descriptor data to list length.
     */
    grlc_unitdata_ind->desc_list.list_len  += length;

    /*
     * Copy user data from SDU to descriptor.
     */
    memcpy (desc->buffer, &sdu->buf[sdu_index], length);
    sdu_index += length;

    if (last_desc)
    {
      /*
       * Add this descriptor (not the first) to the descriptor list.
       */
      last_desc->next = (ULONG)desc;
    }
    else
    {
      /*
       * Insert first descriptor in descriptor list.
       */
      grlc_unitdata_ind->desc_list.first     = (ULONG)desc;
    }

    /*
     * Store this descriptor for later use.
     */
    last_desc = desc;
  }

  return;
} /* rx_copy_test_primitive_data */
#endif /* _SIMULATION_ */


/*
+------------------------------------------------------------------------------
| Function    : rx_grlc_xdata_ind
+------------------------------------------------------------------------------
| Description : Handles the primitives GRLC_DATA_IND / GRLC_UNITDATA_IND.
|
| Parameters  : *grlc_unitdata_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void rx_grlc_xdata_ind ( T_GRLC_UNITDATA_IND *grlc_unitdata_ind )
{
  T_PDU_TYPE        frame_type;
  UBYTE             protected_mode;
  UBYTE             sapi;
  T_FRAME_NUM       ns;
  BOOL              ciphering;
  USHORT            header_size;
  BOOL              frame_ok;


  TRACE_FUNCTION( "grlc_xdata_ind" );

  /*
   * Unassigning old TLLI 
   *
   * GMM has to be informed, that new TLLI has been received. So GMM can unassign old TLLI 
   * and old PTMSI in GRLC
   * Old tlli will be unassigned here. Normally GMM has to sent LLGMM_ASSIGN_REQ with
   * new_tlli != all 1's and old_tlli == all 1,s. See 04.64 cp. 6.1 <R.LLC.TLLI_ASS.A.002>
   */
#ifdef LL_2to1
  if ( PS_TLLI_INVALID     != llc_data->tlli_old 
#else
  if ( LLGMM_TLLI_INVALID     != llc_data->tlli_old 
#endif
  &&   grlc_unitdata_ind->tlli != llc_data->tlli_old )
  {
    PALLOC ( llgmm_tlli_ind, LLGMM_TLLI_IND );
      llgmm_tlli_ind->new_tlli = grlc_unitdata_ind->tlli;
#ifdef LL_2to1
      llc_data->tlli_old = PS_TLLI_INVALID;
#else
      llc_data->tlli_old = LLGMM_TLLI_INVALID;
#endif
    PSEND ( hCommGMM,   llgmm_tlli_ind);
  }
  rx_analyse_ctrl_field (grlc_unitdata_ind, &frame_type, &protected_mode,
    &sapi, &ns, &ciphering, &header_size, &frame_ok);

  if (frame_ok EQ TRUE)
  {
    /*
     * Check, if the sapi of the frame is supported. If ok, switch context 
     * and handle the frame.
     */
    switch (sapi)
    {
      case LL_SAPI_1:
      case LL_SAPI_3:
      case LL_SAPI_5:
      case LL_SAPI_7:
      case LL_SAPI_9:
      case LL_SAPI_11:
        SWITCH_LLC (sapi);
        rx_send_decipher_req (grlc_unitdata_ind, frame_type, protected_mode, 
          ns, header_size, ciphering);
        /*
         * Free only the primitive (desc_list copied in rx_send_decipher_req)
         */
        PFREE (grlc_unitdata_ind); /* Do not use PFREE_DESC here !*/
        break;

      default:
        /*
         * Ignore frame.
         * Free prim and desc_list, because they are not used further
         */
        PFREE_DESC (grlc_unitdata_ind);
        TRACE_0_INFO("Frame received for reserved SAPI");
        break;
    }
  }
  else /* frame_ok NEQ TRUE */
  {
    /*
     * Free GRLC_UNITDATA_IND along with complete descriptor list.
     */
    PFREE_DESC (grlc_unitdata_ind);
    TRACE_EVENT("Frame ignored!");
  }

  return;
} /* rx_grlc_xdata_ind() */

#endif /* CF_FAST_EXEC */

