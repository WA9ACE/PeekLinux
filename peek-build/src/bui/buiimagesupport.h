

#ifndef IMAGESUPPORT_H
#define IMAGESUPPORT_H

#ifndef SUPPORT_PNG
#define SUPPORT_PNG
#endif 

#include "baldispapi.h"

//#include "restypedefines.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//! image type
enum
{
  IMAGE_TYPE_UNKNOWN = 0,
  IMAGE_TYPE_RAW_BITMAP,
  IMAGE_TYPE_BITMAP,
#ifdef SUPPORT_PNG
  IMAGE_TYPE_PNG,
#endif
  IMAGE_TYPE_GIF,
  /* 
  IMAGE_TYPE_WBMP,
  IMAGE_TYPE_RAW_COMPRESSED_BITMAP,
  IMAGE_TYPE_ICON,
  */
  IMAGE_TYPE_JPG,
  IMAGE_TYPE_USER_DEFINED_BEGIN
};  

typedef uint16 ImageTypeT;

//! image decode fit type
typedef enum
{
  IMAGE_FIT_NONE,         /*output full image*/
  IMAGE_FIT_LCD_ZOOM,     /*if image is larger than LCD size, zoom image to fit LCD*/
  IMAGE_FIT_LCD_CUT       /*if image is larger than LCD size, cut image to fit LCD*/
} ImageFitTypeT;


//! image decode option
typedef struct
{
  ImageFitTypeT FitType;
  uint8 BitCount;
  uint16 OutputWidth;
  uint16 OutputHeight;
} ImageDecodeOptionT;

//! image decode outpu type
typedef struct
{
  int8 IsROM;
  int8 IsTransparent;
  BalDispColorT TransparentColor;
  BalDispBitmapT bmp;
} ImageDecodeOutputT;

typedef int32 ImageReturnTypeT;
#define IMAGE_RETURN_SUCCESS 0
#define IMAGE_RETURN_DEFAULT_ERROR 1
#define IMAGE_RETURN_OUT_OF_MEMORY 2

//! decode image stream function type
typedef ImageReturnTypeT (*ImageDecodeStreamFuncT)(const uint8 *bufferP, uint32   buflen, 
                          ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);
//! decode image file function type                          
typedef ImageReturnTypeT (*ImageDecodeFileFuncT)(const char *FileNameP, 
                          ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);


//! image type and suffix table type
#define IMAGE_MAX_SUFFIX_LENGH 4
typedef struct
{
  ImageTypeT ImageType;
  char Suffix[IMAGE_MAX_SUFFIX_LENGH+1];
}ImageSuffixTableT;

//! image decode function table type
typedef struct
{
  ImageTypeT ImageType;
  ImageDecodeStreamFuncT DecodeStreamFuncP;
  ImageDecodeFileFuncT   DecodeFileFuncP;
}ImageDecodeFuncTableT;


ImageReturnTypeT ImageRegisterSuffix(const ImageSuffixTableT *ImageSuffixTableP);
ImageReturnTypeT ImageRegisterDecodeFunc(const ImageDecodeFuncTableT *ImageDecodeFuncTableP);
  
//ImageReturnTypeT ImageDecodeResourceID(RESOURCE_ID(BYDImageResT) ResourceId, 
  //                                     ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);
//ImageReturnTypeT ImageDecodeBuffer(ImageTypeT ImageType, const uint8 *BufferP, uint32 BufLen, 
//                                   ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);
//ImageReturnTypeT ImageDecodeFile(ImageTypeT ImageType, const char *FileNameP, 
  //                                 ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);

//ImageTypeT ImageGetTypeByResourceID(RESOURCE_ID(BYDImageResT)  ResourceId);
ImageTypeT ImageGetTypeBySuffix(const char *SuffixP);
ImageTypeT ImageGetTypeByFilename(const char *FilenameP);

typedef struct
{ 
  unsigned long biSize; 
  long biWidth; 
  long biHeight; 
  unsigned short biPlanes; 
  unsigned short biBitCount ;
  unsigned long biCompression; 
  unsigned long biSizeImage; 
  long biXPelsPerMeter; 
  long biYPelsPerMeter; 
  unsigned long biClrUsed; 
  unsigned long biClrImportant; 
} BMPINFOHEADER;

typedef struct
{ 
  unsigned char rgbBlue;
  unsigned char rgbGreen;
  unsigned char rgbRed;
  unsigned char rgbReserved;
} RGBQ;

typedef struct 
{ 
  BMPINFOHEADER bmiHeader; 
  RGBQ bmiColors[1]; 
} BMPINFO;

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

#ifdef __cplusplus
}
#endif /* __cplusplus */





#endif
