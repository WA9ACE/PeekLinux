/*******************************************************************************\
##                                                                             *
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION          *
##                                                                             *
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE        *
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE      *
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO      *
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT       *
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL        *
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC          *
##                                                                             *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      fms_api.h
*
*   BRIEF:          This file defines the API of the FM stack.
*
*   DESCRIPTION:    General
*
*					The fms_api layer defines the procedures used by the FM applications.
*                   
*   AUTHOR:   Keren Gazit
*
\*******************************************************************************/


#ifndef __FMS_API_H
#define __FMS_API_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "fm_types.h"
#include "fm_utils.h"
#include "fms_transport.h"
#include "bthal_vc.h"


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * FmEvent structure
 *
 *     Represents FM event.
 */
typedef struct _FmEvent 
{
	/* Defines the event that caused the callback */
    TIFM_U8	type;

	union 
	{
        struct 
		{
			/* The command done */
			TIFM_U8 cmd;
			/* The status of this command */
			FmStatus status;
			/* A value - needed only in specific commands.
			   When irrelevant will be FM_NO_VALUE */
			TIFM_U32 value;     
        } cmd_done;        		
        struct 
		{
			/* Status of the radio_tuned event */
			TIFM_U8 status;				
			/* The frequency that is currently tuned */
			TIFM_U32 frequency;     
        } radio_tuned;        		
        struct 
		{
			/* The frequency that is currently tuned */
			TIFM_U32 frequency;     
			/* The new ps Name */
			TIFM_U8 *psName;				
        } ps_changed;        		
        struct 
		{
			/* The current PI */
			TIFM_U16 pi;     
			/* The size of the new AF list */
			TIFM_U8 afListSize;
			/* The new AF list */
			TIFM_U32 *afList;
        } af_list_changed;        		
        struct 
		{
			/* The status of the AF jump */
			TIFM_U8 status;     
			/* The current PI */
			TIFM_U16 pi;
			/* The frequency before the jump */
			TIFM_U32 oldFreq;
			/* The frequency after the jump */
			TIFM_U32 newFreq;
        } af_jump;        		
        struct 
		{
			/* Indicates whether to add the new text to the old one or 
			   start a new clean text */
			TIFM_BOOL changed;     
			/* The length of the radio text */
			TIFM_U8 length;
			/* The radio text */
			TIFM_U8 *radioText;
        } radio_text;        		
        struct 
		{
			/* The new mode - Stereo/Mono */
			TIFM_U8 mode;
        } most_mode_changed;        		
        struct 
		{
			/* The length of the RDS data */
			TIFM_U16 len;
			/* A pointer to the RDS data - must be copied to an application buffer */
			TIFM_U8 *data;
        } raw_rds;        		

    } p;    

} FmEvent;

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * FmCallBack type
 *
 *     A function of this type is called to indicate FM events.
 */
typedef void (*FmCallBack)(const FmEvent *event);

/********************************************************************************
 *
 * Events sent to the application
 *
 *******************************************************************************/

#define FMEVENT_CMD_DONE			        1	/* Command done event */
#define FMEVENT_RADIO_TUNED			        2	/* Radio tuned event */
#define FMEVENT_PS_CHANGED			        3	/* Program Service name changed event */
#define FMEVENT_AF_LIST_CHANGED		        4	/* Alternate Frequency list changed event */
#define FMEVENT_AF_JUMP				        5	/* Alternate Frequency jump event */
#define FMEVENT_RADIO_TEXT			        6	/* Radio Text event */
#define FMEVENT_MOST_MODE_CHANGED           7	/* Mono/Stereo mode changed event */
#define FMEVENT_RAW_RDS                     8	/* Raw RDS event */
#define FMEVENT_RX_AUDIO_PATH_CHANGED       9	/* FM Rx audio path was changed */


/********************************************************************************
 *
 * FM commands definitions
 *
 *******************************************************************************/

