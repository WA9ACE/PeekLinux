/*---------------------------------------------------------------------------
               ----> DO NOT REMOVE THE FOLLOWING NOTICE <----

      Copyright (c) 2003 - 2006 Datalight, Inc.  All Rights Reserved.

  Datalight, Incorporated is a Washington State corporation located at:

        21520 30th Dr SE, Suite 110,      Tel:  425-951-8086
        Bothell, WA  98021                Fax:  425-951-8094
        USA                               Http://www.datalight.com

  This software, including without limitation, all source code and documen-
  tation, is the confidential, trade secret property of Datalight, Inc., and
  is protected under the copyright laws of the United States and other juris-
  dictions.

  ---> Portions of the software are patent pending <---

  In addition to civil penalties for infringement of copyright under applic-
  able U.S. law, 17 U.S.C. 1204 provides criminal penalties for violation of
  (a) the restrictions on circumvention of copyright protection systems found
  in 17 U.S.C. 1201 and (b) the protections for the integrity of copyright
  management information found in 17 U.S.C. 1202.

  U.S. Government Restricted Rights:  Use, duplication, reproduction, or
  transfer of this commercial product and accompanying documentation is
  restricted in accordance with FAR 12.212 and DFARS 227.7202 and by a
  License Agreement.

  IN ADDITION TO COPYRIGHT AND PATENT LAW, THIS SOFTWARE IS PROTECTED UNDER A
  SOURCE CODE AGREEMENT, NON-DISCLOSURE AGREEMENT (NDA), OR SIMILAR BINDING
  CONTRACT BETWEEN DATALIGHT, INC. AND THE LICENSEE ("BINDING AGREEMENT").
  YOUR RIGHT, IF ANY, TO COPY, PUBLISH, MODIFY OR OTHERWISE USE THE SOFTWARE,
  IS SUBJECT TO THE TERMS AND CONDITIONS OF THE BINDING AGREEMENT.  BY USING
  THE SOFTWARE IN ANY MANNER, IN WHOLE OR IN PART, YOU AGREE TO BE BOUND BY
  THE TERMS OF THE BINDING AGREEMENT.  CONTACT DATALIGHT, INC. AT THE ADDRESS
  SET FORTH ABOVE IF YOU OBTAINED THIS SOFTWARE IN ERROR.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                              Description

    This header defines the Datalight POSIX interface for Reliance.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                              Revision History
    $Log: reliance_posix.h $
    Revision 1.2  2006/03/08 02:10:52  Pauli
    Added an error code.
    Revision 1.1  2006/03/06 19:07:04Z  Pauli
    Initial revision
---------------------------------------------------------------------------*/

#ifndef RELIANCE_POSIX_H_INCLUDED
#define RELIANCE_POSIX_H_INCLUDED

#include "reliance_rtos.h"


#define DLP_OPEN_MAX        REL_MAX_OPEN_FILES


/*  Seek Origin
*/
#define DLP_SEEK_SET        REL_SEEK_SET
#define DLP_SEEK_CUR        REL_SEEK_CUR
#define DLP_SEEK_END        REL_SEEK_END


/*  Open Flags
*/
#define DLP_O_RDONLY        REL_O_RDONLY
#define DLP_O_WRONLY        REL_O_WRONLY
#define DLP_O_RDWR          REL_O_RDWR
#define DLP_O_APPEND        REL_O_APPEND
#define DLP_O_CREAT         REL_O_CREAT
#define DLP_O_TRUNC         REL_O_TRUNC
#define DLP_O_EXCL          REL_O_EXCL
#define DLP_O_BINARY        0


/*  File permissions
*/
#define DLP_S_IRUSR         0000400
#define DLP_S_IWUSR         0000200
#define DLP_S_IXUSR         0000100
#define DLP_S_IRWXU         (DLP_S_IRUSR | DLP_S_IWUSR | DLP_S_IXUSR)
#define DLP_S_IRGRP         0000040
#define DLP_S_IWGRP         0000020
#define DLP_S_IXGRP         0000010
#define DLP_S_IRWXG         (DLP_S_IRGRP | DLP_S_IWGRP | DLP_S_IXGRP)
#define DLP_S_IROTH         0000004
#define DLP_S_IWOTH         0000002
#define DLP_S_IXOTH         0000001
#define DLP_S_IRWXO         (DLP_S_IROTH | DLP_S_IWOTH | DLP_S_IXOTH)
#define DLP_S_ISUID         0004000
#define DLP_S_ISGID         0002000
#define DLP_S_ISVTX         0001000


/* File types
*/
#define DLP_S_IFMT          0170000 /* File type mask */
#define DLP_S_IFBLK         0060000 /* Block device special */
#define DLP_S_IFCHR         0020000 /* Character special */
#define DLP_S_IFDIR         0040000 /* Directory */
#define DLP_S_IFIFO         0010000 /* FIFO special */
#define DLP_S_IFLNK         0120000 /* Symbolic link */
#define DLP_S_IFREG         0100000 /* Regular */
#define DLP_S_IFSOCK        0140000 /* Socket special */


/*  File type macros to test the file type.
*/
#define DLP_S_ISBLK(m)      (m & DLP_S_IFBLK)
#define DLP_S_ISCHR(m)      (m & DLP_S_IFCHR)
#define DLP_S_ISDIR(m)      (m & DLP_S_IFDIR)
#define DLP_S_ISFIFO(m)     (m & DLP_S_IFIFO)
#define DLP_S_ISLNK(m)      (m & DLP_S_IFLNK)
#define DLP_S_ISREG(m)      (m & DLP_S_IFREG)
#define DLP_S_ISSOCK(m)     (m & DLP_S_IFSOCK)


