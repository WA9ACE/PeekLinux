/* =========================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================== */
/*
 $Project name: Basic Bluetooth MMI
 $Project code:
 $Module:   Bluetooth BMG MFW
 $File:       Mfw_BtipsOppc.c
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the OPP Client APPlication functionality.

********************************************************************************
 $History: Mfw_BtipsOppc.c

  26/06/07 Sasken original version

 $End

*******************************************************************************/




/*******************************************************************************

                                Include files

*******************************************************************************/

#define ENTITY_MFW
/* includes */

#include <string.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#ifdef FF_MMI_BTIPS_APP
#include "osapi.h"
#include "btl_common.h"
#include "debug.h"

//#include "../app_main.h"
#include "btl_oppc.h"
#include "btl_config.h"
#include "bthal_fs.h"
//#include "../../btl/inc/int/obstore.h"
#include "obstore.h"

#include "mfw_BtipsOppc.h"
#include "Mfw_mfw.h"
#include "mfw_Btips.h"
#include "mfw_ffs.h"

extern int   oppc_files_count;
//Array to load fileNames in content directory
extern char* oppc_file_names[OPPC_MAX_FILES]; 

/* Macro for sending events to UI - meanwhile, trace message to PCO Viewer */
extern int vsi_o_state_ttrace ( const char * const format, ... );
#define UI_DISPLAY(s) vsi_o_state_ttrace s

/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/


/*******************************************************************************
 *
 * Macro definitions
 *
 ******************************************************************************/
#define OPPCA_BUFF_MAX                    (1 * 512)

#define BTL_CONFIG_OPP_MAX_FILE_NAME_LEN	min(GOEP_MAX_UNICODE_LEN/2-2, BTHAL_FS_MAX_FILE_NAME_LENGTH)


#define OPPCA_CHECK_FS_NAME_LEN(fsPath)    \
             (OS_StrLen(fsPath) < (BTL_CONFIG_OPP_MAX_FILE_NAME_LEN + BTHAL_FS_MAX_PATH_LENGTH + 1)) 





/*******************************************************************************
 *
 * OPPC External Function prototypes
 *
 ******************************************************************************/

void mfw_btips_oppcInit(void);
void mfw_btips_oppcDeinit(void);

void mfw_btips_oppcEncapsulatePush(const char *fsPushPath, BD_ADDR *bd_addr);
void mfw_btips_oppcEncapsulatePull(const char *fsPullPath, BD_ADDR *bd_addr);
void mfw_btips_oppcEncapsulateExchange(const char *fsPushPath, const char *fsPullPath, BD_ADDR *bd_addr);







/*-------------------------------------------------------------------------------
 * OppcaCurrOperation type
 *
 *     Defines the OPPCA current operation.
 */
typedef U8 OppcaCurrOperation;

#define OPPCA_OPER_NONE     		0x00	
#define OPPCA_OPER_BASIC_PULL		0x01	/* Pull */
#define OPPCA_OPER_BASIC_PUSH    	0x02	/* Push */
#define OPPCA_OPER_BASIC_EXCHANGE	0x03	/* Push-Pull */
#define OPPCA_OPER_ENC_PULL 		0x04	/* Connect-Pull-Disconnect */
#define OPPCA_OPER_ENC_PUSH 		0x05	/* Connect-Push-Disconnect */
#define OPPCA_OPER_ENC_EXCHANGE     0x06	/* Connect-Push-Pull-Disconnect */


/*-------------------------------------------------------------------------------
 * OppcaState type
 *
 *     Defines the OPPCA current state.
 */
typedef U8 OppcaState;

#define OPPCA_IDLE				0x00	/* OBEX and Transport disconnected */   
#define OPPCA_CONNECTING		0x01	/* OBEX connecting */
#define OPPCA_TP_CONNECTED		0x02	/* Transport connected */
#define OPPCA_CONNECTED			0x03	/* OBEX connected */
#define OPPCA_DISCONNECTING		0x04	/* OBEX disconnecting */   
#define OPPCA_TP_DISCONNECTING	0x05	/* Transport disconnecting */
#define OPPCA_DISABLING     	0x06	/* OBEX and Transport disabling */


