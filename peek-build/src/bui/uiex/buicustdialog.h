
#include "buidialog.h"
#include "buimenuexmodel.h"
#include "buimenuexctrl.h"
#include "buistatictextdata.h"
#include "buidialogutils.h"
#include "buiime.h"


#ifndef UICUSTDIALOG_H
#define UICUSTDIALOG_H
//! Dialog class define

#define TEXT_ZONE_EDIT CTRL_TYPE_EDIT
#define TEXT_ZONE_STATIC CTRL_TYPE_STATICTEXT

typedef enum
{
  CZONE_NONE,
  CZONE_PROGRESS,
  CZONE_MAX
}DlgCZoneT;


class CustDialogC : public DialogC
{
public:
  CustDialogC(DlgCStyleT style = (DlgCStyleT)0);
  virtual ~CustDialogC();

  bool CreateEx(void* ExtraDataP,uint32 WinId = 0, uint32 Style = 0, DispLCDTypeT LcdType = LCD_MAIN);
  bool SetText(const BStringC& text,CtrlTypeT ctrlType = CTRL_TYPE_EDIT,int index = 0);
  bool SetText(RESOURCE_ID(StringT) StringResId,CtrlTypeT ctrlType = CTRL_TYPE_EDIT,int index = 0);
  bool GetText(BStringC& str,CtrlTypeT ctrlType = CTRL_TYPE_EDIT, int index = 0);
  bool SetInputMethod(uint32 nImeMode);
  bool SetMaxLimit(uint32 nCount);
  bool SetValue(DlgCZoneT ZoneId,uint32 nValue);
  void SetIsHelpText(bool Help);//for help text
  bool GetIsHelpText(void){return bHelpText;}//for help text
  void SetTimeOut(uint32 nTimeout);//for help text
  
protected:
  bool OnKeyPress(uint32 KeyCode);
  void OnCloseHelpText();

private:
  BTimerC mClearTimer;//for help text
  bool bHelpText;//for help text
  DlgCStyleT mDlgCStyle;
  
  DECLARE_MAIL_MAP()
};


#endif

