

#ifndef UIIMEABC_H
#define UIIMEABC_H

#include "buiime.h"
#include <ctype.h>

#define ALPHA_INIT_VALUE ((uint32)-1)

/******************************************************************************
*                              BImeAbcLowerC
******************************************************************************/
/*
Lowercase abc IME,inputing character a-z
*/
extern "C"
{
void OnTimerActived(uint32 TimerId);

void OnLongPressActived(uint32 TimerId);
}

enum {
  IMEABC_LOWER = 0,
  IMEABC_WORD,
  IMEABC_UPPER
};

class BImeAbcLowerC : public BImeC
{
  protected :
    ExeTimerT mTimer;
    int8      mTimeNum;
    bool      mTimerStarted;
    uint32    mAhlphaValue;
    ExeTimerT mLongPressTimer;
    uint32    mPressedKey;
    bool      mAutoCapital;
  private :
    void AlphaInput(uint32 Input);
    bool OnChangeToAlpha(uint32 *Input, int8 *Times);
  public :
    BImeAbcLowerC(EditCtrlC * EditP, bool SetName = TRUE);
    virtual ~BImeAbcLowerC();
  public:
    void TimerActived();
    virtual bool KeyHandler(uint32 KeyCode, uint32 KeyMsg);
    virtual char GetChar(char c, bool Replace = FALSE);

    virtual void AutoCapital(bool Auto);
    virtual bool SwitchCapital();
    virtual void AddCharToBuffer(char);

    virtual void Init(const BStringC &, uint32);
    virtual void Reset(const BStringC &String, uint32 CursorPos);

    void LongPressActived();
};

inline BImeAbcLowerC::BImeAbcLowerC(EditCtrlC * EditP, bool SetName /* = TRUE*/)
  : BImeC(EditP)
{
  mAhlphaValue = (uint32)-1;
  mTimeNum = 0;
  mTimerStarted = FALSE;
  ExeTimerCreate(&mTimer, OnTimerActived, (uint32)this, 1, 0);
  if (SetName)
  {
    //mName = "abc";
    mName.Empty();
    mName += "abc";
  }
  SetImeClass(IME_CLASS_ABC);
  ExeTimerCreate(&mLongPressTimer, OnLongPressActived, (uint32)this, 1, 0);
}

inline BImeAbcLowerC::~BImeAbcLowerC()
{
  if (mTimerStarted)
    ExeTimerStop(&mTimer);
  ExeTimerDelete(&mTimer);
  ExeTimerStop(&mLongPressTimer);
  ExeTimerDelete(&mLongPressTimer);
}

inline char BImeAbcLowerC::GetChar(char c, bool /* Replace = FALSE*/)
{
  //Replace = FALSE; // Suppress warning
  return c;
}

inline void BImeAbcLowerC::Init(const BStringC &, uint32)
{
  mAhlphaValue = ALPHA_INIT_VALUE;
  mTimeNum = 0;
}

inline void BImeAbcLowerC::Reset(const BStringC &String, uint32 CursorPos) 
{
  Init(String, CursorPos);
  SwitchCapital();
}

/******************************************************************************
*                              BImeAbcUpperC
******************************************************************************/
/*
Uppercase abc IME,inputing character A-Z 
*/
class BImeAbcUpperC : public BImeAbcLowerC
{
  public :
    BImeAbcUpperC(EditCtrlC * EditP);
    virtual char GetChar(char c, bool Replace = FALSE);

    virtual void AutoCapital(bool Auto);
    virtual bool SwitchCapital();
};

inline BImeAbcUpperC::BImeAbcUpperC(EditCtrlC * EditP)
  : BImeAbcLowerC(EditP, FALSE)
{
  //mName = "ABC";
  mName.Empty();
  mName += "ABC";
}

inline char BImeAbcUpperC::GetChar(char c, bool /* Replace = FALSE*/)
{
  //Replace = FALSE;
  return (char)toupper(c);
}

inline void BImeAbcUpperC::AutoCapital(bool /*Auto*/)
{
}

inline bool BImeAbcUpperC::SwitchCapital()
{
  return FALSE;
}

/******************************************************************************
*                              BImeAbcWordC
******************************************************************************/
/*
Word abc IME,inputing character A-Z for the first char in a word
and a-z for the others
*/
class BImeAbcWordC : public BImeAbcLowerC
{
  private :
    BStringC mText;
    uint32  mCaretPos;
  public :
    BImeAbcWordC(EditCtrlC * EditP);
    virtual ~BImeAbcWordC();
    virtual void Init(const BStringC &String, uint32 CursorPos);
    virtual char GetChar(char c, bool Replace = FALSE);

    virtual void AddCharToBuffer(char c);
};

inline BImeAbcWordC::BImeAbcWordC(EditCtrlC * EditP)
  : BImeAbcLowerC(EditP, FALSE)
{
  mCaretPos = 0;
  //mName = "Abc";
  mName.Empty();
  mName += "Abc";
}

inline BImeAbcWordC::~BImeAbcWordC()
{
}

inline void BImeAbcWordC::Init(const BStringC &String, uint32 CursorPos)
{
  BImeAbcLowerC::Init(String, CursorPos);
  //mText = String;
  mText.Empty();
  mText += String;
  
  mCaretPos = CursorPos;
}



#endif

