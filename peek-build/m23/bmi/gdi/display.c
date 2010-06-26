/*
+-----------------------------------------------------------------------------
|  Project :
|  Modul   :  display.c
+-----------------------------------------------------------------------------
|  Copyright 2002 Texas Instruments Berlin, AG
|                 All rights reserved.
|
|                 This file is confidential and a trade secret of Texas
|                 Instruments Berlin, AG
|                 The receipt of or possession of this file does not convey
|                 any rights to reproduce or disclose its contents or to
|                 manufacture, use, or sell anything it may describe, in
|                 whole, or in part, without the specific written consent of
|                 Texas Instruments Berlin, AG.
+-----------------------------------------------------------------------------
|  Purpose :
+-----------------------------------------------------------------------------
*/
/*

   	Nov 24, 2006 DR: OMAPS00092906  x0039928(sumanth)
	Description: CT-GCF[34.3]-Some broadcasted charcters not dispayed correctly in MMI
	Solution	: A value in the translation_ascii_2_unicode[] array is modified. The 28th entry
	is replaced from 0xFFFF to 0x001B 
	
       Jun 29, 2006 DR: OMAPS00083156  x0039928(sumanth)
	Description: CT-GCF/PTCRB-LL[27.22.4.8.1]-MS does not display the complete text
	Solution	: dspl_getDisplayType returns DSPL_COLOUR instead of DSPL_BW if 
	COLOURDISPLAY or FF_SSL_ADAPTATION flag is enabled.

	Jun 09, 2006 ER: OMAPS00057437 - xdeepadh
	SSL plane property updated.
	
	Feb 27, 2006 ER: OMAPS00057437 - xrashmic
	SSL adaptation implemented
	
    Nov 04, 2005 DR: OMAPS00052595
	Description: Simulation build fails for Locosto
	Solution	: This function is not being used for the simulation build hence
		the contents of the same are put under the compiler flag #ifndef WIN32


	Nov 02, 2005 DR: OMAPS00051051 - xpradipg
	Description:The display is cluttered when the camera is in the viewfinder 
			    mode for a long time
	Solution: The check for requested state and current state is moved before
			the assignment of the current state with the requested state.
	
	Sept 15, 2005 REF: LOCOSTO-ENH-34257 - xpradipg
	Description: Locosto: MIgration to New LCD APIs
	Solution: Migrated to New APIs by replacing the old driver APIs with
	corresponding New LCD APIs
       xrashmic 22 Aug, 2004 MMI-SPR-32798
       Adding the support for screen capture using a dynamically assigned key.
   
	July 1, 2005 REF: CRR 32677 - xpradipg
	Description: Alignmnet of the display for each row. Send the un negated value to LCD driver
	Solution: The alignment of the display is corrected by decreasing the number of columns by 1.
			The negated value is corrected and sent to the driver

	June 03, 2005 REF: GSM-ENH-31636 xpradipg
	Description:	Change the board ID for Isample to 71 and add new defination
					for the CALLISTO with baord ID 70
	Solution:		Add the definition of ALLISTO with board ID 70 and change the
					board Id to 71 for Isample
   
	Mar 30, 2005	REF: CRR 29986	xpradipg
	Description: Optimisation 1: Removal of unused variables and dynamically
	allocate/ deallocate mbndata
	Solution: removed the unused variable pictureCopy_col and the static 
	qualifier so as to be accessed by the ATB module


	CRR 28825:  xpradipg - 11 Feb 2005
	Description:Extension of GDI-for display of ISample and code cleanup
	Solution: The code replication for D_Sample and E_sample are removed and a 
	new	flag is added for the ISample. currently the BOARD ID is set to 70 for
	CALLISTO and 71 for ISample.

GW 31/01/03 -	Added support for 16 bit LCD buffer -
					Split DSAMPLE_COLOUR code into DSAMPLE_COLOUR32 (original 32bits/pixel)
					and DSAMPLE_COLOUR16 (16 bits/pixel - untested)
				Removed CSAMPLE_COLOUR option
				Removed 'PLOT' option for D-sample - too slow to be useful

*/

#ifndef DRV_DSPL_C
#define DRV_DSPL_C
#endif

/*==== INCLUDES ===================================================*/
#define ENTITY_CST

#include <string.h>
#if defined (WIN32)
#include <stdio.h>
#endif

#include "typedefs.h"
#include "gdi.h"
#include "dspl.h"
#include "font_bitmaps.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"
#include "p_mmi.h"
#include "tok.h"
#include "cst/cst.h"
#include "os.h"

#include "rvf/rvf_api.h"

#undef R2D_ENABLED
#ifndef _SIMULATION_

//xrashmic 22 Feb, 2006 SSL_Adaptation 
#ifdef FF_SSL_ADAPTATION //xrashmic 22 Feb, 2006 SSL_Adaptation
#include "omx_ssl/inc/OMX_SSLComponent.h"
#include "omx_core/inc/omx_tiimage.h"
#endif //FF_SSL_ADAPTATION

#if (defined(_TARGET_) && (BOARD!=10) && (BOARD!=34)&& (BOARD!=35)&& (BOARD!=46) && (BOARD !=70) && (BOARD != 71))
/*
 * R2D is not supported by EP2 module (BOARD == 10, of customer 6351/36)
 * and by Avenger2/Perseus1 board (BOARD == 34). (dbachran / 2002-11-27)
 * and by P2 board (BOARD == 35) and F-sample (BOARD == 46) (rsauer / 2004-05-27)
 */
#define R2D_ENABLED
#endif
#endif
 



#ifdef FF_MMI_NEW_LCD_API_MIG 
#include "lcd_manager.h"
#endif
#ifndef _SIMULATION_
    #ifdef R2D_ENABLED
        #include "r2d/r2d.h"
        #include "r2d/r2d_config.h"
    #endif
#endif


 //xrashmic 22 Feb, 2006 SSL_Adaptation 
#ifdef FF_SSL_ADAPTATION
OMX_HANDLETYPE *pHandle, pHandlePlane1, pHandlePlane2;
OMX_BUFFERHEADERTYPE buffer_header;
int mixed_mode_offset=0;
int mmi_mode_buf_size=0;
int mixed_mode_buf_size=0;
#endif //FF_SSL_ADAPTATION



/* GW enable/disable trace using the MMI */
/* NB. Traces are only for diagnostics while developing and must be removed for code deliveries!*/
/* defining mmi_trace as <empty> will remove them */
//#define MMI_TRACE
#define mmi_trace
//void mmi_trace(char* str);
/*==== EXPORT =====================================================*/
#define ENCODE_OFFSET 24
//define EASY_TEXT_ENABLED

/************************************************************************
* The following macros define what hardware we are running on and what 	*
* drivers are being used to drive the display							*
* BSAMPLE - 84x48 display												*
* CSAMPLE - 84x48 display												*
* CSAMPLE_COLOUR - no longer supported									*
* DSAMPLE - 176x220 display which can be driven as 16 bit colour or b+w	*
*      DSAMPLE_COLOUR - colour display 									*
*           DSAMPLE_COLOUR16 - 16 bits per pixel displayed 				*
*                              NB. This has not been tested at all - it	*
*                              may not display correctly or may crash	*
*                              although it does not crash when enabled 	*
*                              with the 32bit drivers)                	*
*                                                                       *
*           DSAMPLE_COLOUR32 - 32 bits per pixel displayed (16 bits are	*
*								 zero and have no effect)				*
*      DSAMPLE_LITE - 1 bit per pixel (b+w display)                    	*
* GW SPR#1840 25/03/03													*
* Also, the compiler switches have been added to allow us to develop  	*
* the MMI for a different final format (b+w/smaller display).			*
* COLOURDISPLAY - If not defined - all output is to be b+w				*
* LSCREEN - If not defined - screen is 'small' - use the 8x6 font 		*
*************************************************************************/
#if (defined(_TARGET_))
//#if (BOARD==33)
#if ((BOARD == 8) || (BOARD == 9))
#define CSAMPLE
#elif ((BOARD == 40) || (BOARD == 41))
#define DSAMPLE
#elif (BOARD == 43)
#define ESAMPLE
//June 03, 2005 REF: GSM-ENH-31636 xpradipg
#elif (BOARD == 70)
#define CALLISTO
//CRR 28825:  xpradipg - 11 Feb 2005
#elif (BOARD == 71)  // LCD enabling of ISAMPLE
#define ISAMPLE
#else
//Other board type
#define BSAMPLE
#endif
#else
//_TARGET_ not defined
//Not running on hardware - select the hw for simulation purposes.
//TISH modified for MSIM
//Begin
#define ISAMPLE
//End

//Also, if not already disabled - disable trace code
#ifdef MMI_TRACE
#undef MMI_TRACE
#endif
#endif
//June 03, 2005 REF: GSM-ENH-31636 xpradipg
//CRR 28825:  xpradipg - 11 Feb 2005
#if (defined(DSAMPLE) || defined(ESAMPLE) || defined(ISAMPLE) || defined(CALLISTO))
  /* For a start, we use exactly the same definitions for E-Sample as we do
     for D-Sample. IMPORTANT: If the E-Sample LCD differs from D-Sample, the
     following lines must be properly adapted!
  */
  #if (R2D_LCD_DISPLAY == R2D_COLOR)
    #define DSAMPLE_COLOUR
    #if (R2D_PIXEL_POS_TO_BIT_POS == 5)
      #define DSAMPLE_COLOUR32
    #endif
    #if (R2D_PIXEL_POS_TO_BIT_POS == 4)
      #define DSAMPLE_COLOUR16
    #endif
  #else
    #define DSAMPLE_LITE
    #undef COLOURDISPLAY
  #endif
#endif /* ESAMPLE */

#if defined (FF_MMI_NEW_LCD_API_MIG) || defined (FF_SSL_ADAPTATION) //xrashmic 22 Feb, 2006 SSL_Adaptation 
	#define DSAMPLE_COLOUR
//	#if ( LCD_PIXEL_FORMAT == RGB565)
		#define DSAMPLE_COLOUR16
//	#endif
#endif	
//Ensure that if the H/W does not support colour, the compiler option is overridden
//- we can display b+w only on a colour display but not the other way round
#ifndef DSAMPLE_COLOUR
#ifdef COLOURDISPLAY
#undef COLOURDISPLAY
#endif
#endif


/*
USE_PLOT -	Defined if we are to use r2d_draw_point to output a display
			This method will always produce a display (of sorts) but can be very slow
USE_BLIT_RECT - Defined if we provide a 24 bit colour display and allow the riviera code to
				translate into the LCD format.
USE_BUFFER - 	This must be defined if USE_BLIT_RECT is being used. This will require an image
				of the colour display to be stored in local memory.

If neither are defined then we must create our buffer in the same format as the LCD.
NB if 'USE_BLIT_RECT' is not defined, any changes to the h/w or driver software will have an
impact on the code below.

*/

