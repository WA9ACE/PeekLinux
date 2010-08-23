#include "p_malloc.h"

long getcwd(char *buf, unsigned long size) {
	buf = p_strdup("/");
	return (long)buf; 
}
