/* 
+------------------------------------------------------------------------------
|  File:       app_bat.c
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
|  Purpose :  Test functions for testing the BAT interface -- command parser.
+----------------------------------------------------------------------------- 
*/ 


#define APP_BAT_C

/*==== INCLUDES =============================================================*/

#include "app_util.h"
#include "bat.h"
#include "bat_ctrl.h"
#include "bat_intern.h"


#include <string.h>             /* String functions, e. g. strncpy(). */
#include <ctype.h>
#include <stdlib.h>

#include "vsi.h"                /* A lot of macros. */
#ifndef _SIMULATION_
#include "custom.h"
#include "gsm.h"                /* A lot of macros. */
#include "tools.h"              /* Common tools. */
#endif /* _SIMULATION_ */

#include "bat_cfg.h"
#include "p_app.h"

#ifdef FF_GPF_TCPIP
#include "socket_api.h"
#endif

#include "app.h"


#define NO_ADD_VAL 0xFFFFFFFF

GLOBAL void app_bat_send_result_prim (U8 result, U32 add_info);

/*
 * Command handler
 */
GLOBAL char *app_bat_close(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_cmd_data(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_complete_test(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_flow_control_test1(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_uns_rcv_and_abort_test(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_multi_channel_creation_test(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_ctrl(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_deinit(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_delete(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_init(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_new(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_open(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_receive_data_cb(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cpin(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cnmi(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_ata(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_atd(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_ath(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_q_cscb(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_q_cpinf(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_q_cpol(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_t_cpol(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cfun(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cmgs(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cmgl(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cops(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_ccwa(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_signal_cb(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_uns_open (app_cmd_entry_t *, char *, char *, char *, core_func_t );

GLOBAL char *app_bat_send_cpbr(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cpbw(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_q_cgatt(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_q_cgact(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cgatt(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cgact(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cgdcont(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cgpco(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_q_cgpco(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_quer_set_clck (app_cmd_entry_t *, char *, char *, char *, core_func_t );
GLOBAL char *app_bat_send_per_als (app_cmd_entry_t *, char *, char *, char *, core_func_t );
GLOBAL char *app_bat_send_q_cscs(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_q_clcc(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cscs(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_csms(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cgsms(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cpbf(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cgmi(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cgmm(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cgmr(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cgsn(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_stress_test(app_cmd_entry_t *, char *, char *, char *, core_func_t);
GLOBAL char *app_bat_send_cniv(app_cmd_entry_t *, char *, char *,char *, core_func_t);
GLOBAL char *app_bat_send_q_cniv(app_cmd_entry_t *, char *, char *,char *, core_func_t);
GLOBAL char *app_bat_send_custom(app_cmd_entry_t *, char *, char *, char *, core_func_t);

/*
 * Core functions (to be called by the command handler, if applicable)
 */

/* -- No core functions required for now */


/* Command handler table. */
static app_cmd_entry_t app_bat_cmd_table[] = {
  { "bat_init",               app_bat_init,                 0,   "[InstNum]" },
  { "bat_deinit",             app_bat_deinit,               0,   "" },
  { "bat_new",                app_bat_new,                  0,   "[ClntNum]" },
  { "bat_delete",             app_bat_delete,               0,   "[InstHndl]" },
  { "bat_open",               app_bat_open,                 0,   "[InstHndl]" },
  { "bat_uns_open",           app_bat_uns_open,             0,   "" },
  { "bat_close",              app_bat_close,                0,   "[ClntHndl]"},
  { "bat_ctrl",               app_bat_ctrl,                 0,   "" },
  { "bat_receive_data_cb",    app_bat_receive_data_cb,      0,   "[ConHndl]" },
  { "bat_signal_cb",          app_bat_signal_cb,            0,   "[ConHndl][SigNum]" },
  { "bat_complete_test",      app_bat_complete_test,       0,   "" },
  { "bat_flow_control_test1",     app_bat_flow_control_test1,       0,   "" },
  { "bat_uns_rcv_and_abort_test",     app_bat_uns_rcv_and_abort_test,       0,   "" },
  { "bat_multi_channel_creation_test",     app_bat_multi_channel_creation_test,       0,   "" },
  { "bat_send_q_cscb",        app_bat_send_q_cscb,          0,   "[ClntHndl]" },
  { "bat_send_q_clcc",        app_bat_send_q_clcc,          0,   "[ClntHndl]" },
  { "bat_send_q_cscs",        app_bat_send_q_cscs,          0,   "[ClntHndl]" },
  { "bat_send_q_cpinf",       app_bat_send_q_cpinf,         0,   "[ClntHndl]" },
  { "bat_send_q_cpol",        app_bat_send_q_cpol,          0,   "[ClntHndl]" },
  { "bat_send_q_cgatt",       app_bat_send_q_cgatt,         0,   "[ClntHndl]" },
  { "bat_send_q_cgact",       app_bat_send_q_cgact,         0,   "[ClntHndl]" },
  { "bat_send_q_cgpco",       app_bat_send_q_cgpco,         0,   "[ClntHndl]" },
  { "bat_send_t_cpol",        app_bat_send_t_cpol,          0,   "[ClntHndl]" },
  { "bat_send_cfun",          app_bat_send_cfun,            0,   "[ClntHndl]" },  
  { "bat_send_cgmi",        app_bat_send_cgmi,          0,   "[ClntHndl]" },
  { "bat_send_cgmm",        app_bat_send_cgmm,          0,   "[ClntHndl]" },
  { "bat_send_cgmr",        app_bat_send_cgmr,          0,   "[ClntHndl]" },
  { "bat_send_cgsn",        app_bat_send_cgsn,          0,   "[ClntHndl]" },
  { "bat_send_cmgs",          app_bat_send_cmgs,            0,   "[ClntHndl]" },  
  { "bat_send_cmgl",          app_bat_send_cmgl,            0,   "[ClntHndl] [UNREAD|READ|UNSENT|SENT|ALL]" },  
  { "bat_send_cops",          app_bat_send_cops,            0,   "[ClntHndl]" },  
  { "bat_send_cpin",          app_bat_send_cpin,            0,   "[ClntHndl]" },
  { "bat_send_cnmi",          app_bat_send_cnmi,            0,   "[ClntHndl]" },  
  { "bat_send_csms",          app_bat_send_csms,            0,   "[ClntHndl] [service]" },  
  { "bat_send_cgsms",         app_bat_send_cgsms,           0,   "[ClntHndl] [service]" },  
  { "bat_send_ccwa",          app_bat_send_ccwa,            0,   "[ClntHndl]" },
  { "bat_send_ata",           app_bat_send_ata,             0,   "[ClntHndl]" },
  { "bat_send_atd",           app_bat_send_atd,             0,   "[ClntHndl] [phone number]" },
  { "bat_send_ath",           app_bat_send_ath,             0,   "[ClntHndl]" },
  { "bat_send_cscs",          app_bat_send_cscs,            0,   "[ClntHndl] [charset]" },
  { "bat_send_cpbr",           app_bat_send_cpbr,           0,   "[ClntHndl] [index]" },
  { "bat_send_cpbw",          app_bat_send_cpbw,            0,   "[ClntHndl] [index] [name]" },
  { "bat_send_cpbf",          app_bat_send_cpbf,            0,   "[ClntHndl] [search string]" },
  { "bat_send_cgatt",         app_bat_send_cgatt,           0,   "[ClntHndl]" },
  { "bat_send_cgact",         app_bat_send_cgact,           0,   "[ClntHndl]" },
  { "bat_send_cgdcont",       app_bat_send_cgdcont,         0,   "[ClntHndl]" },
  { "bat_send_cgpco",         app_bat_send_cgpco,           0,   "[ClntHndl]" },
  { "bat_clck_query",         app_bat_quer_set_clck,        0,   "[ConHndl] [ConCom]" },
  { "bat_send_per_als",       app_bat_send_per_als,         0,   "[ConHndl] [ConCom]" },
  { "bat_stress_test",        app_bat_stress_test,          0,   "[ClntHndl]" }, 
  { "bat_send_cniv",         app_bat_send_cniv,                 0,  "[ClntHndl]"},
  { "bat_send_q_cniv",         app_bat_send_q_cniv,                 0,  "[ClntHndl]"},
  { "bat_send_custom",        app_bat_send_custom,          0,   "[ClntHndl] [custom string]" }, 
  { 0,                         0,                           0,   0},
} ;


/*==== Local data ============================================================*/

typedef struct
{
  size_t size;
} T_app_map_response_2_size;

static T_app_map_response_2_size response_2_size[] =
{
  {sizeof(T_BAT_no_parameter)},            /* BAT_RES_AT_OK                  = 0x0     */
  {sizeof(T_BAT_res_at_connect)},          /* BAT_RES_AT_CONNECT             = 0x1     */
  {sizeof(T_BAT_no_parameter)},            /* BAT_RES_AT_NO_CARRIER_FINAL    = 0x2     */
  {sizeof(T_BAT_res_plus_cme_error)},      /* BAT_RES_PLUS_CME_ERROR         = 0x3     */
  {sizeof(T_BAT_res_plus_cms_error)},      /* BAT_RES_PLUS_CMS_ERROR         = 0x4     */
  {sizeof(T_BAT_no_parameter)},            /* BAT_RES_AT_BUSY                = 0x5     */
  {sizeof(T_BAT_no_parameter)},            /* BAT_RES_AT_CHANNEL_BUSY        = 0x6    */       
  {sizeof(T_BAT_no_parameter)},            /* BAT_RES_AT_NO_ANSWER           = 0x7    */       
  {sizeof(T_BAT_res_plus_ext_error)},      /* BAT_RES_PLUS_EXT_ERROR         = 0x8    */       
  {sizeof(T_BAT_res_que_percent_als)},     /* BAT_RES_QUE_PERCENT_ALS        = 0x9    */       
  {sizeof(T_BAT_res_tst_percent_als)},     /* BAT_RES_TST_PERCENT_ALS        = 0xa    */       
  {sizeof(T_BAT_res_que_percent_atr)},     /* BAT_RES_QUE_PERCENT_ATR        = 0xb    */       
  {sizeof(T_BAT_res_que_percent_band)},    /* BAT_RES_QUE_PERCENT_BAND       = 0xc    */       
  {sizeof(T_BAT_res_tst_percent_band)},    /* BAT_RES_TST_PERCENT_BAND       = 0xd    */       
  {sizeof(T_BAT_res_que_plus_cacm)},       /* BAT_RES_QUE_PLUS_CACM          = 0xe    */       
  {sizeof(T_BAT_res_que_plus_camm)},       /* BAT_RES_QUE_PLUS_CAMM          = 0xf    */       
  {sizeof(T_BAT_res_set_plus_caoc)},       /* BAT_RES_SET_PLUS_CAOC          = 0x10   */       
  {sizeof(T_BAT_res_set_plus_cbc)},        /* BAT_RES_SET_PLUS_CBC           = 0x11   */       
  {sizeof(T_BAT_res_que_percent_cbhz)},    /* BAT_RES_QUE_PERCENT_CBHZ       = 0x12   */       
  {sizeof(T_BAT_res_tst_percent_cbhz)},    /* BAT_RES_TST_PERCENT_CBHZ       = 0x13   */       
  {sizeof(T_BAT_res_que_plus_cbst)},       /* BAT_RES_QUE_PLUS_CBST          = 0x14   */       
  {sizeof(T_BAT_res_que_percent_ccbs)},    /* BAT_RES_QUE_PERCENT_CCBS       = 0x15   */       
  {sizeof(T_BAT_res_set_plus_ccfc)},       /* BAT_RES_SET_PLUS_CCFC          = 0x16   */       
  {sizeof(T_BAT_res_que_plus_cclk)},       /* BAT_RES_QUE_PLUS_CCLK          = 0x17   */       
  {sizeof(T_BAT_res_que_plus_ccug)},       /* BAT_RES_QUE_PLUS_CCUG          = 0x18   */       
  {sizeof(T_BAT_res_set_plus_ccwa)},       /* BAT_RES_SET_PLUS_CCWA          = 0x19   */       
  {sizeof(T_BAT_res_set_plus_ccwa_w)},     /* BAT_RES_SET_PLUS_CCWA_W        = 0x1a   */       
  {sizeof(T_BAT_res_set_plus_ceer)},       /* BAT_RES_SET_PLUS_CEER          = 0x1b   */       
  {sizeof(T_BAT_res_que_plus_cfun)},       /* BAT_RES_QUE_PLUS_CFUN          = 0x1c   */       
  {sizeof(T_BAT_res_que_percent_cgaatt)},  /* BAT_RES_QUE_PERCENT_CGAATT     = 0x1d   */       
  {sizeof(T_BAT_res_que_plus_cgact)},      /* BAT_RES_QUE_PLUS_CGACT         = 0x1e   */       
  {sizeof(T_BAT_res_que_plus_cgatt)},      /* BAT_RES_QUE_PLUS_CGATT         = 0x1f   */       
  {sizeof(T_BAT_res_que_percent_cgclass)}, /* BAT_RES_QUE_PERCENT_CGCLASS    = 0x20   */       
  {sizeof(T_BAT_res_que_plus_cgclass)},    /* BAT_RES_QUE_PLUS_CGCLASS       = 0x21   */       
  {sizeof(T_BAT_res_que_plus_cgdcont)},    /* BAT_RES_QUE_PLUS_CGDCONT       = 0x22   */       
  {sizeof(T_BAT_res_set_plus_cgpaddr)},    /* BAT_RES_SET_PLUS_CGPADDR       = 0x23   */       
  {sizeof(T_BAT_res_tst_plus_cgpaddr)},    /* BAT_RES_TST_PLUS_CGPADDR       = 0x24   */       
  {sizeof(T_BAT_res_set_percent_cgpco)},   /* BAT_RES_SET_PERCENT_CGPCO      = 0x25   */       
  {sizeof(T_BAT_res_que_percent_cgppp)},   /* BAT_RES_QUE_PERCENT_CGPPP      = 0x26   */       
  {sizeof(T_BAT_res_que_plus_cgqmin)},     /* BAT_RES_QUE_PLUS_CGQMIN        = 0x27   */       
  {sizeof(T_BAT_res_que_plus_cgqreq)},     /* BAT_RES_QUE_PLUS_CGQREQ        = 0x28   */       
  {sizeof(T_BAT_res_que_percent_cgreg)},   /* BAT_RES_QUE_PERCENT_CGREG      = 0x29   */       
  {sizeof(T_BAT_res_que_plus_cgreg)},      /* BAT_RES_QUE_PLUS_CGREG         = 0x2a   */       
  {sizeof(T_BAT_res_que_plus_cgsms)},      /* BAT_RES_QUE_PLUS_CGSMS         = 0x2b   */       
  {sizeof(T_BAT_res_set_percent_chpl)},    /* BAT_RES_SET_PERCENT_CHPL       = 0x2c   */       
  {sizeof(T_BAT_res_set_percent_chpl_w)},  /* BAT_RES_SET_PERCENT_CHPL_W     = 0x2d   */       
  {sizeof(T_BAT_res_set_plus_cimi)},       /* BAT_RES_SET_PLUS_CIMI          = 0x2e   */       
  {sizeof(T_BAT_res_que_plus_cind)},       /* BAT_RES_QUE_PLUS_CIND          = 0x2f   */       
  {sizeof(T_BAT_res_que_plus_clan)},       /* BAT_RES_QUE_PLUS_CLAN          = 0x30   */       
  {sizeof(T_BAT_res_tst_plus_clan)},       /* BAT_RES_TST_PLUS_CLAN          = 0x31   */       
  {sizeof(T_BAT_res_que_plus_clcc)},       /* BAT_RES_QUE_PLUS_CLCC          = 0x32   */       
  {sizeof(T_BAT_res_set_plus_clck)},       /* BAT_RES_SET_PLUS_CLCK          = 0x33   */       
  {sizeof(T_BAT_res_que_plus_clip)},       /* BAT_RES_QUE_PLUS_CLIP          = 0x34   */       
  {sizeof(T_BAT_res_que_plus_clip_w)},     /* BAT_RES_QUE_PLUS_CLIP_W        = 0x35   */       
  {sizeof(T_BAT_res_que_plus_clir)},       /* BAT_RES_QUE_PLUS_CLIR          = 0x36   */       
  {sizeof(T_BAT_no_parameter)},            /* BAT_RES_SET_PLUS_CLVL          = 0x37   */       
  {sizeof(T_BAT_res_que_plus_clvl)},       /* BAT_RES_QUE_PLUS_CLVL          = 0x38   */       
  {sizeof(T_BAT_res_que_plus_cmer)},       /* BAT_RES_QUE_PLUS_CMER          = 0x39   */       
  {sizeof(T_BAT_res_set_plus_cmgc)},       /* BAT_RES_SET_PLUS_CMGC          = 0x3a   */       
  {sizeof(T_BAT_res_set_plus_cmgl)},       /* BAT_RES_SET_PLUS_CMGL          = 0x3b   */       
  {sizeof(T_BAT_res_set_plus_cmgl_w)},     /* BAT_RES_SET_PLUS_CMGL_W        = 0x3c   */       
  {sizeof(T_BAT_res_set_plus_cmgr)},       /* BAT_RES_SET_PLUS_CMGR          = 0x3d   */       
  {sizeof(T_BAT_res_set_plus_cmgr_w)},     /* BAT_RES_SET_PLUS_CMGR_W        = 0x3e   */       
  {sizeof(T_BAT_res_set_plus_cmgs)},       /* BAT_RES_SET_PLUS_CMGS          = 0x3f   */       
  {sizeof(T_BAT_res_set_plus_cmgw)},       /* BAT_RES_SET_PLUS_CMGW          = 0x40   */       
  {sizeof(T_BAT_res_que_plus_cmod)},       /* BAT_RES_QUE_PLUS_CMOD          = 0x41   */       
  {sizeof(T_BAT_res_que_plus_cmut)},       /* BAT_RES_QUE_PLUS_CMUT          = 0x42   */       
  {sizeof(T_BAT_res_que_plus_cmux)},       /* BAT_RES_QUE_PLUS_CMUX          = 0x43   */       
  {sizeof(T_BAT_res_que_percent_cnap)},    /* BAT_RES_QUE_PERCENT_CNAP       = 0x44   */       
  {sizeof(T_BAT_res_que_plus_cnmi)},       /* BAT_RES_QUE_PLUS_CNMI          = 0x45   */       
  {sizeof(T_BAT_res_set_plus_cnum)},       /* BAT_RES_SET_PLUS_CNUM          = 0x46   */       
  {sizeof(T_BAT_res_set_plus_cnum_w)},     /* BAT_RES_SET_PLUS_CNUM_W        = 0x47   */       
  {sizeof(T_BAT_res_que_plus_colp)},       /* BAT_RES_QUE_PLUS_COLP          = 0x48   */       
  {sizeof(T_BAT_res_que_plus_colp_w)},     /* BAT_RES_QUE_PLUS_COLP_W        = 0x49   */       
  {sizeof(T_BAT_res_que_percent_cops)},    /* BAT_RES_QUE_PERCENT_COPS       = 0x4a   */       
  {sizeof(T_BAT_res_tst_percent_cops)},    /* BAT_RES_TST_PERCENT_COPS       = 0x4b   */       
  {sizeof(T_BAT_res_que_plus_cops)},       /* BAT_RES_QUE_PLUS_COPS          = 0x4c   */       
  {sizeof(T_BAT_res_tst_plus_cops)},       /* BAT_RES_TST_PLUS_COPS          = 0x4d   */       
  {sizeof(T_BAT_res_que_percent_cops_w)},  /* BAT_RES_QUE_PERCENT_COPS_W     = 0x4e   */       
  {sizeof(T_BAT_res_tst_percent_cops_w)},  /* BAT_RES_TST_PERCENT_COPS_W     = 0x4f   */       
  {sizeof(T_BAT_res_set_percent_cpals)},   /* BAT_RES_SET_PERCENT_CPALS      = 0x50   */       
  {sizeof(T_BAT_res_que_percent_cpals)},   /* BAT_RES_QUE_PERCENT_CPALS      = 0x51   */       
  {sizeof(T_BAT_res_set_percent_cpals_w)}, /* BAT_RES_SET_PERCENT_CPALS_W    = 0x52   */       
  {sizeof(T_BAT_res_que_percent_cpals_w)}, /* BAT_RES_QUE_PERCENT_CPALS_W    = 0x53   */       
  {sizeof(T_BAT_res_set_plus_cpas)},       /* BAT_RES_SET_PLUS_CPAS          = 0x54   */       
  {sizeof(T_BAT_res_set_plus_cpbf)},       /* BAT_RES_SET_PLUS_CPBF          = 0x55   */       
  {sizeof(T_BAT_res_tst_plus_cpbf)},       /* BAT_RES_TST_PLUS_CPBF          = 0x56   */       
  {sizeof(T_BAT_res_set_plus_cpbf_w)},     /* BAT_RES_SET_PLUS_CPBF_W        = 0x57   */       
  {sizeof(T_BAT_res_tst_plus_cpbf_w)},     /* BAT_RES_TST_PLUS_CPBF_W        = 0x58   */       
  {sizeof(T_BAT_res_set_plus_cpbr)},       /* BAT_RES_SET_PLUS_CPBR          = 0x59   */       
  {sizeof(T_BAT_res_tst_plus_cpbr)},       /* BAT_RES_TST_PLUS_CPBR          = 0x5a   */       
  {sizeof(T_BAT_res_set_plus_cpbr_w)},     /* BAT_RES_SET_PLUS_CPBR_W        = 0x5b   */       
  {sizeof(T_BAT_res_tst_plus_cpbr_w)},     /* BAT_RES_TST_PLUS_CPBR_W        = 0x5c   */       
  {sizeof(T_BAT_res_que_percent_cpbs)},    /* BAT_RES_QUE_PERCENT_CPBS       = 0x5d   */         
  {sizeof(T_BAT_res_que_plus_cpbs)},       /* BAT_RES_QUE_PLUS_CPBS          = 0x5e    */
  {sizeof(T_BAT_res_tst_plus_cpbw)},       /* BAT_RES_TST_PLUS_CPBW          = 0x5f    */
  {sizeof(T_BAT_res_tst_plus_cpbw_w)},     /* BAT_RES_TST_PLUS_CPBW_W        = 0x60    */
  {sizeof(T_BAT_res_set_percent_cpcfu)},   /* BAT_RES_SET_PERCENT_CPCFU      = 0x61    */
  {sizeof(T_BAT_res_que_percent_cphs)},    /* BAT_RES_QUE_PERCENT_CPHS       = 0x62    */
  {sizeof(T_BAT_res_que_plus_cpin)},       /* BAT_RES_QUE_PLUS_CPIN          = 0x63    */
  {sizeof(T_BAT_res_que_percent_cpinf)},   /* BAT_RES_QUE_PERCENT_CPINF      = 0x64    */
  {sizeof(T_BAT_res_set_percent_cpmb)},    /* BAT_RES_SET_PERCENT_CPMB       = 0x65    */
  {sizeof(T_BAT_res_que_percent_cpmb)},    /* BAT_RES_QUE_PERCENT_CPMB       = 0x66   */        
  {sizeof(T_BAT_res_tst_percent_cpmbw)},   /* BAT_RES_TST_PERCENT_CPMBW      = 0x67   */       
  {sizeof(T_BAT_res_tst_percent_cpmbw_w)}, /* BAT_RES_TST_PERCENT_CPMBW_W    = 0x68   */       
  {sizeof(T_BAT_res_set_percent_cpmb_w)},  /* BAT_RES_SET_PERCENT_CPMB_W     = 0x69   */       
  {sizeof(T_BAT_res_set_plus_cpms)},       /* BAT_RES_SET_PLUS_CPMS          = 0x6a   */       
  {sizeof(T_BAT_res_que_plus_cpms)},       /* BAT_RES_QUE_PLUS_CPMS          = 0x6b   */       
  {sizeof(T_BAT_res_set_percent_cpnums)},  /* BAT_RES_SET_PERCENT_CPNUMS     = 0x6c   */       
  {sizeof(T_BAT_res_tst_percent_cpnums)},  /* BAT_RES_TST_PERCENT_CPNUMS     = 0x6d   */       
  {sizeof(T_BAT_res_set_percent_cpnums_w)},/* BAT_RES_SET_PERCENT_CPNUMS_W   = 0x6e   */       
  {sizeof(T_BAT_res_tst_percent_cpnums_w)},/* BAT_RES_TST_PERCENT_CPNUMS_W   = 0x6f   */       
  {sizeof(T_BAT_res_que_plus_cpol)},       /* BAT_RES_QUE_PLUS_CPOL          = 0x70   */       
  {sizeof(T_BAT_res_tst_plus_cpol)},       /* BAT_RES_TST_PLUS_CPOL          = 0x71   */       
  {sizeof(T_BAT_res_que_plus_cpol_w)},     /* BAT_RES_QUE_PLUS_CPOL_W        = 0x72   */       
  {sizeof(T_BAT_res_tst_plus_cpol_w)},     /* BAT_RES_TST_PLUS_CPOL_W        = 0x73   */       
  {sizeof(T_BAT_res_que_percent_cpopn)},   /* BAT_RES_QUE_PERCENT_CPOPN      = 0x74   */       
  {sizeof(T_BAT_res_que_percent_cprsm)},   /* BAT_RES_QUE_PERCENT_CPRSM      = 0x75   */       
  {sizeof(T_BAT_res_que_plus_cpuc)},       /* BAT_RES_QUE_PLUS_CPUC          = 0x76   */       
  {sizeof(T_BAT_res_que_plus_cpuc_w)},     /* BAT_RES_QUE_PLUS_CPUC_W        = 0x77   */       
  {sizeof(T_BAT_res_set_percent_cpvwi)},   /* BAT_RES_SET_PERCENT_CPVWI      = 0x78   */       
  {sizeof(T_BAT_res_que_percent_creg)},    /* BAT_RES_QUE_PERCENT_CREG       = 0x79   */       
  {sizeof(T_BAT_res_tst_plus_cres)},       /* BAT_RES_TST_PLUS_CRES          = 0x7a   */       
  {sizeof(T_BAT_res_que_plus_crlp)},       /* BAT_RES_QUE_PLUS_CRLP          = 0x7b   */       
  {sizeof(T_BAT_res_set_plus_crsm)},       /* BAT_RES_SET_PLUS_CRSM          = 0x7c   */       
  {sizeof(T_BAT_res_tst_plus_csas)},       /* BAT_RES_TST_PLUS_CSAS          = 0x7d   */       
  {sizeof(T_BAT_res_que_plus_csca)},       /* BAT_RES_QUE_PLUS_CSCA          = 0x7e   */       
  {sizeof(T_BAT_res_que_plus_cscb)},       /* BAT_RES_QUE_PLUS_CSCB          = 0x7f   */       
  {sizeof(T_BAT_res_que_plus_cscs)},       /* BAT_RES_QUE_PLUS_CSCS          = 0x80   */       
  {sizeof(T_BAT_res_set_plus_csim)},       /* BAT_RES_SET_PLUS_CSIM          = 0x81   */       
  {sizeof(T_BAT_res_que_plus_csms)},       /* BAT_RES_QUE_PLUS_CSMS          = 0x82   */       
  {sizeof(T_BAT_res_que_plus_csns)},       /* BAT_RES_QUE_PLUS_CSNS          = 0x83   */       
  {sizeof(T_BAT_res_que_percent_csq)},     /* BAT_RES_QUE_PERCENT_CSQ        = 0x84   */       
  {sizeof(T_BAT_res_set_plus_csq)},        /* BAT_RES_SET_PLUS_CSQ           = 0x85   */       
  {sizeof(T_BAT_res_que_plus_csta)},       /* BAT_RES_QUE_PLUS_CSTA          = 0x86   */       
  {sizeof(T_BAT_res_que_plus_csvm)},       /* BAT_RES_QUE_PLUS_CSVM          = 0x87   */       
  {sizeof(T_BAT_res_que_percent_ctty)},    /* BAT_RES_QUE_PERCENT_CTTY       = 0x88   */       
  {sizeof(T_BAT_res_que_plus_ctzu)},       /* BAT_RES_QUE_PLUS_CTZU          = 0x89   */       
  {sizeof(T_BAT_res_que_percent_cust)},    /* BAT_RES_QUE_PERCENT_CUST       = 0x8a   */       
  {sizeof(T_BAT_res_que_percent_data)},    /* BAT_RES_QUE_PERCENT_DATA       = 0x8b   */       
  {sizeof(T_BAT_res_set_percent_dinf)},    /* BAT_RES_SET_PERCENT_DINF       = 0x8c   */       
  {sizeof(T_BAT_res_que_plus_ds)},         /* BAT_RES_QUE_PLUS_DS            = 0x8d   */       
  {sizeof(T_BAT_res_que_plus_fap)},        /* BAT_RES_QUE_PLUS_FAP           = 0x8e   */       
  {sizeof(T_BAT_res_que_plus_fbo)},        /* BAT_RES_QUE_PLUS_FBO           = 0x8f   */       
  {sizeof(T_BAT_res_que_plus_fbs)},        /* BAT_RES_QUE_PLUS_FBS           = 0x90   */       
  {sizeof(T_BAT_res_que_plus_fbu)},        /* BAT_RES_QUE_PLUS_FBU           = 0x91   */       
  {sizeof(T_BAT_res_que_plus_fcc)},        /* BAT_RES_QUE_PLUS_FCC           = 0x92   */       
  {sizeof(T_BAT_res_que_plus_fclass)},     /* BAT_RES_QUE_PLUS_FCLASS        = 0x93   */       
  {sizeof(T_BAT_res_que_plus_fcq)},        /* BAT_RES_QUE_PLUS_FCQ           = 0x94   */       
  {sizeof(T_BAT_res_que_plus_fcr)},        /* BAT_RES_QUE_PLUS_FCR           = 0x95   */       
  {sizeof(T_BAT_res_que_plus_fcs)},        /* BAT_RES_QUE_PLUS_FCS           = 0x96   */       
  {sizeof(T_BAT_res_que_plus_fct)},        /* BAT_RES_QUE_PLUS_FCT           = 0x97   */       
  {sizeof(T_BAT_res_que_plus_fea)},        /* BAT_RES_QUE_PLUS_FEA           = 0x98   */       
  {sizeof(T_BAT_res_que_plus_ffc)},        /* BAT_RES_QUE_PLUS_FFC           = 0x99   */       
  {sizeof(T_BAT_res_que_plus_fhs)},        /* BAT_RES_QUE_PLUS_FHS           = 0x9a   */       
  {sizeof(T_BAT_res_que_plus_fie)},        /* BAT_RES_QUE_PLUS_FIE           = 0x9b   */       
  {sizeof(T_BAT_res_que_plus_fis)},        /* BAT_RES_QUE_PLUS_FIS           = 0x9c   */       
  {sizeof(T_BAT_res_que_plus_fit)},        /* BAT_RES_QUE_PLUS_FIT           = 0x9d   */       
  {sizeof(T_BAT_res_que_plus_fli)},        /* BAT_RES_QUE_PLUS_FLI           = 0x9e   */       
  {sizeof(T_BAT_res_que_plus_flo)},        /* BAT_RES_QUE_PLUS_FLO           = 0x9f   */       
  {sizeof(T_BAT_res_que_plus_flp)},        /* BAT_RES_QUE_PLUS_FLP           = 0xa0   */       
  {sizeof(T_BAT_res_que_plus_fms)},        /* BAT_RES_QUE_PLUS_FMS           = 0xa1   */       
  {sizeof(T_BAT_res_que_plus_fns)},        /* BAT_RES_QUE_PLUS_FNS           = 0xa2   */       
  {sizeof(T_BAT_res_que_plus_fpa)},        /* BAT_RES_QUE_PLUS_FPA           = 0xa3   */       
  {sizeof(T_BAT_res_que_plus_fpi)},        /* BAT_RES_QUE_PLUS_FPI           = 0xa4   */       
  {sizeof(T_BAT_res_que_plus_fps)},        /* BAT_RES_QUE_PLUS_FPS           = 0xa5   */       
  {sizeof(T_BAT_res_que_plus_fpw)},        /* BAT_RES_QUE_PLUS_FPW           = 0xa6   */       
  {sizeof(T_BAT_res_que_plus_frq)},        /* BAT_RES_QUE_PLUS_FRQ           = 0xa7   */       
  {sizeof(T_BAT_res_que_plus_fsa)},        /* BAT_RES_QUE_PLUS_FSA           = 0xa8   */       
  {sizeof(T_BAT_res_que_plus_fsp)},        /* BAT_RES_QUE_PLUS_FSP           = 0xa9   */       
  {sizeof(T_BAT_res_que_plus_icf)},        /* BAT_RES_QUE_PLUS_ICF           = 0xaa   */       
  {sizeof(T_BAT_res_que_plus_ifc)},        /* BAT_RES_QUE_PLUS_IFC           = 0xab   */       
  {sizeof(T_BAT_res_que_percent_imei)},    /* BAT_RES_QUE_PERCENT_IMEI       = 0xac   */       
  {sizeof(T_BAT_res_que_plus_ipr)},        /* BAT_RES_QUE_PLUS_IPR           = 0xad   */       
  {sizeof(T_BAT_res_que_percent_pbcf)},    /* BAT_RES_QUE_PERCENT_PBCF       = 0xae   */       
  {sizeof(T_BAT_res_que_percent_ppp)},     /* BAT_RES_QUE_PERCENT_PPP        = 0xaf   */       
  {sizeof(T_BAT_res_que_percent_pvrf)},    /* BAT_RES_QUE_PERCENT_PVRF       = 0xb0   */       
  {sizeof(T_BAT_res_que_percent_rdl)},     /* BAT_RES_QUE_PERCENT_RDL        = 0xb1   */       
  {sizeof(T_BAT_res_que_percent_rdlb)},    /* BAT_RES_QUE_PERCENT_RDLB       = 0xb2   */       
  {sizeof(T_BAT_res_que_percent_satc)},    /* BAT_RES_QUE_PERCENT_SATC       = 0xb3   */       
  {sizeof(T_BAT_res_que_percent_satcc)},   /* BAT_RES_QUE_PERCENT_SATCC      = 0xb4   */       
  {sizeof(T_BAT_res_set_percent_sate)},    /* BAT_RES_SET_PERCENT_SATE       = 0xb5   */       
  {sizeof(T_BAT_res_que_plus_ws46)},        /* BAT_RES_QUE_PLUS_WS46          = 0xb6   */
  {sizeof(T_BAT_res_que_plus_cdip)},        /* BAT_RES_QUE_PLUS_CDIP         = 0xb7   */
  {sizeof(T_BAT_res_set_percent_cmgr)},    /* BAT_RES_SET_PERCENT_CMGR       = 0xb8   */
  {sizeof(T_BAT_res_set_percent_cmgl)},    /* BAT_RES_SET_PERCENT_CMGL       = 0xb9   */
  {sizeof(T_BAT_res_set_plus_cmss)},       /* BAT_RES_SET_PLUS_CMSS          = 0xba   */
  {sizeof(T_BAT_res_set_plus_cgmi)},       /* BAT_RES_SET_PLUS_CGMI          = 0xbb  */ 
  {sizeof(T_BAT_res_set_plus_cgmm)},       /* BAT_RES_SET_PLUS_CGMM          = 0xbc  */ 
  {sizeof(T_BAT_res_set_plus_cgmr)},       /* BAT_RES_SET_PLUS_CGMR          = 0xbd   */ 
  {sizeof(T_BAT_res_set_plus_cgsn)},       /* BAT_RES_SET_PLUS_CGSN          = 0xbe */
  {0},          /*As response structure of command id=0xbf is absent still  to get the proper size of structure of response according to the command id  */                                   
  {0},          /*As response structure of command id=0xc0 is absent still to get the proper size of structure of response according to the command id  */
  {sizeof(T_BAT_res_que_percent_cniv)},     /* BAT_RES_QUE_PERCENT_CNIV        = 0xc1   */
};

static T_app_map_response_2_size unsolicited_2_size[] =
{
  {sizeof(T_BAT_no_parameter)},            /* BAT_RES_UNS_AT_BUSY            = 0x1000 */
  {sizeof(T_BAT_no_parameter)},            /* BAT_RES_UNS_AT_NO_ANSWER       = 0x1001 */
  {sizeof(T_BAT_res_uns_plus_cbm)},        /* BAT_RES_UNS_PLUS_CBM           = 0x1002 */
  {sizeof(T_BAT_res_uns_plus_cbmi)},       /* BAT_RES_UNS_PLUS_CBMI          = 0x1003 */
  {sizeof(T_BAT_res_uns_percent_ccbs)},    /* BAT_RES_UNS_PERCENT_CCBS       = 0x1004 */
//  {sizeof(T_BAT_res_uns_percent_cccm)},    /* BAT_RES_UNS_PERCENT_CCCM       = 0x1005 */
  {sizeof(T_BAT_res_uns_plus_cccm)},       /* BAT_RES_UNS_PLUS_CCCM          = 0x1006 */
  {sizeof(T_BAT_res_uns_percent_cccn)},    /* BAT_RES_UNS_PERCENT_CCCN       = 0x1007 */
  {sizeof(T_BAT_res_uns_plus_ccwa)},       /* BAT_RES_UNS_PLUS_CCWA          = 0x1008 */
  {sizeof(T_BAT_res_uns_plus_ccwa_w)},     /* BAT_RES_UNS_PLUS_CCWA_W        = 0x1009 */
  {sizeof(T_BAT_no_parameter)},            /* BAT_RES_UNS_PLUS_CCWE          = 0x100a */
  {sizeof(T_BAT_res_uns_plus_cds)},        /* BAT_RES_UNS_PLUS_CDS           = 0x100b */
  {sizeof(T_BAT_no_parameter)},            /* BAT_RES_UNS_PLUS_CCWV          = 0x100c */
  {sizeof(T_BAT_res_uns_percent_cgev)},    /* BAT_RES_UNS_PERCENT_CGEV       = 0x100d */
  {sizeof(T_BAT_res_uns_plus_cgev)},       /* BAT_RES_UNS_PLUS_CGEV          = 0x100e */
  {sizeof(T_BAT_res_uns_percent_cgreg)},   /* BAT_RES_UNS_PERCENT_CGREG      = 0x100f */
  {sizeof(T_BAT_res_uns_plus_cgreg)},      /* BAT_RES_UNS_PLUS_CGREG         = 0x1010 */
  {sizeof(T_BAT_res_uns_plus_clae)},       /* BAT_RES_UNS_PLUS_CLAE          = 0x1011 */
  {sizeof(T_BAT_res_uns_plus_clav)},       /* BAT_RES_UNS_PLUS_CLAV          = 0x1012 */
  {sizeof(T_BAT_res_uns_plus_clip)},       /* BAT_RES_UNS_PLUS_CLIP          = 0x1013 */
  {sizeof(T_BAT_res_uns_plus_clip_w)},     /* BAT_RES_UNS_PLUS_CLIP_W        = 0x1014 */
  {sizeof(T_BAT_res_uns_plus_cmt)},        /* BAT_RES_UNS_PLUS_CMT           = 0x1015 */
  {sizeof(T_BAT_res_uns_plus_cmti)},       /* BAT_RES_UNS_PLUS_CMTI          = 0x1016 */
  {sizeof(T_BAT_res_uns_plus_cmt_w)},      /* BAT_RES_UNS_PLUS_CMT_W         = 0x1017 */
  {sizeof(T_BAT_res_uns_percent_cnap)},    /* BAT_RES_UNS_PERCENT_CNAP       = 0x1018 */
  {sizeof(T_BAT_res_uns_plus_colp)},       /* BAT_RES_UNS_PLUS_COLP          = 0x1019 */
  {sizeof(T_BAT_res_uns_plus_colp_w)},     /* BAT_RES_UNS_PLUS_COLP_W        = 0x101a */
  {sizeof(T_BAT_res_uns_percent_cpi)},     /* BAT_RES_UNS_PERCENT_CPI        = 0x101b */
  {sizeof(T_BAT_res_uns_percent_cpi_w)},   /* BAT_RES_UNS_PERCENT_CPI_W      = 0x101c */
  {sizeof(T_BAT_res_uns_percent_cpri)},    /* BAT_RES_UNS_PERCENT_CPRI       = 0x101d */
  {sizeof(T_BAT_res_uns_percent_cproam)},  /* BAT_RES_UNS_PERCENT_CPROAM     = 0x101e */
  {sizeof(T_BAT_res_uns_percent_cpvwi)},   /* BAT_RES_UNS_PERCENT_CPVWI      = 0x101f */
  {sizeof(T_BAT_res_uns_plus_cr)},         /* BAT_RES_UNS_PLUS_CR            = 0x1020 */
  {sizeof(T_BAT_res_uns_percent_creg)},    /* BAT_RES_UNS_PERCENT_CREG       = 0x1021 */
  {sizeof(T_BAT_res_uns_plus_cring)},      /* BAT_RES_UNS_PLUS_CRING         = 0x1022 */
  {sizeof(T_BAT_res_uns_percent_csq)},     /* BAT_RES_UNS_PERCENT_CSQ        = 0x1023 */
  {sizeof(T_BAT_res_uns_plus_cssi)},       /* BAT_RES_UNS_PLUS_CSSI          = 0x1024 */
  {sizeof(T_BAT_res_uns_percent_cssn)},    /* BAT_RES_UNS_PERCENT_CSSN       = 0x1025 */
  {sizeof(T_BAT_res_uns_plus_cssu)},       /* BAT_RES_UNS_PLUS_CSSU          = 0x1026 */
  {sizeof(T_BAT_res_uns_percent_cstat)},   /* BAT_RES_UNS_PERCENT_CSTAT      = 0x1027 */
  {sizeof(T_BAT_res_uns_percent_ctyi)},    /* BAT_RES_UNS_PERCENT_CTYI       = 0x1028 */
  {sizeof(T_BAT_res_uns_plus_ctzr)},       /* BAT_RES_UNS_PLUS_CTZR          = 0x1029 */
  {sizeof(T_BAT_res_uns_percent_ctzv)},    /* BAT_RES_UNS_PERCENT_CTZV       = 0x102a */
  {sizeof(T_BAT_res_uns_plus_ctzv)},       /* BAT_RES_UNS_PLUS_CTZV          = 0x102b */
  {sizeof(T_BAT_res_uns_percent_ctzv_w)},  /* BAT_RES_UNS_PERCENT_CTZV_W     = 0x102c */
  {sizeof(T_BAT_res_uns_plus_cusd)},       /* BAT_RES_UNS_PLUS_CUSD          = 0x102d */
  {sizeof(T_BAT_res_uns_plus_cusd_w)},     /* BAT_RES_UNS_PLUS_CUSD_W        = 0x102e */
  {sizeof(T_BAT_no_parameter)},            /* BAT_RES_UNS_AT_NO_CARRIER      = 0x102f */
  {sizeof(T_BAT_res_uns_plus_fhs)},        /* BAT_RES_UNS_PLUS_FHS           = 0x1030 */
  {sizeof(T_BAT_res_uns_plus_ilrr)},       /* BAT_RES_UNS_PLUS_ILRR          = 0x1031 */
  {sizeof(T_BAT_res_uns_percent_rdl)},     /* BAT_RES_UNS_PERCENT_RDL        = 0x1032 */
  {sizeof(T_BAT_res_uns_percent_rdlb)},    /* BAT_RES_UNS_PERCENT_RDLB       = 0x1033 */
  {sizeof(T_BAT_res_uns_percent_sata)},    /* BAT_RES_UNS_PERCENT_SATA       = 0x1034 */
  {sizeof(T_BAT_res_uns_percent_sati)},    /* BAT_RES_UNS_PERCENT_SATI       = 0x1035 */
  {sizeof(T_BAT_res_uns_percent_satn)},    /* BAT_RES_UNS_PERCENT_SATN       = 0x1036 */
  {sizeof(T_BAT_res_uns_percent_simef)},   /* BAT_RES_UNS_PERCENT_SIMEF      = 0x1037 */
  {sizeof(T_BAT_res_uns_percent_simins)},  /* BAT_RES_UNS_PERCENT_SIMINS     = 0x1038 */
  {sizeof(T_BAT_res_uns_percent_simrem)},  /* BAT_RES_UNS_PERCENT_SIMREM     = 0x1039 */
  {sizeof(T_BAT_res_uns_percent_sncnt)},    /* BAT_RES_UNS_PERCENT_SNCNT      = 0x103a */
  {sizeof(T_BAT_res_uns_percent_cniv)}    /* BAT_RES_UNS_PERCENT_CNIV       = 0x103b */
};



GLOBAL T_BAT_client clnt_table[BAT_MAX_TEST_INST_NUM][BAT_MAX_TEST_CLNT_NUM+1] = 
                                          /* NrOfCl, Clnt0, Clnt1, Clnt2, Clnt3 */
                                                {{0,0xffff,0xffff,0xffff,0xffff},  /* instance 0 */
                                                 {0,0xffff,0xffff,0xffff,0xffff}}; /* instance 1 */

#define RESP_LINES     20
#define RESP_LINE_LEN 256
LOCAL  CHAR  at_cmd_resp_param [RESP_LINES][RESP_LINE_LEN];

struct app_bat_globals
{
  T_BAT_plus_cscs_cs  cs;
} app_bat_globs = {BAT_CSCS_CS_IRA};

/*==== Local functions =======================================================*/

/*---------------------------------------------------------------------------
 * Definition of command handler functions
 *---------------------------------------------------------------------------*/
GLOBAL void signal_cb (T_BAT_client client, T_BAT_signal signal)
{
  TRACE_FUNCTION ("signal_cb()");
  TRACE_EVENT_P2("CLIENT HNDL IS: 0x%04x, SIGNAL IS: %d", client, signal);
  return;
}

GLOBAL void signal_cb1 (T_BAT_client client, T_BAT_signal signal)
{
  TRACE_FUNCTION ("signal_cb1()");
  TRACE_EVENT_P2("CLIENT HNDL IS: 0x%04x, SIGNAL IS: %d", client, signal);
  return;
}

GLOBAL void signal_cb2 (T_BAT_client client, T_BAT_signal signal)
{
  TRACE_FUNCTION ("signal_cb2()");
  TRACE_EVENT_P2("CLIENT HNDL IS: 0x%04x, SIGNAL IS: %d", client, signal);
  return;
}

#define BITFIELD_CHECK(x, y)  (x & y)

LOCAL int app_bat_get_size (T_BAT_ctrl_response ctrl_response)
{
  if (BITFIELD_CHECK(ctrl_response, 0x1000)) /* BAT_BIT_12 */
  {
    return (unsolicited_2_size[ctrl_response - 0x1000].size);
  }
  else
  {
    return (response_2_size[ctrl_response].size);
  }
}


LOCAL void response_content (T_BAT_cmd_response *resp)
{ 
  int resp_len = app_bat_get_size(resp->ctrl_response);
  TRACE_EVENT_P2("response_content(): response = 0x%04X, size = %i", resp->ctrl_response, resp_len);
  TRACE_BINDUMP(GDD_DIO_handle, TC_USER4, "BAT RESPONSE PARAMETER", (U8 *)resp->response.ptr_at_ok, resp_len);
}


typedef enum
{
    CLIENT_HANDLE_OK = 0,
    CLIENT_HANDLE_OUT_OF_BOUNDS,
    CLIENT_HANDLE_NOT_VALID
} T_CLIENT_HANDLE_ERROR;

LOCAL T_CLIENT_HANDLE_ERROR check_client_handle (T_BAT_client clnt_hndl)
{
  U8 clnt_hndl_upper = (U8)((clnt_hndl&0xff00)>>8);  /* the instance */
  U8 clnt_hndl_lower = (U8) (clnt_hndl&0x00ff);      /* the client   */

  TRACE_EVENT_P2("check_client_handle(): instance = %d, client = %d", clnt_hndl_upper, clnt_hndl_lower);

  if((clnt_hndl_lower >= BAT_MAX_TEST_CLNT_NUM) OR (clnt_hndl_upper >= BAT_MAX_TEST_INST_NUM))
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return (CLIENT_HANDLE_OUT_OF_BOUNDS);
  }
  if(clnt_table[clnt_hndl_upper][0] EQ 0) /* first element in clnt_table informs about Nr of Clnts */
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return (CLIENT_HANDLE_NOT_VALID);
  }
  
  if(clnt_table[clnt_hndl_upper][clnt_hndl_lower] EQ 0xffff) /* invalid client */
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return (CLIENT_HANDLE_NOT_VALID);
  }

  return (CLIENT_HANDLE_OK);
}

LOCAL char *error_client_handle (T_CLIENT_HANDLE_ERROR err)
{
  TRACE_FUNCTION("error_client_handle()");
  switch (err)
  {
    case (CLIENT_HANDLE_OUT_OF_BOUNDS):
    {
      TRACE_ERROR("Input clnt_hndl out of bounds");
      return ("Input clnt_hndl out of bounds");
    }
    case (CLIENT_HANDLE_NOT_VALID):
    {
      TRACE_ERROR("client handle not valid");
      return ("client handle not valid");
    }
    default:
    {
      TRACE_ERROR("unknown client handle error");
      return ("unknown client handle error");
    }
  }
}


GLOBAL int response_cb (T_BAT_client client, T_BAT_cmd_response *rsp)
{
  TRACE_FUNCTION("response_cb()");
  TRACE_EVENT_P2("CLIENT HNDL IS: 0x%04x, RESPONSE TAG IS: 0x%04x", client, rsp->ctrl_response);
  response_content (rsp);
  
  switch (rsp->ctrl_response)
  {
    case (BAT_RES_SET_PLUS_CMGL):
    {
      T_BAT_res_set_plus_cmgl    *ptr_set_plus_cmgl;  
      ptr_set_plus_cmgl =( void *) (rsp->response.ptr_set_plus_cmgl);   
      TRACE_EVENT("BAT_RES_SET_PLUS_CMGL response");
      TRACE_EVENT_P1("index  = %d", ptr_set_plus_cmgl->sms_index);
      TRACE_EVENT_P1("status = %d", ptr_set_plus_cmgl->stat);
      break;
    }
    case (BAT_RES_SET_PLUS_CPBR):
    {
  T_BAT_res_set_plus_cpbr    *ptr_set_plus_cpbr;  
      ptr_set_plus_cpbr =( void *) (rsp->response.ptr_set_plus_cpbr); 
      TRACE_EVENT("BAT_RES_SET_PLUS_CPBR response");
      memset(at_cmd_resp_param[0], 0x00,  RESP_LINE_LEN) ; 
      memset(at_cmd_resp_param[1], 0x00,  RESP_LINE_LEN) ; 
      memcpy(at_cmd_resp_param[0],  ptr_set_plus_cpbr->number, ptr_set_plus_cpbr->c_number) ;
      memcpy(at_cmd_resp_param[1],  ptr_set_plus_cpbr->text, ptr_set_plus_cpbr->c_text) ;
  
      TRACE_EVENT_P4("+CPBR: %d,\"%s\",%d,\"%s\"", ptr_set_plus_cpbr->index, 
                                                   at_cmd_resp_param[0], 
                                                   ptr_set_plus_cpbr->type,
                                                   at_cmd_resp_param[1]);
     break ;
    }
    case (BAT_RES_SET_PLUS_CPBF):
    {
      T_BAT_res_set_plus_cpbf    *ptr_set_plus_cpbf;  
      ptr_set_plus_cpbf =( void *) (rsp->response.ptr_set_plus_cpbf); 
      TRACE_EVENT("BAT_RES_SET_PLUS_CPBF response");
      memset(at_cmd_resp_param[0], 0x00,  RESP_LINE_LEN) ; 
      memset(at_cmd_resp_param[1], 0x00,  RESP_LINE_LEN) ; 
      memcpy(at_cmd_resp_param[0],  ptr_set_plus_cpbf->number, ptr_set_plus_cpbf->c_number) ;
      memcpy(at_cmd_resp_param[1],  ptr_set_plus_cpbf->text, ptr_set_plus_cpbf->c_text) ;
  
      TRACE_EVENT_P4("+CPBF: %d,\"%s\",%d,\"%s\"", ptr_set_plus_cpbf->index, 
                                                   at_cmd_resp_param[0], 
                                                   ptr_set_plus_cpbf->type,
                                                   at_cmd_resp_param[1]);
     break ;
    }
    case (BAT_RES_QUE_PLUS_CGATT):
    {
  T_BAT_res_que_plus_cgatt    *ptr_que_plus_cgatt;  
      ptr_que_plus_cgatt =( void *) (rsp->response.ptr_que_plus_cgatt); 

      TRACE_EVENT("BAT_RES_QUE_PLUS_CGATT:");

      if (ptr_que_plus_cgatt->state EQ BAT_CGATT_STATE_DETACHED)
        TRACE_EVENT("BAT_RES_QUE_PLUS_CGATT: GPRS DETACHED");
      else
        TRACE_EVENT("BAT_RES_QUE_PLUS_CGATT: GPRS ATTACHED");

      break ;
    }
    case (BAT_RES_QUE_PLUS_CGACT):
    {
  T_BAT_res_que_plus_cgact    *ptr_que_plus_cgact;  
      ptr_que_plus_cgact =( void *) (rsp->response.ptr_que_plus_cgact); 

      if(ptr_que_plus_cgact->state EQ BAT_CGACT_STATE_DEACTIVATED)
        TRACE_EVENT("BAT_RES_QUE_PLUS_CGACT: GPRS DEACTIVATED");
      else
        TRACE_EVENT("BAT_RES_QUE_PLUS_CGACT: GPRS ACTIVATED");
      break ;
    }
    case (BAT_RES_QUE_PLUS_CGDCONT):
    {
  T_BAT_res_que_plus_cgdcont  *ptr_que_plus_cgdcont;  
      ptr_que_plus_cgdcont =( void *) (rsp->response.ptr_que_plus_cgdcont); 

      memset(at_cmd_resp_param[0], 0x00,  RESP_LINE_LEN) ; 
      memset(at_cmd_resp_param[1], 0x00,  RESP_LINE_LEN) ; 
   
      memcpy(at_cmd_resp_param[0],  ptr_que_plus_cgdcont->apn, ptr_que_plus_cgdcont->c_apn) ;
      memcpy(at_cmd_resp_param[1],  ptr_que_plus_cgdcont->pdp_addr, ptr_que_plus_cgdcont->c_pdp_addr) ;
     
      TRACE_EVENT_P1("BAT_RES_QUE_PLUS_CGCONT: APN is %s",  at_cmd_resp_param[0])  ;
      TRACE_EVENT_P1("BAT_RES_QUE_PLUS_CGCONT: PDP Address is %s", at_cmd_resp_param[1]);
      break ;
    }
    case (BAT_RES_SET_PERCENT_CGPCO):
    {
  T_BAT_res_set_percent_cgpco *ptr_que_per_cgpco;
      ptr_que_per_cgpco =( void *) (rsp->response.ptr_set_percent_cgpco); 

      memset(at_cmd_resp_param[0], 0x00,  RESP_LINE_LEN) ; 
      memset(at_cmd_resp_param[1], 0x00,  RESP_LINE_LEN) ; 
   
      memcpy(at_cmd_resp_param[0],  ptr_que_per_cgpco->pco, ptr_que_per_cgpco->c_pco) ;
     
      TRACE_EVENT_P1("BAT_RES_SET_PERCENT_CGPCO: PCO STRING %s",  at_cmd_resp_param[0])  ;
      break ;
    }
    case (BAT_RES_QUE_PLUS_CPOL):
    {
      rsp->response.ptr_que_plus_cpol->oper[rsp->response.ptr_que_plus_cpol->c_oper] = '\0';
      TRACE_EVENT_P3("BAT_RES_QUE_PLUS_CPOL: %d,%d,\"%s\"", 
                      rsp->response.ptr_que_plus_cpol->index,
                      rsp->response.ptr_que_plus_cpol->format,
                      rsp->response.ptr_que_plus_cpol->oper);
      break;
    }
    case (BAT_RES_TST_PLUS_CPOL):
    {
      TRACE_EVENT_P2("BAT_RES_TST_PLUS_CPOL: %d,%d", 
                      rsp->response.ptr_tst_plus_cpol->index1,
                      rsp->response.ptr_tst_plus_cpol->index2);
      break;
    }
    case (BAT_RES_QUE_PLUS_CSCS):
    {
      TRACE_EVENT_P1("BAT_RES_QUE_PLUS_CSCS: %d", 
                      rsp->response.ptr_que_plus_cscs->cs);
      break;
    }  
    case (BAT_RES_SET_PLUS_CCWA):
    {
      TRACE_EVENT_P1("BAT_RES_SET_PLUS_CCWA: %d", 
                      rsp->response.ptr_set_plus_ccwa->status);
      break;
    }
    case (BAT_RES_SET_PLUS_CGMI):
    {
      TRACE_EVENT_P1("BAT_RES_SET_PLUS_CGMI: %s", 
                      rsp->response.ptr_set_plus_cgmi->manufacturer);
      break;
    } 
    case (BAT_RES_SET_PLUS_CGMM):
    {
      TRACE_EVENT_P1("BAT_RES_SET_PLUS_CGMM: %s", 
                      rsp->response.ptr_set_plus_cgmm->model);
      break;
    } 
    case (BAT_RES_SET_PLUS_CGMR):
    {
      TRACE_EVENT_P1("BAT_RES_SET_PLUS_CGMR: %s", 
                      rsp->response.ptr_set_plus_cgmr->revision);
      break;
    } 
    case (BAT_RES_SET_PLUS_CGSN):
    {
      TRACE_EVENT_P1("BAT_RES_SET_PLUS_CGSN: %s", 
                      rsp->response.ptr_set_plus_cgsn->sn);
      break;
    } 
    case (BAT_RES_AT_OK):
    {
      TRACE_EVENT("BAT_RES_AT_OK"); 
      break;
    }
    case (BAT_RES_AT_CONNECT):
    {
      TRACE_EVENT("BAT_RES_AT_CONNECT"); 
      break;
    }
    case (BAT_RES_AT_NO_CARRIER_FINAL):
    {
      TRACE_EVENT("BAT_RES_AT_NO_CARRIER_FINAL"); 
      break;
    }    
    case (BAT_RES_PLUS_CME_ERROR):
    {
      TRACE_EVENT("BAT_RES_PLUS_CME_ERROR"); 
      break;
    }    
    case (BAT_RES_PLUS_CMS_ERROR):
    {
      TRACE_EVENT("BAT_RES_PLUS_CMS_ERROR"); 
      break;
    }

    case (BAT_RES_AT_BUSY):
    {
      TRACE_EVENT("BAT_RES_AT_BUSY"); 
      break;
    }    
    case BAT_RES_UNS_AT_BUSY:
    {
      TRACE_EVENT("BAT_RES_UNS_AT_BUSY");
      break;
    }
    case BAT_RES_AT_CHANNEL_BUSY:
    {
      TRACE_EVENT("BAT_RES_AT_CHANNEL_BUSY");
      break;
    }
    case (BAT_RES_AT_NO_ANSWER):
    {
      TRACE_EVENT("BAT_RES_AT_NO_ANSWER"); 
      break;
    }    
    case BAT_RES_UNS_AT_NO_ANSWER:
    {
      TRACE_EVENT("BAT_RES_UNS_AT_NO_ANSWER");
      break;
    }
    case (BAT_RES_PLUS_EXT_ERROR):
    {
      TRACE_EVENT("BAT_RES_PLUS_EXT_ERROR"); 
      break;
    }
    default:
    {
     TRACE_EVENT("response_cb(): THIS BAT CMD RESPONSE IS STILL UNCOVERED");          
     break ;
    }
  }
  return 0;
}


GLOBAL int response_cb_busy (T_BAT_client client, T_BAT_cmd_response *rsp)
{
  static int i = 0;
  TRACE_FUNCTION ("response_cb_busy()");
  
  TRACE_EVENT_P2("CLIENT HNDL IS: 0x%04x, RESPONSE TAG IS: 0x%04x", client, rsp->ctrl_response);
  TRACE_EVENT_P1("CONTENT OF RESPONSE IS: %s", rsp->response.ptr_at_ok);
 
  /* returns 1 (busy) for the the first call and 0 (ok) for the second call */ 
  if (i EQ 0)
  {
    i++;
    return 1;
  }
 
  return 0;
}

typedef enum
{
 MOTO_CMD_MAID = 0x2001,
 MOTO_CMD_MAPV, 
 MOTO_CMD_MDBAD,
 MOTO_CMD_MDBL, 
 MOTO_CMD_MDBR, 
 MOTO_CMD_MDBW,
 MOTO_CMD_MDFILE, 
 MOTO_CMD_MDIR,
 MOTO_CMD_MFOLDER, 
 MOTO_CMD_MODE, 
 MOTO_CMD_MPBR, 
 MOTO_CMD_MPBW, 
 MOTO_CMD_MPDPM , 
 MOTO_CMD_MR, 
 MOTO_CMD_MSYN, 
 MOTO_CMD_MTDR, 
 MOTO_CMD_MW
} T_MOTO_CMD;


GLOBAL int unsolicited_result_cb (T_BAT_client client, T_BAT_cmd_response *rsp)
{
  TRACE_FUNCTION ("unsolicited_result_cb()");

  TRACE_EVENT_P2("CLIENT HNDL IS: 0x%04x, RESPONSE TAG IS: 0x%04x", client, rsp->ctrl_response);

  if (rsp->ctrl_response & 0x2000)
  {
    rsp->response.ptr_custom->buf[rsp->response.ptr_custom->c_buf] = '\0';
    TRACE_BINDUMP(GDD_DIO_handle, TC_USER4, "USER DATA", (U8 *)rsp->response.ptr_custom->buf, 
                                                           rsp->response.ptr_custom->c_buf);
    
    TRACE_EVENT_P2("unsolicited_result_cb(): custom: len = %d %s", 
                    rsp->response.ptr_custom->c_buf, rsp->response.ptr_custom->buf);
    return 0;
  }
  else
  {
    response_content (rsp);
  }

  switch (rsp->ctrl_response)
  {
    case (BAT_RES_UNS_PERCENT_CPI):
    {
      T_BAT_res_uns_percent_cpi *cpi = rsp->response.ptr_res_percent_cpi;

      switch(cpi->msgtype)
      {
        case(BAT_P_CPI_TYPE_SETUP):
        {
          TRACE_EVENT("BAT_P_CPI_TYPE_SETUP");
          break;
        }
        case(BAT_P_CPI_TYPE_DISC):
        {
          TRACE_EVENT("BAT_P_CPI_TYPE_DISC");
          break;
        }        
        case(BAT_P_CPI_TYPE_ALERTING):
        {
          TRACE_EVENT("BAT_P_CPI_TYPE_ALERTING");
          break;
        }        
        case(BAT_P_CPI_TYPE_PROC):
        {
          TRACE_EVENT("BAT_P_CPI_TYPE_PROC");
          break;
        }        
        case(BAT_P_CPI_TYPE_SYNC):
        {
          TRACE_EVENT("BAT_P_CPI_TYPE_SYNC");
          break;
        }
        case(BAT_P_CPI_TYPE_PROGRESS):
        {
          TRACE_EVENT("BAT_P_CPI_TYPE_PROGRESS");
          break;
        }
        case(BAT_P_CPI_TYPE_CONNECT):
        {
          TRACE_EVENT("BAT_P_CPI_TYPE_CONNECT");
          break;
        }
        case(BAT_P_CPI_TYPE_RELEASE):
        {
          TRACE_EVENT("BAT_P_CPI_TYPE_RELEASE");
          break;
        }
        case(BAT_P_CPI_TYPE_REJECT):
        {
          TRACE_EVENT("BAT_P_CPI_TYPE_REJECT");
          break;
        }
      }
      break;
    }
    default:
    {
     TRACE_EVENT("unsolicited_result_cb(): THIS BAT CMD RESPONSE IS STILL UNCOVERED");          
     break ;
    }
  }

  return 0;
}
GLOBAL void instance_signal_cb (T_BAT_signal result)
{
  TRACE_FUNCTION ("instance_signal_cb()");
  TRACE_EVENT_P1("SIGNAL IS: %d", result);
  return;
}


/*
+-------------------------------------------------------------------+
| PROJECT :                       MODULE  : app_bat                 |
|                                 ROUTINE : app_bat_send_result_prim|
+-------------------------------------------------------------------+

  PURPOSE : Provides the output over APP.
           
*/
GLOBAL void app_bat_send_result_prim (U8 result, U32 add_info)
{
#ifndef _TARGET_
 /* send message to the test interface*/
  PALLOC (app_bat_test_result, APP_BAT_TEST_RSLT);
  
  TRACE_FUNCTION_P2("app_bat_send_result_prim(): result is: %d, add_info is: %d", result, add_info);

  app_bat_test_result->test_rslt = result;
  app_bat_test_result->additional_info = add_info;
  PSEND (hCommMMI, app_bat_test_result);
#endif /* _TARGET_ */
  return;
}

GLOBAL char *app_bat_init (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  void *mem = NULL;
  int inst_num = get_item(param1, BAT_MAX_TEST_INST_NUM, TRUE);
 
  TRACE_FUNCTION("app_bat_init()");
  MALLOC(mem, BAT_INSTANCE_HEADER_SIZE*inst_num);  
  result = bat_init(mem, (U8)inst_num);
  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return "Call to bat_init() failed";
  }

  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_deinit (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                             char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = bat_deinit();
  TRACE_FUNCTION("app_bat_deinit()");

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_deinit() failed");
  }
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_new (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                          char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  void *mem = NULL;
  T_BAT_instance inst_hndl = BAT_INVALID_INSTANCE_HANDLE;
  int i = 0;
  int clnt_num = get_item(param1, BAT_MAX_TEST_CLNT_NUM, TRUE);

  TRACE_FUNCTION("app_bat_new()");

    /* Find free slot for connection handle */
  for (i = 0; i < BAT_MAX_TEST_INST_NUM && clnt_table[i][0] != 0; i++)
  {  

  }

  if(clnt_num > BAT_MAX_TEST_CLNT_NUM OR i > BAT_MAX_TEST_INST_NUM)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Max instance num too big");
  }


  MALLOC(mem, (clnt_num*BAT_CLIENT_SIZE + BAT_INSTANCE_SIZE));

  app_set_config ();

  result = bat_new (&inst_hndl, mem, (U8)clnt_num, BAT_APP_CONFIG, instance_signal_cb);

  TRACE_EVENT_P1 ("inst_hndl is %d", inst_hndl);

  if(result != BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return("Call to bat_new() failed");
  }

  clnt_table[inst_hndl][0]++;

  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_delete (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                            char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_instance inst_hndl = BAT_BROADCAST_CHANNEL; /* connection number -index into local static connection table */

  TRACE_FUNCTION("app_bat_delete()");

  inst_hndl = get_item(param1, 0, FALSE);
  if(inst_hndl >= BAT_MAX_TEST_INST_NUM)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Connection number out of bounds");
  }
  result = bat_delete (inst_hndl);
  if(result != BAT_OK )
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to gdd_close failed");
  }

  clnt_table[inst_hndl][0] = 0;
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_uns_open (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                            char *param2, char *param3, core_func_t core_func)
{
  T_BAT_client client;
  TRACE_FUNCTION("app_bat_uns_open()");
  if (bat_uns_open(0, &client, unsolicited_result_cb))
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_uns_open() failed");
  }
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_open (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                            char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_INVALID_CLIENT_HANDLE; /* connection number -index into local static connection table */
  T_BAT_instance inst_hndl = BAT_BROADCAST_CHANNEL;
  U8 clnt_hndl_upper = 0;
  U8 clnt_hndl_lower = 0;

  TRACE_FUNCTION("app_bat_open()");

  inst_hndl = get_item(param1, 0, TRUE);
  if (inst_hndl >= BAT_MAX_TEST_INST_NUM)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("input inst_hndl is wrong");
  }
  
  if ( clnt_table[inst_hndl][0] EQ 0)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return("input inst_hndl is wrong, instance is not opened yet");
  }
  
  result = bat_open (inst_hndl, &clnt_hndl, response_cb, signal_cb);
  if(result != BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return("Call to bat_open() failed");
  }

  clnt_hndl_upper = (U8)((0xff00&clnt_hndl)>>8);
  clnt_hndl_lower = (U8) (0x00ff&clnt_hndl);
  TRACE_EVENT_P2("The returned clnt_hndl is 0x%02X%02X", clnt_hndl_upper, clnt_hndl_lower);

  if(clnt_hndl_upper NEQ inst_hndl)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return("clnt_hndl is wrong");
  }

  clnt_table[inst_hndl][clnt_hndl_lower+1] = clnt_hndl;
  clnt_table[inst_hndl][0]++; 

  TRACE_EVENT_P3("app_bat_open(): clnt_table[%d][%d] = 0x%04X", inst_hndl, clnt_hndl_lower+1, clnt_hndl);

  app_bat_uns_open (cmd_entry_ptr, param1, param2, param3, core_func);
  app_bat_send_result_prim(0, NO_ADD_VAL);

  return 0;
}

GLOBAL char *app_bat_close (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                            char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_INVALID_CLIENT_HANDLE; /* connection number -index into local static connection table */
  U8 clnt_hndl_upper;
  U8 clnt_hndl_lower;
  T_CLIENT_HANDLE_ERROR err;

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  clnt_hndl_upper = (U8)((clnt_hndl&0xff00)>>8);
  clnt_hndl_lower = (U8) (clnt_hndl&0x00ff);

  result = bat_close (clnt_hndl);
  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_close() failed");
  }

  clnt_table[clnt_hndl_upper][clnt_hndl_lower+1] = 0xffff;
  clnt_table[clnt_hndl_upper][0]--;
    
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

/*
 *  sending of BAT commands
 */

GLOBAL char *app_bat_send_q_clcc(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                  char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_no_parameter    my_que_plus_clcc; 
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_q_clcc()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  my_bat_cmd_send.ctrl_params = BAT_CMD_QUE_PLUS_CLCC;
  my_bat_cmd_send.params.ptr_que_plus_clcc = &my_que_plus_clcc ;
  my_que_plus_clcc.bat_dummy = 0xFF;
  
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if (result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_q_clcc() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_send_q_cscs(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                  char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_no_parameter    my_que_plus_cscs; 
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_q_cscs()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  my_bat_cmd_send.ctrl_params = BAT_CMD_QUE_PLUS_CSCS ;
  my_bat_cmd_send.params.ptr_que_plus_cscs = &my_que_plus_cscs ;
  my_que_plus_cscs.bat_dummy = 0xFF;
  
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if (result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_q_cscs() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_send_csms(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                               char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_csms my_set_plus_csms;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_csms()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_set_plus_csms, sizeof(my_set_plus_csms), FALSE);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CSMS;
  
  my_bat_cmd_send.params.ptr_set_plus_csms = &my_set_plus_csms;

  /* resolve the service requested */
  my_set_plus_csms.service = (T_BAT_plus_csms_service)atoi(param2);;

  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_csms() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_send_ccwa (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result    = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_ccwa my_set_plus_ccwa;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_ccwa()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_set_plus_ccwa, sizeof(my_set_plus_ccwa), FALSE);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CCWA;
  my_bat_cmd_send.params.ptr_set_plus_ccwa = &my_set_plus_ccwa;
  my_set_plus_ccwa.mode   = (T_BAT_plus_ccwa_mode) atoi(param2);
  my_set_plus_ccwa.bearer_class = BAT_CCWA_CLASS_NOT_PRESENT;
  
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_ccwa() failed");
  }  
  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_send_cgsms(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                               char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cgsms my_set_plus_cgsms;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cgsms()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_set_plus_cgsms, sizeof(my_set_plus_cgsms), FALSE);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CGSMS;
  
  my_bat_cmd_send.params.ptr_set_plus_cgsms = &my_set_plus_cgsms;

  /* resolve the service requested */
  my_set_plus_cgsms.service = (T_BAT_plus_cgsms_service) atoi(param2);

  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cgsms() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_send_cscs(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                               char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cscs    my_set_plus_cscs; 
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cscs()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CSCS ;
  my_bat_cmd_send.params.ptr_set_plus_cscs = &my_set_plus_cscs;
  
  /* on ATI is also supported some Scandinavian smoerebroed */
  if (param2 NEQ NULL)
  {     
    if      (strcmp(param2, "IRA") EQ 0)  my_set_plus_cscs.cs = BAT_CSCS_CS_IRA;
    else if (strcmp(param2, "GSM") EQ 0)  my_set_plus_cscs.cs = BAT_CSCS_CS_GSM;
    else if (strcmp(param2, "HEX") EQ 0)  my_set_plus_cscs.cs = BAT_CSCS_CS_HEX;
    else if (strcmp(param2, "UCS2") EQ 0) my_set_plus_cscs.cs = BAT_CSCS_CS_UCS2;
    else
    {
      TRACE_EVENT("app_bat_send_cscs(): support only UCS2, HEX, GSM, IRA (default)");
      my_set_plus_cscs.cs = BAT_CSCS_CS_IRA;
    }
  }
  else
  {
    my_set_plus_cscs.cs = BAT_CSCS_CS_IRA;  /*default = International Reference Alphabet */
  }

  app_bat_globs.cs = my_set_plus_cscs.cs;
  
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if (result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cscs() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}


GLOBAL char *app_bat_send_cpbr (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cpbr my_set_plus_cpbr;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cpbr()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }
  
  memset(&my_set_plus_cpbr, sizeof(my_set_plus_cpbr), 0);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CPBR;
  my_bat_cmd_send.params.ptr_set_plus_cpbr = &my_set_plus_cpbr;

  /* without given any parameters, set to the same as AT+CPBR=1,1 */
  if (param2 EQ NULL)
  {     
    my_set_plus_cpbr.index1 = 1;
  }
  else
  {
    my_set_plus_cpbr.index1 = atoi(param2);
  }
  if (param3 EQ NULL)
  {     
    my_set_plus_cpbr.index2 =  my_set_plus_cpbr.index1;
  }
  else
  {
    my_set_plus_cpbr.index2 = atoi(param3) ;
  }
  TRACE_EVENT_P2("app_bat_send_cpbr(): send binary AT+CPBR=%d,%d", my_set_plus_cpbr.index1,  my_set_plus_cpbr.index2);

  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cfun() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}


GLOBAL char *app_bat_send_cpbw (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cpbw  my_set_plus_cpbw;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cpbw()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_set_plus_cpbw, sizeof(my_set_plus_cpbw), 0);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CPBW;
  my_bat_cmd_send.params.ptr_set_plus_cpbw = &my_set_plus_cpbw;

  if( param2 EQ NULL)
  {
    my_set_plus_cpbw.index = 1;
  }
  else
  {
    my_set_plus_cpbw.index = atoi(param2);
  }

  /*
   * AT+CPBW=index,"number",type,"name"
   * with test application we can only get 3 parameters,
   * where normally param1 --> clnt_hndl
   *                param2 --> index
   *                param3 --> number
   *
   * But we are more interrested in changing the phone book name
   * due to different character sets.
   * That's why we assign param3 to the name and hard code the number
   */

  if(param3==NULL)
  {
    strcpy((char *)(my_set_plus_cpbw.text), "Doe, John");
    my_set_plus_cpbw.c_text = strlen("Doe, John");
  }
  else
  {
    strcpy((char *)my_set_plus_cpbw.text, param3);
    my_set_plus_cpbw.c_text = strlen(param3);
  }
  my_set_plus_cpbw.v_text = TRUE; 

  my_set_plus_cpbw.v_number = TRUE; 
  strcpy((char *)(my_set_plus_cpbw.number), "12345678" ); 
  my_set_plus_cpbw.c_number = strlen((const char *)(my_set_plus_cpbw.number)); 

  TRACE_EVENT_P2("app_bat_send_cpbw(): %s, %s", my_set_plus_cpbw.number, my_set_plus_cpbw.text);

  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if (result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cfun() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}


GLOBAL char *app_bat_send_cpbf (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                     char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cpbf  my_set_plus_cpbf;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cpbf()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_set_plus_cpbf, sizeof(my_set_plus_cpbf), 0);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CPBF;
  my_bat_cmd_send.params.ptr_set_plus_cpbf = &my_set_plus_cpbf;

  if( param2 EQ NULL)
  {
    my_set_plus_cpbf.c_findtext = sizeof("Hello")-1;
    strcpy((char *)my_set_plus_cpbf.findtext,"Hello");   
  }
  else
  {
    my_set_plus_cpbf.c_findtext = strlen(param2);
    strcpy((char *)my_set_plus_cpbf.findtext,param2);
  }

  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if (result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cfun() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_send_cgmi(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result    = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_send     lbatSend;
  T_BAT_no_parameter lbatDummy;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cgmi()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  lbatSend.ctrl_params              = BAT_CMD_SET_PLUS_CGMI;
  lbatDummy.bat_dummy               = 0xFF;
  lbatSend.params.ptr_set_plus_cgmi = &lbatDummy;

  result = bat_send (clnt_hndl, &lbatSend);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to app_bat_send_cgmi() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_send_cgmm(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result    = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_send     lbatSend;
  T_BAT_no_parameter lbatDummy;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cgmm()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  lbatSend.ctrl_params              = BAT_CMD_SET_PLUS_CGMM;
  lbatDummy.bat_dummy               = 0xFF;
  lbatSend.params.ptr_set_plus_cgmm = &lbatDummy;

  result = bat_send (clnt_hndl, &lbatSend);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to app_bat_send_cgmm() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_send_cgmr(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result    = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_send     lbatSend;
  T_BAT_no_parameter lbatDummy;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cgmr()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  lbatSend.ctrl_params              = BAT_CMD_SET_PLUS_CGMR;
  lbatDummy.bat_dummy               = 0xFF;
  lbatSend.params.ptr_set_plus_cgmr = &lbatDummy;

  result = bat_send (clnt_hndl, &lbatSend);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to app_bat_send_cgmr() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_send_cgsn(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result    = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_send     lbatSend;
  T_BAT_no_parameter lbatDummy;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cgsn()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  lbatSend.ctrl_params              = BAT_CMD_SET_PLUS_CGSN;
  lbatDummy.bat_dummy               = 0xFF;
  lbatSend.params.ptr_set_plus_cgsn = &lbatDummy;

  result = bat_send (clnt_hndl, &lbatSend);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return("Call to app_bat_send_cgsn() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

/*
 * add some gprs command for test TCPIP
 */
GLOBAL char *app_bat_send_q_cgatt(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_no_parameter    my_que_plus_cgatt; 
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_q_cgactt()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  my_bat_cmd_send.ctrl_params = BAT_CMD_QUE_PLUS_CGATT ;
  my_bat_cmd_send.params.ptr_que_plus_cgatt = &my_que_plus_cgatt ;
  /* full functionality mode*/
  /* Reset the MT before changing functionality level */
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cfun() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}



GLOBAL char *app_bat_send_q_cgact(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                  char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_no_parameter    my_que_plus_cgact; 
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_q_cgact()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  my_bat_cmd_send.ctrl_params = BAT_CMD_QUE_PLUS_CGACT ;
  my_bat_cmd_send.params.ptr_que_plus_cgact = &my_que_plus_cgact ;
  /* full functionality mode*/
  /* Reset the MT before changing functionality level */
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cfun() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}



GLOBAL char *app_bat_send_cgatt(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                char *param2, char *param3, core_func_t core_func) 
{

  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cgatt  my_set_plus_cgatt;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cgatt()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_set_plus_cgatt , sizeof(my_set_plus_cgatt),  0 );
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CGATT ;
  my_bat_cmd_send.params.ptr_set_plus_cgatt = &my_set_plus_cgatt ;
  /* full functionality mode*/
  if(param2 EQ NULL)
    my_set_plus_cgatt.state = BAT_CGATT_STATE_ATTACHED;
  else
    my_set_plus_cgatt.state = (T_BAT_plus_cgatt_state) atoi(param2) ;

  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cgatt() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}


GLOBAL char *app_bat_send_cgact(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func) 
{

  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cgact  my_set_plus_cgact;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cgact()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_set_plus_cgact , sizeof(my_set_plus_cgact),  0 );
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CGACT ;
  my_bat_cmd_send.params.ptr_set_plus_cgact = &my_set_plus_cgact ;
  /* full functionality mode*/
  if(param2==NULL)
    my_set_plus_cgact.state =(T_BAT_plus_cgact_state) 1 ;
  else
    my_set_plus_cgact.state=(T_BAT_plus_cgact_state) atoi(param2) ;

  /* set Activate which pdp context !!!  */
  my_set_plus_cgact.c_cid = 1 ; 
  my_set_plus_cgact.cid[0] = (T_BAT_pdp_cid) 1 ;
   
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cgact() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}



GLOBAL char *app_bat_send_cgpco(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                char *param2, char *param3, core_func_t core_func) 
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_percent_cgpco my_set_per_cgpco ;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;
  CHAR User[10] ;
  CHAR Pass[10] ;
  USHORT  pco_len = 100;
  UBYTE   pco_array[100];

  TRACE_FUNCTION("app_bat_send_cgpco()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_set_per_cgpco , sizeof(my_set_per_cgpco),  0 );
  memset(pco_array , 0x00 , sizeof(pco_array));
  memset(User , 0x00 , sizeof(User)) ;
  memset(Pass , 0x00 , sizeof(Pass)) ;
  
  
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PERCENT_CGPCO ;
  my_bat_cmd_send.params.ptr_set_percent_cgpco= &my_set_per_cgpco ;
  /* full functionality mode*/
  if(param2!=NULL)
    strcpy(User ,param2);
  else
    strcpy(User ,"WAP");

  if(param3!=NULL)
    strcpy(Pass ,param3);
  else
    strcpy(Pass ,"WAP");

  my_set_per_cgpco.cid = (T_BAT_pdp_cid)1 ;
  my_set_per_cgpco.format = (T_BAT_percent_cgpco_format) 1 ;  /* ASCII mode */ 
  my_set_per_cgpco.mode  = (T_BAT_percent_cgpco_mode) 0  ;  /* Set  parameter command */
  
  my_set_per_cgpco.v_pco = 1  ;
  my_set_per_cgpco.c_pco = 0  ; 

#if 0
 ret = utl_create_pco (pco_array, &pco_len,
                        ACI_PCO_CONTENTMASK_AUTH | 
                        ACI_PCO_CONTENTMASK_DNS1 |
                        ACI_PCO_CONTENTMASK_DNS2,
                        ACI_PCO_CONFIG_PROT_PPP,
                        ACI_PCO_AUTH_PROT_PAP, (UBYTE*)&User, (UBYTE*)&Pass, dns_adr1, dns_adr2);

 ret = utl_create_pco (pco_array, &pco_len,
                        0x00000001 | 
                        0x00000002 |
                        0x00000004,
                        0,
                        0xc023, (UBYTE*)&User, (UBYTE*)&Pass, dns_adr1, dns_adr2);

#endif  

  strcpy((char *)pco_array, "PAP,WAP,WAP,10.2.3.4,10.0.0.10" ) ;
 
  TRACE_EVENT_P2("=======Input PCO common string is ===%s==== len is %d", pco_array, pco_len) ;

  memcpy(&my_set_per_cgpco.pco, &pco_array, pco_len ) ;
  my_set_per_cgpco.c_pco = pco_len ; 

  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cgpco() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_send_q_cgpco(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                  char *param2, char *param3, core_func_t core_func) 
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_percent_cgpco my_set_per_cgpco ;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;
  USHORT  pco_len = 100;
  UBYTE   pco_array[100];


  TRACE_FUNCTION("app_bat_send_q_cgpco()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_set_per_cgpco , sizeof(my_set_per_cgpco),  0 );
  memset(pco_array , 0x00 , sizeof(pco_array));
  
  
  my_bat_cmd_send.ctrl_params = (T_BAT_ctrl_params) BAT_CMD_SET_PERCENT_CGPCO ;
  my_bat_cmd_send.params.ptr_set_percent_cgpco= &my_set_per_cgpco ;
  /* full functionality mode*/
  
  my_set_per_cgpco.cid = (T_BAT_pdp_cid) 1 ;
  my_set_per_cgpco.format = (T_BAT_percent_cgpco_format) 1 ;  /* ASCII mode */
  my_set_per_cgpco.mode  = (T_BAT_percent_cgpco_mode) 1  ;  /* Query  parameter command */
  
  my_set_per_cgpco.v_pco = 1  ;
  my_set_per_cgpco.c_pco = 0  ; 

 
  TRACE_EVENT_P2("pco_array = %s, len = %d", pco_array, pco_len) ;

  memcpy(&my_set_per_cgpco.pco, &pco_array, pco_len ) ;
  my_set_per_cgpco.c_pco = pco_len ; 
 
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cgpco() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}


GLOBAL char *app_bat_send_cgdcont(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func) 
{

  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cgdcont  my_set_plus_cgdcont ;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cgdcont()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_set_plus_cgdcont , sizeof(my_set_plus_cgdcont),  0 );
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CGDCONT ;
  my_bat_cmd_send.params.ptr_set_plus_cgdcont = &my_set_plus_cgdcont ;
  /* full functionality mode*/
   if(param2!=NULL )
   {
     memcpy( my_set_plus_cgdcont.apn  , param2, strlen(param2)  );   
          my_set_plus_cgdcont.c_apn = strlen(param2)  ;
          my_set_plus_cgdcont.v_apn = 1  ;
          my_set_plus_cgdcont.pdp_type = BAT_PDP_TYPE_IPV4 ;  //PDP_T_IP ;

     my_set_plus_cgdcont.cid =(T_BAT_pdp_cid) 1 ;
          my_set_plus_cgdcont.v_pdp_addr= 1  ;
          my_set_plus_cgdcont.c_pdp_addr= 16  ;
          memset(my_set_plus_cgdcont.pdp_addr , '\0' , 16 ) ; 
     my_set_plus_cgdcont.d_comp = (T_BAT_plus_cgdcont_d_comp) -1 ;
     my_set_plus_cgdcont.h_comp = (T_BAT_plus_cgdcont_h_comp) -1 ;
      
      
   }
   else
   {
     strcpy( (char *)my_set_plus_cgdcont.apn  , "CMWAP" );   
          my_set_plus_cgdcont.c_apn = strlen("CMWAP")  ;
      
          my_set_plus_cgdcont.v_apn = 1  ;
          my_set_plus_cgdcont.pdp_type = BAT_PDP_TYPE_IPV4; //PDP_T_IP ;
          my_set_plus_cgdcont.cid = (T_BAT_pdp_cid) 1 ;
      
          my_set_plus_cgdcont.v_pdp_addr= 1  ;
          my_set_plus_cgdcont.c_pdp_addr= 16  ;
          memset(my_set_plus_cgdcont.pdp_addr ,'\0' , 16 ) ;  
     my_set_plus_cgdcont.d_comp = (T_BAT_plus_cgdcont_d_comp) -1 ;
     my_set_plus_cgdcont.h_comp = (T_BAT_plus_cgdcont_h_comp) -1 ;
   }

  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cgdcont() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}


GLOBAL char *app_bat_send_cfun (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cfun my_set_plus_cfun;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cfun()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_set_plus_cfun, sizeof(my_set_plus_cfun), FALSE);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CFUN;
  my_bat_cmd_send.params.ptr_set_plus_cfun = &my_set_plus_cfun;
  /* full functionality mode*/
  my_set_plus_cfun.fun = BAT_CFUN_FUN_FULL;
  /* Reset the MT before changing functionality level */
  my_set_plus_cfun.rst = BAT_CFUN_RST_NOT_PRESENT; 
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cfun() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

#define MAX_SM_LEN 176

typedef struct            /* data buffer for Short Messages */
{
  UBYTE data[MAX_SM_LEN];
  UBYTE len;
} T_ACI_SM_DATA;

LOCAL char *HexToGsm(char *cl, U8 *value)
{
  int i;
  U8 character;

  *value = 0;

  /*
   * for both nibbles
   */
  for (i=0;i<2;i++)
  {
    /*
     * shift the result of the last loop
     */
    *value = (*value) << 4;

    /*
     * Isolate next nibble in ASCII
     */
    character = toupper(*cl++);

    /*
     * convert Nibble character to value
     */
    switch (character)
    {
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
        *value = *value + (character - 'A' + 10);
        break;
      default:
        /*
         * 0-9
         */
        *value = *value + (character - '0');
        break;
    }
  }

  /*
   * return pointer to the rest of PDU
   */
  return cl;
}

GLOBAL char *app_bat_send_cmgs (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result    = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cmgs my_set_plus_cmgs;
  T_BAT_cmd_send my_bat_cmd_send;
  char *cl, cl_str[] = "079194710600403401000B811027513356F8000008C22015442FCFE9";  
  T_ACI_SM_DATA pdu;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cmgs()");
  
  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  pdu.len = 0;
  cl = cl_str;

  {
    U8 i = 0;
    U8 sca_len = 0;
    U8 offset  = 0;
    U8 pdu_message_octets_length = 0;

    /*
     * Extract the length (excluding SCA octets) outto be parsed from
     * the input pointer cl
     */

    SHORT length_of_pdu_message = strlen(cl)/2 - 8;
    TRACE_EVENT_P1("length_of_pdu_message: %d", length_of_pdu_message);

    HexToGsm (cl, &sca_len);
    
    /*
    calculation of the real length of PDU string
    The first octet in the input is an indicator of the length of the SMSC information supplied.
    And this Octet and the SMSC content do not count in the real length of PDU string.
    so here, the length of the input needs to minus 2 + sca_len*2 and then divide 2.
    */
    pdu_message_octets_length = (strlen(cl)-2-sca_len*2)/2;
    TRACE_EVENT_P1("pdu_message_octets_length: %d", pdu_message_octets_length);

    pdu.len = length_of_pdu_message + sca_len + 1;
    TRACE_EVENT_P1("pdu.len: %d", pdu.len);

    for (i = offset; i < pdu.len AND *cl NEQ '\0' AND i < MAX_SM_LEN; i++)
    {
      cl = HexToGsm(cl, &pdu.data[i]);
    }
  }
  memset(&my_set_plus_cmgs, sizeof(my_set_plus_cmgs), 0);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CMGS;
  my_bat_cmd_send.params.ptr_set_plus_cmgs = &my_set_plus_cmgs;
  memcpy(my_set_plus_cmgs.pdu, pdu.data, pdu.len);
  my_set_plus_cmgs.c_pdu = pdu.len;                               /* total length */
  my_set_plus_cmgs.length = my_set_plus_cmgs.c_pdu;   /* length without SCA */

  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cmgs() failed");
  } 
  app_bat_send_result_prim(0, NO_ADD_VAL); 
  return 0;
}


GLOBAL char *app_bat_send_cmgl (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result    = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cmgl my_set_plus_cmgl;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cmgs()");
  
  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  if( param2 EQ NULL)
  {
    my_set_plus_cmgl.stat = BAT_CMGL_STAT_ALL;
  }
  else
  {
    if (strcmp(param2, "ALL") EQ 0)
    {
      my_set_plus_cmgl.stat = BAT_CMGL_STAT_ALL;
    }
    if (strcmp(param2, "SENT") EQ 0)
    {
      my_set_plus_cmgl.stat = BAT_CMGL_STAT_STO_SEND;
    }
    if (strcmp(param2, "UNSENT") EQ 0)
    {
      my_set_plus_cmgl.stat = BAT_CMGL_STAT_STO_UNSENT;
    }
    if (strcmp(param2, "READ") EQ 0)
    {
      my_set_plus_cmgl.stat = BAT_CMGL_STAT_REC_READ;
    }
    if (strcmp(param2, "UNREAD") EQ 0)
    {
      my_set_plus_cmgl.stat = BAT_CMGL_STAT_REC_UNREAD;
    }
  }
  
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CMGL;
  my_bat_cmd_send.params.ptr_set_plus_cmgl = &my_set_plus_cmgl;

  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cmgl() failed");
  } 
  app_bat_send_result_prim(0, NO_ADD_VAL); 
  return 0;
  
}


GLOBAL char *app_bat_send_cops (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result    = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cops my_set_plus_cops;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_cops()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_set_plus_cops, sizeof(my_set_plus_cops), FALSE);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_COPS;
  my_bat_cmd_send.params.ptr_set_plus_cops = &my_set_plus_cops;
  my_set_plus_cops.mode   = BAT_COPS_MODE_AUTO;
  my_set_plus_cops.format = BAT_COPS_FORMAT_NOT_PRESENT;
  my_set_plus_cops.v_oper = FALSE; 
  my_set_plus_cops.c_oper = 0; 
  
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cops() failed");
  }  
  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_send_cpin (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cpin my_cmd_plus_cpin;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;
  char number[] = "1236";

  clnt_hndl = get_item(param1, 0, TRUE);

  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_cmd_plus_cpin, sizeof(my_cmd_plus_cpin), FALSE);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CPIN;
  my_bat_cmd_send.params.ptr_set_plus_cpin = &my_cmd_plus_cpin;
   
  if (param2)
  {
    strcpy((char *)&(my_cmd_plus_cpin.pin), (const char *)param2);
    my_cmd_plus_cpin.c_pin = strlen(param2);
  }
  else
  {
    strcpy((char *)&(my_cmd_plus_cpin.pin), (const char *)number);
    my_cmd_plus_cpin.c_pin = strlen(number);
  }
  
  if (param3)
  {
    strcpy((char *)&(my_cmd_plus_cpin.newpin), (const char *)param3);
    my_cmd_plus_cpin.c_newpin = strlen(param3);
    my_cmd_plus_cpin.v_newpin = 1;   
  }

  TRACE_EVENT_P1("app_bat_send_cpin(): PIN = %s", my_cmd_plus_cpin.pin);
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cpin() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_send_cnmi (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cnmi my_cmd_plus_cnmi;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;
  SHORT mt   = BAT_CNMI_MT_NOT_PRESENT;
  SHORT bm   = BAT_CNMI_BM_NOT_PRESENT;
  SHORT ds   = BAT_CNMI_DS_NOT_PRESENT;

  clnt_hndl = get_item(param1, 0, TRUE);

  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_cmd_plus_cnmi, sizeof(my_cmd_plus_cnmi), FALSE);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CNMI;
  my_bat_cmd_send.params.ptr_set_plus_cnmi = &my_cmd_plus_cnmi;

  /*
  *   +CNMI has up to three parameters. These may as well all be
  *   delivered in "param2", separated by commas. We can extract them
  *   using the ATI function "parse()". The extraction is done
  *   not to elements of the BAT message directly, but via intermediate local
  *   variables. This is to avoid any issues with variable sizes.
  */
  TRACE_EVENT_P1("app_bat_send_cnmi(): param2 = :%s:", param2);
  if (!parse(param2,"ddd", &mt, &bm, &ds))
  {
    app_bat_send_result_prim(1,NO_ADD_VAL);
    return("Failed to parse +CNMI"); 
  }

  /*
  *   Now copy the data into the BAT message structure.
  */
  my_cmd_plus_cnmi.mt = (T_BAT_VAL_plus_cnmi_mt)mt;
  my_cmd_plus_cnmi.bm = (T_BAT_VAL_plus_cnmi_bm)bm;
  my_cmd_plus_cnmi.ds = (T_BAT_VAL_plus_cnmi_ds)ds;

  TRACE_EVENT_P3("app_bat_send_cnmi(): mt = %d bm = %d ds = %d", my_cmd_plus_cnmi.mt, 
		                             my_cmd_plus_cnmi.bm, my_cmd_plus_cnmi.ds);
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cnmi() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}


GLOBAL char *app_bat_send_atd (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_at_d my_cmd_atd;
  T_BAT_cmd_send my_bat_cmd_send;
  char number[] = "03039831270;";
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_atd()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  memset(&my_cmd_atd, sizeof(my_cmd_atd), FALSE);
  my_bat_cmd_send.ctrl_params = BAT_CMD_AT_D;
  my_bat_cmd_send.params.ptr_at_d = &my_cmd_atd;
   
  if (param2)
  {
    strcpy((char *)&(my_cmd_atd.dial_string), (const char *)param2);
    my_cmd_atd.c_dial_string = strlen(param2);
  }
  else
  {
    strcpy((char *)&(my_cmd_atd.dial_string), (const char *)number);
    my_cmd_atd.c_dial_string = strlen(number);
  }

  TRACE_EVENT_P1("app_bat_send_atd(): call %s", my_cmd_atd.dial_string);
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_atd() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}


GLOBAL char *app_bat_send_ata (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_send my_bat_cmd_send;
  T_BAT_no_parameter dummy;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_ata()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  dummy.bat_dummy = 0xFF;
  my_bat_cmd_send.ctrl_params = BAT_CMD_AT_A;
  my_bat_cmd_send.params.ptr_at_a = &dummy;
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_ata() failed");
  } 
  app_bat_send_result_prim(0, NO_ADD_VAL); 
  return 0;
}

GLOBAL char *app_bat_send_per_als (app_cmd_entry_t *cmd_entry_ptr, char *param1,
                           char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_send my_bat_cmd_send;
  T_BAT_cmd_set_percent_als my_set_per_als;
  T_BAT_no_parameter my_quer_per_als;
  int clnt_com = get_item(param2, 0, TRUE);
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_per_als()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }
  
  if(clnt_com EQ BAT_CMD_SET_PERCENT_ALS) /* AT%ALS = 0 or 1 */
  {
    memset(&my_set_per_als, sizeof(my_set_per_als), FALSE);
    my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PERCENT_ALS;
    my_bat_cmd_send.params.ptr_set_percent_als = &my_set_per_als;
    my_set_per_als.mode = BAT_P_ALS_MOD_SPEECH;
  }
  else /* AT%ALS? */
  {
    my_bat_cmd_send.ctrl_params = BAT_CMD_QUE_PERCENT_ALS;
    my_bat_cmd_send.params.ptr_que_percent_als = &my_quer_per_als;
  }

  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_per_als() failed");
  }
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_send_ath (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_send my_bat_cmd_send;
  T_BAT_no_parameter dummy;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_ath()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  dummy.bat_dummy = 0xFF;

  my_bat_cmd_send.ctrl_params = BAT_CMD_AT_H;
  my_bat_cmd_send.params.ptr_at_h = &dummy;
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_ath() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);

  return 0;
}


GLOBAL char *app_bat_send_q_cscb(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                     char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result    = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_send     lbatSend;
  T_BAT_no_parameter lbatDummy;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_q_cscb()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  lbatSend.ctrl_params              = BAT_CMD_QUE_PLUS_CSCB;
  lbatDummy.bat_dummy               = 0xFF;
  lbatSend.params.ptr_que_plus_cscb = &lbatDummy;

  result = bat_send (clnt_hndl, &lbatSend);

  if(result NEQ BAT_OK)
  {
      TRACE_ERROR("Call to app_bat_send_q_cscb() failed");
      return 0;
  }  
  return 0;
}

GLOBAL char *app_bat_send_q_cpinf(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                     char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result    = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_send     lbatSend;
  T_BAT_no_parameter lbatDummy;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("bat_send_q_cpinf()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  lbatSend.ctrl_params              = BAT_CMD_QUE_PERCENT_CPINF;
  lbatDummy.bat_dummy               = 0xFF;
  lbatSend.params.ptr_que_percent_cpinf = &lbatDummy;

  result = bat_send (clnt_hndl, &lbatSend);

  if(result NEQ BAT_OK)
  {
      TRACE_ERROR("Call to bat_send_q_cpinf() failed");
      return 0;
  }  
  return 0;
}


GLOBAL char *app_bat_send_q_cpol(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                 char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result    = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_send     lbatSend;
  T_BAT_no_parameter lbatDummy;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("bat_send_q_cpol()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  lbatSend.ctrl_params              = BAT_CMD_QUE_PLUS_CPOL;
  lbatDummy.bat_dummy               = 0xFF;
  lbatSend.params.ptr_que_plus_cpol = &lbatDummy;

  result = bat_send (clnt_hndl, &lbatSend);

  if(result NEQ BAT_OK)
  {
      TRACE_ERROR("Call to app_bat_send_q_cpinf() failed");
      return 0;
  }  
  return 0;
}

GLOBAL char *app_bat_send_t_cpol(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                 char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result    = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_send     lbatSend;
  T_BAT_no_parameter lbatDummy;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("bat_send_t_cpol()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  lbatSend.ctrl_params = BAT_CMD_TST_PLUS_CPOL;
  lbatDummy.bat_dummy  = 0xFF;
  lbatSend.params.ptr_tst_plus_cpol = &lbatDummy;

  result = bat_send (clnt_hndl, &lbatSend);

  if(result NEQ BAT_OK)
  {
      TRACE_ERROR("Call to app_bat_send_t_cpinf() failed");
      return 0;
  }  
  return 0;
}





GLOBAL char *app_bat_quer_set_clck (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                           char *param2, char *param3, core_func_t core_func)
{
  /* bat_clck_query 0 0 --> unlock FDN, 0 1 --> lock FDN, 0 2 --> query status FDN */
  T_BAT_return result    = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_clck my_set_plus_clck;
  T_BAT_cmd_send my_bat_cmd_send;
  int mode = BAT_CLCK_MODE_QUERY;
  char passwd[] = "0000";
  T_CLIENT_HANDLE_ERROR err;
  
  TRACE_FUNCTION("app_bat_quer_set_clck()");
  
  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  mode = get_item(param2, 0, FALSE);
  
  memset(&my_set_plus_clck, sizeof(my_set_plus_clck), FALSE);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CLCK;
  my_bat_cmd_send.params.ptr_set_plus_clck= &my_set_plus_clck;
  my_set_plus_clck.fac   = BAT_CLCK_FAC_FD;
  my_set_plus_clck.bearer_class = BAT_CLASS_NOT_PRESENT;
  
  if(mode NEQ BAT_CLCK_MODE_QUERY)
  {
    memset(&my_set_plus_clck.passwd, sizeof(my_set_plus_clck.passwd), 0);
    strcpy((char *)&(my_set_plus_clck.passwd), (const char *)passwd);
    my_set_plus_clck.c_passwd = strlen(passwd);
    my_set_plus_clck.v_passwd = 1;   
  }
  my_set_plus_clck.mode =(T_BAT_plus_clck_mode) mode;   
   
  result = bat_send (clnt_hndl, &my_bat_cmd_send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to app_bat_send_cops() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}



GLOBAL char *app_bat_ctrl(app_cmd_entry_t *cmd_entry_ptr,
                          char *param1, char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_INVALID_CLIENT_HANDLE;
  T_BAT_ctrl app_bat_ctrl;
  app_bat_ctrl.event = BAT_ABORT;

  clnt_hndl = get_item(param1, 0, FALSE);
  
  if(param2 && !strcmp(string_to_lower(param2), "READY"))
    app_bat_ctrl.event = BAT_APP_READY_RESOURCE;


  result = bat_ctrl (clnt_hndl, &app_bat_ctrl);

  if(result != BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_ctrl() failed");
  }
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_receive_data_cb(app_cmd_entry_t *cmd_entry_ptr,
                          char *param1, char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_GDD_CON_HANDLE con_hndl = 0xffff;
  char *buf = "BAT_LIB_TEST";
  
  TRACE_FUNCTION("app_bat_receive_data_cb()") ;
  con_hndl = (T_GDD_CON_HANDLE) get_item(param1, 0x0100, FALSE);

  result = (T_BAT_return) bat_gdd_receive_data_cb(con_hndl, (T_GDD_BUF *)buf);

  if(result != BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to app_bat_receive_data_cb() failed");
  }    
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_signal_cb(app_cmd_entry_t *cmd_entry_ptr,
                          char *param1, char *param2, char *param3, core_func_t core_func)
{
  T_GDD_CON_HANDLE con_hndl = 0xffff;
  T_GDD_SIGNAL gdd_sig = {(T_GDD_SIGTYPE)0};
  
  con_hndl = get_item(param1, 0x0100, FALSE);
  gdd_sig.sig = (T_GDD_SIGTYPE) get_item(param2, 1, FALSE);

  bat_gdd_signal_cb(con_hndl, gdd_sig);
  app_bat_send_result_prim(0, NO_ADD_VAL);

  return 0;
}


/*****************************************************************************************
 *
 * This test case tests the complete use scenario: initializing the instances, opening 
 * different clients, sending cmd, closing the clients, deiniting the instances. Then repeat
 * this once more.
 *
 *****************************************************************************************/

LOCAL void free_mem(void *mem1, void *mem2, void *mem3, void *mem4, void *mem5)
{
  if (mem1 NEQ NULL)
    MFREE(mem1);
  if (mem2 NEQ NULL)
    MFREE(mem2);
  if (mem3 NEQ NULL)
    MFREE(mem3);
  if (mem4 NEQ NULL)
    MFREE(mem4);
  if (mem5 NEQ NULL)
    MFREE(mem5);
  return;
}

GLOBAL char *app_bat_complete_test(app_cmd_entry_t *cmd_entry_ptr,
                          char *param1, char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  void *mem1 = NULL;
  void *mem2 = NULL;
  int inst_num = 2;
  int clnt_num = 2;
  int i;

  T_BAT_instance inst_hndl = BAT_INVALID_INSTANCE_HANDLE;
  T_BAT_client  clnt_hndl = BAT_INVALID_CLIENT_HANDLE;
  T_BAT_client  client = BAT_INVALID_CLIENT_HANDLE;

  T_BAT_cmd_set_plus_cfun my_set_plus_cfun;
  T_BAT_cmd_send my_bat_cmd_send;
 
  TRACE_FUNCTION("app_bat_complete_test()");
  for (i = 0; i < 2; i ++)
  {
    MALLOC(mem1, BAT_INSTANCE_HEADER_SIZE*inst_num);  
    MALLOC(mem2, (clnt_num*BAT_CLIENT_SIZE + BAT_INSTANCE_SIZE));


    TRACE_EVENT_P1 ("calling bat_init() the %d th time", i);

    if (bat_init(mem1, (U8)inst_num) NEQ BAT_OK)
    {
      TRACE_ERROR("ERROR: Call to bat_init() failed");
      free_mem(mem1,mem2,NULL,NULL,NULL);
      return 0;
    }

    app_set_config ();

    if (bat_new (&inst_hndl, mem2, (U8)clnt_num, BAT_APP_CONFIG, instance_signal_cb) NEQ BAT_OK)
    {
      TRACE_ERROR("Call to bat_new() failed");
      free_mem(mem1,mem2,NULL,NULL,NULL);
      return 0;
    }
    vsi_t_sleep (VSI_CALLER 100);

    if (bat_open (inst_hndl, &clnt_hndl, response_cb, signal_cb) NEQ BAT_OK)
    {
      TRACE_ERROR("Call to bat_open() failed");
      free_mem(mem1,mem2,NULL,NULL,NULL);
      return 0;
    }
    vsi_t_sleep (VSI_CALLER 100);

    if (bat_uns_open(0, &client, unsolicited_result_cb))
    {
      TRACE_ERROR("Call to bat_uns_open() failed");
      free_mem(mem1,mem2,NULL,NULL,NULL);
      return 0;
    }

    memset(&my_set_plus_cfun, sizeof(my_set_plus_cfun), FALSE);
    my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CFUN;
    my_bat_cmd_send.params.ptr_set_plus_cfun = &my_set_plus_cfun;
    /* full functionality mode*/
    my_set_plus_cfun.fun = BAT_CFUN_FUN_FULL;
    /* Reset the MT before changing functionality level */
    my_set_plus_cfun.rst = BAT_CFUN_RST_NOT_PRESENT;

    TRACE_EVENT("calling bat_send()");
   
    result = bat_send (clnt_hndl, &my_bat_cmd_send);

    if(result NEQ BAT_OK)
    {
      TRACE_ERROR("Call to bat_send() failed");
      free_mem(mem1,mem2,NULL,NULL,NULL);
      return 0;
    }  

    vsi_t_sleep (VSI_CALLER 200);

    TRACE_EVENT("calling bat_close()");
   
    result = bat_close (clnt_hndl);

    if(result NEQ BAT_OK)
    {
      TRACE_ERROR("Call to bat_close() failed");
      free_mem(mem1,mem2,NULL,NULL,NULL);
      return 0;
    }
    vsi_t_sleep (VSI_CALLER 100);

    TRACE_EVENT("calling bat_close() for unsolicited");

    result = bat_close (client);

    if(result NEQ BAT_OK)
    {
      TRACE_ERROR("Call to bat_close() failed");
      free_mem(mem1,mem2,NULL,NULL,NULL);
      return 0;
    }
    result = bat_delete(inst_hndl);
    vsi_t_sleep (VSI_CALLER 300);

    if(result NEQ BAT_OK)
    {
      TRACE_ERROR("Call to bat_delete() failed");
      free_mem(mem1,mem2,NULL,NULL,NULL);
      return 0;
    }

    result = bat_deinit();

    if(result NEQ BAT_OK)
    {
      TRACE_ERROR("Call to bat_deinit() failed");
      free_mem(mem1,mem2,NULL,NULL,NULL);
      return 0;
    }

    free_mem(mem1,mem2,NULL,NULL,NULL);
  }
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

/*****************************************************************************************
 *
 * This test case tests the flow control between app and BAT Lib
 *
 *****************************************************************************************/

GLOBAL char *app_bat_flow_control_test1(app_cmd_entry_t *cmd_entry_ptr,
                          char *param1, char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  void *mem1 = NULL;
  void *mem2 = NULL;
  int inst_num = 2;
  int clnt_num = 2;
  T_BAT_instance inst_hndl = BAT_INVALID_INSTANCE_HANDLE;
  T_BAT_client  clnt_hndl = BAT_INVALID_CLIENT_HANDLE;

  T_BAT_cmd_set_plus_cfun my_set_plus_cfun;
  T_BAT_cmd_send my_bat_cmd_send;
  T_BAT_ctrl app_bat_ctrl;
 
  TRACE_FUNCTION("app_bat_complete_test1()");
  MALLOC(mem1, BAT_INSTANCE_HEADER_SIZE*inst_num);  
  MALLOC(mem2, (clnt_num*BAT_CLIENT_SIZE + BAT_INSTANCE_SIZE));


  TRACE_EVENT("calling bat_init()");

  result = bat_init(mem1, (U8)inst_num);

  if(result NEQ BAT_OK)
  {
    TRACE_ERROR("ERROR: Call to bat_init() failed");
    free_mem(mem1,mem2,NULL,NULL,NULL);
    return 0;
  }

  app_set_config ();

  TRACE_EVENT("calling bat_new()");

  result = bat_new (&inst_hndl, mem2, (U8)clnt_num, BAT_APP_CONFIG, instance_signal_cb);

  TRACE_EVENT_P1 ("inst_hndl is %d", inst_hndl);

  if(result != BAT_OK)
  {
    TRACE_ERROR("Call to bat_new() failed");
    free_mem(mem1,mem2,NULL,NULL,NULL);
    return 0;
  }

  TRACE_EVENT("calling bat_open()");

  result = bat_open (inst_hndl, &clnt_hndl, response_cb_busy, signal_cb);

  if(result NEQ BAT_OK)
  {
    TRACE_ERROR("Call to bat_open() failed");
    free_mem(mem1,mem2,NULL,NULL,NULL);
    return 0;
  }

  TRACE_EVENT("calling bat_uns_open()");

  memset(&my_set_plus_cfun, sizeof(my_set_plus_cfun), FALSE);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CFUN;
  my_bat_cmd_send.params.ptr_set_plus_cfun = &my_set_plus_cfun;
  /* full functionality mode*/
  my_set_plus_cfun.fun = BAT_CFUN_FUN_FULL;
  /* Reset the MT before changing functionality level */
  my_set_plus_cfun.rst = BAT_CFUN_RST_NOT_PRESENT;

  TRACE_EVENT("calling bat_send()");
 
  result = bat_send (clnt_hndl, &my_bat_cmd_send);
  if(result NEQ BAT_OK)
  {
    TRACE_ERROR("Call to bat_send() failed");
    free_mem(mem1,mem2,NULL,NULL,NULL);
    return 0;
  } 

  app_bat_ctrl.event = BAT_APP_READY_RESOURCE;
  result = bat_ctrl(clnt_hndl, &app_bat_ctrl);
  vsi_t_sleep (VSI_CALLER 500);

  if(result NEQ BAT_OK)
  {
    TRACE_ERROR("Call to bat_ctrl() failed");
    free_mem(mem1,mem2,NULL,NULL,NULL);
    return 0;
  } 
  result = bat_send (clnt_hndl, &my_bat_cmd_send);
  vsi_t_sleep (VSI_CALLER 500);

  if(result NEQ BAT_OK)
  {
    TRACE_ERROR("Call to bat_send() failed");
    free_mem(mem1,mem2,NULL,NULL,NULL);
    return 0;
  }
  app_bat_send_result_prim(0, NO_ADD_VAL); 
  return 0;
}


/*****************************************************************************************
 *
 * This test case specifically tests receiving of unsolicited code, aborting of a 
 * running command and running a 2nd BAT cmd afterwords.
 *
 * NOTE: this test case has to be used together with the PSI simulation stub.
 *
 *****************************************************************************************/

GLOBAL char *app_bat_uns_rcv_and_abort_test(app_cmd_entry_t *cmd_entry_ptr,
                          char *param1, char *param2, char *param3, core_func_t core_func)
{
  T_BAT_instance inst_hndl = BAT_INVALID_INSTANCE_HANDLE;
  T_BAT_client  clnt_hndl = BAT_INVALID_CLIENT_HANDLE;
  int i;

  static T_BAT_client test_clnt_table[1][8+1] = {
                        {0,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff}};
  static void *mem_hdr = NULL;
  void *mem_inst = NULL;

  T_BAT_cmd_set_plus_cfun my_set_plus_cfun;
  T_BAT_cmd_send my_bat_cmd_send;
  T_BAT_ctrl app_bat_ctrl;


  memset(&my_set_plus_cfun, sizeof(my_set_plus_cfun), FALSE);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CFUN;
  my_bat_cmd_send.params.ptr_set_plus_cfun = &my_set_plus_cfun;
  /* full functionality mode*/
  my_set_plus_cfun.fun = BAT_CFUN_FUN_FULL;
  /* Reset the MT before changing functionality level */
  my_set_plus_cfun.rst = BAT_CFUN_RST_RESET;

  TRACE_FUNCTION("app_bat_uns_rcv_and_abort_test()");

  MALLOC(mem_hdr, BAT_INSTANCE_HEADER_SIZE*4);  
  MALLOC(mem_inst, BAT_INSTANCE_SIZE + 8*BAT_CLIENT_SIZE);  

  TRACE_EVENT("calling bat_init()");
/* init BAT */
  if ( bat_init(mem_hdr, 1))
  {
    TRACE_ERROR ("Call to bat_init () failed!");
    MFREE(mem_hdr);
    return 0;
  }
  app_set_config ();

  TRACE_EVENT("calling bat_new()");

  if (bat_new (&inst_hndl, mem_inst, 8, BAT_APP_CONFIG, instance_signal_cb) OR inst_hndl NEQ 0)
  {
    TRACE_ERROR("Call to bat_new() failed");
    free_mem(mem_hdr, mem_inst,NULL,NULL,NULL);
    return 0;
  }
  TRACE_EVENT_P1 ("inst_hndl is %d", inst_hndl);
  vsi_t_sleep (VSI_CALLER 300);
  
  TRACE_EVENT("calling bat_uns_open()");

  if (bat_uns_open(0, &clnt_hndl, unsolicited_result_cb))
  {
    free_mem(mem_hdr, mem_inst,NULL,NULL,NULL);
    return 0;
  }

/* create BAT client for this instance */
  for (i = 0; i < 8; i ++)
  {
    TRACE_EVENT_P1("Open clnt_id  %d for inst 0", i);
    if (bat_open(inst_hndl, &test_clnt_table[inst_hndl][i+1],response_cb, signal_cb))
    {
      TRACE_ERROR("Call to bat_open() failed");
      free_mem(mem_hdr, mem_inst,NULL,NULL,NULL);
      return 0;
    }
    vsi_t_sleep (VSI_CALLER 100);
    test_clnt_table[inst_hndl][0]++;

    TRACE_EVENT("calling bat_send()");

 /* send out a command from all clients */
    if (bat_send (test_clnt_table[inst_hndl][i+1], &my_bat_cmd_send))
    {
      TRACE_ERROR("Call to 1st bat_send() failed");
      free_mem(mem_hdr, mem_inst,NULL,NULL,NULL);
      return 0;
    }  
    vsi_t_sleep (VSI_CALLER 300);

/* abort the cmd running on client 7 */
    if (i EQ 7)
    {
      app_bat_ctrl.event = BAT_ABORT;
      if (bat_ctrl(test_clnt_table[inst_hndl][i+1], &app_bat_ctrl))
      {
        TRACE_ERROR("Call to bat_ctrl() failed");
        MFREE(mem_inst);
        return 0;
      }
      vsi_t_sleep (VSI_CALLER 300);
    }

/* send out another command from all clients */
    if (bat_send (test_clnt_table[inst_hndl][i+1], &my_bat_cmd_send))
    {
      TRACE_ERROR("Call to 2nd bat_send() failed");
      free_mem(mem_hdr, mem_inst,NULL,NULL,NULL);
      return 0;
    }  
    vsi_t_sleep (VSI_CALLER 300);
  }
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}


/*****************************************************************************************
 *
 * This test case tests the opening of multiple channels, sending of AT cmds, receiving of
 * unsolicited code, aborting of a running command.
 *
 * NOTE: this test case has to be used together with the PSI simulation stub.
 *
 *****************************************************************************************/

GLOBAL char *app_bat_multi_channel_creation_test(app_cmd_entry_t *cmd_entry_ptr,
                          char *param1, char *param2, char *param3, core_func_t core_func)
{
  T_BAT_instance inst_hndl[4] = {0xff,0xff,0xff,0xff};
  int i,j;

/* 4 instance with 8 clnts each, the first element is the num of opened clnts */
  static T_BAT_client test_clnt_table[4][6+1] = {
                                              {0,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff},
                                              {0,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff},
                                              {0,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff},
                                              {0,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff}};
  void *mem_hdr = NULL;

  void *mem_inst[4] = {NULL,NULL,NULL,NULL};

  T_BAT_cmd_set_plus_cfun my_set_plus_cfun;
  T_BAT_cmd_send my_bat_cmd_send;


  memset(&my_set_plus_cfun, sizeof(my_set_plus_cfun), FALSE);
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CFUN;
  my_bat_cmd_send.params.ptr_set_plus_cfun = &my_set_plus_cfun;
  /* full functionality mode*/
  my_set_plus_cfun.fun = BAT_CFUN_FUN_FULL;
  /* Reset the MT before changing functionality level */
  my_set_plus_cfun.rst = BAT_CFUN_RST_RESET;



  TRACE_FUNCTION("app_bat_multi_channel_creation_test()");

  MALLOC(mem_hdr, BAT_INSTANCE_HEADER_SIZE*4);  

  TRACE_EVENT("calling bat_init()");
/* init BAT */
  if ( bat_init(mem_hdr, 4))
  {
    TRACE_ERROR ("Call to bat_init () failed!");
    MFREE(mem_hdr);
    return 0;
  }
  app_set_config ();


/*************************************************************************************************
 *
 * create 4 BAT instances with each instance 6 clients
 *
 ************************************************************************************************/
  for (i = 0; i < 4; i ++)
  {
    TRACE_EVENT_P1("calling bat_new() for the %d th instance", i);
    MALLOC(mem_inst[i], BAT_INSTANCE_SIZE + 8*BAT_CLIENT_SIZE);  
    if (bat_new (&inst_hndl[i], mem_inst[i], 8, BAT_APP_CONFIG, instance_signal_cb) OR inst_hndl[i] NEQ i)
    {
      free_mem (mem_hdr,mem_inst[0],mem_inst[1],mem_inst[2],mem_inst[3]);
      return 0;
    }
    TRACE_EVENT_P1 ("created inst_hndl:  %d", inst_hndl[i]);
    for (j = 0; j < 6; j ++)
    {
      TRACE_EVENT_P1("Opened clnt_id:  %d ", j);
      if (bat_open(inst_hndl[i], &test_clnt_table[i][j+1],response_cb, signal_cb))
      {
        free_mem (mem_hdr,mem_inst[0],mem_inst[1],mem_inst[2],mem_inst[3]);
        return 0;
      }
      vsi_t_sleep (VSI_CALLER 100);
      test_clnt_table[inst_hndl[i]][0]++;
 /* send out a command from all clients */
      if (bat_send (test_clnt_table[i][j+1], &my_bat_cmd_send))
      {
        free_mem (mem_hdr,mem_inst[0],mem_inst[1],mem_inst[2],mem_inst[3]);
        return 0;
      }  
      vsi_t_sleep (VSI_CALLER 300);
    }
  }
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_stress_test (app_cmd_entry_t *cmd_entry_ptr, char *param1,
                                  char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_plus_cfun my_set_plus_cfun;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;
  U16 i = 0;
 
  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  while (i<400)
  {
    TRACE_EVENT_P1("Processing cycle %d",i);

    memset(&my_set_plus_cfun, sizeof(my_set_plus_cfun), FALSE);

    my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PLUS_CFUN;

    my_bat_cmd_send.params.ptr_set_plus_cfun = &my_set_plus_cfun;

    /* full functionality mode*/
    my_set_plus_cfun.fun = BAT_CFUN_FUN_FULL;

    /* Reset the MT before changing functionality level */
    my_set_plus_cfun.rst = BAT_CFUN_RST_NOT_PRESENT;

    result = bat_send (clnt_hndl, &my_bat_cmd_send);
 
    if(result NEQ BAT_OK)
    {
      app_bat_send_result_prim(1, NO_ADD_VAL);
      break;
    }

    vsi_t_sleep (VSI_CALLER 3000);
    i++;
  }

  if(result NEQ BAT_OK)
  {
    return ("Call to bat_send_cfun() failed");
  }
  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
}

GLOBAL char *app_bat_send_cniv(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                               char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_set_percent_cniv my_cmd_percent_cniv;
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;
  
  TRACE_FUNCTION("app_bat_send_cniv()");
  
  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }
  memset(&my_cmd_percent_cniv, 0 , sizeof(my_cmd_percent_cniv));
  my_bat_cmd_send.ctrl_params = BAT_CMD_SET_PERCENT_CNIV;
  my_bat_cmd_send.params.ptr_set_percent_cniv = &my_cmd_percent_cniv;
  
  if ( (param2 NEQ NULL) AND atoi(param2) )
  { 
    my_cmd_percent_cniv.mode= BAT_CNIV_MODE_ON;
  }  
  
  result = bat_send (clnt_hndl, &my_bat_cmd_send);
  
  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_cniv() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;
  
  
}

GLOBAL char *app_bat_send_q_cniv(app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                               char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_no_parameter    my_que_percent_cniv; 
  T_BAT_cmd_send my_bat_cmd_send;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_q_cniv()");
  
  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  my_bat_cmd_send.ctrl_params = BAT_CMD_QUE_PERCENT_CNIV;
  my_bat_cmd_send.params.ptr_que_percent_cniv = &my_que_percent_cniv;

  result = bat_send (clnt_hndl, &my_bat_cmd_send);
  
  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_q_cniv() failed");
  }  
  app_bat_send_result_prim(0, NO_ADD_VAL);
  return 0;

}

GLOBAL char *app_bat_send_custom (app_cmd_entry_t *cmd_entry_ptr, char *param1, 
                                     char *param2, char *param3, core_func_t core_func)
{
  T_BAT_return result = BAT_ERROR;
  T_BAT_client clnt_hndl = BAT_CONTROL_CHANNEL;
  T_BAT_cmd_send send;  
  T_BAT_custom custom;
  T_CLIENT_HANDLE_ERROR err;

  TRACE_FUNCTION("app_bat_send_custom()");

  clnt_hndl = get_item(param1, 0, FALSE);
  
  err = check_client_handle (clnt_hndl);
  if (err NEQ CLIENT_HANDLE_OK)
  {
    return (error_client_handle(err));
  }

  send.ctrl_params = BAT_CMD_CUSTOM;
  send.params.ptr_custom = &custom;

  if (param2 EQ NULL)
  {     
    strcpy((char *)custom.buf, "OK");
  }
  else if (strcmp(param2, "DATA") EQ 0)
  { 
    custom.buf[0] = 0x80;
    custom.buf[1] = 0x81;
    custom.buf[2] = 0x82;
    custom.buf[3] = 0x83;
    custom.buf[4] = 0x84;
    custom.buf[5] = 0x85;
    custom.buf[6] = 0x86;
    custom.buf[7] = 0x87;
    custom.buf[8] = 0x88;
    custom.buf[9] = 0x89;
    custom.buf[10] = 0x8A;
    custom.buf[11] = 0x8B;
    custom.buf[12] = 0x8C;
    custom.buf[13] = 0x8D;
    custom.buf[14] = 0x8E;
    custom.buf[15] = 0x8F;
    custom.buf[16] = 0x90;
    custom.buf[17] = 0x91;
    custom.buf[18] = 0x92;
    custom.buf[19] = 0x93;
    custom.buf[20] = 0x94;
    custom.buf[21] = 0x95;
    custom.buf[22] = 0x96;
    custom.buf[23] = 0x97;
    custom.buf[24] = 0x98;
    custom.buf[25] = 0x99;
    custom.buf[26] = 0x9A;
    custom.buf[27] = 0x9B;
    custom.buf[28] = 0x9C;
    custom.buf[29] = 0x9D;
    custom.buf[30] = 0x9E;
    custom.buf[31] = 0x9F;
    custom.buf[32] = 0xA0;
    custom.buf[33] = 0xA1;
    custom.buf[34] = 0xA2;
    custom.buf[35] = 0xA3;
    custom.buf[36] = 0xA4;
    custom.buf[37] = 0xA5;
    custom.buf[38] = 0xA6;
    custom.buf[39] = 0xA7;
    custom.buf[40] = 0xA8;
    custom.buf[41] = 0xA9;
    custom.buf[42] = 0xAA;
    custom.buf[43] = 0xAB;
    custom.buf[44] = 0xAC;
    custom.buf[45] = 0xAD;
    custom.buf[46] = 0xAE;
    custom.buf[47] = 0xAF;
    custom.buf[48] = 0xB0;
    custom.buf[49] = 0xB1;
    custom.buf[50] = 0xB2;
    custom.buf[51] = 0xB3;
    custom.buf[52] = 0xB4;
    custom.buf[53] = 0xB5;
    custom.buf[54] = 0xB6;
    custom.buf[55] = 0xB7;
    custom.buf[56] = 0xB8;
    custom.buf[57] = 0xB9;
    custom.buf[58] = 0xBA;
    custom.buf[59] = 0xBB;
    custom.buf[60] = 0xBC;
    custom.buf[61] = 0xBD;
    custom.buf[62] = 0xBE;
    custom.buf[63] = 0xBF;
    custom.buf[64] = 0xC0;
    custom.buf[65] = 0xC1;
    custom.buf[66] = 0xC2;
    custom.buf[67] = 0xC3;
    custom.buf[68] = 0xC4;
    custom.buf[69] = 0xC5;
    custom.buf[70] = 0xC6;
    custom.buf[71] = 0xC7;
    custom.buf[72] = 0xC8;
    custom.buf[73] = 0xC9;
    custom.buf[74] = 0xCA;
    custom.buf[75] = 0xCB;
    custom.buf[76] = 0xCC;
    custom.buf[77] = 0xCD;
    custom.buf[78] = 0xCE;
    custom.buf[79] = 0xCF;
    custom.buf[80] = 0xD0;
    custom.buf[81] = 0xD1;
    custom.buf[82] = 0xD2;
    custom.buf[83] = 0xD3;
    custom.buf[84] = 0xD4;
    custom.buf[85] = 0xD5;
    custom.buf[86] = 0xD6;
    custom.buf[87] = 0xD7;
    custom.buf[88] = 0xD8;
    custom.buf[89] = 0xD9;
    custom.buf[90] = 0xDA;
    custom.buf[91] = 0xDB;
    custom.buf[92] = 0xDC;
    custom.buf[93] = 0xDD;
    custom.buf[94] = 0xDE;
    custom.buf[95] = 0xDF;
    custom.buf[96] = 0xE0;
    custom.buf[97] = 0xE1;
    custom.buf[98] = 0xE2;
    custom.buf[99] = 0xE3;
    custom.buf[100] = 0xE4;
    custom.buf[101] = 0xE5;
    custom.buf[102] = 0xE6;
    custom.buf[103] = 0xE7;
    custom.buf[104] = 0xE8;
    custom.buf[105] = 0xE9;
    custom.buf[106] = 0xEA;
    custom.buf[107] = 0xEB;
    custom.buf[108] = 0xEC;
    custom.buf[109] = 0xED;
    custom.buf[110] = 0xEE;
    custom.buf[111] = 0xEF;
    custom.buf[112] = 0xF0;
    custom.buf[113] = 0xF1;
    custom.buf[114] = 0xF2;
    custom.buf[115] = 0xF3;
    custom.buf[116] = 0xF4;
    custom.buf[117] = 0xF5;
    custom.buf[118] = 0xF6;
    custom.buf[119] = 0xF7;
    custom.buf[120] = 0xF8;
    custom.buf[121] = 0xF9;
    custom.buf[122] = 0xFA;
    custom.buf[123] = 0xFB;
    custom.buf[124] = 0xFC;
    custom.buf[125] = 0xFD;
    custom.buf[126] = 0xFE;
    custom.buf[127] = 0x00;
  }
  else
  {
    strcpy((char *)custom.buf, param2);
  }
  
  if (param3 && strcmp(param3, "MR") EQ 0)
  {
    send.ctrl_params = (T_BAT_ctrl_params)MOTO_CMD_MR;
    TRACE_EVENT_P1("app_bat_send_custom():  len = %d ", custom.c_buf);
  }
  else
  {
    TRACE_EVENT_P2("app_bat_send_custom():  cmd = %s, len = %d ", custom.buf, custom.c_buf);
  }
  custom.c_buf = strlen((char *)custom.buf);

  result = bat_send (clnt_hndl, &send);

  if(result NEQ BAT_OK)
  {
    app_bat_send_result_prim(1, NO_ADD_VAL);
    return ("Call to bat_send_ata() failed");
  } 
  app_bat_send_result_prim(0, NO_ADD_VAL); 
  return 0;
}






/*---------------------------------------------------------------------------
 * Definition of core functions
 *---------------------------------------------------------------------------*/

/* -- No core functions required for now */


/*==== Exported functions ====================================================*/

char *app_handle_command_bat(char *command)
{
  TRACE_FUNCTION ("app_handle_command_bat()");
  return app_handle_command(command, app_bat_cmd_table);
}


/* EOF */
