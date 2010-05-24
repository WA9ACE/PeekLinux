


#ifndef UIBITMAPDECODE_H
#define UIBITMAPDECODE_H



#include "buiimagesupport.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*function for raw bitmap*/
ImageReturnTypeT RawBitmapDecodeStream(const uint8 *BufferP, uint32   BufLen, 
                          ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);

ImageReturnTypeT RawBitmapDecodeFile(const char *FileNameP,
 	                       ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);

/*decode functions for bitmap file*/
ImageReturnTypeT BitmapDecodeStream(const uint8 *BufferP, uint32   BufLen, 
                          ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);

ImageReturnTypeT BitmapDecodeFile(const char *FileNameP, 
                          ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);



#ifdef __cplusplus
}
#endif /* __cplusplus */





#endif /*UIBITMAPDECODE_H*/
