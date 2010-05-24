/**
 * @file rfsnand_api.h
 *
 * API Definition for RFSNAND SWE.
 *
 * @author  D. Meijer (dolf.meijer@ict.nl)
 * @version 0.1
 */

/*
 * History:
 *
 * Date        Author               Modification
 * -------------------------------------------------------------------
 * 2/23/2004   D. Meijer (dolf.meijer@ict.nl    Create.
 *
 * (C) Copyright 2004 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __RFSNAND_API_H_
#define __RFSNAND_API_H_

#include "rfs/fscore_types.h"
#include "rvm/rvm_gen.h"             /* Generic RVM types and functions. */
#include "rfsnand/rfsnand_message.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name RFSNAND Return type and return values.
 *
 * Currently they are the standard RV return types, but they may
 * be customized in the future.
 */
/*@{*/
typedef T_RV_RET   T_RFSNAND_RETURN;

#define  RFSNAND_OK              RV_OK
#define  RFSNAND_NOT_SUPPORTED   RV_NOT_SUPPORTED
#define  RFSNAND_MEMORY_ERR      RV_MEMORY_ERR
#define  RFSNAND_INTERNAL_ERR    RV_INTERNAL_ERR
#define  RFSNAND_INVALID_PARAM   RV_INVALID_PARAMETER
/*@}*/

/**
 * @name API functions
 *
 * API functions declarations (bridge functions).
 */
/*@{*/

/**
 * rfsnand_set_partition_table
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
 * @return  FSCORE_EOK       Ok
 * @return  FSCORE_EACCES    Not able to handle this request at this moment 
 *                            (SWE not initialised)..
 * @return  FSCORE_EINVALID  Invalid argument (Invalid command or invalid return_path)
 * @return  FSCORE_EMEMORY   Insufficient RAM resources to process the request.
 */
extern T_FSCORE_RET rfsnand_set_partition_table( T_FSCORE_PARTITION_TABLE *partition_table,
                                                 T_RV_RETURN               return_path);

/**
 * rfsnand_get_func_table
 *
 * Detailled description.
 * This function returns a pointer to the FS-core function table.
 * This function is synchronous.
 *
 * @param   func_table_p:     A pointer to the function table structure. 
 *
 * @return  FSCORE_EOK        Ok
 * @return  FSCORE_EINVALID   Invalid argument (func_table_p == NULL)
**/
extern T_FSCORE_RET rfsnand_get_func_table (T_FSCORE_FUNC_TABLE  *func_table_p);


/**
 * rfsnand_fcntl
 *
 * Detailled description.
 * This function provides control on the properties of a file that is already open. 
 * The argument fd is a descriptor to be operated on by cmd as described below. 
 * The third parameter is called arg and is technically a pointer to void, but the 
 * interpretation depends on the command.
 *
 * This function is synchronous. Switching from synchronous to asynchronous 
 * operations or vice-versa, by setting flag F_SETFL, has only effect on the succeeding
 * operations (like file writing or reading) and not on the function rfsnand_fcntl() 
 * itself. When a switch from asynchronous operations to synchronous operations is 
 * made and there are some pending asynchronous operations, the return_path of these
 * pending operations should stay valid. 
 *
 * @param   fd          File descriptor obtained when the file was opened.
 * @param   cmd         The commands are:
 *                      F_SETFL       Set file operation mode associated with file 
 *                                    descriptor fd (arg is interpreted as an UINT8)
 *                      F_GETFL       Get file operation mode associated with fd (arg is 
 *                                    ignored).
 *                      F_SETRETPATH  Set return path to be used for notification
 *                                    (arg is interpreted as an pointer to T_RV_RETURN)
 *
 * @param   arg_p       -In case parameter is F_SETFL the argument is *UINT8:
 *                      O_AIO   Force operate asynchronously, 
 *                              the caller will be notified either by the return path
 *                              (callback function or message).
 *                      O_SIO   Default blocking (i.e: synchronous) I/O operations
 *                      -In case parameter is F_GETFL: no argument required(return value)
 *                      -In case parameter is F_SETRETPATH the argument is *T_RV_RETURN:
 *                       The Riviera return call/message information
 *
 * @return  (positive value)  Value of flags (in case of successful F_GETFL command
 *                            execution)
 * @return  FSCORE_EOK        Ok (in case of successful command execution others than 
 *                            F_GETFL)
 * @return  FSCORE_EBADFD     Invalid file descriptor.
 * @return  FSCORE_EINVALID   Invalid argument (Invalid command or invalid return_path)
 */                 
