/* Network/Time Management functions */
#include "typedefs.h"
#include "header.h"
#include "vsi.h"
#include "pconst.cdg"
#include "p_sim.h"
#include "acie.h"
#include "hwsim.h"
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
#include "psa.h"
#include "cus_aci.h"
#include "m_sat.h"
#include "psa_sim.h"
#include "p_gmmreg.h"
#include "hwat.h"
#include "rtc.h"

static T_RTC_TIME current_time;
static T_RTC_DATE current_date;

static char time_string[15];
static char date_string[15];

/* defines */
#define E_NM_OPN 0x4000
#define NM_AUTOMATIC    0       /* automatic mode set */
#define NM_MANUAL       1       /* manual mode is set */

static int pSlot; 
static UBYTE mfwPplmnReq;
static BOOL sat_update = FALSE;
static U8 last_selected_mode = NOT_PRESENT_8BIT;
static U8 reg_stat;
static U8 reg_flag;

U8 hw_IMSI [LONG_NAME];

typedef struct
{
  USHORT nreg_cause;             /* nreg cause from MMR_NREG_IND          */
  UBYTE  forbidden_id[30];       /* forbidden indicator from MMR_PLMN_IND */
  UBYTE  fieldstrength [30];     /* fieldstrength from MMR_PLMN_IND       */
} T_NM_ACI_DATA;

T_NM_ACI_DATA  nm_aci_data;

typedef struct /* data for timer zone */
{
  UBYTE year;
  UBYTE month;
  UBYTE day;
  UBYTE hour;
  UBYTE minute;
  UBYTE second;
  UBYTE timezone;
} T_HW_TIME_IND;

static T_HW_SP_NAME service_provider_name;
static U8 display_condition;
static T_HW_TIME_IND tim_ind;

GLOBAL void sAT_PlusCOPSE(UBYTE *oper, UBYTE format,
                          T_HW_LNAME *long_name,
                          T_HW_SNAME *short_name,
                          UBYTE *numeric_name);

int nm_reg_status (void)
{
    emo_printf("nm_reg_status()");

    return reg_stat;
}

int nm_reregistration(void) {
     emo_printf("------------ NM_REREGISTRATION ----------------\n");
     return 0;
}

int nm_registration (U8 mode, U8 freq_bands, U8 limited)
{
    U8 sim_stat;

    emo_printf("nm_registration()");

    sAT_Abort(CMD_SRC_LCL, AT_CMD_NRG);

    sim_stat = sim_status_check();
    reg_flag = 1;

    if (limited)
    {
        if (sAT_PercentNRG(CMD_SRC_LCL,
                           NRG_RGMD_Auto,
                           NRG_SVMD_Limited,
                           NRG_FRMT_Numeric,
                           NULL) != AT_EXCT)
            emo_printf("Limited Service error: SAT_PercentNRG");
        return sim_stat;
    }

    if (!limited && sim_stat == IMEI_NOT_VALID)
      return IMEI_NOT_VALID;

    if (!limited && sim_stat != SIM_ACTIVE)
      return SIM_NOT_ACTIVE;


    if (mode == NM_MANUAL)
    {

        //   Description :  It is supposed that the sample register itself automatically (in Manual Mode)
        //                  to the last manual selected HPLMN cell before the power cycle, and this does not happen;
         //                 instead this, the sample shows up the available networks list
         //  Fix         :  call to nm_avail_plmn_list() is changed to call to sAT_PercentNRG()
        if (sAT_PercentNRG(CMD_SRC_LCL,
                           NRG_RGMD_Manual,
                           NRG_SVMD_Full,
                           NRG_FRMT_Numeric,
                           NULL) != AT_EXCT)
        {
            emo_printf("sAT_PercentNRG error");
        }
    }
    else
    {
        if (sAT_PercentNRG(CMD_SRC_LCL,
                           NRG_RGMD_Auto,
                           NRG_SVMD_Full,
                           NRG_FRMT_Numeric,
                           NULL) != AT_EXCT)
        {
            emo_printf("sAT_PercentNRG error");
        }

    }
    return SIM_ACTIVE;
}

