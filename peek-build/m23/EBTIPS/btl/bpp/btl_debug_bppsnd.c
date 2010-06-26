#ifndef __BTL_OBEX_UTILS_H
#define __BTL_OBEX_UTILS_H

#include "bttypes.h"
#include "obex.h"

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

#endif	/* __BTL_OBEX_UTILS_H */

