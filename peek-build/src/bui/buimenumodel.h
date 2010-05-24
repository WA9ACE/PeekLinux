

#ifndef UIMENUMODEL_H
#define UIMENUMODEL_H

#include "buivector.h"
#include "buiwindow.h"
#include "buicontrol.h"

#include "buimenudefine.h"

#include "buimenuctrl.h"
#include "buimenudrawer.h"
#include "buimenuview.h"


#if 0
#include "buimenudrawer.h"

#include "buicontrol.h"
#include "resourcedatabase.h"
#include "vector.h"
#include "dispapi.h"
#include "stdio.h"
#include "math.h"
#include "uimenucontrol.h"
#include "hugemenucontrol.h"

#endif


#define ICON_NUMBER 3//alq 20080505

class MenuItemVendorC;
class ItemDrawerC;
class MenuViewC;

//! the definition of base class of menu title and menu item
class MenuResDataC
{
/*! \cond private */      
  //!  *the following friend class will call SetMenuModel
  friend class MenuModelC;
  friend class HugeMenuModelC;
  friend class MenuTestAppC;
protected:
  //!the title text or menu item text:  default value is null
  RESOURCE_ID(StringT)  mTextId;
  StringT *mTextP;

  //!animation :  default value is null
  RESOURCE_ID(AnimationCtrlResT) mAnimationId;

  //!icon1 and icon2:  default value is null
  RESOURCE_ID(ImageResT) mIconId[ICON_NUMBER];
  
  //!horizontal alignment:  default value is DISP_HALIGN_CENTER
  BalDispHAlignT mHAlign;

  //!title font or item font:  default value is DISP_FONT_NORMAL_PLAIN
  BalDispFontT mFontId;
  
  //!the title drawer or item drawer:  default value is null
  ItemDrawerC *mItemDrawerP;

  //!the menu model:  default value is null
  MenuModelC *mMenuModelP;

  uint8 mShortcut; // Shortcut key
  int32 mShortcutPos;
  bool mShortcutEnabled;
  
protected:
  virtual void ReleaseResource();


/*! \endcond */  

public:
  MenuResDataC();
  virtual ~MenuResDataC();
  void SetText(BStringC &Text);
  void SetText(RESOURCE_ID(StringT) Text);
  void SetAnimation(RESOURCE_ID(AnimationCtrlResT) AnimationId);
  inline RESOURCE_ID(AnimationCtrlResT) GetAnimationId(){  return mAnimationId; };
  void SetIcon(uint16 IconIndex,RESOURCE_ID(ImageResT) IconId);
  RESOURCE_ID(ImageResT) GetIcon(uint16 IconIndex) const;
  void SetFont(BalDispFontT Font);
  void SetHAlign(BalDispHAlignT HAlign);
  void ReleaseDrawer();

  inline BStringC GetText() const{return BStringC(mTextP);};
  inline void GetText(BStringC& Str){Str = mTextP;};
  inline BalDispFontT GetFont() const{return mFontId;};
  inline BalDispHAlignT GetHAlign(){return mHAlign;};
  inline void SetMenuModel(MenuModelC *MenuModelP){mMenuModelP = MenuModelP;};
  //inline void SetShortcut(uint8 key) {mShortcut = key;};
  inline uint8 GetShortcut() {return mShortcut;};
  inline int32 GetShortcutPos() {return mShortcutPos;};
  inline void  SetShortcut(uint8 Shortcut){mShortcutPos = Shortcut;};
  inline  void SetShortcutPos(int32 ShortcutPos){mShortcutPos = ShortcutPos;};
  inline void SetShortcutEnable(bool Enable = TRUE){mShortcutEnabled = Enable;};

};

class MenuTitleC:public MenuResDataC
{
private:
  //!the title resource ID:  the default value is null
  RESOURCE_ID(MenuTitleResT) mResourceId;

  //!the title type:  the default value isMENU_TITLE_TYPE_DEFAULT
  MenuTitleTypeT mTitleType;
  uint16 mStringLen;
  
public:
  MenuTitleC();
  virtual ~MenuTitleC();
  ItemDrawerC* GetTitleDrawer(uint16 CurrentItemIndex);
  void SetTitleType(MenuTitleTypeT TitleType);
  void Create(RESOURCE_ID(MenuTitleResT) ResourceId);
  inline ResourceIdT GetResourceId() const{  return (ResourceIdT)mResourceId; };
  inline MenuTitleTypeT GetTitleType() {return mTitleType;};
  bool SetTextOfCurItemIndex(uint16 CurrentItemIndex);
};

class MenuItemC:public MenuResDataC
{

