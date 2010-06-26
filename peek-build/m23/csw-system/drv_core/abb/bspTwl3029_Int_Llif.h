/*=============================================================================
 *    Copyright 2004 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_TWL3029_INT_LLIF_HEADER
#define BSP_TWL3029_INT_LLIF_HEADER

#include "bspUtil_BitUtil.h"

/*=============================================================================
 * Component Description:
 *    Provides accessor and manipulator macros for TWL3029 aka Triton
 *    register fields for BARINT mddule. Fields offsets and widths are encoded in contants
 *    that are to be fed into simple macros. This common approach should
 *    help to prevent masking errors.
 *
 *    the BARINT registers consist 
 *     - of page selection registers
 *     - general configuration registers
 *     - interrurpt registers
 */


/*========= Register PAGE =======================
 */
  
#define BSP_TWL3029_LLIF_PAGEREG_PAGE_OFFSET      0			
#define BSP_TWL3029_LLIF_PAGEREG_PAGE_WIDTH       2

#define BSP_TWL3029_LLIF_PAGEREG0_PAGE_OFFSET     0			
#define BSP_TWL3029_LLIF_PAGEREG0_PAGE_WIDTH      2

#define BSP_TWL3029_LLIF_PAGEREG1_PAGE_OFFSET     0			
#define BSP_TWL3029_LLIF_PAGEREG1_PAGE_WIDTH      2

#define BSP_TWL3029_LLIF_PAGEREG2_PAGE_OFFSET     0			
#define BSP_TWL3029_LLIF_PAGEREG2_PAGE_WIDTH      2

#define BSP_TWL3029_LLIF_PAGEREG3_PAGE_OFFSET     0			
#define BSP_TWL3029_LLIF_PAGEREG3_PAGE_WIDTH      2

/*========= Register TESTUNLOCK =======================
 */
 
#define BSP_TWL3029_LLIF_CKG_TEST_UNLOCK_KEY_OFFSET   0
#define BSP_TWL3029_LLIF_CKG_TEST_UNLOCK_KEY_WIDTH    8

/*========= Register MCLKFREQ =======================
 */
#define BSP_TWL3029_LLIF_CKG_MCLKFREQ_MCLK1_FREQ_OFFSET      0
#define BSP_TWL3029_LLIF_CKG_MCLKFREQ_MCLK1_FREQ_WIDTH       2

#define BSP_TWL3029_LLIF_CKG_MCLKFREQ_MCLK1_WREN_OFFSET      2
#define BSP_TWL3029_LLIF_CKG_MCLKFREQ_MCLK1_WREN_WIDTH       1

#define BSP_TWL3029_LLIF_CKG_MCLKFREQ_MCLK2_FREQ_OFFSET      3
#define BSP_TWL3029_LLIF_CKG_MCLKFREQ_MCLK2_FREQ_WIDTH       2

#define BSP_TWL3029_LLIF_CKG_MCLKFREQ_MCLK2_WREN_OFFSET      5
#define BSP_TWL3029_LLIF_CKG_MCLKFREQ_MCLK2_WREN_WIDTH       1


/*========= Register MCLKTEST =======================
 */
#define BSP_TWL3029_LLIF_CKG_MCLKTEST_MCLK_DDEFAULT_OFFSET      0
#define BSP_TWL3029_LLIF_CKG_MCLKTEST_MCLK_DDEFAULT_WIDTH       1

#define BSP_TWL3029_LLIF_CKG_MCLKTEST_MCLK_SRC_OFFSET           1
#define BSP_TWL3029_LLIF_CKG_MCLKTEST_MCLK_SRC_WIDTH            2

#define BSP_TWL3029_LLIF_CKG_MCLKTEST_MCLK_RSVRD_1_OFFSET       3
#define BSP_TWL3029_LLIF_CKG_MCLKTEST_MCLK_RSVRD_1_WIDTH        1

#define BSP_TWL3029_LLIF_CKG_MCLKTEST_MCLK_STATUS_OFFSET        4
#define BSP_TWL3029_LLIF_CKG_MCLKTEST_MCLK_STATUS_WIDTH         2

#define BSP_TWL3029_LLIF_CKG_MCLKTEST_MCLK_ON_OFFSET            6
#define BSP_TWL3029_LLIF_CKG_MCLKTEST_MCLK_ON_WIDTH             2

/*========= Register IT2MASKP2H =======================
 */
