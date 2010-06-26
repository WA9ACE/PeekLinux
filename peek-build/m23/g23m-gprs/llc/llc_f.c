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
|  Purpose :  Contains global functions of Logical Link Control (LLC)
+----------------------------------------------------------------------------- 
*/ 

#define LLC_F_C

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

#include "llc_par.h"    /* to get the default values of the LLC paramters */
#include "llc_uf.h"     /* to get the XID parameter definitions */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : llc_init_parameters
+------------------------------------------------------------------------------
| Description : This procedure initialises the LLC layer parameters with 
|               their default values as they are defined in LLC_PAR.H.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void llc_init_parameters (void)
{
  UBYTE             incarnation;

  TRACE_FUNCTION ("llc_init_parameters");

  llc_data->version                               = LLC_VERSION_ALL_SAPIS;

  llc_data->iov_ui                                = LLC_IOV_UI_ALL_SAPIS;
  llc_data->iov_i_base[IMAP(LL_SAPI_3)]           = LLC_IOV_I_SAPI_3;
  llc_data->iov_i_base[IMAP(LL_SAPI_5)]           = LLC_IOV_I_SAPI_5;
  llc_data->iov_i_base[IMAP(LL_SAPI_9)]           = LLC_IOV_I_SAPI_9;
  llc_data->iov_i_base[IMAP(LL_SAPI_11)]          = LLC_IOV_I_SAPI_11;

  /*
   * The LLC parameters for T200 are contained in T200 service data.
   */
  llc_data->t200_base[UIMAP(LL_SAPI_1)].length    = LLC_T200_SAPI_1;
  llc_data->t200_base[UIMAP(LL_SAPI_3)].length    = LLC_T200_SAPI_3;
  llc_data->t200_base[UIMAP(LL_SAPI_5)].length    = LLC_T200_SAPI_5;
  llc_data->t200_base[UIMAP(LL_SAPI_7)].length    = LLC_T200_SAPI_7;
  llc_data->t200_base[UIMAP(LL_SAPI_9)].length    = LLC_T200_SAPI_9;
  llc_data->t200_base[UIMAP(LL_SAPI_11)].length   = LLC_T200_SAPI_11;

  /*
   * The LLC parameters for T201 are contained in T201 service data.
   * T201 is being set to the same values as T200 for all SAPIs.
   * <R.LLC.LLC_PAR.A.015>
   */
  llc_data->n200_base[UIMAP(LL_SAPI_1)]           = LLC_N200_SAPI_1;
  llc_data->n200_base[UIMAP(LL_SAPI_3)]           = LLC_N200_SAPI_3;
  llc_data->n200_base[UIMAP(LL_SAPI_5)]           = LLC_N200_SAPI_5;
  llc_data->n200_base[UIMAP(LL_SAPI_7)]           = LLC_N200_SAPI_7;
  llc_data->n200_base[UIMAP(LL_SAPI_9)]           = LLC_N200_SAPI_9;
  llc_data->n200_base[UIMAP(LL_SAPI_11)]          = LLC_N200_SAPI_11;

  llc_data->n201_u_base[UIMAP(LL_SAPI_1)]         = LLC_N201_U_SAPI_1;
  llc_data->n201_u_base[UIMAP(LL_SAPI_3)]         = LLC_N201_U_SAPI_3;
  llc_data->n201_u_base[UIMAP(LL_SAPI_5)]         = LLC_N201_U_SAPI_5;
  llc_data->n201_u_base[UIMAP(LL_SAPI_7)]         = LLC_N201_U_SAPI_7;
  llc_data->n201_u_base[UIMAP(LL_SAPI_9)]         = LLC_N201_U_SAPI_9;
  llc_data->n201_u_base[UIMAP(LL_SAPI_11)]        = LLC_N201_U_SAPI_11;

  llc_data->n201_i_base[IMAP(LL_SAPI_3)]          = LLC_N201_I_SAPI_3;
  llc_data->n201_i_base[IMAP(LL_SAPI_5)]          = LLC_N201_I_SAPI_5;
  llc_data->n201_i_base[IMAP(LL_SAPI_9)]          = LLC_N201_I_SAPI_9;
  llc_data->n201_i_base[IMAP(LL_SAPI_11)]         = LLC_N201_I_SAPI_11;
      
  llc_data->md_base[IMAP(LL_SAPI_3)]              = LLC_MD_SAPI_3;
  llc_data->md_base[IMAP(LL_SAPI_5)]              = LLC_MD_SAPI_5;
  llc_data->md_base[IMAP(LL_SAPI_9)]              = LLC_MD_SAPI_9;
  llc_data->md_base[IMAP(LL_SAPI_11)]             = LLC_MD_SAPI_11;
      
  llc_data->mu_base[IMAP(LL_SAPI_3)]              = LLC_MU_SAPI_3;
  llc_data->mu_base[IMAP(LL_SAPI_5)]              = LLC_MU_SAPI_5;
  llc_data->mu_base[IMAP(LL_SAPI_9)]              = LLC_MU_SAPI_9;
  llc_data->mu_base[IMAP(LL_SAPI_11)]             = LLC_MU_SAPI_11;
      
  llc_data->kd_base[IMAP(LL_SAPI_3)]              = LLC_KD_SAPI_3;
  llc_data->kd_base[IMAP(LL_SAPI_5)]              = LLC_KD_SAPI_5;
  llc_data->kd_base[IMAP(LL_SAPI_9)]              = LLC_KD_SAPI_9;
  llc_data->kd_base[IMAP(LL_SAPI_11)]             = LLC_KD_SAPI_11;
      
  llc_data->ku_base[IMAP(LL_SAPI_3)]              = LLC_KU_SAPI_3;
  llc_data->ku_base[IMAP(LL_SAPI_5)]              = LLC_KU_SAPI_5;
  llc_data->ku_base[IMAP(LL_SAPI_9)]              = LLC_KU_SAPI_9;
  llc_data->ku_base[IMAP(LL_SAPI_11)]             = LLC_KU_SAPI_11;

  /*
   * Reset all OCs for unacknowledged transfer.
   */
  for (incarnation = 0; incarnation < MAX_SAPI_INC; incarnation++)
  {
    llc_data->sapi_base[incarnation].oc_ui_tx = 0L;
    llc_data->sapi_base[incarnation].oc_ui_rx = 0L;
  }

  return;
} /* llc_init_parameters() */


