/* 
+------------------------------------------------------------------------------
|  File:       tcpip_utils.c
+------------------------------------------------------------------------------
|  Copyright 2003 Texas Instruments Berlin, AG 
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
|  Purpose :  Utility functions for GPF-based TCP/IP
+----------------------------------------------------------------------------- 
*/ 


#define TCPIP_UTILS_C

#define ENTITY_TCPIP

/*==== INCLUDES =============================================================*/

#include <string.h>             /* String functions, e. g. strncpy(). */
#include <stdio.h>

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "custom.h"
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "pei.h"        /* to get PEI interface */
#include "tools.h"      /* to get common tools */
#include "dti.h"        /* For DTI library definitions. */
#include "glob_defs.h"
#include "tcpip.h"      /* to get the global entity definitions */

/* RNET includes
 */
#include "rv_general.h"
#include "rnet_api.h"
#include "rnet_rt_env.h"
#include "rnet_message.h"
#include "rnet_rt_i.h"


/*==== Local prototypes =====================================================*/




/*==== Macros ===============================================================*/



/*==== Types ================================================================*/


/*==== Local data ===========================================================*/



/*==== Other public functions ===============================================*/

unsigned long tcpip_sim_fake_data(int socket, U16 len)
{
  static char tmp[100000] ;

/*   MALLOC(tmp, len) ; */
  *tmp = 0 ;
  if (len > 30)
  {
    sprintf(tmp, "data len 0x%0.4x for socket %-.2d\n", len, socket) ;
  }
  if (len > strlen(tmp))
  {
    memset(tmp + strlen(tmp), '0' + socket, len - strlen(tmp)) ;
  }
  return (unsigned long) tmp ;
}


void tcpip_dump_packet(char *title, U8 *data, U16 len)
{
  static char buf[80] ;
  int i = 0 ;

  strcpy(buf, "pkt dump from ") ;
  strcat(buf, title) ;
  strcat(buf, "\n    ") ;
  while (len-- > 0)
  {
    sprintf(buf + strlen(buf), " %02x", *data++) ;
    if (!(++i % 4))
    {
      if (!(i % 8))
      {
        TRACE_EVENT(buf) ;
        sprintf(buf, "    ") ;
      }
      else
      {
        sprintf(buf + strlen(buf), " ") ;
      }
    }
  }
  printf("%s", buf) ;
}

static char *inet_ntoa(U32 ipaddr)
{
  static char ipaddr_string[sizeof("000.000.000.000")] ;
  UBYTE *addrbyte_p ;        /* Pointer to single address bytes. */

  addrbyte_p = (UBYTE *) &ipaddr ;
  sprintf(ipaddr_string,"%u.%u.%u.%u",
          addrbyte_p[3], addrbyte_p[2], addrbyte_p[1], addrbyte_p[0]);
  return ipaddr_string ;
}



void tcpip_if_properties(NGifnet *netp)
{
  char *name ;
  U32 value ;

  ngIfGenCntl(netp, NG_CNTL_GET, NG_IFO_NAME, &name) ;
  TRACE_EVENT_P1("Interface name %s", name) ;

  ngIfGenCntl(netp, NG_CNTL_GET, NG_IFO_OUTQ_MAX, &value) ;
  TRACE_EVENT_P1("    NG_IFO_OUTQ_MAX = %d", value) ;

  ngIfGenCntl(netp, NG_CNTL_GET, NG_IFO_ADDR, &value) ;
  TRACE_EVENT_P1("    NG_IFO_ADDR = %s", inet_ntoa(ngNTOHL(value))) ;
  
  ngIfGenCntl(netp, NG_CNTL_GET, NG_IFO_NETMASK, &value) ;
  TRACE_EVENT_P1("    NG_IFO_NETMASK = 0x%x", ngNTOHL(value)) ;

  ngIfGenCntl(netp, NG_CNTL_GET, NG_IFO_DSTADDR, &value) ;
  TRACE_EVENT_P1("    NG_IFO_DSTADDR = %s", inet_ntoa(ngNTOHL(value))) ;

  ngIfGenCntl(netp, NG_CNTL_GET, NG_IFO_FLAGS, &value) ;
  TRACE_EVENT_P1("    NG_IFO_FLAGS = 0x%x", value) ;

  ngIfGenCntl(netp, NG_CNTL_GET, NG_IFO_DEV1, &value) ;
  TRACE_EVENT_P1("    NG_IFO_DEV1 = 0x%x", value) ;

  ngIfGenCntl(netp, NG_CNTL_GET, NG_IFO_DEV2, &value) ;
  TRACE_EVENT_P1("    NG_IFO_DEV2 = 0x%x", value) ;

  ngIfGenCntl(netp, NG_CNTL_GET, NG_IFO_DEVPTR1, &value) ;
  TRACE_EVENT_P1("    NG_IFO_DEVPTR1 = 0x%x", value) ;

  ngIfGenCntl(netp, NG_CNTL_GET, NG_IFO_DEVPTR2, &value) ;
  TRACE_EVENT_P1("    NG_IFO_DEVPTR2 = 0x%x", value) ;

  ngIfGenCntl(netp, NG_CNTL_GET, NG_IFO_PROMISC, &value) ;
  TRACE_EVENT_P1("    NG_IFO_PROMISC = 0x%x", value) ;

  ngIfGenCntl(netp, NG_CNTL_GET, NG_IFO_ALLMULTI, &value) ;
  TRACE_EVENT_P1("    NG_IFO_ALLMULTI = 0x%x", value) ;

  ngIfGenCntl(netp, NG_CNTL_GET, NG_IFO_MTU, &value) ;
  TRACE_EVENT_P1("    NG_IFO_MTU = %d", value) ;
}
