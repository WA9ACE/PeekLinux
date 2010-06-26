/**
 * @file	rfsfat_api.h
 *
 * API Definition for RFSFAT SWE.
 *
 * @author	Anton van Breemen (abreemen@ict.nl)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/29/2004	Anton van Breemen (abreemen@ict.nl)		Create.
 *	2/19/2004	E. Oude Middendorp, translation to official fs core api 
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */

#ifndef __RFSFAT_API_H_
#define __RFSFAT_API_H_

#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */
#include "rtc/rtc_api.h"
#include "rfs/rfs_message.h"
#include "rfs/fscore_types.h"
#include "rfsfat/rfsfat_ffs.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define RFSFAT_MAX_RET_ENTRIES			0x7FFFFFFE




/**
 * rfsfat_set_partition_table
 *
 * Detailled description.
 * This function informs the according FS-core of a new partition table (list of
 * partition information). The parameter partition_table is a pointer to the
 * memory where the appropriate partition table information is stored. 
 *
 * The function returns immediately. Only the return_path parameter is verified
 * immediately. Processing is done asynchronous and the result is returned through
 * the return path. 
 *
 * When a null configuration is set, there is no partition information available
 * anymore (e.g. when the media is removed). In this case the partition table
 * contains a number of partitions with the value zero and the pointer to the
 * partition information is null. 
 *
 * To be able to pair the response message with the request (This request can be
 * handled by different FS-cores simultaneously), the response message contains
 * the pointer partition_table to the partition.
 * 
 * @param   partition_table: Pointer to the stored partition table data
 * @param   return_path:     This is the return path information of the client.
 *                           The structure provides information about the way the
 *                           FS-core must react asynchronous (using a call-back
 *                           principle or a return message).
 *
 * @return	FSCORE_EOK       Ok
 * @return  FSCORE_EACCES    Not able to handle this request at this moment 
 *                            (SWE not initialised)..
 * @return  FSCORE_EINVALID  Invalid argument (Invalid command or invalid return_path)
 * @return  FSCORE_EMEMORY   Insufficient RAM resources to process the request.
 */
  T_FSCORE_RET rfsfat_set_partition_table (T_FSCORE_PARTITION_TABLE *
										   partition_table,
										   T_RV_RETURN return_path);

/**
 * rfsfat_get_func_table
 *
 * Detailled description.
 * This function returns a pointer to the FS-core function table.
 * This function is synchronous.
 *
 * @param   func_table_p:     A pointer to the function table structure. 
 *
 * @return	FSCORE_EOK        Ok
 * @return  FSCORE_EACCES     Not able to handle this request at this moment 
 *                            (SWE not initialised)..
 * @return  FSCORE_EINVALID   Invalid argument (Invalid command or invalid return_path)
 **/
  T_FSCORE_RET rfsfat_get_func_table (T_FSCORE_FUNC_TABLE * func_table_p);



/**
 * rfsfat_fcntl
 *
 * Detailled description.
 * This function provides control on the properties of a file that is already open. 
 * The argument fd is a descriptor to be operated on by cmd as described below. 
 * The third parameter is called arg and is technically a pointer to void, but the 
 * interpretation depends on the command.
 *
 * This function is a synchronous function. Switching from synchronous to asynchronous 
 * operations or vice-versa, by setting flag F_SETFL, has only effect on the succeding
 * operations (like file writing or reading) and not on the function rfsfat_fcntl() 
 * itself. When a switch from asynchronous operations to syn-chronous operations is 
 * made and there are some pending asynchronous operations, the return_path of these
 * pending operations should stay valid. 
 *
 * @param   fd          File descriptor obtained when the file was opened.
 * @param   cmd         The commands are:
 *                      F_SETFL       Set the file status associated with the file 
 *                                    descriptor fd (arg is interpreted as an UINT8)
 *                      F_GETFL	      Get the file status associated with fd (arg is 
 *                                    ignored).
 *                      F_SETRETPATH	Sets the return path to be used for notification
 *                                    (arg is interpreted as an pointer to T_RV_RETURN)
 *
 * @param   arg_p       -In case parameter is F_SETFL the argument is *UINT8:
 *                      O_AIO	  Force time consuming call to operate asynchronously, 
 *                              the caller will be notified either by the re-turn path
 *                              (callback function or message).
 *                      O_SIO		Default blocking (i.e: synchronous) I/O operations
 *                      -In case parameter F_GETFL: no argument required(return value)
 *                      -In case parameter is F_SETRETPATH the argument is *T_RV_RETURN:
 *                       The Riviera return call/message information
 *
 * @return	( positive value)	Value of flags (in case of successful F_GETFL command
 *                            execution)
 * @return	FSCORE_EOK        Ok (in case of successful command execution others than 
 *                            F_GETFL)
 * @return  FSCORE_EBADFD     Invalid file descriptor.
 * @return  FSCORE_EINVALID   Invalid argument (Invalid command or invalid return_path)
 */
  extern T_FSCORE_RET rfsfat_fcntl (T_FSCORE_FD fd, INT8 cmd, void *arg_p);

/**
 * rfsfat_close
 *
 * Detailled description.
 * This function closes an open file. 
 *
 * @param   fd              File descriptor obtained when the file was opened.
 *
 * @return	FSCORE_EOK      Ok
 * @return  FSCORE_EBADFD   The file argument is not a valid file descriptor.
 */
  extern T_FSCORE_RET rfsfat_close (T_FSCORE_FD fd);
/**
 * rfsfat_close_nb
 *
 * Detailled description.
 * This function closes an open file. 
 *
 * @param   fd              File descriptor obtained when the file was opened.
 * @param   return_path		return path to send the results back 
 *
 * @return	FSCORE_EOK      Ok
 * @return  FSCORE_EBADFD   The file argument is not a valid file descriptor.
 */
  extern T_FSCORE_RET rfsfat_close_nb (T_FSCORE_FD fd,T_RV_RETURN_PATH return_path);

/**
 * rfsfat_write
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
 * @return	(Positive value)	  Number of bytes actually written.
 * @return  FSCORE_EBADFD	      The file argument is not a valid file descriptor.
 * @return  FSCORE_EACCES	      The file is not writable.
 * @return  FSCORE_EBADOP	      The file is not open for writing.
 * @return  FSCORE_EFBIG	      An attempt was made to write a file that exceeds the 
 *                              maximum file size. 
 * @return  FSCORE_ENOSPACE	    Out of data space.
 * @return  FSCORE_EDEVICE	    Device I/O error
 */
  extern T_FSCORE_SIZE rfsfat_write (T_FSCORE_FD fd,
									 const void *buf, T_FSCORE_SIZE size);

/**
 * rfsfat_write_nb
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
 * @param   return_path	return path to send the result
 *
 * @return	(Positive value)	  Number of bytes actually written.
 * @return  FSCORE_EBADFD	      The file argument is not a valid file descriptor.
 * @return  FSCORE_EACCES	      The file is not writable.
 * @return  FSCORE_EBADOP	      The file is not open for writing.
 * @return  FSCORE_EFBIG	      An attempt was made to write a file that exceeds the 
 *                              maximum file size. 
 * @return  FSCORE_ENOSPACE	    Out of data space.
 * @return  FSCORE_EDEVICE	    Device I/O error
 */
  extern T_FSCORE_SIZE rfsfat_write_nb (T_FSCORE_FD fd,
									 const void *buf, T_FSCORE_SIZE size,T_RV_RETURN_PATH return_path);
  
