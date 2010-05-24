/********************************************************************
	created:	2008/01/10
	created:	10:1:2008   17:14
	filename: 	E:\louvro_code\louvro_code\Vin\inc\lcd_if.h
	file path:	E:\louvro_code\louvro_code\Vin\inc
	file base:	lcd_if
	file ext:	h
	author:		Neil Chen
	
	purpose:	
	Copyright (c) 2008-2010, BTC Wireless Limited, Inc.
*********************************************************************/


#ifndef LCD_IF_H
#define LCD_IF_H


#include "sysdefs.h"
#include "lcd/lcd_manager.h"

typedef struct
{
    const char  *VendorP;
    const char  *ModelIdP;
    int16       WidthInPixels;
    int16       HeightInPixels;
    uint8       BitCountPerPixel;
    uint8       *DataBufP;
}HwdLcdInfoT;

typedef uint8 HwdLcdIdT;

/* !!!!!!do not change it!!!!!!! */
#define HWD_LCD_MAIN_ID         (HwdLcdIdT)(0)
#define HWD_LCD_SUB_ID          (HwdLcdIdT)(1)

/* LCD Drive Interface */
void HwdLcdInit(void);
bool HwdLcdGetDevInfo(HwdLcdIdT LcdId, HwdLcdInfoT *HwdLcdInfoP);
bool HwdLcdUpdateAll(HwdLcdIdT LcdId);
bool HwdLcdUpdateRect(HwdLcdIdT LcdId, int16 x, int16 y, int16 dx, int16 dy);
#define HwdLcdRefreshAll(LcdId) HwdLcdUpdateAll(LcdId)
#define HwdLcdRefreshRect(LcdId, x, y, dx, dy) HwdLcdUpdateRect(LcdId, x, y, dx, dy)
void HwdLcdSetContrast(HwdLcdIdT LcdId, uint8 Level);
void HwdLcdBacklightOn(bool On);
void HwdLcdOn(HwdLcdIdT LcdId, bool On);



/****Code for LCD driver only!!!!*****/

/* define LCD parameters */
#define MAIN_LCD_WIDTH      (int16)LCD_COL
#define MAIN_LCD_HEIGHT     (int16)LCD_ROW

#define MAIN_LCD_BITCOUNT   (uint8)(16)
#define MAIN_LCD_PIXEL_SIZE (uint8)(2)



#endif

