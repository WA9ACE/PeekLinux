/****************************************************************************/
/*  array     v2.54                                                         */
/*  Copyright (c) 1996-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
/* NOTE THAT THIS IS A TEMPORARY IMPLEMENTATION.  THIS WILL BE MERGED INTO  */
/* MEMORY.C AT A LATER TIME.                                                */
/****************************************************************************/
#include <cstdlib>

typedef void  (*CTOR)  (void *);
typedef void  (*CCTOR) (void *, void *);
typedef void  (*DTOR)  (void *, int);
typedef void *(MYNEW)  (std::size_t);
typedef void  (*MYDEL) (void *);
typedef void  (*MYDEL2)(void *, std::size_t);

extern "C" 
{
   void *__canew   (std::size_t nelem, std::size_t selem, CTOR ctor, 
		    DTOR dtor, MYNEW mynew, MYDEL mydel);
   void *__pcanew  (void *array, std::size_t nelem, std::size_t selem, 
                    CTOR ctor, DTOR dtor);
   void *__anew    (void *array, std::size_t nelem, std::size_t selem, 
		    CTOR ctor);
   void  __acctor  (void *dst_array, std::size_t nelem, std::size_t selem,
                    CCTOR cctor, void *src_array);
   void  __cadelete(void *array, std::size_t nelem, std::size_t selem, 
		    DTOR dtor, MYDEL mydel, bool is_2_arg);
   void  __adelete (void *array, std::size_t nelem, std::size_t selem, 
		    DTOR dtor, int dealloc, int);
}

#define LARGE_TYPE_MASK (sizeof(long double) - 1)

namespace
{
   struct array_prefix
   {
      std::size_t nelem;
   };

   extern "C" std::size_t __array_new_prefix_size = 
                    (sizeof(array_prefix) + LARGE_TYPE_MASK) & ~LARGE_TYPE_MASK;

   /*************************************************************************/
   /* ALLOC_ARRAY() - ALLOCATE MEMORY FOR AN ARRAY, INCLUDING ITS HEADER.   */
   /*************************************************************************/
   void *alloc_array(std::size_t nelem, std::size_t selem, MYNEW mynew)
   {
      void *array;

      std::size_t size     = nelem*selem + __array_new_prefix_size;

      if (mynew)  array = mynew(size);
      else        array = operator new[](size);

      if (array)
      {
	 ((array_prefix *)array)->nelem = nelem;
	 array = (char *)array + __array_new_prefix_size;
      }

      return array;
   }

   /*************************************************************************/
   /* DEALLOC_ARRAY() - DEALLOCATE MEMORY FOR AN ARRAY, INCLUDING ITS HEADER*/
   /*************************************************************************/
   void dealloc_array(void *array, std::size_t nelem, std::size_t selem, 
		      MYDEL mydel, bool is_2_arg)
   {
      std::size_t size = nelem * selem + __array_new_prefix_size;
      array = (char *)array - __array_new_prefix_size;

      if      (!mydel)    operator delete[](array);
      else if (is_2_arg)  ((MYDEL2)mydel)(array, size);
      else                mydel(array);
   }

   /*************************************************************************/
   /* ARRAY_NEW() - ALLOCATES AND INITIALIZES ARRAYS.                       */
   /*************************************************************************/
   void *array_new(void *array, std::size_t nelem, std::size_t selem, 
		   CTOR ctor, MYNEW mynew)
   {
      /*--------------------------------------------------------------------*/
      /* IF SPACE FOR THE ARRAY ISN'T ALLOCATED YET, ALLOCATE IT            */
      /*--------------------------------------------------------------------*/
      if (!array && !(array = alloc_array(nelem, selem, mynew)))  return NULL;
      
      /*--------------------------------------------------------------------*/
      /* CALL THE CONTRUCTOR FOR EACH ELEMENT OF THE ARRAY.                 */
      /*--------------------------------------------------------------------*/
      if (ctor)
      {
	 char *p = (char *)array;
	 for (int i = 0; i < nelem; i++, p += selem)  ctor(p);
      }

      return array;
   }

