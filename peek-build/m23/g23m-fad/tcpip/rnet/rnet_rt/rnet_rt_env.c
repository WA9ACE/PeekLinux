/**
 * @file    rnet_rt_env.c
 *
 * Coding of the Riviera RNET Generic Functions,
 *
 * @author  Jose Yp-Tcha (j-yp-tcha@ti.com)
 * @version 0.1
 */

/*
 * $Id: rnet_rt_env.c,v 1.5 2002/10/30 15:23:34 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date         Author          Modification
 *  -------------------------------------------------------------------
 *  3/19/2002  Jose Yp-Tcha (j-yp-tcha@ti.com)    Create.
 *  3/29/2002 Regis Feneon      NexGenIP configuration
 *  5/6/2002    Regis Feneon  ATP driver support
 *  10/28/2002 Regis Feneon      added tests for RNET_RT_ETH_SUPPORT
 *
 * (C) Copyright 2002 by TI, All Rights Reserved
 */

#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#ifdef _WINDOWS
#include "windows.h"
#include "rnet_rt_ngwinnet_i.h"
#endif

#include "rnet_rt_i.h"
#include "rnet_rt_env.h"

#include "rvm_priorities.h"
#include "rnet_trace_i.h"

#ifdef _WINDOWS
#define BUF_HEADER_SIZE 16
#else
#define BUF_HEADER_SIZE 0
#endif
#define BUF_DATA_SIZE 1500


/* Buffer for TCPIP internal memory menagement */
static T_RNET_RT_ENV_CTRL_BLK rnet_data_buf;
static unsigned char tcpip_buf[RNET_RT_BUFPOOL_SIZE];


/**
 * Pointer on the structure gathering all the global variables
 * used by RNET instance.
 */
T_RNET_RT_ENV_CTRL_BLK *rnet_rt_env_ctrl_blk_p;

/* Type 4, temporary */
T_RVM_RETURN rnet_rt_temp_core (void);

/**
 * Called by the RV manager to learn
 * tut requirements in terms of memory, SWEs...
 *
 * @param swe_info  Pointer to the structure to fill
 *            containing infos related to the tut SWE.
 *
 * @return  RVM_OK
 */
T_RVM_RETURN rnet_rt_get_info(T_RVM_INFO_SWE * swe_info)
{
  RNET_RT_SEND_TRACE("RNET_RT: get_info called", RV_TRACE_LEVEL_DEBUG_LOW);

  /* The SWE is a Type 4 SWE */
  swe_info->swe_type = RVM_SWE_TYPE_4;

  /* Used for info */
  memcpy(swe_info->type_info.type4.swe_name, "RNET_RT", sizeof("RNET_RT"));
  /*
   * This is the real way to indentify a SWE.
   * Look in rvm_use_id_list.h and rvm_ext_use_id_list.h.
   */
  swe_info->type_info.type4.swe_use_id = RNET_RT_USE_ID;

  /* SWE info */
  swe_info->type_info.type4.stack_size = RNET_RT_STACK_SIZE;
  swe_info->type_info.type4.priority = RVM_RNET_RT_TASK_PRIORITY;
  swe_info->type_info.type4.version = BUILD_VERSION_NUMBER(0,1,0);

  /* Memory bank info */
  swe_info->type_info.type4.nb_mem_bank = 1;
  memcpy(swe_info->type_info.type4.mem_bank[0].bank_name,
    "RNET_RT_PRIM", sizeof("RNET_RT_PRIM"));
  swe_info->type_info.type4.mem_bank[0].initial_params.size =
    RNET_RT_MB_PRIM_SIZE;


  swe_info->type_info.type4.mem_bank[0].initial_params.watermark =
    RNET_RT_MB_PRIM_WATERMARK;

  /*
   * Linked SWE info.
   */
#ifdef RNET_RT_ATP_SUPPORT
  /* we require ATP SWE to run */
  swe_info->type_info.type4.linked_swe_id[0] = ATP_USE_ID;
  swe_info->type_info.type4.nb_linked_swe = 1;
#else
  swe_info->type_info.type4.nb_linked_swe = 0;
#endif

  /* Set the return path: NOT USED. */
  swe_info->type_info.type4.return_path.callback_func = NULL;
  swe_info->type_info.type4.return_path.addr_id = 0;

  /* Generic functions */
  swe_info->type_info.type4.set_info = rnet_rt_set_info;
  swe_info->type_info.type4.init = rnet_rt_init;
/*
  swe_info->type_info.type3.start = rnet_rt_start;
  swe_info->type_info.type3.handle_message = rnet_rt_handle_message;
  swe_info->type_info.type3.handle_timer = rnet_rt_handle_timer;
*/
  swe_info->type_info.type4.stop = rnet_rt_stop;
  swe_info->type_info.type4.kill = rnet_rt_kill;

  /* Type 4 specific generic functions */
  swe_info->type_info.type4.core = rnet_rt_temp_core;
  /* End of specific generic functions */

  return RVM_OK;
}

