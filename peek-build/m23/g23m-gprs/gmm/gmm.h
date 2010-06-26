/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm.h
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
|             GPRS Mobility Management (GPRS)
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_H
#define GMM_H

#define ENTITY_GMM

#include "message.h"
/*==== DIAGNOSTICS ==========================================================*/
/* 
 * GMM_TCS4 is defined to use MMPM SAP document from TI DK
 */
#ifndef GMM_TCS4
#define GMM_TCS4
#endif


#ifdef TRACE_FUNC
#ifdef IDENTATION
  #define GMM_TRACE_FUNCTION(a)        \
    char trace_string[]=a;\
    vsi_o_ttrace(VSI_CALLER TC_FUNC,"%*s%s() {",gmm_data->deep,"",trace_string);\
    gmm_data->deep+=4;
  #define GMM_RETURN  vsi_o_ttrace(VSI_CALLER TC_FUNC,"%*s} /* %s */",gmm_data->deep-=4,"",trace_string);return;
  #define GMM_RETURN_(a) \
    vsi_o_ttrace(VSI_CALLER TC_FUNC,"%*s} /* %s() */",gmm_data->deep-=4,"",\
      trace_string);\
    return a;
#else
  #define GMM_TRACE_FUNCTION(a) vsi_o_ttrace(VSI_CALLER TC_FUNC,a)
  #define GMM_RETURN  return
  #define GMM_RETURN_(a)  return (a)

#endif

#else
  #define GMM_TRACE_FUNCTION(a)
  #define GMM_RETURN  return
  #define GMM_RETURN_(a)  return (a)
#endif 

#ifdef TRACE_ERR
#define GMM_TRACE_0_ERROR(s)    vsi_o_error_ttrace(s)
#define GMM_TRACE_1_ERROR(s,p1) vsi_o_error_ttrace(s,p1)
#else
#define GMM_TRACE_1_ERROR(s,p1)
#endif

#ifdef TRACE_EVE
  #define TRACE_0_DATA(s)             vsi_o_ttrace(VSI_CALLER TC_USER1, "Data: " s)
  #define TRACE_1_DATA(s,p1)          vsi_o_ttrace(VSI_CALLER TC_USER1, "Data: " s,p1)
  #define TRACE_2_DATA(s,p1,p2)       vsi_o_ttrace(VSI_CALLER TC_USER1, "Data: " s,p1,p2)
  #define TRACE_3_DATA(s,p1,p2,p3)    vsi_o_ttrace(VSI_CALLER TC_USER1, "Data: " s,p1,p2,p3)
  #define TRACE_4_DATA(s,p1,p2,p3,p4) vsi_o_ttrace(VSI_CALLER TC_USER1, "Data: " s,p1,p2,p3,p4)
  #define TRACE_5_DATA(s,p1,p2,p3,p4,p5)      vsi_o_ttrace(VSI_CALLER TC_USER1, "Data: " s,p1,p2,p3,p4,p5)
  #define TRACE_6_DATA(s,p1,p2,p3,p4,p5,p6)   vsi_o_ttrace(VSI_CALLER TC_USER1, "Data: " s,p1,p2,p3,p4,p5,p6)
  #define TRACE_7_DATA(s,p1,p2,p3,p4,p5,p6,p7)    vsi_o_ttrace(VSI_CALLER TC_USER1, "Data: " s,p1,p2,p3,p4,p5,p6,p7)
  #define TRACE_8_DATA(s,p1,p2,p3,p4,p5,p6,p7,p8) vsi_o_ttrace(VSI_CALLER TC_USER1, "Data: " s,p1,p2,p3,p4,p5,p6,p7,p8)
  #define TRACE_9_DATA(s,p1,p2,p3,p4,p5,p6,p7,p8,p9) vsi_o_ttrace(VSI_CALLER TC_USER1, "Data: " s,p1,p2,p3,p4,p5,p6,p7,p8,p9)
#else
  #define TRACE_0_DATA(s)             
  #define TRACE_1_DATA(s,p1)          
  #define TRACE_2_DATA(s,p1,p2)       
  #define TRACE_3_DATA(s,p1,p2,p3)    
  #define TRACE_4_DATA(s,p1,p2,p3,p4) 
  #define TRACE_5_DATA(s,p1,p2,p3,p4,p5) 
  #define TRACE_6_DATA(s,p1,p2,p3,p4,p5,p6) 
  #define TRACE_7_DATA(s,p1,p2,p3,p4,p5,p6,p7) 
  #define TRACE_8_DATA(s,p1,p2,p3,p4,p5,p6,p7,p8) 
  #define TRACE_9_DATA(s,p1,p2,p3,p4,p5,p6,p7,p8,p9) 
#endif


#ifdef TRACE_EVE
  #define TRACE_0_INFO(s)             vsi_o_ttrace(VSI_CALLER TC_EVENT, "Info: " s)
  #define TRACE_1_INFO(s,p1)          vsi_o_ttrace(VSI_CALLER TC_EVENT, "Info: " s,p1)
  #define TRACE_2_INFO(s,p1,p2)       vsi_o_ttrace(VSI_CALLER TC_EVENT, "Info: " s,p1,p2)
  #define TRACE_3_INFO(s,p1,p2,p3)    vsi_o_ttrace(VSI_CALLER TC_EVENT, "Info: " s,p1,p2,p3)
  #define TRACE_4_INFO(s,p1,p2,p3,p4) vsi_o_ttrace(VSI_CALLER TC_EVENT, "Info: " s,p1,p2,p3,p4)
#else
  #define TRACE_0_INFO(s)             
  #define TRACE_1_INFO(s,p1)          
  #define TRACE_2_INFO(s,p1,p2)       
  #define TRACE_3_INFO(s,p1,p2,p3)    
  #define TRACE_4_INFO(s,p1,p2,p3,p4) 
