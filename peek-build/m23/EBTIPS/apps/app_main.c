/*******************************************************************************\
*                                                                       		*
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
*   FILE NAME:      app_main.c
*
*   DESCRIPTION:	This file contains the implementation of the app_main module
*					functionality - main file for the BT applications.
*
*   AUTHOR:         V. Abram
*
\*******************************************************************************/

/* BTIPS Includes */
#include "me.h"
#include "osapi.h"
#include "app_main.h"
#include "btl_common.h"
#include "bthal_btdrv.h"
#include "bthal_pm.h"
#include "btl_commoni.h"
#include "lineparser.h"
#include "EBTIPS_version.h"
#include "fms_api.h"

/* Internal functions prototypes */
static void AppBtlCallBack (const BtlEvent *event);
static void MAINA_ProcessUserAction(U8 *msg);
static const char *AppName(BtlModuleType moduleType);
static BOOL APP_MAIN_BT_ON (void);
static BOOL APP_MAIN_BT_OFF (void);


#define MAIN_COMMANDS_NUM 2

typedef BOOL (*FuncType)(void);

typedef struct
{
	char 		funcName[LINE_PARSER_MAX_STR_LEN];
	FuncType    funcPtr;
} _mainCommands;

/* BMG commands array */
static const  _mainCommands mainCommands[] =  { {"bton",	    APP_MAIN_BT_ON}, 		       
												{"btoff",      APP_MAIN_BT_OFF}		
											};


/* Declaration of static variables */
BOOL isBtRadioEnabled = FALSE;	/* To check at start-up */

/* External functions */
extern void BMGA_Init(void); 
extern void BMGA_Deinit(void); 
extern void BMGA_ProcessUserAction(U8 *msg);

extern void BPPA_Init(void);
extern void BPPA_Deinit(void);
extern void BPPA_ProcessUserAction(U8 *msg);

extern void HIDA_Init(void); 
extern void HIDA_Deinit(void); 
extern void HIDA_ProcessUserAction(U8 * msg);

extern void MDGA_Init(void); 
extern void MDGA_Deinit(void); 
extern void MDGA_UI_ProcessUserAction(U8 *msg);

extern BOOL PBAPA_Init(void);
extern BOOL PBAPA_Deinit(void);
extern void PBAPA_ProcessUserAction(U8 *msg);

extern BOOL SAPA_Init(void);
extern BOOL SAPA_Deinit(void);
extern void SAPA_ProcessUserAction(U8 *msg);

extern void VGA_Init(void);
extern void VGA_Deinit(void);
extern void VGA_ProcessUserAction(U8 *msg);

extern void FTPSA_ProcessUserAction(U8 *msg);
extern void FTPSA_Init(void);
extern void FTPSA_Deinit(void);

extern void FTPCA_ProcessUserAction(U8 *msg);
extern void FTPCA_Init(void);
extern void FTPCA_Deinit(void);

extern void A2DPA_Init(void);
extern void A2DPA_Deinit(void);
extern void A2DPA_ProcessUserAction(U8 *msg);

extern void AVRCPTGA_UI_ProcessUserAction(U8 *msg);
extern void AVRCPTGA_Init(void); 
extern void AVRCPTGA_Deinit(void); 

extern void OPPSA_Init(void); 
extern void OPPSA_Deinit(void); 
extern void OPPSA_UI_ProcessUserAction(U8 * msg);

extern void OPPCA_Init(void); 
extern void OPPCA_Deinit(void); 
extern void OPPCA_UI_ProcessUserAction(U8 * msg);

extern void BIPIA_Init(void); 
extern void BIPIA_Deinit(void); 
extern void BIPIA_UI_ProcessUserAction(U8 * msg);

extern void BIPRA_Init(void); 
extern void BIPRA_Deinit(void); 
extern void BIPRA_UI_ProcessUserAction(U8 * msg);

extern void SPPA_Init(void); 
extern void SPPA_Deinit(void); 
extern void SPPA_UI_ProcessUserAction(U8 * msg);

extern void BSCA_Init(void); 
extern void BSCA_Deinit(void); 
extern void BSCA_UI_ProcessUserAction(U8 * msg);

extern void L2CAPA_THROUGHPUT_ProcessUserAction(U8 *msg);