/**
 * rfsfat_read
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
 * @return	(Positive value)    Number of bytes actually read.
 * @return	FSCORE_EBADFD	      The file argument is not a valid file descriptor.
 * @return	FSCORE_EACCES	      The file is not readable.
 * @return	FSCORE_EBADOP	      The file is not open for reading.
 * @return	FSCORE_ENOSPACE	    Out of data space.
 * @return	FSCORE_EDEVICE	    Device I/O error
 */
  extern T_FSCORE_SIZE rfsfat_read (T_FSCORE_FD fd,
									void *buf, T_FSCORE_SIZE size);
/**
 * rfsfat_read_nb
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
 * @param   return_path	return path to send the result
 *
 * @return	(Positive value)    Number of bytes actually read.
 * @return	FSCORE_EBADFD	      The file argument is not a valid file descriptor.
 * @return	FSCORE_EACCES	      The file is not readable.
 * @return	FSCORE_EBADOP	      The file is not open for reading.
 * @return	FSCORE_ENOSPACE	    Out of data space.
 * @return	FSCORE_EDEVICE	    Device I/O error
 */
  extern T_FSCORE_SIZE rfsfat_read_nb (T_FSCORE_FD fd,
									   void *buf, T_FSCORE_SIZE size,T_RV_RETURN_PATH return_path);
/**
 * rfsfat_lseek
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
 * When the whence is set to FSCORE_SEEK_END the file offset is set to size of the file
 * plus the offset. In this case new blocks will be added to the file and this can 
 * take additional time. This means, also de-pending on system load, the blocking 
 * version (synchronous operation) this function can block the caller for a time.
 *
 * 
 * @param  fd           File descriptor obtained when the file was opened.
 * @param  offset       Offset (in bytes) to move the file pointer.
 * @param  whence       Reference used to reposition the file pointer defined as 
 *                      follow:
 *                      FSCORE_SEEK_SET	Absolute offset from start of file
 *                      FSCORE_SEEK_CUR	the offset is set to its current location plus
 *                                    offset bytes
 *                      FSCORE_SEEK_END	the offset is set to the size of the file plus
 *                                    offset bytes.
 *
 * @return  (Positive value)  New position of the file pointer
 * @return  FSCORE_EBADFD	    The fd argument is not a valid file descriptor.
 * @return  FSCORE_EINVALID	  The whence argument is not a proper value, or the 
 *                            resulting file offset would be invalid.
 * @return  FSCORE_EBADOP	    Bad operation. Seek not allowed with the flags used to
 *                            open the file.
 * @return  FSCORE_EDEVICE	  Device I/O error
 */
  extern T_FSCORE_OFFSET rfsfat_lseek (T_FSCORE_FD fd,
									   T_FSCORE_OFFSET offset,
									   T_FSCORE_WHENCE whence);

/**
 * rfsfat_lseek_nb
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
 * When the whence is set to FSCORE_SEEK_END the file offset is set to size of the file
 * plus the offset. In this case new blocks will be added to the file and this can 
 * take additional time. This means, also de-pending on system load, the blocking 
 * version (synchronous operation) this function can block the caller for a time.
 *
 * 
 * @param  fd           File descriptor obtained when the file was opened.
 * @param  offset       Offset (in bytes) to move the file pointer.
 * @param  whence       Reference used to reposition the file pointer defined as 
 *                      follow:
 *                      FSCORE_SEEK_SET	Absolute offset from start of file
 *                      FSCORE_SEEK_CUR	the offset is set to its current location plus
 *                                    offset bytes
 *                      FSCORE_SEEK_END	the offset is set to the size of the file plus
 *                                    offset bytes.
 * @param   return_path	return path to send the result
 *
 * @return  (Positive value)  New position of the file pointer
 * @return  FSCORE_EBADFD	    The fd argument is not a valid file descriptor.
 * @return  FSCORE_EINVALID	  The whence argument is not a proper value, or the 
 *                            resulting file offset would be invalid.
 * @return  FSCORE_EBADOP	    Bad operation. Seek not allowed with the flags used to
 *                            open the file.
 * @return  FSCORE_EDEVICE	  Device I/O error
 */
  extern T_FSCORE_OFFSET rfsfat_lseek_nb (T_FSCORE_FD fd,
									   T_FSCORE_OFFSET offset,
									   T_FSCORE_WHENCE whence,T_RV_RETURN_PATH return_path);


extern T_FSCORE_RET
rfsfat_fsync_nb (T_FSCORE_FD fd,   T_RV_RETURN_PATH return_path);
/** * rfsfat_fchmod
 *
 * Detailled description.
 * The function rfsfat_fchmod() sets the permission bits of the specified file 
 * descriptor fd to required mode.
 * A mode is created from OR'd permission bit masks defined by T_FSCORE_MODE.
 * 
 * @param  fd           File descriptor obtained when the file was opened.
 * @param  mode         Specifies the attribute (permission bits) of the file
 *
 * @return  FSCORE_EOK	        Ok
 * @return  FSCORE_EBADFD	      The fd argument is not a valid file descriptor.
 * @return  FSCORE_ENAMETOOLONG	Object's name is too long.
 * @return  FSCORE_ENOTDIR	    A component of the path prefix is not a directory.
 * @return  FSCORE_EACCES	      Search permission is denied for a component of the 
 *                              path prefix.
 * @return  FSCORE_EBADNAME     Object's name contains illegal characters.
 * @return  FSCORE_ENOTAFILE	  Object is not a file.
 * @return  FSCORE_ENOENT	      No such file or directory.
 * @return  FSCORE_EINVALID	    Bad mod option
 * @return  FSCORE_ELOCKED	    The file is locked (already opened in a conflicting 
 *                              mode).
 */
  extern T_FSCORE_RET rfsfat_fchmod (T_FSCORE_FD fd, T_FSCORE_MODE mode);

/**
 * rfsfat_fstat
 *
 * Detailled description.
 * The rfsfat_fstat() function obtains information about an open file associated by the 
 * file descriptor fd.
 *
 * @param   fd          File descriptor obtained when the file was opened.
 * @param   stat        Contains information (meta-data) about the specified object.
 *
 * @return  FSCORE_EOK	          Ok.
 * @return  FSCORE_ENOENT	        Object not found.
 * @return  FSCORE_ENOTDIR	      A component of the path prefix is not a directory.
 * @return  FSCORE_EBADFD	        Bad file descriptor.
 * @return  FSCORE_EACCES	        Search permission is denied for a component of the path 
 *                                prefix.
 * @return  FSCORE_ENAMETOOLONG   Object's name is too long.
 * @return  FSCORE_EBADNAME	      Object's name contains illegal characters.
 */
  extern T_FSCORE_RET rfsfat_fstat (T_FSCORE_FD fd, T_FSCORE_STAT * stat);


  
/**
 * rfsfat_fsync
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
 * @return  FSCORE_EOK	      Ok.
 * @return  FSCORE_EBADFD	    Invalid file descriptor.
 * @return  FSCORE_ENOSPACE	  Out of data space.
 * @return  FSCORE_EFSFULL	  File system full, no free inodes.
 * @return  FSCORE_EDEVICE	  Device I/O error
 */
  extern T_FSCORE_RET rfsfat_fsync (T_FSCORE_FD fd);



