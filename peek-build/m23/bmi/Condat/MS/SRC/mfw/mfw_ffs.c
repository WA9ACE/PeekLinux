/*
+--------------------------------------------------------------------+
| PROJECT:	MMI-Framework (8417)		$Workfile::	mfw_ffs.c	    			$|
| $Author::								$Revision::	1			    	$|
| CREATED:			     		$Modtime::								$|
| STATE  :	code														  |
+--------------------------------------------------------------------+

   MODULE  : mfw_ffs

   PURPOSE : This module contains FFS Interface functions.

   HISTORY:

    Feb  19 2007, x0061088(Prachi)  LOCOSTO-DRT-OMAPS00112518
    Description:Can't access Fileviewer when too many pictures has been taken
    (no space in FFS). After taking maximum snapshot(up to out of memory) cant access
    ETM . ETM shows error "Maximum open file reached"
    Solution: As the relience flag is enabled, to solve this issue, function Flash_formatted()
    is now checking the return value of ffs_opendir() to close the mmi/ directory properly.
    Also MMI is calling mfw_close_dir() to close mmi/jpeg/ directory while exiting from camera 
    application.


	Jan 10, 2007 DR: OMAPS00110568 x0039928	
	Description: Remove duplication of FFS.Flashdata from GDI
	Solution: Added two new functions get_ffs_tty_status() and get_ffs_ttypftype()
	
    	Nov 07, 2006 ER: OMAPS00102732 x0pleela
	Description: FFS Support for Intel Sibley Flash - Intel 256+64 non-ADMUX (PF38F4050M0Y0C0Q)
	Solution: Closing the opened directory if readdir is not successful and the new code 
   			is under the compilation flag FF_MMI_RELIANCE_FFS

   	Sep 08, 2006 DR: OMAPS00091250 x0pleela
 	Description:The phone resets if no MEPD data is presented
 	Solution: Added new function ffs_flash_write() which writes a string into FFS 	

    xrashmic 22 Aug, 2004 MMI-SPR-32798
    Adding the support for screen capture using a dynamically assigned key.

    Apr 14, 2005	REF: CRR 29991   xpradipg
    Description:	Optimisation 5: Remove the static allocation and use dynamic 
    			allocation/ deallocation for pb_list and black_list
    Solution:	The static definition is removed and replaced with the dynamic
    			allocation   

    Oct 19, 2004 REF: CRR MMI-SPR-26002 xkundadu 
    Issue description:  Golite booting problem.
    Solution: If the FFS is not formatted, prevent writing into FFS.

    Aug 25, 2004  REF: CRR 20655  xnkulkar
    Description: Voice Memo functionality not working
    Solution:	  The voice recording functionality was failing because 
    			 " mmi" folder is not present. As a solution, we create the 
    			 "mmi" folder and then proceed with recording.
*/

#define ENTITY_MFW

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

#include "prim.h"

#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#include "mfw_sys.h"
#include "prim.h"

#include "vsi.h"

#include "mfw_mfw.h"
#include "mfw_win.h"

#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
//TISH modified for MSIM
//#ifdef WIN32
//#include "MmiMain.h"
//#endif
#include "mfw_lng.h"
#include "mfw_tim.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_phb.h"
#include "mfw_cm.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_phb.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_sat.h"

#include "mfw_ffs.h"

#include "dspl.h"

#include "gdi.h" //ES!!
#ifdef NEPTUNE_BOARD
#include "ffs.h"
#endif
#define hCommACI _ENTITY_PREFIXED(hCommACI)

#if( BOARD == 61 )
#ifdef PCM_2_FFS
#define T_PSPDF_SMS 20
#define MAX_NO_SMS 20
#endif
#endif

//	Apr 14, 2005	REF: CRR 29991   xpradipg 
//	New file added for the blacklist. The total length along with the path aand
//	file name should not exceed 20 characters
#ifdef FF_MMI_OPTIM
#define BLACK_LIST_PATH_AND_FILE "/mmi/blacklist"
#endif

#if defined (NEW_FRAME)
EXTERN T_HANDLE hCommACI;
#else
EXTERN T_VSI_CHANDLE hCommACI;
#endif
#ifdef TI_PS_HCOMM_CHANGE
#define _hCommMMI  hCommACI
#else 
#define hCommMMI  hCommACI
#endif

typedef enum
{
  DATA,
  NO_DATA
} WriteFlag;


 FlashData FFS_flashData;

static WriteFlag write_flag;

int flash_formatted(void); /* SPR#2487 - DS - Function to check if FFS is formatted */

//TISH, patch for call control CLIR
//start
UBYTE getCLIRState(void)
{
	if (FFS_flashData.CLIR)
		TRACE_EVENT("FFS_flashData.CLIR EQ true");
	else
		TRACE_EVENT("FFS_flashData.CLIR EQ false");
	return FFS_flashData.CLIR;
}
//end
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_FFS            |
| STATE   : code                        ROUTINE : mfw_flash_write     |
+--------------------------------------------------------------------+

  PURPOSE : To write a string into FFS.

*/
//x0pleela 08 Sep, 2006  DR: OMAPS00091250
void mfw_flash_write(char *Msg,...)
{  
	fd_t fd= -1;   
	char log_ffs_buf[256];	
	va_list varpars;
	va_start (varpars, Msg);      
	vsprintf(log_ffs_buf,Msg,varpars);
	va_end (varpars);   
	
	fd = ffs_open((const char*)"/mmiMEPD", FFS_O_APPEND | FFS_O_WRONLY );
	if(fd < 0) 
		fd=ffs_open((const char*)"/mmiMEPD", FFS_O_CREATE |FFS_O_WRONLY );
	if(fd > 0)
	{
		ffs_write(fd, log_ffs_buf, strlen(log_ffs_buf));
		ffs_close(fd);
	}
}

effs_t flash_write(void)
{ 

//  Oct 19, 2004 REF: CRR MMI-SPR-26002 xkundadu 
//  Issue description:  Golite booting problem.
//  Solution: If the FFS is not formatted, dont write into the FFS.
               
	if ( flash_formatted() == TRUE) 
	{
  		return (flash_data_write("/mmi","mmidata",  &FFS_flashData, sizeof(FFS_flashData)));
	}
	else 
	{
		TRACE_EVENT("ERROR flash_data_write(): NOT FORMATTED!");
		// Return NOT formatted message.
        	return EFFS_NOFORMAT;
	}
}

effs_t flash_update(void)
{
  if (write_flag == DATA)
  { write_flag = NO_DATA;
    return (flash_write());
  }
  else
    return EFFS_OK;
}
//TISH modified for MSIM
#ifndef _WIN32_
// EF 22/10/02	for WIN32 simulation, set anykeyanswer on, and keylock off
/* Marcus: Issue 1719: 11/02/2003: Changed return value from effs_t to int */
int flash_read(void)
{

#ifdef PCM_2_FFS
    int val = ffs_fread("/mmi/mmidata",  &FFS_flashData, sizeof(FFS_flashData));
#else
    int val = FFS_fread("/mmi/mmidata",  &FFS_flashData, sizeof(FFS_flashData));
#endif

    if (val == sizeof(FFS_flashData))
    {
        val = EFFS_OK;  /* Successful, return Zero as no data_size as in flash_data_read*/
    }
    else
    {
        TRACE_ERROR("The FFS data could not be read!");
        TRACE_EVENT_P1("val = %d", val);
        /* Error, presumably -ve as in FFS_ERRORS, or the actual number of bytes read */
    }
    return val; 	
}
#else
int flash_read()
{FFS_flashData.settings_status|= SettingsAnyKeyAnswerOn;
FFS_flashData.settings_status &= ~SettingsKeyPadLockOn;
#ifdef TI_PS_FF_CONC_SMS
FFS_flashData.ConcatenateStatus = TRUE;
#else
FFS_flashData.ConcatenateStatus = FALSE;
#endif /*TI_PS_FF_CONC_SMS*/

}
#endif

