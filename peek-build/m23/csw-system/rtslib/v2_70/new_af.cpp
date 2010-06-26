/*****************************************************************************/
/* new_af     v2.54                                                          */
/* Copyright (c) 1996-2004 Texas Instruments Incorporated                    */
/*****************************************************************************/
#include <new>

/****************************************************************************/
/*                                                                          */
/* OPERATOR NEW[]() - DEFAULT GLOBAL ALLOCATION FUNCTION FOR ARRAYS.        */
/*                                                                          */
/****************************************************************************/
void *operator new[](std::size_t size) // throw(std::bad_alloc)
{
   return operator new(size);
}
