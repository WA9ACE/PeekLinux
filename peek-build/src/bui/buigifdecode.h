


#ifndef UIGIFDECODE_H
#define UIGIFDECODE_H



#include "buiimagesupport.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*decode functions for bitmap file*/
ImageReturnTypeT GifDecodeStream(const uint8 *BufferP, uint32   BufLen, ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);

ImageReturnTypeT GifDecodeFile(const char *FileNameP, ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);
ImageReturnTypeT GifDecodeAnimate(const char *FileNameP,ImageDecodeOptionT *ImageDecodeOptionP,  ImageDecodeOutputT *DecodeOutputP, void **Handle, uint8 &Num);
ImageReturnTypeT GifDecodeAnimateStream(const uint8 *BufferP, uint32 buflen,ImageDecodeOptionT *ImageDecodeOptionP,  ImageDecodeOutputT *DecodeOutputP, void **Handle, uint8 &Num);
ImageReturnTypeT GifDecodeFrame(void *Handle, uint8 Index);
ImageReturnTypeT GifDecodeFree(void** HandlePP);


#ifdef __cplusplus
}
#endif /* __cplusplus */





#endif /*UIBITMAPDECODE_H*/