extern T_FSCORE_RET rfsnand_fcntl (T_FSCORE_FD fd, INT8 cmd, void *arg_p);

/**
 * rfsnand_close
 *
 * Detailled description.
 * This function closes an open file. 
 *
 * @param   fd              File descriptor obtained when the file was opened.
 *
 * @return  FSCORE_EOK      Ok
 * @return  FSCORE_EBADFD   The file argument is not a valid file descriptor.
 */
extern T_FSCORE_RET rfsnand_close (T_FSCORE_FD fd);

/**
 * rfsnand_write
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
 * @return  (Positive value)    Number of bytes actually written.
 * @return  FSCORE_EBADFD       The file argument is not a valid file descriptor.
 * @return  FSCORE_EACCES       The file is not writable.
 * @return  FSCORE_EBADOP       The file is not open for writing.
 * @return  FSCORE_EFBIG        An attempt was made to write a file that exceeds the 
 *                              maximum file size. 
 * @return  FSCORE_ENOSPACE     Out of data space.
 * @return  FSCORE_EINVALID     Negative size given 
 * @return  FSCORE_EDEVICE      Device I/O error
 */
extern T_FSCORE_SIZE rfsnand_write( T_FSCORE_FD     fd,
                                    const void     *buf,
                                    T_FSCORE_SIZE  size);

/**
 * rfsnand_read
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
 * @return  (Positive value)    Number of bytes actually read.
 * @return  FSCORE_EBADFD       The file argument is not a valid file descriptor.
 * @return  FSCORE_EACCES       The file is not readable.
 * @return  FSCORE_EBADOP       The file is not open for reading.
 * @return  FSCORE_ENOSPACE     Out of data space.
 * @return  FSCORE_EDEVICE      Device I/O error
 */
extern T_FSCORE_SIZE rfsnand_read(  T_FSCORE_FD    fd,
                                   const void     *buf, 
                                   T_FSCORE_SIZE  size);

/**
 * rfsnand_lseek
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
 *                      FSCORE_SEEK_SET Absolute offset from start of file
 *                      FSCORE_SEEK_CUR the offset is set to its current location plus
 *                                    offset bytes
 *                      FSCORE_SEEK_END the offset is set to the size of the file plus
 *                                    offset bytes.
 *
 * @return  (Positive value)  New position of the file pointer
 * @return  FSCORE_EBADFD     The fd argument is not a valid file descriptor.
 * @return  FSCORE_EINVALID   The whence argument is not a proper value, or the 
 *                            resulting file offset would be invalid.
 * @return  FSCORE_EBADOP     Bad operation. Seek not allowed with the flags used to
 *                            open the file.
 * @return  FSCORE_EDEVICE    Device I/O error
 */
extern T_FSCORE_OFFSET rfsnand_lseek (T_FSCORE_FD       fd,
                                      T_FSCORE_OFFSET  offset,
                                      T_FSCORE_WHENCE  whence);

/**
 * rfsnand_fchmod
 *
 * Detailled description.
 * The function rfsnand_fchmod() sets the permission bits of the specified file 
 * descriptor fd to required mode.
 * A mode is created from OR'd permission bit masks defined by T_FSCORE_MODE.
 * 
 * @param  fd           File descriptor obtained when the file was opened.
 * @param  mode         Specifies the attribute (permission bits) of the file
 *
 * @return  FSCORE_EOK          Ok
 * @return  FSCORE_EBADFD       The fd argument is not a valid file descriptor.
 * @return  FSCORE_ENAMETOOLONG Object's name is too long.
 * @return  FSCORE_ENOTDIR      A component of the path prefix is not a directory.
 * @return  FSCORE_EACCES       Search permission is denied for a component of the 
 *                              path prefix.
 * @return  FSCORE_EBADNAME     Object's name contains illegal characters.
 * @return  FSCORE_ENOTAFILE    Object is not a file.
 * @return  FSCORE_ENOENT       No such file or directory.
 * @return  FSCORE_EINVALID     Bad mod option
 * @return  FSCORE_ELOCKED      The file is locked (already opened in a conflicting 
 *                              mode).
 */
extern T_FSCORE_RET rfsnand_fchmod (T_FSCORE_FD    fd,
                                    T_FSCORE_MODE  mode);

