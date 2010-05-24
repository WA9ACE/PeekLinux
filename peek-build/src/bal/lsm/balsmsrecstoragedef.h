
 #ifndef BAL_SMS_REC_STORAGE_DEF_H
 #define BAL_SMS_REC_STORAGE_DEF_H
 
 #include "sysdefs.h"
 #include "balapi.h"
 #include "balsmsapi.h"
 
 #ifdef __cplusplus
 extern "C"
 {
 #endif 


 /* extern declaration */
 /* see in valsmsrecstorage.c */
 extern BalSmsDevListT BalSmsDevList[BAL_SMS_MAX_REG_DEV_CNT];
 extern uint16  BalSmsListHead;
 extern BalSmsLookUpTableT*   BalSmsLookUpTable;
 extern ExeSemaphoreT  SmsSemaphoreCb;
 extern BalSmsStorageInitStateT gValSmsStorageInitState;

 /* see in valsmscust.c */
 extern BalSmsCustParaT BalSmsCustPara;
 extern BalSmsCustFlashParaT BalSmsCustFlashPara;
 extern BalSmsCustExtraFlashParaT BalSmsCustExtraFlashPara;

 /* these interface are used for val sms storage module */
 /* see in valsmsflash.c */
 uint32 BalSmsFileClose(uint16 *FileHandleP);
 uint32 BalSmsFileDelRec(uint16 FileHandle ,uint16 MaxRecCount,uint16 RecId);
 uint32 BalSmsFileUpdateRec(uint16 FileHandle ,uint16 MaxRecCount,uint16 RecId, void *BufferP,uint32 Length);
 uint32 BalSmsFileReadRec( uint16 FileHandle, uint16 MaxRecCount, uint16 RecId, void *BufferP,uint32* LengthP );
 bool   BalSmsFileOpen(const char* fileName, uint16 *  pFileHandle, uint16 RecCount, uint16 RecSize);

 /* see in valsmsrecstorage.c */
 bool BalSmsInsert2SortList(uint16 *pFirstAddress, uint16 ToAddAddress);
 bool BalSmsDelFromSortList(uint16 *pFirstAddress,uint16 ToDelAddress);
 uint32 BalSmsDevMatch(uint8 DevId,uint8* DevIndexP);
 void BalSmsGetDevRecId(uint16 Address,uint8* DevIdP,uint16* RecIdP);
 
 bool BalSmsGetMemFullStatus(BalDeviceT DevId);
 void BalSmsSendMemIndication(BalDeviceT DeviceType,BalSmsMemFullIndicationT Indication);

 /* see in valsmscust.c*/
 void BalSmsCustInitPara(void);
 void BalSmsCustInitFlashPara(void);
 void BalSmsCustInitExtraFlashPara(void);
 BalSmsDevOptT* BalSmsCustGetDevOpt(uint8 DevId);
 BalDeviceT BalSmsDevReMapping(uint8 DevId);
 uint32 BalSmsDevMapping(BalDeviceT DevType,BalSmsDevMappingIdT* DevMappingIdP);

 #ifdef __cplusplus
 }
 #endif /* __cplusplus */
 #endif
