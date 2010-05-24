
#include "builistdialog.h"

#ifndef UIHELPTEXT_H
#define UIHELPTEXT_H 
/******************************************************************************
* 
* FILE NAME   : buihelptext.h
*
* DESCRIPTION :
*
*   This is the interface for the DialogC class.
*
* HISTORY     :
*
*   See Log at end of file
*
* Copyright (c) 2007, BTC Technologies, Inc.
******************************************************************************/



//! Dialog class define
class HelpTextC
{
public:
  HelpTextC();
  virtual ~HelpTextC();


protected:
  bool OnKeyPress(uint32 KeyCode);

  int32 OnCreate(void);
  
private:  
  bool bHelpTextShow;
 
  DECLARE_MAIL_MAP()
};


/*****************************************************************************
* $Log: buihelptext.h $
* Revision 1.4  2007/11/05 13:21:57  Zou Wenqiang
* Initial revision
*****************************************************************************/


#endif

