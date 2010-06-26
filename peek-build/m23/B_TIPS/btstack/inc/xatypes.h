#ifndef __XATYPES_H
#define __XATYPES_H
/***************************************************************************
 *
 * File:
 *     $Workfile$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision$
 *
 * Description:
 *     This file contains declarations for iAnywhere (formerly iAnywhere)
 *     protocol stacks.
 *
 * Created:
 *     November 19, 1996
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "config.h"

#if TI_CHANGES == XA_ENABLED

#include "bthal_types.h"

/****************************************************************************
 *
 * Base definitions used by the stack and framer
 *
 ****************************************************************************/
 
#ifdef BTHAL_XATYPES_CONFLICT

#else

typedef BTHAL_U32 	U32;
typedef BTHAL_U16	U16;
typedef BTHAL_U8	U8;

typedef  BTHAL_S32	S32;
typedef  BTHAL_S16	S16;
typedef  BTHAL_S8	S8;

/* Boolean Definitions */
typedef BTHAL_BOOL	BOOL;

#endif /* (BTHAL_XATYPES_CONFLICT == BTHAL_TRUE) */

/* Variable sized integers. Used to optimize processor efficiency by
 * using the most efficient data size for counters, arithmetic, etc.
 */

typedef BTHAL_I16	I16;
typedef BTHAL_I8		I8;
typedef BTHAL_I32  	I32;

#ifndef TRUE
#define 	TRUE		BTHAL_TRUE	
#endif

#ifndef FALSE
#define 	FALSE		BTHAL_FALSE
#endif

typedef void (*PFV) (void);

#else		/* TI_CHANGES == XA_ENABLED */

/****************************************************************************
 *
 * Base definitions used by the stack and framer
 *
 ****************************************************************************/

#ifndef BOOL_DEFINED
typedef int BOOL;   /* IGNORESTYLE */
#endif

typedef unsigned long  U32;
typedef unsigned short U16;
typedef unsigned char  U8;

typedef          long  S32;
typedef          short S16;
typedef          char  S8;

/* Variable sized integers. Used to optimize processor efficiency by
 * using the most efficient data size for counters, arithmatic, etc.
 */
typedef unsigned long  I32;

#if XA_INTEGER_SIZE == 4
typedef unsigned long  I16;
typedef unsigned long  I8;
#elif XA_INTEGER_SIZE == 2
typedef unsigned short I16;
typedef unsigned short I8;
#elif XA_INTEGER_SIZE == 1
typedef unsigned short I16;
typedef unsigned char  I8;
#else
#error No XA_INTEGER_SIZE specified!
#endif

typedef void (*PFV) (void);

/* Boolean Definitions */
#ifndef TRUE
#define TRUE  (1==1)
#endif /* TRUE */

#ifndef FALSE
#define FALSE (0==1)  
#endif /* FALSE */

#endif /* TI_CHANGES */

#endif /* __XATYPES_H */



