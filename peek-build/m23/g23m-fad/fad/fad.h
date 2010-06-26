/*
+-----------------------------------------------------------------------------
|  Project :  GSM-F&D (8411)
|  Modul   :  FAD
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
|             Fax Adaptation 3.45
+-----------------------------------------------------------------------------
*/


#ifndef FAD_H
#define FAD_H

#include "cl_ribu.h"

/*
 * Macros
 */

#undef  TRACE_FAD_DL_TCF

#define  TRACE_FAD_DL_STATUS
#define  TRACE_FAD_UL_STATUS

#ifndef _SIMULATION_        /* keeps testcases passing */
#define BMI_TCF_WORKAROUND
#endif

#ifndef NTRACE

/* for trace purposes */

#define SERVICE_NAME_KER    "KER"
#define SERVICE_NAME_RCV    "RCV"
#define SERVICE_NAME_RCVSUB "RCVSUB"
#define SERVICE_NAME_SND    "SND"
#define SERVICE_NAME_SNDSUB "SNDSUB"

#endif

#define KER     ker.
#define RCV     rcv.
#define RCVSUB  rcv.sub.
#define SND     snd.
#define SNDSUB  snd.sub.

#define SHARED_FAX_DATA_RAM

/*
 * instance management
 */

#define GET_INSTANCE(p) &fad_data_base[0]

/*
 * Constants
 */

/*
 * global unchangeable constants
 */
#define SBM_MAX_PRIMS   10  /* max numbers of prims in SBM   */
#define FRAME_SIZE       8  /* num of data bytes in fad frame */
#define MAX_NEW_FRAMES   9  /* number of 64 bit frames
                               which can be received
                               in one ra_data_ind primitive */

#define MAX_SDU_SIZE (MAX_NEW_FRAMES * FRAME_SIZE)

/*==== TYPES ======================================================*/

/*
 * Constants
 */

/*
 * Bitoffset for encoding/decoding
 */
#define ENCODE_OFFSET        0

/*
 * Dynamic Configuration Numbers
 */

/*
 * States of the entity process KERNEL
 */
#define KER_NULL             0
#define IDLE                 1
#define BCS_REC              2
#define BCS_TRA              3
#define MSG_REC_TCF          4
#define MSG_REC              5
#define MSG_TRA_TCF          6
#define MSG_TRA              7

/*
 * States of the entity process SND
 */
#define SYNC_SND             0
#define PREAM_SND            1
#define TCF_SND              2
#define TRAIN_SND            3
#define TRAIN_CNF_SND        4
#define BCS_SND              5
#define MSG_SND              6

/*
 * States of the entity process RCV
 */
#define SYNC_RCV             0
#define PREAM_RCV            1
#define TCF_RCV              2
#define TRAIN_RCV            3
#define BCS_RCV              4
#define MSG_RCV              5

/* substates */
#define SUB_IDLE             0
#define SUB_PREAMBLE         1
#define SUB_TCF              2
#define SUB_POST_TCF         3
#define SUB_DATA             4

/*
 * FAD Constants
 */

/* shared */

/* phase identifiers */
#define IDENT_BCS_REC   0x11
#define IDENT_MSG_REC   0x33
#define IDENT_MSG_TRA   0x44

/* Layer 1 TI frame */
#define L1_TI_LEN             40

/* RCV specific */

#ifdef _TARGET_
#define FAD_DESCBUF_LEN        8
#endif

#define BCS_TOLERANCE         70    /* 70% tolerance for correct BCS data */
#define TRAIN_TOLERANCE       50

/* SND specific */

/* Frame formats */

#define FR_SEQ_SYNC           "\x3E\x37\x50\x96\xC1\xC8\xAF\x69"
#define FR_SEQ_PREAMBLE       "\x11\x7E"    /* BCS-REC + preamble */
#define FR_SEQ_TRAIN_SND      "\x33\x0F"    /* MSG-REC + STATUS   */
#define FR_SEQ_TRAIN_RCV      "\x44\x0F"    /* MSG-TRA + STATUS   */
#define FR_SEQ_TCF            "\x00\x00\x00\x00\x00\x00\x00\x00"

#define VAL_TCF               0x00
#define HDLC_FLAG             0x7E
#define VAL_TRAIN             0x0F

