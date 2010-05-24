


#ifndef BAL_PHB_CUST_H
#define BAL_PHB_CUST_H

#include "sysdefs.h"
#include "balapi.h"
#include "balphbapi.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BAL_PHB_MAX_FLASH_NAME_CNT    500
#define BAL_PHB_MAX_FLASH_GROUP_CNT   5
#define BAL_PHB_MAX_FLASH_SIZE	  	102400 /* Phb Flash size, 100k*/
#define BAL_PHB_MAX_FLASH_REC_CNT  	(BAL_PHB_MAX_FLASH_NAME_CNT + BAL_PHB_MAX_FLASH_GROUP_CNT)	   /* Phb Flash Record counts */	

#ifdef SYS_OPTION_RUIM
#define BAL_PHB_MAX_UIM_REC_CNT 200
#define BAL_PHB_UIM_MAX_ALPHA_LENGTH BAL_PHB_MAX_ALPHA_LENGTH
#define BAL_PHB_UIM_MAX_BAL_PHONE_LENGTH BAL_PHB_MAX_BAL_PHONE_LENGTH
#define BAL_PHB_MAX_REC_CNT BAL_PHB_MAX_FLASH_REC_CNT + BAL_PHB_MAX_UIM_REC_CNT
#define BAL_PHB_MAX_PHONE_NUMBER_CNT (BAL_PHB_MAX_UIM_REC_CNT + BAL_PHB_MAX_FLASH_NAME_CNT * 5)
#else
#define BAL_PHB_MAX_REC_CNT BAL_PHB_MAX_FLASH_REC_CNT
#define BAL_PHB_MAX_PHONE_NUMBER_CNT (BAL_PHB_MAX_FLASH_NAME_CNT * 5)
#endif


#define BAL_PHB_MAX_REC_SIZE	1024  /* max contact size in bytes */
#define BAL_PHB_MAX_REC_BUFFER_CNT 4

#define BAL_PHB_MAX_ALPHA_LENGTH 51
#define BAL_PHB_MAX_PHONE_LENGTH 33  /* string type */
#define BAL_PHB_MAX_BAL_PHONE_LENGTH 16 /* BCD Type */
#ifndef BAL_PHB_MAX_GROUP_PER_REC
#define BAL_PHB_MAX_GROUP_PER_REC 5
#endif
#define UIM_EXTENDED_FEATURE

#define MaxHanZi             6725/*7315*4 */

/* Bal phb device type */
/* DON'T add item value to 0, it has been define to BAL_PHB_DEV_NONE in valphbapi.c */
typedef enum
{
	 BAL_PHB_DEV_UIM         = 0x01,
	 BAL_PHB_DEV_FLASH       = 0x02,
	 BAL_PHB_DEV_MAX
} BalPhbDeviceT;

/* Bal phb text filed type */
#define  BAL_PHB_FIELD_TEXT_TYPE_EMAIL  0x0001
#define  BAL_PHB_FIELD_TEXT_TYPE_URL  0x0002
#define  BAL_PHB_FIELD_TEXT_TYPE_POSTAL  0x0004
#define  BAL_PHB_FIELD_TEXT_TYPE_NOTE  0x0008
#define  BAL_PHB_FIELD_TEXT_TYPE_COMPANY  0x0010
#define  BAL_PHB_FIELD_TEXT_TYPE_JOBTITLE  0x0020
#define  BAL_PHB_FIELD_TEXT_TYPE_FORMALNAME  0x0040
#define  BAL_PHB_FIELD_TEXT_TYPE_NICKNAME  0x0080
#define  BAL_PHB_FIELD_TEXT_TYPE_BIRTHDAY  0x0100
#define  BAL_PHB_FIELD_TEXT_TYPE_EXTENDED  0x0200
#define  BAL_PHB_FIELD_TEXT_TYPE_STREET  0x0400
#define  BAL_PHB_FIELD_TEXT_TYPE_CITY  0x0800
#define  BAL_PHB_FIELD_TEXT_TYPE_STATE   0x1000
#define  BAL_PHB_FIELD_TEXT_TYPE_ZIP   0x2000
#define  BAL_PHB_FIELD_TEXT_TYPE_COUNTRY   0x4000
#define  BAL_PHB_FIELD_TEXT_TYPE_NUMBER   0x8000
#define  BAL_PHB_FIELD_TEXT_TYPE_NAME   0x00010000

