#include "typedefs.h"

#include "DataObject.h"
#include "URL.h"

#include "system_battery.h"

//#include "balbattapi.h"

#include "vsi.h"
#include "gsm.h"
#include "p_sim.h"
#include "p_aci.h"
#include "p_mmi.h"
#include "pwr.h"

static DataObject* SYSTEM_BATTERY;
static DataObjectField* BATTERY_CHARGE_LEVEL;
static DataObjectField* BATTERY_CHARGE_STATE;

static void system_battery_set_battery_level(unsigned int level)
{
	if (!SYSTEM_BATTERY)
		return;

	emo_printf("Battery Charge Level Update: %d", level);
	BATTERY_CHARGE_LEVEL->field.uinteger = level;
	dataobject_setValue(SYSTEM_BATTERY, "charge-level", BATTERY_CHARGE_LEVEL);
}

static void system_battery_set_charge_state(unsigned int state)
{
	if (!SYSTEM_BATTERY)
		return;

	emo_printf("Battery Charge State Update: %d", state);
	BATTERY_CHARGE_STATE->field.uinteger = state;
	dataobject_setValue(SYSTEM_BATTERY, "charge-state", BATTERY_CHARGE_STATE);
}

#if 0
static void system_battery_callback(RegIdT RegId, unsigned int MsgId, void* MsgBufferP)
{
	switch (MsgId)
	{
		case BAL_BATT_CHARGE_STATE:
		{
			system_battery_set_charge_state((unsigned int)MsgBufferP);
		}
		break;

		case BAL_BATT_CAPACITY:
		{
			system_battery_set_battery_level((unsigned int)MsgBufferP);
		}
		break;
		
		default:
		break;
	}
}
#endif

#define hCommACI aci_hCommACI

typedef  BOOL (*T_PRIM_HANDLER)(ULONG, void *);
EXTERN T_HANDLE         hCommACI; 

static void pwrCb (drv_SignalID_Type *signal_params)
{
    pwr_Status_Type *para;
    PALLOC(battery_ind, MMI_BATTERY_IND);

    para = (pwr_Status_Type *) signal_params->UserData;
    battery_ind->volt = para->BatteryLevel;
    battery_ind->temp = para->Status;

    PSENDX(ACI,battery_ind);
}

static BOOL mmePrimHandler (USHORT opc, void *data)
{
     U8 level;
     U8 state;

	emo_printf("mmePrimHandler()");
 
     switch (opc)
     {
         case MMI_BATTERY_IND:
             level = ((T_MMI_BATTERY_IND *) data)->volt;
             state = ((T_MMI_BATTERY_IND *) data)->temp;
	

 			 system_battery_set_battery_level(level);
		     system_battery_set_charge_state(state);

			 emo_printf("BATTERY_IND: %d %d", level, state);
 			 return TRUE;
		default:
		    return FALSE;
	}
}

void system_battery_init()
{
	URL *url = url_parse(SYSTEM_BATTERY_URI, URL_ALL);
	
	SYSTEM_BATTERY = dataobject_construct(url, 1);
	BATTERY_CHARGE_LEVEL = dataobjectfield_uint(1);
	BATTERY_CHARGE_STATE = dataobjectfield_uint(1);
	
	system_battery_set_battery_level(1);
	system_battery_set_charge_state(1);

	pwr_Init(pwrCb);
	//mmeBattInfo(10, 0);
	aci_create((T_PRIM_HANDLER)mmePrimHandler,NULL);

//	BalBattRegister(system_battery_callback);
}