/**
 * Called by the RV manager to inform the tut SWE about
 * addr_id, return path, mb_id and error function.
 *
 * It is called only once.
 *
 * @param addr_id     Address ID of the TUT SWE.
 *              Used to send messages to the SWE.
 * @param return_path   Return path array of the linked SWEs.
 * @param bk_id_table   Array of memory bank ids allocated to the SWE.
 * @param call_back_error_ft Callback function to call in case of unrecoverable error.
 * @return  RVM_MEMORY_ERR ou RVM_OK.
 */
T_RVM_RETURN rnet_rt_set_info ( T_RVF_ADDR_ID addr_id,
              T_RV_RETURN_PATH return_path[],
              T_RVF_MB_ID bk_id_table[],
              T_RVM_CB_FUNC call_back_error_ft)
{
  /* Memory bank status (red, yellow, green). */

  RNET_RT_SEND_TRACE("RNET_RT: set_info called", RV_TRACE_LEVEL_DEBUG_LOW);

  /* Create instance gathering all the variable used by TUT instance */
  rnet_rt_env_ctrl_blk_p = &rnet_data_buf;
  
  /* Store the address ID. */
  rnet_rt_env_ctrl_blk_p->addr_id = addr_id;

  /* Store the pointer to the error function. */
  rnet_rt_env_ctrl_blk_p->error_ft = call_back_error_ft;

  /*
   * Store the mem bank id.
   * Memory bank ID (mb_id) can be retrieved later using rvf_get_mb_id function.
   */
  rnet_rt_env_ctrl_blk_p->mb_id = bk_id_table[0];

  return RVM_OK;
}

/**
 * Called by the RV manager to initialize the
 * RNET SWE before creating the task and calling tut_start.
 *
 * @return  RVM_OK
 */

#define CFGMAX 40
#define CFG( opt, arg) { cfg[i].cfg_option = (opt); cfg[i++].cfg_arg = (arg); }