/* Bal phb field type */
typedef enum
{
	BAL_PHB_FIELD_TYPE_NAME 	= BAL_PHB_FIELD_TYPE_NAME_INTER,
	BAL_PHB_FIELD_TYPE_NUMBER	= BAL_PHB_FIELD_TYPE_NUMBER_INTER,
	BAL_PHB_FIELD_TYPE_GROUP	= BAL_PHB_FIELD_TYPE_GROUP_INTER,
	BAL_PHB_FIELD_TYPE_NUMBER_INFO,

  BAL_PHB_FIELD_TYPE_RINGER = BAL_PHB_FIELD_TYPE_RINGER_INTER,
  BAL_PHB_FIELD_TYPE_PICTURE,
  
  BAL_PHB_FIELD_TYPE_EMAIL = BAL_PHB_FIELD_TYPE_EMAIL_INTER,
  BAL_PHB_FIELD_TYPE_URL,
  BAL_PHB_FIELD_TYPE_POSTAL,
  BAL_PHB_FIELD_TYPE_NOTE,
  BAL_PHB_FIELD_TYPE_COMPANY,
  BAL_PHB_FIELD_TYPE_JOBTITLE,
  BAL_PHB_FIELD_TYPE_FORMALNAME,
  BAL_PHB_FIELD_TYPE_NICKNAME,
  BAL_PHB_FIELD_TYPE_BIRTHDAY,
  BAL_PHB_FIELD_TYPE_EXTENDEDADDRESS,
  BAL_PHB_FIELD_TYPE_STREET,
  BAL_PHB_FIELD_TYPE_CITY,
  BAL_PHB_FIELD_TYPE_STATER,
  BAL_PHB_FIELD_TYPE_ZIPCODE,
  BAL_PHB_FIELD_TYPE_COUNTRY,

  BAL_PHB_FIELD_TYPE_TEXT = BAL_PHB_FIELD_TYPE_TEXT_INTER,
  BAL_PHB_FIELD_TYPE_TEXT_TYPE,

	BAL_PHB_FIELD_TYPE_MAX = 0xF0
}BalPhbFieldTypeT;

/* Bal phb field id */
typedef enum
{
	BAL_PHB_FIELD_ID_PRIMARY_NAME 	= 0x00,
	BAL_PHB_FIELD_ID_PRIMARY_NUMBER = 0x02,
	BAL_PHB_FIELD_ID_PRIMARY_NUMBER_INFO = 0x10,
  BAL_PHB_FIELD_ID_RINGER = 0x21,
  BAL_PHB_FIELD_ID_PICTURE = 0x22,
	BAL_PHB_FIELD_ID_PRIMARY_GROUP = 0x30,
  BAL_PHB_FIELD_ID_TEXT_TYPE = 0x80,
	BAL_PHB_FIELD_ID_MAX = 0xF0
}BalPhbFieldIdT;

/* Bal phb number infor, defined by cust ,TON&NPI must be kept for uim number */
typedef struct
{
  uint8  TON;			/* type of number (from uim protocol)*/
  uint8	 NPI;			/* number plan identification (from uim protocol)*/
  uint16 SpeedDial;		/* UI defined */
  uint8  Type;			/* UI defined */
  bool   Restricted;	/* UI defined */
} BalPhbNumberInfoT;

/* for customer */
typedef struct 
{
	uint8   BufState;					/* The buffer state*/
	uint8   DeviceId;					/* The device id for buffer contact */
	uint16 	RecId;						/* The record id for  the contact in val db */
	uint16 	DataLen;					/* The actual length of the contact data segment */
	uint8   Data[BAL_PHB_MAX_REC_SIZE]; /* The buffer for data */
}BalPhbRecordBufferCustT;

/* for customer */
typedef  struct 
{
  uint8  GroupId[BAL_PHB_MAX_GROUP_PER_REC];
  uint16 NextAddress;
  uint32 Flag;
  uint32 RingId;
  uint32 PictureId;
  uint16 NameLen[2];
  uint8  AlphaIdentifier[BAL_PHB_MAX_ALPHA_LENGTH * 2]; /*Alpha Identifier*/ 
} BalPhbNameSummaryCustT;

/* for customer */
typedef struct
{
	uint16 	DevRecId;		/* device id and record id */
	uint8	FieldId;		/* field id for this phone number */
	uint8   PhoneNumber[BAL_PHB_MAX_BAL_PHONE_LENGTH]; /*Dialing Number/SSC String BCD*/
} BalPhbNumberSummaryCustT;

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif


