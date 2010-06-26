/*****************************************************************************
 * $Id: ngeth.h,v 1.3 2001/11/20 14:36:07 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Ethernet Interface Structures
 * Ethernet Address Resolution protocol
 *----------------------------------------------------------------------------
 *    Copyright (c) 1999-2001 NexGen Software.
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
 * 07/09/98 - Regis Feneon
 * 12/01/99 -
 *  clarifications in ethernet interface data structure
 * 23/07/99 -
 *  protocol list removed
 * 08/02/2000 -
 *  removed proto for ngEtherInit()
 *  added proto for ngEtherInput()
 * 15/02/2000 -
 *  added eif_buflist in NGethifdata and ngEtherBufAttach()/ngEtherBufDetach()
 *  prototypes for driver internal buffer management
 * 19/07/2000 -
 *  added arp protocol structure
 *  new interface structure NGethifnet (derived from NGethifdata)
 *   removed eif_dev1,eif_dev2,eif_buflist now in NGifnet
 *  removed options NG_ETHIFO_DEV1 and NG_ETHIFO_DEV2
 *  removed protos for ngEtherBufAttach() and ngEtherBufDetach()
 *  added fromisr parameter to ngEtherInput()
 * 20/07/2000 -
 *  added NG_ARPO_ADDENTRY and NG_ARPO_DELENTRY commands
 *  added definition of NGarphost structure
 * 03/11/2000 -
 *  added multicast options
 *  added ngEtherInit() and ngEtherCRC32() prototypes
 *  added eif_flags field
 *  added options FULLDUPLEX,10BASET,10BASE2,100BASETX
 *****************************************************************************/

#ifndef __NG_NGETH_H_INCLUDED__
#define __NG_NGETH_H_INCLUDED__

#include <ngip.h>

/* NG_BEGIN_DECLS // confuses Source Insight */

/*
 * Public
 */

#define NG_ETHIF_DATA( netp, member) (((NGethifnet *)(netp))->member)

#define NG_ETHIF_BUFI_MAX 4
#define NG_ETHIF_BUFO_MAX 4
#define NG_ETHIF_DATA_MAX 16
#define NG_ETHIF_MULTIADDRS_MAX 8

/* ethernet interface */
typedef struct {
    NGifnet eif_net; /* generic interface */
    NGushort eif_flags; /* some flags: */
#define NG_ETHIFF_FULLDUPLEX 0x0001 /* force full-duplex mode */
#define NG_ETHIFF_10BASET 0x0002 /* select 10BASET output */
#define NG_ETHIFF_10BASE2 0x0004 /* select 10BASE2 output */
#define NG_ETHIFF_100BASETX 0x0008 /* select 100BASETX output */
    NGubyte eif_addr[6]; /* board ethernet address */
    int eif_irq; /* irq level */
    u_int eif_iobase; /* io port address */
    void NGfar *eif_base; /* base address */
    NGuint eif_physbase; /* base address (physical) */
    NGbuf *eif_bufi[NG_ETHIF_BUFI_MAX]; /* input buffers (dma) */
    NGbuf *eif_bufo[NG_ETHIF_BUFO_MAX]; /* output buffers (dma) */
    int eif_allmultis; /* >0 if get all multicasts */
    struct {
      NGshort eifm_refcount; /* >0 if valid */
      NGubyte eifm_addr[6]; /* multicast address */
    } eif_multiaddrs[NG_ETHIF_MULTIADDRS_MAX];
    NGubyte eif_data[NG_ETHIF_DATA_MAX]; /* driver specific data */
} NGethifnet;

/* arp host entry */
typedef struct {
    int ae_flags; /* entry flags */
#define NG_ARPF_INUSE 0x0001 /* entry in use */
#define NG_ARPF_INCOMPLETE 0x0002 /* not complete */
#define NG_ARPF_REJECT 0x0004 /* rejected state */
#define NG_ARPF_STATIC 0x0008 /* permanent entry */
    NGuint ae_inaddr; /* host IP address */
    NGubyte ae_phaddr[6]; /* ethernet address */
    int ae_expire; /* timeout */
    int ae_asked; /* number of retries */
    NGbuf *ae_holdmsg; /* last message waiting */
} NGarpent;

/* ethernet/arp options */
#define NG_ETHIFO_ADDR 0x0200
#define NG_ETHIFO_PRMODE 0x0201
#define NG_ETHIFO_MCAST 0x0202
#define NG_ETHIFO_IRQ 0x0203
#define NG_ETHIFO_IOBASE 0x0204
#define NG_ETHIFO_MEMBASE 0x0205
#define NG_ETHIFO_DEV1 NG_IFO_DEV1
#define NG_ETHIFO_DEV2 NG_IFO_DEV2
#define NG_ETHIFO_FULLDUPLEX 0x0206
#define NG_ETHIFO_10BASET 0x0207
#define NG_ETHIFO_10BASE2 0x0208
#define NG_ETHIFO_100BASETX 0x0209
#define NG_ARPO_MAX 0x0280
#define NG_ARPO_TABLE 0x0281
#define NG_ARPO_RETRY 0x0282
#define NG_ARPO_WAIT 0x0283
#define NG_ARPO_EXPIRE 0x0284
#define NG_ARPO_REJECT 0x0285
#define NG_ARPO_ADDENTRY 0x0286
#define NG_ARPO_DELENTRY 0x0287

/* argument for ADDENTRY and DELENTRY */
typedef struct {
    NGuint arp_inaddr;
    NGubyte arp_phaddr[6];
} NGarphost;

/* ARP User Functions */
int ngArpAddEntry( NGuint in_addr, NGubyte *ph_addr);
int ngArpDeleteEntry( NGuint in_addr);

/* protocol structure */
extern const NGproto ngProto_ARP;

/*
 * Private
 */

/* Standard Ethernet Functions */
int ngEtherInit( NGifnet *netp);
int ngEtherCntl( NGifnet *netp, int cmd, int option, void *arg);
int ngEtherOutput( NGifnet *netp, NGbuf *bufp, NGuint dst_addr);
void ngEtherInput( NGifnet *netp, NGbuf *bufp, int fromisr);
NGuint ngEtherCRC32( const void *buf, int buflen);

/* ARP Internal Functions */
void ngArpRequest( NGifnet *ifnetp, NGuint in_addr);
NGarpent *ngArpLookUp( NGuint in_addr, int create);

/* ethernet broadcast and multicast addresses */
extern const NGubyte ngEtherAddrBcast[6];
extern const NGubyte ngEtherAddrZero[6];

/* arp data */
extern int ngArp_max; /* size of arp table */
extern NGarpent *ngArp_table; /* arp table */
extern int ngArp_retry; /* maximum number of retries */
extern u_int ngArp_wait; /* pause between two retries */
extern u_int ngArp_expire; /* expiration timer */
extern u_int ngArp_reject; /* reject timer */

/* NG_END_DECLS // confuses Source Insight */

#endif

