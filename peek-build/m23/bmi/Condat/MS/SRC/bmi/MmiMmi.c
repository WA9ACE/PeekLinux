/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   MMI
 $File:       MmiMmi.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/10/00

********************************************************************************

 Description:

    This provides the root mofule for the basic MMI

********************************************************************************
 $History: MmiMmi.c

      Apr 09, 2007 OMAPS00124879 x0039928(sumanth)
      Description : I sample crashes while reading the received SMS from the Agilent
      Solution     : Heap memory is increased from 35k to 45k
      
 	Oct 05, 2006    ER: OMAPS00094496 x0061088(Prachi)
 	Description:Enhance RSSI to 3 parameters: Strength, quality, min-access-level
 	Solution:To solve the ER OMAPS0094496,mmiInit() is now calling sAT_PercentCSQ()
 
 

	Jun 13 2005,	REF:LOCOSTO-OTH-32113	-	xpradipg
	Description:	Bypass the powerkey initialization at bootup
	Solution:		The powerkey initialization is bypassed by calling the
					drvKeyUpDown() in mmi_main() instead of key_power_initialize

//  	Jun 23, 2004        	REF: CRR MMI-SPR-16665  Rashmi C N(Sasken)
// 	Description:		Ocasionaly the LCD used to get stuck with the TI LOGO during power off.
//	Solution:			By calling dspl_Enable(1) we are forcing the LCD to get updated.

//  	July 05, 2006        	REF: OMAPS00084167  Ravishankar(Wipro)
// 	Description:	Automatic bootup with no effect on press of power on key.
//	Solution:			Checking the status of BDL operation being completed for booting up MMI
//                by using the function dev_Mbox_isDspMboxReady()and based on status calling
//                mmiInit().


  25/10/00      Original Condat(UK) BMI version.         

 $End

*******************************************************************************/




/*******************************************************************************

                                Include Files

*******************************************************************************/

#define ENTITY_MFW

/* includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif
#include "mfw_sys.h"

#include "prim.h"


#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
#include "mfw_lng.h"
#include "mfw_tim.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_phb.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_mme.h"
#include "mfw_sat.h"
#include "mfw_sms.h"

#include "cus_aci.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif

#include "ksd.h"
#include "psa.h"


#include "MmiMmi.h"
#include "MmiMain.h"

#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"

/* Retrieve PM duration from FlashData structure */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
#include "mfw_ffs.h"
#endif
#endif

#include "dspl.h"
#include "drv_key.h"

#ifdef FF_MMI_MULTIMEDIA
/* To Assign the MSL_Init Return Value */
#include "MSL.h"
#include "mmiMultimediaApp.h"
#include "omxutil.h"
/* Bug Fix - OMAPS00084167 */
/* Added - To access the funtion dev_Mbox_isDspMboxReady () */
#include "typedefs.h"
#include "main_system.h"
#include "dev_Mbox.h"  /* 04-07-2006 */
#include "mfw_acie.h"   /* prototypes of aci_delete and aci_create */
#endif

#ifdef MMI_LITE
#define MFW_MEMORY_POOL_SIZE 22000 /*SPR 2686*/
#else
	#ifdef FF_MMI_MMS
	#define MFW_MEMORY_POOL_SIZE (220*1024)
        #else
		#ifdef FF_MMI_MULTIMEDIA
			#define MFW_MEMORY_POOL_SIZE (220 * 1024)   // RAVI - 18-07-2005 - For Image display
		#else
	#define MFW_MEMORY_POOL_SIZE 45000
	#endif
#endif
#endif


#ifndef _SIMULATION_
extern void AI_Power(unsigned char power);
#endif  //_SIMULATION_

/*******************************************************************************

                                Global Data Elements

*******************************************************************************/

U16 mmiScrX, mmiScrY;                   /* display sreen size       */

static U8 mfwMem [MFW_MEMORY_POOL_SIZE];      /* mfw memory pool          */
/* x0039928 - Lint warning fix
static int initialized = 0;              */

/* Begin Bug Fix - OMAPS00084167 */
/* Added for Primitive exchange */
#ifdef ENABLE_KEY_ON_BOOTING
#define hCommACI _ENTITY_PREFIXED(hCommACI)

#if defined (NEW_FRAME)
EXTERN T_HANDLE hCommACI;
#else
EXTERN T_VSI_CHANDLE hCommACI;
#endif

#define BOOTUP_WAIT_TIME    20
#define BOOTUP_SUCCESS_IND  1
#define MFW_BOOTUP_IND      0x910 

static int iPrimitiveSlot = 0;

/* ====================================================== */
/**
* Structure which holds the Status details 
**/
/* ====================================================== */
typedef struct
{
      int  iStatus;
}T_MFW_BOOTUP_IND;

GLOBAL BOOL bootup_response_cb (ULONG opc, void * data);
#endif

extern BOOL powered_on;

/* End - OMAPS00084167 */



/*******************************************************************************

                                Public Methods

*******************************************************************************/

/*******************************************************************************

 $Function:     mmi_main

 $Description:  entry point for the MMI, this hands control to mmiInit

 $Returns:    None.

 $Arguments:  None.

*******************************************************************************/
extern void emo_printf(const char *fmt, ...);

EXTERN BOOL appdata_response_cb (ULONG opc, void * data);
static int pSlot;

