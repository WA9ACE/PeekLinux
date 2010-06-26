#ifndef _MMIPCM_H_
#define _MMIPCM_H_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Memory Configurations
 $File:		    MmiPcm.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    Types definitions for the permanent memory configuration
    (MMI Specific extensions)
                        
********************************************************************************
 $History: MmiPcm.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/




/*******************************************************************************
                                                                              
   Field Identifier
                                                                              
*******************************************************************************/

#define PCM_LOG_ID      "NMLOG"         /* Network Log Identifier   */
#define PCM_LOG_SIZE	27            




/*******************************************************************************
                                                                              
   Field Network Log
                                                                              
*******************************************************************************/

typedef struct EFnmlog                  /* Network Log              */
{
    UBYTE status;                       /* status                   */
    UBYTE plmn_name[20];                /* plmn name                */
    UBYTE network_name [6];             /* plmn name numeric        */
} EF_NMLOG;
                            


/*******************************************************************************
                                                                              
   Logging Definitions
                                                                              
*******************************************************************************/

#define NM_LOG_NOT_AVAIL        0
#define NM_LOG_AVAIL_NOT_USED   1
#define NM_LOG_AVAIL_USED       2




/*******************************************************************************
                                                                              
   End of File
                                                                              
*******************************************************************************/

#endif

