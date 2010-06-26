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
*   FILE NAME:      opps_ui.c
*
*   DESCRIPTION:    This file contains the implementation of the OPPS sample 
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
#include "bthal_fs.h"
#include "lineparser.h"


/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

#define OPPSA_UI_DEFAULT_FULL_NAME      "/QbInbox/default_object/mycard.vcf"
#define OPPSA_UI_DEFAULT_FULL_DIR       "/QbInbox/default_object"
#define OPPSA_UI_NEW_FULL_NAME          "/QbInbox/default_object/mycard_t.vcf"


/*******************************************************************************
 *
 * Macro definitions
 *
 ******************************************************************************/

#define OPPSA_UI_CHECK_OBJ_LOCATION(objectLocation)                  \
          ((objectLocation == BTL_OBJECT_LOCATION_FS) ||         \
              (objectLocation == BTL_OBJECT_LOCATION_MEM))

 

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * OppsaPullUseObject useObject
 *
 *     	Defines which object to use in accept/reject Pull operation.
 */
typedef U8 OppsaPullUseObject;

#define OPPSA_PULL_USE_DEFAULT_OBJ     				(0x01)
#define OPPSA_PULL_USE_NEW_OBJ           			(0x02)	


 /*-------------------------------------------------------------------------------
 * OppsaUiCommandId type
 *
 *     Defines the OPPS user commands
 */
typedef U8 OppsaUiCommandId;

#define OPPSA_UI_ABORT                      0
#define OPPSA_UI_GET_SECURITY               1
#define OPPSA_UI_SET_SECURITY               2
#define OPPSA_UI_ENABLE                     3
#define OPPSA_UI_DISABLE                    4
#define OPPSA_UI_CREATE                     5
#define OPPSA_UI_DESTROY                    6       
#define OPPSA_UI_DISCONNECT                 7    
#define OPPSA_UI_GET_CONNCTED_DEVICE        8
#define OPPSA_UI_SET_ACCEPT                 9
#define OPPSA_UI_SET_OBJ_LOCATION           10
#define OPPSA_UI_CREATE_DEFAULT_OBJ         11

typedef struct _OppsaUiCommand
{
	char 	            cmdName[LINE_PARSER_MAX_STR_LEN];
	OppsaUiCommandId    cmdId;
} OppsaUiCommand;


/********************************************************************************
 *
 * Function prototypes
 *
 *******************************************************************************/

void OPPSA_UI_ProcessUserAction(U8 *msg);



/********************************************************************************
 *
 * External functions
 *
 *******************************************************************************/
extern void OPPSA_Init(void);
extern void OPPSA_Deinit(void);
extern void OPPSA_Create(void);
extern void OPPSA_Destroy(void);
extern void OPPSA_Enable(void);
extern void OPPSA_Disable(void);
extern void OPPSA_SetDefaultObject(const BtlObject *defaultObject);
extern void OPPSA_AbortServer(void);
extern void OPPSA_Disconnect(void);
extern void OPPSA_GetConnectedDevice(void);
extern BOOL OPPSA_GetSecurityLevel(void);
extern void OPPSA_SetSecurityLevel(BtSecurityLevel level);
extern void OPPSA_AcceptObjectRequest(BOOL accept);
extern void OPPSA_SetObjLocation(BtlObjectLocation objLocation);


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/


/* OPPS commands array */
static const OppsaUiCommand oppsaUiCommands[] =  
{  
    {"abort",	        OPPSA_UI_ABORT}, 		       
	{"getsec",          OPPSA_UI_GET_SECURITY}, 			
	{"setsec",          OPPSA_UI_SET_SECURITY}, 		     	
	{"enable",          OPPSA_UI_ENABLE},   
	{"disable",         OPPSA_UI_DISABLE}, 		    
	{"create",          OPPSA_UI_CREATE},		     	 
	{"destroy",         OPPSA_UI_DESTROY},		     	 
	{"disconnect",      OPPSA_UI_DISCONNECT},		     	 
	{"getcon",          OPPSA_UI_GET_CONNCTED_DEVICE},
    {"accept",          OPPSA_UI_SET_ACCEPT},
    {"objLocation",      OPPSA_UI_SET_OBJ_LOCATION},
    {"createDefaultVcard",  OPPSA_UI_CREATE_DEFAULT_OBJ}
};

#define OPPSA_COMMANDS_NUM (sizeof(oppsaUiCommands) / sizeof(OppsaUiCommand))

static BtlObjectLocation   objectLocation = BTL_OBJECT_LOCATION_FS;   


/*******************************************************************************
 *
 * Internal Function prototypes
 *
 ******************************************************************************/
