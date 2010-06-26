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

#ifndef IP_UDP_H
#define IP_UDP_H

/*-------------------------------------------------------------------------*/

#include "typedefs.h"

/*---- Macros for the IP header -------------------------------------------*/

#define SET_IP_VERSION(ip_header,value) (\
  ((UBYTE*)(ip_header))[0] = (UBYTE) \
    ((value)<<4 & 0xF0 | ((UBYTE*)(ip_header))[0] & 0x0F) \
)

#define SET_IP_HEADER_LEN(ip_header,value) (\
  ((UBYTE*)(ip_header))[0] = (UBYTE) \
    (((UBYTE*)(ip_header))[0] & 0xF0 | (value) & 0x0F) \
)

#define SET_IP_TYPE_OF_SERVICE(ip_header,value) (\
  ((UBYTE*)(ip_header))[1] = (UBYTE) (value) \
)

#define SET_IP_TOTAL_LEN(ip_header,value) (\
  ((UBYTE*)(ip_header))[2] = (UBYTE) ((value)>>8), \
  ((UBYTE*)(ip_header))[3] = (UBYTE) (value) \
)

#define SET_IP_IDENT(ip_header,value) (\
  ((UBYTE*)(ip_header))[4] = (UBYTE) ((value)>>8), \
  ((UBYTE*)(ip_header))[5] = (UBYTE) (value) \
)

#define SET_IP_OFF_FLAG(ip_header,value) (\
  ((UBYTE*)(ip_header))[6] = (UBYTE) \
    (((UBYTE*)(ip_header))[6] & 0x7F | (value)<<7 & 0x80) \
)

#define SET_IP_DF_FLAG(ip_header,value) (\
  ((UBYTE*)(ip_header))[6] = (UBYTE) \
    (((UBYTE*)(ip_header))[6] & 0xBF | (value)<<6 & 0x40) \
)

#define SET_IP_MF_FLAG(ip_header,value) (\
  ((UBYTE*)(ip_header))[6] = (UBYTE) \
    (((UBYTE*)(ip_header))[6] & 0xDF | (value)<<5 & 0x20) \
)

#define SET_IP_FRAG_OFFSET(ip_header,value) (\
  ((UBYTE*)(ip_header))[6] = (UBYTE) \
    (((UBYTE*)(ip_header))[6] & 0xE0 | (value)>>8 & 0x1F), \
  ((UBYTE*)(ip_header))[7] = (UBYTE) (value) \
)

#define SET_IP_TTL(ip_header,value) (\
  ((UBYTE*)(ip_header))[8] = (UBYTE) (value) \
)

#define SET_IP_PROT(ip_header,value) (\
  ((UBYTE*)(ip_header))[9] = (UBYTE) (value) \
)

#define SET_IP_CHECKSUM(ip_header,value) (\
  ((UBYTE*)(ip_header))[10] = (UBYTE) (value), \
  ((UBYTE*)(ip_header))[11] = (UBYTE) ((value)>>8) \
) /* please take note of the reversed byte order */

#define RESET_IP_CHECKSUM(ip_header) (\
  ((UBYTE*)(ip_header))[10] = 0, \
  ((UBYTE*)(ip_header))[11] = 0 \
)

#define SET_IP_SOURCE_ADDR(ip_header,value) (\
  ((UBYTE*)(ip_header))[12] = (UBYTE) ((value)>>24), \
  ((UBYTE*)(ip_header))[13] = (UBYTE) ((value)>>16), \
  ((UBYTE*)(ip_header))[14] = (UBYTE) ((value)>>8), \
  ((UBYTE*)(ip_header))[15] = (UBYTE) (value) \
)

#define SET_IP_DEST_ADDR(ip_header,value) (\
  ((UBYTE*)(ip_header))[16] = (UBYTE) ((value)>>24), \
  ((UBYTE*)(ip_header))[17] = (UBYTE) ((value)>>16), \
  ((UBYTE*)(ip_header))[18] = (UBYTE) ((value)>>8), \
  ((UBYTE*)(ip_header))[19] = (UBYTE) (value) \
)

