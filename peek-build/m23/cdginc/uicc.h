/*
+--------------------------------------------------------------------------+
| PROJECT : PROTOCOL STACK                                                 |
| FILE    : uicc.h                                                         |
| SOURCE  : "__out__\LNX_TI_cgt2_70\rel_n5_camera_micron\cdginc\condat\ms\doc\sap\8010_136_SIMDRV_SAP.pdf"                                 |
| LastModified : "2004-06-10"                                              |
| IdAndVersion : "8010.136.03.009"                                         |
| SrcFileTime  : "Thu Nov 29 09:27:56 2007"                                |
| Generated by CCDGEN_2.5.5A on Wed Mar 11 09:58:58 2009                   |
|           !!DO NOT MODIFY!!DO NOT MODIFY!!DO NOT MODIFY!!                |
+--------------------------------------------------------------------------+
*/

/* PRAGMAS
 * PREFIX                 : SIMDRV
 * COMPATIBILITY_DEFINES  : NO
 * ALWAYS_ENUM_IN_VAL_FILE: YES
 * ENABLE_GROUP: YES
 * CAPITALIZE_TYPENAME: NO
 */

#ifndef UICC_INCLUDE
#define UICC_INCLUDE


#define PUB_L1_MISC_ENTER__Uicc_H

#define PUB_L1_MISC_ENTER__FILENAME _Uicc_H
#define PUB_L1_MISC_ENTER__Uicc_H__FILE_TYPE CDGINC
#define PUB_L1_MISC_ENTER__Uicc_H__LAST_MODIFIED _2004_06_10
#define PUB_L1_MISC_ENTER__Uicc_H__ID_AND_VERSION _8010_136_03_009

#define PUB_L1_MISC_ENTER__Uicc_H__SRC_FILE_TIME _Thu_Nov_29_09_27_56_2007

#include "pub_L1_misc_enter.h"

#undef PUB_L1_MISC_ENTER__Uicc_H

#undef PUB_L1_MISC_ENTER__FILENAME


/*
 * Enum to value table VAL_reset_return_val
 * CCDGEN:WriteEnum_Count==30
 */
#ifndef UICC_VAL_RESET_RETURN_VAL_CLASS
#define UICC_VAL_RESET_RETURN_VAL_CLASS
enum 
{
  UICC_SIM_INSERTED              = 0x0,           /* A SIM is inserted and ATR/PPS was successful */
  UICC_SIM_NOT_INSERTED          = 0x1,           /* No SIM inserted (hardware detected) */
  UICC_INVALID_CARD              = 0x2,           /* Card is not responding or gives unintelligible answers, communications time out. */
  UICC_ME_FAILURE                = 0x3,           /* Power management related problems, e.g. voltage selection */
  UICC_IMPROPER_CALL_BACK        = 0x4,           /* simdrv_register function called with pointer improperly set, e.g. NULL. */
  UICC_ME_READER_NOT_AVAILABLE   = 0x5            /* The reader requested is not available */
};
typedef Uint8 Uicc_ValResetReturnVal;
#endif /* UICC_VAL_RESET_RETURN_VAL_CLASS */

/*
 * Enum to value table VAL_len
 * CCDGEN:WriteEnum_Count==32
 */
#ifndef UICC_VAL_LEN_CLASS
#define UICC_VAL_LEN_CLASS
enum 
{
  UICC_VAL_LEN__RANGE_MIN        = 0x0,           /* minimum                        */
  UICC_VAL_LEN__RANGE_MAX        = 0x100,         /* maximum                        */
  UICC_LENGTH_UNKNOWN            = 0xffff         /* Indicating that the length expected is unknown. */
};
typedef Uint16 Uicc_ValLen;
#endif /* UICC_VAL_LEN_CLASS */

/*
 * Enum to value table VAL_sw1_2
 * CCDGEN:WriteEnum_Count==34
 */
#ifndef UICC_VAL_SW_1_2_CLASS
#define UICC_VAL_SW_1_2_CLASS
enum 
{
  UICC_ERR_NOCARD                = 0x1,           /* No SIM inserted (hardware detected) */
  UICC_ERR_NOT_RESET             = 0x2,           /* A reset has not been performed on the driver with the requested Id. */
  UICC_ERR_ME_FAIL               = 0x3,           /* Unrecoverable ME failure (for instance interrupt fails to occur) */
  UICC_ERR_RETRY_FAIL            = 0x4,           /* ME/SIM communication failed after certain retries, SIM reset required */
  UICC_ERR_PARAM_WRONG           = 0x5            /* A driver function is called with invalid parameters */
};
typedef Uint8 Uicc_ValSw1_2;
#endif /* UICC_VAL_SW_1_2_CLASS */

/*
 * Enum to value table VAL_cla
 * CCDGEN:WriteEnum_Count==36
 */
