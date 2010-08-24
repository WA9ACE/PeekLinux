#include <file.h>
#include <string.h>
#include "ffs.h"

static int peek_open(const char *path, unsigned flags, int llv_fd)
{
	int openflag = 0;
	char *colon;

	if ((colon = strchr(path, ':')) != 0)
		path = colon + 1;

	if (!flags) openflag |= FFS_O_RDONLY;
	if ((flags & O_WRONLY) == O_WRONLY) openflag |= FFS_O_WRONLY;
	if ((flags & O_RDWR) == O_RDWR) openflag |= FFS_O_RDWR;
	if ((flags & O_APPEND) == O_APPEND) openflag |= FFS_O_APPEND;
	if ((flags & O_CREAT) == O_CREAT) openflag |= FFS_O_CREATE;
	if ((flags & O_TRUNC) == O_TRUNC) openflag |= FFS_O_TRUNC;

	/* No binary flag in FFS and RO is = 0
	 * So if binary is set and nothing else assume RO
	 * Otherwise we ignore it */ 
        if (flags == O_BINARY) openflag |= FFS_O_RDONLY;


	return ffs_open(path, openflag);
}

static int peek_close(int dev_fd)
{
	return ffs_close(dev_fd);
}

static int peek_read(int dev_fd, char *buf, unsigned count)
{
	return ffs_read(dev_fd, (void *)buf, count);
}

static int peek_write(int dev_fd, const char *buf, unsigned count)
{
	return ffs_write(dev_fd, (void *)buf, count);
}

static off_t peek_seek(int dev_fd, off_t offset, int origin)
{
	int whence = 0;

	if (origin == SEEK_SET) whence = FFS_SEEK_SET;
	else if (origin == SEEK_CUR) whence = FFS_SEEK_CUR;
	else if (origin == SEEK_END) whence = FFS_SEEK_END;

	return ffs_seek(dev_fd, offset, whence);
}

static int peek_unlink(const char *path)
{
	char *colon;

	if ((colon = strchr(path, ':')) != 0)
		path = colon + 1;

	return ffs_remove(path);
}

static int peek_rename(const char *old_name, const char *new_name)
{
	char *colon;

	if ((colon = strchr(old_name, ':')) != 0)
		old_name = colon + 1;

	if ((colon = strchr(new_name, ':')) != 0)
		new_name = colon + 1;

	return ffs_rename(old_name, new_name);
}

void peek_file_init(void)
{
	add_device("peekffs", _MSA, peek_open, peek_close, peek_read, peek_write, peek_seek, peek_unlink, peek_rename);
}

