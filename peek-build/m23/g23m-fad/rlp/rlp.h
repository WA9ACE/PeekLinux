/*
+-----------------------------------------------------------------------------
|  Project :  CSD (8411)
|  Modul   :  Rlp.h
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
|             Radio Link Protocol
+-----------------------------------------------------------------------------
*/

#ifndef RLP_H
#define RLP_H

/*
 * Conditional Compiling
 */

#undef TRACE_INTERPRETED_RLP_FRAME
#undef TRACE_RLP_FRAME
#define V42BIS

/*
 * Macros
 */

 /*
 * instance management
 */
#define GET_INSTANCE(p) &rlp_data_base[0]

/*
 * Constants
 */
/*
 * global unchangable constants
 */
#ifdef V42BIS
#define ORIG_RLP_VERS	      1     /* rlp version (1=single link ext)*/
#else
#define ORIG_RLP_VERS	      0     /* rlp version (0=single link)*/
#endif

#define WIND_SIZE_MAX   61     /* max window size RLP	     */
#define DATA_SIZE_SHORT 25     /* num of data bytes in rlp frame (short format) */
#define DATA_SIZE_LONG	67     /* num of data bytes in rlp frame (long format, version 1) */

/*
 * default timeout for the RLP timer
 */
#define GEN_TO	TT_VALUE /* default retransmission timer*/
#define UL_TO   TUL_RCV_VALUE

/*
 * max number of outstanding SREJs
 */
#define MAX_SREJ_COUNT  10

/*
* timer indices
*
* 0..MAX_SREJ_COUNT  reserved for field
*/
#define TT_HANDLE	    (MAX_SREJ_COUNT+1)
#define TRCVR_HANDLE    (MAX_SREJ_COUNT+2)
#define TXID_HANDLE	    (MAX_SREJ_COUNT+3)
#define TUL_RCV_HANDLE  (MAX_SREJ_COUNT+4)

/*
 * number of frames that are requested in case if low water
 */

/*
 * maximum number N2 can have
 */
#define N2_MAX  255

/*
 * derived constants
 */
#define HEADER_LEN        (RLP_HEADER_LEN/8)
#define TRAILER_LEN       (RLP_TRAILER_LEN/8)
#define FRAME_SIZE_SHORT  (DATA_SIZE_SHORT+HEADER_LEN+TRAILER_LEN)
#define FRAME_SIZE_LONG   (DATA_SIZE_LONG+HEADER_LEN+TRAILER_LEN)

/*
 * constants for XID parameter types
 */
#define XID_T_NULL	    0  /* end tag			*/
#define XID_T_RLP_VERS	1  /* rlp version number		*/
#define XID_T_K_IWF_MS	2  /* window size iwf->ms 	*/
#define XID_T_K_MS_IWF	3  /* window size ms->iwf 	*/
#define XID_T_T1	      4  /* acknowledge timer		*/
#define XID_T_N2	      5  /* max num of retrans attempts */
#define XID_T_T2	      6  /* reply delay 		*/
#define XID_T_COMPR	    7  /* data compression		*/

/*
 * constants for XID parameter length in bytes
 */
#define XID_L_RLP_VERS	1  /* rlp version number		*/
#define XID_L_K_MS_IWF	1  /* window size ms->iwf 	*/
#define XID_L_K_IWF_MS	1  /* window size iwf->ms 	*/
#define XID_L_T1	      1  /* acknowledge timer		*/
#define XID_L_N2	      1  /* max num of retrans attempts */
#define XID_L_T2	      1  /* reply delay 		*/
#define XID_L_COMPR	    4  /* data compression		*/

/*
 * constants, to indicate parameters, which are used in XID
 */
#define UF_SET_RLP_VERS   0x01
#define UF_SET_K_IWF_MS   0x02
#define UF_SET_K_MS_IWF   0x04
#define UF_SET_T1	        0x08
#define UF_SET_N2	        0x10
#define UF_SET_T2	        0x20
#define UF_SET_COMPR	    0x40
#define UF_ALL		        0x7F

/*
 * default values for XID
 */
#define DEF_RLP_VERS	  0
#define DEF_K_MS_IWF	  61
#define DEF_K_IWF_MS	  61
#define DEF_T1_FR14400	  520
#define DEF_T1_FR9600	  480
#define DEF_T1_FR4800	  540
#define DEF_T1_HR4800	  780

#define DEF_T2		  80
#define DEF_N2		  6
#define DEF_PT		  0
#define DEF_P0		  0
#define DEF_P1		  512
#define DEF_P2		  6

/*
 * Bitoffset for encoding/decoding
 */
#define ENCODE_OFFSET	  0

/*
 * Parameter of dynamic configuration commands
 */

#ifndef NTRACE

/* for trace purposes */

#define SERVICE_NAME_KER      "KER"
#define SERVICE_NAME_KERXID_C "XID_C"
#define SERVICE_NAME_RCV      "RCV"
#define SERVICE_NAME_SND      "SND"

#endif

#define KER       ker.
#define KERXID_C  ker.XID_C.
#define RCV	      rcv.
#define SND	      snd.

/*
 * State index for the entity processes
 */

/*
 * States of the entity process KERNEL
 */
#define RLP_ADM_AND_DETACHED	      0
#define RLP_ADM_AND_ATTACHED	      1
#define RLP_PENDING_CONNECT_REQ     2
#define RLP_PENDING_CONNECT_IND     3
#define RLP_CONNECTION_ESTABLISHED  4
#define RLP_PENDING_RESET_REQ	      5
#define RLP_PENDING_RESET_IND	      6
#define RLP_DISCONNECT_INITIATED    7
#define RLP_PERMANENT_ERROR	        9
#define RLP_PENDING_REMAP_REQ	      10
#define RLP_REMAP_DATA		          11

/*
 * States of the entity process RCV
 */
#define RPDU_WAIT_FOR_A_BLOCK 0


/*
 * States of the entity process SND
 */
#define SPDU_WAIT_EVENT 0

/*==== TYPES ======================================================*/

/*
 * RLP global typedefs
 */

typedef ULONG T_BITSET;

typedef UBYTE	    T_BIT_INT;
typedef T_BIT_INT T_BIT;

typedef UBYTE	    T_FRAME_NUM;
typedef UBYTE	    T_EXT_FRAME_NUM;

/*
 * structure of a RLP Frame
 */
typedef UBYTE   T_RLP_FRAMEDATA;
/*
typedef UBYTE   T_RLP_FRAME_SHORT[FRAME_SIZE_SHORT];
*/
typedef UBYTE   T_RLP_FRAME_LONG[FRAME_SIZE_LONG];
typedef UBYTE*  T_RLP_FRAMEPTR;

/*
 * bitcoding of the M5-M1 of the RLP header for the
 * different frame types.
 */
