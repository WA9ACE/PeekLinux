/**
 * @file  fscore_types.h
 *
 * Generic type definitions for UFS file system cores software entities.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  2/19/2004   E. Oude Middendorp    Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __FSCORE_TYPES_H_
#define __FSCORE_TYPES_H_

#define FFS_UNICODE 1   /* for unicode implementation */


#include "gbi/gbi_api.h"    /* Generic rfs types and definitions */
#include "rfs/rfs_api.h"    /* Generic rfs types and definitions */


//#include "rvm_gen.h"    /* Generic RVM types and functions. */
//#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif


#define FREE_SPACE_OPT  1


#if FREE_SPACE_OPT

#define INVALID_FREE_SPACE_VAL   0xFFFFFFFF

#endif




/**
 *  Below is a list of all defined FSCORE exceptions. All exceptions
 *  returned are of type T_FSCORE_RET unless otherwise stated.
 *  All exception codes, except FSCORE_EOK, are negative.
 *  In order to provide source compatibility with future version of FSCORE,
 *  the application programmer should only check for exceptions by testing
 *  if the return code is less than zero. A zero or positive re-turn code
 *  should be treated as a success indication unless otherwise noted.
 */
typedef T_RFS_RET             T_FSCORE_RET;



#define FSCORE_EOK            RFS_EOK
#define FSCORE_ENODEVICE      RFS_ENODEVICE
#define FSCORE_EAGAIN         RFS_EAGAIN
#define FSCORE_ENOSPACE       RFS_ENOSPACE
#define FSCORE_EFSFULL        RFS_EFSFULL
#define FSCORE_EBADNAME       RFS_EBADNAME
#define FSCORE_ENOENT         RFS_ENOENT
#define FSCORE_EEXISTS        RFS_EEXISTS
#define FSCORE_EACCES         RFS_EACCES
#define FSCORE_ENAMETOOLONG   RFS_ENAMETOOLONG
#define FSCORE_EINVALID       RFS_EINVALID
#define FSCORE_ENOTEMPTY      RFS_ENOTEMPTY
#define FSCORE_EFBIG          RFS_EFBIG
#define FSCORE_ENOTAFILE      RFS_ENOTAFILE
#define FSCORE_ENUMFD         RFS_ENUMFD
#define FSCORE_EBADFD         RFS_EBADFD
#define FSCORE_EBADDIR        RFS_EBADDIR
#define FSCORE_EBADOP         RFS_EBADOP
#define FSCORE_ELOCKED        RFS_ELOCKED
#define FSCORE_EMOUNT         RFS_EMOUNT
#define FSCORE_EDEVICE        RFS_EDEVICE
#define FSCORE_EBUSY          RFS_EBUSY
#define FSCORE_ENOTADIR       RFS_ENOTADIR
#define FSCORE_EMAGIC         RFS_EMAGIC
#define FSCORE_EMEMORY        RFS_EMEMORY
#define FSCORE_EMSGSEND       RFS_EMSGSEND
#define FSCORE_ENOTALLOWED    RFS_ENOTALLOWED
#define FSCORE_ENOTSUPPORTED  RFS_ENOTSUPPORTED
#define FSCORE_EEMPTY         RFS_EEMPTY
#define FSCORE_EINTERNALERROR RFS_EINTERNALERROR
#define FSCORE_CORRUPT_LFN    RFS_CORRUPT_LFN
#define FSCORE_EFILETOOBIG    RFS_E_FILETOOBIG
#define FSCORE_EUNKNOWNPART   RFS_EUNKOWNFORMAT

/**
 *  Defines the mode attribute and values.
 *  The mode is an attribute of a file indicating the permission bits.
 *  The mode attribute is formed by OR'ing the values below.
 */
typedef T_RFS_MODE T_FSCORE_MODE;

#define FSCORE_IXUSR  RFS_IXUSR // Execute permission for the user
#define FSCORE_IWUSR  RFS_IWUSR  // Write permission for the user
#define FSCORE_IRUSR  RFS_IRUSR  // Read permission for the user
#define FSCORE_IRWXU  RFS_IRWXU  // Read Write permission mask (default) for user

#define FSCORE_IXGRP  RFS_IXGRP  // Execute permission for group
#define FSCORE_IWGRP  RFS_IWGRP  // Write permission for group
#define FSCORE_IRGRP  RFS_IRGRP  // Read permission for group
#define FSCORE_IRWXG  RFS_IRWXG  // Read Write permission mask (default) for group

