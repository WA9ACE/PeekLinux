/*
+------------------------------------------------------------------------------
|  File:       app_bat_atcmds.c
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
|  Purpose :  Example application for BAT -- at command command parser.
+-----------------------------------------------------------------------------
*/

#include <string.h>             /* String functions, e. g. strncpy(). */

#include "app_util.h"
#include "p_bat.h"
#include "typedefs.h"
#include "vsi.h"
#include "gdd.h"
#include "bat.h"
#include "bat_ctrl.h"
#include "bat_intern.h"

#ifdef FF_GPF_TCPIP
#include "socket_api.h"
#endif

#include "app.h"


/*
 *  #define CO_FST_BAT_DEBUG
 *  #ifdef CO_FST_BAT_DEBUG
 *  LOCAL T_BAT_return bat_send (T_BAT_client clnt_hndl, T_BAT_cmd_send *cmd)
 *  { TRACE_EVENT("bat_send()"); return BAT_OK; }
 *  #else
 *  EXTERN T_BAT_return bat_send (T_BAT_client clnt_hndl, T_BAT_cmd_send *cmd);
 *  #endif
 */


EXTERN T_BAT_client clnt_table[][BAT_MAX_TEST_CLNT_NUM+1];

static char *handle_atcmd(app_cmd_entry_t *, char *,char *, char *, core_func_t );
static char *atcmd_help(app_cmd_entry_t *, char *,char *, char *, core_func_t );
static void exec_at_cmd(int at_cmd, char* param1, char* param2);

static app_cmd_entry_t app_bat_atcmds_table[] = {
  { "at_bat_cmd",  handle_atcmd,  exec_at_cmd,  "at_cmd [param]" },
  { "at_bat_help", atcmd_help,    0,            "" },
  {0,0,0,0}
};

  /*
  ** Add all the BAT parameters into a union ... if each is defined in there own case statement the compiler
  ** adds them ALL individually onto the stack and takes up a LOT of space ... Oooops!
  */