T_RVM_RETURN rnet_rt_init( void)
{
  int i, init_err;
  NGcfgent cfg[CFGMAX];
#ifdef _WINDOWS
  /* windows specific settings */
  NGuint ipaddr, ipnetmask, ipgateway, ipdns1, ipdns2;
  char *winpcapdev, *domain;
  static char tmpbuf[128], tmpbuf2[128];
#endif

  RNET_RT_SEND_TRACE("RNET_RT: init called", RV_TRACE_LEVEL_DEBUG_LOW);

  /*
   * NexGenIP initialisation
   */

#ifdef _WINDOWS
  /* get windows settings */
  if( (GetEnvironmentVariable( "RNET_RT_WIN32_IPADDR", tmpbuf, sizeof( tmpbuf)) == 0) ||
      (ngInetATON( tmpbuf, &ipaddr) != NG_EOK)) {
    ipaddr = 0;
  }
  if( (GetEnvironmentVariable( "RNET_RT_WIN32_IPNETMASK", tmpbuf, sizeof( tmpbuf)) == 0) ||
      (ngInetATON( tmpbuf, &ipnetmask) != NG_EOK)) {
    ipnetmask = 0;
  }
  if( (GetEnvironmentVariable( "RNET_RT_WIN32_IPGATEWAY", tmpbuf, sizeof( tmpbuf)) == 0) ||
      (ngInetATON( tmpbuf, &ipgateway) != NG_EOK)) {
    ipgateway = 0;
  }
  if( (GetEnvironmentVariable( "RNET_RT_WIN32_IPDNS1", tmpbuf, sizeof( tmpbuf)) == 0) ||
      (ngInetATON( tmpbuf, &ipdns1) != NG_EOK)) {
    ipdns1 = 0;
  }
  if( (GetEnvironmentVariable( "RNET_RT_WIN32_IPDNS2", tmpbuf, sizeof( tmpbuf)) == 0) ||
      (ngInetATON( tmpbuf, &ipdns1) != NG_EOK)) {
    ipdns2 = 0;
  }
  if( GetEnvironmentVariable( "RNET_RT_WIN32_WINPCAPDEV", tmpbuf, sizeof( tmpbuf)) == 0) {
    winpcapdev = NULL;
  }
  else {
    winpcapdev = tmpbuf;
  }
  if( GetEnvironmentVariable( "RNET_RT_WIN32_DOMAIN", tmpbuf, sizeof( tmpbuf2)) == 0) {
    domain = NULL;
  }
  else {
    domain = tmpbuf2;
  }
#endif

  /* allocate buffer pool */
  rnet_rt_env_ctrl_blk_p->buf_net = &tcpip_buf;

  /* initialise mutex */
  if( rvf_initialize_mutex( &rnet_rt_env_ctrl_blk_p->mutex) != RV_OK) {
    RNET_RT_SEND_TRACE("RNET_RT: Cannot initialize mutex ",RV_TRACE_LEVEL_ERROR);
    return RVM_INTERNAL_ERR;
  }

  /*
   * Configuration Table
   */
  i = 0;
  /* TCP */
  CFG( NG_CFG_PROTOADD, NG_CFG_PTR( &ngProto_TCP));
  CFG( NG_TCPO_TCB_MAX, NG_CFG_INT( RNET_RT_SOCK_MAX));
  CFG( NG_TCPO_TCB_TABLE, NG_CFG_PTR( (/*(rnet_data *)*/ rnet_rt_env_ctrl_blk_p)->tcbtable));
  /* UDP */
  CFG( NG_CFG_PROTOADD, NG_CFG_PTR( &ngProto_UDP));
  /* IP */
  CFG( NG_CFG_PROTOADD, NG_CFG_PTR( &ngProto_IP));
#ifdef _WINDOWS
  if( ipgateway != 0) {
    CFG( NG_IPO_ROUTE_DEFAULT, NG_CFG_LNG( ipgateway));
  }
#endif
#ifdef RNET_RT_ETH_SUPPORT
  /* ARP */
  CFG( NG_CFG_PROTOADD, NG_CFG_PTR( &ngProto_ARP));
  CFG( NG_ARPO_MAX, NG_CFG_INT( RNET_RT_ARP_MAX));
  CFG( NG_ARPO_TABLE, NG_CFG_PTR( (/*(rnet_data *)*/ rnet_rt_env_ctrl_blk_p)->arptable));
  CFG( NG_ARPO_EXPIRE, NG_CFG_INT( 120));
#endif
  /* RESOLVER */
  CFG( NG_CFG_PROTOADD, NG_CFG_PTR( &ngProto_RESOLV));
  CFG( NG_RSLVO_QUERY_MAX, NG_CFG_INT( RNET_RT_RESOLV_QUERY_MAX));
  CFG( NG_RSLVO_QUERY, NG_CFG_PTR( (/*(rnet_data *)*/ rnet_rt_env_ctrl_blk_p)->resolvquery));
  CFG( NG_RSLVO_CACHE_MAX, NG_CFG_INT( RNET_RT_RESOLV_CACHE_MAX));
  CFG( NG_RSLVO_CACHE_ENT, NG_CFG_PTR( (/*(rnet_data *)*/ rnet_rt_env_ctrl_blk_p)->resolvcache));
  CFG( NG_RSLVO_TO, NG_CFG_INT( RNET_RT_RESOLV_TIMEOUT));
#ifdef _WINDOWS
  if( ipdns1 != 0) {
    CFG( NG_RSLVO_SERV1_IPADDR, NG_CFG_LNG( ipdns1));
  }
  if( ipdns2 != 0) {
    CFG( NG_RSLVO_SERV2_IPADDR, NG_CFG_LNG( ipdns2));
  }
  if( domain != NULL) {
    CFG( NG_RSLVO_DOMAIN, NG_CFG_PTR( domain));
  }
#endif
#ifdef RNET_RT_LOOPBACK_SUPPORT
  /* Loopback Interface */
  CFG( NG_CFG_IFADD, NG_CFG_PTR( &rnet_rt_env_ctrl_blk_p->ifnet_lo));
  CFG( NG_CFG_DRIVER, NG_CFG_PTR( &ngNetDrv_LOOPBACK));
  CFG( NG_IFO_NAME, NG_CFG_PTR( "lo0"));
#endif
#if defined(_WINDOWS) && defined(RNET_RT_ETH_SUPPORT)
  /* Ethernet interface */
  CFG( NG_CFG_IFADD, NG_CFG_PTR( &rnet_rt_env_ctrl_blk_p->ifnet_eth));
  CFG( NG_CFG_DRIVER, NG_CFG_PTR( &ngNetDrv_WIN32));
  CFG( NG_IFO_NAME, NG_CFG_PTR( "eth0"));
  CFG( NG_IFO_OUTQ_MAX, NG_CFG_INT( 16));
  if( ipaddr != 0) {
    CFG( NG_IFO_ADDR, NG_CFG_LNG( ipaddr));
  }
  if( ipnetmask != 0) {
    CFG( NG_IFO_NETMASK, NG_CFG_LNG( ipnetmask));
  }
  if( winpcapdev != NULL) {
    CFG( NG_IFO_DEVPTR1, NG_CFG_PTR( winpcapdev));
  }
#endif
#ifdef RNET_RT_ATP_SUPPORT
  /* ATP point-to-point Interface */
  CFG( NG_CFG_IFADD, NG_CFG_PTR( &rnet_rt_env_ctrl_blk_p->ifnet_atp));
  CFG( NG_CFG_DRIVER, NG_CFG_PTR( &rnet_rt_netdrv_atp));
  CFG( NG_IFO_NAME, NG_CFG_PTR( "ppp0"));
#endif
#ifdef RNET_RT_DTI_SUPPORT
  /* DTI (i. e. GMS or GPRS) point-to-point Interface */
  CFG( NG_CFG_IFADD, NG_CFG_PTR( &rnet_rt_env_ctrl_blk_p->ifnet_dti));
  CFG( NG_CFG_DRIVER, NG_CFG_PTR( &rnet_rt_netdrv_dti));
  CFG( NG_IFO_NAME, NG_CFG_PTR( "gsm0"));
#endif
  /* end of table */
  CFG( NG_CFG_END, 0);

  /* initialise protocols and network interfaces */
  init_err = rnet_rt_ngip_init( rnet_rt_env_ctrl_blk_p->buf_net,
                                RNET_RT_BUFPOOL_SIZE,
                                BUF_HEADER_SIZE,
                                BUF_DATA_SIZE,
                                rnet_rt_env_ctrl_blk_p->socktable,
                                RNET_RT_SOCK_MAX,
                                cfg);
  if( init_err != NG_EOK) {
    RNET_RT_SEND_TRACE("RNET_RT: Cannot initialize NexGenIP stack ",RV_TRACE_LEVEL_ERROR);
    return RVM_INTERNAL_ERR;
  }

#ifdef RNET_RT_NGIP_DEBUG_ENABLE
  /* set debug */
  ngDebugModOnOff[NG_DBG_IP] = 1;
  ngDebugModOnOff[NG_DBG_UDP] = 1;
  ngDebugModOnOff[NG_DBG_TCP] = 1;
  ngDebugModOnOff[NG_DBG_RESOLV] = 1;
#endif

  return RVM_OK;
}

