/*****************************************************************************/
/* del_sofp  v2.54                                                           */
/* Copyright (c) 1996-2004 Texas Instruments Incorporated                    */
/*****************************************************************************/
#include <new>

/****************************************************************************/
/*                                                                          */
/* OPERATOR DELETE() - PLACEMENT VERSION OF THE DEFAULT GLOBAL DEALLOCATION */
/*                     FUNCTION FOR OBJECTS.                                */
/*                                                                          */
/****************************************************************************/
void operator delete  (void *ptr, void*) // throw()
{}