void mmi_main( void )
{
    /* JVJ #1874 - mmi_main doesnt start the MMI. It only initilizes the Keyboard handler*/
    /* The keyboard handler will call mmiInit() when the Power Key is pressed */
//    	Jun 13 2005,	REF:LOCOSTO-OTH-32113	-	xpradipg
#if (BOARD == 61)  
/* BUG FIX - OMAPS00084167 */
/* For multimedia alone generate the primitive */
#ifdef ENABLE_KEY_ON_BOOTING
      /* Send the primitive to MMI */
     PALLOC (bootup_indication, MFW_BOOTUP_IND);
#endif
/* END - OMAPS00084167 */

#if FF_MMI_SERVICES_MIGRATION
	   drvKeyUpDown(0,0);
#else
	 emo_printf("mmi_main() keypower init called\n");
	 key_power_init();
#endif


#ifndef ENABLE_KEY_ON_BOOTING
	vsi_t_sleep(VSI_CALLER 50);
	mmiInit();	 
#else
      /* Begin - Bug fix - OMAPS00084167 */   
      iPrimitiveSlot = aci_create (bootup_response_cb, NULL); 

     bootup_indication->iStatus  = BOOTUP_SUCCESS_IND;
     PSENDX (ACI, bootup_indication);
     /* End - OMAPS00084167 */
#endif


#else  /* Other Builds */
     emo_printf("mmi_main() keypower init called\n");

     pSlot = aci_create(appdata_response_cb,NULL);

     emo_printf("mmi_main() aci_create() returned = %d", pSlot);
     key_power_init();
#endif

}
/* Begin - Bugfix - OMAPS00084167 */
#ifdef ENABLE_KEY_ON_BOOTING
/* ===========================================================*/
/**
* bootup_response_cb () -  response function.
*
* @param ULONG opc  - Operation Code.
*
* @param void * data - structure data.
*
*  @return BOOL  - TRUE / FALSE
*
*  @see         
*/
/* =========================================================== */
GLOBAL BOOL bootup_response_cb (ULONG opc, void * data)
{
       T_MFW_BOOTUP_IND *bootup_indication;
	   
        if (opc == MFW_BOOTUP_IND) {
              bootup_indication = (T_MFW_BOOTUP_IND *)data;

              /* Status is 1 then bootup sequence has enabled */ 
              if (bootup_indication->iStatus == BOOTUP_SUCCESS_IND) {
                   /* Contineous Pooling */ 
                   while (dev_Mbox_isDspMboxReady() == FALSE)
                    {
                         vsi_t_sleep (VSI_CALLER BOOTUP_WAIT_TIME); /* Small Delay after each check */
                    }  

                   aci_delete (iPrimitiveSlot);
                   
                   /* Once DSP initialization is done */
                   mmiInit ();                  
                   
                   return TRUE;

              }
	   
        }

	return FALSE;

}
#endif /* End - OMAPS00084167 */



/*******************************************************************************

 $Function:     mmiInit

 $Description:  This initialises the MMI application

 $Returns:    None.

 $Arguments:  None.

*******************************************************************************/

void mmiInit( void )
{
#ifdef FF_MMI_MULTIMEDIA
   unsigned int uiResult = 0;
#endif

  /* get the resources running
  */

  /* Initialise the MFW and windowing system
  */
  emo_printf("mfwInit() got called\n");
  mfwInit( mfwMem,   sizeof( mfwMem ) );
  winInit( &mmiScrX, &mmiScrY );


  timInit();                          /* init timer handler       */
  kbdInit();                          /* init keyboard handler    */
  lngInit();                          /* init language handler    */
  #ifndef NEW_EDITOR				/* SPR#1428 - SH - New Editor changes */
  edtInit();                          /* init editor handler      */
  #endif /* NEW_EDITOR */
 
  mnuInit();                          /* init menu handler        */
  icnInit();                          /* init icon handler        */
  //Oct 05, 2006    ER: OMAPS00094496 x0061088(Prachi)
  sAT_PercentCSQ ( CMD_SRC_LCL, CSQ_Enable );
  mainInit(0);
#ifdef FF_MMI_MULTIMEDIA
    /* MSL Init Call */
   uiResult = MSL_Init();
   vsi_t_sleep(0, 100);
   mmi_setInitResult(uiResult);

/* Power management. Enable/Disable on boot up based on value stored*/
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
    if(FFS_flashData.pm_duration)
    {
        SVC_SCN_EnablePM( 1, FFS_flashData.pm_duration);
    }
#endif
#endif
   /* PCM Setup Done */
/*  MSLRM_Start ();  */
#endif

}






/*******************************************************************************

 $Function:     mmiExit

 $Description:  This exits the MMI application

 $Returns:    None.

 $Arguments:  None.

*******************************************************************************/





void mmiExit( void )
{
    TRACE_EVENT("mmiExit");

  icnExit();                          /* finit icon handler       */
  mnuExit();                          /* finit menu handler       */
  #ifndef NEW_EDITOR				/* SPR#1428 - SH - New Editor changes */
  edtExit();                          /* finit edit handler       */
  #endif /* NEW_EDITOR */
  lngExit();                          /* finit language handler   */
  kbdExit();                          /* finit keyboard handler   */
  timExit();                          /* finit timer handler      */

  winExit();                          /* finit window handler     */
  mfwExit();                          /* exit mfw                 */

  dspl_ClearAll();
//  	Jun 23, 2004        	REF: CRR MMI-SPR-16665  Rashmi C N(Sasken)
// 	Description:		Ocasionaly the LCD used to get stuck with the TI LOGO during power off.
//	Solution:			By calling dspl_Enable(1) we are forcing the LCD to get updated.
  dspl_Enable(1);
  TRACE_EVENT("SWITCHING_OFF!!!!!");
  vsi_t_sleep (VSI_CALLER 50);
  /*
   *  Power off the board
   *
   */
#ifndef _SIMULATION_
  AI_Power(0);
#endif   //_SIMULATION_

}




/*******************************************************************************

                                End Of File

*******************************************************************************/

