

#ifndef UIEDITDEFINE_H
#define UIEDITDEFINE_H

#include "baldispapi.h"

#define INTERNATIONAL_PREFIX_SYMBOL     '+'       //!< International prefix symbol
#define TIME_PAUSE_SYMBOL               'P'       //!< Time pause symbol
#define HARD_PAUSE_SYMBOL               'W'       //!< Hard pause symbol
#define STAR_SYMBOL                     '*'       //!< Star symbol
#define POUND_SYMBOL                    '#'       //!< Pound symbol
#define SEMICOLON_SYMBOL                ';'       //!< Semicolon symbol  
#define NEWLINE_SYMBOL                  '\n'      //!< New line symbol
#define EMPTY_SYMBOL                    ' '       //!< Empty symbol
#define SEMICOLON_STRING_SYMBOL         ";;"
#define LEFT_BRACKET_SYMBOL             '['
#define RIGHT_BRACKET_SYMBOL            ']'
//! Edit notify event
typedef enum 
{
  EDIT_EVENT_DATA_EMPTY,            //!< Edit's content is changed from not empty state to empty state
  EDIT_EVENT_DATA_NOT_EMPTY,        //!< Empty to not empty state
  EDIT_EVENT_DATA_FULL,             //!< Not full to full state
  EDIT_EVENT_DATA_NOT_FULL,         //!< Full to not full state
  EDIT_EVENT_DATA_OVERFLOW,         //!< Notified while adding content to a full edit
  EDIT_EVENT_RECT_RESIZED,          //!< Edit's rect is resized
  EDIT_EVENT_DATA_UPDATED,          //!< Edit's content is updated
  EDIT_EVENT_SCROLL_END,            //   For QWERT: Get (large) email body when scroll down to the end
  EDIT_EVENT_SCROLL_BEGIN,          //   For QWERT: Send when scroll to the begin
  EDIT_EVENT_SYMBOL_ADD             //   For QWERT:Add symbol by dialog or wheel for address.
} EditEventT;

//! Edit style
typedef enum
{
  EDIT_STYLE_LEFT             = 0x0000,        //!< Edit's content is left align
  EDIT_STYLE_CENTER           = 0x0001,        //!< Edit's content is center align 
  EDIT_STYLE_RIGHT            = 0x0002,        //!< Edit's content is right align
  EDIT_STYLE_MULTILINE        = 0x0004,        //!< Edit's is mutiline, display v-scrollbar automatically
  EDIT_STYLE_AUTOVSCROLL      = 0x0008,        //!< Reserved, not being used now
  EDIT_STYLE_BORDER           = 0x0010,        //!< Edit having border or not
  EDIT_STYLE_PASSWORD         = 0x0020,        //!< Edit is in password mode
  EDIT_STYLE_CALL             = 0x0040,        //!< Edit is in call mode 
  EDIT_STYLE_NUMERIC          = 0x0080,        //!< Edit is in numeric mode
  EDIT_STYLE_NUMBER           = 0x0100,        //!< Edit is in number mode
  EDIT_STYLE_READONLY         = 0x0200,        //!< Edit is in readonly mode
  EDIT_STYLE_ADDRESS          = 0X0400,        //!< Edit is in address mode
  EDIT_STYLE_MAX              = 0x7FFFFFFFL
} EditStyleT;

//! Edit resource value of mode indication
typedef enum
{
  EDIT_MODE_NORMAL,                 //!< Normal mode, showing caret, IME box, number box
  EDIT_MODE_PASSWORD,               //!< Password mode, showing caret, not showing IME box, number box
  EDIT_MODE_CALL_NUMBER,            //!< Call number mode, not showing caret, IME box, number box
  EDIT_MODE_NUMERIC,                //!< Numeric mode, showing caret, number box, not showing IME box
  EDIT_MODE_NUMBER,                 //!< Number mode, showing caret, number box, not showing IME box
  EDIT_MODE_ADDRESS,                //!< Address mode, showing caret, not showing number box, not showing IME box
  EDIT_MODE_MAX
} EditModeT;

//! Edit's 8 bit/7 bit latin char supporting indication
typedef enum
{
  EDIT_CODEPAGE_LATIN_8_BIT,        //!< Edit support 8 bit latin char
  EDIT_CODEPAGE_LATIN_7_BIT         //!< Edit support 7 bit latin char
} EditCodePageLatinModeT;

//! Edit's preview mode supporting indication
typedef enum
{
  PREVIEW_MODE_OFF,                 //!< Preview mode off
  PREVIEW_MODE_TEXT                 //!< Reserved, not being used now. Show some chars and '...'
} PreviewModeT;

//! Edit's ime area indication
typedef enum
{
  IME_AREA_GLOBAL           = 0,    //!< Edit supporting all IME
  IME_AREA_EAST_ASIA        = 1,    //!< Edit supporting IME in ease asia
  IME_AREA_SOUTH_ASIA       = 2,
  IME_AREA_SOUTHEAST_ASIA   = 3,
  IME_AREA_MIDDLE_EAST      = 4,
  IME_AREA_WEST_EUROPE      = 5,
  IME_AREA_EAST_EUROPE      = 6,
  IME_AREA_NORTH_EUROPE     = 7,
  IME_AREA_NORTH_AMERICA    = 8,
  IME_AREA_SOUTH_AMERICA    = 9,
  // ...
  IME_AREA_MAX
} ImeAreaT;

//! Edit's color
typedef struct 
{
  BalDispColorT BorderColor;     //!< Edit border color; 

  // Text area
  BalDispColorT TextColor;       //!< Edit text color; 
  BalDispColorT BkColor;         //!< Edit background color; 
  BalDispColorT FocusBkColor;    //!< Edit text focus background color
  BalDispColorT SelBkColor;      //!< Edit selected text backgroud color

  // Title area
  BalDispColorT TitleTextColor;  //!< Title text color; 
  BalDispColorT TitleBkColor;    //!< including title, IMEBox, NumberBox

  // Caret
  BalDispColorT CaretColor;      //!< Caret color; 
} EditColorsT;



#endif

