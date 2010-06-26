/**
 * @file    rnet_rt_ngwinnet.c
 *
 * Riviera RNET - WIN32 (WINPCAP) Ethernet Driver
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  4/5/2002    Regis Feneon  Riviera version
 *
 */

/*****************************************************************************
 * $Id: rnet_rt_ngwinnet.c,v 1.2 2002/04/30 12:44:46 rf Exp $
 * $Name: ti_20021030 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Win32 support for NexGenOS - Network Driver
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2001 NexGen Software.
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
 * ngNetDrv_WIN32
 *----------------------------------------------------------------------------
 * 23/04/2001 - Regis Feneon
 * 20/11/2001 -
 *  full support of multicast filtering
 *  selection of adapter by name (option DEV1PTR) works with NT/2000 and 98
 *****************************************************************************/

#ifdef _WINDOWS

/* BPF packet driver */
#include <winsock2.h>
#include <net/bpf.h>
#include <packet32.h>
#include <ntddndis.h>
#include <limits.h>

/* remove duplicated definitions */
#define NG_NO_BSDSOCK_DECLS
#define NG_UTYPES

#include "rnet_rt_i.h"
#include "rnet_trace_i.h"

#include <ngos.h>
#include <ngeth.h>
#include <ngip/ethernet.h>

#define RCVDATAMAX 16384

struct winnet_data {
  LPADAPTER lpAdapter;
  LPADAPTER lpAdapter2;
  HANDLE drvthread;
  HANDLE sem;
  LPPACKET lpPacket;
  char buffer[RCVDATAMAX]; /* buffer to hold the data coming from the driver */
};

/*
 * Receive task
 */
static DWORD WINAPI drvHandler( LPVOID data)
{
  NGifnet *netp;
  struct winnet_data *dp;
  T_RNET_RT_NGIP_NETIF_MSG *msg;
  int err;

  netp = (NGifnet *) data;
  dp = (struct winnet_data *) netp->if_dev2;

  if((dp->lpPacket = PacketAllocatePacket()) == NULL){
    return( 0);
  }
  PacketInitPacket( dp->lpPacket, dp->buffer, RCVDATAMAX);

  /* set a 1 second read timeout... */
  PacketSetReadTimeout( dp->lpAdapter, 1000);

  while(1)
  {

    /* capture the packets */
    if( !PacketReceivePacket( dp->lpAdapter, dp->lpPacket, TRUE)) {
      break;
    }

    if( dp->lpPacket->ulBytesReceived > 0) {
      /* prepare message */
      msg = NULL;
      err = rvf_get_msg_buf( rnet_rt_env_ctrl_blk_p->mb_id,
        sizeof(T_RNET_RT_NGIP_NETIF_MSG), RNET_RT_NGIP_NETIF_MSG,
        (T_RV_HDR **) &msg);
      if( (err != RVF_GREEN) && (err != RVF_YELLOW)) {
        RNET_RT_SEND_TRACE("RNET_RT: cannot allocate MSG ",RV_TRACE_LEVEL_WARNING);
      }
      else {
        /* send msg to rnet */
        msg->netp = netp;
        msg->msgp = NULL;
        if( rvf_send_msg( rnet_rt_env_ctrl_blk_p->addr_id, (T_RV_HDR *) msg) != RVF_OK) {
          RNET_RT_SEND_TRACE("RNET_RT: cannot send MSG ",RV_TRACE_LEVEL_WARNING);
        }
        else {
          /* wait for end of packet processing */
          WaitForSingleObject( dp->sem, INFINITE);
        }
      }
    }

  }

  PacketFreePacket( dp->lpPacket);
  dp->lpPacket = NULL;

  return( 0);
}

/*****************************************************************************
 * drvCntl()
 *****************************************************************************
 *  Process incoming packets
 *****************************************************************************
 * Parameters:
 *  netp        network interface
 */

