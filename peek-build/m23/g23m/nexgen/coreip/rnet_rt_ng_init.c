/**
 * @file    rnet_rt_ng_init.c
 *
 * Riviera RNET - NexGenOS/NexGenIP Global Initialization
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  3/14/2002   Regis Feneon  Riviera version
 *  28/10/2002   Regis Feneon  setup system clock frequency to NG_TIMERHZ
 *
 */

/*****************************************************************************
 * $Id: rnet_rt_ng_init.c,v 1.4 2002/10/30 15:30:32 rf Exp $
 * $Name: ti_20021030 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * NexGenOS Global Initialization
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2002 NexGen Software.
 *
 *  All rights reserved. NexGen Software' source code is an unpublished
 *  work and the use of a copyright notice does not imply otherwise.
 *  This source code contains confidential, trade secret material of
 *  NexGen Software. Any attempt or participation in deciphering, decoding,
 *  reverse engineering or in any way altering the source code is
 *  strictly prohibited, unless the prior written consent of
 *  NexGen Software is obtained.
 *
 *    This software is  supplied  under  the terms of a
 *    license agreement or nondisclosure agreement with
 *    NexGen Software, and may not be copied or disclosed
 *    except  in  accordance  with  the  terms of  that
 *    agreement.
 *
 *----------------------------------------------------------------------------
 * ngInit()
 *----------------------------------------------------------------------------
 * 21/10/98 - Regis Feneon
 * 27/10/98 -
 *  Added call to ngOSInit
 * 06/12/98 -
 *  Added new config table support
 * 30/12/98 -
 *  New configuration system
 * 12/01/99 -
 *  change in interface initialization - ngIfAdd is no more called from here
 * 30/06/99 -
 *  driver init function suppressed - now interface initialisation is done
 *  in ngInit function
 * 12/08/99 -
 *  version and copyright strings added
 * 15/09/99 -
 *  NG_IF_ADD: call driver init function if set
 * 11/10/99 -
 *  added device io interface options
 * 05/11/99 -
 *  added NG_BUFO_ALLOC_BUF_F option
 * 08/11/99 -
 *  ngDevioData moved here
 * 20/03/2000 -
 *  updated version string to 1.1A
 * 30/05/2000 -
 *  added error codes
 *  now network interfaces are initialized here even in rtos mode
 * 09/06/2000 -
 *  added initialization of static data
 * 19/07/2000 -
 *  v1.2 - added new device options and device task creation
 *  now network interfaces, buffers and socket are optionals
 *  added initialization of if_buflist for network interfaces
 * 22/08/2000 -
 *  initialization of new NGdev structure
 * 19/09/2000 -
 *  added test and error reporting for device and interface opening
 * 03/10/2000 -
 *  added buffer deallocation options
 * 05/10/2000 -
 *  initialization of polling list
 * 03/11/2000 -
 *  added initialization of if_setmulti_f
 * 24/11/2000 -
 *  now ngProtoList is an array indexed with the protocol number
 *  added ngYield_lasttime initialization
 *  netp->if_mtu = min( ngBufDataMax, netd->netd_mtu)
 * 18/01/2001 -
 *  initialize and lock global mutex before parsing the config table
 *  now unlock mutex correctly when driver errors
 *  removed calls to ngDebuGSetLevel() and ngDebugSetModule()
 * 08/02/2001 -
 *  initialize dev_iflags & dev_oflags to zero
 * 14/02/2001 -
 *  added initialization of ngSock_selnsleep and ngSock_selsem
 * 04/03/2001 -
 *  global data in ngOSData, added support for modules
 *  removed so_handle & dcb_handle
 * 29/05/2001 -
 *  UNLOCK before initializing modules
 *****************************************************************************/

#include "rnet_rt_i.h"

#include <ngos.h>
#include <ngos/netglob.h>

/* Copyright and version number */

const char * const ngVer_copyright = "Copyright (c) 2002 WWW.NexGen-Software.fr";
const char * const ngVer_NGOS = "NexGenOS/RNET v1.3b";
extern const char * const ngVer_NGIP;
char ngVer_str[256];