static union
{
  T_BAT_cmd_set_percent_als percent_als;     /*<  0:  4> pointer to The set command structure of the %ALS command. */
  T_BAT_cmd_set_percent_band percent_band;   /*<  0:  4> pointer to The set command structure of the %BAND command. */
  T_BAT_cmd_andc            andc;                /*<  0:  4> pointer to The set command structure of the &C command. */
  T_BAT_cmd_set_plus_cacm   plus_cacm;       /*<  0:  4> pointer to The set command structure of the +CACM command. */
  T_BAT_cmd_set_plus_camm   plus_camm;       /*<  0:  4> pointer to The set command structure of the +CAMM command. */
  T_BAT_cmd_set_plus_caoc   plus_caoc;       /*<  0:  4> pointer to The set command structure of the +CAOC command. */
  T_BAT_cmd_set_percent_cbhz percent_cbhz;   /*<  0:  4> pointer to The set command structure of the %CBHZ command. */
  T_BAT_cmd_set_plus_cbst   plus_cbst;       /*<  0:  4> pointer to The set command structure of the +CBST command. */
  T_BAT_cmd_set_percent_ccbs percent_ccbs;   /*<  0:  4> pointer to The set command structure of the %CCBS command. */
  T_BAT_cmd_set_plus_ccfc   plus_ccfc;       /*<  0:  4> pointer to The set command structure of the +CCFC command. */
  T_BAT_cmd_set_plus_cclk   plus_cclk;       /*<  0:  4> pointer to The set command structure of the +CCLK command. */
  T_BAT_cmd_set_plus_ccug   plus_ccug;       /*<  0:  4> pointer to The set command structure of the +CCUG command. */
  T_BAT_cmd_set_plus_ccwa   plus_ccwa;       /*<  0:  4> pointer to The set command structure of the +CCWA command. */
  T_BAT_cmd_set_plus_ccwa_w plus_ccwa_w;     /*<  0:  4> pointer to The set command structure of the +CCWA_W command. */
  T_BAT_cmd_set_plus_cfun   plus_cfun;       /*<  0:  4> pointer to The set command structure of the +CFUN command. */
  T_BAT_cmd_set_percent_cgaatt percent_cgaatt; /*<  0:  4> pointer to The set command structure of the %CGAATT command. */
  T_BAT_cmd_set_plus_cgact  plus_cgact;      /*<  0:  4> pointer to The set command structure of the +CGACT command. */
  T_BAT_cmd_set_plus_cgans  plus_cgans;      /*<  0:  4> pointer to The set command structure of the +CGANS command. */
  T_BAT_cmd_set_plus_cgatt  plus_cgatt;      /*<  0:  4> pointer to The set command structure of the +CGATT command. */
  T_BAT_cmd_set_percent_cgclass percent_cgclass; /*<  0:  4> pointer to The set command structure of the %CGCLASS command. */
  T_BAT_cmd_set_plus_cgclass plus_cgclass;   /*<  0:  4> pointer to The set command structure of the +CGCLASS command. */
  T_BAT_cmd_set_plus_cgdata plus_cgdata;     /*<  0:  4> pointer to The set command structure of the +CGDATA command. */
  T_BAT_cmd_set_plus_cgdcont plus_cgdcont;   /*<  0:  4> pointer to The set command structure of the +CGDCONT command. */
  T_BAT_cmd_set_percent_cgmm percent_cgmm;   /*<  0:  4> pointer to The set command structure of the %CGMM command. */
  T_BAT_cmd_set_plus_cgpaddr plus_cgpaddr;   /*<  0:  4> pointer to The set command structure of the +CGPADDR command. */
  T_BAT_cmd_set_percent_cgpco percent_cgpco; /*<  0:  4> pointer to The set command structure of the %CGPCO command. */
  T_BAT_cmd_set_percent_cgppp percent_cgppp; /*<  0:  4> pointer to The set command structure of the %CGPPP command. */
  T_BAT_cmd_set_plus_cgqmin plus_cgqmin;     /*<  0:  4> pointer to The set command structure of the +CGQMIN command. */
  T_BAT_cmd_set_plus_cgqreq plus_cgqreq;     /*<  0:  4> pointer to The set command structure of the +CGQREQ command. */
  T_BAT_cmd_set_plus_cgsms  plus_cgsms;      /*<  0:  4> pointer to The set command structure of the +CGSMS command. */
  T_BAT_cmd_set_percent_chld percent_chld;   /*<  0:  4> pointer to The set command structure of the %CHLD command. */
  T_BAT_cmd_set_percent_chpl percent_chpl;   /*<  0:  4> pointer to The set command structure of the %CHPL command. */
  T_BAT_cmd_set_percent_chpl_w percent_chpl_w; /*<  0:  4> pointer to The set command structure of the %CHPL_W command. */
  T_BAT_cmd_set_plus_cind   plus_cind;       /*<  0:  4> pointer to The set command structure of the +CIND command. */
  T_BAT_cmd_set_plus_clan   plus_clan;       /*<  0:  4> pointer to The set command structure of the +CLAN command. */
  T_BAT_cmd_set_plus_clck   plus_clck;       /*<  0:  4> pointer to The set command structure of the +CLCK command. */
  T_BAT_cmd_set_plus_clir   plus_clir;       /*<  0:  4> pointer to The set command structure of the +CLIR command. */
  T_BAT_cmd_set_plus_clvl   plus_clvl;       /*<  0:  4> pointer to The set command structure of the +CLVL command. */
  T_BAT_cmd_set_plus_cmer   plus_cmer;       /*<  0:  4> pointer to The set command structure of the +CMER command. */
  T_BAT_cmd_set_plus_cmgc   plus_cmgc;       /*<  0:  4> pointer to The set command structure of the +CMGC command. */
  T_BAT_cmd_set_plus_cmgd   plus_cmgd;       /*<  0:  4> pointer to The set command structure of the +CMGD command. */
  T_BAT_cmd_set_plus_cmgl   plus_cmgl;       /*<  0:  4> pointer to The set command structure of the +CMGL command. */
  T_BAT_cmd_set_plus_cmgl_w plus_cmgl_w;     /*<  0:  4> pointer to The set command structure of the +CMGL_W command. */
  T_BAT_cmd_set_plus_cmgr   plus_cmgr;       /*<  0:  4> pointer to The set command structure of the +CMGR command. */
  T_BAT_cmd_set_plus_cmgr_w plus_cmgr_w;     /*<  0:  4> pointer to The set command structure of the +CMGR_W command. */
  T_BAT_cmd_set_plus_cmgs   plus_cmgs;       /*<  0:  4> pointer to The set command structure of the +CMGS command. */
  T_BAT_cmd_set_plus_cmgw   plus_cmgw;       /*<  0:  4> pointer to The set command structure of the +CMGW command. */
  T_BAT_cmd_set_plus_cmod   plus_cmod;       /*<  0:  4> pointer to The set command structure of the +CMOD command. */
  T_BAT_cmd_set_plus_cmss   plus_cmss;       /*<  0:  4> pointer to The set command structure of the +CMSS command. */
  T_BAT_cmd_set_plus_cmut   plus_cmut;       /*<  0:  4> pointer to The set command structure of the +CMUT command. */
  T_BAT_cmd_set_plus_cmux   plus_cmux;       /*<  0:  4> pointer to The set command structure of the +CMUX command. */
  T_BAT_cmd_set_plus_cnma   plus_cnma;       /*<  0:  4> pointer to The set command structure of the +CNMA command. */
  T_BAT_cmd_set_plus_cnmi   plus_cnmi;       /*<  0:  4> pointer to The set command structure of the +CNMI command. */
  T_BAT_cmd_set_percent_cops percent_cops;   /*<  0:  4> pointer to The set command structure of the %COPS command. */
  T_BAT_cmd_set_plus_cops   plus_cops;       /*<  0:  4> pointer to The set command structure of the +COPS command. */
  T_BAT_cmd_set_percent_cops_w percent_cops_w; /*<  0:  4> pointer to The set command structure of the %COPS_W command. */
  T_BAT_cmd_set_percent_cpals percent_cpals; /*<  0:  4> pointer to The set command structure of the %CPALS command. */
  T_BAT_cmd_set_percent_cpals_w percent_cpals_w; /*<  0:  4> pointer to The set command structure of the %CPALS_W command. */
  T_BAT_cmd_set_plus_cpbf   plus_cpbf;       /*<  0:  4> pointer to The set command structure of the +CPBF command. */
  T_BAT_cmd_set_plus_cpbf_w plus_cpbf_w;     /*<  0:  4> pointer to The set command structure of the +CPBF_W command. */
  T_BAT_cmd_set_plus_cpbr   plus_cpbr;       /*<  0:  4> pointer to The set command structure of the +CPBR command. */
  T_BAT_cmd_set_plus_cpbr_w plus_cpbr_w;     /*<  0:  4> pointer to The set command structure of the +CPBR_W command. */
  T_BAT_cmd_set_plus_cpbs   plus_cpbs;       /*<  0:  4> pointer to The set command structure of the +CPBS command. */
  T_BAT_cmd_set_plus_cpbw   plus_cpbw;       /*<  0:  4> pointer to The set command structure of the +CPBW command. */
  T_BAT_cmd_set_plus_cpbw_w plus_cpbw_w;     /*<  0:  4> pointer to The set command structure of the +CPBW_W command. */
  T_BAT_cmd_set_percent_cpcfu percent_cpcfu; /*<  0:  4> pointer to The set command structure of the %CPCFU command. */
  T_BAT_cmd_set_percent_cphs percent_cphs;   /*<  0:  4> pointer to The set command structure of the %CPHS command. */
  T_BAT_cmd_set_plus_cpin   plus_cpin;       /*<  0:  4> pointer to The set command structure of the +CPIN command. */
  T_BAT_cmd_set_percent_cpmb percent_cpmb;   /*<  0:  4> pointer to The set command structure of the %CPMB command. */
  T_BAT_cmd_set_percent_cpmbw percent_cpmbw; /*<  0:  4> pointer to The set command structure of the %CPMBW command. */
  T_BAT_cmd_set_percent_cpmbw_w percent_cpmbw_w; /*<  0:  4> pointer to The set command structure of the %CPMBW_W command. */
  T_BAT_cmd_set_percent_cpmb_w percent_cpmb_w; /*<  0:  4> pointer to The set command structure of the %CPMB_W command. */
  T_BAT_cmd_set_plus_cpms   plus_cpms;       /*<  0:  4> pointer to The set command structure of the +CPMS command. */
  T_BAT_cmd_set_percent_cpnums percent_cpnums; /*<  0:  4> pointer to The set command structure of the %CPNUMS command. */
  T_BAT_cmd_set_percent_cpnums_w percent_cpnums_w; /*<  0:  4> pointer to The set command structure of the %CPNUMS_W command. */
  T_BAT_cmd_set_plus_cpol   plus_cpol;       /*<  0:  4> pointer to The set command structure of the +CPOL command. */
  T_BAT_cmd_set_plus_cpol_w plus_cpol_w;     /*<  0:  4> pointer to The set command structure of the +CPOL_W command. */
  T_BAT_cmd_set_percent_cprsm percent_cprsm; /*<  0:  4> pointer to The set command structure of the %CPRSM command. */
  T_BAT_cmd_set_plus_cpuc   plus_cpuc;       /*<  0:  4> pointer to The set command structure of the +CPUC command. */
  T_BAT_cmd_set_plus_cpuc_w plus_cpuc_w;     /*<  0:  4> pointer to The set command structure of the +CPUC_W command. */
  T_BAT_cmd_set_percent_cpvwi percent_cpvwi; /*<  0:  4> pointer to The set command structure of the %CPVWI command. */
  T_BAT_cmd_set_plus_cpwd   plus_cpwd;       /*<  0:  4> pointer to The set command structure of the +CPWD command. */
  T_BAT_cmd_set_plus_cres   plus_cres;       /*<  0:  4> pointer to The set command structure of the +CRES command. */
  T_BAT_cmd_set_plus_crlp   plus_crlp;       /*<  0:  4> pointer to The set command structure of the +CRLP command. */
  T_BAT_cmd_set_plus_crsm   plus_crsm;       /*<  0:  4> pointer to The set command structure of the +CRSM command. */
  T_BAT_cmd_set_plus_csas   plus_csas;       /*<  0:  4> pointer to The set command structure of the +CSAS command. */
  T_BAT_cmd_set_plus_csca   plus_csca;       /*<  0:  4> pointer to The set command structure of the +CSCA command. */
  T_BAT_cmd_set_plus_cscb   plus_cscb;       /*<  0:  4> pointer to The set command structure of the +CSCB command. */
  T_BAT_cmd_set_plus_cscs   plus_cscs;       /*<  0:  4> pointer to The set command structure of the +CSCS command. */
  T_BAT_cmd_set_plus_csim   plus_csim;       /*<  0:  4> pointer to The set command structure of the +CSIM command. */
  T_BAT_cmd_set_plus_csms   plus_csms;       /*<  0:  4> pointer to The set command structure of the +CSMS command. */
  T_BAT_cmd_set_plus_csns   plus_csns;       /*<  0:  4> pointer to The set command structure of the +CSNS command. */
  T_BAT_cmd_set_plus_csta   plus_csta;       /*<  0:  4> pointer to The set command structure of the +CSTA command. */
  T_BAT_cmd_set_plus_csvm   plus_csvm;       /*<  0:  4> pointer to The set command structure of the +CSVM command. */
  T_BAT_cmd_set_plus_ctfr   plus_ctfr;       /*<  0:  4> pointer to The set command structure of the +CTFR command. */
  T_BAT_cmd_set_percent_ctty percent_ctty;   /*<  0:  4> pointer to The set command structure of the %CTTY command. */
  T_BAT_cmd_set_plus_ctzu   plus_ctzu;       /*<  0:  4> pointer to The set command structure of the +CTZU command. */
  T_BAT_cmd_set_plus_cusd   plus_cusd;       /*<  0:  4> pointer to The set command structure of the +CUSD command. */
  T_BAT_cmd_set_plus_cusd_w plus_cusd_w;     /*<  0:  4> pointer to The set command structure of the +CUSD_W command. */
  T_BAT_cmd_set_percent_cust percent_cust;   /*<  0:  4> pointer to The set command structure of the %CUST command. */
  T_BAT_cmd_set_percent_cwup percent_cwup;   /*<  0:  4> pointer to The set command structure of the %CWUP command. */
  T_BAT_cmd_at_d            at_d;                /*<  0:  4> pointer to The set command structure of the D command. */
  T_BAT_cmd_set_percent_data percent_data;   /*<  0:  4> pointer to The set command structure of the %DATA command. */
  T_BAT_cmd_set_percent_dinf percent_dinf;   /*<  0:  4> pointer to The set command structure of the %DINF command. */
  T_BAT_cmd_set_plus_ds     plus_ds;         /*<  0:  4> pointer to The set command structure of the +DS command. */
  T_BAT_cmd_at_d_w          at_d_w;              /*<  0:  4> pointer to The set command structure of the D_W command. */
  T_BAT_cmd_set_percent_em  percent_em;      /*<  0:  4> pointer to The set command structure of the %EM command. */
  T_BAT_cmd_set_plus_fap    plus_fap;        /*<  0:  4> pointer to The set command structure of the +FAP command. */
  T_BAT_cmd_set_plus_fbo    plus_fbo;        /*<  0:  4> pointer to The set command structure of the +FBO command. */
  T_BAT_cmd_set_plus_fbu    plus_fbu;        /*<  0:  4> pointer to The set command structure of the +FBU command. */
  T_BAT_cmd_set_plus_fcc    plus_fcc;        /*<  0:  4> pointer to The set command structure of the +FCC command. */
  T_BAT_cmd_set_plus_fclass plus_fclass;     /*<  0:  4> pointer to The set command structure of the +FCLASS command. */
  T_BAT_cmd_set_plus_fcq    plus_fcq;        /*<  0:  4> pointer to The set command structure of the +FCQ command. */
  T_BAT_cmd_set_plus_fcr    plus_fcr;        /*<  0:  4> pointer to The set command structure of the +FCR command. */
  T_BAT_cmd_set_plus_fct    plus_fct;        /*<  0:  4> pointer to The set command structure of the +FCT command. */
  T_BAT_cmd_set_plus_fea    plus_fea;        /*<  0:  4> pointer to The set command structure of the +FEA command. */
  T_BAT_cmd_set_plus_ffc    plus_ffc;        /*<  0:  4> pointer to The set command structure of the +FFC command. */
  T_BAT_cmd_set_plus_fie    plus_fie;        /*<  0:  4> pointer to The set command structure of the +FIE command. */
  T_BAT_cmd_set_plus_fis    plus_fis;        /*<  0:  4> pointer to The set command structure of the +FIS command. */
  T_BAT_cmd_set_plus_fit    plus_fit;        /*<  0:  4> pointer to The set command structure of the +FIT command. */
  T_BAT_cmd_set_plus_fli    plus_fli;        /*<  0:  4> pointer to The set command structure of the +FLI command. */
  T_BAT_cmd_set_plus_flo    plus_flo;        /*<  0:  4> pointer to The set command structure of the +FLO command. */
  T_BAT_cmd_set_plus_flp    plus_flp;        /*<  0:  4> pointer to The set command structure of the +FLP command. */
  T_BAT_cmd_set_plus_fms    plus_fms;        /*<  0:  4> pointer to The set command structure of the +FMS command. */
  T_BAT_cmd_set_plus_fns    plus_fns;        /*<  0:  4> pointer to The set command structure of the +FNS command. */
  T_BAT_cmd_set_plus_fpa    plus_fpa;        /*<  0:  4> pointer to The set command structure of the +FPA command. */
  T_BAT_cmd_set_plus_fpi    plus_fpi;        /*<  0:  4> pointer to The set command structure of the +FPI command. */
  T_BAT_cmd_set_plus_fps    plus_fps;        /*<  0:  4> pointer to The set command structure of the +FPS command. */
  T_BAT_cmd_set_plus_fpw    plus_fpw;        /*<  0:  4> pointer to The set command structure of the +FPW command. */
  T_BAT_cmd_set_plus_frq    plus_frq;        /*<  0:  4> pointer to The set command structure of the +FRQ command. */
  T_BAT_cmd_set_plus_fsa    plus_fsa;        /*<  0:  4> pointer to The set command structure of the +FSA command. */
  T_BAT_cmd_set_plus_fsp    plus_fsp;        /*<  0:  4> pointer to The set command structure of the +FSP command. */
  T_BAT_cmd_set_plus_icf    plus_icf;        /*<  0:  4> pointer to The set command structure of the +ICF command. */
  T_BAT_cmd_set_plus_ifc    plus_ifc;        /*<  0:  4> pointer to The set command structure of the +IFC command. */
  T_BAT_cmd_set_plus_ipr    plus_ipr;        /*<  0:  4> pointer to The set command structure of the +IPR command. */
  T_BAT_cmd_set_percent_ppp percent_ppp;     /*<  0:  4> pointer to The set command structure of the %PPP command. */
  T_BAT_cmd_set_percent_pvrf percent_pvrf;   /*<  0:  4> pointer to The set command structure of the %PVRF command. */
  T_BAT_cmd_set_percent_rdl percent_rdl;     /*<  0:  4> pointer to The set command structure of the %RDL command. */
  T_BAT_cmd_set_percent_rdlb percent_rdlb;   /*<  0:  4> pointer to The set command structure of the %RDLB command. */
  T_BAT_cmd_set_percent_satc percent_satc;   /*<  0:  4> pointer to The set command structure of the %SATC command. */
  T_BAT_cmd_set_percent_sate percent_sate;   /*<  0:  4> pointer to The set command structure of the %SATE command. */
  T_BAT_cmd_set_percent_satr percent_satr;   /*<  0:  4> pointer to The set command structure of the %SATR command. */
  T_BAT_cmd_set_percent_satt percent_satt;   /*<  0:  4> pointer to The set command structure of the %SATT command. */
  T_BAT_cmd_set_percent_sncnt percent_sncnt; /*<  0:  4> pointer to The set command structure of the %SNCNT command. */
  T_BAT_cmd_set_percent_vts percent_vts;     /*<  0:  4> pointer to The set command structure of the %VTS command. */
  T_BAT_cmd_set_plus_vts    plus_vts;        /*<  0:  4> pointer to The set command structure of the +VTS command. */
  T_BAT_cmd_set_plus_ws46   plus_ws46;       /*<  0:  4> pointer to The set command structure of the +WS46 command. */
  T_BAT_cmd_set_percent_cniv percent_cniv;  /*<  0:  4> pointer to The set command structure of the %CNIV command. */
  T_BAT_no_parameter        no_param;       /*<  0:  4> pointer to Dummy structure                         */
} bat_params;


