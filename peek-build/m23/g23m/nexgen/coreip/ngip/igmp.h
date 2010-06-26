/*****************************************************************************
 * $Id: igmp.h,v 1.2 2001/04/02 11:08:22 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Internet Group Management Protocol Definitions
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
 * 03/11/2000 - Regis Feneon
 *****************************************************************************/

#ifndef __NG_IGMP_H_INCLUDED__
#define __NG_IGMP_H_INCLUDED__

#include <ngip.h>

/* NG_BEGIN_DECLS // confuses Source Insight */

typedef struct {
  NGubyte igmp_type; /* version & type of message */
  NGubyte igmp_code; /* unused (zero) */
  NGushort igmp_cksum; /* checksum */
  NGuint igmp_group; /* group address (reports) */
} NGigmp;

/* type of messages */
#define IGMP_HOST_MEMBERSHIP_QUERY 0x11
#define IGMP_HOST_MEMBERSHIP_REPORT 0x12

#define IGMP_MINLEN 8

#define IGMP_MAX_HOST_REPORT_DELAY 10 /* 10 secondes max for report */

/* NG_END_DECLS // confuses Source Insight */

#endif

