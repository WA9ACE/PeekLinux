#include "typedefs.h"
#include "header.h"
#include "p_em.h"
#include "p_mncc.h"
#include "p_mmreg.h"
#include "p_mnsms.h"
#include "m_fac.h"
#include "p_8010_137_nas_include.h"
#include "p_mmi.h"
#include "aci_cmh.h"
#include "p_8010_152_ps_include.h"
#include "gaci_cmh.h"
#include "p_dl.h"
#include "hwat.h"

#include "gprs.h"

/* AT Call backs */

extern void gprs_set_signal_level(U32 level);
extern void gprs_set_location(U32 lac, U32 ci);

GLOBAL void rAT_BUSY (T_ACI_AT_CMD cmdId, SHORT cId)
{
	emo_printf("rAT_BUSY()");
  	
	// XXX cm_error_connect(cId);
}

GLOBAL void rAT_CONNECT (T_ACI_AT_CMD cmdId,
                          T_ACI_BS_SPEED speed,
                          SHORT cId)
{
	emo_printf("rAT_CONNECT()");

  	// XXX cm_data_connect(cId);
}

GLOBAL void rAT_NO_ANSWER (T_ACI_AT_CMD cmdId, SHORT cId)
{
  emo_printf("rAT_NO_ANSWER()");

  // XXX cm_error_connect(cId);
}

GLOBAL void rAT_NO_CARRIER( T_ACI_AT_CMD cmdId, SHORT cId )
{
  emo_printf("rAT_NO_CARRIER()");

  // XXX cm_error_connect(cId);
}

GLOBAL void rAT_OK ( T_ACI_AT_CMD cmdId )
{

  emo_printf("rAT_OK %d",cmdId);

  switch ((int)cmdId)
  {
    case AT_CMD_CFUN:
      sim_ok_cfun ();
      break;

    case AT_CMD_CPBW:
	  // Remove phone book write for now
      /*phb_signal(E_PHB_READY, 0); */
      break;

    case AT_CMD_COPS:
      // XXX nm_ok_cops ();
      break;

    case AT_CMD_NRG:
      // XXX nm_ok_cops ();
      break;

    case AT_CMD_PVRF:
      // XXX sim_ok_cpin ();
      break;

    case AT_CMD_CPIN:
      // XXX sim_ok_cpin ();
      break;

    case AT_CMD_A:
      // XXX cm_ok_connect();
      break;

    case AT_CMD_CMGD:
	  emo_printf("ACI has deleted SMS!!");
      // XXX sms_ok_delete ();
      break;

    case AT_CMD_CMGR:
      // XXX sms_ok_change ();
      break;

    case AT_CMD_CHUP:
      // XXX cm_ok_disconnect(CMD_SRC_LCL, 0);
      break;

    case AT_CMD_H:
      // XXX cm_ok_disconnect(CMD_SRC_LCL, 0);
      break;

    case AT_CMD_ABRT:
      // XXX cm_ok_disconnect(CMD_SRC_LCL, 0);
      break;

    case AT_CMD_D:
      // XXX cm_ok_connect();
	  break;

    case AT_CMD_CPWD:
      // XXX sim_ok_cpinc();
      break;

    case AT_CMD_CHLD:
      //cm_result_cmd(CMD_SRC_LCL, Present);
      break;

    case AT_CMD_CCFC:
      break;

    case AT_CMD_CCWA:
      break;

    case KSD_CMD_CB:
    case KSD_CMD_CF:
    case KSD_CMD_CL:
    case KSD_CMD_CW:
    case KSD_CMD_PWD:
    case KSD_CMD_UBLK:
      // XXX ss_ok_string(cmdId);
      break;

    case AT_CMD_CMGL:
        // XXX sms_ok_CMGL();
       break;
    case AT_CMD_CPMS:
    case AT_CMD_CMGW:
    case AT_CMD_CMSS:
    case AT_CMD_CMGS:
      break;

    case AT_CMD_CPOL:
      // XXX nm_ok_pref_plmn();
      break;

    case AT_CMD_CSAS:
      // XXX sms_ok_csas();
      break;

  case AT_CMD_CGATT:
  case AT_CMD_CGDCONT:
  case AT_CMD_CGACT:
  case AT_CMD_CGQREQ:
  case AT_CMD_CGQMIN:
  case AT_CMD_CGDATA:
  case AT_CMD_CGPADDR:
  case AT_CMD_CGAUTO:
  case AT_CMD_CGANS:
  case AT_CMD_CGCLASS:
  case AT_CMD_CGEREP:
  case AT_CMD_CGREG:
  case AT_CMD_CGSMS:
      gprs_ok(cmdId);
      break;

  case AT_CMD_BAND:
    // XXX nm_ok_band();
    break;
	
 default:
    break;
  }
}

