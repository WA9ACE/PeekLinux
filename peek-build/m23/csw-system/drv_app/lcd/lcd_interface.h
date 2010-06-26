#ifndef LCD_INTERFACE_HEADER
#define LCD_INTERFACE_HEADER

#include "Lcd_Transport.h"
#include "Lcd_manager.h"




/*=============================================================================
 *  Enumerations and Structures
 *============================================================================*/

 typedef enum 
{
	LCD_IF_RET_OK,
	LCD_IF_RET_FAIL
}ELCD_IF_RET;


 
/******************************************************************************
 *                            PROTOTYPE DEFINITION
 ******************************************************************************
 *           The following prototypes declares the public interface.
 */
/*============================================================================*/
/*!
 * @function lcd_pri_if_init 
 *
 * @discussion
 * <b> Description </b><br>
 *   This function initialises the LCD interface and the LCD controller.
 *   
 * <b> Context </b><br>
 *   This must be called before any other display services and functions
 *
 *  @result <br>
 *     The LCD interface and the LCD controller will be initialized.
 *
 */
    
     ELCD_IF_RET lcd_pri_if_init ();
 
/*=============================================================================
 * @function lcd_pri_if_display
 *
 * @discussion
 * <b> Description </b><br>
 *    
 *	 
 *   
 * <b> Context </b><br>
 *   
 *	
 *
 *  @result <br>
 *    
 *	  
 *
 */

	 ELCD_IF_RET lcd_pri_if_display(UINT16 *ImageDataPtrIF,lcd_fb_coordinates *p_lcd_coord);


/*=============================================================================
 * @function lcd_pri_if_control
 *
 * @discussion
 * <b> Description </b><br>
 *    
 *	 
 *   
 * <b> Context </b><br>
 *   
 *	
 *
 *  @result <br>
 *    
 *	  
 *
 */

  ELCD_IF_RET lcd_pri_if_control(T_LCD_COMMAND command, void *p_cmd_param);

#endif	
