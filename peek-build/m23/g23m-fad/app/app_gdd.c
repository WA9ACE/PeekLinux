/*
+------------------------------------------------------------------------------
|  File:       app_gdd.c
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
|  Purpose :  Test functions for testing the GDD interface -- command parser.
+-----------------------------------------------------------------------------
*/


#define APP_GDD_C

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

#include "../gdd_dio/gdd.h"
#include "../gdd_dio/gdd_dio.h"
#include "../gdd_dio/dio_il_psi_stub.h" /* for DIO buffer manipulation functions */

/*==== Local defines =========================================================*/

/* Some bogus con handle */
#define TEST_CON_HANDLE 1

/* Max number of test connections required */
#define TEST_MAX_CON    4


/*
 * Command handler
 */

/* The base GDD interface functions */
static char *app_gdd_init(app_cmd_entry_t *, int, char * [], core_func_t) ;
static char *app_gdd_deinit(app_cmd_entry_t *, int, char * [], core_func_t) ;
static char *app_gdd_connect(app_cmd_entry_t *, int, char * [], core_func_t) ;
static char *app_gdd_disconnect(app_cmd_entry_t *, int, char * [], core_func_t) ;
static char *app_gdd_get_send_buffer(app_cmd_entry_t *, int, char * [], core_func_t) ;
static char *app_gdd_send_data(app_cmd_entry_t *, int, char * [], core_func_t) ;
static char *app_gdd_receive_data(app_cmd_entry_t *, int, char * [], core_func_t) ;
static char *app_gdd_sig_ready_rcv(app_cmd_entry_t *, int, char * [], core_func_t) ;

/* More complex test functions encapsulated in functions */
static char *app_gdd_init_test_parms(app_cmd_entry_t *, int, char * [], core_func_t) ;
static char *app_gdd_deinit_test_parms(app_cmd_entry_t *, int, char * [], core_func_t) ;
static char *app_gdd_connect_test_parms(app_cmd_entry_t *, int, char * [], core_func_t) ;
static char *app_gdd_disconnect_test_parms(app_cmd_entry_t *, int, char * [], core_func_t) ;
static char *app_gdd_get_send_buffer_test_parms(app_cmd_entry_t *, int, char * [], core_func_t) ;
static char *app_gdd_send_data_test_parms(app_cmd_entry_t *, int, char * [], core_func_t) ;

static char *app_gdd_immediate_disconnect(app_cmd_entry_t *, int, char * [], core_func_t) ;

#ifdef WIN32
/* This function uses psi stub functions which are only available in simulation testing */
static char *app_gdd_test_sig_sbuf_avail(app_cmd_entry_t *, int, char * [], core_func_t) ;
#endif /* WIN32 */

/*extern char *app_gdd_sys_test(app_cmd_entry_t *, int, char * [], core_func_t) ;*/
static char *app_gdd_test_helper_functions(app_cmd_entry_t *, int, char * [], core_func_t) ;

/*
 * Core functions (to be called by the command handler, if applicable)
 */

/* -- No core functions required for now */


/* Command handler table. */
static app_cmd_entry_t app_gdd_cmd_table[] = {
  /* Commands that trigger the basic GDD API functions */
  { "gdd_init",             (cmd_handler_t)app_gdd_init,           (core_func_t)0,                  "[BAT|APP] {HowManyCon} [NO|YES]" },
  { "gdd_deinit",           (cmd_handler_t)app_gdd_deinit,          (core_func_t)0,                  "[BAT|APP]" },
  { "gdd_connect",          (cmd_handler_t)app_gdd_connect,         (core_func_t)0,                  "[BAT|APP]" },
  { "gdd_disconnect",       (cmd_handler_t)app_gdd_disconnect,      (core_func_t)0,                  "{ConNum}" },
  { "gdd_get_send_buffer",  (cmd_handler_t)app_gdd_get_send_buffer, (core_func_t)0,                  "{ConNum} {NumBytes}" },
  { "gdd_test_send_data",   (cmd_handler_t)app_gdd_send_data,       (core_func_t)0,                  "{ConNum} {NumBytes}" },
  { "gdd_test_receive_data",(cmd_handler_t)app_gdd_receive_data,    (core_func_t)0,                  "{ConNum} {NumBytes}" },
  { "gdd_sig_ready_rcv",    (cmd_handler_t)app_gdd_sig_ready_rcv,   (core_func_t)0,                  "{ConNum}" },
  /* Commands that test the input parameters of the API functions.
     These tests are to make sure that the API can handler crappy user input/parameter values. */
  { "gdd_init_test_parms",            (cmd_handler_t)app_gdd_init_test_parms,             (core_func_t)0, "" },
  { "gdd_deinit_test_parms",          (cmd_handler_t)app_gdd_deinit_test_parms,           (core_func_t)0, "" },
  { "gdd_connect_test_parms",         (cmd_handler_t)app_gdd_connect_test_parms,          (core_func_t)0, "" },
  { "gdd_disconnect_test_parms",      (cmd_handler_t)app_gdd_disconnect_test_parms,       (core_func_t)0, "" },
  { "gdd_get_send_buffer_test_parms", (cmd_handler_t)app_gdd_get_send_buffer_test_parms,  (core_func_t)0, "" },
  { "gdd_send_data_test_parms",       (cmd_handler_t)app_gdd_send_data_test_parms,        (core_func_t)0, "" },
  /* Test miscellaneous API function / combinations / special cases */
  { "gdd_test_immediate_disconnect",  (cmd_handler_t)app_gdd_immediate_disconnect,        (core_func_t)0, "" },
#ifdef WIN32
  { "gdd_test_sig_sbuf_avail",        (cmd_handler_t)app_gdd_test_sig_sbuf_avail,         (core_func_t)0, "" },
#endif /* WIN32 */
  { "gdd_helper_test",                (cmd_handler_t)app_gdd_test_helper_functions,       (core_func_t)0, "" },
/*  { "gdd_sys_test",                   app_gdd_sys_test,                    0, "" },  */
  { 0, 0, 0, 0}, /* Terminate table */
} ;