int drvCntl( NGifnet *netp, int cmd, int opt, void *arg)
{
  struct winnet_data *dp;
  int off, nsig;
  struct bpf_hdr *hdr;
  NGbuf *bufp;

  switch( opt) {
  case NG_RNETIFO_HANDLE_MSG:

    dp = (struct winnet_data *) netp->if_dev2;

    off = 0;
    while( off < dp->lpPacket->ulBytesReceived) {

      /* BPF header */
      hdr = (struct bpf_hdr *)(((NGubyte*)dp->lpPacket->Buffer)+off);
      /* start of data */
      off += hdr->bh_hdrlen;

      /* copy data to message buffer */
      ngBufAlloc( bufp);
      if( bufp != NULL) {
        bufp->buf_datap = ((NGubyte *) bufp) + ngBufDataOffset - ETHER_HDR_LEN;
        bufp->buf_datalen = hdr->bh_datalen;
        ngMemCpy( bufp->buf_datap, ((NGubyte*)dp->lpPacket->Buffer)+off, bufp->buf_datalen);
        ngEtherInput( netp, bufp, 1);
      }
      else {
        netp->if_iqdrops++;
      }

      /* next packet */
      off = Packet_WORDALIGN( off + hdr->bh_datalen);
    }

    /* signal driver task */
    ReleaseSemaphore( dp->sem, 1, NULL);

    return( NG_EOK);

  default:
    return( ngEtherCntl( netp, cmd, opt, arg));
  }
}

/*****************************************************************************
 * drvStart()
 *****************************************************************************
 *  Unqueue the next available buffer and start the sending engine.
 *****************************************************************************
 * Parameters:
 *  netp        network interface
 */

static void drvStart( NGifnet *netp)
{
  NGbuf *bufp;
  int ictl;
  LPPACKET lpPacket;
  struct winnet_data *dp;

  dp = (struct winnet_data *) netp->if_dev2;

  /* message to send ? */
  ngBufDequeue( netp, bufp);

  if( bufp == NULL) {
    return;
  }

  if( (lpPacket = PacketAllocatePacket()) == NULL) {
    netp->if_oerrors++;
  }
  else {
    PacketInitPacket( lpPacket, bufp->buf_datap, bufp->buf_datalen);
    PacketSendPacket( dp->lpAdapter2, lpPacket, TRUE);
    PacketFreePacket( lpPacket);
    netp->if_opackets++;
    netp->if_obytes += bufp->buf_datalen;
  }

  /* release the buffer */
  ngBufOutputFree( bufp);

  return;
}

/*****************************************************************************
 * drvInit()
 *****************************************************************************
 * setup multicast receive filter
 *****************************************************************************
 * Parameters:
 *  netp        network interface
 * Return value: 0 if OK or error code
 */

static struct bpf_insn BPFRejectEtherSource[] =
{
  /* Hardcoded Source Address: 01.02.03.04.05.06 */
  /* Check Source Ethernet source address High DWORD At Offset 6 */
  BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 6),
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x01020304, 0, 3), /* Source == 1.2.3.4 */
  /* Check Source Ethernet source address Low WORD At Offset 10 */
  BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 10),
  BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x0506, 0, 1), /* Source == 5.6 */

  BPF_STMT(BPF_RET+BPF_K, 0 ), // Reject
  BPF_STMT(BPF_RET+BPF_K, (UINT)-1), // Accept. Value is bytes to be
};

static void drvInit( NGifnet *netp)
{
  struct bpf_program bpfprg;
  struct winnet_data *dp;
  static struct {
    ULONG Oid;
    ULONG Length;
    UCHAR Data[ETHER_ADDR_LEN*NG_ETHIF_MULTIADDRS_MAX];
  } oid;
  int i;

  dp = (struct winnet_data *) netp->if_dev2;

  /* set hardware reception filter */
  if( netp->if_flags & NG_IFF_PROMISC)
    PacketSetHwFilter( dp->lpAdapter, NDIS_PACKET_TYPE_PROMISCUOUS);
  else if( (netp->if_flags & NG_IFF_ALLMULTI) ||
           NG_ETHIF_DATA( netp, eif_allmultis))
    PacketSetHwFilter( dp->lpAdapter, NDIS_PACKET_TYPE_ALL_MULTICAST|
                                      NDIS_PACKET_TYPE_BROADCAST|
                                      NDIS_PACKET_TYPE_DIRECTED);
  else
    PacketSetHwFilter( dp->lpAdapter, NDIS_PACKET_TYPE_MULTICAST|
                                      NDIS_PACKET_TYPE_BROADCAST|
                                      NDIS_PACKET_TYPE_DIRECTED);

  /* set multicast list */
  oid.Oid = OID_802_3_MULTICAST_LIST;
  oid.Length = 0;
  if( !(netp->if_flags & NG_IFF_PROMISC) &&
      !(netp->if_flags & NG_IFF_ALLMULTI) &&
      !NG_ETHIF_DATA( netp, eif_allmultis)) {
    for( i=0; i<NG_ETHIF_MULTIADDRS_MAX; i++) {
      if( NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_refcount) {
        ngMemCpy( &oid.Data[oid.Length],
                  NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_addr,
                  ETHER_ADDR_LEN);
        oid.Length += ETHER_ADDR_LEN;
      }
    }
  }
  PacketRequest( dp->lpAdapter, TRUE, (PPACKET_OID_DATA) &oid);

  /* set BPF filter: reject our own transmissions */
  BPFRejectEtherSource[1].k = (NG_ETHIF_DATA( netp, eif_addr[0]) << 24) |
                              (NG_ETHIF_DATA( netp, eif_addr[1]) << 16) |
                              (NG_ETHIF_DATA( netp, eif_addr[2]) << 8) |
                              (NG_ETHIF_DATA( netp, eif_addr[3]));
  BPFRejectEtherSource[3].k = (NG_ETHIF_DATA( netp, eif_addr[4]) << 8) |
                              (NG_ETHIF_DATA( netp, eif_addr[5]));
  bpfprg.bf_len = sizeof( BPFRejectEtherSource)/sizeof( BPFRejectEtherSource[0]);
  bpfprg.bf_insns = BPFRejectEtherSource;
  PacketSetBpf( dp->lpAdapter, &bpfprg);

  return;
}

