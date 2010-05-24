

#ifndef UIAPP_H
#define UIAPP_H 

#include "buimsguser.h"
#include "builinkedlist.h"
#include "buimsgdefs.h"
#include "buimsgids.h"
#include "buiview.h"
#include "buimodel.h"
#include "buivector.h"

class BApplicationC;

typedef void (BApplicationC::*AppMailHandlerT)(void);


//! Application state define
typedef enum
{
  APP_STATE_INITIAL, //!< initial state
  APP_STATE_START, //!< start state
  APP_STATE_ACTIVE, //!< active state
  APP_STATE_SUSPEND, //!< suspend state
  APP_STATE_STOP //!< stop state
} AppStateT;

//! Map APPMSG_START to OnStart
#define ON_APPMSG_START(ThisClass) \
 { APPMSG_START, APPMSG_START, MHType_l_v, \
    (MailHandlerT)(AppMailHandlerT)(int32 (BApplicationC::*)(void))&ThisClass::OnStart },

//! Map APPMSG_SUSPEND to OnSuspend 
#define ON_APPMSG_SUSPEND(ThisClass) \
 { APPMSG_SUSPEND, APPMSG_SUSPEND, MHType_l_v, \
    (MailHandlerT)(AppMailHandlerT)(int32 (BApplicationC::*)(void))&ThisClass::OnSuspend },

//! Map APPMSG_RESUME to OnResume 
#define ON_APPMSG_RESUME(ThisClass) \
 { APPMSG_RESUME, APPMSG_RESUME, MHType_l_v, \
    (MailHandlerT)(AppMailHandlerT)(int32 (BApplicationC::*)(void))&ThisClass::OnResume },

//! Map APPMSG_STOP to OnStop 
#define ON_APPMSG_STOP(ThisClass) \
 { APPMSG_STOP, APPMSG_STOP, MHType_l_v, \
    (MailHandlerT)(AppMailHandlerT)(int32 (BApplicationC::*)(void))&ThisClass::OnStop },

//! Map APPMSG_POPUP_OPTIONS to OnPopupOptions
#define ON_APPMSG_POPUPOPTIONS(ThisClass) \
 { APPMSG_POPUP_OPTIONS, APPMSG_POPUP_OPTIONS, MHType_v_v, \
    (MailHandlerT)(AppMailHandlerT)(void (BApplicationC::*)(void))&ThisClass::OnPopupOptions },

//! Map APPMSG_SET_VALUE to OnSetValue 
#define ON_APPMSG_SETVALUE(ThisClass) \
 { APPMSG_SET_VALUE, APPMSG_SET_VALUE, MHType_v_w, \
    (MailHandlerT)(AppMailHandlerT)(void (BApplicationC::*)(uint32))&ThisClass::OnSetValue },

//! Map APPMSG_SOUND_EVENT to OnSoundEvent 
#define ON_APPMSG_SOUND(ThisClass) \
 { APPMSG_SOUND_EVENT, APPMSG_SOUND_EVENT, MHType_v_w, \
    (MailHandlerT)(AppMailHandlerT)(void (BApplicationC::*)(uint32))&ThisClass::OnSoundEvent },

//! Map APPMSG_EARPHONE_EVENT to OnSoundEvent 
#define ON_APPMSG_EARPHONE(ThisClass) \
 { APPMSG_EARPHONE_EVENT, APPMSG_EARPHONE_EVENT, MHType_v_w, \
    (MailHandlerT)(AppMailHandlerT)(void (BApplicationC::*)(uint32))&ThisClass::OnEarphoneEvent },

//! Map APPMSG_SD_CARD_EVENT to OnSoundEvent
#define ON_APPMSG_STORAGE_EVENT(ThisClass) \
 { APPMSG_STORAGE_EVENT, APPMSG_STORAGE_EVENT, MHType_v_v, \
    (MailHandlerT)(AppMailHandlerT)(void (BApplicationC::*)(void))&ThisClass::OnStorageEvent },

//! Map APPMSG_SOUND_EVENT to OnSoundEvent 
#define ON_APPMSG_CONFLICT(ThisClass) \
 { APPMSG_CONFLICT_EVENT, APPMSG_CONFLICT_EVENT, MHType_v_w, \
    (MailHandlerT)(AppMailHandlerT)(void (BApplicationC::*)(uint32))&ThisClass::OnConflictEvent },


