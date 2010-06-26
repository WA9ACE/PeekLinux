/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		
 $File:		    MmiNetwork.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
                        
********************************************************************************
	Jun 23, 2006    DR: OMAPS00082374 x0pleela
   	Description: EONS Failures - unexpected output from the MMI: 2 carriers; on the top always T-Mobile 
   	Solution:a) Changed the size of SIZE_NETWORK_NAME from 6 to 7


*******************************************************************************
                                                                              
                                Required Include Files
                                                                              
*******************************************************************************/


#ifndef _DEF_MMI_NETWORK_H_
#define _DEF_MMI_NETWORK_H_

/*
 * Events from network management dialog
 */
#define NETWORK_NO_SERVICE         400
#define NETWORK_FULL_SERVICE       401
#define NETWORK_SEARCH_NETWORK     402
#define NETWORK_NO_PLMN_XXX_AVAIL  403


/*
 * NETWORK_NO_SERVICE  : no parameter
 *
 * The mobile has no or limited service
 *
 * NETWORK_FULL_SERVICE : no parameter
 *
 * The mobile has full service. The current network as text string and in MCC/MNC format
 * can be requested by the function network_get_plmn.
 *
 * NETWORK_SEARCH_NETWORK : no parameter
 *
 * The mobile indicates searching for a network.
 *
 * NETWORK_NO_PLMN_XXX_AVAIL: no parameter
 *
 * The network has enabled network logging, but this network is not available. The logged
 * network can be requested by the function network_get_plmn.
 * 
 */
#define SIZE_PLMN_NAME 			20
#define SIZE_NETWORK_NAME		7 //x0pleela 23 June, 2006 DR:OMAPS00082374

typedef struct  
{
  UBYTE plmn_name [SIZE_PLMN_NAME];        /* textual format of PLMN */
  UBYTE network_name [SIZE_NETWORK_NAME];      /* numeric format of PLMN */
  UBYTE service_provider_name[SP_NAME]; /* Marcus: Issue 1618: 24/01/2003 */
  UBYTE roaming_indicator;     /* Roaming Indicator      */
} T_CURRENT_NETWORK;

#ifdef FF_TIMEZONE
typedef enum 
{
 NitzAutomatic=0,
 NitzOn,
 NitzOff
} T_NITZ_UPDATE;
#endif

/*
 * Prototypes
 */
T_MFW_HND network_create             (T_MFW_HND parent_window);
void      network_destroy            (T_MFW_HND own_window);
void      network                    (T_MFW_HND win, USHORT event, SHORT value, void * parameter);

/*
 * Menu entries
 */
int       network_set_mode_auto      (T_MFW_MNU * m, T_MFW_MNU_ITEM * mi);
int       network_set_mode_man       (T_MFW_MNU * m, T_MFW_MNU_ITEM * mi);
int		  network_pref_list          (T_MFW_MNU * m, T_MFW_MNU_ITEM * mi);
int       network_log_on             (T_MFW_MNU * m, T_MFW_MNU_ITEM * mi);
int       network_log_off            (T_MFW_MNU * m, T_MFW_MNU_ITEM * mi);
int settingsNITZ (T_MFW_MNU *m, T_MFW_MNU_ITEM *i);//x0066814

USHORT    nm_flag_log_on             (T_MFW_MNU * m, T_MFW_MNU_ATTR * ma, T_MFW_MNU_ITEM * mi);
USHORT    nm_flag_log_off            (T_MFW_MNU * m, T_MFW_MNU_ATTR * ma, T_MFW_MNU_ITEM * mi);

void      network_get_name           (T_CURRENT_NETWORK * nm);
void      network_start_full_service (void);
T_MFW_HND networkInit (T_MFW_HND parent_window);

//x0pleela 08 June, 2006  DR: OMAPS00079607
//Adding prototype
#ifdef FF_CPHS
UBYTE getRoamingStatus(void);
#endif

#endif

