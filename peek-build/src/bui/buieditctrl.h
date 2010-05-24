

#ifndef UIEDITCONTROL_H
#define UIEDITCONTROL_H 


#include "buivector.h"
#include "buicontrol.h"

#include "buieditdefine.h"
#include "buimsgtimer.h"
#include "buidcc.h"
#include "buitrace.h"
#include "buicaret.h"
#include "buistring.h"
#include "buistatictextctrl.h"
#include "buiscrollbar.h"
#include "builistdialog.h"
#include "buipopupdialog.h"

#include "baldispapi.h"
typedef enum
{
	SYMBOL_SINGLE = 0,
	SYMBOL_COMB,
	SYMBOL_MAX
}ScrollSymLabT;

typedef struct
{
  char sym_s;
  char* sym_c;
  ScrollSymLabT lab;
}ScrollSymMapT;


//! Edit control content update flag
typedef enum
{
  UPDATE_CONTENT_BEGIN = 0,   //!< Begin edit content updating
  UPDATE_CONTENT_END          //!< End edit content updating
} UpdateContentFlagT;
typedef enum
{
  SCROLL_NONE = 0,
  SCROLL_UP,
  SCROLL_DOWN,
  SCROLL_LEFT_RIGHT
}ScrollContentFlagT;

typedef enum
{
  TURN_PAGE_UP = 0,
  TURN_PAGE_DOWN,
  TURN_PAGE_BOTTOM,
  TURN_PAGE_TOP
}TurnPageFlagT;

//! Position structure to indicate edit content position
typedef struct 
{
  uint16 Index;               //!< Index of edit content segment, always be 0 as EditCtrlC
  uint16 Offset;              //!< Offset of position in a edit content segment
} PositionT;

//! Resize information
typedef struct 
{
  uint16    MaxHeight;        //!< Max height after control resizing, designated by resource
  uint16    MinHeight;        //!< Min height after control resizing, init value: rect.dy
} ResizeInfoT;

//! Preview information
typedef struct 
{
  PreviewModeT  Mode;         //!< Control show preview mode or not
  bool          IsActived;    //!< Control is in preview mode or not
  uint16        Height;       //!< Control height while in preview mode
} PreviewInfoT;

//! Edit line information
typedef struct 
{
  PositionT StartPos;         //!< Line start position in edit content
  PositionT EndPos;           //!< Line end position in edit content
  uint16    Height;           //!< Line height
  uint32    TotalChars;       //!< Reserved, not being used. Char count contained in line
} LineInfoT;

//! Call number style information
typedef struct
{
  bool      Topdown;          //!< Show content with top down mode or not
  bool      CaretShow;        //!< Show caret or not
  bool      AutoFont;         //!< Adjust font automatically
  BalDispFontT BigFont;       //!< Big font
  BalDispFontT SmallFont;     //!< Small font
} CallNumberStyleT;

//! Caret information
typedef struct 
{
  bool      Show;             //!< Show caret or not
  CaretC    *CaretP;          //!< Pointer to caret object
  uint16    CurrentLine;      //!< Index of line having caret
  PositionT Position;         //!< Logical position of caret in edit
  PositionT LastPosition;     //!< Last updated logical position in horizontal dimension
} CaretInfoT;

//! Edit's appearance
class AppearanceC
{
public:
  AppearanceC()
  {
    LeftMargin = 0;
    RightMargin = 0;
  }
  RectC     TextRect;         //!< Edit's content showing area
  uint16    LeftMargin;       //!< Left margin in TextRect
  uint16    RightMargin;      //!< Right margin in TextRect
};

//! Lines information structure
class LinesInfoC
{
public:
  LinesInfoC()
  {
    FirstVisibleLine = 0;
    VisibleLines = 0;
    ValidLines = 0;
  }
  uint16  FirstVisibleLine;   //!< Top line number showed in the Text area, 0 based
  uint16  VisibleLines;       //!< Lines showed in the text area
  uint16  ValidLines;         //!< Lines contain valid information
  BVectorC Items;              //!< Vector of LineInfoT*
};

