/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  l2r.h
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
|             L2R
+----------------------------------------------------------------------------- 
*/ 

#ifndef L2R_H
#define L2R_H

/*
 * Conditional Compiling
 */

#define L2R_TRACE_FLOW
 
/*
 * instance management
 */
#define GET_INSTANCE(p) &l2r_data_base[0]
#define L2R_INSTANCES   1

/*
 * Types
 */

typedef enum
{
  NONTRANSP = 0,
  TRANSP    = 1
} T_MODE;

/*
 * Constants
 */

/*
 * information for dti library
 */
#define L2R_DTI_UP_INTERFACE      0
#define L2R_DTI_UP_CHANNEL        0
#define L2R_DTI_UP_DEF_INSTANCE   0
#define L2R_DTI_UPLINK_QUEUE_SIZE 0
#define L2R_BREAK_LEN             2

#define TIMER_TUP_SND    1

/*
 * global unchangeable constants
 */

#define DATA_SIZE_SHORT        25     /* num of data bytes in l2r frame (short format) */
#define DATA_SIZE_LONG         67     /* num of data bytes in l2r frame (long format)*/

#define LL_TO   TUP_SND_VALUE /* default value for timer UP_SND (lower layer waiting for data) */

 /*
 * derived constants
 */
#define HEADER_LEN    (RLP_HEADER_LEN/8) /* 16 / 8 = 2 */
#define TRAILER_LEN   0 /* No CRC -> no trailer */
#define HT_LEN        (HEADER_LEN + TRAILER_LEN)

#define RLP_FRAME_SIZE_SHORT  (DATA_SIZE_SHORT + HT_LEN)
#define RLP_FRAME_SIZE_LONG   (DATA_SIZE_LONG  + HT_LEN)

#define L2R_FRAMES_PER_PRIM_MAX  ((L2R_BYTES_PER_PRIM_MAX + RLP_FRAME_SIZE_SHORT - 1) / RLP_FRAME_SIZE_SHORT)

#define L2R_ENCODING_OFFSET (HEADER_LEN)        /* Offset L2R frame in RLP frame in bytes */

#define MAX_UP_REMAP_QUEUE_SIZE (MAX_UP_PRIMS + L2R_K_MS_IWF_MAX + 1)
#define UP_REMAP_BUFFER_SIZE  20

/* global user specific constants */

#define MAX_UP_PRIMS        17  /* Maximum number of primitives, which L2R holds for sending, */
#define MAX_DL_PRIMS        17  /* Maximum number of primitives, which L2R holds for receiving */
                                /* 17 prims * 10 Frames * 25 Bytes * 8 Bit = 34000 Bit (>32kBit)*/
                                /* if there are less than 10 frames per primitive, then the maximum */
                                /* buffer size will be smaller. */

#define UP_RIBU_PREALLOC    2   /* Number of primitives to be preallocated (minimum: 2) */
#define MIN_RIBU_SIZE       3   /* Minimum ring buffer size (send and receive) */
                                /* minimum: UP_RIBU_PREALLOC+1 */

/* constants for L2RCOP status octets */

#define SO_ADR_MASK           0x1F  /* 0b00011111 */
#define SO_ADR_MASK_TWO_OCT   0x7F  /* 0b01111111 */

#define SO_TWO_OCTET 27 /* 0b11011 */
#define SO_BREAK_ACK 28 /* 0b11100 */
#define SO_BREAK_REQ 29 /* 0b11101 */
#define SO_END_FULL  30 /* 0b11110 */
#define SO_END_EMPTY 31 /* 0b11111 */

#define SO_SA_BIT 7
#define SO_SB_BIT 6
#define SO_X_BIT  5

#define SO_SA_BIT_MASK 0x80
#define SO_SB_BIT_MASK 0x40
#define SO_X_BIT_MASK  0x20

#define SO_STATUS_BITS_MASK   (SO_SA_BIT_MASK + SO_SB_BIT_MASK)

#define GET_SO_SA_BIT(oct)    (((oct) & SO_SA_BIT_MASK) >> SO_SA_BIT)
#define GET_SO_SB_BIT(oct)    (((oct) & SO_SB_BIT_MASK) >> SO_SB_BIT)
#define GET_SO_X_BIT(oct)     (((oct) & SO_X_BIT_MASK)  >> SO_X_BIT)

/*
#define CLR_BIT(oct, bit)     (oct &= ~(1<<bit))
*/
#define SET_BIT(oct, bit)     (oct |=  (1<<bit))

/*
 * Bitoffset for encoding/decoding
 */
#define ENCODE_OFFSET        0

#ifndef NTRACE

/* for trace purposes */

#define SERVICE_NAME_DTI    "DTI"
#define SERVICE_NAME_MGT    "MGT"
#define SERVICE_NAME_CONIND "CONIND"
#define SERVICE_NAME_BREAK  "BREAK"
#define SERVICE_NAME_UP     "UP"
#define SERVICE_NAME_UP_UL  "UP_UL"
#define SERVICE_NAME_UP_LL  "UP_LL"
#define SERVICE_NAME_DN     "DN"
#define SERVICE_NAME_DN_UL  "DN_UL"
#define SERVICE_NAME_DN_LL  "DN_LL"

