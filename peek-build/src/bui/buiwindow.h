

#ifndef UIWINDOW_H
#define UIWINDOW_H 

#include "buidefs.h"
#include "buimsguser.h"
#include "builinkedlist.h"
#include "buidcc.h"
#include "buirect.h"
#include "buiimagesupport.h"
#include "buiimage.h"

//#include "restypedefines.h"


class WindowC;

typedef void (WindowC::*WinMailHandlerT)(void);


#define WINSTYLE_CHILD          0x80000000L //!< window is a child of other window
#define WINSTYLE_VISIBLE        0x40000000L //!< window is visible
#define WINSTYLE_DISABLED       0x20000000L //!< window is disabled
#define WINSTYLE_TRANSPARENT    0x10000000L //!< window is transparent
#define WINSTYLE_TITLE          0x08000000L //!< window has title
#define WINSTYLE_SKBAR          0x04000000L //!< window has softkey bar
#define WINSTYLE_VSCROLL        0x02000000L //!< window has vertical scrollbar
#define WINSTYLE_HSCROLL        0x01000000L //!< window has horizontal scrollbar

//! WinEvent Id define
typedef enum
{
  WINEVTID_NONE,
  WINEVTID_OnFocusChange,
  WINEVTID_OnKeyPress,
  WINEVTID_MAX
} WinEventIdT;


//! Map WINMSG_CREATE to OnCreate
#define ON_WINMSG_CREATE(ThisClass) \
 { WINMSG_CREATE, WINMSG_CREATE, MHType_l_v, \
    (MailHandlerT)(WinMailHandlerT)(int32 (WindowC::*)(void))&ThisClass::OnCreate },

//! Map WINMSG_DESTROY to OnDestroy 
#define ON_WINMSG_DESTROY(ThisClass) \
 { WINMSG_DESTROY, WINMSG_DESTROY, MHType_v_v, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(void))&ThisClass::OnDestroy },

//! Map WINMSG_MOVE to OnMove 
#define ON_WINMSG_MOVE(ThisClass) \
 { WINMSG_MOVE, WINMSG_MOVE, MHType_v_ii, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(int16, int16))&ThisClass::OnMove },

//! Map WINMSG_SIZE to OnSize 
#define ON_WINMSG_SIZE(ThisClass) \
 { WINMSG_SIZE, WINMSG_SIZE, MHType_v_ii, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(int16, int16))&ThisClass::OnSize },

//! Map WINMSG_ENABLE to OnEnable 
#define ON_WINMSG_ENABLE(ThisClass) \
 { WINMSG_ENABLE, WINMSG_ENABLE, MHType_v_b, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(bool))&ThisClass::OnEnable },

//! Map WINMSG_SHOW to OnShow 
#define ON_WINMSG_SHOW(ThisClass) \
 { WINMSG_SHOW, WINMSG_SHOW, MHType_v_b, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(bool))&ThisClass::OnShow },

//! Map WINMSG_SETFOCUS to OnSetFocus  
#define ON_WINMSG_SETFOCUS(ThisClass) \
 { WINMSG_SETFOCUS, WINMSG_SETFOCUS, MHType_v_W, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(WindowC*))&ThisClass::OnSetFocus },

//! Map WINMSG_KILLFOCUS to OnKillFocus 
#define ON_WINMSG_KILLFOCUS(ThisClass) \
 { WINMSG_KILLFOCUS, WINMSG_KILLFOCUS, MHType_v_W, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(WindowC*))&ThisClass::OnKillFocus },

//! Map WINMSG_SHOWNOTIFY to OnShowNotify 
#define ON_WINMSG_SHOWNOTIFY(ThisClass) \
 { WINMSG_SHOWNOTIFY, WINMSG_SHOWNOTIFY, MHType_v_v, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(void))&ThisClass::OnShowNotify },

//! Map WINMSG_HIDENOTIFY to OnHideNotify 
#define ON_WINMSG_HIDENOTIFY(ThisClass) \
 { WINMSG_HIDENOTIFY, WINMSG_HIDENOTIFY, MHType_v_v, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(void))&ThisClass::OnHideNotify },