/*
+------------------------------------------------------------------------------
| Function    : llc_get_ffs_data
+------------------------------------------------------------------------------
| Description : Load configured LLC parameter from FFS. In case of _SIMULATION_
|               only default values are returned.
|
| Parameters  : type             - XID Paramter type
|               sapi_array_index - Array index of Sapi !!!
|
+------------------------------------------------------------------------------
*/
LOCAL USHORT llc_get_ffs_data (UBYTE type, UBYTE sapi_array_index)
{

  switch (type)
  {
#ifdef _SIMULATION_

    case XID_T200:      
      if (llc_data->ffs_xid.t200[sapi_array_index].valid)
        return llc_data->ffs_xid.t200[sapi_array_index].value; /* 50 = 5 sec !!! */
      else
        return (USHORT)INT2XID(llc_data->t200_base[sapi_array_index].length);

    case XID_N200:      
      if (llc_data->ffs_xid.n200[sapi_array_index].valid)
        return (USHORT)llc_data->ffs_xid.n200[sapi_array_index].value;
      else
        return (USHORT)llc_data->n200_base[sapi_array_index];

    case XID_MD:        
      if (llc_data->ffs_xid.md[sapi_array_index].valid)
        return llc_data->ffs_xid.md[sapi_array_index].value;
      else
        return llc_data->md_base[sapi_array_index];

    case XID_MU:        
      if (llc_data->ffs_xid.mu[sapi_array_index].valid)
        return (USHORT)llc_data->ffs_xid.mu[sapi_array_index].value;
      else
        return (USHORT)llc_data->mu_base[sapi_array_index];

    case XID_N201_U:    
      if (llc_data->ffs_xid.n201_u[sapi_array_index].valid)
        return llc_data->ffs_xid.n201_u[sapi_array_index].value;
      else
        return llc_data->n201_u_base[sapi_array_index];

    case XID_N201_I:
      if (llc_data->ffs_xid.n201_i[sapi_array_index].valid)
        return llc_data->ffs_xid.n201_i[sapi_array_index].value;
      else
        return llc_data->n201_i_base[sapi_array_index];

    case XID_KD:        
      if (llc_data->ffs_xid.kd[sapi_array_index].valid)
        return (USHORT)llc_data->ffs_xid.kd[sapi_array_index].value;
      else
        return (USHORT)llc_data->kd_base[sapi_array_index];

    case XID_KU:        
      if (llc_data->ffs_xid.ku[sapi_array_index].valid)
        return (USHORT)llc_data->ffs_xid.ku[sapi_array_index].value;
      else
        return (USHORT)llc_data->ku_base[sapi_array_index];

#else

    case XID_T200:      return (USHORT)INT2XID(llc_data->t200_base[sapi_array_index].length);
    case XID_N200:      return (USHORT)llc_data->n200_base[sapi_array_index];
/*  case XID_N201_U:    return (USHORT)llc_data->n201_u_base[sapi_array_index]; */
/*  case XID_KD:        return (USHORT)llc_data->kd_base[sapi_array_index]; */
/*  case XID_KU:        return (USHORT)llc_data->ku_base[sapi_array_index]; */
    case XID_N201_I:    return (USHORT)N201_I_SUPPORTED;
    case XID_N201_U:    return (USHORT)N201_U_SUPPORTED;
    case XID_KD:        return (USHORT)KD_KD_SUPPORTED; 
    case XID_KU:        return (USHORT)KD_KU_SUPPORTED;
    case XID_MD:        return (USHORT)KD_MD_SUPPORTED;
    case XID_MU:        return (USHORT)KD_MU_SUPPORTED;


#endif /* _SIMULATION_ */

    default:
      TRACE_ERROR ("Illegal FFS parameter used");
      return 0;
  }

} /* llc_get_ffs_data */


/*
+------------------------------------------------------------------------------
| Function    : llc_init_requested_xid
+------------------------------------------------------------------------------
| Description : Fill in requested XID parameters into requested_xid structure
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void llc_init_requested_xid (void)
{
  UBYTE  u_inc, i_inc, sapi;
  USHORT requested_value;

  for (u_inc = 0; u_inc < ACKNOWLEDGED_INC; u_inc++)
  {
    llc_data->requested_l3_xid_base[u_inc].valid  = FALSE;
  }

  for (sapi = LL_SAPI_1; sapi <= LL_SAPI_11; sapi += 2)
  {
    u_inc = UIMAP(sapi);

    llc_data->u_base[u_inc].requested_xid.version.valid = FALSE;
    llc_data->u_base[u_inc].requested_xid.iov_ui.valid  = FALSE;
    llc_data->u_base[u_inc].requested_xid.iov_i.valid   = FALSE;
    llc_data->u_base[u_inc].requested_xid.reset.valid   = FALSE;

    llc_data->u_base[u_inc].requested_xid.t200.valid    = FALSE;
    llc_data->u_base[u_inc].requested_xid.n200.valid    = FALSE;
    llc_data->u_base[u_inc].requested_xid.n201_u.valid  = FALSE;
    llc_data->u_base[u_inc].requested_xid.n201_i.valid  = FALSE;
    llc_data->u_base[u_inc].requested_xid.kd.valid      = FALSE;
    llc_data->u_base[u_inc].requested_xid.ku.valid      = FALSE;
    llc_data->u_base[u_inc].requested_xid.md.valid      = FALSE;
    llc_data->u_base[u_inc].requested_xid.mu.valid      = FALSE;

    requested_value = llc_get_ffs_data (XID_T200, u_inc);
    if (requested_value != (USHORT)INT2XID(llc_data->t200_base[u_inc].length))
    {
      llc_data->u_base[u_inc].requested_xid.t200.valid  = TRUE;
      llc_data->u_base[u_inc].requested_xid.t200.value  = requested_value;
    }

    requested_value = llc_get_ffs_data (XID_N200, u_inc);
    if (requested_value != llc_data->n200_base[u_inc])
    {
      llc_data->u_base[u_inc].requested_xid.n200.valid  = TRUE;
      llc_data->u_base[u_inc].requested_xid.n200.value  = (UBYTE)requested_value;
    }

    requested_value = llc_get_ffs_data (XID_N201_U, u_inc);
    if (requested_value != llc_data->n201_u_base[u_inc])
    {
      llc_data->u_base[u_inc].requested_xid.n201_u.valid  = TRUE;
      llc_data->u_base[u_inc].requested_xid.n201_u.value  = requested_value;
    }

    if ((sapi != LL_SAPI_1) && (sapi != LL_SAPI_7))
    {
      i_inc = (UBYTE)IMAP(sapi);

      requested_value = llc_get_ffs_data (XID_N201_I, i_inc);
      if (requested_value != llc_data->n201_i_base[i_inc])
      {
        llc_data->u_base[u_inc].requested_xid.n201_i.valid  = TRUE;
        llc_data->u_base[u_inc].requested_xid.n201_i.value  = requested_value;
      }

      requested_value = llc_get_ffs_data (XID_MD, i_inc);
      if (requested_value != llc_data->md_base[i_inc])
      {
        llc_data->u_base[u_inc].requested_xid.md.valid  = TRUE;
        llc_data->u_base[u_inc].requested_xid.md.value  = requested_value;
      }

      requested_value = llc_get_ffs_data (XID_MU, i_inc);
      if (requested_value != llc_data->mu_base[i_inc])
      {
        llc_data->u_base[u_inc].requested_xid.mu.valid  = TRUE;
        llc_data->u_base[u_inc].requested_xid.mu.value  = requested_value;
      }

      requested_value = llc_get_ffs_data (XID_KD, i_inc);
      if (requested_value != llc_data->kd_base[i_inc])
      {
        llc_data->u_base[u_inc].requested_xid.kd.valid  = TRUE;
        llc_data->u_base[u_inc].requested_xid.kd.value  = (UBYTE)requested_value;
      }
  
      requested_value = llc_get_ffs_data (XID_KU, i_inc);
      if (requested_value != llc_data->ku_base[i_inc])
      {
        llc_data->u_base[u_inc].requested_xid.ku.valid  = TRUE;
        llc_data->u_base[u_inc].requested_xid.ku.value  = (UBYTE)requested_value;
      }
    }
  }
} /* llc_init_requested_xid() */


