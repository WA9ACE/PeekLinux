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
|  Purpose :  Definitions for the Protocol Stack Entity Rate Adaption.
+----------------------------------------------------------------------------- 
*/ 

#ifndef RA_H
#define RA_H

#include "cl_ribu.h"

/*
 * Conditional Compiling
 */

#define HO_WORKAROUND
#undef V42BIS

#undef TRACE_RA_TRA_STATUS
#undef TRACE_RA_TRA_DATA

#undef TRACE_RA_FAX_DL_FRAME_TYPE
#undef TRACE_RA_FAX_UL_FRAME_TYPE
#undef TRACE_RA_FAX_UL_SKIP

#undef TRACE_RA_RLP

#if defined TRACE_RA_TRA_STATUS \
 || defined TRACE_RA_FAX_DL_FRAME_TYPE \
 || defined TRACE_RA_FAX_UL_FRAME_TYPE \
 || defined _SIMULATION_

#define BYTE2HEXSTR(B, H) {UBYTE b, *a = (UBYTE*)(H);\
        b = (((UBYTE)(B)) >> 4) & 0x0F;\
        if (b > 9) b += 'A'-10; else b += '0'; *a = b;\
        b = ((UBYTE)(B)) & 0x0F;\
        if (b > 9) b += 'A'-10; else b += '0'; *(a+1) = b;}

#endif

/*
  This constant gives the minimum number of frames,
  which have to be sent in a FR_TRAIN sequence.
  It is not derived from any protocol specfication,
  but found to be necessary to establish fax calls
  (MOC) at 2400bps on the D1 network
 */
#define MIN_FR_TRAIN_LEN 32

 /*
 * Macros
 */
/*
 * instance management
 */
#ifndef NTRACE

/* for trace purposes */

#define SERVICE_NAME_KER  "KER"

#endif

#define KER ker.

/*
 * States of the entity process TRANS
 */
#define TRA_NULL            0
#define TRA_INIT_L1         1
#define TRA_SYNCH_TCH_START 2
#define TRA_SYNCH_TCH_FOUND 3
#define TRA_WAIT_SYNC_LOST  4
#define TRA_DATA_TRANS_PRE1 5
#define TRA_DATA_TRANS_PRE2 6
#define TRA_DATA_TRANS_PRE3 7
#define TRA_DATA_TRANS      8
#define TRA_STATES          9

/*
 * States of the entity process RLP
 */
#define RLP_NULL            0
#define RLP_INIT_L1         1
#define RLP_DATA_TRANS      2
#define RLP_STATES          3

#ifdef FF_FAX
/*
 * States of the entity process FAX
 */
#define FAX_NULL            0
#define FAX_INIT_L1         1
#define FAX_SYNCH_TCH_START 2
#define FAX_SYNCH_TCH_FOUND 3
#define FAX_WAIT_SYNC_LOST  4
#define FAX_DATA_TRANS_PRE  5
#define FAX_DATA_TRANS      6
#define FAX_MODIFY          7
#define FAX_STATES          8
#endif /* FF_FAX */

/*
 * Bitmask definitions:
 * msk_xxx  bitmask left-justified value
 * pos_xxx  bitposition of the Less Significant Bit
 * adr_xxx  index into the shm_addr and cont_addr tables
 * wof_xxx  word offset which is to be added to the address
 *          in addr_shm table
 */

/*
 * Bitmask definitions for the d_ra_conf control word of the RADAP
 */

#define msk_conf_b_itc            0x0003
#define pos_conf_b_itc            0
#define adr_conf_b_itc            d_ra_conf
#define wof_conf_b_itc            0

/*
  #define V_ITC_SPEECH    0
*/
  #define V_ITC_DATA      1
  #define V_ITC_FAX       2

#define msk_conf_b_nsb            0x0004
#define pos_conf_b_nsb            2
#define adr_conf_b_nsb            d_ra_conf
#define wof_conf_b_nsb            0

  #define V_NSB_ONE_STOP  0
  #define V_NSB_TWO_STOP  1

#define msk_conf_b_ndb            0x0008
#define pos_conf_b_ndb            3
#define adr_conf_b_ndb            d_ra_conf
#define wof_conf_b_ndb            0

  #define V_NDMSK_B_7_DBITS   0
  #define V_NDMSK_B_8_DBITS   1