/*****************************************************************************
 * drvOpen()
 *****************************************************************************
 * Driver initialisation
 *****************************************************************************
 * Parameters:
 *  netp        network interface
 * Return value: 0 if OK or error code
 */

#define ADAPTER_NUM_MAX 10
#define ADAPTER_NAME_MAX 256
static char AdapterList[ADAPTER_NUM_MAX][ADAPTER_NAME_MAX];
static char AdapterName[8192];

static int drvOpen( NGifnet *netp)
{
  int i, j, AdapterNum;
  DWORD tid;
  DWORD dwErrorCode, dwVersion, dwWindowsMajorVersion;
  struct bpf_program bpfprg;
  ULONG AdapterLength;
  struct winnet_data *dp;

  /* interface already running ? */
  if( netp->if_flags & NG_IFF_RUNNING) {
    RNET_RT_SEND_TRACE("RNET_RT: WINNET_open: already running",RV_TRACE_LEVEL_ERROR);
    return( NG_EALREADY);
  }

  /*
   * obtain the name of the adapters installed on this machine
   * the data returned by PacketGetAdapterNames is different in Win95 and in WinNT.
   * We have to check the os on which we are running
   */
  i = 0;
  dwVersion=GetVersion();
  dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));

  if (!(dwVersion >= 0x80000000 && dwWindowsMajorVersion >= 4)) {
    /* Windows NT - unicode strings */
    WCHAR *temp, *temp1;

    AdapterLength = sizeof( AdapterName)/2;
    PacketGetAdapterNames( AdapterName, &AdapterLength);

    temp = (WCHAR *) AdapterName;
    temp1 = (WCHAR *) AdapterName;
    while( (*temp != '\0') || (*(temp-1) != '\0')) {
      if( *temp == '\0') {
        /* convert name to ASCII */
        j = 0;
        while( *temp1 != 0) {
          AdapterList[i][j++] = *temp1++;
        }
        AdapterList[i][j] = 0;
        temp1 = temp+1;
        i++;
      }
      temp++;
    }
    AdapterNum = i;
  }
  else {
    /* Windows 95 - ascii strings */
    char *tempa, *temp1a;

    AdapterLength = sizeof( AdapterName);
    PacketGetAdapterNames( AdapterName, &AdapterLength);

    tempa = AdapterName;
    temp1a = AdapterName;
    while( (*tempa!='\0')||(*(tempa-1)!='\0')) {
      if (*tempa=='\0') {
        ngMemCpy( AdapterList[i], temp1a, tempa-temp1a);
        temp1a=tempa+1;
        i++;
      }
      tempa++;
    }
    AdapterNum=i;
  }

  if( AdapterNum == 0) {
    /* no device installed */
    RNET_RT_SEND_TRACE("RNET_RT: WINNET_open: winpcap not installed",RV_TRACE_LEVEL_ERROR);
    return( NG_ENODEV);
  }

  /* device name provided ? */
  if( netp->if_devptr1 != NULL) {
    netp->if_dev1 = -1;
    for( i=0; i<AdapterNum; i++) {
      if( ngStrCmp( netp->if_devptr1, AdapterList[i]) == 0) {
        netp->if_dev1 = i;
        break;
      }
    }
    if( netp->if_dev1 == -1) {
      RNET_RT_SEND_TRACE("RNET_RT: WINNET_open: device not found",RV_TRACE_LEVEL_ERROR);
      return( NG_ENODEV);
    }
  }

  /* allocate data structure */
  dp = (struct winnet_data *) LocalAlloc( LMEM_FIXED, sizeof( struct winnet_data));
  if( dp == NULL) {
    RNET_RT_SEND_TRACE("RNET_RT: WINNET_open: LocalAlloc failed",RV_TRACE_LEVEL_ERROR);
    return( NG_ENOMEM);
  }

  netp->if_dev2 = (int) dp;

  /* we need to open two connections for receiving and sending */
  dp->lpAdapter = PacketOpenAdapter( AdapterList[netp->if_dev1]);
  if( !dp->lpAdapter || (dp->lpAdapter->hFile == INVALID_HANDLE_VALUE)) {
    dwErrorCode = GetLastError();
    LocalFree( (HLOCAL) netp->if_dev2);
    RNET_RT_SEND_TRACE("RNET_RT: WINNET_open: cannot open device",RV_TRACE_LEVEL_ERROR);
    return( NG_ENODEV);
  }

  dp->lpAdapter2 = PacketOpenAdapter( AdapterList[netp->if_dev1]);
  if( !dp->lpAdapter2 || (dp->lpAdapter2->hFile == INVALID_HANDLE_VALUE)) {
    dwErrorCode=GetLastError();
    PacketCloseAdapter( dp->lpAdapter);
    LocalFree( (HLOCAL) netp->if_dev2);
    RNET_RT_SEND_TRACE("RNET_RT: WINNET_open: cannot open device",RV_TRACE_LEVEL_ERROR);
    return( NG_ENODEV);
  }

  /* interface is running */
  netp->if_flags |= NG_IFF_RUNNING|NG_IFF_UP;

  /* get mac address */
  {
    struct {
      ULONG Oid;
      ULONG Length;
      UCHAR Data[ETHER_ADDR_LEN];
    } oid;

    oid.Oid = OID_802_3_CURRENT_ADDRESS;
    oid.Length = ETHER_ADDR_LEN;
    if( PacketRequest( dp->lpAdapter, FALSE, (PPACKET_OID_DATA) &oid)) {
      ngMemCpy( NG_ETHIF_DATA( netp, eif_addr), oid.Data, ETHER_ADDR_LEN);
    }
  }

  /* set a 128K buffer in the driver */
  PacketSetBuff( dp->lpAdapter, 128000);

  /* set receive filter */
  drvInit( netp);

  /* create handler task */
  dp->sem = CreateSemaphore( NULL, 0, 100, NULL);
  dp->drvthread = CreateThread( NULL, 0, drvHandler, (LPVOID) netp, 0, &tid);

  return( NG_EOK);
}

