

#ifndef UIMENUSETTINGDIALOG_H
#define UIMENUSETTINGDIALOG_H

#include "buidialog.h"
#include "buicontrol.h"
#include "buisettingsdialog.h"


class MenuSettingDialogC : public DialogC
{
public:
  MenuSettingDialogC();
  void LoadSettingInfo(SettingInfoT *SettingInfoP, SettingMsgT *SettingMsgP);
/*! \cond private */
  void SetMenuTitle(RESOURCE_ID(StringT) StringResId);
private:
  int32 OnCreate();
  void OnShowNotify();
  void OnHideNotify();
  void OnDestroy(void);
  void OnMenuItemHovered(uint32 index);
  bool OnKeyPress(uint32 KeyCode);
  void OnOptionSelected();
  void OnCancel();

private:
  SettingInfoT *mCurrSettingInfoP;
	SettingMsgT *mSettingMsgP;
  bool mSavedFlag;
  void *mOriginalValueP;
  void *mModifiedValueP;	
/*! \endcond */

  DECLARE_MAIL_MAP()
};

/*!
\brief  constructor functon

\retval none
*/
inline MenuSettingDialogC::MenuSettingDialogC():mSavedFlag(FALSE)
	, mOriginalValueP(NULL), mModifiedValueP(NULL)
	
{ 
	mCurrSettingInfoP = NULL;
}

/*!
\brief  initialize variables

\param SettingInfoP - SettingInfo pointer.It should not be null.
\param SettingMsgP - SettingMsg pointer.
\retval none
*/
inline void MenuSettingDialogC::LoadSettingInfo(SettingInfoT *SettingInfoP, SettingMsgT *SettingMsgP) 
{
  mCurrSettingInfoP = SettingInfoP;
	mSettingMsgP = SettingMsgP;
}





#endif
