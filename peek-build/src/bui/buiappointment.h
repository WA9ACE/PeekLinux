

#ifndef UIAPPOINTMENT_H
#define UIAPPOINTMENT_H 

#include "sysdefs.h"
#include "buidatectrl.h"
#include "buiapp.h"

#include "baldb.h"

//#define SCHEDULER_DEBUG

#ifdef SCHEDULER_DEBUG
#define SCHEDULER_TRACE_STR(Str)              MonPrintf((char*)(Str))
#define SCHEDULER_TRACE_VAR(Str, Var)         MonPrintf((char*)(Str), (Var))
#define SCHEDULER_TRACE_VAR2(Str, Var1, Var2) MonPrintf((char*)(Str), (Var1), (Var2))
#else
#define SCHEDULER_TRACE_STR(Str)
#define SCHEDULER_TRACE_VAR(Str, Var)
#define SCHEDULER_TRACE_VAR2(Str, Var1, Var2)
#endif

#define UI_APPT_SUBJECT

//indicate that the type small than NORMAL_TYPE_START_FLAG should display specially
#define NORMAL_TYPE_START_FLAG  UI_APPOINTMENT_MEETING 

//for duration record
#define UI_DURATION_INVALID_RECID    0xFFFF
#define UI_DURATION_MAX_REC_CNT      500

typedef enum
{
  UI_RECORD_RESULT_SUCCESS,
  UI_RECORD_RESULT_FAIL,
  UI_RECORD_RESULT_NULL
}UiScheRecResultT;

typedef enum
{
  UI_APPOINTMENT_FREE = 0x00 ,
  UI_APPOINTMENT_INIT,
  UI_APPOINTMENT_SLEEP,
  UI_APPOINTMENT_VALID,
  UI_APPOINTMENT_MINDED,     //has alerted and stoped by user
  UI_APPOINTMENT_EXPIRED
}UiAppointmentStateT;

typedef enum
{
	UI_APPT_REC_TYPE_DAILY = 0,
	UI_APPT_REC_TYPE_WEEKLY,
	UI_APPT_REC_TYPE_MONTHLY,
	UI_APPT_REC_TYPE_YEARLY
}UiApptRecTypeT;

typedef uint8 UiApptRecSubTypeT;
#define UI_APPT_REC_SUBTYPE_BYCOUNT 0
#define UI_APPT_REC_SUBTYPE_BYWEEKDAY  1
#define UI_APPT_REC_SUBTYPE_BYWEEK 0
#define UI_APPT_REC_SUBTYPE_BYMONTHDAY 0
#define UI_APPT_REC_SUBTYPE_BYMONTHWEEK 1
#define UI_APPT_REC_SUBTYPE_ONMONTHDAY 0
#define UI_APPT_REC_SUBTYPE_ONMONTHWEEK 1

typedef enum
{
  UI_APPT_REC_FIRST_WEEK = 0,
	UI_APPT_REC_SECOND_WEEK,
	UI_APPT_REC_THIRD_WEEK,
	UI_APPT_REC_FOURTH_WEEK,
	UI_APPT_REC_LAST_WEEK
}UiApptWeekSerialT;

typedef struct
{
	UiApptRecTypeT Type;
	UiApptRecSubTypeT SubType;
	uint16 Interval;  
	uint8 Serial;
	uint8 Value;
}UiApptRecurrenceT;

typedef enum
{
	UI_APPT_REC_ONLY_ONCE,
	UI_APPT_REC_PERIOD,
	UI_APPT_REC_FOREVER
}UiRecModeT;

typedef enum
{
	UI_APPT_ALARM_OFF,
	UI_APPT_ALARM_SILENT,
	UI_APPT_ALARM_ALERT
}UiApptAlarmTypeT;

//temp for compile
typedef enum
{
	UI_APPT_REMINDER_15_MINUTES = 15,
	UI_APPT_REMINDER_10_MINUTES = 10,
	UI_APPT_REMINDER_5_MINUTES = 5,
	UI_APPT_REMINDER_0_MINUTE = 0
}UiApptReminderT;

typedef enum
{
	EST, 
	EDT, 
	CST, 
	CDT, 
	MST, 
	MDT, 
	PST, 
	PDT, 
  AKST, 
  AKDT, 
  HAST, 
  HADT, 
  UTC/*GMT*/
}TimeZoneT;

const int UI_APPT_SUBJECT_MAX_LENGTH = 256;
typedef PACKED struct 
{
  uint8 CodePage;
  uint8 StringLen;
  uint8 String[UI_APPT_SUBJECT_MAX_LENGTH];
}UiApptSubjectT;

const int UI_APPT_LOCATION_MAX_LENGTH = 32;
typedef PACKED struct 
{
  uint8 CodePage;
  uint8 StringLen;
  uint8 String[UI_APPT_LOCATION_MAX_LENGTH];
}UiApptLocationT;

