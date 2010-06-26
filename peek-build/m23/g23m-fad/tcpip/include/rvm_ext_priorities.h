/*************************************************************************************
 *                                                                                   *
 *  Name        rvm_ext_priorities.h                                                 *
 *                                                                                   *
 *  Function    this file priorities defintitions for basic SWEs               *
 *        It includes a file
 *                                                                                   *
 *  Version    0.1                                       *
 *                                               *
 *   Date         Modification                                 *
 *  ------------------------------------                           *
 *  10/11/2000  Create                                     *
 *                                               *
 *  Author    Cristian Livadiotti (c-livadiotti@ti.com)                   *
 *                                               *
 * (C) Copyright 2000 by Texas Instruments Incorporated, All Rights Reserved         *
 *                                                                                   *
 * --------------------------------------------------------------------------------- *
 *                                                                                   *
 *   History:                                                                        *
 *                                                                                   *
 *   10/18/2001 - Updated for R2D by Christophe Favergeon                         *
 *                                                                                   *
 *************************************************************************************/

#ifndef __RVM_EXT_PRIORITIES_H_
#define __RVM_EXT_PRIORITIES_H_

/* PRIORITIES SETTING:                        */
/* All user priority should be set under 250: */
/* Higher values are reserved                 */

/*
** External Priority definitions
*/
#define RVM_EXPL_TASK_PRIORITY (240)
#define RVM_OBX_TASK_PRIORITY (240)
#define RVM_OPP_TASK_PRIORITY (240)
#define RVM_SYN_TASK_PRIORITY (240)
#define RVM_FTP_TASK_PRIORITY (240)
#define RVM_RGUI_TASK_PRIORITY (240)
#define RVM_R2D_TASK_PRIORITY (239)

#define RVM_DEV1_TASK_PRIORITY (245)
#define RVM_DEV2_TASK_PRIORITY (245)
#define RVM_DEV3_TASK_PRIORITY (245)

#define RVM_UVM_TASK_PRIORITY (245)

#endif /* __RVM_EXT_PRIORITIES_H_ */

