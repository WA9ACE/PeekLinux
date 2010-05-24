#if 1
#ifndef _BAL_BTTEST_H_
#define _BAL_BTTEST_H_
#include "sysdefs.h"
#include "balsndapi.h"
#define  BAL_BT_TEST 

void BalCSRBTInit(char *name, char* addr);
void BalCSRBTInquiry(uint8 devNum);
void BalCSRBTPairing(char *devAddr, char* pin);
void BalCSRTrustDevRegister(char *addr, char *linkKey);
void BalCSRBTConnect(char *devAddr);
void BalCSRBTDisconnect(char *devAddr);
void BalCSRBTCtrl( bool);
void BalCSRVisibleSet(bool);
void BalCSRBTIdleRequest(void);
void BalCSRBTReset(void);
void BalCSRVolumeSet(BalSoundVolumeModesT VolMode, BalSoundVolumeT Volume);
void BalCSRBTIncomingCall(void);
#endif
#endif