typedef struct _OppcaData 
{
    OppcaState          state;

    BOOL                isConnected;

    BOOL 	            operationOngoing;

    BtSecurityLevel     securityLevel;

    BtlObjectLocation   objLocation;

    OppcaCurrOperation  currOper;

    /* Memory Objects */
    char                pushBuff[OPPCA_BUFF_MAX];
    char                pullBuff[OPPCA_BUFF_MAX];

} OppcaData;

 
/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

static OppcaData    oppClient;
static BtlOppcContext   *oppcContext = 0;   

static char ObPushName[BTL_CONFIG_OPP_MAX_FILE_NAME_LEN + 1] = {'\0'};
static char ObPullName[BTL_CONFIG_OPP_MAX_FILE_NAME_LEN + 1] = {'\0'};

extern char push_file_name[];

/*******************************************************************************
 *
 * Internal Function prototypes
 *
 ******************************************************************************/
static void  btips_oppcCallback(const BtlOppcEvent *Event);

static const char *mfw_btips_oppcGoepOpName(GoepOperation Op);

static BOOL mfw_btips_oppcBuildPushObject(BtlObject *objToPush, const char *fsPath, char *buff);
static BOOL mfw_btips_oppcBuildPullObject(BtlObject *objToPull, const char *fsPath, char *buff);

static BOOL mfw_btips_oppcConvertToMemPushObject(BtlObject *obj, const char *fsPushPathh, char *buff);
static void mfw_btips_oppcSetFsPushObject(BtlObject *obj, const char *fsPushPath);
static void mfw_btips_oppcSetFsPullObject(BtlObject *obj, const char *fsPullPath);
static void mfw_btips_oppcSetMemPullObject(BtlObject *obj, const char *fsPullPath, char *buff);

/*---------------------------------------------------------------------------
 *            mfw_btips_oppcInit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX Client for the OPP application 
 *            and the Object Store.
 *
 * Return:    void
 *
 */
void mfw_btips_oppcInit(void)
{
    BtStatus status;
    
    oppClient.state = OPPCA_IDLE;
    oppClient.isConnected = FALSE;
    oppClient.operationOngoing = FALSE;
    oppClient.securityLevel = BSL_NO_SECURITY;
    oppClient.currOper = OPPCA_OPER_NONE;
    oppClient.objLocation = BTL_OBJECT_LOCATION_FS;
    
    status = BTL_OPPC_Create((BtlAppHandle *)0, btips_oppcCallback, &oppClient.securityLevel, &oppcContext);
    TRACE_EVENT_P1("BTL_OPPC_Create() returned %s.",pBT_Status(status));

    status = BTL_OPPC_Enable(oppcContext);
    TRACE_EVENT_P1("BTL_OPPC_Enable() returned %s.",pBT_Status(status));
}


/*---------------------------------------------------------------------------
 *            mfw_btips_oppcDeinit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Client protocol.
 *
 * Return:    void
 *
 */
void mfw_btips_oppcDeinit(void)
{
	BtStatus status;

	status =  BTL_OPPC_Disable(oppcContext);
	
    Report(("BTL_OPPC_Disable() returned %s.",pBT_Status(status)));

    if (status == BT_STATUS_SUCCESS)
    {
    status = BTL_OPPC_Destroy(&oppcContext);
    
    Report(("BTL_OPPC_Destroy() returned %s.", pBT_Status(status)));

        oppClient.state = OPPCA_IDLE;
        oppClient.isConnected = FALSE;
        oppClient.operationOngoing = FALSE;
    }
    else
    {
        oppClient.state = OPPCA_DISABLING;
    }
}


