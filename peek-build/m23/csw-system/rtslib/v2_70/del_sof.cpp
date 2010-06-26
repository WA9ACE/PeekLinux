/*****************************************************************************/
/* del_sof  v2.54                                                            */
/* Copyright (c) 1996-2004 Texas Instruments Incorporated                    */
/*****************************************************************************/
#include <new>

/****************************************************************************/
/*                                                                          */
/* OPERATOR DELETE() - DEFAULT GLOBAL DEALLOCATION FUNCTION FOR SINGLE      */
/*                     OBJECTS.                                             */
/*                                                                          */
/****************************************************************************/
void  operator delete(void *ptr) // throw()
{
   if (ptr)  std::free(ptr);
}
