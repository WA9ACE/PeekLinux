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
|             SDL-documentation (U-statemachine)
+-----------------------------------------------------------------------------
*/


#ifndef LLC_UF_C
#define LLC_UF_C
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

#include "llc_f.h"      /* to get global functions, e.g. llc_init_parameters */
#include "llc_uf.h"     /* to get local XID definitions */
#include "llc_t200s.h"  /* to get signal interface to T200 */
#include "llc_txs.h"    /* to get signal interface to TX */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
 * CHECK_INSTANCE:
 * v(ariable)   - variable name of parameter
 */
#define CHECK_INSTANCE(v)                                                     \
  if (llc_data->decoded_xid.##v##.valid)                                      \
  {                                                                           \
    /*                                                                        \
     * More than one instance of the same XID parameter type is included.     \
     */                                                                       \
    if (cr_bit EQ SGSN_COMMAND)                                               \
    {                                                                         \
      /*                                                                      \
       * XID command: Ignore all instances except the first.                  \
       * <R.LLC.XIDNEG_R.A.014>                                               \
       */                                                                     \
      continue;                                                               \
    }                                                                         \
    else /* SGSN_RESPONSE */                                                  \
    {                                                                         \
      /*                                                                      \
       * XID response: The XID information field is regarded as invalid.      \
       * <R.LLC.XID_INVA.A.012>                                               \
       */                                                                     \
      TRACE_0_INFO ("More than one instance of an XID-IE in RSP");            \
      return FALSE;                                                           \
    }                                                                         \
  }


/*
 * CHECK_LENGTH_VALUE:
 * l(ength)     - default length of parameter
 * c(ondition)  - out-of-range value condition
 * v(ariable)   - variable name of parameter
 * d(default)   - default value of parameter
 */
#define CHECK_LENGTH_VALUE(l,c,v,d)                                           \
  if ((length NEQ l) OR (c))                                                  \
  {                                                                           \
    /*                                                                        \
     * Unsupported length or out-of-range values.                             \
     */                                                                       \
    if (cr_bit EQ SGSN_COMMAND)                                               \
    {                                                                         \
      /*                                                                      \
       * XID command: Set the value for this type according to our            \
       * preferences.                                                         \
       * <R.LLC.XIDNEG_R.A.003>                                               \
       */                                                                     \
      llc_data->decoded_xid.##v##.valid = TRUE;                               \
      llc_data->decoded_xid.##v##.value = d;                                  \
      break;                                                                  \
    }                                                                         \
    else /* SGSN_RESPONSE */                                                  \
    {                                                                         \
      /*                                                                      \
       * XID response: The XID information field is regarded as invalid.      \
       * <R.LLC.XID_INVA.A.007>, <R.LLC.XID_INVA.A.009>                       \
       */                                                                     \
      TRACE_0_INFO ("Unsupported length or out-of-range values in RSP");      \
      return FALSE;                                                           \
    }                                                                         \
  }


/*
 * CHECK_SENSE_OF_NEGOTIATION:
 * d(ata)       - received data value to check
 * v(ariable)   - variable name of parameter
 * s(ense)      - sense of negotiation (XID_SENSE_UP / XID_SENSE_DOWN)
 */
#define CHECK_SENSE_OF_NEGOTIATION(d,v,s)                                     \
  {                                                                           \
    /*                                                                        \
     * XID response must not contain an XID parameter with a value that       \
     * violates the sense of negotiation.                                     \
     * <R.LLC.XID_INVA.A.008>                                                 \
     */                                                                       \
    if (cr_bit EQ SGSN_RESPONSE)                                              \
    {                                                                         \
      /*                                                                      \
       * First compare the value with the requested value, if there is any    \
       */                                                                     \
      if ( (llc_data->u->requested_xid.##v##.valid)    AND                    \
           (llc_data->u->requested_xid.##v##.value s d)  )                    \
      {                                                                       \
        TRACE_0_INFO ("Sense of negotiation error");                          \
        return FALSE;                                                         \
      }                                                                       \
      else if( !(llc_data->u->requested_xid.##v##.valid) AND                  \
                (*(llc_data->##v) s d) )                                      \
      {                                                                       \
        /*                                                                    \
         * If the value was not requested but included in response, compare   \
         * it with the current value                                          \
         */                                                                   \
        TRACE_0_INFO ("Value not requested, but sense of negotiation error"); \
        return FALSE;                                                         \
      }                                                                       \
    }                                                                         \
  }


/*
+------------------------------------------------------------------------------
| Function    : u_init
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of u_frames
|               for all SAPIs.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_init (void)
{
  TRACE_FUNCTION( "u_init" );

  /*
   * Initialise every incarnation of U with state TLLI_UNASSIGNED.
   */
  SWITCH_SERVICE (llc, u, 0);
  INIT_STATE (U_0, U_TLLI_UNASSIGNED);

  SWITCH_SERVICE (llc, u, 1);
  INIT_STATE (U_1, U_TLLI_UNASSIGNED);

  SWITCH_SERVICE (llc, u, 2);
  INIT_STATE (U_2, U_TLLI_UNASSIGNED);

  SWITCH_SERVICE (llc, u, 3);
  INIT_STATE (U_3, U_TLLI_UNASSIGNED);

  SWITCH_SERVICE (llc, u, 4);
  INIT_STATE (U_4, U_TLLI_UNASSIGNED);

  SWITCH_SERVICE (llc, u, 5);
  INIT_STATE (U_5, U_TLLI_UNASSIGNED);

  return;
} /* u_init() */



/*
+------------------------------------------------------------------------------
| Function    : u_init_sapi
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of u_frames
|               for the given SAPI after a TLLI assignment of LLC was occurred.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_init_sapi (void)
{
  TRACE_FUNCTION( "u_init_sapi" );

  /*
   * Initial, no XID command is currently being sent.
   */
  llc_data->u->xid_pending                 = FALSE;

  llc_data->u->xid_tag                     = 0L;
  llc_data->u->xid_tag_negotiate           = 0L;
  llc_data->u->xid_tag_sent                = 0L;

  llc_data->u->release_requested           = FALSE;
  llc_data->u->ll_xid_resp_pending         = FALSE;

  return;
} /* u_init_sapi() */

/*
+------------------------------------------------------------------------------
| Function    : u_build_u_frame
+------------------------------------------------------------------------------
| Description : This procedure fills the given sdu with a correct LLC frame,
|               containing the address field and the control field (U format).
|               The information field is not filled in by this procedure.
|               The checksum field (FCS) is also not filled in by this
|               procedure, it is filled in by the send_pdu service before
|               sending.
|
| Parameters  : sdu - a valid pointer to an SDU, containing enough octets for
|                     the requested command/response
|               cr_bit - C/R bit for the frame
|               sapi - a valid SAPI for unacknowledged operation
|               pf_bit - P/F bit for the U frame
|               command - a valid command/response for the U frame
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_build_u_frame
(
#ifdef LL_DESC
  T_desc_list3 *desc_list3,
#else
  T_sdu *sdu,
#endif
  T_BIT cr_bit,
  UBYTE sapi,
  T_BIT pf_bit,
  T_COMMAND command
)
{
#ifdef LL_DESC
  T_desc3 *desc3;
  UBYTE   *desc_buf;
#endif

  TRACE_FUNCTION( "u_build_u_frame" );

#ifdef LL_DESC
  /*
   * Let every frame begin at the beginning.
   */
  desc3 = (T_desc3*)desc_list3->first;
  desc3->offset = 0;
  desc_buf = (UBYTE*)desc3->buffer;

  /*
   * Fill in address and control fields.
   * NOTE: The values of the commands are chosen to correspond with the
   * defined values of the Mx bits in the control field. Therefore the given
   * command can be used to build the header, regardless of its actual value.
   */
  desc_buf[0] = ((UBYTE)cr_bit << 6) | sapi;
  desc_buf[1] = 0xE0 | ((UBYTE)pf_bit << 4) | (UBYTE)command;


  /*
   * Determine the length of the frame, depending on the command/
   * response.
   */
  switch (command)
  {
#ifdef REL99 
    case U_NULL:
      /*
       * The NULL command contains no information field. The size
       * of the frame is fixed. (FCS ist not yet considered in l_buf, will
       * be included when FCS is appended to the frame.)
       */
      desc3->len = U_NULL_SIZE_BITS/8;
      break;
#endif /* REL99 */
    case U_DM:
      /*
       * The DM response contains no information field. The size
       * of the frame is fixed. (FCS ist not yet considered in l_buf, will
       * be included when FCS is appended to the frame.)
       */
      desc3->len = U_DM_SIZE_BITS/8;
      break;
    case U_DISC:
      /*
       * The DISC response contains no information field. The size
       * of the frame is fixed. (FCS ist not yet considered in l_buf, will
       * be included when FCS is appended to the frame.)
       */
      desc3->len = U_DISC_SIZE_BITS/8;
      break;
    case U_UA:
      /*
       * The UA response may contain an information field. Set only the
       * size of the header (address + control fields).
       */
      desc3->len = 2;
      break;
    case U_SABM:
      /*
       * The SABM command may contain an information field. Set only the
       * size of the header (address + control fields).
       */
      desc3->len = 2;
      break;
    case U_FRMR:
      /*
       * The FRMR response contains an information field of fixed size.
       * u_insert_frmr_information() relies on a sane value in l_buf,
       * therefore set only the size of the header (address + control field).
       */
      desc3->len = 2;
      break;
    case U_XID:
      /*
       * The XID command/response contains an information field. Set only the
       * size of the header (address + control fields).
       */
      desc3->len = 2;
      break;
    default:
      desc3->len = 0;
      TRACE_ERROR ("Undefined Command specified");
      break;
  }

  desc_list3->list_len = desc3->len;

  return;

#else
  /*
   * Let every frame begin at the beginning.
   */
  sdu->o_buf = 0;

  /*
   * Fill in address and control fields.
   * NOTE: The values of the commands are chosen to correspond with the
   * defined values of the Mx bits in the control field. Therefore the given
   * command can be used to build the header, regardless of its actual value.
   */
  sdu->buf[0] = ((UBYTE)cr_bit << 6) | sapi;
  sdu->buf[1] = 0xE0 | ((UBYTE)pf_bit << 4) | (UBYTE)command;


  /*
   * Determine the length of the frame, depending on the command/
   * response.
   */
  switch (command)
  {
#ifdef REL99 
   case U_NULL:
      /*
       * The NULL command contains no information field. The size
       * of the frame is fixed. (FCS ist not yet considered in l_buf, will
       * be included when FCS is appended to the frame.)
       */
      sdu->l_buf = U_NULL_SIZE_BITS;
      break;
#endif /* REL99 */

    case U_DM:
      /*
       * The DM response contains no information field. The size
       * of the frame is fixed. (FCS ist not yet considered in l_buf, will
       * be included when FCS is appended to the frame.)
       */
      sdu->l_buf = U_DM_SIZE_BITS;
      break;
    case U_DISC:
      /*
       * The DISC response contains no information field. The size
       * of the frame is fixed. (FCS ist not yet considered in l_buf, will
       * be included when FCS is appended to the frame.)
       */
      sdu->l_buf = U_DISC_SIZE_BITS;
      break;
    case U_UA:
      /*
       * The UA response may contain an information field. Set only the
       * size of the header (address + control fields).
       */
      sdu->l_buf = 2*8;
      break;
    case U_SABM:
      /*
       * The SABM command may contain an information field. Set only the
       * size of the header (address + control fields).
       */
      sdu->l_buf = 2*8;
      break;
    case U_FRMR:
      /*
       * The FRMR response contains an information field of fixed size.
       * u_insert_frmr_information() relies on a sane value in l_buf,
       * therefore set only the size of the header (address + control field).
       */
      sdu->l_buf = 2*8;
      break;
    case U_XID:
      /*
       * The XID command/response contains an information field. Set only the
       * size of the header (address + control fields).
       */
      sdu->l_buf = 2*8;
      break;
    default:
      sdu->l_buf = 0;
      TRACE_ERROR ("Undefined Command specified");
      break;
  }

  return;
#endif /* LL_DESC */
} /* u_build_u_frame() */



/*
+------------------------------------------------------------------------------
| Function    : u_check_xid
+------------------------------------------------------------------------------
| Description : This procedure checks the XID information field that is given
|               in SDU. If it is invalid, rc_xid_valid is set to FALSE,
|               otherwise to TRUE. All XID parameters are decoded and stored
|               in llc_decoded_xid (global variable) if they are valid. The
|               parameters cr_bit and command are necessary to distinguish
|               between the different commands and responses. Each parameter
|               is checked for multiple instances, correct sense of
|               negotiation, and valid field length and value.
|
| Parameters  : sdu - a valid pointer to an SDU
|               cr_bit - C/R bit of the frame
|               command - command/response of the U frame
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL u_check_xid (T_sdu *sdu,
                         T_BIT cr_bit,
                         T_COMMAND command)
{
  UBYTE             *par;
  BOOL              xl;
  UBYTE             type;
  UBYTE             length = 0;
  UBYTE             *data;
  ULONG             value;


  TRACE_FUNCTION( "u_check_xid" );

  /*
   * Initialise each valid flag to FALSE.
   */
  llc_data->decoded_xid.version.valid   = FALSE;
  llc_data->decoded_xid.iov_ui.valid    = FALSE;
  llc_data->decoded_xid.iov_i.valid     = FALSE;
  llc_data->decoded_xid.t200.valid      = FALSE;
  llc_data->decoded_xid.n200.valid      = FALSE;
  llc_data->decoded_xid.n201_u.valid    = FALSE;
  llc_data->decoded_xid.n201_i.valid    = FALSE;
  llc_data->decoded_xid.md.valid        = FALSE;
  llc_data->decoded_xid.mu.valid        = FALSE;
  llc_data->decoded_xid.kd.valid        = FALSE;
  llc_data->decoded_xid.ku.valid        = FALSE;
  llc_data->decoded_xid.reset.valid     = FALSE;

  llc_data->decoded_l3_xid.valid        = FALSE;

  /*
   * Initialise par with the first XID parameter.
   * Process each XID parameter until the end of the list is reached:
   * - Check if end of list is reached (max. SDU length).
   * ...process XID parameter (set xl and length)...
   * - Increment par to point to the next XID parameter (is incremented by
   *   (data) length plus header length).
   */
#ifdef TRACE_EVE
  {
    int i;
    int l = sdu->l_buf/8;
    int o = sdu->o_buf/8;

    vsi_o_ttrace(VSI_CALLER TC_EVENT, "XID sdu dump len:%d bytes", l);

    for (i=0; i<l; i+=10)
    {
      vsi_o_ttrace(VSI_CALLER TC_EVENT, "%.2x %.2x %.2x %.2x %.2x   %.2x %.2x %.2x %.2x %.2x",
                                        sdu->buf[o+i+0], sdu->buf[o+i+1],
                                        sdu->buf[o+i+2], sdu->buf[o+i+3], sdu->buf[o+i+4],

                                        sdu->buf[o+i+5], sdu->buf[o+i+6],
                                        sdu->buf[o+i+7], sdu->buf[o+i+8], sdu->buf[o+i+9]);
    }
  }
#endif

  for (par = &(sdu->buf[sdu->o_buf/8]);
    par < (&(sdu->buf[sdu->o_buf/8]) + sdu->l_buf/8);
    par += length + (xl ? 2 : 1))
  {
    /*
     * Store XL bit in boolean xl.
     */
    xl = (*par & 0x80) > 0;

    /*
     * Store (data) length, and set data to point to the beginning
     * of the XID parameter data.
     */
    if (xl)
    {
      /*
       * XL == 1: length is given in 8 bits.
       */
      length = (*par & 0x03) << 6;
      length += (*(par+1) & 0xFC) >> 2;

      data = par + 2;
    }
    else
    {
      /*
       * XL == 0: length is given in 2 bits.
       */
      length = *par & 0x03;

      data = par + 1;
    }


    if (data + length > &(sdu->buf[sdu->o_buf/8]) + sdu->l_buf/8)
    {
      /*
       * The XID parameter field violates the LLC frame format (exceeds given
       * SDU size).
       * <R.LLC.XID_INVA.A.001>
       */
      return FALSE;
    }


    /*
     * Store type field.
     */
    type = (*par & 0x7C) >> 2;


    /*
     * Check type and store parameter data, if any.
     */
    switch (type)
    {
      case XID_VERSION:
        CHECK_INSTANCE (version);

        /*
         * XID response must not contain an XID parameter with a value that
         * violates the sense of negotiation.
         * <R.LLC.XID_INVA.A.008>
         */
        if (cr_bit EQ SGSN_RESPONSE)
        {
          /*
           * First compare the value with the requested value, if there is any
           */
          if ( (llc_data->u->requested_xid.version.valid)    AND
               (llc_data->u->requested_xid.version.value XID_SENSE_DOWN (*data & 0x0F))  )
          {
            return FALSE;
          }
          else if( !(llc_data->u->requested_xid.version.valid) AND
                    llc_data->version XID_SENSE_DOWN (*data & 0x0F) )
          {
            /*
             * If the value was not requested but included in response, compare
             * it with the current value
             */
            return FALSE;
          }
        }
        /*lint -e685 Relational operator always evaluates to false*/
        CHECK_LENGTH_VALUE (XID_VERSION_LEN,
          ((*data & 0x0F) > XID_VERSION_MAX),
          version, llc_data->version);
        /*lint +e685 Relational operator always evaluates to false*/
        /*
         * Set version as valid and store value.
         */
        llc_data->decoded_xid.version.valid = TRUE;
        llc_data->decoded_xid.version.value = *data & 0x0F;
        TRACE_1_INFO("XID Version:%d", llc_data->decoded_xid.version.value);
        break;
      case XID_IOV_UI:
        CHECK_INSTANCE (iov_ui);

        value = ((ULONG)*data) << 24;
        value += ((ULONG)*(data+1)) << 16;
        value += ((ULONG)*(data+2)) << 8;
        value += *(data+3);
        /*lint -e685 Relational operator always evaluates to false*/
        CHECK_LENGTH_VALUE (XID_IOV_UI_LEN,
          (value > XID_IOV_UI_MAX),
          iov_ui, llc_data->iov_ui);
        /*lint +e685 Relational operator always evaluates to false*/
        /*
         * Set iov_ui as valid and store value.
         */
        llc_data->decoded_xid.iov_ui.valid = TRUE;
        llc_data->decoded_xid.iov_ui.value = value;
        TRACE_1_INFO("XID IOV-UI:0x%.8X", value);
        break;
      case XID_IOV_I:
        if (command EQ U_XID)
        {
          /*
           * IOV-I must not be contained in an XID command/response.
           * <R.LLC.XID_INVA.A.010>, <R.LLC.XID_PAR.A.003>
           */
          TRACE_0_INFO("IOV-I must not be contained in an XID command/response");
          return FALSE;
        }

        CHECK_INSTANCE (iov_i);

        value = ((ULONG)*data) << 24;
        value += ((ULONG)*(data+1)) << 16;
        value += ((ULONG)*(data+2)) << 8;
        value += *(data+3);
        /*lint -e685 Relational operator always evaluates to false*/
        CHECK_LENGTH_VALUE (XID_IOV_I_LEN,
          (value > XID_IOV_I_MAX),
          iov_i, *(llc_data->iov_i));
        /*lint +e685 Relational operator always evaluates to false*/
        /*
         * Set iov_i as valid and store value.
         */
        llc_data->decoded_xid.iov_i.valid = TRUE;
        llc_data->decoded_xid.iov_i.value = value;
        TRACE_1_INFO("XID IOV-I:0x%.8X", value);
        break;
      case XID_T200:
        CHECK_INSTANCE (t200);

        value = ((ULONG)*data) << 8;
        value += *(data+1);
        value &= 0x00000FFFL;

        /*
         * XID response must not contain an XID parameter with a value that
         * violates the sense of negotiation.
         * <R.LLC.XID_INVA.A.008>
         */
        if (cr_bit EQ SGSN_RESPONSE)
        {
          /*
           * First compare the value with the requested value, if there is any
           */
          if ( (llc_data->u->requested_xid.t200.valid)    AND
               (llc_data->u->requested_xid.t200.value XID_SENSE_UP value)  )
          {
            TRACE_1_INFO("Value of T200:%d illegal", value);
            return FALSE;
          }
          else if( !(llc_data->u->requested_xid.t200.valid) AND
                    (USHORT)INT2XID(llc_data->t200->length) XID_SENSE_UP value )
          {
            /*
             * If the value was not requested but included in response, compare
             * it with the current value
             */
            TRACE_1_INFO("Requested value of T200:%d illegal", value);
            return FALSE;
          }
        }

        CHECK_LENGTH_VALUE (XID_T200_LEN,
          ((value < XID_T200_MIN) OR (value > XID_T200_MAX)),
          t200, (USHORT)INT2XID(llc_data->t200->length));

        /*
         * Set t200 as valid and store value.
         */
        llc_data->decoded_xid.t200.valid = TRUE;
        llc_data->decoded_xid.t200.value = (USHORT)value;
        TRACE_1_INFO("XID T200:%d", llc_data->decoded_xid.t200.value);
        break;
      case XID_N200:
        CHECK_INSTANCE (n200);

        CHECK_SENSE_OF_NEGOTIATION ((*data & 0x0F), n200, XID_SENSE_UP);
         /*lint -e685 Relational operator always evaluates to false*/
        CHECK_LENGTH_VALUE (XID_N200_LEN,
          (((*data & 0x0F) < XID_N200_MIN) OR ((*data & 0x0F) > XID_N200_MAX)),
          n200, *(llc_data->n200));
        /*lint +e685 Relational operator always evaluates to false*/
        /*
         * Set n200 as valid and store value.
         */
        llc_data->decoded_xid.n200.valid = TRUE;
        llc_data->decoded_xid.n200.value = *data & 0x0F;
        TRACE_1_INFO("XID N200:%d", llc_data->decoded_xid.n200.value);
        break;
      case XID_N201_U:
        CHECK_INSTANCE (n201_u);

        value = ((ULONG)*data) << 8;
        value += *(data+1);
        value &= 0x000007FFL;

        CHECK_SENSE_OF_NEGOTIATION ((USHORT)value, n201_u, XID_SENSE_DOWN);

        CHECK_LENGTH_VALUE (XID_N201_U_LEN,
          (((USHORT)value < XID_N201_U_MIN) OR (value > XID_N201_U_MAX)),
          n201_u, *(llc_data->n201_u));

        /*
         * Set n201_u as valid and store value.
         */
        llc_data->decoded_xid.n201_u.valid = TRUE;
        llc_data->decoded_xid.n201_u.value = (USHORT)value;
        TRACE_1_INFO("XID N201-U:%d", llc_data->decoded_xid.n201_u.value);
        break;
      case XID_N201_I:
        CHECK_INSTANCE (n201_i);

        value = ((ULONG)*data) << 8;
        value += *(data+1);
        value &= 0x000007FFL;

        CHECK_SENSE_OF_NEGOTIATION ((USHORT)value, n201_i, XID_SENSE_DOWN);

        CHECK_LENGTH_VALUE (XID_N201_I_LEN,
          ((value < XID_N201_I_MIN) OR (value > XID_N201_I_MAX)),
          n201_i, *(llc_data->n201_i));

        /*
         * Set n201_i as valid and store value.
         */
        llc_data->decoded_xid.n201_i.valid = TRUE;
        llc_data->decoded_xid.n201_i.value = (USHORT)value;
        TRACE_1_INFO("XID N201-I:%d", llc_data->decoded_xid.n201_i.value);
        break;
      case XID_MD:
        CHECK_INSTANCE (md);

        value = ((ULONG)*data) << 8;
        value += *(data+1);
        value &= 0x00007FFFL;

        CHECK_SENSE_OF_NEGOTIATION ((USHORT)value, md, XID_SENSE_DOWN);

        CHECK_LENGTH_VALUE (XID_MD_LEN,
          ((value != XID_MD_OFF) AND ((value < XID_MD_MIN) OR (value > XID_MD_MAX))),
          md, *(llc_data->md));

        /*
         * Set md as valid and store value.
         */
        llc_data->decoded_xid.md.valid = TRUE;
        llc_data->decoded_xid.md.value = (USHORT)value;
        TRACE_1_INFO("XID MD:%d", llc_data->decoded_xid.md.value);
        break;
      case XID_MU:
        CHECK_INSTANCE (mu);

        value = ((ULONG)*data) << 8;
        value += *(data+1);
        value &= 0x00007FFFL;

        CHECK_SENSE_OF_NEGOTIATION ((USHORT)value, mu, XID_SENSE_DOWN);

        CHECK_LENGTH_VALUE (XID_MU_LEN,
          ((value != XID_MU_OFF) AND ((value < XID_MU_MIN) OR (value > XID_MU_MAX))),
          mu, *(llc_data->mu));

        /*
         * Set mu as valid and store value.
         */
        llc_data->decoded_xid.mu.valid = TRUE;
        llc_data->decoded_xid.mu.value = (USHORT)value;
        TRACE_1_INFO("XID MU:%d", llc_data->decoded_xid.mu.value);
        break;
      case XID_KD:
        CHECK_INSTANCE (kd);

        CHECK_SENSE_OF_NEGOTIATION (*data, kd, XID_SENSE_DOWN);
        /*lint -e685 Relational operator always evaluates to false*/
        CHECK_LENGTH_VALUE (XID_KD_LEN,
          ((*data < XID_KD_MIN) OR (*data > XID_KD_MAX)),
          kd, *(llc_data->kd));
        /*lint +e685 Relational operator always evaluates to false*/
        /*
         * Set kd as valid and store value.
         */
        llc_data->decoded_xid.kd.valid = TRUE;
        llc_data->decoded_xid.kd.value = *data;
        TRACE_1_INFO("XID KD:%d", llc_data->decoded_xid.kd.value);
        break;
      case XID_KU:
        CHECK_INSTANCE (ku);

        CHECK_SENSE_OF_NEGOTIATION (*data, ku, XID_SENSE_DOWN);
        /*lint -e685 Relational operator always evaluates to false*/
        CHECK_LENGTH_VALUE (XID_KU_LEN,
          ((*data < XID_KU_MIN) OR (*data > XID_KU_MAX)),
          ku, *(llc_data->ku));
        /*lint +e685 Relational operator always evaluates to false*/
        /*
         * Set ku as valid and store value.
         */
        llc_data->decoded_xid.ku.valid = TRUE;
        llc_data->decoded_xid.ku.value = *data;
        TRACE_1_INFO("XID KU:%d", llc_data->decoded_xid.ku.value);
        break;
      case XID_LAYER_3:
        if (llc_data->decoded_l3_xid.valid)
        {
          /*
           * More than one instance of the same XID parameter type is included.
           */
          if (cr_bit EQ SGSN_COMMAND)
          {
            /*
             * XID command: Ignore all instances except the first.
             * <R.LLC.XIDNEG_R.A.014>
             */
            continue;
          }
          else /* SGSN_RESPONSE */
          {
            /*
             * XID response: The XID information field is regarded as invalid.
             * <R.LLC.XID_INVA.A.012>
             */
            TRACE_0_INFO("More than one L3 parameters are included in XID response.");
            return FALSE;
          }
        }

        if ((llc_data->current_sapi NEQ LL_SAPI_3) AND
            (llc_data->current_sapi NEQ LL_SAPI_5) AND
            (llc_data->current_sapi NEQ LL_SAPI_9) AND
            (llc_data->current_sapi NEQ LL_SAPI_11))
        {
          /*
           * Layer-3 parameters on a SAPI different from 3, 5, 9, and 11.
           * <R.LLC.XID_INVA.A.011>
           */
          TRACE_0_INFO("L3 parameters on a SAPI different from 3, 5, 9, and 11.");
          return FALSE;
        }

        /*
         * Set layer_3 as valid and store Layer-3 XID parameters.
         */
        llc_data->decoded_l3_xid.valid = TRUE;
        llc_data->decoded_l3_xid.length = length;
        memcpy (llc_data->decoded_l3_xid.value, data, length);
        TRACE_1_INFO("XID L3:%d bytes", length);
        break;
      case XID_RESET:
        if (par NEQ &(sdu->buf[sdu->o_buf/8]))
        {
          /*
           * Reset must be the first parameter in the XID information field.
           * <R.LLC.XID_INVA.A.004>
           */
          TRACE_0_INFO("Reset must be the first XID parameter.");
          return FALSE;
        }

        if (cr_bit EQ SGSN_COMMAND)
        {
          if (command EQ U_SABM)
          {
            /*
             * SABM command: Reset is not allowed.
             * <R.LLC.XID_INVA.A.003>
             */
            TRACE_0_INFO("Reset is not allowed in SABM command");
            return FALSE;
          }
        }
        else /* SGSN_RESPONSE */
        {
          /*
           * UA/XID response: Reset is not allowed.
           * <R.LLC.XID_INVA.A.005>
           */
          TRACE_0_INFO("Reset is not allowed in UA/XID response");
          return FALSE;
        }

        /*
         * Set reset as valid.
         */
        llc_data->decoded_xid.reset.valid = TRUE;
        TRACE_0_INFO("XID Reset");
        break;
      default:
        /*
         * Unrecognised type field.
         */
        if (cr_bit EQ SGSN_COMMAND)
        {
          /*
           * XID command: Skip XID parameter.
           * <R.LLC.XIDNEG_R.A.002>
           */
          continue;
        }
        else /* SGSN_RESPONSE */
        {
          /*
           * XID response: The XID information field is regarded as invalid.
           * <R.LLC.XID_INVA.A.006>
           */
          TRACE_0_INFO("Unrecognised type field in XID response.");
          return FALSE;
        }
    }
  }

  /*
   * All XID parameters have been processed, XID information field seems
   * to be correct.
   */
  return TRUE;
} /* u_check_xid() */



/*
+------------------------------------------------------------------------------
| Function    : u_eval_xid
+------------------------------------------------------------------------------
| Description : This procedure evaluates the decoded XID information field in
|               llc_decoded_xid (global variable). If Reset is present, the
|               state of every incarnation in ABM is set to ADM (including
|               the current incarnation!), all LLC layer parameters are set
|               to their default values, the OCs for unack. transfer are set
|               to 0, and the parameter reset_received is set to TRUE. If a
|               command has been received (cr_bit is set to SGSN_COMMAND),
|               all XID parameters are evaluated and stored as global values,
|               if acceptable. Each XID parameter is tagged in llc_xid_tag
|               for the next response. Otherwise (a response has been
|               received), store the received parameters as global values
|               and reset their valid flags in llc_requested_xid. In each
|               case, if N201-I or N201-U have been changed, or if Layer-3
|               XID parameters have been received, set xid_ind to TRUE,
|               otherwise to FALSE. Additionally remove any received parameter
|               in llc_xid_negotiate, because it has been explicitly
|               negotiated by the peer.
|
| Parameters  : cr_bit          - SGSN_COMMAND/_RESPONSE has been received
|               reset_received  - TRUE indicates received Reset parameter,
|                                 else FALSE
|               xid_ind         - TRUE indicates changed N201-U and/or N201-I,
|                                 so that LL_XID_IND must be sent to layer 3
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_eval_xid (T_BIT cr_bit,
                        BOOL *reset_received,
                        BOOL *xid_ind)
{
  UBYTE             incarnation;


  TRACE_FUNCTION ("u_eval_xid");

  /*
   * Preset both indicators with default values.
   */
  *reset_received = FALSE;
  *xid_ind = FALSE;

  /*
   * Preset global XID indicator for next response.
   */
  llc_data->u->xid_tag = 0L;


  /**************************************************************************
   * Reset:
   * Has to be processed first in order to take effect!
   */
  if (llc_data->decoded_xid.reset.valid)
  {
    /*
     * Initialise all LLC layer parameters for all SAPIs.
     */
    llc_init_parameters();
    llc_init_requested_xid();

    /*
     * Reset all incarnations of service U that are in state 'ABM' to 'ADM'.
     */
    for (incarnation = 0; incarnation < U_NUM_INC; incarnation++)
    {
      llc_data->u_base[incarnation].state = U_ADM;
    }

    /*
     * LLC has to be reset. Send SIG_U_LLME_RESET_IND.
     */
    *reset_received = TRUE;
  }


  /**************************************************************************
   * Version:
   * Shall not be negotiated while in state 'ABM'.
   * <R.LLC.XID_PAR.A.001>
   */
  if ((llc_data->decoded_xid.version.valid) AND (GET_STATE(U) NEQ U_ABM))
  {
    if (cr_bit EQ SGSN_COMMAND)
    {
      if (llc_xid_value_acceptable (llc_data->current_sapi, XID_VERSION,
        llc_data->decoded_xid.version.value))
      {
        llc_data->version = llc_data->decoded_xid.version.value;
      }
      else /* value is not acceptable */
      {
        /*
         * Sense of negotiation down: offer lower value; sense of
         * negotiation up: offer higher value.
         * This happens automatically by just responding with our current
         * global value.
         */
      }

      /*
       * Tag parameter for next response.
       */
      llc_data->u->xid_tag |= 0x00000001L << XID_VERSION;
    }
    else /* SGSN_RESPONSE */
    {
      /*
       * Sense of negotiation-correctness of response has already been checked
       * in u_check_xid().
       */
      llc_data->version = llc_data->decoded_xid.version.value;
    }

    /*
     * Remove tag in xid_tag_negotiate, because this parameter has been
     * explicitly negotiated by the peer.
     * <R.LLC.XIDNEG_R.A.015>
     */
    llc_data->u->xid_tag_negotiate &= ~(0x00000001L << XID_VERSION);
  }


  /**************************************************************************
   * IOV-UI:
   * Shall only be negotiated in state 'ADM'. <R.LLC.XID_PAR.A.002>
   */
  if ((llc_data->decoded_xid.iov_ui.valid) AND (GET_STATE(U) NEQ U_ABM) )
  {
    /*
     * Do not tag iov_ui for the XID response, because it shall only be
     * transmitted in downlink direction.
     * <R.LLC.XID_PAR.A.004>
     */

    /*
     * Copy value to global variable (no sense of negotiation).
     */
    llc_data->iov_ui = llc_data->decoded_xid.iov_ui.value;
  }


  /**************************************************************************
   * IOV-I:
   * Shall only be negotiated with SABM and UA frames. This is already
   * checked in u_check_xid().
   * <R.LLC.XID_PAR.A.003>, <R.LLC.XID_INVA.A.010>
   */
  if (llc_data->decoded_xid.iov_i.valid)
  {
    /*
     * Do not tag iov_i for the XID response, because it shall only be
     * transmitted in downlink direction.
     * <R.LLC.XID_PAR.A.004>
     */

    /*
     * Copy value to global variable (no sense of negotiation).
     */
    *(llc_data->iov_i) = llc_data->decoded_xid.iov_i.value;
  }


  /**************************************************************************
   * T200 (and T201):
   * Can be negotiated in state 'ADM' and 'ABM'. New values shall only apply
   * to timers set after the negotiation has been completed (which is
   * accomplished automatically).
   * <R.LLC.XID_PAR.A.006>, <R.LLC.XID_PAR.A.007>
   */
  if (llc_data->decoded_xid.t200.valid)
  {
    if (cr_bit EQ SGSN_COMMAND)
    {
      if (llc_xid_value_acceptable (llc_data->current_sapi, XID_T200,
        llc_data->decoded_xid.t200.value))
      {
        llc_data->t200->length = XID2INT (llc_data->decoded_xid.t200.value);
      }
      else /* value is not acceptable */
      {
        /*
         * Sense of negotiation down: offer lower value; sense of
         * negotiation up: offer higher value.
         * This happens automatically by just responding with our current
         * global value.
         */
      }

      /*
       * In case we also want to negotiate this value, but to a lower!
       * value, delete this parameter from the requested XID structure
       */
      if (llc_data->u->requested_xid.t200.valid AND
          llc_data->u->requested_xid.t200.value <= llc_data->decoded_xid.t200.value)
      {
            llc_data->u->requested_xid.t200.valid = FALSE;
      }

      /*
       * Tag parameter for next response.
       */
      llc_data->u->xid_tag |= 0x00000001L << XID_T200;
    }
    else /* SGSN_RESPONSE */
    {
      /*
       * Sense of negotiation-correctness of response has already been checked
       * in u_check_xid().
       */
      llc_data->t200->length = XID2INT (llc_data->decoded_xid.t200.value);
    }

    /*
     * Remove tag in xid_tag_negotiate, because this parameter has been
     * explicitly negotiated by the peer.
     * <R.LLC.XIDNEG_R.A.015>
     */
    llc_data->u->xid_tag_negotiate &= ~(0x00000001L << XID_T200);
  }


  /**************************************************************************
   * N200:
   * Can be negotiated in state 'ADM' and 'ABM'.
   * <R.LLC.XID_PAR.A.006>
   */
  if (llc_data->decoded_xid.n200.valid)
  {
    if (cr_bit EQ SGSN_COMMAND)
    {
      if (llc_xid_value_acceptable (llc_data->current_sapi, XID_N200,
        llc_data->decoded_xid.n200.value))
      {
        *(llc_data->n200) = llc_data->decoded_xid.n200.value;
      }
      else /* value is not acceptable */
      {
        /*
         * Sense of negotiation down: offer lower value; sense of
         * negotiation up: offer higher value.
         * This happens automatically by just responding with our current
         * global value.
         */
      }

      /*
       * In case we also want to negotiate this value, but to a lower!
       * value, delete this parameter from the requested XID structure
       */
      if (llc_data->u->requested_xid.n200.valid AND
          llc_data->u->requested_xid.n200.value <= llc_data->decoded_xid.n200.value)
      {
          llc_data->u->requested_xid.n200.valid = FALSE;
      }

      /*
       * Tag parameter for next response.
       */
      llc_data->u->xid_tag |= 0x00000001L << XID_N200;
    }
    else /* SGSN_RESPONSE */
    {
      /*
       * Sense of negotiation-correctness of response has already been checked
       * in u_check_xid().
       */
      *(llc_data->n200) = llc_data->decoded_xid.n200.value;
    }

    /*
     * Remove tag in xid_tag_negotiate, because this parameter has been
     * explicitly negotiated by the peer.
     * <R.LLC.XIDNEG_R.A.015>
     */
    llc_data->u->xid_tag_negotiate &= ~(0x00000001L << XID_N200);
  }


  /**************************************************************************
   * N201-U:
   * Can be negotiated in state 'ADM' and 'ABM'. If N201-U is negotiated to a
   * lower value than previously used, then any queued or new U and UI frames
   * that violate the new value of N201-U should be discarded and not
   * transmitted.
   * <R.LLC.XID_PAR.A.006>, <R.LLC.XID_PAR.A.010>
   */
  if (llc_data->decoded_xid.n201_u.valid)
  {
    if (cr_bit EQ SGSN_COMMAND)
    {
      if (llc_xid_value_acceptable (llc_data->current_sapi, XID_N201_U,
                                    llc_data->decoded_xid.n201_u.value))
      {
        if (llc_data->decoded_xid.n201_u.value NEQ *(llc_data->n201_u))
        {
          *(llc_data->n201_u) = llc_data->decoded_xid.n201_u.value;

          /*
           * LL_XID_IND has to be sent to layer 3.
           */
          *xid_ind = TRUE;
        }
      }
      else /* value is not acceptable */
      {
        /*
         * Sense of negotiation down: offer lower value; sense of
         * negotiation up: offer higher value.
         * This happens automatically by just responding with our current
         * global value.
         */
      }

      /*
       * In case we also want to negotiate this value, but to a higher
       * value, delete this parameter from the requested XID structure
       */
      if (llc_data->u->requested_xid.n201_u.valid AND
          llc_data->u->requested_xid.n201_u.value >= llc_data->decoded_xid.n201_u.value)
      {
          llc_data->u->requested_xid.n201_u.valid = FALSE;
      }

      /*
       * Tag parameter for next response.
       */
      llc_data->u->xid_tag |= 0x00000001L << XID_N201_U;
    }
    else /* SGSN_RESPONSE */
    {
      if (llc_data->decoded_xid.n201_u.value NEQ *(llc_data->n201_u))
      {
        /*
         * Sense of negotiation-correctness of response has already been checked
         * in u_check_xid().
         */
        *(llc_data->n201_u) = llc_data->decoded_xid.n201_u.value;

        /*
         * LL_XID_IND has to be sent to layer 3.
         */
        *xid_ind = TRUE;
      }
    }

    /*
     * Remove tag in xid_tag_negotiate, because this parameter has been
     * explicitly negotiated by the peer.
     * <R.LLC.XIDNEG_R.A.015>
     */
    llc_data->u->xid_tag_negotiate &= ~(0x00000001L << XID_N201_U);
  }


  /**************************************************************************
   * N201-I:
   * N201-I can be negotiated to any value in state 'ADM'. It can only be
   * negotiated to the same or higher value as previously used in state 'ABM'.
   * <R.LLC.XID_PAR.A.008>, <R.LLC.XID_PAR.A.009>
   */
  if (llc_data->decoded_xid.n201_i.valid)
  {
    if (cr_bit EQ SGSN_COMMAND)
    {
      if (llc_xid_value_acceptable (llc_data->current_sapi, XID_N201_I,
        llc_data->decoded_xid.n201_i.value))
      {
        if (((GET_STATE (U) EQ U_ABM) AND
          (llc_data->decoded_xid.n201_i.value > *(llc_data->n201_i)))
          OR /* state 'ADM' */
          (llc_data->decoded_xid.n201_i.value NEQ *(llc_data->n201_i)))
        {
          *(llc_data->n201_i) = llc_data->decoded_xid.n201_i.value;

          /*
           * LL_XID_IND has to be sent to layer 3.
           */
          *xid_ind = TRUE;
        }
      }
      else /* value is not acceptable */
      {
        /*
         * Sense of negotiation down: offer lower value; sense of
         * negotiation up: offer higher value.
         * This happens automatically by just responding with our current
         * global value.
         */
      }

      /*
       * In case we also want to negotiate this value, but to a higher
       * value, delete this parameter from the requested XID structure
       */

      if (llc_data->u->requested_xid.n201_i.valid AND
          llc_data->u->requested_xid.n201_i.value >= llc_data->decoded_xid.n201_i.value)
      {
        llc_data->u->requested_xid.n201_i.valid = FALSE;
      }

      /*
       * Tag parameter for next response.
       */
      llc_data->u->xid_tag |= 0x00000001L << XID_N201_I;
    }
    else /* SGSN_RESPONSE */
    {
      /*
       * Sense of negotiation-correctness of response has already been checked
       * in u_check_xid().
       */
      if (((GET_STATE (U) EQ U_ABM) AND
        (llc_data->decoded_xid.n201_i.value > *(llc_data->n201_i)))
        OR /* state 'ADM' */
        (llc_data->decoded_xid.n201_i.value NEQ *(llc_data->n201_i)))
      {
        *(llc_data->n201_i) = llc_data->decoded_xid.n201_i.value;

        /*
         * LL_XID_IND has to be sent to layer 3.
         */
        *xid_ind = TRUE;
      }
    }

    /*
     * Remove tag in xid_tag_negotiate, because this parameter has been
     * explicitly negotiated by the peer.
     * <R.LLC.XIDNEG_R.A.015>
     */
    llc_data->u->xid_tag_negotiate &= ~(0x00000001L << XID_N201_I);
  }


  /**************************************************************************
   * mD:
   * mD can be negotiated to any value in state 'ADM'. It can only be
   * negotiated to the same or higher value as previously used in state 'ABM'.
   * <R.LLC.XID_PAR.A.008>, <R.LLC.XID_PAR.A.009>
   */
  if (llc_data->decoded_xid.md.valid)
  {
    if (cr_bit EQ SGSN_COMMAND)
    {
      if (llc_xid_value_acceptable (llc_data->current_sapi, XID_MD,
        llc_data->decoded_xid.md.value))
      {
        if (((GET_STATE (U) EQ U_ABM) AND
          (llc_data->decoded_xid.md.value > *(llc_data->md)))
          OR /* state 'ADM' */
          (llc_data->decoded_xid.md.value NEQ *(llc_data->md)))
        {
          *(llc_data->md) = llc_data->decoded_xid.md.value;
        }
      }
      else /* value is not acceptable */
      {
        /*
         * Sense of negotiation down: offer lower value; sense of
         * negotiation up: offer higher value.
         * This happens automatically by just responding with our current
         * global value.
         */
      }

      /*
       * In case we also want to negotiate this value, but to a higher
       * value, delete this parameter from the requested XID structure
       */
      if (llc_data->u->requested_xid.md.valid AND
          llc_data->u->requested_xid.md.value >= llc_data->decoded_xid.md.value)
      {
          llc_data->u->requested_xid.md.valid = FALSE;
      }

      /*
       * Tag parameter for next response.
       */
      llc_data->u->xid_tag |= 0x00000001L << XID_MD;
    }
    else /* SGSN_RESPONSE */
    {
      /*
       * Sense of negotiation-correctness of response has already been checked
       * in u_check_xid().
       */
      if (((GET_STATE (U) EQ U_ABM) AND
        (llc_data->decoded_xid.md.value > *(llc_data->md)))
        OR /* state 'ADM' */
        (llc_data->decoded_xid.md.value NEQ *(llc_data->md)))
      {
        *(llc_data->md) = llc_data->decoded_xid.md.value;
      }
    }

    /*
     * Remove tag in xid_tag_negotiate, because this parameter has been
     * explicitly negotiated by the peer.
     * <R.LLC.XIDNEG_R.A.015>
     */
    llc_data->u->xid_tag_negotiate &= ~(0x00000001L << XID_MD);
  }


  /**************************************************************************
   * mU:
   * mU can be negotiated to any value in state 'ADM'. It can only be
   * negotiated to the same or higher value as previously used in state 'ABM'.
   * <R.LLC.XID_PAR.A.008>, <R.LLC.XID_PAR.A.009>
   */
  if (llc_data->decoded_xid.mu.valid)
  {
    if (cr_bit EQ SGSN_COMMAND)
    {
      if (llc_xid_value_acceptable (llc_data->current_sapi, XID_MU,
        llc_data->decoded_xid.mu.value))
      {
        if (((GET_STATE (U) EQ U_ABM) AND
          (llc_data->decoded_xid.mu.value > *(llc_data->mu)))
          OR /* state 'ADM' */
          (llc_data->decoded_xid.mu.value NEQ *(llc_data->mu)))
        {
          *(llc_data->mu) = llc_data->decoded_xid.mu.value;
        }
      }
      else /* value is not acceptable */
      {
        /*
         * Sense of negotiation down: offer lower value; sense of
         * negotiation up: offer higher value.
         * This happens automatically by just responding with our current
         * global value.
         */
      }

      /*
       * In case we also want to negotiate this value, but to a higher
       * value, delete this parameter from the requested XID structure
       */
      if (llc_data->u->requested_xid.mu.valid AND
          llc_data->u->requested_xid.mu.value >= llc_data->decoded_xid.mu.value)
      {
        llc_data->u->requested_xid.mu.valid = FALSE;
      }

      /*
       * Tag parameter for next response.
       */
      llc_data->u->xid_tag |= 0x00000001L << XID_MU;
    }
    else /* SGSN_RESPONSE */
    {
      /*
       * Sense of negotiation-correctness of response has already been checked
       * in u_check_xid().
       */
      if (((GET_STATE (U) EQ U_ABM) AND
        (llc_data->decoded_xid.mu.value > *(llc_data->mu)))
        OR /* state 'ADM' */
        (llc_data->decoded_xid.mu.value NEQ *(llc_data->mu)))
      {
        *(llc_data->mu) = llc_data->decoded_xid.mu.value;
      }
    }

    /*
     * Remove tag in xid_tag_negotiate, because this parameter has been
     * explicitly negotiated by the peer.
     * <R.LLC.XIDNEG_R.A.015>
     */
    llc_data->u->xid_tag_negotiate &= ~(0x00000001L << XID_MU);
  }


  /**************************************************************************
   * kD:
   * kD can be negotiated to any value in state 'ADM'. It can only be
   * negotiated to the same or higher value as previously used in state 'ABM'.
   * <R.LLC.XID_PAR.A.008>, <R.LLC.XID_PAR.A.009>
   */
  if (llc_data->decoded_xid.kd.valid)
  {
    if (cr_bit EQ SGSN_COMMAND)
    {
      if (llc_xid_value_acceptable (llc_data->current_sapi, XID_KD,
        llc_data->decoded_xid.kd.value))
      {
        if (((GET_STATE (U) EQ U_ABM) AND
          (llc_data->decoded_xid.kd.value > *(llc_data->kd)))
          OR /* state 'ADM' */
          (llc_data->decoded_xid.kd.value NEQ *(llc_data->kd)))
        {
          *(llc_data->kd) = llc_data->decoded_xid.kd.value;
        }
      }
      else /* value is not acceptable */
      {
        /*
         * Sense of negotiation down: offer lower value; sense of
         * negotiation up: offer higher value.
         * This happens automatically by just responding with our current
         * global value.
         */
      }

      /*
       * In case we also want to negotiate this value, but to a higher
       * value, delete this parameter from the requested XID structure
       */
      if (llc_data->u->requested_xid.kd.valid AND
          llc_data->u->requested_xid.kd.value >= llc_data->decoded_xid.kd.value)
      {
          llc_data->u->requested_xid.kd.valid = FALSE;
      }

      /*
       * Tag parameter for next response.
       */
      llc_data->u->xid_tag |= 0x00000001L << XID_KD;
    }
    else /* SGSN_RESPONSE */
    {
      /*
       * Sense of negotiation-correctness of response has already been checked
       * in u_check_xid().
       */
      if (((GET_STATE (U) EQ U_ABM) AND
        (llc_data->decoded_xid.kd.value > *(llc_data->kd)))
        OR /* state 'ADM' */
        (llc_data->decoded_xid.kd.value NEQ *(llc_data->kd)))
      {
        *(llc_data->kd) = llc_data->decoded_xid.kd.value;
      }
    }

    /*
     * Remove tag in xid_tag_negotiate, because this parameter has been
     * explicitly negotiated by the peer.
     * <R.LLC.XIDNEG_R.A.015>
     */
    llc_data->u->xid_tag_negotiate &= ~(0x00000001L << XID_KD);
  }


  /**************************************************************************
   * kU:
   * kU can be negotiated to any value in state 'ADM'. It can only be
   * negotiated to the same or higher value as previously used in state 'ABM'.
   * <R.LLC.XID_PAR.A.008>, <R.LLC.XID_PAR.A.009>
   */
  if (llc_data->decoded_xid.ku.valid)
  {
    if (cr_bit EQ SGSN_COMMAND)
    {
      if (llc_xid_value_acceptable (llc_data->current_sapi, XID_KU,
        llc_data->decoded_xid.ku.value))
      {
        if (((GET_STATE (U) EQ U_ABM) AND
          (llc_data->decoded_xid.ku.value > *(llc_data->ku)))
          OR /* state 'ADM' */
          (llc_data->decoded_xid.ku.value NEQ *(llc_data->ku)))
        {
          *(llc_data->ku) = llc_data->decoded_xid.ku.value;
        }
      }
      else /* value is not acceptable */
      {
        /*
         * Sense of negotiation down: offer lower value; sense of
         * negotiation up: offer higher value.
         * This happens automatically by just responding with our current
         * global value.
         */
      }

      /*
       * In case we also want to negotiate this value, but to a higher
       * value, delete this parameter from the requested XID structure
       */
      if (llc_data->u->requested_xid.ku.valid AND
          llc_data->u->requested_xid.ku.value >= llc_data->decoded_xid.ku.value)
      {
          llc_data->u->requested_xid.ku.valid = FALSE;
      }

      /*
       * Tag parameter for next response.
       */
      llc_data->u->xid_tag |= 0x00000001L << XID_KU;
    }
    else /* SGSN_RESPONSE */
    {
      /*
       * Sense of negotiation-correctness of response has already been checked
       * in u_check_xid().
       */
      if (((GET_STATE (U) EQ U_ABM) AND
        (llc_data->decoded_xid.ku.value > *(llc_data->ku)))
        OR /* state 'ADM' */
        (llc_data->decoded_xid.ku.value NEQ *(llc_data->ku)))
      {
        *(llc_data->ku) = llc_data->decoded_xid.ku.value;
      }
    }

    /*
     * Remove tag in xid_tag_negotiate, because this parameter has been
     * explicitly negotiated by the peer.
     * <R.LLC.XIDNEG_R.A.015>
     */
    llc_data->u->xid_tag_negotiate &= ~(0x00000001L << XID_KU);
  }


  /**************************************************************************
   * Layer 3 stuff
   */
  if (llc_data->decoded_l3_xid.valid)
  {
    /*
     * LL_XID_IND has to be sent to layer 3.
     */
    *xid_ind = TRUE;

    if (cr_bit EQ SGSN_COMMAND)
    {
      /*
       * Tag parameter for next response.
       */
      llc_data->u->xid_tag |= 0x00000001L << XID_LAYER_3;
    }
    else /* SGSN_RESPONSE */
    {
      /*
       * Reset valid flag of parameter in requested_xid because it has been
       * included in the response.
       */
     llc_data->requested_l3_xid->valid = FALSE;
    }

    /*
     * Remove tag in xid_tag_negotiate, because this parameter has been
     * explicitly negotiated by the peer.
     * <R.LLC.XIDNEG_R.A.015>
     */
    llc_data->u->xid_tag_negotiate &= ~(0x00000001L << XID_LAYER_3);
  }

  return; /* u_eval_xid */
}


/*
+------------------------------------------------------------------------------
| Function    : u_insert_xid
+------------------------------------------------------------------------------
| Description :  This procedure inserts (i.e. appends) the XID parameters that
|                are tagged in llc_xid_tag. This variable is normally set in
|                u_eval_xid(). The parameter cr_bit indicates if the XID
|                information field is written in a command or a response frame.
|                The values of the parameters are taken from llc_requested_xid
|                if marked as valid (normally the cause when a command frame is
|                to be sent), otherwise from global variables (the normal cause
|                for a response frame). If a response frame is to be sent, and
|                the parameter is tagged in llc_xid_tag, but has not been
|                received in the command frame (as indicated by
|                llc_decoded_xid), the parameter is tagged in llc_xid_negotiate.
|                Each parameter is tagged in llc_xid_tag_sent for collision
|                checks and the like
|
| Parameters  : sdu     - a valid pointer to an SDU, containing enough octets
|                         for the tagged number of XID parameters
|               cr_bit  - MS_COMMAND/_RESPONSE is to be sent
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_insert_xid
(
#ifdef LL_DESC
  T_desc_list3 *desc_list3,
#else
  T_sdu *sdu,
#endif
  T_BIT cr_bit
)
{
  UBYTE             *data;
  ULONG             value;
#ifdef LL_DESC
  UBYTE             *desc_buf;
  T_desc3           *desc3;
#endif
  TRACE_FUNCTION( "u_insert_xid" );

  /*
   * Set data to point to the first free data octet in sdu. data has to
   * be incremented with each octet that is written in sdu to ensure that
   * it always points to the first free data octet.
   */
#ifdef LL_DESC
  desc3    = (T_desc3*) desc_list3->first;
  desc_buf = (UBYTE*)desc3->buffer;
  data     = &desc_buf[(desc3->offset)+(desc3->len)];
#else
  data = &sdu->buf[(sdu->o_buf/8)+(sdu->l_buf/8)];
#endif
  /*
   * Preset global XID sent indicator for next response.
   */
  llc_data->u->xid_tag_sent = 0L;


  /*************************************************************************
   * Version:
   * Shall not be negotiated while in state 'ABM'.
   * <R.LLC.XID_PAR.A.001>
   */
  if (llc_data->u->xid_tag & (0x00000001L << XID_VERSION) AND
    (GET_STATE (U) NEQ U_ABM))
  {
    if (cr_bit EQ MS_COMMAND)
    {
      value = llc_data->u->requested_xid.version.value;
    }
    else
    {
      if (llc_data->u->requested_xid.version.valid)
      {
        llc_data->version = llc_data->u->requested_xid.version.value;
      }

      value = llc_data->version;

      /*
       * Tag a parameter for further negotiation that was not included in
       * the XID command in every XID response until the parameter has been
       * explicitly negotiated, either by responding to an XID command that
       * included the parameter, or by explicitly including the parameter
       * the next time an XID command is transmitted.
       * <R.LLC.XIDNEG_R.A.015>
       */
      if (!llc_data->decoded_xid.version.valid)
      {
        llc_data->u->xid_tag_negotiate |= 0x00000001L << XID_VERSION;
      }
    }

    /*
     * Append determined value to SDU data, increment SDU data pointer to
     * point to the first free data octet again.
     */
    *data++ = (UBYTE)0x00 | ((UBYTE)XID_VERSION << 2) | (UBYTE)XID_VERSION_LEN;
    *data++ = (UBYTE)value;

#ifdef LL_DESC
    desc3->len += (XID_VERSION_LEN + 1);
#else
    sdu->l_buf += (XID_VERSION_LEN + 1) * 8;
#endif
    /*
     * Tag parameter for collision checks and the like.
     */
    llc_data->u->xid_tag_sent |= 0x00000001L << XID_VERSION;
  }

  /*************************************************************************
   * T200:
   * Can be negotiated in state 'ADM' and 'ABM'.
   * <R.LLC.XID_PAR.A.006>
   */
  if (llc_data->u->xid_tag & (0x00000001L << XID_T200))
  {
    if (cr_bit EQ MS_COMMAND)
    {
      value = llc_data->u->requested_xid.t200.value;
    }
    else
    {
      if (llc_data->u->requested_xid.t200.valid)
      {
        llc_data->t200->length = XID2INT(llc_data->u->requested_xid.t200.value);
      }

      value = INT2XID(llc_data->t200->length);

      /*
       * Tag a parameter for further negotiation that was not included in
       * the XID command in every XID response until the parameter has been
       * explicitly negotiated, either by responding to an XID command that
       * included the parameter, or by explicitly including the parameter
       * the next time an XID command is transmitted.
       * <R.LLC.XIDNEG_R.A.015>
       */
      if (!llc_data->decoded_xid.t200.valid)
      {
        llc_data->u->xid_tag_negotiate |= 0x00000001L << XID_T200;
      }
    }

    /*
     * Append determined value to SDU data, increment SDU data pointer to
     * point to the first free data octet again.
     */
    *data++ = (UBYTE)0x00 | ((UBYTE)XID_T200 << 2) | (UBYTE)XID_T200_LEN;
    *data++ = (UBYTE)(value >> 8);
    *data++ = (UBYTE)value;

#ifdef LL_DESC
    desc3->len += (XID_T200_LEN + 1);
#else
    sdu->l_buf += (XID_T200_LEN + 1) * 8;
#endif

    /*
     * Tag parameter for collision checks and the like.
     */
    llc_data->u->xid_tag_sent |= 0x00000001L << XID_T200;
  }

  /*************************************************************************
   * N200:
   * Can be negotiated in state 'ADM' and 'ABM'.
   * <R.LLC.XID_PAR.A.006>
   */
  if (llc_data->u->xid_tag & (0x00000001L << XID_N200))
  {
    if (cr_bit EQ MS_COMMAND)
    {
      value = llc_data->u->requested_xid.n200.value;
    }
    else
    {
      if (llc_data->u->requested_xid.n200.valid)
      {
        *(llc_data->n200) = llc_data->u->requested_xid.n200.value;
      }

      value = *(llc_data->n200);

      /*
       * Tag a parameter for further negotiation that was not included in
       * the XID command in every XID response until the parameter has been
       * explicitly negotiated, either by responding to an XID command that
       * included the parameter, or by explicitly including the parameter
       * the next time an XID command is transmitted.
       * <R.LLC.XIDNEG_R.A.015>
       */
      if (!llc_data->decoded_xid.n200.valid)
      {
        llc_data->u->xid_tag_negotiate |= 0x00000001L << XID_N200;
      }
    }

    /*
     * Append determined value to SDU data, increment SDU data pointer to
     * point to the first free data octet again.
     */
    *data++ = (UBYTE)0x00 | ((UBYTE)XID_N200 << 2) | (UBYTE)XID_N200_LEN;
    *data++ = (UBYTE)value;

#ifdef LL_DESC
    desc3->len += (XID_N200_LEN + 1);
#else
    sdu->l_buf += (XID_N200_LEN + 1) * 8;
#endif

    /*
     * Tag parameter for collision checks and the like.
     */
    llc_data->u->xid_tag_sent |= 0x00000001L << XID_N200;
  }

  /*************************************************************************
   * N201-U:
   * Can be negotiated in state 'ADM' and 'ABM'.
   * <R.LLC.XID_PAR.A.006>
   */
  if (llc_data->u->xid_tag & (0x00000001L << XID_N201_U))
  {
    if (cr_bit EQ MS_COMMAND)
    {
      value = llc_data->u->requested_xid.n201_u.value;
    }
    else
    {
      if (llc_data->u->requested_xid.n201_u.valid)
      {
        *(llc_data->n201_u) = llc_data->u->requested_xid.n201_u.value;
      }

      value = *(llc_data->n201_u);

      /*
       * Tag a parameter for further negotiation that was not included in
       * the XID command in every XID response until the parameter has been
       * explicitly negotiated, either by responding to an XID command that
       * included the parameter, or by explicitly including the parameter
       * the next time an XID command is transmitted.
       * <R.LLC.XIDNEG_R.A.015>
       */
      if (!llc_data->decoded_xid.n201_u.valid)
      {
        llc_data->u->xid_tag_negotiate |= 0x00000001L << XID_N201_U;
      }
    }

    /*
     * Append determined value to SDU data, increment SDU data pointer to
     * point to the first free data octet again.
     */
    *data++ = (UBYTE)0x00 | ((UBYTE)XID_N201_U << 2) | (UBYTE)XID_N201_U_LEN;
    *data++ = (UBYTE)(value >> 8);
    *data++ = (UBYTE)value;

#ifdef LL_DESC
    desc3->len += (XID_N201_U_LEN + 1);
#else
    sdu->l_buf += (XID_N201_U_LEN + 1) * 8;
#endif

    /*
     * Tag parameter for collision checks and the like.
     */
    llc_data->u->xid_tag_sent |= 0x00000001L << XID_N201_U;
  }

  /*************************************************************************
   * N201-I:
   * Can be negotiated in state 'ADM' and 'ABM'.
   * <R.LLC.XID_PAR.A.006>
   */
  if (llc_data->u->xid_tag & (0x00000001L << XID_N201_I))
  {
    if (cr_bit EQ MS_COMMAND)
    {
      value = llc_data->u->requested_xid.n201_i.value;
    }
    else
    {
      if (llc_data->u->requested_xid.n201_i.valid)
      {
        *(llc_data->n201_i) = llc_data->u->requested_xid.n201_i.value;
      }

      value = *(llc_data->n201_i);

      /*
       * Tag a parameter for further negotiation that was not included in
       * the XID command in every XID response until the parameter has been
       * explicitly negotiated, either by responding to an XID command that
       * included the parameter, or by explicitly including the parameter
       * the next time an XID command is transmitted.
       * <R.LLC.XIDNEG_R.A.015>
       */
      if (!llc_data->decoded_xid.n201_i.valid)
      {
        llc_data->u->xid_tag_negotiate |= 0x00000001L << XID_N201_I;
      }
    }

    /*
     * Append determined value to SDU data, increment SDU data pointer to
     * point to the first free data octet again.
     */
    *data++ = (UBYTE)0x00 | ((UBYTE)XID_N201_I << 2) | (UBYTE)XID_N201_I_LEN;
    *data++ = (UBYTE)(value >> 8);
    *data++ = (UBYTE)value;

#ifdef LL_DESC
    desc3->len += (XID_N201_I_LEN + 1);
#else
    sdu->l_buf += (XID_N201_I_LEN + 1) * 8;
#endif

    /*
     * Tag parameter for collision checks and the like.
     */
    llc_data->u->xid_tag_sent |= 0x00000001L << XID_N201_I;
  }

  /*************************************************************************
   * md:
   * Can be negotiated in state 'ADM' and 'ABM'.
   * <R.LLC.XID_PAR.A.006>
   */
  if (llc_data->u->xid_tag & (0x00000001L << XID_MD))
  {
    if (cr_bit EQ MS_COMMAND)
    {
      value = llc_data->u->requested_xid.md.value;
    }
    else
    {
      if (llc_data->u->requested_xid.md.valid)
      {
        *(llc_data->md) = llc_data->u->requested_xid.md.value;
      }

      value = *(llc_data->md);

      /*
       * Tag a parameter for further negotiation that was not included in
       * the XID command in every XID response until the parameter has been
       * explicitly negotiated, either by responding to an XID command that
       * included the parameter, or by explicitly including the parameter
       * the next time an XID command is transmitted.
       * <R.LLC.XIDNEG_R.A.015>
       */
      if (!llc_data->decoded_xid.md.valid)
      {
        llc_data->u->xid_tag_negotiate |= 0x00000001L << XID_MD;
      }
    }

    /*
     * Append determined value to SDU data, increment SDU data pointer to
     * point to the first free data octet again.
     */
    *data++ = (UBYTE)0x00 | ((UBYTE)XID_MD << 2) | (UBYTE)XID_MD_LEN;
    *data++ = (UBYTE)(value >> 8);
    *data++ = (UBYTE)value;

#ifdef LL_DESC
    desc3->len += (XID_MD_LEN + 1);
#else
    sdu->l_buf += (XID_MD_LEN + 1) * 8;
#endif

    /*
     * Tag parameter for collision checks and the like.
     */
    llc_data->u->xid_tag_sent |= 0x00000001L << XID_MD;
  }

  /*************************************************************************
   * mu:
   * Can be negotiated in state 'ADM' and 'ABM'.
   * <R.LLC.XID_PAR.A.006>
   */
  if (llc_data->u->xid_tag & (0x00000001L << XID_MU))
  {
    if (cr_bit EQ MS_COMMAND)
    {
      value = llc_data->u->requested_xid.mu.value;
    }
    else
    {
      if (llc_data->u->requested_xid.mu.valid)
      {
        *(llc_data->mu) = llc_data->u->requested_xid.mu.value;
      }

      value = *(llc_data->mu);

      /*
       * Tag a parameter for further negotiation that was not included in
       * the XID command in every XID response until the parameter has been
       * explicitly negotiated, either by responding to an XID command that
       * included the parameter, or by explicitly including the parameter
       * the next time an XID command is transmitted.
       * <R.LLC.XIDNEG_R.A.015>
       */
      if (!llc_data->decoded_xid.mu.valid)
      {
        llc_data->u->xid_tag_negotiate |= 0x00000001L << XID_MU;
      }
    }

    /*
     * Append determined value to SDU data, increment SDU data pointer to
     * point to the first free data octet again.
     */
    *data++ = (UBYTE)0x00 | ((UBYTE)XID_MU << 2) | (UBYTE)XID_MU_LEN;
    *data++ = (UBYTE)(value >> 8);
    *data++ = (UBYTE)value;

#ifdef LL_DESC
    desc3->len += (XID_MU_LEN + 1);
#else
    sdu->l_buf += (XID_MU_LEN + 1) * 8;
#endif

    /*
     * Tag parameter for collision checks and the like.
     */
    llc_data->u->xid_tag_sent |= 0x00000001L << XID_MU;
  }

  /*************************************************************************
   * kd:
   * Can be negotiated in state 'ADM' and 'ABM'.
   * <R.LLC.XID_PAR.A.006>
   */
  if (llc_data->u->xid_tag & (0x00000001L << XID_KD))
  {
    if (cr_bit EQ MS_COMMAND)
    {
      value = llc_data->u->requested_xid.kd.value;
    }
    else
    {
      if (llc_data->u->requested_xid.kd.valid)
      {
        *(llc_data->kd) = llc_data->u->requested_xid.kd.value;
      }

      value = *(llc_data->kd);

      /*
       * Tag a parameter for further negotiation that was not included in
       * the XID command in every XID response until the parameter has been
       * explicitly negotiated, either by responding to an XID command that
       * included the parameter, or by explicitly including the parameter
       * the next time an XID command is transmitted.
       * <R.LLC.XIDNEG_R.A.015>
       */
      if (!llc_data->decoded_xid.kd.valid)
      {
        llc_data->u->xid_tag_negotiate |= 0x00000001L << XID_KD;
      }
    }

    /*
     * Append determined value to SDU data, increment SDU data pointer to
     * point to the first free data octet again.
     */
    *data++ = (UBYTE)0x00 | ((UBYTE)XID_KD << 2) | (UBYTE)XID_KD_LEN;
    *data++ = (UBYTE)value;

#ifdef LL_DESC
    desc3->len += (XID_KD_LEN + 1);
#else
    sdu->l_buf += (XID_KD_LEN + 1) * 8;
#endif

    /*
     * Tag parameter for collision checks and the like.
     */
    llc_data->u->xid_tag_sent |= 0x00000001L << XID_KD;
  }

  /*************************************************************************
   * ku:
   * Can be negotiated in state 'ADM' and 'ABM'.
   * <R.LLC.XID_PAR.A.006>
   */
  if (llc_data->u->xid_tag & (0x00000001L << XID_KU))
  {
    if (cr_bit EQ MS_COMMAND)
    {
      value = llc_data->u->requested_xid.ku.value;
    }
    else
    {
      if (llc_data->u->requested_xid.ku.valid)
      {
        *(llc_data->ku) = llc_data->u->requested_xid.ku.value;
      }

      value = *(llc_data->ku);

      /*
       * Tag a parameter for further negotiation that was not included in
       * the XID command in every XID response until the parameter has been
       * explicitly negotiated, either by responding to an XID command that
       * included the parameter, or by explicitly including the parameter
       * the next time an XID command is transmitted.
       * <R.LLC.XIDNEG_R.A.015>
       */
      if (!llc_data->decoded_xid.ku.valid)
      {
        llc_data->u->xid_tag_negotiate |= 0x00000001L << XID_KU;
      }
    }

    /*
     * Append determined value to SDU data, increment SDU data pointer to
     * point to the first free data octet again.
     */
    *data++ = (UBYTE)0x00 | ((UBYTE)XID_KU << 2) | (UBYTE)XID_KU_LEN;
    *data++ = (UBYTE)value;

#ifdef LL_DESC
    desc3->len += (XID_KU_LEN + 1);
#else
    sdu->l_buf += (XID_KU_LEN + 1) * 8;
#endif

    /*
     * Tag parameter for collision checks and the like.
     */
    llc_data->u->xid_tag_sent |= 0x00000001L << XID_KU;
  }

  /*************************************************************************
   * Insert Layer 3 (sense of negotiation: both)
   */
  if (llc_data->u->xid_tag & (0x00000001L << XID_LAYER_3))
  {
    if (llc_data->requested_l3_xid->valid)
    {
      /*
       * Write two bytes header
       */
      *data++ = (UBYTE)0x80 /* = set length to 8 bit */ | ((UBYTE)XID_LAYER_3 << 2)
                | ((UBYTE)((llc_data->requested_l3_xid->length >> 6) & 0x03));
      *data++ = (UBYTE)(llc_data->requested_l3_xid->length << 2);

      /*
       * Copy data
       */
      memcpy (data, llc_data->requested_l3_xid->value,
                    llc_data->requested_l3_xid->length);

      /*
       * Increase DESC/SDU size
       */
#ifdef LL_DESC
      desc3->len += (llc_data->requested_l3_xid->length + 2);
#else
      sdu->l_buf += (llc_data->requested_l3_xid->length + 2) * 8;
#endif
      /*
       * Tag parameter for collision checks and the like.
       */
      llc_data->u->xid_tag_sent |= 0x00000001L << XID_LAYER_3;
    }
  }

#ifdef LL_DESC
  desc_list3->list_len = desc3->len;
#endif /* LL_DESC */

#ifdef TRACE_EVE
#ifdef LL_DESC
  {
    int i;
    int l = desc3->len;
    int o = desc3->offset;

    vsi_o_ttrace(VSI_CALLER TC_EVENT, "ul XID sdu dump len:%d bytes", l);

    for (i=0; i<l; i+=10)
    {
      vsi_o_ttrace(VSI_CALLER TC_EVENT, "%.2x %.2x %.2x %.2x %.2x   %.2x %.2x %.2x %.2x %.2x",
                                        desc_buf[o+i+0], desc_buf[o+i+1],
                                        desc_buf[o+i+2], desc_buf[o+i+3], desc_buf[o+i+4],

                                        desc_buf[o+i+5], desc_buf[o+i+6],
                                        desc_buf[o+i+7], desc_buf[o+i+8], desc_buf[o+i+9]);
    }
  }

#else
  {
    int i;
    int l = sdu->l_buf/8;
    int o = sdu->o_buf/8;

    vsi_o_ttrace(VSI_CALLER TC_EVENT, "ul XID sdu dump len:%d bytes", l);

    for (i=0; i<l; i+=10)
    {
      vsi_o_ttrace(VSI_CALLER TC_EVENT, "%.2x %.2x %.2x %.2x %.2x   %.2x %.2x %.2x %.2x %.2x",
                                        sdu->buf[o+i+0], sdu->buf[o+i+1],
                                        sdu->buf[o+i+2], sdu->buf[o+i+3], sdu->buf[o+i+4],

                                        sdu->buf[o+i+5], sdu->buf[o+i+6],
                                        sdu->buf[o+i+7], sdu->buf[o+i+8], sdu->buf[o+i+9]);
    }
  }

#endif /* LL_DESC */
#endif /* TRACE_EVE */
  /*
   * If we have build an response including all XID parameters,
   * we can clean all the xid_tags and layer 3 parameters.
   */
  if (cr_bit EQ MS_RESPONSE)
  {
    llc_data->u->xid_tag = 0;
    llc_data->requested_l3_xid->valid = FALSE;
  }

  return;
} /* u_insert_xid() */


/*
+------------------------------------------------------------------------------
| Function    : u_insert_frmr_information
+------------------------------------------------------------------------------
| Description : This procedure inserts (i.e. appends) an FRMR information
|               field into the given sdu. frame is the rejected frame of which
|               the control field is included (ctrl_length is truncated to a
|               maximum of six octets; if it is not known, length is
|               determinated from pdu_type), v_s and v_r are the current
|               send and receive state numbers of ITX/IRX, cr_bit indicates
|               if the frame was a command or response (SGSN_COMMAND/RESPONSE),
|               and reason indicates the reason of the frame rejection
|               condition (the lower nibble of reason is equivalent to W4-W1).
|
| Parameters  : sdu         - a valid pointer to an SDU, containing enough
|                             octets for the FRMR information field
|                             (U_FRMR_INFO_SIZE)
|               frame       - frame that caused the frame rejection condition
|               pdu_type    - frame type
|               ctrl_length - control field length, if known
|               v_s         - current V(S)
|               v_r         - current V(R)
|               cr_bit      - setting of C/R bit in frame
|               reason      - reason of the frame rejection condition (lower
|                             nibble conforms to W4-W1 in FRMR response)
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_insert_frmr_information
(
#ifndef LL_DESC
  T_sdu *sdu,
#else
  T_desc_list3 *desc_list3,
#endif
  T_LL_UNITDATA_IND *frame,
  T_PDU_TYPE pdu_type,
  USHORT ctrl_length,
  T_FRAME_NUM v_s,
  T_FRAME_NUM v_r,
  T_BIT cr_bit,
  UBYTE reason
)
{

#ifndef LL_DESC
  UBYTE             *sdu_data;
#else
  UBYTE             *help, *desc_data;
  T_desc3           *desc3;
#endif
  UBYTE             *ctrl;
  USHORT            octet;


  TRACE_FUNCTION( "u_insert_frmr_information" );

#ifndef LL_DESC
  /*
   * Set sdu_data to point to the first free data octet in sdu. sdu_data
   * has to be incremented with each octet that is written in sdu to ensure
   * that it always points to the first free data octet.
   */
  sdu_data = &sdu->buf[(sdu->o_buf/8)+(sdu->l_buf/8)];

  /*
   * Adjust length of SDU to include fixed sized FRMR information field.
   */
  sdu->l_buf += U_FRMR_INFO_SIZE * 8;

  /*
   * Set ctrl to point to the first octet of the faulty frame control field.
   */
  ctrl = &frame->sdu.buf[(frame->sdu.o_buf/8)+1];

  /*
   * If ctrl_length is unknown, the frame type is known, therefore the length
   * of the control field can be easily determined.
   */
  if (ctrl_length EQ FRMR_CTRL_LENGTH_UNKNOWN)
  {
    if (pdu_type EQ I_FRAME)
    {
      ctrl_length = I_CTRL_OCTETS;
    }
    else if (pdu_type EQ S_FRAME)
    {
      ctrl_length = S_CTRL_OCTETS;
    }
    else if (pdu_type EQ UI_FRAME)
    {
      ctrl_length = UI_CTRL_OCTETS;
    }
    else if (pdu_type EQ U_FRAME)
    {
      ctrl_length = U_CTRL_OCTETS;
    }
    else
    {
      /*
       * Undefined control field type, set control field length to SDU length
       * minus address field, limited to six octets.
       */
      TRACE_ERROR ("undefined control field type");
      ctrl_length = (sdu->l_buf/8 - 1) < 6 ? (sdu->l_buf/8 - 1) : 6;
    }
  }
  else /* ctrl_length is already known */
  {
    /*
     * Limit control field length to six octets, according to 04.64.
     */
    ctrl_length = ctrl_length < 6 ? ctrl_length : 6;
  }


  /*
   * Copy the first six octets of faulty control field. If the control field
   * is smaller that six octets, the unused octets are set to 0.
   */
  for (octet = 0; octet < 6; octet++)
  {
    if (octet < ctrl_length)
    {
      sdu_data[octet] = ctrl[octet];
    }
    else
    {
      sdu_data[octet] = 0x00;
    }
  }


  /*
   * Insert remaining four octets of FRMR information field:
   * +---+---+---+---+---+---+---+---+
   * | X | X | X | X |      V(S)     |
   * +---+---+---+---+---+---+---+---+
   * |        V(S)       | X |  V(R) |
   * +---+---+---+---+---+---+---+---+
   * |            V(R)           |C/R|
   * +---+---+---+---+---+---+---+---+
   * | X | X | X | X | W4| W3| W2| W1|
   * +---+---+---+---+---+---+---+---+
   */
  sdu_data[octet++] = (UBYTE)(v_s >> 5) & 0x0F;
  sdu_data[octet++] = (UBYTE)(v_s << 3) | ((UBYTE)(v_r >> 7) & 0x0003);
  sdu_data[octet++] = (UBYTE)(v_r << 1) | (cr_bit EQ SGSN_COMMAND ?
    0x00 : 0x01);
  sdu_data[octet++] = reason & 0x0F;

  return;
#else
  /*
   * Set sdu_data to point to the first free data octet in sdu. sdu_data
   * has to be incremented with each octet that is written in sdu to ensure
   * that it always points to the first free data octet.
   */
  desc3 = (T_desc3*)desc_list3->first;

  help = (U8*)desc3->buffer;
  desc_data = &help[desc3->offset + desc3->len];

  /*
   * Adjust length of SDU to include fixed sized FRMR information field.
   */
  desc3->len += U_FRMR_INFO_SIZE;
  desc_list3->list_len += U_FRMR_INFO_SIZE;

  /*
   * Set ctrl to point to the first octet of the faulty frame control field.
   */
  ctrl = &frame->sdu.buf[(frame->sdu.o_buf/8)+1];

  /*
   * If ctrl_length is unknown, the frame type is known, therefore the length
   * of the control field can be easily determined.
   */
  if (ctrl_length EQ FRMR_CTRL_LENGTH_UNKNOWN)
  {
    if (pdu_type EQ I_FRAME)
    {
      ctrl_length = I_CTRL_OCTETS;
    }
    else if (pdu_type EQ S_FRAME)
    {
      ctrl_length = S_CTRL_OCTETS;
    }
    else if (pdu_type EQ UI_FRAME)
    {
      ctrl_length = UI_CTRL_OCTETS;
    }
    else if (pdu_type EQ U_FRAME)
    {
      ctrl_length = U_CTRL_OCTETS;
    }
    else
    {
      /*
       * Undefined control field type, set control field length to SDU length
       * minus address field, limited to six octets.
       */
      TRACE_ERROR ("undefined control field type");
      ctrl_length = desc3->len - 1 < 6 ? (desc3->len - 1) : 6;
    }
  }
  else /* ctrl_length is already known */
  {
    /*
     * Limit control field length to six octets, according to 04.64.
     */
    ctrl_length = ctrl_length < 6 ? ctrl_length : 6;
  }


  /*
   * Copy the first six octets of faulty control field. If the control field
   * is smaller that six octets, the unused octets are set to 0.
   */
  for (octet = 0; octet < 6; octet++)
  {
    if (octet < ctrl_length)
    {
      desc_data[octet] = ctrl[octet];
    }
    else
    {
      desc_data[octet] = 0x00;
    }
  }


  /*
   * Insert remaining four octets of FRMR information field:
   * +---+---+---+---+---+---+---+---+
   * | X | X | X | X |      V(S)     |
   * +---+---+---+---+---+---+---+---+
   * |        V(S)       | X |  V(R) |
   * +---+---+---+---+---+---+---+---+
   * |            V(R)           |C/R|
   * +---+---+---+---+---+---+---+---+
   * | X | X | X | X | W4| W3| W2| W1|
   * +---+---+---+---+---+---+---+---+
   */
  desc_data[octet++] = (UBYTE)(v_s >> 5) & 0x0F;
  desc_data[octet++] = (UBYTE)(v_s << 3) | ((UBYTE)(v_r >> 7) & 0x0003);
  desc_data[octet++] = (UBYTE)(v_r << 1) | (cr_bit EQ SGSN_COMMAND ?
    0x00 : 0x01);
  desc_data[octet++] = reason & 0x0F;

  return;
#endif
} /* u_insert_frmr_information() */


/*
+------------------------------------------------------------------------------
| Function    : u_send_sabm
+------------------------------------------------------------------------------
| Description : This procedure allocates a LL_UNITDATA_REQ primitive, fills in
|               the required parameters, builds an U frame header containing
|               an SABM command, and sends this primitive to TX. _Before_ it is
|               sent to TX, T200 has to be started, because otherwise the
|               primitive may not be valid anymore.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_send_sabm (void)
{
  USHORT sdu_byte_len = 0;
#ifdef LL_DESC
  T_desc3 *desc3;
#endif

  TRACE_FUNCTION( "u_send_sabm" );

  u_tag_xid_parameters(0, TRUE);

  /*
   * Calculate bit length of tagged XID parameters. First, add
   * the size of each tagged parameter. The size of layer-3
   * parameters is variable, so add the given length in octets.
   */
  ADD_IF_TAGGED (sdu_byte_len, XID_VERSION);
  ADD_IF_TAGGED (sdu_byte_len, XID_T200);
  ADD_IF_TAGGED (sdu_byte_len, XID_N200);
  ADD_IF_TAGGED (sdu_byte_len, XID_N201_U);
  ADD_IF_TAGGED (sdu_byte_len, XID_N201_I);
  ADD_IF_TAGGED (sdu_byte_len, XID_MD);
  ADD_IF_TAGGED (sdu_byte_len, XID_MU);
  ADD_IF_TAGGED (sdu_byte_len, XID_KD);
  ADD_IF_TAGGED (sdu_byte_len, XID_KU);
  if (llc_data->u->xid_tag & (0x00000001L << XID_LAYER_3))
  {
    sdu_byte_len += llc_data->requested_l3_xid->length + 2;
  }

  /*
   * Add SDU header and FCS field to the XID response size to get
   * the overall SDU size.
   */
  sdu_byte_len += U_HDR_SIZE + FCS_SIZE;

#ifdef LL_DESC
  {
    PALLOC(ll_unitdesc_req, LL_UNITDESC_REQ);

    desc3 = llc_palloc_desc((USHORT)sdu_byte_len, 0); /* One desc3 descriptor and buffer allocated */

    ll_unitdesc_req->sapi = llc_data->current_sapi;
    ll_unitdesc_req->tlli = llc_data->u->current_tlli;
    /*
     * No attach to primitive necessary. For retransmission prim
     * data is copied in T200.
     */
    ll_unitdesc_req->attached_counter = CCI_NO_ATTACHE;
    /*
     * LLC does not know the QoS profile.
     */
    ll_unitdesc_req->ll_qos    = llc_data->cur_qos;

    TRACE_EVENT_P1("peak throughput = %d",llc_data->cur_qos.peak);

    /*
     * LLC signalling frames are always sent with highest priority.
     */
    ll_unitdesc_req->radio_prio = llc_data->cur_radio_prio;
    TRACE_EVENT_P1("radio priority = %d",llc_data->cur_radio_prio);
#ifdef REL99
    /* 
     * From 24.008 & 23.060 it is interpreted that for all signalling data, a 
     * predefined PFI LL_PFI_SIGNALING shall be used.
     */
    ll_unitdesc_req->pkt_flow_id = llc_data->cur_pfi;
    TRACE_EVENT_P1("packet flow id = %d",llc_data->cur_pfi);
#endif  /* REL99 */
    /*
     * Set parameter of descriptor list3
     */
    ll_unitdesc_req->desc_list3.first = (ULONG)desc3;
    ll_unitdesc_req->desc_list3.list_len = desc3->len;

    u_build_u_frame (&ll_unitdesc_req->desc_list3, MS_COMMAND,
      llc_data->current_sapi, 1, U_SABM);

    u_insert_xid (&ll_unitdesc_req->desc_list3, MS_COMMAND);

    /*
     * T200 has to be started _before_ the primitive is sent to TX, because
     * the primitive is copied by t200_start() and it may not be valid
     * anymore after sending to TX.
     */
    sig_u_t200_start_req (ll_unitdesc_req, GRLC_DTACS_DEF);

    sig_u_tx_data_req (ll_unitdesc_req, GRLC_DTACS_DEF);
  }

#else
  {
    PALLOC_SDU (ll_unitdata_req, LL_UNITDATA_REQ, (USHORT)(sdu_byte_len*8));

    ll_unitdata_req->sapi = llc_data->current_sapi;
    ll_unitdata_req->tlli = llc_data->u->current_tlli;
    /*
     * No attach to primitive necessary. For retransmission prim
     * data is copied in T200.
     */
    ll_unitdata_req->attached_counter = CCI_NO_ATTACHE;
    /*
     * LLC does not know the QoS profile.
     */
    ll_unitdata_req->ll_qos.peak = LL_PEAK_SUB;
    /*
     * LLC signalling frames are always sent with highest priority.
     */
    ll_unitdata_req->radio_prio = LL_RADIO_PRIO_1;
#ifdef REL99 
    /*
     * From 24.008 & 23.060 it is interpreted that for all signalling data, a
     * predefined PFI LL_PFI_SIGNALING shall be used.
     */
    ll_unitdata_req->pkt_flow_id = LL_PFI_SIGNALING;
#endif  /* REL99 */

    u_build_u_frame (&ll_unitdata_req->sdu, MS_COMMAND,
      llc_data->current_sapi, 1, U_SABM);

    u_insert_xid (&ll_unitdata_req->sdu, MS_COMMAND);

    /*
     * T200 has to be started _before_ the primitive is sent to TX, because
     * the primitive is copied by t200_start() and it may not be valid
     * anymore after sending to TX.
     */
    sig_u_t200_start_req (ll_unitdata_req, GRLC_DTACS_DEF);

    sig_u_tx_data_req (ll_unitdata_req, GRLC_DTACS_DEF);
  }
#endif
  return;
} /* u_send_sabm() */


