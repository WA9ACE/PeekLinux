/**
 * @file
 *
 * @brief Utility functions implementation for AS SWE.
 *
 * This file contains the definition of miscellaneous utility functions.
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


#include "as/as_api.h"
#include "as/as_i.h"
#include "as/as_message.h"
#include "as/as_midi.h"

#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#endif
#include "ffs/ffs_api.h"


#include <string.h>


/**
 * This private function is intended to perform any adaptation to
 * the current player (whatever its type: MIDI, MP3, ...).
 * This adaptation is performed according to the current state of
 * the GPRS mode.
 *
 * For instance, on a MIDI player, this function dynamically changes
 * the number of simultaneous MIDI voices:
 * - 16 in GPRS mode,
 * - the value defined through player_set_params(AS_PLAYER_TYPE_MIDI),
 *   which normally is 24 for a ringer, and 32 for a MMI play.
 *
 * @noparam
 *
 * @retval AS_OK  The operation was successful.
 * @retval AS_PLAYER_ERR  It was impossible to change the player
 *                        parameters.
 */
T_AS_RET
as_adapt_gprs_mode(void)
{
    T_AS_RET rc = AS_OK;

    switch (as_ctrl_blk->player_type)
    {
#ifdef AS_OPTION_MIDI
    case AS_PLAYER_TYPE_MIDI:
        rc = as_midi_set_voice_limit();
        break;
#endif

    // case AS_PLAYER_TYPE_MP3:
    default:
        // Nothing to do.
        break;
    }

    return rc;
}


/**
 * This private function enters the recover mode by setting the
 * recover variable with the received message identifier and the client
 * return path with the return path from the received message.
 *
 * Then it trace an error telling the reason why the AS is entering in
 * recover mode.
 *
 * @param msg_id @in The received message identifier.
 * @param rp     @in The return path from the received message.
 *
 * @noreturn
 */
void
as_enter_recover_mode(UINT32 msg_id, const T_RV_RETURN_PATH* rp)
{
    as_ctrl_blk->recover = msg_id;
    as_ctrl_blk->client_rp = *rp;

    AS_TRACE_2(ERROR, "enter_recover_mode(): call flow error, message %08x received in state %d",
               msg_id, as_ctrl_blk->state);
}


/**
 * This private function checks if the '/aud/ringer' directory exists.
 * If not, it will create it.
 *
 * @noparam
 *
 * @retval AS_OK  The operation was successful.
 * @retval AS_FFS_ERR  It was impossible to create the directory.
 */
T_AS_RET
as_ringer_create_dir(void)
{
#if(AS_RFS_API == 1)

	T_WCHAR mp_uc11[15];
	T_WCHAR mp_uc22[30];
	char str1[] = "/FFS/aud";
	char str2[] = "/FFS/aud/ringer";
	T_RFS_RET res;
	convert_u8_to_unicode(str1, mp_uc11);
	convert_u8_to_unicode(str2, mp_uc22);

	// Create the '/aud' directory.
    res = rfs_mkdir(mp_uc11,RFS_IRWXU);

    if ((res != RFS_EOK) && (res != RFS_EEXISTS))
    {
        AS_TRACE_1(ERROR,
                   "ringer_create_dir() error %d: unable to create '/aud'.",
                   res);
        return (T_AS_RET)AS_FFS_ERR;
    }

    // Create the '/aud/ringer' directory.
    res = rfs_mkdir(mp_uc22,RFS_IRWXU);

    if ((res != RFS_EOK) && (res != RFS_EEXISTS))
    {
        AS_TRACE_1(ERROR,
                   "ringer_create_dir() error %d: unable to create '/aud/ringer'.",
                   (T_FFS_RET)res);
        return (T_AS_RET)AS_FFS_ERR;
    }

    return AS_OK;
#else
    // Create the '/aud' directory.
    T_FFS_RET res = ffs_mkdir("/aud");

    if ((res != EFFS_OK) && (res != EFFS_EXISTS))
    {
        AS_TRACE_1(ERROR,
                   "ringer_create_dir() error %d: unable to create '/aud'.",
                   res);
        return (T_AS_RET)AS_FFS_ERR;
    }

    // Create the '/aud/ringer' directory.
    res = ffs_mkdir("/aud/ringer");

    if ((res != EFFS_OK) && (res != EFFS_EXISTS))
    {
        AS_TRACE_1(ERROR,
                   "ringer_create_dir() error %d: unable to create '/aud/ringer'.",
                   res);
        return (T_AS_RET)AS_FFS_ERR;
    }

    return AS_OK;
#endif
}