/*==== Local data ============================================================*/

#define GDD_MTU_SIZE  1500

static const T_GDD_FUNC * gdd_func = &gdd_func_dio;

static const  T_GDD_DIO_CAP gdd_dio_cap = { GDD_MTU_SIZE };


/** Local structure for maintaining the test connections */
typedef struct
{
  T_GDD_CON_HANDLE con_handle;
  T_GDD_BUF * send_buf;
  T_GDD_BUF * rcv_buf;
  int         cnt_sigtype_send_buf_available;
} APP_GDD_CON_TABLE_ENTRY;

static APP_GDD_CON_TABLE_ENTRY con_table[TEST_MAX_CON];

/*==== Local functions =======================================================*/


/** Create capabilities structure */
static T_GDD_CAP gdd_create_capabilities_300()
{
  T_GDD_CAP cap;
  cap.dio_cap = gdd_dio_cap;
  return cap;
}


static void app_gdd_setup_segment(U8 * ptr, U16 size, U16 offset)
{
  int i;
  for(i=0; i < size; ++i)
  {
    ptr[i] = (U8)i + offset;
  }
}


/** Setup the data bytes in provided buffer in order to create
 * a well-defined test buffer. The test data is created according
 * to a simple algorithm. The size is also used as an offset
 * in the algorithm.
 */
void app_gdd_setup_test_buffer(T_GDD_BUF * buf, U16 size)
{
  void * tmp_buf;
  MALLOC(tmp_buf, size);

  app_gdd_setup_segment(tmp_buf, size, size);

  if(gdd_write_buf(tmp_buf, size, buf) < 0)
  {
    TRACE_ASSERT(0);
  }

  MFREE(tmp_buf);
}

/** Verify the integrity of a test buffer that has been created
 * before with the function app_gdd_setup_test_buffer().
 *
 * Return TRUE if two buffers are the same, FALSE otherwise */
BOOL app_gdd_verify_test_buffer(const T_GDD_BUF * buf, U16 size)
{
  char * tmp_buf_ref;
  char * tmp_buf_received;
  MALLOC(tmp_buf_ref, size);
  MALLOC(tmp_buf_received, size);

  app_gdd_setup_segment((U8 *)tmp_buf_ref, size, size);

  if(gdd_read_buf(buf, (U8 *)tmp_buf_received, size) < 0)
  {
    TRACE_ASSERT(0);
  }

  if(memcmp(tmp_buf_ref, tmp_buf_received, size))
  {
    return FALSE;
  }

  return TRUE;
}


/** Convert from instance string representation to the numeric instance number */
int get_inst_num(const char * str)
{
  if(!strncmp("BAT", str, 3))
    return GDD_INST_BAT;
  else if(!strncmp("APP", str, 3))
    return GDD_INST_APP;
  else if(!strncmp("TCP", str, 3))
    return GDD_INST_TCP;
  else if(!strncmp("SOCK", str, 4))
    return GDD_INST_SOCK;
  else if(!strncmp("SOCKCFG", str, 7))
    return GDD_INST_SOCKCFG;
  else
    return -1; /* invalid instance */
}


/* For Target, we must include the two DIO buffer helper functions,
   because we don't include the PSI stub, where they reside for simulation build. */
#if defined (_TARGET_)

void copy_dio_buf(const T_dio_buffer * buf_in, T_dio_buffer ** buf_out)
{
  int i;
  T_dio_segment * seg_in;
  T_dio_segment * seg_out;

  TRACE_FUNCTION( "copy_dio_buf" );

  /* Allocate new buffer */
  MALLOC((*buf_out), (USHORT)sizeof(T_dio_buffer));
  (*buf_out)->c_dio_segment = buf_in->c_dio_segment;
  (*buf_out)->length=buf_in->length; 

  /* allocate segement array and copy data accross */
  MALLOC((*buf_out)->ptr_dio_segment,(USHORT)sizeof(T_dio_segment)*buf_in->c_dio_segment);
  memcpy((*buf_out)->ptr_dio_segment,buf_in->ptr_dio_segment,(USHORT)sizeof(T_dio_segment)*buf_in->c_dio_segment);
  (*buf_out)->c_dio_segment = buf_in->c_dio_segment;

  /* Copy each segment */
  seg_in  = buf_in->ptr_dio_segment;
  seg_out = (*buf_out)->ptr_dio_segment;
  for(i=0;i<buf_in->c_dio_segment;++i, ++seg_in, ++seg_out )
  {
    seg_out->c_data = seg_in->c_data;
    MALLOC(seg_out->ptr_data, seg_out->c_data);
    memcpy(seg_out->ptr_data, seg_in->ptr_data, seg_out->c_data);
  }

  TRACE_EVENT_P1("Allocated new DIO buffer %x", *buf_out);
}

