#ifndef __BTC_ATA_SIM_H_20080224__
#define __BTC_ATA_SIM_H_20080224__

#ifdef __cplusplus
extern "C" {
#endif


#define SIM_NOT_INSERT            0
#define SIM_NOT_ACTIVE            1
#define SIM_STK_RESET             2 
#define IMEI_NOT_VALID            3
#define SIM_ACTIVE                4

int ata_sim_init(void); 
void ata_sim_exit(void);  
void ata_sim_ok(int);
void ata_sim_error(int, int);


int ata_sim_pin_count(unsigned char pin_id);
unsigned char *ata_sim_imsi(void);
unsigned char *ata_sim_iccid(void);

unsigned char ata_sim_status(void);

#ifdef __cplusplus
}
#endif
#endif //__BTC_ATA_SIM_H_20080224__