int nm_decodeSourceId( UBYTE sourceId)
{
   emo_printf("nm_decodeSourceId()");
   switch (sourceId)
   {   
        case Read_ROM_TABLE:            return NM_ROM;
        case Read_EONS:                 return NM_EONS;
        case Read_CPHS:                 return NM_CPHS;
        case Read_INVALID:                      
        default:                        return NM_NONE;
   }
}

void nm_update_plmn_data( T_HW_NETWORK_STRUCT *plmn_ident, 
                                                            T_ACI_OPER_NTRY oper_ntry)
{
     emo_printf("nm_update_plmn_data()");
 
  	 plmn_ident->opn_read_from = (T_HW_NM_OPN)nm_decodeSourceId(oper_ntry.source);


      //update plmn_ident with EONS
      memset(plmn_ident->network_long.data, 0, LONG_NAME);
      memset(plmn_ident->network_short.data, 0, SHORT_NAME);
      memcpy( plmn_ident->network_long.data, 
                      oper_ntry.longName, 
                      strlen(oper_ntry.longName) );
    
      memcpy( plmn_ident->network_short.data, 
                      oper_ntry.shrtName, 
                    strlen(oper_ntry.shrtName));

      plmn_ident->network_long.len = strlen((const char*)(plmn_ident->network_long.data));
      plmn_ident->network_short.len = strlen((const char*)(plmn_ident->network_short.data)); 
}

void nm_mmi_parameters (T_imsi_field *imsi)
{
    void simDecodeIMSI (U8 *ci, U8 li, U8 *di);

    emo_printf("nm_mmi_parameters()");

    simDecodeIMSI(imsi->field,imsi->c_field,hw_IMSI);
}

static int nm_sign_exec (int event, void * para)
{
   emo_printf("nm_sign_exec() event: %d", event);
	/*
   switch (event)
   {
       case E_NM_FULL_SERVICE:
       case E_NM_OPN:
           memcpy(&nm_data->para.ident,para, sizeof(T_MFW_NETWORK_STRUCT));
           break;
       case E_NM_PLMN_LIST:
           memcpy(&nm_data->para.plmns,para, sizeof(T_MFW_PLMN_LIST));
           break;
       case E_NM_PREF_PLMN_LIST:
           memcpy(&nm_data->para.pplmns,para, sizeof(T_MFW_PREF_PLMN_LIST));
           break;
       case E_NM_PPLMN_MEM:
           memcpy(&nm_data->para.pplmn_mem,para, sizeof(T_MFW_PPLMN_MEM));
           break;
       case E_NM_CELL_SELECT:
           memcpy(&nm_data->para.cell_info,para, sizeof(T_MFW_CELL_STRUCT));
           break;
       case E_MFW_NET_IND:
           memcpy(&nm_data->para.net_info,para, sizeof(T_MFW_NET_IND));
           break;
       case E_MFW_TIME_IND:
           memcpy(&nm_data->para.tim_info,para, sizeof(T_RTC_TIME_IND));
           break;
     default:	
   	 	return FALSE;
    }
	*/
	/* XXX: execute handler */
	
    return TRUE;
}

static void nm_signal (int event, void *para)
{
    emo_printf("nm_signal() event[%d]", event);
    //nm_sign_exec(event,para);
}

void nm_get_network_data(T_HW_NETWORK_STRUCT *plmn_netw_data)
{
        emo_printf("nm_get_network_data()");
        
        //send E_NM_OPN event 
        nm_signal(E_NM_OPN, plmn_netw_data); 
}

