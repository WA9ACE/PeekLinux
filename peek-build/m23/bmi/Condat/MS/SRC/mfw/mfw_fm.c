#ifdef FF_MMI_FILEMANAGER
/* =========================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================== */

/*==========================================================
* @file mfw_fmc     
*
* This provides the functionality of File Manager Applications.
* It supports file/Directory related operations on NOR, NAND and MMC
*
* @path  \bmi\condat\ms\src\mfw
*
* @rev  00.01
*/
/* ========================================================== */
/*===========================================================
*!
*! Revision History
*! ===================================
	
	Nov 12 2007 DRT :OMAPS00144861 x0056422
	Description: MM: There are some issues with resume and stop 
	callback in BT MMI code. 
	

	Aug 27, 2007 REF:DRT OMAPS00137370 x0045876
   	Description: MM: MIDI/IMY file is not played correctly after listening a 
   				 MP3 in the Browse Midi list.
   	Solution: Param Voice_Limit is changed when MP3 or AAC file is played. So before 
   			  playing MP3 or AAC file, Voice_Limit should be saved.
   			  
	 Jul 04, 2007 DRT:OMAPS00135749  x0062174(Nimitha)
	 Description: Once the audio file has finished automatically play button has to be pressed twice,
    	 in order to be played again.
    	Solution : The global variable aud_state_status is set to AUD_FM_NONE once the file playing stopped.
    	
	April 27, 2007 DRT:OMAPS00128836 x0073106
	Description: Unexpected behavior when copying images with the same names  from a memory to another.
	Solution:In  mfw_fm_copy_start () added a if condition if(rfs_stat() && image file). 

    Apr 10, 2007    DRT: OMAPS00125309  x0039928
    Description: MM - Mono option does not work => MMI changes required
    Solution: Selected Output channel from the settings menu is set for all the players.
    
    Apr 06, 2007    DRT: OMAPS00124877  x0039928
    Description: MM: Voice limit Options doesn't work
    Solution: voice limit value is taken from the global variable set by menu options.
    
	Mar 28, 2007   ER: OMAPS00106188  x0039928
	Description: Align Midi application menu choices and audio Player menu choices 
	
    Mar 15, 2007    DRT: OMAPS00120201  x0039928
    Description: JPEG dir not created in FFS in N5.23 pre release
    Solution: The directories are created after format for NOR-MS, NAND and T-FLASH
   	
*! 10-Apr-2006 mf:  Modified x0020906.  Change required per SRxxxx
*! to provide File Manager Functionality.
*!
*!
*! 17-Jan-2006 mf: Revisions appear in reverse chronological order;
*! that is, newest first.  The date format is dd-Mon-yyyy.
* =========================================================== */

/******************************************************************************
                                Include Files
*******************************************************************************/
#define ENTITY_MFW

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define FF_MMI_RFS_ENABLED

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif 


#include "mfw_mfw.h"
#include "mfw_phb.h"
#include "mfw_phbi.h"
#include "mfw_cm.h"
#include "mfw_cmi.h"

#include "mfw_nm.h"
#include "mfw_sim.h"
#include "mfw_sima.h"
#include "mfw_nmi.h"
#include "mfw_simi.h"
#include "mfw_sms.h"
#include "mfw_smsi.h"
#include "mfw_win.h"

#include "ksd.h"
#include "psa.h"

#if defined (FAX_AND_DATA)
#include "aci_fd.h"
#endif

//#ifdef FF_MMI_RFS_ENABLED
#include "rfs/rfs_api.h"
//#else
#include "ffs/ffs_api.h"
//#endif

#include "message.h"
#include "prim.h"
#include "aci_cmh.h"

#include "cmh.h"
#include "phb.h"
#include "cmh_phb.h"

#include "mfw_ss.h"
#include "mfw_ssi.h"
#include "mfw_win.h"

#include "gdi.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif

#ifdef FF_MMI_MIDI_FORMAT
#include "bae/bae_options.h"
#endif

#include "mfw_fm.h"
/**********************NOTE***************************
FFS header To be replaced with FS Abstraction Header 
******************************************************/

#ifdef FF_MMI_A2DP_AVRCP
#include "mfw_bt_api.h"
#include "mfw_bt_private.h"
#endif


#ifdef INT_PHONEBOOK
#include "ATBPbGI.h"
#endif
/*******************************************************************************

                               Defines

*******************************************************************************/
EXTERN MfwHdr * current_mfw_elem;
static MSL_HANDLE msl_handle;
static T_RV_RETURN_PATH fm_aud_return_path;
T_MFW_AUD_PARA para_aud;
extern UBYTE mfw_player_playback_loop_flag;
extern T_AS_PLAYER_PARAMS player_para;
extern INT16 player_channel;
extern int aud_state_status;


/* OMAPS00151698, x0056422 */
#ifdef FF_MMI_A2DP_AVRCP
extern BMI_BT_STRUCTTYPE tGlobalBmiBtStruct;
#endif
//Daisy tang added for Real Resume feature 20071107
//start
BOOL isPlayer_Real_Pause = FALSE;
UINT32   last_file_size_played = 0; 
char last_inputFileName[FM_MAX_DIR_PATH_LENGTH];
T_WCHAR last_inputFileName_uc[FM_MAX_DIR_PATH_LENGTH];
T_AS_PLAYER_TYPE last_file_type;
BOOL last_play_bar;	
UINT32 last_para_aud_pt;
//end


/*******************************************************************************

                                Local prototypes

*******************************************************************************/

void Msl_mslil_Callback(void* handle, U32 ucp_type, U32 tCMd, U32 tStatus);
void mfw_fm_audPlay_cb(void *parameter);
void mfw_fm_signal(T_MFW_EVENT event, void *para);
BOOL mfw_fm_sign_exec (T_MFW_HDR * cur_elem, T_MFW_EVENT event, T_MFW_FM_PARA * para);
/* June 28, 2007 DRT: OMAPS00135749 x0062174 */
EXTERN void mmi_set_aud_state_status( T_FM_AUD_STATE status);


/*******************************************************************************
 $Function:		mfw_fm_create

 $Description:		This function  initialises an Mfw fm entity and adds it to the Window Stack

 $Returns:		T_MFW_HND	: A handle for the entity

 $Arguments:		hWin	: Parent Window Handle
 				event	: Event Mask of the events to be handled
 				cbfunc	: Callback function to handle the events
*******************************************************************************/
T_MFW_HND mfw_fm_create(T_MFW_HND hWin, T_MFW_EVENT event, T_MFW_CB cbfunc)
{
	T_MFW_HDR *hdr;
	T_MFW_FM  *fm_para;

	TRACE_FUNCTION("mfw_FM_create()");

	hdr = (T_MFW_HDR *) mfwAlloc(sizeof (T_MFW_HDR));
	fm_para = (T_MFW_FM *) mfwAlloc(sizeof (T_MFW_FM));

	if (!hdr OR !fm_para)
		return FALSE;

	/*
	* initialisation of the handler
	*/
	fm_para->emask   = event;
	fm_para->handler = cbfunc;

	hdr->data = fm_para;		
	hdr->type = MfwTypfm;

	/*
	* installation of the handler
	*/
	return mfwInsert((T_MFW_HDR *)hWin, hdr);
}

/*******************************************************************************

 $Function:		mfw_fm_delete

 $Description:		This function clears down an Mfw entity and removes it from the
 				Window Stack

 $Returns:		T_MFW_RES	: The result of the function

 $Arguments:		T_MFW_HND	: The Handle of the entity to be removed

*******************************************************************************/
T_MFW_RES mfw_fm_delete(T_MFW_HND hnd)
{
	TRACE_FUNCTION("mfw_FM_delete()");

	if (!hnd OR !((T_MFW_HDR *)hnd)->data)
		return MFW_RES_ILL_HND;

	if (!mfwRemove((T_MFW_HDR *)hnd))
		return MFW_RES_ILL_HND;

	mfwFree((U8 *)(((T_MFW_HDR *) hnd)->data),sizeof(T_MFW_FM));
	mfwFree((U8 *)hnd,sizeof(T_MFW_HDR));

	return MFW_RES_OK;
}

/*******************************************************************************

 $Function:		mfw_fm_sign_exec

 $Description:		This function sends the Mfw FM  events from the Mfw to the BMI.

 $Returns:		None

 $Arguments:		None

*******************************************************************************/
BOOL mfw_fm_sign_exec (T_MFW_HDR * cur_elem, T_MFW_EVENT event, T_MFW_FM_PARA * para)
{
	TRACE_FUNCTION("mfw_fm_sign_exec()");


	while (cur_elem)
	{
		/*
		* event handler is available
		*/
		if (cur_elem->type EQ MfwTypfm)
		{
			T_MFW_FM * fm_data;
			TRACE_EVENT("MfwTyFm");
			/*
			* handler is FM management handler
			*/
			fm_data = (T_MFW_FM *)cur_elem->data;
			if (fm_data->emask & event)
			{
				/*
				* event is expected by the call back function
				*/
				fm_data->event = event;
				switch (event)
				{
					/*
					** Generic Events
					*/
					case E_FM_THMB_INIT:
					case E_FM_THMB_DEINIT:
					case E_FM_THMB_GEN:
					case E_FM_IMG_INIT:
					case E_FM_IMG_DRAW:
					case E_FM_IMG_DEINIT:
					case E_FM_AUDIO_STOP:
						if(para!=NULL)
							memcpy (&fm_data->para, para, sizeof (T_MFW_FM_PARA));
						break;
				}
				/*
				* if call back defined, call it
				*/
				if (fm_data->handler)
				{
					// store current mfw elem
					current_mfw_elem = cur_elem;
					if ((*(fm_data->handler)) (fm_data->event, (void *)&fm_data->para))
						return TRUE;
				}
			}
		}
		cur_elem = cur_elem->next;
	}
	return FALSE;
}

/*******************************************************************************

 $Function:		mfw_FM_signal

 $Description:		This function sends the Mfw events from the Mfw to the BMI.

 $Returns:		None

 $Arguments:		T_MFW_EVENT	: The event to be sent to the BMI
 				void *			: Pointer to the Event data

*******************************************************************************/
void mfw_fm_signal(T_MFW_EVENT event, void *para)
{
	UBYTE temp;
	temp = dspl_Enable(0);

	TRACE_FUNCTION("mfw_FM_signal()");

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
			if (mfw_fm_sign_exec (mfwFocus, event, para))
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
			mfw_fm_sign_exec (mfwRoot, event, para);
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
			if (mfw_fm_sign_exec (h, event, para))
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
		
		mfw_fm_sign_exec (mfwRoot, event, para);
		
	}	
	dspl_Enable(temp);
	return;
}