#define TEM__GDD_DIO_MTU_SIZE_MAX  1500

void allocate_rx_dio_buf(T_dio_buffer ** buf_out)
{
  const U16 segments[] = {2, TEM__GDD_DIO_MTU_SIZE_MAX};

  TRACE_FUNCTION( "allocate_rx_dio_buf" );

  allocate_dio_buf(buf_out, segments, 2);
}


void allocate_dio_buf(T_dio_buffer ** buf_out,
                             const U16 seg_size[], U16 num_seg)
{
  int idx_seg;
  
  TRACE_FUNCTION( "allocate_rx_dio_buf" );

  /* allocate segement array and copy data accross */
  MALLOC(*buf_out, (USHORT)sizeof(T_dio_buffer));
  (*buf_out)->c_dio_segment = (U8)num_seg;
  MALLOC((*buf_out)->ptr_dio_segment,(USHORT)sizeof(T_dio_segment)*(*buf_out)->c_dio_segment);
  (*buf_out)->length = 0;
  
  for(idx_seg=0; idx_seg<num_seg; ++idx_seg)
  {
    T_dio_segment * seg = &((*buf_out)->ptr_dio_segment[idx_seg]);
    seg->c_data = seg_size[idx_seg];
    MALLOC(seg->ptr_data, seg->c_data);
    (*buf_out)->length += seg->c_data;
  }
}

void free_dio_buf(T_dio_buffer ** buf)
{
  int i;
  T_dio_segment * seg = (*buf)->ptr_dio_segment;

  TRACE_FUNCTION( "free_dio_buf" );

  /* Free each segement */
  for(i=0;i<(*buf)->c_dio_segment; ++i, ++seg)
  {
    if (seg->ptr_data NEQ NULL)
      MFREE(seg->ptr_data);
  }

  /* Free segment array */
  if ((*buf)->ptr_dio_segment NEQ NULL)
    MFREE((*buf)->ptr_dio_segment);

  /* Free the actual buffer */
  MFREE(*buf);

  (*buf) = 0;
}

#endif /* (_TARGET_) */


/*---------------------------------------------------------------------------
 * GDD callbacks
 *---------------------------------------------------------------------------*/

/** gdd_receive_data_cb */
GDD_RESULT app_gdd_receive_data_cb
( T_GDD_CON_HANDLE    con_handle,
  T_GDD_BUF *         buf )
{
  int i;

  TRACE_FUNCTION("app_gdd_receive_data_cb");

  /* Search connection slot */
  for(i = 0; i < TEST_MAX_CON; ++i)
  {
    if(con_table[i].con_handle EQ con_handle)
    {
      T_GDD_BUF * new_buf;

      if(con_table[i].rcv_buf)
      {
        TRACE_ERROR("Receive buffer already in use");
        return GDD_INTERNAL_ERROR;
      }

      /* Create a copy of the buffer */
      copy_dio_buf(
        (const T_dio_buffer *)buf,
        (T_dio_buffer **)(&new_buf));

      con_table[i].rcv_buf = new_buf;

      return GDD_OK;
    }
  }

  TRACE_ERROR("callback called with bad connection handle");
  return GDD_INTERNAL_ERROR;
}

/** gdd_signal_cb */
GDD_RESULT app_gdd_signal_cb
( T_GDD_CON_HANDLE    con_handle,
  T_GDD_SIGNAL        sig )
{
  int i;

  TRACE_FUNCTION("app_gdd_signal_cb");

  /* Search connection slot */
  for(i = 0; i < TEST_MAX_CON; ++i)
  {
    if(con_table[i].con_handle EQ con_handle)
    {
      if(sig.sig EQ GDD_SIGTYPE_SEND_BUF_AVAILABLE)
      {
        ++con_table[i].cnt_sigtype_send_buf_available;
      }
      /* Processing of other signals goes here ... */

      return GDD_OK;

    }
  }

  /* In case of the connection signal, we might not have a connection slot
     at this stage, this it is not considered an error. */
  if( sig.sig NEQ GDD_SIGTYPE_CONNECTION_OPENED )
    TRACE_ERROR("callback called with bad connection handle");

  return GDD_INTERNAL_ERROR;
}


/*---------------------------------------------------------------------------
 * Definition of command handler functions
 *---------------------------------------------------------------------------*/

static char *app_gdd_init
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  GDD_RESULT result;
  char * mem = 0;
  int num_con = get_item(argv[2], 4, TRUE);
  int inst_id = get_inst_num(argv[1]);
  if(inst_id < 0)
  {
    TRACE_ERROR("Invalid instance specified");
    BAT_TEST_FAILED();
    return 0;
  }
    
  TRACE_FUNCTION("app_gdd_init");

  if(argv[3] && !strcmp(string_to_lower(argv[3]), "YES"))
  {
    MALLOC(mem, num_con * GDD_DIO_SIZEOF_CONDATA);
  }

  result = gdd_func->gdd_init((T_GDD_INST_ID)inst_id, (char*)mem, (U16)num_con) ;
  if(result != GDD_OK)
  {
      TRACE_ERROR("Call to gdd_init failed");
      BAT_TEST_FAILED();
      return 0;
  }

  BAT_TEST_PASSED();
  return 0;
}