#endif

#define DTI     dti.

#define MGT     mgt.
#define CONIND  mgt.ConIndState.
#define BREAK   mgt.BreakState.

#define UP      up.
#define UP_UL   up.UL_State.
#define UP_LL   up.LL_State.

#define DN      dn.
#define DN_UL   dn.UL_State.
#define DN_LL   dn.LL_State.

/*
 * States of the entity process MGT
 */
#define MGT_DETACHED                0
#define MGT_PENDING_ATTACH          1
#define MGT_ATTACHED                2
#define MGT_PENDING_CONNECT         3
#define MGT_CONNECTED               4
#define MGT_DISCONNECT_INITIATED    5
#define MGT_PENDING_DETACH          6
#define MGT_PERMANENT_ERROR         7

/*
 * States of the entity process DN
 */
#define DN_DISCONNECTED             0
#define DN_CONNECTED                1
#define DN_WAIT_FIRST_DATA          2

/*
 * States of the entity process UP
 */
#define UP_DISCONNECTED             0
#define UP_CONNECTED                1
#define UP_REMAP                    2

/*==== TYPES ======================================================*/

/**********************************************************************************/
/*
 * L2R global typedefs
 */

#define IW_IDLE 0
#define IW_WAIT 1

#define ISW_IDLE 0
#define ISW_SEND 1
#define ISW_WAIT 2

#define FL_INACTIVE  0
#define FL_ACTIVE    1
#define FL_INVALID   2

typedef UBYTE  T_FLOW;
typedef UBYTE  T_BIT;
typedef UBYTE  T_PRIM_INDEX;
typedef UBYTE  T_UP_REMAP_PRIM_INDEX;
typedef UBYTE* T_P_UBYTE;
/*
typedef UBYTE  T_L2R_FRAME_SHORT[DATA_SIZE_SHORT];
*/
typedef UBYTE T_L2R_FRAME_LONG[DATA_SIZE_LONG];

typedef T_L2R_FRAME_LONG*     T_P_L2R_FRAME;
typedef T_P_L2R_FRAME         T_ADR_VECTOR[L2R_FRAMES_PER_PRIM_MAX];
typedef T_ADR_VECTOR*         T_P_ADR_VECTOR;
typedef T_RLP_REMAP_DATA_IND* T_P_RLP_REMAP_DATA_IND;

typedef struct
{
  T_PRIM_INDEX            nFr;        /* number of entries in vectors */
  T_ADR_VECTOR            adr;        /* array of adress vectors */
  T_PRIM_INDEX            index;      /* vector index of current frame */
  UBYTE                   offset;     /* offset in current frame */
  UBYTE                   off_status; /* offset of status octet in current frame */
  T_P_RLP_REMAP_DATA_IND  prim;       /* pointer to primitive (for FREE etc.) */
} T_RPRIM_DESCRIPTOR;                 /* remap primitive descriptor */

/**********************************************************************************/
/*
 * Process downlink specific types
 */

typedef T_RLP_DATA_IND*    T_P_RLP_DATA_IND;

typedef struct
{
  T_PRIM_INDEX      nFr;        /* number of entries in vectors */
  T_P_ADR_VECTOR    dadr;       /* pointer to adress vector */
  T_PRIM_INDEX      index;      /* vector index of current frame */
  UBYTE             offset;     /* offset in current frame */
  UBYTE             off_status; /* offset of status octet in current frame */
  T_P_RLP_DATA_IND  prim;       /* pointer to primitive (for FREE etc.) */
} T_DPRIM_DESCRIPTOR;

typedef T_DPRIM_DESCRIPTOR*  T_P_DPRIM_DESCRIPTOR;

typedef UBYTE T_PRIM_DESC_RIBU_INDEX;
typedef UBYTE T_PRIM_DESC_RIBU_SIZE;

/*
 * D o w n l i n k   R i n g   B u f f e r
 *
 * The Downlink Ring Buffer is an ordinary ring buffer. The L2RCOP entity
 * writes received primitives into the buffer, the relay entity reads them
 * from the buffer, and then the L2RCOP entity frees the primitive.
 */


/*
 * Ring buffer is by one bigger than max. number of prims,
 * because it needs always one empty entry
 */
#define MAX_DPRIM_RIBU_SIZE     (MAX_DL_PRIMS + 1)

typedef struct
{
  /*
  The field read may be written only by the relay entity.
  The other fields may be written only by the L2RCOP entity.
  If the relay entity finds a NULL pointer at the read position,
  then it doesn't read any data.
  */
  T_PRIM_DESC_RIBU_INDEX  free; /* current free index in primDesc array */
  T_RIBU                  idx;
  T_P_DPRIM_DESCRIPTOR    _primDesc[MAX_DPRIM_RIBU_SIZE];
} T_DPRIM_DESC_RIBU;