/* length in bytes */
#define LEN_SEQ_SYNC          FRAME_SIZE
#define LEN_SEQ_PREAMBLE      2
#define LEN_SEQ_TCF           FRAME_SIZE
#define LEN_SEQ_TRAIN         2

#define SYNC_CYCLE_NONE       0
#define SYNC_CYCLE_7200       4
#define SYNC_CYCLE_12000      6

#define SYNC_RESET                  3

#define REPEAT_FOREVER        0xFFFF    /* bytes remaining - SYNC */

#define PREAMBLE_BYTES_TI_EXP   30      /* = 400 ms (give some extra tolerance to required 850 ms) */
#define PREAMBLE_BYTES_TI_SND   74      /* = 1 sec   */

#define TIME_AWAIT_TCF          75      /*   75 millisec. */
#define TIME_TCF_SND          1500      /* 1500 millisec. */
#define TIME_TCF_RCV          1350      /* 1500 millisec. - 10% */

#ifdef _SIMULATION_
#define TIME_THRESHOLD        5000      /* 5-sec. MSG buffer - T.32, §7.4 */
#else
#define TIME_THRESHOLD_TI     1000
#endif

#define MAX_SND_FRAMES        3
#define SND_FRAME_LEN         FRAME_SIZE   /* 11.01.99/DL: 24 -> 8 */
#define RCV_FRAME_LEN         24
#define HDLC_LEN              (RCV_FRAME_LEN * 12)
#define BCS_BUF_LEN           (SND_FRAME_LEN * MAX_SND_FRAMES * 16) /* 336 */

#ifdef _SIMULATION_
#define BCS_RATE              300
#define HDLC_REPEAT           8
#endif

/* frame types - FAD internal */
#define FT_PREAMBLE          0x06
#define FT_TRAIN_RESPONSE    0x07
#define FT_NONE              0x0F

/*==== TYPES ======================================================*/

/*
 * entity global data
 */

/*
 * send buffer management
 */
#define SBM_PRIMBUF_SIZE  (SBM_MAX_PRIMS+1)

typedef T_FAD_DATA_REQ  T_SBM_PRIM;
typedef UBYTE           T_SBM_PRIM_INDEX;

typedef struct
{
  T_SBM_PRIM *Prim;
} T_SBM_PRIMSLOT;

/*
 * receive buffer management
 */
#define RBM_PQ_MAX_PRIM   10
#define RBM_PQ_SIZE       (RBM_PQ_MAX_PRIM+1)

typedef T_FAD_DATA_IND    T_RBM_PRIM;

/*
 * data for send buffer management
 */

typedef struct
{
  USHORT           syncCycle;
  USHORT           framesUntilSync;
  USHORT           CurrPrimOff;
  USHORT           CurrPrimLen;
  USHORT           FrameSize;
  UBYTE            frames[MAX_SDU_SIZE];
  T_RIBU ribu;
  T_SBM_PRIMSLOT   Prim[SBM_PRIMBUF_SIZE];
  T_SBM_PRIM_INDEX PA;
} T_SBM;

/*
 * data for receive buffer management
 */

#define DL_RIBU_DEPTH 4

typedef struct
{
  BOOL          Initialised;
  USHORT        FramesPerPrim;
  USHORT        FramesInCurrPrim;
  T_RIBU_FD    *ribu;
  T_RBM_PRIM   *CurrPrim;
  T_RIBU        PQ;
  T_RBM_PRIM   *PQ_Array[RBM_PQ_SIZE];
} T_RBM_FAD;

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *name;
  char *state_name;
#endif
  BOOL forward_data;
} T_KER;

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *name;
  char *state_name;
#endif
} T_STATE;

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *name;
  char *state_name;
#endif
  T_STATE sub; /* substate */

  UBYTE   reset;
  BOOL    send_status;
  BOOL    send_ready_to_t30;
  BOOL    final;
  UBYTE   seq_buf [BCS_BUF_LEN];
  UBYTE   ul_FD_align[L1_TI_LEN];

  USHORT  ra_req_frames;
  USHORT  seq_buflen;
  USHORT  seq_bufpos;
  USHORT  bytes_to_send;
  USHORT  threshold;    /* T.32: 5-sec. buffer */
  USHORT  data_to_send;
  USHORT  data_bufpos;
  USHORT  frames_per_prim;
  USHORT  trans_rate;

  T_FD FD;
  U8 FD_buf[BCS_BUF_LEN];
  
  T_FAD_DATA_REQ *fad_data_req;
