/**
 * @file	camd_state_i.h
 * 
 * Internal definitions for CAMD state machine, 
 * i.e. the states and the functions corresponding to the states.
 *
 * The messages are declared in camd_message.h, since they can 
 * be seen from outside.
 *
 * @author	raymond zandbergen (raymond.zandbergen@ict.nl)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/12/2003	raymond zandbergen (raymond.zandbergen@ict.nl)		Create.
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#ifndef __CAMD_STATE_I_H_
#define __CAMD_STATE_I_H_


#include "rv/rv_general.h"

typedef enum
{
	CAMD_SNAPSHOT_MODE,
	CAMD_VIEWFINDER_MODE,
	CAMD_CAMERA_ENABLED,
	CAMD_CAMERA_DISABLED,
	CAMD_INITIALISED,
	CAMD_UNINITIALSIED,
	CAMD_KILLED,
	CAMD_STOPPED
} T_CAMD_INTERNAL_STATE;



#endif /* __CAMD_STATE_I_H_ */
