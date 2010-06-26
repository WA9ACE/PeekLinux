/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Modul   :  LLS
+----------------------------------------------------------------------------- 
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
*/
#include "rvm/rvm_use_id_list.h"
#include "lls/lls_env.h"
#include "lls/lls_i.h"

/*==== VARS ==========================================================*/
/* global addr id */
T_LLS_ENV_CTRL_BLK* lls_env_ctrl_blk = NULL;
T_LLS_ENV_CTRL_BLK  lls_env_ctrl_blk_data;

BOOL lls_idle = FALSE;


/*
+------------------------------------------------------------------------------
| Function    : lls_init
+------------------------------------------------------------------------------
| Description : This function does basic initialization light services module
+------------------------------------------------------------------------------
*/
T_RVM_RETURN  lls_init ()
{

       lls_env_ctrl_blk=&lls_env_ctrl_blk_data;
	/* Store the pointer to the error function. */
	lls_env_ctrl_blk->error_ft = NULL;

        /* Store the mem bank id. */
        lls_env_ctrl_blk->prim_id = 0;
   
    //--------------------------------------------------------------------------------
    lls_env_ctrl_blk->swe_is_initialized = TRUE;
    if (lls_initialize() != RV_OK)
         return RV_INTERNAL_ERR;
    
    return (RV_OK);

} /* End lls_init(..) */


