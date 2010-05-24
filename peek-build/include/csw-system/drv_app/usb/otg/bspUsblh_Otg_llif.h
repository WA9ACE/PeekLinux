/*===================================================================
 *    Copyright 2004 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_USBLH_OTG_LLIF_HEADER
#define BSP_USBLH_OTG_LLIF_HEADER

#include "bspUtil_BitUtil.h"
#include "bspUtil_MemUtil.h"

/*=============================================================================
 * Component Description:
 *   Defines macros for the fields within the usblh OTG block.
 */

/*=============================================================================
 * Constants
 */

/* ============ Register: locosto core config  ============ */
#define BSP_LOCOSTO_CORE_CONFIG_USB_VBUS_CTRL_OFFSET	0
#define BSP_LOCOSTO_CORE_CONFIG_USB_VBUS_CTRL_WIDTH	1
#define BSP_LOCOSTO_CORE_CONFIG_USB_VBUS_MODE_OFFSET	1
#define BSP_LOCOSTO_CORE_CONFIG_USB_VBUS_MODE_WIDTH	1
#define BSP_LOCOSTO_CORE_CONFIG_USB_FILTER_CD_OFFSET	2 
#define BSP_LOCOSTO_CORE_CONFIG_USB_FILTER_CD_WIDTH 	3
#define BSP_LOCOSTO_CORE_CONFIG_USB_PRT_OFFSET	5	 
#define BSP_LOCOSTO_CORE_CONFIG_USB_PRT_WIDTH	2 
#define BSP_LOCOSTO_CORE_CONFIG_USB_TRX_MODE_OFFSET	7
#define BSP_LOCOSTO_CORE_CONFIG_USB_TRX_MODE_WIDTH		2
#define BSP_LOCOSTO_CORE_CONFIG_REG_USBX_SYNCHRO_OFFSET	10
#define BSP_LOCOSTO_CORE_CONFIG_REG_USBX_SYNCHRO_WIDTH		1

/* ============ Register: usb clk  ============ */
#define BSP_LOCOSTO_USBCLK_REG_CLK_FREQUENCY_OFFSET 0
#define BSP_LOCOSTO_USBCLK_REG_CLK_FREQUENCY_WIDTH 2
/* ============ Register: core config  ============ */
enum
{
    BSP_LOCOSTO_CORE_CONFIG_USB_VBUS_CTRL_USB_DISCONNECT   = 0x0,
    BSP_LOCOSTO_CORE_CONFIG_USB_VBUS_CTRL_USB_CONNECT   = 0x1
};


enum
{
    BSP_LOCOSTO_CORE_CONFIG_USB_VBUS_MODE_USE_USB_VBUS_CTRL_VALUE   = 0x0,
    BSP_LOCOSTO_CORE_CONFIG_USB_VBUS_MODE_USE_UIS480_VBUS_PIN	    = 0x1
};


enum
{
    BSP_LOCOSTO_CORE_CONFIG_USB_FILTER_CD_NO_FILTER 	= 0x0,
    BSP_LOCOSTO_CORE_CONFIG_USB_FILTER_CD_5_MS	 	= 0x1,
    BSP_LOCOSTO_CORE_CONFIG_USB_FILTER_CD_10_MS 	= 0x2,
    BSP_LOCOSTO_CORE_CONFIG_USB_FILTER_CD_15_MS 	= 0x3,    
    BSP_LOCOSTO_CORE_CONFIG_USB_FILTER_CD_20_MS 	= 0x4,    
    BSP_LOCOSTO_CORE_CONFIG_USB_FILTER_CD_25_MS 	= 0x5,    
    BSP_LOCOSTO_CORE_CONFIG_USB_FILTER_CD_30_MS 	= 0x6,    
    BSP_LOCOSTO_CORE_CONFIG_USB_FILTER_CD_ZERO_FILTER 		= 0x7    
};

enum
{
    BSP_LOCOSTO_CORE_CONFIG_USB_PORT_NOT_AVAILABLE	= 0x0,
	BSP_LOCOSTO_CORE_CONFIG_USB_PORT_AVAILABLE	= 0x1
};

enum
{
    BSP_LOCOSTO_CORE_CONFIG_USB_TRX_MODE_3_PIN	= 0x0,
    BSP_LOCOSTO_CORE_CONFIG_USB_TRX_MODE_4_PIN	= 0x1
};


enum
{
    BSP_LOCOSTO_CORE_CONFIG_REG_USBX_SYNCHRO_NO_DELAY	= 0x0,
    BSP_LOCOSTO_CORE_CONFIG_REG_USBX_48_MHZ_CYCLE_DELAY	= 0x1
};

enum
{
    BSP_LOCOSTO_USBCLK_REG_ENABLE_13MHZ_CLK	= 0x0,
    BSP_LOCOSTO_USBCLK_REG_ENABLE_52MHZ_CLK	= 0x2
};

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
#define BSP_USBLH_OTG_LLIF_GET( _name, _ptr )                 \
    (                                                         \
          BSPUTIL_MEMUTIL_READ( (_ptr),                       \
              BSPUTIL_BITUTIL_DATAUNIT_32,                    \
              _name ##_OFFSET,                                \
              _name ##_WIDTH )                                \
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
#define BSP_USBLH_OTG_LLIF_SET( _name,_ptr,_newValue )  \
{                                                       \
    BSPUTIL_MEMUTIL_WRITE( (_ptr),                      \
            (_newValue),                                \
            BSPUTIL_BITUTIL_DATAUNIT_32,                \
            _name ##_OFFSET,                            \
            _name ##_WIDTH );                           \
}

/*=============================================================================
 * Description:
 *    Gets a value from a local (shadowed or in local memory) register.
 *
 *    _name - the name of a field, without the trailing _OFFSET or _WIDTH
 *    _ptr - pointer to the register that the field is in.
 */
#define BSP_USBLH_OTG_LLIF_GET_LOCAL( _name, _ptr)        \
    (                                                     \
    BSPUTIL_BITUTIL_BIT_FIELD_GET( (_ptr),                \
            BSPUTIL_BITUTIL_DATAUNIT_32,                  \
            name ##_OFFSET,                               \
            name ##_WIDTH )                               \
    )


/*=============================================================================
 * Description:
 *    Sets a value into a  local (shadowed or in local memory) register field.
 *
 *    _name - the name of a field, without the trailing _OFFSET or _WIDTH
 *    _ptr - pointer to the register that the field is in.
 *    _newValue - new value for the field.
 */
#define BSP_USBLH_OTG_LLIF_SET_LOCAL( name,_ptr,_newValue )     \
{                                                               \
    BSPUTIL_BITUTIL_BIT_FIELD_SET( (_ptr),                      \
            (_newValue),                                        \
            BSPUTIL_BITUTIL_DATAUNIT_32,                        \
            name ##_OFFSET,                                     \
            name ##_WIDTH );                                    \
}




#endif