typedef enum
{
  UF_UI    =0x00,
  UF_DM    =0x03,
  UF_SABM  =0x07,
  UF_DISC  =0x08,
  UF_UA    =0x0C,
  UF_NULL  =0x0F,
  UF_XID   =0x17,
  UF_TEST  =0x1C,
  UF_REMAP =0x11
} T_UF;

/*
 * bitcoding of the S2-S1 of the RLP header for the
 * different frame types.
 */
typedef enum
{
  SF_RR   =0x00,
  SF_RNR  =0x01,
  SF_REJ  =0x02,
  SF_SREJ =0x03
} T_SF;

typedef enum
{
  PDU_INVALID,
  PDU_SABM,
  PDU_DISC,
  PDU_UA,
  PDU_DM,
  PDU_TEST,
  PDU_NULL,
  PDU_XID,
  PDU_UI,
  PDU_RR_I,
  PDU_RNR_I,
  PDU_SREJ_I,
  PDU_REJ_I,
  PDU_RR,
  PDU_RNR,
  PDU_SREJ,
  PDU_REJ,
  PDU_REMAP
} T_PDU_TYPE;

/*
 * Process kernel specific types
 */
#define IS_IDLE   0
#define IS_SEND   1

#define IW_IDLE   0
#define IW_WAIT   1

#define ISW_IDLE  0
#define ISW_SEND  1
#define ISW_WAIT  2

#define  DTX_N    0
#define  DTX_RR   1
#define  DTX_RNR  2

typedef UBYTE T_COUNTER;

#define INVALID_IDX	((UBYTE) -1)

/*
 * send buffer management
 */
#define SBM_INVALID_IDX   INVALID_IDX

#define SBM_BUF_SIZE	  (WIND_SIZE_MAX+1)
#define SBM_PRIMBUF_SIZE  SBM_BUF_SIZE

typedef T_RLP_DATA_REQ	  T_SBM_PRIM;

typedef UBYTE T_SBM_BUFFER_INDEX;
typedef UBYTE T_SBM_BUFFER_EXTIDX;

typedef USHORT T_SBM_PRIM_INDEX;
typedef USHORT T_SBM_PRIM_EXTIDX;

typedef struct
{
  T_SBM_PRIM *Prim;
  UBYTE       RefCount;
} T_SBM_PRIMSLOT;

typedef enum
{
  SBM_IDLE,
  SBM_SEND,
  SBM_WAIT
} T_SBM_STATE;

typedef struct
{
  T_SBM_PRIM_EXTIDX	PrimSlot1;  /* Index on T_SBM_PRIM	   */
  T_SBM_PRIM_EXTIDX	PrimSlot2;  /* Index on T_SBM_PRIM	   */
  USHORT		    Offset1;        /* Offset of data in primitive  */
  USHORT		    Offset2;        /* Offset of data in primitive  */
  USHORT		    Len1;	          /* Length of data in primitive  */
  USHORT		    Len2;	          /* Length of data in primitive  */
  T_SBM_STATE		S_State;        /* 'SBM_SEND' means, data has to be sent (with seq. num. n) */
  T_SBM_BUFFER_EXTIDX	NextSREJ;
} T_SBM_SLOT;

/*
 * receive buffer management
 */
#define RBM_INVALID_IDX   INVALID_IDX

#define RBM_BUF_SIZE	  (WIND_SIZE_MAX+1)

#ifdef _SIMULATION_
#define RBM_PQ_MAX_PRIM   10
#else
#define RBM_PQ_MAX_PRIM   4
#endif

#define RBM_PQ_THRE_RNR   (RBM_PQ_MAX_PRIM-2)
#define RBM_PQ_SIZE	      (RBM_PQ_MAX_PRIM+1)

typedef T_RLP_DATA_IND  T_RBM_PRIM;

typedef UBYTE T_RBM_BUFFER_INDEX;
typedef UBYTE T_RBM_BUFFER_EXTIDX;
typedef UBYTE T_RBM_PQ_INDEX;

typedef T_RLP_FRAME_LONG  T_RBM_FRAME_LONG;
typedef T_RLP_FRAMEPTR	  T_RBM_FRAMEPTR;

typedef enum
{
  RBM_IDLE,
  RBM_RCVD,
  RBM_SREJ,
  RBM_WAIT
} T_RBM_STATE;


typedef struct
{
  T_RBM_FRAME_LONG Frame;   /* To store user information */
  T_RBM_STATE	   R_State;   /* 'RBM_RCVD': data has been received (with seq. num. n)
			                         'RBM_SREJ': the retrans. has to be requested using srej(n)
			                         'RBM_WAIT': the entity waits for the requested retransmitted data */
} T_RBM_SLOT;

/*
 * SREJ timer managment
 */

#define SRM_DATA_SIZE	 (WIND_SIZE_MAX+1)

typedef T_COUNTER	 T_SRM_COUNTER;

typedef struct
{
  T_SRM_COUNTER   count;
} T_SRM_SLOT;

/*
 * process global data
 */

/*
 * data for process kernel
 */

/*
 * table entry for timeout handling of the T_RCVS timer
 */
typedef struct
{
  BOOL	    	isFree;
  T_FRAME_NUM	slot;
  UBYTE 	    trcvs_running;
} T_RCVS_ENTRY;

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *name;
  char *state_name;
#endif
} T_SUB_STATE;

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *name;
  char *state_name;