#define FM_CMD_POWER_ON					0	/* Power On command */
#define FM_CMD_POWER_OFF				1	/* Power Off command */
#define FM_CMD_MOST_SET					2	/* Mono/Stereo Set command */
#define FM_CMD_BAND_SET					3	/* Band set command */
#define FM_CMD_MUTE						4	/* Mute command */
#define FM_CMD_VOLUME_SET				5	/* Volume set command */
#define FM_CMD_RDS_SET					6	/* RDS set command */
#define FM_CMD_TUNE						7	/* Tune command */
#define FM_CMD_RSSI_GET					8	/* RSSI get command */
#define FM_CMD_SEEK						9	/* Seek command */
#define FM_CMD_SET_AF					10	/* Set AF command */
#define FM_CMD_SET_STEREO_BLEND			11	/* Set stereo blend command */
#define FM_CMD_SET_DEEMPHASIS_MODE		12	/* Set deemphasis mode command */
#define FM_CMD_SET_RSSI_SEARCH_LEVEL	13	/* Set RSSI level command */
#define FM_CMD_SET_PAUSE_LEVEL			14	/* Set pause level command */
#define FM_CMD_SET_PAUSE_DURATION		15	/* Set pause duration command */
#define FM_CMD_SET_RDS_RBDS_MODE		16	/* Set RDS/RBDS mode command */
#define FM_CMD_MOST_GET					17	/* Get Mono/Stereo mode command */
#define FM_CMD_AUDIO_ENABLE				18	/* Audio Enable command */
#define FM_HANDLE_GEN_INT				19	/* Internal use */
#define FM_READ_RDS						20	/* Internal use */
#define FM_HANDLE_AF_JUMP				21	/* Internal use */
#define FM_HANDLE_STEREO_CHANGE			22	/* Internal use */
#define FM_HANDLE_HW_MAL				23	/* Internal use */
#define FM_HANDLE_TIMEOUT				24	/* Internal use */
/*******************************************************
****   IF YOU ADD ANOTHER OPERATION YOU MUST UPDATE  ***
****   FM_LAST_OP TO THE LAST OPERATION OPCODE		 ***
*******************************************************/
#define FM_LAST_OP						(FM_HANDLE_TIMEOUT)
/*******************************************************
*******************************************************/


/*****************************************************************
 *
 * Commands parameters definitions 
 *
 ****************************************************************/

#define FM_POWER_OFF					0	/* Parameter for internal FM_SetPower function */
#define FM_POWER_ON						1	/* Parameter for internal FM_SetPower function */
#define FM_STEREO_MODE					0	/* Parameter for FM_SetMoSt function */
#define FM_MONO_MODE					1	/* Parameter for FM_SetMoSt function */
#define FM_EUROPE_US_BAND				0	/* Parameter for FM_SetBand function */	
#define FM_JAPAN_BAND					1	/* Parameter for FM_SetBand function */	
#define FM_RF_MUTE_OFF					0	/* Parameter for FM_RfDependentMute function */
#define FM_RF_MUTE_ON					1	/* Parameter for FM_RfDependentMute function */
#define FM_MUTE_VOICE					0	/* Parameter for FM_MuteRadio function */	
#define FM_REDUCE_VOICE					1	/* Parameter for FM_MuteRadio function */
#define FM_UNMUTE_VOICE					2	/* Parameter for FM_MuteRadio function */
#define FM_RDS_OFF						0	/* Parameter for FM_SetRDS function */
#define FM_RDS_ON						1	/* Parameter for FM_SetRDS function */
#define FM_DIR_DOWN						0	/* Parameter for FM_Seek function */
#define FM_DIR_UP						1	/* Parameter for FM_Seek function */
#define FM_SEEK							0	/* Parameter for FM_Seek function */
#define FM_STOP_SEEK					1	/* Parameter for FM_Seek function */
#define FM_AF_OFF						0	/* Parameter for FM_SetAF function */
#define FM_AF_ON						1	/* Parameter for FM_SetAF function */
#define FM_STEREO_SWITCHED_BLEND		0	/* Parameter for FM_SetStereoBlend function */
#define FM_STEREO_SOFT_BLEND			1	/* Parameter for FM_SetStereoBlend function */
#define FM_50USEC_DEEMPHASIS			0	/* Parameter for FM_SetDeemphasisMode function */
#define FM_75USEC_DEEMPHASIS			1	/* Parameter for FM_SetDeemphasisMode function */
#define FM_RDS_MODE						0	/* Parameter for FM_SetRdsRbds function */
#define FM_RBDS_MODE					1	/* Parameter for FM_SetRdsRbds function */



