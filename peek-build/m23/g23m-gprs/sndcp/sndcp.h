/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Modul   :  sndcp.h
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
|
|  These files implement Van Jacobson Compression algorithm which is taken from
|  University of California Berkley's Implementation.
|
|
|  "Copyright (c)1989 Regents of the University of California.
|  All rights reserved.Redistribution and use in source and binary forms are
|  permitted, provided that the above  copyright notice and this paragraph are
|  duplicated in all such forms and that any documentation advertising materials,
|  and  other materials related to such distributionand use acknowledge that the
|  software was developed by the University of California, Berkeley.
|  The name of the University may not be  used to endorse or promote products
|  derived from this software without specific  prior written permission. 
|  THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
|  INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND 
|  FITNESS FOR A PARTICULAR PURPOSE"
+-----------------------------------------------------------------------------
|  Purpose :  Definitions for the Protocol Stack Entity
|             SNDCP
+-----------------------------------------------------------------------------
*/

/*---- HISTORY --------------------------------------------------------------*/

#ifndef SNDCP_H
#define SNDCP_H

#ifndef _SNDCP_DTI_2_
#define _SNDCP_DTI_2_
#endif


#ifndef SNDCP_UPM_INCLUDED
#define SNDCP_UPM_INCLUDED
#endif


#include "prim.h"

#ifdef SNDCP_2to1

#ifndef T_desc_list3
#define T_desc_list3 T_LL_desc_list3
#endif
#ifndef T_ll_qos
#define T_ll_qos T_PS_qos_r97
#endif

#endif /* SNDCP_2to1 */

#ifdef TI_PS_FF_V42BIS
  #include "v42b_type.h"
#endif /* TI_PS_FF_V42BIS */

/*
 * Compiler switch for tracing of user data, e.q. TCP/IP header and payload
 * Payload is traced only if config primitive TRACE_PACKET_ON was received.
 */
#ifdef SNDCP_TRACE_ALL
 /*
  * compiler switch for tracing of a buffer as hex-dump
  */
  #define SNDCP_TRACE_BUFFER
#endif /* SNDCP_TRACE_ALL */

/*
 * compiler switch for tracing of decoded TCP/IP datagram
 * Per default only the 40 bytes or only decoded TCPIP header is traced.
 * Payload is traced only if config primitive TRACE_PACKET_ON was received.
 */
#define SNDCP_TRACE_IP_DATAGRAM
#define SNDCP_UL_PACKET              (0x1)
#define SNDCP_DL_PACKET              (0x2)

/*
 * defines the user of the vsi interface
 */
#define VSI_CALLER            SNDCP_handle,


/*
 * Macros
 */

 /*
 * This is just a TEMPORARY define until the issues with OPTION_MULTITHREAD
 * are settled. This define SHOULD be contained in GSM.H.
 */
#undef _ENTITY_PREFIXED
#define _ENTITY_PREFIXED(N) sndcp_##N

/*
 * Customized Trace Macros
 */
#ifndef TRACE_EVENT_P16
  #define TRACE_EVENT_P16(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16)\
       vsi_o_event_ttrace(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16);
#endif /* TRACE_EVENT_P16 */
#ifndef TRACE_EVENT_P14
  #define TRACE_EVENT_P14(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14)\
       vsi_o_event_ttrace(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14);
#endif /* TRACE_EVENT_P14 */
#ifndef TRACE_EVENT_P12
  #define TRACE_EVENT_P12(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12)\
       vsi_o_event_ttrace(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12);
#endif /* TRACE_EVENT_P12 */
#ifndef TRACE_EVENT_P10
  #define TRACE_EVENT_P10(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10)\
       vsi_o_event_ttrace(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10);
#endif /* TRACE_EVENT_P10 */

/* 
 * The following macro is similar to PFREE_DESC2. Instead of doing
 * a PFREE(P) it does a MFREE(p). This macro is used to free primitives
 * like sn_data_req which are removed from SN SAP and added into this 
 * header file.
 */
#define MFREE_PRIM(p) { MFREE_DESC2 ((p)->desc_list2.first);      \
                        MFREE ((p));                              \
                      }

/*
 * Constants
 */

/*
 * Possible values for su->cia_state.
 */
#define CIA_IDLE      23
#define CIA_BUSY      23 /*29*/

/*
 * Maximum number of DTI links.
 */
#if defined (SNDCP_2to1) || defined (REL99)|| \
    defined (_SIMULATION_)
#define SNDCP_MAX_DTI_LINKS        16
#else
#define SNDCP_MAX_DTI_LINKS        3
#endif
/*
 * Maximum number of Van Jacobson header compressors.
 */
#define SNDCP_MAX_VJ_COUNT        1


/*
 * The default value for N201-I according to
 * GSM 4.64, version 6.51, table 9
 * For test purposes a smaller value is used.
 */
#ifdef _SIMULATION_
#define N201_I_DEFAULT    70
#else /* _SIMULATION_ */
#define N201_I_DEFAULT    1503
#endif /* _SIMULATION_ */


/*
 * The default value for N201-U according to
 * GSM 4.64, version 6.51, table 9.
 * For test purposes a smaller value is used.
 */
#ifdef _SIMULATION_
#define N201_U_DEFAULT    70
#else /* _SIMULATION_ */
#define N201_U_DEFAULT    500
#endif /* _SIMULATION_ */

/*
 * N-PDU size
 */
#define SNDCP_N_PDU_SIZE  750

/*
 * The re-establishment time for GSM 4.65, 6.2.1.4.
 */
#ifdef _SIMULATION_
#define MG_RE_EST_TIME    3000
#else /* _SIMULATION_ */
#define MG_RE_EST_TIME    3000
#endif /* _SIMULATION_ */

/*
 * Bitoffset for encoding/decoding
 */

#define ENCODE_OFFSET         (37 * 8)

/*
 * Byteoffset for encoding/decoding
 */

#define ENCODE_OFFSET_BYTE         37

/*
 * return values for procedures mg_decode_xid, mg_check_cnf_xid.
 */
/*
 * Parsed XID is ok (used for format and/or content).
 */
#define MG_XID_OK                   0
/*
 * Parsed XID block has incorrect content.
 */
#define MG_XID_BAD_CONTENT     1
/*
 * Parsed XID block was in bad format.
 */
#define MG_XID_BAD_FORMAT      2

/*
 * Other constants for service mg.
 */
/*
 * Maximal number of renegotiations in case of invalid cnf.
 */
#define MG_MAX_RENEGO  4
/*
 * Maximal number of compression entities: 32 each.
 */
#define MG_MAX_ENTITIES 32
/*
 * Maximum number of PCOMP values: 16.
 */
#define MG_MAX_PCOMP 16
/*
 * Maximum number of DCOMP values: 16.
 */
#define MG_MAX_DCOMP 16
/*
 * Possible values for service var 'mod_expects':
 */
#define MG_MOD_X_NONE   0xff
#define MG_MOD_X_REL    1
#define MG_MOD_X_EST    2
#define MG_MOD_X_READY  0
/*
 * State of compression entity or dcomp/pcomp value
 */
#define MG_UNASSIGNED 23
/*
 * State of compression entity or dcomp/pcomp value
 */
#define MG_SELECTED 37
/*
 * State of compression entity or dcomp/pcomp value
 */
#define MG_ASSIGNED 47
/*
 * Possible field lengths in xid blocks in case of omitted parameters.
 * acc. to GSM 4.65, 6.6.2.1.
 * for p bit == 1
 */
#define MG_DATA_P_0_NSAPIS_OM     0
#define MG_DATA_P_0_P0_OM         2
#define MG_DATA_P_0_P1_OM         3
#define MG_DATA_P_0_P2_OM         5

#define MG_HEADER_P_0_NSAPIS_OM   0
#define MG_HEADER_P_0_S0_M_1_OM   2

/*
 * The same for p bit == 0
 */
#define MG_DATA_P_1_NSAPIS_OM     1
#define MG_DATA_P_1_P0_OM         3
#define MG_DATA_P_1_P1_OM         4
#define MG_DATA_P_1_P2_OM         6

#define MG_HEADER_P_1_NSAPIS_OM   1
#define MG_HEADER_P_1_S0_M_1_OM   3


/*
 * Is the 'nsapis' parameter in xid block for data or header entity?
 */
#define MG_XID_V42_NSAPIS        23
#define MG_XID_VJ_NSAPIS         19



/*
 * number of implemented instances of service nu
 */
#define NU_NUM_INC 16

/*
 * LLC operation modes
 */
#define SNDCP_ACK   17
#define SNDCP_UNACK 37


/*
 * Number of unacknowledged segment numbers (acc to GSM 4.65: 3)
 */