/**
 * Called by the RV manager to start the RNET SWE,
 *
 * @return  RVM_OK.
 */
T_RVM_RETURN rnet_rt_start( void)
{

  RNET_RT_SEND_TRACE("RNET_RT: start called", RV_TRACE_LEVEL_DEBUG_LOW);

  /* arm timer */

  /* It looks as if the timer runs by a factor of 10 slower in the
   * simulation than it ought to, so we speed it up again here. Target
   * testing must reveal if there is really a difference. */
  
#ifdef _SIMULATION_
#define MILLISECONDS_PER_SECOND 100
#else  /* _SIMULATION_ */
#define MILLISECONDS_PER_SECOND 1000
#endif    
  rvf_start_timer( RNET_RT_NGIP_TIMER,
                   RVF_MS_TO_TICKS(MILLISECONDS_PER_SECOND/NG_TIMERHZ),
                   FALSE);     // TRUE --> FALSE , set once timer for OMAPS00169870 05102008 by pinghua

  /* open interfaces */
  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  rnet_rt_ngip_start();
  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  return RVM_OK;
}

/**
 * Called by the RV manager to stop the RNET SWE.
 *
 * @return  RVM_OK
 */
T_RVM_RETURN rnet_rt_stop( void)
{
  /* NB: Other SWEs have not been killed yet, tut can send messages to other SWEs. */
  RNET_RT_SEND_TRACE("RNET_RT: stop called", RV_TRACE_LEVEL_DEBUG_LOW);

  /* stop protocol timer */
  rvf_stop_timer( RNET_RT_NGIP_TIMER);

  /* shutdown TCP/IP stack */
  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  rnet_rt_ngip_stop();
  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  /* release resources */

  rvf_delete_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

#if 0
  rvf_free_buf( rnet_rt_env_ctrl_blk_p->buf_net);
  rvf_free_buf( rnet_rt_env_ctrl_blk_p);
#endif

  rnet_rt_env_ctrl_blk_p->buf_net = NULL;
  rnet_rt_env_ctrl_blk_p = NULL;

  return RVM_OK;
}

