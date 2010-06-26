/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_UICC_PHY_MAP_HEADER
#define BSP_UICC_PHY_MAP_HEADER


#include "bspUtil_BitUtil.h"
//#include "bspUtil_MemUtil.h"
//#include "bspCmap.h"
 
/*=============================================================================
 * Component Description:
 *    Register map for the BSP_UICC driver.
 */


/*=============================================================================
 * Constants
 
 #define BSP_UICC_PHY_MAP_BASE_ADDRESS   BSP_CMAP_PHYSICAL_BASE_ADDRESS_MPU_SIM*/

#define BSP_UICC_PHY_MAP_BASE_ADDRESS   0xFFFFA800


 /*============================================================================
 * Description:
 *  Define register offsets from the base address of USIM
 */
#define BSP_UICC_PHY_MAP_REG_USIM_CMD_OFFSET        (0x0000 * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_STAT_OFFSET       (0x0001 * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_CONF1_OFFSET      (0x0002 * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_CONF2_OFFSET      (0x0003 * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_CONF3_OFFSET      (0x0004 * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_IT_OFFSET         (0x0005 * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_DRX_OFFSET        (0x0006 * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_DTX_OFFSET        (0x0007 * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_MASKIT_OFFSET     (0x0008 * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_FIFOS_OFFSET      (0x0009 * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_CGT_OFFSET        (0x000A * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_CWT_OFFSET        (0x000B * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_BWT_LSB_OFFSET    (0x000C * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_BWT_MSB_OFFSET    (0x000D * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_DEBUG_REG_OFFSET        (0x000E * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_CONF_SAM1_DIV_OFFSET    (0x000F * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_CONF4_REG_OFFSET        (0x0010 * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_ATR_CLK_PRD_NBS_OFFSET  (0x0011 * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_CONF_ETU_DIV_OFFSET     (0x0012 * 2)
#define BSP_UICC_PHY_MAP_REG_USIM_CONF5_REG_OFFSET        (0x0013 * 2)


/*=============================================================================
 * Description:
 *  Define pointers to each of the registers.
 */
#define BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR                                                   \
(                                                                                           \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_UICC_PHY_MAP_BASE_ADDRESS,             \
                                                 BSP_UICC_PHY_MAP_REG_USIM_CMD_OFFSET )     \
)

#define BSP_UICC_PHY_MAP_REG_USIM_STAT_PTR                                                  \
(                                                                                           \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_UICC_PHY_MAP_BASE_ADDRESS,             \
                                                 BSP_UICC_PHY_MAP_REG_USIM_STAT_OFFSET )    \
)

#define BSP_UICC_PHY_MAP_REG_USIM_CONF1_PTR                                                 \
(                                                                                           \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_UICC_PHY_MAP_BASE_ADDRESS,             \
                                                 BSP_UICC_PHY_MAP_REG_USIM_CONF1_OFFSET )   \
)
 
#define BSP_UICC_PHY_MAP_REG_USIM_CONF2_PTR                                                 \
(                                                                                           \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_UICC_PHY_MAP_BASE_ADDRESS,             \
                                                 BSP_UICC_PHY_MAP_REG_USIM_CONF2_OFFSET )   \
)
 
#define BSP_UICC_PHY_MAP_REG_USIM_CONF3_PTR                                                 \
(                                                                                           \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_UICC_PHY_MAP_BASE_ADDRESS,             \
                                                 BSP_UICC_PHY_MAP_REG_USIM_CONF3_OFFSET )   \
)
 
#define BSP_UICC_PHY_MAP_REG_USIM_IT_PTR                                                    \
(                                                                                           \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_UICC_PHY_MAP_BASE_ADDRESS,             \
                                                 BSP_UICC_PHY_MAP_REG_USIM_IT_OFFSET )      \
)
 
#define BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR                                                \
(                                                                                           \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_UICC_PHY_MAP_BASE_ADDRESS,             \
                                                 BSP_UICC_PHY_MAP_REG_USIM_MASKIT_OFFSET )  \
)
 
