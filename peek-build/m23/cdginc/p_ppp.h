/*
+--------------------------------------------------------------------------+
| PROJECT : PROTOCOL STACK                                                 |
| FILE    : p_ppp.h                                                        |
| SOURCE  : "__out__\LNX_TI_cgt2_70\rel_n5_camera_micron\cdginc\condat\ms\doc\sap\ppp.pdf"                 |
| LastModified : "2002-10-21"                                              |
| IdAndVersion : "8441.116.99.016"                                         |
| SrcFileTime  : "Thu Nov 29 09:51:00 2007"                                |
| Generated by CCDGEN_2.5.5A on Wed Mar 11 09:58:58 2009                   |
|           !!DO NOT MODIFY!!DO NOT MODIFY!!DO NOT MODIFY!!                |
+--------------------------------------------------------------------------+
*/

/* PRAGMAS
 * PREFIX                 : NONE
 * COMPATIBILITY_DEFINES  : NO (require PREFIX)
 * ALWAYS_ENUM_IN_VAL_FILE: NO
 * ENABLE_GROUP: NO
 * CAPITALIZE_TYPENAME: NO
 */


#ifndef P_PPP_H
#define P_PPP_H


#define CDG_ENTER__P_PPP_H

#define CDG_ENTER__FILENAME _P_PPP_H
#define CDG_ENTER__P_PPP_H__FILE_TYPE CDGINC
#define CDG_ENTER__P_PPP_H__LAST_MODIFIED _2002_10_21
#define CDG_ENTER__P_PPP_H__ID_AND_VERSION _8441_116_99_016

#define CDG_ENTER__P_PPP_H__SRC_FILE_TIME _Thu_Nov_29_09_51_00_2007

#include "CDG_ENTER.h"

#undef CDG_ENTER__P_PPP_H

#undef CDG_ENTER__FILENAME


#include "p_ppp.val"

#ifndef __T_peer_channel__
#define __T_peer_channel__
/*
 * PPP-peer communication channel name
 * CCDGEN:WriteStruct_Count==2323
 */
typedef struct
{
  U8                        peer_entity[6];           /*<  0:  6> PPP-peer communication channel name value          */
  U8                        _align0;                  /*<  6:  1> alignment                                          */
  U8                        _align1;                  /*<  7:  1> alignment                                          */
} T_peer_channel;
#endif

#ifndef __T_protocol_channel__
#define __T_protocol_channel__
/*
 * network protocol communication channel name
 * CCDGEN:WriteStruct_Count==2324
 */
typedef struct
{
  U8                        protocol_entity[6];       /*<  0:  6> network protocol communication channel name value  */
  U8                        _align0;                  /*<  6:  1> alignment                                          */
  U8                        _align1;                  /*<  7:  1> alignment                                          */
} T_protocol_channel;
#endif

#ifndef __T_login__
#define __T_login__
/*
 * authentication values
 * CCDGEN:WriteStruct_Count==2325
 */
typedef struct
{
  U8                        name_len;                 /*<  0:  1> length of name                                     */
  U8                        name[PPP_LOGIN_NAME_LENGTH]; /*<  1: 33> login name                                         */
  U8                        password_len;             /*< 34:  1> length of password                                 */
  U8                        password[PPP_PASSWORD_LENGTH]; /*< 35: 25> login password                                     */
} T_login;
#endif


/*
 * End of substructure section, begin of primitive definition section
 */

#ifndef __T_PPP_ESTABLISH_REQ__
#define __T_PPP_ESTABLISH_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==2328
 */
typedef struct
{
  U8                        mode;                     /*<  0:  1> working mode                                       */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U16                       mru;                      /*<  2:  2> maximum receive unit                               */
  U8                        ap;                       /*<  4:  1> authentication protocol                            */
  U8                        _align1;                  /*<  5:  1> alignment                                          */
  U8                        _align2;                  /*<  6:  1> alignment                                          */
  U8                        _align3;                  /*<  7:  1> alignment                                          */
  T_login                   login;                    /*<  8: 60> authentication values                              */
  U32                       accm;                     /*< 68:  4> async control character map                        */
  U8                        rt;                       /*< 72:  1> restart timer                                      */
  U8                        mc;                       /*< 73:  1> max configure                                      */
  U8                        mt;                       /*< 74:  1> max terminate                                      */
  U8                        mf;                       /*< 75:  1> max failure                                        */
  U8                        ppp_hc;                   /*< 76:  1> header compression of PPP connection               */
  U8                        _align4;                  /*< 77:  1> alignment                                          */
  U8                        _align5;                  /*< 78:  1> alignment                                          */
  U8                        _align6;                  /*< 79:  1> alignment                                          */
  U32                       ip;                       /*< 80:  4> Internet Protocol address                          */
  U32                       dns1;                     /*< 84:  4> primary DNS server address                         */
  U32                       dns2;                     /*< 88:  4> secondary DNS server address                       */
  T_peer_channel            peer_channel;             /*< 92:  8> PPP-peer communication channel name                */
  T_protocol_channel        protocol_channel;         /*<100:  8> network protocol communication channel name        */
  U8                        peer_direction;           /*<108:  1> Direction for the Peer Layer                       */
  U8                        prot_direction;           /*<109:  1> Direction for the Protocol Layer                   */
  U8                        _align7;                  /*<110:  1> alignment                                          */
  U8                        _align8;                  /*<111:  1> alignment                                          */
  U32                       peer_link_id;             /*<112:  4> Link Id to Peer Layer                              */
  U32                       prot_link_id;             /*<116:  4> Link Id to Protocol Layer                          */
} T_PPP_ESTABLISH_REQ;
#endif

