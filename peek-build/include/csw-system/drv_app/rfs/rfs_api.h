/**
 * @file  rfs_api.h
 *
 * API Definition for RFS SWE.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  1/23/2004  ()   Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __RFS_API_H_
#define __RFS_API_H_


#include <time.h>
#include "rvm/rvm_gen.h"    /* Generic RVM types and functions. */
#include "rtc/rtc_api.h"

#ifdef __cplusplus
extern "C"
{
#endif


/**
 *  These defnitions are necessary for the operation mode of functions to
 *  an opened file. The operation can be asynchronous or synschronous.
 */

#define F_SETFLAG     0x00
#define F_SETRETPATH  0x01
#define F_GETFLAG     0x02

#define O_AIO         0x00
#define O_SIO         0x01


#define MAX_MP_STAT_NAME 			16
#define RFS_FAT12_FS_NAME			"FAT12\0\0"
#define RFS_FAT16_FS_NAME			"FAT16\0\0"
#define RFS_FAT32_FS_NAME			"FAT32\0\0"
#define RFS_RELIANCE_FS_NAME 		"RELIANCE\0\0"
#define RFS_UNKNOWN_FS_NAME			"UNKNOWN\0\0"



#define RFS_MPT_NAME_FFS 			"FFS\0\0"
#define RFS_MPT_NAME_NOR 			"NOR\0\0"
#define RFS_MPT_NAME_NAND 			"NAND\0\0"
#define RFS_MPT_NAME_MMC 			"MMC\0\0"



/**
 * @name RFS Return type and return values.
 *
 * Currently they are the standard RV return types, but they may
 * be customized in the future.
 */
/*@{*/
typedef T_RV_RET T_RFS_RETURN;
typedef UINT16				  T_WCHAR;

#define RFS_OK              RV_OK
#define RFS_NOT_SUPPORTED   RV_NOT_SUPPORTED
#define RFS_MEMORY_ERR      RV_MEMORY_ERR
#define RFS_INTERNAL_ERR    RV_INTERNAL_ERR
/*@}*/



/* Attribute masks */
#define FFS_ATTRIB_READWRITE_MASK                                          0x00
#define FFS_ATTRIB_READONLY_MASK                                           0x01
#define FFS_ATTRIB_HIDDEN_MASK                                             0x02
#define FFS_ATTRIB_SYSTEM_MASK                                             0x04
#define FFS_ATTRIB_PART_MASK                                               0x08
#define FFS_ATTRIB_SUBDIR_MASK                                             0x10
#define FFS_ATTRIB_ARCHIVE_MASK                                            0x20



typedef enum
{
  RFS_RELIANCE =0,
  RFS_FAT,
  RFS_UNKNOWN_FS
}T_RFS_FSTYPE;


/**********************************

ENUM T_FFS_ATTRIB_TYPE describes the
various File/ Dir attributes that FAT file system provides.
*********************************** */

typedef enum
{
	FFS_ATTRIB_READWRITE   	= FFS_ATTRIB_READWRITE_MASK,
	FFS_ATTRIB_READONLY    	= FFS_ATTRIB_READONLY_MASK,
	FFS_ATTRIB_HIDDEN 		= FFS_ATTRIB_HIDDEN_MASK,
	FFS_ATTRIB_SYSTEM		= FFS_ATTRIB_SYSTEM_MASK,
	FFS_ATTRIB_PART			= FFS_ATTRIB_PART_MASK,
	FFS_ATTRIB_SUBDIR       = FFS_ATTRIB_SUBDIR_MASK,
	FFS_ATTRIB_ARCHIVE	    = FFS_ATTRIB_ARCHIVE_MASK,
	FFS_ATTRIB_INVALID
}T_FFS_ATTRIB_TYPE;



/* Defines the index of the asynchronous operation (command), which is finished. */
typedef enum{
    RFS_CLOSE_RSP,
    RFS_WRITE_RSP,
    RFS_READ_RSP,
    RFS_LSEEK_RSP,
    RFS_FCHMOD_RSP,
    RFS_FSTAT_RSP,
    RFS_FSYNC_RSP,
    RFS_OPEN_RSP,
    RFS_CHMOD_RSP,
    RFS_STAT_RSP,
    RFS_REMOVE_RSP,
    RFS_RENAME_RSP,
    RFS_MKDIR_RSP,
    RFS_RMDIR_RSP,
    RFS_OPENDIR_RSP,
    RFS_READDIR_RSP,
    RFS_PREFORMAT_RSP,
    RFS_FORMAT_RSP,
    RFS_GETATTRIB_RSP,
    RFS_SETATTRIB_RSP,
    RFS_GETDATETIME_RSP,
    RFS_CLOSEDIR_RSP,
    RFS_MOUNT_RSP,
    RFS_UNMOUNT_RSP,
    RFS_SETLABEL_RSP,
    RFS_GETLABEL_RSP
} T_RFS_CMD_ID;

/* Defines the flag type and values. The open mode is established as a combination of
 * the bits de-fined below. Of the following first three values, only one can be set.
 * The other flags can be OR'ed to one of these first three values. When a file is
 * opened for write-only it can not be read and when a file is opened for read only,
 * it can not be written.
 */
typedef UINT16 T_RFS_FLAGS;

#define RFS_O_RDONLY  0x00
#define RFS_O_WRONLY  0x01
#define RFS_O_RDWR    0x02
#define RFS_O_CREAT   0x04
#define RFS_O_APPEND  0x08
#define RFS_O_TRUNC   0x10
#define RFS_O_EXCL    0x20

/* Defines the mode attribute and values. The mode is an attribute of a file
 * indicating the permission bits. The mode attribute is formed by OR'ing the values
 * below.
 */
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

/* Defines the file descriptor. */
typedef INT16 T_RFS_FD;

/* Defines the size type.*/
typedef INT32 T_RFS_SIZE;

/* Defines the offset type. */
typedef INT32 T_RFS_OFFSET;


#if 0
/* Defines the structure containing information (meta-data) about the statistics of a
 * file system
 */
typedef struct {
    UINT16  oname_length;   // Maximum length object names in characters
    UINT16  pname_length;   // Maximum length path names in characters
    UINT8   max_openfiles;  // Maximum number of open files for all device
                            // (static number)
    UINT8   max_opendirs;     // Maximum number of open directories for all
                            // devices (static number)
    UINT8 cur_openfiles;    // Number of files currently opened for all devices
    UINT8 cur_opendirs;     // Number of directories currently opened for all
                            // devices
} T_RFS_FS_STAT;

#endif

/* Defines the structure containing information (meta-data) about the statistics of a
 * mount point
 */
typedef struct {
    UINT32      mpt_id;         // Mount point id
    UINT32      reserved;  		// for future use
    UINT32      reserved_1;     
    UINT32      reserved_2;  
    UINT32      blk_size;        // block size of the partition
    UINT32      nr_blks;         // Number of blocks 
    UINT32      free_space;      // Available space for storage on media's  partition (in Kb)
    UINT32      mpt_size;   	// Total partition size (in Kb)
    UINT32      used_size;        // Used partition size (in Kb)
    char        fs_type[MAX_MP_STAT_NAME];      // Type of media (string format)
    char        media_type[MAX_MP_STAT_NAME];   // Type of file system  (string format)
} T_RFS_MP_STAT;


/* Defines the structure containing information (meta-data) about the statistics of a
 * file/directory
 */
typedef struct {
    UINT32      ino;      // Object inode number (unique id)
    UINT32      size;     // Object size in bytes
    time_t      mtime;    // Last modification time
    time_t      ctime;    // Last status change time
    UINT32      reserved; // For future use
    T_RFS_MODE  mode;     // Object permission (ugo)
} T_RFS_FILE_DIR_STAT;

/* Defines the overall stat type, which could contain information (meta-data) about
 * the statistics of a file system, a mountpoint or file/directory.
 */
typedef union {
#if 0
    T_RFS_FS_STAT         file_system;
#endif
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
    UINT16    resrv_0;
    UINT32    resrv_1; 
} T_RFS_DIR;

/**
 * @name RFS Error definitions
 *
 * Below is a list of all defined RFS exceptions. All exceptions returned are of
 * type T_RFS_RET unless otherwise stated. All exception codes, except RFS_EOK,
 * are negative. In order to provide source compatibility with future version of RFS,
 * the application programmer should only check for exceptions by testing if the
 * return code is less than zero. A zero or positive return code should be treated as
 * a success indication unless otherwise noted.
 */
/*@{*/

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
#define RFS_EUNKOWNFORMAT     -32

/* This defines the union containing a file descriptor and unique pair ID.
 * Depending on the request op-eration the valid member should be used.
 */
typedef struct{
    T_RFS_FD      fd;
    T_RFS_RET     pair_id;
} T_RFS_PAIR_VALUE;

/*
 *  RFS API: Limitations (not configurable)
 */
#define RFS_MAX_OBJNAME_LENGTH          256
#define RFS_DEF_MOUNTPOINT              "Nand"
#define RFS_DEF_DIR                     "Temp"
#define RFS_MAX_FS_CORES                0x03

/**
 * rfs_fcntl
 *
 * Detailled description.
 * This function provides control on the properties of a file that is already open.
 * The argument fd is a descriptor to be operated on by cmd as described below.
 * The third parameter is called arg and is technically a pointer to void, but the
 * interpretation depends on the command.
 *
 * This function is a synchronous function. Switching from synchronous to asynchronous
 * operations or vice-versa, by setting flag F_SETFL, has only effect on the succeding
 * operations (like file writing or reading) and not on the function rfs_fcntl()
 * itself. When a switch from asynchronous operations to syn-chronous operations is
 * made and there are some pending asynchronous operations, the return_path of these
 * pending operations should stay valid.
 *
 * @param   fd          File descriptor obtained when the file was opened.
 * @param   cmd         The commands are:
 *                      F_SETFL       Set the file status associated with the file
 *                                    descriptor fd (arg is interpreted as an UINT8)
 *                      F_GETFL       Get the file status associated with fd (arg is
 *                                    ignored).
 *                      F_SETRETPATH  Sets the return path to be used for notification
 *                                    (arg is interpreted as an pointer to T_RV_RETURN)
 *
 *                      The flags F_SETFL can be as follows:
 *                      O_AIO   Force time consuming call to operate asynchronously,
 *                              the caller will be notified either by the re-turn path
 *                              (callback function or message).
 *                      O_SIO   Default blocking (i.e: synchronous) I/O operations
 * @param   arg         Arguments depending on cmd, Possible parameters are the flag
 *                      (enabling and disabling asynchro-nous operations) and the
 *                      command for setting the return path.
 *
 * @return  ( positive value) Value of flags (in case of successful F_GETFL command
 *                            execution)
 * @return  RFS_EOK           Ok (in case of successful command execution others than
 *                            F_GETFL)
 * @return  RFS_EBADFD        Invalid file descriptor.
 * @return  RFS_EINVALID      Invalid argument (Invalid command or invalid return_path)
 */
extern T_RFS_RET rfs_fcntl(T_RFS_FD   fd,
                           INT8       cmd,
                           void       *arg);

/**
 * rfs_close
 *
 * Detailled description.
 * This function closes an open file.
 *
 * @param   fd          File descriptor obtained when the file was opened.
 *
 * @return  RFS_EOK     Ok
 * @return  RFS_EBADFD  The file argument is not a valid file descriptor.
 */
extern T_RFS_RET rfs_close(T_RFS_FD fd);



extern T_RFS_RET rfs_closedir(T_RFS_DIR *dir_p);

/**
 * rfs_write
 *
 * Detailled description.
 * This function writes data to a previously opened file identified by fd. It
 * writes size bytes of data from the buffer pointed by buf at the current position
 * of the file pointer in the file. When the write operation completes, the current
 * position of the file pointer is set to the end of the newly added data.
 *
 * If the operation succeeds the (positive) number of written bytes is returned.
 * Otherwise an error is re-turned (negative value).
 *
 * @param   fd          File descriptor obtained when the file was opened.
 * @param   buf         Pointer to buffer of data to write.
 * @param   size        Number of bytes to write.
 *
 * @return  (Positive value)  Number of bytes actually written.
 * @return  RFS_EBADFD        The file argument is not a valid file descriptor.
 * @return  RFS_EACCES        The file is not writable.
 * @return  RFS_EBADOP        The file is not open for writing.
 * @return  RFS_EFBIG         An attempt was made to write a file that exceeds the
 *                            maximum file size.
 * @return  RFS_ENOSPACE      Out of data space.
 * @return  RFS_EDEVICE       Device I/O error
 */
extern T_RFS_SIZE rfs_write(T_RFS_FD    fd,
                            const void  *buf,
                            T_RFS_SIZE  size);

/**
 * rfs_read
 *
 * Detailled description.
 * This function reads data from the previously opened file identified by fd.
 * A maximum of size bytes is read from the file into the buffer pointed by buf.
 * The buffer memory needs to be allocated by the client. When the read operation
 * completes, the file pointer is advanced to the end of the data read.
 *
 * If the operation succeeds the (positive) number of bytes read is returned.
 * Otherwise an error is re-turned (negative value).
 *
 * @param   fd          File descriptor obtained when the file was opened.
 * @param   buf         Pointer to a buffer where the data will be copied into
 *                      (The size of this buffer has to be at least size bytes).
 * @param   size        Maximum number of bytes to read.
 *
 * @return  (Positive value)  Number of bytes actually read.
 * @return  RFS_EBADFD        The file argument is not a valid file descriptor.
 * @return  RFS_EACCES        The file is not readable.
 * @return  RFS_EBADOP        The file is not open for reading.
 * @return  RFS_ENOSPACE      Out of data space.
 * @return  RFS_EDEVICE       Device I/O error
 */
extern T_RFS_SIZE rfs_read(T_RFS_FD   fd,
                           void *buf,
                           T_RFS_SIZE size);

/**
 * rfs_lseek
 *
 * Detailled description.
 * This function repositions the offset of the file descriptor fd to the argument
 * offset according to the di-rective whence. The argument fd must be an open file
 * descriptor.
 *
 * If the operation succeeds the (positive) new position of the file pointer is
 * returned. Otherwise an error is returned (negative value).
 *
 * Note:
 * When the whence is set to RFS_SEEK_END the file offset is set to size of the file
 * plus the offset. In this case new blocks will be added to the file and this can
 * take additional time. This means, also de-pending on system load, the blocking
 * version (synchronous operation) this function can block the caller for a time.
 *
 *
 * @param  fd           File descriptor obtained when the file was opened.
 * @param  offset       Offset (in bytes) to move the file pointer.
 * @param  whence       Reference used to reposition the file pointer defined as
 *                      follow:
 *                      RFS_SEEK_SET  Absolute offset from start of file
 *                      RFS_SEEK_CUR  the offset is set to its current location plus
 *                                    offset bytes
 *                      RFS_SEEK_END  the offset is set to the size of the file plus
 *                                    offset bytes.
 *
 * @return  (Positive value)  New position of the file pointer
 * @return  RFS_EBADFD        The fd argument is not a valid file descriptor.
 * @return  RFS_EINVALID      The whence argument is not a proper value, or the
 *                            resulting file offset would be invalid.
 * @return  RFS_EBADOP        Bad operation. Seek not allowed with the flags used to
 *                            open the file.
 * @return  RFS_EDEVICE       Device I/O error
 */
extern T_RFS_OFFSET rfs_lseek(T_RFS_FD     		fd,
                              T_RFS_OFFSET 		offset,
                              INT8   whence);

/**
 * rfs_fchmod
 *
 * Detailled description.
 * The function rfs_fchmod() sets the permission bits of the specified file
 * descriptor fd to required mode.
 * A mode is created from OR'd permission bit masks defined by T_RFS_MODE.
 *
 * @param  fd           File descriptor obtained when the file was opened.
 * @param  mode         Specifies the attribute (permission bits) of the file
 *
 * @return  RFS_EOK           Ok
 * @return  RFS_EBADFD        The fd argument is not a valid file descriptor.
 * @return  RFS_ENAMETOOLONG  Object's name is too long.
 * @return  RFS_EACCES        Search permission is denied for a component of the
 *                            path prefix.
 * @return  RFS_EBADNAME      Object's name contains illegal characters.
 * @return  RFS_ENOTAFILE     Object is not a file.
 * @return  RFS_ENOENT        No such file or directory.
 * @return  RFS_EINVALID      Bad mod option
 * @return  RFS_ELOCKED       The file is locked (already opened in a conflicting
 *                            mode).
 */
extern T_RFS_RET rfs_fchmod(T_RFS_FD    fd,
                            T_RFS_MODE  mode);

/**
 * rfs_fstat
 *
 * Detailled description.
 * The rfs_fstat() function obtains information about an open file associated by the
 * file descriptor fd.
 *
 * @param   fd          File descriptor obtained when the file was opened.
 * @param   stat        Contains information (meta-data) about the specified object.
 *
 * @return  RFS_EOK           Ok.
 * @return  RFS_ENOENT        Object not found.
 * @return  RFS_EBADFD        Bad file descriptor.
 * @return  RFS_EACCES        Search permission is denied for a component of the path
 *                            prefix.
 * @return  RFS_ENAMETOOLONG  Object's name is too long.
 * @return  RFS_EBADNAME      Object's name contains illegal characters.
 */
extern T_RFS_RET rfs_fstat(T_RFS_FD   fd,
                           T_RFS_STAT *stat);

/**
 * rfs_fsync
 *
 * Detailled description.
 * This function cause the transfer of all modified data and attributes of fd, which
 * wasn't immediately written to the storage device, to the permanent storage device
 * associated with the file described by fd.  The reason that sometimes the data is
 * not immediately written on the storage device, can be caused by different reasons,
 * like different buffer sizes that are used by various software and hardware
 * components, task scheduling or hardware delays.
 *
 * To ensure that the data is consistent and really written on the physical media,
 * this function can be used. Saving unwritten data on storage devices is also known
 * as flushing.
 *
 * @param   fd          File descriptor obtained when the file was opened.
 *
 * @return  RFS_EOK       Ok.
 * @return  RFS_EBADFD    Invalid file descriptor.
 * @return  RFS_ENOSPACE  Out of data space.
 * @return  RFS_EFSFULL   File system full, no free inodes.
 * @return  RFS_EDEVICE   Device I/O error
 */
extern T_RFS_RET rfs_fsync(T_RFS_FD fd);

/**
 * rfs_open
 *
 * Detailled description.
 * The file specified by pathname is opened for reading and/or writing as specified
 * by the argument flags and the file descriptor is returned to the calling process.
 * The flags argument may indicate the file is to be created if it does not exist
 * (by specifying the RFS_O_CREAT flag), in which case the file is created with mode
 * mode. Else the mode will be ignored.
 *
 * If the operation succeeds the file pointer used to mark the current position within
 * the file is set to the beginning of the file and a (positive) file descriptor is
 * returned. Otherwise an error is returned (negative value).
 *
 * @param   pathname  Null terminated string containing the unique name of the file
 *                    to open or create.
 * @param   flags     Specifies the attribute used to open the file.
 * @param   mode      Specifies the mode argument (permission bits of the file) and
 *                    will be used when the RFS_O_CREAT flag is specified in flags.
 *
 * @return  (Positive value)  File descriptor of file opened.
 * @return  RFS_EEXISTS       An object of the same name already exists.
 * @return  RFS_EACCES        - Search permission is denied for a component of the
 *                              path prefix.
 *                            - or the required permissions (for reading and/or
 *                              writing) are denied for the given flags.
 *                            - or RFS_O_CREAT is specified, the file does not exist,
 *                              and the directory in which it  is to be created does
 *                              not permit writing.
 * @return  RFS_ENAMETOOLONG  Object's name is too long.
 * @return  RFS_EBADNAME      Object's name contains illegal characters.
 * @return  RFS_ENUMFD        Max number of used file descriptors reach
 * @return  RFS_ENOENT        No such file or directory.
 * @return  RFS_EINVALID      Bad open flag options.
 * @return  RFS_ELOCKED       The file is locked (already opened for writing, in a
 *                            conflicting mode).
 * @return  RFS_EMOUNT        Invalid mount point
 */
extern T_RFS_FD rfs_open(const T_WCHAR *pathname,
                         T_RFS_FLAGS  flags,
                         T_RFS_MODE   mode);

/**
 * rfs_open_nb
 *
 * Detailled description.
 * This is a non-blocking function variant of rfs_open(). For a detailed description
 * see according function.
 *
 * @param   pathname    Null terminated string containing the unique name of the file
 *                      to open or create.
 * @param   flags       Specifies the attribute used to open the file.
 * @param   mode        Specifies the mode argument (permission bits of the file) and
 *                      will be used when the RFS_O_CREAT flag is specified in flags.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a
 *                            received response.
 * @return  RFS_EACCES        The RFS is not able to handle this request at this moment.
 * @return  RFS_EMEMORY       Insufficient memory to create message request
 * @return  RFS_EINVALID      Invalid argument (return_path is invalid)
 */
extern T_RFS_RET rfs_open_nb(const T_WCHAR    *pathname,
                             T_RFS_FLAGS  flags,
                             T_RFS_MODE   mode,
                             T_RV_RETURN   return_path);

/**
 * rfs_chmod
 *
 * Detailled description.
 * The function rfs_chmod() sets the file permission bits of the file specified by
 * the pathname pathname to required mode.
 *
 * A mode is created from OR'd permission bit masks defined by T_RFS_MODE.
 *
 * @param   pathname    Null terminated string containing the unique name of the
 *                      file for changing the mode.
 * @param   mode        Specifies the attribute (permission bits) of the file
 *
 * @return  RFS_EOK           Ok
 * @return  RFS_EBADFD        The fd argument is not a valid file descriptor.
 * @return  RFS_ENAMETOOLONG  Object's name is too long.
 * @return  RFS_EACCES        Search permission is denied for a component of the path
 *                            prefix.
 * @return  RFS_EBADNAME      Object's name contains illegal characters.
 * @return  RFS_ENOTAFILE     Object is not a file.
 * @return  RFS_ENOENT        No such file or directory.
 * @return  RFS_EINVALID      Bad mod option
 * @return  RFS_ELOCKED       The file is locked (already opened in conflicting mode).
 * @return  RFS_EMOUNT        Invalid mount point
 */
extern T_RFS_RET  rfs_chmod(const T_WCHAR  *pathname,
                            T_RFS_MODE  mode);

/**
 * rfs_chmod_nb
 *
 * Detailled description.
 * This is a non-blocking function variant of rfs_chmod(). For a detailed description
 * see according function.
 *
 * @param   pathname    Null terminated string containing the unique name of the
 *                      file for changing the mode.
 * @param   mode        Specifies the attribute (permission bits) of the file
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a
 *                            received response.
 * @return  RFS_EACCES        The RFS is not able to handle this request at this moment.
 * @return  RFS_EMEMORY       Insufficient memory to create message request
 * @return  RFS_EINVALID      Invalid argument (return_path is invalid)
 */
extern T_RFS_RET rfs_chmod_nb(const T_WCHAR   *pathname,
                              T_RFS_MODE   mode,
                              T_RV_RETURN  return_path);

/**
 * rfs_stat
 *
 * Detailled description.
 * The rfs_stat() function obtains information about the file or device associated to
 * the mountpoint pointed to by pathname.
 *
 * If pathname is NULL, general information about the file system is returned (e.g.
 * default mountpoint and file system limits). If pathname is '/mountpoint',
 * information on the device associated to the mount point is returned (e.g. speed
 * data of the mount pount). if pathname ends with a directory or file, the
 * appropriate information for the directory or file is returned.
 *
 * @param   stat      Contains information (meta-data) about the specified object.
 * @param   pathname  Terminated string containing NULL, the name of the mountpount,
 *                    file or directory.
 *
 * @return  RFS_EOK           Ok.
 * @return  RFS_ENOENT        Object not found.
 * @return  RFS_EBADFD        Bad file descriptor.
 * @return  RFS_EACCES        Search permission is denied for a component of the path
 *                            prefix.
 * @return  RFS_EMOUNT        Invalid mount point
 * @return  RFS_ENAMETOOLONG  Object's name is too long.
 * @return  RFS_EBADNAME      Object's name contains illegal characters.
 */
extern T_RFS_RET rfs_stat(const T_WCHAR  *pathname,
                          T_RFS_STAT  *stat);

/**
 * rfs_stat_nb
 *
 * Detailled description.
 * This is a non-blocking function variant of rfs_stat(). For a detailed description
 * see according function.
 *
 * @param   stat        Contains information (meta-data) about the specified object.
 * @param   pathname    Terminated string containing NULL, the name of the mountpount,
 *                      file or directory.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a
 *                            received response.
 * @return  RFS_EACCES        The RFS is not able to handle this request at this moment.
 * @return  RFS_EMEMORY       Insufficient memory to create message request
 * @return  RFS_EINVALID      Invalid argument (return_path is invalid)
 */
extern T_RFS_RET rfs_stat_nb(const T_WCHAR  *pathname,
                             T_RFS_STAT  *stat,
                             T_RV_RETURN return_path);
/**
 * rfs_remove
 *
 * Detailled description.
 * This function removes the object with the pathname given by pathname. The pathname
 * is a null termi-nated string. If the object does not exist, RFS_ENOENT is returned.
 * If a directory is to be removed, it must be empty, otherwise RFS_ENOTEMPTY is
 * returned. It is not possible to remove a file that is open.
 *
 * @param   pathname  Null terminated string containing the unique name of the object.
 *
 * @return  RFS_EOK           Ok.
 * @return  RFS_ENOENT        File was not found.
 * @return  RFS_ENAMETOOLONG  Object's name is too long.
 * @return  RFS_EBADNAME      Object's name contains illegal characters.
 * @return  RFS_EACCES        Search permission is denied for a component of the path
 *                            prefix.
 * @return  RFS_EACCES        File could not be removed (read-only).
 * @return  RFS_ELOCKED       The file is open
 * @return  RFS_EDEVICE       Device I/O error
 * @return  RFS_EMOUNT        Invalid mount point
 * @return  RFS_ENOTEMPTY     The named directory contains files other than '.' and
 *                            '..' in it.
 */
extern T_RFS_RET rfs_remove(const T_WCHAR *pathname);

/**
 * rfs_remove_nb
 *
 * Detailled description.
 * This is a non-blocking function variant of rfs_remove(). For a detailed description
 * see according function.
 *
 * @param   pathname  Null terminated string containing the unique name of the object.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a
 *                            received response.
 * @return  RFS_EACCES        The RFS is not able to handle this request at this moment.
 * @return  RFS_EMEMORY       Insufficient memory to create message request
 * @return  RFS_EINVALID      Invalid argument (return_path is invalid)
 */
extern T_RFS_RET rfs_remove_nb(const T_WCHAR  *pathname,
                               T_RV_RETURN return_path);


/**
 * rfs_rename
 *
 * Detailled description.
 * This function renames files and directories. The names are the full path to the
 * object. It is possible to move the object to a different path simple by specifying
 * a new path in the newname string. The old-name object must exist and the newname
 * must not existe or else an error will be returned.
 *
 * Renaming (moving) of a file is only granted on the same mountpoint (media
 * partition). If the new path indicates another mountpoint an error is returned.
 *
 * @param   oldname   Null terminated string containing the unique name including the
 *                    path of the existing object in the File System.
 * @param   newname   Null terminated string containing the unique name including the
 *                    path, which oldname is desired to change name or location to.
 *
 * @return  RFS_EOK           Ok.
 * @return  RFS_ENOENT        oldname  object does not exist.
 * @return  RFS_EEXISTS       newname object already exists.
 * @return  RFS_EACCES        Object could not be modified (read-only).
 * @return  RFS_ENAMETOOLONG  Object's name is too long.
 * @return  RFS_EBADNAME      Object's name contains illegal characters.
 * @return  RFS_EFSFULL       Failed to allocate an inode for the changed object.
 * @return  RFS_ENOTALLOWED   Renaming is not allowed (another new path contains
 *                            another mountpoint)
 */
extern T_RFS_RET rfs_rename(const T_WCHAR  *oldname,
                            const T_WCHAR  *newname);

/**
 * rfs_rename_nb
 *
 * Detailled description.
 * This is a non-blocking function variant of rfs_rename(). For a detailed description
 * see according function.
 *
 * @param   oldname     Null terminated string containing the unique name including
 *                      the path of the existing object in the File System.
 * @param   newname     Null terminated string containing the unique name including the
 *                      path, which oldname is desired to change name or location to.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a
 *                            received response.
 * @return  RFS_EACCES        The RFS is not able to handle this request at this moment.
 * @return  RFS_EMEMORY       Insufficient memory to create message request
 * @return  RFS_EINVALID      Invalid argument (return_path is invalid)
 */
extern T_RFS_RET rfs_rename_nb(const T_WCHAR  *oldname,
                               const T_WCHAR  *newname,
                               T_RV_RETURN return_path);
/**
 * rfs_set_label_nb
 *
 * Detailled description.
 * @param   pathname    Null terminated string containing the unique name of the
 *                      directory to create
 * @param   mode        Specifies the attribute (permission bits) of the directory
 *
 * @return  RFS_EOK Ok.

 */
extern T_RFS_RET rfs_set_label_nb(const T_WCHAR *mpt_name,  const T_WCHAR  *label, 
                        T_RV_RETURN return_path);

/**
 * rfs_get_label_nb
 *
 * Detailled description.
 * @param   pathname    Null terminated string containing the unique name of the
 *                      directory to create
 * @param   mode        Specifies the attribute (permission bits) of the directory
 *
 * @return  RFS_EOK Ok.

 */
extern T_RFS_RET rfs_get_label_nb(const T_WCHAR *mpt_name,  T_WCHAR *label, 
                        T_RV_RETURN return_path);

/**
 * rfs_mkdir
 *
 * Detailled description.
 * This function creates a directory with the pathname given by pathname. The
 * pathname is a null termi-nated string. All components of the pathname must be
 * already existing directories. This means that it is not possible to
 * rfs_mkdir("/gsm/rf/tx") if the directories /gsm/ and /gsm/rf are not already
 * created.
 *
 * @param   pathname    Null terminated string containing the unique name of the
 *                      directory to create
 * @param   mode        Specifies the attribute (permission bits) of the directory
 *
 * @return  RFS_EOK Ok.
 * @return  RFS_EEXISTS Directory already exists.
 * @return  RFS_ENAMETOOLONG  Object's name is too long.
 * @return  RFS_EACCES  Search permission is denied for a component of the path prefix.
 * @return  RFS_EBADNAME  Name of the directory contains illegal characters
 * @return  RFS_ENOSPACE  Failed to allocate space for object's data.
 * @return  RFS_EFSFULL Failed to allocate an inode for the object.
 * @return  RFS_EDEVICE Device I/O error
 * @return  RFS_EMOUNT  Invalid mount point
 */
extern T_RFS_RET rfs_mkdir(const T_WCHAR  *pathname,
                           T_RFS_MODE  mode);

/**
 * rfs_mkdir_nb
 *
 * Detailled description.
 * This is a non-blocking function variant of rfs_mkdir(). For a detailed description
 * see according function.
 *
 * @param   pathname    Null terminated string containing the unique name of the
 *                      directory to create
 * @param   mode        Specifies the attribute (permission bits) of the directory
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a
 *                            received response.
 * @return  RFS_EACCES        The RFS is not able to handle this request at this moment.
 * @return  RFS_EMEMORY       Insufficient memory to create message request
 * @return  RFS_EINVALID      Invalid argument (return_path is invalid)
 */
extern T_RFS_RET rfs_mkdir_nb(const T_WCHAR   *pathname,
                              T_RFS_MODE   mode,
                              T_RV_RETURN  return_path);

/**
 * rfs_rmdir
 *
 * Detailled description.
 * This function removes a directory file whose name is given by pathname. The
 * directory must be empty.
 *
 * @param   pathname    Null terminated string containing the unique name of the
 *                      directory to remove.
 *
 * @return  RFS_EOK           Ok.
 * @return  RFS_ENOENT        The named directory does not exist
 * @return  RFS_ENAMETOOLONG  Object's name is too long.
 * @return  RFS_ENOTEMPTY     The named directory contains files other than '.' and
 *                            '..' in it.
 * @return  RFS_EACCES        Search permission is denied for a component of the path
 *                            prefix.
 * @return  RFS_EACCES        No write permission to delete the directory entry
 * @return  RFS_EBUSY         The directory to be removed is the mount point for a
 *                            mounted file system or the current directory.
 * @return  RFS_EBADNAME      Name of the directory contains illegal characters
 * @return  RFS_EMOUNT        Invalid mount point
 */
extern T_RFS_RET rfs_rmdir(const T_WCHAR *pathname);

/**
 * rfs_rmdir_nb
 *
 * Detailled description.
 * This is a non-blocking function variant of rfs_rmdir(). For a detailed description
 * see according function.
 *
 * @param   pathname    Null terminated string containing the unique name of the
 *                      directory to remove.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a
 *                            received response.
 * @return  RFS_EACCES        The RFS is not able to handle this request at this moment.
 * @return  RFS_EMEMORY       Insufficient memory to create message request
 * @return  RFS_EINVALID      Invalid argument (return_path is invalid)
 */
extern T_RFS_RET rfs_rmdir_nb(const T_WCHAR *pathname,
                              T_RV_RETURN return_path);

/**
 * rfs_opendir
 *
 * Detailled description.
 * This function opens the directory named by pathname, associates a directory stream
 * with it, and re-turns a pointer to be used to identify the directory stream in
 * subsequent operations.
 *
 * If the operation succeeds the (positive) pointer to the directory structure is
 * returned. Otherwise an error is returned (negative value).
 *
 * @param   pathname    Null terminated string containing the unique name of the
 *                      directory we want to open.
 *
 * @return  (Positive value)  pointer to the directory structure
 * @return  RFS_ENOENT        Directory not found.
 * @return  RFS_EACCES        Search permission is denied for a component of the path
 *                            prefix.
 * @return  RFS_ENAMETOOLONG  Object's name is too long.
 * @return  RFS_EBADNAME      Name of the directory contains illegal characters
 * @return  RFS_ENOSPACE      Out of data space.
 * @return  RFS_EMOUNT        Invalid mount point
 */
extern T_RFS_SIZE rfs_opendir(const T_WCHAR  *pathname,
                               T_RFS_DIR *dirp);


/**
 * rfs_opendir_nb
 *
 * Detailled description.
 * This is a non-blocking function variant of rfs_opendir(). For a detailed description
 * see according function.
 *
 * @param   pathname    Null terminated string containing the unique name of the
 *                      directory we want to open.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a
 *                            received response.
 * @return  RFS_EACCES        The RFS is not able to handle this request at this moment.
 * @return  RFS_EMEMORY       Insufficient memory to create message request
 * @return  RFS_EINVALID      Invalid argument (return_path is invalid)
 */
extern T_RFS_RET rfs_opendir_nb(const T_WCHAR  *pathname,
                                T_RFS_DIR   *dirp,
                                T_RV_RETURN return_path);

/**
 * rfs_readdir
 *
 * Detailled description.
 * This function reads an entry from a directory previously opened by rfs_opendir().
 * The input parameter is a pointer to the opened directory stream (specified by the
 * argument dirp). The structure associated with the pointer dirp, keeps track of the
 * directory entry last read by the function rfs_readdir(). To achieve this, the
 * pointer dirp is also an output parameter.
 *
 * The other (output) parameters concern directory entry information, which is read.
 * This information exists of a pointer to a buffer containing the name of the entry
 * and the size of the buffer. The buffer memory needs to be allocated by the client.
 *
 * A positive return value denotes that the buffer pointed to by buf, contains the
 * null-terminated name of the entry found. A zero is returned if there were no more
 * entries in the directory and the buffer pointed to by buf is left untouched. The
 * function returns a negative value if an exception is encountered.
 *
 * In order to read all entries in a directory, rfs_readdir() should be called until
 * it returns zero.
 *
 * @param   dirp      Pointer to a T_RFS_DIR structure obtained in a previous call to
 *                    rfs_opendir(), representing a directory stream.
 * @param   buf       Pointer to a buffer, which should contain the name of the
 *                    directory entry.
 * @param   size      Size in bytes of the buffer pointed by buf.
 *
 * @return  (Positive value)  Number of bytes actually read.
 * @return  RFS_EBADDIR       Invalid directory descriptor
 * @return  RFS_ENOSPACE      Out of data space.
 */
extern T_RFS_SIZE rfs_readdir(T_RFS_DIR     *dirp,
                              T_WCHAR    *buf,
                              T_RFS_SIZE    size);

/**
 * rfs_readdir_nb
 *
 * Detailled description.
 * This is a non-blocking function variant of rfs_readdir(). For a detailed description
 * see according function.
 *
 * @param   dirp        Pointer to a T_RFS_DIR structure obtained in a previous call to
 *                      rfs_opendir(), representing a directory stream.
 * @param   buf         Pointer to a buffer, which should contain the name of the
 *                      directory entry.
 * @param   size        Size in bytes of the buffer pointed by buf.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a
 *                            received response.
 * @return  RFS_EACCES        The RFS is not able to handle this request at this moment.
 * @return  RFS_EMEMORY       Insufficient memory to create message request
 * @return  RFS_EINVALID      Invalid argument (return_path is invalid)
 */
extern T_RFS_RET rfs_readdir_nb(T_RFS_DIR    *dirp,
                                T_WCHAR*buf,
                                T_RFS_SIZE   size,
                                T_RV_RETURN  return_path);

/**
 * rfs_preformat
 *
 * Detailled description.
 * This function erases all data in RFS. With this pre-format function it is only
 * possible to erase the data in a media partition. Within RFS a media partition is
 * the same as a mountpoint. For this pre-format function it is required that the
 * given pathname should be a mountpunt '/mountpoint'. If this is not the case, no
 * pre-formatting takes place and an error is returned.
 *
 * The pre-format operation cannot be reversed or undone. Note that depending on the
 * underlying flash hardware, the pre-format operation can take anything from a few
 * milliseconds to several seconds. Most flash memories in a normal environment take
 * around one second (typical) to erase each sector. The magic number must equal the
 * hexadecimal constant 0xDEAD. If the magic number given by magic is incorrect,
 * RFS_EMAGIC is returned.
 *
 * @param   pathname    Null terminated string containing the name of the partition
 *                      to format.
 * @param   magic       Magic value to access the function. Must be 0xDEAD.
 *
 * @return  RFS_EOK         Ok.
 * @return  RFS_EMAGIC      Magic number is incorrect.
 * @return  RFS_EINVALID    An erase operation is currently in progress. Retry the operation again later.
 * @return  RFS_ENODEVICE   The flash device is unknown (not supported).
 * @return  RFS_EMEMORY     Message allocation failed.
 * @return  RFS_EMSGSEND    Message sending failed.
 * @return  RFS_EDEVICE     Device I/O error
 * @return  RFS_ENOTALLOWED Pre-formatting is not allowed (pathname is no mountpoint)
 */
extern T_RFS_RET rfs_preformat(const T_WCHAR *pathname,
                               UINT16     magic);

/**
 * rfs_preformat_nb
 *
 * Detailled description.
 * This is a non-blocking function variant of rfs_preformat(). For a detailed
 * description see according function.
 *
 * @param   pathname    Null terminated string containing the name of the partition
 *                      to format.
 * @param   magic       Magic value to access the function. Must be 0xDEAD.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a
 *                            received response.
 * @return  RFS_EACCES        The RFS is not able to handle this request at this moment.
 * @return  RFS_EMEMORY       Insufficient memory to create message request
 * @return  RFS_EINVALID      Invalid argument (return_path is invalid)
 */
extern T_RFS_RET rfs_preformat_nb(const T_WCHAR  *pathname,
                                  UINT16      magic,
                                  T_RV_RETURN return_path);

/**
 * rfs_format
 *
 * Detailled description.
 * This function formats the RFS. With this format function a new file system is
 * created on the requested media partition. Within RFS, a media partition is the
 * same as a mount point. For this format function it is required that the given
 * pathname should be a mount punt '/mountpoint'. If this is not the case, no
 * formatting takes place and an error is returned.
 *
 * With the optional name given by the argument name the volume name of the root
 * directory. However the name is completely ignored and has absolutely no meaning
 * to RFS. It can not be read or retrieved later on. If name is the null pointer,
 * a default name is used. Otherwise name is a null terminated string and optionally
 * followed by some arbitrary descriptive name for the RFS volume.
 *
 * This function must be called after rfs_preformat() and before any other operation
 * on RFS. In order to avoid spurious calls of this dangerous, unrecoverable function,
 * the magic number must have the hexadecimal value of 0x2BAD to format the flash.
 *
 * @param   pathname    Null terminated string containing the name of the partition
 *                      to format.
 * @param   name        Null terminated string containing the name of the RFS volume.
 * @param   magic       Magic value to access the function. Must be 0x2BAD.
 *
 * @return  RFS_EOK         Ok.
 * @return  RFS_EAGAIN      Previous RFS_preformat() has not finished yet.
 * @return  RFS_EINVALID    Magic number is incorrect.
 * @return  RFS_EBADNAME    Name contains illegal characters
 * @return  RFS_EMEMORY     Message allocation failed.
 * @return  RFS_EMSGSEND    Message sending failed.
 * @return  RFS_EDEVICE     Device I/O error
 * @return  RFS_ENOTALLOWED Formatting is not allowed (pathname is no mountpoint)
 */
extern T_RFS_RET  rfs_format(const T_WCHAR *pathname,
                             const T_WCHAR *name,
                             UINT16     magic);

/**
 * rfs_format_nb
 *
 * Detailled description.
 * This is a non-blocking function variant of rfs_format(). For a detailed description
 * see according function.
 *
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a
 *                            received response.
 * @return  RFS_EACCES        The RFS is not able to handle this request at this moment.
 * @return  RFS_EMEMORY       Insufficient memory to create message request
 * @return  RFS_EINVALID      Invalid argument (return_path is invalid)
 */
extern T_RFS_RET rfs_format_nb(const T_WCHAR  *pathname,
                               const T_WCHAR  *name,
                               UINT16      magic,
                               T_RV_RETURN return_path);

extern T_RFS_RET rfs_getattribute(const T_WCHAR * pathname, T_FFS_ATTRIB_TYPE * attr);
extern T_RFS_RET rfs_setattribute(const T_WCHAR * pathname, T_FFS_ATTRIB_TYPE  attr);
extern T_RFS_RET rfs_getdatetime(const T_WCHAR *pathname,T_RTC_DATE_TIME *date_time_p,T_RTC_DATE_TIME *crdate_time_p);
extern T_RFS_RET rfs_unmount(T_RFS_FSTYPE fs_type);
extern T_RFS_RET rfs_mount(T_RFS_FSTYPE fs_type);



extern T_RFS_RET rfs_send_message(T_RV_HDR * msg_p);
/* Unicode and string operations */
extern T_RFS_RET wstrcmp (const T_WCHAR *str1,const T_WCHAR* str2);
extern void convert_unicode_to_u8(const T_WCHAR *pathname, char *mp);
extern void convert_u8_to_unicode(const char *mp, T_WCHAR *mp_uc);
extern void convert_unicode_to_u8_length(const T_WCHAR *pathname, char *mp,UINT32 length);
extern void convert_u8_to_unicode_length(const char *mp, T_WCHAR *mp_uc, UINT32 length);
extern T_WCHAR * wstrchr(const T_WCHAR *s, int c);
extern UINT16  wstrlen(const T_WCHAR  *str_uc);
extern void  wstrcpy(  T_WCHAR *dest, const T_WCHAR *src);
extern void  wstrncpy(  T_WCHAR *dest, const T_WCHAR *src,UINT16 n);
extern T_WCHAR * wstrcat(T_WCHAR *dest, const T_WCHAR *src);
T_WCHAR *wstrrchr(const T_WCHAR *s, int c);




void create_full_path (char *path, const char *dir, const char *fname);

/*@}*/

#ifdef __cplusplus
}
#endif


#endif /*__RFS_API_H_*/

