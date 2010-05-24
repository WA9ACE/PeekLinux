

#ifndef UIMENUCONTROL_H
#define UIMENUCONTROL_H


#include "buivector.h"
#include "buiwindow.h"
#include "buicontrol.h"

#include "buimenudefine.h"
#include "buimenumodel.h"
#include "buimenuview.h"
#include "buimenuimpl.h"

//! other classes declare
class MenuItemC;
class MenuTitleC;
class MenuItemVendorC;
class MenuInputC;
class ItemDrawerC;

enum
{
    MENU_TYPE_DEFAULT = 0,
    MENU_TYPE_COMBOBOX,
    MENU_TYPE_MAX
};
//! menu control define
class MenuCtrlC: public ControlC
{
protected:
  MenuInputC *mMenuInputP;//!< default value is true
  MenuViewC *mMenuViewP;//!< default value is true
  MenuModelC *mMenuModelP;//!< default value is true
  BVectorC*	mShortcutKey;

/*! \cond private */
protected:
  void ConstructMenu(MenuModelC *MenuModelP);
  virtual MenuModelC* CreateModel();
  void ReleaseResource();
  void OnMove(int16 X, int16 Y);  
  bool OnKeyPress(uint32 KeyCode);
  bool OnKeyRelease(uint32 KeyCode);
  void OnSetFocus(WindowC* OldWinP);  
  void OnKillFocus(WindowC* NewWinP);
  void OnShowNotify(void);
  void OnHideNotify(void);
  void OnAnimationItem(void *MsgP);
  void OnCheckBoxChangeStatus(void);
  void OnCheckAll(void);
  void OnUnCheckAll(void);
  void OnHoldKeyTimer(uint32 KeyCode);
  bool cheakShortcutState(MenuItemC *ItemP);
  virtual bool LoadResource(ResourceIdT ResourceId);
  virtual void Draw(DCC *DcP);
/*! \endcond */  
private:
  uint16 mParentCtrlType;
  ResourceIdT mItemFocusBgImg;
  ResourceIdT mItemUnFocusBgImg;
  ResourceIdT mItemUnFocusBgImgSection;

public:
  MenuCtrlC();  
  virtual ~MenuCtrlC();
  void SendNotify(MenuEventT MenuEvent);
  void InsertItem(uint16 ItemIndex, MenuItemC *ItemP);
  void AppendItem(MenuItemC *ItemP);
  void DeleteItem(uint16 ItemIndex);
  void DeleteItem(MenuItemC *ItemP);
  MenuItemC* FindItemByText(BStringC &Text);
  MenuItemC* FindItemByParameter(uint32 Parameter );
  MenuItemC* FindItemByID( RESOURCE_ID(MenuItemResT)  ItemId);
  int FindIndexByText(BStringC &Text);
  MenuItemC* GetItem(uint16 ItemIndex);
  uint16 GetItemCount();
  void SelectItem(uint16 ItemIndex);
  void SelectItem(MenuItemC *ItemP);
  uint16 GetSelectedItem();
  void SetSelectedItem(uint16 item);
  MenuTitleC* GetTitle();
  void SetRect(RectC &Rect);
  void SetMenuSize(MenuSizeT MenuSize);
  MenuSizeT GetMenuSize();
  void SetMenuType( MenuTypeT MenuType );
  void SetTitle(MenuTitleC *MenuTitleP);
  void CheckItem(uint16 ItemIndex);
  void CheckAll();
  void UnCheckAll();
  bool ItemIsChecked(uint16 ItemIndex);
  const BVectorC* GetAllCheckedItems();
  void SetRadioItemIndex(uint16 ItemIndex);
  uint16 GetRadioItemIndex();
  void SetFlag(uint32 FlagMask);
  void ClearFlag(uint32 FlagMask);
  void SetMenuDrawer(MenuDrawerC *MenuDrawerP);
  void SetGridMenuDetail(GridMenuDetailT GridMenuDetail);
  void SetItemHoverMessage(uint32 Message);
  void SetSupportBgImg(bool Support);
  ItemDrawerC *GetICurrentItemDrawer();
  ItemDrawerC *GetTitleDrawer(); 
  virtual void RefreshData();
  MenuTypeT GetMenuType();
  void SetTitleMargin(uint16 Margin);
  uint16 GetTitleMargin();
  void SetItemMargin(uint16 Margin);
  uint16 GetItemMargin();
  MenuViewC *GetMenuView(){return mMenuViewP;};
  
  bool IsFocused();
  void SetMenuColor(MenuColorsT);
  void SetMenuScrollType(MenuScrollT MenuScroll);
  MenuScrollT GetMenuScrollType();
  void SetAlwaysScrollbar(bool bShow);//Default = TRUE;
  void SetTopItemIndex(uint16 ItemIndex);
  uint16 GetTopItemIndex();
  void SetParentCtrlType(uint16 type){mParentCtrlType = type;};
  uint16 GetParentCtrlType(){return mParentCtrlType;};//add for horizon scroll
  void SetItemBgImg(ResourceIdT focus, ResourceIdT unfocus, ResourceIdT unfocussection);
 void SetSelectionType(MenuSelectionT SelectionType);//ZLL
 BVectorC* GetSelectedItems();//ZLL
 void  DeleteSelectedItems();
  DECLARE_MAIL_MAP()
};

inline bool MenuCtrlC::IsFocused()
{
  return mFocused;
}

class BYDMenuCtrlC: public MenuCtrlC//HugeMenuCtrlC
{
/*! \cond private */  
protected:
  virtual bool LoadResource(ResourceIdT ResourceId);
  virtual MenuModelC* CreateModel();
#if 0  //added by Randolph Wang for passing compile
  void OnHideNotify(void);
#endif
public:
  BYDMenuCtrlC();
  virtual ~BYDMenuCtrlC(){};
  BVectorC* GetCheckedItems();
  void SetCurrentItemIndex(uint16 focus);
  void UpdateData();

  void SetSubItemVisible(uint16 nSubItemIndex,bool bVisible);

  protected:
  void LConstructMenu(MenuModelC *MenuModelP);
 	
  DECLARE_MAIL_MAP()
};

//! huge menu control define
class HugeMenuCtrlC: public BYDMenuCtrlC
{
public:
  virtual bool Create(ResourceIdT ResourceId, WindowC* ParentWinP, uint32 WinId = 0);
  void InsertItem(uint16 Item, MenuItemC *MenuItem);
  /*{
    Item = Item;
    MenuItem = MenuItem;
    mMenuModelP->InsertItem(Item,MenuItem);
  };*/
  void AppendItem(MenuItemC *MenuItem);
  /*{
    MenuItem = MenuItem;
    mMenuModelP->AppendItem(MenuItem);
  };*/
  void DeleteItem(MenuItemC *ItemP);
  /*{
    ItemP = ItemP; 
    mMenuModelP->DeleteItem(ItemP);
  };*/

/*! \cond private */  
protected:
  virtual bool LoadResource(ResourceIdT ResourceId);
/*! \endcond */  

public:
  HugeMenuCtrlC();
  virtual ~HugeMenuCtrlC(){};
  virtual void RefreshData();
  void SetMenuItemVendor(MenuItemVendorC *MenuItemVendorP);
  MenuItemVendorC* GetMenuItemVendor(void);
  void DeleteItem(uint16 ItemIndex);

};




#endif

