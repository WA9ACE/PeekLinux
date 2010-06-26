#ifndef _MMICAMERAAPP_H_
#define _MMICAMERAAPP_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		CameraTest
 $File:		       MmiCameraApp.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Texas Instruments France                                                       
 $Date:		    16/06/04                                                    
                                                                               
********************************************************************************
                                                                              
 Description:

    This module provides the Camera functionality.
                        
********************************************************************************
 $History: MmiCameraApp.h

   April 03 2007 ER:OMAPS00116772 x0061088(Prachi)
  Description:Support of 1.3 Mpixel camera on Locosto Plus

	Jan  10 2006, xdeepadh - LOCOSTO-ENH-
 	Description:	Application to test camera
 	Solution:	Implemented the camera application with following functionalities
 			preview, snapshot ,configure and image saving.
	16/06/04   			
	   
 $End

*******************************************************************************/


/*******************************************************************************
                                                                              
                                Include files
                                                                              
*******************************************************************************/




/*******************************************************************************
                                                                              
                                Interface constants
                                                                              
*******************************************************************************/
typedef struct 
{
	void * buffer;
	U16  width;
	U16 height;
} T_FRAME;

//Maximum border frames to be displayed in the MMI
#define FRAME_MAX_FILES 2
typedef struct 
{
	BOOL quality;
	BOOL rotate;
	BOOL burstmode;
	BOOL filename;
	BOOL frame;
	BOOL zoom;
	/*April 03 2007 ER:OMAPS00116772 x0061088(Prachi)*/
	BOOL AutoSave;
} T_CONFIGURATIONS;

typedef enum {
	CAM_INIT=0,
	CAM_DRAW,
	CAM_SUSPEND,
	CAM_RESUME,
	CAM_DEINT,
	CAM_EXIT,
	CAM_EDITOR_INIT,
	CAM_EDITOR_CANCEL,
	CAM_EDITOR_SELECT
}CAM_WIN_EVENT;
typedef struct
{
	T_MMI_CONTROL	mmi_control;
	T_MFW_HND		win;
	T_MFW_HND		mfwwin;	
	T_MFW_HND		optwin;	
	T_MFW_HND		infowin;	
	T_MFW_HND		parent;
	T_MFW_HND         	kbd;
	MfwIcnAttr     		ct_bitmap;
	SHORT          		id;
	T_MFW_CAM_STATE	cam_current_state;
}	T_MMI_CAM_Win_data;

//editor data structure
typedef struct
{
	T_MMI_CONTROL	mmi_control;
	MfwHnd			win;
	MfwHnd			cam_win_editor;
	UBYTE			cam_buffer[CAM_FILENAME_EDITOR_INPUT_LEN+1];
}	T_CAM_Editor_Data;


//Dialog Screen timeout
#define CAM_INFO_SCRN_TIMEOUT  1000
#define  TRANSP_INDEX 0x0020
//Input length of Editor
/*******************************************************************************
                                                                              
                                Public methods
                                                                              
*******************************************************************************/
GLOBAL int mmi_camera_launch_view_finder (MfwMnu* m, MfwMnuItem* i);
GLOBAL int mmi_camera_set_quality (MfwMnu* m, MfwMnuItem* i);
GLOBAL int mmi_camera_set_storage (MfwMnu* m, MfwMnuItem* i);
GLOBAL int mmi_camera_set_borderframe (MfwMnu* m, MfwMnuItem* i);
GLOBAL int mmi_camera_set_rotation (MfwMnu* m, MfwMnuItem* i);
GLOBAL int mmi_camera_set_burstmode (MfwMnu* m, MfwMnuItem* i);
GLOBAL int mmi_camera_edit_filename(MfwMnu* m, MfwMnuItem* i);
/*April 03 2007 ER:OMAPS00116772 x0061088(Prachi)*/
GLOBAL int mmi_camera_set_auto_save(MfwMnu* m, MfwMnuItem* i);
#if CAM_SENSOR == 1
GLOBAL int mmi_camera_set_resolution (MfwMnu* m, MfwMnuItem* i);
#endif 

/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif //_MMICAMERAAPP_H_