//! Map WINMSG_PARENTNOTIFY to OnParentNotify 
#define ON_WINMSG_PARENTNOTIFY(ThisClass) \
 { WINMSG_PARENTNOTIFY, WINMSG_PARENTNOTIFY, MHType_v_wl, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(uint32, int32))&ThisClass::OnParentNotify },

//! Map WINMSG_NOTIFY to OnNotify  
#define ON_WINMSG_NOTIFY(ThisClass) \
 { WINMSG_NOTIFY, WINMSG_NOTIFY, MHType_b_wl, \
    (MailHandlerT)(WinMailHandlerT)(bool (WindowC::*)(uint32, int32))&ThisClass::OnNotify },

//! Map WINMSG_ERASEBKGND to OnEraseBkgnd  
#define ON_WINMSG_ERASEBKGND(ThisClass) \
 { WINMSG_ERASEBKGND, WINMSG_ERASEBKGND, MHType_b_v, \
    (MailHandlerT)(WinMailHandlerT)(bool (WindowC::*)(void))&ThisClass::OnEraseBkgnd },

//! Map WINMSG_PAINT to OnPaint  
#define ON_WINMSG_PAINT(ThisClass) \
 { WINMSG_PAINT, WINMSG_PAINT, MHType_v_v, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(void))&ThisClass::OnPaint },

//! Map WINMSG_KEYPRESS to OnKeyPress  
#define ON_WINMSG_KEYPRESS(ThisClass) \
 { WINMSG_KEYPRESS, WINMSG_KEYPRESS, MHType_b_w, \
    (MailHandlerT)(WinMailHandlerT)(bool (WindowC::*)(uint32))&ThisClass::OnKeyPress }, 

//! Map WINMSG_KEYHOLD to OnKeyHold  
#define ON_WINMSG_KEYHOLD(ThisClass) \
 { WINMSG_KEYHOLD, WINMSG_KEYHOLD, MHType_b_w, \
    (MailHandlerT)(WinMailHandlerT)(bool (WindowC::*)(uint32))&ThisClass::OnKeyHold },
    
//! Map WINMSG_KEYRELEASE to OnKeyRelease  
#define ON_WINMSG_KEYRELEASE(ThisClass) \
 { WINMSG_KEYRELEASE, WINMSG_KEYRELEASE, MHType_b_w, \
    (MailHandlerT)(WinMailHandlerT)(bool (WindowC::*)(uint32))&ThisClass::OnKeyRelease },

//! Map WINMSG_CHANGEFOCUS to OnChangeFocus  
#define ON_WINMSG_CHANGEFOCUS(ThisClass) \
 { WINMSG_CHANGEFOCUS, WINMSG_CHANGEFOCUS, MHType_v_w, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(uint32))&ThisClass::OnChangeFocus },

//! Map WINMSG_MENU_ITEM_SCROLL to OnScroll  
#define ON_WINMSG_MENU_ITEM_SCROLL(ThisClass) \
 { WINMSG_MENU_ITEM_SCROLL, WINMSG_MENU_ITEM_SCROLL, MHType_v_wl, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(uint32))&ThisClass::OnScroll },

//! Map WINMSG_MENU_ITEM_SCROLL to OnScroll  
#define ON_WINMSG_MENU_ITEM_SELECTED(ThisClass) \
 { WINMSG_MENU_ITEM_SELECTED, WINMSG_MENU_ITEM_SELECTED, MHType_v_w, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(uint32))&ThisClass::OnOptionSelected },
//!Map WINMSG_HELPTEXT_TIMEOUT to OnHelpTextTimeOut  
#define ON_WINMSG_HELPTEXT_TIMEOUT(ThisClass)\
 { WINMSG_HELPTEXT_TIMEOUT, WINMSG_HELPTEXT_TIMEOUT, MHType_v_v, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(void))&ThisClass::OnHelpTextTimeOut },

#define ON_WINMSG_APP_NOTIFY_EVENT(ThisClass)\
 { WINMSG_APP_NOTIFY_EVENT, WINMSG_APP_NOTIFY_EVENT, MHType_v_wl, \
    (MailHandlerT)(WinMailHandlerT)(void (WindowC::*)(void))&ThisClass::OnWinEventNotify },



//! Window type     
typedef enum
{
  WIN_TYPE_DIALOG, //!< Dialog 
  WIN_TYPE_CONTROL  //!< Control
} WinTypeT;

