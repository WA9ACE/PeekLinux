#ifndef _MMIAOC_H_
#define _MMIAOC_H_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Advice of Charge
 $File:		    MmiAoc.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the start code for the MMI
                        
********************************************************************************
 $History: MmiAoc.h

	May 26, 2006 ER: OMAPS00079607 x0012849 :Jagannatha M
	Description: CPHS: Display 'charge/Timers' menu based on AOC ( Advice of charge )		       
	
	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/




/*******************************************************************************
                                                                              
                                Include files
                                                                              
*******************************************************************************/






/*******************************************************************************
                                                                              
                                Interface constants
                                                                              
*******************************************************************************/




/*******************************************************************************
                                                                              
                                Public methods
                                                                              
*******************************************************************************/
int aoc_credit_remaining (MfwMnu* m, MfwMnuItem* i);
int aoc_charge_rate (MfwMnu* m, MfwMnuItem* i);
int aoc_view_limit (MfwMnu* m, MfwMnuItem* i);
int aoc_change_limit (MfwMnu* m, MfwMnuItem* i);
int aoc_cancel_limit (MfwMnu* m, MfwMnuItem* i);
int aoc_last_charge (MfwMnu* m, MfwMnuItem* i);
int aoc_total_charge (MfwMnu* m, MfwMnuItem* i);
int aoc_reset_charge (MfwMnu* m, MfwMnuItem* i);

USHORT		service_check_aoc	(struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi);

//x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
#ifdef FF_CPHS
void setAOCflag(int aocValue);
int getAOCflag(void);
#endif
EXTERN LONG acm_last_call; //accumulated call meter  from the last call

/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif

