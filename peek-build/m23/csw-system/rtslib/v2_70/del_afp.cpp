/*****************************************************************************/
/* del_af     v2.54                                                          */
/* Copyright (c) 1996-2004 Texas Instruments Incorporated                    */
/*****************************************************************************/
#include <new>

/****************************************************************************/
/*                                                                          */
/* OPERATOR DELETE[]() - PLACEMENT VERSION OF THE DEFAULT GLOBAL            */
/*                       DEALLOCATION FUNCTION FOR ARRAYS.                  */
/*                                                                          */
/****************************************************************************/
void operator delete[](void *ptr, void*) // throw()
{}