/**********************************************************************************/
/*
 * Process uplink specific types
 */

typedef T_RLP_DATA_REQ*    T_P_RLP_DATA_REQ;

typedef struct
{
  /* The fields index, offset and full may be written only by the relay entity */
  /* The other fields may be written only by the L2RCOP entity */

  T_PRIM_INDEX      nFr;        /* number of entries in vectors */
  T_P_ADR_VECTOR    adr;        /* pointer to adress vector */
  T_PRIM_INDEX      index;      /* vector index of current frame */
  UBYTE             offset;     /* offset in current frame */
  UBYTE             off_status; /* offset of status octet in current frame */
  BOOL              full;       /* primitive is filled up and ready for sending */
  T_FLOW            sa;         /* last sa bit in this primitive */
  T_FLOW            sb;         /* last sb bit in this primitive */
  T_P_RLP_DATA_REQ  prim;       /* pointer to primitive (for FREE etc.) */
} T_UPRIM_DESCRIPTOR;

typedef T_UPRIM_DESCRIPTOR*  T_P_UPRIM_DESCRIPTOR;

/*
 * U p l i n k   R i n g   B u f f e r
 *
 *  The L2RCOP entity can take a primitive away from the relay entity. This
 *  happens, when there are not enough data to be sent and the timer expires.
 *  In this case the L2RCOP entity takes the half ready primitive and sends
 *  it to RLP
 *
 *  The handling of the ring buffer is as follows:
 *
 *  *   The L2RCOP entity allocates a number of primitives. The slot in which
 *      the next primitive has to be allocated is indicated by the alloc index.
 *      Besides the primitive, which is just beeing filled by the relay
 *      entity, there should be one complete empty primitive ready. This
 *      means that the alloc index is at least by two greater than the write
 *      index, unless the L2RCOP entity can not provide new primitives quick
 *      enough.
 *  *   The relay entity writes data into the primitive indicated by the write
 *      index.
 *  *   When a primitive is filled up, the relay advances the write pointer and
 *      sends a DTI2_DATA_REQ primitive to the L2RCOP entity.
 *  *   When the L2RCOP entity receives the DTI2_DATA_REQ primitive it takes the
 *      primitive at the read position (which usually is identical with the
 *      previous write position), and advances the read index.
 *      Then it sends the primitive to the lower layer,
 *      and checks, if a new primitive has to be allocated. 
 *  *   When a timeout occurs, the L2RCOP entity locks the complete buffer.
 *      If no primitive is in the ring buffer (i.e read = write = alloc), a
 *      new primitive is allocated and sent as an empty primitive to the lower
 *      layer. If a completely filled primitive is in the buffer (i.e write /=
 *      read), this primitive is sent in the regular way. Otherwise there is
 *      a partial filled or empty primitive in the buffer (write /= alloc).
 *      Then this primitive is sent and the write index is incremented. In the
 *      case of a partial filled primitive the last uncompleted L2R status
 *      octet has to be finished before sending the primitive. Finally it
 *      is checked, if a new primitive has to be allocated.
 *  *   When a break is requested from the upper layer, the data are discarded
 *      and a BREAK signal is put into a new primitive and sent to the lower layer.
 *      Then the buffer is new initialised.
 */

/*
 * Ring buffer is by one bigger than max. number of prims, because it
 * needs always one empty entry
 */
#define MAX_UPRIM_RIBU_SIZE (MAX_UP_PRIMS+1)

typedef struct
{
  /*
  The field write may be written only by the relay entity.
  The other fields may be written only by the L2RCOP entity.
  If the relay entity finds a NULL pointer at the write position,
  then it doesn't write any data.
  */
  T_PRIM_DESC_RIBU_INDEX    alloc;          /* current alloc index in primDesc array */
  T_RIBU                    idx;
  T_P_UPRIM_DESCRIPTOR      primDesc[MAX_UPRIM_RIBU_SIZE];
} T_UPRIM_DESC_RIBU;


/**********************************************************************************/
/*
 * Relay entity specific types
 */


/**********************************************************************************/
/*
 * process global data
 */

/**********************************************************************************/

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *name;
  char *state_name;
#endif
} T_SUB_STATE;

/*
 * data for process management
 */

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *name;
  char *state_name;
#endif

  T_SUB_STATE BreakState;
  T_SUB_STATE ConIndState;    /* IW_WAIT: RLP_CONNECT_IND is pending during MGT_PENDING_ATTACH*/
  
  BOOL        Connected;      /* Indicates, that L2R is in the connected state */
  BOOL        FlowCtrlUsed;   /* Flow Control is used (no COPnoFlCt) */

  T_PRIM_DESC_RIBU_SIZE    RiBuSize;       /* Size of ring buffers (number of primitives) */
  
  ULONG       link_id;        /* Channel identifier             */
  UBYTE       InstID;         /* instance identifier */
  USHORT      FrameSize;      /* Size of frame, (RLP_FRAME_SIZE_LONG or RLP_FRAME_SIZE_SHORT) */
  UBYTE       FramesPerPrim;  /* Number of frames per primitive */
  ULONG       ConnectPrimType;/* {L2R_CONNECT_CNF, L2R_CONNECT_IND} */
} T_MGT;

