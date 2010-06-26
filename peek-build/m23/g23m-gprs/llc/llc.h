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
|  Purpose :  Definitions for the Protocol Stack Entity
|             Logical Link Control (LLC)
+-----------------------------------------------------------------------------
*/

#ifndef LLC_H
#define LLC_H

/*#ifdef  FF_EGPRS
#ifndef LLC_EDGE
#define LLC_EDGE
#endif  *//*LLC_EDGE*/
/*#endif */  /*FF_EGPRS*/


/*
 * Shift of constants from SAPs LL and LLGMM to PS_include are handled here.
 */
#ifdef LL_2to1


#ifndef DTACS_MOBILITY_MANAGEMENT
#define DTACS_MOBILITY_MANAGEMENT GRLC_MOBILITY_MANAGEMENT
#endif
#ifndef GRR_DTACS_DEFGRR_DTACS_DEF
#define GRR_DTACS_DEF GRLC_DTACS_DEF
#endif
#ifndef GRR_DTACS_MOBILITY_MANAGEMENT
#define GRR_DTACS_MOBILITY_MANAGEMENT GRLC_DTACS_MOBILITY_MANAGEMENT
#endif


#ifndef LLC_MAX_CNF
#define LLC_MAX_CNF LL_MAX_CNF
#endif
#ifndef LLC_MAX_L3_XID_LEN
#define LLC_MAX_L3_XID_LEN LL_MAX_L3_XID_LEN
#endif



#ifndef LLGMM_ERRCS_ACK_NO_PEER_RES_REEST
#define LLGMM_ERRCS_ACK_NO_PEER_RES_REEST CAUSE_LLC_ACK_NO_PEER_RES_REEST
#endif
#ifndef LLGMM_ERRCS_DISC_NO_PEER_RES
#define LLGMM_ERRCS_DISC_NO_PEER_RES CAUSE_LLC_DISC_NO_PEER_RES
#endif
#ifndef LLGMM_ERRCS_DM1_RECEIVED
#define LLGMM_ERRCS_DM1_RECEIVED CAUSE_LLC_DM1_RECEIVED
#endif
#ifndef LLGMM_ERRCS_DM1_RECEIVED_REEST
#define LLGMM_ERRCS_DM1_RECEIVED_REEST CAUSE_LLC_DM1_RECEIVED_REEST
#endif
#ifndef LLGMM_ERRCS_FRMR_COND
#define LLGMM_ERRCS_FRMR_COND CAUSE_LLC_FRMR_COND
#endif
#ifndef LLGMM_ERRCS_FRMR_COND_REEST
#define LLGMM_ERRCS_FRMR_COND_REEST CAUSE_LLC_FRMR_COND_REEST
#endif
#ifndef LLGMM_ERRCS_FRMR_RECEIVED
#define LLGMM_ERRCS_FRMR_RECEIVED CAUSE_LLC_FRMR_RECEIVED
#endif
#ifndef LLGMM_ERRCS_L3_REEST
#define LLGMM_ERRCS_L3_REEST CAUSE_LLC_L3_REEST
#endif
#ifndef LLGMM_ERRCS_MULT_ASS_TLLI
#define LLGMM_ERRCS_MULT_ASS_TLLI CAUSE_LLC_MULT_ASS_TLLI
#endif
#ifndef LLGMM_ERRCS_NO_PEER_RES
#define LLGMM_ERRCS_NO_PEER_RES CAUSE_LLC_NO_PEER_RES
#endif
#ifndef LLGMM_ERRCS_PEER_BUSY_REEST
#define LLGMM_ERRCS_PEER_BUSY_REEST CAUSE_LLC_PEER_BUSY_REEST
#endif
#ifndef LLGMM_ERRCS_PEER_REEST
#define LLGMM_ERRCS_PEER_REEST CAUSE_LLC_PEER_REEST
#endif
#ifndef LLGMM_ERRCS_SABM_NO_PEER_RES
#define LLGMM_ERRCS_SABM_NO_PEER_RES CAUSE_LLC_SABM_NO_PEER_RES
#endif
#ifndef LLGMM_ERRCS_XID_NO_PEER_RE
#define LLGMM_ERRCS_XID_NO_PEER_RES CAUSE_LLC_XID_NO_PEER_RES
#endif
#ifndef LLGMM_ERRCS_DM0_RECEIVED_REEST
#define LLGMM_ERRCS_DM0_RECEIVED_REEST CAUSE_LLC_DM0_RECEIVED_REEST
#endif


#ifndef LL_ERRCS_NO_PEER_RES
#define LL_ERRCS_NO_PEER_RES CAUSE_LLC_NO_PEER_RES
#endif
#ifndef LL_ERRCS_INVALID_XID
#define LL_ERRCS_INVALID_XID CAUSE_LLC_INVALID_XID
#endif

#ifndef LL_PEAK_SUB
#define LL_PEAK_SUB PS_PEAK_SUB
#endif
#ifndef LL_RELCS_DM_RECEIVED
#define LL_RELCS_DM_RECEIVED CAUSE_LLC_DM_RECEIVED
#endif
#ifndef LL_RELCS_INVALID_XID
#define LL_RELCS_INVALID_XID CAUSE_LLC_INVALID_XID
#endif
#ifndef LL_RELCS_NORMAL
#define LL_RELCS_NORMAL CAUSE_LLC_NORMAL_REL
#endif
#ifndef LL_RELCS_NO_PEER_RES
#define LL_RELCS_NO_PEER_RES CAUSE_LLC_NO_PEER_RES
#endif

#ifndef LL_DELAY_SUB
#define LL_DELAY_SUB PS_DELAY_SUB
#endif
#ifndef LL_MEAN_SUB
#define LL_MEAN_SUB PS_MEAN_SUB
#endif
#ifndef LL_NO_REL
#define LL_NO_REL PS_NO_REL
#endif
#ifndef LL_PRECED_SUB
#define LL_PRECED_SUB PS_PRECED_SUB
#endif
#ifndef LL_RLC_PROT
#define LL_RLC_PROT PS_RLC_PROT
#endif

#ifndef LL_SAPI_1
#define LL_SAPI_1 PS_SAPI_1
#endif
#ifndef LL_SAPI_3
#define LL_SAPI_3 PS_SAPI_3
#endif
#ifndef LL_SAPI_5
#define LL_SAPI_5 PS_SAPI_5
#endif
#ifndef LL_SAPI_7
#define LL_SAPI_7 PS_SAPI_7
#endif
#ifndef LL_SAPI_9
#define LL_SAPI_9 PS_SAPI_9
#endif
#ifndef LL_SAPI_11
#define LL_SAPI_11 PS_SAPI_11
#endif
#ifndef LL_RADIO_PRIO_1
#define LL_RADIO_PRIO_1 PS_RADIO_PRIO_1
#endif
#ifndef LL_TLLI_INVALID
#define LL_TLLI_INVALID PS_TLLI_INVALID
#endif

#ifndef GRLC_RADIO_PRIO_1
#define GRLC_RADIO_PRIO_1 PS_RADIO_PRIO_1
#endif
#ifndef GRLC_PEAK_SUB
#define GRLC_PEAK_SUB PS_PEAK_SUB
#endif

#ifndef T_desc_list3
#define T_desc_list3 T_LL_desc_list3
#endif

#endif /* LL_2to1 */



/* --------------------------------------------------------------------------------- */

/* remove traces */
#ifndef _SIMULATION_
#ifdef GET_STATE
  #undef GET_STATE
  #define GET_STATE(P)        (ENTITY_DATA->P state)
#endif
#endif /* _SIMULATION_ */
/* --------------------------------------------------------------------------------- */
#ifdef TRACE_EVE
  #define TRACE_0_INFO(s)                 TRACE_EVENT   ("Info: " s)
  #define TRACE_1_INFO(s,p1)              TRACE_EVENT_P1("Info: " s,p1)
  #define TRACE_2_INFO(s,p1,p2)           TRACE_EVENT_P2("Info: " s,p1,p2)
  #define TRACE_3_INFO(s,p1,p2,p3)        TRACE_EVENT_P3("Info: " s,p1,p2,p3)
  #define TRACE_4_INFO(s,p1,p2,p3,p4)     TRACE_EVENT_P4("Info: " s,p1,p2,p3,p4)
#else
  #define TRACE_0_INFO(s)
  #define TRACE_1_INFO(s,p1)
  #define TRACE_2_INFO(s,p1,p2)
  #define TRACE_3_INFO(s,p1,p2,p3)
  #define TRACE_4_INFO(s,p1,p2,p3,p4)
#endif