  friend class MenuModelC;
  friend class HugeMenuModelC;

protected:
  //!message and parameter:  default value is 0
  uint32 mMessageId;
  uint32 mParameter;

  //!the menu item resource id:  default value is 0
  RESOURCE_ID(MenuItemResT) mResourceId;


  //!the status of multi check box:  default value is FALSE
  bool mIsChecked;
  bool mDim;//For Item Dim Value Save
  bool mIsActivate; // mode
 bool mIsSplitLine;
 bool mIsSelected;// add by zhanglanlan for multi select

public:
  MenuItemC();
  virtual ~MenuItemC();
  void Create(RESOURCE_ID(MenuItemResT) ResourceId);
  void SetDim(bool bDim){mDim = bDim; ReleaseDrawer();}//alq 20080504
  bool GetDim(){return mDim;}
  void SetItemChecked(bool Checked);
  virtual ItemDrawerC* GetItemDrawer(uint16 ItemIndex);
  inline void SetMessageId(uint32 MsgId){mMessageId = MsgId;};
  inline uint32 GetMessageId() const{return mMessageId;};
  inline void SetParameter(uint32 Parameter){mParameter = Parameter;};
  inline uint32 GetParameter() const{return mParameter;};
  inline bool GetItemChecked(){return mIsChecked;};
  inline ResourceIdT GetResourceId() const{return (ResourceIdT)mResourceId;};
  inline void SetActivate(bool activate = TRUE) {mIsActivate = activate;};
  inline bool GetActivate() {return mIsActivate;};
  inline void SetSplitLine(bool SplitLine = FALSE) {mIsSplitLine = SplitLine;};
  inline bool GetSplitLine() {return mIsSplitLine;};    
  // for multi select

  void Multi_SetItemSelected(bool Selected); // multi
  inline bool Multi_GetItemSelected(){return mIsSelected;};
      
};


class MenuModelC
{
/*! \cond private */        
protected:
  BVectorC mCheckedItems;
  BVectorC mSelectedItems;
  //!default value is null, need to release the pointer
  MenuTitleC *mMenuTitleP;
  
  //! need to release these pointers which points the MenuItemC* 
  BVectorC mItemArray;
  
  //!default value is MENU_LIST_DEFAULT 
  MenuTypeT mMenuType;
  
  //!default value is 0 
  uint32 mFlags;
  
  //!default value is DefaultMenuSize   
  MenuSizeT mMenuSize;
  
  //!default value is the LCD rect
  RectC mRect;
  
  //!default value is normal platin
  BalDispFontT mItemFont;

  //!default value is MENU_SCROLL_END_ITME_BY_PAGE
  MenuScrollT mScrollType;
  MenuSelectionT mSelectionType;

  //!for huge menu control 
  RESOURCE_ID(MenuItemResT) *mItemResP;  //!<default value is 0 ,need not release the pointer 
  MenuItemVendorC  *mMenuItemVendorP; //!<default is null,need to release the pointer
  uint16 mStaticItemCount;//!<default value is 0

  //!for radio box menu
  uint16 mRadioItemIndex;//!<default is 0, it is useful only when menu type is radio
  
  //!menu view
  MenuViewC *mMenuViewP; //!<default value is NULL,need not release the pointer

  //!menu resource id
  RESOURCE_ID(MenuCtrlResT) mResourceId;//!<default value is null

  //!for menu drawer
  MenuDrawerC *mMenuDrawerP;

  //!for GRID menu default value is DefaultGridMenuDetail
  GridMenuDetailT mGridDetail;

  //!Item hover message
  uint32 mItemHoverMessage;//!<it is a app message
protected:  
  void ReleaseResource();
/*! \endcond */  
  
public:
  MenuModelC();
  virtual ~MenuModelC();
  virtual void Create(ResourceIdT ResourceId);  
  void SetMenuType(MenuTypeT MenuType);
  void SetMenuSize(MenuSizeT MenuSize);
  void SetRect(RectC &Rect);
  void SetRadioItem(uint16 RadioItemIndex);
  void ReleaseAllItemDrawer();
  void SetFlag(uint32 FlagMask);
  void SetTitle(MenuTitleC *MenuTitleP);
  void ClearFlag(uint32 FlagMask);
  virtual uint16 GetItemCount();
  virtual MenuItemC* GetItem(uint16 ItemIndex);
  virtual void DeleteItem(uint16 ItemIndex);
  uint16 DeleteItem(MenuItemC *ItemP);
  void InsertItem(uint16 ItemIndex, MenuItemC *MenuItemP);
  void AppendItem(MenuItemC *MenuItemP);
  MenuItemC* FindItemByText(BStringC &Text);
  MenuItemC* FindItemByParameter(uint32 Parameter );
  MenuItemC* FindItemByID( RESOURCE_ID(MenuItemResT)  ItemId);
  int FindIndexByText(BStringC &Text);
  virtual void PrepareScreenData(PrepareTypeT PrepareType);
  void SetMenuItemVendor(MenuItemVendorC *MenuItemVendorP);
  void SetMenuDrawer(MenuDrawerC *MenuDrawerP);
  int ScrollMultiData(DataScrollT ScrollType);