   /*************************************************************************/
   /* ARRAY_DEL() - DEALLOCATES AN DE-INITIALIZES ARRAYS.                   */
   /*************************************************************************/
   void array_del(void *array, std::size_t nelem, std::size_t selem, 
		  DTOR dtor, bool dealloc, MYDEL mydel, bool is_2_arg)
   {
      if (!array) return;

      /*--------------------------------------------------------------------*/
      /* FOR VARIABLE LENGTH ARRAYS, THE NUMBER OF ELEMENTS IS -1.          */
      /* SO LOOK IT UP IN THE PREFIX.                                       */
      /*--------------------------------------------------------------------*/
      if (nelem == (std::size_t)-1)  
      {
	 nelem = 
	      ((array_prefix *)((char *)array-__array_new_prefix_size))->nelem;
      }

      /*--------------------------------------------------------------------*/
      /* CALL THE DESTRUCTOR FOR EACH ELEMENT OF THE ARRAY.                 */
      /*--------------------------------------------------------------------*/
      if (dtor)
      {
	 char *p = (char *)array + ((nelem - 1) * selem);
	 for (int i = 0; i < nelem; i++, p -= selem) 
	    dtor(p, 2); // 2 INDICATES DESTRUCT WHOLE OBJECT, BUT DO NOT DELETE
      }

      /*--------------------------------------------------------------------*/
      /* DEALLOCATE THE SPACE USED BY THE ARRAY, IF REQUESTED.              */
      /*--------------------------------------------------------------------*/
      if (dealloc)  dealloc_array(array, nelem, selem, mydel, is_2_arg);
   }
}

/****************************************************************************/
/* __CANEW() - RTS ROUTINE FOR CLASS ARRAY NEW.                             */
/****************************************************************************/
void *__canew(std::size_t nelem, std::size_t selem, CTOR ctor, DTOR dtor,
	      MYNEW mynew, MYDEL mydel)
{
   return array_new(NULL, nelem, selem, ctor, mynew);
}

/****************************************************************************/
/* __PCANEW() - RTS ROUTINE FOR PLACEMENT CLASS ARRAY NEW.                  */
/****************************************************************************/
void *__pcanew(void *array, std::size_t nelem, std::size_t selem, 
	       CTOR ctor, DTOR dtor)
{
   return array_new(array, nelem, selem, ctor, NULL);
}

/****************************************************************************/
/* __ANEW() - RTS ROUTINE FOR ARRAY NEW.                                    */
/****************************************************************************/
void *__anew(void *array, std::size_t nelem, std::size_t selem, CTOR ctor)
{
   return array_new(array, nelem, selem, ctor, NULL);
}

/****************************************************************************/
/* __ACCTOR() - ARRAY COPY CONSTRUCTOR.  CALLS ctor() COPY CONSTRUCTOR ON   */
/*              EACH ELEMENT OF dst, WITH THE CORRESPONDING ELEMENT IN src  */
/*              BEING THE ARGUMENT TO THE CONSTRUCTOR.  dst AND src ARE     */
/*              ARRAYS OF nelem ELEMENTS OF SIZE selem.                     */
/****************************************************************************/
void __acctor(void *dst_array, std::size_t nelem, std::size_t selem,
	      CCTOR cctor, void *src_array)
{
   char *dst = (char *)dst_array;
   char *src = (char *)src_array;

   if (cctor)
      for (int i = 0; i < nelem; i++, dst += selem, src += selem)
	 cctor((void *)dst, (void *)src);
}

/****************************************************************************/
/* __CADELETE() - RTS ROUTINE FOR CLASS ARRAY DELETE.                       */
/****************************************************************************/
void __cadelete(void *array, std::size_t nelem, std::size_t selem, DTOR dtor,
		MYDEL mydel, bool is_2_arg)
{
   array_del(array, nelem, selem, dtor, true, mydel, is_2_arg);
}

/****************************************************************************/
/* __ADELETE() - RTS ROUTINE FOR ARRAY DELETE.                              */
/****************************************************************************/
void __adelete(void *array, std::size_t nelem, std::size_t selem, DTOR dtor,
	       int dealloc, int)
{
   array_del(array, nelem, selem, dtor, dealloc, NULL, false);
}

