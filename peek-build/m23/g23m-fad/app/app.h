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
void app_pstat(void) ;
void app_server(char *server) ;
void app_port(char *port) ;
void app_buffer(char *bufsize) ;
void app_bearer(char *bearer) ;
void app_switch_flow(int flow_on) ;
LOCAL void app_print_conn_info(T_SOCK_BEARER_INFO_CNF *info);

void app_open_bearer(int prov, int size, int reps);
void app_close_bearer();

char *string_to_lower(char *s) ;

#endif /*  FF_GPF_TCPIP */

#endif /* !APP_H */
