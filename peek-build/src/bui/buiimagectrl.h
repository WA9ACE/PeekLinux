

#ifndef IMAGECTRL_H
#define IMAGECTRL_H

#include "buicontrol.h"
#include "buiimagesupport.h"
#include "buiimage.h"

#include "restypedefines.h"

class ImageCtrlDataC;
//! image control class
class ImageCtrlC : public ControlC
{
public:

protected:
  virtual bool LoadResource(ResourceIdT ResourceId);

  virtual void Draw(DCC *DcP);
  void OnHideNotify(void);
  bool OnKeyPress(uint32 KeyCode);

public:
  ImageCtrlC();
  virtual ~ImageCtrlC();

  virtual bool CanHaveFocus(){return FALSE;}    //virtual function of WindowC 

 
  void SetRasterOp(BalDispRasterOpT rasterOp);
  void SetDrawTransparentColor(BalDispColorT TransparentColor);
  void SetFitType(ImageFitTypeT FitType);
  bool GetBitmap(ImageDecodeOutputT &DecodeOutput);
  bool GetBitmap(BalDispBitmapT &BitmapOutput);
  
  bool SetImage(RESOURCE_ID(ImageResT)  ResourceId);
  bool SetImage(ImageTypeT ImageType, const char *BufferP, int32 BufLen);
  bool SetImage(ImageTypeT ImageType, const char *FileNameP);
  bool SetImage(BalDispBitmapT &Bmp, bool IsROM);

/*! \cond private */
private:
  ImageCtrlC(const ImageCtrlC&);
  ImageCtrlC& operator =(const ImageCtrlC&);
/*! \endcond */

private:
  ImageCtrlDataC *mDataP;
   
  DECLARE_MAIL_MAP()
  
};






#endif
