
#include "buidialog.h"
#include "buimenuexmodel.h"
#include "buimenuexctrl.h"
#include "buistatictextdata.h"
#include "buidialogutils.h"

#ifndef UILISTDIALOG_H
#define UILISTDIALOG_H 

typedef enum
{
	ISI_TEXT0 = 0,
	ISI_TEXT1,
	ISI_TEXT2,
        ISI_TEXT3,        // add by zhanglanlan
        ISI_TEXT4,
	
	ISI_IMG0,
	ISI_IMG1,
	ISI_IMG2,
	ISI_CHK,	
	ISI_END
}ItemSubIndexT;

//added by hebo 2008.03.10
typedef enum
{
    FONT_STYLE_NORMAL_TYPE = 0,
    FONT_STYLE_BOLD_TYPE,
    FONT_STYLE_UNDERLINE_TYPE
 }ItemFontStyleT;
// add by zhanglanlan
typedef enum
{
  MULTI_MARK = 0,
  SINGLE_MARK
 }MarkTypeT;

//add by luoyu in 2007.10.15

typedef struct 
{
ItemSubIndexT  ItemSubIndex;//cell style exp: ISI_TEXT0...ISI_IMG0...
uint32  x;                //cell position x 
uint32  y;               //cell position y
uint32  dx;             //cell position dx
uint32  dy;            //cell position dy
BalDispFontT   font;  //cell font
BalDispColorT TextColors;  //fond color
BalDispColorT HighLightTextColor; //item is selected,fond color
uint16   AlignFlag ;//high 8-bits set  VAlign 010 OR 001 OR 000 ; low 8-bits set HAlign 010 OR 001 OR 000 .   ex.  0x11 =  (BAL_DISP_VALIGN_CENTER , BAL_DISP_HALIGN_CENTER)                                                                          
} SubItemStyleInfoT;

#define MAXNUM 6
typedef struct  
{
  DlgMStyleT DMStyle;    
  uint32  nCells;             //the num of cells  in the item
  SubItemStyleInfoT  subiteminfo[MAXNUM];//cells info
}ItemInfoT;



//end  by luoyu in 2007.10.15


//! Dialog class define
class ListDialogC : public DialogC
{
public:
  ListDialogC(DlgMStyleT style);
  virtual ~ListDialogC();

  //bool SetData(ResourceIdT ResourceId);//Update Data by Resource Id
  bool CreateEx(void* ExtraDataP,uint32 WinId = 0, uint32 Style = WINSTYLE_SKBAR, DispLCDTypeT LcdType = LCD_MAIN);
  bool CreateEx(void* ExtraDataP,RESOURCE_ID(StringT) TitleResId, uint32 WinId = 0, uint32 Style = WINSTYLE_SKBAR, DispLCDTypeT LcdType = LCD_MAIN);
  void SetData(RESOURCE_ID(BYDMenuCtrlDataT) ResourceId);
  bool InsertItem(uint16 ItemIndex, BYDMenuItemC *ItemP,bool bUpdate = FALSE);
  bool AppendItem(BYDMenuItemC *ItemP,bool bUpdate = FALSE);
  bool DeleteItem(uint16 ItemIndex,bool bUpdate = FALSE);
  bool DeleteItemAll(bool bUpdate = FALSE);
  bool AddSubItemDataByData(uint16 ItemIndex,uint16 SubIndex,void **pvData,uint32 nMsgId = 0, uint32 dwParam = 0);
  bool AddSubItemData(uint16 ItemIndex, uint16 SubIndex, ResourceIdT ResId, uint32 nMsgId = 0, uint32 dwParam = 0);
  bool GetSubItemData(uint16 ItemIndex,uint16 SubIndex,void **pvData,uint32& nValue);
  void SetItemMessage(uint16 ItemIndex,uint32 nMsgId,uint32 nParameter = 0);
  void SetItemDim(uint16 ItemIndex,bool bDim);
  bool GetItemDim(uint16 ItemIndex);
  MenuItemC* GetItem(uint16 ItemIndex);
  uint16 GetItemCount();
  void SetFlags(uint32 dwFlags);
  void SetMyDyMenu(bool bDyMenu){m_bDyMenu = bDyMenu;}
  bool ChangeStyle(DlgMStyleT DlgStyle);
  uint16 GetFocusIndex();  // get the currentItem
  void SetFocusIndex(uint16 focus); //set the currentItem
  void SetHelpText(bool bShow){bHelpTextShow = bShow;};
  ItemInfoT* GetItemStyleInfo();
  virtual void SetTitle(const BStringC& Title);
  virtual void SetTitle(RESOURCE_ID(StringT) ResId);
  void SetEmptyString(BStringC &strEmpty);
  void SetDyMenuInitInfo(uint16 nMaxCount,ItemDataFuncT cbDataFunc = NULL);
  virtual ControlC* GetControlByType(CtrlTypeT ControlType, uint8 Number = 0);
  virtual void RemoveChild(const WindowC* ChildWinP);
  void SetItemHelpText(uint16 ItemIndex,RESOURCE_ID(StringT) Text);
  void GetItemHelpText(RESOURCE_ID(StringT)& Text,uint16 ItemIndex);
  //For Mark
  void CheckItem(uint16 ItemIndex,bool bCheck); //No handle Mark Softkey
  bool GetMark(uint16 ItemIndex);
  void SetMark(int32 ItemIndex,bool bMark);
  bool GetMarkState(void);
  void SetAlwaysMark(bool bAlways){m_bAlwaysMark = bAlways;}
  void SetMarkSelect();
  void SetMarkAll(bool bMark);
  void SetIsTitleHaveIndex(bool IsHave){mIsTitleHaveIndex = IsHave;}
  bool GetIsTitleHaveIndex(void){return mIsTitleHaveIndex;}
  void SetTitleIndexImage(RESOURCE_ID(ImageResT) ImageID);
  RESOURCE_ID(ImageResT) GetTitleIndexImage(void){return mTitleIndexImage;}
  void SetAlwaysScrollbar(bool bShow);
  uint16 GetScreenItemCount();
  void SetScreenItemCount(int16 nItemScreen);

