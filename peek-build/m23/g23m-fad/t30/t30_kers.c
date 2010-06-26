/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  t30_kers
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
|  Purpose :  This Modul defines the functions for processing
|             of incomming signals for the component T30 of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#ifndef T30_KERS_C
#define T30_KERS_C
#endif

#define ENTITY_T30

#define MEMCOPLEN(d,s,c) memcpy(t30_data->hdlc_rcv.d, s, MINIMUM(t30_data->hdlc_rcv.c, NSF_LEN))

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

LOCAL void snd_fad_rcv_tcf_req(void)
{
  PALLOC (fad_rcv_tcf_req, FAD_RCV_TCF_REQ);
  fad_rcv_tcf_req->trans_rate = t30_data->trans_rate;
  PSENDX (FAD, fad_rcv_tcf_req);
}

LOCAL void snd_t30_cap_ind(void)
{
  PALLOC (t30_cap_ind, T30_CAP_IND);
  memcpy (&t30_cap_ind->hdlc_info, &t30_data->hdlc_rcv, sizeof(T_hdlc_info));
  memset (&t30_data->hdlc_rcv, 0, sizeof(T_hdlc_info));
  PSENDX (MMI, t30_cap_ind);
}

GLOBAL void act_on_sgn_req(void)
{
  switch (t30_data->sgn_req)
  {
  case SGN_EOM:
    SET_STATE (KER, T30_SND_SGN);
    _decodedMsg[0] = BCS_EOM;
    sig_ker_bcs_bdat_req (FINAL_YES);
    break;

  case SGN_EOP:
    SET_STATE (KER, T30_SND_SGN);
    _decodedMsg[0] = BCS_EOP;
    sig_ker_bcs_bdat_req (FINAL_YES);
    break;

  case SGN_MPS:
    SET_STATE (KER, T30_SND_SGN);
    _decodedMsg[0] = BCS_MPS;
    sig_ker_bcs_bdat_req (FINAL_YES);
    break;

  case SGN_PRI_EOM:
    SET_STATE (KER, T30_SND_SGN);
    _decodedMsg[0] = BCS_PRI_EOM;
    sig_ker_bcs_bdat_req (FINAL_YES);
    break;

  case SGN_PRI_EOP:
    SET_STATE (KER, T30_SND_SGN);
    _decodedMsg[0] = BCS_PRI_EOP;
    sig_ker_bcs_bdat_req (FINAL_YES);
    break;

  case SGN_PRI_MPS:
    SET_STATE (KER, T30_SND_SGN);
    _decodedMsg[0] = BCS_PRI_MPS;
    sig_ker_bcs_bdat_req (FINAL_YES);
    break;

  default:
    break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERS            |
| STATE   : code                       ROUTINE : sig_bcs_ker_bdat_ind|
+--------------------------------------------------------------------+
  PURPOSE : Process signal BDAT_IND received from process BCS.
            This signal contains a received HDLC frame from FAD,
            decoded by CCD.
*/

GLOBAL void sig_bcs_ker_bdat_ind (void)
{
  TRACE_FUNCTION ("sig_bcs_ker_bdat_ind()");

  switch (GET_STATE (KER))
  {
    case T30_NULL:
#ifdef _SIMULATION_ /* test BCS formatter only */
      if (t30_data->test_mode & TST_BCS)
      {
        SHORT size = 0;
        switch (_decodedMsg[0])
        {
          case BCS_DIS:      size = sizeof(T_BCS_DIS)      ; break;
          case BCS_CSI:      size = sizeof(T_BCS_CSI)      ; break;
          case BCS_NSF:      size = sizeof(T_BCS_NSF)      ; break;
          case BCS_DTC:      size = sizeof(T_BCS_DTC)      ; break;
          case BCS_CIG:      size = sizeof(T_BCS_CIG)      ; break;
          case BCS_NSC:      size = sizeof(T_BCS_NSC)      ; break;
          case BCS_PWD_POLL: size = sizeof(T_BCS_PWD_POLL) ; break;
          case BCS_SEP:      size = sizeof(T_BCS_SEP)      ; break;
          case BCS_DCS:      size = sizeof(T_BCS_DCS)      ; break;
          case BCS_TSI:      size = sizeof(T_BCS_TSI)      ; break;
          case BCS_NSS:      size = sizeof(T_BCS_NSS)      ; break;
          case BCS_SUB:      size = sizeof(T_BCS_SUB)      ; break;
          case BCS_PWD_SND:  size = sizeof(T_BCS_PWD_SND)  ; break;
          
          case BCS_CFR:
          case BCS_FTT:
          case BCS_EOM:
          case BCS_MPS:
          case BCS_EOP:
          case BCS_PRI_EOM:
          case BCS_PRI_MPS:
          case BCS_PRI_EOP:
          case BCS_MCF:
          case BCS_RTP:
          case BCS_RTN:
          case BCS_PIP:
          case BCS_PIN:
          case BCS_DCN:
          case BCS_CRP:
            size = 1;
            break;
        }
        {
        PALLOC_SDU (dti_data_test_ind, DTI2_DATA_TEST_IND, REPORT_SIZE_BITS);
        dti_data_test_ind->link_id         = t30_data->link_id;
        dti_data_test_ind->parameters.p_id = DTI_PID_UOS;
        dti_data_test_ind->parameters.st_lines.st_flow      = DTI_FLOW_OFF;
        dti_data_test_ind->parameters.st_lines.st_line_sa   = DTI_SA_ON;
        dti_data_test_ind->parameters.st_lines.st_line_sb   = DTI_SB_ON;
        dti_data_test_ind->parameters.st_lines.st_break_len = DTI_BREAK_OFF;
        dti_data_test_ind->sdu.l_buf  = size << 3;
        dti_data_test_ind->sdu.o_buf  = 0;
        memcpy (dti_data_test_ind->sdu.buf, _decodedMsg, size);
        PSENDX (MMI, dti_data_test_ind);
        }
      }
#endif
      break;

    case T30_IDLE:
    {
      switch (_decodedMsg[0])
      {
      case BCS_CIG: TRACE_EVENT ("Rec  BCS_CIG");
        {
        MCAST (bcs_cig, BCS_CIG);
        t30_data->hdlc_rcv.c_cig = bcs_cig->c_clg_sub_nr;
        MEMCOPSIZ(t30_data->hdlc_rcv.cig, bcs_cig->clg_sub_nr);
        } 
        break;

      case BCS_CSI: TRACE_EVENT ("Rec  BCS_CSI");
        {
        MCAST (bcs_csi, BCS_CSI);
        t30_data->hdlc_rcv.c_csi = bcs_csi->c_cld_sub_nr;
        MEMCOPSIZ(t30_data->hdlc_rcv.csi, bcs_csi->cld_sub_nr);
        }
        break;

      case BCS_DCN: TRACE_EVENT ("Rec  BCS_DCN");
        SET_STATE (KER, T30_IDLE);
        TIMERSTOP (T1_INDEX);
        snd_t30_sgn_ind(SGN_DCN);
        break;

      case BCS_DIS: TRACE_EVENT ("Rec  BCS_DIS");
        {
        MCAST (bcs_dis, BCS_DIS);
        TIMERSTOP (T1_INDEX);
        ker_fill_dis_info (bcs_dis);
        t30_data->fmod    = FMOD_SND;
        t30_data->repeat  = 1;
        t30_data->dir     = 0x80;
        snd_t30_cap_ind();
        SET_STATE (KER, T30_SND_CAP);
        memset (&t30_data->hdlc_snd, 0, sizeof(T_hdlc_info));
        }
        break;

      case BCS_DTC: TRACE_EVENT ("Rec  BCS_DTC");
        {
        MCAST (bcs_dtc, BCS_DTC);
        TIMERSTOP (T1_INDEX);
        ker_fill_dtc_info (bcs_dtc);
        t30_data->fmod    = FMOD_SND;
        t30_data->repeat  = 1;
        snd_t30_cap_ind();
        SET_STATE (KER, T30_SND_CAP);
        memset (&t30_data->hdlc_snd, 0, sizeof(T_hdlc_info));
        }
        break;

      case BCS_NSC: TRACE_EVENT ("Rec  BCS_NSC");
        {
        MCAST (bcs_nsc, BCS_NSC);
        t30_data->hdlc_rcv.c_nsc = (UBYTE)(bcs_nsc->non_std_fac.l_non_std_fac >> 3);
        MEMCOPLEN(nsc, bcs_nsc->non_std_fac.b_non_std_fac, c_nsc);
        }
        break;

      case BCS_NSF: TRACE_EVENT ("Rec  BCS_NSF");
        {
        MCAST (bcs_nsf, BCS_NSF);
        t30_data->hdlc_rcv.c_nsf = (UBYTE)(bcs_nsf->non_std_fac.l_non_std_fac >> 3);
        MEMCOPLEN(nsf, bcs_nsf->non_std_fac.b_non_std_fac, c_nsf);
        }
        break;

      case BCS_PWD_POLL: TRACE_EVENT ("Rec  BCS_PWD_POLL");
        {
        MCAST (bcs_pwd_poll, BCS_PWD_POLL);
        t30_data->hdlc_rcv.c_pwd = bcs_pwd_poll->c_pm_pword;
        MEMCOPSIZ(t30_data->hdlc_rcv.pwd, bcs_pwd_poll->pm_pword);
        }
        break;

      case BCS_SEP: TRACE_EVENT ("Rec  BCS_SEP");
        {
        MCAST (bcs_sep, BCS_SEP);
        t30_data->hdlc_rcv.c_sep = bcs_sep->c_pm_sub_addr;
        MEMCOPSIZ(t30_data->hdlc_rcv.sep, bcs_sep->pm_sub_addr);
        }
        break;

      default:
        TIMERSTOP (T1_INDEX);
        snd_error_ind(ERR_PH_B_SND_COMREC_INVALID_CMD_RCVD);
        break;
      }
      break;
    }
    
    case T30_RCV_DCN:
      switch (_decodedMsg[0])
      {
      case BCS_DCN: TRACE_EVENT ("Rec  BCS_DCN");
        snd_complete_ind(CMPL_EOP);
        break;

      default:
        snd_error_ind(ERR_PH_D_RCV_INVALID_RESP_RCVD);
        SET_STATE (KER, T30_IDLE);
        break;
      }
      TIMERSTOP (T2_INDEX);
      break;

    case T30_RCV_DCS:
    {
      switch (_decodedMsg[0])
      {
      case BCS_CIG: TRACE_EVENT ("Rec  BCS_CIG");
        {
        MCAST (bcs_cig, BCS_CIG);
        t30_data->hdlc_rcv.c_cig = bcs_cig->c_clg_sub_nr;
        MEMCOPSIZ(t30_data->hdlc_rcv.cig, bcs_cig->clg_sub_nr);
        }
        break;

      case BCS_CRP: TRACE_EVENT ("Rec  BCS_CRP");
        SET_STATE (KER, T30_RCV_DIS);
        ker_send_dis ();
        break;

      case BCS_CSI: TRACE_EVENT ("Rec  BCS_CSI");
        {
        MCAST (bcs_csi, BCS_CSI);
        t30_data->hdlc_rcv.c_csi = bcs_csi->c_cld_sub_nr;
        MEMCOPSIZ(t30_data->hdlc_rcv.csi, bcs_csi->cld_sub_nr);
        }
        break;

      case BCS_DCN: TRACE_EVENT ("Rec  BCS_DCN");
        SET_STATE (KER, T30_IDLE);
        TIMERSTOP (T1_INDEX);
        TIMERSTOP (T4_INDEX);
        snd_t30_sgn_ind(SGN_DCN);
        break;

      case BCS_DCS: TRACE_EVENT ("Rec  BCS_DCS");
        {
        MCAST (bcs_dcs, BCS_DCS);
        ker_fill_dcs_info (bcs_dcs);
        }
        TIMERSTOP (T1_INDEX);
        TIMERSTOP (T4_INDEX);
        
        if (t30_data->fmod NEQ FMOD_POLL)
          t30_data->dir = 0;

        t30_data->fmod    = FMOD_RCV;
        t30_data->repeat  = 1;
        snd_t30_cap_ind();
        SET_STATE (KER, T30_RCV_TCF);
        snd_fad_rcv_tcf_req();
        break;

      case BCS_DIS: TRACE_EVENT ("Rec  BCS_DIS");
        {
        MCAST (bcs_dis, BCS_DIS);
        ker_fill_dis_info (bcs_dis);
        }
        TIMERSTOP (T1_INDEX);
        TIMERSTOP (T4_INDEX);
        t30_data->fmod    = FMOD_SND;
        t30_data->prev    = HDLC_ADDR;
        t30_data->dir     = 0x80;
        snd_t30_cap_ind();          
        SET_STATE (KER, T30_IDLE);
        break;

      case BCS_DTC: TRACE_EVENT ("Rec  BCS_DTC");
        {
        MCAST (bcs_dtc, BCS_DTC);
        ker_fill_dtc_info (bcs_dtc);
        }
        TIMERSTOP (T1_INDEX);
        TIMERSTOP (T4_INDEX);

        if (t30_data->fmod NEQ FMOD_POLL)
          t30_data->dir = 0;

        t30_data->fmod = FMOD_POLL;
        snd_t30_cap_ind();          
        SET_STATE (KER, T30_SND_CAP);
        break;

      case BCS_MPS: TRACE_EVENT ("Rec  BCS_MPS");
        switch (t30_data->prev)
        {
        case BCS_RTN:
          _decodedMsg[0] = BCS_RTN;
          SET_STATE (KER, T30_RCV_RT2);
          sig_ker_bcs_bdat_req (FINAL_YES);
          break;

        case BCS_RTP: TRACE_EVENT ("Rec  BCS_RTP");
          _decodedMsg[0] = BCS_RTP;
          SET_STATE (KER, T30_RCV_RT2);
          sig_ker_bcs_bdat_req (FINAL_YES);
          break;

        default:
          snd_error_ind(ERR_PH_B_RCV_INVALID_RESP_RCVD);
          TIMERSTOP (T1_INDEX);
          TIMERSTOP (T4_INDEX);
          SET_STATE (KER, T30_IDLE);
          break;
        }
        break;

      case BCS_NSC: TRACE_EVENT ("Rec  BCS_NSC");
        {
        MCAST (bcs_nsc, BCS_NSC);
        t30_data->hdlc_rcv.c_nsc = (UBYTE)(bcs_nsc->non_std_fac.l_non_std_fac >> 3);
        MEMCOPLEN(nsc, bcs_nsc->non_std_fac.b_non_std_fac, c_nsc);
        }
        break;

      case BCS_NSF: TRACE_EVENT ("Rec  BCS_NSF");
        {
        MCAST (bcs_nsf, BCS_NSF);
        t30_data->hdlc_rcv.c_nsf = (UBYTE)(bcs_nsf->non_std_fac.l_non_std_fac >> 3);
        MEMCOPLEN(nsf, bcs_nsf->non_std_fac.b_non_std_fac, c_nsf);
        }
        break;

      case BCS_NSS: TRACE_EVENT ("Rec  BCS_NSS");
        {
        MCAST (bcs_nss, BCS_NSS);
        t30_data->hdlc_rcv.c_nss = (UBYTE)(bcs_nss->non_std_fac.l_non_std_fac >> 3);
        MEMCOPLEN(nss, bcs_nss->non_std_fac.b_non_std_fac, c_nss);
        }
        break;

      case BCS_PWD_POLL: TRACE_EVENT ("Rec  BCS_PWD_POLL");
        {
        MCAST (bcs_pwd_poll, BCS_PWD_POLL);
        t30_data->hdlc_rcv.c_pwd = bcs_pwd_poll->c_pm_pword;
        MEMCOPSIZ(t30_data->hdlc_rcv.pwd, bcs_pwd_poll->pm_pword);
        }
        break;

      case BCS_PWD_SND: TRACE_EVENT ("Rec  BCS_PWD_SND");
        {
        MCAST (bcs_pwd_snd, BCS_PWD_SND);
        t30_data->hdlc_rcv.c_pwd = bcs_pwd_snd->c_sm_pword;
        MEMCOPSIZ(t30_data->hdlc_rcv.pwd, bcs_pwd_snd->sm_pword);
        }
        break;

      case BCS_SEP: TRACE_EVENT ("Rec  BCS_SEP");
        {
        MCAST (bcs_sep, BCS_SEP);
        t30_data->hdlc_rcv.c_sep = bcs_sep->c_pm_sub_addr;
        MEMCOPSIZ(t30_data->hdlc_rcv.sep, bcs_sep->pm_sub_addr);
        }
        break;

      case BCS_SUB: TRACE_EVENT ("Rec  BCS_SUB");
        {
        MCAST (bcs_sub, BCS_SUB);
        t30_data->hdlc_rcv.c_sub = bcs_sub->c_sub_addr;
        MEMCOPSIZ(t30_data->hdlc_rcv.sub, bcs_sub->sub_addr);
        }
        break;

      case BCS_TSI: TRACE_EVENT ("Rec  BCS_TSI");
        {
        MCAST (bcs_tsi, BCS_TSI);
        t30_data->hdlc_rcv.c_tsi = bcs_tsi->c_tra_sub_nr;
        MEMCOPSIZ(t30_data->hdlc_rcv.tsi, bcs_tsi->tra_sub_nr);
        }
        break;

      default:
        TRACE_EVENT_P1 ("Rec  %02x", _decodedMsg[0]);
        snd_error_ind(ERR_PH_B_RCV_INVALID_RESP_RCVD);
        TIMERSTOP (T1_INDEX);
        TIMERSTOP (T4_INDEX);
        SET_STATE (KER, T30_IDLE);
        break;
      }
      break;
    }

    case T30_RCV_TCF:
      switch (_decodedMsg[0])
      {
      case BCS_DCS: TRACE_EVENT ("Rec  BCS_DCS");
        TIMERSTOP (T1_INDEX);
        TIMERSTOP (T4_INDEX);

        if (t30_data->rate_modified)
          snd_fad_rcv_tcf_req();
        break;
      }
      break;

    case T30_RCV_MSG:
      switch (_decodedMsg[0])
      {
      case BCS_DCN: TRACE_EVENT ("Rec  BCS_DCN");
        snd_t30_sgn_ind(SGN_DCN);
        SET_STATE (KER, T30_IDLE);
        TIMERSTOP (T2_INDEX);
        break;

      case BCS_MPS: TRACE_EVENT ("Rec  BCS_MPS");
        TIMERSTOP (T2_INDEX);
        _decodedMsg[0] = t30_data->prev;
        SET_STATE (KER, T30_RCV_CFR);
        sig_ker_bcs_bdat_req (FINAL_YES);
        break;

      case BCS_TSI: TRACE_EVENT ("Rec  BCS_TSI");
        {
        MCAST (bcs_tsi, BCS_TSI);
        t30_data->hdlc_rcv.c_tsi = bcs_tsi->c_tra_sub_nr;
        MEMCOPSIZ(t30_data->hdlc_rcv.tsi, bcs_tsi->tra_sub_nr);
        }
        SET_STATE (KER, T30_RCV_DCS);
        break;

      default:
        snd_error_ind(ERR_PH_B_RCV_INVALID_RESP_RCVD);
        TIMERSTOP (T2_INDEX);
        SET_STATE (KER, T30_IDLE);
        break;
      }
      break;

    case T30_RCV_PST:
      TIMERSTOP (T2_INDEX);
      switch (_decodedMsg[0])
      {
      case BCS_DCN: TRACE_EVENT ("Rec  BCS_DCN");
        SET_STATE (KER, T30_IDLE);
        snd_t30_sgn_ind(SGN_DCN);
        break;

      case BCS_EOM: TRACE_EVENT ("Rec  BCS_EOM");
        t30_data->res = SGN_EOM;
        snd_t30_sgn_ind(SGN_EOM);
        break;

      case BCS_EOP: TRACE_EVENT ("Rec  BCS_EOP");
        t30_data->res = SGN_EOP;
        snd_t30_sgn_ind(SGN_EOP);
        break;

      case BCS_MPS: TRACE_EVENT ("Rec  BCS_MPS");
        t30_data->res = SGN_MPS;
        snd_t30_sgn_ind(SGN_MPS);
        break;

      case BCS_PRI_EOM: TRACE_EVENT ("Rec  BCS_PRI_EOM");
        t30_data->res = SGN_PRI_EOM;
        snd_t30_sgn_ind(SGN_PRI_EOM);
        break;

      case BCS_PRI_EOP: TRACE_EVENT ("Rec  BCS_PRI_EOP");
        t30_data->res = SGN_PRI_EOP;
        snd_t30_sgn_ind(SGN_PRI_EOP);
        break;

      case BCS_PRI_MPS: TRACE_EVENT ("Rec  BCS_PRI_MPS");
        t30_data->res = SGN_PRI_MPS;
        snd_t30_sgn_ind(SGN_PRI_MPS);
        break;

      default:
        snd_error_ind(ERR_PH_D_RCV_INVALID_RESP_RCVD);
        SET_STATE (KER, T30_IDLE);
        break;
      }
      break;

    case T30_RCV_T2:
      TIMERSTOP (T2_INDEX);
      switch (_decodedMsg[0])
      {
      case BCS_DCN: TRACE_EVENT ("Rec  BCS_DCN");
        snd_t30_sgn_ind(SGN_DCN);
        SET_STATE (KER, T30_IDLE);
        break;

      case BCS_EOM: TRACE_EVENT ("Rec  BCS_EOM");
        TIMERSTART (T2_INDEX, T2_VALUE);
        _decodedMsg[0] = BCS_MCF;
        sig_ker_bcs_bdat_req (FINAL_YES);
        break;

      default:
        snd_error_ind(ERR_PH_D_RCV_INVALID_RESP_RCVD);
        SET_STATE (KER, T30_IDLE);
        break;
      }
      break;

    case T30_SND_CFR:
      switch (_decodedMsg[0])
      {
      case BCS_CFR: TRACE_EVENT ("Rec  BCS_CFR");
        TIMERSTOP (T4_INDEX);
        SET_STATE (KER, T30_SND_MSG);
        t30_data->mux.mode = MUX_MSG;
        sig_ker_mux_mux_req ();
        snd_t30_phase_ind(MSG_PHASE);
        break;

      case BCS_CIG: TRACE_EVENT ("Rec  BCS_CIG");
        {
        MCAST (bcs_cig, BCS_CIG);
        t30_data->hdlc_rcv.c_cig = bcs_cig->c_clg_sub_nr;
        MEMCOPSIZ(t30_data->hdlc_rcv.cig, bcs_cig->clg_sub_nr);
        }
        break;

      case BCS_CRP: TRACE_EVENT ("Rec  BCS_CRP");
        TIMERSTOP (T4_INDEX);
        t30_data->repeat++;
        SET_STATE (KER, T30_SND_CAP);
        snd_t30_sgn_ind(SGN_CRP);
        break;

      case BCS_CSI: TRACE_EVENT ("Rec  BCS_CSI");
        {
        MCAST (bcs_csi, BCS_CSI);
        t30_data->hdlc_rcv.c_csi = bcs_csi->c_cld_sub_nr;
        MEMCOPSIZ(t30_data->hdlc_rcv.csi, bcs_csi->cld_sub_nr);
        }
        break;

      case BCS_DCN: TRACE_EVENT ("Rec  BCS_DCN");
        SET_STATE (KER, T30_IDLE);
        TIMERSTOP (T4_INDEX);
        snd_t30_sgn_ind(SGN_DCN);
        break;

      case BCS_DIS: TRACE_EVENT ("Rec  BCS_DIS");
        TIMERSTOP (T4_INDEX);

        if (t30_data->repeat++ EQ 3)
        {
          snd_error_ind(ERR_PH_B_SND_DIS_DTC_RCVD_3_TIMES);
          SET_STATE (KER, T30_IDLE);
        }
        else
        {
          MCAST (bcs_dis, BCS_DIS);
          ker_fill_dis_info (bcs_dis);
          t30_data->fmod = FMOD_SND;
          snd_t30_cap_ind();          
          SET_STATE (KER, T30_SND_CAP);
        }
        memset (&t30_data->hdlc_snd, 0, sizeof(T_hdlc_info));
        break;

      case BCS_DTC: TRACE_EVENT ("Rec  BCS_DTC");
        TIMERSTOP (T4_INDEX);
        if (t30_data->repeat++ EQ 3)
        {
          snd_error_ind(ERR_PH_B_SND_DIS_DTC_RCVD_3_TIMES);
          SET_STATE (KER, T30_IDLE);
        }
        else
        {
          MCAST (bcs_dtc, BCS_DTC);
          ker_fill_dtc_info (bcs_dtc);
          t30_data->fmod = FMOD_POLL;
          snd_t30_cap_ind();          
          SET_STATE (KER, T30_SND_CAP);
        }
        memset (&t30_data->hdlc_snd, 0, sizeof(T_hdlc_info));
        break;

      case BCS_FTT: TRACE_EVENT ("Rec  BCS_FTT");
        TIMERSTOP (T4_INDEX);
        SET_STATE (KER, T30_SND_CAP);
        snd_t30_sgn_ind(SGN_FTT);
        break;

      case BCS_NSC: TRACE_EVENT ("Rec  BCS_NSC");
        {
        MCAST (bcs_nsc, BCS_NSC);
        t30_data->hdlc_rcv.c_nsc = (UBYTE)(bcs_nsc->non_std_fac.l_non_std_fac >> 3);
        MEMCOPLEN(nsc, bcs_nsc->non_std_fac.b_non_std_fac, c_nsc);
        }
        break;

      case BCS_NSF: TRACE_EVENT ("Rec  BCS_NSF");
        {
        MCAST (bcs_nsf, BCS_NSF);
        t30_data->hdlc_rcv.c_nsf = (UBYTE)(bcs_nsf->non_std_fac.l_non_std_fac >> 3);
        MEMCOPLEN(nsf, bcs_nsf->non_std_fac.b_non_std_fac, c_nsf);
        }
        break;

      default:
        snd_error_ind(ERR_PH_B_SND_INVALID_RESP_RCVD);
        TIMERSTOP (T4_INDEX);
        SET_STATE (KER, T30_IDLE);
        break;
      }
      break;
    
    case T30_SND_MCF:
      switch (_decodedMsg[0])
      {
      case BCS_CRP: TRACE_EVENT ("Rec  BCS_CRP");
        if (t30_data->repeat EQ 3)
        {
          snd_error_ind(ERR_PH_D_SND_RSPREC);
          SET_STATE (KER, T30_DCN);
          _decodedMsg[0] = BCS_DCN;
          sig_ker_bcs_bdat_req (FINAL_YES);
        }
        else
        {
          t30_data->repeat++;
          act_on_sgn_req();
        }
        break;

      case BCS_DCN: TRACE_EVENT ("Rec  BCS_DCN");
        SET_STATE (KER, T30_IDLE);
        TIMERSTOP (T4_INDEX);
        snd_t30_sgn_ind(SGN_DCN);
        break;

      case BCS_MCF: TRACE_EVENT ("Rec  BCS_MCF");
        switch (t30_data->sgn_req)
        {
        case SGN_EOM:
          TIMERSTOP (T1_INDEX);
          TIMERSTOP (T4_INDEX);
          snd_complete_ind(CMPL_EOM);
          break;

        case SGN_EOP:
          TIMERSTOP (T4_INDEX);
          _decodedMsg[0] = BCS_DCN;
          SET_STATE (KER, T30_SND_DCN);
          sig_ker_bcs_bdat_req (FINAL_YES);
          break;

        case SGN_MPS:
          TIMERSTOP (T4_INDEX);
          SET_STATE (KER, T30_SND_MSG);
          t30_data->mux.mode = MUX_MSG;
          sig_ker_mux_mux_req ();
          snd_t30_phase_ind(MSG_PHASE);
          break;

        default:
          break;
        }
        break;

      case BCS_PIN: TRACE_EVENT ("Rec  BCS_PIN");
        TIMERSTOP (T4_INDEX);
        SET_STATE (KER, T30_SND_PI);
        snd_t30_sgn_ind(SGN_PIN);
        break;

      case BCS_PIP: TRACE_EVENT ("Rec  BCS_PIP");
        TIMERSTOP (T4_INDEX);
        SET_STATE (KER, T30_SND_PI);
        snd_t30_sgn_ind(SGN_PIP);
        break;

      case BCS_RTN: TRACE_EVENT ("Rec  BCS_RTN");
        TIMERSTOP (T4_INDEX);
        switch (t30_data->sgn_req)
        {
        case SGN_EOM:
          SET_STATE (KER, T30_IDLE);
          snd_t30_sgn_ind(SGN_RTN);
          break;

        case SGN_EOP:
        case SGN_MPS:
          SET_STATE (KER, T30_SND_CAP);
          snd_t30_sgn_ind(SGN_RTN);
          break;

        default:
          snd_error_ind(ERR_PH_D_SND_UNSPEC);
          SET_STATE (KER, T30_DCN);
          _decodedMsg[0] = BCS_DCN;
          sig_ker_bcs_bdat_req (FINAL_YES);
          break;
        }
        break;

      case BCS_RTP: TRACE_EVENT ("Rec  BCS_RTP");
        TIMERSTOP (T4_INDEX);
        switch (t30_data->sgn_req)
        {
        case SGN_EOM:
          SET_STATE (KER, T30_IDLE);
          snd_t30_sgn_ind(SGN_RTP);
          break;

        case SGN_EOP:
          _decodedMsg[0] = BCS_DCN;
          SET_STATE (KER, T30_SND_DCN);
          sig_ker_bcs_bdat_req (FINAL_YES);
          break;

        case SGN_MPS:
          SET_STATE (KER, T30_SND_CAP);
          snd_t30_sgn_ind(SGN_RTP);
          break;

        default:
          snd_error_ind(ERR_PH_D_SND_UNSPEC);
          SET_STATE (KER, T30_DCN);
          _decodedMsg[0] = BCS_DCN;
          sig_ker_bcs_bdat_req(FINAL_YES);
          break;
        }
        break;

      default:
        TIMERSTOP (T4_INDEX);
        switch (t30_data->sgn_req)
        {
        case SGN_EOM: case SGN_PRI_EOM:
          snd_error_ind(ERR_PH_D_SND_INVALID_RESP_TO_EOM);
          break;

        case SGN_EOP: case SGN_PRI_EOP:
          snd_error_ind(ERR_PH_D_SND_INVALID_RESP_TO_EOP);
          break;

        case SGN_MPS: case SGN_PRI_MPS:
          snd_error_ind(ERR_PH_D_SND_INVALID_RESP_TO_MPS);
          break;

        default:
          snd_error_ind(ERR_PH_D_SND_UNSPEC);
          break;
        }
        SET_STATE (KER, T30_DCN);
        _decodedMsg[0] = BCS_DCN;
        sig_ker_bcs_bdat_req (FINAL_YES);
        break;
      }
      break;

    default:
      break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERS            |
| STATE   : code                       ROUTINE : sig_msg_ker_mdat_ind|
+--------------------------------------------------------------------+
  PURPOSE : Process signal MDAT_IND received from process BCS.
            The signal contains a fax data block which is passed to DTI.
*/
GLOBAL void sig_msg_ker_mdat_ind (T_FAD_DATA_IND *fad_data_ind)
{
  TRACE_FUNCTION ("sig_msg_ker_mdat_ind()");
  switch (GET_STATE (KER))
  {
    case T30_RCV_DAT:
      if (fad_data_ind->final)
      {
        TIMERSTART (T2_INDEX, MSG_VALUE);
        SET_STATE (KER, T30_RCV_RDYF);
      }
      else
      {
        SET_STATE (KER, T30_RCV_RDY);
      }
      prepare_dti_data_ind(fad_data_ind);
      snd_dti_data_ind(&t30_data->dti_data_ind);
      break;

    case T30_RCV_DATW:
      if (fad_data_ind->final)
      {
        TIMERSTART (T2_INDEX, MSG_VALUE);
        SET_STATE (KER, T30_RCV_RDYF);
      }
      else
      {
        TIMERSTOP (T2_INDEX);
        SET_STATE (KER, T30_RCV_RDY);
      }
      prepare_dti_data_ind(fad_data_ind);
      snd_dti_data_ind(&t30_data->dti_data_ind);
      break;

    case T30_RCV_MSG:
    case T30_RCV_MSGW:
      if (fad_data_ind->final)
      {
        TIMERSTART (T2_INDEX, MSG_VALUE);
        SET_STATE (KER, T30_RCV_RDYF);
      }
      prepare_dti_data_ind(fad_data_ind);
      break;

    case T30_RCV_RDY:
      prepare_dti_data_ind(fad_data_ind);
      break;

    default:
      TRACE_EVENT("ERROR: FAD_DATA_IND in wrong KER state");
      break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERS            |
| STATE   : code                       ROUTINE : sig_bcs_ker_err_ind |
+--------------------------------------------------------------------+
  PURPOSE : Process signal ERR_IND received from process BCS.
            This signal contains an error message from the BCS formatter.
*/
GLOBAL void sig_bcs_ker_err_ind (UBYTE cause)
{
  const char tab[8][25] =
  {
    "",
    "BUF_FULL",
    "CCD_DEC",
    "CCD_ENC",
    "FCS",
    "FINAL",
    "FRAME_NO_FLAG",
    "FRAME_TOO_MANY_FRAMES"
  };

  TRACE_FUNCTION ("sig_bcs_ker_err_ind()");
  TRACE_EVENT_P2("*** BCS error cause = %d, %s", cause, tab[cause]);

  switch (GET_STATE (KER))
  {
    case T30_SND_CFR:
    {
      switch (cause)
      {
      case ERR_FCS:
      {
        TIMERSTOP (T4_INDEX);
        t30_data->repeat++;
        SET_STATE (KER, T30_SND_CAP);
        snd_t30_sgn_ind(SGN_FCS_ERR);
        break;
      }
      default:
        break;
      }
      break ;
    }
    case T30_SND_MCF:
      switch (cause)
      {
      case ERR_FCS:
        if (t30_data->repeat EQ 3)
        {
          snd_error_ind(ERR_PH_D_SND_RSPREC);
          SET_STATE (KER, T30_DCN);
          _decodedMsg[0] = BCS_DCN;
          sig_ker_bcs_bdat_req (FINAL_YES);
        }
        break;

      default:
        t30_data->repeat++;
        act_on_sgn_req();
        break;
      }
      break;

    default:
      break;
  }
}
#undef MEMCOPLEN
