

#ifndef GRIDMENU_H_
#define GRIDMENU_H_

#include "buimenuctrl.h"
#include "buitrace.h"
#include "buianimationgroupctrl.h"
#include "buiwindow.h"
#include "buidialog.h"

extern void SetMainMenuGridStyle(DialogC *DlgP);
extern void SetMainMenuListStyle(DialogC *DlgP);

class AnimIconCellDataC : public CellDataC
{  

/*! \cond private */ 
protected:
  BVectorC mIconVector;
  BalDispRectT mAnimRect;
  RESOURCE_ID(AnimationCtrlResT) mAnimResId;
/*! \endcond */ 

public:
  AnimIconCellDataC(WinHandleT WinHandle);
  virtual ~AnimIconCellDataC(){};

  void PushIcon(RESOURCE_ID(ImageResT) IconId);
  void SetAnimation(RESOURCE_ID(AnimationCtrlResT) AnimResId, uint16 ItemIndex); 

/*! \cond private */   
protected:   
  virtual void Draw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties);
public:
  void virtual PrepareForDraw(DCC *DC, CellStatusT &CellStatus, CellPropertiesT &CellProperties);
  virtual void DrawForMenu(DCC *DC, ItemStatusT &Status, MenuPropertiesT &MenuProperties);
/*! \endcond */ 
};



class GridMenuDrawerC: public InnerMenuDrawerC
{
public:
  GridMenuDrawerC(){ mExtraDataP = NULL; };
  virtual ~GridMenuDrawerC(){};
  virtual ItemDrawerC* GetItemDrawer(uint16 ItemIndex);  

/*! \cond private */   
private:  
  void* mExtraDataP;
/*! \endcond */ 
};


class ListMenuDrawerC: public InnerMenuDrawerC
{
public:
  ListMenuDrawerC(){};
  virtual ~ListMenuDrawerC(){};
  virtual ItemDrawerC* GetItemDrawer(uint16 ItemIndex);  
};




#endif


