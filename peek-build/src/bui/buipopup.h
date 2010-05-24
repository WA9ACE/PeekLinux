

#ifndef UIPOPUP_H
#define UIPOPUP_H 

#include "buidialog.h"
#include "buimsgtimer.h"

//! Define the popup dialog id 
#define DLGID_POPUP 0xFFFF0000
#define PopupC DialogC
//! Popup dialog type define
typedef enum
{
  POPUP_WARNING,  //!< warning type
  POPUP_OKAY,     //!< Ok type
  POPUP_INFO,     //!< Information type, not display animation
  POPUP_MAX 
} PopupTypeT;

#if 0
//! Popup class define
class PopupC : public DialogC
{
public:
  PopupC(uint32 ClearTime);
  virtual ~PopupC();

  void SetCloseUpdate(bool Update);

private:
  BTimerC mClearTimer;

  bool mCloseUpdate;
  
  void OnClosePopup(void);
  bool OnKeyPress(uint32 KeyCode);

  DECLARE_MAIL_MAP()
};



/*!
\brief Set popup dialog close update attribute

\param Update -[in] TRUE if want update the popup after it close, else FALSE
\retval none
*/
inline void PopupC::SetCloseUpdate(bool Update)
{
  mCloseUpdate = Update;
}

#endif


PopupC* UiOpenPopup(RESOURCE_ID(StringT)StringResId, void* ExtraDataP, uint32 ClearTime, PopupTypeT PopupType, DispLCDTypeT LcdType = LCD_MAIN);

PopupC* UiOpenPopup(const BStringC& String, void * ExtraDataP, uint32 ClearTime, PopupTypeT PopupType, DispLCDTypeT LcdType = LCD_MAIN);




#endif

