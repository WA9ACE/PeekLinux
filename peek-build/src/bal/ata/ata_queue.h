#ifndef __BTC_ATA_QUEUE_H_20080112__
#define __BTC_ATA_QUEUE_H_20080112__

#ifdef __cplusplus
extern "C" {
#endif

#include "ata_cfg.h"
#include "ata_cmd.h"

typedef struct 
{
	char cmdbuffer[ATA_AT_CMD_MAX_LEN];
	unsigned char size;
}T_ata_cmd_send; 

typedef struct ata_queue_entry_s
{
  struct ata_queue_entry_s      *next;
  T_ata_cmd_send                cmd; 
  unsigned char                 client_id;
} T_ata_queue_entry;

typedef struct
{
  T_ata_queue_entry *first;
  T_ata_queue_entry *last;
} T_ata_queue;

extern T_ata_queue* ata_queue_create (void);
extern void         ata_queue_put (T_ata_queue *que, T_ata_cmd_send *cmd, unsigned char client_id, int dataSize);
extern void         ata_queue_put_head (T_ata_queue *que, 
									    T_ata_cmd_send *cmd, 
									    unsigned char client_id, 
									    int dataSize);
extern int          ata_queue_get (T_ata_queue *que, T_ata_cmd_send *cmd, unsigned char *client_id);
extern void         ata_queue_remove (T_ata_queue *que);

#ifdef __cplusplus
}
#endif
#endif /* __BTC_ATA_QUEUE_H_20080112__ */