#endif

  /*------------ variables according to GSM 04.22 ----------------------------------*/

  T_BIT 	    ABit;	      /* to store the Ack_Flg value of a received Connect_Resp frame */

  T_BIT 	    Ackn_FBit;	  /* Value of the F-Bit used in the next acknowledging PDU. */
  UBYTE	        Ackn_State;   /* 'SEND': an acknowledging PDU (Supervisory or Data) has to be sent */
				              /* after receiving an I-frame or a command frame with the Poll bit = 1 */

  USHORT	    BytesPerPrim; /* number of bytes in RLP data primitives */
  BOOL		    Connected;	  /* Process in RLP_CONNECTION_ESTABLISHED state */

  USHORT	    Def_T1;       /* Default value for T1, depends on transmission rate */

  T_COUNTER	    DISC_Count;   /* to count the transmissions of DISC.*/
  BOOL		    DISC_Ind;	  /* In RLP_DISCONNECT_INITIATED: Indicate succesfull DISC by RLP_DISC_IND */
  T_BIT 	    DISC_PBit;	  /* The value of the P-bit in the next DISC command PDU.*/
  UBYTE	        DISC_State;   /* 'SEND': the DISC command PDU has to be sent at the next possible opportunity. */
				              /* 'WAIT': the RLP entity waits for the corresponding response.*/
  T_BIT 	    DM_FBit;	  /* Value of the F-Bit used in the next DM response PDU.*/
  UBYTE         DM_State;	  /* 'SEND': the PDU DM has to be sent. */
  UBYTE	        DTX_SF;	      /* to store the last Superv. frame for DTX (only RR or RNR can be suppressed) */
  T_FRAME_NUM   DTX_VR;	      /* to store the last transmitted value of VR (used to decide the DTX condition) */
  USHORT	    FrameSize;	  /* Size of RLP frame may be FRAME_SIZE_SHORT or FRAME_SIZE_LONG */
  USHORT	    FramesPerPrim;/* number of frames in RLP data primitives (calculated from BytesPerPrim) */

  T_FRAME_NUM	K_ms_iwf;	    /* window size ms->iwf */
  T_FRAME_NUM	K_iwf_ms;	    /* window size iwf->ms */

  BOOL		    LRFull;	      /* Local Receiver Full */
  BOOL		    LRReady;	  /* Local Receiver Ready */

  UBYTE 	    N2; 	        /* maximum numer of retransmission attempts */

  USHORT	    OldFrameSize;	/* Size of RLP frame before REMAP */

  T_FRAME_NUM	Orig_K_iwf_ms;  /* original value for k_iwf_ms */
  T_FRAME_NUM	Orig_K_ms_iwf;  /* original value for k_ms_iwf */
  UBYTE 	    Orig_N2;	    /* original value for n2 */
  UBYTE 	    Orig_P0;	    /* original value for p0 */
  USHORT	    Orig_P1;	    /* original value for p1 */
  UBYTE 	    Orig_P2;	    /* original value for p2 */
  UBYTE 	    Orig_Pt;	    /* original value for pt */
  USHORT	    Orig_T1;	    /* original value for t1 */
  USHORT	    Orig_T2;	    /* original value for t2 */

  UBYTE 	    P0; 	        /* V.42bis data compression request */
  USHORT	    P1; 	        /* number of possible codewords   */
  UBYTE 	    P2; 	        /* maximum encodable string length */

  T_COUNTER	    Poll_Count;   /* to count the transmissions of poll requests*/
  UBYTE	        Poll_State;   /* 'SEND': a supervisory PDU with P-bit set to one has to be sent*/
				              /* 'WAIT': the RLP entity waits for the response with F-bit set to one */
  UBYTE	        Poll_xchg;    /* 'IDLE': sending of a frame with P-bit set is allowed */
				              /* 'WAIT': an acknowledgement of a previous P-bit is outstanding */
  UBYTE 	    Pt;	          /* type of data compression */

  UBYTE	        REJ_State;	  /* The REJ_State is send if and only if a REJ PDU has to be sent */

  T_RLP_FRAME_LONG  REMAP_Frame;      /* frame to be sent in the next REMAP PDU*/
  T_FRAME_DESC	    REMAP_FrameDesc;  /* Descriptor for REMAP_Frame (static) */
  T_FRAME_NUM	    RemapNr;		      /* Frame number taken from REMAP response */

  UBYTE 	    Rlp_Vers;	  /* negotiated RLP version */
  BOOL		    RRReady;	  /* Remote Receiver Ready*/
  T_COUNTER	    SABM_Count;   /* to count the transmissions of SABM*/
  UBYTE	        SABM_State;   /* 'SEND': the SABM PDU has to be sent */
				                    /* 'WAIT': the RLP entity waits for the UA response */
  T_SF		    SF; 	        /* to store the last superv. PDU type*/
  USHORT	    SREJ_Count;   /* number of outstanding SREJs == number of running SREJ timers (downlink) */

  USHORT	    T1; 	    /* value for acknowledge timer */
  USHORT	    T2; 	    /* reply delay */
  USHORT	    T_ul;	    /* value for timer T_UL_RCV (upper layer waiting for data) */

  T_RLP_FRAME_LONG  TEST_R_Frame;     /* data to be sent in the next TEST response PDU */
  T_FRAME_DESC	    TEST_R_FrameDesc; /* descriptor for TEST_R_Frame (static) */
  T_BIT 	        TEST_R_FBit;      /* value of the P-Bit used in the next TEST response PDU */
  UBYTE	            TEST_R_State;     /* 'SEND': the TEST response PDU has to be sent */

  T_BIT 	        UA_FBit;	    /* value of the F-Bit used in the next UA response */
  UBYTE	            UA_State;	    /* if (UA_State = SEND) an UA PDU has to be sent */

  T_RLP_FRAME_LONG  UI_Frame;	    /* frame to be sent in the next UI PDU*/
  T_FRAME_DESC	    UI_FrameDesc;   /* Descriptor for UI_Frame (static) */
  T_BIT 	        UI_PBit;	    /* value of the P-Bit used in the next UI PDU */
  UBYTE	            UI_State;	    /* 'SEND': a UI PDU has to be sent */

  UBYTE	            UL_Rcv_State;   /* 'WAIT': Upper layer is waiting for data */
  UBYTE	            UL_Snd_State;   /* 'WAIT': RLP is waiting for data from upper layer*/

  T_COUNTER	        XID_Count;	  /* to count the transmissions of XID commands */

  T_RLP_FRAME_LONG  XID_C_Frame;    /* data to be sent in the next XID command PDU */
  T_FRAME_DESC	    XID_C_FrameDesc;/* Descriptor for XID_C_Frame (static) */
  T_BIT 	        XID_C_PBit;       /* value of the P-Bit used in the next XID command PDU */

  T_SUB_STATE	    XID_C;          /* 'SEND': the XID command PDU has to be sent */
				                    /* 'WAIT': the RLP entity waits for the next XID response */

  T_BITSET	        XID_C_Used_Flg; /* Contains bits that indicate
                                       which parameters have been received in the last XID command PDU */

  T_RLP_FRAME_LONG  XID_R_Frame;      /* frame to be sent in the next XID response PDU */
  T_FRAME_DESC	    XID_R_FrameDesc;  /* Descriptor for XID_R_Frame (static) */
  T_BIT 	        XID_R_FBit;       /* value of the P-Bit used in the next XID response PDU */
  UBYTE	            XID_R_State;      /* 'SEND': the XID response PDU has to be sent*/
  T_BITSET	        XID_R_Used_Flg;   /* Contains bits that indicate
                                       which parameters have to be send in the next XID response PDU */

  T_RCVS_ENTRY	    rcvsTab[MAX_SREJ_COUNT];

} T_KER;

/*
 * data for process receive_pdu
 */

/* no global data */

/*
 * data for process send_pdu
 */

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *name;
  char *state_name;
#endif

  T_RLP_FRAME_LONG  HO_Frame;	    /* Buffer and Descriptor for RLP Frames with header only */
  T_FRAME_DESC	    HO_FrameDesc;
  USHORT	    FrameSize;		      /* Size of RLP frame may be FRAME_SIZE_SHORT or FRAME_SIZE_LONG */
} T_SND;

