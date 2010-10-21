#include "system_time.h"

#include "DataObject.h"
#include "URL.h"
#include "hwtimer.h"
#include "rtc.h"

extern char *hw_td_get_clock_str(void);
extern T_RTC_TIME* hw_td_get_time(void);

static DataObject *SYSTEM_TIME;
static DataObjectField *SYSTEM_TIME_NOW;
static DataObjectField *SYSTEM_TIME_H1;
static DataObjectField *SYSTEM_TIME_H2;
static DataObjectField *SYSTEM_TIME_M1;
static DataObjectField *SYSTEM_TIME_M2;

#define TIME_UPDATE_INTERVAL (45 * 1000)

static void system_time_set_components()
{
	const char *time = hw_td_get_clock_str();
	const T_RTC_TIME *timev = hw_td_get_time();

	emo_printf("system_set_time() setting time to %s", time);

	dataobjectfield_setString(SYSTEM_TIME_NOW, time);

	SYSTEM_TIME_H1->field.uinteger = timev->hour / 10;
	SYSTEM_TIME_H2->field.uinteger = timev->hour % 10;
	SYSTEM_TIME_M1->field.uinteger = timev->minute / 10;
	SYSTEM_TIME_M2->field.uinteger = timev->minute % 10;
}

static void updateTimeCB(tDS *timeData, void *opaque)
{	
	if (!SYSTEM_TIME)
		system_time_init();

	system_time_set_components();

	dataobjectfield_setIsModified(SYSTEM_TIME_NOW, 1);
	dataobjectfield_setIsModified(SYSTEM_TIME_H1, 1);
	dataobjectfield_setIsModified(SYSTEM_TIME_H2, 1);
	dataobjectfield_setIsModified(SYSTEM_TIME_M1, 1);
	dataobjectfield_setIsModified(SYSTEM_TIME_M2, 1);

	dataobject_setIsModified(SYSTEM_TIME, 1);

	lgui_set_dirty();
	updateScreen();

	emo_printf("system_time_init() timer returned %d", timerStart(timeData, TIME_UPDATE_INTERVAL));
}

void system_time_init()
{
	tDS *t;
	URL *url;

	dataobject_platformInit();

	url = url_parse(SYSTEM_TIME_URI, URL_ALL);

	SYSTEM_TIME = dataobject_construct(url, 1);

	SYSTEM_TIME_NOW = dataobjectfield_string("");
	SYSTEM_TIME_H1 = dataobjectfield_uint(0);
	SYSTEM_TIME_H2 = dataobjectfield_uint(0);
	SYSTEM_TIME_M1 = dataobjectfield_uint(0);
	SYSTEM_TIME_M2 = dataobjectfield_uint(0);

	system_time_set_components();

	dataobject_setValue(SYSTEM_TIME, "data", SYSTEM_TIME_NOW);
	dataobject_setValue(SYSTEM_TIME, "H1", SYSTEM_TIME_H1);
	dataobject_setValue(SYSTEM_TIME, "H2", SYSTEM_TIME_H2);
	dataobject_setValue(SYSTEM_TIME, "M1", SYSTEM_TIME_M1);
	dataobject_setValue(SYSTEM_TIME, "M2", SYSTEM_TIME_M2);

	url_delete(url);

	t = timerCreate(updateTimeCB, 0);
	emo_printf("system_time_init() timer returned %d", timerStart(t, TIME_UPDATE_INTERVAL));
}

