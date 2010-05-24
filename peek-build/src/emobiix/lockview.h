

#ifndef LOCKVIEW_H_
#define LOCKVIEW_H_

#include "buiview.h"
#include "appuilifecycle.h"

#define MIN_SEC_LEN     5
#define MAX_SEC_LEN     10
#define PHONE_LOCK_LEN  4

//supeng 2007.12.20
#define MIN_PUK_LEN 8
#define MAX_PUK_LEN 34

#define PUK2_LEN 8

#define MAX_LOCK_LEN 10
#define MAX_LOCK_CODE_ERROR_TIMES  5
#define LOCK_CONSECUTIVE_ERROR_TIMER 300000

//! Pop up dialog id
typedef enum
{
  LOCK_POPUP_NULL,
  LOCK_POPUP_CODE_ERROR,
  LOCK_POPUP_CODE_LENGTH_ERROR,
  LOCK_POPUP_PIN_BLOCKED,
  LOCK_POPUP_PIN_ACCEPTED,
  LOCK_POPUP_PIN_CHANGED,
  LOCK_POPUP_PIN_UNBLOCKED,
  LOCK_POPUP_PIN_UNMATCH,
  LOCK_POPUP_WAITFOR_UIMID,
  LOCK_POPUP_WAITFOR_OPERATOR,
  LOCK_POPUP_WAITFOR_AREA
}LockPopupIdT;

enum
{
  E_SECURITY_POP_NONE,
  E_SECURITY_POP_PUK_NOT_MATCH,
  E_SECURITY_POP_PUK_INVALID,
  E_SECURITY_POP_PIN_INVALID,
  E_SECURITY_POP_PIN_CHANGED,
  E_SECURITY_POP_PIN2_CHANGED,
  E_SECURITY_POP_CODE_ERROR,
  E_SECURITY_POP_NOT_ALLOWED,
  E_SECURITY_POP_NOT_MATCH,
  E_SECURITY_POP_CODE_ACCEPTED,
  E_SECURITY_POP_CODE_CHANGED,
  E_SECURITY_POP_SECURITY_CODE_CHANGED,
  E_SECURITY_POP_SECURITY_CODE_SHORT,
  E_SECURITY_POP_SECURITY_CODE_CREATED,
  E_SECURITY_POP_SECURITY_CODE_INVALID,
  E_SECURITY_POP_EMERGENCY_ERROR,
  E_SECURITY_POP_CALL_CARRIER,
  E_SECURITY_POP_NO_UIM_QUERY,
  E_SECURITY_POP_ACTIVE_PIN,
  E_SECURITY_POP_MAX
};

/*! 
\brief Lock module view class

Description:
\n This class is responsible for lock view display:
\n 1. Dialongs including input pin, puk, dead lock...
\n 2. Popup warning dialog if needed(such as pin blocked, code does not match...)
*/
class LockViewC: public ViewC
{
public:
  LockViewC();
  virtual ~LockViewC();

  //DialogC *OpenLockDialog(UiLifecycleDlgIdT DialogId, uint8 LeftCount = 0, bool RightClose = TRUE);
  virtual bool CancelDialog(int DialogId);
  virtual bool UpdateDialog(int DialogId);
  //virtual bool UpdateDialog(int DialogId,int Times);

  bool GetEditContent(BStringC &String);
  void ClearEditContent();
  uint32 GetTopDlg(void);
  //bool OpenPopup(LockPopupIdT PopupId);
  bool OpenSetting(uint32 dwParam, bool bPinDim = FALSE, uint16 focus = 0);
  DialogC* OpenSecutiryDialog(uint32 dwDlgId, bool bFlag = FALSE);
  bool OpenSecutiryPop(uint32 dwDlgId);

private:
  //DialogC *GetEditDialog();
  
};



#endif


