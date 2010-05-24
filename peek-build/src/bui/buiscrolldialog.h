

#ifndef UISCROLLDIALOG_H
#define UISCROLLDIALOG_H 


#include "buidialog.h"


#include "builinkedlist.h"
#include "buidcc.h"
#include "buirect.h"


#include "restypedefines.h"


typedef struct 
{
   uint16 priority;
   WindowC *winP;
   RectC rect;
 }ProCtrlT;
//! Scroll Dialog class define
class ScrollDialogC : public DialogC
{
public:
  ScrollDialogC();
  ScrollDialogC(uint16 *array,uint16 num);
  virtual ~ScrollDialogC();

  virtual bool Create(ResourceIdT ResourceId, void* ExtraDataP, uint32 WinId = 0,                      
                      uint32 Style = (WINSTYLE_TITLE|WINSTYLE_SKBAR|WINSTYLE_VSCROLL),
                      DispLCDTypeT LcdType = LCD_MAIN);
  
  bool CreateEx(void* ExtraDataP,uint32 WinId= 0,uint32 Style = (WINSTYLE_TITLE|WINSTYLE_SKBAR|WINSTYLE_VSCROLL),
  	                     DispLCDTypeT LcdType = LCD_MAIN);

  void SetScrollUnit(uint16 Unit);
  uint16 GetScrollUnit(void);
  
  virtual void AddChild(WindowC* ChildWinP);
  void RemoveChild(WindowC* ChildWinP);
  void DeleteChild(WindowC* DelWinP);
  
  void SetPriority (uint16 ProIndex = 0){mProIndex =ProIndex; }
  uint16 GetPriority(){return mProIndex;}

  void SetFocusIndex(uint16 focus);
  
  void ChangeWinPriority(WindowC *WinP,uint16 newPrioIndex);
  WindowC* CreateControl(CtrlTypeT CtrlType, uint16 PriorityIndex,ResourceIdT ResId);
  WindowC* GetFocusControl(int16 nFocus = -1);
  uint16 GetFocusIndex();
  uint16 GetCount();

 protected:
// 	 void InsertChild(ProCtrlT* temp);
	 void AddChildInfo(ProCtrlT* ChildInfoP);
     void RemoveChildInfo(WindowC* ChildWinP);
//	 bool GetFocusAble(){return mFocusable;}
	 void  PBFocusNext(WindowC * Winp);
     void CalulateView();
	 void MoveToFocusWindow();
protected:
  typedef enum
  {
    SCROLL_ADJUST,
    SCROLL_UP,
    SCROLL_DOWN
  } ScrollDirectionT;

protected:  
  //virtual void AddChild(WindowC* ChildWinP);
 // virtual void RemoveChild(const WindowC* ChildWinP);

  void UpdateScrollBar(void);

protected:
  bool OnKeyPress(uint32 KeyCode);
  bool OnKeyHold(uint32 KeyCode);
  bool OnKeyRelease(uint32 KeyCode); 
  void OnChangeFocus(uint32 Direction);  

  //-------------------------------------
  void OnPBChangeFocus(uint32 Direction);

   IteratorT PBFocusFirst(WindowC * Winp);

  //-------------------------------------

  void OnScroll(ScrollDirectionT Direction);

protected:
  uint16 mVPosition;
  uint16 mMaxChildBottom;
  uint16 mScrollUnit;
  BLinkedListC mProInfo;
  uint16 mProIndex;
  BVectorC InitRes;
  uint16 mTopIndex;
  uint16 mFocusIndex;
  uint16 mCount;

  bool mMark;

  DECLARE_MAIL_MAP()
};


inline void ScrollDialogC::SetScrollUnit(uint16 Unit)
{
  mScrollUnit = Unit;
}

inline uint16 ScrollDialogC::GetScrollUnit(void)
{
  return mScrollUnit;
}




#endif


