/**
 * @file	secure_rom.h
 *
 * IMEI protection using secure ROM services.
 *
 * @author	 ()
 * @version 0.2
 */

/*
 * History:
 *
 *	Date		Author					Modification
 *	-------------------------------------------------------------------
 *	4/19/2004	Heino Juvoll Madsen		Create.
 *
 * (C) Copyright 2004 by TI, All Rights Reserved
 */

#ifndef __SECURE_ROM_H_
#define __SECURE_ROM_H_

#if (CHIPSET == 12) || (CHIPSET == 15)

#define C_IMEISV_BYTES     8
#define C_IMEISV_SIZE      2

typedef enum
  {
    C_IMEI_OK               =  0,
    C_IMEI_ERROR            = -1,
    C_IMEI_WRONG_DIE_ID     = -2,
    C_IMEI_READ_IMEI_FAILED = -3
  } E_IMEI_STATUS;

/*
+------------------------------------------------------------------------------
| Function    : securerom_drv
+------------------------------------------------------------------------------
| Description : The function uses secure ROM services to check the validity of
|               the IMEI-SV.
|
| Parameters  : inBufSize  - size of buffer where to store IMEI, min. 8 byte
|               *outBufPtr - pointer to buffer where to store the IMEI
| Return      :              0 - OK
|                           <0 - ERROR
+------------------------------------------------------------------------------
*/
BYTE securerom_drv (USHORT inBufSize, UBYTE *outBufPtr);

#endif /* (CHIPSET == 12) || (CHIPSET == 15) */

#endif /*__SECURE_ROM_H_*/
