/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  T30
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
|  Purpose :  Definitions for the Protocol Stack Entity T30
+----------------------------------------------------------------------------- 
*/ 


#ifndef T30_H
#define T30_H

#define SHARED_FAX_DATA_RAM

#define KER_DEBUG_BCS
#undef KER_DEBUG_MSG
#undef SW_FLOW_CTRL

/*
 * instance management
 */
#define GET_INSTANCE(p) &t30_data_base[0]
#define T30_INSTANCES 1

/*
 * Constants
 */

#define HDLC_FLAG  0x7E

/* 
 * number of HDLC_FLAGs between HDLC fields
 */
#ifdef _SIMULATION_
#define HDLC_FLAGS 1
#else
#define HDLC_FLAGS 3
#endif

#define HDLC_ADDR  0xFF

/****** Transparent Data Commands T.32 ******/

/* DLE transparency */

#define TDC_DLE 0x10 /* substitute one 0x10 pattern */
#define TDC_SUB 0x1a /* substitute two 0x10 patterns */

/* transmitter commands */

#define TDC_ETX 0x03 /* acknowledge <CAN> */
#define TDC_PRI 0x21 /* request Procedure Interrupt */
#define TDC_MPS 0x2c /* end of page, more to follow */
#define TDC_EOP 0x2e /* end of document and session */
#define TDC_EOM 0x3b /* end of document */
#define TDC_BCR 0x3f /* check Buffer Credit */

/* receiver commands */

#define TDC_SOH 0x41 /* <SOH> in packet data */
#define TDC_ETB 0x57 /* <ETB> in packet data */
#define TDC_OVR 0x4f /* overrun error marker */

#ifdef SW_FLOW_CTRL
#define TDC_DC1 0x11 /* XON */
#endif

#define TDC_DC2 0x12 /* receive buffer status: buffer is empty */
#ifdef SW_FLOW_CTRL
#define TDC_DC3 0x13 /* XOFF */
#endif

#define TDC_CAN 0x18 /* Requests DCE/DTE to stop delivering Phase C data */

#define MSG_SIZE_BITS 2880   /* value taken from former psa_t30.h */

/*
 * information for dti library
 */
#define T30_DTI_UP_INTERFACE      0
#define T30_DTI_UP_CHANNEL        0
#define T30_DTI_UP_DEF_INSTANCE   0
#define T30_DTI_UPLINK_QUEUE_SIZE 0

#define READY_UPLI_PAGE           1
#define READY_UPLI_SDU            2

#define BCS_FRM_FILLED            0
#define BCS_FRM_FILLING           1
#define BCS_FRM_FULL              2
#define BCS_FRM_SIZE              300
#define BCS_FRM_SIZE_BITS         (BCS_FRM_SIZE << 3)

#define CCD_OK                    0
#define CCD_ERR                   1

#define CHK_OK                    0
#define CHK_FCS_ERR               1
#define CHK_FIN_ERR               2

#define CTL_C0                    0xC0
#define CTL_C8                    0xC8

#define ERR_BUF_FULL              1
#define ERR_CCD_DEC               2
/*
#define ERR_CCD_ENC               3
*/
#define ERR_FCS                   4
#define ERR_FINAL                 5
#define ERR_FRAME_NO_FLAG         6
#define ERR_FRAME_TOO_MANY_FRAMES 7

#define FINAL_NO                  0
#define FINAL_YES                 1

#define FMOD_IDLE                 0
#define FMOD_SND                  1
#define FMOD_RCV                  2
#define FMOD_POLL                 3

#define FRAMES_MAX                10

#define FRM_OK                    0
#define FRM_ERR_NO_FLAG           1
#define FRM_ERR_TOO_MANY_FRAMES   2

#define REPORT_SIZE               128
#define REPORT_SIZE_BITS          (REPORT_SIZE << 3)

#define RTC_NO                    0
#define RTC_YES                   1

/*
 * Bitoffset for encoding/decoding
 */
#define ENCODE_OFFSET             0

#define T30_HEADER_LEN_BYT        2          /* Address byte and control byte */
#define T30_TRAILER_LEN_BYT       2          /* FCS */

#define BIT_STREAM_LEN_BYT        ((MAX_BITSTREAM_LEN_T30 + 7) / 8)
                                  /* No. of bytes, rounded to byte boundary */
