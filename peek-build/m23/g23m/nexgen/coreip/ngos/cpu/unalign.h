/*****************************************************************************
 * $Id: unalign.h,v 1.3.2.1 2002/10/04 14:35:58 rf Exp $
 * $Name: rel_1_3_b_10 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * CPU files - Unaligned data handling macros
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
 * 19/07/2000 - Regis Feneon
 * 04/10/2002 - LV
 *  ngURDXX() macros: casts was missing (16-bits architectures)
 *****************************************************************************/

#ifndef __NG_UNALIGN_H_INCLUDED__
#define __NG_UNALIGN_H_INCLUDED__

/* unaligned data types */
typedef NGubyte NGuuint16[2];
typedef NGubyte NGuuint32[4];

/* unaligned data access */
#ifndef NG_ALIGN_STRICT
/* processor can directly access unaligned data */
#define ngURD16( ptr) (*((NGushort *)(ptr)))
#define ngURD32( ptr) (*((NGuint *)(ptr)))
#define ngUWR16( ptr, val) (*((NGushort *)(ptr)) = ((NGushort)val))
#define ngUWR32( ptr, val) (*((NGuint *)(ptr)) = ((NGuint)val))
#define ngURDLE16( ptr) ngLETOH16(ngURD16((ptr)))
#define ngURDLE32( ptr) ngLETOH32(ngURD32((ptr)))
#define ngURDBE16( ptr) ngBETOH16(ngURD16((ptr)))
#define ngURDBE32( ptr) ngBETOH32(ngURD32((ptr)))
#define ngUWRLE16( ptr, val) ngUWR16((ptr),ngHTOLE16((val)))
#define ngUWRLE32( ptr, val) ngUWR32((ptr),ngHTOLE32((val)))
#define ngUWRBE16( ptr, val) ngUWR16((ptr),ngHTOBE16((val)))
#define ngUWRBE32( ptr, val) ngUWR32((ptr),ngHTOBE32((val)))
#else
/* processor cannot access unaligned data */
#define ngURDLE16( ptr) ((NGushort) ((NGushort) ((NGubyte *)(ptr))[0]| \
                                    ((NGushort)(((NGubyte *)(ptr))[1])<<8)))
#define ngURDLE32( ptr) ((NGuint)((NGuint) ((NGubyte *)(ptr))[0] | \
                                 ((NGuint)(((NGubyte *)(ptr))[1])<<8) | \
                                 ((NGuint)(((NGubyte *)(ptr))[2])<<16) | \
                                 ((NGuint)(((NGubyte *)(ptr))[3])<<24) ))
#define ngURDBE16( ptr) ((NGushort) ((NGushort) ((NGubyte *)(ptr))[1]| \
                                    ((NGushort)(((NGubyte *)(ptr))[0])<<8)))
#define ngURDBE32( ptr) ((NGuint)((NGuint) ((NGubyte *)(ptr))[3] | \
                                 ((NGuint)(((NGubyte *)(ptr))[2])<<8) | \
                                 ((NGuint)(((NGubyte *)(ptr))[1])<<16) | \
                                 ((NGuint)(((NGubyte *)(ptr))[0])<<24) ))
#define ngUWRLE16( ptr, val) { ((NGubyte *)(ptr))[0] = (NGubyte)((NGushort)val); \
                                ((NGubyte *)(ptr))[1] = (NGubyte)((NGushort)val>>8); }
#define ngUWRLE32( ptr, val) { ((NGubyte *)(ptr))[0] = (NGubyte)((NGuint)val); \
                                ((NGubyte *)(ptr))[1] = (NGubyte)((NGuint)val>>8); \
                                ((NGubyte *)(ptr))[2] = (NGubyte)((NGuint)val>>16); \
                                ((NGubyte *)(ptr))[3] = (NGubyte)((NGuint)val>>24); }
#define ngUWRBE16( ptr, val) { ((NGubyte *)(ptr))[1] = (NGubyte)((NGushort)val); \
                                ((NGubyte *)(ptr))[0] = (NGubyte)((NGushort)val>>8); }
#define ngUWRBE32( ptr, val) { ((NGubyte *)(ptr))[3] = (NGubyte)((NGuint)val); \
                                ((NGubyte *)(ptr))[2] = (NGubyte)((NGuint)val>>8); \
                                ((NGubyte *)(ptr))[1] = (NGubyte)((NGuint)val>>16); \
                                ((NGubyte *)(ptr))[0] = (NGubyte)((NGuint)val>>24); }
#ifdef NG_LITTLE_ENDIAN
#define ngURD16( ptr) ngURDLE16((ptr))
#define ngURD32( ptr) ngURDLE32((ptr))
#define ngUWR16( ptr, val) ngUWRLE16((ptr),(val))
#define ngUWR32( ptr, val) ngUWRLE32((ptr),(val))
#endif
#ifdef NG_BIG_ENDIAN
#define ngURD16( ptr) ngURDBE16((ptr))
#define ngURD32( ptr) ngURDBE32((ptr))
#define ngUWR16( ptr, val) ngUWRBE16((ptr),(val))
#define ngUWR32( ptr, val) ngUWRBE32((ptr),(val))
#endif
#endif /* NG_ALIGN_STRICT */

#endif

