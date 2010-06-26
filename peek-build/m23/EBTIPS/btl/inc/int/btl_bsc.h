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
*   FILE NAME:		btl_bcs.h
*
*   DESCRIPTION:	Type definitions and function prototypes for the btl_bcs.c
*					module implementing BTL Complex Scenarios.
*
*   AUTHOR:			V. Abram
*
\*******************************************************************************/

#ifndef __BTL_BSC_H
#define __BTL_BSC_H
 
/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bttypes.h"
#include "avrcp.h"
#include "btl_avrcptg.h"


 /********************************************************************************
 *
 * Types
 *
 *******************************************************************************/
 
/*---------------------------------------------------------------------------
 * BtlBscEventType type
 *
 *	   Represents possible types of events which may be received by the BSC
 *	   from different modules.
 */
typedef U8 BtlBscEventType;

/* A module - sender of event established SLC */
#define BTL_BSC_EVENT_SLC_CONNECTED	            (1)

/* A module - sender of event disconnected SLC */
#define BTL_BSC_EVENT_SLC_DISCONNECTED		    (2)

/* A2DP module started streaming music */
#define BTL_BSC_EVENT_AUDIO_STREAMING_STARTED   (3)

/* A2DP module stopped streaming music */
#define BTL_BSC_EVENT_AUDIO_STREAMING_STOPPED   (4)

/* Audio gateway received indication of incoming call */
#define BTL_BSC_EVENT_INCOMING_CALL	            (5)

/* The call in audio gateway is finished */
#define BTL_BSC_EVENT_CALL_FINISHED	            (6)

/*-------------------------------------------------------------------------------
 * BtlBscEventSource type
 *
 *     Defines the source of event sent to the BSC state machine.
 */
typedef U8 BtlBscEventSource;

#define BTL_BSC_EVENT_SOURCE_A2DP		        (0x00)
#define BTL_BSC_EVENT_SOURCE_AVRCPTG		    (0x01)
#define BTL_BSC_EVENT_SOURCE_AG		            (0x02)
#define BTL_BSC_EVENT_SOURCE_VG		            (BTL_BSC_EVENT_SOURCE_AG)

#define BTL_BSC_MAX_NUM_OF_WATCHED_MODULES      (BTL_BSC_EVENT_SOURCE_AG + 1)

/*-------------------------------------------------------------------------------
 * BtlBscWatchedInfo structure
 *
 *	   Represents information which needs for watching of a module.
 */
typedef struct _BtlBscWatchedInfo
{
        void            *context;       /* Possible additional info (BTL context
                                         * of event sender...) */
        U32             channel;        /* Possible additional info (channel) */
	    BD_ADDR         bdAddr;     	/* BD ADDR of the remote device */
        BOOL            isConnected;    /* Shows whether this info is active */
        
} BtlBscWatchedInfo;

/*-------------------------------------------------------------------------------
 * BtlBscEvent structure
 *
 *	   Represents event used by the BSC state machine.
 */
typedef struct _BtlBscEvent
{
    /* Info for managing list of these structures */
    ListEntry           node;

	/* Event's source */
	BtlBscEventSource   source;

    /* Event's type */
    BtlBscEventType     type;

    /* Reflects whether BD ADDR in member 'info' is valid */
    BOOL                isBdAddrValid;

	/* Events data */
    BtlBscWatchedInfo   info;

} BtlBscEvent;

/*-------------------------------------------------------------------------------
 * BtlBscVoiceHandoverConfig structure
 *
 *	   Represents configuration of the voice handover.
 */
typedef struct _BtlBscVoiceHandoverConfig
{
    /* If enabled, voice call handover is performed automatically without user
     * interaction */
    BOOL    autoVoiceCallHandover;
    
} BtlBscVoiceHandoverConfig;

/*-------------------------------------------------------------------------------
 * BtlBscPauseMediaPlayerOnIncCallConfig structure
 *
 *	   Represents configuration of pausing Media Player on arriving of incoming
 *     call.
 */