GLOBAL void rAT_PercentALS(T_ACI_ALS_MOD ALSmode) {
	emo_printf("rAT_PercentALS()");
}

void rAT_PercentBC (BYTE val)
{
    emo_printf("rAT_PercentBC()");
}

void rAT_PercentCCBS( T_ACI_CCBS_IND  indct,
                        T_ACI_CCBS_STAT status,
                        T_ACI_CCBS_SET *setting )
{
    emo_printf("rAT_PercentCCBS()");
}

GLOBAL void rAT_PercentCCCN ( T_ACI_FAC_DIR tDirection,
                              SHORT cId,
                              T_MNCC_fac_inf *acFie )
{
	emo_printf("rAT_PercentCCCN ()");
}

GLOBAL void rAT_PercentCGEV (T_CGEREP_EVENT event, T_CGEREP_EVENT_REP_PARAM *param )
{
	emo_printf("rAT_PercentCGEV()");
	emo_printf("GPRS eventrep: %d", (short)event);
}

GLOBAL void rAT_PercentCGREG ( T_P_CGREG_STAT stat, USHORT lac, USHORT ci, BOOL bActiveContext )
{
   
   	//T_HW_GPRS_CELL_STRUCT_P cell;
 
	emo_printf("rAT_PercentCGREG()");
    emo_printf("GPRS regState: %d, lac %X, ci %X", (short)stat, lac, ci);
    gprs_set_location(lac, ci);
	//XXX we should likely signal an event here instead of setting the location from here
    //cell.ci = ci;
    //gprs_signal(E_HW_GPRS_R_REG, &cell);
 
}

void rAT_PercentCLOG(T_ACI_CLOG *cmd)
{
	emo_printf("rAT_PercentCLOG()");
}

GLOBAL void rAT_PercentCMGR  ( T_ACI_CMGL_SM*  sm,
                               T_ACI_CMGR_CBM* cbm )
{
	emo_printf("rAT_PercentCMGR()");
}
 
GLOBAL void rAT_PercentCMGL ( T_ACI_CMGL_SM * smLst )
{
	emo_printf("rAT_PercentCMGL()");
}

GLOBAL void rAT_PercentCMGRS ( T_ACI_CMGRS_MODE  mode,
                                UBYTE             tp_mr,
                                UBYTE             resend_count,
                                UBYTE             max_retrans )
{
	emo_printf("rAT_PercentCMGRS ()");
}

GLOBAL void rAT_PercentCNAP  ( T_callingName *NameId,T_ACI_CNAP_STATUS status)
{

	emo_printf("rAT_PercentCNAP()");
}

GLOBAL void rAT_PercentCNIV (T_MMR_INFO_IND *mmr_info_ind)
{
	emo_printf("rAT_PercentCNIV()");
}

GLOBAL void rAT_PlusCLCK  ( T_ACI_CLSSTAT * clsStaLst) {}
GLOBAL void rAT_PlusCIMI  (CHAR *imsi) {}
GLOBAL void rAT_PlusCCFC  (T_ACI_CCFC_SET* setting) {}
GLOBAL void rAT_PlusCLIR  (T_ACI_CLIR_MOD  mode, T_ACI_CLIR_STAT stat) {}
GLOBAL void rAT_PercentCOLR ( T_ACI_COLR_STAT stat) {}


