/*
+----------------------------------------------------------------------------
| Project: GSM-WAP (8444)
| Modul: UDP
+----------------------------------------------------------------------------
| Copyright 2002 Texas Instruments Berlin, AG
|                All rights reserved.
|
|                This file is confidential and a trade secret of Texas
|                Instruments Berlin, AG
|                The receipt of or possession of this file does not convey
|                any rights to reproduce or disclose its contents or to
|                manufacture, use, or sell anything it may describe, in
|                whole, or in part, without the specific written consent of
|                Texas Instruments Berlin, AG.
+----------------------------------------------------------------------------
| Purpose: Definitions for the Protocol Stack Entity
|          Internet Protocol
+----------------------------------------------------------------------------
*/

#ifndef UDP_H
#define UDP_H

/*-------------------------------------------------------------------------*/

#define UDP_SINGLE_LAYER 0

/* Information for DTI library */

#define UDP_DTI_DEF_INSTANCE UDP_SINGLE_LAYER

#define UDP_DTI_LL_INTERFACE 0 /* Lower layer interface */
#define UDP_DTI_HL_INTERFACE 1 /* Higher layer interface */

#define UDP_DTI_DEF_CHANNEL 0
#define UDP_DTI_INIT_QUEUE_SIZE 0

#include "ip_udp.h"

/*---- Types --------------------------------------------------------------*/

/* Instance management */

#ifdef OPTION_MULTI_INSTANCE
  #define GET_INSTANCE(p) (udp_data_base + p->custom.route.inst_no)
  #define UDP_INSTANCES MAX_INSTANCES
#else
  #define GET_INSTANCE(p) (udp_data_base + UDP_SINGLE_LAYER)
  #define UDP_INSTANCES 1
#endif

/*---- Constants ----------------------------------------------------------*/

/* Constants for the UDP packet */

#define MIN_HEADER_LEN       5 /* Min IP header length in 32 bit words */

/* Errors */
#define NO_ERROR             0 /* No errors found */

#define LEN_PSEUDO_HEADER_B 12 /* Length of pseudo header in bytes */

/* States of higher layer port */

#define PORT_DOWN            0
#define PORT_BOUND           1
#define PORT_ACTIVE          2

/* States for error downlink */

#define STATE_DL_NO_ERROR    0 /* No errors */
#define STATE_DL_ERROR       1 /* We have one or more errors */

/* State index for the entity process/services */

#define KER ker.
#define HILA hila.
#define LOLA lola.

/* States of the entity process kernel */

#define DEACTIVATED          0 /* */
#define ACTIVE_NC            1 /* Not configured */
#define CONNECTED            2 /* */

/* States for send and receive packet services */

#define DOWN                 0 /* State not active */
#define CONNECTING           1 /* State is connecting */
#define IDLE                 2 /* State no incoming signals */
#define SEND                 3 /* State sending packet */
#define WAIT                 4 /* State waiting for packet */

#ifndef NTRACE
  /* For trace purposes */
  #define SERVICE_NAME_KER "KER"
  #define SERVICE_NAME_HILA "HILA"
  #define SERVICE_NAME_LOLA "LOLA"
#endif

/*---- TYPES --------------------------------------------------------------*/

/* Later on included between WAP and UDP - To do */

typedef struct
{
  UBYTE src_ip[4];
  UBYTE des_ip[4];
  UBYTE src_port[2];
  UBYTE des_port[2];
} T_SRC_DES;

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char * name;
  char * state_name;
#endif
  UBYTE dti_state;
  ULONG link_id;
  BOOL drop_packet; /* TRUE: not sending packet to lower layer */
  char entity_name [UDPA_ENTITY_NAME_LEN];

  T_DTI2_DATA_REQ * dti_data_req; /* Data primitive to lower layer */
} T_HILA;

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char * name;
  char * state_name;
#endif
  UBYTE dti_state;
  ULONG link_id;
  BOOL drop_packet; /* TRUE: not sending packet to higher layer */
  char entity_name [UDPA_ENTITY_NAME_LEN];

  UBYTE state_err; /* State variable for errors */
  ULONG src_addr; /* Source address for DL datagram */
  ULONG dst_addr; /* Destination address for DL datagram */
  USHORT dst_port; /* Destination port for DL datagram */
  USHORT src_port; /* Source port for DL datagram */

  T_DTI2_DATA_IND * dti_data_ind; /* We are sending data to higher layer */
} T_LOLA;