/**
 * rfsfat_fsync_nb
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
 * @return  FSCORE_EOK	      Ok.
 * @return  FSCORE_EBADFD	    Invalid file descriptor.
 * @return  FSCORE_ENOSPACE	  Out of data space.
 * @return  FSCORE_EFSFULL	  File system full, no free inodes.
 * @return  FSCORE_EDEVICE	  Device I/O error
 */
  extern T_FSCORE_RET rfsfat_fsync_nb (T_FSCORE_FD fd,T_RV_RETURN_PATH return_path);




//BTI: Prototypes done



/**
 * rfsfat_open_nb
 *
 * Detailled description.
 * The file specified by pathname is opened for reading and/or writing as specified 
 * by the argument flags and the file descriptor is returned to the calling process. 
 * The flags argument may indicate the file is to be created if it does not exist 
 * (by specifying the rfsfat_O_CREAT flag), in which case the file is created with mode
 * mode. Else the mode will be ignored.
 * 
 * If the operation succeeds the file pointer used to mark the current position within
 * the file is set to the beginning of the file and a (positive) file descriptor is 
 * returned. Otherwise an error is returned (negative value).
 *
 * @param   pathname    Null terminated string containing the unique name of the file 
 *                      to open or create.
 * @param   flags       Specifies the attribute used to open the file. 
 * @param   mode        Specifies the mode argument (permission bits of the file) and 
 *                      will be used when the FSCORE_O_CREAT flag is specified in flags.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)	Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES	    The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY	  Insufficient memory to create message request
 * @return  FSCORE_EINVALID	  Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND	  Error while sending the internal message
 */
  extern T_FSCORE_RET rfsfat_open_nb (const char *pathname,
									  T_FSCORE_FLAGS flags,
									  T_FSCORE_MODE mode,
									  T_RV_RETURN return_path);

#if (FFS_UNICODE ==1)
  extern T_FSCORE_RET rfsfat_open_uc_nb(const T_WCHAR *pathname,
									  	T_FSCORE_FLAGS flags,
									  	T_FSCORE_MODE mode,
									  	T_RV_RETURN_PATH return_path);
#endif




/**
 * rfsfat_chmod_nb
 *
 * Detailled description.
 * The function rfsfat_chmod() sets the file permission bits of the file specified by 
 * the pathname pathname to required mode. 
 * 
 * A mode is created from OR'd permission bit masks defined by T_FSCORE_MODE.
 *
 * @param   pathname    Null terminated string containing the unique name of the 
 *                      file for changing the mode.
 * @param   mode        Specifies the attribute (permission bits) of the file
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)	Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES	    The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY	  Insufficient memory to create message request
 * @return  FSCORE_EINVALID	  Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND	  Error while sending the internal message
 */
  extern T_FSCORE_RET rfsfat_chmod_nb (const char *pathname,
									   T_FSCORE_MODE mode,
									   T_RV_RETURN return_path);
/**
 * rfsfat_stat_nb
 *
 * Detailled description.
 * The rfsfat_stat() function obtains information about the file or device associated to 
 * the mountpoint pointed to by pathname. 
 * 
 * If pathname is NULL, general information about the file system is returned (e.g.
 * default mountpoint and file system limits). If pathname is '/mountpoint', 
 * information on the device associated to the mount point is returned (e.g. speed
 * data of the mount pount). if pathname ends with a directory or file, the 
 * appropriate information for the directory or file is returned. 
 *
 * @param   stat        Contains information (meta-data) about the specified object.
 * @param   pathname    Terminated string containing NULL, the name of the mountpount, 
 *                      file or directory.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)	Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES	    The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY	  Insufficient memory to create message request
 * @return  FSCORE_EINVALID	  Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND	  Error while sending the internal message
 */
  extern T_FSCORE_RET rfsfat_stat_nb (const char *pathname,
									  T_FSCORE_STAT * stat,
									  T_RV_RETURN return_path);


#if (FFS_UNICODE ==1)

extern T_FSCORE_RET rfsfat_stat_uc_nb (
									   const T_WCHAR *pathname,
									  T_FSCORE_STAT * stat,
									  T_RV_RETURN_PATH return_path);



#endif



/**
 * rfsfat_remove_nb
 *
 * Detailled description.
 * This function removes the object with the pathname given by pathname. The pathname
 * is a null termi-nated string. If the object does not exist, FSCORE_ENOENT is returned.
 * If a directory is to be removed, it must be empty, otherwise FSCORE_ENOTEMPTY is 
 * returned. It is not possible to remove a file that is open.
 *
 * @param   pathname    Null terminated string containing the unique name of the object.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)	Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES	    The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY	  Insufficient memory to create message request
 * @return  FSCORE_EINVALID	  Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND	  Error while sending the internal message
 */
  extern T_FSCORE_RET rfsfat_remove_nb (const char *pathname,
										T_RV_RETURN return_path);


#if (FFS_UNICODE ==1)
 extern T_FSCORE_RET rfsfat_remove_uc_nb (
 										const T_WCHAR *pathname,
										T_RV_RETURN_PATH return_path);

#endif



/**
 * rfsfat_rename_nb
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
 * @param   oldname     Null terminated string containing the unique name including the
 *                      path of the existing object in the File System.
 * @param   newname     Null terminated string containing the unique name including the
 *                      path, which oldname is desired to change name or location to.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)	Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES	    The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY	  Insufficient memory to create message request
 * @return  FSCORE_EINVALID	  Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND	  Error while sending the internal message
 */
  extern T_FSCORE_RET rfsfat_rename_nb (const char *oldname,
										const char *newname,
										T_RV_RETURN return_path);


#if (FFS_UNICODE == 1)

 extern T_FSCORE_RET rfsfat_rename_uc_nb(const T_WCHAR *oldname,
 										 const T_WCHAR *newname,
										T_RV_RETURN_PATH return_path);



#endif



/**
 * rfsfat_mkdir_nb
 *
 * Detailled description.
 * This function creates a directory with the pathname given by pathname. The 
 * pathname is a null termi-nated string. All components of the pathname must be 
 * already existing directories. This means that it is not possible to 
 * rfsfat_mkdir("/gsm/rf/tx") if the directories /gsm/ and /gsm/rf are not already 
 * created.
 *
 * @param   pathname    Null terminated string containing the unique name of the
 *                      directory to create
 * @param   mode        Specifies the attribute (permission bits) of the directory
 *											
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)	Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES	    The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY	  Insufficient memory to create message request
 * @return  FSCORE_EINVALID	  Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND	  Error while sending the internal message
 *
 * NOTE: the Mode parameter is Not used, FAT doesn't specifiy how to deal with
 *			 the read/write permission behaviour of directories 
 */
  extern T_FSCORE_RET rfsfat_mkdir_nb (const char *pathname,
									   T_FSCORE_MODE mode,
									   T_RV_RETURN return_path);



#if (FFS_UNICODE ==1)
 extern T_FSCORE_RET rfsfat_mkdir_uc_nb (const T_WCHAR *pathname,
									   T_FSCORE_MODE mode,
									   T_RV_RETURN_PATH return_path);

#endif



