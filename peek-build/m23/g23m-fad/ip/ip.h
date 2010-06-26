/*
+----------------------------------------------------------------------------
|  Project :  GSM-WAP (8444)
|  Modul   :  IP
+----------------------------------------------------------------------------
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
+----------------------------------------------------------------------------
|  Purpose :  Definitions for the Protocol Stack Entity
|             Internet Protocol
+----------------------------------------------------------------------------
*/

#ifndef IP_H
#define IP_H

/*-------------------------------------------------------------------------*/

#if defined WIN32 && ! defined _SIMULATION_
  #define _SIMULATION_
#endif

#ifdef _SIMULATION_
  #undef _TARGET_
#else
  #ifndef _TARGET_
    #define _TARGET_
  #endif
#endif

/* Information for DTI library */

#define IP_DTI_DEF_INSTANCE        0
#define IP_DTI_HL_INTERFACE        0
#define IP_DTI_LL_INTERFACE        1
#define IP_DTI_DEF_CHANNEL         0

#define IP_DTI_UPLINK_QUEUE_SIZE   0
#define IP_DTI_DOWNLINK_QUEUE_SIZE 0

/*==== Types =======================================================*/

/* Instance management */

#ifdef OPTION_MULTI_INSTANCE
  #define GET_INSTANCE(p) (ip_data_base + p->custom.route.inst_no)
  #define IP_INSTANCES MAX_INSTANCES
#else
  #define GET_INSTANCE(p) ip_data_base
  #define IP_INSTANCES 1
#endif

/*==== Constants ============================================================*/

#define MAX_LAYER 1 /* Connected to layers */

/* States for send and receive packets */

/* Constants for the IP packet */

#define MIN_HEADER_LEN    5 /* Min IP header length in 32 bit words */
#define MAX_PACKET_LEN 1500 /* Max packet len from higher layer */

/* Segmenting states and parameter */

#define NO_SEGMENTS          0 /* Normal packet */
#define SEND_SEGMENT         1 /* Sending segments */
#define READ_SEGMENT         2 /* Read segment */
#define GO_ON_SEGMENTING    10 /* Fragmenting ok - go on */
#define NO_SPACE_SEGMENTING 10 /* Not possible to sort cs. of less space */
#define MAX_SEGMENTS        50 /* Max received segments */
#define MAX_SEGM_SERVER      1 /* Max active server by rec. segments */

/* Errors */

#define NO_ERROR          0 /* No errors found */
#define ERR_PACKET_LEN    1 /* Packet length error */
#define CHANGE_PACKET_LEN 2 /* Length desc_list > total len indicated in header */

/* IP addresses */

#define MAX_ADDR_TYPES   15 /* Max field address-types */
#define MCAST_ADDR        0 /* Multicast address or class D address */
#define LINK_LAYER_BCAST  1 /* Link layer broadcast address */
#define LOOP_BACK_ADDR    2 /* Loopback address */
#define CLASS_E_ADDR      3 /* Class E address */
#define NO_DEST_ADDR      4 /* No destination address */
#define BCAST_ADDR_255    5 /* Broadcast address 255.255.255.255 */
#define NETW_ADDR         6 /* Network address */
#define CLASS_A_ADDR      7 /* Class A address */
#define CLASS_B_ADDR      8 /* Class B address */
#define CLASS_C_ADDR      9 /* Class C address */
#define DEFAULT_R_ADDR   10 /* Default route address */
#define BAD_UL_SRC_ADDR  11 /* Indicate bad uplink source address */
#define BAD_DL_SRC_ADDR  12 /* Indicate bad downlink source address */
#define BAD_UL_DEST_ADDR 13 /* Indicate bad uplink destination address */
#define BAD_DL_DEST_ADDR 14 /* Indicate bad uplink destination address */

/* Only for testing */

#define TEST_SRC_ADDR  0x0a0b0c0d /* Source test address */
#define TEST_DEST_ADDR 0x0a0b0c0e /* Destination test address */

/* Type of primitives */

#define P_DTI_READY       0
#define P_DTI_DATA_REQ    1
#define P_DTI_GETDATA_REQ 2
#define P_DTI_DATA_IND    3

