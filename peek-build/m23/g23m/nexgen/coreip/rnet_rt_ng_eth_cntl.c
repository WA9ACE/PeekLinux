/*****************************************************************************
 * $Id: eth_cntl.c,v 1.2 2001/04/02 14:31:40 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Generic Ethernet Interface Functions
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
 * ngEtherCntl()
 *----------------------------------------------------------------------------
 * 17/09/98 - Regis Feneon
 * 29/12/98 -
 *  real option processing added
 * 19/07/2000 -
 *  removed NG_ETHIFO_DEV1 and NG_ETHIFO_DEV2
 * 03/11/2000 -
 *  added multicast support
 *  added FULLDUPLEX & 10BASEXX options
 *****************************************************************************/

#include <ngip.h>
#include <ngeth.h>

/*****************************************************************************
 * ngEtherCntl()
 *****************************************************************************
 * Set/Get interface options
 *****************************************************************************
 * Parameters:
 *  netid       network id
 *  cmd         NG_CNTL_SET/NG_CNTL_GET
 *  opt         option name (see net_eth.h)
 *  arg         option argument
 * Return value: 0 or error code
 */

int ngEtherCntl( NGifnet *netp, int cmd, int opt, void *arg)
{
  NGubyte maddr[6];
  int i;

  if( cmd == NG_CNTL_SET) switch( opt) {
  case NG_ETHIFO_ADDR:
    ngMemCpy( NG_ETHIF_DATA( netp, eif_addr), *((NGubyte **) arg),
               sizeof( NG_ETHIF_DATA( netp, eif_addr)));
    break;
  case NG_ETHIFO_IRQ:
    NG_ETHIF_DATA( netp, eif_irq) = *((int *) arg);
    break;
  case NG_ETHIFO_IOBASE:
    NG_ETHIF_DATA( netp, eif_iobase) = *((u_int *) arg);
    break;
  case NG_ETHIFO_MEMBASE:
    NG_ETHIF_DATA( netp, eif_physbase) = *((NGuint *) arg);
    NG_ETHIF_DATA( netp, eif_base) =
     ngMTOK( NG_ETHIF_DATA( netp, eif_physbase));
    break;
  case NG_IFO_ADDMULTI:
  case NG_IFO_DELMULTI:
    /* construct ethernet multicast address */
    maddr[0] = 0x01;
    maddr[1] = 0x00;
    maddr[2] = 0x5e;
    maddr[3] = ((NGubyte *) arg)[1] & 0x7f;
    maddr[4] = ((NGubyte *) arg)[2];
    maddr[5] = ((NGubyte *) arg)[3];
    /* find the address into the interface list */
    for( i=0; i<NG_ETHIF_MULTIADDRS_MAX; i++) {
      if( NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_refcount &&
        (NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_addr[5] == maddr[5]) &&
        (NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_addr[4] == maddr[4]) &&
        (NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_addr[3] == maddr[3]) &&
        (NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_addr[2] == maddr[2]) &&
        (NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_addr[1] == maddr[1]) &&
        (NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_addr[0] == maddr[0]))
        break;
    }
    if( opt == NG_IFO_DELMULTI) {
      if( i == NG_ETHIF_MULTIADDRS_MAX) {
        if( --NG_ETHIF_DATA( netp, eif_allmultis) == 0)
          netp->if_setmulti_f( netp);
      }
      else {
        if( --NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_refcount == 0)
          netp->if_setmulti_f( netp);
      }
      break;
    }
    /* add the entry */
    if( i == NG_ETHIF_MULTIADDRS_MAX) {
      /* find an empty slot */
      for( i=0; i<NG_ETHIF_MULTIADDRS_MAX; i++) {
        if( NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_refcount == 0)
          break;
      }
      if( i == NG_ETHIF_MULTIADDRS_MAX) {
        /* no free slot */
        if( NG_ETHIF_DATA( netp, eif_allmultis)++ == 0)
          netp->if_setmulti_f( netp);
      }
      else {
        /* new slot */
        NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_refcount = 1;
        ngMemCpy( &NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_addr[0],
                  &maddr[0], 6);
        netp->if_setmulti_f( netp);
      }
    }
    else {
      NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_refcount++;
    }
    break;
  case NG_ETHIFO_FULLDUPLEX:
    if( *(int *) arg)
      NG_ETHIF_DATA( netp, eif_flags) |= NG_ETHIFF_FULLDUPLEX;
    else
      NG_ETHIF_DATA( netp, eif_flags) &= ~NG_ETHIFF_FULLDUPLEX;
    break;
  case NG_ETHIFO_10BASET:
    if( *(int *) arg)
      NG_ETHIF_DATA( netp, eif_flags) |= NG_ETHIFF_10BASET;
    else
      NG_ETHIF_DATA( netp, eif_flags) &= ~NG_ETHIFF_10BASET;
    break;
  case NG_ETHIFO_10BASE2:
    if( *(int *) arg)
      NG_ETHIF_DATA( netp, eif_flags) |= NG_ETHIFF_10BASE2;
    else
      NG_ETHIF_DATA( netp, eif_flags) &= ~NG_ETHIFF_10BASE2;
    break;
  case NG_ETHIFO_100BASETX:
    if( *(int *) arg)
      NG_ETHIF_DATA( netp, eif_flags) |= NG_ETHIFF_100BASETX;
    else
      NG_ETHIF_DATA( netp, eif_flags) &= ~NG_ETHIFF_100BASETX;
    break;
  default:
    /* call generic interface control function */
    return( ngIfGenCntl( netp, cmd, opt, arg));
  }
  else if( cmd == NG_CNTL_GET) switch( opt) {
  case NG_ETHIFO_ADDR:
    ngMemCpy( *((NGubyte **) arg), NG_ETHIF_DATA( netp, eif_addr),
               sizeof( NG_ETHIF_DATA( netp, eif_addr)));
    break;
  case NG_ETHIFO_IRQ:
    *((int *) arg) = NG_ETHIF_DATA( netp, eif_irq);
    break;
  case NG_ETHIFO_IOBASE:
    *((int *) arg) = NG_ETHIF_DATA( netp, eif_iobase);
    break;
  case NG_ETHIFO_MEMBASE:
    *((NGuint *) arg) = NG_ETHIF_DATA( netp, eif_physbase);
    break;
  default:
    return( ngIfGenCntl( netp, cmd, opt, arg));
  }
  else return( NG_EINVAL);
  return( NG_EOK);
}

