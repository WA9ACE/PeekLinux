

#ifndef UIMENUDRAWER_H
#define UIMENUDRAWER_H

#include "buianimationctrl.h"
#include "buimsgtimer.h"

#include "buimenuctrl.h"
#include "buimenumodel.h"

/*#define MENU_SCROLL_BY_PAGE*/ /*FOR PN310*/

 //! the menu property type
typedef struct
{
  MenuColorsT  MenuColors;
  bool SupportBgImg;
  uint32 Flags; 
  uint32 Style;
}MenuPropertiesT;

 //! the item status  type
typedef struct
{
 bool IsSelected;
 bool FocusLost;
 bool IsTitle;
 bool IsCurrentItem; //zhanglanlan
}ItemStatusT;

 //! the cell colors type
typedef struct
{
BalDispColorT  TextColor;
BalDispColorT  BgColor;
BalDispColorT  HighLightTextColor;
BalDispColorT  HighLightBgColor;
}CellColorsT;

 //! the cell property type
typedef struct
{
CellColorsT CellColors;
bool SupportBgImg;
uint32 Flags; 
uint32 Style;
}CellPropertiesT;

 //! the cell status  type
typedef struct
{
 bool IsHighlight;
 bool IsTitle;
}CellStatusT;

class MenuModelC;

 //! the CellDataC definition
class CellDataC
{
friend class ItemDrawerC;
/*! \cond private */  
protected:
  //!for number cell
  uint16 mItemIndex;//!<default is 0
  bool mIsTextOfItemIndex;//added for the text of item index
  
  //!for multi text or icon
  uint16 mCurrentDataIndex;//!<default is 0
  
  BalDispFontT mFont;
  BalDispHAlignT mHAlign; 
  BalDispVAlignT mVAlign; 

  BalDispRectT mRect;
  int16 mBaselineTop;//!< default is -1: means vertical middle 

  CellColorsT mCellColors; //!<default is UiTheme color
  bool mUseCustomerColor;//!<default is FALSE
  bool mTransparent; //!<default is true

  CellStatusT mCellStatus;
  CellPropertiesT mCellProperty;
  DCC mDc;

  WinHandleT mWinHandle;

  uint16 mCellId;
/*! \endcond */     
public:
  CellDataC(WinHandleT WinHandle);   
  virtual ~CellDataC(){};
  inline BalDispRectT GetCellRect(){return mRect;};
  inline void SetCellRect(BalDispRectT Rect){mRect = Rect;};
  void SetIsTextOfItemIndex(bool IsIndex){mIsTextOfItemIndex = IsIndex;}
  bool GetIsTextOfItemIndex(void){return mIsTextOfItemIndex;}
  inline CellColorsT GetCellColors(){return mCellColors;};
  inline void SetCellColors(CellColorsT Colors)
  {
    mUseCustomerColor = TRUE;
    mCellColors = Colors;
  };

  inline void SetItemIndex(uint16 ItemIndex){mItemIndex = ItemIndex;};
  
  inline int16 GetCellBaselineTop(){return mBaselineTop;};
  inline void SetCellBaselineTop(int16 yPos){mBaselineTop = yPos;};

  inline BalDispFontT GetCellFont(){return mFont;};
  inline void SetCellFont(BalDispFontT Font){mFont = Font;};

  inline BalDispHAlignT GetHAlign(){return mHAlign;};
  inline void SetHAlign(BalDispHAlignT HAlign){mHAlign = HAlign;};
  inline BalDispVAlignT GetVAlign(){return mVAlign;};
  inline void SetVAlign(BalDispVAlignT VAlign){mVAlign = VAlign;};

  inline bool GetCellTransparent(){return mTransparent;};
  inline void SetCellTransparent(bool Transparent){mTransparent = Transparent;};

  inline uint16 GetCurrentDataIndex(){return mCurrentDataIndex;};
  inline void   SetCurrentDataIndex(uint16 CurrentDataIndex){mCurrentDataIndex = CurrentDataIndex;};
  
  inline virtual uint16 GetDataCount(){return 1;};
  inline uint16 GetCellId(){return mCellId;};
  inline void SetCellId(uint16 CellId){mCellId = CellId;};
  void virtual PrepareForDraw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties)
	{
	  DC = DC;
    CellStatus = CellStatus;
    CellProperties = CellProperties;
	};
  
