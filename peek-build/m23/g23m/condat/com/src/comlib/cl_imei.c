/*
+-----------------------------------------------------------------------------
|  Project :  COMLIB
|  Modul   :  cl_imei.c
+-----------------------------------------------------------------------------
|  Copyright 2002 Texas Instruments Berlin, AG
|                 All rights reserved.
|
|                 This file is confidential and a trade secret of Texas
|                 Instruments Berlin, AG
|                 The receipt of or possession of this file does not convey
|                 any rights to reproduce or disclose its contents or to
|                 manufacture, use, or sell anything it may describe, in
|                 whole, or in part, without the specific written consent of
|                 Texas Instruments Berlin, AG.
+-----------------------------------------------------------------------------
|  Purpose :  Definitions of common library functions: IMEI decryption with
              DES algorithm
+-----------------------------------------------------------------------------
*/
/*
 *  Version 1.0
 */

/**********************************************************************************/

/*
NOTE:
*/

/**********************************************************************************/

#ifndef CL_IMEI_C
#define CL_IMEI_C

#include "typedefs.h"
#include "vsi.h"        /* to get a lot of macros */

#ifndef _SIMULATION_
#include "ffs/ffs.h"
#include "chipset.cfg"
#include "board.cfg"
#include "memif/mem.h"
#include "pcm.h"
#endif /* ifdef SIMULATION */

#include "cl_imei.h"
#include "cl_des.h"
#include <string.h>

#undef FF_PROTECTED_IMEI
#if defined(CL_IMEI_CALYPSO_PLUS_PLATFORM) && defined(FF_PROTECTED_IMEI)
#include "secure_rom/secure_rom.h"
#endif


#if defined( CL_IMEI_CALYPSO_PLATFORM) OR defined(CL_IMEI_CALYPSO_PLUS_PLATFORM) OR !defined(CL_IMEI_LOCOSTO_PLATFORM)

static UBYTE stored_imei[CL_IMEI_SIZE]; /* when the imei is read once, the value
                                             is stored in this buffer */

static UBYTE imei_flag = 0;  /* this flag indicates, if IMEI was successful read
                                and is  stored in the stored_imei buffer */
#endif /*CL_IMEI_CALYPSO_PLATFORM OR CL_IMEI_CALYPSO_PLUS_PLATFORM	*/
#if !defined (CL_IMEI_CALYPSO_PLUS_PLATFORM)
/* Default IMEISV for D-Sample 00440000-350-111-20 */
const  UBYTE C_DEFAULT_IMEISV_DSAMPLE[CL_IMEI_SIZE] =
             {0x00, 0x44, 0x00, 0x00, 0x35, 0x01, 0x11, 0x20};
#define CL_IMEI_FFS_PATH   "/gsm/imei.enc"
#endif /* CL_IMEI_CALYPSO_PLATFORM */

#ifdef CL_IMEI_CALYPSO_PLUS_PLATFORM
/* Default IMEISV for E-Sample 00440000-351-222-30 */
const  UBYTE C_DEFAULT_IMEISV_ESAMPLE[CL_IMEI_SIZE] =
             {0x00, 0x44, 0x00, 0x00, 0x35, 0x12, 0x22, 0x30};
#endif  /* CL_IMEI_CALYPSO_PLUS_PLATFORM */

/*==== FUNCTIONS ==================================================*/
#ifdef FF_PROTECTED_IMEI

