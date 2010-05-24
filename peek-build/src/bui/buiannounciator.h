

#ifndef UIANNOUNCIATOR_H
#define UIANNOUNCIATOR_H 

#include "buivector.h"
#include "buidcc.h"
#include "buimsguser.h"
#include "buimsgtimer.h"
#include "buicontrol.h"
#include "buitrace.h"
#include "buiannounciatorcust.h"

#include "resdatabase.h"

//! Announciator class
class AnnounciatorC : public ControlC
{
typedef struct
{
  uint8 StateId;
  bool BlinkFlag;
  uint8 IconCount;
  RESOURCE_ID(ImageResT)* ResourceId;
  int8 CurrentIconIndex;
  uint8 ElementIndex;
}StateT;

typedef struct
{
  BalDispRectT Rect;
  BalDispHAlignT AlignMode;
  int8 CurrentStateIndex; /* -1 means it dosen't have a valid state yet */
  AnnElementIdT ElementId;
  BTimerC *AnimationTimerP;
}ElementT;

typedef struct
{
  uint8 RowHeight;
  AnnTypeT Type;
  uint16 AnimationTimerInterval;
}IndicatorT;  

typedef struct
{
  AnnounciatorC *AnnounciatorP;
  uint8 StateIndex;
  uint8 ElementIndex;
}AnimationNotificationT;

typedef enum
{
  MODIFY_NONE,
  MODIFY_DELETE,
  MODIFY_INSERT,
  MODIFY_UPDATE
}ModifyTypeT;

public:
  AnnounciatorC();
  virtual ~AnnounciatorC(void);

  void AddDialogCount(void);
  void ReleaseDialogCount(void);

  void UpdateState(uint32 ElementId,uint32 StateId);
/*! \cond private */
protected:
  virtual bool LoadResource(ResourceIdT ResourceId);
  virtual void Draw(DCC *DcP);

  void OnShowNotify(void);
  void OnHideNotify(void);
  void OnMove(int16 X, int16 Y);
  void OnAnimationNofify(void* MsgP);
/*! \endcond */

private:
  void SetColor(DCC *DcP, BalDispColorT Color);
  void SetPenAndBrush(DCC *DcP);
  void DrawIcon(DCC *DcP, ElementT *ElementP, StateT *StateP);
  bool InsertElement(ElementT *ElementP, int8 &Index);
  void CalculateRect(ElementT *ModifyElementP = NULL, ModifyTypeT ModifyType = MODIFY_NONE, uint8 Index = 0);
  int16 ExcludeCenterRect(uint8 Index, int16 x, int16 dx, bool Direction, int16 LeftX, int16 RightX);
  BalDispRectT *GetElementRect(AnnElementIdT ElementId);
  void CalculateCenter(ElementT *ModifyElementP, ModifyTypeT ModifyType, uint8 Index);

  IndicatorT mIndicator;
  BVectorC mElements;
  BVectorC mStates;
  BVectorC mSortedElements;
  int8 mDialogOpenCount;

  DECLARE_MAIL_MAP()
};





#endif