/**********************************************************************************/
/*
 * data for process downlink
 */

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *name;
  char *state_name;
#endif

  T_SUB_STATE               UL_State;     /* State of upper layer (only for test environment) */
                                          /* WAIT: Upper layer is waiting for data from L2R */

  T_SUB_STATE               LL_State;     /* State of lower layer */
                                          /* WAIT: L2R has requested data from LL and is still waiting for it */
  
  BOOL                      FlowCtrlUsed; /* Flow control is used */
  UBYTE                     InstID;       /* instance number */
  ULONG                     link_id;      /* Channel identifier             */
  BOOL                      DtiConnected; /* DTI is connected */
  T_FLOW                    DnFlow;       /* Flow control state for downlink data transfer */
  T_FLOW                    UpFlow;       /* Flow control state for uplink data transfer */
  T_FLOW                    ULFlow;       /* Flow control state of upper layer */
  T_FLOW                    LLFlow;       /* Flow control state of lower layer */
  T_FLOW                    MrgFlow;      /* Merged flow control state = LLFlow + UpFlow */
#ifdef L2R_TRACE_FLOW
  T_FLOW                    LastSentFlow; /* Last flow control state, sent to DTI */
#endif
  BOOL                      ReportMrgFlow;/* Merged flow has changed and must be reported to upper layer */
  T_PRIM_DESC_RIBU_INDEX    FlowThresh;   /* Threshold for flow control */
  UBYTE                     LastState;    /* Last received state (sa, sb, x as in status octet) */
  
  T_DTI2_DATA_IND           *Brk_dti_data_ind; /* data indication for relaying break indication */

  T_DPRIM_DESC_RIBU         RiBu;
  T_DPRIM_DESCRIPTOR        PrimDesc[MAX_DPRIM_RIBU_SIZE];
  T_ADR_VECTOR              AdrVec[MAX_DPRIM_RIBU_SIZE];

} T_DN;

/**********************************************************************************/
/*
 * data for process uplink
 */

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char *name;
  char *state_name;
#endif

  T_SUB_STATE             UL_State;        /* State of upper layer (only for test environment) */
                                           /* IW_'WAIT': L2R is waiting for data from upper layer*/

  T_SUB_STATE             LL_State;        /* State of lower layer */
                                           /* ISW_WAIT: LL has requested data and is still waiting for it */
                                           /* ISW_SEND: data are ready for sending, but no request from LL */

  BOOL                    FlowCtrlUsed;    /* Flow control is used */
  UBYTE                   InstID;          /* instance number */
  ULONG                   link_id;         /* Channel identifier */
  BOOL                    DtiConnected;    /* DTI is connected */
  T_FLOW                  DnFlow;          /* Flow control state for downlink data transfer */
  T_FLOW                  UpFlow;          /* Flow control state for uplink data transfer */
  T_FLOW                  ULFlow;          /* Flow control state of upper layer */
  T_FLOW                  LLFlow;          /* Flow control state of lower layer */
  T_FLOW                  MrgFlow;         /* Merged flow control state = UP_ULFlow + UP_DNFlow */
  T_PRIM_DESC_RIBU_INDEX  FlowThreshLo;    /* Low Threshold for flow control */
  T_PRIM_DESC_RIBU_INDEX  FlowThreshHi;    /* High Threshold for flow control */
  USHORT                  DataSize;	   	   /* Size of data, (calculated from FrameSize) */
  USHORT                  FrameSize;       /* Size of frame, (RLP_FRAME_SIZE_LONG or RLP_FRAME_SIZE_SHORT) */
  USHORT                  OldFrameSize;    /* Size of RLP frame (before REMAP command) */
  UBYTE                   FramesPerPrim;
  UBYTE                   LastRcvdSa;      /* Last received value of sa */
  UBYTE                   LastRcvdSb;      /* Last received value of sb */
  UBYTE                   LastSentSa;      /* Last sent value of sa */
  UBYTE                   LastSentSb;      /* Last sent value of sb */
  T_FLOW                  LastSentFlow;    /* Last sent value of flow control */

  T_DTI2_DATA_REQ        *Prim;            /* For DTI interface to hold last received primitive */

  T_UPRIM_DESC_RIBU       RiBu;
  T_UPRIM_DESCRIPTOR      PrimDesc[MAX_UPRIM_RIBU_SIZE];
  T_ADR_VECTOR            AdrVec[MAX_UPRIM_RIBU_SIZE];

  BOOL                    StoreDataActive; /* prevents recursive calls of up_store_data */
  BOOL                    UrgentMsg;       /* A BREAK primitive is waiting to be sent */
  BOOL                    DiscardRemapData;/* Data in RLP_REMAP_DATA_IND have to be discarded */
  T_RPRIM_DESCRIPTOR      QRemapPrimDesc;
  T_P_RLP_REMAP_DATA_IND  QRemap[MAX_UP_REMAP_QUEUE_SIZE]; /* Queue for primitives, which have to be remapped */
  T_UP_REMAP_PRIM_INDEX   QRemapRead;      /* Read index for REMAP Queue */
  T_UP_REMAP_PRIM_INDEX   QRemapWrite;     /* Write index for REMAP Queue */
  UBYTE                   BRemap[UP_REMAP_BUFFER_SIZE]; /* Char. buffer for copying data from REMAP Queue */
  T_P_UBYTE               BRemapAdr;       /* Start address in remap buffer */
  USHORT                  BRemapLen;       /* Number of bytes in remap buffer */
  UBYTE                   BRemapSa;        /* Value of SA in remap buffer */
  UBYTE                   BRemapSb;        /* Value of SB in remap buffer */
  UBYTE                   BRemapLastState; /* Last state in remap buffer (sa, sb, x as in status octet) */
} T_UP;

