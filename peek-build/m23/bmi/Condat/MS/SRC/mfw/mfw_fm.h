/* =========================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================== */

/*==========================================================
* @file mfw_FileManager.h
*
* This provides the functionality of File Manager Applications.
* It supports file/folder related operations on FFS and MMC
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
	Apr 06, 2007 DRT: OMAPS00124874 x0039928
	Description: MM: Renaming process of an Imy file is incorrect
	Solution: Seperate enum values for different audio files is added.
	
	Mar 28, 2007   ER: OMAPS00106188  x0039928
	Description: Align Midi application menu choices and audio Player menu choices 

*! 10-Apr-2006 mf:  Modified x0020906.  Change required per SRxxxx
*! to provide File Manager Functionality.
*!
*!
*! 17-Jan-2006 mf: Revisions appear in reverse chronological order;
*! that is, newest first.  The date format is dd-Mon-yyyy.
* =========================================================== */


#ifndef DEF_MFW_FILEMANAGER
#define DEF_MFW_FILEMANAGER
//Includes for Image Services
#include "msl/inc/msl_imgthmb.h"
#include "msl/inc/msl_imgview.h" 
//Includes for Audio Services
#include "as/as_api.h" 
#include "as/as_message.h"
#include "mfw_mfw.h"

/*******************************************************************************
                                                                              
                                Interface constants
                                                                              
*******************************************************************************/
#define FM_MAX_EXT_LENGTH	   5
#define FM_MAX_OBJ_NAME_LENGTH  200// 45
#ifdef FF_MMI_UNICODE_SUPPORT
/* x0083025 OMAPS00156759 
 * Changed from 250 to 100 because to much memory is being taken when unicode 
 * is enabled (2 bytes per char) and system would eventually crash */
#define FM_MAX_DIR_PATH_LENGTH 100
#else
#define FM_MAX_DIR_PATH_LENGTH 250
#endif
#define FM_MAX_OBJ_DATE_LENGTH 15
#define FM_MAX_OBJ_TIME_LENGTH 15

#define FM_COPY_BUFFER_SIZE 	1000

#define FM_MAX_OBJ  20
#define E_FM_THMB_INIT			(T_MFW_EVENT)0x00000001
#define E_FM_THMB_DEINIT           (T_MFW_EVENT)0x00000010
#define E_FM_THMB_GEN			(T_MFW_EVENT)0x00000100
#define E_FM_IMG_INIT			(T_MFW_EVENT)0x00001000
#define E_FM_IMG_DEINIT		(T_MFW_EVENT)0x00010000
#define E_FM_IMG_DRAW			(T_MFW_EVENT)0x00100000
#define E_FM_AUDIO_STOP		(T_MFW_EVENT)0x01000000
#define E_FM_AUDIO_PROG		(T_MFW_EVENT)0x10000000
#define E_FM_ALL_SERVICES		(T_MFW_EVENT)0xFFFFFFFF
//Image 

typedef enum
{
	FM_UCP_ERROR_FATAL = -8,
	FM_UCP_ERROR_NONFATAL,
	FM_UCP_ERROR_UNKNOWN,
	FM_UCP_ERROR_INVALID_ARGUMENT,
	FM_UCP_ERROR_BAD_STREAM,
	FM_UCP_ERROR_NO_MEM,
	FM_UCP_ERROR_READ_FAILED,
	FM_UCP_ERROR,
	FM_UCP_NONE = 0,
	FM_UCP_NO_ERROR,
	FM_UCP_THMB_INIT,
	FM_UCP_THMB_GEN,
	FM_UCP_THMB_DEINIT,
	FM_UCP_THMBIMG_INIT,
	FM_UCP_THMBIMG_DRAW,
	FM_UCP_THMBIMG_DEINIT,
	FM_UCP_QCIFIMG_INIT,
	FM_UCP_QCIFIMG_DRAW,
	FM_UCP_QCIFIMG_DEINIT
}T_FM_IMG_STATE;
typedef struct
{
	T_FM_IMG_STATE state;
	T_FM_IMG_STATE cmd;
	int ipMsl;
}T_FM_IMG;

typedef struct
{
	BOOL thumbnail;
 }T_FM_OBJ_IMAGE;
