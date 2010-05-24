/*===================================================================
 *    Copyright 2003-2004 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_I2C_MULTI_MASTER_LLIF_HEADER
#define BSP_I2C_MULTI_MASTER_LLIF_HEADER

#include "bspUtil_BitUtil.h"
#include "bspUtil_MemUtil.h"

/*=============================================================================
 * Component Description:
 *   Defines macros for the fields within the I2C_MULTI_MASTER block.
 */

/*=============================================================================
 * Constants
 */

/****** Definitions for any complete I2C_MULTI_MASTER Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_COMPLETE_REG_OFFSET  0
#define BSP_I2C_MULTI_MASTER_LLIF_COMPLETE_REG_WIDTH   16

/****** Definitions for REV Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_REV_OFFSET 0
#define BSP_I2C_MULTI_MASTER_LLIF_REV_WIDTH  7

/****** Definitions for IE Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_IE_AL_OFFSET   0
#define BSP_I2C_MULTI_MASTER_LLIF_IE_AL_WIDTH    1
#define BSP_I2C_MULTI_MASTER_LLIF_IE_AL_MASK     0x0001

#define BSP_I2C_MULTI_MASTER_LLIF_IE_NACK_OFFSET 1
#define BSP_I2C_MULTI_MASTER_LLIF_IE_NACK_WIDTH  1
#define BSP_I2C_MULTI_MASTER_LLIF_IE_NACK_MASK 0x0002

#define BSP_I2C_MULTI_MASTER_LLIF_IE_ARDY_OFFSET 2
#define BSP_I2C_MULTI_MASTER_LLIF_IE_ARDY_WIDTH  1
#define BSP_I2C_MULTI_MASTER_LLIF_IE_ARDY_MASK 0x0004

#define BSP_I2C_MULTI_MASTER_LLIF_IE_RRDY_OFFSET 3
#define BSP_I2C_MULTI_MASTER_LLIF_IE_RRDY_WIDTH  1
#define BSP_I2C_MULTI_MASTER_LLIF_IE_RRDY_MASK 0x0008

#define BSP_I2C_MULTI_MASTER_LLIF_IE_XRDY_OFFSET 4
#define BSP_I2C_MULTI_MASTER_LLIF_IE_XRDY_WIDTH  1
#define BSP_I2C_MULTI_MASTER_LLIF_IE_XRDY_MASK 0x0010

/****** Definitions for STAT Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_OFFSET    0
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_WIDTH     1
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK 0x0001

#define BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_OFFSET  1
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_WIDTH   1
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK 0x0002

#define BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_OFFSET  2
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_WIDTH   1
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK 0x0004

#define BSP_I2C_MULTI_MASTER_LLIF_STAT_RRDY_OFFSET  3
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_RRDY_WIDTH   1
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_RRDY_MASK  0x0008

#define BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_OFFSET  4
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_WIDTH   1
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_MASK 0x0010

#define BSP_I2C_MULTI_MASTER_LLIF_STAT_ADO_OFFSET   8
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_ADO_WIDTH    1

#define BSP_I2C_MULTI_MASTER_LLIF_STAT_AAS_OFFSET   9
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_AAS_WIDTH    1

#define BSP_I2C_MULTI_MASTER_LLIF_STAT_XUDF_OFFSET  10
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_XUDF_WIDTH   1

#define BSP_I2C_MULTI_MASTER_LLIF_STAT_ROVR_OFFSET  11
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_ROVR_WIDTH   1

#define BSP_I2C_MULTI_MASTER_LLIF_STAT_BB_OFFSET    12
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_BB_WIDTH     1
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_BB_MASK     0x1000

#define BSP_I2C_MULTI_MASTER_LLIF_STAT_SBD_OFFSET   15
#define BSP_I2C_MULTI_MASTER_LLIF_STAT_SBD_WIDTH    1

/****** Definitions for IV Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_IV_INTCODE_OFFSET 0
#define BSP_I2C_MULTI_MASTER_LLIF_IV_INTCODE_WIDTH  3

/****** Definitions for BUF Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_BUF_XDMA_EN_OFFSET 7
#define BSP_I2C_MULTI_MASTER_LLIF_BUF_XDMA_EN_WIDTH  1

#define BSP_I2C_MULTI_MASTER_LLIF_BUF_RDMA_EN_OFFSET 15
#define BSP_I2C_MULTI_MASTER_LLIF_BUF_RDMA_EN_WIDTH  1

/****** Definitions for CNT Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_CNT_DCOUNT_OFFSET 0
#define BSP_I2C_MULTI_MASTER_LLIF_CNT_DCOUNT_WIDTH  16

/****** Definitions for DATA Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_DATA_OFFSET 0
#define BSP_I2C_MULTI_MASTER_LLIF_DATA_WIDTH  16

/****** Definitions for CON Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_CON_STT_OFFSET 0
#define BSP_I2C_MULTI_MASTER_LLIF_CON_STT_WIDTH  1
#define BSP_I2C_MULTI_MASTER_LLIF_CON_STT_MASK  0x0001

#define BSP_I2C_MULTI_MASTER_LLIF_CON_STP_OFFSET 1
#define BSP_I2C_MULTI_MASTER_LLIF_CON_STP_WIDTH  1
#define BSP_I2C_MULTI_MASTER_LLIF_CON_STP_MASK   0x0002

#define BSP_I2C_MULTI_MASTER_LLIF_CON_RM_OFFSET 2
#define BSP_I2C_MULTI_MASTER_LLIF_CON_RM_WIDTH  1

#define BSP_I2C_MULTI_MASTER_LLIF_CON_XA_OFFSET  8
#define BSP_I2C_MULTI_MASTER_LLIF_CON_XA_WIDTH   1

#define BSP_I2C_MULTI_MASTER_LLIF_CON_TRX_OFFSET 9
#define BSP_I2C_MULTI_MASTER_LLIF_CON_TRX_WIDTH  1
#define BSP_I2C_MULTI_MASTER_LLIF_CON_TRX_MASK  0x0200

#define BSP_I2C_MULTI_MASTER_LLIF_CON_MST_OFFSET 10
#define BSP_I2C_MULTI_MASTER_LLIF_CON_MST_WIDTH  1
#define BSP_I2C_MULTI_MASTER_LLIF_CON_MST_MASK 0x0400

#define BSP_I2C_MULTI_MASTER_LLIF_CON_STB_OFFSET 11
#define BSP_I2C_MULTI_MASTER_LLIF_CON_STB_WIDTH  1

#define BSP_I2C_MULTI_MASTER_LLIF_CON_BE_OFFSET  14
#define BSP_I2C_MULTI_MASTER_LLIF_CON_BE_WIDTH   1

#define BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_EN_OFFSET 15
#define BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_EN_WIDTH  1
#define BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_EN_MASK 0x8000

/****** Definitions for OA Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_OA_OFFSET 0
#define BSP_I2C_MULTI_MASTER_LLIF_OA_WIDTH  10

/****** Definitions for SA Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_SA_OFFSET 0
#define BSP_I2C_MULTI_MASTER_LLIF_SA_WIDTH  10

/****** Definitions for PSC Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_PSC_OFFSET 0
#define BSP_I2C_MULTI_MASTER_LLIF_PSC_WIDTH  8

/****** Definitions for SCLL Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_SCLL_OFFSET 0
#define BSP_I2C_MULTI_MASTER_LLIF_SCLL_WIDTH  8

/****** Definitions for SCLH Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_SCLH_OFFSET 0
#define BSP_I2C_MULTI_MASTER_LLIF_SCLH_WIDTH  8


/****** Definitions for SYS_TEST Register *******/
#define BSP_I2C_MULTI_MASTER_LLIF_SDAO_OFFSET 0
#define BSP_I2C_MULTI_MASTER_LLIF_SDAO_WIDTH  1

