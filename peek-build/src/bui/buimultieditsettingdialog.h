

#ifndef UIMULTIEDITSETTINGSDIALOG_H
#define UIMULTIEDITSETTINGSDIALOG_H

#include "buisettingsdialog.h"
#include "buidialog.h"
#include "buicontrol.h"

    
class MultiEditSettingDialogC : public DialogC
{
  friend class EditSettingDialogC;
public:
  MultiEditSettingDialogC();
  void LoadSettingInfo(SettingInfoT *SettingInfoP, MultiSettingInfoT *MultiSettingInfoP, SettingMsgT *SettingMsgP); 

/*! \cond private */
private:   
  void OnShowNotify(void);
  void OnMenuItemHovered(uint32 Msg);
  bool OnKeyPress(uint32 KeyCode);
  void OnOpenEditDialog();
  void OnCancel();
  void DrawStatus();
  bool CalculateOffset(bool IsNext);
  SettingInfoT* GetNextSettingInfo();


private:
  SettingInfoT *mCurrSettingInfoP;
  MultiSettingInfoT *mCurrMultiSettingInfoP;
  
  SettingInfoT mEditSettingInfo;
  SettingMsgT *mSettingMsgP;
  BStringC mTextP;
/*! \endcond */

  DECLARE_MAIL_MAP()
};

/*!
\brief  constructor functon

\retval none
*/
inline MultiEditSettingDialogC::MultiEditSettingDialogC()
{
  mCurrSettingInfoP = NULL;
  mCurrMultiSettingInfoP = NULL;
  mSettingMsgP = NULL;
  //mTextP = "";
  mTextP.Empty();
  mTextP += "";
}

/*!
\brief  initialize variables

\param SettingInfoP - SettingInfo pointer.It should not be null.
\param MultiSettingInfoP - MultiSettingInfo pointer.
\param SettingMsgP - SettingMsg pointer.
\retval none
*/
inline void MultiEditSettingDialogC::LoadSettingInfo(SettingInfoT *SettingInfoP, 
  MultiSettingInfoT *MultiSettingInfoP, SettingMsgT *SettingMsgP)
{
  mCurrSettingInfoP = SettingInfoP;
  mCurrMultiSettingInfoP = MultiSettingInfoP;
  memcpy(&mEditSettingInfo, mCurrSettingInfoP, sizeof(SettingInfoT));
  mSettingMsgP = SettingMsgP;
}



#endif