#define MFW_FM_ZOOM_LOW 	 1024
#define MFW_FM_ZOOM_MAX	 (8 * MFW_FM_ZOOM_LOW)
typedef enum
{
	MFW_FM_ROTATE_0 = 	0,
	MFW_FM_ROTATE_90, 
	MFW_FM_ROTATE_180,
	MFW_FM_ROTATE_270,
	MFW_FM_ROTATE_MAX
}T_FM_IMG_ROTATE;
//Audio
typedef enum
{
	FM_AUD_ERROR_NOTALLOWED = -12,
	FM_AUD_ERROR_AUDIO,
	FM_AUD_ERROR_DENIED,
	FM_AUD_ERROR_PLAYER,
	FM_AUD_ERROR_FFS,
	FM_AUD_ERROR_MSGING,
	FM_AUD_ERROR_NOTREADY,
	FM_AUD_ERROR_INVALIDPARAM,
	FM_AUD_ERROR_INTERNAL,
	FM_AUD_ERROR_MEM,
	FM_AUD_ERROR_NOTSUPPORTED,
	FM_AUD_ERROR = -1 ,
	FM_AUD_NO_ERROR,
	FM_AUD_NONE,
	FM_AUD_PLAY,
	FM_AUD_FORWARD,
	FM_AUD_REWIND,
	FM_AUD_PAUSE,
	FM_AUD_STOP,
}T_FM_AUD_STATE;

#define FM_AUD_MIDI_CHANNEL_MONO      1
#define FM_AUD_MIDI_CHANNEL_STEREO   2

#define FM_AUD_MP3_CHANNEL_MONO      0
#define FM_AUD_MP3_CHANNEL_STEREO   1

#define FM_AUD_AAC_CHANNEL_MONO      0
#define FM_AUD_AAC_CHANNEL_STEREO   1
#define FM_MFW_QCIF_WIDTH 176
#define FM_MFW_QCIF_HEIGHT 144
#define FM_MFW_PREVIEW_WIDTH  48
#define FM_MFW_PREVIEW_HEIGHT 48

typedef struct
{
	T_FM_AUD_STATE state;
}T_FM_AUD;

typedef struct
{
	BOOL hidden;
}T_FM_OBJ_AUDIO;

typedef enum
{
	FM_IMAGE=0,
	FM_AUDIO
}T_FM_APP_TYPE;	

//Misc
typedef enum
{
	FM_INVALID=-1,
	FM_NOR_FLASH,
	FM_NORMS_FLASH,
	FM_NAND_FLASH,
	FM_T_FLASH
}T_FM_DEVICE_TYPE;	
typedef enum
{
	FM_NONE=-1,
	FM_OPEN,
	FM_BACK,
	FM_COPY,
	FM_MOVE,
	FM_OPTIONS_FILE,
	FM_OPTIONS_FOLDER,
	FM_IMGVIEW,
	FM_RENAME,
	FM_DELETE,
	FM_NEWDIR,
	FM_REMOVE
}T_FM_OPERATION_TYPE;	

typedef enum
{
	OBJECT_TYPE_NONE=-1,
	OBJECT_TYPE_FILE,
	OBJECT_TYPE_AUDIO,
	OBJECT_TYPE_AUDIO_MIDI,
	OBJECT_TYPE_AUDIO_MP3,
	OBJECT_TYPE_AUDIO_AAC,
	OBJECT_TYPE_IMAGE,
	OBJECT_TYPE_FOLDER,
	OBJECT_TYPE_IMAGE_FOLDER,
	OBJECT_TYPE_AUDIO_FOLDER,
	OBJECT_TYPE_AUDIO_XMF,                       /* Apr 06, 2007 DRT: OMAPS00124874 x0039928 */
	OBJECT_TYPE_AUDIO_IMY,
	OBJECT_TYPE_AUDIO_MMF,
	OBJECT_TYPE_AUDIO_MXMF,
	OBJECT_TYPE_AUDIO_SMS,
	OBJECT_TYPE_AUDIO_WAV
}T_FM_OBJ_TYPE;

typedef enum
{
	FM_COPY_ERROR = -1,
	FM_COPY_NONE,
	FM_COPY_START,
	FM_COPY_PROGRESS,
	FM_COPY_DONE
}T_FM_COPY_STATE;
typedef enum
{
	FM_STATE_NONE=0,	
	FM_STATE_ERROR
}T_MFW_FM_STATE;
typedef enum
{
	FM_NO_ERROR=0,
	FM_DIR_DOESNOTEXISTS,
	FM_FILE_EXISTS,
	FM_FILE_DOESNOTEXISTS,
	FM_READDIR_ERROR,
	FM_RENAME_ERROR,
	FM_DELETE_ERROR,
	FM_DELETE_DIRNOTEMPTY,
	FM_DIR_EXISTS,
	FM_FORMAT_ERROR
}T_MFW_FM_STATUS;


