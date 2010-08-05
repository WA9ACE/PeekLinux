#include "PeekEmobiix.h"

#include "Hardware.h"
#include "DataObject.h"
#include "URL.h"

static DataObject *batteryObject;
static DataObjectField *batteryLevel;
static DataObjectField *batteryCharging;

static DataObject *signalObject;
static DataObjectField *signalLevel;
static DataObjectField *signalGPRS;

static char *LEVEL[] = {"0", "1", "2", "3", "4", "5"};

void HardwareInit(void)
{
	URL *url;

	url = url_parse("system://local/battery", URL_ALL);
	batteryObject = dataobject_construct(url, 1);
	batteryLevel = dataobjectfield_string(LEVEL[0]);
	dataobject_setValue(batteryObject, "level", batteryLevel);
	batteryCharging = dataobjectfield_string(LEVEL[0]);
	dataobject_setValue(batteryObject, "charging", batteryCharging);

	url = url_parse("system://local/signal", URL_ALL);
	signalObject = dataobject_construct(url, 1);
	signalLevel = dataobjectfield_string(LEVEL[0]);
	dataobject_setValue(signalObject, "level", batteryLevel);
	signalGPRS = dataobjectfield_string(LEVEL[0]);
	dataobject_setValue(signalObject, "GPRS", signalGPRS);
}

void peek_setSignalLevel(int i)
{
	if (i < 0 || i > 5)
		return;
	signalLevel->field.string = LEVEL[i];
}

void peek_setGRPSStatus(int i)
{
	if (i)
		signalGPRS->field.string = LEVEL[1];
	else
		signalGPRS->field.string = LEVEL[0];
}

void peek_setBatteryLevel(int i)
{
	if (i < 0 || i > 5)
		return;
	batteryLevel->field.string = LEVEL[i];
}

void peek_setCharging(int i)
{
	if (i)
		batteryCharging->field.string = LEVEL[1];
	else
		batteryCharging->field.string = LEVEL[0];
}