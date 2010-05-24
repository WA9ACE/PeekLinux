

#ifndef TIMEDATECTRL_H
#define TIMEDATECTRL_H 

#include "buiwindow.h"
#include "buicontrol.h"
#include "buimsgtimer.h"
#include "buikeymap.h"
#include "restypedefines.h"

#include "sysdefs.h"

#define SECONDS_PER_YEAR (86400*365)
#define BASE_OFFSET 432000
#define LEAP_YEAR 366
#define NORM_YEAR 365
#define BASE_YEAR 1980
#define LEAP_LOOP (366+(3*365))
#define MIN_YEAR  1981
#define MAX_YEAR  2079

  /*typedef enum {
      NO_CURSOR,
      CURSOR_YEAR,
      CURSOR_MONTH,
      CURSOR_DAY,
      CURSOR_HOUR,
      CURSOR_MINUTE,
      CURSOR_AMPM
  }CursPosName;*/

//! datetime element type 
typedef enum
{
  DT_ELEMENT_TYPE_DATE = 0,
  DT_ELEMENT_TYPE_TIME,
  DT_ELEMENT_TYPE_WEEK,
  DT_ELEMENT_TYPE_NUM
}DateTimeElementTypeT;

//! datetime edit style
typedef enum
{
  DT_EDIT_UNAVAILABLE = 0,            //!< don't allow edit 
  DT_EDIT_UPDOWN_KEY_CHANGE_VALUE = 1,
  DT_EDIT_UPDOWN_KEY_CHANGE_FOCUS
}DateTimeEditStyleT;

//! Structure of current network time 
typedef struct
{
  uint32 year;
  uint32 month;
  uint32 day;
  uint32 hour;
  uint32 minute;
  uint32 second;
}TimeTableT;

//! Structure of date which need to be edit
typedef struct
{
  uint32 year;
  uint8 month;
  uint8 day;
  uint8 dayofweek;
}DateStructT;
//!Structure of date which is default need to be edit
typedef struct
{
  uint32 year;
  uint8 month;
  uint8 day;
  bool IsValidFlag;
}DefaultDateStructT;

//! Structure of time which need to be edit
typedef struct
{
  int8 hour;
  int8 minute;
  int8 second;
  int8 am;       //!< am = 1:AM; am = 2:PM; am = 0;none;
}TimeStructT;

//! String of Day of week and month  
typedef struct
{
  int8 hour;
  int8 minute;
  //int8 second;
  int8 am;       //!< am = 1:AM; am = 2:PM; am = 0;none;
  bool IsValidFlag;
}DefaultTimeStructT;

//Custom Time format for not allowed edit
typedef enum
{
	/*HH =0,
    MM,
	SS,*/
	HHMM_12H = 0,
    MMSS_12H,
	HHMMSS_12H,
	HHMM_24H,
    MMSS_24H,
	HHMMSS_24H
}CustomTimeFormatStructT;
typedef enum
{
  DTN_YY = 1,
  DTN_MM = 2,
  DTN_DD = 3,
  DTN_MMM =4,
  DTN_YYYY = 5,
  DTN_MAX
}DateTypeNameT;
//YY=1,MM=2,DD=3,MMM=4,YYYY = 5,
typedef enum
{
  DTT_DDMMYYYY = 0x325,
  DTT_DDMMMYYYY = 0x345,
  DTT_DDMMYY = 0x321,
  DTT_MMDDYYYY = 0x235,
  DTT_MMMDDYYYY = 0x435,
  DTT_MMDDYY = 0x231,
  DTT_YYYYMMDD = 0x523,
  DTT_YYYYMMMDD = 0x543,
  DTT_YYMMDD = 0x123
}DateTimeT;


//custom date format
typedef enum
{
	//dot
	DDMMYYYY_DOT = 0,
    DDMMMYYYY_DOT,
	DDMMYY_DOT,
	MMDDYYYY_DOT,
	MMMDDYYYY_DOT,
	MMDDYY_DOT,
	YYYYMMDD_DOT,
	YYYYMMMDD_DOT,
	YYMMDD_DOT,
	//BIAS
    DDMMYYYY_BIAS,
    DDMMMYYYY_BIAS,
	DDMMYY_BIAS,
	MMDDYYYY_BIAS,
	MMMDDYYYY_BIAS,
	MMDDYY_BIAS,
	YYYYMMDD_BIAS,
	YYYYMMMDD_BIAS,
	YYMMDD_BIAS,
	//LINE
	DDMMYYYY_LINE,
    DDMMMYYYY_LINE,
	DDMMYY_LINE,
	MMDDYYYY_LINE,
	MMMDDYYYY_LINE,
	MMDDYY_LINE,
	YYYYMMDD_LINE,
	YYYYMMMDD_LINE,
	YYMMDD_LINE,
	//SPACE
	DDMMYYYY_SPACE,
    DDMMMYYYY_SPACE,
	DDMMYY_SPACE,
	MMDDYYYY_SPACE,
	MMMDDYYYY_SPACE,
	MMDDYY_SPACE,
	YYYYMMDD_SPACE,
	YYYYMMMDD_SPACE,
	YYMMDD_SPACE,
	//special format
	YYYY,
	MMDD_DOT,
	MMDD_BIAS,
	MMDD_LINE,
	MMDD_SPACE
}CustomDateFormatStructT;