#define HDLC_BUF_SIZE              (2 * sizeof(USHORT) + T30_HEADER_LEN_BYT + BIT_STREAM_LEN_BYT + T30_TRAILER_LEN_BYT)
                                  /* SDU header + T30 Header + HDLC Info + Trailer*/
#define HDLC_BUF_SIZE_LNG          ((HDLC_BUF_SIZE + sizeof (ULONG) - 1) / sizeof (ULONG))
                                  /* No. of ULONGs, rounded to long boundary */

/*
 * Timer
 */
#define T1 0      /* Timer T1 */
#define T2 1      /* Timer T2 */
#define T4 2      /* Timer T4 */

/*
 * Dynamic Configuration Numbers
 */
#define TIMER_SET        1
#define TIMER_RESET      2
#define TIMER_SPEED_UP   3
#define TIMER_SLOW_DOWN  4
#define TIMER_SUPPRESS   5

/*
 * State index for the entity processes
 */
#define KER        ker.
#define MUX        mux.

/*
 * States of the entity processes
 *
 * Process KER
 */
enum
{
  T30_NULL,
  T30_DCN,
  T30_IDLE,
  T30_RCV_CFR,
  T30_RCV_DAT,
  T30_RCV_DATW,
  T30_RCV_DCN,
  T30_RCV_DCS,
  T30_RCV_DIS,
  T30_RCV_MCF,
  T30_RCV_MSG,
  T30_RCV_MSGW,
  T30_RCV_PI,
  T30_RCV_PST,
  T30_RCV_PSTW,
  T30_RCV_RDY,
  T30_RCV_RDYF,
  T30_RCV_RDYW,
  T30_RCV_RT,
  T30_RCV_RT2,
  T30_RCV_T2,
  T30_RCV_TCF,
  T30_SND_CAP,
  T30_SND_CFR,
  T30_SND_DAT,
  T30_SND_DATF,
  T30_SND_DCN,
  T30_SND_DCN1,
  T30_SND_DCS,
  T30_SND_DTC,
  T30_SND_MCF,
  T30_SND_MSG,
  T30_SND_PI,
  T30_SND_PRI,
  T30_SND_PST,
  T30_SND_PSTW,
  T30_SND_RDY,
  T30_SND_RDYW,
  T30_SND_SGN,
  T30_SND_TCF,
  T30_ACTIVATE,
  T30_DEACTIVATE
};

/*
 * Process MUX
 */
#define T30_MUX_OFF               0
#define T30_MUX_BCS               1
#define T30_MUX_MSG               2

/*==== TYPES ======================================================*/

/*
 * entity global data
 */
typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *state_name;
#endif
} T_STATE;

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *state_name;
#endif
  UBYTE mode;
} T_MUX;

typedef struct
{
  USHORT beg, end, len;
} T_T30_FRAME;

typedef struct
{
  T_STATE ker;
  T_MUX mux;
  UBYTE  dti_state; /* state variable for DTI library */
  UBYTE  threshold;
  BOOL   rate_modified;
  UBYTE  repeat;
  UBYTE  sgn_ind;
  UBYTE  sgn_req;
  UBYTE  res;
  UBYTE  prev;
  UBYTE  fmod;
  UBYTE  hdlc_report;
  UBYTE  dir;
  UBYTE  data_cnf;
  UBYTE  preamble_ind;
  UBYTE  bitorder;
  UBYTE  stuff_bits;
  UBYTE  stuff_ones;
  BOOL   TDC_doli_pending;
  BOOL   TDC_upli_pending;
  BOOL   dti_data_ind_final;

  USHORT rtc_zero;
  USHORT  rtc_eol;
  USHORT eol;
  USHORT trans_rate;
  USHORT frames_per_prim;

  T_T30_FRAME frm[FRAMES_MAX];
    
  ULONG  bcs_frm[(BCS_FRM_SIZE / sizeof (ULONG)) + 1];
  ULONG  BCI_stream[HDLC_BUF_SIZE_LNG];

  ULONG link_id;

  T_hdlc_info    hdlc_rcv;
  T_hdlc_info    hdlc_snd;

  T_FAD_DATA_REQ *fad_data_req;

  T_DTI2_DATA_IND *dti_data_ind;
  T_DTI2_DATA_IND *dti_data_ind_empty;

  UBYTE   *dti_data_req_ptr;
  T_desc2 *dti_data_req_desc;

#ifdef _SIMULATION_ /* test_mode */
  USHORT  test_mode;
#endif
} T_T30_DATA;

