

#ifndef UISOFTKEYBARCTRL_H
#define UISOFTKEYBARCTRL_H 


#include "buivector.h"
#include "buiwindow.h"
#include "buicontrol.h"
#include "buibuttonctrl.h"

//! SoftkeyBar display color which will be defined in uitheme.c
typedef struct
{
  BalDispColorT BorderColor;    //!< Border Color, just like PenColor in DC
  BalDispColorT BKColor;        //!< Background Color, just like BrushColor in DC
}SoftkeyBarCtrlColorsT;

typedef enum
{
  SOFTKEYBAR_LSK,
  SOFTKEYBAR_MSK,
  SOFTKEYBAR_RSK,
  SOFTKEYBAR_MAX
} SoftkeyType;

//! Return value
enum SoftkeyBarErrorT
{
  SOFTKEYBAR_SUCCESS,           //!< Operation success
  SOFTKEYBAR_OUT_OF_BOUND,      //!< Index is larger than SOFTKEYBAR_BUTTON_MAX
  SOFTKEYBAR_BUTTON_NOT_EXIST,  //!< Button does not exist(ex: ButtonP parameter is NULL)
  SOFTKEYBAR_BUTTON_INVALID     //!< Index is larger than the size of button list
};

#define SOFTKEYBAR_BUTTON_MAX        3 //!< The max size of button list which allowed

//! SoftkeyBar control class define
class SoftkeyBarCtrlC : public ControlC
{

public:
  SoftkeyBarCtrlC(void);
  virtual ~SoftkeyBarCtrlC(void);
  
  SoftkeyBarErrorT SetButtonTitle(uint8 ButtonIndex, const BStringC &Title);
  SoftkeyBarErrorT SetButtonTitle(uint8 ButtonIndex, RESOURCE_ID(StringResT) TitleID);
  bool GetButtonTitle(uint8 ButtonIndex, BStringC &Title);
  
  SoftkeyBarErrorT SetButtonMessageId(uint8 ButtonIndex, uint32 MessageId);
  uint32 GetButtonMessageId(uint8 ButtonIndex);
  
  SoftkeyBarErrorT SetButtonMessageParam(uint8 ButtonIndex, uint32 Parameter);
  uint32 GetButtonMessageParam(uint8 ButtonIndex);
  
  SoftkeyBarErrorT SetButtonIcon(uint8 ButtonIndex, RESOURCE_ID(ImageResT) Icon);
  RESOURCE_ID(ImageResT) GetButtonIcon(uint8 ButtonIndex);
  
  SoftkeyBarErrorT SetButtonFont(uint8 ButtonIndex, BalDispFontT Font);
  BalDispFontT GetButtonFont(uint8 ButtonIndex);

  SoftkeyBarErrorT SetButtonRect(uint8 ButtonIndex, BalDispRectT Rect);
  BalDispRectT GetButtonRect(uint8 ButtonIndex);

  SoftkeyBarErrorT SetButtonFocus(uint8 ButtonIndex);
  bool GetButtonFocus(uint8 ButtonIndex);
  SoftkeyBarErrorT SetButtonProp(uint8 ButtonIndex, const BStringC &Title,uint32 MessageId, uint32 Parameter);
  ButtonCtrlC *GetButton(uint8 ButtonIndex);
  
  void SetAlignMode(BalDispHAlignT Mode);
  BalDispHAlignT GetAlignMode(void);
  
  void SetBGColor(int32 Color);
  int32 GetBGColor(void);

  uint8 GetButtonTotal(void);

  SoftkeyBarErrorT AppendButton(ButtonCtrlC *ButtonP);
  SoftkeyBarErrorT InsertButton(uint8 Index,ButtonCtrlC *ButtonP);
  SoftkeyBarErrorT InsertButton(uint8 Index,RESOURCE_ID(ButtonCtrlResT) ButtonID);
  SoftkeyBarErrorT RemoveButton(uint8 Index);
  SoftkeyBarErrorT GetSoftkeyInfo(SoftkeyType etType, BStringC& strText, uint32& dwMsgId, uint32& dwParam);
  SoftkeyBarErrorT SetSoftkeyInfo(SoftkeyType etType, BStringC& strText, uint32 dwMsgId, uint32 dwParam);
  SoftkeyBarErrorT SetSoftkey(RESOURCE_ID(SoftkeyBarCtrlResT) ResourceId);
  SoftkeyBarErrorT SetSoftkey(SoftkeyType etType, RESOURCE_ID(ButtonCtrlResT) ResourceId);
  SoftkeyBarErrorT SetSoftkey(SoftkeyType etType, BStringC& strText, uint32 dwMsgId = 0, uint32 dwParam = 0);

  void SetTransparent(bool Transparent);
  bool GetTransparent(void);

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
  uint8                   mFocusButtonIndex;
  BalDispHAlignT             mAlignMode;
  BVectorC                 mButtonArray;
  int16                   mBGColor;
  bool                    mIsPressed;
  
  void ReArrangeButton(void);
  void ReleaseButtonArray(void);
  uint16 IsVirtualSoftKey(uint32 KeyCode);
  
  DECLARE_MAIL_MAP()
};

inline void SoftkeyBarCtrlC::SetBGColor(int32 Color)
{
  mBGColor = Color;
}

inline int32 SoftkeyBarCtrlC::GetBGColor()
{
  return mBGColor;
}

inline uint8 SoftkeyBarCtrlC::GetButtonTotal()
{
  return mButtonArray.GetSize();
}





#endif
