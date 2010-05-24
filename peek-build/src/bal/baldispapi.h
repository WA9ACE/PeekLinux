#ifndef _BALDISPAPI_H_
#define _BALDISPAPI_H_

//#include "bal_def.h"
#include "sysdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*--------------------------------------------------------------------
* Macro and data structure defines
*--------------------------------------------------------------------*/

typedef uint32 BalDispColorT;

typedef uint8 BalDispLcdIdT;

/* please do not modify the following lcd ids */
#define BAL_DISP_MAIN_LCD_ID        (BalDispLcdIdT)(0)
#define BAL_DISP_SUB_LCD_ID         (BalDispLcdIdT)(1)

#define BAL_DISP_INVALID_LCD_ID     (BalDispLcdIdT)(0xFF)
typedef enum
{
    BAL_DISP_LCD_OWNER_CBP,
    BAL_DISP_LCD_OWNER_OTHER,
    BAL_DISP_LCD_OWNER_TOTAL
}BalDispLcdOwnerT;

#define DISP_RGB_565(R, G, B)   \
   ( (BalDispColorT) ( (((uint16)((R)>>3) & 0x001F) << 11) + (((uint16)((G)>>2) & 0x003F) << 5) + ((uint16)((B)>>3) & 0x001F)) )

#define BAL_DISP_RGB_WHITE_565  DISP_RGB_565(0xFF, 0xFF, 0xFF)
#define BAL_DISP_RGB_BLACK_565  DISP_RGB_565(0x0, 0x0, 0x0)

#define BAL_DISP_RGB_RED_565    DISP_RGB_565(0xFF, 0x0, 0x0)
#define BAL_DISP_RGB_GREEN_565  DISP_RGB_565(0x0, 0xFF, 0x0)
#define VAl_DISP_RGB_BLUE_565   DISP_RGB_565(0x0, 0x0, 0xFF)
/* defined by wanglili 20080508 */
#define BAL_DISP_RGB_GREY_565	DISP_RGB_565(0xC0, 0xC0, 0xC0)
#define BAL_DISP_RGB_DEEPGREY_565	DISP_RGB_565(0x90, 0x90, 0x90)

#define BAL_DISP_INVALID_COLOR   (0xFFFFFFFF)

typedef enum
{
  BYD_FONT_NORMAL_A,           		 /*16pt*/
  BYD_FONT_NORMAL_ITALIC_A,            /*16I*/
  BYD_FONT_NORMAL_BOLD_A,		/*16B*/
  BYD_FONT_NORMAL_BOLDITALIC_A, 	/*16BI*/
  BYD_FONT_LARGE_A,				/*Arial 20*/
  BYD_FONT_LARGE_ITALIC_A,              /*Arial 20L*/
  BYD_FONT_LARGE_BOLD_A,           	/*Arial 20B*/
  BAL_DISP_FONT_TOTAL
}FontT;

#define BAL_DISP_FONT_NORMAL_A		 	 	BYD_FONT_NORMAL_A
#define BAL_DISP_FONT_NORMAL_ITALIC_A  		BYD_FONT_NORMAL_ITALIC_A
#define BAL_DISP_FONT_NORMAL_BOLD_A		 	BYD_FONT_NORMAL_BOLD_A
#define BAL_DISP_FONT_NORMAL_BOLDITALIC_A	BYD_FONT_NORMAL_BOLDITALIC_A
#define BAL_DISP_FONT_LARGE_A 	 			 	BYD_FONT_LARGE_A
#define BAL_DISP_FONT_LARGE_ITALIC_A 	 	 	BYD_FONT_LARGE_ITALIC_A
#define BAL_DISP_FONT_LARGE_BOLD_A   	 	 	BYD_FONT_LARGE_BOLD_A




 #define BYD_FONT_SMALL_DIGI            BAL_DISP_FONT_NORMAL_A
 #define BYD_FONT_SMALL_BOLD            BAL_DISP_FONT_NORMAL_BOLD_A
 #define BYD_FONT_SMALL_LIGHT          BAL_DISP_FONT_NORMAL_ITALIC_A
 #define BYD_FONT_SMALL_CONDENSED_LIGHT 		BAL_DISP_FONT_NORMAL_ITALIC_A
 #define BYD_FONT_NORMAL_NORMAL         BAL_DISP_FONT_NORMAL_A
 #define BYD_FONT_NORMAL_BOLD          BAL_DISP_FONT_NORMAL_BOLD_A
 #define BYD_FONT_LARGE_BOLD          BAL_DISP_FONT_LARGE_BOLD_A