#endif

#ifdef TRACE_PRIM
  #define TRACE_0_PARA(s)             vsi_o_ttrace(VSI_CALLER TC_PRIM, "Para: " s)
  #define TRACE_1_PARA(s,p1)          vsi_o_ttrace(VSI_CALLER TC_PRIM, "Para: " s,p1)
  #define TRACE_2_PARA(s,p1,p2)       vsi_o_ttrace(VSI_CALLER TC_PRIM, "Para: " s,p1,p2)
  #define TRACE_3_PARA(s,p1,p2,p3)    vsi_o_ttrace(VSI_CALLER TC_PRIM, "Para: " s,p1,p2,p3)
  #define TRACE_4_PARA(s,p1,p2,p3,p4) vsi_o_ttrace(VSI_CALLER TC_PRIM, "Para: " s,p1,p2,p3,p4)
  #define TRACE_7_PARA(s,p1,p2,p3,p4,p5,p6,p7) vsi_o_ttrace(VSI_CALLER TC_PRIM, "Para: " s,p1,p2,p3,p4,p5,p6,p7)
  #define TRACE_8_PARA(s,p1,p2,p3,p4,p5,p6,p7,p8) vsi_o_ttrace(VSI_CALLER TC_PRIM, "Para: " s,p1,p2,p3,p4,p5,p6,p7,p8)
  #define TRACE_9_PARA(s,p1,p2,p3,p4,p5,p6,p7,p8,p9) vsi_o_ttrace(VSI_CALLER TC_PRIM, "Para: " s,p1,p2,p3,p4,p5,p6,p7,p8,p9)
  #define TRACE_10_PARA(s,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10) vsi_o_ttrace(VSI_CALLER TC_PRIM, "Para: " s,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10)
  #define TRACE_11_PARA(s,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) vsi_o_ttrace(VSI_CALLER TC_PRIM, "Para: " s,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)  
  #define TRACE_12_PARA(s,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12) vsi_o_ttrace(VSI_CALLER TC_PRIM, "Para: " s,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12)  

  #define TRACE_0_OUT_PARA(s)             vsi_o_ttrace(VSI_CALLER TC_PRIM, "OPar: " s)
  #define TRACE_1_OUT_PARA(s,p1)          vsi_o_ttrace(VSI_CALLER TC_PRIM, "OPar: " s,p1)
  #define TRACE_2_OUT_PARA(s,p1,p2)       vsi_o_ttrace(VSI_CALLER TC_PRIM, "OPar: " s,p1,p2)
  #define TRACE_3_OUT_PARA(s,p1,p2,p3)    vsi_o_ttrace(VSI_CALLER TC_PRIM, "OPar: " s,p1,p2,p3)
  #define TRACE_4_OUT_PARA(s,p1,p2,p3,p4) vsi_o_ttrace(VSI_CALLER TC_PRIM, "OPar: " s,p1,p2,p3,p4)
  #define TRACE_8_OUT_PARA(s,p1,p2,p3,p4,p5,p6,p7,p8) vsi_o_ttrace(VSI_CALLER TC_PRIM, "OPar: " s,p1,p2,p3,p4,p5,p6,p7,p8)
  #define TRACE_9_OUT_PARA(s,p1,p2,p3,p4,p5,p6,p7,p8,p9) vsi_o_ttrace(VSI_CALLER TC_PRIM, "OPar: " s,p1,p2,p3,p4,p5,p6,p7,p8,p9)
  #define TRACE_PRIM_FROM(s)              vsi_o_ttrace(VSI_CALLER TC_PRIM, "Pdir: " s)
  #define TRACE_PRIM_TO(s)                vsi_o_ttrace(VSI_CALLER TC_PRIM, "Pdir: " s)
#else
  #define TRACE_0_PARA(s)             
  #define TRACE_1_PARA(s,p1)          
  #define TRACE_2_PARA(s,p1,p2)       
  #define TRACE_3_PARA(s,p1,p2,p3)    
  #define TRACE_4_PARA(s,p1,p2,p3,p4) 
  #define TRACE_7_PARA(s,p1,p2,p3,p4,p5,p6,p7)
  #define TRACE_8_PARA(s,p1,p2,p3,p4,p5,p6,p7,p8)
  #define TRACE_9_PARA(s,p1,p2,p3,p4,p5,p6,p7,p8,p9)
  #define TRACE_10_PARA(s,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10)
  #define TRACE_11_PARA(s,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)
  #define TRACE_12_PARA(s,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12)

  #define TRACE_0_OUT_PARA(s)             
  #define TRACE_1_OUT_PARA(s,p1)          
  #define TRACE_2_OUT_PARA(s,p1,p2)       
  #define TRACE_3_OUT_PARA(s,p1,p2,p3)    
  #define TRACE_4_OUT_PARA(s,p1,p2,p3,p4) 
  #define TRACE_8_OUT_PARA(s,p1,p2,p3,p4,p5,p6,p7,p8)
  #define TRACE_9_OUT_PARA(s,p1,p2,p3,p4,p5,p6,p7,p8,p9)
  #define TRACE_PRIM_FROM(s)  
  #define TRACE_PRIM_TO(s)
#endif


#ifndef TRACE_INFO       /* only used in local environment */
#  define TRACE_INFO(x)
#endif

#ifndef TRACE_COMMENT    /* only used in local environment */
#  define TRACE_COMMENT(x)
#endif

/*
 * This is just a TEMPORARY define until the issues with OPTION_MULTITHREAD
 * are settled. This define SHOULD be contained in GSM.H.
 */
#undef _ENTITY_PREFIXED
#define _ENTITY_PREFIXED(N) gmm_##N
/*
 * defines the user of the vsi interface
 */
#define VSI_CALLER            GMM_handle,
#define VSI_CALLER_SINGLE     GMM_handle

