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
 $Module:   Bluetooth BMG APPlication
 $File:       mfw_BtipsA2dp.c
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS BMG APPlication functionality.

********************************************************************************
 $History: mfw_BtipsA2dp.c

  26/06/07 Sasken original version

 $End

*******************************************************************************/




/*******************************************************************************

                                Include files

*******************************************************************************/

#define ENTITY_MFW

#if BT_STACK == XA_ENABLED
#include <me.h>
#include <bttypes.h>
#include <sec.h>
#endif

#include "btl_common.h"
#include "debug.h"
#include "vsi.h"
#include "overide.h"

/* BTL includes */
#include "btl_a2dp.h"
#include "btl_config.h"
#include "bthal_mm.h"


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
#include "mfw_BtipsA2dp.h"
#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_icn.h"	/* included for mfw_mnu.h */
#include "mfw_mnu.h"
#include "mfw_tim.h"
#include "mfw_kbd.h"
#include "mfw_sat.h" /* included for MmiDummy.h, included for MmiMenu.h */
#include "mfw_btips.h"
#include "mfw_ffs.h"

extern char szSelectedFileName[256];
extern char* szSelectedFileExt;
extern int   a2dp_files_count;
//Array to load fileNames in content directory
extern char* a2dp_file_names[A2DP_MAX_FILES]; 


/*-------------------------------------------------------------------------------
 * MAX_SUPPORTED_BITPOOL type
 *
 *     Represents the maximum supported bitpool
 */
#define MAX_SUPPORTED_BITPOOL				(53) 

/*-------------------------------------------------------------------------------
 * MIN_SUPPORTED_BITPOOL type
 *
 *     Represents the minimum supported bitpool
 */
#define MIN_SUPPORTED_BITPOOL				(2) 

#define A2DP_MAX_NUM_STREAMS  (BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT + \
							   BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT)
static BOOL isStreamClosed[A2DP_MAX_NUM_STREAMS];
static BOOL  playingNow = FALSE;
static BOOL  streamingNow = FALSE;
static BtSecurityLevel     securityLevel = BSL_NO_SECURITY;
static BtlA2dpContext 	  *btlA2dpContext = 0;
static BtlA2dpCodec		   codec;
BtlA2dpStreamId     streamId = -1;
static BthalMmSbcInfo	   sbcLocalCap;	
static BthalMmMpeg1_2_audioInfo	   mp3LocalCap;
static BOOL	streamSbcFile = FALSE;
extern T_MFW_HND hA2dpDeviceWnd;
BOOL bPlayMp3File = FALSE;
BOOL bPlaySbcFile = FALSE;

void APP_A2DP_StartPcmStream(BTHAL_U32 streamId);

static void  mfw_btips_a2dpEventCallBack(const BtlA2dpEvent *event);
static char * pCodecType(AvdtpCodecType CType);
static char * pAvError(AvdtpError Error);
static void mfw_btips_a2dpSetDefaultSbcLocalCap(void);
static void mfw_btips_a2dpSetDefaultMp3LocalCap(void);

#ifdef FF_MMI_BTIPS_APP

/*Function Definitions*/
void APP_A2DP_StartPcmStream(BTHAL_U32 streamId)
{
	BtStatus status;
	status = BTL_A2DP_StartStream(btlA2dpContext, streamId);
       TRACE_FUNCTION_P1("BTL_A2DP_StartStream() returned %s.", 
                      pBT_Status(status));
}

/*******************************************************************************

 $Function:       mfw_btips_a2dpInit

 $Description:   Function to initialise the a2dp file list.

 $Returns:		None

 $Arguments:		None

*******************************************************************************/
void mfw_btips_a2dpInit(void)
{
	BtStatus status;
	TRACE_FUNCTION("mfw_btips_a2dpInit");

	status = BTL_A2DP_Create(0, mfw_btips_a2dpEventCallBack, &securityLevel, &btlA2dpContext);
	mfw_btips_a2dpSetDefaultSbcLocalCap();
	mfw_btips_a2dpSetDefaultMp3LocalCap();
	bthalMmSetExternalSbcEncoder(streamSbcFile);
	bthalMmSetPcmPlatformSupportedCapabilities(&sbcLocalCap);
	TRACE_FUNCTION("SBC Local capabilities has been set");
	
	bthalMmSetMp3PlatformSupportedCapabilities(&mp3LocalCap);
	TRACE_FUNCTION("MP3 Local capabilities has been set");
	
	status = BTL_A2DP_Enable(btlA2dpContext, 0, (BTL_A2DP_SUPPORTED_FEATURES_PLAYER | BTL_A2DP_SUPPORTED_FEATURES_MIXER));

	TRACE_FUNCTION_P1("BTL_A2DP_Enable() returned %s.", pBT_Status(status));
	if (status == BT_STATUS_SUCCESS)
	{
		OS_MemSet(isStreamClosed, TRUE, A2DP_MAX_NUM_STREAMS);
		
	}	
	else
	{
		TRACE_FUNCTION("A2DP Enable Failed!");
	}
}
/*******************************************************************************

 $Function:     mfw_btips_a2dpPopulateFiles

 $Description:Populates the MIDI file names into the array

 $Returns:		The count of the files in the directory

 $Arguments:		file_names:Pointer to the arrayto store file names
				max_files_count:Max file counts to be stored in array
*******************************************************************************/

