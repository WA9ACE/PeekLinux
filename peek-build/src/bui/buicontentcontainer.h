
#ifndef UICONTENTCONTAINER_H
#define UICONTENTCONTAINER_H 

#include "buicontentstatelistener.h"
#include "buicontentobject.h"
#include "buivector.h"
class ContentContainerC
{
public:
  ContentContainerC();
  ~ContentContainerC();
  int8 AddContent(ContentObjectC* content,uint16 index,bool toNotify=TRUE);
  int8 RemoveContent(ContentObjectC* content,bool toNotify=TRUE);
  int8 RemoveContent(uint16 contentIndex,bool toNotify=TRUE);
  int8 RemoveGlyph(uint16 glyphIndex,uint16 glyphCount,bool toNotify=TRUE);
  ContentObjectC * GetContentByIndex(uint16 index);
  bool GetIndexByContent (ContentObjectC * content,uint16* index);
  uint16 GetContentCounts ();
  bool RegisterForStateNotification(ContentStateListenerC*  listener);
  bool DeregisterForStateNotification(ContentStateListenerC*  listener);
  virtual BStringC *Serialize();
  virtual void GetText(BStringC &String);
  bool GetGlyphIndexInObject(uint16 index,uint16 * contentIndex,uint16 * glyphIndex);
  bool GetGlyphIndexInContainer(uint16 contentIndex,uint16 glyphIndex,uint16 * index);
  uint16 GetGlyphCountInContainer();
private:
  BVectorC mContentObjects; 
  BVectorC mListeners;

private:
  void NotifyListener(uint16 index,bool updateView);
};



#endif