/* Bit offset for encoding/decoding */

#define ENCODE_OFFSET 0

/* Timer */

#define TREASSEMBLY 0 /* Reassembly timer */

/* Timeout for reassembly segments */

#define TIME_REASSEMBLY 30000 /* 30 - 60 seconds is the correct value */

/* Dynamic configuration numbers */

#define RESET       0
#define FRAME_TRACE 6

/* State index for the entity process/services */

#define KER   ker.
#define HILA hila.
#define LOLA lola.

/* States of the entity process kernel */

#define DEACTIVATED 0
#define ACTIVE_NC   1 /* Not configured */
#define CONNECTED   2

/* States for send and receive packet services */

#define DOWN       0 /* State not active */
#define CONNECTING 1 /* State is connecting */
#define IDLE       2 /* State no incoming signals */
#define SEND       3 /* State sending packet */
#define WAIT       4 /* State waiting for packet */

/* For trace purposes */

#ifndef NTRACE
  #define SERVICE_NAME_KER   "KER"
  #define SERVICE_NAME_HILA "HILA"
  #define SERVICE_NAME_LOLA "LOLA"
#endif

/*============ TYPES ======================================================*/

/* Global variable-structure for uplink */

typedef struct
{
  UBYTE state; /* State uplink */
#ifndef NTRACE
  char * name;
  char * state_name;
#endif
  T_DTI2_DATA_REQ * dti_data_req; /* Data primitive uplink */
  BOOL drop_packet; /* TRUE: not sending packet to lower layer */
  UBYTE ttl; /* Time to live */
  USHORT segment_id; /* Number of the segment packet */
  USHORT header_len; /* Header length */
  UBYTE state_segment; /* State segmenting uplink */
  ULONG next_segment_desc; /* Pointer to next segment desc */
  USHORT last_segment_pos; /* Last position for copy in next_segment_desc */
  USHORT segment_offset; /* Offset for segmenting */
  USHORT sended_segment_len; /* Sended segment length include header */
  USHORT list_len_segment; /* Listlength of the income datagram */
  UBYTE segment_prot; /* Protokoll higher layer by fragmenting */
  T_desc2 * first_desc_segment; /* First desc for sending */
  USHORT identity; /* Sets the identity */
} T_HILA;

/* Global variable-structure for downlink */

typedef struct
{
  UBYTE state; /* State downlink */
#ifndef NTRACE
  char * name;
  char * state_name;
#endif
  T_DTI2_DATA_IND * dti_data_ind; /* Inputbuffer from higher layer */
  T_DTI2_DATA_IND * data_ind_reassembly [MAX_SEGM_SERVER]; /* For receiving segments */
  BOOL drop_packet; /* TRUE: not sending packet to lower layer */
  USHORT header_len; /* Header length */
  UBYTE state_reassembly [MAX_SEGM_SERVER]; /* State segmenting uplink */
  BOOL got_first_segment [MAX_SEGM_SERVER]; /* Got first segment by reassembling */
  BOOL got_last_segment [MAX_SEGM_SERVER]; /* Got last segment */
  ULONG ip_source_addr_segment [MAX_SEGM_SERVER]; /* IP source address for segment */
  USHORT pos_server; /* Position of fragmenting server */
  USHORT id_reassemble [MAX_SEGM_SERVER]; /* Identity of the reassemble fragments */
  UBYTE prot_reassemble [MAX_SEGM_SERVER]; /* Protocol reassembly */
  UBYTE timer_reass_running [MAX_SEGM_SERVER]; /* Timer is running */
  ULONG sort_reass_array [2] [MAX_SEGMENTS]; /* Used to sort the incoming fragments */
} T_LOLA;

/* Entity data for each IP instance */

typedef struct
{
  UBYTE state; /* State variables for each process */
#ifndef NTRACE
  char * name;
  char * state_name;
#endif
  ULONG source_addr;/* Source IP address */
  ULONG dst_addr; /* Destination address */
  ULONG peer_addr; /* Destination address for routing */
  ULONG netmask; /* Netmask for the routing table */
  char entity_name_hl [IPA_ENTITY_NAME_LEN]; /* Entity name higher layer */
  char entity_name_ll [IPA_ENTITY_NAME_LEN]; /* Entity name lower layer */
  ULONG link_id_hl;
  ULONG link_id_ll;
  USHORT mtu; /* Maximum transfer unit */
  BOOL send_icmp; /* ICMP message shall be sent */
  T_DTI2_DATA_REQ * icmp_dti_data_req; /* For sending ICMP message to uplink */
} T_KER;

