

#ifndef UIDIALOG_H
#define UIDIALOG_H 

#include "buiwindow.h"
#include "buicontrol.h"
#include "buisoftkeybarctrl.h"
#include "buiscrollbar.h"
#include "buiannounciator.h"

#include "restypedefines.h"


typedef enum
{
  DLG_TYPE_NORMAL,
  DLG_TYPE_LIST,
  DLG_TYPE_OPTION,
  DLG_TYPE_POPUP,
  DLG_TYPE_CUST,
  DLG_TYPE_COLOR,
  DLG_TYPE_USEDETAIL,
  DLG_TYPE_MAX
}DlgTypeT;

typedef enum
{
  TITLE_TYPE_NORMAL = 0,
  TITLE_TYPE_INDEX,
  TITLE_TYPE_EDIT,
  TITLE_TYPE_SIDES,
  TITLE_TYPE_LIMAGE,
  
  TITLE_TYPE_MAX
}TitleTypeT;


//! This structure is used in ui theme to configure the dialog colors
typedef struct 
{
  BalDispColorT BkColor;         //!< dialog background color
  BalDispColorT TitleTextColor;  //!< dialog title text color
} DialogColorsT;

typedef struct
{
  BalDispFontT Font;
  BalDispHAlignT HAlign;
  int16 Height;
  int16 MarginX;
  int16 MarginY;
} TitleLayoutT;

typedef struct
{
  int16 Height;
} SKBarLayoutT;

typedef struct  
{
  int16 Width;
} VScrollLayoutT;

typedef struct
{
  TitleLayoutT Title;
  SKBarLayoutT SKBar;
  VScrollLayoutT VScroll;
} DialogLayoutT;

#define DIALOGC_TITLEIMAGE_NUM  2

class MenuCtrlC;
//! Dialog class define
class DialogC : public WindowC
{
public:
  DialogC();
  virtual ~DialogC();
  virtual void SetDlgType(DlgTypeT type){mDlgType = type;}
  virtual DlgTypeT GetDlgType(){return mDlgType;}

  virtual bool ReLoadResource(ResourceIdT ResourceId);

  void SetCloseMail(uint32 CloseMailID, uint32 CloseReason = 0);
  uint32 GetCloseMailID(void) const;
  uint32 GetCloseReason(void) const;

  void SetDialogInfo(void* DialogInfoP);
  void* GetDialogInfo(void) const;

  void SetCreateParam(uint32 Param);
  uint32 GetCreateParam(void) const;

  virtual ControlC* GetControlByType(CtrlTypeT ControlType, uint8 Number = 0);
  ControlC* GetControlByResourceId(ResourceIdT ResourceId);
  bool ReplaceControl(ControlC* OldCtrlP, ControlC* NewCtrlP);

  virtual void SetTitle(const BStringC& Title);
  virtual void SetTitle(RESOURCE_ID(StringT) ResId);
  void SetTitleType(TitleTypeT type){mTitleType = type;};
  void SetTitleInfo(const BStringC& TitleInfo);  
  void SetTitleInfo(RESOURCE_ID(StringT) ResId);
  void SetTitleImage(RESOURCE_ID(ImageResT) ImageResId, int index = 0);
  

  bool CloseDialog(bool SendCloseMail = FALSE);

  void SetSupportBG(bool Support);
  bool GetSupportBG(void);
  void SetTranslucence(bool Translucence);

  void SetAppDraw(bool AppDraw);
  bool GetAppDraw(void);
  void SetCloseUpdate(bool Update){Update = Update;};

  const RectC& GetTitleRect(void) const;
  void SetSelectItemBgImage(ResourceIdT ImageResId);
  void SetUnSelectItemBgImage(ResourceIdT ImageResId);// zhanglanlan
  ImageC* GetSelectItemBgImage(void) const {return mSelectItemBgImageP;};
  ImageC* GetUnSelectItemBgImage(void) const {return mUnSelectItemBgImageP;}; // zhanglanlan
  
  SoftkeyBarCtrlC* GetSkBar(void);
  ScrollBarControlC* GetVScrollBar(void);

  //Add by Byd-ZouWq 2007-1115
  void SetSoftkeyText(RESOURCE_ID(StringT) ResLeftID,RESOURCE_ID(StringT) ResMidId,RESOURCE_ID(StringT) ResRightID);
  void SetSoftkeyMessageId(uint32 nLeftMsgId,uint32 nMiddleMsgId,uint32 nRightMsgId,bool bReset = FALSE);
  void SetSoftkeyText(const BStringC& sLeftID,const BStringC& sMidId,const BStringC& sRightID);
  SoftkeyBarErrorT SetSoftkeyInfo(SoftkeyType etType, BStringC& strText, uint32 dwMsgId = 0, uint32 dwParam = 0);
  SoftkeyBarErrorT SetSoftkeyInfo(SoftkeyType etType, RESOURCE_ID(StringT) ResId, uint32 dwMsgId = 0, uint32 dwParam = 0);
  SoftkeyBarErrorT GetSoftkeyInfo(SoftkeyType etType, BStringC& strText, uint32& dwMsgId, uint32& dwParam);
  bool GetCanDialingFlag();
  void SetDialingFlag(bool bCan){mbDialNum = bCan;}
  virtual void RemoveChild(const WindowC* ChildWinP);
  virtual void SaveString(uint32 dwDlgId, BStringC& strText);
  
protected:
  WindowC* CreateControl(CtrlTypeT Type);
  
