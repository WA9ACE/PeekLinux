#include "lcd_manager.h"
#include "lcd_interface.h"
extern T_RVF_ADDR_ID lcd_addr_id;
UINT16 *ImageDataPtrTemp; //Global Framebuffer pointer used
/*============================================================================*/
/*                            Public Functions								  */
/*============================================================================*/

lcd_configparams lcd_config_params; 

T_RV_RET lcd_initialization( T_LCD_SELECT sel )
	 {
	 	rvf_send_trace("LCD MANAGER INIT STARTED",24, NULL_PARAM, 
			   RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );

		// added by santosh 
				
		lcd_config_params.height=LCD_HEIGHT;
		lcd_config_params.width=LCD_WIDTH;
		lcd_config_params.orientation=HORIZONTAL;
		lcd_config_params.endianness=LITTLE_ENDIAN;
		lcd_config_params.pixel_format=RGB565;
		lcd_config_params.refresh_control=REF_ENABLED;

		// added by santosh

		
		if(sel == DISPLAY_MAIN_LCD)
		{
		  if(lcd_pri_if_init() == LCD_IF_RET_OK)
		  	return RV_OK;
	  	  else
		  	return RV_NOT_SUPPORTED;
		}
		return RV_OK;
	 }

 

 


 T_RV_RET lcd_display(  T_LCD_SELECT sel,UINT16 *ImageDataPtr,lcd_fb_coordinates *p_lcd_coord)
	 {
	    if(ImageDataPtr!=NULL)
	    	{
	 	ImageDataPtrTemp = ImageDataPtr;
		rvf_send_trace("LCD MANAGER DISPLAY STARTED",24, NULL_PARAM, 
			   RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );
		if(lcd_config_params.refresh_control==REF_ENABLED)
			rvf_send_event( (UINT8)(lcd_addr_id & 0xFF), EVENT_MASK(RVF_APPL_EVT_0) );
		rvf_delay(RVF_MS_TO_TICKS(LCD_REFRESH_PERIOD));
		return RV_OK;
	    	}
		else
			{
			rvf_send_trace("LCD MANAGER POINTER NULL",24, NULL_PARAM, 
			   RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );
			return RVM_MEMORY_ERR;
			}
	 }

 T_RV_RET lcd_control(T_LCD_SELECT sel, T_LCD_COMMAND command, void *p_cmd_param)
	 {
		if(sel == DISPLAY_MAIN_LCD)
		{
		  if(lcd_pri_if_control(command, p_cmd_param) == LCD_IF_RET_OK)
		  	return RV_OK;
	  	  else
		  	return RV_NOT_SUPPORTED;
		}
		return RV_OK;
	 }





