/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  CUS_ACI
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
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
|  Purpose :  Custom dependent definitions for AT Command Interpreter
|             Use this header for definitions to integrate the
|             ACI in your target system !
+----------------------------------------------------------------------------- 
*/ 

#ifndef CUS_ACI_H
#define CUS_ACI_H

/*==== CONSTANTS ==================================================*/
/*
 * TIMER_VALUES
 *
 * Description :  The constants define the timer values
 *                for the MMI timer TXXX depending on the various
 *                channel types and service access point identifiers.
 *                If your target system uses other units please
 *                change the values.
 */

/*
 * all values in milliseconds
 */
#define TICS_PER_DECIHOURS     (20*60*6*50)
#define TDTE_INACTIV_VALUE     500
#define TRING_VALUE            3000
#define TMPTY_VALUE            10000        /* 10s (5 to 30 seconds) */
#define TECT_VALUE             13500  /* 13,5s (5 to 15 seconds GSM 04.91 and 04.80) */
#define TDTMF_VALUE            3000         /*  3s wait time GSM 02.07 */

#ifdef VOCODER_FUNC_INTERFACE
#define VOCODER_VALUE          35
#endif

/* MAX_ACI_TIMER
 * Description :  The constant defines the number of
 *                timeouts which can be stored. */
#if defined FF_EOTD /*&& defined _SIMULATION_ */
#define MAX_ACI_TIMER 30
#else /*  no defined FF_EOTD */
#define MAX_ACI_TIMER 25
#endif
/*
 * VERSION
 *
 * Description :  The constants define the type and the value
 *                of a version identification. The version
 *                is part of the monitor struct.
 */

#ifndef SMI
#define T_VERSION   CHAR
#define VERSION_ACI  "ACI 1.0"
#endif

#ifdef SMI
#define T_VERSION   CHAR
#define VERSION_SMI  "SMI 1.0"
#endif

/*
 * VSI_CALLER
 *
 * Description :  For multithread applications the constant VSI_CALLER
 *                must be defined to identify the calling thread of the
 *                VSI-Interface. This must be done correponding to the
 *                type of T_VSI_CALLER in VSI.H. The comma symbol is
 *                neccessary because the vsi-functions are called
 *                like this vsi_xxx (VSI_CALLER par2, par3 ...)
 */

EXTERN T_HANDLE aci_handle;

#define VSI_CALLER aci_handle,
#define VSI_CALLER_SINGLE aci_handle

/*
 * It is a good idea to ensure that the following two definitions 
 * are not becoming longer than MAX_ALPHA_OPER_LEN.
 * Currently (21-May-2005) there is no long operator name known longer
 * than 25 characters and no short operator name known longer than 8
 * characters.
 * 27.007 limits the long operator names to 16 characters and 
 * the short operator names to 8 characters in the description
 * of the AT+COPS command, however there are a lot of operators
 * currently which exceed this limit for the long operator name.
 */
#define MAX_LONG_OPER_LEN     (25+1)  /* "Cable & Wireless Guernsey" + '\0' */
#define MAX_SHRT_OPER_LEN     (8+1)   /* short name + '\0' */ 

/*
 * Network operator list
 *
 * Description :  this is the list of all known network operators.
 *                There are five entries for each operator. A long
 *                name (max.24 chars), a short name (max.10 chars),
 *                the mobile country code, the mobile network code
 *                and a language reference. It is important to leave
 *                the EOL indicator at the end of the list to terminate
 *                the list.
 * Source :       The list depends on the GSM Association Infocentre - SE_13
 *                document
 * 
 */
typedef struct
{
    const char * longName;
    const char * shrtName;
    SHORT  mcc;
    SHORT  mnc;
} T_OPER_NTRY_FIXED;

/*
 * Beware, structure used from MFW directly by some "side access".
 */
typedef struct
{
    char longName[MAX_LONG_OPER_LEN];
    char shrtName[MAX_SHRT_OPER_LEN];
    SHORT  mcc;
    SHORT  mnc;
    UBYTE  pnn;          /* PLMN Network Name Source (for EONS) */
    UBYTE  long_len;     /* for EONS */
    UBYTE  long_ext_dcs;
    UBYTE  shrt_len;     /* for EONS */
    UBYTE  shrt_ext_dcs;
} T_OPER_ENTRY;

#ifdef TI_PS_OP_OPN_TAB_ROMBASED
    /* No definition for rom table is required as it is defined in xml file. */
#else 
#ifdef CMH_MMF_C

/*
 * Beware, data used from MFW directly by some "side access".
 */
