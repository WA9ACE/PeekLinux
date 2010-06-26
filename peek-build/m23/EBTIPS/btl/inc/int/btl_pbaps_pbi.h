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
*   FILE NAME:      btl_pbaps_pbi.h
*
*   DESCRIPTION:    This file contains function prototypes, constant and type definitions
*				    for btl_pbaps_pbi.c module, which is a BTL internal interface between
*					the BTL_PBAP and the BTHAL_PB that handles the phonebook objects.  
*					This module is called by  BTL_PBAPS module to prepare the phonebook data 
*					for the GOEP in order to send it to the peer device. It calls the BTHAL_PB 
*					to retrieve the device's phonebook data. 
*
*   AUTHOR:         Yoni Shavit
*
\*******************************************************************************/

#ifndef __BTL_PBAPS_PB_H
#define __BTL_PBAPS_PB_H

#include <bthal_pb.h>
#include <pbap.h>
#include "btl_unicode.h"

/****************************************************************************
 *
 * Section: Constants
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * BTL_PBAPS_PB_MAX_PATH_LEN constant
 *
 *     Maximum number of characters allowed for path names + 1 (null-
 *     terminating character).
 */
#define BTL_PBAPS_PB_MAX_PATH_LEN         25

/*---------------------------------------------------------------------------
 * BTL_PBAPS_PB_MAX_ENTRY_INDEX_LEN constant
 *
 *     Maximum number of characters allowed for entry index name recevied 
 *     from the client + 1 (null- terminating character). For examlpe 0.vcf
 */
#define BTL_PBAPS_PB_MAX_ENTRY_INDEX_LEN         10

/****************************************************************************
 *
 * Section: Types
 *
 ***************************************************************************/

/* Forward declarations */
typedef struct _BtlPbapsPbParams  			  BtlPbapsPbParams;
typedef struct _BtlPbapsPbFolderListingParams BtlPbapsPbFolderListingParams;
typedef struct _BtlPbapsPbVcardEntryParams    BtlPbapsPbVcardEntryParams;

/*---------------------------------------------------------------------------
 * BtlPbapsPbSetPathFlags type
 *
 *     	Used as an input argument for the  BtlPbapsPbSetPbPath fucntion. 
 *		Indicates whether the virtual PBAP folder structure path should be 
 *		reset or backuped in the BtlPbapsPbSetPbPath function, 
 *		or to identify the portion of path information that
 *      is provided in the Phonebook Path.  For example, a Pull Phonebook
 *      operation supplies the entire absolute path name for the phonebook,
 *      while Pull Vcard Listing and Pull Vcard Entry provide only the name
 *      of the phonebook and the name of the vCard, respectively.
 */
typedef U8 BtlPbapsPbSetPathFlags;

#define BTL_PBAPS_PB_SET_PATH_RESET          0x00 	 /* Reset path to root folder  */
#define BTL_PBAPS_PB_SET_PATH_BACKUP         0x01    /* Back up a level */
#define BTL_PBAPS_PB_SET_PATH_LOCAL          0x02	 /* Local path information */
#define BTL_PBAPS_PB_SET_PATH_ABSOLUTE       0x03    /* Absolute path information */


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*--------------------------------------------------------------------------
 * BtlPbapsPbParams structure
 *
 *     Holds the needed parameters for the pull phone book operation 
 */
struct _BtlPbapsPbParams 
{
    char                pbName[BTL_PBAPS_PB_MAX_PATH_LEN]; /* Name of the phonebook to receive (*.vcf) */
    BthalPbVcardFilter  filter;            		 			/* Filter of the required vCard fields */
    BthalPbEntryFormat  format;             			    /* Format of vCard (2.1 or 3.0) */
    U16                 maxListCount;           		    /* Maximum of vCard entries supported */
    U16                 startOffset;        				/* Offset of the first vCard entry */
} ;


/*--------------------------------------------------------------------------
 * BtlPbapsFolderListingParams structure
 *
 *     Holds the needed parameters for the pull vCard listing operation
 */
struct _BtlPbapsPbFolderListingParams 
{
    char                folderName[BTL_PBAPS_PB_MAX_PATH_LEN];    /* Name of the folder to retrieve */
    PbapVcardSortOrder  order;              					   /* Indexed/Alphabetical/Phonetical */
    U8                  searchAttrib;       					   /* Name (0x00), Number (0x01), or Sound (0x02) */
    BtlUtf8             searchValue[BTHAL_PB_MAX_ENTRY_NAME];       /* Absolute or partial */
    U16                 maxListCount;  			       			   /* Maximum of vCard entries supported */
    U16                 listStartOffset;    				  	   /* Offset of the first vCard entry */
} ;

