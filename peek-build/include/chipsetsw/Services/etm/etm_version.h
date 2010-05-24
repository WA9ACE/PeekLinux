/********************************************************************************
 * Enhanced TestMode (ETM) - Revision Controle System Header
 * @file	etm_version.h 
 *
 * @author	Kim T. Peteren (ktp@ti.com)
 * @version 0.1
 *
 *
 * History:
 *
 * 	Date       	Modification
 *  ------------------------------------
 *  21/10/2003	Creation
 * 
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 *********************************************************************************/

#ifndef _ETM_VERSION_H_
#define _ETM_VERSION_H_


/******************************************************************************
 * Enhanced TestMode version numbers 
 *****************************************************************************/

#define ETM_VERSION      0x0105L // Resolved compiler warnings.
//#define ETM_VERSION      0x0104L // added target ready signal
//#define ETM_VERSION      0x0103L // Fixed issue regarding interaction with the ETM registration database.
//#define ETM_VERSION      0x0102L // Updated ETM task state machine, removed recption of ATP events. 
//#define ETM_VERSION      0x0100L // First Version


#define ETM_API_VERSION  0x0100L // First Version


/******************************************************************************
 * Version of ETM CORE  
 *****************************************************************************/

#define ETM_CORE_VERSION 0x0107L // Updated dieID to support 128 bit dieID.
//#define ETM_CORE_VERSION 0x0106L // Resolved compiler warnings.
//#define ETM_CORE_VERSION 0x0105L // Updated codec funtions to fullfill TRITON interface.
//#define ETM_CORE_VERSION 0x0104L // ETM AT command interpreter is updated a lot.
//#define ETM_CORE_VERSION 0x0103L // Version integrated full support of mpr, mpw, tms etc.
//#define ETM_CORE_VERSION 0x0102L // Updated etm_version() for TCS3.x branch
//#define ETM_CORE_VERSION 0x0101L // First Version


/******************************************************************************
 * Version of ETM AUDIO  
 *****************************************************************************/

#define ETM_AUDIO_VERSION 0x0107L // Changed _ES first two bytes from 16 bits to 8 bits 
//#define ETM_AUDIO_VERSION 0x0106L // Resolved compiler warnings.
//#define ETM_AUDIO_VERSION 0x0105L // Integrated tone generator interface
//#define ETM_AUDIO_VERSION 0x0104L // Integrated support of Locosto acoustics algorithm.
//#define ETM_AUDIO_VERSION 0x0103L // Integrated support of ECHO SUPPRESSOR
//#define ETM_AUDIO_VERSION 0x0102L // Implemented callback function instead of msg handling.
//#define ETM_AUDIO_VERSION 0x0101L // Version updated with support of new AUDIO parameters - not yet fully tested
//#define ETM_AUDIO_VERSION 0x0100L // First Version

#endif // End of ETM_VERSION
