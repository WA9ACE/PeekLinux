#ifndef __BTC_ATA_API_H_20080112__
#define __BTC_ATA_API_H_20080112__

#ifdef __cplusplus
extern "C" {
#endif

#include "ata_datatype.h"
#include "ata_cmd.h"

typedef void (*ata_resp_cb_t)(T_ATA_Response_Type type, T_ATA_CMD cmd, T_ATA_RSP value);

extern int ata_init(unsigned char client, ata_resp_cb_t cb);
extern int ata_dinit(unsigned char client);
extern T_ATA_return ata_send_at (unsigned char client, const char *atstr);

#ifdef __cplusplus
}
#endif
#endif //__BTC_ATA_API_H_20080112__