/******************************************************
 *
 * AF jump status 
 *
 ******************************************************/

#define AF_JUMP_SUCCESS								0	/* AF jumped successfully */
#define AF_JUMP_FAILED_LIST_FINISHED				1	/* AF jump failed and no more alternative frequencies */
#define AF_JUMP_FAILED_LIST_NOT_FINISHED			2	/* AF jump failed but there are other AF to jump to */

/* Internal */
#define CHIP_ON			0
#define CHIP_OFF		1

/*-------------------------------------------------------------------------------
 * FmRxAudioPath type
 *
 *     Represents FM Rx audio path types.
 */
typedef TIFM_U8 FmRxAudioPath;

 /*****************************************************************
 *
 * Rx audio path routing definitions 
 *
 ****************************************************************/

#define FM_RX_AUDIO_PATH_OFF					    (0)	/* Audio is switched off */
#define FM_RX_AUDIO_PATH_HEADSET				    (1)	/* Audio is routed to wired headset */
#define FM_RX_AUDIO_PATH_HANDSET				    (2)	/* Audio is routed to handset */

/*-------------------------------------------------------------------------------
 * FmAppHandle structure
 *
 *     The application can register itself and recieve an application handle.
 *	   Not used currently !!!
 */
typedef struct _FmAppHandle
{
	/* allows an App Handle to be placed on a list of all App handles */
	TIFM_ListNode	node;							

	/* List of contexts associated with this App handle */
	TIFM_ListNode	*contextsList[1];

	char		appName[1];
} FmAppHandle;


/*-------------------------------------------------------------------------------
 * FmContext structure
 *
 *     Each application must create a context before starting to use the FM 
 */
typedef struct _FmContext 
{
	/* Indicates whether this context is already allocated or not */
	TIFM_BOOL		isAllocated;
	/* Callback for the application to handle events */
	FmCallBack 		fmCallcack;
	/* Application handle - can be 0 if not registered */
	FmAppHandle		*appHandle;	
	/* BTHAL VC context - can be 0, if not registered */
    BthalVcContext  *vcContext;

} FmContext;



/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

 /*-------------------------------------------------------------------------------
 * FM_Init()
 *
 * Brief:  
 *		Initializes FM module.
 *
 * Description:
 *    It is usually called at system startup.
 *    This function must be called by the system before any other FM function.
 *	  This function must be called after initializing the BT tasks!!!
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		FM_STATUS_SUCCESS - FM was initialized successfully.
 *
 *		FM_STATUS_FAILED -  FM failed initialization.
 */
FmStatus FM_Init(void);

/*-------------------------------------------------------------------------------
 * FM_Deinit()
 *
 * Brief:  
 *		Deinitializes FM module.
 *
 * Description:
 *		After calling this function, no FM function can be called.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void
 *
 * Returns:
 *		FM_STATUS_SUCCESS - FM was deinitialized successfully.
 *
 *		FM_STATUS_FAILED -  FM failed deinitialization.
 */
FmStatus FM_Deinit();

