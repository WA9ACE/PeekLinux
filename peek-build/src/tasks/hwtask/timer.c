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
#include "vsi.h"

extern T_HANDLE aci_handle;

static int timerTimeoutCount;             /* overrun counter          */
static int timerTimeoutBusy;              /* overrun marker           */
static int timerPrecMs;              /* minimum timer intervall  */
static T_TIME timerExpected;

struct tDS {
	timerCB tCB;
	void *tData;
	int left;
	int time;
};

static int timerGetDrift()
{
	T_TIME now;
	vsi_t_time(aci_handle, &now);
	
	emo_printf("timerGetDrift now:%d expected:%d drift:%d", now, timerExpected, (int)now - (int)timerExpected);

	return (int)now - (int)timerExpected;
}

static void timerSetExpected(int offset)
{
	vsi_t_time(aci_handle, &timerExpected);
	timerExpected += offset;
}

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
	emo_printf("timerInit");
	timerTimeoutCount = 0;
	timerTimeoutBusy = 0;
	timerExpected = 0;
	tmrInit(timerTimeout);

	/* Obtain precision of irq tick */
	tmrStart(1);
	timerPrecMs = tmrStop();
	emo_printf("timerInit() - timerPrecMs - 0x%08x", timerPrecMs);
}

/* Insert new element into timer event list */
tDS *timerCreate(timerCB tcb, void *opaque)
{
	List *timeEventQ = timer_get_queue();
	tDS *timeData = (void *)malloc(sizeof(tDS));

	//emo_printf("timerCreate %08X", timeData);

	if(!timeData)		
		return NULL;

	timeData->tCB = tcb;
	timeData->left = 0;
	timeData->tData = opaque;

	return timeData;
}

static void timerInsert(tDS *timeData)
{
	List *timeEventQ = timer_get_queue();
	ListIterator iter;

	//emo_printf("timerInsert %08X", timeData);

	if (!list_size(timeEventQ))
	{
		list_append(timeEventQ, timeData);
		return;
	}

	for(list_rbegin(timeEventQ, &iter); !listIterator_finished(&iter); listIterator_next(&iter))
	{
		tDS *iTimeData = listIterator_item(&iter);
		//emo_printf("timerInsert %08X left %d, %08X left %d", timeData, timeData->left, iTimeData, iTimeData->left);
		if (timeData->left >= iTimeData->left)
		{
			//emo_printf("timerInsert after");
			listIterator_insertAfter(&iter, timeData);
			return;
		}
	}

	//emo_printf("timerInsert front");
	list_prepend(timeEventQ, timeData);
}

static void timersAdjust(int time)
{
	List *timeEventQ = timer_get_queue();
	tDS *timeData = NULL;
	ListIterator iter;

	for(list_begin(timeEventQ, &iter); !listIterator_finished(&iter); listIterator_next(&iter))
	{
		timeData = (tDS *)listIterator_item(&iter);
		timeData->left += time;
	}
}

/* Start timer */
int timerStart(tDS *timeData, unsigned int time)
{
	List *timeEventQ = timer_get_queue();
	ListIterator iter;
	tDS *fTimeData;
	int left, diff;

	timeData->time = time;

	if(timeData->time <= 0)
		return -1;

	//emo_printf("timerStart %08X", timeData);

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
		timersAdjust(diff);
	}

	timerInsert(timeData);

	if(!left)
	{
		/* Sync all timers */
		fTimeData = (tDS *)list_firstItem(timeEventQ);

		if(!fTimeData)
			return -1;
		left = fTimeData->left;

		for(list_begin(timeEventQ, &iter); !listIterator_finished(&iter); listIterator_next(&iter)) 
		{
			timeData = (tDS *)listIterator_item(&iter);
			timeData->left -= left;
		}
	}

	//emo_printf("timerStart() starting timer left - %d", left);
//	timerSetExpected(left);
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
			emo_printf("timerRemove queued %08X", timeData);
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
			emo_printf("timerRemove active %08X", timeData);
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
	emo_printf("timerTimeout");
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
//	int drift = timerGetDrift();
	int timeout;

	//emo_printf("timerSignal");

	if(!list_size(timeEventQ))
		return;

//	timersAdjust(-drift);

	for(list_begin(timeEventQ, &iter); !listIterator_finished(&iter); listIterator_next(&iter))
	{
		timeData = (tDS *)listIterator_item(&iter);

		//emo_printf("timerSignal item %08X left %d", timeData, timeData->left);
	
		if(timeData->left <= 0) 
		{
			/* Item has timed out. Add to Running queue */
			timeData->left = -1;
			list_append(timeEventR, (void *)timeData);
			//emo_printf("timerSignal timed out %08X", timeData);
			listIterator_remove(&iter);
		} else {
			/* No more elements needing to run so break */
			timeout = timeData->left;
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
//		timerSetExpected(timeout);
		tmrStart(timeout); 
	}

	if(list_size(timeEventR))
	{
		/* Start elements in running queue */
		for (list_begin(timeEventR, &iter); !listIterator_finished(&iter); listIterator_next(&iter))
		{
			timeData = (tDS *)listIterator_item(&iter);

			/* timer expired..  */
			//emo_printf("timerSignal expired %08X", timeData);
			listIterator_remove(&iter);

			/* Call Handler */
			if(timeData->tCB)
				(timeData->tCB)(timeData, timeData->tData);
		}
	}

}

