/* Backlight related functions */
#include "typedefs.h"
#include "header.h"
#include "light.h"

//Light level settings 
#define BL_NO_LIGHT 0
#define BL_MAX_LIGHT 255
#define BL_SET_IDLE -1
#define BL_NO_CHANGE -2

//Events that may affect the backlight 
enum {
        BL_INIT,
        BL_IDLE_TIMER,
        BL_KEY_PRESS,
        BL_INCOMING_SMS,
        BL_INCOMING_CALL,
        BL_EXIT,
        BL_SPARE_OPTION_1,
        BL_SPARE_OPTION_2,
        BL_SPARE_OPTION_3,
        BL_LAST_OPTION
};

static int mme_backlightStatus[BL_LAST_OPTION];

void mmeBackLight (U8 level)
{
    light_SetStatus(LIGHT_DEVICE_BACKLIGHT,level);
}

void mme_backlightEvent(int event)
{
       int light;
       if ((event >=BL_INIT) && (event <BL_LAST_OPTION))
       {
               light  = mme_backlightStatus[event];
               if (light == BL_SET_IDLE)
               {
                       light_setBacklightIdle();
               }
               else if (light == BL_NO_CHANGE)
               {
                       //No change
               }
               else if ((light >=BL_NO_LIGHT) && (light <=BL_MAX_LIGHT))
               {
                       mmeBackLight ((U8)light);
               }       
       
       }
}

void mme_setBacklightEvent(int event, int lightLevel)
{
        if ((event >=BL_INIT) && (event <BL_LAST_OPTION))
        {
                mme_backlightStatus[event] = lightLevel;
        }
}

void backlightInit() {
         int i;
         for (i=0;i<BL_LAST_OPTION;i++) {
                mme_setBacklightEvent(i,BL_MAX_LIGHT);
         }
         mme_setBacklightEvent(BL_IDLE_TIMER,BL_SET_IDLE);
         mme_setBacklightEvent(BL_KEY_PRESS,BL_MAX_LIGHT);
         mme_setBacklightEvent(BL_INCOMING_CALL,BL_MAX_LIGHT);
         mme_setBacklightEvent(BL_EXIT,BL_NO_LIGHT);

         /*and tell the backlight that the init event has happened */
         mme_backlightEvent(BL_INIT);
}

