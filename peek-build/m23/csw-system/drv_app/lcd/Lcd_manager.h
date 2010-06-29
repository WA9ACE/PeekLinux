/*=============================================================================
 * Copyright 2005 Texas Instruments Incorporated. All Rights Reserved.
*/

#ifndef LCD_MANAGER_HEADER
#define LCD_MANAGER_HEADER

#include "sys_types.h"
#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */
#include "rvm/rvm_use_id_list.h"

//#define LCD_TRACE_ENABLE
#if defined(LCD_TRACE_ENABLE)
#define LCD_TRACE(string)\
	rvf_send_trace ("LCD: "string,(sizeof("LCD: "string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )
#define LCD_TRACE_PARAM(string, param)\
	rvf_send_trace ("LCD: "string,(sizeof("LCD: "string)-1),(UINT32)param,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )
#else
#define LCD_TRACE(string)
#define LCD_TRACE_PARAM(string, param)
#endif
	
#define QWERT_TASK_SYNCH_LCD_200806

/*============================================================================*/
/*      	T_LCD_SELECT               selects a specific LCD				  */
/*============================================================================*/

typedef enum 
{
	DISPLAY_MAIN_LCD,
   	DISPLAY_SUB_LCD
} T_LCD_SELECT;


/*============================================================================*/
/*			 T_LCD_ENDIAN              selects the endianness to be used for  */
/*									   the Pixel data						  */
/*============================================================================*/

typedef enum 
{
	LITTLE_ENDIAN,
	BIG_ENDIAN
} T_LCD_ENDIAN;


/*============================================================================*/
/*      	T_LCD_PIXFORMAT       selects the Pixel format for the pixel data */
/*============================================================================*/

typedef enum 
{
	RGB565,
	RGB666,
	RGB888
} T_LCD_PIXFORMAT;


/*============================================================================*/
/*      	T_LCD_ORIENTATION    selects the orientation of the LCD           */
/*============================================================================*/

typedef enum 
{
	HORIZONTAL,
	VERTICAL
} T_LCD_ORIENTATION;


/*============================================================================*/
/*      	T_LCD_REFCONTROL    selects if LCD refresh is enabled or disabled */
/*============================================================================*/

typedef enum 
{
	REF_ENABLED,
	REF_DISABLED
} T_LCD_REFCONTROL;


/*============================================================================*/
/*      	T_LCD_COMMAND    Command to be issued to the LCD driver			  */
/*============================================================================*/

typedef enum 
{
	LCD_CLEAR,
	LCD_DISPLAYON,
	LCD_DISPLAYOFF,
	LCD_GETCONFIG,
	LCD_SETCONFIG,
	LCD_REFCONTROL
} T_LCD_COMMAND;


/*============================================================================*/
/*      	Lcd_fb_coordinates               selects the Pixel co-ordinates   */ 
/*											 to be refreshed				  */
/*============================================================================*/

typedef struct 
{
	UINT16 start_x;
   	UINT16 start_y;
   	UINT16 end_x;
   	UINT16 end_y;
}lcd_fb_coordinates;


/*============================================================================*/
/*      lcd_configParams             Parameters for LCD configuration.		  */
/*									 These are the parameters which could be  */
/*									 configured from the application.		  */
/*============================================================================*/

typedef struct 
{
	UINT16	height;	/* height of the display panel */
	UINT16	width;	/* width of the display panel */
	T_LCD_ORIENTATION orientation; /* orientation of the LCD */
	T_LCD_PIXFORMAT 	pixel_format;   /* RGB format */
	T_LCD_ENDIAN endianness;	/* Endianness of the pixel data */
	T_LCD_REFCONTROL refresh_control;    /* refresh control */
}lcd_configparams;


/*============================================================================*/
/*      	lcd_TuningTable             parameters of the tuning table. This  */
/*										table gives the whole list of         */
/*									    parameters which the application can  */
/*										configure as well as the read-only    */
/*										parameters which are controlled at the*/
/*										driver level.						  */
/*============================================================================*/

typedef struct 
{
	BOOL partial_update;     /* datatype bool..does it support windowing or partial update of the LCD framebuffer*/         
   	BOOL On_Screen_Display;  /*datatype bool.. does it support OSD On Screen Display */
   	BOOL dedicated_dma;      /*datatype bool.. is there dedicated dma */
	lcd_configparams *p_lcd_configparams;
}lcd_tuningtable;


#define LCD_WIDTH  319//175//127   //modified for 128x128 by gaofeng
#define LCD_HEIGHT 240//220//128   //modified for 128x128 by gaofeng

#define LCD_PIXEL_FORMAT RGB565
#define LCD_REFRESH_PERIOD 20

#define LCD_COL (LCD_WIDTH+1)
#define LCD_ROW (LCD_HEIGHT)



#define LCD_APPL_EVT_DISPLAY					RVF_APPL_EVT_0
#define LCD_APPL_EVT_DISPLAYON				RVF_APPL_EVT_1
#define LCD_APPL_EVT_DISPLAYOFF				RVF_APPL_EVT_2

extern UINT32 lcd_initializated;



/******************************************************************************
 *                            PROTOTYPE DEFINITION
 ******************************************************************************
 *           The following prototypes declares the public interface.
 */
/*============================================================================*/
/*!
 * @function lcd_initialization
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to initialize the LCD manager.It also initialises 
 *	 the LCD display and the LCD controller driver.  
 *   
 * <b> Context </b><br>
 *   This must be called before any other display services and functions
 *
 *  @result <br>
 *     
 */
    
     T_RV_RET lcd_initialization(T_LCD_SELECT sel);
 



/*============================================================================*/
/*!
 * @function lcd_display
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to loads pixel data from frame buffer into LCD display
 *   
 * <b> Context </b><br>
 *   
 *
 *  @result <br>
 *    
 */
     T_RV_RET lcd_display(T_LCD_SELECT sel, UINT16 *ImageDataPtr, lcd_fb_coordinates *p_lcd_coord);


 /*============================================================================*/
/*!
 * @function lcd_control
 *
 * @discussion
 * <b> Description </b><br>
 *   This function controls the display by sending different commands
 *   
 * <b> Context </b><br>
 *   
 *
 *  @result <br>
 *    
 */
     T_RV_RET lcd_control(T_LCD_SELECT sel, T_LCD_COMMAND command, void *p_cmd_param);





typedef enum{
	LCD_LED_OFF,
	LCD_LED_ON
} LCD_LED_ONOFF_t;


#define LCD_LED(state) (state==LCD_LED_ON?(GPIO_SET_OUTPUT(LCD_LED_GPIO)):(GPIO_CLEAR_OUTPUT(LCD_LED_GPIO)))




 /*============================================================================*/
/*!
 * @function lcd_led_onoff
 *
 * @discussion
 * <b> Description </b><br>
 *   This function controls the led on / off
 *   
 * <b> Context </b><br>
 *   
 *
 *  @result <br>
 *    
 */
     T_RV_RET lcd_led_onoff(LCD_LED_ONOFF_t onoff);

T_RV_RET lcd_led_getstatus(LCD_LED_ONOFF_t *onoff);


 /*============================================================================*/
/*!
 * @function lcd_set_sleep_time
 *
 * @discussion
 * <b> Description </b><br>
 *   This function set lcd sleep time in ms
 *   
 * <b> Context </b><br>
 *   
 *
 *  @result <br>
 *    
 */
T_RV_RET lcd_set_sleep_time(UINT32 ms);

#endif	