/**
 * rfsnand_fstat
 *
 * Detailled description.
 * The rfsnand_fstat() function obtains information about an open file associated by the 
 * file descriptor fd.
 *
 * @param   fd          File descriptor obtained when the file was opened.
 * @param   stat        Contains information (meta-data) about the specified object.
 *
 * @return  FSCORE_EOK            Ok.
 * @return  FSCORE_ENOENT         Object not found.
 * @return  FSCORE_ENOTDIR        A component of the path prefix is not a directory.
 * @return  FSCORE_EBADFD         Bad file descriptor.
 * @return  FSCORE_EACCES         Search permission is denied for a component of the path 
 *                                prefix.
 * @return  FSCORE_ENAMETOOLONG   Object's name is too long.
 * @return  FSCORE_EBADNAME       Object's name contains illegal characters.
 */
extern T_FSCORE_RET rfsnand_fstat( T_FSCORE_FD   fd,
                                   T_FSCORE_STAT *stat);

/**
 * rfsnand_fsync
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
 * @return  FSCORE_EOK        Ok.
 * @return  FSCORE_EBADFD     Invalid file descriptor.
 * @return  FSCORE_ENOSPACE   Out of data space.
 * @return  FSCORE_EFSFULL    File system full, no free inodes.
 * @return  FSCORE_EDEVICE    Device I/O error
 */
extern T_FSCORE_RET rfsnand_fsync (T_FSCORE_FD fd);

/**
 * rfsnand_open
 *
 * Detailled description.
 * The file specified by pathname is opened for reading and/or writing as specified 
 * by the argument flags and the file descriptor is returned to the calling process. 
 * The flags argument may indicate the file is to be created if it does not exist 
 * (by specifying the rfsnand_O_CREAT flag), in which case the file is created with mode
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
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES     The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY    Insufficient memory to create message request
 * @return  FSCORE_EINVALID   Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND   Error while sending the internal message
 */
extern T_FSCORE_RET rfsnand_open( const char      *pathname, 
                                  T_FSCORE_FLAGS  flags, 
                                  T_FSCORE_MODE   mode,
                                  T_RV_RETURN     return_path);
/**
 * rfsnand_chmod
 *
 * Detailled description.
 * The function rfsnand_chmod() sets the file permission bits of the file specified by 
 * the pathname pathname to required mode. 
 * 
 * A mode is created from OR'd permission bit masks defined by T_FSCORE_MODE.
 *
 * @param   pathname    Null terminated string containing the unique name of the 
 *                      file for changing the mode.
 * @param   mode        Specifies the attribute (permission bits) of the file
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES     The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY    Insufficient memory to create message request
 * @return  FSCORE_EINVALID   Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND   Error while sending the internal message
 */
extern T_FSCORE_RET rfsnand_chmod(const char        *pathname, 
                                  T_FSCORE_MODE     mode,
                                  T_RV_RETURN       return_path);
/**
 * rfsnand_stat
 *
 * Detailled description.
 * The rfsnand_stat() function obtains information about the file or device associated to 
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
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES     The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY    Insufficient memory to create message request
 * @return  FSCORE_EINVALID   Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND   Error while sending the internal message
 */
extern T_FSCORE_RET rfsnand_stat( const char     *pathname,
                                  T_FSCORE_STAT  *stat,
                                  T_RV_RETURN    return_path);

/**
 * rfsnand_remove
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
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES     The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY    Insufficient memory to create message request
 * @return  FSCORE_EINVALID   Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND   Error while sending the internal message
 */
extern T_FSCORE_RET rfsnand_remove( const char  *pathname,
                                    T_RV_RETURN return_path);

/**
 * rfsnand_rename
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
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES     The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY    Insufficient memory to create message request
 * @return  FSCORE_EINVALID   Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND   Error while sending the internal message
 */
extern T_FSCORE_RET rfsnand_rename( const char  *oldname, 
                                    const char  *newname, 
                                    T_RV_RETURN return_path);
/**
 * rfsnand_mkdir
 *
 * Detailled description.
 * This function creates a directory with the pathname given by pathname. The 
 * pathname is a null termi-nated string. All components of the pathname must be 
 * already existing directories. This means that it is not possible to 
 * rfsnand_mkdir("/gsm/rf/tx") if the directories /gsm/ and /gsm/rf are not already 
 * created.
 *
 * @param   pathname    Null terminated string containing the unique name of the
 *                      directory to create
 * @param   mode        Specifies the attribute (permission bits) of the directory
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES     The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY    Insufficient memory to create message request
 * @return  FSCORE_EINVALID   Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND   Error while sending the internal message
 */
extern T_FSCORE_RET rfsnand_mkdir(const char        *pathname, 
                                  T_FSCORE_MODE   mode,
                                  T_RV_RETURN      return_path);

