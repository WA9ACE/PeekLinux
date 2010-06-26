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
|             procedures and functions as described in the 
|             SDL-documentation (RX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_RXF_C
#define LLC_RXF_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include <string.h>     /* to get memcpy() */

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */

#include "llc_rxf.h"    /* to get local defines */
#include "llc_rxp.h"    /* to get the function rx_cci_decipher_cnf */
#include "llc_f.h"      /* to get global functions, e.g. llc_generate_input */
#ifndef TI_PS_OP_CIPH_DRIVER 
#include "cci_fbsf.h"     /* to get functional interface */
#endif
/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/
#ifndef CF_FAST_EXEC

GLOBAL UBYTE rx_get_desc_octet (T_desc_list *desc_list, 
                               USHORT offset, 
                               UBYTE *data_ptr);
#endif /* CF_FAST_EXEC */

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : rx_init
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of 
|               receive_pdu.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void rx_init (void)
{ 
  TRACE_FUNCTION( "rx_init" );
  
  /*
   * Initialise service RX with state TLLI_UNASSIGNED.
   */
  INIT_STATE (RX, RX_TLLI_UNASSIGNED);

  return;
} /* rx_init() */

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : rx_analyse_ctrl_field
+------------------------------------------------------------------------------
| Description : This procedure analyses the received LLC frame control field 
|               and sets frame_type according to the frame type (U, UI, I/S). 
|               protected_mode is set according to the protected mode of the 
|               frame (U and I/S always protected, UI depends on setting of 
|               PM bit). The type of protected_mode is the same as in 
|               CCI_DECIPHER_REQ. ns is set to N(S) for I frames, N(U) for 
|               UI frames, or an undefined value for S and U frames. ciphering 
|               is set to TRUE if the frame is ciphered (U never, I/S always, 
|               UI depends on setting of E bit), otherwise to FALSE. frame_ok 
|               indicates if the frame contains enough octets to contain a 
|               known control field (i.e. all necessary information is 
|               accessible, not the control field is complete!).
|
| Parameters  : grlc_unitdata_ind  - a valid pointer to a GRLC_UNITDATA_IND 
|                                   primitive
|               frame_type        - a valid pointer to a T_PDU_TYPE variable
|               protected_mode    - a valid pointer to a UBYTE variable
|               sapi              - a valid pointer to a UBYTE variable
|               ns                - a valid pointer to a T_FRAME_NUM variable
|               ciphering         - a valid pointer to a BOOL variable
|               header_size       - a valid pointer to a USHORT variable
|               frame_ok          - a valid pointer to a BOOL variable
|
+------------------------------------------------------------------------------
*/

#ifndef CF_FAST_EXEC

GLOBAL void rx_analyse_ctrl_field (T_GRLC_UNITDATA_IND *grlc_unitdata_ind,
                                   T_PDU_TYPE         *frame_type,
                                   UBYTE              *protected_mode,
                                   UBYTE              *sapi,
                                   T_FRAME_NUM        *ns,
                                   BOOL               *ciphering,
                                   USHORT             *header_size,
                                   BOOL               *frame_ok)
{
  UBYTE             first_octet;
  UBYTE             sec_octet;
  UBYTE             command_octet;
  UBYTE             sack_k_octet;


  TRACE_FUNCTION( "rx_analyse_ctrl_field" );

  /*
   * Check if the frame contains enough octets to access the first octet of
   * the control field to find out the type of the frame.
   */
  if (grlc_unitdata_ind->desc_list.list_len < CTRL_MIN_OCTETS)
  {
    *frame_ok = FALSE;
    return;
  }

  /*
   * Assume initially that the frame is ok.
   */
  *frame_ok = TRUE;
  
  /*
   * Set first_octet to the value of the first frame octet (offset 0), which 
   * is the address field.
   */
  rx_get_desc_octet (&grlc_unitdata_ind->desc_list, 0, &first_octet);

  *sapi = first_octet & 0x0F;

  /*
   * Set sec_octet to the value of the second frame octet (offset 1), which 
   * is the first octet of the control field.
   */
  rx_get_desc_octet (&grlc_unitdata_ind->desc_list, 1, &sec_octet);


  /*
   * Determine frame_type, along with ns, protected_mode, and ciphering,
   * depending on the frame type.
   */
  if ((sec_octet & I_FRAME_MASK) EQ I_FRAME_ID)
  {
    /*
     * I frame, protected, ciphered, at least 4 octets required to access all
     * requested information (1 Address, 3 Control).
     */
    *frame_type     = I_FRAME;
    *protected_mode = CCI_PM_PROTECTED;
    *ciphering      = TRUE;

    /*
     * Check if the frame contains enough octets to access the complete
     * I frame control field.
     */
    if (grlc_unitdata_ind->desc_list.list_len < I_CTRL_MIN_OCTETS)
    {
      *frame_ok = FALSE;
      return;
    }

    /*
     * Determine the header_size
     */
    *header_size = I_CTRL_MIN_OCTETS;

    /*
     * Add bytes in case of SACK-Bitmap (add K+1). Therefore get at first 
     * the command octet (offset 3). In case of an SACK, get next the k 
     * octet (offset 4) and add the additional size.
     */
    rx_get_desc_octet (&grlc_unitdata_ind->desc_list, 3, &command_octet);
    
    if ( (command_octet & 0x03) == I_FRAME_SACK )
    {
      *header_size += 1; /* k octet */

      if (grlc_unitdata_ind->desc_list.list_len < *header_size)
      {
        *frame_ok = FALSE;
        return;
      }

      rx_get_desc_octet (&grlc_unitdata_ind->desc_list, 4, &sack_k_octet);

      *header_size += (sack_k_octet & 0x1F); /* bitmap size */

      if (grlc_unitdata_ind->desc_list.list_len < *header_size)
      {
        *frame_ok = FALSE;
        return;
      }
    }

    /*
     * Extract N(S) and store it in ns.
     */
    *ns = (((T_FRAME_NUM)
      rx_get_desc_octet (&grlc_unitdata_ind->desc_list, 1, NULL)) & 0x1F) << 4;
    *ns |= (((T_FRAME_NUM)
      rx_get_desc_octet (&grlc_unitdata_ind->desc_list, 2, NULL)) >> 4);
  }
  else if ((sec_octet & S_FRAME_MASK) EQ S_FRAME_ID)
  {
    /*
     * S frame, protected, not ciphered. No N(S) present, only N(R).
     */
    *frame_type     = S_FRAME;
    *protected_mode = CCI_PM_PROTECTED;
    *ciphering      = FALSE;

    *header_size    = S_CTRL_MIN_OCTETS;
    
    /* not necessary to add bytes in case of SACK - value is not used */
  }
  else if ((sec_octet & UI_FRAME_MASK) EQ UI_FRAME_ID)
  {
    /*
     * UI frame, at least 3 octets required to access all requested 
     * information (1 Address, 2 Control).
     */
    *frame_type     = UI_FRAME;

    /*
     * Check if the frame contains enough octets to access the complete
     * UI frame control field.
     */
    if (grlc_unitdata_ind->desc_list.list_len < UI_CTRL_MIN_OCTETS)
    {
      *frame_ok = FALSE;
      return;
    }

    /*
     * Extract protected mode setting of frame (PM bit).
     */
    if (rx_get_desc_octet (&grlc_unitdata_ind->desc_list, 2, NULL) & 0x01)
    {
      *protected_mode = CCI_PM_PROTECTED;
    }
    else
    {
      *protected_mode = CCI_PM_UNPROTECTED;
    }

    /*
     * Extract ciphering setting of frame (E bit).
     */
    if (rx_get_desc_octet (&grlc_unitdata_ind->desc_list, 2, NULL) & 0x02)
    {
      *ciphering = TRUE;
    }
    else
    {
      *ciphering = FALSE;
    }

    *header_size = UI_CTRL_MIN_OCTETS;

    /*
     * Extract N(U) and store it in ns.
     */
    *ns = (((T_FRAME_NUM)
      rx_get_desc_octet (&grlc_unitdata_ind->desc_list, 1, NULL)) & 0x07) << 6;
    *ns |= (((T_FRAME_NUM)
      rx_get_desc_octet (&grlc_unitdata_ind->desc_list, 2, NULL)) >> 2);
  }
  else if ((sec_octet & U_FRAME_MASK) EQ U_FRAME_ID)
  {
    /*
     * U frame, protected, not ciphered. No N(S) present.
     */
    *frame_type     = U_FRAME;
    *protected_mode = CCI_PM_PROTECTED;
    *ciphering      = FALSE;

    *header_size    = U_CTRL_MIN_OCTETS;
  }

  return;
} /* rx_analyse_ctrl_field() */

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : rx_decipher_req
+------------------------------------------------------------------------------
| Description : Handles the function rx_decipher_req. This functions sets the
|               ciphering parameters and calls the deciphering driver function.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/

#ifndef CF_FAST_EXEC
GLOBAL void rx_decipher_req (T_CCI_DECIPHER_REQ *decipher_req)
{ 
  T_CIPH_init_cipher_req_parms  init_cipher_req_parms;
  T_CIPH_cipher_req_parms       cipher_req_parms;
  T_CIPH_in_data_list           in_data_list;
  T_CIPH_out_data               out_data;
  T_CIPH_ck                     ck;
  U16                           i;
  U8                            status;
  U16                           cnt = 0;

  TRACE_FUNCTION( "rx_decipher_req" );

#ifdef LLC_TRACE_CIPHERING
  TRACE_EVENT("DOWNLINK CIPHERED DATA");
  llc_trace_desc_list(&decipher_req->desc_list);
#endif

  /*
   * Copy pointer to desc's from CIPHER_REQ to the in_data_list
   * The in_data array in allocated dynamically in this func.
   */
  llc_copy_dl_data_to_list(decipher_req, &in_data_list);
  /*
   * Set ciphering parameters 
   */
  cipher_req_parms.gprs_parameters.pm = decipher_req->pm;
  cipher_req_parms.gprs_parameters.header_size = decipher_req->header_size;
  cipher_req_parms.gprs_parameters.ciphering_input = 
                                             decipher_req->ciphering_input;
  cipher_req_parms.gprs_parameters.threshold = 0;
  init_cipher_req_parms.direction = CIPH_DOWNLINK_DIR;
  init_cipher_req_parms.algo      = decipher_req->ciphering_algorithm;
  init_cipher_req_parms.ptr_ck = & ck;
  /*
   * Copy ciphering key
   */
  for (i = 0; i < 8;i++) {
    init_cipher_req_parms.ptr_ck->ck_element[i] = decipher_req->kc.key[i];
  }

  {
    /* Use GRLC_DATA_REQ instead of CCI_CIPHER_CNF to avoid PPASS in LLC*/
    PALLOC_SDU (ll_unitdata_ind, LL_UNITDATA_IND,
           (USHORT)((decipher_req->desc_list.list_len*8) - FCS_SIZE_BITS));
  
    ll_unitdata_ind->sdu.o_buf = 0;
    ll_unitdata_ind->sdu.l_buf = 0;
    out_data.buf = 
         (U32)(&ll_unitdata_ind->sdu.buf[ll_unitdata_ind->sdu.o_buf >> 3]);
    /*
     * Initialize ciphering driver and decipher data
     */
#ifdef TI_PS_OP_CIPH_DRIVER
    ciph_init_cipher_req (&init_cipher_req_parms, NULL);
    ciph_cipher_req (&cipher_req_parms, &in_data_list, &out_data, &status);
#else
    ciph_init_cipher_req_sim (&init_cipher_req_parms, NULL);
    ciph_cipher_req_sim (&cipher_req_parms, &in_data_list, &out_data, &status);
#endif
    /*
     * "Send" DECIPHER_CNF to LLC
     */
    ll_unitdata_ind->sdu.l_buf  = out_data.len * 8;
    ll_unitdata_ind->tlli       = decipher_req->reference1;
    ll_unitdata_ind->cipher     = (UBYTE)decipher_req->reference2;
    if (status == CIPH_CIPH_PASS){
      ll_unitdata_ind->sapi = CCI_FCS_PASSED;
    } else {
      ll_unitdata_ind->sapi = CCI_FCS_FAILED;
    }

#ifdef LLC_TRACE_CIPHERING
    TRACE_EVENT("DOWNLINK DECIPHERED DATA");
    llc_trace_sdu(&ll_unitdata_ind->sdu);
#endif
      rx_cci_decipher_cnf(ll_unitdata_ind);
  }
  /*
   * Free in_data array from in_data_list allocated in llc_copy_dl_data_to_list
   */
  if(in_data_list.ptr_in_data != NULL){
    MFREE(in_data_list.ptr_in_data);
    in_data_list.ptr_in_data = NULL;
  }
  /*
   * Free descriptors from the deipher_req->desc_list
   */
  if (decipher_req != NULL)
  {
    T_desc *desc = (T_desc *)decipher_req->desc_list.first;
    T_desc *next_desc;
    while (desc NEQ NULL){
      next_desc = (T_desc *)desc->next;
      MFREE (desc);
      desc = next_desc;
      /* increase freed partitions counter */
      cnt++;
    }       
    MFREE(decipher_req);
    decipher_req = NULL;
  }
  llc_data->fbs.cci_freed_partition +=cnt;

  /* trace number of freed partitions */
  if(llc_data->fbs.cci_info_trace){
    TRACE_EVENT_P2("INFO CCI: freed partitions %ld, total=%ld",
                        cnt,llc_data->fbs.cci_freed_partition);
  }

} /* rx_decipher_req */

#endif

/*
+------------------------------------------------------------------------------
| Function    : rx_send_decipher_req
+------------------------------------------------------------------------------
| Description : This procedure allocates the CCI_DECIPHER_REQ primitive, fills
|               in all necessary parameters and sends the primitive to CCI.
|
| Parameters  : grlc_unitdata_ind  - a valid pointer to a GRLC_UNITDATA_IND 
|                                   primitive
|               frame_type        - indicates (un)acknowledged operation mode
|               protected_mode    - pm setting for CCI_DECIPHER_REQ
|               ns                - N(S)/N(U) for deciphering, otherwise
|                                   undefined value
|               header_size       - size of header bytes (not ciphered bytes)
|               ciphering         - indicates deciphering (TRUE/FALSE)
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void rx_send_decipher_req (T_GRLC_UNITDATA_IND *grlc_unitdata_ind,
                                  T_PDU_TYPE  frame_type,
                                  UBYTE       protected_mode,
                                  T_FRAME_NUM ns,
                                  USHORT      header_size,
                                  BOOL        ciphering)
{
  ULONG oc;
  
  TRACE_FUNCTION ("rx_send_decipher_req");

  {
    /*
     * No need to PPASS GRLC_xDATA_IND, because desc_list contains a pointer 
     * which can be simply copied.
     */
    T_CCI_DECIPHER_REQ *cci_decipher_req;
    MALLOC(cci_decipher_req, sizeof(T_CCI_DECIPHER_REQ));

    /*
     * Requires rx_analyse_ctrl_field() to set a correct CCI value.
     */
    cci_decipher_req->pm = protected_mode;

    if (ciphering EQ TRUE)
    {
      cci_decipher_req->reference2 = LL_CIPHER_ON; /* re-use of reference is ok */
      cci_decipher_req->ciphering_algorithm = llc_data->ciphering_algorithm;
      memcpy (&cci_decipher_req->kc, &llc_data->kc, sizeof(T_kc));

      /*
       * Calculate the OC which is valid for this ns. This could be the
       * current OC or OC + 1 in case of an modulo overflow of ns
       */
      switch (frame_type)
      {
        case I_FRAME:
        case S_FRAME:
          if (ns >= llc_data->sapi->vr)
            oc = llc_data->sapi->oc_i_rx;
          else
            oc = llc_data->sapi->oc_i_rx + (MAX_SEQUENCE_NUMBER+1);
          break;

        default:
          if (ns >= llc_data->sapi->vur)
            oc = llc_data->sapi->oc_ui_rx;
          else
            oc = llc_data->sapi->oc_ui_rx + (MAX_SEQUENCE_NUMBER+1);
          break;
      }

      llc_generate_input (llc_data->current_sapi, frame_type, ns, 
        &cci_decipher_req->ciphering_input, oc);

      cci_decipher_req->direction = CCI_DIRECTION_DOWNLINK;
    }
    else /* ciphering EQ FALSE */
    {
      cci_decipher_req->reference2 = LL_CIPHER_OFF;
      cci_decipher_req->ciphering_algorithm = CCI_CIPHER_NO_ALGORITHM;
    }

    cci_decipher_req->header_size = header_size;

    /*
     * TLLI must be stored somewhere
     */
    cci_decipher_req->reference1 = grlc_unitdata_ind->tlli;

    cci_decipher_req->desc_list = grlc_unitdata_ind->desc_list;

    /* TRACE_EVENT ("CCI thread not available, using functional interface"); */

    rx_decipher_req(cci_decipher_req);
  }

  return;
} /* rx_send_decipher_req() */

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : rx_interpret_frame
+------------------------------------------------------------------------------
| Description : This procedure analyses the LLC header and checksum of the 
|               given frame (U, UI, or I) and sets sapi, pdu_type, command, 
|               cr_bit, pf_bit, nr, and ns accordingly. The FCS field is not 
|               included in frame, it has already been stripped off. frame_ok 
|               is set to TRUE if the frame fulfils the following requirements:
|               known PDU type, valid length, valid PD bit, valid SAPI. 
|               frame_rej indicates a frame rejection condition if any bit of 
|               W4-W1 is set. If a frame is rejected, frame_rej_ctrl_length 
|               indicates the length of the control field. If this length 
|               could not be determined, frame_rej_ctrl_length is set to the 
|               number of control field octets of the frame.
|
| Parameters  : frame     - contains the frame to be analysed, must be 
|                           a valid pointer
|               sapi      - will be set to the SAPI of the frame, must be 
|                           a valid pointer
|               pdu_type  - will be set to the PDU type of the frame, must be 
|                           a valid pointer
|               command   - will be set to the command (I/S, U) of the frame 
|                           (if available), must be a valid pointer
|               cr_bit    - will be set to the C/R bit of the frame, must be 
|                           a valid pointer
|               pf_bit    - will be set to the P/F bit of the frame (if 
|                           available), must be a valid pointer
|               nr        - will be set to N(R) / N(U) of the frame (if 
|                           available), must be a valid pointer
|               ns        - will be set to N(S) of the frame (if available), 
|                           must be a valid pointer
|               frame_ok  - TRUE if the frame is ok, else FALSE, must be 
|                           a valid pointer
|               frame_rej - indicates a frame rejection condition, must be 
|                           a valid pointer
|               frame_rej_ctrl_length - number of octets in the rejected 
|                           control field, must be a valid pointer
|
+------------------------------------------------------------------------------
*/

