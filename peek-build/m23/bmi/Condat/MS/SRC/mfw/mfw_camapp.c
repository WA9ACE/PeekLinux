/*
+--------------------------------------------------------------------+
| PROJECT:	MMI-Framework (8417)		$Workfile::	mfw_camapp.c	    $|
| $Author::	NDH							$Revision::	1			    $|
| CREATED:	21.05.04		     		$Modtime::	21.05.04 14:58	$|
| STATE  :	code														 |
+--------------------------------------------------------------------+

   MODULE  : MFW_CAMAPP

   PURPOSE : This module contains Camera Interface functions.

   HISTORY:

	 Oct 23 2007. DRT:OMAPS00149238. x0083025 (adrian)
	 Description: The quality is same for the 3 modes (economy, Standard, High)
	 Solution: Data type sent to MM from BMI is different. Changed data type to match.

     May 24 2007 DR:OMAPS00133032 x0061088(Prachi)
     Description:Snapshot with border frame picture captured with tiny square 
     frame on the left top corner.

   
    April 03 2007 ER:OMAPS00116772 x0061088(Prachi)
    Description:Support of 1.3 Mpixel camera on Locosto Plus
    
   Mar 09 2007 DRT :OMAPS00118881 x0061088(Prachi)
   Description :Camera Application Not Attached doesn't describe the real situation
   Solution : mfw_msl_cam_create() is now checking for error "MSL_IMGCAP_ERROR_NOMEMORY"
   returning from msl_cam_create().If msl_cam_create() returns the error
   "MSL_IMGCAP_ERROR_NOMEMORY" then the MMI shows a error  message "Not Enough Memory" and
   exits from the camera application.


	xrashmic 11 Aug 2006, OMAPS00089912 
	Based on framebuffer selection, the color format for snapshot would be RGB565 or YUYV 
	Viewfinder frame buffer width and height set to QCIF width and QCIF height respectively
	
	xrashmic 02 Aug 2006, OMAPS00087612
  	Hanlding of MSL errors for the snapshot save failed scenario

    May  17 2006, xdeepadh - LOCOSTO-DRT-OMAPS00077374
    Description:Saving Images in NAND flash during viewfinder->snapshot.
    Solution: Based on the compile time flag FF_MMI_RFS_ENABLED, the 
                  appropriate file system APIS will be called.
                  
    May  02 2006, xdeepadh - LOCOSTO-DRT-OMAPS00076437
    Description:	MT call not handled in camera Application
    Solution: The asynch event handling in Camera Application has been handled.

    	Jan  10 2006, xdeepadh - LOCOSTO-ENH-
 	Description:	Application to test camera
 	Solution:	Implemented the camera application with following functionalities
 			preview, snapshot ,configure and image saving.

*/




#ifdef FF_MMI_CAMERA_APP



/*
** Include Files
*/

#define ENTITY_MFW
/* includes */
#include <string.h>


#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#include "message.h" // included for aci_cmh.h .... 
#include "prim.h" //included for aci_cmh.h .... 
#include "aci_cmh.h" // included for mfw_sat.h, included for MmiDummy.h, included for MmiMenu.h 

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_camapp.h"
#include "mks/mks_api.h"
//      May  17 2006, xdeepadh - LOCOSTO-DRT-OMAPS00077374
#ifdef FF_MMI_RFS_ENABLED
#include "rfs/rfs_api.h"
#else
#include "ffs/ffs_api.h"
#endif  //FF_MMI_RFS_ENABLED

#include "mfw_ffs.h"

#if (BOARD == 71)
#define ISAMPLE
#endif


#include "dspl.h"
#include "msl/inc/msl_imgcap.h"

/*Variable declarations*/
EXTERN MfwHdr * current_mfw_elem;
MSL_HANDLE imgCapHandle;
void Msl_mslil_Callback(void* handle, U32 ucp_type, U32 tCMd, U32 tStatus);

//Feb  19 2006, x0061088(Prachi)  LOCOSTO-DRT-OMAPS00112518
#ifdef FF_MMI_RELIANCE_FFS
 T_FFS_DIR  close_dir;
#endif

/*******************************************************************************

                                Local prototypes

*******************************************************************************/

BOOL mfw_cam_sign_exec (T_MFW_HDR * cur_elem, T_MFW_EVENT event, T_MFW_CAM_PARA * para);
void mfw_cam_signal(T_MFW_EVENT event, void *para);

/*******************************************************************************

 $Function:		mfw_cam_create

 $Description:		This function  initialises an Mfw Camera entity and adds it to the Window Stack

 $Returns:		T_MFW_HND	: A handle for the entity

 $Arguments:		T_MFW_HND		: Parent Window Handle
 				T_MFW_EVENT	: Event Mask of the events to be handled
 				T_MFW_CB		: Callback function to handle the events

*******************************************************************************/
T_MFW_HND mfw_cam_create(T_MFW_HND hWin, T_MFW_EVENT event, T_MFW_CB cbfunc)
{
	T_MFW_HDR *hdr;
	T_MFW_CAM  *cam_para;

	TRACE_FUNCTION("mfw_cam_create()");

	hdr = (T_MFW_HDR *) mfwAlloc(sizeof (T_MFW_HDR));
	cam_para = (T_MFW_CAM *) mfwAlloc(sizeof (T_MFW_CAM));

	if (!hdr OR !cam_para)
		return FALSE;

	/*
	* initialisation of the handler
	*/
	cam_para->emask   = event;
	cam_para->handler = cbfunc;

	hdr->data = cam_para;		
	hdr->type = MfwTypCt;		

	/*
	* installation of the handler
	*/
	return mfwInsert((T_MFW_HDR *)hWin, hdr);
}