#ifdef _TARGET_
  T_RA_DATA_REQ ra_data_req;
#endif
} T_SND;

#define FLAGFOUND_MIN 5

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *name;
  char *state_name;
#endif
  T_STATE sub; /* substate */

  BOOL    train_flg;
  BOOL    prim_ready;
  BOOL    data_incoming;
  BOOL    t30_req_frames;
  BOOL    data_in_rbm;
  BOOL    preamble_pending;
  BOOL    bcs_pending;
  UBYTE   bcs_data_len;
  UBYTE   hdlc [HDLC_LEN];

  UBYTE   FlagFound;  /* HDLC flag in preamble found */
  UBYTE   AlignShift; /* shift of HDLC flag in received RA_DATA_IND */
  USHORT  AlignWord;

  USHORT  bytes_good;
  USHORT  bytes_expect;
  USHORT  bytes_rcvd;
  USHORT  hdlc_len;
  USHORT  data_bufpos;
} T_RCV;

typedef struct
{
  T_KER  ker;
  T_SND  snd;
  T_RCV  rcv;
  T_SBM  sbm;
  T_RBM_FAD  rbm;
} T_FAD_DATA;

/*
 *  SBM, RBM: constants, type defs
 */
#define ENTITY_DATA fad_data

/*==== EXPORT =====================================================*/
/*
 * instance data base
 */

#ifdef FAD_PEI_C
GLOBAL T_FAD_DATA *fad_data;
#else
EXTERN T_FAD_DATA *fad_data;
#endif

#ifdef SHARED_FAX_DATA_RAM

  /*lint -e526 : not defined */
  EXTERN UBYTE l2r_data_base[];
  EXTERN ULONG l2r_data_magic_num;

  #define fad_data_base ((T_FAD_DATA*)l2r_data_base)

  #define fad_data_magic_num l2r_data_magic_num

#else /* !SHARED_FAX_DATA_RAM */

  #ifdef FAD_PEI_C
  GLOBAL T_FAD_DATA fad_data_base[1];
  GLOBAL ULONG fad_data_magic_num;
  #else
  EXTERN T_FAD_DATA fad_data_base[];
  EXTERN ULONG fad_data_magic_num;
  #endif

#endif /* !SHARED_FAX_DATA_RAM */

#define FAD_DATA_MAGIC_NUM (('F'<<24) + ('A'<<16) + ('D'<<8))   /* "FAD",NUL */

/*
 *  Prototypes
 *
 *  FAD KERNEL
 *
 *  KERNEL primitive processing
 */

#ifdef USE_L1FD_FUNC_INTERFACE
  EXTERN void l1i_ra_detect_req(T_RA_DETECT_REQ *ra_detect_req);
#endif

#ifdef OPTION_MULTITHREAD
  #define ker_fad_activate_req   _ENTITY_PREFIXED(ker_fad_activate_req)
  #define ker_fad_deactivate_req _ENTITY_PREFIXED(ker_fad_deactivate_req)
  #define ker_fad_modify_req     _ENTITY_PREFIXED(ker_fad_modify_req)
  #define ker_fad_data_req       _ENTITY_PREFIXED(ker_fad_data_req)
  #define ker_fad_snd_tcf_req    _ENTITY_PREFIXED(ker_fad_snd_tcf_req)
  #define ker_fad_rcv_tcf_req    _ENTITY_PREFIXED(ker_fad_rcv_tcf_req)
  #define ker_fad_ready_req      _ENTITY_PREFIXED(ker_fad_ready_req)
  #define ker_fad_ignore_req     _ENTITY_PREFIXED(ker_fad_ignore_req)
#endif

EXTERN void ker_fad_activate_req   (T_FAD_ACTIVATE_REQ   *fad_activate_req);
EXTERN void ker_fad_deactivate_req (T_FAD_DEACTIVATE_REQ *fad_deactivate_req);
EXTERN void ker_fad_modify_req     (T_FAD_MODIFY_REQ     *fad_modify_req);
EXTERN void ker_fad_data_req       (T_FAD_DATA_REQ       *fad_data_req);
EXTERN void ker_fad_snd_tcf_req    (T_FAD_SND_TCF_REQ    *fad_snd_tcf_req);
EXTERN void ker_fad_rcv_tcf_req    (T_FAD_RCV_TCF_REQ    *fad_rcv_tcf_req);
EXTERN void ker_fad_ready_req      (T_FAD_READY_REQ      *fad_ready_req);
EXTERN void ker_fad_ignore_req     (T_FAD_IGNORE_REQ     *fad_ignore_req);