/*
+------------------------------------------------------------------------------
| Function    : u_send_disc
+------------------------------------------------------------------------------
| Description : This procedure allocates a LL_UNITDATA_REQ primitive, fills in
|               the required parameters, builds an U frame header containing
|               an DISC command, and sends this primitive to TX. _Before_ it is
|               sent to TX, T200 has to be started, because otherwise the
|               primitive may not be valid anymore.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_send_disc (void)
{
#ifdef LL_DESC
  T_desc3* desc3;
#endif
  TRACE_FUNCTION( "u_send_disc" );

  {
#ifndef LL_DESC
    PALLOC_SDU (ll_unitdesc_req, LL_UNITDATA_REQ,
      U_HDR_SIZE_BITS + FCS_SIZE_BITS);
#else
    PALLOC (ll_unitdesc_req, LL_UNITDESC_REQ);

    desc3 = llc_palloc_desc((U_HDR_SIZE_BITS + FCS_SIZE_BITS)/8, 0);
    ll_unitdesc_req->desc_list3.first    = (ULONG)desc3;
    ll_unitdesc_req->desc_list3.list_len = desc3->len;
#endif

    ll_unitdesc_req->sapi = llc_data->current_sapi;
    ll_unitdesc_req->tlli = llc_data->u->current_tlli;
    /*
     * No attach to primitive necessary. For retransmission prim
     * data is copied in T200.
     */
    ll_unitdesc_req->attached_counter = CCI_NO_ATTACHE;
    /*
     * LLC does not know the QoS profile.
     */
    ll_unitdesc_req->ll_qos    = llc_data->cur_qos;

    TRACE_EVENT_P1("peak throughput = %d",llc_data->cur_qos.peak);

    /*
     * LLC signalling frames are always sent with highest priority.
     */
    ll_unitdesc_req->radio_prio = llc_data->cur_radio_prio;
    TRACE_EVENT_P1("radio priority = %d",llc_data->cur_radio_prio);