static char *app_gdd_deinit
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  GDD_RESULT result;
  int inst_id = get_inst_num(argv[1]);
  if(inst_id < 0)
  {
    TRACE_ERROR("Invalid instance specified");
    BAT_TEST_FAILED();
    return 0;
  }

  TRACE_FUNCTION("app_gdd_deinit");

  result =(GDD_RESULT)gdd_func->gdd_deinit((T_GDD_INST_ID)inst_id) ;
  if(result != GDD_OK)
  {
      TRACE_ERROR("Call to gdd_deinit failed");
      BAT_TEST_FAILED();
      return 0;
  }

  BAT_TEST_PASSED();
  return 0;
}

static char *app_gdd_connect
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  GDD_RESULT result;
  int i = 0;
  const T_GDD_CAP gdd_cap = gdd_create_capabilities_300();
  int inst_id = get_inst_num(argv[1]);
  if(inst_id < 0)
  {
    TRACE_ERROR("Invalid instance specified");
    BAT_TEST_FAILED();
    return 0;
  }

  TRACE_FUNCTION("app_gdd_connect");


  /* Find free slot for connection handle */
  while(i < TEST_MAX_CON && con_table[i].con_handle != 0)
  {
    ++i;
  }
  if(i EQ TEST_MAX_CON)
  {
      TRACE_ERROR("No free slot for connection handle");
      BAT_TEST_FAILED();
      return 0;
  }

  result = gdd_func->gdd_connect
    ((T_GDD_INST_ID)inst_id, &(con_table[i].con_handle), &gdd_cap,(T_GDD_RECEIVE_DATA_CB)app_gdd_receive_data_cb,(T_GDD_SIGNAL_CB) app_gdd_signal_cb);
  if(result != GDD_OK)
  {
      TRACE_ERROR("Call to gdd_connect failed");
      BAT_TEST_FAILED();
      return 0;
  }

  BAT_TEST_PASSED();
  return 0;
}

static char *app_gdd_disconnect
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  GDD_RESULT result;
  int con_num; /* connection number -index into local static connection table */

  TRACE_FUNCTION("app_gdd_disconnect");

  con_num = get_item(argv[1], 0, FALSE);
  if(con_num >= TEST_MAX_CON)
  {
      TRACE_ERROR("Connection number out of bounds");
      BAT_TEST_FAILED();
      return 0;
  }
  if(con_table[con_num].con_handle EQ 0)
  {
      TRACE_ERROR("Connection number not valid - no connection handle");
      BAT_TEST_FAILED();
      return 0;
  }

  result = gdd_func->gdd_disconnect(con_table[con_num].con_handle);
  if(result != GDD_OK)
  {
      TRACE_ERROR("Call to gdd_disconnect failed");
      BAT_TEST_FAILED();
      return 0;
  }

  /* Free our application buffers which we have allocated ourselfs */
  if(con_table[con_num].rcv_buf)
  {
    free_dio_buf((T_dio_buffer **)(&con_table[con_num].rcv_buf));
    con_table[con_num].rcv_buf = 0;
  }
  
  con_table[con_num].con_handle = 0;

  BAT_TEST_PASSED();
  return 0;
}

static char *app_gdd_get_send_buffer
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  GDD_RESULT result;
  int con_num; /* connection number -index into local static connection table */
  int num_bytes;
  T_GDD_BUF * buf;

  TRACE_FUNCTION("app_gdd_get_send_buffer");

  con_num = get_item(argv[1], 0, TRUE);
  if(con_num >= TEST_MAX_CON)
  {
      TRACE_ERROR("Connection number out of bounds");
      BAT_TEST_FAILED();
      return 0;
  }
  if(con_table[con_num].con_handle EQ 0)
  {
      TRACE_ERROR("Connection number not valid - no connection handle");
      BAT_TEST_FAILED();
      return 0;
  }

  num_bytes = get_item(argv[2], 100, FALSE);

  result = gdd_func->gdd_get_send_buffer(con_table[con_num].con_handle, &buf, (U16)num_bytes);
  if(result != GDD_OK)
  {
      TRACE_ERROR("Call to gdd_get_send_buffer failed");
      BAT_TEST_FAILED();
      return 0;
  }

  con_table[con_num].send_buf = buf;

  BAT_TEST_PASSED();
  return 0;
}

