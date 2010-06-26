/*=============================================================================
 *    Copyright 2005 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_TWL3029_PWR_LLIF_PG2X_HEADER
#define BSP_TWL3029_PWR_LLIF_PG2X_HEADER

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
 *
 *    defines are specific to hardware version PG2.x only.
 */
 
/*=========BSP_TWL3029_LLIF_PMC_MASTER_CFG_PU_PD_MSB=========
*/


/*=========BSP_TWL3029_LLIF_PMC_MASTER_CLOCK_GATING=========
*/ 
 
#define BSP_TWL3029_LLIF_PMC_MASTER_CLOCK_GATING_GATED_OFFSET            0
#define BSP_TWL3029_LLIF_PMC_MASTER_CLOCK_GATING_GATED_WIDTH             8 
 

/*=========BSP_TWL3029_LLIF_PMC_SLAVE_VCORE_CFG_VSEL=========
*/
#define BSP_TWL3029_LLIF_PMC_SLAVE_VCORE_CFG_VSEL_VRDBB_SEL_OFFSET       0   
#define BSP_TWL3029_LLIF_PMC_SLAVE_VCORE_CFG_VSEL_VRDBB_SEL_WIDTH        4

/*=========BSP_TWL3029_LLIF_PMC_SLAVE_VCORE_CFG_ROOF=========
 */
#define BSP_TWL3029_LLIF_PMC_SLAVE_VCORE_CFG_ROOF_VROOF_OFFSET           0  
#define BSP_TWL3029_LLIF_PMC_SLAVE_VCORE_CFG_ROOF_VROOF_WIDTH            4
 
/*=========BSP_TWL3029_LLIF_PMC_SLAVE_VCORE_CFG_FLOOR=========
 */
#define BSP_TWL3029_LLIF_PMC_SLAVE_VCORE_CFG_FLOOR_VFLOOR_OFFSET         0 
#define BSP_TWL3029_LLIF_PMC_SLAVE_VCORE_CFG_FLOOR_VFLOOR_WIDTH          4
  

#endif
