/****************************************************************************/
/*  ltoa v2.54 								    */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <string.h>
#define BUFLEN 20

_CODE_ACCESS int ltoa(long val, char *buffer)
{
    char           tempc[BUFLEN];
    register char *bufptr;
    register int   neg = val < 0;
    register long  uval = neg ? -val : val;

    *(bufptr = &tempc[BUFLEN - 1]) = 0;

    do {*--bufptr = (uval % 10) + '0';}  while(uval /= 10);
    if (neg) *--bufptr = '-';

    memcpy(buffer,bufptr, uval = (tempc + BUFLEN) - bufptr);
    return uval - 1;    /* DON'T COUNT NULL TERMINATION */
}

