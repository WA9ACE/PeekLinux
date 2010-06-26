/*******************************************************************************\
##                                                                           	*
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
##																				*
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
##																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_common.h
*
*	BRIEF:          This file defines the common API of the BTL component and 
*					general types, defines, and prototypes for BTL component.
*
*   DESCRIPTION:    General
*
*				    This file defines the common API of the BTL component and 
*					general types, defines, and prototypes for BTL component.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/

#ifndef __BTL_COMMON_H
#define __BTL_COMMON_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <bttypes.h>
#include "btl_unicode.h"


/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _BtlAppHandle 	BtlAppHandle;

typedef struct _BtlEvent		BtlEvent;

/*-------------------------------------------------------------------------------
 * BtlObjectLocation type
 *
 *     Defines the location of an object.
 */
typedef U8 BtlObjectLocation;

#define BTL_OBJECT_LOCATION_MEM 			(0x00)
#define BTL_OBJECT_LOCATION_FS	    		(0x01)

/*-------------------------------------------------------------------------------
 * BtlCallBack type
 *
 *     A function of this type is called to indicate Common BTL events.
 */
typedef void (*BtlCallBack)(const BtlEvent	*event);

/*-------------------------------------------------------------------------------
 * BtlState type
 *
 *     State of the Bluetooth protocol stack and radio
 */
 typedef U8 BtlState;

/* Stack is not initialized */
#define BTL_STATE_NOT_INITIALIZED				(0)

/* The stack is initializing */
#define BTL_STATE_INITIALIZING					(1)

/* The stack has encountered an error while initializing the BT S/W */
#define BTL_STATE_INITIALIZATION_ERR  			(2)

/* Stack is initialized, BT radio is off => BT is not operational */
#define BTL_STATE_RADIO_OFF					(3)

/* The radio is initializing */
#define BTL_STATE_INITIALIZING_RADIO			(4)

/* The stack has encountered an error while initializing the radio H/W */
#define BTL_STATE_RADIO_INITIALIZATION_ERR	(5)

/* BT is fully operational (stack + radio) */
#define BTL_STATE_OPERATIONAL					(6)

/* The radio is initializing */
#define BTL_STATE_DEINITIALIZING_RADIO		(7)


/*---------------------------------------------------------------------------
 * BtlEventType type
 *
 *     All indications and confirmations are sent through a callback
 *     function. The event types are defined below.
 */
typedef U8 BtlEventType;

#define BTL_EVENT_INIT_COMPLETE			(1)
#define BTL_EVENT_RADIO_ON_COMPLETE		(2)
#define BTL_EVENT_RADIO_OFF_COMPLETE		(3)
#define BTL_EVENT_DEINIT_COMPLETE			(4)

/*---------------------------------------------------------------------------
 * BtlErrorType type
 */
typedef U8 BtlErrorType;

#define BTL_ERROR_TYPE_NONE				(0)
#define BTL_ERROR_TYPE_UNSPECIFIED		(1)


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlObject structure
 *
 *     Represents an object.
 */
typedef struct _BtlObject 
{
	/* 
	A null-terminated string representing the name of the object.
	In TX operation, this is the name of the transferred object as given to the peer 
    over the air.
	
    In RX operation, if the name is unknown set to NULL (name will be received from 
	peer), or set the name of the remote requested object.
    (i.e. - FTP profile: Sample_A.txt, set to the user requested object from peer.
          - OPP profile: set to NULL, since the default vcard object is given by peer.)
	*/
	const BtlUtf8		*objectName;
	
	/* 
	A null-terminated string representing the mime-type of the object, 
	for example: "text/x-vcard". This field is optional, so it can be set to NULL.
	In TX operation, this is the type of the transferred object.
	In RX operation, this field is ignored. 
	*/
	const char		*objectMimeType;

	/* Defines the location of the object, used to determine the relevant data in "location" */
	BtlObjectLocation objectLocation;

	union 
	{
		/* Relevant only if "objectLocation" is BTL_OBJECT_LOCATION_FS */
		struct 
		{
    		/* 
    		A null-terminated string representing the full path of the object in the FS.
    		In TX operation, this is the physical local full path of the transferred 
            object in FS.
			
            In RX operation, this is the physical local full path to store the transferred 
            object in FS. 
            (i.e. - FTP profile: C:\workingDirectory\Sample_B.txt, set to the stored file in FS.
                  - OPP profile: C:\currentFolder\Sample_C.txt, set to the stored file in FS.
			*/
			const BtlUtf8	*fsPath;
			
		} fsLocation;

		/* Relevant only if "objectLocation" is BTL_OBJECT_LOCATION_MEM */
		struct 
		{
    		/* The address of the object in memory */
			char	*memAddr;

			/* 
			The size of the object in memory.
			In TX operation, this is the actual size of the transferred object in memory.
			In RX operation, this is the max available space in memory in which the object
			will be stored. 
			*/
			U32			size;
			
		} memLocation;
        
    } location;

} BtlObject;

