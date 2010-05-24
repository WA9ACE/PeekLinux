#ifndef SYSDEFS_H
#define SYSDEFS_H



//#include "bal_def.h"

#define __align(x) // TODO: fixme neil
#define PACKED  // TODO: fixme neil
//#define INLINE 


//#undef __int64
//#define __int64 (long long)  //added by wangran

/*--------------------------------------------------------------------
* Define system wide data types
*--------------------------------------------------------------------*/
#ifndef BASIC_TYPES
#define BASIC_TYPES

#ifndef int8
#define int8 signed char
#endif

#ifndef int16
#define int16 signed short
#endif

#ifndef int32
#define int32 signed long
#endif

//#define int64 signed long long
#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned short
#endif

#ifndef uint32
#define uint32 unsigned long
#endif

#endif/*BASIC_TYPES*/

// TODO: check TI compiler if support int64. neil
typedef long long       __int64;
typedef unsigned long long   uint64;


/* Use of bitfields is discouraged for portability reasons. However,
 * in some cases it can simplify the code e.g. Hardware drivers.
 */ 
typedef int bitfld;

//#ifndef OTTS

//#undef FALSE
//#undef TRUE
#ifdef __cplusplus

#define FALSE   false
#define TRUE    true

typedef bool    boolean;

#else

#undef TRUE
#undef FALSE

typedef unsigned char bool;
#define TRUE  ((unsigned char)1)
#define FALSE ((unsigned char)0)

#endif

//#endif

#ifndef NULL
#define NULL 0
#endif


#include "ottsdefs.h"





/*----------------------------------------------------------------------------
* System Wide Definitions
*   The definitions below are those that are shared across all CP software.
*   By defining them in this file, each unit and/or API will not need to make
*   their own definition of these.
*
*   In order to be included in this list, the entity must have a well-defined
*   system definition.  For example, they can be drawn from IS-2000 standard
*   documents.
*----------------------------------------------------------------------------*/

/*
** Definition of CDMA system time
**   Kept in terms of a 20 msec frame count.
*/
typedef uint32 SysSystemTimeT;

/* define Cdma Band type */
typedef enum 
{
  SYS_BAND_CLASS_0,
  SYS_BAND_CLASS_1,
  SYS_BAND_CLASS_2,
  SYS_BAND_CLASS_3,
  SYS_BAND_CLASS_4,
  SYS_BAND_CLASS_5,
  SYS_BAND_CLASS_6,
  SYS_BAND_CLASS_7,
  SYS_BAND_CLASS_8,
  SYS_BAND_CLASS_9,
  SYS_BAND_CLASS_10,
  SYS_BAND_CLASS_NOT_USED 
} SysCdmaBandT; 

/* define Multiplex Options for the MAC Multiplex Layer */
typedef enum
{
   SYS_MUX_NULL     = 0,
   SYS_MUX_OPTION1  = 1,
   SYS_MUX_OPTION2,
   SYS_MUX_OPTION3,
   SYS_MUX_OPTION4,
   SYS_MUX_OPTION5,
   SYS_MUX_OPTION6,
   SYS_MUX_OPTION7,
   SYS_MUX_OPTION8,
   SYS_MUX_OPTION9,
   SYS_MUX_OPTION10,
   SYS_MUX_OPTION11,
   SYS_MUX_OPTION12,
   SYS_MUX_OPTION13,
   SYS_MUX_OPTION14,
   SYS_MUX_OPTION15,
   SYS_MUX_OPTION16,
   SYS_MUX_OPTION809 = 0x809,
   SYS_MUX_OPTION80A,
   SYS_MUX_OPTION811 = 0x811,
   SYS_MUX_OPTION812,
   SYS_MUX_OPTION821 = 0x821,
   SYS_MUX_OPTION822,
   SYS_MUX_OPTION905 = 0x905,
   SYS_MUX_OPTION906,
   SYS_MUX_OPTION909 = 0x909,
   SYS_MUX_OPTION90A,
   SYS_MUX_OPTION911 = 0x911,
   SYS_MUX_OPTION912,
   SYS_MUX_OPTION921 = 0x921,
   SYS_MUX_OPTION922
} SysMultiplexOptionT;


/* define types used in Service Config msgs from PSW to L1D and LMD */
typedef enum
{
  FCH_20MS_FRAME_SIZE,
  FCH_5_AND_20MS_FRAME_SIZE
} SysFchFrameSizeT;

typedef enum
{
  DCCH_FRAME_SIZE_RESERVED,
  DCCH_20MS_FRAME_SIZE,
  DCCH_5MS_FRAME_SIZE,
  DCCH_5_AND_20MS_FRAME_SIZE
} SysDcchFrameSizeT;

