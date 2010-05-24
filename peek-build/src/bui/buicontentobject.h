
#ifndef UICONTENTOBJECT_H
#define UICONTENTOBJECT_H 

#include "buirect.h"
#include "buistring.h"
#include "buidcc.h"
class ContentObjectC
{
public:
  virtual ~ContentObjectC() {}
  virtual char* GetType()= 0 ;
  virtual RectC GetGlyphRect(uint16 glyphIndex) = 0;
  virtual bool DrawGlyph(DCC* DcP,RectC area,uint16 glyphIndex) = 0;
  virtual uint16 DeleteGlyph(uint16 glyphIndex,uint16 glyphCount) = 0;
  virtual BStringC* Serialize() = 0;
  virtual ContentObjectC* Deserialize(BStringC *xml) = 0;
  virtual ContentObjectC* InsertContent(ContentObjectC *object,uint16 glyphIndex,uint8 * result) = 0;
  virtual uint16 GetGlyphCount() = 0;
  virtual bool Play(){ return FALSE;}
  virtual BStringC GetText() { return BStringC();}
};



#endif