static void OppsaUiSetFsDefaultObject(BtlObject *obj, const char *ObName, const char* fullPath);
static void OppsaUiCreateVCardFile(OppsaPullUseObject useObj);



/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/


/*------------------------------------------------------------------------------
 *            OPPSA_UI_ProcessUserAction
 *------------------------------------------------------------------------------
 *
 * Synopsis:  Processes primitives for the OPP server application
 *
 * Return:    void
 *
 */
void OPPSA_UI_ProcessUserAction(U8 *msg)
{
	U8 idx;
	U8 command[LINE_PARSER_MAX_STR_LEN];
	BtSecurityLevel level;
	BOOL accept;
	OppsaUiCommandId cmdId = 0;
	BtlObjectLocation objLocation = 2; 
	U8 paramBuff[20];
            
	if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(command, LINE_PARSER_MAX_STR_LEN))
	{
		Report(("OPPSA_UI: Wrong OPP command. Can't parse line"));
		return;
	}

	/* Look for the correct function */
	for(idx = 0; idx < OPPSA_COMMANDS_NUM; idx++)
	{
		if (OS_StrCmp((const char *)command, oppsaUiCommands[idx].cmdName) == 0)
		{
			cmdId = oppsaUiCommands[idx].cmdId;
			break;
		}
	}

	if (idx == OPPSA_COMMANDS_NUM)
	{
	    Report(("OPPSA_UI: [command] not found"));	
	    return;
	}

	Report(("OPPSA_UI: Received action - \"%s\".", oppsaUiCommands[idx].cmdName));
	
	switch (cmdId)    
	{

		case OPPSA_UI_ABORT:
			
			OPPSA_AbortServer();
			break;

		case OPPSA_UI_GET_SECURITY:
			
			OPPSA_GetSecurityLevel();
			break;

		case OPPSA_UI_SET_SECURITY:
			
			if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextU8(&level, FALSE) )
			{   
				Report(("OPPSA_UI_SET_SECURITY: [level] failed "));	
				break;
			}

			OPPSA_SetSecurityLevel(level);
			break;

		case OPPSA_UI_ENABLE:

			OPPSA_Enable();
			break;

		case OPPSA_UI_DISABLE:
			
		    OPPSA_Disable();			
		    break;

		case OPPSA_UI_CREATE:
			
		    OPPSA_Create();
		    break;

		case OPPSA_UI_DESTROY:
			
		    OPPSA_Destroy();
			break;

		case OPPSA_UI_DISCONNECT:
		   
		    OPPSA_Disconnect();
			break;

		case OPPSA_UI_GET_CONNCTED_DEVICE:
		    
		    OPPSA_GetConnectedDevice();
		    break;

		case OPPSA_UI_SET_ACCEPT:

		    if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextBool(&accept))
		    {   
		        Report(("OPPSA_UI_SET_ACCEPT: [accept] failed "));	
		        break;
		    }
		    
		    OPPSA_AcceptObjectRequest(accept);
		    break;

		case OPPSA_UI_SET_OBJ_LOCATION:
			
		    if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(paramBuff, LINE_PARSER_MAX_STR_LEN))
		    {
		        Report(("OPPSA_UI_SET_OBJ_LOCATION: [Memory or FS] failed"));	
		        break;
		    }

		    if (OS_StrCmp((const char *)paramBuff, "Memory") == 0)
		    {
		        Report(("OPPSA: Using Memory object"));
		        objLocation = BTL_OBJECT_LOCATION_MEM;
		    }
		    else if (OS_StrCmp((const char *)paramBuff, "FS") == 0)
		    {
		        Report(("OPPSA: Using FS object"));
		        objLocation = BTL_OBJECT_LOCATION_FS;
		    }
		    else
		    {
		        Report(("OPPSA_UI_SET_OBJ_LOCATION: [Memory or FS] failed"));	
		    }

		    if (TRUE == OPPSA_UI_CHECK_OBJ_LOCATION(objLocation))
		    {
		        OPPSA_SetObjLocation(objLocation);
		    }
		    else
		    {
		        Report(("OPPSA_UI_SET_OBJ_LOCATION: [objLocation] invalid "));	
		    }
		    break;

		case OPPSA_UI_CREATE_DEFAULT_OBJ:

		    OppsaUiCreateVCardFile(OPPSA_PULL_USE_DEFAULT_OBJ);
		    OppsaUiCreateVCardFile(OPPSA_PULL_USE_NEW_OBJ);
		    break;

		default:
		    Report(("OPPSA_UI: Mistake in commands table"));
		    break;
    } 
}


