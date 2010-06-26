/*****************************************************************************/
/* RTTI.H     v2.54                                                          */
/* Copyright (c) 1996-2004 Texas Instruments Incorporated                    */
/*****************************************************************************/

#ifndef _RTTI
#define _RTTI

#ifndef  __embedded_cplusplus

#include <typeinfo>
#include <vtbl.h>

/******************************************************************************/
/* Definitions for the type_info_impl flags:                                  */
/*    TIF_LOCAL  - The type_info_impl variable was local                      */
/******************************************************************************/
#define TIF_LOCAL 0x01

/******************************************************************************/
/* Definitions for the base class specification flags:                        */
/*    BCS_NO_FLAGS   - No flags are set.                                      */
/*    BCS_VIRTUAL    - Base class is virtual.                                 */
/*    BCS_LAST       - This is the last base class in the array.              */
/*    BCS_PUBLIC     - Base class is public (Applies to non-direct bases).    */
/*    BCS_AMBIGUOUS  - Base class is ambiguous                                */
/*    BCS_DIRECT     - Base class is direct.                                  */
/******************************************************************************/
#define BCS_NO_FLAGS	0x00
#define BCS_VIRTUAL	0x01
#define BCS_LAST	0x02
#define BCS_PUBLIC      0x04
#define BCS_AMBIGUOUS	0x08
#define BCS_DIRECT	0x10

namespace std
{
   /***************************************************************************/
   /* Internal type information structure.                                    */
   /***************************************************************************/
   typedef struct type_info_impl
   {
      type_info               user_type_info;    /* User visible type_info    */
      char                   *name;              /* Name of the type          */
      unsigned int            flags;             /* Flags                     */
      const struct base_class_spec *bcs_entries; /* Array of base class specs */
   } TYPE_INFO_IMPL;

   /***************************************************************************/
   /* Base class specification, used in internal type information structure.  */
   /***************************************************************************/
   typedef struct base_class_spec
   {
      const TYPE_INFO_IMPL *type_info;  /* The type_info for the base class */
      short           offset;      /* Offset of base class in derived class */
      char            flags;       /* Base class specification flags        */
   } BASE_CLASS_SPEC;

   /***************************************************************************/
   /* PROTOTYPES FOR RTTI INTERNAL ROUTINES.                                  */
   /***************************************************************************/
   bool  matching_type_info (const TYPE_INFO_IMPL *cti1,
			     const TYPE_INFO_IMPL *cti2);
   void *dyn_cast           (void                 *objp,
			     const VTBL_ENTRY     *vtbl_ptr,
			     const TYPE_INFO_IMPL *class_info,
                             void                 *srcp,
                             const TYPE_INFO_IMPL *source_info);
   void *dyn_cast_ref       (void                 *objp,
			     const VTBL_ENTRY     *vtbl_ptr,
			     const TYPE_INFO_IMPL *class_info,
                             void                 *srcp,
                             const TYPE_INFO_IMPL *source_info);
   void *get_typeid         (const VTBL_ENTRY *vtbl_ptr);
   void  throw_bad_cast     (void);
   void  throw_bad_typeid   (void);
   void  dump_type_info     (const type_info& info);
}

#endif /* __embedded_cplusplus */
#endif /* _RTTI */
