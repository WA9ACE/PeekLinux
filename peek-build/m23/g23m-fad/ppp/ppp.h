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
|  Purpose :  Definitions for the Protocol Stack Entity
|             Point-to-Point Protocol (PPP)
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_H
#define PPP_H

/*==== MACROS ======================================================*/

/*
 * defines the user of the vsi interface
 */
#define VSI_CALLER            PPP_handle,
#define VSI_CALLER_SINGLE     PPP_handle

/*
 * VERSION
 *
 * Description :  The constants define the type and the value
 *                of a version identification. The version
 *                is part of the monitor struct.
 */
#define VERSION_PPP   "PPP 1.0"

/*
 * This is just a TEMPORARY define until the issues with OPTION_MULTITHREAD
 * are settled. This define SHOULD be contained in GSM.H.
 */
#undef _ENTITY_PREFIXED
#define _ENTITY_PREFIXED(N) ppp_##N

/*
 * these macros might be defined more globally!!!
 */
#ifdef FRAME_OFFSET_ZERO

#define GET_PD(s,p)               p=s.buf[3] & 0x0F
#define GET_TI(s,t)               t=(s.buf[3] & 0xF0)>>4

#else /* FRAME_OFFSET_ZERO */

#define GET_PD(s,p)               ccd_decodeByte(s.buf, (USHORT)(s.o_buf+4), 4, &p)
#define GET_TI(s,t)               ccd_decodeByte(s.buf, s.o_buf, 4, &t)

#endif /* FRAME_OFFSET_ZERO */


/*==== CONSTANTS ======================================================*/

/*
 * Bitoffset for encoding/decoding
 */
#define ENCODE_OFFSET         0


/*
 * Service definitions. Used to access service data with GET/SET_STATE.
 *
 * Services with multiple incarnation have to be defined as xxx->
 * Services with only one incarnation have to be defined as xxx.
 *
 */
#define PPP_SERVICE_RT    rt. 
#define PPP_SERVICE_PRX   prx.
#define PPP_SERVICE_PTX   ptx.
#define PPP_SERVICE_FRX   frx.
#define PPP_SERVICE_FTX   ftx.
#define PPP_SERVICE_ARB   arb.
#define PPP_SERVICE_LCP   lcp.
#define PPP_SERVICE_ONA   ona->
#define PPP_SERVICE_NCP   ncp.
#define PPP_SERVICE_PAP   pap.
#define PPP_SERVICE_CHAP  chap.


/*
 * Service name definitions for trace purposes. 
 */
#ifndef NTRACE

#define SERVICE_NAME_PPP_SERVICE_RT   "RT"
#define SERVICE_NAME_PPP_SERVICE_PRX  "PRX"
#define SERVICE_NAME_PPP_SERVICE_PTX  "PTX"
#define SERVICE_NAME_PPP_SERVICE_FRX  "FRX"
#define SERVICE_NAME_PPP_SERVICE_FTX  "FTX"
#define SERVICE_NAME_PPP_SERVICE_ARB  "ARB"
#define SERVICE_NAME_PPP_SERVICE_LCP  "LCP"
#define SERVICE_NAME_PPP_SERVICE_ONA  "ONA"
#define SERVICE_NAME_PPP_SERVICE_NCP  "NCP"
#define SERVICE_NAME_PPP_SERVICE_PAP  "PAP"
#define SERVICE_NAME_PPP_SERVICE_CHAP "CHAP"

#endif /* !NTRACE */


/*
 * State definitions for each service.
 */

#define RT_STOPPED                                              1
#define RT_STARTED                                              2

#define PRX_NDTI                                                3
#define PRX_NDTI_START_FLOW                                     4
#define PRX_READY                                               5
#define PRX_DEAD                                                6
#define PRX_DEAD_DTI                                            7

#define PTX_READY                                               8
#define PTX_BLOCKED                                             9
#define PTX_DEAD                                               10

#define FRX_READY_NDTI                                         11
#define FRX_TRANSPARENT_NDTI                                   12
#define FRX_READY                                              13
#define FRX_DEAD                                               14
#define FRX_DEAD_DTI                                           15
#define FRX_TRANSPARENT                                        16