/* TCS 2.1 */
 /* TCS 2.1 */
 /* TCS 2.1 */
/*
 ****************************************************************************
 * The following declarations shall be identical with the corresponding 
 * declarations located in RR. 
 ****************************************************************************
 */
#ifdef GPRS
EXTERN UBYTE rr_csf_get_radio_access_capability( T_ra_cap *ra_cap );
#endif

/*
 ****************************************************************************
 * The above declarations shall be identical with the corresponding 
 * declarations located in RR. 
 ****************************************************************************
 */

/*
 * This is just a TEMPORARY define until the issues with OPTION_MULTITHREAD
 * are settled. This define SHOULD be contained in GSM.H.
 */

#define SEND_ATTACH_COMPLETE 1
#define SEND_NOT_ATTACH_COMPLETE 0
/*
 * max attach attempts
 */
#define MAX_AAC               5
/*
 * max attempts for timeout of T3310
 */
#define MAX_CT3310            5
/*
 * If the skip bit in the message is not 0 the message is invalid
 */
#define SKIP_VALID            0

/*
 * Bitoffset for encoding/decoding
 * LLC header size
 * and LLC tail size
 */
#define ENCODE_OFFSET         (37*8)
#define ENCODE_LLC_FCS_SIZE   (3*8)

/*
 * These MASKS are defined in GSM 03.03 and needed for translation
 * from PTMSI to TLLI
 */
/*
 *  11 ...
 */
#define LOCAL_TLLI_SET_MASK       0xC0000000
#define LOCAL_TLLI_RES_MASK       0xFFFFFFFF
/*
 *  10 ...
 */
#define FOREIGN_TLLI_SET_MASK     0x80000000
#define FOREIGN_TLLI_RES_MASK     0xBFFFFFFF
/*
 *  01111 ...
 */
#define RANDOM_TLLI_SET_MASK      0x78000000
#define RANDOM_TLLI_RES_MASK      0x7FFFFFFF

#define INVALID_PTMSI_SIGNATURE   0xffffff

#define INVALID_PDP_CONTEXT_STATUS  0xffff

#ifdef FRAME_OFFSET_ZERO

#define GET_PD(s,p)               p=s.buf[3] & 0x0F
#define GET_TI(s,t)               t=(s.buf[3] & 0xF0)>>4

#else

#define GET_PD(s,p)               ccd_decodeByte(s.buf, (USHORT)(s.o_buf+4), 4, &p)
#define GET_TI(s,t)               ccd_decodeByte(s.buf, s.o_buf, 4, &t)

#endif

/*
 * Protocol Discrimator
 */
#define PD_CC                     3
#define PD_MM                     5
#define PD_RR                     6
#define PD_GMM                    8
#define PD_SMS                    9
#define PD_SM                     10
#define PD_SS                     11
#define PD_TST                    15

/*
 * one second is defined as 1000 ms 
 */
#define SEC   1000

/*
 * The definition of detach types are different between uplink and downlink
 * direction. thatswhy i have to translate it.
 */
#define  GMM_DT_RE_ATTACH         20
/*
 *
 */
 #define  GMM_MM_DEREG                     0
 #define  GMM_MM_REG_NORMAL_SERVICE        1
 #define  GMM_MM_REG_NO_CELL_AVAILABLE      3
 #define  GMM_MM_REG_UPDATE_NEEDED          4 
 #define  GMM_MM_REG_INITATED_VIA_GPRS          5 
 /*
  * GRR state machine
  */
#define GMM_GRR_STATE_OFF   0
#define GMM_GRR_STATE_ON   1
#define GMM_GRR_STATE_SUSPENDING    2
#define GMM_GRR_STATE_SUSPENDED    3
#define GMM_GRR_STATE_CR   4
/*
  * LLC state machine
  */
#define GMM_LLC_STATE_SUSPENED_RAU    0
#define GMM_LLC_STATE_SUSPENED     1
#define GMM_LLC_STATE_ASSIGNED    2
#define GMM_LLC_STATE_UNASSIGNED   3 
/*
 * suspension cause
 */
#define GMM_SUSP_NONE           0x00
#define GMM_SUSP_UNKNOWN        0x00
#define GMM_SUSP_IMSI_DETACH    0x01
#define GMM_SUSP_LAU            0x02
#define GMM_SUSP_CALL           0x04
#define GMM_SUSP_EM_CALL        0x08
#define GMM_SUSP_LOCAL_DETACH       0x10

/*
 * bis_size_message_table - This table is need in Service TX to calculate
 *                          the BIT size from _decodedMsg
 */