/*-------------------------------------------------------------------------------
 * FM_Create()
 *
 * Brief:  
 *		Allocates a unique FM context.
 *		CURRENTLY ONLY 1 FM CONTEXT CAN BE ALLOCATED!!!
 *
 * Description:
 *		This function must be called before any other FM function.
 *		The allocated context should be supplied in subsequent FM calls.
 *		The caller must also provide a callback function, which will be called 
 *		on FM events.
 *		The caller can provide an application handle which was previously allocated.
 *		appHandle IS NOT IN USE IN THE CURRENT IMPLEMENTATION.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		appHandle [in] - application handle. CURRENTLY NOT IN USE.
 *
 *		fmCallback [in] - all FM events will be sent to this callback.
 *		
 *		fmContext [out] - allocated FM context.
 *
 * Returns:
 *		FM_STATUS_SUCCESS - FM context was created successfully.
 *
 *		FM_STATUS_INVALID_PARM -  The FM context was already allocated.
 */
FmStatus FM_Create(FmAppHandle *appHandle, const FmCallBack fmCallback, FmContext **fmContext);

/*-------------------------------------------------------------------------------
 * FM_Destroy()
 *
 * Brief:  
 *		Releases the FM context (previously allocated with FM_Create).
 *
 * Description:
 *		An application should call this function when it completes using FM services.
 *		Upon completion, the FM context is set to null in order to prevent 
 *		the application from an illegal attempt to keep using it.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		fmContext [in/out] - FM context.
 *
 * Returns:
 *		FM_STATUS_SUCCESS - FM context was destroyed successfully.
 *
 *		FM_STATUS_INVALID_PARM -  FM context wasn't allocated.
 */
FmStatus FM_Destroy(FmContext **fmContext);

/*-------------------------------------------------------------------------------
 * FM_Enable()
 *
 * Brief:  
 *		Enable FM, called after FM_Create.
 *
 * Description:
 *		After calling this function, FM is powered on and ready for use.
 *		This procedure will initialize the chip (BT init) and only after 
 *		that will power on the FM. Powering on the FM sends the FM init script.
 *		After that the FM is ON and ready for any command.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 * Returns:
 *		FM_STATUS_PENDING - Power On the FM was successfully started.
 *		 Completion will be signaled via a call to the application callback
 *		 with command_done event.
 *
 *		FM_STATUS_FAILED_ALREADY_PENDING - Failed because enable command is already
 *		in progress.
 *
 *		FM_STATUS_INVALID_PARM - The FM context is not allocated.
 */
FmStatus FM_Enable(FmContext *fmContext);

/*-------------------------------------------------------------------------------
 * FM_Disable()
 *
 * Brief:  
 *		Disable FM, called before FM_Destroy.
 *
 * Description:
 *		If the FM is On it will be powered off.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 * Returns:
 *		FM_STATUS_PENDING - Power Off the FM was successfully started.
 *		 Completion will be signaled via a call to the application callback
 *		 with command_done event.
 *
 *		FM_STATUS_FAILED_ALREADY_PENDING - Failed because disable command is already
 *		in progress.
 *
 *		FM_STATUS_INVALID_PARM - The FM context is not allocated.
 */
FmStatus FM_Disable(FmContext *fmContext);

