/*
+------------------------------------------------------------------------------
|  File:       app.h
+------------------------------------------------------------------------------
|  Copyright 2003 Texas Instruments Berlin, AG
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
|  Purpose :  Definitions for the Protocol Stack Entity app.
+-----------------------------------------------------------------------------
*/

#ifndef APP_H
#define APP_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/

#ifdef _SIMULATION_
#define VSI_CALLER
#else  /* _SIMULATION_ */
#define VSI_CALLER       APP_handle,
#endif /* _SIMULATION_ */

#define ENTITY_DATA      app_data

#define ENCODE_OFFSET    0             /* Bitoffset for encoding/decoding */

/* Timer definitions */
#define T001             0
#define T002             1

/* Timer durations */
#define T001_VALUE       1000          /* 1 second */
#define T002_VALUE       3000          /* 3 seconds */

/* make the pei_create function unique */
#define pei_create       app_pei_create


/* Default values for application procedures. */
#define APP_DEF_BUFFER_SIZE    200
#define APP_DEF_DOWNLOAD_REPS  1
#define APP_DEF_DOWNLOAD_SIZE  5000
#define APP_DEF_UPLOAD_REPS    1
#define APP_DEF_UPLOAD_SIZE    5000
#define APP_DEF_UDP_INTERVAL   100 /* ms between packets */
#define APP_DEF_ECHO_ITEMS     10
#define APP_DEF_ECHO_REPS      1
#define APP_DEF_DNSQUERY_TIMES 1

typedef enum {
  APP_PROV_T_MOBILE = 1,
  APP_PROV_VODAFONE,
  APP_PROV_DEFAULT = APP_PROV_VODAFONE,
  APP_PROV_HUTCH,
  APP_PROV_AIRTEL,
  APP_PROV_CUSTOM,
  // add new providers here
  APP_PROV_INVALID
} T_APP_PROV;

#ifdef _SIMULATION_
#define APP_DEF_SERVER         "137.167.238.28"
#define APP_DEF_CLOSE_SLEEP    2000 /* Sleep before sock_close_bearer(). */
#define APP_DATA_DUMP_LENGTH   256
#define APP_DEF_DNSQUERY_ADDR  "chuck.berlin.tide.ti.com"
#else  /* _SIMULATION_ */
#define APP_DEF_SERVER         "72.20.1.6"
#define APP_DEF_CLOSE_SLEEP    2000
#define APP_DATA_DUMP_LENGTH   16
#define APP_DEF_DNSQUERY_ADDR  "4.2.2.1"
#endif /* _SIMULATION_ */

#define APP_DEF_TCPSRV_PORT    3425
#define APP_DEF_TCPSRV_REPEAT  0

#define APP_N_CLIENTS          10

#define APP_SEND_PATTERN \
    "Testapp for GPF-based TCP/IP [ni@ti.com] " \
    "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOP" \
    "QRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ "

/*==== TYPES =================================================================*/

/* APP global typedefs */

typedef struct                         /* T_APP_DATA */
{
  unsigned char version;
/*
 * entity parameters
 */
} T_APP_DATA;

/*==== EXPORTS ===============================================================*/

short app_pei_create (T_PEI_INFO **info);

#ifdef APP_PEI_C
/* Entity data base */
T_APP_DATA               app_data_base;
T_APP_DATA               *app_data;

/* Communication handles */
T_HANDLE                 APP_handle;
#else  /* APP_PEI_C */
extern T_APP_DATA           app_data_base;
extern T_APP_DATA           *app_data;
extern T_HANDLE             hCommYYY;
extern T_HANDLE             hCommMMI;
extern T_HANDLE             APP_handle;
#ifdef FF_GPF_TCPIP
extern T_SOCK_API_INSTANCE  sock_api_inst ;
extern T_SOCK_BEARER_HANDLE sock_bearer_handle;
EXTERN char custom_apn[SOCK_MAX_APN_LEN+1];
EXTERN char custom_user_id[SOCK_MAX_USERID_LEN+1];
EXTERN char custom_password[SOCK_MAX_PASSWORD_LEN+1];
#endif /* FF_GPF_TCPIP */
#endif /* APP_PEI_C */

#define BAT_MAX_TEST_CLNT_NUM 4
#define BAT_MAX_TEST_INST_NUM 2

#ifdef FF_GPF_TCPIP

/* We can run different types of application processes, according to the
 * commend sent by the user. */
typedef enum {
  AP_NONE,                      /* For uninitialized process types. */
  AP_TCPDL,                     /* Download some data over TCP. */
  AP_TCPUL,                     /* Upload some data over TCP. */
  AP_UDPDL,                     /* Download some data over UDP. */
  AP_UDPUL,                     /* Upload some data over UDP. */
  AP_TCPECHO,                   /* Send/receive data to/from TCP echo port. */
  AP_UDPECHO,                   /* Send/receive data to/from UDP echo port. */
  AP_TCPSRV,                    /* TCP server application. */
  AP_DNSQRY,                    /* Issue DNS queries and collect result. */
  AP_TCPFORK,                   /* Forked TCP server process. */
  AP_INVALID
} APP_PROCTYPE_T ;

/* Process states; the state transitions are mostly linear in this order. */
typedef enum {
  PS_IDLE,                      /* Initial state, process not running. */
  PS_W_DCM_OPEN,                /* Waiting for DCM to open connection. */
  PS_W_DCM_OPEN_ONLY,           /* Waiting for DCM to open connection - no further action. */
  PS_W_CREAT,                   /* Waiting for socket create confirmation. */
  PS_W_SCONN,                   /* Waiting for socket connect confirmation. */
  PS_W_BIND,                    /* Waiting for socket bind confirmation. */
  PS_W_LISTN,                   /* Waiting for confirmation of listen call. */
  PS_LISTENS,                   /* Listens for client connections. */
  PS_W_DNS,                     /* Waiting for a DNS query. */
  PS_COMM,                      /* Happily exchanging data. */
  PS_W_SCLOS,                   /* Waiting for socket close confirmation. */
  PS_W_DCLOS,                   /* Waiting for DCM to close connection. */
  PS_W_CONN_INFO,               /* Waiting for connection information */
  PS_DCM_OPEN,                  /* DCM (bearer) connecion opened*/
  PS_SOCK_OPEN,                 /* Socket and bearer open */
  PS_INVALID
} PROC_STAT_T ;

/* The data a process holds. May be dynamically allocated in the future. */
typedef struct PROCESS_CONTEXT_S {
  APP_PROCTYPE_T ptype ;        /* Type of application process */
  PROC_STAT_T pstate ;          /* Process status as defined above. */
  int in_shutdown ;             /* Non-zero iff process is being shut down. */
  T_SOCK_EVENTSTRUCT *last_evt; /* Last event passed from the Socket API. */
  T_SOCK_IPPROTO ipproto ;      /* IP protocol number for this process (TCP or
                                 * UDP); unused with dq. */
  char *server_name ;           /* May be a domain name or an IP address in
                                 * dotted decimal notation. */
  T_SOCK_IPADDR server_ipaddr ; /* Server IP address. (Will be IPADDR_ANY in
                                 * case of AP_TCPSRV.) */
  T_SOCK_PORT server_port ;     /* Server port number. (Also in case of
                                 * AP_TCPSRV.) */

  /* The following variables are in use only where appropriate, of course --
   * as indicated in the comment. */

  int f_id ;                    /* Identity of TCP server fork. */
  int spec_items ;              /* Specified number of items to transfer. (The
                                 * items are single bytes for dl and ul.) */
  int spec_reps ;               /* Specified number of repetitions. */

  int data_sent ;               /* Total amount of data sent (ul, te, ue). */
  int data_rcvd ;               /* Total amount of data recvd (dl, te, ue). */
  int items_sent ;              /* Number of blocks/packets/queries sent (ul,
                                 * te, ue, dq). */
  int items_rcvd ;              /* Number of blocks/packets/responses received
                                 * (dl, te, ue, dq). */
  int n_reps ;                  /* Number of repetitions done. */
  int errors ;                  /* Number of errors at all. */
  T_SOCK_SOCKET psocket ;       /* The socket in use by the process. */
  int network_is_open ;         /* Non-zero iff we have an open network
                                 * connection. */
  int psocket_is_open ;         /* Non-zero iff we have an open psocket. */
  BOOL bearer_only;             /* if set, only a Bearer will be opened */ 
} PROC_CONTEXT_T ;

/*==== Callbacks =============================================================*/

void app_sock_callback(T_SOCK_EVENTSTRUCT *event, void *context) ;


/*==== APP-internal functions ================================================*/

/** Initialize the application core, including APIs for DCM and Sockets.
 *
 * @param handle    own communication handle
 * @return PEI_OK/PEI_ERROR depending on the success of the API initializations.
 */
BOOL app_initialize_tcpip(T_HANDLE app_handle) ;

/** Parse a command and execute it if it is valid. Return appropriate error
 * message if the command is invalid or fails.
 *
 * @param command    command line to execute
 * @return an error message or NULL on success
 */
char *app_handle_command_tcpip(char *command) ;


void app_start_tcpdl(int prov, int size, int reps) ;
void app_start_tcpul(int prov, int size, int reps) ;
void app_start_udpdl(int prov, int size, int reps) ;
void app_start_udpul(int prov, int size, int reps) ;
void app_start_tcpecho(int prov, int size, int reps) ;
void app_start_udpecho(int prov, int size, int reps) ;
void app_start_tcpsrv(int prov, int port, int repeat) ;
void app_start_dnsquery(int prov, int times, char *address) ;
void app_shutdown(void) ;
void app_pstat(PROC_CONTEXT_T *pcont) ;
void app_server(char *server) ;
void app_port(char *port) ;
void app_buffer(char *bufsize) ;
void app_bearer(char *bearer) ;
void app_switch_flow(PROC_CONTEXT_T *pcont, int flow_on) ;
LOCAL void app_print_conn_info(T_SOCK_BEARER_INFO_CNF *info);

void app_open_bearer(int prov, int size, int reps);
void app_close_bearer();

char *string_to_lower(char *s) ;

#endif /*  FF_GPF_TCPIP */

#endif /* !APP_H */
