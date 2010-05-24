#include "LcdControl.h"
#include "Debug.h"

#include "sysdefs.h"
#include "bal_def.h"
//#include "balkeypad.h"
//#include "lgui.h"

#include "ballightapi.h"
//#include "balmiscapi.h"
//#include "buikeymap.h"
//#include "buimsgmanager.h"
//#include "bal_nm.h"
#include "balbattapi.h"
#include "bal_settingmodelhelper.h"
#include "exeapi.h"

#include "ballightapi.h"
#define ONE_SECOND_IN_MS    1000
#define TIME_LCDBACKLIGHT_OFF   60*1000   //zhangfanghui add.
#define TIME_LCDBACKLIGHT_DIMTOOFF 45*1000
#define TIME_LCDBACKLIGHT_DIM   15*1000

#define TRY_LCD_OPERATE_TIMES  5//added by li.panxun@byd.com

uint16 configLightOpId = 0;

uint32 LcdWakeUp(void) {
	int i;
	BalLightLevelT preLevel;
	BalLightLevelT curLevel;
	BKProfileListT powermanagermode;

        BalSetPartialParameters(LCD_MAIN, 0, 0, 0, 0, FALSE);

        BalGetLightStatus(BAL_LIGHT_LCD, &preLevel, NULL);
        powermanagermode = GET_BK_PROFILE();

  //      if(HighLight)
    //    {
             curLevel = BAL_LIGHT_LEVEL_3;
      //  }
        //else if(powermanagermode == eBatteryHog)
        //{
          //   curLevel = BAL_LIGHT_LEVEL_3;
        //}
       // else if(powermanagermode == eStandard)
        //{
        //     curLevel = BAL_LIGHT_LEVEL_3;
       // }
       // else//eBatterySaver
      //  {
       //      curLevel = BAL_LIGHT_LEVEL_1;
        //}

        if (BalGetKeypadLock())
             curLevel = BAL_LIGHT_LEVEL_1;//LCD DIM

        if(curLevel != preLevel)
        {
             BalLightBrightness(BAL_LIGHT_LCD, curLevel);
               //step 1---we send message to LCD task
               for (i=0;i<TRY_LCD_OPERATE_TIMES;i++)
               {
                     if(BalLcdScreenOn())
                     {
                        //EMO_DEBUG(EMO_DEBUG_UI, EMO_DLVL_WARN, "enable LCD success in %d time(s)\n", i+1);
                               break;
                     }

                     NU_Sleep(30);//enable LCD failed , try the next time!
               }
               openlcdflag = 2;
#ifdef EMO_SIM
	       return;
#endif
               NU_Sleep(50);

                   //step 2---check LCD task handle it or not?
                   //if not,we force call lcd_init_cmdsequence function,
                   //our driver TEAM may think it's a bad solution,we don't care it!
                   if (lcd_g_state != 1/*DISPLAY_OFF*/)
               {
                 lcd_init_cmdsequence();
                 lcd_g_state = 1;
                 //EMO_DEBUG(EMO_DEBUG_UI, EMO_DLVL_WARN, "enable LCD success by calling function\n");
               }
                NU_Sleep(50);
            //configLightOpId = BalLightOn(BAL_LIGHT_LCD, 0, 0, 0);
            for (i=0;i<TRY_LCD_OPERATE_TIMES;i++)
	    {
              configLightOpId = BalLightOn(BAL_LIGHT_LCD, 0, 0, 0);
              if(configLightOpId == 0xFFFF)
              {
              		//EMO_DEBUG(EMO_DEBUG_UI, EMO_DLVL_WARN, "light on LCD failed in %d time(s),we only try %d times!\n", i+1, TRY_LCD_OPERATE_TIMES);
                        NU_Sleep(30);//light on LCD failed , try the next time!
                        continue;
              } else
                        break;
            }

            return TRUE;
        }
	return TRUE;
}