#define BSP_UICC_PHY_MAP_REG_USIM_DRX_PTR                                                   \
(                                                                                           \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_UICC_PHY_MAP_BASE_ADDRESS,             \
                                                 BSP_UICC_PHY_MAP_REG_USIM_DRX_OFFSET )     \
)
 
#define BSP_UICC_PHY_MAP_REG_USIM_DTX_PTR                                                   \
(                                                                                           \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_UICC_PHY_MAP_BASE_ADDRESS,             \
                                                 BSP_UICC_PHY_MAP_REG_USIM_DTX_OFFSET )     \
)

#define BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR                                                 \
(                                                                                           \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_UICC_PHY_MAP_BASE_ADDRESS,             \
                                                 BSP_UICC_PHY_MAP_REG_USIM_FIFOS_OFFSET )   \
)
 
#define BSP_UICC_PHY_MAP_REG_USIM_CGT_PTR                                                   \
(                                                                                           \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_UICC_PHY_MAP_BASE_ADDRESS,             \
                                                 BSP_UICC_PHY_MAP_REG_USIM_CGT_OFFSET )     \
)
 
#define BSP_UICC_PHY_MAP_REG_USIM_CWT_PTR                                                   \
(                                                                                           \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_UICC_PHY_MAP_BASE_ADDRESS,             \
                                                 BSP_UICC_PHY_MAP_REG_USIM_CWT_OFFSET )     \
)
 
#define BSP_UICC_PHY_MAP_REG_USIM_BWT_LSB_PTR                                               \
(                                                                                           \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_UICC_PHY_MAP_BASE_ADDRESS,             \
                                                 BSP_UICC_PHY_MAP_REG_USIM_BWT_LSB_OFFSET ) \
)
 
#define BSP_UICC_PHY_MAP_REG_USIM_BWT_MSB_PTR                                               \
(                                                                                           \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( BSP_UICC_PHY_MAP_BASE_ADDRESS,             \
                                                 BSP_UICC_PHY_MAP_REG_USIM_BWT_MSB_OFFSET ) \
)

#define BSP_UICC_PHY_MAP_REG_USIM_DEBUG_REG_PTR            \
(                                                          \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR(           \
        BSP_UICC_PHY_MAP_BASE_ADDRESS,                     \
        BSP_UICC_PHY_MAP_REG_USIM_DEBUG_REG_OFFSET )       \
)

#define BSP_UICC_PHY_MAP_REG_USIM_CONF_SAM1_DIV_PTR        \
(                                                          \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR(           \
        BSP_UICC_PHY_MAP_BASE_ADDRESS,                     \
        BSP_UICC_PHY_MAP_REG_USIM_CONF_SAM1_DIV_OFFSET )   \
)

#define BSP_UICC_PHY_MAP_REG_USIM_CONF4_REG_PTR            \
(                                                          \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR(           \
        BSP_UICC_PHY_MAP_BASE_ADDRESS,                     \
        BSP_UICC_PHY_MAP_REG_USIM_CONF4_REG_OFFSET )       \
)

#define BSP_UICC_PHY_MAP_REG_USIM_ATR_CLK_PRD_NBS_PTR      \
(                                                          \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR(           \
        BSP_UICC_PHY_MAP_BASE_ADDRESS,                     \
        BSP_UICC_PHY_MAP_REG_USIM_ATR_CLK_PRD_NBS_OFFSET ) \
)
    
#define BSP_UICC_PHY_MAP_REG_USIM_CONF_ETU_DIV_PTR         \
(                                                          \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR(           \
        BSP_UICC_PHY_MAP_BASE_ADDRESS,                     \
        BSP_UICC_PHY_MAP_REG_USIM_CONF_ETU_DIV_OFFSET )    \
)
    
#define BSP_UICC_PHY_MAP_REG_USIM_CONF5_REG_PTR            \
(                                                          \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR(           \
        BSP_UICC_PHY_MAP_BASE_ADDRESS,                     \
        BSP_UICC_PHY_MAP_REG_USIM_CONF5_REG_OFFSET )       \
)
#endif





