/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRLC
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
|  Purpose :  Definitions for entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_H
#define GRLC_H

/*
 * defines the user of the vsi interface
 */
#define VSI_CALLER            GRLC_handle,
#define VSI_CALLER_SINGLE     GRLC_handle 

/*
 * Macros
 */

#define ENCODE_OFFSET   0  /* PALLOC SDU*/ 

/*
 * Bit and byte length for encoding/decoding
 */

#define BIT_UL_CTRL_BLOCK_MAC_HEADER  8
#define BIT_UL_CTRL_BLOCK_CONTENTS    176
#define BIT_UL_CTRL_BLOCK             (BIT_UL_CTRL_BLOCK_MAC_HEADER+BIT_UL_CTRL_BLOCK_CONTENTS)

#define BYTE_UL_CTRL_BLOCK            (BIT_UL_CTRL_BLOCK/BITS_PER_BYTE)


/*
 * The following definitions are used for the management of single block
 * sending without TBF establishment.
 *
 * TC holds a queue of size MSG_OWNER_MAX_NUM to buffer the single blocks
 * requested by the services. Only services MEAS and CS can request single
 * blocks in parallel. In case service CTRL has requested a single block,
 * services MEAS and CS are not allowed to request any single blocks.
 *
 * Service CS uses the same buffer as the service CTRL. Only one single block
 * can be stored for both services. The service MEAS has an individual buffer.
 * At this time only one single block can be stored for this service but we are
 * free to increase the size just by increasing the value of MAX_CTRL_BLK_NUM.
 */
#define MAX_CTRL_BLK_NUM    5 /* number of single blocks that can be stored at */
                              /* the same time                                 */

#define BLK_INDEX_CTRL      0 /* Packet Cell Change Failure                    */
#define BLK_INDEX_CS        0 /* Packet Measurement Report (NC Measurements)   */
#define BLK_INDEX_TM        1 /* Packet Resource Request (Reallocation)        */
#define BLK_INDEX_MEAS      2 /* Packet Measurement Report (EXT Measurements)  */


typedef UBYTE T_BLK_OWNER;

#define BLK_STATE_NONE      0 /* no control message associated                 */
#define BLK_STATE_ALLOCATED 1 /* control message is allocated                  */
#define BLK_STATE_SENT_REQ  2 /* control message is sent, waiting for response */

typedef UBYTE T_BLK_STATE;

/*
 * Constants
 */

/* 
 * Disable Get State traces only for target enviroment
 */

#ifdef _TARGET_
#undef GET_STATE
#define GET_STATE(P)        (ENTITY_DATA->P state)
#endif


/*
 * Define the size of the GRLC queue in frame
 */
#define PEI_PRIM_QUEUE_SIZE 32

/*
 * Defines the size of the queue which do a tempory save of llc primitives
 */

#ifdef _TARGET_
#define PRIM_QUEUE_SIZE  20
#else
#define PRIM_QUEUE_SIZE  10
#endif/*TARGET*/


#define GMM_PRIM_QUEUE_SIZE  2
#define MAX_WINDOW_SIZE      64


#define PRIM_QUEUE_SIZE_TOTAL  (PRIM_QUEUE_SIZE+GMM_PRIM_QUEUE_SIZE)

#define MAX_LLC_PDU_SIZE  1560
#define MAX_GRLC_USER_DATA (MAX_WINDOW_SIZE * 50) /* window size * user data with CS4 */
#define FN_MAX           0x297000 

/* MAC HEADER TYPE*/
#define RLC_DATA_BLOCK      0
#define CTRL_BLK_NO_OPT     1
#define CTRL_BLK_OPT        2

/*
 * Size of Next Poll Array
 */
#define NEXT_POLL_ARRAY_SIZE   8
#define WIN_SIZE              64  /*window size of rlc*/
/*
 * Size of Poll Type Array
 */
#define POLL_TYPE_ARRAY_SIZE     8




/*
TBF BSN RANGE
*/
#define NR_OF_PDUS_PER_RLCMAC_BLOCK   8 /* max. number of pdus in one rlc mac block */
#define TLLI_SIZE                     4 /* tlli size in UBYTE */ 

#define WIN_MOD     63
#define BSN_MAX    127

#define RD_LI_CNT_MAX                 8 /* max. number of pdus in one rlc mac block  in downlink direction*/


#define OFFSET_CTRL_BLOCK_IDX  200

/*
end constants for RU
*/
#define MAX_UL_TN      4 /* max. number of timeslots supported in uplink */ 

/*
start constants for RD
*/


#define RD_NEXT_ARRAY_DATA_SIZE       50 /* valid for all coding schemes */

#define ACK_CNT_MEAS_RPT_FIRST         2
#define ACK_CNT_NORMAL                 1

/*
end constants for RD
*/

/*
start constants for TM
*/
#define RADIO_PRIO_4         3

/*
end constants for TM
*/

/*
start constants for TPC
*/
#define MIN_PCL_DCS1800 28 /* minimum power control level DCS 1800 */
#define MAX_PCL_DCS1800 29 /* maximum power control level DCS 1800 */
#define MIN_PCL_GSM900  31 /* minimum power control level GSM 900  */
#define MAX_PCL_GSM900  0  /* maximum power control level GSM 900  */
#define MIN_PCL_PCS1900 21 /* minimum power control level PCS 1900 */
#define MAX_PCL_PCS1900 22 /* maximum power control level PCS 1900 */

#define GAMMA_0_DCS1800 36
#define GAMMA_0_GSM900  39
#define GAMMA_0_PCS1900 36

