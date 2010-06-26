#ifndef __BTL_OBEX_UTILS_H
#define __BTL_OBEX_UTILS_H

#include "bttypes.h"
#include "obex.h"
#include "bthal_fs.h"

/*---------------------------------------------------------------------------
 * BTL_UTILS_ObStatus2BtStatus()
 *
 *     Converts an ObStatus to a BtStatus.
 *
 * Parameters:
 *     addr - Binary device address to convert.
 *
 * Returns:
 *     char * - A static internal string that contains the ASCII version
 */
BtStatus BTL_UTILS_ObStatus2BtStatus(ObStatus obStatus);


/*---------------------------------------------------------------------------
 * BTL_OBEX_UTILS_ConvertFsErrorToObexError()
 *
 *     Convert FS error to OBEX error
 *
 * Parameters:
 *     btFsStatus - FS Error code
 *
 * Returns:
 *     ObexRespCode - OBRC_xxx Error Code
 */
ObexRespCode BTL_OBEX_UTILS_ConvertFsErrorToObexError(BtFsStatus btFsStatus);


/*---------------------------------------------------------------------------
 * BTL_OBEX_UTILS_StartTimer()
 *
 *     Start a timer when obex profile execute obex operation.
 *
 * Parameters:
 *     time [in] - The value of timer in milliseconds.
 *
 *     obexTimeoutHandler [in] - handler when the timer expired
 *
 *     context - the obex profile context 
 *
 * Returns:
 *     None
 */
void BTL_OBEX_UTILS_StartTimer(EvmTimer *timer, TimeT time, EvmTimerNotify handler, void *context);


/*---------------------------------------------------------------------------
 *            BTL_OBEX_UTILS_CancelTimer
 *
 * Synopsis:  Cancel the start obex operation timer.
 *
 * Return:    None
 */
void BTL_OBEX_UTILS_CancelTimer(EvmTimer* timer);


#endif	/* __BTL_OBEX_UTILS_H */

