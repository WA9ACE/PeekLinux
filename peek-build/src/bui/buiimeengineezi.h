

#ifndef UI_IME_ENGINE_EZI_H
#define UI_IME_ENGINE_EZI_H

#include "zi8api.h"
#include "zi8begin.h"
#include "zi8end.h"

#include "buiimeengine.h"

#define MAX_CANDIDATES_NUM            10
#define MAX_WORD_SIZE                 32
#define MAX_ELEMENT_SIZE              32

#if (defined USER_EZI_CHN) || (defined USER_EZI_CHN_TW)
#define MAX_ELEMENT_BUFFER_SIZE       10
#define MAX_WORD_BUFFER_SIZE          10
#define MAX_WORD_LENGTH_SIZE          10
#endif

class BImeEngineEZiC : public BImeEngineC
{
public:
  virtual ~BImeEngineEZiC();

public:
  virtual bool Load();
  virtual void Unload();

  virtual void CreateGlobalData();
  virtual void Initialize();

  virtual bool AddElement(uint32 KeyCode);
  virtual bool GetCandidates(BVectorC &Candidates, uint16 &FirstCandidateIndex);

  virtual void SetLanguage(ImeModeT ImeMode);

  virtual bool SwitchCapital();

public:
  virtual uint16  GetElementCount();
  virtual bool    IsElementBufferEmpty();
  virtual bool    IsElementBufferFull();

private:
  BImeEngineEZiC();

  void    CopyCandidates(BVectorC &Candidates);
  void    FreeCandidates(BVectorC &Candidates);

/******************************************************************************
* User database operations
******************************************************************************/
public:
  virtual void UdbSave(const BStringC &NewWord);


/******************************************************************************
* class static functions and data members
******************************************************************************/
public:
  static BImeEngineEZiC* GetInstance();
  static void ReleaseGlobalData();

private:
  static BImeEngineEZiC *TheInstance;
#ifdef ZI8_NO_GLOBAL_VARIABLES
  static void *sZi8GlobalDataP;
#endif

protected:

private:
  ZI8GETPARAM   mEziInfo;
  ZI8WCHAR      mElementBuf[MAX_ELEMENT_SIZE];
  ZI8WCHAR      mCandidatesBuf[MAX_CANDIDATES_NUM * MAX_WORD_SIZE];
  uint16        mTotalCandidateNum;

};

/*****************************************************************************
* $Log$
*****************************************************************************/

#endif

