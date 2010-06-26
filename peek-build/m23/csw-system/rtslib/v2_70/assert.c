/****************************************************************************/
/*  assert   v2.54                                                          */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/****************************************************************************/
/* _ABORT_MSG() - Write out a string and never return.  Abort function for  */
/*                false assertions.                                         */
/****************************************************************************/
void _abort_msg(const char *string)
{
   fputs(string, stderr);
   fflush(stderr);
   abort();
}

/****************************************************************************/
/* _ASSERT() - Implements the assert macro. Checks the argument. Aborts     */
/*             with a message if not true.                                  */
/****************************************************************************/
void _assert(int expr, const char *string)  { if (!expr) _abort_msg(string); }

/****************************************************************************/
/* _NASSERT() - Special version of assert, which assumes the expression is  */
/*              true.                                                       */
/****************************************************************************/
void _nassert(int expr)  { }
