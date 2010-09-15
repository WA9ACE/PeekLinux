#define LONG_NAME         26
#define SHORT_NAME        10
#define SP_NAME           17
#define NUMERIC_LENGTH    7     /* 3 digits MCC + 3 digits MNC + '\0' */

typedef struct
{
        USHORT ci;              /* cell identification */
        USHORT lid;             /* location identification */
}T_HW_CELL_STRUCT;

typedef struct  /* LONG NAME */
{
  UBYTE dcs;
  UBYTE len;                  /* length of string data        */
  UBYTE data[LONG_NAME];      /* string data                  */
} T_HW_LNAME;

typedef struct  /* SHORT NAME */
{
  UBYTE dcs;
  UBYTE len;                  /* length of string data        */
  UBYTE data[SHORT_NAME];     /* string data                  */
} T_HW_SNAME;

typedef struct  /* SERVICE PROVIDER NAME */
{
  UBYTE dcs;
  UBYTE len;               /* length of string data        */
  UBYTE data[SP_NAME];     /* string data                  */
  UBYTE dummy;
} T_HW_SP_NAME;

typedef enum
{
  NM_ROM,                       /* operator name read from ROM   */
  NM_EONS,                      /* operator name read from EONS */
  NM_CPHS,                      /* operator name read from ONS   */
  NM_NITZ,                      /* operator name read from NITZ  */
  NM_NONE = 255         /* no operator name                     */
}T_HW_NM_OPN;

typedef struct
{
    SHORT index;
    T_HW_SP_NAME service_provider; /* service provider name */
    T_HW_LNAME network_long;  /* operator name in long format */
    T_HW_SNAME network_short; /* operator name in short format */
    UBYTE network_numeric[NUMERIC_LENGTH];  /* operator name in numeric format */
    UBYTE display_condition;
    UBYTE roaming_indicator;    /* PLMN in HPLMN or not */
    UBYTE forbidden_indicator;  /* PLMN is member of the forbidden PLMN list */
    UBYTE *fieldstrength;       /* fieldstrength of PLMN */
    T_HW_NM_OPN opn_read_from; /* Holds the source id from where operator name was read */
} T_HW_NETWORK_STRUCT;