/*==== EXPORT =====================================================*/

#define T1_INDEX  0
#define T2_INDEX  1
#define T4_INDEX  2

/*
 * instance data base
 */
#ifdef T30_PEI_C
GLOBAL T_T30_DATA *t30_data;
#else
EXTERN T_T30_DATA *t30_data;
#endif

#ifdef SHARED_FAX_DATA_RAM

  /*lint -e526 : not defined */
  EXTERN UBYTE rlp_data_base[];
  #define t30_data_base ((T_T30_DATA*)rlp_data_base)

  EXTERN ULONG rlp_data_magic_num;
  #define t30_data_magic_num rlp_data_magic_num

#else

  #ifdef T30_PEI_C
  GLOBAL T_T30_DATA t30_data_base[T30_INSTANCES];
  GLOBAL ULONG t30_data_magic_num;
  #else
  EXTERN T_T30_DATA t30_data_base[];
  EXTERN ULONG t30_data_magic_num;
  #endif

#endif /* !SHARED_FAX_DATA_RAM */

#define T30_DATA_MAGIC_NUM (('T'<<24) + ('3'<<16) + ('0'<<8))   /* "T30",NUL */

#define ENTITY_DATA t30_data

/*---------------------------------------------------------------------
 *  Prototypes
 *
 *  T30 KERNEL
 *
 *  KERNEL primitive processing
 */

EXTERN void ker_fad_activate_cnf    (T_FAD_ACTIVATE_CNF   *fad_activate_cnf  );
EXTERN void ker_fad_data_cnf        (T_FAD_DATA_CNF       *fad_data_cnf      );
EXTERN void ker_fad_deactivate_cnf  (T_FAD_DEACTIVATE_CNF *fad_deactivate_cnf);
EXTERN void ker_fad_error_ind       (T_FAD_ERROR_IND      *fad_error_ind     );
EXTERN void ker_fad_rcv_tcf_cnf     (T_FAD_RCV_TCF_CNF    *fad_rcv_tcf_cnf   );
EXTERN void ker_fad_ready_ind       (T_FAD_READY_IND      *fad_ready_ind     );
EXTERN void ker_fad_snd_tcf_cnf     (T_FAD_SND_TCF_CNF    *fad_snd_tcf_cnf   );

EXTERN void ker_t30_activate_req    (T_T30_ACTIVATE_REQ   *t30_activate_req  );
EXTERN void ker_t30_cap_req         (T_T30_CAP_REQ        *t30_cap_req       );
EXTERN void ker_t30_config_req      (T_T30_CONFIG_REQ     *t30_config_req    );
EXTERN void ker_t30_deactivate_req  (T_T30_DEACTIVATE_REQ *t30_deactivate_req);
EXTERN void ker_t30_modify_req      (T_T30_MODIFY_REQ     *t30_modify_req    );
EXTERN void ker_t30_sgn_req         (T_T30_SGN_REQ        *t30_sgn_req       );
EXTERN void ker_t30_dti_req         (T_T30_DTI_REQ        *t30_dti_req       );

#ifdef OPTION_MULTITHREAD
#define act_on_sgn_req        _ENTITY_PREFIXED(act_on_sgn_req)
#define sig_bcs_ker_bdat_ind  _ENTITY_PREFIXED(sig_bcs_ker_bdat_ind)
#define sig_bcs_ker_err_ind   _ENTITY_PREFIXED(sig_bcs_ker_err_ind )
#define sig_msg_ker_mdat_ind  _ENTITY_PREFIXED(sig_msg_ker_mdat_ind)
#endif /* OPTION_MULTITHREAD */

EXTERN void act_on_sgn_req(void);
EXTERN void sig_bcs_ker_bdat_ind(void);
EXTERN void sig_bcs_ker_err_ind (UBYTE cause);
EXTERN void sig_msg_ker_mdat_ind(T_FAD_DATA_IND *fad_data_ind);

#define sig_dti_ker_connection_opened_ind                 \
        _ENTITY_PREFIXED(sig_dti_ker_connection_opened_ind)
#define sig_dti_ker_connection_closed_ind                 \
        _ENTITY_PREFIXED(sig_dti_ker_connection_closed_ind)