/**
 * This private function performs the following operations:
 * - opens and reads the given file containing the ringer mode information.
 * - fills the ringer information with the read data.
 * - if the file does not exist or there is a read error, the ringer
 *   information is filled with default values (MEDIUM volume and BUZZER type).
 * - closes the file.
 *
 * @param filename @in  The name of the file to read.
 * @param info     @out A pointer to the buffer to fill with
 *                      the ringer information.
 *
 * @retval AS_OK  The operation was successful.
 * @retval AS_FFS_ERR  It was impossible to read the ringer information.
 *                     Default values are used.
 */

#if(AS_RFS_API == 1)
T_AS_RET
as_ringer_read_file(const T_WCHAR*    filename,
                    T_AS_RINGER_INFO* info)
#else

T_AS_RET
as_ringer_read_file(const char*       filename,
                    T_AS_RINGER_INFO* info)
#endif
{

#if(AS_RFS_API == 1)
	T_RFS_FD fd;
#else
	T_FFS_FD fd;
#endif
#if(AS_RFS_API == 1)
	T_WCHAR mp_uc1[50];
	char str1[] = "/FFS/Btk_TstContent/IMY_01.imy";
#endif
	T_AS_RET rc        = AS_OK;
#if(AS_RFS_API == 1)
		convert_u8_to_unicode(str1, mp_uc1);
#endif


    // Open the file in read mode.
#if(AS_RFS_API == 1)
	 fd = rfs_open(filename, RFS_O_RDONLY, 0x0777);
#else
	 fd = ffs_open(filename, FFS_O_RDONLY);
#endif

    if (fd == RFS_ENOENT)
    {
        // File not found, do nothing.
        // It will be created during write operation.
    }
    else
    {
        if (fd >= 0)
        {
#if(AS_RFS_API == 1)
			T_RFS_SIZE r_size = rfs_read(fd, (void*) info, sizeof(T_AS_RINGER_INFO));
#else
			// OK file was correctly opened. try to read it.
            T_FFS_SIZE r_size = ffs_read(fd, (void*) info, sizeof(T_AS_RINGER_INFO));
#endif

            if (r_size != sizeof(T_AS_RINGER_INFO))
            {
                AS_TRACE_2(ERROR,
                           "ringer_read_file() error/read_size %d: unable to read ringer info from file '%s'.",
                           r_size, filename);
                rc = (T_AS_RET)AS_FFS_ERR;
            }
            else
            {
                AS_DEBUG_1(LOW,
                           "ringer_read_file() ringer info successfully read from file '%s'.",
                           filename);
            }

#if(AS_RFS_API == 1)
			rfs_close(fd);
#else
			ffs_close(fd);
#endif
        }
        else
        {
            AS_TRACE_2(ERROR,
                       "ringer_read_file() error %d: unable to open file '%s'.",
                       fd, filename);
            rc = (T_AS_RET)AS_FFS_ERR;
        }
    }

    // If an error occurred, use default values.
    if (rc != AS_OK)
    {
#if (AS_OPTION_BUZZER == 1)
        AS_TRACE_0(WARNING,
                   "ringer_read_file() using default volume=MEDIUM, player=BUZZER");
        info->player_type   = AS_PLAYER_TYPE_BUZZER;
        info->volume        = AS_VOLUME_MEDIUM;
        info->data_location = AS_LOCATION_FILE;
        info->data.filename[0] = '\0';
#else
        AS_TRACE_0(WARNING,
                   "ringer_read_file() using default volume=MEDIUM, player=MIDI, file=/Btk_TstContent/IMY_01.imy");
        info->player_type   = AS_PLAYER_TYPE_MIDI;
        info->volume        = AS_VOLUME_MEDIUM;
        info->data_location = AS_LOCATION_FILE;
#if(AS_RFS_API == 1)
		wstrcpy(info->data.filename, mp_uc1);
#else
		strcpy(info->data.filename, "/Btk_TstContent/IMY_01.imy");
#endif
#endif // AS_OPTION_BUZZER
    }

    return rc;
}