/*==== CONSTANTS ==================================================*/
//	Mar 30, 2005	REF: CRR 29986	xpradipg
//	removed the static qualifier to be accessed by the ATB module.
#ifdef FF_MMI_OPTIM
const int translation_ascii_2_unicode[256] = {
#else
static const int translation_ascii_2_unicode[256] = {
#endif
	0x0000, 0x00A4, 0x00d8, 0xFFFF, 0x0394, 0x03A8, 0x00BF, 0x00F8, 0xFFFF, 0xFFFF, 0x000A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,0xFFFF, 0x001B, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026,0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
	0x0040,0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, /* SPR#2034 - SH */
	0xFFFF, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,

	0xFFFF, 0x00C7, 0x00FC, 0x00E9, 0x00E4, 0x00E0, 0x00E5, 0xFFFF, 0xFFFF, 0xFFFF, 0x00E8, 0xFFFF, 0xFFFF, 0x00EC, 0x00C4, 0x00C5, 
	0x00C9, 0x00E6, 0x00C6, 0xFFFF, 0x00F6, 0x00F2, 0xFFFF, 0x00F9, 0xFFFF, 0x00D6, 0x00DC, 0xFFFF, 0x00A3, 0x00A5, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x00F1, 0x00D1, 0xFFFF, 0xFFFF, 0x00A7, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x00A1, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0x00DF, 0x0393, 0x03A0, 0x03A3, 0xFFFF, 0xFFFF, 0xFFFF, 0x03A6, 0x0398, 0x03A9, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x039B, 
	0x039E, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
	
	};
/*==== VARIABLES ==================================================*/
//EXTERN UBYTE        extDisplay;          /* external display ? */
EXTERN UBYTE        extDisplay;          /* external display ? */
GLOBAL UBYTE dspl_char_type;

/*
 * We don't need display.c with ACI-only builds, but CST references extDisplay
 * above, so we need to keep that one for now.
 * Only process this file, if MMI is set to 1 (SMI) or 2 (BMI).
 */
#if (MMI != 0)

GLOBAL dspl_DevCaps displayData;
#if defined (NEW_FRAME)
EXTERN T_HANDLE       hCommL1;
#else
EXTERN T_VSI_CHANDLE  hCommL1;
#endif

static int scrEnabled = 0;              /* screen update enable     */


static T_HANDLE mmi_handle;
static T_HANDLE hCommMMI = VSI_ERROR;

/*==== FUNCTIONS ==================================================*/
/*
 * send to external Display
 */
LOCAL void dspl_SendDisplayReq (USHORT x, USHORT y, char *text);
GLOBAL void kbd_test (UBYTE key);

/*
 * TI driver functions
 */
extern void LCD_SetPower        (void);
extern void LCD_Reset           (void);
extern void LCD_Enable          (int);
extern void LCD_Clear           (void);
extern void LCD_WriteString     (int y, int x, char *s);
extern void LCD_PixBlt          (int x, int y, int w, int h, char *bmp);
extern void LCD_DrawIcon        (int x, int y, char id);
extern void LCD_TranslateBitmap (int w, int h, char *bo, char *bt);
extern void LCD_Cursor          (int row, int col);

GLOBAL void rt_ExtPrimitive ( USHORT TaskHandle, USHORT DestComHandle,
                              char *ExtDest, OS_QDATA *Msg );
/*
 * Internal Functions for Graphic Driver (Workaround)
 */
LOCAL  void scrInit      (void);
LOCAL  void scrExit      (void);
LOCAL  void scrSize      (int *x, int *y);
#if (!CUST)//JVJE #if (CUST==1)

LOCAL  void scrClear     (void);
#endif
LOCAL  void scrClearRect (int px, int py, int sx, int sy);
LOCAL  void scrText      (int x, int y, char *txt);
LOCAL  void scrText_Unicode      (int x, int y, USHORT *txt);
LOCAL  void scrLine      (int px, int py, int sx, int sy);
LOCAL  void scrRect      (int px, int py, int sx, int sy);
LOCAL  int  scrFntHeight (void);
LOCAL  int  scrFntGetFit (char *tp, int lineWidth);
LOCAL  int  scrFntGetLen (char *tp, int nChars);
#ifdef MMI_LINT_WARNING_REMOVAL 
LOCAL  int  scrBmpDraw   (int px, int py, int sx, int sy,
                          int ix, char *bm, int mode);
#endif
#ifdef MMI_LINT_WARNING_REMOVAL
LOCAL  void scrCursor    (int x, int y);
#endif
//accessible from outside e.g 3th application MMS
void scrPoint     (int px, int py, int col);
LOCAL  int  scrUpdate    (int onOff);

LOCAL  void scrText_Invert_Unicode (int x, int y, USHORT *txt); /*kk*/
#ifdef MMI_LINT_WARNING_REMOVAL
LOCAL int checkPixelColour(int px, int py, U32 col);
#endif
LOCAL void scrLCDUpdate (void);

void scrText_Invert (int x, int y, char *txt);

void scrDrawFilledRect (int px, int py, int sx, int sy, U32 col);

int scrGetRealLength(char *str, int* totChar, int* totPixel);

LOCAL void scrUnfocusDisplay(void);




void dspl_show_bitmap(int x, int y,t_font_bitmap* current_bitmap,U32 attr );




GLOBAL UBYTE dspl_SetWindow(USHORT   in_X1,
                         USHORT   in_Y1,
                         USHORT   in_X2,
                         USHORT   in_Y2);







/*==== CONSTANTS ==================================================*/

/*******************************************************************
 *                                                                 *
 * PART I: Driver Interface                                        *
 *                                                                 *
 *******************************************************************/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_Init                  |
+--------------------------------------------------------------------+

  PURPOSE : The function initializes the driver큦 internal data. The
            function returns DRV_INITIALIZED if the driver has already
            been initialized and is ready to be used or already in use.
            In case of an initialization failure, which means that the
            driver cannot be used, the function returns DRV_INITFAILURE.

*/
GLOBAL UBYTE dspl_Init (void)
{
  /*
   * Default is character Display
   */
  displayData.DisplayType = DSPL_TYPE_CHARACTER;

  if (!extDisplay)                /* use internal display */
  {
#if defined (SIMULATE_LCD)
    LCD_Reset  ();                /* use character oriented display */
    LCD_Enable (1);
    LCD_Clear  ();
  scrInit();          /* we need this also for glob vars, kk 14.02.2000*/
#else
    scrInit();                    /* use graphic display */
#endif
  }
  else
#if defined (SIMULATE_LCD)
  LCD_Enable (0);
#else
  scrExit();
#endif
  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_Exit                  |
+--------------------------------------------------------------------+

  PURPOSE : The function is called when the driver functionality is
            not longer needed. The function "de-allocates" all
            allocated resources and finalizes the driver.

*/

GLOBAL void dspl_Exit (void)
{
  scrExit();
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_Clear                 |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to clear a specific region of the
            display using the current background colour. The region
            is specified by the upper left corner (X1, Y1) and the
            lower right corner (X2, Y2) inclusive. The background
            color can be set using dspl_SetBkgColor () function.

*/

GLOBAL UBYTE dspl_Clear (USHORT   in_X1,
                         USHORT   in_Y1,
                         USHORT   in_X2,
                         USHORT   in_Y2)
{
  scrClearRect ((int)in_X1,(int)in_Y1,(int)(in_X2-in_X1+1),(int)(in_Y2-in_Y1+1));

  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_Clear                 |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to clear a specific region of the
            display using the current background colour. The region
            is specified by the upper left corner (X1, Y1) and the
            lower right corner (X2, Y2) inclusive. The background
            color can be set using dspl_SetBkgColor () function.

*/

GLOBAL UBYTE dspl_ClearAll (void)
{
#if (!CUST)
  scrClear ();
#endif

  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6147)         MODULE  : SMI_WM                   |
| STATE   : code                  ROUTINE : dspl_UnfocusDisplay      |
+--------------------------------------------------------------------+

  PURPOSE : Calls the function that will cause the entire display to appear much 
  			darker, allowing the pop-up dialog to be more clearly the focus 
  			especially the softkeys.
GW #2294 23/07/03 - Created
*/
GLOBAL UBYTE dspl_unfocusDisplay (void)
{
	scrUnfocusDisplay();
	return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_Enable                |
+--------------------------------------------------------------------+

  PURPOSE : The function is used to turn the display on or off. While
            a display is switched off, it is possible to perform any
            drawing function.

*/

GLOBAL UBYTE dspl_Enable (UBYTE   in_Enable)
{
  return scrUpdate (in_Enable);
}

/*
xreddymn Apr-07-2005 MMI-SPR-29655: Added function to update region
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_Enable_Region         |
+--------------------------------------------------------------------+

  PURPOSE : The function is used to update a region on the screen

*/

void dspl_Enable_Region(INT16 x1, INT16 y1, INT16 x2, INT16 y2)
{
#ifdef R2D_ENABLED
	r2d_flush_region(x1, y1, x2, y2);
#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_GetDeviceCaps         |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to retrieve the capabilities of the
            display device including the dimensions of the display and
            the logical unit in which the dimensions is measured.

*/

GLOBAL void dspl_GetDeviceCaps (dspl_DevCaps * out_DeviceCapsPtr)
{
  int x,y;

  scrSize (&x, &y);

  out_DeviceCapsPtr->Height = (USHORT)y;
  out_DeviceCapsPtr->Width  = (USHORT)x;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_SetDeviceCaps         |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to set the capabilities of the
            display device including the dimensions of the display and
            the logical unit in which the dimensions is measured.

*/

GLOBAL void dspl_SetDeviceCaps (dspl_DevCaps * in_DeviceCapsPtr)
{
  displayData.DisplayType = in_DeviceCapsPtr->DisplayType;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_GetIconImage          |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to copy the image of a driver
            internal icon into an application specific icon buffer.
            The icon may modify the icon. In case of a successful
            completion the function returns DRV_OK. In case the
            size of the buffer where the icon image shall be copied
            is too small the driver returns DRV_INVALID_PARAMS. In
            case a specific driver implementation does not support
            this functionality the driver returns DSPL_FCT_NOTSUPPORTED.

*/

GLOBAL UBYTE dspl_GetIconImage (UBYTE     in_Icon,
                                USHORT    in_Size,
                                UBYTE   * out_IconImagePtr)
{
  return DSPL_FCT_NOTSUPPORTED;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_SetCursor             |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to change the current cursor settings.
            These settings include the type of cursor and the mode
            (e.g. static cursor, not flashing). A set of standard
            types and modes is defined.

*/

GLOBAL UBYTE dspl_SetCursor (UBYTE in_CursorType,
                             UBYTE in_CursorMode)
{
  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_SetCursorPos          |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to set the current cursor position.
            If the function succeeds, the cursor is set to the new
            position. If one of the values is out of range, the function
            returns DRV_INVALID_PARAMS. The number of rows and columns
            the display supports can be retrieved using the function
            dspl_GetDevCaps(). The upper left corner has the coordinates
            0,0. The values in_X and in_Y are measured in logical units
            depending on the capabilities of the device. The means a
            graphical display will use pixels.

GW 05/09/01 Added size x and size y parameters to allow for chinese chars.
*/

GLOBAL UBYTE dspl_SetCursorPos (USHORT   in_X,
                                USHORT   in_Y,
                USHORT   in_SizeX,
                USHORT   in_SizeY
                )
{
  if (displayData.DisplayType EQ DSPL_TYPE_CHARACTER)
  {
     LCD_Cursor (in_Y, in_X);               /* set cursor position */
     if (extDisplay)                        /* if external display */
       dspl_SendDisplayReq (in_X, in_Y, NULL);
  }
  else
     scrLine(in_X,in_Y+in_SizeY-1,in_X+in_SizeX-1,in_Y+in_SizeY-1);
  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_ShowCursor            |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to change the status of the cursor.
            The cursor can be visible or invisible. The function returns
            the previous status of the cursor.

*/

GLOBAL UBYTE dspl_ShowCursor (UBYTE in_Show)
{
  return DSPL_CURSOR_VISIBLE;
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_SetBkgColor           |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to change the color used for
            background painting. If the color is out of range, the
            driver returns DRV_INVALID_PARAMS and leaves the color
            unchanged.

*/

GLOBAL UBYTE dspl_SetBkgColor (UBYTE in_Color)
{
  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_SetFrgColor           |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to change the color used for
            foreground painting, e.g. text color. If the color is
            out of range, the driver returns DRV_INVALID_PARAMS and
            leaves the color unchanged.
            set a call-back function which

*/

GLOBAL UBYTE dspl_SetFrgColor (UBYTE in_Color)
{
  return DSPL_FCT_NOTSUPPORTED;
}

enum
{
	COL_TYPE_FGD =0,
	COL_TYPE_BGD,
	COL_TYPE_HIGHLIGHT,
	COL_TYPE_BORDER,
	COL_TYPE_SHADOW,
	MAX_COL_TYPE
};
enum
{
	COL_CMD_INIT,
	COL_CMD_GET,
	COL_CMD_SET,
	COL_CMD_SETSHADOW,
	COL_CMD_RESTORECOL,
	MAX_COL_CMD
};

//This definition allows us to have a transparent colour in 256 colour bitmaps.
//We lose the colour nearly-black gray but I don't think it will be missed!
//Better to lose white than gray but will require all icons to be updated!
#define TRANSPARENT_256_COL 0x25

//This definition allows us to have a transparent colour in 16bit LCD colour bitmaps.
//We lose almost white R=0xF8 G=0xFC B=0xF8. NB word is inverted
#define TRANSPARENT_16BIT_COL 0x0020

//using ARGB format, transparent = 0xFFxxxxxx
#define TRANSPARENT_32BIT_COLOUR 0xFF000000

//Hide colours
LOCAL U32 colour_class (int cmd, int colId, U32 inColour)
{
static U32 g_col[MAX_COL_TYPE];
static U32 g_colOld[MAX_COL_TYPE];
	switch (cmd)
	{
		case COL_CMD_INIT:
			colour_class(COL_CMD_SET,	COL_TYPE_FGD,			0x00404040	);
			colour_class(COL_CMD_SET,	COL_TYPE_BGD,			0x00FFFFFF	); //white
			colour_class(COL_CMD_SET,	COL_TYPE_HIGHLIGHT,		0x00FF00FF	);
			colour_class(COL_CMD_SET,	COL_TYPE_BORDER,		0x00FFFFFF	);
			colour_class(COL_CMD_SET,	COL_TYPE_SHADOW,		0x40010101	); //50% translucency
			return (0);
			/*break;*/
		case COL_CMD_SET:
			if (inColour != 0)
			{
				g_colOld[colId]	=	g_col[colId];
				g_col[colId]	=	inColour;
			}
			return(g_colOld[colId]);
			/*break;*/
		case COL_CMD_GET:
#ifdef COLOURDISPLAY
#ifdef DSAMPLE_COLOUR
			return(g_col[colId]);
#endif
#else
			switch (colId)
			{
				case COL_TYPE_FGD:			return (0x00FFFFFF);
				case COL_TYPE_BGD:			return (0x00000000);
				case COL_TYPE_HIGHLIGHT:	return (0x00000000);
				case COL_TYPE_BORDER:		return (0x00000000);
				case COL_TYPE_SHADOW:		return (0x00000000);
				default:					return (0);
			}
#endif
			/*break;*/
		case COL_CMD_SETSHADOW:
			g_colOld[colId]	=	g_col[colId];//currently we dont really need to save the old value
			// set shadow to 50% black
			g_col[colId] = 0x40010101;// | ((inColour >> 2) & 0x003F3F3F);
			return(0x40010101);
			/*break;*/
		case COL_CMD_RESTORECOL:
			g_col[colId] = g_colOld[colId];
			return (0);
			/*break;*/
		default:
			break;
	}
	return (0);
}

GLOBAL U32 dspl_SetFgdColour (U32 inColour)			{	return (colour_class(COL_CMD_SET,		COL_TYPE_FGD,		inColour));	}
GLOBAL U32 dspl_SetBgdColour (U32 inColour)			{	return (colour_class(COL_CMD_SET,		COL_TYPE_BGD,		inColour));	}
GLOBAL U32 dspl_SetHighlightColour (U32 inColour)	{	return (colour_class(COL_CMD_SET,		COL_TYPE_HIGHLIGHT,	inColour));	}
GLOBAL U32 dspl_SetBorderColour (U32 inColour)		{	return (colour_class(COL_CMD_SET,		COL_TYPE_BORDER,  	inColour));	}
GLOBAL U32 dspl_SetShadowColour (U32 inColour)		{	return (colour_class(COL_CMD_SETSHADOW,	COL_TYPE_SHADOW,  	inColour));	}

GLOBAL U32 dspl_GetFgdColour ( void )				{	return (colour_class(COL_CMD_GET,		COL_TYPE_FGD,		0));	}
GLOBAL U32 dspl_GetBgdColour ( void )				{	return (colour_class(COL_CMD_GET,		COL_TYPE_BGD,		0));	}
GLOBAL U32 dspl_GetHighlightColour ( void )			{	return (colour_class(COL_CMD_GET,		COL_TYPE_HIGHLIGHT,	0));	}
GLOBAL U32 dspl_GetBorderColour ( void )			{	return (colour_class(COL_CMD_GET,		COL_TYPE_BORDER,	0));	}
GLOBAL U32 dspl_GetShadowColour (void)				{	return (colour_class(COL_CMD_GET,		COL_TYPE_SHADOW,  	0));	}

GLOBAL void dspl_InitColour (void)
{
	colour_class(COL_CMD_INIT,	0,	0);
}
GLOBAL void dspl_RestoreColour (void)
{
	colour_class(COL_CMD_RESTORECOL,	COL_TYPE_FGD,	0);
	colour_class(COL_CMD_RESTORECOL,	COL_TYPE_BGD,	0);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_DrawIcon              |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to draw a driver internal icon.
            The origin of the icon is the upper left corner, defined
            by the parameters in_X/in_Y.

*/

GLOBAL UBYTE dspl_DrawIcon (UBYTE  in_IconID,
                            USHORT in_X,
                            USHORT in_Y)
{
  return DSPL_FCT_NOTSUPPORTED;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_DrawLine              |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to draw a line from a specific
            location defined by the parameters in_X1/in_Y1, to a
            specific location defined by the parameters in_X2/in_Y2.
            The display큦 origin is the upper left corner with the
            co-ordinates (0,0). The function uses the current
            foreground color, which can be set using the
            dspl_SetFrgColor (), to draw the line.

*/

GLOBAL UBYTE dspl_DrawLine (USHORT in_X1,
                            USHORT in_Y1,
                            USHORT in_X2,
                            USHORT in_Y2)
{
    scrLine(in_X1,in_Y1,in_X2,in_Y2);

    return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_DrawRect              |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to draw a rectangle. The upper left
            corner of the rectangle is defined by the parameters
            in_X1/in_Y1. The lower right corner of the rectangle is
            defined by the parameters in_X2/in_Y2.
            The display큦 origin is the upper left corner with the
            co-ordinates (0,0). The function uses the current
            foreground color, which can be set using the
            dspl_SetFrgColor (), to draw the rectangle.

*/

GLOBAL UBYTE dspl_roundRectFill (int px,
                            	int  py,
                            	int sx,
                            	int sy,
                            	int border)
{
	int i,j,col;
	col = dspl_GetBgdColour();
	scrDrawFilledRect(px-border,py,	sx+border*2,sy, col );
	scrDrawFilledRect(px,py-border,	sx,sy+border*2, col );

	for (i=0;i<border;i++)
		for (j=0;j<border-i;j++)
		{
			scrPoint(px-i,		py-j,col);
			scrPoint(px+sx+i,	py-j,col);
			scrPoint(px+sx+i,	py+sy+j,col);
			scrPoint(px-i,		py+sy+j,col);
		}

  return DRV_OK;
}
GLOBAL UBYTE dspl_roundRect (int px,
                            	int  py,
                            	int sx,
                            	int sy,
                            	int border)
{
    scrLine(px ,py-border,		px+sx,py-border);
    scrLine(px,py+sy+border,	px+sx,py+sy+border);
    scrLine(px-border ,py,		px-border,py+sy);
    scrLine(px+sx+border,py,	px+sx+border,py+sy);

    scrLine(px ,py-border,		px-border ,py				);
    scrLine(px,py+sy+border,	px-border,py+sy			);
    scrLine(px+sx,py+sy+border,	px+sx+border,py+sy			);
    scrLine(px+sx+border,py,	px+sx,py-border		);

  return DRV_OK;
}

GLOBAL UBYTE dspl_DrawRect (USHORT in_X1,
                            USHORT in_Y1,
                            USHORT in_X2,
                            USHORT in_Y2)
{
  scrRect(in_X1,in_Y1,in_X2-in_X1,in_Y2-in_Y1);

  return DRV_OK;
}

GLOBAL UBYTE dspl_DrawFilledRect (USHORT in_X1,
								  USHORT in_Y1,
								  USHORT in_X2,
								  USHORT in_Y2)
{
  scrDrawFilledRect(in_X1,in_Y1,in_X2-in_X1,in_Y2-in_Y1, dspl_GetFgdColour() );
  return DRV_OK;
}
GLOBAL UBYTE dspl_DrawFilledBgdRect (USHORT in_X1,
								  USHORT in_Y1,
								  USHORT in_X2,
								  USHORT in_Y2)
{
  scrDrawFilledRect(in_X1,in_Y1,in_X2-in_X1,in_Y2-in_Y1, dspl_GetBgdColour() );
  return DRV_OK;
}
GLOBAL UBYTE dspl_DrawFilledColRect (USHORT in_X1,
								  USHORT in_Y1,
								  USHORT in_X2,
								  USHORT in_Y2,
								  U32 Col)
{
  scrDrawFilledRect(in_X1,in_Y1,in_X2-in_X1,in_Y2-in_Y1, Col );
  return DRV_OK;
}
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_DrawEllipse           |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to draw an ellipse. The center of
            the ellipse is the center of the bounding rectangle
            specified by the parameters.
            The display큦 origin is the upper left corner with the
            co-ordinates (0,0). The function uses the current
            foreground color, which can be set using the
            dspl_SetFrgColor (), to draw the ellipse.

*/

GLOBAL UBYTE dspl_DrawEllipse (USHORT in_X1,
                               USHORT in_Y1,
                               USHORT in_X2,
                               USHORT in_Y2)
{
  return DSPL_FCT_NOTSUPPORTED;
}




/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_BitBlt                |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to display a bitmap at the
            specified location using the raster operation provided.
            The bitmap format is customer specific but must include
            the size of the bitmap.


*/

GLOBAL UBYTE dspl_BitBlt2(short in_X,
                          short in_Y,
                          USHORT in_Width,
                          USHORT in_Height,
                          void * in_BmpPtr,
                          USHORT in_index,
                          int bmpFormat)
{
	t_font_bitmap current_bitmap;
	char *bmpPtr;
	int bmpSize;
	
	int attr = SHOWBITMAP_NORMAL;
	bmpPtr = (char *)in_BmpPtr;
	if (bmpFormat == BMP_FORMAT_256_COLOUR)
	{
		bmpSize = in_Height*in_Width; //bitmaps are 256 colour
		bmpPtr = &bmpPtr[bmpSize*in_index]; //for bitmaps that have multiple states
	}
	else if (bmpFormat == BMP_FORMAT_32BIT_COLOUR)
	{
		bmpSize = in_Height*in_Width; //bitmaps are 256 colour
		bmpPtr = &bmpPtr[bmpSize*in_index]; //for bitmaps that have multiple states
	}
	else if (bmpFormat == BMP_FORMAT_16BIT_LCD_COLOUR)
	{
#ifdef DSAMPLE_COLOUR32
		fastCopyBitmap(in_X,in_Y, 					// start position of bitmap
						in_Width,	in_Height,		//size of bitmap
						(char*)	in_BmpPtr,
						in_X,in_Y, 					// start position of output area
						in_Width,	in_Height,		//size of output area
						dspl_GetBgdColour(), bmpFormat);
		return DRV_OK;
#else
		bmpSize = in_Height*in_Width*4; //bitmaps are 16bit colour
		bmpPtr = &bmpPtr[bmpSize*in_index]; //for bitmaps that have multiple states
#endif
	}
	else if (bmpFormat == BMP_FORMAT_16BIT_LCD_COMPRESSED_COLOUR)
	{
#ifdef DSAMPLE_COLOUR16
		fastCopyBitmap(in_X,in_Y, 					// start position of bitmap
						in_Width,	in_Height,		//size of bitmap
						(char*)	in_BmpPtr,
						in_X,in_Y, 					// start position of output area
						in_Width,	in_Height,		//size of output area
						dspl_GetBgdColour(), bmpFormat);
		return DRV_OK;
#else
		bmpSize = in_Height*in_Width*2; //bitmaps are 16bit colour
		bmpPtr = &bmpPtr[bmpSize*in_index]; //for bitmaps that have multiple states
#endif
	}
	else //b+w image
	{
		#ifdef MMI_TRACE
		mmi_trace("b+w image");
		#endif
		if (bmpFormat == BMP_FORMAT_BW_2x4 )
		{
			//Temp fix to display larger b+w bitmaps (160x124)
			attr = SHOWBITMAP_SCALE_2x4;
			in_Width = in_Width /2;
			in_Height = in_Height /4;
			#ifdef MMI_TRACE
			mmi_trace("Icontype = scale 2x4");
			#endif
		}

		bmpSize = in_Height*((in_Width+7) >> 3); //bitmaps are unpacked
		bmpPtr = &bmpPtr[bmpSize*in_index]; //for bitmaps that have multiple states

	}
	current_bitmap.height	= in_Height;
	current_bitmap.width	= in_Width;
	current_bitmap.bitmapSize = 0;// bmpSize;
	current_bitmap.bitmap	= bmpPtr;
	current_bitmap.format	= bmpFormat;
	dspl_show_bitmap(in_X,in_Y, &current_bitmap, attr);

  return DRV_OK;

}

GLOBAL UBYTE dspl_BitBlt (USHORT in_X,
                          USHORT in_Y,
                          USHORT in_Width,
                          USHORT in_Height,
                          USHORT in_Index,
                          void * in_BmpPtr,
                          USHORT in_Rop)
{
#ifdef BSAMPLE
#ifdef MMI_LINT_WARNING_REMOVAL
//Use old procedure
  scrBmpDraw ((int)in_X, (int) in_Y, (int) in_Width,
              (int)in_Height, (int)in_Index,
              (char *)in_BmpPtr, (int)in_Rop);
#endif
  return DRV_OK;
#else
  return (dspl_BitBlt2( (short) in_X, (short) in_Y, in_Width, in_Height, in_BmpPtr, in_Index, BMP_FORMAT_BW_UNPACKED));
#endif

}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_SelectFontbyID        |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to select a font used for
            displaying text. Text can be displayed using the
            functions dspl_TextOut_Cmode.
            Driver specific fonts are identified be a font ID
            (parameter in_Font). The definition of fonts and font
            identifiers is not in scope of G23. Fonts and font
            identifiers have to be defined by the customer. The
            specific implementation of the display driver and the MMI
            using the driver have the knowledge about the available
            fonts, their identification and how to use them.

*/

GLOBAL UBYTE dspl_SelectFontbyID (UBYTE in_Font)
{
  return DSPL_FCT_NOTSUPPORTED;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_SelectFontbyImage     |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to select a font used for
            displaying text. Text can be displayed using the
            functions dspl_TextOut_Cmode.
            Application specific fonts are identified by the parameter
            in_FontPtr, the address of the buffer containing the
            application specific font. The structure of the font image
            in not in the scope of G23. The structure of the font images
            have to be defined by the customer implementing the driver.

*/

GLOBAL UBYTE dspl_SelectFontbyImage (UBYTE * in_FontPtr)
{
  return DSPL_FCT_NOTSUPPORTED;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_GetFontImage          |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to copy the image of a font into an
            application specific font buffer. The application may modify
            the font. In case of a successful completion the function
            returns DRV_OK. In case the size of the buffer where the
            font image shall be copied to is too small the driver returns
            DRV_INVALID_PARAMS. In case a specific driver implementation
            does not support this functionality the driver returns
            DSPL_FCT_NOTSUPPORTED.

*/

GLOBAL UBYTE dspl_GetFontImage (UBYTE    in_Font,
                                USHORT   in_Size,
                                UBYTE *  out_FontPtr)
{
  return DSPL_FCT_NOTSUPPORTED;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_GetFontHeight         |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to retrieve the vertical size of
            the currently selected font. The function returns the
            height measured in logical units depending on the device
            capabilities (e.g. pixels or multiple of characters). Call
            the function dspl_SelectFont() to select a font.

*/

GLOBAL UBYTE dspl_GetFontHeight (void)
{
  return scrFntHeight();
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_GetTextExtent         |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to calculate the size of a 0-terminated
            string, to which the parameter in_Text points. The function
            returns the size needed to display the text. The value of
            the size is measured in units depending on the device
            capabilities (e.g. pixels or multiple of characters). Call
            the function dspl_SelectFont() to select a font.

*/

GLOBAL USHORT dspl_GetTextExtent_old (char * in_Text, USHORT in_Length)
{
 if (displayData.DisplayType EQ DSPL_TYPE_CHARACTER)
   return in_Length;
 else
   return scrFntGetLen ( in_Text, ( int ) in_Length );
}


GLOBAL USHORT dspl_GetTextExtent (char * in_Text, USHORT in_Length)
{
	int nPixel=0;
	int nChar = in_Length;
	if (in_Text==NULL)
		return(0);

 	if (displayData.DisplayType EQ DSPL_TYPE_CHARACTER )
   	{
   		return in_Length;
   	}
	scrGetRealLength(in_Text, &nChar, &nPixel);

	return (nPixel);
}





int dspl_GetTextExtent2 (char * in_Text, USHORT in_Length)
{
	return (dspl_GetTextExtent ( in_Text, in_Length));

}
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_GetNcharToFit         |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to calculate the number of characters of the input
  string that will fit into the specified width.

*/
/*SPR 1541, added function*/
//Calculate number of chars that fit in the available space.
GLOBAL USHORT dspl_GetNcharToFit (char * in_Text, USHORT pixelWidth)
{
        int nPixel=pixelWidth;
        int nChar= 0;
        if (in_Text==NULL)
                return(0);

        scrGetRealLength(in_Text, &nChar, &nPixel);

        return (nChar);
}
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_GetMaxTextLen         |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to calculate the sub-string
            (number of characters) of a 0-terminated string (in_Text)
            that can be displayed in the region given by the parameter
            in_HSize. The value of in_HSize is measured in logical
            units (e.g. pixels or multiple of characters). Call
            the function dspl_SelectFont() to select a font.

*/

GLOBAL USHORT dspl_GetMaxTextLen (char  * in_Text,
                                  USHORT   in_HSize)
{
  if (displayData.DisplayType EQ DSPL_TYPE_CHARACTER)
    return strlen (in_Text);
  else
    return scrFntGetFit ( in_Text, ( int ) in_HSize );
}




GLOBAL void dspl_set_char_type(UBYTE char_type){
  dspl_char_type = char_type;
}

GLOBAL UBYTE dspl_get_char_type(void){
  return dspl_char_type;
}

int dspl_getDisplayType( void )
{
//	Sept 15, 2005 REF: LOCOSTO-ENH-34257 - xpradipg
// Jun 29, 2006 DR: OMAPS00083156  x0039928(sumanth)
// Fix: Check for flag FF_SSL_ADAPTATION is added.
#if (((defined(FF_MMI_NEW_LCD_API_MIG) ||defined(FF_SSL_ADAPTATION)) && ( defined(ISAMPLE) || defined (CALLISTO))) \
       || defined(COLOURDISPLAY))
	return DSPL_COLOUR;
#else /*a0393213 18th aug 2006-warnings removal-preprocessor statements modified*/
	#ifdef R2D_ENABLED
	if (R2D_LCD_DISPLAY == R2D_COLOR)
		return (DSPL_COLOUR);
	else
	#endif	
	return (DSPL_BW);
#endif
	
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_TextOut_Cmode               |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to display a text at the given
            location in_X/in_Y uding the defined attributes (in_Attrib)
            and the currently selected color (foreground and background).
            The cursor position is left unchanged. The driver will not
            take the displays bounding rectangle into consideration and
            therefore text may be cut off.
            Linefeeds and carriage returns are not supported and there-
            fore must be handled by upper layers.

GW 14/02/03 - SPR#1736 Modified code to call 'dspl_ScrText' to allow a border to be drawn
						round chinese characters.

*/


GLOBAL UBYTE dspl_TextOut_Cmode (USHORT  in_X,
                           USHORT  in_Y,
                           UBYTE   in_Attrib,
                           char * in_Text)
{
  int in_X_signed;
  int in_Y_signed;
	int txtStyle=0;
  // SH - If we want signed coordinates (i.e. if x and y can have negative numbers)
  // we need to convert the unsigned values into two's complement.

   if (in_Attrib & DSPL_TXTATTR_SIGNED_COORDS)
    {
    in_X_signed = (SHORT)in_X;
    in_Y_signed = (SHORT)in_Y;
    }
   else
    {
    in_X_signed = in_X;
    in_Y_signed = in_Y;
    }

#if defined (WIN32)
   if (scrEnabled)
   {
     if (in_Attrib & DSPL_TXTATTR_INVERS)
     {
       scrText_Invert(in_X, in_Y, in_Text);
     }
     else
     {
       scrText(in_X, in_Y, in_Text);
     }
   }
   else
     LCD_WriteString (in_Y, in_X, (UBYTE *) in_Text);
#else
   if (displayData.DisplayType EQ DSPL_TYPE_CHARACTER)
   {
     if (in_Attrib & DSPL_TXTATTR_INVERS)
   	   {
    if (dspl_char_type == DSPL_TYPE_ASCII)
      scrText_Invert(6 * in_X + 2, 8 * in_Y, in_Text);
    else
      scrText_Invert_Unicode(12 * in_X + 2, 12 * in_Y,(USHORT*) in_Text);
     }
     else
     {
    if (dspl_char_type == DSPL_TYPE_ASCII)
      scrText(6 * in_X_signed + 2, 8 * in_Y_signed, in_Text);
    else
      scrText_Unicode(12 * in_X_signed + 2, 12 * in_Y_signed, (USHORT*)in_Text);
     }
   }
   else
   {
   		txtStyle = 0;
   		if (in_Attrib & DSPL_TXTATTR_INVERS)
   			txtStyle = TXT_STYLE_INVERT;
   		else
   			txtStyle = TXT_STYLE_NORMAL;
		if (dspl_char_type != DSPL_TYPE_ASCII)
   			txtStyle = txtStyle  | TXT_STYLE_UNICODE;
   	 	dspl_ScrText( in_X_signed,in_Y_signed,in_Text, txtStyle);
//      		scrText_Unicode(in_X_signed,in_Y_signed, (USHORT*)in_Text);

   }
#endif
   /*
    * if an external display simulation is used and switched on with the
    * EXT_DISPLAY config primitive - send a MMI_DISPLAY_REQ primitive.
    */
   if (extDisplay)
     dspl_SendDisplayReq (in_X, in_Y, in_Text);

  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_TextOut                 |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to display a text at the given
            location in_X/in_Y using the defined attributes (in_Attrib)
			It calls dspl_ScrText to actually display the text

GW 14/02/03 - SPR#1736 Modified code to call 'dspl_ScrText' to allow a border to be drawn
						round chinese characters.
*/

GLOBAL UBYTE dspl_TextOut(USHORT  in_X,
                           USHORT  in_Y,
                           UBYTE   in_Attrib,
                           char * in_Text)
{
	
	int in_X_signed;
	int in_Y_signed;
	int txtStyle=0;
	char* op_str;

	/*MC SPR 1319*/
	if (in_Text == NULL)
		return DRV_OK;
	/*MC end*/

	op_str = in_Text;

//TISH modified for MSIM
//Begin
/*
#if defined (WIN32)
	if (!scrEnabled)
	{
		LCD_WriteString (in_Y, in_X, (UBYTE *) in_Text);
	}
	else
#endif
*/
//End
	{

		if (in_Attrib & DSPL_TXTATTR_SIGNED_COORDS)
		{
			in_X_signed = (SHORT)in_X;
			in_Y_signed = (SHORT)in_Y;
		}
		else
		{
			in_X_signed = in_X;
			in_Y_signed = in_Y;
		}

		/*Mc SPR 1526, we no longer check for current mode*/
		if (in_Attrib & DSPL_TXTATTR_INVERS)
			txtStyle = TXT_STYLE_INVERT;
		else
			txtStyle = TXT_STYLE_NORMAL;


//TISH modified for MSIM
		if ((in_Attrib&DSPL_TXTATTR_UNICODE) || ((UBYTE)in_Text[0] ==0x80))
		{
			txtStyle = txtStyle | TXT_STYLE_UNICODE;
//TISH modified for MSIM
			if ((UBYTE)op_str[0] ==0x80)//if unicode tag at beginning of string
				op_str = &op_str[2];
		}
		if (displayData.DisplayType EQ DSPL_TYPE_CHARACTER)
		{
			/* the following is only accurate for a fixed-width font*/
			if (txtStyle & TXT_STYLE_UNICODE)
			{
				in_X_signed = in_X_signed*12+2;
				in_Y_signed = in_Y_signed*12;
			}
			else
			{
				in_X_signed = in_X_signed*6+2;
				in_Y_signed = in_Y_signed*8;
			}
		}

   	 	dspl_ScrText( in_X_signed,in_Y_signed,op_str, txtStyle);
	}
   /*
    * if an external display simulation is used and switched on with the
    * EXT_DISPLAY config primitive - send a MMI_DISPLAY_REQ primitive.
    */
   if (extDisplay)
     dspl_SendDisplayReq (in_X, in_Y, in_Text);

	return DRV_OK;
}
/**/



/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_SetWorkShadow         |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to define the address of the
            shadow RAM for drawing operations.

*/

GLOBAL UBYTE dspl_SetWorkShadow (UBYTE * in_ShadowPtr)
{
  return DSPL_FCT_NOTSUPPORTED;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_SetDisplayShadow      |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to define the address of the
            shadow RAM for displaying operations.

*/

GLOBAL UBYTE dspl_SetDisplayShadow (UBYTE * in_ShadowPtr)
{
  return DSPL_FCT_NOTSUPPORTED;
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6147)         MODULE  : SMI_WM                   |
| STATE   : code                  ROUTINE : dspl_SendDisplayReq      |
+--------------------------------------------------------------------+

  PURPOSE : sends an MMI_DISPLAY_REQ primitive to write
            the text at position X,Y to PL. If the parameter
            text is NULL only the cursor position
            should change. If in this case the X and Y value
            is 0xffff the cursor will be hide.

*/
LOCAL void dspl_SendDisplayReq (USHORT x, USHORT y, char *text)
{
#ifdef NEW_FRAME

T_VOID_STRUCT *prim;
#endif

  USHORT sdu_len = (text NEQ NULL) ? strlen (text) : 0;
  PALLOC_SDU (mmi_display_req, MMI_DISPLAY_REQ, (USHORT)(sdu_len<<3));

  mmi_display_req->c_x = x;
  mmi_display_req->c_y = y;

  if (text NEQ NULL)
  {
    /*
     * text output
     */
    mmi_display_req->attrib.content = CON_TEXT;
    mmi_display_req->attrib.control = CTL_NORMAL;
    mmi_display_req->sdu.o_buf = 0;
    mmi_display_req->sdu.l_buf = (sdu_len<<3);
    memcpy (mmi_display_req->sdu.buf, text, sdu_len);
  }
  else
  {
    /*
     * change the cursor position
     */
    mmi_display_req->attrib.content = CON_CURSOR;
    mmi_display_req->attrib.control = ((x EQ 0xffff) ? CTL_HIDDEN : CTL_NORMAL);
    mmi_display_req->sdu.o_buf = 0;
    mmi_display_req->sdu.l_buf = 0;
  }

#if defined NEW_FRAME
    if ( hCommMMI == VSI_ERROR )
    {
      mmi_handle = vsi_e_handle ( 0, "MMI" );
      hCommMMI = vsi_c_open ( 0, "MMI" );
      prim = vsi_c_pnew ( 100, 0x8000 FILE_LINE_MACRO );
      strcpy ((char*)prim, "REDIRECT MMI 0000111000001010 PAN");
      vsi_c_primitive ( mmi_handle, D2P(prim) );
    }
    PSEND(hCommMMI,mmi_display_req);

#else
    ext_primitive (D2P(mmi_display_req), "CST", "PAN",
                   sizeof (T_PRIM_HEADER) +
                   sizeof (T_MMI_DISPLAY_REQ) +
                   sdu_len);
    PFREE (mmi_display_req);
#endif
}

GLOBAL UBYTE dspl_DrawWin (USHORT px, USHORT py, USHORT sx, USHORT sy, int format, t_font_bitmap *bgdBmp)
{
	USHORT x1,y1,x2,y2;
	int xOfs,yOfs;

	if (bgdBmp == NULL)
	{
		scrDrawFilledRect( px+3,py+3, sx, sy, 0x00010101 ); //shadow
		scrDrawFilledRect( px,py, sx, sy, dspl_GetBgdColour() );

		scrRect( px-1,py-1, sx+1, sy+1);
	}
	else
	{
		xOfs = (sx-bgdBmp->width)/2;
		yOfs = (sy-bgdBmp->height)/2;

		dspl_GetWindow(&x1,&y1,&x2,&y2 );

		switch (format)
		{
			case DSPL_WIN_NORMAL:
					dspl_show_bitmap(px, py, bgdBmp, SHOWBITMAP_NORMAL );
					break;
			case DSPL_WIN_CENTRE:
					dspl_show_bitmap(px+xOfs, py+yOfs, bgdBmp, SHOWBITMAP_NORMAL );
					break;
			case DSPL_WIN_CLIP:
					dspl_SetWindow(px,py,px+sx,px+sy );
					dspl_show_bitmap(px, py, bgdBmp, SHOWBITMAP_NORMAL );
					break;
			case DSPL_WIN_CENTRE_CLIP:
					dspl_SetWindow(px,py,px+sx,px+sy );
					dspl_show_bitmap(px+xOfs, py+yOfs, bgdBmp, SHOWBITMAP_NORMAL );
					break;
			case DSPL_WIN_TILE:
					dspl_SetWindow(px,py,px+sx,px+sy );
					yOfs=0;
					while(yOfs < sy)
					{
						xOfs=0;
						while (xOfs < sx)
						{
							dspl_show_bitmap(px+xOfs, py+yOfs, bgdBmp, SHOWBITMAP_NORMAL );
							xOfs=xOfs+bgdBmp->width;
						}
						yOfs=yOfs+bgdBmp->height;
					}
					break;
		}
		dspl_SetWindow(x1,y1,x2,y2 );
	}

/*	switch (format)
	{
		case 0:
		default:
		break;
	}*/
	return DRV_OK;
}
static int win_x1, win_x2, win_y1, win_y2;

//Set-up area to be drawn in - limit window to the size of the screen
//Inputs in_x1/y1 - first column/row that can be drawn on
//		in_x2/y2 - last column/row
//NB:-	inputs are the same as used to clear/draw rectangles BUT
//		values stored in win_x2/y2 are +1 to ensure values are treated the same as scrX/Y
GLOBAL UBYTE dspl_SetWindow(USHORT   in_X1,
                         USHORT   in_Y1,
                         USHORT   in_X2,
                         USHORT   in_Y2)
{
int sizeX,sizeY;

	scrSize (&sizeX, &sizeY);
	if (in_X1>0)
		win_x1 = in_X1;
	else
		win_x1 = 0;
	if (in_Y1>0)
		win_y1 = in_Y1;
	else
		win_y1 = 0;
	if (in_X2<sizeX)
		win_x2 = in_X2+1;
	else
		win_x2 = sizeX;
	if (in_Y2<sizeY)
		win_y2 = in_Y2+1;
	else
		win_y2 = sizeY;
	return DRV_OK;

}

GLOBAL UBYTE dspl_ResetWindow( void )
{
	win_x1 = 0;
	win_y1 = 0;
	scrSize (&win_x2, &win_y2);

	return DRV_OK;

}

GLOBAL UBYTE dspl_GetWindow(USHORT* x1,USHORT* y1,USHORT* x2,USHORT* y2 )
{
	*x1= win_x1;
	*x2= win_x2;
	*y1= win_y1;
	*y2= win_y2;
	return DRV_OK;
	
}

/*******************************************************************
 *                                                                 *
 * PART II: Workaround for Graphical Displays                      *
 *                                                                 *
 *******************************************************************/
//June 03, 2005 REF: GSM-ENH-31636 xpradipg
//CRR 28825:  xpradipg - 11 Feb 2005
#if (defined(DSAMPLE) || defined(ESAMPLE) || defined(ISAMPLE) || defined(CALLISTO))
/*GW 21/02/03 - There is an unused long word at the end of each line for the new drivers for some good reason I'm sure.*/
#ifdef DSAMPLE_COLOUR16
#ifdef FF_MMI_NEW_LCD_API_MIG
#define LCD_COL	(LCD_WIDTH+1)
#else 
#ifdef FF_SSL_ADAPTATION //xrashmic 22 Feb, 2006 SSL_Adaptation 
#define LCD_COL	176
#else//FF_SSL_ADAPTATION
#define LCD_COL   (R2D_WIDTH+1)                      /* display size+32 bits    */
#endif //FF_SSL_ADAPTATION
#endif
#else
#define LCD_COL   (R2D_WIDTH)                      /* display size             */
#endif
#ifdef FF_MMI_NEW_LCD_API_MIG
#define LCD_ROW (LCD_HEIGHT)
#else
#ifdef FF_SSL_ADAPTATION //xrashmic 22 Feb, 2006 SSL_Adaptation 
#define LCD_ROW 220
#else//FF_SSL_ADAPTATION
#define LCD_ROW 	(R2D_HEIGHT)
#endif //FF_SSL_ADAPTATION
#endif

#define HEADER_INFO (3)
#define DISPLAY_SIZE (LCD_ROW*LCD_ROW_NUM_LWORDS)
/* static char scratch [100];      *//* command buffer - does not support output of new display  */
#ifndef DSAMPLE_LITE
unsigned char bgdColourArray[LCD_COL*4];
#endif
#endif

#ifdef CSAMPLE
#define LCD_COL 84                      /* display size             */
#define LCD_ROW 48
#define LCD_COL_NUM_LWORDS ((LCD_ROW+31)/32) /* col size in longwords         */
#endif //CSAMPLE

#ifdef BSAMPLE
#define LCD_COL 84                     /* display size             */
#define LCD_COL_B ((LCD_COL+7)/8)       /* row size in byte         */
#define LCD_ROW 48
#define DISPL_DIV 4
#endif

/*
LCD_ROW_NUM_LWORDS - number of long words used to store a row of pixels
BYTEPERPIXEL - number of bytes used to store apixel on-screen
*/

#define CHECK_LIMITS /*Add bounds checks to code */
/*******************************************************************************
 $Macro:    	DSPL_SET_BITCOLOUR
 $Description:	This macro sets the colour of the bit at the provided long word address
 $Returns:		None.
 $Arguments:	x - x position of pixel (may be unused)
 				j - 32 bit offset within array of the pixel to be set
 				col - colour (in LCD format) of the pixel to be set
 				txt - debug text for traces - used if 'j' is bounds checked first.
*******************************************************************************/
/*******************************************************************************
 $Macro:    	DSPL_GET_LWORD
 $Description:	This macro
 $Returns:		The 32bit offset of the word containing the pixel
 $Arguments:	px - x position of pixel
 				py - y position of pixel
*******************************************************************************/
/*******************************************************************************
 $Macro:    	DSPL_GET_BYTE
 $Description:	This macro
 $Returns:		The 8bit offset of the word containing the pixel
 $Arguments:	px - x position of pixel
 				py - y position of pixel
*******************************************************************************/
/*******************************************************************************
 $Macro:    	DSPL_GET_BITCOLOUR
 $Description:	This macro
 $Returns:		The colour in LCD format of the RGB input
 $Arguments:	col - 32 bit RGB colour value
*******************************************************************************/
/*******************************************************************************
 $Macro:    	DSPL_SET_XYBIT_COLOUR
 $Description:	This macro
 $Returns:		None
 $Arguments:	x,y - posiiton of pixel to be set
 				col - 0 =clear bixel, anything else - set pixel
 				txt - trace for debug (if position is out-of-range)
*******************************************************************************/
#ifdef DSAMPLE_LITE
#define LCD_ROW_NUM_LWORDS ((LCD_COL+31)/32) /* col size in longwords         */
#define BYTEPERPIXEL (1/8 - unused)
#ifdef CHECK_LIMITS
#define DSPL_SET_BITCOLOUR(x,j,col,txt) \
{\
if (((j)>=0) && ((j)<DISPLAY_SIZE)) \
{\
	int m_px = 1 << (x & 0x1F);\
	if (col)  picture_col[j] = picture_col[j] | (m_px);\
	else    picture_col[j] = picture_col[j]  & (~m_px);\
}else {mmi_trace(txt);}\
}
#define DSPL_SET_XYBIT_COLOUR(x,y,col,txt) \
{\
	int m_px = 1 << (x & 0x1F);\
	int word = DSPL_GET_LWORD(x,y);\
	if (((word)>=0) && ((word)<DISPLAY_SIZE)) \
	{\
		if (col) word = word | m_px;\
		else word = word & (~m_px);\
	}\
}

#else
#define DSPL_SET_BITCOLOUR(x,j,col,txt) \
{\
	int m_px = 1 << (x & 0x1F);\
	if (col) picture_col[j] = picture_col[j] | (m_px);\
	else  picture_col[j] = picture_col[j]  & (~m_px);\
}
#define DSPL_SET_XYBIT_COLOUR(x,y,col,txt) \
{\
	int m_px = 1 << (x & 0x1F);\
	int word = DSPL_GET_LWORD(x,y);\
	if (col) word = word | m_px;\
	else word = word & (~m_px);\
}

#endif
#define DSPL_GET_LWORD(px,py) ((py)*LCD_ROW_NUM_LWORDS + ((px)/32))
#define DSPL_GET_BYTE(px,py) (((py)*4)*LCD_ROW_NUM_LWORDS + ((px)/8))
#define DSPL_GET_BITCOLOUR(col32) (not used!)
#define DSPL_GET_PIXELCOLOUR(col32) (not used!)
#endif //DSAMPLE_LITE




#ifdef DSAMPLE_COLOUR32
#define LCD_ROW_NUM_LWORDS (LCD_COL) /* col size in longwords         */
#define BYTEPERPIXEL 4
#ifdef CHECK_LIMITS
//safe version-includes trace for errors
#define DSPL_SET_BITCOLOUR(x,j,col,txt) {if (((j)>=0) && ((j)<DISPLAY_SIZE)) picture_col[j] = (col); else {mmi_trace(txt);}}
#else
#define DSPL_SET_BITCOLOUR(x,j,col,txt) {picture_col[j] = (col);}//faster version - no bounds check
#endif
#define DSPL_GET_LWORD(px,py) ((py)*LCD_ROW_NUM_LWORDS + (px))
#define DSPL_GET_BYTE(px,py) (((py)*4)*LCD_ROW_NUM_LWORDS + ((px)*4))
#define DSPL_GET_BITCOLOUR(col32) (~(((((col32) &0x00F80000)  >> 8) | (((col32) &0x0000FC00)	>> 5) | (((col32) &0x000000F8)	>> 3))))
#define DSPL_GET_PIXELCOLOUR(col32) (((((col32)<<8) &0x00F80000) | (((col32)<<5) &0x0000FC00) | (((col32)<<3) &0x000000F8)))
#endif

#ifdef DSAMPLE_COLOUR16
//	July 1, 2005 REF: CRR 32677 - xpradipg
//	the extra word padding is not required for the ISAMPLE AND CALLISTO
#if (defined(ISAMPLE) || defined(CALLISTO))
#define LCD_ROW_NUM_LWORDS ((LCD_COL)/2) /* col size in longwords         */
#else
#define LCD_ROW_NUM_LWORDS ((LCD_COL+1)/2) /* col size in longwords         */
#endif
#define BYTEPERPIXEL 2
#ifdef PIXEL_0_IN_LSW
#ifdef CHECK_LIMITS
#define DSPL_SET_BITCOLOUR(x,j,col,txt) \
{\
if (((j)>=0) && ((j)<DISPLAY_SIZE)) \
{	if ((x)&0x01)\
		picture_col[j] = (picture_col[j] & 0xFFFF0000) | (col & 0x0000FFFF);\
	else\
		picture_col[j] = (picture_col[j] & 0x0000FFFF) | ((col <<16) & 0xFFFF0000);\
}else {mmi_trace(txt);}\
}
#else
#define DSPL_SET_BITCOLOUR(x,j,col,txt) \
{	if ((x)&0x01)\
		picture_col[j] = (picture_col[j] & 0xFFFF0000) | (col & 0x0000FFFF);\
	else\
		picture_col[j] = (picture_col[j] & 0x0000FFFF) | ((col <<16) & 0xFFFF0000);\
}
#endif
#else
#ifdef CHECK_LIMITS
#define DSPL_SET_BITCOLOUR(x,j,col,txt) \
{\
if (((j)>=0) && ((j)<DISPLAY_SIZE)) \
{	if ((x)&0x01)\
		picture_col[j] = (picture_col[j] & 0x0000FFFF) | ((col <<16) & 0xFFFF0000);\
	else\
		picture_col[j] = (picture_col[j] & 0xFFFF0000) | (col & 0x0000FFFF);\
}else {}\
}
#else
#define DSPL_SET_BITCOLOUR(x,j,col,txt) \
{	if ((x)&0x01)\
		picture_col[j] = (picture_col[j] & 0x0000FFFF) | ((col <<16) & 0xFFFF0000);\
	else\
		picture_col[j] = (picture_col[j] & 0xFFFF0000) | (col & 0x0000FFFF);\
}
#endif

#endif
//GW Added () round 'px' in definition below
#define DSPL_GET_LWORD(px,py) ((py)*LCD_ROW_NUM_LWORDS + ((px)/2))
#define DSPL_GET_BYTE(px,py) (((py)*4)*LCD_ROW_NUM_LWORDS + ((px)*2))
//	July 1, 2005 REF: CRR 32677 - xpradipg
//	the value is not negated here since the LCD driver expects without negation
#if (defined(ISAMPLE) || defined(CALLISTO))
#define DSPL_GET_BITCOLOUR(col32) (((((col32) &0x00F80000)  >> 8) | (((col32) &0x0000FC00)	>> 5) | (((col32) &0x000000F8)	>> 3)))
#else
#define DSPL_GET_BITCOLOUR(col32) (~(((((col32) &0x00F80000)  >> 8) | (((col32) &0x0000FC00)	>> 5) | (((col32) &0x000000F8)	>> 3))))
#endif
#define DSPL_GET_PIXELCOLOUR(col32) (((((col32)<<8) &0x00F80000) | (((col32)<<5) &0x0000FC00) | (((col32)<<3) &0x000000F8)))
#endif

// 3 words of header info for 'pictures'
#ifdef DSAMPLE_LITE
static UINT32*  picture_bw    = NULL;//Pointer to data in display.
static UINT32*  pictureCopy_bw= NULL;//Pointer to data in display.
#endif//DSAMPLE_LITE

#ifdef DSAMPLE_COLOUR
static UINT32*  picture_col    = NULL;//Pointer to data in display.
//	Mar 30, 2005	REF: CRR 29986	xpradipg
//	removed the unused variable
#ifndef FF_MMI_OPTIM
static UINT32*  pictureCopy_col= NULL;//Pointer to data in display.
#endif
#endif

#ifdef CSAMPLE
#define HEADER_INFO (3)
#define DISPLAY_SIZE (LCD_COL*LCD_COL_NUM_LWORDS)
static UINT32 	display_bw [DISPLAY_SIZE+HEADER_INFO+10];
#ifdef USE_PLOT
static UINT32 	displayCopy_bw [DISPLAY_SIZE+HEADER_INFO+1];   /* copy to flag differences */
#endif
static UINT32*  picture_bw=NULL;//Pointer to data in display.
static UINT32*  pictureCopy_bw= NULL;//Pointer to data in display.
/* static char scratch [100];      */ /* command buffer - does not support output of new display  */
#endif //CSAMPLE

#ifdef BSAMPLE
static UBYTE displayScr [LCD_ROW][LCD_COL_B];   /* display buffer           */
static UBYTE displayCopy [LCD_ROW][LCD_COL_B];   /* copy to flag differences */
/* static char scratch [LCD_ROW*LCD_COL_B*2+100]; */     /* command buffer           */
#endif //BSAMPLE


//Sept 15, 2005 REF: LOCOSTO-ENH-34257 - xpradipg
//the frame buffer will be defined in the MMI, this has to be put under the
//section DMA in the memory - need to find out how it is done
#if defined(ISAMPLE ) || defined(CALLISTO)
#if defined( FF_MMI_NEW_LCD_API_MIG) || defined(FF_SSL_ADAPTATION) //xrashmic 22 Feb, 2006 SSL_Adaptation 
#pragma DATA_SECTION(lcd_frame_buffer, ".r2dbuffer")
UINT32 lcd_frame_buffer [LCD_ROW*LCD_ROW_NUM_LWORDS];
#endif
#endif

#ifdef BSAMPLE /*a0393213 lint warnings removal - variable put under #ifdef*/
static int mobEnabled = 1;              /* mobile simulation disabled      */
#endif

extern int mfwExtOut (char *cmd);       /* forward to ext. display  */
UBYTE lcd_output_ready  = TRUE;
UBYTE lcd_output_needed = FALSE;

#define scrHlen 18
#ifdef BSAMPLE
static const char *scrHeader = "CONFIG MFWMOB SCR ";
#endif

static int scrX, scrY;                  /* display size             */
#if defined (WIN32)
static int mtkEnabled = 0;              /* MMI Toolkit              */
#endif

static int internal = 0;                /* internal use only        */
#ifdef BSAMPLE /*a0393213 lint warnings removal - #ifdef added*/
static int xIndex;                      /* data transfer index      */
#endif

//accessible from outside e.g 3th application MMS
void scrPoint (int px, int py, int col);
#if (BOARD == 70) || (BOARD == 71)
U8* get_LCD_bitmap(void)
{
       return NULL;
}

U32 get_screen_size(void)
{
    return 0;
}

void get_screen_capture(U8 *dest_data)
{

}

#else

//xrashmic 22 Aug, 2004 MMI-SPR-32798
#ifdef DSAMPLE_COLOUR16
#define SC_BITMAP_SIZE ( ((R2D_WIDTH + 7) / 8) * R2D_HEIGHT + 2 * sizeof(U16))
#endif
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : get_LCD_bitmap                |
+--------------------------------------------------------------------+

  PURPOSE : Return the frame buffer  
                   xrashmic 22 Aug, 2004 MMI-SPR-32798
                   Implemented by xreddymn
*/

U8* get_LCD_bitmap(void)
{
#ifdef DSAMPLE_COLOUR16
	return((U8*)picture_col);
#else
       return NULL;
#endif
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : get_screen_size                |
+--------------------------------------------------------------------+

  PURPOSE : Return the size of the bitmap to hold the screen shot
                   xrashmic 22 Aug, 2004 MMI-SPR-32798
                   Implemented by xreddymn
*/
U32 get_screen_size(void)
{
#ifdef DSAMPLE_COLOUR16
    return SC_BITMAP_SIZE;
#else
    return 0;
#endif
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : get_screen_capture                |
+--------------------------------------------------------------------+

  PURPOSE : Dumps the frame buffer data(16bit) into a monochrome data
                   xrashmic 22 Aug, 2004 MMI-SPR-32798
                   Implemented by xreddymn
*/

void get_screen_capture(U8 *dest_data)
{
#ifdef DSAMPLE_COLOUR16
	U8 *source_data;
	U32 source_offset=0;
	U32 dest_offset=4;
	S32 i,j;
	U16 pixel_RGB16;
	U8 R, G, B;
	S32 avg;
	U8 color=0; 

	/* First two fields (u16) are width and height */
	dest_data[0]=((R2D_WIDTH) & 0xFF);
	dest_data[1]=((R2D_WIDTH) & 0xFF00)>>8;
	dest_data[2]=((R2D_HEIGHT) & 0xFF);
	dest_data[3]=((R2D_HEIGHT) & 0xFF00)>>8;
	source_data=get_LCD_bitmap();
	for(j=0;j<R2D_HEIGHT;j++)
	{
		for(i=0;i<R2D_WIDTH;i++)
		{
			if((i%8)==0)
			{
				dest_data[dest_offset]=0;
				dest_offset++;
			}
			pixel_RGB16=source_data[source_offset++];
			pixel_RGB16|=source_data[source_offset++]<<8;
			pixel_RGB16=~pixel_RGB16;
			R=((pixel_RGB16>>11)<<3)|0x07;
			G=(((pixel_RGB16>>5)&0x3f)<<2)|0x03;
			B=((pixel_RGB16&0x1f)<<3)|0x07;
			avg=R+G+B;
			avg/=3;
			if(avg<0x80) color=0;
			else color=1;
			dest_data[dest_offset-1]|=(color<<(i%8));
		}
		source_offset+=4;	/* Add framebuffer padding bytes here */
	} 
#endif    
}
#endif
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_DSPL                   |
| STATE   : code                ROUTINE : dspl_BitBlt                |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to display a bitmap at the
            specified location using the raster operation provided.
            The bitmap format MUST be the same as the display and
            cannot contain any transparent colours
            If the bitmap does not cover the entire screen area, the rest of the
            area is drawn in the background colour provided.

*/
#ifdef DSAMPLE_COLOUR
void fastCopyBitmap(int startX, 	int startY, 	// start position of bitmap
						int bmpSx,	int bmpSy,		//size of bitmap
						char*	srcBitmap,
						int posX,   	int posY,   	// start of area to be copied into
						int sx,     	int sy,     	// size of area to be copied into
						U32 bgd_col,	int bmpFormat)
{
	char * image;
	int image_sx,image_x;
	int bgd_sx_left;
	int bgd_sx_right;
	int i,y;
	char *screen = (char*)picture_col;
	U32 bgdColour;

	if (posX < win_x1)
	{
		sx = sx - (win_x1-posX);
		posX = win_x1;
	}
	if (posX+sx >win_x2)
	{
		sx = win_x2 - posX;
	}
	if (posY < win_y1)
	{
		sy = sy - (win_y1-posY);
		posY = win_y1;
	}
	if (posY+sy >win_y2)
	{
		sy = win_y2 - posY;
	}
//TISH modified for MSIM
/*
{
	char trc[80];
	sprintf(trc,"%d,%d,%d,%d,%d,%d",sx,sy,win_x1,win_y1,win_x2,win_y2);
	#ifdef MMI_TRACE
	mmi_trace(trc);
	#endif
}
*/
#ifdef DSAMPLE_COLOUR32
	if (bmpFormat != BMP_FORMAT_16BIT_LCD_COLOUR)
#endif
#ifdef DSAMPLE_COLOUR16
	if (bmpFormat != BMP_FORMAT_16BIT_LCD_COMPRESSED_COLOUR)
#endif
	{
		int maxY;
		//Either display as much of the image as possible or as much as will fit.
		if (bmpSy > sy)
			maxY = sy;
		else
			maxY = bmpSy;
		dspl_Clear(posX,posY,(U16)(posX+sx-1),(U16)(posY+sy-1));
		dspl_BitBlt2((short) posX,(short)posY,(USHORT)bmpSx,(USHORT)maxY,
					(void *)srcBitmap,0, bmpFormat);
		return;
	}

	bgdColour = DSPL_GET_BITCOLOUR(bgd_col);
	bgdColourArray[0] = (bgdColour & 0x000000FF);
	bgdColourArray[1] = (bgdColour & 0x0000FF00) >>8;
	bgdColourArray[2] = (bgdColour & 0x00FF0000) >>16;
	bgdColourArray[3] = (bgdColour & 0xFF000000) >>24;
	for (i=BYTEPERPIXEL;i<LCD_COL*BYTEPERPIXEL;i++)
		bgdColourArray[i] = bgdColourArray[i-BYTEPERPIXEL];
	bgd_sx_left = (startX- posX);

	if (srcBitmap)
	{
		image_x = posX - startX;
		image_sx = bmpSx;
		if (image_x < 0)
		{
			image_x = 0;
		}
		else
			image_sx = image_sx - image_x;
	}
	else
	{
		image_sx = 0;
		image_x  = 0;
	}
	if (bgd_sx_left <0)
		bgd_sx_left=0;
	if ((image_sx + bgd_sx_left) > sx)
	{
		image_sx = sx - bgd_sx_left;
		bgd_sx_right = 0;
	}
	else
	{
		bgd_sx_right = sx-(image_sx+bgd_sx_left);
	}
	if (bgd_sx_right <0)
		bgd_sx_right=0;
	bgd_sx_right = bgd_sx_right*BYTEPERPIXEL;
	bgd_sx_left  = bgd_sx_left*BYTEPERPIXEL;
	if (image_sx > 0)
		image_sx = image_sx*BYTEPERPIXEL;
	else
		image_sx = 0;
	if (image_x > 0)
		image_x = image_x*BYTEPERPIXEL;
	else
		image_x = 0;
	if (posY+sy > LCD_ROW)
		sy = LCD_ROW - posY;
	for (y=posY; y<posY+sy; y++)
	{
		int yOfs = DSPL_GET_BYTE(posX,y);
		if ((yOfs >=0) && (yOfs<DISPLAY_SIZE*4))
		{
		if (((srcBitmap) && (y>startY) && (y <bmpSy+startY) && (image_sx >0)))
		{
				image = &srcBitmap[(y-startY)*bmpSx*BYTEPERPIXEL];
			if (bgd_sx_left >0)
				memcpy(&screen[yOfs],&bgdColourArray[0],bgd_sx_left);
			memcpy(&screen[yOfs+bgd_sx_left],&image[image_x],image_sx);
			if (bgd_sx_right >0)
				memcpy(&screen[yOfs+bgd_sx_left+image_sx],&bgdColourArray[0],bgd_sx_right);

		}
		else
		{
			if(sx*BYTEPERPIXEL>0)   /*a0393213 lint warnings removal*/
				memcpy(&screen[yOfs],&bgdColourArray[0],sx*BYTEPERPIXEL);
			}
		}
	}
}
#else
void fastCopyBitmap(int startX, 	int startY, 	// start position of bitmap
						int bmpSx,	int bmpSy,		//size of bitmap
						char*	srcBitmap,
						int posX,   	int posY,   	// start of area to be copied into
						int sx,     	int sy,     	// size of area to be copied into
						U32 bgd_col,	int bmpFormat)
{
	//Fast copy is currently only required for colour bitmaps
}
#endif



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrMobUpdate       |
+--------------------------------------------------------------------+

  PURPOSE : Shadow display to mobile simulation

*/

void scrMobUpdate (void)
{
//TISH modified for MSIM
#ifndef WIN32
#if defined (WIN32)
  USHORT x,y;
  USHORT i;
  USHORT z;

  if (mtkEnabled)
  {
    USHORT   sdu_len;

    switch (mtkEnabled)
    {
    case 1: // ASAMPLE
      z= LCD_ROW;
      sdu_len = 13 * z; // (100 * z Pixels);
      {
        PALLOC_SDU (mmi_display_req, MMI_DISPLAY_REQ, (USHORT)(sdu_len<<3));
        {
          mmi_display_req->c_x = LCD_COL;
          mmi_display_req->c_y = z;
          i=0;
          for (y=0;y<z;y++)
            for (x=0; x < 13; x++)
              mmi_display_req->sdu.buf[i++] = displayScr[y][x];
        }

        TRACE_EVENT ("Send Display Content");
#if defined NEW_FRAME
        {
          {
            T_QMSG Message;

              Message.MsgType = MSG_PRIMITIVE;
              Message.Msg.Primitive.Prim = D2P(mmi_display_req);
              Message.Msg.Primitive.PrimLen = sizeof (T_MMI_DISPLAY_REQ) + LCD_COL_B*z;
            #ifdef MEMORY_SUPERVISION
              vsi_c_send (VSI_CALLER hCommL1 ,&Message, __FILE__, __LINE__);
            #else
              vsi_c_send (VSI_CALLER hCommL1 ,&Message);
            #endif
          }
        }
#else
//        vsi_c_send (VSI_CALLER hCommL1, D2P(mmi_display_req), sizeof (T_MMI_DISPLAY_REQ) + LCD_COL_B*z);
#endif
      }
      break;

    case 2: // another target with other dimensions
      {
        char buf[40];
        sprintf (buf, "LCD NEEDED %d READY %d", lcd_output_needed, lcd_output_ready);
        TRACE_EVENT (buf);
      }
      lcd_output_needed = TRUE;
      if (lcd_output_ready)
      {
        lcd_output_needed = FALSE;
        lcd_output_ready = FALSE;

        sdu_len = 14 * 16 + 20;
#if (LCD_ROW >= 64)
        {
          PALLOC_SDU (mmi_display_req, MMI_DISPLAY_REQ, sdu_len<<3);

          mmi_display_req->c_x = LCD_COL;
          mmi_display_req->c_y = 16;
          mmi_display_req->attrib.content = 3;
          i=0;
          for (y=48;y<64;y++)
            for (x=0; x < 13; x++)
              mmi_display_req->sdu.buf[i++] = displayScr[y][x];
#if defined NEW_FRAME
          {
            T_QMSG Message;

              Message.MsgType = MSG_PRIMITIVE;
              Message.Msg.Primitive.Prim = D2P(mmi_display_req);
              Message.Msg.Primitive.PrimLen = sizeof (T_MMI_DISPLAY_REQ) + 14*16+20;
            #ifdef MEMORY_SUPERVISION
              vsi_c_send (VSI_CALLER hCommL1 ,&Message, __FILE__, __LINE__);
            #else
              vsi_c_send (VSI_CALLER hCommL1 ,&Message);
            #endif
          }
#else
          vsi_c_send (VSI_CALLER hCommL1, D2P(mmi_display_req), sizeof (T_MMI_DISPLAY_REQ) + 14*16+20);
#endif
        }
#endif
#if (LCD_ROW >= 48)
        {
          PALLOC_SDU (mmi_display_req, MMI_DISPLAY_REQ, (USHORT)(sdu_len<<3));

          mmi_display_req->c_x = LCD_COL;
          mmi_display_req->c_y = 16;
          mmi_display_req->attrib.content = 2;
          i=0;
          for (y=32;y<48;y++)
            for (x=0; x < 13; x++)
              mmi_display_req->sdu.buf[i++] = displayScr[y][x];
#if defined NEW_FRAME
          {
            T_QMSG Message;

              Message.MsgType = MSG_PRIMITIVE;
              Message.Msg.Primitive.Prim = D2P(mmi_display_req);
              Message.Msg.Primitive.PrimLen = sizeof (T_MMI_DISPLAY_REQ) + 14*16+20;
            #ifdef MEMORY_SUPERVISION
              vsi_c_send (VSI_CALLER hCommL1 ,&Message, __FILE__, __LINE__);
            #else
              vsi_c_send (VSI_CALLER hCommL1 ,&Message);
            #endif
          }
#else
          vsi_c_send (VSI_CALLER hCommL1, D2P(mmi_display_req), sizeof (T_MMI_DISPLAY_REQ) + 14*16+20);
#endif
        }
#endif
        {
          PALLOC_SDU (mmi_display_req, MMI_DISPLAY_REQ, (USHORT)(sdu_len<<3));

          mmi_display_req->c_x = LCD_COL;
          mmi_display_req->c_y = 16;
          mmi_display_req->attrib.content = 1;
          i=0;
          for (y=16;y<32;y++)
            for (x=0; x < 13; x++)
              mmi_display_req->sdu.buf[i++] = displayScr[y][x];
#if defined NEW_FRAME
          {
            T_QMSG Message;

              Message.MsgType = MSG_PRIMITIVE;
              Message.Msg.Primitive.Prim = D2P(mmi_display_req);
              Message.Msg.Primitive.PrimLen = sizeof (T_MMI_DISPLAY_REQ) + 14*16+20;
            #ifdef MEMORY_SUPERVISION
              vsi_c_send (VSI_CALLER hCommL1 ,&Message, __FILE__, __LINE__);
            #else
              vsi_c_send (VSI_CALLER hCommL1 ,&Message);
            #endif
          }
#else
          vsi_c_send (VSI_CALLER hCommL1, D2P(mmi_display_req), sizeof (T_MMI_DISPLAY_REQ) + 14*16+20);
#endif
        }
        {
          PALLOC_SDU (mmi_display_req, MMI_DISPLAY_REQ, (USHORT)(sdu_len<<3));

          mmi_display_req->c_x = LCD_COL;
          mmi_display_req->c_y = 16;
          mmi_display_req->attrib.content = 0;
          i=0;
          for (y=0;y<16;y++)
            for (x=0; x < 13; x++)
              mmi_display_req->sdu.buf[i++] = displayScr[y][x];
#if defined NEW_FRAME
          {
            T_QMSG Message;

              Message.MsgType = MSG_PRIMITIVE;
              Message.Msg.Primitive.Prim = D2P(mmi_display_req);
              Message.Msg.Primitive.PrimLen = sizeof (T_MMI_DISPLAY_REQ) + 14*16+20;
            #ifdef MEMORY_SUPERVISION
              vsi_c_send (VSI_CALLER hCommL1 ,&Message, __FILE__, __LINE__);
            #else
              vsi_c_send (VSI_CALLER hCommL1 ,&Message);
            #endif
          }
#else
          vsi_c_send (VSI_CALLER hCommL1, D2P(mmi_display_req), sizeof (T_MMI_DISPLAY_REQ) + 14*16+20);
#endif
        }
        TRACE_EVENT ("Send Display Content");
      }
      break;
    }
  }
#endif
//TISH modified for MSIM
#endif//WIN32

#ifdef BSAMPLE
  if (mobEnabled)
  {
/*  ## scratch is not required and function mfwExtOut is 
      ## not doing anything
     sprintf(scratch,"%s%s %d",scrHeader,"disp",LCD_COL_B);
    xIndex = 0;
    mfwExtOut(scratch);
    */
  }
#endif
}


int dspl_GetContrastColour( int ipCol)
{
	switch (ipCol & 0x00808080)
	{
		case 0x000000:		return (0x00FFFFFF);
		case 0x000080:		return (0x00FFFF00);
		case 0x008000:		return (0x00FF00FF);
		case 0x008080:		return (0x00FF0000);
		case 0x800000:		return (0x0000FFFF);
		case 0x800080:		return (0x0000FF00);
		case 0x808000:		return (0x000000FF);
		case 0x808080:		return (0x00000000);
		default:			return (0x00808080);
	}
	
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrLCDUpdate       |
+--------------------------------------------------------------------+

  PURPOSE : Shadow display to LCD

*/

//GW Procedure/data supplied by new TI Driver V5.3.1
#ifdef R2D_ENABLED

static T_R2D_SHAPE_PTR	scrDisplayRect = NULL; //rectangle defined as shape of the display
static T_R2D_GC_PTR		scrPicPtr = NULL; //Pointer to gc with picture in it (b+w and col)
static T_RVF_MB_ID			mb_id;	//ID of memory bank
void r2d_refresh(void);
int riv_err;
extern T_R2D_GC_PTR r2d_g_lcd_gc;
#endif // R2D_ENABLED
static int picInit = 0;	//Have we set up the mb id etc yet ?

//Initialises data required for bolck copy of a 'picture'
void scrRivPictureClear(void)
{
#ifdef CSAMPLE
    memset(display_bw,0,sizeof(display_bw));
#ifdef USE_PLOT
    memset(displayCopy_bw,0,sizeof(displayCopy_bw));
	pictureCopy_bw = &displayCopy_bw[3]; //start of input data
#endif

	display_bw[0] = LCD_COL;
	display_bw[1] = LCD_ROW;
	display_bw[2] = DISPLAY_SIZE;
	picture_bw = &display_bw[3]; //start of input data
#endif

#ifdef DSAMPLE_COLOUR
#ifdef USEBUFFER
	display_colour[0] = LCD_COL;
	display_colour[1] = LCD_ROW;
	display_colour[2] = DISPLAY_SIZE;
	picture_col = &display_colour[3]; //start of input data
#else
//Sept 15, 2005 REF: LOCOSTO-ENH-34257 - xpradipg
//the picture_col is assigned with the address of the new frame buffer
	#if defined(FF_MMI_NEW_LCD_API_MIG) || defined(FF_SSL_ADAPTATION) //xrashmic 22 Feb, 2006 SSL_Adaptation 
		picture_col = lcd_frame_buffer;
	#else
	picture_col = r2d_get_pixmap(r2d_g_lcd_gc);
#endif
#endif
	{
		U32 bitCol;
		int j,y;
		//GW When clearing screen - do not allow transparent background colours!
		int bgdCol = dspl_GetBgdColour();
		int fgdCol = dspl_GetFgdColour();
		if (((bgdCol & 0x00F0F0F0) == (fgdCol  & 0x00F0F0F0)) ||
		    ((bgdCol & 0xFF000000) == TRANSPARENT_32BIT_COLOUR))
		{
			/*Foreground and background colours are the same (or near enough) */
			/* or background colour is transparent                            */
			/*Select a colour that will contrast with the current foreground colour */
			bgdCol = dspl_GetContrastColour(fgdCol);
		}
		bitCol = (DSPL_GET_BITCOLOUR(bgdCol)) & 0x00FFFFFF;

#ifdef DSAMPLE_COLOUR32
		bgdColourArray[0] = (bitCol & 0x000000FF);
		bgdColourArray[1] = (bitCol & 0x0000FF00) >>8;
		bgdColourArray[2] = (bitCol & 0x00FF0000) >>16;
		bgdColourArray[3] = (bitCol & 0xFF000000) >>24;
		for (j=4;j<LCD_COL*4;j++)
			bgdColourArray[j] = bgdColourArray[j-4];
		for (y=0; y<LCD_ROW; y++)
		{
			int yOfs = (y*LCD_ROW_NUM_LWORDS);
			memcpy(&picture_col[yOfs] ,bgdColourArray,LCD_COL*4);
		}
#endif
#ifdef DSAMPLE_COLOUR16
		bgdColourArray[0] = (bitCol & 0x000000FF);
		bgdColourArray[1] = (bitCol & 0x0000FF00) >>8;
		for (j=2;j<LCD_COL*2;j++)
			bgdColourArray[j] = bgdColourArray[j-2];
		for (y=0; y<LCD_ROW; y++)
		{
			int yOfs = (y*LCD_ROW_NUM_LWORDS);
			memcpy(&picture_col[yOfs] ,bgdColourArray,LCD_COL*2);
		}
#endif
	}

#endif

#ifdef DSAMPLE_LITE
	picture_bw = r2d_get_pixmap(r2d_g_lcd_gc);
	memset(picture_bw,0,DISPLAY_SIZE*4);
#endif

}
void scrRivPictureInit( void )
{
#if (defined(DSAMPLE_COLOUR) && defined(USEBUFFER))
	static int err = -1; /*a0393213 18th aug 2006-warnings removal-variables (err, mb_name and mb_param) put under compilation flag*/
#endif
#if (defined(CSAMPLE) || (defined(DSAMPLE_COLOUR) && defined(USEBUFFER)))
	T_RVF_MB_NAME 		mb_name;
	T_RVF_MB_PARAM 		mb_param;
#endif
	//Ensure data is only created once.
//Sept 15, 2005 REF: LOCOSTO-ENH-34257 - xpradipg. the scrRivePictureClear() is called even in case of ISAMPLE and CALLSTO boards	
#if defined( R2D_ENABLED) || defined (ISAMPLE) || defined (CALLISTO)
	if (picInit != 0)
		return;
	picInit = 1; //int underway.
	scrRivPictureClear();
#endif
#ifdef CSAMPLE
	strcpy(mb_name, "FRED");
	mb_param.size = 256;
	mb_param.watermark = 128;
	riv_err = rvf_create_mb( mb_name, mb_param, &mb_id);
	if (riv_err == 0)
	{
		scrPicPtr = r2d_new_picture_context(mb_id, display_bw,R2D_LCD_KIND);
		scrDisplayRect = r2d_new_rectangle(mb_id,0,0,LCD_COL-1,LCD_ROW-1);
	}
	else
	{
		scrPicPtr = NULL;
		scrDisplayRect  = NULL;
	}
#endif

#ifdef DSAMPLE_COLOUR
#ifdef USEBUFFER
	strcpy(mb_name, "FRED");
	mb_param.size = 256;
	mb_param.watermark = 128;
	err = rvf_create_mb( mb_name, mb_param, &mb_id);
	if (err == 0)
	{
		scrPicPtr = r2d_new_picture_context(mb_id, display_colourPtr,R2D_FULL_KIND);// R2D_LCD_KIND);
		scrDisplayRect = r2d_new_rectangle(mb_id,0,0,LCD_COL+1,LCD_ROW+1);
	}
	else
#endif
	{
#if !defined(FF_MMI_NEW_LCD_API_MIG) && !defined(FF_SSL_ADAPTATION) //xrashmic 22 Feb, 2006 SSL_Adaptation 
		scrPicPtr = NULL;
		scrDisplayRect  = NULL;
#endif		
	}
#endif

}

//GW Change the display to  
#ifdef DSAMPLE_COLOUR
#ifdef DSAMPLE_COLOUR32
#define TOP_BITS_SET 0xC618
#endif
#ifdef DSAMPLE_COLOUR16
#define TOP_BITS_SET 0xC618C618
#endif
#endif
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6147)         MODULE  : SMI_WM                   |
| STATE   : code                  ROUTINE : scrUnfocusDisplay      |
+--------------------------------------------------------------------+

  PURPOSE : Causes the entire display to appear much darker, allowing the pop-up 
  			dialog to be more clearly the focus especially the softkeys.

GW #2294 23/07/03 - Created
*/
void scrUnfocusDisplay(void)
{
#ifndef _SIMULATION_  // Turn off this BMI Schnickschnack ...
	int x,y;
	UINT32 *picPtr;
#ifndef DSAMPLE_COLOUR
	UINT32 mask;
#endif
#ifdef DSAMPLE_COLOUR
	picPtr = picture_col;
#else
	picPtr = picture_bw;
#endif
	if (picPtr != NULL)
	{
		for (y=0;y<LCD_ROW;y++)
		{
#ifndef DSAMPLE_COLOUR
			if (y & 0x01)
				mask = 0xAAAAAAAA;
			else
				mask = 0x55555555;
#endif
			for (x=0;x<LCD_ROW_NUM_LWORDS;x++)
			{
#ifdef DSAMPLE_COLOUR
				//We Want to reduce colour by 50% (shift left) and set top bits to bits to 0
				//Since picture colours are inverted, set top bits.
				//Also, colouris very bright - reduce by 75%
				if ((*picPtr & TOP_BITS_SET) != TOP_BITS_SET)
					*picPtr = ((*picPtr) >> 2) | TOP_BITS_SET;
#else
				//B+W display - blank every second pixel
				*picPtr = (*picPtr) & mask;
#endif
				picPtr++;
			}
		}
	}
#endif /* _SIMULATION_ */
}


//Set colour (only when different from last time)
void scrSetR2DFgdCol(UINT32 col)
{
#ifdef R2D_ENABLED /*a0393213 18th aug 2006 warnings removal-entire fn included conditionally*/
	U16 opCol_a,opCol_r,opCol_g,opCol_b;
	static UINT32 oldCol = 0xFFFFFFFF;
	if (col != oldCol)
	{
#ifdef DSAMPLE_COLOUR
		opCol_a = 0xFF; // ((col & 0x0000FF00) >> 16);
		opCol_r = ((col & 0x0000F800) >>  8);
		opCol_g = ((col & 0x000007E0) >>  3);
		opCol_b = ((col & 0x0000001F) <<  3);
#else
		opCol_a = 0xFF; // ((col & 0x0000FF00) >> 16);
		opCol_r = ((col & 0x000000E0)       );
		opCol_g = ((col & 0x0000001C) <<  3);
		opCol_b = ((col & 0x00000003) <<  6);
		if (opCol_r == 0)	opCol_r = 0x80;
		if (opCol_g == 0)	opCol_g = 0x40;
#endif
		r2d_set_foreground_color_with_argb(r2d_g_lcd_gc, opCol_a, opCol_r, opCol_g, opCol_b );
		oldCol = col;
	}
#endif
}
//Set colour (only when different from last time)
void scrSetR2DBgdCol(UINT32 col)
{
#ifdef R2D_ENABLED
	U16 opCol_a,opCol_r,opCol_g,opCol_b;
	static UINT32 oldCol = 0xFFFFFFFF;
	if (col != oldCol)
	{
#ifdef DSAMPLE_COLOUR
		opCol_a = 0xFF; // ((col & 0x0000FF00) >> 16);
		opCol_r = ((col & 0x0000F800) >>  8);
		opCol_g = ((col & 0x000007E0) >>  3);
		opCol_b = ((col & 0x0000001F) <<  3);
#else
		opCol_a = 0xFF; // ((col & 0x0000FF00) >> 16);
		opCol_r = ((col & 0x000000E0)       );
		opCol_g = ((col & 0x0000001C) <<  3);
		opCol_b = ((col & 0x00000003) <<  6);
		if (opCol_r == 0)	opCol_r = 0x80;
		if (opCol_g == 0)	opCol_g = 0x40;
#endif
		r2d_set_background_color_with_argb(r2d_g_lcd_gc, opCol_a, opCol_r, opCol_g, opCol_b );
		oldCol = col;
	}
#endif
}
//GW Procedure/data supplied by new TI Driver V5.3.1
//length of array (32bit words) to hold a copy of the screen (maybe)
#ifdef R2D_ENABLED
extern T_R2D_GC_PTR r2d_g_lcd_gc;
#endif
/*  */
void scrPlotR2DPoint(U32 col, INT16 x, INT16 y)
{
#ifdef R2D_ENABLED
	static U32 fCol=0xFFFFFFFF;
	static U32 bCol=0xFFFFFFFF;
	static U32 flip=0;
	if (fCol==col)
	{
		scrSetR2DFgdCol(col );
		r2d_draw_point(r2d_g_lcd_gc,x,y);
	}
	else if (bCol == col)
	{
		scrSetR2DBgdCol(col );
		r2d_erase_point(r2d_g_lcd_gc,x,y);
	}
	else if (flip)
	{
		flip = 0;
		scrSetR2DBgdCol(col );

		r2d_erase_point(r2d_g_lcd_gc,x,y);
		bCol = col;
	}
	else
	{
		flip = 1;
		scrSetR2DFgdCol(col );
		r2d_draw_point(r2d_g_lcd_gc,x,y);
		fCol = col;
	}
#endif
}

#ifdef BSAMPLE
void scrLCDUpdate (void)
{
#ifdef USE_PLOT
	int nPlots=0;

	int row,rowstart;
	int kstart,kend,k;
	UBYTE *bcopy;
	int i,j;
	UBYTE m;
	UBYTE  *bm;
#else
	int i;
#endif

#if defined (WIN32_SIM)
    return;
#endif
    if (!scrEnabled)
        return;

#ifdef USE_PLOT
	bm   = &displayScr[0][0];
    bcopy = &displayCopy[0][0];
    m = 0x80;
    for (j = 0; j < LCD_ROW; j++)
    {
        for (k = 0; k < (LCD_COL+7)/8; k++)
        {
          //Only plot data if it has changed since last time
          if (*bm != *bcopy)
          {
            m = 0x80;
            kstart = k*8;
            kend = k*8+8;
            if (kend>LCD_COL)
              kend = LCD_COL;
            for (i = kstart; i < kend; i++)
            {
#ifdef R2D_ENABLED
                if ((*bm) & m)

					r2d_draw_point(r2d_g_lcd_gc,i,j);
                else
					r2d_erase_point(r2d_g_lcd_gc,j,i);
#endif
                m >>= 1;
            }
            *bcopy = *bm;//Update value
          }
          bm++;
          bcopy++;
        }
    }

#else
    //* work around limitations of TI driver !                       *
    for (i = 0; i < LCD_ROW; i++)
    {
        displayScr[i][LCD_COL_B-1] &= 0xF0;
        displayScr[i][LCD_COL_B-1] |= (UBYTE) ((displayScr[i][6] >> 2) & 0x0F);
    }

    LCD_PixBlt(0,0,LCD_COL_B*8,8,(char*)&displayScr[0][0]);
    LCD_PixBlt(0,8,LCD_COL_B*8,8,(char*)(&(displayScr[8][0])));
    LCD_PixBlt(0,16,LCD_COL_B*8,8,(char*)(&(displayScr[16][0])));
    LCD_PixBlt(0,24,LCD_COL_B*8,8,(char*)(&(displayScr[24][0])));
#if (!CUST)//JVJE #if (CUST==1)
	LCD_PixBlt(0,32,LCD_COL_B*8,8,(char*)(&(displayScr[32][0])));
	LCD_PixBlt(0,40,LCD_COL_B*8,8,(char*)(&(displayScr[40][0])));
#endif
#endif

}
#endif




#ifdef CSAMPLE
void scrLCDUpdate (void)
{
int kstart,kend,k;
int row,rowstart;
int   i,j;
INT16 x1,y1;
UINT32  m;
UINT32  *bm;
UINT32  *bcopy;
static UINT32 col=0;
int count;
int nPlots=0;

#if defined (WIN32_SIM)
    return;
#endif
    if (!scrEnabled)
        return;

#ifdef USE_PLOT

	bm   = picture_bw;
    bcopy = pictureCopy_bw;
	for (j = 0; j < DISPLAY_SIZE; j++)
	{
		if (*bm != *bcopy)
	    {
        	//Only plot data if it has changed since last time
		    m = 0x80000000;

		    x1 = j / LCD_COL_NUM_LWORDS;
		    y1 = j % LCD_COL_NUM_LWORDS;
		    y1 = y1 << 5;
		    y1 = (y1 + 16);
		    x1 = (scrX-1)-x1;
		    kstart = y1;
		    kend = y1+32;
		    for (i = kstart; i < kend; i++)
        	{
				nPlots++;
#ifdef R2D_ENABLED
				if ((*bm) & m)
					r2d_draw_point(r2d_g_lcd_gc,x1,(i%64));
            	else

					r2d_erase_point(r2d_g_lcd_gc,x1,(i%64));
#endif
				m >>= 1;
			}
 			*bcopy = *bm;//Update value
       	}
      	bm++;
      	bcopy++;
      	if (nPlots > MAX_NUMBER_PLOTS)
      			return;
    }
#else
	if (scrPicPtr != NULL)
	{
#ifdef R2D_ENABLED
		r2d_fast_blit(scrPicPtr,r2d_g_lcd_gc);
#endif
		vsi_t_sleep(VSI_CALLER (20));
	}
#endif //old/new
}
#endif //CSAMPLE

//June 03, 2005 REF: GSM-ENH-31636 xpradipg
//CRR 28825:  xpradipg - 11 Feb 2005
#if (defined(DSAMPLE) || defined(ESAMPLE) || defined(ISAMPLE) || defined(CALLISTO))// changes for flag cleanup and addition for ISAMPLE
void scrLCDUpdate (void)
{
//	Sept 15, 2005 REF: LOCOSTO-ENH-34257 - xpradipg
//the co-ordinates are set to the entire LCD as we will be refreshing whole LCD
#ifdef FF_MMI_NEW_LCD_API_MIG
lcd_fb_coordinates p_lcd_coord = { 0,0,LCD_COL,LCD_ROW};
#endif
#if defined (WIN32_SIM)
    return;
#endif
    if (!scrEnabled)
        return;
//Sept 15, 2005 REF: LOCOSTO-ENH-34257 - xpradipg
//call the new lcd AP in place of the old for refresh
#ifdef FF_MMI_NEW_LCD_API_MIG
	lcd_display(DISPLAY_MAIN_LCD,(U16*)picture_col,&p_lcd_coord);
#else
 //xrashmic 22 Feb, 2006 SSL_Adaptation
#ifdef FF_SSL_ADAPTATION //xrashmic 22 Feb, 2006 SSL_Adaptation 
	 (void)(OMX_EmptyThisBuffer(*pHandle,&buffer_header)); /*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
#endif// FF_SSL_ADAPTATION 

#ifdef R2D_ENABLED
	r2d_flush();
#endif
#endif
}
#endif//D-sample

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrMobCommand      |
+--------------------------------------------------------------------+

  PURPOSE : Command string from mobile simulation

*/


void scrMobCommand (char *s)
{
#ifdef BSAMPLE
    char *p;
    unsigned char *q;
    int i;
    char scratch [LCD_ROW*LCD_COL_B*2+100];     /* command buffer           */	

    if (strstr(s,"ena") == s)
    {
        mobEnabled = 1;
        return;
    }
    if (strstr(s,"dis") == s)
    {
        mobEnabled = 0;
        return;
    }
    if (strstr(s,"ok") == s)
    {
        if (xIndex < 0)
            return;
        strcpy(scratch,scrHeader);
        if (xIndex >= sizeof(displayScr))
        {
            strcat(scratch,"done");
            mfwExtOut(scratch);
            return;
        }
        strcat(scratch,"data ");
        p = scratch + strlen(scratch);
        sprintf(p,"%04d %04d ",xIndex,LCD_COL_B*LCD_ROW/DISPL_DIV);
        p = scratch + strlen(scratch);
        q = (UBYTE *) displayScr;
        for (i = 0; i < LCD_COL_B * LCD_ROW / DISPL_DIV; i++)
        {
            sprintf(p,"%02x",q[xIndex++]);
            p += 2;
        }
        *p = '\0';
        mfwExtOut(scratch);
        return;
    }
    #endif
}

#ifdef FF_SSL_ADAPTATION
OMX_ERRORTYPE EmptyBufferDone_cb(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData,OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	return OMX_ErrorNone;
}

 OMX_ERRORTYPE EventHandler_cb(OMX_IN OMX_HANDLETYPE hComponent,  OMX_IN OMX_PTR pAppData, OMX_IN OMX_EVENTTYPE eEvent, OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2, OMX_IN OMX_PTR pEventData)
{
	return OMX_ErrorNone;
}
#endif //#ifdef FF_SSL_ADAPTATION

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrInit            |
+--------------------------------------------------------------------+

  PURPOSE : initialize screen driver

*/

void scrInit (void)
{
 //xrashmic 22 Feb, 2006 SSL_Adaptation
#ifdef FF_SSL_ADAPTATION
	OMX_ERRORTYPE omx_err;
	OMX_CALLBACKTYPE omx_callback;	
	OMX_SSL_PLANE_CONFIGTYPE planeConfig1, planeConfig2;
	OMX_U32 nActiveDispID;
#endif //FF_SSL_ADAPTATION 

//	Sept 15, 2005 REF: LOCOSTO-ENH-34257 - xpradipg
//New LCD API for initialization
#ifdef FF_MMI_NEW_LCD_API_MIG
	lcd_initialization(DISPLAY_MAIN_LCD);
#else 
#ifndef FF_SSL_ADAPTATION//xrashmic 22 Feb, 2006 SSL_Adaptation 
    LCD_SetPower(); /* ES!! */
    LCD_Reset();
    LCD_Enable(1);
#endif //FF_SSL_ADAPTATION
#endif	
	dspl_InitColour ();

//TISH modified for MSIM
//Begin
//#ifdef LSCREEN
//	font_initFont(1);
//#else
	font_initFont(0);
//#endif
//End

#ifdef BSAMPLE
    memset(displayScr,0,sizeof(displayScr));
    memset(displayCopy,0,sizeof(displayCopy));
#else
	scrRivPictureInit();
#endif
 //xrashmic 22 Feb, 2006 SSL_Adaptation
#ifdef FF_SSL_ADAPTATION
	//Creating Plane1 : full screen MMI mode - (x,y) = (0,0) and (width, height) = (176, 220)
	 mixed_mode_offset = (LCD_COL * LCD_ROW_NUM_LWORDS);
	 mixed_mode_buf_size = ((LCD_ROW-LCD_COL) * LCD_ROW_NUM_LWORDS * 4);
	 mmi_mode_buf_size=(LCD_ROW * LCD_ROW_NUM_LWORDS * 4);

	omx_callback.EmptyBufferDone=EmptyBufferDone_cb;
	omx_callback.EventHandler=EventHandler_cb;
	omx_callback.FillBufferDone=NULL;
	omx_err=OMX_GetHandle(&pHandlePlane1,OMXSSL_COMP_NAME,lcd_frame_buffer,&omx_callback);
/*	if(omx_err != OMX_ErrorNone)
	{
		//Error in initialization of the SSL components
	}*/
	/*a0393213 18th aug 2006 warnings removal-OMX_IndexConfigSSLPlaneActiveDispID typecasted*/
	omx_err=OMX_GetConfig(pHandlePlane1, (OMX_INDEXTYPE)OMX_IndexConfigSSLPlaneActiveDispID, &nActiveDispID);
	planeConfig1.nXOffset=0;
	planeConfig1.nYOffset=0;
	planeConfig1.nXLen=LCD_COL;
	planeConfig1.nYLen=LCD_ROW;
	planeConfig1.tAlpha=OMX_SSL_ALPHA0;/*a0393213 18th aug 2006 warnings removal-0 changed to OMX_SSL_ALPHA0*/
	planeConfig1.bAlwaysOnTop=OMX_FALSE;
	planeConfig1.bDSAPlane=OMX_FALSE;/*a0393213 18th aug 2006 warnings removal-0 changed to OMX_FALSE*/
	planeConfig1.tPlaneImgFormat=OMX_TICOLOR_16bitRGB565;
	planeConfig1.nActiveDispID=nActiveDispID;
	//Jun 09, 2006 ER: OMAPS00057437 - xdeepadh
	planeConfig1.tPlaneBlendType = OMX_SSL_PLANEBLEND_OVERLAP ;
	pHandle=&pHandlePlane1;
	buffer_header.pBuffer=(OMX_U8*)lcd_frame_buffer;
	buffer_header.nAllocLen=LCD_ROW*LCD_ROW_NUM_LWORDS*4;//Total number of bytes allocated in this buffer.
      buffer_header.nSize = LCD_ROW*LCD_ROW_NUM_LWORDS*4;//Total number of bytes allocated in this buffer.
      /*a0393213 18th aug 2006 warnings removal-OMX_IndexConfigSSLPlaneConfig typecasted*/
	omx_err= OMX_SetConfig(pHandlePlane1,(OMX_INDEXTYPE)OMX_IndexConfigSSLPlaneConfig,&planeConfig1);
	omx_err=OMX_SendCommand(pHandlePlane1, OMX_CommandStateSet, OMX_StateIdle,NULL);
	omx_err=OMX_SendCommand(pHandlePlane1, OMX_CommandStateSet, OMX_StateExecuting,NULL);	
	//Creating Plane2 : Mixed mode - (x,y) = (0,176) and (width, height) = (176, 44) - only for softkey updation
	/*a0393213 18th aug 2006 warnings removal-LCD_COL is changed to LCD_ROW_NUM_LWORDS*/
	omx_err=OMX_GetHandle(&pHandlePlane2,OMXSSL_COMP_NAME,lcd_frame_buffer+(LCD_ROW_NUM_LWORDS*LCD_ROW_NUM_LWORDS),&omx_callback);
	if(omx_err != OMX_ErrorNone)
	{
		//Error in initialization of the SSL components
	}
	/*a0393213 18th aug 2006 warnings removal-OMX_IndexConfigSSLPlaneActiveDispID typecasted*/
	omx_err=OMX_GetConfig(pHandlePlane2, (OMX_INDEXTYPE)OMX_IndexConfigSSLPlaneActiveDispID, &nActiveDispID);
	planeConfig2.nXOffset=0;
	planeConfig2.nYOffset=LCD_COL;
	planeConfig2.nXLen=LCD_COL;
	planeConfig2.nYLen=LCD_ROW-LCD_COL;
	planeConfig2.tAlpha=OMX_SSL_ALPHA0;/*a0393213 18th aug 2006 warnings removal-0 changed to OMX_SSL_ALPHA0*/
	planeConfig2.bAlwaysOnTop=OMX_TRUE;/*a0393213 18th aug 2006 warnings removal-1 changed to OMX_TRUE*/
	planeConfig2.bDSAPlane=OMX_FALSE;/*a0393213 18th aug 2006 warnings removal-0 changed to OMX_FALSE*/
	planeConfig2.tPlaneImgFormat=	OMX_TICOLOR_16bitRGB565;
	planeConfig2.nActiveDispID=nActiveDispID;
	//Jun 09, 2006 ER: OMAPS00057437 - xdeepadh
	planeConfig2.tPlaneBlendType = OMX_SSL_PLANEBLEND_OVERLAP ;
	/*a0393213 18th aug 2006 warnings removal-OMX_IndexConfigSSLPlaneConfig typecasted*/
	omx_err= OMX_SetConfig(pHandlePlane2,(OMX_INDEXTYPE)OMX_IndexConfigSSLPlaneConfig,&planeConfig2);	
	omx_err=OMX_SendCommand(pHandlePlane2, OMX_CommandStateSet, OMX_StateIdle,NULL);
#endif //FF_SSL_ADAPTATION 
//Sept 15, 2005 REF: LOCOSTO-ENH-34257 - xpradipg
//clear the buffer with background color, since r2d_erase does the same
#ifdef FF_MMI_NEW_LCD_API_MIG
	scrRivPictureClear();
#else
#ifdef R2D_ENABLED
	r2d_erase( r2d_g_lcd_gc );
#endif
#endif
#ifdef BSAMPLE
    mobEnabled = 0;
#endif
    scrEnabled = 1;
    internal = 0;
    scrX = LCD_COL;
    scrY = LCD_ROW;
#ifdef BSAMPLE	
    xIndex = -1;
#endif
	displayData.DisplayType = DSPL_TYPE_GRAPHIC;
	 //GW Set display window to full screen
	dspl_ResetWindow();
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrExit            |
+--------------------------------------------------------------------+

  PURPOSE : finalize screen driver

*/

void scrExit (void)
{
    LCD_Enable(0);
#ifdef BSAMPLE	
    mobEnabled = 0;
#endif
    scrEnabled = 0;
#ifdef BSAMPLE
    xIndex = -1;
#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrUpdate          |
+--------------------------------------------------------------------+

  PURPOSE : set screen update mode

GW 18/09/01 clear text invert flag whenever display is enabled/disabled.
*/
//GW mmi_trace is a call to the MMI to provide timings for MMI updates -
//REMOVE FROM FINAL BUILD (if I forget!)
int scrUpdate (int onOff)
{
    int ena = scrEnabled;
	

    scrEnabled = onOff;
    if (scrEnabled)
    {
#ifdef MMI_TRACE
mmi_trace("scrupdate() enable start");
#endif
	scrLCDUpdate();
        scrMobUpdate();

#ifdef MMI_TRACE
mmi_trace("scrupdate() enable end");
#endif
    }
    else
	{
#ifdef MMI_TRACE
mmi_trace("scrupdate() disabled");
#endif
	}
    return ena;
}

 //xrashmic 22 Feb, 2006 SSL_Adaptation 
#ifdef FF_SSL_ADAPTATION
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : SSL Adaptation            |
| STATE   : code                        ROUTINE : dspl_set_to_mixed_mode            |
+--------------------------------------------------------------------+

  PURPOSE : Changes mode from MMI to mixed mode

*/
void dspl_set_to_mixed_mode(void)
{
       /*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/	
	(void)(OMX_SendCommand(pHandlePlane1, OMX_CommandStateSet, OMX_StateIdle,NULL));
	buffer_header.pBuffer=(OMX_U8*)(lcd_frame_buffer+mixed_mode_offset);
	buffer_header.nAllocLen=mixed_mode_buf_size;//Total number of bytes allocated in this buffer.
	buffer_header.nSize=mixed_mode_buf_size;//Total number of bytes allocated in this buffer.
	pHandle=&pHandlePlane2;
	/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
	(void)(OMX_SendCommand(pHandlePlane2, OMX_CommandStateSet, OMX_StateExecuting,NULL));		
}


void dspl_set_mmi_to_idle_mode(void)
{
       /*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/	
	(void)(OMX_SendCommand(pHandlePlane1, OMX_CommandStateSet, OMX_StateIdle,NULL));
}

void dspl_set_mmi_to_execute_mode(void)
{
	/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/	
	(void)(OMX_SendCommand(pHandlePlane1, OMX_CommandStateSet, OMX_StateExecuting,NULL));
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : SSL Adaptation            |
| STATE   : code                        ROUTINE : dspl_set_to_mmi_mode            |
+--------------------------------------------------------------------+

  PURPOSE : Changes mode from mixed mode to MMI mode

*/
void dspl_set_to_mmi_mode(void)
{
	/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/	
	(void)(OMX_SendCommand(pHandlePlane2, OMX_CommandStateSet, OMX_StateIdle,NULL));
	buffer_header.pBuffer=(OMX_U8*)lcd_frame_buffer;
	buffer_header.nAllocLen=mmi_mode_buf_size;//Total number of bytes allocated in this buffer.
	buffer_header.nSize = mmi_mode_buf_size;//Total number of bytes allocated in this buffer.
	pHandle=&pHandlePlane1;
	/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/	
	(void)(OMX_SendCommand(pHandlePlane1, OMX_CommandStateSet, OMX_StateExecuting,NULL));	
}
#endif // FF_SSL_ADAPTATION 


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrSize            |
+--------------------------------------------------------------------+

  PURPOSE : retrieve display size

*/

void scrSize (int *x, int *y)
{
    *x = scrX;
    *y = scrY;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrOnOff           |
+--------------------------------------------------------------------+

  PURPOSE : switch display On/Off

*/

void scrOnOff (int on)
{
//kk to be checked
/*  ## scratch is not required and function mfwExtOut is 
      ## not doing anything
   sprintf(scratch,"%sscrOnOff:%d",scrHeader,on);
   mfwExtOut(scratch);
   */
}

#if (!CUST)//JVJE #if (CUST==1)
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrLight           |
+--------------------------------------------------------------------+

  PURPOSE : switch display backlight On/Off

*/

void scrLight (int on)
{
//kk to be checked
/*  ## scratch is not required and function mfwExtOut is 
      ## not doing anything
    sprintf(scratch,"%sscrLight:%d",scrHeader,on);
   mfwExtOut(scratch);
 */
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrClear           |
+--------------------------------------------------------------------+

  PURPOSE : clear display

*/

void scrClear (void)
{

#ifdef BSAMPLE
    memset(displayScr,0,sizeof(displayScr));
#else
	scrRivPictureClear();
#endif

    if (scrEnabled)
    {
//Sept 15, 2005 REF: LOCOSTO-ENH-34257 - xpradipg
//this is specific to old driver. A corresponding API exist with new apis. 
//need to check if it can be used 
#if !defined( FF_MMI_NEW_LCD_API_MIG) && !defined(FF_SSL_ADAPTATION) //xrashmic 22 Feb, 2006 SSL_Adaptation 
        LCD_Clear();                    /* TI driver function       */
#endif
        scrMobUpdate();                 /* update mobile simulation */
    }
}
#endif //CUST==1

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrClearRect       |
+--------------------------------------------------------------------+

  PURPOSE : clear display area

*/
#ifdef DSAMPLE_LITE

void scrDrawFilledRect (int px, int py, int sx, int sy, U32 col)
{
	int i,j;
	U32 opCol;
	int word;

	//Limit rectangle to the size of the screen.
	if ((px >= scrX) || (py >= scrY))
		return;

	if ((col& 0xFF000000) == TRANSPARENT_32BIT_COLOUR)
		return; //Background colour is transparent

	//Allow -ve sizes but change data round
	if (sx <0)
	{
		px = px+sx;
		sx = -sx;
	}
	if (sy <0)
	{
		py = py+sy;
		sy = -sy;
	}
	//Limit rectangle to size of window
	if (px<win_x1)
	{
		sx = sx - win_x1;
		px = win_x1;
	}
	if (py<win_y1)
	{
		sy = sy - win_y1;
		py = win_y1;
	}

	if (px+sx >=win_x2)
		sx = win_x2 - px-1;
	if (py+sy >=win_y2)
		sy = win_y2 - py-1;
	if ((sx <= 0) || (sy<=0))
		return;//cannot draw zero width rectangle.

	opCol = col;
	{
		U32 maskL, maskR;
		int skipBits,startWord,endWord;
		maskL = 0xFFFFFFFF;
		skipBits = px & 0x1F;
		maskL = maskL << skipBits;
		startWord = px >> 5;
		maskR = 0xFFFFFFFF;
		skipBits = (px+sx) & 0x1F;
		maskR = ~(maskR << skipBits);
		endWord = (px+sx) >> 5;
		if (startWord == endWord)
		{
			maskL = maskL & maskR;
			maskR = maskL;
		}

		if (col == 0)
		{	//Set bgd colour to Black (0)
		    for (j = py; j < py + sy; j++)
		    {
				word = DSPL_GET_LWORD(px,j);
				picture_bw[word] = picture_bw[word] & (~maskL);
				word++;
				for (i = startWord+1; i < endWord; i++)
				{
					picture_bw[word] = 0;
					word++;
				}
				word = DSPL_GET_LWORD(px+sx,j);
				picture_bw[word] = picture_bw[word] & (~maskR);
		    }
		}
		else
		{	//Set bgd colour to White (1)
		    for (j = py; j < py + sy; j++)
		    {
				word = DSPL_GET_LWORD(px,j);
				picture_bw[word] = picture_bw[word] | (maskL);
				word++;
				for (i = startWord+1; i < endWord; i++)
				{
					picture_bw[word] = 0xFFFFFFFF;
					word++;
				}
				word = DSPL_GET_LWORD(px+sx,j);
				picture_bw[word] = picture_bw[word] | (maskR);
		    }
		}
	}
}
#endif

#ifdef DSAMPLE_COLOUR

void scrDrawFilledRect (int px, int py, int sx, int sy, U32 col)
{
	int i;
	
	char* screen;

	//Limit rectangle to the size of the screen.
	if ((px >= scrX) || (py >= scrY))
		return;
	if ((col& 0xFF000000) == TRANSPARENT_32BIT_COLOUR)
		return; //Background colour is transparent

	//Allow -ve sizes but change data round
	if (sx <0)
	{
		px = px+sx;
		sx = -sx;
	}
	if (sy <0)
	{
		py = py+sy;
		sy = -sy;
	}
	//Limit rectangle to size of window
	if (px<win_x1)
	{
		sx = sx - win_x1;
		px = win_x1;
	}
	if (py<win_y1)
	{
		sy = sy - win_y1;
		py = win_y1;
	}
	if (px+sx > win_x2)
		sx = win_x2 - px;
	if (py+sy > win_y2)
		sy = win_y2 - py;
	if ((sx <= 0) || (sy<=0))
		return;//cannot draw zero width rectangle.
	
	//GW Fix - invert colour
#ifdef USE_PLOT
   {
       int j;
       for (j = py; j < py + sy; j++)
       {
   		for (i = px; i < px+sx; i++)
   			scrPoint(i,j,col);
       }
    }
#else
	{
		U32 bgdColour;
		int y;
		screen = (char*)picture_col;
		bgdColour = DSPL_GET_BITCOLOUR(col);
		bgdColourArray[0] = (bgdColour & 0x000000FF);
		bgdColourArray[1] = (bgdColour & 0x0000FF00) >>8;
		bgdColourArray[2] = (bgdColour & 0x00FF0000) >>16;
		bgdColourArray[3] = (bgdColour & 0xFF000000) >>24;
		for (i=BYTEPERPIXEL;i<sx*BYTEPERPIXEL;i++)
			bgdColourArray[i] = bgdColourArray[i-BYTEPERPIXEL];
		for (y=py; y<py+sy; y++)
		{
//			int yOfs = (y*LCD_COL+px);
			int yOfs = DSPL_GET_BYTE(px,y);
			memcpy(&screen[yOfs] ,bgdColourArray,sx*BYTEPERPIXEL);
		}
	}
#endif//use plot
}
#endif


#ifdef BSAMPLE
void scrDrawFilledRect (int px, int py, int sx, int sy, U32 col)
{
	scrClearRect (px, py, sx, sy);
}
#endif
#ifdef CSAMPLE
void scrDrawFilledRect (int px, int py, int sx, int sy, U32 col)
{
	scrClearRect (px, py, sx, sy);
}
#endif

void scrClearRect (int px, int py, int sx, int sy)
{

#ifdef BSAMPLE
	int i,j;
    int x1, x2;
    UBYTE m1, m2;
#endif

#ifdef CSAMPLE
	int x1;
	int i,j;
    int y1s, y2s;
    int y1f, y2f;
	U32 m1s,m2s;
	U32 m1f,m2f;

    int y1, y2;
	U32 m1,m2;
#endif

    if (px < 0 || py < 0 || px >= scrX || py >= scrY
                                       || sx < 0 || sy < 0)
        return;
    if (px + sx > scrX)
        sx = scrX - px;
    if (py + sy > scrY)
        sy = scrY - py;

	//Limit rectangle to size of window
	if (px<win_x1)
      {
		sx = sx - win_x1;
		px = win_x1;
      }
	if (py<win_y1)
	{
		sy = sy - win_y1;
		py = win_y1;
	}

	if (px+sx >win_x2)
		sx = win_x2 - px;
	if (py+sy >win_y2)
		sy = win_y2 - py;
	if ((sx <= 0) || (sy<=0))
		return;//cannot draw zero width rectangle.

#ifdef BSAMPLE
    x1 = px / 8;                        /* first x offset           */
    x2 = (px + sx) / 8;                 /* last x offset            */
    m1 = m2 = 0xff;
    m1 = (UBYTE) ~(m1 >> (px & 7));        /* left mask                */
    m2 = (UBYTE) (m2 >> ((px + sx) & 7));  /* right mask               */
    if (x1 == x2)
        m1 = m2 = m1 | m2;              /* combined masks           */
    for (j = py; j < py + sy; j++)
    {
        displayScr[j][x1] &= m1;
        for (i = x1 + 1; i < x2; i++)
            displayScr[j][i] = '\0';
        displayScr[j][x2] &= m2;
    }
#endif

#ifdef CSAMPLE
    for (i=0;i<sx;i++)
    	for (j=0;j<sy;j++)
    		scrPoint(px+i,py+j,0);
#endif

//June 03, 2005 REF: GSM-ENH-31636 xpradipg
//CRR 28825:  xpradipg - 11 Feb 2005
#if (defined(DSAMPLE) || defined(ESAMPLE) || defined(ISAMPLE) || defined(CALLISTO)) //clean up of flag and addition for ISAMPLE
	scrDrawFilledRect ( px, py, sx, sy, dspl_GetBgdColour());
#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr              |
| STATE   : code                        ROUTINE : scrText_Unicode      |
+--------------------------------------------------------------------+

  PURPOSE : draw text

*/

void scrText_Unicode (int x, int y, USHORT *txt)
{
  
  USHORT current_code,first_byte_code,second_byte_code;
  t_font_bitmap* current_bitmap,*error_bitmap;

    if ( y > scrY)
        return;

    while ((*txt) && (x <= scrX))
    {
    current_code = (USHORT)((*txt)<<8)+(USHORT)((*txt)>>8);/*The USHORT is inverted, therefore we have to apply this simple change*/
    current_bitmap = get_bitmap(current_code);
		if (!current_bitmap)	/*ASCII codes will be represented*/
    {
			first_byte_code = (USHORT)((*txt)&0xFF);
      error_bitmap = get_bitmap(first_byte_code);
      if (error_bitmap)
      {
        dspl_show_bitmap(x,y,error_bitmap,SHOWBITMAP_INVERT);
            x += error_bitmap->width;
      }
      second_byte_code = (USHORT)((*txt)>>8);
      error_bitmap = get_bitmap(second_byte_code);
      if (error_bitmap)
      {
        dspl_show_bitmap(x,y,error_bitmap,SHOWBITMAP_INVERT);
            x += error_bitmap->width;
      }
    }
    else
    {
      dspl_show_bitmap(x,y,current_bitmap,SHOWBITMAP_NORMAL);
          x += current_bitmap->width;
    }
      txt++;
  }

}


#ifndef DSAMPLE_COLOUR
//Should work for DSAMPLE_LITE
void dspl_show_bitmap(int x,int y,t_font_bitmap* current_bitmap,U32 attr )
{
 	USHORT i, j,a,b;
	int bmpWidthBytes;
	bmpWidthBytes = (current_bitmap->width + 0x07) >> 3;

	for (i = 0; i < current_bitmap->height; i++)
    	for (j = 0; j < current_bitmap->width; j++)
 		{
			a = (j&(0x07));
			b = i*(bmpWidthBytes)+((j&0xF8)>>3);
			if ((attr & SHOWBITMAP_SCALE_2x4) == 0)
			{
				if ((attr & SHOWBITMAP_NORMAL_MASK) == SHOWBITMAP_NORMAL)
					scrPoint(x+j,y+i,current_bitmap->bitmap[b]&(0x80>>a));
				else
					scrPoint(x+j,y+i,((UBYTE)~(current_bitmap->bitmap[b]))&(0x80>>a));
			}
			else
			{
				int x1,y1;
				int px = (current_bitmap->bitmap[b]&(0x80>>a));
				for (x1 = x+j*2;x1 < x+j*2+2; x1++)
					for (y1 = y+i*4; y1<y+i*4+4; y1++)
						scrPoint(x1,y1,px);
			}
		}
}

#endif

#ifdef DSAMPLE_COLOUR
void dspl_show_bitmap(int x,int y,t_font_bitmap* current_bitmap,U32 attr )
{
	int i, j;
	int bmpByte;		// which byte in bitmap we are looking at
	UBYTE bmpMask;		// which bit in the (b+w) bitmap we are looking at
	UBYTE bitSet;		// is the bit set or clear.
	U32 bitCol;			// colour of bit in a colour bitmap
	U32 fCol,bCol, col;/*MC SPR 1526*/
	int step;
	int lWord;

	if (current_bitmap==NULL)
		return;
	switch (current_bitmap->format)
	{
	case BMP_FORMAT_BW_UNPACKED:
	case BMP_FORMAT_BW_PACKED:
		//b+w bitmap - deal with both packed and unpacked.
		bmpMask = 0x80;
		bmpByte = 0;
		if (attr & SHOWBITMAP_INVERT) //invert
		{
			fCol = dspl_GetBgdColour();
			bCol = dspl_GetFgdColour();
		}
		else
		{
			fCol = dspl_GetFgdColour();
			bCol = dspl_GetBgdColour();
		}
		if (fCol==bCol)
		{
			fCol = 0x000000FF;
			bCol =0x00FFFF00;
		}
		for (i = 0; i < current_bitmap->height; i++)
		{
			for (j = 0; j < current_bitmap->width; j++)
		    {
				/*	a = (j&(0x07));
					b = i*((current_bitmap->bitmapSize)/(current_bitmap->height))+((j&0xF8)>>3);
					bitSet = current_bitmap->bitmap[b]&(0x80>>a);*/
		    	bitSet = current_bitmap->bitmap[bmpByte]&(bmpMask);

				/*MC SPR 1526*/
				if (bitSet)
					col = fCol;
				else
					col = bCol;
				if (col!=TRANSPARENT_32BIT_COLOUR)/*if colour transparent, don't draw point*/
				{
					scrPoint(x+j,y+i,col);
				}
				/*MC 1526 end*/

				if (bmpMask <= 1)
				{
					bmpMask = 0x80;
					bmpByte++;
				}
				else
					bmpMask = bmpMask >>1;
			}
			// If we have an unpacked bitmaps and we are not already on a new byte, we move to a new byte
			if ((current_bitmap->format == BMP_FORMAT_BW_UNPACKED) && (bmpMask != 0x80))
			{
				bmpMask = 0x80;
				bmpByte++;
			}
		}
		break;
	case BMP_FORMAT_256_COLOUR:
	  //bitmap is 256 colour bitmap - ignore 'attr'
		bmpByte = 0;

		for (i = 0; i < current_bitmap->height; i++)
		{
			for (j = 0; j < current_bitmap->width; j++)
		    {
		    	UINT32 col;
		    	
		    	col = current_bitmap->bitmap[bmpByte];
				if (col!=TRANSPARENT_256_COL)
				{
				bitCol = (((col &0xE0)   <<16) |
						  ((col &0x1C)	<<11) |
						  ((col &0x03)	<< 6));/**/
					//assume 16 bit colour
					scrPoint(x+j,   y+i,bitCol);
				}
				bmpByte = bmpByte+1; //move to next byte

			}
		}
		break;
	case BMP_FORMAT_32BIT_COLOUR:
	  //bitmap is a 32 bit colour bitmap - ignore 'attr'
		bmpByte = 0;
		for (i = 0; i < current_bitmap->height; i++)
		{
			for (j = 0; j < current_bitmap->width; j++)
		    {
		    	//32 bit bitmap - data is in bytes
				//bmpByte = (i*current_bitmap->width+j)*4;
				bitCol = ((current_bitmap->bitmap[bmpByte]   	<<24) |
						(current_bitmap->bitmap[bmpByte+1]	<<16) |
						(current_bitmap->bitmap[bmpByte+2]	<<8)  |
						(current_bitmap->bitmap[bmpByte+3]));
		    	if ((bitCol & 0xFF000000)!=TRANSPARENT_32BIT_COLOUR)
					scrPoint(x+j,y+i,bitCol);
				bmpByte = bmpByte+4; //move to next 32bit word
			}
		}
		break;
	case BMP_FORMAT_16BIT_LCD_COLOUR:				 //32 bits only first 2 bytes used
	case BMP_FORMAT_16BIT_LCD_COMPRESSED_COLOUR: //16 bits
		bmpByte = 0;
		if (current_bitmap->format == BMP_FORMAT_16BIT_LCD_COMPRESSED_COLOUR)
			step = 2;
		else
			step = 4;
		for (i = 0; i < current_bitmap->height; i++)
		{
			for (j = 0; j < current_bitmap->width; j++)
		    {
		    	//32 bit bitmap - data is in bytes
				//bmpByte = (i*current_bitmap->width+j)*4;
//TISH modified for MSIM
//Begin
				bitCol = (U8)(current_bitmap->bitmap[bmpByte+1]);
				bitCol = ((bitCol <<8) | (U8)(current_bitmap->bitmap[bmpByte]));
//				bitCol = ((U8)(current_bitmap->bitmap[bmpByte] <<8) | (U8)(current_bitmap->bitmap[bmpByte+1]));
		    	if ((bitCol & 0x0000FFFF)!=TRANSPARENT_16BIT_COL)
		    	{

					lWord = DSPL_GET_LWORD(x+j,y+i);
//	July 1, 2005 REF: CRR 32677 - xpradipg
//	The data present of all 16bit images are in the negated format hence we 
//    renegate to get it in the right format
#if (defined(ISAMPLE) ||defined(CALLISTO))
//					bitCol = ~bitCol;
#endif
		    		DSPL_SET_BITCOLOUR(x+j,lWord,bitCol,"dsplShowbitmap");
				}
//End
		    	bmpByte = bmpByte+step; //move to next 32/16bit word
			}
		}

		break;
	default: //Unknown format
		break;
	}
}
#endif


#define NO_COLOUR 0xFF123456
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr             |
| STATE   : code                        ROUTINE : dspl_setBorderWidth  |
+--------------------------------------------------------------------+

  PURPOSE : sets current border width

GW 10/02/03 SPR#1721 - Created
*/
static int txtBorder=0;
int dspl_setBorderWidth(int borderSize)
{
	int oldBorder;
	oldBorder = txtBorder;
	if ((borderSize >=0) && (borderSize <=4))
		txtBorder = borderSize;
	return (oldBorder);
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr             |
| STATE   : code                        ROUTINE : dspl_getBorderWidth   |
+--------------------------------------------------------------------+

  PURPOSE : returns current border width

GW 10/02/03 SPR#1721 - Created

*/
int dspl_getBorderWidth(void)
{
	return(txtBorder);
}
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr             |
| STATE   : code                        ROUTINE : scrCharBorderOutput  |
+--------------------------------------------------------------------+

  PURPOSE : draw text with a border

GW 10/02/03 SPR#1721 - Created
*/
void scrCharBorderOutput(t_font_bitmap* chrBmp,
						unsigned int fgdCol,unsigned int bgdCol,unsigned int borderCol,
						int bWidth,
						int px,int py
						)
{
	int x,y,i;
	int line[10];
	int ofs;
	int bgdLine;
	unsigned int col;/*a0393213 lint warnings removal - type of col changed from int to uint*/
	int emask,pmask;
	int init_emask = (0x01 << (28+bWidth));
	int init_pmask = (0x01 << (28+bWidth));
/*

*/
	for (i=0;i<10;i++)
		line[i] = 0;

	//trace("col=",fgdCol, bgdCol,borderCol,0x13);

	//set up border mask
	for (i=1;i<bWidth*2;i++)
	{
		init_emask = init_emask |  (init_pmask >> i) |  (init_pmask << i);
	}
	//trace("masks=",init_emask,init_pmask,0,0x12);

	for (y=-bWidth;y<chrBmp->height+bWidth;y++)
	{
		//trace("y,ht,bWidth",y,chrBmp->height,bWidth, 0x03);
		for (i=0;i<bWidth*2;i++)
			line[i]=line[i+1];
		line[bWidth*2] =0;

		if (y+bWidth<chrBmp->height)
		{
			//work out where we want to display background
			bgdLine = 0x0FFFFFF0;
			bgdLine = bgdLine & (bgdLine << (24-chrBmp->width));
			//Assume bitmaps are unpacked
			switch((chrBmp->width+7) & (0xF8))
			{
			case 0x00:	//no data
					break;
			case 0x08: // 1 byte/char
				ofs = y+bWidth;
//TISH modified for MSIM
				line[bWidth*2] = ((unsigned int)(UBYTE)chrBmp->bitmap[ofs]) << 20;
				break;
			case 0x10: // 2 bytes/char
				ofs = (y+bWidth)*2;
//TISH modified for MSIM
				line[bWidth*2] = (((unsigned int)(UBYTE)chrBmp->bitmap[ofs]) << 20) |
								(((unsigned int)(UBYTE)chrBmp->bitmap[ofs+1]) << 12) ;
				break;
			case 0x18: // 3 bytes/char
				ofs = (y+bWidth)*3;
//TISH modified for MSIM
				line[bWidth*2] = (((unsigned int)(UBYTE)chrBmp->bitmap[ofs]) << 20) |
								(((unsigned int)(UBYTE)chrBmp->bitmap[ofs+1]) << 12)  |
								(((unsigned int)(UBYTE)chrBmp->bitmap[ofs+2]) << 4) ;
				break;
			default:// unable to handle bitmaps > 24 pixels wide
				//output checkerboard pattern as an error
				if (y &0x01)
					line[bWidth*2] = 0x0CCCCCC0;
				else
					line[bWidth*2] = 0x05555550;
				break;
			}
		}
		else
		{
			line[bWidth*2] = 0x00000000;
			bgdLine = 0x00;
		}
		//trace("line=",line[0],line[1], line[2], 0x13);

		emask = init_emask; // if any of the 3 pixels are set - erase point
		pmask = init_pmask; // only when pixel is set - plot point.

		for (x=-bWidth;x<chrBmp->width+bWidth;x++)
		{
			col = NO_COLOUR;
			if (line[bWidth] & pmask)
			{
				//pixel is in fgd colour
				col = fgdCol;
			}
			else
			{
				for (i=0;i<bWidth*2+1;i++)
				{
					if (line[i] & emask)
						col = borderCol;
				}
				if ((bgdLine & pmask) && (col == NO_COLOUR))
				{
					col = bgdCol;
				}
			}
			if ((col != NO_COLOUR) && (col != TRANSPARENT_32BIT_COLOUR))
				scrPoint(px+x,py+y,(int)col);
			emask = emask>>1;
			pmask = pmask>>1;
//			trace("masks=",emask,pmask,col,0x13);
		}
	}
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr             |
| STATE   : code                        ROUTINE : scrFastCharOutput    |
+--------------------------------------------------------------------+

  PURPOSE : draw text avoiding using scrPoint

GW 10/02/03 SPR#1736 - Created
*/
#ifdef DSAMPLE_COLOUR

void scrFastCharOutput(t_font_bitmap* chrBmp,
						unsigned int fColconv,unsigned int bColconv,
						int x,int y
						)
{
	int fHeight = dspl_GetFontHeight();
	int ofs = chrBmp->height - fHeight;
	int width = chrBmp->width;
	int i,j;
	int sx,sy,ex,ey,lWord;
	int bitSet;
	UBYTE mask;
	int ofsj,nbytes;
	unsigned int col;
	//ensure that the data is never wiritten 'off-screen'
	if (x+width > win_x2)
		ex = win_x2-x;
	else
		ex = width;
	if (y+fHeight > win_y2)
		ey = win_y2-y;
	else
		ey = fHeight;
	if (x < win_x1)
		sx = win_x1-x;
	else
		sx = 0;
	if (y < win_y1)
		sy = win_y1-y;
	else
		sy = 0;

	nbytes = (width+7) >>3;
	ofs = ofs + sy;
	for (i=sy;i<ey;i++)
	{
		mask = 0x80 >> (sx & 0x07);
		ofsj = sx >> 3;
		for (j=sx;j<ex;j++)
		{
			//Have we data for the bitmap ?. If not, output a blank line.
			if (ofs >=0)
				bitSet = chrBmp->bitmap[ofs*nbytes+ofsj]&(mask);
			else
				bitSet = 0;

			if (bitSet)
				col = fColconv;
			else
				col = bColconv;
			if (col!=TRANSPARENT_32BIT_COLOUR)
			{
			    lWord = DSPL_GET_LWORD(j+x,i+y);
			    DSPL_SET_BITCOLOUR(j+x,lWord,col,"scrFastCharOutput");
			}
			mask = mask >> 1;
			if (mask==0)
			{
				ofsj++;
				mask = 0x80;
			}
		}
		ofs++;
	}
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr              |
| STATE   : code                        ROUTINE : scrCharOutput        |
+--------------------------------------------------------------------+

  PURPOSE : draw text

GW 10/02/03 SPR#1736 - Created
*/
void scrCharOutput(t_font_bitmap* chrBmp,
						unsigned int fCol,unsigned int bCol,
						int x,int y,
						int wChar, int hChar
						)
{
	int fHeight = dspl_GetFontHeight();
	int ofs = chrBmp->height - fHeight;
	int width = chrBmp->width;
	int i,j,k = 0;
	int bitSet;
	
	int ofsj,nbytes;
	unsigned int col;
	int shiftj;
	ofs = chrBmp->height - fHeight;
	width = chrBmp->width;
	k = 0;
	for (i=0;i<fHeight;i++)
	{
		k = 0;
		nbytes = (width+7) >>3;
		for (j=0;j<width;j++)
		{
			shiftj= j & 0x07;
			ofsj = j >> 3;
			//Have we data for the bitmap ?. If not, output a blank line.
			if (ofs >=0)
				bitSet = chrBmp->bitmap[ofs*nbytes+ofsj]&(0x80>>shiftj);
			else
				bitSet = 0;

			if (bitSet)
				col = fCol;
			else
				col = bCol;
			if (col!=TRANSPARENT_32BIT_COLOUR)
			{
				if (!hChar)
					scrPoint(k+x,i+y,col);
				else
				{
					scrPoint(k+x,i*2+y,col);
					scrPoint(k+x,i*2+1+y,col);
				}
				if (wChar)
				{
					if ((j>=1) && (j<=wChar))
					{	//for j=1,2,3 - plot 2 pixels
						k++;
						if (!hChar)
							scrPoint(k+x,i+y,col);
						else
						{
							scrPoint(k+x,i*2+y,col);
							scrPoint(k+x,i*2+1+y,col);
						}
					}
				}
			}
			k++;
		}
		ofs++;
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr              |
| STATE   : code                        ROUTINE : dspl_ScrText         |
+--------------------------------------------------------------------+

  PURPOSE : draw a line of ascii/unicode text

GW 18/09/01 - Made invert flag local to file not procedure
GW 10/02/03 SPR#1721	Added code to call separate procedure if a border is required.
						Removed existing 'border' code.
GW 14/02/03 SPR#1736 - Split code into separate procedures
						Modified error display code to output ascii characters of the code if no
						matching bitmap is found.
*/
void dspl_ScrText (int x, int y, char *txt, int style)
{
	int nChar = 0;
    int i, j, k;
	t_font_bitmap* chrBmp;
	t_font_bitmap* errBmp[2] = {NULL,NULL};
	int str1, str2;
	int border = 0;
	int shadow = 0;
	int fHeight;
	int wChar,hChar,shadowHt;
	unsigned int fCol,bCol,borderCol,shadowCol,fColconv,bColconv;
	
	int startX;
    USHORT code;

	startX = x;
	shadowCol = 0;
	borderCol = 0;
	border = dspl_getBorderWidth();
	//default to wide chars (for the moment)
	if (style & TXT_STYLE_WIDECHAR)
		wChar = 3;
	else
		wChar = 0;
	if (style & TXT_STYLE_HIGHCHAR)
		hChar = 1;
	else
		hChar = 0;

	//Set-up default colours
	fCol = dspl_GetFgdColour();
	bCol = dspl_GetBgdColour();
	switch (style & TXT_STYLE_MASK)
    {
	case TXT_STYLE_NORMAL:	//Normal
	default:
		break;
	case TXT_STYLE_INVERT:	//Invert
		fCol = dspl_GetBgdColour();
		bCol = dspl_GetFgdColour();
		break;
	case TXT_STYLE_HIGHLIGHT:	//highlight - colour background of selected item in differently.
		bCol = dspl_GetHighlightColour();
		break;
	case TXT_STYLE_BORDER:	// 1 pixelborder
		if (border==0)
			border = 1;
		break;
	case TXT_STYLE_2PIXEL_BORDER:	// 2 pixelborder
		if (border==0)
			border = 2;
		break;
	case TXT_STYLE_3PIXEL_BORDER:	// 3 pixelborder
		if (border==0)
			border = 3;
		break;
	case TXT_STYLE_4PIXEL_BORDER:	// 4 pixelborder
		if (border==0)
			border = 4;
		break;
	case TXT_STYLE_SHADOW1: //shadow
		shadow = 1;
		break;
	case TXT_STYLE_SHADOW2: //deeper shadow
		shadow = 2;
		break;
	}

	if (fCol==bCol)
	{
		fCol = 0x00FF0000;
		bCol = 0x0000FFFF;
	}
	if (border)
	{
		borderCol = dspl_GetBorderColour();
		if (borderCol == fCol)
		{	//A border colour the same as the text colour - get a contrasting colour
			borderCol = dspl_GetContrastColour(fCol);
		}
	}
	else
		borderCol = 0;

	if (shadow)
		shadowCol = dspl_GetShadowColour();
	else
		shadowCol = 0;

	//	SH - modified because it doesn't allow text to start off the screen
	// GW - allow text to start off-screen but finish if completely off-screen
    if (y > scrY)
        return;
	if (txt ==NULL)
		return;

	fHeight = dspl_GetFontHeight();
	if (hChar)
		shadowHt = fHeight*2;
	else
		shadowHt = fHeight;

	//For faster operation - precalculate colours
#ifdef DSAMPLE_COLOUR
	if (fCol!=TRANSPARENT_32BIT_COLOUR)
		fColconv = DSPL_GET_BITCOLOUR(fCol);
	else
		fColconv = fCol;
	if (bCol!=TRANSPARENT_32BIT_COLOUR)
		bColconv = DSPL_GET_BITCOLOUR(bCol);
	else
		bColconv = bCol;
#endif
	code = 0xFFFF;
    while ((code !=0) &&(x <= scrX))
    {
    	nChar++;
	    /*mc SPR 1319*/
    	if (style &TXT_STYLE_UNICODE)
    	{
    		str1 = *txt;
    		str2 = *(txt+1);
    		code = ((str1<<8)&0xFF00) | (str2&0x00ff);
    	}
    	else/*mc end*/
    	{
    		str1 = *txt;
    		str2 = 0x00;
    		/*SPR 1508, convert ASCII char to unicode before drawing it*/
    		code = translation_ascii_2_unicode[*txt];
    	}
    	if (code != 0)
		{
			chrBmp = get_bitmap(code);
			if (chrBmp == NULL)
			{
				errBmp[0] = get_bitmap(str1);
				errBmp[1] = get_bitmap(str2);
			}
#ifdef MMI_TRACE
			{
				char bfr[40];
				if ((chrBmp==NULL) || (chrBmp->code==0xFFFF))
				{
					sprintf(bfr,"Undefined bitmap for 0x%04X",code);
					mmi_trace(bfr);
				}
			}
#endif
		}
    	else //end of string reached
			chrBmp =NULL;
		if (chrBmp != NULL)
		{
			if (border)
			{
				scrCharBorderOutput(chrBmp,fCol,bCol,borderCol,border,x,y);
			}
#ifdef DSAMPLE_COLOUR
			else if ((!hChar) && (!wChar))
			{	//GW 28/11/02 - for speed, don't use 'scrPoint'
				//NB Code is only correct for a D-sample
				scrFastCharOutput(chrBmp,fColconv,bColconv,x,y);
			}
#endif //DSAMPLE_COLOUR
			else
			{
				scrCharOutput(chrBmp,fCol,bCol,x,y,wChar,hChar);
			}
		    x = x + chrBmp->width + wChar; //add 3 for 'wide' chars
		    //If there is a pixel border between chars at the moment -
		    x = x+border;
		}
		else if (code!=0)
		{	//We have no bitmap - display ascii equivalent
			for (i=0;i<2;i++)
			{	if (errBmp[i]!= NULL)
				{
					scrCharOutput(errBmp[i],fCol,bCol,x,y,wChar,hChar);
				    x = x + errBmp[i]->width + wChar;
				    errBmp[i] = NULL;
				}
			}
		}
		/*MC SPR 1319*/
		if (style &TXT_STYLE_UNICODE)
			txt+=2;
		else
		/*mc end*/
			txt++;
	}
    if (shadow)
	{
		for (i=0;i<shadowHt;i++)
		{
			for (k=0;k<shadow;k++)
				scrPoint(x+k,i+y+k,shadowCol);
		}
		for (j=startX;j<x;j++)
		{
			for (k=0;k<shadow;k++)
				scrPoint(j+1+k,shadowHt+y+k, shadowCol);
		}
	}
#ifdef MMI_TRACE

	{
		char bfr[40];
		sprintf(bfr,"Nchr=%d (%d,%d)",nChar,x,y);
		mmi_trace(bfr);
	}
#endif
}


void scrText (int x, int y, char *txt)
{
	//Display non-inverted text
	dspl_ScrText (x,y, txt, TXT_STYLE_NORMAL);
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrText_Invert_Unicode|
+--------------------------------------------------------------------+

  PURPOSE : draw text inverted

*/

void scrText_Invert_Unicode (int x, int y, USHORT *txt)
{
  UBYTE current_width=0;
  USHORT current_code,first_byte_code,second_byte_code;
  t_font_bitmap* current_bitmap,*error_bitmap;
    if (x < 0 || y < 0 || y + 12 > scrY)
        return;



    while ((*txt) && (x + current_width <= scrX))
    {
    current_code = (USHORT)((*txt)<<8)+(USHORT)((*txt)>>8);/*The USHORT is inverted, therefore we have to apply this simple change*/
    current_bitmap = get_bitmap(current_code);
		if (!current_bitmap)	/*ASCII codes will be represented*/
    {
      first_byte_code = (USHORT)((*txt)&0xFF);
      error_bitmap = get_bitmap(first_byte_code);
      if (error_bitmap)
      {
        dspl_show_bitmap(x,y,error_bitmap,SHOWBITMAP_INVERT);
            x += error_bitmap->width;
      }
      second_byte_code = (USHORT)((*txt)>>8);
      error_bitmap = get_bitmap(second_byte_code);
      if (error_bitmap)
      {
        dspl_show_bitmap(x,y,error_bitmap,SHOWBITMAP_INVERT);
            x += error_bitmap->width;
      }

    }
    else
    {
      dspl_show_bitmap(x,y,current_bitmap,SHOWBITMAP_INVERT);
          x += current_bitmap->width;
    }
      txt++;
  }

}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrText_Invert     |
+--------------------------------------------------------------------+

  PURPOSE : draw text inverted

*/

void scrText_Invert (int x, int y, char *txt)
{
	//Display inverted text
	dspl_ScrText (x,y, txt, TXT_STYLE_INVERT);
}


GLOBAL UBYTE   dspl_str_length(char * str)
{
  UBYTE count=0;
  USHORT* current;

  if (dspl_get_char_type() == DSPL_TYPE_ASCII)
        return (strlen(str));
  else
  {
    current = (USHORT*) str;
    while (*current)
    {
      count++;
      current++;
    }
    return count;
  }
}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrLine            |
+--------------------------------------------------------------------+

  PURPOSE : draw line

*/

void scrLine (int x1, int y1, int x2, int y2)
{
    int x, y, dx, dy, c1, c2, err, swap = 0;
	U32 fCol;

    if (x1 < 0 || x1 >= scrX || x2 < 0 || x2 >= scrX ||
        y1 < 0 || y1 >= scrY || y2 < 0 || y2 >= scrY)
        return;
	fCol = dspl_GetFgdColour();
    dx = x2 - x1; dy = y2 - y1;
    if (((dx < 0) ? -dx : dx) < ((dy < 0) ? -dy : dy))
    {
        swap = 1;                       /* take the long way        */
        x = x1; x1 = y1; y1 = x;
        x = x2; x2 = y2; y2 = x;
    }
    if (x1 > x2)
    {
        x = x1; x1 = x2; x2 = x;        /* always move to the right */
        y = y1; y1 = y2; y2 = y;
    }

    dx = x2 - x1; dy = y2 - y1;
    c1 = dy * 2; dy = 1;
    if (c1 < 0)
    {
        c1 = -c1;
        dy = -1;
    }
    err = c1 - dx; c2 = err - dx;
    x = x1; y = y1;
    while (x <= x2)
    {
        scrPoint(swap?y:x,swap?x:y,fCol);
        x++;
        if (err < 0)
            err += c1;
        else
        {
            y += dy;
            err += c2;
        }
    }

    if (internal)
        return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrRect            |
+--------------------------------------------------------------------+

  PURPOSE : draw rectangle

*/

void scrRect (int px, int py, int sx, int sy)
{
    internal = 1;
    scrLine(px,py,px+sx,py);
    scrLine(px,py+sy,px+sx,py+sy);
    scrLine(px,py,px,py+sy);
    internal = 0;
    scrLine(px+sx,py,px+sx,py+sy);
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrFntHeight       |
+--------------------------------------------------------------------+

  PURPOSE : get height of current font

*/

int scrFntHeight (void)
{
#ifdef LSCREEN
	return 16;
#else
  if (dspl_char_type == DSPL_TYPE_ASCII)
    return 8;
  else
    return 12;
#endif
}



//Really get information about
//	(1) how many chars we can have on a line
//  (2) how many pixels are in a string
//GW SPR#1721 - Include a border in the calculations
int scrGetRealLength(char *str, int* totChar, int* totPixel)
{
	int nChar=0;
	int nPixel=0;
	int maxChar,maxPixel;
	t_font_bitmap* bmp;
	int strEnd;
	USHORT str16,str1,str2;
	UBYTE current_display_char_type = dspl_char_type;/*mc SPR 1319*/
	int border;

	//Calculate extra pixels allowed for the border
	border = dspl_getBorderWidth();

	if (*totChar<=0) //assume full str required.
		maxChar = 256; //stop after 256 chars
	else
		maxChar = *totChar; //stop after 256 chars
	if (*totPixel<=0)// assume no limit to len - limit at 2x screen
		maxPixel = 2*scrX;
	else
		maxPixel = *totPixel;
	strEnd = FALSE;
	/*MC SPR 1319, char type is unicode if unicode tag*/
//TISH modified for MSIM
	if((unsigned char)(str[0]) == 0x80)
	{	current_display_char_type = DSPL_TYPE_UNICODE;
		str+=2; /*ignore first char SPR 1442*/
		nChar+=2;/*MC*/
	}
	else /*SPR 1526, assume if no unicode tag, we're in ASCII*/
		current_display_char_type = DSPL_TYPE_ASCII;
	/*MC 1526, end*/
	while (!strEnd)
	{
		if (current_display_char_type == DSPL_TYPE_ASCII)
		{
			str16 = *str;
			str++;
		}
		else
		{
			str1 = *str;
			str++;
			str2 = *str;
			str++;
			str16 = ((str1 << 8) & 0xFF00) + (str2 & 0x00FF);/*MC, fixed bitmask 1319*/

		}
		if (str16 == 0)
		{	//reached end of string.
			strEnd = TRUE;
			bmp = NULL;
		}
		else
			bmp = get_bitmap(str16);
		if (bmp!= NULL)
		{
			if (nPixel +bmp->width < maxPixel)
			{
				nPixel = nPixel + bmp->width+border;
				if (current_display_char_type == DSPL_TYPE_ASCII)
					nChar++;
				else
					nChar+=2;
				if (nChar >= maxChar)
					strEnd = TRUE;
			}
			else
				strEnd=TRUE;
		}
	}
	*totChar = nChar;
	*totPixel = nPixel;

	return (nPixel);
}




/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrFntGetFit       |
+--------------------------------------------------------------------+

  PURPOSE : number of string chars that fit into the line

*/


int scrFntGetFit (char *tp, int lineWidth)
{
	

//	scrGetRealLength(tp, &totChar, &lineWidth);
//	return (totChar);


  if (dspl_char_type == DSPL_TYPE_ASCII)
    return (lineWidth /6);
  else
      return (lineWidth / 12);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrFntGetLen       |
+--------------------------------------------------------------------+

  PURPOSE : number of pixel used by first n string chars

*/




int scrFntGetLen(char *tp, int nChars)
{
	


  int count=0;

  if (dspl_char_type == DSPL_TYPE_ASCII)
      return (nChars * 6);
  else
  {
    USHORT* temp = (USHORT*)tp;

    while (*temp)
    {
      UBYTE first_byte = (UBYTE)(*temp);
      if (first_byte)
        count+=12;
      else
        count+=6;
      temp++;
    }
      return count;
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrBmpDraw         |
+--------------------------------------------------------------------+

  PURPOSE : draw bitmap

*/
/*a0393213(R.Prabakar) lint warnings removal
   warning:symbol(scrBmpDraw) not referenced
   soln     :this function was not at all called. so this function has been put under MMI_LINT_WARNING_REMOVAL,
               which is not defined. If this function is needed this flag can be removed*/
#ifdef MMI_LINT_WARNING_REMOVAL 
int scrBmpDraw (int px, int py, int sx, int sy,
                    int ix, char *bm, int mode)
{
    int i, j;
    unsigned char m;

    if (px < 0 || px >= scrX || py < 0 || py >= scrY
                                       || sx < 0 || sy < 0)
        return 0;
    if (px + sx > scrX)
        sx = scrX - px;
    if (py + sy > scrY)
        sy = scrY - py;

    m = 0x80;
    bm += (sx * sy * ix) / 8;
    for (j = 0; j < sy; j++)
    {
        for (i = 0; i < sx; i++)
        {
            scrPoint(px+i,py+j,*bm & m);
            m >>= 1;
            if (!m)
            {
                m = 0x80;
                bm++;
            }
        }
        if (m != 0x80)
        {
            m = 0x80;
            bm++;
        }
    }

    return 0;
}

#endif
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrCursor          |
+--------------------------------------------------------------------+

  PURPOSE : set cursor at specified position

*/
/*a0393213(R.Prabakar) lint warnings removal
   warning:symbol(scrCursor) not referenced
   soln     :this function was not at all called. so this function has been put under MMI_LINT_WARNING_REMOVAL,
               which is not defined. If this function is needed this flag can be removed*/
#ifdef MMI_LINT_WARNING_REMOVAL
void scrCursor (int x, int y)
{
    scrLine(x,y+7,x+5,y+7);
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_scr            |
| STATE   : code                        ROUTINE : scrPoint           |
+--------------------------------------------------------------------+
  PURPOSE : draw point
*/

//Return colour of pixel at point (px,py) or zero if not on screen.
/*a0393213(R.Prabakar) lint warnings removal
   warning:symbol(checkPixelColour) not referenced
   soln     :this function was not at all called. so this function has been put under MMI_LINT_WARNING_REMOVAL,
               which is not defined. If this function is needed this flag can be removed*/
#ifdef MMI_LINT_WARNING_REMOVAL 
static int getScrPoint (int px, int py)
{
#ifdef BSAMPLE
    unsigned char m = 0x80;
#endif //BSAMPLE
#ifdef CSAMPLE
	U32 m2;
#endif //CSAMPLE
	int x1,y1;
	int lWord; // index for colour word
	U32 opWord; //olour word
	
	

    if (px < 0 || py < 0 || px >= scrX || py >= scrY)
        return (0);

#ifdef BSAMPLE
    m >>= px & 7;                       // mask
    px >>= 3; // x offset
	if (displayScr[py][px] & m)
		return (0x00FFFFFF);
	else
		return (0x00000000);

#endif //BSAMPLE

#ifdef CSAMPLE
    //Display plots data from Right to Left
    // - reverse x-coordinate
    //Also display seems to be 16 pixels down the screen -
    //plot points at y=(y-16) mod 64
    x1 = (scrX-1)-px;
    y1 = (py+48) & 63; //(py-16) ;
    //Display is in columns for TI drivers
	m2 = 0x80000000;
    m2 = m2 >> (y1 & 31); //shift m2 by 0-31
    y1 = y1 >> 5; // 32bits/word
	return (picture_bw[lWord] & m2);
#endif //CSAMPLE

#ifdef DSAMPLE_COLOUR
	//i/p colour is 32 bit - reduce to 16
    x1 = px;
    y1 = py;
    lWord = y1*LCD_ROW_NUM_LWORDS + x1;
    //GW invert pic bits
	opWord = ~picture_col[lWord];

	//Change from 16(8) bits to 32(24) bits
#ifdef DSAMPLE_COLOUR32
	opWord = opWord & 0x0000FFFF;
#endif //DSAMPLE_COLOUR32
#ifdef DSAMPLE_COLOUR16
#ifdef PIXEL_0_IN_LSW
	if (lWord & 0x01)
		opWord = ((opWord >>16) & 0x0000FFFF);
	else
		opWord = ((opWord     ) & 0x0000FFFF);
#else
	if (lWord & 0x01)
		opWord = ((opWord     ) & 0x0000FFFF);
	else
		opWord = ((opWord >>16) & 0x0000FFFF);
#endif
#endif //DSAMPLE_COLOUR16
	return(DSPL_GET_PIXELCOLOUR(opWord));
#endif //DSAMPLE_COLOUR

#ifdef DSAMPLE_LITE
	//i/p colour is 32 bit - reduce to 16
    {
    	int xmask;
    	x1 = px;
    	y1 = py;
    	lWord = DSPL_GET_LWORD(x1,y1); //y1*LCD_ROW_NUM_LWORDS + x1 >> 5;
    	xmask = 0x01 << (x1 & 0x1F);
    	//GW invert pic bits
		if (picture_bw[lWord] & xmask)
			opWord = 0x00FFFFFF;
		else
			opWord = 0x00000000;
		//Change from 16(8) bits to 32(24) bits
	}
	return (opWord);
#endif //DSAMPLE_LITE

}
#endif
/*a0393213(R.Prabakar) lint warnings removal
   warning:symbol(checkPixelColour) not referenced
   soln     :this function was not at all called. so this function has been put under MMI_LINT_WARNING_REMOVAL,
               which is not defined. If this function is needed this flag can be removed*/
#ifdef MMI_LINT_WARNING_REMOVAL 
static int checkPixelColour(int px, int py, U32 col)
{
	U32 currCol;

	currCol = getScrPoint (px, py);
	if ((currCol & 0x00F8FCF8) == (col & 0x00F8FCF8))//16bit colour
		return (1); //colours match
	else
		return (0); //colours are different
}
#endif
unsigned int translucence(unsigned int ipCol,unsigned int picCol)
{
	int trans1,trans2;
	U32 col_red1,col_green1,col_blue1;
	U32 col_red2,col_green2,col_blue2;
	U32 rmerge,gmerge,bmerge;
	U32 opCol;

	//0xFF=transparent	(col=picCol)
	//0x00=solid 		(col=ipCol)
	trans1 = (ipCol >>24) & 0xFF;
	trans2 = 255-trans1;
	if (trans1 == 0x00)
		return (ipCol);
	else if (trans1 == 0xFF)
		return (picCol);

	//Read in colour from 24 bits
	col_red1  	= (ipCol & 0x00FF0000) >> 8;
	col_green1	= (ipCol & 0x0000FF00) >> 8;
	col_blue1	= (ipCol & 0x000000FF);

	//Read in colour from 8 bits
	col_red2  	= (picCol & 0x00FF0000) >> 8;
	col_green2	= (picCol & 0x0000FF00) >> 8;
	col_blue2	= (picCol & 0x000000FF);
	//red = 0x0000xx00, green = 0x000000xx, blue = 0x000000xx (256x higher than it should be)

	//Round values up
//	rmerge = (col_red1  *trans1+col_red2  *trans2+0x0FFFF)      & 0x00FF0000;
//	gmerge = (col_green1*trans1+col_green2*trans2+0x000FF)      & 0x0000FF00;
//	bmerge =((col_blue1 *trans1+col_blue2 *trans2+0x000FF) >> 8)& 0x000000FF;
	rmerge = (col_red1  *trans2+col_red2  *trans1+0x1F0000)      & 0x00FF0000;
	gmerge = (col_green1*trans2+col_green2*trans1+0x001000)      & 0x0000FF00;
	bmerge =((col_blue1 *trans2+col_blue2 *trans1+0x001000) >> 8)& 0x000000FF;

	opCol = rmerge|gmerge|bmerge;
	return (opCol);
}


#ifdef BSAMPLE
static void scrPoint (int px, int py, int col)
{
    unsigned char m = 0x80;

    //GW Do not plot if outside of the defined window
    if (px < win_x1 || py < win_y1 || px >= win_x2 || py >= win_y2)
        return;

    m >>= px & 7;                       // mask
    px >>= 3;                           // x offset

    if (col)
		displayScr[py][px] |= m;
    else
    	displayScr[py][px] &= (UBYTE) ~m;
}
#endif //BSAMPLE

#ifdef CSAMPLE
static void scrPoint (int px, int py, int col)
{
	U32 m2;
	int x1,y1;
	int lWord; // index for colour word
    if (px < win_x1 || py < win_y1 || px >= win_x2 || py >= win_y2)
        return;
    //Display plots data from Right to Left
    // - reverse x-coordinate
    //Also display seems to be 16 pixels down the screen -
    //plot points at y=(y-16) mod 64
    x1 = (scrX-1)-px;
    y1 = (py+48) & 63; //(py-16) ;
    //Display is in columns for TI drivers
	m2 = 0x80000000;
    m2 = m2 >> (y1 & 31); //shift m2 by 0-31
    y1 = y1 >> 5; // 32bits/word
    lWord = x1*LCD_COL_NUM_LWORDS+y1;
	if (col)
		picture_bw[lWord] |= m2;
	else
		picture_bw[lWord] &= (U32)(~m2);
}
#endif //CSAMPLE

#ifdef DSAMPLE_COLOUR
void scrPoint (int px, int py, int col)
{
	int lWord; // index for colour word
	U32 bitCol;
	//i/p colour is 32 bit
    if (px < win_x1 || py < win_y1 || px >= win_x2 || py >= win_y2)
        return;
    lWord = DSPL_GET_LWORD(px,py);
	bitCol = DSPL_GET_BITCOLOUR(col);
    DSPL_SET_BITCOLOUR(px,lWord,bitCol,"plot");
}
#endif //DSAMPLE_COLOUR

#ifdef DSAMPLE_LITE
void scrPoint (int px, int py, int col)
{
	int m_px;
	int word ;
	//i/p colour is 32 bit
    if (px < win_x1 || py < win_y1 || px >= win_x2 || py >= win_y2)
        return;

	m_px = 1 << (px & 0x1F);
	word = DSPL_GET_LWORD(px,py);
	if (col)
		picture_bw[word] = picture_bw[word] | m_px;
	else
		picture_bw[word] = picture_bw[word] & (~m_px);
}
#endif //DSAMPLE_LITE



/*******************************************************************
 *                                                                 *
 * PART III: LCD Simulation for Windows                            *
 *                                                                 *
 *******************************************************************/
#if defined (WIN32)

#define LCDE_COL 20
#define LCDE_ROW 4
static int screensize_x = 16;	
static int screensize_y = 4;

#define NULL_TERM   '\0'  /* string termination                   */

static CHAR lcd_screen [LCDE_ROW+2][LCDE_COL+3];
static CHAR cursor_line[LCDE_COL+3];

static int lcd_cursX, lcd_cursY, lcd_enabled = FALSE;

LOCAL void LCD_Refresh (void)
{
  int i;
  CHAR buf[100];

  for (i=0; i<screensize_y+2; i++)
  {
    sprintf (buf, "~LCD~%s", &lcd_screen[i][0]);
#if defined NEW_FRAME
    vsi_o_ttrace (0, 0xffff, buf);
#else
#endif
    if (lcd_cursY+1 EQ i)
    {
      cursor_line[lcd_cursX+1] = '^';
      sprintf (buf, "~LCD~%s", cursor_line);
#if defined NEW_FRAME
      vsi_o_ttrace (0, 0xffff, buf);
#else
#endif
      cursor_line[lcd_cursX+1] = ' ';
    }
    else
    {
      if (i < screensize_y+1)
      {
        sprintf (buf, "~LCD~%s", cursor_line);
#if defined NEW_FRAME
        vsi_o_ttrace (0, 0xffff, buf);
#else
#endif
      }
    }
  }
}

GLOBAL void LCD_Enable (int on)
{
  lcd_enabled = (on EQ 1);
}

GLOBAL void LCD_Clear (void)
{
  int r, c;

  if (lcd_enabled)
  {
    lcd_cursX = lcd_cursY = 0;

    for (r=1; r<screensize_y+1; r++)
      for (c=1; c<screensize_x+1; c++)
        lcd_screen[r][c] = ' ';

    LCD_Refresh ();
  }
}

GLOBAL void LCD_Reset (void)
{
  int r, c, i;

  memset (cursor_line, ' ', screensize_x+3);
  cursor_line[0] = cursor_line[screensize_x+1] = '|';
  cursor_line[screensize_x+2] = NULL_TERM;

  for (r=1; r < screensize_y+1; r++)
  {
    lcd_screen[r][0] = '|';
    lcd_screen[r][screensize_x+1] = '|';
  }
  for (c=1; c < screensize_x+1; c++)
  {
    lcd_screen[0][c] = '-';
    lcd_screen[screensize_y+1][c] = '-';
  }

  lcd_screen[0][0]                 = '+';
  lcd_screen[screensize_y+1][0]         = '+';
  lcd_screen[0][screensize_x+1]         = '+';
  lcd_screen[screensize_y+1][screensize_x+1] = '+';

  for (i=0; i<screensize_y+2; i++)
    lcd_screen [i][screensize_x+2] = NULL_TERM;

  lcd_enabled = TRUE;

  LCD_Clear ();

  lcd_enabled = FALSE;
}

GLOBAL void LCD_WriteString (int row, int col, CHAR *s)
{
  if (s == NULL)
  {
    TRACE_EVENT("LCD_WriteString got NULL Pointer as Text");
    return;
  }
  if (lcd_enabled)
  {
    int len = strlen (s);

    if (row < screensize_y AND col < screensize_x)
    {
      if ((col + len) > screensize_x)
      {
        /* clip the string */
        s[(len = screensize_x-col)] = NULL_TERM;
      }
      memcpy (&lcd_screen[row+1][col+1], s, len);
      LCD_Refresh ();
    }
  }
}

GLOBAL void LCD_Cursor (int row, int col)
{
  if (lcd_enabled)
  {
    if (row < screensize_y AND col < screensize_x)
    {
      lcd_cursY = row;
      lcd_cursX = col;
      LCD_Refresh ();
    }
  }
}

GLOBAL void LCD_SetPower (void)
{
}

GLOBAL void LCD_PixBlt (int x, int y, int w, int h, char *bmp)
{
}

/*
 * Stimulation of the lcd driver
 */
GLOBAL void lcd_test (UBYTE mode)
{
  TRACE_EVENT ("Display Redirection activated");
  mtkEnabled = mode;
  scrInit();
  if (mtkEnabled EQ 2)
  {
    scrY = 64;
 }
}
#endif /* WIN32 */

//Sept 15, 2005 REF: LOCOSTO-ENH-34257 - xpradipg
//New function added to control the refreshing of the LCD
/*******************************************************************************

 $Function:    	dspl_refresh
 $Description:	controls the refresh of the LCD from MMI
 $Returns:		the current status of refresh
 $Arguments:	State - indicating the state to be set to.

*******************************************************************************/
int	dspl_control(int state)
{

	//	Nov 04, 2005 DR: OMAPS00052595
//#ifndef _SIMULATION_
//TISH modified for MSIM
#if !defined(WIN32) && !defined(FF_SSL_ADAPTATION) //xrashmic 22 Feb, 2006 SSL_Adaptation 

#ifdef FF_MMI_NEW_LCD_API_MIG
	static BOOL refresh_status = DSPL_ACCESS_ENABLE;
	T_LCD_REFCONTROL status;
	TRACE_EVENT_P1("dspl_referesh() - state =>%d",state);	


	if(state == refresh_status)
		return refresh_status;
//	Nov 02, 2005 DR: OMAPS00051051 - xpradipg
//	Check for the state is made before the assignment
	if( state != DSPL_ACCESS_QUERY)
		refresh_status = state;
#endif
		
	switch(state)
	{
		case DSPL_ACCESS_DISABLE:
#ifdef FF_MMI_NEW_LCD_API_MIG
			status = REF_DISABLED;
			TRACE_EVENT("calling lcd_control with disable lcd");
			lcd_control(DISPLAY_MAIN_LCD,LCD_REFCONTROL, (void *)&status);
			return refresh_status;
#endif			
		case DSPL_ACCESS_ENABLE:
#ifdef FF_MMI_NEW_LCD_API_MIG		
			status = REF_ENABLED;
			TRACE_EVENT("calling lcd_control with disable lcd");
			lcd_control(DISPLAY_MAIN_LCD,LCD_REFCONTROL, (void *)&status);
#else
			r2d_g_refresh_disabled = state;
#endif			
		case DSPL_ACCESS_QUERY:	
		default:
#ifdef FF_MMI_NEW_LCD_API_MIG
			return refresh_status;
#else
			return r2d_g_refresh_disabled;
#endif
	}
#else
	return 0;
#endif
}	

#endif /* MMI != 0 */