#define TPC_SWITCHED_ON        0xFF /* normal TPC process is applied  */
#define TPC_ALFGAM_NOT_PRESENT 0xFF /* the ALPHA and GAMMA values are */
                                    /* marked as not present          */

#define CLIP_T_AVG(x)           ( (x) > 25 ? 25 : (x) )
#define CLIP_ALPHA(x)           ( (x) > 10 ? 10 : (x) )
#define CLIP_BS_PAG_BLKS_RES(x) ( (x) > 12 ? 0  : (x) )
#define CLIP_BS_PRACH_BLKS(x)   ( (x) > 12 ? 0  : (x) )

#define M_TPC_TRACE_STATIC      0x01 /* trace TPC parameter everytime tpc_update_pch is called    */
#define M_TPC_TRACE_CHANGE      0x02 /* trace TPC parameter only in case at least one PCL changes */
#define M_TPC_TRACE_FUNCTION    0x04 /* trace mask for parameters of maca_power_control           */
/*
end constants for TPC
*/

/*
start constants for MEAS
*/
#define MEAS_ACRCY 1000 /* accuracy */ 
/*
end constants for MEAS
*/


#define NOT_SET 255

/*
 * Service definitions. Used to access service data with GET/SET_STATE.
 */


#define TM        tm.
#define RD        rd.
#define RU        ru.
#define GFF       gff.




/*
 * Timer definitions (also used by GRLC_PEI.C)
 */

typedef enum
{
  T3164 = 0,  /* handled by service RU         */
  T3166,      /* handled by service RU         */
  T3168,      /* handled by service TM         */
  T3180,      /* handled by service RU         */
  T3182,      /* handled by service RU         */
  T3184,      /* handled by service RU         */
  T3188,      /* handled by service TM         */
  T3190,      /* handled by service RD         */
  T3192,      /* handled by service RD         */
  T3314,      /* handled by service RU         */

  TIMER_COUNT /* !!! always the last entry !!! */    

} T_TIMER_INDEX;

/*#define T3198     no timer   */   /* handled by service RU: used relative to fn, defined in T_RLC_DATA_BLOCK: T3198 = BS_CV_MAX * block periods: not timer is used */


#define T3164_VALUE       5000        /* ms */ 
#define T3166_VALUE       5000        /* ms */
/*      T3168_VALUE - assigned in system info (in GPRS Cell Options)
                      range 500,1000,...4000ms */
#define T3180_VALUE       5000        /* ms */
#define T3182_VALUE       5000        /* ms */
#define T3184_VALUE       5000        /* ms */
#define T3188_VALUE       5000        /* ms */
#define T3190_VALUE       5000        /* ms */

/*#define T3192_VALUE - assigned in system info (in GPRS Cell Options)
                      range 500,1000,...4000ms */
/*      T3198_VALUE - value depends BS_CV_MAX, there may be some inctances of this timer,
                      how many? the duration is not very long (asumption) - how long?
                      handling with rlc_uplink/downlink functions and a table?   */

/*
 * Service name definitions for trace purposes. The service abbrevation
 * (e.g. RU) has to be the same as above for the service definitions.
 */
#ifndef NTRACE

#define SERVICE_NAME_TM       "TM"
#define SERVICE_NAME_RU       "RU"
#define SERVICE_NAME_RD       "RD"
#define SERVICE_NAME_GFF      "GFF"


#endif /* !NTRACE */



/*
 * Service TM
 */
#define TM_ACCESS_DISABLED          0
#define TM_ACCESS_PREPARED          1
#define TM_PIM                      2
#define TM_PAM                      3
#define TM_PTM                      4
#define TM_WAIT_4_PIM               5

/*
 * Service RD
 */

#define RD_NULL     0
#define RD_ACK      1
#define RD_REL_ACK  2
#define RD_UACK     3
#define RD_REL_UACK 4
#define RD_NET_REL  5
#define RD_WAIT_FOR_STARTING_TIME_ACK  6
#define RD_WAIT_FOR_STARTING_TIME_UACK 7
#define RU_NET_REL                     8

/*
 * Service RU
 */

#define RU_NULL     0  /* not active state of RU  */
#define RU_ACK      1  /* acknowledged mode state of RU */
#define RU_REL_ACK  2  /* release state in acknowledged mode of RU, i.e. cv=0 is transmitted but not acked */
#define RU_SEND_PCA 3  /* send packet control acknowledgment state of RU in acknowledged and unacknowledged  mode,
i.e all blocks are transmitted and acked, waiting for of FN for transmiitng the packet control ack message */
#define RU_UACK     4  /* unacknowledged mode state of RU */
#define RU_REL_UACK 5  /* release state in unacknowledged mode of RU, i.e. cv=0 is transmitted but not acked
							         (i.e receiving of packet uplink ack/nack with fbi=1) */
#define RU_WAIT_FOR_FIRST_CALL_ACK  6
#define RU_WAIT_FOR_FIRST_CALL_UACK 7

/*
 * Service GFF
 */


#define GFF_ACTIVE      0
#define GFF_DEACTIVE    1
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
#define GFF_TWO_PHASE   2
#endif


/*
 * states for grlc_data->tc.send_grlc_ready_ind
 */
#define SEND_A_GRLC_READY_IND    0
#define WAIT_FOR_LLC_DATA_REQ   1
#define PRIM_QUEUE_FULL         2

/* 
 * states for Ready Timer
 */
#define STANDBY_STATE                   0
#define READY_STATE                     1