//! Application class define
class BApplicationC : public MailClientC
{
public:
  BApplicationC();
  virtual ~BApplicationC();
  virtual AppIdT GetAppId() = 0;

  virtual void HandleKey(uint32 KeyMsgId, uint32 KeyCode);
  virtual void HandleWinNotify(DialogC* DialogP, uint32 ParamA, int32 ParamB);
  virtual void OnCreateDialog(DialogC* DialogP);
  virtual void OnSetData(DialogC* DialogP);
  virtual void OnSetMark(DialogC* DialogP,int32 ItemIndex,bool bMark);
  virtual void OnDestroyDialog(DialogC* DialogP);
  virtual void OnShowDialog(DialogC* DialogP);
  virtual void OnHideDialog(DialogC* DialogP);
  virtual void OnScrollItem(DialogC* DialogP,uint16 nCurIndex,uint32 nReason);
  virtual void OnOptionSelected(DialogC* DialogP,uint32 dwParam);//For Normal Option Selected
  virtual void OnOptionSelected(DialogC* DialogP,uint16 wParam,uint32 dwFlags = 0);//For Flags Option Selected
  virtual void OnColorSelected(BalDispColorT color);
  virtual void OnSetReorder(DialogC* DialogP){DialogP = DialogP;}
  virtual void OnNotifyWinEvent(DialogC *DialogP,WinEventIdT WinEventId,uint32 dwParam = 0);
  virtual void OnNotifyBatteryEvent(uint8 ucMessage);//zhangfanghui
  virtual void OnNotifyChargerEvent(uint8 ucMessage);//zhangfanghui
  virtual void OnNotifyRssiEvent(uint8 ucMessage);//zhangfanghui
  virtual void OnNotifySleepEvent(bool isSleep);//zouyonghang
  bool AddModel(ModelC *ModelP);  
  bool RemoveModel(ModelC *ModelP);
  void RemoveAllModels(void);

  AppStateT GetAppState(void);
  void SetAppState(AppStateT State);

  void StackInvoke(bool bSuspend = FALSE, bool bRemove = FALSE);
  
protected:
  BVectorC mModelArray; //! array of associated model
  
  int32 OnStart();
  int32 OnSuspend();
  int32 OnResume();
  int32 OnStop();
  
  void OnPopupOptions(void);
  void OnSetValue(uint32 Msg);
  void OnSoundEvent(uint32 eventType);
  void OnEarphoneEvent(uint32 eventType);
  void OnStorageEvent(void);
  
  void SetModelView(ModelC *ModelP,BApplicationC *AppP);

  virtual void StackPush(void** ppUserData);  // notify application to push its data to stack as soon as enter the application from external
  virtual void StackPop(void* pUserData);     // notify application to pop and free its data when back from other application
  virtual void StackDataSyn(void* pUserData); // notify application to synchronize data before resume if user has ever re-retered the application

private:
  void DeleteModelView(ModelC *ModelP);

  AppStateT mState;
  BVectorC   mStack;           // stack for re-entry, to store user data.
  int       mnEverStackDepth;  // stack size ever get

  DECLARE_MAIL_MAP()
};


//! Get the application's state
inline AppStateT BApplicationC::GetAppState(void)
{
  return mState;
}

//! Set the application's state
inline void BApplicationC::SetAppState(AppStateT State)
{
  mState = State;
}

//! Set the application's Selected
inline void BApplicationC::OnOptionSelected(DialogC* DialogP,uint32 dwParam)
{
  DialogP = DialogP;
  dwParam = dwParam;
}

//! Set the application's Selected
inline void BApplicationC::OnOptionSelected(DialogC* DialogP,uint16 wParam,uint32 dwFlags)
{
  DialogP = DialogP;
  wParam = wParam;
  dwFlags = dwFlags;
}
//! Set the WinEvent Notify
inline void BApplicationC::OnNotifyWinEvent(DialogC *DialogP,WinEventIdT WinEventId,uint32 dwParam)
{
  DialogP = DialogP;
  WinEventId = WinEventId;
  dwParam = dwParam;
}



#endif

