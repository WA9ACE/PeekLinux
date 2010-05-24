

#ifndef UIMAILTIMER_H
#define UIMAILTIMER_H 

#include "sysdefs.h"
#include "exeapi.h"

#include "buidefs.h"
#include "buimsguser.h"
#include "builinkedlist.h"

#define UI_TIMER_HANDLER_DEBUG

#ifdef UI_TIMER_HANDLER_DEBUG
#define UI_TIMER_DEBUG_STR(Str)       MonPrintf((char*)"UITimerDebug: "Str)
#define UI_TIMER_DEBUG_VAR(Var)       MonPrintf((char*)#Var" = %d", (Var))
#define UI_TIMER_DEBUG_VAR_HEX(Var)   MonPrintf((char*)#Var" = %x", (Var))
#define UI_TIMER_DEBUG_VAR_STR(Str, Var)      MonPrintf((char*)Str#Var": %x", (Var))
#else
#define UI_TIMER_DEBUG_STR(Str)
#define UI_TIMER_DEBUG_VAR(Var)
#define UI_TIMER_DEBUG_VAR_HEX(Var)
#define UI_TIMER_DEBUG_VAR_STR(Str, Var)
#endif


enum PrecesionType
{
	PRECESION_100_MILLISECOND,
      PRECESION_ONE_SECOND,
      PRECESION_ONE_MINUTE
};

//! Mail Timer class define
class BTimerC
{
public:
  BTimerC();
  virtual ~BTimerC();

  void StartTimer(uint32 InitialTime, uint32 MailMsgId, WinHandleT WinHandle = WIN_HANDLE_NULL, uint32 ParamA = 0, int32 ParamB = 0, ParamTypeT ParamType = BY_VALUE);
  void StopTimer(void);

  bool GetTimerStarted(void) const;

  //neil this will be called in C code
  static void MailTimerCallBack(uint32 TimerId); 
private:

  void ResetTimerInfo(void);

private:
  ExeTimerT mExeTimer;
  bool mTimerStarted;
  uint32 mMailMsgId;
  WinHandleT mWinHandle;
  uint32 mParamA;
  int32 mParamB;
  ParamTypeT mParamType;  
};

/*!
\brief Get the timer status

\retval TRUE if timer is started
*/  
inline bool BTimerC::GetTimerStarted(void) const
{
  return mTimerStarted;
}


typedef struct
{
  MailClientC *MailClientP;
  MailMsgT MailMsg;
  uint32 Duration;
  uint32 TimeCount;
  uint32 TimerType;
  uint32 TimerID;
}TimerMsgT;

class BTimerManagerC: public MailClientC
{
public:
  virtual ~BTimerManagerC();

  static BTimerManagerC* GetInstance(void);

  uint32 RegisterForMail(TimerMsgT *TimerMsgP);
  void UnregisterForMail(PrecesionType TimerType, uint32 TimerID);

private:
  BTimerManagerC();
   void MilliSecondTimeOut();
   void SecondTimeOut();
   void MinuteTimeOut();
   void OnTimeout(uint32 TimerType);
   void GetCalibrationTime(uint32 TimerType);
private:
	static BTimerManagerC* _instance;

	uint32 TimerID;
	uint32 SecondTimerID;
	uint32 MinuteTimerID;

	// 0.1 second precesion
	BLinkedListC mRegisteredClients;
	// 1 second precesion
	BLinkedListC mSecRegisterClients;
	// 1 minute precesion
	BLinkedListC mMinRegisterclients;

	uint32 MilliTimerLastTime;
	uint32 SecondTimerLastTime;
	uint32 MinuteTimerLastTime;

	uint32 MilliTimerDuration;
	uint32 SecondTimerDuration;
	uint32 MinuteTimerDuration;
	BTimerC mTimer;
	BTimerC mSecondTimer;
	BTimerC mMinuteTimer;

	DECLARE_MAIL_MAP()
};

//! Get the unique instance of the BTimerManagerC
inline BTimerManagerC* UiGetTimerManager(void)
{
  return BTimerManagerC::GetInstance();
}



#endif