#ifdef _SIMULATION_
#define SNDCP_SEGMENT_NUMBERS_UNACK     25
#else
#define SNDCP_SEGMENT_NUMBERS_UNACK     7
#endif /* _SIMULATION_ */

/*
 * Number of acknowledged segment numbers. This value is implementation
 * dependant. Here a bit mask of 32 is used in service sua to check the
 * confirmation status of a segmented N-PDU which then implies a maximum
 * number of 32.
 */
#define SNDCP_SEGMENT_NUMBERS_ACK     32

/* 
 * Maximum Segment Number in a Single NPDU which SNDCP will assemble 
 * in downlink. If SNDCP receives more than SNDCP_MAX_SEG_NUMBER segments
 * in one NPDU, SNDCP will discard the stored segments and will keep on
 * discarding the incoming segments till it receives the last segment.
 */
#define SNDCP_MAX_SEGMENT_NUMBER      100


/*
 * The resulting max value of the fully confirmed 32 segments mask.
 */
#define SNDCP_MAX_COMPLETE 4294967295

/*
 * The length of the queue for N-PDUs (in form of SN_UNITDATA_REQ prims).
 */
#define SN_UNITDATA_Q_LEN     SNDCP_NUMBER_OF_NSAPIS

/*
 * The length of the queue for N-PDUs (in form of SN_DATA_REQ prims).
 */
#define SN_DATA_Q_LEN     SNDCP_NUMBER_OF_NSAPIS

/*
 * The length of an SN Unitdata Pdu header, acc. to GSM 4.65 in bytes.
 * This SN-PDU is the first segment of an N-PDU. The octet for DCOMP and PCOMP
 * is included in the SN-UNITDATA PDU format.
 */
#define SN_UNITDATA_PDP_HDR_LEN_BIG   4


/*
 * The length of an SN Unitdata Pdu header, acc. to GSM 4.65 in bytes.
 * This SN-PDU is not the first segment of an N-PDU. The octet for DCOMP and
 * PCOMP is not included in SN-UNITDATA PDU format.
 */
#define SN_UNITDATA_PDP_HDR_LEN_SMALL   3

/*
 * The length of an SN Data Pdu header, acc. to GSM 4.65 in bytes.
 * This SN-PDU is the first segment of an N-PDU. The octet for DCOMP and PCOMP
 * is included in the SN-DATA PDU format.
 */
#define SN_DATA_PDP_HDR_LEN_BIG   3


/*
 * The length of an SN Data Pdu header, acc. to GSM 4.65 in bytes.
 * This SN-PDU is not the first segment of an N-PDU. The octet for DCOMP and
 * PCOMP is not included in the SN-DATA PDU PDU format. The octet with the
 * N-PDU number is not included in the SN-DATA PDU format.
 */
#define SN_DATA_PDP_HDR_LEN_SMALL   1



/*
 * Number of implemented instances of service su.
 */
#define SU_NUM_INC 4

/*
 * Number of implemented instances of service sua.
 */
#define SUA_NUM_INC 4

/*
 * number of implemented instances of service nd
 */
#define ND_NUM_INC 16

/*
 * number of implemented instances of service sd
 */
#define SD_NUM_INC 4

/*
 * number of implemented instances of service sda
 */
#define SDA_NUM_INC 4

/*
 * Maximum number of NSAPIs according to specification, implementation
 * independent.
 */
#define SNDCP_NUMBER_OF_NSAPIS 16

/*
 * Maximum number of LLC SAPIs according to specification, implementation
 * independent.
 */
#define SNDCP_NUMBER_OF_SAPIS 4

/*
 * Maximum N-pdu number in unacknowledged LLC operation mode
 */
#define MAX_NPDU_NUMBER_UNACK 4096

/*
 * Maximum N-pdu number in acknowledged LLC operation mode
 */
#define MAX_NPDU_NUMBER_ACK 256

/*
 * The implementation dependant length in bits for an XID block that
 * will be requested by the MS.
 */
#define SNDCP_XID_BLOCK_BIT_LEN 192

/*
 * Constants for filling the requested XID block.
 */
/*
 * The parameter type for version.
 */
#define SNDCP_XID_PARAM_TYPE_0      0
/*
 * Length of parameter type 0 in octets.
 */
#define SNDCP_XID_0_LEN             1
/*
 * Version number 0.
 */
#define SNDCP_XID_VERSION           0
/*
 * The parameter type for data compressors.
 */
#define SNDCP_XID_PARAM_TYPE_1      1
/*
 * Length of parameter type 1 in octets.
 * RES value 2 shorter because algorithm type and dcomp bytes omitted.
 */
#define SNDCP_XID_1_LEN             10
#define SNDCP_XID_1_LEN_RES          8
/*
 * P bit value 1.
 */
#define SNDCP_P_BIT_1         1
/*
 * P bit value 0.
 */
#define SNDCP_P_BIT_0         0
/*
 * Compressor entity 0.
 */
#define SNDCP_NTT_0           0
/*
 * the octet with | X | X | X | algorithm type.
 */
#define SNDCP_XID_V42               0
/*
 * Length of V42bis parameters in octets.
 * RES values 1 shorter because of omitted dcomp value.
 */
#define SNDCP_XID_V42_LEN           7
#define SNDCP_XID_V42_LEN_RES       6
/*
 * The octet with| DCOMP1 | X | will be set dynamically.
 * One default value.
 */
#define SNDCP_DCOMP1                      1
/*
 * The two octets with applicable nsapis will be set dynamically.
 * default value is SNDCP_NSAPIS_DEFAULT like in case of header compression.
 */
/*
 * The direction octet will be set dynamically, values may be
 * SNSM_COMP_NEITHER_DIRECT,
 * SNSM_COMP_MS_SGSN_DIRECT,
 * SNSM_COMP_SGSN_MS_DIRECT,
 * SNSM_COMP_BOTH_DIRECT
 * Default: SNSM_COMP_BOTH_DIRECT
 */
#ifndef SNDCP_UPM_INCLUDED
#define SNDCP_V42_DEFAULT_DIRECTION SNSM_COMP_BOTH_DIRECT
#else 
#define SNDCP_V42_DEFAULT_DIRECTION NAS_DCOMP_BOTH_DIRECT
#endif /* Compression parameters */ /* SNDCP_UPM_INCLUDED */
/*
 * The 2 P1 octets (max codewords) will be set dynamically.
 * Default value: 2048
 */
#define SNDCP_V42_DEFAULT_P1        2048
/*
 * The P2 (max char num) octet will be set dynamically.
 * Default value: 20
 */
#define SNDCP_V42_DEFAULT_P2        20
/*
 * The parameter type for header compressors.
 */
#define SNDCP_XID_PARAM_TYPE_2      2
/*
 * Length of parameter type 2 in octets
 * RES value 2 shorter because algorithm type and pcomp bytes omitted.
 */
#define SNDCP_XID_2_LEN_RES         5
#define SNDCP_XID_2_LEN             7
/*
 * The octet with | X | X | X | algorithm type.
 */
#define SNDCP_XID_VJ                0
/*
 * Length of VanJacobson parameters in octets.
 * RES value 1 shorter because pcomp byte omitted.
 */
#define SNDCP_XID_VJ_LEN            4
#define SNDCP_XID_VJ_LEN_RES        3
#define SNDCP_MAX_NUMBER_OF_VJ_SLOTS    16

/*
 * The octet with| PCOMP1 | PCOMP2 | will be set dynamically.
 * Two default values.
 */
#define SNDCP_PCOMP1                      1
#define SNDCP_PCOMP2                      2
/*
 * The two octets with applicable nsapis will be set dynamically.
 * Default value: 0
 */
#define SNDCP_NSAPIS_DEFAULT              0
/*
 * The octet with S0 - 1 will be set dynamically.
 * Default value: 15
 */
#define SNDCP_VJ_DEFAULT_S0_M_1              15

/*
 * "states" of nsapis and sapis that are stored in the arrays nsapi_state_ra
 * and sapi_state_ra
 */
#define MG_IDLE           0
/*
 * Waiting for LL_ESTABLISH_CNF.
 */
#define MG_EST            1
/*
 * Waiting for LL_XID_CNF.
 */
#define MG_XID            2
/*
 * Must send SNSM_ACTIVATE_RES.
 */
#define MG_ACT            4
/*
 * Waiting for LL_RELEASE_CNF.
 */
#define MG_REL            8
/*
 * Must send SNSM_DEACTIVATE_RES.
 */
#define MG_DEACT          0x10
/*
 * Waiting for SNSM_SEQUEMCE_IND.
 */
#define MG_SEQ            0x20
 /*
  * A local LL_RELEASE_REQ must be sent after the end of the running sub
  * procedure.
  */
