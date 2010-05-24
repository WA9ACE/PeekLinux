
#ifndef UITEXTCONTENTOBJECT_H
#define UITEXTCONTENTOBJECT_H 

#include "buicontentobject.h"
#include "buistring.h"
class TextContentObjectC : public ContentObjectC
{
public:
  TextContentObjectC(const BStringC string) 
  { 
    // mTextContent = string;
    mTextContent.Empty();
    mTextContent += string;
  }
virtual char* GetType() ;
virtual RectC GetGlyphRect(uint16 glyphIndex);
virtual bool DrawGlyph(DCC* DcP,RectC area,uint16 glyphIndex);
virtual uint16 DeleteGlyph(uint16 glyphIndex,uint16 glyphCount);
virtual BStringC *Serialize();
virtual ContentObjectC *Deserialize(BStringC *xml);
virtual ContentObjectC * InsertContent(ContentObjectC *object,uint16 glyphIndex,uint8 * result);
virtual uint16 GetGlyphCount();

virtual BStringC GetText() {return mTextContent;}
void SetText(const BStringC &str)
{ 
  //mTextContent = str;
  mTextContent.Empty();
  mTextContent += str;
}

private:
  BStringC mTextContent;
};



#endif

