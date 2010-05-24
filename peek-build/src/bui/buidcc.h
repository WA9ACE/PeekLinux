

#ifndef UIDCC_H
#define UIDCC_H


#include "bal_def.h"
#include "baldispapi.h"

//#include "buistring.h"

//! lcd size type
typedef enum
{
  UI_DISP_LCD_128x128,
  UI_DISP_LCD_128x160,
  UI_DISP_LCD_320x240,
  UI_DISP_LCD_SIZE_NUM
} DispLCDSizeTypeT;

//! colors
#define DISP_RGB_LTGRAY          (DISP_RGB_565(0x15,0x2a,0x15))
#define DISP_RGB_DKGRAY          (DISP_RGB_565(0x0a,0x15,0x0a))
#define DISP_RGB_DKPOPGAY        (DISP_RGB_565(0x87,0x88,0x8a)) //add by maoshanshan for popup gray
#define DISP_RGB_BLUE            (DISP_RGB_565(0, 0, 0xff))

#define UI_DISP_RGB_565(R,G,B)   DISP_RGB_565(R>>3,G>>2,B>>3)

//! base lcd width
#if 0	//qwert
#define UI_DISP_BMP_SCRN_WIDE_BASE             128
#define UI_DISP_BMP_SCRN_HIGH_BASE             160
#else
#define UI_DISP_BMP_SCRN_WIDE_BASE             320
#define UI_DISP_BMP_SCRN_HIGH_BASE             240
#endif

#define UI_DISP_ANN_HIGH_BASE             16
#define UI_DISP_SKB_HIGH_BASE             16

#define UI_DISP_BMP_SCRN_WIDE_SUB_BASE      96
#define UI_DISP_BMP_SCRN_HIGH_SUB_BASE      64

//! real lcd width
#ifdef DISP_LCD_128x128
#define UI_DISP_LCD_SIZE                  UI_DISP_LCD_128x128
#define UI_DISP_BMP_SCRN_WIDE             128
#define UI_DISP_BMP_SCRN_HIGH             128
#elif defined(DISP_LCD_128x160)
#define UI_DISP_LCD_SIZE                  UI_DISP_LCD_128x160
#define UI_DISP_BMP_SCRN_WIDE             128
#define UI_DISP_BMP_SCRN_HIGH             160
#elif defined(DISP_LCD_320x240)	//qwert
#define UI_DISP_LCD_SIZE                  UI_DISP_LCD_320x240
#define UI_DISP_BMP_SCRN_WIDE             320
#define UI_DISP_BMP_SCRN_HIGH             240
#endif

#define UI_DISP_ANN_HIGH             16
#define UI_DISP_SKB_HIGH             16

#define UI_DISP_BMP_SCRN_WIDE_SUB      96
#define UI_DISP_BMP_SCRN_HIGH_SUB      64

extern BalDispRectT MainFullRect;
extern BalDispRectT SubFullRect;

//! color depth and count
#ifdef DISP_LCD_565
#define UI_DISP_LCD_COLOR_DEPTH       16
#define UI_DISP_LCD_COLOR_COUNT       65536
#else
#define UI_DISP_LCD_COLOR_DEPTH       1
#define UI_DISP_LCD_COLOR_COUNT       2 
#endif

//! lcd type
typedef enum
{
  LCD_MAIN,
  LCD_SUB,
  LCD_TOTAL
} DispLCDTypeT;

//! color bit count type
typedef enum
{
  DISP_COLOR_SCHEME_BLACK_WHITE = 1,
  DISP_COLOR_SCHEME_2BITS,
  DISP_COLOR_SCHEME_4BITS,
  DISP_COLOR_SCHEME_8BITS,
  DISP_COLOR_SCHEME_16BITS_565,
  DISP_COLOR_SCHEME_16BITS_555,
  DISP_COLOR_SCHEME_24BITS,
  DISP_COLOR_SCHEME_32BITS
}DispColorSchemeT;

//! 3D type
typedef enum
{
  DISP_3D_NORMAL
}Disp3DTypeT ;


//! brush type
typedef enum
{
  DISP_BRUSH_SOLID
}DispBrushT;


//! brush class
class BrushC
{
public:
  DispBrushT Type;
  BalDispColorT Color;
  BrushC(BalDispColorT color=BAL_DISP_RGB_WHITE_565,DispBrushT type =DISP_BRUSH_SOLID):Type(type),Color(color){};
};


//! pen class
class PenC
{
public:
  BalDispPenT Type;
  BalDispColorT Color;  
  PenC(BalDispColorT color=BAL_DISP_RGB_BLACK_565, BalDispPenT type = BAL_DISP_PEN_0):Type(type),Color(color){};
};

typedef BalDispModeT DispBackGroundModeT;

//! back ground mode class
class BackGroundModeC
{
public:
  DispBackGroundModeT Type;
  BalDispColorT    TransColor;
  BackGroundModeC(DispBackGroundModeT type, BalDispColorT color=0):Type(type),TransColor(color){};
  BackGroundModeC(BalDispColorT color):Type(BAL_DISP_MODE_TRANSPARENT),TransColor(color){};
};