extern void FMRADIOA_Init();
extern void FMRADIOA_Deinit();
extern void FMRADIOA_UI_ProcessUserAction(U8 * msg);

/* Functions definitions */

/*******************************************************************************\
*  FUNCTION:	APP_Init
*                                                                         
*  PARAMETERS:	None.
*                                                                         
*  DESCRIPTION:	Initializes application environment and BT stack.              
*                                                                         
*  RETURNS:		None.
\*******************************************************************************/
void APP_Init()
{
	Report(("APP_MAIN: Starting APP_Init"));
		
	/* Print Anchor version */
	Report(("Anchor Version - %d.%d.%d",
			ANCHOR_VERSION_X,
			ANCHOR_VERSION_Y,
			ANCHOR_VERSION_Z));
	BTHAL_OS_Sleep(100);  
    /* Initialize BTIPS basic components: BT stack, BTL layer... */
	Report(("APP_MAIN: Starting BTL_Init"));	
if (BT_STATUS_SUCCESS != BTL_Init(AppBtlCallBack))
		{
			Report(("Fatal Error: Could not initialize the BTIPS basic components\n"));
		}


	Assert(FM_STATUS_SUCCESS == FM_Init());


    /* Initialize application modules */
	BMGA_Init();
	BSCA_Init();
	SPPA_Init();
	PBAPA_Init();
	BIPIA_Init();
	BIPRA_Init();
	MDGA_Init();
	A2DPA_Init();
	AVRCPTGA_Init();
	BPPA_Init();
   	HIDA_Init();
	FTPSA_Init();
	FTPCA_Init();
	OPPSA_Init();
	OPPCA_Init(); 
	VGA_Init();
	SAPA_Init();

	FMRADIOA_Init();

}


/*******************************************************************************\
*  FUNCTION:	APP_Deinit
*                                                                         
*  PARAMETERS:	None.
*                                                                         
*  DESCRIPTION:	Initializes application environment and BT stack.              
*                                                                         
*  RETURNS:		None.
\*******************************************************************************/

void APP_Deinit()
{
	/* Deinitilization should mirror the init sequence in reverse:
     * deinitialize BT radio, if it was enabled, applications, and BTIPS basic
     * components */
   if (isBtRadioEnabled)
    {
		BTL_RadioOff();
    }

	FMRADIOA_Deinit();
	SPPA_Deinit();
	VGA_Deinit();
	OPPCA_Deinit();
	OPPSA_Deinit();
	FTPSA_Deinit();
	FTPCA_Deinit();
	HIDA_Deinit();
	BPPA_Deinit();
	PBAPA_Deinit();
	BIPIA_Deinit();
	BIPRA_Deinit();
	AVRCPTGA_Deinit();
	A2DPA_Deinit();
	MDGA_Deinit();
	SAPA_Deinit();
	BSCA_Deinit();
	BMGA_Deinit();

	BTL_Deinit();
}

static void AppBtlCallBack(const BtlEvent *event)
{
	switch (event->type)
	{
		case BTL_EVENT_RADIO_OFF_COMPLETE:
			Report(("APP MAIN: AppBtlCallBack: RADIO OFF COMPLETE"));
			break;
	
		case BTL_EVENT_RADIO_ON_COMPLETE:
			Report(("APP MAIN: AppBtlCallBack: RADIO ON COMPLETE"));
			break;
	
	};	
}


/*******************************************************************************\
*  FUNCTION:		APP_ProcessUserAction
*                                                                         
*  PARAMETERS:	U8 - application message receiver ID.
*               U8 *msg - pointer to received message.
*                                                                         
*  DESCRIPTION:	Processes received user action.              
*                                                                         
*  RETURNS:		None.
\*******************************************************************************/
void APP_ProcessUserAction(U8 appMsgRcvId, U8 *msg)
{
	LINE_PARSER_STATUS 	status;	
	BtlModuleType 	    moduleType;
	
	status = LINE_PARSER_ParseLine(msg," ");
	
	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("APP MAIN: Wrong command line"));
		return;
	}

	if ( (char)appMsgRcvId != '-')
	{
		Report(("APP MAIN: Illegal command: must start with '-'"));
		return;
	}
	
	status = LINE_PARSER_GetNextModuleType(&moduleType);

	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("APP MAIN: Illegal module type"));
		return;
	}

	Report(("APP MAIN: Primitive for application %s", AppName(moduleType)));

	switch (moduleType)
	{
		case BTL_MODULE_TYPE_SPP:
			SPPA_UI_ProcessUserAction(msg);
			break;

		case BTL_MODULE_TYPE_MAIN:
			MAINA_ProcessUserAction(msg);
			break;

		case BTL_MODULE_TYPE_AVRCPTG:
			AVRCPTGA_UI_ProcessUserAction(msg);
			break;

		case BTL_MODULE_TYPE_BMG:
			BMGA_ProcessUserAction(msg);
			break;

		case BTL_MODULE_TYPE_BPPSND:
			BPPA_ProcessUserAction(msg);
			break;

		case BTL_MODULE_TYPE_MDG:
			MDGA_UI_ProcessUserAction(msg);
			break;

		case BTL_MODULE_TYPE_PBAPS:
			PBAPA_ProcessUserAction(msg);
			break;

		case BTL_MODULE_TYPE_OPPC:
			OPPCA_UI_ProcessUserAction(msg);
			break;

		case BTL_MODULE_TYPE_OPPS:
			OPPSA_UI_ProcessUserAction(msg);
			break;

		case BTL_MODULE_TYPE_VG:
			VGA_ProcessUserAction(msg);
			break;

		case BTL_MODULE_TYPE_HID:
			HIDA_ProcessUserAction(msg);
			break;	

		case BTL_MODULE_TYPE_FTPS:
			FTPSA_ProcessUserAction(msg);
			break;

		case BTL_MODULE_TYPE_FTPC:
			FTPCA_ProcessUserAction(msg);
			break;

		case BTL_MODULE_TYPE_A2DP:
			A2DPA_ProcessUserAction(msg);
			break;
			
		case BTL_MODULE_TYPE_SAPS:
			SAPA_ProcessUserAction(msg);
			break;

		case BTL_MODULE_TYPE_BIPINT:
			BIPIA_UI_ProcessUserAction(msg);
			break;

		case BTL_MODULE_TYPE_BIPRSP:
			BIPRA_UI_ProcessUserAction(msg);
			break;
			
		case BTL_MODULE_TYPE_BSC:
			BSCA_UI_ProcessUserAction(msg);
			break;
			
		case BTL_MODULE_TYPE_L2CAP_THROUGHPUT:
			L2CAPA_THROUGHPUT_ProcessUserAction(msg);
			break;


		case BTL_MODULE_TYPE_FMRADIO:
			  FMRADIOA_UI_ProcessUserAction(msg);
			  break;
			
	    default:
		    Report(("APP MAIN: Error Sample App ID!"));
		    break;
	}
}

/*******************************************************************************\
*  FUNCTION:		APP_ProcessEvent
*                                                                         
*  PARAMETERS:	ULONG opc - opcode of received signal;
*				void *data - pointer to received data.
*                                                                         
*  DESCRIPTION:	Processes received signal.              
*                                                                         
*  RETURNS:		None.
\*******************************************************************************/
void APP_ProcessEvent(U32 opc, void *data)
{	
	U32 appMsgRcvId = opc & APP_MSG_RCV_ID_MASK;		/* Receiver ID */

	/* Route messages to appropriate receiver - sample application */
	switch (appMsgRcvId)
	{
		default:
			Report(("APP MAIN: Signals should not be used!"));
			break;			
	}
}

/*******************************************************************************\
*  FUNCTION:	MAINA_ProcessUserAction
*                                                                         
*  PARAMETERS:	msg - buffer holding primitive string.
*                                                                         
*  DESCRIPTION:	Processes primitives for Main Sample APP.
*                                                                         
*  RETURNS:		None.
\*******************************************************************************/
void MAINA_ProcessUserAction(U8 *msg)
{
	int i;
	char command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS lineStatus;

	lineStatus = LINE_PARSER_GetNextStr((U8 *)(command), LINE_PARSER_MAX_STR_LEN);
	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("Wrong MAIN command. Can't parse line"));
		return;
	}

	/* Look for the correct function */
	for(i = 0; i < MAIN_COMMANDS_NUM; i++)
	{
		if (OS_StrCmp(command, mainCommands[i].funcName) == 0)
		{
			mainCommands[i].funcPtr();
			return;
		}
	}
	Report(("Wrong MAIN command"));
	}


/*---------------------------------------------------------------------------
 *            APP_MAIN_BT_ON
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enable BT radio.		
 *
 * Return:    TRUE if Enabling BT radio is successful, FALSE otherwise.
 *
 */
BOOL APP_MAIN_BT_ON (void)
{
	
	Report(("APP_MAIN_BT_ON"));
	if (FALSE == isBtRadioEnabled)
	{
		isBtRadioEnabled = TRUE;
		UI_DISPLAY(("APP MAIN: Enabling BT radio"));
		BTL_RadioOn();
	}
	else
	{
		Report(("APP MAIN: BT radio is already enabled"));
	}
	return(TRUE);
}


/*---------------------------------------------------------------------------
 *            APP_MAIN_BT_OFF
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disable BT radio.		
 *
 * Return:    TRUE if Disabling BT radio is successful, FALSE otherwise.
 *
 */
BOOL APP_MAIN_BT_OFF (void)
{

	if (TRUE == isBtRadioEnabled)
	{
		UI_DISPLAY(("APP MAIN: Disabling BT radio"));
		BTL_RadioOff();
		isBtRadioEnabled = FALSE;
	}
	else
	{
		Report(("APP MAIN: BT radio is already disabled"));
	}
	return(TRUE);
}

/*******************************************************************************\
*  FUNCTION:	AppName
*                                                                         
*  DESCRIPTION:	Return a pointer to the name of the current sample application.              
*                                                                         
*  RETURNS:		ASCII String pointer.
\*******************************************************************************/
static const char *AppName(BtlModuleType moduleType)
{
	switch (moduleType) 
	{

		case BTL_MODULE_TYPE_MAIN:
			return "MAIN";
		case BTL_MODULE_TYPE_BMG:
			return "BMG";
		case BTL_MODULE_TYPE_SPP:
			return "SPP";			
		case BTL_MODULE_TYPE_OPPC:
			return "OPPC";
		case BTL_MODULE_TYPE_OPPS:
			return "OPPS";
		case BTL_MODULE_TYPE_BPPSND:
			return "BPP";
	    	case BTL_MODULE_TYPE_BIPINT:
			return "BIPINT";
	    	case BTL_MODULE_TYPE_BIPRSP:
			return "BIPRSP";
		case BTL_MODULE_TYPE_PBAPS:
			return "PBAP";
		case BTL_MODULE_TYPE_PAN:
			return "PAN";
		case BTL_MODULE_TYPE_SAPS:
			return "SAPS";
		case BTL_MODULE_TYPE_AVRCPTG:
			return "AVRCP";
		case BTL_MODULE_TYPE_FTPS:
			return "FTPS";
		case BTL_MODULE_TYPE_FTPC:
			return "FTPC";
		case BTL_MODULE_TYPE_VG:
			return "VG";
		case BTL_MODULE_TYPE_RFCOMM:
			return "RFCOMM";
		case BTL_MODULE_TYPE_HID:
			return "HID";
		case BTL_MODULE_TYPE_A2DP:
			return "A2DP";
		case BTL_MODULE_TYPE_MDG:
			return "MDG";
		case BTL_MODULE_TYPE_BSC:
			return "BSC";
		case BTL_MODULE_TYPE_L2CAP:
			return "L2CAP";
		case BTL_MODULE_TYPE_L2CAP_THROUGHPUT:
			return "L2CAP_THROUGHPUT";
		case BTL_MODULE_TYPE_FMRADIO:
			return "FMRADIO";

		default:
   			return "Unknown";
    }
}


U8 APP_HexDigitToInt(const char _val)
{
	if ((_val >= '0') && (_val <= '9'))
	{
		return ASCII_TO_INT(_val);
	}
	else if ((_val >= 'A') && (_val <= 'F'))
	{
		return (10 + (_val - 'A'));
	}
	else if ((_val >= 'a') && (_val <= 'f'))
	{
		return (10 + (_val - 'a'));
	}
	else
	{
		/* Set to invalid application */
		return 255;
	}
}

