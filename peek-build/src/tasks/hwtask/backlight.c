/* Backlight related functions */
#include "typedefs.h"
#include "header.h"
#include "light.h"
#include "hwtimer.h"
#include "vsi.h"

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

#define BACKLIGHT_IDLE 30000
static unsigned int backlight_timer = 0;
static BOOL backlight_sleep = 0;
static tDS *idleTime;

extern T_HANDLE aci_handle;

void blightSnaptime(void) {
	T_TIME t;

	return;
	vsi_t_time(aci_handle, &t);
	backlight_timer = t / 1000;
	if(backlight_sleep) {
		backlight_sleep = 0;
        light_SetStatus(LIGHT_DEVICE_BACKLIGHT, BL_MAX_LIGHT);
		if(idleTime)
			timerStart(idleTime, BACKLIGHT_IDLE);
	}
}

static void idleBacklight(tDS *timeData, void *opaque)
{
	T_TIME t;
	long timeElapsed;

	vsi_t_time(aci_handle, &t);

	timeElapsed = t - backlight_timer;

	/* Idle time out 60 seconds */
	if(timeElapsed >= BACKLIGHT_IDLE) {
		light_setBacklightIdle();
		backlight_sleep = 1;
		return;
	} 
		
    emo_printf("idleBacklight() timerstart ret %d", timerStart(timeData, BACKLIGHT_IDLE - timeElapsed));
}

void backlightInit() {
         int i;

		 light_Init();

		/*
         for (i=0;i<BL_LAST_OPTION;i++) {
                mme_setBacklightEvent(i,BL_MAX_LIGHT);
         }
         mme_setBacklightEvent(BL_IDLE_TIMER,BL_SET_IDLE);
         mme_setBacklightEvent(BL_KEY_PRESS,BL_MAX_LIGHT);
         mme_setBacklightEvent(BL_INCOMING_CALL,BL_MAX_LIGHT);
         mme_setBacklightEvent(BL_EXIT,BL_NO_LIGHT);
		*/

        ///mme_backlightEvent(BL_INIT);
	//	blightSnaptime();
	//	idleTime = timerCreate(idleBacklight, NULL);
   // 	timerStart(idleTime, BACKLIGHT_IDLE);

		light_SetStatus(LIGHT_DEVICE_BACKLIGHT, BL_MAX_LIGHT);
}

