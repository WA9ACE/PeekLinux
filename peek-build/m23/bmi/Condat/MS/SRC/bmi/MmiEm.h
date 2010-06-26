#ifndef _MMI_EM_H_
#define MMI_EM_H_
/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Engineering mode
 $File:		    MmiEm.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    23/09/02                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

   	 Handles Engineering mode at MMI level
                        
********************************************************************************
 $History: MmiEm.c

    Apr 24, 2006    ERT: OMAPS00067603 x0043642
    Description: Engineering mode
    Solution:  Added new engineering mode function prototypes to satisfy Neptune engineering mode
    requirements.

 $End*/


/*******************************************************************************
                                                                              
                                Include files
                                                                              
*******************************************************************************/



#include "mfw_mfw.h"
#include "mfw_mnu.h"


/******************************Function Prototypes*******************************/
void mmi_em_init(void);

void mmi_em_exit(void);

//#ifdef TI_PS_FF_EM
#if defined (TI_PS_FF_EM) || defined (MMI_EM_ENABLED)
/*Starts the engineering mode menu*/
void mmi_em_start_eng_mode_menus(T_MFW_HND parent);
#endif

#ifndef NEPTUNE_BOARD


/*Called from menu; begins process to display Mobile Information*/
int Mmi_em_display_mobile_info_data(MfwMnu* m, MfwMnuItem* i); 

/*Called from menu; begins process to display Serving Cell parameters*/
int Mmi_em_display_serving_cell_params(MfwMnu* m, MfwMnuItem* i);

/*Called from menu; begins process to display Neighbouring Cell parameters*/
int Mmi_em_display_neighbour_cell_params(MfwMnu* m, MfwMnuItem* i);

/*Called from menu; begins process to display Locationparameters*/
int Mmi_em_display_location_params(MfwMnu* m, MfwMnuItem*i );

/*begins process to display Ciphering, hopping and discontinuous transmission parameters*/
int Mmi_em_display_ciph_hop_DTX_params(MfwMnu* m, MfwMnuItem* i);

/*Called from menu; begins process to display GPRS specific parameters*/
int Mmi_em_display_GPRS_params(MfwMnu* m, MfwMnuItem* i);

#else /* if NEPTUNE_BOARD */

/*Called from menu; begins process to display Serving Cell parameters*/
int Mmi_em_start(MfwMnu* m, MfwMnuItem* i);


#endif  /*ifndef NEPTUNE_BOARD */

#endif
