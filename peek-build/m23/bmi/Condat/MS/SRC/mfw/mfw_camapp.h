/*
+--------------------------------------------------------------------+
| PROJECT:	MMI-Framework (8417)		$Workfile::	mfw_camapp.h	    $|
| $Author::	NDH							$Revision::	1			    $|
| CREATED:	21.05.04		     		$Modtime::	21.05.04	 14:58	$|
| STATE  :	code														 |
+--------------------------------------------------------------------+

   MODULE  : MFW_CAMAPP

   PURPOSE : This module contains the definitions for the Camera Interface.
***********************************************************************************************
  $History: mfw_camapp.h

  Oct 23 2007. DRT:OMAPS00149238. x0083025 (adrian)
  Description: The quality is same for the 3 modes (economy, Standard, High)
  Solution: Data type sent to MM from BMI is different. Changed data type to match.

    April 03 2007 ER:OMAPS00116772 x0061088(Prachi)
    Description:Support of 1.3 Mpixel camera on Locosto Plus

   Mar 15 2007 DRT OMAPS00120112 x0061088(Prachi)
   MFW_CAM_BURST_ON is set equal to 4.

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
************************************************************************************************/
#include "msl/inc/msl_api.h"
typedef enum 
{
	QUALITY_ECONOMY=40,
	QUALITY_STANDARD=55,
	QUALITY_HIGH=65
} T_MFW_CAM_QUALITY;

//Camera Test Return Values
typedef enum 
{
	MFW_CAM_OK							= 0,
	MFW_CAM_FAILED						=- 1
 } T_MFW_CAM_STATUS;

// CAM Mfw -> BMI signal parameter

typedef enum
{
       MFW_CAM_NO_ROTATE,
	MFW_CAM_ROTATE_90, 
	MFW_CAM_ROTATE_180,		
	MFW_CAM_ROTATE_270	
}T_MFW_CAM_ROTATE;

typedef enum
{
MFW_CAM_BURST_OFF=1,
//Mar 15 2007 DRT OMAPS00120112 x0061088(Prachi)	
MFW_CAM_BURST_ON =4	
}T_MFW_CAM_BUSTMODE;

/*Defines for Zoom -------------------------------------*/

#define MFW_CAM_ZOOM_LOW 	 1024
#define MFW_CAM_ZOOM_MAX	 (8 * MFW_CAM_ZOOM_LOW)
#define MFW_CAM_ZOOM_NONE    1024

#define VGA_WIDTH   640
#define VGA_HEIGHT  480
#define QCIF_WIDTH  176
#define QCIF_HEIGHT 144
//add by prachi
#if CAM_SENSOR == 1
#define SXGA_WIDTH  1280
#define SXGA_HEIGHT 1024
#endif

#define CAM_FILENAME_EDITOR_INPUT_LEN 45 

//   May  17 2006, xdeepadh - LOCOSTO-DRT-OMAPS00077374
#ifdef FF_MMI_RFS_ENABLED
#define JPEG_CONTENT_DIR 				"/INANDA1"
#else
#define JPEG_CONTENT_DIR 				"/jpeg"
#endif //FF_MMI_RFS_ENABLED

#define E_CAM_INIT			   (T_MFW_EVENT)0x00000001
#define E_CAM_VIEWFINDER	   (T_MFW_EVENT)0x00000002
#define E_CAM_SNAPSHOT   (T_MFW_EVENT)0x00000004
#define E_CAM_PAUSE             (T_MFW_EVENT)0x00000008
#define E_CAM_DEINIT             (T_MFW_EVENT)0x00000010
/* April 03 2007 ER:OMAPS00116772 x0061088(Prachi) */
#define E_CAM_PREVIEW             (T_MFW_EVENT)0x00000012
#define E_CAM_ALL_SERVICES	   (T_MFW_EVENT)0xFFFFFFFF
// CT Control Block
typedef enum
{
	CAM_STATE_NONE,
	CAM_STATE_INIT,
	CAM_STATE_INIT_DONE,
	CAM_STATE_VIEWFINDER,
	CAM_STATE_VIEWFINDER_DONE,
	CAM_STATE_SNAPSHOT,
	CAM_STATE_SNAPSHOT_DONE,
	CAM_STATE_SNAPSHOT_SAVE_ERROR,//xrashmic 02 Aug 2006, OMAPS00087612
	CAM_STATE_SNAPSHOT_NOSPACE_ERROR, //omaps00151335
	CAM_STATE_PAUSE,
	CAM_STATE_PAUSE_DONE,
	CAM_STATE_DEINIT,
	CAM_STATE_DEINIT_DONE,
	CAM_STATE_ERROR,
/* April 03 2007 ER:OMAPS00116772 x0061088(Prachi) */	
	CAM_STATE_PREVIEW_DONE
}T_MFW_CAM_STATE;