#ifdef TRACE_PRIM
  #define TRACE_0_PARA(s)                 vsi_o_ttrace(VSI_CALLER TC_PRIM, "IPar: " s)
  #define TRACE_1_PARA(s,p1)              vsi_o_ttrace(VSI_CALLER TC_PRIM, "IPar: " s,p1)
  #define TRACE_2_PARA(s,p1,p2)           vsi_o_ttrace(VSI_CALLER TC_PRIM, "IPar: " s,p1,p2)
  #define TRACE_3_PARA(s,p1,p2,p3)        vsi_o_ttrace(VSI_CALLER TC_PRIM, "IPar: " s,p1,p2,p3)
  #define TRACE_4_PARA(s,p1,p2,p3,p4)     vsi_o_ttrace(VSI_CALLER TC_PRIM, "IPar: " s,p1,p2,p3,p4)

  #define TRACE_0_OUT_PARA(s)             vsi_o_ttrace(VSI_CALLER TC_PRIM, "OPar: " s)
  #define TRACE_1_OUT_PARA(s,p1)          vsi_o_ttrace(VSI_CALLER TC_PRIM, "OPar: " s,p1)
  #define TRACE_2_OUT_PARA(s,p1,p2)       vsi_o_ttrace(VSI_CALLER TC_PRIM, "OPar: " s,p1,p2)
  #define TRACE_3_OUT_PARA(s,p1,p2,p3)    vsi_o_ttrace(VSI_CALLER TC_PRIM, "OPar: " s,p1,p2,p3)
  #define TRACE_4_OUT_PARA(s,p1,p2,p3,p4) vsi_o_ttrace(VSI_CALLER TC_PRIM, "OPar: " s,p1,p2,p3,p4)

  #define TRACE_PRIM_FROM(s)              vsi_o_ttrace(VSI_CALLER TC_PRIM, "Pdir: " s)
  #define TRACE_PRIM_TO(s)                vsi_o_ttrace(VSI_CALLER TC_PRIM, "Pdir: " s)
#else
  #define TRACE_0_PARA(s)
  #define TRACE_1_PARA(s,p1)
  #define TRACE_2_PARA(s,p1,p2)
  #define TRACE_3_PARA(s,p1,p2,p3)
  #define TRACE_4_PARA(s,p1,p2,p3,p4)

  #define TRACE_0_OUT_PARA(s)
  #define TRACE_1_OUT_PARA(s,p1)
  #define TRACE_2_OUT_PARA(s,p1,p2)
  #define TRACE_3_OUT_PARA(s,p1,p2,p3)
  #define TRACE_4_OUT_PARA(s,p1,p2,p3,p4)

  #define TRACE_PRIM_FROM(s)
  #define TRACE_PRIM_TO(s)
#endif
/* --------------------------------------------------------------------------------- */

/* 
 * The following macro is similar to PFREE_DESC2. Instead of doing
 * a PFREE(P) it does a MFREE(p). This macro is used to free CCI primitives
 * which are removed from CCI SAP and added into this header file.
 */
#define MFREE_PRIM_DESC2(p) { MFREE_DESC2 ((p)->desc_list2.first);      \
                              MFREE ((p));                              \
                             }

/* 
 * The following macro is similar to PFREE_DESC. Instead of doing
 * a PFREE(P) it does a MFREE(p). This macro is used to free CCI primitives
 * which are removed from CCI SAP and added into this header file.
 */
#define MFREE_PRIM_DESC(p) { MFREE_DESC ((p)->desc_list.first);      \
                             MFREE ((p));                            \
                           }

/*
 * defines the user of the vsi interface
 */
#define VSI_CALLER            LLC_handle,
#define VSI_CALLER_SINGLE     LLC_handle

/*
 * Macros
 */

/*
 * Switch all LLC services with multiple incarnations to use the incarnation
 * for the given SAPI. Current sapi is stored and all LLC layer parameters
 * with multiple incarnations are switched to use the incarnation for the
 * given SAPI.
 */
#define SWITCH_LLC(s)         llc_data->current_sapi = s;                     \
                              SWITCH_PARAM (iov_i,     IMAP(s));              \
                              SWITCH_PARAM (n200,     UIMAP(s));              \
                              SWITCH_PARAM (n201_u,   UIMAP(s));              \
                              SWITCH_PARAM (n201_i,    IMAP(s));              \
                              SWITCH_PARAM (md,        IMAP(s));              \
                              SWITCH_PARAM (mu,        IMAP(s));              \
                              SWITCH_PARAM (kd,        IMAP(s));              \
                              SWITCH_PARAM (ku,        IMAP(s));              \
                              SWITCH_PARAM (requested_l3_xid, IMAP(s));       \
                              SWITCH_SERVICE (llc, sapi,  UIMAP(s));          \
                              SWITCH_SERVICE (llc, u,     UIMAP(s));          \
                              SWITCH_SERVICE (llc, itx,    IMAP(s));          \
                              SWITCH_SERVICE (llc, irx,    IMAP(s));          \
                              SWITCH_SERVICE (llc, uitx,  UIMAP(s));          \
                              SWITCH_SERVICE (llc, uirx,  UIMAP(s));          \
                              SWITCH_SERVICE (llc, t200,  UIMAP(s));

/*
 * Switch service s of entity e to use incarnation i.
 */
#define SWITCH_SERVICE(e,s,i) e##_data->##s = &(##e##_data->##s##_base[i]);

/*
 * Switch LLC layer parameter p to incarnation i.
 */
#define SWITCH_PARAM(p,i) llc_data->##p = &(llc_data->##p##_base[i]);


/*
 * Map SAPI to incarnation, for either acknowledged (4 incarnations) or
 * unacknowledged operation (6 incarnations)
 */
#ifdef LL_2to1
#define IMAP(s)               (s == PS_SAPI_3 ? 0 :                           \
                               s == PS_SAPI_5 ? 1 :                           \
                               s == PS_SAPI_9 ? 2 :                           \
                               s == PS_SAPI_11? 3 : 0)

#define UIMAP(s)              (s == PS_SAPI_1 ? 0 :                           \
                               s == PS_SAPI_3 ? 1 :                           \
                               s == PS_SAPI_5 ? 2 :                           \
                               s == PS_SAPI_7 ? 3 :                           \
                               s == PS_SAPI_9 ? 4 :                           \
                               s == PS_SAPI_11? 5 : 0)
#else
#define IMAP(s)               (s == LL_SAPI_3 ? 0 :                           \
                               s == LL_SAPI_5 ? 1 :                           \
                               s == LL_SAPI_9 ? 2 :                           \
                               s == LL_SAPI_11? 3 : 0)

#define UIMAP(s)              (s == LL_SAPI_1 ? 0 :                           \
                               s == LL_SAPI_3 ? 1 :                           \
                               s == LL_SAPI_5 ? 2 :                           \
                               s == LL_SAPI_7 ? 3 :                           \
                               s == LL_SAPI_9 ? 4 :                           \
                               s == LL_SAPI_11? 5 : 0)
#endif


/*
 * Add octet size of XID parameter P to variable V if the parameter is tagged.
 */
#define ADD_IF_TAGGED(V,P)                                                    \
  if (llc_data->u->xid_tag & (0x00000001L << P))                              \
  {                                                                           \
    V += P##_LEN + P##_HDR_LEN;                                               \
  }

/*
 * Tag XID parameter if requested to negotiate
 */
