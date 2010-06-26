/*****************************************************************************
 * $Id: if_cntl.c,v 1.4 2001/05/14 17:17:39 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Generic Interface Functions
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
 * ngIfGenCntl()
 *----------------------------------------------------------------------------
 * 29/12/98 - Regis Feneon
 * 09/02/2000 -
 *  now use NG_IN_XXX macros
 *  corrected return values
 * 19/07/2000 -
 *  added NG_IFO_DEV1,NG_IFO_DEV2,NG_IFO_DEVPTR1,NG_IFO_DEVPTR2
 * 06/11/2000 -
 *  added NG_IFO_PROMISC & NG_IFO_ALLMULTI
 * 22/02/2001 -
 *  added NG_IFO_MTU option processing
 *  call ngIpRouteUpdate()
 *****************************************************************************/

#include <ngip.h>

/*****************************************************************************
 * ngIfGenCntl()
 *****************************************************************************
 * Set/Get interface options
 *****************************************************************************
 * Parameters:
 *  netid       network id
 *  cmd         NG_CNTL_SET/NG_CNTL_GET
 *  opt         option name (see net_if.h)
 *  arg         option argument
 * Return value: 0 or error code
 */

int ngIfGenCntl( NGifnet *netp, int cmd, int opt, void *arg)
{

    if( cmd == NG_CNTL_SET) switch( opt) {
    case NG_IFO_NAME:
        /* interface name */
        netp->if_name = *((char **) arg);
        break;
    case NG_IFO_OUTQ_MAX:
        /* size of interface output queue */
        if( *((int *) arg) <= 0) return( NG_EINVAL);
        netp->if_outq.qu_nelmax = *((int *) arg);
        break;
    case NG_IFO_ADDR:
    case NG_IFO_NETMASK:
        /* set interface address and/or subnet mask */
        if( opt == NG_IFO_ADDR) {
            netp->if_addr = *((NGuint *) arg);
            /* set netmask */
            if( NG_IN_CLASSA( netp->if_addr)) {
                netp->if_netmask = NG_IN_CLASSA_NET;
            }
            else if( NG_IN_CLASSB( netp->if_addr)) {
                netp->if_netmask = NG_IN_CLASSB_NET;
            }
            else if( NG_IN_CLASSC( netp->if_addr)) {
                netp->if_netmask = NG_IN_CLASSC_NET;
            }
            else {
                /* invalid address */
                netp->if_addr = 0;
                netp->if_netmask = 0;
            }
            netp->if_subnetmask = netp->if_netmask;
        }
        else {
            netp->if_subnetmask = netp->if_netmask|*((NGuint *) arg);
        }
        /* set other address values */
        netp->if_net = netp->if_addr & netp->if_netmask;
        netp->if_netbroadcast = netp->if_net | ~netp->if_netmask;
        netp->if_subnet = netp->if_addr & netp->if_subnetmask;
        netp->if_broadaddr = netp->if_subnet | ~netp->if_subnetmask;
        /* update routing */
        ngIpRouteUpdate();
        break;
    case NG_IFO_DSTADDR:
        /* set destination address with point-to-point interface only */
        if( (netp->if_flags & NG_IFF_POINTOPOINT) == 0) return( NG_EINVAL);
        netp->if_dstaddr = *((NGuint *) arg);
        break;
    case NG_IFO_FLAGS:
        /* dont change internal flags */
        netp->if_flags = (netp->if_flags & NG_IFF_CANTCHANGE)
                          |(*((int *) arg) & ~NG_IFF_CANTCHANGE);
        break;
    case NG_IFO_DEV1:
        netp->if_dev1 = *((int *) arg);
        break;
    case NG_IFO_DEV2:
        netp->if_dev2 = *((int *) arg);
        break;
    case NG_IFO_DEVPTR1:
        netp->if_devptr1 = *((void **) arg);
        break;
    case NG_IFO_DEVPTR2:
        netp->if_devptr2 = *((void **) arg);
        break;
    case NG_IFO_PROMISC:
        if( *(int *) arg)
          netp->if_flags |= NG_IFF_PROMISC;
        else
          netp->if_flags &= ~NG_IFF_PROMISC;
        break;
    case NG_IFO_ALLMULTI:
        if( *(int *) arg)
          netp->if_flags |= NG_IFF_ALLMULTI;
        else
          netp->if_flags &= ~NG_IFF_ALLMULTI;
        break;
    case NG_IFO_MTU:
        if( (*((int *) arg) <= 0) || (*((int *) arg) > (int) ngBufDataMax))
          return( NG_EINVAL);
        netp->if_mtu = *((int *) arg);
        break;
    default:
        return( NG_EINVAL);
    }
    else if( cmd == NG_CNTL_GET) switch( opt) {
    case NG_IFO_NAME:
        *((char **) arg) = netp->if_name;
        break;
    case NG_IFO_OUTQ_MAX:
        *((int *) arg) = netp->if_outq.qu_nelmax;
        break;
    case NG_IFO_ADDR:
        *((NGuint *) arg) = netp->if_addr;
        break;
    case NG_IFO_NETMASK:
        *((NGuint *) arg) = netp->if_subnetmask;
        break;
    case NG_IFO_BRDADDR:
        if( netp->if_flags & NG_IFF_POINTOPOINT) return( NG_EINVAL);
        *((NGuint *) arg) = netp->if_broadaddr;
        break;
    case NG_IFO_DSTADDR:
        if( (netp->if_flags & NG_IFF_POINTOPOINT) == 0) return( NG_EINVAL);
        *((NGuint *) arg) = netp->if_dstaddr;
        break;
    case NG_IFO_FLAGS:
        *((int *) arg) = netp->if_flags;
        break;
    case NG_IFO_DEV1:
        *((int *) arg) = netp->if_dev1;
        break;
    case NG_IFO_DEV2:
        *((int *) arg) = netp->if_dev2;
        break;
    case NG_IFO_DEVPTR1:
        *((void **) arg) = netp->if_devptr1;
        break;
    case NG_IFO_DEVPTR2:
        *((void **) arg) = netp->if_devptr2;
        break;
    case NG_IFO_MTU:
        *((int *) arg) = netp->if_mtu;
        break;
    default:
        return( NG_EINVAL);
    }
    else return( NG_EINVAL);
    return( NG_EOK);
}