void rAT_sms_ready(void)
{
    emo_printf("rAT_sms_ready()");

	/*
    if (phbReady)
    {
      f_update = HW_SMS_NEW;
      sms_read_msg_info();
      smsReady = 0;
      phbReady = 0;
    }
    else
      smsReady = 1;
	*/
}

void rAT_phb_status (T_ACI_PB_STAT status)
{
	emo_printf("rAT_phb_status()");

}

void rAT_changedQOS ( SHORT cid, T_PS_qos *qos )
{
	emo_printf("rAT_changedQOS()");
}

GLOBAL void rAT_Z ( void ){}

void rAT_PercentCPI(SHORT cId,
                     T_ACI_CPI_MSG msgType,
                     T_ACI_CPI_IBT ibt,
                     T_ACI_CPI_TCH tch,
                     USHORT        cause )
{
	emo_printf("rAT_PercentCPI()");
}

void rAT_PercentCPRI (UBYTE gsm_ciph, UBYTE gprs_ciph)
{
	emo_printf("rAT_PercentCPRI()");
    //call here the function from MMI
    //mmi_cprs_cb(gsm_ciph, gprs_ciph);
}

GLOBAL void rAT_PercentCPRSM ( T_ACI_CPRSM_MOD mode )
{
	emo_printf("rAT_PercentCPRSM ()");
}

GLOBAL void rAT_SignalSMS ( UBYTE state )
{
  // XXX
  emo_printf("rAT_SignalSMS()");
#if 0
  switch (state)
  {
        case SMS_STATE_INITIALISING: initialising_flag = TRUE; break;
        case SMS_STATE_READY: initialising_flag = FALSE; break;
  }
#endif
}

GLOBAL void rAT_PercentCREG  ( T_ACI_CREG_STAT       status,
                               USHORT                lac,
                               USHORT                cid,
                               T_ACI_P_CREG_GPRS_IND gprs_ind)
{
	emo_printf("rAT_PercentCREG()");
}

#define MMI_MIN_RXLEV_FAULT 99
GLOBAL void rAT_PercentCSQ  (UBYTE rssi, UBYTE ber, UBYTE actlevel, UBYTE min_access_level)
{

	static UBYTE prelevel=0;

	emo_printf("rAT_PercentCSQ()");

    if((min_access_level NEQ MMI_MIN_RXLEV_FAULT) AND (rssi < min_access_level ))
    {
    	prelevel = 0;
		gprs_set_signal_level(0);
    } else {
        if( prelevel != actlevel )
        {
			gprs_set_signal_level(actlevel);
            prelevel = actlevel;
        }
	}
}

void rAT_PercentSQ (BYTE val)
{
	emo_printf("rAT_PercentSQ()");
}

GLOBAL void rAT_PercentCSSN ( T_ACI_FAC_DIR tDirection,
                              T_ACI_FAC_TRANS_TYPE tType,
                              T_MNCC_fac_inf *acFie )
{
	emo_printf("rAT_PercentCSSN ()");
}

GLOBAL void rAT_PercentCSTAT ( T_ACI_STATE_MSG msgType )
{
  T_HW_NETWORK_STRUCT plmn_eons_data;
  
  emo_printf("rAT_PercentCSTAT ()");
  memset(&plmn_eons_data, 0, sizeof(T_HW_NETWORK_STRUCT) );
  if( (msgType.entityId EQ STATE_MSG_EONS ) AND 
        (msgType.entityState EQ ENTITY_STATUS_Ready) )
  {
	emo_printf("rAT_PercentCSTAT() - nm_get_COPN");
    nm_get_COPN(&plmn_eons_data);  

    //send E_NM_OPN event 
    nm_get_network_data(&plmn_eons_data);
  }

}