/**
 * rfsfat_rmdir_nb
 *
 * Detailled description.
 * This function removes a directory file whose name is given by pathname. The 
 * directory must be empty.
 *
 * @param   pathname    Null terminated string containing the unique name of the 
 *                      directory to remove.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)	Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES	    The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY	  Insufficient memory to create message request
 * @return  FSCORE_EINVALID	  Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND	  Error while sending the internal message
 */
  extern T_FSCORE_RET rfsfat_rmdir_nb (const char *pathname,
									   T_RV_RETURN return_path);




#if (FFS_UNICODE ==1)
 extern T_FSCORE_RET rfsfat_rmdir_uc_nb (
										const T_WCHAR *pathname,
									   T_RV_RETURN_PATH return_path);

#endif

/**
 * rfsfat_opendir_nb
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
 * @param   dirp        Pointer to a T_FSCORE_DIR structure. To be used in 
 *                      consequitive rfsfat_readdir_nb().
 *                      It is representing a directory stream.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)	Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES	    The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY	  Insufficient memory to create message request
 * @return  FSCORE_EINVALID	  Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND	  Error while sending the internal message
 */
  extern T_FSCORE_RET rfsfat_opendir_nb (const char *pathname,
										 T_FSCORE_DIR * dirp,
										 T_RV_RETURN return_path);



#if (FFS_UNICODE ==1)
  extern T_FSCORE_RET rfsfat_opendir_uc_nb (const T_WCHAR *pathname,
										 T_FSCORE_DIR * dirp,
										 T_RV_RETURN_PATH return_path);
#endif


/**
 * rfsfat_readdir_nb
 *
 * Detailled description.
 * This function reads an entry from a directory previously opened by rfsfat_opendir().
 * The input parameter is a pointer to the opened directory stream (specified by the
 * argument dirp). The structure associated with the pointer dirp, keeps track of the
 * directory entry last read by the function rfsfat_readdir(). To achieve this, the 
 * pointer dirp is also an output parameter.
 *
 * The other (output) parameters concern directory entry information, which is read. 
 * This information exists of a pointer to a buffer containing the name of the entry 
 * and the size of the buffer. The buffer memory needs to be allocated by the client. 
 *
 * A positive return value denotes that the buffer pointed to by buf (member of the
 * structure pointed by direntp) contains the null-terminated name of the entry found.
 * A zero is returned if there were no more entries in the directory and the buffer 
 * pointed to by buf is left untouched. The function returns a nega-tive value if an 
 * exception is encountered.
 *
 * In order to read all entries in a directory, rfsfat_readdir() should be called until
 * it returns zero.
 *
 * @param   dirp      Pointer to a T_FSCORE_DIR structure obtained in a previous call to 
 *                    rfsfat_opendir(), representing a directory stream.
 * @param   buf       Pointer to buffer into which the found name shall be returned
 * @param   size      size of buffer 'buf'. max 'size' characters may be copied in 'buf'
 * @param   buf       Pointer to buffer into which the found name shall be returned
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)	Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES	    The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY	  Insufficient memory to create message request
 * @return  FSCORE_EINVALID	  Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND	  Error while sending the internal message
 */
  extern T_FSCORE_RET rfsfat_readdir_nb (T_FSCORE_DIR * dirp,
										 char *buf,
										 T_FSCORE_SIZE size,
										 T_RV_RETURN return_path);



#if (FFS_UNICODE ==1)

extern T_FSCORE_RET rfsfat_readdir_uc_nb (T_FSCORE_DIR * dirp,
				   T_WCHAR *buf, T_FSCORE_SIZE size, T_RV_RETURN_PATH return_path);
#endif				   



/**
 * rfsfat_closedir_nb
 *
 * Detailled description.
 * This function closes opened directory, cleanup the internal data structures & resources
 * allocated for this directory 
 *
 * @param   dirp      Pointer to a T_FSCORE_DIR structure obtained in a previous call to 
 *                    rfsfat_opendir(), representing a directory stream.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)	Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES	    The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY	  Insufficient memory to create message request
 * @return  FSCORE_EINVALID	  Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND	  Error while sending the internal message
 */
  extern T_FSCORE_RET rfsfat_closedir_nb (T_FSCORE_DIR * dirp,T_RV_RETURN return_path);



/**
 * rfsfat_preformat_nb
 *
 * Detailled description.
 * With this pre-format function it is only 
 * possible to erase the data in a media partition. Within UFS a media partition is
 * the same as a mountpoint. For this pre-format function it is required that the 
 * given pathname should be a mountpunt '/mountpoint'. If this is not the case, no 
 * pre-formatting takes place and an error is returned.
 *
 * The pre-format operation cannot be reversed or undone. Note that depending on the 
 * underlying flash hardware, the pre-format operation can take anything from a few
 * milliseconds to several seconds. Most flash memories in a normal environment take 
 * around one second (typical) to erase each sector. The magic number must equal the
 * hexadecimal constant 0xDEAD. If the magic number given by magic is incorrect, 
 * FSCORE_EMAGIC is returned.
 *
 * @param   pathname    Null terminated string containing the name of the partition 
 *                      to format.
 * @param   magic       Magic value to access the function. Must be 0xDEAD.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)	Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES	    The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY	  Insufficient memory to create message request
 * @return  FSCORE_EINVALID	  Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND	  Error while sending the internal message
 */
  extern T_FSCORE_RET rfsfat_preformat_nb (const char *pathname,
										   UINT16 magic,
										   T_RV_RETURN return_path);




#if (FFS_UNICODE ==1)
extern T_FSCORE_RET rfsfat_preformat_uc_nb (
											const T_WCHAR*pathname,
										   UINT16 magic,
										   T_RV_RETURN_PATH return_path);
#endif




/**
 * rfsfat_format_nb
 *
 * Detailled description.
 * With this format function a new file system is 
 * created on the requested media partition. Within UFS, a media partition is the 
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
 * This function must be called after rfsfat_preformat() and before any other operation
 * on RFS. In order to avoid spurious calls of this dangerous, unrecoverable function,
 * the magic number must have the hexadecimal value of 0x2BAD to format the flash.
 *
 * @param   pathname    Null terminated string containing the name of the partition
 *                      to format.
 * @param   name        Null terminated string containing the name of the RFS volume.
 * @param   magic       Magic value to access the function. Must be 0x2BAD.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)	Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES	    The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY	  Insufficient memory to create message request
 * @return  FSCORE_EINVALID	  Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND	  Error while sending the internal message
 */
  extern T_FSCORE_RET rfsfat_format_nb (const char *pathname,
										const char *name,
										UINT16 magic,
										T_RV_RETURN return_path);




#if (FFS_UNICODE ==1)
extern T_FSCORE_RET rfsfat_format_uc_nb (const T_WCHAR*			pathname ,
										 const T_WCHAR *name,
										UINT16 magic,
										T_RV_RETURN_PATH return_path);



#endif


 extern T_FSCORE_RET rfsfat_trunc (T_FSCORE_FD fd, T_FSCORE_SIZE size);
  extern T_FSCORE_RET rfsfat_trunc_nb (T_FSCORE_FD fd, T_FSCORE_SIZE size,T_RV_RETURN_PATH return_path);
  extern T_FSCORE_RET rfsfat_truncate_nb (const char* pathname, T_FSCORE_SIZE size,T_RV_RETURN_PATH return_path);
  extern T_FSCORE_RET rfsfat_file_read_nb(const char *pathname,void *buf,T_FSCORE_SIZE size,T_RV_RETURN_PATH return_path);
  extern T_FSCORE_RET rfsfat_file_write_nb(const char *pathname,void *buf,T_FSCORE_SIZE size,T_FSCORE_FLAGS openflags,T_RV_RETURN_PATH return_path);  
  extern T_FSCORE_RET rfsfat_query_nb(char query, void * buf,T_RV_RETURN_PATH return_path);
  extern T_FSCORE_RET rfsfat_query_partition_nb(T_GBI_MEDIA_TYPE media_type,char *buf,T_RV_RETURN_PATH return_path);

