/**
 * @file	gil_gpf.c
 *
 * Generic Interface Layer implementation.
 */

/*
 * History:
 *
 *	Date       	Author        		Modification
 *	-------------------------------------------------------------------
 *	08/18/2004	Frederic Maria 		Create.
 *
 * (C) Copyright 2004 by Texas Instruments, All Rights Reserved
 */


#include "gil/gil_gpf.h"


/**
 * This function is intended to be called by ACI upon receipt of a
 * MMI_GIL_IND primitive.
 * Data associated with this primitive is of type T_MMI_GIL_IND which contains
 * the callback function and the opaque associated data to pass as unique
 * argument.
 *
 * @param prim  @in  The primitive message.
 *
 * @noreturn
 */
void gil_gpf_dispatch_message(void* prim)
{
    T_GIL_MSG* msg = (T_GIL_MSG*) prim;

    // Simply invoke the callback function using the associated CAMA data.
    msg->gil_cb(msg->gil_data);

    // Now, the primitive used to send the message can be freed.
    gil_gpf_free_primitive(prim);
}