void rAT_PlusCLIP(T_ACI_CLIP_STAT   stat,
                  CHAR            * number,
                  T_ACI_TOA       * type,
                  U8                validity,
                  CHAR            * subaddr,
                  T_ACI_TOS       * satype,
                  T_ACI_PB_TEXT   * alpha)
{

	emo_printf("rAT_PlusCLIP()");
}

void rAT_PlusCUSD(T_ACI_CUSD_MOD m, T_ACI_USSD_DATA *ussd, SHORT dcs)
{
	emo_printf("rAT_PlusCUSD()");

}

GLOBAL void rAT_PlusCTZV ( S32 timezone )
{
	emo_printf("rAT_PlusCTZV()");
}

void rAT_PlusCSSU  ( T_ACI_CSSU_CODE code,
                      SHORT           index,
                      CHAR           *number,
                      T_ACI_TOA      *type,
                      CHAR           *subaddr,
                      T_ACI_TOS      *satype)
{
	emo_printf("rAT_PlusCSSU()");
}

void rAT_PlusCSSI(T_ACI_CSSI_CODE code, SHORT index)
{
	emo_printf("rAT_PlusCSSI()");
}

GLOBAL void rAT_PlusCME   ( T_ACI_AT_CMD cmdId, T_ACI_CME_ERR err )
{
  emo_printf("rAT_PlusCME(cmdId %d, err %d)",cmdId, err);

  switch ((int)cmdId)
  {
  case AT_CMD_CPBW:
		// FIXME
        //phb_signal(E_PHB_ERROR, 0);
        break;  
  case AT_CMD_COPS:
    // XXX nm_error_cops ((T_ACI_CME_ERR)err);
    break;

  case AT_CMD_NRG:
		//XXX fix
	/*
        TRACE_EVENT_P1("CME ERROR VALUE in NRG: %d",err);
            if( (nm_get_mode() == NM_MANUAL) && // this is mfw also
                        ((err == CME_ERR_NoServ) ||(err == CME_ERR_LimServ) ||(err ==CME_ERR_Unknown)))
                {
                                nm_get_plmn_list_during_bootup();
                }
            else
                {
                        nm_error_cops ((T_ACI_CME_ERR)err);
                }
	*/
    break;
  case AT_CMD_CFUN:
    // XXX sim_error_cfun ((T_ACI_CME_ERR)err);
    break;

  case AT_CMD_CPIN:
    // XXX sim_error_cpin ((T_ACI_CME_ERR)err);
    break;

  case AT_CMD_PVRF:
    // XXX sim_error_cpin ((T_ACI_CME_ERR)err);
    break;

  case AT_CMD_CHLD:
// XXX
//    cm_result_cmd(CMD_SRC_LCL, NotPresent);
    break;

  /* We dont need password/pin stuff */
  case AT_CMD_CLCK:
  case AT_CMD_CPWD: 
  break;

  case KSD_CMD_CB:
  case KSD_CMD_CF:
  case KSD_CMD_CL:
  case KSD_CMD_CW:
  case KSD_CMD_PWD:
  case KSD_CMD_UBLK:
    // XXX ss_error_string(cmdId, err);
    break;

  case AT_CMD_CPUC:
  case AT_CMD_CAMM:
  case AT_CMD_CACM:
    // XXX cm_error_aoc(cmdId, err);
    break;

  /* We don't need phonebook support */
  case AT_CMD_CNUM:
    //phb_read_upn_eeprom();
    break;

  case AT_CMD_CUSD:
  case KSD_CMD_USSD:
    // XXX ss_error_ussd(cmdId, err);
    break;

  case AT_CMD_CPOL:
    //nm_error_pref_plmn();
    break;

  case AT_CMD_VTS:
    break;

  case AT_CMD_D:
    // XXX cm_error_dial(err);
    break;

  case AT_CMD_CLAN:
    // XXX sim_read_lp_cnf(SIM_CAUSE_EF_INVALID,NULL);
    break;

  case AT_CMD_CGATT:
  case AT_CMD_CGDCONT:
  case AT_CMD_CGACT:
  case AT_CMD_CGQREQ:
  case AT_CMD_CGQMIN:
  case AT_CMD_CGDATA:
  case AT_CMD_CGPADDR:
  case AT_CMD_CGAUTO:
  case AT_CMD_CGANS:
  case AT_CMD_CGCLASS:
  case AT_CMD_CGEREP:
  case AT_CMD_CGREG:
  case AT_CMD_CGSMS:
      // XXX gprs_error(cmdId, err);
      break;

  case AT_CMD_CCBS:
    break;

  case AT_CMD_H:
    // XXX cm_error_disconnect();
    break;
  case AT_CMD_CTFR:
    break;

  case AT_CMD_SATE:
  	break;
  }
}