/*
 *  KERNEL signal processing
 */

/*
 *  KERNEL procedures
 */

#ifdef OPTION_MULTITHREAD
  #define ker_init           _ENTITY_PREFIXED(ker_init)
  #define ker_SetError       _ENTITY_PREFIXED(ker_SetError)
#endif

EXTERN void ker_init         (void);
EXTERN void ker_SetError     (USHORT cause);

/*
 *  FAD RCV
 *
 *  RCV primitive processing
 */

#ifdef OPTION_MULTITHREAD
  #define rcv_ra_data_ind         _ENTITY_PREFIXED(rcv_ra_data_ind)

#endif

EXTERN void rcv_ra_data_ind      (T_RA_DATA_IND *ra_data_ind);

/*
 *  RCV signal processing
 */
#ifdef OPTION_MULTITHREAD
/* KER -> RCV */
  #define sig_ker_rcv_await_tcf_req _ENTITY_PREFIXED(sig_ker_rcv_await_tcf_req)
  #define sig_ker_rcv_ready_req     _ENTITY_PREFIXED(sig_ker_rcv_ready_req)
  #define sig_ker_rcv_reset_req     _ENTITY_PREFIXED(sig_ker_rcv_reset_req)
/* RCV -> KER */
  #define sig_rcv_ker_preamble_ind  _ENTITY_PREFIXED(sig_rcv_ker_preamble_ind)
  #define sig_rcv_ker_tcf_ind       _ENTITY_PREFIXED(sig_rcv_ker_tcf_ind)
  #define sig_rcv_ker_train_ind     _ENTITY_PREFIXED(sig_rcv_ker_train_ind)
  #define sig_rcv_ker_train_end_ind _ENTITY_PREFIXED(sig_rcv_ker_train_end_ind)
  #define sig_rcv_ker_bcs_ind       _ENTITY_PREFIXED(sig_rcv_ker_bcs_ind)
  #define sig_rcv_ker_msg_ind       _ENTITY_PREFIXED(sig_rcv_ker_msg_ind)
  #define sig_rcv_ker_status_ind    _ENTITY_PREFIXED(sig_rcv_ker_status_ind)
  #define sig_rcv_ker_error_ind     _ENTITY_PREFIXED(sig_rcv_ker_error_ind)
#endif

EXTERN void sig_ker_rcv_await_tcf_req  (void);
EXTERN void sig_ker_rcv_ready_req      (void);
EXTERN void sig_ker_rcv_reset_req      (void);

EXTERN void sig_rcv_ker_preamble_ind   (void);
EXTERN void sig_rcv_ker_tcf_ind        (void);
EXTERN void sig_rcv_ker_train_ind      (void);
EXTERN void sig_rcv_ker_train_end_ind  (void);
EXTERN void sig_rcv_ker_bcs_ind        (UBYTE *hdlc, USHORT hdlc_len);
EXTERN void sig_rcv_ker_msg_ind        (T_FAD_DATA_IND *fad_data_ind);
EXTERN void sig_rcv_ker_status_ind     (void);
EXTERN void sig_rcv_ker_error_ind      (USHORT cause);

/*
 *  RCV procedures
 */

#ifdef OPTION_MULTITHREAD
  #define rcv_init                  _ENTITY_PREFIXED(rcv_init)
  #define rcv_ResetFormatter        _ENTITY_PREFIXED(rcv_ResetFormatter)
  #define rcv_FrameType             _ENTITY_PREFIXED(rcv_FrameType)
  #define rcv_SetHdlcFrame          _ENTITY_PREFIXED(rcv_SetHdlcFrame)
  #define rcv_SetErrorRatio         _ENTITY_PREFIXED(rcv_SetErrorRatio)
  #define rcv_GetMsgData            _ENTITY_PREFIXED(rcv_GetMsgData)
  #define rcv_SetFinalBuffer        _ENTITY_PREFIXED(rcv_SetFinalBuffer)
#endif

