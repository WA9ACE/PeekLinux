/*=============================================================================
 *    Copyright 2003-2004 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_I2C_MULTI_MASTER_MAP_HEADER
#define BSP_I2C_MULTI_MASTER_MAP_HEADER

#include "chipset.cfg"
#include "bspUtil_BitUtil.h"
#include "bspUtil_MemUtil.h"
#include "sys_map.h"
 
/*=============================================================================
 * Component Description:
 *    Register map for I2C Master system block.
 */


/*=============================================================================
 * Constants
 */
/*=============================================================================
 * Description:
 *  Define the base address of I2C_MULTI_MASTER.
 */
#if  (CHIPSET==15)
#define BSP_I2C1_MULTI_MASTER_MAP_BASE_ADDRESS  C_MAP_I2C_BASE
#define BSP_I2C2_MULTI_MASTER_MAP_BASE_ADDRESS  C_MAP_I2C_TRITON_BASE
#else
#error "Not Supported this configuration"
#endif

/*=============================================================================
 * Description:
 *  Define offsets from the base address for each register.
 */
#if (CHIPSET==15)
#define BSP_I2C_MULTI_MASTER_MAP_REV_OFFSET   0x00
#define BSP_I2C_MULTI_MASTER_MAP_IE_OFFSET    0x02
#define BSP_I2C_MULTI_MASTER_MAP_STAT_OFFSET  0x04
#define BSP_I2C_MULTI_MASTER_MAP_IV_OFFSET    0x06
#define BSP_I2C_MULTI_MASTER_MAP_RSVD1_OFFSET 0x08
#define BSP_I2C_MULTI_MASTER_MAP_BUF_OFFSET   0x0A
#define BSP_I2C_MULTI_MASTER_MAP_CNT_OFFSET   0x0C
#define BSP_I2C_MULTI_MASTER_MAP_DATA_OFFSET  0x0E
#define BSP_I2C_MULTI_MASTER_MAP_RSVD2_OFFSET 0x10
#define BSP_I2C_MULTI_MASTER_MAP_CON_OFFSET   0x12
#define BSP_I2C_MULTI_MASTER_MAP_OA_OFFSET    0x14
#define BSP_I2C_MULTI_MASTER_MAP_SA_OFFSET    0x16
#define BSP_I2C_MULTI_MASTER_MAP_PSC_OFFSET   0x18
#define BSP_I2C_MULTI_MASTER_MAP_SCLL_OFFSET  0x1A
#define BSP_I2C_MULTI_MASTER_MAP_SCLH_OFFSET  0x1C
#define BSP_I2C_MULTI_MASTER_MAP_TEST_OFFSET  0x1E
#else
#define BSP_I2C_MULTI_MASTER_MAP_REV_OFFSET   0x00
#define BSP_I2C_MULTI_MASTER_MAP_IE_OFFSET    0x04
#define BSP_I2C_MULTI_MASTER_MAP_STAT_OFFSET  0x08
#define BSP_I2C_MULTI_MASTER_MAP_IV_OFFSET    0x0C
#define BSP_I2C_MULTI_MASTER_MAP_RSVD1_OFFSET 0x10
#define BSP_I2C_MULTI_MASTER_MAP_BUF_OFFSET   0x14
#define BSP_I2C_MULTI_MASTER_MAP_CNT_OFFSET   0x18
#define BSP_I2C_MULTI_MASTER_MAP_DATA_OFFSET  0x1C
#define BSP_I2C_MULTI_MASTER_MAP_RSVD2_OFFSET 0x20
#define BSP_I2C_MULTI_MASTER_MAP_CON_OFFSET   0x24
#define BSP_I2C_MULTI_MASTER_MAP_OA_OFFSET    0x28
#define BSP_I2C_MULTI_MASTER_MAP_SA_OFFSET    0x2C
#define BSP_I2C_MULTI_MASTER_MAP_PSC_OFFSET   0x30
#define BSP_I2C_MULTI_MASTER_MAP_SCLL_OFFSET  0x34
#define BSP_I2C_MULTI_MASTER_MAP_SCLH_OFFSET  0x38
#define BSP_I2C_MULTI_MASTER_MAP_TEST_OFFSET  0x3C
#endif
#if (CHIPSET==15)
/*=============================================================================
 * Description:
 *  Define pointers to each of the registers.
 */