/*
+------------------------------------------------------------------------------
| Function    : llc_init_requested_xid_sapi
+------------------------------------------------------------------------------
| Description : Fill in requested XID parameters into requested_xid structure
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void llc_init_requested_xid_sapi (T_SAPI sapi)
{
  UBYTE  u_inc, i_inc;
  USHORT requested_value;

  u_inc = UIMAP(sapi);

  llc_data->u_base[u_inc].requested_xid.t200.valid    = FALSE;
  llc_data->u_base[u_inc].requested_xid.n200.valid    = FALSE;
  llc_data->u_base[u_inc].requested_xid.n201_u.valid  = FALSE;
  llc_data->u_base[u_inc].requested_xid.n201_i.valid  = FALSE;
  llc_data->u_base[u_inc].requested_xid.kd.valid      = FALSE;
  llc_data->u_base[u_inc].requested_xid.ku.valid      = FALSE;
  llc_data->u_base[u_inc].requested_xid.md.valid      = FALSE;
  llc_data->u_base[u_inc].requested_xid.mu.valid      = FALSE;

  requested_value = llc_get_ffs_data (XID_T200, u_inc);
  if (requested_value != (USHORT)INT2XID(llc_data->t200_base[u_inc].length))
  {
    llc_data->u_base[u_inc].requested_xid.t200.valid  = TRUE;
    llc_data->u_base[u_inc].requested_xid.t200.value  = requested_value;
  }

  requested_value = llc_get_ffs_data (XID_N200, u_inc);
  if (requested_value != llc_data->n200_base[u_inc])
  {
    llc_data->u_base[u_inc].requested_xid.n200.valid  = TRUE;
    llc_data->u_base[u_inc].requested_xid.n200.value  = (UBYTE)requested_value;
  }

  requested_value = llc_get_ffs_data (XID_N201_U, u_inc);
  if (requested_value != llc_data->n201_u_base[u_inc])
  {
    llc_data->u_base[u_inc].requested_xid.n201_u.valid  = TRUE;
    llc_data->u_base[u_inc].requested_xid.n201_u.value  = requested_value;
  }

  if ((sapi != LL_SAPI_1) && (sapi != LL_SAPI_7))
  {
    i_inc = (UBYTE)IMAP(sapi);

    requested_value = llc_get_ffs_data (XID_N201_I, i_inc);
    if (requested_value != llc_data->n201_i_base[i_inc])
    {
      llc_data->u_base[u_inc].requested_xid.n201_i.valid  = TRUE;
      llc_data->u_base[u_inc].requested_xid.n201_i.value  = requested_value;
    }

    requested_value = llc_get_ffs_data (XID_MD, i_inc);
    if (requested_value != llc_data->md_base[i_inc])
    {
      llc_data->u_base[u_inc].requested_xid.md.valid  = TRUE;
      llc_data->u_base[u_inc].requested_xid.md.value  = requested_value;
    }

    requested_value = llc_get_ffs_data (XID_MU, i_inc);
    if (requested_value != llc_data->mu_base[i_inc])
    {
      llc_data->u_base[u_inc].requested_xid.mu.valid  = TRUE;
      llc_data->u_base[u_inc].requested_xid.mu.value  = requested_value;
    }

    requested_value = llc_get_ffs_data (XID_KD, i_inc);
    if (requested_value != llc_data->kd_base[i_inc])
    {
      llc_data->u_base[u_inc].requested_xid.kd.valid  = TRUE;
      llc_data->u_base[u_inc].requested_xid.kd.value  = (UBYTE)requested_value;
    }
  
    requested_value = llc_get_ffs_data (XID_KU, i_inc);
    if (requested_value != llc_data->ku_base[i_inc])
    {
      llc_data->u_base[u_inc].requested_xid.ku.valid  = TRUE;
      llc_data->u_base[u_inc].requested_xid.ku.value  = (UBYTE)requested_value;
    }
  }
} /* llc_init_requested_xid_sapi() */



/*
+------------------------------------------------------------------------------
| Function    : llc_generate_input
+------------------------------------------------------------------------------
| Description : This procedure calculates the frame-dependent input for 
|               UI_FRAMES / I_FRAMES. S_FRAMES are treated like I_FRAMES. 
|               Parameter direction must be one of 
|               CCI_DIRECTION_UPLINK/DOWNLINK. The input is generated 
|               according to section A.2.1 in GSM 04.64, and written to 
|               paramete input. This procedure is called from services 
|               TX and RX.
|
| Parameters  : sapi          - valid SAPI number
|               frame_type    - indicates acknowledged/unacknowledged frames
|               lfn           - LLC frame number
|               direction     - CCI_DIRECTION_UPLINK/CCI_DIRECTION_DOWNLINK
|               cipher_input  - generated ciphering input
|
+------------------------------------------------------------------------------
*/
GLOBAL void llc_generate_input (UBYTE sapi,
                                T_PDU_TYPE frame_type,
                                T_FRAME_NUM lfn,
                                ULONG *cipher_input,
                                ULONG oc)
{
  ULONG             sx;


  TRACE_FUNCTION ("llc_generate_input");

  /*
   * According to 04.64, Annex A.2.1:
   *   SX = 2exp27 * SAPI + 2exp31
   */
  sx = 134217728 * sapi + 2147483648;

  if ((frame_type EQ I_FRAME) OR (frame_type EQ S_FRAME))
  {
    /*
     * According to 04.64, Annex A.2.1:
     *   Input = ((IOV-I + LFN + OC) modulo 2exp32
     */
    *cipher_input = (ULONG)(*(llc_data->iov_i) + lfn + oc);
  }
  else /* unacknowledged mode */
  {
    /*
     * According to 04.64, Annex A.2.1:
     *   Input = ((IOV-UI XOR SX) + LFN + OC) modulo 2exp32
     */
    *cipher_input = (ULONG)((llc_data->iov_ui ^ sx) + lfn + oc);
  }

  return;
} /* llc_generate_input() */


/*
+------------------------------------------------------------------------------
| Function    : llc_build_crc24
+------------------------------------------------------------------------------
| Description : This procedure builds a CRC24 checksum according to RFC2440,
|               which is needed by LLC to build and check the Frame Check
|               Sequence (FCS) which is included in each frame.
|
| Parameters  : octets  - a valid pointer to the frame contents
|               len     - number of valid octets
|
+------------------------------------------------------------------------------
*/