#define TAG_IF_REQUESTED(V,P)                                                 \
  if (llc_data->##V##valid)                                                   \
  {                                                                           \
    llc_data->u->xid_tag |= (0x00000001L << P);                               \
  }

/*
 * Tag XID parameter if requested to negotiate and sense of negotiation fits
 */
#define TAG_IF_REQUESTED_RSP(S,V,P)                                           \
  if (llc_data->u->requested_xid.##V##.valid)                                 \
  {                                                                           \
    if (llc_data->decoded_xid.##V##.valid)                                    \
    {                                                                         \
      /* simple add parameter. Sense of negotiation is already checked */     \
      llc_data->u->xid_tag |= (0x00000001L << P);                             \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      /* Sense of negotiation compared with current values */                 \
      if (llc_data->u->requested_xid.##V##.value  S  *(llc_data->##V))        \
      {                                                                       \
        llc_data->u->xid_tag |= (0x00000001L << P);                           \
      }                                                                       \
    }                                                                         \
  }


/*
 * Timer values are negotiated in units of 0.1 seconds but internally stored
 * in milliseconds. Therefore a conversion must be done with each XID
 * negotiation of timer values.
 */
#define XID_TIMER_CONVERSION  100

#define XID2INT(l)            (l * XID_TIMER_CONVERSION)
#define INT2XID(l)            (l / XID_TIMER_CONVERSION)

/*
 * Timer start- and stop macros for LLC
 */
#define TIMERSTART(t,d)       itx_##t##_start((d))
#define TIMERSTOP(t)          itx_##t##_stop()


/*
 * Number of service incarnations (LLME, TX, and RX have only one incarnation)
 */
#define ACKNOWLEDGED_INC      4
#define UNACKNOWLEDGED_INC    6
#define MAX_SAPI_INC          6


#define U_NUM_INC             UNACKNOWLEDGED_INC

#define ITX_NUM_INC           ACKNOWLEDGED_INC
#define IRX_NUM_INC           ACKNOWLEDGED_INC

#define UITX_NUM_INC          UNACKNOWLEDGED_INC
#define UIRX_NUM_INC          UNACKNOWLEDGED_INC

#define T200_NUM_INC          UNACKNOWLEDGED_INC
#define T201_NUM_INC          ACKNOWLEDGED_INC


/*
 * Constants
 */

/*
 * Value constants for attached_counter
 */
#define CCI_NO_ATTACHE                 (0x0)      /* no entity/service is attached to the primitive */
/*
 * Value constants for fcs_check
 */
#define CCI_FCS_PASSED                 (0x0)      /* FCS check has been successfully passed */
#define CCI_FCS_FAILED                 (0x1)      /* FCS check has been failed      */
/*
 * Value constants for pm
 */
#define CCI_PM_UNPROTECTED             (0x0)      /* FCS covers frame header and information fields */
#define CCI_PM_PROTECTED               (0x1)      /* FCS covers frame header field and first N202 octets of information field */
/*
 * Value constants for ciphering_algorithm
 */
#define CCI_CIPHER_NO_ALGORITHM        (0x0)      /* frame shall not be ciphered    */
#define CCI_CIPHER_GPRS_A5_1           (0x1)      /* frame shall be ciphered using algorithm GPRS A5/1 */
#define CCI_CIPHER_GPRS_A5_2           (0x2)      /* frame shall be ciphered using algorithm GPRS A5/2 */
/*
 * Value constants for direction
 */
#define CCI_DIRECTION_UPLINK           (0x0)      /* direction of LLC frame transmission is MS to SGSN */
#define CCI_DIRECTION_DOWNLINK         (0x1)      /* direction of LLC frame transmission is SGSN to MS */



/*
 * Bitoffset for encoding/decoding
 */
#define ENCODE_OFFSET         0


/*
 * C/R bit
 */
#define SGSN_COMMAND          1
#define SGSN_RESPONSE         0
#define MS_COMMAND            0
#define MS_RESPONSE           1

/*
 * Size definitions for common U frames (in octets and in bits).
 * Define only the header of the frame. FCS_SIZE will be added when the FCS
 * is calculated!
 */
#define U_HDR_SIZE            2
#define U_HDR_SIZE_BITS       (U_HDR_SIZE * 8)
#define FCS_SIZE              3
#define FCS_SIZE_BITS         (FCS_SIZE * 8)

#ifdef REL99 
#define U_NULL_SIZE           (U_HDR_SIZE)
#define U_NULL_SIZE_BITS      (U_NULL_SIZE * 8)
#endif /* REL99 */

#define U_DISC_SIZE           (U_HDR_SIZE)
#define U_DISC_SIZE_BITS      (U_DISC_SIZE * 8)

#define U_DM_SIZE             (U_HDR_SIZE)
#define U_DM_SIZE_BITS        (U_DM_SIZE * 8)

#define U_FRMR_INFO_SIZE      10
#define U_FRMR_SIZE           (U_HDR_SIZE + U_FRMR_INFO_SIZE)
#define U_FRMR_SIZE_BITS      (U_FRMR_SIZE * 8)

/*
 * Masks and IDs for the different frame formats. Bitwise AND *_MASK with
 * the first octet of the control field must result in *_ID.
 */
#define I_FRAME_MASK          0x80
#define S_FRAME_MASK          0xC0
#define UI_FRAME_MASK         0xE0
#define U_FRAME_MASK          0xE0

#define I_FRAME_ID            0x00
#define S_FRAME_ID            0x80
#define UI_FRAME_ID           0xC0
#define U_FRAME_ID            0xE0

/*
 * Number of octets that must be contained in a valid control field of a frame.
 */
#define I_CTRL_OCTETS         3
#define S_CTRL_OCTETS         2
#define UI_CTRL_OCTETS        2
#define U_CTRL_OCTETS         1

/*
 * Minimum number of octets that must be contained in a frame to access
 * the complete control field.
 */
#define CTRL_MIN_OCTETS       2
#define I_CTRL_MIN_OCTETS     4
#define S_CTRL_MIN_OCTETS     3
#define UI_CTRL_MIN_OCTETS    3
#define U_CTRL_MIN_OCTETS     2

/*
 * Minimum number of octets that must be contained in a valid frame, excluding
 * the FCS field.
 */
#define I_FRAME_MIN_OCTETS_WITHOUT_FCS  5
#define S_FRAME_MIN_OCTETS_WITHOUT_FCS  3
#define UI_FRAME_MIN_OCTETS_WITHOUT_FCS 3
#define U_FRAME_MIN_OCTETS_WITHOUT_FCS  2

/*
 * Minimum number of octets that must be contained in a valid frame.
 */
#define FRAME_MIN_OCTETS      5
#define I_FRAME_MIN_OCTETS    8
#define S_FRAME_MIN_OCTETS    6
#define UI_FRAME_MIN_OCTETS   6
#define U_FRAME_MIN_OCTETS    5

#define U_FRAME_FRMR_INFO_OCTETS  U_FRMR_INFO_SIZE

#define S_FRAME_SACK_MIN_CTRL_OCTETS  1
#define S_FRAME_SACK_MAX_CTRL_OCTETS  32

/*
 * I frame and S frame commands/responses (bits S1, S2).
 */
#define I_FRAME_RR            0x00
#define I_FRAME_ACK           0x01
#define I_FRAME_RNR           0x02
#define I_FRAME_SACK          0x03

/*
 * U frame commands/responses (bits M4, M3, M2, M1).
 */
#ifdef REL99 
#define U_FRAME_NULL          0x00
#endif /*REL99*/
#define U_FRAME_DM            0x01
#define U_FRAME_DISC          0x04
#define U_FRAME_UA            0x06
#define U_FRAME_SABM          0x07
#define U_FRAME_FRMR          0x08
#define U_FRAME_XID           0x0B

/*
 * Reasons of frame rejection condition. The lower nibble conforms
 * to W4-W1 in the FRMR response frame.
 */
#define FRMR_W1                         0x01
#define FRMR_W2                         0x02
#define FRMR_W3                         0x04
#define FRMR_W4                         0x08

/*
 * !!!!!asap <R.LLC.XCEPTION.A.010> "W1 bit: Bit W3 shall be set to 1 in
 * conjunction with this bit." Is that always the case?
 */
#define FRMR_INCORRECT_LENGTH           FRMR_W1
#define FRMR_INCORRECT_LENGTH_ABM       (FRMR_W1 | FRMR_W4)
#define FRMR_EXCEEDS_N201               FRMR_W2
#define FRMR_EXCEEDS_N201_ABM           (FRMR_W2 | FRMR_W4)
#define FRMR_UNDEFINED_CTRL             FRMR_W3
#define FRMR_UNDEFINED_CTRL_ABM         (FRMR_W3 | FRMR_W4)

#define FRMR_CTRL_LENGTH_UNKNOWN        0x00


/*
 * Maximum value of LLC sequence numbers and state variables:
 * N(S), N(R), V(S), V(R), ...
 */
#define MAX_SEQUENCE_NUMBER   511


/*
 * UITX_*_QUEUE_SIZE defines the maximum number of frames for the various
 * UITX service incarnations (depeding on SAPI). UITX is the only service
 * which is limited with uplink flow control. Enough memory must be present
 * for all other services (U, ITX). U requires a maximum of 2 simultaneously
 * stored frames per SAPI: the first being a received response to a
 * previously sent command, and the second being a command which is to be sent
 * directly afterwards.
 */
#define UITX_1_QUEUE_SIZE     10
#define UITX_3_QUEUE_SIZE     1
#define UITX_5_QUEUE_SIZE     1
#define UITX_7_QUEUE_SIZE     1
#define UITX_9_QUEUE_SIZE     1
#define UITX_11_QUEUE_SIZE    1

#define ITX_3_QUEUE_SIZE      1
#define ITX_5_QUEUE_SIZE      1
#define ITX_9_QUEUE_SIZE      1
#define ITX_11_QUEUE_SIZE     1

/*
 * Because of the lack of flow control between GRLC and LLC, it is possible,
 * that LLC recieve frames from GRLC, but can not forward them to the higher
 * layers. The following constants define the number of UI frames to buffer
 * per SAPI until we get an LL_GETUNITDATA_REQ
 */
#define UIRX_QUEUE_SIZE        32 

/*
 * The number of queued I-frames in ITX is limited to the windowsize ku.
 * If only ku frames are queued and we got an acknowledge for this, LLC
 * would have no more frames to send. The following variable defines a
 * number of additional frames to buffer.
 */
#ifdef _SIMULATION_
  #define ITX_ADD_QUEUE_SIZE     8
#else
  #define ITX_ADD_QUEUE_SIZE     1
#endif
/*
 * Maximum supported LLC parameter which should be negotiated initial.
 * Only values which are different to the default should be defined here.
 * In future this value should be read from the flash file system.
 */
#define N201_U_SUPPORTED       520
#define N201_I_SUPPORTED       520
#define KD_KD_SUPPORTED         64
#if defined (LL_2to1) || defined (REL99)  
#define KD_KU_SUPPORTED         16
#else
#define KD_KU_SUPPORTED         8
#endif
#define KD_MD_SUPPORTED          0
#define KD_MU_SUPPORTED          0

/*
 * Service definitions. Used to access service data with GET/SET_STATE.
 *
 * Services with multiple incarnation have to be defined as xxx->
 * Services with only one incarnation have to be defined as xxx.
 */
#define LLME                  llme.
#define SAPI                  sapi->
#define U                     u->
#define ITX                   itx->
#define IRX                   irx->
#define UITX                  uitx->
#define UIRX                  uirx->
#define T200                  t200->
#define T201                  t201->
#define TX                    tx.
#define RX                    rx.


/* #ifndef NTRACE*/

/*
 * These defintions are only needed for debugging purposes (!NTRACE).
 */

/*
 * The following definitions are necessary for the state initialisation of
 * services with the INIT_STATE() macro.
 */
#define U_0                   U
#define U_1                   U
#define U_2                   U
#define U_3                   U
#define U_4                   U
#define U_5                   U

#define ITX_0                 ITX
#define ITX_1                 ITX
#define ITX_2                 ITX
#define ITX_3                 ITX

#define IRX_0                 IRX
#define IRX_1                 IRX
#define IRX_2                 IRX
#define IRX_3                 IRX

#define UITX_0                UITX
#define UITX_1                UITX
#define UITX_2                UITX
#define UITX_3                UITX
#define UITX_4                UITX
#define UITX_5                UITX

#define UIRX_0                UIRX
#define UIRX_1                UIRX
#define UIRX_2                UIRX
#define UIRX_3                UIRX
#define UIRX_4                UIRX
#define UIRX_5                UIRX

#define T200_0                T200
#define T200_1                T200
#define T200_2                T200
#define T200_3                T200
#define T200_4                T200
#define T200_5                T200

#define T201_0                T201
#define T201_1                T201
#define T201_2                T201
#define T201_3                T201

/*
 * Service name definitions for trace purposes. The service abbrevation
 * (e.g. LLME) has to be the same as above for the service definitions.
 */
#define SERVICE_NAME_LLME     "LLME"

#define SERVICE_NAME_U_0      "U_1"
#define SERVICE_NAME_U_1      "U_3"
#define SERVICE_NAME_U_2      "U_5"
#define SERVICE_NAME_U_3      "U_7"
#define SERVICE_NAME_U_4      "U_9"
#define SERVICE_NAME_U_5      "U_11"

#define SERVICE_NAME_ITX_0    "ITX_3"
#define SERVICE_NAME_ITX_1    "ITX_5"
#define SERVICE_NAME_ITX_2    "ITX_9"
#define SERVICE_NAME_ITX_3    "ITX_11"

#define SERVICE_NAME_IRX_0    "IRX_3"
#define SERVICE_NAME_IRX_1    "IRX_5"
#define SERVICE_NAME_IRX_2    "IRX_9"
#define SERVICE_NAME_IRX_3    "IRX_11"

#define SERVICE_NAME_UITX_0   "UITX_1"
#define SERVICE_NAME_UITX_1   "UITX_3"
#define SERVICE_NAME_UITX_2   "UITX_5"
#define SERVICE_NAME_UITX_3   "UITX_7"
#define SERVICE_NAME_UITX_4   "UITX_9"
#define SERVICE_NAME_UITX_5   "UITX_11"

#define SERVICE_NAME_UIRX_0   "UIRX_1"
#define SERVICE_NAME_UIRX_1   "UIRX_3"
#define SERVICE_NAME_UIRX_2   "UIRX_5"
#define SERVICE_NAME_UIRX_3   "UIRX_7"
#define SERVICE_NAME_UIRX_4   "UIRX_9"
#define SERVICE_NAME_UIRX_5   "UIRX_11"

#define SERVICE_NAME_T200_0   "T200_1"
#define SERVICE_NAME_T200_1   "T200_3"
#define SERVICE_NAME_T200_2   "T200_5"
#define SERVICE_NAME_T200_3   "T200_7"
#define SERVICE_NAME_T200_4   "T200_9"
#define SERVICE_NAME_T200_5   "T200_11"

#define SERVICE_NAME_TX       "TX"
#define SERVICE_NAME_RX       "RX"

/* #endif !NTRACE */


/*
 * Timer definitions. For each timer a BASE and MAX value is defined.
 */
#define TIMER_T200_BASE       (0)
#define TIMER_T200_1          (TIMER_T200_BASE)
#define TIMER_T200_3          (TIMER_T200_BASE+1)
#define TIMER_T200_5          (TIMER_T200_BASE+2)
#define TIMER_T200_7          (TIMER_T200_BASE+3)
#define TIMER_T200_9          (TIMER_T200_BASE+4)
#define TIMER_T200_11         (TIMER_T200_BASE+5)
#define TIMER_T200_MAX        (TIMER_T200_11)

#define TIMER_T201_BASE       (TIMER_T200_MAX+1)
#define TIMER_T201_3          (TIMER_T201_BASE)
#define TIMER_T201_5          (TIMER_T201_BASE+1)
#define TIMER_T201_9          (TIMER_T201_BASE+2)
#define TIMER_T201_11         (TIMER_T201_BASE+3)
#define TIMER_T201_MAX        (TIMER_T201_11)

#define TIMER_MAX             (TIMER_T201_MAX)
#define TIMER_NUM             (TIMER_MAX+1)


/*
 * State definitions for each service.
 */

#define LLME_TLLI_UNASSIGNED            1
#define LLME_TLLI_ASSIGNED              2
#define LLME_TLLI_ASSIGNED_SUSPENDED    3

#define U_TLLI_UNASSIGNED               1
#define U_ADM                           2
#define U_LOCAL_ESTABLISHMENT           3
#define U_REMOTE_ESTABLISHMENT          4
#define U_ABM                           5
#define U_LOCAL_RELEASE                 6
#define U_ESTABLISH_RES_PENDING         7

#define ITX_TLLI_UNASSIGNED             1
#define ITX_TLLI_ASSIGNED               2
#define ITX_ABM                         3
#define ITX_ABM_PEER_BUSY               4

#define IRX_TLLI_UNASSIGNED             1
#define IRX_TLLI_ASSIGNED               2
#define IRX_ABM                         3
#define IRX_ABM_BUSY                    4

#define UITX_TLLI_UNASSIGNED_NOT_READY  1
#define UITX_TLLI_UNASSIGNED_READY      2
#define UITX_ADM_NOT_READY              3
#define UITX_ADM_READY                  4

#define UIRX_TLLI_UNASSIGNED_NOT_READY  1
#define UIRX_TLLI_UNASSIGNED_READY      2
#define UIRX_ADM_NOT_READY              3
#define UIRX_ADM_READY                  4

#define T200_TLLI_UNASSIGNED            1
#define T200_RESET                      2
#define T200_RUNNING                    3

#define TX_TLLI_UNASSIGNED_NOT_READY    1
#define TX_TLLI_UNASSIGNED_READY        2
#define TX_TLLI_ASSIGNED_NOT_READY      3
#define TX_TLLI_ASSIGNED_READY          4

#define RX_TLLI_UNASSIGNED              1
#define RX_TLLI_ASSIGNED                2

#ifndef TI_PS_OP_CIPH_DRIVER

/*
 * user defined constants
 */

#define CIPH_SIZE_CK                   (0x10)     
#define CIPH_MAX_ELEM                  (0x14)     
#define CIPH_MAX_HEADER_SIZE           (0x25)     
#define CIPH_N202                      (0x4)      

/*==== TYPES ======================================================*/

/*
 * The following definitions was originally generated by CCD. The definitions are not used as 
 * primitive definitions anymore, only for SNDCP internal usage.
 */
/* --------------------------------------------------------------------------------- */ 
/*
 * enum to Variable algo
 * Ciphering algorithm.
 * CCDGEN:WriteEnum_Count==2834
 */
#ifndef __T_CIPH_algo__
#define __T_CIPH_algo__
typedef enum
{
  CIPH_EA0                       = 0x0,           /* No encryption                  */
  CIPH_EA1                       = 0x1,           /* Frame shall be ciphered using algorithm EA1 */
  CIPH_EA2                       = 0x2,           /* Frame shall be ciphered using algorithm EA2 */
  CIPH_EA3                       = 0x3            /* Frame shall be ciphered using algorithm EA3 */
}T_CIPH_algo;
#endif
/*
 * enum to Variable direction
 * DIRECTION parameter
 * CCDGEN:WriteEnum_Count==2836
 */
#ifndef __T_CIPH_direction__
#define __T_CIPH_direction__
typedef enum
{
  CIPH_UPLINK_DIR                = 0x0,           /* See [3G 33.102]                */
  CIPH_DOWNLINK_DIR              = 0x1            /* See [3G 33.102]                */
}T_CIPH_direction;
#endif
/*
 * enum to Variable status
 * Status of the ciphering process
 * CCDGEN:WriteEnum_Count==2853
 */
#ifndef __T_CIPH_status__
#define __T_CIPH_status__
typedef enum
{
  CIPH_CIPH_FAIL                 = 0x0,           /* Ciphering successful           */
  CIPH_CIPH_PASS                 = 0x1,           /* Ciphering failed               */
  CIPH_CIPH_BUSY                 = 0x2,           /* Ongoing ciphering              */
  CIPH_FCS_ERROR                 = 0x3            /* FCS error                      */
}T_CIPH_status;
#endif
/*
 * enum to Variable pm
 * Protected or unprotected mode
 * CCDGEN:WriteEnum_Count==2840
 */
#ifndef __T_CIPH_pm__
#define __T_CIPH_pm__
typedef enum
{
  CIPH_PM_UNPROTECTED            = 0x0,           /* FCS covers frame header field and first N202 octets of information field */
  CIPH_PM_PROTECTED              = 0x1            /* FCS covers frame header and information fields */
}T_CIPH_pm;
#endif

#ifndef __T_CIPH_ck__
#define __T_CIPH_ck__
/*
 * Ciphering key.
 * CCDGEN:WriteStruct_Count==13956
 */
typedef struct
{
  U8                        ck_element[CIPH_SIZE_CK]; /*<  0: 16> T_CIPH_ck_element,  Ciphering key element          */
} T_CIPH_ck;
#endif

#ifndef __T_CIPH_init_cipher_req_parms__
#define __T_CIPH_init_cipher_req_parms__
/*
 * Ciphering algorithm initialisation parameters.
 * CCDGEN:WriteStruct_Count==13968
 */
typedef struct
{
  U8                        algo;                     /*<  0:  1> T_CIPH_algo,  Ciphering algorithm.                 */
  U8                        direction;                /*<  1:  1> T_CIPH_direction,  DIRECTION parameter             */
  U8                        zzz_align0;               /*<  2:  1> alignment                                          */
  U8                        zzz_align1;               /*<  3:  1> alignment                                          */
  T_CIPH_ck                 *ptr_ck;                  /*<  4:  4> pointer to Ciphering key.                          */
} T_CIPH_init_cipher_req_parms;
#endif


#ifndef __T_CIPH_gprs_parameters__
#define __T_CIPH_gprs_parameters__
/*
 * Structure containing LLC parameters
 * CCDGEN:WriteStruct_Count==13976
 */
typedef struct
{
  U8                        pm;                       /*<  0:  1> T_CIPH_pm,  Protected or unprotected mode          */
  U8                        zzz_align0;               /*<  1:  1> alignment                                          */
  U16                       header_size;              /*<  2:  2> T_CIPH_header_size,  Frame header size             */
  U32                       ciphering_input;          /*<  4:  4> frame-dependent ciphering input                    */
  U32                       threshold;                /*<  8:  4> T_CIPH_threshold,  Threshold parameter             */
} T_CIPH_gprs_parameters;
#endif

#ifndef __T_CIPH_umts_parameters__
#define __T_CIPH_umts_parameters__
/*
 * Structure containing umts parameters
 * CCDGEN:WriteStruct_Count==13980
 */
typedef struct
{
  U8                        bearer;                   /*<  0:  1> T_CIPH_bearer,  BEARER parameter                   */
  U8                        zzz_align0;               /*<  1:  1> alignment                                          */
  U8                        zzz_align1;               /*<  2:  1> alignment                                          */
  U8                        zzz_align2;               /*<  3:  1> alignment                                          */
  U32                       count_c;                  /*<  4:  4> COUNT-C parameter                                  */
  U8                        dest_bit_offset;          /*<  8:  1> T_CIPH_dest_bit_offset,  Bit offset                */
  U8                        org_bit_offset;           /*<  9:  1> T_CIPH_org_bit_offset,  Bit offset                 */
  U8                        zzz_align3;               /*< 10:  1> alignment                                          */
  U8                        zzz_align4;               /*< 11:  1> alignment                                          */
  U32                       threshold;                /*< 12:  4> T_CIPH_threshold,  Threshold parameter             */
} T_CIPH_umts_parameters;
#endif

#ifndef __T_CIPH_in_data__
#define __T_CIPH_in_data__
/*
 * Structure containing length and pointer to the data
 * CCDGEN:WriteStruct_Count==13984
 */
typedef struct
{
  U16                       len;                      /*<  0:  2> Length of buffer                                   */
  U8                        zzz_align0;               /*<  2:  1> alignment                                          */
  U8                        zzz_align1;               /*<  3:  1> alignment                                          */
  U32                       buf;                      /*<  4:  4> Pointer to the data in buffer                      */
} T_CIPH_in_data;
#endif

/*
 * Stucture containing length and pointer to the data
 * CCDGEN:WriteStruct_Count==13988
 */
#ifndef __T_CIPH_out_data__
#define __T_CIPH_out_data__
typedef T_CIPH_in_data T_CIPH_out_data;
#endif
#ifndef __T_CIPH_cipher_req_parms__
#define __T_CIPH_cipher_req_parms__
/*
 * Ciphering algorithm parameters.
 * CCDGEN:WriteStruct_Count==13990
 */
typedef union
{
  T_CIPH_gprs_parameters    gprs_parameters;          /*<  0: 12> Structure containing LLC parameters                */
  T_CIPH_umts_parameters    umts_parameters;          /*<  0: 16> Structure containing umts parameters               */
} T_CIPH_cipher_req_parms;
#endif

#ifndef __T_CIPH_in_data_list__
#define __T_CIPH_in_data_list__
/*
 * Structure containing a list of in_data elements
 * CCDGEN:WriteStruct_Count==13994
 */
typedef struct
{
  T_CIPH_in_data            *ptr_in_data;             /*<  0:  4> pointer to Structure containing length and pointer to the data */
  U16                       c_in_data;                /*<  4:  2> Number of elements                                 */
  U8                        zzz_align0;               /*<  6:  1> alignment                                          */
  U8                        zzz_align1;               /*<  7:  1> alignment                                          */
} T_CIPH_in_data_list;
#endif

#endif /* TI_PS_OP_CIPH_DRIVER */

/* --------------------------------------------------------------------------------- */

#ifndef __T_kc__
#define __T_kc__
typedef struct
{
  U8                        key[8];                   /*<  0:  8> ciphering key content                              */
} T_kc;
#endif

#ifndef __T_CCI_CIPHER_REQ__
#define __T_CCI_CIPHER_REQ__
typedef struct
{
  U8                        pm;                       /*<  0:  1> protected mode                                     */
  U8                        ciphering_algorithm;      /*< 16:  1> ciphering algorithm                                */
  U8                        direction;                /*< 17:  1> ciphering transfer direction                       */
  U8                        attached_counter;         /*< 29:  1> attached to primitive data counter                 */
  U32                       ciphering_input;          /*<  4:  4> frame-dependent ciphering input                    */
  T_kc                      kc;                       /*<  8:  8> ciphering key                                      */
  U32                       reference1;               /*< 32:  4> frame reference number                             */
  U16                       header_size;              /*< 36:  2> frame header size                                  */
  U8                        zzz_align0;               /*< 38:  1> alignment                                          */
  U8                        zzz_align1;               /*< 39:  1> alignment                                          */
  T_sdu                     sdu;                      /*< 40: ? > service data unit                                  */
} T_CCI_CIPHER_REQ;
#endif

#ifndef __T_CCI_DECIPHER_REQ__
#define __T_CCI_DECIPHER_REQ__
typedef struct
{
  U8                        pm;                       /*<  0:  1> protected mode                                     */
  U8                        ciphering_algorithm;      /*<  1:  1> ciphering algorithm                                */
  U8                        direction;                /*< 16:  1> ciphering transfer direction                       */
  U8                        zzz_align0;               /*< 30:  1> alignment                                          */
  T_kc                      kc;                       /*<  4:  8> ciphering key                                      */
  U32                       ciphering_input;          /*< 12:  4> frame-dependent ciphering input                    */
  U32                       reference1;               /*< 20:  4> frame reference number                             */
  U32                       reference2;               /*< 24:  4> second frame reference number                      */
  U16                       header_size;              /*< 28:  2> frame header size                                  */
  U8                        zzz_align1;               /*< 30:  1> alignment                                          */
  U8                        zzz_align2;               /*< 31:  1> alignment                                          */
  T_desc_list               desc_list;                /*< 32: ? > list of generic data descriptors                   */
} T_CCI_DECIPHER_REQ;
#endif

#ifndef __T_CCI_RESET_REQ__
#define __T_CCI_RESET_REQ__
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_CCI_RESET_REQ;
#endif

#ifndef __T_CCI_CIPHER_DESC_REQ__
#define __T_CCI_CIPHER_DESC_REQ__
typedef struct
{
  U8                        pm;                       /*<  0:  1> protected mode                                     */
  U8                        ciphering_algorithm;      /*< 16:  1> ciphering algorithm                                */
  U8                        direction;                /*< 17:  1> ciphering transfer direction                       */
  U8                        attached_counter;         /*< 29:  1> attached to primitive data counter                 */
  U32                       ciphering_input;          /*<  4:  4> frame-dependent ciphering input                    */
  T_kc                      kc;                       /*<  8:  8> ciphering key                                      */
  U32                       reference1;               /*< 32:  4> frame reference number                             */
  U16                       header_size;              /*< 36:  2> frame header size                                  */
  U8                        zzz_align0;               /*< 38:  1> alignment                                          */
  U8                        zzz_align1;               /*< 39:  1> alignment                                          */
  T_desc_list3              desc_list3;               /*< 40:  8> List of generic data descriptors                   */
} T_CCI_CIPHER_DESC_REQ;
#endif

/* --------------------------------------------------------------------------------- */

/*
 * LLC global typedefs
 */

typedef UBYTE       T_BIT_INT;
typedef T_BIT_INT   T_BIT;

/*
 * T_FRAME_NUM must be unsigned!
 */
typedef USHORT      T_FRAME_NUM;

/*
 * FRAME_NUM_VALID(a, b, c) equal to ETSI: a <= b <= c
 */
#define FRAME_NUM_VALID(a,b,c)  ( (((T_FRAME_NUM)((b)-(a)))%(MAX_SEQUENCE_NUMBER+1)) <=    \
                                  (((T_FRAME_NUM)((c)-(a)))%(MAX_SEQUENCE_NUMBER+1)) )

/*
 * FRAME_WIN_VALID(a, b, k)  equal to ETSI: a < b + k
 */
#define FRAME_WIN_VALID(a,b,k)  ( (((T_FRAME_NUM)((a)-(b)))%(MAX_SEQUENCE_NUMBER+1)) < (k) )

/*
 * FRAME_NUM_DISTANCE(a,b)  equal to b - a for frame nums
 */
#define FRAME_NUM_DISTANCE(a,b)  ((T_FRAME_NUM)((b)-(a))%(MAX_SEQUENCE_NUMBER+1))


typedef struct
{
  UBYTE data[S_FRAME_SACK_MAX_CTRL_OCTETS];
} T_SACK_BITMAP;


typedef enum
{
  ABIT_NO_REQ,
  ABIT_SET_REQ
} T_ABIT_REQ_TYPE;

typedef enum
{
  IQ_NO_FRAME,      /* return code only */
  IQ_NEW_FRAME,     /* new in the queue */
  IQ_RETR_FRAME,    /* ITX - marked for retransmission */
  IQ_W4ACK_FRAME,   /* ITX - is send, but no ack up to now */
  IQ_IS_ACK_FRAME   /* ITX - is ack and waiting for to be next in sequence */
} T_IQ_STATUS;

typedef enum
{
  NS_EQUAL_VR,
  NS_NO_SEQUENCE_ERROR,
  NS_EQUAL_VR_PLUS_1,
  NS_IN_RANGE_VR_KU
} T_LAST_NS_TYPE;

typedef enum
{
  I_FRAME = I_FRAME_ID,
  S_FRAME = S_FRAME_ID,
  UI_FRAME = UI_FRAME_ID,
  U_FRAME = U_FRAME_ID
} T_PDU_TYPE;

typedef enum
{
  /*
   * The values are chosen to correspond with the Sx and Mx bits in the frame
   * control field. Thus these commands can be used to build the header of a
   * frame.
   * NOTE:
   * Some I and U frame commands have the same value.
   */
#ifdef REL99 
  U_NULL = U_FRAME_NULL,
#endif /* REL99 */
  I_RR = I_FRAME_RR,
  I_ACK = I_FRAME_ACK,
  I_RNR = I_FRAME_RNR,
  I_SACK = I_FRAME_SACK,
  U_SABM = U_FRAME_SABM,
  U_DISC = U_FRAME_DISC,
  U_UA = U_FRAME_UA,
  U_DM = U_FRAME_DM,
  U_FRMR = U_FRAME_FRMR,
  U_XID = U_FRAME_XID
} T_COMMAND;

typedef enum
{
  NO_SERVICE,
  SERVICE_LLME,
  SERVICE_ITX,
  SERVICE_IRX,
  SERVICE_UITX,
  SERVICE_UIRX,
  SERVICE_U,
  SERVICE_TX,
  SERVICE_RX,
  SERVICE_T200
} T_SERVICE;

typedef enum
{
  PRIM_DATA,
  PRIM_UNITDATA,
  PRIM_REMOVED,
  NO_PRIM
} T_PRIM_TYPE;

typedef enum
{
  EXPIRY_TIMED,
  EXPIRY_REQUESTED
} T_EXPIRY_MODE_TYPE;


/*
 * Don't use enum for SAPI, because that's larger in size than UBYTE.
 */
typedef UBYTE                 T_SAPI;

#define SAPI_1                LL_SAPI_1
#define SAPI_2                LL_SAPI_INVALID
#define SAPI_3                LL_SAPI_3
#define SAPI_4                LL_SAPI_INVALID
#define SAPI_5                LL_SAPI_5
#define SAPI_6                LL_SAPI_INVALID
#define SAPI_7                LL_SAPI_7
#define SAPI_8                LL_SAPI_INVALID
#define SAPI_9                LL_SAPI_9
#define SAPI_10               LL_SAPI_INVALID
#define SAPI_11               LL_SAPI_11
#define SAPI_12               LL_SAPI_INVALID
#define SAPI_13               LL_SAPI_INVALID
#define SAPI_14               LL_SAPI_INVALID
#define SAPI_15               LL_SAPI_INVALID

/*
 * Indicates which primitive to use after successful (re-)establishment:
 * LL_ESTABLISH_IND/CNF
 */
typedef UBYTE                 T_ESTABLISHMENT;

#define IND_ESTABLISHMENT     1
#define CNF_ESTABLISHMENT     2

/*
 * Local TX transmit queue.
 * NOTE:
 * The ph_* variables must exactly match the types of T_GRLC_DATA_REQ /
 * T_GRLC_UNITDATA_REQ.
 */
typedef struct T_TX_QUEUE_ELEMENT
{
  struct T_TX_QUEUE_ELEMENT   *next;              /* pointer to next element */
  ULONG                       primitive;          /* stored primitive */
  ULONG                       reference;          /* entry reference number */
  T_PRIM_TYPE                 prim_type;          /* primitive type */
  UBYTE                       ph_sapi;            /* primitive header: sapi */
  ULONG                       ph_tlli;            /*   tlli */
  UBYTE                       ph_grlc_qos_peak;   /*   grlc_qos.peak */
  UBYTE                       ph_radio_prio;      /*   radio_prio */
  UBYTE                       ph_cause;           /*   cause */
  T_SERVICE                   rx_service;         /* flow control service */
  BOOL                        remove_frame;       /* TRUE: frame has to remove */
#ifdef REL99 
  UBYTE                       ph_pkt_flow_id;     /* packet flow identifier */
#endif /* REL99 */
} T_TX_QUEUE;


typedef struct T_ITX_I_QUEUE_ENTRY
{
  struct T_ITX_I_QUEUE_ENTRY  *next;              /* pointer to next element */
#ifdef LL_DESC
  T_LL_DESC_REQ*              frame;              /* pointer to L3 prim */
#else
  T_LL_DATA_REQ*              frame;              /* pointer to L3 prim */
#endif
  USHORT                      n_retr;             /* retransmission counter */
  T_IQ_STATUS                 status;             /* status flag */
  T_FRAME_NUM                 ns;                 /* send sequence number N(S) */
  ULONG                       oc_i_tx;            /* OC to use for this N(S) */

  /*
   * Copy of original LL_DATA_REQ parameter for attached_counter handling
   */
#ifdef LL_2to1
  T_PS_qos_r97                ll_qos;             /* quality of service  */
  T_LL_reference1             reference;          /* ciphering indicator     */
#else
  T_ll_qos                    ll_qos;             /* quality of service  */
  T_reference1                reference;          /* ciphering indicator     */
#endif
#ifdef REL99 
  UBYTE                       pkt_flow_id;         /*Packet Flow Identifier  */
#endif /* REL99 */
#ifdef LL_DESC
  USHORT                      offset;             /* Offset of buffer     */
  USHORT                      len;                /* Length of buffer      */
#else
  USHORT                      sdu_o_buf;          /* Offset of SDU data      */
  USHORT                      sdu_l_buf;          /* Length of SDU data      */
#endif
  UBYTE                       radio_prio;         /* Radio Priority          */
  UBYTE                       seg_pos;            /* First and/or last seg?  */

} T_ITX_I_QUEUE_ENTRY;


typedef struct
{
  struct T_ITX_I_QUEUE_ENTRY  *first;             /* pointer to first element */
  USHORT                      entries;            /* number of Queue enties */
} T_ITX_I_QUEUE;


typedef struct T_ITX_S_QUEUE_ENTRY
{
  struct T_ITX_S_QUEUE_ENTRY  *next;              /* pointer to next element */
  T_COMMAND                   sx;                 /* Supervisiory command Sx */
  T_ABIT_REQ_TYPE             rt;                 /* A bit request flag */
  T_FRAME_NUM                 nr;                 /* N(R) of command */
  T_SACK_BITMAP               bitmap;             /* SACK bitmap */
} T_ITX_S_QUEUE_ENTRY;


typedef struct T_IRX_QUEUE
{
  struct T_IRX_QUEUE          *next;              /* pointer to next element */
  T_LL_UNITDATA_IND*          frame;              /* pointer to L3 prim */
  T_FRAME_NUM                 ns;                 /* received frame number N(S) */
} T_IRX_QUEUE;

typedef struct T_UIRX_QUEUE
{
  struct T_UIRX_QUEUE          *next;             /* pointer to next element */
  T_LL_UNITDATA_IND*          frame;              /* pointer to L3 prim */
} T_UIRX_QUEUE;


typedef struct /* T_XID_PARAMETERS */
{
  struct /* Version */
  {
    BOOL                      valid;
    UBYTE                     value;
  } version;
  struct /* IOV-UI */
  {
    BOOL                      valid;
    ULONG                     value;
  } iov_ui;
  struct /* IOV-I */
  {
    BOOL                      valid;
    ULONG                     value;
  } iov_i;
  struct /* T200 */
  {
    BOOL                      valid;
    USHORT                    value;
  } t200;
  struct /* N200 */
  {
    BOOL                      valid;
    UBYTE                     value;
  } n200;
  struct /* N201-U */
  {
    BOOL                      valid;
    USHORT                    value;
  } n201_u;
  struct /* N201-I */
  {
    BOOL                      valid;
    USHORT                    value;
  } n201_i;
  struct /* mD */
  {
    BOOL                      valid;
    USHORT                    value;
  } md;
  struct /* mU */
  {
    BOOL                      valid;
    USHORT                    value;
  } mu;
  struct /* kD */
  {
    BOOL                      valid;
    UBYTE                     value;
  } kd;
  struct /* kU */
  {
    BOOL                      valid;
    UBYTE                     value;
  } ku;
  struct /* Reset */
  {
    BOOL                      valid;
  } reset;
} T_XID_PARAMETERS;



typedef struct /* T_FFS_XID_PARAMETERS */
{
  struct /* T200 */
  {
    BOOL                      valid;
    USHORT                    value;
  } t200[UNACKNOWLEDGED_INC];
  struct /* N200 */
  {
    BOOL                      valid;
    UBYTE                     value;
  } n200[UNACKNOWLEDGED_INC];
  struct /* N201-U */
  {
    BOOL                      valid;
    USHORT                    value;
  } n201_u[UNACKNOWLEDGED_INC];
  struct /* N201-I */
  {
    BOOL                      valid;
    USHORT                    value;
  } n201_i[ACKNOWLEDGED_INC];
  struct /* mD */
  {
    BOOL                      valid;
    USHORT                    value;
  } md[ACKNOWLEDGED_INC];
  struct /* mU */
  {
    BOOL                      valid;
    USHORT                    value;
  } mu[ACKNOWLEDGED_INC];
  struct /* kD */
  {
    BOOL                      valid;
    UBYTE                     value;
  } kd[ACKNOWLEDGED_INC];
  struct /* kU */
  {
    BOOL                      valid;
    UBYTE                     value;
  } ku[ACKNOWLEDGED_INC];
} T_FFS_XID_PARAMETERS;


typedef struct /* T_LLME_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_LLME_DATA;


typedef struct /* T_U_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
  /*
   * TLLI of current transaction (is set with incoming primitives, time-outs,
   * and signals with primitive payload).
   */
  ULONG                       current_tlli;

  /*
   * Number of retransmissions per sent command frame.
   */
  UBYTE                       retransmission_counter;

  /*
   * If LLC receives an release request from Layer 3 and is not able to confirm
   * the request immediately, release_requested has to be set to TRUE.
   */
  UBYTE                       release_requested;

  /*
   * Send LL_ESTABLISH_IND/CNF after successful establishment.
   */
  T_ESTABLISHMENT             ind_cnf_establishment;

  /*
   * Indicates if XID command has been sent.
   */
  BOOL                        xid_pending;

  /*
   * Indicates that an LL_XID_IND has been sent.
   */
  BOOL                        ll_xid_resp_pending;

  /*
   * Reason of frame rejection condition.
   */
  UBYTE                       frmr_reason;

  /*
   * Used to tag parameters for the next XID response. This is done by
   * setting the corresponding bit (with the definitions from llc_uf.h) to 1.
   * This variable is initialised by llme_init(), and with every call of
   * u_eval_xid().
   */
  ULONG                       xid_tag;

  /*
   * Tags parameters for further negotiation that have not been included in
   * the XID command but are included in the XID response. These parameters
   * have to be included in every XID response until the parameter has been
   * explicitly negotiated, either by responding to an XID command that
   * included the parameter, or by explicitly including the parameter
   * the next time an XID command is transmitted.
   * <R.LLC.XIDNEG_R.A.015>
   * This variable is initialised by llme_init().
   */
  ULONG                       xid_tag_negotiate;

  /*
   * Used to tag parameters that have actually been sent in the last command
   * or response frame. It is used for collision checks and the like.
   * This variable is initialised by llme_init(), and with every call of
   * u_insert_xid().
   */
  ULONG                       xid_tag_sent;

  /*
   * Structure to contain XID parameters that are requested to be negotiated.
   * Valid flags are initialised by llme_init(), and are being reset when
   * the parameter has explicitly been included in a response.
   */
  T_XID_PARAMETERS            requested_xid;

} T_U_DATA;


typedef struct /* T_ITX_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif

  BOOL                        tx_waiting;
  BOOL                        buffer_was_full;

  /*
   * I frame buffer size variables: Total length of
   * transmitted and not acknowlegded information fields.
   */
  ULONG                       B_tx;

  /*
   * Local transmit queues for I- and S frames
   */
  T_ITX_I_QUEUE               i_queue;
  T_ITX_S_QUEUE_ENTRY         *s_queue;

  /*
   * T201 Data
   */
  T_ITX_I_QUEUE_ENTRY         *t201_entry;
  USHORT                      n_pb_retr;

} T_ITX_DATA;


typedef struct /* T_IRX_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif

  /*
   * I frame buffer size variables: Total length of
   * stored information fields.
   */
  ULONG                       B_rx;

  /*
   * Next frame number to forward to L3
   */
  T_FRAME_NUM                 vf;

  /*
   * L3 ready to receive a data indication flag
   */
  BOOL                        ll_send_ready;
  T_LAST_NS_TYPE              last_ns;

  T_IRX_QUEUE                 *queue;
} T_IRX_DATA;


typedef struct /* T_UITX_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
  /*
   * Unacknowledged send sequence number
   */
  T_FRAME_NUM                 vu;
} T_UITX_DATA;


typedef struct /* T_UIRX_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
  /*
   * Bit field for received N(U) values, relative to V(UR).
   */
  ULONG                       last_32_frames_bitfield;

  T_UIRX_QUEUE                *queue;

} T_UIRX_DATA;


typedef struct /* T_T200_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
  /*
   * Length of timer T200 in milliseconds (is negotiated in 0.1s!).
   * LLC layer parameter.
   */
  T_TIME                      length;
  /*
   * The frame which is associated with the timer, the RLC/MAC cause of the
   * frame, and the service that started the timer. This is the t200_data
   * array of SDL.
   */
#ifdef LL_DESC
  T_LL_UNITDESC_REQ           *frame;
#else
  T_LL_UNITDATA_REQ           *frame;
#endif
  UBYTE                       cause;
  T_SERVICE                   originator;

} T_T200_DATA;


typedef struct /* T_TX_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
  /*
   * Local transmit queue (single-linked dynamic list).
   */
  T_TX_QUEUE                  *queue;
  /*
   * Counter of available space for UITX primitives in the queue.
   */
  UBYTE                       queue_counter_itx[ITX_NUM_INC];
  UBYTE                       queue_counter_uitx[UITX_NUM_INC];
} T_TX_DATA;