void rAT_PlusCMGC(UBYTE mr)
{
  emo_printf("rAT_PlusCMGC()");

  //XXX: fix for sms
  //sms_check_mt_update();
  //sms_signal(E_SMS_CMD_AVAIL, &mr);
}

GLOBAL void rAT_PlusCMGD ()
{
  emo_printf("rAT_PlusCMGD");
  //concSMS_clearIncompleteMsg();
  //sms_ok_delete();
}

GLOBAL void rAT_PlusCSMS (T_ACI_CSMS_SERV service,
                          T_ACI_CSMS_SUPP mt,
                          T_ACI_CSMS_SUPP mo,
                          T_ACI_CSMS_SUPP bm) {}

GLOBAL void rAT_PlusCSIM  ( SHORT           rspLen,
                             UBYTE          *rsp    ) {}

void rAT_PercentMMITEST(char *param)
{
	emo_printf("rAT_PercentMMITEST");
}

void rAT_PercentKSIR ( T_ACI_KSIR *ksStat)
{
    emo_printf("rAT_PercentKSIR()");
}

GLOBAL void rAT_PercentEMETS ( UBYTE entity ) { }
GLOBAL void rAT_PercentEM ( /*UBYTE srcId,*/ T_EM_VAL *val_tmp ) {} 
GLOBAL void rAT_PercentEMET ( /*UBYTE srcId,*/ T_EM_VAL val_tmp ) {}

GLOBAL void rAT_PercentRDLB ( T_ACI_CC_RDL_BLACKL_STATE state )
{
	emo_printf("rAT_PercentRDLB ()");
}

GLOBAL void rAT_PercentRDL ( T_ACI_CC_REDIAL_STATE state )
{
	emo_printf("rAT_PercentRDL ()");
}

void rAT_PercentRLOG(T_ACI_RLOG *rslt)
{
	emo_printf("rAT_PercentRLOG ()");
}

GLOBAL void rAT_PlusCRSM  ( SHORT           sw1,
                            SHORT           sw2,
                            SHORT           rspLen,
                            UBYTE          *rsp    ) {}
GLOBAL void rAT_PlusCRING_OFF ( SHORT cId )
{
	emo_printf("rAT_PlusCRING_OFF ()");
}

void rAT_PlusCRING(T_ACI_CRING_MOD mode,
                   T_ACI_CRING_SERV_TYP type1,
                   T_ACI_CRING_SERV_TYP type2)
{
	emo_printf("rAT_PlusCRING ()");
}

GLOBAL void rAT_PlusCLAE  (T_ACI_LAN_SUP  *CLang )
{
	emo_printf("rAT_PlusCLAE()");
}

GLOBAL void rAT_PlusCLAN  (T_ACI_LAN_SUP  *CLang )
{
	emo_printf("rAT_PlusCLAN()");
}

void rAT_PercentCTV()
{
	emo_printf("rAT_PercentCTV()");
}

void rAT_PercentCTYI(T_ACI_CTTY_NEG neg, T_ACI_CTTY_TRX trx)
{
	emo_printf("rAT_PercentCTYI()");
}

