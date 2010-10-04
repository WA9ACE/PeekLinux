/* 
 * Timer functions
 *
 * Timers are kept in a queued list organized by next time to run. 
 * Timers are moved to running list after they haved timed out then removed after cb executed.
 */

#include "typedefs.h"
#include "hwtimer.h"
#include "tmr.h"
#include "List.h"

static int timerTimeoutCount;             /* overrun counter          */
static int timerTimeoutBusy;              /* overrun marker           */
static int timerPrecMs;              /* minimum timer intervall  */

/* Pending Timer Queue */
List *timer_get_queue()
{
    static List *timer_queue = NULL;
    if (!timer_queue)
        timer_queue = list_new();

    return timer_queue;
}

/* Running Timer Queue */
List *timer_get_r_queue()
{
    static List *timer_r_queue = NULL;
    if (!timer_r_queue)
        timer_r_queue = list_new();

    return timer_r_queue;
}

/* Init and obtain timer precision */
void timerInit(void)
{
	timerTimeoutCount = 0;
	timerTimeoutBusy = 0;
	tmrInit(timerTimeout);

	/* Obtain precision of irq tick */
	tmrStart(1);
	timerPrecMs = tmrStop();
}

/* Insert new element into timer event list */
tDS *timerCreate(timerCB *tcb, unsigned int time, void *opaque)
{
	List *timeEventQ = timer_get_queue();
	tDS *timeData = (void *)malloc(sizeof(tDS));

	if(!timeData)		
		return NULL;

	timeData->tCB = tcb;
	timeData->time = time;
	timeData->left = 0;
	timeData->tData = opaque;

	return timeData;
}

static void timerInsert(tDS *timeData)
{
	List *timeEventQ = timer_get_queue();
	ListIterator iter;

    for(list_begin(timeEventQ, &iter); !listIterator_finished(&iter); listIterator_next(&iter))
	{
		tDS *iTimeData = listIterator_item(&iter);
		if(timeData->left >= iTimeData->left)
		{
			listIterator_insertBefore(&iter, timeData);
			break;
		}
	}

}

/* Start timer */
int timerStart(tDS *timeData)
{
	List *timeEventQ = timer_get_queue();
	ListIterator iter;
	tDS *fTimeData;
	int left, diff;

	if(timeData->time <= 0)
		return -1;

	if(timeData->time < timerPrecMs)
		timeData->time = timerPrecMs;

	timeData->left = timeData->time;
	left = tmrStop();	/* get systimer left */
	timerRemove(timeData); /* remove if running */

	if(left <= timeData->left) 
		timeData->left -= left;	/* Adjust for next timer */
	else
	{
		diff = left - timeData->left; /* Correction value */
		left = timeData->left;		  /* New timeout */
		timeData = 0;
		timersAdjust(diff);
	}

	timerInsert(timeData);

	if(!left)
	{
		/* Sync all timers */
		fTimeData = (tDS *)list_firstItem(timeEventQ);

		if(!fTimeData)
			return -1;
		last = fTimeData->last;

		for(list_begin(timeEventQ, &iter); !listIterator_finished(&iter); listIterator_next(&iter)) 
		{
			timeData = (tDS *)listIterator_item(&iter);
			timeData->left -= left;
		}
	}

	tmrStart(left); /* restart timer */

	return 0;
}

/* Adjust all timers in list */
static void timerAdjust(unsigned int time)
{
	List *timeEventQ = timer_get_queue();
	tDS *timeData;
	ListIterator iter;

	for(list_begin(timeEventQ, &iter); !listIterator_finished(&iter); listIterator_next(&iter))
    {
    	timeData = (tDS *)listIterator_item(&iter);
        timeData->left += time;
    }
}

/* Remove timer queued or active */
static void timerRemove(tDS *timeData) 
{
    List *timeEventQ = timer_get_queue();
    List *timeEventR = timer_get_r_queue();
    tDS *curtimeData = NULL;
    ListIterator iter;

	/* Check queued first */
    for(list_begin(timeEventQ, &iter); !listIterator_finished(&iter); listIterator_next(&iter))
    {
		curtimeData = (tDS *)listIterator_item(&iter);
		if(curtimeData == timeData)
		{
			listIterator_remove(&iter);
			return;
		}
    }
 
	/* Active now.. What happens if its running and gets preempted? */	
    for(list_begin(timeEventR, &iter); !listIterator_finished(&iter); listIterator_next(&iter))
    {
        curtimeData = (tDS *)listIterator_item(&iter);
        if(curtimeData == timeData)
        {
            listIterator_remove(&iter);
			return;
        }
    }

	return;
}

/* Reset and stop timer */
void timerStop(tDS *timeData) 
{
	timeData->left = 0;
	timeData->time = 0;
	
	timerRemove(timeData);

	return;
}

/* thread context timeout */
void timerTimeout (void)
{
    timerTimeoutCount++;

    if (timerTimeoutBusy)
        return;

    timerTimeoutBusy = 1;
    while (timerTimeoutCount)
    {
        timerTimeoutCount--;
        timerSignal();
    }
    timerTimeoutBusy = 0;
}

void timerSignal(void)
{
    List *timeEventQ = timer_get_queue();
	List *timeEventR = timer_get_r_queue();
	tDS	*timeData = NULL;
    ListIterator iter;
	int timeout;

    if(!list_size(timeEventQ))
        return;

    for(list_begin(timeEventQ, &iter); !listIterator_finished(&iter); listIterator_next(&iter))
    {
        timeData = (tDS *)listIterator_item(&iter);

		if(!timeData->left) 
		{
			/* Item has timed out. Add to Running queue */
        	timeData->left = -1;
			list_append(timeEventR, (void *)timeData);
			listIterator_remove(&iter);
		} else {
			/* No more elements needing to run so break */
			break;
		}
		
    }

	if(list_size(timeEventQ))
	{
		/* Still elements in queue so update and restart */
		for(list_begin(timeEventQ, &iter); !listIterator_finished(&iter); listIterator_next(&iter))
		{
			timeData = (tDS *)listIterator_item(&iter);
			timeData->left -= timeout;
		}

		/* Restart the timer */
		tmrStart(timeout); 
	}

    if(list_size(timeEventR))
    {
		/* Start elements in running queue */
		for(list_begin(timeEventR, &iter); !listIterator_finished(&iter); listIterator_next(&iter))
		{
			timeData = (tDS *)listIterator_item(&iter);

			if(timeData->tCB)
			{
				/* Call Handler */
				(void)((*(timeData->tCB))(timeData));
			}

			listIterator_remove(&iter);

		}
	}

}