/* 
 * Precomputed table for polynomial : 0x00ad85dd (high term in LSB)
 * 24-bit masks are packed in 16-bits words
 *
 */
 
const USHORT a_fcs24_tab_rev_packed[384] = {
  0x0000, 0x7600, 0xd6a7, 0x4557, 0x21f6, 0x20e2, 0x8115, 0x63b7, 0x6126, 0xc442, 0x3441, 0x9763, 
  0x0991, 0xe734, 0xe2ae, 0x4cc6, 0xb0c2, 0x14eb, 0x8884, 0xf283, 0x552f, 0xcdd3, 0xa575, 0xa36a, 
  0x1322, 0x5468, 0xbeb4, 0x5675, 0x039e, 0x48f1, 0x9237, 0x41df, 0x0935, 0xd760, 0x1629, 0xff70, 
  0x1ab3, 0xc55c, 0x8abd, 0x5fe4, 0x92aa, 0x7cf8, 0x9ba6, 0xd0eb, 0x3d3c, 0xdef1, 0x871d, 0xcb79, 
  0x2644, 0x32d0, 0x0681, 0x6313, 0x6526, 0xf0c4, 0xa751, 0x2767, 0xb100, 0xe206, 0x7091, 0x4745, 
  0x2fd5, 0xa3e4, 0x3288, 0x6a82, 0xf412, 0xc4cd, 0xaec0, 0xb653, 0x8509, 0xeb97, 0xe1a5, 0x734c, 
  0x3566, 0x10b8, 0x6e92, 0x7031, 0x474e, 0x98d7, 0xb473, 0x050f, 0xd913, 0xf124, 0x52f9, 0x2f56, 
  0x3cf7, 0x818c, 0x5a9b, 0x79a0, 0xd67a, 0xacde, 0xbde2, 0x943b, 0xed1a, 0xf8b5, 0xc3cd, 0x1b5f, 
  0x4733, 0x45fb, 0x2de0, 0x0264, 0x120d, 0xdba5, 0xc626, 0x504c, 0x9a61, 0x8371, 0x07ba, 0x6c24, 
  0x4ea2, 0xd4cf, 0x19e9, 0x0bf5, 0x8339, 0xefac, 0xcfb7, 0xc178, 0xae68, 0x8ae0, 0x968e, 0x582d, 
  0x5411, 0x6793, 0x45f3, 0x1146, 0x3065, 0xb3b6, 0xd504, 0x7224, 0xf272, 0x9053, 0x25d2, 0x0437, 
  0x5d80, 0xf6a7, 0x71fa, 0x18d7, 0xa151, 0x87bf, 0xdc95, 0xe310, 0xc67b, 0x99c2, 0xb4e6, 0x303e, 
  0x6177, 0x012b, 0xfdc6, 0x2420, 0x56dd, 0x0b83, 0xe062, 0x149c, 0x4a47, 0xa535, 0x436a, 0xbc02, 
  0x68e6, 0x901f, 0xc9cf, 0x2db1, 0xc7e9, 0x3f8a, 0xe9f3, 0x85a8, 0x7e4e, 0xaca4, 0xd25e, 0x880b, 
  0x7255, 0x2343, 0x95d5, 0x3702, 0x74b5, 0x6390, 0xf340, 0x36f4, 0x2254, 0xb617, 0x6102, 0xd411, 
  0x7bc4, 0xb277, 0xa1dc, 0x3e93, 0xe581, 0x5799, 0xfad1, 0xa7c0, 0x165d, 0xbf86, 0xf036, 0xe018, 
  0x85dd, 0xabad, 0x7b22, 0xc08a, 0xfc5b, 0x8d67, 0x04c8, 0xbe1a, 0xcca3, 0x419f, 0xe9ec, 0x3ae6, 
  0x8c4c, 0x3a99, 0x4f2b, 0xc91b, 0x6d6f, 0xb96e, 0x0d59, 0x2f2e, 0xf8aa, 0x480e, 0x78d8, 0x0eef, 
  0x96ff, 0x89c5, 0x1331, 0xd3a8, 0xde33, 0xe574, 0x17ea, 0x9c72, 0xa4b0, 0x52bd, 0xcb84, 0x52f5, 
  0x9f6e, 0x18f1, 0x2738, 0xda39, 0x4f07, 0xd17d, 0x1e7b, 0x0d46, 0x90b9, 0x5b2c, 0x5ab0, 0x66fc, 
  0xa399, 0xef7d, 0xab04, 0xe6ce, 0xb88b, 0x5d41, 0x228c, 0xfaca, 0x1c85, 0x67db, 0xad3c, 0xeac0, 
  0xaa08, 0x7e49, 0x9f0d, 0xef5f, 0x29bf, 0x6948, 0x2b1d, 0x6bfe, 0x288c, 0x6e4a, 0x3c08, 0xdec9, 
  0xb0bb, 0xcd15, 0xc317, 0xf5ec, 0x9ae3, 0x3552, 0x31ae, 0xd8a2, 0x7496, 0x74f9, 0x8f54, 0x82d3, 
  0xb92a, 0x5c21, 0xf71e, 0xfc7d, 0x0bd7, 0x015b, 0x383f, 0x4996, 0x409f, 0x7d68, 0x1e60, 0xb6da, 
  0xc2ee, 0x9856, 0x8065, 0x87b9, 0xcfa0, 0x7620, 0x43fb, 0x8de1, 0x37e4, 0x06ac, 0xda17, 0xc1a1, 
  0xcb7f, 0x0962, 0xb46c, 0x8e28, 0x5e94, 0x4229, 0x4a6a, 0x1cd5, 0x03ed, 0x0f3d, 0x4b23, 0xf5a8, 
  0xd1cc, 0xba3e, 0xe876, 0x949b, 0xedc8, 0x1e33, 0x50d9, 0xaf89, 0x5ff7, 0x158e, 0xf87f, 0xa9b2, 
  0xd85d, 0x2b0a, 0xdc7f, 0x9d0a, 0x7cfc, 0x2a3a, 0x5948, 0x3ebd, 0x6bfe, 0x1c1f, 0x694b, 0x9dbb, 
  0xe4aa, 0xdc86, 0x5043, 0xa1fd, 0x8b70, 0xa606, 0x65bf, 0xc931, 0xe7c2, 0x20e8, 0x9ec7, 0x1187, 
  0xed3b, 0x4db2, 0x644a, 0xa86c, 0x1a44, 0x920f, 0x6c2e, 0x5805, 0xd3cb, 0x2979, 0x0ff3, 0x258e, 
  0xf788, 0xfeee, 0x3850, 0xb2df, 0xa918, 0xce15, 0x769d, 0xeb59, 0x8fd1, 0x33ca, 0xbcaf, 0x7994, 
  0xfe19, 0x6fda, 0x0c59, 0xbb4e, 0x382c, 0xfa1c, 0x7f0c, 0x7a6d, 0xbbd8, 0x3a5b, 0x2d9b, 0x4d9d
};


