#ifndef BAL_SMARTMESSAGE_FLASH_H
#define BAL_SMARTMESSAGE_FLASH_H

#include "sysdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//#define MAX_SMS_LENGTH_IN_FILE 253
//#define MAX_SMS_REC_NUM_IN_FILE 60

#define MAX_RINGTONE_REC_NUM_IN_FILE 60
#define MAX_VCARD_REC_NUM_IN_FILE 60
#define MAX_SM_REC_NUM_IN_FILE (MAX_RINGTONE_REC_NUM_IN_FILE+MAX_VCARD_REC_NUM_IN_FILE)

#define MAX_DATA_LEN_IN_SMS 140
#define MAX_DATA_LENGTH_IN_FILE 140*3
#define MAX_SM_TITLE_LEN 15*2 +1 + 1
#define SEGMENT_LENGTH 2
#define RING_TONE_PORT 0x1581
#define VCARD_PORT 0x23F4
#define ASCII_BLANK 0x20

#define FIRST_SEGMENT 	1
#define SECOND_SEGMENT  2
#define THIRD_SEGMENT 	3
#define FIRST_SEGMENT_MASK 0x00000001
#define SECOND_SEGMENT_MASK  0x00000002
#define THIRD_SEGMENT_MASK 	0x00000004
#define DATA_LEN_OFFSET 8
#define DATA_LEN_MASK 0xFF

#define MAX_SM_PHONE_LENGTH 21

typedef struct 
{
  uint8  Year;
  uint8  Month;
  uint8  Day;
  uint8  Hours;
  uint8  Minutes;
  uint8  Seconds;
} BalSMTimeT;

typedef enum 
{
    SM_SUCCESS = 0 ,
    SM_MEMORY_FAILURE,	
    SM_GENERAL_FAILURE,
    SM_FLASH_OUT_OF_BOUND,
    SM_FLASH_FILE_FAILD,    
    SM_FLASH_FILE_FULL ,
    SM_MERGED
    
} BalSMResultT;

typedef enum
{
    SM_COMPLETED = 1,
    SM_UNCOMPLETED = 2, 
    SM_READ       = 4,
    SM_TO_BE_READ = 8   //sure completed
}BalSMRecStatusT;

/*smart message record header struct in file*/
typedef  struct
{
	//SMS field
      uint16 port;
      uint8 recStatus;
      uint8  From[MAX_SM_PHONE_LENGTH];
      uint8  callBack[MAX_SM_PHONE_LENGTH];  
      uint8  SMTitle[MAX_SM_TITLE_LEN];/*The last byte is for indicating Codepage 1:unicode*/
      uint16 SMRefNumber;
      uint16  SMTotalSegments;

      /********************************************************************
      uint32 SMCurrentSegement:
      the first 8 bits used to record the received segments. Actually only first 3 bits will be used. If one of the max 3 segments received, the mapping bit will be 1, otherwise 0. 
      the second 8 bits indicate the first segment data length.
      the third 8 bits indicate the second segment data length.
      the forth 8 bits indicate the third segment data length. 

      Note:if all segmengts are received.the 2 ~ 4 bytes save the total length of the merged smart message.
      ********************************************************************/
      uint32 SMCurrentSegement;  
	
      BalSMTimeT TimeStamp;// Also can use it to judge whether overtime in waiting for receiving segments
}BalSMFileRecHeaderT;

/*smart message record struct in file*/
typedef  struct
{
      BalSMFileRecHeaderT recHeader;
      uint8 SMSourceData[MAX_DATA_LENGTH_IN_FILE];
}BalSMFileRecordT;

typedef  struct
{
      BalSMFileRecHeaderT recHeader;
      uint16 recID;
}BalSMHeaderRecIDT;

typedef  struct
{
  	
      //SMS field
      uint8  From[MAX_SM_PHONE_LENGTH];
      uint8  callBack[MAX_SM_PHONE_LENGTH];  
      BalSMTimeT TimeStamp;
      
         // "//SCKL"Text-based header  10 - 21 bytes length
      uint16 Destination;  				/*Destination Port*/
      uint16 Source;	   				/*Source Port*/
      uint8 SMTitle[MAX_SM_TITLE_LEN];	/*The last byte is for indicating Codepage 1:unicode*/
      uint16 SMRefNumber;              		/*Reference Number, from 00 to FF, a number indicates the index of the vCard sent from a handset. We can judge witch vCard a segment belongs to together with From. */
      uint16 SMTotalSegments;   		/*the total segments number, from 01 to 03.*/
      uint16 SMCurrentSegment;      	/*current segment' s index in the segments. from 01 to vCardTotalSegments.*/
      uint16 SourceDataLen;
      uint8 SMSourceData[MAX_DATA_LEN_IN_SMS];
	  
}BalSmartMessageRecT;

typedef struct
{
	uint16 port;      	//smart message type
	bool IsExist;    	//indicate whether the smart message exists.
	uint8 IsUnread;	//the record read or unread.0-read,1-unread
}BalSmartMessageRecTable;
/******************************************************************************
FUNCTION BalGetSMRecorId

DESCRIPTION
  before update,add,delete record ,should get the record id.

Parameters:
	SMRecP -[in] the receiving smart message
	recId - [out]the record id 

Returns:   
	BalSMResultT

*******************************************************************************/
BalSMResultT BalGetSMRecorId(BalSmartMessageRecT *SMRecP,uint16 *recId, uint16 portType);