typedef struct /* T_RX_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_RX_DATA;


typedef struct /* T_SAPI_DATA */
{
  /*
   * Unacknowledged receive sequence numbers (0..511)
   */
  T_FRAME_NUM                 vur;
  T_FRAME_NUM                 last_vur;

  /*
   * Acknowledged mode frame numbers (0..511)
   */
  T_FRAME_NUM                 vr;
  T_FRAME_NUM                 va;
  T_FRAME_NUM                 vs;

  /*
   * Binary overflow counters for acknowledged/unacknowledged mode and
   * uplink/downlink. These variables are initialised with llme_init(),
   * and the unacknowledged OCs are reset when XID reset has been received.
   */
  ULONG                       oc_ui_tx;
  ULONG                       oc_ui_rx;
  ULONG                       oc_i_tx;
  ULONG                       oc_i_rx;

  /*
   * pbit_outstanding set to TRUE indicates that a frame with P bit set to 1
   * is currently outstanding
   */
  BOOL                        pbit_outstanding;

} T_SAPI_DATA;

typedef struct /* Layer 3 */
{
  BOOL                      valid;
  UBYTE                     length;
#ifdef LL_2to1
  UBYTE                     value[LL_MAX_L3_XID_LEN];
#else
  UBYTE                     value[LLC_MAX_L3_XID_LEN];
#endif
} T_L3_XID;