/*
 * data for send buffer management
 */
typedef struct
{
  T_SBM_PRIM_INDEX	    CurrPrimSlot;
  USHORT		            CurrPrimOff;
  USHORT		            CurrPrimLen;
  T_SBM_BUFFER_INDEX	  K;
  T_SBM_BUFFER_EXTIDX	  LastSentFrame;
  T_SBM_PRIMSLOT	      Prim[SBM_PRIMBUF_SIZE];
  T_SBM_SLOT		        Frame[SBM_BUF_SIZE];
  USHORT		            FrameSize;
  T_SBM_BUFFER_EXTIDX	  SREJ_List;
  T_SBM_BUFFER_INDEX	  REJ_Buf[N2_MAX +1];
  UBYTE 		            REJ_BufLo;
  UBYTE 		            REJ_BufHi;
  UBYTE 		            REJ_BufSize;
  T_SBM_BUFFER_INDEX	  REJ_BufLastN;
  T_SBM_BUFFER_INDEX	  VA;
  T_SBM_BUFFER_INDEX	  VD;
  T_SBM_BUFFER_INDEX	  VS;
} T_SBM;

/*
 * data for receive buffer management
 */
typedef struct
{
  BOOL			          Initialised;
  BOOL			          FrameInRiBu;
  T_RBM_PRIM         *CurrPrim;
  USHORT		          CurrPrimCou;
  USHORT		          FramesPerPrim;
  T_RBM_BUFFER_INDEX  K;
  T_RBM_SLOT		      Slot[RBM_BUF_SIZE];
  USHORT		          FrameSize;
  T_RBM_FRAME_LONG	  Frame;
  T_RBM_PRIM	       *PQ_Array[RBM_PQ_SIZE];
  T_RBM_PQ_INDEX	    PQ_Read;
  T_RBM_PQ_INDEX	    PQ_Write;
  T_RBM_BUFFER_INDEX  VR;
  T_RBM_BUFFER_INDEX  LastVR;
} T_RBM;

/*
 * data for srej timer management
 */
typedef struct
{
  T_SRM_SLOT   Data[SRM_DATA_SIZE];
} T_SRM;

#if defined (TRACE_INTERPRETED_RLP_FRAME) || defined (TRACE_RLP_FRAME)

typedef struct
{
  char trc_buf[80];
  UBYTE idx;
} T_RLP_DEBUG;

#endif

/*
 * entity data for each RLP instance
 */

typedef struct
{
  UBYTE 	  tul_rcv_running;
  T_FRAME_NUM rcvs_slot;

  T_KER 	  ker; /* process kernel */
  T_SND 	  snd; /* process send_pdu */
  T_SBM 	  sbm; /* send buffer manager */
  T_SUB_STATE rcv;
  T_RBM 	  rbm; /* receive buffer manager */
  T_SRM 	  srm; /* srej timer manager */

  /* Flags for frame trace */
  BOOL		 uplink_frame_trace;
  BOOL		 downlink_frame_trace;

#if defined (TRACE_INTERPRETED_RLP_FRAME) || defined (TRACE_RLP_FRAME)
  T_RLP_DEBUG deb;
#endif

} T_RLP_DATA;


/*==== EXPORT =====================================================*/
/*
 * data base
 */


/*
 * instance data base
 */
#ifdef RLP_PEI_C

/*lint -e759 : header declaration for symbol 'rlp_data_base' could be moved from header to module */
GLOBAL T_RLP_DATA rlp_data_base[1], *rlp_data;
GLOBAL ULONG rlp_data_base_size;
GLOBAL ULONG rlp_data_magic_num;

#else

EXTERN T_RLP_DATA  rlp_data_base [], *rlp_data;
EXTERN ULONG rlp_data_magic_num;

#endif

#define ENTITY_DATA rlp_data
#define RLP_DATA_MAGIC_NUM (('R'<<24) + ('L'<<16) + ('P'<<8))	/* "FAD",NUL */

/*
 *  Prototypes
 *
 *  RLP KERNEL
 *
 *  KERNEL primitive processing
 */

EXTERN void ker_rlp_attach_req	 (T_RLP_ATTACH_REQ	*rlp_attach_req);
EXTERN void ker_rlp_detach_req	 (T_RLP_DETACH_REQ	*rlp_detach_req);
EXTERN void ker_rlp_connect_req	 (T_RLP_CONNECT_REQ	*rlp_connect_req);
EXTERN void ker_rlp_connect_res	 (T_RLP_CONNECT_RES	*rlp_connect_res);
EXTERN void ker_rlp_disc_req	 (T_RLP_DISC_REQ	*rlp_disc_req);
EXTERN void ker_rlp_reset_req	 (T_RLP_RESET_REQ	*rlp_reset_req);
EXTERN void ker_rlp_reset_res	 (T_RLP_RESET_RES	*rlp_reset_res);
EXTERN void ker_rlp_data_req	 (T_RLP_DATA_REQ	*rlp_data_req);
EXTERN void ker_rlp_getdata_req	 (T_RLP_GETDATA_REQ	*rlp_getdata_req);
EXTERN void ker_rlp_ui_req	 (T_RLP_UI_REQ		*rlp_ui_req);
EXTERN void ker_rlp_remap_req	 (T_RLP_REMAP_REQ	*rlp_remap_req);
EXTERN void ker_rlp_remap_data_res (T_RLP_REMAP_DATA_RES	*ker_rlp_remap_data_res);

/*
 *  KERNEL signal processing
 */
#ifdef OPTION_MULTITHREAD
  #define sig_rcv_ker_sabm_ind	   _ENTITY_PREFIXED(sig_rcv_ker_sabm_ind)
  #define sig_rcv_ker_disc_ind	   _ENTITY_PREFIXED(sig_rcv_ker_disc_ind)
  #define sig_rcv_ker_ua_ind	     _ENTITY_PREFIXED(sig_rcv_ker_ua_ind)
  #define sig_rcv_ker_dm_ind	     _ENTITY_PREFIXED(sig_rcv_ker_dm_ind)
  #define sig_rcv_ker_rr_ind	     _ENTITY_PREFIXED(sig_rcv_ker_rr_ind)
  #define sig_rcv_ker_rnr_ind	     _ENTITY_PREFIXED(sig_rcv_ker_rnr_ind)
  #define sig_rcv_ker_rej_ind	     _ENTITY_PREFIXED(sig_rcv_ker_rej_ind)
  #define sig_rcv_ker_srej_ind	   _ENTITY_PREFIXED(sig_rcv_ker_srej_ind)
  #define sig_rcv_ker_rr_i_ind	   _ENTITY_PREFIXED(sig_rcv_ker_rr_i_ind)
  #define sig_rcv_ker_rnr_i_ind    _ENTITY_PREFIXED(sig_rcv_ker_rnr_i_ind)
  #define sig_rcv_ker_rej_i_ind    _ENTITY_PREFIXED(sig_rcv_ker_rej_i_ind)
  #define sig_rcv_ker_srej_i_ind   _ENTITY_PREFIXED(sig_rcv_ker_srej_i_ind)
  #define sig_rcv_ker_xid_ind	     _ENTITY_PREFIXED(sig_rcv_ker_xid_ind)
  #define sig_rcv_ker_test_ind	   _ENTITY_PREFIXED(sig_rcv_ker_test_ind)
  #define sig_rcv_ker_ready_ind    _ENTITY_PREFIXED(sig_rcv_ker_ready_ind)
  #define sig_rcv_ker_remap_ind    _ENTITY_PREFIXED(sig_rcv_ker_remap_ind)
  #define sig_rcv_ker_rawdata_ind  _ENTITY_PREFIXED(sig_rcv_ker_rawdata_ind)