/*---------------------------------------------------------------------------
 * BtlObjProgressInd structure
 * 
 *     This structure is used during progress indications.
 */
struct _BtlObjProgressInd 
{
	/* Name of the object, null-terminated. */
	BtlUtf8					*name;

	/* Current position (in bytes) of the operation. */
   	U32						currPos;

	/* Max position (in bytes) of the operation. */
	U32						maxPos;
} ;

/*---------------------------------------------------------------------------
 * BtlEvent structure
 * 
 *     This structure is sent to the callback specified for BTL_Init. It contains
 *	   BTL events and associated information elements.
 */
struct _BtlEvent
{
	BtlEventType		type;
	BtStatus			status;
};

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * BTL_Init()
 *
 *	Brief:  
 *		Performs initialization of all basic BT components.
 *
 *	Description:
 *		Performs initialization of all basic BT components: BT stack, BTL layer...
 *		This function does not enable BT radio.
 *		It is usually called at system startup.
 *
 * Type:
 *		Synchronous or Asynchronous
 *
 * Parameters:
 *		callback [in] - all Common BTL events will be sent to this callback
 *
 * Generated Events:
 *      BTL_EVENT_INIT_COMPLETE
 *
 * Returns:
 *		BT_STATUS_SUCCESS - initialization is successfull.
 *
 *		BT_STATUS_PENDING - The initialization process started successfully. Results will be sent
 *								to callback
 *
 *		BT_STATUS_FAILED -  BTL failed initialization.
 */
BtStatus BTL_Init(BtlCallBack callback);


/*-------------------------------------------------------------------------------
 * BTL_Deinit()
 *
 *	Brief:  
 *		Performs deinitialization of all basic BT components.
 *
 *	Description:
 *		Performs deinitialization of all basic BT components: BT stack, BTL layer...
 *		This function disable BT radio, if it's enabled.
 *		It is usually called at system shutdown.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void
 *
 * Returns:
 *		BT_STATUS_SUCCESS - deinitialization is successfull.
 *
 *		BT_STATUS_FAILED -  BTL deinitialization failed.
 */
BtStatus BTL_Deinit(void);


/*-------------------------------------------------------------------------------
 * BTL_RadioOn()
 *
 *	Brief:  
 *		Enable the Bluetooth radio.
 *
 *	Description:
 *		Enable the Bluetooth radio.
 *		This function must be called before using the Bluetooth radio.
 *		When enable pending operation is completed successfully, BTL_EVENT_RADIO_ON_COMPLETE 
 *		event will be received.
 *		When enable pending operation is aborted, BTL_EVENT_RADIO_ON_COMPLETE event will
 *		not be received.
 *
 * Type:
 *		Asynchronous or Synchronous
 *
 * Parameters:
 *		None.
 *
 * Generated Events:
 *      BTL_EVENT_RADIO_ON_COMPLETE
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - Initialization was successful.  
 *			The bluetooth radio is already on.
 *
 *		BT_STATUS_PENDING - Initialization was started successfully.  The
 *         application will be notified when initialization is complete.  If 
 *         initialization is successful, BTEVENT_HCI_INTIALIZED will be 
 *         received. If initialization fails, BTEVENT_HCI_INIT_ERROR will be 
 *         received. Initialization will be retried BT_HCI_NUM_INIT_RETRIES
 *         times when an error occurs. After initialization has been retried 
 *         and has failed the maximum number of times, BTEVENT_HCI_FAILED will
 *         be received.
 *
 *		BT_STATUS_IMPROPER_STATE -  Enable the bluetooth radio failed since the stack
 *			is not initialized or the Radio is not off. 
 */
