

#ifndef UIWEEKCTRL_H
#define UIWEEKCTRL_H


#include "buicontrol.h"
#include "restypedefines.h"

#define SUN_BIT 0
#define MON_BIT 1
#define TUE_BIT 2
#define WED_BIT 3
#define THU_BIT 4
#define FRI_BIT 5
#define STA_BIT 6

typedef enum
{
  WEEK_TYPE_FOR_SHOW,
  WEEK_TYPE_FOR_EDIT
}WeekCtrlTypeT;

typedef enum 
{
  WEEK_EVENT_CUR_WEEK_IS_SETED,     //!< Current cursor focus week is seted
  WEEK_EVENT_CUR_WEEK_IS_UNSETED    //!< Current cursor focus week is unseted
} WeekEventT;

class WeekCtrlC : public ControlC
{

public:
  WeekCtrlC();
  virtual ~WeekCtrlC(){};
  
  virtual bool LoadResource(ResourceIdT ResourceId);
  void OnSetFocus(WindowC* OldWinP);  
  void OnKillFocus(WindowC* NewWinP);
  
  void SetWeekDayValue(uint8 ValueBitmap){mSetValue = ValueBitmap;}
  
  void SetCurrentWeekDay();
  uint8 GetWeekDayValue(){return mSetValue;}

protected:
  bool OnKeyPress(uint32 KeyCode);
  void NotifyEvent();
  
private:
  virtual void Draw(DCC *DcP);
  void DrawSlot(DCC *DcP, uint32 Position, bool IsHighLight);
  void DrawSetFlag(DCC *DcP, uint32 Position, bool UpdateDisplay);
 
  
  /**********************************************************
  *mSetvalue should be recgonized as 8 bit seperate values:
  *bit: 0---1---2---3---4---5---6---7
  *     Sun  Mon  Tue  Wed  Tur   Fri   Sat     All
  *bit 7 set, means all the days of week are set
  **********************************************************/
  uint8 mSetValue;
  uint32 mCursorPosition;

  DECLARE_MAIL_MAP()

};
#endif