/*--------------------------------------------------------------------------
 * BtlPbapsPbVcardEntryParams structure
 *
 *     Holds the needed parameters for the pull entry operation
 */
struct _BtlPbapsPbVcardEntryParams 
{
    char                entryName[BTL_PBAPS_PB_MAX_ENTRY_INDEX_LEN]; /* Name of the entry to retrieve (*.vcf) */
    BthalPbVcardFilter  filter;            							 /* Filter of the required vCard fields */
    BthalPbEntryFormat  format;             					     /* Format of vCard (2.1 or 3.0) */
};

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlPbapsPbInit()
 *
 *	    This fucntion inits the BTL_PBAP_PB module. 
 *		
 * Type:
 *		Synchronous
 *
 * Parameters:
 *	
 *		server [in] - The current PBAP server session 
 *
 * Returns:
 *
 *		BT_STATUS_SUCCESS - Indicates that the operation was successful 
 *
 *     	BT_STATUS_FAILED - Indicates that the operation failed.
 *
 */
BtStatus BtlPbapsPbInit(PbapServerSession *server);

/*-------------------------------------------------------------------------------
 * BtlPbapsPbStartOper()
 *
 *	    This fucntion inits BTL_PBAP_PB paramters at the start of any pull operation.
 *		
 * Type:
 *		Synchronous /Asynchronous
 *
 * Parameters:
 *	
 * Returns:
 *
 *		BT_STATUS_SUCCESS - Indicates that the operation was successful 
 *
 *     	BT_STATUS_FAILED - Indicates that the operation failed.
 *
 */
BtStatus BtlPbapsPbStartOper();

/*-------------------------------------------------------------------------------
 * BtlPbapsPbBuildEntryData()
 *
 *	    This function builds data for the pull entry operation. 
 *		Call BtlPbapsGetData to get the data synchoronously after it is ready.
 *      The first call to the function will always be in the context of the BT stack. If the pb is asynchronious,
 *		the function will be called later in the context or in the pb task to continue the operaiton.
 *		At the end of the operation PBAP_continue function should be called in order to continue the 
 *		stack operation. 
 *		
 *		
 * Type:
 *		Synchronous /Asynchronous
 *
 * Parameters:
 *		stackTask [in] - If TRUE the function is called in the context of the stack task.
 						 If false the function is called in the context of the phonebook task.
 * Returns:
 *
 *		BT_STATUS_PENDING - Indicates that the operation was successful  (Aysch mode)
 *
 *		BT_STATUS_SUCCESS - Indicates that the operation was successful (Synch mode)
 *
 *		BT_STATUS_NOT_FOUND - No matching entry was found.  
 *
 *     	BT_STATUS_FAILED - Indicates that the operation failed.
 *
 *		BT_STATUS_IMPROPER_STATE - Entry data can not be built since the phonebook is not valid.
 *								   one or more entired were modified/deleted during the
 *								   session. Return value "Precondition failed" should be returned to the PCE
 *
 */
BtStatus BtlPbapsPbBuildEntryData(BOOL stackTask);



/*-------------------------------------------------------------------------------
 * BtlPbapsPbBuildPbData()
 *
 *	    This function builds data for the pull phonebook operation. 
 *		Call BtlPbapsPbGetData to get the data synchoronously after it is ready.
 *		The first call to the function will always be in the context of the BT stack. If the pb is asynchronious,
 *		the function will be called later in the context or in the pb task to continue the operaiton.
 *		At the end of the operation PBAP_continue function should be called in order to continue the 
 *		stack operation. 
 *
 * Type:
 *		Synchronous /Asynchronous
 *
 * Parameters:
 *		stackTask [in] - If TRUE the function is called in the context of the stack task.
 						 If false the function is called in the context of the phonebook task.
 * Returns:
 *
 *		BT_STATUS_PENDING - Indicates that the operation was successful  (Aysch mode)
 *
 *		BT_STATUS_SUCCESS - Indicates that the operation was successful (Synch mode)
 *
 *		BT_STATUS_FAILED - Indicates that the operation failed.
 *
 */
BtStatus BtlPbapsPbBuildPbData(BOOL stackTask);

