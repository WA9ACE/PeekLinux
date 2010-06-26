/****************************************************************************/
/*  rand.c v2.54 							    */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <stdlib.h>
#include <_lock.h>

/***************************************************************/
/* THIS IS AN IMPLEMENTATION WHICH IS INTENDED TO BE PORTABLE, */
/* AS PER THE ANSI STANDARD C SPECIFICATION                    */
/***************************************************************/
static _DATA_ACCESS unsigned long next = 1;

_CODE_ACCESS int rand(void)
{
     int r;
     _lock();
     next = next * 1103515245 + 12345;
     r = (int)((next/65536) % ((unsigned long)RAND_MAX + 1));
     _unlock();
     return r;
}

_CODE_ACCESS void srand(unsigned seed)
{
     _lock();
     next = seed;
     _unlock();
}
