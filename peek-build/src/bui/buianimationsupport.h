

#ifndef ANIMATIONSUPPORT_H
#define ANIMATIONSUPPORT_H


#include "baldispapi.h"

#include "buiimagesupport.h"

#include "restypedefines.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//! newver stop
#define ANIMATION_NEVER_STOP 0xFFFF

//! animationt ype
enum 
{
  ANIMATION_TYPE_UNKNOWN = 0,
  ANIMATION_TYPE_BITMAP,
  ANIMATION_TYPE_GIF,
  ANIMATION_TYPE_USER_DEFINED_BEGIN
};
typedef uint16 AnimationTypeT;

//! disposal method type
enum 
{
  ANIMATION_DISPOSAL_METHOD_NONE = 1,
  ANIMATION_DISPOSAL_METHOD_TO_BACKGROUND,
  ANIMATION_DISPOSAL_METHOD_TO_PRE_BITMAP
};

//! animation handle type
typedef uint32 AnimationHandleT;
#define ANIMATION_INVALID_HANDLE 0

//! animation function return type
typedef int32 AnimationReturnTypeT;
#define ANIMATION_RETURN_SUCCESS 0
#define ANIMATION_RETURN_EOF     (-1)
#define ANIMATION_RETURN_DEFAULT_ERROR 1
#define ANIMATION_RETURN_OUT_OF_MEMORY 2

//! animation frame type
typedef struct{
  BalDispRectT rect;
  uint16          Duration;           /*In milliseconds */
  uint8           DisposalMethod;
  ImageDecodeOutputT Image;
}AnimationFrameT;


//! animation decode function types
typedef AnimationHandleT (*AnimationCreateByResourceIDFuncT)(ResourceIdT ResourceId);
typedef AnimationHandleT (*AnimationCreateByBufferFuncT)(const uint8 *BufP, int Buflen);
typedef AnimationHandleT (*AnimationCreateByFileFuncT)(const char *FilenameP);
typedef AnimationReturnTypeT (*AnimationGetFirstFrameFuncT)( AnimationHandleT AnimationHandle, AnimationFrameT **AnimationFramePP);
typedef AnimationReturnTypeT (*AnimationGetNextFrameFuncT)( AnimationHandleT AnimationHandle, AnimationFrameT **AnimationFramePP);
typedef AnimationReturnTypeT (*AnimationGetStopFrameFuncT)( AnimationHandleT AnimationHandle, AnimationFrameT **AnimationFramePP);
typedef AnimationReturnTypeT (*AnimationFreeHandleFuncT)(AnimationHandleT AnimationHandle);

// animation decode information type
typedef struct{
  AnimationTypeT AnimationType;
  AnimationCreateByResourceIDFuncT  CreateByResourceFuncP;
  AnimationCreateByBufferFuncT      CreateByBufferFuncP;
  AnimationCreateByFileFuncT        CreateByFileFuncT;
  AnimationGetFirstFrameFuncT       GetFirstFrameFuncP;
  AnimationGetNextFrameFuncT        GetNextFrameFuncP;
  AnimationGetStopFrameFuncT        GetStopFrameFuncP;
  AnimationFreeHandleFuncT          FreeHandleFuncP;
}AnimationDecodeInfoT;



AnimationReturnTypeT AnimationRegister(const AnimationDecodeInfoT *AnimationDecodeInfoP);

AnimationHandleT AnimationCreateByResourceID(RESOURCE_ID(AnimationResT) ResourceId);
AnimationHandleT AnimationCreateByBuffer(AnimationTypeT AnimationType, const uint8 *BufP, int BufLen);
AnimationHandleT AnimationCreateByFile(AnimationTypeT AnimationType, const char *FilenameP);
AnimationReturnTypeT AnimationGetFirstFrame( AnimationHandleT AnimationHandle, AnimationFrameT **AnimationFramePP);
AnimationReturnTypeT AnimationGetNextFrame( AnimationHandleT AnimationHandle, AnimationFrameT **AnimationFramePP);
AnimationReturnTypeT AnimationGetStopFrame( AnimationHandleT AnimationHandle, AnimationFrameT **AnimationFramePP);
AnimationReturnTypeT AnimationFreeHandle(AnimationHandleT AnimationHandle);
void  AnimationInitFrame (AnimationFrameT *AnimationFrameP);

 

#ifdef __cplusplus
}
#endif /* __cplusplus */




#endif