/* define array sizes for pilot lists */
#define SYS_CP_MAX_CANDIDATE_FREQ_LIST    20   /* array size for used and unused PN lists */
#define SYS_CP_MAX_ACTIVE_LIST_PILOTS      6   /* array size for active pilot list */ 
#define SYS_CP_MAX_CANDIDATE_LIST_PILOTS  10   /* array size for candidate list */ 
#define SYS_CP_MAX_NEIGHBOR_LIST_PILOTS   40   /* array size for neighbor pilot list */ 
#define SYS_CP_MAX_REMAINING_LIST_PILOTS  20   /* array size for Remaining pilot list */ 
#define SYS_CP_MAX_IDLE_PRIORITY_PILOTS   40   /* max number of pilot pn's in idle priority message */
#define SYS_CP_MAX_NUM_TC_ACTIVE_PILOTS    6   /* array size for active set list */
#define SYS_CP_MAX_ANALOG_FREQ_SEARCH      7   /* array size for analog candidate freq search list */
/* Amala K. 03/01/02 - AFLT */
#define SYS_CP_MAX_AFLT_LIST_PILOTS       40   /* max number of pilot pn's in position determination msg */
                                               /* array size for AFLT neighbor pilot list */


/* define maximum number of SCH supported by the physical layer */
#define SYS_MAX_FSCH 1  /* Maximum number of forward SCH */
#define SYS_MAX_RSCH 1  /* Maximum number of reverse SCH */

/* Define interrupt types used in SysIntEnable call */
typedef enum
{                 
   SYS_FIQ_INT  = 0x40,                 
   SYS_IRQ_INT  = 0x80,
   SYS_ALL_INT  = SYS_IRQ_INT | SYS_FIQ_INT                 
} SysIntT;


/************************************************************************* 
  defined types for RLP and LMD layers 
**************************************************************************/

/* 
   Valid is2000 sr_ids range from 0 to 7.  The additional sr_ids are for 
   internal use only. The null sr_id identifies there is no associated 
   rlp channel. The lmd sr_id is used by lmd during is95 calls.
*/
#define SYS_SR_ID_MAX         9                 
#define SYS_SR_ID_NULL        SYS_SR_ID_MAX

#if 1
#define SYS_SR_ID_IS95_LMD    1
#else
#define SYS_SR_ID_IS95_LMD    SYS_SR_ID_MAX - 1
#endif
#define SYS_SR_ID_IS2000_MAX  SYS_SR_ID_MAX - 2
#define SYS_SR_ID_IS95_PRIM   1
#define SYS_SR_ID_IS95_SEC    2

#define SYS_RLP_MAX_FRAME_SIZE 34   /* Max. size per RLP frame in byte.  */
									/* this number will be increased     */
typedef enum
{                              
   SYS_RLP_RATE_BLANK =  0,         /* blank rate                        */
   SYS_RLP_RATE_1_16,               /* 1/16  rate                        */
   SYS_RLP_RATE_1_8,                /* 1/8   rate                        */
   SYS_RLP_RATE_3_16,               /* 3/16  rate                        */
   SYS_RLP_RATE_1_4,                /* 1/4 rate                          */
   SYS_RLP_RATE_3_8,                /* rate 3/8                          */
   SYS_RLP_RATE_7_16,               /* rate 7/16                         */
   SYS_RLP_RATE_1_2,                /* rate 1/2                          */
   SYS_RLP_RATE_3_4,                /* rate 3/4                          */
   SYS_RLP_RATE_7_8,                /* rate 7/8                          */
   SYS_RLP_RATE_1,                  /* full rate                         */
   SYS_RLP_RATE_1x,                 /* 1x rate for SCH                   */
   SYS_RLP_RATE_2x,                 /* 2x rate for SCH                   */
   SYS_RLP_RATE_TOTAL,              /* number of rates                   */
   SYS_RLP_RATE_BAD = 255           /* erasure frame                     */
} SysRlpFrameRateT;

/* For setup AMPS or CDMA mode */
typedef enum
{	SYS_SET_CDMA_MODE = 0,
	SYS_SET_AMPS_MODE
}SysAmpsCdmaSelectT; 

typedef PACKED struct
{
   uint8 FrameBuf[SYS_RLP_MAX_FRAME_SIZE];  
} SysRlpFrameBufT;					

/************************************************************************* 
  defined types for PSW and MMI
**************************************************************************/
#define SYS_SYSTIME_SIZE         5     /* # bytes to hold 36 bits  */

/************************************************************************* 
  defined types for PSW and LMD
**************************************************************************/

#define MAX_STAT_ELEMENT         28 /* According to IS2000.3 rel 0 
                                       MAX_STAT_ELEMENT is 28       */
#define LMD_EACH_ON             1

typedef enum  
{
   MUX1_REV_FCH   = 0,
   MUX1_FOR_FCH,
   PAG,
   ACC,
   LAYER2_RTC,
   MUX2_REV_FCH,
   MUX2_FOR_FCH,
   SCCH_REV,
   SCCH_FOR,
   MUX1_REV_DCCH,
   MUX1_FOR_DCCH,
   MUX2_REV_DCCH,
   MUX2_FOR_DCCH,
   SCH0_REV,
   SCH1_REV,
   SCH0_FOR,
   SCH1_FOR,
   BCCH_TYPE,
   FCCCH_TYPE,
#ifdef LMD_EACH_ON
   EACH_BA,
#endif
   END_OF_GROUP_LIST
} StatCounterT;
#endif
