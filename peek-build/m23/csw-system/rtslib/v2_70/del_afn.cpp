/*****************************************************************************/
/* del_afn     v2.54                                                         */
/* Copyright (c) 1996-2004 Texas Instruments Incorporated                    */
/*****************************************************************************/
#include <new>

/****************************************************************************/
/*                                                                          */
/* OPERATOR DELETE[]() - NO EXCEPTION VERSION OF THE DEFAULT GLOBAL         */
/*                       DEALLOCATION FUNCTION FOR ARRAYS.                  */
/*                                                                          */
/****************************************************************************/
void  operator delete[](void *ptr, const std::nothrow_t& nt_arg) // throw()
{
   operator delete(ptr, nt_arg);
}