EXTERN void rcv_init                (void);
EXTERN void rcv_ResetFormatter      (void);
EXTERN UBYTE rcv_FrameType          (T_FD *pFD);
EXTERN void rcv_SetHdlcFrame        (T_FD *pFD, UBYTE *hdlc_data, USHORT *hdlc_len);
EXTERN void rcv_SetErrorRatio       (T_FD *pFD);

EXTERN BOOL rcv_StoreMsgData        (T_FD *pFD, BOOL *primAvail);

EXTERN BOOL rcv_GetMsgData          (T_FAD_DATA_IND **fad_data_ind, BOOL data_incoming);
EXTERN void rcv_SetFinalBuffer      (void);

#ifdef _TARGET_
EXTERN void l1i_ra_detect_req(T_RA_DETECT_REQ *ra_detect_req);
#endif

/*
 *  FAD SND
 *
 *  SND primitive processing
 */

#ifdef OPTION_MULTITHREAD
  #define snd_ra_ready_ind          _ENTITY_PREFIXED(snd_ra_ready_ind)
#endif

EXTERN void snd_ra_ready_ind  (T_RA_READY_IND *ra_ready_ind);

/*
 *  SND signal processing
 */
#ifdef OPTION_MULTITHREAD
/* KER -> SND */
  #define sig_ker_snd_activate_req      _ENTITY_PREFIXED(sig_ker_snd_activate_req)
  #define sig_ker_snd_sync_req         _ENTITY_PREFIXED(sig_ker_snd_sync_req)
  #define sig_ker_snd_train_req        _ENTITY_PREFIXED(sig_ker_snd_train_req)
  #define sig_ker_snd_tcf_req          _ENTITY_PREFIXED(sig_ker_snd_tcf_req)
  #define sig_ker_snd_preamble_req     _ENTITY_PREFIXED(sig_ker_snd_preamble_req)
  #define sig_ker_snd_bcs_req          _ENTITY_PREFIXED(sig_ker_snd_bcs_req)
  #define sig_ker_snd_status_req       _ENTITY_PREFIXED(sig_ker_snd_status_req)
/* SND -> KER */
  #define sig_snd_ker_ready_ind        _ENTITY_PREFIXED(sig_snd_ker_ready_ind)
  #define sig_snd_ker_data_sent_ind    _ENTITY_PREFIXED(sig_snd_ker_data_sent_ind)
#endif

EXTERN void sig_ker_snd_activate_req   (T_FAD_ACTIVATE_REQ *fad_activate_req);
EXTERN void sig_ker_snd_sync_req       (BOOL reset);
EXTERN void sig_ker_snd_train_req      (T_FAD_DATA_REQ *fad_data_req,
                                        BOOL send_fax);
EXTERN void sig_ker_snd_tcf_req        (void);
EXTERN void sig_ker_snd_preamble_req   (T_FAD_DATA_REQ *fad_data_req);
EXTERN void sig_ker_snd_bcs_req        (T_FAD_DATA_REQ *fad_data_req);
EXTERN void sig_ker_snd_status_req     (void);
EXTERN void sig_ker_snd_msg_req        (T_FAD_DATA_REQ *fad_data_req);

EXTERN void sig_snd_ker_ready_ind      (void);
EXTERN void sig_snd_ker_data_sent_ind  (void);

/*
 *  SND procedures
 */

#ifdef OPTION_MULTITHREAD
  #define snd_init                  _ENTITY_PREFIXED(snd_init)
  #define snd_SendReset             _ENTITY_PREFIXED(snd_SendReset)
  #define snd_SetSequence           _ENTITY_PREFIXED(snd_SetSequence)
  #define snd_SendSequence          _ENTITY_PREFIXED(snd_SendSequence)
  #define snd_DurationToBytes       _ENTITY_PREFIXED(snd_DurationToBytes)
  #define snd_BuildStatusFrames     _ENTITY_PREFIXED(snd_BuildStatusFrames)
  #define snd_StoreMsgData          _ENTITY_PREFIXED(snd_StoreMsgData)
  #define snd_SendMsgData           _ENTITY_PREFIXED(snd_SendMsgData)
  #define snd_SendBcsData           _ENTITY_PREFIXED(snd_SendBcsData)
#endif

