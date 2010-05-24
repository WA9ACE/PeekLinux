
#ifndef _PSWNAM_H_

#define _PSWNAM_H_ 1

/*****************************************************************************
 * Includes
 ****************************************************************************/
#include "sysdefs.h"
#include "pswcustom.h"
#include "hlwapi.h"

/*****************************************************************************
 * Definitions for use with NAM
 ****************************************************************************/
  #define NUM_BANDS_SUPPORTED 5     /* 4 bands supported; Cellular, PCS */
                                    /* skip, T53, Korean PCS            */

                                      
  #define CP_MAX_TMSI_ZONE_LEN  8
  #define MN_PASSWD_MAX_SIZE    16
  #define MN_NAI_MAX_SIZE       72

  #define NAM_MEID_SIZE         7    /* size of MEID */
  #define NAM_CST_SIZE  3 /*size of CST 6f32*/

  #define REMAINING_NAM_SIZE    1

  #define NAM_PREF_MODE_ANALOG        0
  #define NAM_PREF_MODE_CDMA          1
  #define NAM_PREF_MODE_ANALOG_ONLY   2
  #define NAM_PREF_MODE_CDMA_ONLY     3

/* Bit Map of some Nam Boolean Fields to save memory */
  #define IMSI_M_VALID          0x01   /* Indicates if the IMSI_M is programed */
  #define IMSI_T_VALID          0x02   /* Indicates if the IMSI_T is programed */

  #define OTA_ALLOW_OTAPA       0x02   /* User flag to reject network OTAPA attemps */
  #define AUTH_AKEY_VALID       0x01   /* Akey Valid. OK for Authentication */
  #define AUTH_ENABLE_VP        0x02   /* Voice Privacy Enabled */
  #define MOB_TERM_HOME_ENABLED 0x01   /* receive mobile terminated calls in home system */
  #define MOB_TERM_SID_ENABLED  0x02   /* receive mobile terminated calls in foreign SID system */
  #define MOB_TERM_NID_ENABLED  0x04   /* receive mobile terminated calls in foreign NID system */

  #define NAM_MAX_MDN_DIGITS    16     /* max MDN digit number in NAM template */
/*****************************************************************************
 * Registration structures
 ****************************************************************************/
typedef enum
{ 
    BAND_CLASS_0 = 0,   /* 800 MHz cellular band       */
    BAND_CLASS_1,       /* 1.8 to 2.0 GHz PCS band     */
    BAND_CLASS_2,       /* 872 to 960 MHz TACS band    */
    BAND_CLASS_3,       /* 832 to 925 MHz JTACS band   */
    BAND_CLASS_4,       /* 1.75 to 1.87 GHz Korean PCS */
    BAND_CLASS_5,       /* 450 MHz NMT band            */
    BAND_CLASS_6,       /* 2 GHz IMT-2000 band         */
    BAND_CLASS_7,       /* 700 MHz band                */
    BAND_CLASS_8,       /* 1800 MHz band               */
    BAND_CLASS_9,       /* 900 MHz band                */
    BAND_CLASS_10,      /* Secondary 800 MHz NMT band  */
    BAND_CLASS_NOT_USED
} BandClass;

/* Zone based registration list */
  typedef PACKED struct
    {
      uint16    regZone;      /* registration zone  */
      uint16    sid;          /* system id */
      uint16    nid;          /* network id */
      uint16    ageTimer;     /* age limit in seconds */
      bool      timerEnabled;
      uint8     block;        /* PCS block or serving system */
      BandClass bandClass;
    } ZoneList;

  /* System/Network registration list */
  typedef PACKED struct
    {
      uint16    sid;          /* system id */
      uint16    nid;          /* network id */
      uint16    ageTimer;     /* age limit in seconds */
      bool      timerEnabled;
      uint8     block;        /* PCS block or serving system */
      BandClass bandClass; 
    } SidNidList;

