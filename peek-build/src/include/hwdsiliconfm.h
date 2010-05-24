#include "custbtc.h"

#ifdef DENALI_FM_TEMP
#ifndef SUPPORT_FM
#define SUPPORT_FM
#endif
#endif /*DENALI_FM_TEMP*/

 #ifdef SUPPORT_FM
 
 #ifndef _HWD_SILICONFM_API_H_
 #define _HWD_SILICONFM_API_H_
 
 #ifdef __cplusplus
 extern "C"
 {
 #endif /* __cplusplus */
 typedef enum
 {
 	FM_SPACE_20,
 	FM_SPACE_10,
 	FM_SPACE_5
 }FmChSpT;

 extern bool FmIsPlay();

 extern void slipFmCspSet( FmChSpT chSp );
 extern void slipFmVolSet ( uint8 volume, uint8 dmute);
 extern uint16 slipFmVolGet (void);
 
 extern void slipFmBbSet(uint16 min, uint16 max);
 extern uint8 slipFmseek (uint8 seekup);
 
 extern uint8 slipFmRssiGet(void);
 extern void slipFmRssiThrSet(uint8 threshold);
 extern void slipFmMonoSet(bool mono);
 extern uint8 slipFmStereoGet(void);
 extern void slipFmInit (void);
 extern void slipFmPlay(void);
 extern void slipFmStop(void);
 extern uint16 slipFmFrqGet(void);
 extern bool  slipFmFrqSet(uint16 frequency);/* TRUE: seek failure*/
 
 #ifdef __cplusplus
 }
 #endif /* __cplusplus */
 #endif
 
 #endif
 