/**
 * This private function performs the following operations:
 * - opens the given file.
 * - writes the ringer information to the file.
 * - closes the file.
 *
 * @param filename @in The name of the file to write.
 * @param info     @in A pointer to the buffer to write to the given file.
 *
 * @retval AS_OK  The operation was successful.
 * @retval AS_FFS_ERR  It was impossible to write the ringer information.
 */

#if(AS_RFS_API == 1)
T_AS_RET
as_ringer_write_file(const T_WCHAR*  filename,
                     const T_AS_RINGER_INFO* info)
#else
T_AS_RET
as_ringer_write_file(const char*             filename,
                     const T_AS_RINGER_INFO* info)
#endif
{

#if(AS_RFS_API == 1)
	T_RFS_FD fd;
#else
	T_FFS_FD fd;
#endif
	T_AS_RET rc = AS_OK;

#if(AS_RFS_API == 1)
        if(as_ctrl_blk->ringer_file_created == FALSE)
	as_ringer_create_dir(); //  added to avoid RFS initialization problem
#endif

#if(AS_RFS_API == 1)
	 fd = rfs_open(filename, RFS_O_WRONLY | RFS_O_CREAT, 0x0777);
#else
	 fd = ffs_open(filename, FFS_O_WRONLY | FFS_O_CREATE);
#endif

    if (fd >= 0)
    {
#if(AS_RFS_API == 1)
		T_RFS_SIZE w_size = rfs_write(fd, (void*) info, sizeof(T_AS_RINGER_INFO));
#else
		T_FFS_SIZE w_size = ffs_write(fd, (void*) info, sizeof(T_AS_RINGER_INFO));
#endif

        if (w_size != sizeof(T_AS_RINGER_INFO))
        {
            AS_TRACE_2(ERROR,
                       "ringer_write_file() error/writen_size %d: unable to write ringer info to file '%s'.",
                       w_size, filename);
            rc = (T_AS_RET)AS_FFS_ERR;
        }
        else
        {
            AS_DEBUG_1(LOW,
                       "ringer_write_file() ringer info successfully written to file '%s'.",
                       filename);
        }

#if(AS_RFS_API == 1)
		rfs_close(fd);
#else
		ffs_close(fd);
#endif
        if(as_ctrl_blk->ringer_file_created == FALSE)
	{
           // Read the configuration files for each supported ringer mode.
	   // Do not take error into account. The as_ringer_read_file()
           // function sets default values if file is not available.
           (void) as_ringer_read_file(as_ctrl_blk->filename[AS_RINGER_MODE_IC],
                               &as_ctrl_blk->ringer[AS_RINGER_MODE_IC]);
           (void) as_ringer_read_file(as_ctrl_blk->filename[AS_RINGER_MODE_SMS],
                               &as_ctrl_blk->ringer[AS_RINGER_MODE_SMS]);
           (void) as_ringer_read_file(as_ctrl_blk->filename[AS_RINGER_MODE_ALARM],
                               &as_ctrl_blk->ringer[AS_RINGER_MODE_ALARM]);
           
	   as_ctrl_blk->ringer_file_created = TRUE;
	}
    }
    else
    {
        AS_TRACE_2(ERROR,
                   "ringer_write_file() error %d: unable to open file '%s'.",
                   fd, filename);
        rc = (T_AS_RET)AS_FFS_ERR;
    }

    return rc;
}


/**
 * This private function performs the following operations:
 * - allocates a AS_START_IND message buffer from the SWE memory bank.
 * - fills the message using given parameters.
 * - sends the message according to the given return path.
 *
 * @param status      @in The status to reply to client SWE.
 * @param interrupted @in Whether the current START request interrupted
 *                        a previous one.
 * @param rp          @in The return path to use to reply the message.
 *
 * @noreturn
 *
 * @see Asynchronous response of as_play_file(), as_play_memory(), as_ring().
 */