static char *app_gdd_send_data
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  GDD_RESULT result;
  int con_num; /* connection number -index into local static connection table */
  int num_bytes;
  T_GDD_BUF * buf;

  TRACE_FUNCTION("app_gdd_send_data");

  con_num = get_item(argv[1], 0, TRUE);
  if(con_num >= TEST_MAX_CON)
  {
      TRACE_ERROR("Connection number out of bounds");
      BAT_TEST_FAILED();
      return 0;
  }
  if(con_table[con_num].con_handle EQ 0)
  {
      TRACE_ERROR("Connection number not valid - no connection handle");
      BAT_TEST_FAILED();
      return 0;
  }
  buf = con_table[con_num].send_buf;
  if(buf EQ 0)
  {
      TRACE_ERROR("No buffer for sending");
      BAT_TEST_FAILED();
      return 0;
  }

  num_bytes = get_item(argv[2], 100, FALSE);

  app_gdd_setup_test_buffer(buf, (U16)num_bytes);

  result = gdd_func->gdd_send_data(con_table[con_num].con_handle, buf);
  if(result != GDD_OK)
  {
      TRACE_ERROR("Call to gdd_get_send_buffer failed");
      BAT_TEST_FAILED();
      return 0;
  }

  con_table[con_num].send_buf = 0;

  BAT_TEST_PASSED();
  return 0;
}



static char *app_gdd_receive_data
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  int con_num; /* connection number -index into local static connection table */
  int num_bytes;
  T_GDD_BUF * buf;

  TRACE_FUNCTION("app_gdd_receive_data");

  con_num = get_item(argv[1], 0, FALSE);
  if(con_num >= TEST_MAX_CON)
  {
      TRACE_ERROR("Connection number out of bounds");
      BAT_TEST_FAILED();
      return 0;
  }
  if(con_table[con_num].con_handle EQ 0)
  {
      TRACE_ERROR("Connection number not valid - no connection handle");
      BAT_TEST_FAILED();
      return 0;
  }
  buf = con_table[con_num].rcv_buf;
  if(buf EQ 0)
  {
      TRACE_ERROR("No buffer received");
      BAT_TEST_FAILED();
      return 0;
  }

  num_bytes = get_item(argv[2], 100, FALSE);

  /* Compare/verify the received buffer */
  if(app_gdd_verify_test_buffer(buf, (U16)num_bytes) EQ FALSE)
  {
      TRACE_ERROR("Verification of received buffer failed");
      BAT_TEST_FAILED();
      return 0;
  }

  /* Free our application buffers which we have allocated ourselfs */
  if(con_table[con_num].rcv_buf)
  {
    free_dio_buf((T_dio_buffer **)(&con_table[con_num].rcv_buf));
    con_table[con_num].rcv_buf = 0;
  }

  BAT_TEST_PASSED();
  return 0;
}



static char *app_gdd_sig_ready_rcv
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  int con_num; /* connection number -index into local static connection table */

  TRACE_FUNCTION("app_gdd_sig_ready_rcv");

  con_num = get_item(argv[1], 0, FALSE);
  if(con_num >= TEST_MAX_CON)
  {
      TRACE_ERROR("Connection number out of bounds");
      BAT_TEST_FAILED();
      return 0;
  }
  if(con_table[con_num].con_handle EQ 0)
  {
      TRACE_ERROR("Connection number not valid - no connection handle");
      BAT_TEST_FAILED();
      return 0;
  }

  gdd_func->gdd_signal_ready_rcv(con_table[con_num].con_handle);

  BAT_TEST_PASSED();
  return 0;
}


static char *app_gdd_init_test_parms
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  GDD_RESULT result;

  TRACE_FUNCTION("app_gdd_init_test_parms");

  /*
   * Test range of user ID
   */
  /*lint -e778*/
  result = gdd_func->gdd_init(GDD_INST_NONE, (void *)0, (U16)4) ;
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (user = GDD_INST_BAT - 1)");
    BAT_TEST_FAILED();
    return 0;
  }
  result = gdd_func->gdd_init(GDD_NUM_INSTS, 0, 4);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (user = GDD_NUM_INSTS)");
    BAT_TEST_FAILED();
    return 0;
  }

  /*
   * Nothing to do for the mem parameter
   * (should already have been tested before)
   */

  /*
   * Test number of connections
   */
  result = gdd_func->gdd_init(GDD_INST_BAT, 0, 0);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (num_con = 0)");
    BAT_TEST_FAILED();
    return 0;
  }
  result = gdd_func->gdd_init(GDD_INST_BAT, 0, TEST_MAX_CON+1);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (num_con = TEST_MAX_CON+1)");
    BAT_TEST_FAILED();
    return 0;
  }

  BAT_TEST_PASSED();
  return 0;
}

static char *app_gdd_deinit_test_parms
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  GDD_RESULT result;

  TRACE_FUNCTION("app_gdd_deinit_test_parms");

  /*
   * Test range of user ID
   */
  result = gdd_func->gdd_deinit(GDD_INST_NONE) ; /*lint -e778*/
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (user = GDD_INST_BAT - 1)");
    BAT_TEST_FAILED();
    return 0;
  }
  result = gdd_func->gdd_deinit(GDD_NUM_INSTS);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (user = GDD_NUM_INSTS)");
    BAT_TEST_FAILED();
    return 0;
  }

  BAT_TEST_PASSED();
  return 0;
}