/*---------------------------------------------------------------------------
 *            mfw_btips_oppcEncapsulatePush
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate an OBEX Object Push procedure.
 *            Includes Connect and Disconnect from the opp server.  
 *			  If the given BD_ADDR is NULL, a search is performed.
 *
 * Return:    void
 *
 */
void mfw_btips_oppcEncapsulatePush(const char *fsPushPath, BD_ADDR *bd_addr)
{
    BtStatus status;
	BtlObject objToPush;
	
	Report(("mfw_btips_oppcEncapsulatePush: fsPushPath = %s", fsPushPath));

    if (TRUE != OPPCA_CHECK_FS_NAME_LEN(fsPushPath))
	{
		Report(("mfw_btips_oppcEncapsulatePush: Can't push. fsPushPath too long"));
		return;
	}
	
    if (TRUE != mfw_btips_oppcBuildPushObject(&objToPush, fsPushPath, oppClient.pushBuff))
    {
        Report(("OPPCA_BasicPull: Can't pull"));
        return;
    }

    status = BTL_OPPC_ConnectPushDisconnect(oppcContext, bd_addr, &objToPush);

	Report(("BTL_OPPC_ConnectPushDisconnect() returned %s.", pBT_Status(status)));
	if (status == OB_STATUS_PENDING)
	{
		Report(("mfw_btips_oppcEncapsulatePush(): Started ..."));
		oppClient.operationOngoing = TRUE;
        oppClient.currOper = OPPCA_OPER_ENC_PUSH;
    }
}


/*---------------------------------------------------------------------------
 *            mfw_btips_oppcEncapsulatePull
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate an OBEX Business Card Pull procedure.
 *            Includes Connect and Disconnect from the opp server.  
 *			  If the given BD_ADDR is NULL, a search is performed.
 *
 * Return:    void
 *
 */
void mfw_btips_oppcEncapsulatePull(const char *fsPullPath, BD_ADDR *bd_addr)
{
    BtStatus status;
	BtlObject objToPull;
	
    Report(("mfw_btips_oppcEncapsulatePull: fsPullPath = %s", fsPullPath));
   
    if (TRUE != OPPCA_CHECK_FS_NAME_LEN(fsPullPath))
	{
		Report(("mfw_btips_oppcEncapsulatePull: Can't pull. fsPullPath too long"));
		return;
	}
     
    if (TRUE != mfw_btips_oppcBuildPullObject(&objToPull, fsPullPath, oppClient.pullBuff))
    {
        Report(("mfw_btips_oppcEncapsulatePull: Can't pull"));
        return;
    }
    
	status = BTL_OPPC_ConnectPullDisconnect(oppcContext, bd_addr, &objToPull);
	
	Report(("BTL_OPPC_Pull() returned %s.", pBT_Status(status)));
		
	if (status == OB_STATUS_PENDING)
	{
		Report(("mfw_btips_oppcEncapsulatePull(): Started ..."));
		oppClient.operationOngoing = TRUE;
        oppClient.currOper = OPPCA_OPER_ENC_PULL;
    }
}


/*---------------------------------------------------------------------------
 *            mfw_btips_oppcEncapsulateExchange
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate an OBEX Business Card Exchange procedure.
 *            Includes Connect and Disconnect from the opp server.  
 *			  If the given BD_ADDR is NULL, a search is performed.
 *
 * Return:    void
 *
 */
