/*****************************************************************************
 * $Id: tcp_data.c,v 1.1 2001/05/28 16:28:23 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Transmission Control Protocol - Global Data
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
 * 28/05/2001 - Regis Feneon
 *  moved from tcp.c
 *****************************************************************************/

#include <ngip.h>
#include <ngtcp.h>
#include <ngdsock.h>

/*
 * Global TCP variables
 */
NGuint ngTcp_Iss;
NGsock *ngTcp_LastSock;
NGnode ngTcp_Sockq; /* socket in use */
NGqueue ngTcp_TcpcbFreeq; /* free TCP control blocks */
int ngTcpBufDataOffset;
#ifdef NG_IPSTATS_SUPPORTED
NGtcpstat ngTcpStat; /* statistics */
#endif

