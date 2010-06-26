/*
+-----------------------------------------------------------------------------
|  File     : psi.h
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
|  Purpose  : Definitions for the Protocol Stack Entity
|             Protokol Stack Interface (PSI).
|  $Identity:$
+-----------------------------------------------------------------------------
*/
#ifndef PSI_H
#define PSI_H

/*==== MACROS ======================================================*/

/*
 * defines the user of the vsi interface
 */
#define VSI_CALLER            PSI_handle,
#define VSI_CALLER_SINGLE     PSI_handle

/*
 * VERSION
 *
 * Description :  The constants define the type and the value
 *                of a version identification. The version
 *                is part of the monitor struct.
 */
#define VERSION_PSI    "PSI 1.0"

/*
 * This is just a TEMPORARY define until the issues with OPTION_MULTITHREAD
 * are settled. This define SHOULD be contained in GSM.H.
 */
#undef _ENTITY_PREFIXED
#define _ENTITY_PREFIXED(N) psi_##N

/*
 * Communication handles (see also PSI_PEI.C)
 */
#define hCommMMI                psi_hCommMMI 
#define hCommPSI                psi_hCommPSI

/*==== CONSTANTS ======================================================*/


/*
 * Scaling
 */
#define PSI_INSTANCES                6 /* ? RM: PKT 4, UART 2, later further AAA ??*/

/*
 * Return values of ker_new_instance()
 */
#define PSI_NEW_OK    0
#define PSI_NEW_FULL  1
#define PSI_NEW_USED  2
/*
 * Return values of device switching functions.
 */
#define PSI_DEVICE_FOUND      0
#define PSI_DEVICE_NOT_FOUND  1

/*
* Return values of instance switching functions.
 */
#define PSI_INST_OK         0
#define PSI_INST_NOT_FOUND  1

/*
 * Service definitions. Used to access service data with GET/SET_STATE.
 */
#define PSI_SERVICE_KER       ker.
#define PSI_SERVICE_RX        rx.
#define PSI_SERVICE_TX        tx.
#define PSI_SERVICE_DTX       dtx.
#define PSI_SERVICE_DRX       drx.

/*
 * Service name definitions for trace purposes. 
 */
#ifndef NTRACE

#define SERVICE_NAME_PSI_SERVICE_KER    "KER"
#define SERVICE_NAME_PSI_SERVICE_RX     "RX"
#define SERVICE_NAME_PSI_SERVICE_TX     "TX"
#define SERVICE_NAME_PSI_SERVICE_DRX    "DRX"
#define SERVICE_NAME_PSI_SERVICE_DTX    "DTX"

#endif /* !NTRACE */

/* State definitions for each service.*/

#define PSI_KER_DEAD           1
#define PSI_KER_CONNECTING     2
#define PSI_KER_READY          3
#define PSI_KER_MODIFY         4

#define PSI_KER_DTI_DEAD       1
#define PSI_KER_DTI_OPENING    2
#define PSI_KER_DTI_READY      3

#define PSI_DRX_DEAD_NOT_READY 1
#define PSI_DRX_DEAD_READY     2
#define PSI_DRX_NOT_READY      3
#define PSI_DRX_READY          4

#define PSI_TX_DEAD            1
#define PSI_TX_READY           2
#define PSI_TX_BUFFER          3
#define PSI_TX_FLUSHING        4

#define PSI_DTX_DEAD           1
#define PSI_DTX_NOT_READY      2
#define PSI_DTX_READY          3

#define PSI_RX_DEAD_NOT_READY  1
#define PSI_RX_NOT_READY       2
#define PSI_RX_READY           3
#define PSI_RX_BUFFER          4

#define PSI_SEG_NUM            4  /* max number of buffer segments */
#define PSI_SEG_SER_NUM        1  /* max number of buffer segments for serial driver */
#define PSI_SEG_PKT_NUM        2  /* max number of buffer segments for packet (BAT) driver */
#define PSI_WRITE_BUF_MAX      2
#define PSI_READ_BUF_MAX       2