//! font class
class FontC
{ 
public:
  BalDispFontT Type;
  FontC(BalDispFontT type=BAL_DISP_FONT_NORMAL_PLAIN):Type(type){};
};


//! dc class
class DCC
{
public:
  static uint8 UpdateDisplay(DispLCDTypeT LCDType, BalDispBitmapT *BmpP=NULL);
  static uint8 UpdateDisplayRect(DispLCDTypeT LCDType, BalDispBitmapT *BmpP, BalDispRectT &rect);
  
public:
  DCC(DispLCDTypeT LCDType=LCD_MAIN);
  DCC(const DCC &dc);
  ~DCC();
  
  
  //font
  FontC GetFont() const;
  FontC SetFont(const FontC &font);
  void GetFontInfo(uint16 *AscentP, uint16 *DescentP);
  
  //pen
  PenC GetPen() const;
  PenC SetPen(const PenC &pen);
  
  //brush
  BrushC GetBrush() const;
  BrushC SetBrush(const BrushC &brush);
  
  //background mode
  BackGroundModeC GetBackGroundMode() const;
  BackGroundModeC SetBackGroundMode(const BackGroundModeC &BackGroundMode);
  
  //back ground color
  BalDispColorT GetBackGroundColor() const;
  BalDispColorT SetBackGroundColor(BalDispColorT color);
  
  //raster operation
  BalDispRasterOpT GetROP() const;
  BalDispRasterOpT SetROP(BalDispRasterOpT ro);
  
  //font codepage
  BalDispFontCodepageT GetFontCodepage() const;
  BalDispFontCodepageT SetFontCodepage(BalDispFontCodepageT codepage);
  
  //Drawthrough
  bool GetDrawThrough() const;
  bool SetDrawThrough(bool DrawThrough);
  
  //cliprect
  BalDispRectT GetClipRect() const;
  BalDispRectT SetClipRect(const BalDispRectT &rect);
  
  //pixel
  BalDispColorT GetPixel(int16 x, int16 y) const;
  BalDispColorT SetPixel(int16 x, int16 y, BalDispColorT color);
  
  //text
  BalDispColorT GetTextColor() const;
  BalDispColorT SetTextColor(BalDispColorT color);
  
  uint16 GetTextSize(uint8* TextP, uint16 Length);
  //uint16 GetTextSize(const BStringC &str);
  
  void MeasureText(uint8 *TextP, uint16 Length, uint16 MaxWidth, int16 *CharFitsP,int16 *PixelsP);
  //void MeasureText(const BStringC &str, uint16 MaxWidth, int16 *CharFitsP,int16 *PixelsP);
  
  void DrawText(uint8 *Text, uint16 Length, BalDispRectT *RectP, BalDispHAlignT Align);
  //void DrawText(const BStringC &str, BalDispRectT *RectP, BalDispHAlignT Align);

  void DrawGrayText(uint8 *TextP, uint16 Length, BalDispRectT *RectP, BalDispHAlignT Align);
  //void DrawGrayText(const BStringC &str, BalDispRectT *RectP, BalDispHAlignT Align);

  void TextOut(int16 x, int16 y, uint8 *TextP, uint16 Length);
  //void TextOut(int16 x, int16 y, BStringC &str);
  
  //draw
  void DrawLine(const BalDispPointT &P0, const BalDispPointT &P1);
  void DrawRect(const BalDispRectT &Rect, bool filled=FALSE);
  void Draw3DRect(const BalDispRectT &Rect, Disp3DTypeT Type3D=DISP_3D_NORMAL, bool filled=FALSE);
  void EraseRect(const BalDispRectT &Rect);
  
  
  //update Display
  uint8 UpdateDisplay(BalDispBitmapT *BmpP=NULL);
  uint8 UpdateDisplayRect(BalDispBitmapT *BmpP, const BalDispRectT &rect);
  
  //bitblt
  void BitBlt(int16 xDst, int16 yDst, int16 dx, int16 dy, BalDispBitmapT &BmpSrc, int16 xSrc, int16 ySrc);
  void BitBltTransparent(int16 xDst, int16 yDst, int16 dx, int16 dy, BalDispBitmapT &BmpSrc, int16 xSrc, int16 ySrc);
  
  void BitBlt(int16 xDst, int16 yDst, int16 dx, int16 dy, DCC *SrcDCP, int16 xSrc, int16 ySrc);
  void BitBltTransparent(int16 xDst, int16 yDst, int16 dx, int16 dy, DCC *SrcDCP, int16 xSrc, int16 ySrc);
  
  void ClearScreen(); 
  void ObscureScreen();
 
  //memory DC, Bitmap
  bool CreateCompatibleDC(DCC *DCP,BalDispBitmapT *BmpP=NULL);
  bool IsMemoryDC() const;
  
  bool SetBitmap(BalDispBitmapT *BmpP);
  BalDispBitmapT* GetBitmap();
  
  //
  uint16 GetLCDWidth() const;
  uint16 GetLCDHeight() const;
  uint8 GetBitCount() const;
  DispLCDTypeT GetLCDType() const;
  bool SetLCDType(DispLCDTypeT LcdType);
  
  
private:
  BalDispDevContextT mDevContext;
  bool mIsMemoryDC;
  bool mNeedFreeBmp;
};






#endif // UIDCC_H