/******************************************************************************
 *
 *  FUNCTION NAME: f_crc24_tab
 *
 *  The function compute the 24-bit FCS and is using table for it 
 *
 *  ARGUMENT LIST:
 *
 *  Argument        Type      IO  Description
 *  -------------   --------  --  ---------------------------------------------
 *  data_p          T_BYTE*   I  Pointer on the input buffer
 *  d_length          T_UINT16  I  Number of bytes to process
 *  d_l_crc_init    T_UINT16  I  Init value of the CRC
 *
 * RETURN VALUE:    
 *  Argument        Type      Description
 *  -------------   --------  ---------------------------------------------
 *  d_l_crc         T_UINT32  24-bit FCS stored in a 32-bit word
 *
 *****************************************************************************/
GLOBAL ULONG llc_build_crc24 (UBYTE *d_data_p, 
                              USHORT d_length)
{
  /*  GLOBAL VARIABLES:
   *
   *  Variables       Type      IO  Description
   *  -------------   --------  --  -------------------------------------------
   *  none
   */

  /*  LOCAL VARIABLES:
   *
   *  Variables       Type      Description
   *  -------------   -------   ----------------------------------------------
   *  d_l_crc         T_UINT32  24-bit FCS stored in a 32-bit word
   *  d_l_tab_res     T_UINT32  FCS mask extracted from the table
   *  d_l_tab_index   T_UINT32  Index of the FCS mask into the table
   *  d_byte_msb      T_UINT16  Intermediate variable used to know the position
   *                            FCSof the 24-bit FCS into two 16-bits words
   */
  ULONG  d_l_crc, d_l_tab_res, d_l_tab_index;
  USHORT d_byte_msb;

  d_l_crc = 0x00ffffff;
  
  do 
  {
    /* Most efficient C implementation with 32-bits tables but require more d_data rom */
    /* d_l_crc = (d_l_crc >> 8) ^ a_fcs24_tab_rev[(d_l_crc ^ *d_data_p++) & 0xff];     */

    /* Details very close to the ASM implementation */
    d_l_tab_index = (d_l_crc ^ *d_data_p++) & 0x0ff;
    d_l_crc >>= 8;

    if(d_l_tab_index & 0x01)
      d_byte_msb = 1;
    else
      d_byte_msb = 0;

    d_l_tab_index *= 3;

    d_l_tab_index >>= 1;

    d_l_crc &= 0x0000ffff;  /* Replace the code in comment below */

    d_l_tab_res = a_fcs24_tab_rev_packed[d_l_tab_index];
    d_l_tab_res |= (a_fcs24_tab_rev_packed[d_l_tab_index+1] << 16);

    if(d_byte_msb == 1)
      d_l_tab_res >>= 8;

    d_l_crc ^= d_l_tab_res;
  } 
  while (--d_length);

  d_l_crc = (~d_l_crc) & 0x00ffffff;

  return (d_l_crc);
} /* llc_build_crc24() */

/*
+------------------------------------------------------------------------------
| Function    : llc_xid_value_acceptable
+------------------------------------------------------------------------------
| Description : This procedure checks if the value of the XID parameter for
|               the given SAPI is acceptable, or not. The return value is TRUE
|               for an accepted value, and FALSE for an unaccepted value.
|
| Parameters  : sapi          - SAPI number
|               xid_parameter - XID parameter (defined in llc_uf.h)
|               xid_value     - value of XID parameter (actually UBYTE/USHORT/
|                               ULONG, depending on the parameter)
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL llc_xid_value_acceptable (UBYTE sapi,
                                      UBYTE xid_parameter,
                                      ULONG xid_value)
{
  BOOL              rc;
  UBYTE             u_inc, i_inc;


  TRACE_FUNCTION ("llc_xid_value_acceptable");

  /*
   * Preset rc in case of unknown XID parameters or unknown SAPI values.
   */
  rc  = FALSE;
  u_inc = UIMAP(sapi);
  i_inc = IMAP(sapi);

  /*
   * Accept only possible (valid) values for now (defined in llc_uf.h).
   */
  switch (xid_parameter)
  {
    case XID_VERSION:
      if (xid_value EQ LLC_VERSION_ALL_SAPIS)
      {
        rc = TRUE;
        TRACE_EVENT ("Version value accepted");
      }
      else
      {
        TRACE_EVENT ("Version value NOT accepted");
      }
      break;

    case XID_IOV_UI:
      /*
       * Must not be checked, ignored.
       */
      TRACE_EVENT ("IOV-UI values are not checked.");
      break;

    case XID_IOV_I:
      /*
       * Must not be checked, ignored.
       */
      TRACE_EVENT ("IOV-I values are not checked.");
      break;

    case XID_T200:
      switch (sapi)
      {
        case LL_SAPI_1:
        case LL_SAPI_3:
        case LL_SAPI_5:
        case LL_SAPI_7:
        case LL_SAPI_9:
        case LL_SAPI_11:
          if ( (xid_value >= (llc_data->u_base[u_inc].requested_xid.t200.valid ?
                llc_get_ffs_data(XID_T200, u_inc): XID_T200_MIN))  
               AND (xid_value <= XID_T200_MAX) )
          {
            rc = TRUE;
            TRACE_EVENT ("T200 value accepted");
          }
          else
          {
            TRACE_EVENT ("T200 value NOT accepted");
          }
          break;
        default:
          TRACE_ERROR ("invalid SAPI value for T200");
          break;
      }
      break;

    case XID_N200:
      switch (sapi)
      {
        case LL_SAPI_1:
        case LL_SAPI_3:
        case LL_SAPI_5:
        case LL_SAPI_7:
        case LL_SAPI_9:
        case LL_SAPI_11:
          if ( (xid_value >= (llc_data->u_base[u_inc].requested_xid.n200.valid ?
                llc_get_ffs_data(XID_N200, u_inc): XID_N200_MIN))  
               AND (xid_value <= XID_N200_MAX) )
          {
            rc = TRUE;
            TRACE_EVENT ("N200 value accepted");
          }
          else
          {
            TRACE_EVENT ("N200 value NOT accepted");
          }
          break;
        default:
          TRACE_ERROR ("invalid SAPI value for N200");
          break;
      }
      break;

    case XID_N201_U:
      switch (sapi)
      {
        case LL_SAPI_1:
        case LL_SAPI_3:
        case LL_SAPI_5:
        case LL_SAPI_7:
        case LL_SAPI_9:
        case LL_SAPI_11:
          if ( (xid_value >= (USHORT)XID_N201_U_MIN) AND 
             (xid_value <= (llc_data->u_base[u_inc].requested_xid.n201_u.valid ?
              llc_get_ffs_data(XID_N201_U, u_inc): XID_N201_U_MAX)) )
          {
            rc = TRUE;
            TRACE_EVENT ("N201-U value accepted");
          }
          else
          {
            TRACE_EVENT ("N201-U value NOT accepted");
          }
          break;
        default:
          TRACE_ERROR ("invalid SAPI value for N201-U");
          break;
      }
      break;

    case XID_N201_I:
      switch (sapi)
      {
        case LL_SAPI_3:
        case LL_SAPI_5:
        case LL_SAPI_9:
        case LL_SAPI_11:
          if ( (xid_value >= XID_N201_I_MIN) AND 
               (xid_value <= (llc_data->u_base[u_inc].requested_xid.n201_i.valid ?
                llc_get_ffs_data(XID_N201_I, i_inc): XID_N201_I_MAX)) )
          {
            rc = TRUE;
            TRACE_EVENT ("N201-I value accepted");
          }
          else
          {
            TRACE_EVENT ("N201-I value NOT accepted");
          }
          break;
        default:
          TRACE_ERROR ("invalid SAPI value for N201-I");
          break;
      }
      break;

    case XID_MD:
      switch (sapi)
      {
        case LL_SAPI_3:
        case LL_SAPI_5:
        case LL_SAPI_9:
        case LL_SAPI_11:
          if ( (xid_value == XID_MD_OFF) OR ((xid_value >= XID_MD_MIN) AND 
               (xid_value <= (llc_data->u_base[u_inc].requested_xid.md.valid ?
              llc_get_ffs_data(XID_MD, i_inc): XID_MD_MAX)) ))
          {
            rc = TRUE;
            TRACE_EVENT ("mD value accepted");
          }
          else
          {
            TRACE_EVENT ("mD value NOT accepted");
          }
          break;
        default:
          TRACE_ERROR ("invalid SAPI value for mD");
          break;
      }
      break;

    case XID_MU:
      switch (sapi)
      {
        case LL_SAPI_3:
        case LL_SAPI_5:
        case LL_SAPI_9:
        case LL_SAPI_11:
          if ( (xid_value == XID_MU_OFF) OR ((xid_value >= XID_MU_MIN) AND 
               (xid_value <= (llc_data->u_base[u_inc].requested_xid.mu.valid ?
                llc_get_ffs_data(XID_MU, i_inc): XID_MU_MAX)) ))
          {
            rc = TRUE;
            TRACE_EVENT ("mU value accepted");
          }
          else
          {
            TRACE_EVENT ("mU value NOT accepted");
          }
          break;
        default:
          TRACE_ERROR ("invalid SAPI value for mU");
          break;
      }
      break;

    case XID_KD:
      switch (sapi)
      {
        case LL_SAPI_3:
        case LL_SAPI_5:
        case LL_SAPI_9:
        case LL_SAPI_11:
          if ( (xid_value >= XID_KD_MIN) AND 
               (xid_value <= (llc_data->u_base[u_inc].requested_xid.kd.valid ?
                llc_get_ffs_data(XID_KD, i_inc): XID_KD_MAX)) )
          {
            rc = TRUE;
            TRACE_EVENT ("kD value accepted");
          }
          else
          {
            TRACE_EVENT ("kD value NOT accepted");
          }
          break;
        default:
          TRACE_ERROR ("invalid SAPI value for kD");
          break;
      }
      break;

    case XID_KU:
      switch (sapi)
      {
        case LL_SAPI_3:
        case LL_SAPI_5:
        case LL_SAPI_9:
        case LL_SAPI_11:
          if ( (xid_value >= XID_KU_MIN) AND
               (xid_value <= (llc_data->u_base[u_inc].requested_xid.ku.valid ?
                llc_get_ffs_data(XID_KU, i_inc): XID_KU_MAX)) )
          {
            rc = TRUE;
            TRACE_EVENT ("kU value accepted");
          }
          else
          {
            TRACE_EVENT ("kU value NOT accepted");
          }
          break;
        default:
          TRACE_ERROR ("invalid SAPI value for kU");
          break;
      }
      break;

    case XID_LAYER_3:
      /*
       * Must not be checked, ignored.
       */
      TRACE_EVENT ("Layer-3 values are not checked.");
      break;

    case XID_RESET:
      /*
       * Must not be checked, ignored.
       */
      TRACE_EVENT ("Reset is not checked.");
      break;

    default:
      TRACE_ERROR ("unknown XID parameter");
      break;
  }

  return rc;
} /* llc_xid_value_acceptable() */


