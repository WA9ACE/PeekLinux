/* 
+------------------------------------------------------------------------------
|  File:       app_cmds.c
+------------------------------------------------------------------------------
|  Copyright 2004 Texas Instruments Berlin, AG 
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
|  Purpose :  Example application for TCP/IP and Socket API -- command parser.
+----------------------------------------------------------------------------- 
*/ 


#define APP_CMDS_C

#ifdef FF_GPF_TCPIP

#define ENTITY_APP

/*==== INCLUDES =============================================================*/

#include <string.h>             /* String functions, e. g. strncpy(). */
#include <ctype.h>
#include <stdlib.h>
#ifndef _SIMULATION_
#include "typedefs.h"           /* Condat data types. */
#endif /* _SIMULATION_ */
#include "vsi.h"                /* A lot of macros. */
#ifndef _SIMULATION_
#include "custom.h"
#include "gsm.h"                /* A lot of macros. */
//#include "prim.h"               /* Definitions of used SAP and directions. */
#include "pei.h"                /* PEI interface. */
#include "tools.h"              /* Common tools. */
#endif /* _SIMULATION_ */
#include "prim.h"               /* Definitions of used SAP and directions. */
#include "socket_api.h"             /* Socket API. */
#include "app.h"                /* Global entity definitions. */



char custom_apn[SOCK_MAX_APN_LEN+1];
char custom_user_id[SOCK_MAX_USERID_LEN+1];
char custom_password[SOCK_MAX_PASSWORD_LEN+1];

BOOL custom_apn_valid = FALSE;
/*==== Local defines =========================================================*/

#define MAX_TOKENS 4            /* Maximum number of tokens to search for;
                                 * first token is the command name, last token
                                 * the rest of the line. */




/*==== Local data ============================================================*/

/* Type of command table entry. */
typedef struct APP_CMD_ENTRY app_cmd_entry_t ;

/* Type of some application core functions. */
typedef void (*core_func_t)(int prov, int n, int reps) ;

/* Type of command handler functions. */
typedef char *(*cmd_handler_t)(app_cmd_entry_t *cmd_entry_ptr,
                               char *param1,
                               char *param2,
                               char *param3,
                               core_func_t core_func) ;

/* Struct for command table entry. */
struct APP_CMD_ENTRY {
  char *cmd_name ;              /* Command string. */
  cmd_handler_t cmd_func ;      /* Command handler function. */
  core_func_t core_func ;       /* Related core function (if applicable). */
  char *cmd_usage ;             /* Command usage string (informative only). */
} ;