#define BSP_I2C_MULTI_MASTER_LLIF_SDAI_OFFSET I
#define BSP_I2C_MULTI_MASTER_LLIF_SDAI_WIDTH  1

#define BSP_I2C_MULTI_MASTER_LLIF_SCLO_OFFSET 2
#define BSP_I2C_MULTI_MASTER_LLIF_SCLO_WIDTH  1

#define BSP_I2C_MULTI_MASTER_LLIF_SCLI_OFFSET 3
#define BSP_I2C_MULTI_MASTER_LLIF_SCLI_WIDTH  1

#define BSP_I2C_MULTI_MASTER_LLIF_TMODE_OFFSET  12
#define BSP_I2C_MULTI_MASTER_LLIF_TMODE_WIDTH    2

#define BSP_I2C_MULTI_MASTER_LLIF_FREE_OFFSET 14
#define BSP_I2C_MULTI_MASTER_LLIF_FREE_WIDTH  1

#define BSP_I2C_MULTI_MASTER_LLIF_STEN_OFFSET 15
#define BSP_I2C_MULTI_MASTER_LLIF_STEN_WIDTH  1


/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_CON_STP_NO_STOP = 0,
    BSP_I2C_MULTI_MASTER_LLIF_CON_STP_STOP = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_Con_StopNoStop;

/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_CON_TRX_READ  = 0,
    BSP_I2C_MULTI_MASTER_LLIF_CON_TRX_WRITE = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_Con_ReadWrite;

/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_CON_MST_DISABLED = 0,
    BSP_I2C_MULTI_MASTER_LLIF_CON_MST_ENABLED  = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_Con_MstEnaDisabled;

/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_DISABLED = 0,
    BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_ENABLED  = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_Con_I2cEnaDisabled;



/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_STAT_BB_BUS_FREE    = 0,
    BSP_I2C_MULTI_MASTER_LLIF_STAT_BB_BUS_BUSY    = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_Stat_BbState;

/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_NORMAL        = 0,
    BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_ARBITRATION_LOST  = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_Stat_AlState;

/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_NORMAL  = 0,
    BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_OCCURED = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_Stat_NackState;

/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_FALSE  = 0,
    BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_TRUE   = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_Stat_ArdyState;