#ifdef REL99
    /* 
     * From 24.008 & 23.060 it is interpreted that for all signalling data, a 
     * predefined PFI LL_PFI_SIGNALING shall be used.
     */
    ll_unitdesc_req->pkt_flow_id = llc_data->cur_pfi;
    TRACE_EVENT_P1("packet flow id = %d",llc_data->cur_pfi);

#endif  /* REL99 */


    u_build_u_frame
      (
#ifndef LL_DESC
      &ll_unitdesc_req->sdu,
#else
      &ll_unitdesc_req->desc_list3,
#endif
      MS_COMMAND,
      llc_data->current_sapi, 1, U_DISC
      );

    /*
     * T200 has to be started _before_ the primitive is sent to TX, because
     * the primitive is copied by t200_start() and it may not be valid
     * anymore after sending to TX.
     */
    sig_u_t200_start_req (ll_unitdesc_req, GRLC_DTACS_DEF);

    sig_u_tx_data_req (ll_unitdesc_req, GRLC_DTACS_DEF);
  }

  return;
} /* u_send_disc() */

#ifdef REL99 
/*
+------------------------------------------------------------------------------
| Function    : sig_tx_u_send_null
+------------------------------------------------------------------------------
| Description : This procedure allocates a LL_UNITDATA_REQ primitive, fills in
|               the required parameters, builds an U frame header containing
|               an NULL command, and sends this primitive to TX.
|
| Parameters  : cause - frame cause indicates grr about cell notification
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tx_u_send_null (UBYTE cause)
{
#ifdef LL_DESC
  T_desc3* desc3;
#endif
  TRACE_FUNCTION( "sig_tx_u_send_null" );

  {
#ifndef LL_DESC
    PALLOC_SDU (ll_unitdesc_req, LL_UNITDATA_REQ,
      U_HDR_SIZE_BITS + FCS_SIZE_BITS);
#else
    PALLOC (ll_unitdesc_req, LL_UNITDESC_REQ);
    desc3 = llc_palloc_desc((U_HDR_SIZE_BITS + FCS_SIZE_BITS)/8, 0);
    ll_unitdesc_req->desc_list3.first    = (ULONG)desc3;
    ll_unitdesc_req->desc_list3.list_len = desc3->len;
#endif
    /*
     * Set TLLI for current transaction.
     */
    llc_data->u->current_tlli = llc_data->tlli_new;

    ll_unitdesc_req->sapi = llc_data->current_sapi;
    ll_unitdesc_req->tlli = llc_data->u->current_tlli;
    /*
     * No attach to primitive necessary. For retransmission prim
     * data is copied in T200.
     */
    ll_unitdesc_req->attached_counter = CCI_NO_ATTACHE;
    /*
     * LLC does not know the QoS profile.
     */
    ll_unitdesc_req->ll_qos.peak = GRLC_PEAK_SUB;
    /*
     * LLC signalling frames are always sent with highest priority.
     */
    ll_unitdesc_req->radio_prio = GRLC_RADIO_PRIO_1;

    u_build_u_frame
    (
#ifndef LL_DESC
      &ll_unitdesc_req->sdu,
#else
      &ll_unitdesc_req->desc_list3,
#endif
      MS_COMMAND,
      llc_data->current_sapi, 0, U_NULL
     );

    sig_u_tx_data_req (ll_unitdesc_req, cause);
  }
  return;
} /* sig_tx_u_send_null() */

