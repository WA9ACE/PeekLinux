/*****************************************************************************/
/* del_sofn  v2.54                                                           */
/* Copyright (c) 1996-2004 Texas Instruments Incorporated                    */
/*****************************************************************************/
#include <new>

/****************************************************************************/
/*                                                                          */
/* OPERATOR DELETE() - NO EXCEPTION VERSION OF THE DEFAULT GLOBAL           */
/*                     DEALLOCATION FUNCTION FOR SINGLE OBJECTS.            */
/*                                                                          */
/****************************************************************************/
void  operator delete(void *ptr, const std::nothrow_t&) // throw()
{
   if (ptr)  std::free(ptr);
}
