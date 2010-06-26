/****************************************************************************/
/*  STKDEPTH.C v2.54                                                        */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/*                                                                          */
/*  Stack Depth Analysis Support                                            */
/****************************************************************************/
#include <stdlib.h>
#include <stdio.h>

extern unsigned long main_func_sp  ;  /* SP of main                     */
unsigned long max_stack_required = 0; /* Largest stack seen "so far"    */

void print_max_stack(void)
{
    if (max_stack_required && main_func_sp)
        printf("Max stack used:%d\n", max_stack_required);
}