/*****************************************************************************
 * IMSI structure definitions
 ****************************************************************************/

  /* An IMSI is <= 15 digits in length. It is composed of:           */
  /*     mcc (3 digits) + nmsi (up to 12 digits)                     */

  /* An NMSI is <= 12 digits in length. Is is composed of:           */
  /*     imsi_11_12 (2 digits) + imsi_s (10 digits)  */
  /* In turn, imsi_s is composed of 2 parts:                         */
  /*    imsi_s1 (upper 3 digits) + imsi_s2 (lower 7 digits)          */

  /* If the imsi is Class 0 then imsiClass must be set to            */
  /*  CP_IMSI_CLASS_0 and addrNum is set to 8                        */
  /* If the imsi is Class 1 then imsiClass must be set to            */
  /*  CP_IMSI_CLASS_1 and addrNum is set to # of digits in NMSI - 4  */

  typedef PACKED struct
    {
      uint16 mcc;                  /* Mobile Country Code                */
      uint8  imsi_11_12;           /* 7 bit  IMSI_11_12                  */
      uint16 imsi_s2;              /* 10 bit IMSI_S2 value               */
      uint32 imsi_s1;              /* 24 bit IMSI_S1 value               */
      uint8  imsiClass;            /* CP_IMSI_CLASS_0/1 indication       */
      uint8  addrNum;              /* number of digits in NMSI - 4       */
    } IMSIType;