#ifndef UICC_VAL_CLA_CLASS
#define UICC_VAL_CLA_CLASS
enum 
{
  UICC_GSM_CLASS_BYTE            = 0xa0,          /* GSM Class byte according to [GSM 11.11 / 3GPP 51.011] */
  UICC_UMTS_CLASS_BYTE           = 0x80,          /* UMTS Class byte according to [ETSI 102 221 10.1.2] */
  UICC_UICC_CLASS_BYTE           = 0x0            /* UICC Class byte according to [ISO 7816-4] */
};
typedef Uint8 Uicc_ValCla;
#endif /* UICC_VAL_CLA_CLASS */

/*
 * Enum to value table VAL_ins
 * CCDGEN:WriteEnum_Count==38
 */
#ifndef UICC_VAL_INS_CLASS
#define UICC_VAL_INS_CLASS
enum 
{
  UICC_INS_SELECT                = 0xa4,          /* Select Instruction             */
  UICC_INS_STATUS                = 0xf2,          /* Status Instruction             */
  UICC_INS_READ_BINARY           = 0xb0,          /* Read Binary Instruction        */
  UICC_INS_UPDATE_BINARY         = 0xd6,          /* Update Binary Instruction      */
  UICC_INS_READ_RECORD           = 0xb2,          /* Read Record Instruction        */
  UICC_INS_UPDATE_RECORD         = 0xdc,          /* Update Record Instruction      */
  UICC_INS_SEEK                  = 0xa2,          /* Seek Instruction               */
  UICC_INS_INCREASE              = 0x32,          /* Increase Instruction           */
  UICC_INS_VERIFY_CHV            = 0x20,          /* Verify Chv Instruction         */
  UICC_INS_CHANGE_CHV            = 0x24,          /* Change Chv Instruction         */
  UICC_INS_DISABLE_CHV           = 0x26,          /* Disable Chv Instruction        */
  UICC_INS_ENABLE_CHV            = 0x28,          /* Ensable Chv Instruction        */
  UICC_INS_UNBLOCK_CHV           = 0x2c,          /* Unblock Chv Instruction        */
  UICC_INS_INVALIDATE            = 0x4,           /* Invalidate Instruction         */
  UICC_INS_REHABILITATE          = 0x44,          /* Rehabilitate Instruction       */
  UICC_INS_AUTHENTICATE          = 0x88,          /* Run GSM Algorithm (2/2.5G) / Authenticate (3G) Instruction */
  UICC_INS_TERMINAL_PROFILE      = 0x10,          /* Terminal Profile Instruction   */
  UICC_INS_ENVELOPE              = 0xc2,          /* Envelope Instruction           */
  UICC_INS_FETCH                 = 0x12,          /* Fetch Instruction              */
  UICC_INS_TERMINAL_RESPONSE     = 0x14,          /* Terminal Response Instruction  */
  UICC_INS_GET_RESPONSE          = 0xc0           /* Get Response Instruction       */
};
typedef Uint8 Uicc_ValIns;
#endif /* UICC_VAL_INS_CLASS */

/*
 * Enum to value table VAL_voltage_select
 * CCDGEN:WriteEnum_Count==40
 */
#ifndef UICC_VAL_VOLTAGE_SELECT_CLASS
#define UICC_VAL_VOLTAGE_SELECT_CLASS
enum 
{
  UICC_REQ_VOLTAGE_SEL           = 0x0,           /* The SIM reader driver shall perform voltage selection */
  UICC_OMIT_VOLTAGE_SEL          = 0x1            /* The SIM reader driver shall restart with the current voltage */
};
typedef Uint8 Uicc_ValVoltageSelect;
#endif /* UICC_VAL_VOLTAGE_SELECT_CLASS */

/*
 * Enum to value table VAL_config_requested
 * CCDGEN:WriteEnum_Count==42
 */
#ifndef UICC_VAL_CONFIG_REQUESTED_CLASS
#define UICC_VAL_CONFIG_REQUESTED_CLASS
enum 
{
  UICC_OMIT_CONFIG_CHARACTERISTICS = 0x0,         /* PS Shall not retrieve Configuration Characteristics */
  UICC_REQUEST_CONFIG_CHARACTERISTICS = 0x1       /* PS Shall retrieve Configuration Characteristics */
};
typedef Uint8 Uicc_ValConfigRequested;
#endif /* UICC_VAL_CONFIG_REQUESTED_CLASS */

/*
 * Enum to value table VAL_reader_id
 * CCDGEN:WriteEnum_Count==44
 */