#define FSCORE_IXOTH  RFS_IXOTH  // Execute permission for others
#define FSCORE_IWOTH  RFS_IWOTH  // Write permission for others
#define FSCORE_IROTH  RFS_IROTH  // Read permission for others
#define FSCORE_IRWXO  RFS_IRWXO  // Read Write permission mask (default) for others

#define MAX_NR_PART   16
/* MACRO constansts for Partition names */
/* ALERT: Dont change these names & value */
#define NOR_PARTITION_NAME				"NOR\0\0"
#define NAND_PARTITION_NAME				"INANDA1\0"
#define MMC_PARTITION_NAME				"MMC\0\0"
#define FAT_MAX_PART_SIZE				5




/**
 *  Defines the file descriptor.
 */
typedef INT16 T_FSCORE_FD;

/**
 *  Defines the size type.
 */
typedef INT32 T_FSCORE_SIZE;

/**
 *  Defines the offset type.
 */
typedef INT32 T_FSCORE_OFFSET;





/**
 *  This structure defines the partition information containing media
 *  information and detailed partition in-formation.
 */
typedef struct
{
  UINT8          	 	partition_name[GBI_MAX_PARTITION_NAME+1]; // Partition name
  UINT8					gbi_part_name[GBI_MAX_PARTITION_NAME+1];  /* Partition name from GBI */
  UINT8           		media_nbr;        // Media id
  UINT8           		partition_nbr;    // Partition id (in the above mentioned media)
  T_GBI_FS_TYPE   		filesys_type;     // Indicates file system used on this partition
  T_GBI_BLKSIZE   		blk_size;         // Partition block size
  T_GBI_BLOCK     		partition_size;   // Number of blocks used
  BOOL			  		is_mounted;
  T_GBI_MEDIA_TYPE 		media_type;

#if FREE_SPACE_OPT  
  UINT32				free_space;	
#endif
   
}T_FSCORE_PARTITION_INFO;


/**
 *  This structure defines the partition table containing the number of
 *  partitions of this FS-core and a pointer to the actual information table.
 */
typedef struct
{
  UINT8 				fscore_numb;
  UINT8                 nbr_partitions;     // Number of partitions in table
  T_FSCORE_PARTITION_INFO  *partition_info_table; // Pointer to partitions table
}T_FSCORE_PARTITION_TABLE;


#define T_FSCORE_WHENCE INT8
#define RFS_SEEK_SET (0)
#define RFS_SEEK_CUR (1)
#define RFS_SEEK_END (2)

/**
 *  This defines the index of the asynchronous operation (command),
 *  which is finished.
 */
typedef enum{
  FSCORE_INVALID_RSP = -1, 
  FSCORE_CLOSE_RSP = 0,
  FSCORE_WRITE_RSP,
  FSCORE_READ_RSP,
  FSCORE_LSEEK_RSP,
  FSCORE_FCHMOD_RSP,
  FSCORE_FSTAT_RSP,
  FSCORE_FSYNC_RSP,
  FSCORE_OPEN_RSP,
  FSCORE_CHMOD_RSP,
  FSCORE_STAT_RSP,
  FSCORE_REMOVE_RSP, // 10
  FSCORE_RENAME_RSP,
  FSCORE_MKDIR_RSP,
  FSCORE_RMDIR_RSP,
  FSCORE_OPENDIR_RSP,
  FSCORE_READDIR_RSP,
  FSCORE_PREFORMAT_RSP,
  FSCORE_FORMAT_RSP,
  FSCORE_SETPARTTBL_RSP,
  FSCORE_TRUNC_RSP,
  FSCORE_GETATTRIB_RSP, // 20
  FSCORE_SETATTRIB_RSP,
  FSCORE_GETDATETIME_RSP,
  FSCORE_QUERY_RSP,
  FSCORE_QUERY_PARTITION_RSP,
  FSCORE_FILE_READ_RSP,
  FSCORE_FILE_WRITE_RSP,
  FSCORE_TRUNC_NAME_RSP,
  FSCORE_CLOSEDIR_RSP,
  FSCORE_MOUNT_RSP,
  FSCORE_UNMOUNT_RSP, //30
  FSCORE_SET_VOL_LABEL_RSP,
  FSCORE_GET_VOL_LABEL_RSP
}T_FSCORE_CMD_ID;