effs_t flash_data_write(const char* dir_name, const char* file_name, void* data_pointer, int data_size)
{ 
    T_FFS_DIR dir;
    int val;
    char file[200];

    /* SPR#2487 - DS - If FFS is not formatted, return from function */
    if (flash_formatted() == FALSE)
    {
        TRACE_EVENT("ERROR flash_data_write(): NOT FORMATTED!");
        return EFFS_NOFORMAT;
    }

#ifndef PCM_2_FFS   
    val = FFS_opendir(dir_name, &dir);
#else
    val = ffs_opendir((const signed char *)dir_name, &dir);
#endif

    if(val < 0)
    {
    
#ifndef PCM_2_FFS    
        val = FFS_mkdir(dir_name);
#else
	val = ffs_mkdir(dir_name);
#endif

        switch(val)
        {
        case EFFS_OK:
        case EFFS_EXISTS:
            break;
        default:
                TRACE_ERROR("The FFS directory could not be created!");
              	TRACE_EVENT_P2("val = %d dir name = %s", val, dir_name);
            return (effs_t)val; 	
        }
    }

    sprintf(file, "%s/%s", dir_name, file_name);
	
#ifdef PCM_2_FFS	
    val = ffs_fwrite( (const S8 *) file, data_pointer, data_size);
#else
    val = FFS_fwrite( file, data_pointer, data_size);
#endif

    if(val NEQ EFFS_OK)
    {  
        TRACE_ERROR("The FFS file could not be created");
      	TRACE_EVENT_P2("val = %d file name = %s", val, file);
    }  


//x0pleela 07 Nov, 2006 DVT: OMAPS00102732
//Adding the MMI compilation flag FF_MMI_RELIANCE_FFS
#if defined ( PCM_2_FFS    ) || defined( FF_MMI_RELIANCE_FFS)
    ffs_closedir( &dir );
#endif
    
    return (effs_t)val;
}

/* Marcus: Issue 1719: 11/02/2003: Changed return value from effs_t to int */
int flash_data_read(const char* dir_name, const char* file_name, void* data_pointer, int data_size)
{ 
    int val;
    char file[200];
      
    sprintf(file, "%s/%s", dir_name, file_name);

#ifdef PCM_2_FFS
    val = ffs_fread((const S8 *) file, data_pointer, data_size);
#else
    val = FFS_fread( file, data_pointer, data_size);
#endif
    
    if (val NEQ data_size)
    {
        TRACE_ERROR("The FFS data could not be read!");
        TRACE_EVENT_P1("val = %d", val);
        /* Error, presumably -ve as in FFS_ERRORS, or the actual number of bytes read */
    }
    return val;
}

/* SPR#2487 - DS - Function to check if FFS is formatted */
int flash_formatted(void)
{
    T_FFS_DIR dir;
    int val;
   char sFileName[100];

    /* Attempt to open "/mmi" directory */
#ifndef PCM_2_FFS    
    val = FFS_opendir("/mmi", &dir);
#else
	val = ffs_opendir("/mmi", &dir);
#endif	
    

    /* If return val is not EFFS_NOFORMAT then 
     * assume that FFS is formatted.
     */
    if (val == EFFS_NOFORMAT)
        return FALSE;
    else
    {
        //Feb  19 2007, x0061088(Prachi)  LOCOSTO-DRT-OMAPS00112518
        if (val >= 0)
        {
            while (ffs_readdir (&dir, sFileName, 100) > 0x0 );

	//x0pleela 07 Nov, 2006 DVT: OMAPS00102732
	//Adding the MMI compilation flag FF_MMI_RELIANCE_FFS			
		//x0pleela 31 Oct, 2006  
		//Closing the directory aa per the new FFS logic
#ifdef FF_MMI_RELIANCE_FFS		
		ffs_closedir(&dir);
#endif
            	return TRUE;
           
        }
        else if (EFFS_NOTFOUND == val) 
        {                                               
            /* if the mmi directory was not found, but the FFS was formatted, */
            /* create the mmi dir.*/

#ifndef PCM_2_FFS    
            val = FFS_mkdir("/mmi");
#else
            val = ffs_mkdir("/mmi");
#endif
            switch(val)
            {
                case EFFS_OK:
                case EFFS_EXISTS:
                    break;
                default:
                    TRACE_ERROR("The /mmi directory could not be created!");
                    return FALSE;
            }
        }
    }
        return TRUE;
}
    
