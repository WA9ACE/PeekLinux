
#ifndef _HWD_BATT_H_

#define _HWD_BATT_H_

#include "sysdefs.h"
#include "hwdaccapi.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum 
{
   HWD_BATT_TYPE_UNKNOWN = -1,             /* do NOT change this line. */
   HWD_BATT_TYPE_LI = 0,
   HWD_BATT_TYPE_NH,
   HWD_BATT_TYPE_NUM
} HwdBattTypeT;

typedef enum
{
   HWD_BATT_ID_NONE = -1,                  /* DO NOT change this line. it means no battery exists. */
   HWD_BATT_ID_1 = 0,                      /* placeholder              */
   HWD_BATT_ID_2,                          /* placeholder              */
   HWD_BATT_ID_3,                          /* placeholder              */
   HWD_BATT_ID_4,                          /* placeholder              */
   HWD_BATT_ID_5,                          /* placeholder              */
   HWD_BATT_ID_UNKNOWN,
   HWD_BATT_ID_MAX,
   HWD_BATT_ID_DEFAULT = HWD_BATT_ID_1
} HwdBattIdT;

typedef struct
{
   int16       Temperature;                /* Temperature of Battery  (0.01 Celsius) */
   uint32      Resistance;                 /* Resistance of Thermistor (ohms)*/
} HwdThermInfoT;

#define THERM_T_VS_R_NUM    16

typedef struct 
{
   HwdBattTypeT        BattType;           /* Battery type                */
   uint32              IdMinRes;           /* Minimum resistance of battery ID resistor */
   uint32              IdMaxRes;           /* Maximum resistance of battery ID resistor */
   uint16              Capacity;           /* Battery capacity in mah     */
   uint16              MaxVolt;            /* Battery top cut-off voltage in mV */
                                           /* Li: 4.2V or 4.1V */
                                           /* Ni-MH: 4.5-4.8V  */
   HwdThermInfoT       ThermInfo[THERM_T_VS_R_NUM];        
                                           /* Themistor characteristic. temp vs resistance */
   /* these two members are defined for Ni-MH battery. */                                        
   uint16              DeltaV;             /* mV */
   uint16              TempRisePerMin;     /* UINT: 0.01 Celsius/Min.*/
   /* this member is define for Li Battery to control the charge termination. */
   uint16              TopoffCurrent;      /* in ma uint. */
} HwdBattInfoT;

typedef enum
{
   BATT_CTRL_DELTAV,
   BATT_CTRL_TEMP_RISE,
   BATT_CTRL_TOP_OFF_CURRENT
} BattCtrlIdT;

typedef struct 
{
   uint16      minVolt;                    /* Minimum output voltage of charger in mV */
   uint16      maxVolt;                    /* Maximum output voltage of charger in mV */
} HwdChargerInfoT;

typedef PACKED struct
{
   uint16      Volt;                       /* voltage in mV                           */
   uint16      AdcValue;                   /* ADC value corresponding to the voltage  */   
} HwdAdcVoltT;

typedef PACKED struct
{
   int16       Temp;                       /* Temperature in Celsius       */
   uint16      AdcValue;                   /* ADC value corresponding to the voltage  */   
} HwdAdcTempT;

typedef struct
{
   /* Operations of HwdBattInfoT. */
   HwdBattTypeT (* GetBattType)(HwdBattIdT);
   uint16       (* GetBattMaxVolt)(HwdBattIdT);        /* returned voltage value in mv. */
   uint16       (* GetBattMaxCapacity)(HwdBattIdT);    /* returned value in mah. */
   uint16       (* GetBattMiscInfo)(HwdBattIdT BattId, BattCtrlIdT Id);
   
   /* Battery driver APIs. */
   HwdBattIdT   (* GetBattId)(void);
   
   /* Battery driver will return voltage value in 1mv uint. */
   uint16       (* GetBattVolt)(void);
    
   /* Battery driver will return temperature value in (0.01 Celsius) uint */
   int16        (* GetBattTemp)(void);
    
   /* Charger driver APIs. */
   bool         (* ChargerInit)(HwdAccCommParamT *);
   
   /* get charge current measured in HW */
   uint16       (* GetChargeCurrent)(void);
   
   /* set HW charger current, return the actual value set to HW IC, 
    * because driver may have itself method to rise the charge current */
   uint16       (* SetChargeCurrent)(uint16);
   
   void         (* ChargeEnable)(bool);
   
   /* Charger driver may check the external voltage validity in this function. */
   bool         (* ChargerExist)(void);
} HwdBattDrvT;

#ifdef __cplusplus
}
#endif

#endif