//! String of Day of week and month  
typedef struct
{
  RESOURCE_ID(StringT) StringId; //!< resource id
}WeekMonthStringT;

//! DateTime control class define
class DateTimeCtrlC : public ControlC
{
public:
  DateTimeCtrlC();
  virtual ~DateTimeCtrlC();
  int8 GetAllowEdit(){return mEditAllowed;}
  void SetAllowEdit(DateTimeEditStyleT style){mEditAllowed = style;}
  void SetFormat(int8 Part, RESOURCE_ID(StringT) FormatID);
  void SetFormat(int8 Part, const StringT* pstrFormat);
  void SetFormat(DateTimeT dateType,char symbol = '/',int8 Part = 0);
  void SetFont(int8 Part, BalDispFontT Font);
  void SetPartRect(int8 Part, BalDispRectT Rect);
  void SetDate(uint16 year, uint8 month, uint8 day, bool update = TRUE);
  DateStructT *GetDate();
  void SetTime(int8 hour, int8 minute, int8 second, bool update = TRUE);
  void SetDefaultDate(uint16 year, uint8 month, uint8 day);
  void SetDefaultTime(int8 hour,int8 minute,int8 am);
  TimeStructT *GetTime();
  void SetAlignMode(int8 Part,BalDispHAlignT Mode);
  void SetCustomTimeFormat(CustomTimeFormatStructT CustomFormat,bool update = FALSE);  //for not allowed edit 
  void SetCustomDateFormat(CustomDateFormatStructT CustomFormat,bool update = FALSE);  
  void SetNoCycle(bool bNoCycle){mbNoCycle = bNoCycle;}
  //For Lunar
  void SetLunar(bool bLunar){m_bLunar = bLunar;}
  //bool SetDateToLunar();
  
/*! \cond private */
protected:
  virtual bool LoadResource(ResourceIdT ResourceId);
  bool OnNotify(uint32 ParamA, int32 ParamB);
  void OnParentNotify(uint32 Message, int32 Param);
  void OnShowNotify();
  void OnHideNotify(void);
  int32 OnCreate();
  void OnDestroy();
  void OnMove(int16 X, int16 Y);
  void OnSize(int16 Width, int16 Height);
  void OnEnable(bool Enable);
  void OnShow(bool Show);
  bool OnEraseBkgnd();
  void OnSetFocus(WindowC* OldWinP);
  void OnKillFocus(WindowC* NewWinP);
  bool OnKeyPress(uint32 KeyCode);
  bool OnKeyRelease(uint32 KeyCode);
  //bool OnKeyHold(uint32 KeyCode);
  void OnClockAct(uint32 flag);
  void OnHoldKeyTimer(uint32 KeyCode);
  virtual void Draw(DCC *DcP);
  void DrawLunarDate(DCC *DcP);
/*! \endcond */ 

private:
  typedef enum {
      NO_CURSOR,
      CURSOR_YEAR,
      CURSOR_MONTH,
      CURSOR_DAY,
      CURSOR_HOUR,
      CURSOR_MINUTE,
      CURSOR_AMPM
  }CursPosName;
    
  typedef enum {
      HOUR_FORMAT=0,
      MINUTE_FORMAT,
      SECOND_FORMAT,
      YEAR_FORMAT,
      MONTH_FORMAT,
      DAY_FORMAT,
      NUM_OF_FOMAT_KINDS
  }PartFormat;  

  typedef enum
  {
    DATE_EDIT = 0,
    TIME_EDIT,
    DATETIME_EDIT
  }EditDateTimeKind;

