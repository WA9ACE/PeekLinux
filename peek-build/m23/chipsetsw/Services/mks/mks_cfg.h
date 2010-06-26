/**
 * @file   mks_cfg.h
 *
 * Configuration file for MKS service.
 *
 * This file is part of the MKS API.
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  11/19/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _MKS_CFG_H_
#define _MKS_CFG_H_



/** Define the number max of magic key sequence.
 */
#define MKS_NB_MAX_OF_KEY_SEQUENCE (5)

/** Define the number max of key in a magic key sequence.
 */
#define MKS_NB_MAX_OF_KEY_IN_KEY_SEQUENCE (10)

/** Define the number max of notified key for post sequence.
 */
#define MKS_NB_MAX_OF_KEY_FOR_POST_SEQUENCE (10)

/** Define time to wait (ms) between two keys before the key sequence reset.
 */
#define MKS_TIME_RESET_KEY_SEQUENCE 3000

/** Define the maximum size for key sequence name
 */
#define KPD_MAX_CHAR_NAME 20

#endif /* _MKS_CFG_H_ */