/*-------------------------------------------------------------------------------
 * FM_SetMoSt()
 *
 * Brief:  
 *		Set the Mono/Stereo mode.
 *
 * Description:
 *		Set the radio to Mono or Stereo mode.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 *		mode [in] - The mode to set: FM_STEREO_MODE or FM_MONO_MODE.
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. The new parameters are updated and the command will be done
 *		only once. 
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM - The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_SetMoSt(FmContext *fmContext, TIFM_U8 mode);

/*-------------------------------------------------------------------------------
 * FM_SetBand()
 *
 * Brief:  
 *		Set the radio band.
 *
 * Description:
 *		Choose between Europe/US band (87.5-108MHz) and Japan band (76-90MHz).
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 *		mode [in] - the mode to set: FM_EUROPE_US_BAND or FM_JAPAN_BAND.
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. The new parameters are updated and the command will be done
 *		only once. 
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM - The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_SetBand(FmContext *fmContext, TIFM_U8 mode);

/*-------------------------------------------------------------------------------
 * FM_MuteRadio()
 *
 * Brief:  
 *		Set the mute mode.
 *
 * Description:
 *		Sets the radio mute mode - mute, unmute or attenuate (reduce voice)
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 *		mode [in] - the mode to set: FM_MUTE_VOICE, FM_REDUCE_VOICE or FM_UNMUTE_VOICE.
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. The new parameters are updated and the command will be done
 *		only once. 
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM - The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_MuteRadio(FmContext *fmContext, TIFM_U8 mode);

/*-------------------------------------------------------------------------------
 * FM_RfDependentMute()
 *
 *		Enable/Disable the RF dependent mute feature.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 *		mode [in] - On/Off. Doesn't affect the regular mute state. 
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. The parameters were updated and the command will be done
 *		only once. When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM -  The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_RfDependentMute(FmContext *fmContext, TIFM_U8 mode);

/*-------------------------------------------------------------------------------
 * FM_SetVolume()
 *
 * Brief:  
 *		Set the gain level of the audio left & right channels.
 *
 * Description:
 *		Sets the radio gain level.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 *		gain [in] - the volume gain to set - Default value is 0x78b8, which is set to produce -1 dB 
 *		of the audio full scale when modulating 1 KHz tone at (delta)F = 105 KHz
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. The new parameters are updated and the command will be done
 *		only once. 
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM - The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_SetVolume(FmContext *fmContext, TIFM_U16 gain);

/*-------------------------------------------------------------------------------
 * FM_SetRDS()
 *
 * Brief:  
 *		Turns the RDS feature on or off.
 *
 * Description:
 *		Turns RDS ON/Off. When RDS is off no RDS information is collected.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 *		mode [in] - the RDS mode to set: FM_RDS_ON or FM_RDS_OFF.
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. The new parameters are updated and the command will be done
 *		only once. 
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM - The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_SetRDS(FmContext *fmContext, TIFM_U8 mode);

/*-------------------------------------------------------------------------------
 * FM_Tune()
 *
 * Brief:  
 *		Tunes the radio.
 *
 * Description:
 *		Tunes the radio to a specific frequency.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 *		freq [in] - the frequency to set in KHz.
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. The new parameters are updated and the command will be done
 *		only once. 
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM - The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_Tune(FmContext *fmContext, TIFM_U32 freq);

/*-------------------------------------------------------------------------------
 * FM_RssiGet()
 *
 * Brief:  
 *		Reads the current RSSI.
 *
 * Description:
 *		Read the current RSSI. The value will be received in the cmd_done event.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. It will be done	only once. 
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM - The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_RssiGet(FmContext *fmContext);

/*-------------------------------------------------------------------------------
 * FM_Seek()
 *
 * Brief: 
 *		Seeks the next good station or stop the current seek.
 *
 * Description:
 *		Seeks up/down for a frequency that its RSSI is higher than RSSI_SEARCH_LEVEL. 
 *		The seek command searches until it finds a good station, until it reaches 
 *		the band limit or until a stop command is sent.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 *		mode [in] - The seek mode: FM_SEEK or FM_STOP_SEEK.
 *
 *		direction [in] - The seek direction: FM_DIR_DOWN or FM_DIR_UP.
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *      will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_FAILED_ALREADY_PENDING - The seek command is already waiting
 *		for execution. It will be done only once.When done an event will 
 *		be sent to the application callback once
 *
 *		FM_STATUS_SUCCESS - This status can be received only when mode
 *		is stop_seek. It will be recieved if there is no seek to stop or
 *		if the seek wasn't started yet and it will be cancelled before starting.
 *
 *		FM_STATUS_INVALID_PARM -  The FM context wasn't allocated.
 *
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 *
 *		Only one cmd_done event will be sent with one of 3 statuses:
 *		FM_STATUS_SEEK_SUCCESS - The seek was successfully done.
 *		FM_STATUS_SEEK_REACHED_BAND_LIMIT - The seek is finished due
 *		to reaching the band limit.
 *		FM_STATUS_SEEK_STOPPED - The seek was stopped due to a stop_seek
 *		command.
 */
FmStatus FM_Seek(FmContext *fmContext, TIFM_U8 mode, TIFM_U8 direction);