//! Content information structure
class ContentInfoC
{
public:
  ContentInfoC():String(512)
  {
    MaxSize = 0;
    LatinMode = EDIT_CODEPAGE_LATIN_8_BIT;
  }
  uint32  MaxSize;            //!< Edit's max size, in Bytes
  BStringC String;
  EditCodePageLatinModeT LatinMode;   //!< Latin 8 bit/7 bit mode
};


class ClipBoardC
{
public:
  ~ClipBoardC();
  static ClipBoardC* GetInstance(void);

  bool IsEmpty();
  
  void StoreContent(BStringC& Text);
  bool RetrieveContent(BStringC& Text);

private:
  ClipBoardC();
  static ClipBoardC* _instance; 

  BStringC* mTextP;
};


inline bool ClipBoardC::IsEmpty()
{
  return (bool)(IsNull(mTextP) || mTextP->IsEmpty());
}

/*!
\brief Get the unique instance of the ClipBoardC 

\retval the unique instance of ClipBoardC
*/ 
inline ClipBoardC* UiGetClipBoard(void)
{
  return ClipBoardC::GetInstance();
}


class BImeC;
class BImeManagerC;

/**
 *  \defgroup EditCtrlC
 *  \ingroup ControlC
 */
/**
 *  \ingroup EditCtrlC
 *  \brief Edit control class
 */
class EditCtrlC : public ControlC
{
public:
  EditCtrlC();
  EditCtrlC(CtrlTypeT ControlType);
  virtual ~EditCtrlC();
  
  virtual bool ReLoadResource(ResourceIdT ResourceId);

protected:
  virtual bool LoadResource(ResourceIdT ResourceId);

private:
  void Initialize();
  void ReleaseResource();
  void InitResParam(const EditCtrlResT *ResourceP);
  void InitLines();
  void RollContent(DCC *DcP);
  void DrawTextRectWithoutBorder(const RectC &rect, DCC *DcP);
  uint8 ReBuildLine(uint16 Line);
    
// Data information
private:
  ContentInfoC  mContent;
  bool mNewLine;   //add for qwert: if allow to insert new line, default value is true;
  bool mShiftHold;
  int32 mInsertPosition;
  ScrollContentFlagT mRollScreen;
  void ClearData();
  uint16 GetDataItemsCount();
  bool ProcessClearWord();//add for qwert: clear a whole word when long press clear key.

public:
  BStringC * GetData();

  // Add and Clear operation: from current caret position
  virtual bool Add(char Ch);
  virtual bool Add(uint16 WCh);
  virtual bool Add(const BStringC &String);
  virtual bool Add(RESOURCE_ID(StringT) StringResId);
  virtual bool Add(const StringT* StringP);

  virtual bool Clear(uint16 Count);
  virtual bool IsEmpty();

  virtual bool Replace(const BStringC &ReplaceString, uint16 CountCleared);
  bool Replace(char Ch);
  bool Replace(uint16 WCh);
  bool Replace(const StringT* StringP, uint16 CountCleared);  
  bool Append(const BStringC &AppendString);

  void SetAddNewLine(bool Enable);

  virtual bool ProcessAdd(char Ch);
  virtual bool ProcessAdd(const BStringC &String);
  virtual bool ProcessClear(uint16 Count);
  virtual bool ProcessClearChar();
  virtual bool PostProcessClear();
  
  bool ReverseClear(const BStringC &ClearStr); //add for qwert: delete ClearStr from end of the content
   
public:
  void Reset();
  
  void SetMaxSize(uint32 MaxSize);
  uint32 GetMaxSize() const;

  bool IsReachedMaxSize();
  int32 GetRemainingSpace(const BStringC &String);

  uint16 GetNumOfChars();
  
private:
  virtual void ClearToStartPosition();

// Text operations
private:
  uint16 GetStringWidth(const BStringC &String, SizeT CharIndex, SizeT NumOfChars);
  uint16 GetLatinTextWidth(const char * TextP, uint16 Length);
  uint16 GetUnicodeTextWidth(uint8 * TextP, uint16 Length);

  BStringC * GetUnicodeText();

  uint16 GetFontHeight(DCC &Dc);
public:
  virtual void GetText(BStringC &String);
  void GetText(BStringC &String, uint16 &CaretOffset);
  
