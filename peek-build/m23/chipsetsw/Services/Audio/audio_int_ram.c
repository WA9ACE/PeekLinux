/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_int_ram.c                                             */
/*                                                                          */
/*  Purpose:    This function contains the global variables that are        */
/*              stored into the internal RAM                                */
/*              ----------------------------                                */
/*																	                                      	*/
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date                 Modification                                       */
/*  ------------------------------------                                    */
/*  10 November 2003     Create                                             */
/*                                                                          */
/*  Author     Stephanie Levieil                                            */
/*                                                                          */
/* (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"
#ifdef RVM_AUDIO_SWE
	/* include the usefull config file */
	#include "config/l1sw.cfg"

	#if (L1_EXT_AUDIO_MGT==1)

		#include "rv/rv_general.h"
    #include "Audio/audio_api.h"
	
		
    /**** Global variables ****/
		/* Ram buffer that contains melody datas that are send to the DMA*/
   // UINT16 audio_play_buffer[AUDIO_EXT_MIDI_BUFFER_SIZE*2];
      
	#endif
#endif /* #ifdef RVM_AUDIO_SWE */