/*******************************************************************************

 $Function:		mfw_cam_delete

 $Description:		This function clears down an Mfw Camera entity and removes it from the
 				Window Stack

 $Returns:		T_MFW_RES	: The result of the function

 $Arguments:		T_MFW_HND	: The Handle of the entity to be removed

*******************************************************************************/
T_MFW_RES mfw_cam_delete(T_MFW_HND hnd)
{
	TRACE_FUNCTION("mfw_cam_delete()");

	if (!hnd OR !((T_MFW_HDR *)hnd)->data)
		return MFW_RES_ILL_HND;

	if (!mfwRemove((T_MFW_HDR *)hnd))
		return MFW_RES_ILL_HND;

	mfwFree((U8 *)(((T_MFW_HDR *) hnd)->data),sizeof(T_MFW_CAM));
	mfwFree((U8 *)hnd,sizeof(T_MFW_HDR));

	return MFW_RES_OK;
}

/*******************************************************************************

 $Function:		mfw_cam_sign_exec

 $Description:		This function sends the Mfw Camera test events from the Mfw to the BMI.

 $Returns:		None

 $Arguments:		None

*******************************************************************************/
BOOL mfw_cam_sign_exec (T_MFW_HDR * cur_elem, T_MFW_EVENT event, T_MFW_CAM_PARA * para)
{
	TRACE_FUNCTION("mfw_cam_sign_exec()");
	TRACE_EVENT_P2("Event %d, mfw_state %d",event,para->state);

	while (cur_elem)
	{
		/*
		* event handler is available
		*/
		if (cur_elem->type EQ MfwTypCt)
		{
			T_MFW_CAM * cam_data;
			/*
			* handler is CM management handler
			*/
			cam_data = (T_MFW_CAM *)cur_elem->data;
			if (cam_data->emask & event)
			{
				/*
				* event is expected by the call back function
				*/
				cam_data->event = event;
				switch (event)
				{
					/*
					** Generic Events
					*/
					case E_CAM_INIT:
						if(para!=NULL)
							memcpy (&cam_data->para, para, sizeof (T_MFW_CAM_PARA));
						break;
					case E_CAM_VIEWFINDER:
						if(para!=NULL)
							memcpy (&cam_data->para, para, sizeof (T_MFW_CAM_PARA));
						break;
					case E_CAM_SNAPSHOT:
						if(para!=NULL)
							memcpy (&cam_data->para, para, sizeof (T_MFW_CAM_PARA));
						break;
					/* April 03 2007 ER:OMAPS00116772 x0061088(Prachi) */	
    				case E_CAM_PREVIEW:
						if(para!=NULL)
							memcpy (&cam_data->para, para, sizeof (T_MFW_CAM_PARA));
						break;						
					case E_CAM_PAUSE:
						if(para!=NULL)
							memcpy (&cam_data->para, para, sizeof (T_MFW_CAM_PARA));
						break;
					case E_CAM_DEINIT:
						if(para!=NULL)
							memcpy (&cam_data->para, para, sizeof (T_MFW_CAM_PARA));
						break;
				}

				/*
				* if call back defined, call it
				*/
				if (cam_data->handler)
				{
					// store current mfw elem
					current_mfw_elem = cur_elem;

					if ((*(cam_data->handler)) (cam_data->event, (void *)&cam_data->para))
						return TRUE;
				}
			}
		}
		cur_elem = cur_elem->next;
	}
	return FALSE;
}

/*******************************************************************************

 $Function:		mfw_cam_signal

 $Description:		This function sends the Mfw Bluetooth events from the Mfw to the BMI.

 $Returns:		None

 $Arguments:		T_MFW_EVENT	: The event to be sent to the BMI
 				void *			: Pointer to the Event data

*******************************************************************************/
void mfw_cam_signal(T_MFW_EVENT event, void *para)
{
	UBYTE temp;
	temp = dspl_Enable(0);

	TRACE_FUNCTION("mfw_cam_signal()");

	if (mfwSignallingMethod EQ 0)
	{
		/*
		* focus is on a window
		*/
		if (mfwFocus)
		{
			/*
			* send event to sim management
			* handler if available
			*/
			if (mfw_cam_sign_exec (mfwFocus, event, para))
			{
				dspl_Enable(temp);
				return;
			}
		}

		/*
		* actual focussed window is not available
		* or has no network management registration
		* handler, then search all nodes from the root.
		*/
		if (mfwRoot)
			mfw_cam_sign_exec (mfwRoot, event, para);
	}
	else
	{
		MfwHdr * h = 0;

		/*
		* Focus set, then start here
		*/
		if (mfwFocus)
			h = mfwFocus;

		/*
		* Focus not set, then start root
		*/
		if (!h)
			h = mfwRoot;

		/*
		* No elements available, return
		*/
		while (h)
		{
			/*
			* Signal consumed, then return
			*/
			if (mfw_cam_sign_exec (h, event, para))
			{
				dspl_Enable(temp);
				return;
			}

			/*
			* All windows tried inclusive root
			*/
			if (h == mfwRoot)
			{
				dspl_Enable(temp);
				return;
			}

			/*
			* get parent window
			*/
			h = mfwParent(mfwParent(h));

			if (h)
				h = ((MfwWin * )(h->data))->elems;
		}
		
		mfw_cam_sign_exec (mfwRoot, event, para);
		
	}
	
	dspl_Enable(temp);

	return;
}