#define GET_IP_VERSION(ip_header) (\
  ((UBYTE*)(ip_header))[0]>>4 \
)

#define GET_IP_HEADER_LEN(ip_header) (\
  ((UBYTE*)(ip_header))[0] & 0xF \
)

#define GET_IP_HEADER_LEN_B(ip_header) (\
  GET_IP_HEADER_LEN (ip_header) << 2 \
)

#define GET_IP_TYPE_OF_SERVICE(ip_header) (\
  ((UBYTE*)(ip_header))[1] \
)

#define GET_IP_TOTAL_LEN(ip_header) (\
  ((UBYTE*)(ip_header))[2]<<8 | ((UBYTE*)(ip_header))[3] \
)

#define GET_IP_IDENT(ip_header) (\
  ((UBYTE*)(ip_header))[4]<<8 | ((UBYTE*)(ip_header))[5] \
)

#define GET_IP_OFF_FLAG(ip_header) (\
  ((UBYTE*)(ip_header))[6]>>7 & 0x1 \
)

#define GET_IP_DF_FLAG(ip_header) (\
  ((UBYTE*)(ip_header))[6]>>6 & 0x1 \
)

#define GET_IP_MF_FLAG(ip_header) (\
  ((UBYTE*)(ip_header))[6]>>5 & 0x1 \
)

#define GET_IP_FRAG_OFFSET(ip_header) (\
  (((UBYTE*)(ip_header))[6] & 0x1F) << 8 | ((UBYTE*)(ip_header))[7] \
)

#define GET_IP_FRAG_OFFSET_B(ip_header) (\
  GET_IP_FRAG_OFFSET (ip_header) << 3 \
)

#define GET_IP_TTL(ip_header) (\
  ((UBYTE*)(ip_header))[8] \
)

#define GET_IP_PROT(ip_header) (\
  ((UBYTE*)(ip_header))[9] \
)

#define GET_IP_CHECKSUM(ip_header) (\
  ((UBYTE*)(ip_header))[10] | ((UBYTE*)(ip_header))[11]<<8 \
) /* please take note of the reversed byte order */

#define GET_IP_SOURCE_ADDR(ip_header) (\
  ((UBYTE*)(ip_header))[12]<<24 | \
  ((UBYTE*)(ip_header))[13]<<16 | \
  ((UBYTE*)(ip_header))[14]<<8 | \
  ((UBYTE*)(ip_header))[15] \
)

#define GET_IP_DEST_ADDR(ip_header) (\
  ((UBYTE*)(ip_header))[16]<<24 | \
  ((UBYTE*)(ip_header))[17]<<16 | \
  ((UBYTE*)(ip_header))[18]<<8 | \
  ((UBYTE*)(ip_header))[19] \
)

/*---- Macros for the UDP header ------------------------------------------*/

#define SET_UDP_SRC_PORT(ip_header,value) (\
  ((UBYTE*)(ip_header))[20] = (UBYTE) ((value)>>8), \
  ((UBYTE*)(ip_header))[21] = (UBYTE) (value) \
)

#define SET_UDP_DST_PORT(ip_header,value) (\
  ((UBYTE*)(ip_header))[22] = (UBYTE) ((value)>>8), \
  ((UBYTE*)(ip_header))[23] = (UBYTE) (value) \
)

#define SET_UDP_LEN(ip_header,value) (\
  ((UBYTE*)(ip_header))[24] = (UBYTE) ((value)>>8), \
  ((UBYTE*)(ip_header))[25] = (UBYTE) (value) \
)

#define SET_UDP_CHKSUM(ip_header,offset,value) (\
  ((UBYTE*)(ip_header))[(offset)+6] = (UBYTE) (value), \
  ((UBYTE*)(ip_header))[(offset)+7] = (UBYTE) ((value)>>8) \
) /* please take note of the reversed byte order */

#define RESET_UDP_CHKSUM(ip_header,offset) (\
  ((UBYTE*)(ip_header))[(offset)+6] = 0, \
  ((UBYTE*)(ip_header))[(offset)+7] = 0 \
)

