

#ifndef BAL_SMS_CUST_H
#define BAL_SMS_CUST_H


#ifdef __cplusplus
extern "C"
{
#endif 

 /* max record count for sms in flash db file */
 #define BAL_SMS_MAX_FLASH_REC_CNT 60


 /* the voice mail storage device */
 #define BAL_SMS_VMN_STORAGE_DEV BAL_SMS_STORAGE_DEV_FLASH

 /* sms extra storage dev type */
 #define BAL_SMS_EXTRA_STORAGE_DEV_TYPE BAL_DEV_FLASH

 /* sms extra storage dev id */
 #define BAL_SMS_EXTRA_STORAGE_DEV_ID 	BAL_SMS_STORAGE_DEV_EXTRA_FLASH

 
  #ifdef SYS_OPTION_EMS
 /* the sms data segments count */
 #define BAL_SMS_MAX_FLASH_DATA_REC_CNT 401

 /* the sms data segment size */
 #define BAL_SMS_FLASH_DATA_REC_SIZE	sizeof(BalSmsRecordT)

 /* the voice mail storage device */
 #define BAL_SMS_VMN_STORAGE_DEV BAL_SMS_STORAGE_DEV_FLASH
 
 /*the voice mail position in flash data db file ,the last one*/
 #define BAL_SMS_VMN_ADDRESS	BAL_SMS_MAX_FLASH_DATA_REC_CNT - 1

 /* for sms flash db file,not include one voice mail */
 #define BAL_SMS_MAX_FLASH_FILE_REC_CNT   BAL_SMS_MAX_FLASH_REC_CNT

 /* sms flash record size */
 #define BAL_SMS_FLASH_REC_SIZE 	sizeof(BalSmsLookUpRecT)
 
 #else
 /* the sms data segments count */
 #define BAL_SMS_MAX_FLASH_DATA_REC_CNT  0

 /* sms flash record size */
 #define BAL_SMS_FLASH_DATA_REC_SIZE 	0

 /*the voice mail position in flash db file ,the last one*/
 #define BAL_SMS_VMN_ADDRESS	BAL_SMS_MAX_FLASH_REC_CNT

  /* for sms flash db file,include one voice mail */
 #define BAL_SMS_MAX_FLASH_FILE_REC_CNT   BAL_SMS_MAX_FLASH_REC_CNT + 1

  /* sms flash record size */
 #define BAL_SMS_FLASH_REC_SIZE 	sizeof(BalSmsRecordT)
 #endif

 
 
 #ifdef SYS_OPTION_RUIM
 #define BAL_SMS_MAX_UIM_REC_CNT 40
 #define BAL_SMS_MAX_UIM_REC_SIZE   sizeof(BalSmsRecordT)
 #else
 #define BAL_SMS_MAX_UIM_REC_CNT 0
 #endif

 #define BAL_SMS_MAX_REC_CNT  (BAL_SMS_MAX_UIM_REC_CNT + BAL_SMS_MAX_FLASH_REC_CNT)
 
 #define BAL_SMS_MAX_RAM_REC_CNT 10
 
 #define BAL_SMS_EXTRA_REC_SIZE 200  /* bytes of each extra info record*/

 /* extra recor for all record in flash( sms + one voic mail ) */
 #define BAL_SMS_EXTRA_REC_CNT  BAL_SMS_MAX_FLASH_REC_CNT


 /* val sms storage device */
typedef enum
{
  /*BAL_SMS_STORAGE_DEV_NONE        		= 0x00,*/
  BAL_SMS_STORAGE_DEV_UIM         		= 0x01,
  BAL_SMS_STORAGE_DEV_RAM         		= 0x02,
  BAL_SMS_STORAGE_DEV_FLASH   			= 0x03,
  BAL_SMS_STORAGE_DEV_EXTRA_FLASH   	= 0x04,
  BAL_SMS_STORAGE_DEV_MAX
} BalSmsStorageDeviceT;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