/*******************************************************************************

 $Function:		mfw_cam_setDefaultConfigurations

 $Description:		This function sets the ImgCap ucp structure to default values

 $Returns:		None

 $Arguments:		None

*******************************************************************************/
T_MFW_CAM_ERROR mfw_cam_setDefaultConfigurations()
{
	MSL_IMGCAP_STATUS status;


	

	MSL_CAM_CONFIGTYPE mfw_cam_config;
	MSL_DISPLAY_CONFIGTYPE mfw_cam_dspl_config;

        TRACE_FUNCTION("mfw_msl_cam_setDefaultConfig()");

       status = MSL_ImgCap_SetConfig(imgCapHandle,MSL_CALLBACKSET_CONFIGINDEX,(MSL_CALLBACK)Msl_mslil_Callback);

	mfw_cam_config.tImgFormat = MSL_COLOR_RGB565;
	mfw_cam_config.tMode = MSL_CAMERAMODE_VF;
	mfw_cam_config.unImgHeight = QCIF_HEIGHT ;  //xrashmic 11 Aug 2006, OMAPS00089912 
	mfw_cam_config.unImgWidth  = QCIF_WIDTH;

	if(status == MSL_IMGCAP_STATUS_OK)
	    status =  MSL_ImgCap_SetConfig(imgCapHandle,MSL_CAMERA_CONFIGINDEX,&mfw_cam_config);


	mfw_cam_dspl_config.tImgFormat = MSL_COLOR_RGB565;
	         
       mfw_cam_dspl_config.unDisplayImgHeight = QCIF_HEIGHT;//xrashmic 11 Aug 2006, OMAPS00089912
	mfw_cam_dspl_config.unDisplayImgWidth = QCIF_WIDTH;
	mfw_cam_dspl_config.unDisplayXOffset = 0;
	mfw_cam_dspl_config.unDisplayYOffset = 0;
	if(status == MSL_IMGCAP_STATUS_OK)
		status = MSL_ImgCap_SetConfig(imgCapHandle,MSL_DISPLAY_CONFIGINDEX,&mfw_cam_dspl_config);
 /* April 03 2007 ER:OMAPS00116772 x0061088(Prachi) */ 

    status = MSL_ImgCap_SetConfig(imgCapHandle, MSL_SAVESS_CONFIGINDEX,&FFS_flashData.cam_auto_save);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in configuring");
			return MFW_CAM_ERROR;
	}

	return MFW_CAM_NO_ERROR;

  
}


