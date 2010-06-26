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
|  Purpose :  Utilities and stuff common for the different test parts
+----------------------------------------------------------------------------- 
*/ 


#define APP_UTIL_C

/*==== INCLUDES =============================================================*/

#include "app_util.h"

#include <string.h>             /* String functions, e. g. strncpy(). */
#include <ctype.h>
#include <stdlib.h>
#ifndef _SIMULATION_
#endif /* _SIMULATION_ */
#include "vsi.h"                /* A lot of macros. */
#ifndef _SIMULATION_
#include "custom.h"
#include "gsm.h"                /* A lot of macros. */
#include "tools.h"              /* Common tools. */
#endif /* _SIMULATION_ */

/*==== Local defines =========================================================*/

#define MAX_TOKENS 4            /* Maximum number of tokens to search for;
                                 * first token is the command name, last token
                                 * the rest of the line. */
/*==== Local functions =======================================================*/

static char **tokenize_cmd(char *command)
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

char *string_to_lower(char *s)
{
  char *tmp ;

  for (tmp = s; *tmp; tmp++)
  {
    *tmp = tolower(*tmp) ;
  }
  return s ;
}

/*==== Exported functions ====================================================*/

/** Parse a command and execute it if it is valid. Return appropriate error
 * message if the command is invalid or fails.
 * 
 * @param command    command line to execute
 * @return an error message or NULL on success
 */
char *app_handle_command(char *command, app_cmd_entry_t * cmd_table)
{
  char **tokened_cmd ;          /* Tokenized command line. */
  int cmd_index ;

  TRACE_FUNCTION("app_handle_command()") ;

  tokened_cmd = tokenize_cmd(command) ;
  if (!tokened_cmd[0])
  {
    return "ERROR: empty command line" ;
  }

  string_to_lower(tokened_cmd[0]) ; /* convert to lower char */
  cmd_index = 0 ;
  while (cmd_table[cmd_index].cmd_name)
  {
    if (!strcmp(tokened_cmd[0], cmd_table[cmd_index].cmd_name))
    {
      TRACE_EVENT_P4("Call %s(%s, %s, %s)", cmd_table[cmd_index].cmd_name,
                     tokened_cmd[1] ? tokened_cmd[1] : "(null)",
                     tokened_cmd[2] ? tokened_cmd[2] : "(null)",
                     tokened_cmd[3] ? tokened_cmd[3] : "(null)") ;
      return cmd_table[cmd_index].cmd_func(&cmd_table[cmd_index],
                                           tokened_cmd[1],
                                           tokened_cmd[2],
                                           tokened_cmd[3],
                                           cmd_table[cmd_index].core_func) ;
    }
    cmd_index++ ;
  }
  return "ERROR: command not recognized" ;
}



/**
 * Convert a string into an integer.
 * If not possibly, assume the default value.
 */
int get_item(char *param, int default_item, BOOL can_null)
{
  U32 item = default_item;
  if (param)
  {
    item = atoi(param);
    if (!can_null AND item EQ 0)
    {
      TRACE_EVENT("WARNING: item is 0, setting to default");
      item = default_item;
    }
  }
  return item;
}


/* This is onoy for the case that the macro UI_TRACE() is undefined, in which
 * case the linker bemoans it as missing. */

/* EOF */