/*  File system attributes
*/
#define DLP_ST_RDONLY       0x0001  /*  Read-only */
#define DLP_ST_NOSUID       0x0002  /*  Does not support setuid/setgid-bit semantics. */
#define DLP_ST_QUOTA        0x0004  /*  Supports quotas. */
#define DLP_ST_NOTRUNC      0x0008  /*  Does not truncate files longer than NAME_MAX. */
#define DLP_ST_CASE_PRES    0x0010  /*  Preserves case of file names. */
#define DLP_ST_CASE_SENS    0x0020  /*  Supports case-sensitive file names. */
#define DLP_ST_UNICODE      0x0040  /*  Supports UNICODE path names. */
#define DLP_ST_ACLS         0x0080  /*  Supports persistent ACLs for security. */
#define DLP_ST_FILECOMP     0x0100  /*  Supports per-file compression. */
#define DLP_ST_COMPRESSED   0x0200  /*  Is compressed. */
#define DLP_ST_CRYPT        0x0400  /*  Supports encryption. */
#define DLP_ST_OBJ_IDS      0x0800  /*  Supports object IDs. */
#define DLP_ST_REPARSE      0x1000  /*  Supports reparse points. */
#define DLP_ST_SPARSE       0x2000  /*  Supports sparse files */


/*  Error codes
*/
#define DLP_ENOENT          1
#define DLP_EINVAL          2
#define DLP_EEXIST          3
#define DLP_ENOTEMPTY       4
#define DLP_EBADF           5
#define DLP_EPERM           6


typedef int                 DLP_size_t;
typedef unsigned int        DLP_mode_t;
typedef long                DLP_off_t;
typedef unsigned short      DLP_dev_t;
typedef unsigned long       DLP_ino_t;
typedef unsigned short      DLP_nlink_t;
typedef unsigned short      DLP_uid_t;
typedef unsigned short      DLP_gid_t;
typedef unsigned long       DLP_time_t;
typedef int                 DLP_blksize_t;
typedef unsigned long       DLP_blkcnt_t;


#define DLP_FSTYPSZ 16


struct DLP_stat {
    DLP_mode_t  st_mode;            /*  file mode/type */
    DLP_ino_t   st_ino;             /*  file serial number */
    DLP_dev_t   st_dev;             /*  device number where this file is located */
    DLP_nlink_t st_nlink;           /*  number of links to the file */
    DLP_uid_t   st_uid;             /*  user id */
    DLP_gid_t   st_gid;             /*  group id */
    DLP_off_t   st_size;            /*  size of the file, in bytes */
    DLP_time_t  st_atime;           /*  access time */
    DLP_time_t  st_mtime;           /*  modified time */
    DLP_time_t  st_ctime;           /*  creation time */
};


struct DLP_statvfs {
	unsigned long f_bsize;          /*  block size */
	unsigned long f_frsize;         /*  fundamental size (fragmentation size) */
	DLP_blkcnt_t f_blocks;          /*  total blocks */
	DLP_blkcnt_t f_bfree;           /*  blocks free */
	DLP_blkcnt_t f_bavail;          /*  blocks available for non-priviledged processes */
	DLP_blkcnt_t f_files;           /*  total file nodes */
	DLP_blkcnt_t f_ffree;           /*  file nodes free */
	DLP_blkcnt_t f_favail;          /*  file nodes available for non-priviledged processes */
	unsigned long f_fsid;           /*  file system id */
	unsigned long f_flag;           /*  */
	unsigned long f_namemax;        /*  maximum name length */
	unsigned long f_type;           /*  file system type */
	char f_basetype[DLP_FSTYPSZ + 1]; /*  file system name */
	char f_str[DLP_FSTYPSZ + 1];    /*  volume label */
};


struct DLP_dirent{
    char        d_name[REL_MAX_PATH + 1];
};


typedef void DLP_DIR;



/*  POSIX API
*/
int             dlp_chdir( const char * );
int             dlp_close( int );
int             dlp_closedir( DLP_DIR * );
int	            dlp_creat( const char *, DLP_mode_t );
int             dlp_errnoget(void);
int             dlp_errnoset(int iErr);
int             dlp_fstat( int, struct DLP_stat * );
int             dlp_fstatvfs( int, struct DLP_statvfs * );
int             dlp_fsync( int );
int             dlp_ftruncate( int, DLP_off_t );
char *          dlp_getcwd( char *, DLP_size_t);
int             dlp_link ( const char *, const char * );
DLP_off_t       dlp_lseek( int, DLP_off_t, int );
int             dlp_mkdir( const char * );
int             dlp_open( const char *, int, ... );
DLP_DIR *       dlp_opendir( const char * );
DLP_size_t      dlp_read( int, char *, DLP_size_t );
struct DLP_dirent * dlp_readdir( DLP_DIR * );
int             dlp_remove( const char * );
int             dlp_rename( const char *, const char * );
int             dlp_rmdir( const char * );
int             dlp_stat( const char *, struct DLP_stat * );
int             dlp_statvfs( const char *, struct DLP_statvfs *);
int             dlp_unlink ( const char * );
DLP_size_t      dlp_write( int, char *, DLP_size_t );



#endif