/*******************************************************************************

 $Function:		mfw_cam_setSnapshotConfigurations

 $Description:		This function configures the msl with default values

 $Returns:		None

 $Arguments:		None
xrashmic 11 Aug 2006, OMAPS00089912 : Added a parameter boolean 'frame'
*******************************************************************************/
T_MFW_CAM_ERROR mfw_cam_setSnapshotConfigurations(BOOL frame)
{
	MSL_IMGCAP_STATUS status;

	char  filename[CAM_FILENAME_EDITOR_INPUT_LEN];
	
	MSL_CAM_CONFIGTYPE mfw_cam_config;
	MSL_DISPLAY_CONFIGTYPE mfw_cam_dspl_config;
	MSL_FILE_CONFIGTYPE mfw_cam_file_config ;

        TRACE_FUNCTION("mfw_cam_setSnapshotConfigurations()");

       status = MSL_ImgCap_SetConfig(imgCapHandle,MSL_CALLBACKSET_CONFIGINDEX,(MSL_CALLBACK)Msl_mslil_Callback);
        //xrashmic 11 Aug 2006, OMAPS00089912 
        //For optimal performance in the camera application Color format for camera capture 
        //in the case of snapshot should be YUYV for no frame and RGB565 only if overlay option is selected
        if(frame)
		mfw_cam_config.tImgFormat = MSL_COLOR_RGB565;
	else
		mfw_cam_config.tImgFormat = MSL_COLOR_YUYV;
	mfw_cam_config.tMode = MSL_CAMERAMODE_SS;
//added by prachi
#if CAM_SENSOR == 1
    if (FFS_flashData.cam_changed_resolution ==  TRUE )
  	{
 	switch(FFS_flashData.cam_resolution)
	{
		case CAM_RESOLUTION_VGA:
			mfw_cam_config.unImgHeight = VGA_HEIGHT ;  
			mfw_cam_config.unImgWidth  = VGA_WIDTH;
			break;
		case CAM_RESOLUTION_QCIF:
            mfw_cam_config.unImgHeight = QCIF_HEIGHT ;  
	        mfw_cam_config.unImgWidth  = QCIF_WIDTH;
			break;
		case CAM_RESOLUTION_SXGA:
            mfw_cam_config.unImgHeight = SXGA_HEIGHT ; 
	        mfw_cam_config.unImgWidth  = SXGA_WIDTH;
			break;
	}
		
  	}
  else 
  	{
  	/* May 24 2007 DR:OMAPS00133032 x0061088(Prachi)*/
    mfw_cam_config.unImgHeight = VGA_HEIGHT ; 
	mfw_cam_config.unImgWidth  = VGA_WIDTH;
  	}
#else 
    mfw_cam_config.unImgHeight =VGA_HEIGHT;
	mfw_cam_config.unImgWidth =VGA_WIDTH;
#endif  
	
	if(status == MSL_IMGCAP_STATUS_OK)
	    status =  MSL_ImgCap_SetConfig(imgCapHandle,MSL_CAMERA_CONFIGINDEX,&mfw_cam_config);

	mfw_cam_dspl_config.tImgFormat = MSL_COLOR_RGB565;
        
	mfw_cam_dspl_config.unDisplayImgHeight = QCIF_HEIGHT; //xrashmic 11 Aug 2006, OMAPS00089912 
	mfw_cam_dspl_config.unDisplayImgWidth = QCIF_WIDTH;
	mfw_cam_dspl_config.unDisplayXOffset = 0;
	mfw_cam_dspl_config.unDisplayYOffset = 0;
	if(status == MSL_IMGCAP_STATUS_OK)
		status = MSL_ImgCap_SetConfig(imgCapHandle,MSL_DISPLAY_CONFIGINDEX,&mfw_cam_dspl_config);

        mfw_cam_file_config.sFileName= (char *) mfwAlloc(CAM_FILENAME_EDITOR_INPUT_LEN);

	switch(FFS_flashData.snap_storage)
	{
		case SNAP_STG_FFS:
		       sprintf(filename,"%s%s/%s%d","/FFS/mmi",JPEG_CONTENT_DIR,(char *)mmi_get_Filename(),(int)FFS_flashData.camera_file_counter);			
			break;
		case SNAP_STG_NORMS:
		       sprintf(filename,"%s%s/%s%d","/NOR/",JPEG_CONTENT_DIR,(char *)mmi_get_Filename(),(int)FFS_flashData.camera_file_counter);			
			break;
		case SNAP_STG_NAND:
			sprintf(filename,"%s%s/%s%d","/NAND",JPEG_CONTENT_DIR,(char *)mmi_get_Filename(),(int)FFS_flashData.camera_file_counter);			
			break;
		case SNAP_STG_MMC:
			sprintf(filename,"%s%s/%s%d","/MMC",JPEG_CONTENT_DIR,(char *)mmi_get_Filename(),(int)FFS_flashData.camera_file_counter);			
			break;
	}
	
        TRACE_EVENT_P1("filename ************is %s",filename);
        strcpy(mfw_cam_file_config.sFileName, filename);
		
//      May  17 2006, xdeepadh - LOCOSTO-DRT-OMAPS00077374        
//Set the file type
//#ifdef FF_MMI_RFS_ENABLED        
//        mfw_cam_file_config.tFileType = MSL_FILETYPE_RFS;
#ifdef FF_MMI_FILEMANAGER
        mfw_cam_file_config.tFileType = MSL_FILETYPE_RFS;
#else
        mfw_cam_file_config.tFileType = MSL_FILETYPE_FFS;
#endif
        if(status == MSL_IMGCAP_STATUS_OK)
             status =  MSL_ImgCap_SetConfig(imgCapHandle,MSL_ENCFILE_CONFIGINDEX,&mfw_cam_file_config);
        mfwFree((U8 *)mfw_cam_file_config.sFileName,CAM_FILENAME_EDITOR_INPUT_LEN);

	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in configuring");
			return MFW_CAM_ERROR;
	}

	return MFW_CAM_NO_ERROR;

  
}
 
 
 
/*******************************************************************************

 $Function:		mfw_cam_setQualityConfig

 $Description:		This function is used to update the MSL of the new quliaty factor in ImgCap ucp 
 
 $Returns:		None

 $Arguments:		None
 
*******************************************************************************/
// Oct 23 2007. DRT:OMAPS00149238. x0083025 (adrian)
T_MFW_CAM_ERROR mfw_msl_cam_setQualityConfig(U32 qFactor)
{
	MSL_IMGCAP_STATUS status;
   	TRACE_FUNCTION("mfw_msl_cam_setQualityConfig()");

	status=MSL_ImgCap_SetConfig(imgCapHandle,MSL_ENCQUALITY_CONFIGINDEX,&qFactor);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in updating quality config");
			return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
}