  void SetTitleMargin(uint16 Margin);  
  uint16 GetTitleMargin();
  void SetItemMargin(uint16 Margin);  
  uint16 GetItemMargin();
  void SetSupportBgImg(bool Support);
  void SetMenuView(MenuViewC *MenuViewP); 

  void SetItemChecked(uint16 Index, bool Checked);
  void SetItemChecked(MenuItemC *ItemP, uint16 Index, bool Checked);
// zhanglanlan for multi select
  void Multi_SetItemSelected(uint16 index,bool selected);
  void Multi_SetItemSelected(uint16 index);
  inline BVectorC* Multi_GetSelectedItems(){return &mSelectedItems;};
  void ClearSelectedItems();
  void SetSelectionType(MenuSelectionT SelectionType);
  inline MenuSelectionT GetSelectionType(){return mSelectionType;};
// end zhanglanlan
  MenuItemVendorC* GetMenuItemVendor(){return mMenuItemVendorP;};
  inline MenuViewC* GetMenuView(){return mMenuViewP;};  
  inline MenuDrawerC* GetMenuDrawer(){return mMenuDrawerP;}; 
  inline uint32 Flags() const{return mFlags;};
  inline uint16 GetRadioItemIndex(){return mRadioItemIndex;};
  inline ResourceIdT GetResourceId(){return (ResourceIdT)mResourceId;};
  inline RectC GetRect(){return mRect;};
  inline MenuSizeT GetMenuSize(){return mMenuSize;};
  inline MenuTypeT GetMenuType(){return mMenuType;};
  inline MenuTitleC* GetTitle(){return mMenuTitleP;};
  inline GridMenuDetailT GetGridMenuDetail(){return mGridDetail;};
  inline void SetGridMenuDetail(GridMenuDetailT GridMenuDetail){mGridDetail = GridMenuDetail; };
  inline void SetItemHoverMessage(uint32 Message){mItemHoverMessage = Message;};
  inline uint32 GetItemHoverMessage(){return mItemHoverMessage;};

  inline BVectorC* GetCheckedItems(){return &mCheckedItems;};
  inline void ClearCheckedItems(){mCheckedItems.ReSet();};
  inline void SetMenuScrollType(MenuScrollT MenuScroll){mScrollType = MenuScroll;};
  inline MenuScrollT GetMenuScrollType(){return mScrollType;}; 
};

//Add by BYD-zouwq 2007.09.12
class BYDMenuModelC:public MenuModelC//HugeMenuModelC
{
public:
  virtual void Create(ResourceIdT ResourceId);
  //virtual uint16 GetItemCount();
  //virtual MenuItemC* GetItem(uint16 ItemIndex);
};

class HugeMenuModelC:public BYDMenuModelC
{
private:
  BVectorC mTempItems;//!<solve the leak of memory 
  
private:
  inline void InsertItem(uint16 ItemIndex, MenuItemC *MenuItem)
	{
    ItemIndex = ItemIndex;
    MenuItem = MenuItem;
	};
  inline void AppendItem(MenuItemC *MenuItem)
	{
	  MenuItem = MenuItem;
	};
  inline uint16 DeleteItem(MenuItemC *ItemP)
	{
	  ItemP = ItemP;
	  return 0; 
	};

  MenuItemC* GetMenuItem(uint16 ItemIndex);
  
public:
  HugeMenuModelC();
  virtual ~HugeMenuModelC();
  virtual uint16 GetItemCount();
  virtual MenuItemC* GetItem(uint16 ItemIndex);
  virtual void DeleteItem(uint16 Item);
  virtual void PrepareScreenData(PrepareTypeT PrepareType); 
  
};

class MenuItemVendorC
{

public:
  virtual MenuItemC *GetMenuItem(uint16 index) = 0;
  virtual bool GetCheckedStatus(uint16 /*index*/){return FALSE;}
  virtual uint16 GetCount() = 0;
  virtual uint16 GetBaseIndex(){ return 0;}
  virtual ~MenuItemVendorC() {}
};





#endif
