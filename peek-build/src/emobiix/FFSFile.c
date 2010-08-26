#include "File.h"
#include "ffs.h"
#include "dirent.h"

struct File_t
{
	T_FFS_FD handle;
	char *filename;
};

File *file_open(const char *filename, uint32 mode)
{
	int ffsOret;
	File *output; 

	ffsOret = ffs_open(filename, mode);
	if (ffsOret < 0)
		return NULL;

	output = (File *)p_malloc(sizeof(File));
	if (!output)
		return NULL;

	output->handle = ffsOret;
	output->filename = (char *)p_strdup(filename);
	return output;
}

File *file_openRead(const char *filename)
{
	return file_open(filename, FFS_O_RDONLY);
}

File *file_openWrite(const char *filename)
{
	return file_open(filename, FFS_O_CREATE | FFS_O_WRONLY);
}

File *file_openAppend(const char *filename)
{
	return file_open(filename, FFS_O_APPEND);
}

int file_read(File *f, int bytes, void *buffer)
{
	return ffs_read(f->handle, buffer, bytes);
}

int file_write(File *f, int bytes, const void *buffer)
{
	return ffs_write(f->handle, (void *)buffer, bytes);
}

int file_seek(File *f, int offset, FilePosition pos)
{
	uint32 whence;

	switch (pos) {
		case FP_START:
			whence = FFS_SEEK_SET;
			break;
		case FP_END:
			whence = FFS_SEEK_END;
			break;
		case FP_CURRENT:
		default:
			whence = FFS_SEEK_CUR;
	}

	return ffs_seek(f->handle, offset, whence);
}

int file_pos(File *f)
{
	return ffs_seek(f->handle, 0, FFS_SEEK_CUR);
}

int file_eof(File *f)
{
	return -1;
}

int file_size(File *f)
{
	int ffsSret;
	struct stat_s ffsStat;

	ffsSret = ffs_stat(f->filename, &ffsStat);
	if (ffsSret < 0)
		return ffsSret;

	return ffsStat.size;
}

int file_close(File *f)
{
	uint32 ffsCret;

	ffsCret = ffs_close(f->handle);
	p_free(f->filename);
	p_free(f);

	return ffsCret;
}

int file_mkdir(const char *directory)
{
	return ffs_mkdir(directory);
}

int file_move(const char *fromfile, const char *tofile)
{
	return ffs_rename(fromfile, tofile);
}

/* posix wrapper stuff */

DIR *opendir(const char *name)
{
	DIR *retdir;
	struct dir_s tdir;
	int dirRet;

	dirRet = ffs_opendir(name, &tdir);
	
	if(dirRet < 0) {

	}

	retdir = (DIR *)malloc(sizeof(DIR));

	return retdir;
}

struct dirent *readdir (DIR *dirp)
{
//int FEXPORT ffs_readdir(struct dir_s *dir, char *name, int size);
	

}

int closedir(DIR *dir) {
//	ffs_closedir(&dir);
}