#endif /* REL99 */
/*
+------------------------------------------------------------------------------
| Function    : u_send_ua
+------------------------------------------------------------------------------
| Description : This procedure allocates a LL_UNITDATA_REQ primitive, fills in
|               the required parameters, builds an U frame header containing
|               an UA response with the given pf_bit setting, and sends this
|               primitive to TX. The parameter include_xid indicates if an
|               XID information field (according to llc_xid_tag) shall be
|               included (=TRUE) or not.
|
| Parameters  : pf_bit      - setting of the P/F bit in the received command
|               include_xid - include XID information field in frame, or not
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_send_ua (T_BIT pf_bit,
                       BOOL include_xid)
{
#ifdef LL_DESC
T_desc3*  desc3;
#endif
  USHORT byte_len = 0;


  TRACE_FUNCTION( "u_send_ua" );

  if (include_xid EQ TRUE)
  {

    u_tag_xid_parameters(1, TRUE);

    /*
     * Calculate bit length of tagged XID parameters. First, add
     * the size of each tagged parameter. The size of layer-3
     * parameters is variable, so add the given length in octets.
     */
    ADD_IF_TAGGED (byte_len, XID_VERSION);
    ADD_IF_TAGGED (byte_len, XID_T200);
    ADD_IF_TAGGED (byte_len, XID_N200);
    ADD_IF_TAGGED (byte_len, XID_N201_U);
    ADD_IF_TAGGED (byte_len, XID_N201_I);
    ADD_IF_TAGGED (byte_len, XID_MD);
    ADD_IF_TAGGED (byte_len, XID_MU);
    ADD_IF_TAGGED (byte_len, XID_KD);
    ADD_IF_TAGGED (byte_len, XID_KU);
    if (llc_data->u->xid_tag & (0x00000001L << XID_LAYER_3))
    {
      byte_len += llc_data->requested_l3_xid->length + 2;
    }
  }

  /*
   * Add data header and FCS field to the XID response size to get
   * the overall data size.
   */
  byte_len += U_HDR_SIZE + FCS_SIZE;

  {
#ifndef LL_DESC
    PALLOC_SDU (ll_unitdesc_req, LL_UNITDATA_REQ, (USHORT)(byte_len*8));
#else
    PALLOC (ll_unitdesc_req, LL_UNITDESC_REQ);

    desc3 = llc_palloc_desc((USHORT)(byte_len), 0);
    ll_unitdesc_req->desc_list3.first    = (ULONG)desc3;
    ll_unitdesc_req->desc_list3.list_len = desc3->len;

#endif

    ll_unitdesc_req->sapi = llc_data->current_sapi;
    ll_unitdesc_req->tlli = llc_data->u->current_tlli;
    /*
     * No attach to primitive necessary. There is no retransmission.
     */
    ll_unitdesc_req->attached_counter = CCI_NO_ATTACHE;
    /*
     * LLC does not know the QoS profile.
     */
    ll_unitdesc_req->ll_qos    = llc_data->cur_qos;
    TRACE_EVENT_P1("peak throughput = %d",llc_data->cur_qos.peak);

    /*
     * LLC signalling frames are always sent with highest priority.
     */
    ll_unitdesc_req->radio_prio = llc_data->cur_radio_prio;
    TRACE_EVENT_P1("radio priority = %d",llc_data->cur_radio_prio);

#ifdef REL99 
    /*
     * From 24.008 & 23.060 it is interpreted that for all signalling data, a
     * predefined PFI LL_PFI_SIGNALING shall be used.
     */
    ll_unitdesc_req->pkt_flow_id = llc_data->cur_pfi;
    TRACE_EVENT_P1("packet flow id = %d",llc_data->cur_pfi);
#endif  /* REL99 */

    u_build_u_frame
      (
#ifndef LL_DESC
      &ll_unitdesc_req->sdu,
#else
      &ll_unitdesc_req->desc_list3,
#endif
      MS_RESPONSE, llc_data->current_sapi, pf_bit, U_UA
      );

    if (include_xid EQ TRUE)
    {
      u_insert_xid
        (
#ifndef LL_DESC
        &ll_unitdesc_req->sdu,
#else
        &ll_unitdesc_req->desc_list3,
#endif
        MS_RESPONSE
        );
    }

    sig_u_tx_data_req (ll_unitdesc_req, GRLC_DTACS_DEF);
  }

  llc_init_requested_xid_sapi(llc_data->current_sapi);
  return;
} /* u_send_ua() */


