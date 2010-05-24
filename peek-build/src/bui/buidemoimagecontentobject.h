
#ifndef UIDEMOIMAGECONTENTOBJECT_H
#define UIDEMOIMAGECONTENTOBJECT_H 

#include "buicontentobject.h"
#include "buistring.h"
#include "buiimage.h"

class DemoImageContentObjectC : public ContentObjectC
{
public:
  DemoImageContentObjectC();
  virtual char* GetType() ;
  virtual RectC GetGlyphRect(uint16 glyphIndex);
  virtual bool DrawGlyph(DCC* DcP,RectC area,uint16 glyphIndex);
  virtual uint16 DeleteGlyph(uint16 glyphIndex,uint16 glyphCount);
  virtual BStringC *Serialize();
  virtual ContentObjectC *Deserialize(BStringC *xml);
  virtual ContentObjectC * InsertContent(ContentObjectC *object,uint16 glyphIndex,uint8 * result);
  virtual uint16 GetGlyphCount();
private:
  ImageC mImage;
};



#endif

