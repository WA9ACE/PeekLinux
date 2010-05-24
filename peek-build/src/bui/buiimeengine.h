

#ifndef UI_IME_ENGINE_H
#define UI_IME_ENGINE_H

#include "buistring.h"

#include "buiime.h"

class BImeEngineC
{
public:
  BImeEngineC();
  virtual ~BImeEngineC() = 0;

public:
  BStringC * GetName();

public:
  virtual bool Load();
  virtual void Unload();

  virtual void CreateGlobalData() = 0;
  virtual void Initialize() = 0;

  virtual bool AddElement(uint32 KeyCode) = 0;
  virtual bool GetCandidates(BVectorC &Candidates, uint16 &FirstCandidateIndex) = 0;

  virtual void SetLanguage(ImeModeT ImeMode) = 0;

  virtual bool SwitchCapital() = 0;

public:
  virtual uint16  GetElementCount() = 0;
  virtual bool    IsElementBufferEmpty() = 0;
  virtual bool    IsElementBufferFull() = 0;


/******************************************************************************
* User database operations
******************************************************************************/
public:
  virtual void UdbSave(const BStringC &NewWord);


/******************************************************************************
* class static functions and data members
******************************************************************************/
protected:
  static bool   sInitialized;

protected:
  BStringC       mName;

};


inline BStringC * BImeEngineC::GetName()
{
  return &mName;
}

/*****************************************************************************
* $Log$
*****************************************************************************/

#endif

