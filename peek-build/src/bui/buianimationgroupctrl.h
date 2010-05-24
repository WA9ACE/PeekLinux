

#ifndef ANIMATIONGROUPCTRL_H
#define ANIMATIONGROUPCTRL_H

#include "buimsgtimer.h"
#include "buidcc.h"
#include "buivector.h"
#include "buiimagectrl.h"
#include "buianimationctrl.h"



class AnimationGroupCtrlC : public ControlC
{

protected:
  virtual bool LoadResource(ResourceIdT ResourceId);
  virtual void Draw(DCC *DcP);
  
  bool OnNotify(uint32 ParamA, int32 ParamB);
  
  void OnShowNotify(void);
  void OnHideNotify(void);
  void OnNextFrame();
  
  bool OnEraseBkgnd();
  void OnMove(int16 X, int16 Y);

public:
  AnimationGroupCtrlC();
  virtual ~AnimationGroupCtrlC();

  virtual bool CanHaveFocus(){return FALSE;}

  
  void Play();
  void Stop(bool UpdateDisplay = TRUE);

  void SetStopNotify(uint32 MsgId);
  
  uint16 GetNumOfAnimations() const;
  bool Play(uint16 Index);
  void StopCurrentAnimation();
  AnimationCtrlC* GetAnimationByIndex(uint16 Index);

/*! \cond private */
private:
  AnimationGroupCtrlC(const AnimationGroupCtrlC&);
  AnimationGroupCtrlC& operator =(const AnimationGroupCtrlC&);
/*! \endcond */   

private:
  BVectorC mAnimationArray;
  uint16 mCurrentPlayingIndex;
  bool mPlaying;
  bool NeedPlayWhenShow;
 
  DECLARE_MAIL_MAP()
};





#endif