/*
+------------------------------------------------------------------------------
| Function    : u_send_dm
+------------------------------------------------------------------------------
| Description : This procedure allocates a LL_UNITDATA_REQ primitive, fills in
|               all required parameters, builds an U frame header containing a
|               DM response with the given pf_bit setting, and sends this
|               primitive to TX.
|
| Parameters  : pf_bit  - setting of the P/F bit in the received command
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_send_dm (T_BIT pf_bit)
{
#ifdef LL_DESC
  T_desc3* desc3;
#endif

  TRACE_FUNCTION( "u_send_dm" );

  {
#ifndef LL_DESC
    PALLOC_SDU (ll_unitdesc_req, LL_UNITDATA_REQ,
      U_DM_SIZE_BITS + FCS_SIZE_BITS);
#else
    PALLOC (ll_unitdesc_req, LL_UNITDESC_REQ);

    desc3 = llc_palloc_desc((U_DM_SIZE_BITS + FCS_SIZE_BITS)/8, 0);
    ll_unitdesc_req->desc_list3.first    = (ULONG)desc3;
    ll_unitdesc_req->desc_list3.list_len = desc3->len;
#endif

    ll_unitdesc_req->sapi = llc_data->current_sapi;
    ll_unitdesc_req->tlli = llc_data->u->current_tlli;
    /*
     * No attach to primitive necessary. There is no retransmission.
     */
    ll_unitdesc_req->attached_counter = CCI_NO_ATTACHE;
    /*
     * LLC does not know the QoS profile.
     */
    ll_unitdesc_req->ll_qos    = llc_data->cur_qos;

    TRACE_EVENT_P1("peak throughput = %d",llc_data->cur_qos.peak);


    /*
     * LLC signalling frames are always sent with highest priority.
     */
    ll_unitdesc_req->radio_prio = llc_data->cur_radio_prio;
    TRACE_EVENT_P1("radio priority = %d",llc_data->cur_radio_prio);

