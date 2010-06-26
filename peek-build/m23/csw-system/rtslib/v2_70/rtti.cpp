/*****************************************************************************/
/* RTTI     v2.54                                                            */
/* Copyright (c) 1996-2004 Texas Instruments Incorporated                    */
/*****************************************************************************/
#ifndef  __embedded_cplusplus

#include <cstdlib>
#include "rtti.h"


namespace {
/******************************************************************************/
/* D_TO_B_CONVERSION - Performs derived to base class conversions.            */
/*                                                                            */
/*  Converts objp from a pointer to a derived class (described by class_info) */
/*  to a pointer to a base class (described by base_info) and stores          */
/*  the resulting pointer in new_objp.  Returns TRUE if the base class was    */
/*  found and the conversion was done; otherwise returns FALSE.               */
/*                                                                            */
/*  objp may be NULL when this routine is called simply to determine whether  */
/*  the conversion is possible.  This is the case when exception              */
/*  specifications are being tested.                                          */
/*                                                                            */
/******************************************************************************/
int d_to_b_conversion(void**                     objp,
		      void**                     new_objp,
		      const std::TYPE_INFO_IMPL *class_info,
		      const std::TYPE_INFO_IMPL *base_info)
{
  bool                       found         = false;
  bool                       is_ambiguous  = false;
  bool                       is_accessible = false;
  bool                       last          = false;
  void                       *ptr           = objp ? *objp : NULL;
  const std::BASE_CLASS_SPEC *bcsp;

  /*--------------------------------------------------------------------------*/
  /* NO BASE CLASSES, THEN NO POSSIBILITY FOR CONVERSIONS.                    */
  /*--------------------------------------------------------------------------*/
  if (!class_info->bcs_entries)  return false;

  /*--------------------------------------------------------------------------*/
  /* LOOK THROUGH THE BASE CLASS LIST FOR A MATCH.  SEARCH FOR DIRECT BASES   */
  /* FIRST, SO THAT TOP LEVEL VIRTUAL BASE CLASSES ARE FOUND FIRST.           */
  /*--------------------------------------------------------------------------*/
  for (bcsp = class_info->bcs_entries, last = false;
       !last && !found && !is_ambiguous;
       last = bcsp->flags & BCS_LAST, bcsp++)
  {
     /*----------------------------------------------------------------------*/
     /* CHECK FOR A MATCH WITH THE DESIRED BASE CLASS.                       */
     /*----------------------------------------------------------------------*/
     if (std::matching_type_info(bcsp->type_info, base_info)) 
     {
	is_ambiguous  = bcsp->flags & BCS_AMBIGUOUS;
	is_accessible = bcsp->flags & BCS_PUBLIC;

	if (!is_ambiguous && is_accessible) 
	{
	   found = true;

	   if (ptr)
	   {
	      /*-------------------------------------------------------------*/
	      /* IF A VIRTUAL BASE CLASS, THE OFFSET PROVIDES THE LOCATION   */
	      /* OF A POINTER TO THE BASE CLASS.                             */
	      /*-------------------------------------------------------------*/
	      if (bcsp->flags & BCS_VIRTUAL)
		 *new_objp = *((void **)((char *)ptr + bcsp->offset));
	       
	      /*-------------------------------------------------------------*/
	      /* IF A NON-VIRTUAL BASE CLASS, THE OFFSET POINTS TO THE BASE  */
	      /* SUB-OBJECT.                                                 */
	      /*-------------------------------------------------------------*/
	      else
		 *new_objp = (void *)((char *)ptr + bcsp->offset);
	   }
	}
     }
  }

  /*--------------------------------------------------------------------------*/
  /* IF THE BASE CLASS WAS FOUND OR DETERMINED TO BE AMBIGUOUS, THEN RETURN   */
  /* THAT RESULT.                                                             */
  /*--------------------------------------------------------------------------*/
  if (is_ambiguous || found)  return found;

  /*--------------------------------------------------------------------------*/
  /* ELSE LOOK THROUGH INDIRECT BASE CLASSES FOR A MATCH.                     */
  /*--------------------------------------------------------------------------*/
  for (bcsp = class_info->bcs_entries, last = false;
       !last && !found;
       last = bcsp->flags & BCS_LAST, bcsp++)
  {
     void *b_objp     = ptr ? (void *)((char *)ptr + bcsp->offset) : NULL;
     void *new_b_objp = NULL;

     /*----------------------------------------------------------------------*/
     /* ENSURE THAT THE BASE CLASS HAS BASES AND THAT THE BASE CLASS IS      */
     /* ACCESSIBLE AND UNAMBIGUOUS.                                          */
     /*----------------------------------------------------------------------*/
     if (!bcsp->type_info->bcs_entries || (bcsp->flags & BCS_AMBIGUOUS) ||
	 !(bcsp->flags & BCS_PUBLIC))
	continue;

     /*---------------------------------------------------------------------*/
     /* NOW RECURSIVELY LOOK THROUGH THE BASE CLASSES OF THE BASE.          */
     /*---------------------------------------------------------------------*/
     if (d_to_b_conversion(&b_objp, &new_b_objp, bcsp->type_info, base_info))
     {	found = true;
	if (ptr)  *new_objp = new_b_objp;
     }
  }
     
  return found;
}

/******************************************************************************/
/* FIND_BASE_CLASS_AT_ADDR - Find base class for a given class pointer.       */
/*                                                                            */
/*  Find the base class specification entry that corresponds to the base      */
/*  class pointed to by "base_ptr", whose type is specified by "base_info"    */
/*  in the object pointed to by "obj_ptr".  The base class must be accessible,*/
/*  but need not be unambiguous.                                              */
/******************************************************************************/

static const std::BASE_CLASS_SPEC *find_base_class_at_addr(void  *obj_ptr,
                                       void                      *base_ptr,
                                       const std::TYPE_INFO_IMPL *obj_info,
                                       const std::TYPE_INFO_IMPL *base_info)
{
   const std::BASE_CLASS_SPEC *bcsp;
   void                 *new_ptr;
   bool                 done = false;

   for (bcsp = obj_info->bcs_entries; bcsp != NULL && !done;
                                done = (bcsp->flags & BCS_LAST) != 0, bcsp++)
   {
   /*-------------------------------------------------------------------------*/
   /* ADJUST THE POINTER BY THE OFFSET PROVIDED IN BASE CLASS SPECIFICATION   */
   /*-------------------------------------------------------------------------*/
       new_ptr = (void*) (((char *) obj_ptr) + bcsp->offset);

   /*-------------------------------------------------------------------------*/
   /* FOR A VIRTUAL BASE CLASS THE OFFSET PROVIDES THE LOCATION OF A POINTER  */
   /* TO THE BASE CLASS.  DEREFERENCE THE POINTER AND USE THAT VALUE.         */
   /*-------------------------------------------------------------------------*/
       if (bcsp->flags & BCS_VIRTUAL)
          new_ptr = *((void **)new_ptr);

   /*-------------------------------------------------------------------------*/
   /* IF BOTH THE ADDRESS AND TYPE MATCHES THEN WE FOUND A MATCH.             */
   /*-------------------------------------------------------------------------*/
       if (new_ptr == base_ptr &&
                                matching_type_info(bcsp->type_info, base_info))
          return bcsp;

   /*-------------------------------------------------------------------------*/
   /* NO MATCH, CHECK THE BASE CLASSES OF THIS BASE CLASS.                    */
   /*-------------------------------------------------------------------------*/

       if ((bcsp->flags & BCS_PUBLIC) != 0)
       {
           const std::BASE_CLASS_SPEC *result;
           result = find_base_class_at_addr(new_ptr, base_ptr,
                                       bcsp->type_info, base_info);
           if (result) return result;
       }
   }
   return NULL;
}

} // end namespace