/*-------------------------------------------------------------------------------
 * FM_SetAF()
 *
 * Brief: 
 *		Turns AF feature On or Off.		
 *
 * Description:
 *		Turns the 'Alternative Frequency' feature of the RDS on or off. 
 *		When On the AF information is collected and when low rssi is detected 
 *		a switch to an alternative frequency is done. 
 *		When off the information will be collected but no switch will be done.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 *		mode [in] - AF feature On or Off
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. The new parameters are updated and the command will be done
 *		only once. 
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM -  The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_SetAF(FmContext *fmContext, TIFM_U8 mode);

/*-------------------------------------------------------------------------------
 * FM_GetCurrentFreq()
 *
 * Brief: 
 *		Returns the current frequency.
 *
 * Description:
 *		Get the current tuned frequency.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 *		freq [out] - The current frequency the radio is tuned to.
 *
 * Returns:
 *		FM_STATUS_SUCCESS - Operation was successfully finished. The frequency
 *		is written in freq parameter.
 *
 *		FM_STATUS_INVALID_PARM -  The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_GetCurrentFreq(FmContext *fmContext, TIFM_U32 *freq);

/*-------------------------------------------------------------------------------
 * FM_GetCurrentPi()
 *
 * Brief: 
 *		Returns the current pi.
 *
 * Description:
 *		Get the current program indictor.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 *		pi [out] - The current pi  the radio is tuned to.
 *
 * Returns:
 *		FM_STATUS_SUCCESS - Operation was successfully finished. The pi
 *		is written in pi parameter.
 *
 *		FM_STATUS_INVALID_PARM -  The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 *
 *		FM_STATUS_FAIL_RDS_OFF - fail to get PI because rds is off.
 *
 *		FM_STATUS_FAIL_NO_VALUE_AVAILABLE - fail to get PI because PI no available yet.
 */
FmStatus FM_GetCurrentPi(FmContext *fmContext, TIFM_U16 *pi);


/*-------------------------------------------------------------------------------
 * Low level configurations:
 * The following functions are responsible for configurations of the radio. 
 * It should be used mostly at init if the default configuration needs to be changed.
*/

