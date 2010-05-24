

#ifndef UIIME123_H
#define UIIME123_H

#include "buiime.h"
#include "buisymbols123.h"
/******************************************************************************
*                                 BIme123C
******************************************************************************/
class BIme123C : public BImeC
{
  public :
    BIme123C(EditCtrlC * EditP);

  public:
    virtual bool KeyHandler(uint32 KeyCode, uint32 KeyMsg);
};

inline BIme123C::BIme123C(EditCtrlC * EditP)
  : BImeC(EditP)
{
  mName.Empty();
  mName += "123";
}
#if 0
inline char BIme123C::ProcessPoundKey()
{
  return '#';
}

inline char BIme123C::ProcessStarKey()
{
  return '*';
}
#endif
/******************************************************************************
*                              BImePhoneNumberC
******************************************************************************/
extern "C" void OnPhoneNumberStarKey(uint32 TimerId);

class BImePhoneNumberC : public BIme123C
{
  private :
    bool mStartKey;
    bool mTimerStarted;
    ExeTimerT mTimer;
	
	uint8 mStarKeyIndex;
	static char mStarKeyTable[];

  public :
    BImePhoneNumberC(EditCtrlC * EditP);

    virtual ~BImePhoneNumberC();

    void TimerActived();
    virtual bool KeyHandler(uint32 KeyCode, uint32 KeyMsg);

  protected:
    virtual char ProcessPoundKey();
    virtual char ProcessStarKey();
};

inline BImePhoneNumberC::BImePhoneNumberC(EditCtrlC * EditP)
  : BIme123C(EditP)
{
  mStartKey = FALSE;
  mTimerStarted = FALSE;
  mStarKeyIndex = 0;
  ExeTimerCreate(&mTimer, OnPhoneNumberStarKey, (uint32)this, 1, 0);
}

inline BImePhoneNumberC::~BImePhoneNumberC()
{
  if (mTimerStarted)
    ExeTimerStop(&mTimer);
  ExeTimerDelete(&mTimer);
}

inline char BImePhoneNumberC::ProcessPoundKey()
{
  return '#';
}

#if 0
/******************************************************************************
*                              BImeAddressC
******************************************************************************/
extern "C" void OnAddressStarKey(uint32 TimerId);

class BImeAddressC : public BIme123C
{
private :
  bool mStartKey;
  bool mTimerStarted;
  ExeTimerT mTimer;

  uint8 mStarKeyIndex;
  static char mStarKeyAddEditTable[];
  uint8 mIsLimitIme;

  public :
    BImeAddressC(EditCtrlC * EditP);
    virtual ~BImeAddressC();

    void TimerActived();
    virtual bool KeyHandler(uint32 KeyCode, uint32 KeyMsg);

  protected:
    virtual char ProcessPoundKey();
    virtual char ProcessStarKey(EditCtrlC* EditCtrlP);
};

inline BImeAddressC::BImeAddressC(EditCtrlC * EditP)
  : BIme123C(EditP)
{
  mStartKey = FALSE;
  mTimerStarted = FALSE;
  mStarKeyIndex = 0;
  mIsLimitIme = FALSE;
  ExeTimerCreate(&mTimer, OnAddressStarKey, (uint32)this, 1, 0);
}

inline BImeAddressC::~BImeAddressC()
{
  if (mTimerStarted)
    ExeTimerStop(&mTimer);
  ExeTimerDelete(&mTimer);
}

inline char BImeAddressC::ProcessPoundKey()
{
  return '#';
}
#endif

/******************************************************************************
*                                 ImeFullKPC
******************************************************************************/
//added by hebo 20080331 for qwert full keypad
typedef enum
{
    IME_NO_KEY = 0,
    IME_DOUBLE_QUESTIONMASKS,
    IME_QUOTE_KEY,
    IME_DOT_KEY,
    IME_SEMICOLON_KEY,
    IME_AT_KEY,
    IME_COLON_KEY,
    IME_REDUCE_KEY,
    IME_UNDERLINE_KEY,
    IME_COMMA_KEY,
    IME_EQUAL_KEY,
    IME_SPACE_KEY
}ImeSymKeyIdT;

class BImeQWERTC : public BImeC
{
  public :
    BImeQWERTC(EditCtrlC * EditP);
    void OpenImeSysbolDilog(void);
    void CloseImeSysbolDilog(void);
  public:
    virtual bool KeyHandler(uint32 KeyCode, uint32 KeyMsg);

  protected:
    ImeSymKeyIdT GetImeSymKeyId(uint32 KeyCode);
 
  private:
    static char mSymbolKeyTable[];
    static char mSymbolKeyTable2[];
    Symbols123C* mSymbols123C;
};

inline BImeQWERTC::BImeQWERTC(EditCtrlC * EditP)
  : BImeC(EditP),mSymbols123C(NULL)
{

  mName.Empty();
  mName += "QWERT";
}

#endif

/******************************************************************************
*                              ImeAddressC
******************************************************************************/
class BImeAddressC : public BImeQWERTC
{
private :
  
  uint32 mPreKey;   
  public :
    BImeAddressC(EditCtrlC * EditP);
    virtual ~BImeAddressC();

    virtual bool KeyHandler(uint32 KeyCode, uint32 KeyMsg);
};

inline BImeAddressC::BImeAddressC(EditCtrlC * EditP)
  : BImeQWERTC(EditP)
{
    mName.Empty();
    mName += "Address";
    mPreKey = 0;
}

inline BImeAddressC::~BImeAddressC()
{
}