/*******************************************************************************

 $Function:		mfw_cam_setRotateConfig

 $Description:		This function is used to update the MSL of the new rotate in ImgCap ucp 
 
 $Returns:		None

 $Arguments:		None

*******************************************************************************/
T_MFW_CAM_ERROR mfw_msl_cam_setRotateConfig(int tRotate)
{
	MSL_IMGCAP_STATUS status;
   	TRACE_FUNCTION("mfw_msl_cam_setRotateConfig()");

	status=MSL_ImgCap_SetConfig(imgCapHandle,MSL_ROTATE_CONFIGINDEX,&tRotate);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in updating the rotate config");
			return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
}

/*******************************************************************************

 $Function:		mfw_cam_setBrustModeConfig

 $Description:		This function is used to update the MSL of the Burst mode in ImgCap ucp 
 
 $Returns:		None

 $Arguments:		None

*******************************************************************************/
T_MFW_CAM_ERROR mfw_msl_cam_setBurstModeConfig(UBYTE burstCount)
{
	MSL_IMGCAP_STATUS status;
	TRACE_FUNCTION("mfw_msl_cam_setBurstModeConfig()");

	status=MSL_ImgCap_SetConfig(imgCapHandle, MSL_BURSTCOUNT_CONFIGINDEX,&burstCount);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
		TRACE_ERROR("Error in updating Burstmode config");
		return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
}

/*******************************************************************************

 $Function:		mfw_cam_setfileNameConfig

 $Description:		This function is used to update the MSL of the filename of the saved image
 				in ImgCap ucp 
 
 $Returns:		None

 $Arguments:		None

*******************************************************************************/
T_MFW_CAM_ERROR mfw_msl_cam_setFileNameConfig()
{
	MSL_IMGCAP_STATUS status;
	MSL_FILE_CONFIGTYPE mfw_cam_file_config ;
	char  filename[CAM_FILENAME_EDITOR_INPUT_LEN];

   	TRACE_FUNCTION("mfw_msl_cam_setFileNameConfig()");

        mfw_cam_file_config.sFileName= (char *) mfwAlloc(CAM_FILENAME_EDITOR_INPUT_LEN);

	switch(FFS_flashData.snap_storage)
	{
		case SNAP_STG_FFS:
		       sprintf(filename,"%s%s/%s%d","/FFS/mmi",JPEG_CONTENT_DIR,(char *)mmi_get_Filename(),(int)FFS_flashData.camera_file_counter);			
			break;
		case SNAP_STG_NORMS:
		       sprintf(filename,"%s%s/%s%d","/NOR/mmi",JPEG_CONTENT_DIR,(char *)mmi_get_Filename(),(int)FFS_flashData.camera_file_counter);			
			break;
		case SNAP_STG_NAND:
			sprintf(filename,"%s%s/%s%d","/NAND",JPEG_CONTENT_DIR,(char *)mmi_get_Filename(),(int)FFS_flashData.camera_file_counter);			
			break;
		case SNAP_STG_MMC:
			sprintf(filename,"%s%s/%s%d","/MMC",JPEG_CONTENT_DIR,(char *)mmi_get_Filename(),(int)FFS_flashData.camera_file_counter);			
			break;
	}
	
        TRACE_EVENT_P1("filename ************is %s",filename);
        strcpy(mfw_cam_file_config.sFileName, filename);
//      May  17 2006, xdeepadh - LOCOSTO-DRT-OMAPS00077374        
// Set the file type
//#ifdef FF_MMI_RFS_ENABLED        
#ifdef FF_MMI_FILEMANAGER   
        mfw_cam_file_config.tFileType = MSL_FILETYPE_RFS;
#else
        mfw_cam_file_config.tFileType = MSL_FILETYPE_FFS;
#endif
        /*if(status != MSL_IMGCAP_STATUS_OK)*//*a0393213 warnings removal-variable "status" is used before its value is set*/
        status =  MSL_ImgCap_SetConfig(imgCapHandle,MSL_ENCFILE_CONFIGINDEX,&mfw_cam_file_config);
        mfwFree((U8 *)mfw_cam_file_config.sFileName,CAM_FILENAME_EDITOR_INPUT_LEN);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in updating filename");
			return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
}


/*******************************************************************************

 $Function:		mfw_cam_set_overlayConfig

 $Description:		This function is used to update the MSL of the Border frames for both 
 				Viewfinder and Snapshot
 
 $Returns:		None

 $Arguments:		None

*******************************************************************************/
T_MFW_CAM_ERROR mfw_msl_cam_setOverlayConfig(MSL_U8 * overlayBuffer)
{
	MSL_IMGCAP_STATUS status;
	MSL_IMG_OVERLAYCONFIG mfw_cam_overlayconfig ;
   	TRACE_FUNCTION("mfw_msl_cam_setOverlayConfig()");

	mfw_cam_overlayconfig.nAlpha = 0;
	mfw_cam_overlayconfig.nImgHeight = QCIF_HEIGHT;        //xrashmic 11 Aug 2006, OMAPS00089912 
	mfw_cam_overlayconfig.nImgWidth = QCIF_WIDTH;
	mfw_cam_overlayconfig.nOverlayXOffset =  0;
	mfw_cam_overlayconfig.nOverlayYOffset =  0;
	mfw_cam_overlayconfig.nTransparencyColor = 0x0020 ;
	mfw_cam_overlayconfig.pOverlayBuff = overlayBuffer;
	mfw_cam_overlayconfig.tOverlayMode =  MSL_OVERLAYMODE_COLORKEY;
	status =  MSL_ImgCap_SetConfig(imgCapHandle,MSL_OVERLAY_CONFIGINDEX,&mfw_cam_overlayconfig);
	    
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in pdating viewfinder config");
			return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
}