/*! \cond private */  
protected:
  void CalculateRealRect(BalDispRectT &Rect,DCC *DC,int16 BaselineTop);
  virtual void Draw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties)
  {
    DC = DC;
    CellStatus = CellStatus;
    CellProperties = CellProperties;
  };
/*! \endcond */   

public:
  virtual void ResetAnimation(){};
  virtual void DeliverMail(uint32 MsgId, void *MsgBufferP)
  {
    MsgId = MsgId;
    MsgBufferP = MsgBufferP;
  };
  virtual void DrawForMenu(DCC *DC,ItemStatusT &Status, MenuPropertiesT &MenuProperties)
  {
    DC = DC;
    Status = Status;
    MenuProperties = MenuProperties;
  };

public :
ImageC *GridImageP;
bool mIsGrid;
bool mHasDecode;
void SetGridImage(ImageC* pGridimage){GridImageP = pGridimage;}
void SetFlag(bool BoolParam){mIsGrid = BoolParam;} 
void SetHasDecode(bool BoolValue){mHasDecode = BoolValue;}
bool GetHasDecode(){return mHasDecode;}
};

class TextCellDataC: public CellDataC
{
/*! \cond private */  
protected:
  BVectorC mStringVector;//!<StringT*

  //!for animation text
  bool mNeedAnimation; //!<default value is FALSE
  bool mPositionResetFlag;//!<default value is FALSE
  uint16 mAnimationTextNum;//!<default value is 0
  BTimerC mAnimationTimer;
/*! \endcond */  
public:
  TextCellDataC(WinHandleT WinHandle);
  virtual ~TextCellDataC();

  virtual uint16 GetDataCount(){return mStringVector.GetSize();};
  void PushText(BStringC &CellText);
  void SetText(BStringC &CellText,uint16 DataIndex);
  void DeleteText(uint16 DataIndex);

  BStringC GetCurrentText(){return BStringC((StringT*)mStringVector[mCurrentDataIndex]);};

/*! \cond private */    
protected:
  BStringC GetDisplayText();  
  virtual void Draw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties);
  void OnAnimationText(void *MailMsgP);
  void DrawText();
  void DrawUnHighlightText(BalDispRectT Rect);
/*! \endcond */  
  
public:
  void virtual PrepareForDraw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties)
	{
    DC = DC;
    CellStatus = CellStatus;
    CellProperties = CellProperties;
	};

  virtual void ResetAnimation();
  virtual void DeliverMail(uint32 MsgId, void *MsgBufferP);
  virtual void DrawForMenu(DCC *DC,ItemStatusT &Status, MenuPropertiesT &MenuProperties);
};

class IconCellDataC: public CellDataC
{
/*! \cond private */    
protected:
  BVectorC mIconVector;/*const ImageResT* **/
/*! \endcond */  

public:
  IconCellDataC(WinHandleT WinHandle):CellDataC(WinHandle)
  {
    mIconVector.ReSet();
  };

  
  virtual ~IconCellDataC()
  {
  };

  void PushIcon(RESOURCE_ID(ImageResT) IconId);
  void SetIcon(RESOURCE_ID(ImageResT) IconId,uint16 DataIndex);
  void DeleteIcon(uint16 DataIndex);

  virtual uint16 GetDataCount(){return mIconVector.GetSize();};
/*! \cond private */    
protected:
  virtual void Draw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties);
/*! \endcond */  
  
public:
  void virtual PrepareForDraw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties)
	{
    DC = DC;
    CellStatus = CellStatus;
    CellProperties = CellProperties;
	};

  virtual void DrawForMenu(DCC *DC,ItemStatusT &Status, MenuPropertiesT &MenuProperties);
};

class NumberCellDataC: public CellDataC
{
protected:

public:
  NumberCellDataC(WinHandleT WinHandle):CellDataC(WinHandle)
  {
  };
  virtual ~NumberCellDataC()
  {
  };
/*! \cond private */    
protected:
  virtual void Draw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties);
/*! \endcond */  
  
public:
  void virtual PrepareForDraw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties)
	{
    DC = DC;
    CellStatus = CellStatus;
    CellProperties = CellProperties;
	};

  virtual void DrawForMenu(DCC *DC,ItemStatusT &Status, MenuPropertiesT &MenuProperties);
};