#define FTX_READY                                              17
#define FTX_BLOCKED                                            18
#define FTX_READY_DTI_FULL                                     19
#define FTX_BLOCKED_DTI_FULL                                   20
#define FTX_DEAD                                               21
#define FTX_DEAD_START_FLOW                                    22
#define FTX_TRANSPARENT                                        23
#define FTX_TRANSPARENT_DTI_FULL                               24

#define ARB_DEAD                                               25
#define ARB_LCP_ESTABLISH                                      26
#define ARB_LCP_PDP                                            27
#define ARB_LCP_NCP                                            28
#define ARB_LCP_RECONF                                         29
#define ARB_LCP_MOD                                            30
#define ARB_PAP_ESTABLISH                                      31
#define ARB_PAP_RECONF                                         32
#define ARB_CHAP_ESTABLISH                                     33
#define ARB_CHAP_RECONF                                        34
#define ARB_PDP_WAIT                                           35
#define ARB_PDP_SENT                                           36
#define ARB_NCP_ESTABLISH                                      37
#define ARB_NCP_RECONF                                         38
#define ARB_NCP_MOD                                            39
#define ARB_IP                                                 40
#define ARB_TRANSPARENT                                        41

#define LCP_STATE                                              42

#define ONA_CLOSED                                             43
#define ONA_CLOSING                                            44
#define ONA_REQ_SENT                                           45
#define ONA_ACK_RCVD                                           46
#define ONA_ACK_SENT                                           47
#define ONA_OPENED                                             48

#define NCP_STATE                                              49

#define PAP_DOWN                                               50
#define PAP_UP                                                 51

#define CHAP_DOWN                                              52
#define CHAP_UP                                                53

/*
 * state definitions for PTX TX buffer
 */

#define PTX_BUFFER_FULL                                        70      /* a state of PTX TX buffer */
#define PTX_BUFFER_READY                                       71      /* a state of PTX TX buffer */

/*
 * state definitions for FTX TX buffer
 */
#define PTX_FTX_BUFFER_READY                                   72      /* a state of FTX TX buffer */
#define PTX_FTX_BUFFER_FULL                                    73      /* a state of FTX TX buffer */

/*
 * state definitions for uplink data flow
 */

#define FRX_DATA_FLOW_DEAD                                     74      /* a state of PTX TX buffer */
#define FRX_DATA_FLOW_READY                                    75      /* a state of PTX TX buffer */


/*
 * PPP spicific defines
 */
#define DTI_PID_IPCP              0x8021
#define DTI_PID_LCP               0xc021
#define DTI_PID_PAP               0xc023
#define DTI_PID_CHAP              0xc223

#define CODE_CONF_REQ             1
#define CODE_CONF_ACK             2
#define CODE_CONF_NAK             3
#define CODE_CONF_REJ             4
#define CODE_TERM_REQ             5
#define CODE_TERM_ACK             6
#define CODE_CODE_REJ             7
#define CODE_PROT_REJ             8
#define CODE_ECHO_REQ             9
#define CODE_ECHO_REP             10
#define CODE_DISC_REQ             11

#define CODE_AUTH_REQ             1
#define CODE_AUTH_ACK             2
#define CODE_AUTH_NAK             3

#define CODE_CHALLENGE            1
#define CODE_RESPONSE             2
#define CODE_SUCCESS              3
#define CODE_FAILURE              4

#define FORWARD_RCRP              1
#define FORWARD_RCRN              2
#define FORWARD_RCA               3
#define FORWARD_RCN               4
#define FORWARD_RTR               5
#define FORWARD_RTA               6
#define FORWARD_RUC               7
#define FORWARD_RXJN              8
#define FORWARD_RER               9
#define FORWARD_RPJ_LCP           10
#define FORWARD_RPJ_PAP           12
#define FORWARD_RPJ_CHAP          13
#define FORWARD_RPJ_IPCP          14
#define FORWARD_RPJ_IP            15
#define FORWARD_RPJ_CTCP          16
#define FORWARD_RPJ_UTCP          17
#define FORWARD_DISCARD           18
#define FORWARD_RARP              19
#define FORWARD_RARN              20
#define FORWARD_RAA               21
#define FORWARD_RAN               22
#define FORWARD_RC                23
#define FORWARD_RRP               24
#define FORWARD_RRN               25
#define FORWARD_RS                26
#define FORWARD_RF                27

