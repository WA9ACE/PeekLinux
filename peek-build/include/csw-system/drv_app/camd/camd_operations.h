/**
 * @file	camd_operations.h
 *
 * API for CAMD SWE.
 *
 * @author	raymond zandbergen
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/12/2003	raymond zandbergen (ICT)		Create.
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */


#ifndef CAMD_OPERATIONS_H
#define CAMD_OPERATIONS_H
#include "camd/camd_api.h"
#include "camd/camd_i.h"

/**
 * Switches the sensor between enabled and disabled state
 *
 * @param msg_p     pointer to a CAMD_ENABLE_SENSOR_REQ_MSG
 *
 * @return RV_OK						normal operation
 *         RV_INTERNAL_ERR				internal error occured
 *         RV_MEMORY_ERR				out of memory
 *
 */
extern T_RV_RET camd_int_enable_sensor (T_CAMD_MSG * msg_p);

/**
 *
 * Applies snapshot parameters contained in the req msg
 *
 * @param msg_p     pointer to a CAMD_SET_SNAPSHOT_PARAMETERS_REQ_MSG
 *
 * @return RV_OK						normal operation
 *         RV_INTERNAL_ERR				internal error occured
 *         RV_MEMORY_ERR				out of memory
 *
 */
extern T_RV_RET camd_int_set_snapshot_parameters (T_CAMD_MSG * msg_p);

/**
 *
 * Retrieves snapshot parameters currently in use
 *
 * @param msg_p     pointer to a CAMD_GET_SNAPSHOT_PARAMETERS_REQ_MSG
 *
 * @return RV_OK						normal operation
 *         RV_INTERNAL_ERR				internal error occured
 *         RV_MEMORY_ERR				out of memory
 *
 */
extern T_RV_RET camd_int_get_snapshot_parameters (T_CAMD_MSG * msg_p);

/**
 *
 * Takes a snapshot
 *
 * @param msg_p     pointer to a CAMD_GET_SNAPSHOT_REQ_MSG
 *
 * @return RV_OK						normal operation
 *         RV_INTERNAL_ERR				internal error occured
 *         RV_MEMORY_ERR				out of memory
 *
 */
extern T_RV_RET camd_int_get_snapshot (T_CAMD_MSG * msg_p);

/**
 *
 * Applies viewfinder parameters contained in the req msg
 *
 * @param msg_p     pointer to a CAMD_SET_VIEWFINDER_PARAMETERS_REQ_MSG
 *
 * @return RV_OK						normal operation
 *         RV_INTERNAL_ERR				internal error occured
 *         RV_MEMORY_ERR				out of memory
 *
 */
extern T_RV_RET camd_int_set_viewfinder_parameters (T_CAMD_MSG * msg_p);

/**
 *
 * Retrieves viewfinder parameters contained currently in use
 *
 * @param msg_p     pointer to a CAMD_GET_VIEWFINDER_PARAMETERS_REQ_MSG
 *
 * @return RV_OK						normal operation
 *         RV_INTERNAL_ERR				internal error occured
 *         RV_MEMORY_ERR				out of memory
 *
 */
extern T_RV_RET camd_int_get_viewfinder_parameters (T_CAMD_MSG * msg_p);

/**
 *
 * Returns a stream of viewfinder frames until a new request
 * arrives in the mailbox
 *
 * @param msg_p     pointer to a CAMD_GET_VIEWFINDER_FRAME_REQ_MSG
 *
 * @return RV_OK						normal operation
 *         RV_INTERNAL_ERR				internal error occured
 *         RV_MEMORY_ERR				out of memory
 *
 */
extern T_RV_RET camd_int_get_viewfinder_frames (T_CAMD_MSG * msg_p);



#if (CAM_SENSOR==1)
/*
 * This function is used to zoom IN or zoom OUT depending on the parameter zoom 
 * This is not implemented as a message because it leads to a overhead . 
 */

T_RV_RET camd_set_zoom(T_CAMD_ZOOM zoom);


#endif
#endif /* def CAMD_OPERATIONS_H */
