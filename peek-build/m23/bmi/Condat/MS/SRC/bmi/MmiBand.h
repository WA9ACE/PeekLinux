#ifndef _MMI_BAND_H_
#define _MMI_BAND_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Frequency band selection
 $File:		    MmiBand.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    03/04/02                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the start code for the MMI
                        
********************************************************************************
 $History: MmiBand.h

	03/04/02  			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/




/*******************************************************************************
                                                                              
                                Include files
                                                                              
*******************************************************************************/
#include "mfw_mnu.h"

/*******************************************************************************
                                                                              
                               Prototypes
                                                                              
*******************************************************************************/

/*display radio mode/type status*/
int mmi_band_radio_mode_info(MfwMnu* m, MfwMnuItem* i);

/*change radio mode/type to automatic*/
int mmi_band_radio_mode_change_auto(MfwMnu* m, MfwMnuItem* i);

/*change radio mode/type manually*/
int mmi_band_radio_mode_change_manual(MfwMnu* m, MfwMnuItem* i);

#endif /*_MMI_BAND_H_*/
