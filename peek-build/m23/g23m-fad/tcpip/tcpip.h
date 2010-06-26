/* 
+------------------------------------------------------------------------------
|  File:       tcpip.h
+------------------------------------------------------------------------------
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
|  Purpose :  Definitions for the Protocol Stack Entity TCPIP.
+----------------------------------------------------------------------------- 
*/ 

#ifndef TCPIP_H
#define TCPIP_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/

#define TCPIP_MAX_DTI_LINKS 4   /* For phase one, this is one DTI link per
                                 * application plus one to the lower layer
                                 * (PPP or SNDCP. */

#define TCPIP_DTI_QUEUE_SIZE 5  /* Arbitrary value. */

#define TCPIP_DEFAULT_WINDOW 1  /* Default window size regarding payload data
                                 * between TCPIP and the application. As we do
                                 * not really do window-based flow control
                                 * yet, this value only means "send another
                                 * primitive". */

#ifdef VSI_CALLER
#undef VSI_CALLER
#endif /* VSI_CALLER */
#define VSI_CALLER       tcpip_handle,

#define hCommMMI         tcpip_hCommMMI  /* Communication handle */

/*==== TYPES =================================================================*/


/* Flow control status. */
typedef enum { TCPIP_FLOWCTL_XOFF = 0, TCPIP_FLOWCTL_XON } T_flowctl_status ;

/* Global entity data.
 * 
 */
typedef struct                  /* T_TCPIP_DATA */
{
  unsigned char version ;
  BOOL is_initialized ;         /* TCPIP/RNET has been initialized.  */
  DTI_HANDLE dti_handle ;       /* TCPIP's own DTI handle. */
  struct                        /* Data for the DTI link to the lower layer */
  {
    U32 link_id ;               /* DTI link identifier. */
    T_flowctl_status flowstat_ul ;
                                /* Flow control status to lower layer
                                 * (uplink). */
    T_flowctl_status flowstat_dl ;
                                /* Flow control status to IP (downlink). */
  }
  ll[1] ;                       /* Indexed by DTI channel number. */
  U32 config_dns_address ;      /* DNS server address, set by config
                                 * primitive, in network byte order. Overrides
                                 * the first DNS server address in
                                 * TCPIP_IFCONFIG_REQ. */
} T_TCPIP_DATA;




/*==== EXPORTS ===============================================================*/

#ifdef TCPIP_PEI_C

/* Entity data base */
T_TCPIP_DATA               tcpip_data_base;
T_TCPIP_DATA               *tcpip_data;

/* Communication handles */
T_HANDLE                 hCommMMI = VSI_ERROR;
T_HANDLE                 tcpip_handle;
T_HANDLE                 hCommTCPIP = VSI_ERROR ;

#else  /* TCPIP_PEI_C */

extern T_TCPIP_DATA      tcpip_data_base, *tcpip_data;
extern T_HANDLE          hCommMMI;
extern T_HANDLE          tcpip_handle;
extern T_HANDLE          hCommTCPIP ;

#endif /* TCPIP_PEI_C */

/*==== Some functions ========================================================*/

/** Shut down RNET and deallocate data. This defined as a separate function
 * because it will also be called by pei_exit().
 * 
 */
void tcpip_do_shutdown(void) ;


/*==== DTI-related functions =================================================*/

/** DTI callback function according to dti.h.
 * 
 */
void tcpip_dti_callback(U8 instance, U8 interfac, U8 channel, U8 reason,
                        T_DTI2_DATA_IND *dti_data_ind) ;


/** Confirm the result of a TCPIP_DTI_REQ. This function is called
 * from tcpip_dti.c, so it must not be static.
 * 
 * @param dti_conn    Indicates whether the DTI link is to be established or
 *                    disconnected
 * @param link_id     DTI link identifier
 */
void tcpip_dti_cnf(U8 dti_conn, U32 link_id) ;


/** Send the data buffer with the specified length to the lower layer. The
 * data buffer will be freed by the caller.
 * 
 * @param data        Pointer to the data.
 * @param length      Length of the data.
 */
void tcpip_dti_send_data_ll(U8 *data, U16 length) ;



/*==== Primitive handler functions ===========================================*/

/** Handle the primitive of the same name.
 * 
 * @param primdata    Pointer to primitive data (duh!).
 */
void tcpip_initialize_req(void *primdata) ;
void tcpip_shutdown_req(void *primdata) ;
void tcpip_ifconfig_req(void *primdata) ;
void tcpip_dti_req(void *primdata) ;
void tcpip_create_req(void *primdata) ;
void tcpip_close_req(void *primdata) ;
void tcpip_bind_req(void *primdata) ;
void tcpip_listen_req(void *primdata) ;
void tcpip_connect_req(void *primdata) ;
void tcpip_data_req(void *primdata) ;
void tcpip_data_res(void *primdata) ;
void tcpip_sockname_req(void *primdata) ;
void tcpip_peername_req(void *primdata) ;
void tcpip_hostinfo_req(void *primdata) ;
void tcpip_mtu_size_req(void *primdata) ;
void tcpip_internal_ind(void *primdata) ;

/** Send a message to self.
 * 
 * @param msg_p    pointer to message
 * @param msg_id   message identification
 */
void tcpip_send_internal_ind(U32 msg_p, U32 msg_id) ;


/* DTI primitive handlers.
 */
void tcpip_dti_ready_ind     (T_DTI2_READY_IND      *dti_ready_ind) ;
void tcpip_dti_data_req      (T_DTI2_DATA_REQ       *dti_data_req) ;
void tcpip_dti_data_ind      (T_DTI2_DATA_IND       *dti_data_ind) ;
void tcpip_dti_getdata_req   (T_DTI2_GETDATA_REQ    *dti_getdata_req) ;
#if defined(_SIMULATION_)       /* Used only for simulation test primitives. */
void tcpip_dti_data_test_ind (T_DTI2_DATA_TEST_IND  *dti_data_test_ind) ;
void tcpip_dti_data_test_req (T_DTI2_DATA_TEST_REQ  *dti_data_test_req) ;
#endif /* _SIMULATION_ */
void tcpip_dti_connect_req   (T_DTI2_CONNECT_REQ    *dti_connect_req) ;
void tcpip_dti_disconnect_ind(T_DTI2_DISCONNECT_IND *dti_disconnect_ind) ;
void tcpip_dti_connect_cnf   (T_DTI2_CONNECT_CNF    *dti_connect_cnf) ;
void tcpip_dti_connect_ind   (T_DTI2_CONNECT_IND    *dti_connect_ind) ;
void tcpip_dti_connect_res   (T_DTI2_CONNECT_RES    *dti_connect_res) ;
void tcpip_dti_disconnect_req(T_DTI2_DISCONNECT_REQ *dti_disconnect_req) ;



#endif /* !TCPIP_H */

