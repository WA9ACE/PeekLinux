

#ifndef UIMENUEXDRAWER_H
#define UIMENUEXDRAWER_H
/******************************************************************************
* 
* FILE NAME   : buimenuexdrawer.h
*
* DESCRIPTION :
*
*   This is the interface for the CellDataC class and ItemDrawerC class.
*
* HISTORY     :
*
*   See Log at end of file
*
* Copyright (c) 2007.9.30, BYD Technologies, Inc.
******************************************************************************/
#include "buimenudrawer.h"

//Add by BYD-zouwq 2007.09.12
class BYDMenuDrawerC: public InnerMenuDrawerC
{
public:
  BYDMenuDrawerC();
  virtual ItemDrawerC* GetItemDrawer(uint16 ItemIndex);

  void SetSubItemVisible(uint16 nSubItemIndex,bool bVisible);
  bool GetSubItemVisible(uint16 nSubItemIndex);

private:
  uint32 m_nSubItemVisible;
};
//End by BYD-zouwq 2007.09.12

/*****************************************************************************
* Initial revision
*****************************************************************************/


#endif//UIMENUEXDRAWER_H