void nm_nitz_info_ind(T_MMR_INFO_IND * mmr_info_ind)
{
  UBYTE flag;

  emo_printf("nm_nitz_info_ind() ");

  flag = 0;
  memset(&tim_ind, 0, sizeof(T_HW_TIME_IND));
  /* copy time */
  if (mmr_info_ind->ntz.v_tz EQ TRUE AND
      mmr_info_ind->time.v_time EQ TRUE )
  {
    flag = 1;
    tim_ind.timezone = mmr_info_ind->ntz.tz;
    tim_ind.year     = mmr_info_ind->time.year;
    tim_ind.month    = mmr_info_ind->time.month;
    tim_ind.day      = mmr_info_ind->time.day;
    tim_ind.hour     = mmr_info_ind->time.hour;
    tim_ind.minute   = mmr_info_ind->time.minute;
    tim_ind.second   = mmr_info_ind->time.second;
  }
  else if (mmr_info_ind->ntz.v_tz EQ TRUE AND
           mmr_info_ind->time.v_time NEQ TRUE )
  {
    flag = 1;
    tim_ind.timezone = mmr_info_ind->ntz.tz;
  }

  ///XXX: fix to update time
  //if (flag)
  //  nm_signal(E_MFW_TIME_IND, &tim_ind);
}

void nm_get_COPN( T_HW_NETWORK_STRUCT *plmn_ident )
{
  T_ACI_COPS_MOD mode;
  T_ACI_COPS_FRMT format;
  T_ACI_OPER_NTRY oper_ntry;
  char opername[LONG_NAME];

  emo_printf("nm_get_COPN()");

  memset (&oper_ntry, 0, sizeof( T_ACI_OPER_NTRY ));
  //Check where MS is registered
  if( qAT_PlusCOPS ( CMD_SRC_LCL, &mode,
                                        &format, (char *)opername ) EQ AT_CMPL )
  {
    //Request info about operator name
    if(qAT_PercentCOPN(CMD_SRC_LCL, 
                format, 
                (char *)opername, 
                &oper_ntry) EQ AT_CMPL )
    {     
        nm_update_plmn_data(plmn_ident, oper_ntry);       
    }
  }
}

GLOBAL BOOL nma_response_cb (ULONG opc, void * data)
{
  T_MMR_NREG_IND * nreg_ind;
  T_MMR_INFO_IND  mmr_nitz_ind;
  T_GMMREG_INFO_IND *gmmreg_info_ind;

  //emo_printf("nma_response_cb()");

  switch (opc)
  {
    case MMR_PLMN_IND:
      /*
       * available PLMN list, store fieldstrength
       * and forbidden indicator (not yet included)
       */
      return FALSE;   /* normal processing of ACI   */

    case MMR_NREG_IND:
      /*
       * store the nreg cause
       */
      nreg_ind = (T_MMR_NREG_IND *)data;
      nm_aci_data.nreg_cause = nreg_ind->cause;
      return FALSE;   /* normal processing of ACI   */

    case MMR_INFO_IND:
      emo_printf("  case MMREG_INFO_IND: ");
      /*
       * send this NITZ information to MMI
       */
       nm_nitz_info_ind((T_MMR_INFO_IND *)data);
      return FALSE;   /* normal processing of ACI   */

    case GMMREG_INFO_IND:
      emo_printf("  case GMMREG_INFO_IND: ");

      gmmreg_info_ind= (T_GMMREG_INFO_IND *)data;
      memcpy (&(mmr_nitz_ind.plmn), &(gmmreg_info_ind->plmn), sizeof(T_plmn));

      memcpy (&(mmr_nitz_ind.full_name),&(gmmreg_info_ind->full_net_name_gmm), sizeof (T_full_name));
      memcpy (&(mmr_nitz_ind.short_name),&(gmmreg_info_ind->short_net_name_gmm), sizeof (T_short_name));

      memcpy (&(mmr_nitz_ind.ntz), &(gmmreg_info_ind->net_time_zone),sizeof (T_ntz));
      memcpy (&(mmr_nitz_ind.time), &(gmmreg_info_ind->net_time), sizeof(T_time));

      nm_nitz_info_ind(&mmr_nitz_ind);
      gprs_attach(1);
      return FALSE;   /* normal processing of ACI   */
  }

  return FALSE;       /* not processed by extension */
}

