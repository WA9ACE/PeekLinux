

#ifndef UIEDITSETTINGDIALOG_H
#define UIEDITSETTINGDIALOG_H

#include "buidialog.h"
#include "buicontrol.h"
#include "buisettingsdialog.h"
#include "buimultieditsettingdialog.h"

class EditSettingDialogC : public DialogC
{
public:
	EditSettingDialogC();
  void LoadSettingInfo(SettingInfoT *SettingInfoP, SettingMsgT *SettingMsgP);
  
/*! \cond private */  
  void SetEditTitle(BStringC &Title);
	void SetEditTitle(RESOURCE_ID(StringT) StringResId);
	void SetMultiEditDlg(MultiEditSettingDialogC *MultiEditDlg);

private:
	int32 OnCreate();
	void OnShowNotify();
	void OnDestroy();
	void OnHideNotify();
	bool OnKeyPress(uint32 KeyCode);
	void OnSaveEdit();
	void SaveSetting();
	void OnNextEdit();
	void OnCancel();

private:
	SettingInfoT *mCurrSettingInfoP;
	SettingMsgT *mSettingMsgP;
	bool mSavedFlag;
	bool mSaveResult;
	bool mValueValid;
	MultiEditSettingDialogC *mMultiEditDlgP;
/*! \endcond */

DECLARE_MAIL_MAP()
};


/*!
\brief  constructor functon

\retval none
*/
inline EditSettingDialogC::EditSettingDialogC():mSavedFlag(FALSE)
	, mSaveResult(FALSE), mValueValid(FALSE), mMultiEditDlgP(NULL)
{	
  mCurrSettingInfoP = NULL;
}

/*!
\brief  initialize variables

\param SettingInfoP - SettingInfo pointer.It should not be null.
\param SettingMsgP - SettingMsg pointer.
\retval none
*/
inline void EditSettingDialogC::LoadSettingInfo(SettingInfoT *SettingInfoP, SettingMsgT *SettingMsgP) 
{
	mCurrSettingInfoP = SettingInfoP;
	mSettingMsgP = SettingMsgP;
}

inline void EditSettingDialogC::SetMultiEditDlg(MultiEditSettingDialogC *MultiEditDlg)
{
	mMultiEditDlgP = MultiEditDlg;
}




#endif