class AnimationCellDataC: public CellDataC
{
friend class ItemDrawerC;
/*! \cond private */    
protected:
   AnimationCtrlC *mAnimationP;
/*! \endcond */  

public:
  AnimationCellDataC(WinHandleT WinHandle):CellDataC(WinHandle),mAnimationP(NULL)
  {
  };
  virtual ~AnimationCellDataC()
  {
    if (NULL != mAnimationP)
    {
      WinHandleT WinHandle = mAnimationP->GetWinHandle();
      
      if (WinHandle >= WIN_HANDLE_MIN && WinHandle <= WIN_HANDLE_MAX)
      {    
        mAnimationP->SetParent(NULL);
        delete mAnimationP;
        mAnimationP = NULL;
      }
    }
  };

  void SetAnimation(RESOURCE_ID(AnimationCtrlResT) AnimationId);  
  void SetAnimation(AnimationCtrlC *AnimationP);
/*! \cond private */    
protected:
  virtual void Draw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties);
/*! \endcond */  

public:
  void virtual PrepareForDraw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties);
  virtual void DrawForMenu(DCC *DC,ItemStatusT &Status, MenuPropertiesT &MenuProperties);
};

class LeftArrowCellDataC: public CellDataC
{
public:
  LeftArrowCellDataC(WinHandleT WinHandle):CellDataC(WinHandle)
  {
  };
  virtual ~LeftArrowCellDataC()
  {
  };
/*! \cond private */    
protected:  
  virtual void Draw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties);
/*! \endcond */  

public:
  void virtual PrepareForDraw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties)
	{
    DC = DC; 
    CellStatus = CellStatus;
    CellProperties = CellProperties;
	};

  virtual void DrawForMenu(DCC *DC,ItemStatusT &Status, MenuPropertiesT &MenuProperties);
};

class RightArrowCellDataC: public CellDataC
{

public:
  RightArrowCellDataC(WinHandleT WinHandle):CellDataC(WinHandle)
  {
  };
  virtual ~RightArrowCellDataC()
  {
  };
/*! \cond private */    
protected:    
  virtual void Draw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties);
/*! \endcond */  
  
public:
  void virtual PrepareForDraw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties)
	{
    DC = DC;
    CellStatus = CellStatus;
    CellProperties = CellProperties;
	};

  virtual void DrawForMenu(DCC *DC,ItemStatusT &Status, MenuPropertiesT &MenuProperties);
};

class ItemDrawerC
{

BVectorC mCellVector;
BalDispRectT mRect; //!<the item rect or title rect
uint16 mCurrentDataIndex;//!<default value is 0
uint16 mParentCtrlType;
  ResourceIdT mItemFocusBgImg;
  ResourceIdT mItemUnFocusBgImg;
  ResourceIdT mItemUnFocusBgImgSection;
  WinHandleT mWinhandle;//alq 20080609

public:
  uint16 AppendCellData(CellDataC *CellDataP);
  void InsertCellData(uint16 CellIndex,CellDataC *CellDataP);
  void DeleteCellData(uint16 CellIndex);
  CellDataC* GetCellData(uint16 CellIndex);
  CellDataC* GetCellDataById(uint16 CellId);
  void SetItemIndex(uint16 ItemIndex);
  void SetCurrentDataIndex(uint16 CurrentDataIndex);
  
  inline uint16 GetCurrentDataIndex(){return mCurrentDataIndex;};
  inline void SetRect(BalDispRectT Rect){mRect = Rect;};
  void DeliverMail(uint32 MsgId, void *MsgBufferP);
  virtual void ResetAnimation();
  int ScrollData(DataScrollT ScrollType);
  int GetCellCount(void){return mCellVector.GetSize();}

  void SetParentWinHandle(WinHandleT winhandle){mWinhandle = winhandle ;};//alq 20080709 : handle when the top dialog is not listdialog.
  WinHandleT GetParentWinhandle(void){return mWinhandle;};//alq 20080709

public:
  ItemDrawerC():mCurrentDataIndex(0){mCellVector.ReSet();mParentCtrlType = 0;};
  virtual ~ItemDrawerC();
  virtual void DoLowerDrawItem(DCC *DC,ItemStatusT &Status,MenuPropertiesT &MenuProperties);
  void SetParentCtrlType(uint16 style){mParentCtrlType = style;};
  void SetItemBgImg(ResourceIdT focus, ResourceIdT unfocus, ResourceIdT unfocussection);

