#ifndef _PEEKEMOBIIX_H_
#define _PEEKEMOBIIX_H_

#ifdef __cplusplus
extern "C" {
#endif

/* 0 (unknown), 1 - 5 */
void peek_setSignalLevel(int);

/* true or false */
void peek_setGRPSStatus(int);

/* 0 (unknown), 1 - 5 */
void peek_setBatteryLevel(int);

/* true or false */
void peek_setCharging(int);

#ifdef __cplusplus
}
#endif

#endif