#define PROTOCOL_LCP_MSB          0xc0
#define PROTOCOL_LCP_LSB          0x21
#define PROTOCOL_PAP_MSB          0xc0
#define PROTOCOL_PAP_LSB          0x23
#define PROTOCOL_CHAP_MSB         0xc2
#define PROTOCOL_CHAP_LSB         0x23
#define PROTOCOL_VJ_MSB           0x00
#define PROTOCOL_VJ_LSB           0x2d
#define PROTOCOL_IPCP_MSB         0x80
#define PROTOCOL_IPCP_LSB         0x21

#define PPP_INITFCS               0xffff
#define PPP_GOODFCS               0xf0b8

#define PPP_HDLC_ESCAPE           0x7d
#define PPP_HDLC_FLAG             0x7e
#define PPP_ASCII_DEL             0x7f

#define PPP_AP_DEFAULT            PPP_AP_NO
#define PPP_ACFC_DEFAULT          FALSE
#define PPP_PFC_DEFAULT           FALSE
#define PPP_HC_DEFAULT            PPP_HC_OFF
#define PPP_MSID_DEFAULT          0
#define PPP_IP_DEFAULT            PPP_IP_DYNAMIC
#define PPP_PDNS_DEFAULT          PPP_DNS1_DYNAMIC
#define PPP_SDNS_DEFAULT          PPP_DNS2_DYNAMIC
#define PPP_GATEWAY_DEFAULT       PPP_IP_DYNAMIC

#define PPP_MRU_MIN               1500

#define CHAP_MSG_DIGEST_LEN       16

#define FTX_QUEUE_SIZE            1
#define PTX_QUEUE_SIZE            1


#define PPP_PCO_MASK_LCP_MRU      0x00000001 /* MRU value */
#define PPP_PCO_MASK_LCP_AP       0x00000002 /* LCP authentication protocol */
#define PPP_PCO_MASK_LCP_TWO      0x00000004 /* two LCP packets */
#define PPP_PCO_MASK_AUTH_PROT    0x00000008 /* authentification packets (PAP/CHAP) */
#define PPP_PCO_MASK_IPCP_HC      0x00000010 /* VJ header compression */
#define PPP_PCO_MASK_IPCP_IP      0x00000020 /* IP address */
#define PPP_PCO_MASK_IPCP_PDNS    0x00000040 /* primary DNS address */
#define PPP_PCO_MASK_IPCP_SDNS    0x00000080 /* secondary DNS address */
#define PPP_PCO_MASK_IPCP_GATEWAY 0x00000100 /* gateway address */
#ifdef _SIMULATION_
#define PPP_PCO_MASK_DEFAULT      (PPP_PCO_MASK_LCP_MRU   | \
                                   PPP_PCO_MASK_LCP_AP    | \
                                   PPP_PCO_MASK_LCP_TWO   | \
                                   PPP_PCO_MASK_AUTH_PROT | \
                                   PPP_PCO_MASK_IPCP_HC   | \
                                   PPP_PCO_MASK_IPCP_IP   | \
                                   PPP_PCO_MASK_IPCP_PDNS | \
                                   PPP_PCO_MASK_IPCP_SDNS)
#else /* _SIMULATION_ */
#define PPP_PCO_MASK_DEFAULT      (PPP_PCO_MASK_AUTH_PROT | \
                                   PPP_PCO_MASK_IPCP_PDNS | \
                                   PPP_PCO_MASK_IPCP_SDNS)
#endif /* _SIMULATION_ */
/*
 * size of a descriptor which includes the frame
 */
#define FTX_GET_DESC_SIZE          (400 - (UBYTE)sizeof(T_desc2))

/*
 * frx_add_desc states 
 */
#define FRX_ADD_HDLC_BEGIN                                      1
#define FRX_ADD_ADDRESS                                         2
#define FRX_ADD_CONTROL                                         3
#define FRX_ADD_PROTOCOL1                                       4
#define FRX_ADD_PROTOCOL2                                       5
#define FRX_ADD_INFORMATION                                     6
#define FRX_ADD_FCS1                                            7
#define FRX_ADD_FCS2                                            8
#define FRX_ADD_HDLC_END                                        9
#define FRX_ADD_ERROR                                          10

/*
 * Definitions for DTILIB
 */

