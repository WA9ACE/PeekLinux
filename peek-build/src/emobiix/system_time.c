#include "system_time.h"

#include "DataObject.h"
#include "URL.h"
#include "hwtimer.h"

extern char *hw_td_get_clock_str(void);

static DataObject *SYSTEM_TIME;
static DataObjectField *SYSTEM_TIME_NOW;

#define TIME_UPDATE_INTERVAL (45 * 1000)

void system_set_time()
{
	const char *time = hw_td_get_clock_str();

	if (!SYSTEM_TIME)
		system_time_init();

	emo_printf("system_set_time() setting time to %s", time);

	dataobjectfield_setString(SYSTEM_TIME_NOW, time);
	dataobjectfield_setIsModified(SYSTEM_TIME_NOW, 1);
	dataobject_setIsModified(SYSTEM_TIME, 1);

	lgui_set_dirty();
	updateScreen();
}

static void updateTimeCB(tDS *timeData, void *opaque)
{
	system_set_time();
}

void system_time_init()
{
	tDS *t;
	URL *url;

	dataobject_platformInit();

	url = url_parse(SYSTEM_TIME_URI, URL_ALL);

	SYSTEM_TIME = dataobject_construct(url, 1);
	SYSTEM_TIME_NOW = dataobjectfield_string(hw_td_get_clock_str());

	dataobject_setValue(SYSTEM_TIME, "data", SYSTEM_TIME_NOW);

	url_delete(url);

	t = timerCreate(updateTimeCB, 0);
	emo_printf("system_time_init() timer returned %d", timerStart(t, TIME_UPDATE_INTERVAL));
}