/*#if defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   !defined(REL99)     || defined(LL_2to1) */

GLOBAL void rx_interpret_frame (T_sdu *frame,
                                UBYTE *sapi, 
                                T_PDU_TYPE *pdu_type, 
                                T_COMMAND *command, 
                                T_BIT *cr_bit, 
                                T_BIT *pf_bit, 
                                T_FRAME_NUM *nr, 
                                T_FRAME_NUM *ns, 
                                BOOL *frame_ok, 
                                UBYTE *frame_rej,
                                USHORT *frame_rej_ctrl_length,
                                UBYTE  cipher)
{
  USHORT            min_length = 0;     /* minimum required frame length  */
  BOOL              check_length;       /* check/ignore length at the end */

  TRACE_FUNCTION( "rx_interpret_frame" );

  /*
   * Preset variables with suspected success, control field length is not
   * yet known, so assume complete frame length.
   */
  *frame_ok               = TRUE;
  *frame_rej              = FRAME_NOT_REJ;
  *frame_rej_ctrl_length  = frame->l_buf/8 - 1;

  /*
   * Frame length is selectively activated, depending on frame type, command,
   * etc. Ignore frame length per default.
   */
  check_length            = FALSE;


  /*
   * Check if the frame contains enough octets to access the first octet of
   * the control field to find out the type of the frame.
   */
  if (frame->l_buf/8 < CTRL_MIN_OCTETS)
  {
    *frame_ok = FALSE;
    return;
  }


  /*
   * Check if PD bit is set to 0. If it is not, the frame is invalid.
   * <R.LLC.XCEPTION.A.001>
   */
  if ((frame->buf[frame->o_buf/8] & 0x80) != 0x00)
  {
    *frame_ok = FALSE;
    return;
  }


  /*
   * Extract C/R bit.
   */
  *cr_bit = (frame->buf[frame->o_buf/8] & 0x40) >> 6;


  /*
   * Extract SAPI and check if the frame contains a reserved SAPI.
   * <R.LLC.XCEPTION.A.001>
   */
  *sapi = frame->buf[frame->o_buf/8] & 0x0F;
  switch (*sapi) /* !!!!! constants or function/macro to determine invalid sapis */
  {
    case 0:
    case 2:
    case 4:
    case 6:
    case 8:
    case 10:
    case 12:
    case 13:
    case 14:
    case 15:
    {
      *frame_ok = FALSE;
      return;
    }
  }


  /*
   * Determine the PDU type of the frame, along with the minimum length
   * required for this PDU type to constitute a complete frame. 
   * Additionally, extract available variables for each PDU type.
   */
  if ((frame->buf[(frame->o_buf/8)+1] & I_FRAME_MASK) EQ I_FRAME_ID)
  {
    /*
     * I frame, at least 5 octets (1 Address, 3 Control, 1(++) 
     * Information, no FCS)
     */
    *pdu_type               = I_FRAME;
    min_length              = I_FRAME_MIN_OCTETS_WITHOUT_FCS;
    *frame_rej_ctrl_length  = I_CTRL_OCTETS;

    /*
     * Extract A bit (stored in P/F bit)
     */
    *pf_bit = (frame->buf[(frame->o_buf/8)+1] & 0x40) >> 6;

    /*
     * Check if the frame contains enough octets to access the complete
     * I frame control field.
     */
    if (frame->l_buf/8 < I_CTRL_MIN_OCTETS)
    {
      *frame_ok = FALSE;
      return;
    }

    /*
     * Extract N(S), N(R)
     */
    *ns = (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+1] & 0x1F) << 4;
    *ns |= (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+2] >> 4);

    *nr = (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+2] & 0x07) << 6;
    *nr |= (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+3] >> 2);

    /*
     * Determine the command of the I frame (S1 & S2 bits)
     */
    switch (frame->buf[(frame->o_buf/8)+3] & 0x03)
    {
      case I_FRAME_RR:
        TRACE_4_PARA("I-RR s:%d len:%d nr:%d ns:%d", *sapi, BYTELEN(frame->l_buf), *nr, *ns);
        *command = I_RR;
        break;

      case I_FRAME_ACK:
        TRACE_4_PARA("I-ACK s:%d len:%d nr:%d ns:%d", *sapi, BYTELEN(frame->l_buf), *nr, *ns);
        *command = I_ACK;
        break;

      case I_FRAME_RNR:
        TRACE_4_PARA("I-RNR s:%d len:%d nr:%d ns:%d", *sapi, BYTELEN(frame->l_buf), *nr, *ns);
        *command = I_RNR;
        break;

      case I_FRAME_SACK:
        TRACE_4_PARA("I-SACK s:%d len:%d nr:%d ns:%d", *sapi, BYTELEN(frame->l_buf), *nr, *ns);
        *command = I_SACK;

        /*
         * Check if the frame contains enough octets to access the complete
         * I frame SACK control field. K is at octet I_CTRL_MIN_OCTETS + 1
         * and there must be at least K+1 octets within the bitmap. Therefore
         * the frame must contain a minimum of I_CTRL_MIN_OCTETS+2.
         */
        if (frame->l_buf/8 < I_CTRL_MIN_OCTETS+2)
        {
          *frame_ok = FALSE;
          return;
        }

        /*
         * min_length is being modified to be more accurate for SACK frames,
         * according to the Bitmap Length Indicator K in 
         * frame->buf[(frame->o_buf/8)+4].
         */
        min_length += (frame->buf[(frame->o_buf/8)+4] & 0x1F) + 1;

        check_length = TRUE;
        break;

      default:
        /*
         * Frame rejection condition due to receipt of a command or
         * response field that is undefined or not implemented (set
         * W3 bit to 1).
         * <R.LLC.XCEPTION.A.002>
         */
        *frame_ok = FALSE;
        *frame_rej = FRAME_REJ_W3;
        return;
    }
  }
  else if ((frame->buf[(frame->o_buf/8)+1] & S_FRAME_MASK) EQ S_FRAME_ID)
  {
    /*
     * S frame, fixed 3 octets (1 Address, 2 Control, 0 Information, no FCS)
     */
    *pdu_type               = S_FRAME;
    min_length              = S_FRAME_MIN_OCTETS_WITHOUT_FCS;
    *frame_rej_ctrl_length  = S_CTRL_OCTETS;

    /*
     * Extract A bit (stored in P/F bit)
     */
    *pf_bit = (frame->buf[(frame->o_buf/8)+1] & 0x20) >> 5;

    /*
     * Check if the frame contains enough octets to access the complete
     * S frame control field.
     */
    if (frame->l_buf/8 < S_CTRL_MIN_OCTETS)
    {
      *frame_ok               = FALSE;
      *frame_rej              = FRAME_REJ_W1;
      *frame_rej_ctrl_length  = frame->l_buf/8 - 1;
      return;
    }

    /*
     * Extract N(R)
     */
    *nr = (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+1] & 0x07) << 6;
    *nr |= (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+2] >> 2);

    /*
     * Determine the command of the S frame (S1 & S2 bits)
     */
    switch (frame->buf[(frame->o_buf/8)+2] & 0x03)
    {
      case I_FRAME_RR:
        TRACE_2_PARA("S-RR s:%d nr:%d", *sapi, *nr);
        *command = I_RR;
        check_length = TRUE;
        break;

      case I_FRAME_ACK:
        TRACE_2_PARA("S-ACK s:%d nr:%d", *sapi, *nr);
        *command = I_ACK;
        check_length = TRUE;
        break;

      case I_FRAME_RNR:
        TRACE_2_PARA("S-RNR s:%d nr:%d", *sapi, *nr);
        *command = I_RNR;
        check_length = TRUE;
        break;

      case I_FRAME_SACK:
        TRACE_2_PARA("S-SACK s:%d nr:%d", *sapi, *nr);
        *command = I_SACK;
        /*
         * min_length is being modified to be more accurate for SACK frames.
         * The S frame SACK format adds a number of up to 32 octets to the
         * control field.
         */
        min_length += S_FRAME_SACK_MIN_CTRL_OCTETS;

        check_length = FALSE;
        break;

      default:
        /*
         * Frame rejection condition due to receipt of a command or
         * response field that is undefined or not implemented (set
         * W3 bit to 1).
         * <R.LLC.XCEPTION.A.002>
         */
        *frame_ok = FALSE;
        *frame_rej = FRAME_REJ_W3;
        return;
    }
  }
  else if ((frame->buf[(frame->o_buf/8)+1] & UI_FRAME_MASK) EQ UI_FRAME_ID)
  {
    /*
     * UI frame, at least 3 octets (1 Address, 2 Control, 0(++)
     * Information, no FCS)
     */
    *pdu_type               = UI_FRAME;
    min_length              = UI_FRAME_MIN_OCTETS_WITHOUT_FCS;
    *frame_rej_ctrl_length  = UI_CTRL_OCTETS;

    /*
     * Check if the frame contains enough octets to access the complete
     * UI frame control field.
     */
    if (frame->l_buf/8 < UI_CTRL_MIN_OCTETS)
    {
      *frame_ok = FALSE;
      return;
    }

    /*
     * Extract N(U) (is stored in N(R))
     */
    *nr = (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+1] & 0x07) << 6;
    *nr |= (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+2] >> 2);

    TRACE_4_PARA("UI s:%d len:%d nr:%d c:%d", *sapi, BYTELEN(frame->l_buf), *nr, cipher);
  }
  else if ((frame->buf[(frame->o_buf/8)+1] & U_FRAME_MASK) EQ U_FRAME_ID)
  {
    /*
     * U frame, at least 2 octets (1 Address, 1 Control, 0(++)
     * Information, no FCS)
     */
    *pdu_type               = U_FRAME;
    min_length              = U_FRAME_MIN_OCTETS_WITHOUT_FCS;
    *frame_rej_ctrl_length  = U_CTRL_OCTETS;

    /*
     * Extract P/F bit
     */
    *pf_bit = (frame->buf[(frame->o_buf/8)+1] & 0x10) >> 4;

    /*
     * Check if the frame contains enough octets to access the complete
     * U frame control field.
     * NOTE:
     * This check could be omitted, because the U frame control field size
     * is only one octet.
     */
    if (frame->l_buf/8 < U_CTRL_MIN_OCTETS)
    {
      *frame_ok = FALSE;
      *frame_rej = FRAME_REJ_W1;
      return;
    }

    /*
     * Determine the command of the U frame (M4, M3, M2, M1).
     * Adjust the minimum length of the frame, if possible.
     */
    switch (frame->buf[(frame->o_buf/8)+1] & 0x0F)
    {
      case U_FRAME_DM:
        TRACE_1_PARA("DM s:%d", *sapi);
        *command = U_DM;
        /*
         * No information field is permitted.
         */
        check_length = TRUE;
        break;
      case U_FRAME_DISC:
        TRACE_1_PARA("DISC s:%d", *sapi);
        *command = U_DISC;
        /*
         * No information field is permitted.
         */
        check_length = TRUE;
        break;
      case U_FRAME_UA:
        TRACE_2_PARA("UA s:%d len:%d", *sapi, BYTELEN(frame->l_buf));
        *command = U_UA;
        /*
         * No knowledge if an information field is permitted, because
         * this is only the case when UA is the response to SABM.
         */
        break;
      case U_FRAME_SABM:
        TRACE_2_PARA("SABM s:%d len:%d", *sapi, BYTELEN(frame->l_buf));
        *command = U_SABM;
        /*
         * An information field is permitted, containing XID 
         * parameters. Therefore the size of the information field
         * is not (yet) known.
         */
        break;
      case U_FRAME_FRMR:
        TRACE_1_PARA("FRMR s:%d", *sapi);
        *command = U_FRMR;
        /*
         * FRMR response contains an information field of 10 octets.
         * <R.LLC.XCEPTION.A.008>
         */
        min_length += U_FRAME_FRMR_INFO_OCTETS;
        check_length = TRUE;
        break;
      case U_FRAME_XID:
        if( *cr_bit == SGSN_COMMAND )
        {
          TRACE_2_PARA("XID REQ s:%d len:%d", *sapi, BYTELEN(frame->l_buf));
        }
        else
        {
          TRACE_2_PARA("XID RSP s:%d len:%d", *sapi, BYTELEN(frame->l_buf));
        }
        *command = U_XID;
        /*
         * An information field is required, containing XID
         * parameters. Therefore the size of the information field
         * is not (yet) known.
         */
        break;
      default:
        TRACE_0_INFO("Not supported U frame received");
        /*
         * Frame rejection condition due to receipt of a command or
         * response field that is undefined or not implemented (set
         * W3 bit to 1).
         * <R.LLC.XCEPTION.A.002>
         */
        *frame_ok = FALSE;
        *frame_rej = FRAME_REJ_W3;
        return;
    }
  }


  /*
   * Determine if it is requested to check the frame length exactly
   * (check_length EQ TRUE), or just to check the minimum frame length
   * (check_length EQ FALSE).
   */
  if (check_length AND (frame->l_buf/8 NEQ min_length))
  {
    /*
     * Actual length of frame doesn't correspond with computed length.
     */
    *frame_ok = FALSE;

    /*
     * Check if frame rejection condition occured: S or U frame with
     * incorrect length (= W1 + W3 bits). frame_rej_ctrl_length has already 
     * been set above to the correct control field length.
     * <R.LLC.XCEPTION.A.002>, <R.LLC.XCEPTION.A.010>
     */
    if ((*pdu_type EQ S_FRAME) OR (*pdu_type EQ U_FRAME))
    {
      *frame_rej = FRAME_REJ_W1 | FRAME_REJ_W3;
    }
    return;
  }
  else if (frame->l_buf/8 < min_length)
  {
    /*
     * Frame doesn't contain enough octets to include the address field, 
     * control field, information field, and FCS field necessary to constitute
     * a complete frame according to the PDU type.
     * <R.LLC.XCEPTION.A.001>
     */
    *frame_ok = FALSE;
    return;
  }

  return;
} /* rx_interpret_frame() */