#ifdef GMM_PEI_C
const int bit_size_message_table [/*MAX_MESSAGE_ID_GMM*/] =
{ 
  0, /* not defined                                                         =0x0*/
  BSIZE_ATTACH_REQUEST          , /* max bitlength of coded ATTACH_REQUEST  =0x1 */
  BSIZE_ATTACH_ACCEPT           , /* max bitlength of coded ATTACH_ACCEPT   =0x2 */        
  BSIZE_ATTACH_COMPLETE         ,/* max bitlength of coded ATTACH_COMPLETE  =0x3 */         
  BSIZE_ATTACH_REJECT           ,/* max bitlength of coded ATTACH_REJECT    =0x4 */        
  BSIZE_U_DETACH_REQUEST        ,/* max bitlength of coded U_DETACH_REQUEST =0x5*/         
  BSIZE_D_DETACH_ACCEPT         , /* max bitlength of coded D_DETACH_ACCEPT=0x6 */         
  0, /* not defined */
  BSIZE_ROUTING_AREA_UPDATE_REQUEST   , /* max bitlength of coded ROUTING_AREA_UPDATE_REQUEST   =0x8 */
  BSIZE_ROUTING_AREA_UPDATE_ACCEPT    , /* max bitlength of coded ROUTING_AREA_UPDATE_ACCEPT    =0x9 */
  BSIZE_ROUTING_AREA_UPDATE_COMPLETE  , /* max bitlength of coded ROUTING_AREA_UPDATE_COMPLETE  =0xa */
  BSIZE_ROUTING_AREA_UPDATE_REJECT    , /* max bitlength of coded ROUTING_AREA_UPDATE_REJECT    =0xb */
  0, /* not defined                                                                             =0xc */
  0, /* not defined                                                                             =0xd */
  0, /* not defined                                                                             =0xe */
  0, /* not defined                                                                             =0xf */
  BSIZE_P_TMSI_REALLOCATION_COMMAND   , /* max bitlength of coded P_TMSI_REALLOCATION_COMMAND   =0x10 */        
  BSIZE_P_TMSI_REALLOCATION_COMPLETE  , /* max bitlength of coded P_TMSI_REALLOCATION_COMPLETE  =0x11 */        
  BSIZE_AUTHENTICATION_AND_CIPHERING_REQUEST  , /* max bitlength of coded AUTHENTICATION_AND_CIPHERING_REQUEST  =0x12 */        
  BSIZE_AUTHENTICATION_AND_CIPHERING_RESPONSE , /* max bitlength of coded AUTHENTICATION_AND_CIPHERING_RESPONSE =0x13 */        
  BSIZE_AUTHENTICATION_AND_CIPHERING_REJECT   , /* max bitlength of coded AUTHENTICATION_AND_CIPHERING_REJECT   =0x14 */        
  BSIZE_IDENTITY_REQUEST         , /* max bitlength of coded IDENTITY_REQUEST =0x15 */
  BSIZE_IDENTITY_RESPONSE        , /* max bitlength of coded IDENTITY_RESPONSE=0x16 */        
  0, /* not defined                                                           =0x17 */
  0, /* not defined                                                           =0x18 */
  0, /* not defined                                                           =0x19 */
  0, /* not defined                                                           =0x1a */
  0, /* not defined                                                           =0x1b */
  0, /* not defined                                                           =0x1c */
  0, /* not defined                                                           =0x1d */
  0, /* not defined                                                           =0x1e */
  0, /* not defined                                                           =0x1f */
  BSIZE_GMM_STATUS             , /* max bitlength of coded D_GMM_STATUS       =0x20 */
  BSIZE_GMM_INFORMATION           /* max bitlength of coded GMM_INFORMATION   =0x21 */
};

#else
EXTERN const int bit_size_message_table [];
#endif



/*
 * Service definitions. Used to access service data with GET/SET_STATE.
 *
 * Services with multiple incarnation have to be defined as xxx->
 * Services with only one incarnation have to be defined as xxx.
 *
 * NOTE: For S1, LLC does access multiple incarnations with GET/SET_ISTATE.
 */
#define KERN                  kern.
#define CU                    kern.states.cu.
#define GU                    kern.states.gu.
#define MM                    kern.states.mm.
#define TX                    tx.
#define RX                    rx.
#define RDY                   rdy.
#define SYNC                  sync.

/*
 * Service name definitions for trace purposes. The service abbrevation
 * (e.g. LLME) has to be the same as above for the service definitions.
 */
#ifndef NTRACE

#define SERVICE_NAME_KERN     "KERN"
#define SERVICE_NAME_CU       "CU"
#define SERVICE_NAME_GU       "GU"
#define SERVICE_NAME_LLC      "LLC"
#define SERVICE_NAME_GRR      "GRR"
#define SERVICE_NAME_TX       "TX"
#define SERVICE_NAME_RX       "RX"
#define SERVICE_NAME_RDY      "RDY"
#define SERVICE_NAME_SYNC     "SYNC"
#define SERVICE_NAME_MM       "MM"

#endif /* !NTRACE */


/*
 * Timer definitions.
 */

#define kern_T3302  0     /* Timeout for Attach or RAU failer*/      
#define kern_T3310  1     /* Timeout for ATTACH */
#define kern_T3311  2     /* Timeout for Attach or RAU Reject*/
#define kern_T3321  3     /* Timeout for DETACH */

#define rdy_T3312   5      /* Standby or RAU timer */

#define kern_TPOWER_OFF    8     /* POWER OFF timer */
#define sync_TSYNC         9    /* timer for synchronization supervision, 
                                    between GRR CELL_IND and MM ACTIVATE_IND */
#define kern_TLOCAL_DETACH     10     /* disable GRR timer */

#define TIMER_MAX   11

/*
 *  AniteB2 V1.17 TC 44.2.1.2.8 Relese 99 ms
 */
#define T3302_VALUE       (12*60000)    /* ms */
 #define T3312_VALUE       (54*60000)    /* ms */

/* Anite B2 TC 44.2.2.1.4/ 44.2.1.1.7 */
#define T3310_VALUE       15000        /* ms */

#define T3311_VALUE       15000        /* ms */
#define T3321_VALUE       15000        /* ms */
#define T3314_VALUE       44000        /* ms */

#define T3316_VALUE       44000        /* ms */
#define TPOWER_OFF_VALUE   5000        /* ms */
#define TSYNC              2000         /* ms */
#define TLOCAL_DETACH_VALUE    5000        /* ms */


#ifdef REL99

/*For sgsnr handling*/
#define R_98NW 0
#define R_99NW 1

/*
 * Cell notification not supported
 */
#define NO_CELL_NOTIFY        0 /* TCS 4.0 */

/*
 * First cell notification in this RA. Don't use LLC NULL frame
 */
#define FIRST_CELL_NOTIFY     1 /* TCS 4.0 */

/*
 * Second or higher cell notification in this RA. Can use LLC NULL frame
 */
#define NOT_FIRST_CELL_NOTIFY 2 /* TCS 4.0 */
#endif


