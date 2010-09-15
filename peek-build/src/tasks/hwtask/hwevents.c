#include "hwtask.h"
#include "Debug.h"
#include "p_malloc.h"
#include "List.h"
#include "Map.h"

static Map* hwGetSubs()
{
	static Map *hwSubs = 0; 

	if (!hwSubs)
		hwSubs = map_int();

	return hwSubs;
}

static int hwEventComparator(void *lhs, void *rhs)
{
	if (!lhs || !rhs)
		return 0;

	return ((HW_EVENT *)lhs)->cb == ((HW_EVENT *)rhs)->cb;
}

int hwEventSubscribe(hwEventType eventType, HW_EVENT_CB cb, void *context)
{
	List *hwCbList = (List *)map_find(hwGetSubs(), (void *)eventType);
	HW_EVENT *hwEvent = p_malloc(sizeof(HW_EVENT));

	emo_printf("%s(): Installing event %d (%08X)", __FUNCTION__, eventType, cb);

	if (!hwCbList)
	{
		hwCbList = list_new();
		map_append(hwGetSubs(), (void *)eventType, (void *)hwCbList);
	}
	
	hwEvent->cb = cb;
	hwEvent->context = context;

	list_append(hwCbList, (void *)hwEvent);
	return 0;
}

int hwEventUnsubscribe(hwEventType eventType, HW_EVENT_CB cb)
{
	static HW_EVENT eventKey;
	List *hwCbList = (List *)map_find(hwGetSubs(), (void *)eventType);
	ListIterator *event;

	if (!hwCbList)
	{
		emo_printf("%s(): No events registered %d (%08X)", __FUNCTION__, eventType, cb);
		return 0;
	}

	eventKey.cb = cb;
	event = (ListIterator *)list_find(hwCbList, (void *)&eventKey, (ListComparitor)(hwEventComparator));
	if (!event) 
	{
		emo_printf("%s(): Event not registered %d (%08X)", __FUNCTION__, eventType, cb);
		return 0;
	}

	emo_printf("%s(): Removing event %d (%08X)", __FUNCTION__, eventType, cb);
	listIterator_remove(event);
	p_free(event);
	return 0;
}

int hwEventTrigger(hwEventType eventType, void *eventData)
{
	List *hwCbList = (List *)map_find(hwGetSubs(), (void *)eventType);
	ListIterator it;

	if (!hwCbList)
	{
		emo_printf("%s(): Event not registered %d", __FUNCTION__, eventType);
		return 0;
	}

	list_begin(hwCbList, &it);
	while (!listIterator_finished(&it))
	{
		HW_EVENT *event = (HW_EVENT *)listIterator_item(&it);
		int ret;

		emo_printf("%s(): Trigger event registered %d (%08X)", __FUNCTION__, eventType, event->cb);

		ret = event->cb(eventType, eventData, event->context);

		emo_printf("%s(): Event registered %d (%08X) returned %d", __FUNCTION__, eventType, event->cb, ret);
		
		listIterator_next(&it);
	}
	return 0;
}