void
as_send_start_ind(T_AS_RET                status,
                  BOOLEAN                 interrupted,
                  const T_RV_RETURN_PATH* rp)
{
    T_AS_START_IND* msg;
    T_RVF_MB_STATUS mb_status;

    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_START_IND),
                            (T_RVF_BUFFER **) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status != RVF_RED)
    {
        if (mb_status == RVF_YELLOW)
        {
            AS_TRACE_L(WARNING,
                       "send_start_ind() memory usage reached the threshold, mb_id=",
                       as_ctrl_blk->mb_external);
        }

        // Fill the message.

        // System information.
        msg->header.msg_id       = AS_START_IND;
        msg->header.src_addr_id  = as_ctrl_blk->addr_id;
        msg->header.dest_addr_id = rp->addr_id;

        // Ringer start ind parameters.
        msg->status      = status;
        msg->interrupted = interrupted;

        // Use return path to send back the indication.
        if (rp->callback_func != NULL)
        {
            // Send the message using callback function.
            rp->callback_func((void *) msg);

            AS_DEBUG_0(LOW,
                       "send_start_ind() successfully called callback function.");

            // Free the allocated message.
            if (rvf_free_buf(msg) != RVF_OK)
            {
                AS_TRACE_0(WARNING,
                           "send_start_ind() unable to free message.");
            }
        }
        else
        {
            // Send the message using the addess id.
            if (rvf_send_msg(rp->addr_id, msg) != RVF_OK)
            {
                AS_TRACE_0(ERROR, "send_start_ind() unable to send message.");
                rvf_free_buf(msg);
            }
            else
            {
                AS_DEBUG_0(LOW, "send_start_ind() successfully sent message.");
            }
        }
    }
    else
    {
        AS_TRACE_L(ERROR,
                   "send_start_ind() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
    }
}


/**
 * This private function performs the following operations:
 * - allocates a AS_STOP_IND message buffer from the SWE memory bank.
 * - fills the message using given parameters.
 * - sends the message according to the given return path.
 *
 * @param status       @in The status to reply to client SWE.
 * @param end_of_media @in A boolean telling if the message is sent due
 *                         to the reach of the end of melody.
 * @param rp           @in The return path to use to reply the message.
 *
 * @noreturn
 *
 * @see Asynchronous response of as_stop().
 */
void
as_send_stop_ind(T_AS_RET                status,
                 BOOLEAN                 end_of_media,
                 const T_RV_RETURN_PATH* rp)
{
    T_AS_STOP_IND*  msg;
    T_RVF_MB_STATUS mb_status;

    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_STOP_IND),
                            (T_RVF_BUFFER **) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status != RVF_RED)
    {
        if (mb_status == RVF_YELLOW)
        {
            AS_TRACE_L(WARNING,
                       "send_stop_ind() memory usage reached the threshold, mb_id=",
                       as_ctrl_blk->mb_external);
        }

        // Fill the message.

        // System information.
        msg->header.msg_id       = AS_STOP_IND;
        msg->header.src_addr_id  = as_ctrl_blk->addr_id;
        msg->header.dest_addr_id = rp->addr_id;

        // Ringer stop ind parameters.
        msg->status       = status;
        msg->end_of_media = end_of_media;
	 msg->file_size = as_ctrl_blk->player_data.memory.size; //Daisy tang added for Real Resume feature 20071107
        // Use return path to send back the indication.
        if (rp->callback_func != NULL)
        {
            // Send the message using callback function.
            rp->callback_func((void *) msg);

            AS_DEBUG_0(LOW, "send_stop_ind() successfully called callback function.");

            // Free the allocated message.
            if (rvf_free_buf(msg) != RVF_OK)
            {
                AS_TRACE_0(WARNING,
                           "send_stop_ind() unable to free message.");
            }
        }
        else
        {
            // Send the message using the addess id.
            if (rvf_send_msg(rp->addr_id, msg) != RVF_OK)
            {
                AS_TRACE_0(ERROR, "send_stop_ind() unable to send message.");
                rvf_free_buf(msg);
            }
            else
            {
                AS_DEBUG_0(LOW, "send_stop_ind() successfully sent message.");
            }
        }
    }
    else
    {
        AS_TRACE_L(ERROR,
                   "send_stop_ind() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
    }
}