int mfw_btips_a2dpPopulateFiles(char **file_names, int max_files_count,char* dir_name_p)
{

	T_FFS_DIR	dir                             = {0};
	char		       dest_name_p[100]    =  "";
	int                  i, files_count            =  0;
	char               *fileExt;

	TRACE_FUNCTION("mfw_btips_a2dpPopulateFiles");

       TRACE_FUNCTION_P1("a2dp_files_count - %s", dir_name_p);
	if (ffs_opendir (dir_name_p, &dir) <= 0)
	{
	       TRACE_FUNCTION("Cld not open Dir");
		
		files_count = 0;
	}
	//Release previous allocated memory before allocating again
	mfw_btips_a2dpUnpopulateFiles(file_names);
	//read the  file names
	//file_count will be used to populate the array index
	for (i = 0; ffs_readdir (&dir, dest_name_p, 100) > 0x0; i++)
	{
	
		TRACE_FUNCTION_P1("Reading Dir - %s",dest_name_p);
		(file_names[files_count]) = (char*)mfwAlloc(strlen(dest_name_p) + 1);
		if(file_names[files_count] != NULL)
		{
			//Retrieve file extension.Store only the supported file formats in array
			fileExt =mfw_btips_a2dpGetExtension(dest_name_p);
			//List the MP3 files also.Mp3 files hsould not be listed or ULC
			if ( (strcmp(fileExt, "mp3") == 0))
			{
				
				strcpy (file_names[files_count], dest_name_p);
				TRACE_FUNCTION_P1("file_names - %s", file_names[files_count]);
				files_count++;
			}
			else if ( (strcmp(fileExt, "wav")== 0))
			{
				
				strcpy (file_names[files_count], dest_name_p);
				TRACE_FUNCTION_P1("file_names - %s", file_names[files_count]);
				files_count++;
			}
			else if ( (strcmp(fileExt, "pcm")== 0))
			{
				strcpy (file_names[files_count], dest_name_p);
				TRACE_FUNCTION_P1("file_names - %s", file_names[files_count]);
				files_count++;
			}
			else if(files_count >= max_files_count)
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
	return files_count;
}
/*******************************************************************************

 $Function:     mfw_btips_a2dpUnpopulateFiles

 $Description:Frees the memory allocated for the array
 $Returns:		None

 $Arguments:		file_names:Pointer to the arrayto store file names

*******************************************************************************/
void mfw_btips_a2dpUnpopulateFiles(char **file_names)
{
	int i;
	TRACE_FUNCTION("mfw_btips_a2dpUnpopulateFiles");
	
	//Free the allocated memory for file names
	for (i = 0; i<A2DP_MAX_FILES; i++)
	{
		if(file_names[i])
		{
	           mfwFree((U8 *)file_names[i],(U16)strlen(file_names[i]) + 1);
		}
		file_names[i]=NULL;
	}
    
}
/*******************************************************************************

 $Function:     mfw_btips_a2dpGetExtension

 $Description: public function to retrieve the extension of a file

 $Returns:Extention of the filename

 $Arguments: scr- Filename

*******************************************************************************/
 char *mfw_btips_a2dpGetExtension(char *src)
{
    int i,len;

	TRACE_FUNCTION("mfw_btips_a2dpGetExtension");
    len = strlen(src);  
    for(i = 0; i < len; i++){
        if(src[i] == '.'){
            return (src+i+1);
        }
    }
    return (src+i);
}
/*******************************************************************************

 $Function:     sounds_midi_return_file_number

 $Description: returns the file count 

 $Returns:		Count of the Midi files

 $Arguments:		None

*******************************************************************************/
int mfw_btips_a2dpGetNumFiles(void)
{
	TRACE_FUNCTION("sounds_midi_return_file_number");
	TRACE_FUNCTION_P1("mfw_btips_a2dpGetNumFiles -a2dp_files_count - %d", a2dp_files_count);	
	if(a2dp_files_count<=0)
		return 0;
	else
		return a2dp_files_count; 

}
/*******************************************************************************

 $Function:     mfw_btips_a2dpGetFileName

 $Description: Returns the file name 

 $Returns:		Name of the file 

 $Arguments:		index:The index of the file, whose name has to be retrieved

*******************************************************************************/

char* mfw_btips_a2dpGetFileName(UBYTE index)
{

	TRACE_FUNCTION("mfw_btips_a2dpGetFileName");
	TRACE_EVENT_P2("A2dp_file_names[%d] is %s",index,(char *)a2dp_file_names[index]);
	return (char *) a2dp_file_names[index];

}

/*******************************************************************************

 $Function:     mfw_btips_a2dpEventCallBack

 $Description: Returns the file name 

 $Returns:		Name of the file 

 $Arguments:		index:The index of the file, whose name has to be retrieved

*******************************************************************************/

static void  mfw_btips_a2dpEventCallBack(const BtlA2dpEvent *event)
{

	A2dpError      error = A2DP_ERR_NO_ERROR;
	A2dpCallbackParms 	*Parms;
	BOOL streamStopped = FALSE;
	BtStatus status;
//	char szSelectedFilePath[256]

	Parms = event->callbackParms;

	TRACE_FUNCTION_P1("mfw_btips_a2dpEventCallBack %d",Parms->event);

       switch (Parms->event) {
       case A2DP_EVENT_STREAM_OPEN_IND:
	        TRACE_FUNCTION_P1("A2DP_EVENT_STREAM_OPEN_IND streamId %d.", event->streamId);
	        TRACE_FUNCTION_P2("    Codec: %s, Length: %d",
               pCodecType(Parms->p.configReq->codec.codecType),
               Parms->p.configReq->codec.elemLen);

		/*if (promptUserOnIncomingConAndStart)
		{
			AppReport("Please Accept or Reject open stream request");
		}
		else
		*/
		{
			/* Auto-accept connection */
			status = BTL_A2DP_OpenStreamResponse(btlA2dpContext, event->streamId, A2DP_ERR_NO_ERROR);
			TRACE_FUNCTION_P1("BTL_A2DP_OpenStreamResponse() returned %s.", 
                     pBT_Status(status));
		}
		
        break;  
		
    case A2DP_EVENT_GET_CONFIG_IND:
		TRACE_FUNCTION_P1("A2DP_EVENT_GET_CONFIG_IND streamId %d.", event->streamId);
		TRACE_FUNCTION("To configure and open strean press 'Open Stream' button ");
		/*if(streamId > 0)
		{
			mfw_btips_a2dpCloseStream();
		}*/
			
		status = BTL_A2DP_OpenStream(btlA2dpContext, streamId);
		isStreamClosed[event->streamId] = FALSE;
		bthalMmStreamStateIsConnected(event->streamId, TRUE);
		TRACE_FUNCTION_P1("BTL_A2DP_OpenStream() returned %s.", 
              pBT_Status(status));
        break;

    case A2DP_EVENT_STREAM_OPEN:
		TRACE_FUNCTION_P1("A2DP_EVENT_STREAM_OPEN streamId %d.", event->streamId);
		isStreamClosed[event->streamId] = FALSE;
		bthalMmStreamStateIsConnected(event->streamId, TRUE);
		if(bPlaySbcFile)
		{
			TRACE_FUNCTION("Opening PCM File ");
			if(AppPcmSimOpenFile(szSelectedFileName))
			{
				TRACE_FUNCTION("PCM File Open Success!!");
			}
			else
			{
				TRACE_FUNCTION("PCM File Open Failed!!");
			}
       	}
			
		if(bPlayMp3File)
		{
			TRACE_FUNCTION("Opening Mp3 File ");
			if(AppMp3OpenFile(szSelectedFileName))
			{
				TRACE_FUNCTION("Mp3 File Open Success!!");
			}
			else
			{
				TRACE_FUNCTION("Mp3 File Open Failed!!");
			}
			status = BTL_A2DP_StartStream(btlA2dpContext, streamId);
       		TRACE_FUNCTION_P1("BTL_A2DP_StartStream() returned %s.", 
                      pBT_Status(status));
		
		}
		
		
        break;

    case A2DP_EVENT_STREAM_IDLE:
		TRACE_FUNCTION_P1("A2DP_EVENT_STREAM_IDLE streamId %d.", event->streamId);
		isStreamClosed[event->streamId] = FALSE;
		streamStopped = TRUE;
		
        break;

    case A2DP_EVENT_STREAM_CLOSED:
		TRACE_FUNCTION_P1("A2DP_EVENT_STREAM_CLOSED streamId %d.", event->streamId);
		isStreamClosed[event->streamId] = TRUE;
		streamStopped = TRUE;
		bthalMmStreamStateIsConnected(event->streamId, FALSE);
        break;

    case A2DP_EVENT_STREAM_START_IND:
        	TRACE_FUNCTION_P1("A2DP_EVENT_STREAM_START_IND streamId %d.", event->streamId);

		/*if (promptUserOnIncomingConAndStart)
		{
			AppReport("Please Accept or Reject start stream request");
		}
		else
		*/
		{
			/* Auto-accept stream start */
			status = BTL_A2DP_StartStreamResponse(btlA2dpContext, event->streamId, A2DP_ERR_NO_ERROR);
			TRACE_FUNCTION_P1("BTL_A2DP_StartStreamResponse() returned %s.", 
                      pBT_Status(status));
		}
		
        break;

    case A2DP_EVENT_STREAM_STARTED:
		TRACE_FUNCTION_P1("A2DP_EVENT_STREAM_STARTED streamId %d.", event->streamId);

		isStreamClosed[event->streamId] = FALSE;
		streamingNow = TRUE;
		if(bPlaySbcFile)
		{
			TRACE_FUNCTION("Playing PCM File ");
			AppPcmSimPlayFile();
       	}
		else if(bPlayMp3File)
		{
			TRACE_FUNCTION("Playing Mp3 File ");
			AppMp3PlayFile();
		
		}
		TRACE_FUNCTION_P1("Playing File  %s : ", szSelectedFileName);
        break;

    case A2DP_EVENT_STREAM_SUSPENDED:
		TRACE_FUNCTION_P1("A2DP_EVENT_STREAM_SUSPENDED streamId %d.", event->streamId);

		isStreamClosed[event->streamId] = FALSE;
		
		if (Parms->error != AVDTP_ERR_NO_ERROR)
		{
		    TRACE_FUNCTION_P2("Stream Suspend Failed, reason: %s, status: %s", 
                  pAvError(Parms->error),
                  pBT_Status(Parms->status));
		}
		/*switch (event->streamId)
		{
			case (0):
				SendDlgItemMessage(AppWnd, IDC_STREAM0_OPEND, BM_SETCHECK, BST_CHECKED, 0L);
				SendDlgItemMessage(AppWnd, IDC_STREAM0_STREAMING, BM_SETCHECK, BST_UNCHECKED, 0L);
				break;

			case (1):
				SendDlgItemMessage(AppWnd, IDC_STREAM1_OPEND, BM_SETCHECK, BST_CHECKED, 0L);
				SendDlgItemMessage(AppWnd, IDC_STREAM1_STREAMING, BM_SETCHECK, BST_UNCHECKED, 0L);
				break;

			case (2):
				SendDlgItemMessage(AppWnd, IDC_STREAM2_OPEND, BM_SETCHECK, BST_CHECKED, 0L);
				SendDlgItemMessage(AppWnd, IDC_STREAM2_STREAMING, BM_SETCHECK, BST_UNCHECKED, 0L);
				break;

			case (3):
				SendDlgItemMessage(AppWnd, IDC_STREAM3_OPEND, BM_SETCHECK, BST_CHECKED, 0L);
				SendDlgItemMessage(AppWnd, IDC_STREAM3_STREAMING, BM_SETCHECK, BST_UNCHECKED, 0L);
				break;
		}
		*/
        break;

    case A2DP_EVENT_STREAM_ABORTED:
		TRACE_FUNCTION_P1("A2DP_EVENT_STREAM_ABORTED streamId %d.", event->streamId);
        if (Parms->error != AVDTP_ERR_NO_ERROR) {
            TRACE_FUNCTION_P2("Stream Abort Failed, reason: %s, status: %s", 
                      pAvError(Parms->error),
                      pBT_Status(Parms->status));
        }
		/*switch (event->streamId)
		{
			case (0):
				if(isStreamClosed[0] == FALSE)
				{
					
					SendDlgItemMessage(AppWnd, IDC_STREAM0_IDLE, BM_SETCHECK, BST_CHECKED, 0L);
				}
				SendDlgItemMessage(AppWnd, IDC_STREAM0_OPEND, BM_SETCHECK, BST_UNCHECKED, 0L);
				SendDlgItemMessage(AppWnd, IDC_STREAM0_STREAMING, BM_SETCHECK, BST_UNCHECKED, 0L);
				break;

			case (1):
				if(isStreamClosed[1] == FALSE)
				{
					
					SendDlgItemMessage(AppWnd, IDC_STREAM1_IDLE, BM_SETCHECK, BST_CHECKED, 0L);
				}
				SendDlgItemMessage(AppWnd, IDC_STREAM1_OPEND, BM_SETCHECK, BST_UNCHECKED, 0L);
				SendDlgItemMessage(AppWnd, IDC_STREAM1_STREAMING, BM_SETCHECK, BST_UNCHECKED, 0L);
				break;

			case (2):
				if(isStreamClosed[2] == FALSE)
				{
					
					SendDlgItemMessage(AppWnd, IDC_STREAM2_IDLE, BM_SETCHECK, BST_CHECKED, 0L);
				}
				SendDlgItemMessage(AppWnd, IDC_STREAM2_OPEND, BM_SETCHECK, BST_UNCHECKED, 0L);
				SendDlgItemMessage(AppWnd, IDC_STREAM2_STREAMING, BM_SETCHECK, BST_UNCHECKED, 0L);
				break;

			case (3):
				if(isStreamClosed[3] == FALSE)
				{
					
					SendDlgItemMessage(AppWnd, IDC_STREAM3_IDLE, BM_SETCHECK, BST_CHECKED, 0L);
				}
				SendDlgItemMessage(AppWnd, IDC_STREAM3_OPEND, BM_SETCHECK, BST_UNCHECKED, 0L);
				SendDlgItemMessage(AppWnd, IDC_STREAM3_STREAMING, BM_SETCHECK, BST_UNCHECKED, 0L);
				break;
		}
*/
		isStreamClosed[event->streamId] = FALSE;
		streamStopped = TRUE;
		bPlaySbcFile = FALSE;
		bPlayMp3File = FALSE;
        break;

	case A2DP_EVENT_DISABLED:
		TRACE_FUNCTION("A2DP_EVENT_DISABLED");
		/*SendDlgItemMessage(AppWnd, IDC_STREAM0_IDLE, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM0_OPEND, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM0_STREAMING, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM0_CLOSED, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM1_IDLE, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM1_OPEND, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM1_STREAMING, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM1_CLOSED, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM2_IDLE, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM2_OPEND, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM2_STREAMING, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM2_CLOSED, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM3_IDLE, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM3_OPEND, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM3_STREAMING, BM_SETCHECK, BST_UNCHECKED, 0L);
		SendDlgItemMessage(AppWnd, IDC_STREAM3_CLOSED, BM_SETCHECK, BST_UNCHECKED, 0L);
		*/
		streamStopped = TRUE;
		
        break;

	default: 
		TRACE_FUNCTION_P1("MFW A2DP Unexpected event %d!", Parms->event);
        break;
    }
	
	if ( streamStopped )
	{
		streamingNow = FALSE;
	}
}
/*---------------------------------------------------------------------------
 *            pCodecType()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Prints codec type message
 *
 */
static char * pCodecType(AvdtpCodecType CType)
{
    switch (CType) {
    case AVDTP_CODEC_TYPE_SBC:
        return "SBC";
    case AVDTP_CODEC_TYPE_MPEG1_2_AUDIO:
        return "MPEG1_2_AUDIO";
    case AVDTP_CODEC_TYPE_MPEG2_4_AAC:
        return "MPEG2_4_AAC";
    case AVDTP_CODEC_TYPE_ATRAC:
        return "ATRAC";
    case AVDTP_CODEC_TYPE_NON_A2DP:
        return "Non-A2DP";
    default:
        return "UNKNOWN";
    }
}
/*---------------------------------------------------------------------------
 *            pAvError()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Prints AV error message
 *
 */
static char * pAvError(AvdtpError Error)
{
    switch (Error) {
    case AVDTP_ERR_NO_ERROR:
        return "AVDTP_ERR_NO_ERROR";
    case AVDTP_ERR_BAD_HEADER_FORMAT:
        return "AVDTP_ERR_BAD_HEADER_FORMAT";
    case AVDTP_ERR_BAD_LENGTH:
        return "AVDTP_ERR_BAD_LENGTH";
    case AVDTP_ERR_BAD_ACP_SEID:
        return "AVDTP_ERR_BAD_ACP_SEID";
    case AVDTP_ERR_IN_USE:
        return "AVDTP_ERR_IN_USE";
    case AVDTP_ERR_NOT_IN_USE:
        return "AVDTP_ERR_NOT_IN_USE";
    case AVDTP_ERR_BAD_SERV_CATEGORY:
        return "AVDTP_ERR_BAD_SERV_CATEGORY";
    case AVDTP_ERR_BAD_PAYLOAD_FORMAT:
        return "AVDTP_ERR_BAD_PAYLOAD_FORMAT";
    case AVDTP_ERR_NOT_SUPPORTED_COMMAND:
        return "AVDTP_ERR_NOT_SUPPORTED_COMMAND";
    case AVDTP_ERR_INVALID_CAPABILITIES:
        return "AVDTP_ERR_INVALID_CAPABILITIES";
    case AVDTP_ERR_BAD_RECOVERY_TYPE:
        return "AVDTP_ERR_BAD_RECOVERY_TYPE";
    case AVDTP_ERR_BAD_MEDIA_TRANSPORT_FORMAT:
        return "AVDTP_ERR_BAD_MEDIA_TRANSPORT_FORMAT";
    case AVDTP_ERR_BAD_RECOVERY_FORMAT:
        return "AVDTP_ERR_BAD_RECOVERY_FORMAT";
    case AVDTP_ERR_BAD_ROHC_FORMAT:
        return "AVDTP_ERR_BAD_ROHC_FORMAT";
    case AVDTP_ERR_BAD_CP_FORMAT:
        return "AVDTP_ERR_BAD_CP_FORMAT";
    case AVDTP_ERR_BAD_MULTIPLEXING_FORMAT:
        return "AVDTP_ERR_BAD_MULTIPLEXING_FORMAT";
    case AVDTP_ERR_UNSUPPORTED_CONFIGURATION:
        return "AVDTP_ERR_UNSUPPORTED_CONFIGURATION";
    case AVDTP_ERR_BAD_STATE:
        return "AVDTP_ERR_BAD_STATE";
    case A2DP_ERR_BAD_SERVICE:
        return" A2DP_ERR_BAD_SERVICE";
    case A2DP_ERR_INSUFFICIENT_RESOURCE:
        return" A2DP_ERR_INSUFFICENT_RESOURCE";
    case A2DP_ERR_INVALID_CODEC_TYPE:
        return" A2DP_ERR_INVALID_CODEC_TYPE";
    case A2DP_ERR_NOT_SUPPORTED_CODEC_TYPE:
        return" A2DP_ERR_NOT_SUPPORTED_CODEC_TYPE";
    case A2DP_ERR_INVALID_SAMPLING_FREQUENCY:
        return" A2DP_ERR_INVALID_SAMPLING_FREQUENCY";
    case A2DP_ERR_NOT_SUPPORTED_SAMP_FREQ:
        return" A2DP_ERR_NOT_SUPPORTED_SAMP_FREQ";
    case A2DP_ERR_INVALID_CHANNEL_MODE:
        return" A2DP_ERR_INVALID_CHANNEL_MODE";
    case A2DP_ERR_NOT_SUPPORTED_CHANNEL_MODE:
        return" A2DP_ERR_NOT_SUPPORTED_CHANNEL_MODE";
    case A2DP_ERR_INVALID_SUBBANDS:
        return" A2DP_ERR_INVALID_SUBBANDS";
    case A2DP_ERR_NOT_SUPPORTED_SUBBANDS:
        return" A2DP_ERR_NOT_SUPPORTED_SUBBANDS";
    case A2DP_ERR_INVALID_ALLOCATION_METHOD:
        return" A2DP_ERR_INVALID_ALLOCATION_METHOD";
    case A2DP_ERR_NOT_SUPPORTED_ALLOC_METHOD:
        return" A2DP_ERR_NOT_SUPPORTED_ALLOC_METHOD";
    case A2DP_ERR_INVALID_MIN_BITPOOL_VALUE:
        return" A2DP_ERR_INVALID_MIN_BITPOOL_VALUE";
    case A2DP_ERR_NOT_SUPPORTED_MIN_BITPOOL_VALUE:
        return" A2DP_ERR_NOT_SUPPORTED_MIN_BITPOOL_VALUE";
    case A2DP_ERR_INVALID_MAX_BITPOOL_VALUE:
        return" A2DP_ERR_INVALID_MAX_BITPOOL_VALUE";
    case A2DP_ERR_NOT_SUPPORTED_MAX_BITPOOL_VALUE:
        return" A2DP_ERR_NOT_SUPPORTED_MAX_BITPOOL_VALUE";
    case A2DP_ERR_INVALID_LAYER:
        return" A2DP_ERR_INVALID_LAYER";
    case A2DP_ERR_NOT_SUPPORTED_LAYER:
        return" A2DP_ERR_NOT_SUPPORTED_LAYER";
    case A2DP_ERR_NOT_SUPPORTED_CRC:
        return" A2DP_ERR_NOT_SUPPORTED_CRC";
    case A2DP_ERR_NOT_SUPPORTED_MPF:
        return" A2DP_ERR_NOT_SUPPORTED_MPF";
    case A2DP_ERR_NOT_SUPPORTED_VBR:
        return" A2DP_ERR_NOT_SUPPORTED_VBR";
    case A2DP_ERR_INVALID_BIT_RATE:
        return" A2DP_ERR_INVALID_BIT_RATE";
    case A2DP_ERR_NOT_SUPPORTED_BIT_RATE:
        return" A2DP_ERR_NOT_SUPPORTED_BIT_RATE";
    case A2DP_ERR_INVALID_OBJECT_TYPE:
        return" A2DP_ERR_INVALID_OBJECT_TYPE";
    case A2DP_ERR_NOT_SUPPORTED_OBJECT_TYPE:
        return" A2DP_ERR_NOT_SUPPORTED_OBJECT_TYPE";
    case A2DP_ERR_INVALID_CHANNELS:
        return" A2DP_ERR_INVALID_CHANNELS";
    case A2DP_ERR_NOT_SUPPORTED_CHANNELS:
        return" A2DP_ERR_NOT_SUPPORTED_CHANNELS";
    case A2DP_ERR_INVALID_VERSION:
        return" A2DP_ERR_INVALID_VERSION";
    case A2DP_ERR_NOT_SUPPORTED_VERSION:
        return" A2DP_ERR_NOT_SUPPORTED_VERSION";
    case A2DP_ERR_NOT_SUPPORTED_MAXIMUM_SUL:
        return" A2DP_ERR_NOT_SUPPORTED_MAXIMUM_SUL";
    case A2DP_ERR_INVALID_BLOCK_LENGTH:
        return" A2DP_ERR_INVALID_BLOCK_LENGTH";
    case A2DP_ERR_INVALID_CP_TYPE:
        return" A2DP_ERR_INVALID_CP_TYPE";
    case A2DP_ERR_INVALID_CP_FORMAT:
        return" A2DP_ERR_INVALID_CP_FORMAT";
    default:
        return "UNKNOWN";
    }
}


/*---------------------------------------------------------------------------
 *            mfw_btips_a2dpSetDefaultSbcLocalCap()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Save local SBC capabilities
 *
 */
static void mfw_btips_a2dpSetDefaultSbcLocalCap(void)
{
	sbcLocalCap.samplingFreq = ( BTHAL_MM_SBC_SAMPLING_FREQ_16000 | BTHAL_MM_SBC_SAMPLING_FREQ_32000|BTHAL_MM_SBC_SAMPLING_FREQ_44100 |BTHAL_MM_SBC_SAMPLING_FREQ_48000);
	sbcLocalCap.channelMode = BTHAL_MM_SBC_CHANNEL_MODE_MONO | BTHAL_MM_SBC_CHANNEL_MODE_DUAL_CHANNEL | BTHAL_MM_SBC_CHANNEL_MODE_STEREO | BTHAL_MM_SBC_CHANNEL_MODE_JOINT_STEREO;
	sbcLocalCap.blockLength = (BTHAL_MM_SBC_BLOCK_LENGTH_4 | BTHAL_MM_SBC_BLOCK_LENGTH_8 | BTHAL_MM_SBC_BLOCK_LENGTH_12 | BTHAL_MM_SBC_BLOCK_LENGTH_16);
	sbcLocalCap.subbands = BTHAL_MM_SBC_SUBBANDS_8 | BTHAL_MM_SBC_SUBBANDS_4;
	sbcLocalCap.allocationMethod = BTHAL_MM_SBC_ALLOCATION_METHOD_LOUDNESS | BTHAL_MM_SBC_ALLOCATION_METHOD_SNR;
	sbcLocalCap.maxBitpoolValue = MAX_SUPPORTED_BITPOOL;
	sbcLocalCap.minBitpoolValue = MIN_SUPPORTED_BITPOOL;
}

/*---------------------------------------------------------------------------
 *            mfw_btips_a2dpSetDefaultMp3LocalCap()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Save local PM3 capabilities
 *
 */
static void mfw_btips_a2dpSetDefaultMp3LocalCap(void)
{
	mp3LocalCap.bitRate = 0xFFFF;
	mp3LocalCap.channelMode = (BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_MONO | BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_DUAL_CHANNEL | BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_STEREO | BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_JOINT_STEREO);
	mp3LocalCap.crcProtection = BTHAL_MM_MPEG1_2_AUDIO_CRC_PROTECTION_SUPPORTED;
	mp3LocalCap.layer = (BTHAL_MM_MPEG1_2_AUDIO_LAYER_1 | BTHAL_MM_MPEG1_2_AUDIO_LAYER_2 | BTHAL_MM_MPEG1_2_AUDIO_LAYER_3);
	mp3LocalCap.mpf2 = BTHAL_MM_MPEG1_2_AUDIO_MPF2_SUPPORTED;
	mp3LocalCap.samplingFreq = (BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_48000 | BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_44100 | BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_32000 | BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_24000 | BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_22050 | BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_16000);
	mp3LocalCap.vbr = BTHAL_MM_MPEG1_2_AUDIO_VBR_SUPPORTED;

}
 BtStatus mfw_btips_a2dpConnectStream(BD_ADDR bdAddr)
{
	BtStatus status;
	
	TRACE_FUNCTION("mfw_btips_a2dpConnectStream");

	if ( (strcmp(szSelectedFileExt, "mp3") == 0))
	{
		bPlayMp3File = TRUE;
		bPlaySbcFile = FALSE;
         	codec = BTL_A2DP_CODEC_MPEG1_2_AUDIO;
		status = BTL_A2DP_ConnectStream(btlA2dpContext,&bdAddr, codec, &streamId);
		if (status == BT_STATUS_PENDING)
		{
			TRACE_FUNCTION("Connecting Mp3 stream (opening signaling channel in process)...");
		}
		else
		{
			TRACE_FUNCTION_P1("connecting Mp3 stream failed, status: %s", pBT_Status(status));
		}
	}
	else if ( (strcmp(szSelectedFileExt, "pcm") == 0))
	{
		bPlaySbcFile = TRUE;
		bPlayMp3File = FALSE;
		codec = BTL_A2DP_CODEC_SBC;
		status = BTL_A2DP_ConnectStream(btlA2dpContext,&bdAddr, codec, &streamId);
		if (status == BT_STATUS_PENDING)
		{
			TRACE_FUNCTION("Connecting SBC stream (opening signaling channel in process)...");
		}
		else
		{
			TRACE_FUNCTION_P1("connecting SBC stream failed, status: %s", pBT_Status(status));
		}
		
	}
	else if ( (strcmp(szSelectedFileExt, "wav") == 0))
	{
		bPlaySbcFile = TRUE;
		bPlayMp3File = FALSE;
		codec = BTL_A2DP_CODEC_SBC;
		status = BTL_A2DP_ConnectStream(btlA2dpContext,&bdAddr, codec, &streamId);
		if (status == BT_STATUS_PENDING)
		{
			TRACE_FUNCTION("Connecting SBC stream (opening signaling channel in process)...");
		}
		else
		{
			TRACE_FUNCTION_P1("connecting SBC stream failed, status: %s", pBT_Status(status));
		}
		
	}
	return status;
}
 BtStatus mfw_btips_a2dpCloseStream()
{
	BtStatus status;

	TRACE_FUNCTION("mfw_btips_a2dpCloseStream");

	if ( (strcmp(szSelectedFileExt, "mp3") == 0))
	{
         	//AppMp3StopFile();
         	AppStopAudioFile();
		AppMp3CloseFile();
	}
	else if ( (strcmp(szSelectedFileExt, "pcm") == 0))
	{
		
		AppPcmSimStopFile();
		AppPcmSimCloseFile();
	}
	else if ( (strcmp(szSelectedFileExt, "wav") == 0))
	{
		
		AppPcmSimStopFile();
		AppPcmSimCloseFile();
	}
	status = BTL_A2DP_CloseStream(btlA2dpContext, streamId);
	TRACE_FUNCTION_P1("BTL_A2DP_CloseStream returned %s", pBT_Status(status));
	return status;
}

 BOOL mfw_btips_a2dpGetConnectedDevice (BD_ADDR *bdAddr, U8 *name)
{
	BtStatus status;
	
	status = BTL_A2DP_GetConnectedDevice (btlA2dpContext, streamId, bdAddr);
	
	if (status == BT_STATUS_SUCCESS)
	{
		mfw_btips_bmgGetDeviceName (bdAddr, name);
		return TRUE;
	}
	return FALSE;
}

BOOL mfw_btips_a2dpDisconnect (BD_ADDR bdAddr)
{
	BtStatus btStatus;
	
	btStatus = BTL_A2DP_CloseStream (btlA2dpContext, streamId);

	return TRUE;
}

#endif