/*
 * Global Variables
 */

const NGproto *ngProtoList[NG_PROTO_MAX]; /* chained list of protocols */
NGifnet *ngIfList; /* chained list of interfaces */

NGqueue ngBuf_freeq; /* queue of free message buffers */
u_int ngBufDataMax; /* maximum data size in message buffer */
u_int ngBufDataOffset; /* ip header offset in message buffer */

NGqueue ngSock_freeq; /* queue of free socket descriptors */

/* OS global data */
NGosdata ngOSData;

/* version string */

/*****************************************************************************
 * rnet_rt_ngip_init()
 *****************************************************************************
 * Initialize the stack
 *****************************************************************************
 * Parameters:
 *  bufpool_ptr   pointer to network buffer pool
 *  bufpool_size  size of network buffer pool
 *  buf_hdr_size  maximum link-layer header size
 *  buf_data_size maximum link-layer data size (mtu)
 *  socktable     pointer to table of sockets
 *  sockmax       maximum number of sockets
 *  cfg           pointer to configuration table
 * Return value: 0 or error code
 */

int rnet_rt_ngip_init( void *bufpool_ptr, int bufpool_size, int buf_hdr_size,
  int buf_data_size, T_RNET_RT_SOCK *socktable, int sockmax, const NGcfgent *cfg)
{
  NGifnet *netp, *lastnetp;
  NGnetdrv *drvp;
  const NGproto *pr;
  int flag, i, error, buf_total_size;
  int bufmax;
  NGubyte *p;
  /* int ncfg; */

  /* initialize version string */
  ngStrCpy( ngVer_str, ngVer_NGOS);
  ngStrCat( ngVer_str, ", ");
  ngStrCat( ngVer_str, ngVer_NGIP);
  ngStrCat( ngVer_str, ", ");
  ngStrCat( ngVer_str, ngVer_copyright);

  /* intialize OS data */
  NGOS_INIT_NAME( "RIVIERA");
  NGOS_INIT_VERSION( 1, 30, ' ');
  NGOS_INIT_DATE( 28, 10, 2002);
  NGOS_INIT_CLOCKFREQ( NG_TIMERHZ);

  error = 0;

  /*
   * Message Buffers
   */

  /* calculate data offset */
  /* align data on processor word boundary */
#if defined (NG_ALIGN32)
  ngBufDataOffset = (sizeof( NGbuf) + buf_hdr_size + 3)&~3;
  buf_total_size = (ngBufDataOffset + buf_data_size + 3)&~3;
#elif defined (NG_ALIGN16)
  ngBufDataOffset = (sizeof( NGbuf) + buf_hdr_size + 1)&~1;
  buf_total_size = (ngBufDataOffset + buf_data_size + 1)&~1;
#else
#error "NG_ALIGNXX macro not defined"
#endif
  ngBufDataMax = buf_total_size - ngBufDataOffset;

  /* total number of buffers */
  bufmax = bufpool_size/buf_total_size;

  /* initialize queue of free buffers */
  NG_QUEUE_INIT( &ngBuf_freeq, bufmax);
  p = (NGubyte *) bufpool_ptr;
  for( i = 0; i < bufmax; i++) {
    ((NGbuf *) p)->buf_node.prev = (NGnode *) p;
    ((NGbuf *) p)->buf_node.next = (NGnode *) p;
    NG_QUEUE_IN( &ngBuf_freeq, (NGbuf *) p);
    p += buf_total_size;
  }

  /*
   * Socket Control Blocks
   */
  NG_QUEUE_INIT( &ngSock_freeq, sockmax);
  ngMemSet( socktable, 0, sockmax*sizeof( T_RNET_RT_SOCK));
  for( i = 0; i < sockmax; i++) {
    NG_QUEUE_IN( &ngSock_freeq, socktable);
    socktable++;
  }

  /*
   * Install protocols and network interfaces
   */

  /* reset global data */
  for( i=0; i<NG_PROTO_MAX; i++) {
    ngProtoList[i] = NULL;
  }
  ngIfList = NULL;

  /* read configuration table */
  pr = NULL;
  lastnetp = netp = NULL;
  flag = 1;
  while( flag) {
    switch( cfg->cfg_option) {

    /*
     * Special commands
     */
    case NG_CFG_END:
      flag = 0;
      break;
    case NG_CFG_NOP:
      break;

#ifdef RNET_RT_NGIP_DEBUG_ENABLE
    case NG_DEBUGO_LEVEL:
      ngDebugLevel = *((int *) &cfg->cfg_arg);
      break;
    case NG_DEBUGO_MODULE:
      if( *((unsigned int *) &cfg->cfg_arg) > NG_DBG_MAX) break;
      ngDebugModOnOff[*((int *) &cfg->cfg_arg)] = 1;
      break;
#endif

    /*
     * Add a Protocol
     */
    case NG_CFG_PROTOADD:
      netp = NULL;
      pr = *((const NGproto **) &cfg->cfg_arg);
      /* check protocol number */
      if( (pr->pr_protocol >= NG_PROTO_MAX) ||
        (pr->pr_protocol < 0) ||
        (ngProtoList[pr->pr_protocol] != NULL) ) {
        pr = NULL;
        error |= NG_EINIT_PROTOCOL;
        break;
      }
      /* add protocol to the list */
      ngProtoList[pr->pr_protocol] = pr;
      /* call protocol init function */
      if( pr->pr_init_f) (pr->pr_init_f)();
      break;

    /*
     * Add an Interface
     */
    case NG_CFG_IFADD:
    case NG_CFG_IFADDWAIT:
      pr = NULL;
      netp = *((NGifnet **) &cfg->cfg_arg);
      cfg++; /* driver MUST follow */
      if( cfg->cfg_option != NG_CFG_DRIVER) {
        netp = NULL;
        cfg--;
        break;
      }
      /* pointer to driver structure */
      drvp = *((NGnetdrv **) &cfg->cfg_arg);

      /* initialize interface structure */
      ngMemSet( netp, 0, sizeof( NGifnet));
      netp->if_type = drvp->netd_type;
      netp->if_flags = drvp->netd_flags;
      netp->if_mtu = drvp->netd_mtu;
      if( netp->if_mtu > (int) ngBufDataMax) netp->if_mtu = ngBufDataMax;
      netp->if_bps = drvp->netd_bps;
      netp->if_drvname = drvp->netd_name;
      netp->if_open_f = drvp->netd_open_f;
      netp->if_close_f = drvp->netd_close_f;
      netp->if_output_f = drvp->netd_output_f;
      netp->if_start_f = drvp->netd_start_f;
      netp->if_cntl_f = drvp->netd_cntl_f;
      netp->if_setmulti_f = drvp->netd_setmulti_f;
      /* initialize output queue */
      NG_QUEUE_INIT( &netp->if_outq, NG_IF_OUTQ_DFLT);
      /* initialize internal buffer list */
      NG_NODE_INIT( &netp->if_buflist);

      /* call driver init function if set */
      if( drvp->netd_init_f != NULL)
          (void)(drvp->netd_init_f)( netp);

      /* add interface in the list */
      if( lastnetp) lastnetp->if_next = netp;
      else ngIfList = netp;
      lastnetp = netp;
      /* autostart flag */
      if( (cfg-1)->cfg_option == NG_CFG_IFADD)
          netp->if_type |= NG_IFT_AUTOSTART;
      break;

    /*
     * specific options
     */
    default:
      /* pass the option to the current device/interface/protocol */
      if( netp) (void)(netp->if_cntl_f)( netp, NG_CNTL_SET,
                         cfg->cfg_option, (void *) &cfg->cfg_arg);
      else if( pr) (void)(pr->pr_cntl_f)( NG_CNTL_SET,
                         cfg->cfg_option, (void *) &cfg->cfg_arg);
      break;

    }

    cfg++;
  }

  return( error);
}