#define msk_conf_b_ur             0x00f0
#define pos_conf_b_ur             4
#define adr_conf_b_ur             d_ra_conf
#define wof_conf_b_ur             0

  #define V_UR_300        1
  #define V_UR_1200       2
  #define V_UR_2400       3
  #define V_UR_4800       4
  #define V_UR_9600       5
  /* not used             6 */
  #define V_UR_1200_75    7
  #define V_UR_14400      8

#define msk_conf_b_ce             0x0100
#define pos_conf_b_ce             8
#define adr_conf_b_ce             d_ra_conf
#define wof_conf_b_ce             0
  #define V_CE_TRANSP     0
  #define V_CE_NON_TRANSP 1

#define msk_conf_b_ct             0x0200
#define pos_conf_b_ct             9
#define adr_conf_b_ct             d_ra_conf
#define wof_conf_b_ct             0
  #define V_CT_FR         0
  #define V_CT_HR         1

/*
 * Bitmask definitions for the d_ra_act control word of the RADAP
 */
#define msk_act_b_init            0x0001
#define pos_act_b_init            0
#define adr_act_b_init            d_ra_act
#define wof_act_b_init            0

#define msk_act_b_syncul          0x0002
#define pos_act_b_syncul          1
#define adr_act_b_syncul          d_ra_act
#define wof_act_b_syncul          0

#define msk_act_b_syncdl          0x0004
#define pos_act_b_syncdl          2
#define adr_act_b_syncdl          d_ra_act
#define wof_act_b_syncdl          0

#define msk_act_b_ovspul          0x0008
#define pos_act_b_ovspul          3
#define adr_act_b_ovspul          d_ra_act
#define wof_act_b_ovspul          0

#define msk_act_b_f48blk_ul       0x0020
#define pos_act_b_f48blk_ul       5
#define adr_act_b_f48blk_ul       d_ra_act
#define wof_act_b_f48blk_ul       0

#define msk_act_b_f48blk_dl       0x0040
#define pos_act_b_f48blk_dl       6
#define adr_act_b_f48blk_dl       d_ra_act
#define wof_act_b_f48blk_dl       0

/*
 * Bitmask definitions for the d_ra_test
 * control word of the RADAP
 */
/*
#define msk_test_b_sul            0x0001
#define pos_test_b_sul            0
#define adr_test_b_sul            d_ra_test
#define wof_test_b_sul            0

#define msk_test_b_sdl            0x0002
#define pos_test_b_sdl            1
#define adr_test_b_sdl            d_ra_test
#define wof_test_b_sdl            0

#define msk_test_b_sched          0x0004
#define pos_test_b_sched          2
#define adr_test_b_sched          d_ra_test
#define wof_test_b_sched          0
*/

#define msk_test_b_t_dl_debug     0x0010
#define pos_test_b_t_dl_debug     4
#define adr_test_b_t_dl_debug     d_ra_test
#define wof_test_b_t_dl_debug     0

/*
 * Bitmask definitions for the d_ra_statu
 * control word of the RADAP
 */
#define msk_statu_b_sa            0x0001
#define pos_statu_b_sa            0
#define adr_statu_b_sa            d_ra_statu
#define wof_statu_b_sa            0

#define msk_statu_b_sb            0x0002
#define pos_statu_b_sb            1
#define adr_statu_b_sb            d_ra_statu
#define wof_statu_b_sb            0

#define msk_statu_b_x             0x0004
#define pos_statu_b_x             2
#define adr_statu_b_x             d_ra_statu
#define wof_statu_b_x             0

#define msk_statu_b_fed_ul        0x0008
#define pos_statu_b_fed_ul        3
#define adr_statu_b_fed_ul        d_ra_statu
#define wof_statu_b_fed_ul        0

/*
 * Bitmask definitions for the d_ra_statd control word of the RADAP
 */
#define msk_statd_b_sa            0x0001
#define pos_statd_b_sa            0
#define adr_statd_b_sa            d_ra_statd
#define wof_statd_b_sa            0

#define msk_statd_b_sb            0x0002
#define pos_statd_b_sb            1
#define adr_statd_b_sb            d_ra_statd
#define wof_statd_b_sb            0

#define msk_statd_b_x             0x0004
#define pos_statd_b_x             2
#define adr_statd_b_x             d_ra_statd
#define wof_statd_b_x             0

/*
 * for d_ra_statd only
 */
