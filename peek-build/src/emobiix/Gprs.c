#include "Gprs.h"
#include "Debug.h"
#include "DataObject.h"
#include "URL.h"
#include "ConnectionContext.h"

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

#define SYSTEM_WEATHER_URI "tcp://remote/weather"

extern ConnectionContext *connectionContext;

void system_weather_request()
{
	static DataObject *WEATHER = NULL;
	static URL *weatherUrl;

	if (!WEATHER)
	{
		weatherUrl = url_parse(SYSTEM_WEATHER_URI, URL_ALL);
		WEATHER = dataobject_construct(weatherUrl, 0);
		dataobject_setValue(WEATHER, "data", dataobjectfield_string(""));
		dataobject_setValue(WEATHER, "onsyncfinished", dataobjectfield_string(
				"me = DataObject.this()\n"
				"d = DataObject.locate(\"weatherinfo\")\n"
				"lbl = \"ZIP: \" .. me:getValue(\"zipcode\") .. \" TEMP:\" .. me:getValue(\"temp\")\n"
				"print(lbl)\n"
				"d:setValue(\"data\", lbl)\n"
			)
		);
	
	dataobject_setValue(WEATHER, "lac", dataobjectfield_uint(GPRS_LOCATION_LAC->field.uinteger));
	dataobject_setValue(WEATHER, "ci", dataobjectfield_uint(GPRS_LOCATION_CI->field.uinteger));
	}
	if (connectionContext)
	{
		emo_printf("Requesting weather");
		connectionContext_syncRequestForce(connectionContext, weatherUrl, WEATHER);
	}
}

void gprs_set_status(char status)
{
	if (!GPRS_DO)
		gprs_dataobject_init();

	GPRS_STATUS->field.uinteger = status;
	dataobjectfield_setIsModified(GPRS_STATUS, 1);
	dataobject_setIsModified(GPRS_DO, 1);

	force_dataobject_redraw();
}

void gprs_set_on(char status)
{
	if (!GPRS_DO)
		gprs_dataobject_init();

	emo_printf("GPRS data is %d", status);
	GPRS_ON->field.uinteger = status;
	dataobjectfield_setIsModified(GPRS_ON, 1);
	dataobject_setIsModified(GPRS_DO, 1);

	force_dataobject_redraw();
}

void gprs_set_emobiix_on(char status)
{
	if (!GPRS_DO)
		gprs_dataobject_init();

	emo_printf("EMOBIIX connection is %d", status);
	GPRS_EMOBIIX_ON->field.uinteger = status;
	dataobjectfield_setIsModified(GPRS_EMOBIIX_ON, 1);
	dataobject_setIsModified(GPRS_DO, 1);

	if (status == 1)
		system_weather_request();

	force_dataobject_redraw();
}

void gprs_set_signal_level(unsigned int level)
{
	if (!GPRS_DO)
		gprs_dataobject_init();

	emo_printf("GPRS Signal Level Update: %d", level);
	GPRS_SIGNAL_LEVEL->field.uinteger = level;
	dataobjectfield_setIsModified(GPRS_SIGNAL_LEVEL, 1);
	dataobject_setIsModified(GPRS_DO, 1);

	force_dataobject_redraw();
}

void gprs_set_location(unsigned int lac, unsigned int ci)
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