#endif

EXTERN void sig_rcv_ker_sabm_ind     (void);
EXTERN void sig_rcv_ker_disc_ind     (T_BIT);
EXTERN void sig_rcv_ker_ua_ind	     (T_BIT);
EXTERN void sig_rcv_ker_dm_ind	     (T_BIT);
EXTERN void sig_rcv_ker_rr_ind	     (T_BIT, T_BIT, T_FRAME_NUM);
EXTERN void sig_rcv_ker_rnr_ind      (T_BIT, T_BIT, T_FRAME_NUM);
EXTERN void sig_rcv_ker_rej_ind      (T_BIT, T_BIT, T_FRAME_NUM);
EXTERN void sig_rcv_ker_srej_ind     (T_BIT, T_BIT, T_FRAME_NUM);
EXTERN void sig_rcv_ker_rr_i_ind     (T_BIT, T_BIT, T_FRAME_NUM, T_FRAME_NUM);
EXTERN void sig_rcv_ker_rnr_i_ind    (T_BIT, T_BIT, T_FRAME_NUM, T_FRAME_NUM);
EXTERN void sig_rcv_ker_rej_i_ind    (T_BIT, T_BIT, T_FRAME_NUM, T_FRAME_NUM);
EXTERN void sig_rcv_ker_srej_i_ind   (T_BIT, T_BIT, T_FRAME_NUM, T_FRAME_NUM);
EXTERN void sig_rcv_ker_xid_ind      (T_BIT, T_BIT);
EXTERN void sig_rcv_ker_test_ind     (T_BIT, T_BIT);
EXTERN void sig_rcv_ker_ready_ind    (void);
EXTERN void sig_rcv_ker_remap_ind    (void);

#ifdef _SIMULATION_
EXTERN void sig_rcv_ker_rawdata_ind  (T_RA_DATA_IND *);
#else
EXTERN void sig_rcv_ker_rawdata_ind  (void);
#endif
/*
 *  KERNEL procedures
 */

#ifdef OPTION_MULTITHREAD

  #define ker_init                _ENTITY_PREFIXED(ker_init)
  #define ker_get_xid_data        _ENTITY_PREFIXED(ker_get_xid_data)
  #define ker_put_xid_data        _ENTITY_PREFIXED(ker_put_xid_data)
  #define ker_copy_frame_to_sdu   _ENTITY_PREFIXED(ker_copy_frame_to_sdu)
  #define ker_copy_sdu_to_frame   _ENTITY_PREFIXED(ker_copy_sdu_to_frame)
  #define ker_get_frame_desc      _ENTITY_PREFIXED(ker_get_frame_desc)
  #define ker_i_handler           _ENTITY_PREFIXED(ker_i_handler)
  #define ker_init_link_vars      _ENTITY_PREFIXED(ker_init_link_vars)
  #define ker_reset_all_t_rcvs    _ENTITY_PREFIXED(ker_reset_all_t_rcvs)
  #define ker_s_handler           _ENTITY_PREFIXED(ker_s_handler)
  #define ker_send_data           _ENTITY_PREFIXED(ker_send_data)
  #define ker_send_txu            _ENTITY_PREFIXED(ker_send_txu)
  #define ker_deinit_link_vars    _ENTITY_PREFIXED(ker_deinit_link_vars)
  #define ker_send_rlp_error_ind  _ENTITY_PREFIXED(ker_send_rlp_error_ind)
  #define ker_init_xid_data       _ENTITY_PREFIXED(ker_init_xid_data)
  #define ker_getSlotTRCVS        _ENTITY_PREFIXED(ker_getSlotTRCVS)
  #define ker_fill_remap_frame    _ENTITY_PREFIXED(ker_fill_remap_frame)
  #define ker_send_remap_data     _ENTITY_PREFIXED(ker_send_remap_data)
  #define ker_set_frame_size      _ENTITY_PREFIXED(ker_set_frame_size)

#ifdef TRACE_RLP_FRAME
  #define ker_trace_rlp_frame     _ENTITY_PREFIXED(ker_trace_rlp_frame)
#endif

#endif

EXTERN void	ker_init(void);

EXTERN void		  ker_get_xid_data
			  (
			    T_RLP_FRAMEDATA *raw_data,
			    UBYTE	    index,
			    T_RLP_XID_IND   *xid_data,
			    BOOL	     ms_is_initiator,
			    ULONG	    *used_flags
			  );

EXTERN void		  ker_put_xid_data
			  (
			    T_RLP_FRAMEDATA *l_Data,	      /* buffer for data */
			    UBYTE	            index,	      /* start index in buffer */
			    ULONG	            l_uf,	        /* valid parameters */
			    BOOL	         ms_is_initiator, /* MS is sending XID*/
			    UBYTE	        l_rlp_version,    /* negotiated vers. */
			    T_FRAME_NUM   l_k_iwf_ms,       /* winSize ms->iwf  */
			    T_FRAME_NUM   l_k_ms_iwf,       /* winSize iwf->ms  */
			    USHORT	      l_t1,	            /* ack timer value  */
			    UBYTE	        l_n2,	            /* num restransmiss */
			    USHORT	      l_t2,	            /* reply delay	  */
			    UBYTE	        l_pt,	            /* type data compr. */
			    UBYTE	        l_p0,	            /* v42bis comp. req */
			    USHORT	      l_p1,	            /* num possible code*/
			    UBYTE	        l_p2	            /* max encod. strlen*/
			  );

EXTERN void		  ker_copy_frame_to_sdu
			  (
			    T_RLP_FRAMEPTR  frame,
			    T_sdu	   *sdu
			  );

EXTERN void		  ker_copy_sdu_to_frame
			  (
			    T_sdu	   *sdu,
			    T_RLP_FRAMEPTR  frame,
			    USHORT	    n
			  );