#define msk_statd_b_syncdet       0x0008
#define pos_statd_b_syncdet       3
#define adr_statd_b_syncdet       d_ra_statd
#define wof_statd_b_syncdet       0

/*
 * Bitmask definitions for the d_fax control word of the RADAP
 */
#define msk_fax_b_status_stop     0x0001
#define pos_fax_b_status_stop     0
#define adr_fax_b_status_stop     d_fax
#define wof_fax_b_status_stop     0

#define msk_fax_b_status_detect   0x0002
#define pos_fax_b_status_detect   1
#define adr_fax_b_status_detect   d_fax
#define wof_fax_b_status_detect   0

#define msk_fax_b_ident_thres     0x007c
#define pos_fax_b_ident_thres     2
#define adr_fax_b_ident_thres     d_fax
#define wof_fax_b_ident_thres     0

#define msk_fax_b_info_thres      0x0f80
#define pos_fax_b_info_thres      7
#define adr_fax_b_info_thres      d_fax
#define wof_fax_b_info_thres      0

#define msk_fax_b_status_receive  0x1000
#define pos_fax_b_status_receive  12
#define adr_fax_b_status_receive  d_fax
#define wof_fax_b_status_receive  0

#define msk_fax_b_data_receive    0x2000
#define pos_fax_b_data_receive    13
#define adr_fax_b_data_receive    d_fax
#define wof_fax_b_data_receive    0

#define msk_fax_b_byte_interface  0x4000
#define pos_fax_b_byte_interface  14
#define adr_fax_b_byte_interface  d_fax
#define wof_fax_b_byte_interface  0

/*
 * Bitmask definitions for the a_rau control field of the RADAP
 */
#define RAU_DATA_14400           36
#define RAU_DATA_9600            24
#define RAU_DATA_4800            12
#define RAU_DATA_2400             6
#define RAU_DATA_1200             3

#define msk_rau_byte_cnt          0x00ff
#define pos_rau_byte_cnt          0
#define adr_rau_byte_cnt          a_rau
#define wof_rau_byte_cnt          0

#define msk_rau_break_len         0xff00
#define pos_rau_break_len         8
#define adr_rau_break_len         a_rau
#define wof_rau_break_len         1

#define msk_rau_break_pos         0x00ff
#define pos_rau_break_pos         0
#define adr_rau_break_pos         a_rau
#define wof_rau_break_pos         1

/*
#define msk_rau_data              0xffff
#define pos_rau_data              0
*/
#define adr_rau_data              a_rau
#define wof_rau_data              2

/*
 * Bitmask definitions for the a_rad control field of the RADAP
 */
/*
#define MAX_RAD_DATA              36
*/
#define msk_rad_byte_cnt          0x00ff
#define pos_rad_byte_cnt          0
#define adr_rad_byte_cnt          a_rad
#define wof_rad_byte_cnt          0

#define msk_rad_break_len         0xff00
#define pos_rad_break_len         8
#define adr_rad_break_len         a_rad
#define wof_rad_break_len         1

#define msk_rad_break_pos         0x00ff
#define pos_rad_break_pos         0
#define adr_rad_break_pos         a_rad
#define wof_rad_break_pos         1

/*
#define msk_rad_data              0xffff
#define pos_rad_data              0
*/
#define adr_rad_data              a_rad
#define wof_rad_data              2

/*
 * Definitions of frame sizes for non transparent mode (in bytes, not including the FCC)
 */

#define NT_DATA_SIZE              27  /* Full frame at 9600 full rate and 4800 half rate */
#define NT_DATA_SIZE_14400_A      36  /* First half of frame at 14400 */
#define NT_DATA_SIZE_14400_B      33  /* Second half of frame at 14400 */
#define NT_DATA_SIZE_4800_FR_A    15  /* First half of frame at 4800 full rate */
#define NT_DATA_SIZE_4800_FR_B    12  /* Second half of frame at 4800 full rate */

/*
 * Bitmask definitions for the a_ntu control field of the RADAP
 */

#define msk_ntu_byte_cnt          0x00ff
#define pos_ntu_byte_cnt          0
#define adr_ntu_byte_cnt          a_ntu
#define wof_ntu_byte_cnt          0

#define msk_ntu_M1                0x0001
#define pos_ntu_M1                0
#define adr_ntu_M1                a_ntu
#define wof_ntu_M1                1