/*****************************************************************************
 * drvClose()
 *****************************************************************************
 * Driver cleanup
 * Ethernet controler shutdown
 *****************************************************************************
 * Parameters:
 *  netp        network interface
 * Return value: 0 if OK or error code
 */

static int drvClose( NGifnet *netp)
{
  NGbuf *bufp;
  struct winnet_data *dp;

  /* interface should be running to be closed */
  if( !(netp->if_flags & NG_IFF_RUNNING)) {
    return( NG_EALREADY);
  }
  /* clear flags */
  netp->if_flags &= ~(NG_IFF_RUNNING|NG_IFF_UP);

  /* clear pending messages */
  while( 1) {
    ngBufDequeue( netp, bufp);
    if( bufp == NULL) break;
    ngBufOutputFree( bufp);
  }

  dp = (struct winnet_data *) netp->if_dev2;

  /* release resources */
  CloseHandle( dp->drvthread);
  CloseHandle( dp->sem);
  PacketCloseAdapter( dp->lpAdapter);
  PacketCloseAdapter( dp->lpAdapter2);
  LocalFree( (HLOCAL) netp->if_dev2);

  return( NG_EOK);
}

/********************************************************************/
/* Driver entry point */

const NGnetdrv ngNetDrv_WIN32 = {
  "WINNET-RIVIERA",
  NG_IFT_ETHER,
  NG_IFF_BROADCAST|NG_IFF_SIMPLEX|NG_IFF_MULTICAST,
  ETHERMTU,
  10000000UL,
  ngEtherInit,
  drvOpen,
  drvClose,
  ngEtherOutput,
  drvStart,
  drvCntl,
  drvInit
};

#endif /* _WINDOWS */

