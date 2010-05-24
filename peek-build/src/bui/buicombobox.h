#ifndef UICOMBOBOX_H
#define UICOMBOBOX_H

/******************************************************************************
* 
* FILE NAME   : buicombobox.h
*
* DESCRIPTION :
*
*   This is the interface for the ComboBoxControlC class.
*
* Copyright (c) 2007, BYD Technologies, Inc.
******************************************************************************/

#include "buicontrol.h"
#include "buistatictextctrl.h"
#include "buimenuctrl.h"
#include "buidcc.h"
#include "buistring.h"
#include "restypedefines.h"
#include "buimenuexctrl.h"//hebo

#define QWERT_COMBOBOX_TEST_MONPRINTF

typedef enum
{
  COMBOBOX_CHILDCTRL_TITLE = 0,
  COMBOBOX_CHILDCTRL_ICON,
  COMBOBOX_CHILDCTRL_MENU,
  COMBOBOX_MAX
}COMBOBOX_CHILDCTRL_TYPE;

typedef enum
{
  COMBOBOX_WIDTH_STYLE_FIXED = 0,
  COMBOBOX_WIDTH_STYLE_VARIABLE,
  COMBOBOX_WIDTH_STYLE_MAX
}COMBOBOX_WIDTH_STYLE;

typedef enum
{
  COMBOBOX_ORIENTATION_DOWN = 0,
  COMBOBOX_ORIENTATION_UP,
  COMBOBOX_ORIENTATION_MAX
}COMBOBOX_ORIENTATION;

typedef enum
{
  COMBOBOX_VSCROLLTYPE_DEFAULT = 0,
  COMBOBOX_VSCROLLTYPE_TXTBL,//reserve
  COMBOBOX_VSCROLLTYPE_NONE,
  COMBOBOX_VSCROLLTYPE_MAX
}COMBOBOX_VSCROLLTYPE;

typedef struct
{
  BalDispColorT TextColor;      //!< Text Color
  BalDispColorT BorderColor;    //!< Border Color, just like PenColor in DC
  BalDispColorT BKColor;        //!< Background Color, just like BrushColor in DC
}ComboBoxCtrlColorsT;

class ComboBoxControlC : public ControlC
{

private:
  StaticTextCtrlC *mTitleP;
  MenuCtrlC *mMenuP;
  BalDispRectT mResRect;
  bool mSelected;
  bool mLoadResourced;
  uint16 mMaxItemNum;
  uint16 mMaxWidth;
  uint16 mMinWidth;
  uint16 mWidth;
  uint16 mMaxStringWidth;
  COMBOBOX_WIDTH_STYLE mWidthStyle;
  COMBOBOX_ORIENTATION mOrientation;
  BStringC mTitleDisp;
  BStringC mTitleFull;
  BalDispRectT mTitleRect;
  uint16 mVisiableItems;
  uint16 mDispItemCount;
  COMBOBOX_VSCROLLTYPE mVScrollType;
  uint16 mVScrollWidth;
  //bool mFirstDownDrop;
  BalDispFontT mFont;
  ImageC* IconImg;
  ImageC* UpMarginImg;
  ImageC* DownMarginImg;
  ImageC* UpMarginArrowImg;
  ImageC* DownMarginArrowImg;
  
private:
  bool AdjustLayout();
  void AdjustDispTitle();


public:
  ComboBoxControlC();
  virtual ~ComboBoxControlC();

  /*Get title's text in str, if success, return TRUE*/
  bool GetTitleText(BStringC &str);

  /*Set title's text with str, if success, return TRUE*/
  bool SetTitleText(BStringC str);

  /*Set title's text from down-drop menu's item ,witch is specified by itemindex, if success, return TRUE*/
  bool SetTitleText(uint16 itemindex);

  bool IsSelectStatus();//reserve

  /*Verify down-drop menu is spreaded*/
  bool IsSpreaded();

  /*Spread  down-drop menu, If failed, return FALSE*/
  bool SpreadCombobox();

  /*Close down-drop menu, If failed, return FALSE*/
  bool CloseCombobox();
  
  /*If failed, return FALSE*/
  bool InsertItem(BStringC &str, uint16 index);
  bool InsertDyItem(BStringC &str, uint16 index);

  /*If failed, return FALSE*/
  bool DeleteItem(uint16 index);

  /*If failed, return FALSE*/
  bool DeleteItem(BStringC &str);

  /*Get Focus item index, If failed, return -1*/
  int GetSelectItemIndex();

  /*If failed, return FALSE*/
  bool GetSelectItem(BStringC &str);

  /*If failed, return FALSE*/
  bool GetItemTextByIndex(BStringC &str, uint16 index);

  /*If failed, return -1*/
  int GetItemIndexByText(BStringC &str);

  /*Select item by index, if success, return TRUE*/
  bool SelectItemByIndex(uint16 index);

  /*Select item by str, if success, return TRUE*/
  bool SelectItemByText(BStringC &str);
  
  /*The default value is COMBOBOX_ORIENTATION_DOWN, in this case, the down-drop menu will be placed under the title
  if the value is COMBOBOX_ORIENTATION_UP, the title will be placed under down-drop menu*/
  void SetOrientation(COMBOBOX_ORIENTATION orientation = COMBOBOX_ORIENTATION_DOWN);