/*
+------------------------------------------------------------------------------
| Function    : llc_palloc_desc
+------------------------------------------------------------------------------
| Description : This function allocates a descriptor of type T_DESC3
|
| Parameters  : len -  length of descriptor
|             : offset offset of descriptor
|
| Return      : poniter to T_DESC3
|
+------------------------------------------------------------------------------
*/
#ifdef LL_DESC
GLOBAL T_desc3* llc_palloc_desc( U16 len, U16 offset)
{
        T_desc3 *desc3;
        U8 *buffer;

        TRACE_FUNCTION ("llc_palloc_desc");

        MALLOC (desc3, (USHORT)(sizeof(T_desc3)));
        
        MALLOC (buffer, len + offset);

        desc3->next = NULL;
        desc3->offset = offset;
        desc3->len = len;
        desc3->buffer = (ULONG)buffer;

        return desc3;
}

/*
+------------------------------------------------------------------------------
| Function    : llc_cl_desc3_free
+------------------------------------------------------------------------------
| Description : Frees the descriptor connected to the desc3 descriptor. This 
|               free will when applicable cause the frame to decrease a 
|               connected counter attached to the allocation or really free
|               the memory in case the counter is zero.
|
| Parameters  : T_desc3* pointer to desc3 descriptor.
|
+------------------------------------------------------------------------------
*/
GLOBAL void llc_cl_desc3_free(T_desc3* p_desc3)
{
  T_desc3* help;
  while(p_desc3)
  {
    help = (T_desc3*)p_desc3->next;
    MFREE(p_desc3->buffer);
    MFREE(p_desc3);
    p_desc3 = help;
  }
}
#endif /* LL_DESC */

#ifndef TI_PS_OP_CIPH_DRIVER
/*
+------------------------------------------------------------------------------
| Function    : llc_fbs_init
+------------------------------------------------------------------------------
| Description : The function fbs_init() initializes service 
|               variablaes
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void llc_fbs_init ( void )
{ 
  TRACE_FUNCTION( "llc_fbs_init" );
  llc_data->fbs.initialized = FALSE;
}
#endif
/*
+------------------------------------------------------------------------------
| Function    : llc_copy_ul_data_to_list
+------------------------------------------------------------------------------
| Description : The function copy_data_to_list copies the length and the pointer 
|               of the linked descriptor list to the in_data_list.
|               It is used in uplink direction
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/

GLOBAL void llc_copy_ul_data_to_list
( 
 T_CCI_CIPHER_DESC_REQ *cipher_req,
 T_CIPH_in_data_list *in_data_list
)
{ 
  
  T_desc3*           cipher_desc;
  U16                cnt = 0;
  T_CIPH_in_data*    in_data_array;


  /*
   * Count the number of descriptors in the incoming desc_list
   */
  cipher_desc = (T_desc3*)cipher_req->desc_list3.first;
  while (cipher_desc){
    cipher_desc = (T_desc3*)cipher_desc->next;  
    cnt ++;
  }
  /*
   * Allocate an array for T_CIPH_in_data_list 
   */
  MALLOC(in_data_array,(sizeof(T_CIPH_in_data) * cnt));
  in_data_list->ptr_in_data = in_data_array;

  /*
   * Copy buf and len parameters from descriptors to the in_data's
   */
  cipher_desc = (T_desc3*)cipher_req->desc_list3.first;
  cnt = 0;
  while (cipher_desc)
  {
    in_data_list->ptr_in_data[cnt].buf = 
      (U32)&((U8*)cipher_desc->buffer)[cipher_desc->offset];
    in_data_list->ptr_in_data[cnt].len = cipher_desc->len;

    cipher_desc = (T_desc3*)cipher_desc->next;  
    cnt++;
  }
  in_data_list->c_in_data = cnt;  
  
} /* llc_copy_ul_data_to_list */