typedef struct
{
	UINT32 aud_tt;
	UINT16 aud_pt;
}T_MFW_AUD_PARA;

typedef union
{
	T_FM_AUD aud;
	T_FM_IMG img;
}T_FM_DIRTYPE_INFO;

typedef union
{
	T_FM_IMG_STATE img_state;
	T_FM_AUD_STATE aud_state;
} T_MFW_FM_PARA;

typedef union
{
	T_FM_OBJ_AUDIO audio;
	T_FM_OBJ_IMAGE image;
}T_FM_OBJ_DATA;

typedef struct
{
	char name[FM_MAX_OBJ_NAME_LENGTH];
	#ifdef FF_MMI_UNICODE_SUPPORT
		T_WCHAR name_uc[FM_MAX_OBJ_NAME_LENGTH];
	#endif
	T_FM_OBJ_TYPE type;
}T_FM_OBJ;

typedef struct
{
	char name[15];
}T_FM_OBJ_ITEM;

typedef struct
{
	int size;
	char date[FM_MAX_OBJ_DATE_LENGTH];
	char time[FM_MAX_OBJ_TIME_LENGTH];
}T_FM_OBJ_PROPERTIES;

typedef struct
{
	T_FM_OBJ       *obj_list[FM_MAX_OBJ];
	T_FM_OBJ_PROPERTIES *obj_properties[FM_MAX_OBJ];
	T_FM_OBJ_DATA *obj_data[FM_MAX_OBJ];
	UBYTE                      num_objs;
	char                         dir_name[FM_MAX_OBJ_NAME_LENGTH];
	char                         dir_path[FM_MAX_DIR_PATH_LENGTH];
	T_FM_OBJ_TYPE        dir_type;
	int                            depth;	
	int                            startIndex;
	int	                        currentIndex;
	T_FM_DIRTYPE_INFO     dirinfo;
}T_FM_CURDIR;
typedef struct
{
	T_FM_OBJ       *obj_list[FM_MAX_OBJ];
	UBYTE                      num_objs;
	char                         dir_name[FM_MAX_OBJ_NAME_LENGTH];
	char                         dir_path[FM_MAX_DIR_PATH_LENGTH];
	T_FM_OBJ_TYPE        dir_type;
	int	                        currentIndex;
	int                            depth;	
}T_FM_DESTDIR;
typedef struct
{
    T_MFW_EVENT      emask;         /* events of interest */
    T_MFW_EVENT      event;          /* current event */
    T_MFW_CB           handler;
    T_MFW_FM_PARA  para;
} T_MFW_FM;
typedef struct
{

	T_RFS_FD sourceFileID;
//	int sourceFileID;
#ifdef FF_MMI_UNICODE_SUPPORT
	T_WCHAR *sourceFile;
#else
	char *sourceFile;
#endif
	T_FM_DEVICE_TYPE source_type;
	int  sourceFileSize;

	T_RFS_FD destFileID;
//	int destFileID;
#ifdef FF_MMI_UNICODE_SUPPORT
	T_WCHAR *destinationFile;
#else
	char *destinationFile;
#endif
	T_FM_DEVICE_TYPE destination_type;
	char *buf;
	T_RFS_SIZE buf_size;
	int  bytesRead;
	T_FM_COPY_STATE state;
}T_MFW_FM_COPYMOVE_STRUCT;

#define THUMB "thumbnail"

#define NORDIR "/mmi/"
#define NORDIR_IMG "jpeg"
#define NORDIR_AUD "tones"

#define NORMSDIR "/"
#define NORMSDIR_IMG "jpeg"
#define NORMSDIR_AUD "tones"

#define NANDDIR "/"
#define NANDDIR_IMG  "jpeg"
#define NANDDIR_AUD  "tones"

#define TFLASHDIR "/"
#define TFLASHDIR_IMG "jpeg"
#define TFLASHDIR_AUD "tones"