static char *app_gdd_connect_test_parms
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  GDD_RESULT result;
  const T_GDD_CAP gdd_cap = gdd_create_capabilities_300();

  TRACE_FUNCTION("app_gdd_connect_test_parms");

  /*
   * Test range of user ID
   */
  result = gdd_func->gdd_connect(GDD_INST_NONE, &(con_table[0].con_handle),  /*lint -e778*/
    &gdd_cap, (T_GDD_RECEIVE_DATA_CB)app_gdd_receive_data_cb, (T_GDD_SIGNAL_CB)app_gdd_signal_cb);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (user = GDD_INST_BAT - 1)");
    BAT_TEST_FAILED();
    return 0;
  }
  result = gdd_func->gdd_connect(GDD_NUM_INSTS, &(con_table[0].con_handle),
    &gdd_cap, (T_GDD_RECEIVE_DATA_CB)app_gdd_receive_data_cb, (T_GDD_SIGNAL_CB)app_gdd_signal_cb);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (user = GDD_NUM_INSTS)");
    BAT_TEST_FAILED();
    return 0;
  }

  /* Test handling of con_handle = 0 */
  result = gdd_func->gdd_connect(GDD_INST_BAT, 0,
    &gdd_cap, (T_GDD_RECEIVE_DATA_CB)app_gdd_receive_data_cb, (T_GDD_SIGNAL_CB)app_gdd_signal_cb);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (con_handle = 0)");
    BAT_TEST_FAILED();
    return 0;
  }

  /* Test handling of cap = 0 */
  result = gdd_func->gdd_connect(GDD_INST_BAT, &(con_table[0].con_handle),
    0, (T_GDD_RECEIVE_DATA_CB)app_gdd_receive_data_cb, (T_GDD_SIGNAL_CB)app_gdd_signal_cb);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (cap = 0)");
    BAT_TEST_FAILED();
    return 0;
  }

  /* Test handling of rcv_cb = 0 */
  result = gdd_func->gdd_connect(GDD_INST_BAT, &(con_table[0].con_handle),
    &gdd_cap, 0,(T_GDD_SIGNAL_CB) app_gdd_signal_cb);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (rcv_cb = 0)");
    BAT_TEST_FAILED();
    return 0;
  }

  /* Test handling of sig_cb = 0 */
  result = gdd_func->gdd_connect(GDD_INST_BAT, &(con_table[0].con_handle),
    &gdd_cap, app_gdd_receive_data_cb, 0);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (sig_cb = 0)");
    BAT_TEST_FAILED();
    return 0;
  }

  BAT_TEST_PASSED();
  return 0;
}

static char *app_gdd_disconnect_test_parms
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  GDD_RESULT result;

  TRACE_FUNCTION("app_gdd_disconnect_test_parms");

  /* Test handling of con_handle = 0 */
  result = gdd_func->gdd_disconnect(0);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (con_handle = 0)");
    BAT_TEST_FAILED();
    return 0;
  }

  BAT_TEST_PASSED();
  return 0;
}

static char *app_gdd_get_send_buffer_test_parms
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  GDD_RESULT result;
  T_GDD_BUF * buf;
  
  TRACE_FUNCTION("app_gdd_get_send_buffer_test_parms");

  /* Test handling of con_handle = 0 */
  result = gdd_func->gdd_get_send_buffer(0, &buf, GDD_MTU_SIZE);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (con_handle = 0)");
    BAT_TEST_FAILED();
    return 0;
  }

  /* Test handling of buf = 0 */
  result = gdd_func->gdd_get_send_buffer(TEST_CON_HANDLE, 0, GDD_MTU_SIZE);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (buf = 0)");
    BAT_TEST_FAILED();
    return 0;
  }

  /* Test handling of size = 0 */
  result = gdd_func->gdd_get_send_buffer(TEST_CON_HANDLE, &buf, 0);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (size = 0)");
    BAT_TEST_FAILED();
    return 0;
  }
  /* Test handling of size >  GDD_MTU_SIZE */
  result = gdd_func->gdd_get_send_buffer(TEST_CON_HANDLE, &buf, GDD_MTU_SIZE+1);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (size = GDD_MTU_SIZE+1)");
    BAT_TEST_FAILED();
    return 0;
  }

  BAT_TEST_PASSED();
  return 0;
}

static char *app_gdd_send_data_test_parms
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  GDD_RESULT result;
  T_GDD_BUF buf;

  TRACE_FUNCTION("app_gdd_send_data_test_parms");

   /* Test handling of con_handle = 0 */
  result = gdd_func->gdd_send_data(0, &buf);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (con_handle = 0)");
    BAT_TEST_FAILED();
    return 0;
  }

  /* Test handling of buf = 0 */
  result = gdd_func->gdd_send_data(TEST_CON_HANDLE, 0);
  if(result NEQ GDD_INVALID_PARAMS)
  {
    TRACE_ERROR("BAD param value *NOT* detected (buf = 0)");
    BAT_TEST_FAILED();
    return 0;
  }

  BAT_TEST_PASSED();
  return 0;
}


/*
 * Test connect, followed by immedate disconnect without waiting for connect signal
 */