/*
+------------------------------------------------------------------------------
| Function    : llc_copy_dl_data_to_list
+------------------------------------------------------------------------------
| Description : The function copy_data_to_list copies the length and the pointer 
|               of the linked descriptor list to the in_data_list.
|               It is used in downlink direction
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/

GLOBAL void llc_copy_dl_data_to_list
( 
 T_CCI_DECIPHER_REQ *decipher_req,
 T_CIPH_in_data_list *in_data_list
)
{ 
  
  T_desc* decipher_desc;
  T_CIPH_in_data*   in_data_array;
  USHORT   cnt = 0;

  /*
   * Count the number of descriptors in the incoming desc_list
   */
  decipher_desc = (T_desc*)decipher_req->desc_list.first;
  while (decipher_desc){
    decipher_desc = (T_desc*)decipher_desc->next;  
    cnt ++;  
  }
  
  /*
   * Allocate an array of for T_CIPH_in_data 
   */
  MALLOC(in_data_array, (sizeof(T_CIPH_in_data) * cnt));
  in_data_list->ptr_in_data = in_data_array;

  /*
   * Copy buf and len parameters from descriptors to the in_data's
   */
  decipher_desc = (T_desc*)decipher_req->desc_list.first;
  cnt = 0;
  while (decipher_desc)
  {
    in_data_list->ptr_in_data[cnt].buf = (U32)decipher_desc->buffer;
    in_data_list->ptr_in_data[cnt].len = decipher_desc->len;

    decipher_desc = (T_desc*)decipher_desc->next;  
    cnt++;
  }
  in_data_list->c_in_data = cnt;  
  
} /* llc_copy_dl_data_to_list */