/*
 * CCI global typedefs
 */
typedef enum
{
  CIPHER_REQ,
  DECIPHER_REQ,
  NO_REQ
} T_CCI_PRIM_TYPE;

/* CCI data base */
typedef struct /*T_FBS_SERVICE */
{
  UBYTE                  state;
#ifndef NTRACE
  char                   *name;
  char                   *state_name;
#endif

  USHORT volatile        *cntl_reg;
  USHORT volatile        *status_reg;
  USHORT volatile        *status_irq_reg;
  USHORT volatile        *conf_ul_reg1;
  USHORT volatile        *conf_ul_reg2;
  USHORT volatile        *conf_ul_reg3;
  USHORT volatile        *conf_ul_reg4;
  USHORT volatile        *conf_ul_reg5;
  USHORT volatile        *conf_dl_reg1;
  USHORT volatile        *conf_dl_reg2;
  USHORT volatile        *conf_dl_reg3;
  USHORT volatile        *conf_dl_reg4;
  USHORT volatile        *conf_dl_reg5;
  USHORT volatile        *kc_reg1;
  USHORT volatile        *kc_reg2;
  USHORT volatile        *kc_reg3;
  USHORT volatile        *kc_reg4;
  USHORT volatile        *fcs_ul_reg1;
  USHORT volatile        *fcs_ul_reg2;
  USHORT volatile        *fcs_dl_reg1;
  USHORT volatile        *fcs_dl_reg2;
#if (BOARD == 61 OR BOARD == 71)	/* G-Sample or I-Sample */
  USHORT volatile        *switch_reg;
#endif
  USHORT volatile        *data16_reg;
  UBYTE  volatile        *data8_reg;

#ifdef _GEA_SIMULATION_
  UBYTE                  *simulated_reg;
  UBYTE                  *simulated_reg_buffer;
#endif

  /*primitives should also be defined as global */
#ifdef LL_DESC  
  T_CCI_CIPHER_DESC_REQ       *cci_cipher_req;
#else
  T_CCI_CIPHER_REQ       *cci_cipher_req;
#endif
  T_CCI_DECIPHER_REQ     *cci_decipher_req;

#ifndef TI_PS_OP_CIPH_DRIVER
  T_CIPH_init_cipher_req_parms ciph_params;
  BOOL initialized;
#endif
  ULONG                  cci_freed_partition;
  BOOL                   cci_info_trace;

} T_FBS_DATA;


