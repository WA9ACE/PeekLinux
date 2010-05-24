

#ifndef UIADDRESSEDITCONTROL_H
#define UIADDRESSEDITCONTROL_H 


#include "buieditctrl.h"
#include "buicontentviewer.h"
#include "buicontentcontainer.h"
#include "buidcc.h"
#include "buicaret.h"

typedef enum
{
  ADDRESSEDIT_BEFORE = 0,
  ADDRESSEDIT_AFTER,
  ADDRESSEDIT_CURR
  
} AddressEditDirectT;

typedef enum
{
  STARKEY_NONE = 0,
  STARKEY_SINGLEADD,
  STARKEY_REPLACE,
  STARKEY_BEFORE_NAMEADDRESS,
  
  STARKEY_MAX
  
} StarKeyAddT;

typedef struct
{
  bool ReverseDisplay;
  bool LastSemicolonClear;
  StarKeyAddT StarKeyAddType;
  uint16 StartReverseLine;
  uint16 EndReverseLine;
} AddressEditInfoT;

class AddressEditCtrlC : public EditCtrlC
{
public:
  AddressEditCtrlC();
  virtual ~AddressEditCtrlC();
  
  void PostProcessAdd();
  bool IsLimitIme();
  AddressEditInfoT* GetAddressEditInfo();
  
  virtual bool ProcessAdd(char Ch);
  virtual bool ProcessAdd(const BStringC &String);
  
private:
  void PreProcessAdd();
  
  bool IsAddStartKey(char Ch);
  void TrimLeftandRight(BStringC &String);
  bool IsNameAddress(uint16 CaretOffset, AddressEditDirectT Direct);
  
private:
  //virtual void Draw(DCC *DcP);
  //virtual void DrawContent(DCC *DcP);
  //virtual void DrawTextArea(DCC *DcP);
  //virtual void ClearToStartPosition();
  //virtual bool GetCaretRect(RectC &Rect);
  virtual bool Clear(uint16 Count);
  
  virtual bool PostProcessClear();
  virtual bool ToMoveOut(bool Down);
  virtual void ProcessUpDownKey(bool Down);
  virtual bool IsCaretAtStartPosition();
  virtual void MoveCaretH(int16 Chars);
  virtual void MoveCaretV(int16 Lines);

  virtual void MarkBegin();
  
  virtual bool PreProcessReverseDisplay();
  virtual bool ProcessReverseDisplay(DCC *DcP, uint16 Line, BStringC &String, uint8 Begin);
  virtual uint16 ResetCaretOffset();
  
private:
  AddressEditInfoT mAddressEditInfo;
  
protected:
  bool OnKeyPress(uint32 KeyCode);
  
  DECLARE_MAIL_MAP()
};

inline AddressEditInfoT* AddressEditCtrlC::GetAddressEditInfo()
{
  return &mAddressEditInfo;
}



#endif