/*
Aug 25, 2004  REF: CRR 20655  xnkulkar

+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_FFS		                               |
| STATE  : code 			ROUTINE: flash_makedir                                             |
+--------------------------------------------------------------------+

   PURPOSE :  Creates a directory if not present
*/
void flash_makedir(char * dir_name)
{
	T_FFS_DIR dir;
#ifndef PCM_2_FFS    
	int   val = FFS_opendir(dir_name , &dir);
#else
	int   val = ffs_opendir(dir_name , &dir);
#endif


	if(val < 0)
	{
   
#ifndef PCM_2_FFS    
		val = FFS_mkdir(dir_name);
#else
		val = ffs_mkdir(dir_name);
#endif
		switch(val)
		{
			case EFFS_OK:
			case EFFS_EXISTS:
				break;
			default:
				TRACE_ERROR("The FFS directory could not be created!");
				TRACE_EVENT_P2("val = %d dir name = %s", val, dir_name);
				return; 	
		}
	}
	
//x0pleela 07 Nov, 2006 DVT: OMAPS00102732
	//Adding the MMI compilation flag FF_MMI_RELIANCE_FFS
#if defined( PCM_2_FFS) || defined (FF_MMI_RELIANCE_FFS)
    ffs_closedir( &dir );
#endif

}	

#ifdef MMI_EM_ENABLED
#ifndef NEPTUNE_BOARD
/* This portion is not valid for Neptune, hence excluding it.*/
U32 get_screen_size(void);
void get_screen_capture(U8 *dest_data);
U32 screen_capture_index=0;
/*******************************************************************************

 $Function:		screen_capture_write

 $Description:	Implemented By Manju. Write the file into FFS
                     xrashmic 22 Aug, 2004 MMI-SPR-32798
 $Returns:		none

 $Arguments:	none

*******************************************************************************/
void screen_capture_write(U8 *filename, U8 *buffer, S32 size)
{
	fd_t fd= -1;
	fd = ffs_open((const char*)filename, FFS_O_CREATE | FFS_O_WRONLY);
	ffs_write(fd, buffer, size);
	ffs_close(fd);
}

/*******************************************************************************

 $Function:		screen_capture

 $Description:	Implemented By Manju. Writes the LCD buffer into a file in FFS.
                     xrashmic 22 Aug, 2004 MMI-SPR-32798
 $Returns:		none

 $Arguments:	none

*******************************************************************************/
void screen_capture(void)
{
	U8 *dest_data;
	U8 filename[16];
	U32 dest_size;
        dest_size=get_screen_size();
        dest_data=(U8*)malloc(dest_size);
	memset(dest_data, 0, 320*240*4);
	get_screen_capture(dest_data);
	sprintf((char*)filename,(const char*)"/sc%04d",screen_capture_index);
	screen_capture_write(filename, dest_data, dest_size);
        free(dest_data);//, dest_size);
	screen_capture_index++;
}
#endif /* ifndef NEPTUNE_BOARD*/
#endif


// Apr 14, 2005	REF: CRR 29991   xpradipg 
#ifdef FF_MMI_OPTIM
/*******************************************************************************

 $Function:		flash_MMI_blackList_open

 $Description:	This opens the blacklist file
 
 $Returns:		value of the open result

 $Arguments:	none

*******************************************************************************/
int8 flash_MMI_blackList_open( )
{
	T_FFS_FD file=EFFS_NOFORMAT;	
	/*a0393213 compiler warnings removal - variable status removed*/
	if(flash_formatted())
	{
		file = FFS_open(BLACK_LIST_PATH_AND_FILE, FFS_O_RDWR);
		if(file == EFFS_NOTFOUND)
		{
			TRACE_FUNCTION("the file does not exist and is created");
			file = FFS_open(BLACK_LIST_PATH_AND_FILE, FFS_O_RDWR | FFS_O_CREATE);
			if(file < 0 ) 
			{
				TRACE_FUNCTION("file creation failed");
				return file;
			}
			return file;
		}
	}
	return file;
}
/*******************************************************************************

 $Function:		flash_MMI_blackList_close

 $Description:	This closes the file opened for read operation
 
 $Returns:		zero for success and -1 for failure

 $Arguments:	handle - handle of the file to be closed

*******************************************************************************/
void flash_MMI_blackList_close(int8 handle)
{
	FFS_close((T_FFS_FD)handle);
}
		