#ifndef UICC_VAL_READER_ID_CLASS
#define UICC_VAL_READER_ID_CLASS
enum 
{
  UICC_VAL_READER_ID__RANGE_MIN  = 0x1,           /* minimum                        */
  UICC_VAL_READER_ID__RANGE_MAX  = 0x2            /* maximum                        */
};
typedef Uint8 Uicc_ValReaderId;
#endif /* UICC_VAL_READER_ID_CLASS */
#define UICC_SIZE_ATR_INFO             (0x21)     
#ifndef UICC_ATR_STRING_INFO_CLASS
#define UICC_ATR_STRING_INFO_CLASS
/*
 * SIM Card Info
 * CCDGEN:WriteStruct_Count==1284
 */
typedef struct
{
  Uint8                     atrStringArrayCount;      /*<  0:  1> counter                                            */
  Uint8                     atrStringArray[UICC_SIZE_ATR_INFO]; /*<  1: 33> SIM Card Answer to reset string                    */
  Uint8                     __dummy0;                 /*< 34:  1> alignment                                          */
  Uint8                     __dummy1;                 /*< 35:  1> alignment                                          */
} Uicc_AtrStringInfo;
#endif /* UICC_ATR_STRING_INFO_CLASS */


/*
 * typedef between var and valtab enums
 */
#ifndef UICC_LEN_CLASS
#define UICC_LEN_CLASS
typedef Uicc_ValLen Uicc_Len;
#endif /* Uicc_Len_CLASS */
#define UICC_MAX_RESULT                (0x100)    
#ifndef UICC_RESULT_INFO_CLASS
#define UICC_RESULT_INFO_CLASS
/*
 * Result Buffer
 * CCDGEN:WriteStruct_Count==1286
 */
typedef struct
{
  Uicc_Len                  len;                      /*<  0:  2> Uicc_Len,  Maximum length of expected data         */
  Uint16                    resultArrayCount;         /*<  2:  2> counter                                            */
  Uint8                     *resultArray;             /*<  4:  4> pointer codetransparent to Result byte             */
} Uicc_ResultInfo;
#endif /* UICC_RESULT_INFO_CLASS */

#define UICC_MAX_DATA_SIZE             (0xff)     
#ifndef UICC_DATA_INFO_CLASS
#define UICC_DATA_INFO_CLASS
/*
 * Data element info
 * CCDGEN:WriteStruct_Count==1288
 */
typedef struct
{
  Uint8                     __dummy0;                 /*<  0:  1> alignment                                          */
  Uint8                     __dummy1;                 /*<  1:  1> alignment                                          */
  Uint8                     __dummy2;                 /*<  2:  1> alignment                                          */
  Uint8                     dataArrayCount;           /*<  3:  1> counter                                            */
  Uint8                     *dataArray;               /*<  4:  4> pointer codetransparent to Data element            */
} Uicc_DataInfo;
#endif /* UICC_DATA_INFO_CLASS */


/*
 * typedef between var and valtab enums
 */
#ifndef UICC_CLA_CLASS
#define UICC_CLA_CLASS
typedef Uicc_ValCla Uicc_Cla;
#endif /* Uicc_Cla_CLASS */
#ifndef UICC_INS_CLASS
#define UICC_INS_CLASS
typedef Uicc_ValIns Uicc_Ins;
#endif /* Uicc_Ins_CLASS */
#ifndef UICC_CMD_HEADER_CLASS
#define UICC_CMD_HEADER_CLASS
/*
 * Transparent command header
 * CCDGEN:WriteStruct_Count==1290
 */
typedef struct
{
  Uicc_Cla                  cla;                      /*<  0:  1> Uicc_Cla,  Class Byte                              */
  Uicc_Ins                  ins;                      /*<  1:  1> Uicc_Ins,  instruction code                        */
  Uint8                     p1;                       /*<  2:  1> Parameter 1 of the SIM APDU                        */
  Uint8                     p2;                       /*<  3:  1> Parameter 2 of the SIM APDU                        */
} Uicc_CmdHeader;
#endif /* UICC_CMD_HEADER_CLASS */

#ifndef UICC_CONFIG_CHARACTERISTICS_CLASS
#define UICC_CONFIG_CHARACTERISTICS_CLASS
/*
 * Configuration Characteristics
 * CCDGEN:WriteStruct_Count==1292
 */
typedef struct
{
  Uint8                     uiccCharacteristics;      /*<  0:  1> UICC Characteristics                               */
  Uint8                     __dummy0;                 /*<  1:  1> alignment                                          */
  Uint8                     __dummy1;                 /*<  2:  1> alignment                                          */
  Uint8                     __dummy2;                 /*<  3:  1> alignment                                          */
} Uicc_ConfigCharacteristics;
#endif /* UICC_CONFIG_CHARACTERISTICS_CLASS */


/*
 * typedef between var and valtab enums
 */
