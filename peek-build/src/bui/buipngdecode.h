#ifndef UIPNGDECODE_H
#define UIPNGDECODE_H


#include "buiimagesupport.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*decode functions for png file*/
ImageReturnTypeT PngDecodeStream(const uint8 *BufferP, uint32   BufLen, ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);
ImageReturnTypeT PngDecodeFile(const char *FileNameP, ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);

#ifdef __cplusplus
}
#endif /* __cplusplus */





#endif
