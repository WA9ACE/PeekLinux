


#ifndef UIGIFANIMATIONDECODE_H
#define UIGIFANIMATIONDECODE_H



#include "buianimationsupport.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


AnimationHandleT GifAnimationCreateByResourceID(ResourceIdT ResourceId);
//AnimationHandleT BmpAnimationCreateByBuffer(const uint8 *BufP, int Buflen);
AnimationHandleT GifAnimationCreateByFile(const char *FilenameP);
AnimationReturnTypeT GifAnimationGetFirstFrame( AnimationHandleT AnimationHandle, AnimationFrameT **AnimationFrmaePP);
AnimationReturnTypeT GifAnimationGetNextFrame( AnimationHandleT AnimationHandle, AnimationFrameT **AnimationFrmaePP);
AnimationReturnTypeT GifAnimationGetStopFrame( AnimationHandleT AnimationHandle, AnimationFrameT **AnimationFrmaePP);
AnimationReturnTypeT GifAnimationFreeHandle(AnimationHandleT AnimationHandle);


#ifdef __cplusplus
}
#endif /* __cplusplus */





#endif /*UIBMPANIMATIONDECODE_H*/
