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

#ifndef APP_UTIL_H
#define APP_UTIL_H


#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __TODO__ __FILE__ "("__STR1__(__LINE__)") : T O D O: "

/*==== INCLUDES =============================================================*/

#include "typedefs.h"           /* Condat data types. */

/*==== CONSTS ===============================================================*/
/*==== DEFINES =========================================================*/

#define BAT_MAX_TEST_CLNT_NUM 4
#define BAT_MAX_TEST_INST_NUM 2

/* Result of test case */
#define BAT_TEST_PASS         0
#define BAT_TEST_FAIL         1

/* Indicate no additional values for test result */
#define NO_ADD_VAL 0xFFFFFFFF

/* Macros for easier coding */
#define BAT_TEST_PASSED() app_bat_send_result_prim(BAT_TEST_PASS, NO_ADD_VAL)
#define BAT_TEST_FAILED() app_bat_send_result_prim(BAT_TEST_FAIL, NO_ADD_VAL)

/*==== TYPES ================================================================*/

/* Type of command table entry. */
typedef struct APP_CMD_ENTRY app_cmd_entry_t ;

/* Type of some application core functions. */
typedef void (*core_func_t)(int , char* , char*) ;

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


/*==== FUNCTIONS ============================================================*/

/** Parse a command and execute it if it is valid. Return appropriate error
 * message if the command is invalid or fails.
 * 
 * @param command    command line to execute
 * @return an error message or NULL on success
 */
char *app_handle_command(char *command, app_cmd_entry_t * cmd_table);

/**
 * Convert a string into an integer.
 * If not possibly, assume the default value.
 */
int get_item(char *param, int default_item, BOOL can_null);

/**
 * Convert a string to lower case
 */
char *string_to_lower(char *s) ;

/**
 * Send a T_APP_BAT_TEST_RSLT result primitive.
 * Result can be either BAT_TEST_PASS or BAT_TEST_FAIL.
 */
void app_bat_send_result_prim (U8 result, U32 add_info);


#endif /* !APP_UTIL_H */
