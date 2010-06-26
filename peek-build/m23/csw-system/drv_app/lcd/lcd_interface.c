
#include "Lcd_interface.h"
//#define TIME_MEASUREMENT 1

/*============================================================================*/
/*                            Public Functions								  */
/*============================================================================*/
/*!* @function lcd_pri_if_init 
 *
 * @discussion
 * <b> Description </b><br>
 *   
 *   
 * <b> Context </b><br>
 *   
 *
 *  @result <br>
 *    
 *
 */
    
     ELCD_IF_RET lcd_pri_if_init ()
	 {
		if(lcd_parallel_config()== LCD_PAR_RET_OK)
		{
			return LCD_IF_RET_OK;
		}
		else
		{
			return LCD_IF_RET_FAIL;
		}
	 }
 




ELCD_IF_RET lcd_pri_if_display(UINT16 *ImageDataPtrIF,lcd_fb_coordinates *p_lcd_coord )
{
  UINT16 t1,t2;
		
			#if (L1_POWER_MGT !=0)
		    Lcd_Vote_DeepSleepStatus();
		    #endif
		
		#ifdef TIME_MEASUREMENT
		t1 = rvf_get_tick_count(); 
		#endif 
		if(lcd_parallel_display(ImageDataPtrIF)== LCD_PAR_RET_OK)
		{
			#ifdef TIME_MEASUREMENT
		    t2 = rvf_get_tick_count(); 
//			rvf_send_trace("Time taken to refresh in Milisec:",strlen("Time taken to refresh in Milisec:"), 
 //                                                       RVF_TICKS_TO_MS ((t2 - t1)), 
 //                                                       RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID ); 
			#endif 
			return LCD_IF_RET_OK;
		}
		else
		{
			return LCD_IF_RET_FAIL;
		}

}


ELCD_IF_RET lcd_pri_if_control(T_LCD_COMMAND command, void *p_cmd_param)
{

		if(lcd_parallel_control(command,p_cmd_param)== LCD_PAR_RET_OK)
		{
			return LCD_IF_RET_OK;
		}
		else
		{
			return LCD_IF_RET_FAIL;
		}
}
		
		