#define msk_ntu_M2                0x0002
#define pos_ntu_M2                1
#define adr_ntu_M2                a_ntu
#define wof_ntu_M2                1

#define msk_ntu_empty             0x0004
#define pos_ntu_empty             2
#define adr_ntu_empty             a_ntu
#define wof_ntu_empty             1

/*
#define msk_ntu_data              0xffff
#define pos_ntu_data              0
*/
#define adr_ntu_data              a_ntu
#define wof_ntu_data              2

/*
 * Bitmask definitions for the a_ntd control word of the RADAP
 */
#define msk_ntd_byte_cnt          0x00ff
#define pos_ntd_byte_cnt          0
#define adr_ntd_byte_cnt          a_ntd
#define wof_ntd_byte_cnt          0

#define msk_ntd_M1                0x0001
#define pos_ntd_M1                0
#define adr_ntd_M1                a_ntd
#define wof_ntd_M1                1

/*
#define msk_ntd_M2                0x0002
#define pos_ntd_M2                1
#define adr_ntd_M2                a_ntd
#define wof_ntd_M2                1
*/

#define msk_ntd_fcs_ok            0x0008
#define pos_ntd_fcs_ok            3
#define adr_ntd_fcs_ok            a_ntd
#define wof_ntd_fcs_ok            1

/*
#define msk_ntd_data              0x00ff
#define pos_ntd_data              0
*/
#define adr_ntd_data              a_ntd
#define wof_ntd_data              2

/*
 * Bitmask definitions for the a_faxu control word of the RADAP
 */
#define msk_faxu_byte_cnt         0x00ff
#define pos_faxu_byte_cnt         0
#define adr_faxu_byte_cnt         a_faxu
#define wof_faxu_byte_cnt         0

/*
#define msk_faxu_sync_cnt         0x00ff
#define pos_faxu_sync_cnt         0
#define adr_faxu_sync_cnt         a_faxu
#define wof_faxu_sync_cnt         0
*/

#define msk_faxu_control          0xff00
#define pos_faxu_control          8
#define adr_faxu_control          a_faxu
#define wof_faxu_control          0

/*
#define msk_faxu_data             0xffff
#define pos_faxu_data             0
*/
#define adr_faxu_data             a_faxu
#define wof_faxu_data             1

/*
 * Bitmask definitions for the a_faxd control word of the RADAP
 */
#define msk_faxd_byte_cnt         0x00ff
#define pos_faxd_byte_cnt         0
#define adr_faxd_byte_cnt         a_faxd
#define wof_faxd_byte_cnt         0

/*
#define msk_faxd_sync_cnt         0x00ff
#define pos_faxd_sync_cnt         0
#define adr_faxd_sync_cnt         a_faxd
#define wof_faxd_sync_cnt         0
*/

#define msk_faxd_control          0xff00
#define pos_faxd_control          8
#define adr_faxd_control          a_faxd
#define wof_faxd_control          0

#define V_CTRL_DATA    0
#define V_CTRL_STATUS  1
#define V_CTRL_SYNC    2
#define V_CTRL_TRAIN   3

/*
#define msk_faxd_data             0xffff
#define pos_faxd_data             0
*/
#define adr_faxd_data             a_faxd
#define wof_faxd_data             1

/*
 * index for the address array
 */
#define d_ra_conf   0
#define d_ra_act    1
#define d_ra_test   2
#define d_ra_statu  3
#define d_ra_statd  4
#define d_fax       5
#define a_rau       6
#define a_rad       7
#define a_ntu       8
#define a_ntd       9
#define a_faxu      10
#define a_faxd      11 /* must be the last in the list */

#define RA_FIFO_DEPTH 4

#define MAX_TRANS_BUFFER    36
#define MAX_FAX_BUFFER_UL   72
#define FAD_DESCBUF_LEN     MAX_FAX_BUFFER_UL /* for simulation */

#ifdef _SIMULATION_
#define MAX_NTRANS_BUFFER   36
#define MAX_FAX_BUFFER_DL   72
#endif

#define MODEL_RLP   1
#define MODEL_TRANS 2
#ifdef FF_FAX
#define MODEL_FAX   3
#endif

/*==== TYPES ======================================================*/

typedef struct
{
  USHORT *shm_addr[a_faxd+1];
} T_SHARED_MEM;

