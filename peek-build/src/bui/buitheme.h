

#ifndef UITHEME_H
#define UITHEME_H

#include "baldispapi.h"

#include "restypedefines.h"

#include "buimenudefine.h"
#include "buiscrollbar.h"
#include "buieditdefine.h"
#include "buistatictextctrl.h"
#include "buibuttonctrl.h"
#include "buisoftkeybarctrl.h"
#include "buidialog.h"


typedef struct
{
  ScrollBarColorsT ScrollBarColors;
  MenuColorsT MenuColors;
  EditColorsT EditColors;
  StaticTextCtrlColorsT StaticTextColors;
  ButtonCtrlColorsT ButtonColors;
  SoftkeyBarCtrlColorsT SoftkeyBarColors;
  DialogColorsT DialogColors;
  RESOURCE_ID(ImageResT) DlgBgImgId;
  RESOURCE_ID(ImageResT) DlgBgImgSubId;
  RESOURCE_ID(ImageResT) MenuBgImgId;
} UiColorThemeT;


class UiThemeC
{  
public:
  virtual ~UiThemeC(void);
  
  static UiThemeC* GetInstance(void);
  static void DelInstance(void);

  void SetColorTheme(uint8 Type);
  const UiColorThemeT* GetColorTheme(void);
  
  ImageC* GetMenuBgImg(void);
  ImageC* GetDialogBgImg(DispLCDTypeT LcdType = LCD_MAIN);

  const DialogLayoutT* GetDialogLayout(void);

private:
  UiThemeC();

  void ResetMember(void);
  
private:
  static UiThemeC* _instance;

  const UiColorThemeT* mColorThemeP;

  const DialogLayoutT* mDialogLayoutP;
  
  ImageC* mDlgBgImgP;
  ImageC* mDlgBgImgSubP;
  ImageC* mMenuBgImgP;
};


inline const UiColorThemeT* UiThemeC::GetColorTheme(void)
{
  return mColorThemeP;
}

inline const DialogLayoutT* UiThemeC::GetDialogLayout(void)
{
  return mDialogLayoutP;
}

/*!
\brief Get the unique instance of the UiThemeC 

\retval the unique instance of UiThemeC
*/
inline UiThemeC* UiGetTheme(void)
{
  return UiThemeC::GetInstance();
}

inline const UiColorThemeT* UiGetColorTheme(void)
{
  return UiGetTheme()->GetColorTheme();
}




#endif
