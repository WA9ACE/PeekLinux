#ifndef _MMIVOICEMEMO_H_
#define _MMIVOICEMEMO_H_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		VoiceMemo
 $File:		    MmiVoiceMemo.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the start code for the MMI
                        
********************************************************************************
 $History: MmiVoiceMemo.h
 
	Mar 15, 2006   ER: OMAPS00067709  x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Added prototypes of functions pcm_voice_memo_play, pcm_voice_memo_record, 
			voice_buffering_edit_num, voice_buffering_record_start, voice_buffering_record_stop, 
			voice_buffering_playback_start
	
	Feb 24, 2006    ER: OMAPS00067709 x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Added prototypes of the functions to support PCM Voice memo feature
	   
 $End

*******************************************************************************/
EXTERN void voice_memo_init			(void);
EXTERN int 	voice_memo_play 		(MfwMnu* m, MfwMnuItem* i);
EXTERN int	voice_memo_record		(MfwMnu* m, MfwMnuItem* i);
EXTERN int voice_memo_storeage (MfwMnu* m, MfwMnuItem* i);
#ifdef FF_PCM_VM_VB
EXTERN int  pcm_voice_memo_play  (MfwMnu* m, MfwMnuItem* i);
EXTERN int  pcm_voice_memo_record	 (MfwMnu* m, MfwMnuItem* i);
EXTERN int  voice_buffering_edit_num  (MfwMnu* m, MfwMnuItem* i);
EXTERN int  voice_buffering_record_start  (MfwMnu* m, MfwMnuItem* i);
EXTERN int  voice_buffering_record_stop  (MfwMnu* m, MfwMnuItem* i);
EXTERN int PCM_voice_memo_storeage (MfwMnu* m, MfwMnuItem* i);
EXTERN int voice_buffer_storeage (MfwMnu* m, MfwMnuItem* i);
#endif
//x0pleela 24 Feb, 2006  ER OMAPS00067709
#ifdef FF_PCM_VM_VB		
EXTERN int 	pcm_voice_memo_play 		(MfwMnu* m, MfwMnuItem* i);
EXTERN int	pcm_voice_memo_record		(MfwMnu* m, MfwMnuItem* i);

//x0pleela 09 Mar, 2006  ER: OMAPS00067709
//Prototypes of functions supporting voice buffering
EXTERN int  voice_buffering_edit_num (MfwMnu* m, MfwMnuItem* i);
EXTERN int  voice_buffering_record_start (MfwMnu* m, MfwMnuItem* i);
EXTERN int  voice_buffering_record_stop (MfwMnu* m, MfwMnuItem* i);
EXTERN void voice_buffering_playback_start(void );
#endif

/*******************************************************************************
                                                                              
                                Include files
                                                                              
*******************************************************************************/

extern USHORT record_position;




/*******************************************************************************
                                                                              
                                Interface constants
                                                                              
*******************************************************************************/




/*******************************************************************************
                                                                              
                                Public methods
                                                                              
*******************************************************************************/

/* pretty standard initialise, execute and exit methods
*/



/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif

