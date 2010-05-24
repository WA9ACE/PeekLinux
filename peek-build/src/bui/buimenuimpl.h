

#ifndef UIMENUIMPL_H
#define UIMENUIMPL_H

#include "buimsgtimer.h"

#include "buimenumodel.h"
#include "buimenuview.h"

//! menu item cursor move type
enum CursorMoveT 
{
MENU_CURSOR_MOVE_NEXT_ROW,   
MENU_CURSOR_MOVE_PREVIOUS_ROW,     
MENU_CURSOR_MOVE_NEXT_COLUMN,     
MENU_CURSOR_MOVE_PREVIOUS_COLUMN,     
MENU_CURSOR_MOVE_NEXT_PAGE,     
MENU_CURSOR_MOVE_PREVIOUS_PAGE,     
MENU_CURSOR_MOVE_FIRST_ITEM,      
MENU_CURSOR_MOVE_LAST_ITEM     
}; 


class MenuDrawerC;

//! the definition of menu input class
class MenuInputC
{
/*! \cond private */    
protected:
  MenuViewC *mMenuViewP;
  MenuModelC *mMenuModelP;

  BTimerC mItemHoverTimer;
  BTimerC mHoldKeyTimer;
/*! \endcond */  
  
public:
  MenuInputC():
  mMenuViewP(NULL),mMenuModelP(NULL)
  {
  };
  
  virtual ~MenuInputC(){};

  void SetMenuModel(MenuModelC *MenuModelP){mMenuModelP = MenuModelP;};
  void SetMenuView(MenuViewC *MenuViewP){mMenuViewP = MenuViewP;};
  
  virtual bool OnKeyPress(uint32 KeyCode);
  
  virtual bool OnKeyRelease(uint32 KeyCode);

  virtual void CalculateCurrentItemIndex(uint16 ItemIndex);

  void ReleaseDecodeData(uint32 OldTopItemIndex,uint32 NewTopItemIndex);
/*! \cond private */      
protected:
  virtual void ScrollItem(CursorMoveT MoveType);
  void UpdateView(CursorMoveT ScrollDirection);
/*! \endcond */  

};

class GridMenuInputC: public MenuInputC
{
/*! \cond private */      
protected:
  virtual void ScrollItem(CursorMoveT MoveType);
/*! \endcond */  

public:
  virtual bool OnKeyPress(uint32 KeyCode);
  virtual bool OnKeyRelease(uint32 KeyCode);
  void ReleaseDecodeData(uint32 OldTopItemIndex,uint32 NewTopItemIndex,GridMenuDetailT GridMenuDetail);

};





#endif

