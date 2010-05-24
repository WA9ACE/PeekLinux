#ifndef UIJPEGDECODE_H
#define UIJPEGDECODE_H


#include "buiimagesupport.h"
#include "buijpegdec.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*decode functions for png file*/
ImageReturnTypeT JpegDecodeStream(const uint8 *BufferP, uint32   BufLen, ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);
ImageReturnTypeT JpegDecodeFile(const char *FileNameP, ImageDecodeOptionT *ImageDecodeOptionP, ImageDecodeOutputT *DecodeOutputP);
JpegErrorType JpegDecodeFromBuf(const uint8 *BufferP, uint32 BufLen, ImageDecodeOutputT *DecodeOutputP);
JpegErrorType JpegDecodeFromFile(char *FileName, ImageDecodeOutputT *DecodeOutputP);
#ifdef __cplusplus
}
#endif /* __cplusplus */





#endif
