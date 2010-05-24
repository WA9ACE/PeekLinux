#ifndef BUISYMBOLS123_H
#define BUISYMBOLS123_H 
/******************************************************************************
* 
* FILE NAME   : buisymbols123.h
*
* DESCRIPTION :
*
*   This is the interface for the symbols input class.
*
* HISTORY     :
*
*   See Log at end of file
*
* Copyright (c) 2006, VIA Technologies, Inc.
******************************************************************************/

#include "buiDialog.h"
#include "buieditctrl.h"

enum
{
  SYMBOLS_TYPE_EN = 0,
  SYMBOLS_TYPE_ADDRESS,
  SYMBOLS_TYPE_MAX
};

typedef enum
{
	SYM_SINGLE = 0,
	SYM_COMB,
	SYM_MAX
}SYM_LAB;

typedef struct
{
  const char   sym_s;
  const char* sym_c;
  SYM_LAB 	lab;
}SYM_MAP;

class Symbols123C : public ControlC
{
public:
  Symbols123C(EditCtrlC &Edit, 
                 uint8 SymbolType = SYMBOLS_TYPE_EN);
  ~Symbols123C();

protected:
  bool OnKeyPress(uint32 KeyCode);
  bool OnKeyRelease(uint32 KeyCode);
  //add by lizhongyi 20080731
  void OnSetFocus(WindowC* OldWinP);  
  void OnKillFocus(WindowC* NewWinP);
  virtual bool LoadResource(ResourceIdT ResourceId);
  
  void DrawBackground(DCC* DcP);
private:
  virtual void Draw(DCC *DcP);
  int GetMaxSymBound(void);
  void SetSymParameter(void);
  void PaintSymbols(DCC *DcP);

private:
  EditCtrlC &mEdit;
  
  uint8 mLine;
  uint8 mCol;
  uint8 mMaxCol;
  uint8 mMaxLine;
  uint8 mNumCell;
  uint8 mCurrentType;
  BalDispRectT mCellRect;
  BalDispRectT mSymbolsRect;
  int mFontBound;
  BalDispBitmapT m_symbmp;
  ImageC mImage; //Symbols123 dialog backimage pointer add by maoshanshan 2008.7.1
  // In order to skip the first key release event
  bool FirstRelease;
  SYM_MAP *SymbolsTable;

  DECLARE_MAIL_MAP()
};

/*****************************************************************************
* End of File
*****************************************************************************/
#endif


