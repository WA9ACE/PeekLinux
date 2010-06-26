#ifndef __FM_TYPES_H
#define __FM_TYPES_H


/* -------------------------------------------------------------
 *					Platform-Depndent Part							
 *																
 * 		SET THE VALUES OF THE FOLLOWING PRE-PROCESSOR 			
 *		DEFINITIONS 	TO THE VALUES THAT APPLY TO THE 				
 *		TARGET PLATFORM											
 *																
 */

/* Size of type (char) in the target platform, in bytes	*/
#define FM_CHAR_SIZE	(1)

/* Size of type (short) in the target platform, in bytes */
#define FM_SHORT_SIZE 	(2)

/* Size of type (long) in the target platform, in bytes	*/
#define FM_LONG_SIZE 	(4)

/* Size of type (int) in the target platform, in bytes	*/
#define FM_INT_SIZE 	(4)

/* -------------------------------------------------------------
 *					8 Bits Types
 */
#if FM_CHAR_SIZE == 1

typedef unsigned char	 	TIFM_U8;
typedef signed char 		TIFM_S8;

#elif FM_SHORT_SIZE == 1

typedef unsigned short 		TIFM_U8;
typedef          short 		TIFM_S8;

#elif FM_INT_SIZE == 1

typedef unsigned int 		TIFM_U8;
typedef          int 		TIFM_S8;

#else

#error Unable to define 8-bits basic types!

#endif

/* -------------------------------------------------------------
 *					16 Bits Types
 */
#if FM_SHORT_SIZE == 2

typedef unsigned short		TIFM_U16;
typedef          short 		TIFM_S16;

#elif FM_INT_SIZE == 2

typedef unsigned int 		TIFM_U16;
typedef          int 		TIFM_S16;

#else

#error Unable to define 16-bits basic types!

#endif

/* -------------------------------------------------------------
 *					32 Bits Types
 */
#if FM_LONG_SIZE == 4

typedef unsigned long 	TIFM_U32;
typedef          long 	TIFM_S32;

#elif FM_INT_SIZE == 4

typedef unsigned int 	TIFM_U32;
typedef          int 	TIFM_S32;

#else

#error Unable to define 32-bits basic types!

#endif

/* -------------------------------------------------------------
 *			Native Integer Types (# of bits irrelevant)
 */
typedef int				TIFM_INT;
typedef unsigned int	TIFM_UINT;

	
/* -------------------------------------------------------------
 *					Types for Performance							
 *																
 *	Variable sized integers. Used to optimize processor efficiency by		
 *  using the most efficient data size for counters, arithmatic, etc.			
 */
typedef unsigned long  TIFM_I32;

#if FM_INT_SIZE == 4

typedef unsigned long  TIFM_I16;
typedef unsigned long  TIFM_I8;

#elif FM_INT_SIZE == 2

typedef unsigned short TIFM_I16;
typedef unsigned short TIFM_I8;

#elif FM_INT_SIZE == 1

typedef unsigned short TIFM_I16;
typedef unsigned char  TIFM_I8;

#else

#error Unsupported FM_INT_SIZE Value!

#endif

/* --------------------------------------------------------------
 *					Boolean Definitions							 
 */

/* --------------------------------------------------------------
 *					Boolean Definitions							 
 */
typedef int TIFM_BOOL;

#define TIFM_TRUE  (1 == 1)
#define TIFM_FALSE (0 == 1) 

/* --------------------------------------------------------------
 *					Null Definition							 
 */
#ifndef NULL
#define NULL    0
#endif		




/* FM statuses */
typedef TIFM_U8 FmStatus;

#define FM_STATUS_SUCCESS							0  /* Successful and complete */
#define FM_STATUS_FAILED							1  /* Operation failed */
#define FM_STATUS_PENDING							2  /* Successfully started but pending */
#define FM_STATUS_INVALID_PARM						3
#define FM_STATUS_BT_NOT_INITIALIZED		        4  /* Operation failed because fm is not initialized yet */
#define FM_STATUS_FAILED_FM_NOT_ON			        5 /* Operation failed because fm is not initialized yet */
#define FM_STATUS_FM_ALREADY_ON				        6  /* FM was already initialized */
#define FM_STATUS_SEEK_REACHED_BAND_LIMIT			7
#define FM_STATUS_SEEK_STOPPED						8
#define FM_STATUS_SEEK_SUCCESS						9
#define FM_STATUS_STOP_SEEK							10	/* Internal use */
#define FM_STATUS_PENDING_UPDATE_CMD_PARAMS			11
#define FM_STATUS_FAILED_ALREADY_PENDING			12
#define FM_STATUS_INVALID_TYPE						13
#define FM_STATUS_HCI_INIT_ERR						14
#define FM_STATUS_IN_PROGRESS						15
#define FM_STATUS_INTERNAL_ERROR					16
#define FM_STATUS_FAIL_RDS_OFF					17
#define FM_STATUS_FAIL_NO_VALUE_AVAILABLE			18


#define FM_NO_VALUE									0xff


#endif	/* __FM_TYPES_H */

