/*****************************************************************************/
/* TYPEINFO     v2.54                                                        */
/* Copyright (c) 1996-2004 Texas Instruments Incorporated                    */
/*****************************************************************************/
#if 0

/*****************************************************************************/
/* The name of this file is typeinfo_.cpp to avoid an implicit rule problem  */
/* with gmake when typeinfo.cpp has a timestamp later than typeinfo.         */
/*****************************************************************************/

#endif
#ifndef __embedded_cplusplus

#include <cstring>
#include <typeinfo>
#include "rtti.h"

/*****************************************************************************/
/* ~TYPE_INFO - Virtual destructor.  Should never be called directly.        */
/*****************************************************************************/
std::type_info::~type_info()  { }

/******************************************************************************/
/* OPERATOR== - Returns TRUE if the type represented by the given object      */
/*              represents the same type this object represents.              */
/******************************************************************************/
bool std::type_info::operator==(const type_info& rhs) const
{
   const TYPE_INFO_IMPL *class_info1 = (const TYPE_INFO_IMPL *)this;
   const TYPE_INFO_IMPL *class_info2 = (const TYPE_INFO_IMPL *)&rhs;

   return matching_type_info(class_info1, class_info2);
}

/******************************************************************************/
/* OPERATOR!= - Returns TRUE if the type represented by the given object      */
/*              represents a different type than this object represents.      */
/******************************************************************************/
bool std::type_info::operator!=(const type_info& rhs) const
{
   const TYPE_INFO_IMPL *class_info1 = (const TYPE_INFO_IMPL *)this;
   const TYPE_INFO_IMPL *class_info2 = (const TYPE_INFO_IMPL *)&rhs;

   return !matching_type_info(class_info1, class_info2);
}

/******************************************************************************/
/* BEFORE - Return TRUE if the type this object represents precedes the type  */
/*          represented by the given object, according to a collating         */
/*          sequence.                                                         */
/*                                                                            */
/*  This is implemented by separating types into those that are local types   */
/*  and external. In the first case, the addresses of the type_info structures*/
/*  are always unique.  In the latter, there may be more than 1 copy of the   */
/*  type_info structure for a single type.  Types in the first category are   */
/*  always greater than those in the second.  Within the first category, a    */
/*  collating sequence is determined by comparing the addresses of the        */
/*  type's type_info structure.  Within the second, a collating sequence is   */
/*  based upon the lexical ordering of the type's name.                       */
/******************************************************************************/
bool std::type_info::before(const type_info& rhs) const
{
   const TYPE_INFO_IMPL *class_info1 = (const TYPE_INFO_IMPL *)this;
   const TYPE_INFO_IMPL *class_info2 = (const TYPE_INFO_IMPL *)&rhs;

   if (class_info1->flags & TIF_LOCAL)
      if (class_info2->flags & TIF_LOCAL)
         return class_info1 < class_info2 ? true : false;
      else
         return false;
   else
      if (class_info2->flags & TIF_LOCAL)
         return true;
      else
         return strcmp(class_info1->name, class_info2->name) < 0 ? true : false;
}

/*****************************************************************************/
/* NAME -  Returns the source level name of the type the object represents.  */
/*****************************************************************************/
const char *std::type_info::name() const
{
   const TYPE_INFO_IMPL *class_info = (const TYPE_INFO_IMPL *)this;

   return class_info->name;
}

/*****************************************************************************/
/* MATCHING_TYPE_INFO - Determines if two type entries refer to same type.   */
/*****************************************************************************/
bool std::matching_type_info(const TYPE_INFO_IMPL *cti1, 
			     const TYPE_INFO_IMPL *cti2)
{
   return (cti1 == cti2 || 
	   (!(cti1->flags & TIF_LOCAL) &&
	    !(cti2->flags & TIF_LOCAL) &&
	    !strcmp(cti1->name, cti2->name)));
}

#endif /* __embedded_cplusplus */
