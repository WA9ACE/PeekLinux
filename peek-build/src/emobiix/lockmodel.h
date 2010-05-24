

#ifndef LOCKMODEL_H_
#define LOCKMODEL_H_

#ifdef SYS_OPTION_RUIM
#include "baluimapi.h"
#endif

#include "buikeymap.h"
#include "buimodel.h"

#include "asluilifecycle.h"

#include "uilifecycleutils.h"
#include "lockview.h"

/*! 
\brief Lock model class

Description:
\n This class is responsible for all kinks of lock function:
\n 1. Uim verify(including pin management, uim check...)
\n 2. Operator verify
\n 3. Cell verify
\n 4. Stored data for lock view using
\n 5. Update lock view display
*/
class LockModelC: public ModelC
{
public:
  LockModelC();
  virtual ~LockModelC();
  
  bool HandleKey(uint32 KeyMsgId = KEYMSG_PRESS, uint32 KeyCode = KP_NO_KEY);
  void SetStoredData(UiLifecycleStoredLockT* pstSecurity);
  bool ProcessMail(uint32 MsgId, void *MsgP);  
#ifdef SYS_OPTION_RUIM
  BalChvStatusT GetUimStatus();
#endif  
  bool GetFromSetting();
  void ProcessInterSetting(uint32 dwParam);
  void SetSecurityData(UiLifecycleStoredNormalT* pstNorm);
  void CheckSecurityCode(uint32 dwMsgId, uint32 dwParam);
  void CheckLockCode(uint32 dwMsgId, uint32 dwParam);

private:  
  int m_nTimes;
  bool m_bNamFlag;
  bool m_bFromSetting;
  bool m_bChangePIN2;
  UiLifecycleStoredLockT* m_pstSecurity;
  UiLifecycleStoredNormalT* m_pstNorm;
  BStringC m_strPwd;
  uint32 m_dwInterParam; // record the dialog id before entering password
  uint32 m_dwMsgId;
  uint32 m_dwParam;
  //bool   m_bLockTimer;  
  int    m_nLockInputTimes; // number of consecutive lock code input 
  BTimerC m_LockTimer; // 5 min timer for consecutive incorrect lock code input
#ifdef SYS_OPTION_RUIM
  char m_Pin[MAX_PIN_LEN+1]; // current PIN
  char m_NewPin[MAX_PIN_LEN+1]; // change pin
  char m_Puk[MIN_PUK_LEN+1];
  BalChvStatusT m_stRuimStatus;

  void PinPass(void);
  bool CheckRuim(void);
#endif  

private:
  bool CheckPin(BStringC& strText);
  bool CheckPuk(BStringC& strText);
  bool CheckOperator(void);
  bool CheckCell(void);
  void AfterCheckOperator();
  void SecuritySetting(BStringC& strText);
};

inline void LockModelC::SetStoredData(UiLifecycleStoredLockT* pstSecurity)
{
  m_pstSecurity = pstSecurity;
}

#ifdef SYS_OPTION_RUIM
inline BalChvStatusT LockModelC::GetUimStatus()
{
  return m_stRuimStatus;
}
#endif

inline bool LockModelC::GetFromSetting()
{
  return m_bFromSetting;
}

inline void LockModelC::SetSecurityData(UiLifecycleStoredNormalT* pstNorm)
{
  m_pstNorm = pstNorm;
}




#endif