EXTERN void		  ker_get_frame_desc
			  (
			    T_RLP_FRAMEPTR  frame,
			    T_FRAME_DESC   *frameDesc
			  );

EXTERN BOOL ker_i_handler(T_FRAME_NUM ns);

EXTERN void		  ker_init_link_vars
			  (
			    void
			  );

#ifdef TRACE_RLP_FRAME
EXTERN void		  ker_trace_rlp_frame
			  (
			    T_RLP_FRAMEPTR  frame
			  );
#endif

EXTERN void		  ker_reset_all_t_rcvs
			  (
			    void
			  );

EXTERN void		  ker_s_handler
			  (
			    T_BIT	cBit,
			    T_BIT	pFBit,
			    T_SF	sf,
			    T_FRAME_NUM nr,
			    BOOL	*retransError
			  );

EXTERN void		  ker_send_data
			  (
			    void
			  );

EXTERN BOOL		  ker_send_txu
			  (
			    void
			  );

EXTERN void		  ker_deinit_link_vars
			  (
			    void
			  );

EXTERN void		  ker_send_rlp_error_ind
			  (
			    USHORT cause
			  );

EXTERN void		  ker_init_xid_data
			  (
			    T_RLP_ATTACH_REQ  *rlp_attach_request
			  );

EXTERN T_FRAME_NUM	  ker_getSlotTRCVS
			  (
			    USHORT index
			  );

EXTERN void		  ker_fill_remap_frame
			  (
			    T_FRAME_NUM  vr
			  );

EXTERN BOOL		  ker_send_remap_data
			  (
			    void
			  );

EXTERN void		  ker_set_frame_size
			  (
			    UBYTE rate
			  );

/*
 *  RLP RCV
 *
 *  RCV primitive processing
 */
#ifdef _SIMULATION_
EXTERN const void rcv_ra_ready_ind(T_RA_READY_IND *ra_ready_ind);
#else
EXTERN void rcv_ra_ready_ind(void);
#endif

EXTERN void rcv_ra_data_ind(T_RA_DATA_IND *ra_data_ind);

/*
 *  RCV signal processing
 */
#ifdef OPTION_MULTITHREAD
  #define sig_ker_rcv_rawdata_res   _ENTITY_PREFIXED(sig_ker_rcv_rawdata_res)
#endif

EXTERN void sig_ker_rcv_rawdata_res
	    (
	      T_PDU_TYPE  pduType,
	      T_BIT	  cBit,
	      T_BIT	  pFBit,
	      T_FRAME_NUM nr,
	      T_FRAME_NUM ns,
	      BOOL	  crc
	    );

/*
 *  RCV procedures
 */

#ifdef OPTION_MULTITHREAD
  #define rcv_init		      _ENTITY_PREFIXED(rcv_init)
#endif

EXTERN void rcv_init(void);

/*
 *  RLP SND
 */

/*
 *  SND signal processing
 */
#ifdef OPTION_MULTITHREAD
  #define sig_ker_snd_sabm_req	 _ENTITY_PREFIXED(sig_ker_snd_sabm_req)
  #define sig_ker_snd_disc_req	 _ENTITY_PREFIXED(sig_ker_snd_disc_req)
  #define sig_ker_snd_ua_req		 _ENTITY_PREFIXED(sig_ker_snd_ua_req)
  #define sig_ker_snd_dm_req		 _ENTITY_PREFIXED(sig_ker_snd_dm_req)
  #define sig_ker_snd_rr_req		 _ENTITY_PREFIXED(sig_ker_snd_rr_req)
  #define sig_ker_snd_rnr_req		 _ENTITY_PREFIXED(sig_ker_snd_rnr_req)
  #define sig_ker_snd_rej_req		 _ENTITY_PREFIXED(sig_ker_snd_rej_req)
  #define sig_ker_snd_srej_req	 _ENTITY_PREFIXED(sig_ker_snd_srej_req)
  #define sig_ker_snd_rr_i_req	 _ENTITY_PREFIXED(sig_ker_snd_rr_i_req)
  #define sig_ker_snd_rnr_i_req  _ENTITY_PREFIXED(sig_ker_snd_rnr_i_req)
  #define sig_ker_snd_rej_i_req  _ENTITY_PREFIXED(sig_ker_snd_rej_i_req)
  #define sig_ker_snd_srej_i_req _ENTITY_PREFIXED(sig_ker_snd_srej_i_req)
  #define sig_ker_snd_ui_req		 _ENTITY_PREFIXED(sig_ker_snd_ui_req)
  #define sig_ker_snd_xid_req		 _ENTITY_PREFIXED(sig_ker_snd_xid_req)
  #define sig_ker_snd_test_req	 _ENTITY_PREFIXED(sig_ker_snd_test_req)
  #define sig_ker_snd_remap_req  _ENTITY_PREFIXED(sig_ker_snd_remap_req)
  #define sig_ker_snd_null_req	 _ENTITY_PREFIXED(sig_ker_snd_null_req)

  #define sig_ker_snd_set_frame_size_req _ENTITY_PREFIXED(sig_ker_snd_set_frame_size_req)
#endif

EXTERN void sig_ker_snd_sabm_req	   (void);
EXTERN void sig_ker_snd_disc_req	   (T_BIT);
EXTERN void sig_ker_snd_ua_req		   (T_BIT);
EXTERN void sig_ker_snd_dm_req		   (T_BIT);

#ifdef ENABLE_DTX
EXTERN void sig_ker_snd_rr_req		   (T_BIT, T_BIT, T_FRAME_NUM, T_BIT);
#else
EXTERN void sig_ker_snd_rr_req		   (T_BIT, T_BIT, T_FRAME_NUM);
#endif

EXTERN void sig_ker_snd_rnr_req 	   (T_BIT, T_BIT, T_FRAME_NUM, T_BIT);
EXTERN void sig_ker_snd_rej_req 	   (T_BIT, T_BIT, T_FRAME_NUM);
EXTERN void sig_ker_snd_srej_req	   (T_BIT, T_BIT, T_FRAME_NUM);
EXTERN void sig_ker_snd_rr_i_req	   (T_BIT, T_BIT, T_FRAME_NUM, T_FRAME_NUM, T_FRAME_DESC *);
EXTERN void sig_ker_snd_rnr_i_req	   (T_BIT, T_BIT, T_FRAME_NUM, T_FRAME_NUM, T_FRAME_DESC *);
EXTERN void sig_ker_snd_rej_i_req	   (T_BIT, T_BIT, T_FRAME_NUM, T_FRAME_NUM, T_FRAME_DESC *);
EXTERN void sig_ker_snd_srej_i_req	 (T_BIT, T_BIT, T_FRAME_NUM, T_FRAME_NUM, T_FRAME_DESC *);
EXTERN void sig_ker_snd_ui_req		   (T_BIT, T_BIT, T_FRAME_DESC *);
EXTERN void sig_ker_snd_xid_req 	   (T_BIT, T_BIT, T_FRAME_DESC *);
EXTERN void sig_ker_snd_test_req	   (T_BIT, T_BIT, T_FRAME_DESC *);
EXTERN void sig_ker_snd_remap_req	   (T_FRAME_DESC *);
EXTERN void sig_ker_snd_null_req	   (void);