#define MG_REL_NEC_LOC    0x40
 /*
  * An LL_XID_REQ must be sent after the end of the running sub procedure.
  */
#define MG_XID_NEC        0x80
 /*
  * An LL_ESTABLISH_REQ must be sent after the end of the running sub procedure.
  */
#define MG_EST_NEC        0x100
 /*
  * A non local LL_RELEASE_REQ must be sent after the end of the running sub
  * procedure.
  */
#define MG_REL_NEC_PEER   0x200
 /*
  * A LL_ESTABLISH_IND is expected or received
  */
#define MG_EST_IND        0x400

 /*
  * SNDCP got SN_DTI_REQ and the DTI information is stored !
  */
#define MG_DTI_INFO       0x800 


/*
 * Service definitions. Used to access service data with GET/SET_STATE.
 *
 * Services with multiple incarnation have to be defined as xxx->
 * Services with only one incarnation have to be defined as xxx.
 */
#define CIA                 cia.
#define MG                  mg.
#define NU                  nu->
#define SU                  su->
#define SUA                 sua->
#define ND                  nd->
#define PD                  pd->
#define PDA                 pda->
#define SD                  sd->
#define SDA                 sda->

/*
 * workaround for init
 */
#define NU_0                  nu->
#define NU_1                  nu->
#define NU_2                  nu->
#define NU_3                  nu->
#define NU_4                  nu->
#define NU_5                  nu->
#define NU_6                  nu->
#define NU_7                  nu->
#define NU_8                  nu->
#define NU_9                  nu->
#define NU_10                 nu->
#define NU_11                 nu->
#define NU_12                 nu->
#define NU_13                 nu->
#define NU_14                 nu->
#define NU_15                 nu->

#define SU_0                  su->
#define SU_1                  su->
#define SU_2                  su->
#define SU_3                  su->

#define SUA_0                 sua->
#define SUA_1                 sua->
#define SUA_2                 sua->
#define SUA_3                 sua->

#define ND_0                  nd->
#define ND_1                  nd->
#define ND_2                  nd->
#define ND_3                  nd->
#define ND_4                  nd->
#define ND_5                  nd->
#define ND_6                  nd->
#define ND_7                  nd->
#define ND_8                  nd->
#define ND_9                  nd->
#define ND_10                 nd->
#define ND_11                 nd->
#define ND_12                 nd->
#define ND_13                 nd->
#define ND_14                 nd->
#define ND_15                 nd->

#define SD_0                  sd->
#define SD_1                  sd->
#define SD_2                  sd->
#define SD_3                  sd->

#define SDA_0                 sda->
#define SDA_1                 sda->
#define SDA_2                 sda->
#define SDA_3                 sda->

#define PD_0                  pd->
#define PD_1                  pd->
#define PD_2                  pd->
#define PD_3                  pd->

#define PDA_0                 pda->
#define PDA_1                 pda->
#define PDA_2                 pda->
#define PDA_3                 pda->

#ifdef NTRACE
#undef SNDCP_TRACE_ALL    /* if NTRACE is defined, SNDCP_TRACE_ALL should be disabled */
#endif
/*
 * Service name definitions for trace purposes.
 */
#ifndef NTRACE

#define SERVICE_NAME_CIA     "CIA"

#define SERVICE_NAME_MG     "MG"

#define SERVICE_NAME_ND_0      "ND_0"
#define SERVICE_NAME_ND_1      "ND_1"
#define SERVICE_NAME_ND_2      "ND_2"
#define SERVICE_NAME_ND_3      "ND_3"
#define SERVICE_NAME_ND_4      "ND_4"
#define SERVICE_NAME_ND_5      "ND_5"
#define SERVICE_NAME_ND_6      "ND_6"
#define SERVICE_NAME_ND_7      "ND_7"
#define SERVICE_NAME_ND_8      "ND_8"
#define SERVICE_NAME_ND_9      "ND_9"
#define SERVICE_NAME_ND_10     "ND_10"
#define SERVICE_NAME_ND_11     "ND_11"
#define SERVICE_NAME_ND_12     "ND_12"
#define SERVICE_NAME_ND_13     "ND_13"
#define SERVICE_NAME_ND_14     "ND_14"
#define SERVICE_NAME_ND_15     "ND_15"

#define SERVICE_NAME_NU_0      "NU_0"
#define SERVICE_NAME_NU_1      "NU_1"
#define SERVICE_NAME_NU_2      "NU_2"
#define SERVICE_NAME_NU_3      "NU_3"
#define SERVICE_NAME_NU_4      "NU_4"
#define SERVICE_NAME_NU_5      "NU_5"
#define SERVICE_NAME_NU_6      "NU_6"
#define SERVICE_NAME_NU_7      "NU_7"
#define SERVICE_NAME_NU_8      "NU_8"
#define SERVICE_NAME_NU_9      "NU_9"
#define SERVICE_NAME_NU_10     "NU_10"
#define SERVICE_NAME_NU_11     "NU_11"
#define SERVICE_NAME_NU_12     "NU_12"
#define SERVICE_NAME_NU_13     "NU_13"
#define SERVICE_NAME_NU_14     "NU_14"
#define SERVICE_NAME_NU_15     "NU_15"

#define SERVICE_NAME_SU_0      "SU_0"
#define SERVICE_NAME_SU_1      "SU_1"
#define SERVICE_NAME_SU_2      "SU_2"
#define SERVICE_NAME_SU_3      "SU_3"

#define SERVICE_NAME_SUA_0      "SUA_0"
#define SERVICE_NAME_SUA_1      "SUA_1"
#define SERVICE_NAME_SUA_2      "SUA_2"
#define SERVICE_NAME_SUA_3      "SUA_3"

#define SERVICE_NAME_SD_0      "SD_0"
#define SERVICE_NAME_SD_1      "SD_1"
#define SERVICE_NAME_SD_2      "SD_2"
#define SERVICE_NAME_SD_3      "SD_3"

#define SERVICE_NAME_SDA_0      "SDA_0"
#define SERVICE_NAME_SDA_1      "SDA_1"
#define SERVICE_NAME_SDA_2      "SDA_2"
#define SERVICE_NAME_SDA_3      "SDA_3"

#define SERVICE_NAME_PD_0      "PD_0"
#define SERVICE_NAME_PD_1      "PD_1"
#define SERVICE_NAME_PD_2      "PD_2"
#define SERVICE_NAME_PD_3      "PD_3"

#define SERVICE_NAME_PDA_0     "PDA_0"
#define SERVICE_NAME_PDA_1     "PDA_1"
#define SERVICE_NAME_PDA_2     "PDA_2"
#define SERVICE_NAME_PDA_3     "PDA_3"

#endif /* !NTRACE */


/*
 * 1 re-establishment timer for each sapi.
 */
#define TIMER_MAX     4


/*
 * State definitions for each service.
 */
#define CIA_DEFAULT                           23
                                              
#define MG_DEFAULT                             7
                                              
#define NU_UNACK_SU_RECEPTIVE                  8
#define NU_UNACK_SU_NOT_RECEPTIVE              9
                                              
#define NU_ACK_SUA_RECEPTIVE                  10
#define NU_ACK_SUA_NOT_RECEPTIVE              11
#define NU_SUS_SUA_RECEPTIVE                  12
#define NU_SUS_SUA_NOT_RECEPTIVE              13
#define NU_REC_SUA_NOT_RECEPTIVE              14

#ifndef NCONFIG

#define NU_SEND_UNACK_SU_RECEPTIVE            60
#define NU_SEND_UNACK_SU_NOT_RECEPTIVE        61

#define NU_SEND_ACK_SUA_RECEPTIVE             62
#define NU_SEND_ACK_SUA_NOT_RECEPTIVE         63
#define NU_SEND_SUS_SUA_RECEPTIVE             64
#define NU_SEND_SUS_SUA_NOT_RECEPTIVE         65
#define NU_SEND_REC_SUA_NOT_RECEPTIVE         66

#endif


#define SU_LLC_NOT_RECEPTIVE                  22
#define SU_LLC_RECEPTIVE                      23
#define SU_LLC_RECEPTIVE_SUSPEND              24
#define SU_LLC_NOT_RECEPTIVE_SUSPEND          26

#define SUA_LLC_NOT_RECEPTIVE                 82
#define SUA_LLC_RECEPTIVE                     83
#define SUA_LLC_RECEPTIVE_SUSPEND             84
#define SUA_LLC_NOT_RECEPTIVE_SUSPEND         86

#define ND_DEFAULT                            31
#define ND_RECOVER                            37
#define ND_SUSPEND                            41