/******************************************************************************/
/* DYN_CAST - Perform the dynamic_cast<> operation.                           */
/*                                                                            */
/*  This routine handles:                                                     */
/*    - casts of a polymorphic objects type to void*, which is defined as     */
/*      returning a pointer to the complete object type                       */
/*    - polymorphic base to derived casts                                     */
/*    - polymorphic cross casts                                               */
/*                                                                            */
/*  objp is the source operand of the cast.  If the source is                 */
/*  an object (and not a pointer) then a pointer to the source operand        */
/*  is used.  vtbl_ptr is a pointer to the virtual function table from        */
/*  the source operand.  class_info is a pointer to the TYPE_INFO_IMPL        */
/*  structure associated with the destination type.  If the source            */
/*  operand is being cast to void*, class_info will be NULL.                  */
/*                                                                            */
/*  srcp is the original pointer being cast.  It is different from objp if    */
/*  the original type did not have a virtual function table associated with   */
/*  it.  source_info is the type info pointer for the static type of the      */
/*  pointer being cast.  These parameters are used to check the access of the */
/*  base class associated with the pointer being cast.                        */
/*                                                                            */
/*  The information about the dynamic type of the source object is obtained   */
/*  from entry zero of the virtual function table.                            */
/*  Entry zero of the virtual function table is organized differently than the*/
/*  other table entries.  The information in that entry is provided to support*/
/*  the dynamic_cast and typeinfo operations.  Entry zero contains the        */
/*  following information:                                                    */
/*                                                                            */
/*  	delta:		The offset from the complete object pointer to        */
/*  			objp (i.e., the value to be subtracted from           */
/*  			objp to get the complete object pointer.              */
/*  	function:	Pointer to the typeinfo_impl structure that           */
/*  		        for the dynamic type of objp.                         */
/*                                                                            */
/******************************************************************************/
void *std::dyn_cast(void                 *objp,
		    const VTBL_ENTRY     *vtbl_ptr,
		    const TYPE_INFO_IMPL *class_info,
                    void                 *srcp,
                    const TYPE_INFO_IMPL *source_info)
{
   void                  *complete_objp;
   const TYPE_INFO_IMPL  *complete_class_info;

   /*-------------------------------------------------------------------------*/
   /* GET THE COMPLETE OBJECT.                                                */
   /*-------------------------------------------------------------------------*/
   complete_objp = (void *)((char *)objp - vtbl_ptr->first.delta);

   /*-------------------------------------------------------------------------*/
   /* GET THE TYPE OF THE COMPLETE OBJECT.                                    */
   /*-------------------------------------------------------------------------*/
   complete_class_info = (const TYPE_INFO_IMPL *)vtbl_ptr->first.class_info;

   /*-------------------------------------------------------------------------*/
   /* DO SOMETHING PREDICTABLE IF PARTS OF THE APPLICATION WERE COMPILED      */
   /* WITHOUT RTTI ENABLED.                                                   */
   /*-------------------------------------------------------------------------*/
   if (!complete_class_info)  throw_bad_cast();

   /*-------------------------------------------------------------------------*/
   /* IF class_info IS NULL, THE OBJECT POINTER IS BEING CAST TO VOID *, SO   */
   /* RETURN A POINTER TO THE COMPLETE OBJECT.                                */
   /*-------------------------------------------------------------------------*/
   if (!class_info)  return complete_objp;

   else 
   {
      bool      access_okay = true;
   /*-------------------------------------------------------------------------*/
   /* MAKE SURE THE BASE CLASS POINTED TO BY THE srcp IS AN ACCESSIBLE BASE   */
   /* CLASS. find_base_class_at_addr WILL RETURN NULL IF THE BASE CLASS IS AN */
   /* INDIRECT BASE CLASS OF A PRIVATE BASE CLASS.  IT WILL RETURN A POINTER  */
   /* TO THE BASE CLASS ENTRY IF IT IS A BASE CLASS OF AN ACCESSIBLE          */
   /* BASE CLASS (OR IF IT IS A DIRECT BASE CLASS).  CHECK THE RETURNED BASE  */
   /* CLASS ENTRY TO MAKE SURE THE BASE CLASS IS PUBLIC.  THE CONVERSIONS     */
   /* THAT FOLLOW ARE DONE ONLY IF THE SOURCE CLASS IS A PUBLIC BASE CLASS    */
   /*-------------------------------------------------------------------------*/
      if (class_info == source_info)
         access_okay = true;
      else
      {
         const std::BASE_CLASS_SPEC *bcsp;
         bcsp = find_base_class_at_addr (complete_objp, srcp,
                                          complete_class_info, source_info);
         access_okay = (bcsp != NULL) && ((bcsp->flags & BCS_PUBLIC) != 0);
      }

      if (access_okay)
      {
   /*-------------------------------------------------------------------------*/
   /* ELSE IF THE OBJECT POINTER IS BEING CAST TO THE TYPE OF THE COMPLETE    */
   /* OBJECT, RETURN A POINTER TO THE COMPLETE OBJECT.                        */
   /*-------------------------------------------------------------------------*/
         if (matching_type_info(complete_class_info, class_info))
            return complete_objp;

   /*-------------------------------------------------------------------------*/
   /* ELSE THE OBJECT POINTER IS BEING CAST TO THE TYPE OF A SUB-OBJECT OF    */
   /* THE OBJECT'S DYNAMIC TYPE.  IF THE TYPE IS ACCESSIBLE AND UNAMBIGUOUS,  */
   /* THEN RETURN A POINTER TO THE SUB-OBJECT OF THAT TYPE.                   */
   /*-------------------------------------------------------------------------*/
         else
         {
            void *new_objp = NULL;
            int   found    = d_to_b_conversion(&complete_objp, &new_objp,
                                         complete_class_info, class_info);

            return found ? new_objp : NULL;
         }
      }
   }
   return NULL;
}

