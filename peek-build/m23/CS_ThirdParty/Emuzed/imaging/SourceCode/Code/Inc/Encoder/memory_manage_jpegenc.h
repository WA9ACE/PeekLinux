/*
*******************************************************************************
                       Copyright eMuzed Inc., 2003-2004.
All rights Reserved, Licensed Software Confidential and Proprietary Information
    of eMuzed Incorporation Made available under Non-Disclosure Agreement OR
                            License as applicable.
*******************************************************************************
*/

/*
*******************************************************************************
Product     : JPEG encoder
Module      : Memory management
File        : memory_manage.h
Description : 

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author              Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Jan 06, 2005   Intial Code	G.Nageswara Rao	   MemoryManagement routines header
*******************************************************************************
*/
#ifndef JPEGENC_MEMORY
#define JPEGENC_MEMORY


void* EmzMemoryAlloc_JpegEnc(int32 size);
void EmzFreeMemory_JpegEnc(void* ptr);

#endif