#ifdef CL_IMEI_CALYPSO_PLATFORM
/*
+------------------------------------------------------------------------------
| Function    : get_dieID
+------------------------------------------------------------------------------
| Description : the function reads the Die-ID from base band processor and
|               extracts it from 4 BYTEs to 8 BYTEs.
|
| Parameters  : inBufSize  - size of buffer where to store Die ID, min.8 BYTE
|               *outBufPtr - pointer to buffer where to store the Die ID
| Return      : void
+------------------------------------------------------------------------------
*/
LOCAL void get_dieID(USHORT inBufSize, UBYTE *outBufPtr)
{
  int i;
  USHORT *outBuf16 = (USHORT*)&outBufPtr[0];
  volatile USHORT *reg_p = (USHORT *) CL_IMEI_DIE_ID_REG;

  TRACE_FUNCTION("get_dieID()");

  if(inBufSize < CL_IMEI_DIE_ID_SIZE){
    TRACE_ERROR("CL IMEI ERROR: buffer size for Die ID to short!");
  }
#ifdef IMEI_DEBUG
  TRACE_EVENT_P1("CL IMEI INFO: Die-ID address(0x%x)", CL_IMEI_DIE_ID_REG);
#endif
  for (i = 0; i < CL_IMEI_DIE_ID_SIZE; i++) {
    /* Die ID is 4 BYTE long, extract it to 8 BYTE. */
    outBuf16[i] = (USHORT)(*(UINT8*)(reg_p)++);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : ffs_get_imeisv
+------------------------------------------------------------------------------
| Description : the function reads IMEI from FFS
|
| Parameters  : inBufSize  - size of buffer where to store IMEI, min. 8 BYTE
|               *outBufPtr - pointer to buffer where to store the IMEI
| Return      :              0 - OK
|                           <0 - ERROR
+------------------------------------------------------------------------------
*/
LOCAL BYTE ffs_get_imeisv (USHORT inBufSize, UBYTE *outBufPtr)
{
  UBYTE i;
  UBYTE isdid_buf[CL_IMEI_ISDID_SIZE];
  UBYTE r_dieId[CL_DES_KEY_SIZE]; /* read Die ID */
  UBYTE d_dieId[CL_DES_KEY_SIZE]; /* deciphered Die ID */
  SHORT ret;

  TRACE_FUNCTION("ffs_get_imeisv()");

  if(inBufSize < CL_IMEI_SIZE){
    TRACE_ERROR("CL IMEI ERROR: buffer size for IMEI to short!");
    return CL_IMEI_ERROR;
  }

  /*
   * Read ISDID(enciphered IMEISV+DieID) from FFS.
   */
  if((ret = ffs_file_read(CL_IMEI_FFS_PATH, isdid_buf, CL_IMEI_ISDID_SIZE)) >= EFFS_OK)
  {
   /*
    * Read Die ID for using as DES key
    */
    get_dieID(CL_DES_KEY_SIZE, r_dieId);
   /*
    * Call DES algorithm routine
    */
    /* decipher first 8 BYTEs */
    cl_des(&isdid_buf[0], r_dieId, outBufPtr, CL_DES_DECRYPTION);
    /* decipher the rest 8 BYTEs */
    cl_des(&isdid_buf[CL_DES_BUFFER_SIZE], r_dieId, d_dieId, CL_DES_DECRYPTION);
    if(!memcmp(d_dieId, r_dieId, CL_DES_KEY_SIZE))
    {
      /* Die ID is valid  */
      ret = CL_IMEI_OK;
    } else {/* Die ID is corrupted */
      char pr_buf[126];
      TRACE_ERROR("CL IMEI ERROR: Die ID is corrupted");
      sprintf(pr_buf,"Read DieID: %02x %02x %02x %02x %02x %02x %02x %02x",
                      r_dieId[0], r_dieId[1], r_dieId[2], r_dieId[3],
                      r_dieId[4], r_dieId[5], r_dieId[6], r_dieId[7]);
      TRACE_ERROR(pr_buf);
      sprintf(pr_buf,"Deciphered DieID: %02x %02x %02x %02x %02x %02x %02x %02x",
                      d_dieId[0], d_dieId[1], d_dieId[2], d_dieId[3],
                      d_dieId[4], d_dieId[5], d_dieId[6], d_dieId[7]);
      TRACE_ERROR(pr_buf);

      ret = CL_IMEI_INVALID_DIE_ID;
    }
  } else {
    ret = CL_IMEI_READ_IMEI_FAILED;
  }

    return ret;

}/* ffs_get_imeisv() */
#endif /* CL_IMEI_CALYPSO_PLATFORM */


#ifdef CL_IMEI_CALYPSO_PLUS_PLATFORM
/*
+------------------------------------------------------------------------------
| Function    : securerom_get_imeisv
+------------------------------------------------------------------------------
| Description : the function reads IMEI from Secure ROM
|
| Parameters  : inBufSize  - size of buffer where to store IMEI, min. 8 BYTE
|               *outBufPtr - pointer to buffer where to store the IMEI
| Return      :              0 - OK
|                           <0 - ERROR
+------------------------------------------------------------------------------
*/
LOCAL BYTE securerom_get_imeisv (USHORT inBufSize, UBYTE *outBufPtr)
{
  BYTE ret;

  TRACE_FUNCTION("securerom_get_imeisv()");

  if((ret = securerom_drv(inBufSize, outBufPtr)) == CL_IMEI_OK){
    return CL_IMEI_OK;
  } else {
    return CL_IMEI_READ_IMEI_FAILED;
  }
}
#endif /* CL_IMEI_CALYPSO_PLUS_PLATFORM */

#ifndef CL_IMEI_LOCOSTO_PLATFORM
/*
+------------------------------------------------------------------------------
| Function    : get_secure_imeisv
+------------------------------------------------------------------------------
| Description : the function reads IMEI either from FFS or from Secure ROM of
|               from other locations depended on hardware platform
|
| Parameters  :     inBufSize  - size of buffer where to store IMEI, min. 8 BYTE
|                   *outBufPtr - pointer to buffer where to store the IMEI
| Return      :              0 - OK
|               negative value - ERROR
+------------------------------------------------------------------------------
*/
LOCAL BYTE get_secure_imeisv(USHORT inBufSize, UBYTE *outBufPtr)
{
  UBYTE chipset = CHIPSET;
  UBYTE board = BOARD;

  TRACE_FUNCTION("get_secure_imeisv()");

/*
 * SW is running on Calypso platform (D-Sample)
 */
#ifdef CL_IMEI_CALYPSO_PLATFORM
  /*
   * Read IMEI from FFS inclusive deciphering with DES.
   */
  if((ffs_get_imeisv (inBufSize, outBufPtr)) EQ CL_IMEI_OK)
  {
    /* store IMEI */
    memcpy(stored_imei, outBufPtr, CL_IMEI_SIZE);
    imei_flag = 1;
    return CL_IMEI_OK;
  }
#else /* CL_IMEI_CALYPSO_PLATFORM */
/*
 * SW is running on Calypso plus platform (E-Sample)
 */
#ifdef CL_IMEI_CALYPSO_PLUS_PLATFORM
  if((securerom_get_imeisv (inBufSize, outBufPtr)) EQ CL_IMEI_OK)
  {
    /* store IMEI */
    memcpy(stored_imei, outBufPtr, CL_IMEI_SIZE);
    imei_flag = 1;
    return CL_IMEI_OK;
  }
#else /* CL_IMEI_CALYPSO_PLUS_PLATFORM */
/*
 * SW is running on an other platform (neither Calypso nor Calypso plus)
 */
#ifdef CL_IMEI_OTHER_PLATFORM
  {
    TRACE_EVENT_P2("CL IMEI WARNING: unknown hardware: board=%d, chipset=%d, return default imei",
                                                                             board, chipset);
    memcpy(outBufPtr, C_DEFAULT_IMEISV_DSAMPLE, CL_IMEI_SIZE);
    return CL_IMEI_OK;
  }
#endif /* CL_IMEI_OTHER_PLATFORM */
#endif /* CL_IMEI_CALYPSO_PLUS_PLATFORM */
#endif  /* CL_IMEI_CALYPSO_PLATFORM */


} /* get_secure_imeisv() */
#endif /* CL_IMEI_LOCOSTO_PLATFORM */


#endif /* FF_PROTECTED_IMEI */

#ifndef CL_IMEI_LOCOSTO_PLATFORM
/*
+------------------------------------------------------------------------------
| Function    : cl_get_imeisv
+------------------------------------------------------------------------------
| Description : Common IMEI getter function
|
| Parameters  : imeiBufSize  - size of buffer where to store IMEI, min 8 BYTEs
|               *imeiBufPtr  - pointer to buffer where to store the IMEI
|               imeiType     - indicates, if the IMEI should be read from
|                              FFS/Secure ROM (value=CL_IMEI_GET_SECURE_IMEI) or
|                              if the already read and stored IMEI (if available)
|                              should be delivered (value=CL_IMEI_GET_STORED_IMEI)
|                              The second option should be used only by ACI or
|                              BMI to show the IMEISV on mobile's display or
|                              in terminal window, e.g. if user calls *#06#.
|                              For IMEI Control reason (used by ACI), the value
|                              has to be CL_IMEI_CONTROL_IMEI
| Return      :           OK - 0
|                      ERROR - negative values
+------------------------------------------------------------------------------
*/
extern BYTE cl_get_imeisv(USHORT imeiBufSize, UBYTE *imeiBufPtr, UBYTE imeiType)
{
  BYTE ret = 0;

  TRACE_FUNCTION("cl_get_imeisv()");

#ifdef _SIMULATION_
    memcpy(imeiBufPtr, C_DEFAULT_IMEISV_DSAMPLE, CL_IMEI_SIZE);
    return CL_IMEI_OK;
#else /* _SIMULATION_ */

#ifdef FF_PROTECTED_IMEI
  /*
   * The user has required a stored IMEI. If it has been already read
   * and stored, so return stored IMEI
   */
  if((imeiType == CL_IMEI_GET_STORED_IMEI) && (imei_flag == 1)){
    memcpy(imeiBufPtr, stored_imei, CL_IMEI_SIZE);
    return CL_IMEI_OK;
  }
  /*
   * The user has required a secure IMEI. Read IMEI from FFS or from Secure ROM
   */
  if((ret = get_secure_imeisv(imeiBufSize, imeiBufPtr)) == CL_IMEI_OK)
  {
    return CL_IMEI_OK;
  } else {
    TRACE_ERROR("CL IMEI FATAL ERROR: IMEI not available!");
    /*
     * Notify the Frame entity about FATAL ERROR, but not in the case,
     * if ACI is checking IMEI, because ACI will take trouble about it.
     */
    if (imeiType != CL_IMEI_CONTROL_IMEI){
      TRACE_ASSERT(ret == CL_IMEI_OK);
    }
    return ret;
  }
/* 
 * The feature flag FF_PROTECTED_IMEI is not enabled.
 */
#else /* FF_PROTECTED_IMEI */

/*
 * Return default CALYPSO+ IMEISV value
 */
#ifdef CL_IMEI_CALYPSO_PLUS_PLATFORM

  TRACE_EVENT("CL IMEI INFO: return default IMEI-SV number");
  memcpy(imeiBufPtr, C_DEFAULT_IMEISV_ESAMPLE, CL_IMEI_SIZE);

/*
 * CL_IMEI_CALYPSO_PLATFORM or CL_IMEI_OTHER_PLATFORM is defined.
 * Try to read the IMEI number from the old ffs:/pcm/IMEI file, 
 * if it failes, return default CALYPSO IMEISV value
 */
#else /* CL_IMEI_CALYPSO_PLUS_PLATFORM */
  {
    UBYTE version;
    USHORT ret;
    UBYTE buf[SIZE_EF_IMEI];
 
    ret = pcm_ReadFile ((UBYTE *)EF_IMEI_ID, SIZE_EF_IMEI, buf, &version);
    if(ret == PCM_OK){
      TRACE_EVENT("CL IMEI INFO: return IMEI-SV number from ffs:/pcm/IMEI");
      /*
       * swap digits
       */
      imeiBufPtr[0] = ((buf[0] & 0xf0) >> 4) | ((buf[0] & 0x0f) << 4);
      imeiBufPtr[1] = ((buf[1] & 0xf0) >> 4) | ((buf[1] & 0x0f) << 4);
      imeiBufPtr[2] = ((buf[2] & 0xf0) >> 4) | ((buf[2] & 0x0f) << 4);
      imeiBufPtr[3] = ((buf[3] & 0xf0) >> 4) | ((buf[3] & 0x0f) << 4);
      imeiBufPtr[4] = ((buf[4] & 0xf0) >> 4) | ((buf[4] & 0x0f) << 4);
      imeiBufPtr[5] = ((buf[5] & 0xf0) >> 4) | ((buf[5] & 0x0f) << 4);
      imeiBufPtr[6] = ((buf[6] & 0xf0) >> 4) | ((buf[6] & 0x0f) << 4);
      imeiBufPtr[7] = ((buf[7] & 0xf0) >> 4) | ((buf[7] & 0x0f) << 4);
      /* store IMEI */
      memcpy(stored_imei, imeiBufPtr, CL_IMEI_SIZE);
      imei_flag = 1;
      
    }else{
      TRACE_EVENT("CL IMEI INFO: return default IMEI-SV number");
      memcpy(imeiBufPtr, C_DEFAULT_IMEISV_DSAMPLE, CL_IMEI_SIZE);
    }
  }
#endif /* CL_IMEI_CALYPSO_PLUS_PLATFORM */

  return CL_IMEI_OK;

#endif /* FF_PROTECTED_IMEI */
#endif /* _SIMULATION_ */
}
#endif /* #ifndef CL_IMEI_LOCOSTO_PLATFORM */

#endif /* CL_IMEI_C */