typedef struct 
{  
  UiAppointmentTypeT ApptType;	
	DateT  StartDate;
	HourMinuteT StartTime;
	DateT  EndDate;
	HourMinuteT EndTime;	
	UiRecModeT RecMode;
	UiApptRecurrenceT Recurrence;	
	DateT RecEndDate;	
	//TimeZoneT TimeZone;
	UiApptAlarmTypeT AlarmType;
	//uint32 AlarmId;
	DateT  AlarmDate;
	HourMinuteT AlarmTime;		
	//bool Vibrate;
}UiAppointmentInfoT;

typedef struct 
{
  UiApptSubjectT Subject;
	UiApptLocationT Location;
	UiAppointmentInfoT Info;
	UiAppointmentStateT State;
}UiAppointmentRecordT;

typedef struct
{
  bool IsValid;
  uint16 ApptRecId;
  DateT StartDate;
  DateT EndDate;
}UiDurationRecordT;

typedef struct
{ 
  uint16 DurRecId;
  UiDurationRecordT DurRecord;
}UiDurationInfoT;

class AppointmentC
{
public:
  AppointmentC(const UiAppointmentRecordT& Record, uint16 RecId);
  virtual ~AppointmentC(); 

  static uint16* GetDurationDbHandle();
  
  bool Initial();
  bool UpdateState();
  
  uint16 GetRecId();
  UiAppointmentInfoT GetInfo();
  void SetInfo(UiAppointmentInfoT Info);
  UiAppointmentStateT GetState(); 
  void SetState(UiAppointmentStateT State);
  uint32 FillEventFlag(DateTypeT DateType, DateT Date, int8 StartWeek);
  bool IsEventDate(DateT Date, int8 StartWeek, DateT& EventStartDate);
  bool GetEventDurAtDate(DateT Date, int8 StartWeek, HourMinuteT& StartTime, HourMinuteT& EndTime); 

  void AddDuration(UiDurationInfoT DurInfo);
  UiScheRecResultT DeleteNodeAtDate(DateT Date, int8 StartWeek);
  UiScheRecResultT DeleteNodesInDur(DateT StartDate, DateT EndDate, int8 StartWeek);
  UiScheRecResultT DeleteAllNodes();
  
private:
  uint16 GetFreeDurRecordId();
  UiScheRecResultT DeleteDurRecord(int Index);
  bool IsEventStartDate(DateT Date, int8 StartWeek);  
  bool IsAlarmDate(DateT Date, int8 StartWeek);
  bool IsInRecDuration(DateT Date, int8 StartWeek);  
  bool IsStartDateInDaily(DateT Date);
  bool IsStartDateInWeekly(DateT Date, int8 StartWeek);
  bool IsStartDateInMonthly(DateT Date);
  bool IsStartDateInYearly(DateT Date); 
#if 0  
  void CountPreNextDate(DateT BaseDate, DateT& PreDate, DateT& NextDate);  
  void CountPreNextDateInDaily(DateT BaseDate, DateT& PreDate, DateT & NextDate);
  void CountPreNextDateInWeekly(DateT BaseDate, DateT& PreDate, DateT & NextDate);
  void CountPreNextDateInMonthly(DateT BaseDate, DateT& PreDate, DateT & NextDate);
  void CountPreNextDateInYearly(DateT BaseDate, DateT& PreDate, DateT & NextDate);
#endif 
private:
  static uint16 DurationDbHandle;
  
private:
  uint16 mRecId;
  UiAppointmentInfoT mInfo;
  UiAppointmentStateT mState;
  
  BVectorC mDurationSet;

  uint32 mEventFlag;
#if 0
  WeekEventFlagT mWeeklyEventFlag[7];
#endif
};

inline uint16 AppointmentC::GetRecId()
{
  return mRecId;
}

inline UiAppointmentInfoT AppointmentC::GetInfo()
{  
  return mInfo; 
}

inline UiAppointmentStateT AppointmentC::GetState()
{  
  return mState; 
}

inline void AppointmentC::SetInfo(UiAppointmentInfoT Info)
{
  mInfo = Info;
}

inline void AppointmentC::SetState(UiAppointmentStateT State)
{
  mState = State;
}

inline uint16* AppointmentC::GetDurationDbHandle()
{
  return &DurationDbHandle;
}

uint32 GetEventDur(DateT startdate, DateT enddate);
bool GetFirstStartDate(const UiApptRecurrenceT& Recurrence, const DateT& StartDate, DateT & EventStartDate);
bool GetDateByYMD(DateT in_date, int8 m_interval, uint8 d_serial, DateT & out_date);
bool GetDateByYMW(DateT in_date, int8 m_interval, uint8 w_serial, uint8 week, DateT & out_date);
bool CheckDateByYMW(DateT date, uint8 w_serial, uint8 week);
bool CheckYearMonth(int32 & year, int32 & month);
int8 CountDayByWeekSerial(uint16 year, uint8 month, uint8 w_serial, uint8 week);
int8 CmpHourMinute(HourMinuteT HM1, HourMinuteT HM2);
bool CheckApptRecInfo(const UiApptRecurrenceT & RecInfo, UiApptRecurrenceT & ReturnRecInfo);

#endif

