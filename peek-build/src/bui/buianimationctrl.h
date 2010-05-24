/**************************************************************************************************
* %version: 2.1.1 %  %instance: HZPT_1 %   %date_created: 2007/08/27 13:04:47 %  %created_by: yliu %  %derived_by: yliu %
**************************************************************************************************/

#ifndef ANIMATIONCTRL_H
#define ANIMATIONCTRL_H

#include "buiimagectrl.h"
#include "buimsgtimer.h"
#include "buidcc.h"
#include "buianimationsupport.h"

//! animation notify event type define
enum AnimationCtrlNotifyEventT
{
  ANIMATION_CTRL_NOTIFY_EVENT_STOP = 0x01
};

class AnimationDataC;
//! animatino control class
class AnimationCtrlC : public ControlC
{

protected:
  virtual bool LoadResource(ResourceIdT ResourceId);
  virtual void Draw(DCC *DcP);

  void OnShowNotify(void);
  void OnHideNotify(void);
  void OnNextFrame();
  
  bool OnEraseBkgnd();

public:
  AnimationCtrlC();
  virtual ~AnimationCtrlC();

  virtual bool CanHaveFocus(){return FALSE;}

  
  void Play();
  void Stop(bool UpdateDisplay = TRUE);

  void SetRepeat(uint16 RepeatCount);
  
  bool SetAnimation(RESOURCE_ID(AnimationResT)  ResourceId);
  bool SetAnimation(AnimationTypeT AnimationType, const char *FileNameP);
  bool SetAnimation(AnimationTypeT AnimationType, const char *BufferP, int Buflen);

  void SetStopNotify(uint32 MsgId);
  void SetIntervalTime(uint16 wTimeInMillisecond = 0xFFFF);

/*! \cond private */
private:
  AnimationCtrlC(const AnimationCtrlC&);
  AnimationCtrlC& operator =(const AnimationCtrlC&);
/*! \endcond */ 

private:
  AnimationDataC *mDataP;
   
  DECLARE_MAIL_MAP()
};





#endif

