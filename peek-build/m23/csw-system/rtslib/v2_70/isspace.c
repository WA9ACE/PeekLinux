/****************************************************************************/
/*  isspace v2.54                                                           */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#undef _INLINE
#include <ctype.h> 

_CODE_ACCESS int isspace(int c)
{
    return(_isspace(c));
}