/*******************************************************************************

 $Function:		mfw_cam_set_SSOverlayConfig

 $Description:		This function is used to update the MSL of the Border frames for both 
 				Viewfinder and Snapshot
 
 $Returns:		None

 $Arguments:		None

*******************************************************************************/
T_MFW_CAM_ERROR mfw_cam_set_SSOverlayConfig(MSL_U8 * overlayBuffer)
{
	MSL_IMGCAP_STATUS status;
	MSL_IMG_OVERLAYCONFIG mfw_cam_overlayconfig ;
   	TRACE_FUNCTION("mfw_cam_set_SSOverlayConfig()");

	mfw_cam_overlayconfig.nAlpha = 0;
	#if CAM_SENSOR == 1
	if( FFS_flashData.cam_changed_resolution == TRUE )
	{
      switch(FFS_flashData.cam_resolution)
      	{
      	case CAM_RESOLUTION_VGA:
          mfw_cam_overlayconfig.nImgHeight = VGA_HEIGHT;
	      mfw_cam_overlayconfig.nImgWidth = VGA_WIDTH;
          break;
        case CAM_RESOLUTION_QCIF:
          mfw_cam_overlayconfig.nImgHeight = QCIF_HEIGHT;
	      mfw_cam_overlayconfig.nImgWidth = QCIF_WIDTH;
          break;
      	}
	} 
    else
    {
       mfw_cam_overlayconfig.nImgHeight = VGA_HEIGHT;
	   mfw_cam_overlayconfig.nImgWidth = VGA_WIDTH;
    }
	#else
	mfw_cam_overlayconfig.nImgHeight = VGA_HEIGHT;
	mfw_cam_overlayconfig.nImgWidth = VGA_WIDTH;
	#endif
	mfw_cam_overlayconfig.nOverlayXOffset =  0;
	mfw_cam_overlayconfig.nOverlayYOffset =  0;
	mfw_cam_overlayconfig.nTransparencyColor = 0x0020 ;
	mfw_cam_overlayconfig.pOverlayBuff = overlayBuffer;
	mfw_cam_overlayconfig.tOverlayMode =  MSL_OVERLAYMODE_COLORKEY;
	status =  MSL_ImgCap_SetConfig(imgCapHandle,MSL_OVERLAY_CONFIGINDEX,&mfw_cam_overlayconfig);
	    
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in pdating viewfinder config");
			return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
}


/*******************************************************************************

 $Function:		mfw_cam_set_zoomConfig

 $Description:		This function is used to update the MSL of the Border frames for both 
 				Viewfinder and Snapshot
 
 $Returns:		None

 $Arguments:		None

*******************************************************************************/
T_MFW_CAM_ERROR mfw_msl_cam_setZoomConfig(U32  zFactor)
{
	MSL_IMGCAP_STATUS status;
	U32 zoomFactor = zFactor ;
   	TRACE_FUNCTION("mfw_msl_cam_setZoomConfig()");

	status=MSL_ImgCap_SetConfig(imgCapHandle,MSL_ZOOM_CONFIGINDEX,&zoomFactor);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in updating zoom config");
			return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
}

