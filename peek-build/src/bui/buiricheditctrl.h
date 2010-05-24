
#ifndef UIRICHEDITCONTROL_H
#define UIRICHEDITCONTROL_H 


#include "buieditctrl.h"
#include "buicontentviewer.h"
#include "buicontentcontainer.h"
#include "buidcc.h"
#include "buicaret.h"


class RichEditCtrlC : public EditCtrlC
{
public:
  RichEditCtrlC();
  virtual ~RichEditCtrlC();
  
  ContentViewerC * GetViewer() {return mViewer;}
  ContentContainerC * GetModel() { return mModel;}
  ContentViewerC * SetViewer(ContentViewerC * viewer);
  ContentContainerC * SetModel(ContentContainerC * model);

  virtual bool ProcessAdd(const BStringC &String);
  virtual bool ProcessClear(uint16 Count);
  virtual bool Replace(const BStringC &ReplaceString, uint16 CountCleared);

  virtual bool IsEmpty();
  virtual void GetText(BStringC &String);

  virtual bool IsCaretAtStartPosition();
  virtual void MoveCaretH(int16 Chars);
  
// Data information
private:
  ContentContainerC * mModel;
  ContentViewerC * mViewer;

private:
  //virtual void Draw(DCC *DcP);
  //virtual void DrawTextArea(DCC *DcP);
  virtual void DrawContent(DCC *DcP);
  virtual void ClearToStartPosition();
  virtual bool GetCaretRect(RectC &Rect);
  virtual bool ToMoveOut(bool Down);
  virtual void ProcessMoveCaretV(int16 Lines);

  virtual void ProcessVolumeUpDownKey(bool Down);
  
protected:
  virtual bool LoadResource(ResourceIdT ResourceId);
  virtual void SetTextRect();

private:
  void AdjustCursorGlyphIndex(int16 AddedGlyphCount);
  void AdjustDisplayWindowPos(uint16 CursorGlyphIndex, bool NeedLayout = TRUE);

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

  //void OnSetFocus(WindowC* OldWinP){}  
  //void OnKillFocus(WindowC* NewWinP){}
  
  //bool OnKeyPress(uint32 KeyCode);
  //bool OnKeyRelease(uint32 KeyCode);
  
  // RichEdit control self-defined message handler
  void OnClearAll();
  void OnCaretBlink();

  DECLARE_MAIL_MAP()
};



#endif

