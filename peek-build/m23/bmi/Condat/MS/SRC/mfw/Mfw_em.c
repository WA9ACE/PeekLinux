/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_em.c         $|
| $Author:: MC  $ CONDAT UK          $Revision:: 1                  $|
| CREATED: 23.09.02                                                 $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_EM

   PURPOSE : Engineering Mode functions

*/

/*
********************************Include Files**********************************************
**/

#define ENTITY_MFW


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#ifndef PCM_2_FFS
#include "pcm.h"
#endif

#ifndef NEPTUNE_BOARD
#include "ffs/ffs.h"
#else
#include "ffs.h"
#endif

#include "message.h"
#include "prim.h"
#include "aci_cmh.h"
#if defined (FAX_AND_DATA)
#include "aci_fd.h"
#endif

#ifdef GPRS
#include "gprs.h"
#endif
#include "dti_conn_mng.h"
#include "phb.h"
#include "psa.h"
#include "cmh.h"
#include "cmh_phb.h"
#include "psa.h"
#include "psa_sim.h"
#include "cus_aci.h"
#include "cmh_mm.h"
#include "cmh_sim.h"
#include "Mfw_em.h"
#include "aci_em.h" 
#include "mfw_win.h"

#ifdef NEPTUNE_BOARD /*EngMode */
#include "Aci_cmh.h"
#endif

#ifdef NEPTUNE_BOARD
T_ENG_INFO_REQ* Eng_Info_Req;  /*EngMode */
T_ENG_INFO_STATIC_REQ* Eng_Info_Static_Req;

int EmType = NULL;
#endif

#define ASCII_OFFSET 0x30 /*SPR 1554*/

/*
********************************Internal data types**********************************************
**/

#ifndef NEPTUNE_BOARD
typedef union
{
	MFW_EM_Mobile_Info MobileInfo;
	MFW_EM_Serving_Cell_Parameters SCInfo;
	MFW_EM_Neighbour_Cell_Parameters NCInfo;
	MFW_EM_Location_Parameters	LocInfo;
	MFW_EM_Ciph_hop_DTX_Parameters CHDTXInfo;
	MFW_EM_GPRS_Parameter GPRSInfo;
}
	T_MFW_EM_PARA;
#endif    

typedef struct
{
    T_MFW_EVENT     emask;          /* events of interest */
    T_MFW_EVENT     event;          /* current event */
    T_MFW_CB        handler;
#ifndef NEPTUNE_BOARD
    T_MFW_EM_PARA  para;
#endif
    
} T_MFW_EM;
/*
********************************Global variables**********************************************
**/

#ifndef NEPTUNE_BOARD
MFW_EM_Data* MfwEmData= NULL;
#endif

EXTERN MfwHdr *current_mfw_elem;

/*
********************************Prototypes**********************************************
**/

#ifndef NEPTUNE_BOARD
int Mfw_em_sign_exec(T_MFW_HDR * hdr, T_MFW_EVENT event,T_MFW_EM_PARA *para); 
#else /*if NEPTUNE BOARD*/
int Mfw_em_sign_exec(T_MFW_HDR * hdr, T_MFW_EVENT event,int* dummy); 
#endif
void Mfw_em_signal(MfwEvt event, void *para);

#ifndef NEPTUNE_BOARD  /*EngMode */
void Mfw_em_cb(T_DRV_SIGNAL_EM* signal);
#endif
static int emCommand (U32 cmd, void *h);

/**************************************Public Functions*****************************************/

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_EM            |
| STATE   : code                        ROUTINE : Mfw_em_init           |
+--------------------------------------------------------------------+

  PURPOSE : initialise module

*/
int Mfw_em_init(void)
{	TRACE_EVENT("Mfw_em_init()");
#ifndef NEPTUNE_BOARD
	em_Init(Mfw_em_cb, NULL); /*init EM driver*/	
#endif /* NEPTUNE_BOARD */    
	mfwCommand[MfwTypEm] = (MfwCb) emCommand;
	return TRUE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_EM            |
| STATE   : code                        ROUTINE : Mfw_em_create           |
+--------------------------------------------------------------------+

  PURPOSE : binds passed callback function to event mask

*/
MfwHnd Mfw_em_create(MfwHnd hWin, MfwEvt event,  MfwCb cb_func)
{
	MfwHdr *hdr;
    T_MFW_EM *em_para;

    TRACE_EVENT("em_create()");

    hdr = (MfwHdr *) mfwAlloc(sizeof(MfwHdr));
    em_para = (T_MFW_EM *) mfwAlloc(sizeof(T_MFW_EM));

    if (!hdr || !em_para)
        return 0;

    em_para->emask = event;
    em_para->handler = cb_func;

    hdr->data = em_para;
    hdr->type = MfwTypEm;

    return mfwInsert((MfwHdr *) hWin,hdr);
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_EM            |
| STATE   : code                        ROUTINE : Mfw_em_exit           |
+--------------------------------------------------------------------+

  PURPOSE : exit module

*/
void Mfw_em_exit(void)
{	TRACE_EVENT("Mfw_em_exit()");
#ifndef NEPTUNE_BOARD
	if (MfwEmData != NULL)
		mfwFree((void*)MfwEmData, sizeof(MFW_EM_Data));
	
	em_Exit();
#endif    
}
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_EM            |
| STATE   : code                        ROUTINE : Mfw_em_get_data           |
+--------------------------------------------------------------------+

  PURPOSE : requests data from ACI EM driver

*/
int Mfw_em_get_data(MfwEmDataType type)
{
    TRACE_EVENT("Mfw_em_get_data()");

#ifdef NEPTUNE_BOARD
    EmType = type;
#endif

#ifndef NEPTUNE_BOARD /* EngMode */

#ifndef _EM_MMI_MFW_TEST
/*If not already allocated, Allocate memory to store data in*/
if (MfwEmData == NULL)	
	if ((MfwEmData = (MFW_EM_Data*)mfwAlloc(sizeof(MFW_EM_Data))) == NULL)
		return FALSE;
	/*Request appropriate data from Em driver*/
	switch(type)
	{
		case EM_MOBILE_INFO: 
			/*MC SPR 1554, added call to request software version and identity param*/
			em_Read_Data_Parameter(EM_CLASS_MOBILE_DATA, EM_SUBCLASS_SW_VERSION, 1,Mfw_em_cb);
			em_Read_Data_Parameter(EM_CLASS_MOBILE_DATA,EM_SUBCLASS_ID, 1, Mfw_em_cb);
		break;
		case EM_SERVING_CELL_PARAMS:
			em_Read_Data_Parameter(EM_CLASS_INFRA_DATA, EM_SUBCLASS_SC, 1, Mfw_em_cb);
		break;
		case EM_NEIGHBOURING_CELL_PARAMS:
			em_Read_Data_Parameter(EM_CLASS_INFRA_DATA, EM_SUBCLASS_SC, 1, Mfw_em_cb);
      em_Read_Data_Parameter(EM_CLASS_INFRA_DATA, EM_SUBCLASS_NC, 1, Mfw_em_cb);
		break;
		case EM_LOCATION_PARAMS:
			em_Read_Data_Parameter(EM_CLASS_INFRA_DATA, EM_SUBCLASS_LOC_PAG, 1, Mfw_em_cb);
      em_Read_Data_Parameter(EM_CLASS_INFRA_DATA, EM_SUBCLASS_SC, 1, Mfw_em_cb);
		break;
		case EM_CIPH_HOP_DTX_PARAMS:
			em_Read_Data_Parameter(EM_CLASS_INFRA_DATA, EM_SUBCLASS_CIPH_HOP_DTX, 1, Mfw_em_cb);
		break;
		case EM_GPRS_PARAMS:
			em_Read_Data_Parameter(EM_CLASS_INFRA_DATA, EM_SUBCLASS_SC_GPRS, 1, Mfw_em_cb);
		break;
	}
	return TRUE;
#endif 


			
#ifdef _EM_MMI_MFW_TEST
/*Test harness, just returns made-up data to MMI*/
if (MfwEmData == NULL)	
	if ((MfwEmData = (MFW_EM_Data*)mfwAlloc(sizeof(MFW_EM_Data))) == NULL)
		return FALSE;
	switch(type)
	{
		case EM_MOBILE_INFO:
		{	//MfwEmData->MobileInfo.software_version = 0x44;
			strcpy(MfwEmData->MobileInfo.IMEI,  "123456789ABC");
			strcpy(MfwEmData->MobileInfo.IMSI,  "123456789ABC");
			MfwEmData->MobileInfo.TMSI = 76;
			Mfw_em_signal(EM_MOBILE_INFO, (void*)&MfwEmData->MobileInfo);
		}
		break;
		case EM_SERVING_CELL_PARAMS:
		{	
			MfwEmData->SCInfo.arfcn= 0x44;
			MfwEmData->SCInfo.RSSI= 86;
			MfwEmData->SCInfo.RXQ = 67;
			MfwEmData->SCInfo.RLT = 67;
			Mfw_em_signal(EM_SERVING_CELL_PARAMS, (void*)&MfwEmData->SCInfo);
		}
		break;		
		case EM_NEIGHBOURING_CELL_PARAMS:
		{	MfwEmData->NCInfo.NUM= 0x05;
			strcpy((char*)MfwEmData->NCInfo.arfcn, "WERTYUIOPAS");
			strcpy((char*)MfwEmData->NCInfo.RSSI, "12345");
			
			Mfw_em_signal(EM_NEIGHBOURING_CELL_PARAMS, (void*)&MfwEmData->NCInfo);
				
		}
		break;
		case EM_LOCATION_PARAMS:
		{	strcpy((char*)MfwEmData->LocInfo.MCC, "123");
			strcpy((char*)MfwEmData->LocInfo.MNC, "456");
			MfwEmData->LocInfo.LAC = 12345;
			MfwEmData->LocInfo.CI = 12045;
			
			Mfw_em_signal(EM_LOCATION_PARAMS, (void*)&MfwEmData->LocInfo);
				
		}
		break;
		case EM_GPRS_PARAMS:
		{	MfwEmData->GPRSInfo.ULS= 0;
			MfwEmData->GPRSInfo.DLS= 0;
			MfwEmData->GPRSInfo.coding_scheme = 0;
			Mfw_em_signal(EM_GPRS_PARAMS, (void*)&MfwEmData->GPRSInfo);
				
		}
		break;
		case EM_CIPH_HOP_DTX_PARAMS:
		{	MfwEmData->CHDTXInfo.ciph_status= 0;
			MfwEmData->CHDTXInfo.hopping_channels= 0;
			MfwEmData->CHDTXInfo.DTX_status = 1;
			Mfw_em_signal(EM_CIPH_HOP_DTX_PARAMS, (void*)&MfwEmData->CHDTXInfo);
				
		}
		break;
	}
	return TRUE;
#endif

#endif
}

/***********************************************Private functions:********************************/
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_EM            |
| STATE   : code                        ROUTINE : emCommand         |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

static int emCommand (U32 cmd, void *h)
{TRACE_EVENT("emCommand()");
    switch (cmd)
    {
        case MfwCmdDelete:              /* delete me                */
            if (!h)
                return 0;
            em_delete(h);
            return 1;
        default:
            break;
    }

    return 0;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_EM            |
| STATE   : code                        ROUTINE : em_delete         |
+--------------------------------------------------------------------+

  PURPOSE : delete EM event handler

*/

MfwRes em_delete (MfwHnd h)
{
    TRACE_EVENT("em_delete()");

    if (!h || !((MfwHdr *) h)->data)
        return MfwResIllHnd;

    if (!mfwRemove((MfwHdr *) h))
        return MfwResIllHnd;

    mfwFree((U8 *) ((MfwHdr *) h)->data,sizeof(T_MFW_EM));
    mfwFree((U8 *) h,sizeof(MfwHdr));

    return MfwResOk;
}
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_EM            |
| STATE   : code                        ROUTINE : Mfw_em_sign_exec           |
+--------------------------------------------------------------------+

  PURPOSE : send event to MMI

*/
#ifndef NEPTUNE_BOARD
int Mfw_em_sign_exec(T_MFW_HDR * cur_elem, T_MFW_EVENT event,T_MFW_EM_PARA *para)
{
 TRACE_EVENT("Mfw_em_sign_exec()");

    while (cur_elem)
    {
        if (cur_elem->type == MfwTypEm)
        {
            T_MFW_EM *em_data;
            em_data = (T_MFW_EM *) cur_elem->data;
            if (em_data->emask & event)
            {
                em_data->event = event;
                switch (event)
                {
                    case EM_MOBILE_INFO:
                        memcpy(&em_data->para.MobileInfo,
                                para,sizeof(MFW_EM_Mobile_Info));
                        break;
                   
                    case EM_SERVING_CELL_PARAMS:
                        memcpy(&em_data->para.SCInfo,
                                para,sizeof(MFW_EM_Serving_Cell_Parameters));
                        break;
                    case EM_NEIGHBOURING_CELL_PARAMS:
                        memcpy(&em_data->para.NCInfo,
                                para,sizeof(MFW_EM_Neighbour_Cell_Parameters));
                        break;
                   
                    case EM_LOCATION_PARAMS:
                        memcpy(&em_data->para.LocInfo,
                                para,sizeof(MFW_EM_Location_Parameters));
                        break;
                     case EM_CIPH_HOP_DTX_PARAMS:
                        memcpy(&em_data->para.CHDTXInfo,
                                para,sizeof(MFW_EM_Ciph_hop_DTX_Parameters));
                        break;
                   
                    case EM_GPRS_PARAMS:
                        memcpy(&em_data->para.GPRSInfo,
                                para,sizeof(MFW_EM_GPRS_Parameter));
                        break;
                }
                if (em_data->handler)
                {
                
                  // store current mfw elem
                  current_mfw_elem = cur_elem;
             
                  if ((*(em_data->handler))(em_data->event,
                                             (void *) &em_data->para))
                    return TRUE;
                }
            }
        }
        cur_elem = cur_elem->next;
    }

    return FALSE;


}

#else /* if NEPTUNE BOARD*/
int Mfw_em_sign_exec(T_MFW_HDR * cur_elem, T_MFW_EVENT event, int* dummy)
{
 TRACE_EVENT("Mfw_em_sign_exec()");

    while (cur_elem)
    {
        if (cur_elem->type == MfwTypEm)
        {
            T_MFW_EM *em_data;
            em_data = (T_MFW_EM *) cur_elem->data;
                em_data->event = event;
                if (em_data->handler)
                {
                  // store current mfw elem
                  current_mfw_elem = cur_elem;
             
                  if ((*(em_data->handler))(em_data->event,
                                             (void *) &dummy))
                    return TRUE;
                }
        }
        cur_elem = cur_elem->next;
    }

    return FALSE;


}

#endif
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_EM            |
| STATE   : code                        ROUTINE : Mfw_em_signal           |
+--------------------------------------------------------------------+

  PURPOSE : sends event to MMi via Mfw_em_sign_exec()

*/
void Mfw_em_signal(MfwEvt event, void *para)
{
 UBYTE temp = dspl_Enable(0);
    TRACE_EVENT ("em_signal()");

  if (mfwSignallingMethod EQ 0)
  {
    if (mfwFocus)
        if (Mfw_em_sign_exec(mfwFocus,event,para))
			{
				dspl_Enable(temp);            
				return;
			}
    if (mfwRoot)
        Mfw_em_sign_exec(mfwRoot,event,para);
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
        if (Mfw_em_sign_exec (h, event, para))
			{
				dspl_Enable(temp);            
				return;
			}

        /*
         * All windows tried inclusive root
         */
        if (h == mfwRoot)
			{
				dspl_Enable(temp);            
				return;
			}

        /*
         * get parent window
         */
        h = mfwParent(mfwParent(h));
		if(h)
			h = ((MfwWin * )(h->data))->elems;
      }
      Mfw_em_sign_exec (mfwRoot, event, para);
    }      
	dspl_Enable(temp);            
	return;


}

#ifndef NEPTUNE_BOARD /*EngMode */
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_EM            |
| STATE   : code                        ROUTINE : Mfw_em_cb           |
+--------------------------------------------------------------------+

  PURPOSE : Handles events from the ACI EM driver

*/
void Mfw_em_cb(T_DRV_SIGNAL_EM* signal)
{
	TRACE_EVENT_P1("Mfw_em_cb, signalType: %d", signal->SignalType);
	
	switch(signal->SignalType)
	{
		case EM_SUBCLASS_SC:
		{	/*copy Serving Cell Information to appropriate data structure*/

            T_EM_SC_INFO_CNF *serv_cell = (T_EM_SC_INFO_CNF*)&signal->UserData.sc;
               
			MfwEmData->SCInfo.arfcn = serv_cell->arfcn;
			MfwEmData->SCInfo.RSSI = serv_cell->rxlev;
			MfwEmData->SCInfo.RXQ = serv_cell->rxqual_f;
			MfwEmData->SCInfo.RLT = serv_cell->rlt;
      MfwEmData->SCInfo.C1  = serv_cell->c1;
      MfwEmData->SCInfo.C2  = serv_cell->c2;
      MfwEmData->SCInfo.BSIC  = serv_cell->bsic;
      MfwEmData->SCInfo.TAV  = serv_cell->tav;
      MfwEmData->SCInfo.LAC  = serv_cell->lac;
      
      MfwEmData->NCInfo.arfcn[6] = MfwEmData->SCInfo.arfcn;
      MfwEmData->NCInfo.RSSI[6] = MfwEmData->SCInfo.RSSI;
      MfwEmData->NCInfo.C2[6] = MfwEmData->SCInfo.C2;
      MfwEmData->NCInfo.LAC_NC[6] = MfwEmData->SCInfo.LAC;
      MfwEmData->NCInfo.BSIC_NC[6] = MfwEmData->SCInfo.BSIC;

			/* copy Location Info*/
			MfwEmData->LocInfo.LAC = serv_cell->lac;

      			/*send event to MMI*/
			Mfw_em_signal(EM_SERVING_CELL_PARAMS, (void*)&MfwEmData->SCInfo);
		}		
		break;	
		case EM_SUBCLASS_SC_GPRS:
		{	/*MC, SPR 1554 added code to copy NMOinto GPRS data structure*/

			T_EM_SC_GPRS_INFO_CNF *gprs_info = (T_EM_SC_GPRS_INFO_CNF*)&signal->UserData.sc_gprs;

			MfwEmData->GPRSInfo.NMO = gprs_info->nmo;
      MfwEmData->GPRSInfo.NDTS = gprs_info->tn;
      MfwEmData->GPRSInfo.RAC = gprs_info->rac;
      MfwEmData->GPRSInfo.C31 = gprs_info->c31;
      MfwEmData->GPRSInfo.C32 = gprs_info->c32;

			Mfw_em_signal(EM_GPRS_PARAMS, (void*)&MfwEmData->GPRSInfo);
		}
		break;
		case EM_SUBCLASS_NC:
		{	T_EM_NC_INFO_CNF *neighbour_cell = (T_EM_NC_INFO_CNF*)&signal->UserData.nc;
			MfwEmData->NCInfo.NUM = neighbour_cell->no_ncells;
			memcpy(MfwEmData->NCInfo.arfcn, neighbour_cell->arfcn_nc, 6*sizeof(USHORT));
			memcpy(MfwEmData->NCInfo.RSSI, neighbour_cell->rxlev_nc, 6*sizeof(UBYTE));
			memcpy(MfwEmData->NCInfo.C2, neighbour_cell->c2_nc, 6*sizeof(SHORT));
			memcpy(MfwEmData->NCInfo.LAC_NC, neighbour_cell->lac_nc, 6*sizeof(USHORT));
			memcpy(MfwEmData->NCInfo.BSIC_NC, neighbour_cell->bsic_nc, 6*sizeof(UBYTE));
     Mfw_em_signal(EM_NEIGHBOURING_CELL_PARAMS, (void*)&MfwEmData->NCInfo);
		}
		case EM_SUBCLASS_LOC_PAG:
		{	MfwEmData->LocInfo.LUP = signal->UserData.log_pag.t3212;
			/*Convert MCC & MNC to ASCII strings from BCD*/
			utl_BCD2DialStr (signal->UserData.log_pag.mcc, MfwEmData->LocInfo.MCC, 3);
			utl_BCD2DialStr (signal->UserData.log_pag.mnc, MfwEmData->LocInfo.MNC, 3);
			Mfw_em_signal(EM_LOCATION_PARAMS, (void*)&MfwEmData->LocInfo);
		}
		case EM_SUBCLASS_CIPH_HOP_DTX:
		{	MfwEmData->CHDTXInfo.ciph_status = signal->UserData.cip.ciph_stat;
			MfwEmData->CHDTXInfo.HSN = signal->UserData.cip.hsn;
			//!!!!!HOPPING CHANNELS!!!!!!!!!!!!!!
 /*     memcpy(MfwEmData->CHDTXInfo.ma, signal->UserData.cip.hop_chn.ma, 65);
      if (signal->UserData.cip.hop_chn.v_start)
      {
			  memcpy(MfwEmData->CHDTXInfo.ma2, signal->UserData.cip.hop_chn.ma2, 65);
      }*/
      MfwEmData->CHDTXInfo.DTX_status = signal->UserData.cip.dtx_stat;
			Mfw_em_signal(EM_CIPH_HOP_DTX_PARAMS, (void*)&MfwEmData->CHDTXInfo);
		}
		case EM_SUBCLASS_ID:
		{	/*MC, SPR 1554 added conversion of IMEI and IMSI from numbers to ASCII*/
			
			int i;
			/*convert to ASCII digits*/
			for (i=0; i < 15; i++)
			{
				MfwEmData->MobileInfo.IMEI[i]= signal->UserData.id.em_imeisv.ident_dig[i] + ASCII_OFFSET;
				MfwEmData->MobileInfo.IMSI[i]=signal->UserData.id.em_imsi.ident_dig[i] +ASCII_OFFSET;
			}
			MfwEmData->MobileInfo.IMEI[15] = NULL;
			MfwEmData->MobileInfo.IMSI[15] = NULL;	
	
			MfwEmData->MobileInfo.TMSI = signal->UserData.id.tmsi;
			
			Mfw_em_signal(EM_MOBILE_INFO, (void*)&MfwEmData->MobileInfo);
		}
		case EM_SUBCLASS_SW_VERSION:
		{	
			T_EM_SW_VER* sw_ver = &signal->UserData.version;
			memcpy(MfwEmData->MobileInfo.SIM_version, sw_ver->sim, MAX_VER*sizeof(char));
			memcpy(MfwEmData->MobileInfo.CC_version , sw_ver->cc, MAX_VER*sizeof(char));
			memcpy(MfwEmData->MobileInfo.SS_version , sw_ver->ss, MAX_VER*sizeof(char));
			memcpy(MfwEmData->MobileInfo.SMS_version , sw_ver->sms, MAX_VER*sizeof(char));
			memcpy(MfwEmData->MobileInfo.MM_version, sw_ver->mm, MAX_VER*sizeof(char));
			memcpy(MfwEmData->MobileInfo.RR_version , sw_ver->rr, MAX_VER*sizeof(char));
			memcpy(MfwEmData->MobileInfo.DL_version , sw_ver->dl, MAX_VER*sizeof(char));
			memcpy(MfwEmData->MobileInfo.ALR_version , sw_ver->alr, MAX_VER*sizeof(char));
		}
		default:
			TRACE_EVENT("Unknown EM data type");

	}

}

#endif

#ifdef NEPTUNE_BOARD
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_EM            |
| STATE   : code                        ROUTINE : Mfw_em_response           |
+--------------------------------------------------------------------+

  PURPOSE : sends event to MMi via Mfw_em_sign_exec()

*/
void Mfw_em_response_EINFO(T_ENG_INFO_REQ *emode_data, 
            T_ENG_INFO_STATIC_REQ *emode_static_data)
{
    int* dummy;
    TRACE_FUNCTION("Mfw_em_response_EINFO()");
    Eng_Info_Req = emode_data;
    Mfw_em_signal(EmType, (void*)&dummy);
    
}

void Mfw_em_response_ESINFO(T_ENG_INFO_STATIC_REQ *emode_static_data)
{
    TRACE_FUNCTION("Mfw_em_response_ESINFO()");
    Eng_Info_Static_Req = emode_static_data;
}


#endif