/*******************************************************************************

 $Function:		mfw_msl_cam_cb

 $Description:		This is the callback funtion for the imgCap UCP
 
 $Returns:		None

 $Arguments:		tCmd - specifies the cmd for which the callback is received
 				tStatus - Status message
 
*******************************************************************************/
void mfw_msl_cam_cb( U32 tCMd,U32 tStatus)
{
	T_MFW_CAM_PARA para;

   	TRACE_FUNCTION("mfw_msl_cam_cb()");

	switch(tCMd)
	{
		case MSL_CMD_INIT:
			if(tStatus==MSL_IMGCAP_STATUS_OK)
		   	{
		   		TRACE_EVENT("Init Done");
				para.state=CAM_STATE_INIT_DONE;
			}
			else	
			{
		   		TRACE_EVENT("Init Error");
				para.state=CAM_STATE_ERROR;
			}
			mfw_cam_signal(E_CAM_INIT,&para);
			break;
		case MSL_CMD_VIEWFINDER:
			if(tStatus==MSL_IMGCAP_STATUS_OK)
		   	{
		   		TRACE_EVENT("Viewfinder done");
				para.state=CAM_STATE_VIEWFINDER_DONE;
			}
			else	
			{
		   		TRACE_EVENT("Viewfinder Error");
				para.state=CAM_STATE_ERROR;
			}
			mfw_cam_signal(E_CAM_VIEWFINDER,&para);
			break;
		case MSL_CMD_SNAPSHOT:
			if(tStatus==MSL_IMGCAP_STATUS_OK)
		   	{
		   		TRACE_EVENT("Snapshot done");
				para.state=CAM_STATE_SNAPSHOT_DONE;
			}
			//xrashmic 02 Aug 2006, OMAPS00087612
			//Saving of snapshot to the filesystem has failed.
			else if(tStatus==MSL_IMGCAP_ERROR_IOWRITE)
			{
				TRACE_EVENT("Snapshot not saved");
				para.state=CAM_STATE_SNAPSHOT_SAVE_ERROR;
			}
			else if(tStatus==MSL_IMGCAP_ERROR_NOSPACE)
			{
				TRACE_EVENT("No Space in File System");
				para.state=CAM_STATE_SNAPSHOT_NOSPACE_ERROR;
			}	
			else	
		   	{
		   		TRACE_EVENT("Snapshot error");
				para.state=CAM_STATE_ERROR;
			}
			mfw_cam_signal(E_CAM_SNAPSHOT,&para);
			break;
		case MSL_CMD_PAUSE:
			if(tStatus==MSL_IMGCAP_STATUS_OK)
		   	{
		   		TRACE_EVENT("Pause Done");
				para.state=CAM_STATE_PAUSE_DONE;
			}
			else	
		   	{
		   		TRACE_EVENT("Pause error");
				para.state=CAM_STATE_ERROR;
			}
			mfw_cam_signal(E_CAM_PAUSE,&para);
			break;
		case MSL_CMD_DEINIT	:
			if(tStatus==MSL_IMGCAP_STATUS_OK)
		   	{
		   		TRACE_EVENT("Deint done");
				para.state=CAM_STATE_DEINIT_DONE;
			}
			else	
		   	{
		   		TRACE_EVENT("Pause error");
				para.state=CAM_STATE_ERROR;
			}
			mfw_cam_signal(E_CAM_DEINIT,&para);
			break;
        /* April 03 2007 ER:OMAPS00116772 x0061088(Prachi) */
        case MSL_CMD_PREVIEWDONE :
            if(tStatus == MSL_IMGCAP_STATUS_OK) 
			{
				 TRACE_EVENT("Preview Done");
			 	 para.state = CAM_STATE_PREVIEW_DONE;
            } 	
			else
			{
				TRACE_EVENT("Preview Error");
				para.state = CAM_STATE_ERROR;
			}
			mfw_cam_signal(E_CAM_PREVIEW,&para);
			break;
	}
}

/*******************************************************************************

 $Function:	mfw_cam_creates

 $Description:	This function creates the JPEG_CONTENT_DIR and creates the camera engine. 

 $Returns:	None

 $Arguments:	None

*******************************************************************************/
T_MFW_CAM_ERROR mfw_msl_cam_create(void)
{
	MSL_IMGCAP_STATUS status;

//May  17 2006, xdeepadh - LOCOSTO-DRT-OMAPS00077374
#ifdef FF_MMI_RFS_ENABLED
	T_RFS_DIR  dir;
 	T_RFS_RET rfsResult;
	
	TRACE_EVENT("mfw_cam_init()");
   	 rfsResult = rfs_opendir(JPEG_CONTENT_DIR,&dir);           
	 
        if(RFS_ENOENT == rfsResult)
        {
        	//Directory does not exsists, create it and initialize the table and write it to FFS
		/*rfsResult = rfs_mkdir(JPEG_CONTENT_DIR,RFS_IRWXU);
		if(RFS_EOK == rfsResult)
		{
				TRACE_EVENT("open Dir Passed"); 
    		}
    		else
       			TRACE_EVENT("open Dir Failed");    */
        }
#else       
	T_FFS_DIR  dir;
 	T_FFS_RET ffsResult;
	
	TRACE_FUNCTION("mfw_cam_init()");

	ffsResult = ffs_opendir(JPEG_CONTENT_DIR,&dir);       
//Feb  19 2006, x0061088(Prachi)LOCOSTO-DRT-OMAPS00112518
#ifdef FF_MMI_RELIANCE_FFS
    close_dir = dir;
#endif 

		
        if(EFFS_NOTFOUND == ffsResult)
    	{
    		//Directory does not exsists, create it and initialize the table and write it to FFS
		ffsResult = ffs_mkdir(JPEG_CONTENT_DIR);
		if(EFFS_OK == ffsResult)
		{
			TRACE_EVENT("Create Dir Passed"); 
		}
			else
   			 TRACE_EVENT("Create Dir Failed");    
    	} 
	else 
	{
		TRACE_EVENT_P1("No directory created %d",ffsResult);
	}
#endif //FF_MMI_RFS_ENABLED
 
	status = MSL_ImgCap_Create (&imgCapHandle);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
	   //Mar 09 2007 DRT :OMAPS00118881 x0061088(Prachi)
	    TRACE_ERROR("Error creating handler");
        if(status == MSL_IMGCAP_ERROR_NOMEMORY)
        {
            TRACE_ERROR("No memory Error");
			return MFW_CAM_ERROR_NO_MEMORY;
        }
        else
       	{
       	  TRACE_ERROR("Error creating handler");
		  return MFW_CAM_ERROR;
        }  
	}

	return MFW_CAM_NO_ERROR;
}
/*******************************************************************************

 $Function:	mfw_cam_init

 $Description:	Initializes the camera engine. 

 $Returns:	None

 $Arguments:	None

*******************************************************************************/
T_MFW_CAM_ERROR mfw_msl_cam_init(void)
{
	MSL_IMGCAP_STATUS status;

	TRACE_FUNCTION("mfw_msl_cam_init()");
	//Do the default configuration for the camera and image components
	
	status=MSL_ImgCap_Init(imgCapHandle);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in initializing MSL camera");
			return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
}
/*******************************************************************************

 $Function:		mfw_msl_cam_Deinit

 $Description:		Deinitializes the cam

 $Returns:		None

 $Arguments:		None

*******************************************************************************/
T_MFW_CAM_ERROR mfw_msl_cam_deinit(void)
{
	MSL_IMGCAP_STATUS status;
	TRACE_FUNCTION("mfw_msl_cam_Deinit()");
	status = MSL_ImgCap_Deinit(imgCapHandle);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in De-initializing MSL camera");
			return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
}