const T_OPER_NTRY_FIXED operListFixed[] =
{
    {"Test Network",              "TestNet",          0x001, 0x01F   }, /* NOT UPDATED, because of test network            */
    {"Test Net 222",              "Test222",          0x222, 0x22F   }, /* NOT UPDATED, because of test network            */
    {"one",                       "one",              0x232, 0x05F   },
    {"A1",                        "A1",               0x232, 0x01F   },  
    {"T-Mobile A",                "TMO A",            0x232, 0x03F   },  
    {"A tele.ring",               "telering",         0x232, 0x07F   },
    {"SETAR GSM",                 "SETARGSM",         0x363, 0x01F   },
    {"AF AWCC",                   "AWCC",             0x412, 0x01F   },  
    {"UNITEL",                    "UNITEL",           0x631, 0x02F   },
    {"AMC - AL",                  "A M C",            0x276, 0x01F   },
    {"vodafone AL",               "voda AL",          0x276, 0x02F   },
#if !(defined(BT_ADAPTER) && defined(_SIMULATION_))
    /*
     *  UPDATED NETWORKS
     */ 
    {"Cingular",                  "Cingular",     0x000, 0x180   },
    {"vodafone GR",               "voda GR",      0x202, 0x05F   },
    {"GR Q-TELECOM",              "Q-TELCOM",     0x202, 0x09F   },
    {"GR COSMOTE",                "C-OTE",        0x202, 0x01F   },
    {"TIM GR",                    "TIM",          0x202, 0x10F   },
    {"vodafone NL",               "voda NL",      0x204, 0x04F   },
    {"NL Telfort",                "NL Tlfrt",     0x204, 0x12F   },
    {"T-Mobile NL",               "TMO NL",       0x204, 0x16F   },
    {"Orange NL",                 "Orange",       0x204, 0x20F   },
    {"NL KPN ",                   "NL KPN",       0x204, 0x08F   },
    {"BEL PROXIMUS",              "PROXI",        0x206, 0x01F   },
    {"B mobistar",                "mobi*",        0x206, 0x10F   },
    {"BASE",                      "BASE",         0x206, 0x20F   },
    {"Orange F",                  "Orange",       0x208, 0x01F   },
    {"F SFR",                     "SFR",          0x208, 0x10F   },
    {"F - BOUYGUES TELECOM",      "BYTEL",        0x208, 0x20F   },
    {"STA-MOBILAND",              "M-AND",        0x213, 0x03F   },
    {"vodafone ES",               "voda ES",      0x214, 0x01F   },
    {"movistar",                  "movistar",     0x214, 0x07F   },
    {"E AMENA",                   "AMENA",        0x214, 0x03F   },
    {"H PANNON GSM",              "PANNON",       0x216, 0x01F   },
    {"T-Mobile H",                "TMO H",        0x216, 0x30F   },
    {"vodafone HU",               "voda HU",      0x216, 0x70F   },
    {"BA-ERONET",                 "ERONET",       0x218, 0x03F   },
    {"MOBI'S",                    "MOBI'S",       0x218, 0x05F   },
    {"BH GSMBIH",                 "GSMBIH",       0x218, 0x90F   },
    {"T-Mobile HR",               "T-Mobile",     0x219, 0x01F   },
    {"HR VIP",                    "VIP",          0x219, 0x10F   },
    {"ProMonte",                  "ProMonte",     0x220, 0x02F   },
    {"YUG 03",                    "SCGTS",        0x220, 0x03F   },
    {"MONET",                     "MONET",        0x220, 0x04F   },
    {"YU MOBTEL",                 "MOBTEL",       0x220, 0x01F   },
    {"I TIM",                     "TIM",          0x222, 0x01F   },
    {"vodafone IT",               "voda IT",      0x222, 0x10F   },
    {"I WIND",                    "I WIND",       0x222, 0x88F   },
    {"RO Cosmorom",               "Cosmorom",     0x226, 0x03F   },
    {"RO CONNEX",                 "CONNEX",       0x226, 0x01F   },
    {"RO ORANGE",                 "ORANGE",       0x226, 0x10F   },
    {"Swisscom",                  "SWISS",        0x228, 0x01F   },
    {"sunrise",                   "sunrise",      0x228, 0x02F   },
    {"orange CH",                 "ORANGE",       0x228, 0x03F   },
    {"In&Phone",                  "In&Phone",     0x228, 0x07F   },
    {"CHE, Tele2 Switzerland",    "T2",           0x228, 0x08F   },
    {"T-Mobile CZ",               "TMO CZ",       0x230, 0x01F   },
    {"EUROTEL - CZ",              "ET - CZ",      0x230, 0x02F   },
    {"OSKAR",                     "OSKAR",        0x230, 0x03F   },
    {"Orange SK",                 "Orange",       0x231, 0x01F   },
    {"T-Mobile SK",               "TMO SK",       0x231, 0x02F   },
    {"A1",                        "A1",           0x232, 0x01F   },
    {"T-Mobile A",                "TMO A",        0x232, 0x03F   },
    {"one",                       "one",          0x232, 0x05F   },
    {"A tele.ring",               "telering",     0x232, 0x07F   },
    {"O2 - UK",                   "O2 -UK",       0x234, 0x10F   },
    {"vodafone UK",               "voda UK",      0x234, 0x15F   },
    {"T-Mobile UK",               "TMO UK",       0x234, 0x30F   },
    {"T-Mobile UK",               "TMO UK",       0x234, 0x31F   },
    {"T-Mobile UK",               "TMO UK",       0x234, 0x32F   },
    {"Orange",                    "Orange",       0x234, 0x33F   },
    {"JT-Wave",                   "JT-Wave",      0x234, 0x50F   },
    {"Cable & Wireless Guernse",  "C&W",          0x234, 0x55F   },
    {"Manx Pronto",               "Pronto",       0x234, 0x58F   },
    {"TDC MOBIL",                 "DK TDC",       0x238, 0x01F   },
    {"DK SONOFON",                "SONO",         0x238, 0x02F   },
    {"TELIA DK",                  "TELIA",        0x238, 0x20F   },
    {"Orange",                    "Orange",       0x238, 0x30F   },
    {"TELIA S",                   "TELIA",        0x240, 0x01F   },
    {"S COMVIQ",                  "IQ",           0x240, 0x07F   },
    {"vodafone SE",               "voda SE",      0x240, 0x08F   },
    {"SpringMobil SE",            "Spring",       0x240, 0x10F   },
    {"N Telenor",                 "TELENOR",      0x242, 0x01F   },
    {"N NetCom",                  "NetCom",       0x242, 0x02F   },
    {"Teletopia 3",               "T3",           0x242, 0x03F   },
    {"FINNET or FI 2G",           "FINNET",       0x244, 0x03F   },
    {"FI elisa",                  "elisa",        0x244, 0x05F   },
    {"FINNET or FI 2G",           "FINNET",       0x244, 0x12F   },
    {"FI AMT",                    "FI AMT",       0x244, 0x14F   },
    {"FI SONERA",                 "SONERA",       0x244, 0x91F   },
    {"OMNITEL LT",                "OMT",          0x246, 0x01F   },
    {"LT BITE GSM",               "BITE",         0x246, 0x02F   },
    {"TELE2",                     "TELE2",        0x246, 0x03F   },
    {"LV LMT GSM, LV LMT",        "LMT GSM",      0x247, 0x01F   },
    {"LV TELE2",                  "TELE2",        0x247, 0x02F   },
    {"EE EMT GSM",                "EMT",          0x248, 0x01F   },
    {"TELE2",                     "TELE2",        0x248, 0x03F   },
    {"EE elisa",                  "elisa",        0x248, 0x02F   },
    {"MTS-RUS",                   "MTS",          0x250, 0x01F   },
    {"MegaFon RUS",               "MegaFon",      0x250, 0x02F   },
    {"SIBCHALLENGE RUS",          "RUS_SCN",      0x250, 0x04F   },
    {"SCS, RUS 05, RUS SCS, RU",  "STC",          0x250, 0x05F   },
  /*{"SCS RUS",                   "SCS",          0x250, 0x05F   },*/
  /*{"RUS SCS",                   "SCS",          0x250, 0x05F   },*/
    {"RUS 07, RUS SMARTS",        "SMARTS",       0x250, 0x07F   },
    {"RUS DTC",                   "DTC",          0x250, 0x10F   },
    {"ORENSOT",                   "ORENSOT",      0x250, 0x11F   },
    {"RF FAR EAST",               "RF FEast",     0x250, 0x12F   },
  /*{"RUS - Sakhalin",            "SAKHALIN",     0x250, 0x12F   },*/
  /*{"RUS FEast; RUS 12",         "BWC",          0x250, 0x12F   },*/
  /*{"SIBI-RUS",                  "SIBI",         0x250, 0x12F   },*/
  /*{"RUS 12, RUS Far East",      "Far East",     0x250, 0x12F   },*/
    {"RUS 12",                    "AKOS GSM",     0x250, 0x120   },
    {"RUS Kuban-GSM",             "KUGSM",        0x250, 0x13F   },
    {"Di-ex",                     "Di-ex",        0x250, 0x14F   },
    {"RUS15, RUS SMARTS",         "SMARTS",       0x250, 0x15F   },
    {"RUS16,250 16",              "NTC",          0x250, 0x16F   },
    {"RUS 17",                    "ERMAK",        0x250, 0x17F   },
    {"RUS_BASHCELL",              "BASHCELL",     0x250, 0x19F   },
  /*{"DTI",                       "DTIGSM",       0x250, 0x19F   },*/
  /*{"RUS: INDIGO",               "INDIGO",       0x250, 0x19F   },*/
  /*{"RUS.INDIGO",                "INDIGO",       0x250, 0x19F   },*/
    {"RUS 20",                    "MOTIV",        0x250, 0x20F   },
  /*{"TELE2",                     "TELE2",        0x250, 0x20F   },*/
    {"RUS Beeline",               "Beeline",      0x250, 0x28F   },
    {"Uraltel",                   "RUS39",        0x250, 0x39F   },
  /*{"Uraltel",                   "Uraltel",      0x250, 0x39F   },*/
  /*{"RUS SUCT",                  "SUCT",         0x250, 0x39F   },*/
    {"RUS North Caucasian GSM",   "NC-GSM",       0x250, 0x44F   },
    {"RUS BMT",                   "BMT",          0x250, 0x07F   },
    {"Primetelefone RUS",         "Primtel",      0x250, 0x92F   },
    {"Beeline",                   "Beeline",      0x250, 0x99F   },
    {"UA UMC",                    "UMC",          0x255, 0x01F   },
    {"UKR-WellCOM",               "WellCOM",      0x255, 0x02F   },
    {"UA-KYIVSTAR",               "UA-KS",        0x255, 0x03F   },
    {"UA life:)",                 "life:)",       0x255, 0x06F   },
    {"UA-GT",                     "GT",           0x255, 0x05F   },
    {"BY VELCOM",                 "VELCOM",       0x257, 0x01F   },
    {"MTS BY",                    "MTS",          0x257, 0x02F   },
    {"MD VOXTEL",                 "VOXTEL",       0x259, 0x01F   },
    {"MD MOLDCELL",               "MDCELL",       0x259, 0x02F   },
    {"Era",                       "Era",          0x260, 0x02F   },
    {"Orange PL",                 "Orange",       0x260, 0x03F   },
    {"Plus GSM",                  "PLUS",         0x260, 0x01F   },
    {"T-Mobile D",                "TMO D",        0x262, 0x01F   },
    {"Vodafone.de",               "Vodafone",     0x262, 0x02F   },
    {"E-Plus",                    "E-Plus",       0x262, 0x03F   },
    {"o2 - de",                   "o2 - de",      0x262, 0x07F   },
    {"o2 - de",                   "o2 - de",      0x262, 0x08F   },
    {"GIBTEL GSM",                "GIBTEL",       0x266, 0x01F   },
    {"vodafone P",                "voda P",       0x268, 0x01F   },
    {"P OPTIMUS",                 "OPTIM",        0x268, 0x03F   },
    {"P TMN",                     "TMN",          0x268, 0x06F   },
    {"L LUXGSM",                  "LUXGSM",       0x270, 0x01F   },
    {"L TANGO",                   "TANGO",        0x270, 0x77F   },
    {"L VOX.LU",                  "VOX.LU",       0x270, 0x99F   },
    {"vodafone IE",               "voda IE",      0x272, 0x01F   },
    {"02 - IRL",                  "02 -IRL",      0x272, 0x02F   },
    {"IRL - METEOR",              "METEOR",       0x272, 0x03F   },
    {"IS SIMINN",                 "SIMINN",       0x274, 0x01F   },
    {"Og Vodafone",               "Vodafone",     0x274, 0x02F   },
    {"Og Vodafone",               "Vodafone",     0x274, 0x03F   },
    {"Viking",                    "Viking",       0x274, 0x04F   },
    {"AMC - AL",                  "A M C",        0x276, 0x01F   },
    {"vodafone AL",               "voda AL",      0x276, 0x02F   },
    {"vodafone MT",               "voda MT",      0x278, 0x01F   },
    {"go mobile",                 "gomobile",     0x278, 0x21F   },
    {"CYTAMOBILE-VODAFONE",       "CYTAVODA",     0x280, 0x01F   },
    {"areeba",                    "areeba",       0x280, 0x10F   },
    {"GEO-GEOCELL",               "GCELL",        0x282, 0x01F   },
    {"MAGTI-GSM-GEO",             "MAGTI",        0x282, 0x02F   },
    {"RA-ARMGSM",                 "ARMMO1",       0x283, 0x01F   },
    {"RA 05",                     "RA 05",        0x283, 0x05F   },
    {"M-TEL GSM BG",              "M-TEL",        0x284, 0x01F   },
    {"BG GLOBUL",                 "GLOBUL",       0x284, 0x05F   },
    {"TR TURKCELL",               "TCELL",        0x286, 0x01F   },
    {"TR TELSIM",                 "TELSIM",       0x286, 0x02F   },
    {"AVEA",                      "AVEA",         0x286, 0x03F   },
    {"Føroya Tele",               "FT-GSM",       0x288, 0x01F   },
    {"KALL",                      "KALL",         0x288, 0x02F   },
    {"TELE Greenland",            "TELE GRL",     0x290, 0x01F   },
    {"SI vodafone",               "SI voda",      0x293, 0x40F   },
    {"MOBITEL",                   "MOBITEL",      0x293, 0x41F   },
    {"SI VEGA 070",               "VEGA 070",     0x293, 0x70F   },
    {"MKD-MOBIMAK",               "MOBI-M",       0x294, 0x01F   },
    {"MKD COSMOFON",              "COSMOFON",     0x294, 0x02F   },
    {"SwisscomFL",                "Swiss FL",     0x295, 0x01F   },
    {"Orange FL",                 "OFL",          0x295, 0x02F   },
    {"FL1",                       "FL1",          0x295, 0x05F   },
    {"LI TANGO",                  "TANGO",        0x295, 0x77F   },
    {"Fido",                      "Fido",         0x302, 0x370   },
    {"Rogers Wireless",           "Rogers",       0x302, 0x720   },
    {"SPM AMERIS",                "AMERIS",       0x308, 0x01F   },
    {"Union Telephone",           "Union Te",     0x310, 0x020   },
    {"Centennial Wireless",       "Centenni",     0x310, 0x030   },
    {"Cellular One",              "Cellular",     0x310, 0x040   },
    {"DIGICEL",                   "JAM DC",       0x310, 0x050   },
    {"Highland Cellular",         "Highland",     0x310, 0x070   },
    {"Corr Wireless",             "Corr",         0x310, 0x080   },
    {"Plateau Wireless",          "Plateau",      0x310, 0x100   },
    {"Cingular",                  "Cingular",     0x310, 0x150   },
    {"T-Mobile",                  "T-Mobile",     0x310, 0x160   },
    {"Cingular",                  "Cingular",     0x310, 0x170   },
    {"West Central Wireless",     "West Cen",     0x310, 0x180   },
    {"Alaska Wireless ",          "Alaska W",     0x310, 0x190   },
    {"T-Mobile ",                 "T-Mobile",     0x310, 0x200   },
    {"T-Mobile ",                 "T-Mobile",     0x310, 0x210   },
    {"T-Mobile ",                 "T-Mobile",     0x310, 0x220   },
    {"T-Mobile ",                 "T-Mobile",     0x310, 0x230   },
    {"T-Mobile ",                 "T-Mobile",     0x310, 0x240   },
    {"T-Mobile ",                 "T-Mobile",     0x310, 0x250   },
    {"T-Mobile ",                 "T-Mobile",     0x310, 0x260   },
    {"T-Mobile ",                 "T-Mobile",     0x310, 0x270   },
    {"T-Mobile ",                 "T-Mobile",     0x310, 0x310   },
    {"WestLink Comm",             "WestLink",     0x310, 0x340   },
    {"Carolina Phone",            "Carolina",     0x310, 0x350   },
    {"AT&T Wireless",             "AT&T",         0x310, 0x380   },
    {"Yorkville Telephone",       "Yorkvill",     0x310, 0x390   },
    {"Cingular",                  "Cingular",     0x310, 0x410   },
    {"Cincinnati Bell Wireless",  "CBW",          0x310, 0x420   },
    {"Viaero Wireless",           "Viaero",       0x310, 0x450   },
    {"SunCom",                    "SunCom",       0x310, 0x490   },
    {"West Virginia Wireless",    "WVW",          0x310, 0x530   },
    {"Cellular One DCS",          "Cell One",     0x310, 0x560   },
    {"T-Mobile",                  "T-Mobile",     0x310, 0x580   },
    {"Epic Touch",                "EpicTouc",     0x310, 0x610   },
    {"AmeriLink PCS",             "AmeriLin",     0x310, 0x630   },
    {"Einstein PCS",              "Einstein",     0x310, 0x640   },
    {"T-Mobile",                  "T-Mobile",     0x310, 0x660   },
    {"Wireless 2000 PCS",         "W 2000 P",     0x310, 0x670   },
    {"NPI Wireless",              "NPI",          0x310, 0x680   },
    {"Immix Wireless",            "IMMIX",        0x310, 0x690   },
    {"Telemetrix",                "Telemetr",     0x310, 0x740   },
    {"PTSI",                      "PTSI",         0x310, 0x760   },
    {"i wireless",                "i wirele",     0x310, 0x770   },
    {"AirLink PCS",               "AirLink",      0x310, 0x780   },
    {"Pinpoint",                  "Pinpoint",     0x310, 0x790   },
    {"T-Mobile",                  "T-Mobile",     0x310, 0x800   },
    {"AT&T Wireless",             "AT&T",         0x310, 0x980   },
    {"Sprocket",                  "Sprocket",     0x311, 0x140   },
    {"Blue Sky",                  "Blue Sky",     0x332, 0x11F   },
    {"Telcel GSM",                "TELCEL",       0x334, 0x020   },
    {"C&W",                       "C&W",          0x338, 0x180   },
    {"DIGICEL",                   "DIGICEL",      0x338, 0x05F   },
    {"F-Orange",                  "Orange",       0x340, 0x01F   },
    {"FR",                        "OMT",          0x340, 0x02F   },
    {"AMIGO",                     "AMIGO",        0x340, 0x08F   },
    {"BOUYGTEL-C",                "BOUYG-C",      0x340, 0x20F   },
    {"C&W",                       "C&W",          0x342, 0x600   },
    {"DIGICEL",                   "DIGICEL",      0x342, 0x750   },
    {"Cingular",                  "Cingular",     0x342, 0x810   },
    {"APUA PCS ANTIGUA",          "APUA-PCS",     0x344, 0x030   },
    {"APUA-PCS ANTIGUA",          "APUA-PCS",     0x344, 0x30F   },
    {"C&W",                       "C&W",          0x344, 0x920   },
    {"Cingular",                  "Cingular",     0x344, 0x930   },
    {"C&W",                       "C&W",          0x346, 0x140   },
    {"CCT Boatphone",             "CCTBVI",       0x348, 0x570   },
    {"Cingular",                  "Cingular",     0x350, 0x010   },
    {"BTC MOBILITY LTD.",         "MOBILITY",     0x350, 0x02F   },
    {"Cingular",                  "Cingular",     0x352, 0x030   },
  /*{"DIGICEL",                   "DIGICEL",      0x352, 0x030   },*/
    {"C&W",                       "C&W",          0x352, 0x110   },
    {"C&W",                       "C&W",          0x354, 0x860   },
    {"C&W",                       "C&W",          0x356, 0x110   },
    {"Cingular",                  "Cingular",     0x358, 0x030   },
    {"DIGICEL",                   "DIGICEL",      0x358, 0x050   },
    {"C&W",                       "C&W",          0x358, 0x110   },
    {"Cingular",                  "Cingular",     0x360, 0x010   },
    {"DIGICEL",                   "DIGICEL",      0x360, 0x070   },
    {"C&W",                       "C&W",          0x360, 0x110   },
    {"Telcell GSM",               "Telcell",      0x362, 0x51F   },
    {"Cingular",                  "Cingular",     0x362, 0x630   },
    {"ANT CURACAO TELECOM GSM",   "CT GSM",       0x362, 0x69F   },
    {"ANT",                       "CHIPPIE",      0x362, 0x951   },
    {"SETAR GSM",                 "SETARGSM",     0x363, 0x01F   },
    {"BaTelCell",                 "BaTelCel",     0x364, 0x39F   },
    {"C&W",                       "C&W",          0x365, 0x840   },
    {"Cingular",                  "Cingular",     0x366, 0x020   },
    {"C&W",                       "C&W",          0x366, 0x110   },
    {"CU/C_COM",                  "C_COM",        0x368, 0x01F   },
    {"ORANGE",                    "ORANGE",       0x370, 0x01F   },
    {"TSTT",                      "TSTT",         0x374, 0x12F   },
    {"C&W",                       "C&W",          0x376, 0x350   },
    {"AZE - AZERCELL GSM",        "ACELL",        0x400, 0x01F   },
    {"BAKCELL GSM 2000",          "BKCELL",       0x400, 0x02F   },
    {"KZ K-MOBILE",               "K-MOBILE",     0x401, 0x01F   },
    {"KZ KCELL",                  "KCELL",        0x401, 0x02F   },
    {"BT B-Mobile",               "B-Mobile",     0x402, 0x11F   },
    {"Hutch",                     "Hutch",        0x404, 0x01F   },
    {"AirTel",                    "AirTel",       0x404, 0x02F   },
    {"AirTel",                    "AirTel",       0x404, 0x03F   },
    {"Hutch",                     "Hutch",        0x404, 0x030   },
    {"IDEA",                      "IDEA",         0x404, 0x04F   },
    {"Hutch",                     "Hutch",        0x404, 0x05F   },
    {"IDEA",                      "IDEA",         0x404, 0x07F   },
    {"AirTel",                    "AirTel",       0x404, 0x10F   },
    {"Hutch",                     "Hutch",        0x404, 0x11F   },
    {"IDEA",                      "IDEA",         0x404, 0x12F   },
    {"Hutch",                     "Hutch",        0x404, 0x13F   },
    {"INA SPICE",                 "SPICE",        0x404, 0x14F   },
    {"Hutch",                     "Hutch",        0x404, 0x15F   },
    {"AIRCEL",                    "AIRCEL",       0x404, 0x17F   },
    {"IDEA",                      "IDEA",         0x404, 0x19F   },
    {"Orange",                    "Orange",       0x404, 0x20F   },
    {"BPL MOBILE",                "BPL MOBI",     0x404, 0x21F   },
    {"IDEA",                      "IDEA",         0x404, 0x22F   },
    {"IDEA",                      "IDEA",         0x404, 0x24F   },
    {"AIRCEL",                    "AIRCEL",       0x404, 0x25F   },
    {"BPL MOBILE",                "BPL MOBI",     0x404, 0x27F   },
    {"AIRCEL",                    "AIRCEL",       0x404, 0x28F   },
    {"AIRCEL",                    "AIRCEL",       0x404, 0x29F   },
    {"AirTel",                    "AirTel",       0x404, 0x31F   },
    {"AIRCEL",                    "AIRCEL",       0x404, 0x33F   },
    {"CellOne",                   "CellOne",      0x404, 0x34F   },
    {"AIRCEL",                    "AIRCEL",       0x404, 0x35F   },
    {"AIRCEL",                    "AIRCEL",       0x404, 0x37F   },
    {"CellOne",                   "CellOne",      0x404, 0x38F   },
    {"IND AIRTEL",                "AIRTEL",       0x404, 0x40F   },
    {"INA RPG",                   "RPG",          0x404, 0x41F   },
    {"INA AIRCEL",                "AIRCEL",       0x404, 0x42F   },
    {"BPL MOBILE",                "BPL MOB",      0x404, 0x43F   },
    {"INA SPICE",                 "SPICE",        0x404, 0x44F   },
    {"AirTel",                    "AirTel",       0x404, 0x45F   },
    {"BPL MOBILE",                "BPL MOB",      0x404, 0x46F   },
    {"AirTel",                    "AirTel",       0x404, 0x49F   },
    {"CellOne",                   "CellOne",      0x404, 0x51F   },
    {"CellOne",                   "CellOne",      0x404, 0x53F   },
    {"CellOne",                   "CellOne",      0x404, 0x54F   },
    {"CellOne",                   "CellOne",      0x404, 0x55F   },
    {"IDEA",                      "IDEA",         0x404, 0x56F   },
    {"CellOne",                   "CellOne",      0x404, 0x57F   },
    {"CellOne",                   "CellOne",      0x404, 0x58F   },
    {"CellOne",                   "CellOne",      0x404, 0x59F   },
    {"Hutch",                     "Hutch",        0x404, 0x60F   },
    {"CellOne",                   "CellOne",      0x404, 0x62F   },
    {"CellOne",                   "CellOne",      0x404, 0x64F   },
    {"CellOne",                   "CellOne",      0x404, 0x66F   },
    {"IN-DOLPHIN",                "DOLPHIN",      0x404, 0x68F   },
    {"IN-DOLPHIN",                "DOLPHIN",      0x404, 0x69F   },
    {"INDH1",                     "Oasis",        0x404, 0x70F   },
    {"CellOne",                   "CellOne",      0x404, 0x71F   },
    {"CellOne",                   "CellOne",      0x404, 0x72F   },
    {"CellOne",                   "CellOne",      0x404, 0x73F   },
    {"CellOne",                   "CellOne",      0x404, 0x74F   },
    {"CellOne",                   "CellOne",      0x404, 0x75F   },
    {"CellOne",                   "CellOne",      0x404, 0x76F   },
    {"CellOne",                   "CellOne",      0x404, 0x77F   },
    {"IDEA",                      "IDEA",         0x404, 0x78F   },
    {"CellOne",                   "CellOne",      0x404, 0x79F   },
    {"CellOne",                   "CellOne",      0x404, 0x80F   },
    {"CellOne",                   "CellOne",      0x404, 0x81F   },
    {"Hutch",                     "Hutch",        0x404, 0x84F   },
    {"Hutch",                     "Hutch",        0x404, 0x86F   },
    {"Hutch",                     "Hutch",        0x404, 0x88F   },
    {"AirTel",                    "AirTel",       0x404, 0x90F   },
    {"AirTel",                    "AirTel",       0x404, 0x92F   },
    {"AirTel",                    "AirTel",       0x404, 0x93F   },
    {"AirTel",                    "AirTel",       0x404, 0x94F   },
    {"AirTel",                    "AirTel",       0x404, 0x95F   },
    {"AirTel",                    "AirTel",       0x404, 0x96F   },
    {"AirTel",                    "AirTel",       0x404, 0x97F   },
    {"AirTel",                    "AirTel",       0x404, 0x98F   },
    {"IND AirTel",                "AIRTEL",       0x405, 0x51F   },
    {"IND AirTel",                "AIRTEL",       0x405, 0x52F   },
    {"IND AirTel",                "AIRTEL",       0x405, 0x53F   },
    {"IND AirTel",                "AIRTEL",       0x405, 0x54F   },
    {"IND AirTel",                "AIRTEL",       0x405, 0x55F   },
    {"IND Airtel",                "Airtel",       0x405, 0x56F   },
    {"Hutch",                     "Hutch",        0x405, 0x66F   },
    {"HUTCH",                     "HUTCH",        0x405, 0x67F   },
    {"Mobilink",                  "Mobilink",     0x410, 0x01F   },
    {"PK-UFONE",                  "UFONE",        0x410, 0x03F   },
    {"PAK - PL",                  "PAKTEL",       0x410, 0x04F   },
    {"Telenor PK",                "TELENOR",      0x410, 0x06F   },
    {"WaridTel",                  "WaridTel",     0x410, 0x07F   },
    {"AF AWCC",                   "AWCC",         0x412, 0x01F   },
    {"ROSHAN",                    "ROSHAN",       0x412, 0x20F   },
    {"Mobitel",                   "MOBITEL",      0x413, 0x01F   },
    {"SRI - CELLTEL",             "CELLTEL",      0x413, 0x03F   },
    {"Hutch",                     "Hutch",        0x413, 0x08F   },
    {"SRI DIALOG",                "DIALOG",       0x413, 0x02F   },
    {"MM 900",                    "MPTGSM",       0x414, 0x01F   },
    {"alfa",                      "alfa",         0x415, 0x01F   },
    {"RL MTC Lebanon",            "MTC LIBA",     0x415, 0x03F   },
    {"UMNIAH",                    "UMNIAH",       0x416, 0x03F   },
    {"Fastlink",                  "FSTLNK",       0x416, 0x01F   },
    {"JO MobCom",                 "MobCom",       0x416, 0x77F   },
    {"SYRIATEL",                  "SYRIATEL",     0x417, 0x01F   },
    {"areeba",                    "areeba",       0x417, 0x02F   },
    {"SYR MOBILE SYR",            "MOBILE",       0x417, 0x09F   },
    {"ASIACELL",                  "ASIACELL",     0x418, 0x00F   },
    {"SanaTel",                   "SanaTel",      0x418, 0x02F   },
    {"Atheer Iraq",               "ATHEER",       0x418, 0x20F   },
    {"IRAQNA",                    "IRAQNA",       0x418, 0x30F   },
    {"ASIACELL",                  "ASIACELL",     0x418, 0x05F   },
    {"SanaTel",                   "SanaTel",      0x418, 0x08F   },
    {"KT MTCNet",                 "MTC",          0x419, 0x02F   },
    {"KT WATANIYA",               "WATANIYA",     0x419, 0x03F   },
    {"Etihad Etisalat",           "Etihad E",     0x420, 0x003   },
    {"ALJAWAL",                   "KSA",          0x420, 0x01F   },
    {"SabaFon",                   "SABAFON",      0x421, 0x01F   },
    {"SPACETEL",                  "SPACETEL",     0x421, 0x02F   },
    {"OMAN MOBILE",               "OMAN",         0x422, 0x02F   },
    {"nawras",                    "nawras",       0x422, 0x03F   },
    {"UAE ETISALAT",              "ETSLT",        0x424, 0x02F   },
    {"IL ORANGE",                 "ORANGE",       0x425, 0x01F   },
    {"IL Cellcom",                "Cellcom",      0x425, 0x02F   },
    {"JAWWAL-PALESTINE",          "JAWWAL",       0x425, 0x05F   },
    {"BATELCO",                   "BATELCO",      0x426, 0x01F   },
    {"MTC VODAFONE BH",           "MTC-VFBH",     0x426, 0x02F   },
    {"QAT QATARNET",              "Q-NET",        0x427, 0x01F   },
    {"MN MobiCom",                "MobiCom",      0x428, 0x99F   },
    {"IR-TCI",                    "432 11",       0x432, 0x11F   },
    {"IR KISH",                   "KIFZO",        0x432, 0x14F   },
    {"IR MTCE",                   "MTCE",         0x432, 0x19F   },
    {"IR, VALIACOM",              "VALIACOM",     0x432, 0x32F   },
    {"UZB CSOCOM GSM",            "COSCOM",       0x434, 0x05F   },
    {"UZB-UZD",                   "UZDGSM",       0x434, 0x07F   },
    {"UZB DAEWOO-GSM",            "DW-GSM",       0x434, 0x04F   },
    {"Somoncom",                  "Somoncom",     0x436, 0x01F   },
    {"Indigo-T",                  "INDIGO",       0x436, 0x02F   },
    {"TJK MLT",                   "MLT",          0x436, 0x03F   },
    {"Babilon-M",                 "Babilon",      0x436, 0x04F   },
    {"BITEL KGZ",                 "BITEL",        0x437, 0x01F   },
    {"VN MOBIFONE",               "VMS",          0x452, 0x01F   },
    {"VN VINAPHONE",              "GPC",          0x452, 0x02F   },
    {"VNM and VIETTEL",           "VIETTEL",      0x452, 0x04F   },
    {"HK CSL",                    "CSL",          0x454, 0x00F   },
    {"HK CSL",                    "CSL",          0x454, 0x02F   },
    {"3(2G)",                     "3(2G)",        0x454, 0x04F   },
    {"SmarToneVodafone",          "SMC-Voda",     0x454, 0x06F   },
    {"HK NEW WORLD",              "NWPCS",        0x454, 0x10F   },
    {"HK PEOPLES",                "PEOPLES",      0x454, 0x12F   },
    {"HK SUNDAY",                 "SUNDAY",       0x454, 0x16F   },
    {"HK CSL",                    "CSL",          0x454, 0x18F   },
    {"SmarTone Macau",            "SmarTone",     0x455, 0x00F   },
    {"Hutchison MAC",             "HT Macau",     0x455, 0x03F   },
    {"MAC-CTM",                   "CTM",          0x455, 0x01F   },
    {"MOBITEL - KHM",             "MT-KHM",       0x456, 0x01F   },
    {"CAMBODIA SHINAWATRA",       "CAMSHIN",      0x456, 0x18F   },
    {"KHM-Hello GSM",             "KHM-SM",       0x456, 0x02F   },
    {"ETL MOBILE NETWORK",        "ETLMNW",       0x457, 0x02F   },
    {"45703",                     "LATMOBIL",     0x457, 0x03F   },
    {"TANGO LAO",                 "TANGO",        0x457, 0x08F   },
    {"CHINA  MOBILE",             "CMCC",         0x460, 0x00F   },
    {"CHN-CUGSM",                 "CU-GSM",       0x460, 0x01F   },
    {"Far EasTone",               "FET",          0x466, 0x01F   },
    {"KGT-Online",                "KGT",          0x466, 0x88F   },
    {"Chunghwa",                  "CHT",          0x466, 0x92F   },
    {"TWN MOBITAI",               "TW MOB",       0x466, 0x93F   },
    {"TW Mobile",                 "TWM",          0x466, 0x97F   },
    {"KP SUN",                    "SUNNET",       0x467, 0x03F   },
    {"BGD-GP",                    "GP",           0x470, 0x01F   },
    {"BGD AKTEL",                 "AKTEL",        0x470, 0x02F   },
    {"Banglalink",                "Banglali",     0x470, 0x03F   },
    {"BGD bMobile",               "bMobile",      0x470, 0x04F   },
    {"WMOBILE",                   "WMOBILE",      0x472, 0x002   },
    {"MV DHIMOBILE",              "D-MOBILE",     0x472, 0x01F   },
    {"MY MAXIS",                  "MY MAXIS",     0x502, 0x12F   },
    {"DiGi",                      "DiGi",         0x502, 0x16F   },
    {"MY CELCOM",                 "CELCOM",       0x502, 0x19F   },
    {"Telstra Mobile",            "Telstra",      0x505, 0x01F   },
    {"YES OPTUS",                 "Optus",        0x505, 0x02F   },
    {"vodafone AU",               "voda AU",      0x505, 0x03F   },
    {"IND INDOSAT",               "INDOSAT",      0x510, 0x01F   },
    {"LIPPO TEL",                 "LIPPOTEL",     0x510, 0x08F   },
    {"IND TELKOMSEL",             "T-SEL",        0x510, 0x10F   },
    {"IND - Excelcom",            "proXL",        0x510, 0x11F   },
    {"IND INDOSAT",               "INDOSAT",      0x510, 0x21F   },
    {"TLS-TT",                    "TT",           0x514, 0x02F   },
    {"Globe Telecom-PH",          "GLOBE",        0x515, 0x02F   },
    {"SMART",                     "SMART",        0x515, 0x03F   },
    {"PH Sun Cellular",           "SUN",          0x515, 0x05F   },
    {"ISLACOM",                   "ISLACOM",      0x515, 0x01F   },
    {"TH GSM",                    "TH GSM",       0x520, 0x01F   },
    {"TH ACT 1900",               "ACT-1900",     0x520, 0x15F   },
    {"TH-DTAC",                   "DTAC",         0x520, 0x18F   },
    {"TH GSM 1800",               "GSM 1800",     0x520, 0x23F   },
    {"TH True",                   "True",         0x520, 0x99F   },
    {"SingTel",                   "SingTel",      0x525, 0x01F   },
    {"SingTel-G18",               "SingTel",      0x525, 0x02F   },
    {"SGP-M1-3GSM",               "M1-3GSM",      0x525, 0x03F   },
    {"STARHUB-SGP",               "STARHUB",      0x525, 0x05F   },
    {"BRU-DSTCom",                "DSTCom",       0x528, 0x11F   },
    {"vodafone NZ",               "voda NZ",      0x530, 0x01F   },
    {"PNGBMobile",                "BMobile",      0x537, 0x01F   },
    {"U-CALL",                    "U-CALL",       0x539, 0x01F   },
    {"VUT SMILE",                 "SMILE",        0x541, 0x01F   },
    {"FJ VODAFONE",               "VODAFONE",     0x542, 0x01F   },
    {"Blue Sky",                  "Blue Sky",     0x544, 0x11F   },
    {"KL-Frigate",                "KI-FRIG",      0x545, 0x09F   },
    {"NCL MOBILIS",               "MOBNCL",       0x546, 0x01F   },
    {"F-VINI",                    "VINI",         0x547, 0x20F   },
    {"CK KOKANET",                "KOKANET",      0x548, 0x01F   },
    {"FSM Telecom",               "FSMTC",        0x550, 0x01F   },
    {"vodafone EG",               "voda EG",      0x602, 0x02F   },
    {"EGY MobiNiL",               "MobiNiL",      0x602, 0x01F   },
    {"ALG Mobilis",               "Mobilis",      0x603, 0x01F   },
    {"Djezzy",                    "Djezzy",       0x603, 0x02F   },
    {"DZA NEDJMA",                "NEDJMA",       0x603, 0x03F   },
    {"MOR MEDITEL",               "MEDITEL",      0x604, 0x00F   },
    {"MOR IAM",                   "IAM",          0x604, 0x01F   },
    {"TUNISIANA",                 "TUNSIANA",     0x605, 0x03F   },
    {"TUNISIE TELECOM",           "TUNTEL",       0x605, 0x02F   },
    {"LIBYANA",                   "LIBYANA",      0x606, 0x00F   },
    {"GAMCEL",                    "GAMCEL",       0x607, 0x01F   },
    {"SN ALIZE",                  "ALIZE",        0x608, 0x01F   },
    {"SN-SENTEL SG",              "SENTEL",       0x608, 0x02F   },
    {"MR MATTEL",                 "MATTEL",       0x609, 0x01F   },
    {"MALITEL ML",                "MALITEL",      0x610, 0x01F   },
    {"IKATEL ML",                 "IKATEL",       0x610, 0x02F   },
    {"GN LAGUI",                  "LAGUI",        0x611, 0x02F   },
    {"Orange CI",                 "Orange",       0x612, 0x03F   },
    {"TELECEL-CI",                "TELCEL",       0x612, 0x05F   },
    {"BF Celtel",                 "celtel",       0x613, 0x02F   },
    {"SAHELCOM",                  "SAHELCOM",     0x614, 0x01F   },
    {"CELTEL",                    "CELTEL",       0x614, 0x02F   },
    {"NE TELECEL",                "TELECEL",      0x614, 0x03F   },
    {"TG-TOGO CELL",              "TGCELL",       0x615, 0x01F   },
    {"BELL BENIN COMMUNICATION",  "BBCOM",        0x616, 0x004   },
    {"TELECEL BENIN",             "TLCL-BEN",     0x616, 0x02F   },
    {"BJ BENINCELL",              "BENCELL",      0x616, 0x03F   },
    {"CELLPLUS-MRU",              "CELL +",       0x617, 0x01F   },
    {"EMTEL-MRU",                 "EMTEL",        0x617, 0x10F   },
    {"LBR Lonestar Cell",         "LoneStar",     0x618, 0x01F   },
    {"LIBERCELL",                 "LIBERCEL",     0x618, 0x02F   },
    {"Celcom GSM",                "Celcom",       0x618, 0x03F   },
    {"CELTEL SL",                 "CELTEL",       0x619, 0x01F   },
    {"MILLICOM SL",               "MILLICOM",     0x619, 0x02F   },
    {"GH SPACEFON",               "SPACE",        0x620, 0x01F   },
    {"GH-MOBITEL",                "mobitel",      0x620, 0x03F   },
    {"GH ONEtouch",               "ONEtouch",     0x620, 0x02F   },
    {"MTN - NG",                  "MTN-NG",       0x621, 0x30F   },
    {"NG Mtel",                   "Mtel",         0x621, 0x40F   },
    {"Glo NG",                    "glo",          0x621, 0x50F   },
    {"CELTEL TCD",                "CELTEL",       0x622, 0x01F   },
    {"MTN CAM",                   "62401",        0x624, 0x01F   },
    {"Orange CAM",                "Orange",       0x624, 0x02F   },
    {"CPV MOVEL",                 "CMOVEL",       0x625, 0x01F   },
    {"STP CSTmovel",              "CSTmovel",     0x626, 0x01F   },
    {"GNQ01",                     "GETESA",       0x627, 0x01F   },
    {"628 01/LIBERTIS",           "LIBERTIS",     0x628, 0x01F   },
    {"GAB TELECEL",               "TELECEL",      0x628, 0x02F   },
    {"CELTEL GA",                 "CELTEL",       0x628, 0x03F   },
    {"CELTEL",                    "CELTEL R",     0x629, 0x01F   },
    {"COG LIBERTIS",              "LIBERTIS",     0x629, 0x10F   },
    {"VODACOM CD",                "VODACOM",      0x630, 0x01F   },
    {"CELTEL DRC",                "CELTEL",       0x630, 0x02F   },
    {"CD OASIS",                  "OASIS",        0x630, 0x89F   },
    {"UNITEL",                    "UNITEL",       0x631, 0x02F   },
    {"GTM",                       "GTM",          0x632, 0x07F   },
    {"SEYCEL",                    "633-01",       0x633, 0x01F   },
    {"SC SmartCom",               "SmartC",       0x633, 0x02F   },
    {"SEZ AIRTEL",                "AIRTEL",       0x633, 0x10F   },
    {"MobiTel SDN",               "MobiTel",      0x634, 0x01F   },
    {"areeba SDN",                "areeba",       0x634, 0x02F   },
    {"R-CELL",                    "RCELL",        0x635, 0x10F   },
    {"ETH-MTN",                   "ET-MTN",       0x636, 0x01F   },
    {"SOMTELESOM",                "TELESOM",      0x637, 0x01F   },
    {"SOMAFONE",                  "SOMAFONE",     0x637, 0x04F   },
    {"Som Golis",                 "Golis",        0x637, 0x30F   },
    {"DJ EVATIS",                 "EVATIS",       0x638, 0x01F   },
    {"Safaricom",                 "SAF-COM",      0x639, 0x02F   },
    {"CELTEL",                    "CELTEL",       0x639, 0x03F   },
    {"MOBITEL - TZ",              "MOBITEL",      0x640, 0x02F   },
    {"VodaCom",                   "VodaCom",      0x640, 0x04F   },
    {"celtel",                    "celtel",       0x640, 0x05F   },
    {"ZANTEL-TZ",                 "ZANTEL",       0x640, 0x03F   },
    {"UG CelTel",                 "CELTEL",       0x641, 0x01F   },
    {"MTN-UGANDA",                "MTN-UG",       0x641, 0x10F   },
    {"UTL-Mango",                 "UTL-Mang",     0x641, 0x11F   },
    {"Spacetel BI",               "SPACETEL",     0x642, 0x01F   },
    {"ONATEL BDI",                "ONATEL",       0x642, 0x03F   },
    {"BUSAFA",                    "SAFARIS",      0x642, 0x02F   },
    {"TELECEL-BDI",               "BDITL",        0x642, 0x82F   },
    {"MOZ - mCel",                "mCel",         0x643, 0x01F   },
    {"VodaCom-MZ",                "VodaCom",      0x643, 0x04F   },
    {"ZM CELTEL",                 "CELTEL",       0x645, 0x01F   },
    {"TELECEL ZM",                "TELECEL",      0x645, 0x02F   },
    {"MG Madacom",                "Madacom",      0x646, 0x01F   },
    {"MG ANTARIS",                "ANTARIS",      0x646, 0x02F   },
    {"Orange re",                 "Orange",       0x647, 0x00F   },
    {"SFR REUNION",               "SFR RU",       0x647, 0x10F   },
    {"ZW NET*ONE",                "NETONE",       0x648, 0x01F   },
    {"TELECEL ZW",                "TELECEL",      0x648, 0x03F   },
    {"ZW ECONET",                 "ECONET",       0x648, 0x04F   },
    {"MTC NAMIBIA",               "MTCNAM",       0x649, 0x01F   },
    {"MW CP 900",                 "CP 900",       0x650, 0x01F   },
    {"CELTEL MW",                 "CELTEL",       0x650, 0x10F   },
    {"Vodacom Lesotho",           "Vodacom-",     0x651, 0x01F   },
    {"LS-ECONET-EZI-CEL",         "EZI-CEL",      0x651, 0x02F   },
    {"BW MASCOM",                 "MASCOM",       0x652, 0x01F   },
    {"Orange",                    "Orange",       0x652, 0x02F   },
    {"Swazi-MTN",                 "SwaziMTN",     0x653, 0x10F   },
    {"HURI",                      "HURI",         0x654, 0x001   },
    {"VodaCom-SA",                "VODA",         0x655, 0x01F   },
    {"Cell C",                    "Cell C",       0x655, 0x07F   },
    {"MTN-SA",                    "MTN",          0x655, 0x10F   },
    {"BTL",                       "BTL",          0x702, 0x67F   },
    {"PCS",                       "PCS",          0x704, 0x01F   },
    {"Comcel_GSM",                "COMCEL",       0x704, 0x02F   },
    {"movistar",                  "movistar",     0x704, 0x03F   },
    {"ESV PERSONAL",              "PERSONAL",     0x706, 0x01F   },
    {"Digicel",                   "DIGICEL",      0x706, 0x02F   },
    {"TELEMOVIL",                 "TM",           0x706, 0x03F   },
    {"movistar",                  "movistar",     0x706, 0x04F   },
    {"ESV PERSONAL",              "PERSONAL",     0x706, 0x10F   },
    {"Megatel GSM",               "ALOHN",        0x708, 0x001   },
    {"CELTELHND",                 "CELTEL",       0x708, 0x02F   },
    {"HND",                       "HT - 200",     0x708, 0x030   },
    {"ENITEL GSM",                "ENITEL",       0x710, 0x21F   },
    {"SERCOM",                    "SERCOM",       0x710, 0x73F   },
    {"I.C.E.",                    "I.C.E.",       0x712, 0x01F   },
    {"PANCW",                     "PANCW",        0x714, 0x01F   },
    {"TIM PERU",                  "TIM",          0x716, 0x10F   },
    {"PERSONAL",                  "AR TP",        0x722, 0x34F   },
    {"TIM BRASIL",                "TIM",          0x724, 0x02F   },
    {"TIM BRASIL",                "TIM",          0x724, 0x03F   },
    {"TIM BRASIL",                "TIM",          0x724, 0x04F   },
    {"Claro",                     "Claro",        0x724, 0x05F   },
    {"BRA SCTL",                  "SCTL",         0x724, 0x15F   },
    {"BRA BrTCelular",            "BrTCel",       0x724, 0x16F   },
    {"TELEMIG CEL",               "TELEMIGC",     0x724, 0x23F   },
    {"AMAZONIA",                  "AMAZONIA",     0x724, 0x24F   },
    {"Oi",                        "Oi",           0x724, 0x31F   },
    {"CTBC CEL",                  "CTBC",         0x724, 0x32F   },
    {"CTBC CEL",                  "CTBC",         0x724, 0x33F   },
    {"CTBC CEL",                  "CTBC",         0x724, 0x34F   },
    {"ENTEL PCS",                 "ENTEL PC",     0x730, 0x01F   },
    {"movistar",                  "movistar",     0x730, 0x02F   },
    {"CL ENTEL PCS",              "ENTEL",        0x730, 0x01F   },
    {"ENTEL PCS",                 "ENTEL PC",     0x730, 0x10F   },
    {"COLOMBIA - COMCEL S.A",     "COMCEL",       0x732, 0x101   },
    {"OLA",                       "OLA",          0x732, 0x111   },
    {"VZ INFO",                   "INFONT",       0x734, 0x01F   },
    {"DIGITEL TIM",               "DIGITEL",      0x734, 0x02F   },
    {"NUEVATEL",                  "VIVA",         0x736, 0x01F   },
    {"EMOVIL",                    "BOMOV",        0x736, 0x02F   },
    {"GUY TW",                    "TWTGUY",       0x738, 0x001   },
    {"GUY CLNK PLS",              "CLNK PLS",     0x738, 0x002   },
    {"MOVISTAR",                  "MOVISTAR",     0x740, 0x00F   },
    {"PORTA GSM",                 "PORTAGSM",     0x740, 0x01F   },
    {" HOLA PARAGUAY",            "VOX",          0x744, 0x01F   },
    {"PRY Porthable",             "Porth",        0x744, 0x02F   },
    {"Telecel GSM",               "Telecel",      0x744, 0x04F   },
    {"PY Personal",               "Personal",     0x744, 0x05F   },
    {"SR.TELESUR.GSM",            "TeleG",        0x746, 0x02F   },
    {"MOVISTAR",                  "Movistar",     0x748, 0x07F   },
    {"CTI Uruguay",               "CTIURY",       0x748, 0x10F   },
    {"Altyn Asyr",                "TM Cell",      0x795, 0x02F   },
    {"MCP Maritime Com",          "MCP",          0x901, 0x12F   },
#endif
/*--- EOL -------------------------------------------------------*/
    { NULL,                       NULL,                -1,-1   }
};
#else

EXTERN const T_OPER_NTRY_FIXED operListFixed[];

#endif      /* of #ifdef CMH_MMF_C */

#endif      /* TI_PS_OP_OPN_TAB_ROMBASED */

#endif