#define PROT_LAYER                0
#define PEER_LAYER                1
#define PROT_CHANNEL              0
#if defined(_SIMULATION_)
#define PEER_CHANNEL              0
#else
#define PEER_CHANNEL              1
#endif
#define MAX_TRIES_OPEN_VSI_CHAN   5
#define PPP_INSTANCE              0
#define MAX_PPP_LINKS             2

/*
 * packet size for normal PPP packets
 * this value is just used to reduce the allocated memory
 */
#define FRX_ADD_SMALL_PACKET_SIZE          (100 - (UBYTE)(sizeof(T_desc2)))

/*
 * number of ONA incarnations: LCP and NCP
 */
#define ONA_NUM_INC                                             2

#ifdef FF_STATUS_TE
/*
 * driver handle for wakeup driver
 */
#define STE_HANDLE                                         0x0001
#endif /* FF_STATUS_TE */

/*
 * Timer handle definitions.
 */

#define RT_INDEX                 0
#define TIMER_MAX               (RT_INDEX + 1)

/*==== TYPES ======================================================*/

#ifdef OPTION_TIMER
/*
 * timer configuration table
 */
typedef struct
{
  UBYTE            t_mode;
  ULONG            t_val;
} T_TIMER_CONFIG;
#endif /* OPTION_TIMER */

/*
 * Test environment (simulator) vs. Implementation environment.
 */
typedef enum
{
  TEST_ENV,
  IMPL_ENV
} T_ENVIRONMENT;

typedef enum
{
  SERVICE_PTX,
  SERVICE_PRX,
  SERVICE_FRX,
  SERVICE_FTX,
  SERVICE_RT,
  SERVICE_ARB,
  SERVICE_PAP,
  SERVICE_CHAP,
  SERVICE_LCP,
  SERVICE_NCP,
  SERVICE_ONA,
  NO_SERVICE
} T_SERVICE;

/*
 * dat structure for each service
 */
typedef struct /* T_RT_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
  
  /*ab hier service spezif.
   * timeout value
   */
  T_TIME                      time;

} T_RT_DATA;


typedef struct /* T_PRX_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
  
  /* ab hier service spezif.
   * 
   */
#ifdef FF_STATUS_TE
  UBYTE    TE_data_buffered;     /* Status of TE-data-buffer asleep/awake */
  UBYTE    TE_data_buffer_p_id;  /* When TE asleep here the p_id is buffered */
  T_desc2* TE_data_buffer_desc_list_first; /* When TE asleep here the desc-list is buffered */
#endif /* FF_STATUS_TE */

} T_PRX_DATA;


typedef struct /* T_PTX_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
  /* 
   * service specific members 
   */
  USHORT          ftx_buffer_state;  /* state of FTX TX buffer */
  USHORT          ptx_buffer_state;  /* state of PTX TX buffer */

} T_PTX_DATA;


typedef struct /* T_FRX_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
  
  /* ab hier service spezif.
   * 
   */
  T_desc2*        received_data;     /* last received data stream */
  USHORT          proceed_data;      /* number of octets already proceeded */
  UBYTE           frame_complete;    /* indicates whether a complete frame is
                                        received or not */
  USHORT          stored_ptype;      /* stored packet type */
  T_desc2*        stored_packet;     /* stored packet */
  USHORT          stored_len;        /* stored packet length */
  USHORT          calc_fcs;          /* calculated fcs */
  UBYTE           escape;            /* indicates HDLC Control Escape detection */
  T_desc2*        cur_desc;          /* current descriptor for storing */
  USHORT          cur_desc_size;     /* size of the current descriptor buffer */
  UBYTE           store_state;       /* state of frame decoding */
  USHORT          data_flow_state;   /* state of FRX->PTX data flow */

#ifdef PPP_HDLC_TRACE
  T_desc2*        hdlc_frame;     /* complete HDLC frame stored for traces */
#endif /* PPP_HDLC_TRACE */

} T_FRX_DATA;


typedef struct /* T_FTX_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
  
  /* 
   * from here on service specific
   */

  UBYTE*       accmtab;  /* Async-Control-Character-Map Table */

  UBYTE        acfc;     /* Address and Control Field Compression */
  UBYTE        pfc;      /* Protocol Field Compression */

} T_FTX_DATA;


