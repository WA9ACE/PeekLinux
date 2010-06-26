/*****************************************************************************/
/* new_afp   v2.54                                                           */
/* Copyright (c) 1996-2004 Texas Instruments Incorporated                    */
/*****************************************************************************/
#include <new>

/****************************************************************************/
/*                                                                          */
/* OPERATOR NEW[]() - PLACEMENT VERSION OF THE DEFAULT GLOBAL ALLOCATION    */
/*                    FUNCTION FOR ARRAYS.                                  */
/*                                                                          */
/****************************************************************************/
void *operator new[](std::size_t, void *ptr) // throw()
{
   return ptr;
}