typedef struct
{
  USHORT timer;
  USHORT delay_OFF_ON;
  USHORT delay_ON_OFF;
  UBYTE  pos;
  UBYTE  current;
  UBYTE  last;
} T_STATUS_BIT;

typedef struct
{
  BOOL   ul_data_pending;
  BOOL   ul_signals_pending;
  BOOL   ul_break_pending;
  UBYTE  overspeed;
  UBYTE  ready_ind_idx;
  BOOL   data_req_rec;  /* flag, if RA_DATA_REQ received */

  UBYTE  ul_sa;
  UBYTE  ul_sb;
  UBYTE  ul_x;
  UBYTE  ul_status;

  T_STATUS_BIT dl_sa;
  T_STATUS_BIT dl_sb;
  T_STATUS_BIT dl_x;

  UBYTE  break_pos;
  USHORT break_len;

#ifndef HO_WORKAROUND
  T_RA_BREAK_IND ra_break_ind;
#endif
  T_FD *dl_pFD;

  T_FRAME_DESC ul_frame_desc;
} T_TRA;

typedef struct
{
  UBYTE const skip_seq_same[8];
  UBYTE const skip_seq_other[8];
} T_SKIP;

#ifdef FF_FAX
typedef struct
{
  BOOL  CMM_flag;

  UBYTE ul_frame_type;
  UBYTE ul_frame_type_last;
  UBYTE train_seq[2];
  UBYTE train_len;
  UBYTE seq_idx;
  UBYTE req_idx;

  T_RIBU_FD *ul_ribu;
  T_FD *ul_pFD;
  T_FD ul_train_FD;

#if defined TRACE_RA_FAX_DL_FRAME_TYPE || defined _SIMULATION_
  UBYTE dl_frame_type_last;
  UBYTE dl_state_last;
#endif

  T_FD *dl_pFD;

  const T_SKIP *skip_seq;
  const UBYTE  *req_seq;

} T_FAX;
#endif /* FF_FAX */

#define RLP_UL_QSIZE 16

typedef struct
{
  BOOL          ul_data_pending;
  BOOL          ul_48_first_half_sent;
  UBYTE         m1;
  T_RIBU        ulq;
  T_FRAME_DESC  ul_frame_desc[RLP_UL_QSIZE];
  T_FRAME_DESC  dl_frame_desc;

#ifdef _SIMULATION_
  UBYTE dl_buf[MAX_FAX_BUFFER_DL]; /* maximum is MAX_FAX_BUFFER_DL */
#endif

#ifdef TRACE_RA_RLP
  ULONG         fcs_error;
#endif
} T_RLP;

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
  T_STATE ker;
  T_SHARED_MEM shm;

  BOOL    activated;
  UBYTE   model;
  UBYTE   nsb;
  UBYTE   ndb;
  UBYTE   user_rate;
  UBYTE   tra_rate;

  USHORT  sync_timer;
  USHORT  cycle_time;

  T_TRA   *tra;
  T_RLP   *rlp;

#ifdef FF_FAX
  T_FAX   *fax;
#endif

  T_RA_DATA_IND       ra_data_ind;
  T_RA_READY_IND      ra_ready_ind[2];
  
#ifdef _SIMULATION_

  T_RIBU_FD *dl_ribu;
  T_FD ul_FD;
  UBYTE ul_buf[MAX_TRANS_BUFFER];

#else

  T_RA_BREAK_IND      ra_break_ind;
  T_RA_DEACTIVATE_CNF ra_deactivate_cnf;

#endif


} T_RA_DATA;

/*==== EXPORT =====================================================*/

#ifdef OPTION_MULTITHREAD
  #define hCommMMI        _ENTITY_PREFIXED(hCommMMI)
  #define hCommRLP        _ENTITY_PREFIXED(hCommRLP)
#ifdef FF_FAX
  #define hCommFAD        _ENTITY_PREFIXED(hCommFAD)
#endif
  #define hCommTRA        _ENTITY_PREFIXED(hCommTRA) /* for trans mode */
#endif

#define ENTITY_DATA ra_data

#ifdef RA_DLL_C

GLOBAL T_RA_DATA *ra_data;

