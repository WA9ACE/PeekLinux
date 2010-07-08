#include "ffs.h"
#include "ffsapi.h"

FsiResultT ffsErrorMap(int err) {
	switch(err) {
		case EFFS_OK:
			return FSI_SUCCESS;
		case EFFS_NODEVICE:
		case EFFS_CORRUPTED:
		case EFFS_NOPREFORMAT:
		case EFFS_NOFORMAT:
		case EFFS_BADFORMAT:
		case EFFS_MAGIC:
		case EFFS_AGAIN:
		case EFFS_NOSYS:
		case EFFS_DRIVER:
			return FSI_ERR_SYSTEM;

		case EFFS_NOSPACE:
		case EFFS_FSFULL:
			return FSI_ERR_FULL;
		case EFFS_BADNAME:
		case EFFS_NOTFOUND:
			return FSI_ERR_NOTEXIST;
		case EFFS_EXISTS:
			return FSI_ERR_EXIST;
		case EFFS_ACCESS:
			return FSI_ERR_ACCESS_DENY;
		case EFFS_NAMETOOLONG:
		case EFFS_INVALID:
		case EFFS_DIRNOTEMPTY:
		case EFFS_NOTADIR:
		case EFFS_SPARE:
		case EFFS_FILETOOBIG:
		case EFFS_NOTAFILE:
		case EFFS_PATHTOODEEP:
			return FSI_ERR_UNKNOWN;

		case EFFS_NUMFD:
			return FSI_ERR_MAX_OPEN;
		case EFFS_BADFD:
			return FSI_ERR_BAD_FD;
		case EFFS_BADOP:
		case EFFS_LOCKED:

		case EFFS_TOOBIG:
		case EFFS_MEMORY:
		case EFFS_MSGSEND:
		case EFFS_FWBUF2BIG:
			return FSI_ERR_UNKNOWN;
	   default:
		return FSI_ERR_UNKNOWN;
	}
}

FsiResultT FsiFileOpen(FsiHandleT *FileP, const char *FileNameP, FsiFileOpenModeT Mode) 
{
	uint32 ffsMode;
	int    ffsOret;

	if(!FileNameP || !FileP)
		return (FsiResultT)1;
	switch(Mode) {
		case FSI_FILE_OPEN_READ_EXIST:
			ffsMode = FFS_O_RDONLY;
			break;
		case FSI_FILE_OPEN_WRITE_EXIST:
			ffsMode = FFS_O_APPEND;
			break;
		case FSI_FILE_OPEN_CREATE_NEW:
			ffsMode = FFS_O_CREATE | FFS_O_APPEND;
			break;
		case FSI_FILE_OPEN_CREATE_ALWAYS:
			ffsMode = FFS_O_CREATE | FFS_O_APPEND;
			break;
		case FSI_FILE_OPEN_WRITE_ALWAYS:
			ffsMode = FFS_O_CREATE | FFS_O_APPEND;
			break;
	  default:
	    return (FsiResultT)1;
	}

	ffsOret = ffs_open(FileNameP, ffsMode);
	if(ffsOret < 0)
		return ffsErrorMap(ffsOret);
	*FileP = ffsOret;
	return (FsiResultT)0;
}

FsiResultT FsiFileRead(void *BufferP, uint32 ItemSize, uint32 *ItemNumP,
                       FsiHandleT File) 
{
	int ffsRret;

        if(!BufferP || !ItemSize || !ItemNumP)
                return (FsiResultT)1;
	ffsRret = ffs_read(File, BufferP, ((*ItemNumP) * ItemSize));
	if(ffsRret < 0) 
		return ffsErrorMap(ffsRret);
	*ItemNumP = ffsRret;
	return (FsiResultT)0;
}

FsiResultT FsiFileWrite(void *BufferP, uint32 ItemSize, uint32 *ItemNumP,
                        FsiHandleT File)
{
	int ffsWret;
        if(!BufferP || !ItemSize || !ItemNumP)
                return (FsiResultT)1;
	ffsWret = ffs_write(File, BufferP, ((*ItemNumP) * ItemSize));
        if(ffsWret < 0)
                return ffsErrorMap(ffsWret);
	*ItemNumP = ffsWret;
        return (FsiResultT)0;
}

FsiResultT FsiRemove(const char *NameP)
{
	uint32 ffsRem;

        if(!NameP)
		return (FsiResultT)1;
	ffsRem = ffs_remove(NameP);
	
	return ffsErrorMap(ffsRem);
}

FsiResultT FsiGetFileLength(const char *NameP, uint32 *FileLengthP)
{
	uint32 ffsSret;
	struct stat_s ffsStat;

        if(!NameP || !FileLengthP)
                return (FsiResultT)1;
	ffsSret = ffs_stat(NameP, &ffsStat);
	if(ffsErrorMap(ffsSret))
		return ffsErrorMap(ffsSret);
	*FileLengthP = ffsStat.size;
	return (FsiResultT)0;	
}

FsiResultT FsiFileClose(FsiHandleT File) {
	uint32 ffsCret;

	ffsCret = ffs_close(File);
	return ffsErrorMap(ffsCret);
}