  virtual bool LoadResource(ResourceIdT ResourceId);
  virtual void Draw(DCC *DcP);

  virtual void AddChild(WindowC* ChildWinP);

protected:
  bool OnNotify(uint32 ParamA, int32 ParamB);
  void OnParentNotify(uint32 Message, int32 Param);
  void OnShowNotify(void);
  void OnHideNotify(void);
  int32 OnCreate(void);
  void OnDestroy(void);
  void OnEnable(bool Enable);
  void OnShow(bool Show);  
  bool OnEraseBkgnd(void);
  void OnScroll(uint16 nCurIndex, uint32 nReason);
  bool OnKeyPress(uint32 KeyCode);
  bool OnKeyHold(uint32 KeyCode);
  bool OnKeyRelease(uint32 KeyCode);
  void OnWinEventNotify(uint16 wEventId,uint32 dwParam);
  void InvalidShortcut(bool bCheckKey = FALSE);
  bool ProcessShortcut(MenuCtrlC* pMenuCtrl = NULL);

private:
  bool PreOnKey(uint32 KeyMsgId, uint32 KeyCode);

  void DrawBackground(DCC* DcP);
  void DrawTitle(DCC* DcP);  
  SoftkeyBarCtrlC* GetSoftkeyBar(void);

protected:
  BStringC mTitle;
  BStringC mTitleInfo;
  DlgTypeT mDlgType;//For Extend to Detail which one class
  ImageC *mSelectItemBgImageP;//the selected background image
  ImageC *mUnSelectItemBgImageP;//the unselected background image  add by zhanglanlan
  TitleTypeT mTitleType;
  ImageC mTitleImg[DIALOGC_TITLEIMAGE_NUM];

  
private:
  uint32 mCloseMailID;
  uint32 mCloseReason;
  void* mDialogInfoP;
  uint32 mCreateParam;
  
  bool mSupportBG;
  bool mAppDraw;
  bool mbDialNum;//Can Dial Number Directory in this dialog

  RectC mTitleRect;

  AnnounciatorC* mAnnP;
  SoftkeyBarCtrlC* mSkBarP;
  ScrollBarControlC* mVScrollBarP;
  
  DECLARE_MAIL_MAP()
};

/*! 
\brief Set the dialog close mail id and parameter

this close mail will be sent out when dialog is closing
*/
inline void DialogC::SetCloseMail(uint32 CloseMailID, uint32 CloseReason)
{
  mCloseMailID = CloseMailID;
  mCloseReason = CloseReason;
}

//! Get dialog close mail id 
inline uint32 DialogC::GetCloseMailID(void) const
{
  return mCloseMailID;
}

//! Get dialog close mail parameter
inline uint32 DialogC::GetCloseReason(void) const
{
  return mCloseReason;
}

/*! 
\brief Set dialog information

the information structure pointed by DialogInfoP may be application
specific

\param DialogInfoP -[in] point to a dialog information structure
\retval none
*/
inline void DialogC::SetDialogInfo(void* DialogInfoP)
{
  mDialogInfoP = DialogInfoP;
}

/*!
\brief Get dialog information

the information structure may be application specific

\retval the point to the dialog information structure 
*/ 
inline void* DialogC::GetDialogInfo(void) const
{
  return mDialogInfoP;
}

/*!
\brief Set dialog create parameter

the meaning of Param may be application specific
*/  
inline void DialogC::SetCreateParam(uint32 Param)
{
  mCreateParam = Param;
}

/*!
\brief Get dialog create parameter

the meaning of Param may be application specific
*/  
inline uint32 DialogC::GetCreateParam(void) const
{
  return mCreateParam;
}

//! Make dialog support or not support background image
inline void DialogC::SetSupportBG(bool Support)
{
  mSupportBG = Support;
}

//! Get whether the dialog support background image
inline bool DialogC::GetSupportBG(void)
{
  return mSupportBG;
}

//! Set whether dialog should be drawn by app
inline void DialogC::SetAppDraw(bool AppDraw)
{
  mAppDraw = AppDraw;
}

//! Get whether dialog should be draw by app
inline bool DialogC::GetAppDraw(void)
{
  return mAppDraw;
}

//! Get the softkey bar
inline SoftkeyBarCtrlC* DialogC::GetSkBar(void)
{
  return mSkBarP;
}

//! Get the vertical scroll bar
inline ScrollBarControlC* DialogC::GetVScrollBar(void)
{
  return mVScrollBarP;
}

#endif