EXTERN void sig_ker_snd_set_frame_size_req (USHORT);

/*
 *  SND procedures
 */

#ifdef OPTION_MULTITHREAD
  #define snd_init		      _ENTITY_PREFIXED(snd_init)
  #define snd_send_u_frame	_ENTITY_PREFIXED(snd_send_u_frame)
  #define snd_send_s_frame	_ENTITY_PREFIXED(snd_send_s_frame)
  #define snd_send_si_frame	_ENTITY_PREFIXED(snd_send_si_frame)
#endif

EXTERN void snd_init(T_SND *snd);

EXTERN void snd_send_u_frame
	    (
	      T_UF	    frameType,
	      T_BIT	    pFBit,
	      T_BIT	    crBit,
	      T_FRAME_DESC *dataDesc
	    );

EXTERN void snd_send_s_frame
	    (
	      T_SF	    frameType,
	      T_FRAME_NUM   nr,
	      T_BIT	    pFBit,
	      T_BIT	    crBit,
	      T_FRAME_DESC *dataDesc
	    );

EXTERN void snd_send_si_frame
	    (
	      T_SF	    frameType,
	      T_FRAME_NUM   nr,
	      T_BIT	    pFBit,
	      T_FRAME_NUM   ns,
	      T_BIT	    crBit,
	      T_FRAME_DESC *dataDesc
	    );

/*
 * Send Buffer Management SBM
 */
#ifdef OPTION_MULTITHREAD
  #define sbm_set_wind_size	  _ENTITY_PREFIXED(sbm_set_wind_size)
  #define sbm_init		        _ENTITY_PREFIXED(sbm_init)
  #define sbm_store_prim	    _ENTITY_PREFIXED(sbm_store_prim)
  #define sbm_ack_upto_n	    _ENTITY_PREFIXED(sbm_ack_upto_n)
  #define sbm_srej_frame	    _ENTITY_PREFIXED(sbm_srej_frame)
  #define sbm_get_frame 	    _ENTITY_PREFIXED(sbm_get_frame)
  #define sbm_rej_from_n	    _ENTITY_PREFIXED(sbm_rej_from_n)
  #define sbm_frame_in_range	_ENTITY_PREFIXED(sbm_frame_in_range)
  #define sbm_space_in_buf	  _ENTITY_PREFIXED(sbm_space_in_buf)
  #define sbm_deinit		      _ENTITY_PREFIXED(sbm_deinit)
  #define sbm_set_retrans	    _ENTITY_PREFIXED(sbm_set_retrans)
  #define sbm_check_rej_count	_ENTITY_PREFIXED(sbm_check_rej_count)
  #define sbm_recall_prim	    _ENTITY_PREFIXED(sbm_recall_prim)
#endif

EXTERN void	      sbm_set_wind_size
		      (
			T_SBM_BUFFER_INDEX windSize
		      );

EXTERN void	      sbm_init
		      (
			T_SBM_BUFFER_INDEX windSize,
			USHORT		   frameSize,
			UBYTE		   n2
		      );

EXTERN void	      sbm_store_prim
		      (
			T_SBM_PRIM *sendPrim
		      );

EXTERN BOOL	      sbm_ack_upto_n
		      (
			T_SBM_BUFFER_EXTIDX
		      );

EXTERN void	      sbm_srej_frame
		      (
			T_SBM_BUFFER_INDEX n
		      );

EXTERN void	      sbm_get_frame
		      (
			T_FRAME_DESC	   *frameDesc,
			T_SBM_BUFFER_INDEX *frameNo,
			BOOL		   *framesCouldBeSent,
			BOOL		   *ok
		      );

EXTERN void	      sbm_rej_from_n
		      (
			T_SBM_BUFFER_INDEX n,
			BOOL  *retransError
		      );

EXTERN BOOL	      sbm_frame_in_range
		      (
			T_SBM_BUFFER_INDEX n
		      );

EXTERN USHORT	      sbm_space_in_buf
		      (
			void
		      );

EXTERN void	     sbm_deinit
		      (
			void
		      );

EXTERN void	     sbm_set_retrans
		      (
			UBYTE n2
		      );

EXTERN void	     sbm_check_rej_count
		      (
			BOOL *retransError
		      );

EXTERN BOOL	     sbm_recall_prim
		      (
			T_SBM_PRIM **sendPrim
		      );

EXTERN void	     sbm_reset_after_remap
		      (
			USHORT		   newFrameSize,
			T_SBM_BUFFER_INDEX new_vs
		      );

/*
 * Receive Buffer Management RBM
 */
#ifdef OPTION_MULTITHREAD
  #define rbm_set_wind_size       _ENTITY_PREFIXED(rbm_set_wind_size)
  #define rbm_init                _ENTITY_PREFIXED(rbm_init)
  #define rbm_deinit              _ENTITY_PREFIXED(rbm_deinit)
  #define rbm_reset               _ENTITY_PREFIXED(rbm_reset)
  #define rbm_reset_srej_slots    _ENTITY_PREFIXED(rbm_reset_srej_slots)
  #define rbm_reset_all_r_states  _ENTITY_PREFIXED(rbm_reset_all_r_states)
  #define rbm_check_slots_srej    _ENTITY_PREFIXED(rbm_check_slots_srej)
  #define rbm_set_rslot_wait      _ENTITY_PREFIXED(rbm_set_rslot_wait)
  #define rbm_set_rslot_wait2srej _ENTITY_PREFIXED(rbm_set_rslot_wait2srej)
  #define rbm_set_rslot_rcvd      _ENTITY_PREFIXED(rbm_set_rslot_rcvd)
  #define rbm_get_current_frame   _ENTITY_PREFIXED(rbm_get_current_frame)
  #define rbm_accept_current_frame _ENTITY_PREFIXED(rbm_accept_current_frame)
  #define rbm_mark_missing_i_frames_srej  _ENTITY_PREFIXED(rbm_mark_missing_i_frames_srej)
  #define rbm_count_missing_i_frames      _ENTITY_PREFIXED(rbm_count_missing_i_frames)
  #define rbm_buffer_all_in_sequence_frames _ENTITY_PREFIXED(rbm_buffer_all_in_sequence_frames)
  #define rbm_ns_check            _ENTITY_PREFIXED(rbm_ns_check)
  #define rbm_get_vr              _ENTITY_PREFIXED(rbm_get_vr)
  #define rbm_get_prim            _ENTITY_PREFIXED(rbm_get_prim)
  #define rbm_get_curr_prim       _ENTITY_PREFIXED(rbm_get_curr_prim)
  #define rbm_move_current_frame  _ENTITY_PREFIXED(rbm_move_current_frame)
  #define rbm_is_state_wait       _ENTITY_PREFIXED(rbm_is_state_wait)
  #define rbm_prepare_remap       _ENTITY_PREFIXED(rbm_prepare_remap)

