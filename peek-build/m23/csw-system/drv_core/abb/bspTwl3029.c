/*==============================================================================
 *    Copyright 2004 Texas Instruments Inc. All rights reserved.
 */
/*==============================================================================
 * File Contents:
 *   This module contains functions associated with Triton ABB. 
 *
 *   The functions and macros are used for initializing Triton, 
 *    preparing and sending data structures
 *   for accessing Triton registers via the bsp I2C manager.
 */

/*===========================================================================
 * Defines and Macros
 */

/*===========================================================================
 * Local Data
 */

/*===========================================================================
 * Public Functions
 */
#include "types.h"


#include "bspTwl3029_Int_Map.h"
#include "bspTwl3029_Int_Llif.h"
#include "bspTwl3029_Pwr_Map.h"
#include "bspTwl3029_Pwr_Llif.h"
#include "bspTwl3029_Aud_Map.h"
#include "bspTwl3029_Aud_Llif.h"
#include "bspTwl3029_Aux_Map.h"
#include "bspTwl3029_Aux_Llif.h"
#include "bspTwl3029_I2c.h"
#include "bspTwl3029_Intc.h"
#include "bspTwl3029_Audio.h"
#if(OP_L1_STANDALONE==0)
#include "rv/rv_defined_swe.h"
#endif
#ifdef RVM_USB_SWE
#include "bspTwl3029_UsbOtg.h"
#endif

#include "bspTwl3029_Sim.h"
#include "bspTwl3029_Power.h"
#include "bspTwl3029_Rtc.h"
#include "bspTwl3029_Madc.h"
#include "bspTwl3029_Bci.h"

/*===========================================================================
 * Function: 
 *
 * Description: This function initializes the TWL3029 device driver: the 
 *              individual modules and shadow register array.
 *
 * Inputs:     none
 *             
 * Returns:    BspTwl3029_ReturnCode
 *
 * Notes:      
 */
BspTwl3029_ReturnCode bspTwl3029_init(void)
{
   
   
    BspTwl3029_ReturnCode returnCode= BSP_TWL3029_RETURN_CODE_FAILURE;
   
           
   /* init i2c  */ 
   /*triton register shadow array is initialized via twl3029_I2c_init */    
   returnCode = BspTwl3029_I2c_init();
   if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {
      /*init twl3029 interrupt */    
      returnCode = bspTwl3029_IntC_init();
   }
   if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {
      /*init twl3029 power management */    
      returnCode = bspTwl3029_Power_init();
   }
   if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {
      /* init audio  */
      returnCode = bspTwl3029_Audio_init();
   }
      
#ifdef RVM_USB_SWE
   if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {
      /* init USb transactor */
       returnCode = bspTwl3029_UsbOtg_init(); 
      /* init the other modules */
   }
#endif   
   
   if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {
      /* init SIM */
       returnCode = bspTwl3029_Sim_init(); 
      /* init the other modules */
   }
   
   if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {
      /* init RTC */
       returnCode = bspTwl3029_Rtc_init(); 
      /* init the other modules */
   }
   if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {
      /* init MADC */
       returnCode = bspTwl3029_Madc_init(); 
      /* init the other modules */
   }
   if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {
      /* init Bci */
       returnCode = bspTwl3029_Bci_init(); 
   }
  /* enable any required Triton interrupts  */
  if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
  /* enable UsbOtg */
  {
     bspTwl3029_IntC_enableSource((BspTwl3029_IntC_SourceId)BSP_TWL3029_INTC_SOURCE_ID_USB_D);
	 bspTwl3029_IntC_enableSource((BspTwl3029_IntC_SourceId)BSP_TWL3029_INTC_SOURCE_ID_PM_USB_VBUS);
  }
  /* enable rtcOtg */
  {
     bspTwl3029_IntC_enableSource((BspTwl3029_IntC_SourceId)BSP_TWL3029_INTC_SOURCE_ID_RTC);
  }
  /* enable madcOtg */
  {
     bspTwl3029_IntC_enableSource((BspTwl3029_IntC_SourceId)BSP_TWL3029_INTC_SOURCE_ID_MADC_P1);
  }
  return returnCode;
}