#define GET_UDP_SRC_PORT(ip_header,offset) (\
  ((UBYTE*)(ip_header))[offset]<<8 | ((UBYTE*)(ip_header))[(offset)+1] \
)

#define GET_UDP_DST_PORT(ip_header,offset) (\
  ((UBYTE*)(ip_header))[(offset)+2]<<8 | ((UBYTE*)(ip_header))[(offset)+3] \
)

#define GET_UDP_LEN(ip_header,offset) (\
  ((UBYTE*)(ip_header))[(offset)+4]<<8 | ((UBYTE*)(ip_header))[(offset)+5] \
)

#define GET_UDP_CHK_SUM(ip_header,offset) (\
  ((UBYTE*)(ip_header))[(offset)+6] | ((UBYTE*)(ip_header))[(offset)+7]<<8 \
) /* please take note of the reversed byte order */

/*---- Macros for the UDP pseudo header -----------------------------------*/

#define SET_UDP_PSEUDO_H_SRC_ADDR(ip_header,value) (\
  ((UBYTE*)(ip_header))[0] = (UBYTE) ((value)>>24), \
  ((UBYTE*)(ip_header))[1] = (UBYTE) ((value)>>16), \
  ((UBYTE*)(ip_header))[2] = (UBYTE) ((value)>>8), \
  ((UBYTE*)(ip_header))[3] = (UBYTE) (value) \
)

#define SET_UDP_PSEUDO_H_DEST_ADDR(ip_header,value) (\
  ((UBYTE*)(ip_header))[4] = (UBYTE) ((value)>>24), \
  ((UBYTE*)(ip_header))[5] = (UBYTE) ((value)>>16), \
  ((UBYTE*)(ip_header))[6] = (UBYTE) ((value)>>8), \
  ((UBYTE*)(ip_header))[7] = (UBYTE) (value) \
)

#define SET_UDP_PSEUDO_H_ZERO(ip_header,value) (\
  ((UBYTE*)(ip_header))[8] = (UBYTE) (value) \
)

#define SET_UDP_PSEUDO_H_PROT(ip_header,value) (\
  ((UBYTE*)(ip_header))[9] = (UBYTE) (value) \
)

#define SET_UDP_PSEUDO_H_LEN(ip_header,value) (\
  ((UBYTE*)(ip_header))[10] = (UBYTE) ((value)>>8), \
  ((UBYTE*)(ip_header))[11] = (UBYTE) (value) \
)

/*---- Macros for the ICMP header -----------------------------------------*/

#define SET_ICMP_TYPE(ip_header,value,offset) (\
  ((UBYTE*)(ip_header))[offset] = (UBYTE) (value) \
)

#define SET_ICMP_CODE(ip_header,value,offset) (\
  ((UBYTE*)(ip_header))[(offset)+1] = (UBYTE) (value) \
)

#define SET_ICMP_CHK_SUM(ip_header,value,offset) (\
  ((UBYTE*)(ip_header))[(offset)+2] = (UBYTE) (value), \
  ((UBYTE*)(ip_header))[(offset)+3] = (UBYTE) ((value)>>8) \
) /* please take note of the reversed byte order */

#define RESET_ICMP_CHK_SUM(ip_header,offset) (\
  ((UBYTE*)(ip_header))[(offset)+2] = 0, \
  ((UBYTE*)(ip_header))[(offset)+3] = 0 \
)

#define SET_ICMP_IDENT(ip_header,value,offset) (\
  ((UBYTE*)(ip_header))[(offset)+4] = (UBYTE) ((value)>>8), \
  ((UBYTE*)(ip_header))[(offset)+5] = (UBYTE) (value) \
)

#define SET_ICMP_SEQ_NR(ip_header,value,offset) (\
  ((UBYTE*)(ip_header))[(offset)+6] = (UBYTE) ((value)>>8), \
  ((UBYTE*)(ip_header))[(offset)+7] = (UBYTE) (value) \
)

#define GET_ICMP_TYPE(ip_header,offset) (\
  ((UBYTE*)(ip_header))[offset] \
)

#define GET_ICMP_CODE(ip_header,offset) (\
  ((UBYTE*)(ip_header))[(offset)+1] \
)