  void SetText(const BStringC &String);
  void SetText(RESOURCE_ID(StringT) StringResId);
  void SetText(const StringT *StringP);

  // Code Page
  void SetLatinMode(EditCodePageLatinModeT LatinMode);
  EditCodePageLatinModeT GetLatinMode();
  uint32 GetMaxCharNumber();

  bool IsAsciiChar(uint16 Char);

// Line information
protected:
  void AllocateLinesInfo(uint16 StartLine);
  LinesInfoC* GetLinesInfo();
private:
  LinesInfoC mLinesInfo;
  void SetFirstVisibleLine();
  void SetVisibleLines();
  void SetLinesInfo();

  bool FillLinesInfoItems(uint16 StartLine);
  bool FillLinesInfoItemsChar(uint16 StartLine);
  bool ProcessFillLinesInfoItems(uint16 StartLine);
  bool ProcessFillLinesInfoItemsChar(uint16 StartLine);
public:
  uint16 GetLineCount() const;
  uint16 GetVisibleLines() const;
  uint16 GetFirstVisibleLine() const;
  uint16 GetCurrentLine() const;
  int16 GetLineIndex(int16 CaretOffset);

  void LineScroll(int16 Lines);
  void ScrollToFirstPage(DispVAlignT CaretVAlign);

// Appearance
protected:
  AppearanceC mAppearance;
  
public:
  void GetRect(RectC *RectP) const;
  //void SetRect(const RectC &Rect, bool Redraw = FALSE);

  uint16 GetValidTextRectWidth();
  
  // Set/Get margins
  void SetLeftMargin(uint16 LeftMargin);
  void SetRightMargin(uint16 RightMargin);
  uint16 GetLeftMargin() const;
  uint16 GetRightMargin() const;
  uint16 GetMargin() const;
  
// Message notify
protected:  
  void LoseFocus(uint8 Direction);
  
private:
  bool mClearSomething;
  bool mClearRepeat;
  BTimerC mClearTimer;

  void NotifyEvent(EditEventT EditEvent);

  // Data upated
private:
  bool mNotifyDataUpdated;
  void ProcessUpdateContent(UpdateContentFlagT Flag);

public:
  void SetNotifyDataUpdated(bool Enable);
  bool GetNotifyDataUpdated();
  
  void NotifyTextFullEvent();
  void NotifyTextOverflowEvent();

  // Resize
private:
  ResizeInfoT mResizeInfo;

  bool  CanBeResizabled();
  bool  Resize(bool Fit);
  
// Modification flag
private:
  bool mModified;
  
public:
  bool GetModify() const;
  void SetModify(bool Modified = TRUE);

// preview mode
private:
  PreviewInfoT mPreviewInfo;

  void DrawPreview(DCC *DcP);

public:
  void SetPreviewMode(PreviewModeT PreviewMode);
  PreviewModeT GetPreviewMode();

// IME management
protected:
  void InitIme(bool Reset = TRUE);
  
private:
  static ImeAreaT mDefaultImeArea;
  //ImeModeT mImeMode;
  uint32   mImeGroup;

  void CreateImeManager();
  uint32 GetImeGroup();
  
protected:
   BImeManagerC *mImeManager;

public:
  void ImeChanged();
  BStringC * GetCurrentImeName();
  void GetImeNameList(BVectorC &NameList);
  void SetIme(const BStringC &ImeName);
  //bool SetImeMode(ImeModeT ImeMode) { return TRUE;}
  
  void AutoCapital(bool Auto);

  void SetImeGroup(uint32 ImeGroup);
  static void InitImeArea(ImeAreaT ImeArea = IME_AREA_GLOBAL);

  // IME candidate, using IMECandidateDialogC

// Key operations
protected:
  virtual void ProcessUpDownKey(bool Down);
  void ProcessLeftRightKey(bool Right);
  virtual void ProcessVolumeUpDownKey(bool Down);
private:
  void ProcessTurnPage(TurnPageFlagT flag);
  void ProcessSymbolKey(bool Down);
  
// Draw operations
protected:
  void DrawTextArea(DCC *DcP);
  void DrawTextRect(const RectC &Rect, DCC *DcP);
  virtual void DrawContent(DCC *DcP);
  virtual void SetTextRect();
  virtual bool PreProcessReverseDisplay();
  virtual bool ProcessReverseDisplay(DCC *DcP, uint16 Line, BStringC &String, uint8 Begin);

private:
  virtual void Draw(DCC *DcP);
  void DrawNormal(DCC *DcP);
  void DrawSingleLine(DCC *DcP);