/**
 * Called by the RV manager to kill the RNET SWE,
 * after the rnet_stop function has been called.
 *
 * @return  RVM_OK
 */
T_RVM_RETURN rnet_rt_kill( void)
{
  /*
   * DO NOT SEND MESSAGES
   */

  return RVM_OK;
}

/* Temporary core for RNET */
T_RVM_RETURN rnet_rt_temp_core (void)
{
  T_RVM_RETURN error_status;
  T_RV_HDR *msg_p ;
  UINT16 rec_event;

  RNET_TRACE_HIGH("RNET_RT: core task started");

  /* start RNET */
  rnet_rt_start();

  error_status = RV_OK;
  while (error_status == RV_OK )
  {
    rec_event = rvf_wait(0xffff, 0); /* Wait (infinite) for all events. */
    if (rec_event & RVF_TASK_MBOX_0_EVT_MASK)
    {
      msg_p = rvf_read_mbox(0);
      error_status = rnet_rt_handle_message(msg_p);
    }
    if (rec_event & RVF_TIMER_1_EVT_MASK)
    {
      error_status = rnet_rt_handle_timer(NULL);
    }
  }
  if (error_status ==  RV_MEMORY_ERR ) /* If a memory error happened .. */
  {
    (void)(rnet_rt_env_ctrl_blk_p->error_ft("RNET", RVM_MEMORY_ERR, 0,
              " Memory Error : the RNET primitive memory bank is RED "));
  }
  return RVM_OK;
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