/*******************************************************************************
 $Function:		mfw_fm_readDir

 $Description:		Reads the objects from the given directory

 $Returns:		FM_DIR_DOESNOTEXISTS/FM_READDIR_ERROR/FM_NO_ERROR

 $Arguments:		dir_path		: Current Directory Path
 				dir_name		: Current Directory Name
 				num_objs	: num of obejcts
 				obj_list		: object list
 				source		: Current Drive 
*******************************************************************************/
T_MFW_FM_STATUS mfw_fm_readDir(char *dir_path, char *dir_name, UBYTE *num_objs, T_FM_OBJ **obj_list, T_FM_DEVICE_TYPE  source, T_FM_APP_TYPE  app)
{
	char curDir[FM_MAX_DIR_PATH_LENGTH];
	/* x0083025 - OMAPS00156759 - Jan 29, 2008 */
#if defined(FF_MMI_UNICODE_SUPPORT)||defined (FF_MMI_RFS_ENABLED)
		T_WCHAR objName_uc[FM_MAX_OBJ_NAME_LENGTH];
		T_WCHAR curObj_uc[FM_MAX_DIR_PATH_LENGTH];
#else
		char objName_u8[FM_MAX_OBJ_NAME_LENGTH];
#endif
		
#ifdef FF_MMI_RFS_ENABLED
		char ext1[FM_MAX_EXT_LENGTH];
		T_RFS_DIR f_dir;
		UINT16 curDir_uc[FM_MAX_DIR_PATH_LENGTH];
		T_RFS_RET result;
		T_RFS_STAT f_stat;
#else
	T_FFS_DIR f_dir;
#endif
	int iCount=0;
	
	TRACE_FUNCTION("mfw_fm_readDir");

	*num_objs = 0;
	memset(curDir, 0, FM_MAX_DIR_PATH_LENGTH);
#ifdef FF_MMI_RFS_ENABLED
	switch (source)
	{
		case FM_NOR_FLASH:
			sprintf(curDir,"%s%s%s","/FFS",dir_path,dir_name);
			TRACE_EVENT_P1("Current Dir  %s",curDir);
			break;    
		case FM_NORMS_FLASH:
			sprintf(curDir,"%s%s%s","/NOR",dir_path,dir_name);
			TRACE_EVENT_P1("Current Dir  %s",curDir);
			break;    
		case FM_NAND_FLASH:
			sprintf(curDir,"%s%s%s","/NAND",dir_path,dir_name);
			TRACE_EVENT_P1("Current Dir  %s",curDir);
			break;
		case FM_T_FLASH:
			sprintf(curDir,"%s%s%s","/MMC",dir_path,dir_name);
			TRACE_EVENT_P1("Current Dir  %s",curDir);
			break;
		default:  
			TRACE_EVENT("Default:  Invalid value");  
			break;
	}
	
			convert_u8_to_unicode((const char *)curDir, curDir_uc);
			if (rfs_opendir (curDir_uc, &f_dir) < 0)
			{
				TRACE_EVENT_P1("Opening dir %s Failed",curDir);
				return FM_DIR_DOESNOTEXISTS; 
			}

			for (iCount = 0; rfs_readdir (&f_dir, objName_uc, FM_MAX_OBJ_NAME_LENGTH) > 0x0; )
			{
				if (objName_uc[0]  == '\0')
				{
					return FM_READDIR_ERROR;
				}
					if( objName_uc[0] != '.' )
					{
						/* x0083025 - OMAPS00156759 - Jan 29, 2008 */
						wstrcpy(curObj_uc, curDir_uc);
						{ // append '/' char to the end of string
							T_WCHAR *tmp = curObj_uc;
							while (*tmp) ++tmp;
							*tmp++ = '/';
							*tmp = 0;
						}
						wstrcat(curObj_uc, objName_uc);						

					result= rfs_stat(curObj_uc, &f_stat);

					if(result == RFS_EOK)
					{
						if(f_stat.file_dir.mode & RFS_IXUSR)
						{	
							obj_list[iCount] = (T_FM_OBJ*)mfwAlloc(sizeof(T_FM_OBJ));
							
							/* x0083025 - OMAPS00156759 - Jan 29, 2008 */
							#ifdef FF_MMI_UNICODE_SUPPORT
								wstrcpy(obj_list[iCount]->name_uc, objName_uc);
								TRACE_EVENT_P1("%s", obj_list[iCount]->name_uc);
							#endif
								convert_unicode_to_u8(objName_uc, obj_list[iCount]->name);
								TRACE_EVENT_P1("%s", obj_list[iCount]->name);
							(*num_objs)++;
							iCount++;
							if((*num_objs) >= FM_MAX_OBJ )
							{
								TRACE_EVENT("MAX COUNT Reached");
								if(source == FM_NOR_FLASH)			
									rfs_closedir(&f_dir);
								return FM_NO_ERROR;
							}
						}
						else
						{
								/* x0083025 - OMAPS00156759 - Jan 29, 2008 */
								convert_unicode_to_u8(wstrchr(objName_uc,'.')+1, ext1);		
								
								if(ext1)
								{
								switch(app)
								{
								case FM_IMAGE:
									if(strcmp(ext1, "jpg") == 0) 
									{	
										obj_list[iCount] = (T_FM_OBJ*)mfwAlloc(sizeof(T_FM_OBJ));
										
										/* x0083025 - OMAPS00156759 - Jan 29, 2008 */
										convert_unicode_to_u8(objName_uc, obj_list[iCount]->name);
										
									TRACE_EVENT_P1("%s", obj_list[iCount]->name);
									(*num_objs)++;
									iCount++;
									if((*num_objs) >= FM_MAX_OBJ )
									{
										TRACE_EVENT("MAX COUNT Reached");
										if(source == FM_NOR_FLASH)			
											rfs_closedir(&f_dir);
										return FM_NO_ERROR;
									}
								}
								break;
							case FM_AUDIO:
								if(
				
#ifdef FF_MP3_RINGER
									(strcmp(ext1, "mp3") == 0)
#else
									(0)
#endif								
#ifdef FF_AAC_RINGER
							 	 || (strcmp(ext1, "aac") == 0) 							 	 
#else
								 ||	(0)
#endif				

#ifdef FF_MMI_MIDI_FORMAT

#ifdef  PAL_ENABLE_XMF 
								 || (strcmp(ext1, "xmf") == 0)
#else
								 ||	(0)
#endif
#ifdef PAL_ENABLE_IMELODY 
							     || (strcmp(ext1, "imy") == 0)							     
#else
								 ||	(0)
#endif
#ifdef PAL_ENABLE_SMAF_MA3 
								 || (strcmp(ext1, "mmf") == 0)
#else
								 ||	(0)								 
#endif				
#ifdef PAL_ENABLE_XMF 
								 || (strcmp(ext1, "mxmf") == 0)
#else
								 ||	(0)								 
#endif			
#ifdef PAL_ENABLE_SMS
								 || (strcmp(ext1, "sms") == 0)
#else
								 ||	(0)								 
#endif									
#ifdef PAL_ENABLE_DIGITAL_AUDIO
								 || (strcmp(ext1, "wav") == 0) 
#else
								 ||	(0)
#endif			
#ifdef PAL_ENABLE_MIDI_NORMALIZER 
								 || (strcmp(ext1, "mid") == 0) 
#else
								 ||	(0)
#endif				

#endif
								  )
									
								{
									obj_list[iCount] = (T_FM_OBJ*)mfwAlloc(sizeof(T_FM_OBJ));
									/* x0083025 - OMAPS00156759 - Jan 29, 2008 */									
									#ifdef FF_MMI_UNICODE_SUPPORT
										wstrcpy(obj_list[iCount]->name_uc, objName_uc);
										TRACE_EVENT_P1("%s", obj_list[iCount]->name_uc);
									#else
										convert_unicode_to_u8(objName_uc, obj_list[iCount]->name);
										TRACE_EVENT_P1("%s", obj_list[iCount]->name);
									#endif
									(*num_objs)++;
									iCount++;
									if((*num_objs) >= FM_MAX_OBJ )
									{
										TRACE_EVENT("MAX COUNT Reached");
      										if(source == FM_NOR_FLASH)			
											rfs_closedir(&f_dir);
										 return FM_NO_ERROR;
									}				
								}
								break;
							}
							}
					}
				}
			}    
			}    
			TRACE_EVENT_P2("Dir %s contains %d Objects",curDir,iCount);
         if(source == FM_NOR_FLASH)			
			rfs_closedir(&f_dir);
#else
			sprintf(curDir,"%s%s",dir_path,dir_name);
			TRACE_EVENT_P1("Current Dir  %s",curDir);

			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			if (ffs_opendir (curDir, &f_dir) < 0)
			{
				TRACE_EVENT_P1("Opening dir %s Failed",curDir);
				return FM_DIR_DOESNOTEXISTS; 
			}
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
				for (iCount = 0; ffs_readdir (&f_dir, objName_u8, FM_MAX_OBJ_NAME_LENGTH) > 0x0; iCount++)
				{
					if (objName_u8[0]	== '\0')
					{
						return FM_READDIR_ERROR;
					}
					if( objName_u8[0] != '.' )
					{
						obj_list[iCount] = (T_FM_OBJ*)mfwAlloc(sizeof(T_FM_OBJ));
						strcpy(obj_list[iCount]->name, objName_u8);
						TRACE_EVENT_P1("%s", obj_list[iCount]->name);
						(*num_objs)++;
						if((*num_objs) >= FM_MAX_OBJ )
						{
							TRACE_EVENT("MAX COUNT Reached");
							 break;
						}
					}
				}	 
				TRACE_EVENT_P2("Dir %s contains %d Objects",curDir,iCount);
#endif			
				
	
	return FM_NO_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_rename

 $Description:		Renames the selcted object with the new name

 $Returns:		FM_RENAME_ERROR/FM_NO_ERROR

 $Arguments:		old_filename		: Old filename
 				new_filename		: New filename
 				source			: Current Drive
*******************************************************************************/
#ifdef FF_MMI_UNICODE_SUPPORT
T_MFW_FM_STATUS mfw_fm_rename(T_WCHAR* old_filename, T_WCHAR* new_filename, T_FM_DEVICE_TYPE  source)
#else
T_MFW_FM_STATUS mfw_fm_rename(char* old_filename, char * new_filename, T_FM_DEVICE_TYPE  source)
#endif
{

#ifdef FF_MMI_RFS_ENABLED 
	T_RFS_RET rfsResult;
	char old_filename_mt[FM_MAX_DIR_PATH_LENGTH];
	char new_filename_mt[FM_MAX_DIR_PATH_LENGTH];
	UINT16 new_filename_uc[FM_MAX_DIR_PATH_LENGTH];
	UINT16 old_filename_uc[FM_MAX_DIR_PATH_LENGTH];
#else

#if defined(FF_MMI_UNICODE_SUPPORT) && !defined(FF_MMI_RFS_ENABLED)
	char old_filename_u8[FM_MAX_DIR_PATH_LENGTH];
	char new_filename_u8[FM_MAX_DIR_PATH_LENGTH];
#endif

	T_FFS_RET ffsResult;
#endif

	TRACE_FUNCTION("mfw_fm_rename");

#ifdef FF_MMI_RFS_ENABLED 		
	switch (source)
	{
		case FM_NOR_FLASH:
			strcpy(old_filename_mt, "/FFS");
			strcpy(new_filename_mt, "/FFS");
			break;
		case FM_NORMS_FLASH:
			strcpy(old_filename_mt, "/NOR");
			strcpy(new_filename_mt, "/NOR");
			break;
		case FM_NAND_FLASH:
			strcpy(old_filename_mt, "/NAND");
			strcpy(new_filename_mt, "/NAND");
			break;
		case FM_T_FLASH:
			strcpy(old_filename_mt, "/MMC");
			strcpy(new_filename_mt, "/MMC");
			break;
	}

#ifdef FF_MMI_UNICODE_SUPPORT
	convert_u8_to_unicode(old_filename_mt,old_filename_uc);
	convert_u8_to_unicode(new_filename_mt, new_filename_uc);
	wstrcat(old_filename_uc, old_filename);
	wstrcat(new_filename_uc, new_filename);
#else
	strcat(old_filename_mt, old_filename);
	strcat(new_filename_mt, new_filename);
	convert_u8_to_unicode(old_filename_mt,old_filename_uc);
	convert_u8_to_unicode(new_filename_mt, new_filename_uc);
#endif
 			
	rfsResult = rfs_rename(old_filename_uc, new_filename_uc);
	if(rfsResult != RFS_EOK)
	{
		return FM_RENAME_ERROR;
	}
#else

#ifdef FF_MMI_UNICODE_SUPPORT
	convert_unicode_to_u8(old_filename, old_filename_u8);
	convert_unicode_to_u8(new_filename, new_filename_u8;

	ffsResult = ffs_rename(old_filename_u8,new_filename_u8);
#else
	ffsResult = ffs_rename(old_filename,new_filename);
#endif
	if(ffsResult != EFFS_OK)
			{
				return FM_RENAME_ERROR;
			}
#endif			
	return FM_NO_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_rename_image

 $Description:		Renames the selected image and its thumbnail with the new name

 $Returns:		FM_RENAME_ERROR/FM_NO_ERROR

 $Arguments:		path				: Current directory path
 				dirname			: Current Directory name
 				old_filename		: Old filename
 				new_filename		: New filename
 				source			: Current Drive
*******************************************************************************/
T_MFW_FM_STATUS mfw_fm_rename_image(char * path, char * dirname, char* old_filename, char * new_filename, T_FM_DEVICE_TYPE  source)
{
	T_MFW_FM_STATUS ffsResult;
	char t_oldfilename[FM_MAX_DIR_PATH_LENGTH];
	char t_newfilename[FM_MAX_DIR_PATH_LENGTH];

#ifdef FF_MMI_UNICODE_SUPPORT
	T_WCHAR oldfilename_uc[FM_MAX_DIR_PATH_LENGTH];
	T_WCHAR newfilename_uc[FM_MAX_DIR_PATH_LENGTH]; 
#endif

	TRACE_FUNCTION("mfw_fm_rename_image");

#ifdef FF_MMI_RFS_ENABLED 			

	sprintf(t_oldfilename,"%s%s/%s.jpg",path,dirname,old_filename);
	sprintf(t_newfilename,"%s%s/%s.jpg",path,dirname,new_filename);
/* February 8, 2008 DRT:OMAPS00156759 (x0083025) For Unicode Support--> Start*/	

#ifdef FF_MMI_UNICODE_SUPPORT		
	convert_u8_to_unicode(t_oldfilename, oldfilename_uc); 
	convert_u8_to_unicode(t_newfilename, newfilename_uc);	
	ffsResult = mfw_fm_rename(oldfilename_uc,newfilename_uc,source); 
#else
	ffsResult = mfw_fm_rename(t_oldfilename,t_newfilename,source);
#endif
			if(ffsResult != FM_NO_ERROR)
			{
				return FM_RENAME_ERROR;
			}
	switch(source)
	{
		case FM_NOR_FLASH:
			sprintf(t_oldfilename,"%s%s/%s.tmb",NORDIR,THUMB,old_filename);
			sprintf(t_newfilename,"%s%s/%s.tmb",NORDIR,THUMB,new_filename);
			break;
		case FM_NORMS_FLASH:
			sprintf(t_oldfilename,"%s%s/%s.tmb",NORMSDIR,THUMB,old_filename);
			sprintf(t_newfilename,"%s%s/%s.tmb",NORMSDIR,THUMB,new_filename);
			break;			
		case FM_NAND_FLASH:
			sprintf(t_oldfilename,"%s%s/%s.tmb",NANDDIR,THUMB,old_filename);
			sprintf(t_newfilename,"%s%s/%s.tmb",NANDDIR,THUMB,new_filename);
			break;
		case FM_T_FLASH:
			sprintf(t_oldfilename,"%s%s/%s.tmb",TFLASHDIR,THUMB,old_filename);
			sprintf(t_newfilename,"%s%s/%s.tmb",TFLASHDIR,THUMB,new_filename);
			break;
	}
	
#ifdef FF_MMI_UNICODE_SUPPORT		
	convert_u8_to_unicode(t_oldfilename, oldfilename_uc); 
	convert_u8_to_unicode(t_newfilename, newfilename_uc);	
	ffsResult = mfw_fm_rename(oldfilename_uc,newfilename_uc,source); 
#else
	ffsResult = mfw_fm_rename(t_oldfilename,t_newfilename,source);
#endif
	if(ffsResult != FM_NO_ERROR)
	{
		return FM_RENAME_ERROR;
	}
#else
			sprintf(t_oldfilename,"%s%s/%s.jpg",path,dirname,old_filename);
			sprintf(t_newfilename,"%s%s/%s.jpg",path,dirname,new_filename);

			ffsResult = mfw_fm_rename(t_oldfilename,t_newfilename,source);
			if(ffsResult != FM_NO_ERROR)
			{
				return FM_RENAME_ERROR;
			}
			sprintf(t_oldfilename,"%s%s/%s.tmb",NORDIR,THUMB,old_filename);
			sprintf(t_newfilename,"%s%s/%s.tmb",NORDIR,THUMB,new_filename);

			ffsResult = mfw_fm_rename(t_oldfilename,t_newfilename,source);

			if(ffsResult != FM_NO_ERROR)
			{
				return FM_RENAME_ERROR;
			}
#endif
	return FM_NO_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_createRootDir

 $Description:		Creates the root jpeg, tones directory

 $Returns:		FM_NO_ERROR

 $Arguments:		source	: Current Drive
*******************************************************************************/
T_MFW_FM_STATUS mfw_fm_createRootDir(T_FM_DEVICE_TYPE  source)
{
#ifdef FF_MMI_RFS_ENABLED
	T_RFS_RET ffsResult;
	T_RFS_DIR f_dir;
	UINT16 dir_path_uc[FM_MAX_DIR_PATH_LENGTH];
#else
	T_FFS_RET ffsResult;
	T_FFS_DIR f_dir;
#endif
	char dir_path[FM_MAX_DIR_PATH_LENGTH];

	TRACE_FUNCTION("mfw_fm_createRootDir");

#ifdef FF_MMI_RFS_ENABLED	
	TRACE_EVENT_P1("Source %d",source);
	switch (source)
	{
		case FM_NOR_FLASH:
			sprintf(dir_path,"%s%s%s","/FFS",NORDIR,NORDIR_IMG);
			convert_u8_to_unicode(dir_path, dir_path_uc);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = rfs_opendir(dir_path_uc,&f_dir); 
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			if(ffsResult > 0)
				rfs_closedir(&f_dir);
			else if(RFS_ENOENT == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			}
			sprintf(dir_path,"%s%s%s","/FFS",NORDIR,THUMB);
			convert_u8_to_unicode(dir_path, dir_path_uc);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = rfs_opendir(dir_path_uc,&f_dir);       
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			if(ffsResult > 0)
				rfs_closedir(&f_dir);
			else if(RFS_ENOENT == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			}
			sprintf(dir_path,"%s%s%s","/FFS",NORDIR,NORDIR_AUD);
			convert_u8_to_unicode(dir_path, dir_path_uc);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = rfs_opendir(dir_path_uc,&f_dir);       
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			if(ffsResult > 0)
				rfs_closedir(&f_dir);
			else if(RFS_ENOENT == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			}
			break;
		case FM_NORMS_FLASH:
			sprintf(dir_path,"%s%s%s","/NOR",NORMSDIR,NORMSDIR_IMG);
			convert_u8_to_unicode(dir_path, dir_path_uc);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = rfs_opendir(dir_path_uc,&f_dir); 
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
	//		if(ffsResult > 0)
	//			rfs_closedir(&f_dir);
	//		else 
			if(RFS_ENOENT == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			}
			sprintf(dir_path,"%s%s%s","/NOR",NORMSDIR,THUMB);
			convert_u8_to_unicode(dir_path, dir_path_uc);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = rfs_opendir(dir_path_uc,&f_dir);       
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
		//	if(ffsResult > 0)
		//		rfs_closedir(&f_dir);
		//	else 
			if(RFS_ENOENT == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			}
			sprintf(dir_path,"%s%s%s","/NOR",NORMSDIR,NORMSDIR_AUD);
			convert_u8_to_unicode(dir_path, dir_path_uc);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = rfs_opendir(dir_path_uc,&f_dir);       
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
		//	if(ffsResult > 0)
		//		rfs_closedir(&f_dir);
		//	else 
			if(RFS_ENOENT == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			}
			break;			
		case FM_NAND_FLASH:
			/**********************NOTE***************************
			FS Abstraction API to be added
			******************************************************/
			sprintf(dir_path,"%s%s%s","/NAND",NANDDIR,NANDDIR_IMG);
			convert_u8_to_unicode(dir_path, dir_path_uc);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = rfs_opendir(dir_path_uc,&f_dir);       
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
		//	if(ffsResult > 0)
		//		rfs_closedir(&f_dir);
//			else
				if(RFS_ENOENT == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			}
			sprintf(dir_path,"%s%s%s","/NAND",NANDDIR,THUMB);
			convert_u8_to_unicode(dir_path, dir_path_uc);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = rfs_opendir(dir_path_uc,&f_dir);       
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
		//	if(ffsResult > 0)
		//		rfs_closedir(&f_dir);
		//	else
		if(RFS_ENOENT == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			}
			sprintf(dir_path,"%s%s%s","/NAND",NANDDIR,NANDDIR_AUD);
			convert_u8_to_unicode(dir_path, dir_path_uc);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = rfs_opendir(dir_path_uc,&f_dir);       
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
		//	if(ffsResult > 0)
		//		rfs_closedir(&f_dir);
		//	else 
		if(RFS_ENOENT == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			}
			break;
		case FM_T_FLASH:
			/**********************NOTE***************************
			FS Abstraction API to be added
			******************************************************/
			sprintf(dir_path,"%s%s%s","/MMC",TFLASHDIR,TFLASHDIR_IMG);
			convert_u8_to_unicode(dir_path, dir_path_uc);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = rfs_opendir(dir_path_uc,&f_dir);      
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
		//	if(ffsResult > 0)
		//		rfs_closedir(&f_dir);
		//	else
		if(RFS_ENOENT == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			}
			sprintf(dir_path,"%s%s%s","/MMC",TFLASHDIR,THUMB);
			convert_u8_to_unicode(dir_path, dir_path_uc);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = rfs_opendir(dir_path_uc,&f_dir);       
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
	//		if(ffsResult > 0)
		//		rfs_closedir(&f_dir);
		//	else
		if(RFS_ENOENT == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			}
			sprintf(dir_path,"%s%s%s","/MMC",TFLASHDIR,TFLASHDIR_AUD);
			convert_u8_to_unicode(dir_path, dir_path_uc);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = rfs_opendir(dir_path_uc,&f_dir);     
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
	//		if(ffsResult > 0)
		//		rfs_closedir(&f_dir);
		//	else
		if(RFS_ENOENT == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
			}
			break;
	}
#else
			sprintf(dir_path,"%s%s",NORDIR,NORDIR_IMG);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = ffs_opendir(dir_path,&f_dir);       
			if(EFFS_NOTFOUND == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = ffs_mkdir(dir_path);
			}
			sprintf(dir_path,"%s%s",NORDIR,THUMB);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = ffs_opendir(dir_path,&f_dir);       
			if(EFFS_NOTFOUND == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = ffs_mkdir(dir_path);
			}
			sprintf(dir_path,"%s%s",NORDIR,NORDIR_AUD);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			ffsResult = ffs_opendir(dir_path,&f_dir);       
			if(EFFS_NOTFOUND == ffsResult)
			{
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffsResult = ffs_mkdir(dir_path);
			}
#endif
	return FM_NO_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_newdir

 $Description:		creates a new direcotry with the given name

 $Returns:		FM_DIR_EXISTS/FM_NO_ERROR

 $Arguments:		dirname		: new directory name
 				source		: Current Drive
*******************************************************************************/
T_MFW_FM_STATUS mfw_fm_newdir(char *dirname, T_FM_DEVICE_TYPE  source)
{
#ifdef FF_MMI_RFS_ENABLED
	T_RFS_RET ffsResult;
	T_RFS_DIR f_dir;
	char pathname[FM_MAX_DIR_PATH_LENGTH];
	UINT16 pathname_uc[FM_MAX_DIR_PATH_LENGTH];
#else
	T_FFS_RET ffsResult;
	T_FFS_DIR f_dir;
#endif	

	TRACE_FUNCTION("mfw_fm_newdir");

#ifdef FF_MMI_RFS_ENABLED	
	switch (source)
	{
		case FM_NOR_FLASH:
			strcpy(pathname,"/FFS");
			break;
		case FM_NORMS_FLASH:
			strcpy(pathname,"/NOR");
			break;			
		case FM_NAND_FLASH:
			strcpy(pathname,"/NAND");
			break;
		case FM_T_FLASH:
			strcpy(pathname,"/MMC");
			break;
	}
			strcat(pathname,dirname);
			convert_u8_to_unicode(pathname,pathname_uc);

			ffsResult = rfs_opendir(pathname_uc,&f_dir);       
			if(RFS_ENOENT == ffsResult)
			{
				ffsResult = rfs_mkdir(pathname_uc,RFS_IRWXU);
			}
			else
			{
				return FM_DIR_EXISTS;
			}
#else

			ffsResult = ffs_opendir(dirname,&f_dir);       
			if(EFFS_NOTFOUND == ffsResult)
			{
				ffsResult = ffs_mkdir(dirname);
			}
			else
			{
				return FM_DIR_EXISTS;
			}
#endif
	return FM_NO_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_readProperties

 $Description:		Reads the properties of all the objects in the given directory

 $Returns:		None

 $Arguments:		dir_path		: Current Directory Path
 				dir_name		: Current Directory Name
 				num_objs	: num of obejcts
 				obj_list		: object list
 				obj_properties: Object properties
 				source		: Current Drive 
*******************************************************************************/
void mfw_fm_readProperties(char *dir_path, char *dir_name, UBYTE num_objs, T_FM_OBJ **obj_list, T_FM_OBJ_PROPERTIES **obj_properties,T_FM_DEVICE_TYPE  source, T_FM_APP_TYPE app)
{
	/* January 16, 2008 DRT:OMAPS00156759 (x0082844) Shubhro -> For Unicode Support--> Start*/
	#if defined(FF_MMI_UNICODE_SUPPORT)||defined (FF_MMI_RFS_ENABLED)
		T_WCHAR curObj_uc[FM_MAX_DIR_PATH_LENGTH];
	#else
		char curObj_u8[FM_MAX_DIR_PATH_LENGTH];
	#endif
	/* January 16, 2008 DRT:OMAPS00156759 (x0082844) Shubhro --> End*/
	
	char curPath[FM_MAX_DIR_PATH_LENGTH];
	int iCount=0;

#ifdef FF_MMI_RFS_ENABLED
	T_RFS_RET result;
	T_RFS_STAT f_stat;	
	T_RTC_DATE_TIME  time_and_date;
#else
	T_FFS_RET result;
	T_FFS_STAT f_stat;
#endif	
	
	TRACE_FUNCTION("mfw_fm_readProperties");

#ifdef FF_MMI_RFS_ENABLED
	switch (source)
	{
		case FM_NOR_FLASH:
			sprintf(curPath,"%s%s%s/","/FFS",dir_path,dir_name);
			break;
		case FM_NORMS_FLASH:
			sprintf(curPath,"%s%s%s/","/NOR",dir_path,dir_name);
			break;			
		case FM_NAND_FLASH:
			sprintf(curPath,"%s%s%s/","/NAND",dir_path,dir_name);
			break;
		case FM_T_FLASH:
			sprintf(curPath,"%s%s%s/","/MMC",dir_path,dir_name);
			break;
	}    


		for (iCount = 0;iCount<num_objs; iCount++)
			{
			
		/* February 8, 2008 DRT:OMAPS00156759 (x0083025) For Unicode Support--> Start*/	
				convert_u8_to_unicode(curPath, curObj_uc);
		
			#ifdef FF_MMI_UNICODE_SUPPORT
				// If it's an image it's stored as char
				if (app == FM_AUDIO)
				{
					wstrcat(curObj_uc, obj_list[iCount]->name_uc);
				}
				else
			#endif
				{ 
					T_WCHAR filename_uc[FM_MAX_OBJ_NAME_LENGTH];
					convert_u8_to_unicode(obj_list[iCount]->name, filename_uc);
					wstrcat(curObj_uc, filename_uc);
				}
				result= rfs_stat(curObj_uc, &f_stat);
				if(result == RFS_EOK)
				{
					if(f_stat.file_dir.mode & RFS_IXUSR)
						obj_list[iCount]->type=OBJECT_TYPE_FOLDER;
					else
						obj_list[iCount]->type=OBJECT_TYPE_FILE;
				}
				TRACE_EVENT_P2("Object %s type %d",curObj_uc, obj_list[iCount]->type);

				obj_properties[iCount] = (T_FM_OBJ_PROPERTIES *)mfwAlloc(sizeof(T_FM_OBJ_PROPERTIES));
				obj_properties[iCount]->size = f_stat.file_dir.size;
				result = rfs_getdatetime(curObj_uc,NULL  /* creation date and time */,&time_and_date  /* last modified date and time */);
				sprintf(obj_properties[iCount]->date,"%d-%d-%d", time_and_date.day, time_and_date.month, 2000 + time_and_date.year);
				sprintf(obj_properties[iCount]->time,"%d.%d.%d", time_and_date.hour, time_and_date.minute, time_and_date.second);
			}

#else
		sprintf(curPath,"%s%s/",dir_path,dir_name);
		for (iCount = 0;iCount<num_objs; iCount++)
		{
			sprintf(curObj_u8,"%s%s",curPath,obj_list[iCount]->name);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			result= ffs_stat( curObj_u8, (void *)&f_stat );
			if(result == EFFS_OK)
			{
				switch(f_stat.type)
				{
					case 	OT_FILE:
						obj_list[iCount]->type=OBJECT_TYPE_FILE;
						break;
					case 	OT_DIR:
						obj_list[iCount]->type=OBJECT_TYPE_FOLDER;
						break;
					default:
						obj_list[iCount]->type=OBJECT_TYPE_NONE;
						break;
				}
			}
			TRACE_EVENT_P2("name %s, Object type %d", obj_list[iCount]->name,obj_list[iCount]->type);
			obj_properties[iCount] = (T_FM_OBJ_PROPERTIES*)mfwAlloc(sizeof(T_FM_OBJ_PROPERTIES));
			obj_properties[iCount]->size=f_stat.size;
		}
#endif
}

/*******************************************************************************
 $Function:		mfw_fm_readFIleFolders

 $Description:		Classifies all the objects as either File or Directory

 $Returns:		None

 $Arguments:		fm_curr_dir	: Current directory
 				source		: Current Drive
*******************************************************************************/
void mfw_fm_readFileFolders(char *dir_path, char *dir_name, UBYTE num_objs, T_FM_OBJ **obj_list, T_FM_DEVICE_TYPE  source)
{
	char curObj[FM_MAX_DIR_PATH_LENGTH];
	char curPath[FM_MAX_DIR_PATH_LENGTH];
	int iCount=0;
#ifdef FF_MMI_RFS_ENABLED
	T_RFS_RET result;
	T_RFS_STAT f_stat;
	UINT16 curObj_uc[FM_MAX_DIR_PATH_LENGTH];
#else
	T_FFS_RET result;
	T_FFS_STAT f_stat;
#endif

TRACE_FUNCTION("mfw_fm_readFileFolders");

#ifdef FF_MMI_RFS_ENABLED
	switch (source)
	{
		case FM_NOR_FLASH:
			sprintf(curPath,"%s%s%s/","/FFS",dir_path,dir_name);
			break;
		case FM_NORMS_FLASH:
			sprintf(curPath,"%s%s%s/","/NOR",dir_path,dir_name);
			break;			
		case FM_NAND_FLASH:
			sprintf(curPath,"%s%s%s/","/NAND",dir_path,dir_name);
			break;
		case FM_T_FLASH:
			sprintf(curPath,"%s%s%s/","/MMC",dir_path,dir_name);
			break;
	}    
			for (iCount = 0;iCount<num_objs; iCount++)
			{
				sprintf(curObj,"%s%s",curPath,obj_list[iCount]->name);
				convert_u8_to_unicode(curObj, curObj_uc);
				result= rfs_stat(curObj_uc, &f_stat);
				if(result == RFS_EOK)
				{
					if(f_stat.file_dir.mode & RFS_IXUSR)
						obj_list[iCount]->type=OBJECT_TYPE_FOLDER;
					else
						obj_list[iCount]->type=OBJECT_TYPE_FILE;
				}
				TRACE_EVENT_P2("Object %s type %d",curObj, obj_list[iCount]->type);
			}
#else
		sprintf(curPath,"%s%s/",dir_path,dir_name);

		for (iCount = 0;iCount<num_objs; iCount++)
			{
				sprintf(curObj,"%s%s",curPath,obj_list[iCount]->name);

				result= ffs_stat( curObj, (void *)&f_stat );
				if(result == EFFS_OK)
				{
					switch(f_stat.type)
					{
						case 	OT_FILE:
							obj_list[iCount]->type=OBJECT_TYPE_FILE;
							break;
						case 	OT_DIR:
							obj_list[iCount]->type=OBJECT_TYPE_FOLDER;
							break;
						default:
							obj_list[iCount]->type=OBJECT_TYPE_NONE;
							break;
					}
				}
				TRACE_EVENT_P2("Object %s type %d",curObj, obj_list[iCount]->type);
			}
#endif
}
/*******************************************************************************
 $Function:		mfw_fm_getMemStat

 $Description:		Extracts the memory usage details of a given drive

 $Returns:		None

 $Arguments:		nfree		: free bytes
 				nused		: used bytes
 				source		: Current Drive
*******************************************************************************/
void mfw_fm_getMemStat(int *nfree, int *nused,T_FM_DEVICE_TYPE  source)	
{

	T_RFS_STAT f_stat;
	char curObj[FM_MAX_DIR_PATH_LENGTH];
	UINT16 curObj_uc[FM_MAX_DIR_PATH_LENGTH];
	TRACE_FUNCTION("mfw_fm_getMemStat");

	memset(curObj, 0, FM_MAX_DIR_PATH_LENGTH);
	switch (source)
	{
		case FM_NOR_FLASH:
			strcpy(curObj,"/FFS");
			break;
		case FM_NORMS_FLASH:
			strcpy(curObj,"/NOR");
			break;			
		case FM_NAND_FLASH:
			strcpy(curObj,"/NAND");
			break;
		case FM_T_FLASH:
			strcpy(curObj,"/MMC");
			break;
	}
		convert_u8_to_unicode(curObj, curObj_uc);
		rfs_stat(curObj_uc, &f_stat);    /* warning fix */
		*nfree = f_stat.mount_point.free_space;
		*nused = f_stat.mount_point.used_size;
		
}

/*******************************************************************************
 $Function:		mfw_fm_format

 $Description:		Formats the given drive

 $Returns:		FM_NO_ERROR/FM_FORMAT_ERROR

 $Arguments:		source		: Current Drive
*******************************************************************************/
T_MFW_FM_STATUS mfw_fm_format(T_FM_DEVICE_TYPE  source)	
{
	T_RFS_RET result;
	UINT16 mount_point_uc[6];
	TRACE_FUNCTION("mfw_fm_format");
	memset(mount_point_uc, 0x00,6);
	switch (source)
	{
		case FM_NORMS_FLASH:
			convert_u8_to_unicode("/NOR", mount_point_uc);
			break;
		case FM_NAND_FLASH:
			convert_u8_to_unicode("/NAND", mount_point_uc);
			break;
		case FM_T_FLASH:
			convert_u8_to_unicode("/MMC", mount_point_uc);
			break;
	}

	result = rfs_preformat(mount_point_uc, 0xDEAD);
	if(result < RFS_EOK)
		return FM_FORMAT_ERROR;
	else
	{
		result = rfs_format(mount_point_uc, NULL, 0x2BAD);
		if (result < RFS_EOK)
			return FM_FORMAT_ERROR;
		else
		{
		/*  Mar 15, 2007    DRT: OMAPS00120201  x0039928 */
		/* Fix: Creating the default directories for the selected device after format */
			mfw_fm_createRootDir(source);
			return FM_NO_ERROR;
		}
	}
}

/*******************************************************************************
 $Function:		mfw_fm_remove

 $Description:		Deletes the selected object

 $Returns:		FM_DELETE_DIRNOTEMPTY/FM_DELETE_ERROR/FM_NO_ERROR

 $Arguments:		source		: Current Drive
*******************************************************************************/
#ifdef FF_MMI_UNICODE_SUPPORT
T_MFW_FM_STATUS mfw_fm_remove(T_WCHAR* objname_u16,T_FM_DEVICE_TYPE  source)	
#else
T_MFW_FM_STATUS mfw_fm_remove(char* objname,T_FM_DEVICE_TYPE  source)	
#endif
{
#ifdef FF_MMI_RFS_ENABLED
	T_RFS_RET rfsResult;
	char objname_mt[FM_MAX_DIR_PATH_LENGTH];
	T_WCHAR objname_uc[FM_MAX_DIR_PATH_LENGTH];
#else
	T_FFS_RET ffsResult;
#endif
	

	TRACE_FUNCTION("mfw_fm_remove");

#ifdef FF_MMI_RFS_ENABLED
	switch (source)
	{
		case FM_NOR_FLASH:
			strcpy(objname_mt,"/FFS");
			break;
		case FM_NORMS_FLASH:
			strcpy(objname_mt,"/NOR");
			break;			
		case FM_NAND_FLASH:
			strcpy(objname_mt,"/NAND");
			break;
		case FM_T_FLASH:
			strcpy(objname_mt,"/MMC");
			break;
	}
  #ifdef FF_MMI_UNICODE_SUPPORT
	convert_u8_to_unicode(objname_mt, objname_uc);
	wstrcat(objname_uc, objname_u16);
  #else
			strcat(objname_mt,objname);
			convert_u8_to_unicode(objname_mt, objname_uc);
  #endif
	TRACE_EVENT_P1("objname  %s",objname_uc);
			rfsResult = rfs_remove(objname_uc);
			if(rfsResult != RFS_EOK)
			{
				return FM_DELETE_ERROR;
			}
#else
	{
	#ifdef FF_MMI_UNICODE_SUPPORT
		char objname[FM_MAX_DIR_PATH_LENGTH];
		convert_unicode_to_u8(objname_u16, objname);
	#endif
	TRACE_EVENT_P1("objname  %s",objname);
			ffsResult = ffs_remove(objname);
	}
			if(ffsResult != EFFS_OK)
			{
				if(ffsResult == EFFS_DIRNOTEMPTY)
					return FM_DELETE_DIRNOTEMPTY;
				else
					return FM_DELETE_ERROR;
			}

#endif
	return FM_NO_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_remove_image

 $Description:		Deletes the selcted image and its thumbnail

 $Returns:		FM_DELETE_ERROR/FM_NO_ERROR

 $Arguments:		path				: Current directory path
 				dirname			: Current Directory name
 				old_filename		: Old filename
 				new_filename		: New filename
 				source			: Current Drive
*******************************************************************************/
T_MFW_FM_STATUS mfw_fm_remove_image(char * path, char * dirname, char* objname,T_FM_DEVICE_TYPE  source)	
{
	char obj[FM_MAX_DIR_PATH_LENGTH];
	T_MFW_FM_STATUS ffsResult;

	TRACE_FUNCTION("mfw_fm_delete_image");

	sprintf(obj,"%s%s/%s.jpg",path,dirname,objname);
	TRACE_EVENT_P1("obj %s",obj);
#ifdef FF_MMI_UNICODE_SUPPORT
	{
		T_WCHAR obj_uc[FM_MAX_DIR_PATH_LENGTH];
		convert_u8_to_unicode(obj, obj_uc);
		ffsResult = mfw_fm_remove(obj_uc,source);
	}
#else
	ffsResult = mfw_fm_remove(obj,source);
#endif
	if(ffsResult != FM_NO_ERROR)
	{
		return FM_DELETE_ERROR;
	}
	switch(source)
	{
		case FM_NOR_FLASH:
			sprintf(obj,"%s%s/%s.tmb",NORDIR,THUMB,objname);
			break;
		case FM_NORMS_FLASH:
			sprintf(obj,"%s%s/%s.tmb",NORMSDIR,THUMB,objname);
			break;			
		case FM_NAND_FLASH:
			sprintf(obj,"%s%s/%s.tmb",NANDDIR,THUMB,objname);
			break;
		case FM_T_FLASH:
			sprintf(obj,"%s%s/%s.tmb",TFLASHDIR,THUMB,objname);
			break;
	}
#ifdef FF_MMI_UNICODE_SUPPORT
	{
		T_WCHAR obj_uc[FM_MAX_DIR_PATH_LENGTH];
		convert_u8_to_unicode(obj, obj_uc);
		ffsResult = mfw_fm_remove(obj_uc,source);
	}
#else
	ffsResult = mfw_fm_remove(obj,source);
#endif
	if(ffsResult != FM_NO_ERROR)
	{
		return FM_DELETE_ERROR;
	}
	return FM_NO_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_copy_start

 $Description:		starts copying the give file

 $Returns:		FM_COPY_ERROR/FM_COPY_DONE/FM_COPY_PROGRESS

 $Arguments:		data		: Copy/Move operation related structure
*******************************************************************************/
T_FM_COPY_STATE mfw_fm_copy_start(T_MFW_FM_COPYMOVE_STRUCT *data)	
{
	char  *ext2;
	T_RFS_STAT f_stat;
	T_RFS_SIZE iResult = 0;
	UINT16 source_file_uc[FM_MAX_DIR_PATH_LENGTH];
	UINT16 destination_file_uc[FM_MAX_DIR_PATH_LENGTH];
	char source_file[FM_MAX_DIR_PATH_LENGTH];
	char destination_file[FM_MAX_DIR_PATH_LENGTH];
	
	TRACE_FUNCTION("mfw_fm_copy_start");

	switch (data->source_type)
	{
		case FM_NOR_FLASH:
			strcpy(source_file,"/FFS");
			break;
		case FM_NORMS_FLASH:
			strcpy(source_file,"/NOR");
			break;			
		case FM_NAND_FLASH:
			strcpy(source_file,"/NAND");
			break;
		case FM_T_FLASH:
			strcpy(source_file,"/MMC");
			break;
	}

	switch (data->destination_type)
	{
		case FM_NOR_FLASH:
			strcpy(destination_file,"/FFS");
			break;		
		case FM_NORMS_FLASH:
			strcpy(destination_file,"/NOR");
			break;				
		case FM_NAND_FLASH:
			strcpy(destination_file,"/NAND");
			break;
		case FM_T_FLASH:
			strcpy(destination_file,"/MMC");
			break;
	}
#ifdef FF_MMI_UNICODE_SUPPORT
	convert_u8_to_unicode(source_file, source_file_uc);
	convert_u8_to_unicode(destination_file, destination_file_uc);
	wstrcat(source_file_uc, data->sourceFile);
	wstrcat(destination_file_uc, data->destinationFile);
#else
	strcat(source_file, data->sourceFile);
	strcat(destination_file, data->destinationFile);

	convert_u8_to_unicode(source_file, source_file_uc);
	convert_u8_to_unicode(destination_file, destination_file_uc);
#endif

	ext2=(char *)mmi_fm_get_ext(source_file);//April 27, 2007 DRT:OMAPS00128836 x0073106
	
	if( ( (rfs_stat(destination_file_uc, &f_stat)) != RFS_ENOENT)//April 27, 2007 DRT:OMAPS00128836 x0073106
		&& (strcmp((const char*)ext2, "jpg") ==0 ))
	{
				return FM_COPY_NONE;
	}

	data->sourceFileID = (T_RFS_FD)rfs_open( source_file_uc, RFS_O_RDONLY, NULL);
	data->destFileID= (T_RFS_FD)rfs_open( destination_file_uc, RFS_O_WRONLY|RFS_O_CREAT, RFS_IRUSR|RFS_IWUSR);
	data->bytesRead = 0;

	if(data->sourceFileSize > FM_COPY_BUFFER_SIZE )
	{
		data->buf_size = FM_COPY_BUFFER_SIZE;
		data->bytesRead = data->buf_size;
	}
	else 
	{
		data->buf_size = data->sourceFileSize;
		data->bytesRead = data->buf_size;
	}

	iResult = (T_RFS_SIZE)rfs_read(data->sourceFileID, data->buf,  data->buf_size );
	TRACE_EVENT_P1("iResult %d",iResult);
	if(iResult < 0)
	{
		//Error Reading
		rfs_close( data->sourceFileID);
		rfs_close(data->destFileID );
		return FM_COPY_ERROR;
	}
	iResult = (T_RFS_SIZE)rfs_write( data->destFileID, data->buf,  data->buf_size);
	TRACE_EVENT_P1("iResult %d",iResult);
	if(iResult < 0)
	{
		//Error writing
		rfs_close( data->sourceFileID);
		rfs_close(data->destFileID );
		return FM_COPY_ERROR;
	}
	if(data->bytesRead < data->sourceFileSize)
	{							
		return FM_COPY_PROGRESS;
	}
	else
	{
		rfs_close( data->sourceFileID);
		rfs_close(data->destFileID );
		return FM_COPY_DONE;
	}

//	return FM_COPY_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_copy_continue

 $Description:		resumes copying the give file

 $Returns:		FM_COPY_ERROR/FM_COPY_DONE/FM_COPY_PROGRESS

 $Arguments:		data		: Copy/Move operation related structure
*******************************************************************************/
T_FM_COPY_STATE mfw_fm_copy_continue(T_MFW_FM_COPYMOVE_STRUCT *data)	
{
	T_RFS_SIZE iResult = 0;

/*	
	UINT16 source_file_uc[FM_MAX_DIR_PATH_LENGTH];
	UINT16 destination_file_uc[FM_MAX_DIR_PATH_LENGTH];
	char source_file[FM_MAX_DIR_PATH_LENGTH];
	char destination_file[FM_MAX_DIR_PATH_LENGTH];
	
	TRACE_FUNCTION("mfw_fm_copy_continue");


	switch (data->source_type)
	{
		case FM_NOR_FLASH:
			strcpy(source_file,"/FFS");
			break;
		case FM_NORMS_FLASH:
			strcpy(source_file,"/NOR");
			break;			
		case FM_NAND_FLASH:
			strcpy(source_file,"/NAND");
			break;
		case FM_T_FLASH:
			strcpy(source_file,"/MMC");
			break;
	}

	switch (data->destination_type)
	{
		case FM_NOR_FLASH:
			strcpy(destination_file,"/FFS");
			break;
		case FM_NORMS_FLASH:
			strcpy(destination_file,"/NOR");
			break;			
		case FM_NAND_FLASH:
			strcpy(destination_file,"/NAND");
			break;
		case FM_T_FLASH:
			strcpy(destination_file,"/MMC");
			break;
	}

	strcat(source_file, data->sourceFile);
	strcat(destination_file, data->destinationFile);

	convert_u8_to_unicode(source_file, source_file_uc);
	convert_u8_to_unicode(destination_file, destination_file_uc);

*/
	if( (data->bytesRead+ FM_COPY_BUFFER_SIZE) < data->sourceFileSize)
	{
		//More blocks to be read
		data->buf_size = FM_COPY_BUFFER_SIZE;
		data->bytesRead = data->bytesRead + data->buf_size;
	}
	else 
	{
		//Last chunk to be read
		data->buf_size = data->sourceFileSize -  data->bytesRead;
		data->bytesRead = data->bytesRead + FM_COPY_BUFFER_SIZE;
	}

	iResult = (T_RFS_SIZE)rfs_read(data->sourceFileID, data->buf,  data->buf_size );
	TRACE_EVENT_P1("iResult %d",iResult);
	if(iResult < 0)
	{
		//Error Reading
		rfs_close( data->sourceFileID);
		rfs_close(data->destFileID );
		return FM_COPY_ERROR;
	}
	iResult = (T_RFS_SIZE)rfs_write( data->destFileID, data->buf,  data->buf_size);
	TRACE_EVENT_P1("iResult %d",iResult);
	if(iResult < 0)
	{
		//Error Writing
		rfs_close( data->sourceFileID);
		rfs_close(data->destFileID );
		return FM_COPY_ERROR;
	}
						
	if(data->bytesRead < data->sourceFileSize)
	{							
		return FM_COPY_PROGRESS;
	}
	else
	{
		rfs_close(data->sourceFileID);
		rfs_close(data->destFileID );
		return FM_COPY_DONE;
	}
//	return FM_COPY_ERROR;
}

//Image List
/*******************************************************************************
 $Function:		mfw_fm_checkThumbnail

 $Description:		Checks for thumbnail

 $Returns:		FM_FILE_DOESNOTEXISTS/FM_NO_ERROR

 $Arguments:		obj				: Current image
 				source			: Current Drive
*******************************************************************************/
T_MFW_FM_STATUS mfw_fm_checkThumbnail( char * obj,T_FM_DEVICE_TYPE  source)
{

	char filename[FM_MAX_DIR_PATH_LENGTH];
#ifdef FF_MMI_RFS_ENABLED
	T_RFS_FD	fd                   = 0;
	UINT16 filename_uc[FM_MAX_DIR_PATH_LENGTH];
#else
	T_FFS_FD  	fd                   = 0;
#endif	
	TRACE_FUNCTION("mfw_fm_check_thumbnail");
	
	TRACE_EVENT_P1("File %s",obj);

#ifdef FF_MMI_RFS_ENABLED	
	memset(filename, 0, FM_MAX_DIR_PATH_LENGTH);
	switch(source)
	{
		case FM_NOR_FLASH:
			sprintf(filename,"%s%s%s/%s.tmb","/FFS",NORDIR,THUMB,obj);
			break;
		case FM_NORMS_FLASH:
			sprintf(filename,"%s%s%s/%s.tmb","/NOR",NORMSDIR,THUMB,obj);
			break;			
		case FM_NAND_FLASH:
			sprintf(filename,"%s%s%s/%s.tmb","/NAND",NANDDIR,THUMB,obj);
			break;
		case FM_T_FLASH:
			sprintf(filename,"%s%s%s/%s.tmb","/MMC",TFLASHDIR,THUMB,obj);
			break;
	}
	convert_u8_to_unicode(filename, filename_uc);
	if ((fd = rfs_open (filename_uc, RFS_O_RDONLY, NULL)) == RFS_ENOENT)
	{
		TRACE_EVENT("Create the thmbnail.");
		return FM_FILE_DOESNOTEXISTS;
	}
	else 
	{
		TRACE_EVENT_P1("Error %d",fd);
		rfs_close(fd);
		return FM_FILE_EXISTS;
	}
#else
			sprintf(filename,"%s%s/%s.tmb",NORDIR,THUMB,obj);
			/**********************NOTE***************************
			FFS API To be replaced with FS Abstraction API 
			******************************************************/
			if ((fd = ffs_open (filename, FFS_O_RDONLY)) <= EFFS_NOTFOUND)
			{
				TRACE_EVENT("Create the thmbnail.");
				return FM_FILE_DOESNOTEXISTS;
			}
			else 
			{
				TRACE_EVENT_P1("Error %d",fd);
				/**********************NOTE***************************
				FFS API To be replaced with FS Abstraction API 
				******************************************************/
				ffs_close(fd);
				return FM_FILE_EXISTS;
			}
#endif
//	return FM_FILE_DOESNOTEXISTS;
}
/*******************************************************************************
 $Function:		mfw_fm_imgViewer_create

 $Description:		This function will create the handle of imageviewer UCP

 $Returns:		Result of the UCP creation

 $Arguments:		None
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgViewer_create(void)
{
	MSL_IMGVIEW_STATUS mslResult       =  MSL_IMGVIEW_STATUS_OK;

	TRACE_FUNCTION ("mfw_fm_imgViewer_create()");

	mslResult =     MSL_ImgView_Create(&msl_handle);
	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_imgViewer_init

 $Description:		This function will initialise the imageviewer UCP

 $Returns:		Result of the UCP initialization

 $Arguments:		None
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgViewer_init(void)
{
	MSL_IMGVIEW_STATUS mslResult       =  MSL_IMGVIEW_STATUS_OK;

	TRACE_FUNCTION ("mfw_fm_imgViewer_init()");

	mslResult =     MSL_ImgView_Init(msl_handle);

	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_imgViewer_destroy

 $Description:		This function destroy the imageviewer UCP

 $Returns:		Result of the UCP destroy

 $Arguments:		None
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgViewer_destroy(void)
{
	MSL_IMGVIEW_STATUS mslResult       =  MSL_IMGVIEW_STATUS_OK;

	TRACE_FUNCTION ("mfw_fm_imgViewer_destroy()");
	mslResult =     MSL_ImgView_Destroy(msl_handle);
	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_imgViewer_deinit

 $Description:		This function will deinit the imageviewer UCP

 $Returns:		Result of the UCP de-initialization

 $Arguments:		None
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgViewer_deinit(void)
{
	MSL_IMGVIEW_STATUS mslResult       =  MSL_IMGVIEW_STATUS_OK;

	TRACE_FUNCTION ("mfw_fm_imgViewer_deinit()");

	mslResult =     MSL_ImgView_Deinit(msl_handle);
	
	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;
}


/*******************************************************************************
 $Function:		mfw_fm_imgViewer_pause

 $Description:		This function will pause the imageviewer UCP. 

 $Returns:		Result of the UCP pause

 $Arguments:		None
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgViewer_pause(void)
{
	MSL_IMGVIEW_STATUS mslResult       =  MSL_IMGVIEW_STATUS_OK;

	TRACE_FUNCTION ("mfw_fm_imgViewer_pause()");
	 mslResult = MSL_ImgView_Pause((MSL_HANDLE)msl_handle);

	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;
}


/*******************************************************************************
 $Function:		mfw_fm_imgViewer_view

 $Description:		This function will start the rendering of the image on LCD.

 $Returns:		Result of the drawing the image

 $Arguments:		None
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgViewer_view(void)
{
	MSL_IMGVIEW_STATUS mslResult       =  MSL_IMGVIEW_STATUS_OK;

	TRACE_FUNCTION ("mfw_fm_imgViewer_view()");
	mslResult = MSL_ImgView_View((MSL_HANDLE)msl_handle);

	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;}
/*******************************************************************************
 $Function:		mfw_fm_imgViewer_setImageProperty

 $Description:		This function will set the parameters for the imageviewer UCP

 $Returns:		Result of the UCP updation of configurations

 $Arguments:		filepath	: Image path
 				filename	: Image name
 				width	: Image width
 				height	: Image height
 				xoffset	: Image x offset
 				yoffset	: Image y offset
 				source	: Current drive
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgViewer_setImageProperty( char * filepath, char *filename, int width, int height,int xoffset, int yoffset, T_FM_DEVICE_TYPE  source)
{

	MSL_IMGVIEW_STATUS  mslResult;
	//The fullpath will be included.
	char tmb_name_p[FM_MAX_OBJ_NAME_LENGTH];
	MSL_DISPLAY_CONFIGTYPE mfw_fm_dspl_config;
	MSL_FILE_CONFIGTYPE mfw_fm_file_config ;
	int rotation_degree = 0;
	int zoom_factor = 0;

	TRACE_FUNCTION ("mfw_fm_imgViewer_setImageProperty()");
    
	mfw_fm_dspl_config.tImgFormat = MSL_COLOR_RGB565;
	mfw_fm_dspl_config.unDisplayImgHeight = height;
	mfw_fm_dspl_config.unDisplayImgWidth = width;
	mfw_fm_dspl_config.unDisplayXOffset = xoffset;
	mfw_fm_dspl_config.unDisplayYOffset = yoffset;
	mslResult = MSL_ImgView_SetConfig(msl_handle,MSL_DISPLAY_CONFIGINDEX,&mfw_fm_dspl_config);
	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		mslResult = MSL_ImgView_SetConfig (msl_handle,MSL_ROTATE_CONFIGINDEX,&rotation_degree);
	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		mslResult = MSL_ImgView_SetConfig(msl_handle,MSL_ZOOM_CONFIGINDEX,&zoom_factor);
       mfw_fm_file_config.sFileName = (char *) mfwAlloc(FM_MAX_OBJ_NAME_LENGTH);

#ifdef FF_MMI_RFS_ENABLED	   
	switch (source)
	{
		case FM_NOR_FLASH:
			sprintf(tmb_name_p,"%s%s%s/%s.tmb","/FFS",NORDIR,THUMB, filename);
	//		mfw_fm_file_config.tFileType = MSL_FILETYPE_FFS;
			break;
		case FM_NORMS_FLASH:
			sprintf(tmb_name_p,"%s%s%s/%s.tmb","/NOR",NORMSDIR,THUMB, filename);
	//		mfw_fm_file_config.tFileType = MSL_FILETYPE_FFS;
			break;
		case FM_NAND_FLASH:
			sprintf(tmb_name_p,"%s%s%s/%s.tmb","/NAND",NANDDIR,THUMB, filename);
			/**********************NOTE***************************
			Drive type to be updated 
			******************************************************/
	//		mfw_fm_file_config.tFileType = MSL_FILETYPE_RFS;
			break;
		case FM_T_FLASH:
			sprintf(tmb_name_p,"%s%s%s/%s.tmb","/MMC",TFLASHDIR,THUMB, filename);
			/**********************NOTE***************************
			Drive type to be updated 
			******************************************************/
			//mfw_fm_file_config.tFileType = MSL_FILETYPE_TFLASH;
			break;
		default:
			sprintf(tmb_name_p,"/%s/%s.tmb",THUMB, filename);
	}
	strcat(mfw_fm_file_config.sFileName,tmb_name_p);
#else	
	strcpy(mfw_fm_file_config.sFileName,tmb_name_p);
#endif
	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		mslResult =  MSL_ImgView_SetConfig(msl_handle,MSL_DECFILE_CONFIGINDEX,&mfw_fm_file_config);
	 mfwFree((U8 *)mfw_fm_file_config.sFileName,FM_MAX_OBJ_NAME_LENGTH);
	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;

}

/*******************************************************************************
 $Function:		mfw_fm_imgViewer_setConfigs

 $Description:		This function will set the parameters for the imageviewer UCP

 $Returns:		Result of the UCP updation of configurations

 $Arguments:		filepath			: Image path
 				filename			: Image name
 				width			: Image width
 				height			: Image height
 				xoffset			: Image x offset
 				yoffset			: Image y offset
 				zFactor			: Zoom factor
 				rotation_degree	: Rotate factor
 				source			: Current drive
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgViewer_setConfigs(char *filename,int width, int height,int xoffset, int yoffset,U32 zFactor,int rotation_degree,T_FM_DEVICE_TYPE  source)
{
	MSL_IMGVIEW_STATUS  mslResult;
	MSL_DISPLAY_CONFIGTYPE mfw_fm_dspl_config;
	MSL_FILE_CONFIGTYPE mfw_fm_file_config ;
	int zoom_factor = zFactor;
	int rAngle = rotation_degree;

	TRACE_FUNCTION ("mfw_fm_imgViewer_setRotationConfigs()");

	mfw_fm_dspl_config.unDisplayImgHeight = height;
	mfw_fm_dspl_config.unDisplayImgWidth = width;
	mfw_fm_dspl_config.unDisplayXOffset = xoffset;
	mfw_fm_dspl_config.unDisplayYOffset = yoffset;
	mslResult = MSL_ImgView_SetConfig(msl_handle,MSL_DISPLAY_CONFIGINDEX,&mfw_fm_dspl_config);

	mfw_fm_file_config.sFileName = (char *) mfwAlloc(FM_MAX_OBJ_NAME_LENGTH);

#ifdef FF_MMI_RFS_ENABLED	
	switch (source)
	{
		case FM_NOR_FLASH:
			strcpy(mfw_fm_file_config.sFileName,"/FFS");
		//	mfw_fm_file_config.tFileType = MSL_FILETYPE_FFS;
			break;
		case FM_NORMS_FLASH:
			strcpy(mfw_fm_file_config.sFileName,"/NOR");
		//	mfw_fm_file_config.tFileType = MSL_FILETYPE_FFS;
			break;		
		case FM_NAND_FLASH:
			strcpy(mfw_fm_file_config.sFileName,"/NAND");
			/**********************NOTE***************************
			Drive type to be updated 
			******************************************************/
		//	mfw_fm_file_config.tFileType = MSL_FILETYPE_RFS;
			break;
		case FM_T_FLASH:
			strcpy(mfw_fm_file_config.sFileName,"/MMC");
			/**********************NOTE***************************
			Drive type to be updated 
			******************************************************/
			//mfw_fm_file_config.tFileType = MSL_FILETYPE_TFLASH;
			break;
	}
	strcat(mfw_fm_file_config.sFileName,filename);
#else
	strcpy(mfw_fm_file_config.sFileName,filename);
#endif
	
	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		 mslResult =  MSL_ImgView_SetConfig(msl_handle,MSL_DECFILE_CONFIGINDEX,&mfw_fm_file_config);

	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		mslResult = MSL_ImgView_SetConfig(msl_handle,MSL_ZOOM_CONFIGINDEX,&zoom_factor);

	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		mslResult = MSL_ImgView_SetConfig (msl_handle,MSL_ROTATE_CONFIGINDEX,&rAngle);

	 mfwFree((U8 *)mfw_fm_file_config.sFileName,FM_MAX_OBJ_NAME_LENGTH);

	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;
}

/*******************************************************************************
 $Function:		mfw_imgView_setcallback

 $Description:		This function will set the callback for viewer UCP

 $Returns:		Result of the updating the callback function

 $Arguments:		None
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgViewer_setcallback(void)
{
	MSL_IMGVIEW_STATUS  mslResult;

	TRACE_FUNCTION("mfw_fm_imgViewer_setcallback");
	
	mslResult = MSL_ImgView_SetConfig(msl_handle,MSL_CALLBACKSET_CONFIGINDEX,(MSL_CALLBACK)Msl_mslil_Callback);
	if(mslResult == MSL_IMGVIEW_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_imgThmb_create

 $Description:		This function will create the handle of thumbnail UCP

 $Returns:		Result of the UCP create

 $Arguments:		None
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgThmb_create(void)
{
	MSL_IMGTHMB_STATUS mslResult       =  MSL_IMGTHMB_STATUS_OK;

	TRACE_FUNCTION ("mfw_fm_imgThmb_create()");

	mslResult =  MSL_ImgThmb_Create(&msl_handle);

	if(mslResult == MSL_IMGTHMB_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_imgThmb_init

 $Description:		This function will initialise of thumbnail UCP

 $Returns:		Result of the UCP initialization

 $Arguments:		None
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgThmb_init(void)
{
	MSL_IMGTHMB_STATUS mslResult       =  MSL_IMGTHMB_STATUS_OK;

	TRACE_FUNCTION ("mfw_fm_imgThmb_init()");

	mslResult =  MSL_ImgThmb_Init(msl_handle);
	
	if(mslResult == MSL_IMGTHMB_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_imgThmb_destroy

 $Description:		This function will destroy the thumbnail UCP.

 $Returns:		Result of the UCP destroy

 $Arguments:		None
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgThmb_destroy(void)
{
	MSL_IMGTHMB_STATUS  mslResult;
	
	TRACE_FUNCTION ("mfw_fm_imgThmb_destroy()");
	
	mslResult =   MSL_ImgThmb_Destroy(msl_handle);
	if (mslResult != MSL_IMGTHMB_STATUS_OK)
	{
		MSL_ImgThmb_Destroy(msl_handle);
		msl_handle=0;
		return FM_UCP_ERROR;
	}
	if(mslResult == MSL_IMGTHMB_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_imgThmb_deinit

 $Description:		This function will deinit the thumbnail UCP.

 $Returns:		Result of the UCP de-initialization

 $Arguments:		None
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgThmb_deinit(void)
{
	MSL_IMGTHMB_STATUS  mslResult;
	
	TRACE_FUNCTION ("mfw_fm_imgThmb_deinit()");
	
	mslResult =   MSL_ImgThmb_Deinit(msl_handle);
	
	if(mslResult == MSL_IMGTHMB_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;
}

/*******************************************************************************

 $Function:		mfw_fm_imgThmb_pause

 $Description:		This function will pasue the thumbnail UCP.

 $Returns:		Result of the UCP pause

 $Arguments:		None

*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgThmb_pause(void)
{
	MSL_IMGTHMB_STATUS  mslResult;
	
	TRACE_FUNCTION ("mfw_fm_imgThmb_pause()");
	mslResult = MSL_ImgThmb_Pause((MSL_HANDLE)msl_handle);
	
	if(mslResult == MSL_IMGTHMB_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_imgThmb_generate

 $Description:		This function will generate the thumbnail file in filesystem.

 $Returns:		Result of the thumbanil generation

 $Arguments:		None
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgThmb_generate(void)
{
	MSL_IMGTHMB_STATUS  mslResult;
	
	TRACE_FUNCTION ("mfw_fm_imgThmb_generate()");

	mslResult = MSL_ImgThmb_Generate((MSL_HANDLE)msl_handle);
	if(mslResult == MSL_IMGTHMB_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;
}


/*******************************************************************************
 $Function:		mfw_fm_imgThmb_setparams

 $Description:		This function will set the parameters for thumbnail UCP

 $Returns:		Result of the UCP updation of configurations

 $Arguments:		inFilepath			: Image path
 				inFilename			: Image name
 				width			: Image width
 				height			: Image height
 				source			: Current drive
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgThmb_setparams(char * inFilepath, char* inFilename, int width,int height, T_FM_DEVICE_TYPE  source)
{
	MSL_IMGTHMB_STATUS mslResult       =  MSL_IMGTHMB_STATUS_OK;
	char tmb_name_p[FM_MAX_DIR_PATH_LENGTH];

	MSL_RESCALE_CONFIGTYPE mfw_fm_rescaleconfig ;
	MSL_FILE_CONFIGTYPE mfw_fm_file_config ;

	TRACE_FUNCTION ("mfw_fm_imgThmb_setparams()");

	mfw_fm_rescaleconfig.unRescaledImgHeight = height;
	mfw_fm_rescaleconfig.unRescaledImgWidth =width ;
	mslResult =  MSL_ImgThmb_SetConfigs(msl_handle,MSL_RESCALE_CONFIGINDEX,&mfw_fm_rescaleconfig);
	mfw_fm_file_config.sFileName = (char *) mfwAlloc(FM_MAX_DIR_PATH_LENGTH);
	switch (source)
	{
		case FM_NOR_FLASH:
			sprintf(mfw_fm_file_config.sFileName,"%s%s%s.jpg","/FFS",inFilepath,inFilename);
			sprintf(tmb_name_p,"%s%s%s/%s.tmb","/FFS",NORDIR,THUMB, inFilename);
		//	mfw_fm_file_config.tFileType = MSL_FILETYPE_FFS;
			break;
		case FM_NORMS_FLASH:
			sprintf(mfw_fm_file_config.sFileName,"%s%s%s.jpg","/NOR",inFilepath,inFilename);
			sprintf(tmb_name_p,"%s%s%s/%s.tmb","/NOR",NORMSDIR,THUMB, inFilename);
		//	mfw_fm_file_config.tFileType = MSL_FILETYPE_FFS;
			break;		
		case FM_NAND_FLASH:
			/**********************NOTE***************************
			Drive type to be updated 
			******************************************************/
			sprintf(mfw_fm_file_config.sFileName,"%s%s%s.jpg","/NAND",inFilepath,inFilename);
			sprintf(tmb_name_p,"%s%s%s/%s.tmb","/NAND",NANDDIR,THUMB, inFilename);
			mfw_fm_file_config.tFileType = MSL_FILETYPE_RFS;
			break;
		case FM_T_FLASH:
			/**********************NOTE***************************
			Drive type to be updated 
			******************************************************/
			sprintf(mfw_fm_file_config.sFileName,"%s%s%s.jpg","/MMC",inFilepath,inFilename);
			sprintf(tmb_name_p,"%s%s%s/%s.tmb","/MMC",TFLASHDIR,THUMB, inFilename);
			//mfw_fm_file_config.tFileType = MSL_FILETYPE_TFLASH;
			break;
	}
	if(mslResult == MSL_IMGTHMB_STATUS_OK)
	{
		mslResult =  MSL_ImgThmb_SetConfigs(msl_handle,MSL_DECFILE_CONFIGINDEX,&mfw_fm_file_config);
	}
	sprintf(mfw_fm_file_config.sFileName,"%s",tmb_name_p);
	if(mslResult == MSL_IMGTHMB_STATUS_OK)
	{
		mslResult =  MSL_ImgThmb_SetConfigs(msl_handle,MSL_ENCFILE_CONFIGINDEX,&mfw_fm_file_config);
	}
	 mfwFree((U8 *)mfw_fm_file_config.sFileName,FM_MAX_DIR_PATH_LENGTH);
	if(mslResult == MSL_IMGTHMB_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR; 
}

/*******************************************************************************
 $Function:		mfw_fm_imgThmb_setcallback

 $Description:		This function will set the callback for thumbnail UCP

 $Returns:		Result of the UCP updation of configurations

 $Arguments:		None
*******************************************************************************/
T_FM_IMG_STATE  mfw_fm_imgThmb_setcallback()
{
	MSL_IMGTHMB_STATUS mslResult       =  MSL_IMGTHMB_STATUS_OK;

	TRACE_FUNCTION("mfw_fm_imgThmb_setcallback");

	mslResult = MSL_ImgThmb_SetConfigs(msl_handle,MSL_CALLBACKSET_CONFIGINDEX,(MSL_CALLBACK)Msl_mslil_Callback);
	if(mslResult == MSL_IMGTHMB_STATUS_OK)
		return FM_UCP_NO_ERROR;
	else
		return FM_UCP_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_thmb_mslcb

 $Description:		This function is the callback for the thumbnail UCP

 $Returns:		None

 $Arguments:		tCMd	: Current command being executed by the MSL
 				tStatus	: Status of the current command
*******************************************************************************/
void mfw_fm_thmb_mslcb( U32 tCMd,U32 tStatus)
{
	T_MFW_FM_PARA para;

	TRACE_FUNCTION("mfw_fm_thmb_mslcb()");
	switch(tCMd)
	{
		case MSL_CMD_INIT:
			TRACE_EVENT_P1("MSL_CMD_INIT %d",tStatus);
			if(tStatus ==MSL_IMGTHMB_STATUS_OK)
			{
				para.img_state=FM_UCP_NO_ERROR;
			}
			else
			{
				para.img_state=FM_UCP_ERROR;
			}
			mfw_fm_signal(E_FM_THMB_INIT, &para);
			break;
		case MSL_CMD_GENERATE:
			TRACE_EVENT_P1("MSL_CMD_GENERATE %d",tStatus);
			switch(tStatus)
			{
				case MSL_IMGTHMB_STATUS_OK:
					para.img_state =   FM_UCP_NO_ERROR;
					break;
				case MSL_IMGTHMB_ERROR_BAD_STREAM:
					para.img_state =   FM_UCP_ERROR_BAD_STREAM;
					break;
				case MSL_IMGTHMB_ERROR_UNKNOWN:
					para.img_state =   FM_UCP_ERROR_UNKNOWN;
					break;
				case MSL_IMGTHMB_ERROR_IOWRITE:
					para.img_state =   FM_UCP_ERROR_NO_MEM;
					break;
				case MSL_IMGTHMB_ERROR_INVALID_ARGUMENT:
					para.img_state =   FM_UCP_ERROR_INVALID_ARGUMENT;
					break;
				default :
					para.img_state =   FM_UCP_ERROR;
			}
			mfw_fm_signal(E_FM_THMB_GEN, &para);
			break;
		case MSL_CMD_DEINIT:
			TRACE_EVENT_P1("MSL_CMD_DEINIT %d",tStatus);
			if(tStatus ==MSL_IMGTHMB_STATUS_OK)
			{
				para.img_state=FM_UCP_NO_ERROR;
			}
			else
			{
				para.img_state=FM_UCP_ERROR;
			}
			mfw_fm_signal(E_FM_THMB_DEINIT, &para);
			break;
	}
}

/*******************************************************************************
 $Function:		mfw_fm_view_mslcb

 $Description:		This function is the callback for the Viewer UCP

 $Returns:		None

 $Arguments:		tCMd	: Current command being executed by the MSL
 				tStatus	: Status of the current command
*******************************************************************************/
void mfw_fm_view_mslcb( U32 tCMd,U32 tStatus)
{
	T_MFW_FM_PARA para;

	TRACE_FUNCTION("mfw_fm_view_mslcb()");

	switch(tCMd)
	{
		case MSL_CMD_INIT:
			TRACE_EVENT_P1("MSL_CMD_INIT %d",tStatus);
			if(tStatus ==MSL_IMGVIEW_STATUS_OK)
			{
				para.img_state=FM_UCP_NO_ERROR;
			}
			else
			{
				para.img_state=FM_UCP_ERROR;
			}
			mfw_fm_signal(E_FM_IMG_INIT, &para);
			break;
		case MSL_CMD_VIEW:
			TRACE_EVENT_P1("MSL_CMD_VIEW %d",tStatus);
			switch(tStatus )
			{
				case MSL_IMGVIEW_STATUS_OK:
					para.img_state = FM_UCP_NO_ERROR;
					break;
				case MSL_IMGVIEW_ERROR_BAD_STREAM:
					para.img_state =  FM_UCP_ERROR_BAD_STREAM;
					break;
				case MSL_IMGVIEW_ERROR_UNKNOWN:
					para.img_state =  FM_UCP_ERROR_UNKNOWN;
					break;
				case MSL_IMGVIEW_ERROR_IOREAD:
					para.img_state =  FM_UCP_ERROR_READ_FAILED;
					break;
				case MSL_IMGVIEW_ERROR_INVALID_ARGUMENT:
					para.img_state =  FM_UCP_ERROR_INVALID_ARGUMENT;
					break;
				default :
					para.img_state =  FM_UCP_ERROR;
			}
			mfw_fm_signal(E_FM_IMG_DRAW,&para);
			break;
		case MSL_CMD_DEINIT:
			TRACE_EVENT_P1("MSL_CMD_DEINIT %d",tStatus);
			if(tStatus ==MSL_IMGVIEW_STATUS_OK)
			{
				para.img_state=FM_UCP_NO_ERROR;
			}
			else
			{
				para.img_state=FM_UCP_ERROR;
			}
			mfw_fm_signal(E_FM_IMG_DEINIT, &para);
			break;
			
	}
}


//Audio List 

/*******************************************************************************
 $Function:		mfw_fm_audPlay_cb

 $Description:		Callback fundtion for auio play/stop

 $Returns:		None

 $Arguments:		parameter	: The structure conveying the stop indication data
*******************************************************************************/
void mfw_fm_audPlay_cb(void *parameter)
{
	T_MFW_FM_PARA para;
	T_AS_STOP_IND *stop_ind = (T_AS_STOP_IND *)parameter;
	
	/* OMAPS00151698, x0056422 */
	T_AS_START_IND *start_ind = ( T_AS_START_IND *)parameter;

	/* OMAPS00151698, x0056422 */

	T_AS_PROBAR_IND_MP3 *mp3_pb = (T_AS_PROBAR_IND_MP3 *)parameter;

	TRACE_FUNCTION("mfw_fm_audPlay_cb");

	
	
	if(mmi_get_aud_state_status() == FM_AUD_NONE)
	{

       TRACE_EVENT_P1("AS_START_IND status %d ",start_ind->status);
      mmi_set_aud_state_status(FM_AUD_PLAY);

				/* OMAPS00151698, x0056422 */
				#ifdef FF_MMI_A2DP_AVRCP

					//if BT connected and aud is initialized, and the command wasnt from BT, then send callback
					tGlobalBmiBtStruct.tAudioState = FM_AUD_PLAY;
					if((tGlobalBmiBtStruct.bConnected != BMI_BT_NOTCONNECTED) 
						&& (start_ind->status == AS_OK))           
					{  


						 if( NULL != tGlobalBmiBtStruct.BmiEventCallback)
						tGlobalBmiBtStruct.BmiEventCallback(BMI_EVENT_USER_PLAY);
						tGlobalBmiBtStruct.tCmdSrc == BMI_BT_COMMAND_NONE;
					}
				#endif // FF_MMI_A2DP_AVRCP
	}

	switch(stop_ind->header.msg_id )
	{
		/* OMAPS00151698, x0056422 */
		 case AS_PAUSE_IND:
                     TRACE_EVENT_P1("AS_PAUSE_IND status %d",stop_ind->status);
                      mmi_set_aud_state_status( FM_AUD_PAUSE);
                      /* OMAPS00151698, x0056422 */
                     #ifdef FF_MMI_A2DP_AVRCP
                     tGlobalBmiBtStruct.tAudioState = FM_AUD_PAUSE;
                    if(tGlobalBmiBtStruct.bConnected != BMI_BT_NOTCONNECTED)
                   {
                             if(NULL != tGlobalBmiBtStruct.BmiEventCallback)
                             tGlobalBmiBtStruct.BmiEventCallback(BMI_EVENT_USER_PAUSE);           
                        tGlobalBmiBtStruct.tCmdSrc = BMI_BT_COMMAND_NONE;
                  }  
                 #endif//FF_MMI_A2DP_AVRCP
                  break;
		/* OMAPS00151698, x0056422 */

		case AS_STOP_IND:
			TRACE_EVENT_P2("AS_STOP_IND status %d end %d",stop_ind->status, stop_ind->end_of_media);
			mfw_unset_stereo_path(mfw_get_current_audioDevice());
			//Daisy tang added for Real Resume feature 20071107
			if(isPlayer_Real_Pause)
			{
				last_file_size_played = stop_ind->file_size;
				last_para_aud_pt = para_aud.aud_pt;
			}
			
			switch((int)(stop_ind->status))
			{
				case AS_NOT_SUPPORTED:
					para.aud_state = FM_AUD_ERROR_NOTSUPPORTED;
					break;
				case AS_MEMORY_ERR:
					para.aud_state = FM_AUD_ERROR_MEM;
					break;
				case AS_INTERNAL_ERR:
					para.aud_state = FM_AUD_ERROR_INTERNAL;
					break;
				case AS_INVALID_PARAM:
					para.aud_state = FM_AUD_ERROR_INVALIDPARAM;
					break;
				case AS_NOT_READY: 
					para.aud_state = FM_AUD_ERROR_NOTREADY;
					break;
				case AS_MESSAGING_ERR:
					para.aud_state = FM_AUD_ERROR_MSGING;
					break;
				case AS_FFS_ERR:
					para.aud_state = FM_AUD_ERROR_FFS;
					break;
				case AS_PLAYER_ERR:  
					para.aud_state = FM_AUD_ERROR_PLAYER;
					break;
				case AS_DENIED:
					para.aud_state = FM_AUD_ERROR_DENIED;
					break;
				case AS_AUDIO_ERR:
					para.aud_state = FM_AUD_ERROR_AUDIO;
					break;
				case AS_NOT_ALLOWED:
					para.aud_state = FM_AUD_ERROR_NOTALLOWED;
					break;
				default:
					para.aud_state = FM_AUD_NONE;
			}
			/* June 28, 2007 DRT: OMAPS00135749 x0062174 */
			mmi_set_aud_state_status( FM_AUD_NONE);

			 /* OMAPS00151698, x0056422 */
                  #ifdef FF_MMI_A2DP_AVRCP
                  tGlobalBmiBtStruct.tAudioState = FM_AUD_NONE;
                  /* x0056422, updated changes from this ID  */ 
				  /*OMAPS001448610 : This callback code is removed since L1 sends the callback to BT for
                                                  stop playback irrespective of whether the stop is due to end of media or is user prompted. */
                    tGlobalBmiBtStruct.tCmdSrc = BMI_BT_COMMAND_NONE;

                  #endif//FF_MMI_A2DP_AVRCP
				  /* OMAPS00151698, x0056422 */
			mfw_fm_signal(E_FM_AUDIO_STOP, &para);
			break;

	case AS_PROBAR_IND:
			TRACE_EVENT_P2("pt %d  tt %d",mp3_pb->u32TotalTimePlayed, mp3_pb->u32totalTimeEst);
			para_aud.aud_pt = mp3_pb->u32TotalTimePlayed + last_para_aud_pt;			//Daisy tang added for Real Resume feature 20071107
			para_aud.aud_tt = mp3_pb->u32totalTimeEst;
			mfw_fm_signal(E_FM_AUDIO_PROG, NULL);
			break;
	}	
	return;

}

/*******************************************************************************
 $Function:		mfw_fm_aud_get_type

 $Description:		Deduces the type of audio file

 $Returns:		audio file type

 $Arguments:		file_name	: audio file 
*******************************************************************************/
T_FM_OBJ_TYPE mfw_fm_aud_get_type(char * file_name)
{
	T_AS_PLAYER_TYPE player_type;
	UINT16 filename_uc[47];
	TRACE_FUNCTION("mfw_fm_aud_get_type");

	convert_u8_to_unicode(file_name, filename_uc);
	as_deduce_player_type(filename_uc,&player_type);
	switch(player_type)
	{
		case AS_PLAYER_TYPE_MIDI:
			TRACE_EVENT("AS_PLAYER_TYPE_MIDI");
			return OBJECT_TYPE_AUDIO_MIDI;
		case AS_PLAYER_TYPE_MP3:
			TRACE_EVENT("AS_PLAYER_TYPE_MP3");
			return OBJECT_TYPE_AUDIO_MP3;
		case AS_PLAYER_TYPE_AAC:
			TRACE_EVENT("AS_PLAYER_TYPE_AAC");
			return OBJECT_TYPE_AUDIO_AAC;
		default:
			TRACE_EVENT("Default");
			return OBJECT_TYPE_FILE;
	}
}
 
/*******************************************************************************
 $Function:		mfw_fm_audPlay

 $Description:		Plays the selected audio file

 $Returns:		None
  
 $Arguments:		file_name	: audio file 
 				file_type		: audio file type
*******************************************************************************/
#ifdef FF_MMI_UNICODE_SUPPORT
T_FM_AUD_STATE mfw_fm_audPlay(T_WCHAR * filename_uc, T_FM_OBJ_TYPE file_type, BOOL play_bar)
{

#else 
T_FM_AUD_STATE mfw_fm_audPlay(char * file_name, T_FM_OBJ_TYPE file_type, BOOL play_bar)
{
	UINT16 filename_uc[FM_MAX_DIR_PATH_LENGTH];
	
	convert_u8_to_unicode(file_name, filename_uc);
#endif
	T_AS_RET status;
	T_AS_PLAYER_TYPE player_type;
    int i=0;

	TRACE_FUNCTION("mfw_fm_audPlay");

	//Daisy tang added for Real Resume feature 20071107
	//start
	#ifdef FF_MMI_UNICODE_SUPPORT
	   for(i==0;i<FM_MAX_DIR_PATH_LENGTH;i++)
	   	{
	   	last_inputFileName_uc[i]=filename_uc[i];
	   	}
	
	#else
	strcpy(last_inputFileName, file_name); 
	#endif
	last_file_type = file_type;
	last_play_bar = play_bar;
	last_file_size_played = 0;
	last_para_aud_pt = 0;
	//end
	switch(file_type)
	{
		case OBJECT_TYPE_AUDIO_MIDI:
			TRACE_EVENT("AS_PLAYER_TYPE_MIDI");
			
			/* Aug 27, 2007    DRT: OMAPS00137370  x0045876 */
			/* Voice Limit parameter should be saved to a temp var before playing 
			   MP3 or AAC file */
			player_para.midi.voice_limit = mfw_player_midi_get_voice_limit();
			/* Apr 10, 2007    DRT: OMAPS00125309  x0039928 */
			/* MONO is 1 and STEREO is 2 for midi player*/
		 	player_para.midi.output_channels = player_channel;	 
			player_type = AS_PLAYER_TYPE_MIDI;
			break;
		case OBJECT_TYPE_AUDIO_MP3:
			TRACE_EVENT("AS_PLAYER_TYPE_MP3");

			/* Aug 27, 2007    DRT: OMAPS00137370  x0045876 */
			/* Voice Limit parameter should be saved to a temp var before playing 
			   MP3 or AAC file */
			mfw_player_set_voice_limit (player_para.midi.voice_limit);
			/* Apr 10, 2007    DRT: OMAPS00125309  x0039928 */
			/* MONO is 0 and STEREO is 1 for mp3 player*/
			player_para.mp3.mono_stereo = player_channel - 1;
			player_para.mp3.size_file_start = 0;
			player_type = AS_PLAYER_TYPE_MP3;
			break;
		case OBJECT_TYPE_AUDIO_AAC:
			TRACE_EVENT("AS_PLAYER_TYPE_AAC");

			/* Aug 27, 2007    DRT: OMAPS00137370  x0045876 */
			/* Voice Limit parameter should be saved to a temp var before playing 
			   MP3 or AAC file */
			mfw_player_set_voice_limit (player_para.midi.voice_limit);
			/* Apr 10, 2007    DRT: OMAPS00125309  x0039928 */
			/* MONO is 0 and STEREO is 1 for aac player*/
			player_para.aac.mono_stereo = player_channel - 1 ;
			player_para.aac.size_file_start= 0;
			player_type = AS_PLAYER_TYPE_AAC;
			break;
		default:
			/* Aug 27, 2007    DRT: OMAPS00137370  x0045876 */
			/* Voice Limit parameter should be saved to a temp var before playing 
			   MP3 or AAC file */
			player_para.midi.voice_limit = mfw_player_midi_get_voice_limit();
			/* Apr 10, 2007    DRT: OMAPS00125309  x0039928 */
			/* MONO is 1 and STEREO is 2 for midi player*/
			player_para.midi.output_channels = player_channel;	
			player_type = AS_PLAYER_TYPE_MIDI;
			break;
	}
	as_player_set_params(player_type,&player_para);

	mfw_set_stereo_path(mfw_get_current_audioDevice());
	fm_aud_return_path.addr_id = 0;
	fm_aud_return_path.callback_func = mfw_fm_audPlay_cb;

	status=as_play_file(player_type,filename_uc,AS_VOLUME_HIGH,mfw_player_playback_loop_flag,play_bar,&fm_aud_return_path);
	if(status == AS_OK)
		return FM_AUD_NO_ERROR;
	else
		return FM_AUD_ERROR;
}

/*******************************************************************************
 $Function:		mfw_fm_audStop

 $Description:		Stops playing the selected audio file

 $Returns:		None

 $Arguments:		file_name	: audio file 
 				file_type		: audio file type
*******************************************************************************/
T_FM_AUD_STATE mfw_fm_audStop(void)
{
	T_AS_RET status;
	TRACE_EVENT("mfw_fm_audStop");
	fm_aud_return_path.addr_id = 0;
	fm_aud_return_path.callback_func = mfw_fm_audPlay_cb;
	status=as_stop(&fm_aud_return_path);
	if(status == AS_OK)
		return FM_AUD_NO_ERROR;
	else
		return FM_AUD_ERROR;
}  

T_FM_AUD_STATE mfw_fm_audPause(void)
{
	T_AS_RET status;
	TRACE_EVENT("mfw_fm_audPause");
	fm_aud_return_path.addr_id = 0;
	fm_aud_return_path.callback_func = mfw_fm_audPlay_cb;
	status=as_pause(&fm_aud_return_path);
	if(status == AS_OK)
		return FM_AUD_NO_ERROR;
	else
		return FM_AUD_ERROR;
}  

T_FM_AUD_STATE mfw_fm_audResume(void)
{
	T_AS_RET status;
	TRACE_EVENT("mfw_fm_audResume");
	fm_aud_return_path.addr_id = 0;
	fm_aud_return_path.callback_func = mfw_fm_audPlay_cb;
	 mmi_set_aud_state_status( FM_AUD_PLAY);


	/* OMAPS00151698, x0056422 */
    #ifdef FF_MMI_A2DP_AVRCP
    tGlobalBmiBtStruct.tAudioState = FM_AUD_PLAY;
    if(tGlobalBmiBtStruct.bConnected != BMI_BT_NOTCONNECTED)
    {
            /*OMAPS001448610 : This check for Headset/handset command is to be removed since the handling is now exactly the same. */
            if(NULL != tGlobalBmiBtStruct.BmiEventCallback)
                tGlobalBmiBtStruct.BmiEventCallback(BMI_EVENT_USER_RESUME);           
            tGlobalBmiBtStruct.tCmdSrc = BMI_BT_COMMAND_NONE;

	 }  
	#endif//FF_MMI_A2DP_AVRCP

	/* OMAPS00151698, x0056422 */
	status=as_resume(&fm_aud_return_path);
	if(status == AS_OK)
		return FM_AUD_NO_ERROR;
	else
		return FM_AUD_ERROR;
}  
//Daisy tang added for Real Resume feature 20071107
//start
T_FM_AUD_STATE mfw_fm_audRealPause(void)
{
	T_AS_RET status;
	TRACE_EVENT("mfw_fm_audRealPause");
	isPlayer_Real_Pause = TRUE;

	fm_aud_return_path.addr_id = 0;
	fm_aud_return_path.callback_func = mfw_fm_audPlay_cb;
	status=as_stop(&fm_aud_return_path);
	if(status == AS_OK)
		return FM_AUD_NO_ERROR;
	else
		return FM_AUD_ERROR;
}  

T_FM_AUD_STATE mfw_fm_audRealResume(void)
{	
	T_AS_RET status;
	T_AS_PLAYER_TYPE player_type;
	UINT16 filename_uc[FM_MAX_DIR_PATH_LENGTH];
	int i=0;

   
	TRACE_FUNCTION("mfw_fm_audRealResume");
	
	if(!isPlayer_Real_Pause)
		return FM_AUD_ERROR; 
	if((last_file_type NEQ OBJECT_TYPE_AUDIO_MP3) AND (last_file_type NEQ OBJECT_TYPE_AUDIO_AAC))
		return FM_AUD_ERROR; 
	
	isPlayer_Real_Pause = FALSE;

	switch(last_file_type)
	{
		case OBJECT_TYPE_AUDIO_MP3:
			TRACE_EVENT("AS_PLAYER_TYPE_MP3");

				/* Aug 27, 2007    DRT: OMAPS00137370  x0045876 */
				/* Voice Limit parameter should be saved to a temp var before playing 
				   MP3 or AAC file */
				mfw_player_set_voice_limit (player_para.midi.voice_limit);
				/* Apr 10, 2007    DRT: OMAPS00125309  x0039928 */
				/* MONO is 0 and STEREO is 1 for mp3 player*/
				player_para.mp3.mono_stereo = player_channel - 1;
				player_para.mp3.size_file_start = last_file_size_played;//Daisy tang added for MP3 feature 20071107
				player_type = AS_PLAYER_TYPE_MP3;
				break;
			case OBJECT_TYPE_AUDIO_AAC:
				TRACE_EVENT("AS_PLAYER_TYPE_AAC");

				/* Aug 27, 2007    DRT: OMAPS00137370  x0045876 */
				/* Voice Limit parameter should be saved to a temp var before playing 
				   MP3 or AAC file */
				mfw_player_set_voice_limit (player_para.midi.voice_limit);
				/* Apr 10, 2007    DRT: OMAPS00125309  x0039928 */
				/* MONO is 0 and STEREO is 1 for aac player*/
				player_para.aac.mono_stereo = player_channel - 1 ;
				player_para.aac.size_file_start= last_file_size_played;//Daisy tang added for MP3 feature 20071107

				player_type = AS_PLAYER_TYPE_AAC;
				break;
		}
		as_player_set_params(player_type,&player_para);

		mfw_set_stereo_path(mfw_get_current_audioDevice());
		fm_aud_return_path.addr_id = 0;
		fm_aud_return_path.callback_func = mfw_fm_audPlay_cb;
#ifdef FF_MMI_UNICODE_SUPPORT	
           for(i==0;i<FM_MAX_DIR_PATH_LENGTH;i++)
		   	{
                      filename_uc[i]=last_inputFileName_uc[i];
		   }
#else
		convert_u8_to_unicode(last_inputFileName, filename_uc);
#endif
		status=as_play_file(player_type,filename_uc,AS_VOLUME_HIGH,mfw_player_playback_loop_flag,last_play_bar,&fm_aud_return_path);
		if(status == AS_OK)
			return FM_AUD_NO_ERROR;
		else
			return FM_AUD_ERROR;	
}  
//end


#endif