GLOBAL T_HANDLE  ra_handle;
GLOBAL T_HANDLE  hCommMMI = VSI_ERROR;       /* MMI  Communication */
GLOBAL T_HANDLE  hCommTRA = VSI_ERROR;       /* Transp Communication */
GLOBAL T_HANDLE  hCommRLP = VSI_ERROR;       /* RLP  Communication */
#ifdef FF_FAX
GLOBAL T_HANDLE  hCommFAD = VSI_ERROR;       /* FAD  Communication */
#endif

#ifdef HO_WORKAROUND
GLOBAL USHORT _act_d_ra_conf;
#endif

#else /* !RA_DLL_C */

EXTERN T_RA_DATA *ra_data;

EXTERN T_HANDLE  ra_handle;
EXTERN T_HANDLE  hCommMMI;                   /* MMI  Communication */
EXTERN T_HANDLE  hCommTRA;                   /* Transp. Communication */
EXTERN T_HANDLE  hCommRLP;                   /* RLP  Communication */
#ifdef FF_FAX
EXTERN T_HANDLE  hCommFAD;                   /* FAD  Communication */
#endif

#ifdef HO_WORKAROUND
EXTERN  USHORT _act_d_ra_conf;
#endif

#endif /* !RA_DLL_C */

/*
 * function prototypes for process TRA
 */

#ifdef OPTION_MULTITHREAD
#define tra_init                _ENTITY_PREFIXED(tra_init)
#define tra_idle                _ENTITY_PREFIXED(tra_idle)
#define tra_ul_null             _ENTITY_PREFIXED(tra_ul_null)
#define tra_ul_init_l1          _ENTITY_PREFIXED(tra_ul_init_l1)
#define tra_dl_sync_tch_start   _ENTITY_PREFIXED(tra_dl_sync_tch_start)
#define tra_ul_sync_tch_found   _ENTITY_PREFIXED(tra_ul_sync_tch_found)
#define tra_dl_wait_sync_lost   _ENTITY_PREFIXED(tra_dl_wait_sync_lost)
#define tra_dl_wait_for_connect _ENTITY_PREFIXED(tra_dl_wait_for_connect)
#define tra_dl_wait_for_uart    _ENTITY_PREFIXED(tra_dl_wait_for_uart)
#define tra_dl_got_uart         _ENTITY_PREFIXED(tra_dl_got_uart)
#define tra_ul_data_trans       _ENTITY_PREFIXED(tra_ul_data_trans)
#define tra_dl_data_trans       _ENTITY_PREFIXED(tra_dl_data_trans)
#endif

EXTERN void tra_init(UBYTE);
EXTERN void tra_idle(void);
EXTERN void tra_ul_null(void);
EXTERN void tra_ul_init_l1(void);
EXTERN void tra_dl_sync_tch_start(void);
EXTERN void tra_ul_data_trans(void);
EXTERN void tra_ul_sync_tch_found(void);
EXTERN void tra_dl_wait_sync_lost(void);
EXTERN void tra_dl_wait_for_connect(void);
EXTERN void tra_dl_wait_for_uart(void);
EXTERN void tra_dl_got_uart(void);
EXTERN void tra_dl_data_trans(void);

#ifdef FF_FAX
/*
 * function prototypes for process FAX
 */

#ifdef OPTION_MULTITHREAD
#define fax_init                 _ENTITY_PREFIXED(fax_init)
#define fax_idle                 _ENTITY_PREFIXED(fax_idle)
#define fax_ul_null              _ENTITY_PREFIXED(fax_ul_null)
#define fax_ul_init_l1           _ENTITY_PREFIXED(fax_ul_init_l1)
#define fax_dl_sync_tch_start    _ENTITY_PREFIXED(fax_dl_sync_tch_start)
#define fax_ul_sync_tch_found    _ENTITY_PREFIXED(fax_ul_sync_tch_found)
#define fax_dl_wait_sync_lost    _ENTITY_PREFIXED(fax_dl_wait_sync_lost)
#define fax_ul_data_trans        _ENTITY_PREFIXED(fax_ul_data_trans)
#define fax_dl_data_trans        _ENTITY_PREFIXED(fax_dl_data_trans)
#define fax_ul_modify            _ENTITY_PREFIXED(fax_ul_modify)
#endif

EXTERN void fax_init(void);
EXTERN void fax_idle(void);
EXTERN void fax_ul_null(void);
EXTERN void fax_ul_init_l1(void);
EXTERN void fax_dl_sync_tch_start(void);
EXTERN void fax_ul_data_trans(void);
EXTERN void fax_ul_sync_tch_found(void);
EXTERN void fax_dl_wait_sync_lost(void);
EXTERN void fax_dl_data_trans(void);
EXTERN void fax_ul_modify(void);

