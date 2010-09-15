#ifndef __HWTASK_H__
#define __HWTASK_H__

typedef enum 
{
	HW_KEYBOARD_EVENT,
	HW_DISPLAY_EVENT,
	HW_SIM_EVENT
} hwEventType;

typedef int (*HW_EVENT_CB)(hwEventType eventType, void *eventData, void *context);

typedef struct
{
	HW_EVENT_CB cb;
	void *context;
} HW_EVENT;

int hwEventSubscribe(hwEventType eventType, HW_EVENT_CB cb, void *context);
int hwEventUnsubscribe(hwEventType eventType, HW_EVENT_CB cb);
int hwEventTrigger(hwEventType eventType, void *eventData);

#endif // __HWTASK_H__

