

#ifndef UIMENUDEFINE_H
#define UIMENUDEFINE_H

#include "baldispapi.h"

#define SCROLL_BAR_WIDTH 5

 //! the grid menu detail information
typedef struct
{
 uint16 RowCount;
 uint16 ColumnCount;
 uint16 ItemWidth;
 uint16 ItemHeight;
 uint16 RowGap;
 uint16 ColumnGap;
}GridMenuDetailT;

 //! the menu title type
enum MenuTitleTypeT
{
MENU_TITLE_TYPE_DEFAULT = 0,
MENU_TITLE_LEFT_ICON,
MENU_TITLE_RIGHT_ICON,
MENU_TITLE_BOTH_ICON,
MENU_TITLE_LEFT_ANIMATION,
MENU_TITLE_RIGHT_ANIMATION,
MENU_TITLE_ICON_WITH_TEXT
};

 //! the menu  type
enum MenuTypeT
{
MENU_LIST_DEFAULT = 0,
MENU_LIST_STANDARD_ICON,
MENU_LIST_STANDARD_NOICON,
MENU_LIST_RADIOBOX,
MENU_LIST_CHECKBOX,
MENU_LIST_ICON_CHECKBOX,
MENU_LIST_ANIMATION,
MENU_GRID_ICON_ONLY,
MENU_GRID_ICON_TEXT,
MENU_GRID_ANIMATION,
MENU_LIST_CUSTOMIZE = 20,
MENU_GRID_CUSTOMIZE,
MENU_EX_TYPE,//Add by BYD
MENU_EX_GRID_TYPE,//Add by BYD
MENU_EX_GRID_MORE_TYPE,//Add by BYD
MENU_SETTING_VOLUME_TYPE = 40,
MENU_LIST_POPUP,
MENU_LIST_EDIT  //add for qwert: only use for editor

};

 //! the menu size
typedef struct
{
uint16 TitleHeight;
uint16 ItemHeight;
uint16 HighlightItemHeight;
uint16 TitleItemGap;
}MenuSizeT;

 //! the menu flag
enum MenuFlagT
{
MENU_FLAG_ITEM_SERIAL_NUMBER_HAS_RECT                 = 0x00000001,
MENU_FLAG_NAVIGATE_USE_LEFT_RIGHT_KEY                 = 0x00000002,
MENU_FLAG_ITEM_NO_HIGHLIGHT                           = 0X00000004,
MENU_FLAG_ITEM_NO_3D_EFFECT                           = 0X00000008,
MENU_FLAG_SELECTED_ITEM_NO_BACKGROUND                 = 0X00000010,

//! for the menu title
MENU_TITLE_FLAG_SHOW_COUNG_INDEX      = 0x01000000,
MENU_TITLE_FLAG_SHOW_ITEM_TEXT        = 0x02000000,
MENU_TITLE_FLAG_SHOW_LEFT_RIGHT_ARROW = 0x04000000,
MENU_TITLE_FLAG_SHOW_CURRENT_INDEX    = 0x08000000
};

 //! the prepare data type for huge menu
enum PrepareTypeT
{
PREPARE_TYPE_NEXT,
PREPARE_TYPE_PREVIOUS,
PREPARE_TYPE_RESET
};

 //! the data scroll type
enum DataScrollT 
{
DATA_SCROLL_LEFT,
DATA_SCROLL_RIGHT
}; 

 //! the Menu scroll type
enum MenuScrollT 
{
  MENU_SCROLL_ITEM_BY_PAGE,     //when focus the first or last item of the screen, scroll screen 
  MENU_SCROLL_END_ITEM_BY_PAGE, //when focus the first or last item of all items, scroll screen 
  MENU_SCROLL_END_ITEM_STOP, //when focus the first or  last item of all items,can't scroll
  MENU_SCROLL_END_ITEM_CONTINUE //no matter which item focused, scroll only one item
}; 

enum MenuSelectionT
{
  MENU_SINGLE,
  MENU_MULTI
}; //ZLL

 //! the menu colors type
typedef struct 
{
  BalDispColorT MMBorderCol;       //!<Menu border color 
  //! for the menu title
  BalDispColorT MMTitleCol;        //!<Menu title color(text) 
  BalDispColorT MMTitleBgCol;    //!<Menu title background color 
  BalDispColorT MMTitleTopBorderCol;//!<for 3D effect 
  BalDispColorT MMTitleBottomBorderCol;//!<for 3D effect 
  BalDispColorT MMTitleLeftBorderCol;//!<for 3D effect 
  BalDispColorT MMTitleRightBorderCol;//!<for 3D effect
  //! for the menu item
  BalDispColorT MMItemCol;         //!<Menu item color(text) 
  BalDispColorT MMItemBgCol;       //!<Menu item background color 
  BalDispColorT MMSelItemCol;      //!<Menu selected item color(text) 
  BalDispColorT MMSelItemBgCol;   //!<Menu selected item background color 
  BalDispColorT MMSelItemTopBorderCol; //!<for 3D effect 
  BalDispColorT MMSelItemBottomBorderCol;//!<for 3D effect 
  BalDispColorT MMSelItemLeftBorderCol; //!<for 3D effect 
  BalDispColorT MMSelItemRightBorderCol;//!<for 3D effect 
 
  //! serial number
  BalDispColorT MMSNBgCol;//!<serial number back color 
  BalDispColorT MMSNTxtCol;//!<serial number text color 
  BalDispColorT MMSNSelBgCol;//!<serial number back color of selected item 
  BalDispColorT MMSNSelTxtCol;//!<serial number text color of selected item 

  //! lose fcous
  BalDispColorT MMLFItemCol;         //!<Menu item color(text) 
  BalDispColorT MMLFItemBgCol;       //!<Menu item background color 
  BalDispColorT MMLFSelItemCol;      //!<Menu selected item color(text) 
  BalDispColorT MMLFSelItemBgCol;    //!<Menu selected item background color 
  BalDispColorT MMLFSelItemTopBorderCol; //!<for 3D effect 
  BalDispColorT MMLFSelItemBottomBorderCol;//!<for 3D effect 
  BalDispColorT MMLFSelItemLeftBorderCol; //!<for 3D effect 
  BalDispColorT MMLFSelItemRightBorderCol;//!<for 3D effect 
}MenuColorsT;

 //! the menu event type
typedef enum 
{
  MENU_EVENT_CHECKBOX_STATUS_CHANGE = 0x1  
} MenuEventT;




#endif

