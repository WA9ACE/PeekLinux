#ifndef UICHECKBOXCTRL_H
#define UICHECKBOXCTRL_H 


#include "buiwindow.h"
#include "buicontrol.h"
#include "buimsgids.h"

#include "restypedefines.h"




//! Invalid color value which will be used to decide whether the for color has been set.
#define INVALID_COLOR (-2)


//! Checkbox control class define
class CheckboxCtrlC: public ControlC
{
public:
  CheckboxCtrlC(void);
  virtual ~CheckboxCtrlC(void);
  
  void SetLabel(const BStringC &Label);
  void SetLabel(RESOURCE_ID(StringResT) LabelID);
  bool SetLabel(BStringC &Label);
  bool GetLabel(BStringC &Label);
  

  void SetIcon(RESOURCE_ID(ImageResT) IconId);
  RESOURCE_ID(ImageResT) GetIcon(void);
  
  void SetFont(BalDispFontT Font);
  BalDispFontT GetFont(void);
  
  void SetBGColor(int32 Color);
  int32 GetBGColor(void); 

  void SetFGColor(int32 Color);
  int32 GetFGColor(void); 

  void SetTransparent(bool Transparent);
  bool GetTransparent(void);

  void SetAlignMode(BalDispHAlignT Mode);
  BalDispHAlignT GetAlignMode(void);

  void SetCheckState(bool checked = FALSE){    mIsChecked = checked;  };
  bool IsChecked(){    return mIsChecked; };
  
/*! \cond private */
protected:
  virtual bool LoadResource(ResourceIdT ResourceId);
  virtual void Draw(DCC *DcP);

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
/*! \endcond */  

private:
  BStringC                 mLabel;
  RESOURCE_ID(ImageResT)  mIcon;
  uint8                   mDisplayMode;
  uint32                  mMessageId;
  uint32                  mParameter;
  bool                    mIconIsDynamic;
  int32                   mBGColor;
  int32                   mFGColor;
  bool                    mIsPressed;
  BalDispFontT               mFont;
  BalDispHAlignT      mHAlignMode;
  bool 			mIsChecked;
  bool			mShowCheckState;
  RESOURCE_ID(ImageResT)		mCheckedIcon;
  RESOURCE_ID(ImageResT)		mUncheckedIcon;
  BalDispHAlignT	mIconAlignMode;
  ImageC* mCheckImage;
  

  void DrawIcon(DCC *DcP,BalDispRectT Rect,BalDispBitmapT Bmp); 
  bool GetBitmap(BalDispBitmapT &Bmp);

  DECLARE_MAIL_MAP()
  
};


inline bool CheckboxCtrlC::GetLabel(BStringC &Label)
{
  Label.Empty();
  Label += mLabel;
  return TRUE;
}


inline RESOURCE_ID(ImageResT) CheckboxCtrlC::GetIcon() 
{
  return mIcon;
}

inline void CheckboxCtrlC::SetFont(BalDispFontT Font)
{
  mFont = Font;
}

inline BalDispFontT CheckboxCtrlC::GetFont()
{
  return mFont;
}

inline void CheckboxCtrlC::SetBGColor(int32 Color)
{
  mBGColor = Color;
}

inline int32 CheckboxCtrlC::GetBGColor()
{
  return mBGColor;
}

inline void CheckboxCtrlC::SetFGColor(int32 Color)
{
  mFGColor = Color;
}

inline int32 CheckboxCtrlC::GetFGColor() 
{
  return mFGColor;
}

inline void CheckboxCtrlC::SetAlignMode(BalDispHAlignT Mode)
{
  mHAlignMode = Mode;
}

inline BalDispHAlignT CheckboxCtrlC::GetAlignMode()
{
  return mHAlignMode;
}





#endif