/*
 * State definitions for each service.
 */

#define KERN_GMM_NULL_NO_IMSI                      0
#define KERN_GMM_NULL_IMSI                         1
#define KERN_GMM_DEREG_INITIATED                   2  
#define KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH        3
#define KERN_GMM_DEREG_NO_CELL_AVAILABLE           4
#define KERN_GMM_DEREG_LIMITED_SERVICE             5
#define KERN_GMM_DEREG_NO_IMSI                     6
#define KERN_GMM_DEREG_PLMN_SEARCH                 7
#define KERN_GMM_DEREG_SUSPENDED                   8

#define KERN_GMM_REG_INITIATED                     9

#define KERN_GMM_REG_NO_CELL_AVAILABLE             10
#define KERN_GMM_REG_LIMITED_SERVICE               11
#define KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM       12
#define KERN_GMM_REG_ATTEMPTING_TO_UPDATE          13
#define KERN_GMM_REG_RESUMING                      14
#define KERN_GMM_REG_SUSPENDED                     15
#define KERN_GMM_REG_NORMAL_SERVICE                16

#define KERN_GMM_RAU_INITIATED                     17

#define KERN_GMM_RAU_WAIT_FOR_NPDU_LIST            18

#define KERN_GMM_REG_IMSI_DETACH_INITIATED         19

#define KERN_GMM_DEREG_SUSPENDING                  20
#define KERN_GMM_DEREG_RESUMING                    21

#define KERN_GMM_REG_SUSPENDING                    22
#define KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ  23
#define KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ     24
#define KERN_GMM_REG_TEST_MODE                     25
#define KERN_GMM_NULL_PLMN_SEARCH                  26
#define KERN_GMM_REG_TEST_MODE_NO_IMSI             27

#define TX_READY      0
#define TX_NOT_READY  1 

#define RX_READY      0
#define RX_NOT_READ   1

#define RDY_READY       0
#define RDY_STANDBY     1
#define RDY_DEACTIVATED 2
#define RDY_STANDBY_TWICE    3

enum
{
  SYNC_IDLE          = 0,
  SYNC_WAIT_FOR_GSM  = 1,
  SYNC_WAIT_FOR_GPRS = 2
};

/*==== TYPES ======================================================*/

/*
 * GMM global typedefs
 */


/*
 * things to do after receiving of gmmrr_suspend_cnf, whenn local detach has 
 * been called before.
 */
typedef enum
{
  GMM_LOCAL_DETACH_PROC_NOT_CHANGED           =0,
  GMM_LOCAL_DETACH_PROC_NOTHING               =1,
  GMM_LOCAL_DETACH_PROC_ENTER_NULL_IMSI_LIMITED_SERVICE_REQ=2,
  GMM_LOCAL_DETACH_PROC_ENTER_NULL_IMSI       =3,
  GMM_LOCAL_DETACH_PROC_ENTER_NULL_NO_IMSI    =4,
  GMM_LOCAL_DETACH_PROC_RESUME                =5,
  GMM_LOCAL_DETACH_PROC_ENTER_DEREG           =6,
  GMM_LOCAL_DETACH_PROC_UNASSIGN              =7,
  GMM_LOCAL_DETACH_PROC_ENTER_REG_NORMAL      =8,
  GMM_LOCAL_DETACH_PROC_SUSP_LAU              =9,
  GMM_LOCAL_DETACH_PROC_RE_ATTACH             =10,
  GMM_LOCAL_DETACH_PROC_RAU                   =11,
  GMM_LOCAL_DETACH_PROC_DISABLE               =12,
  GMM_LOCAL_DETACH_PROC_COMB_DISABLE          =13,
  GMM_LOCAL_DETACH_PROC_SIM_REMOVED           =14,
  GMM_LOCAL_DETACH_PROC_AUTH_FAILED           =15,
  GMM_LOCAL_DETACH_PROC_POWER_OFF             =16,
  GMM_LOCAL_DETACH_PROC_SOFT_OFF              =17
} T_LOCAL_DETACH_PROC;
/*
 * TLLI type
 */
typedef enum
{
  LOCAL_TLLI,
  FOREIGN_TLLI,
  RANDOM_TLLI,
  CURRENT_TLLI,
  OLD_TLLI,
  INVALID_TLLI
} T_TLLI_TYPE;


/*
 * Test environment (simulator) vs. Implementation environment.
 */
typedef enum
{
  TEST_ENV,
  IMPL_ENV
} T_ENVIRONMENT;


typedef UBYTE       T_BIT_INT;
typedef T_BIT_INT   T_BIT;


typedef enum
{
  SERVICE_KERN,
  SERVICE_TX,
  SERVICE_RX,
  SERVICE_RDY,
  SERVICE_SYNC,
  NO_SERVICE
} T_SERVICE;

typedef enum
{
  PRIM_DATA,
  PRIM_UNITDATA,
  NO_PRIM
} T_PRIM_TYPE;

typedef enum
{
  GMM_NO_RAU,
  GMM_RAU,
  GMM_PERIODIC_RAU
} T_RAU_TYPE;

typedef enum
{
  GU_UPDATE_NEEDED,
  GU_UPDATE_NOT_NEEDED
} T_GU_UPDATE_STATE;

/*
 * T_ATTACH_CAP is sruct, which defines the way to attach
 */
/* 
 * GMMREG_AT_NOT_KNOWN is in addition to the values given in 
 * gmmreg to indicate that no gmmreg_attach_req is received yet
 * this case is only after power on 
 */
#define GMMREG_AT_NOT_KNOWN    0
/*
 * GSIM is no longer available, so the Update defines are done here
 * These values defined in 11.11
 */
