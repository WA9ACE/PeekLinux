#include "typedefs.h"

/*
typedef struct tag_SettingModeHelperInfo
{
        SndAndAlertListT eSndAndAlert;
        BKProfileListT eBKProfile;
        DispThemeListT eDispTheme;
        FlightModeListT eFlightMode;
        ScreenControlT stScreenControl;
        DateAndTime stDateTime;
        LockControlT stLockControl; 
        TimeModeSet eTimeMode;
        GCFStateSet eGCFState;
        FontModeSet eFontMode;
        SoundAndAlertControlT stSoundAndAlert;
        TestModeSet eTestMode;
        unsigned char DayLight;
        PayModeSet ePayMode;
} SettingModeHelplerInfo;
*/

unsigned int stCurSettingMode;

unsigned int timer_state=0;

int http_error_counter(void) {
  return 0;
}

int nm_reregistration(void) {
    // fix and move to aci code
    return 0;
}
