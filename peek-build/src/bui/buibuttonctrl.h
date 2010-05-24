

#ifndef UIBUTTONCTRL_H
#define UIBUTTONCTRL_H 


#include "buiwindow.h"
#include "buicontrol.h"
#include "buimsgids.h"

#include "restypedefines.h"

//! Invalid color value which will be used to decide whether the for color has been set.
#define INVALID_COLOR (-2)

//! Button display color which will be defined in uitheme.c
typedef struct
{
  BalDispColorT TextColor;      //!< Text Color
  BalDispColorT BorderColor;    //!< Border Color, just like PenColor in DC
  BalDispColorT BKColor;        //!< Background Color, just like BrushColor in DC
}ButtonCtrlColorsT;

//! Button control class define
class ButtonCtrlC : public ControlC
{
public:
  ButtonCtrlC(void);
  virtual ~ButtonCtrlC(void);
  
  void SetTitle(const BStringC &Title);
  void SetTitle(RESOURCE_ID(StringResT) TitleID);
  bool GetTitle(BStringC &Title);
  
  void SetMessageId(uint32 MessageId);
  uint32 GetMessageId(void);
  
  void SetMessageParam(uint32 Parameter);
  uint32 GetMessageParam(void);
  
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

  /* add by wanglili 20080508*/
  void SetEnabled(bool Enabled);
  /* added by wanglili 20080618 */
  void SetBorder(bool EnableBorder);
  /* added by wanglili 20080627 */
  void SetDisabledImage(ResourceIdT ImageResId);
  /* added by wanglili 20080718 to set focus text color*/
  void SetFocusTextColor(int32 Color);
  
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
  BStringC                 mTitle;
  RESOURCE_ID(ImageResT)  mIcon;
  uint8                   mDisplayMode;
  uint32                  mMessageId;
  uint32                  mParameter;
  bool                    mIconIsDynamic;
  int32                   mBGColor;
  int32                   mFGColor;
  bool                    mIsPressed;
  BalDispFontT               mFont;
  BalDispHAlignT      mAlignMode;

  /* add by wanglili 20080508*/
  bool 			mEnabled;

/* added by wanglili 20080618 */
  bool mEnableBorder;
  /* added by wanglili 20080527*/
  ImageC *mDisabledImage;

  /* added by wanglili 20080718 to set focus text color*/
  int32 mFocusTextColor;

  void DrawIcon(DCC *DcP,BalDispRectT Rect,BalDispBitmapT Bmp); 
  void TranslateMsg();
  bool GetBitmap(BalDispBitmapT &Bmp);

  DECLARE_MAIL_MAP()
  
};

inline bool ButtonCtrlC::GetTitle(BStringC &Title)
{
  //Title = mTitle;
  Title.Empty();
  Title += mTitle;
  return TRUE;
}

inline void ButtonCtrlC::SetMessageId(uint32 MessageId)
{
  mMessageId = MessageId;
  TranslateMsg();
}

inline uint32 ButtonCtrlC::GetMessageId()
{
  return mMessageId;
}

inline void ButtonCtrlC::SetMessageParam(uint32 Parameter)
{
  if(!IsEqual(mMessageId,WINMSG_KEYPRESS))
  {
    mParameter = Parameter;
  }
}

inline uint32 ButtonCtrlC::GetMessageParam()
{
  return mParameter;
}

inline RESOURCE_ID(ImageResT) ButtonCtrlC::GetIcon() 
{
  return mIcon;
}

inline void ButtonCtrlC::SetFont(BalDispFontT Font)
{
  mFont = Font;
}

inline BalDispFontT ButtonCtrlC::GetFont()
{
  return mFont;
}

inline void ButtonCtrlC::SetBGColor(int32 Color)
{
  mBGColor = Color;
}

inline int32 ButtonCtrlC::GetBGColor()
{
  return mBGColor;
}

inline void ButtonCtrlC::SetFGColor(int32 Color)
{
  mFGColor = Color;
}

inline int32 ButtonCtrlC::GetFGColor() 
{
  return mFGColor;
}

inline void ButtonCtrlC::SetAlignMode(BalDispHAlignT Mode)
{
  mAlignMode = Mode;
}

inline BalDispHAlignT ButtonCtrlC::GetAlignMode()
{
  return mAlignMode;
}







#endif
