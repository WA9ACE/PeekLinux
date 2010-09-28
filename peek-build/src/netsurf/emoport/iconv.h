#ifndef _ICONV_H
#define _ICONV_H

#include <stddef.h>

#define __THROW

typedef int *iconv_t;

extern iconv_t iconv_open(const char *tocode, const char *fromcode) __THROW;
extern int iconv_close(iconv_t cd) __THROW;
extern size_t iconv(iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft) __THROW;

#define libiconv_open   iconv_open	
#define libiconv_close  iconv_close	
#define libiconv        iconv				

#endif
