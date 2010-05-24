




#ifndef BAL_PHB_DEF_H
#define BAL_PHB_DEF_H

#include "sysdefs.h"
#include "balapi.h"
#include "balphbapi.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define BAL_PHB_MAX_GROUP_PER_REC 5
/* buffer state */
typedef enum
{
	BAL_PHB_BUF_FREE = 0x00,
	BAL_PHB_BUF_USED,
	BAL_PHB_BUF_MODIFIED,
	BAL_PHB_BUF_MAX_STATE
}BalPhbBufferStateT;

typedef struct 
{
	uint8   BufState;		/* The buffer state*/
	uint8   DeviceId;		/* The device id for buffer contact */
	uint16 	RecId;			/* The record id for  the contact in val db */
	uint16 	DataLen;		/* The actual length of the contact data segment */
	uint8   Data; 			/* The buffer for data */
}BalPhbRecordBufferT;

typedef  struct 
{
  uint8  GroupId[BAL_PHB_MAX_GROUP_PER_REC];
  uint16 NextAddress;
  uint8  AlphaIdentifier; /*Alpha Identifier*/ 
} BalPhbNameSummaryT;

typedef struct
{
	uint16 	DevRecId;	  	/* device id and record id */
	uint8	FieldId;		/* field id for this phone number */
	uint8   PhoneNumber; 	/*Dialing Number/SSC String BCD*/
} BalPhbNumberSummaryT;


/* The struct of device list */
typedef struct
{
	uint8	DevId;					/* device id */
	uint16	NameSumOffset;   		/* the first cache item index name summary cache */
	BalPhbCountT   BalPhbCount;		/* device storage information */
	BalPhbDevOptT   * BalPhbOpt;	/* device operation function */
}BalPhbDevListT;


/* called by val, init phb variables and device */
void BalPhbInit(void);

void BalPhbDataInit(void);



#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif 