/*-------------------------------------------------------------------------------
 * BtlPbapsPbGetData()
 *
 *	    This funciton fills the given buffer with the pull phonebook operation data synchronously. A 'build data' 
 * 		funciton such as BtlPbapsPbBuildPbData must be called perior to this funciton in order to build that data.
 *		Note that the given buffer length may not be long enough for the ready data. If so, 
 *		the paramter 'more' indicates to the caller that there is more data waiting for sending.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		dataPtr [in] - Pointer to the data to be filled.
 *
 *		dataLen [in] - The length of the buffer dataPtr.
 *		
 *		more [out] - Indicates to the caller whether the is more ready data of the entry to be sent. 
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Indicates that the operation was successful  
 *
 *		BT_STATUS_FAILED - Indicates that the operation failed.
 *
 */
BtStatus BtlPbapsPbGetData(U8 *dataPtr, U16 *dataLen, BOOL *more);

/*-------------------------------------------------------------------------------
 * BtlPbapsPbBuildFolderListingData()
 *
 *	    This asynchronous function builds data for the pull vCard listing operation.
 *		The funciton can be both synchronous or asynchronous depanding on the platform. 
 *		The first call to the function will always be in the context of the BT stack. If the pb is asynchronious,
 *		the function will be called later in the context or in the pb task to continue the operaiton.
 *		At the end of the operation PBAP_continue function should be called in order to continue the 
 *		stack operation. 
 *
 *
 * Type:
 *		Synchronous /Asynchronous
 *
 * Parameters:
 *		stackTask [in] - If TRUE the function is called in the context of the stack task.
 *						 If false the function is called in the context of the phonebook task.
 * Returns:
 *		BT_STATUS_PENDING - Indicates that the operation was successful  (Aysch mode)
 *
 *		BT_STATUS_SUCCESS - Indicates that the operation was successful (Synch mode) 
 *
 *		BT_STATUS_FAILED - Indicates that the operation failed.
 *
 */
BtStatus BtlPbapsPbBuildFolderListingData(BOOL stackTask);

/*-------------------------------------------------------------------------------
 * BtlPbapsPbSetPbPath()
 *
 *	    This fucntion sets the phonebook path according to the PBAP virtual folder structure.
 *		The function performs no access to the device's phonebook, and the new path status is saved localy, 
 *		to be used in future operations.
 *
 * Type:
 *		Synchronous
 *		
 *
 * Parameters:
 *		Path [in] - The phonebook path in terms of PBAP virtual folder structure.
 *
 *		BtlPbapsPbSetPathFlags [in] -  Indicates whether the path should be reset or backuped.
 *
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Indicates that the operation was successful  
 *
 *     	BT_STATUS_FAILED - Indicates that the operation failed.
 */
BtStatus BtlPbapsPbSetPbPath(const char *path, BtlPbapsPbSetPathFlags flags);


/*-------------------------------------------------------------------------------
 * BtlPbapsPbGetPbParams()
 *
 *	    This fucntion requests for the number of new missed calls that was not checked yet by the user,
 *		and for the number of records in the given phonebook path, if those are needed accoring to 
 *		previously received Rx params.
 *		The first call to the function will always be in the context of the BT stack. If the pb is asynchronious,
 *		the function will be called later in the context or in the pb task to continue the operaiton.
 *		At the end of the operation PBAP_continue function should be called in order to continue the 
 *		stack operation. 
 *		If this function fails, the OBEX operation should be aborted.  
 *
 *		
 *
 * Type:
 *		synchronous / asynchronous.
 *
 * Parameters:
 *
 *		pullPbOper [in] - Determines the pull operation:
 *						True - pull phonebook
 *						False - pull vCardListing
 *
 *		stackTask [in] - If TRUE the function is called in the context of the stack task.
 *						 If false the function is called in the context of the phonebook task.
 * Returns:
 *		BT_STATUS_PENDING - Indicates that the operation was successful  (Aysch mode)
 *
 *		BT_STATUS_SUCCESS - Indicates that the operation was successful (Synch mode)
 *
 *		
 *		BT_STATUS_INVALID_PARM - Indicates that a wrong Rx parameter was recevied. Abort the OBEX operation with 
 *						   PBRC_BAD_REQUEST return code .
 *
 *		BT_STATUS_BUSY - Indicates that the phonebook is busy. Abort the OBEX operation with 
 *						   PBRC_SERVICE_UNAVAILABLE return code.
 *
 *		BT_STATUS_FAILED - Indicates that the operation failed. Abort the OBEX operation with 
 *						   PBRC_SERVICE_UNAVAILABLE return code.
 *
 *		BT_STATUS_NO_RESOURCES - Indicates that the operation failed due to lack of internal resources.
 *									Abort the OBEX operation with PBRC_NOT_ACCEPTABLE return code
 *
 *		BT_STATUS_IMPROPER_STATE - Indicates that the operation failed since a pull vCard listing operation
 *								   cannot be performed from the given virtual PB folder's path.
 *
 */
