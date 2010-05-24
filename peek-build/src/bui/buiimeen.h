

#ifndef UI_IME_EN_H
#define UI_IME_EN_H


#include "buiime.h"
#include "buiimeengine.h"

#define MAX_CANDIDATES_NUM            10

class BImeEnC : public BImeC
{
public:
  BImeEnC(EditCtrlC * EditP, BImeEngineC &ImeEngine);
  virtual ~BImeEnC();

public:
  virtual BStringC * GetName(bool Incremental = TRUE);
  virtual void Init(const BStringC &Text, uint32 CaretPos);
  virtual void Reset(const BStringC &Text, uint32 CaretPos);

  virtual bool KeyHandler(uint32 KeyCode, uint32 KeyMsg);
  virtual void AddCharToBuffer(char /*Char*/);
  virtual bool ShiftIme();
  virtual void AutoCapital(bool Auto);

  bool SwitchCapital();

  void Initialize();

private:
  uint8     mWordLen;
  bool      mAutoCapital;

private:
  bool HandleKeyPress(uint32 KeyCode);

  void ResetCandidates();
  void FreeCandidates();
  void ReplaceCandidate();

  void CreateGlobalData();
  void SetLanguage();

private:
  BImeEngineC    &mImeEngine;

  uint16        mFirstCandidateIndex;
  uint16        mCurCandidateIndex;
  uint16        mOldCandidateCharNum;

  BVectorC       mCandidates;
};

/*****************************************************************************
* $Log$
*****************************************************************************/

#endif