void mfw_fm_readProperties(char *dir_path, char *dir_name, UBYTE num_objs, T_FM_OBJ **obj_list, T_FM_OBJ_PROPERTIES **obj_properties,T_FM_DEVICE_TYPE  source, T_FM_APP_TYPE app);
void mfw_fm_readFileFolders(char *dir_path, char *dir_name, UBYTE num_objs, T_FM_OBJ **obj_list, T_FM_DEVICE_TYPE  source);
T_MFW_FM_STATUS mfw_fm_create_rootdir(T_FM_DEVICE_TYPE  source);
T_MFW_HND mfw_fm_create(T_MFW_HND hWin, T_MFW_EVENT event, T_MFW_CB cbfunc);
T_MFW_RES mfw_fm_delete(T_MFW_HND hnd);
T_MFW_FM_STATUS mfw_fm_format(T_FM_DEVICE_TYPE  source)	;
#ifdef FF_MMI_UNICODE_SUPPORT
T_MFW_FM_STATUS mfw_fm_rename(T_WCHAR* old_filename, T_WCHAR* new_filename, T_FM_DEVICE_TYPE  source);
T_MFW_FM_STATUS mfw_fm_remove(T_WCHAR* objname,T_FM_DEVICE_TYPE  source);
#else
T_MFW_FM_STATUS mfw_fm_rename(char* old_filename, char * new_filename, T_FM_DEVICE_TYPE  source);
T_MFW_FM_STATUS mfw_fm_remove(char* objname,T_FM_DEVICE_TYPE  source);	
#endif

T_MFW_FM_STATUS mfw_fm_rename_image(char * path, char * dirname, char* old_filename, char * new_filename,T_FM_DEVICE_TYPE  source);
T_MFW_FM_STATUS mfw_fm_remove_image(char * path, char * dirname, char* objname,T_FM_DEVICE_TYPE  source)	;
T_MFW_FM_STATUS mfw_fm_newdir(char *dirname, T_FM_DEVICE_TYPE  source);
T_MFW_FM_STATUS mfw_fm_createRootDir(T_FM_DEVICE_TYPE  source);
T_FM_COPY_STATE mfw_fm_copy_start(T_MFW_FM_COPYMOVE_STRUCT *data)	;
T_FM_COPY_STATE mfw_fm_copy_continue(T_MFW_FM_COPYMOVE_STRUCT *data)	;
void mfw_fm_getMemStat(int *nfree, int *nused,T_FM_DEVICE_TYPE  source);


//Image
T_FM_IMG_STATE  mfw_fm_imgThmb_create(void);
T_FM_IMG_STATE  mfw_fm_imgThmb_init(void);
T_FM_IMG_STATE  mfw_fm_imgThmb_destroy(void);
T_FM_IMG_STATE  mfw_fm_imgThmb_deinit(void);
T_FM_IMG_STATE  mfw_fm_imgThmb_pause(void);
T_FM_IMG_STATE  mfw_fm_imgThmb_generate(void);
T_FM_IMG_STATE  mfw_fm_imgThmb_setparams(char * inFilepath, char* inFilename, int width,int height,T_FM_DEVICE_TYPE  source);
T_FM_IMG_STATE  mfw_fm_imgThmb_setcallback(void);
T_FM_IMG_STATE  mfw_fm_imgViewer_create(void);
T_FM_IMG_STATE  mfw_fm_imgViewer_init(void);
T_FM_IMG_STATE  mfw_fm_imgViewer_destroy(void);
T_FM_IMG_STATE  mfw_fm_imgViewer_deinit(void);
T_FM_IMG_STATE  mfw_fm_imgViewer_pause(void);
T_FM_IMG_STATE  mfw_fm_imgViewer_view(void);
T_FM_IMG_STATE  mfw_fm_imgViewer_setparams(char * inFilepath, char* inFilename, int width,int height);
T_FM_IMG_STATE  mfw_fm_imgViewer_setcallback(void);
void mfw_fm_thmb_mslcb( U32 tCMd,U32 tStatus);
void mfw_fm_view_mslcb( U32 tCMd,U32 tStatus);
T_MFW_FM_STATUS mfw_fm_checkThumbnail( char * obj,T_FM_DEVICE_TYPE  source);
T_FM_IMG_STATE  mfw_fm_imgViewer_setConfigs(char *filename,int width, int height,int xoffset, int yoffset,U32 zFactor,int rotation_degree, T_FM_DEVICE_TYPE  source);
T_FM_IMG_STATE  mfw_fm_imgViewer_setImageProperty( char * filepath, char *filename, int width, int height,int xoffset, int yoffset,T_FM_DEVICE_TYPE  source);
void mfw_fm_thmb_mslcb( U32 tCMd,U32 tStatus);
void mfw_fm_view_mslcb( U32 tCMd,U32 tStatus);



