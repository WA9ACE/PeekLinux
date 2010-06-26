/*******************************************************************************

					CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		      MmiSatClassE.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		    Condat(UK)                                                         
 $Date:		    13/03/03                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This is the header file for the SAT Class E implementation.
                        
********************************************************************************
 $History: MmiSatClassE.h

	13/03/03			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/

#ifndef _DEF_MMI_SAT_CLASS_E_H_
#define _DEF_MMI_SAT_CLASS_E_H_

#include "mfw_mfw.h"
#include "mfw_sat.h"

/*
 * External Interfaces
 */

T_MFW_HND sat_class_e_create(T_MFW_HND parent_win);
void sat_class_e_exec (T_MFW_HND win, USHORT event, SHORT value,  T_SAT_CMD * sat_command);

#endif  /* _DEF_MMI_SAT_CLASS_E_H_   */
