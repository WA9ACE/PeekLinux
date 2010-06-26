#ifndef __ATCONFIG_H
#define __ATCONFIG_H
/****************************************************************************
 *
 * File:
 *     $Workfile:atconfig.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:9$
 *
 * Description: This file contains an AT Command Processor configuration 
 *     constants.
 *             
 * Copyright 2002-2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,  
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "config.h"

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_HEADSET  
#define AT_HEADSET          XA_DISABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_HANDSFREE
#define AT_HANDSFREE        XA_DISABLED
#endif

#if AT_HANDSFREE == XA_ENABLED
/* Headset commands must be included when using handsfree. */
#undef AT_HEADSET
#define AT_HEADSET          XA_ENABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_PHONEBOOK
#define AT_PHONEBOOK        XA_DISABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_SMS       
#define AT_SMS              XA_DISABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_DUN      
#define AT_DUN              XA_DISABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_ROLE_TERMINAL
#define AT_ROLE_TERMINAL        XA_DISABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_ROLE_MOBILE
#define AT_ROLE_MOBILE          XA_DISABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_MAX_PARMS
#define AT_MAX_PARMS        45
#endif

#if (AT_MAX_PARMS < 16) || (AT_MAX_PARMS > 255)
#error "AT_MAX_PARMS must be between 16 and 255."
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_MAX_INDICATORS
#define AT_MAX_INDICATORS   9
#endif

#if (AT_MAX_INDICATORS < 1) || (AT_MAX_INDICATORS > 255)
#error "AT_MAX_INDICATORS must be between 8 and 255."
#endif

#endif /* __ATCONFIG_H */
