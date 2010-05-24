#ifndef _HWDPMU_H_
#define _HWDPMU_H_

#define LDO_OFF FALSE
#define LDO_ON  TRUE

/*All PMU LDO ID must based on 0!*/
typedef enum
{
    HWD_PMU_LDO_OUT1=0,
    HWD_PMU_LDO_OUT2,
    HWD_PMU_LDO_OUT3,
    HWD_PMU_LDO_OUT4,
    HWD_PMU_LDO_OUT5,
    HWD_PMU_LDO_OUT6,
    HWD_PMU_LDO_OD1,
    HWD_PMU_LDO_OD2,
    HWD_PMU_LDO_MAX
}HwdPmuLdoIdT; 


/************************************************************************************
*   The following PMU interfaces are for RF Driver                                  *
*************************************************************************************/
void HwdPmuInitRfLdos(void);
void HwdPmuEnableRfLdos(bool Enable);      /*Double control RF PA type*/
void HwdPmuEnableRfRxLdo(bool Enable);   /*Single control RF PA type*/
void HwdPmuEnableRfTxLdo(bool Enable);   /*Single control RF PA type*/


/************************************************************************************
*   The following PMU interfaces are for battery driver                             *
************************************************************************************/
void HwdPmuEnableCharge(bool Enable);
uint16 HwdPmuSetChargeCurrent(uint16 Current);
bool HwdPmuIsEndOfCharge(void);

/************************************************************************************
*   The following PMU interface are for Phone system                                *
*************************************************************************************/
void HwdPmuInit(void);
void HwdPmuPwrOffBeforeDeepSleep(void);
void HwdPmuPwrOnAfterDeepSleep(void);
void HwdPmuEnableLdo(HwdPmuLdoIdT LdoId, bool Enable);
void HwdPmuPhonePwrOff(void);

#endif /* _HWDPMU_H_ */