/* get and set atrributes */
  extern T_FSCORE_RET rfsfat_getattribute_nb(const char *pathname,T_RV_RETURN_PATH return_path);
  extern T_FSCORE_RET rfsfat_setattribute_nb(const char *pathname,T_FFS_ATTRIB_TYPE attrib, T_RV_RETURN_PATH return_path);
  extern T_FSCORE_RET rfsfat_getdatetime_nb(const char *pathname,T_RTC_DATE_TIME *date_time_p, T_RTC_DATE_TIME * crdate_time_p,T_RV_RETURN_PATH return_path);

/* set and get volume label */
  extern T_FSCORE_RET rfsfat_set_label_nb(const T_WCHAR *pathname, const T_WCHAR *label, T_RV_RETURN_PATH return_path);
  extern T_FSCORE_RET rfsfat_get_label_nb(const T_WCHAR *pathname, T_WCHAR *label, T_RV_RETURN_PATH return_path);
  
#if (FFS_UNICODE ==1)
  extern T_FSCORE_RET rfsfat_truncate_uc_nb (const T_WCHAR* pathname, T_FSCORE_SIZE size,T_RV_RETURN_PATH return_path);
  extern T_FSCORE_RET rfsfat_getattribute_uc_nb(const T_WCHAR *pathname,T_RV_RETURN_PATH return_path);
  extern T_FSCORE_RET rfsfat_setattribute_uc_nb(const T_WCHAR *pathname,T_FFS_ATTRIB_TYPE attrib,T_RV_RETURN_PATH return_path);
  extern T_FSCORE_RET rfsfat_getdatetime_uc_nb(const T_WCHAR *pathname,T_RTC_DATE_TIME *date_time_p,T_RTC_DATE_TIME * crdate_time_p,T_RV_RETURN_PATH return_path);
  extern T_FSCORE_RET rfsfat_chmod_uc_nb (const T_WCHAR *pathname,T_FSCORE_MODE mode, T_RV_RETURN return_path);



#if 0
  extern T_FSCORE_RET rfsfat_file_read_uc_nb(const T_WCHAR *pathname,void *buf,T_FSCORE_SIZE size,T_RV_RETURN_PATH return_path);
  extern T_FSCORE_RET rfsfat_file_write_uc_nb(const T_WCHAR *pathname,void *buf,T_FSCORE_SIZE size,T_FSCORE_FLAGS openflags,T_RV_RETURN_PATH return_path);  
#endif /* #if 0*/  



void rfsfat_connect_unicode_to_u8(
                            void  (*unicode_to_u8)(const T_WCHAR  *name_uc,
                                                   UINT8               *name_u8));


void rfsfat_connect_u8_to_unicode(
                            void  (*u8_to_unicode)(const UINT8         *name_u8,
                                                   T_WCHAR        *name_uc));



#endif


T_FSCORE_RET rfsfat_mount_nb(T_RV_RETURN_PATH return_path);

T_FSCORE_RET rfsfat_unmount_nb(T_RV_RETURN_PATH return_path);



  
/**
 * rfsfat_get_sw_version
 *
 * Detailled description.
 * This function returns the version of this service entity.
 * 
 * @return    [0-15]	BUILD	Build number
 *            [16-23]	MINOR	Minor version number
 *            [24-31]	MAJOR	Major version number
 */
  extern UINT32 rfsfat_get_sw_version (void);

/*@}*/


/** 
 * The message offset must differ for each SWE in order to have 
 * unique msg_id in the system.
 */
#define RFSFAT_MESSAGE_OFFSET	 BUILD_MESSAGE_OFFSET(RFSFAT_USE_ID)
#define RFSFAT_INIT_MSG	 		  	(RFSFAT_MESSAGE_OFFSET)

typedef struct 
{
  unsigned char rfsfat_value;
}T_RFSFAT_INIT_MSG;

/**
 * @name RFSFAT_OPEN_REQ_MSG
 *
 * Detailled description
 * The T_FSFAT_OPEN_REQ_MSG message can be used to open a file. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_OPEN_RSP.
 */
/*@{*/

/** Open request. */
#define RFSFAT_OPEN_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0001)

  typedef struct
  {
	T_RV_HDR hdr;
	const char *pathname;
	T_FSCORE_FLAGS flags;
	T_FSCORE_MODE mode;
	T_RV_RETURN return_path;
	T_FSCORE_PAIR_VALUE pair_value;
  } T_RFSFAT_OPEN_REQ_MSG;


/**
 * @name RFSFAT_READ_REQ_MSG
 *
 * Detailled description
 * The T_FSFAT_READ_REQ_MSG message can be used to read from a file. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_READ_RSP.
 */