/* #endif */ /* CF_FAST_EXEC || _SIMULATION_ */


/*
+------------------------------------------------------------------------------
| Function    : rx_strip_llc_header
+------------------------------------------------------------------------------
| Description : This procedure strips the LLC header field off of the sdu 
|               so that the sdu contains only the remaining L3-PDU.
|
| Parameters  : sdu       - contains the SDU (frame), must be a valid pointer
|               pdu_type  - contains the PDU type of the frame, must be 
|                           a valid PDU type
|               command   - contains the command (I/S, U) of the frame, must be
|                           a valid command for the given PDU type
|
+------------------------------------------------------------------------------
*/

#ifndef CF_FAST_EXEC

GLOBAL void rx_strip_llc_header (T_sdu *sdu, 
                                 T_PDU_TYPE pdu_type,
                                 T_COMMAND command)
{ 
  UBYTE header_len = 0;


  TRACE_FUNCTION ("rx_strip_llc_header");
  
  switch (pdu_type)
  {
    case I_FRAME:
      /*
       * I frame 4 octets. Leave SACK Bitmap in PDU, will be stripped later!
       * (1 Address, 3Control).
       */
      header_len = I_CTRL_MIN_OCTETS;
      break;
    case S_FRAME:
      /*
       * S frame 3 octets. Leave SACK Bitmap in PDU, will be stripped later!
       * (1 Address, 2(+32 max) Control).
       */
       header_len = S_CTRL_MIN_OCTETS;
      break;
    case UI_FRAME:
      /*
       * UI frame, 3 octets (1 Address, 2 Control).
       */
      header_len = UI_CTRL_MIN_OCTETS;
      break;
    case U_FRAME:
      /*
       * U frame, 2 octets (1 Address, 1 Control).
       */
      header_len = U_CTRL_MIN_OCTETS;
      break;
    default:
      TRACE_ERROR ("Unknown PDU type");
      break;
  }

  /*
   * Adjust the beginning of the PDU using the determined header_len.
   */
  sdu->o_buf += header_len * 8;

  /*
   * Adjust the length of the PDU by the size of the header field.
   */
  sdu->l_buf -= header_len * 8;

  return;
} /* rx_strip_llc_header() */

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : rx_get_desc_octet
+------------------------------------------------------------------------------
| Description : This procedure maps the given linear offset to a descriptor
|               list and returns the value of the octet at this offset. If
|               offset is too large for descriptor list, 0x00 is returned.
|               Each descriptor in descriptor list is evaluated until the 
|               requested offset is reached. The resulting octet value is 
|               written to the referenced parameter data_ptr (if not set to 
|               NULL), and is additionally returned.
|
| Parameters  : desc_list - a valid pointer to a valid descriptor list
|               offset    - linear octet offset, beginning with 0
|               data_ptr  - a valid pointer to be set to the resulting octet
|                           value, or NULL
|
+------------------------------------------------------------------------------
*/