typedef struct /* T_ARB_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
  
  /* ab hier service spezif.
   * 
   */
  UBYTE         pri;          /* Protocol Reject Identifier */
  UBYTE         to_counter;   /* Time Out Counter */
  T_desc2*      last_ipcp;    /* Last sent IPCP packet */

UBYTE  dti_connect_state_prot; /* DTI Connect State for the Protocol Layer     */
UBYTE  dti_connect_state_peer; /* DTI Connect State for the Peer Layer         */

} T_ARB_DATA;


typedef struct /* T_LCP_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
  
  /* ab hier service spezif.
   * 
   */
  USHORT  req_mru;    /* requested maximum receive unit */
  ULONG   req_accm;   /* requested async control character map */
  UBYTE   req_ap;     /* requested authetication protocol */

  USHORT  s_mru;      /* to send maximum receive unit */
  ULONG   s_accm;     /* to send async control character map */
  UBYTE   s_pfc;      /* to send protocol field compression */
  UBYTE   s_acfc;     /* to send address and control field compression */
  USHORT  s_rejected; /* indicator of rejection of sent configuration entity_options */

  USHORT  r_mru;      /* received maximum receive unit */
  ULONG   r_accm;     /* received async control character map */
  UBYTE   r_pfc;      /* received protocol field compression */
  UBYTE   r_acfc;     /* received address and control field compression */

  UBYTE   n_ap;       /* negotiated authentication protocol */

  UBYTE   scr;        /* indicator whether configure request was sent */
  UBYTE   str;        /* indicator whether terminate request was sent */

  UBYTE   rcr;        /* indicator whether configure request was received */

  UBYTE   nscri;      /* new send configure request identifier */
  UBYTE   nstri;      /* new send terminate request identifier */
  UBYTE   nscji;      /* new send code reject identifier */

  UBYTE   lrcri;      /* last received configure request identifier */

} T_LCP_DATA;


typedef struct /* T_ONA_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
  
  /* ab hier service spezif.
   * 
   */
  UBYTE restarts; /* counter for Configure-Request and Terminate-Request sent */
  UBYTE failures; /* counter for Configure-Nak and Configure-Reject sent      */
  UBYTE loops;    /* counter for Configure-Ack received */

} T_ONA_DATA;


typedef struct /* T_NCP_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
  
  /* ab hier service spezif.
   * 
   */
  UBYTE   req_hc;       /* requested header compression protocol */
  UBYTE   req_msid;     /* requested max slot identifier */
  ULONG   req_ip;       /* requested IP address */
  ULONG   req_pdns;     /* requested primary DNS server address */
  ULONG   req_sdns;     /* requested secondary DNS server address */
  ULONG   req_gateway;  /* requested Gateway address */

  UBYTE   s_hc;       /* sent header compression */
  UBYTE   s_msid;     /* sent max slot identifier */
  USHORT  s_rejected; /* indicator of rejection of sent configuration entity_options */

  UBYTE   r_hc;       /* received header compression */
  UBYTE   r_msid;     /* received max slot identifier */

  ULONG   n_ip;       /* negotiated IP address */
  ULONG   n_pdns;     /* negotiated primary DNS server address */
  ULONG   n_sdns;     /* negotiated secondary DNS server address */
  ULONG   n_gateway;  /* negotiated Gateway address */

  UBYTE   scr;        /* indicator whether configure request was sent */
  UBYTE   str;        /* indicator whether terminate request was sent */

  UBYTE   rcr;        /* indicator whether configure request was received */

  UBYTE   nscri;      /* new send configure request identifier */
  UBYTE   nstri;      /* new send terminate request identifier */
  UBYTE   nscji;      /* new send code reject identifier */

  UBYTE   lrcri;      /* last received configure request identifier */

} T_NCP_DATA;


typedef struct /* T_PAP_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
  
  /* ab hier service spezif.
   * 
   */
  T_login  login;          /* PAP authentication values (client mode) */
  T_desc2* ar_packet;      /* PAP Authentication Request packet (server mode) */
  UBYTE    counter;        /* counter for timer restart */
  UBYTE    nari;           /* new authenticate request identifier */

} T_PAP_DATA;