#define sig_dti_ker_tx_buffer_full_ind                    \
        _ENTITY_PREFIXED(sig_dti_ker_tx_buffer_full_ind   )
#define sig_dti_ker_tx_buffer_ready_ind                   \
        _ENTITY_PREFIXED(sig_dti_ker_tx_buffer_ready_ind  )
#define sig_dti_ker_data_received_ind                     \
        _ENTITY_PREFIXED(sig_dti_ker_data_received_ind    )

/*
 * wrapping functions for dtilib primitives
 */

/*
 * these functions are called by pei_dti_callback
 */

EXTERN void sig_dti_ker_connection_opened_ind(void);
EXTERN void sig_dti_ker_connection_closed_ind(void);
EXTERN void sig_dti_ker_tx_buffer_full_ind(void);
EXTERN void sig_dti_ker_tx_buffer_ready_ind(void);
EXTERN void sig_dti_ker_data_received_ind(T_DTI2_DATA_REQ *dti_data_req);
/*
 *  KERNEL procedures
 */
#ifdef OPTION_MULTITHREAD

  #define ker_check_rtc      _ENTITY_PREFIXED(ker_check_rtc    )
  #define ker_check_rtc      _ENTITY_PREFIXED(ker_check_rtc    )
  #define ker_fill_dcs_info  _ENTITY_PREFIXED(ker_fill_dcs_info)
  #define ker_fill_dis_info  _ENTITY_PREFIXED(ker_fill_dis_info)
  #define ker_fill_dtc_info  _ENTITY_PREFIXED(ker_fill_dtc_info)
  #define ker_init           _ENTITY_PREFIXED(ker_init         )
  #define ker_res_sub        _ENTITY_PREFIXED(ker_res_sub      )
  #define ker_send_dcs       _ENTITY_PREFIXED(ker_send_dcs     )
  #define ker_send_dis       _ENTITY_PREFIXED(ker_send_dis     )
  #define ker_send_dtc       _ENTITY_PREFIXED(ker_send_dtc     )
  #define ker_send_err       _ENTITY_PREFIXED(ker_send_err     )

  #define snd_error_ind      _ENTITY_PREFIXED(snd_error_ind    )
  #define snd_t30_sgn_ind    _ENTITY_PREFIXED(snd_t30_sgn_ind  )
  #define snd_t30_sgn_ind    _ENTITY_PREFIXED(snd_t30_sgn_ind  )

#if defined _SIMULATION_ || defined KER_DEBUG_BCS || defined KER_DEBUG_MSG
  #define ker_debug _ENTITY_PREFIXED(ker_debug)
#endif

  #define TDCcompress          _ENTITY_PREFIXED(TDCcompress)
  #define prepare_dti_data_ind _ENTITY_PREFIXED(prepare_dti_data_ind)
  #define snd_dti_data_ind     _ENTITY_PREFIXED(snd_dti_data_ind)
  #define snd_t30_phase_ind    _ENTITY_PREFIXED(snd_t30_phase_ind)
  #define dti_connect_open     _ENTITY_PREFIXED(dti_connect_open)
  #define dti_connect_setup    _ENTITY_PREFIXED(dti_connect_setup)
  #define dti_connect_close    _ENTITY_PREFIXED(dti_connect_close)
  #define dti_disconnect       _ENTITY_PREFIXED(dti_disconnect)
  #define snd_t30_dti_cnf      _ENTITY_PREFIXED(snd_t30_dti_cnf)
  #define snd_dti_ready_ind    _ENTITY_PREFIXED(snd_dti_ready_ind)

#endif

EXTERN UBYTE ker_check_rtc     (T_FAD_DATA_IND *fad_data_ind);
EXTERN void  ker_fill_dcs_info (T_BCS_DCS *bcs_dcs);
EXTERN void  ker_fill_dis_info (T_BCS_DIS *bcs_dis);
EXTERN void  ker_fill_dtc_info (T_BCS_DTC *bcs_dtc);
EXTERN void  ker_init          (T_T30_DATA *t30_data);
EXTERN void  ker_send_dcs      (T_T30_CAP_REQ *t30_cap_req);
EXTERN void  ker_send_dis      (void);
EXTERN void  ker_send_dtc      (T_T30_CAP_REQ *t30_cap_req);
EXTERN void  ker_send_err      (USHORT cause);

