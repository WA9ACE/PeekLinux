/*****************************************************************************/
/* new_afn     v2.54                                                         */
/* Copyright (c) 1996-2004 Texas Instruments Incorporated                    */
/*****************************************************************************/
#include <new>

/****************************************************************************/
/*                                                                          */
/* OPERATOR NEW[]() - NO EXCEPTION VERSION OF THE DEFAULT GLOBAL ALLOCATION */
/*                    FUNCTION FOR ARRAYS.                                  */
/*                                                                          */
/****************************************************************************/
void *operator new[](std::size_t size, const std::nothrow_t& nt_arg) //throw()
{
   return operator new(size, nt_arg);
}