typedef struct /* T_LLC_DATA */
{
  /*
   * Indicates LLC is suspended or not (default).
   * This variable is initialised by llme_init().
   */
  BOOL                        suspended;

  /*
   * Indicates LLC that GRLC is suspended or not (default).
   * This variable is initialised by llme_init().
   */
  BOOL                        grlc_suspended;

  /*
   * Current SAPI number. This is used for outgoing primitives. It is set
   * with each external event, like an incoming primitive or a time-out.
   */
  T_SAPI                      current_sapi;

  /*
   * TLLI New and TLLI Old. These are set by GMM via LLGMM_ASSIGN_REQ.
   * TLLI New is used for sending primitives (if the upper layer did not
   * already specify the TLLI). TLLI Old is currently unused.
   * Both variables are initialised by llme_init().
   */
  ULONG                       tlli_new;
  ULONG                       tlli_old;

  /*
   * LLC layer parameters (according to GSM 04.64, Version 6.4.0). These
   * parameters are initialised by llc_init_parameters().
   */
  UBYTE                       version;

  ULONG                       iov_ui;
  ULONG                       iov_i_base[ACKNOWLEDGED_INC];
  ULONG                       *iov_i;

  /*
   * T200 and T201 values are defined in the respective service data arrays.
   */
  UBYTE                       n200_base[UNACKNOWLEDGED_INC];
  UBYTE                       *n200;

  USHORT                      n201_u_base[UNACKNOWLEDGED_INC];
  USHORT                      n201_i_base[ACKNOWLEDGED_INC];
  USHORT                      *n201_u;
  USHORT                      *n201_i;

  USHORT                      md_base[ACKNOWLEDGED_INC];
  USHORT                      mu_base[ACKNOWLEDGED_INC];
  USHORT                      *md;
  USHORT                      *mu;

  UBYTE                       kd_base[ACKNOWLEDGED_INC];
  UBYTE                       ku_base[ACKNOWLEDGED_INC];
  UBYTE                       *kd;
  UBYTE                       *ku;

  T_L3_XID                    requested_l3_xid_base[ACKNOWLEDGED_INC];
  T_L3_XID                    *requested_l3_xid;

  /*
   * Structure to contain decoded XID parameters.
   * Valid flags are initialized by u_check_xid() each time received XID
   * is checked.
   */
  T_XID_PARAMETERS            decoded_xid;
  T_L3_XID                    decoded_l3_xid;

  /*
   * LLC layer variables needed for ciphering (see services TX/RX). These
   * variables are set with LLGMM_ASSIGN_REQ by GMM.
   */
#ifdef LL_2to1
  T_LLGMM_llgmm_kc            kc;
#else
  T_llgmm_kc                  kc;
#endif


  UBYTE                       ciphering_algorithm;

  /*
   * Service data structures.
   *
   * Services with multiple incarnations require an array of structures
   * named xxx_base[] with xxx = service abbrevation, and additionally a
   * pointer named *xxx, which will be accessed instead of xxx_base.
   *
   * Services with only one incarnation just have to declare one structure
   * named xxx (no need for xxx_base[] and *xxx).
   *
   * The differentiation between the two access possibilites is made with
   * the defines of the service names above (LLC_SERVICE_XXX).
   */
  T_LLME_DATA                 llme;

  T_SAPI_DATA                 sapi_base[MAX_SAPI_INC];
  T_SAPI_DATA                 *sapi;

  T_U_DATA                    u_base[U_NUM_INC];
  T_U_DATA                    *u;

  T_ITX_DATA                  itx_base[ITX_NUM_INC];
  T_IRX_DATA                  irx_base[IRX_NUM_INC];
  T_ITX_DATA                  *itx;
  T_IRX_DATA                  *irx;

  T_UITX_DATA                 uitx_base[UITX_NUM_INC];
  T_UIRX_DATA                 uirx_base[UIRX_NUM_INC];
  T_UITX_DATA                 *uitx;
  T_UIRX_DATA                 *uirx;

  T_T200_DATA                 t200_base[T200_NUM_INC];
  T_T200_DATA                 *t200;

  T_TX_DATA                   tx;
  T_RX_DATA                   rx;

  T_FBS_DATA                  fbs;

#ifdef _SIMULATION_
  /*
   * Structure to contain XID parameters that are configured in ffs.
   */
  T_FFS_XID_PARAMETERS        ffs_xid;
#endif

  /*Number of milliseconds to delay after sending reset_ind to SNDCP*/
  USHORT millis;
#ifdef REL99 
  U16                         cur_pfi;    /* current packet flow identifier   */
#endif
  T_ll_qos                    cur_qos;        /* quality of service R97           */
  U8                          cur_radio_prio; /* T_PS_radio_prio,  Radio priority */
} T_LLC_DATA;