/**
 * rfsnand_rmdir
 *
 * Detailled description.
 * This function removes a directory file whose name is given by pathname. The 
 * directory must be empty.
 *
 * @param   pathname    Null terminated string containing the unique name of the 
 *                      directory to remove.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES     The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY    Insufficient memory to create message request
 * @return  FSCORE_EINVALID   Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND   Error while sending the internal message
 */
extern T_FSCORE_RET rfsnand_rmdir(const char  *pathname, T_RV_RETURN return_path);

/**
 * rfsnand_opendir
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
 *                      consequitive rfsnand_readdir().
 *                      It is representing a directory stream.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES     The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY    Insufficient memory to create message request
 * @return  FSCORE_EINVALID   Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND   Error while sending the internal message
 */
extern T_FSCORE_RET rfsnand_opendir(const char      *pathname,
                                    T_FSCORE_DIR    *dirp,
                                    T_RV_RETURN     return_path);

/**
 * rfsnand_readdir
 *
 * Detailled description.
 * This function reads an entry from a directory previously opened by rfsnand_opendir().
 * The input parameter is a pointer to the opened directory stream (specified by the
 * argument dirp). The structure associated with the pointer dirp, keeps track of the
 * directory entry last read by the function rfsnand_readdir(). To achieve this, the 
 * pointer dirp is also an output parameter.
 *
 * The other (output) parameters concern directory entry information, which is read. 
 * This information consists of a pointer to a buffer containing the name of the entry 
 * and the size of the buffer. The buffer memory needs to be allocated by the client. 
 *
 * A positive return value denotes that the buffer pointed to by buf (member of the
 * structure pointed by direntp) contains the null-terminated name of the entry found.
 * A zero is returned if there were no more entries in the directory and the buffer 
 * pointed to by buf is left untouched. The function returns a nega-tive value if an 
 * exception is encountered.
 *
 * In order to read all entries in a directory, rfsnand_readdir() should be called until
 * it returns zero.
 *
 * @param   dirp      Pointer to a T_FSCORE_DIR structure obtained in a previous call to 
 *                    rfsnand_opendir(), representing a directory stream.
 * @param   buf       Pointer to buffer into which the found name shall be returned
 * @param   size      size of buffer 'buf'. max 'size' characters may be copied in 'buf'
 * @param   buf       Pointer to buffer into which the found name shall be returned
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES     The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY    Insufficient memory to create message request
 * @return  FSCORE_EINVALID   Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND   Error while sending the internal message
 */
extern T_FSCORE_RET rfsnand_readdir(T_FSCORE_DIR    *dirp, 
                                    char            *buf,
                                    T_FSCORE_SIZE   size,
                                    T_RV_RETURN     return_path);
/**
 * rfsnand_preformat
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
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES     The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY    Insufficient memory to create message request
 * @return  FSCORE_EINVALID   Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND   Error while sending the internal message
 * @return  FSCORE_EMAGIC     Bad magic number given.
 */
extern  T_FSCORE_RET rfsnand_preformat (const char  *pathname,
                                        UINT16      magic,
                                        T_RV_RETURN return_path);

/**
 * rfsnand_format
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
 * This function must be called after rfsnand_preformat() and before any other operation
 * on RFS. In order to avoid spurious calls of this dangerous, unrecoverable function,
 * the magic number must have the hexadecimal value of 0x2BAD to format the flash.
 *
 * @param   pathname    Null terminated string containing the name of the partition
 *                      to format.
 * @param   name        Null terminated string containing the name of the RFS volume.
 * @param   magic       Magic value to access the function. Must be 0x2BAD.
 * @param   return_path Return path for notifications.
 *
 * @return  (Positive value)  Unique pair ID used to pair a initiated request with a 
 *                            received response.
 * @return  FSCORE_EACCES     The RFS is not able to handle this request at this moment.
 * @return  FSCORE_EMEMORY    Insufficient memory to create message request
 * @return  FSCORE_EINVALID   Invalid argument (return_path is invalid)
 * @return  FSCORE_EMSGSEND   Error while sending the internal message
 * @return  FSCORE_EMAGIC     Bad magic number given.
 */
extern T_FSCORE_RET rfsnand_format( const char  *pathname,
                                    const char  *name,
                                    UINT16      magic,
                                    T_RV_RETURN return_path);
/*@}*/

#ifdef __cplusplus
}
#endif

#endif /*__RFSNAND_API_H_*/