/******************************************************************************/
/* DYN_CAST_REF - Interface to dyn_cast() for reference type casts.           */
/*                                                                            */
/*  An exception is thrown if the cast fails.                                 */
/*                                                                            */
/******************************************************************************/
void *std::dyn_cast_ref(void                 *objp,
			const VTBL_ENTRY     *vtbl_ptr,
			const TYPE_INFO_IMPL *class_info,
                        void                 *srcp,
                        const TYPE_INFO_IMPL *source_info)
{
   void *result = dyn_cast(objp, vtbl_ptr, class_info, srcp, source_info);

   if (!result) throw_bad_cast();

   return result;
}

/******************************************************************************/
/* GET_TYPEID - Returns the type_info object for a given polymorphic type.    */
/*                                                                            */
/******************************************************************************/
void *std::get_typeid(const VTBL_ENTRY *vtbl_ptr)
{
   const TYPE_INFO_IMPL *class_info;

   if (!vtbl_ptr)  throw_bad_typeid();

   class_info = (const TYPE_INFO_IMPL *)vtbl_ptr->first.class_info;

   /*-------------------------------------------------------------------------*/
   /* DO SOMETHING PREDICTABLE IF PARTS OF THE APPLICATION WERE COMPILED      */
   /* WITHOUT RTTI ENABLED.                                                   */
   /*-------------------------------------------------------------------------*/
   if (!class_info)  throw_bad_typeid();

   return (void*)&class_info->user_type_info;
}