#define GU1_UPDATED        0
#define GU2_NOT_UPDATED      1
#define GU3_PLMN_NOT_ALLOWED    2
#define GU3_ROAMING_NOT_ALLOWED  3  /* RA not allowed */
/*
 * These values defined in 11.11 (original in RR.SAP in old GSM code
 */
#define CKSN_RES                       0x7         /* reserved                       */
#define CKSN_NOT_PRES                  0xff        /* not present                    */

/*
 * MAX_LIST_OF_FORBIDDEN_PLMNS_FOR_GPRS_SERVICE defines the max value of PLMNs
 */
#define MAX_LIST_OF_FORBIDDEN_PLMNS_FOR_GPRS_SERVICE 1

/*
 * cu_state stores the status of cell update. if llgmm_trigger_req has been 
 * sent with cause cell_update cu_state is set to remember the state when 
 * sending gmmrr_cell_res to GRR
 */
#define CU_NOT_REQUESTED  0
#define CU_REQUESTED      1
#define CU_CELL_RES_SENT  2
#define CU_REQUESTED_CELL_RES_SENT  3 /* if periodic rau accept has been received 
                                          * CU has to be sent although no cell had changed
                                          */

typedef struct /* T_GMM_STATE */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_GMM_STATE;

typedef struct /* T_GMM_STATES */
{
  /*
   * MM State
   */
  T_GMM_STATE                mm;           /* mm state          */
  /* 
   * Cell Update State
   *
   * This variable remembers whether the MS is searching for a PLMN
   * TRUE in the timespan MMGMM_NET_REQ to MMGMM_NREG_IND/MMGMM_PLMN_IND. 
   * This variable may be used to delay e.g. periodic updated until 
   * the network search has ended. 
   */
  T_GMM_STATE                cu;           /* cell update state */
  /*
   * Update states
   */
  T_GMM_STATE                gu;           /* GMM update state */
  
} T_GMM_STATES;

/*
 *    !!!  ATTENTION  !!!
 *    This struct will be written in one block to the FFS.
 *    So the position of the variables can not be changed!
 */
typedef struct 
{                                 /* default values */
  U8   cipher_on;                  
} T_GMM_FFS;

typedef struct
{
  UBYTE  cipher_on;
  UBYTE  nmo;

#ifdef REL99
  UBYTE sgsnr_flag; /* TCS 4.0 */
  UBYTE cell_notification; /* TCS 4.0 */
#endif

  BOOL   preuse_off;
} T_CONFIG;



typedef struct
{
  /*
   * GMM has to switch on GRR after mmgmm_activate_ind only the first time
   */
  UBYTE grr_state;
  /* 
   * attach_type as defined in GMMREG_ATTACH_REQ
   * as attach_type!type_of_attach with additional value 
   * GMMREG_AT_NOT_KNOWN = 4
   * MMI given attach type like mobile class.
   */
  UBYTE attach_type  ;

  /*
   * attach_type set in attach procedure only
   */

  UBYTE attach_proc_type;
  /*
   * update_proc__type set in rau procedure only
   */

  UBYTE update_proc_type;

  /*
   * This parameter is needed for for SIM to know if GMM attached 
   * one time succsessful after PO to update the ME 
   * see GSM 03.60 ch. 13.4 
   */
  
  UBYTE attach_acc_after_po;
  /* 
   * result type includes the result attach typeof the
   * last RAU or ATTACH procedure
   */
  UBYTE result_type;
  /* T_plmn_mt_caps plmn_mt_caps; */
  UBYTE sms_radio_priority_level;
  UBYTE old_net_mode;

  UBYTE mobile_class;
  UBYTE netIII_attach;
  /*
   * dependend on the attach direction the appropriate parameter is set
   */
  BOOL  gmmreg;
#ifndef GMM_TCS4
  BOOL  gmmsm;
#endif
  /*
   * to sent gmmrr_attache_finished only once i store the state
   */
  BOOL gmmrr_attach_finished_sent;
  /*
   * to sent llgmm_resume only once i store the state
   */
  UBYTE llc_state;

  /* BOOL  normal_service; */

  T_RAU_TYPE rau_initiated;
  BOOL attempting_to_update_mm;

  /*
   * service mode is given by MMI
   */
  UBYTE service_mode;
  /* 
   * attach complete is used to remember whether attach_complete has to
   * be sent again or never HACK: some networks accept only attach complete
   * with old TLLI
   */ 
  BOOL  attach_complete;
  /*
   * GSM 04.08: If for the last attempt to update the registration of the location area a MM 
   * specific procedure was performed, the value of the update type IE in the 
   * ROUTING AREA UPDATE REQUEST message shall indicate "combined RA/LA updating 
   * with IMSI attach". 
   */
  BOOL mm_lau_attempted;
  /*
   * GMM has to remember that state was attempting to UPDATE also if LAU 
   * has been performed afer aac>5
   * see R&S test case 44.2.3.2.7
   */
  BOOL enter_attempting_to_update_after_lau;
  /*
   * network_selection_mode is given by GMMREG_PLMN_MODE_REQ and switches 
   * between manual and automatic network selection mode (cops=1,2,"... or 
   * cops=?
   */
  UBYTE network_selection_mode;

  /* 
   * In case the user performed a manual network selection, we remember the 
   * user requested PLMN in this variable.
   */
  T_plmn plmn_requested;

  BOOL grr_via_llc_suspended;

  T_plmn list_of_forbidden_plmns_for_gprs_service[MAX_LIST_OF_FORBIDDEN_PLMNS_FOR_GPRS_SERVICE];

  T_routing_area_identification     rai_accepted;

  /*
   *in case of switching ms_classes GRR has to be informed after detach/attach proceudres
   */
  BOOL mobile_class_changed;

  /*
   * if RAU or attach procedure is interupted by GMMRR_CR_IND GMM stores the timer value
   * to be able to restart the timer if rau has not changed.
   */
  T_TIME  t3310_value;

#ifdef REL99
  /*
   * PDP context status used in R99 RAU request message is stored here. Ref 10.5.7.1 of 24.008
   */
  USHORT  pdp_context_status; /* TCS 4.0 */
#endif


} T_ATTACH_CAP;