#define MAX_SER_BAUD_RATE     23

/*
 * Number of bytes for the Protocol field
 */
#define PSI_NUM_PID_BYTE            2

/* mask for line states */
#define PSI_DIO_SA_ON          0
#define PSI_DIO_SA_OFF         0x80000000
#define PSI_DIO_SB_OFF         0x40000000
#define PSI_DIO_X_OFF          0x20000000
#define PSI_DIO_RING_ON        0x10000000
#define PSI_DIO_ESC_DET        0x08000000
#define PSI_DIO_BRK_REC        0x02000000
#define PSI_DIO_SLEEP_SUPP     0x80000000
#define PSI_DIO_SER_ESC_ON     0x80000000
#define PSI_DIO_BAUDR_AUTO     0x00000001
#define PSI_DTI_RING_ON        0x0004
#define PSI_DTI_RING_OFF       0x0008
#define PSI_DTI_DCD_ON         0x0001
#define PSI_DTI_DCD_OFF        0x0002
/*==== TYPES ======================================================*/

/* controling send order of buffer to DTI */
typedef enum
{
  BUFFER_1 = 1,
  BUFFER_2,
  BUFFER_MAX
}T_NEXT_SEND_DTI_ID;

/* data structure for each service */ 
typedef struct /* T_KER_DATA */
{
  UBYTE                       state;
  /* Required for state traces.*/
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
/* driver capabilities */
  T_DIO_CAP *capabilities;
/* service specific */
/* The state of the dti connection.*/
  UBYTE dti_state;
} T_KER_DATA;


typedef struct /* T_RX_DATA */
{
  UBYTE                       state;
  /* Required for state traces.*/
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
  /* How big are the segments in the read buffer?*/
  U16 psi_seg_size[PSI_SEG_NUM];
  /* How many segments exist */
  U8 number_segm;
  /* two receive dio buffers */   
  T_dio_buffer* psi_buffer_1;
  T_dio_buffer* psi_buffer_2;
  /*  pending dio buffer list */    
  T_dio_buffer* psi_buffer_pending[PSI_READ_BUF_MAX];
  /* two desc2 buffers */    
  T_desc2* psi_buffer_1_desc;
  T_desc2* psi_buffer_2_desc;
    /*  pending desc2 buffer list */    
  T_desc2* psi_buffer_desc_pending[PSI_READ_BUF_MAX];
  /* Is it used by driver or pending to be sent to dtx service? */   
  BOOL psi_buffer_1_used;
  BOOL psi_buffer_2_used;
  /* control if buffer is already read*/ 
  BOOL psi_buffer_1_read;
 /* Buffer are pending to be sent to DTI2, waiting for dtx ready */   
   BOOL psi_buffer_pend[PSI_READ_BUF_MAX];
  /* index of next sent buffer (order!) */
  U8 next_send_id;
  /* store control info sent by DIO */
  T_DIO_CTRL_LINES psi_control_info_ser;
} T_RX_DATA;


typedef struct /* T_TX_DATA */
{
  UBYTE                       state;
  /* Required for state traces.*/
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
 
  T_dio_buffer *psi_buffer_pending;      /* buffered packet */
  T_desc2      *psi_buffer_desc_pending; /* buffered packet for the DESC */
  T_desc2      *psi_buffer_desc_pid_pending;  /* buffered protocol identifier element */
  T_dio_buffer *psi_buffer_pending_flush;      /* buffered packet, send after flushing */
  T_desc2      *psi_buffer_desc_pending_flush; /* buffered packet for the DESC, send after flushing  */

  T_dio_buffer *psi_buffer[PSI_WRITE_BUF_MAX];/* number of write buffer */
  T_desc2      *psi_buf_desc[PSI_WRITE_BUF_MAX];/* number of stored desc list buffer */
  T_desc2      *psi_buf_pid_desc[PSI_WRITE_BUF_MAX];/* number of stored protocol id desc list buffer */
  U8            in_driver; /* Count packets in driver, must not be bigger than PSI_WRITE_BUF_MAX */

  /* Is it used to be sent to DIO */
  BOOL psi_buffer_1_used;
 /* Buffer state (is pending to be sent to DIO ?) */   
  BOOL psi_buffer_1_pend;
 /* flag for flushing procedure */   
  BOOL psi_dio_flush;
 /* flag for PSI_LINE_STATE_REQ */
  BOOL flag_line_state_req;
  /* store control info sent by DTI/ACI */
  T_DIO_CTRL_LINES psi_control_info_ser;
} T_TX_DATA;


