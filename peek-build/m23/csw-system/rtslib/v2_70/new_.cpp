/*****************************************************************************/
/* new     v2.54                                                             */
/* Copyright (c) 1996-2004 Texas Instruments Incorporated                    */
/*****************************************************************************/
#if 0

/*****************************************************************************/
/* The name of this file is new_.cpp to avoid an implicit rule problem with  */
/* gmake when new.cpp has a timestamp later than new.                        */
/*****************************************************************************/

#endif
#include <cstdlib>
#include <new>

namespace std
{
   const nothrow_t nothrow;
   void default_new_handler();

   new_handler new_handler_fun = default_new_handler;
}

/****************************************************************************/
/*                                                                          */
/* SET_NEW_HANDLER() - ESTABLISHES THE GIVEN FUNCTION AS THE CURRENT NEW    */
/*                     HANDLER.  RETURNS THE PREVIOUS ONE.                  */
/*                                                                          */
/****************************************************************************/
std::new_handler std::set_new_handler(std::new_handler new_p) // throw()
{
   std::new_handler old_p = std::new_handler_fun;

   std::new_handler_fun = new_p;
   return old_p;
}

/****************************************************************************/
/*                                                                          */
/* DEFAULT_NEW_HANDLER() - THIS IMPLEMENTATION'S DEFAULT NEW HANDLER.       */
/*                                                                          */
/****************************************************************************/
void std::default_new_handler()
{
// throw std::bad_alloc;
   std::abort();
}