//! Window class define
class WindowC : public MailClientC
{
public:
  WindowC(WinTypeT WinType);
  virtual ~WindowC();
  
  virtual int32 ProcessMail(uint32 MailMsgId, WinHandleT WinHandle, uint32 ParamA, uint32 ParamB);
  
public:
  static WindowC* FromHandle(WinHandleT WinHandle);
  static bool IsValidWindow(const WindowC* WinP);

  //virtual bool Create(ResourceIdT ResourceId, WindowC* ParentWinP, uint32 WinId = 0);
  //virtual bool Create(ResourceIdT ResourceId, void* ExtraDataP, uint32 WinId = 0,                      
 //                     uint32 Style = WINSTYLE_SKBAR,//modified by hebo 20080408
 //                        uint32 Style = WINSTYLE_VISIBLE,
 //                     DispLCDTypeT LcdType = LCD_MAIN);

  //virtual bool ReLoadResource(ResourceIdT ResourceId);

  virtual bool DestroyWindow(void);
  
  WinTypeT GetWinType(void) const;
  WinHandleT GetWinHandle(void) const;
  
  uint32 SetWinID(uint32 NewId);  
  uint32 GetWinID(void) const;

  void SetStyle(uint32 Style);
  uint32 GetStyle(void) const;

  void SetExtraData(void* ExtraDataP);  
  void* GetExtraData(void) const;

  //void SetBackImage(ResourceIdT ImageResId); 
  void SetBackImage(ImageTypeT ImageType, const char *FileNameP);
  ImageC *GetBackImage(void) const;
  //void SetFocusImage(ResourceIdT ImageResId);

  //void SetResourceId(ResourceIdT ResourceId);
  //ResourceIdT GetResourceId(void) const;

  void GetWinRect(LPRectT RectP) const;

  void SetLcdType(DispLCDTypeT LcdType);
  const DCC* GetDC(void) const;

  DCC* BeginPaint(void);
  void EndPaint(void);

  void ValidateRect(LPCRectT RectP);
  void InvalidateRect(LPCRectT RectP, bool EraseBg = TRUE);
  void UpdateWindow(bool Synchronous = TRUE); 

  const RectC& GetClientRect(void) const;
  
  bool GetUpdateRect(LPRectT RectP) const;

  void MoveWindow(int16 x, int16 y, int16 Width = -1, int16 Height = -1);
  void CenterWindow(WindowC* OwnerP = NULL);  
  
  void SetVisible(bool Show);
  bool IsVisible(void) const;

  void LockWindow(bool Lock = TRUE);
  bool IsLocked(void) const;

  void EnableWindow(bool Enable = TRUE);
  bool IsEnabled(void) const;

  bool IsTransparent(void) const;

  bool IsChild(void) const;

  bool IsOnHide(void) const;

  WindowC* GetChildWin(uint32 WinId) const;

  bool SetFocusChild(WindowC* NewFocusChildP);  
  WindowC* GetFocusChild(void) const;
  virtual void SetFocusAble(bool bFocus){mFocusable = bFocus;}
  bool GetFocusAble(void){return mFocusable;}

  bool SetParent(WindowC* NewParentP);  
  WindowC* GetParent(void) const;
  bool SetParentLite(WindowC* ParentP);

  bool SetOwner(WindowC* NewOwnerP);
  WindowC* GetOwner(void) const;

  bool SetMainWin(const WindowC* MainWinP);
  WindowC* GetMainWin(void) const;  

  void DockWindow(bool Dock);
  bool IsDocked(void) const;

  bool GetCycleFocus(void); 
  void SetCycleFocus(bool isCycle);
protected:
  /*! 
  \brief Load specified resource and initialize the window

  Create will call this function to load the window resource.
  It is a abstract virtual functon, so WindowC-derived class 
  should implement this function if it want be instantialized.
  */
  //virtual bool LoadResource(ResourceIdT ResourceId) = 0;

  /*! 
  \brief Draw the window

  OnPaint will call this virtual function to draw the window.
  It is a abstract virtual functon, so WindowC-derived class 
  should implement this function if it want be instantialized.
  */
  virtual void Draw(DCC *DcP) = 0;