  /*Set combobox's width style:
 The default value is COMBOBOX_WIDTH_STYLE_FIXED, in this case, the width will be set to the maximal one among 
      Rect.dx defined in Resource File, length if called SetWidth(length) and combobox images's width TXTBL has offered.
      The images include title background image, highlight image, up and down margin image, etc.
 If widthstyle is COMBOBOX_WIDTH_STYLE_VARIABLE, the combobox will try to show all item text whole, but the combobox's
      width should be longer than title background img's width, and be limitted by MaxWidth and MinWidth ,the MaxWidth and 
      MinWidth is set when App call SetMaxWidth(uint16 length) and SetMinWidth(uint16 length). if App never call 
      SetMaxWidth(uint16 length) or SetMinWidth(uint16 length = 0), the width will be not restricted.
  */
  void SetWidthStyle(COMBOBOX_WIDTH_STYLE WidthStyle = COMBOBOX_WIDTH_STYLE_FIXED);

  /*If SetWidthStyle set width-style as COMBOBOX_WIDTH_STYLE_VARIABLE, length will limit combobox's max-width */
  void SetMaxWidth(uint16 length = 0);

  /*If SetWidthStyle set width-style as COMBOBOX_WIDTH_STYLE_VARIABLE, length will limit combobox's min-width*/
  void SetMinWidth(uint16 length = 0);

  /*If SetWidthStyle set width-style as COMBOBOX_WIDTH_STYLE_FIXED, combobox's width will be set to length, But 
  if length less than Rect.dx witch is specified in Resource File, the width will be set to Rect.dx*/
  void SetWidth(uint16 length = 0);

  /*Set screen display item count of down-drop menu*/
  void SetVisibleItems(uint16 count  );

  uint16 GetVisibleItems();

  /*Currently there are two options for vertical scrollbar: COMBOBOX_VSCROLLTYPE_DEFAULT and COMBOBOX_VSCROLLTYPE_NONE,
  The second param width is reserved*/
  void InitMenuVerticalScroll(COMBOBOX_VSCROLLTYPE scrolltype = COMBOBOX_VSCROLLTYPE_DEFAULT, uint16 width = 10);

  uint16 GetItemCount(); 
  BalDispFontT SetTextFont(BalDispFontT TextFont);


protected:
  virtual bool LoadResource(ResourceIdT ResourceId);
   virtual void Draw(DCC *DcP);


// Message Handler
protected:
  void OnSetFocus(WindowC* OldWinP);  
  void OnKillFocus(WindowC* NewWinP);
  void OnChangeFocus(uint32 Direction);  	
  bool OnKeyPress(uint32 KeyCode);
  void OnHideNotify(void);
  void OnShowNotify(void);
  
  DECLARE_MAIL_MAP()
};
/***************************************************************************
*Keypress:

KP_BACKSPACE_KEY : If the combobox is spreaded before key pressed, the combobox will close the down-drop menu.
                                  If the combobox is not spreaded before key pressed, the combobox will send a WINMSG_NOTIFY 
                                  to APP. The ParamA is set with CTRL_TYPE_COMBOBOX , the ParamB is set with
                                  WINMSG_COMBOBOX_BACKSPACE, App will recv the msg in MyAppC::HandleWinNotify.
KP_ENTER_KEY          : If the combobox is spreaded before key pressed, the combobox will close the down-drop menu.
                                  And set title text with selected item. And the combobox will send a WINMSG_NOTIFY to APP, the 
                                  ParamA is set with: CTRL_TYPE_COMBOBOX , ParamB is set with 
                                  WINMSG_COMBOBOX_SELECTITEM, App will recv the msg in MyAppC::HandleWinNotify.
                                  If the combobox is not spreaded before key pressed, the combobox will spread the down-drop menu
                                  And select item with titie text. If title text is not in down-drop menu, combobox will select the first 
                                  item in down-drop menu.
For example:
void MyAppC::HandleWinNotify(DialogC* DialogP, uint32 ParamA, int32 ParamB)
{
  if (ParamA == CTRL_TYPE_COMBOBOX && ParamB == WINMSG_COMBOBOX_BACKSPACE)		
  {
     //Add your code here:

     return ;
  }
  if (ParamA == CTRL_TYPE_COMBOBOX && ParamB == WINMSG_COMBOBOX_SELECTITEM)		
  {
     //Add your code here:

     return ;
  }
}




KP_UP_KEY               : If the combobox is spreaded before key pressed, the down-drop menu will select the prev item. If
                                  the selected item is the first one, the combobox will do nothing.
                                  If the combobox is not spreaded before key pressed, And the combobox 's parent has only one child
                                  control, the combobox will do nothing, and can recv focus msg as before. If the combobox's parent 
                                  has more than one child control, the combobox will lose focus, and the focus will change to the prev 
                                  control.
KP_DOWN_KEY          : If the combobox is spreaded before key pressed, the down-drop menu will select the next item. If
                                  the selected item is the last one, the combobox will do nothing.
                                  If the combobox is not spreaded before key pressed, And the combobox 's parent has only one child
                                  control, the combobox will do nothing, and can recv focus msg as before. If the combobox's parent 
                                  has more than one child control, the combobox will lose focus, and the focus will change to the next 
                                  control.
****************************************************************************/
#endif