GLOBAL void rAT_PercentCTZV (T_MMR_INFO_IND *mmr_info_ind, S32 timezone )
{
	emo_printf("rAT_PercentCTZV()");
}

void rAT_PercentSATA  ( SHORT           cId,
                               LONG            rdlTimeout_ms,
                               T_ACI_SATA_ADD *addParm)
{
	emo_printf("rAT_PercentSATA()");
}

void rAT_PercentSATE (S16 len, U8 *res)
{
	emo_printf("rAT_PercentSATE()");
}

void rAT_PercentSATI (S16 len, U8 *sc)
{
	emo_printf("rAT_PercentSATI()");
}

void rAT_PercentSATN (S16 len, U8 *sc, T_ACI_SATN_CNTRL_TYPE  cntrl_type)
{
	emo_printf("rAT_PercentSATN()");
}

void rAT_PercentSIMEF (T_SIM_FILE_UPDATE_IND *sim_file_update_ind)
{
	emo_printf("rAT_PercentSIMEF()");
}

void rAT_PercentSIMINS( T_ACI_CME_ERR err )
{
	emo_printf("rAT_percentSIMINS()");
}

void rAT_PercentSIMREM( T_ACI_SIMREM_TYPE srType )
{
	emo_printf("rAT_PercentSIMREM()");
}

GLOBAL void rAT_PercentSNCNT ( UBYTE c_id,
                               ULONG octets_uplink,
                               ULONG octets_downlink,
                               ULONG packets_uplink,
                               ULONG packets_downlink )
{
	emo_printf("rAT_PercentSNCNT()");
}

void rAT_PlusCBM(SHORT           sn,
                 SHORT           mid,
                 SHORT           dcs,
                 UBYTE           page,
                 UBYTE           pages,
                 T_ACI_CBM_DATA* data)
{
	emo_printf("rAT_PlusCBM()");
}

void rAT_PlusCCCM(LONG *ccm)
{
	emo_printf("rAT_PlusCCCM()");
}

GLOBAL void rAT_PlusCCWA ( T_ACI_CLSSTAT * clsStatLst,
                           CHAR          * number,
                           T_ACI_TOA     * type,
                           U8              validity,
                           T_ACI_CLASS     call_class,
                           T_ACI_PB_TEXT * alpha)
{
	emo_printf("rAT_PlusCCWA()");
}

GLOBAL void rAT_PlusCCWV ( T_ACI_CCWV_CHRG charge ) {}

GLOBAL void rAT_PlusCDIP( CHAR   * number,
                             T_ACI_TOA       * type,
                             CHAR            * subaddr,
                             T_ACI_TOS       * satype)
{
	emo_printf("rAT_PlusCDIP()");
}


void rAT_PlusCDS(T_ACI_CDS_SM *st)
{
	emo_printf("rAT_PlusCDS()");
}

void rAT_PlusCGACT ( SHORT link_id )
{
	emo_printf("rAT_PlusCGACT()");
}

void rAT_PlusCGANS ( SHORT link_id )
{
	emo_printf("rAT_PlusCGANS()");
}

void rAT_PlusCGDATA ( SHORT link_id )
{
	emo_printf("rAT_PlusCGDATA()");
}

void rAT_PlusCGEREP ( T_CGEREP_EVENT            event, 
                      T_CGEREP_EVENT_REP_PARAM *param )
{
  emo_printf("rAT_PlusCGEREP()");
  emo_printf("GPRS eventrep: %d", (short)event);

}

void rAT_PlusCGREG  ( T_CGREG_STAT stat, 
                      USHORT       lac, 
                      USHORT       ci )
{ 
  //T_HW_GPRS_CELL_STRUCT cell;

  emo_printf("rAT_PlusCGREG()");
  emo_printf("GPRS regState: %d, lac %X, ci %X", (short)stat, lac, ci);
  /* XXX:
   * send signal for changed registry state to event manager
   */

  /*
  cell.stat = stat;
  cell.lac = lac;
  cell.ci = ci;
  gprs_signal(E_HW_GPRS_R_REG, &cell);
  */
}

