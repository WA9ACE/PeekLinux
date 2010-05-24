#ifndef __BTC_ATA_CMD_PARSER_H_20080114__
#define __BTC_ATA_CMD_PARSER_H_20080114__

#ifdef __cplusplus
extern "C" {
#endif

void ata_parse_at_command(const char *atstr);
void ata_parse_at_response(const char *rsp);
int ata_param_count(void);
int ata_get_param_int(const char *atStr,int index);
int ata_get_param_string(const char *atStr,int index,unsigned char *to,int sz);

/* e.g. AT+CPUC? */
#define ATA_AT_QUERY_READ (-1)
/* e.g. AT+CPUC=? */
#define ATA_AT_QUERY_TEST (-2)

#ifdef __cplusplus
}
#endif
#endif /* __BTC_ATA_CMD_PARSER_H_20080114__ */
