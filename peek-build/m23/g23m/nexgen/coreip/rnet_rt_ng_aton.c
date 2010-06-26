/*****************************************************************************
 * $Id: aton.c,v 1.2 2001/04/02 14:31:40 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Internet address manipulation
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
 * ngInetATON()
 *----------------------------------------------------------------------------
 * 25/05/98 - Regis Feneon
 *****************************************************************************/

#include <ngip.h>

#undef _ISDIGIT
#define _ISDIGIT(c) (((c)>='0')&&((c)<='9'))

/*****************************************************************************
 * ngInetATON()
 *****************************************************************************
 * Convert an ascii string to an internet address
 *****************************************************************************
 * Parameters:
 *  str          pointer to ascii string containing an address in internet
 *              dot notation. Supported formats:
 *                  a.b.c.d
 *                  a.b.c   (with c treated as 16 bits)
 *                  a.b     (with b treated as 24 bits)
 *                  a       (with a treated as 32 bits)
 * addr         Filled with the internet address.
 * Return value: 0 or NG_EINVAL for an invalid string
 */

int ngInetATON( const char *str, NGuint *addr)
{
  NGuint val;
  int n;
  char c;
  unsigned int parts[4];
  unsigned int *pp = parts;

  c = *str;
  for (;;) {
    /*
     * Collect number up to ``.''.
     */
    if (!_ISDIGIT(c))
      return( NG_EINVAL);
    val = 0;
    for (;;) {
      if( _ISDIGIT(c)) {
        val = (val * 10) + (c - '0');
        c = *++str;
      } else
        break;
    }
    if (c == '.') {
      if( (pp >= parts + 3) || (val > 0xff))
        return( NG_EINVAL);
      *pp++ = val;
      c = *++str;
    } else
      break;
  }
  /*
   * Check for trailing characters.
   */
  if ((c != '\0') && (c != ' '))
    return( NG_EINVAL);
  /*
   * Concoct the address according to
   * the number of parts specified.
   */
  n = pp - parts + 1;
  switch (n) {

  case 0:
    return( NG_EINVAL); /* initial nondigit */

  case 1: /* a -- 32 bits */
    break;

  case 2: /* a.b -- 8.24 bits */
    if (val > 0xffffffUL)
      return( NG_EINVAL);
    val |= ((NGuint) parts[0]) << 24;
    break;

  case 3: /* a.b.c -- 8.8.16 bits */
    if (val > 0xffffUL)
      return( NG_EINVAL);
    val |= (((NGuint) parts[0]) << 24) |
               (((NGuint) parts[1]) << 16);
    break;

  case 4: /* a.b.c.d -- 8.8.8.8 bits */
    if (val > 0xff)
      return( NG_EINVAL);
    val |= (((NGuint) parts[0]) << 24) |
               (((NGuint) parts[1]) << 16) |
               (((NGuint) parts[2]) << 8);
    break;
  }
  if( addr)
    *addr = ngHTONL( val);
  return( NG_EOK);
}

