/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      bthal_pb.c
*
*   DESCRIPTION:    This file uses a sample phonebook implementation on windonws platform
*					for implementing the bthal_pb.h API, providing basic phonebook funcitonality
*					such as getting phonebook entries data. The sample phonebook can be 
*					configured using the PBAP sapmle application.
*
*   AUTHOR:         Yoni Shavit
*
\*******************************************************************************/

#include "btl_config.h"
#include "osapi.h"
#include "bthal_common.h"
#if BTL_CONFIG_PBAPS==   BTL_CONFIG_ENABLED

#include <bthal_pb.h>
#include <bthal_utils.h>
#include <bthal_types.h>


/****************************************************************************
 *
 * Section: Types
 *
 ***************************************************************************/
 
/* Default 0.vcf ownder vCard (version 2.1 format) */
#define OWNER_VCARD     "BEGIN:VCARD\r\nVERSION:2.1\r\nORG;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:" \
                        "Extended Systems, Inc.\r\nN:Hendrix;Kevin\r\nFN:Kevin Hendrix\r\n" \
                        "ADR;WORK;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:;;1600 SW Western Blvd.;Corvallis;" \
                        "OR;97330\r\nTEL;HOME;VOICE:(541) 758-6123\r\nTEL;WORK;VOICE:(541) 758-6123\r\n" \
                        "EMAIL:owner@testing.com\r\nEND:VCARD\r\n"

/* The 1.vcf in the phonebook  */
#define VCARD_ENTRY_1     "BEGIN:VCARD\r\nVERSION:2.1\r\nORG;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:" \
                        "Extended Systems, Inc.\r\nN:Ram;Malovany\r\nFN:Ram Malovany\r\n" \
                        "ADR;WORK;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:;;1600 SW Western Blvd.;Corvallis;" \
                        "OR;97330\r\nTEL;HOME;VOICE:(541) 758-6123\r\nTEL;WORK;VOICE:(541) 999-0000\r\n" \
                        "EMAIL:owner@testing.com\r\nEND:VCARD\r\n"

/* The 1.vcf in the phonebook  */
#define VCARD_ENTRY_2   "BEGIN:VCARD\r\nVERSION:2.1\r\nORG;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:" \
                        "Extended Systems, Inc.\r\nN:Kipi;Kippod\r\nFN:Kipi Kippod\r\n" \
                        "ADR;WORK;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:;;1600 SW Western Blvd.;Corvallis;" \
                        "OR;97330\r\nTEL;HOME;VOICE:(541) 758-6123\r\nTEL;WORK;VOICE:(541) 123-45673\r\n" \
                        "EMAIL:owner@testing.com\r\nEND:VCARD\r\n"
                        

#define 	PB_MAIN_PB 					"telecom/pb"
#define 	PB_MAIN_INCOMING			"telecom/ich"	
#define 	PB_MAIN_OUTGOING			"telecom/och"	
#define 	PB_MAIN_MISSED				"telecom/mch"	
#define 	PB_MAIN_COMBINED		    "telecom/cch"
#define 	PB_SIM_PB 					"SIM1/telecom/pb"
#define 	PB_SIM_INCOMING				"SIM1/telecom/ich"
#define 	PB_SIM_OUTGOING				"SIM1/telecom/och"	
#define 	PB_SIM_MISSED				"SIM1/telecom/mch"	
#define 	PB_SIM_COMBINED		   		"SIM1/telecom/cch"

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/


typedef struct _PbData PbData;

/*---------------------------------------------------------------------------
 *  PbData structure
 *
 *      Represents a sample phonebook with few entries
 */
struct _PbData
{
	BTHAL_U8 *entryDataPtr[3];
	BthalPbEntryName entryName[3];
	
};


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

PbData hardCodedPb;


/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
 
/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * BTHAL_PB_Init()
 */
