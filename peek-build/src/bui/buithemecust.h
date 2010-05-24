

#ifndef UITHEMECUST_H
#define UITHEMECUST_H

#include "baldispapi.h"

#include "restypedefines.h"

#include "buimenudefine.h"
#include "buiscrollbar.h"
#include "buieditdefine.h"
#include "buistatictextctrl.h"
#include "buibuttonctrl.h"
#include "buisoftkeybarctrl.h"
#include "buidialog.h"
#include "buitheme.h"
  
enum ColorThemeT
{
  THEME_DEFAULT,
  THEME_GREEN,
  THEME_VIOLET,
  THEME_ORANGE,
  THEME_BLACK,
  THEME_COUNT
};


extern uint8 ThemeCount;
extern UiColorThemeT ColorTheme[THEME_COUNT];
extern DialogLayoutT DialogLayout;




#endif

