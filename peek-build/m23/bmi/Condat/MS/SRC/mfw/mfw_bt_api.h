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
*   FILE NAME:      mfw_bt_api.h
*
*   DESCRIPTION:	This file defines the API between BMI (MMI) and BT applications
*
*   AUTHOR:         Uzi Doron
*
*	Rev 0.2
*
\*******************************************************************************/


#ifndef __MFW_BT_API
#define __MFW_BT_API
/**********************************************/
/*                                            */
/*                                            */
/*    from here on - events from BMI to BT    */
/*                                            */
/*                                            */
/**********************************************/

/*-------------------------------------------------------------------------------
 * BmiAvEvent type
 *
 *     Defines Audio/Video events initiated by MMI side.
 *     These events will be sent from BMI to BT
 *
 */
typedef enum
{
BMI_EVENT_USER_PLAY = 0x0001, /* user pressed Play   on handset */
BMI_EVENT_USER_STOP,               /* user pressed Stop   on handset */
BMI_EVENT_USER_PAUSE,            /* user pressed Pause  on handset */
BMI_EVENT_USER_RESUME,         /* user pressed Resume  on handset */ 
BMI_EVENT_TRACK_START,         /* a track is starting to play */
BMI_EVENT_TRACK_END              /* a track ended playing */
}BmiAvEvent;

/*-------------------------------------------------------------------------------
 * BmiEventCallback type
 *
 *     A callback function, implemented at BT application, that will be called by
 *     BMI to send events (of type BmiAvEvent) to BT. BMI will be able to send events
 *     after BT has registered such an event callback function
 *
 *     At BT side the function prototype will look like:
 *     void BT_NotifyBmiEvent(BmiAvEvent event);
 *
 *     NOTE: Since the callback is called from BMI context, the BT application should
 *     not spend considerable time in this function. In case significant processing 
 *     time and/or asynchronous activity is required as a result of calling this function,
 *     the BT application should switch to its own context to perform this activity.
 *     It must not block the BMI application.
*/
typedef void (*BmiEventCallback)(BmiAvEvent event);

/*-------------------------------------------------------------------------------
 * BMI_RegisterEventCallback()
 *
 *		BT application will call this function to register an event callback
 *      function with BMI.
 *
 * Parameters:
 *		eventCallback [in] - pointer of the callback function. if this parameter is
 *                           NULL (0), BMI should not try to make callbacks.
 *
 * Returns:
 *      void
 *
 */
void BMI_RegisterEventCallback(BmiEventCallback eventCallback);



/**********************************************/
/*                                            */
/*                                            */
/*    from here on - events from BT to BMI    */
/*                                            */
/*                                            */
/**********************************************/

/*-------------------------------------------------------------------------------
 * BtAppEvent type
 *
 *     Defines events initiated by BT side.
 *     These events will be sent from BT to BMI
 */
typedef enum 
{
BT_EVENT_USER_PLAY = 0x0044,    /* user pressed Play   on BT headset */
BT_EVENT_USER_STOP,                   /* user pressed Stop   on BT headset */
BT_EVENT_USER_PAUSE                 /* user pressed Pause  on BT headset */
}BtAppEvent;

/*-------------------------------------------------------------------------------
 * BMI_NotifyBtEvent()
 *
 *		Notify the BMI about an A/V event initiated at BT side.
 *      This function is implemented in the BMI application and used by the BT application.
 *
 *      NOTE: Since it is called from BT context, the BMI application should not spend
 *      considerable time in this function. In case significant processing time and/or 
 *      asynchronous activity is required as a result of calling this function, the BMI
 *      application should switch to its own context to perform this activity. It must
 *      not block the BT application.
 *
 * Parameters:
 *		event [in] - the reported event
 *
 * Returns:
 *      void
 *
 */
void BMI_NotifyBtEvent(BtAppEvent event);

/*-------------------------------------------------------------------------------
 * BMI_SwitchAudioToBtHeadset()
 *
 *		Connect or disconnect the audio output to BT headset.
 *      BT will call this function to let BMI know an A2DP link to a wireless headset
 *      has been connected (and is in Open state), or disconnected. 
 * 
 *      When link is connected, BMI will decide whether it wants to 
 *      direct the audio output to the wireless headset.
 *
 * Parameters:
 *		connected [in] - whether the link has just been connected or disconnected:
 *                       TRUE - connected
 *                       FALSE - disconnected
 *
 * Returns:
 *      when connect parameter is TRUE:
 *		- return TRUE when BMI approves and will use A2DP headset as the audio output
 *		- return FALSE when BMI rejects using A2DP headset as the audio output
 *
 *      when connect parameter is FALSE - returned value is ignored
 *
 */
BOOL BMI_SwitchAudioToBtHeadset(BOOL connected);
#endif /* __MFWE_BT_API */

