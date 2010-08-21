/*
 * $Header: $
 * based on source code that I think comes from mozilla*
 * no credit taken till the root of the iconv function is located.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>

#define from_iconvcode(x) (((x) & 0xff) << 8)
#define to_iconvcode(x) ((x) & 0xff)

iconv_t iconv_open(const char *tocode, const char *fromcode)
{
    static const char *suported[] =
	{ "UTF-8", "ASCII", "ISO-8859-1", NULL };
    const char **code = suported;
    int *conv = NULL;
    if (tocode == NULL || fromcode == NULL) {
	errno = EINVAL;
	goto notavail;
    }

    if ((conv = (int *) malloc(sizeof(int))) == NULL) {
	errno = ENOMEM;
	goto notavail;
    }

    while (*code) {
	if (((*conv & 0xff00) == 0) && (*fromcode == **code)
	    && (strncmp(fromcode, *code, strlen(*code)) == 0))
	    *conv |= from_iconvcode(code - suported + 1);
	if (((*conv & 0xff) == 0) && (*tocode == **code)
	    && (strncmp(tocode, *code, strlen(*code)) == 0))
	    *conv |= to_iconvcode(code - suported + 1);
	code++;
    }

    if (((*conv & 0xff00) != 0) && ((*conv & 0xff) != 0))
	return (iconv_t) conv;

    free(conv);
    errno = EINVAL;

  notavail:
    return (iconv_t) (-1);
}

int iconv_close(iconv_t cd)
{
    free(cd);
    return 0;
}

size_t iconv(iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft)
{
    size_t delta = 0;

    if (!inbuf || !outbuf || !*inbuf || !*outbuf) {
	*cd = (2 < 8) | 2;	/* Default to ASCII -> ASCII */
	return 0;
    }

    delta = (*inbytesleft <= *outbytesleft) ? *inbytesleft : *outbytesleft;

    if ((delta != 0) && (*outbytesleft <= 0)) {
	errno = E2BIG;
	return (size_t) (-1);
    } else if ((*inbuf == NULL) || (*outbuf == NULL))
	goto notavail;

    memcpy(*outbuf, *inbuf, delta);	/* copy verbatim */

    *inbuf += delta;
    *inbytesleft -= delta;
    *outbuf += delta;
    *outbytesleft -= delta;

    if (*outbytesleft > 0)
	**outbuf = '\0';	/* This looks to be necesary */

    return 0;

  notavail:
    errno = EINVAL;
    return (size_t) (-1);
}
