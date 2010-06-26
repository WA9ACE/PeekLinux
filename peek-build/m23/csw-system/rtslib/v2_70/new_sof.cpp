/*****************************************************************************/
/* new_sof  v2.54                                                            */
/* Copyright (c) 1996-2004 Texas Instruments Incorporated                    */
/*****************************************************************************/
#include <cstdlib>
#include <new>

namespace std
{
   extern new_handler new_handler_fun;
}

/****************************************************************************/
/*                                                                          */
/* OPERATOR NEW() - DEFAULT GLOBAL ALLOCATION FUNCTION FOR SINGLE OBJECTS.  */
/*                                                                          */
/****************************************************************************/
void *operator new(std::size_t size) // throw(std::bad_alloc)
{
   void *p;

   /*-----------------------------------------------------------------------*/
   /* IF SIZE IS ZERO, INCREASE TO ONE SO THAT A UNIQUE POINTER IS RETURNED.*/
   /*-----------------------------------------------------------------------*/
   if (size == 0) size = 1;

   /*-----------------------------------------------------------------------*/
   /* ATTEMPT TO ALLOCATE THE REQUESTED SPACE.                              */
   /* IF ATTEMPT FAILS, CALL new_handler() IF SET, ELSE THROW AN EXCEPTION. */
   /* THE LOOP TERMINATES WHEN ALLOCATION IS SUCCESSFUL, new_handler()      */
   /* NEVER RETURNS, OR AN EXCEPTION IS THROWN.                             */
   /*-----------------------------------------------------------------------*/
   while (!(p = std::malloc(size)))
   {
      if (std::new_handler_fun)  std::new_handler_fun();
//    else                       throw std::bad_alloc;
      else                       return p;
   }

   return p;
}
