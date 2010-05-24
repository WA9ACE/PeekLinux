
#ifndef UICONTENTSTATELISTENER_H
#define UICONTENTSTATELISTENER_H 

#include "sysdefs.h"
class ContentStateListenerC
{
public:
  virtual void ContentStateNotify(uint16 index,bool updateView) = 0;
};



#endif