int nm_init (void)
{

    emo_printf("nm_init()");

	/*
    if (last_selected_mode == NOT_PRESENT_8BIT)
    {
            last_selected_mode = FFS_flashData.PLMN_selection_mode;
    }
	*/
    /* install prim handler     */
    pSlot = aci_create(nma_response_cb,NULL);

    reg_flag = 0;
    mfwPplmnReq = 0;
    reg_stat = NOT_PRESENT_8BIT;
    sat_update  = FALSE;

    return 0;
}

void nm_spn_cnf (USHORT error, T_EF_SPN *efspn)
{
	emo_printf("nm_spn_cnf()");
}

/*  Change registration status */
void nm_search_reg_status(void)
{
  T_ACI_NRG_RGMD regMode;
  T_ACI_NRG_SVMD srvMode;
  T_ACI_NRG_FRMT oprFrmt;
  T_ACI_NRG_SVMD srvStat;
  U8 oper[LONG_NAME];

  qAT_PercentNRG(CMD_SRC_LCL,
                 &regMode,
                 &srvMode,
                 &oprFrmt,
                 &srvStat,
                 (char *)oper);

  if (srvStat == NRG_SVMD_Limited)
  {
    reg_stat = HW_LIMITED_SERVICE;
    nm_signal(E_NM_LIMITED_SERVICE,NULL);
  }
  if (srvStat == NRG_SVMD_NoSrv)
  {
    reg_stat = HW_NO_SERVICE;
    nm_signal(E_NM_NO_SERVICE,NULL);
  }
}

void rAT_PlusCREG (T_ACI_CREG_STAT stat,
                   USHORT          lac,
                   USHORT          cid)
{
    T_HW_NETWORK_STRUCT plmn_ident;
    T_HW_CELL_STRUCT cell;
    T_ACI_NRG_RGMD regMode;
    T_ACI_NRG_SVMD srvMode;
    T_ACI_NRG_FRMT oprFrmt;
    T_ACI_NRG_SVMD srvStat;
    USHORT cause;
    U8 oper[LONG_NAME];

    emo_printf("rAT_PlusCREG()");

    switch (stat)
    {
        case CREG_STAT_Reg:
        case CREG_STAT_Roam:
            reg_stat = HW_FULL_SERVICE;
            qAT_PercentNRG(CMD_SRC_LCL,
                           &regMode,
                           &srvMode,
                           &oprFrmt,
                           &srvStat,
                           (char *)oper);
            sAT_PlusCOPSE(oper,(U8) oprFrmt,&plmn_ident.network_long,
                                      &plmn_ident.network_short,
                                      plmn_ident.network_numeric);
            memcpy(plmn_ident.service_provider.data,
                   service_provider_name.data,
                   service_provider_name.len);
            plmn_ident.service_provider.len = service_provider_name.len;
            plmn_ident.display_condition = display_condition & 0x01;
            if (stat == CREG_STAT_Reg)
                plmn_ident.roaming_indicator = 0;
            if (stat == CREG_STAT_Roam)
                plmn_ident.roaming_indicator = 1;
            
            nm_get_COPN( &plmn_ident );      

            nm_signal(E_NM_FULL_SERVICE,&plmn_ident);

            cell.ci = cid;
            cell.lid = lac;
            nm_signal(E_NM_CELL_SELECT,&cell);         
            break;
            
        case CREG_STAT_NoSearch:
        case CREG_STAT_NotPresent:
            if (!reg_flag)
                nm_search_reg_status();
            break;
        case CREG_STAT_Search:
            if (reg_flag)
            {
                reg_stat = HW_SEARCH_NETWORK;
                nm_signal(E_NM_SEARCH_NETWORK,NULL);
            }
            else
                nm_search_reg_status();
            break;
        case CREG_STAT_Denied:
            qAT_PlusCEER( CMD_SRC_LCL, &cause);
            if (cause EQ 0x8488)     // If SIM is rejected in authentification
            	sim_invalid_ind();
            else
                nm_signal(E_NM_FORBIDDEN_NETWORK, NULL);
            break;
        default:
            nm_search_reg_status();
            break;
    }
}



