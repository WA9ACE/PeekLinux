#ifndef __RADIOMGI_H
#define __RADIOMGI_H

/***************************************************************************
 *
 * File:
 *     $Workfile:radiomgi.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:15$
 *
 * Description:
 *     This file contains internal definitions for the Radio Manager.
 *
 * Created:
 *     April 11, 2003
 *
 * Copyright 2003-2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "bttypes.h"
      
/*--------------------------------------------------------------------------*
 * Typedefs and Data structures used internally by Radio Manager            *
 *--------------------------------------------------------------------------*/
 
/* Radio Manager States */
#define RMGR_STATE_SHUTDOWN             0
#define RMGR_STATE_SHUTTING_DOWN        1
#define RMGR_STATE_RESETTING            2
#define RMGR_STATE_INITIALIZING         3
#define RMGR_STATE_INITIALIZING_RADIO   4
#define RMGR_STATE_READY                5

/* Radio Manager context */
typedef struct _BtRmgContext {
    U8              retries;          /* Number of retries during init */
    U8              rmState;          /* Radio manager state           */
} BtRmgContext;

#endif /* __RADIOMGI_H */

