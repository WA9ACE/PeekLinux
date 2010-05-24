

#ifndef UIWINMANAGER_H
#define UIWINMANAGER_H 

#include "baldispapi.h"

#include "buidefs.h"
#include "builinkedlist.h"
#include "buidialog.h"
#include "buirect.h"
#include "buiapp.h"


//! WinManger class define
class WinManagerC : public MailClientC
{
public:
  virtual ~WinManagerC();
  static WinManagerC* GetInstance(void);

  virtual int32 ProcessMail(uint32 MailMsgId, WinHandleT WinHandle, uint32 ParamA, uint32 ParamB);

  void UpdateAllWindows(bool Synchronous, DispLCDTypeT LcdType = LCD_MAIN);
  void InvalidateRect(LPCRectT RectP, bool EraseBg = TRUE, DispLCDTypeT LcdType = LCD_MAIN);
  bool GetUpdateRect(LPRectT RectP, DispLCDTypeT LcdType = LCD_MAIN);

  DialogC* GetTopDialog(DispLCDTypeT LcdType = LCD_MAIN);
  DialogC* FindDialog(AppIdT AppId, uint32 WinId);
  void  SetTopDialog(AppIdT AppId, uint32 WinId);
  
  bool HasAppDialog(BApplicationC* AppP);
  
  void CloseAllDialogs(bool SendCloseMail = FALSE);
  DialogC* GetAppDialog(BApplicationC* AppP, uint16 Index = 0, DispLCDTypeT LcdType = LCD_MAIN);
  void ActiveAppDialogs(BApplicationC* AppP);
  void CloseAppDialogs(BApplicationC* AppP, bool SendCloseMail = FALSE);
  void CloseDialogsUntilDlgId(uint32 dlgId,bool SendCloseMail = FALSE,bool bCloseCur = FALSE);
  void CloseDialogsUntilDlgResId(RESOURCE_ID(DialogResT) dlgResId,bool SendCloseMail = FALSE,bool bCloseCur = FALSE);

private:
  WinManagerC();
  DispLCDTypeT GetLcdType(WinHandleT WinHandle);

private:
  static WinManagerC* _instance;
  BLinkedListC mDialogList[LCD_TOTAL];
  RectC mInvalidRect[LCD_TOTAL];
  bool mEraseBkgnd;
  bool mDisablePaint[LCD_TOTAL];
  DECLARE_MAIL_MAP()
};


/*!
\brief Get the topmost dialog for the lcd of LcdType 

\param LcdType -[in] lcd type
\retval the top dialog
*/ 
inline DialogC* WinManagerC::GetTopDialog(DispLCDTypeT LcdType)
{
  return (DialogC*)(mDialogList[LcdType].GetHead());
}

/*!
\brief Get the unique instance of the WinManagerC 

\retval the unique instance of WinManagerC
*/ 
inline WinManagerC* UiGetWinManager(void)
{
  return WinManagerC::GetInstance();
}




#endif