/**
 *  This defines the union containing a file descriptor and unique pair ID.
 *  Depending on the request op-eration the valid member should be used.
 */
typedef struct {
  T_FSCORE_FD       fd;
  T_FSCORE_RET      pair_id;
} T_FSCORE_PAIR_VALUE;


/**
 *  This defines the flag type and values.
 *  The open mode is established as a combination of the bits de-fined below.
 *  Of the following first three values, only one can be set.
 *  The other flags can be OR'ed to one of these first three values.
 *  When a file is opened for write-only it can not be read and when a file
 *  is opened for read only, it can not be written.
 */
typedef T_RFS_FLAGS       T_FSCORE_FLAGS;

#define FSCORE_O_RDONLY   RFS_O_RDONLY
#define FSCORE_O_WRONLY   RFS_O_WRONLY
#define FSCORE_O_RDWR     RFS_O_RDWR
#define FSCORE_O_CREATE   RFS_O_CREAT
#define FSCORE_O_APPEND   RFS_O_APPEND
#define FSCORE_O_TRUNC    RFS_O_TRUNC
#define FSCORE_O_EXCL  RFS_O_EXCL

#if 0
/* FLAG Types */
typedef  enum
{
 F_GETFLAG=0,
 F_SETFLAG,
 F_SETRETPATH
}T_FILE_FLAGS;

#endif
typedef struct {
  T_RV_HDR                  hdr;
  T_FSCORE_PARTITION_TABLE  *partition_table;
  T_RV_RETURN               return_path;
} T_FSCORE_SET_PARTITION_TABLE_REQ_MSG;

typedef struct {
  T_RV_HDR                  hdr;
  T_FSCORE_PARTITION_TABLE  *partition_table;
  T_FSCORE_RET              result;
} T_FSCORE_SET_PARTITION_TABLE_RSP_MSG;



typedef struct {
  T_RV_HDR            hdr;
  T_FSCORE_CMD_ID     command_id;
  T_FSCORE_PAIR_VALUE pair_value;
  T_FSCORE_RET        result;
  UINT8               fscore_nmb;
} T_FSCORE_READY_RSP_MSG;


#if 0

/**
 *  Defines the structure containing information (meta-data) about the
 *  statistics of a file system core itself.
 */
typedef struct {
  UINT16  oname_length;   // Maximum length object names in characters
  UINT16  pname_length;   // Maximum length path names in characters
  UINT8   max_openfiles;  // Maximum number of open files
  UINT8   max_opendirs;   // Maximum number of open directories
  UINT8   cur_openfiles;  // Number of files currently opened
  UINT8   cur_opendirs;   // Number of directories currently opened
} T_FSCORE_CORE_STAT;



/**
 *  Defines the structure containing information (meta-data) about the
 *  statistics of a partition.
 */
typedef struct {
  UINT32    dev;      // Device identification, the device serial number if available.
                      // - Only supported when build without RFS
  UINT32    blksize;  // Block size (in byte)
                      // - Only supported when build without RFS
  UINT32    read_speed;   // Read speed in Kb/s
                      // Measurement conditions: average speed,
                      // 20 blocks, 100 Kb, clock-cycle 12 MHz
                      // - Only supported when build without RFS
  UINT32    write_speed;    // Write speed in Kb/s
                      // Measurement conditions: average speed,
                      // 20 blocks, 100 Kb, clock-cycle 12 MHz
                      //  - Only supported when build without RFS
  UINT32    max_fsize;    // Maximum file size (in Kb)
  UINT32    dev_size;   // Total device size (in Kb)
                        // - Only supported when build without RFS
  UINT32    free_space;   // Available space for storage on media's
                          // partition (in Kb)
  UINT32    partition_size; // Total partition size (in Kb)
                            // - Only supported when build without RFS
  UINT32    used_size;    // Used partition size (in Kb)
  T_FSCORE_MODE mode;     // Object permission (ugo)
  UINT8     max_part_openfiles; // Maximum number of open files per partition
  UINT8     max_part_opendirs;  // Maximum number of open directories per partition
  char fs_type[GBI_MAX_FS_NAME];  // Type of media (string format)
                                  // - Only supported when build without RFS
  char media_type[GBI_MAX_PARTITION_NAME]; // Type of file system  (string format)
                                           //- Only supported when build without RFS
} T_FSCORE_PARTITION_STAT;



