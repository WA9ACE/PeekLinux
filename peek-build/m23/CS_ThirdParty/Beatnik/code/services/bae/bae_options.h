/**
 * @file	bae_options.h
 *
 * Options defined for the BAE instance.
 *
 * @author	Richard Powell (richard@beatnik.com)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/17/2004	Richard Powell (richard@beatnik.com)		Create.
 *	12/8/2004	Richard Powell (richard@beatnik.com)		Updated for Riviera21.
 *
 * (C) Copyright 2004 by Beatnik, Inc., All Rights Reserved
 */

#ifndef __BAE_OPTIONS_H_
#define __BAE_OPTIONS_H_

#if (LOCOSTO_LITE==0)
#define BAE_LOCOSTO
#else
#define BAE_ULTRA_LOCOSTO
#endif


#include "bae_options_pvt.h"

#endif /* __BAE_OPTIONS_H_ */
