/*******************************************************************************\
##                                                                           	*
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
##																				*
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
##																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      bthal_types.h
*
*   BRIEF:    		Definitions for basic BTHAL Types.
*
*   DESCRIPTION:	This file defines the BASIC bthal types. These would be used
*					as base types for upper layers (such as ESI stack etc.)
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/

#ifndef __BTHAL_TYPES_H
#define __BTHAL_TYPES_H

/* -------------------------------------------------------------
 *					Platform-Depndent Part							
 *																
 * 		SET THE VALUES OF THE FOLLOWING PRE-PROCESSOR 			
 *		DEFINITIONS 	TO THE VALUES THAT APPLY TO THE 				
 *		TARGET PLATFORM											
 *																
 */

/* Size of type (char) in the target platform, in bytes	*/
#define BTHAL_CHAR_SIZE	(1)

/* Size of type (short) in the target platform, in bytes */
#define BTHAL_SHORT_SIZE 	(2)

/* Size of type (long) in the target platform, in bytes	*/
#define BTHAL_LONG_SIZE 	(4)

/* Size of type (int) in the target platform, in bytes	*/
#define BTHAL_INT_SIZE 		(4)

/* -------------------------------------------------------------
 *					8 Bits Types
 */
#if BTHAL_CHAR_SIZE == 1

typedef unsigned char	 	BTHAL_U8;
typedef signed char 		BTHAL_S8;

#elif BTHAL_SHORT_SIZE == 1

typedef unsigned short 	BTHAL_U8;
typedef          short 		BTHAL_S8;

#elif BTHAL_INT_SIZE == 1

typedef unsigned int 		BTHAL_U8;
typedef          int 			BTHAL_S8;

#else

#error Unable to define 8-bits basic types!

#endif

/* -------------------------------------------------------------
 *					16 Bits Types
 */
#if BTHAL_SHORT_SIZE == 2

typedef unsigned short 	BTHAL_U16;
typedef          short 		BTHAL_S16;

#elif BTHAL_INT_SIZE == 2

typedef unsigned int 		BTHAL_U16;
typedef          int 			BTHAL_S16;

#else

#error Unable to define 16-bits basic types!

#endif

/* -------------------------------------------------------------
 *					32 Bits Types
 */
#if BTHAL_LONG_SIZE == 4

typedef unsigned long 	BTHAL_U32;
typedef          long 	BTHAL_S32;

#elif BTHAL_INT_SIZE == 4

typedef unsigned int 	BTHAL_U32;
typedef          int 		BTHAL_S32;

#else

#error Unable to define 32-bits basic types!

#endif

/* -------------------------------------------------------------
 *			Native Integer Types (# of bits irrelevant)
 */
typedef int			BTHAL_INT;
typedef unsigned int	BTHAL_UINT;

	
/* -------------------------------------------------------------
 *					Types for Performance							
 *																
 *	Variable sized integers. Used to optimize processor efficiency by		
 *  using the most efficient data size for counters, arithmatic, etc.			
 */
typedef unsigned long  BTHAL_I32;

#if BTHAL_INT_SIZE == 4

typedef unsigned long  BTHAL_I16;
typedef unsigned long  BTHAL_I8;

#elif BTHAL_INT_SIZE == 2

typedef unsigned short BTHAL_I16;
typedef unsigned short BTHAL_I8;

#elif BTHAL_INT_SIZE == 1

typedef unsigned short BTHAL_I16;
typedef unsigned char  BTHAL_I8;

#else

#error Unsupported BTHAL_INT_SIZE Value!

#endif

/* --------------------------------------------------------------
 *					Boolean Definitions							 
 */
typedef unsigned char BTHAL_BOOL;

#define BTHAL_TRUE  (1 == 1)
#define BTHAL_FALSE (0==1) 

/* --------------------------------------------------------------
 *					Null Definition							 
 */
#ifndef NULL
#define NULL    0
#endif


/* -------------------------------------------------------------
 *					Platform Dependent Definitions							
 *																
* 		For Platforms where there is a conflict between the BT stack definitions 			
*		and platform definitions  the developer can use the following 
*		BTHAL_XATYPES_CONFLICT definition in order to take the definitions from 
*		the platform instead of the BT stack.
*
*		In windows we don't define this definition  since we don't have a conflict.  
* 
*		e.g. in Locosto we add theses two lines:
* 		#include "typedefs.h" 
* 		#define BTHAL_XATYPES_CONFLICT 
 *
 */


#include "typedefs.h"
#include "vsi.h"
#define BTHAL_XATYPES_CONFLICT  


#endif /* __BTHAL_TYPES_H */

