#ifndef __BTC_ATA_DATATYPE_H_20080114__
#define __BTC_ATA_DATATYPE_H_20080114__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum              /* ATA functional return codes */
{
  ATA_FAIL = -1,           /* execution of command failed */
  ATA_CMPL,                /* execution of command completed */
  ATA_EXCT,                /* execution of command is in progress */
  ATA_BUSY,                /* execution of command is rejected due to a busy command handler */
  ATA_NOSUPPORT            /* execution of command is not support */
} T_ATA_return;

typedef enum
{
    ATA_Response,
    ATA_Unlicited_event
}T_ATA_Response_Type;

typedef struct
{
    T_ATA_Response_Type type;
    int cmdID;
    int  value;
}T_ATA_Response;

#ifdef __cplusplus
}
#endif
#endif /* __BTC_ATA_DATATYPE_H_20080114__ */