#define BSP_TWL3029_LLIF_INT2_IT2MASKP2H_INT2_P2_MSK_OFFSET      0
#define BSP_TWL3029_LLIF_INT2_IT2MASKP2H_INT2_P2_MSK_WIDTH       8       /* INT2 mask bits 8 - 15 for P2 (modem) processor*/

/*========= Register IT2MASKP2HL =======================
 */
#define BSP_TWL3029_LLIF_INT2_IT2MASKP2L_INT2_P2_MSK_OFFSET      0 
#define BSP_TWL3029_LLIF_INT2_IT2MASKP2L_INT2_P2_MSK_WIDTH       8        /* INT2 mask bits 0 - 7 */

/*========= Register IT2STATUSP2H =======================
 */
#define BSP_TWL3029_LLIF_INT2_IT2STATUSP2H_INT2_P2_STS_OFFSET     0
#define BSP_TWL3029_LLIF_INT2_IT2STATUSP2H_INT2_P2_STS_WIDTH      8       /* INT2 status bits 8 - 15 for P2 (modem) processor*/

/*========= Register IT2STATUSP2L =======================
 */
#define BSP_TWL3029_LLIF_INT2_IT2STATUSP2L_INT2_P2_STS_OFFSET     0 
#define BSP_TWL3029_LLIF_INT2_IT2STATUSP2L_INT2_P2_STS_WIDTH      8        /* INT2 status bits 0 - 7 */



/*========= Register IT2MASKP1H =======================
 */
#define BSP_TWL3029_LLIF_INT2_IT2MASKP2H_INT2_P1_MSK_OFFSET       0
#define BSP_TWL3029_LLIF_INT2_IT2MASKP2H_INT2_P1_MSK_WIDTH        8       /* INT2 mask bits 8 - 15 for P2 (modem) processor*/

/*========= Register IT2MASKP1L =======================
 */
#define BSP_TWL3029_LLIF_INT2_IT2MASKP2L_INT2_P1_MSK_OFFSET       0 
#define BSP_TWL3029_LLIF_INT2_IT2MASKP2L_INT2_P1_MSK_WIDTH        8        /* INT2 mask bits 0 - 7 */

/*========= Register IT2STATUSP1H =======================
 */
#define BSP_TWL3029_LLIF_INT2_IT2STATUSP1H_INT2_P1_STS_OFFSET     0
#define BSP_TWL3029_LLIF_INT2_IT2STATUSP1H_INT2_P1_STS_WIDTH      8       /* INT2 status bits 8 - 15 for P2 (modem) processor*/

/*========= Register IT2STATUSP2L =======================
 */
#define BSP_TWL3029_LLIF_INT2_IT2STATUSP1L_INT2_P1_STS_OFFSET     0 
#define BSP_TWL3029_LLIF_INT2_IT2STATUSP1L_INT2_P1_STS_WIDTH      8        /* INT2 status bits 0 - 7 */

/*=============================================================================
 * Accessories
 */


/*=============================================================================
 * Description:
 *    Gets a value from a local (shadowed or in local memory) register.
 *
 *    _name - the name of a field, without the trailing _OFFSET or _WIDTH
 *    _ptr - pointer to the register that the field is in.
 */
#define BSP_TWL3029_LLIF_GET_FIELD( _name,                      \
                                    _ptr )                      \
(                                                               \
    BSPUTIL_BITUTIL_BIT_FIELD_GET( (_ptr),                      \
                                   BSPUTIL_BITUTIL_DATAUNIT_8,  \
                                   _name ##_OFFSET,             \
                                   _name ##_WIDTH )             \
)


/*=============================================================================
 * Description:
 *    Sets a value into a  local (shadowed or in local memory) register field.
 *
 *    _name - the name of a field, without the trailing _OFFSET or _WIDTH
 *    _ptr - pointer to the register that the field is in.
 *    _newValue - new value for the field.
 */
#define BSP_TWL3029_LLIF_SET_FIELD( _name,                      \
                                    _ptr,                       \
                                    _newValue )                 \
{                                                               \
    BSPUTIL_BITUTIL_BIT_FIELD_SET( (_ptr),                      \
                                   (_newValue),                 \
                                   BSPUTIL_BITUTIL_DATAUNIT_8,  \
                                   _name ##_OFFSET,             \
                                   _name ##_WIDTH );            \
}

/*=============================================================================
 * Description:
 *    enum used to set/unset single bits in a register .
 *
 */
 enum
{
   BSP_TWL3029_REGISTER_BIT_LOW  = 0x00,
   BSP_TWL3029_REGISTER_BIT_HIGH = 0x01
};



#endif