/******************************************************************************/
/* THROW_BAD_CAST - Throws a bad cast exception.                              */
/******************************************************************************/
void std::throw_bad_cast(void)
{
   // throw bad_cast();
   abort();  // For now, just call abort
}

/******************************************************************************/
/* THROW_BAD_TYPEID - Throws a bad typeid exception.                          */
/******************************************************************************/
void std::throw_bad_typeid(void)
{
   // throw bad_typeid();
   abort();  // For now, just call abort
}

#include <cstdio>
/******************************************************************************/
/* DUMP_TYPE_INFO - Dumps debug info about the given type representation.     */
/******************************************************************************/
void std::dump_type_info(const type_info& info)
{
   /*-------------------------------------------------------------------------*/
   /* GET THE INTERNAL REPRESENTATION OF THE TYPE.  WE DEPEND UPON THE FACT   */
   /* THAT THE USER VISIBLE TYPE_INFO IS THE FIRST MEMBER OF THIS STRUCTURE.  */
   /*-------------------------------------------------------------------------*/
   const TYPE_INFO_IMPL *class_info = (const TYPE_INFO_IMPL *)&info;

   /*-------------------------------------------------------------------------*/
   /* NOW OUTPUT LOTS OF USEFUL INFORMATION.  START WITH THE TYPE ITSELF.     */
   /*-------------------------------------------------------------------------*/
   fprintf(stderr, "\n*********************************************\n");
   fprintf(stderr, "Type information for: %s\n",
	   class_info->name ? class_info->name : "<NULL>");
   fprintf(stderr, "  flags       :");
   if (class_info->flags & TIF_LOCAL)  fprintf(stderr, " local");
   fprintf(stderr, "\n");

   /*-------------------------------------------------------------------------*/
   /* AND THEN INFO ABOUT ITS BASE CLASSES, IF THERE ARE ANY.                 */
   /*-------------------------------------------------------------------------*/
   if (class_info->bcs_entries)
   {
      const BASE_CLASS_SPEC *bcsp;
      bool                   last;

      /*----------------------------------------------------------------------*/
      /* OUTPUT INFO ABOUT THE TYPE'S INHERITANCE.                            */
      /*----------------------------------------------------------------------*/
      fprintf(stderr, "  base classes:\n");

      for (bcsp = class_info->bcs_entries, last = false;
       !last;
       last = bcsp->flags & BCS_LAST, bcsp++)
      {
	 char	*name = bcsp->type_info->name;

	 fprintf(stderr, "    name=%s\n", name ? name : "<NULL>");
	 fprintf(stderr, "    offset=%0ld\n", (long)bcsp->offset);
	 fprintf(stderr, "    flags:");
	 if (bcsp->flags & BCS_VIRTUAL)   fprintf(stderr, " virtual");
	 if (bcsp->flags & BCS_LAST)      fprintf(stderr, " last");
	 if (bcsp->flags & BCS_PUBLIC)    fprintf(stderr, " public");
	 if (bcsp->flags & BCS_AMBIGUOUS) fprintf(stderr, " ambiguous");
	 if (bcsp->flags & BCS_DIRECT)    fprintf(stderr, " direct");
	 fprintf(stderr, "\n");
      }

      /*----------------------------------------------------------------------*/
      /* OUTPUT INFO ABOUT THE TYPE'S DIRECT BASE CLASSES.                    */
      /*----------------------------------------------------------------------*/
      for (bcsp = class_info->bcs_entries, last = false;
       !last;
       last = bcsp->flags & BCS_LAST, bcsp++)
	 if (bcsp->flags & BCS_DIRECT) 
	    dump_type_info(bcsp->type_info->user_type_info);
   }

   fprintf(stderr, "*********************************************\n");
}

#endif
