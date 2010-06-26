/****************************************************************************/
/*                                                                          */
/*  Name        main.c                                                      */
/*                                                                          */
/*  Function    this file contains Application_Initialize function          */
/*              called by Nucleus environment                               */
/*  Version		0.1                                                         */
/*                                                                          */
/* 	Date       	Modification                                                */
/*  ------------------------------------                                    */
/*  18 June 2001		Create                                              */
/*                                                                          */
/*	Author		Laurent Sollier                                             */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/


/*==== INCLUDES ===================================================*/
#include "l1sw.cfg"   /* OP_L1_STANDALONE definition */
#include "swconfig.cfg"
#if (OP_L1_STANDALONE == 0)
  #include "rv.cfg"
  #include "debug.cfg"
#endif
#include "board.cfg"
#include "timer.h"
#include "l1_confg.h"
#include "types.h"
#if (OP_L1_STANDALONE == 1)
  #include "uart/uart.h"
#endif

/*==== FUNCTIONS ==================================================*/

#if (PSP_STANDALONE == 0)
	#if (OP_L1_STANDALONE == 1)
  		extern void Initialize_L1_stand_alone (void *first_available_memory);
	#else
  		extern void Cust_Init_Layer1(void);
  		extern void StartFrame(void);
	#endif
#endif /* PSP_STANDALONE == 0 */

#if (CODE_VERSION != SIMULATION)
  extern void Init_Target(void);
  extern void Init_Unmask_IT(void);
  extern void Init_Drivers(void);
  int FileSystemInitialize(void);
  unsigned char pcm_init(void);
#endif

#if (PSP_STANDALONE == 0)
	extern void Init_Serial_Flows (void);
#endif

#if (OP_L1_STANDALONE == 0)
  #if WCP_PROF == 1
  extern void prf_Init(void);
  #endif
#endif

/*
 * Application_Initialize
 *
 * Initialization Main function, called by Nucleus (INC_Initialize)
 */
Uint32 prelim_boot_time=0;
void Application_Initialize (void *first_available_memory)
{

#if (CODE_VERSION != SIMULATION)

  /*
   * Low-level HW Initialization.
   */
  Init_Target ();

  /*
   * Drivers Initialization.
   */
  Init_Drivers ();

#endif /*   (CODE_VERSION != SIMULATION) */

#if (PSP_STANDALONE == 0)
	#if (OP_L1_STANDALONE == 0)
	
		#if (_GSM==1)

	    #if (RELIANCE_FS==1 || DRP_FW_EXT ==1)
		/*
   		* Condat G2-3 SW Initialization including Frame.
   		*/
  		StartFrame ();
      #endif

      #if (RELIANCE_FS==1)
              // Initilaises Flash FX and Reliance early in the life of 
              // the system. This need to be done ehere for l1_initliaise to
              // read claibarion data. This call assumes that delay calibaration
              //info is configures in ffxconf.h
		FileSystemInitialize();

		// Initialising PCM data
		// If this is not done, voice calls
		// will not go through. Only SMS can
		// be sent and recieved.
		pcm_init();
      #endif /* RELIANCE_FS == 1*/
			  
		/*
   	   	* Layer1 SW Initialization.
   	   	*/
	  	Cust_Init_Layer1 ();

  	  	/*
   	   	* SerialSwitch & Serial Flows Initialization.
       	* WARNING!!! Must always be done after Layer1 Initialization
   	   	*/
	  	Init_Serial_Flows ();
      #endif // _GSM

  	  #else /* OP_L1_STANDALONE == 0*/
  		Init_Serial_Flows ();

	  	Initialize_L1_stand_alone (first_available_memory);
	    #endif   /* (OP_L1_STANDALONE == 0) */


	#if (OP_L1_STANDALONE == 0)

    #if (RELIANCE_FS==0 && DRP_FW_EXT==0)
		  #if (_GSM==1)
  		/*
   		* Condat G2-3 SW Initialization including Frame.
   		*/
  		StartFrame ();
  		#endif // _GSM
	  #endif

	   	#if (TEST==1) && (_GSM==0)
  		/*
	    * The Watchdog is used as a General Purpose Timer for the Nucleus Ticks when
	    * The Layer1 is not started => Test without GSM/GPRS.
	    */
	   	Init_Watchdog_Timer (); 
	   	#endif

	#endif   /* (OP_L1_STANDALONE == 0) */
#else /* PSP_STANDALONE == 1*/
       StartFrame ();

		#if (RELIANCE_FS==1)
              // Initilaises Flash FX and Reliance early in the life of 
              // the system. This need to be done ehere for l1_initliaise to
              // read claibarion data. This call assumes that delay calibaration
              //info is configures in ffxconf.h
		FileSystemInitialize();		
        #endif /* RELIANCE_FS == 1*/


    /*
   	* SerialSwitch & Serial Flows Initialization.
    * WARNING!!! Must always be done after Layer1 Initialization
   	*/
	  	Init_Serial_Flows ();
	/*
	* The Watchdog is used as a General Purpose Timer for the Nucleus Ticks when
	* The Layer1 is not started => Test without GSM/GPRS.
	*/
	Init_Watchdog_Timer (); 
#endif /* PSP_STANDALONE */
  	 	#if WCP_PROF == 1
  		/*
   		* WCP Performance Profiler, Ver.2.
   		*
  	 	* WARNING!!! Must always be done after StartFrame() because
   		* profiler uses Frame services!
   		*/
  		prf_Init();
  		#endif

	

#if (CODE_VERSION != SIMULATION)

  /*
   * Unmask used Interrupts.
   */
#if (REMU==1)
  rv_start();
#endif

#if ((OP_L1_STANDALONE == 0) && (PSP_STANDALONE == 0))
Dtimer1_Start (0);

/*Subtract the current timer value and Multiply by the PTV value and multiply by the time period of one clock tick 
 *which is 1.23 us.0xFFFF is the timer load value , so the difference is taken
 */
prelim_boot_time=(((0xFFFF)-(Dtimer1_ReadValue()))*256)*1.23;
Bsp_Boot_Time_Take_Snapshot(PRELIM_BOOT);
#endif

Init_Unmask_IT ();

#endif   /* (CODE_VERSION != SIMULATION) */

} 