#ifndef __T_PPP_ESTABLISH_CNF__
#define __T_PPP_ESTABLISH_CNF__
/*
 * 
 * CCDGEN:WriteStruct_Count==2329
 */
typedef struct
{
  U16                       mru;                      /*<  0:  2> maximum receive unit                               */
  U8                        ppp_hc;                   /*<  2:  1> header compression of PPP connection               */
  U8                        msid;                     /*<  3:  1> max slot identifier                                */
  U32                       ip;                       /*<  4:  4> Internet Protocol address                          */
  U32                       dns1;                     /*<  8:  4> primary DNS server address                         */
  U32                       dns2;                     /*< 12:  4> secondary DNS server address                       */
} T_PPP_ESTABLISH_CNF;
#endif

#ifndef __T_PPP_TERMINATE_REQ__
#define __T_PPP_TERMINATE_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==2330
 */
typedef struct
{
  U8                        lower_layer;              /*<  0:  1> status of lower layer                              */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
} T_PPP_TERMINATE_REQ;
#endif

#ifndef __T_PPP_TERMINATE_IND__
#define __T_PPP_TERMINATE_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2331
 */
typedef struct
{
  U16                       ppp_cause;                /*<  0:  2> PPP error cause                                    */
  U8                        _align0;                  /*<  2:  1> alignment                                          */
  U8                        _align1;                  /*<  3:  1> alignment                                          */
} T_PPP_TERMINATE_IND;
#endif

#ifndef __T_PPP_PDP_ACTIVATE_IND__
#define __T_PPP_PDP_ACTIVATE_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2332
 */
typedef struct
{
  U8                        ppp_hc;                   /*<  0:  1> header compression of PPP connection               */
  U8                        msid;                     /*<  1:  1> max slot identifier                                */
  U8                        _align0;                  /*<  2:  1> alignment                                          */
  U8                        _align1;                  /*<  3:  1> alignment                                          */
  T_sdu                     sdu;                      /*<  4: ? > list of protocol configuration options             */
} T_PPP_PDP_ACTIVATE_IND;
#endif

#ifndef __T_PPP_PDP_ACTIVATE_RES__
#define __T_PPP_PDP_ACTIVATE_RES__
/*
 * 
 * CCDGEN:WriteStruct_Count==2333
 */
typedef struct
{
  U8                        ppp_hc;                   /*<  0:  1> header compression of PPP connection               */
  U8                        msid;                     /*<  1:  1> max slot identifier                                */
  U8                        _align0;                  /*<  2:  1> alignment                                          */
  U8                        _align1;                  /*<  3:  1> alignment                                          */
  U32                       ip;                       /*<  4:  4> Internet Protocol address                          */
  T_sdu                     sdu;                      /*<  8: ? > list of protocol configuration options             */
} T_PPP_PDP_ACTIVATE_RES;
#endif

#ifndef __T_PPP_PDP_ACTIVATE_REJ__
#define __T_PPP_PDP_ACTIVATE_REJ__
/*
 * 
 * CCDGEN:WriteStruct_Count==2334
 */
typedef struct
{
  U16                       ppp_cause;                /*<  0:  2> PPP error cause                                    */
  U8                        _align0;                  /*<  2:  1> alignment                                          */
  U8                        _align1;                  /*<  3:  1> alignment                                          */
} T_PPP_PDP_ACTIVATE_REJ;
#endif

#ifndef __T_PPP_MODIFICATION_REQ__
#define __T_PPP_MODIFICATION_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==2335
 */
typedef struct
{
  U8                        ppp_hc;                   /*<  0:  1> header compression of PPP connection               */
  U8                        msid;                     /*<  1:  1> max slot identifier                                */
  U8                        _align0;                  /*<  2:  1> alignment                                          */
  U8                        _align1;                  /*<  3:  1> alignment                                          */
} T_PPP_MODIFICATION_REQ;
#endif

#ifndef __T_PPP_MODIFICATION_CNF__
#define __T_PPP_MODIFICATION_CNF__
/*
 * 
 * CCDGEN:WriteStruct_Count==2336
 */
typedef struct
{
  U8                        ppp_hc;                   /*<  0:  1> header compression of PPP connection               */
  U8                        msid;                     /*<  1:  1> max slot identifier                                */
  U8                        _align0;                  /*<  2:  1> alignment                                          */
  U8                        _align1;                  /*<  3:  1> alignment                                          */
} T_PPP_MODIFICATION_CNF;
#endif

#ifndef __T_PPP_DTI_CONNECTED_IND__
#define __T_PPP_DTI_CONNECTED_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2337
 */
typedef struct
{
  U8                        connected_direction;      /*<  0:  1> DTI direction                                      */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
} T_PPP_DTI_CONNECTED_IND;
#endif

#ifndef __T_NEW_POWER_STATUS__
#define __T_NEW_POWER_STATUS__
/*
 * 
 * CCDGEN:WriteStruct_Count==2338
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_NEW_POWER_STATUS;
#endif


#include "CDG_LEAVE.h"


#endif