BthalStatus BTHAL_PB_Init()
{

	hardCodedPb.entryDataPtr[0]= (BTHAL_U8 *)OWNER_VCARD;
	hardCodedPb.entryDataPtr[1] = (BTHAL_U8 *)VCARD_ENTRY_1;
	hardCodedPb.entryDataPtr[2] = (BTHAL_U8 *)VCARD_ENTRY_2;

	BTHAL_UTILS_MemCopy(hardCodedPb.entryName[0].lastName, (const BTHAL_U8 *)"Hendrix", 8);
	BTHAL_UTILS_MemCopy(hardCodedPb.entryName[0].firstName, (const BTHAL_U8 *)"Kevin", 6);

	BTHAL_UTILS_MemCopy(hardCodedPb.entryName[1].lastName, (const BTHAL_U8 *)"Shavit", 7);
	BTHAL_UTILS_MemCopy(hardCodedPb.entryName[1].firstName, (const BTHAL_U8 *)"Yoni", 6);

	BTHAL_UTILS_MemCopy(hardCodedPb.entryName[2].lastName, (const BTHAL_U8 *)"Ben Kipod", 10);
	BTHAL_UTILS_MemCopy(hardCodedPb.entryName[2].firstName, (const BTHAL_U8 *)"Kipi", 5);
				

	return BTHAL_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_PB_Deinit()
 */
BthalStatus BTHAL_PB_Deinit()
{	
	
	return BTHAL_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_PB_Register()
 */
BthalStatus BTHAL_PB_Register(const BthalPbCallBack pbCallback)
{
	/* Sychronous sample phonebook */
	return BTHAL_STATUS_NOT_SUPPORTED;
}


/*-------------------------------------------------------------------------------
 * BTHAL_PB_OpenPb()
 */
BthalStatus BTHAL_PB_OpenPb(const BthalPbPhonebookType phonebookType, BthalPbHandle *handle)
{	
	BthalStatus status = BTHAL_STATUS_SUCCESS;
	
	/* No use for the handle in the sample phonebook */

	
	
	/* Set phonebook path 
	 * Sample phonebook ignores path and has only one folder. Just print out 
	 * the phonebook path
	 */
	
	switch (phonebookType)
	{
		case 5:  	/* (BTHAL_PB_PATH_MAIN + BTHAL_PB_PATH_PHONEBOOK) */
			Report(("Phonebook path is %s",PB_MAIN_PB));
			break;
		case 9: 	/* (BTHAL_PB_PATH_MAIN + BTHAL_PB_PATH_INCOMING) */	
			Report(("Phonebook path is %s",PB_MAIN_INCOMING));
			break;
		case 17:     /* (BTHAL_PB_PATH_MAIN + BTHAL_PB_PATH_OUTGOING) */
			Report(("Phonebook path is %s",PB_MAIN_OUTGOING));
			break;
		case 33:    /* (BTHAL_PB_PATH_MAIN + BTHAL_PB_PATH_MISSED)   */
			Report(("Phonebook path is %s",PB_MAIN_MISSED));
			break;
		case 65:     /* (BTHAL_PB_PATH_MAIN + BTHAL_PB_PATH_COMBINED) */
			Report(("Phonebook path is %s",PB_MAIN_COMBINED));
			break;

			
		case 6: 	/* (BTHAL_PB_PATH_SIM + BTHAL_PB_PATH_PHONEBOOK) */
			Report(("Phonebook path is %s",PB_SIM_PB));
			break;
		case 10:		/* (BTHAL_PB_PATH_SIM + BTHAL_PB_PATH_INCOMING) */
			Report(("Phonebook path is %s",PB_SIM_INCOMING));
			break;
		case 18: 	/* (BTHAL_PB_PATH_SIM + BTHAL_PB_PATH_OUTGOING) */
			Report(("Phonebook path is %s",PB_SIM_OUTGOING));
			break;
		case 34:	/* (BTHAL_PB_PATH_SIM + BTHAL_PB_PATH_MISSED)   */
			Report(("Phonebook path is %s",PB_SIM_MISSED));
			break;
		case 66:		/* (BTHAL_PB_PATH_SIM + BTHAL_PB_PATH_COMBINED) */
			Report(("Phonebook path is %s",PB_SIM_COMBINED));
			break;
			
		default:
			status = BTHAL_STATUS_FAILED;
			break;
			
		
	}

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTHAL_PB_WasPbChanged()
 */
BthalStatus BTHAL_PB_WasPbChanged(const BthalPbPhonebookType phonebookType, BTHAL_BOOL *pbChanged)
{
	/* since we don't have a real phonebook, always return FASLE */
	*pbChanged = BTHAL_FALSE;
	return (*pbChanged);
}

/*-------------------------------------------------------------------------------
 * BTHAL_PB_GetPbEntriesNum()
 */
BthalStatus BTHAL_PB_GetPbEntriesNum(const BthalPbHandle handle, BthalPbEntriesNum *entiesNum)
{

	BthalStatus 		status = BTHAL_STATUS_SUCCESS;
	
    *entiesNum = 3;


    return (status);
}


/*-------------------------------------------------------------------------------
 * BTHAL_PB_GetNewMissedCallsNum()
 */
BthalStatus BTHAL_PB_GetNewMissedCallsNum(const BthalPbHandle handle, BthalPbEntriesNum *entiesNum)
{	
	BthalStatus status = BTHAL_STATUS_SUCCESS;

	/* The phonebook sample doesn't track missed calls. Just provides 3 as an example */
	*entiesNum = 5;
		
	return (status);
}

/*-------------------------------------------------------------------------------
 * BTHAL_PB_GetEntryData()
 */
BthalStatus BTHAL_PB_GetEntryData(const BthalPbHandle handle, BthalPbEntryId entryId, 
														BthalPbEntryFormat entryFormat,
														BthalPbVcardFilter vcardFilter,
													    BTHAL_U8 *vcfEnrtyBuf, 
													    BthalPbVcfLen *vcfEntryLen)
{
	BthalStatus status = BTHAL_STATUS_SUCCESS;

	/* Ignore filter and format */

	/* Copy Data */
	*vcfEntryLen = OS_StrLen((const char *)hardCodedPb.entryDataPtr[entryId]);
	BTHAL_UTILS_MemCopy(vcfEnrtyBuf, hardCodedPb.entryDataPtr[entryId], *vcfEntryLen);

	return (status);
    
}

/*-------------------------------------------------------------------------------
 * BTHAL_PB_BuildEntriesList()
 */
BthalStatus BTHAL_PB_BuildEntriesList(const BthalPbHandle handle, BthalPbListSearch listSearch, 
															BthalPbListSortingOrder listSortOrder,
															BthalPbEntriesList *entriesList)
{
	/* Sample application currently doesn't support search and sort  */
	return BTHAL_STATUS_NOT_SUPPORTED;
}

/*-------------------------------------------------------------------------------
 * BTHAL_PB_GetListedEntryName()
 */
BthalStatus BTHAL_PB_GetListedEntryName(const BthalPbHandle handle, BthalPbEntryName *entryNameStruct, 
																  BthalPbEntryId entryId)
{
	BthalStatus 	 status = BTHAL_STATUS_SUCCESS;

	BTHAL_UTILS_MemCopy((BTHAL_U8*)entryNameStruct, (BTHAL_U8*) &hardCodedPb.entryName[entryId], sizeof(BthalPbEntryName));
	
	return (status);
	
}

/*-------------------------------------------------------------------------------
 * BTHAL_PB_ClosePb()
 */
 BthalStatus BTHAL_PB_ClosePb(BthalPbHandle *handle)
{
	BthalStatus status = BTHAL_STATUS_SUCCESS;
	return (status);
	
}



#else /*BTL_CONFIG_PBAPS ==   BTL_CONFIG_ENABLED*/

BthalStatus BTHAL_PB_Init(void)

{
	
	Report(("BTHAL_PB_Init -BTL_CONFIG_PBAPS is disabled."));
	return BTHAL_STATUS_SUCCESS;
}
BthalStatus BTHAL_PB_Deinit(void)
{
	Report(("BTHAL_PB_Deinit -BTL_CONFIG_PBAPS is disabled."));
	return BTHAL_STATUS_SUCCESS;

}



#endif /*BTL_CONFIG_PBAPS ==   BTL_CONFIG_ENABLED*/