#define BAL_DISP_FONT_SMALL_PLAIN  BAL_DISP_FONT_NORMAL_ITALIC_A
#define BAL_DISP_FONT_SMALL_BOLD   BAL_DISP_FONT_NORMAL_BOLD_A
#define BAL_DISP_FONT_NORMAL_PLAIN BAL_DISP_FONT_NORMAL_A
#define BAL_DISP_FONT_NORMAL_BOLD  BAL_DISP_FONT_NORMAL_BOLD_A
#define BAL_DISP_FONT_LARGE_PLAIN  BAL_DISP_FONT_LARGE_A
#define BAL_DISP_FONT_LARGE_BOLD   BAL_DISP_FONT_LARGE_BOLD_A

typedef uint32 BalDispFontT;

typedef struct
{
    uint16     StartCode;
    uint16     EndCode;
    uint16     StartIndex;
} BalFontIndexTableT;

typedef struct
{
   BalDispFontT     FontId;
   uint16           GlyphCount;
   uint8            Ascent;
   uint8            Descent;
   bool             FixedWidth;
   uint8            Width;  
   const uint32     *OffsetP; 
   uint16           IndexTableNum;
   const BalFontIndexTableT   *IndexTableP;
   const uint8      *BitmapP;
} BalDispFontInfoT;

typedef enum 
{
    BAL_DISP_FNT_CP_LATIN1 = 0,             /* Basic Latin plus Latin-1 */
    BAL_DISP_FNT_CP_UNICODE,                /* Unicode little mode */
    BAL_DISP_FNT_CP_UNICODE_BIG,            /* Unicode big mode */
    BAL_DISP_FNT_CP_TOTAL                   /* Total number of Codepages */
} BalDispFontCodepageT;

typedef enum
{
    BAL_DISP_RO_COPY,
    BAL_DISP_RO_INVERT,
    BAL_DISP_RO_AND,
    BAL_DISP_RO_OR,
    BAL_DISP_RO_XOR,
    BAL_DISP_RO_ANDNOT,
    BAL_DISP_RO_ORNOT,
    BAL_DISP_RO_TOTAL
} BalDispRasterOpT;  

typedef enum 
{
    BAL_DISP_MODE_TRANSPARENT,
    BAL_DISP_MODE_OPAQUE,
    BAL_DISP_MODE_TOTAL
} BalDispModeT;

typedef enum
{
    BAL_DISP_PEN_0 = 0,                   /* Zero pixel border */
    BAL_DISP_PEN_1 = 1,                   /* One pixel border */
    BAL_DISP_PEN_2 = 2,                   /* two pixel border */
    BAL_DISP_PEN_TOTAL                /* Total number of pens */
} BalDispPenT;

typedef enum 
{
    BAL_DISP_HALIGN_LEFT,
    BAL_DISP_HALIGN_CENTER,
    BAL_DISP_HALIGN_RIGHT
} BalDispHAlignT;

typedef enum 
{ 
    BAL_DISP_VALIGN_UP,         /* Aligns the point with the up side of the bounding rectangle */
    BAL_DISP_VALIGN_CENTER,     /* Aligns the point with the vertical center of the bounding rectangle */
    BAL_DISP_VALIGN_DOWN        /* Aligns the point with the down side of the bounding rectangle */
} BalDispVAlignT;               /* This data type is the alignment used for the text that is generated */

typedef enum
{
    BAL_DISP_TRANSFER_COPY,            /* Copy source to dest; used for opaque images */
    BAL_DISP_TRANSFER_TRANSPARENT,     /* Copy only data that is not the transparent color */
    BAL_DISP_TRANSFER_NON_TRANSPARENT, /* Set dest to specified color before drawing text */
    BAL_DISP_TRANSFER_COLOR_FILL       /* Set dest to specified color */
} BalDispTransferT;

typedef struct 
{
    int16 x, y;               
} BalDispPointT;

typedef struct
{
    int16 x, y;                /* Starting point */
    int16 dx, dy;              /* Width and height */ 
} BalDispRectT;

typedef enum
{
    BAL_DISP_BIT_COUNT_1 = 1,
    BAL_DISP_BIT_COUNT_2 = 2,
    BAL_DISP_BIT_COUNT_8 = 8,
    BAL_DISP_BIT_COUNT_16 = 16,
    BAL_DISP_BIT_COUNT_18 = 18,
    BAL_DISP_BIT_COUNT_24 = 24,
    BAL_DISP_BIT_COUNT_32 = 32,
    BAL_DISP_BIT_COUNT_MAX
} BalDispBitCountTypeT;

typedef enum
{
    BAL_DISP_FORMAT_BMP,
    BAL_DISP_FORMAT_WBMP,
    BAL_DISP_FORMAT_RGB16,
    BAL_DISP_FORMAT_COMPRESSED_RGB,
    BAL_DISP_NUM_FORMATS
} BalDispFormatT;