#define SD_DEFAULT                            44
#define SD_UNACK_DISCARD                      45
#define SD_UNACK_RECEIVE_FIRST_SEGMENT        46
#define SD_UNACK_RECEIVE_SUBSEQUENT_SEGMENT   47
#define SD_UNACK_WAIT_NSAPI                   48

#define PD_DEFAULT                            50

#define PDA_DEFAULT                           70

#define SDA_DEFAULT                           64
#define SDA_ESTABLISH_REQUESTED               65
#define SDA_RECEIVE_FIRST_SEGMENT             66
#define SDA_RECEIVE_SUBSEQUENT_SEGMENT        67
#define SDA_WAIT_NSAPI                        68
#define SDA_ACK_DISCARD                       69



/*==== TYPES ======================================================*/
/*
 * Value constants for VAL_seg_pos
 */
#define SEG_POS_NONE                   (0x0)      /* In between */
#define SEG_POS_FIRST                  (0x1)      /* first      */
#define SEG_POS_LAST                   (0x2)      /* last       */
#define SEG_POS_FIRST_LAST             (0x3)  /* both       */
/*
 * Value constants for algo_type
 */
#define CIA_ALGO_V42                   (0x0)  /* V42bis algorithm */
/*
 * Value constants for comp_inst
 */
#define CIA_COMP_INST_V42_0            (0x0)  /* first instance for V42bis */

/*
 * The following definitions was originally generated by CCD. 
 * The definitions are not used as 
 * primitive definitions anymore, only for SNDCP internal usage.
 */

typedef struct
{
  U8                        delay;        /*<1> delay class       */
  U8                        relclass;     /*<1> Reliability class */
  U8                        peak;         /*<1> peak throughput   */
  U8                        preced;       /*<1> Precedence class  */
  U8                        mean;         /*<1> main throughput   */
  U8                        _align0;      /*<1> alignment         */
  U8                        _align1;      /*<1> alignment         */
  U8                        _align2;      /*<1> alignment         */
} T_cia_qos;

#ifndef __T_pdu_ref__
#define __T_pdu_ref__
typedef struct
{
  U16                       ref_npdu_num; /*<2> ref_npdu_num      */
  U8                        ref_seg_num;  /*<1> ref_seg_num       */
  U8                        ref_nsapi;    /*<1> ref_nsapi         */
} T_pdu_ref;
#endif

typedef struct
{
  U8               sapi;         /*<1> service access point identifier  */
  U8               algo_type;    /*<1> algorithm type                   */
  U8               comp_inst;    /*<1> compressor instance              */
  U8               packet_type;  /*<1> Type of packet.                  */
  T_pdu_ref        pdu_ref;      /*<8> to the segment to be confirmed   */
  T_desc_list2     desc_list2;   /*<8> list of generic data descriptors */
} T_CIA_COMP_REQ;


typedef struct
{
  U8                sapi;         /*<1> service access point identifier     */
  U8                algo_type;    /*<1> algorithm type                      */
  U8                comp_inst;    /*<1> compressor instance                 */
  U8                seg_pos;      /*<1> First and/or last segment of N-PDU? */
  U8                packet_type;  /*<1> Type of packet.                     */
  U8                _align0;      /*<1> alignment                           */
  U8                _align1;      /*<1> alignment                           */
  U8                _align2;      /*<1> alignment                           */
  U32               tlli;         /*<4> temporary logical link identifier   */
  T_cia_qos         cia_qos;      /*<8> quality of service                  */
  T_pdu_ref         pdu_ref;      /*<8> to the segment to be confirmed      */
  T_desc_list3      desc_list3;   /*<8> List of generic data descriptors    */
} T_CIA_COMP_IND;


typedef struct
{
  U8                 sapi;         /*<1> service access point identifier     */
  U8                 _align0;      /*<1> alignment                           */
  U8                 _align1;      /*<1> alignment                           */
  U8                 _align2;      /*<1> alignment                           */
  U32                tlli;         /*<4> temporary logical link identifier   */
  T_pdu_ref          pdu_ref;      /*<8> to the segment to be confirmed      */
  U8                 algo_type;    /*<1> algorithm type                      */
  U8                 comp_inst;    /*<1> compressor instance                 */
  U8                 seg_pos;      /*<1> First and/or last segment of N-PDU? */
  U8                 pcomp;        /*<1> from sn pdu                         */
  T_sdu              sdu;          /*< > service data unit                   */
} T_CIA_DECOMP_REQ;

typedef struct
{
  U8                 algo_type;    /*<1> algorithm type                   */
  U8                 comp_inst;    /*<1> compressor instance              */
  U8                 pcomp;        /*<1> from sn pdu                      */
  U8                 _align0;      /*<1> alignment                        */
  T_pdu_ref          pdu_ref;      /*<8> to the segment to be confirmed   */
  T_desc_list2       desc_list2;   /*< > list of generic data descriptors */
} T_CIA_DECOMP_IND;



/*
The following definitions was originally generated by CCD. The definitions are not used as 
primitive definitions anymore, only for SNDCP internal usage.
*/
typedef struct
{
  U8              nsapi;      /*<1> network layer sapi                */
  U8              p_id;       /*<1> protocol identifier               */
  U8              _align0;    /*<1> alignment                         */
  U8              _align1;    /*<1> alignment                         */
  T_desc_list2    desc_list2; /*<8> list of generic data descriptors  */
} T_SN_DATA_REQ;

typedef struct
{
  U8               nsapi;      /*<1> network layer sapi               */
  U8               p_id;       /*<1> protocol identifier              */
  U8               _align0;    /*<1> alignment                        */
  U8               _align1;    /*<1> alignment                        */
  T_desc_list2     desc_list2; /*<8> list of generic data descriptors */
} T_SN_UNITDATA_REQ;

typedef struct
{
  U8                nsapi;       /*<1> network layer sapi               */
  U8                p_id;        /*<1> protocol identifier              */
  U8                _align0;     /*<1> alignment                        */
  U8                _align1;     /*<1> alignment                        */
  T_desc_list2      desc_list2;  /*<8> list of generic data descriptors */
} T_SN_UNITDATA_IND;

typedef struct
{
  U8                 nsapi;       /*<1> network layer sapi               */
  U8                 p_id;        /*<1> protocol identifier              */
  U8                 _align0;     /*<1> alignment                        */
  U8                 _align1;     /*<1> alignment                        */
  T_desc_list2       desc_list2;  /*<8> list of generic data descriptors */
} T_SN_DATA_IND;

typedef struct
{
  U8                  nsapi;    /*<1> network layer sapi   */
  U8                  p_id;     /*<1> protocol identifier  */
  U8                  _align0;  /*<1> alignment            */
  U8                  _align1;  /*<1> alignment            */
  T_sdu               sdu;      /*< > service data unit    */
} T_SN_DATA_IND_TEST;

typedef struct
{
  U8                  nsapi;     /*<1> network layer sapi   */
  U8                  p_id;      /*<1> protocol identifier  */
  U8                  _align0;   /*<1> alignment            */
  U8                  _align1;   /*<1> alignment            */
  T_sdu               sdu;       /*< > service data unit    */
} T_SN_UNITDATA_IND_TEST;

/*
 * SNDCP global typedefs
 */

#ifdef SNDCP_UPM_INCLUDED 
typedef T_PS_qos_r97 T_snsm_qos; 
typedef T_SN_ACTIVATE_REQ T_SNSM_ACTIVATE_IND;
#endif  /*SNDCP_UPM_INCLUDED*/




/*
 * Structs used to store XID parameters.
 */
/*
 * Parameters for V42.bis data compression.
 */
typedef struct /* T_XID_V42_BLOCK */
{
  /*
   * Has V42 field been set in the block?.
   */
  BOOL is_set;
  /*
   * P bit.
   */
  UBYTE p_bit;
  /*
   * Entity number.
   */
  UBYTE ntt;
  /*
   * Algorithm type.
   */
  UBYTE algo_type;
  /*
   * The one DCOMP value needed for V42.bis.
   */
  UBYTE dcomp;
  /*
   * Applicable nsapis set?
   */
  BOOL nsapis_set;
  /*
   * Applicable nsapis (bit mask);
   */
  USHORT nsapis;
  /*
   * Direction set?
   */
  BOOL p0_set;
  /*
   * Direction
   */
  UBYTE p0;
  /*
   * Max number of codewords set?
   */
  BOOL p1_set;
  /*
   * Max number of codewords.
   */
  USHORT p1;
  /*
   * Max char number set?
   */
  BOOL p2_set;
  /*
   * Max char number.
   */
  UBYTE p2;

} T_XID_V42_BLOCK;

/*
 *  Used by VanJacobsen to store TCP-Headers
 */
