


#ifndef BAL_PHB_API_H
#define BAL_PHB_API_H

#ifdef __cplusplus
extern "C"
{
#endif 

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sysdefs.h"
#include "balapi.h"


/*===========================================================================

                      MARCO DEFINES

===========================================================================*/
/* Field struct data length defined */
#define PHB_FIELD_ID_LEN 		FIELD_SIZE(BalPhbFieldT,FieldId)
#define PHB_FIELD_TYPE_LEN 		FIELD_SIZE(BalPhbFieldT,FieldType)
#define PHB_FIELD_DATALEN_LEN 	FIELD_SIZE(BalPhbFieldT,DataLength)
#define PHB_FIELD_HEADER_LEN 	FIELD_OFFSET(BalPhbFieldT,Data)

/*===========================================================================

                      TYPE DEFINES

===========================================================================*/
/* BAL phb filed type */
typedef PACKED struct
{
	uint8  FieldId;
	uint8  FieldType;
	uint16 DataLength;
	void * Data;
}BalPhbFieldT;

/* Bal phb search index field parameter*/
typedef struct
{
	uint16 FieldLength;
	uint16 FieldLength1;
	void*  FieldData;
	void*  FieldData1;
}BalPhbIndexFieldT;

/* Bal phb search mode */
typedef enum
{
	BAL_PHB_SEARCH_BY_FULL_NAME = 0x00,		/* search by name full matched */
	BAL_PHB_SEARCH_BY_PART_NAME,    		/* search by name part matched */
  BAL_PHB_SEARCH_BY_FULL_NAME_EXCLUDE_GROUP, /* search by name full matched and exclude group */
  BAL_PHB_SEARCH_BY_PART_NAME_EXCLUDE_GROUP, /* search by name part matched and exclude group */
	BAL_PHB_SEARCH_BY_PART_NAME_AND_DEVICE,    		/* search by name part matched and device */
	BAL_PHB_SEARCH_BY_PART_NAME_AND_NUMBER,    		/* search by name part matched and device */
  BAL_PHB_SEARCH_BY_PART_NAME_AND_EMAIL,
  BAL_PHB_SEARCH_BY_PART_NAME_AND_NUMBER_OR_EMAIL,
  BAL_PHB_SEARCH_BY_PART_NAME_AND_POSTAL,
  BAL_PHB_SEARCH_BY_FULL_NAME_AND_RECID,
  BAL_PHB_SEARCH_BY_PART_NAME_AND_RECID,
	BAL_PHB_SEARCH_BY_PHONE_NUMBER = 0x10,	/* search by phone number */
	BAL_PHB_SEARCH_BY_PART_PHONE_NUMBER,	/* search by part phone number */
	BAL_PHB_SEARCH_BY_GROUP_ID = 0x20,		/* search by group id */
	BAL_PHB_SEARCH_BY_DEVICE_ID = 0x30,		/* search by device id */
  BAL_PHB_SEARCH_BY_LIST_NAMES = 0x40,
  BAL_PHB_SEARCH_BY_LIST_GROUP,
  BAL_PHB_SEARCH_BY_LIST_ALL,
  BAL_PHB_SEARCH_BY_LANGUAGE_AND_DEVICE = 0x50, /* search by the language script */
  BAL_PHB_SEARCH_BY_TEXT_TYPE_NAMES_ONLY = 0x60, /* search by text type */
  BAL_PHB_SEARCH_BY_TEXT_TYPE_NUMBERS_ONLY,
  BAL_PHB_SEARCH_BY_TEXT_TYPE_EMAILS_ONLY,
  BAL_PHB_SEARCH_BY_TEXT_TYPE_NUMBERS_OR_EMAILS,
  BAL_PHB_SEARCH_BY_TEXT_TYPE_POSTAL_ADDRESS_ONLY,
  BAL_PHB_SEARCH_BY_TEXT_TYPE_RING,
  BAL_PHB_SEARCH_BY_TEXT_TYPE_PICTURE,
  BAL_PHB_SEARCH_BY_MAX
}BalPhbSearchModeT;

/* Bal phb search parameter */
typedef struct
{
	BalPhbIndexFieldT FieldPara; 		/* searching data */
	uint16	StartPosition; 				/* search start position */
	BalPhbSearchModeT   SearchMode; 	/* searching mode */
}BalPhbSearchParaT;

/* Bal phb error */
typedef enum
{ 
	BAL_PHB_ERR_NONE 		= 0,	/* Operating success */
    BAL_PHB_ERR_PARMETER 	= 1,    /* Inputting invalid parameter */
	BAL_PHB_ERR_NOT_PARA_INIT,			/* val phb parameters not init(BalPhbInit has not been called) */

	BAL_PHB_ERR_DEV_UNSUPPORT,		/* unsupport phb device,error device id */
	BAL_PHB_ERR_DEV_REG,			/* phb device register error, register device is full */
	BAL_PHB_ERR_DEV_ALREADY_REG,	/* phb device already register */
	BAL_PHB_ERR_DEV_NOT_REG,		/* phb device not register */
	BAL_PHB_ERR_DEV_NOT_OPEN,		/* phb device not open */
	BAL_PHB_ERR_DEV_ALREADY_OPEN,	/* phb device already open */
	BAL_PHB_ERR_DEV_OPEN,			/* error when open phb device open */
	BAL_PHB_ERR_DEV_CLOSE,			/* error when close phb device failure */
	BAL_PHB_ERR_DEV_INIT_BUSY,		/* phb device initial busy */

	BAL_PHB_ERR_REC_EMPTY,			/* empty record of phb device */
	BAL_PHB_ERR_REC_READ,			/* read record  of phb device failure */
	BAL_PHB_ERR_REC_WRITE,			/* write record of phb device failure */
	BAL_PHB_ERR_REC_DELETE,			/* delete record of phb device failure */
	
	BAL_PHB_ERR_NO_BAL_MEMORY,		/* val malloc failure */
	BAL_PHB_ERR_NO_MEMORY,			/* no memory for new phb record */
	BAL_PHB_ERR_NO_ENOUGH_MEMORY,	/* no enough memory for new phb record */
	BAL_PHB_ERR_NO_BUFFER,			/* no record buffer for field operating */
	
	BAL_PHB_ERR_MAX_REC_SIZE,		/* no more space of the record for this field */
	BAL_PHB_ERR_FIELD_NONEXISTENCE  /* no such phb field */

}BalPhbErrorT;

/* Bal phb device count */
typedef struct
{
  	uint16 PhbDevMaxRecCount;
  	uint16 PhbDevFreeCount;
} BalPhbCountT;

/* Bal phb field type */
typedef enum
{
	BAL_PHB_FIELD_TYPE_NAME_INTER 	= 0x00,
	BAL_PHB_FIELD_TYPE_NUMBER_INTER	= 0x01,
	BAL_PHB_FIELD_TYPE_GROUP_INTER	= 0x03,	

  BAL_PHB_FIELD_TYPE_RINGER_INTER = 0x05,
  BAL_PHB_FIELD_TYPE_PICTURE_INTER = 0x06,

  BAL_PHB_FIELD_TYPE_EMAIL_INTER = 0x08,

  BAL_PHB_FIELD_TYPE_TEXT_INTER = 0x20,
  BAL_PHB_FIELD_TYPE_TEXT_TYPE_INTER,
	BAL_PHB_FIELD_TYPE_MAX_INTER
}BalPhbFieldTypeInterT;


/* init state */
typedef enum
{
	BAL_PHB_INIT_INVALID = 0x00, 		/* the phb dev initial invalid */
	BAL_PHB_INIT_VALID,					/* the phb dev initial valid */
	BAL_PHB_INIT_NOT_COMPLETE,			/* the phb init not complete */
	BAL_PHB_INIT_COMPLETE				/* the phb init complete */
}BalPhbInitStateT;

/* val phb device init msg */
typedef PACKED  struct 
{
    BalPhbInitStateT PhbInitState;
	
} BalPhbInitStateMsgT;


/* val phb device init call back func */
typedef void	 (*BalPhbDevInitCallBack)(void);

/* val phb device opt func */
typedef uint32	 (*BalPhbDevInit)(BalPhbDevInitCallBack CallBackP);
typedef bool	 (*BalPhbDevIsReady)(void);
typedef uint32  (*BalPhbDevOpen)(BalPhbCountT*  BalPhbCountP);
typedef uint32  (*BalPhbDevReadRecord)(uint16 RecId, void * Buf, uint32* LengthP);
typedef uint32  (*BalPhbDevUpdateRecord)(uint16 RecId,void * Buf, uint32 Length);
typedef uint32  (*BalPhbDevDeleteRecord)(uint16 RecId);
typedef uint32	 (*BalPhbDevClose)(void);

typedef struct
{
	BalPhbDevInit		  PhbInit;
	BalPhbDevIsReady	  PhbIsReady;
	BalPhbDevOpen         PhbOpen;
	BalPhbDevReadRecord	  PhbReadRecord;
	BalPhbDevUpdateRecord PhbUpdateRecord;
	BalPhbDevDeleteRecord PhbDeleteRecord;
	BalPhbDevClose		  PhbClose;
}BalPhbDevOptT;

/* Bal phb compare function */
typedef int8 (*BalPhbSortCompare)	(const BalPhbIndexFieldT* FirstFieldP,const BalPhbIndexFieldT* SecondFieldP);
typedef int8 (*BalPhbSearchCompare)(const BalPhbIndexFieldT* FirstFieldP,const BalPhbIndexFieldT* SecondFieldP,BalPhbSearchModeT SearchMode);

/* Bal phb name sort compare structure */
typedef struct
{
	BalPhbSortCompare PhbNameSortCompare;
}BalPhbSortCompareT;

/* Bal phb search compare structure */
typedef struct
{
  BalPhbSearchCompare PhbNameSearchCompare;
  BalPhbSearchCompare PhbNumberSearchCompare;
  BalPhbSearchCompare PhbGroupSearchCompare;
  BalPhbSearchCompare PhbDeviceSearchCompare;
  BalPhbSearchCompare PhbListSearchCompare;
  BalPhbSearchCompare PhbLanguageSearchCompare;
  BalPhbSearchCompare PhbTextSearchCompare;
}BalPhbSearchCompareT;


typedef struct
{
	uint8 	BalPhbAlphaLength;	 		/* max name length */
	uint8  	BalPhbPhoneLength;   		/* max string type phone number length */
	uint8   BalPhbMaxDevCount;			/* max phb storage device count */
	uint16  BalPhbMaxRecSize;  			/* max record size */
	uint16  BalPhbMaxRecCount;     		/* max record count */
	uint16  BalPhbMaxRecBufCount;  		/* max record buffer count */
	uint16  BalPhbMaxPhoneNumberCnt;   	/* max count of phone number in phone */
	uint16  BalPhbMaxFieldType;			/* max record field type */
	uint16  BalPhbMaxFieldId;			/* max record field id */
	
	uint16	BalPhbRecBufSize;			/* size of BalPhbRecordBufferCustT */
	uint16  BalPhbNameSumSize;			/* size of BalPhbNameSummaryCustT */
	uint16  BalPhbNumSumSize;			/* size of BalPhbNumberSummaryCustT*/

	void*  BalPhbRecBufAddress;			/* record buffer address */
	void*  BalPhbNameSumAddress;		/* name summary address */
	void*  BalPhbNumSumAddress;			/* number summary address */
	
}BalPhbCustParaT;

/*===========================================================================

                       FUNCTION DEFINITIONS 

===========================================================================*/

/* called by lsm, init phb storage */
void BalPhbStorageInit(ExeRspMsgT* InitRspInfoP);

/* called by lsm, init phb storage call back */
void BalPhbStorageInitCallBack(void);


/*****************************************************************************

FUNCTION NAME: BalPhbGetFreeRedId

DESCRIPTION:
        Get a Free Record Id from storage device.
        
PARAMETERS:
         DeciceId[in]: device Id
         FreeRecordIdP[out]: free record Id 
        
RETURNED VALUES:
        BAL_PHB_ERR_DEV_UNSUPPORT:  error input device id
        BAL_PHB_ERR_PARMETER:		error input parameters
        BAL_PHB_ERR_NO_MEMORY:		no memory for new record,the free record 
        							count is 0
        BAL_PHB_ERR_NONE:			success
        
*****************************************************************************/

uint32 BalPhbGetFreeRecId(uint8 DeviceId,  uint16 *FreeRecordIdP);


/*****************************************************************************

FUNCTION NAME: BalPhbSetField

DESCRIPTION:
		Setting fields data to cache buffer.if the field length is 0,so means 
		to clear this field.
        
PARAMETERS:
      	DeviceId[in]:  device id of the contact record
      	RecordId[in]:  record id of the contact record
      	Field[in]:      field data to be set

RETURNED VALUES:
        BAL_PHB_ERR_DEV_UNSUPPORT:  	error input device id
        BAL_PHB_ERR_PARMETER:			error input parameters
        BAL_PHB_ERR_NO_BUFFER:			no buffer for operating record,need flush
        BAL_PHB_ERR_MAX_REC_SIZE:		no more space of the record for this field
        BAL_PHB_ERR_FIELD_NONEXISTENCE: no such phb field
        BAL_PHB_ERR_NONE:				success
        (other error code ,see in valphbapi.h)
        
*****************************************************************************/

uint32 BalPhbSetField(uint8 DeviceId, uint16 RecordId,BalPhbFieldT Field);


/*****************************************************************************

FUNCTION NAME: BalPhbGetField

DESCRIPTION:
		Getting field data form cache buffer.
        
PARAMETERS:
      	DeviceId[in]:  device id of the contact record
      	RecordId[in]:  record id of the contact record
      	FieldP[in][out]:    field data to be sets

RETURNED VALUES:
        BAL_PHB_ERR_DEV_UNSUPPORT:  	error input device id
        BAL_PHB_ERR_PARMETER:			error input parameters
        BAL_PHB_ERR_NO_BUFFER:			no buffer for operating record,need flush
        BAL_PHB_ERR_FIELD_NONEXISTENCE: no such phb field
        BAL_PHB_ERR_NONE:				success    
        (other error code ,see in valphbapi.h)
        
*****************************************************************************/

uint32 BalPhbGetField(uint8 DeviceId, uint16 RecordId,BalPhbFieldT *FieldP);



/*****************************************************************************

FUNCTION NAME: BalPhbClearField

DESCRIPTION:
		Clearing a  contact's field in ram buffer.
        
PARAMETERS:
      	DeviceId[in]:  device id of the contact record
        RecordId[in]:  record id of the contact record
        FieldId[in]:   field id of the field to be cleared

RETURNED VALUES:
        BAL_PHB_ERR_DEV_UNSUPPORT:  	error input device id
        BAL_PHB_ERR_PARMETER:			error input parameters
        BAL_PHB_ERR_NO_BUFFER:			no buffer for operating record,need flush
        BAL_PHB_ERR_FIELD_NONEXISTENCE: no such phb field
        BAL_PHB_ERR_NONE:				success
        (other error code ,see in valphbapi.h)
        
*****************************************************************************/

uint32 BalPhbClearField(uint8 DeviceId, uint16 RecordId,uint8 FieldId);




/*****************************************************************************

FUNCTION NAME: BalPhbFlushRecord

DESCRIPTION:
		Flushing  a  record, this is mean-writing a record to flash.
        
PARAMETERS:
        DeviceId[in]:  device Id , defined by BalPhbDeviceIdT
        RecordId[in]:  record Id

RETURNED VALUES:
        BAL_PHB_ERR_DEV_UNSUPPORT:  	error input device id
        BAL_PHB_ERR_PARMETER:			error input parameters
        BAL_PHB_ERR_NONE:				success
        (other error code ,see in valphbapi.h)
        
*****************************************************************************/

uint32 BalPhbFlushRecord(uint8 DeviceId, uint16 RecordId);


/*****************************************************************************

FUNCTION NAME: BalPhbReleaseRecord

DESCRIPTION:
		Release the record which is stored in the val buffer
        
PARAMETERS:
        DeviceId[in]:  device Id , defined by BalPhbDeviceIdT
        RecordId[in]:  record Id

RETURNED VALUES:
        BAL_PHB_ERR_DEV_UNSUPPORT:  	error input device id
        BAL_PHB_ERR_PARMETER:			error input parameters,
        								the record is not in buffer
        BAL_PHB_ERR_NONE:				success
        
*****************************************************************************/

uint32 BalPhbReleaseRecord(uint8 DeviceId, uint16 RecordId);


/*****************************************************************************

FUNCTION NAME: BalPhbDeleteRecord

DESCRIPTION:
		Deleting record.
        
PARAMETERS:
        DeciceId[in]: device Id
        RecordId[in]: record Id

RETURNED VALUES:
        BAL_PHB_ERR_DEV_UNSUPPORT:  	error input device id
        BAL_PHB_ERR_PARMETER:			error input parameters
        BAL_PHB_ERR_NONE:				success
        (other error code ,see in valphbapi.h)
        
*****************************************************************************/

uint32 BalPhbDeleteRecord(uint8 DeviceId, uint16 RecordId);



/*****************************************************************************

FUNCTION NAME: BalPhbSearch

DESCRIPTION:
		Search the record by BalPhbSearchParaT
        
PARAMETERS:
		BalPhbSearchParaP [in][out]: Bal phone book search parameter
		ResultDevId [out]:	         result device id
	    ResultRecId [out]:	         result record id
	    ResultFieldIdP[out]:		 result field id  (only for phone number search)


RETURNED VALUES:
        TRUE: 	success,matched
        FALSE:  failed,not matched
        
*****************************************************************************/

bool BalPhbSearch (BalPhbSearchParaT* BalPhbSearchParaP, uint8* ResultDevIdP, uint16* ResultRecIdP, uint8* ResultFieldIdP);


/*****************************************************************************

FUNCTION NAME: BalPhbGetCount

DESCRIPTION:
		Getting Phone book number of record
        
PARAMETERS:
		DeviceId [in]: 		Device id of the storage device
        PhbRecCountP [out]: Bal phb memory information pointer
        
RETURNED VALUES:
        BAL_PHB_ERR_DEV_UNSUPPORT: error input device id
        BAL_PHB_ERR_PARMETER:	   PhbRecCountP is Null
        BAL_PHB_ERR_NONE:		   SUCCESS
        
*****************************************************************************/

uint32 BalPhbGetCount(uint8 DeviceId,BalPhbCountT* PhbRecCountP);


/*****************************************************************************

FUNCTION NAME: BalPhbGetRecIdByIndex

DESCRIPTION:
		Getting the record Id and device id from cache by record index (sorting by name)
        
PARAMETERS:
        RecIndex[in]: 	  The val phb record index in cache list,sotring by name, beginning from 0
        DeviceIdP[out]:   The device id of the indexing contact record
	    RecordIdP[out]:   The record id of the indexing contact record

RETURNED VALUES:
        BAL_PHB_ERR_PARMETER:  Input parameters error
        BAL_PHB_ERR_REC_EMPTY: Input recIndex's record is empty,not matched
        BAL_PHB_ERR_NONE:	   Success
*****************************************************************************/

uint32 BalPhbGetRecIdByIndex(uint16 RecIndex,uint8* DeviceIdP, uint16* RecordIdP);


/*****************************************************************************

FUNCTION NAME: BalPhbGetIndexByRecId

DESCRIPTION:
		Getting the Index Id from cache by device id and record Id(sorting by name)
        
PARAMETERS:
        DeviceId[in]:   The device id of the indexing contact record
	    RecordId[in]:   The record id of the indexing contact record
	    RecIndexP[out]: The val phb record index in cache list,sorting by name, beginning from 0

RETURNED VALUES:
        BAL_PHB_ERR_DEV_UNSUPPORT:  error input device id
        BAL_PHB_ERR_PARMETER:		error input parameters
        BAL_PHB_ERR_REC_EMPTY:		record is empty,not matched
        BAL_PHB_ERR_NONE:			success
        
*****************************************************************************/
uint32 BalPhbGetIndexByRecId(uint8 DeviceId, uint16 RecordId, uint16* RecIndexP);

/*****************************************************************************

FUNCTION NAME: BalPhbClearField

DESCRIPTION:
		Clearing a  contact's field in ram buffer.
        
PARAMETERS:
      	DeviceId[in]:  device id of the contact record
        RecordId[in]:  record id of the contact record
        FieldId[in]:   field id of the field to be cleared

RETURNED VALUES:
        BAL_PHB_ERR_DEV_UNSUPPORT:  	error input device id
        BAL_PHB_ERR_PARMETER:			error input parameters
        BAL_PHB_ERR_NO_BUFFER:			no buffer for operating record,need flush
        BAL_PHB_ERR_FIELD_NONEXISTENCE: no such phb field
        BAL_PHB_ERR_DEV_INIT_BUSY:	val phb initial busy
        BAL_PHB_ERR_DEV_NOT_REG:	val phb dev not register
        BAL_PHB_ERR_NONE:				success
        (other error code ,see in valphbapi.h)
        
*****************************************************************************/
uint32 BalPhbChangeField(uint8 DeviceId, uint16 RecordId,uint8 FieldId, uint8 FldIdNew);
void BalPhbNameSort(bool FirstName);

#ifdef SYS_OPTION_RUIM
/* for phb uim message deliver */
void BalPhbDeliverMailMsg( uint32 msg_id,
                           void*  msg_buffer,
                           uint32 msg_size );
#endif




#ifdef __cplusplus

}
#endif /* __cplusplus */
#endif 