static char *atcmd_help(app_cmd_entry_t *cmd_entry_ptr,
                        char *param1, char *param2, char *param3, core_func_t core_func)
{
  TRACE_EVENT("Available commands:");
  TRACE_EVENT("at_bat_cmd atd [dialstring]");
  TRACE_EVENT("at_bat_cmd ata");
  TRACE_EVENT("at_bat_cmd at+clir");
//  TRACE_EVENT("at_bat_cmd at+colp");
  TRACE_EVENT("at_bat_cmd at+ceer");
//  TRACE_EVENT("at_bat_cmd at+crc");
  TRACE_EVENT("at_bat_cmd at+cmgs");
  TRACE_EVENT("at_bat_cmd at+cpbs");
  TRACE_EVENT("at_bat_cmd at+cpbr");
  TRACE_EVENT("at_bat_cmd at+cfun");
  TRACE_EVENT("at_bat_cmd at+cpbw");
  TRACE_EVENT("at_bat_cmd at+cscs");
  TRACE_EVENT("at_bat_cmd at+cusd");
  TRACE_EVENT("at_bat_cmd at+ccfc");
  TRACE_EVENT("at_bat_cmd at+cbst");
  TRACE_EVENT("at_bat_help");
  TRACE_EVENT("--------------------------------------");

  return 0 ;
}


