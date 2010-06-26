/****************************************************************************/
/*  clock v2.54                                                             */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/

/****************************************************************************/
/* OBVIOUSLY, THIS IS SIMPLY A STUB, WHICH BY DEFINITION RETURNS -1         */
/* IF THE SYSTEM TIME IS UNAVAILABLE.                                       */ 
/*--------------------------------------------------------------------------*/
/* OBVIOUSLY, THIS IS NO LONGER A STUB.  THIS WILL RETURN THE CLK REGISTER  */
/* FROM THE EMULATOR OR SIMULATOR.  THIS IS A CYCLE ACURATE COUNT OF THE    */ 
/* NUMBER OF CYCLES THAT HAVE ELAPSED SINCE THE START OF THE PROGRAM.       */ 
/****************************************************************************/
#include <time.h> 
extern clock_t HOSTclock();

_CODE_ACCESS clock_t clock(void)   
{
    return (clock_t)HOSTclock();
}