EXTERN void snd_init                (void);
GLOBAL void snd_SendReset           (USHORT bytes_to_send);
EXTERN void snd_SetSequence         (UBYTE *seq_buf, USHORT *seq_buflen,
                                     UBYTE *sequence, USHORT seq_len,
                                     USHORT repeat);
EXTERN USHORT snd_SendSequence      (USHORT bytes_to_send, UBYTE fr_type);
EXTERN USHORT snd_DurationToBytes   (USHORT trans_rate, USHORT duration);

#ifdef _SIMULATION_
EXTERN void snd_BuildStatusFrames(T_FAD_DATA_REQ *fad_data_req, USHORT max_bytes);
#else
EXTERN void snd_BuildStatusFrames(T_FAD_DATA_REQ *fad_data_req);
#endif

EXTERN void snd_StoreMsgData        (T_FAD_DATA_REQ *fad_data_req);
EXTERN BOOL snd_SendMsgData         (UBYTE req_frames);

#ifdef _SIMULATION_
EXTERN void snd_SendBcsData (USHORT bytes_to_send);
#else
EXTERN void snd_SendBcsData (void);
#endif

/* SBM_RBM_BEGIN */
/*
 * Send Buffer Management SBM
 */
#ifdef OPTION_MULTITHREAD
  #define sbm_init                  _ENTITY_PREFIXED(sbm_init)
  #define sbm_store_prim            _ENTITY_PREFIXED(sbm_store_prim)
  #define sbm_get_frame             _ENTITY_PREFIXED(sbm_get_frame)
  #define sbm_free_empty_prims      _ENTITY_PREFIXED(sbm_free_empty_prims)
#endif

EXTERN void sbm_init       (USHORT frameSize);
EXTERN void sbm_store_prim (T_SBM_PRIM *sendPrim);
EXTERN BOOL sbm_get_frame  (T_FRAME_DESC *frameDesc, UBYTE reqFrames);
EXTERN void sbm_free_empty_prims (void);

/*
 * Receive Buffer Management RBM
 */

#ifdef OPTION_MULTITHREAD
  #define rbm_init            _ENTITY_PREFIXED(rbm_init           )
  #define rbm_deinit          _ENTITY_PREFIXED(rbm_deinit         )
  #define rbm_reset           _ENTITY_PREFIXED(rbm_reset          )
  #define rbm_store_frames    _ENTITY_PREFIXED(rbm_store_frames   )
  #define rbm_get_prim        _ENTITY_PREFIXED(rbm_get_prim       )
  #define rbm_get_curr_prim   _ENTITY_PREFIXED(rbm_get_curr_prim  )
#endif

EXTERN void rbm_init(USHORT framesPerPrim);
EXTERN void rbm_deinit(BOOL final);
EXTERN void rbm_reset(void);

EXTERN BOOL rbm_store_frames(T_FD *pFDw, BOOL *primIsReady);

EXTERN T_RBM_PRIM *rbm_get_prim(void);
EXTERN T_RBM_PRIM *rbm_get_curr_prim(void);

/*
 * Communication handles
 */
#ifdef OPTION_MULTITHREAD
  #define hCommT30  _ENTITY_PREFIXED(hCommT30)
#ifdef _SIMULATION_
  #define hCommRA   _ENTITY_PREFIXED(hCommRA)
#endif
#endif

#ifdef FAD_PEI_C

GLOBAL T_HANDLE fad_handle;              /* own handle        */
GLOBAL T_HANDLE hCommT30 = VSI_ERROR;    /* T30 Communication */
#ifdef _SIMULATION_
GLOBAL T_HANDLE hCommRA  = VSI_ERROR;    /* RA  Communication */
#endif

#else

EXTERN T_HANDLE  fad_handle;             /* own handle         */
EXTERN T_HANDLE  hCommT30;               /* T30  Communication */
#ifdef _SIMULATION_
EXTERN T_HANDLE  hCommRA;                /* RA   Communication */
#endif

#endif

GLOBAL void fad_exec_timeout(USHORT index);

#define BYTE2HEXSTR(B, H) {UBYTE b, *a = (UBYTE*)(H);\
        b = (((UBYTE)(B)) >> 4) & 0x0F;\
        if (b > 9) b += 'A'-10; else b += '0'; *a = b;\
        b = ((UBYTE)(B)) & 0x0F;\
        if (b > 9) b += 'A'-10; else b += '0'; *(a+1) = b;}

#endif