static char *app_gdd_immediate_disconnect
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  GDD_RESULT result;
  int con_idx = 0;
  
  int user_id = get_item(argv[1], GDD_INST_BAT, TRUE);
  const T_GDD_CAP gdd_cap = gdd_create_capabilities_300();

  TRACE_FUNCTION("app_gdd_immediate_disconnect");

  /* Find free slot for connection handle */
  while(con_idx < TEST_MAX_CON && con_table[con_idx].con_handle != 0)
  {
    ++con_idx;
  }
  if(con_idx EQ TEST_MAX_CON)
  {
    TRACE_ERROR("No free slot for connection handle");
    BAT_TEST_FAILED();
    return 0;
  }
  
  result = gdd_func->gdd_connect
    ((T_GDD_INST_ID)user_id, &(con_table[con_idx].con_handle), &gdd_cap,
            (T_GDD_RECEIVE_DATA_CB) app_gdd_receive_data_cb, (T_GDD_SIGNAL_CB)app_gdd_signal_cb);
  if(result != GDD_OK)
  {
    TRACE_ERROR("Call to gdd_connect failed");
    BAT_TEST_FAILED();
    return 0;
  }
  TRACE_EVENT_P2("Created connection, con_handle = %d (slot %d)", con_table[con_idx].con_handle, con_idx);
  
  /* Don't sleep - but you might want to activate for debugging */
  /*vsi_t_sleep (VSI_CALLER 2000);*/
  
  result = gdd_func->gdd_disconnect(con_table[con_idx].con_handle);
  if(result != GDD_OK)
  {
    TRACE_ERROR("Call to gdd_disconnect failed");
    BAT_TEST_FAILED();
    return 0;
  }
  TRACE_EVENT_P1("Disconnected connection, con_handle = %d", con_table[con_idx].con_handle);
  
  /* Free connection slot */
  con_table[con_idx].con_handle = 0;

  BAT_TEST_PASSED();
  return 0;
}


#ifdef WIN32

/*
 * The the correct sending of the signal GDD_SIGTYPE_SEND_BUF_AVAILABLE
 */
static char *app_gdd_test_sig_sbuf_avail
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  GDD_RESULT result;
  T_GDD_BUF * buf1, * buf2, * buf3;
  int con_idx = 0;
  int user_id = get_item(argv[1], GDD_INST_BAT, TRUE);
  const T_GDD_CAP gdd_cap = gdd_create_capabilities_300();

  TRACE_FUNCTION("app_gdd_test_sig_sbuf_avail");

  /* Switch off the automatic provision of send buffers in the PSI stub */
  psi_stub_send_rx_buf_after_read(FALSE);

  /* Find free slot for connection handle */
  while(con_idx < TEST_MAX_CON && con_table[con_idx].con_handle != 0)
  {
    ++con_idx;
  }
  if(con_idx EQ TEST_MAX_CON)
  {
    TRACE_ERROR("No free slot for connection handle");
    BAT_TEST_FAILED();
    return 0;
  }

  /* Connect & disconnect */
  result = gdd_func->gdd_connect
    (user_id, &(con_table[con_idx].con_handle), &gdd_cap, app_gdd_receive_data_cb, app_gdd_signal_cb);
  if(result != GDD_OK)
  {
    TRACE_ERROR("Call to gdd_connect failed");
    BAT_TEST_FAILED();
    return 0;
  }
  result = gdd_func->gdd_disconnect(con_table[con_idx].con_handle);
  if(result != GDD_OK)
  {
    TRACE_ERROR("Call to gdd_disconnect failed");
    BAT_TEST_FAILED();
    return 0;
  }

  /* Reset test counter */  
  con_table[con_idx].cnt_sigtype_send_buf_available = 0;

  /* Create connection */
  result = gdd_func->gdd_connect
    (user_id, &(con_table[con_idx].con_handle), &gdd_cap, app_gdd_receive_data_cb, app_gdd_signal_cb);
  if(result != GDD_OK)
  {
    TRACE_ERROR("Call to gdd_connect failed");
    BAT_TEST_FAILED();
    return 0;
  }
  TRACE_EVENT_P2("Created connection, con_handle = %d (slot %d)", con_table[con_idx].con_handle, con_idx);

  /* Wait for 2 seconds to be sure that we will have the send buffer by then. */
  vsi_t_sleep (VSI_CALLER 2000);

  /* Get a first send buffer */
  result = gdd_func->gdd_get_send_buffer(con_table[con_idx].con_handle, &buf1, GDD_MTU_SIZE);
  if(result NEQ GDD_OK)
  {
    TRACE_ERROR("Error: gdd_get_send_buffer() failed");
    BAT_TEST_FAILED();
    return 0;
  }

  result = gdd_func->gdd_get_send_buffer(con_table[con_idx].con_handle, &buf2, GDD_MTU_SIZE);
  if(result NEQ GDD_NO_BUF_AVAILABLE)
  {
    TRACE_ERROR("gdd_get_send_buffer() did not fail as expected");
    BAT_TEST_FAILED();
    return 0;
  }

  /* Now we send back the first buffer. */
  result = gdd_func->gdd_send_data(con_table[con_idx].con_handle, buf1);
  if(result NEQ GDD_OK)
  {
    TRACE_ERROR("gdd_send_data() failed");
    BAT_TEST_FAILED();
    return 0;
  }

  /* Now we can get the second send buffer */
  result = gdd_func->gdd_get_send_buffer(con_table[con_idx].con_handle, &buf2, GDD_MTU_SIZE);
  if(result NEQ GDD_OK)
  {
    TRACE_ERROR("gdd_get_send_buffer failed");
    BAT_TEST_FAILED();
    return 0;
  }

  /* Remove the receive buffer previously received */
  free_dio_buf((T_dio_buffer **)(&con_table[con_idx].rcv_buf));
  con_table[con_idx].rcv_buf = 0;

  /* Now we send back the 2nd buffer buffer. */
  result = gdd_func->gdd_send_data(con_table[con_idx].con_handle, buf2);
  if(result NEQ GDD_OK)
  {
    TRACE_ERROR("gdd_send_data() failed");
    BAT_TEST_FAILED();
    return 0;
  }

  /* When we try to get the third buffer, we should now fail,
     as we switched of automatic provision of RX buffers at the beginning */
  result = gdd_func->gdd_get_send_buffer(con_table[con_idx].con_handle, &buf3, GDD_MTU_SIZE);
  if(result NEQ GDD_NO_BUF_AVAILABLE)
  {
    TRACE_ERROR("gdd_get_send_buffer should have failed with GDD_NO_BUF_AVAILABLE");
    BAT_TEST_FAILED();
    return 0;
  }

  /* Create a new RX buffer in the PSI stub */
  psi_stub_provide_rx_buf();

  /* Now we should be able to get the new buffer */
  result = gdd_func->gdd_get_send_buffer(con_table[con_idx].con_handle, &buf3, GDD_MTU_SIZE);
  if(result NEQ GDD_OK)
  {
    TRACE_ERROR("gdd_get_send_buffer failed");
    BAT_TEST_FAILED();
    return 0;
  }

  /* Two signals GDD_SIGTYPE_SEND_BUF_AVAILABLE should have been sent
     (one after the connection, and one when we called pst_stub_provide_rx_buf
      the last time) */
  if(con_table[con_idx].cnt_sigtype_send_buf_available NEQ 1)
  {
    TRACE_ERROR("Wrong value in cnt_sigtype_send_buf_available");
    BAT_TEST_FAILED();
    return 0;
  }

  result = gdd_func->gdd_disconnect(con_table[con_idx].con_handle);
  if(result != GDD_OK)
  {
    TRACE_ERROR("Call to gdd_disconnect failed");
    BAT_TEST_FAILED();
    return 0;
  }
  TRACE_EVENT_P1("Disconnected connection, con_handle = %d", con_table[con_idx].con_handle);

  /* Free connection slot */
  con_table[con_idx].con_handle = 0;

  /* Switch on the automatic provision of send buffers in the PSI stub */
  psi_stub_send_rx_buf_after_read(TRUE);

  BAT_TEST_PASSED();
  return 0;
}