/*---------------------------------------------------------------------------
 * OppsUiSetFsDefaultObject()
 *
 * Synopsis:  Function for setting the OPPS default object.
 *
 * Return:    void
 *
 */
static void OppsaUiSetFsDefaultObject(BtlObject *obj, const char *ObName, const char* fullPath)
{			
    
	obj->objectName      			    = (const BtlUtf8 *)ObName;
	obj->location.fsLocation.fsPath     = (const BtlUtf8 *)fullPath;

	/* This should be provided by the user (i.e. this is just an example) */
	obj->objectMimeType 			    = "text/x-vCard";
	obj->objectLocation 			    = BTL_OBJECT_LOCATION_FS;
}


/*---------------------------------------------------------------------------
 *            OppsaUiCreateVCardFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Creates the default VCard file 
 *
 * Return:    void.
 *
 */
static void OppsaUiCreateVCardFile(OppsaPullUseObject useObj)
{
	static const char  defaultContent[] = "BEGIN:VCARD\r\nVERSION:2.1\r\nN:Smith;Adam\r\nFN:Adam Smith\r\n\
TITLE:Software Engineer\r\nTEL;WORK;VOICE:972-9-12345678\r\nTEL;CELL;VOICE:972-54-7654321\r\n\
EMAIL;PREF;INTERNET:adams@ti.com\r\nREV:20070423T074123Z\r\nEND:VCARD\r\n";

	static const char  newContent[] = "BEGIN:VCARD\nVERSION:2.1\r\nN:New;Adam\r\nFN:Adam New\r\nTITLE:Software Engieneer\r\n\
TEL;WORK;VOICE:972-9-12345678\r\nTEL;CELL;VOICE:972-54-7654321\r\nEMAIL;PREF;INTERNET:adamn@ti.com\r\n\
REV:20070423T075012Z\r\nEND:VCARD\r\n";
    
    BtFsStatus      btFsStatus;
    BthalFsFileDesc fp;
    BTHAL_U32       pNumWritten;
    BTHAL_U16       nSize;
    const char      *fsContent = NULL;
    const BTHAL_U8  *fsPath = NULL;
	
    switch (useObj)
    {
        case OPPSA_PULL_USE_DEFAULT_OBJ:
            fsContent   = defaultContent;
            fsPath      = (const BTHAL_U8 *)OPPSA_UI_DEFAULT_FULL_NAME;
            break;

        case OPPSA_PULL_USE_NEW_OBJ:
            fsContent   = newContent;
            fsPath      = (const BTHAL_U8 *)OPPSA_UI_NEW_FULL_NAME;
            break;
    }
    
    /* Creates / Overwrites the current default VCard file */
    BTHAL_FS_Remove(fsPath);
    
    btFsStatus = BTHAL_FS_Open(fsPath, 
                        BTHAL_FS_O_CREATE | BTHAL_FS_O_RDWR | BTHAL_FS_O_BINARY,  /* Create file */
                            (BthalFsFileDesc *)&fp);

    if (BT_STATUS_HAL_FS_SUCCESS != btFsStatus)
    {
        Report(("OppsaUiCreateDefaultVCardFile(): Error - could not create Default VCard file %s.", OPPSA_UI_DEFAULT_FULL_NAME));
        Report(("Make sure that %s directory exists !.", OPPSA_UI_DEFAULT_FULL_DIR));
        return;
    }
    
    nSize = OS_StrLen(fsContent);

    if (BT_STATUS_HAL_FS_SUCCESS == BTHAL_FS_Write(fp, (void*)fsContent, nSize, &pNumWritten))
	{
        switch (useObj)
        {
            case OPPSA_PULL_USE_DEFAULT_OBJ:
                Report(("OppsaUiCreateVCardFile(): Default VCard file \"%s\" created successfully.", OPPSA_UI_DEFAULT_FULL_NAME));
                break;

            case OPPSA_PULL_USE_NEW_OBJ:
                Report(("OppsaUiCreateVCardFile(): New VCard file \"%s\" created successfully.", OPPSA_UI_NEW_FULL_NAME));
                break;
        }
	}
	else
	{
		Report(("OppsaUiCreateVCardFile(): Error - Unable to create VCard file."));
	}

	
    BTHAL_FS_Close(fp);

    Report(("OppsaUiCreateDefaultVCardFile(): nSize = %d, pNumWritten = %d", nSize, pNumWritten));
}



#else /*BTL_CONFIG_OPP == BTL_CONFIG_ENABLED */

void OPPSA_UI_ProcessUserAction(U8 *msg)
{
	msg=msg;
	Report(("OPP_APP is disabled via BTL_CONFIG."));
}


#endif /*BTL_CONFIG_OPP == BTL_CONFIG_ENABLED */