void mfw_btips_oppcEncapsulateExchange(const char *fsPushPath, const char *fsPullPath, BD_ADDR *bd_addr)
{
    BtStatus status;
	BtlObject objToPush, objToPull;
	
	Report(("mfw_btips_oppcEncapsulateExchange: fsPushPath = %s, fsPullPath = %s", fsPushPath, fsPullPath));


	/* -------- Push Operation --------- */
    
    if (TRUE != OPPCA_CHECK_FS_NAME_LEN(fsPushPath))
	{
		Report(("mfw_btips_oppcEncapsulateExchange: Can't push. fsPushath too long"));
		return;
	}

	if (TRUE != mfw_btips_oppcBuildPushObject(&objToPush, fsPushPath, oppClient.pushBuff))
    {
        Report(("mfw_btips_oppcEncapsulateExchange: Can't push"));
        return;
    }
   
	/* -------- Pull Operation --------- */

    if (TRUE != OPPCA_CHECK_FS_NAME_LEN(fsPullPath))
	{
		Report(("mfw_btips_oppcEncapsulateExchange: Can't pull. fsPullPath too long"));
		return;
	}

    if (TRUE != mfw_btips_oppcBuildPullObject(&objToPull, fsPullPath, oppClient.pullBuff))
    {
        Report(("mfw_btips_oppcEncapsulateExchange: Can't pull"));
        return;
    }

    
    status = BTL_OPPC_ConnectExchangeDisconnect(oppcContext, bd_addr, &objToPush, &objToPull);

	Report(("BTL_OPPC_ConnectExchangeDisconnect() returned %s.", pBT_Status(status)));
	
	if (status == OB_STATUS_PENDING)
	{
		Report(("mfw_btips_oppcEncapsulateExchange(): Started ..."));
		oppClient.operationOngoing = TRUE;
        oppClient.currOper = OPPCA_OPER_ENC_EXCHANGE;
    }
}	


/*---------------------------------------------------------------------------
 *            btips_oppcCallback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes OBEX Client protocol events.
 *
 * Return:    void
 *
 */
static void btips_oppcCallback(const BtlOppcEvent *Event)
{
    BD_ADDR bdAddr;
	char addr[BDADDR_NTOA_SIZE];
    BtStatus status;

    switch (Event->oppcEvent->event)
	{
        case OPPC_EVENT_COMPLETE:
			
            TRACE_EVENT_P1("btips_oppcCallback: The current %s is complete.", mfw_btips_oppcGoepOpName(Event->oppcEvent->oper)); 

            switch (Event->oppcEvent->oper) 
		    {
                case GOEP_OPER_CONNECT:
                    TRACE_EVENT_P1("btips_oppcCallback: Transport Connection has been established with %s", bdaddr_ntoa(&bdAddr, addr));
                    BTL_OPPC_GetConnectedDevice(oppcContext, &bdAddr);
                    oppClient.state = OPPCA_CONNECTED;
                    oppClient.isConnected = TRUE;
                    break;
                    
                case GOEP_OPER_DISCONNECT:
                    TRACE_EVENT("btips_oppcCallback: Transport Connection has been disconnected.");
		    	oppClient.isConnected = FALSE;
			oppClient.state = OPPCA_IDLE;
                    oppClient.operationOngoing = FALSE;
                    break;


                case GOEP_OPER_PULL:

                    switch(oppClient.currOper)
                    {
                        case OPPCA_OPER_BASIC_PULL:
                            oppClient.operationOngoing = FALSE;
                            break;

                        case OPPCA_OPER_BASIC_EXCHANGE:
                            oppClient.operationOngoing = FALSE;
                            break;
                    }

                    break;


                case GOEP_OPER_PUSH:

                    switch(oppClient.currOper)
                    {
                        case OPPCA_OPER_BASIC_PUSH:
                            oppClient.operationOngoing = FALSE;
                            break;
                    }
                    break;
            }
            break;

        case OPPC_EVENT_ABORTED:
			
			UI_DISPLAY(("btips_oppcCallback: %s operation failed.", mfw_btips_oppcGoepOpName(Event->oppcEvent->oper))); 
			break;

        case OPPC_EVENT_TP_CONNECTED:
			
            BTL_OPPC_GetConnectedDevice(oppcContext, &bdAddr);
			UI_DISPLAY(("btips_oppcCallback: Transport Connection is up to %s.", bdaddr_ntoa(&bdAddr, addr)));

            oppClient.state = OPPCA_TP_CONNECTED;
            oppClient.isConnected = TRUE;
			break;

       	case OPPC_EVENT_TP_DISCONNECTED:
			
            UI_DISPLAY(("btips_oppcCallback: Transport Connection has been disconnected."));
            
            
			oppClient.isConnected = FALSE;
            oppClient.operationOngoing = FALSE;
            
            if (oppClient.state != OPPCA_DISABLING)
            {
            oppClient.state = OPPCA_IDLE; 
            }
			break;

        
        case OPPC_EVENT_DISCOVERY_FAILED:
			
            UI_DISPLAY(("btips_oppcCallback: Discovery Failure."));
			oppClient.state = OPPCA_IDLE;
			break;

     	case OPPC_EVENT_NO_SERVICE_FOUND:
			
            UI_DISPLAY(("btips_oppcCallback: No OBEX Service Found."));
			oppClient.state = OPPCA_IDLE;
			break;
		
	
	    case OPPC_EVENT_OBJECT_PROGRESS:
			
            UI_DISPLAY(("btips_oppcCallback: progress bar ..."));
            
            break;
   
    
        case OPPC_EVENT_DISABLED:
            
            UI_DISPLAY(("btips_oppcCallback: Receiving OPPC_EVENT_DISABLED."));
            
            if (OPPCA_DISABLING == oppClient.state)
            {
                status = BTL_OPPC_Destroy(&oppcContext);
    
                Report(("BTL_OPPC_Destroy() returned %s.", pBT_Status(status)));  
            }
            
            oppClient.state = OPPCA_IDLE;
            break;

        default:
		    
            UI_DISPLAY(("btips_oppcCallback: Unexpected event: %i", Event->oppcEvent->event));
            break;
    }
}