BtStatus BtlPbapsPbGetPbParams(BOOL pullPbOper, BOOL stackTask);


/*-------------------------------------------------------------------------------
 * BtlPbapsPbSetPullPbParams()
 *
 *	    Copy the pull phonebook params from the event params to an internal struct.
 *
 * Type:
 *		synchronous
 *
 * Parameters:
 *		
 *		PbapServerCallbackParms [in] - Event parameters
 *
 * Returns:
 *		void
 */
void BtlPbapsPbSetPullPbParams(PbapServerCallbackParms *Parms);

/*-------------------------------------------------------------------------------
 * BtlPbapsPbSetPullListParams()
 *
 *	    Copy the pull vCard listing params from the event params to an internal struct.
 *
 * Type:
 *		synchronous
 *
 * Parameters:
 *		
 *		PbapServerCallbackParms [in] - Event parameters
 *
 * Returns:
 *		void
 */
void BtlPbapsPbSetPullListParams(PbapServerCallbackParms *Parms);

/*-------------------------------------------------------------------------------
 * BtlPbapsPbSetPullEntryParams()
 *
 *	    Copy the pull entry params from the event params to an internal struct.
 *
 * Type:
 *		synchronous
 *
 * Parameters:
 *		
 *		PbapServerCallbackParms [in] - Event parameters
 *
 * Returns:
 *		void
 */
void BtlPbapsPbSetPullEntryParams(PbapServerCallbackParms *Parms);

/*-------------------------------------------------------------------------------
 * BtlPbapsPbUpdateNameParam()
 *
 *	    This funciton is called in order to setup the phonebook/enrty name 
 *      if the Application Parameters were never received via the PBAP_EVENT_PARAMS_RX 
 *      event, or if the Name header was sent after the Application Parameters.
 *		In that case the 'name' field wasn't supplied at the functions -
 *		BtlPbapsPbSetPullPbParams, BtlPbapsPbSetPullListParams, BtlPbapsPbSetPullEntryParams
 *
 * Type:
 *		synchronous
 *
 * Parameters:
 *		
 *		oper [in] - PBAP operation name
 *
 *
 * Returns:
 *		void
 */
void BtlPbapsPbUpdateNameParam(PbapOp oper, char * nameParam);

/*-------------------------------------------------------------------------------
 * BtlPbapsPbSetPullEntryParams()
 *
 *	    Inits the different pull params. 
 *
 * Type:
 *		synchronous
 *
 * Parameters:
 *		
 *		
 * Returns:
 *		void
 */
void BtlPbapsPbInitParams();

/*---------------------------------------------------------------------------
 * BtlPbapsPbSetChangedStatus
 *
 * 	  	Sets the 'pbChanged' flag with the given changedFlag.
 *		Used along with BtlPbapsPbGetChangedStatus in order to check whether the
 *      phonebook validation was already checked by asking the BTHAL.
 *
 * Parameters:

 *		changedFlag [in] - Boolean flag.
 * Return:
 *
 *		void
 */
void BtlPbapsPbSetChangedStatus(BOOL changedFlag);

/*---------------------------------------------------------------------------
 * BtlPbapsPbGetChangedStatus
 *
 * 	  	Gets the 'pbChanged' flag.  
 *		Used along with BtlPbapsPbSetChangedStatus in order to check whether the
 *      phonebook validation was already checked by asking the BTHAL.
 *
 * Parameters:
 *		None
 * Return:
 *
 *		Boolean flag.
 */

BOOL BtlPbapsPbGetChangedStatus();

/*---------------------------------------------------------------------------
 * BtlPbapsPbAbortOperation
 *
 * 	  	This fucntion is called if the client issued an operation abort. 
 *		In that case close the phonebook and call continue. 
 *
 * Parameters:
 *		sendContinue[in] - Indicates whether the PBAP_Continue function should 
 *					       be called in the end of the operation. 
 * Return:
 *
 *		Void
 */

void  BtlPbapsPbAbortOperation(BOOL sendContinue);


#endif /* __BTL_PBAPS_PB_H */