  void DrawLine(uint16 Line, DCC *DcP);
  RectC  GetLineRect(uint16 Line);
  uint16 GetLineRectY(uint16 Line);

  uint16 GetDisplayWidth(PositionT PosStart, PositionT PosEnd);
  uint16 GetLinesHeight(uint16 Start, uint16 End);    
  
  void DrawString(DCC *DcP, BStringC &String, 
                  SizeT CharIndex, SizeT NumOfChars, BalDispRectT *RectP);
  void DrawLatinText(DCC *DcP, const char *TextP, uint16 Length, BalDispRectT *RectP);
  void DrawUnicodeText(DCC *DcP, uint8 *TextP, uint16 Length, BalDispRectT *RectP);
  void DrawBackGroundImage(DCC * DcP);
  
public:
  void SetDrawTransparent(bool Transparent = TRUE);

private:
  BalDispColorT mBKColor;
  BalDispColorT mFocusBKColor;

public:
  BalDispFontT SetEditFont(BalDispFontT TextFont);
  BalDispFontT GetEditFont();

  BalDispColorT SetEditTextColor(BalDispColorT TextColor);
  BalDispColorT GetEditTextColor();

  BalDispColorT SetBKColor(BalDispColorT BKColor);
  BalDispColorT GetBKColor();

  BalDispColorT SetFocusBKColor(BalDispColorT FocusBKColor);
  BalDispColorT GetFocusBKColor();

  BalDispColorT SetBorderColor(BalDispColorT BorderColor);
  BalDispColorT GetBorderColor();
  uint16     GetBorderWidth();

  BalDispModeT SetBKMode(BalDispModeT BKMode);
  BalDispModeT GetBKMode();
  
// Style
private:
  char mPasswordChar;

  void FillWithPasswordChar(BStringC &String);

public:  
  char GetPasswordChar() const;
  void SetPasswordChar(char Ch);

  uint16 GetPasswordCharWidth();

  bool SetReadOnly(bool ReadOnly = TRUE);
  bool IsReadOnly();

  bool IsLeftAlign();
  bool IsRightAlign();
  bool IsCenterAlign();

  bool IsMultiLine();

  bool InNumberNumericStyle();

  bool HavingBorder();

// CallNumber Style
private:
  CallNumberStyleT mCallNumStyle;

  bool AdjustCallNumberStyleFont(bool &LineCalculated);

public:
  void SetCallNumberStyleTopdown(bool Topdown);
  bool GetStyleTopdown();
  void SetCallNumberStyleAutoFont(bool AutoFont);
  bool GetStyleAutoFont();
  void SetCallNumberStyleCaretShow(bool CaretShow);
  bool GetCallNumberStyleCaretShow();
  
// Hyphen
private:
  bool mHyphen;
  
  void AutoHyphenize();
  bool AddHyphen(BStringC &String, uint16 &CaretOffset);
  bool RemoveHyphen(BStringC &String, uint16 &CaretOffset);

public:
  void SetHyphenEnable(bool Enable);
  
// Caret
protected:
  virtual bool ToMoveOut(bool Down);
  virtual void ProcessMoveCaretV(int16 Lines);
  void SetCaretInfo();
  CaretInfoT* GetCaretInfo();
  
private:
  CaretInfoT mCaretInfo;
  bool mDynamicOffset;
  bool mLoseFocus;
  uint16 mLoseFocusLine;
  void CreateCaret();
  void SetCaretLine();
  
  int  ComparePos(const PositionT &Pos1, const PositionT &Pos2);