#define GET_ICMP_CHK_SUM(ip_header,offset) (\
  ((UBYTE*)(ip_header))[(offset)+2] | ((UBYTE*)(ip_header))[(offset)+3]<<8 \
) /* please take note of the reversed byte order */

#define GET_ICMP_IDENT(ip_header,offset) (\
  ((UBYTE*)(ip_header))[(offset)+4]<<8 | ((UBYTE*)(ip_header))[(offset)+5] \
)

#define GET_ICMP_SEQ_NR(ip_header,offset) (\
  ((UBYTE*)(ip_header))[(offset)+6]<<8 | ((UBYTE*)(ip_header))[(offset)+7] \
)

/*---- Constants ----------------------------------------------------------*/

/* Types of primitives */

#define P_DTI_READY_IND            0
#define P_DTI_DATA_REQ             1
#define P_DTI_GETDATA_REQ          2
#define P_DTI_DATA_IND             3
#define P_IP_ADDR_CNF              4
#define P_UDP_ERROR_RES            5
#define P_UDP_GETDATA_REQ          6
#define P_UDP_BIND_REQ             7
#define P_UDP_CONFIG_REQ           8
#define P_UDPA_CONFIG_REQ          9
#define P_DTI_DATA_TEST_IND       10
#define P_DTI_DATA_TEST_REQ       11
#define P_UDP_CLOSEPORT_REQ       12
#define P_TIMER_REASSEMBLY        13
#define P_UDPA_ACTIVATE_REQ       14

/* For building packets */

#define B_NORMAL_PACKET            1 /* Build normal packet */
#define B_SEGMENT                  2 /* Build segment packet */
#define B_ICMP_ECHO                3 /* Build ICMP echo packet */
#define B_ICMP_ECHO_REPLY          4 /* Build ICMP echo reply packet */
#define B_ICMP_TTL_0               5 /* Build TTL packet */
#define B_ICMP_NO_FORWARD          6 /* Build ICMP when no forward */
#define B_IP_FORWARD               7 /* Build packet forward */
#define B_ICMP_TIME_EX             8 /* Time exceeded by segmenting */
#define B_ICMP_PACKET              9 /* ICMP packet */
#define B_NO_DEST_PORT            10 /* No destination port */
#define B_ICMP_REASSEMBLE_TIMEOUT 11 /* Build ICMP when timeout reassembly */

#define BUILD_NO_PACKET            0 /* No packet shall be built */

/* For IP packet header */

#define IP_VERSION                 4 /* IP version number */
#define ICMP_PROT                  1 /* ICMP protocol */
#define UDP_PROT                  17 /* UDP protocol */
#define DL_SEGMENT                 1 /* DL segment */
#define NORMAL_SERVICE             0 /* No services */
#define NO_SEGMENT_ID              0 /* No segment ID */
#define START_SEGMENT_ID           0 /* Offset for identity segmenting */
#define IDENTITY_0                 0 /* Identity set to zero */
#define FLAG_NOT_SET               0 /* Flag zero */
#define FLAG_SET                   1 /* Flag one */
#define NO_OFFSET_FRAG             0 /* No offset for fragmenting */
#define STANDARD_TTL              64 /* Standard TTL by initialisation */
#define LEN_IP_HEADER_B           20 /* Minimum length of IP header in bytes */

/* ICMP messages */

#define ICMP_ECHO_REPLY            0 /* ICMP ping reply */
#define LEN_ICMP_ECHO_REPLY       64 /* Datagram ICMP echo reply in bytes */
#define LEN_ICMP_TIMEOUT         128 /* Datagram ICMP timeout in bytes */

#define ICMP_CODE_ECHO_REPLY       0 /* ICMP ping reply */
#define ICMP_CODE_NO_HOST          1 /* No destination host*/
#define ICMP_CODE_NO_FORWARD       0 /* No forward route */
#define ICMP_CODE_NO_PORT          3 /* No destination port */
#define ICMP_CODE_FRAGM_TIME_EXC   1 /* Reassembly time exceeded */

