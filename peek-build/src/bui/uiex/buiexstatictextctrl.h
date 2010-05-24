

#ifndef UIEXSTATICTEXTCTRL_H
#define UIEXSTATICTEXTCTRL_H
/******************************************************************************
* 
* FILE NAME   : buimenuexctrl.h
*
* DESCRIPTION :
*
*   This is the interface Static Text Control Ex
*
* HISTORY     :
*
*   See Log at end of file
*
* Copyright (c) 2007, BYD Technologies, Inc.
******************************************************************************/

#include "buiwindow.h"
#include "buicontrol.h"
#include "buistatictextctrl.h"
#include "buiscrollbar.h"
#include "buiexcontainer.h"

#define STATICEX_SCROLLBAR_WIDTH 5
#define STATICEX_SCROLLBAR_HEIGHT_SPACE 5
//! BYD menu control define
class StaticTextExCtrlC: public StaticTextCtrlC
{
public:
  StaticTextExCtrlC();
  virtual ~StaticTextExCtrlC();
  virtual bool Create(ResourceIdT ResourceId, WindowC* ParentWinP, uint32 WinId = 0);
  
protected:
  virtual bool LoadResource(ResourceIdT ResourceId);
  virtual void Draw(DCC *DcP);

private:

  ScrollBarControlC *m_pScrollbar;
  ContainerC *m_pContainer;
  bool m_bDrawed;

  DECLARE_MAIL_MAP()
};


/*****************************************************************************
* $Log: buimenuctrl.h $
* Create Zou Wenqiang 2007-12-29
* Initial revision
*****************************************************************************/


#endif