  void DrawCaret();
  virtual bool GetCaretRect(RectC &Rect);
  
protected:
  virtual uint16& GetCaretOffset();
  virtual uint16 SetCaretOffset(uint16 CaretOffset);
  virtual uint16& GetLastCaretOffset();
  virtual uint16 SetLastCaretOffset(uint16 CaretOffset);
  virtual uint16 ResetCaretOffset(); 
  virtual bool IsCaretAtStartPosition();

  void ClearCaretInfo();
  void SetCaretPosition();

public:
  void ShowCaret();
  void HideCaret();
  uint16 GetEditCaretOffset();
  bool SetEditCaretOffset(uint16 CaretOffset);
  bool SetDynamicOffset(bool Enable = TRUE);

  virtual void MoveCaretH(int16 Chars);
  virtual void MoveCaretV(int16 Lines);
  
// ScrollBar
private:
  bool mShowScrollBar;
  ScrollBarControlC *mScrollBarP;

  void CreateScrollBar();
  void SetScrollBarRect();
  RectC  GetScrollBarRect();
  uint16 GetScrollBarWidth();

// Title
private:
  bool mShowTitle;
  StaticTextCtrlC *mTitleP;

  void UpdateTitle();
  void CreateTitle(RESOURCE_ID(StaticTextCtrlResT) TitleId);

  void SetTitleRect();
  uint16 GetTitleHeight();
  bool HavingTitleArea();
  void SetTitleAreaStyle(StaticTextCtrlC *StaticTextP, BalDispHAlignT HAlign);
  
public:
  void SetTitle(const BStringC & String);
  void SetTitle(RESOURCE_ID(StringT) StringResId);
  void SetTitle(const RichStringC &RichString);

  RichStringC *GetTitle();
  BStringC GetTitleText();

// Input method box
private:
  bool mShowIMEBox;
  StaticTextCtrlC *mIMEBoxP;

  void DrawIMEBox();
  void SetIMEBoxRect();
  void CreateIMEBox();
  
public:
  void SetIMEBoxEnable(bool Enable);
  
// Input number box
private:
  bool mIsImeIndicatorBound;
  bool mShowNumberBox;
  StaticTextCtrlC *mNumberBoxP;

  void DrawNumberBox();
  void SetNumberBoxRect();
  void CreateNumberBox();
  
public:
  void SetNumberBoxEnable(bool Enable);
  void UnbindImeIndicator(uint16 x, uint16 y);

  bool OpenSysbolDilog(void);
  bool CloseSysbolDilog(void);

//Add by lizhongyi 080703
//Mark add a character is success or not.
private:
  bool mAddChar;
public:
  void setAddChar(bool AddChar);
  bool getAddChar(void) const;

// Clipboard operations
public:
  typedef enum
  {
    MARK_STATE_NONE,
    MARK_STATE_WAIT,
    MARK_STATE_DONE
  } MarkStateT;

private:
  MarkStateT mMarkState;
  
  uint16 mStartPos;

  bool GetSelText(BStringC& SelText);
  bool GetSelRange(uint16& StartPos, uint16& EndPos);

public:
  virtual void MarkBegin();
  uint8 GetMarkState();
  void MarkStartPos(bool SelectAll);
  void MarkEnd();

  void Clear(); 
  void Copy();
  void Cut();
  void Paste(); 

//Editor options
public:
    void GetEditorMenuItems(BVectorC& MenuVec, WinHandleT OptionDialogHandle);
// Message Handler
protected:
  bool OnNotify(uint32 ParamA, int32 ParamB);
  void OnParentNotify(uint32 Message, int32 Param);  
  void OnShowNotify(void);
  void OnHideNotify(void);
  
  int32 OnCreate(void);
  void OnDestroy(void);

  void OnMove(int16 X, int16 Y);
  void OnSize(int16 Width, int16 Height);

  void OnEnable(bool Enable);

  void OnShow(bool Show);  
  bool OnEraseBkgnd(void);

  void OnSetFocus(WindowC* OldWinP);  
  void OnKillFocus(WindowC* NewWinP);
  
  bool OnKeyPress(uint32 KeyCode);
  bool OnKeyRelease(uint32 KeyCode);
  bool OnKeyHold(uint32 KeyCode);

  // Edit control self-defined message handler
  void OnClearAll();
  void OnClearRepeat();
  void OnCaretBlink();
  void OnQwertImeClose();
  void InputSymbol();

