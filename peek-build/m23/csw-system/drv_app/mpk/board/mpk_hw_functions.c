/**
 * @file  mpk_task.c
 *
 * TASK for MPK SWE.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  5/20/2003 Dennis Gallas ()   Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

/*
 * NOTE: This file will be compiled for the target environment
 */
#include "mpk/mpk_i.h"

#include "mpk/mpk_api.h"

/**
 * Get MPK
 *
 * Detailled description.
 * This function returns the manufacturer Public Key id 128-bit value. 
 * The client has to provide a buffer via the key parameter in which 
 * the key will be copied. This buffer size shall be at least 16 bytes.
 *
 * @return  RV_OK
 */                 
T_RV_RET get_mpk_id(UINT8* key_p)
{
  /* Offset   Bit
   *  0C    Bit(0)..Bit(15)
   *  0E    Bit(16)..Bit(31)
   *  ...   ...
   *  1A    Bit(112)..Bit(127)
   */

  volatile UINT8 *reg_p = (UINT8 *) MPK_ID_REG;
  
  BE_STREAM_TO_ARRAY(key_p, reg_p, MPK_SIZE);

  return RV_OK;
}
