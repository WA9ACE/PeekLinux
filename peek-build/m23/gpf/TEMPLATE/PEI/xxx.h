/* 
+------------------------------------------------------------------------------
|  File:       xxx.h
+------------------------------------------------------------------------------
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  Definitions for the Protocol Stack Entity xxx.
+----------------------------------------------------------------------------- 
*/ 

#ifndef XXX_H
#define XXX_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/

#define MY_NAME          "XXX"
#define VSI_CALLER       XXX_handle,
#define ENTITY_DATA      xxx_data

#define ENCODE_OFFSET    0             /* Bitoffset for encoding/decoding */
#define hCommYYY         xxx_hCommYYY  /* Communication handle */

/* Timer definitions */
#define T001             0
#define T002             1

/* Timer durations */
#define T001_VALUE       1000          /* 1 second */
#define T002_VALUE       3000          /* 3 seconds */

/* make the pei_create function unique */
#define pei_create       xxx_pei_create

#ifdef XXX_PEI_C
const static T_STR_IND tmr_name_to_ind[] = 
{
  { "T001",   T001 },
  { "T002",   T002 },
  { NULL,     0 }
};
#endif

/*==== TYPES =================================================================*/

/* XXX global typedefs */

typedef struct                         /* T_XXX_DATA */
{
  unsigned char version;
/*
 * entity parameters
 */
} T_XXX_DATA;

/*==== EXPORTS ===============================================================*/

short pei_create (T_PEI_INFO **info);

#ifdef XXX_PEI_C

/* Entity data base */
T_XXX_DATA               xxx_data_base;
T_XXX_DATA               *xxx_data;

/* Communication handles */
T_HANDLE                 hCommYYY = VSI_ERROR;
T_HANDLE                 hCommXXX = VSI_ERROR;
T_HANDLE                 XXX_handle;

#else  /* XXX_PEI_C */

extern T_XXX_DATA        xxx_data_base, *xxx_data;
extern T_HANDLE          hCommYYY;
extern T_HANDLE          XXX_handle;

#endif /* XXX_PEI_C */

#endif /* !XXX_H */