  //edit control option msg handler
private:
  BVectorC EditorDlgStack;
  void ProcessEditMsg(uint32 MailMsgId, void* param);

  void SetListDialogMenuItemParam(ListDialogC* ListDlgP);
  void CloseEditorDialog(int NumToClose);
  ListDialogC* OpenEditorListDialog(RESOURCE_ID(BYDMenuCtrlDataT) DlgResId, RESOURCE_ID(StringT) TitleResId);
  PopupDialogC* OpenEditorPopupDialog(DlgStyleT DlgStyle, DlgPZoneT nZoneId, ResourceIdT ResourceId);
  PopupDialogC* OpenEditorPopupDialog(DlgStyleT DlgStyle, DlgPZoneT nZoneId, void *value);
/* add for qwert: auto fill */
private:
  bool mInitCCEditor;
  uint16 mMaxVisibleItem;

protected:
  MenuCtrlC *mMenuP;
  void AdjustLayout();
  bool ProcessMenuKeyCode(uint32 keycode);  
public:
  /* init menuctrl uesd in cceditor*/
  bool InitCCEditor(uint16 MaxVisibleItem = 3);
  bool SpreadList();
  /* clear all the items in list*/
  bool ClearList();
  /* close the list: clearlist() then updatewindow */
  bool CloseList();
  bool InsertItem(BStringC &str, uint16 index);
  bool DeleteItem(uint16 index);
  int32 GetSelectItemIndex();
  bool GetSelectItem(BStringC &str);
  /* the state of list: invisible or the item count is 0 will return false */
  bool IsSpread();
/*begin--wrd--add for scroll key in hold case20081018*/
private:
  bool mKeyScroll;
  uint8 mSymbolKeyNum;
  uint8 mSymbolCount;
  int8 mSymbolLen;
  char* mSymbolInsert;
  ScrollSymMapT* mScrollSymbol;
/*end--wrd20081018*/

  #ifdef VIN_AUTOCORRECT
  public:
  int32 StartCaretIndex,EndCaretIndex;   //gyq
  #endif
  DECLARE_MAIL_MAP()
};

inline uint16 EditCtrlC::GetDataItemsCount()
{
  return 1;
}

inline uint16 EditCtrlC::GetNumOfChars()
{
  BStringC String(mContent.String);
  uint16 CaretOffset = 0;

  RemoveHyphen(String, CaretOffset);

  return String.GetNumOfChars();
}

inline BStringC * EditCtrlC::GetData()
{
  return &(mContent.String);
}

inline bool EditCtrlC::IsEmpty()
{
  return (bool)(IsEqual(mContent.String.GetLength(), 0));
}


inline void EditCtrlC::Reset()
{
  Clear(mContent.String.GetNumOfChars());
}


inline void EditCtrlC::SetMaxSize(uint32 MaxSize)
{
  mContent.MaxSize = MaxSize;

  DrawNumberBox();
}

inline uint32 EditCtrlC::GetMaxSize() const
{
  return mContent.MaxSize;
}

inline void EditCtrlC::SetLatinMode(EditCodePageLatinModeT LatinMode)
{
  mContent.LatinMode = LatinMode;

  DrawNumberBox();
}

inline EditCodePageLatinModeT EditCtrlC::GetLatinMode()
{
  return mContent.LatinMode;
}

inline bool EditCtrlC::IsAsciiChar(uint16 Char)
{
  return (bool)(Char < 256);
}

inline LinesInfoC* EditCtrlC::GetLinesInfo()
{
  return &mLinesInfo;
}

inline uint16 EditCtrlC::GetLineCount() const
{
  return mLinesInfo.ValidLines;
}

inline uint16 EditCtrlC::GetVisibleLines() const
{
  return mLinesInfo.VisibleLines;
}

inline uint16 EditCtrlC::GetFirstVisibleLine() const
{
  return mLinesInfo.FirstVisibleLine;
}

inline uint16 EditCtrlC::GetCurrentLine() const
{
  return mCaretInfo.CurrentLine;
}

