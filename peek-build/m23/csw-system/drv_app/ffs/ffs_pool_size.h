/**
 * @file	ffs_pool_size.h
 *
 * Declarations of:
 * - the memory bank sizes and their watermark
 * - the SWE stack size
 * - the pool size needed (generally the sum of memory bank and stack sizes)
 */

/*
 * History:
 *
 *	Date       	Author              Modification
 *	-------------------------------------------------------------------
 *	12/03/2004	Frederic Maria      Extracted from rvf_pool_size.h
 *
 * (C) Copyright 2004 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __FFS_POOL_SIZE_H_
#define __FFS_POOL_SIZE_H_


#ifndef _WINDOWS
  #include "config/swconfig.cfg"
#endif
#include "rv/rv_defined_swe.h"


/*
 * Values used in ffs_env.h
 */
#define FFS_STACK_SIZE   (8192+512)
#define FFS_MAILBUF_SIZE (1024)      // Default: Max 20 pending FFS mails.

/* FFS_STREAMBUF_SIZE:
   Maximum number of open files (fs.fd_max) * Chunk size (fs.fd_buf_size) */
#if (!GSMLITE)
    #ifdef RVM_MSFE_SWE
        #define FFS_STREAMBUF_SIZE (40960)
    #else
#if	(LOCOSTO_LITE)
        #define FFS_STREAMBUF_SIZE (2048)
#else
        #define FFS_STREAMBUF_SIZE (8192)
#endif // LOCOSTO_LITE
    #endif
#else
    #define FFS_STREAMBUF_SIZE (2048)
#endif

#define FFS_MB1_SIZE   (FFS_MAILBUF_SIZE + FFS_STREAMBUF_SIZE)
#define FFS_POOL_SIZE  (FFS_STACK_SIZE + FFS_MB1_SIZE)


#endif /*__FFS_POOL_SIZE_H_*/
