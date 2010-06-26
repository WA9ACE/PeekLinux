/**
 * @file
 *
 * @brief Utility functions prototypes for AS SWE.
 *
 * This file contains the prototypes of miscellaneous utility functions.
 * All these functions are private to the AS SWE.
 */

/*
 * History:
 *
 *	Date       	Author			Modification
 *	-------------------------------------------------------------------
 *	1/25/2005	f-maria@ti.com		Create.
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __AS_UTILS_H_
#define __AS_UTILS_H_


#include "as/as_api.h"

#include "rv/rv_general.h"
#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#endif

/// Adapt the behavior of the current player according to the GPRS mode.
extern T_AS_RET
as_adapt_gprs_mode(void);

/// Enter the AS recover mode.
extern void
as_enter_recover_mode(UINT32 msg_id, const T_RV_RETURN_PATH* rp);

/// Create the ringer configuration directory.
extern T_AS_RET
as_ringer_create_dir(void);

#if(AS_RFS_API == 1)
/// Read the file and update the ringer information.
extern T_AS_RET
as_ringer_read_file(const T_WCHAR*       filename,
                    T_AS_RINGER_INFO* info);

/// Write the file with the given ringer information.
extern T_AS_RET
as_ringer_write_file(const T_WCHAR*             filename,
                     const T_AS_RINGER_INFO* info);
#else

/// Read the file and update the ringer information.
extern T_AS_RET
as_ringer_read_file(const char*       filename,
                    T_AS_RINGER_INFO* info);

/// Write the file with the given ringer information.
extern T_AS_RET
as_ringer_write_file(const char*             filename,
                     const T_AS_RINGER_INFO* info);
#endif

/// Send an AS_START_IND message back to the client.
extern void
as_send_start_ind(T_AS_RET                status,
                  BOOLEAN                 interrupted,
                  const T_RV_RETURN_PATH* rp);

/// Send an AS_STOP_IND message back to the client.
extern void
as_send_stop_ind(T_AS_RET                status,
                 BOOLEAN                 end_of_media,
                 const T_RV_RETURN_PATH* rp);

/// Send an AS_PAUSE_IND message back to the client.
//AS_PAUSE_IND for BT
extern void
as_send_pause_ind(T_AS_RET                status,
                 BOOLEAN                 end_of_media,
                 const T_RV_RETURN_PATH* rp);
/// Send an AS_INTERNAL_STOP_REQ message to the AS.
extern void
as_send_internal_stop_req(void);


#endif // __AS_UTILS_H_
