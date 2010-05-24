/**
 * @file  nan_bm_message.h
 *
 * Data structures:
 * 1) used to send messages to the NAN_BM SWE,
 * 2) NAN_BM can receive.
 *
 * @author  J.A. Renia
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  2/27/2006 J.A. Renia        Create.
 *
 * (C) Copyright 2006 by ICT Embedded, All Rights Reserved
 */

#ifndef __NAN_BM_MESSAGE_H_
#define __NAN_BM_MESSAGE_H_


#include "rv/rv_general.h"

// #include "nan_bm/nan_bm_cfg.h"
#include "nan/nan_cfg.h"


#ifdef __cplusplus
extern "C"
{
#endif


/** 
 * The message offset must differ for each SWE in order to have 
 * unique msg_id in the system.
 */
 // #define NAN_BM_MESSAGE_OFFSET  BUILD_MESSAGE_OFFSET(NAN_BM_USE_ID)
 #define NAN_BM_MESSAGE_OFFSET  BUILD_MESSAGE_OFFSET(NAN_USE_ID)



/**
 * @name NAN_BM_SAMPLE_MESSAGE
 *
 * Short description.
 *
 * Detailled description
 */
/*@{*/
/** Message ID. */
#define NAN_BM_SAMPLE_MESSAGE (NAN_BM_MESSAGE_OFFSET | 0x001)

/** Message structure. */
  typedef struct
  {
    /** Message header. */
  T_RV_HDR hdr;

    /** Some parameters. */
  /* ... */

  }
  T_NAN_BM_SAMPLE_MESSAGE;
/*@}*/


#ifdef __cplusplus
}
#endif

#endif              /* __NAN_BM_MESSAGE_H_ */