typedef struct sndcp_tcp_header/* T_SNDCP_TCP_HEADER */
{
  /* TCP */
  /*
   *  Source Port
   */
  USHORT th_sport;

  /*
   *  Destination Port
   */
  USHORT th_dport;

  /*
   *  Sequence Number
   */
  ULONG th_seq;

  /*
   *  Acknowledgement Number
   */
  ULONG th_ack;

  /*
   *  Data Offset
   */
  UBYTE th_off;
 /*
  *  Flags: urg, ack, psh, rst, syn, fin
  */
  UBYTE th_flags;

  /*
   *  Window
   */
  USHORT th_win;

  /*
   *  Checksum
   */
  USHORT th_sum;

  /*
   *  Urgent Pointer
   */
  USHORT th_urp;

} T_SNDCP_TCP_HEADER;

/*
 *  Used by VanJacobsen to store IP-Headers
 */
typedef struct sndcp_ip_header/* T_SNDCP_IP_HEADER */
{
  /* IP */

  /*
   *  Protocol Verion, Header Length
   */
  UBYTE ip_vhl;

  /*
   *  Type of Service
   */
  UBYTE ip_tos;

  /*
   *  Total Length
   */
  USHORT ip_len;

  /*
   *  Packet ID
   */
  USHORT ip_id;

  /*
   *  Don't fragment, More fragments, Fragment Offset
   */
  USHORT ip_off;

  /*
   *  Time to live
   */
  UBYTE ip_ttl;

  /*
   *  Protocol
   */
  UBYTE ip_p;

  /*
   *  Header Checksum
   */
  USHORT ip_sum;

  /*
   *  Source Address
   */
  ULONG ip_src;

  /*
   *  Destination Address
   */
  ULONG ip_dst;

} T_SNDCP_IP_HEADER;


/*
 * Parameters for VanJacobson header compression.
 */
typedef struct /* T_XID_VJ_BLOCK */
{
  /*
   * Has VJ field been set in the block?.
   */
  BOOL is_set;
  /*
   * P bit.
   */
  UBYTE p_bit;
  /*
   * Entity number.
   */
  UBYTE ntt;
  /*
   * Algorithm type.
   */
  UBYTE algo_type;
  /*
   * The first PCOMP value needed for Van Jacobson.
   */
  UBYTE pcomp1;
  /*
   * The second PCOMP value needed for Van Jacobson.
   */
  UBYTE pcomp2;
  /*
   * Applicable nsapis set?
   */
  BOOL nsapis_set;
  /*
   * Applicable nsapis (bit mask);
   */
  USHORT nsapis;
  /*
   * Direction. (Note: only for internal purposes, 
   * not included in XID block)
   */
  UBYTE direction;
  /*
   * Number of state slots minus  set?
   */
  UBYTE s0_m_1_set;
  /*
   * Number of state slots minus 1.
   */
  UBYTE s0_m_1;

} T_XID_VJ_BLOCK;

typedef struct /* T_XID_BLOCK */
{
  BOOL version_set;
  UBYTE version;
  T_XID_V42_BLOCK v42;
  T_XID_VJ_BLOCK vj;
} T_XID_BLOCK;


/*
 * This is used to buffer N-PDUs in service nu.
 *
 * Instances of this struct are added to a dynamic list:
 */
typedef struct t /* T_NPDU_BUFFER */
{
  struct t* next;
  T_SN_DATA_REQ* sn_data_req;
  UBYTE npdu_number;
} T_NPDU_BUFFER;

/*
 * This is used to store information about segmented N-PDUs in service sua:
 * One T_SEG_INFO entry is for 1 segmented N-PDU.
 * a. How many segments have been sent from 1 N-PDU?
 * b. Which of these segments have been confirmed by the peer?
 * c. What is the "reference" struct?
 * d. Who is the affected NSAPI?
 *
 * Instances of this struct are added to a dynamic list:
 */
typedef struct tt /* T_SEG_INFO */
{
  struct tt* next;
  UBYTE number_of_segments;
  /*
   * An array of bit flags for 32 possible segments per N-PDU;
   * This restricts number of segments to 32.
   */
  ULONG cnf_segments;
  UBYTE npdu_number;
  UBYTE nsapi;
} T_SEG_INFO;

/*
 * Constants and bit-masks used by header compression algorithm
 */
#define TH_FIN                      0x01    /* FIN-Bit Mask */
#define TH_SYN                      0x02    /* SYN-Bit Mask */
#define TH_RST                      0x04    /* RST-Bit Mask */
#define TH_PUSH                     0x08    /* PSH-Bit Mask */
#define TH_ACK                      0x10    /* ACK-Bit Mask */
#define TH_URG                      0x20    /* URG-Bit Mask */
#define TH_FLAGS                    (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG)
#define TCP_PUSH_BIT                0x10    /* PUSH-Bit Mask */

/* Bits in first octet of compressed packet, for what changed in a packet */
#define NEW_C                       0x40
#define NEW_I                       0x20
#define NEW_S                       0x08
#define NEW_A                       0x04
#define NEW_W                       0x02
#define NEW_U                       0x01

/* reserved, special-case values */
/* echoed interactive traffic */
#define SPECIAL_I                   (NEW_S|NEW_W|NEW_U)
/* unidirectional data */
#define SPECIAL_D                   (NEW_S|NEW_A|NEW_W|NEW_U)
#define SPECIALS_MASK               (NEW_S|NEW_A|NEW_W|NEW_U)

/* maximal change of values for sending compressed headers */
#define MAX_CHANGE                  0xFFFE
/* TCP-IP Protocol ID */
#define PROT_TCPIP                  6
/* Position number of protocol type within a UBYTE buffer */
#define PR_TYPE_POS                 9
/* Header Length Mask */
#define HL_MASK                     0x0F
/* tossing rcvd frames because of input err */
#define SLF_TOSS                    1
/* Max. TCP/IP header length should really be 128 */
#define MAX_HDR_LEN                 128
#define TMP_HDR_LEN                 168
#define MAX_STATES                  SNDCP_MAX_NUMBER_OF_VJ_SLOTS

/* packet types as defined in RFC 1144 */
#define TYPE_IP                     0x40
#define TYPE_UNCOMPRESSED_TCP       0x70
#define TYPE_COMPRESSED_TCP         0x80
#define TYPE_ERROR                  0x00

/*
 * "state" data for each active tcp conversation on the wire.  This is
 * basically a copy of the entire IP/TCP header from the last packet
 * we saw from the conversation together with a small identifier
 * the transmit & receive ends of the line use to locate saved header.
 */
struct cstate {
  struct cstate   *cs_next;    /* next most recently used cstate (xmit only) */
  USHORT          cs_hlen;     /* size of hdr (receive only) */
  UBYTE           cs_id;       /* connection # associated with this state */
  UBYTE           cs_filler;
  UBYTE           cs_hdr[MAX_HDR_LEN]; /* ip/tcp hdr from most recent packet */
  T_SNDCP_IP_HEADER *cs_ip;    /* ptr to ip/tcp hdr from most recent packet */
};

/*
 * all the state data for one serial line (we need one of these per line).
 */
struct slcompress {
  /* most recently used tstate */
  struct cstate   *last_cs;
  /* last rcvd conn. id */
  UBYTE           last_recv;
  /* last sent conn. id */
  UBYTE           last_xmit;
  USHORT          flags;
  /* xmit connection states */
  struct cstate   tstate[MAX_STATES];
  /* receive connection states */
  struct cstate   rstate[MAX_STATES];
  /* buffer to store temporary received ip/tcp header */
  UBYTE           tcpip_hdr[TMP_HDR_LEN];
  USHORT          slots_to_use;
};

/*
 * A struct comp_buf is used in the call cia_comp_vj and cia_decomp_vj because
 * that routine needs to modify both the start address and length if the
 * incoming packet is compressed/decompressed
 */
struct comp_buf {
    /* length of complete packet */
    USHORT        pack_len;
    /* length of header + some data passed to comp/decomp routine */
    USHORT        hdr_len;
    /* pointer to start of packet */
    ULONG         c_hdr;
    /* packet type */
    UBYTE         p_type;
    UBYTE         filler;
    USHORT        filler1;
};



typedef struct /* T_CIA_DATA */
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
   * Copy of mg.cur_xid_block.
   */
  T_XID_BLOCK cur_xid_block;

  /*
   * all the state data for one serial line
   */
  struct slcompress comp;

#ifdef SNDCP_TRACE_ALL
  SHORT cia_decomp_ind_number[SNDCP_NUMBER_OF_NSAPIS];
#endif
#ifdef TI_PS_FF_V42BIS
  T_V42B_DICO* enc;
  T_V42B_DICO* dec;
  U16 trabu[1500];
#endif /* TI_PS_FF_V42BIS */
  /*
   * Currently assembled unacknowledged N-PDU downlink.
   */
  T_CIA_DECOMP_IND* cur_cia_decomp_ind[SNDCP_NUMBER_OF_NSAPIS];

} T_CIA_DATA;