typedef UBYTE T_READY_TIMER_STATE;

#define READY_TIMER_HANDLING_DISABLED   0
#define READY_TIMER_HANDLING_ENABLED    1

typedef UBYTE T_READY_TIMER_HANDLING;

/*==== TYPES ======================================================*/

/*
 * GRLC global typedefs
 */

/* enums */

typedef UBYTE T_PRIM_TYPE;


typedef enum
{
  CS_ZERO = 255,
  CS_1    = 0,
  CS_2    = 1,
  CS_3    = 2,
  CS_4    = 3
} T_CODING_SCHEME;



typedef enum
{ 
  TFI_CHANGE_NULL,    /* no tfi modification needed after starting time is elapsed */
  TFI_CHANGE_UL,      /* uplink tfi shall modified after starting time is elapsed */
  TFI_CHANGE_DL,      /* downlink tfi shall modified after starting time is elapsed */
  TFI_CHANGE_ALL      /* uplink and downlink tfi shall modified after starting time is elapsed */
} T_TFI_CHANGE;

typedef enum
{
  TBF_TYPE_NULL,
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  TBF_TYPE_TP_ACCESS,
#endif
  TBF_TYPE_UL,
  TBF_TYPE_DL,
  TBF_TYPE_CONC
} T_TBF_TYPE;

/* 
 * Please keep the numbering scheme, so that bit operations can be used.
 */
#define REL_TYPE_NULL  0 /* no release of TBF requested        */
#define REL_TYPE_DL    1 /* release of DL TBF requested        */
#define REL_TYPE_UL    2 /* release of UL TBF requested        */
#define REL_TYPE_DL_UL 3 /* release of DL and UL TBF requested */

typedef UBYTE T_REL_TYPE;




/*start enum for rd*/
typedef enum /* T_NEXT_POLL_BLOCK_TYPE*/
{
  NEXT_POLL_BLOCK_NONE,
  NEXT_POLL_BLOCK_DL_DATA,
  NEXT_POLL_BLOCK_CTRL
}T_NEXT_POLL_BLOCK_TYPE;

/*
 * used by rd : vn array (enum is using ulong)
 */
#define  VN_RECEIVED  1
#define  VN_INVALID   255

/*start enum for ru*/
typedef enum /* T_PAN_CNT*/
{
  PAN_INC,
  PAN_DEC
}T_PAN_CNT;

#define VB_NACKED           0  /* negative acknowledged */
#define VB_ACKED            1  /* positive acknowledged */
#define VB_PENDING_ACK      2  /* not jet acknowledged  */
#define VB_FIRST_SEG_TXD    3 
#define VB_INVALID        255  /* outside window        */


typedef enum /* T_LAST_BSN*/
{
  LAST_BSN_IS_SENT,                   /* final uplink data block(cv=0) is sent(confirmed by l1)*/
  LAST_BSN_STALL_CONDITION,           /* stall indication in uplink, ack or uack mode */
  LAST_BSN_RESUME_UACK_MODE_AFTER_SI, /* in rlc uack, data transfer is allowed after SI */
  LAST_BSN_NOT_BULIT =255             /* final data block is not sent */
}T_LAST_BSN;

/*end um for ru*/

/*start enum for GFF*/

typedef enum /* RLC_STATUS_TYPE*/
{
  RLC_STATUS_NULL = 0,
  RLC_STATUS_UL   = 1,
  RLC_STATUS_DL   = 2,
  RLC_STATUS_BOTH = 3
}T_RLC_STATUS_TYPE;
/*end um for GFF*/


/*
 * Structure
 */

typedef enum /* T_FA_TYPE*/
{
  FA_NO_CURRENT,
  FA_NO_NEXT,
  FA_BITMAP,
  FA_REPEAT
}T_FA_TYPE;

typedef struct  /* parameterts of one fixed alloction*/
{
  T_CGRLC_fix_alloc_struct    alloc;
  USHORT                      ul_res_sum;      /* number of uplink resources during this fixed allocation*/
  ULONG                       alloc_start_fn;
  ULONG                       alloc_end_fn;
} T_FA_ALLOC;


typedef struct  /* T_FA_MANAG*/
{
  T_FA_TYPE           fa_type;
  T_FA_ALLOC          current_alloc;
  T_FA_ALLOC          next_alloc;
  BOOL                repeat_alloc;
  USHORT              tbf_oct_cnt;        /* number of octets, to be transmitted during the current tbf*/
  USHORT              tbf_oct_cap_remain; /* remainig capacity in octetes of current fix allocation*/
  USHORT              ul_res_used;        /* number of used uplink resources(timeslots) used in fixed allocation*/
  USHORT              ul_res_remain;      /* number of REMAINING uplink resources(timeslots) in fixed allocation*/
  USHORT              ul_res_sum;         /* number of total uplink resources in fix alloc mode during one tbf*/
} T_FA_MANAG;




typedef struct /* T_UL_TBF */
{
  ULONG             tlli;         /* tlli that is valid for activ TBF */
  UBYTE             access_type;  /* access type of the current tbf*/  
  UBYTE             mac_mode;     
  UBYTE             rlc_mode;     /* is estimated in tc_start_access */
  T_CODING_SCHEME   cs_type;
  UBYTE             ti;           /*  =1 tlli field present in data block, else not present*/
  BOOL              tlli_cs_type; /*  coding scheme for rlc data blocks including TLLI*/
  UBYTE             prio;         /*  Radio Prioritity from grlc_data_req or
                                      grlc_unitdata_req; should be set in tm_start_access */
  UBYTE             ac_class;     /*  access class and radio priority of the network */
  UBYTE             nr_blocks;    /*  set in ts_start_access; CS_1 is used */
                                  /*  4 Access Burst or RLCMAC-Block,
                                      maybe storing is not necessary*/
  UBYTE             nts;          /*  number of timeslots allocated for tbf */
  UBYTE             ts_mask;      /*  timeslot mask */
  UBYTE             bs_cv_max;
  USHORT            rlc_db_granted; /* indicates if close ended or open ended tbf is used */

  T_FA_MANAG        fa_manag;     /* contains all fixed alloction parameters*/
} T_UL_TBF;