#ifndef UICC_RESET_RETURN_VAL_CLASS
#define UICC_RESET_RETURN_VAL_CLASS
typedef Uicc_ValResetReturnVal Uicc_ResetReturnVal;
#endif /* Uicc_ResetReturnVal_CLASS */
#ifndef UICC_LEN_CLASS
#define UICC_LEN_CLASS
typedef Uicc_ValLen Uicc_Len;
#endif /* Uicc_Len_CLASS */
#ifndef UICC_SW_1_2_CLASS
#define UICC_SW_1_2_CLASS
typedef Uicc_ValSw1_2 Uicc_Sw1_2;
#endif /* Uicc_Sw1_2_CLASS */
#ifndef UICC_CLA_CLASS
#define UICC_CLA_CLASS
typedef Uicc_ValCla Uicc_Cla;
#endif /* Uicc_Cla_CLASS */
#ifndef UICC_INS_CLASS
#define UICC_INS_CLASS
typedef Uicc_ValIns Uicc_Ins;
#endif /* Uicc_Ins_CLASS */
#ifndef UICC_VOLTAGE_SELECT_CLASS
#define UICC_VOLTAGE_SELECT_CLASS
typedef Uicc_ValVoltageSelect Uicc_VoltageSelect;
#endif /* Uicc_VoltageSelect_CLASS */
#ifndef UICC_CONFIG_REQUESTED_CLASS
#define UICC_CONFIG_REQUESTED_CLASS
typedef Uicc_ValConfigRequested Uicc_ConfigRequested;
#endif /* Uicc_ConfigRequested_CLASS */
#ifndef UICC_READER_ID_CLASS
#define UICC_READER_ID_CLASS
typedef Uicc_ValReaderId Uicc_ReaderId;
#endif /* Uicc_ReaderId_CLASS */
#ifndef UICC_DUMMY_CLASS
#define UICC_DUMMY_CLASS
/*
 * 
 * CCDGEN:WriteStruct_Count==1294
 */
typedef struct
{
  Puf_PrimHdr primHdr;
  Uicc_AtrStringInfo        *atrStringInfoPtr;        /*<  0:  4> pointer to SIM Card Info                           */
  Uicc_ResetReturnVal       resetReturnVal;           /*<  4:  1> Uicc_ResetReturnVal,  Reset return value           */
  Uint8                     __dummy0;                 /*<  5:  1> alignment                                          */
  Uint8                     __dummy1;                 /*<  6:  1> alignment                                          */
  Uint8                     __dummy2;                 /*<  7:  1> alignment                                          */
  Uicc_ResultInfo           *resultInfoPtr;           /*<  8:  4> pointer to Result Buffer                           */
  Uicc_Len                  len;                      /*< 12:  2> Uicc_Len,  Maximum length of expected data         */
  Uint8                     __dummy3;                 /*< 14:  1> alignment                                          */
  Uint8                     __dummy4;                 /*< 15:  1> alignment                                          */
  Uicc_DataInfo             *dataInfoPtr;             /*< 16:  4> pointer to Data element info                       */
  Uicc_Sw1_2                sw1_2;                    /*< 20:  2> Uicc_Sw1_2,  Status Words                          */
  Uicc_Cla                  cla;                      /*< 22:  1> Uicc_Cla,  Class Byte                              */
  Uicc_Ins                  ins;                      /*< 23:  1> Uicc_Ins,  instruction code                        */
  Uicc_CmdHeader            cmdHeader;                /*< 24:  4> Transparent command header                         */
  Uicc_VoltageSelect        voltageSelect;            /*< 28:  1> Uicc_VoltageSelect,  Perform Voltage Selection     */
  Uint8                     p1;                       /*< 29:  1> Parameter 1 of the SIM APDU                        */
  Uint8                     p2;                       /*< 30:  1> Parameter 2 of the SIM APDU                        */
  Uicc_ConfigRequested      configRequested;          /*< 31:  1> Uicc_ConfigRequested,  Configuration Characteristics requested */
  Uicc_ConfigCharacteristics *configCharacteristicsPtr; /*< 32:  4> pointer to Configuration Characteristics           */
  Uicc_ReaderId             readerId;                 /*< 36:  1> Uicc_ReaderId,  Reader Id                          */
  Uint8                     uiccCharacteristics;      /*< 37:  1> UICC Characteristics                               */
  Uint8                     __dummy5;                 /*< 38:  1> alignment                                          */
  Uint8                     __dummy6;                 /*< 39:  1> alignment                                          */
} Uicc_Dummy;

#define UICC_DUMMY_SIZE() \
( \
  sizeof(Uicc_Dummy) \
)
#endif /* UICC_DUMMY_CLASS */

#define UICC_MIN_RESULT                (0x1)      
#define UICC_MIN_DATA_SIZE             (0x1)      
#define UICC_MAX_READERS               (0x2)      

#include "pub_L1_misc_leave.h"


#endif
