
#ifndef BAL_SCHEDULER_API_H
#define BAL_SCHEDULER_API_H

#include "sysdefs.h"
#include "balfsicfg.h"
#include "balschedulercust.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BAL_SCHEDULER_MAX_REGISTERED_IDS 5

typedef struct
{
  uint8 year;  		// 00 ¡« 99
  uint8 month;      // 01 ¡« 12
  uint8 day;          /* 01 ¡« 31       £¨1£¬ 3£¬ 5£¬ 7£¬ 8£¬ 10£¬ 12£©
       		           01 ¡«30        £¨4£¬ 6£¬ 9£¬ 11£©
                                28                 £¨2 when common year£©
                                29                 £¨2 when leap year£©*/
  uint8 hour;         // 00 ¡« 23
  uint8 minute;     // 00 ¡« 59
  uint8 second;     // 00 ¡« 59

  uint8   wday;        /* days in a week               - [0,6] */
  uint8   mode_12_hour; /* TRUE->12 hour mode ; FALSE-> 24 hour mode */
  uint8   PM_flag;      /* if 12 hour flag = TRUE TRUE->PM ; FALSE->AM */
}
BalTimeTableT;

typedef struct
{
  BalTimeTableT alarmTime;
  uint32 ringerID;
}
BalSchedulerClockT;

typedef PACKED struct
{
  uint8 CodePage;
  uint16 StringLen;
  uint8 String[BAL_MAX_SCHEDULER_CONTENT_LEN];/*SIDB_STRING_MAX_LENGTH*/
}
BalSidbStringT;

typedef enum
{
    SCHEDULER_FREE = 0x00 ,
    SCHEDULER_VALID= 0x01,
    SCHEDULER_EXPIRED
}BalSchedulerStateT;


/*structure for store in flash;*/
typedef struct
{
  BalSchedulerStateT	state;
  BalSchedulerClockT alarmPara;
  BalSidbStringT sidbstring;
}
BalSchedulerFlashRecT;

typedef struct
{
    BalSchedulerStateT  State;
	uint32 Seconds;
	uint16 NextId;
}BalSchedulerListT;

typedef enum
{
    SCHEDULER_SUCESS,
    SCHEDULER_ERROR
    
}
BalSchedulerResultT;

/* Bal scheduler error */
typedef enum
{ 
	BAL_SCHEDULER_ERR_NONE 		= 0,	/* Operating success */
    BAL_SCHEDULER_ERR_PARMETER 	= 1,    /* Inputting invalid parameter */
	BAL_SCHEDULER_ERR_NOT_INIT,			/* val scheduler subsystem does not init(BalSchedulerInit has not been called) */
	BAL_SCHEDULER_ERR_REINIT,				/* val scheduler subsystem has been initialized,and it needn't init again */

	BAL_SCHEDULER_ERR_NO_SPACE,			/* there is no space to add a record */

	BAL_SCHEDULER_ERR_DEV_NOT_OPEN,		/* scheduler device not open */
	BAL_SCHEDULER_ERR_DEV_OPEN,			/* error when open scheduler device open */
	BAL_SCHEDULER_ERR_DEV_CLOSE,			/* error when close scheduler device failure */

	BAL_SCHEDULER_ERR_REC_READ,			/* read record  of scheduler device failure */
	BAL_SCHEDULER_ERR_REC_WRITE,			/* write record of scheduler device failure */
	BAL_SCHEDULER_ERR_REC_DELETE,			/* delete record of scheduler device failure */
	BAL_SCHEDULER_ERR_REC_EMPTY,			/* empty record of scheduler device */
	
	BAL_SCHEDULER_ERR_MAX

}BalSchedulerErrorT;

typedef enum
{
	BAL_SCHEDULER_UI,
	BAL_SCHEDULER_PST
}
BalSchedulerTypeT;

typedef enum
{
  BAL_Scheduler_EVENT_WRITE     = 0x00000001,
  BAL_Scheduler_EVENT_DELETE,  
  BAL_Scheduler_MAX_EVENTS
} BalSchedulerEventIdT;

/* SMS callback function structure */
typedef void (*BalSchedulerEventFunc) ( void);

/* SMS Event register table structure */
typedef struct
{
  bool            IsUse;            /* if this entry is in use */
  BalSchedulerTypeT           UIType;         /*PST or UI*/
  BalSchedulerEventFunc CallBack;         /* function to call back */
} BalSchedulerRegTableT;

typedef struct
{
	uint16 iMaxRecordCount;	/* max count of record */
	uint8 iMaxContentLength;	/* max length of name */
	uint16 iMaxRecordLength;/* max length of a record */
	void* pRecordSummaryAddress; /* the address of record summary info buffer */
    void*	pRecordTempAddress; /* the address of record temp buffer for read*/
}BalSchedulerCustParamT;

/******************************************************************************
FUNCTION 

DESCRIPTION
  

Parameters:  	

Returns:   true is success, and false is error  

*******************************************************************************/
bool    BalSchedulerInit(void);
bool    BalSchedulerInitVar(void);
void    BalSchedulerCustInit(void);
	
BalSchedulerResultT BalSchedulerRead(uint16 LogRecId, BalSchedulerFlashRecT * ReadBuf);

BalSchedulerResultT BalSchedulerDelete(uint16 LogRecId);
BalSchedulerResultT BalSchedulerDeleteAll(void);

BalSchedulerResultT BalSchedulerWrite(uint16  * LogRecId, BalSchedulerFlashRecT * WriteBuf);
uint16 BalSchedulerRegister( BalSchedulerEventFunc CallBack );
void BalSchedulerUnregister( uint16 SchedulerRegId );

uint16 BalSchedulerGetRecNums(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif




