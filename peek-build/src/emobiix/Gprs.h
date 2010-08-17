#ifndef _GPRS_H_
#define _GPRS_H_

#include "typedefs.h"

#define GPRS_URI "system://local/gprs"

#define GPRS_NOT_PRESENT  (-1)  //not present, last state is not indicated
#define GPRS_NOT_REGISTERED (0)//not registered, no searching
#define GPRS_REGISTERED (1)//registered, home network
#define GPRS_NOT_REG (2)//not registered, but searching
#define GPRS_DENIED (3) //registration denied
#define GPRS_UNKNOWN (4) //unknown
#define GPRS_ROAMING (5) //registered, roaming
#define SIGNAL_FLIGHT_M (5)

#ifdef __cplusplus
extern "C" {
#endif

//void GprsRegisterRssi(void);
void gprs_set_status(BOOL status);
void gprs_set_signal_level(U32 level);
void gprs_set_location(U32 lac, U32 ci);

//static void rssiEventHandler(RegIdT RegId, uint32 MsgId, void* MsgBufferP);

#ifdef __cplusplus
}
#endif

#endif /* _GPRS_H_ */

