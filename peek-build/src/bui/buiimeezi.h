

#ifndef UIIMEEZI_H
#define UIIMEEZI_H

#include "zi8api.h"
#include "zi8begin.h"
#include "zi8end.h"

#include "buiime.h"

#define MAX_CANDIDATES_NUM            10
#define MAX_WORD_SIZE                 64
#define MAX_ELEMENT_SIZE              64

#if (defined USER_EZI_CHN)||(defined USER_EZI_CHN_TW)
#ifdef DISP_LCD_96x64
#define MAX_ELEMENT_BUFFER_SIZE       7
#define MAX_WORD_BUFFER_SIZE          7
#define MAX_WORD_LENGTH_SIZE          7
#else
#define MAX_ELEMENT_BUFFER_SIZE       10
#define MAX_WORD_BUFFER_SIZE          10
#define MAX_WORD_LENGTH_SIZE          10
#endif
#endif

class ImeeZiC : public BImeC
{
public:
  ImeeZiC(EditCtrlC * EditP);
  virtual ~ImeeZiC();

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

/******************************************************************************
* class static functions and data members
******************************************************************************/
public:
  static void Destory();

  static bool sInitialized;
#ifdef ZI8_NO_GLOBAL_VARIABLES
  static void *sZi8GlobalDataP;
#endif

/******************************************************************************
* eZi member functions and data members
******************************************************************************/
private:
  bool Load();
  void Unload();
  bool HandleKeyPress(uint32 KeyCode);
  bool GetCandidates();
  void ResetCandidates();
  void FreeCandidates();
  void ReplaceCandidate();

  void CreateGlobalData();
  void SetLanguage();

  void FreeNumVector();

private:
  ZI8GETPARAM   mEziInfo;
  ZI8WCHAR      mElementBuf[MAX_ELEMENT_SIZE];
  ZI8WCHAR      mCandidatesBuf[MAX_CANDIDATES_NUM * MAX_WORD_SIZE];
  uint16        mTotalCandidateNum;

  ZI8WCHAR      mWordBuf[MAX_WORD_SIZE];

  //ZI8WCHAR      mEziWordText[MAX_WORD_SIZE];
  //uint16        mEziCandiPos;
  //uint16        mEziElementPos;
  //uint16        mEziCurPage;
  //BVectorC       mEziNumOfPage;

  BVectorC       mCandidates;
  uint16        mCurCandidateIndex;
  uint16        mOldCandidateCharNum;

#if 0
#if (defined USER_EZI_CHN)
private:
  void LoadEziChn();
  void UnLoadEziChn();
  bool HandleEziChn(uint32 iKey, bool IsDraw = FALSE);
  void CopyBuffersToScreen(bool IsDraw = FALSE);
  void DrawSyllable();
  ZI8WCHAR* GetCandidate(ZI8WCHAR* CandiBuf, uint8 CandiIndex);

private:
  ZI8GETPARAM mEziPinyinInfo;
  ZI8GETPARAM mEziWordInfo;
  ZI8WCHAR mEziPinyinElement[MAX_ELEMENT_BUFFER_SIZE];
  ZI8WCHAR mEziWordElement[MAX_ELEMENT_BUFFER_SIZE];
  ZI8WCHAR mEziCandidate[MAX_WORD_BUFFER_SIZE*MAX_WORD_LENGTH_SIZE];
  ZI8WCHAR mEziCurrentWord[MAX_WORD_BUFFER_SIZE*MAX_WORD_LENGTH_SIZE];
  //ZI8WCHAR mEziWord[MAX_WORD_BUFFER_SIZE*MAX_WORD_LENGTH_SIZE];
  ZI8WCHAR mEziPinyin[MAX_WORD_BUFFER_SIZE*MAX_WORD_LENGTH_SIZE];
  uint8 mUnderLineNum;
  uint8 mCandiPos;
  uint8 mCurPinyinNum;
  uint8 mCurPage;
  bool mNoMoreWord;
  bool mNoMorePinyin;
#endif
#endif
};

/*****************************************************************************
* $Log$
*****************************************************************************/

#endif

