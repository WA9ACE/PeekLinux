

#ifndef UIAPPMANAGER_H
#define UIAPPMANAGER_H 


#include "sysdefs.h"

#include "buimsguser.h"
#include "buivector.h"
#include "buiapp.h"


//! AppManager class define
class BAppManagerC : public MailClientC
{
public:
  virtual ~BAppManagerC();
  static BAppManagerC* GetInstance(void);
  
  virtual int32 ProcessMail(uint32 MailMsgId, WinHandleT WinHandle, uint32 ParamA, uint32 ParamB);

  void AddApp(BApplicationC* ApplicationP);
  BApplicationC* RemoveApp(AppIdT AppId);
  BApplicationC* GetCurrentApp();
  void RemoveAllApps(void);

  int32 StartApp(uint8 AppId);
  int32 SuspendApp(uint8 AppId);
  int32 ResumeApp(uint8 AppId);
  int32 StopApp(uint8 AppId);
  int32 SoundEvent(uint8 AppId, uint32 eventType);
  int32 EarphoneEvent(uint8 AppId, uint32 eventType);
  int32 ConflictEvent(uint8 AppId, uint32 eventType);
  int32  SendBatteryEvent(uint32 ucMessage);//zhangfanghui
  int32  SendChargerEvent(uint32 ucMessage);//zhangfanghui
  int32 SendrssiEvent(uint32 ucMessage);//zhangfanghui
  int32 TurnOnLight(uint8 AppId, uint32 eventType);
private:
  BAppManagerC();

private:
  static BAppManagerC* _instance;
  
  BApplicationC* mAppArray[APPID_MAX];
};


//! Get the unique instance of the BAppManagerC
inline BAppManagerC* UiGetAppManager(void)
{
  return BAppManagerC::GetInstance();
}




#endif

