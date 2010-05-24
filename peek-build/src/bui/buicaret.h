

#ifndef CARET_H
#define CARET_H 


#include "buiwindow.h"
#include "buicontrol.h"

//! Caret class
class CaretC
{
private:
  enum 
  {
    DEFAULT_BLINK_TIME = 500,       // 0.5 second
    DEFAULT_COLOR      = BAL_DISP_RGB_BLACK_565
  };

public:
  CaretC(WinHandleT OwnerHandle, const RectC &Rect);
  virtual ~CaretC();

  void Toggle();

  void GetRect(RectC *RectP) const;
  void SetRect(const RectC &Rect);

  BalDispColorT GetColor() const;
  BalDispColorT SetColor(BalDispColorT Color);

  void Enable(bool Enable);
  void Active(bool Active);

  uint32 GetBlinkTime();
  void SetBlinkTime(uint32 BlinkTime);
  
  void Draw();

private:
  //void Initialze(uint8 Width, uint8 Height);

  void Hide();
  void Show();

private:
  WinHandleT  mOwnerHandle;

  bool        mActive;
  bool        mEnable;

  uint32      mBlinkTime;         // in milliseconds
  BTimerC  mBlinkTimer;
  
  RectC       mRect;
  BalDispColorT  mColor;

  DCC         *mDcP;          // Using parent's dc
};

inline BalDispColorT CaretC::GetColor() const
{
  return mColor;
}

inline BalDispColorT CaretC::SetColor(BalDispColorT Color)
{
  BalDispColorT OldColor = mColor;
  mColor = Color;
  return OldColor;
}

inline uint32 CaretC::GetBlinkTime()
{
  return mBlinkTime;
}

inline void CaretC::SetBlinkTime(uint32 BlinkTime)
{
  if (BlinkTime < DEFAULT_BLINK_TIME)
    BlinkTime = DEFAULT_BLINK_TIME;
  mBlinkTime = BlinkTime;
}




#endif