#ifdef REL99
    /* 
     * From 24.008 & 23.060 it is interpreted that for all signalling data, a 
     * predefined PFI LL_PFI_SIGNALING shall be used.
     */
    ll_unitdesc_req->pkt_flow_id = llc_data->cur_pfi;
    TRACE_EVENT_P1("packet flow id = %d",llc_data->cur_pfi);

#endif  /* REL99 */

    u_build_u_frame
      (
#ifndef LL_DESC
      &ll_unitdesc_req->sdu,
#else
      &ll_unitdesc_req->desc_list3,
#endif
      MS_RESPONSE,
      llc_data->current_sapi, pf_bit, U_DM
      );

    sig_u_tx_data_req (ll_unitdesc_req, GRLC_DTACS_DEF);
  }

  return;
} /* u_send_dm() */


/*
+------------------------------------------------------------------------------
| Function    : u_send_frmr
+------------------------------------------------------------------------------
| Description : This procedure allocates a LL_UNITDATA_REQ primitive, fills in
|               the required parameters, builds an U frame header containing
|               an FRMR response, fills in the FRMR information field based on
|               the given frame, vs, vr, the cr_bit setting of frame
|               (SGSN_COMMAND/REPONSE), and the reason of the frame rejection
|               condition (lower nibble conforms to W4-W1 in FRMR response).
|               This primitive is then sent to TX.
|
| Parameters  : frame       - frame that caused the frame rejection condition
|               pdu_type    - frame type
|               ctrl_length - control field length, if known
|               vs          - current V(S)
|               vr          - current V(R)
|               cr_bit      - setting of C/R bit in frame
|               reason      - reason of the frame rejection condition (lower
|                             nibble conforms to W4-W1 in FRMR response)
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_send_frmr (T_LL_UNITDATA_IND *frame,
                         T_PDU_TYPE pdu_type,
                         USHORT ctrl_length,
                         T_FRAME_NUM vs,
                         T_FRAME_NUM vr,
                         T_BIT cr_bit,
                         UBYTE reason)
{
#ifdef LL_DESC
  T_desc3* desc3;
#endif

  TRACE_FUNCTION( "u_send_frmr" );

  {
#ifndef LL_DESC
    PALLOC_SDU (ll_unitdesc_req, LL_UNITDATA_REQ,
    U_FRMR_SIZE_BITS + FCS_SIZE_BITS);
#else
    PALLOC (ll_unitdesc_req, LL_UNITDESC_REQ);

    desc3 = llc_palloc_desc((U_FRMR_SIZE_BITS + FCS_SIZE_BITS)/8, 0);
    ll_unitdesc_req->desc_list3.first    = (ULONG)desc3;
    ll_unitdesc_req->desc_list3.list_len = desc3->len;
#endif

    ll_unitdesc_req->sapi = llc_data->current_sapi;
    ll_unitdesc_req->tlli = llc_data->u->current_tlli;
    /*
     * No attach to primitive necessary. There is no retransmission.
     */
    ll_unitdesc_req->attached_counter = CCI_NO_ATTACHE;
    /*
     * LLC does not know the QoS profile.
     */
    ll_unitdesc_req->ll_qos    = llc_data->cur_qos;

    TRACE_EVENT_P1("peak throughput = %d",llc_data->cur_qos.peak);


    /*
     * LLC signalling frames are always sent with highest priority.
     */
    ll_unitdesc_req->radio_prio = llc_data->cur_radio_prio;
    TRACE_EVENT_P1("radio priority = %d",llc_data->cur_radio_prio);

