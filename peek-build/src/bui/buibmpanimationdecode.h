


#ifndef UIBMPANIMATIONDECODE_H
#define UIBMPANIMATIONDECODE_H



#include "buianimationsupport.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


AnimationHandleT BmpAnimationCreateByResourceID(ResourceIdT ResourceId);
AnimationHandleT BmpAnimationCreateByBuffer(const uint8 *BufP, int Buflen);
AnimationHandleT BmpAnimationCreateByFile(const char *FilenameP);
AnimationReturnTypeT BmpAnimationGetFirstFrame( AnimationHandleT AnimationHandle, AnimationFrameT **AnimationFramePP);
AnimationReturnTypeT BmpAnimationGetNextFrame( AnimationHandleT AnimationHandle, AnimationFrameT **AnimationFramePP);
AnimationReturnTypeT BmpAnimationGetStopFrame( AnimationHandleT AnimationHandle, AnimationFrameT **AnimationFramePP);
AnimationReturnTypeT BmpAnimationFreeHandle(AnimationHandleT AnimationHandle);


#ifdef __cplusplus
}
#endif /* __cplusplus */





#endif /*UIBMPANIMATIONDECODE_H*/