/**********************************************************************************/
/*
 * data for relay entity
 */

/*
 * entity data for each L2R instance
 */

typedef struct
{
  T_SUB_STATE dti; /* state variable for DTI library */
  T_MGT mgt;       /* process management */
  T_DN  dn;        /* process downlink */
  T_UP  up;        /* process uplink */
} T_L2R_DATA;


/*==== EXPORT =====================================================*/
/*
 * data base
 */

EXTERN T_L2R_DATA  l2r_data_base [];
EXTERN T_L2R_DATA *l2r_data;
EXTERN ULONG       l2r_data_magic_num;

#define L2R_DATA_MAGIC_NUM (('L'<<24) + ('2'<<16) + ('R'<<8))   /* "L2R",NUL */

#define ENTITY_DATA l2r_data

/*
 *  Prototypes
 *
 *  L2R MANAGEMENT
 *
 *  MANAGEMENT primitive processing
 */


#ifdef OPTION_MULTITHREAD
  #define mgt_l2r_connect_cnf         _ENTITY_PREFIXED(mgt_l2r_connect_cnf)
  #define mgt_l2r_activate_req        _ENTITY_PREFIXED(mgt_l2r_activate_req)
  #define mgt_l2r_deactivate_req      _ENTITY_PREFIXED(mgt_l2r_deactivate_req)
  #define mgt_l2r_connect_req         _ENTITY_PREFIXED(mgt_l2r_connect_req)
  #define mgt_l2r_disc_req            _ENTITY_PREFIXED(mgt_l2r_disc_req)
  #define mgt_l2r_dti_req             _ENTITY_PREFIXED(mgt_l2r_dti_req)
  
  #define mgt_rlp_detach_cnf          _ENTITY_PREFIXED(mgt_rlp_detach_cnf)
  #define mgt_rlp_connect_ind         _ENTITY_PREFIXED(mgt_rlp_connect_ind)
  #define mgt_rlp_connect_cnf         _ENTITY_PREFIXED(mgt_rlp_connect_cnf)
  #define mgt_rlp_disc_ind            _ENTITY_PREFIXED(mgt_rlp_disc_ind)
  #define mgt_rlp_disc_cnf            _ENTITY_PREFIXED(mgt_rlp_disc_cnf)
  #define mgt_rlp_reset_ind           _ENTITY_PREFIXED(mgt_rlp_reset_ind)
  #define mgt_rlp_reset_cnf           _ENTITY_PREFIXED(mgt_rlp_reset_cnf)
  #define mgt_rlp_ui_ind              _ENTITY_PREFIXED(mgt_rlp_ui_ind)
  #define mgt_rlp_xid_ind             _ENTITY_PREFIXED(mgt_rlp_xid_ind)
  #define mgt_rlp_error_ind           _ENTITY_PREFIXED(mgt_rlp_error_ind)
  #define mgt_rlp_statistic_ind       _ENTITY_PREFIXED(mgt_rlp_statistic_ind)
#endif

EXTERN void mgt_l2r_connect_cnf      (UBYTE                   nack);
EXTERN void mgt_l2r_activate_req     (T_L2R_ACTIVATE_REQ     *l2r_activate_req);
EXTERN void mgt_l2r_deactivate_req   (T_L2R_DEACTIVATE_REQ   *l2r_deactivate_req);
EXTERN void mgt_l2r_connect_req      (T_L2R_CONNECT_REQ      *l2r_connect_req);
EXTERN void mgt_l2r_disc_req         (T_L2R_DISC_REQ         *l2r_disc_req);
EXTERN void mgt_l2r_dti_req          (T_L2R_DTI_REQ          *l2r_dti_req);