BtStatus BTL_RadioOn(void);


/*-------------------------------------------------------------------------------
 * BTL_RadioOff()
 *
 *	Brief:  
 *		Disable the Bluetooth radio.
 *
 *	Description:
 *		Disable the Bluetooth radio.
 *		After calling this function, Bluetooth radio can not be used anymore.
 *		When disable pending operation is completed, the BTL_EVENT_RADIO_OFF_COMPLETE 
 *		event will be received.
 *		This function must not be called during a callback.
 *
 * Type:
 *		Asynchronous or Synchronous
 *
 * Parameters:
 *		None.
 *
 * Generated Events:
 *      BTL_EVENT_RADIO_OFF_COMPLETE
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - De-Initialization was successful.  
 *			The bluetooth radio is already off.
 *
 *		BT_STATUS_PENDING - De-Initialization was started successfully.  The
 *			application will be notified by BTL_EVENT_RADIO_OFF_COMPLETE
 *			when de-initialization is complete. 
 *          
 *			In case the event BTL_EVENT_RADIO_ON_COMPLETE was previously generated
 *			(i.e. BTL_RadioOn initialization was complete) then:  
 *			if de-initialization is successful, BTEVENT_HCI_DEINITIALIZED will be 
 *			received. If de-initialization fails, BTEVENT_HCI_INIT_ERROR will be 
 *			received. 
 *
 *		BT_STATUS_IMPROPER_STATE -  De-Initialization failed to start. 
 *			Disable the bluetooth Radio failed since it is already de-initializing or 
 *			the stack is not initialized.
 */
BtStatus BTL_RadioOff(void);


/*-------------------------------------------------------------------------------
 * BTL_GetState()
 *
 *	Brief:  
 *		Reads the state of the stack and radio.
 *
 *	Description:
 *		Reads the state of the stack and radio.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		None
 *		
 * Returns:
 *		The state. If the value is BTL_STATE_OPERATIONAL the stack and radio are 
 *		initialized and ready for operation
 */
BtlState BTL_GetState(void);

 
/*-------------------------------------------------------------------------------
 * BTL_RegisterApp()
 *
 *	Brief:  
 *		Allocates a unique application handle for a new BTL application.
 *
 *	Description:
 *		Allocates a unique application handle for a new BTL application.
 *		The optional application handle can link between different modules.
 *		This function must be called by a new applicaton before any other BTL 
 *		API function, except BTL_Init.
 *		The allocated handle should be supplied in subsequent BTL API calls.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		appHandle [out] - allocated application handle.
 *
 *		appName [in] - Optional App name. May be 0. Useful for debugging purposes.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Application handle was allocated successfully.
 *
 *		BT_STATUS_FAILED -  Failed to allocate application handle.
 */
BtStatus BTL_RegisterApp(BtlAppHandle **appHandle, const BtlUtf8* appName);


/*-------------------------------------------------------------------------------
 * BTL_DeRegisterApp()
 *
 *	Brief:  
 *		Releases an application handle (previously allocated with BTL_RegisterApp).
 *
 *	Description:
 *		Releases an application handle (previously allocated with BTL_RegisterApp).
 *		An application should call this function when it completes using BTL services.
 *		Upon completion, the application handle is set to null in order to prevent 
 *		the application from an illegal attempt to keep using it.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		appHandle [in/out] - application handle.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Application handle was deallocated successfully.
 *
 *		BT_STATUS_FAILED -  Failed to deallocate application handle.
 */
BtStatus BTL_DeRegisterApp(BtlAppHandle **appHandle);


#endif /* __BTL_COMMON_H */