//Audio
T_FM_AUD_STATE mfw_fm_audStop(void);
#ifdef FF_MMI_UNICODE_SUPPORT
T_FM_AUD_STATE mfw_fm_audPlay(T_WCHAR * file_name, T_FM_OBJ_TYPE file_type, BOOL play_bar);
#else
T_FM_AUD_STATE mfw_fm_audPlay(char * file_name, T_FM_OBJ_TYPE file_type, BOOL play_bar);
#endif
T_FM_OBJ_TYPE mfw_fm_aud_get_type(char * file_name);
T_FM_AUD_STATE mfw_fm_audPause(void);
T_FM_AUD_STATE mfw_fm_audResume(void);
T_MFW_FM_STATUS mfw_fm_readDir(char *dir_path, char *dir_name, UBYTE *num_objs, T_FM_OBJ **obj_list, T_FM_DEVICE_TYPE  source, T_FM_APP_TYPE  app);
/* OMAPS00151698, x0056522 */
void mfw_fm_audPlay_cb(void *parameter);

/*********************************/

#if 0
typedef UINT16 T_RFS_MODE;

#define RFS_IXUSR   0x0100  // Execute permission for the user
#define RFS_IWUSR   0x0200  // Write permission for the user
#define RFS_IRUSR   0x0400  // Read permission for the user
#define RFS_IRWXU   0x0700  // Read Write permission mask (default) for the user

#define RFS_IXGRP   0x0010  // Execute permission for group
#define RFS_IWGRP   0x0020  // Write permission for group
#define RFS_IRGRP   0x0040  // Read permission for group
#define RFS_IRWXG   0x0070  // Read Write permission mask (default) for group

#define RFS_IXOTH   0x0001  // Execute permission for others
#define RFS_IWOTH   0x0002  // Write permission for others
#define RFS_IROTH   0x0004  // Read permission for others
#define RFS_IRWXO   0x0007  // Read Write permission mask (default) for others

typedef INT32 T_RFS_RET;

#define RFS_EOK               0

#define RFS_ENODEVICE         -1
#define RFS_EAGAIN            -2
#define RFS_ENOSPACE          -3
#define RFS_EFSFULL           -4
#define RFS_EBADNAME          -5
#define RFS_ENOENT            -6
#define RFS_EEXISTS           -7
#define RFS_EACCES            -8
#define RFS_ENAMETOOLONG      -9
#define RFS_EINVALID          -10
#define RFS_ENOTEMPTY         -11
#define RFS_EFBIG             -12
#define RFS_ENOTAFILE         -13
#define RFS_ENUMFD            -14
#define RFS_EBADFD            -15
#define RFS_EBADDIR           -16
#define RFS_EBADOP            -17
#define RFS_ELOCKED           -18
#define RFS_EMOUNT            -19
#define RFS_EDEVICE           -20
#define RFS_EBUSY             -21
#define RFS_ENOTADIR          -22
#define RFS_EMAGIC            -23
#define RFS_EMEMORY           -24
#define RFS_EMSGSEND          -25
#define RFS_ENOTALLOWED       -26
#define RFS_ENOTSUPPORTED     -27
#define RFS_EEMPTY            -28
#define RFS_EINTERNALERROR    -29
#define RFS_CORRUPT_LFN       -30
#define RFS_E_FILETOOBIG      -31


typedef union {
    T_RFS_FS_STAT         file_system;
    T_RFS_MP_STAT         mount_point;
    T_RFS_FILE_DIR_STAT   file_dir;
} T_RFS_STAT;

/* Defines the DIR type. This represents a directory stream, which is an ordered
 * sequence of all the di-rectory entries (files) in a particular directory.
 */
typedef struct {
    UINT32    opendir_ino;  // inode of directory that was opened
    UINT32    lastread_ino; // last inode of the read direcoty entry
    UINT8     mpt_idx;      // mount point where the directory is located
    UINT8     fscore_nmb; // file system type
} T_RFS_DIR;

typedef enum
{
	FFS_ATTRIB_READWRITE   	= FFS_ATTRIB_READWRITE_MASK,
	FFS_ATTRIB_READONLY    	= FFS_ATTRIB_READONLY_MASK,
	FFS_ATTRIB_HIDDEN 		= FFS_ATTRIB_HIDDEN_MASK,
	FFS_ATTRIB_SYSTEM		= FFS_ATTRIB_SYSTEM_MASK,
	FFS_ATTRIB_SUBDIR       = FFS_ATTRIB_SUBDIR_MASK,
	FFS_ATTRIB_ARCHIVE	    = FFS_ATTRIB_ARCHIVE_MASK,
	FFS_ATTRIB_INVALID
}T_FFS_ATTRIB_TYPE;

/**********************************/
#endif

#endif

