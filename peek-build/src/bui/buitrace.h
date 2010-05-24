

#ifndef UITRACE_H_
#define UITRACE_H_

#include "sysdefs.h"

#ifdef BYD_USE_SIM
  #include <assert.h>
  #include <stdio.h>
  #ifdef UI_DEBUG 
    #define UIASSERT(IsTrue) assert(IsTrue)
  #else
    #define UIASSERT(IsTrue)
  #endif 
#else          
  #ifdef UI_DEBUG
    void __UiAssert(uint32 IsTrue, uint32 Line, uint32 File);    
    #define UIASSERT(IsTrue)  __UiAssert((uint32)(IsTrue), __LINE__, FOUR_CHAR_FILENAME);
  #else
    #define UIASSERT(IsTrue)        
  #endif
  
#endif

#include <string.h>

enum
{
  UI_TRACE_ID_START = 0,  
  UI_TRACE_ID_DISPATCH_MAIL,
  UI_TRACE_ID_UPDATE_WINDOW,
  UI_TRACE_ID_DRAW_WINDOW,
  UI_TRACE_ID_UPDATE_LCD,
  UI_TRACE_ID_APP_ON_DIALOG_EVENT,
  UI_TRACE_ID_APP_HANDLE_WIN_NOTIFY,
  UI_TRACE_ID_APP_HANDLE_KEY,
  UI_TRACE_ID_END = 0xFF
};

void UiTrace(uint16 UiTraceId, uint32 NumArgs = 0, uint32 Param1 = 0, uint32 Param2 = 0,
             uint32 Param3 = 0, uint32 Param4 = 0);

#define FOUR_CHAR_FILENAME  (*(uint32*)((uint32)(strrchr(__FILE__,'.')-4)&0xfffffffc))

#ifdef UI_DEBUG
  void UiWriteMailHandleLog(uint32* LogP);
  void UiPrintMailHandleLog(void);
#endif



#endif