/*---------------------------------------------------------------------------
 *            mfw_btips_oppcGoepOpName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to the name of the current operation.
 *
 * Return:    ASCII String pointer.
 *
 */
static const char *mfw_btips_oppcGoepOpName(GoepOperation Op)
{
    switch (Op) 
	{
    case GOEP_OPER_PUSH:
        return "Push";
    case GOEP_OPER_PULL:
        return "Pull";
    case GOEP_OPER_CONNECT:
        return "Connect";
    case GOEP_OPER_DISCONNECT:
        return "Disconnect";
    case GOEP_OPER_ABORT:
        return "Abort";
	case GOEP_OPER_DELETE:
        return "Delete";
	case GOEP_OPER_SETFOLDER:
        return "Set Folder";
    }
    return "Unknown";
}


/*---------------------------------------------------------------------------
 * mfw_btips_oppcBuildPushObject()
 *
 * Synopsis:  Build push object
 *
 * Return:    TRUE - success FALSE - failed
 *
 */
static BOOL mfw_btips_oppcBuildPushObject(BtlObject *objToPush, const char *fsPath, char *buff)
{
	if (oppClient.objLocation == BTL_OBJECT_LOCATION_MEM)
	{
		if (TRUE != mfw_btips_oppcConvertToMemPushObject(objToPush, fsPath, buff))
        {
           return FALSE;
        }
	}
    else if (oppClient.objLocation == BTL_OBJECT_LOCATION_FS)
    {
        mfw_btips_oppcSetFsPushObject(objToPush, fsPath);
    }

    return TRUE;
}


/*---------------------------------------------------------------------------
 * mfw_btips_oppcBuildPullObject()
 *
 * Synopsis:  Build push object
 *
 * Return:    TRUE - success FALSE - failed
 *
 */
static BOOL mfw_btips_oppcBuildPullObject(BtlObject *objToPull, const char *fsPath, char *buff)
{
	if (oppClient.objLocation == BTL_OBJECT_LOCATION_MEM)
	{
		mfw_btips_oppcSetMemPullObject(objToPull, fsPath, buff);
	}
    else if (oppClient.objLocation == BTL_OBJECT_LOCATION_FS)
    {
        mfw_btips_oppcSetFsPullObject(objToPull, fsPath);	
    }

    return TRUE;
}