typedef struct /* T_MG_DATA */
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
   * XID parameters that show what the user requested.
   */
  T_XID_BLOCK user_xid_block [SNDCP_NUMBER_OF_NSAPIS];
  /*
   * XID parameters that are sent with LL_XID_REQ or LL_ESTABLISH_REQ.
   */
  T_XID_BLOCK req_xid_block [SNDCP_NUMBER_OF_SAPIS];
  /*
   * Confirmed XID parameters.
   */
  T_XID_BLOCK cnf_xid_block [SNDCP_NUMBER_OF_SAPIS];
  /*
   * Indicated XID parameters.
   */
  T_XID_BLOCK ind_xid_block [SNDCP_NUMBER_OF_SAPIS];
  /*
   * Responded XID parameters.
   */
  T_XID_BLOCK res_xid_block [SNDCP_NUMBER_OF_SAPIS];
  /*
   * Currently used XID parameters.
   */
  T_XID_BLOCK cur_xid_block [SNDCP_NUMBER_OF_SAPIS];
  /*
   * XID parameters that have not yet been requested, but are now needed,
   * e.g. when an establishment is pending and another SNSM_ACTIVATE_IND
   * comes in with compression required.
   */
  T_XID_BLOCK new_xid_block [SNDCP_NUMBER_OF_SAPIS];
  /*
   * index: sapi + data compression entity number, elements: state;
   * indicates the state of the given entity, one of MG_UNASSIGNED,
   * MG_SELECTED, MG_ASSIGNED UBYTE .
   */
  UBYTE sapi_dntt_state_ra [SNDCP_NUMBER_OF_SAPIS][MG_MAX_ENTITIES];
  /* index: sapi + header compression entity number, elements: state;
   * indicates the state of the given entity, one of MG_UNASSIGNED,
   * MG_SELECTED, MG_ASSIGNED UBYTE .
   */
  UBYTE sapi_pntt_state_ra [SNDCP_NUMBER_OF_SAPIS][MG_MAX_ENTITIES];
  /*
   * index: sapi + data compression entity number + nsapi,
   * elements: is nsapi conneted to given entity?;
   * indicates whether the NSAPI uses the given entity on the given SAPI.
   */
  BOOL sapi_dntt_nsapi_set_ra [SNDCP_NUMBER_OF_SAPIS]
                              [MG_MAX_ENTITIES]
                              [SNDCP_NUMBER_OF_NSAPIS];
  /*
   * index: sapi + header compression entity number + nsapi,
   * elements: is nsapi conneted to given entity?;
   * indictes whether the NSAPI uses the given entity on the given SAPI.
   */
  BOOL sapi_pntt_nsapi_set_ra [SNDCP_NUMBER_OF_SAPIS]
                              [MG_MAX_ENTITIES]
                              [SNDCP_NUMBER_OF_NSAPIS];
  /*
   * index: sapi + dcomp value, elements: state;
   * indicates the state of the given dcomp value, one of MG_UNASSIGNED,
   * MG_SELECTED, MG_ASSIGNED UBYTE .
   */
  UBYTE sapi_dcomp_state_ra [SNDCP_NUMBER_OF_SAPIS][MG_MAX_DCOMP];
  /*
   * index: sapi + pcomp value, elements: state;
   * indicates the state of the given pcomp value, one of MG_UNASSIGNED,
   * MG_SELECTED, MG_ASSIGNED UBYTE .
   */
  UBYTE sapi_pcomp_state_ra [SNDCP_NUMBER_OF_SAPIS][MG_MAX_PCOMP];
  /*
   * index: sapi + dcomp value, elements: data compression entity number;
   * indicates the entity to which the given dcomp value in the given SAPi 
   * is assigned.
   */
  UBYTE sapi_dcomp_dntt_ra [SNDCP_NUMBER_OF_SAPIS][MG_MAX_DCOMP];
  /*
   * index: sapi + pcomp value, elements: header compression entity number;
   * indicates the entity to which the given pcomp value in the given SAPi
   * is assigned.
   */
  UBYTE sapi_pcomp_pntt_ra [SNDCP_NUMBER_OF_SAPIS][MG_MAX_PCOMP];
  /*
   * index: sapi, elements: number of renegotiations;
   */
  UBYTE renego [SNDCP_NUMBER_OF_SAPIS];
  /*
   * index: sapi + dntt value, elements: will the ntt be rejected?
   * For entities that are proposed by the network but cannot be set up.
   */
  BOOL sapi_dntt_rej_ra [SNDCP_NUMBER_OF_SAPIS][MG_MAX_ENTITIES];
  /*
   * index: sapi + dntt value, elements: will the ntt be rejected?
   * For entities that are proposed by the network but cannot be set up.
   */
  BOOL sapi_pntt_rej_ra [SNDCP_NUMBER_OF_SAPIS][MG_MAX_ENTITIES];
  /*
   * What is expected in the course of a context modification?
   * Possible values: MG_MOD_X_NONE, MG_MOD_X_REL, MG_MOD_X_EST, MOD_X_READY.
   */
  UBYTE mod_expects;
  /*
   * Bit_mask for all nsapis waiting with snsm_modify_res for est
   * and/or rel.
   */
  USHORT waiting_nsapis;
  /*
   * Bit_mask for all nsapis currently suspended.
   */
  USHORT suspended_nsapis;

} T_MG_DATA;


typedef struct /* T_NU_DATA */
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
   * the affected nsapi
   */
  UBYTE nsapi;
  /*
   * The Send N-PDU number for unacknowledged data transfer.
   */
  USHORT send_npdu_number_unack;
  /*
   * The Send N-PDU number for acknowledged data transfer.
   */
  UBYTE send_npdu_number_ack;
  /*
   * The Receive N-PDU number for unacknowledged data transfer.
   */
  UBYTE rec_npdu_number_ack;
  /*
   * Here the sent N-PDUs are stored.
   */
  T_NPDU_BUFFER* first_buffered_npdu;
  /*
   * This is the next npdu buffer to be resent.
   */
  T_NPDU_BUFFER* next_resend;
  /*
   * Is an SN_READY_IND pending?
   */
  BOOL sn_ready_ind_pending;
  /*
   * Is an SN_UNITREADY_IND pending?
   */
  BOOL sn_unitready_ind_pending;
  /*
   * Is an SN_SWITCH_CNF expected?
   */
  BOOL sn_switch_cnf_expected;
#ifdef SNDCP_UPM_INCLUDED
  /*
   * Is an SN_DTI_CNF expected?
   */
  BOOL sn_dti_cnf_expected;
#endif /* SNDCP_UPM_INCLUDED */
  /*
   * Is DTI connection opened?
   */
  BOOL connection_is_opened;
  /*
   * This flag indicates if config prim "DISCARD" or "RESUME"
   * for certain NSAPI was received. TRUE means "DISCARD" was received.
   * FALSE is the default state and will be set if "RESUME" was received.
   */
  BOOL discard;

  /*
   * This variables are needed for statistics showing how much data was
   * discarded and how much sent.
   */
  ULONG sent_data;
  ULONG discarded_data;

} T_NU_DATA;


typedef struct /* T_SU_DATA */
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
   * The affected sapi.
   */
  UBYTE sapi;
  /*
   * The max segment size for unacknowledged data transfer.
   */
  USHORT n201_u;
  /*
   * An array with one T_SN_UNITDATA_REQ* per NSAPI. It is ensured by the flow
   * control with service nu that only one sn_unitdata_req per nsapi may be
   * present. This array works like a queue with a read and a write pointer.
   */
  T_SN_UNITDATA_REQ* sn_unitdata_q [SN_UNITDATA_Q_LEN];
  /*
   * An array with the N-PDU numbers for the above sn_unitdata_req primitives.
   */
  USHORT npdu_number_q [SN_UNITDATA_Q_LEN];
  /*
   * The read pointer for sn_unitdata_q.
   */
  UBYTE sn_unitdata_q_read;
  /*
   * The write pointer for sn_unitdata_q.
   */
  UBYTE sn_unitdata_q_write;
#ifdef _SNDCP_DTI_2_
  /*
   * An Array with a number of T_LL_UNITDESC_REQ* to store all segments of
   * 1 N-PDU after they have been received from cia.
   */
  T_LL_UNITDESC_REQ* ll_unitdesc_q [SNDCP_SEGMENT_NUMBERS_UNACK];
  /*
   * The read pointer for ll_unitdesc_q.
   */
  U8 ll_unitdesc_q_read;
  /*
   * The write pointer for ll_unitdesc_q.
   */
  U8 ll_unitdesc_q_write;