/* Converts a given PLMN indication into the two other formats. */

GLOBAL void sAT_PlusCOPSE(UBYTE *oper, UBYTE format,
                          T_HW_LNAME *long_name,
                          T_HW_SNAME *short_name,
                          UBYTE *numeric_name)
{
  T_OPER_ENTRY plmnDesc;
  BOOL found;

  emo_printf("sAT_PlusCOPSE()");

  long_name->len = 0;
  short_name->len = 0;
  strcpy ((char *)numeric_name, "");

  switch (format)
  {
    case COPS_FRMT_Numeric:
      strcpy ((char *)numeric_name, (char *)oper);
      found = cmhMM_FindNumeric (&plmnDesc, (char *)oper);
      if (found)
      {
        UBYTE length=strlen((const char*)plmnDesc.longName);
        long_name->len = MINIMUM(LONG_NAME-1, length);
        memcpy (long_name->data, plmnDesc.longName, long_name->len);
        long_name->data[long_name->len] = '\0';
        long_name->dcs = HW_ASCII;
          length=strlen((const char*)plmnDesc.shrtName);
        short_name->len = MINIMUM(SHORT_NAME-1, length);
        memcpy (short_name->data, plmnDesc.shrtName, short_name->len);
        short_name->data[short_name->len] = '\0';
        short_name->dcs = HW_ASCII;
      }
      break;

    case COPS_FRMT_Long:
      if (oper)
      {
        UBYTE length=strlen((const char *)oper);
        long_name->len = MINIMUM(LONG_NAME-1, length);
        memcpy (long_name->data, oper, long_name->len);
        long_name->data[long_name->len] = '\0';
        long_name->dcs = HW_ASCII;
      }
      found = cmhMM_FindName (&plmnDesc, (char *)oper, CPOL_FRMT_Long);
      if (found)
      {
        UBYTE length;
        numeric_name[0] = ((plmnDesc.mcc >> 8) & 0xf) + '0';
        numeric_name[1] = ((plmnDesc.mcc >> 4) & 0xf) + '0';
        numeric_name[2] = ( plmnDesc.mcc       & 0xf) + '0';
        numeric_name[3] = ((plmnDesc.mnc >> 8) & 0xf) + '0';
        numeric_name[4] = ((plmnDesc.mnc >> 4) & 0xf) + '0';
        if ((plmnDesc.mnc & 0xf) EQ 0xf)
          numeric_name[5] = '\0'; /* 2-digit-MNC */
        else
          numeric_name[5] = ( plmnDesc.mnc       & 0xf) + '0';
        numeric_name[6] = '\0';
        length=strlen((const char*)plmnDesc.shrtName);
        short_name->len = MINIMUM(SHORT_NAME-1, length);
        memcpy (short_name->data, plmnDesc.shrtName, short_name->len);
        short_name->data[short_name->len] = '\0';
        short_name->dcs = HW_ASCII;
      }
      break;

    case COPS_FRMT_Short:
      if (oper)
      {
         UBYTE length=strlen((const char *)oper);
        short_name->len = MINIMUM(SHORT_NAME-1, length);
        memcpy (short_name->data, oper, short_name->len);
        short_name->data[short_name->len] = '\0';
        short_name->dcs = HW_ASCII;
      }
      found = cmhMM_FindName (&plmnDesc, (char *)oper, CPOL_FRMT_Short);
      if (found)
      {
        UBYTE length;
        numeric_name[0] = ((plmnDesc.mcc >> 8) & 0xf) + '0';
        numeric_name[1] = ((plmnDesc.mcc >> 4) & 0xf) + '0';
        numeric_name[2] = ( plmnDesc.mcc       & 0xf) + '0';
        numeric_name[3] = ((plmnDesc.mnc >> 8) & 0xf) + '0';
        numeric_name[4] = ((plmnDesc.mnc >> 4) & 0xf) + '0';
        if ((plmnDesc.mnc & 0xf) EQ 0xf)
          numeric_name[5] = '\0'; /* 2-digit-MNC */
        else
          numeric_name[5] = ( plmnDesc.mnc       & 0xf) + '0';
        numeric_name[6] = '\0'; /* 3-digit-MNC */
        length=strlen((const char*)plmnDesc.longName);
        long_name->len = MINIMUM(LONG_NAME-1, length);
        memcpy (long_name->data, plmnDesc.longName, long_name->len);
        long_name->data[long_name->len] = '\0';
        long_name->dcs = HW_ASCII;
      }
      break;
  }
}