/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_STAT_RRDY_FALSE  = 0,
    BSP_I2C_MULTI_MASTER_LLIF_STAT_RRDY_TRUE   = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_Stat_RrdyState;


/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_FALSE  = 0,
    BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_TRUE   = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_Stat_XrdyState;


/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_STAT_CLEAR_INT = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_Stat_ClearInt;

/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_DISABLED  = 0,
    BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_ENABLED   = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_Ie_IrqEnaDis;


/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_BUF_RDMA_DISABLED  = 0,
    BSP_I2C_MULTI_MASTER_LLIF_BUF_RDMA_ENABLED   = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_Buf_RdmaEnaDis;

/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_BUF_XDMA_DISABLED  = 0,
    BSP_I2C_MULTI_MASTER_LLIF_BUF_XDMA_ENABLED   = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_Buf_XdmaEnaDis;

/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_SYSTEST_STEN_DISABLED  = 0,
    BSP_I2C_MULTI_MASTER_LLIF_SYSTEST_STEN_ENABLED   = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_SysTest_StenEnaDis;

/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_SYSTEST_TMODE_DISABLED  = 0,
    BSP_I2C_MULTI_MASTER_LLIF_SYSTEST_TMODE_IOMODE   = 3
};
typedef Uint16 BspI2cMultiMaster_Llif_SysTest_TMode_EnaDis;

/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_SYSTEST_SCLO_LOW  = 0,
    BSP_I2C_MULTI_MASTER_LLIF_SYSTEST_SCLO_HIGH   = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_SysTest_ScloLowHigh;

/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_SYSTEST_SDAO_LOW  = 0,
    BSP_I2C_MULTI_MASTER_LLIF_SYSTEST_SDAO_HIGH   = 1
};
typedef Uint16 BspI2cMultiMaster_Llif_SysTest_SdaoLowHigh;

/*=============================================================================
 * Description:
 *  These defines are used for clearing the specified flags
 *  in the Status register
 */
enum
{
    BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_CLEAR = (BSP_I2C_MULTI_MASTER_LLIF_STAT_CLEAR_INT << BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_OFFSET),
    BSP_I2C_MULTI_MASTER_LLIF_STAT_RRDY_CLEAR = (BSP_I2C_MULTI_MASTER_LLIF_STAT_CLEAR_INT << BSP_I2C_MULTI_MASTER_LLIF_STAT_RRDY_OFFSET),
    BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_CLEAR = (BSP_I2C_MULTI_MASTER_LLIF_STAT_CLEAR_INT << BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_OFFSET)
};
typedef Uint16 BspI2cMaster_Llif_StatusFlagClear;

/*=============================================================================
 * Utility Macros
 */
/*=============================================================================
 * Description:
 *    Gets a value from a register field. (XXXNote, don't use this for
 *    registers that must also be shadowed).
 *
 *    _name - the name of a field, without the trailing _OFFSET or _WIDTH
 *    _ptr - pointer to the register that the field is in.
 */
#define BSP_I2C_MULTI_MASTER_LLIF_GET( _name,           \
                                       _ptr )           \
(                                                       \
    BSPUTIL_MEMUTIL_READ( (_ptr),                       \
                          BSPUTIL_BITUTIL_DATAUNIT_16,  \
                          _name ##_OFFSET,              \
                          _name ##_WIDTH )              \
)


/*=============================================================================
 * Description:
 *    Sets a value into a register field. (XXXNote, don't use this for
 *    registers that must also be shadowed).
 *
 *    _name - the name of a field, without the trailing _OFFSET or _WIDTH
 *    _ptr - pointer to the register that the field is in.
 *    _newValue - new value for the field.
 */
#define BSP_I2C_MULTI_MASTER_LLIF_SET( _name,           \
                                       _ptr,            \
                                       _newValue )      \
{                                                       \
    BSPUTIL_BITUTIL_BIT_FIELD_SET16( (_ptr),                      \
                           (_newValue),                 \
                            _name ##_OFFSET,             \
                           _name ##_WIDTH );            \
}

/*=============================================================================
 * Description:
 *    Gets a value from a local (shadowed or in local memory) register.
 *
 *    _name - the name of a field, without the trailing _OFFSET or _WIDTH
 *    _ptr - pointer to the register that the field is in.
 */
#define BSP_I2C_MULTI_MASTER_LLIF_GET_LOCAL( _name,             \
                                             _ptr )             \
(                                                               \
    BSPUTIL_BITUTIL_BIT_FIELD_GET( (_ptr),                      \
                                   BSPUTIL_BITUTIL_DATAUNIT_16, \
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
#define BSP_I2C_MULTI_MASTER_LLIF_SET_LOCAL( _name,             \
                                             _ptr,              \
                                             _newValue )        \
{                                                               \
    BSPUTIL_BITUTIL_BIT_FIELD_SET( (_ptr),                      \
                                   (_newValue),                 \
                                   BSPUTIL_BITUTIL_DATAUNIT_16, \
                                   _name ##_OFFSET,             \
                                   _name ##_WIDTH );            \
}

#endif