inline void EditCtrlC::GetRect(RectC *RectP) const 
{
  UIASSERT(!IsNull(RectP));
  
  // get the editable rect of a edit control.
  memcpy((void *)RectP, (void *)&mAppearance.TextRect, sizeof(mAppearance.TextRect));
}
  
inline uint16 EditCtrlC::GetValidTextRectWidth()
{
  return mAppearance.TextRect.dx - GetBorderWidth() - GetMargin()
           - GetScrollBarWidth();
}

// Set/Get margins
inline void EditCtrlC::SetLeftMargin(uint16 LeftMargin)
{
  if((LeftMargin >= 0) && (mWinRect.dx > LeftMargin)) //check the leftmargin
     mAppearance.LeftMargin = LeftMargin;
  AllocateLinesInfo(0);
}

inline void EditCtrlC::SetRightMargin(uint16 RightMargin)
{
  if((RightMargin >= 0)&& (mWinRect.dx > RightMargin)) //check the rightmargin
     mAppearance.RightMargin = RightMargin;
  AllocateLinesInfo(0);
}

inline uint16 EditCtrlC::GetLeftMargin() const
{
  return mAppearance.LeftMargin;
}

inline uint16 EditCtrlC::GetRightMargin() const
{
  return mAppearance.RightMargin;
}

inline uint16 EditCtrlC::GetMargin() const
{
  return mAppearance.LeftMargin + mAppearance.RightMargin;
}

inline void EditCtrlC::SetNotifyDataUpdated(bool Enable)
{
  mNotifyDataUpdated = Enable;
}

inline bool EditCtrlC::GetNotifyDataUpdated()
{
  return mNotifyDataUpdated;
}

inline bool EditCtrlC::CanBeResizabled()
{
  return (bool)(mResizeInfo.MaxHeight > mResizeInfo.MinHeight);
}

// Modification flag
inline bool EditCtrlC::GetModify() const
{
  // Determines whether the contents have been modified
  return mModified;
}

inline void EditCtrlC::SetModify(bool Modified /*= TRUE*/)
{
  // Sets or clears the modification flag
  mModified = Modified;
}

inline void EditCtrlC::SetPreviewMode(PreviewModeT PreviewMode)
{
  mPreviewInfo.Mode = PreviewMode;
}

inline PreviewModeT EditCtrlC::GetPreviewMode()
{
  return mPreviewInfo.Mode;
}

inline void EditCtrlC::SetDrawTransparent(bool Transparent /*= TRUE*/)
{
  if (Transparent)
  {
    mStyle |= WINSTYLE_TRANSPARENT;
  }
  else
  {
    mStyle &= ~WINSTYLE_TRANSPARENT;
  }
}

inline BalDispFontT EditCtrlC::SetEditFont(BalDispFontT TextFont)
{
  if(TextFont>=BAL_DISP_FONT_TOTAL || TextFont<BYD_FONT_NORMAL_A)
  	return (mDc.GetFont()).Type;
  return mDc.SetFont(FontC(TextFont)).Type;
}

inline BalDispFontT EditCtrlC::GetEditFont()
{
  return mDc.GetFont().Type;;
}

inline BalDispColorT EditCtrlC::SetEditTextColor(BalDispColorT TextColor)
{
  return mDc.SetTextColor(TextColor);
}

inline BalDispColorT EditCtrlC::GetEditTextColor()
{
  return mDc.GetTextColor();
}

inline BalDispColorT EditCtrlC::SetBKColor(BalDispColorT BKColor)
{
  BalDispColorT OldColor = mBKColor;
  if (BKColor != BAL_DISP_INVALID_COLOR  )
      mBKColor = BKColor;
  return OldColor;
}

inline BalDispColorT EditCtrlC::GetBKColor()
{
  return mBKColor;
}

inline BalDispColorT EditCtrlC::SetFocusBKColor(BalDispColorT FocusBKColor)
{
  BalDispColorT OldColor = mFocusBKColor;
  if (FocusBKColor != BAL_DISP_INVALID_COLOR )
      mFocusBKColor = FocusBKColor;
  return OldColor;
}

inline BalDispColorT EditCtrlC::GetFocusBKColor()
{
  return mFocusBKColor;
}

