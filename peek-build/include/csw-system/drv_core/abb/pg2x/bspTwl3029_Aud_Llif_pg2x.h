/*=============================================================================
 *    Copyright 2005 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_TWL3029_AUD_LLIF_PG2X_HEADER
#define BSP_TWL3029_AUD_LLIF_PG2X_HEADER

#include "bspUtil_BitUtil.h"

/*=============================================================================
 * Component Description:
 *    Provides accessor and manipulator macros for TWL3029 aka Triton
 *    register fields for BARPWR module. Fields offsets and widths are encoded in contants
 *    that are to be fed into simple macros. This common approach should
 *    help to prevent masking errors.
 *
 *    the BARINT registers consist of: 
 *     -  RTC registers
 *     -  power master registers registers
 *     -  power slave registers
 */

/*=========BSP_TWL3029_LLIF_AUDIO_TOGB=========
 */

/*====BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS========
 */
#define BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_RSVD_OFFSET  4 
#define BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_RSVD_WIDTH   4

#define BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_SPKON_OFFSET  3 
#define BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_SPKON_WIDTH   1
/*=========BSP_TWL3029_LLIF_AUDIO_CTRL1=========
 */

/*=========BSP_TWL3029_LLIF_AUDIO_CTRL2=========
 */

/*=========BSP_TWL3029_LLIF_AUDIO_CTRL3=========
 */


/*=========BSP_TWL3029_LLIF_AUDIO_CTRL4=========
 */

/*=========BSP_TWL3029_LLIF_AUDIO_CTRL5=========
 */
 

#define BSP_TWL3029_LLIF_AUDIO_CTRL5_RSVD_OFFSET    7
#define BSP_TWL3029_LLIF_AUDIO_CTRL5_RSVD_WIDTH     1

#define BSP_TWL3029_LLIF_AUDIO_CTRL5_I2SLRJ_OFFSET  6
#define BSP_TWL3029_LLIF_AUDIO_CTRL5_I2SLRJ_WIDTH   1

/*=========BSP_TWL3029_LLIF_AUDIO_CTRL6=========
 */


/*=========BSP_TWL3029_LLIF_AUDIO_POPMAN=========
 */


/*=========BSP_TWL3029_LLIF_AUDIO_POPAUTO=========
*/

/*=========BSP_TWL3029_LLIF_AUDIO_VFTEST=========
 */  
#define BSP_TWL3029_LLIF_AUDIO_VFTEST_RSVD_OFFSET  3
#define BSP_TWL3029_LLIF_AUDIO_VFTEST_RSVD_WIDTH   5

#define BSP_TWL3029_LLIF_AUDIO_VFTEST_I2SVSPMUX_OFFSET  2
#define BSP_TWL3029_LLIF_AUDIO_VFTEST_I2SVSPMUX_WIDTH   1

#define BSP_TWL3029_LLIF_AUDIO_VFTEST_I2SINVSCK_OFFSET  1
#define BSP_TWL3029_LLIF_AUDIO_VFTEST_I2SINVSCK_WIDTH   1

#define BSP_TWL3029_LLIF_AUDIO_VFTEST_I2SLOOP_OFFSET  0
#define BSP_TWL3029_LLIF_AUDIO_VFTEST_I2SLOOP_WIDTH   1

/*=========BSP_TWL3029_LLIF_AUDIO_AUDFTEST3=========
*/ 


#define BSP_TWL3029_LLIF_AUDIO_AUDFTEST3_RSVD_0_OFFSET  0
#define BSP_TWL3029_LLIF_AUDIO_AUDFTEST3_RSVD_0_WIDTH   2

/*=========BSP_TWL3029_LLIF_AUDIO_HFTEST1=========
*/ 


#define BSP_TWL3029_LLIF_AUDIO_HFTEST1_SPKCLKDIV2Z_OFFSET   1
#define BSP_TWL3029_LLIF_AUDIO_HFTEST1_SPKCLKDIV2Z_WIDTH    1




#endif
