
#include "buidialog.h"
#include "buimenuexmodel.h"
#include "buimenuexctrl.h"
#include "buistatictextdata.h"
#include "buidialogutils.h"
#include "builistdialog.h"

#ifndef UIOPTIONDIALOG_H
#define UIOPTIONDIALOG_H 
/******************************************************************************
* 
* FILE NAME   : buioptiondialog.h
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

#define ITEM_LAST -1

//! Dialog class define
typedef struct OptionMenuT
{
  RESOURCE_ID(ImageResT)  imageRes;
  RESOURCE_ID(StringT)    textRes;
  uint16                  wParam;
  bool                    bDim;
  struct OptionMenuT*     pNext;
  uint32 dwFlags;
} OptionMenuT;

class OptionDialogC : public ListDialogC
{
public:
  OptionDialogC(bool bStore = TRUE);
  virtual ~OptionDialogC();
  bool LoadMenuTree(OptionMenuT *OptionP);
  bool IsCurrItemDim(void);
  bool DeleteMenuItem(uint16 wParam);
  bool InsertMenuItem(OptionMenuT& stItem, int nIndex = ITEM_LAST);
  void SetMessageIdAsFlags(bool bFlag = TRUE){m_bFlags = bFlag;}

protected:
  void OnOptionSelected(uint32 dwParam);
  bool OnKeyPress(uint32 KeyCode);

private:
  bool         m_bStore;
  OptionMenuT* m_pOption;
  bool m_bFlags;
  
private:
  void CopyData(OptionMenuT** ppDest, OptionMenuT* pOption);
  void DeleteData(OptionMenuT* pOption);
  bool DeleteMenuItem(OptionMenuT* pOption, uint16 wParam);

  DECLARE_MAIL_MAP()
};

OptionDialogC* OpenOptionMenu(uint32 dwDlgId, BApplicationC* pApp, OptionMenuT* pstMenuInfo, BStringC& strTitle, bool bSynchronous = TRUE);

inline OptionDialogC* OpenOptionMenu(uint32 dwDlgId, BApplicationC* pApp, OptionMenuT* pstMenuInfo, RESOURCE_ID(StringT) strTitleRes, bool bSynchronous = TRUE)
{
  BStringC strTitle;
  if (strTitleRes)
    strTitle.Load(strTitleRes);
  return OpenOptionMenu(dwDlgId, pApp,pstMenuInfo, strTitle, bSynchronous);
}
/*****************************************************************************
* $Log: buidialog.h $
* Revision 1.4  2007/01/12 13:21:57  jinqi
* Initial revision
*****************************************************************************/


#endif