typedef struct
{
  T_HILA hila; /* Data for each higher layer */
  T_LOLA lola; /* Data for each lower layer */
  T_KER ker;
} T_IP_DATA;

#ifdef GPRS
  typedef enum
  {
    UNIT_INVALID = -1,
    UNIT_SNDCP,
    UNIT_ACI,
    UNIT_UART,
    UNIT_PPPS,
    UNIT_PPPC,
    UNIT_L2R,
    UNIT_T30,
    UNIT_IP,
    UNIT_TRA,
    UNIT_UDP,
    UNIT_WAP,
    UNIT_MTST,
    UNIT_BLUETOOTH,
    UNIT_NULL,
    UNIT_MAX
  } IP_UNIT_ID;
#endif

/*==== EXPORT =====================================================*/

/* Data base */

extern DTI_HANDLE ip_hDTI;

/* Instance data base */

extern T_IP_DATA ip_data_base [], * ip_data;

/* DTI data base. The data base is allocated in function dti_init(). */

extern DTI_HANDLE udp_hDTI;

#define ENTITY_DATA ip_data

/* Prototypes timer */

#ifdef OPTION_MULTITHREAD
  #define tim_init_timer       _ENTITY_PREFIXED (tim_init_timer)
  #define tim_set_timeout_flag _ENTITY_PREFIXED (tim_set_timeout_flag)
  #define tim_handle_timeout   _ENTITY_PREFIXED (tim_handle_timeout)
  #define tim_flush_fifo       _ENTITY_PREFIXED (tim_flush_fifo)
  #define tim_reassembly       _ENTITY_PREFIXED (tim_reassembly)
#endif

BOOL tim_init_timer (void);

/* Timer specific timeout handling routines */

void tim_reassembly (/* USHORT layer, USHORT segm_server */);

/* Prototypes customer specific functions */

#ifdef OPTION_MULTITHREAD
  #define csf_init_timer  _ENTITY_PREFIXED (csf_init_timer)
  #define csf_close_timer _ENTITY_PREFIXED (csf_close_timer)
#endif

BOOL csf_init_timer (void);
void csf_close_timer (void);
void csf_stop_timer (USHORT index);
BOOL csf_timer_expired (
  USHORT index,
  USHORT * layer,
  USHORT * segm_server,
  USHORT * timer
);

/*==== Prototypes ==================================================*/

/* Prototypes
 *
 * IP KERNEL
 *
 * KERNEL primitive processing
 */

