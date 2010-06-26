/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  t30_kerf
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
|  Purpose :  This Modul defines the procedures and functions for
|             the component T30 of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#ifndef T30_KERF_C
#define T30_KERF_C
#endif

#define ENTITY_T30

/*==== INCLUDES ===================================================*/

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "pcm.h"
#include "vsi.h"
#include "macdef.h"
#include "pconst.cdg"
#include "mconst.cdg"
#include "message.h"
#include "ccdapi.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"
#include "cnf_t30.h"
#include "mon_t30.h"
#include "pei.h"
#include "tok.h"
#include "dti.h"      /* functionality of the dti library */
#include "t30.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_init            |
+--------------------------------------------------------------------+

  PURPOSE : initialize kernel

*/

GLOBAL void ker_init (T_T30_DATA *pt30_data)
{
  TRACE_FUNCTION ("ker_init()");

  memset (pt30_data, 0, sizeof (T_T30_DATA));
  SET_STATE (KER, T30_NULL);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_check_rtc       |
+--------------------------------------------------------------------+

  PURPOSE : This function scans the received fax data for RTC.

*/

GLOBAL UBYTE ker_check_rtc (T_FAD_DATA_IND *fad_data_ind)
{
  USHORT i;
  UBYTE bit_ptr;

  TRACE_FUNCTION ("ker_check_rtc()");

#ifdef _SIMULATION_ /* RTC checking OFF */
  if (t30_data->test_mode & TST_RTC)
    return RTC_NO;
#endif

  if (fad_data_ind EQ NULL)
  {
    t30_data->rtc_zero = 0;
    t30_data->rtc_eol  = 0;
    return RTC_NO;
  }

  for (i = 0 ; i < (fad_data_ind->sdu.l_buf >> 3); i++)
  {
    UBYTE ch = fad_data_ind->sdu.buf[i];
    if (!ch)
    {
      t30_data->rtc_zero += 8;
      continue;
    }
    bit_ptr = 0x80;
    do
    {
      if (ch & bit_ptr)
      {
        if (t30_data->rtc_zero >= 11)
          t30_data->eol++;

        if (t30_data->rtc_zero EQ 11)
        {
          if (++t30_data->rtc_eol EQ 6)
          {
            t30_data->rtc_eol  = 0;
            t30_data->rtc_zero = 0;
            t30_data->eol -= 5;
            return RTC_YES;
          }
        }
        else
        {
          if (t30_data->rtc_eol)
            t30_data->rtc_eol = 0;
          else if (t30_data->rtc_zero >= 11)
            t30_data->rtc_eol++;
        }
        t30_data->rtc_zero = 0;
      }
      else
        t30_data->rtc_zero++;

      bit_ptr >>= 1;
    }
    while (bit_ptr);
  }
  return RTC_NO;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dcs_cap0   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the mandatory fax capabilities.

*/

LOCAL void ker_fill_dcs_cap0 (T_BCS_DCS *bcs_dcs)
{
#define X t30_data->hdlc_rcv.dcs
#define Y bcs_dcs->cap0_snd

  TRACE_FUNCTION ("ker_fill_dcs_cap0()");

  X.rec_fax_op     = Y.rec_fax_op;
  X.data_sig_rate  = Y.data_sig_rate;
  X.R8_lines_pels  = Y.R8_lines_pels;
  X.two_dim_coding = Y.two_dim_coding;
  X.rec_width      = Y.rec_width;
  X.max_rec_len    = Y.max_rec_len;
  X.min_scan_time  = Y.min_scan_time;

  /*
  t30_data->trans_rate = Y.data_sig_rate;
  */
#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dcs_cap1   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 1.

*/

LOCAL void ker_fill_dcs_cap1 (T_BCS_DCS *bcs_dcs)
{
#define X t30_data->hdlc_rcv.dcs
#define Y bcs_dcs->cap1_snd

  TRACE_FUNCTION ("ker_fill_dcs_cap1()");

  if (bcs_dcs->v_cap1_snd)
  {
    X.uncomp_mode   = Y.uncomp_mode;
    X.err_corr_mode = Y.err_corr_mode;
    X.frame_size    = Y.frame_size;
    X.t6_coding     = Y.t6_coding;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dcs_cap2   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 2.

*/

LOCAL void ker_fill_dcs_cap2 (T_BCS_DCS *bcs_dcs)
{
#define X t30_data->hdlc_rcv.dcs
#define Y bcs_dcs->cap2_snd

  TRACE_FUNCTION ("ker_fill_dcs_cap2()");

  if (bcs_dcs->v_cap2_snd)
  {
    /* for future use */
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dcs_cap3   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 3.

*/

LOCAL void ker_fill_dcs_cap3 (T_BCS_DCS *bcs_dcs)
{
#define X t30_data->hdlc_rcv.dcs
#define Y bcs_dcs->cap3_snd

  TRACE_FUNCTION ("ker_fill_dcs_cap3()");

  if (bcs_dcs->v_cap3_snd)
  {
    X.R8_lines        = Y.R8_lines;
    X.r_300_pels      = Y.r_300_pels;
    X.R16_lines_pels  = Y.R16_lines_pels;
    X.resolution_type = Y.resolution_type;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dcs_cap4   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 4.

*/

LOCAL void ker_fill_dcs_cap4 (T_BCS_DCS *bcs_dcs)
{
#define X t30_data->hdlc_rcv.dcs
#define Y bcs_dcs->cap4_snd

  TRACE_FUNCTION ("ker_fill_dcs_cap4()");

  if (bcs_dcs->v_cap4_snd)
  {
    X.subaddr  = Y.subaddr;
    X.password = Y.password;
    X.bft      = Y.bft;
    X.dtm      = Y.dtm;
    X.edi      = Y.edi;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dcs_cap5   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 5.

*/

LOCAL void ker_fill_dcs_cap5 (T_BCS_DCS *bcs_dcs)
{
#define X t30_data->hdlc_rcv.dcs
#define Y bcs_dcs->cap5_snd

  TRACE_FUNCTION ("ker_fill_dcs_cap5()");

  if (bcs_dcs->v_cap5_snd)
  {
    X.btm        = Y.btm;
    X.char_mode  = Y.char_mode;
    X.mixed_mode = Y.mixed_mode;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dcs_cap6   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 6.

*/

LOCAL void ker_fill_dcs_cap6 (T_BCS_DCS *bcs_dcs)
{
#define X t30_data->hdlc_rcv.dcs
#define Y bcs_dcs->cap6_snd

  TRACE_FUNCTION ("ker_fill_dcs_cap6()");

  if (bcs_dcs->v_cap6_snd)
  {
    X.proc_mode_26       = Y.proc_mode_26;
    X.dig_network_cap    = Y.dig_network_cap;
    X.duplex             = Y.duplex;
    X.jpeg               = Y.jpeg;
    X.full_colour        = Y.full_colour;
    X.huffman_tables     = Y.huffman_tables;
    X.r_12_bits_pel_comp = Y.r_12_bits_pel_comp;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dcs_cap7   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 7.

*/

LOCAL void ker_fill_dcs_cap7 (T_BCS_DCS *bcs_dcs)
{
#define X t30_data->hdlc_rcv.dcs
#define Y bcs_dcs->cap7_snd

  TRACE_FUNCTION ("ker_fill_dcs_cap7()");

  if (bcs_dcs->v_cap7_snd)
  {
    X.no_subsamp                 = Y.no_subsamp;
    X.cust_illum                 = Y.cust_illum;
    X.cust_gamut                 = Y.cust_gamut;
    X.na_letter                  = Y.na_letter;
    X.na_legal                   = Y.na_legal;
    X.sing_prog_seq_coding_basic = Y.sing_prog_seq_coding_basic;
    X.sing_prog_seq_coding_L0    = Y.sing_prog_seq_coding_L0;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dcs_info   |
+--------------------------------------------------------------------+

  PURPOSE : This function fills the hdlc-frame-buffer with the
            received DCS-parameters.

*/

GLOBAL void ker_fill_dcs_info (T_BCS_DCS *bcs_dcs)
{
  TRACE_FUNCTION ("ker_fill_dcs_info()");

  memset (&t30_data->hdlc_rcv.dcs, 0xFF,
          sizeof (t30_data->hdlc_rcv.dcs));

  t30_data->hdlc_rcv.v_dcs = 1;

  ker_fill_dcs_cap0 (bcs_dcs);
  ker_fill_dcs_cap1 (bcs_dcs);
  ker_fill_dcs_cap2 (bcs_dcs);
  ker_fill_dcs_cap3 (bcs_dcs);
  ker_fill_dcs_cap4 (bcs_dcs);
  ker_fill_dcs_cap5 (bcs_dcs);
  ker_fill_dcs_cap6 (bcs_dcs);
  ker_fill_dcs_cap7 (bcs_dcs);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dis_cap0   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the mandatory fax capabilities.

*/

LOCAL void ker_fill_dis_cap0 (T_BCS_DIS *bcs_dis)
{
#define X t30_data->hdlc_rcv.dis
#define Y bcs_dis->cap0_rcv

  TRACE_FUNCTION ("ker_fill_dis_cap0()");

  X.v8             = Y.v8;
  X.n_byte         = Y.n_byte;
  X.ready_tx_fax   = Y.ready_tx_fax;
  X.rec_fax_op     = Y.rec_fax_op;
  X.data_sig_rate  = Y.data_sig_rate;
  X.R8_lines_pels  = Y.R8_lines_pels;
  X.two_dim_coding = Y.two_dim_coding;
  X.rec_width      = Y.rec_width;
  X.max_rec_len    = Y.max_rec_len;
  X.min_scan_time  = Y.min_scan_time;

  /*
  t30_data->trans_rate = Y.data_sig_rate;
  */
#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dis_cap1   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 1.

*/

LOCAL void ker_fill_dis_cap1 (T_BCS_DIS *bcs_dis)
{
#define X t30_data->hdlc_rcv.dis
#define Y bcs_dis->cap1_rcv

  TRACE_FUNCTION ("ker_fill_dis_cap1()");

  if (bcs_dis->v_cap1_rcv)
  {
    X.uncomp_mode   = Y.uncomp_mode;
    X.err_corr_mode = Y.err_corr_mode;
    X.t6_coding     = Y.t6_coding;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dis_cap2   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 2.

*/

LOCAL void ker_fill_dis_cap2 (T_BCS_DIS *bcs_dis)
{
#define X t30_data->hdlc_rcv.dis
#define Y bcs_dis->cap2_rcv

  TRACE_FUNCTION ("ker_fill_dis_cap2()");

  if (bcs_dis->v_cap2_rcv)
  {
    /* for future use */
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dis_cap3   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 3.

*/

LOCAL void ker_fill_dis_cap3 (T_BCS_DIS *bcs_dis)
{
#define X t30_data->hdlc_rcv.dis
#define Y bcs_dis->cap3_rcv

  TRACE_FUNCTION ("ker_fill_dis_cap3()");

  if (bcs_dis->v_cap3_rcv)
  {
    X.R8_lines         = Y.R8_lines;
    X.r_300_pels       = Y.r_300_pels;
    X.R16_lines_pels   = Y.R16_lines_pels;
    X.i_res_pref       = Y.i_res_pref;
    X.m_res_pref       = Y.m_res_pref;
    X.min_scan_time_hr = Y.min_scan_time_hr;
    X.sel_polling      = Y.sel_polling;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dis_cap4   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 4.

*/

LOCAL void ker_fill_dis_cap4 (T_BCS_DIS *bcs_dis)
{
#define X t30_data->hdlc_rcv.dis
#define Y bcs_dis->cap4_rcv

  TRACE_FUNCTION ("ker_fill_dis_cap4()");

  if (bcs_dis->v_cap4_rcv)
  {
    X.subaddr      = Y.subaddr;
    X.password     = Y.password;
    X.ready_tx_doc = Y.ready_tx_doc;
    X.bft          = Y.bft;
    X.dtm          = Y.dtm;
    X.edi          = Y.edi;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dis_cap5   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 5.

*/

LOCAL void ker_fill_dis_cap5 (T_BCS_DIS *bcs_dis)
{
#define X t30_data->hdlc_rcv.dis
#define Y bcs_dis->cap5_rcv

  TRACE_FUNCTION ("ker_fill_dis_cap5()");

  if (bcs_dis->v_cap5_rcv)
  {
    X.btm            = Y.btm;
    X.ready_tx_mixed = Y.ready_tx_mixed;
    X.char_mode      = Y.char_mode;
    X.mixed_mode     = Y.mixed_mode;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dis_cap6   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 6.

*/

LOCAL void ker_fill_dis_cap6 (T_BCS_DIS *bcs_dis)
{
#define X t30_data->hdlc_rcv.dis
#define Y bcs_dis->cap6_rcv

  TRACE_FUNCTION ("ker_fill_dis_cap6()");

  if (bcs_dis->v_cap6_rcv)
  {
    X.proc_mode_26       = Y.proc_mode_26;
    X.dig_network_cap    = Y.dig_network_cap;
    X.duplex             = Y.duplex;
    X.jpeg               = Y.jpeg;
    X.full_colour        = Y.full_colour;
    X.r_12_bits_pel_comp = Y.r_12_bits_pel_comp;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dis_cap7   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 7.

*/

LOCAL void ker_fill_dis_cap7 (T_BCS_DIS *bcs_dis)
{
#define X t30_data->hdlc_rcv.dis
#define Y bcs_dis->cap7_rcv

  TRACE_FUNCTION ("ker_fill_dis_cap7()");

  if (bcs_dis->v_cap7_rcv)
  {
    X.no_subsamp                 = Y.no_subsamp;
    X.cust_illum                 = Y.cust_illum;
    X.cust_gamut                 = Y.cust_gamut;
    X.na_letter                  = Y.na_letter;
    X.na_legal                   = Y.na_legal;
    X.sing_prog_seq_coding_basic = Y.sing_prog_seq_coding_basic;
    X.sing_prog_seq_coding_L0    = Y.sing_prog_seq_coding_L0;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dis_info   |
+--------------------------------------------------------------------+

  PURPOSE : This function fills the hdlc-frame-buffer with the
            received DIS-parameters.

*/

GLOBAL void ker_fill_dis_info (T_BCS_DIS *bcs_dis)
{
  TRACE_FUNCTION ("ker_fill_dis_info()");

  memset (&t30_data->hdlc_rcv.dis, 0xFF,
          sizeof (t30_data->hdlc_rcv.dis));

  t30_data->hdlc_rcv.v_dis = 1;

  ker_fill_dis_cap0 (bcs_dis);
  ker_fill_dis_cap1 (bcs_dis);
  ker_fill_dis_cap2 (bcs_dis);
  ker_fill_dis_cap3 (bcs_dis);
  ker_fill_dis_cap4 (bcs_dis);
  ker_fill_dis_cap5 (bcs_dis);
  ker_fill_dis_cap6 (bcs_dis);
  ker_fill_dis_cap7 (bcs_dis);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dtc_cap0   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the mandatory fax capabilities.

*/

LOCAL void ker_fill_dtc_cap0 (T_BCS_DTC *bcs_dtc)
{
#define X t30_data->hdlc_rcv.dtc
#define Y bcs_dtc->cap0_rcv

  TRACE_FUNCTION ("ker_fill_dtc_cap0()");

  X.v8             = Y.v8;
  X.n_byte         = Y.n_byte;
  X.ready_tx_fax   = Y.ready_tx_fax;
  X.rec_fax_op     = Y.rec_fax_op;
  X.data_sig_rate  = Y.data_sig_rate;
  X.R8_lines_pels  = Y.R8_lines_pels;
  X.two_dim_coding = Y.two_dim_coding;
  X.rec_width      = Y.rec_width;
  X.max_rec_len    = Y.max_rec_len;
  X.min_scan_time  = Y.min_scan_time;

  /*
  t30_data->trans_rate = Y.data_sig_rate;
  */
#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dtc_cap1   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 1.

*/

LOCAL void ker_fill_dtc_cap1 (T_BCS_DTC *bcs_dtc)
{
#define X t30_data->hdlc_rcv.dtc
#define Y bcs_dtc->cap1_rcv

  TRACE_FUNCTION ("ker_fill_dtc_cap1()");

  if (bcs_dtc->v_cap1_rcv)
  {
    X.uncomp_mode   = Y.uncomp_mode;
    X.err_corr_mode = Y.err_corr_mode;
    X.t6_coding     = Y.t6_coding;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dtc_cap2   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 2.

*/

LOCAL void ker_fill_dtc_cap2 (T_BCS_DTC *bcs_dtc)
{
#define X t30_data->hdlc_rcv.dtc
#define Y bcs_dtc->cap2_rcv

  TRACE_FUNCTION ("ker_fill_dtc_cap2()");

  if (bcs_dtc->v_cap2_rcv)
  {
    /* for future use */
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dtc_cap3   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 3.

*/

LOCAL void ker_fill_dtc_cap3 (T_BCS_DTC *bcs_dtc)
{
#define X t30_data->hdlc_rcv.dtc
#define Y bcs_dtc->cap3_rcv

  TRACE_FUNCTION ("ker_fill_dtc_cap3()");

  if (bcs_dtc->v_cap3_rcv)
  {
    X.R8_lines         = Y.R8_lines;
    X.r_300_pels       = Y.r_300_pels;
    X.R16_lines_pels   = Y.R16_lines_pels;
    X.i_res_pref       = Y.i_res_pref;
    X.m_res_pref       = Y.m_res_pref;
    X.min_scan_time_hr = Y.min_scan_time_hr;
    X.sel_polling      = Y.sel_polling;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dtc_cap4   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 4.

*/

LOCAL void ker_fill_dtc_cap4 (T_BCS_DTC *bcs_dtc)
{
#define X t30_data->hdlc_rcv.dtc
#define Y bcs_dtc->cap4_rcv

  TRACE_FUNCTION ("ker_fill_dtc_cap4()");

  if (bcs_dtc->v_cap4_rcv)
  {
    X.subaddr      = Y.subaddr;
    X.password     = Y.password;
    X.ready_tx_doc = Y.ready_tx_doc;
    X.bft          = Y.bft;
    X.dtm          = Y.dtm;
    X.edi          = Y.edi;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dtc_cap5   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 5.

*/

LOCAL void ker_fill_dtc_cap5 (T_BCS_DTC *bcs_dtc)
{
#define X t30_data->hdlc_rcv.dtc
#define Y bcs_dtc->cap5_rcv

  TRACE_FUNCTION ("ker_fill_dtc_cap5()");

  if (bcs_dtc->v_cap5_rcv)
  {
    X.btm            = Y.btm;
    X.ready_tx_mixed = Y.ready_tx_mixed;
    X.char_mode      = Y.char_mode;
    X.mixed_mode     = Y.mixed_mode;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dtc_cap6   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 6.

*/

LOCAL void ker_fill_dtc_cap6 (T_BCS_DTC *bcs_dtc)
{
#define X t30_data->hdlc_rcv.dtc
#define Y bcs_dtc->cap6_rcv

  TRACE_FUNCTION ("ker_fill_dtc_cap6()");

  if (bcs_dtc->v_cap6_rcv)
  {
    X.proc_mode_26       = Y.proc_mode_26;
    X.dig_network_cap    = Y.dig_network_cap;
    X.duplex             = Y.duplex;
    X.jpeg               = Y.jpeg;
    X.full_colour        = Y.full_colour;
    X.r_12_bits_pel_comp = Y.r_12_bits_pel_comp;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dtc_cap7   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 7.

*/

LOCAL void ker_fill_dtc_cap7 (T_BCS_DTC *bcs_dtc)
{
#define X t30_data->hdlc_rcv.dtc
#define Y bcs_dtc->cap7_rcv

  TRACE_FUNCTION ("ker_fill_dtc_cap7()");

  if (bcs_dtc->v_cap7_rcv)
  {
    X.no_subsamp                 = Y.no_subsamp;
    X.cust_illum                 = Y.cust_illum;
    X.cust_gamut                 = Y.cust_gamut;
    X.na_letter                  = Y.na_letter;
    X.na_legal                   = Y.na_legal;
    X.sing_prog_seq_coding_basic = Y.sing_prog_seq_coding_basic;
    X.sing_prog_seq_coding_L0    = Y.sing_prog_seq_coding_L0;
  }

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_fill_dtc_info   |
+--------------------------------------------------------------------+

  PURPOSE : This function fills the hdlc-frame-buffer with the
            received DTC-parameters.

*/

GLOBAL void ker_fill_dtc_info (T_BCS_DTC *bcs_dtc)
{
  TRACE_FUNCTION ("ker_fill_dtc_info()");

  memset (&t30_data->hdlc_rcv.dtc, 0xFF,
          sizeof (t30_data->hdlc_rcv.dtc));

  t30_data->hdlc_rcv.v_dtc = 1;

  ker_fill_dtc_cap0 (bcs_dtc);
  ker_fill_dtc_cap1 (bcs_dtc);
  ker_fill_dtc_cap2 (bcs_dtc);
  ker_fill_dtc_cap3 (bcs_dtc);
  ker_fill_dtc_cap4 (bcs_dtc);
  ker_fill_dtc_cap5 (bcs_dtc);
  ker_fill_dtc_cap6 (bcs_dtc);
  ker_fill_dtc_cap7 (bcs_dtc);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dcs_cap0   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the mandatory fax capabilities.

*/

LOCAL void ker_prep_dcs_cap0 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DCS     *bcs_dcs)
{
#define X bcs_dcs->cap0_snd
#define Y t30_cap_req->hdlc_info.dcs

  TRACE_FUNCTION ("ker_prep_dcs_cap0()");

  X.rec_fax_op     = Y.rec_fax_op;
  X.data_sig_rate  = Y.data_sig_rate;
  X.R8_lines_pels  = Y.R8_lines_pels;
  X.two_dim_coding = Y.two_dim_coding;
  X.rec_width      = Y.rec_width;
  X.max_rec_len    = Y.max_rec_len;
  X.min_scan_time  = Y.min_scan_time;

  /*
  t30_data->trans_rate = Y.data_sig_rate;
  */
#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dcs_cap1   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 1.

*/

LOCAL void ker_prep_dcs_cap1 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DCS     *bcs_dcs)
{
#define X bcs_dcs->cap1_snd
#define Y t30_cap_req->hdlc_info.dcs

  TRACE_FUNCTION ("ker_prep_dcs_cap1()");

  if (Y.uncomp_mode != 0xFF)
  {
    bcs_dcs->v_cap1_snd = TRUE;

    X.uncomp_mode       = Y.uncomp_mode;
    X.err_corr_mode     = Y.err_corr_mode;
    X.frame_size        = Y.frame_size;
    X.t6_coding         = Y.t6_coding;
  }
  else
    bcs_dcs->v_cap1_snd = FALSE;

#undef X
#undef Y
}

#ifdef EXTENDED_FAX_CAP2
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dcs_cap2   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 2.

*/

LOCAL void ker_prep_dcs_cap2 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DCS     *bcs_dcs)
{
#define X bcs_dcs->cap2_snd
#define Y t30_cap_req->hdlc_info.dcs

  TRACE_FUNCTION ("ker_prep_dcs_cap2()");

  if (1)    /* for future use */
  {
    bcs_dcs->v_cap2_snd = TRUE;
  }
  else
    bcs_dcs->v_cap2_snd = FALSE;

#undef X
#undef Y
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dcs_cap3   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 3.

*/

LOCAL void ker_prep_dcs_cap3 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DCS     *bcs_dcs)
{
#define X bcs_dcs->cap3_snd
#define Y t30_cap_req->hdlc_info.dcs

  TRACE_FUNCTION ("ker_prep_dcs_cap3()");

  if (Y.R8_lines != 0xFF)
  {
    bcs_dcs->v_cap3_snd = TRUE;

    X.R8_lines          = Y.R8_lines;
    X.r_300_pels        = Y.r_300_pels;
    X.R16_lines_pels    = Y.R16_lines_pels;
    X.resolution_type   = Y.resolution_type;
  }
  else
    bcs_dcs->v_cap3_snd = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dcs_cap4   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 4.

*/

LOCAL void ker_prep_dcs_cap4 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DCS     *bcs_dcs)
{
#define X bcs_dcs->cap4_snd
#define Y t30_cap_req->hdlc_info.dcs

  TRACE_FUNCTION ("ker_prep_dcs_cap4()");

  if (Y.subaddr != 0xFF)
  {
    bcs_dcs->v_cap4_snd = TRUE;

    X.subaddr           = Y.subaddr;
    X.password          = Y.password;
    X.bft               = Y.bft;
    X.dtm               = Y.dtm;
    X.edi               = Y.edi;
  }
  else
    bcs_dcs->v_cap4_snd = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dcs_cap5   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 5.

*/

LOCAL void ker_prep_dcs_cap5 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DCS     *bcs_dcs)
{
#define X bcs_dcs->cap5_snd
#define Y t30_cap_req->hdlc_info.dcs

  TRACE_FUNCTION ("ker_prep_dcs_cap5()");

  if (Y.btm != 0xFF)
  {
    bcs_dcs->v_cap5_snd = TRUE;

    X.btm               = Y.btm;
    X.char_mode         = Y.char_mode;
    X.mixed_mode        = Y.mixed_mode;
  }
  else
    bcs_dcs->v_cap5_snd = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dcs_cap6   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 6.

*/

LOCAL void ker_prep_dcs_cap6 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DCS     *bcs_dcs)
{
#define X bcs_dcs->cap6_snd
#define Y t30_cap_req->hdlc_info.dcs

  TRACE_FUNCTION ("ker_prep_dcs_cap6()");

  if (Y.proc_mode_26 != 0xFF)
  {
    bcs_dcs->v_cap6_snd  = TRUE;

    X.proc_mode_26       = Y.proc_mode_26;
    X.dig_network_cap    = Y.dig_network_cap;
    X.duplex             = Y.duplex;
    X.jpeg               = Y.jpeg;
    X.full_colour        = Y.full_colour;
    X.huffman_tables     = Y.huffman_tables;
    X.r_12_bits_pel_comp = Y.r_12_bits_pel_comp;
  }
  else
    bcs_dcs->v_cap6_snd  = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dcs_cap7   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 7.

*/

LOCAL void ker_prep_dcs_cap7 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DCS     *bcs_dcs)
{
#define X bcs_dcs->cap7_snd
#define Y t30_cap_req->hdlc_info.dcs

  TRACE_FUNCTION ("ker_prep_dcs_cap7()");

  if (Y.no_subsamp != 0xFF)
  {
    bcs_dcs->v_cap7_snd          = TRUE;

    X.no_subsamp                 = Y.no_subsamp;
    X.cust_illum                 = Y.cust_illum;
    X.cust_gamut                 = Y.cust_gamut;
    X.na_letter                  = Y.na_letter;
    X.na_legal                   = Y.na_legal;
    X.sing_prog_seq_coding_basic = Y.sing_prog_seq_coding_basic;
    X.sing_prog_seq_coding_L0    = Y.sing_prog_seq_coding_L0;
  }
  else
    bcs_dcs->v_cap7_snd          = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dcs        |
+--------------------------------------------------------------------+

  PURPOSE : This function fills the hdlc-frame-buffer with the
            DCS-parameters to send.

*/

LOCAL void ker_prep_dcs (T_T30_CAP_REQ *t30_cap_req,
                          T_BCS_DCS     *bcs_dcs)
{
  TRACE_FUNCTION ("ker_prep_dcs()");

  ker_prep_dcs_cap0 (t30_cap_req, bcs_dcs);
  ker_prep_dcs_cap1 (t30_cap_req, bcs_dcs);

#ifdef EXTENDED_FAX_CAP2
  ker_prep_dcs_cap2 (t30_cap_req, bcs_dcs);
#endif

  ker_prep_dcs_cap3 (t30_cap_req, bcs_dcs);
  ker_prep_dcs_cap4 (t30_cap_req, bcs_dcs);
  ker_prep_dcs_cap5 (t30_cap_req, bcs_dcs);
  ker_prep_dcs_cap6 (t30_cap_req, bcs_dcs);
  ker_prep_dcs_cap7 (t30_cap_req, bcs_dcs);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dis_cap0   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the mandatory fax capabilities.

*/

LOCAL void ker_prep_dis_cap0 (T_BCS_DIS *bcs_dis)
{
#define X bcs_dis->cap0_rcv
#define Y t30_data->hdlc_snd.dis

  TRACE_FUNCTION ("ker_prep_dis_cap0()");

  X.v8             = Y.v8;
  X.n_byte         = Y.n_byte;
  X.ready_tx_fax   = Y.ready_tx_fax;
  X.rec_fax_op     = Y.rec_fax_op;
  X.data_sig_rate  = Y.data_sig_rate;
  X.R8_lines_pels  = Y.R8_lines_pels;
  X.two_dim_coding = Y.two_dim_coding;
  X.rec_width      = Y.rec_width;
  X.max_rec_len    = Y.max_rec_len;
  X.min_scan_time  = Y.min_scan_time;

  /*
  t30_data->trans_rate = Y.data_sig_rate;
  */
#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dis_cap1   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 1.

*/

LOCAL void ker_prep_dis_cap1 (T_BCS_DIS *bcs_dis)
{
#define X bcs_dis->cap1_rcv
#define Y t30_data->hdlc_snd.dis

  TRACE_FUNCTION ("ker_prep_dis_cap1()");

  if (Y.uncomp_mode != 0xFF)
  {
    bcs_dis->v_cap1_rcv = TRUE;

    X.uncomp_mode       = Y.uncomp_mode;
    X.err_corr_mode     = Y.err_corr_mode;
    X.t6_coding         = Y.t6_coding;
  }
  else
    bcs_dis->v_cap1_rcv = FALSE;

#undef X
#undef Y
}

#ifdef EXTENDED_FAX_CAP2
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dis_cap2   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 2.

*/

LOCAL void ker_prep_dis_cap2 (T_BCS_DIS *bcs_dis)
{
#define X bcs_dis->cap2_rcv
#define Y t30_data->hdlc_snd.dis

  TRACE_FUNCTION ("ker_prep_dis_cap2()");

  if (1)    /* for future use */
  {
    bcs_dis->v_cap2_rcv = TRUE;
  }
  else
    bcs_dis->v_cap2_rcv = FALSE;

#undef X
#undef Y
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dis_cap3   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 3.

*/

LOCAL void ker_prep_dis_cap3 (T_BCS_DIS *bcs_dis)
{
#define X bcs_dis->cap3_rcv
#define Y t30_data->hdlc_snd.dis

  TRACE_FUNCTION ("ker_prep_dis_cap3()");

  if (Y.R8_lines != 0xFF)
  {
    bcs_dis->v_cap3_rcv = TRUE;

    X.R8_lines          = Y.R8_lines;
    X.r_300_pels        = Y.r_300_pels;
    X.R16_lines_pels    = Y.R16_lines_pels;
    X.i_res_pref        = Y.i_res_pref;
    X.m_res_pref        = Y.m_res_pref;
    X.min_scan_time_hr  = Y.min_scan_time_hr;
    X.sel_polling       = Y.sel_polling;
  }
  else
    bcs_dis->v_cap3_rcv = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dis_cap4   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 4.

*/

LOCAL void ker_prep_dis_cap4 (T_BCS_DIS *bcs_dis)
{
#define X bcs_dis->cap4_rcv
#define Y t30_data->hdlc_snd.dis

  TRACE_FUNCTION ("ker_prep_dis_cap4()");

  if (Y.subaddr != 0xFF)
  {
    bcs_dis->v_cap4_rcv = TRUE;

    X.subaddr           = Y.subaddr;
    X.password          = Y.password;
    X.ready_tx_doc      = Y.ready_tx_doc;
    X.bft               = Y.bft;
    X.dtm               = Y.dtm;
    X.edi               = Y.edi;
  }
  else
    bcs_dis->v_cap4_rcv = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dis_cap5   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 5.

*/

LOCAL void ker_prep_dis_cap5 (T_BCS_DIS *bcs_dis)
{
#define X bcs_dis->cap5_rcv
#define Y t30_data->hdlc_snd.dis

  TRACE_FUNCTION ("ker_prep_dis_cap5()");

  if (Y.btm != 0xFF)
  {
    bcs_dis->v_cap5_rcv = TRUE;

    X.btm               = Y.btm;
    X.ready_tx_mixed    = Y.ready_tx_mixed;
    X.char_mode         = Y.char_mode;
    X.mixed_mode        = Y.mixed_mode;
  }
  else
    bcs_dis->v_cap5_rcv = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dis_cap6   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 6.

*/

LOCAL void ker_prep_dis_cap6 (T_BCS_DIS *bcs_dis)
{
#define X bcs_dis->cap6_rcv
#define Y t30_data->hdlc_snd.dis

  TRACE_FUNCTION ("ker_prep_dis_cap6()");

  if (Y.proc_mode_26 != 0xFF)
  {
    bcs_dis->v_cap6_rcv  = TRUE;

    X.proc_mode_26       = Y.proc_mode_26;
    X.dig_network_cap    = Y.dig_network_cap;
    X.duplex             = Y.duplex;
    X.jpeg               = Y.jpeg;
    X.full_colour        = Y.full_colour;
    X.r_12_bits_pel_comp = Y.r_12_bits_pel_comp;
  }
  else
    bcs_dis->v_cap6_rcv  = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dis_cap7   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 7.

*/

LOCAL void ker_prep_dis_cap7 (T_BCS_DIS *bcs_dis)
{
#define X bcs_dis->cap7_rcv
#define Y t30_data->hdlc_snd.dis

  TRACE_FUNCTION ("ker_prep_dis_cap7()");

  if (Y.no_subsamp != 0xFF)
  {
    bcs_dis->v_cap7_rcv          = TRUE;

    X.no_subsamp                 = Y.no_subsamp;
    X.cust_illum                 = Y.cust_illum;
    X.cust_gamut                 = Y.cust_gamut;
    X.na_letter                  = Y.na_letter;
    X.na_legal                   = Y.na_legal;
    X.sing_prog_seq_coding_basic = Y.sing_prog_seq_coding_basic;
    X.sing_prog_seq_coding_L0    = Y.sing_prog_seq_coding_L0;
  }
  else
    bcs_dis->v_cap7_rcv          = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dis        |
+--------------------------------------------------------------------+

  PURPOSE : This function fills the hdlc-frame-buffer with the
            DIS-parameters to send.

*/

LOCAL void ker_prep_dis (T_BCS_DIS *bcs_dis)
{
  TRACE_FUNCTION ("ker_prep_dis()");

  ker_prep_dis_cap0 (bcs_dis);
  ker_prep_dis_cap1 (bcs_dis);

#ifdef EXTENDED_FAX_CAP2
  ker_prep_dis_cap2 (bcs_dis);
#endif
  
  ker_prep_dis_cap3 (bcs_dis);
  ker_prep_dis_cap4 (bcs_dis);
  ker_prep_dis_cap5 (bcs_dis);
  ker_prep_dis_cap6 (bcs_dis);
  ker_prep_dis_cap7 (bcs_dis);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dtc_cap0   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the mandatory fax capabilities.

*/

LOCAL void ker_prep_dtc_cap0 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DTC     *bcs_dtc)
{
#define X bcs_dtc->cap0_rcv
#define Y t30_cap_req->hdlc_info.dtc

  TRACE_FUNCTION ("ker_prep_dtc_cap0()");

  X.v8             = Y.v8;
  X.n_byte         = Y.n_byte;
  X.ready_tx_fax   = Y.ready_tx_fax;
  X.rec_fax_op     = Y.rec_fax_op;
  X.data_sig_rate  = Y.data_sig_rate;
  X.R8_lines_pels  = Y.R8_lines_pels;
  X.two_dim_coding = Y.two_dim_coding;
  X.rec_width      = Y.rec_width;
  X.max_rec_len    = Y.max_rec_len;
  X.min_scan_time  = Y.min_scan_time;

  /*
  t30_data->trans_rate = Y.data_sig_rate;
  */
#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dtc_cap1   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 1.

*/

LOCAL void ker_prep_dtc_cap1 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DTC     *bcs_dtc)
{
#define X bcs_dtc->cap1_rcv
#define Y t30_cap_req->hdlc_info.dtc

  TRACE_FUNCTION ("ker_prep_dtc_cap1()");

  if (Y.uncomp_mode != 0xFF)
  {
    bcs_dtc->v_cap1_rcv = TRUE;

    X.uncomp_mode       = Y.uncomp_mode;
    X.err_corr_mode     = Y.err_corr_mode;
    X.t6_coding         = Y.t6_coding;
  }
  else
    bcs_dtc->v_cap1_rcv = FALSE;

#undef X
#undef Y
}

#ifdef EXTENDED_FAX_CAP2
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dtc_cap2   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 2.

*/

LOCAL void ker_prep_dtc_cap2 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DTC     *bcs_dtc)
{
#define X bcs_dtc->cap2_rcv
#define Y t30_cap_req->hdlc_info.dtc

  TRACE_FUNCTION ("ker_prep_dtc_cap2()");

  if (1)     /* for future use */
  {
    bcs_dtc->v_cap2_rcv = TRUE;
  }
  else
    bcs_dtc->v_cap2_rcv = FALSE;

#undef X
#undef Y
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dtc_cap3   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 3.

*/

LOCAL void ker_prep_dtc_cap3 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DTC     *bcs_dtc)
{
#define X bcs_dtc->cap3_rcv
#define Y t30_cap_req->hdlc_info.dtc

  TRACE_FUNCTION ("ker_prep_dtc_cap3()");

  if (Y.R8_lines != 0xFF)
  {
    bcs_dtc->v_cap3_rcv = TRUE;

    X.R8_lines          = Y.R8_lines;
    X.r_300_pels        = Y.r_300_pels;
    X.R16_lines_pels    = Y.R16_lines_pels;
    X.i_res_pref        = Y.i_res_pref;
    X.m_res_pref        = Y.m_res_pref;
    X.min_scan_time_hr  = Y.min_scan_time_hr;
    X.sel_polling       = Y.sel_polling;
  }
  else
    bcs_dtc->v_cap3_rcv = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dtc_cap4   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 4.

*/

LOCAL void ker_prep_dtc_cap4 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DTC     *bcs_dtc)
{
#define X bcs_dtc->cap4_rcv
#define Y t30_cap_req->hdlc_info.dtc

  TRACE_FUNCTION ("ker_prep_dtc_cap4()");

  if (Y.subaddr != 0xFF)
  {
    bcs_dtc->v_cap4_rcv = TRUE;

    X.subaddr           = Y.subaddr;
    X.password          = Y.password;
    X.ready_tx_doc      = Y.ready_tx_doc;
    X.bft               = Y.bft;
    X.dtm               = Y.dtm;
    X.edi               = Y.edi;
  }
  else
    bcs_dtc->v_cap4_rcv = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dtc_cap5   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 5.

*/

LOCAL void ker_prep_dtc_cap5 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DTC     *bcs_dtc)
{
#define X bcs_dtc->cap5_rcv
#define Y t30_cap_req->hdlc_info.dtc

  TRACE_FUNCTION ("ker_prep_dtc_cap5()");

  if (Y.btm != 0xFF)
  {
    bcs_dtc->v_cap5_rcv = TRUE;

    X.btm               = Y.btm;
    X.ready_tx_mixed    = Y.ready_tx_mixed;
    X.char_mode         = Y.char_mode;
    X.mixed_mode        = Y.mixed_mode;
  }
  else
    bcs_dtc->v_cap5_rcv = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dtc_cap6   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 6.

*/

LOCAL void ker_prep_dtc_cap6 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DTC     *bcs_dtc)
{
#define X bcs_dtc->cap6_rcv
#define Y t30_cap_req->hdlc_info.dtc

  TRACE_FUNCTION ("ker_prep_dtc_cap6()");

  if (Y.proc_mode_26 != 0xFF)
  {
    bcs_dtc->v_cap6_rcv  = TRUE;

    X.proc_mode_26       = Y.proc_mode_26;
    X.dig_network_cap    = Y.dig_network_cap;
    X.duplex             = Y.duplex;
    X.jpeg               = Y.jpeg;
    X.full_colour        = Y.full_colour;
    X.r_12_bits_pel_comp = Y.r_12_bits_pel_comp;
  }
  else
    bcs_dtc->v_cap6_rcv  = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dtc_cap7   |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the extended fax capabilities 7.

*/

LOCAL void ker_prep_dtc_cap7 (T_T30_CAP_REQ *t30_cap_req,
                               T_BCS_DTC     *bcs_dtc)
{
#define X bcs_dtc->cap7_rcv
#define Y t30_cap_req->hdlc_info.dtc

  TRACE_FUNCTION ("ker_prep_dtc_cap7()");

  if (Y.no_subsamp != 0xFF)
  {
    bcs_dtc->v_cap7_rcv          = TRUE;

    X.no_subsamp                 = Y.no_subsamp;
    X.cust_illum                 = Y.cust_illum;
    X.cust_gamut                 = Y.cust_gamut;
    X.na_letter                  = Y.na_letter;
    X.na_legal                   = Y.na_legal;
    X.sing_prog_seq_coding_basic = Y.sing_prog_seq_coding_basic;
    X.sing_prog_seq_coding_L0    = Y.sing_prog_seq_coding_L0;
  }
  else
    bcs_dtc->v_cap7_rcv          = FALSE;

#undef X
#undef Y
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_prep_dtc        |
+--------------------------------------------------------------------+

  PURPOSE : This function fills the hdlc-frame-buffer with the
            DTC-parameters to send.

*/

LOCAL void ker_prep_dtc (T_T30_CAP_REQ *t30_cap_req,
                          T_BCS_DTC     *bcs_dtc)
{
  TRACE_FUNCTION ("ker_prep_dtc()");

  ker_prep_dtc_cap0 (t30_cap_req, bcs_dtc);
  ker_prep_dtc_cap1 (t30_cap_req, bcs_dtc);

#ifdef EXTENDED_FAX_CAP2
  ker_prep_dtc_cap2 (t30_cap_req, bcs_dtc);
#endif

  ker_prep_dtc_cap3 (t30_cap_req, bcs_dtc);
  ker_prep_dtc_cap4 (t30_cap_req, bcs_dtc);
  ker_prep_dtc_cap5 (t30_cap_req, bcs_dtc);
  ker_prep_dtc_cap6 (t30_cap_req, bcs_dtc);
  ker_prep_dtc_cap7 (t30_cap_req, bcs_dtc);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_req_csi         |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the optional command CSI.

*/

LOCAL void ker_req_csi (void)
{
  MCAST (csi, BCS_CSI);

  TRACE_FUNCTION ("ker_req_csi()");

  if (t30_data->hdlc_snd.c_csi)
  {
    csi->fcf = BCS_CSI;

    csi->c_cld_sub_nr = t30_data->hdlc_snd.c_csi;

    memcpy (csi->cld_sub_nr, t30_data->hdlc_snd.csi,
            sizeof (t30_data->hdlc_snd.csi));

    sig_ker_bcs_bdat_req (FINAL_NO);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_req_nsf         |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the optional command NSF.

*/

LOCAL void ker_req_nsf (void)
{
  MCAST (nsf, BCS_NSF);

  TRACE_FUNCTION ("ker_req_nsf()");

  if (t30_data->hdlc_snd.c_nsf)
  {
    nsf->fcf = BCS_NSF;

    nsf->non_std_fac.l_non_std_fac = t30_data->hdlc_snd.c_nsf << 3;
    nsf->non_std_fac.o_non_std_fac = 0;

    memcpy (nsf->non_std_fac.b_non_std_fac, t30_data->hdlc_snd.nsf,
            sizeof (t30_data->hdlc_snd.nsf));

    sig_ker_bcs_bdat_req (FINAL_NO);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_res_cig         |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the optional command CIG.

*/

LOCAL void ker_res_cig (T_T30_CAP_REQ *t30_cap_req)
{
  MCAST (cig, BCS_CIG);

  TRACE_FUNCTION ("ker_res_cig()");

  if (t30_cap_req->hdlc_info.c_cig)
  {
    cig->fcf = BCS_CIG;

    cig->c_clg_sub_nr = t30_cap_req->hdlc_info.c_cig;

    memcpy (cig->clg_sub_nr, t30_cap_req->hdlc_info.cig,
            sizeof (t30_cap_req->hdlc_info.cig));

    sig_ker_bcs_bdat_req (FINAL_NO);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_res_crp         |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the optional command CRP.

*/

LOCAL void ker_res_crp (T_T30_CAP_REQ *t30_cap_req)
{
  MCAST (crp, BCS_CRP);

  TRACE_FUNCTION ("ker_res_crp()");

  if (t30_cap_req->hdlc_info.crp)
  {
    crp->fcf = BCS_CRP;

    sig_ker_bcs_bdat_req (FINAL_NO);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_res_nsc         |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the optional command NSC.

*/

LOCAL void ker_res_nsc (T_T30_CAP_REQ *t30_cap_req)
{
  MCAST (nsc, BCS_NSC);

  TRACE_FUNCTION ("ker_res_nsc()");

  if (t30_cap_req->hdlc_info.c_nsc)
  {
    nsc->fcf = BCS_NSC;

    nsc->non_std_fac.l_non_std_fac = t30_cap_req->hdlc_info.c_nsc << 3;
    nsc->non_std_fac.o_non_std_fac = 0;

    memcpy (nsc->non_std_fac.b_non_std_fac, t30_cap_req->hdlc_info.nsc,
            sizeof (t30_cap_req->hdlc_info.nsc));

    sig_ker_bcs_bdat_req (FINAL_NO);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_res_nss         |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the optional command NSS.

*/

LOCAL void ker_res_nss (T_T30_CAP_REQ *t30_cap_req)
{
  MCAST (nss, BCS_NSS);

  TRACE_FUNCTION ("ker_res_nss()");

  if (t30_cap_req->hdlc_info.c_nss)
  {
    nss->fcf = BCS_NSS;

    nss->non_std_fac.l_non_std_fac = t30_cap_req->hdlc_info.c_nss << 3;
    nss->non_std_fac.o_non_std_fac = 0;

    memcpy (nss->non_std_fac.b_non_std_fac, t30_cap_req->hdlc_info.nss,
            sizeof (t30_cap_req->hdlc_info.nss));

    sig_ker_bcs_bdat_req (FINAL_NO);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_res_pwd_poll    |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the optional command PWD_POLL.

*/

LOCAL void ker_res_pwd_poll (T_T30_CAP_REQ *t30_cap_req)
{
  MCAST (pwd, BCS_PWD_POLL);

  TRACE_FUNCTION ("ker_res_pwd_poll()");

  if (t30_cap_req->hdlc_info.c_pwd)
  {
    pwd->fcf = BCS_PWD_POLL;

    pwd->c_pm_pword = t30_cap_req->hdlc_info.c_pwd;

    memcpy (pwd->pm_pword, t30_cap_req->hdlc_info.pwd,
            sizeof (t30_cap_req->hdlc_info.pwd));

    sig_ker_bcs_bdat_req (FINAL_NO);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_res_pwd_snd     |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the optional command PWD_SND.

*/

LOCAL void ker_res_pwd_snd (T_T30_CAP_REQ *t30_cap_req)
{
  MCAST (pwd, BCS_PWD_SND);

  TRACE_FUNCTION ("ker_res_pwd_snd()");

  if (t30_cap_req->hdlc_info.c_pwd)
  {
    pwd->fcf = BCS_PWD_SND;

    pwd->c_sm_pword = t30_cap_req->hdlc_info.c_pwd;

    memcpy (pwd->sm_pword, t30_cap_req->hdlc_info.pwd,
            sizeof (t30_cap_req->hdlc_info.pwd));

    sig_ker_bcs_bdat_req (FINAL_NO);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_res_sep         |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the optional command SEP.

*/

LOCAL void ker_res_sep (T_T30_CAP_REQ *t30_cap_req)
{
  MCAST (sep, BCS_SEP);

  TRACE_FUNCTION ("ker_res_sep()");

  if (t30_cap_req->hdlc_info.c_sep)
  {
    sep->fcf = BCS_SEP;

    sep->c_pm_sub_addr = t30_cap_req->hdlc_info.c_sep;

    memcpy (sep->pm_sub_addr, t30_cap_req->hdlc_info.sep,
            sizeof (t30_cap_req->hdlc_info.sep));

    sig_ker_bcs_bdat_req (FINAL_NO);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_res_sub         |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the optional command SUB.

*/

LOCAL void ker_res_sub (T_T30_CAP_REQ *t30_cap_req)
{
  MCAST (sub, BCS_SUB);

  TRACE_FUNCTION ("ker_res_sub()");

  if (t30_cap_req->hdlc_info.c_sub)
  {
    sub->fcf = BCS_SUB;

    sub->c_sub_addr = t30_cap_req->hdlc_info.c_sub;

    memcpy (sub->sub_addr, t30_cap_req->hdlc_info.sub,
            sizeof (t30_cap_req->hdlc_info.sub));

    sig_ker_bcs_bdat_req (FINAL_NO);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_res_tsi         |
+--------------------------------------------------------------------+

  PURPOSE : This function prepares the optional command TSI.

*/

LOCAL void ker_res_tsi (T_T30_CAP_REQ *t30_cap_req)
{
  MCAST (tsi, BCS_TSI);

  TRACE_FUNCTION ("ker_res_tsi()");

  if (t30_cap_req->hdlc_info.c_tsi)
  {
    tsi->fcf = BCS_TSI;

    tsi->c_tra_sub_nr = t30_cap_req->hdlc_info.c_tsi;

    memcpy (tsi->tra_sub_nr, t30_cap_req->hdlc_info.tsi,
            sizeof (t30_cap_req->hdlc_info.tsi));

    sig_ker_bcs_bdat_req (FINAL_NO);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_send_dcs        |
+--------------------------------------------------------------------+

  PURPOSE : This function sends command DCS to FAD.

*/

GLOBAL void ker_send_dcs (T_T30_CAP_REQ *t30_cap_req)
{
  TRACE_FUNCTION ("ker_send_dcs()");

  ker_res_nss     (t30_cap_req);
  ker_res_pwd_snd (t30_cap_req);
  ker_res_sub     (t30_cap_req);
  ker_res_crp     (t30_cap_req);
  ker_res_tsi     (t30_cap_req);

  {
    MCAST (bcs_dcs, BCS_DCS);
    bcs_dcs->fcf = BCS_DCS;
    ker_prep_dcs (t30_cap_req, bcs_dcs);
    sig_ker_bcs_bdat_req (FINAL_YES);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_send_dis        |
+--------------------------------------------------------------------+

  PURPOSE : This function sends command DIS to FAD.

*/

GLOBAL void ker_send_dis (void)
{
  MCAST (dis, BCS_DIS);

  TRACE_FUNCTION ("ker_send_dis()");

  ker_req_nsf ();
  ker_req_csi ();

  dis->fcf = BCS_DIS;

  ker_prep_dis (dis);

  sig_ker_bcs_bdat_req (FINAL_YES);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_send_dtc        |
+--------------------------------------------------------------------+

  PURPOSE : This function sends command DTC to FAD.

*/

GLOBAL void ker_send_dtc (T_T30_CAP_REQ *t30_cap_req)
{
  MCAST (dtc, BCS_DTC);

  TRACE_FUNCTION ("ker_send_dtc()");

  ker_res_nsc      (t30_cap_req);
  ker_res_pwd_poll (t30_cap_req);
  ker_res_sep      (t30_cap_req);
  ker_res_cig      (t30_cap_req);

  dtc->fcf = BCS_DTC;

  ker_prep_dtc(t30_cap_req, dtc);

  sig_ker_bcs_bdat_req (FINAL_YES);
}

GLOBAL void snd_t30_sgn_ind(UBYTE sgn)
{
  PALLOC (t30_sgn_ind, T30_SGN_IND);
  t30_sgn_ind->sgn = sgn;
  PSENDX (MMI, t30_sgn_ind);
}

GLOBAL void snd_complete_ind(UBYTE cmpl)
{
  PALLOC (t30_cmpl_ind, T30_CMPL_IND);
  t30_cmpl_ind->cmpl = cmpl;
  PSENDX (MMI, t30_cmpl_ind);
  SET_STATE (KER, T30_IDLE);
}

GLOBAL void snd_error_ind(USHORT cause)
{
  PALLOC (t30_error_ind, T30_ERROR_IND);
  t30_error_ind->cause = cause;
  PSENDX (MMI, t30_error_ind);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_send_err        |
+--------------------------------------------------------------------+

  PURPOSE : This function sends an error cause to MMI.

*/

GLOBAL void ker_send_err (USHORT cause)
{
  TRACE_FUNCTION ("ker_send_err()");

  TIMERSTOP (T1_INDEX);
  TIMERSTOP (T2_INDEX);
  TIMERSTOP (T4_INDEX);

  memset (&t30_data->hdlc_rcv, 0, sizeof (T_hdlc_info));

  SET_STATE (KER, T30_IDLE);

  snd_error_ind(cause);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : TDCexpand           |
+--------------------------------------------------------------------+

  PURPOSE :
*/

LOCAL void TDCexpand(T_sdu *sdu, T_desc_list2 *desc_list, BOOL final)
{
  T_desc2 *desc;
  USHORT dataSize, di = 0, si, len;
  UBYTE *p;
  ULONG *pLast = &desc_list->first;

  if (t30_data->dti_data_ind)
  {
    while (*pLast)
    {
      desc = (T_desc2*)pLast;
      pLast = (ULONG*)desc->next;
    }
  }
  len = sdu->l_buf >> 3;
  dataSize = len;

  if (t30_data->TDC_doli_pending EQ TRUE)
    dataSize++;

  p = &sdu->buf[sdu->o_buf >> 3];
  for (si = 0; si < len-1; si++)
    if (p[si] EQ TDC_DLE)
      dataSize++;

  if (final)
    dataSize += 2;

  dataSize = (dataSize/t30_data->frames_per_prim + 1) * t30_data->frames_per_prim;
  MALLOC(desc, (USHORT)(sizeof(T_desc2) + dataSize));
  desc->next = 0;
  *pLast = (ULONG)desc;
  p = &sdu->buf[sdu->o_buf >> 3];
  if (t30_data->TDC_doli_pending EQ TRUE)
  {
    desc->buffer[di++] = TDC_DLE;
    if (p[0] EQ TDC_DLE)
      desc->buffer[di++] = TDC_SUB;/*lint !e661 (Warning -- Possible access of out-of-bounds pointer)*/
    else
    {
      desc->buffer[di++] = TDC_DLE;/*lint !e661 (Warning -- Possible access of out-of-bounds pointer)*/
      desc->buffer[di++] = p[0];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer)*/
    }
    t30_data->TDC_doli_pending = FALSE;
    si = 1;
  }
  else
    si = 0;

  while (si < len-1)
  {
    desc->buffer[di++] = p[si];
    if (p[si] EQ TDC_DLE)
    {
      if (p[si+1] EQ TDC_DLE)
      {
        desc->buffer[di++] = TDC_SUB;/*lint !e661 (Warning -- Possible access of out-of-bounds pointer)*/
        si++;
      }
      else
        desc->buffer[di++] = TDC_DLE;/*lint !e661 (Warning -- Possible access of out-of-bounds pointer)*/
    }
    si++;
  }

  if (si EQ len-1 AND p[si] EQ TDC_DLE)
    t30_data->TDC_doli_pending = TRUE;
  else
    desc->buffer[di++] = p[si];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer)*/
  if (final)
  {
    desc->buffer[di++] = TDC_DLE;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer)*/
    desc->buffer[di++] = TDC_ETX;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer)*/
  }
  desc_list->list_len += desc->len = di;
  desc->size   = desc->len;
  desc->offset = 0;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : TDCcompress        |
+--------------------------------------------------------------------+

  PURPOSE : Move contents of T_desc_list2 into T_sdu w/o TDCs
*/

GLOBAL UBYTE TDCcompress(T_desc2 *desc, T_sdu *sdu)
{
  T_desc2 *desc_next;
  UBYTE *d, *s, *s_end, ready = 0;

  s_end = sdu->buf + t30_data->frames_per_prim;
  s = sdu->buf + (sdu->l_buf >> 3);

  if (t30_data->dti_data_req_ptr EQ NULL)
    d = desc->buffer;
  else
    d = t30_data->dti_data_req_ptr;

  while (s < s_end AND desc AND !ready)
  {
    if (t30_data->TDC_upli_pending)
    {
      switch (*d)
      {
        case TDC_EOM: /* end of document (message) */
          TRACE_EVENT ("DTE->DCE:EOM");
          t30_data->sgn_ind = SGN_EOM; /* cmhT30_PPMRcvd(SGN_EOM) */
          ready = READY_UPLI_PAGE;
          break;

        case TDC_EOP: /* end of document and session (procedure) */
          TRACE_EVENT ("DTE->DCE:EOP");
          t30_data->sgn_ind = SGN_EOP; /* cmhT30_PPMRcvd(SGN_EOP) */
          ready = READY_UPLI_PAGE;
          break;

        case TDC_ETX:
          /*
           * end of page or as acknowledge to a sent DLE,CAN (cancel Phase C)
           */
          TRACE_EVENT ("DTE->DCE:ETX");
          t30_data->sgn_ind = SGN_EOP;
          ready = READY_UPLI_PAGE;
          break;

        case TDC_MPS: /* end of page, more pages to follow */
          TRACE_EVENT ("DTE->DCE:MPS");
          t30_data->sgn_ind = SGN_MPS; /* cmhT30_PPMRcvd(SGN_MPS) */
          ready = READY_UPLI_PAGE;
          break;

        case TDC_SOH: /* SOH (start of header) in packet mode */
          TRACE_EVENT ("DTE->DCE:SOH");
          break;

        case TDC_ETB: /* ETB (end of text block) in packet mode */
          TRACE_EVENT ("DTE->DCE:ETB");
          break;

        case TDC_OVR: /* overrun error marker */
          TRACE_EVENT ("DTE->DCE:OVERRUN in DTE");
          /*
           * add call to cmhT30 here overrun handling ??
           */
          break;

        case TDC_BCR: /* buffer credit request */
          TRACE_EVENT ("DTE->DCE:REQ CREDIT");
          switch (GET_STATE (KER))
          {
            case T30_SND_MSG:
              break;
            case T30_RCV_MSG:
              break;
          }
          break;

        case TDC_PRI: /* procedural interrupt signal */
          TRACE_EVENT ("DTE->DCE:PRI");
          t30_data->sgn_ind = SGN_NOT_USED; /* cmhT30_PRIRcvd() */
          break;

        case TDC_DC2: /* receive buffer status: buffer is empty */
          TRACE_EVENT ("DTE->DCE:DC2");
          t30_data->sgn_ind = SGN_NOT_USED; /* cmhT30_DTERdy2Rcv() */
          break;

        case TDC_CAN: /* Requests DCE/DTE to stop delivering Phase C data */
          TRACE_EVENT ("DTE->DCE:CANCEL");
          /*
           * add call to cmhT30 here
           */
          break;

        default:
          if ('a' <= *d AND *d <= 'n') /* interpret as fax cap setting */
          {
            TRACE_EVENT ("DTE->DCE:FAX CAPS");
            /*
             * add call to cmhT30 here for setup the fax caps
             */
          }
          else
          {
            *s = TDC_DLE;
            s++;
            sdu->l_buf += 8;
            if (*d EQ TDC_SUB) /* double the TDC_DLE */
            {
              *s = *d;
              s++;
              sdu->l_buf += 8;
            }
            t30_data->TDC_upli_pending = FALSE;

            d++;
            if (d EQ desc->buffer + desc->len)
            {
              desc_next = (T_desc2 *)desc->next;
              MFREE(desc);
              desc = desc_next;
              d = desc->buffer;
            }
          }
        }
      t30_data->TDC_upli_pending = FALSE;
    }
    else
    {
      if (*d EQ TDC_DLE)
      {
        t30_data->TDC_upli_pending = TRUE;
      }
      else
      {
        *s = *d;
        s++;
        sdu->l_buf += 8;
      }
      d++;
      if (d EQ desc->buffer + desc->len)
      {
        desc_next = (T_desc2 *)desc->next;
        MFREE(desc);
        desc = desc_next;
        d = desc->buffer;
      }
    }
    if (desc AND !ready)
      t30_data->dti_data_req_ptr = d;
    else
      t30_data->dti_data_req_ptr = NULL;
  }
  if (s EQ s_end)
    ready |= READY_UPLI_SDU;

  return ready;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : check_ignore        |
+--------------------------------------------------------------------+

  PURPOSE :
*/

LOCAL BOOL check_ignore(T_FAD_DATA_IND *fad_data_ind)
{
  if (!fad_data_ind)
    return TRUE;

  if (ker_check_rtc(fad_data_ind) EQ RTC_YES AND !fad_data_ind->final)
  {
    PALLOC (fad_ignore_req, FAD_IGNORE_REQ);
    PSENDX (FAD, fad_ignore_req);
    SET_STATE(KER, T30_RCV_RDYF);
    return TRUE;
  }
  return FALSE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : append_dti_data_ind |
+--------------------------------------------------------------------+

  PURPOSE :
*/

LOCAL BOOL append_dti_data_ind(T_FAD_DATA_IND *fad_data_ind)
{
  TRACE_FUNCTION ("append_dti_data_ind()");

  TDCexpand(&fad_data_ind->sdu, &t30_data->dti_data_ind->desc_list2, fad_data_ind->final);
  return check_ignore(fad_data_ind);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : create_dti_data_ind |
+--------------------------------------------------------------------+

  PURPOSE :
*/

LOCAL BOOL create_dti_data_ind(T_FAD_DATA_IND *fad_data_ind)
{
  PALLOC (dti_data_ind, DTI2_DATA_IND);

  TRACE_FUNCTION ("create_dti_data_ind()");

  dti_data_ind->link_id         = t30_data->link_id;
  dti_data_ind->parameters.p_id = DTI_PID_UOS;
  dti_data_ind->parameters.st_lines.st_flow      = DTI_FLOW_ON;
  dti_data_ind->parameters.st_lines.st_line_sa   = DTI_SA_ON;
  dti_data_ind->parameters.st_lines.st_line_sb   = DTI_SB_ON;
  dti_data_ind->parameters.st_lines.st_break_len = DTI_BREAK_OFF;

  dti_data_ind->desc_list2.list_len = 0;
  dti_data_ind->desc_list2.first = (ULONG)NULL;

  if (fad_data_ind)
  {
    t30_data->dti_data_ind = dti_data_ind;
    TDCexpand(&fad_data_ind->sdu, &dti_data_ind->desc_list2, fad_data_ind->final);
    return check_ignore(fad_data_ind);
  }
  else
  {
    t30_data->dti_data_ind_empty = dti_data_ind;
    return FALSE;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : prepare_dti_data_ind|
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void prepare_dti_data_ind(T_FAD_DATA_IND *fad_data_ind)
{
  BOOL ignore;

  TRACE_FUNCTION ("prepare_dti_data_ind()");

  t30_data->dti_data_ind_final = fad_data_ind->final;

  if (!t30_data->dti_data_ind)
    ignore = create_dti_data_ind(fad_data_ind);
  else
    ignore = append_dti_data_ind(fad_data_ind);

  if (!ignore AND !t30_data->dti_data_ind_final)
  {
    PALLOC (fad_ready_req, FAD_READY_REQ);
    PSENDX (FAD, fad_ready_req);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : snd_dti_data_ind    |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void snd_dti_data_ind(T_DTI2_DATA_IND **dti_data_ind)
{
  TRACE_FUNCTION ("snd_dti_data_ind()");

  dti_send_data(
    t30_hDTI,
    T30_DTI_UP_DEF_INSTANCE,
    T30_DTI_UP_INTERFACE,
    T30_DTI_UP_CHANNEL,
    *dti_data_ind
    );

  *dti_data_ind = NULL;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : snd_t30_phase_ind   |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void snd_t30_phase_ind(UBYTE phase)
{
  TRACE_FUNCTION ("snd_t30_phase_ind()");

  {
    PALLOC (t30_phase_ind, T30_PHASE_IND);
    t30_phase_ind->phase = phase;
    PSENDX (MMI, t30_phase_ind);
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : dti_connect_setup   |
+--------------------------------------------------------------------+

  PURPOSE : call dti_open() to establish a dtilib connection
*/

GLOBAL void dti_connect_setup(T_T30_DTI_REQ *t30_dti_req)
{
  TRACE_FUNCTION ("dti_connect_setup()");

  t30_data->link_id = t30_dti_req->link_id;

  if (t30_data->dti_state NEQ DTI_CLOSED)
  {
    dti_disconnect();
  }

  create_dti_data_ind(NULL); /* send empty primitive to set flow control */

  /*
   * open new dti library communication channel
   */
  t30_data->dti_state = DTI_SETUP;

  if (dti_open(
               t30_hDTI,
               T30_DTI_UP_DEF_INSTANCE,
               T30_DTI_UP_INTERFACE,
               T30_DTI_UP_CHANNEL,
               T30_DTI_UPLINK_QUEUE_SIZE,
               t30_dti_req->dti_direction,
               DTI_QUEUE_UNUSED,
               DTI_VERSION_10,
               (U8*)t30_dti_req->entity_name,
               t30_dti_req->link_id
               ))
    return;

  t30_data->dti_state = DTI_CLOSED;
  snd_t30_dti_cnf(T30_DISCONNECT_DTI);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : dti_connect_open    |
+--------------------------------------------------------------------+

  PURPOSE : acknowledge established a dtilib connection
*/

GLOBAL void dti_connect_open()
{
  TRACE_FUNCTION ("dti_connect_open()");

  if(t30_data->dti_state EQ DTI_SETUP)
  {
    t30_data->dti_state = DTI_IDLE;
    snd_t30_dti_cnf(T30_CONNECT_DTI);
    snd_dti_ready_ind();
  }
  else
  {
    TRACE_ERROR ("dti connection is not in SETUP state!");
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : snd_t30_dti_ind     |
+--------------------------------------------------------------------+

  PURPOSE :
*/

LOCAL void snd_t30_dti_ind(void)
{
  TRACE_FUNCTION ("snd_t30_dti_ind()");
  {
  PALLOC (t30_dti_ind, T30_DTI_IND);
  PSENDX (MMI, t30_dti_ind);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : dti_connect_close   |
+--------------------------------------------------------------------+

  PURPOSE : react to closing of dtilib connection
*/

GLOBAL void dti_connect_close()
{
  TRACE_FUNCTION ("ti_connect_close()");

  if(t30_data->dti_state NEQ DTI_CLOSED)
  {
    t30_data->dti_state = DTI_CLOSED;
    snd_t30_dti_ind();
  }
  else
  {
    TRACE_ERROR ("dti connection is not in SETUP/OPEN state!");
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : dti_connect_close   |
+--------------------------------------------------------------------+

  PURPOSE : close down a dtilib connection
*/

GLOBAL void dti_disconnect()
{
  TRACE_FUNCTION ("dti_disconnect()");

  /*
   * shut down dtilib communication
   */
  if(t30_data->dti_state NEQ DTI_CLOSED)
  {
    dti_close(
      t30_hDTI,
      T30_DTI_UP_DEF_INSTANCE,
      T30_DTI_UP_INTERFACE,
      T30_DTI_UP_CHANNEL,
      FALSE
      );
    t30_data->dti_state = DTI_CLOSED;
  }
  else
  {
    TRACE_ERROR ("dti connection is not in SETUP/OPEN state!");
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : snd_t30_dti_cnf     |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void snd_t30_dti_cnf(UBYTE dti_conn)
{
  TRACE_FUNCTION ("snd_t30_dti_cnf()");

  {
    PALLOC (t30_dti_cnf, T30_DTI_CNF);
    t30_dti_cnf->dti_conn = dti_conn;
    PSENDX (MMI, t30_dti_cnf);
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : snd_dti_ready_ind   |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void snd_dti_ready_ind(void)
{
  TRACE_FUNCTION ("snd_dti_ready_ind()");

  dti_start(
    t30_hDTI,
    T30_DTI_UP_DEF_INSTANCE,
    T30_DTI_UP_INTERFACE,
    T30_DTI_UP_CHANNEL
    );
}

#if defined _SIMULATION_ || defined KER_DEBUG_BCS || defined KER_DEBUG_MSG
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERF            |
| STATE   : code                       ROUTINE : ker_debug           |
+--------------------------------------------------------------------+

  PURPOSE : This function debugs the contents of a buffer.
*/

GLOBAL void ker_debug(CHAR *header, UBYTE *buf, USHORT len)
{
#define BYTE2HEXSTR(B, H) {UBYTE b, *a = (UBYTE*)(H);\
        b = (((UBYTE)(B)) >> 4) & 0x0F;\
        if (b > 9) b += 'A'-10; else b += '0'; *a = b;\
        b = ((UBYTE)(B)) & 0x0F;\
        if (b > 9) b += 'A'-10; else b += '0'; *(a+1) = b;}

  USHORT i, k = 0;
  char trace_buf[81];
  
  /*
  TRACE_EVENT("====== KERNEL DEBUG START ======");
  */
  TRACE_EVENT_P2("%s: len=%d", header, len);

  for (i = 0; i < len; i++)
  {
    BYTE2HEXSTR(buf[i], &trace_buf[k]);
    k += 2;
    if (k EQ 80)
      break;
  }
  trace_buf[k] = 0;
  TRACE_EVENT(trace_buf);
  /*
  TRACE_EVENT("====== KERNEL DEBUG END ======");
  */
}
#endif

