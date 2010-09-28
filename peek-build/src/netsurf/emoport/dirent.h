#ifndef _DIRENT_H_
#define _DIRENT_H_

#define MAXNAMLEN       255  //XXX: fix
#define PATH_MAX	255 //XXX: fix
#define R_OK		0 //XXX: fix

struct dirent {
	unsigned long	d_fileno;		/* file number of entry */
	unsigned short	d_reclen;		/* length of this record */
	unsigned short	d_namlen;		/* length of string in d_name */
	char	d_name[MAXNAMLEN + 1];	/* name must be no longer than this */
};


#define	d_ino		d_fileno	/* backward compatibility */

/* definitions for library routines operating on directories. */
#define	DIRBLKSIZ	1024

/* structure describing an open directory. */
typedef struct _dirdesc {
	long	dd_fd;		/* file descriptor associated with directory */
	long	dd_loc;		/* offset in current buffer */
	char	*dd_buf;	/* data buffer */
	long	dd_len;		/* size of data buffer */
	void	*dd_ddloc;	/* Linked list of ddloc structs for telldir/seekdir */
} DIR;

#define	dirfd(dirp)	((dirp)->dd_fd)

#ifndef NULL
#define	NULL	0
#endif

DIR *opendir (const char *name);
struct dirent *readdir (DIR *dirp);
void rewinddir (DIR *dirp);
int closedir (DIR *dirp);

long telldir (const DIR *dirp);
void seekdir (DIR *dirp, long pos);

#endif /* !_DIRENT_H_ */