/*---------------------------------------------------------------------------
 * mfw_btips_oppcConvertToMemPushObject()
 *
 * Synopsis:  Function for testing OPP from memory objects. Convert file object to memory object by reading the 
 *            file and modifying proper fileds uses a static buffer, so only one such object can be supported at a time
 *            maximum file size is 512 KB
 *
 * Return:    void
 *
 */
static BOOL mfw_btips_oppcConvertToMemPushObject(BtlObject *obj, const char *fsPushPath, char *buff)
{
	BtFsStatus btFsStatus;
    BthalFsFileDesc fd;
	BTHAL_U32 numRead;
    BthalFsStat FileStat;

   	btFsStatus = BTHAL_FS_Stat((const BTHAL_U8 *)fsPushPath, &FileStat); 
	if(btFsStatus != BT_STATUS_HAL_FS_SUCCESS)
	{
		Report(("Client: Error - File was not found %s", fsPushPath));
        return FALSE;
	}
	
    if (FileStat.size > OPPCA_BUFF_MAX)
    {
        Report(("Client: Error pushed file > %d bytes", OPPCA_BUFF_MAX));
        return FALSE;
    }
       
    btFsStatus = BTHAL_FS_Open((const BTHAL_U8 *)fsPushPath, 
		            BTHAL_FS_O_RDONLY | BTHAL_FS_O_BINARY,      /* Read Only */
                            (BthalFsFileDesc *)&fd);
 
    if (btFsStatus == BT_STATUS_HAL_FS_SUCCESS)
	{
        btFsStatus  = BTHAL_FS_Read(fd, oppClient.pushBuff, OPPCA_BUFF_MAX, &numRead);
        
        if (btFsStatus == BT_STATUS_HAL_FS_SUCCESS)
	 	{
			Report(("Client: fsPushPath = %s, numRead = %d", fsPushPath, numRead));
	 	}
		
        BTHAL_FS_Close(fd);
	
        /* Extract object name from fsPath */
        OBSTORE_ExtractObjName(fsPushPath, ObPushName);
        
        obj->objectLocation                 = BTL_OBJECT_LOCATION_MEM;
		obj->objectName                     = ObPushName;
        obj->location.memLocation.memAddr   = buff;
		obj->location.memLocation.size      = numRead;
        obj->objectMimeType 			    = "text/x-vCard";
    }
    else
    {
    	Report(("Client: Error - could not open %s", fsPushPath));
        return FALSE;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------
 * mfw_btips_oppcSetMemPullObject()
 *
 * Synopsis:  Function for testing OPP from memory objects. 
 *            Sets memory object fields prior to PULL operation
 *
 * Return:    void
 *
 */
static void mfw_btips_oppcSetMemPullObject(BtlObject *obj, const char *fsPullPath, char *buff)
{			
	OBSTORE_ExtractObjName(fsPullPath, ObPullName);
    
    obj->objectLocation                 = BTL_OBJECT_LOCATION_MEM;
	
    obj->location.fsLocation.fsPath     = NULL;
	obj->location.memLocation.memAddr   = buff;
	obj->location.memLocation.size      = OPPCA_BUFF_MAX;		

    obj->objectMimeType 		        = "text/x-vCard";;
    obj->objectName                     = ObPullName;
}


/*---------------------------------------------------------------------------
 * mfw_btips_oppcSetFsPushObject()
 *
 * Synopsis:  Function for testing OPP from FS Push objects. 
  *
 * Return:    void
 *
 */
static void mfw_btips_oppcSetFsPushObject(BtlObject *obj, const char *fsPushPath)
{			
    OBSTORE_ExtractObjName(fsPushPath, ObPushName);

    obj->objectName      			    = ObPushName;
	obj->location.fsLocation.fsPath     = fsPushPath;
	
    /* This should be provided by the user (i.e. this is just an example) */
    obj->objectMimeType 			    = "text/x-vCard";
	obj->objectLocation 			    = BTL_OBJECT_LOCATION_FS;
}


/*---------------------------------------------------------------------------
 * mfw_btips_oppcSetFsPullObject()
 *
 * Synopsis:  Function for testing OPP from FS Pull objects. 
  *
 * Return:    void
 *
 */
static void mfw_btips_oppcSetFsPullObject(BtlObject *obj, const char *fsPullPath)
{			
    OBSTORE_ExtractObjName(fsPullPath, ObPullName);

    obj->objectName      			    = ObPullName;
	obj->location.fsLocation.fsPath     = fsPullPath;
	obj->objectMimeType 			    = "text/x-vCard";
	obj->objectLocation 			    = BTL_OBJECT_LOCATION_FS;
}

/*---------------------------------------------------------------------------
 *            mfw_btips_oppcCallback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes OBEX client protocol events.
 *
 * Return:    void
 *
 */
void mfw_btips_oppcCallback(T_BTIPS_MMI_IND *para)
{

}

/*******************************************************************************

 $Function:     mfw_btips_oppcPopulateFiles

 $Description:Populates file names into the array

 $Returns:		The count of the files in the directory

 $Arguments:		file_names:Pointer to the arrayto store file names
				max_files_count:Max file counts to be stored in array
*******************************************************************************/
int mfw_btips_oppcPopulateFiles(char **file_names, int max_files_count,char* dir_name_p)
{

	T_FFS_DIR	dir                             = {0};
	char		       dest_name_p[100]    =  "";
	int                  i, files_count            =  0;
	char               *fileExt;

	TRACE_FUNCTION_P1("mfw_btips_oppcPopulateFiles from directory %s", dir_name_p);

	if (ffs_opendir (dir_name_p, &dir) <= 0)
	{
	       TRACE_ERROR("Could not open Dir");
		
		files_count = 0;
	}
	//Release previous allocated memory before allocating again
	mfw_btips_oppcUnpopulateFiles(file_names);
	//read the  file names
	//file_count will be used to populate the array index
	for (i = 0; ffs_readdir (&dir, dest_name_p, 100) > 0x0; i++)
	{
		if((strcmp(dest_name_p, ".") == 0) || (strcmp(dest_name_p, "..") == 0))
			continue;
		TRACE_FUNCTION_P1("Reading Dir - %s",dest_name_p);
		(file_names[files_count]) = (char*)mfwAlloc(strlen(dest_name_p) + 1);
		if(file_names[files_count] != NULL)
		{
			#if 0
			//Retrieve file extension.Store only the supported file formats in array
			fileExt =mfw_btips_oppcGetExtension(dest_name_p);
			//List the MP3 files also.Mp3 files hsould not be listed or ULC
			if ( (strcmp(fileExt, "mp3") == 0))
			{
				strcpy (file_names[files_count], dest_name_p);
				files_count++;
			}
			else if ( (strcmp(fileExt, "wav")== 0))
			{
				strcpy (file_names[files_count], dest_name_p);
				files_count++;
			}
			else if ( (strcmp(fileExt, "pcm")== 0))
			{
				strcpy (file_names[files_count], dest_name_p);
				files_count++;
			}
			else if(files_count >= max_files_count)
				break;
			#endif
			strcpy (file_names[files_count], dest_name_p);
			TRACE_FUNCTION_P1("file_names - %s", file_names[files_count]);
			files_count++;
			if(files_count >= max_files_count)
				break;
		}		
		else
		{
			//files_count = 0;
		}		

	}
	//Closing the directory aa per the new FFS logic
	ffs_closedir(dir);
 
	TRACE_EVENT_P1("read  files_count is %d",files_count);
	TRACE_FUNCTION_P1("read  files_count is %d",files_count);
	return files_count;
}
/*******************************************************************************

 $Function:     mfw_btips_oppcUnpopulateFiles

 $Description:Frees the memory allocated for the array
 $Returns:		None

 $Arguments:		file_names:Pointer to the arrayto store file names

*******************************************************************************/
void mfw_btips_oppcUnpopulateFiles(char **file_names)
{
	int i;
	TRACE_FUNCTION("mfw_btips_oppcUnpopulateFiles");
	mfw_BtipsReport("mfw_btips_oppcUnpopulateFiles");
	
	//Free the allocated memory for file names
	for (i = 0; i<OPPC_MAX_FILES; i++)
	{
		if(file_names[i])
		{
	           mfwFree((U8 *)file_names[i],(U16)strlen(file_names[i]) + 1);
		}
		file_names[i]=NULL;
	}
    
}

/*******************************************************************************

 $Function:     mfw_btips_oppcGetExtension

 $Description: public function to retrieve the extension of a file

 $Returns:Extention of the filename

 $Arguments: scr- Filename

*******************************************************************************/
 char *mfw_btips_oppcGetExtension(char *src)
{
    int i,len;

	TRACE_FUNCTION("mfw_btips_oppcGetExtension");
		mfw_BtipsReport("mfw_btips_oppcGetExtension");
    len = strlen(src);  
    for(i = 0; i < len; i++){
        if(src[i] == '.'){
            return (src+i+1);
        }
    }
    return (src+i);
}
/*******************************************************************************

 $Function:     mfw_btips_oppcGetNumFiles

 $Description: returns the file count 

 $Returns:		Count of the Midi files

 $Arguments:		None

*******************************************************************************/
int mfw_btips_oppcGetNumFiles(void)
{
	TRACE_FUNCTION_P1("mfw_btips_oppcGetNumFiles -oppc_files_count - %d", oppc_files_count);	
	if(oppc_files_count<=0)
		return 0;
	else
		return oppc_files_count; 

}
/*******************************************************************************

 $Function:     mfw_btips_oppcGetFileName

 $Description: Returns the file name 

 $Returns:		Name of the file 

 $Arguments:		index:The index of the file, whose name has to be retrieved

*******************************************************************************/

char* mfw_btips_oppcGetFileName(UBYTE index)
{

	TRACE_FUNCTION_P2("oppc_file_names[%d] is %s",index,(char *)oppc_file_names[index]);
	return (char *) oppc_file_names[index];
}
/*******************************************************************************

 $Function:     mfw_btips_oppcGetConnectDevice

 $Description: Returns the TRUE when succeeds
 $Returns:		TRUE or FALSE 

 $Arguments:		

*******************************************************************************/

BOOL mfw_btips_oppcGetConnectedDevice (BD_ADDR *bdAddr, U8 *name)
{
	BtStatus status;
	char bdaddrstring[BDADDR_NTOA_SIZE];
	
	status = BTL_OPPC_GetConnectedDevice (oppcContext, bdAddr);
	bdaddr_ntoa(bdAddr,bdaddrstring);
	TRACE_FUNCTION_P1("mfw_btips_ftpsGetConnectDevice OPPC %s  ",bdaddrstring);
	
	if (status == BT_STATUS_SUCCESS)
	{
		mfw_btips_bmgGetDeviceName (bdAddr, name);
		TRACE_FUNCTION_P1("mfw_btips_oppcGetConnectDevice OPPC name %s  ",name);
		return TRUE;
	}
	return FALSE;
}

void mfw_btips_oppc_Disconnect(void)
{
	BtStatus status; 	

    if (oppClient.isConnected == TRUE)
	{
        status = BTL_OPPC_Disconnect(oppcContext);
		Report(("BTL_OPPC_Disconnect() returned %s.", pBT_Status(status)));
	}
	else
	{
		Report(("mfw_btips_oppc_Disconnect: Client is not connected!"));
	}
}


#endif