/*
+------------------------------------------------------------------------------
| Function    : llc_fbs_enable_cci_info_trace
+------------------------------------------------------------------------------
| Description : The llc_fbs_enable_cci_info_trace sets the variable 
|               llc_data->fbs.cci_info_trace to TRUE 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void llc_fbs_enable_cci_info_trace ( void )
{ 
  TRACE_FUNCTION( "llc_fbs_enable_cci_info_trace" );
  llc_data->fbs.cci_info_trace = TRUE;
}

#ifdef LLC_TRACE_CIPHERING
/*
+------------------------------------------------------------------------------
| Function    : llc_trace_desc_list3_content
+------------------------------------------------------------------------------
| Description : traces content of a desc3 descriptor list
|
| Parameters  : desc_list3 descriptor list
|
+------------------------------------------------------------------------------
*/
GLOBAL void llc_trace_desc_list3_content(T_desc_list3 desc_list3)
{
  U16 current_pos = 0;  
  U16 dif;  
  U16 data_len = 0;     /* The length of the data to be traced including data offset */
  U8* p_data = NULL;    /* Pointer to byte data element */
  T_desc3* p_desc3 = (T_desc3*)desc_list3.first; /* Pointer to the actual desc3 descriptor element */

  while(p_desc3 != NULL)
  {
    current_pos = p_desc3->offset;
    dif=0;
    p_data = (U8*)p_desc3->buffer;
    data_len = current_pos + p_desc3->len;
    while(current_pos < data_len)
    {
      if (current_pos +8 <= data_len)
      {
        TRACE_EVENT_P8
          ("%02x %02x %02x %02x %02x %02x %02x %02x",
            p_data[current_pos],
            p_data[current_pos+1],
            p_data[current_pos+2],
            p_data[current_pos+3],
            p_data[current_pos+4],
            p_data[current_pos+5],
            p_data[current_pos+6],
            p_data[current_pos+7]
          );
        current_pos += 8;
      }
      else
      {
        dif = data_len - current_pos;
        switch(dif)
        {
          case 1:
            TRACE_EVENT_P1
            ("%02x",
              p_data[current_pos]
            );
            current_pos += 1;
            break;
          case 2:
            TRACE_EVENT_P2
            ("%02x %02x",
              p_data[current_pos],
              p_data[current_pos+1]
            );
            current_pos += 2;
            break;
          case 3:
            TRACE_EVENT_P3
            ("%02x %02x %02x",
              p_data[current_pos],
              p_data[current_pos+1],
              p_data[current_pos+2]
            );
            current_pos += 3;
            break;
          case 4:
            TRACE_EVENT_P4
            ("%02x %02x %02x %02x",
              p_data[current_pos],
              p_data[current_pos+1],
              p_data[current_pos+2],
              p_data[current_pos+3]
            );
            current_pos += 4;
            break;
          case 5:
            TRACE_EVENT_P5
            ("%02x %02x %02x %02x %02x",
              p_data[current_pos],
              p_data[current_pos+1],
              p_data[current_pos+2],
              p_data[current_pos+3],
              p_data[current_pos+4]
            );
            current_pos += 5;
            break;
          case 6:
            TRACE_EVENT_P6
            ("%02x %02x %02x %02x %02x %02x",
              p_data[current_pos],
              p_data[current_pos+1],
              p_data[current_pos+2],
              p_data[current_pos+3],
              p_data[current_pos+4],
              p_data[current_pos+5]
            );
            current_pos += 6;
            break;
          case 7:
            TRACE_EVENT_P7
            ("%02x %02x %02x %02x %02x %02x %02x",
              p_data[current_pos],
              p_data[current_pos+1],
              p_data[current_pos+2],
              p_data[current_pos+3],
              p_data[current_pos+4],
              p_data[current_pos+5],
              p_data[current_pos+6]
            );
            current_pos += 7;
            break;
        }  

      }
    }

    p_desc3 = (T_desc3*)p_desc3->next;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : llc_trace_sdu
+------------------------------------------------------------------------------
| Description : traces content of one sdu
|
| Parameters  : pointer to sdu
|
+------------------------------------------------------------------------------
*/
GLOBAL void llc_trace_sdu(T_sdu* sdu)
{

  USHORT pos = sdu->o_buf >> 3;
  USHORT  frame_len = (sdu->l_buf + 7) / 8;


  TRACE_FUNCTION("llc_trace_sdu");

  while(pos < (frame_len + (sdu->o_buf >> 3)))
  {
    if (pos + 8 <= (frame_len + (sdu->o_buf >> 3))) {
      TRACE_EVENT_P8
        ("%02x %02x %02x %02x %02x %02x %02x %02x",
          sdu->buf[pos],
          sdu->buf[pos + 1],
          sdu->buf[pos + 2],
          sdu->buf[pos + 3],
          sdu->buf[pos + 4],
          sdu->buf[pos + 5],
          sdu->buf[pos + 6],
          sdu->buf[pos + 7]
        );
      pos += 8;
    } else if (pos + 7 <= (frame_len + (sdu->o_buf >> 3))){
      TRACE_EVENT_P7
        ("%02x %02x %02x %02x %02x %02x %02x",
          sdu->buf[pos],
          sdu->buf[pos + 1],
          sdu->buf[pos + 2],
          sdu->buf[pos + 3],
          sdu->buf[pos + 4],
          sdu->buf[pos + 5],
          sdu->buf[pos + 6]
        );
      pos += 7;
    } else if (pos + 6 <= (frame_len + (sdu->o_buf >> 3))){
      TRACE_EVENT_P6
        ("%02x %02x %02x %02x %02x %02x",
          sdu->buf[pos],
          sdu->buf[pos + 1],
          sdu->buf[pos + 2],
          sdu->buf[pos + 3],
          sdu->buf[pos + 4],
          sdu->buf[pos + 5]
        );
      pos += 6;
    } else if (pos + 5 <= (frame_len + (sdu->o_buf >> 3))){
      TRACE_EVENT_P5
        ("%02x %02x %02x %02x %02x",
          sdu->buf[pos],
          sdu->buf[pos + 1],
          sdu->buf[pos + 2],
          sdu->buf[pos + 3],
          sdu->buf[pos + 4]
        );
      pos += 5;
    } else if (pos + 4 <= (frame_len + (sdu->o_buf >> 3))){
      TRACE_EVENT_P4
        ("%02x %02x %02x %02x",
          sdu->buf[pos],
          sdu->buf[pos + 1],
          sdu->buf[pos + 2],
          sdu->buf[pos + 3]
        );
      pos += 4;
    } else if (pos + 3 <= (frame_len + (sdu->o_buf >> 3))){
      TRACE_EVENT_P3
        ("%02x %02x %02x",
          sdu->buf[pos],
          sdu->buf[pos + 1],
          sdu->buf[pos + 2]
        );
      pos += 3;
    } else if (pos + 2 <= (frame_len + (sdu->o_buf >> 3))){
      TRACE_EVENT_P2
        ("%02x %02x",
          sdu->buf[pos],
          sdu->buf[pos + 1]
        );
      pos += 2;
    } else if (pos + 1 <= (frame_len + (sdu->o_buf >> 3))){
      TRACE_EVENT_P1
        ("%02x",
          sdu->buf[pos]
        );
      pos++;
    }

  }
}

/*
+------------------------------------------------------------------------------
| Function    : llc_trace_desc_list
+------------------------------------------------------------------------------
| Description : traces content of one desc_list
|
| Parameters  : pointer to desc_list
|
+------------------------------------------------------------------------------
*/
GLOBAL void llc_trace_desc_list(T_desc_list* desc_list)
{
  USHORT  frame_len = desc_list->list_len;
  T_desc* desc = (T_desc*)desc_list->first;
  USHORT  list_pos = 0;
  USHORT  desc_pos = 0;

  TRACE_FUNCTION("llc_trace_desc_list");

  while(list_pos < frame_len)
  {
    if (desc != NULL) {
      if (desc_pos >= desc->len) {
        desc_pos = 0;
        desc = (T_desc*)desc->next;
      }
    }
    if (desc == NULL) {
      return;
    }
    if (desc_pos + 8 <= desc->len) {
      TRACE_EVENT_P8 ("%02x %02x %02x %02x %02x %02x %02x %02x ",
                      desc->buffer[desc_pos],
                      desc->buffer[desc_pos + 1],
                      desc->buffer[desc_pos + 2],
                      desc->buffer[desc_pos + 3],
                      desc->buffer[desc_pos + 4],
                      desc->buffer[desc_pos + 5],
                      desc->buffer[desc_pos + 6],
                      desc->buffer[desc_pos + 7]
                     );
      list_pos+= 8;
      desc_pos+= 8;
    } else if (desc_pos + 7 <= desc->len) {
      TRACE_EVENT_P7 ("%02x %02x %02x %02x %02x %02x %02x ",
                      desc->buffer[desc_pos],
                      desc->buffer[desc_pos + 1],
                      desc->buffer[desc_pos + 2],
                      desc->buffer[desc_pos + 3],
                      desc->buffer[desc_pos + 4],
                      desc->buffer[desc_pos + 5],
                      desc->buffer[desc_pos + 6]
                     );
      list_pos+= 7;
      desc_pos+= 7;
    } else if (desc_pos + 6 <= desc->len) {
      TRACE_EVENT_P6 ("%02x %02x %02x %02x %02x %02x ",
                      desc->buffer[desc_pos],
                      desc->buffer[desc_pos + 1],
                      desc->buffer[desc_pos + 2],
                      desc->buffer[desc_pos + 3],
                      desc->buffer[desc_pos + 4],
                      desc->buffer[desc_pos + 5]
                     );
      list_pos+= 6;
      desc_pos+= 6;
    } else if (desc_pos + 5 <= desc->len) {
      TRACE_EVENT_P5 ("%02x %02x %02x %02x %02x ",
                      desc->buffer[desc_pos],
                      desc->buffer[desc_pos + 1],
                      desc->buffer[desc_pos + 2],
                      desc->buffer[desc_pos + 3],
                      desc->buffer[desc_pos + 4]
                     );
      list_pos+= 5;
      desc_pos+= 5;
    } else if (desc_pos + 4 <= desc->len) {
      TRACE_EVENT_P4 ("%02x %02x %02x %02x ",
                      desc->buffer[desc_pos],
                      desc->buffer[desc_pos + 1],
                      desc->buffer[desc_pos + 2],
                      desc->buffer[desc_pos + 3]
                     );
      list_pos+= 4;
      desc_pos+= 4;
    } else if (desc_pos + 3 <= desc->len) {
      TRACE_EVENT_P3 ("%02x %02x %02x ",
                      desc->buffer[desc_pos],
                      desc->buffer[desc_pos + 1],
                      desc->buffer[desc_pos + 2]
                     );
      list_pos+= 3;
      desc_pos+= 3;
    } else if (desc_pos + 2 <= desc->len) {
      TRACE_EVENT_P2 ("%02x %02x ",
                      desc->buffer[desc_pos],
                      desc->buffer[desc_pos + 1]
                     );
      list_pos+= 2;
      desc_pos+= 2;
    } else if (desc_pos + 1 <= desc->len) {
      TRACE_EVENT_P1 ("%02x ",
                      desc->buffer[desc_pos]
                     );
      list_pos++;
      desc_pos++;
    }
  } /* while(list_pos < frame_len) */
}
#endif /*LLC_TRACE_CIPHERING */