/*******************************************************************************

 $Function:		flash_MMI_blackList_write

 $Description:	This writes the blacklisted numbers onto the blacklist file
 				if the file does not exist it creates and writes the data
 $Returns:		zero for success and -1 for failure

 $Arguments:	data - to be written into the file
 				len	 - length of the data to be written

*******************************************************************************/
int flash_MMI_blackList_write(U8 *data, SHORT len, SHORT offset)
{

	T_FFS_FD file;	
	/*a0393213 compiler warnings removal - variable status removed*/
	TRACE_FUNCTION("flash_MMI_blackList_write()");
	if(flash_formatted())
	{
		file = FFS_open(BLACK_LIST_PATH_AND_FILE, FFS_O_RDWR);

		if(file == EFFS_NOTFOUND)
		{
			TRACE_FUNCTION("the file does not exist and is created");
			file = FFS_open(BLACK_LIST_PATH_AND_FILE, FFS_O_RDWR | FFS_O_CREATE);
			if(file < 0 ) 
			{
				TRACE_FUNCTION("file creation failed");
				return file;
			}
		}
		TRACE_FUNCTION("the file exist and is opened");
#ifdef PCM_2_FFS		
		ffs_seek(file, offset, FFS_SEEK_SET);
		ffs_write(file, (void*)data, len);
		ffs_close(file);
#else
		FFS_seek(file, offset, FFS_SEEK_SET);
		FFS_write(file, (void*)data, len);
		FFS_close(file);
#endif		

		return 0;
	}
	else
		return -1;	
		
}
/*******************************************************************************

 $Function:		flash_MMI_blackList_read

 $Description:	This reads the blacklisted numbers onto the data buffer
 				
 $Returns:		zero for success and -1 for failure

 $Arguments:	data - buffer into which  data is retrieved from the file
 				len	 - length of the data to be retrieved from the file

*******************************************************************************/
int flash_MMI_blackList_read(int8 file, U8* data, SHORT len,SHORT offset)
{
	int status;
	TRACE_FUNCTION("file is opened and being read");
	TRACE_EVENT_P1("the file is seekded at %d",offset);
	FFS_seek(file, offset, FFS_SEEK_SET);
	status = FFS_read(file, (void *)data, len);
	return status;
}


#endif

#if( BOARD == 61 )
#ifdef PCM_2_FFS
T_FFS_SIZE ffs_ReadRecord(const char *name, void * addr, int size, int index, int recsize)
{
  int n;
  S32 result=0;
  T_FFS_STAT stat;
  char buf[T_PSPDF_SMS*MAX_NO_SMS];

  ffs_stat((const signed char *)name, &stat);
  n = stat.size / recsize;
  
  if (index > n)
    return EFFS_NOTFOUND;

  if (stat.size <= (T_PSPDF_SMS*MAX_NO_SMS))  
  {
    result = ffs_file_read((const signed char *)name, &buf, stat.size);
    if (result == stat.size) 
    {
      memcpy(addr, &buf[(index-1)*recsize], recsize);
      result = recsize; 
    }
  }

  return result;
}

T_FFS_RET ffs_WriteRecord(const char *name, void * addr, int size, int index, int recsize)
{
  int n;
  int result =0;
  T_FFS_STAT stat;
  char buf[T_PSPDF_SMS*MAX_NO_SMS];

  ffs_stat((const signed char *)name, &stat);
  n = stat.size / recsize;

  if (index > n)
    return EFFS_NOTFOUND;

  if (stat.size <= (T_PSPDF_SMS*MAX_NO_SMS))  
  {
    result = ffs_file_read((const signed char *)name, &buf, stat.size);
    if (result == stat.size) 
    {
      memcpy(&buf[(index-1)*recsize], addr, recsize);
      result = ffs_file_write((const signed char *)name, &buf, stat.size, (FFS_O_CREATE|FFS_O_TRUNC));
    }
  }

  return result;
}
#endif

#endif

#ifdef FF_TTY_HCO_VCO
/*******************************************************************************

 $Function:		get_ffs_tty_status

 $Description:	Gives the status of tty in Flash
 
 $Returns:		tty status

 $Arguments:	None

*******************************************************************************/
uint8 get_ffs_tty_status(void)
{
	return FFS_flashData.ttyAlwaysOn;
}

/*******************************************************************************

 $Function:		get_ffs_tty_pftype

 $Description:	This gives the tty profile type to be loaded
 
 $Returns:		tty profile type

 $Arguments:	None

*******************************************************************************/
uint8 get_ffs_tty_pftype(void)
{
	return FFS_flashData.ttyPfType;
}
#endif
