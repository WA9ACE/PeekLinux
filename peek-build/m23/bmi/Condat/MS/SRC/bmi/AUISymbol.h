#ifndef _AUI_SYMBOL_H
#define _AUI_SYMBOL_H
/*******************************************************************************

					CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI                                                   
 $Module:		MMI
 $File:		    AUISymbol.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		                                                    
                                                                               
********************************************************************************
                                                                              
 Description: Header information for AUI symbol screen.
                        
********************************************************************************
 $History: AUISymbol.h

	
	   
 $End

*******************************************************************************/

T_MFW_HND AUI_symbol_Start (T_MFW_HND  parent, USHORT return_event);
void AUI_symbol_Destroy (T_MFW_HND window);
#endif