/**
 * This private function performs the following operations:
 * - allocates a AS_PAUSE_IND message buffer from the SWE memory bank.
 * - fills the message using given parameters.
 * - sends the message according to the given return path.
 *
 * @param status       @in The status to reply to client SWE.
 * @param end_of_media @in A boolean telling if the message is sent due
 *                         to the reach of the end of melody.
 * @param rp           @in The return path to use to reply the message.
 *
 * @noreturn
 *
 * @see Asynchronous response of as_stop().
*/
/*AS_PAUSE_IND for BT */
void
as_send_pause_ind(T_AS_RET                status,
                 BOOLEAN                 end_of_media,
                 const T_RV_RETURN_PATH* rp)
{
    T_AS_STOP_IND*  msg;
    T_RVF_MB_STATUS mb_status;

    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_STOP_IND),
                            (T_RVF_BUFFER **) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status != RVF_RED)
    {
        if (mb_status == RVF_YELLOW)
        {
            AS_TRACE_L(WARNING,
                       "send_stop_ind() memory usage reached the threshold, mb_id=",
                       as_ctrl_blk->mb_external);
        }

        // Fill the message.

        // System information.
        msg->header.msg_id       = AS_PAUSE_IND;
        msg->header.src_addr_id  = as_ctrl_blk->addr_id;
        msg->header.dest_addr_id = rp->addr_id;

        // Ringer stop ind parameters.
        msg->status       = status;
        msg->end_of_media = end_of_media;

        // Use return path to send back the indication.
        if (rp->callback_func != NULL)
        {
            // Send the message using callback function.
            rp->callback_func((void *) msg);

            AS_DEBUG_0(LOW, "send_pause_ind() successfully called callback function.");

            // Free the allocated message.
            if (rvf_free_buf(msg) != RVF_OK)
            {
                AS_TRACE_0(WARNING,
                           "send_pause_ind() unable to free message.");
            }
        }
        else
        {
            // Send the message using the addess id.
            if (rvf_send_msg(rp->addr_id, msg) != RVF_OK)
            {
                AS_TRACE_0(ERROR, "send_pause_ind() unable to send message.");
                rvf_free_buf(msg);
            }
            else
            {
                AS_DEBUG_0(LOW, "send_pause_ind() successfully sent message.");
            }
        }
    }
    else
    {
        AS_TRACE_L(ERROR,
                   "send_pause_ind() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
    }
}


/**
 * This private function performs the following operations:
 * - allocates a AS_INTERNAL_STOP_REQ message buffer from the SWE memory bank.
 * - sends the message to the AS.
 *
 * This message is used internally to notify the AS that the BAE player reached
 * the end of the current played media.
 * Because this end of media is notified via a callback that is called from a
 * different task context than AS, this message is intended to be sent to
 * the AS to really stop the current player.
 *
 * @noparam
 *
 * @noreturn
 */
void
as_send_internal_stop_req(void)
{
    T_AS_INTERNAL_STOP_REQ*  msg;
    T_RVF_MB_STATUS mb_status;

    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_INTERNAL_STOP_REQ),
                            (T_RVF_BUFFER **) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status != RVF_RED)
    {
        if (mb_status == RVF_YELLOW)
        {
            AS_TRACE_L(WARNING,
                       "send_internal_stop_req() memory usage reached the threshold, mb_id=",
                       as_ctrl_blk->mb_external);
        }

        // Fill the message.

        // System information.
        msg->header.msg_id       = AS_INTERNAL_STOP_REQ;
        msg->header.src_addr_id  = as_ctrl_blk->addr_id;
        msg->header.dest_addr_id = as_ctrl_blk->addr_id;

        // Send the message using the addess id.
        if (rvf_send_msg(as_ctrl_blk->addr_id, msg) != RVF_OK)
        {
            AS_TRACE_0(ERROR, "send_internal_stop_req() unable to send message.");
            rvf_free_buf(msg);
        }
        else
        {
            AS_DEBUG_0(LOW,
                       "send_internal_stop_req() successfully sent message.");
        }
    }
    else
    {
        AS_TRACE_L(ERROR,
                   "send_internal_stop_req() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
    }
}