static char *handle_atcmd(app_cmd_entry_t *cmd_entry_ptr, char *param1,
                          char *param2, char *param3, core_func_t core_func)
{
  T_BAT_ctrl_params at_cmd;

  TRACE_FUNCTION("handle_atcmd()");

  TRACE_ASSERT(param1 NEQ NULL);
  param1 = string_to_lower(param1);

  if (strcmp(param1, "atd") EQ 0 )          at_cmd = BAT_CMD_AT_D;
  else if (strcmp(param1, "ata") EQ 0 )     at_cmd = BAT_CMD_AT_A;
  else if (strcmp(param1, "ath") EQ 0 )     at_cmd = BAT_CMD_AT_H;
  else if (strcmp(param1, "at+clir") EQ 0 ) at_cmd = BAT_CMD_SET_PLUS_CLIR;
//  else if (strcmp(param1, "at+colp") EQ 0 ) at_cmd = BAT_CMD_SET_PLUS_COLP;
  else if (strcmp(param1, "at+ceer") EQ 0 ) at_cmd = BAT_CMD_SET_PLUS_CEER;
//  else if (strcmp(param1, "at+crc") EQ 0 ) at_cmd = BAT_CMD_SET_PLUS_CRC;
  else if (strcmp(param1, "at+cmgs") EQ 0 ) at_cmd = BAT_CMD_SET_PLUS_CMGS;
  else if (strcmp(param1, "at+cpbs") EQ 0 ) at_cmd = BAT_CMD_SET_PLUS_CPBS;
  else if (strcmp(param1, "at+cpbr") EQ 0 ) at_cmd = BAT_CMD_SET_PLUS_CPBR;
  else if (strcmp(param1, "at+cfun") EQ 0 ) at_cmd = BAT_CMD_SET_PLUS_CFUN;
  else if (strcmp(param1, "at+cpbw") EQ 0 ) at_cmd = BAT_CMD_SET_PLUS_CPBW;
  else if (strcmp(param1, "at+cscs") EQ 0 ) at_cmd = BAT_CMD_SET_PLUS_CSCS;
  else if (strcmp(param1, "at+cusd") EQ 0 ) at_cmd = BAT_CMD_SET_PLUS_CUSD;
  else if (strcmp(param1, "at+ccfc") EQ 0 ) at_cmd = BAT_CMD_SET_PLUS_CCFC;
  else if (strcmp(param1, "at+cbst") EQ 0 ) at_cmd = BAT_CMD_SET_PLUS_CBST;
  else if (strcmp(param1, "at+cops") EQ 0 ) at_cmd = BAT_CMD_SET_PLUS_COPS;
  else if (strcmp(param1, "at%cniv") EQ 0 ) at_cmd = BAT_CMD_SET_PERCENT_CNIV;
  else
  {
    return("Not supported AT cmd");
  }
  core_func(at_cmd, param2, param3);
  return 0;
}


