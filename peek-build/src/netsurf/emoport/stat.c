#include <sys/stat.h>

#include "relfs.h"
#include "reliance_rtos.h"

int fstat(int fd, struct stat *buf) {
        int ret;
        REL_STAT st;

        ret = relFs_Stat(fd, &st);
        if (ret < 0)
                return -1;

        buf->st_mode = 0;
        if (st.uAttributes & REL_ATTR_DIRECTORY)
                buf->st_mode = S_IFDIR;

        buf->st_size = st.ulSize;
        buf->st_atime = st.ulAccess;
        buf->st_mtime = st.ulModify;
        buf->st_ctime = st.ulCreation;

        return 0;

}

int stat(const char *path, struct stat *buf)
{
	int fd;
	int ret;
	REL_STAT st;

	fd = relFs_Open(path, REL_O_RDONLY, 0x00);
	if (fd < 0)
		return -1;

	ret = relFs_Stat(fd, &st);
	if (ret < 0)
		return -1;

	emo_printf("stat() path: %s Attrs: 0x%08x", path, st.uAttributes);
	buf->st_mode = 0;
	if(st.uAttributes && !(st.uAttributes & REL_ATTR_DIRECTORY)) //XXX: fix me
		buf->st_mode = S_IFREG;
	else if(st.uAttributes & REL_ATTR_DIRECTORY)
		buf->st_mode = S_IFDIR;

	buf->st_size = st.ulSize;
	buf->st_atime = st.ulAccess;
	buf->st_mtime = st.ulModify;
	buf->st_ctime = st.ulCreation;
	
	emo_printf("stat() buf->st_mode: 0x%08x", buf->st_mode);

	relFs_Close(fd);

	return 0;
}

