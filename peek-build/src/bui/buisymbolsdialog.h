

#ifndef UISYSMBOLSDIALOG_H
#define UISYSMBOLSDIALOG_H 


#include "buidialog.h"
#include "buieditctrl.h"

class SymbolsDialogC : public DialogC
{
public:
  SymbolsDialogC(EditCtrlC &Edit);

protected:
  bool OnKeyPress(uint32 KeyCode);
  bool OnKeyRelease(uint32 KeyCode);

private:
  virtual void Draw(DCC *DcP);

  void PaintSymbols(DCC *DcP);

private:
  EditCtrlC &mEdit;
  uint8 mLine;
  uint8 mCol;

  // In order to skip the first key release event
  bool FirstRelease;

  DECLARE_MAIL_MAP()
};



#endif