T_RTC_DATE* hw_td_get_date()
{
        emo_printf("hw_td_get_date");
#ifndef EMO_SIM
        rtc_get_time_date(&current_date, &current_time,RTC_TIME_TYPE_CURRENT); 
#endif
        return &current_date;
}

T_RTC_TIME* hw_td_get_time()
{
        emo_printf("hw_td_get_time");
#ifndef EMO_SIM
        rtc_get_time_date(&current_date, &current_time,RTC_TIME_TYPE_CURRENT); 
#endif
		return &current_time;
}

char* hw_td_get_clock_str()
{       
		emo_printf("hw_td_get_clock_str");
#ifndef EMO_SIM
        rtc_get_time_date(&current_date, &current_time,RTC_TIME_TYPE_CURRENT); 
#endif
        if(current_time.PM_flag==1)
        	sprintf(time_string, "%02d:%02d pm ", current_time.hour, current_time.minute);
        else
        	sprintf(time_string, "%02d:%02d am ", current_time.hour, current_time.minute);

        return time_string;
}

char* hw_td_get_date_str()
{      
		emo_printf("hw_td_get_date_str");
        sprintf(date_string, "%02d/%02d/%04d", current_date.month, current_date.day, current_date.year);
        return date_string;
}

void hw_td_set_time(T_RTC_TIME* time)
{       
		int result;
#ifndef EMO_SIM
        emo_printf("hw_td_set_time");
        result = rtc_set_time_date(&current_date, time);
        if (result == 0)
        {       memcpy(&current_time, time, sizeof(T_RTC_TIME));
                return;
        }
		emo_printf("RTC driver error");
#endif
        return;
}

void hw_td_set_date(T_RTC_DATE* date)
{     	int result;
#ifndef EMO_SIM
        emo_printf("hw_td_set_date");
        result = rtc_set_time_date(date, &current_time);
        if (result == 0)
        {
                memcpy(&current_date, date, sizeof(T_RTC_DATE));
				return;
        }
		emo_printf("RTC driver error");
#endif
        return;
}

void hw_td_init(void) {

#ifndef EMO_SIM
    if(!rtc_clock_cleared())
    {
        rtc_get_time_date(&current_date, &current_time,RTC_TIME_TYPE_CURRENT);
    } else {
        rtc_get_time_date(&current_date, &current_time,RTC_TIME_TYPE_CURRENT);
#else
	{
#endif
        current_time.second = 0;
        current_time.minute = 0;
        current_time.hour = 0;

        current_date.year = 2010;
        current_date.month = 9;
        current_date.day = 22;

        hw_td_set_time(&current_time);
        hw_td_set_date(&current_date);
    }
}

