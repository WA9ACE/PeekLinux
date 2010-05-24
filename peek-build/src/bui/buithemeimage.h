#ifndef UITHEMEIMAGE_H
#define UITHEMEIMAGE_H

#include "baldispapi.h"

typedef enum
{
ImageTheme_I = 1,
ImageTheme_M,
ImageTheme_T
}ImageThemeT;

class UiThemeImageC
{  
public:
  virtual ~UiThemeImageC(void);
  static UiThemeImageC* GetInstance(void);
  static void DelInstance(void);
  void SetImageTheme(ImageThemeT imagetheme);
  ImageThemeT GetImageTheme();

private:
  UiThemeImageC();
  ImageThemeT mImageTheme;
  static UiThemeImageC* _instanceimage;
  bool mImageThemeChanged;
};

inline UiThemeImageC* UiGetThemeImage(void)
{
  return UiThemeImageC::GetInstance();
}
#endif
