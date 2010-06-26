/**
 * @file	secure_rom.c
 *
 * IMEI protection using secure ROM services.
 *
 * @author	 ()
 * @version 0.3
 */

/*
 * History:
 *
 *  Date        Author                  Modification
 *  -------------------------------------------------------------------
 *  2004-04-19  Heino Juvoll Madsen     Create.
 *  2004-11-15  Heino Juvoll Madsen     Support for four different IMEI
 *                                      protection solutions added.
 *
 * (C) Copyright 2004 by TI, All Rights Reserved
 */

#include "chipset.cfg"

#if (CHIPSET == 12) || (CHIPSET == 15)

#include "security/global_types.h"
#include "security/secure_types.h"
#include "mpk/mpk_api.h"

#include "swconfig.cfg"

#include "secure_rom.h"

/*
 *  External variable declaration of d_firmware_certificate
 */
extern volatile const T_MANUFACTURER_CERTIFICATE_PLATFORM_DATA d_firmware_certificate;

/*
+------------------------------------------------------------------------------
| Function    : securerom_drv
+------------------------------------------------------------------------------
| Description : The function uses secure ROM services to check the validity of
|               the IMEI-SV.
|
| Parameters  : inBufSize  - size of buffer where to store IMEI, min. 8 bytes
|               *outBufPtr - pointer to buffer where to store the IMEI
| Return      :              0 - OK
|                           <0 - ERROR
+------------------------------------------------------------------------------
*/
BYTE securerom_drv (USHORT inBufSize, UBYTE *outBufPtr)
{
  UWORD8                  d_i;
  UWORD8                  a_hash_mpk[MPK_SIZE]; /* Hash(MAN_PUB_KEY) */
  UWORD32                *p_platform_data;
  UWORD32                *p_imeisv;
  BOOLEAN                 b_security;
  BOOLEAN                 b_die_id_not_zero;
  BOOLEAN                 b_imeisv_not_zero;

  /*
   *  Secure ROM Services set-up
   */
  E_SECURE_STATUS                           d_status;
  T_SSERVICE_UNBIND                         d_unbind;
  T_ROM_SSERVICE                            pf_ROM_Sservice;
  T_SSERVICE_RUNTIME_PLAT_CHECKER           d_platform_data_checker;
  T_PLATFORM_CERTIFICATE                   *p_platform_certificate;
  T_MANUFACTURER_CERTIFICATE_PLATFORM_DATA *p_firmware_certificate;

  /*
   *  If inBufSize is smaller than C_IMEISV_BYTES then the length of outBufPtr
   *  is too small to contain IMEI-SV.
   */
  if (inBufSize < C_IMEISV_BYTES)
    return C_IMEI_READ_IMEI_FAILED;

  /*
   *  Initialization
   */
  pf_ROM_Sservice = (T_ROM_SSERVICE) C_ROM_SSERVICE_ENTRY_POINT_ADDR;
  p_firmware_certificate = (T_MANUFACTURER_CERTIFICATE_PLATFORM_DATA *) &d_firmware_certificate;

  /*
   *  If the hashing of MAN_PUB_KEY is zero then the target is non-secure.
   */
  mpk_get_mpk_id(a_hash_mpk);

  b_security = FALSE;
  for (d_i = 0; d_i < MPK_SIZE; d_i++)
    if (a_hash_mpk[d_i] != 0)
    {
      b_security = TRUE;
      break;
    }

  /*
   *  Check the 8 bytes IMEI-SV from the beginning of the PLATFORM_DATA field
   *  of the manufacturer certificate.
   */
  b_imeisv_not_zero = FALSE;
  for (d_i = 0; d_i < C_IMEISV_SIZE; d_i++)
    if (p_firmware_certificate->a_platform_data[d_i] != 0)
    {
      b_imeisv_not_zero = TRUE;
      break;
    }

  /*
   *  Check the DIE_ID field of the manufacturer certificate.
   */
  b_die_id_not_zero = FALSE;
  for (d_i = 0; d_i < C_DIE_ID_SIZE; d_i++)
    if (p_firmware_certificate->d_manufacturer_certificate.a_die_id[d_i] != 0)
    {
      b_die_id_not_zero = TRUE;
      break;
    }

  if (b_security && b_imeisv_not_zero)
  {
    if (b_die_id_not_zero)
    {
      /*
       *  Solution 1 - DIE != 0 & IMEISV != 0 in manufacturer certificate.
       */
      d_platform_data_checker.p_Cert = (T_PLATFORM_CERTIFICATE *) NULL;
    }
    else {
      /*
       *  Solution 2 - DIE == 0 & IMEISV != 0 in manufacturer certificate.
       */

      /*
       *  Assign the address of the platform certificate.
       */
#if 0
      d_platform_data_checker.p_Cert = (T_PLATFORM_CERTIFICATE *) p_firmware_certificate + sizeof(T_MANUFACTURER_CERTIFICATE_PLATFORM_DATA);
#else
      d_platform_data_checker.p_Cert = (T_PLATFORM_CERTIFICATE *) IMEI_PLATFORM_CERTIFICATE_ADDRESS;
#endif
    }

    /*
     *  Run-Time Platform Data Checker Service.
     */
    d_status = pf_ROM_Sservice(C_SEC_SERVICE_RUN_TIME_PLATFORM_DATA_CHECKER,
                               (T_SSERVICE_RUNTIME_PLAT_CHECKER *) &d_platform_data_checker,
                               NULL);

    if (d_status == C_SECURE_INVALID_DIE_ID)
      return C_IMEI_WRONG_DIE_ID;
    else if (d_status != C_SECURE_SUCCESS)
      return C_IMEI_ERROR;

    /*
     *  Assign the address of the PLATFORM_DATA field.
     */
    p_platform_data = (UWORD32 *) p_firmware_certificate + sizeof(T_MANUFACTURER_CERTIFICATE) / sizeof(UWORD32);

    /*
     *  Read 8 bytes IMEI-SV from the beginning of the PLATFORM_DATA field of
     *  the manufacturer certificate.
     */
    for (d_i = 0; d_i < 32; d_i += 8)
      outBufPtr[d_i / 8] = (UWORD8) (*p_platform_data >> (24 - d_i));

    p_platform_data++;

    for (d_i = 0; d_i < 32; d_i += 8)
      outBufPtr[sizeof(UWORD32) + d_i / 8] = (UWORD8) (*p_platform_data >> (24 - d_i));
  }
  else
  {
    /*
     *  Assign the address of the platform certificate.
     */
#if 0
    if (b_security)
    {
      /*
       *  Solution 3 - DIE == 0 & IMEISV == 0 in manufacturer certificate.
       */
      p_platform_certificate = (T_PLATFORM_CERTIFICATE *) p_firmware_certificate + sizeof(T_MANUFACTURER_CERTIFICATE_PLATFORM_DATA);
    }
    else
#endif
      /*
       *  Solution 0 - Non-secure, i.e., manufacturer certificate is not used.
       */
      p_platform_certificate = (T_PLATFORM_CERTIFICATE *) IMEI_PLATFORM_CERTIFICATE_ADDRESS;

    /*
     *  Assign the address of the IMEI-SV which is placed right after the
     *  platform certificate in flash.
     */
    p_imeisv = (UWORD32 *) p_platform_certificate + sizeof(T_PLATFORM_CERTIFICATE) / sizeof(UWORD32);

    /*
     *  Run-Time Loader (Unbinding) Secure ROM Service.
     */
    d_unbind.p_Cert       = p_platform_certificate;
    d_unbind.p_Code       = p_imeisv;
    d_unbind.p_CertCtx    = 0L;
    d_unbind.p_UnbindCtx  = 0L;
    d_unbind.d_Steplength = 0L;
    d_unbind.b_start      = TRUE;

    d_status = pf_ROM_Sservice(C_SEC_SERVICE_UNBINDING_ID,
                               (T_SSERVICE_UNBIND *) &d_unbind,
                               NULL);

    if (d_status != C_SECURE_SUCCESS)
      return C_IMEI_ERROR;

    /*
     *  Read 8 bytes IMEI-SV.
     */
    for (d_i = 0; d_i < 32; d_i += 8)
      outBufPtr[d_i / 8] = (UWORD8) (*p_imeisv >> (24 - d_i));

    p_imeisv++;

    for (d_i = 0; d_i < 32; d_i += 8)
      outBufPtr[sizeof(UWORD32) + d_i / 8] = (UWORD8) (*p_imeisv >> (24 - d_i));
  }

  return C_IMEI_OK;
}

#endif /* (CHIPSET == 12) || (CHIPSET == 15) */