#ifdef _SIMULATION_
  #define rbm_store_frame           _ENTITY_PREFIXED(rbm_store_frame)
#endif
#endif

EXTERN BOOL	      rbm_set_wind_size
		      (
			T_RBM_BUFFER_INDEX windSize
		      );

EXTERN void	      rbm_init
		      (
			T_RBM_BUFFER_INDEX windSize,
			USHORT		   frameSize,
			USHORT		   framesPerPrim
		      );

EXTERN void	      rbm_deinit
		      (
			void
		      );

EXTERN void rbm_reset(T_RBM *rbm);

EXTERN void	      rbm_reset_srej_slots
		      (
			void
		      );

EXTERN void	      rbm_reset_all_r_states
		      (
			void
		      );

EXTERN T_RBM_BUFFER_EXTIDX rbm_check_slots_srej
			   (
			     void
			   );

EXTERN void	      rbm_set_rslot_wait
		      (
			T_RBM_BUFFER_INDEX slot
		      );

EXTERN void	      rbm_set_rslot_wait2srej
		      (
			T_RBM_BUFFER_INDEX slot
		      );

EXTERN void	      rbm_set_rslot_rcvd
		      (
			T_RBM_BUFFER_INDEX  slot,
			BOOL		   *resetTimer
		      );

EXTERN T_RBM_FRAMEPTR rbm_get_current_frame
		      (
			void
		      );

EXTERN void	      rbm_accept_current_frame
		      (
			void
		      );

EXTERN void	      rbm_mark_missing_i_frames_srej
		      (
			T_RBM_BUFFER_INDEX ns
		      );

EXTERN void	      rbm_count_missing_i_frames
		      (
			T_RBM_BUFFER_INDEX  ns,
			T_RBM_BUFFER_INDEX *count
		      );

EXTERN void	      rbm_buffer_all_in_sequence_frames
		      (
			T_RBM_BUFFER_INDEX  fromSlot,
			BOOL		   *primIsReady,
			BOOL		   *rcvReady,
			BOOL		   *rcvFull
		      );

EXTERN void	      rbm_ns_check
		      (
			T_RBM_BUFFER_INDEX  ns,
			BOOL		   *valid,
			BOOL		   *expected
		      );

EXTERN T_RBM_BUFFER_INDEX rbm_get_vr
			  (
			    void
			  );

EXTERN BOOL	      rbm_get_prim
		      (
			T_RBM_PRIM **prim,
			BOOL	   *rcvReady,
			BOOL	   *rcvFull
		      );

EXTERN T_RBM_PRIM    *rbm_get_curr_prim
		      (
			void
		      );

EXTERN void	      rbm_move_current_frame
		      (
			T_RBM_BUFFER_INDEX slot
		      );

EXTERN BOOL	      rbm_is_state_wait
		      (
			T_RBM_BUFFER_INDEX slot
		      );

EXTERN T_FRAME_NUM    rbm_prepare_remap
		      (
			USHORT frameSize
		      );

#ifdef _SIMULATION_
EXTERN void	rbm_store_frame(T_RBM_FRAMEPTR frame);
#endif

/*
 * SREJ Timer Management SRM
 */
#ifdef OPTION_MULTITHREAD
  #define srm_init           _ENTITY_PREFIXED(srm_init)
  #define srm_deinit         _ENTITY_PREFIXED(srm_deinit)
  #define srm_reset          _ENTITY_PREFIXED(srm_reset)
  #define srm_clear          _ENTITY_PREFIXED(srm_clear)
  #define srm_count          _ENTITY_PREFIXED(srm_count)
  #define srm_get_counter      _ENTITY_PREFIXED(srm_get_counter)
#endif

EXTERN void	      srm_init
		      (
			void
		      );

EXTERN void	      srm_deinit
		      (
			void
		      );

EXTERN void	      srm_reset
		      (
			void
		      );

EXTERN void	      srm_clear
		      (
			T_FRAME_NUM n
		      );

EXTERN void	      srm_count
		      (
			T_FRAME_NUM n
		      );

EXTERN T_COUNTER      srm_get_counter
		      (
			T_FRAME_NUM n
		      );

/*
 *  timer
 */
#define TIMERSTART(i,v)  vsi_t_start(VSI_CALLER i,v);
#define TIMERSTOP(i)	 vsi_t_stop(VSI_CALLER i);
 /*
 *  time
 *  xid parameters are given in 10^2 s, we store in 10^3 s (ms)
 */
#define TIME_INT2EXT(t) ((t) / 10)
#define TIME_EXT2INT(t) ((t) * 10)

/*
 * Communication handles
 */
#ifdef OPTION_MULTITHREAD
  #define hCommL2R     _ENTITY_PREFIXED(hCommL2R)
#ifdef _SIMULATION_
  #define hCommRA      _ENTITY_PREFIXED(hCommRA)
#endif
#endif

#ifdef RLP_PEI_C

GLOBAL T_HANDLE rlp_handle;
GLOBAL T_HANDLE hCommL2R = VSI_ERROR;	 /* L2R Communication */

#ifdef _SIMULATION_
GLOBAL T_HANDLE hCommRA  = VSI_ERROR;	 /* RA	Communication */
#endif

#else
EXTERN T_HANDLE  rlp_handle;
EXTERN T_HANDLE  hCommL2R;	 /* L2R Communication */

#ifdef _SIMULATION_
EXTERN T_HANDLE  hCommRA;	 /* RA	Communication */
#endif

#endif

GLOBAL void rlp_exec_timeout (USHORT index);

#ifdef _SIMULATION_

#define HEX_BYTE(B,H) { UBYTE b, *a = (UBYTE*)H;\
        b = (((UBYTE)B) >> 4) & 0x0F;\
        if (b > 9) b += 'A'-10; else b += '0'; *a = b;\
        b = ((UBYTE)B) & 0x0F;\
        if (b > 9) b += 'A'-10; else b += '0'; *(a+1) = b;}

#endif

#if defined (TRACE_INTERPRETED_RLP_FRAME) || defined (TRACE_RLP_FRAME)

#define DEC_BYTE(B,A) {UBYTE b, rep, *a = ((UBYTE*)A)+2; b = (UBYTE) B;\
        for (rep=0;rep<3;rep++) {*a = (b%10) + '0'; b /= 10; a--;}}

#endif

#endif