#else /* _SNDCP_DTI_2_ */
  /*
   * An Array with a number of T_LL_UNITDATA_REQ* to store all segments of
   * 1 N-PDU after they have been received from cia.
   */
  T_LL_UNITDATA_REQ* ll_unitdata_q [SNDCP_SEGMENT_NUMBERS_UNACK];
  /*
   * The read pointer for ll_unitdata_q.
   */
  UBYTE ll_unitdata_q_read;
  /*
   * The write pointer for ll_unitdata_q.
   */
  UBYTE ll_unitdata_q_write;
#endif /* _SNDCP_DTI_2_ */

  /*
   * In what state is cia?
   * Set to CIA_IDLE at init time and when last segment of N-PDU has been sent
   * to LLC.
   * Set to CIA_BUSY whenever sig_su_cia_cia_comp_req is sent.
   */
  UBYTE cia_state;
} T_SU_DATA;

typedef struct /* T_SUA_DATA */
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
   * The affected sapi.
   */
  UBYTE sapi;
  /*
   * The max segment size for acknowledged data transfer.
   */
  USHORT n201_i;
  /*
   * An array with one T_SN_DATA_REQ* per NSAPI. It is ensured by the flow
   * control with service nu that only one sn_data_req per nsapi may be
   * present. This array works like a queue with a read and a write pointer.
   */
  T_SN_DATA_REQ* sn_data_q [SN_DATA_Q_LEN];
  /*
   * An array with the N-PDU numbers for the above sn_data_req primitives.
   */
  UBYTE npdu_number_q [SN_DATA_Q_LEN];
  /*
   * The read pointer for sn_data_q.
   */
  UBYTE sn_data_q_read;
  /*
   * The write pointer for sn_data_q.
   */
  UBYTE sn_data_q_write;
  /*
   * Information about the outgoing segments, first element in dynamic list.
   */
  T_SEG_INFO* first_seg_info;
#ifdef _SNDCP_DTI_2_
  /*
   * An Array with a number of T_LL_DESC_REQ* to store all
   * segments of 1 N-PDU after they have been received from cia.
   */
  T_LL_DESC_REQ* ll_desc_q [SNDCP_SEGMENT_NUMBERS_ACK];
  /*
   * The read pointer for ll_desc_q.
   */
  U8 ll_desc_q_read;
  /*
   * The write pointer for ll_desc_q.
   */
  U8 ll_desc_q_write;
#else /* _SNDCP_DTI_2_ */
  /*
   * An Array with a number of T_LL_DATA_REQ* to store all
   * segments of 1 N-PDU after they have been received from cia.
   */
  T_LL_DATA_REQ* ll_data_q [SNDCP_SEGMENT_NUMBERS_ACK];
  /*
   * The read pointer for ll_data_q.
   */
  UBYTE ll_data_q_read;
  /*
   * The write pointer for ll_data_q.
   */
  UBYTE ll_data_q_write;
#endif /* _SNDCP_DTI_2_ */
  /*
   * In what state is cia?
   * Set to CIA_IDLE at init time and when last segment of N-PDU has been sent
   * to LLC.
   * Set to CIA_BUSY whenever sig_sua_cia_cia_comp_req is sent.
   */
  UBYTE cia_state;
} T_SUA_DATA;



typedef struct /* T_ND_DATA */
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
   * the affected nsapi
   */
  UBYTE nsapi;
  /*
   * Receive npdu number for recovery state.
   */
  UBYTE npdu_num;
} T_ND_DATA;

typedef struct /* T_PD_DATA */
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
   * the affected sapi
   */
  UBYTE sapi;
} T_PD_DATA;

typedef struct /* T_PDA_DATA */
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
   * the affected sapi
   */
  UBYTE sapi;
} T_PDA_DATA;


typedef struct /* T_SD_DATA */
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
   * the affected sapi
   */
  UBYTE sapi;
  /*
   * TRUE if an LL_GETUNITDATA_REQ is pending, FALSE else
   */
  BOOL llc_may_send;
  /*
   * The SN_UNITDATA_IND that iscurrently being assembled.
   */
  T_SN_UNITDATA_IND* cur_sn_unitdata_ind[SNDCP_NUMBER_OF_NSAPIS];
  /*
   * Is the affected NSAPI receptive?
   */
  BOOL nsapi_rec_ra[SNDCP_NUMBER_OF_NSAPIS];

} T_SD_DATA;

typedef struct /* T_SDA_DATA */
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
   * the affected sapi
   */
  UBYTE sapi;
  /*
   * TRUE if an LL_GETDATA_REQ is pending, FALSE else
   */
  BOOL llc_may_send;
  /*
   * The SN_DATA_IND that iscurrently being assembled.
   */
  T_SN_DATA_IND* cur_sn_data_ind[SNDCP_NUMBER_OF_NSAPIS];
  /*
   * Is the affected NSAPI receptive?
   */
  BOOL nsapi_rec_ra[SNDCP_NUMBER_OF_NSAPIS];
  /*
   * The N-PDU number of the current uncompressed N-PDU.
   */
  UBYTE uncomp_npdu_num;

} T_SDA_DATA;