#ifdef OPTION_MULTITHREAD
  #define ker_ipa_activate_req   _ENTITY_PREFIXED (ker_ipa_activate_req)
  #define ker_ipa_deactivate_req _ENTITY_PREFIXED (ker_ipa_deactivate_req)
  #define ker_ipa_config_req     _ENTITY_PREFIXED (ker_ipa_config_req)
  #define ker_ip_addr_req        _ENTITY_PREFIXED (ker_ip_addr_req)

  #define sig_dti_ker_connection_opened_hl_ind \
    _ENTITY_PREFIXED (sig_dti_ker_connection_opened_hl_ind)
  #define sig_dti_ker_connection_opened_ll_ind \
    _ENTITY_PREFIXED (sig_dti_ker_connection_opened_ll_ind)
  #define sig_dti_ker_connection_closed_hl_ind \
    _ENTITY_PREFIXED (sig_dti_ker_connection_closed_hl_ind)
  #define sig_dti_ker_connection_closed_ll_ind \
    _ENTITY_PREFIXED (sig_dti_ker_connection_closed_ll_ind)
  #define sig_dti_ker_data_received_hl_ind \
    _ENTITY_PREFIXED (sig_dti_ker_data_received_hl_ind)
  #define sig_dti_ker_tx_buffer_ready_ll_ind \
    _ENTITY_PREFIXED (sig_dti_ker_tx_buffer_ready_ll_ind)
  #define sig_dti_ker_tx_buffer_full_hl_ind \
    _ENTITY_PREFIXED (sig_dti_ker_tx_buffer_full_hl_ind)
  #define sig_dti_ker_data_received_ll_ind \
    _ENTITY_PREFIXED (sig_dti_ker_data_received_ll_ind)
  #define sig_dti_ker_tx_buffer_ready_hl_ind \
    _ENTITY_PREFIXED (sig_dti_ker_tx_buffer_ready_hl_ind)
  #define sig_dti_ker_tx_buffer_full_ll_ind \
    _ENTITY_PREFIXED (sig_dti_ker_tx_buffer_full_ll_ind)

  #define pei_dti_dti_connect_req    _ENTITY_PREFIXED (pei_dti_dti_connect_req)
  #define pei_dti_dti_connect_cnf    _ENTITY_PREFIXED (pei_dti_dti_connect_cnf)
  #define pei_dti_dti_connect_ind    _ENTITY_PREFIXED (pei_dti_dti_connect_ind)
  #define pei_dti_dti_connect_res    _ENTITY_PREFIXED (pei_dti_dti_connect_res)
  #define pei_dti_dti_disconnect_req _ENTITY_PREFIXED (pei_dti_dti_disconnect_req)
  #define pei_dti_dti_disconnect_ind _ENTITY_PREFIXED (pei_dti_dti_disconnect_ind)
  #define pei_dti_dti_ready_ind      _ENTITY_PREFIXED (pei_dti_dti_ready_ind)
  #define pei_dti_dti_data_req       _ENTITY_PREFIXED (pei_dti_dti_data_req)
  #define pei_dti_dti_data_ind       _ENTITY_PREFIXED (pei_dti_dti_data_ind)
  #define pei_dti_dti_getdata_req    _ENTITY_PREFIXED (pei_dti_dti_getdata_req)

  #ifdef _SIMULATION_
    #define pei_dti_dti_data_test_req _ENTITY_PREFIXED (pei_dti_dti_data_test_req)
    #define pei_dti_dti_data_test_ind _ENTITY_PREFIXED (pei_dti_dti_data_test_ind)
  #endif

  #define pei_sig_callback _ENTITY_PREFIXED (pei_sig_callback)

  #define ip_packet_validator  _ENTITY_PREFIXED (ip_packet_validator)
  #define check_ip_address     _ENTITY_PREFIXED (check_ip_address)
  #define chk_packet_len       _ENTITY_PREFIXED (chk_packet_len)
  #define ip_addr_int_to_byte  _ENTITY_PREFIXED (ip_addr_int_to_byte)
  #define build_ip_packet      _ENTITY_PREFIXED (build_ip_packet)
  #define init_ip              _ENTITY_PREFIXED (init_ip)
  #define sort_descs_id_up     _ENTITY_PREFIXED (sort_descs_id_up)
  #define reassemble_fragments _ENTITY_PREFIXED (reassemble_fragments)
  #define config_down_ll       _ENTITY_PREFIXED (config_down_ll)
  #define terminate_ip         _ENTITY_PREFIXED (terminate_ip)
#endif

/* IPA SAP */

void ker_ipa_dti_req (T_IPA_DTI_REQ * ipa_dti_req);
void ker_ipa_config_req (T_IPA_CONFIG_REQ * ipa_config_req);

/* IP SAP */

void ker_ip_addr_req (T_IP_ADDR_REQ * ip_addr_req);

/* Wrapping functions for DTI library primitives */