#ifdef REL99
    /* 
     * From 24.008 & 23.060 it is interpreted that for all signalling data, a 
     * predefined PFI LL_PFI_SIGNALING shall be used.
     */
    ll_unitdesc_req->pkt_flow_id = llc_data->cur_pfi;
    TRACE_EVENT_P1("packet flow id = %d",llc_data->cur_pfi);

#endif  /* REL99 */

    u_build_u_frame
      (
#ifndef LL_DESC
      &ll_unitdesc_req->sdu,
#else
      &ll_unitdesc_req->desc_list3,
#endif
      MS_RESPONSE,
      llc_data->current_sapi, 1, U_FRMR
      );

    u_insert_frmr_information
      (
#ifndef LL_DESC
      &ll_unitdesc_req->sdu,
#else
      &ll_unitdesc_req->desc_list3,
#endif
      frame, pdu_type,
      ctrl_length, vs, vr, cr_bit, reason
      );

    sig_u_tx_data_req (ll_unitdesc_req, GRLC_DTACS_DEF);
  }

  return;
} /* u_send_frmr() */


/*
+------------------------------------------------------------------------------
| Function    : u_send_llgmm_status_ind
+------------------------------------------------------------------------------
| Description : This procedure allocates an LLGMM_STATUS_IND primitive, fills
|               in the error_cause parameter, and sends this primitive to GMM.
|
| Parameters  : error_cause - LLGMM error cause
|
+------------------------------------------------------------------------------
*/
#ifdef CC_CONCEPT
GLOBAL void u_send_llgmm_status_ind (USHORT error_cause)
#else
GLOBAL void u_send_llgmm_status_ind (UBYTE error_cause)
#endif
{
  TRACE_FUNCTION( "u_send_llgmm_status_ind" );

  {
    PALLOC (llgmm_status_ind, LLGMM_STATUS_IND); /* T_LLGMM_STATUS_IND */

#ifdef LL_2to1
    llgmm_status_ind->ps_cause.ctrl_value = CAUSE_is_from_llc;
    llgmm_status_ind->ps_cause.value.llc_cause = error_cause;
#else
    llgmm_status_ind->error_cause = error_cause;
#endif

    PSEND (hCommGMM, llgmm_status_ind);
  }

  return;
} /* u_send_llgmm_status_ind() */


/*
+------------------------------------------------------------------------------
| Function    : u_send_ll_release_cnf
+------------------------------------------------------------------------------
| Description : This procedure allocates an LL_RELEASE_CNF primitive, fills in
|               tlli and sapi parameters, and sends this primitive to SNDCP.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_send_ll_release_cnf (void)
{
  TRACE_FUNCTION( "u_send_ll_release_cnf" );

  {
    PALLOC (ll_release_cnf, LL_RELEASE_CNF);

    ll_release_cnf->sapi = llc_data->current_sapi;
    TRACE_1_OUT_PARA("s:%d", llc_data->current_sapi );
    PSEND (hCommSNDCP, ll_release_cnf);
  }

  llc_data->u->release_requested = FALSE;

  return;
} /* u_send_ll_release_cnf() */



/*
+------------------------------------------------------------------------------
| Function    : u_send_ll_status_ind
+------------------------------------------------------------------------------
| Description : This procedure allocates an LL_STATUS_IND primitive, fills in
|               tlli, sapi and cause parameters and sends this proimitive to L3
|
| Parameters  : cause - LL error cause
|
+------------------------------------------------------------------------------
*/
#ifdef CC_CONCEPT
GLOBAL void u_send_ll_status_ind (USHORT cause)
#else
GLOBAL void u_send_ll_status_ind (UBYTE cause)
#endif
{
  TRACE_FUNCTION( "u_send_ll_status_ind" );

  {
    PALLOC (ll_status_ind, LL_STATUS_IND);

    ll_status_ind->sapi        = llc_data->current_sapi;
#ifdef LL_2to1
    ll_status_ind->ps_cause.ctrl_value = CAUSE_is_from_llc;
    ll_status_ind->ps_cause.value.llc_cause = cause;
#else
    ll_status_ind->error_cause = cause;
#endif

    TRACE_1_OUT_PARA("s:%d", llc_data->current_sapi );
    PSEND (hCommSNDCP, ll_status_ind);
  }

  return;
} /* u_send_ll_status_ind() */



/*
+------------------------------------------------------------------------------
| Function    : u_send_ll_xid_cnf
+------------------------------------------------------------------------------
| Description : This procedure allocates an LL_XID_CNF primitive, fills in
|               tlli, sapi and L3 parameter (if being negotiated), and sends
|               this primitive to SNDCP
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_send_ll_xid_cnf (void)
{
  TRACE_FUNCTION( "u_send_ll_xid_cnf" );

  if (llc_data->decoded_l3_xid.valid EQ TRUE)
  {
    PALLOC_SDU (ll_xid_cnf, LL_XID_CNF, (USHORT)(llc_data->decoded_l3_xid.length * 8));

    ll_xid_cnf->sdu.l_buf = llc_data->decoded_l3_xid.length * 8;
    ll_xid_cnf->sdu.o_buf = 0;

    memcpy (ll_xid_cnf->sdu.buf,
            llc_data->decoded_l3_xid.value,
            llc_data->decoded_l3_xid.length);

    ll_xid_cnf->sapi      = llc_data->current_sapi;

    ll_xid_cnf->n201_u    = *(llc_data->n201_u);
    ll_xid_cnf->n201_i    = *(llc_data->n201_i);

    TRACE_2_OUT_PARA("s:%d xid-len:%d", llc_data->current_sapi, llc_data->decoded_l3_xid.length );
    PSEND (hCommSNDCP, ll_xid_cnf);
  }
  else
  {
    PALLOC_SDU (ll_xid_cnf, LL_XID_CNF, 0);

    ll_xid_cnf->sdu.l_buf = 0;
    ll_xid_cnf->sdu.o_buf = 0;

    ll_xid_cnf->sapi      = llc_data->current_sapi;

    ll_xid_cnf->n201_u    = *(llc_data->n201_u);
    ll_xid_cnf->n201_i    = *(llc_data->n201_i);

    TRACE_2_OUT_PARA("s:%d xid-len:%d", llc_data->current_sapi, 0 );
    PSEND (hCommSNDCP, ll_xid_cnf);
  }

  return;
} /* u_send_ll_xid_cnf() */


/*
+------------------------------------------------------------------------------
| Function    : u_send_ll_establish_ind
+------------------------------------------------------------------------------
| Description : This procedure allocates an LL_ESTABLISH_IND primitive, fills
|               in all necessary parameters using global LLC values (or in case
|               of L-3 XID parameters, using the parameters of received XID
|               cmd/res), and sends this primitive to SNDCP
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_send_ll_establish_ind (void)
{
  USHORT len;

  TRACE_FUNCTION( "u_send_ll_establish_ind" );

  if (llc_data->decoded_l3_xid.valid EQ TRUE)
  {
    len = (llc_data->decoded_l3_xid.length * 8);
  }
  else
  {
    len = 0;
  }

  {
    PALLOC_SDU (ll_establish_ind, LL_ESTABLISH_IND, len);

    if (llc_data->decoded_l3_xid.valid EQ TRUE)
    {
      ll_establish_ind->xid_valid = LL_XID_VALID;

      ll_establish_ind->sdu.o_buf = 0;
      ll_establish_ind->sdu.l_buf = llc_data->decoded_l3_xid.length * 8;

      memcpy (ll_establish_ind->sdu.buf,
              llc_data->decoded_l3_xid.value,
              llc_data->decoded_l3_xid.length);
    }
    else
    {
      ll_establish_ind->xid_valid = LL_XID_INVALID;
    }

    ll_establish_ind->sapi      = llc_data->current_sapi;

    /*
     * evaluate N201_U
     */
    if (llc_data->u->requested_xid.n201_u.valid AND
        llc_data->decoded_xid.n201_u.valid      AND
        !llc_xid_value_acceptable (llc_data->current_sapi, XID_N201_U,
        llc_data->decoded_xid.n201_u.value))
    {
       ll_establish_ind->n201_u = llc_data->u->requested_xid.n201_u.value;
    }
    else if (llc_data->u->requested_xid.n201_u.valid AND
             !llc_data->decoded_xid.n201_u.valid     AND
             !llc_xid_value_acceptable (llc_data->current_sapi, XID_N201_U,
             *(llc_data->n201_u)))
    {
       ll_establish_ind->n201_u = llc_data->u->requested_xid.n201_u.value;
    }
    else
    {
       ll_establish_ind->n201_u    = *(llc_data->n201_u);
    }

    /*
     * evaluate N201_I
     */
    if (llc_data->u->requested_xid.n201_i.valid AND
        llc_data->decoded_xid.n201_i.valid      AND
        !llc_xid_value_acceptable (llc_data->current_sapi, XID_N201_I,
        llc_data->decoded_xid.n201_i.value))
    {
       ll_establish_ind->n201_i = llc_data->u->requested_xid.n201_i.value;
    }
    else if (llc_data->u->requested_xid.n201_i.valid AND
             !llc_data->decoded_xid.n201_i.valid     AND
             !llc_xid_value_acceptable (llc_data->current_sapi, XID_N201_I,
             *(llc_data->n201_i)))
    {
       ll_establish_ind->n201_i = llc_data->u->requested_xid.n201_i.value;
    }
    else
    {
       ll_establish_ind->n201_i = *(llc_data->n201_i);
    }


    TRACE_2_OUT_PARA("s:%d xid-len:%d", ll_establish_ind->sapi, len/8);
    PSEND (hCommSNDCP, ll_establish_ind);

    return;
  }
} /* u_send_ll_establish_ind() */



/*
+------------------------------------------------------------------------------
| Function    : u_send_ll_establish_cnf
+------------------------------------------------------------------------------
| Description : This procedure allocates an LL_ESTABLISH_CNF primitive, fills
|               in all necessary parameters using global LLC values (or in case
|               of L-3 XID parameters, using the parameters of received XID
|               cmd/res), and sends this primitive to SNDCP
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_send_ll_establish_cnf (void)
{
  TRACE_FUNCTION( "u_send_ll_establish_cnf" );

  if (llc_data->decoded_l3_xid.valid EQ TRUE)
  {
    PALLOC_SDU (ll_establish_cnf, LL_ESTABLISH_CNF,
                                  (USHORT)(llc_data->decoded_l3_xid.length * 8));

    ll_establish_cnf->xid_valid = LL_XID_VALID;

    ll_establish_cnf->sdu.o_buf = 0;
    ll_establish_cnf->sdu.l_buf = llc_data->decoded_l3_xid.length * 8;

    memcpy (ll_establish_cnf->sdu.buf,
            llc_data->decoded_l3_xid.value,
            llc_data->decoded_l3_xid.length);

    ll_establish_cnf->sapi      = llc_data->current_sapi;

    ll_establish_cnf->n201_u    = *(llc_data->n201_u);
    ll_establish_cnf->n201_i    = *(llc_data->n201_i);

    TRACE_2_OUT_PARA("s:%d xid-len:%d", llc_data->current_sapi, llc_data->decoded_l3_xid.length );
    PSEND (hCommSNDCP, ll_establish_cnf);
  }
  else
  {
    PALLOC_SDU (ll_establish_cnf, LL_ESTABLISH_CNF, 0);

    ll_establish_cnf->xid_valid = LL_XID_INVALID;

    ll_establish_cnf->sapi      = llc_data->current_sapi;

    ll_establish_cnf->n201_u    = *(llc_data->n201_u);
    ll_establish_cnf->n201_i    = *(llc_data->n201_i);

    TRACE_2_OUT_PARA("s:%d xid-len:%d", llc_data->current_sapi, 0 );
    PSEND (hCommSNDCP, ll_establish_cnf);
  }

  return;
} /* u_send_ll_establish_cnf() */


/*
+------------------------------------------------------------------------------
| Function    : u_send_ll_release_ind
+------------------------------------------------------------------------------
| Description : This procedure allocates an LL_RELEASE_IND primitive, fills
|               in tlli, sapi and cause parameters, and sends this primitive
|               to SNDCP
|
| Parameters  : cause - LL error cause
|
+------------------------------------------------------------------------------
*/
#ifdef CC_CONCEPT
GLOBAL void u_send_ll_release_ind (USHORT cause)
#else
GLOBAL void u_send_ll_release_ind (UBYTE cause)
#endif
{
  TRACE_FUNCTION( "u_send_ll_release_ind" );

  {
    PALLOC (ll_release_ind, LL_RELEASE_IND);

    ll_release_ind->sapi      = llc_data->current_sapi;
#ifdef LL_2to1
    ll_release_ind->ps_cause.ctrl_value = CAUSE_is_from_llc;
    ll_release_ind->ps_cause.value.llc_cause = cause;
#else
    ll_release_ind->cause     = cause;
#endif

    TRACE_1_OUT_PARA("s:%d", llc_data->current_sapi );
    PSEND (hCommSNDCP, ll_release_ind);
  }

  llc_data->u->release_requested = FALSE;

  return;
} /* u_send_ll_release_ind() */



