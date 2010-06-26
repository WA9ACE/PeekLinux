/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_sima.c      $|
| $Author:: Es                          $Revision::  1              $|
| CREATED: 13.10.98                     $Modtime:: 2.03.00 15:14    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_SIMA

   PURPOSE : This modul contains the functions for SIM management.

	Jan 16, 2006 DR: OMAPS00061460 - Shashi Shekar B.S.
	Description: SAT Icon support
	Solution : SAT icon support added.

*/


#define ENTITY_MFW

#include "mfw_sys.h"

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

#include "prim.h"

#include "message.h"
#include "prim.h"
#include "aci_cmh.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "psa.h"
#include "psa_sim.h"

#include "mfw_mfw.h"
#include "mfw_nm.h"
#include "mfw_sim.h"
#include "mfw_simi.h"
#include "mfw_sima.h"
#include "mfw_nmi.h"
#include "mfw_phb.h"
#include "mfw_cm.h"
#include "mfw_cmi.h"
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
#include "mfw_sat.h"
#endif

#ifndef FF_2TO1_PS
#include "p_sim.val"
#endif

#include <string.h>

#define hCommSIM _ENTITY_PREFIXED(hCommSIM)
#if defined (NEW_FRAME)
EXTERN T_HANDLE      hCommSIM;        /* SIM    Communication       */
#else
EXTERN T_VSI_CHANDLE hCommSIM;        /* SIM    Communication       */
#endif

EXTERN void          aci_create (BOOL (*r_cb)(USHORT opc, void * data),
                                 BOOL (*cmd)(char *));

/* Added to remove warning Aug - 11 */
#ifdef NEPTUNE_BOARD
EXTERN void sim_signal (MfwEvt event, void *para);
#endif  /* NEPTUNE_BOARD */
/* End - remove warning Aug - 11 */

    
T_MFW_READ_CALLBACK read_callback_data;

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
EXTERN U8 read_image_instance_file;
#endif

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SIMA             |
| STATE  : code                         ROUTINE: sima_response_cb    |
+--------------------------------------------------------------------+

   PURPOSE : Response Callback Handler.

*/

GLOBAL BOOL sima_response_cb (ULONG opc, void * data)
{
//  TRACE_FUNCTION ("sim_response_cb()");

  switch (opc)
  {
    case SIM_MMI_INSERT_IND:
      /* MMI relevant parameters from the SIM card has received. */
      rAT_PlusCFUNP ((T_SIM_MMI_INSERT_IND *)data);
      return FALSE;   /* processed also by ACI */

    case SIM_ACTIVATE_CNF:
      sim_mmi_parameter((T_SIM_ACTIVATE_CNF *)data);
      return FALSE;

    case SIM_ACTIVATE_IND:
      sim_mmi_update_parameter((T_SIM_ACTIVATE_IND *)data);
      return FALSE;
  }

  return FALSE;       /* not processed by extension */
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SIMA             |
| STATE  : code                         ROUTINE: sima_init           |
+--------------------------------------------------------------------+

   PURPOSE : Install AT-Command Extension.

*/

void sima_init ()
{
  TRACE_FUNCTION ("sima_init()");
}


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SIMA               |
| STATE  : code                         ROUTINE: sim_read_sim          |
+----------------------------------------------------------------------+


   PURPOSE :   Request to read SIM card.

*/

void sim_read_sim(USHORT data_id, UBYTE len, UBYTE max_length)
{
  SHORT table_id;

  TRACE_FUNCTION ("sim_read_sim()");

  table_id = psaSIM_atbNewEntry();

  if(table_id NEQ NO_ENTRY)
  {
    simShrdPrm.atb[table_id].ntryUsdFlg = TRUE;
    simShrdPrm.atb[table_id].accType    = ACT_RD_DAT;
    // R99 SIM Interface changes . Indicates whether path_info variable has valid values 
    simShrdPrm.atb[table_id].v_path_info  = FALSE;
    simShrdPrm.atb[table_id].reqDataFld = data_id;
    simShrdPrm.atb[table_id].dataOff    = 0;
    simShrdPrm.atb[table_id].dataLen    = len;
    simShrdPrm.atb[table_id].recMax     = max_length;
    simShrdPrm.atb[table_id].exchData   = NULL;
    simShrdPrm.atb[table_id].rplyCB     = sim_read_sim_cb;

    simShrdPrm.aId = table_id;

    if(psaSIM_AccessSIMData( ) < 0)
    {
      TRACE_EVENT("FATAL ERROR");
    }
  }
}

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SIMA               |
| STATE  : code                         ROUTINE: sim_read_sim_icon     |
+----------------------------------------------------------------------+


   PURPOSE :   Request to read SIM card EF IMG. This is separated from the 
			   above SIM read function as we have to specify the record number 
			   also while reading the EF record & we dont want this functionality 
			   for Lite variants.

*/

void sim_read_sim_icon(T_SIM_ACCESS_PARAM *sim_icon_read_param)
{
  SHORT table_id;

  TRACE_FUNCTION ("sim_read_sim_icon()");

  table_id = psaSIM_atbNewEntry();

  if(table_id NEQ NO_ENTRY)
  {
    simShrdPrm.atb[table_id].ntryUsdFlg = TRUE;
    simShrdPrm.atb[table_id].accType    = ACT_RD_REC;
    // R99 SIM Interface changes. Indicates whether path_info variable has valid values 
    simShrdPrm.atb[table_id].v_path_info  = FALSE;
    simShrdPrm.atb[table_id].reqDataFld = sim_icon_read_param -> data_id;
    simShrdPrm.atb[table_id].dataOff    = sim_icon_read_param -> offset;
    simShrdPrm.atb[table_id].dataLen    = sim_icon_read_param -> len;
    simShrdPrm.atb[table_id].recMax     = sim_icon_read_param -> max_length;
    simShrdPrm.atb[table_id].exchData   = NULL;
	/* Read the Record number provided, only if available */
	if(sim_icon_read_param -> record_number)
    simShrdPrm.atb[table_id].recNr      = sim_icon_read_param -> record_number;
    simShrdPrm.atb[table_id].rplyCB     = sim_read_sim_cb;

    simShrdPrm.aId = table_id;

    if(psaSIM_AccessSIMData( ) < 0)
    {
      TRACE_EVENT("FATAL ERROR");
    }
  }
}
#endif

/*
+---------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SIMA              |
| STATE  : code                         ROUTINE: sim_read_sim_cb      |
+---------------------------------------------------------------------+


   PURPOSE :   Call back for SIM read.

*/

void sim_read_sim_cb(SHORT table_id)
{
  TRACE_FUNCTION ("sim_read_sim_cb()");

  simShrdPrm.atb[table_id].ntryUsdFlg = FALSE;

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
    /* If we are reading the image data contents, 
    we dont need to enter the switch statement */
    if (read_image_instance_file)
	{
		sim_img_instance_data_cnf(simShrdPrm.atb[table_id].errCode, 
			simShrdPrm.atb[table_id].exchData);
        return;
	}
#endif

  switch (simShrdPrm.atb[table_id].reqDataFld)
  {
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
    /* EF-IMG contents */
    case SIM_IMG:
      sim_img_cnf(simShrdPrm.atb[table_id].errCode, simShrdPrm.atb[table_id].recNr, 
		  (UBYTE *)simShrdPrm.atb[table_id].exchData);
      break;
#endif

    /* Service provide name */
    case SIM_SPN:
      nm_spn_cnf(simShrdPrm.atb[table_id].errCode, (T_EF_SPN *)simShrdPrm.atb[table_id].exchData);
      break;

    /* group identifier level 1 */
    case SIM_GID1:
      sim_gid1_cnf(simShrdPrm.atb[table_id].errCode, simShrdPrm.atb[table_id].exchData);
      break;

    /* group identifier level 2 */
    case SIM_GID2:
      sim_gid2_cnf(simShrdPrm.atb[table_id].errCode, simShrdPrm.atb[table_id].exchData);
      break;

    case SIM_SST:
      sim_read_sst_cnf(simShrdPrm.atb[table_id].errCode, simShrdPrm.atb[table_id].exchData);
      break;
	/*MC CONQ5862, 13/06/02*, if not one of above data fileds, send event to MMI*/
    default:
    	read_callback_data.error_code = simShrdPrm.atb[table_id].errCode;
    	read_callback_data.read_buffer = simShrdPrm.atb[table_id].exchData;
    	sim_signal(E_SIM_READ_CALLBACK, (void*)&read_callback_data);
      break;
  }
}


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SIMA               |
| STATE  : code                         ROUTINE: sim_write_sim         |
+----------------------------------------------------------------------+


   PURPOSE :   Request to write SIM card.

*/

void sim_write_sim(USHORT data_id, UBYTE *data, UBYTE length)
{
  SHORT table_id;

  /* Write preferred PLMN list in SIM card */
  table_id = psaSIM_atbNewEntry();

  if(table_id NEQ NO_ENTRY)
  {
    simShrdPrm.atb[table_id].ntryUsdFlg = TRUE;
    simShrdPrm.atb[table_id].accType    = ACT_WR_DAT;
    //R99 Sim Interface changes. Indicates whether path_info variable has valid values 
    simShrdPrm.atb[table_id].v_path_info  = FALSE;
    simShrdPrm.atb[table_id].reqDataFld = data_id;
    simShrdPrm.atb[table_id].dataOff    = 0;
    simShrdPrm.atb[table_id].dataLen    = length;
    simShrdPrm.atb[table_id].exchData   = data;
    simShrdPrm.atb[table_id].rplyCB     = sim_write_sim_cb;

    simShrdPrm.aId = table_id;

    if(psaSIM_AccessSIMData( ) < 0)
    {
      TRACE_EVENT("FATAL ERROR");
    }
  }
}


/*
+---------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SIMA              |
| STATE  : code                         ROUTINE: sim_write_sim_cb     |
+---------------------------------------------------------------------+


   PURPOSE :   Call back for SIM write.

*/

void sim_write_sim_cb(SHORT table_id)
{
  simShrdPrm.atb[table_id].ntryUsdFlg = FALSE;

#ifdef FF_2TO1_PS
  if (simShrdPrm.atb[table_id].errCode EQ CAUSE_SIM_NO_ERROR)
#else
  if (simShrdPrm.atb[table_id].errCode EQ SIM_NO_ERROR)
#endif
  {
    TRACE_EVENT("FATAL ERROR");
  }
}