EXTERN void mgt_rlp_detach_cnf       (T_RLP_DETACH_CNF       *rlp_detach_cnf);
EXTERN void mgt_rlp_connect_ind      (T_RLP_CONNECT_IND      *rlp_connect_ind);
EXTERN void mgt_rlp_connect_cnf      (T_RLP_CONNECT_CNF      *rlp_connect_cnf);
EXTERN void mgt_rlp_disc_ind         (T_RLP_DISC_IND         *rlp_disc_ind);
EXTERN void mgt_rlp_disc_cnf         (T_RLP_DISC_CNF         *rlp_disc_cnf);
EXTERN void mgt_rlp_reset_ind        (T_RLP_RESET_IND        *rlp_reset_ind);
EXTERN void mgt_rlp_reset_cnf        (T_RLP_RESET_CNF        *rlp_reset_cnf);
EXTERN void mgt_rlp_ui_ind           (T_RLP_UI_IND           *rlp_ui_ind);
EXTERN void mgt_rlp_xid_ind          (T_RLP_XID_IND          *rlp_xid_ind);
EXTERN void mgt_rlp_error_ind        (T_RLP_ERROR_IND        *rlp_error_ind);
EXTERN void mgt_rlp_statistic_ind    (T_RLP_STATISTIC_IND    *rlp_statistic_ind);


/*
 *  MANAGEMENT signal processing
 */
#ifdef OPTION_MULTITHREAD
  #define sig_dn_mgt_break_ind      _ENTITY_PREFIXED(sig_dn_mgt_break_ind)
  #define sig_dn_mgt_first_data_ind _ENTITY_PREFIXED(sig_dn_mgt_first_data_ind)
  #define sig_up_mgt_break_ind      _ENTITY_PREFIXED(sig_up_mgt_break_ind)
#endif

EXTERN void sig_dn_mgt_break_ind(T_BIT sa, T_BIT sb, T_FLOW flow);
EXTERN void sig_dn_mgt_first_data_ind(void);
EXTERN void sig_up_mgt_break_ind(T_DTI2_DATA_REQ  *dti_data_req);

/*
 *  MANAGEMENT procedures
 */

#ifdef OPTION_MULTITHREAD
  #define mgt_init                      _ENTITY_PREFIXED(mgt_init              )
  #define mgt_checkpar                  _ENTITY_PREFIXED(mgt_checkpar          )
  #define mgt_deinit_connection         _ENTITY_PREFIXED(mgt_deinit_connection )
  #define mgt_init_connection           _ENTITY_PREFIXED(mgt_init_connection   )
  #define mgt_send_l2r_error_ind        _ENTITY_PREFIXED(mgt_send_l2r_error_ind)
#endif

EXTERN void mgt_init(T_MGT *dmgt);
EXTERN BOOL mgt_checkpar(T_L2R_ACTIVATE_REQ *ar);
EXTERN void mgt_deinit_connection(void);
EXTERN void mgt_init_connection(BOOL indication);
EXTERN void mgt_send_l2r_error_ind(USHORT cause);

/*
 *  L2R DN
 *
 *  DN primitive processing
 */

#ifdef OPTION_MULTITHREAD
  #define dn_rlp_data_ind        _ENTITY_PREFIXED(dn_rlp_data_ind)
#endif

EXTERN void dn_rlp_data_ind       (T_RLP_DATA_IND       *rlp_data_ind);

/*
 *  DN signal processing
 */

#ifdef OPTION_MULTITHREAD
  #define sig_mgt_dn_conn_req       _ENTITY_PREFIXED(sig_mgt_dn_conn_req)
  #define sig_mgt_dn_disc_req       _ENTITY_PREFIXED(sig_mgt_dn_disc_req)
  #define sig_mgt_dn_break_req      _ENTITY_PREFIXED(sig_mgt_dn_break_req)
  #define sig_mgt_dn_clear_req      _ENTITY_PREFIXED(sig_mgt_dn_clear_req)
  #define sig_mgt_dn_reconn_req     _ENTITY_PREFIXED(sig_mgt_dn_reconn_req)
  #define sig_mgt_dn_dti_conn_setup _ENTITY_PREFIXED(sig_mgt_dn_dti_conn_setup)
  #define sig_up_dn_flow            _ENTITY_PREFIXED(sig_up_dn_flow)
  #define sig_up_dn_ul_flow         _ENTITY_PREFIXED(sig_up_dn_ul_flow)
  #define sig_mgt_dn_send_break_req _ENTITY_PREFIXED(sig_mgt_dn_send_break_req)
#endif

EXTERN void sig_mgt_dn_conn_req(BOOL flowCtrlUsed, T_PRIM_DESC_RIBU_SIZE riBuSize, UBYTE inst_id);

EXTERN void sig_mgt_dn_disc_req   (void);
EXTERN void sig_mgt_dn_break_req  (void);
EXTERN void sig_mgt_dn_clear_req  (void);
EXTERN void sig_mgt_dn_reconn_req (void);
EXTERN void sig_mgt_dn_dti_conn_setup (ULONG link_id);
EXTERN void sig_up_dn_flow        (T_FLOW);     /* Flow Control Active */
EXTERN void sig_up_dn_ul_flow     (T_FLOW);     /* Flow Control Active */
EXTERN void sig_mgt_dn_send_break_req();

/*
 *  DN procedures
 */