#endif /* FF_FAX */

/*
 * function prototypes for process RLP
 */

#ifdef OPTION_MULTITHREAD
#define rlp_init                 _ENTITY_PREFIXED(rlp_init)
#define rlp_idle                 _ENTITY_PREFIXED(rlp_idle)
#define rlp_ul_null              _ENTITY_PREFIXED(rlp_ul_null)
#define rlp_ul_init_l1           _ENTITY_PREFIXED(rlp_ul_init_l1)
#define rlp_ul_data_trans        _ENTITY_PREFIXED(rlp_ul_data_trans)
#define rlp_dl_data_trans        _ENTITY_PREFIXED(rlp_dl_data_trans)
#endif

EXTERN void rlp_init(void);
EXTERN void rlp_idle(void);
EXTERN void rlp_ul_null(void);
EXTERN void rlp_ul_init_l1(void);
EXTERN void rlp_ul_data_trans(void);
EXTERN void rlp_dl_data_trans(void);

/*
 * macros for setting/getting the bitpattern values
 * of the control words in the shared memory.
 */

#define shm_get_dsp_value(var)\
  (((*(ra_data->shm.shm_addr[adr_##var]+wof_##var)) & msk_##var) >> pos_##var)

#define shm_set_dsp_value(var,val)\
{ USHORT sv = val;\
  USHORT sc = ra_data->shm.shm_addr[adr_##var][wof_##var];\
  sv <<= pos_##var;\
  sv &= msk_##var;\
  sc &= ~msk_##var;\
  sc |= sv;\
  ra_data->shm.shm_addr[adr_##var][wof_##var] = sc;}

EXTERN U16 shm_set_dsp_buffer(T_FRAME_DESC *data, USHORT *buffer, USHORT offs, USHORT free);
EXTERN U8  shm_set_dsp_buffer_new(T_FD *data, USHORT *buffer, U8 offs, U8 free);

EXTERN U16 shm_get_dsp_buffer(T_FRAME_DESC *data, USHORT *buffer, USHORT bytes_to_read);
EXTERN U8  shm_get_dsp_buffer_new(USHORT *buffer, U8 bytes_to_read, T_FD *pFD);

#ifdef _SIMULATION_
/*
 * for layer 1 simulation only
 *====================================================================
 * Bitoffset for encoding/decoding
 */
#define ENCODE_OFFSET 0

/*
 * Dynamic Configuration Numbers
 */
#define RA_CNF    9
#define RA_DEACT 10

#endif /* _SIMULATION_ */

/*
 * L1 prototypes
 */

EXTERN void l1i_ra_activate_cnf(void);
EXTERN void l1i_ra_data_ind(T_HANDLE);
EXTERN void l1i_ra_modify_cnf(void);
EXTERN void l1i_ra_ready_ind(T_HANDLE, UBYTE);
#ifndef HO_WORKAROUND
EXTERN void l1i_ra_break_ind(void);
#endif

/*
 * DLL functions
 */

EXTERN void dll_init(void);

EXTERN void dll_data_ul
(
  USHORT *_ul_buffer_address,
  USHORT *_d_ra_conf,
  USHORT *_d_ra_act,
  USHORT *_d_ra_test,
  USHORT *_d_ra_statu,
  USHORT *_d_fax
);

EXTERN void dll_data_dl
(
  USHORT *_dl_buffer_address,
  USHORT *_d_ra_act,
  USHORT *_d_ra_statd
);

/*
 * miscellaneous function prototypes
 */
#ifdef OPTION_MULTITHREAD

#define TimeOut                 _ENTITY_PREFIXED(TimeOut)

#endif

EXTERN BOOL TimeOut(USHORT*);

#ifdef _TARGET_
/*lint -e752 : local declarator not referenced */
EXTERN T_FD *fad_rbm_get_next_FrameDesc(void);  /* in entity FAD referenced */
EXTERN T_FD *tra_get_next_FrameDesc(void);      /* in entity L2R referenced */
EXTERN void rlp_rbm_get_next_buffer(T_FRAME_DESC *frameDesc); /* in entity RLP referenced */
#endif

#endif /* RA_H */
