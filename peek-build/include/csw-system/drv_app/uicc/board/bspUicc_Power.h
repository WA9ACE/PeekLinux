/*=============================================================================
 *    Copyright 2003-2004 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_UICC_POWER_HEADER
#define BSP_UICC_POWER_HEADER

/*=============================================================================
 *  File Description:
 *  Source File for UICC interface to control power to the card
 */

/*=============================================================================
 *   Defines
 *============================================================================*/
 
enum
{
    BSP_UICC_POWER_RETURN_CODE_FAILURE    = (-1),
    BSP_UICC_POWER_RETURN_CODE_SUCCESS    = (0)
};
typedef SYS_WORD16 BspUicc_Power_ReturnCode;

/* Locosto core registers used by USIM */
#define LOCOSTO_CORE_CONF (*(volatile UINT16*)(0xFFFEF01C))
#define CONF_LCD_CAM_ND (*(volatile UINT16*)(0xFFFEF01E))

/*=============================================================================
 *   Public Functions
 *============================================================================*/

/*=============================================================================
 * Description:
 *   Power on the card at the lowest power level
 */
BspUicc_Power_ReturnCode
bspUicc_Power_on( void );

/*=============================================================================
 * Description:
 *   Increase the power level
 */
BspUicc_Power_ReturnCode
bspUicc_Power_increase( void );

/*=============================================================================
 * Description:
 *   Power off the card
 */
BspUicc_Power_ReturnCode
bspUicc_Power_off( void );

BspUicc_Power_ReturnCode
bspUicc_Init_sim( void );

#endif