/*******************************************************************************

 $Function:		mfw_cam_exit

 $Description:		Exits the camera application

 $Returns:		None

 $Arguments:		None

*******************************************************************************/
T_MFW_CAM_ERROR mfw_msl_cam_exit(void)
{
	MSL_IMGCAP_STATUS status;
	TRACE_FUNCTION("mfw_msl_cam_exit()");

	status=MSL_ImgCap_Destroy(imgCapHandle);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in exiting MSL camera");
			return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
}

/*******************************************************************************

 $Function:     mfw_cam_launch_viewfinder

 $Description:	This function starts the viewfinder mode
 
 $Returns:	None

 $Arguments: 	None

*******************************************************************************/
 T_MFW_CAM_ERROR mfw_msl_cam_viewfinder()
{
 	MSL_IMGCAP_STATUS status;

	TRACE_FUNCTION("mfw_msl_cam_viewfinder()");

	status=MSL_ImgCap_Viewfinder(imgCapHandle);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in launching viewfinder");
			return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
	
}

/*******************************************************************************

 $Function:     mfw_cam_suspend_camera

 $Description: This function pauses the ImgCap UCP

 $Returns:	None

 $Arguments: None

*******************************************************************************/
 T_MFW_CAM_ERROR mfw_msl_cam_suspend_camera()
{
	MSL_IMGCAP_STATUS status;
 	TRACE_FUNCTION("mfw_msl_cam_suspend_camera()");

	status=MSL_ImgCap_Pause(imgCapHandle);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in pausing MSL camera");
			return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
}

/*******************************************************************************

 $Function:     mfw_cam_snapshot

 $Description:	This function takes the snapshot. and also saves the same to FFS 

 $Returns:	None

 $Arguments:	None

*******************************************************************************/
T_MFW_CAM_ERROR mfw_msl_cam_snapshot()
{
	MSL_IMGCAP_STATUS status;
	TRACE_FUNCTION("mfw_msl_cam_snapshot()");   

	status=MSL_ImgCap_Snapshot(imgCapHandle);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in taking the snapshot");
			return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
}



/*******************************************************************************

 $Function:     mfw_close_dir

 $Description:	This function close the directory /mm/jpeg/

 $Returns:	None

 $Arguments:	None

*******************************************************************************/
 //Feb  19 2006, x0061088(Prachi)  LOCOSTO-DRT-OMAPS00112518
#ifdef FF_MMI_RELIANCE_FFS 
void mfw_close_dir()
{
   ffs_closedir(close_dir);
   return;
}
#endif

/* April 03 2007 ER:OMAPS00116772 x0061088(Prachi) */
/*******************************************************************************

 $Function:		mfw_cam_setAutoSave

 $Description:		This function is used to update the MSL for Auto Save 
 
 $Returns:		None

 $Arguments:		None
 
*******************************************************************************/
T_MFW_CAM_ERROR mfw_msl_cam_setAutoSave(UBYTE autoSave)
{
	MSL_IMGCAP_STATUS status;
   	TRACE_FUNCTION("mfw_msl_cam_setAutoSave()");

	status=MSL_ImgCap_SetConfig(imgCapHandle,MSL_SAVESS_CONFIGINDEX,&autoSave);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in while setting for Auto Save");
			return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
}

/* April 03 2007 ER:OMAPS00116772 x0061088(Prachi) */
/*******************************************************************************

 $Function:		mfw_msl_cam_save

 $Description:	 Calls MSL save

 $Returns:		None

 $Arguments:		None

*******************************************************************************/
T_MFW_CAM_ERROR mfw_msl_cam_save(void)
{
	MSL_IMGCAP_STATUS status;
	TRACE_FUNCTION("mfw_msl_cam_Deinit()");
	
	status=(MSL_IMGCAP_STATUS)MSL_ImgCap_SaveEncodedBuffers(imgCapHandle);
	if(status != MSL_IMGCAP_STATUS_OK)
	{
			TRACE_ERROR("Error in De-initializing MSL camera");
			return MFW_CAM_ERROR;
	}
	return MFW_CAM_NO_ERROR;
}


#endif
