/*=============================================================================
 * Copyright 2002-2004 Texas Instruments Inc. All rights reserved.
 */

#ifndef BSP_USBLH_OTG_MAP_HEADER
#define BSP_USBLH_OTG_MAP_HEADER

/*=============================================================================
 * File Contents:
 *    Register map for the OTG version 2.x USB Local Host controller.
 */
#include "bspUtil_BitUtil.h"
#include "bspUtil_MemUtil.h"


/*=============================================================================
 * Constants
 */

/*=============================================================================
 * Description:
 *  Define the base address of Memory Interface.
 */
#define BSP_LOCOSTO_CONFIGREG_MAP_BASE_ADDRESS  0xFFFEF000

/*=============================================================================
 * Description:
 *  Define offsets to each of the registers.
 */
#define BSP_LOCOSTO_CONFIGREG_OFFSET			 0x1C
/*=============================================================================
 * Utility Macros
 */


#define BSP_LOCOSTO_CORE_CONFIG_REG_PTR                                             \
(                                                                              \
   BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_LOCOSTO_CONFIGREG_MAP_BASE_ADDRESS,\
       BSP_LOCOSTO_CONFIGREG_OFFSET )                                         \
)

#define BSP_LOCOSTO_USBCLK_REG_PTR  			\
(				\
	BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR(0xFFFFFD00,\
	0x0C)							\
)

#endif