/*****************************************************************************
 * IS-95 NAM structure
 ****************************************************************************/
  typedef enum
    {
      NAM_1 = 1,
      NAM_2
    } NamNumber;

  typedef PACKED struct
    {
      /* Permanent Mobile Station Indicators                                */
      uint32     ESN;            /* Electronic Serial Number                */
      uint8      SCMp[NUM_BANDS_SUPPORTED];
                                /* Station Class Mark for each band         */
      uint8      SLOT_CYCLE_INDEXp; /* slot cycle index                     */
      uint16     MOB_FIRM_REVp;  /* mobile firmware revision                */
      uint8      MOB_MODELp;     /* Manufacturers  model number             */
      uint8      MOB_P_REVp[NUM_BANDS_SUPPORTED];
                                /* Protocol revision number for each band   */

      /* Semi-Permanent Mobile Station Indicators                           */
      bool      ValidZoneEntry; /* indicates a valid ZONE_LIST entry        */
      ZoneList  ZONE_LISTsp;    /* zone based registration list             */
      bool      ValidSidNidEntry; /* indicates a valid SID_NID_LISTsp entry */
      SidNidList SID_NID_LISTsp;  /* System/Network registration list       */
      int32     BASE_LAT_REGsp; /* latitude from base last registered on    */
      int32     BASE_LONG_REGsp;/* longitude from base last registered on   */
      uint16    REG_DIST_REGsp; /* registration distance from last base     */
      uint8     LCKRSN_Psp;     /* lock code reason                         */
      uint8     MAINTRSNsp;     /* maintenance reason                       */
      bool      DIGITAL_REGsp;  /* previous registration on digtal system   */

      /* NAM indicators                                                     */
      uint8     PREF_BANDp;     /* preferred band; Cellular or PCS          */
      uint8     PREF_MODEp;     /* preferred mode; analog or CDMA           */
      uint8     PREF_BLOCK_BAND1p;/* preferred CDMA system; block designator*/
      uint8     PREF_SERV_BAND0p; /* preferred CDMA/AMPS system (A or B)    */
      uint32    A_Key[2];        /* Binary Authentication Key for CAVE       */
      uint8     SSDA[8];         /* Shared Secret Data 'A'                   */
      uint8     SSDB[8];         /* Shared Secret Data 'B'                   */
      uint8     COUNTsp;         /* Call History Parameter                   */
      uint8     ValidIMSImap;    /* Refer to Bit Map text at top of file     */ 
      IMSIType  IMSI_Mp;         /* IMSI_M - min                             */
      IMSIType  IMSI_Tp;         /* IMSI_T                                   */
      uint8     ASSIGNING_TMSI_ZONE_LENsp;
      uint8     ASSIGNING_TMSI_ZONEsp[ CP_MAX_TMSI_ZONE_LEN ];
                                 /*   assigning tmsi zone                    */
      uint32    TMSI_CODEsp;     /* tmsi code                                */
      uint32    TMSI_EXP_TIMEsp; /* tmsi expiration timer                    */
      uint16    SIDp[MAX_POSITIVE_SIDS]; /* home system ids stored in NAM    */
      uint16    NIDp[MAX_POSITIVE_SIDS]; /* home network ids stored in NAM   */
      uint16    NEG_SIDp[MAX_NEGATIVE_SIDS]; /* negative system ids stored in NAM */
      uint16    NEG_NIDp[MAX_NEGATIVE_SIDS]; /* negative network ids stored in NAM */
      uint8     MAX_SID_NID;     /* Max sid/nid pairs that can be stored in NAM*/
      uint8     STORED_POS_SID_NID;  /* No of Pos sid/nid pairs stored in NAM        */
      uint8     STORED_NEG_SID_NID;   /* No of Neg sid/nid pairs stored in NAM*/
      uint8     ACCOLCp;         /* access overload class                    */
      uint8     MobTermMap;      /* receive mobile terminated calls          */
      uint8     BCAST_ADDR_LENp; /* broadcast address length (0-not config.) */
      uint8     BCAST_ADDRp[ CP_BCAST_ADDRESS_MAX_SIZE];
                                 /* broadcast address data                   */
      uint32    SPCp;            /* Service Programing Code                  */
      uint8     OTA_Capability_Map; /* See Bit Map explanation at top of file */
      uint8     MDN_NUM_DIGITS;
      uint8     Mdn[NAM_MAX_MDN_DIGITS];  /* MDN - Mobile directory number  */

      /* Analog support */
      uint16    HOME_SIDp;       /* home sid for analog system support       */
      uint8     EXp;             /* extended address support for analog      */
      uint16    FIRSTCHPp;       /* first paging channel                     */
      uint8     DTXp;            /* Analog DTX Option                        */
      uint16    FCCA, LCCA;      /* Analog Custom Control Channel Set A      */
      uint16    FCCB, LCCB;      /* Custom Control Channel Set B             */
      uint32    NXTREGsp;
      uint16    SIDsp;
      uint16    LOCAIDsp;
      bool      PUREGsp;

      uint16 CPCA, CSCA;         /* CDMA Primary (and Secondary) Channel A */
      uint16 CPCB, CSCB;         /* CDMA Primary (and Secondary) Channel B */

	  /*Mobile IP Parms */
	   uint32    MN_HOME_IP_ADDR;
	   uint32    HA_PRI_IP_ADDR;
	   uint32    HA_SEC_IP_ADDR;
	   uint8     MN_NAI[MN_NAI_MAX_SIZE];
	   uint8     MN_AAA_PASSWORD[MN_PASSWD_MAX_SIZE];
	   uint8	    MN_HA_PASSWORD[MN_PASSWD_MAX_SIZE];
	   uint8     MN_REV_TUNNELING;
	   uint32    MN_HA_SPI;
	   uint32    MN_AAA_SPI;

      uint8     Auth_Capability_Map; /* See Bit Map Explination at top of file */

      /* IS 801.1 Configuration */
      bool	    Is801_OverTcp;
      uint32    Is801_IpAddr;
      uint16    Is801_PortNum;
      uint8     Is801_UserId[HLW_MAX_USRID_LEN];
      uint8     Is801_Pswd[HLW_MAX_PSWD_LEN];
      uint8     Is801_CalledNumber[HLW_MAX_BWSR_DIG_LEN];
      bool      Is801_RlpChannel;

      uint8		 SmsTlMaxRetry;
      uint32    UIM_ID;

      bool      HOME_ONLY;       /* Only Home Systems allowed or not */

      uint8     MEID[NAM_MEID_SIZE];


      /* Following array reserved for future use. If new Nam parameter 
       * needed then decrement from array below. Total Nam size is always 
       * 560 bytes. */
	   uint8		 FUTURE_EXPAND_FIELDs[REMAINING_NAM_SIZE];	

  /* Checksum support */
      uint16    Checksum;            /* checksum for checksum support  */

    } PswIs95NamT;

#endif  /* _NAMDATA_H_ */

