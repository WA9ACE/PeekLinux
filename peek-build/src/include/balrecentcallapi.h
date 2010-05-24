



#ifndef BAL_RECENTCALL_API_H
#define BAL_RECENTCALL_API_H

#ifdef __cplusplus
extern "C"
{
#endif 

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sysdefs.h"
#include "balapi.h"
#include "balrecentcallcust.h"


/*===========================================================================

                      TYPE DEFINES

===========================================================================*/
/* Bal recentcall type */
typedef enum
{
	 BAL_RC_TYPE_INCOMING   = 0x00,	/* the type of recent call is incoming */
	 BAL_RC_TYPE_OUTGOING,			/* the type of recent call is outgoing */
	 BAL_RC_TYPE_MISSED,			/* the type of recent call is missed */
	 BAL_RC_TYPE_MSG_RECIPIENTS,		/* the type of recent call is msg recipients */
	 BAL_RC_TYPE_MSG_ADDRESSER,		/* the type of recent call is msg addresser */
	 BAL_RC_TYPE_ALL,				/* the type of recent call is all calls,this type MUST NOT be modified */
	 BAL_RC_TYPE_MAX				/* it will be used to flag a empty node */
}BalRcTypeT;

/* Bal recentcall set/get record struct */
typedef struct
{
	BalRcTypeT  RcType;						/* the type of recent call,see BalRcTypeT */
	uint32 iSortKey; 						/* sort key,it will be the same as sequence value when the sort mode is operation sequence. */
	uint8  Name[BAL_RC_MAX_NAME_LENGTH];	/* user name */
	uint8  Data[BAL_RC_MAX_DATA_LENGTH]; 	/* record other call log information */
}BalRcRecordT;


/* Bal recentcall sort mode */
typedef enum
{
	 BAL_RC_SORT_MODE_SEQUENCE   = 0x00,	/* Sort mode is operation sequence, this mode is default */
	 BAL_RC_SORT_MODE_TIME,					/* Sort mode is sort time */
	 BAL_RC_SORT_MODE_MAX				
}BalRcSortModeT;

/* Bal recentcall search mode */
typedef enum
{
	 BAL_RC_SEARCH_MODE_TYPEANDNAME   = 0x00,	/* Search by the type and the name of recent call */
	 BAL_RC_SEARCH_MODE_TYPE,					/* Search just by the type of recent call */
	 BAL_RC_SEARCH_MODE_MAX				
}BalRcSearchModeT;

/* Bal recentcall search result/condition struct */
typedef struct
{
	BalRcSearchModeT SearchMode;			/* Search mode,see 	BalRcSearchModeT */				
	BalRcTypeT  RcType;						/* the type of recent call,see BalRcTypeT */
	uint16	iStartIndex;					/* the position of starting to find.system will update the value after user use a find operation. */
	uint16 iRecId;							/* Record id,flag the address where the call log is stored,system will output it to user */
	uint8  Name[BAL_RC_MAX_NAME_LENGTH];	/* user name ,if Search mode is BAL_RC_SEARCH_MODE_TYPE,system will output it to user*/
}BalRcSearchT;


/* Bal phb error */
typedef enum
{ 
	BAL_RC_ERR_NONE 		= 0,	/* Operating success */
    BAL_RC_ERR_PARMETER 	= 1,    /* Inputting invalid parameter */
	BAL_RC_ERR_NOT_INIT,			/* val recentcall subsystem does not init(BalRcInit has not been called) */
	BAL_RC_ERR_REINIT,				/* val recentcall subsystem has been initialized,and it needn't init again */

	BAL_RC_ERR_NO_SPACE,			/* there is no space to add a record */

	BAL_RC_ERR_DEV_NOT_OPEN,		/* recent call device not open */
	BAL_RC_ERR_DEV_OPEN,			/* error when open recentcall device open */
	BAL_RC_ERR_DEV_CLOSE,			/* error when close recentcall device failure */

	BAL_RC_ERR_REC_READ,			/* read record  of rc device failure */
	BAL_RC_ERR_REC_WRITE,			/* write record of rc device failure */
	BAL_RC_ERR_REC_DELETE,			/* delete record of rc device failure */
	BAL_RC_ERR_REC_EMPTY,			/* empty record of rc device */
	
	BAL_RC_ERR_MAX

}BalRcErrorT;

typedef struct
{
	uint16 iMaxRecordCount;	/* max count of record */
	uint8 iMaxNameLength;	/* max length of name */
	uint16 iMaxDataLength;	/* max length of data */
	uint16 iMaxRecordLength;/* max length of a record */
	uint16 iRecordSummarySize;	/* the size of record summary info struct */
	void* pRecordSummaryAddress; /* the address of record summary info buffer */
	void* pRecordBufferAddress; /* the address of record buffer temp for val to read a record */
	uint8 iNameOffset;		/* offset of name in RcRecord struct */
	uint8 iDataOffset;		/* offset of data in RCRecord struct */
}BalRcCustParamT;

/* Bal recentcall summary info struct,just used by record cache for cust*/
typedef struct
{
	BalRcTypeT  RcType;						/* the type of recent call,see BalRcTypeT */
	uint32 iSortKey; 						/* sort key,it will be the same as sequence value when the sort mode is operation sequence. */
	uint16 iNextRecId;						/* Record id,flag the address where the next call log is stored and it is ordered by current sorting mode */
	uint8  Name[BAL_RC_MAX_NAME_LENGTH];	/* user name */
}BalRcSummaryInfoCustT;

/*===========================================================================

                       FUNCTION DEFINITIONS 

===========================================================================*/




/*****************************************************************************

FUNCTION NAME: BalRcInit

DESCRIPTION:
        Initializing Recent call system and build cache, set sort mode
        
PARAMETERS:
        sortMode[in]:BalRcSortModeT type,the default value is BAL_RC_SORTMODE_SEQUENCE;
        
RETURNED VALUES:
        value =  BAL_PHB_ERR_NONE: success
        
*****************************************************************************/

uint32 BalRcInit(BalRcSortModeT sortMode);

/*****************************************************************************

FUNCTION NAME: BalRcDestroy

DESCRIPTION:
        Destroy Recent call system ;
        
PARAMETERS:
        null
        
RETURNED VALUES:
        value =  BAL_PHB_ERR_NONE: success
        
*****************************************************************************/

uint32 BalRcDestroy(void);

/*****************************************************************************

FUNCTION NAME: BalRcAddRecord

DESCRIPTION:
        Add a new recent call into flash.
        
PARAMETERS:
         pRcRecord[in]: the point of BalRcRecordT struct type.
         piRecId[out]: if api return success,system will output the new record id to store the current recent call.
         
RETURNED VALUES:
        value =  BAL_PHB_ERR_NONE: success
        
*****************************************************************************/

uint32 BalRcAddRecord(BalRcRecordT* pRcRecord, uint16* piRecId);


/*****************************************************************************

FUNCTION NAME: BalRcGetRecord

DESCRIPTION:
        Get a recent call from flash.
        
PARAMETERS:
		 iRecId[in]: Record id,flag the address where the call log is stored. 
         pRcRecord[out]: BalRcRecordT struct type.the point of pRcRecord must be valid,and system will output the detail info to user.
         
RETURNED VALUES:
        value =  BAL_PHB_ERR_NONE: success
        
*****************************************************************************/

uint32 BalRcGetRecord(const uint16 iRecId,BalRcRecordT* pRcRecord);

/*****************************************************************************

FUNCTION NAME: BalRcDeleteRecord

DESCRIPTION:
        Delete a recent call from flash.
        
PARAMETERS:
		 iRecId[in]: Record id,flag the address where the call log is stored 
		 
RETURNED VALUES:
        value =  BAL_PHB_ERR_NONE: success
        
*****************************************************************************/

uint32 BalRcDeleteRecord(const uint16 iRecId);

/*****************************************************************************

FUNCTION NAME: BalRcUpdateRecord

DESCRIPTION:
        Update a existed recent call into flash.
        
PARAMETERS:
		iRecId[in]: Record id,flag the address where the call log is stored 
        pRcRecord[in]: the point of BalRcRecordT struct type.system will update the record by specified iRecId
        bSortFlag[in]:if it is true,the record will be resorted ,else system just update the value,doesn't resort it.
         
RETURNED VALUES:
        value =  BAL_PHB_ERR_NONE: success
        
*****************************************************************************/

uint32 BalRcUpdateRecord(const uint16 iRecId, BalRcRecordT* pRcRecord, bool bSortFlag);


/*****************************************************************************

FUNCTION NAME: BalRcGetCountOfRecord

DESCRIPTION:
        Get a count of recent call by the type of recent call.
        
PARAMETERS:
		RcType[in]:		the type of recent call,see BalRcTypeT
		pnCount[out]:	output the count of recent call
         
RETURNED VALUES:
        value =  BAL_PHB_ERR_NONE: success
        
*****************************************************************************/

uint32 BalRcGetCountOfRecord(const BalRcTypeT RcType,uint16* pnCount);


/*****************************************************************************

FUNCTION NAME: BalRcGetRecordByIndex

DESCRIPTION:
        Get the record of recent call by the type of recent call.
        
PARAMETERS:
		RcType[in]:		the type of recent call,see BalRcTypeT
		iIndex[in]: 	the index of recent call order by current type;
		piRecId[out]:	output the record id of recent call 
         
RETURNED VALUES:
        value =  BAL_PHB_ERR_NONE: success
        
*****************************************************************************/

uint32 BalRcGetRecordByIndex(const BalRcTypeT RcType,uint16 iIndex,uint16* piRecId);

/*****************************************************************************

FUNCTION NAME: BalRcGetIndexByRecord

DESCRIPTION:
        Get the index of recent call by the type of recent call.
        
PARAMETERS:
		RcType[in]:		the type of recent call,see BalRcTypeT
		iRecId[in]: 	specified the record id of recent call 
		piIndex[out]:	output the index of recent call order by current type;
         
RETURNED VALUES:
        value =  BAL_PHB_ERR_NONE: success
        
*****************************************************************************/

uint32 BalRcGetIndexByRecord(const BalRcTypeT RcType,uint16 iRecId,uint16* piIndex);

/*****************************************************************************

FUNCTION NAME: BalRcSearchRecord

DESCRIPTION:
        Search a/all record by specified search mode.
        
PARAMETERS:
		pRcSearch[in/out]: the type of recent call,see BalRcTypeT 	
         
RETURNED VALUES:
        value =  BAL_PHB_ERR_NONE: success
        
*****************************************************************************/

uint32 BalRcSearchRecord(BalRcSearchT* pRcSearch);


#ifdef __cplusplus

}
#endif /* __cplusplus */
#endif 



