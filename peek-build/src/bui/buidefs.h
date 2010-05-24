

#ifndef UIDEFS_H
#define UIDEFS_H 
/******************************************************************************
* 
*
******************************************************************************/
#include <string.h>

#include "sysdefs.h"

#ifdef BYD_USE_SIM
#include <windows.h>
#endif

//#include "buinew.h"

#define BYD_SUPPORT_TITLE

//! Window handle type define
typedef uint16 WinHandleT;

#define  WIN_HANDLE_MIN          0x0001
#define  WIN_HANDLE_MAX          0x0800
#define  WIN_HANDLE_NULL         0x0000
#define  WIN_HANDLE_ALL          0xFFFF
#define  WIN_HANDLE_MAIN_LCD     0xFFF0
#define  WIN_HANDLE_SUB_LCD      0xFFF1

//! Parameter type define
typedef enum
{
  BY_VALUE = 0, //!< the parameter is a value
  BY_ADDRESS = 1 //!< the parameter is a address
} ParamTypeT;

//! Mail message struct define
typedef struct
{
  uint32 MailMsgId;
  uint32 ParamA;
  int32 ParamB;
  WinHandleT WinHandle;  
  ParamTypeT ParamType;
} MailMsgT;

#define UI_KEY_SIGNAL EXE_SIGNAL_1
#define UI_END_SIGNAL EXE_SIGNAL_20



#define IsValidPtr(Ptr)         (NULL != (Ptr))
#define IsNull(Ptr)             (NULL == (Ptr))
#define IsEqual(Left, Right)    ((Left) == (Right))
#define IsTrue(Value)           ((Value) == (TRUE))

#define UIDELETE(Ptr)           if (NULL != (Ptr))    \
                                {                     \
                                  delete (Ptr);       \
                                  Ptr = NULL;         \
                                }                     

#define UISTRCMP(Left, R, Right)      (strcmp(Left, Right) R 0)
#define UISTRNCMP(Left, R, Right, N)  (strcmp(Left, Right, N) R 0)

/* Macro for determining the number of items in an array */
#ifndef ARR_SIZE
#define ARR_SIZE(x)   (sizeof(x) / sizeof(x[0]) )
#endif

/* Macros for Min/Max of 2 items */
#ifndef MIN
#define MIN(x,y)      (((x) < (y)) ? (x): (y))
#endif

#ifndef MAX
#define MAX(x,y)      (((x) > (y)) ? (x): (y))
#endif

#ifndef ABS 
#define ABS(x)        (((x) < (0)) ? (-x):(x))
#endif




#endif