typedef struct
{
  /* detach_type as defined in GMMREG_DETACH_REQ
   * as attach_type!type_of_attach with additional value 
   */
  UBYTE detach_type  ;  
  /*
   * dependend on the attach direction the appropriate parameter is set
   */
  BOOL  gmmreg;
  BOOL  network;
  BOOL  normal_service;
  /*
   * GMM has to remember the errr cause if 
   * ( !(48 <= cause && cause <= 63) )
   * 'Retry upon entry into a new cell' occures.
   * GPRS attach shall be
   * performed when a new cell is entered.
   */
  USHORT error_cause;
  /*
   * whenn GMM locally detaches the MS it enters substate suspending
   * to remember what to do next the folloing parameter has been introduced
   */
  T_LOCAL_DETACH_PROC  local_detach_open_proc;
} T_DETACH_CAP;

typedef struct
{
  /*
   * The purpose of the A&C reference number information element is to indicate
   * to the network in the AUTHENTICATION AND CIPHERING RESPONSE message which 
   * AUTHENTICATION AND CIPHERING REQUEST message the MS is replying to.
   * Reference : [1] section 10.5.5.19
   */
  UBYTE                        a_c_reference_number;

  /* SIM interface got modified to support identifiers of requests sent to
   * SIM. This can be used to handle the above mentioned "multiple (two)
   * authentication req messages" problem. The following variable replaces
   * the outstanding_count as a slightly different approach is used. This
   * variable contains the req_id in the last sent SIM_AUTHENTICATION_REQ.
   * On receiving SIM_AUTHENTICATION_CNF, response is sent to network only
   * if req_id matches the value in this variable.*/
  UBYTE                       last_auth_req_id;

  UBYTE                       kc[MAX_KC];          
  UBYTE                       cksn;
  UBYTE                       ciphering_algorithm;
  UBYTE                        imeisv_requested;
} T_AUTH_CAP;

/*
 * procedure collision between detach and others
 */
typedef struct
{
  BOOL  rau;
} T_WHILST_DETACH;

/*
 * cell id to have always the current state
 */
typedef struct
{
  T_plmn  plmn;
  UBYTE   rac;
  USHORT   lac;
  USHORT   cid;
} T_CELL_ID;

typedef struct
{
  BOOL           gmmrr_cell_ind_received; 
  T_cell_env     env;
  U8             gmm_status;
  U8             mm_status;
  U8             net_mode;
#ifdef GMM_TCS4
  U8             rt; /* TCS 4.0 */
#endif


  U32           t3212_val;

#ifdef REL99
  /*
   * For R97-R99 compatibility
   */
  U8                       sgsnr_flag;  /* TCS 4.0 */
#endif

} T_SIG_CELL_INFO;

typedef struct
{
  T_plmn                    plmn;                     /*<  0:  8> plmn identification                                */
  U16                       lac;                      /*<  8:  2> location area code                                 */
  U16                       cid;                      /*< 10:  2> cell id                                            */
  U32                       t3212_val;                /*< 12:  4> value of T3212                                     */
  U8                        status;                   /*< 16:  1> Activation status                                  */
  U8                        gprs_indicator;           /*< 17:  1> GPRS indicator                                     */
} T_MM_CELL_INFO;

/*
 * Includes the several TLLIS 
 */
  
typedef struct
{
  ULONG old;
  ULONG current;
  T_TLLI_TYPE current_type;
} T_TLLI;

/*
 * Includes the several PTMSIs 
 * current PTMSI is the PTMSI given by the network itself or read from SIM
 * new_grr PTMSI is the GRR-assigned PTMSI. Sometimes this GRR-PTMSI has to be set to
 * invalid although the PTMSI from the mobile is still available, i.e. in a re-attach situation.
 * oldold is the second old one, if in concurrent TBF more PTMSIs are exxhanged than 
 * TLLIs are received.
 */
typedef struct
{
  ULONG old;
  ULONG new_grr;
  ULONG current;
} T_PTMSI;


typedef struct
{
  BOOL  available;      
  ULONG value;
} T_PTMSI_SIGNATURE;

typedef struct /* T_KERN_DATA */
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
   *  GMM KERN STATES
   */
  T_GMM_STATES                states;
  /*
   * attach attempt counter
   */
  UBYTE                       aac;
  /*
   *   MM update status
   */
  UBYTE                       ustate;

  /*
   * defines the attach capabitlities
   */
  T_ATTACH_CAP                attach_cap;

  T_DETACH_CAP                detach_cap;

  /*
   * defines the authentication capabitlities
   */
  T_AUTH_CAP                  auth_cap;
  /*
   * collects all procedures which interupted teh detach procedure
   */
  T_WHILST_DETACH             whilst_detach;
  /*
   * Cell information
   */
  T_SIG_CELL_INFO             sig_cell_info;
  /*
   * Cell ID to inform the User
   */
  T_CELL_ID                   cell_id;
  /* 
   * GPRS inicator 
   */
  USHORT                      gprs_indicator;
  
  /*
   * Old Cell information to know when ra ic crossed
   */
  T_SIG_CELL_INFO              old_sig_cell_info;
  T_cell_env                   mm_cell_env;

  /*
   * counter for timers
   */
  UBYTE                        ct3310;
  UBYTE                        ct3321;
  /*
   * timer values
   */
  ULONG                        t3310_val;
  ULONG                        t3311_val;
  ULONG                        t3321_val;
  ULONG                        t3302_val;

  BOOL                         timeout_t3312;
  UBYTE                         sim_op_mode;
  
  BOOL                        gmmrr_resume_sent;
  UBYTE                       suspension_type;
  BOOL                        local_detached;
} T_KERN_DATA;


