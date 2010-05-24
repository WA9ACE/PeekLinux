

#ifndef UIFONTCOLORALOG_H
#define UIFONTCOLORALOG_H 

#include "buidialog.h"
#include "buimsgtimer.h"

typedef struct {
BalDispRectT   Pos;
BalDispColorT TColor;
bool CurFocus;
}ColorST;

class FontColorDialogC:public DialogC
{
public:
	
	FontColorDialogC(BalDispColorT FontColor =BAL_DISP_RGB_BLACK_565);
	virtual ~FontColorDialogC();

	bool CreateEx(void* ExtraDataP, RESOURCE_ID(StringT) StringResId = NULL,uint32 CloseMailID = 0, uint32 WinId = NULL, uint32 Style = WINSTYLE_SKBAR,DispLCDTypeT LcdType = LCD_MAIN);
	
	void DrawFontColorPalette();
	
	void InitFontColor(BalDispColorT color);
	//static BalDispColorT GetFontColor(void){return mSelFontColor;}

protected:
	
	bool OnKeyPress(uint32 /*KeyCode*/);
	void OnClosePopup(){};
	BalDispColorT SelectColorprocess(uint32 KeyCode);
	int DrawFocusColorBG(DCC &dc,BalDispRectT rect);

private:
	BalDispColorT mSelFontColor;
       
	DECLARE_MAIL_MAP()
   
};

#endif
