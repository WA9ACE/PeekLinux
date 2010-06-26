/****************************************************************************/
/*                                                                          */
/*  File Name:  ffs_api.h                                                   */
/*                                                                          */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date                Modification                                        */
/*  ------------------------------------                                    */
/*  28 January   2002   Create                                              */
/*                                                                          */
/*  Author       Stephanie Gerthoux                                         */
/*                                                                          */
/* (C) Copyright 2002 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#ifndef _FFS_API_H_
#define _FFS_API_H_


   #ifdef _WINDOWS
      #include "ffs/pc/ffs_pc_api.h"
   #else
      #include "ffs/ffs.h"
   #endif      


/******************************************************************************
    * Types
 ******************************************************************************/

   #define FFS_OBJECT_TYPE   T_FFS_OBJECT_TYPE      

#endif
