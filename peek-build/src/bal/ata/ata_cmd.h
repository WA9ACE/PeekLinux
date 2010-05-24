#ifndef __BTC_ATA_CMD_H_20080112__
#define __BTC_ATA_CMD_H_20080112__

#ifdef __cplusplus
extern "C" {
#endif

#include "ata_cfg.h"
#include "ata_aci_send.h"

typedef T_ATA_return (*T_ata_send_func_ptr)(const char*);

typedef enum
{
	ATA_PRI_LOW= 0,
	ATA_PRI_NOMAL,
	ATA_PRI_HIGH
} T_ata_cmd_pri;

typedef enum
{
	ATA_TIMEOUT_NO      = 0,
	ATA_TIMEOUT_SHORT   = 10000,
	ATA_TIMEOUT_DEFAULT = 30000,
	ATA_TIMEOUT_LONG    = 60000
} T_ata_cmd_timeout;

typedef struct 
{
    char                *at_string;
    T_ata_send_func_ptr func_ptr;
	int                 timeout;
	T_ata_cmd_pri       pri;
} T_ata_send_type;

static T_ata_send_type ata_at_map[] =
{
#ifdef ATA_UNIT_TEST
	{"+TEST1", ata_plus_test_int_send,   ATA_TIMEOUT_DEFAULT,  ATA_PRI_LOW   },
	{"+TEST2", ata_plus_test_str_send,   ATA_TIMEOUT_LONG,     ATA_PRI_HIGH  },
#endif /* ATA_UNIT_TEST */
    {"+COPS", ata_plus_cops_send,        ATA_TIMEOUT_LONG,     ATA_PRI_NOMAL },
    {"%NRG",  ata_percent_nrg_send,      ATA_TIMEOUT_LONG,     ATA_PRI_NOMAL },
    {"D",     ata_at_d_send,             ATA_TIMEOUT_DEFAULT,  ATA_PRI_HIGH  },
    {"H",     ata_at_h_send,             ATA_TIMEOUT_DEFAULT,  ATA_PRI_HIGH  },
    {"A",     ata_at_a_send,             ATA_TIMEOUT_DEFAULT,  ATA_PRI_HIGH  },
    {"+CFUN", ata_plus_cfun_send,        ATA_TIMEOUT_LONG,     ATA_PRI_NOMAL },
    {"+CFUN?", ata_plus_cfun_q_send,     ATA_TIMEOUT_LONG,     ATA_PRI_NOMAL },
    {"%CREG", ata_percent_creg_send,     ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CLAN", ata_plus_clan_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CIMI", ata_plus_cimi_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CNMA", ata_plus_cnma_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CNMI", ata_plus_cnmi_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CSCA", ata_plus_csca_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CSCS", ata_plus_cscs_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CSMS", ata_plus_csms_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CSVM", ata_plus_csvm_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CSCB", ata_plus_cscb_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CLIP", ata_plus_clip_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CLIR", ata_plus_clir_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CMGD", ata_plus_cmgd_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CMGL", ata_plus_cmgl_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CMGR", ata_plus_cmgr_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CMGS", ata_plus_cmgs_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CMGW", ata_plus_cmgw_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CMSS", ata_plus_cmss_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CSQ",  ata_plus_csq_send,         ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"%CSQ",  ata_percent_csq_send,      ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CCWA", ata_plus_ccwa_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"%CHLD", ata_percent_chld_send,     ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CACM", ata_plus_cacm_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CAOC", ata_plus_caoc_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CCFC", ata_plus_ccfc_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CPBR", ata_plus_cpbr_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CPBS", ata_plus_cpbs_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CPUC", ata_plus_cpuc_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CPWD", ata_plus_cpwd_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CPIN", ata_plus_cpin_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CPOL", ata_plus_cpol_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CLCK", ata_plus_clck_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CPBW", ata_plus_cpbw_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CPMS", ata_plus_cpms_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CUSD", ata_plus_cusd_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+CGSN", ata_plus_cpms_send,        ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"+VTS",  ata_plus_vts_send,         ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"%SATC", ata_percent_satc_send,     ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"%SATE", ata_percent_sate_send,     ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"%SATR", ata_percent_satr_send,     ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
    {"%SATT", ata_percent_satt_send,     ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL },
#ifdef ATA_GPRS
     {"+CGATT", ata_plus_cgatt_send,     ATA_TIMEOUT_DEFAULT,  ATA_PRI_NOMAL }
#endif
};


typedef enum             /* AT command identifier     */
{
  ATA_CMD_NONE      = 0,  /* no ACI command identifier */
  ATA_CMD_CACM      = 1,
  ATA_CMD_CAMM      = 2,
  ATA_CMD_CAOC      = 3,
  ATA_CMD_CBC       = 4,
  ATA_CMD_CBST      = 5,
  ATA_CMD_CCFC      = 6,
  ATA_CMD_CCUG      = 7,
  ATA_CMD_CCWA      = 8,
  ATA_CMD_CCWE      = 9,
  ATA_CMD_CEER      = 10,
  ATA_CMD_CFUN      = 11,
  ATA_CMD_CGACT     = 12,
  ATA_CMD_CGANS     = 13,
  ATA_CMD_CGATT     = 14,
  ATA_CMD_CGAUTO    = 15,
  ATA_CMD_CGCLASS   = 16,
  ATA_CMD_CGDATA    = 17,
  ATA_CMD_CGDCONT   = 18,
  ATA_CMD_CGEREP    = 19,
  ATA_CMD_CGMI      = 20,
  ATA_CMD_CGMM      = 21,
  ATA_CMD_CGMR      = 22,
  ATA_CMD_CGPADDR   = 23,
  ATA_CMD_CGQMIN    = 24,
  ATA_CMD_CGQREQ    = 25,
  ATA_CMD_CGREG     = 26,
  ATA_CMD_CGSMS     = 27,
  ATA_CMD_CGSN      = 28,
  ATA_CMD_CHLD      = 29,
  ATA_CMD_CHUP      = 30,
  ATA_CMD_CIMI      = 31,
  ATA_CMD_CLAC      = 32,
  ATA_CMD_CLAE      = 33,
  ATA_CMD_CLAN      = 34,
  ATA_CMD_CLCC      = 35,
  ATA_CMD_CLCK      = 36,
  ATA_CMD_CLIP      = 37,
  ATA_CMD_CLIR      = 38,
  ATA_CMD_CLVL      = 39,
  ATA_CMD_CMEE      = 40,
  ATA_CMD_CMGC      = 41,
  ATA_CMD_CMGD      = 42,
  ATA_CMD_CMGF      = 43,
  ATA_CMD_CMGL      = 44,
  ATA_CMD_CMGR      = 45,
  ATA_CMD_CMGS      = 46,
  ATA_CMD_CMGW      = 47,
  ATA_CMD_CMOD      = 48,
  ATA_CMD_CMSS      = 49,
  ATA_CMD_CMUT      = 50,
  ATA_CMD_CMUX      = 51,
  ATA_CMD_CNMA      = 52,
  ATA_CMD_CNMI      = 53,
  ATA_CMD_CNUM      = 54,
  ATA_CMD_COLP      = 55,
  ATA_CMD_COPN      = 56,
  ATA_CMD_COPS      = 57,
  ATA_CMD_CPAS      = 58,
  ATA_CMD_CPBF      = 59,
  ATA_CMD_CPBR      = 60,
  ATA_CMD_CPBS      = 61,
  ATA_CMD_CPBW      = 62,
  ATA_CMD_CPIN      = 63,
  ATA_CMD_CPMS      = 64,
  ATA_CMD_CPOL      = 65,
  ATA_CMD_CPUC      = 66,
  ATA_CMD_CPWD      = 67,
  ATA_CMD_CR        = 68,
  ATA_CMD_CRC       = 69,
  ATA_CMD_CREG      = 70,
  ATA_CMD_CRES      = 71,
  ATA_CMD_CRLP      = 72,
  ATA_CMD_CRSL      = 73,
  ATA_CMD_CRSM      = 74,
  ATA_CMD_CSAS      = 75,
  ATA_CMD_CSCA      = 76,
  ATA_CMD_CSCB      = 77,
  ATA_CMD_CSCS      = 78,
  ATA_CMD_CSDH      = 79,
  ATA_CMD_CSMP      = 80,
  ATA_CMD_CSMS      = 81,
  ATA_CMD_CSNS      = 82,
  ATA_CMD_CSQ       = 83,
  ATA_CMD_CSSN      = 84,
  ATA_CMD_CSTA      = 85,
  ATA_CMD_CSVM      = 86,
  ATA_CMD_CTFR      = 87,
  ATA_CMD_CUSD      = 88,
  ATA_CMD_DR        = 89,
  ATA_CMD_DS        = 90,

#ifdef FF_FAX
  ATA_CMD_FAP       = 92,
  ATA_CMD_FBO       = 93,
  ATA_CMD_FBS       = 94,
  ATA_CMD_FBU       = 95,
  ATA_CMD_FCC       = 96,
  ATA_CMD_FCLASS    = 97,
  ATA_CMD_FCQ       = 98,
  ATA_CMD_FCR       = 99,
  ATA_CMD_FCS       = 100,
  ATA_CMD_FCT       = 101,
  ATA_CMD_FDR       = 102,
  ATA_CMD_FDT       = 103,
  ATA_CMD_FEA       = 104,
  ATA_CMD_FFC       = 105,
  ATA_CMD_FHS       = 106,
  ATA_CMD_FIE       = 107,
  ATA_CMD_FIP       = 108,
  ATA_CMD_FIS       = 109,
  ATA_CMD_FIT       = 110,
  ATA_CMD_FKS       = 111,
  ATA_CMD_FLI       = 112,
  ATA_CMD_FLO       = 113,
  ATA_CMD_FLP       = 114,
  ATA_CMD_FMI       = 115,
  ATA_CMD_FMM       = 116,
  ATA_CMD_FMR       = 117,
  ATA_CMD_FMS       = 118,
  ATA_CMD_FND       = 119,
  ATA_CMD_FNR       = 120,
  ATA_CMD_FNS       = 121,
  ATA_CMD_FPA       = 122,
  ATA_CMD_FPI       = 123,
  ATA_CMD_FPS       = 125,
  ATA_CMD_FPW       = 126,
  ATA_CMD_FRQ       = 127,
  ATA_CMD_FSA       = 129,
  ATA_CMD_FSP       = 130,
#endif /* FF_FAX */

  ATA_CMD_GCAP      = 131,
  ATA_CMD_GCI       = 132,
  ATA_CMD_GMI       = 133,
  ATA_CMD_GMM       = 134,
  ATA_CMD_GMR       = 135,
  ATA_CMD_GSN       = 136,
  ATA_CMD_ICF       = 137,
  ATA_CMD_IFC       = 138,
  ATA_CMD_ILRR      = 139,
  ATA_CMD_IPR       = 140,
  ATA_CMD_TM        = 141,
  ATA_CMD_VST       = 142,
  ATA_CMD_WS46      = 143,
  ATA_CMD_ALS       = 144,
  ATA_CMD_CLSA      = 145,
  ATA_CMD_CLOM      = 146,
  ATA_CMD_CLPS      = 147,
  ATA_CMD_CLSR      = 148,
  ATA_CMD_BAND      = 149,
  ATA_CMD_P_CACM    = 150,
  ATA_CMD_P_CAOC    = 151,
  ATA_CMD_CCBS      = 152,
  ATA_CMD_CGAATT    = 153,
  ATA_CMD_P_CGMM    = 154,
  ATA_CMD_P_CGREG   = 155,
  ATA_CMD_CNAP      = 156,
  ATA_CMD_CPI       = 157,
  ATA_CMD_CTTY      = 158,
  ATA_CMD_COLR      = 159,
  ATA_CMD_CPRIM     = 160,
  ATA_CMD_CTV       = 161,
  ATA_CMD_CUNS      = 162,
  ATA_CMD_NRG       = 163,
  ATA_CMD_PPP       = 164,
  ATA_CMD_SATC      = 165,
  ATA_CMD_SATE      = 166,
  ATA_CMD_SATR      = 167,
  ATA_CMD_SATT      = 168,
  ATA_CMD_MTST      = 169,
  ATA_CMD_SNCNT     = 170,
  ATA_CMD_VER       = 171,
  ATA_CMD_P_CGCLASS = 172,
  ATA_CMD_CGPCO     = 173,
  ATA_CMD_CGPPP     = 174,
  ATA_CMD_EM        = 175,
  ATA_CMD_EMET      = 176,
  ATA_CMD_EMETS     = 177,
  ATA_CMD_WAP       = 178,
  ATA_CMD_CBHZ      = 179,
  ATA_CMD_CPHS      = 180,     /* %CPHS   command id */
  ATA_CMD_CPNUMS    = 181,     /* %CPNUMS command id */
  ATA_CMD_CPALS     = 182,     /* %CPALS  command id */
  ATA_CMD_CPVWI     = 183,     /* %CPVWI  voice message waiting command id */
  ATA_CMD_CPOPN     = 184,     /* %CPOPN  operator name string command id */
  ATA_CMD_CPCFU     = 185,     /* %CPCFU  command id */
  ATA_CMD_CPINF     = 186,     /* %CPHS information and customer service profile command id */
  ATA_CMD_CPMB      = 187,     /* %CPHS mailbox numbers */
  ATA_CMD_CPRI      = 188,
  ATA_CMD_DATA      = 189,
  ATA_CMD_DINF      = 190,
  ATA_CMD_P_CLCC    = 191,
  ATA_CMD_P_VST     = 192,
  ATA_CMD_CHPL      = 193,
  ATA_CMD_CTZR      = 194,
  ATA_CMD_VTS       = 195,
  ATA_CMD_PVRF      = 196,
  ATA_CMD_CWUP      = 197,
  ATA_CMD_ABRT      = 198,
  ATA_CMD_EXT       = 199,
  ATA_CMD_D         = 200,     /* D     command id */
  ATA_CMD_O         = 201,     /* O     command id */
  ATA_CMD_A         = 202,     /* A     command id */
  ATA_CMD_H         = 203,     /* H     command id */
  ATA_CMD_Z         = 204,     /* Z     command id */
  ATA_CMD_P_CREG    = 205,
  ATA_CMD_P_CSQ     = 206,     /* %CSQ  command id */
  ATA_CMD_CSIM      = 207,     /* +CSIM command id */
  ATA_CMD_ATR       = 208,     /* %ATR  command id */
  ATA_CMD_SMBS      = 209,
  ATA_CMD_DAR       = 210,     /* %DAR  command id */
  ATA_CMD_RDL       = 211,     /* %RDL command id, process redial mode */
  ATA_CMD_RDLB      = 212,     /* %RDLB command id, process black list */
  ATA_CMD_CIND      = 213,     /* +CIND command id */
  ATA_CMD_CMER      = 214,     /* +CMER command id */
  ATA_CMD_CSCN      = 215,     /* %CSCN command id */
  ATA_CMD_CSTAT     = 216,
  ATA_CMD_CPRSM     = 217,     /* %CPRSM command id */
  ATA_CMD_P_CHLD    = 218,     /* %CHLD  command id */
  ATA_CMD_CTZU      = 219,
  ATA_CMD_P_CTZV    = 220,
  ATA_CMD_P_CNIV    = 221,
  ATA_CMD_P_SECP    = 222,
  ATA_CMD_P_SECS = 223,
  ATA_CMD_P_CSSN = 224, /* %CSSN command id */
  ATA_CMD_CCLK    = 225,
  ATA_CMD_CSSD      = 226,     /* %CSSD  command id */
  ATA_CMD_P_COPS =227,
  ATA_CMD_CPMBW     = 228,     /* %CPMBW command id */
  ATA_CMD_CUST       = 229,    /* %CUST command id */
  ATA_CMD_SATCC       = 230,    /* %SATCC command id */
  ATA_CMD_P_SIMIND  = 231,     /* %SIMIND command id */
  ATA_CMD_SIMRST    = 232,     /* State during SIM reset, not related to any AT cmd */
  ATA_CMD_P_COPN    = 233,
  ATA_CMD_P_CGEREP    = 234,     /* %CGEREP - TI Proprietary CPHS Event Reporting */
#ifdef FF_DUAL_SIM
  ATA_CMD_SIM       = 235,
#endif /*FF_DUAL_SIM*/
  ATA_CMD_CUSCFG    =  236,     /* %CUSCFG comand id*/
  ATA_CMD_CUSDR     =  237,     /* %CUSDR command id */
  ATA_CMD_CMMS      =  238,      /* +CMMS command id */
  ATA_CMD_STDR      =  239,     /* %CUSDR command id */
  ATA_CMD_P_CPBS    =  240,     /* %CPBS comand id*/
  ATA_CMD_P_DBGINFO =  241,     /* %DBGINFO command id */
  ATA_CMD_CDIP      =  242,
  ATA_CMD_P_PBCF    =  243,     /* %PBCF comand id */  
  ATA_CMD_SIMEF     =  244,     /* %SIMEF command id */
  ATA_CMD_EFRSLT    =  245,     /* %EFRSLT command id */
#ifdef SIM_PERS
  ATA_CMD_MEPD      =  246,      /* For %MEPD -for querying ME Personalisation Data... Added on 11/03/2005 */
#endif  
  ATA_CMD_P_CMGMDU  =  247,     /* %CMGMDU command id */
  ATA_CMD_P_CMGL    =  248,
  ATA_CMD_P_CMGR    =  249,
#ifdef FF_CPHS_REL4
  ATA_CMD_P_CFIS    =  250,     /* %CFIS command Id  */
  ATA_CMD_P_MWIS    =  251,     /* %MWIS command Id  */
  ATA_CMD_P_MWI     =  252,     /* %MWI command Id   */
  ATA_CMD_P_MBI     =  253,     /* %MBI command Id   */
  ATA_CMD_P_MBDN    =  254,     /* %MBDN command Id  */
#endif /* FF_CPHS_REL4 */
#ifdef TI_PS_FF_ATA_CMD_P_ECC
  ATA_CMD_P_ECC     =  255,     /* %ECC command id */
#endif /* TI_PS_FF_ATA_CMD_P_ECC */
#ifdef FF_PS_CTREG
  ATA_CMD_CTREG     =  256,
#endif /* FF_PS_CTREG */
//  ATA_CMD_SMSREV,   //Daisy Tang test code
  ATA_CMD_MAX,                  /* maximum command id */
  ATA_CMD_BIGGEST = 0x0000ffff  /* To avoid the lint warning 650 */
} T_ATA_CMD;


typedef enum
{
    ATA_RSP_OK,
    ATA_RSP_ERROR
} T_ATA_RSP;

typedef enum
{
    ATA_NO_ANSWER,
    ATA_NO_DIALTONE,
    ATA_RING,
    ATA_CONNECTION
} T_ATA_UNSOLICITED_EVENT;

#ifdef __cplusplus
}
#endif
#endif //__BTC_ATA_CMD_H_20080112__

