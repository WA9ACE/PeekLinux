/****************************************************************************/
/*  sinit     v2.54                                                         */
/*  Copyright (c) 1996-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <stdlib.h>

extern void _DATA_ACCESS (*_dtors_ptr)(void);

/*************************************************************************/
/* LIST OF REQUIRED STATIC DESTRUCTIONS.  object POINTS TO THE OBJECT    */
/* TO BE DESTROYED, OR NULL IF THERE ARE SEVERAL.  dtor IS THE           */
/* DESTRUCTOR FOR THE OBJECT, AND dfun IS A FUNCTION THAT WILL CALL ALL  */
/* OF THE DESTRUCTORS IS THERE ARE SEVERAL.  ALWAYS ADD TO THE HEAD OF   */
/* THE LIST TO ENSURE THE PROPER ORDER OF DESTRUCTION.                   */
/*************************************************************************/
typedef struct dtor_list
{
   struct dtor_list *next;
   void             *object;
   union
   {
      void  (*dtor)(void *, int);
      void  (*dfun)(void);
   } fun;
} DTOR_LIST;

DTOR_LIST *dtors = NULL;
	 
/****************************************************************************/
/* CALL_DTORS() - PERFORM STATIC DESTRUCTION.                               */
/****************************************************************************/
void call_dtors(void)
{
   while (dtors)
   {
      /*-----------------------------------------------------------------*/
      /* SINCE A DESTRUCTOR MAY ADD NEW DESTRUCTORS TO THE LIST, NOTE    */
      /* AND REMOVE THE HEAD OF THE DESTRUCTOR LIST NOW.                 */
      /*-----------------------------------------------------------------*/
      DTOR_LIST *curr_dtor = dtors;
      dtors = dtors->next;

      /*-----------------------------------------------------------------*/
      /* IF A SINGLE OBJECT, CALL ITS DESTRUCTOR ON ITSELF (A FLAG VALUE */
      /* OF 2 INDICATES THAT OPERATOR DELETE SHOULD NOT BE CALLED).      */
      /* IF MULTIPLE OBJECTS, CALL THE SUPPLIED FUNCTION TO DESTROY THEM */
      /* ALL.                                                            */
      /*-----------------------------------------------------------------*/
      if (curr_dtor->object)  curr_dtor->fun.dtor(curr_dtor->object, 2);
      else                    curr_dtor->fun.dfun();
   }
}

/****************************************************************************/
/* ADD_DTOR() - ADD A DESTRUCTOR TO THE LIST OF STATIC DESTRUCTIONS.        */
/****************************************************************************/
void __add_dtor(DTOR_LIST *dtor_entry)
{
   /*-----------------------------------------------------------------------*/
   /* REGISTER call_dtors() SO exit() WILL CALL IT.  call_dtors() ISN'T     */
   /* CALLED DIRECTLY FROM exit() SO THAT IT ISN'T LINKED IN IF NOT USED.   */
   /*-----------------------------------------------------------------------*/
   _dtors_ptr = call_dtors;

   /*-----------------------------------------------------------------------*/
   /* ADD THE DESTRUCTOR TO THE LIST.                                       */
   /*-----------------------------------------------------------------------*/
   dtor_entry->next = dtors;
   dtors = dtor_entry;
}