#ifdef OPTION_MULTITHREAD
  #define dn_init               _ENTITY_PREFIXED(dn_init               )
  #define dn_check_flow         _ENTITY_PREFIXED(dn_check_flow         )
  #define dn_send_data_ind      _ENTITY_PREFIXED(dn_send_data_ind      )
  #define dn_scan_break_req     _ENTITY_PREFIXED(dn_scan_break_req     )
  #define dn_free_all_prims     _ENTITY_PREFIXED(dn_free_all_prims     )
  #define dn_cond_free_prims    _ENTITY_PREFIXED(dn_cond_free_prims    )
  #define dn_store_prim         _ENTITY_PREFIXED(dn_store_prim         )
  #define dn_cond_report_status _ENTITY_PREFIXED(dn_cond_report_status )
  #define dn_init_ribu          _ENTITY_PREFIXED(dn_init_ribu          )
  #define dn_cond_req_data      _ENTITY_PREFIXED(dn_cond_req_data      )
  #define dn_merge_flow         _ENTITY_PREFIXED(dn_merge_flow         )
  #define dn_store_status       _ENTITY_PREFIXED(dn_store_status       )
  #define dn_cond_report_status _ENTITY_PREFIXED(dn_cond_report_status )
#endif

EXTERN void dn_init(T_DN *ddn);
EXTERN void dn_check_flow(void);
EXTERN void dn_send_data_ind(void);

EXTERN void               dn_scan_break_req
                          (
                            T_P_RLP_DATA_IND  data_ind,
                            BOOL             *found,
                            T_PRIM_INDEX     *index,
                            T_PRIM_INDEX     *frames,
                            T_PRIM_INDEX     *emptyfr,
                            T_BIT            *sa,
                            T_BIT            *sb,
                            T_FLOW           *flow_brk,
                            T_FLOW           *flow_gen
                          );

EXTERN void               dn_store_prim
                          (
                            T_P_RLP_DATA_IND  data_ind,
                            T_PRIM_INDEX      index
                          );

EXTERN void               dn_free_all_prims
                          (
                            void
                          );

EXTERN void               dn_cond_free_prims
                          (
                            void
                          );

EXTERN void               dn_init_ribu
                          (
                            void
                          );

GLOBAL void               dn_cond_report_status
                          (
                            void
                          );

EXTERN void               dn_cond_req_data
                          (
                            void
                          );

EXTERN void               dn_store_status
                          (
                             T_FLOW  flow
                          );

EXTERN void               dn_merge_flow
                          (
                            void
                          );

EXTERN void               dn_cond_report_status
                          (
                            void
                          );

/*
 *  L2R UP
 *
 *  UP primitive processing
 */

#ifdef OPTION_MULTITHREAD
  #define up_rlp_ready_ind           _ENTITY_PREFIXED(up_rlp_ready_ind)
#endif

EXTERN void up_rlp_ready_ind       (T_RLP_READY_IND *rlp_ready_ind);

/*
 *  UP signal processing
 */
#ifdef OPTION_MULTITHREAD
  #define sig_mgt_up_conn_req         _ENTITY_PREFIXED(sig_mgt_up_conn_req)
  #define sig_mgt_up_disc_req         _ENTITY_PREFIXED(sig_mgt_up_disc_req)
  #define sig_mgt_up_break_req        _ENTITY_PREFIXED(sig_mgt_up_break_req)
  #define sig_mgt_up_clear_req        _ENTITY_PREFIXED(sig_mgt_up_clear_req)
  #define sig_mgt_up_dti_conn_setup   _ENTITY_PREFIXED(sig_mgt_up_dti_conn_setup)
  #define sig_mgt_up_dti_conn_open    _ENTITY_PREFIXED(sig_mgt_up_dti_conn_open)
  #define sig_dn_up_flow              _ENTITY_PREFIXED(sig_dn_up_flow)
  #define sig_dn_up_ll_flow           _ENTITY_PREFIXED(sig_dn_up_ll_flow)
#endif


EXTERN void sig_mgt_up_conn_req
            (
              UBYTE                  framesPerPrim,
              USHORT                 frameSize,
              BOOL                   flowCtrlUsed,
              T_PRIM_DESC_RIBU_SIZE  riBufferSize,
              UBYTE                  inst_id
            );

EXTERN void sig_mgt_up_disc_req
            (
              void
            );

EXTERN void sig_mgt_up_break_req
            (
              T_BIT sa,
              T_BIT sb,
              T_FLOW flow
            );

EXTERN void sig_mgt_up_clear_req
            (
              void
            );

EXTERN void sig_mgt_up_dti_conn_setup (ULONG link_id);

EXTERN void sig_mgt_up_dti_conn_open();

EXTERN void sig_dn_up_flow
            (
              T_FLOW  flow
            );

EXTERN void sig_dn_up_ll_flow
            (
              T_FLOW  flow
            );

/*
 *  UP procedures
 */

