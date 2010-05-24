#ifndef UIAPPIDS_H
#define UIAPPIDS_H

#if 0

typedef enum
{
  APPID_NONE = 0,
  APPID_UILIFECYCLE,
//#ifdef UI_UNITTEST
  APPID_UNITTEST,
//#endif  
  APPID_BAL_TEST,
  APPID_HELLO,
  APPID_TXTBL,
  //APPID_CITTEST,
  APPID_TEST,
  
  APPID_MAX
} AppIdT;

#else

typedef enum
{
  APPID_NONE = 0,
  APPID_UILIFECYCLE,
  APPID_CALL,
  APPID_SMS,
  APPID_CONTACT,
  APPID_RECENTCALL,  
  APPID_SETTING,
  APPID_TOOLS,  
  APPID_HIDEMENU,
  APPID_MYMEDIA,  
  APPID_MEDIATOOLS,
  APPID_MP3,
  APPID_CAMERA,
  APPID_UTK,
#ifdef FEATURE_BREW
  APPID_BREW,
#endif
  APPID_WAPOPWV,
//#ifdef UI_UNITTEST
  APPID_UNITTEST,
//#endif  
  APPID_VOICEMEMO,
  APPID_ALARM,
  APPID_CALENDAR,
  APPID_FM,
  APPID_CALCULATOR,//added by jeanson on 20071113
  APPID_TIMER, //add for timer by zhupeiling 20071127
  APPID_STOPWATCH,//added by huyong in 20071217
  APPID_GALLERY,
  APPID_SMARTMESSAGE,//added by xiehao 07/12/13
  APPID_NAMPROGRAM,//add for Hidden menu by Julia.ye 20071210
  APPID_FTD, //added by leonwu 20071212
  APPID_CALLRESTRICT,//add for callrestriction by wangaiping 20071210
  APPID_PLAYER,
  APPID_BAL_TEST,
  
  APPID_HELLO,
  APPID_TXTBL,
  APPID_CITTEST,
  APPID_TEST,
  
  APPID_MAX
} AppIdT;

#endif

#endif