#define BSP_I2C_MULTI_MASTER_MAP_REV_PTR(_baseAddress)                                 \
(                                                                                       \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( (_baseAddress), 			\
                                                 BSP_I2C_MULTI_MASTER_MAP_REV_OFFSET )  \
)

#define BSP_I2C_MULTI_MASTER_MAP_IE_PTR(_baseAddress)                                   \
(                                                                                       \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR(  (_baseAddress), 			\
                                                 BSP_I2C_MULTI_MASTER_MAP_IE_OFFSET )   \
)

#define BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(_baseAddress)                                 \
(                                                                                       \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( (_baseAddress), 			\
                                                 BSP_I2C_MULTI_MASTER_MAP_STAT_OFFSET ) \
)

#define BSP_I2C_MULTI_MASTER_MAP_IV_PTR(_baseAddress)                                   \
(                                                                                       \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( (_baseAddress),			\
                                                 BSP_I2C_MULTI_MASTER_MAP_IV_OFFSET )   \
)

#define BSP_I2C_MULTI_MASTER_MAP_BUF_PTR(_baseAddress)                                  \
(                                                                                       \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( (_baseAddress),			 \
                                                 BSP_I2C_MULTI_MASTER_MAP_BUF_OFFSET )  \
)

#define BSP_I2C_MULTI_MASTER_MAP_CNT_PTR(_baseAddress)                                  \
(                                                                                       \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( (_baseAddress),			 \
                                                 BSP_I2C_MULTI_MASTER_MAP_CNT_OFFSET )  \
)

#define BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(_baseAddress)                                 \
(                                                                                       \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( (_baseAddress),			 \
                                                 BSP_I2C_MULTI_MASTER_MAP_DATA_OFFSET ) \
)


#define BSP_I2C_MULTI_MASTER_MAP_CON_PTR(_baseAddress)                                  \
(                                                                                       \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( (_baseAddress),			 \
                                                 BSP_I2C_MULTI_MASTER_MAP_CON_OFFSET )  \
)

#define BSP_I2C_MULTI_MASTER_MAP_OA_PTR(_baseAddress)                                   \
(                                                                                       \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( (_baseAddress),			 \
                                                 BSP_I2C_MULTI_MASTER_MAP_OA_OFFSET )   \
)

#define BSP_I2C_MULTI_MASTER_MAP_SA_PTR(_baseAddress)                                   \
(                                                                                       \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( (_baseAddress),			 \
                                                 BSP_I2C_MULTI_MASTER_MAP_SA_OFFSET )   \
)

#define BSP_I2C_MULTI_MASTER_MAP_PSC_PTR(_baseAddress)                                  \
(                                                                                       \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( (_baseAddress),			 \
                                                 BSP_I2C_MULTI_MASTER_MAP_PSC_OFFSET )  \
)

#define BSP_I2C_MULTI_MASTER_MAP_SCLL_PTR(_baseAddress)                                 \
(                                                                                       \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( (_baseAddress),			 \
                                                 BSP_I2C_MULTI_MASTER_MAP_SCLL_OFFSET ) \
)

#define BSP_I2C_MULTI_MASTER_MAP_SCLH_PTR(_baseAddress)                                 \
(                                                                                       \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( (_baseAddress),			 \
                                                 BSP_I2C_MULTI_MASTER_MAP_SCLH_OFFSET ) \
)

#define BSP_I2C_MULTI_MASTER_MAP_TEST_PTR(_baseAddress)                                 \
(                                                                                       \
    BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( (_baseAddress),			 \
                                                 BSP_I2C_MULTI_MASTER_MAP_TEST_OFFSET ) \
)
#else
#error "This configuration not supported"
#endif
#endif