typedef struct /* T_CHAP_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
  
  /* ab hier service spezif.
   * 
   */
  T_desc2* c_packet;       /* CHAP Challenge packet (server mode) */
  T_desc2* r_packet;       /* CHAP Response packet (server mode) */
  UBYTE    counter;        /* counter for timer restart */
  UBYTE    fails;          /* counter for failed authentication */
  UBYTE    nci;            /* new challenge identifier */
  UBYTE    sc;             /* indicator whether Challenge packet was sent */
  UBYTE    rc;             /* indicator whether Challenge packet was received */
  UBYTE    sr;             /* indicator whether Respons packet was sent */
  UBYTE    rs;             /* indicator whether Success packet was received */

} T_CHAP_DATA;


/*
 * summery of all service 
 */

typedef struct /* T_PPP_DATA */
{

  /*
   * PPP layer parameters 
   */
  UBYTE                       version;

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
   * the defines of the service names above (PPP_SERVICE_XXX).
   */
  T_RT_DATA                                     rt;
  T_PRX_DATA                                    prx;
  T_PTX_DATA                                    ptx;
  T_FRX_DATA                                    frx;
  T_FTX_DATA                                    ftx;
  T_ARB_DATA                                    arb;
  T_LCP_DATA                                    lcp;
  T_ONA_DATA                                    ona_base[ONA_NUM_INC];
  T_ONA_DATA                                    *ona;
  T_NCP_DATA                                    ncp;
  T_PAP_DATA                                    pap;
  T_CHAP_DATA                                   chap;

  /*
   * global PPP variables
   */

  UBYTE   mode; /* working mode (client, server or transparent) */

  UBYTE   mc;   /* Max. Configure */
  UBYTE   mt;   /* Max. Terminate */
  UBYTE   mf;   /* Max. Failure */
  T_login  login;  /* CHAP, PAP and AUTO authentication values*/

  U16     ppp_cause;  /* cause of PPP termination */

  USHORT  mru;  /* Maximum Receive Unit */
  UBYTE   n_hc; /* negotiated header compression */

  ULONG   pco_mask; /* determines PCO content */

  /*
   * Global variables used by DTILIB.
   */

  DTI_HANDLE ppphDTI;  /* Handle to DTI Data Base */

  /*
   * global PPP constants
   */
  USHORT*                                       fcstab;

} T_PPP_DATA;


/*==== EXPORT =====================================================*/


/*
 * Entity data base
 */

#ifdef PPP_PEI_C
T_PPP_DATA ppp_data_base, *ppp_data;
#else /* PPP_PEI_C */
EXTERN T_PPP_DATA ppp_data_base, *ppp_data;
#endif /* PPP_PEI_C */

#define ENTITY_DATA           ppp_data


/*
 * FCS and ACCM lookup tables 
 */

EXTERN USHORT     fcstab_base[256];
EXTERN UBYTE      accmtab_base[256];


/*
 * Communication handles (see also PPP_PEI.C, PPP_ARBP.C)
 */
#define hCommMMI                ppp_hCommMMI
#define hCommPPP                ppp_hCommPPP
#define hCommUPLINK             ppp_hCommUPLINK
#define hCommDOWNLINK           ppp_hCommDOWNLINK
/*
 * make PPP functions unique
 */
#define pei_create              _ENTITY_PREFIXED(pei_create)
#define rt_init                 _ENTITY_PREFIXED(rt_init)

#ifdef PPP_PEI_C
       T_HANDLE hCommMMI          = VSI_ERROR;
       T_HANDLE hCommPPP          = VSI_ERROR;
       T_HANDLE hCommUPLINK       = VSI_ERROR;
       T_HANDLE hCommDOWNLINK     = VSI_ERROR;

       T_HANDLE PPP_handle;

#else /* PPP_PEI_C */
  EXTERN T_HANDLE hCommMMI;
  EXTERN T_HANDLE hCommPPP;
  EXTERN T_HANDLE hCommUPLINK;
  EXTERN T_HANDLE hCommDOWNLINK;

  EXTERN T_HANDLE PPP_handle;
#endif /* PPP_PEI_C */

/*
 * If all entities are linked into one module this definitions
 * prefixes the global data with the enity name
 */
#ifdef OPTION_MULTITHREAD
  #define _decodedMsg   _ENTITY_PREFIXED(_decodedMsg)
#endif /* OPTION_MULTITHREAD */
#endif /* PPP_H */