typedef enum
{
	MFW_CAM_NO_ERROR,
	MFW_CAM_ERROR,
	//Mar 09 2007 DRT :OMAPS00118881 x0061088(Prachi)
	MFW_CAM_ERROR_NO_MEMORY
}T_MFW_CAM_ERROR;


typedef enum
{
	CAM_IDLE_MODE,
	CAM_VIEWFINDER_MODE,
	CAM_SNAPSHOT_MODE,
	CAM_CONFIGURE_MODE,
	// May  02 2006, xdeepadh - LOCOSTO-DRT-OMAPS00076437
	CAM_VIEWFINDER_ASYNC_MODE,
	CAM_SNAPSHOT_ASYNC_MODE
}T_MFW_CAM_MODE;

typedef struct
{
	T_MFW_CAM_STATE state;
} 
T_MFW_CAM_PARA;

typedef struct
{
    T_MFW_EVENT      emask;         /* events of interest */
    T_MFW_EVENT      event;          /* current event */
    T_MFW_CB           handler;
    T_MFW_CAM_PARA  para;
} T_MFW_CAM;


/*
** Public function Definitions
*/

T_MFW_HND mfw_cam_create(T_MFW_HND hWin, T_MFW_EVENT event, T_MFW_CB cbfunc);
T_MFW_RES mfw_cam_delete(T_MFW_HND hnd);
T_MFW_CAM_ERROR mfw_msl_cam_create(void);
T_MFW_CAM_ERROR mfw_msl_cam_init(void);
T_MFW_CAM_ERROR mfw_cam_setDefaultConfigurations();
T_MFW_CAM_ERROR mfw_cam_setSnapshotConfigurations(BOOL frame);//xrashmic 11 Aug 2006, OMAPS00089912 

// Oct 23 2007. DRT:OMAPS00149238. x0083025 (adrian)
T_MFW_CAM_ERROR mfw_msl_cam_setQualityConfig(U32 qFactor);
T_MFW_CAM_ERROR mfw_msl_cam_setRotateConfig(int tRotate);
T_MFW_CAM_ERROR mfw_msl_cam_setBurstModeConfig(UBYTE burstCount);
T_MFW_CAM_ERROR mfw_msl_cam_setFileNameConfig();
T_MFW_CAM_ERROR mfw_msl_cam_setOverlayConfig(MSL_U8 * overlayBuffer);
T_MFW_CAM_ERROR mfw_cam_set_SSOverlayConfig(MSL_U8 * overlayBuffer);
T_MFW_CAM_ERROR mfw_msl_cam_setZoomConfig(U32 zoomFactor);
void mfw_msl_cam_cb( U32 tCMd,U32 tStatus);
T_MFW_CAM_ERROR mfw_msl_cam_exit(void);
 T_MFW_CAM_ERROR mfw_msl_cam_viewfinder();
 T_MFW_CAM_ERROR mfw_msl_cam_suspend_camera();
T_MFW_CAM_ERROR mfw_msl_cam_snapshot();
T_MFW_CAM_ERROR mfw_msl_cam_deinit();

/* April 03 2007 ER:OMAPS00116772 x0061088(Prachi) */
T_MFW_CAM_ERROR mfw_msl_cam_setAutoSave(UBYTE autoSave);
T_MFW_CAM_ERROR mfw_msl_cam_save();

//Feb  19 2007, x0061088(Prachi)  LOCOSTO-DRT-OMAPS00112518
#ifdef FF_MMI_RELIANCE_FFS 
void mfw_close_dir();
#endif