typedef struct /* T_DTX_DATA */
{
  UBYTE                       state;
  /* Required for state traces.*/
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
} T_DTX_DATA;


typedef struct /* T_DRX_DATA */
{
  UBYTE                       state;
  /* Required for state traces.*/
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif /* !NTRACE */
} T_DRX_DATA;

/*
 * service data base
 */

typedef struct /* T_PSI_DATA */
{
  UBYTE                       version;
  /* Service data structures */
  T_KER_DATA                           ker;
  T_RX_DATA                            rx;
  T_TX_DATA                            tx;
  T_DRX_DATA                           drx;
  T_DTX_DATA                           dtx;
  /* global PSI variables */
  /* Instance number.*/
  U8 instance;
  /* Is this instance used? */
  BOOL used;
  /* The device number used at the affected driver.*/
  U32 device_no;
  /* The device control block for serial config param.*/
  T_DIO_DCB_SER * dcb;
    /* The device control block for packet config param.*/
  T_DIO_DCB_PKT* dcb_pkt;
  /* DTI handle.*/
  DTI_HANDLE hDTI;
  U32 link_id;
} T_PSI_DATA;


/*==== EXPORT =====================================================*/

/*
 * Entity data base
 */
#ifdef PSI_PEI_C
       T_PSI_DATA psi_data_base[PSI_INSTANCES], *psi_data, *psi_sim;
/* global part of read buffers */
#else
EXTERN T_PSI_DATA psi_data_base[], *psi_data, *psi_sim;
#endif

#define ENTITY_DATA           psi_data


/*
 * prefix service functions
 */
#define ker_init              _ENTITY_PREFIXED(ker_init)
#define rx_init               _ENTITY_PREFIXED(rx_init)
#define tx_init               _ENTITY_PREFIXED(tx_init)
#define drx_init              _ENTITY_PREFIXED(drx_init)
#define dtx_init              _ENTITY_PREFIXED(dtx_init)
#define rx_read_data          _ENTITY_PREFIXED(rx_read_data)
#define tx_write_data         _ENTITY_PREFIXED(tx_write_data)
#define tx_create_hdlc_frame  _ENTITY_PREFIXED(tx_create_hdlc_frame)

/*
 * Communication handles (see also PSI_PEI_C.C)
 */
#ifdef PSI_PEI_C

       T_HANDLE hCommMMI        = VSI_ERROR;
       T_HANDLE hCommPSI        = VSI_ERROR;
       T_HANDLE PSI_handle;

/*
 * make the pei_create function unique
 */
#define pei_create              _ENTITY_PREFIXED(pei_create)

#else /* PSI_PEI_C */

  EXTERN T_HANDLE hCommMMI;
  EXTERN T_HANDLE hCommPSI;
  EXTERN T_HANDLE PSI_handle;
  /*
   * functions for switching of entities/instances
   */
  EXTERN UBYTE    pei_select_instances( UBYTE incoming_c_id );

#endif /* PSI_PEI_C */



#ifdef _SIMULATION_
/* 
 * Prototypes for the simulation environment 
 */
EXTERN void psi_diosim_sign_ind (const T_PSI_DIOSIM_SIGN_IND *signal_ind);
#endif /* _SIMULATION_ */

/*
 * If all entities are linked into one module this definitions
 * prefixes the global data with the enity name
 */
#ifdef OPTION_MULTITHREAD
  #define _decodedMsg   _ENTITY_PREFIXED(_decodedMsg)
#endif /* OPTION_MULTITHREAD */

#endif /* !PSI_H */