GLOBAL void rAT_PlusCIEV( T_ACI_MM_CIND_VAL_TYPE sCindValues,
                          T_ACI_MM_CMER_VAL_TYPE sCmerSettings )
{
	emo_printf("rAT_PlusCIEV()");
}

void rAT_PlusCMGL ( T_ACI_CMGL_SM * smLst )
{
    //XXX: Fix for SMS msg list 
	emo_printf("rAT_PlusCMGL()");
}

void rAT_PlusCMGR(T_ACI_CMGL_SM  * sm, T_ACI_CMGR_CBM * cbm)
{
    //XXX: Fix for SMS read msg
	emo_printf("rAT_PlusCMGR()");
}

void rAT_PlusCMGS(UBYTE mr, UBYTE numSeg)
{
	//XXX: Fix for SMS send confirm
	emo_printf("rAT_PlusCMGS()");
}

void rAT_PlusCMGW(UBYTE index, UBYTE numSeg, UBYTE mem)
{
	//XXX: Fix for SMS save sms
	emo_printf("rAT_PlusCMGW()");
}

void rAT_PlusCPOL(SHORT startIdx, SHORT lastIdx,
                  T_ACI_CPOL_OPDESC *operLst,
                  SHORT              usdNtry)
{
	emo_printf("rAT_PlusCPOL()");
}

void rAT_PlusCMSS (UBYTE mr, UBYTE numSeg)
{
	emo_printf("rAT_PlusCMSS()");

}

void rAT_PlusCMT(T_ACI_CMGL_SM*  sm)
{
	emo_printf("rAT_PlusCMT()");
}

void rAT_PlusCMTI(T_ACI_SMS_STOR mem, UBYTE index, T_ACI_CMGL_SM* sm)
{
	emo_printf("rAT_PlusCMTI()");
}

void rAT_PlusCNUM  (T_ACI_CNUM_MSISDN *msisdn, UBYTE num) { }

GLOBAL void rAT_PlusCOLP  ( T_ACI_COLP_STAT stat,
                            CHAR* number,
                            T_ACI_TOA* type,
                            CHAR* subaddr,
                            T_ACI_TOS* satype,
                            T_ACI_PB_TEXT* alpha)
{
	emo_printf("rAT_PlusCOLP");
}

void rAT_PercentCOPS (S16 last_ind, T_ACI_COPS_OPDESC *operLst)
{
	emo_printf("rAT_PercentCOPS()");
}

GLOBAL void rAT_PlusCPIN (T_ACI_CPIN_RSLT code) 
{
	emo_printf("rAT_PlusCPIN()");
}

GLOBAL void rAT_PlusCPMS (T_ACI_SMS_STOR_OCC * mem1,
                          T_ACI_SMS_STOR_OCC * mem2,
                          T_ACI_SMS_STOR_OCC * mem3)
{
	emo_printf("rAT_PlusCPMS");
}

GLOBAL void rAT_PercentDRV( T_ACI_DRV_DEV device,
                            T_ACI_DRV_FCT function,
                            UBYTE         val1,
                            UBYTE         val2)
{
    emo_printf("rAT_PercentDRV()");
    //drvKeyUpDown(val2,val1); //ES!! val2 korrekt ?
}

void rAT_PlusCMS (T_ACI_AT_CMD cmdId, T_ACI_CMS_ERR err, T_EXT_CMS_ERROR *ce)
{
	emo_printf("rAT_PlusCMS()");
	emo_printf("Error: %d",cmdId);
}

void rAT_PlusCOPS (S16 last_ind, T_ACI_COPS_OPDESC *operLst)
{
	emo_printf("rAT_PlusCOPS()");
	//nm_Plus_Percent_COPS(last_ind, operLst);
}

UBYTE getCLIRState(void) { return 0;}
