

#ifndef UISCHEDULER_H
#define UISCHEDULER_H 

#include "buiappointment.h"

#include "baldb.h"

#define UI_SCHEDULER_FILE_NAME    "calendarfile"
#define UI_DURATION_FILE_NAME     "durationfile"

#define UI_SCHE_MAX_REG       1
#define UI_SCHE_INVALID_REGID -1

#define UI_SCHEDULER_INVALID_RECID    0xFFFF
#define UI_SCHEDULER_MAX_REC_CNT      100

//! Calendar delete notes type
typedef enum
{
  UI_SCHE_DELETE_ALL = 0,
  UI_SCHE_DELETE_UNTIL_TODAY,
  UI_SCHE_DELETE_PERIOD,
  UI_SCHE_DELETE_TYPE_NUM
}UiScheDeleteTypeT;

//! Ui scheduler event define
typedef enum
{
  UI_SCHE_EVENT_UPDATE_PREVIEW_DATA    = 0x00000001,
  UI_SCHE_EVENT_INIT_COMPLETE,
  UI_SCHE_EVENT_ALERT_APPT
}UiScheEventIdT;

//!< Ui scheduler callback function define
typedef void (*UiScheEventFunc) (UiScheEventIdT EventId, uint32 Msg);

//! Ui sms register table define
typedef struct
{
  bool            IsUse;            //!< if this entry is in use
  UiScheEventFunc CallBack;         //!< Function to call back 
}UiScheRegTableT;

typedef int16 UiScheRegIdT;             //!< Scheduler register id define

class SchedulerC : public MailClientC
{
public:
  static SchedulerC* GetInstance();
  virtual ~SchedulerC();   

  UiScheRegIdT Register(UiScheEventFunc CallBack);
  void UnRegister(UiScheRegIdT RegId); 

  void InitAppt();
  void PrepareAllAppt();
  void AwakeAllAppt();  
  bool IsScheInitComplete();
  
  UiScheRecResultT AddRecord(const UiAppointmentRecordT & AppointmentRecord);
  UiScheRecResultT UpdateRecord(uint16 RecId, const UiAppointmentRecordT & AppointmentRecord); 
  UiScheRecResultT DeleteRecord(uint16 RecId);  
  UiScheRecResultT DeleteGroupRecords(UiScheDeleteTypeT DeleteType, DateT StartDate, DateT EndDate);
  UiScheRecResultT DeleteNodeAtDate(uint16 RecId, DateT Date);
  UiScheRecResultT DeleteNodesInDur(uint16 RecId, DateT StartDate, DateT EndDate);
  UiScheRecResultT UpdateRecordState(uint16 RecId, UiAppointmentStateT State);
  
  uint16 GetCount();
  BLinkedListC* GetEventList();
  uint16 GetRecIdAtFocusDate(uint16 Index);  
  uint16 GetCountAtFocusDate();  
  bool GetAppointmentInfo(uint16 RecId, UiAppointmentInfoT & AppointmentInfo);
  bool GetAppointmentRecord(uint16 RecId, UiAppointmentRecordT & AppointmentRecord);
  AppointmentC* GetAppointment(uint16 RecId);
  
  void InitEventList(DateT FocusDate, DateTypeT DateType, int8 StartWeek);
  void ChangeFocusDate(DateT FocusDate);
  void ChangeScreen(DateT FocusDate);  
  void StopRemind(uint16 RecId);
  
private:
  void OnPrepareAllAppt();
  void OnAwakeAllAppt();
  void OnAlertAppt(uint32 Msg);
  
  UiScheRecResultT Open();
  UiScheRecResultT Close();   
  uint16 GetFreeRecId();  

  void CleanEventList();
  void AddEventList(uint32 EventFlag, AppointmentC* AppointmentP);
  
  void UpdatePreviewData(bool SendMail = TRUE);
  void UpdateAppointmentList();
  
  void AddAppointment(AppointmentC * AppointmentP);
  void RemoveAppointment(AppointmentC * AppointmentP);
 
private:
  SchedulerC();
  static SchedulerC* _instance;
  static uint16 SchedulerDbHandle;

  UiScheRegTableT mRegTable[UI_SCHE_MAX_REG];
  bool mInitComplete;  
  
  DateTypeT mDateType;
  DateT mFocusDate;
  int8 mStartWeek;
  BLinkedListC mEventList[31];//store appointments list which has event in view mode

  BLinkedListC mAppointmentSet;//store all apponintments
  BLinkedListC* mAppointmentListP;//store the focus date's appointments

  DECLARE_MAIL_MAP()
};

inline uint16 SchedulerC::GetCount()
{
  return mAppointmentSet.GetCount();
}

inline BLinkedListC* SchedulerC::GetEventList()
{
  return mEventList;
}

inline uint16 SchedulerC::GetCountAtFocusDate()
{
  return mAppointmentListP->GetCount();
}

inline bool SchedulerC::IsScheInitComplete()
{
  return mInitComplete;
}

inline SchedulerC* UiGetScheduler(void)
{
  return SchedulerC::GetInstance();
}

#endif