  void PrepareForDraw(DCC *DC,ItemStatusT &Status,MenuPropertiesT &MenuProperties);
/*! \cond private */    
protected:
  virtual void DrawBack(DCC *DC,ItemStatusT &Status,MenuPropertiesT &MenuProperties);
  virtual void DrawCells(DCC *DC,ItemStatusT &Status,MenuPropertiesT &MenuProperties);
  uint16 GetDataCount();
/*! \endcond */  
    
};

class MenuDrawerC
{
/*! \cond private */    
protected:
  MenuModelC *mMenuModelP;
  bool mSupportBgImg;/*default value is FALSE*/
  uint16 mTitleMargin;
  uint16 mItemMargin;
  WinHandleT mWinHandle;
/*! \endcond */  
public:
  MenuDrawerC():mMenuModelP(NULL), mSupportBgImg(FALSE), mTitleMargin(2), mItemMargin(2){};
  virtual ~MenuDrawerC(){};
  
  inline void SetMenuModel(MenuModelC *MenuModelP){mMenuModelP = MenuModelP;};
  inline void SetSupportBgImg(bool Support){mSupportBgImg = Support;};
  inline bool GetSupportBgImg(){return mSupportBgImg;};
  inline void SetTitleMargin(uint16 Margin){mTitleMargin = Margin;};
  inline uint16 GetTitleMargin(){return mTitleMargin;};
  inline void SetItemMargin(uint16 Margin){mItemMargin = Margin;};
  inline uint16 GetItemMargin(){return mItemMargin;};
  inline void SetWinHandle(WinHandleT WinHandle){mWinHandle = WinHandle;};
public:  
  virtual ItemDrawerC* GetItemDrawer(uint16 ItemIndex) = 0;
  virtual ItemDrawerC* GetTitleDrawer(uint16 ItemIndex) = 0;
  virtual void         DoDrawBack(DCC *DC,MenuPropertiesT &MenuProperties) = 0;
};

class InnerMenuDrawerC: public MenuDrawerC
{
public:
  InnerMenuDrawerC(){};
  virtual ~InnerMenuDrawerC(){};
/*! \cond private */    
protected:
  ItemDrawerC* GetDefaultItemDrawer(uint16 ItemIndex);
  ItemDrawerC* GetStandardIconItemDrawer(uint16 ItemIndex);
  ItemDrawerC* GetStandardNoIconItemDrawer(uint16 ItemIndex);
  ItemDrawerC* GetRadioboxItemDrawer(uint16 ItemIndex);
  ItemDrawerC* GetCheckboxItemDrawer(uint16 ItemIndex);
  ItemDrawerC* GetIconCheckboxItemDrawer(uint16 ItemIndex);
  ItemDrawerC* GetAnimationItemDrawer(uint16 ItemIndex);
  ItemDrawerC* GetGridIconTextItemDrawer(uint16 ItemIndex);
  ItemDrawerC* GetGridAnimationItemDrawer(uint16 ItemIndex);
  ItemDrawerC* GetGridIconOnlyItemDrawer(uint16 ItemIndex);

  ItemDrawerC* GetDefaultTitleDrawer(uint16 CurrentItemIndex);
  ItemDrawerC* GetLeftIconTitleDrawer(uint16 CurrentItemIndex);
  ItemDrawerC* GetRightIconTitleDrawer(uint16 CurrentItemIndex);
  ItemDrawerC* GetBothIconTitleDrawer(uint16 CurrentItemIndex);
  ItemDrawerC* GetLeftAnimationTitleDrawer(uint16 CurrentItemIndex);
  ItemDrawerC* GetRightAnimationTitleDrawer(uint16 CurrentItemIndex);
  ItemDrawerC* GetIconWithTextTitleDrawer(uint16 CurrentItemIndex);  

  void DecorateTitleDrawer(ItemDrawerC *TitleDrawerP,BalDispRectT &TitleDrawerRect);

  uint16 GetImageWidth(RESOURCE_ID(ImageResT) IconId);
  uint16 GetAnimationWidth(ResourceIdT ResourceId);
/*! \endcond */  
  
public:  
  virtual ItemDrawerC* GetItemDrawer(uint16 ItemIndex);
  virtual ItemDrawerC* GetTitleDrawer(uint16 CurrentItemIndex);
  virtual void         DoDrawBack(DCC *DC,MenuPropertiesT &MenuProperties);


};




#endif

