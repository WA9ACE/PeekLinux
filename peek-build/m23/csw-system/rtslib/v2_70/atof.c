/****************************************************************************/
/*  atof v2.54                                                              */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/

#ifndef _STDLIB
#undef _INLINE
#endif

#include <stdlib.h>

_CODE_ACCESS double atof(const char *st)
{
    return strtod(st, (char **)0);
}
