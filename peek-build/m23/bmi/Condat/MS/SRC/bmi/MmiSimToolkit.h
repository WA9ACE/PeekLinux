/*******************************************************************************

					CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		    MmiSimToolkit.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the root mofule for the basic MMI
                        
********************************************************************************
 $History: MmiSimToolkit.h

	Nov 17, 2006   REF:DR:OMAPS00104580  x0039928
	Description:   CT GCF - TC27.22.4.13.1 Seq 1.10 - PROACTIVE SIM COMMANDS: 
	SET UP CALL (NORMAL) - Call Set-up Failed
	Solution:  Removed the Redial flag enabling code.
	
	25/10/00			Original Condat(UK) BMI version.	
	   
	Nov 07, 2006 ER:OMAPS00070661 R.Prabakar(a0393213)
	R99 network compliancy : Implemented Language Notification and Provide Local Information (Language setting)
	SAT proactive commands and Language Selection SAT event
	   
 $End

*******************************************************************************/

#ifndef _DEF_MMI_SAT_H_
#define _DEF_MMI_SAT_H_

#include "mfw_mfw.h"

/*
 * External Interfaces
 */

USHORT sim_toolkit_check (struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi);
extern void simToolkitInit(T_MFW_HND parentWindow);
extern void simToolkitExit(void);
void sat_add_unicode_tag_if_needed(char* string); /*SPR 1257, added new function for unicode handling*/
/* Nov 17, 2006   REF:DR:OMAPS00104580  x0039928
    -Deleted Redial flag enabling/disabling code*/

// ADDED - RAVI - 25-11-2005
void sat_release_the_editor (void);
// END - RAVI

#ifdef FF_MMI_R99_SAT_LANG
/*OMAPS00070661 a0393213(R.Prabakar) */
/*ADD ALL QUALIFIERS HERE*/
#define LOCAL_INFO_LANG_SETTING_QUALIFIER 4

#define LANG_NOTIFY_NON_SPEC_LANG_QUALIFIER 0
#define LANG_NOTIFY_SPEC_LANG_QUALIFIER 1

/*ASCII VALUES FOR LANGUAGES*/
#define ASCII_FOR_D 0x64
#define ASCII_FOR_E 0x65
#define ASCII_FOR_H 0x68
#define ASCII_FOR_N 0x6E
#define ASCII_FOR_Z 0x7A

#endif /*FF_MMI_R99_SAT_LANG*/
#endif  /* _DEF_MMI_SAT_H_   */

