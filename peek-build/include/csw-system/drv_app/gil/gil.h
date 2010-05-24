/**
 * @file gil.h
 *
 * Types definition for GIL.
 */

/*
 * History:
 *
 *	Date       	Author        		Modification
 *	-------------------------------------------------------------------
 *	04/03/2003	Frederic Maria 		Create.
 *
 * (C) Copyright 2004 by Texas Instruments, All Rights Reserved
 */

#ifndef __GIL_H_
#define __GIL_H_


// Sized types definition.
#include "general.h"


/// Callback function type.
typedef void (*T_GIL_CB)(void*);


/// Generic message sent to any external sotware entity.
/// Only used for mapping purpose.
typedef struct
{
    T_GIL_CB gil_cb;
    UINT32   gil_data[1];
}
T_GIL_MSG;


#endif // __GIL_H_
