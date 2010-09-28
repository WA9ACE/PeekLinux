#include "p_malloc.h"

char *getcwd(char *buf, unsigned long size) {
	if (!buf)
		return 0;

	strcpy(buf, "/");
	return buf;
}
