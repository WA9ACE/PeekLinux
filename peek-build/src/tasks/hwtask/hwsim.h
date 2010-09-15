#ifndef __HWSIM__H
#define __HWSIM__H

#define SIM_ACTIVATION 1
#define SIM_VERIFY     2
#define SIM_UNBLOCK    3
#define SIM_CHANGE     4
#define SIM_ENABLE     5
#define SIM_DISABLE    6
#define SIM_LOCKED     7
#define SIM_BLOCKED    8
#define SIM_REMOVED    9
#define HW_IMEI_NOT_VALID      20  

#define SIM_ACTIVE                0
#define SIM_NOT_ACTIVE            1
#define SIM_STK_RESET             2
#define IMEI_NOT_VALID            3

#define NM_AUTOMATIC    0       /* automatic mode set */
#define NM_MANUAL       1       /* manual mode is set */

#define E_NM_FULL_SERVICE      0x0001
#define E_NM_NO_SERVICE        0x0002
#define E_NM_LIMITED_SERVICE   0x0004
#define E_NM_SEARCH_NETWORK    0x0008
#define E_NM_PLMN_LIST         0x0010
#define E_NM_NO_NETWORK        0x0020
#define E_NM_DEREGISTRATION    0x0040
#define E_NM_PREF_PLMN_LIST    0x0080
#define E_NM_CHANGE_PPLMN_OK   0x0100
#define E_NM_CHANGE_PPLMN_ERR  0x0200
#define E_NM_PPLMN_MEM         0x0400
//#define E_MFW_NET_IND          0x0800
//#define E_MFW_TIME_IND         0x1000
#define E_NM_CELL_SELECT       0x1001
#define E_NM_BAND              0x2000
#define E_NM_ABORT_NETWORK     0x3000
#define E_NM_FORBIDDEN_NETWORK 0x9000
#define E_NM_ALL_SERVICES      0xFFFF
#define E_NM_PLMN_LIST_BOOTUP  0x8000

#define HW_SIM_UNKNOWN      0

/*
 * events are organised as bitmaps
 */
#define E_SIM_STATUS            0x0001
#define E_SIM_INSERTED          0x0002
#define E_SIM_RESET             0x0004
#define E_SIM_READ_CALLBACK     0x0008

#define E_SIM_ALL_SERVICES  0x000f

#define LONG_NAME         26

#define NOT_PRESENT_8BIT  0xFF

#define HW_SIM_PIN_REQ      1
#define HW_SIM_PUK_REQ      2
#define HW_SIM_PIN2_REQ     3
#define HW_SIM_PUK2_REQ     4
#define HW_SIM_INVALID_CARD 5
#define HW_SIM_NO_SIM_CARD  6
#define HW_SIM_NO_PIN       7
#define HW_SIM_SUCCESS      8
#define HW_SIM_FAILURE      9

#define HW_FULL_SERVICE      1      /* definition for services */
#define HW_LIMITED_SERVICE   2
#define HW_SEARCH_NETWORK    3
#define HW_NO_SERVICE        0


typedef struct {
        SHORT error_code;
        UBYTE* read_buffer;
} T_HW_READ_CALLBACK;

typedef enum
{
        BOOTUP_STATE_NONE = -1,
        BOOTUP_STATE_INPROGRESS,
        BOOTUP_STATE_COMPLETED  
}T_BOOTUP_STATE;

typedef struct {
  UBYTE sim_procedure;
  UBYTE sim_status;
  UBYTE sim_operation_mode;
  UBYTE sim_pin_retries;
  UBYTE sim_status_type;
} T_HW_SIM_STATUS; /* type definition for E_SIM_STATUS event */

/* SIM configuration information */
typedef struct
{
    UBYTE deper_key [16];
    UBYTE phase;
    UBYTE oper_mode;       /* SIM card functionality   */
    UBYTE pref_lang[5];
    UBYTE access_acm;
    UBYTE access_acmmax;
    UBYTE access_puct;
    UBYTE sim_gidl1[5];
    UBYTE sim_gidl2[5];
} T_HW_SIM_CONFIG;

typedef struct
{
  UBYTE display_condition;
  UBYTE service_provider [LONG_NAME];
} T_EF_SPN;

typedef enum       
{
    HW_DCS_7bits = 0x00,
    HW_DCS_8bits = 0x04,
    HW_DCS_UCS2 = 0x08,
    HW_DCS_SIM,
    HW_ASCII
}T_HW_DCS;   

#endif
