/*
Create by zhangfanghui ,to add imei interface.
*/
#ifndef _BALIMEIAPI_H_
#define _BALIMEIAPI_H_
#include "sysdefs.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define IMEI_DIE_ID_REG_LSB    (0xFFFE0000 | 0xF004)          // Locosto: die id bit  63 to 0
#define IMEI_DIE_ID_REG_MSB    (0xFFFE0000 | 0xF040)          // Locosto: die id bit 127 to 64
#define IMEI_DIE_ID_SIZE     4
#define IMEI_LEN 15
typedef struct
{
    unsigned short size; // size of data[]
    int index;           // index into data[]
    unsigned char  data[256];
} T_IMEI_PKT;




//void BalGetImei(char *ImeiBuffer);
void BalGetImei(char ImeiBuffer[IMEI_LEN]);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* #ifndef _BALIMEIAPI_H_ */
