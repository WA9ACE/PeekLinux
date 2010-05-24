

#ifndef DATECTRL_H
#define DATECTRL_H 

#include "sysdefs.h"
#include "buivector.h"
#include "buicontrol.h"
#include "buidatetimectrl.h"

#define ONE_MINUTE_IN_SEC 60
#define ONE_HOUR_IN_SEC   3600
#define ONE_DAY_IN_SEC    86400 
#define ONE_WEEK_IN_SEC   604800 

typedef enum
{
  UI_WEEK_SUNDAY = 0,
	UI_WEEK_MONDAY,
	UI_WEEK_TUESDAY,
	UI_WEEK_WEDNESDAY,
	UI_WEEK_THURSDAY,
	UI_WEEK_FRIDAY,
	UI_WEEK_SATURDAY
}UiWeekdayT;

//! Calendar's weekly holiday& national holidays
typedef struct
{
  uint8 WeeklyHoliday;   //each bit indicate one week
  //DateT NationalHoliday[];
}CalendarHolidayT;

//! Date notify event
typedef enum 
{
  DATE_EVENT_CHANGE_FOCUS_DATE        //!< Date control's focus date is changed but don't need update event list  
} DateEventT;

//! enum of DateCtrl Type 
typedef enum
{
  DATE_TYPE_MONTH,
  DATE_TYPE_WEEK,
  DATE_TYPE_DAY,
  DATE_TYPE_NUM
}DateTypeT;

//! struct of Date
typedef struct
{
  uint16 year;
  uint8 month;
  uint8 day;
}DateT;

//! struct of Time
typedef struct
{
  int8 hour;
  int8 minute;
}HourMinuteT;

//the type above and include REMINDER_WITHOUT_ALARM has special display in weekly mode 
//! enum of Note Type
typedef enum
{
  UI_APPOINTMENT_BIRTHDAY,
  UI_APPOINTMENT_MEMO,
  UI_APPOINTMENT_REMINDER_WITHOUT_ALARM,
  UI_APPOINTMENT_MEETING,
  UI_APPOINTMENT_CALL,       
  UI_APPOINTMENT_REMINDER,
  UI_APPOINTMENT_TYPE_NUM
}UiAppointmentTypeT;

//! Date control class define
class DateCtrlC : public ControlC
{
  public:
    DateCtrlC();
    virtual ~DateCtrlC();
    virtual bool LoadResource(ResourceIdT ResourceId); 
    void SetFocusDate(DateT Date);
    void SetCurNetDate(DateT Date); 
    void SetStartWeek(int8 StartWeek);
    void SetEventList(void* EventListP);
    void SetHoliday(const CalendarHolidayT& Holiday);
    DateT GetFocusDate();
    
  private:
    void NotifyEvent(DateEventT EditEvent);	
    void IninitializeDate();
    bool OnKeyPress(uint32 KeyCode); 
    void OnKeyTimer();
    void OnShowNotify(); 
    void OnHideNotify(); 
    bool OnSetMonthly(int8 direction);
    bool OnSetWeekly(int8 direction, bool NeedCheck = TRUE);
    bool OnSetDate(int8 direction);

    virtual void Draw(DCC *DcP);
    void DrawWeekday(DCC *DcP);
    void DrawMonthlyDate(DCC *DcP);
    void DrawMatrix(DCC *DcP);
    void DrawTime(DCC *DcP);
    void DrawDate(DCC *DcP, const BalDispRectT& DateRect, DateT Date, bool HasEvent, bool IsHoliday = FALSE);    

    void DrawEventIcon(DCC *DcP, RESOURCE_ID(ImageResT) ImageRes, const BalDispRectT& Rect);
    void DrawEventRect(DCC *DcP, uint8 EventCnt, const BalDispRectT& Rect);
    void DrawText(DCC *DcP, const BStringC &str, const BalDispRectT& Rect, BalDispHAlignT Align);

    void BuildDurList(BLinkedListC* EventListP, DateT CurDate, HourMinuteT StartHour, HourMinuteT EndHour, BVectorC & DurList);
    void InsertTimeSolt(BVectorC& DurList, HourMinuteT StartSolt, HourMinuteT EndSolt);
    void InsertIconSolt(BVectorC& DurList, HourMinuteT StartSolt, UiAppointmentTypeT ApptType);
    
