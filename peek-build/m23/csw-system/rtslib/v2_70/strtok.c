/****************************************************************************/
/*  strtok v2.54                                                            */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <string.h>

_CODE_ACCESS char *strtok(char *str1, const char *str2)
{
     static _DATA_ACCESS char *last_end;
     char *start; 

     if (str1) last_end = str1;

     last_end += strspn(last_end, str2);
     if (*last_end == '\0') return (NULL);

     start = last_end;
     last_end += strcspn(last_end, str2);

     if (*last_end != '\0') *last_end++ = '\0'; 

     return start;
}