/*-------------------------------------------------------------------------------
 * FM_AudioEnable()
 *
 * Brief: 
 *		Enable the radio audio.
 *
 * Description:
 *		Enables radio audio. Low level configuration.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_FAILED_ALREADY_PENDING - This command is already waiting
 *		for execution. It will be done only once.
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM -  The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_AudioEnable(FmContext *fmContext);

/*-------------------------------------------------------------------------------
 * FM_SetStereoBlend()
 *
 * Brief: 
 *		Sets the stereo blend.
 *
 * Description:
 *		Sets the stereo blend - switched blend with hysteresis or soft blend.
 *		Low level configuration.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 * 		mode [in] - the mode to set: FM_STEREO_SWITCHED_BLEND or FM_STEREO_SOFT_BLEND.
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. The new parameters are updated and the command will be done
 *		only once. 
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM -  The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_SetStereoBlend(FmContext *fmContext, TIFM_U8 mode);

/*-------------------------------------------------------------------------------
 * FM_SetDeemphasisMode()
 *
 * Brief: 
 *		Choose de-emphasis filter.
 *
 * Description:
 *		Choose de-emphasis filter - 50usec or 75usec de-emphasis.
 *		Low level configuration.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 * 		mode [in] - the de-emphasis mode to set: FM_50USEC_DEEMPHASIS or FM_75USEC_DEEMPHASIS.
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. The new parameters are updated and the command will be done
 *		only once. 
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM -  The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_SetDeemphasisMode(FmContext *fmContext, TIFM_U8 mode);

/*-------------------------------------------------------------------------------
 * FM_RssiSearchLevel()
 *
 * Brief: 
 *		Sets the RSSI level.
 *
 * Description:
 *		Set the signal strength level that once reached will stop the auto search process.
 *		Low level configuration.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 * 		rssi_level [in] - -16 to +15. Each LSB = 1.5051 dBuV
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. The new parameters are updated and the command will be done
 *		only once. 
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM -  The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_RssiSearchLevel(FmContext *fmContext, TIFM_S8 rssi_level);

/*-------------------------------------------------------------------------------
 * FM_SetPauseLevel()
 *
 * Brief: 
 *		Sets the pause level.
 *
 * Description:
 *		A value that is less than pause level and longer or equal to pause duration 
 *		is considered as a 'silence'. FM_SetPauseLevel sets the pause detection level.
 *		Low level configuration.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 * 		pause_level [in] - 0=-6dB, 1=-9 dB etc. in 3dB steps up to 7=-27dB
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. The new parameters are updated and the command will be done
 *		only once. 
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM -  The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_SetPauseLevel(FmContext *fmContext, TIFM_U8 pause_level);

/*-------------------------------------------------------------------------------
 * FM_SetPauseDuration()
 *
 * Brief: 
 *		Sets the pause duration.
 *
 *
 * Description:
 *		A value that is less than pause level and longer or equal to pause duration 
 *		is considered as a 'silence'.
 *		FM_SetPauseDuration sets the duration that defines a pause.
 *		Low level configuration.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 * 		pause_duration [in] - Allows a range of 16 to 46 msec. duration is calculated by: 16 + Value*2 msec
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. The new parameters are updated and the command will be done
 *		only once. 
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM -  The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_SetPauseDuration(FmContext *fmContext, TIFM_U8 pause_duration);

/*-------------------------------------------------------------------------------
 * FM_SetRdsRbds()
 *
 * Brief: 
 *		Choose the RDS mode - RDS/RBDS
 *
 * Description:
 *		Sets the RDS mode. RDS/RBDS.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 * 		mode [in] - FM_RDS_MODE or FM_RBDS_MODE.
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_PENDING_UPDATE_CMD_PARAMS - This command is already waiting
 *		for execution. The new parameters are updated and the command will be done
 *		only once. 
 *		When done an event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM -  The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_SetRdsRbds(FmContext *fmContext, TIFM_U8 mode);

/*-------------------------------------------------------------------------------
 * FM_GetMoSt()
 *
 * Brief: 
 *		Read the current Mono/Stereo mode.
 *
 * Description:
 *		Read the Mono Stereo mode. The value will be received in the cmd_done event.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a cmd_done event to the application callback.
 *
 *		FM_STATUS_FAILED_ALREADY_PENDING - This command is already waiting
 *		for execution. It will be done only once.
 *		When done an event will 
 *		be sent to the application callback once
 *
 *		FM_STATUS_INVALID_PARM -  The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because the FM is not On.
 *									 FM_enable command must be sent before any other command.
 */
FmStatus FM_GetMoSt(FmContext *fmContext);

/*-------------------------------------------------------------------------------
 * FM_SetRxAudioPath()
 *
 * Brief: 
 *		Sets FM Rx audio path.
 *
 * Description:
 *		Sets the FM Rx audio path within the system: handset, wired headset or off.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fmContext [in] - FM context.
 *
 *      audioPath [in] - possible variant of Rx audio path, i.e. handset, wired
 *          headset, off.
 *
 * Returns:
 *		FM_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a FMEVENT_RX_AUDIO_PATH_CHANGED event to the
 *         application callback.
 *
 *		FM_STATUS_FAILED_ALREADY_PENDING - This command is already waiting
 *		   for execution. It will be done only once.
 *		   When done, the event will be sent to the application callback once.
 *
 *		FM_STATUS_INVALID_PARM -  The FM context wasn't allocated.
 *
 *		FM_STATUS_FAILED_FM_NOT_ON - The command couldn't be executed because
 *         the FM is not On. FM_enable command must be sent before any other
 *         command.
 */
FmStatus FM_SetRxAudioPath(FmContext *fmContext,
                           FmRxAudioPath audioPath);

/* Internal callback */
void transportStatusCallback(TiTransportFmNotificationType notification);

#endif