typedef struct /* T_TX_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_TX_DATA;


typedef struct /* T_RX_DATA */
{
  UBYTE                       state;
  /*
   * Required for state traces.
   */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_RX_DATA;

typedef struct /* T_RDY_DATA */
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
   * timer values
   */
  ULONG                       t3312_val;
  ULONG                       t3314_val;
  BOOL t3312_deactivated;
  BOOL                        attempting_to_update;
  ULONG                       timer_value[TIMER_MAX];

} T_RDY_DATA;

typedef struct /* T_SYNC_DATA */
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
   * timer values
   */
  T_cell_info                  grr_cell_info;
  T_SIG_CELL_INFO              sig_cell_info;
  T_MM_CELL_INFO               mm_cell_info;

} T_SYNC_DATA;

typedef struct /* T_GMM_DATA */
{
  /*
   * GMM layer parameters
   */
  UBYTE                       version;
  
#ifdef TRACE_FUNC
#ifdef IDENTATION
  UBYTE deep;
#endif
#endif
  BOOL anite;
  /*
   * counter for random value generator
   */
  UBYTE  rand_counter;
  /* 
   *  indicates whether ciphering is on in GMM or not 
   */
  BOOL                        cipher;      

  /*
   *  TLLI
   */
  T_TLLI tlli;

  USHORT                            acc_contr_class;
  BOOL                              sim_gprs_invalid;

  UBYTE                             gu;

  T_PTMSI                           ptmsi;

  ULONG                             tmsi;
  T_PTMSI_SIGNATURE                 ptmsi_signature;
  T_imsi                            imsi;

  T_drx_parameter                   drx_parameter;
  /*
   * defines cusomer specific options
   * either given be at%cgmm or CONFIG string
   */
  T_CONFIG                    config;

#ifdef REL99
  /*
   * For storing cell notification information
   */
  UBYTE                       cell_notification; /* TCS 4.0 */
#endif

 
  /*
   * Service data structures.
   *
   * Services with multiple incarnations require an array of structures
   * named xxx_base[] with xxx = service abbrevation, and additionally a
   * pointer named *xxx, which will be accessed instead of xxx_base.
   * 
   * Services with only one incarnation just have to declare one structure
   * named xxx (no need for xxx_base[] and *xxx).
   * 
   * The differentiation between the two access possibilites is made with
   * the defines of the service names above (LLC_SERVICE_XXX).
   */
  T_KERN_DATA     kern;
  T_TX_DATA       tx;
  T_RX_DATA       rx;
  T_RDY_DATA      rdy;
  T_SYNC_DATA     sync;
} T_GMM_DATA;

/*==== EXPORT =====================================================*/

/*
 * Entity data base
 */
#ifdef GMM_PEI_C
       T_GMM_DATA gmm_data_base, *gmm_data;
#else
EXTERN T_GMM_DATA gmm_data_base, *gmm_data;
#endif

#define ENTITY_DATA           gmm_data

#define pei_create              gmm_pei_create
/*
 * Communication handles (see also GMM_PEI.C)
 */
#define hCommSMS          gmm_hCommSMS
#define hCommSM            gmm_hCommSM
#define hCommGRLC          gmm_hCommGRLC /* TCS 2.1 */
#define hCommGRR           gmm_hCommGRR
#define hCommLLC           gmm_hCommLLC
#define hCommSIM           gmm_hCommSIM
#define hCommMM            gmm_hCommMM
#define hCommMMI           gmm_hCommMMI
#define hCommGMM           gmm_hCommGMM
#ifdef GMM_TCS4
#define hCommUPM           gmm_hCommUPM
#endif

#ifdef GMM_PEI_C
       T_HANDLE hCommSMS    = VSI_ERROR;
       T_HANDLE hCommSM      = VSI_ERROR;
       T_HANDLE hCommGRLC    = VSI_ERROR; /* TCS 2.1 */
       T_HANDLE hCommGRR     = VSI_ERROR;
       T_HANDLE hCommLLC     = VSI_ERROR;
       T_HANDLE hCommSIM     = VSI_ERROR;
       T_HANDLE hCommMM      = VSI_ERROR;
       T_HANDLE hCommMMI     = VSI_ERROR;
       T_HANDLE hCommGMM     = VSI_ERROR;
       
       T_HANDLE GMM_handle;
#ifdef GMM_TCS4
       T_HANDLE hCommUPM     = VSI_ERROR;
#endif
#else
EXTERN T_HANDLE hCommSMS;
EXTERN T_HANDLE hCommSM;
EXTERN T_HANDLE hCommGRLC; /* TCS 2.1 */
EXTERN T_HANDLE hCommGRR;
EXTERN T_HANDLE hCommLLC;
EXTERN T_HANDLE hCommSIM;
EXTERN T_HANDLE hCommMM;
EXTERN T_HANDLE hCommMMI;
EXTERN T_HANDLE hCommGMM;

#ifdef GMM_TCS4
EXTERN T_HANDLE hCommUPM;
#endif
EXTERN T_HANDLE GMM_handle;
#endif /* GMM_PEI_C */


/*
 * If all entities are linked into one module this definitions
 * prefixes the global data with the enity name
 */
 
#ifdef OPTION_MULTITHREAD
#define _decodedMsg   _ENTITY_PREFIXED(_decodedMsg)
#endif

#ifdef GMM_PEI_C
GLOBAL UBYTE      _decodedMsg [MAX_MSTRUCT_LEN_GMM];
#else
EXTERN UBYTE      _decodedMsg [MAX_MSTRUCT_LEN_GMM];
#endif /* GMM_PEI_C */

#endif /* GMM_H */
