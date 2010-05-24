

#ifndef UICONTROL_H
#define UICONTROL_H 

#include "sysdefs.h"

#include "buiwindow.h"


//! Control type defines
typedef enum
{
  CTRL_TYPE_NONE = 0,
  CTRL_TYPE_IMAGE,
  CTRL_TYPE_ANIMATION,
  CTRL_TYPE_STATICTEXT,
  CTRL_TYPE_EDIT,
  CTRL_TYPE_SOFTKEYBAR,
  CTRL_TYPE_MENU,
  CTRL_TYPE_BUTTON,
  CTRL_TYPE_DATETIME,
  CTRL_TYPE_SCROLLBAR,
  CTRL_TYPE_ANNOUNCIATOR,
  CTRL_TYPE_ANIMATION_GROUP,
  CTRL_TYPE_DATE,
  CTRL_TYPE_WEEK,
  CTRL_TYPE_ADDRESSEDIT,
  CTRL_TYPE_MENUEX = 15,///15 Add by BYD
  CTRL_TYPE_MENUEX_HUGE=16,
  CTRL_TYPE_CONTAINER,
  CTRL_TYPE_TITLEBAR,
  CTRL_TYPE_COMBOBOX,//added by hebo 2008-03-26
  CTRL_TYPE_CHECKBOX,//added by hebo 2008-04-29
  CTRL_TYPE_RADIOBOX, // added by wanglili 20080529
  CTRL_TYPE_RADIOBUTTON,
  CTRL_TYPE_SYMBOLS, // added by lizhongyi 20080731
  CTRL_TYPE_MAX
} CtrlTypeT;

//! Control class define
class ControlC : public WindowC
{
public:
  ControlC(CtrlTypeT ControlType);
  virtual ~ControlC();

  void SetControlType(CtrlTypeT type) {mControlType = type;};//Add by BYD
  CtrlTypeT GetControlType() const;

protected:
  WindowC* CreateControl(CtrlTypeT Type);
	
private:
  CtrlTypeT mControlType;

  DECLARE_MAIL_MAP()
};

//! Get the control type
inline CtrlTypeT ControlC::GetControlType() const
{
  return mControlType;
}




#endif