typedef struct /* T_DL_TBF */
{
  ULONG             tlli;         /* tlli that is valid for activ TBF */
  UBYTE             mac_mode;     
  UBYTE             rlc_mode;     /* is estimated in tc_start_access */
  T_CODING_SCHEME   cs_type;
  UBYTE             nts;          /*  number of timeslots allocated for tbf */
  UBYTE             ts_mask;      /*  timeslot mask */
  UBYTE             t3192_val;
  UBYTE             trans_id;     /* transaction id */
} T_DL_TBF;


typedef struct /* T_TBF_CTRL */
{
  T_TBF_TYPE        tbf_type;
  ULONG             start_fn;
  ULONG             end_fn;
  ULONG             rlc_oct_cnt;
  UBYTE             pdu_cnt;
  UBYTE             vs_vr;
  UBYTE             va_vq;
  USHORT            cnt_ts;
  UBYTE             tfi;
  UBYTE             ack_cnt;
  UBYTE             fbi;
  UBYTE             ret_bsn;
} T_TBF_CTRL;




typedef struct /* T_PRIM_QUEUE */
{
  UBYTE                 next;         /* index of following entry; 0xff if no primitive follows */
  UBYTE                 previous;     /* index of previous entry; 0xff first primitive */
  T_GRLC_DATA_REQ *      prim_ptr;     /* pointer to primitive */
  T_PRIM_TYPE           prim_type;    /* GRLC_UNITDATAreq or GRLC_DATAreq*/
  BOOL                  cv_status;    /* is set if pdu is included in the count down procedure */
  BOOL                  rlc_status;   /* is set if transmission of the pdu has started*/
  BOOL                  re_allocation;/* in front of this pdu is a reallocation necessary */
  BOOL                  start_new_tbf;/* marks the first llc of a new tbf*/
  UBYTE                 last_bsn;     /* indicates the last bsn of th rlc data block, which belongs to the pdu */
} T_PRIM_QUEUE;



typedef struct /* T_NEXT_POLL_ARRAY */
{
  UBYTE               cnt;
  UBYTE               next;
  UBYTE               ctrl_ack;
  ULONG               fn;
  UBYTE               poll_type[POLL_TYPE_ARRAY_SIZE];
}T_NEXT_POLL_ARRAY;

typedef struct /* T_TM_B_DATA : used in Testmode B */
{
  USHORT              block_status; /* stores the block_status received on downlink */
  UBYTE               e_bit;        /* stores the e_bit received on downlink        */
  UBYTE               payload[50];  /* stores the payload received on downlink      */
}T_TM_B_DATA;

typedef struct /* T_TESTMODE  "test mode" */
{
  UBYTE                 mode;                         /* test mode, where
                                                       *  0x00 means no test mode
                                                       *  0x01 means type A
                                                       *  0x02 means type B
                                                       */
  ULONG                 n_pdu;                        /* number of PDUs which are to be transmitted */
  UBYTE                 dl_ts_offset;                 /* Downlink Timeslot Offset, only for CGRLC_LOOP */
  USHORT                prbs_shift_reg;               /* pseudo random bit sequence(prbs)
                                                       * shift register
                                                       */
  UBYTE *               ptr_test_data;                /*
                                                       * reference to data which are callaculated for
                                                       * the current framenumber. All slots related to the same
                                                       * frame number have to have the same data.
                                                       */
  UBYTE                 no_data_calculation;          /*
                                                       * Is set when the prbs for the current
                                                       * frame number is already calculated
                                                       */
  T_TM_B_DATA           rec_data[2];                  /* for Testmode B: received data is stored. Only 2 timeslots
                                                       * are supported. The data will be stored sequentialy depending upon 
                                                       * Downlink Timeslot Offset value 
                                                       */
} T_TESTMODE;







typedef struct  /* saves all downlink data blocks*/    
{
  USHORT                rx_no;
  T_dl_data             dl_data[MAC_MAX_DL_DATA_BLCKS];
} T_dl_struct;



typedef struct  /* saves struct of downlink data blocks*/   
{
  T_dl_struct           data_ptr;
  ULONG                 fn;
} T_FUNC_DL_BLOCKS;


typedef struct  /* functional interface: RLC_UPLINK, RLC_DOWNLINK, MACA_POWER_CONTROL */
{
  T_FUNC_DL_BLOCKS      dl_blocks; 
  T_MAC_DATA_IND        mac_data_ind;
  T_MAC_READY_IND       mac_ready_ind;
  T_MAC_PWR_CTRL_IND    mac_pwr_ctrl_ind;
} T_FUNC_INTERFACE;





/* structure for service internal data */





/*start of structs for RU*/


typedef struct
{
  U8                        mac;                                /*<  mac header                */
  U8                        ti;                                 /*<  TLLI Indicator (TI) bit   */
  U8                        e_bit;                              /*<  Extended bit              */
  U8                        li_cnt;                             /*<  Number of LIs             */
  U8                        li_me[NR_OF_PDUS_PER_RLCMAC_BLOCK]; /*<  LI, M and e-Bit           */
} T_U_DATA;


