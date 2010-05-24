/*=============================================================================
 * Copyright 2002-2004 Texas Instruments Inc. All rights reserved.
 */

#ifndef BSP_USBLH_OTG_HEADER
#define BSP_USBLH_OTG_HEADER

/*=============================================================================
 * Component Description:
 *    This header file provides access to functions exports by bspUsblh_Otg that
 *    are not specified by the bspUsblh.h header file.
 */

/*=============================================================================
 * Description:
 *    This function can be called by an application Initializing OTG controller 
 *
 * Parameters:
 *
 * Returns:
 *
 * Context:
 */
Bool bspUsblh_Otg_Init(void *notused);
Bool bspUsblh_Otg_Register_Triton_Interrupt();
Bool bspUsblh_Otg_Enable_Triton_Interrupt();
Bool bspUsblh_Otg_Disable_Triton_Interrupt();

#endif
