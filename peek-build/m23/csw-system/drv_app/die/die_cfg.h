/**
 * @file	die_cfg.h
 *
 * Configuration definitions for the DIE instance.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	5/30/2003	 ()		Create.
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#ifndef __DIE_CFG_H_
#define __DIE_CFG_H_
#include "chipset.cfg"

/* Software version */
#if (CHIPSET==15)
#define DIE_MAJOR   1
#define DIE_MINOR   0
#define DIE_BUILD   0
#else
#define DIE_MAJOR   0
#define DIE_MINOR   2
#define DIE_BUILD   0
#endif

#endif /* __DIE_CFG_H_ */