typedef struct /* T_RLC_DATA*/
{
  T_sdu *               ptr_data;
  USHORT                l_buf;
  USHORT                o_buf;
} T_RLC_DATA;

typedef struct /* T_RLC_DATA_BLOCK*/
{
  USHORT                block_status;
  T_U_DATA              header;                       /*stores the header of the rlc data block*/
  UBYTE                 data_cnt;                     /*number of pdus  in the rlc data block*/
  T_RLC_DATA            data[NR_OF_PDUS_PER_RLCMAC_BLOCK];     /*stores the pointers of the  sdus*/
  ULONG                 T3198;                        /*timer T3198, value= BS_CV_MAX block periods*/
  UBYTE                 cnt_pl_trans;                 /* counts the number of transmissions over physical link*/
} T_RLC_DATA_BLOCK;

typedef struct /* T_PL_RETRANS*/ 
{
  UBYTE                 cnt;                     /* number of transmitted rlc/mac blocks */
  UBYTE                 blk[MAX_UL_TN];  /* stores the bsn(data block) or ctrl block nr */
} T_PL_RETRANS;



/*end of structs for RU*/

/*start of structs for RD*/




typedef struct /*T_NEXT_ARRAY*/
{
  ULONG *               next;   /*zeig auf next struct vom typ T_NEXT_ARRAY*/
  USHORT                len;
  UBYTE                 data[RD_NEXT_ARRAY_DATA_SIZE];
} T_NEXT_ARRAY;

typedef struct /* T_DATA_ARRAY */       /*for received data blocks*/
{
  BOOL                  pdu_complete;   /* indicates if the last part of the data block is a complete pdu*/
  UBYTE                 pdu_cnt;        /* nr of complete pdus in the current data block*/
  T_NEXT_ARRAY *        first;          /* indicates the first element(first next_array) of the pdu */
} T_DATA_ARRAY;


/*end of structs for RD*/



/*start of structs for GFF*/




typedef struct /* T_RLC_VALUES*/
{
  USHORT              sdu_len;
  USHORT              cnt_ts;
} T_RLC_VALUES;


/*end of structs for GFF*/



/* data base for control messages */
typedef struct /* T_UL_CTRL_BLOCK */
{
  T_BLK_OWNER         owner;
  T_BLK_STATE         state;
  UBYTE               data[BYTE_UL_CTRL_BLOCK];
} T_UL_CTRL_BLOCK;

typedef UBYTE T_BLK_INDEX;



typedef struct /* T_UL_CTRL_BLOCK_ARRAY */
{
  T_UL_CTRL_BLOCK     blk[MAX_CTRL_BLK_NUM];
  T_BLK_INDEX         seq[MAX_CTRL_BLK_NUM];
} T_UL_CTRL_BLOCK_ARRAY;


typedef struct
{
  USHORT l_buf;
  USHORT o_buf;
  UBYTE  buf [2 * BYTE_UL_CTRL_BLOCK];
                       /*              CAN's suggestion never more then 2 blocks */
                       /* or 8 blocks? ID's suggestion 'B_SIZE_D_PAGING_REQ\8+1' */

} T_CTRLBUF;



typedef struct /* T_TM_DATA */
{
  UBYTE                 state;
  UBYTE                 disable_class;        /*  tm_grlc_init sets it to CR, updated with cgrlc_disable_req*/
  UBYTE                 n_res_req;            /* number of sended packet resource request */
  UBYTE                 n_acc_req_procedures; /* number of started access procedures */
  T_UL_CTRL_BLOCK_ARRAY ul_ctrl_blk;        /* holds all relevant information for */
                                            /* sending uplink control blocks      */

  UBYTE                 start_of_new_tbf;     /* index where the new TBF starts
                                               0xff means not used */
  BOOL                  send_grlc_ready_ind;   /* GRLC_READY_IND have to be sent if this variable is TRUE
                                               when a internal signal sig_ru_tm_prim_delete
                                               was received by TM. */
  UBYTE                 change_mark;          /* received from GRR, needed for sending in packet resource request*/
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  BOOL                  pacch_prr_pca_sent;   /* Use during release , Prr,pca was sent */
#endif

  T_CGRLC_freq_param    freq_param; /* frequency parameter of the current TBF */

  USHORT                max_grlc_user_data; /* maximum nr of byte which can be stored in uplink in the LLC data queue*/

#ifndef NTRACE
  char                  *name;
  char                  *state_name;
#endif
} T_TM_DATA;

typedef struct /* T_ACK_CTRL */
{
  UBYTE                       cnt_meas_rpt;
  UBYTE                       cnt_other;
} T_ACK_CTRL;

