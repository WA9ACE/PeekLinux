

#ifndef UIIMEMANAGER_H
#define UIIMEMANAGER_H

#include "buiime.h"
#include "buivector.h"

#include "buieditdefine.h"
#include "buieditctrl.h"

class BImeEngineC;

//! IME manager class
class BImeManagerC
{
  protected :
/*! \cond private */
    EditCtrlC *mParent;
    BVectorC   mImeList;
    int       mCurrentImeIndex;
    bool      mSameClassSwitch;
    bool      mDrawIme;
    BStringC   mSymbolLabel;
/*! \endcond */

  public :
    BImeManagerC(uint32 ImeGroup, EditCtrlC *ParentP);
    ~BImeManagerC();

    BImeC * GetCurrentIme();

    void SetIme(const BStringC &ImeName);
    void ShiftIme();
    void GetImeNameList(BVectorC &NameList);
    void SetSameClassSwitch(bool SameClassSwitch);
    
    bool KeyHandler(uint32 KeyCode, uint32 KeyMsg);
    void InputSymbols();

    /* Auot switch lowercase/uppercase */
    void AutoCapital(bool Auto);

    static uint32 GetImeAreaGroup(ImeAreaT ImeArea);
    static void   Release();

    static BImeEngineC*  CreateImeEngine();
    static void   FreeImeEngine();
};

inline void BImeManagerC::SetSameClassSwitch(bool SameClassSwitch)
{
  mSameClassSwitch = SameClassSwitch;
}

inline BImeC * BImeManagerC::GetCurrentIme()
{
  if (mCurrentImeIndex >= 0 && mCurrentImeIndex < mImeList.GetSize())
    return (BImeC *)mImeList[mCurrentImeIndex];
  else
    return NULL;
}



#endif
