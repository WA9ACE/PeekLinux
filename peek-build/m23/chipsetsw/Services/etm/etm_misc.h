/********************************************************************************
 * Enhanced TestMode (ETM)
 * @file	etm_misc.h 
 *
 * @author	Kim T. Peteren (ktp@ti.com)
 * @version 0.1
 *
 * $Id: etm.c 1.22 Mon, 28 Apr 2003 15:35:40 +0200 ktp $

 *
 * History:
 *
 * 	Date       	Modification
 *  ------------------------------------
 *  16/06/2003	Creation
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 *********************************************************************************/


#ifndef _ETM_MISC_H_
#define _ETM_MISC_H_


/******************************************************************************
 * Usefull Header 
 *****************************************************************************/

#ifndef _WINDOWS
#include "swconfig.cfg"
#include "sys.cfg"
#include "chipset.cfg"
//#include "testmode.cfg"
#endif

#include "l1sw.cfg"

#include "l1_confg.h"

/* Riviera useful header */
#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"
#include "rvf/rvf_target.h"
#include "rvf/rvf_api.h"
#include "rvt/rvt_gen.h"


/* include the usefull L1 header */
#ifdef _WINDOWS
#define BOOL_FLAG
#define CHAR_FLAG
#endif

#include "l1_types.h"
#include "l1audio_const.h"
#include "l1audio_cust.h"
#include "l1audio_defty.h"
#include "l1audio_msgty.h"
#include "l1audio_signa.h"

#if TESTMODE
#include "l1tm_defty.h"
#endif      

#if (L1_GTT == 1)
#include "l1gtt_defty.h"
#endif      

#if (L1_MP3 == 1)
  #include "l1mp3_defty.h"
#endif
#if (L1_AAC == 1)
  #include "l1aac_defty.h"
#endif
#include "l1_const.h"
#include "l1_defty.h"
#include "l1_msgty.h"
#include "l1_signa.h"

#ifdef _WINDOWS
#define L1_ASYNC_C
#endif

#include "l1_varex.h"


/******************************************************************************
 * Defines 
 *****************************************************************************/

#define TM3_PACKET_SIZE 128
#define ETM_PACEKT_SIZE 256

// This is the return TM3 packet structure
// HACK! - taken from l1tm_msgty.h
typedef struct 
{
    unsigned char cid;
    unsigned char index;
    unsigned char status;
    unsigned char size; // size of result[] array
    unsigned char result[TM3_PACKET_SIZE];
} T_TM3_PKT; 


// This is the new TM packet structure
typedef struct
{
    unsigned short size; // size of data[]
    int index;           // index into data[] - for put()/get() functions
    unsigned char  mid;
    unsigned char  status;
    unsigned char  data[ETM_PACEKT_SIZE]; // include cksum
} T_ETM_PKT;


#ifndef BASIC_TYPES
#define BASIC_TYPES
typedef signed   char  int8;
typedef unsigned char  uint8;
typedef signed   short int16;
typedef unsigned short uint16;
typedef signed   int   int32;
typedef unsigned int   uint32;
#endif

/******************************************************************************
 * Prototypes 
 *****************************************************************************/

void *etm_malloc(int size);
int etm_free(void *addr);

int etm_at(T_ETM_PKT *pkt, char *buf);

/******************************************************************************
 * Internal Error codes 
 *****************************************************************************/

enum {

    ETM_TASK_REGISTERED = -100,
    ETM_DB_LIMIT        = -101

};


#endif // _ETM_MISC_H_
