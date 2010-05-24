



#ifndef BAL_LSM_API_H
#define BAL_LSM_API_H

#include "balapi.h"


 #ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
typedef enum
{
 	LSM_FSI_EVT_SD_MOUNT,
  	LSM_FSI_EVT_SD_UNMOUNT,
  	LSM_FSI_EVT_SD_MOUNT_FAIL,
  	LSM_FSI_EVT_MAX
}LsmFsiEventIdT;


typedef enum
{
	LSM_APP_PHB_INIT_MSG 		 = 1,		/* the val phb init message to lsm app */
	LSM_CREATE_DB_MSG			 = 2,		/* create data for db when sys init db */
	LSM_SD_CARD_INSERTED_MSG 	 = 3,
  	LSM_SD_CARD_REMOVED_MSG   	 = 4,
  	LSM_APP_SMS_INIT_MSG	 	 = 5,		/* the val sms init message to lsm app */
	LSM_APP_MAX_MSG
}LsmAppMsgIdT;


/* val lsm module event */
typedef enum 
{
	BAL_LSM_EVENT_SMS_UPDATE = 0,
    BAL_LSM_EVENT_SMS_DELETE,

	BAL_LSM_EVENT_PHB_UPDATE,
	BAL_LSM_EVENT_PHB_DELETE,
	
	BAL_LSM_EVENT_NUM
}BalLsmEventT;


/* val lsm module id */
typedef enum 
{
    BAL_LSM_SMS = 0,
    BAL_LSM_PHB,
    BAL_LSM_MODULE_NUM
} BalLsmIdT;

/* val lsm module notify data */
typedef struct
{
  BalLsmIdT ModuleId;
  uint16  	DevId;
  uint16 	RecId;
} BalLsmNotifyDataT;

/* val lsm module */
typedef struct
{
	bool 			    Init;
	BalEventHandlerT* 	EventHandlerP;
}BalLsmModuleT;

/* lsm init */
void BalLsmInit(BalLsmIdT ModuleId);

/* lsm register callback for module */
RegIdT BalLsmRegister( BalLsmIdT ModuleId,BalEventFunc EventFunc );

/* lsm unregister callback for module */
void BalLsmUnRegister( BalLsmIdT ModuleId,RegIdT RegId );

/* lsm notify by module event */
bool BalLsmNotify(BalLsmNotifyDataT* LsmNotifyDataP, BalLsmEventT LsmEvent);


RegIdT BalLsmFsiRegister(BalEventFunc EventFuncP);

bool BalLsmGetDosFsStatus(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif



