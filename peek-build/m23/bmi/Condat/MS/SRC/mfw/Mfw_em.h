#ifndef _MFW_EM_H_
#define MFW_EM_H_

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_em.h      $|
| $Author: KGT                          $Revision:: 1               $|
| CREATED: 13.02.2001                   $Modtime::  13.02.2001      $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_EM

   PURPOSE : Interfaces between ACI EM Driver and MMI.

   ********************************************************************************
 $History: Mfw_em.h

    Apr 24, 2006    ERT: OMAPS00067603 x0043642
    Description: Engineering mode
    Solution:  Added Enums to handle different em categories.
 $End



*************************************Includes*************************************
*/

#include "mfw_mfw.h"
#include "mfw_sys.h"

/*
*************************************Datatypes*************************************
*/

typedef enum MFW_EM_DataType
{
#ifndef NEPTUNE_BOARD
	EM_MOBILE_INFO = 0x01,
	EM_SERVING_CELL_PARAMS = 0x02,
	EM_NEIGHBOURING_CELL_PARAMS = 0x04,
	EM_LOCATION_PARAMS = 0x08, 
	EM_CIPH_HOP_DTX_PARAMS = 0x10,
	EM_GPRS_PARAMS = 0x20
#else /*  NEPTUNE BOARD */ /*EngMode */
    EM_SERVING_CELL_PARAMS,
    EM_SERVING_CELL_RF_PARAMS,
    EM_SERVING_CHANNEL_PARAMS,
    EM_AMR_PARAMS,
    EM_SNDCP_PARAMS,
    EM_USERLEVEL_PARAMS,
    EM_LLCTRACING_PARAMS,
    EM_RLC_MAC_PARAMS,
    EM_GMM_INFO_PARAMS,
    EM_CALL_STATUS_PARAMS,
    EM_LAYER_MESSAGE_PARAMS,
    EM_NEIGHBOURING_CELL_PARAMS,
    EM_WCDMA_LAYER_MESSAGE_PARAMS,
    EM_3G_INFO_PARAMS,
    EM_PACKET_DATA_PARAMS
#endif
}	MfwEmDataType;

#ifndef NEPTUNE_BOARD /*EngMode */
typedef struct 
{
	/*software_versions*/
	char		SIM_version[MAX_VER];
	char		CC_version[MAX_VER];
	char		SS_version[MAX_VER];
	char		SMS_version[MAX_VER];
	char		MM_version[MAX_VER];
	char		RR_version[MAX_VER];
	char		DL_version[MAX_VER];
	char		ALR_version[MAX_VER];
	
	char		IMEI[16];
	char 		IMSI[16];
	ULONG 	TMSI; /*SPR 1554, changed from LONG to ULONG*/
}MFW_EM_Mobile_Info;

typedef struct
{
  USHORT		arfcn;
  UBYTE		RSSI; //Received field strength
  UBYTE		RXQ; //received quality
  UBYTE		RLT; //radio link timeout counter
  SHORT   C1; //C1 Path lost criterion
  SHORT   C2; //C2 Parameter - often the same as RSSI for SC
  UBYTE   BSIC; //Base Station ID Code
  UBYTE   TAV;    //Timing Advanced
  USHORT  LAC;  //Location Area Code
}MFW_EM_Serving_Cell_Parameters;

typedef struct
{
	UBYTE   NUM; //number of neighbouring cells
	USHORT  arfcn[7];//channel numbers
	UBYTE   RSSI[7];//received signal strength
  SHORT   C2[7];//received C2 Parameter
  USHORT  LAC_NC[7]; //Location Area Code
  UBYTE   BSIC_NC[7];  //Base Station ID Code  
}	MFW_EM_Neighbour_Cell_Parameters;

typedef struct
{
	UBYTE LUP;//Location update period
	UBYTE MCC[4];//mobile country code
	UBYTE MNC[4];//mobile network code
	USHORT LAC; //location area code
	/*SPR 1554, removed cell id */
}MFW_EM_Location_Parameters;

typedef struct
{
	UBYTE ciph_status; //ciphering status
	/*USHORT ma[65]; //MA list after time
  USHORT ma2[65];//MA list before  time*/
	USHORT HSN; //hopping sequence number
 	UBYTE DTX_status;
}MFW_EM_Ciph_hop_DTX_Parameters;

typedef struct
{	/*SPR 1554, changed the data structure to only include the NMO*/
	UBYTE NMO; //network mode of operation
  UBYTE NDTS; //number of Downlink Timeslots
  UBYTE RAC;  //Routing Area Color
  SHORT C31; //C31 Criterion
  SHORT C32; //C32 Criterion
}
MFW_EM_GPRS_Parameter;

typedef struct
{
	MFW_EM_Mobile_Info MobileInfo;
	MFW_EM_Serving_Cell_Parameters SCInfo;
	MFW_EM_Neighbour_Cell_Parameters NCInfo;
	MFW_EM_Location_Parameters	LocInfo;
	MFW_EM_Ciph_hop_DTX_Parameters CHDTXInfo;
	MFW_EM_GPRS_Parameter GPRSInfo;
}
	MFW_EM_Data;

#endif


/*
*************************************Prototypes*************************************
*/

/* Initialise mfw_em module(which initialises EM driver);*/
int Mfw_em_init(void);

/* Bind events specified by mask to an MMI call-back function.*/
MfwHnd Mfw_em_create(MfwHnd hWin, MfwEvt event,  MfwCb cb_func);

/*Deallocate any memory for the module.*/
void Mfw_em_exit(void);

/* Request specified data from EM Driver.*/
int Mfw_em_get_data(MfwEmDataType type);

/* Added to remove warning Aug - 11 */
MfwRes em_delete (MfwHnd h);
EXTERN UBYTE dspl_Enable (UBYTE   in_Enable);
/* End - remove warning */

#endif

