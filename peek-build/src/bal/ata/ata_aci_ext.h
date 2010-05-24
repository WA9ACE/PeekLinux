#ifndef __BTC_ATA_ACI_EXT_H_20080116__
#define __BTC_ATA_ACI_EXT_H_20080116__

#ifdef __cplusplus
extern "C" {
#endif


typedef  int (*T_PRIM_HANDLER)(unsigned long, void *);
typedef  int (*T_CMD_HANDLER)(char *);

typedef struct
{
  T_PRIM_HANDLER prim_handler;
  T_CMD_HANDLER  cmd_handler;
} T_ATA_ACI_EXT;

extern void aci_ext_init(void);
extern int  aci_create(T_PRIM_HANDLER prim_func, T_CMD_HANDLER cmd_func);
extern void aci_delete(int handle);
extern int aci_check_primitive(unsigned long opc, void * data);
extern int aci_check_command(char * cmd);

#ifdef __cplusplus
}
#endif
#endif /* __BTC_ATA_ACI_EXT_H_20080116__ */