typedef struct /* T_RD_DATA */
{
  /*UBYTE*/
  UBYTE                       state;
  UBYTE                       vq;
  UBYTE                       vr;
  UBYTE                       li_cnt;
  UBYTE                       rlc_data_len;
  UBYTE                       f_ack_ind;
  UBYTE                       ssn;
  UBYTE                       last_bsn;
  UBYTE                       bsn_pdu_start;
  UBYTE                       cnt_sent_f_ack;   /* counts number of sent final downlink ack nacks*/

  /*USHORT*/
  USHORT                      pdu_len;
  /*ULONG*/
  ULONG                       fn_p_tbf_rel;
  /*POINTER*/
  /*BOOL*/
  BOOL                        ch_req_in_ack_prog;    /*indicates weather the channel request is programmed in the last packet downlink ack/nack or not*/
  BOOL                        channel_req;    /*indicates weather the channel request is sent in packet downlink ack/naok or not*/
  BOOL                        pdu_complete;
  BOOL                        inSequence;
  BOOL                        release_tbf;
  BOOL                        v_next_tbf_params; /* indicates if next_tbf_params is valid or not*/
  BOOL                        ignore_pdu; /* if true pdu shall be ignored, max_pdu_size reached and until next pdu bound every thing is discarded */

  /*Type*/
  UBYTE                       rlc_mode;
  T_DATA_ARRAY                data_array[WIN_SIZE];
  T_NEXT_ARRAY                *ptr_grlc;
  T_GRLC_DATA_IND              grlc_data_ind;
  T_NEXT_POLL_BLOCK_TYPE      next_poll_block;
  T_CODING_SCHEME             cs_type;
  T_DL_TBF                    next_tbf_params;    /* stores downlink parameters in case of tbf starting time*/

  /*array*/
  UBYTE                       vn[WIN_SIZE];
  USHORT                      li[RD_LI_CNT_MAX];
  UBYTE                       m[RD_LI_CNT_MAX];
  T_ACK_CTRL                  ack_ctrl;

#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_RD_DATA;

typedef struct /* T_RU_DATA */
{
  /*DECLARATIONS*/
  /*UBYTE*/
  UBYTE                       state;
  UBYTE                       nts;
  UBYTE                       nts_max;
  UBYTE                       ti;
  UBYTE                       next_prim;
  UBYTE                       active_prim;
  UBYTE                       vs;
  UBYTE                       va;
  UBYTE                       bsn_ret;
  UBYTE                       last_si_block;
  UBYTE                       cv;
  UBYTE                       N3104;
  UBYTE                       N3104_MAX;
  UBYTE                       count_cv_0;
  UBYTE                       nr_nacked_blks;  /*number of negative acked blocks*/
  UBYTE                       pdu_cnt;         /* nr of pdus per tbf*/
  UBYTE                       pdu_sent;        /* nr of sent pdus per tbf, but stil not acknowlegded*/
  UBYTE                       pdu_rem;         /* nr of remaining pdus per tbf*/
  UBYTE                       pdu_boundaries;  /*nr of need boundaries*/
  /*USHORT*/
  USHORT                      block_status;
  USHORT                      poll_tn;
  USHORT                      rlc_data_size;
  USHORT                      sdu_len;
  USHORT                      sdu_off;
  USHORT                      rlc_octet_cnt;
  USHORT                      tbc;        /*number of rlc data blocks during the tbf, which is requested to send*/
  USHORT                      cnt_ts;
  /*ULONG*/
  ULONG                       poll_fn;
  /*BOOL*/
  BOOL                        tlli_cs_type;
  BOOL                        cd_active;  /*indicates if the countdown procedure is active*/
  BOOL                        reorg_l1_needed; /* set to true if data blocks in l1 queue must be recalculated*/
  BOOL                        v_next_tbf_params; /* indicates if next_tbf_params is valid or not*/
  BOOL                        first_usf; /* true: wait for the first valid usf. FALSE: first usf was read*/
  /*Type*/
  UBYTE                       rlc_mode;
  T_CODING_SCHEME             cs_type;
  T_PL_RETRANS                pl_retrans_current;  /*status of current block, requested blocks to be send by PL*/
  T_LAST_BSN                  last_bsn;           /* needed for starting timer t3182 */
  /*array*/
  T_RLC_DATA_BLOCK            rlc_data[WIN_SIZE]; /*contains the rlc data block, including header and pointer to the data*/
  UBYTE                       vb[WIN_SIZE];
  T_UL_TBF                    next_tbf_params;    /* stores uplink parameters in case of tbf starting time*/


  UBYTE                       write_pos_index;
  T_ul_data                   ul_data[MAX_UL_TN]; 
  BOOL                        release_tbf;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  BOOL                        tbf_re_est;        /*TBF Re-establish on PACCH*/
#endif

#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_RU_DATA;




typedef struct /* T_GFF_DATA */
{
  UBYTE                       state;
  T_RLC_STATUS_TYPE           rlc_status;

#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_GFF_DATA;

typedef ULONG  T_SIGN_VAR_VALUE;
typedef USHORT T_SIGN_VAR_INDEX;

typedef struct /* T_SIGN_VAR */
{
  T_SIGN_VAR_VALUE value;              /* current signal variance */
  T_SIGN_VAR_INDEX index;              /* mean index              */
} T_SIGN_VAR;

typedef USHORT T_C_INDEX;
typedef USHORT T_C_VALUE;

typedef struct /* T_C_FILTER */
{
  T_C_VALUE value;                     /* current C value */
  T_C_INDEX index;                     /* iteration index */
} T_C_FILTER;

typedef struct
{
  USHORT                    total;     /* Total number of bits  */ 
  USHORT                    fault;     /* Number of faulty bits */
} T_BE_INFO;

typedef struct
{
  UBYTE                     num_dl_blck; /* Number of downlink data blocks */
  T_BE_INFO                 be_info[MAC_MAX_DL_DATA_BLCKS];
                                         /* Bit error information          */
} T_BER_IND;

typedef ULONG  T_BER_VALUE;
typedef USHORT T_BER_INDEX;

typedef struct /* T_BER_AVG */
{
  T_BER_VALUE value;                     /* current BER value in % */
  T_BER_INDEX index;                     /* iteration index        */
} T_BER_AVG;

#if ( ! ( MAC_MAX_TIMESLOTS == CGRLC_MAX_TIMESLOTS ) )
  #error MAC_MAX_TIMESLOTS == CGRLC_MAX_TIMESLOTS required
#endif

#if ( ! ( MAC_RXLEV_NONE == CGRLC_RXLEV_NONE ) )
  #error MAC_RXLEV_NONE == CGRLC_RXLEV_NONE required
#endif

#if ( ! ( MAC_RXLEV_MIN == CGRLC_RXLEV_MIN ) )
  #error MAC_RXLEV_MIN == CGRLC_RXLEV_MIN required
#endif

#if ( ! ( MAC_RXLEV_MAX == CGRLC_RXLEV_MAX ) )
  #error MAC_RXLEV_MAX == CGRLC_RXLEV_MAX required
#endif

typedef enum /* T_PCH_IDX */
{
  PCH_IDX_A   = 0,
  PCH_IDX_B   = 1,

  PCH_IDX_MAX = 2
} T_PCH_IDX;

typedef struct /* T_PCH_ARRAY */
{
  UBYTE pch[MAC_MAX_TIMESLOTS];
} T_PCH_ARRAY;

typedef struct /* T_PCH_BUFFER */
{
  T_PCH_IDX   pch_idx;
  T_PCH_ARRAY pch_array[PCH_IDX_MAX];
} T_PCH_BUFFER;

typedef struct /* T_MS_PWR_CAP */
{
  const UBYTE                *p_control;        /* power control level -> output power      */
        SHORT                 pwr_min;          /* minimum nominal output power in the cell */
        SHORT                 gamma_0;          /* GAMMA 0                                  */
} T_MS_PWR_CAP;

typedef struct /* T_TPC_DATA */
{
  UBYTE                       fix_pcl;          /* fixed PCL                      */

  BOOL                        v_fix_pwr_par;
  T_CGRLC_pwr_ctrl_param      fix_pwr_par;      /* fixed power parameter          */
  
  T_CGRLC_pwr_ctrl_param      pwr_par;          /* power control parameter        */
  T_CGRLC_glbl_pwr_ctrl_param glbl_pwr_par;     /* global power control parameter */

  T_PCH_BUFFER                pch_buffer;       /* power control level buffer     */  

#if !defined (NTRACE)

  UBYTE                       n_tpc_trace;

#endif /* #if !defined (NTRACE) */

#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_TPC_DATA;

typedef struct /* T_GRLC_MEAS_DATA */
{
  T_CGRLC_INT_LEVEL_REQ        ilev;
  T_C_FILTER                   c_filter;
  T_SIGN_VAR                   sign_var;
  T_BER_AVG                    ber_avg;
  BOOL                         sq_restart;

#if !defined (NTRACE)

  UBYTE                        v_im_trace;

#endif /* #if !defined (NTRACE) */

#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_GRLC_MEAS_DATA;

typedef struct /* T_GRLC_READY_TIMER */
{
  T_READY_TIMER_HANDLING  handling;
  T_READY_TIMER_STATE     state;
  ULONG                   value;
} T_GRLC_READY_TIMER;

typedef struct /* T_GRLC_DATA */
{
  T_GRLC_READY_TIMER      ready_timer;

  UBYTE                   burst_type;
  UBYTE                   ab_type;
  T_CGRLC_pan_struct      pan_struct;
  UBYTE                   t3168_val;


  T_UL_TBF                uplink_tbf;   /*  This structure is used by TM,RU
                                            during uplink-tbf */
  T_DL_TBF                downlink_tbf; /*  This structure is used by TM,RD
                                            during downlink-tbf */
  T_TBF_TYPE              tbf_type;
  T_REL_TYPE              rel_type;        /* indicates the type of TBF currently being released */
  UBYTE                   prim_start_free; /* index of first free prim queue entry */
  UBYTE                   prim_start_tbf ; /* index of first tbf prim queue entry */

  UBYTE                   save_prim_start_free; /* save value during rau request */
  UBYTE                   save_prim_start_tbf ; /* save value during rau request */
  BOOL                    gmm_procedure_is_running;   /* inidcates if gmm procedure is rnnig or not*/

  USHORT                  prim_user_data; /* user data which is current in the prim_queue */
  T_PRIM_QUEUE            prim_queue[PRIM_QUEUE_SIZE_TOTAL];
                                        /* queueing of llc-pdu's */
  UBYTE                   ta_value;

  UBYTE                   ul_tn_mask;    /* current uplink timeslot mask */
  UBYTE                   dl_tn_mask;    /* current downlink timeslot mask */

  UBYTE                   ul_tfi;
  UBYTE                   dl_tfi;
  UBYTE                   r_bit;        /* current r-bit value - set by TC - read by RU/RD*/
  UBYTE                   poll_start_free;
  UBYTE                   poll_start_tbf;
  ULONG                   ul_fn;        /* framenumber for the current radio block, needed for transmission*/
  ULONG                   dl_fn;        /* framenumber of the last radio block, where a block is received*/
  T_NEXT_POLL_ARRAY       next_poll_array[NEXT_POLL_ARRAY_SIZE] ;
  UBYTE                   N3102;
  T_chan_req_des          chan_req_des; /* this will be included in next Packet Downlink AckNack
                                           write by TC, read by RD */
  T_TESTMODE              testmode;     /* this structure contains all GPRS test mode related data */

    /* only for tracing*/
  ULONG                   grlc_data_req_cnt;
  ULONG                   dl_tbf_start_time;
  ULONG                   ul_tbf_start_time;
  UBYTE                   ul_index;
  UBYTE                   dl_index;
  T_TBF_CTRL              tbf_ctrl[2];

  /*handling of gaps*/
  ULONG                   missed_poll_fn;

  UBYTE                   t3164_to_cnt;   /* conunts the timeouts of timer t3164 in sequence */  
  UBYTE                   start_fn_ul_tfi; /* stores the uplink tfi, which becomes valid after starting time*/
  UBYTE                   start_fn_dl_tfi; /* stores the downlink tfi, which becomes valid after starting time*/
  T_ul_poll_resp          ul_poll_resp[2];//MULTISLOT 
  UBYTE                   ul_poll_pos_index;  /* index of ul_poll_response, need for polling */
  ULONG                   next_poll_fn; /*indicates the next poll fn  */
  T_FUNC_INTERFACE        func;

  ULONG                   nr_of_crc_errors;
  T_TFI_CHANGE            tfi_change;
  BOOL                    ul_tfi_changed;
  UBYTE                   grlc_wo_ccd; /*=0 ccd is used for en/decoding, =1 messages are decoded wo ccd */
#ifdef REL99
  BOOL                    pfi_support;
  U8                      pfi_value;
  U8                      nw_rel;
#endif

#if defined (_TARGET_) /*target debugging*/
  ULONG                   last_ul_fn;
  ULONG                   last_dl_fn;
#define CALL_ERROR_NR_OF_ST_FN 9
  ULONG                   ul_fn_store[CALL_ERROR_NR_OF_ST_FN+1];
  ULONG                   ul_fn_errors[CALL_ERROR_NR_OF_ST_FN+1];
  ULONG                   dl_fn_store[CALL_ERROR_NR_OF_ST_FN+1];
  ULONG                   dl_fn_errors[CALL_ERROR_NR_OF_ST_FN+1];
  UBYTE                   ul_cnt_syn;     /*for target, count calls in rlc uplink*/
  UBYTE                   ul_cnt_asyn;    /*for target, count calls in gffp mac_ready_ind*/
  UBYTE                   dl_cnt_syn;     /*for target, count calls in rlc downlink*/
  UBYTE                   dl_cnt_asyn;    /*for target, count calls in gffp mac_data_ind*/
  UBYTE                   ul_call_errors; /*for target, count calls in rlc uplink during grlc is active*/
  UBYTE                   dl_call_errors; /*for target, count calls in rlc downlink during grlc is active*/
#endif /*(_TARGET_) */

  /*
   * Service data
   */
  T_GFF_DATA               gff;
  T_TM_DATA                tm;
  T_RD_DATA                rd;
  T_RU_DATA                ru;
  T_TPC_DATA               tpc;
  T_GRLC_MEAS_DATA         meas;
}T_GRLC_DATA;








/*==== EXPORT =====================================================*/

/*
 * data base
 */
#ifdef GRLC_PEI_C
       T_GRLC_DATA grlc_data_base, *grlc_data;
#else
EXTERN T_GRLC_DATA grlc_data_base, *grlc_data;
#endif

#define ENTITY_DATA grlc_data 


/*
 * Communication handles (see also GRLC_PEI.C)
 */
#define hCommL1             grlc_hCommMAC    
#define hCommGRR            grlc_hCommGRR
#define hCommGRLC           grlc_hCommGRLC
#define hCommGMM            grlc_hCommGMM
#define hCommLLC            grlc_hCommLLC
#define hCommPL             grlc_hCommPL
#ifdef FF_WAP
  #define hCommWAP          grlc_hCommWAP    /*In future an own FMM Entity is planned*/
#endif

#define _decodedMsg   grlc__decodedMsg


#ifdef GRLC_PEI_C
       T_HANDLE hCommL1         = VSI_ERROR;
       T_HANDLE hCommGRLC       = VSI_ERROR;
       T_HANDLE hCommGRR        = VSI_ERROR;
       T_HANDLE hCommGMM        = VSI_ERROR;
       T_HANDLE hCommLLC        = VSI_ERROR;
       T_HANDLE hCommPL         = VSI_ERROR;
#ifdef FF_WAP
       T_HANDLE hCommWAP         = VSI_ERROR;
#endif
       T_HANDLE GRLC_handle;



GLOBAL UBYTE          _decodedMsg [MAX_MSTRUCT_LEN_GRLC];

/*
 * make the pei_create function unique
 */
#define pei_create              grlc_pei_create

/*
 * make the pei_func_primitive function unique
 */
#define pei_func_primitive      grlc_pei_func_primitive

#else  /* !GRLC_PEI_C */
EXTERN T_HANDLE hCommL1;
EXTERN T_HANDLE hCommGRLC;
EXTERN T_HANDLE hCommGRR;
EXTERN T_HANDLE hCommGMM;
EXTERN T_HANDLE hCommLLC;
EXTERN T_HANDLE hCommPL;
#ifdef FF_WAP
 EXTERN T_HANDLE hCommWAP;
#endif
EXTERN T_HANDLE GRLC_handle;
#endif /* GRLC_PEI_C */


EXTERN UBYTE          _decodedMsg [MAX_MSTRUCT_LEN_GRLC];

#ifdef _TARGET_

#undef TRACE_FUNCTION
#define TRACE_FUNCTION(a)
#undef TRACE_ISIG
#define TRACE_ISIG(a)

#endif /* _TARGET_ */

#endif /* GRLC_H */
