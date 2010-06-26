/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_nma.c       $|
| $Author:: Es                          $Revision::  1              $|
| CREATED: 13.10.98                     $Modtime:: 2.03.00 16:26    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_NMA

   PURPOSE : This modul contains the functions for network management.


   $History:: mfw_nma.c                                              $
 *
 * *****************  Version 17  *****************
 * User: Es           Date: 2.03.00    Time: 16:31
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * use 'aci_delete()' to clear the primitive routing for mfw_nm.
 *
 * *****************  Version 16  *****************
 * User: Vo           Date: 10.01.00   Time: 14:41
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Bug fix: search operator list for long name, short name and numeric
 * name
 *
 * *****************  Version 15  *****************
 * User: Vo           Date: 2.07.99    Time: 17:55
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 14  *****************
 * User: Vo           Date: 20.05.99   Time: 17:40
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 13  *****************
 * User: Es           Date: 15.03.99   Time: 18:22
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 12  *****************
 * User: Vo           Date: 11.03.99   Time: 14:38
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 11  *****************
 * User: Vo           Date: 17.02.99   Time: 19:03
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 10  *****************
 * User: Vo           Date: 12.02.99   Time: 18:59
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 9  *****************
 * User: Vo           Date: 9.02.99    Time: 14:54
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 8  *****************
 * User: Es           Date: 8.12.98    Time: 16:53
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 7  *****************
 * User: Vo           Date: 24.11.98   Time: 21:40
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * first successful compilation of MFW
|
| *****************  Version 6  *****************
| User: Le           Date: 11.11.98   Time: 13:35
| Updated in $/GSM/DEV/MS/SRC/MFW
|
| *****************  Version 5  *****************
| User: Le           Date: 10.11.98   Time: 16:07
| Updated in $/GSM/DEV/MS/SRC/MFW
|
| *****************  Version 4  *****************
| User: Le           Date: 10.11.98   Time: 11:08
| Updated in $/GSM/DEV/MS/SRC/MFW
|
| *****************  Version 3  *****************
| User: Le           Date: 27.10.98   Time: 15:59
| Updated in $/GSM/DEV/MS/SRC/MFW
|
| *****************  Version 2  *****************
| User: Le           Date: 21.10.98   Time: 16:20
| Updated in $/GSM/DEV/MS/SRC/MFW
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

#include "message.h"
#include "prim.h"
#include "aci_cmh.h"
#include "cus_aci.h"
#include "mfw_mfw.h"
#include "mfw_nm.h"
#include "mfw_sima.h"
#include "mfw_nmi.h"
#include "mfw_sim.h"
#include "mfw_simi.h"
#include "mfw_nma.h"

#include "mfw_phb.h"
#include "mfw_cm.h"

#include "ksd.h"

#include "mfw_ss.h"

#include <string.h>

#define hCommMM _ENTITY_PREFIXED(hCommMM)
#if defined (NEW_FRAME)
EXTERN T_HANDLE      hCommMM;        /* MM    Communication       */
#else
EXTERN T_VSI_CHANDLE hCommMM;        /* MM    Communication       */
#endif
#ifndef NEPTUNE_BOARD
EXTERN BOOL cmhMM_FindNumeric ( T_OPER_ENTRY * plmnDesc,  /* Side access into cmh_mmf.c */
                          const CHAR *numStr );
EXTERN BOOL cmhMM_FindName    ( T_OPER_ENTRY * plmnDesc,  /* Side access into cmh_mmf.c */
                          const CHAR *string, T_ACI_CPOL_FRMT format );
#else
EXTERN T_OPER_NTRY * cmhMM_FindNumeric ( CHAR * numStr );
//EXTERN T_OPER_NTRY * cmhMM_FindLongName( CHAR * longStr );
//EXTERN T_OPER_NTRY * cmhMM_FindShrtName( CHAR * shrtStr );
EXTERN T_OPER_NTRY * cmhMM_FindName( const CHAR *string, T_ACI_CPOL_FRMT format);
#endif

EXTERN void          aci_create (BOOL (*r_cb)(ULONG opc, void * data),
                                 BOOL (*cmd)(char *));


typedef struct
{
#ifdef FF_2TO1_PS
  T_CAUSE_ps_cause nreg_cause;
#else
  USHORT nreg_cause;             /* nreg cause from MMR_NREG_IND          */
#endif
  UBYTE  forbidden_id[30];       /* forbidden indicator from MMR_PLMN_IND */
  UBYTE  fieldstrength [30];     /* fieldstrength from MMR_PLMN_IND       */
} T_NM_ACI_DATA;

T_NM_ACI_DATA  nm_aci_data;


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_NMA              |
| STATE  : code                         ROUTINE: nma_response_cb     |
+--------------------------------------------------------------------+

   PURPOSE : Response Callback Handler.

*/

#ifdef FF_2TO1_PS
GLOBAL BOOL nma_response_cb (ULONG opc, void * data)
{
/* BEGIN ADD: Neptune alignment*/
#ifdef NEPTUNE_BOARD
  TRACE_FUNCTION ("nma_response_cb()");

  if (opc EQ MMREG_INFO_IND)
  {
      /*
       * send this NITZ information to MMI
       */
    nm_nitz_info_ind((T_MMREG_INFO_IND *)data);
      return FALSE;   /* normal processing of ACI   */
  }

#else
/* END ADD: Neptune Alignment*/
  T_MMREG_DETACH_IND * nreg_ind;//x0066814 (Geetha), OMAPS00098351
  T_MMREG_INFO_IND * mmr_nitz_ind; //x0066814 (Geetha), OMAPS00098351
  TRACE_FUNCTION ("nma_response_cb()");

  switch (opc)
  {
    case MMREG_PLMN_IND:
      /*
       * available PLMN list, store fieldstrength
       * and forbidden indicator (not yet included)
       */
      return FALSE;   /* normal processing of ACI   */

    case MMREG_DETACH_IND:
      /*
       * store the nreg cause
       */
      nreg_ind = (T_MMREG_DETACH_IND *)data;
      nm_aci_data.nreg_cause = nreg_ind->ps_cause;
      return FALSE;   /* normal processing of ACI   */

  case MMREG_INFO_IND:
     
       /* send this NITZ information to MMI
       */
       TRACE_FUNCTION("  case MMREG_INFO_IND: ");
    nm_nitz_info_ind((T_MMREG_INFO_IND *)data);
      return FALSE;   /* normal processing of ACI   */

//x0066814 (Geetha), OMAPS00098351, To get time and timezone from network
	 case GMMREG_INFO_IND:
	 	       TRACE_FUNCTION("  case GMMREG_INFO_IND: ");
			   memcpy (&(mmr_nitz_ind->plmn), &(gmmreg_info_ind->plmn), sizeof(T_plmn));

    			memcpy (&(mmr_nitz_ind->full_name),&(gmmreg_info_ind->full_net_name_gmm), sizeof (T_full_net_name_gmm));

    			memcpy (&(mmr_nitz_ind->short_name),&(gmmreg_info_ind->short_net_name_gmm), sizeof (T_short_net_name_gmm));

   			 memcpy (&(mmr_nitz_ind->ntz), &(gmmreg_info_ind->net_time_zone),sizeof (T_net_time_zone));

    			memcpy (&(mmr_nitz_ind->time), &(gmmreg_info_ind->net_time), sizeof(T_net_time));

			   
 nm_nitz_info_ind(* mmr_nitz_ind );
      return FALSE;   /* normal processing of ACI   */
  }

/* BEGIN ADD: Neptune alignment*/
#endif
/* END ADD: Neptune alignment*/
  return FALSE;       /* not processed by extension */
}
#else
GLOBAL BOOL nma_response_cb (ULONG opc, void * data)
{
  T_MMR_NREG_IND * nreg_ind;
  T_MMR_INFO_IND  mmr_nitz_ind;//x0066814 (Geetha), OMAPS00098351
  T_GMMREG_INFO_IND *gmmreg_info_ind;//x0066814 (Geetha), OMAPS00098351

  TRACE_FUNCTION ("nma_response_cb()");

  switch (opc)
  {
    case MMR_PLMN_IND:
      /*
       * available PLMN list, store fieldstrength
       * and forbidden indicator (not yet included)
       */
      return FALSE;   /* normal processing of ACI   */

    case MMR_NREG_IND:
      /*
       * store the nreg cause
       */
      nreg_ind = (T_MMR_NREG_IND *)data;
      nm_aci_data.nreg_cause = nreg_ind->cause;
      return FALSE;   /* normal processing of ACI   */

  case MMR_INFO_IND:
TRACE_FUNCTION("  case MMREG_INFO_IND: ");
      /*
       * send this NITZ information to MMI
       */
    nm_nitz_info_ind((T_MMR_INFO_IND *)data);
     return FALSE;   /* normal processing of ACI   */

// x0066814 (Geetha), OMAPS00098351 (NITZ), Timezone info handling
	 case GMMREG_INFO_IND:
	  TRACE_FUNCTION("  case GMMREG_INFO_IND: ");
	   
	 gmmreg_info_ind= (T_GMMREG_INFO_IND *)data;
	 memcpy (&(mmr_nitz_ind.plmn), &(gmmreg_info_ind->plmn), sizeof(T_plmn));

	 memcpy (&(mmr_nitz_ind.full_name),&(gmmreg_info_ind->full_net_name_gmm), sizeof (T_full_name));
	 memcpy (&(mmr_nitz_ind.short_name),&(gmmreg_info_ind->short_net_name_gmm), sizeof (T_short_name));

	 memcpy (&(mmr_nitz_ind.ntz), &(gmmreg_info_ind->net_time_zone),sizeof (T_ntz));
	 memcpy (&(mmr_nitz_ind.time), &(gmmreg_info_ind->net_time), sizeof(T_time));

	   
	 nm_nitz_info_ind(&mmr_nitz_ind);
      return FALSE;   /* normal processing of ACI   */
  }

  return FALSE;       /* not processed by extension */
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_NMA              |
| STATE  : code                         ROUTINE: nma_init            |
+--------------------------------------------------------------------+

   PURPOSE : Install AT-Command Extension.

*/

GLOBAL void nma_init ()
{
  TRACE_FUNCTION ("nma_init()");
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_NMA              |
| STATE  : code                         ROUTINE: sAT_PlusCOPSE       |
+--------------------------------------------------------------------+

   PURPOSE : Converts a given PLMN indication into the two other formats.

*/
#ifdef NO_ASCIIZ
GLOBAL void sAT_PlusCOPSE(UBYTE *oper, UBYTE format,
                          T_MFW_LNAME *long_name,
                          T_MFW_SNAME *short_name,
                          UBYTE *numeric_name)
{
#ifndef NEPTUNE_BOARD
  T_OPER_ENTRY plmnDesc;
  BOOL found;
#else
T_OPER_NTRY * network_entry;
#endif

  TRACE_FUNCTION ("sAT_PlusCOPSE()");

  long_name->len = 0;
  short_name->len = 0;
  strcpy ((char *)numeric_name, "");

  switch (format)
  {
    case COPS_FRMT_Numeric:
#ifndef NEPTUNE_BOARD        
      strcpy ((char *)numeric_name, (char *)oper);
      found = cmhMM_FindNumeric (&plmnDesc, (char *)oper);
      if (found)
      {
        UBYTE length=strlen((const char*)plmnDesc.longName);
        long_name->len = MINIMUM(LONG_NAME-1, length);
        memcpy (long_name->data, plmnDesc.longName, long_name->len);
        long_name->data[long_name->len] = '\0';
        long_name->dcs = MFW_ASCII;
	  length=strlen((const char*)plmnDesc.shrtName);
        short_name->len = MINIMUM(SHORT_NAME-1, length);
        memcpy (short_name->data, plmnDesc.shrtName, short_name->len);
        short_name->data[short_name->len] = '\0';
        short_name->dcs = MFW_ASCII;
      }
#else
      strcpy ((char *)numeric_name, (char *)oper);
      network_entry = cmhMM_FindNumeric ((char *)oper);
      if (network_entry NEQ NULL)
      {
        if (network_entry->longName)
        {
          long_name->len = MINIMUM(LONG_NAME-1, strlen((char *)network_entry->longName));
          memcpy (long_name->data, network_entry->longName, long_name->len);
          long_name->data[long_name->len] = '\0';
          long_name->dcs = MFW_ASCII;
        }
        if (network_entry->shrtName)
        {
          short_name->len = MINIMUM(SHORT_NAME-1, strlen((char *)network_entry->shrtName));
          memcpy (short_name->data, network_entry->shrtName, short_name->len);
          short_name->data[short_name->len] = '\0';
          short_name->dcs = MFW_ASCII;
        }
      }
#endif
      break;

    case COPS_FRMT_Long:
      if (oper)
      {
        UBYTE length=strlen((const char *)oper);
        long_name->len = MINIMUM(LONG_NAME-1, length);
        memcpy (long_name->data, oper, long_name->len);
        long_name->data[long_name->len] = '\0';
        long_name->dcs = MFW_ASCII;
      }
#ifndef NEPTUNE_BOARD
      found = cmhMM_FindName (&plmnDesc, (char *)oper, CPOL_FRMT_Long); /*a0393213 compiler warning removal - COPS_FRMT_Long changed to CPOL_FRMT_Long*/
      if (found)
      {
        UBYTE length;
        numeric_name[0] = ((plmnDesc.mcc >> 8) & 0xf) + '0';
        numeric_name[1] = ((plmnDesc.mcc >> 4) & 0xf) + '0';
        numeric_name[2] = ( plmnDesc.mcc       & 0xf) + '0';
        numeric_name[3] = ((plmnDesc.mnc >> 8) & 0xf) + '0';
        numeric_name[4] = ((plmnDesc.mnc >> 4) & 0xf) + '0';
        if ((plmnDesc.mnc & 0xf) EQ 0xf)
          numeric_name[5] = '\0'; /* 2-digit-MNC */
        else
          numeric_name[5] = ( plmnDesc.mnc       & 0xf) + '0';
        numeric_name[6] = '\0';
        length=strlen((const char*)plmnDesc.shrtName);
        short_name->len = MINIMUM(SHORT_NAME-1, length);
        memcpy (short_name->data, plmnDesc.shrtName, short_name->len);
        short_name->data[short_name->len] = '\0';
        short_name->dcs = MFW_ASCII;
      }
#else

      network_entry = cmhMM_FindName ((char *)oper, (T_ACI_CPOL_FRMT) COPS_FRMT_Long);
      if (network_entry NEQ NULL)
      {
        numeric_name[0] = ((network_entry->mcc >> 8) & 0xf) + '0';
        numeric_name[1] = ((network_entry->mcc >> 4) & 0xf) + '0';
        numeric_name[2] = ( network_entry->mcc       & 0xf) + '0';
        numeric_name[3] = ((network_entry->mnc >> 8) & 0xf) + '0';
        numeric_name[4] = ((network_entry->mnc >> 4) & 0xf) + '0';
        if ((network_entry->mnc & 0xf) EQ 0xf)
          numeric_name[5] = '\0'; /* 2-digit-MNC */
        else
          numeric_name[5] = ( network_entry->mnc       & 0xf) + '0';
        numeric_name[6] = '\0';

        if (network_entry->shrtName)
        {
          short_name->len = MINIMUM(SHORT_NAME-1, strlen((char *)network_entry->shrtName));
          memcpy (short_name->data, network_entry->shrtName, short_name->len);
          short_name->data[short_name->len] = '\0';
          short_name->dcs = MFW_ASCII;
        }
      }
#endif
      break;

    case COPS_FRMT_Short:
      if (oper)
      {
      	 UBYTE length=strlen((const char *)oper);
        short_name->len = MINIMUM(SHORT_NAME-1, length);
        memcpy (short_name->data, oper, short_name->len);
        short_name->data[short_name->len] = '\0';
        short_name->dcs = MFW_ASCII;
      }
#ifndef NEPTUNE_BOARD
      found = cmhMM_FindName (&plmnDesc, (char *)oper, CPOL_FRMT_Short); /*a0393213 compiler warnings removal - COPS_FRMT_Short changed to CPOL_FRMT_Short*/
      if (found)
      {
        UBYTE length;
        numeric_name[0] = ((plmnDesc.mcc >> 8) & 0xf) + '0';
        numeric_name[1] = ((plmnDesc.mcc >> 4) & 0xf) + '0';
        numeric_name[2] = ( plmnDesc.mcc       & 0xf) + '0';
        numeric_name[3] = ((plmnDesc.mnc >> 8) & 0xf) + '0';
        numeric_name[4] = ((plmnDesc.mnc >> 4) & 0xf) + '0';
        if ((plmnDesc.mnc & 0xf) EQ 0xf)
          numeric_name[5] = '\0'; /* 2-digit-MNC */
        else
          numeric_name[5] = ( plmnDesc.mnc       & 0xf) + '0';
        numeric_name[6] = '\0'; /* 3-digit-MNC */
        length=strlen((const char*)plmnDesc.longName);
        long_name->len = MINIMUM(LONG_NAME-1, length);
        memcpy (long_name->data, plmnDesc.longName, long_name->len);
        long_name->data[long_name->len] = '\0';
        long_name->dcs = MFW_ASCII;
      }
#else
network_entry = cmhMM_FindName ((char *)oper, (T_ACI_CPOL_FRMT)COPS_FRMT_Short);
      if (network_entry NEQ NULL)
      {
        numeric_name[0] = ((network_entry->mcc >> 8) & 0xf) + '0';
        numeric_name[1] = ((network_entry->mcc >> 4) & 0xf) + '0';
        numeric_name[2] = ( network_entry->mcc       & 0xf) + '0';
        numeric_name[3] = ((network_entry->mnc >> 8) & 0xf) + '0';
        numeric_name[4] = ((network_entry->mnc >> 4) & 0xf) + '0';
        if ((network_entry->mnc & 0xf) EQ 0xf)
          numeric_name[5] = '\0'; /* 2-digit-MNC */
        else
          numeric_name[5] = ( network_entry->mnc       & 0xf) + '0';
        numeric_name[6] = '\0'; /* 3-digit-MNC */
        
        if (network_entry->longName) 
        {
          long_name->len = MINIMUM(LONG_NAME-1, strlen((char *)network_entry->longName));
          memcpy (long_name->data, network_entry->longName, long_name->len);
          long_name->data[long_name->len] = '\0';
          long_name->dcs = MFW_ASCII;
        }
      }
#endif
      break;
  }
}
#else
GLOBAL void sAT_PlusCOPSE(UBYTE *oper, UBYTE format, UBYTE *long_name,
                          UBYTE *short_name, UBYTE *numeric_name)
{
#ifndef NEPTUNE_BOARD  
 T_OPER_ENTRY plmnDesc;
  BOOL found;
#else
  T_OPER_NTRY * network_entry;
#endif

  TRACE_FUNCTION ("sAT_PlusCOPSE()");

  strcpy ((char *)long_name, "");
  strcpy ((char *)short_name, "");
  strcpy ((char *)numeric_name, "");

  switch (format)
  {
    case COPS_FRMT_Numeric:
#ifndef NEPTUNE_BOARD
      strcpy ((char *)numeric_name, (char *)oper);
      found = cmhMM_FindNumeric (&plmnDesc, (char *)oper);
      if (found)
      {
        strncpy ((char *)long_name, plmnDesc.longName, LONG_NAME-1);
        long_name[LONG_NAME-1] = '\0';

        strncpy ((char *)short_name, plmnDesc.shrtName, SHORT_NAME-1);
        short_name[SHORT_NAME-1] = '\0';
      }
#else
      strcpy ((char *)numeric_name, (char *)oper);
      network_entry = cmhMM_FindNumeric ((char *)oper);
      if (network_entry NEQ NULL)
      {
        if (network_entry->longName)
        {
          strncpy ((char *)long_name, (char *)network_entry->longName, LONG_NAME-1);
          long_name[LONG_NAME-1] = '\0';
        }
        if (network_entry->shrtName)
        {
          strncpy ((char *)short_name, (char *)network_entry->shrtName, SHORT_NAME-1);
          short_name[SHORT_NAME-1] = '\0';
        }
      }
#endif
      break;

    case COPS_FRMT_Long:
      if (oper)
      {
        strncpy ((char *)long_name, (char *)oper, LONG_NAME-1);
        long_name[LONG_NAME-1] = '\0';
      }
#ifndef NEPTUNE_BOARD
      found = cmhMM_FindName (&plmnDesc, (char *)oper, COPS_FRMT_Long);
      if (found)
      {
        numeric_name[0] = ((plmnDesc.mcc >> 8) & 0xf) + '0';
        numeric_name[1] = ((plmnDesc.mcc >> 4) & 0xf) + '0';
        numeric_name[2] = ( plmnDesc.mcc       & 0xf) + '0';
        numeric_name[3] = ((plmnDesc.mnc >> 8) & 0xf) + '0';
        numeric_name[4] = ((plmnDesc.mnc >> 4) & 0xf) + '0';
        if ((plmnDesc.mnc & 0xf) EQ 0xf)
          numeric_name[5] = '\0'; /* 2-digit-MNC */
        else
          numeric_name[5] = ( plmnDesc.mnc       & 0xf) + '0';
        numeric_name[6] = '\0'; /* 3-digit-MNC */

        strncpy ((char *)short_name, plmnDesc.shrtName, SHORT_NAME-1);
        short_name[SHORT_NAME-1] = '\0'; 
      }
#else
      network_entry = cmhMM_FindName ((char *)oper, COPS_FRMT_Long);
      if (network_entry NEQ NULL)
      {
        numeric_name[0] = ((network_entry->mcc >> 8) & 0xf) + '0';
        numeric_name[1] = ((network_entry->mcc >> 4) & 0xf) + '0';
        numeric_name[2] = ( network_entry->mcc       & 0xf) + '0';
        numeric_name[3] = ((network_entry->mnc >> 8) & 0xf) + '0';
        numeric_name[4] = ((network_entry->mnc >> 4) & 0xf) + '0';
        if ((network_entry->mnc & 0xf) EQ 0xf)
          numeric_name[5] = '\0'; /* 2-digit-MNC */
        else
          numeric_name[5] = ( network_entry->mnc       & 0xf) + '0';
        numeric_name[6] = '\0'; /* 3-digit-MNC */

        if (network_entry->shrtName)
        {
          strncpy ((char *)short_name, (char *)network_entry->shrtName, SHORT_NAME-1);
          short_name[SHORT_NAME-1] = '\0'; 
        }
      }
#endif
      break;

    case COPS_FRMT_Short:
      if (oper)
      {
        strncpy ((char *)short_name, (char *)oper, SHORT_NAME-1);
        short_name[SHORT_NAME-1] = '\0'; 
      }
#ifndef NEPTUNE_BOARD
      found = cmhMM_FindName (&plmnDesc, (char *)oper, COPS_FRMT_Short);
      if (found)
      {
        numeric_name[0] = ((plmnDesc.mcc >> 8) & 0xf) + '0';
        numeric_name[1] = ((plmnDesc.mcc >> 4) & 0xf) + '0';
        numeric_name[2] = ( plmnDesc.mcc       & 0xf) + '0';
        numeric_name[3] = ((plmnDesc.mnc >> 8) & 0xf) + '0';
        numeric_name[4] = ((plmnDesc.mnc >> 4) & 0xf) + '0';
        if ((plmnDesc.mnc & 0xf) EQ 0xf)
          numeric_name[5] = '\0'; /* 2-digit-MNC */
        else
          numeric_name[5] = ( plmnDesc.mnc       & 0xf) + '0';
        numeric_name[6] = '\0'; /* 3-digit-MNC */

        strncpy ((char *)long_name, plmnDesc.longName, LONG_NAME-1);
        long_name[LONG_NAME-1] = '\0';
      }
#else
      network_entry = cmhMM_FindName ((char *)oper, COPS_FRMT_Short);
      if (network_entry NEQ NULL)
      {
        numeric_name[0] = ((network_entry->mcc >> 8) & 0xf) + '0';
        numeric_name[1] = ((network_entry->mcc >> 4) & 0xf) + '0';
        numeric_name[2] = ( network_entry->mcc       & 0xf) + '0';
        numeric_name[3] = ((network_entry->mnc >> 8) & 0xf) + '0';
        numeric_name[4] = ((network_entry->mnc >> 4) & 0xf) + '0';
        if ((network_entry->mnc & 0xf) EQ 0xf)
          numeric_name[5] = '\0'; /* 2-digit-MNC */
        else
          numeric_name[5] = ( network_entry->mnc       & 0xf) + '0';
        numeric_name[6] = '\0'; /* 3-digit-MNC */

        if (network_entry->longName) 
        {
          strncpy ((char *)long_name, (char *)network_entry->longName, LONG_NAME-1);
          long_name[LONG_NAME-1] = '\0';
        }
      }
#endif
      break;
  }
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_NMA              |
| STATE  : code                         ROUTINE: sAT_PlusCREGE       |
+--------------------------------------------------------------------+

   PURPOSE : Request NREG cause.

*/

#ifdef FF_2TO1_PS
GLOBAL T_CAUSE_ps_cause sAT_PlusCREGE (void)
{
  TRACE_FUNCTION ("sAT_PlusCREGE()");

  return nm_aci_data.nreg_cause;
}
#else
GLOBAL USHORT sAT_PlusCREGE (void)
{
  TRACE_FUNCTION ("sAT_PlusCREGE()");

  return nm_aci_data.nreg_cause;
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_NMA              |
| STATE  : code                         ROUTINE: sAT_PlusCOPSF       |
+--------------------------------------------------------------------+

   PURPOSE : Request fieldstrength and forbidden indicator for
             PLMN list.

*/

GLOBAL void sAT_PlusCOPSF(UBYTE ** forbidden_id, UBYTE ** fieldstrength)
{
  TRACE_FUNCTION ("sAT_PlusCOPSF()");

  *forbidden_id  = nm_aci_data.forbidden_id;
  *fieldstrength = nm_aci_data.fieldstrength;
}