  virtual void AddChild(WindowC* ChildWinP);
  virtual void RemoveChild(const WindowC* ChildWinP);

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
  void OnPaint(void);  
  bool OnEraseBkgnd(void);
  void OnSetFocus(WindowC* OldWinP);  
  void OnKillFocus(WindowC* NewWinP);
  bool OnKeyPress(uint32 KeyCode);
  bool OnKeyHold(uint32 KeyCode);
  bool OnKeyRelease(uint32 KeyCode);
  void OnChangeFocus(uint32 Direction);
  void DrawBgImage(DCC *DcP);
  
protected:
  RectC mWinRect; //!< window rectangle
  RectC mClientRect; //!< window client rectangle
  DCC mDc; //!< device context
  uint32 mStyle; //!< window style words
  bool mOnHide; //!< whether window is on hide
  bool mFocusable; //!< whether window is focusable
  bool mFocused; //!< whether window is focused
  ImageC *mBackImageP; //!< the background image
  ImageC *mFocusImageP;//Focus Image P;

  BLinkedListC mOwnedList; //!< window list owed by this window
  BLinkedListC mChildList; //!< child window list

private:
  void AddOwned(const WindowC* OwnedWinP);
  void RemoveOwned(const WindowC* OwnedWinP);

  void CascadeInvalidate(LPCRectT RectP, bool EraseBg = TRUE);
  void CascadeDestroy(void);
#ifdef AUTO_ADJUST_RESOURCE
  void AdjustWindow();
#endif  
  
private:
  WinHandleT mWinHandle;
  const WinTypeT mWinType;
  //ResourceIdT mResourceId;
  uint32 mWinId;
  bool mCreated;
  bool mLocked;
  bool mDocked;
  
  void* mExtraDataP;

  RectC mInvalidRect;
  bool mEraseBkgnd;

  WinHandleT mOwner;
  WinHandleT mParent;
  
  WinHandleT mMainWin;
  WinHandleT mFocusWin;

  bool mCycleFocus;
  DECLARE_MAIL_MAP()
};


//! Get the window type 
inline WinTypeT WindowC::GetWinType(void) const
{
  return mWinType;
}

//! Get the window handle
inline WinHandleT WindowC::GetWinHandle(void) const
{
  return mWinHandle;
}

//! Set window id
inline uint32 WindowC::SetWinID(uint32 NewId)
{
  uint32 OldId = mWinId;
  mWinId = NewId;
  return OldId;
}

//! Get window id
inline uint32 WindowC::GetWinID(void) const
{
  return mWinId;
}

//! Set window style
inline void WindowC::SetStyle(uint32 Style)
{
  mStyle = Style;
}

//! Get window stype
inline uint32 WindowC::GetStyle(void) const
{
  return mStyle;
}

//! Set window extra data
inline void WindowC::SetExtraData(void* ExtraDataP)
{
  mExtraDataP = ExtraDataP;
}

//! Get window extra data
inline void* WindowC::GetExtraData(void) const
{
  return mExtraDataP;
}

//! Get window backgroud image 
inline ImageC* WindowC::GetBackImage(void) const
{
  return mBackImageP;
}

//! Set window resource id 
//inline void WindowC::SetResourceId(ResourceIdT ResourceId)
//{
//  mResourceId = ResourceId;
//}

//! Get window resource id
//inline ResourceIdT WindowC::GetResourceId(void) const
//{
//  return mResourceId;
//}  

//! Get window dc point 
inline const DCC* WindowC::GetDC(void) const
{
  return &mDc;
}

//! Lock/Unlock window
inline void WindowC::LockWindow(bool Lock)
{
  mLocked = Lock;
}

//! Get window lock status
inline bool WindowC::IsLocked(void) const
{
  return mLocked;
}

//! Get window on hide status
inline bool WindowC::IsOnHide(void) const
{
  return mOnHide;
}

//! Set the dockable attribute
inline void WindowC::DockWindow(bool Dock)
{
  mDocked = Dock; 
}

//! Get the dockable attribute
inline bool WindowC::IsDocked(void) const
{
  return mDocked;
}
inline void WindowC::SetCycleFocus(bool isCycle)
{
	mCycleFocus = isCycle;
}

inline bool WindowC::GetCycleFocus(void)
{
	return mCycleFocus;
}



#endif