/**
 *  Defines the structure containing information (meta-data) about the
 *  statistics of a file/directory.
 */
typedef struct {
  UINT32          ino;      // Object inode number (unique id)
  UINT32          size;     // Object size in bytes
  time_t          mtime;    // Last modification time: not supported (always 0)
  time_t          ctime;    // Last status change time
  UINT32          n_blocks; // Number of blocks allocated for the object
  T_FSCORE_MODE   mode;     // Object permission (ugo)
} T_FSCORE_FILE_DIR_STAT;


/**
 *  Defines the overall statistics type, which contains information (meta-data) about the status of a file system core or partition or file/directory.
 */
typedef union {
  T_FSCORE_CORE_STAT        fs_core;
  T_FSCORE_PARTITION_STAT   partition;
  T_FSCORE_FILE_DIR_STAT    file_dir;
} T_FSCORE_STAT;

#endif


typedef T_RFS_DIR  			T_FSCORE_DIR;     /* for directory operations */
typedef T_RFS_STAT  		T_FSCORE_STAT;       /* for rfs_stat operations */



/**
 *  This structure defines the generic FS-core function table.
 */
typedef struct
{
  T_FSCORE_RET    (*fscore_fcntl) (T_FSCORE_FD fd, INT8 cmd,void  *arg);
  T_FSCORE_RET    (*fscore_close) (T_FSCORE_FD fd);
  T_FSCORE_SIZE   (*fscore_write) (T_FSCORE_FD fd, const void *buf, T_FSCORE_SIZE size);
  T_FSCORE_SIZE   (*fscore_read)  (T_FSCORE_FD fd, void *buf, T_FSCORE_SIZE size);
  T_FSCORE_OFFSET (*fscore_lseek) (T_FSCORE_FD fd, T_FSCORE_OFFSET offset, T_FSCORE_WHENCE whence);
  T_FSCORE_RET    (*fscore_fchmod)(T_FSCORE_FD fd, T_FSCORE_MODE mode);
  T_FSCORE_RET    (*fscore_fstat) (T_FSCORE_FD fd, T_FSCORE_STAT *stat);
  T_FSCORE_RET    (*fscore_fsync) (T_FSCORE_FD fd);
  T_FSCORE_RET    (*fscore_trunc)(T_FSCORE_FD fd, T_FSCORE_SIZE size);
  T_FSCORE_RET    (*fscore_set_partition_table)(T_FSCORE_PARTITION_TABLE  *partition_table,
                                                T_RV_RETURN return_path);


  T_FSCORE_RET    (*fscore_mount_nb)(T_RV_RETURN return_path);
  T_FSCORE_RET    (*fscore_unmount_nb)(T_RV_RETURN return_path);
 
  /* Unicode functions */
  T_FSCORE_RET    (*fscore_open_uc_nb)(const T_WCHAR *pathname, T_FSCORE_FLAGS  flags,
                                   T_FSCORE_MODE  mode, T_RV_RETURN   return_path);
  T_FSCORE_RET    (*fscore_chmod_uc_nb)(const T_WCHAR   *pathname, T_FSCORE_MODE    mode,
                                   T_RV_RETURN    return_path);
  T_FSCORE_RET    (*fscore_stat_uc_nb) (const T_WCHAR   *pathname, T_FSCORE_STAT *stat,
                                   T_RV_RETURN    return_path);
  T_FSCORE_RET    (*fscore_remove_uc_nb) (const T_WCHAR *pathname, T_RV_RETURN return_path);
  T_FSCORE_RET    (*fscore_rename_uc_nb)  (const T_WCHAR   *oldname, const T_WCHAR    *newname,
                                   T_RV_RETURN    return_path);
  T_FSCORE_RET    (*fscore_mkdir_uc_nb)(const T_WCHAR   *pathname, T_FSCORE_MODE    mode,
                                   T_RV_RETURN    return_path);
  T_FSCORE_RET    (*fscore_rmdir_uc_nb)(const T_WCHAR   *pathname, T_RV_RETURN return_path);
  T_FSCORE_RET    (*fscore_opendir_uc_nb)(const T_WCHAR   *pathname, T_FSCORE_DIR *dirp,
                                   T_RV_RETURN    return_path);
  T_FSCORE_RET    (*fscore_readdir_uc_nb)(T_FSCORE_DIR   *dirp, T_WCHAR *buf,
                                   T_FSCORE_SIZE  size, T_RV_RETURN  return_path);
  T_FSCORE_RET    (*fscore_preformat_uc_nb)(const T_WCHAR   *pathname, UINT16     magic,
                                   T_RV_RETURN    return_path);
  T_FSCORE_RET    (*fscore_format_uc_nb)(const T_WCHAR    *pathname, const T_WCHAR   *name,
                                   UINT16     magic, T_RV_RETURN return_path);
  T_FSCORE_RET    (*fscore_getattrib_uc_nb)(const T_WCHAR *pathname,T_RV_RETURN_PATH return_path);
  T_FSCORE_RET    (*fscore_setattrib_uc_nb)(const T_WCHAR *pathname,T_FFS_ATTRIB_TYPE attrib,T_RV_RETURN_PATH return_path);
  T_FSCORE_RET    (*fscore_getdatetime_uc_nb)(const T_WCHAR *pathname,T_RTC_DATE_TIME *date_time_p,T_RTC_DATE_TIME *crdate_time_p,T_RV_RETURN_PATH return_path);

    /* Ascii functions */
  T_FSCORE_RET    (*fscore_open_nb)(const char *pathname, T_FSCORE_FLAGS  flags,
                                   T_FSCORE_MODE  mode, T_RV_RETURN   return_path);
  T_FSCORE_RET    (*fscore_chmod_nb)(const char   *pathname, T_FSCORE_MODE    mode,
                                   T_RV_RETURN    return_path);
  T_FSCORE_RET    (*fscore_stat_nb) (const char   *pathname, T_FSCORE_STAT *stat,
                                   T_RV_RETURN    return_path);
  T_FSCORE_RET    (*fscore_remove_nb) (const char *pathname, T_RV_RETURN return_path);
  T_FSCORE_RET    (*fscore_rename)  (const char   *oldname, const char    *newname,
                                   T_RV_RETURN    return_path);
  T_FSCORE_RET    (*fscore_mkdir_nb)(const char   *pathname, T_FSCORE_MODE    mode,
                                   T_RV_RETURN    return_path);
  T_FSCORE_RET    (*fscore_rmdir_nb)(const char   *pathname, T_RV_RETURN return_path);
  T_FSCORE_RET    (*fscore_opendir_nb)(const char   *pathname, T_FSCORE_DIR *dirp,
                                   T_RV_RETURN    return_path);
  T_FSCORE_RET    (*fscore_readdir_nb)(T_FSCORE_DIR   *dirp, char *buf,
                                   T_FSCORE_SIZE  size, T_RV_RETURN  return_path);

  T_FSCORE_RET    (*fscore_preformat_nb)(const char   *pathname, UINT16     magic,
                                   T_RV_RETURN    return_path);
  T_FSCORE_RET    (*fscore_format_nb)(const char    *pathname, const char   *name,
                                   UINT16     magic, T_RV_RETURN return_path);
  T_FSCORE_RET    (*fscore_getattrib_nb)(const char *pathname,T_RV_RETURN_PATH return_path);
  T_FSCORE_RET    (*fscore_setattrib_nb)(const char *pathname,T_FFS_ATTRIB_TYPE attrib,T_RV_RETURN_PATH return_path);
  T_FSCORE_RET    (*fscore_getdatetime_nb)(const char *pathname,T_RTC_DATE_TIME *date_time_p,T_RTC_DATE_TIME *crdate_time_p,T_RV_RETURN_PATH return_path);
  T_FSCORE_RET    (*fscore_closedir_uc_nb)(T_FSCORE_DIR   *dirp, T_RV_RETURN   return_path);  
  T_FSCORE_RET    (*fscore_set_label_uc_nb)(const T_WCHAR    *mpt_name, const T_WCHAR *label, T_RV_RETURN   return_path);  
  T_FSCORE_RET    (*fscore_get_label_uc_nb)(const T_WCHAR    *mpt_name, T_WCHAR *label, T_RV_RETURN   return_path);  

} T_FSCORE_FUNC_TABLE;




#ifdef __cplusplus
}
#endif


#endif /*__FSCORE_TYPES_H_*/

