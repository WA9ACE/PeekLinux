
#include "btl_config.h"
#include "btl_defs.h"
#include "btl_obex_utils.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BTL_COMMON);

BtStatus BTL_UTILS_ObStatus2BtStatus(ObStatus obStatus)
{	
	switch (obStatus)
	{
	    case OB_STATUS_SUCCESS: 			return BT_STATUS_SUCCESS;
	    case OB_STATUS_FAILED:			return BT_STATUS_FAILED;
	    case OB_STATUS_PENDING:			return BT_STATUS_PENDING;
	    case OB_STATUS_DISCONNECT:		return BT_STATUS_FAILED;
	    case OB_STATUS_NO_CONNECT:		return BT_STATUS_NO_CONNECTION;
	    case OB_STATUS_MEDIA_BUSY:		return BT_STATUS_INTERNAL_ERROR;	/* Only IrDA - Shouldn't receive that */
	    case OB_STATUS_INVALID_PARM:		return BT_STATUS_INVALID_PARM;
	    case OB_STATUS_INVALID_HANDLE:	return BT_STATUS_INTERNAL_ERROR;
	    case OB_STATUS_PACKET_TOO_SMALL:return BT_STATUS_FAILED;
	    case OB_STATUS_BUSY:				return BT_STATUS_BUSY;
		default:							return BT_STATUS_INTERNAL_ERROR;
	};
}

/*---------------------------------------------------------------------------
 *            BTL_OBEX_UTILS_ConvertFsErrorToObexError
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Convert FS error to OBEX error
 *
 * Return:    OBRC_xxx Error Code
 *
 */ 
ObexRespCode BTL_OBEX_UTILS_ConvertFsErrorToObexError(BtFsStatus btFsStatus)
{
	ObexRespCode rcode;
    char *pStr;

    switch(btFsStatus)
	{
	
    case BT_STATUS_HAL_FS_ERROR_FILE_HANDLE:            /* Bad file number/ handle */

    case BT_STATUS_HAL_FS_ERROR_GENERAL:                /* No more processes or not enough memory 
                                                           or maximum nesting level reached */
                                                        /* No spawned processes */
                                                        /* Resource deadlock would occur */
                                                        /* Math argument */
                                                        /* Too many open files */
                                                        /* Exec format error */
                                                        /* Result too large */
                                                        /* Cross-device link */
                                                        /* Not enough memory */
    
    case BT_STATUS_HAL_FS_ERROR_EXISTS:                 /* File exists */
        
    case BT_STATUS_HAL_FS_ERROR_INVALID:                /* Invalid argument */

    case BT_STATUS_HAL_FS_ERROR_FILE_NOT_CLOSE:         /* File open */
		pStr = "DATABASE LOCKED";
		rcode = OBRC_DATABASE_LOCKED;        
		break;
    
    case BT_STATUS_HAL_FS_ERROR_NOTFOUND:               /* No such file or directory */
		pStr = "FILE/or DIR NOT FOUND";
		rcode = OBRC_NOT_FOUND;                   
		break;

	case BT_STATUS_HAL_FS_ERROR_OUT_OF_SPACE:
		pStr = "DATABASE FULL";
		rcode = OBRC_DATABASE_FULL;                     /* No space left on device */
		break;

	case BT_STATUS_HAL_FS_ERROR_ACCESS_DENIED:          /* Permission denied */
	case BT_STATUS_HAL_FS_ERROR_DIRECTORY_NOT_EMPTY:	/* Permission denied */
	default:
		pStr = "UNAUTHORIZED";
		rcode = OBRC_UNAUTHORIZED;
		break;
	}

	BTL_LOG_ERROR(("Error access to file - %s\n", pStr));		
	return rcode;
}

/*---------------------------------------------------------------------------
 *            BTL_OBEX_UTILS_StartTimer
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Start a timer after obex Push or Pull operation is executed.
 *
 * Return:    None
 */
void BTL_OBEX_UTILS_StartTimer(EvmTimer *timer, TimeT time, EvmTimerNotify handler, void *context)
{
	timer->func = handler;
	timer->context = context;

	BTL_LOG_DEBUG(("OBEX timer is started."));
	EVM_StartTimer(timer, time);
}


/*---------------------------------------------------------------------------
 *            BTL_OBEX_UTILS_CancelTimer
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Cancel the obex Push or Pull operation timer.
 *
 * Return:    None
 */
void BTL_OBEX_UTILS_CancelTimer(EvmTimer* timer)
{
	BTL_LOG_DEBUG(("OBEX timer is stopped."));
	EVM_CancelTimer(timer);
}