/******************************************************************************
FUNCTION BalGetSMUnreadCount

DESCRIPTION
  Get unread SM according portype variable

Parameters:
	portType -[in]  smart message type
Returns:   
	uint8 - return the count of unread SM

*******************************************************************************/
uint8 BalGetSMUnreadCount(uint16 portType);

/******************************************************************************
FUNCTION BalSMFileReadRec

DESCRIPTION
  read a SM record from SM flash file. 

Parameters: 
    recIndex [in] the record index
    pSMRec  [out]the pointer to SM record

Returns:
    true is success, and false is error  
*******************************************************************************/
BalSMResultT BalSMFileReadRec(uint16 recIndex,BalSMFileRecordT *SMFileRecP);


/******************************************************************************
	FUNCTION BalSMFileUpdateRec

	DESCRIPTION
	  update a Smart Message record in RingTone flash file with the index of recIndex..

	Parameters: 

	  recIndex- [in] the record index
		SMRecP-  [in] the pointer to a SMSegment object witch received as a sms

	Returns:
	    SM_SUCCESS -- success update, but record not completed
	    SM_FLASH_COMPLETE -- success update, and record completed
*******************************************************************************/
BalSMResultT BalSMFileUpdateRec(uint16 *recIndexP, BalSmartMessageRecT *SMRecP,uint16 portType);	

/******************************************************************************
	FUNCTION BalSMFileUpdateRecHeader

	DESCRIPTION
	  update a Smart Message record header in RingTone flash file with the index of recIndex..

	Parameters: 

	  recIndex- [in] the record index
	fileRecHeaderP-  [in] the pointer to a BalSmartMessageRecT struct 
		
	Returns:
	    SM_SUCCESS -- success update, but record not completed
*******************************************************************************/
BalSMResultT BalSMFileUpdateRecHeader(uint16 recIndex, BalSMFileRecHeaderT *fileRecHeaderP);

/******************************************************************************
	FUNCTION BalSMFileReadRecHeader

	DESCRIPTION
	  get a Smart Message record header in RingTone flash file with the index of recIndex..

	Parameters: 

	  recIndex- [in] the record index
	fileRecHeaderP-  [out] the pointer to a BalSmartMessageRecT struct 
		
	Returns:
	    SM_SUCCESS -- success update, but record not completed
*******************************************************************************/
BalSMResultT BalSMFileReadRecHeader(uint16 recIndex, BalSMFileRecHeaderT *fileRecHeaderP);


/******************************************************************************
FUNCTION BalSMFileDelRec

DESCRIPTION
  Delete a RingTone record from flash RingTone file. 

Parameters:     
  recIndex [in] the record index
Returns:

*******************************************************************************/
BalSMResultT BalSMFileDelRec(uint16 recIndex);

/******************************************************************************
	FUNCTION BalSMFileCreate

	DESCRIPTION
	  Create Ring Tone file in flash

	Parameters:  	

	Returns:   true is success, and false is error  

*******************************************************************************/
BalSMResultT BalSMFileCreate(void);

/******************************************************************************
FUNCTION BalSMFileOpen

DESCRIPTION
  Open Ring Tone flash file

Parameters:     

Returns:   true is success, and false is error  

*******************************************************************************/
BalSMResultT BalSMFileOpen(void);

/******************************************************************************
FUNCTION BalSMFileClose

DESCRIPTION
  to close the RING TONE flash file.

Parameters:     

Returns:   true is success, and false is error  

*******************************************************************************/
BalSMResultT BalSMFileClose(void);

/******************************************************************************
FUNCTION BalSMInitiateFileRec

DESCRIPTION
  initiate SMFileRec according to SMRecP.

Parameters:     
	SMFileRec -[out]point to BalSMFileRecordT variable.
	SMFileRec -[in]point to BalSmartMessageRecT variable.
Returns:   
	BalSMResultT

*******************************************************************************/
BalSMResultT BalSMInitiateFileRec(BalSMFileRecordT *SMFileRec,const BalSmartMessageRecT *SMRecP) ;

/******************************************************************************
FUNCTION BalInitiateSMFile

DESCRIPTION
  initiate SM File according.

Parameters:     
	none
Returns:   
	void

*******************************************************************************/
void BalInitiateSMFile(void) ;

/******************************************************************************
	FUNCTION BalSMFileUpdateWoleRec

	DESCRIPTION
	  update a Smart Message record  in RingTone flash file with the index of recIndex..

	Parameters: 

	  recIndex- [in] the record index
	  fileRecP-  [in] the pointer to a BalSMFileRecordT struct 
		
	Returns:
	    SM_SUCCESS -- success update, but record not completed
*******************************************************************************/
BalSMResultT BalSMFileUpdateWoleRec(uint16 recIndex, BalSMFileRecordT *fileRecP);

/******************************************************************************
	FUNCTION BalGetSMRecLookupTable

	DESCRIPTION
	  return gSmRecTable

	Parameters: 

	  void
		
	Returns:
	    BalSmartMessageRecTable -- point to gSmRecTable
*******************************************************************************/
BalSmartMessageRecTable* BalGetSMRecLookupTable(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif




