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
*   FILE NAME:      oppc_ui.c
*
*   DESCRIPTION:    This file contains the implementation of the OPPC sample 
*					application user I/F.
*
*   AUTHOR:         Ronen Levy
*
\*******************************************************************************/

#include "btl_config.h"
#include "osapi.h"
#if BTL_CONFIG_OPP == BTL_CONFIG_ENABLED


#include "btl_common.h"
#include "debug.h"

#include "../app_main.h"

#include "btl_opps.h"
#include "btl_oppc.h"
#include "bthal_config.h"
#include "bthal_fs.h"
#include "lineparser.h"


/*******************************************************************************
 *
 * Macro definitions
 *
 ******************************************************************************/

/* This is the FS directory location for all pushed objects */
#define OPPCA_UI_PUSH_OBJ_DEFAULT_DIR        "/QbInbox"

#define OPPCA_UI_CHECK_OBJ_LOCATION(objectLocation)                  \
          ((objectLocation == BTL_OBJECT_LOCATION_FS) ||         \
              (objectLocation == BTL_OBJECT_LOCATION_MEM))

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * OppcaUiCommandId type
 *
 *     Defines the OPPC user commands
 */
typedef U8 OppcaUiCommandId;


#define OPPCA_UI_ABORT                     0
#define OPPCA_UI_GET_SECURITY              1
#define OPPCA_UI_SET_SECURITY              2
#define OPPCA_UI_ENABLE                    3
#define OPPCA_UI_DISABLE                   4
#define OPPCA_UI_CREATE                    5
#define OPPCA_UI_DESTROY                   6       
#define OPPCA_UI_CONNECT                   7
#define OPPCA_UI_DISCONNECT                8    
#define OPPCA_UI_BASIC_PUSH                9
#define OPPCA_UI_BASIC_PULL                10
#define OPPCA_UI_BASIC_EXCHANGE            11
#define OPPCA_UI_ENCAPSULATE_PUSH          12
#define OPPCA_UI_ENCAPSULATE_PULL          13
#define OPPCA_UI_ENCAPSULATE_EXCHANGE      14
#define OPPCA_UI_SET_OBJ_LOCATION          15


typedef struct _OppcaUiCommand
{
	char 	            cmdName[LINE_PARSER_MAX_STR_LEN];
	OppcaUiCommandId    cmdId;
} OppcaUiCommand;


/********************************************************************************
 *
 * Function prototypes
 *
 *******************************************************************************/

void OPPCA_UI_ProcessUserAction(U8 *msg);



/********************************************************************************
 *
 * External functions
 *
 *******************************************************************************/
extern void OPPCA_Init(void);
extern void OPPCA_Deinit(void);
extern void OPPCA_Create(void);
extern void OPPCA_Destroy(void);
extern void OPPCA_Enable(void);
extern void OPPCA_Disable(void);
extern void OPPCA_AbortClient(void);
extern void OPPCA_Connect(BD_ADDR *bd_addr);
extern void OPPCA_Disconnect(void);
extern BOOL OPPCA_GetSecurityLevel(void);
extern BOOL OPPCA_SetSecurityLevel(BtSecurityLevel level);
extern void OPPCA_SetObjLocation(BtlObjectLocation objLocation);

extern void OPPCA_BasicPush(const char *fsPushPath);
extern void OPPCA_BasicPull(const char *fsPullPath);
extern void OPPCA_BasicExchange(const char *fsPushPath, const char *fsPullPath);

extern void OPPCA_EncapsulatePush(const char *fsPushPath, BD_ADDR *bd_addr);
extern void OPPCA_EncapsulatePull(const char *fsPullPath, BD_ADDR *bd_addr);
extern void OPPCA_EncapsulateExchange(const char *fsPushPath, const char *fsPullPath, BD_ADDR *bd_addr);

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/* OPPC commands array */
static const OppcaUiCommand oppcaUiCommands[] =  
{  
    {"abort",	        OPPCA_UI_ABORT}, 		       
	{"getsec",          OPPCA_UI_GET_SECURITY}, 			
	{"setsec",          OPPCA_UI_SET_SECURITY}, 		     	
	{"enable",          OPPCA_UI_ENABLE},   
	{"disable",         OPPCA_UI_DISABLE}, 		    
	{"create",          OPPCA_UI_CREATE},		     	 
	{"destroy",         OPPCA_UI_DESTROY},		     	 
	{"connect",         OPPCA_UI_CONNECT},		     	 
	{"disconnect",      OPPCA_UI_DISCONNECT},		     	 
	{"basicPush",       OPPCA_UI_BASIC_PUSH},	     	 
	{"basicPull",       OPPCA_UI_BASIC_PULL},		     	 
	{"basicExchange",   OPPCA_UI_BASIC_EXCHANGE},
	{"encPush",         OPPCA_UI_ENCAPSULATE_PUSH},		     	 
	{"encPull",         OPPCA_UI_ENCAPSULATE_PULL},		     	 
	{"encExchange",     OPPCA_UI_ENCAPSULATE_EXCHANGE},
    {"objLocation",     OPPCA_UI_SET_OBJ_LOCATION}
};

#define OPPCA_COMMANDS_NUM (sizeof(oppcaUiCommands) / sizeof(OppcaUiCommand))



static U8 fsPushPath[BTHAL_FS_MAX_FILE_NAME_LENGTH + BTHAL_FS_MAX_PATH_LENGTH + 1];
static U8 fsPullPath[BTHAL_FS_MAX_FILE_NAME_LENGTH + BTHAL_FS_MAX_PATH_LENGTH + 1];


/*******************************************************************************
 *
 * Internal Function prototypes
 *
 ******************************************************************************/



/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/


/*------------------------------------------------------------------------------
 *            OPPCA_ProcessUserAction
 *------------------------------------------------------------------------------
 *
 * Synopsis:  Processes primitives for the OPP Client application
 *
 * Return:    void
 *
 */
void OPPCA_UI_ProcessUserAction(U8 *msg)
{
	U8 idx;
	U8 command[LINE_PARSER_MAX_STR_LEN];
	BtlObjectLocation objLocation = 2;
    BtSecurityLevel level;
    BD_ADDR bd_addr;
	U8 bdArray[BDADDR_NTOA_SIZE];
    OppcaUiCommandId cmdId = 0;
    U8 paramBuff[20];
    
	if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(command, LINE_PARSER_MAX_STR_LEN))
	{
		Report(("OPPC_UI: Wrong OPP command. Can't parse line"));
		return;
	}

    /* Look for the correct function */
	for(idx = 0; idx < OPPCA_COMMANDS_NUM; idx++)
	{
		if (OS_StrCmp((const char *)command, oppcaUiCommands[idx].cmdName) == 0)
		{
			cmdId = oppcaUiCommands[idx].cmdId;
			break;
		}
	}

    if (idx == OPPCA_COMMANDS_NUM)
    {
        Report(("OPPCA_UI: [command] not found"));	
	    return;
    }

	Report(("OPPC_UI: Received action - \"%s\".", oppcaUiCommands[idx].cmdName));
	
    
    switch(cmdId)
    {
        case OPPCA_UI_ABORT:
			
            OPPCA_AbortClient();
			break;

		case OPPCA_UI_GET_SECURITY:
			
            OPPCA_GetSecurityLevel();
			break;

		case OPPCA_UI_SET_SECURITY:
			
            if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextU8(&level, FALSE))
	        {   
		        Report(("OPPCA_UI_SET_SECURITY: [level] failed "));	
		        break;
	        }
            
            switch (level)
            {
                case BSL_AUTHENTICATION_OUT:
                case BSL_AUTHORIZATION_OUT:
                case BSL_ENCRYPTION_OUT:
                    if (FALSE == OPPCA_SetSecurityLevel(level))
                    {
                        Report(("OPPCA_UI_SET_SECURITY: [level] invalid "));	
                    }
                    break;

                default:
                    Report(("OPPCA_UI_SET_SECURITY: [level] invalid "));	
                    break;
            }
			break;

   		case OPPCA_UI_ENABLE:
		
            OPPCA_Enable();			
            break;

        case OPPCA_UI_DISABLE:
			
            OPPCA_Disable();			
			break;
	
        case OPPCA_UI_CREATE:
			
            OPPCA_Create();
			break;
	
        case OPPCA_UI_DESTROY:
			
            OPPCA_Destroy();
			break;

        case OPPCA_UI_CONNECT:
			
            if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(bdArray, BDADDR_NTOA_SIZE))
	        {
		        Report(("OPPCA_UI_CONNECT: [bd_addr] failed"));
		        break;
	        }

            bd_addr = bdaddr_aton((const char *)(bdArray));
            OPPCA_Connect(&bd_addr);
			break;

        case OPPCA_UI_DISCONNECT:
			
            OPPCA_Disconnect();
			break;

        case OPPCA_UI_BASIC_PUSH:
            
            if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(fsPullPath, LINE_PARSER_MAX_STR_LEN))
            {
                Report(("OPPCA_UI_BASIC_PUSH: [fsPullPath] failed"));	
		        break;
            }
            
            OPPCA_BasicPush((const char *)fsPullPath);
            break;

        case OPPCA_UI_BASIC_PULL:
			
            if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(fsPullPath, LINE_PARSER_MAX_STR_LEN))
            {
                Report(("OPPCA_UI_BASIC_PULL: [fsPullPath] failed"));	
		        break;
            }
            
            OPPCA_BasicPull((const char *)fsPullPath);
            break;

        case OPPCA_UI_BASIC_EXCHANGE:
			
            if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(fsPushPath, LINE_PARSER_MAX_STR_LEN))
            {
                Report(("OPPCA_UI_BASIC_EXCHANGE: [fsPushPath] failed"));	
		        break;
            }
            
            if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(fsPullPath, LINE_PARSER_MAX_STR_LEN))
            {
                Report(("OPPCA_UI_BASIC_EXCHANGE: [fsPullPath] failed"));	
		        break;
            }
            
            OPPCA_BasicExchange((const char *)fsPushPath, (const char *)fsPullPath);
            break;

        case OPPCA_UI_ENCAPSULATE_PUSH:
			
            if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(fsPushPath, LINE_PARSER_MAX_STR_LEN))
            {
                Report(("OPPCA_UI_ENCAPSULATE_PUSH: [fsPushPath] failed"));	
		        break;
            }
            
            if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(bdArray, BDADDR_NTOA_SIZE))
            {
		        Report(("OPPCA_UI_ENCAPSULATE_PUSH: invalid bd_addr."));
		        break;
	        }

            bd_addr = bdaddr_aton((const char *)(bdArray));
			OPPCA_EncapsulatePush((const char *)fsPushPath, &bd_addr);
            break;

        case OPPCA_UI_ENCAPSULATE_PULL:
			
            if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(fsPullPath, LINE_PARSER_MAX_STR_LEN))
            {
                Report(("OPPCA_UI_ENCAPSULATE_PULL: [fsPullPath] failed"));	
		        break;
            }
            
            if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(bdArray, BDADDR_NTOA_SIZE))
            {
		        Report(("OPPCA_UI_ENCAPSULATE_PULL: invalid bd_addr."));
		        break;
	        }

            bd_addr = bdaddr_aton((const char *)(bdArray));
            OPPCA_EncapsulatePull((const char *)fsPullPath, &bd_addr);
            break;

        case OPPCA_UI_ENCAPSULATE_EXCHANGE:
			
            if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(fsPushPath, LINE_PARSER_MAX_STR_LEN))
            {
                Report(("OPPCA_UI_ENCAPSULATE_EXCHANGE: [fsPushPath] failed"));	
		        break;
            }
            
            if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(fsPullPath, LINE_PARSER_MAX_STR_LEN))
            {
                Report(("OPPCA_UI_ENCAPSULATE_EXCHANGE: [fsPullPath] failed"));	
		        break;
            }
            
            if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(bdArray, BDADDR_NTOA_SIZE))
            {
		        Report(("OPPCA_UI_ENCAPSULATE_EXCHANGE: wrong BD addr."));
		        break;
	        }

            bd_addr = bdaddr_aton((const char *)(bdArray));
			
            OPPCA_EncapsulateExchange((const char *)fsPushPath, (const char *)fsPullPath, &bd_addr);

            break;

         case OPPCA_UI_SET_OBJ_LOCATION:
			
            if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(paramBuff, LINE_PARSER_MAX_STR_LEN))
            {
                Report(("OPPSA_UI_SET_OBJ_LOCATION: [Memory or FS] failed"));	
		        break;
            }

	        if (OS_StrCmp((const char *)paramBuff, "Memory") == 0)
	        {
		        Report(("OPPCA: Using Memory object"));
                objLocation = BTL_OBJECT_LOCATION_MEM;
	        }
	        else if (OS_StrCmp((const char *)paramBuff, "FS") == 0)
	        {
		        Report(("OPPCA: Using FS object"));
                objLocation = BTL_OBJECT_LOCATION_FS;
	        }
	        else
	        {
                Report(("OPPCA_UI_SET_OBJ_LOCATION: [Memory or FS] failed"));	
            }

            if (TRUE == OPPCA_UI_CHECK_OBJ_LOCATION(objLocation))
            {
                OPPCA_SetObjLocation(objLocation);
            }
            else
            {
                Report(("OPPSA_UI_SET_OBJ_LOCATION: [objLocation] invalid "));	
            }

            break;

        default:
            Report(("OPPC_UI: Mistake in commands table"));
            break;
    } 
}

#else /*BTL_CONFIG_OPP == BTL_CONFIG_ENABLED*/

void OPPCA_UI_ProcessUserAction(U8 *msg)
{
	msg=msg;
	Report(("OPP_APP is disabled via BTL_CONFIG."));
}




#endif /*BTL_CONFIG_OPP == BTL_CONFIG_ENABLED*/