  bool mTimeRefreshAllow;
  TimeTableT mTimetable;
  BTimerC mailTimer;
  BTimerC KeyHoldTimer;
  int8 mNumofShowKinds;
  int8 m24Mode;       /*0: AM/PM in 12 Hours, 1: 24 Hours*/
  int8 mShow12flag;
  int8 mEditAllowed;  /*0: show 1:allow edit&updown key change value 2:allow edit&updown key change focus*/
  int8 mTotalFormat[NUM_OF_FOMAT_KINDS];
  TimeStructT mCurrentTime;
  TimeStructT mAllowEditTime;
  DefaultTimeStructT mAllowEditDefaultTime;
  DateStructT mAllowEditDate;
  DefaultDateStructT mAllowEditDefaultDate;
  CursPosName mPosition;/*focus position*/
  CursPosName mHeadPostion;
  CursPosName mTailPosition;
  bool mbFocus;
  bool mbNoCycle;
  EditDateTimeKind mEditKind;
  int8 mInputTimes;
  bool mGetNetFlags;
  BalDispRectT mDTResRect[DT_ELEMENT_TYPE_NUM];
  BalDispRectT mDTRect[DT_ELEMENT_TYPE_NUM];
  BalDispHAlignT mDTAlignMode[DT_ELEMENT_TYPE_NUM];
  const StringT *mDTFormatP[DT_ELEMENT_TYPE_NUM];
  BalDispFontT mDTFontId[DT_ELEMENT_TYPE_NUM];
  const DateTimeElementResT *mDateTimeElemsP[DT_ELEMENT_TYPE_NUM];
  //bool mKeyHoldState;
  void Initialize();
  bool GetNetCondition();
  void GetAndShowCurrentTime(DCC *DcP);
  uint16 GetFontHeight(DCC *DcP);
  bool CanHaveFocus();
  int8 Get24Mode(){return m24Mode;}
  void Set24Mode(int8 mode){m24Mode = mode;}
  int8 GetShow12Flag(){return mShow12flag;}
  void SetShow12Flag(int8 flag){mShow12flag = flag;}
  int8 JudgeIsAm(uint32 hour);
  void BufferCat(StringT *DesPtr, StringT *SourcePtr1, StringT *SourcePtr2);
  void LatinToUnicode(char *DstP, const char *SrcP, int NumCharsToConvert);
  bool LoadTime_Show(TimeTableT timeTable, StringT *stringPtr);
  void LoadTime_Edit(TimeStructT editTime,StringT *stringPtr, BalDispRectT *rectFocus, char *returnChar, DCC *DcP);
  void LoadYear(uint32 Year, StringT *stringPtr);
  bool LoadMonth(uint32 Month, StringT *stringPtr);
  void LoadDay(uint32 Day, StringT *stringPtr);
  void LoadDate_Show(TimeTableT timeTable, StringT *stringPtr, const StringT *formatStrP);
  void LoadDate_Edit(DateStructT editDate, StringT *stringPtr, const StringT *formatStrP, BalDispRectT *rectFocus, char *returnChar, DCC *DcP);
  bool LoadDayOfWeek(TimeTableT timeTable, StringT *stringPtr);
  void AnalyseTimeFormat(const StringT *formatStrP);
  void AnalyseDateFormat(const StringT *formatStrP);
  void AnalyseCursPosSequence(const StringT *TimeformatP, const StringT *DateformatP);
  void DrawDateTimeAllowEdit(DCC *DcP);
  void DrawFocusText(BalDispRectT *rectFocus, char *returnChar, DCC *DcP);
  void DrawDateTime(DCC *DcP);
  void DrawTime(TimeTableT timeTable, DCC *DcP);
  void DrawDayOfWeek(TimeTableT timeTable, DCC *DcP);
  void DrawDate(TimeTableT timeTable, DCC *DcP);
  void UpDownKey(bool up);
  void DigitInput(char Value);
  bool GetNextPreprocess();
  void GetNextEditObj(bool Right);
  void To12Hour(int8 *h, int8 *m);
  void To24Hour(int8 *h, int8 *m);
  void SetReturnCharAndFocusRect(char *returnChar, BalDispRectT *rectFocus, StringT *String4PraX, StringT *String4PraDX, DCC *DcP);
  bool LegalValidation();
  static uint8 Digits2Ascii(uint8 KeyCode);
  void SetDefaultFormat(int8 Part);
  void GetDataHeadPosition(const StringT *DateformatP);
  void GetNextDataEditPosition(bool Right);
  void GetThePositionFromchar(char FlagYmd);
  void CovertSomeDataFormatForEdit(void);
  // const StringT *ChangeSomeDataFormatForEdit(const StringT *formatStrP);

  //For Lunar Day Format
  bool m_bLunar;
  DECLARE_MAIL_MAP()
};

inline void DateTimeCtrlC::SetAlignMode(int8 Part,BalDispHAlignT Mode)
{
  mDTAlignMode[Part] = Mode;
}

inline uint8 DateTimeCtrlC::Digits2Ascii(uint8 KeyCode)
{
  if ((KeyCode >= KP_1_KEY && KeyCode <= KP_9_KEY) || IsEqual(KeyCode, KP_0_KEY))
    return KeyCode - KP_0_KEY + '0';
  if (IsEqual(KeyCode, KP_STAR_KEY))
    return '*';
  if (IsEqual(KeyCode, KP_POUND_KEY))
    return '#';

  return KeyCode;
}

uint32 multiple(uint32 x, uint32 y);

uint32 Divide(uint32 Second, uint32 value, uint32 *returnValue);

uint32 TimeTableToSeconds(TimeTableT date);

int8 GetWeekFromTimeTable(TimeTableT date);

TimeTableT SecondsToTimeTable(uint32 seconds);

int8 GetMonthFromDays(uint32 days, uint32 year, uint32 *returnDays);

bool IsValidNetTime(void);

int8 IsLeapYear(uint32 year);

int8 GetMonthMaxDate(uint32 year, uint8 month);




#endif