typedef struct _BtlBscPauseMediaPlayerOnIncCallConfig
{
    /* If enabled, on arriving of incoming call, event
     * AVRCP_BSC_EVENT_RECOMMENDED_PAUSE will be sent to Media Player which is
     * streaming music via A2DP */
    BOOL    pausePlayerOnIncCall;

    /* If enabled, on arriving of incoming call, event
     * AVRCP_BSC_EVENT_RECOMMENDED_PAUSE will be sent to Media Player, which is
     * streaming music via A2DP to the stereo headset, even if HFP SLC is
     * established with another headset */
    BOOL    pausePlayerOnIncCallToAnotherHeadset;

} BtlBscPauseMediaPlayerOnIncCallConfig;


/********************************************************************************
 *
 * External function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_BSC_SendRecommendedEvent()
 *
 *		Initializes BTL BSC module and allocates required resources.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	avrcptgContext - context of the AVRCPTG application.
 *
 *      event - recommended event to be sent to the application.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the 'pause' event was successfully sent to the
 *          Media Player.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
extern BtStatus BTL_AVRCPTG_BSC_SendRecommendedEvent(BtlAvrcptgContext *avrcptgContext,
                                                     AvrcpEvent event);

 
 /********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_BSC_Init()
 *
 *		Initializes BTL BSC module and allocates required resources.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	None.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the module was successfully initialized.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the initialization failed.
 */
BtStatus BTL_BSC_Init(void);

/*-------------------------------------------------------------------------------
 * BTL_BSC_Deinit()
 *
 *		Deinitializes BTL BSC module and frees allocated resources.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	None.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the module was successfully deinitialized.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the initialization failed.
 */
BtStatus BTL_BSC_Deinit(void);

/*-------------------------------------------------------------------------------
 * BTL_BSC_SetVoiceHandoverConfig()
 *
 *     	Sets configuration of the voice handover.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	config [in] - pointer to voice handover configuration structure.
 *
 * Returns:
 *     	None.
 */
void BTL_BSC_SetVoiceHandoverConfig(const BtlBscVoiceHandoverConfig *config);

/*-------------------------------------------------------------------------------
 * BTL_BSC_GetVoiceHandoverConfig()
 *
 *     	Gets configuration of the voice handover.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	config [out] - pointer to voice handover configuration structure.
 *
 * Returns:
 *     	None.
 */
void BTL_BSC_GetVoiceHandoverConfig(BtlBscVoiceHandoverConfig *config);

/*-------------------------------------------------------------------------------
 * BTL_BSC_SetPauseMediaPlayerOnIncCallConfig()
 *
 *     	Sets configuration of pausing Media Player on arriving of incoming call.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	config [in] - pointer to structure with configuration of pausing Media
 *          Player on arriving of incoming call.
 *
 * Returns:
 *     	None.
 */
void BTL_BSC_SetPauseMediaPlayerOnIncCallConfig(const BtlBscPauseMediaPlayerOnIncCallConfig *config);

/*-------------------------------------------------------------------------------
 * BTL_BSC_GetPauseMediaPlayerOnIncCallConfig()
 *
 *     	Gets configuration of pausing Media Player on arriving of incoming call.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	config [out] - pointer to structure with configuration of pausing Media
 *          Player on arriving of incoming call.
 *
 * Returns:
 *     	None.
 */
void BTL_BSC_GetPauseMediaPlayerOnIncCallConfig(BtlBscPauseMediaPlayerOnIncCallConfig *config);

/*-------------------------------------------------------------------------------
 * BTL_BSC_SendEvent()
 *
 *     	Sends event to BTL_BSC module.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	None.
 *
 * Returns:
 *     	None.
 */
void BTL_BSC_SendEvent(const BtlBscEvent *event);
			    

#endif /* __BTL_BSC_H */