void pei_dti_dti_connect_req (T_DTI2_CONNECT_REQ * dti_connect_req);
void pei_dti_dti_connect_cnf (T_DTI2_CONNECT_CNF * dti_connect_cnf);
void pei_dti_dti_connect_ind (T_DTI2_CONNECT_IND * dti_connect_ind);
void pei_dti_dti_connect_res (T_DTI2_CONNECT_RES * dti_connect_res);
void pei_dti_dti_disconnect_req (T_DTI2_DISCONNECT_REQ * dti_disconnect_req);
void pei_dti_dti_disconnect_ind (T_DTI2_DISCONNECT_IND * dti_disconnect_ind);
void pei_dti_dti_data_req (T_DTI2_DATA_REQ * dti_data_req);
void pei_dti_dti_getdata_req (T_DTI2_GETDATA_REQ * dti_getdata_req);
void pei_dti_dti_data_ind (T_DTI2_DATA_IND * dti_data_ind);
void pei_dti_dti_ready_ind (T_DTI2_READY_IND * dti_ready_ind);
#ifdef _SIMULATION_
  void pei_dti_dti_data_test_req (T_DTI2_DATA_TEST_REQ * dti_data_test_req);
  void pei_dti_dti_data_test_ind (T_DTI2_DATA_TEST_IND * dti_data_test_ind);
#endif

/* UP DTI */

void sig_dti_ker_data_received_hl_ind (T_DTI2_DATA_REQ * dti_data_req);
void sig_dti_ker_connection_opened_hl_ind ();
void sig_dti_ker_connection_closed_hl_ind ();
void sig_dti_ker_tx_buffer_ready_ll_ind ();
void sig_dti_ker_tx_buffer_full_hl_ind ();

/* DN DTI */

void sig_dti_ker_data_received_ll_ind (T_DTI2_DATA_IND * dti_data_ind);
void sig_dti_ker_connection_opened_ll_ind ();
void sig_dti_ker_connection_closed_ll_ind ();
void sig_dti_ker_tx_buffer_ready_hl_ind ();
void sig_dti_ker_tx_buffer_full_ll_ind ();

/* DTILIB callback function */

void pei_sig_callback (
  U8 instance,
  U8 interfac,
  U8 channel,
  U8 reason,
  T_DTI2_DATA_IND * dti_data_ind
);

/* Functions in ip_kerf.c */

BOOL ip_packet_validator (T_desc_list2 * desc_list);
void build_ip_packet (BOOL uplink, UBYTE select);
void init_ip (void);
void terminate_ip (void);
void config_down_ll (void);
void check_ip_address (
  BOOL * addr_type_dest,
  BOOL * addr_type_src,
  ULONG dest_addr,
  ULONG src_addr
);
void ip_addr_int_to_byte (UBYTE * b_values, ULONG ul_value);
UBYTE chk_packet_len (UBYTE * ip_header, T_desc_list2 * desc_list);
USHORT sort_descs_id_up (
  T_desc_list2 * desc_list,
  ULONG sort_array[2][MAX_SEGMENTS]
);
void reassemble_fragments (
  T_DTI2_DATA_IND ** dti_data_ind,
  T_LOLA * p_dl,
  UBYTE * ip_header,
  BOOL first_segment,
  /* BOOL middle_segment, */
  BOOL last_segment /*,*/
  /* USHORT fragm_offset */
);

/* Communication handles */

#ifdef OPTION_MULTITHREAD
  #define hCommMMI _ENTITY_PREFIXED (hCommMMI)
  #define hCommDL _ENTITY_PREFIXED (hCommDL)
  #define hCommIP _ENTITY_PREFIXED (hCommIP)
  #define hCommHL _ENTITY_PREFIXED (hCommHL)
#endif

extern T_HANDLE hCommMMI; /* ACI communication */
extern T_HANDLE hCommDL; /* HL communication */
extern T_HANDLE hCommIP; /* IP communication */
extern T_HANDLE hCommHL; /* HL communication */
extern T_HANDLE ip_handle;

/* Entity common buffer for the decoded air interface message */

#ifdef OPTION_MULTITHREAD
  #define _decodedMsg _ENTITY_PREFIXED (_decodedMsg)
#endif

extern UBYTE _decodedMsg [];

#ifndef NCONFIG
  #ifdef OPTION_MULTITHREAD
    #define partab _ENTITY_PREFIXED (partab)
  #endif
  extern KW_DATA partab [];
#endif

#ifdef _SIMULATION_
  #define Sprintf sprintf
#endif

/* Number of signals (maximal signalNum which occurs in pei_signal) */

#define MAX_SIGNALS 10

/*-------------------------------------------------------------------------*/

#endif /* IP_H */

/*-------------------------------------------------------------------------*/

