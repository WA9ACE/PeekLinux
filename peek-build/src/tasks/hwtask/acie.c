#define ENTITY_MFW

#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"
#include "acie.h"
#include <string.h>

static T_HW_ACI_EXT aci_ext_handler [MAX_ACI_HANDLER];

void aci_ext_init()
{
  UBYTE i;

  emo_printf ("aci_ext_init()");

  for (i=0;i<MAX_ACI_HANDLER;i++)
  {
    aci_ext_handler[i].prim_handler = NULL;
    aci_ext_handler[i].cmd_handler  = NULL;
  }
}

GLOBAL int aci_create(T_PRIM_HANDLER prim_func, T_CMD_HANDLER cmd_func)
{
  UBYTE i;

  emo_printf ("aci_create()");


  /* changes to resolve the BUG OMAPS00064964, checking wether the 
   *  callbacks are registered twice.
  */
  for (i=0;i<MAX_ACI_HANDLER;i++)
  {

        if(aci_ext_handler[i].prim_handler EQ prim_func AND
            aci_ext_handler[i].cmd_handler EQ cmd_func)
       {
//							emo_printf("aci_create() duplicate found %d", i);
               return i;
       }

   }

   for (i=0;i<MAX_ACI_HANDLER;i++)
   {
         if (aci_ext_handler[i].prim_handler EQ NULL AND
             aci_ext_handler[i].cmd_handler EQ NULL)
        {
          /*
            * empty entry
           */
            aci_ext_handler[i].prim_handler = prim_func;
            aci_ext_handler[i].cmd_handler  = cmd_func;
//						emo_printf("aci_create() assigning %d for %x", i, prim_func);
            return i;
         }
     }
  return -1;
}


GLOBAL void aci_delete(int handle)
{
  emo_printf ("aci_delete ()");

  if (handle >= 0 AND
      handle <  MAX_ACI_HANDLER)
  {
    aci_ext_handler[handle].prim_handler = NULL;
    aci_ext_handler[handle].cmd_handler = NULL;
  }
}


GLOBAL BOOL aci_check_primitive(ULONG opc, void * data)
{
  UBYTE i;

  //emo_printf ("aci_check_primitive()");

  for (i=0;i<MAX_ACI_HANDLER;i++)
  {
//		emo_printf("aci_check_primitive() checking %i %x", i, aci_ext_handler[i].prim_handler);
    if (aci_ext_handler[i].prim_handler NEQ NULL)
    {
      /*
       * extension handler is installed; pass 32 bit opcode to primitive
       * handling function; note that the MFW functions are similar to pei_primitive
       * of aci_pei.c and as a loop is used and thus any primitive is parsed, the casting
       * to 32 bit opcodes needs to be performed inside those handling functions 
       * which handle still 16bit SAPs
       */
//			emo_printf("aci_check_primitive() calling %i %x", i, aci_ext_handler[i].prim_handler);
      if (aci_ext_handler[i].prim_handler(opc, data))
        /*
         * primitive is processed by the extension
         */
        return TRUE;
    }
  }
  /*
   * primitive is not processed by the extension
   */
  return FALSE;
}

GLOBAL BOOL aci_check_command(char * cmd)
{
  UBYTE i;

  emo_printf ("aci_check_command()");

  for (i=0;i<MAX_ACI_HANDLER;i++)
  {
    if (aci_ext_handler[i].cmd_handler NEQ NULL)
    {
      /*
       * extension handler is installed
       */
      if ((*aci_ext_handler[i].cmd_handler) (cmd))
        /*
         * command is processed by the extension
         */
        return TRUE;
    }
  }
  /*
   * command is not processed by the extension
   */
  return FALSE;
}

void mmeFlagHandler (void) { }