#define ICMP_TYP_DEST_URECHBL      3 /* Cannot reach the destination */
#define ICMP_TYP_ECHO_REPLY        0 /* Ping request type */
#define ICMP_TYP_ECHO              8 /* Ping */
#define ICMP_TYP_TIME_EXCEDED     11 /* Timeout or TTL is 0 */

/* For UDP packet header */

#define LEN_UDP_HEADER_B           8 /* Length of UDP header in bytes */

/* For init */

#define NO_TUI                     0
#define NO_CH_ID                   0
#define NO_ADDR                    0
#define NO_PORT                    0
#define NO_PROT_ID                 0
#define NO_MTU                     0
#define INIT_NETMASK               0
#define NO_ID_REASSBL              0
#define NO_PROT_REASSBL            0
#define NO_TIMER                   0

/*---- Prototypes ---------------------------------------------------------*/

void table_state_tui (
  void * primitive,
  UBYTE typ,
  UBYTE * akt_layer
);

void free_dti_data_req (T_DTI2_DATA_REQ ** dti_data_req);

void free_dti_data_ind (T_DTI2_DATA_IND ** dti_data_ind);

/* The following functions are actually defined in the UDP module.
 * Due to unfortunate decisions in the past they must still be
 * declared here redundantly, but should really be declared in a
 * common include file. No time to fix this now, though.
 * [ni 2002-06-18]
 */

/** Computes the internet checksum [RFC 1071] over a data block.
 *
 * @param block          pointer to data block
 * @param len            length of the block in octets
 * @return the checksum
 */
USHORT inet_checksum (UBYTE * block, USHORT len);

/** Computes the internet checksum over a DTI descriptor list. There
 * may be a value from a previous partial calculation that is added to
 * the sum as a start value. The function relies on the data length
 * being at least start_offset.
 *
 * @param dlist          descriptor list containing the data
 * @param start_offset   beginning position of interesting data
 * @param start_value    value from previous partial checksum calculation
 * @return the checksum value
 */
USHORT desc_checksum (
  T_desc_list2 * dlist,
  USHORT start_offset,
  ULONG start_value
);

void del_rest_descs (T_desc_list2 * desc_list, T_desc2 * desc);

void put_desc_last_pos (T_desc_list2 * desc_list, T_desc2 * desc);

void put_desc_first_pos (T_desc_list2 * desc_list, T_desc2 * desc_new);

void build_icmp_with_payload (
  T_DTI2_DATA_REQ * data_req,
  USHORT identity,
  UBYTE ttl,
  ULONG src_addr,
  UBYTE icmp_type,
  UBYTE icmp_code
);

void set_desc_len (
  T_desc_list2 * desc_list,
  T_desc2 * desc,
  USHORT len_desc
);

void copy_from_descs_to_desc (
  T_desc2 ** desc,
  T_desc2 * desc_new,
  USHORT copy_len,
  USHORT offset_desc,
  USHORT offset_desc_new,
  USHORT * pos_copy,
  USHORT * total_len_copy
);

void del_descs (T_desc2 * p_desc);

void build_icmp_packet (
  USHORT header_len_b,
  UBYTE typ,
  UBYTE code,
  UBYTE ttl,
  UBYTE * ip_header,
  USHORT identity,
  ULONG dest_addr,
  ULONG src_addr,
  T_desc_list2 * desc_list
);

void build_ip_header (
  UBYTE * ip_header_new,
  USHORT identity,
  UBYTE header_len,
  UBYTE type_of_service,
  UBYTE ttl,
  ULONG src_addr,
  ULONG dest_addr,
  USHORT total_len,
  USHORT fragm_offset,
  UBYTE df_flag,
  UBYTE mf_flag,
  UBYTE prot
);

void truncate_descs (T_desc_list2 * p_desc_list, USHORT new_length);

void free_primitive_data_ind (T_DTI2_DATA_IND ** dti_data_ind);

void free_primitive_data_req (T_DTI2_DATA_REQ ** dti_data_req);

ULONG partial_checksum (UBYTE * block, USHORT len);

/*-------------------------------------------------------------------------*/

#endif /* IP_UDP_H */

/*-------------------------------------------------------------------------*/

