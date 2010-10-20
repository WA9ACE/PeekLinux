#include "Gprs.h"
#include "Debug.h"
#include "DataObject.h"
#include "URL.h"

static DataObject* GPRS_DO = NULL;
static DataObjectField* GPRS_SIGNAL_LEVEL;
static DataObjectField* GPRS_STATUS;
static DataObjectField* GPRS_ON;
static DataObjectField* GPRS_EMOBIIX_ON;
static DataObjectField* GPRS_LOCATION_LAC;
static DataObjectField* GPRS_LOCATION_CI;

void force_dataobject_redraw()
{
	lgui_set_dirty();
	updateScreen();
}

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
	GPRS_EMOBIIX_ON = dataobjectfield_uint(0);
	GPRS_LOCATION_LAC = dataobjectfield_uint(0);
	GPRS_LOCATION_CI = dataobjectfield_uint(0);

	dataobject_setValue(GPRS_DO, "signal-level", GPRS_SIGNAL_LEVEL);
	dataobject_setValue(GPRS_DO, "status", GPRS_STATUS);
	dataobject_setValue(GPRS_DO, "gprs-on", GPRS_ON);
	dataobject_setValue(GPRS_DO, "emobiix-on", GPRS_EMOBIIX_ON);
	dataobject_setValue(GPRS_DO, "lac", GPRS_LOCATION_LAC);
	dataobject_setValue(GPRS_DO, "ci", GPRS_LOCATION_CI);

	/* Set default signal in simulator */
	if(!simAutoDetect()) {
		GPRS_ON->field.uinteger = 1;
		GPRS_SIGNAL_LEVEL->field.uinteger = 3;
	}
}

void gprs_set_status(BOOL status)
{
	if (!GPRS_DO)
		gprs_dataobject_init();

	GPRS_STATUS->field.uinteger = status;
	dataobjectfield_setIsModified(GPRS_STATUS, 1);
	dataobject_setIsModified(GPRS_DO, 1);

	force_dataobject_redraw();
}

void gprs_set_on(BOOL status)
{
	if (!GPRS_DO)
		gprs_dataobject_init();

	emo_printf("GPRS data is %d", status);
	GPRS_ON->field.uinteger = status;
	dataobjectfield_setIsModified(GPRS_ON, 1);
	dataobject_setIsModified(GPRS_DO, 1);

	force_dataobject_redraw();
}

void gprs_set_emobiix_on(BOOL status)
{
	if (!GPRS_DO)
		gprs_dataobject_init();

	emo_printf("EMOBIIX connection is %d", status);
	GPRS_EMOBIIX_ON->field.uinteger = status;
	dataobjectfield_setIsModified(GPRS_EMOBIIX_ON, 1);
	dataobject_setIsModified(GPRS_DO, 1);

	force_dataobject_redraw();
}

void gprs_set_signal_level(U32 level)
{
	if (!GPRS_DO)
		gprs_dataobject_init();

	emo_printf("GPRS Signal Level Update: %d", level);
	GPRS_SIGNAL_LEVEL->field.uinteger = level;
	dataobjectfield_setIsModified(GPRS_SIGNAL_LEVEL, 1);
	dataobject_setIsModified(GPRS_DO, 1);

	force_dataobject_redraw();
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