/*@{*/
/** Read request. */
#define RFSFAT_READ_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0002)

  typedef struct
  {
	T_RV_HDR hdr;
	T_FSCORE_FD fd;
	const void *buf;
	T_FSCORE_SIZE size;
	T_RV_RETURN return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_READ_REQ_MSG;


/**
 * @name RFSFAT_WRITE_REQ_MSG
 *
 * Detailled description
 * The T_FSFAT_WRITE_REQ_MSG message can be used to write to a file. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_WRITE_RSP.
 */
/*@{*/
/** Write request. */
#define RFSFAT_WRITE_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0003)

  typedef struct
  {
	T_RV_HDR hdr;
	T_FSCORE_FD fd;
	const void *buf;
	T_FSCORE_SIZE size;
	T_RV_RETURN return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_WRITE_REQ_MSG;


/**
 * @name RFSFAT_CLOSE_REQ_MSG
 *
 * Detailled description
 * The T_FSFAT_CLOSE_REQ_MSG message can be used to open a file. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_CLOSE_RSP.
 */
/*@{*/
/** Close request. */
#define RFSFAT_CLOSE_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0004)

  typedef struct
  {
	T_RV_HDR hdr;
	T_FSCORE_FD fd;
	T_FSCORE_PAIR_VALUE pair_value;
	T_RV_RETURN return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_CLOSE_REQ_MSG;

/**
 * @name RFSFAT_REMOVE_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_REMOVE_REQ_MSG message can be used to remove a file. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_REMOVE_RSP.
 */
/*@{*/
/** Remove request. */
#define RFSFAT_REMOVE_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0005)

  typedef struct
  {
	T_RV_HDR hdr;
	const char *pathname;
	T_FSCORE_PAIR_VALUE pair_value;
	T_RV_RETURN return_path;
  } T_RFSFAT_REMOVE_REQ_MSG;


/**
 * @name RFSFAT_LSEEK_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_LSEEK_REQ_MSG message can be used to repositions the offset of the file 
 * descriptor. The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, 
 * with command index: FSCORE_LSEEK_RSP.
 */
/*@{*/
/** Lseek request. */
#define RFSFAT_LSEEK_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0006)

  typedef struct
  {
	T_RV_HDR hdr;
	T_FSCORE_FD fd;
	T_FSCORE_OFFSET offset;
	T_FSCORE_WHENCE whence;
	T_RV_RETURN return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_LSEEK_REQ_MSG;

/**
 * @name RFSFAT_FCHMOD_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_FCHMOD_REQ_MSG message can be used to sets the permission bits of the 
 * specified file descriptor. The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, 
 * with command index: FSCORE_FCHMOD_RSP.
 */
/*@{*/
/** Fchmod request. */
#define RFSFAT_FCHMOD_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0007)

  typedef struct
  {
	T_RV_HDR hdr;
	T_FSCORE_FD fd;
	T_FSCORE_MODE mode;
	T_RV_RETURN return_path;
  } T_RFSFAT_FCHMOD_REQ_MSG;

/**
 * @name RFSFAT_FSTAT_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_FSTAT_REQ_MSG message can be used to obtains information about 
 * an open file. The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, 
 * with command index: FSCORE_FSTAT_RSP.
 */
/*@{*/
/** Fstat request. */
#define RFSFAT_FSTAT_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0008)

  typedef struct
  {
	T_RV_HDR hdr;
	T_FSCORE_FD fd;
	T_FSCORE_STAT *stat;
	T_RV_RETURN return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_FSTAT_REQ_MSG;


/**
 * @name RFSFAT_FSYNC_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_FSYNC_REQ_MSG message can be used to save (flush) unwritten data 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, 
 * with command index: FSCORE_FSYNC_RSP.
 */
/*@{*/
/** Fsync request. */
#define RFSFAT_FSYNC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0009)

  typedef struct
  {
	T_RV_HDR hdr;
	T_FSCORE_FD fd;
	T_RV_RETURN return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_FSYNC_REQ_MSG;


/**
 * @name RFSFAT_CHMOD_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_CHMOD_REQ_MSG message can be used to change the permission attributes 
 * of a file or directory. This message is similar to the rfs_chmod_nb() function. 
 * The RFS responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_CHMOD_RSP.
 */
/*@{*/
/** Chmod request. */
#define RFSFAT_CHMOD_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x000A)

  typedef struct
  {
	T_RV_HDR hdr;
	const char *pathname;
	T_RFS_MODE mode;
	T_FSCORE_PAIR_VALUE pair_value;
	T_RV_RETURN return_path;
  } T_RFSFAT_CHMOD_REQ_MSG;


/**
 * @name RFSFAT_STAT_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_STAT_REQ_MSG message can be used to obtain information about a the core,
 * mountpoint, file or directory. The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, 
 * with command index: FSCORE_STAT_RSP.
 */
/*@{*/
/** Stat request. */
#define RFSFAT_STAT_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x000B)

  typedef struct
  {
	T_RV_HDR hdr;
	const char *pathname;
	T_FSCORE_STAT *stat;
	T_FSCORE_PAIR_VALUE pair_value;
	T_RV_RETURN return_path;
  } T_RFSFAT_STAT_REQ_MSG;


/**
 * @name RFSFAT_RENAME_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_RENAME_REQ_MSG message can be used to rename files and directories.
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_RENAME_RSP.
 */
/*@{*/
/** Rename request. */
#define RFSFAT_RENAME_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x000C)

  typedef struct
  {
	T_RV_HDR hdr;
	const char *oldname;
	const char *newname;
	T_FSCORE_PAIR_VALUE pair_value;
	T_RV_RETURN return_path;
  } T_RFSFAT_RENAME_REQ_MSG;

/**
 * @name RFSFAT_MKDIR_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_MKDIR_REQ_MSG message can be used to create a directory 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_MKDIR_RSP.
 */
/*@{*/
/** Mkdir request. */
#define RFSFAT_MKDIR_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x000D)

  typedef struct
  {
	T_RV_HDR hdr;
	const char *pathname;
	T_FSCORE_MODE mode;
	T_FSCORE_PAIR_VALUE pair_value;
	T_RV_RETURN return_path;
  } T_RFSFAT_MKDIR_REQ_MSG;


/**
 * @name RFSFAT_RMDIR_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_RMDIR_REQ_MSG message can be used to removes a directory.
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_RMDIR_RSP.
 */
/*@{*/
/** Rmdir request. */
#define RFSFAT_RMDIR_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x000E)

  typedef struct
  {
	T_RV_HDR hdr;
	const char *pathname;
	T_FSCORE_PAIR_VALUE pair_value;
	T_RV_RETURN return_path;
  } T_RFSFAT_RMDIR_REQ_MSG;

/**
 * @name RFSFAT_OPENDIR_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_OPENDIR_REQ_MSG message can be used to open a directory.
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_OPENDIR_RSP.
 */
/*@{*/
/** Opendir request. */
#define RFSFAT_OPENDIR_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x000F)

  typedef struct
  {
	T_RV_HDR hdr;
	const char *pathname;
	T_FSCORE_DIR *dirp;
	T_FSCORE_PAIR_VALUE pair_value;
	T_RV_RETURN return_path;
  } T_RFSFAT_OPENDIR_REQ_MSG;

/**
 * @name RFSFAT_READDIR_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_READDIR_REQ_MSG message can be used to read an entry from a directory.
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_READDIR_RSP.
 */
/*@{*/
/** Readdir request. */
#define RFSFAT_READDIR_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0010)

  typedef struct
  {
	T_RV_HDR hdr;
	T_FSCORE_DIR *dirp;
	char *buf;
	T_FSCORE_SIZE size;
	T_FSCORE_PAIR_VALUE pair_value;
	T_RV_RETURN return_path;
  } T_RFSFAT_READDIR_REQ_MSG;

/**
 * @name RFSFAT_PREFORMAT_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_PREFORMAT_REQ_MSG message can be used to erase the data in a media partition
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_PREFORMAT_RSP.
 */
/*@{*/
/** Preformat request. */
#define RFSFAT_PREFORMAT_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0011)

  typedef struct
  {
	T_RV_HDR hdr;
	const char *pathname;
	UINT16 magic;
	T_FSCORE_PAIR_VALUE pair_value;
	T_RV_RETURN return_path;
  } T_RFSFAT_PREFORMAT_REQ_MSG;

/**
 * @name RFSFAT_FORMAT_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_FORMAT_REQ_MSG message can be used to format the file system.
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_FORMAT_RSP.
 */
/*@{*/
/** Format request. */
#define RFSFAT_FORMAT_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0012)

  typedef struct
  {
	T_RV_HDR hdr;
	const char *pathname;
	const char *name;
	UINT16 magic;
	T_FSCORE_PAIR_VALUE pair_value;
	T_RV_RETURN return_path;
  } T_RFSFAT_FORMAT_REQ_MSG;


/* This defines the set partition request message, type of message is defined else */
#define RFSFAT_SET_PARTITION_TABLE_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0013)


/* This message should be specfic to File system core */
#define FSCORE_SET_PARTITION_TABLE_REQ_MSG RFSFAT_SET_PARTITION_TABLE_REQ_MSG


/* NOTE: The below two message should have same ids defined by RFS */

/* This defines the set partition response message, type of message is defined else */
#define RFSFAT_SET_PARTITION_TABLE_RSP_MSG FSCORE_SET_PARTITION_TABLE_RSP_MSG

/* This defines the generic RFSFAT ready response message, type of message is defined else */
#define RFSFAT_READY_RSP_MSG  FSCORE_READY_RSP_MSG


/**
 * @name RFSFAT_TRUNC_REQ_MSG
 *
 * Detailled description
 * The T_FSFAT_TRUNC_REQ_MSG message can be used to read from a file. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_TRUNC_RSP.
 */
/*@{*/
/** Trunc request. */
#define RFSFAT_TRUNC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x00016)

  typedef struct
  {
      T_RV_HDR hdr;
	T_FSCORE_FD fd;
	T_FSCORE_SIZE size;
	//T_FSCORE_TRUNC trunc
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_TRUNC_REQ_MSG;



/**
 * @name RFSFAT_TRUNC_NAME_REQ_MSG
 *
 * Detailled description
 * The T_FSFAT_TRUNC_NAME_REQ_MSG message can be used to truncate the file given a name. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_TRUNC_NAME_RSP.
 */
/*@{*/
/** Trunc request. */
#define RFSFAT_TRUNC_NAME_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x00017)

  typedef struct
  {
        T_RV_HDR hdr;
	const char *path_name;
	T_FSCORE_SIZE size;
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_TRUNC_NAME_REQ_MSG;
/**
 * @name RFSFAT_FILE_READ_REQ_MSG
 *
 * Detailled description
 * The T_FSFAT_FILE_READ_REQ_MSG message can be used to read the file given a name. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_FILE_READ_RSP.
 */
/*@{*/
/** Trunc request. */
#define RFSFAT_FILE_READ_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x00018)

  typedef struct
  {
      T_RV_HDR hdr;
	const char *path_name;
	char *buf;
	T_FSCORE_SIZE size;
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_FILE_READ_REQ_MSG;





/**
 * @name RFSFAT_FILE_WRITE_REQ_MSG
 *
 * Detailled description
 * The T_FSFAT_FILE_WRITE_REQ_MSG message can be used to write to the file given a name. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_FILE_WRITE_RSP.
 */
/*@{*/
/** Trunc request. */
#define RFSFAT_FILE_WRITE_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x00019)

  typedef struct
  {
       T_RV_HDR hdr;
	const char *path_name;
	char *buf;
	T_FSCORE_SIZE size;
	T_FSCORE_FLAGS flags;
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_FILE_WRITE_REQ_MSG;



/**
 * @name RFSFAT_QUERY_MSG
 *
 * Detailled description
 * The T_FSFAT_QUERY_MSG message can be used to query the file system. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_QUERY_RSP.
 */
/*@{*/
/** Trunc request. */
#define RFSFAT_QUERY_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0001A)

  typedef struct
  {
    T_RV_HDR hdr;  
	char query;
	char *buf;
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_QUERY_REQ_MSG;



/**
 * @name RFSFAT_QUERY_PARTITION_REQ_MSG
 *
 * Detailled description
 * The T_RFSFAT_QUERY_PARTITION_MSG message can be used to query partition info of the file system. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_QUERY_RSP.
 */
/*@{*/
/** Trunc request. */
#define RFSFAT_QUERY_PARTITION_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0001B)

  typedef struct
  {
       T_RV_HDR hdr;  
    T_GBI_MEDIA_TYPE   media_type;
	char *buf;
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_QUERY_PARTITION_REQ_MSG;


#if (FFS_UNICODE ==1)


/**************************************************/
//                              
// Unicode EXTENTION: MESSAGES
//                              
/**************************************************/


/**
 * @name RFSFAT_CHMOD_UC_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_CHMOD__UC_REQ_MSG message can be used to change the permission attributes 
 * of a file or directory. This message is similar to the rfs_chmod_nb() function. 
 * The RFS responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_CHMOD_RSP.
 */
/*@{*/
/** Chmod request. */
#define RFSFAT_CHMOD_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x001C)

  typedef struct
  {
       T_RV_HDR hdr;  
  const T_WCHAR *pathname;
  T_FSCORE_MODE mode;
  T_FSCORE_PAIR_VALUE pair_value;
  T_RV_RETURN_PATH return_path;
  } T_RFSFAT_CHMOD_UC_REQ_MSG;


/**
 * @name RFSFAT_FORMAT_UC_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_FORMAT_UC_REQ_MSG message can be used to format the file system.
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_FORMAT_RSP.
 */
/*@{*/
/** Format request. */
#define RFSFAT_FORMAT_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x001D)

  typedef struct
  {
   T_RV_HDR hdr;
  const T_WCHAR *pathname;
  const T_WCHAR *name;
  UINT16 magic;
  T_FSCORE_PAIR_VALUE pair_value;
  T_RV_RETURN_PATH return_path;
  } T_RFSFAT_FORMAT_UC_REQ_MSG;


/**
 * @name RFSFAT_MKDIR_UC_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_MKDIR__UC_REQ_MSG message can be used to create a directory 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_MKDIR_RSP.
 */
/*@{*/
/** Mkdir request. */
#define RFSFAT_MKDIR_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x001E)

  typedef struct
  {
         T_RV_HDR hdr;
  const T_WCHAR *pathname;
  T_FSCORE_MODE mode;
  T_FSCORE_PAIR_VALUE pair_value;
  T_RV_RETURN_PATH return_path;
  } T_RFSFAT_MKDIR_UC_REQ_MSG;


/**
 * @name RFSFAT_OPEN_UC_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_OPEN_UC_REQ_MSG message can be used to open a file. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_OPEN_RSP.
 */
/*@{*/

/** Open request. */
#define RFSFAT_OPEN_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x001F) 

  typedef struct
  {
         T_RV_HDR hdr;
    const T_WCHAR *pathname;
    T_FSCORE_FLAGS        flags;
    T_FSCORE_MODE         mode;
    T_RV_RETURN_PATH	return_path;
    T_FSCORE_PAIR_VALUE   pair_value;
  } T_RFSFAT_OPEN_UC_REQ_MSG;


/**
 * @name RFSFAT_PREFORMAT_UC_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_PREFORMAT_UC_REQ_MSG message can be used to erase the data in a media partition
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_PREFORMAT_RSP.
 */
/*@{*/
/** Preformat request. */
#define RFSFAT_PREFORMAT_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0020)

  typedef struct
  {
         T_RV_HDR hdr;
  const T_WCHAR *pathname;
  UINT16 magic;
  T_FSCORE_PAIR_VALUE pair_value;
  T_RV_RETURN_PATH return_path;
  } T_RFSFAT_PREFORMAT_UC_REQ_MSG;

/**
 * @name RFSFAT_REMOVE_UC_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_REMOVE_UC_REQ_MSG message can be used to remove a file. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_REMOVE_RSP.
 */
/*@{*/
/** Remove request. */
#define RFSFAT_REMOVE_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0021)

  typedef struct
  {
         T_RV_HDR hdr;
  const T_WCHAR *pathname;
  T_FSCORE_PAIR_VALUE pair_value;
  T_RV_RETURN_PATH return_path;
  } T_RFSFAT_REMOVE_UC_REQ_MSG;

/**
 * @name RFSFAT_RENAME_UC_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_RENAME_UC_REQ_MSG message can be used to rename files and directories.
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_RENAME_RSP.
 */
/*@{*/
/** Rename request. */
#define RFSFAT_RENAME_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0022)

  typedef struct
  {
         T_RV_HDR hdr;
  const T_WCHAR *oldname;
  const T_WCHAR *newname;
  T_FSCORE_PAIR_VALUE pair_value;
  T_RV_RETURN_PATH return_path;
  } T_RFSFAT_RENAME_UC_REQ_MSG;


/**
 * @name RFSFAT_RMDIR_UC_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_RMDIR_UC_REQ_MSG message can be used to removes a directory.
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_RMDIR_RSP.
 */
/*@{*/
/** Rmdir request. */
#define RFSFAT_RMDIR_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0023)

  typedef struct
  {
         T_RV_HDR hdr;
  const T_WCHAR *pathname;
  T_FSCORE_PAIR_VALUE pair_value;
  T_RV_RETURN_PATH return_path;
  } T_RFSFAT_RMDIR_UC_REQ_MSG;


/**
 * @name RFSFAT_STAT_UC_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_STAT_UC_REQ_MSG message can be used to obtain information about a the core,
 * mountpoint, file or directory. The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, 
 * with command index: FSCORE_STAT_RSP.
 */
/*@{*/
/** Stat request. */
#define RFSFAT_STAT_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0024)

  typedef struct
  {
         T_RV_HDR hdr;
  const T_WCHAR *pathname;
  T_FSCORE_STAT *stat;
  T_FSCORE_PAIR_VALUE pair_value;
  T_RV_RETURN_PATH return_path;
  } T_RFSFAT_STAT_UC_REQ_MSG;


/**
 * @name RFSFAT_OPENDIR_UC_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_OPENDIR_UC_REQ_MSG message can be used to obtain information about a the core,
 * mountpoint, file or directory. The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, 
 * with command index: FSCORE_OPENDIR_RSP.
 */

#define RFSFAT_OPENDIR_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0025)
typedef struct
  {
         T_RV_HDR hdr;
	const T_WCHAR *pathname;
	T_FSCORE_DIR *dirp;
	T_FSCORE_PAIR_VALUE pair_value;
	T_RV_RETURN_PATH return_path;
  } T_RFSFAT_OPENDIR_UC_REQ_MSG;


/**
 * @name RFSFAT_READDIR_REQ_MSG
 *
 * Detailled description
 * The RFSFAT_READDIR_REQ_MSG message can be used to read an entry from a directory.
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message, with command index: 
 * FSCORE_READDIR_RSP.
 */
/*@{*/
/** Readdir request. */
#define RFSFAT_READDIR_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0026)

  typedef struct
  {
         T_RV_HDR hdr;
	T_FSCORE_DIR *dirp;
	T_WCHAR *buf;
	T_FSCORE_SIZE size;
	T_FSCORE_PAIR_VALUE pair_value;
	T_RV_RETURN_PATH return_path;
  } T_RFSFAT_READDIR_UC_REQ_MSG;





/**
 * @name RFSFAT_TRUNC_NAME_UC_REQ_MSG
 *
 * Detailled description
 * The T_FSFAT_TRUNC_NAME_REQ_MSG message can be used to truncate the file given a name. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_TRUNC_NAME_RSP.
 */
/*@{*/
/** Trunc request. */
#define RFSFAT_TRUNC_NAME_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x00027)

  typedef struct
  {
         T_RV_HDR hdr;
	const T_WCHAR *pathname;
	T_FSCORE_SIZE size;
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_TRUNC_NAME_UC_REQ_MSG;






/**************************************************/
//                              
// end of Unicode EXTENTION: MESSAGES
//                              
/**************************************************/


#endif


/* get attribute message */
#define RFSFAT_GETATTRIB_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x00028)

  typedef struct
  {
    T_RV_HDR hdr;
	const char *path_name;
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_GETATTRIB_REQ_MSG;


/* set attribute message */
#define RFSFAT_SETATTRIB_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x00029)

  typedef struct
  {
    T_RV_HDR hdr;
	const char *path_name;
	T_FFS_ATTRIB_TYPE attrib;
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_SETATTRIB_REQ_MSG;


#if (FFS_UNICODE ==1)

/* get attribute message , for unicode */
#define RFSFAT_GETATTRIB_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0002A)
  typedef struct
  {
    T_RV_HDR hdr;
	const T_WCHAR *path_name;
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_GETATTRIB_UC_REQ_MSG;


/* set attribute message, for unicode */
#define RFSFAT_SETATTRIB_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0002B)

  typedef struct
  {
    T_RV_HDR hdr;
	const T_WCHAR *path_name;
	T_FFS_ATTRIB_TYPE attrib;
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_SETATTRIB_UC_REQ_MSG;

#endif



/* get date and time message */
#define RFSFAT_GETDATETIME_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0002C)

  typedef struct
  {
    T_RV_HDR hdr;
	const char *path_name;
	T_RTC_DATE_TIME *date_time_p;
	T_RTC_DATE_TIME * crdate_time_p;
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_GETDATETIME_REQ_MSG;
 


#if (FFS_UNICODE ==1)

/* get date and time message */
#define RFSFAT_GETDATETIME_UC_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0002D)

  typedef struct
  {
    T_RV_HDR hdr;
	const T_WCHAR *path_name;
	T_RTC_DATE_TIME *date_time_p;
	T_RTC_DATE_TIME * crdate_time_p;
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_GETDATETIME_UC_REQ_MSG;




#endif

/* do the mount and unmount of file system */
#define RFSFAT_MOUNT_UNMOUNT_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0002E)

#define RFSFAT_UNMOUNT 					RFS_UNMOUNT
#define RFSFAT_MOUNT 					RFS_MOUNT


  typedef struct
  {
    T_RV_HDR hdr;
	int op;   /* 0-Unmount, 1-mount */
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_MOUNT_UNMOUNT_REQ_MSG;


/**
 * @name RFSFAT_SET_VOL_LABEL_REQ_MSG
 *
 * Detailled description
 * The T_RFSFAT_SET_VOL_LABEL_REQ_MSG message can be used to set the volume label to a partition. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_SET_VOL_LABEL_RSP.
 */
/*@{*/
/** Trunc request. */
#define RFSFAT_SET_VOL_LABEL_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x0002F)

  typedef struct
  {
       T_RV_HDR hdr;
	const T_WCHAR *path_name;
       const T_WCHAR *vol_label;
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_SET_VOL_LABEL_REQ_MSG;

/**
 * @name RFSFAT_GET_VOL_LABEL_REQ_MSG
 *
 * Detailled description
 * The T_RFSFAT_GET_VOL_LABEL_REQ_MSG message returns the volume label of a partition. 
 * The RFSFAT responds with a RFSFAT_READY_RSP_MSG message,
 * with command index: FSCORE_GET_VOL_LABEL_RSP.
 */
/*@{*/
/** Trunc request. */
#define RFSFAT_GET_VOL_LABEL_REQ_MSG (RFSFAT_MESSAGE_OFFSET | 0x00030)

  typedef struct
  {
       T_RV_HDR hdr;
	const T_WCHAR *path_name;
	T_WCHAR *vol_label;
	T_RV_RETURN_PATH return_path;
	T_FSCORE_RET      pair_id;
  } T_RFSFAT_GET_VOL_LABEL_REQ_MSG;


 /* @name RFSFAT Return type and return values.
 *
 */
/*@{*/
  typedef UINT8 T_RFSFAT_RETURN;

#ifdef __cplusplus
}
#endif


#endif 