/*
+------------------------------------------------------------------------------
| Function    : u_send_ll_xid_ind
+------------------------------------------------------------------------------
| Description : This procedure allocates an LL_XID_IND primitive, fills
|               in all necessary parameters using global LLC values (or in case
|               of L-3 XID parameters, using the parameters of received XID
|               cmd/res), and sends this primitive to SNDCP
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_send_ll_xid_ind (void)
{
  TRACE_FUNCTION( "u_send_ll_xid_ind" );



  if (llc_data->decoded_l3_xid.valid EQ TRUE)
  {
    PALLOC_SDU (ll_xid_ind, LL_XID_IND, (USHORT)(llc_data->decoded_l3_xid.length * 8));

    llc_data->u->ll_xid_resp_pending = TRUE;
    ll_xid_ind->xid_valid = LL_XID_VALID;

    ll_xid_ind->sdu.o_buf = 0;
    ll_xid_ind->sdu.l_buf = llc_data->decoded_l3_xid.length * 8;

    memcpy (ll_xid_ind->sdu.buf,
            llc_data->decoded_l3_xid.value,
            llc_data->decoded_l3_xid.length);

    ll_xid_ind->sapi      = llc_data->current_sapi;

    if (llc_data->u->requested_xid.n201_u.valid AND
        !llc_xid_value_acceptable (llc_data->current_sapi, XID_N201_U,
        llc_data->decoded_xid.n201_u.value))
    {
       ll_xid_ind->n201_u = llc_data->u->requested_xid.n201_u.value;
    }
    else
    {
      ll_xid_ind->n201_u  = *(llc_data->n201_u);
    }

    switch (ll_xid_ind->sapi)
    {
      case LL_SAPI_3:
      case LL_SAPI_5:
      case LL_SAPI_9:
      case LL_SAPI_11:
        if (llc_data->u->requested_xid.n201_i.valid AND
            !llc_xid_value_acceptable (llc_data->current_sapi, XID_N201_I,
            llc_data->decoded_xid.n201_i.value))
        {
          ll_xid_ind->n201_i = llc_data->u->requested_xid.n201_i.value;
        }
        else
        {
          ll_xid_ind->n201_i = *(llc_data->n201_i);
        }
        break;

      default:
        ll_xid_ind->n201_i = 0;
        break;
    }

    /*
     * Send primitive LL_XID_IND to either GMM, SNDCP, or GSMS,
     * depending on SAPI.
     */
    switch (ll_xid_ind->sapi)
    {
      case LL_SAPI_1:
        TRACE_PRIM_TO("GMM");
        TRACE_2_OUT_PARA("s:%d xid-len:%d", ll_xid_ind->sapi, llc_data->decoded_l3_xid.length);
        PSEND (hCommGMM, ll_xid_ind);
        break;
      case LL_SAPI_3:
      case LL_SAPI_5:
      case LL_SAPI_9:
      case LL_SAPI_11:
        TRACE_2_OUT_PARA("s:%d xid-len:%d", ll_xid_ind->sapi, llc_data->decoded_l3_xid.length);
        PSEND (hCommSNDCP, ll_xid_ind);
        break;
      case LL_SAPI_7:
#ifdef LL_2to1
        TRACE_PRIM_TO("MM");
        TRACE_2_OUT_PARA("s:%d xid-len:%d", ll_xid_ind->sapi, llc_data->decoded_l3_xid.length);
        PSEND (hCommMM, ll_xid_ind);
#else
        TRACE_PRIM_TO("GSMS");
        TRACE_2_OUT_PARA("s:%d xid-len:%d", ll_xid_ind->sapi, llc_data->decoded_l3_xid.length);
        PSEND (hCommGSMS, ll_xid_ind);
#endif
        break;
      default:
        PFREE (ll_xid_ind);
        TRACE_ERROR ("Invalid global SAPI value");
        break;
    }
  }
  else
  {
    PALLOC_SDU (ll_xid_ind, LL_XID_IND, 0);

    llc_data->u->ll_xid_resp_pending = FALSE;

    ll_xid_ind->xid_valid = LL_XID_INVALID;

    ll_xid_ind->sapi      = llc_data->current_sapi;

    ll_xid_ind->n201_u    = *(llc_data->n201_u);

    switch (ll_xid_ind->sapi)
    {
      case LL_SAPI_3:
      case LL_SAPI_5:
      case LL_SAPI_9:
      case LL_SAPI_11:
        ll_xid_ind->n201_i = *(llc_data->n201_i);
        break;

      default:
        ll_xid_ind->n201_i = 0;
        break;
    }

    /*
     * Send primitive LL_XID_IND to either GMM, SNDCP, or GSMS,
     * depending on SAPI.
     */
    switch (ll_xid_ind->sapi)
    {
      case LL_SAPI_1:
        TRACE_PRIM_TO("GMM");
        TRACE_1_OUT_PARA("s:%d no l3-xid", ll_xid_ind->sapi);
        PSEND (hCommGMM, ll_xid_ind);
        break;
      case LL_SAPI_3:
      case LL_SAPI_5:
      case LL_SAPI_9:
      case LL_SAPI_11:
        TRACE_1_OUT_PARA("s:%d no l3-xid", ll_xid_ind->sapi);
        PSEND (hCommSNDCP, ll_xid_ind);
        break;
      case LL_SAPI_7:
#ifdef LL_2to1
        TRACE_PRIM_TO("MM");
        TRACE_1_OUT_PARA("s:%d no l3-xid", ll_xid_ind->sapi);
        PSEND (hCommMM, ll_xid_ind);
#else
        TRACE_PRIM_TO("GSMS");
        TRACE_1_OUT_PARA("s:%d no l3-xid", ll_xid_ind->sapi);
        PSEND (hCommGSMS, ll_xid_ind);
#endif
        break;
      default:
        PFREE (ll_xid_ind);
        TRACE_ERROR ("invalid global SAPI value");
        break;
    }
  }

  return;
} /* u_send_ll_xid_ind() */


/*
+------------------------------------------------------------------------------
| Function    : u_tag_xid_parameters
+------------------------------------------------------------------------------
| Description : This procedure tags requested parameters
|
|
| Parameters  :
|               - send_ack_para- used to include/ignore the ack para in XID-FRAME
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_tag_xid_parameters (T_BIT cr_bit, BOOL send_ack_para)
{
  TRACE_FUNCTION( "u_tag_xid_parameters" );

  if (cr_bit EQ MS_RESPONSE)
  {
    /*
     * Do some special handling in case we want to set mD or mU to zero
     * (= disable buffered bytes counting) and the peer does not.
     */
    if (llc_data->decoded_xid.mu.valid)
    {
      u_handle_mX_zero_rsp ( llc_data->decoded_xid.mu.value,
                             llc_data->n201_i,
                             *(llc_data->mu), XID_MU_MAX,
                             llc_data->ku,    XID_KU_MIN,  XID_KU );
    }

    if (llc_data->decoded_xid.md.valid)
    {
      u_handle_mX_zero_rsp ( llc_data->decoded_xid.md.value,
                             llc_data->n201_i,
                             *(llc_data->md), XID_MD_MAX,
                             llc_data->kd,    XID_KD_MIN,  XID_KD );
    }
  }

  /*
   * Tag all possible requested XID parameter
   */
  if (cr_bit EQ MS_COMMAND)
  {
    /*
     * In case we are sending a command, we can include all
     * parameters, as long they are valid.
     */
    TAG_IF_REQUESTED (u->requested_xid.version. , XID_VERSION);
    TAG_IF_REQUESTED (u->requested_xid.t200. ,    XID_T200);
    TAG_IF_REQUESTED (u->requested_xid.n200. ,    XID_N200);
    TAG_IF_REQUESTED (u->requested_xid.n201_u. ,  XID_N201_U);
    TAG_IF_REQUESTED (requested_l3_xid-> ,        XID_LAYER_3);
    if(send_ack_para)
    {
      TRACE_EVENT("COMMAND: ACK MODE PARAMETERS INCLUDED");
    TAG_IF_REQUESTED (u->requested_xid.n201_i. ,  XID_N201_I);
    TAG_IF_REQUESTED (u->requested_xid.md. ,      XID_MD);
    TAG_IF_REQUESTED (u->requested_xid.mu. ,      XID_MU);
    TAG_IF_REQUESTED (u->requested_xid.kd. ,      XID_KD);
    TAG_IF_REQUESTED (u->requested_xid.ku. ,      XID_KU);
    }
    else
    {
      llc_data->u->requested_xid.n201_i.valid = FALSE;
      llc_data->u->requested_xid.md.valid = FALSE;
      llc_data->u->requested_xid.mu.valid = FALSE;
      llc_data->u->requested_xid.kd.valid = FALSE;
      llc_data->u->requested_xid.ku.valid = FALSE;
    }

  }
  else
  {
    /*
     * In case we are sending a response, we can include all valid
     * parameters which are in line with the sense of negotiation.
     */
    TAG_IF_REQUESTED_RSP (XID_SENSE_UP,   n200,    XID_N200);
    TAG_IF_REQUESTED_RSP (XID_SENSE_DOWN, n201_u,  XID_N201_U);

    if(send_ack_para)
    {
      TRACE_EVENT("RESPONSE: ACK MODE PARAMETERS INCLUDED");
    TAG_IF_REQUESTED_RSP (XID_SENSE_DOWN, n201_i,  XID_N201_I);
    TAG_IF_REQUESTED_RSP (XID_SENSE_DOWN, md,      XID_MD);
    TAG_IF_REQUESTED_RSP (XID_SENSE_DOWN, mu,      XID_MU);
    TAG_IF_REQUESTED_RSP (XID_SENSE_DOWN, kd,      XID_KD);
    TAG_IF_REQUESTED_RSP (XID_SENSE_DOWN, ku,      XID_KU);
    }

    if (llc_data->u->requested_xid.t200.valid)
    {
      if (llc_data->decoded_xid.t200.valid)
      {
        /* simple add parameter. Sense of negotiation is already checked */
        llc_data->u->xid_tag |= (0x00000001L << XID_T200);
      }
      else
      {
        /* Sense of negotiation compared with current values */
        if (llc_data->u->requested_xid.t200.value XID_SENSE_UP INT2XID(llc_data->t200->length))
        {
          llc_data->u->xid_tag |= (0x00000001L << XID_T200);
        }
      }
    }
    /*
     * L3 parameter cannot be included in response, if not included in
     * request
     */

    /*
     * If an XID parameter which was not included in the SGSN command,
     * but included in the MS Response, must be included in every Response
     * until the parameter is explicitly negotiated by the SGSN
     */

    if(llc_data->u->xid_tag_negotiate)
    {
      llc_data->u->xid_tag |= llc_data->u->xid_tag_negotiate;
    }
  }
} /* u_tag_xid_parameters() */

/*
+------------------------------------------------------------------------------
| Function    : u_send_xid
+------------------------------------------------------------------------------
| Description : This procedure allocates a LL_UNITDATA_REQ primitive, fills in
|               the required parameters, builds an U frame header containing an
|               XID command/response (depending on the cr_bit setting), inserts
|               all XID parmameters that are to be negotiated, and sends this
|               primitive to TX. If a command frame is being sent (see cr_bit),
|               T200 has to be started _before_ the primitive is sent to TX,
|               because otherwise the primitive may not be valid anymore.
|
|               NOTE: T200 must not be running, when an XID command is to be
|               sent. This must be ensured by the caller of u_send_xid().
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_send_xid (T_BIT cr_bit)
{
#ifdef LL_DESC
  T_desc3* desc3;
#endif
  USHORT byte_len = 0;

  TRACE_FUNCTION( "u_send_xid" );

  /*
   * Calculate bit length of tagged XID parameters. First, add
   * the size of each tagged parameter. The size of layer-3
   * parameters is variable, so add the given length in octets.
   */
  ADD_IF_TAGGED (byte_len, XID_VERSION);
  ADD_IF_TAGGED (byte_len, XID_T200);
  ADD_IF_TAGGED (byte_len, XID_N200);
  ADD_IF_TAGGED (byte_len, XID_N201_U);
  /*Acknowledge Mode Parameters will be ignored in XID FRAME:
  ADD_IF_TAGGED (byte_len, XID_N201_I);
  ADD_IF_TAGGED (byte_len, XID_MD);
  ADD_IF_TAGGED (byte_len, XID_MU);
  ADD_IF_TAGGED (byte_len, XID_KD);
  ADD_IF_TAGGED (byte_len, XID_KU);
  */
  if (llc_data->u->xid_tag & (0x00000001L << XID_LAYER_3))
  {
    byte_len += llc_data->requested_l3_xid->length + 2;
  }

  /*
   * Add data header and FCS field to the XID response size to get
   * the overall data size.
   */
  byte_len += U_HDR_SIZE + FCS_SIZE;

  {
#ifndef LL_DESC
    PALLOC_SDU (ll_unitdesc_req, LL_UNITDATA_REQ, (USHORT)(byte_len*8));
#else
    PALLOC (ll_unitdesc_req, LL_UNITDESC_REQ);

    desc3 = llc_palloc_desc(byte_len, 0);

    ll_unitdesc_req->desc_list3.first    = (ULONG)desc3;
    ll_unitdesc_req->desc_list3.list_len = desc3->len;
#endif
    ll_unitdesc_req->sapi         = llc_data->current_sapi;
    ll_unitdesc_req->tlli         = llc_data->tlli_new;
    /*
     * LLC does not know the QoS profile.
     */
    ll_unitdesc_req->ll_qos    = llc_data->cur_qos;
    TRACE_EVENT_P1("peak throughput = %d",llc_data->cur_qos.peak);

    /*
     * LLC signalling frames are always sent with highest priority.
     */
    ll_unitdesc_req->radio_prio = llc_data->cur_radio_prio;
    TRACE_EVENT_P1("radio priority = %d",llc_data->cur_radio_prio);

#ifdef REL99
    /* 
     * From 24.008 & 23.060 it is interpreted that for all signalling data, a 
     * predefined PFI LL_PFI_SIGNALING shall be used.
     */
    ll_unitdesc_req->pkt_flow_id = llc_data->cur_pfi;
    TRACE_EVENT_P1("packet flow id = %d",llc_data->cur_pfi);

#endif  /* REL99 */
    /*
     * No attach to primitive necessary. For retransmission prim
     * data is copied in T200 (if requested).
     */
    ll_unitdesc_req->attached_counter = CCI_NO_ATTACHE;

    /*
     * Label U_SEND_XID_CONT
     */

    /*
     * pf_bit must always be 1 when XID cmd/res are being sent
     */
    u_build_u_frame
      (
#ifndef LL_DESC
      &ll_unitdesc_req->sdu,
#else
      &ll_unitdesc_req->desc_list3,
#endif
      cr_bit, llc_data->current_sapi, 1, U_XID
      );

    u_insert_xid
      (
#ifndef LL_DESC
      &ll_unitdesc_req->sdu,
#else
      &ll_unitdesc_req->desc_list3,
#endif
      cr_bit
      );

    if (cr_bit EQ MS_COMMAND)
    {
      /*
       * remember that an XID command is currently being sent
       */
      llc_data->u->xid_pending = TRUE;

      /*
       * T200 has to be started !before! the primitive is sent to TX.
       */
      sig_u_t200_start_req (ll_unitdesc_req, GRLC_DTACS_DEF);
    }

    /*
     * U frames are always sent with default cause
     */
    sig_u_tx_data_req (ll_unitdesc_req, GRLC_DTACS_DEF);
  }

  if (cr_bit EQ MS_RESPONSE)
  {
    llc_init_requested_xid_sapi(llc_data->current_sapi);
  }

  return;
} /* u_send_xid() */



/*
+------------------------------------------------------------------------------
| Function    : u_handle_optimization
+------------------------------------------------------------------------------
| Description : This procedure adds the values of the requested XID parameter
|               to the decoded XID structure, if they are not included (as an
|               optimization issue).
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_handle_optimization (void)
{

  TRACE_FUNCTION ("u_handle_optimization");


  if (llc_data->u->xid_tag_sent & (0x00000001L << XID_VERSION) AND
      llc_data->decoded_xid.version.valid == FALSE            )
  {
    llc_data->decoded_xid.version.valid = TRUE;
    llc_data->decoded_xid.version.value = llc_data->u->requested_xid.version.valid
                                          ? llc_data->u->requested_xid.version.value
                                          : llc_data->version;
  }

  if (llc_data->u->xid_tag_sent & (0x00000001L << XID_T200) AND
      llc_data->decoded_xid.t200.valid == FALSE            )
  {
    llc_data->decoded_xid.t200.valid = TRUE;
    llc_data->decoded_xid.t200.value = llc_data->u->requested_xid.t200.valid
                                       ? llc_data->u->requested_xid.t200.value
                                       : ((USHORT)INT2XID (llc_data->t200->length));
  }

  if (llc_data->u->xid_tag_sent & (0x00000001L << XID_N200) AND
      llc_data->decoded_xid.n200.valid == FALSE            )
  {
    llc_data->decoded_xid.n200.valid = TRUE;
    llc_data->decoded_xid.n200.value = llc_data->u->requested_xid.n200.valid
                                       ? llc_data->u->requested_xid.n200.value
                                       : *(llc_data->n200);
  }

  if (llc_data->u->xid_tag_sent & (0x00000001L << XID_N201_U) AND
      llc_data->decoded_xid.n201_u.valid == FALSE            )
  {
    llc_data->decoded_xid.n201_u.valid = TRUE;
    llc_data->decoded_xid.n201_u.value = llc_data->u->requested_xid.n201_u.valid
                                         ? llc_data->u->requested_xid.n201_u.value
                                         : *(llc_data->n201_u);
  }

  if (llc_data->u->xid_tag_sent & (0x00000001L << XID_N201_I) AND
      llc_data->decoded_xid.n201_i.valid == FALSE            )
  {
    llc_data->decoded_xid.n201_i.valid = TRUE;
    llc_data->decoded_xid.n201_i.value = llc_data->u->requested_xid.n201_i.valid
                                         ? llc_data->u->requested_xid.n201_i.value
                                         : *(llc_data->n201_i);
  }

  if (llc_data->u->xid_tag_sent & (0x00000001L << XID_MD) AND
      llc_data->decoded_xid.md.valid == FALSE            )
  {
    llc_data->decoded_xid.md.valid = TRUE;
    llc_data->decoded_xid.md.value = llc_data->u->requested_xid.md.valid
                                     ? llc_data->u->requested_xid.md.value
                                     : *(llc_data->md);
  }

  if (llc_data->u->xid_tag_sent & (0x00000001L << XID_MU) AND
      llc_data->decoded_xid.mu.valid == FALSE            )
  {
    llc_data->decoded_xid.mu.valid = TRUE;
    llc_data->decoded_xid.mu.value = llc_data->u->requested_xid.mu.valid
                                     ? llc_data->u->requested_xid.mu.value
                                     : *(llc_data->mu);
  }

  if (llc_data->u->xid_tag_sent & (0x00000001L << XID_KD) AND
      llc_data->decoded_xid.kd.valid == FALSE            )
  {
    llc_data->decoded_xid.kd.valid = TRUE;
    llc_data->decoded_xid.kd.value = llc_data->u->requested_xid.kd.valid
                                     ? llc_data->u->requested_xid.kd.value
                                     : *(llc_data->kd);
  }

  if (llc_data->u->xid_tag_sent & (0x00000001L << XID_KU) AND
      llc_data->decoded_xid.ku.valid == FALSE            )
  {
    llc_data->decoded_xid.ku.valid = TRUE;
    llc_data->decoded_xid.ku.value = llc_data->u->requested_xid.ku.valid
                                     ? llc_data->u->requested_xid.ku.value
                                     : *(llc_data->ku);
  }


  /*
   * Layer 3 XID must be included in resonse, if it was included in request
   */

} /* u_handle_optimization() */