/*==== EXPORT =====================================================*/

/*
 * Entity data base
 */
#ifdef LLC_PEI_C
       T_LLC_DATA llc_data_base, *llc_data;
#else
EXTERN T_LLC_DATA llc_data_base, *llc_data;
#endif

#define ENTITY_DATA           llc_data

/*
 * Communication handles (see also LLC_PEI.C)
 */
#define hCommGMM              llc_hCommGMM
#define hCommSNDCP            llc_hCommSNDCP
#ifdef LL_2to1
#define hCommMM               llc_hCommMM
#else
#define hCommGSMS             llc_hCommGSMS
#endif
#define hCommGRLC             llc_hCommGRLC

#ifdef LLC_PEI_C
       T_HANDLE hCommGMM        = VSI_ERROR;
       T_HANDLE hCommSNDCP      = VSI_ERROR;
#ifdef LL_2to1
       T_HANDLE hCommMM         = VSI_ERROR;
#else
       T_HANDLE hCommGSMS       = VSI_ERROR;
#endif
       T_HANDLE hCommGRLC       = VSI_ERROR;

       T_HANDLE LLC_handle;
/*
 * make the pei_create function unique
 */
#define pei_create              llc_pei_create

#else
EXTERN T_HANDLE hCommGMM;
EXTERN T_HANDLE hCommSNDCP;
#ifdef LL_2to1
EXTERN T_HANDLE hCommMM;
#else
EXTERN T_HANDLE hCommGSMS;
#endif
EXTERN T_HANDLE hCommGRLC;

EXTERN T_HANDLE LLC_handle;

#endif /* LLC_PEI_C */


#endif /* LLC_H */
