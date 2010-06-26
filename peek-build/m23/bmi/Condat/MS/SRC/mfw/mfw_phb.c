/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_phb.c       $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED: 7.1.99                       $Modtime:: 12.01.00 11:19   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_PHB

   PURPOSE : This modul contains phonebook management functions.

   HISTORY:

	June 05, 2007 DR:OMAPS00133241 x0061560(sateeshg)
       Description:ASTEC29607,SIM:The sim card should not return 9240 still.
       Solution     :New enum MFW_PHB_EXCT been sent while the command AT_EXCT is in progress
	
      Nov 03, 2006 DR:OMAPS00097209 a0393213(R.Prabakar)
      API Needed for Maximum Length Of Each Phonebook Category In SIM
      Solution: The interface of pb_read_sizes() changed. While calling the function it should be taken care of.
      
	Aug 18, 2006   ER: OMAPS00089840  x0039928
	Description: Changes to be made in MMI for the functionality "Delete all" ADN entries
	Solution: A new menu item "Delete All" is added in the phone book menu 
	to delete all the sim entries

	July  17, 2006  DR: OMAPS00082792  x0039928
	Description:Changes to be done in MMI for Move all entries from SIM to FFS (Defect 76492)
	Solution: Added condition to check for AT_EXCT return for sAT_PlusCPBW

      Oct 27, 2005	REF: OMAPS 48881 b-nekkare
      Description: PhoneBook: The modify option under phonebook doesn't 
                        work as expected.
       Solution: In case of SIM storage,deleted the previous 2nd comparision
                     with TON as then it would be TRUE if the previous saved 
                     number had a preceding'+'. So currently only checking for 
                     '+' to figure out the TON.


	Apr 14, 2005	REF: CRR 29991   xpradipg
	Description:	Optimisation 5: Remove the static allocation and use dynamic 
					allocation/ deallocation for pb_list and black_list
	Solution:	The static definition is removed and replaced with the dynamic
					allocation


       March 1 ,2004   REF: CRR 27832   x0012852 
	Bug:SIM PIN2 related Actions are displying wrong Error Response.
	Fix:MFW calls sAT_PlusCPBS() with third parameter as NULL.
*/
#define ENTITY_MFW

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif


#include "mfw_mfw.h"
#include "mfw_phb.h"
#include "mfw_phbi.h"
#include "mfw_cm.h"
#include "mfw_cmi.h"

#include "mfw_nm.h"
#include "mfw_sim.h"
#include "mfw_sima.h"
#include "mfw_nmi.h"
#include "mfw_simi.h"
#include "mfw_sms.h"
#include "mfw_smsi.h"
#include "mfw_win.h"

#include "ksd.h"
#include "psa.h"

#if defined (FAX_AND_DATA)
#include "aci_fd.h"
#endif

#include "message.h"
#include "prim.h"
#include "aci_cmh.h"

#include "cmh.h"
#include "phb.h"
#include "cmh_phb.h"

#include "mfw_ss.h"
#include "mfw_ssi.h"
#include "mfw_win.h"

#include "gdi.h"

#ifndef PCM_2_FFS
#include "pcm.h"
#include "mfw_ffs.h"
#endif


/* SPR#1112 - SH - Required for internal phonebook */
#ifdef INT_PHONEBOOK
#include "ATBPbGI.h"
#endif
    
/*************** MACRO definition ***********************************/
//   Apr 14, 2005	REF: CRR 29991   xpradipg
//	MACRO to allocate and deallocate memory for pb_list
#ifdef FF_MMI_OPTIM
#define PB_LIST_ALLOC(ptr,size) \
 	ptr = (T_ACI_PB_ENTR*)mfwAlloc(size);\
	if (NULL == ptr)\
			return MFW_PHB_FAIL;
#define PB_LIST_DEALLOC(ptr,size)\
	if(ptr)\
		mfwFree((U8*)ptr, size);
#define PB_LIST_SIZE (sizeof(T_ACI_PB_ENTR)*PHB_MAX_ENTRY_NUM)
		
#else
#define PB_LIST_ALLOC(ptr, size)
#define PB_LIST_DEALLOC(ptr, size)
#endif			
/********* current define *******************************************/
//   Apr 14, 2005	REF: CRR 29991   xpradipg
//	remove the static allocation
#ifndef FF_MMI_OPTIM
static T_ACI_PB_ENTR   pb_list[PHB_MAX_ENTRY_NUM];
#endif
#ifdef TI_PS_FFS_PHB
static T_ACI_PB_STAT pb_stat;
#endif
static T_MFW_UPN_LIST  upn_list;
static UBYTE cnt;     /* count of user personal numbers */

EXTERN MfwHdr * current_mfw_elem;

// Aug 18, 2006   ER: OMAPS00089840  x0039928
extern BOOL DeleteAll_flag;

//Added by Muthu Raja For PCM Replacement
#ifdef PCM_2_FFS
#define PSPDF_UPN_ID       "/PCM/UPN"           /* User Personal Numbers    */
#endif


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB              |
| STATE  : code                         ROUTINE: phb_create          |
+--------------------------------------------------------------------+


   PURPOSE :  create event for phonebook management
*/

T_MFW_HND phb_create(T_MFW_HND hWin, T_MFW_EVENT event, T_MFW_CB cbfunc)
{
  T_MFW_HDR *hdr;
  T_MFW_PHB  *phb_para;
  MfwHdr *insert_status =0;
  
  TRACE_FUNCTION ("phb_create()");

  hdr      = (T_MFW_HDR *) mfwAlloc(sizeof (T_MFW_HDR));
  phb_para  = (T_MFW_PHB *) mfwAlloc(sizeof (T_MFW_PHB));

  if (!hdr OR !phb_para)
  	{
    	TRACE_ERROR("ERROR: phb_create() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));
   		if(phb_para)
   			mfwFree((U8*)phb_para,sizeof(T_MFW_PHB));	
   		
	   	return FALSE;
  	}

  /*
   * initialisation of the handler
   */
  phb_para->emask   = event;
  phb_para->handler = cbfunc;

  hdr->data = phb_para;                           /* store parameter in node     */
  hdr->type = MFW_TYP_PHB;                        /* store type of event handler */

  /*
   * installation of the handler
   */
  insert_status = mfwInsert((T_MFW_HDR *)hWin, hdr);
  if(!insert_status)
	{
  		TRACE_ERROR("ERROR: phb_create() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)phb_para ,sizeof(T_MFW_PHB));
		return 0;
  	}
  return insert_status;
}


/*
+---------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB               |
| STATE  : code                         ROUTINE: phb_delete           |
+---------------------------------------------------------------------+


   PURPOSE :   delete a event for phonebook management

*/

T_MFW_RES phb_delete(T_MFW_HND h)
{
  TRACE_FUNCTION ("phb_delete()");
  
  if (!h OR !((T_MFW_HDR *)h)->data)
    return MFW_RES_ILL_HND;

  if (!mfwRemove((T_MFW_HDR *)h))
    return MFW_RES_ILL_HND;

  mfwFree((U8 *)(((T_MFW_HDR *) h)->data),sizeof(T_MFW_PHB));
  mfwFree((U8 *)h,sizeof(T_MFW_HDR));

  return MFW_RES_OK;
}


/*
+---------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB               |
| STATE  : code                         ROUTINE: phb_signal           |
+---------------------------------------------------------------------+

   PURPOSE : send a event signal.

*/

void phb_signal(T_MFW_EVENT event, void * para)
{/*MC, SPR 1389, we have to enable the display whenever 
	we send an event up to the MMI*/
UBYTE temp = dspl_Enable(0);
	
  TRACE_FUNCTION ("phb_signal()");
if (mfwSignallingMethod EQ 0)
  {
  /*
   * focus is on a window
   */
  if (mfwFocus)
    /*
     * send event to phonebook management
     * handler if available
     */
    if (phb_sign_exec (mfwFocus, event, para))
    { dspl_Enable(temp);/*MC, SPR 1389*/
	        return;
	 }

  /*
   * acutal focussed window is not available
   * or has no phonebook management handler,
   * then search all nodes from the root.
   */
   if (mfwRoot)
     phb_sign_exec (mfwRoot, event, para);
  }
  else
    {
      MfwHdr * h = 0;

      /*
       * Focus set, then start here
       */
      if (mfwFocus)
        h = mfwFocus;
      /*
       * Focus not set, then start root
       */
      if (!h)
        h = mfwRoot;

      /*
       * No elements available, return
       */

      while (h)


      {
        /*
         * Signal consumed, then return
         */
        if (phb_sign_exec (h, event, para))
        { dspl_Enable(temp);/*MC, SPR 1389*/
	        return;
	 	}

        /*
         * All windows tried inclusive root
         */
        if (h == mfwRoot)
        { dspl_Enable(temp);/*MC, SPR 1389*/
	        return;
	 	}

        /*
         * get parent window
         */
        h = mfwParent(mfwParent(h));
		if(h)
			h = ((MfwWin * )(h->data))->elems;
      }
      phb_sign_exec (mfwRoot, event, para);
    }
   dspl_Enable(temp);/*MC, SPR 1389*/

}

/*
+---------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB               |
| STATE  : code                         ROUTINE: phb_sign_exec        |
+---------------------------------------------------------------------+


   PURPOSE : Send a signal if PHB management handler.

*/

BOOL phb_sign_exec (T_MFW_HDR * cur_elem, T_MFW_EVENT event, T_MFW_PHB_PARA * para)
{
  TRACE_FUNCTION ("phb_sign_exec()");

  while (cur_elem)
  {
    /*
     * event handler is available
     */
    if (cur_elem->type EQ MFW_TYP_PHB)
    {
      T_MFW_PHB * phb_data;
      /*
       * handler is PHB management handler
       */
      phb_data = (T_MFW_PHB *)cur_elem->data;
      if (phb_data->emask & event)
      {
        /*
         * event is expected by the call back function
         */
        phb_data->event = event;
        switch (event)
        {
          case E_PHB_STATUS:
            memcpy (&phb_data->para.phb_status, para, sizeof (T_MFW_PHB_STATUS));
            break;
          case E_PHB_UPN_LIST:
            memcpy (&phb_data->para.upn_list, para, sizeof (T_MFW_UPN_LIST));
            break;
          /* SPR#1112 - SH - Add these events */
          case E_PHB_READY:
          	break;
          case E_PHB_BUSY:
          	break;
        }

        /*
         * if call back defined, call it
         */
        if (phb_data->handler)
        {
          // PATCH LE 06.06.00
          // store current mfw elem
          current_mfw_elem = cur_elem;
          // END PATCH LE 06.06.00

          if ((*(phb_data->handler)) (phb_data->event, (void *)&phb_data->para))
            return TRUE;
        }
      }
    }
    cur_elem = cur_elem->next;
  }
  return FALSE;
}


/*
+-------------------------------------------------------------------+
| PROJECT : GSM-PS (6147)         MODULE  : MFW_PHB                 |
| STATE   : code                  ROUTINE : phb_codePhbType         |
+-------------------------------------------------------------------+
  PURPOSE : This function is used to convert the type of phonebook
            used by MFW to the type of phonebook used by ACI.
*/
/*a0393213 warnings removal-argument type changed from T_PHB_TYPE*/
LOCAL T_ACI_PB_STOR phb_codePhbType ( T_MFW_PHB_TYPE inMem )
{
  switch ( inMem )
  {
    case ( PHB_FDN ): return PB_STOR_Fd;
    case ( PHB_LDN ): return PB_STOR_Ld;
    case ( PHB_ECC ): return PB_STOR_Ed;
    case ( PHB_ADN ): return PB_STOR_Ad;
    case ( PHB_BDN ): return PB_STOR_Bd;
    case ( PHB_LRN ): return PB_STOR_Lr;
    case ( PHB_SDN ): return PB_STOR_Sd;
    case ( PHB_LMN ): return PB_STOR_Lm;
    case ( PHB_ADN_FDN ): return PB_STOR_Af;
    case ( PHB_UPN ): return PB_STOR_Ud;
    default:          return PB_STOR_NotPresent;
  }
}


/*
+-------------------------------------------------------------------+
| PROJECT : GSM-PS (6147)         MODULE  : MFW_PHB                 |
| STATE   : code                  ROUTINE : phb_decodePhbType       |
+-------------------------------------------------------------------+
  PURPOSE : This function is used to convert the type of phonebook
            used by MFW to the type of phonebook used by ACI.
*/
/*a0393213 warnings removal-return type changed from T_PHB_TYPE*/
LOCAL T_MFW_PHB_TYPE phb_decodePhbType ( T_ACI_PB_STOR inMem )
{
  switch ( inMem )
  {
    case ( PB_STOR_Fd ): return PHB_FDN;
    case ( PB_STOR_Ld ): return PHB_LDN;
    case ( PB_STOR_Ed ): return PHB_ECC;
    case ( PB_STOR_Ad ): return PHB_ADN;
    case ( PB_STOR_Bd ): return PHB_BDN;
    case ( PB_STOR_Lr ): return PHB_LRN;
    case ( PB_STOR_Sd ): return PHB_SDN;
    case ( PB_STOR_Lm ): return PHB_LMN;
    case ( PB_STOR_Af ): return PHB_ADN_FDN;
    case ( PB_STOR_Ud ): return PHB_UPN;
    default:          return PHB_NONE;
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB              |
| STATE  : code                         ROUTINE: phb_cvtTon          |
+--------------------------------------------------------------------+


   PURPOSE :  This function is used to convert the type of number
              used by ACI to the type of number used by MFW.

*/

T_MFW_PHB_TON phb_cvtTon(T_ACI_TOA_TON ton)
{
    switch (ton)
	{
    case TON_NotPresent:
    case TON_Unknown:           return MFW_TON_UNKNOWN;
    case TON_International:     return MFW_TON_INTERNATIONAL;
    case TON_National:          return MFW_TON_NATIONAL;
    case TON_NetSpecific:       return MFW_TON_NET_SPECIFIC;
    case TON_DedAccess:         return MFW_TON_DED_ACCESS;
    case TON_Alphanumeric:      return MFW_TON_ALPHA_NUMERIC;
    case TON_Abbreviated:       return MFW_TON_ABBREVIATED;
    case TON_Extended:          return MFW_TON_EXTENDED;
		default:					          return (T_MFW_PHB_TON)ton;
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB              |
| STATE  : code                         ROUTINE: phb_ncvtTon         |
+--------------------------------------------------------------------+


   PURPOSE :  This function is used to convert the type of number
              used by MFW to the type of number used by ACI.

*/

T_ACI_TOA_TON phb_ncvtTon(T_MFW_PHB_TON ton)
{
    switch (ton)
	{
		case MFW_TON_INTERNATIONAL:     return TON_International;
		case MFW_TON_NATIONAL:          return TON_National;
    case MFW_TON_NET_SPECIFIC:      return TON_NetSpecific;
    case MFW_TON_DED_ACCESS:        return TON_DedAccess;
    case MFW_TON_ALPHA_NUMERIC:     return TON_Alphanumeric;
    case MFW_TON_ABBREVIATED:       return TON_Abbreviated;
    case MFW_TON_EXTENDED:          return TON_Extended;
		default:					              return TON_Unknown;
	}
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB              |
| STATE  : code                         ROUTINE: phb_cvtNpi          |
+--------------------------------------------------------------------+


   PURPOSE :  This function is used to convert the numbering plan
              identifier used by ACI to the numbering plan identifier
              used by MFW.

*/
/*a0393213 compiler warnings removal - changed the return type from T_MFW_PHB_TON to T_MFW_PHB_NPI*/
T_MFW_PHB_NPI phb_cvtNpi(T_ACI_TOA_NPI npi)
{
    switch (npi)
	{
    case NPI_NotPresent:
    case NPI_Unknown:       return MFW_NPI_UNKNOWN;
    case NPI_IsdnTelephony: return MFW_NPI_ISDN;
    case NPI_Data:          return MFW_NPI_DATA;
    case NPI_Telex:         return MFW_NPI_TELEX;
    case NPI_Private:       return MFW_NPI_PRIVATE;
    case NPI_National:      return MFW_NPI_NATIONAL;
#ifdef FF_2TO1_PS
    case NPI_Ermes:         return MFW_NPI_M_ERMES;
    case NPI_Cts:           return MFW_NPI_M_CTS;
#else                            
    case NPI_ERMES:         return MFW_NPI_M_ERMES;
    case NPI_CTS:           return MFW_NPI_M_CTS;
#endif                            
		default: return MFW_NPI_UNKNOWN;/*a0393213 warnings removal-npi changed to MFW_NPI_UNKNOWN*/
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB              |
| STATE  : code                         ROUTINE: phb_ncvtNpi         |
+--------------------------------------------------------------------+


   PURPOSE :  This function is used to convert the numbering plan
              identifier used by MFW to the numbering plan identifier
              used by ACI.

*/
/*a0393213 compiler warnings removal - changed the argument from T_MFW_PHB_TON to T_MFW_PHB_NPI*/
T_ACI_TOA_NPI phb_ncvtNpi(T_MFW_PHB_NPI npi)
{
    switch (npi)
	{
		case MFW_NPI_ISDN:          return NPI_IsdnTelephony;
    case MFW_NPI_DATA:          return NPI_Data;
    case MFW_NPI_TELEX:         return NPI_Telex;
    case MFW_NPI_PRIVATE:       return NPI_Private;
    case MFW_NPI_NATIONAL:      return NPI_National;
#ifdef FF_2TO1_PS
    case MFW_NPI_M_ERMES:       return NPI_Ermes;
    case MFW_NPI_M_CTS:         return NPI_Cts;
#else
    case MFW_NPI_M_ERMES:       return NPI_ERMES;
    case MFW_NPI_M_CTS:         return NPI_CTS;
#endif
		default:				        return NPI_Unknown;
	}
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB              |
| STATE  : code                         ROUTINE: phb_cvtService      |
+--------------------------------------------------------------------+


   PURPOSE :  This function is used to convert the service related to
              the phone number used by ACI to the service related to
              the phone numberused by MFW.

*/

T_MFW_PHB_SERVICE phb_cvtService(T_ACI_CNUM_SERV service)
{
    switch (service)
	{
		case CNUM_SERV_Asynch:          return SERVICE_ASYNCH;
		case CNUM_SERV_Synch:           return SERVICE_SYNCH;
        case CNUM_SERV_PadAsynch:       return SERVICE_PAD_ASYNCH;
		case CNUM_SERV_PacketSynch:     return SERVICE_PACKET_SYNCH;
        case CNUM_SERV_Voice:           return SERVICE_VOICE;
        case CNUM_SERV_Fax:             return SERVICE_FAX;
		default:				        return SERVICE_UNKNOWN;
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB              |
| STATE  : code                         ROUTINE: phb_init            |
+--------------------------------------------------------------------+


   PURPOSE :

*/

void phb_init()
{
  TRACE_FUNCTION ("phb_init()");
  
  return;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_PHB            |
| STATE   : code                        ROUTINE : phb_read_entries   |
+--------------------------------------------------------------------+

  PURPOSE : Read phonebook entries

*/
T_MFW phb_read_entries(UBYTE book,
                       UBYTE index,
                       UBYTE mode,
                       UBYTE num_entries,
                       T_MFW_PHB_LIST *entries)
{
  SHORT           start_ind;
  SHORT           stop_ind;
  SHORT           last_ind;
  UBYTE           rcd_num;
  T_ACI_SR_TYP sr_type; /*a0393213 warnings removal-type of sr_type changed from UBYTE*/
  SHORT           i;
  UBYTE           x;
//  Apr 14, 2005	REF: CRR 29991   xpradipg
//	Define the local pointer since it occupies close to 650 bytes
#ifdef FF_MMI_OPTIM
  T_ACI_PB_ENTR *pb_list;
#endif  
  TRACE_FUNCTION ("phb_read_entries()");
  TRACE_EVENT_P4("book%d index%d mode%d num%d", book, index, mode, num_entries);

  /* SPR#1112 - SH - Internal phonebook */
#ifdef INT_PHONEBOOK
  if (book EQ PHB_IPB)
  {
    return GI_pb_ReadRecList(mode, index, num_entries, entries);
  }
#endif

  /* check type of phonebook */
  if (book NEQ PHB_ECC
      AND book NEQ PHB_ADN
      AND book NEQ PHB_FDN
      AND book NEQ PHB_BDN
      AND book NEQ PHB_SDN
      AND book NEQ PHB_LRN
      AND book NEQ PHB_LDN
      AND book NEQ PHB_LMN
      AND book NEQ PHB_ADN_FDN
      AND book NEQ PHB_UPN)
    return MFW_PHB_FAIL;

  if ((book EQ PHB_LRN
       OR book EQ PHB_LDN
       OR book EQ PHB_LMN)
      AND (mode EQ MFW_PHB_NUMBER
           OR mode EQ MFW_PHB_ALPHA))
    return MFW_PHB_FAIL;

  if (book EQ PHB_ECC
      AND (mode EQ MFW_PHB_NUMBER
           OR mode EQ MFW_PHB_ALPHA))
    return MFW_PHB_FAIL;

  if ((book EQ PHB_ADN_FDN)
      AND (mode EQ MFW_PHB_INDEX))
    return MFW_PHB_FAIL;

  /* check index */
  if (index <= 0)
    return MFW_PHB_FAIL;

  switch (mode)
  {
    case MFW_PHB_INDEX:
      sr_type = SR_TYP_Index;
      break;

    case MFW_PHB_NUMBER:
      sr_type = SR_TYP_Number;
      break;

    case MFW_PHB_ALPHA:
      sr_type = SR_TYP_Name;
      break;

    case MFW_PHB_PHYSICAL:
      sr_type = SR_TYP_Physical;
      break;

    default:
      return MFW_PHB_FAIL;
  }

  /* select phonebook */
  if (sAT_PlusCPBS(CMD_SRC_LCL, phb_codePhbType ((T_MFW_PHB_TYPE) book ), NULL) NEQ AT_CMPL)
  return MFW_PHB_FAIL;

//     March 1 ,2004   REF: CRR 27832   x0012852 
//     Bug:SIM PIN2 related Actions are displying wrong Error Response.
//     Fix:MFW calls sAT_PlusCPBS() with third parameter as NULL.
    
     /* Read entries */
  entries->book = book;
  rcd_num = 0;
  start_ind = index;
  stop_ind  = index + (num_entries - 1);
//  Apr 14, 2005	REF: CRR 29991   xpradipg
//	dynamic allocation
	PB_LIST_ALLOC(pb_list,PB_LIST_SIZE);	
  while (num_entries > 0)
  {
    for (i=0; i<PHB_MAX_ENTRY_NUM; i++)
      pb_list[i].index = -1;

    if (cmhPHB_PlusCPBR(CMD_SRC_LCL,
                        sr_type,
                        start_ind,
                        stop_ind,
                        &last_ind,
                        pb_list) EQ AT_CMPL)
    {
      x = 0;
      for (i=start_ind; i<=last_ind; i++)
      {
        if (pb_list[x].index <= 0)
          break;

        memset(&entries->entry[rcd_num].date, 0, sizeof(entries->entry[rcd_num].date));
        memset(&entries->entry[rcd_num].time, 0, sizeof(entries->entry[rcd_num].time));
        if (book EQ PHB_LRN
            OR book EQ PHB_LDN
            OR book EQ PHB_LMN)
        {
            memcpy(entries->entry[rcd_num].date.year, pb_list[x].dateTime.year,  PHB_MAX_DIGITS);
            memcpy(entries->entry[rcd_num].date.month, pb_list[x].dateTime.month, PHB_MAX_DIGITS);
            memcpy(entries->entry[rcd_num].date.day, pb_list[x].dateTime.day, PHB_MAX_DIGITS);
            memcpy(entries->entry[rcd_num].time.hour, pb_list[x].dateTime.hour, PHB_MAX_DIGITS);
            memcpy(entries->entry[rcd_num].time.minute, pb_list[x].dateTime.minute, PHB_MAX_DIGITS);
            memcpy(entries->entry[rcd_num].time.second, pb_list[x].dateTime.second, PHB_MAX_DIGITS);
            memcpy(&entries->entry[rcd_num].line, &pb_list[x].line, sizeof(UBYTE));
        }
        entries->entry[rcd_num].book = phb_decodePhbType (pb_list[x].book);
        entries->entry[rcd_num].index = (UBYTE)pb_list[x].index;


// PATCH MAK 29/09/00. Put in because long number were overwriting the index therefore they were unable to be deleted
		strncpy((char *)entries->entry[rcd_num].number, (char *)pb_list[x].number, (PHB_MAX_LEN-1));
// END PATCH MAK 29/09/00


#ifdef NO_ASCIIZ
        memcpy(entries->entry[rcd_num].name.data, pb_list[x].text.data, pb_list[x].text.len);
        entries->entry[rcd_num].name.len = pb_list[x].text.len;
#else
		if (entries->entry[rcd_num].name[0] == 0x80 || entries->entry[rcd_num].name[0] == 0x00)
        { 	/*MC, SPR 1257 PHB_MAX_LEN is 41 when phonebook extension on, ACI only stores 21 char*/
        	memcpy(entries->entry[rcd_num].name, pb_list[x].text.data, /*PHB_MAX_LEN*/MAX_ALPHA_LEN);
		}
		else
        phb_Alpha2Gsm(&pb_list[x].text, entries->entry[rcd_num].name);
#endif
        entries->entry[rcd_num].ton = phb_cvtTon(pb_list[x].type.ton);
        entries->entry[rcd_num].npi = phb_cvtNpi(pb_list[x].type.npi);
        rcd_num++;
        x++;
      }

      if (num_entries <= PHB_MAX_ENTRY_NUM OR pb_list[x].index EQ -1)
        num_entries = 0;
      else
        num_entries -= PHB_MAX_ENTRY_NUM;

      start_ind = last_ind + 1;
    }

    else
    {
		//   Apr 14, 2005	REF: CRR 29991   xpradipg
		//	deallocate memory before returning
		PB_LIST_DEALLOC(pb_list,PB_LIST_SIZE);
    	return MFW_PHB_FAIL;
     }
  }

  if (rcd_num)
  {
    entries->result = MFW_ENTRY_EXIST;
    entries->num_entries = rcd_num;
  }
  else
  {
    entries->result = MFW_NO_ENTRY;
  }
//   Apr 14, 2005	REF: CRR 29991   xpradipg
//	deallocate memory
	PB_LIST_DEALLOC(pb_list,PB_LIST_SIZE);
 	return MFW_PHB_OK;
}



/* SPR#1727 - DS - Merged updated version from 1.6 branch. */

/* Test that the last 6 digits match. 
If there are less than 6 numbers - its probably not a phone number.
GW- For emergency calls check a full exact match for the full (but short) length
*/
int numbersMatch(char * num1, char* num2)
{
	int j;
	int len1,len2;
	int nDigits = 6;
	len1 = strlen(num1);
	len2 = strlen(num2);

	//If the strings are the same size, test up to 6 characters.
	if (len1 == len2)
		nDigits = len1;

	//Test at least 2 digits and at most 6.
	if ((nDigits >6 ) || (nDigits<2))
		nDigits = 6;

	//Is either string too short ?
	if ((len1 < nDigits) || (len2 < nDigits))
		return (0);

	//Check last 'nDigit' characters
	for (j=0;j<nDigits;j++)
	{
		if (num1[len1-j] != num2[len2-j])
			return (0);
	}
	return (1);
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_PHB            |
| STATE   : code                        ROUTINE : phb_find_entries   |
+--------------------------------------------------------------------+

  PURPOSE : Find phonebook entries

*/

#ifdef NO_ASCIIZ
T_MFW phb_find_entries(UBYTE book,
                      SHORT *index,
                      UBYTE mode,
                      SHORT num_entries,
                      T_MFW_PHB_TEXT *search_pattern,
                      T_MFW_PHB_LIST *entries)
{
// ADDED BY RAVI - ACI CHANGES - 5-10-2005
#if (defined(FF_2TO1_PS) && BOARD != 61)
  UBYTE          found;
#else
  SHORT          found;
#endif
// END ADDITION - RAVI - 5-10-2005
  UBYTE          i;
  SHORT          rcd_num;
  T_ACI_CPBF_MOD      search_mode;/*a0393213 warnings removal-type of search_mode changed from UBYTE*/
  T_ACI_SR_TYP          sr_type;/*a0393213 warnings removal-type of sr_type changed from UBYTE*/
  UBYTE          sum;
  T_ACI_PB_TEXT  findText;
 /*SPR2102, removed dbg variable*/
//   Apr 14, 2005	REF: CRR 29991   xpradipg
//	local pointer definition
#ifdef FF_MMI_OPTIM
	T_ACI_PB_ENTR *pb_list;
#endif	
  TRACE_FUNCTION ("phb_find_entries()");

   /* SPR#1112 - SH - Internal phonebook */
#ifdef INT_PHONEBOOK
  if (book EQ PHB_IPB)
  {
    return GI_pb_Find(mode, (UBYTE *)index, num_entries, search_pattern, entries);
  }
#endif

  if (book NEQ ECC
      AND book NEQ PHB_ADN
      AND book NEQ PHB_FDN
      AND book NEQ PHB_BDN
      AND book NEQ PHB_SDN
      AND book NEQ PHB_ADN_FDN
      AND book NEQ PHB_UPN)
    return MFW_PHB_FAIL;

  if ((book EQ PHB_ADN_FDN)
      AND (mode EQ MFW_PHB_INDEX))
    return MFW_PHB_FAIL;

  switch (mode)
  {
    case MFW_PHB_INDEX:
      sr_type = SR_TYP_Index;
      break;

    case MFW_PHB_NUMBER:
      sr_type = SR_TYP_Number;
      break;

    case MFW_PHB_ALPHA:
      if (!search_pattern->len)
        return PHB_FAIL;
      sr_type = SR_TYP_Name;
      break;

    case MFW_PHB_PHYSICAL:
      sr_type = SR_TYP_Physical;
      break;

    default:
      return MFW_PHB_FAIL;
  }

  /* select phonebook */
  if (sAT_PlusCPBS(CMD_SRC_LCL, phb_codePhbType ((T_MFW_PHB_TYPE) book ), NULL) NEQ AT_CMPL)
  return MFW_PHB_FAIL;
       
//   March 1 ,2004   REF: CRR 27832   x0012852 
//	Bug:SIM PIN2 related Actions are displying wrong Error Response.
//	Fix:MFW calls sAT_PlusCPBS() with third parameter as NULL.


  TRACE_FUNCTION ("phb_find_entries()-got phonebook");

  entries->book = book;
  rcd_num = 0;
  sum     = num_entries;
  search_mode = CPBF_MOD_NewSearch;
//   Apr 14, 2005	REF: CRR 29991   xpradipg
//	allocate memory
	PB_LIST_ALLOC(pb_list, PB_LIST_SIZE);
  while (num_entries > 0)
  {
    for (i=0; i<PHB_MAX_ENTRY_NUM; i++)
      pb_list[i].index = -1;

    if (mode EQ MFW_PHB_ALPHA)
    {
      findText.len = MINIMUM(search_pattern->len, MAX_ALPHA_LEN);
      memcpy(findText.data, search_pattern->data, findText.len);
    }
    else
    {
      findText.len = MINIMUM(search_pattern->len,  MAX_PHB_NUM_LEN-1); //GW -SPR#762 
      strncpy((char *)findText.data, (char *)search_pattern->data, findText.len);
      findText.data[findText.len] = '\0';
    }
    findText.cs = CS_Sim;
    if (cmhPHB_PlusCPBF(CMD_SRC_LCL,
                        &findText,
                        sr_type,
                        search_mode,
/*ADDED BY RAVI - ACI CHANGES - 5-10-2005*/                        
#if (defined(FF_2TO1_PS) && BOARD != 61)
                        (UBYTE *)index,
#else
                        index,
#endif                        
/*END ADDITION - RAVI - 5-10-2005*/
                        &found,
                        pb_list) EQ AT_CMPL)
    {
   /*SPR 2102, removed traces which were causing a crash for long phone numbers*/
      for (i=0; i<PHB_MAX_ENTRY_NUM; i++)
      {
        if ((pb_list[i].index != -1) && (rcd_num < sum))
        {
  TRACE_FUNCTION ("phb_find_entries()-found entry");
/*SPR 2102, removed traces which were causing a crash for long phone numbers*/
  
	        if (numbersMatch((char*)pb_list[i].number,(char*)search_pattern->data))
	        {
  TRACE_FUNCTION ("phb_find_entries()-numbers match");
	          entries->entry[rcd_num].book = phb_decodePhbType (pb_list[i].book);
	          entries->entry[rcd_num].index = (UBYTE)pb_list[i].index;
	          strcpy((char *)entries->entry[rcd_num].number, (char *)pb_list[i].number);
	          memcpy(entries->entry[rcd_num].name.data, pb_list[i].text.data, pb_list[i].text.len);
	          entries->entry[rcd_num].name.len = pb_list[i].text.len;
	          entries->entry[i].ton = phb_cvtTon(pb_list[rcd_num].type.ton);
	          entries->entry[i].npi = phb_cvtNpi(pb_list[rcd_num].type.npi);
	          rcd_num++;
	        }
        }
        else
          break;
      }

      if (num_entries <= PHB_MAX_ENTRY_NUM)
          num_entries = 0;
        else
          num_entries = num_entries - PHB_MAX_ENTRY_NUM;

      if (search_mode EQ CPBF_MOD_NewSearch)
      {
#ifdef FF_2TO1_PS
        entries->num_entries = (U16)found;
#else
        entries->num_entries = found;
#endif        
        search_mode = CPBF_MOD_NextSearch;
      }
    }
    else
      break;
  }

  if (rcd_num)
  {
    entries->result = MFW_ENTRY_EXIST;
  }
  else
  {
  	entries->result = MFW_NO_ENTRY;

  }

  /*SPR#1727 - DS - Use rcd_num variable from the for loop (that added entries to the entry structure) to set the number of entries.
   *                        "found" variable should not be used because it may be corrupted by the ACI and set to a false value if there
   *                         are no matching entries in the phoneook.
   */
  entries->num_entries = rcd_num;
//   Apr 14, 2005	REF: CRR 29991   xpradipg
//	deallocate memory
	PB_LIST_DEALLOC(pb_list,PB_LIST_SIZE);
  return MFW_PHB_OK;
}

#else
T_MFW phb_find_entries(UBYTE book,
                      SHORT *index,
                      UBYTE mode,
                      SHORT num_entries,
                      CHAR *search_pattern,
                      T_MFW_PHB_LIST *entries)
{
// ADDED BY RAVI - ACI CHANGES - 5-10-2005
#if (defined(FF_2TO1_PS) && BOARD != 61)
  UBYTE          found;
#else
  SHORT          found;
#endif
// END ADDITION - RAVI-5-10-2005
  UBYTE          i;
  UBYTE          rcd_num;
  UBYTE          search_mode;
  UBYTE          sr_type;
  UBYTE          sum;
  T_ACI_PB_TEXT  findText;

  TRACE_FUNCTION ("phb_find_entries()");

  if (book NEQ ECC
      AND book NEQ PHB_ADN
      AND book NEQ PHB_FDN
      AND book NEQ PHB_BDN
      AND book NEQ PHB_SDN
      AND book NEQ PHB_ADN_FDN
      AND book NEQ PHB_UPN)
    return MFW_PHB_FAIL;

  if ((book EQ PHB_ADN_FDN)
      AND (mode EQ MFW_PHB_INDEX))
    return MFW_PHB_FAIL;

  switch (mode)
  {
    case MFW_PHB_INDEX:
      sr_type = SR_TYP_Index;
      break;

    case MFW_PHB_NUMBER:
      sr_type = SR_TYP_Number;
      break;

    case MFW_PHB_ALPHA:
      sr_type = SR_TYP_Name;
      break;

    case MFW_PHB_PHYSICAL:
      sr_type = SR_TYP_Physical;
      break;

    default:
      return MFW_PHB_FAIL;
  }

  /* select phonebook */
  if (sAT_PlusCPBS(CMD_SRC_LCL, phb_codePhbType ( (T_MFW_PHB_TYPE)book ), NULL) NEQ AT_CMPL)
  return MFW_PHB_FAIL;
  
//   March 1 ,2004   REF: CRR 27832   x0012852 
//	Bug:SIM PIN2 related Actions are displying wrong Error Response.
//	Fix:MFW calls sAT_PlusCPBS() with third parameter as NULL.
         
  entries->book = book;
  rcd_num = 0;
  sum     = num_entries;
  search_mode = CPBF_MOD_NewSearch;

  while (num_entries > 0)
  {
    for (i=0; i<PHB_MAX_ENTRY_NUM; i++)
      pb_list[i].index = -1;

    findText.len = MINIMUM(strlen(search_pattern), PHB_MAX_LEN-1); //GW -SPR#762 - was MAX_ALPHA_LEN-1);
    strncpy((char *)findText.data, search_pattern, findText.len);
    findText.data[findText.len] = '\0';
    findText.cs = CS_Sim;

    if (cmhPHB_PlusCPBF(CMD_SRC_LCL,
                        &findText,
                        sr_type,
                        search_mode,
/*ADDED BY RAVI - ACI CHANGES - 5-10-2005 */                        
#if (defined(FF_2TO1_PS) && BOARD != 61)
                        (UBYTE *)index,
#else
                        index,
#endif                        
/*END ADDITION - RAVI - 5-10-2005 */
                        &found,
                        pb_list) EQ AT_CMPL)
    {
      for (i=0; i<PHB_MAX_ENTRY_NUM; i++)
      {
        if (pb_list[i].index NEQ -1 AND rcd_num < sum)
        {
          entries->entry[rcd_num].book = phb_decodePhbType (pb_list[i].book);
          entries->entry[rcd_num].index = (UBYTE)pb_list[i].index;
          strcpy((char *)entries->entry[rcd_num].number, (char *)pb_list[i].number);
          phb_Alpha2Gsm(&pb_list[i].text, entries->entry[rcd_num].name);
          entries->entry[i].ton = phb_cvtTon(pb_list[rcd_num].type.ton);
          entries->entry[i].npi = phb_cvtNpi(pb_list[rcd_num].type.npi);
          rcd_num++;
        }
        else
          break;
      }

      if (num_entries <= PHB_MAX_ENTRY_NUM)
          num_entries = 0;
        else
          num_entries = num_entries - PHB_MAX_ENTRY_NUM;

      if (search_mode EQ CPBF_MOD_NewSearch)
      {
#ifdef FF_2TO1_PS
        entries->num_entries = (U16)found;
#else
        entries->num_entries = found;
#endif        
        search_mode = CPBF_MOD_NextSearch;
      }
    }
    else
      break;
  }

  if (rcd_num)
    entries->result = MFW_ENTRY_EXIST;
  else
    entries->result = MFW_NO_ENTRY;

  /*SPR#1727 - DS - Use rcd_num variable from the for loop (that added entries to the entry structure) to set the number of entries.
   *                        "found" variable should not be used because it may be corrupted by the ACI and set to a false value if there
   *                         are no matching entries in the phoneook.
   */
  entries->num_entries = rcd_num;

  return MFW_PHB_OK;
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_PHB            |
| STATE   : code                        ROUTINE : phb_store_entry    |
+--------------------------------------------------------------------+

  PURPOSE : Store phonebook entry

*/

T_MFW phb_store_entry(UBYTE book,
                      T_MFW_PHB_ENTRY *entry,
                      T_MFW_PHB_STATUS *status)
{
// ADDED BY RAVI - ACI CHANGES - 5-10-2005
#if (defined(FF_2TO1_PS) && BOARD != 61)
  UBYTE            used;
  UBYTE            total;
  UBYTE			   avail;
#else
  SHORT            used;
  SHORT            total;
  SHORT			   avail;
#endif  
// END ADDITION - RAVI - 5-10-2005
#ifdef TI_PS_FFS_PHB
  UBYTE            nlength;
#endif
  SHORT            used_ext;
  SHORT            total_ext;
  UBYTE            tlength;
#ifndef TI_PS_FFS_PHB
  UBYTE            service;
#endif
  T_ACI_VP_ABS     dt;
  T_ACI_PB_TEXT    text;
  T_ACI_TOA        toa;
  T_ACI_RETURN phb_result;

//jgg
  TRACE_EVENT("jgg-phb_store_entry");
//jgg

 /* SPR#1112 - SH - Internal phonebook */
#ifdef INT_PHONEBOOK
  if (book EQ PHB_IPB)
  {
  	T_MFW ret;
    ret = GI_pb_WriteRec(entry);
    GI_pb_Info(status);
    return ret;
  }
#endif

  /* check type of phonebook */
  if (book NEQ PHB_ECC
      AND book NEQ PHB_ADN
      AND book NEQ PHB_FDN
      AND book NEQ PHB_BDN
      AND book NEQ PHB_SDN
      AND book NEQ PHB_LRN
      AND book NEQ PHB_LDN
      AND book NEQ PHB_LMN
      AND book NEQ PHB_UPN)
    return MFW_PHB_FAIL;

  /* select phonebook */
  if (sAT_PlusCPBS(CMD_SRC_LCL, phb_codePhbType ((T_MFW_PHB_TYPE) book ), NULL) NEQ AT_CMPL)
  return MFW_PHB_FAIL;
     
//    March 1 ,2004   REF: CRR 27832   x0012852 
//    Bug:SIM PIN2 related Actions are displying wrong Error Response.
//    Fix:MFW calls sAT_PlusCPBS() with third parameter as NULL.

  memset(&dt, 0, sizeof(T_ACI_VP_ABS));
  if (book EQ PHB_LDN
      OR book EQ PHB_LRN
      OR book EQ PHB_LMN)
  {
      memcpy(dt.year, entry->date.year, PHB_MAX_DIGITS);
      memcpy(dt.month, entry->date.month, PHB_MAX_DIGITS);
      memcpy(dt.day, entry->date.day, PHB_MAX_DIGITS);
      memcpy(dt.hour, entry->time.hour, PHB_MAX_DIGITS);
      memcpy(dt.minute, entry->time.minute, PHB_MAX_DIGITS);
      memcpy(dt.second, entry->time.second, PHB_MAX_DIGITS);
  }

  // Oct 27, 2005	REF: OMAPS 48881 b-nekkare
  if (entry->number[0] EQ '+' )
    toa.ton=TON_International;
  else
    toa.ton=TON_Unknown;

  toa.npi=NPI_IsdnTelephony;

#ifdef NO_ASCIIZ
  text.len = entry->name.len;
  memcpy(text.data, entry->name.data, text.len);
  text.cs = CS_Sim;
#else
  phb_Gsm2Alpha(entry->name, &text);
#endif
// July  17, 2006  DR: OMAPS00082792  x0039928
phb_result = sAT_PlusCPBW(CMD_SRC_LCL,
                   (SHORT)(entry->index EQ 0 ? ACI_NumParmNotPresent : entry->index),
                   (char *)entry->number,
                   &toa,
                   &text,
                   &dt);
  switch(phb_result)
  {
  case AT_CMPL:
#if !defined(TI_PS_FFS_PHB)
    pb_read_status(book, &service, &total, &used, &tlength, &avail,&used_ext, &total_ext);
#else
    (void)pb_read_sizes ((T_PHB_TYPE)book, &total, &used, &nlength, &tlength,  /*a0393213 warnings removal-explicit typecasting done*/
                         &used_ext, &total_ext);  /*a0393213 OMAPS00097209 - interface of pb_read_sizes has been changed in ACI side*/
    avail = total - used;
#endif

    status->book = book;
    status->tag_len = tlength;
#ifdef FF_2TO1_PS
    status->max_entries = (U16)total;
    status->used_entries = (U16)used;
    status->avail_entries = (U16)avail;
#else
    status->max_entries = total;
    status->used_entries = used;
    status->avail_entries = avail;
#endif    
//x0061560 Date May17,07 (sateeshg) OMAPS00133241 
return MFW_PHB_OK;
case AT_EXCT:
#ifndef TI_PS_FFS_PHB // Flushing not necessary with FFS based SIM PHB
 if (book EQ PHB_LDN
      OR book EQ PHB_LRN
      OR book EQ PHB_LMN)
  {

	pb_write_eeprom();
  }
#endif

//x0061560 Date May16,07 (sateeshg) OMAPS00133241 
return MFW_PHB_EXCT;

 default:
    return MFW_PHB_FAIL;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_PHB            |
| STATE   : code                        ROUTINE : phb_delete_entry   |
+--------------------------------------------------------------------+

  PURPOSE : Delete phonebook entry

*/

T_MFW phb_delete_entry(UBYTE book, UBYTE index, T_MFW_PHB_STATUS *status)
{
// ADDED BY RAVI - ACI CHANGES - 5-10-2005
#if (defined(FF_2TO1_PS) &&  BOARD != 61)
  UBYTE            used;
  UBYTE            total;
  UBYTE			   avail;
#else
  SHORT            used;
  SHORT            total;
  SHORT			   avail;
#endif  
// END ADDITION BY RAVI - 5-10-2005
#ifdef TI_PS_FFS_PHB
  UBYTE            nlength;
#endif
  SHORT            used_ext;
  SHORT            total_ext;
  UBYTE            tlength;
#ifndef TI_PS_FFS_PHB
  UBYTE			   service;
#endif
  T_ACI_VP_ABS     dt;
  T_ACI_RETURN phb_result;

  TRACE_FUNCTION("phb_delete_entry()");

 /* SPR#1112 - SH - Internal phonebook */
#ifdef INT_PHONEBOOK
  if (book EQ PHB_IPB)
  {
  	T_MFW ret;

    ret = GI_pb_DeleteRec(index);
    GI_pb_Info(status);

    return ret;
  }
#endif

  /* check type of phonebook */
  if (book NEQ PHB_ECC
      AND book NEQ PHB_LDN
      AND book NEQ PHB_LRN
      AND book NEQ PHB_LMN
      AND book NEQ PHB_ADN
      AND book NEQ PHB_FDN
      AND book NEQ PHB_BDN
      AND book NEQ PHB_SDN
      AND book NEQ PHB_UPN)
    return MFW_PHB_FAIL;

  /* select phonebook */
  if (sAT_PlusCPBS(CMD_SRC_LCL, phb_codePhbType ( (T_MFW_PHB_TYPE)book ), NULL) NEQ AT_CMPL)
  return MFW_PHB_FAIL;

//    March 1 ,2004   REF: CRR 27832   x0012852 
//    Bug:SIM PIN2 related Actions are displying wrong Error Response.
//    Fix:MFW calls sAT_PlusCPBS() with third parameter as NULL.
   

  memset(&dt, 0, sizeof(T_ACI_VP_ABS));

//July  17, 2006  DR: OMAPS00082792  x0039928
  phb_result = sAT_PlusCPBW(CMD_SRC_LCL, index, NULL, NULL, NULL, &dt);

  switch(phb_result)
  {
  case AT_CMPL:
#if !defined(TI_PS_FFS_PHB)
    pb_read_status(book, &service, &total, &used, &tlength, &avail,&used_ext, &total_ext);
#else
    (void)pb_read_sizes ((T_PHB_TYPE)book, &total, &used, &nlength, &tlength,        /*a0393213 warnings removal-explicit typecasting done*/
                         &used_ext, &total_ext); /*a0393213 OMAPS00097209 - interface of pb_read_sizes has been changed in ACI side*/
    avail = total - used;
#endif

    status->book = book;
    status->tag_len = tlength;
#ifdef FF_2TO1_PS
    status->max_entries = (U16)total;
    status->used_entries = (U16)used;
    status->avail_entries = (U16)avail;
#else
    status->max_entries = total;
    status->used_entries = used;
    status->avail_entries = avail;
#endif    
    case AT_EXCT:
 	return MFW_PHB_OK;
 default:
    return MFW_PHB_FAIL;
  }
}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_PHB            |
| STATE   : code                        ROUTINE : phb_set_mode       |
+--------------------------------------------------------------------+

  PURPOSE : Set the unrestricted/restricted mode for FDN or BDN

*/

T_MFW phb_set_mode(UBYTE book, UBYTE *passwd)
{
  if (book EQ PHB_FDN)
  {
    /*a0393213 warnings removal - CLCK_FAC_Fd changed to FAC_Fd after ACI enum changes*/
    if (ss_set_clck(FAC_Fd, CLCK_MOD_Lock,
                   (char *)passwd, CLASS_VceDatFaxSms, MFW_PHB) != MFW_SS_OK)
    {
      return MFW_PHB_FAIL;
    }  
  }
  if (book EQ PHB_ADN)
  {
      /*a0393213 warnings removal - CLCK_FAC_Fd changed to FAC_Fd after ACI enum changes*/
    if (ss_set_clck(FAC_Fd, CLCK_MOD_Unlock,
                   (char *)passwd, CLASS_VceDatFaxSms, MFW_PHB) != MFW_SS_OK)
    {
      return MFW_PHB_FAIL;
    }
  }
  return MFW_PHB_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_PHB            |
| STATE   : code                        ROUTINE : phb_ok_clck        |
+--------------------------------------------------------------------+

  PURPOSE : successful switch for FDN

*/

void phb_ok_clck()
{
    T_MFW_PHB_STATUS status;

    memset(&status, 0, sizeof(T_MFW_PHB_STATUS));
    status.book = FDN;
    phb_get_status(&status);
    phb_signal(E_PHB_STATUS, &status);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_PHB            |
| STATE   : code                        ROUTINE : phb_error_clck     |
+--------------------------------------------------------------------+

  PURPOSE : unsuccessful switch for FDN or BDN

*/

void phb_error_clck()
{
    phb_signal(E_PHB_ERROR, 0);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_PHB            |
| STATE   : code                        ROUTINE : phb_check_number   |
+--------------------------------------------------------------------+

  PURPOSE : Check number against the restrictions for FDN or BDN

*/

T_MFW phb_check_number(UBYTE *number)
{
#if (defined(FF_2TO1_PS))
  UBYTE         found;
  UBYTE         index;
  T_PHB_RECORD  entry;
  T_ACI_PB_TEXT text;
#endif
  UBYTE         toa;
  T_ACI_TOA     ton_npi;
  int   i;
  int   len = strlen((char *)number);

  /* Search for '+' digit */
  for (i = 0; (i < len) && (number[i] != '+'); i++);

  /* '+' has been found */
  if (i != len)
  {
    ton_npi.ton = TON_International;

    /* Move characters of the input string to suppress the '+' */
    for (i++; i < len; i++)
      number[i-1] = number[i];
    number[i-1] = '\0';
  }
  else
    ton_npi.ton = TON_Unknown;

    ton_npi.npi = NPI_IsdnTelephony;
  cmhPHB_toaMrg ( &ton_npi, &toa );                      

/* --->>>--- */
#if defined(FF_2TO1_PS)
  if (pb_check_fdn(FDN, number, &found, &entry, toa) NEQ PHB_OK)
    return MFW_PHB_FAIL;
  if (found)
    return PHB_FDN;

#else
  /* The old pb_check_fdn() either delivered PHB_OK and found == 1 or 
   * PHB_FAIL and found == 0. Other combintations were not possibe, 
   * especially PHB_OK and found == 0. Some unreachable / dead code here.
   * Found on g23m S621 by HM 21-Sep-2005 */
  if (pb_check_fdn(toa, number) NEQ PHB_OK)
    return MFW_PHB_FAIL;
  return PHB_FDN;
#endif /* else, #if defined(FF_2TO1_PS) */
/* ---<<<--- */

#if defined(FF_2TO1_PS)
{
  T_ACI_PB_ENTR *pb_list;
  if (sAT_PlusCPBS(CMD_SRC_LCL, PB_STOR_Bd, NULL) NEQ AT_CMPL)
  return MFW_PHB_FAIL;
    
 
 //   March 1 ,2004   REF: CRR 27832   x0012852 SELIM
 //   Bug:SIM PIN2 related Actions are displying wrong Error Response.
 //   Fix:MFW calls sAT_PlusCPBS() with third parameter as NULL.
    
  text.len = MINIMUM(strlen((char *)number), PHB_MAX_LEN-1); //GW -SPR#762 - was MAX_ALPHA_LEN-1);
  strncpy((char *)text.data, (char *)number, text.len);
  text.data[text.len] = '\0';
  text.cs = CS_Sim;
//   Apr 14, 2005	REF: CRR 29991   xpradipg
//	allocate memory
	PB_LIST_ALLOC(pb_list, PB_LIST_SIZE);
  if (cmhPHB_PlusCPBF(CMD_SRC_LCL,
                      &text,
                      SR_TYP_Number,
                      CPBF_MOD_NewSearch, &index,
                      &found, pb_list) NEQ AT_CMPL)
    {
//   Apr 14, 2005	REF: CRR 29991   xpradipg
//	deallocate memory
	PB_LIST_DEALLOC(pb_list, PB_LIST_SIZE);
   	return MFW_PHB_FAIL;
    }
  if (found)
  {
//   Apr 14, 2005	REF: CRR 29991   xpradipg
//	deallocate memory
	PB_LIST_DEALLOC(pb_list, PB_LIST_SIZE);
    return PHB_BDN;
   }
//   Apr 14, 2005	REF: CRR 29991   xpradipg
//	deallocat memory
	PB_LIST_DEALLOC(pb_list, PB_LIST_SIZE);
  return PHB_OK;
}
#endif 

}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_PHB            |
| STATE   : code                        ROUTINE : phb_get_status     |
+--------------------------------------------------------------------+

  PURPOSE : Check number against the restrictions for FDN or BDN

*/

T_MFW phb_get_status(T_MFW_PHB_STATUS *status)
{
// ADDED BY RAVI - ACI CHANGES - 5-10-2005
#if (defined(FF_2TO1_PS) && BOARD != 61)
  UBYTE          used;
  UBYTE          total;
  UBYTE			 avail;
#else
  SHORT          used;
  SHORT          total;
  SHORT			 avail;
#endif  

// END ADDITION BY RAVI - 5-10-2005
  T_PHB_TYPE			 type;/*a0393213 warnings removal-data type of 'type' changed from UBYTE*/
#ifdef TI_PS_FFS_PHB
  UBYTE          num_len;
#endif
  SHORT            used_ext;
  SHORT            total_ext;
#ifndef TI_PS_FFS_PHB
  UBYTE          service;
#endif
  UBYTE          tag_len;

  TRACE_FUNCTION("phb_get_status()");

 /* SPR#1112 - SH - Internal phonebook */
#ifdef INT_PHONEBOOK
  if (status->book EQ PHB_IPB)
  {   
    GI_pb_Info(status);
    return MFW_PHB_OK;
  }
#endif

  switch (status->book)
  {
    case PHB_ECC:
      type = ECC;
      break;
    case PHB_ADN:
      type = ADN;
      break;
    case PHB_FDN:
      type = FDN;
      break;
#ifndef TI_PS_FFS_PHB
	case PHB_ADN_FDN:
      type = ADN_FDN;
      break;
#endif
    case PHB_BDN:
      type = BDN;
      break;
    case PHB_SDN:
      type = SDN;
      break;
    case PHB_LRN:
      type = LRN;
      break;
    case PHB_LDN:
      type = LDN;
      break;
    case PHB_LMN:
      type = LMN;
      break;
    case PHB_UPN:
      type = UPN;
      break;
	default:
	  return MFW_PHB_FAIL;
  }

#if !defined(TI_PS_FFS_PHB)
  if (pb_read_status(type, &service, &total, &used,
	                &tag_len, &avail,&used_ext, &total_ext) == PHB_FAIL)
	return MFW_PHB_FAIL;
#else
  if (pb_read_sizes (type, &total, &used, &num_len, &tag_len, 
                     &used_ext, &total_ext) == PHB_FAIL) /*a0393213 OMAPS00097209 - interface of pb_read_sizes has been changed in ACI side*/
    return MFW_PHB_FAIL;
  avail = total - used;
#endif

  status->tag_len      = tag_len;
#ifdef FF_2TO1_PS
  status->max_entries  = (U16)total;
  status->used_entries = (U16)used;
  status->avail_entries = (U16)avail;
#else
  status->max_entries  = total;
  status->used_entries = used;
  status->avail_entries = avail;
#endif  
  return MFW_PHB_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_PHB            |
| STATE   : code                        ROUTINE : phb_get_mode       |
+--------------------------------------------------------------------+

  PURPOSE : Request the status of FDN

*/

int phb_get_mode(void)
{
    T_ACI_CLSSTAT clsStat;

#ifdef TI_PS_FFS_PHB
    if (pb_stat == PB_STAT_Busy)
        return PHB_LOADING;

    if (pb_stat != PB_STAT_Ready)
        return MFW_PHB_FAIL;
#else
  UBYTE stat;

	pb_status_req(&stat);
	if (stat == PHB_BUSY)
	  return PHB_LOADING;

	if (stat != PHB_READY)
	  return MFW_PHB_FAIL;
#endif
/*a0393213 warnings removal - CLCK_FAC_Fd changed to FAC_Fd after ACI enum changes*/
    if (qAT_PlusCLCK(CMD_SRC_LCL, FAC_Fd,
                     CLASS_NotPresent, &clsStat) EQ AT_CMPL)
    {
        switch (clsStat.status)
        {
            case STATUS_NotActive:
                return PHB_UNRESTRICTED;

            case STATUS_Active:
                return PHB_RESTRICTED;

            case STATUS_NotPresent:
                return PHB_NOT_ALLOCATED;
        }
    }
    return MFW_PHB_FAIL;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_PHB            |
| STATE   : code                        ROUTINE : phb_delete_book    |
+--------------------------------------------------------------------+

  PURPOSE :

*/

T_MFW phb_delete_book(UBYTE book)
{
    T_MFW_PHB_STATUS status;
    T_MFW_PHB_ENTRY  entry;
    T_MFW_PHB_LIST   entries;
	T_MFW_PHB_RETURN v_returnedStatus = MFW_PHB_OK;
  T_PHB_RETURN phb_return;

	TRACE_FUNCTION("phb_delete_book()");

	if (book == PHB_ADN OR
		book == PHB_FDN OR
		book == PHB_BDN OR
		book == PHB_SDN OR
    book == PHB_UPN)

  {
#ifdef TI_PS_FFS_PHB
    /* 
     * When we're returning MFW_PHB_OK and the book is a SIM book 
     * normally we're not really finished with the operation here
     * as the synchronization to the SIM is still ongoing.
     */
    phb_return = pb_del_book((T_PHB_TYPE)book);/*a0393213 warnings removal-explicit typecasting done*/
    if ((phb_return EQ PHB_OK) OR (phb_return EQ PHB_EXCT))
#else
		if (pb_delete_book(book) == PHB_OK)
#endif
			return MFW_PHB_OK;
		else
			return MFW_PHB_FAIL;
	}

    entries.entry = &entry;
    status.book = book;
    phb_get_status(&status);

	/*
	 * PATCH CDB 12 04 2000 : This function has been modified to store the RAM copy of the
	 * LDN/LMN/LRN phonebook in E2PROM. As we had returns in the middle of the function,
	 * we have to make a record of each possible returned value of the various function,
	 * then return it. At the end, we return the copied returned value.
	 */
    while (status.used_entries)
    {
        if (phb_read_entries(book, 1, MFW_PHB_INDEX, 1, &entries) != MFW_PHB_OK)
        {
            v_returnedStatus = MFW_PHB_FAIL;
            break;
		}

        if (!entries.num_entries)
		{
			v_returnedStatus = MFW_PHB_OK;
			break;
		}

        if (phb_delete_entry(book, entries.entry->index, &status)!= MFW_PHB_OK)
		{
			v_returnedStatus = MFW_PHB_FAIL;
			break;
		}
    }

#ifndef TI_PS_FFS_PHB // Flushing not necessary with FFS based SIM PHB
    if ((book EQ PHB_LDN) OR (book EQ PHB_LRN) OR (book EQ PHB_LMN))
	{
	  // Don't forget to store it in the E2PROM !!!
	  pb_write_eeprom();
	}
#endif

    /*
	 * end of beautiful patch
	 */
    return (v_returnedStatus);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_PHB            |
| STATE   : code                        ROUTINE : phb_set_auto_off   |
+--------------------------------------------------------------------+

  PURPOSE :

*/

T_MFW phb_set_auto_off(UBYTE mode)
{
    if (mode == PHB_AUTO_ON)
    {
        if (sAT_PercentPBCF(CMD_SRC_LCL, PBCF_LDN_Enable,
                            PBCF_LRN_Enable, PBCF_LMN_Enable) NEQ AT_CMPL)
            return MFW_PHB_FAIL;
        return MFW_PHB_OK;
    }
    if (mode == PHB_AUTO_OFF)
    {
        if (sAT_PercentPBCF(CMD_SRC_LCL, PBCF_LDN_Disable,
                            PBCF_LRN_Disable, PBCF_LMN_Disable) != AT_CMPL)
            return MFW_PHB_FAIL;
        return MFW_PHB_OK;
    }
	return MFW_PHB_FAIL; /*a0393213 compiler warnings removal - added return statement*/
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_PHB            |
| STATE   : code                        ROUTINE : phb_get_auto_off   |
+--------------------------------------------------------------------+

  PURPOSE :

*/

T_MFW phb_get_auto_off(UBYTE *mode)
{
    T_ACI_PBCF_LDN ldn;
    T_ACI_PBCF_LRN lrn;
    T_ACI_PBCF_LMN lmn;
    if (qAT_PercentPBCF(CMD_SRC_LCL, &ldn, &lrn, &lmn) != AT_CMPL)
        return MFW_PHB_FAIL;

    switch (ldn)
    {
        case PBCF_LDN_Disable:
            *mode = PHB_AUTO_OFF;
            return MFW_PHB_OK;
        case PBCF_LDN_Enable:
            *mode = PHB_AUTO_ON;
            return MFW_PHB_OK;
        default:
            return MFW_PHB_FAIL;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_PHB            |
| STATE   : code                        ROUTINE : phb_switch_entry   |
+--------------------------------------------------------------------+

  PURPOSE : switch one entry between ADN and FDN

*/

T_MFW_PHB_RETURN phb_switch_entry  (T_MFW_PHB_ENTRY *entry, UBYTE *passwd)
{
    T_MFW_PHB_STATUS status;
    SHORT            pn1Cnt = -1;
    SHORT            pn2Cnt = -1;
    SHORT            pk1Cnt = -1;
    SHORT            pk2Cnt = -1;
    T_ACI_PVRF_STAT  ps1 = PVRF_STAT_NotPresent;
    T_ACI_PVRF_STAT  ps2 = PVRF_STAT_NotPresent;
 //   UBYTE			 avail = 0;  // RAVI
    UBYTE            index;

    qAT_PercentPVRF(CMD_SRC_LCL, &pn1Cnt,
                    &pn2Cnt, &pk1Cnt, &pk2Cnt, &ps1, &ps2);
    if (ps2 != PVRF_STAT_NotRequired)
    {
        if (pn2Cnt < 3)
            return MFW_PHB_PIN2_REQ;
        else
            return MFW_PHB_PUK2_REQ;
    }

    index = entry->index;
    entry->index = 0;

    switch (entry->book)
    {
        case PHB_FDN:
            if (phb_store_entry (PHB_ADN, entry, &status) == MFW_PHB_OK)
            {
                phb_delete_entry (PHB_FDN, index, &status);
                return MFW_PHB_OK;
            }
            break;
        case PHB_ADN:
            if (phb_store_entry (PHB_FDN, entry, &status) == MFW_PHB_OK)
            {
                phb_delete_entry (PHB_ADN, index, &status);
                return MFW_PHB_OK;
            }
            break;
        default:
            break;
    }
	return (MFW_PHB_FAIL);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB              |
| STATE  : code                         ROUTINE: phb_read_upn_eeprom |
+--------------------------------------------------------------------+


   PURPOSE :  Read user personal numbers from EEPROM

*/

void phb_read_upn_eeprom(void)
{

#ifdef PCM_2_FFS
 T_PSPDF_UPN efupn;
#else
    EF_UPN    efupn;
#endif

    USHORT    max_rcd;
    T_ACI_TOA type;
    UBYTE     version;
    UBYTE     i;

    TRACE_FUNCTION("phb_read_upn_eeprom()");

    /* Added to remove warning Aug - 11 */
    i = 0;

    if (upn_list.count)
    {
        phb_signal(E_PHB_UPN_LIST, &upn_list);
        return;
    }

#ifdef PCM_2_FFS
	if (ffs_ReadRecord((UBYTE *)PSPDF_UPN_ID, (UBYTE *)&efupn, 
		sizeof( T_PSPDF_UPN ) , (USHORT)(i+1), 1 ) ==  sizeof( T_PSPDF_UPN ) )
#else
    pcm_ReadRecord((UBYTE *)EF_UPN_ID,
				    1,
					SIZE_EF_UPN,
					(UBYTE *)&efupn,
					&version,
					&max_rcd);

#endif
    for (i=0; i<max_rcd; i++)
    {
#ifdef PCM_2_FFS
	if (ffs_ReadRecord((UBYTE *)PSPDF_UPN_ID, (UBYTE *)&efupn, 
		sizeof( T_PSPDF_UPN ) , (USHORT)(i+1), 1 ) ==  sizeof( T_PSPDF_UPN )  )
#else
	    if (pcm_ReadRecord((UBYTE *)EF_UPN_ID,
						   (USHORT)(i+1),
						   SIZE_EF_UPN,
						   (UBYTE *)&efupn,
						   &version,
						   &max_rcd) == DRV_OK)
#endif

        {
            if (efupn.usrNum[0] NEQ 0xff)
            {
                /* copy record */
#ifdef NO_ASCIIZ
/*MC, SPR1257 name strings should use MAX_ALPHA_LEN*/
                cmhPHB_getTagNt (efupn.alphId,
                                 10,            /* alpha length is 10 bytes in eeprom */
                                 (CHAR *)upn_list.upn[cnt].name.data,
                                 /*PHB_MAX_LEN*/MAX_ALPHA_LEN);
#else
/*MC, SPR1257 name strings should use MAX_ALPHA_LEN*/
                                cmhPHB_getTagNt (efupn.alphId,
                                 10,            /* alpha length is 10 bytes in eeprom */
                                 (CHAR *)upn_list.upn[cnt].name,
                                 /*PHB_MAX_LEN*/MAX_ALPHA_LEN);
#endif
                cmhPHB_getAdrStr ((CHAR *)upn_list.upn[cnt].number,
                                  PHB_MAX_LEN - 1,
                                  efupn.usrNum,
                                  efupn.len);

                cmhPHB_toaDmrg (efupn.numTp, &type);
                upn_list.upn[cnt].ton = phb_cvtTon(type.ton);
                upn_list.upn[cnt].npi = (T_MFW_PHB_NPI)phb_cvtNpi(type.npi);
                /* ccp */
                /* ext1 */
                cnt++;
                upn_list.count++;
            }
        }
    }
    phb_signal(E_PHB_UPN_LIST, &upn_list);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB              |
| STATE  : code                         ROUTINE: phb_get_upn         |
+--------------------------------------------------------------------+


   PURPOSE :  Request user personal numbers

*/

void phb_get_upn(void)
{
    /*a0393213 compiler warnings removal - removed variables phb_entry, phb_list*/
    
    T_MFW_PHB_STATUS status;
    int i;
    SHORT           start_ind;
    SHORT           stop_ind;
    SHORT           last_ind;
//   Apr 14, 2005	REF: CRR 29991   xpradipg
//	local pointer definition
#ifdef FF_MMI_OPTIM
	T_ACI_PB_ENTR *pb_list;
#endif	

    TRACE_FUNCTION("phb_get_upn()");

    

    memset(&status, 0, sizeof(status));
    status.book = PHB_UPN;
    phb_get_status(&status);

    upn_list.count = 0;
    /* select phonebook */
    if (sAT_PlusCPBS(CMD_SRC_LCL, phb_codePhbType ( PHB_UPN ), NULL) NEQ AT_CMPL)

//    March 1 ,2004   REF: CRR 27832   x0012852 
//    Bug:SIM PIN2 related Actions are displying wrong Error Response.
//    Fix:MFW calls sAT_PlusCPBS() with third parameter as NULL.
        
      phb_signal(E_PHB_UPN_LIST, &upn_list);

    start_ind = 1;
    stop_ind = 1;
    upn_list.count = 0;
//   Apr 14, 2005	REF: CRR 29991   xpradipg
//	allocate memory
#ifdef FF_MMI_OPTIM
	pb_list = (T_ACI_PB_ENTR*)mfwAlloc(PB_LIST_SIZE);
	if(NULL == pb_list)
		return;
#endif		
    for (i=0; i<status.used_entries; i++)
    {

      if (cmhPHB_PlusCPBR(CMD_SRC_LCL,
                          SR_TYP_Index,
                          start_ind,
                          stop_ind,
                          &last_ind,
                          pb_list) EQ AT_CMPL)
      {
        if (pb_list[0].index <= 0)
            break;

        strcpy((char *)upn_list.upn[i].number, (char *)pb_list[0].number);
#ifdef NO_ASCIIZ
        memcpy(upn_list.upn[i].name.data, pb_list[0].text.data, pb_list[0].text.len);
        upn_list.upn[i].name.len = pb_list[0].text.len;
#else
        phb_Alpha2Gsm(&pb_list[0].text, upn_list.upn[i].name);
#endif
        upn_list.upn[i].ton = phb_cvtTon(pb_list[0].type.ton);
        upn_list.upn[i].npi = (T_MFW_PHB_NPI)phb_cvtNpi(pb_list[0].type.npi);
        start_ind++;
        stop_ind++;
        upn_list.count++;
      }
    }

    phb_signal(E_PHB_UPN_LIST, &upn_list);
//   Apr 14, 2005	REF: CRR 29991   xpradipg
//	deallocate the memory
	PB_LIST_DEALLOC(pb_list, PB_LIST_SIZE);
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB              |
| STATE  : code                         ROUTINE: phb_save_upn        |
+--------------------------------------------------------------------+


   PURPOSE :  Save user personal numbers in EEPROM

*/

T_MFW phb_save_upn(T_MFW_UPN *upn)
{
    T_MFW_PHB_ENTRY entry;
    T_MFW_PHB_STATUS status;

    entry.book = PHB_UPN;
#ifdef NO_ASCIIZ
    memcpy(entry.name.data, upn->name.data, upn->name.len);
    entry.name.len = upn->name.len;
#endif
    strcpy((char *)entry.number, (char *)upn->number);
    // PATCH LE 12.05.00
    // store upn ever on first location
    // zero searches for a free record, which is not
    // available and then the operation fails !

    // old code entry.index = 0;
    entry.index = 1;
    // END PATCH LE 12.05.00
    entry.ton = MFW_TON_UNKNOWN;
    entry.npi = MFW_NPI_UNKNOWN;

    if (phb_store_entry(PHB_UPN, &entry, &status) EQ MFW_PHB_OK)
      return MFW_PHB_OK;
    return MFW_PHB_FAIL;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB              |
| STATE  : code                         ROUTINE: phb_Alpha2Gsm       |
+--------------------------------------------------------------------+


   PURPOSE :  convert alpha to GSM string (set bit 7 to 1)

*/

void phb_Alpha2Gsm(T_ACI_PB_TEXT *text, UBYTE *alpha)
{
	int i;

	for (i=0; i<text->len; i++)
	{
		if(i == (PHB_MAX_LEN - 1))
			break;
		alpha[i] = text->data[i] | 0x80;
	}
	alpha[i] = '\0';
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB              |
| STATE  : code                         ROUTINE: phb_Gsm2Alpha       |
+--------------------------------------------------------------------+


   PURPOSE :  convert zero-terminated GSM string to "data + len"
			  (set bit 7 to 0)

*/

void phb_Gsm2Alpha(UBYTE *alpha, T_ACI_PB_TEXT *text)
{
	int i;

	text->len = strlen((char *)alpha);
  for ( i = 0; i < text->len; i++ )
		alpha[i] = alpha[i] & 0x7F;
	memcpy(text->data, alpha, text->len);
	text->data[text->len] = '\0';
  text->cs = CS_Sim;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB              |
| STATE  : code                         ROUTINE: rAT_phb_status      |
+--------------------------------------------------------------------+


   PURPOSE :  reading phonebook from SIM card is ready.

*/
void rAT_phb_status (T_ACI_PB_STAT status)
{
TRACE_FUNCTION("rAT_phb_status()");
#ifdef TI_PS_FFS_PHB
  pb_stat = status;

  switch (status)
  {
    case PB_STAT_Ready:
      sms_phbReady_ind();
// Aug 18, 2006   ER: OMAPS00089840  x0039928
// Fix : Send PHB READY signal to MMI only if the status indication is for DeleteAll
if(DeleteAll_flag)
	phb_signal(E_PHB_READY,0);
      break;

    case PB_STAT_Busy:
    case PB_STAT_Blocked:
    default:
      phb_signal(E_PHB_BUSY, 0);
      break;
  }
#else
  if (status EQ PB_STAT_Ready)
  {
    sms_phbReady_ind();
	// Aug 18, 2006   ER: OMAPS00089840  x0039928
	// Fix : Send PHB READY signal to MMI only if the status indication is for DeleteAll
	if(DeleteAll_flag)
		phb_signal(E_PHB_READY,0);
  }
  if (status EQ PB_STAT_Blocked)
    phb_signal(E_PHB_BUSY, 0);
#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_PHB              |
| STATE  : code                         ROUTINE: phb_exit            |
+--------------------------------------------------------------------+


   PURPOSE :

*/

void phb_exit(void)
{
  TRACE_FUNCTION ("phb_exit()");
}
