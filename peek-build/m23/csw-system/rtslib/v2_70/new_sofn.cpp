/*****************************************************************************/
/* new_sofn  v2.54                                                           */
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
/* OPERATOR NEW() - NO EXCEPTION VERSION OF THE DEFAULT GLOBAL ALLOCATION   */
/*                  FUNCTION FOR SINGLE OBJECTS.                            */
/*                                                                          */
/****************************************************************************/
void *operator new(std::size_t size, const std::nothrow_t&) // throw()
{
   void *p;

   /*-----------------------------------------------------------------------*/
   /* IF SIZE IS ZERO, INCREASE TO ONE SO THAT A UNIQUE POINTER IS RETURNED.*/
   /*-----------------------------------------------------------------------*/
   if (size == 0) size = 1;

   /*-----------------------------------------------------------------------*/
   /* ATTEMPT TO ALLOCATE THE REQUESTED SPACE.                              */
   /* IF ATTEMPT FAILS, CALL new_handler() IF SET, ELSE RETURN NULL.        */
   /* IF THE CALL TO new_handler() THROWS A std::bad_alloc EXCEPTION,       */
   /* CATCH IT AND RETURN NULL.                                             */
   /*-----------------------------------------------------------------------*/
   while (!(p = std::malloc(size)))
   {
      if (!std::new_handler_fun)  return p;
      else
      {
//       try
//       {
	    std::new_handler_fun();
//       }
//       catch (std::bad_alloc) { return p; }
      }
   }

   return p;
}