typedef struct /* T_SNDCP_DATA */
{


  /*
   * Entity global arrays for organizing purposes
   */

  /*
   * index: nsapi, elements: qos;
   * indicating the quality of service for the given NSAPI.
   */
  T_snsm_qos nsapi_qos_ra[SNDCP_NUMBER_OF_NSAPIS];

  /*
   * index: nsapi, elements: flags;
   * indicating whether the given NSAPI (0..15) is currently in use or not.
   */
  BOOL nsapi_used_ra[SNDCP_NUMBER_OF_NSAPIS];

  /*
   * index: nsapi, elements: flags;
   * indicating the sapi that is used by a certain nsapi.
   */
  UBYTE nsapi_sapi_ra[SNDCP_NUMBER_OF_NSAPIS];

  /*
   * index: nsapi, elements: the radio priority to be used in lower layers;
   * indicating the radio priority to be used in LL_(UNIT-)DATA_REQ prims.
   */
  UBYTE nsapi_prio_ra[SNDCP_NUMBER_OF_NSAPIS];

#ifdef REL99
  /*
   * index: nsapi, elements: the packet flwo identifier to be used in lower layers;
   * indicating the packet flwo identifier to be used in LL_(UNIT-)DATA_REQ prims.
   */
  U16 nsapi_pktflowid_ra[SNDCP_NUMBER_OF_NSAPIS];
#endif /*REL99*/

  /*
   * index: sapi, elements: LLC acknowledged?
   * indicating acknowledged (TRUE) or unacknowledged (FALSE) LLC operation
   * mode of the given LLC SAPI.
   */
  BOOL sapi_ack_ra[SNDCP_NUMBER_OF_SAPIS];

  /*
   * index: nsapi, elements: LLC acknowledged?
   * indicating acknowledged (TRUE) or unacknowledged (FALSE) LLC operation
   * mode of the given NSAPI.
   */
  BOOL nsapi_ack_ra[SNDCP_NUMBER_OF_NSAPIS];

  /*
   * index: sapi, elements: the state;
   * indicating one of MG_EST, MG_REL etc...
   */
  USHORT sapi_state_ra[SNDCP_NUMBER_OF_SAPIS];

  /*
   * index: nsapi, elements: the state;
   * indicating one of MG_EST, MG_REL etc...
   */
  USHORT nsapi_state_ra[SNDCP_NUMBER_OF_NSAPIS];

  /*
   * How many Van Jacobson compressors have been set up?
   * Must not be bigger than SNDCP_MAX_VJ_COUNT.
   */
  UBYTE vj_count;
  /*
   * SNDCP sends an LL_XID_REQ at each context activation.
   * This may switched off by setting this variable to FALSE.
   */
  BOOL always_xid;

#ifndef NCONFIG
  /*
   * In case of config primitive SEND_UNITDATA this variable indicates the
   * number of octets to be sent.
   */
  U32 data_count[SNDCP_NUMBER_OF_NSAPIS];
  /*
   * Number of milliseconds to delay each SSM_ACTIVATE_IND after config prim DELAY.
   */
  USHORT millis;
#endif


#define _SNDCP_MEAN_TRACE_
#ifdef _SNDCP_MEAN_TRACE_
  /*
   * Used for tracing mean throughput up and down SN SAP.
   */
  /*
   * Time in ms.
   */
  T_TIME start_time_uplink_ack [SNDCP_NUMBER_OF_NSAPIS];
  T_TIME start_time_uplink_unack [SNDCP_NUMBER_OF_NSAPIS];
  T_TIME start_time_downlink_ack [SNDCP_NUMBER_OF_NSAPIS];
  T_TIME start_time_downlink_unack [SNDCP_NUMBER_OF_NSAPIS];

  T_TIME cur_time_uplink_ack [SNDCP_NUMBER_OF_NSAPIS];
  T_TIME cur_time_uplink_unack [SNDCP_NUMBER_OF_NSAPIS];
  T_TIME cur_time_downlink_ack [SNDCP_NUMBER_OF_NSAPIS];
  T_TIME cur_time_downlink_unack [SNDCP_NUMBER_OF_NSAPIS];

  /*
   * Number of transferred octets.
   * Skipped octets after config prim DISCARD are skipped.
   */
  ULONG cur_num_uplink_ack [SNDCP_NUMBER_OF_NSAPIS];
  ULONG cur_num_uplink_unack [SNDCP_NUMBER_OF_NSAPIS];
  ULONG cur_num_downlink_ack [SNDCP_NUMBER_OF_NSAPIS];
  ULONG cur_num_downlink_unack [SNDCP_NUMBER_OF_NSAPIS];

  /*
   * Number of transferred packets.
   * Skipped packets after config prim DISCARD are skipped.
   */
  ULONG cur_pac_uplink_ack [SNDCP_NUMBER_OF_NSAPIS];
  ULONG cur_pac_uplink_unack [SNDCP_NUMBER_OF_NSAPIS];
  ULONG cur_pac_downlink_ack [SNDCP_NUMBER_OF_NSAPIS];
  ULONG cur_pac_downlink_unack [SNDCP_NUMBER_OF_NSAPIS];

  /*
   * Direction and ack_mode parameters.
   */
#define SNDCP_MEAN_UP       1
#define SNDCP_MEAN_DOWN     2
#define SNDCP_MEAN_ACK      3
#define SNDCP_MEAN_UNACK    4

#endif /* _SNDCP_MEAN_TRACE_ */

#ifndef SNDCP_UPM_INCLUDED 
#define SNDCP_INTERFACE_ACK   1
#endif 
#define SNDCP_INTERFACE_UNACK 0

#ifdef _SNDCP_DTI_2_
  DTI_HANDLE hDTI;
#else /* _SNDCP_DTI_2_*/
  DTI_HANDLE* hDTI;
#endif /* _SNDCP_DTI_2_*/




  U32 nsapi_linkid_ra [SNDCP_NUMBER_OF_NSAPIS];
  U8* nsapi_neighbor_ra [SNDCP_NUMBER_OF_NSAPIS];
  U8  nsapi_interface_ra [SNDCP_NUMBER_OF_NSAPIS];
  /*
   * Possible values from dti.h: HOME or NEIGHBOR
   * Do not use SN or SNSM values.
   */
  BOOL nsapi_direction_ra [SNDCP_NUMBER_OF_NSAPIS];

#ifdef FLOW_TRACE
  BOOL  flow_control_ra [5][2][2];
#endif /* FLOW_TRACE */

  /*
   * Service data structures
   *
   * Services with multiple incarnations require an array of structures
   * named xxx_base[] with xxx = service abbrevation, and additionally a
   * pointer named *xxx, which will be accessed instead of xxx_base.
   *
   * Services with only one incarnation just have to declare one structure
   * named xxx (no need for xxx_base[] and *xxx).
   *
   * The differentiation between the two access possibilites is made with
   * the defines of the service names above (SNDCP_SERVICE_XXX).
   */
  T_CIA_DATA                   cia;

  T_MG_DATA                    mg;

  T_PD_DATA                    pd_base[SD_NUM_INC];
  T_PD_DATA                    *pd;

  T_PDA_DATA                   pda_base[SD_NUM_INC];
  T_PDA_DATA                   *pda;

  T_NU_DATA                    nu_base[NU_NUM_INC];
  T_NU_DATA                    *nu;

  T_SU_DATA                    su_base[SU_NUM_INC];
  T_SU_DATA                    *su;

  T_SUA_DATA                    sua_base[SUA_NUM_INC];
  T_SUA_DATA                    *sua;

  T_ND_DATA                    nd_base[ND_NUM_INC];
  T_ND_DATA                    *nd;

  T_SD_DATA                    sd_base[SD_NUM_INC];
  T_SD_DATA                    *sd;

  T_SDA_DATA                    sda_base[SDA_NUM_INC];
  T_SDA_DATA                    *sda;

  /*
   * Receiving states.(Regarding to 3GPP 04.65, capture 6.7.1.2)
   * index: NSAPI number, elements: SD/SDA receiving states relaiting to NSAPI.
   * One SAPI can receive SN-PDU's for diffent NSAPI's used currently on 
   * this SAPI. In this case SD/SDA services have to have different receiving 
   * states for each on this SAPI achtive NSAPI.
   */
  UBYTE rec_states[SNDCP_NUMBER_OF_NSAPIS];

  /*
   * Reference to pdu.
   */
  T_pdu_ref cur_pdu_ref[SNDCP_NUMBER_OF_NSAPIS];
  /*
   * The dcomp value in the first segment of the currently reassembled N-PDU.
   */
  UBYTE cur_dcomp[SNDCP_NUMBER_OF_NSAPIS];
  /*
   * The pcomp value in the first segment of the currently reassembled N-PDU.
   */
  UBYTE cur_pcomp[SNDCP_NUMBER_OF_NSAPIS];
  /*
   * First and/or last segment?
   */
  UBYTE cur_seg_pos[SNDCP_NUMBER_OF_NSAPIS];
  /*
   * Has the currently reassembled n-pdu a big header?
   */
  BOOL big_head[SNDCP_NUMBER_OF_NSAPIS];

 /*
  * Global variable indicating if the config primitive
  * TRACE_HEADER_ON has been received and the IP header
  * will be traced
  */
  BOOL trace_ip_header;
 /*
  * Global variable indicating if the config primitive
  * TRACE_PACKET_ON has been received and the complete
  * IP packets (Header+Payload) will traced
  */
  BOOL trace_ip_datagram;
 /*
  * Global variable indicating if the config primitive
  * TRACE_IP_BIN has been received and the complete
  * IP packets (Header+Payload) will be exported via BIN TRACE
  */
  BOOL trace_ip_bin;
  /*
   * This flag indicates if IP filter is activated or not. Per default
   * the IP filter is disabled and the state is set to FALSE.
   * TRUE means "IP_FILTER_ON" was received and filter is activated.
   */
  BOOL ip_filter;
  /*
   * This Counter indicates the Current Segment Number in single NPDU
   * which is being assembled currently.
   * This Counter Value is introduced to encounter the PARTITION 
   * Problem which we may face when there are more than 100 segments in
   * one NPDU are received in downlink.
   */
  U8 cur_segment_number[SNDCP_NUMBER_OF_NSAPIS];
  
  /* Flag to indicate if flow control is received before NSAPI is used.
   * If the flow control is received the corresponding bit for the NSAPI
   * is set in the variable 
   */
  USHORT nsapi_rcv_rdy_b4_used;
  U8 nsapi;
  BOOL tcp_flow;
} T_SNDCP_DATA;


/*==== EXPORT =====================================================*/

/*
 * Entity data base
 */
#ifdef SNDCP_PEI_C
       T_SNDCP_DATA sndcp_data_base, *sndcp_data;
#else
EXTERN T_SNDCP_DATA sndcp_data_base, *sndcp_data;
#endif

#define ENTITY_DATA           sndcp_data


/*
 * Communication handles
 */
#define hCommMMI             _ENTITY_PREFIXED(hCommMMI)

#ifdef SNDCP_UPM_INCLUDED 
#define hCommUPM             _ENTITY_PREFIXED(hCommUPM)
#else
#define hCommSM              _ENTITY_PREFIXED(hCommSM)
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/

#define hCommLLC              _ENTITY_PREFIXED(hCommLLC)

#ifdef SNDCP_PEI_C
       T_HANDLE hCommMMI       = VSI_ERROR;
/*       T_HANDLE hCommSM         = VSI_ERROR; */
#ifdef SNDCP_UPM_INCLUDED 
       T_HANDLE hCommUPM         = VSI_ERROR;
#endif  /*SNDCP_UPM_INCLUDED*/
       T_HANDLE hCommLLC        = VSI_ERROR;

       T_HANDLE SNDCP_handle;
/*
 * make the pei_create function unique
 */
#define pei_create _ENTITY_PREFIXED(pei_create)


#else
EXTERN T_HANDLE hCommMMI;
/*EXTERN T_HANDLE hCommSM; */
#ifdef SNDCP_UPM_INCLUDED 
EXTERN T_HANDLE hCommUPM;
#endif
EXTERN T_HANDLE hCommLLC;

EXTERN T_HANDLE SNDCP_handle;

#endif /* SNDCP_PEI_C */


#endif /* SNDCP_H */
