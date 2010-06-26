/*****************************************************************************/
/* VTBL.H     v2.54                                                          */
/* Copyright (c) 1996-2004 Texas Instruments Incorporated                    */
/*****************************************************************************/

#include <rtti.h>

namespace std
{
   /***************************************************************************/
   /* VIRTUAL FUNCTION TABLE ENTRY                                            */
   /***************************************************************************/
   typedef struct vtbl_entry_first
   {
      short                  delta;   /* OFFSET TO GET TO THE COMPLETE OBJECT */
      struct type_info_impl *class_info; /* DYNAMIC TYPE OF OBJECT            */
   } VTBL_ENTRY_FIRST;

   typedef struct vtbl_entry_rest
   {
      short   delta;      /* OFFSET TO GET TO THE COMPLETE OBJECT   */
      void  (*func)();    /* VIRTUAL FUNCTION TO BE CALLED          */
   } VTBL_ENTRY_REST;

   typedef union vtbl_entry
   {
      VTBL_ENTRY_FIRST first;
      VTBL_ENTRY_REST rest;
   } VTBL_ENTRY;
}
