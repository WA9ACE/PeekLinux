

#ifndef UIMENUVIEW_H
#define UIMENUVIEW_H

#include "buimenuctrl.h"
#include "buimenumodel.h"
#include "buiscrollbar.h"

class MenuDrawerC;
class MenuModelC;

//! the definition of menu view class
class MenuViewC
{
/*! \cond private */      
protected:
  /*vertical scroll,default value is null*/
  ScrollBarControlC *mScrollBarP;
  
  /*MenuModelC,default value is null*/
  MenuModelC *mMenuModelP;
  
  /*the current highlight item index,default value is 0*/
  uint16 mCurrentItemIndex;
  int16 mselectAnchor;
  
  /*the top item index,default value is 0*/
  uint16 mTopItemIndex;

  /*focus*/
  bool mFocused;
  
  /*the DCC*/
  DCC *mDcP;

  WinHandleT mWinHandle;
  WindowC *mWindowP;

  /* the customer has defined focused menu color*/
  bool mUseCustomerColor;
  MenuColorsT mCustomerMenuColor;
protected:
  /*only for animation*/
  void PrepareForDraw();
/*! \endcond */  

  //Always have or always have not
  bool m_bAlwaysScrollbar;
  
public:
  MenuViewC();
  virtual ~MenuViewC();

  //!inline functions 
  void SetMenuModel(MenuModelC *MenuModelP){mMenuModelP = MenuModelP;};    
  inline uint16 GetCurrentItemIndex() const{return mCurrentItemIndex;}; 
  inline uint16 GetTopItemIndex() const{return mTopItemIndex;};   

  void SetCurrentItemIndex(uint16 ItemIndex);
  void  SetSelectAnchor(int16 Anchor){mselectAnchor= Anchor;}; // zhanglanlan for multi select.when user press shift anchor is set
  inline int16  GetSelectAnchor(){return mselectAnchor;}; // zhanglanlan
  
  inline void SetDC(DCC *DcP){mDcP = DcP;};
  inline WinHandleT GetWinHandle(){return mWinHandle;};
  inline WindowC* GetWinP(){return mWindowP;};//optimize code lyf
  void SetWinHandle(WinHandleT WinHandle);
  void SetAlwaysScrollbar(bool bShow){m_bAlwaysScrollbar = bShow;};//Default = TRUE;
  
  DCC* GetDC();

  void UpdateView(int Type = 0);

  void DataChangeNotify();

  void SetMenuColor(MenuColorsT MenuColor);
  void SetParentCtrlType(uint16 type){mParentCtrlType = type;};
  void SetItemBgImg(ResourceIdT focus, ResourceIdT unfocus, ResourceIdT unfocussection);
private:
  uint16 mParentCtrlType;
  ResourceIdT mItemFocusBgImg;
  ResourceIdT mItemUnFocusBgImg;
  ResourceIdT mItemUnFocusBgImgSection;

public:  
  virtual void SetTopItemIndex(uint16 ItemIndex);

  virtual BalDispRectT GetItemRect(uint16 ItemIndex);
  virtual BalDispRectT GetTitleRect();
  virtual uint16  GetScreenCount();
  virtual void SetScreenCount(int16 nScreenCount);
  virtual void Draw(DCC *DcP);
  virtual void DrawBg();
  virtual void DrawTitle();
  virtual void DrawItem(uint16 ItemIndex);
  virtual void CreateScrollBar();
  virtual void ManageScrollBar();
  virtual void UpdateScrollBar();

  virtual void UpdateDataStatus();
};

class GridMenuViewC: public MenuViewC
{
public:
  virtual void SetTopItemIndex(uint16 ItemIndex);
  virtual BalDispRectT GetItemRect(uint16 ItemIndex);
  virtual uint16  GetScreenCount();
  virtual void CreateScrollBar();
  virtual void ManageScrollBar();
  virtual void UpdateScrollBar();
  virtual void Draw(DCC *DcP);

};




#endif