#ifndef CF_FAST_EXEC

GLOBAL UBYTE rx_get_desc_octet (T_desc_list *desc_list, 
                               USHORT offset, 
                               UBYTE *data_ptr)
{
  T_desc            *desc;
  UBYTE             *desc_data;         /* local pointer to data octet */

  /*
   * Check if offset is contained in descriptor list. Return 0x00 if not.
   */
  if (offset >= desc_list->list_len)
  {
    TRACE_ERROR ("offset too large for descriptor list");
    return 0x00;
  }

  /*
   * Search requested data with given linear offset in descriptor list.
   * Empty descriptors are skipped. Check for each descriptor if requested
   * offset is in descriptor, until it is found.
   */
  desc = (T_desc *)desc_list->first;
  desc_data = NULL;
  do
  {
    if (offset < desc->len)
    {
      /*
       * Requested data is in current descriptor. Set desc_data to point to 
       * requested octet with remaining offset (has been decremented by
       * already passed descriptor payload).
       */
      /*lint -e662 Possible creation of out-of-bounds pointer */
      desc_data = &(desc->buffer[offset]);
    }
    else
    {
      /*
       * Requested data is not in current descriptor. Remember data payload
       * of current descriptor as already passed.
       */
      offset -= desc->len;
    }

    desc = (T_desc *)desc->next;
  }
  while ((desc NEQ NULL) AND (desc_data EQ NULL));

  
  if (desc_data EQ NULL)
  {
    TRACE_ERROR ("descriptor list ended before offset found");

    if (data_ptr NEQ NULL)
    {
      *data_ptr = 0;
    }

    return 0;
  }
  else
  {
    /*
     * Store value of found octet in data_ptr, if pointer is valid.
     */
    if (data_ptr NEQ NULL)
    {
      /*lint -e661 Possible access of out-of-bounds pointer */
      *data_ptr = *desc_data;
    }

    return *desc_data;
  }
} /* rx_get_desc_octet() */

#endif /* CF_FAST_EXEC */