#ifdef OPTION_MULTITHREAD
  #define up_init                       _ENTITY_PREFIXED(up_init               )
  #define up_send_prim_timeout          _ENTITY_PREFIXED(up_send_prim_timeout  )
  #define up_check_alloc                _ENTITY_PREFIXED(up_check_alloc        )
  #define up_check_flow                 _ENTITY_PREFIXED(up_check_flow         )
  #define up_store_data                 _ENTITY_PREFIXED(up_store_data         )
  #define up_send_ready                 _ENTITY_PREFIXED(up_send_ready         )
  #define up_deinit_ribu                _ENTITY_PREFIXED(up_deinit_ribu        )
  #define up_send_current_prim          _ENTITY_PREFIXED(up_send_current_prim  )
  #define up_send_prim_cond             _ENTITY_PREFIXED(up_send_prim_cond     )
  #define up_init_ribu                  _ENTITY_PREFIXED(up_init_ribu          )
  #define up_store_status               _ENTITY_PREFIXED(up_store_status       )
  #define up_send_empty_frame           _ENTITY_PREFIXED(up_send_empty_frame   )
  #define up_send_status                _ENTITY_PREFIXED(up_send_status        )
  #define up_merge_flow                 _ENTITY_PREFIXED(up_merge_flow         )
  #define up_rq_init                    _ENTITY_PREFIXED(up_rq_init            )
  #define up_some_data_to_send          _ENTITY_PREFIXED(up_some_data_to_send  )
#endif

#define sig_dti_mgt_connection_opened_ind                   \
        _ENTITY_PREFIXED(sig_dti_mgt_connection_opened_ind)
#define sig_dti_mgt_connection_closed_ind                   \
        _ENTITY_PREFIXED(sig_dti_mgt_connection_closed_ind)
#define sig_dti_dn_tx_buffer_full_ind                       \
        _ENTITY_PREFIXED(sig_dti_dn_tx_buffer_full_ind    )
#define sig_dti_dn_tx_buffer_ready_ind                      \
        _ENTITY_PREFIXED(sig_dti_dn_tx_buffer_ready_ind   )
#define sig_dti_up_data_received_ind                        \
        _ENTITY_PREFIXED(sig_dti_up_data_received_ind     )


EXTERN void up_init(T_UP *dup);

EXTERN void               up_send_prim_timeout
                          (
                            void
                          );

EXTERN void               up_check_alloc
                          (
                            void
                          );

EXTERN void               up_check_flow
                          (
                            void
                          );

EXTERN void               up_store_data
                          (
                            void
                          );

EXTERN void               up_send_ready
                          (
                            void
                          );

EXTERN void               up_deinit_ribu
                          (
                            void
                          );

EXTERN void               up_send_current_prim
                          (
                            void
                          );

EXTERN void               up_send_prim_cond
                          (
                            void
                          );

EXTERN void               up_init_ribu
                          (
                            void
                          );

EXTERN void               up_store_status
                          (
                             T_BIT  sa,
                             T_BIT  sb,
                             T_FLOW flow
                          );

EXTERN void               up_send_empty_frame
                          (
                             T_BIT  sa,
                             T_BIT  sb,
                             T_FLOW flow
                          );

EXTERN void               up_send_status
                          (
                             T_BIT  sa,
                             T_BIT  sb,
                             T_FLOW flow,
                             UBYTE adr
                          );

EXTERN void               up_merge_flow
                          (
                            void
                          );

EXTERN void               up_rq_init
                          (
                            void
                          );

EXTERN BOOL               up_some_data_to_send
                          (
                            void
                          );

/*
 * these functions are called by pei_sig_callback
 */
EXTERN void         sig_dti_mgt_connection_opened_ind();
EXTERN void         sig_dti_mgt_connection_closed_ind();

EXTERN void         sig_dti_dn_tx_buffer_full_ind();
EXTERN void         sig_dti_dn_tx_buffer_ready_ind();

EXTERN void         sig_dti_up_data_received_ind
                          (
                            T_DTI2_DATA_REQ *dti_data_req
                          );

/*
 *  timer
 */
#define TIMERSTART(i,v) vsi_t_start (VSI_CALLER i, v)
#define TIMERSTOP(i)    vsi_t_stop  (VSI_CALLER i);

/*
 * Communication handles
 */

#ifdef OPTION_MULTITHREAD
  #define hCommCTRL     _ENTITY_PREFIXED(hCommCTRL)
  #define hCommRLP      _ENTITY_PREFIXED(hCommRLP)
#ifdef _SIMULATION_
  #define hCommRA       _ENTITY_PREFIXED(hCommRA)
#endif
#endif /* OPTION_MULTITHREAD */

EXTERN DTI_HANDLE l2r_hDTI;     /* DTI connection handle for DTI library      */

EXTERN T_HANDLE  l2r_handle;
EXTERN T_HANDLE  hCommCTRL;      /* Controller Communication                  */
EXTERN T_HANDLE  hCommRLP;       /* RLP  Communication                        */

#ifdef _SIMULATION_
EXTERN T_HANDLE  hCommRA;        /* RA  Communication                         */
#endif

EXTERN T_MODE    l2r_data_mode;

 /*
  * MALLOC / FREE for old frame
  */

#endif
