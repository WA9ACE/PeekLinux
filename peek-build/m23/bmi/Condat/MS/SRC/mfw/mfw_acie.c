/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_acie.c      $|
| $Author:: Es                          $Revision::  1              $|
| CREATED: 26.10.98                     $Modtime:: 2.03.00 15:25    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_ACIE

   PURPOSE : This modul contains ACI external functions.


   $History:: mfw_acie.c                                              $
 * 
 * *****************  Version 8  *****************
 * User: Es           Date: 2.03.00    Time: 16:04
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * some cleanups
 * 
 * *****************  Version 7  *****************
 * User: Vo           Date: 12.02.99   Time: 18:59
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 6  *****************
 * User: Es           Date: 8.12.98    Time: 16:53
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 5  *****************
 * User: Vo           Date: 24.11.98   Time: 21:40
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * first successful compilation of MFW
|
| *****************  Version 4  *****************
| User: Le           Date: 11.11.98   Time: 13:35
| Updated in $/GSM/DEV/MS/SRC/MFW
|
| *****************  Version 3  *****************
| User: Le           Date: 10.11.98   Time: 16:07
| Updated in $/GSM/DEV/MS/SRC/MFW
|
| *****************  Version 2  *****************
| User: Le           Date: 10.11.98   Time: 11:08
| Updated in $/GSM/DEV/MS/SRC/MFW
|
| *****************  Version 1  *****************
| User: Le           Date: 27.10.98   Time: 15:58
| Created in $/GSM/DEV/MS/SRC/MFW
| first implementation

*/
#define ENTITY_MFW

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif
#ifdef BT_INTERFACE
#include "Bti.h"
#include "mfw_mfw.h"
#include "mfw_bt.h"
#endif
#include "mfw_acie.h"
#include <string.h>

static T_MFW_ACI_EXT aci_ext_handler [MAX_ACI_HANDLER];
#ifdef BT_INTERFACE
int pSlotBt = 0;
#endif
/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_ACIE            |
| STATE  : code                         ROUTINE: aci_ext_init        |
+--------------------------------------------------------------------+

   PURPOSE :

*/

void aci_ext_init()
{
  UBYTE i;

  TRACE_FUNCTION ("aci_ext_init()");

  for (i=0;i<MAX_ACI_HANDLER;i++)
  {
    aci_ext_handler[i].prim_handler = NULL;
    aci_ext_handler[i].cmd_handler  = NULL;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_ACIE            |
| STATE  : code                         ROUTINE: aci_create          |
+--------------------------------------------------------------------+

   PURPOSE :

*/

GLOBAL int aci_create(T_PRIM_HANDLER prim_func, T_CMD_HANDLER cmd_func)
{
  UBYTE i;

  TRACE_FUNCTION ("aci_create()");


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


/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_ACIE            |
| STATE  : code                         ROUTINE: aci_delete          |
+--------------------------------------------------------------------+

   PURPOSE :

*/

GLOBAL void aci_delete(int handle)
{
  TRACE_FUNCTION ("aci_delete ()");

  if (handle >= 0 AND
      handle <  MAX_ACI_HANDLER)
  {
    aci_ext_handler[handle].prim_handler = NULL;
    aci_ext_handler[handle].cmd_handler = NULL;
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_ACIE            |
| STATE  : code                         ROUTINE: aci_check_primitive |
+--------------------------------------------------------------------+

   PURPOSE :

*/

GLOBAL BOOL aci_check_primitive(ULONG opc, void * data)
{
  UBYTE i;

  //TRACE_FUNCTION ("aci_check_primitive()");

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
#ifdef _SIMULATION_ 
#ifdef BT_INTERFACE
if(!pSlotBt)
{
	TRACE_EVENT_P1 ("mfw_acie:pSlotBt = %d",pSlotBt);
  bt_init();
}
#endif
#endif
  /*
   * primitive is not processed by the extension
   */
  return FALSE;
}

/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_ACIE            |
| STATE  : code                         ROUTINE: aci_check_command   |
+--------------------------------------------------------------------+

   PURPOSE :

*/

GLOBAL BOOL aci_check_command(char * cmd)
{
  UBYTE i;

  TRACE_FUNCTION ("aci_check_command()");

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
