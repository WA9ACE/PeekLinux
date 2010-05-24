

#ifndef IDLEVIEW_H_
#define IDLEVIEW_H_

#include "buistring.h"
#include "buiview.h"
#include "buipopupdialog.h"
#include "uilifecycleutils.h"
#include "appmsguilifecycleids.h"

//! Pop up id define
typedef enum
{
  IDLE_POPUP_NULL,                 //!< No pop up
  IDLE_POPUP_KEY_WARNING,          //!< Lock key warning
  IDLE_POPUP_SECOND_KEY_WARNING,   //!< Second lock key warning
  IDLE_POPUP_KEY_ACTIVE,           //!< Key active warning
  IDLE_POPUP_KEY_LOCKED,           //!< Key locked warning
  IDLE_POPUP_EMERGENCY_WARNING,    //!< Emergency warning
  IDLE_POPUP_EMPTY_NUMBER,         //!< Dialed number is empty
  IDLE_POPUP_RESET,                //!< Reset success
  IDLE_POPUP_PHONE_LOCK_ON,
  IDLE_POPUP_PHONE_LOCK_OFF,
  IDLE_POPUP_FLIGHT_QUERY,
  IDLE_POPUP_BATTERY_EMPTY,
  IDLE_POPUP_BATTERY_LOW,
  IDLE_POPUP_BATTERY_FULL,
  IDLE_POPUP_LIST_VIEW_SEL,
  IDLE_POPUP_GRID_VIEW_SEL,
  IDLE_POPUP_CHANGES_SAVED,
  IDLE_POPUP_QUERY_ORG_SAVED,
  IDLE_POPUP_QUERY_EXIT_EMERGENCY,
  IDLE_POPUP_QUERY_RESTORE_NORMAL,  // exit emergency call state to normal
  IDLE_POPUP_QUERY_SAVE_CHANGES,
  IDLE_POPUP_QUERY_HEADSET_CONNECT,
  IDLE_POPUP_QUERY_QUIT_DATACONNECT,
  IDLE_POPUP_ACC_UNSUPPORT
}IdlePopupIdT;

enum
{
  E_IDLE_KEYGUARD   = 0x01,
  E_IDLE_MISSEDCALL = 0x02,
  E_IDLE_UNREADSMS  = 0x04,
  E_IDLE_SLEEPMODE  = 0x08
};

enum
{
  ORG_MAINMENU,
  ORG_SHORTCUT,
  ORG_MAX
};

enum
{
  ORG_SAVE_NONE,
  ORG_SAVE_NO,
  ORG_SAVE_YES
};

enum
{
  E_IDLE_UPDATE_BANNER,
  E_IDLE_UPDATE_PROFILE,
  E_IDLE_UPDATE_DATE,
};

/*! 
\brief Idle module view class

Description:
\n This class is responsible for display relative with idle model which including:
\n 1. Idle dialog(announciator, background including animation, time, banner...)
\n 2. Dialing dialog
\n 3. Emergency idle 
\n 4. Key lock and unlock
\n 5. Phone lock and unlock
\n 6. Main menu and shortcut dialog
*/
class IdleViewC: public ViewC
{
public:
  IdleViewC();
  virtual ~IdleViewC();
  
  bool OpenIdle();
  bool OpenMainmenu(uint8* pucItems, uint8 ucStyle, bool bUpdate = FALSE);
//  bool UpdateIdle(SidbStringT& Banner, uint8 ucType = E_IDLE_UPDATE_BANNER);
//  bool UpdateFtd(BStringC& strText, uint8 ucType);

  void DisplayAnimation();
  void DisplayWelcome(char *welcome);

  bool IsTopDialog(uint32 DialogId, DispLCDTypeT LcdType = LCD_MAIN);
};

#endif