static char *app_cmd_data(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_echo(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_tcpsrv(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_dnsquery(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_server(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_shutdown(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_pstat(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_help(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_buffer(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_port(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_bearer(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_xon(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_xoff(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_open_bearer(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_close_bearer(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;
static char *app_cmd_set_provider(app_cmd_entry_t *, char *, char *, char *, core_func_t) ;


/* Command handler table. */
static app_cmd_entry_t app_cmd_table[] = {
#if 0
  { "tcpdl",    app_cmd_data,     app_start_tcpdl,    "[Provider][size] [reps]" },
  { "tcpul",    app_cmd_data,     app_start_tcpul,    "[Provider][size] [reps]" },
  { "udpdl",    app_cmd_data,     app_start_udpdl,    "[Provider][size] [reps]" },
  { "udpul",    app_cmd_data,     app_start_udpul,    "[Provider][size] [reps]" },
  { "tcpecho",  app_cmd_echo,     app_start_tcpecho,  "[Provider][items] [reps]" },
  { "udpecho",  app_cmd_echo,     app_start_udpecho,  "[Provider] [items] [reps]" },
  { "tcpsrv",   app_cmd_tcpsrv,   app_start_tcpsrv,   "[Provider][port] [repeat]" },
  { "dnsquery", app_cmd_dnsquery, 0,                  "[Provider][times] [fqdn]" },
  { "server",   app_cmd_server,   0,                  "[server-address]" },
  { "port",     app_cmd_port,     0,                  "[portnumber]" },
  { "buffer",   app_cmd_buffer,   0,                  "[bufsize]" },
  { "bearer",   app_cmd_bearer,   0,                  "[gsm|gprs|best|none]" },
  { "shutdown", app_cmd_shutdown, 0,                  "" },
  { "stat",     app_cmd_pstat,    0,                  "" },
#endif 
  { "help",     app_cmd_help,     0,                  "" },
#if 0
  { "xon",      app_cmd_xon,      0,                  "" },
  { "xoff",     app_cmd_xoff,     0,                  "" },
  { "open_bearer", app_cmd_open_bearer,app_open_bearer,"[Provider]" },
  { "close_bearer", app_cmd_close_bearer,0,            "[Provider]" },
  { "set_provider", app_cmd_set_provider, 0,"[apn] [user_id] [password]" },
#endif
  { 0,          0,                0,                  0},
} ;

/*============ Support functions =============================================*/
static U32 get_provider(char *param)
{
  U32 prov = APP_PROV_DEFAULT;
  if(param)
  {
    prov = atoi(param) ;
    if( (prov EQ 0) OR (!(prov < APP_PROV_INVALID)) )
    {
      TRACE_EVENT("ERROR: unkown provider - setting to default");
      prov = APP_PROV_DEFAULT;
    }
  }
  return prov;
}

static U32 get_item(char *param, U32 default_item, BOOL can_null)
{
  U32 item = default_item;
  if (param)
  {
    item = atoi(param);
    if (!can_null AND item EQ 0)
    {
      TRACE_EVENT("ERROR: item is 0, setting to default");
      item = default_item;
    }
  }
  return item;
}


/*==== Command handler functions =============================================*/

static char *app_cmd_data(app_cmd_entry_t *cmd_entry_ptr,
                          char *param1, char *param2, char *param3, core_func_t core_func)
{
  U32 prov = get_provider(param1);
  U32 size = get_item(param2, APP_DEF_DOWNLOAD_SIZE,TRUE);
  U32 reps = get_item(param3, APP_DEF_DOWNLOAD_REPS,FALSE);
  
  TRACE_FUNCTION("app_cmd_data()") ;
  core_func(prov,size, reps) ;
  return 0 ;
}


static char *app_cmd_echo(app_cmd_entry_t *cmd_entry_ptr,
                          char *param1, char *param2, char *param3, core_func_t core_func)
{
  U32 prov = get_provider(param1);
  U32 size = get_item(param2, APP_DEF_ECHO_ITEMS,TRUE);
  U32 reps = get_item(param3, APP_DEF_ECHO_REPS,FALSE);
  
  TRACE_FUNCTION("app_cmd_echo()") ;

  core_func(prov,size, reps) ;
  return 0 ;
}


static char *app_cmd_tcpsrv(app_cmd_entry_t *cmd_entry_ptr,
                            char *param1, char *param2, char *param3, core_func_t core_func)
{
  U32 prov = get_provider(param1);
  U32 port = get_item(param2, APP_DEF_TCPSRV_PORT,TRUE);
  U32 rep  = get_item(param3, APP_DEF_TCPSRV_REPEAT,TRUE);
  
  TRACE_FUNCTION("app_cmd_tcpsrv()") ;

  //app_start_tcpsrv(prov,port, rep) ;
  return 0 ;
}


static char *app_cmd_dnsquery(app_cmd_entry_t *cmd_entry_ptr,
                              char *param1, char *param2, char *param3, core_func_t core_func)
{
  U32 prov  = get_provider(param1);
  int times = get_item(param2, APP_DEF_DNSQUERY_TIMES,FALSE);

  //app_start_dnsquery(prov,times, param3) ;
  return 0 ;
}


static char *app_cmd_server(app_cmd_entry_t *cmd_entry_ptr,
                            char *param1, char *param2, char *param3, core_func_t core_func)
{
  ///app_server(param1) ;
  return 0 ;
}


static char *app_cmd_buffer(app_cmd_entry_t *cmd_entry_ptr,
                            char *param1, char *param2, char *param3, core_func_t core_func)
{
  //app_buffer(param1) ;
  return 0 ;
}


static char *app_cmd_port(app_cmd_entry_t *cmd_entry_ptr,
                          char *param1, char *param2, char *param3, core_func_t core_func)
{
  //app_port(param1) ;
  return 0 ;
}


static char *app_cmd_shutdown(app_cmd_entry_t *cmd_entry_ptr,
                              char *param1, char *param2, char *param3, core_func_t core_func)
{
  //app_shutdown() ;
  return 0 ;
}


static char *app_cmd_bearer(app_cmd_entry_t *cmd_entry_ptr,
                            char *param1, char *param2, char *param3, core_func_t core_func)
{
  //app_bearer(param1) ;
  return 0 ;
}


static char *app_cmd_pstat(app_cmd_entry_t *cmd_entry_ptr,
                           char *param1, char *param2, char *param3, core_func_t core_func)
{
  //app_pstat() ;
  return 0 ;
}


static char *app_cmd_xon(app_cmd_entry_t *cmd_entry_ptr,
                         char *param1, char *param2, char *param3, core_func_t core_func)
{
  //app_switch_flow(1) ;
  return 0 ;
}


static char *app_cmd_xoff(app_cmd_entry_t *cmd_entry_ptr,
                          char *param1, char *param2, char *param3, core_func_t core_func)
{
  //app_switch_flow(0) ;
  return 0 ;
}


static char *app_cmd_help(app_cmd_entry_t *cmd_entry_ptr,
                          char *param1, char *param2, char *param3, core_func_t core_func)
{
  TRACE_EVENT("Available commands:") ;
  for (cmd_entry_ptr = &app_cmd_table[0];
       cmd_entry_ptr->cmd_func;
       cmd_entry_ptr++)
  {
    TRACE_EVENT_P2("    %s %s",
                   cmd_entry_ptr->cmd_name, cmd_entry_ptr->cmd_usage) ;
  }
  return 0 ;
}


static char *app_cmd_open_bearer(app_cmd_entry_t *cmd_entry_ptr,  char *param1,
                                 char *param2, char *param3, core_func_t core_func)
{
  U32 prov = get_provider(param1);
  
  TRACE_FUNCTION("app_cmd_open_bearer()") ;
  core_func(prov, 0, 0);
  return 0;
}

static char *app_cmd_close_bearer(app_cmd_entry_t *cmd_entry_ptr,  char *param1,
                                  char *param2, char *param3, core_func_t core_func)
{
  TRACE_FUNCTION("app_cmd_close_bearer()") ;
  //app_close_bearer();
  return 0;
}

static char *app_cmd_set_provider(app_cmd_entry_t *cmd_entry_ptr,  char *param1,
                                  char *param2, char *param3, core_func_t core_func)
{
  /* To indicate presence of valid Custom APN */
  custom_apn_valid = TRUE;
	  
  /* Set the APN string sent by the user */
  strcpy(custom_apn, param1);

  /* Set used ID sent by the user */
  strcpy(custom_user_id, param2);

  /* Set used ID sent by the user */
  strcpy(custom_password, param3);

  return 0;

}

/*==== Local functions =======================================================*/

static char **app_tokenize_cmd(char *command)
{
  /* Three tokens will be enough: command name, parameter 1, parameter 2. Empty
   * tokens will be NULL. */
  static char *cmd_token[MAX_TOKENS] ;
  char *cur ;                   /* Pointer to current character. */
 
  int cur_tok ;                 /* Current token number. */

  
  cur = command ;
  cur_tok = 0 ;
  do
  {
    while (isspace(*cur))       /* FALSE also for NUl character. */
    {
        cur++ ;                 /* Skip whitespace. */
    }
    if (!*cur)                  /* String terminated. */
    {
      cmd_token[cur_tok] = 0 ;   /* No token here and stop. */
      break ;
    }
    cmd_token[cur_tok++] = cur ;
    cmd_token[cur_tok] = 0 ;
    while (*cur && !isspace(*cur))
    {
      cur++ ;                   /* Skip non-whitespace. */
    }
    if (*cur) *cur++ = 0 ;      /* Zero-terminate token if not end of string. */
  }
  while (cur_tok < MAX_TOKENS) ;

  return cmd_token ;
}

#if !defined (FF_GPF_TCPIP) && !defined (CO_TCPIP_TESTAPP)
char *string_to_lower(char *s)
{
  char *tmp ;

  for (tmp = s; *tmp; tmp++)
  {
    *tmp = tolower(*tmp) ;
  }
  return s ;
}
#endif  /* #if !defined (FF_GPF_TCPIP) && !defined (CO_TCPIP_TESTAPP) */


/*==== Exported functions ====================================================*/

/** Parse a command and execute it if it is valid. Return appropriate error
 * message if the command is invalid or fails.
 * 
 * @param command    command line to execute
 * @return an error message or NULL on success
 */
char *app_handle_command_tcpip(char *command)
{
  char **tokened_cmd ;          /* Tokenized command line. */
  int cmd_index ;

  TRACE_FUNCTION("app_handle_command_tcpip()") ;

  tokened_cmd = app_tokenize_cmd(command) ;
  if (!tokened_cmd[0])
  {
    return "ERROR: empty command line" ;
  }

  string_to_lower(tokened_cmd[0]) ; /* convert to lower char */
  cmd_index = 0 ;
  while (app_cmd_table[cmd_index].cmd_name)
  {
    if (!strcmp(tokened_cmd[0], app_cmd_table[cmd_index].cmd_name))
    {
      TRACE_EVENT_P4("Call %s(%s, %s, %s)", app_cmd_table[cmd_index].cmd_name,
                     tokened_cmd[1] ? tokened_cmd[1] : "(null)",
                     tokened_cmd[2] ? tokened_cmd[2] : "(null)",
                     tokened_cmd[3] ? tokened_cmd[3] : "(null)") ;
      return app_cmd_table[cmd_index].cmd_func(&app_cmd_table[cmd_index],
                                               tokened_cmd[1],
                                               tokened_cmd[2],
                                               tokened_cmd[3],
                                               app_cmd_table[cmd_index].core_func) ;
    }
    cmd_index++ ;
  }
  return "ERROR: command not recognized" ;
}

/* This is onoy for the case that the macro UI_TRACE() is undefined, in which
 * case the linker bemoans it as missing. */


#endif /* FF_GPF_TCPIP */

/* EOF */
