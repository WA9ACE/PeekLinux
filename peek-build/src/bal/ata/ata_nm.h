#ifndef __BTC_ATA_NM_H_20080224__
#define __BTC_ATA_NM_H_20080224__

#ifdef __cplusplus
extern "C" {
#endif

/* definition for services */
#define ATA_NO_SERVICE        0
#define ATA_FULL_SERVICE      1      
#define ATA_LIMITED_SERVICE   2
#define ATA_SEARCH_NETWORK    3
#define ATA_SEARCH_DENIED     4
#define ATA_ROAM_SERVICE	5

/* definition for GPRS reg status */
#define ATA_CGREG_STAT_NOT_PRESENT  -1    /* not present, last state is not indicated */
#define ATA_CGREG_STAT_NOT_REG      0     /* not registered, no searching */
#define ATA_CGREG_STAT_REG_HOME     1     /* registered, home network */
#define ATA_CGREG_STAT_SEARCHING    2     /* not registered, but searching */
#define ATA_CGREG_STAT_REG_DEN      3     /* registration denied */
#define ATA_CGREG_STAT_UNKN         4     /* unknown */
#define ATA_CGREG_STAT_REG_ROAM     5     /* registered, roaming */

/* definition for gprs services */
#define    ATA_GPRS_Not_Supported            0
#define    ATA_GPRS_Supported_Limited_Serv   1
#define    ATA_GPRS_Supported                2
#define    ATA_GPRS_Support_Unknown          3

#define LONG_NAME             26
#define SHORT_NAME            10

typedef struct /* data for timer zone */
{
  unsigned char year;
  unsigned char month;
  unsigned char day;
  unsigned char hour;
  unsigned char minute;
  unsigned char second;
  unsigned char timezone;
} T_ATA_TIME_IND;

typedef enum
{
    ATA_NM_OPER_LONG,
    ATA_NM_OPER_SHORT
} ATA_OPER_TYPE;

int ata_nm_init(void); 
void ata_nm_exit(void);

void ata_nm_get_oper_name(char **, ATA_OPER_TYPE);

int ata_nm_get_service_status(void);

int ata_MCA_update_timezone();

#ifdef __cplusplus
}
#endif
#endif //__BTC_ATA_NM_H_20080224__

