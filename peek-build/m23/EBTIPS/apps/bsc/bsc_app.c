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
*   FILE NAME:      bsc_app.c
*
*   DESCRIPTION:    Implementation of Bluetooth System Coordinator sample
*                   application.
*
*   AUTHOR:         V. Abram
*
\*******************************************************************************/


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bttypes.h"
#include "osapi.h"
#include "debug.h"
#include "lineparser.h"
#include "btl_bsc.h"


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Structure to keep track of BSC configuration */
typedef struct
{
	BtlBscVoiceHandoverConfig               handoverConfig;
	BtlBscPauseMediaPlayerOnIncCallConfig   pauseMediaPlayerConfig;
} BscaConfig;


/********************************************************************************
 *
 * Function prototypes
 *
 *******************************************************************************/
void BSCA_Init(void);
void BSCA_Deinit(void);
void BSCA_SetVoiceHandoverConfig(BtlBscVoiceHandoverConfig *handoverConfig);
void BSCA_SetPauseMediaPlayerOnIncCallConfig(BtlBscPauseMediaPlayerOnIncCallConfig *pauseMediaPlayerConfig);


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

static BscaConfig bscaConfiguration =
{
    {FALSE},
    {FALSE, FALSE}
};


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

 /*-------------------------------------------------------------------------------
 * BSCA_Init()
 *
 *		Initialize application module.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	None.
 *
 * Returns:
 *		None.
 */
void BSCA_Init()
{
    Report(("BSCA_Init"));

    /* Set default configuration - disable handling */
    BSCA_SetVoiceHandoverConfig(&bscaConfiguration.handoverConfig);
    BSCA_SetPauseMediaPlayerOnIncCallConfig(&bscaConfiguration.pauseMediaPlayerConfig);
}

/*-------------------------------------------------------------------------------
 * BSCA_Deinit()
 *
 *		Deinitialize application module.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	None.
 *
 * Returns:
 *		None.
 */
void BSCA_Deinit()
{
    Report(("BSCA_Deinit"));

    /* Set default configuration - disable handling */
    BSCA_SetVoiceHandoverConfig(&bscaConfiguration.handoverConfig);
    BSCA_SetPauseMediaPlayerOnIncCallConfig(&bscaConfiguration.pauseMediaPlayerConfig);
}

/*-------------------------------------------------------------------------------
 * BSCA_SetVoiceHandoverConfig()
 *
 *		Processes command 'sethandoverconfig'.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	handoverConfig - pointer to handover configuration structure.
 *
 * Returns:
 *		None.
 */
void BSCA_SetVoiceHandoverConfig(BtlBscVoiceHandoverConfig *handoverConfig)
{
    Report(("BSCA_SetHandoverConfig"));

    BTL_BSC_SetVoiceHandoverConfig((const BtlBscVoiceHandoverConfig *)handoverConfig);

    return;
}

/*-------------------------------------------------------------------------------
 * BSCA_SetPauseMediaPlayerOnIncCallConfig()
 *
 *		Processes command 'setpausemediaplayerconfig'.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	pauseMediaPlayerConfig - pointer to pause media player configuration
 *          structure.
 *
 * Returns:
 *		None.
 */
void BSCA_SetPauseMediaPlayerOnIncCallConfig(BtlBscPauseMediaPlayerOnIncCallConfig *pauseMediaPlayerConfig)
{
    Report(("BSCA_SetPauseMediaPlayerOnIncCallConfig"));

    BTL_BSC_SetPauseMediaPlayerOnIncCallConfig((const BtlBscPauseMediaPlayerOnIncCallConfig *)pauseMediaPlayerConfig);
    
    return;
}
