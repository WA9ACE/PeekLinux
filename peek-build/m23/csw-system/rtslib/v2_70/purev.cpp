/****************************************************************************/
/*  purev     v2.54                                                         */
/*  Copyright (c) 1996-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <cstdlib>

extern "C"
{
   void __pure_virtual_called(void);
}

/****************************************************************************/
/* __PURE_VIRTUAL_CALLED() - A PURE VIRTUAL FUNCTION HAS BEEN CALLED.       */
/*                           NOTIFY THE APPLICATION (WE JUST CALL ABORT FOR */
/*                           NOW).                                          */
/****************************************************************************/
void __pure_virtual_called(void)
{
   std::abort();
}