GLOBAL void exec_at_cmd(int at_cmd, char* param1, char* param2)
{
  T_BAT_cmd_send send_cmd;
  T_BAT_return ret = BAT_ERROR;
  T_BAT_client client = BAT_CONTROL_CHANNEL;
  U8 clnt_hndl_upper = 0;
  U8 clnt_hndl_lower = 0;

  bat_params.no_param.bat_dummy = 0;

  TRACE_FUNCTION("exec_at_cmd()");

  client = get_item(param1, 0, FALSE);

  clnt_hndl_upper = (U8)((0xff00&client)>>8);
  clnt_hndl_lower = (U8)(0x00ff&client);

  if((clnt_hndl_lower >= BAT_MAX_TEST_CLNT_NUM) OR (clnt_hndl_upper >= BAT_MAX_TEST_INST_NUM))
  {
      TRACE_ERROR("Input client out of bounds");
      return;
  }

  if(clnt_table[clnt_hndl_upper][clnt_hndl_lower] EQ 0)
  {
      TRACE_ERROR("client handle not valid - no client handle");
      return;
  }

#if 0
  client = clnt_table[0][0];
#ifndef CO_FST_BAT_DEBUG
  assert(client != 0);
#endif

#endif

  send_cmd.ctrl_params = (T_BAT_ctrl_params)at_cmd;

  switch(at_cmd)
  {
    case BAT_CMD_AT_D:
    {
      char number[] = "+493039831178;";
      if (param1[0] == 0)
      {
        strcpy((char *)&(bat_params.at_d.dial_string), (const char *)param1);
        bat_params.at_d.c_dial_string = strlen(param1);
      }
      else
      {
        strcpy((char *)&(bat_params.at_d.dial_string), (const char *)number);
        bat_params.at_d.c_dial_string = strlen(number);
      }
      send_cmd.params.ptr_at_d = &bat_params.at_d;
      ret = bat_send(client, &send_cmd);
      break;
    }

    case BAT_CMD_AT_A:
    {
      send_cmd.params.ptr_at_a = &bat_params.no_param;
      ret = bat_send(client, &send_cmd);
      break;
    }

    case BAT_CMD_AT_H:
    {
      send_cmd.params.ptr_at_h = &bat_params.no_param;
      ret = bat_send(client, &send_cmd);
      break;
    }

    case BAT_CMD_SET_PLUS_CLIR:
    {
      if (param1[0] == '0') bat_params.plus_clir.n = BAT_CLIR_N_SUBSCRIPTION;
      else if (param1[0] == '1') bat_params.plus_clir.n = BAT_CLIR_N_INVOCATION;
      else if (param1[0] == '2') bat_params.plus_clir.n = BAT_CLIR_N_SUPPRESSION;
      else bat_params.plus_clir.n = BAT_CLIR_N_SUPPRESSION; /* No, or an Invalid, Parameter was passed in */

      send_cmd.params.ptr_set_plus_clir = &bat_params.plus_clir;
      ret = bat_send(client, &send_cmd);
      break;
    }
#ifdef _SIMULATION_
#pragma message( __TODO__"BAT_CMD_SET_PLUS_COLP not defined !" )
#endif
/*
    case BAT_CMD_SET_PLUS_COLP:
    {
      ret = bat_send(client, &send_cmd);
      break;
    }
*/
    case BAT_CMD_SET_PLUS_CEER:
    {
/*      T_BAT_cmd_set_plus_ceer ceer;  */
#ifdef _SIMULATION_
#pragma message( __TODO__"T_BAT_cmd_set_plus_ceer not defined !" )
#endif
/*   ceer.
     send_cmd.params.ptr_set_plus_ceer = &ceer;  */
      TRACE_ASSERT(0);
      ret = bat_send(client, &send_cmd);
      break;
    }
#ifdef _SIMULATION_
#pragma message( __TODO__"BAT_CMD_SET_PLUS_CRC not defined !" )
#endif
/*
    case BAT_CMD_SET_PLUS_CRC:
    {
      ret = bat_send(client, &send_cmd);
      break;
    }
*/
    case BAT_CMD_SET_PLUS_CMGS:
    {
      U8 pdu[] = "0891941067000044F301000D91941097614246F8000011C8329BFD06098354103B2D9687E579";
      strcpy((char *)&(bat_params.plus_cmgs.pdu), (const char *)pdu);
      bat_params.plus_cmgs.length = strlen((const char *)pdu);        /* total length */
      bat_params.plus_cmgs.c_pdu = bat_params.plus_cmgs.length - 9;                   /* length without SCA */
      send_cmd.params.ptr_set_plus_cmgs = &bat_params.plus_cmgs;
      ret = bat_send(client, &send_cmd);
      break;
    }

    case BAT_CMD_SET_PLUS_CPBS:
    {
      bat_params.plus_cpbs.storage = BAT_STORAGE_AD;
      send_cmd.params.ptr_set_plus_cpbs = &bat_params.plus_cpbs;
      ret = bat_send(client, &send_cmd);
      break;
    }

    case BAT_CMD_SET_PLUS_CPBR:
    {
      bat_params.plus_cpbr.index1 = 1;
      bat_params.plus_cpbr.index2 = 2;
      send_cmd.params.ptr_set_plus_cpbr = &bat_params.plus_cpbr;
      ret = bat_send(client, &send_cmd);
      break;
    }

    case BAT_CMD_SET_PLUS_CFUN:
    {
      bat_params.plus_cfun.fun = BAT_CFUN_FUN_FULL;
      bat_params.plus_cfun.rst = BAT_CFUN_RST_NOT_PRESENT;
      send_cmd.params.ptr_set_plus_cfun = &bat_params.plus_cfun;
      ret = bat_send(client, &send_cmd);
      break;
    }

    case BAT_CMD_SET_PLUS_CPBW:
    {
      U8 number[] ="03076903367";
      U8   text[] = "Hello World";
      bat_params.plus_cpbw.index = 2;
      bat_params.plus_cpbw.v_number = TRUE;
      strcpy((char *)&(bat_params.plus_cpbw.number), (const char *)number);
      bat_params.plus_cpbw.c_number = strlen((const char *)number);
      bat_params.plus_cpbw.v_text = TRUE;
      strcpy((char *)&(bat_params.plus_cpbw.text), (const char *)text);
      bat_params.plus_cpbw.c_text = strlen((const char *)text);
      bat_params.plus_cpbw.type = 129;
      send_cmd.params.ptr_set_plus_cpbw = &bat_params.plus_cpbw;
      ret = bat_send(client, &send_cmd);
      break;
    }
    case BAT_CMD_SET_PLUS_CSCS:
    {
      bat_params.plus_cscs.cs = BAT_CSCS_CS_UCS2;
      send_cmd.params.ptr_set_plus_cscs = &bat_params.plus_cscs;
      ret = bat_send(client, &send_cmd);
      break;
    }
    case BAT_CMD_SET_PLUS_CUSD:
    {
      U8 string[] = "00230031003100310023";
      bat_params.plus_cusd.n = (T_BAT_plus_cusd_n)1;
#ifdef _SIMULATION_
#pragma message( __TODO__"T_BAT_VAL_plus_cusd_n: two values are missing in (0,1)" )
#endif
      bat_params.plus_cusd.v_str = (U8)TRUE;
      strcpy((char *)&(bat_params.plus_cusd.str), (const char *)string);
      bat_params.plus_cusd.c_str = strlen((const char *)string);
      bat_params.plus_cusd.dcs = 15;
      send_cmd.params.ptr_set_plus_cusd = &bat_params.plus_cusd;
      ret = bat_send(client, &send_cmd);
      break;
    }
    case BAT_CMD_SET_PLUS_CCFC:
    {
      bat_params.plus_ccfc.reason = BAT_CCFC_REASON_CFU;
      bat_params.plus_ccfc.mode = BAT_CCFC_MODE_ERASURE;
      bat_params.plus_ccfc.v_number = (U8)FALSE;
      bat_params.plus_ccfc.type = (S16)BAT_PARAMETER_NOT_PRESENT;
      bat_params.plus_ccfc.bearer_class = BAT_CCFC_CLASS_VCEDATFAX; /* default  */
      bat_params.plus_ccfc.v_subaddr = (U8)FALSE;
      bat_params.plus_ccfc.satype = (S16)BAT_PARAMETER_NOT_PRESENT;
      bat_params.plus_ccfc.time = (T_BAT_plus_ccfc_time)BAT_CCFC_TIME_NOT_PRESENT;
      send_cmd.params.ptr_set_plus_ccfc = &bat_params.plus_ccfc;
      ret = bat_send(client, &send_cmd);
      break;
    }
    case BAT_CMD_SET_PLUS_CBST:
    {
      bat_params.plus_cbst.speed = BAT_CBST_SPD_9600_V110;
      bat_params.plus_cbst.name = BAT_CBST_NAM_CBST_NAM_ASYNCH;
      bat_params.plus_cbst.ce = BAT_CBST_CE_CBST_CE_NONTRANSPARENT;
      send_cmd.params.ptr_set_plus_cbst= &bat_params.plus_cbst;
      ret = bat_send(client, &send_cmd);
      break;
    }

    case BAT_CMD_SET_PLUS_COPS:
    {
      bat_params.plus_cops.mode = BAT_COPS_MODE_AUTO;
      bat_params.plus_cops.format = BAT_COPS_FORMAT_NOT_PRESENT;
      bat_params.plus_cops.v_oper = FALSE; 
      bat_params.plus_cops.c_oper = 0; 
      send_cmd.params.ptr_set_plus_cops = &bat_params.plus_cops;
      ret = bat_send(client, &send_cmd);
      break;
    }

    case BAT_CMD_SET_PERCENT_CNIV:
    {
      bat_params.percent_cniv.mode = BAT_CNIV_MODE_ON;
      send_cmd.params.ptr_set_percent_cniv= &bat_params.percent_cniv;
      ret = bat_send(client, &send_cmd);
      break;
    }

    default:
      TRACE_ASSERT(0);
      return;
  }

  TRACE_ASSERT(ret == BAT_OK);

}


char *app_handle_at_command(char *command)
{
  return app_handle_command(command, app_bat_atcmds_table);
}


/* EOF */
