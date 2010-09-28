/****************************************************************************/
/*  EXIT.C v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <stdlib.h>
#include <_lock.h>
#if 0
#define MAX_FUN   32
#else
#define MAX_FUN   1
#endif

static int    at_exit_count = 0;
static void (*at_exit_fun[MAX_FUN])();
void        (*_cleanup_ptr)(void) = NULL;
void   _DATA_ACCESS      (*_dtors_ptr)(void)   = NULL;

/****************************************************************************/
/*                                                                          */
/* LOADER_EXIT -							    */
/* 									    */
/* SET C$$EXIT LABEL SO THE DEBUGGER KNOWS WHEN THE C++ PROGRAM HAS	    */
/* COMPLETED.  THIS CAN BE REMOVED IF THE DEBUGGER IS NOT USED.		    */
/*                                                                          */
/****************************************************************************/
static void loader_exit(void)
{
#if defined(EMBED_CIO_BP)
   __asm("         .global C$$EXITE");
#if defined(__32bis__)
   __asm("C$$EXITE:.word 0xDEFED0FE");
#else
   __asm("	 .align  4");
#if defined(__big_endian__)
   __asm("C$$EXITE:.half 0xDEFE");
#else
   __asm("C$$EXITE:.half 0xD0FE");
#endif /* __big_endian__ */
#endif /* __32bis__      */

#else  /* !EMBED_CIO_BP */
   __asm("        .global C$$EXIT");
   __asm("C$$EXIT: nop");
#endif
}

/****************************************************************************/
/* EXIT() - NORMAL PROGRAM TERMINATION.                                     */
/****************************************************************************/
void exit(int status)        
{
   int i;

   /*-------------------------------------------------------------------*/
   /* MUST LOCK WHEN ACCESSING at_exit_count.                           */
   /*-------------------------------------------------------------------*/
   _lock();
   i = at_exit_count;

   /*-------------------------------------------------------------------*/
   /* HANDLE FUNCTIONS REGISTERED THROUGH atexit().                     */
   /*-------------------------------------------------------------------*/
   while (i) (*at_exit_fun[--i])(); 

   /*-------------------------------------------------------------------*/
   /* IF THERE ARE STATIC DESTRUCTORS, CALL THEM.                       */
   /*-------------------------------------------------------------------*/
   if (_dtors_ptr)  (*_dtors_ptr)();

   /*-------------------------------------------------------------------*/
   /* IF FILES ARE POSSIBLY OPEN, _CLEANUP_PTR() WILL BE SETUP TO CLOSE */
   /* THEM.                                                             */
   /*-------------------------------------------------------------------*/
   if (_cleanup_ptr)  (*_cleanup_ptr)();

   loader_exit();

   for (;;);   /* CURRENTLY, THIS SPINS FOREVER */
}

/****************************************************************************/
/* ATEXIT - ATTEMPT TO REGISTER A FUNCTION FOR CALLING AT PROGRAM END       */
/****************************************************************************/
int atexit(void (*fun)())
{
   /*-------------------------------------------------------------------*/
   /* MUST LOCK WHEN ACCESSING at_exit_count AND at_exit_fun.           */
   /*-------------------------------------------------------------------*/
   _lock();
   if (at_exit_count >= MAX_FUN) {
      _unlock();
      return 1;
   }
   at_exit_fun[at_exit_count++] = fun;
   _unlock();
   return 0;
}

/****************************************************************************/
/* ABORT - ABNORMAL PROGRAM TERMINATION.  CURRENTLY JUST HALTS EXECUTION.   */
/****************************************************************************/
void abort(void)
{
    //loader_exit();
    while(1)    /* SPINS FOREVER */
		TCCE_Task_Sleep(2);
}