typedef struct
{
    uint16  Width;                  /* the width of the bitmap */
    uint16  Height;                 /* the height of the bitmap */
    BalDispBitCountTypeT BitCount;  /* the depth of color for pixels */
    uint8*  DataBuf;                /* Points to bitmap buffer */
} BalDispBitmapT;

typedef struct
{
    BalDispLcdIdT           LcdId;
    BalDispBitmapT          Bitmap;
    BalDispBitmapT*         BmpP; 
    BalDispRectT            ClipRect;
    BalDispColorT           TextColor;
    BalDispColorT           BackColor;
    BalDispColorT           PenColor;
    BalDispColorT           BrushColor;
    BalDispModeT            BackMode;
    BalDispColorT           TransColor;
    BalDispPenT             PenType;
    BalDispRasterOpT        RO;
    BalDispFontT            Font;	
    BalDispFontCodepageT    Codepage;
    bool DrawThrough;
} BalDispDevContextT;

typedef enum
{
    BAL_DISP_ERR_NONE,
    BAL_DISP_ERR_INVALID_PARAMETER,
    BAL_DISP_ERR_DRIVER_ERR,
    BAL_DISP_ERR_INVALID_LCD_ID,
    BAL_DISP_ERR_UNSOPPORTED_BITMAP,
    BAL_DISP_ERR_MAX
} BalDispErrorCodeT;

typedef enum
{
    BAL_DISP_CONTRAST_LEVEL_0 = 0,
    BAL_DISP_CONTRAST_LEVEL_1,
    BAL_DISP_CONTRAST_LEVEL_2,
    BAL_DISP_CONTRAST_LEVEL_3,
    BAL_DISP_CONTRAST_LEVEL_4,
    BAL_DISP_CONTRAST_LEVEL_5,
    BAL_DISP_CONTRAST_LEVEL_6,
    BAL_DISP_CONTRAST_LEVEL_7,
    BAL_DISP_CONTRAST_LEVEL_8,
    BAL_DISP_CONTRAST_LEVEL_MAX = BAL_DISP_CONTRAST_LEVEL_8
} BalDispContrastLevelT;

typedef struct
{
   char Vendor[20];    /* max 20 characters */
   char ModelId[20];   /* max 20 characters */
   BalDispBitCountTypeT  BitCount;
   int16      WidthInPixels;
   int16      HeightInPixels;
} BalDispDeviceInfoT;

/* Used for Update the LCD
typedef enum
{
    BAL_DISP_UPDATE_ERROR = 0,
    BAL_DISP_UPDATE_DONE,
    BAL_DISP_UPDATE_OWNER_OTHER,
    BAL_DISP_UPDATE_NO_CHANGE
}BalDispUpdateFlagT;
*/
/* if use enum, there will be an error in edit window, no idea about that */
#define BAL_DISP_UPDATE_ERROR       0
#define BAL_DISP_UPDATE_DONE        1
#define BAL_DISP_UPDATE_OWNER_OTHER 2
#define BAL_DISP_UPDATE_NO_CHANGE   3

/*--------------------------------------------------------------------
* The following functions are to set/get properties in /out of DC
*--------------------------------------------------------------------*/
BalDispErrorCodeT  BalDispInitDC(BalDispDevContextT* dcP, BalDispLcdIdT LcdId, BalDispBitmapT *BitmapP);

BalDispColorT BalDispSetTextColor(BalDispDevContextT* dcP, BalDispColorT Color);
BalDispColorT BalDispGetTextColor(BalDispDevContextT* dcP);

BalDispColorT BalDispSetBackColor(BalDispDevContextT* dcP, BalDispColorT Color);
BalDispColorT BalDispGetBackColor(BalDispDevContextT* dcP);

BalDispModeT BalDispSetBackMode(BalDispDevContextT* dcP, BalDispModeT Mode);
BalDispModeT BalDispGetBackMode(BalDispDevContextT* dcP);

BalDispColorT BalDispSetTransColor(BalDispDevContextT* dcP, BalDispColorT Color);
BalDispColorT BalDispGetTransColor(BalDispDevContextT* dcP);

BalDispPenT BalDispSetPenType(BalDispDevContextT* dcP, BalDispPenT Pen);
BalDispPenT BalDispGetPenType(BalDispDevContextT* dcP);

BalDispColorT BalDispSetPenColor(BalDispDevContextT* dcP, BalDispColorT Color);
BalDispColorT BalDispGetPenColor(BalDispDevContextT* dcP);

