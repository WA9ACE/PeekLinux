/*****************************************************************************
 * $Id: eth_crc.c,v 1.2 2001/04/02 14:31:40 rf Exp $
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
 * ngEtherCRC32()
 *----------------------------------------------------------------------------
 * 06/11/2000 - Regis Feneon
 *****************************************************************************/

#include <ngip.h>
#include <ngeth.h>
#include <ngip/ethernet.h>

/*****************************************************************************
 * ngEtherCRC32()
 *****************************************************************************
 * Compute standard 32-bits ethernet CRC
 *****************************************************************************
 * Parameters:
 *  buf       pointer to data
 *  buflen    size of data
 * Return value: CRC
 */

NGuint ngEtherCRC32( const void *buf, int buflen)
{
  int i, j;
  NGuint crc;
  unsigned int c, carry;

  crc = 0xFFFFFFFFUL;
  for (i = 0; i < buflen; i++) {
    c = ((const NGubyte *) buf)[i];
    for (j = 0; j < 8; j++) {
      carry = ((crc & 0x80000000UL) ? 1 : 0) ^ (c & 0x01);
      crc <<= 1;
      c >>= 1;
      if (carry) crc = (crc ^ 0x04C11DB6UL) | carry;
    }
  }
  return( crc);
}