typedef struct
{
  UBYTE state;
#ifndef NTRACE
  char * name;
  char * state_name;
#endif
  BOOL send_icmp; /* We send an ICMP message */
  UBYTE port_state; /* State of the source port */
  USHORT src_port; /* Source port */
  USHORT dst_port; /* Destination port */
  ULONG src_addr; /* Source address in the IP packet */
  ULONG dst_addr; /* Destination IP address */

  T_DTI2_DATA_REQ * dti_data_req_icmp; /* For ICMP messages */
} T_KER;

/* Entity data for each UDP instance */

typedef struct
{
  T_HILA hila; /* Data for each higher layer */
  T_LOLA lola; /* Data for each lower layer */
  T_KER ker;
} T_UDP_DATA;

/*---- EXPORT -------------------------------------------------------------*/

/* Instance data base */

extern T_UDP_DATA udp_data_base [], * udp_data;

/* The DTI data base is allocated in function dti_init() */

extern DTI_HANDLE udp_hDTI;

#define ENTITY_DATA udp_data

/*---- Prototypes ---------------------------------------------------------*/

/* Prototypes
 *
 * UDP KERNEL
 *
 * KERNEL primitive processing */

#ifdef OPTION_MULTITHREAD

  #define ker_udp_shutdown_res _ENTITY_PREFIXED (ker_udp_shutdown_res)
  #define ker_udp_bind_req _ENTITY_PREFIXED (ker_udp_bind_req)
  #define ker_udp_closeport_req _ENTITY_PREFIXED (ker_udp_closeport_req)
  #define ker_udp_error_res _ENTITY_PREFIXED (ker_udp_error_res)
  #define ker_udpa_dti_req _ENTITY_PREFIXED (ker_udpa_dti_req)
  #define ker_udpa_config_req _ENTITY_PREFIXED (ker_udpa_config_req)

  #define sig_dti_ker_connection_opened_ll_ind \
    _ENTITY_PREFIXED (sig_dti_ker_connection_opened_ll_ind)
  #define sig_dti_ker_connection_opened_hl_ind \
    _ENTITY_PREFIXED (sig_dti_ker_connection_opened_hl_ind)
  #define sig_dti_ker_connection_closed_ll_ind \
    _ENTITY_PREFIXED (sig_dti_ker_connection_closed_ll_ind)
  #define sig_dti_ker_connection_closed_hl_ind \
    _ENTITY_PREFIXED (sig_dti_ker_connection_closed_hl_ind)
  #define sig_dti_ker_data_received_ll_ind \
    _ENTITY_PREFIXED (sig_dti_ker_data_received_ll_ind)
  #define sig_dti_ker_tx_buffer_ready_hl_ind \
    _ENTITY_PREFIXED (sig_dti_ker_tx_buffer_ready_hl_ind)
  #define sig_dti_ker_tx_buffer_full_hl_ind \
    _ENTITY_PREFIXED (sig_dti_ker_tx_buffer_full_hl_ind)
  #define sig_dti_ker_data_received_hl_ind \
    _ENTITY_PREFIXED (sig_dti_ker_data_received_hl_ind)
  #define sig_dti_ker_tx_buffer_ready_ll_ind \
    _ENTITY_PREFIXED (sig_dti_ker_tx_buffer_ready_ll_ind)
  #define sig_dti_ker_tx_buffer_full_ll_ind \
    _ENTITY_PREFIXED (sig_dti_ker_tx_buffer_full_ll_ind)

  #define ker_ip_addr_cnf _ENTITY_PREFIXED (ker_ip_addr_cnf)
  #define udp_build_packet _ENTITY_PREFIXED (udp_build_packet)
  #define init_udp _ENTITY_PREFIXED (init_udp)
  #define deinit_udp _ENTITY_PREFIXED (deinit_udp)

  #define pei_dti_dti_connect_req \
    _ENTITY_PREFIXED (pei_dti_dti_connect_req)
  #define pei_dti_dti_connect_cnf \
    _ENTITY_PREFIXED (pei_dti_dti_connect_cnf)
  #define pei_dti_dti_connect_ind \
    _ENTITY_PREFIXED (pei_dti_dti_connect_ind)
  #define pei_dti_dti_connect_res \
    _ENTITY_PREFIXED (pei_dti_dti_connect_res)
  #define pei_dti_dti_disconnect_req \
    _ENTITY_PREFIXED (pei_dti_dti_disconnect_req)
  #define pei_dti_dti_disconnect_ind \
    _ENTITY_PREFIXED (pei_dti_dti_disconnect_ind)
  #define pei_dti_dti_ready_ind \
    _ENTITY_PREFIXED (pei_dti_dti_ready_ind)
  #define pei_dti_dti_data_req \
    _ENTITY_PREFIXED (pei_dti_dti_data_req)
  #define pei_dti_dti_data_ind \
    _ENTITY_PREFIXED (pei_dti_dti_data_ind)
  #define pei_dti_dti_getdata_req \
    _ENTITY_PREFIXED (pei_dti_dti_getdata_req)
  #define pei_sig_callback \
  _ENTITY_PREFIXED (pei_sig_callback)

  #ifdef _SIMULATION_
    #define pei_dti_dti_data_test_req \
      _ENTITY_PREFIXED (pei_dti_dti_data_test_req)
    #define pei_dti_dti_data_test_ind \
      _ENTITY_PREFIXED (pei_dti_dti_data_test_ind)
  #endif