BalDispColorT BalDispSetBrushColor(BalDispDevContextT* dcP, BalDispColorT Color);
BalDispColorT BalDispGetBrushColor(BalDispDevContextT* dcP);

BalDispRasterOpT BalDispSetROP(BalDispDevContextT* dcP, BalDispRasterOpT RO);
BalDispRasterOpT BalDispGetROP(BalDispDevContextT* dcP);

BalDispFontT BalDispSetFont(BalDispDevContextT* dcP, BalDispFontT Font);
BalDispFontT BalDispGetFont(BalDispDevContextT* dcP);

BalDispFontCodepageT BalDispSetFontCodepage(BalDispDevContextT* dcP, BalDispFontCodepageT Codepage);
BalDispFontCodepageT BalDispGetFontCodepage(BalDispDevContextT* dcP);
   
BalDispRectT BalDispSetClipRect(BalDispDevContextT* dcP, BalDispRectT* RectP);
BalDispRectT BalDispGetClipRect(BalDispDevContextT* dcP);     
    
bool BalDispSetDrawThrough(BalDispDevContextT* dcP, bool DrawThrough);
bool BalDispGetDrawThrough(BalDispDevContextT* dcP);

/*--------------------------------------------------------------------
* The following functions are to draw and bitblt.
*--------------------------------------------------------------------*/
BalDispColorT BalDispSetPixelColor(BalDispDevContextT* dcP, int16 x, int16 y, BalDispColorT Color);
BalDispColorT BalDispGetPixelColor(BalDispDevContextT* dcP, int16 x, int16 y);

void BalDispDrawLine(BalDispDevContextT* dcP, BalDispPointT P0, BalDispPointT P1);
void BalDispDrawRect(BalDispDevContextT* dcP, BalDispRectT* RectP, bool Filled);
void BalDispEraseRect(BalDispDevContextT* dcP, BalDispRectT* RectP);
void BalDispClearScreen(BalDispDevContextT* dcP);
void BalDispObscureScreen(BalDispDevContextT* dcP);
 
void BalDispBitBlt(BalDispDevContextT* dcP, BalDispRectT* DstRectP, BalDispBitmapT* BmpSrcP, int16 xSrc, int16 ySrc);
void BalDispBitBltTransparent(BalDispDevContextT* dcP, BalDispRectT* DstRectP, BalDispBitmapT* BmpSrcP, int16 xSrc, int16 ySrc);


/*--------------------------------------------------------------------
* The following functions are for font and text
*--------------------------------------------------------------------*/
void BalDispFontRegister(BalDispFontT Font, BalDispFontInfoT* FontInfoP, bool Unicode);
void BalDispFontUnRegister(BalDispFontT Font, BalDispFontInfoT* FontInfoP, bool Unicode);      
void BalDispGetFontInfo(BalDispDevContextT *dcP, uint16* AscentP, uint16* DescentP);
void BalDispInitFontInfo(void);
void BalDispMeasureText(BalDispDevContextT* dcP, uint8* TextP, int16 Chars, int16 MaxWidth, int16* CharFitsP, int16* PixelsP);   
uint16 BalDispGetTextSize(BalDispDevContextT* dcP, uint8* TextP, uint16 Count); 	

void BalDispGetFontCharBitmap(BalDispFontT Font,uint16 CharCode, BalDispBitmapT *BitmapP, 
                                bool OnlyWidth);

void BalDispTextOut(BalDispDevContextT* dcP, int16 x, int16 y, uint8* TextP, uint16 Length);
void BalDispDrawText(BalDispDevContextT* dcP, uint8* TextP, uint16 Count, BalDispRectT* RectP, BalDispHAlignT Align);  

/*--------------------------------------------------------------------
* the following functions are related to lcd
*--------------------------------------------------------------------*/
void BalDispSetLcdOwner(BalDispLcdIdT LcdId, BalDispLcdOwnerT LcdOwner);
void BalDispContrastSet(BalDispLcdIdT LcdId, BalDispContrastLevelT Level);
void BalDispDisplayOn(BalDispLcdIdT LcdId, bool OnOff);
void BalDispLcdDevInfoGet(BalDispLcdIdT LcdId, BalDispDeviceInfoT* LcdInfoP);

uint8 BalDispUpdateDisplayRect(BalDispLcdIdT LcdId, BalDispBitmapT* BmpP, BalDispRectT *RectP);
uint8 BalDispRefreshDisplayRect(BalDispLcdIdT LcdId, BalDispBitmapT* BmpP, BalDispRectT *RectP);
bool BalSetPartialParameters(BalDispLcdIdT LcdId, int16 x, int16 y, int16 dx, int16 dy, bool partialoff);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* #ifndef _BALDISPAPI_H_ */ 