EXTERN void  snd_complete_ind  (UBYTE cmpl);
EXTERN void  snd_error_ind     (USHORT cause);
EXTERN void  snd_t30_sgn_ind   (UBYTE sgn);

#if defined _SIMULATION_ || defined KER_DEBUG_BCS || defined KER_DEBUG_MSG
EXTERN void  ker_debug(CHAR *header, UBYTE *buf, USHORT len);
#endif

EXTERN UBYTE TDCcompress(T_desc2 *desc, T_sdu *sdu);
EXTERN void  prepare_dti_data_ind(T_FAD_DATA_IND *fad_data_ind);
EXTERN void  snd_dti_data_ind(T_DTI2_DATA_IND **dti_data_ind);
EXTERN void  snd_t30_phase_ind(UBYTE phase);
EXTERN void  dti_connect_setup(T_T30_DTI_REQ *t30_dti_req);
EXTERN void  dti_connect_open();
EXTERN void  dti_connect_close();
EXTERN void  dti_disconnect();
EXTERN void  snd_t30_dti_cnf(UBYTE dti_conn);
EXTERN void  snd_dti_ready_ind();


/*---------------------------------------------------------------------
 *  T30 BCS FORMATTER
 *
 *  BCS primitive processing
 */

/*
 *  BCS signal processing
 */
#ifdef OPTION_MULTITHREAD
  #define sig_ker_bcs_bdat_req _ENTITY_PREFIXED(sig_ker_bcs_bdat_req)
  #define sig_mux_bcs_bcs_ind  _ENTITY_PREFIXED(sig_mux_bcs_bcs_ind )
#endif

EXTERN void sig_ker_bcs_bdat_req (UBYTE final);
EXTERN void sig_mux_bcs_bcs_ind  (T_FAD_DATA_IND *fad_data_ind);

/*
 *  BCS procedures
 */

#ifdef OPTION_MULTITHREAD
  #define bcs_bcs_decode   _ENTITY_PREFIXED(bcs_bcs_decode  )
  #define bcs_bcs_encode   _ENTITY_PREFIXED(bcs_bcs_encode  )
  #define bcs_fill_bcs_frm _ENTITY_PREFIXED(bcs_fill_bcs_frm)
  #define bcs_init         _ENTITY_PREFIXED(bcs_init        )
#endif

EXTERN void  bcs_bcs_decode   (void);
EXTERN void  bcs_bcs_encode   (UBYTE ctl, UBYTE final);
EXTERN UBYTE bcs_fill_bcs_frm (T_FAD_DATA_IND *fad_data_ind);
EXTERN void  bcs_init         (T_T30_DATA *);

/*---------------------------------------------------------------------
 *  T30 MSG FORMATTER
 *
 *  MSG primitive processing
 */

/*
 *  MSG signal processing
 */
#ifdef OPTION_MULTITHREAD
  #define sig_ker_msg_mdat_req _ENTITY_PREFIXED(sig_ker_msg_mdat_req)
  #define sig_mux_msg_msg_ind  _ENTITY_PREFIXED(sig_mux_msg_msg_ind)
#endif

EXTERN void sig_ker_msg_mdat_req (T_FAD_DATA_REQ *fad_data_req);
EXTERN void sig_mux_msg_msg_ind  (T_FAD_DATA_IND *fad_data_ind);


/*---------------------------------------------------------------------
 *  T30 MULTIPLEXER
 *
 *  MUX primitive processing
 */

EXTERN void mux_fad_data_ind (T_FAD_DATA_IND *fad_data_ind);
EXTERN void mux_fad_mux_ind  (T_FAD_MUX_IND  *fad_mux_ind);

/*
 *  MUX signal processing
 */
#ifdef OPTION_MULTITHREAD
  #define sig_bcs_mux_bcs_req  _ENTITY_PREFIXED(sig_bcs_mux_bcs_req)
  #define sig_ker_mux_mux_req  _ENTITY_PREFIXED(sig_ker_mux_mux_req)
  #define sig_msg_mux_msg_req  _ENTITY_PREFIXED(sig_msg_mux_msg_req)
#endif

EXTERN void sig_bcs_mux_bcs_req (T_FAD_DATA_REQ *fad_data_req);
EXTERN void sig_ker_mux_mux_req (void);
EXTERN void sig_msg_mux_msg_req (T_FAD_DATA_REQ *fad_data_req);