#endif

USHORT inet_checksum (UBYTE * block, USHORT len);
USHORT desc_checksum (
  T_desc_list2 * dlist,
  USHORT start_offset,
  ULONG start_value
);
ULONG partial_checksum (UBYTE * block, USHORT len);

/* UDP SAP */

void ker_udp_shutdown_res (T_UDP_SHUTDOWN_RES * udp_shutdown_res);
void ker_udp_bind_req (T_UDP_BIND_REQ * udp_bind_req);
void ker_udp_closeport_req (T_UDP_CLOSEPORT_REQ * udp_closeport_req);
void ker_udp_error_res (T_UDP_ERROR_RES * udp_error_res);

/* UDPA SAP */

void ker_udpa_dti_req (T_UDPA_DTI_REQ * udpa_dti_req);
void ker_udpa_config_req (T_UDPA_CONFIG_REQ * udpa_config_req);

/* Wrapping functions for dtilib primitives */

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

void sig_dti_ker_connection_opened_hl_ind ();
void sig_dti_ker_connection_closed_hl_ind ();
void sig_dti_ker_tx_buffer_ready_hl_ind ();
void sig_dti_ker_tx_buffer_full_hl_ind ();
void sig_dti_ker_data_received_hl_ind (T_DTI2_DATA_REQ * dti_data_req);

/* DN DTI */

void sig_dti_ker_connection_opened_ll_ind ();
void sig_dti_ker_connection_closed_ll_ind ();
void sig_dti_ker_tx_buffer_ready_ll_ind ();
void sig_dti_ker_tx_buffer_full_ll_ind ();
void sig_dti_ker_data_received_ll_ind (T_DTI2_DATA_IND * dti_data_ind);

/* UL IP SAP */

void ker_ip_addr_cnf (T_IP_ADDR_CNF * ip_addr_cnf);

/* DTILIB callback function */

void pei_sig_callback (
  U8 instance,
  U8 interfac,
  U8 channel,
  U8 reason,
  T_DTI2_DATA_IND * dti_data_ind
);

/* Functions in udp_kerf.c */

void udp_build_packet (BOOL hila, UBYTE to_do);

void init_udp (void);
void deinit_udp (void);

/* Communication handles */

#ifdef OPTION_MULTITHREAD
  #define hCommACI _ENTITY_PREFIXED (hCommACI)
  #define hCommLL _ENTITY_PREFIXED (hCommLL)
  #define hCommHL _ENTITY_PREFIXED (hCommHL)
#endif

extern T_HANDLE hCommACI; /* ACI communication */
extern T_HANDLE hCommLL; /* LL communication */
extern T_HANDLE hCommHL; /* HL communication */
extern T_HANDLE udp_handle;

#ifndef NCONFIG
  #ifdef OPTION_MULTITHREAD
    #define partab _ENTITY_PREFIXED (partab)
  #endif
  extern KW_DATA partab [];
#endif

/*-------------------------------------------------------------------------*/

#endif /* UDP_H */

/*-------------------------------------------------------------------------*/

