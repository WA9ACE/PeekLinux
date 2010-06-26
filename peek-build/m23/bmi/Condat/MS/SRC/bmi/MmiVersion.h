
#ifndef _DEF_MMI_VERSION_H_
#define _DEF_MMI_VERSION_H_
/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		    MmiVersion.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    

                        
********************************************************************************

 $History: MmiVersion.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/


/****************************************************************************************
 *
 * These strings are set during compile time and describe the current version
 *
 ****************************************************************************************
 */
#ifdef BUSER
#define BUILD_USER BUSER
#else
#define BUILD_USER "UNKNOWN"
#endif

#ifdef BTIME
#define BUILD_TIME BTIME
#else
#define BUILD_TIME "UNKNOWN"
#endif

#ifdef BDATE
#define BUILD_DATE BDATE
#else
#define BUILD_DATE "UNKNOWN"
#endif

#endif //_DEF_MMI_VERSION_H_