#define MEMCOPSIZ(d,s) memcpy(d, s, sizeof(s))

/*---------------------------------------------------------------------
 *  timer
 */

#define TIMERSTART(i,v) vsi_t_start (VSI_CALLER i,v);

#define TIMERSTATUS(i,v) vsi_t_status (VSI_CALLER i,v);

#define TIMERSTOP(i) {vsi_t_stop (VSI_CALLER i); \
                      t30_data->data_cnf = FALSE;};

/*
 * Communication handles
 */
#ifdef OPTION_MULTITHREAD
  #define hCommFAD _ENTITY_PREFIXED(hCommFAD)
  #define hCommMMI _ENTITY_PREFIXED(hCommMMI)
#endif

EXTERN DTI_HANDLE t30_hDTI;     /* DTI connection handle for DTI library      */

#ifdef T30_PEI_C
GLOBAL T_HANDLE t30_handle;
GLOBAL T_HANDLE hCommFAD = VSI_ERROR;    /* FAD Communication */
GLOBAL T_HANDLE hCommMMI = VSI_ERROR;    /* MMI Communication */
#else
EXTERN T_HANDLE t30_handle;
EXTERN T_HANDLE hCommFAD;
EXTERN T_HANDLE hCommMMI;
#endif

GLOBAL void t30_timeout (USHORT index);

/*
 * entity common buffer for the decoded air interface message
 */
#ifdef OPTION_MULTITHREAD
  #define _decodedMsg _ENTITY_PREFIXED(_decodedMsg)
#endif

#ifdef T30_PEI_C

GLOBAL const UBYTE BIT_MIRROR[] =
{
  0x00,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,
  0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0,
  0x08,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8,
  0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,0xf8,
  0x04,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4,
  0x14,0x94,0x54,0xd4,0x34,0xb4,0x74,0xf4,
  0x0c,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec,
  0x1c,0x9c,0x5c,0xdc,0x3c,0xbc,0x7c,0xfc,
  0x02,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2,
  0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0xf2,
  0x0a,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea,
  0x1a,0x9a,0x5a,0xda,0x3a,0xba,0x7a,0xfa,
  0x06,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6,
  0x16,0x96,0x56,0xd6,0x36,0xb6,0x76,0xf6,
  0x0e,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee,
  0x1e,0x9e,0x5e,0xde,0x3e,0xbe,0x7e,0xfe,
  0x01,0x81,0x41,0xc1,0x21,0xa1,0x61,0xe1,
  0x11,0x91,0x51,0xd1,0x31,0xb1,0x71,0xf1,
  0x09,0x89,0x49,0xc9,0x29,0xa9,0x69,0xe9,
  0x19,0x99,0x59,0xd9,0x39,0xb9,0x79,0xf9,
  0x05,0x85,0x45,0xc5,0x25,0xa5,0x65,0xe5,
  0x15,0x95,0x55,0xd5,0x35,0xb5,0x75,0xf5,
  0x0d,0x8d,0x4d,0xcd,0x2d,0xad,0x6d,0xed,
  0x1d,0x9d,0x5d,0xdd,0x3d,0xbd,0x7d,0xfd,
  0x03,0x83,0x43,0xc3,0x23,0xa3,0x63,0xe3,
  0x13,0x93,0x53,0xd3,0x33,0xb3,0x73,0xf3,
  0x0b,0x8b,0x4b,0xcb,0x2b,0xab,0x6b,0xeb,
  0x1b,0x9b,0x5b,0xdb,0x3b,0xbb,0x7b,0xfb,
  0x07,0x87,0x47,0xc7,0x27,0xa7,0x67,0xe7,
  0x17,0x97,0x57,0xd7,0x37,0xb7,0x77,0xf7,
  0x0f,0x8f,0x4f,0xcf,0x2f,0xaf,0x6f,0xef,
  0x1f,0x9f,0x5f,0xdf,0x3f,0xbf,0x7f,0xff
};

GLOBAL UBYTE _decodedMsg [MAX_MSTRUCT_LEN_T30];

#else

EXTERN const UBYTE BIT_MIRROR[];
EXTERN UBYTE _decodedMsg[];

#endif

#endif /* T30_H */
