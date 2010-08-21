#ifndef __STRINGS_H__
#define __STRINGS_H__

#include <stddef.h>

extern char *strdup(const char *);
extern int strcasecmp(const char *, const char *);
extern int strncasecmp(const char *s1, const char *s2, size_t n);
extern float strtof(const char *, char **);

#endif // __STRINGS_H__
