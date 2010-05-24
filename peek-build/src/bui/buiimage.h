

#ifndef UIIMAGE_H
#define UIIMAGE_H



#include "buiimagesupport.h"

//#include "restypedefines.h"

class ImageDataC;
//! image class
class ImageC
{
public:

public:
  ImageC();
  virtual ~ImageC();

  void SetDecodeOption(const ImageDecodeOptionT &DecodeOption);
  void SetDecodeOption(uint8 BitCount, ImageFitTypeT FitType= IMAGE_FIT_NONE, uint16 OutputWidth = 0, uint16 OutputHeight = 0);
  bool GetBitmap(ImageDecodeOutputT &DecodeOutput);
  bool GetBitmap(BalDispBitmapT &BitmapOutput);
  
  //bool SetImage(RESOURCE_ID(ImageResT)  ResourceId);
  bool SetImage(ImageTypeT ImageType, const char *BufferP, int32 BufLen);
  bool SetImage(ImageTypeT ImageType, const char *FileNameP);
  bool SetImage(BalDispBitmapT &bmp, bool IsROM);
  
private:
  ImageDataC *mDataP;

/*! \cond private */  
private:
  ImageC(const ImageC&);
  ImageC& operator =(const ImageC&);
/*! \endcond */ 
  
};






#endif
