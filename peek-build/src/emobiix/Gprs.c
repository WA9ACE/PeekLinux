#include "Gprs.h"
#include "Debug.h"
#include "DataObject.h"
#include "URL.h"

/**
 *  GPRS call back function
 *  High-4 bit indicates the device signal and the Low-4 bit indicated GPRS attached whether.
 *
 **/

//static uint8 mSignal;
//static bool mGprs;

static DataObject* GPRS_DO = NULL;
static DataObjectField* GPRS_SIGNAL_LEVEL;
static DataObjectField* GPRS_STATUS;
static DataObjectField* GPRS_ON;
static DataObjectField* GPRS_LOCATION_LAC;
static DataObjectField* GPRS_LOCATION_CI;

void gprs_dataobject_init(void) 
{
	URL *url;

	emo_printf("gprs_dataobject_init()");

	dataobject_platformInit();

	url = url_parse(GPRS_URI, URL_ALL);
	GPRS_DO = dataobject_construct(url, 1);

	GPRS_SIGNAL_LEVEL = dataobjectfield_uint(GPRS_NOT_REGISTERED);
	GPRS_STATUS = dataobjectfield_uint(GPRS_NOT_REGISTERED);
	GPRS_ON = dataobjectfield_uint(0);
	GPRS_LOCATION_LAC = dataobjectfield_uint(0);
	GPRS_LOCATION_CI = dataobjectfield_uint(0);

	dataobject_setValue(GPRS_DO, "signal-level", GPRS_SIGNAL_LEVEL);
	dataobject_setValue(GPRS_DO, "status", GPRS_STATUS);
	dataobject_setValue(GPRS_DO, "gprs-on", GPRS_ON);
	dataobject_setValue(GPRS_DO, "lac", GPRS_LOCATION_LAC);
	dataobject_setValue(GPRS_DO, "ci", GPRS_LOCATION_CI);
}

void gprs_set_status(BOOL status)
{
	if (!GPRS_DO)
		gprs_dataobject_init();

	GPRS_STATUS->field.uinteger = status;
	dataobjectfield_setIsModified(GPRS_STATUS, 1);
	dataobject_setIsModified(GPRS_DO, 1);

    lgui_set_dirty();
	updateScreen();
}

void gprs_set_on(BOOL status)
{
	if (!GPRS_DO)
		gprs_dataobject_init();

	emo_printf("GPRS data is %d", status);
	GPRS_ON->field.uinteger = status;
	dataobjectfield_setIsModified(GPRS_ON, 1);
	dataobject_setIsModified(GPRS_DO, 1);

    lgui_set_dirty();
	updateScreen();
}

void gprs_set_signal_level(U32 level)
{
	if (!GPRS_DO)
		gprs_dataobject_init();

	emo_printf("GPRS Signal Level Update: %d", level);
	GPRS_SIGNAL_LEVEL->field.uinteger = level;
	dataobjectfield_setIsModified(GPRS_SIGNAL_LEVEL, 1);
	dataobject_setIsModified(GPRS_DO, 1);

    lgui_set_dirty();
	updateScreen();
}

void gprs_set_location(U32 lac, U32 ci)
{
	if (!GPRS_DO)
		gprs_dataobject_init();

	emo_printf("GPRS Location Update (lac: %d, ci: %d)", lac, ci);
	GPRS_LOCATION_LAC->field.uinteger = lac;
	GPRS_LOCATION_CI->field.uinteger = ci;
	dataobjectfield_setIsModified(GPRS_LOCATION_LAC, 1);
	dataobjectfield_setIsModified(GPRS_LOCATION_CI, 1);
	dataobject_setIsModified(GPRS_DO, 1);
}

