#include "DataObject.h"
#include "URL.h"

#include "system_battery.h"

#include "balbattapi.h"

static DataObject* SYSTEM_BATTERY;

static void system_battery_set_battery_level(uint32 level)
{
	if (!SYSTEM_BATTERY)
		return;

	emo_printf("Battery Level Update: %d", level);
	dataobject_setValue(SYSTEM_BATTERY, "battery-level", dataobjectfield_uint(level));
}

static void system_battery_set_charge_state(uint32 state)
{
	if (!SYSTEM_BATTERY)
		return;

	emo_printf("Charge State Update: %d", state);
	dataobject_setValue(SYSTEM_BATTERY, "charge-state", dataobjectfield_uint(state));
}

static void system_battery_callback(RegIdT RegId, uint32 MsgId, void* MsgBufferP)
{
	switch (MsgId)
	{
		case BAL_BATT_CHARGE_STATE:
		{
			system_battery_set_charge_state((uint32)MsgBufferP);
		}
		break;

		case BAL_BATT_CAPACITY:
		{
			system_battery_set_battery_level((uint32)MsgBufferP);
		}
		break;
		
		default:
		break;
	}
}

void system_battery_init()
{
	URL *url = url_parse(SYSTEM_BATTERY_URI, URL_ALL);
	
	SYSTEM_BATTERY = dataobject_construct(url, 1);
	
	system_battery_set_battery_level(BAL_BATT_LEVEL_NOT_INIT);
	system_battery_set_charge_state(BALBATT_CHARGE_PREPARE);

	BalBattRegister(system_battery_callback);
}

