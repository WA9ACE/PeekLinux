

#ifndef UIIME_H
#define UIIME_H

#include "buieditctrl.h"

//! ImeClassT used to switch in the same class (example : Abc, ABC, abc),
//! if no same class set a unique ImeClass or set to IME_CLASS_BASE
typedef enum
{
  IME_CLASS_BASE,  //!< No same class switching
  IME_CLASS_ABC,   //!< IME ABC, IME Abc, IME abc
  IME_CLASS_123,   //!< Ime 123
  IME_CLASS_WORD   //!< IME T9, IME t9
} ImeClassT;

//! IME mode
typedef enum
{
  IME_MODE_NONE               = 0x0000L,
  IME_MODE_CALL_NUMBER        = 0x0001L,        //!< Call number
  IME_MODE_123                = 0x0002L,        //!< Numeric
  IME_MODE_ABC_LOWER          = 0x0004L,        //!< abc
  IME_MODE_ABC_UPPER          = 0x0008L,        //!< ABC
  IME_MODE_ABC_UPPER_INITIAL  = 0x0010L,        //!< Abc
  IME_MODE_EN_LOWER           = 0x0020L,        //!< word
  IME_MODE_EN_UPPER           = 0x0040L,        //!< WORD
  IME_MODE_EN_UPPER_INITIAL   = 0x0080L,        //!< Word
  IME_MODE_PINYIN             = 0x0100L,        //!< Chinese Pinyin
  IME_MODE_SC_STROKE          = 0x0200L,        //!< Simplified Chinese Stroke
  IME_MODE_BOPOMOFO           = 0x0400L,        //!< Chinese Bopomofo
  IME_MODE_TC_STROKE          = 0x0800L,        //!< Traditional Chinese Stroke
  IME_MODE_ADDRESS            = 0x1000L,        //!< Address

  //added by hebo 20080331 add new ime mode for qwert 
  IME_MODE_QWERT		= 0x20000L,
  
  IME_MODE_ARABIC             = 0x00010000L,
  IME_MODE_HEBREW             = 0x00020000L,
  IME_MODE_HINDI              = 0x00040000L,
  IME_MODE_THAI               = 0x00080000L,
  IME_MODE_MAX                = 0x7FFFFFFFL
} ImeModeT;

//! IME base class
class BImeC
{
  protected :
/*! \cond private */
    ImeClassT mImeClass;
    ImeModeT  mImeMode;
    EditCtrlC *mEditP;
    BStringC   mName;
/*! \endcond */

  public :
    BImeC(EditCtrlC * EditP);
    virtual ~BImeC();

  public:
    ImeClassT GetImeClass();
    void SetImeClass(ImeClassT ImeClass);
    virtual BStringC * GetName(bool Incremental = TRUE);
    virtual void Init(const BStringC &, uint32);
    virtual void Reset(const BStringC &String, uint32 CursorPos);

    virtual bool KeyHandler(uint32 KeyCode, uint32 KeyMsg) = 0;
    virtual void AddCharToBuffer(char c);

    virtual bool ShiftIme();
    virtual void AutoCapital(bool Auto);
};

inline BImeC::BImeC(EditCtrlC * EditP)
  : mImeClass(IME_CLASS_BASE), mImeMode(IME_MODE_NONE),
    mEditP(EditP)
{ 
}

inline BImeC::~BImeC()
{
}

inline ImeClassT BImeC::GetImeClass()
{
  return mImeClass;
}

inline void BImeC::SetImeClass(ImeClassT ImeClass)
{
  mImeClass = ImeClass;
}

inline BStringC * BImeC::GetName(bool /* Incremental = TRUE*/)
{
  return &mName;
}

inline void BImeC::Init(const BStringC &, uint32)
{
}

inline void BImeC::Reset(const BStringC &String, uint32 CursorPos)
{
  Init(String, CursorPos);
}

inline void BImeC::AddCharToBuffer(char /*c*/)
{
}

inline bool BImeC::ShiftIme()
{
  return FALSE;
}

inline void BImeC::AutoCapital(bool /*Auto*/)
{
}



#endif