#endif /* WIN32 */



/*---------------------------------------------------------------------------
 * Testing helper functions.
 *---------------------------------------------------------------------------*/

static U32 test_gdd_write_buf()
{
  T_dio_buffer * dest_buf;
  char src_buf[250];

  allocate_rx_dio_buf(&dest_buf);
  if(dest_buf->c_dio_segment != 2)
  {
    return((U32) -1);
  }
  if(dest_buf->length < (sizeof(src_buf) + 2 /*PID*/))
  {
    return((U32) -1);
  }

  app_gdd_setup_segment((U8 *)src_buf, sizeof(src_buf), 22 /* arbitrary offset */);

  if(gdd_write_buf(( U8 *)src_buf, sizeof(src_buf), (T_GDD_BUF*)dest_buf) < 0)
  {
    return((U32) -1);
  }

  if(memcmp(src_buf, dest_buf->ptr_dio_segment[1].ptr_data, sizeof(src_buf)))
  {
    return((U32) -1);
  }

  return 0;
}


/*
 * Test the function 'gdd_read_buf()'. Return 0 if OK.
 */
static U32 test_gdd_read_buf()
{
  char src_buf[500];
  char dest_buf[500];
  T_dio_buffer * dio_buf;

  /* Totol size of 502 bytes over 6 segments (incl.PID in first seg) */  
  const U16 segments[] = {2, 15, 150, 35, 100, 200};
  
  allocate_dio_buf(&dio_buf, segments, sizeof(segments)/sizeof(U16));

  app_gdd_setup_segment((U8 *)src_buf, sizeof(src_buf), 22 /* arbitrary offset */);

  if(gdd_write_buf((U8 *)src_buf, sizeof(src_buf), (T_GDD_BUF*)dio_buf) < 0)
  {
    return((U32)-1);
  }

  if(gdd_read_buf((T_GDD_BUF*)dio_buf, (U8 *)dest_buf, sizeof(dest_buf)) < 0)
  {
    return((U32) -1);
  }

  if(memcmp(src_buf, dest_buf, sizeof(src_buf)))
  {
    return((U32)-1);
  }

  return 0;
}


static char *app_gdd_test_helper_functions
(app_cmd_entry_t *cmd_entry_ptr, int argc,
 char * argv[], core_func_t core_func)
{
  if(test_gdd_write_buf())
  {
    BAT_TEST_FAILED();
  }
  if(test_gdd_read_buf())
  {
    BAT_TEST_FAILED();
  }

  BAT_TEST_PASSED();
  return 0;
}


/*---------------------------------------------------------------------------
 * Definition of core functions
 *---------------------------------------------------------------------------*/

/* -- No core functions required for now */


/*==== Exported functions ====================================================*/

char *app_handle_command_gdd(char *command)
{
  return app_handle_command(command, app_gdd_cmd_table);
}


/* EOF */
