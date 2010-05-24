

#ifndef UIEXCONTAINERCONTROL_H
#define UIEXCONTAINERCONTROL_H


#include "buivector.h"
#include "buiwindow.h"
#include "buicontrol.h"
#include "buistatictextctrl.h"
#include "buieditctrl.h"
#include "buidatectrl.h"
#include "buianimationctrl.h"
#include "buibuttonctrl.h"

typedef struct
{
  ControlC *pWin;
  RectC rect;
  uint32 dwFlags;
}ControlInfoT;

//! BYD menu control define
class ContainerC: public ControlC
{
public:
  ContainerC();
  virtual ~ContainerC();

  bool CreateEx(RectC& rect, WindowC* ParentWinP, uint32 WinId = 0);

  virtual bool AddControl(ControlC *pWin,RectC &rect);
  virtual bool AddControl(CtrlTypeT Type,ResourceIdT ResId,RectC &rect);
  virtual bool RemoveControl(uint32 nWinId,bool bAll = TRUE);
  virtual bool RemoveControl(CtrlTypeT ControlType,bool bAll = FALSE);
  virtual bool RemoveControl(ControlC *pWin);
  virtual ControlC* GetControlById(uint32 nWinId,uint8 Number = 0);
  virtual ControlC* GetControlByType(CtrlTypeT ControlType,uint8 Number = 0);
  virtual ControlC* GetControlByResId(ResourceIdT ResId,uint8 Number = 0);
  virtual ControlInfoT* GetControlInfo(CtrlTypeT ControlType,uint8 Number = 0);

  bool SetFlags(CtrlTypeT ControlType,uint32 dwFlags,uint8 Number = 0);

  virtual bool LoadResource(ResourceIdT ResourceId);
  
protected:
  void OnSetFocus(WindowC* WinP);
  void OnKillFocus(WindowC* NewWinP);
  bool OnKeyPress(uint32 KeyCode);
  bool OnKeyHold(uint32 KeyCode);
  bool OnKeyRelease(uint32 KeyCode);
  void OnMove(int16 X, int16 Y);
  virtual void Draw(DCC *DcP);

private:
  BLinkedListC m_pCtrlInfo;
  uint32 m_nMaxWinId;

  IteratorT GetNextIterator(IteratorT pHead,uint8 nType,uint32 nCompare,uint8 Number = 0);
  void LRemoveContainerInfo(ControlInfoT  *pWinInfo);

  DECLARE_MAIL_MAP()
};





#endif