    bool CmpWithFocusDate(DateT Date);
    bool CmpWithCurNetDate(DateT Date);
    
    DateTypeT mDateType;
    DateT mFocusDate;
    DateT mCurNetDate;
    int8 mStartWeek;
    BLinkedListC* mEventListP;
    
    //! mExApptFlag[7]:store the appointments' type between 7:00-8:00
    /*bit           0 1 2.......3 4 5......6 7 8.......9 A B.........
    //description   first     second   third   invalid appointment type
    //
    //value:could indicate max 8 kinds of appointments
    //      000-------Birthday
    //      001-------Memo
    //      010-------Reminder without alarm
    //      011-------end flag
    // 
    //example:      000        001      010        011  ....
    //means: the day has three type appointments between 7:00-8:00
    //
    //example:      001        010      011        011  ....
    //means: the day has two type appointments between 7:00-8:00,
    //they are Memo,Reminder withourt alarm
    //
    //note:the three appointments type should in queue that:
    //     the next's type value should larger than the last's
    */
    uint32 mExApptFlag[7];
    //! store the max time intervals include special intervals between 7:60-8:00
    uint8 mExTimeIntervalNum;

    
    //! the top and current focus time interval in weekly view mode
    //for display extend time intervals for special type of appointments
    //the True time is mCurTimePos+offset, the offset is 7, 
    //because the extend time intervals is between 7:60-8:00
    //           IntervalPos                   RealTime
    //              0                             8:00
    //              1                             9:00
    //              2                            10:00
    //                          ......
    //             15                            23:00
    //             16                             0:00
    //             17                             1:00 
    //                          ......
    //             23                             7:00     
    //             24           -------         Birthday
    //             25           -------           Memo
    //             26           -------     Reminder without alarm
    //
    uint8 mTopTimePos; 
    uint8 mCurTimePos; 

    BTimerC mTimer;
    uint8 mInputNum;

    //! store weekly holiday and national holiday
    CalendarHolidayT mHoliday;

    DECLARE_MAIL_MAP()
};

/*!
\brief Set current focus date

\param Date -[in] DateT
*/
inline void DateCtrlC::SetHoliday(const CalendarHolidayT& Holiday)
{
  mHoliday = Holiday;  
}

/*!
\brief Set current focus date

\param Date -[in] DateT
*/
inline void DateCtrlC::SetFocusDate(DateT Date)
{
  mFocusDate = Date;  
}

/*!
\brief Set current network date

\param Date -[in] DateT
*/
inline void DateCtrlC::SetCurNetDate(DateT Date)
{
  mCurNetDate = Date;  
}

/*!
\brief Set start week in weekdays

\param StartWeek -[in] StartWeek
*/
inline void DateCtrlC::SetStartWeek(int8 StartWeek)
{
  mStartWeek = StartWeek;  
}

/*!
\brief Get current focus date

\param Date -[in] DateT
*/
inline DateT DateCtrlC::GetFocusDate()
{
  return mFocusDate;  
}

int8 CmpDate(DateT date1, DateT date2);
int32 CntDayInterval(DateT date1, DateT date2);
int32 CntMonthInterval(DateT date1, DateT date2);
int32 CntWeekInterval(DateT date1, DateT date2, int8 startweek);
bool IsInSameScreen(DateT date1, DateT date2, DateTypeT datetype, int8 startweek);
bool GetFirstDateInWeek(DateT date, int8 startweek, DateT & firstdate);
int8 GetWeekFromDate(DateT date);
int8 GetWeekNumFromDate(DateT date, int8 startweek);
bool GetDateByOffset(int32 offset, const DateT BaseDate, DateT & Date); 
RESOURCE_ID(StringT) Get1CharWeekRes(int8 week);
RESOURCE_ID(StringT) Get3CharWeekRes(int8 week);
RESOURCE_ID(StringT) GetShortMonthRes(uint8 month);
RESOURCE_ID(StringT) GetLongMonthRes(uint8 month);

#endif

