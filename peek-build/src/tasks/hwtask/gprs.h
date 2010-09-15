#ifndef __HW_GPRS_H
#define __HW_GPRS_H

#define E_HW_GPRS_S_CNTXT       0x00000001
#define E_HW_GPRS_S_ATT         0x00000002
#define E_HW_GPRS_S_ACT         0x00000004
#define E_HW_GPRS_R_ACT         0x00000008
#define E_HW_GPRS_S_DATA        0x00000010
#define E_HW_GPRS_R_DATA        0x00000020
#define E_HW_GPRS_S_QOS         0x00000040
#define E_HW_GPRS_R_QOS         0x00000080
#define E_HW_GPRS_S_QOS_MIN     0x00000100
#define E_HW_GPRS_S_CLASS       0x00000200
#define E_HW_GPRS_S_PDPADDR     0x00000400
#define E_HW_GPRS_S_AUTORESP    0x00000800
#define E_HW_GPRS_S_ANS         0x00001000
#define E_HW_GPRS_R_ANS         0x00002000
#define E_HW_GPRS_S_EREP        0x00004000
#define E_HW_GPRS_R_EREP_RJ     0x00008000
#define E_HW_GPRS_R_EREP_ATT    0x00010000
#define E_HW_GPRS_R_EREP_ACT    0x00020000
#define E_HW_GPRS_R_EREP_DEACT  0x00040000
#define E_HW_GPRS_R_EREP_CLASS  0x00080000
#define E_HW_GPRS_R_REG         0x00200000
#define E_HW_GPRS_S_SMSSERV     0x00400000
#define E_HW_GPRS_OK            0x00800000
#define E_HW_GPRS_ERROR         0x01000000
#define E_HW_GPRS_CONNECT       0x02000000
#define E_HW_GPRS_ERR_CONNECT   0x04000000
#define E_HW_GPRS_COUNTER                      0x08000000

#define E_HW_GPRS_ALL_EVENTS           0x0FFFFFFF


void gprs_ok(T_ACI_AT_CMD cmdId);

#endif
