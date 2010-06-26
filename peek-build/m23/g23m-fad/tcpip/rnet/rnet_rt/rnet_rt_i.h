/**
 * @file  rnet_rt_i.h
 *
 * Internal definitions for RNET_RT.
 *
 * @author  Jose Yp-Tcha (j-yp-tcha@ti.com)
 * @version 0.1
 */

/*
 * $Id: rnet_rt_i.h,v 1.6 2002/10/30 15:23:34 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date         Author          Modification
 *  -------------------------------------------------------------------
 *  3/19/2002  Jose Yp-Tcha (j-yp-tcha@ti.com)    Create.
 *  3/29/2002 Regis Feneon      NexGenIP definitions
 *  10/28/2002 Regis Feneon      added RNET_RT_ETH_SUPPORT
 *
 * (C) Copyright 2002 by TI, All Rights Reserved
 */

#include "rnet_cfg.h"

#ifdef RNET_CFG_REAL_TRANSPORT

#ifndef __RNET_RT_I_H_
#define __RNET_RT_I_H_

#define NG_NO_BSDSOCK_DECLS

#include "rv_general.h"
#include "rvf_api.h"
#include "rvm_gen.h"
#include "rvm_use_id_list.h"
#include "rvm_ext_priorities.h"

#include "rnet_rt_cfg.h"
#include "rnet_rt_api.h"
#include "rnet_rt_message.h"

/** NexGenOS/NexGenIP definitions */
#include "ngos.h"
#include "ngip.h"
#include "ngudp.h"
#include "ngtcp.h"
#include "ngsockio.h"
#include "ngresolv.h"

#ifdef _WINDOWS
#include "ngeth.h"
#endif
#ifdef RNET_RT_ATP_SUPPORT
#include "rnet_rt_atp_i.h"
#endif
#ifdef RNET_RT_DTI_SUPPORT
#include "rnet_rt_dti_i.h"
#endif

/** size of debug output buffer */
#define RNET_RT_NGIP_DEBUG_MAX 128

/** size of host info data buffer */
#define RNET_RT_NGIP_GETHOSTINFO_BUFMAX 192

/* debug informations */
/* #define RNET_RT_NGIP_DEBUG_ENABLE (is defined in the Makefile) */

/** RNET socket control block */
typedef struct {
  /** NexGenIP control block */
  NGsock sock;
  /** return path */
  T_RV_RETURN_PATH return_path;
  /** flags */
  int flags;
#define RNET_RT_SOCKF_NOTIFY_RECV 0x0001
#define RNET_RT_SOCKF_NOTIFY_SEND 0x0002
#define RNET_RT_SOCKF_NOTIFY_CONNECT 0x0004
#define RNET_RT_SOCKF_NOTIFY_CLOSED 0x0008
  /** user (application) data */
  void *user_data;
} T_RNET_RT_SOCK;

/**
 * The Control Block buffer of RNET_RT, which gathers all 'Global variables'
 * used by RNET_RT instance.
 */
typedef struct {
  T_RVM_RETURN (*error_ft)( T_RVM_NAME swe_name,
                          T_RVM_RETURN error_cause,
                          T_RVM_ERROR_TYPE error_type,
                          T_RVM_STRING error_msg);
  T_RVF_MB_ID mb_id;
/** SWE address */
  T_RVF_ADDR_ID addr_id;
/** global mutex for access to SWE */
  T_RVF_MUTEX mutex;
/** pointer to pool of network buffers */
  T_RVF_BUFFER *buf_net;
/** table of socket control blocks */
  T_RNET_RT_SOCK socktable[RNET_RT_SOCK_MAX];
/** table of TCP control blocks */
  NGtcpcb tcbtable[RNET_RT_SOCK_MAX];
/** resolver queries */
  NGslvpquer resolvquery[RNET_RT_RESOLV_QUERY_MAX];
/** resolver cache entries */
  NGslvcaent resolvcache[RNET_RT_RESOLV_CACHE_MAX];
#ifdef RNET_RT_LOOPBACK_SUPPORT
/** loopback interface */
  NGifnet ifnet_lo;
#endif
#ifdef RNET_RT_ETH_SUPPORT
/** ARP table */
  NGarpent arptable[RNET_RT_ARP_MAX];
/** ethernet interface */
  NGethifnet ifnet_eth;
#endif
#ifdef RNET_RT_ATP_SUPPORT
/** ATP point-to-point interface */
  T_RNET_RT_ATP_IFNET ifnet_atp;
#endif
#ifdef RNET_RT_DTI_SUPPORT
/** DTI point-to-point interface */
  T_RNET_RT_DTI_IFNET ifnet_dti;
#endif
/** buffer for debug output */
#ifdef RNET_RT_NGIP_DEBUG_ENABLE
  char buf_debug[RNET_RT_NGIP_DEBUG_MAX];
#endif
/** system clock emulation */
  unsigned long clock;
} T_RNET_RT_ENV_CTRL_BLK;

/** External ref "global variables" structure. */
extern T_RNET_RT_ENV_CTRL_BLK *rnet_rt_env_ctrl_blk_p;

/** get_host_info data */
typedef struct {
  T_RV_RETURN_PATH return_path;
  void *user_data;
  NGubyte tmpbuf[RNET_RT_NGIP_GETHOSTINFO_BUFMAX];
  NGhostent hostent;
} T_RNET_RT_GETHOSTINFO;

/** RNET interface driver message handling */
#define NG_RNETIFO_HANDLE_MSG 0x0210

/** Macro used for tracing RNET messages. */
#define RNET_RT_SEND_TRACE(string, trace_level) \
  rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, RNET_RT_USE_ID)

/**
 * Interface functions between NexGenIP and Riviera
 */
/*@{*/
int rnet_rt_ngip_init( void *bufpool_ptr, int bufpool_size, int buf_hdr_size,
  int buf_data_size, T_RNET_RT_SOCK *socktable, int sockmax, const NGcfgent *cfg);
void rnet_rt_ngip_input( NGbuf *bufp);
void rnet_rt_ngip_timer( void);
int rnet_rt_ngip_start( void);
void rnet_rt_ngip_stop( void);
T_RNET_RET rnet_rt_ngip_error( int err);
/*@}*/

#endif /* __RNET_RT_I_H_ */

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

