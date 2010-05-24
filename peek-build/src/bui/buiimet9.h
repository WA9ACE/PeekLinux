

#ifndef UIIMET9_H
#define UIIMET9_H

#include "buiime.h"
#include "t9api.h"

class ImeT9C : public ImeC
{
  private :
    typedef enum {
      LANGUAGE_ENGLISH = T9PIDEnglish,
      LANGUAGE_SPANISH = T9PIDSpanish
    } LanguageT;

    typedef enum {
      T9_LOWER = 0,
      T9_WORD,
      T9_UPPER,
      T9_MAX
    } T9StatusT;
    int  mWordLen;
    T9StatusT mImeStatus; // 0 : lower, 1 : word, 2 : upper

    bool mAutoCapital;
    
  private :
    bool Initialize();
    LanguageT GetGenericLanguage();
    bool SetT9Language(LanguageT Language);
    void NextWord();
    int SymbToByteNCopy(char *pdDisplay, T9SYMB *psBuffer, unsigned int iLength);
    void CopyTextBufferToScreen();
    T9KEY BalKeyToT9Key(uint32 cKey);
    void SetShtState();
  public :

    static bool mInitialized;
    static T9SYMB * smSymbBufP;
    static T9AuxType * smAuxBufP;
    static T9AWUdbInfo * smUdbInfoP;
    static void Destory();

    ImeT9C(EditCtrlC * EditP);
    virtual ~ImeT9C();

    virtual BStringC * GetName(bool Incremental = TRUE);
    virtual void Init(const BStringC & Text, uint32 CursorPos);
    virtual void Reset(const BStringC & Text, uint32 CursorPos);

    virtual bool KeyHandler(uint32 KeyCode, uint32 KeyMsg) ;
    virtual void AddCharToBuffer(char /*c*/);
    //virtual void Activate(const StringT * NameP = NULL);
    virtual bool ShiftIme();
    /* Auto switch lower/upper case */
    virtual void AutoCapital(bool Auto) {mAutoCapital = Auto;};
    bool SwitchCapital();

    virtual bool OnLongPress(uint32 Key);
};


#endif