inline BalDispColorT EditCtrlC::SetBorderColor(BalDispColorT BorderColor)
{
  return mDc.SetPen(PenC(BorderColor, BAL_DISP_PEN_1)).Color;
}

inline BalDispColorT EditCtrlC::GetBorderColor()
{
  return mDc.GetPen().Color;
}

inline uint16 EditCtrlC::GetBorderWidth()
{
  return (mStyle & EDIT_STYLE_BORDER)? (mDc.GetPen().Type - BAL_DISP_PEN_0): 0;
}

inline BalDispModeT EditCtrlC::SetBKMode(BalDispModeT BKMode)
{
 if(BKMode >=BAL_DISP_MODE_TOTAL)
  	return  BAL_DISP_MODE_TRANSPARENT;
 
  return mDc.SetBackGroundMode(BackGroundModeC(BKMode, BAL_DISP_RGB_WHITE_565)).Type;
}

inline BalDispModeT EditCtrlC::GetBKMode()
{
  return mDc.GetBackGroundMode().Type;
}

inline char EditCtrlC::GetPasswordChar() const
{
  // retrieve the password character.
  // The return value is NULL if no password character exists.
  return (GetStyle() & EDIT_STYLE_PASSWORD)
            ? mPasswordChar
            : 0;
}

inline uint16 EditCtrlC::GetPasswordCharWidth()
{
  return (GetStyle() & EDIT_STYLE_PASSWORD)
            ? GetLatinTextWidth(&mPasswordChar, 1)
            : 0;
}

inline bool EditCtrlC::IsReadOnly()
{
  return ((GetStyle() & EDIT_STYLE_READONLY)? TRUE: FALSE);
}

inline bool EditCtrlC::IsLeftAlign()
{
  return (bool)(!(GetStyle() & (EDIT_STYLE_RIGHT | EDIT_STYLE_CENTER)));
}

inline bool EditCtrlC::IsRightAlign()
{
  return ((GetStyle() & EDIT_STYLE_RIGHT)? TRUE: FALSE);
}

inline bool EditCtrlC::IsCenterAlign()
{
  return ((GetStyle() & EDIT_STYLE_CENTER)? TRUE: FALSE);
}
  
inline bool EditCtrlC::IsMultiLine()
{
  return ((GetStyle() & EDIT_STYLE_MULTILINE)? TRUE: FALSE);
}
  
inline bool EditCtrlC::InNumberNumericStyle()
{
  return (((GetStyle() & EDIT_STYLE_CALL) ||
           (GetStyle() & EDIT_STYLE_NUMERIC) ||
           (GetStyle() & EDIT_STYLE_NUMBER))
           ? TRUE: FALSE);
}

inline bool EditCtrlC::HavingBorder()
{
  return ((GetStyle() & EDIT_STYLE_BORDER)? TRUE: FALSE);
}

inline void EditCtrlC::SetCallNumberStyleTopdown(bool Topdown)
{
  mCallNumStyle.Topdown = Topdown;
}

inline bool EditCtrlC::GetStyleTopdown()
{
  return ((GetStyle() & EDIT_STYLE_CALL)? mCallNumStyle.Topdown: TRUE);
}

inline void EditCtrlC::SetCallNumberStyleAutoFont(bool AutoFont)
{
  mCallNumStyle.AutoFont = AutoFont;
}

inline bool EditCtrlC::GetStyleAutoFont()
{
  return ((GetStyle() & EDIT_STYLE_CALL)? mCallNumStyle.AutoFont: FALSE);
}

inline void EditCtrlC::SetCallNumberStyleCaretShow(bool CaretShow)
{
  mCallNumStyle.CaretShow = CaretShow;
}

inline bool EditCtrlC::GetCallNumberStyleCaretShow()
{
  return mCallNumStyle.CaretShow;
}

inline CaretInfoT* EditCtrlC::GetCaretInfo()
{
  return &mCaretInfo;
}
  
inline uint8 EditCtrlC::GetMarkState()
{
  return (uint8)mMarkState;
}

inline void EditCtrlC::setAddChar(bool AddChar)
{
    mAddChar = AddChar;
}

inline bool EditCtrlC::getAddChar(void) const
{
    return mAddChar;
}

#endif

