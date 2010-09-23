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


#if 0

typedef struct                 /* TIMER CONTROL BLOCK      */

    S32 time;                           /* timeout time             */
    S32 left;                           /* time left before timeout */
} HwTim;

static int timTimeoutCount;             /* overrun counter          */
static int timTimeoutBusy;              /* overrun marker           */
static int timTimerPrecMs;              /* minimum timer intervall  */

static HwTim idletc;

void timStart (HwTim *tc)
{
    S32 left, diff;

    if (tc->time <= 0)
        return;

    if (tc->time < timTimerPrecMs)
        tc->time = timTimerPrecMs;

    tc->left = tc->time;
    left = tmrStop();                   /* get systimer left time   */
    //timRemove(tc);                      /* remove, if running       */
    if (left <= tc->left)
        tc->left -= left;               /* adjust for next timer    */
    else
    {
        diff = left - tc->left;         /* correction value         */
        left = tc->left;                /* new timeout              */
        tc->left = 0;                   /* this is the first        */
    }

    tmrStart(left);                     /* restart timer            */

    return;
}

int timesEvent (void) {
{

   //idleEvent(IdleUpdate);

  mme_backlightEvent(BL_IDLE_TIMER);

  timStart(&idletc);

  return 1;
}

void timInit (void)
{
    void timTimeout (void);

    timTimeoutCount = 0;
    timTimeoutBusy = 0;
    tmrInit(timTimeout);
    tmrStart(1);
    timTimerPrecMs = tmrStop();

    return;
}

void timTimeout (void)
{
    timTimeoutCount++;

    if (timTimeoutBusy)
        return;

    timTimeoutBusy = 1;
    while (timTimeoutCount)
    {
        timTimeoutCount--;
        timSignal();
    }
    timTimeoutBusy = 0;
}

void timSignal (void)
{
    S32 tout;
    UBYTE temp;

        timRoot = timRoot->next;        /* remove element           */

        /* SPR#2029 - DS - Ensure second timer exists. Port of HLE fix. */
          if (timRoot != 0)
            timRoot->next2 = 0;
    }

    if (timRoot)
    {
        tout = timRoot->left;
        tc = timRoot;
        while (tc)
        {
            tc->left -= tout;           /* adjust time left entry   */
            tc = tc->next;
        }
        tmrStart(tout);                 /* start next session       */
    }

    while (ActiveTOut && ActiveTOut->left < 0)          /* signal timout handlers   */
    {
        ActiveTOut->left = 0;

/* PATCH PMC 000721: save the next pointer because the memory associated with 
 * to may be released in the timer handler function.
 */
        timSavedNext = ActiveTOut->next2;
/* END PATCH PMC 000721 */
                
        if (ActiveTOut->handler)
        {
        // PATCH LE 06.06.00
        // store current mfw elem
          current_mfw_elem = ActiveTOut->mfwHeader;      /* SPR#1597 - SH - Change mfw_header to mfwHeader */
        // END PATCH LE 06.06.00

                /* NM, p011b */ 
                temp = dspl_Enable(0);
                /* p011b end */ 
                
                  (void)((*(ActiveTOut->handler))(ActiveTOut->time,ActiveTOut));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
                

                /* NM, p011c */
                dspl_Enable(temp);
                /* p011c end */   
                
        }
/* PATCH PMC 000721: use the SavedNext pointer to set ActiveTOut */
                ActiveTOut = timSavedNext;
    /* cq18182 pointer cleared here, this fix is only temporary as it seems to fix the current problem, however further investigation
          is required as to why the timSavedNext pointer was not being cleared. 10-03-04 MZ. */
                timSavedNext = NULL;
/* END PATCH PMC 000721 */
    }

        /* cq18182 add check and clear the pointer 10-03-04 MZ.*/
        if(ActiveTOut != NULL)
                ActiveTOut = NULL;

}

#endif