  //For Reorder
  void SetReorder(bool bStart, uint8 *pArrIndex = NULL, uint32 dwMsgId = 0);
  void SwitchOrderIndex(void);
  bool GetReorderArrIndex(uint8** ppArrIndex,uint16& nCount);
  //For Dynamic Decode of PNG or GIF
  void SetDynamicDecode(bool BoolValue ){ mDyDecode = BoolValue;}
  bool GetDynamicDecode(){return mDyDecode;}
  void SetIconCellIndex(uint16 index){mIconIndex = index;}
  uint16 GetIconCellIndex(){return mIconIndex;}

  void SetSubItemVisible(uint16 nSubItemIndex,bool bVisible);

//added by hebo 2008.03.10
 void SetListFontStyle( BalDispFontT FontStyle);
 void  SetItemFontStyle( uint16 ItemIndex, BalDispFontT FontStyleT);

	//for no mark but also show icon
  void	SetShowMarkIcon(bool isShow);
  bool	IsShowMarkIcon(void){return mShowMarkIconStyle;}

  void SetMarkType(MarkTypeT type);
  MarkTypeT GetMarkType(){return mMarkType;}
  void  SetListScrollType(MenuScrollT ScrollType);
  MenuScrollT GetListScrollType();
  BVectorC* GetSelectedItems();
  void  DeleteSelectedItems();

  
protected:
  bool mDyDecode;//add 1219
  uint16 mIconIndex;//add 1219
  BStringC mMenuTitle;
  
  int32 m_nMarkcount;
  DlgMStyleT mDlgMStyle;

  //for Reorder...
  bool m_bReorder;
  bool m_bReorderMove;
  bool m_bReorderDone;
  uint8 *m_pArrIndex;
  uint16 m_nReorderIndex;
  uint16 m_nPreOrderIndex;
  uint32  m_dwMsgIdReorder;
  //For Play End Sound When Scroll to
  bool m_bIsPlaySound;

protected:
  bool OnKeyPress(uint32 KeyCode);
  bool OnKeyHold(uint32 KeyCode);//add 1.2
  int32 OnCreate(void);
  void OnScroll(uint16 nCurIndex, uint32 nReason);
  MenuCtrlC* GetMenuCtrl(void);
  
  virtual void Draw(DCC* DcP);  
  
private:    
  bool bHelpTextShow;  
  bool m_bDyMenu;
  BTimerC mHelpPreTimer;//added by chend1201
  BStringC mLSK;
  BStringC mMSK;
  BStringC mRSK;
  bool m_bAlwaysMark;
  bool mIsTitleHaveIndex;
  RESOURCE_ID(ImageResT) mTitleIndexImage;
  BStringC m_sEmpty;

  bool  mShowMarkIconStyle;//added by hebo for no mark but also show icon
  MarkTypeT mMarkType;
  int16 mLastCheckedIndex;
  
private:
  void SetItemData(BYDMenuItemC *ItemP,uint16 SubIndex,ResourceIdT ResId);
  bool LGetStyleDlgInfo(ResourceIdT& dlgIDD, ResourceIdT& dlgItemBgIDI ,ResourceIdT& dlgunselectItemBgIDI, ResourceIdT & dlgbackgroundIDI); // zhanglanlan  alq 20080428
  bool LDeleteCount(int16 nStep,bool bAuto = TRUE);
  bool LDeleteCheckedItems(uint16 ItemIndex);
  bool LIsScreenItem(uint16 ItemIndex);
  BVectorC *LGetCheckedItems();
  bool LUpdateCheckItems(uint16 ItemIndex,bool bInsert = TRUE);
  void SetMarkMSoft(int32 ItemIndex = -1);
  void SetReorderSoftKey(bool bReorder);
  void SetTimerStop(void){mHelpPreTimer.StopTimer();}
  void SetTimerStart(void);
  void OnHelpTextTimeOut(void);
  void LPlayEndSound(bool bPlay);
  bool LGetHelpTextShow();
  void LSetResouceTitle(ResourceIdT dlgIDD);
  bool  IsWinStyle(uint32 Style);//alq 20080504
  
  DECLARE_MAIL_MAP()
};





#endif

