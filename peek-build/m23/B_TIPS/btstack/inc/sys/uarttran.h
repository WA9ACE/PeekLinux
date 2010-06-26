#ifndef __UATRAN_H
#define __UATRAN_H

/***************************************************************************
 *
 * File:
 *     $Workfile:uarttran.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:40$
 *
 * Description:
 *     This file contains definitions and structures specific
 *     to the UART HCI transport driver.
 *
 * Created:
 *     December 21, 1999
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "bttypes.h"
#include "hcitrans.h"
#include "sys/rxbuff.h"
#include "eventmgr.h"

/* MS to wait for HCI events */
#define UTP_UART_EVENT_WAIT_TIME  10000

/* Receive States */
#define RXS_GET_TYPE      1
#define RXS_GET_SEQ       2
#define RXS_GET_HEADER    3
#define RXS_GET_BUFFER    4
#define RXS_GET_DATA      5

/* Transmit States */
#define TXS_IDLE          1
#define TXS_TYPE          2
#define TXS_HEADER        3
#define TXS_DATA          4

/* Packet types or indicators */
#define IND_COMMAND         (U8)0x01
#define IND_ACL_DATA        (U8)0x02
#define IND_SCO_DATA        (U8)0x03
#define IND_EVENT           (U8)0x04
#define IND_ERROR_MSG       (U8)0x05
#define IND_NEGOTIATION     (U8)0x06

/* Transport driver context
 * 
 * Contains queues and state information for the HCI-UART
 * transport driver.
 */
typedef struct _UATRAN_Context {

    /* Transmit queues */
    ListEntry   txQueue;
    ListEntry   txEnroute;

    /* Callback Entry points */
    TranEntry    tranEntry;
    TranCallback callback;

    /* RX State variables */
    U16         maxReadLen;
    U16         totalLen;
    U8          buffType;
    U8          *ptr;
    U8          header[4];
    U8          headerLen;
    RxBuffHandle rxBuffHandle;
    U8          rxState;

    /* TX State variables */
    HciPacket  *txPacket;
    U8          txState;
    const U8   *txBp;              /* pointer to location to write from */
    U16         txLen;              /* num bytes in buffer to write */
    U8          txType;             /* type of packet we're transmitting */
    U8          txFrag;             /* index of fragment to process */
    EvmTimer    watchdog;           /* Watchdog timer for HCI events */
    U8          unackedCommand;     /* TRUE when there is an unacknowledged command */
} UATRAN_Context;

/* Define the appropriate external reference to the transport context,
 * if necessary.
 */
#if (XA_CONTEXT_PTR == XA_ENABLED)
extern UATRAN_Context *uaxprt;
#define UATRAN(s) (uaxprt->s)

#elif (XA_CONTEXT_PTR == XA_DISABLED)
extern UATRAN_Context uaxprt;
#define UATRAN(s) (uaxprt.s)
#endif

/* Function prototypes */
BtStatus UARTTRAN_Init(TranCallback tranCallback);
BtStatus UARTTRAN_Shutdown(void);

#endif /* __UATRAN_H */
