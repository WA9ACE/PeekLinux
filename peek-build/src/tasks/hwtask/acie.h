#ifndef __ACIE__H
#define __ACIE__H

#define MAX_ACI_HANDLER 10

typedef  BOOL (*T_PRIM_HANDLER)(ULONG, void *);
typedef  BOOL (*T_CMD_HANDLER)(char *);

typedef struct
{
  T_PRIM_HANDLER prim_handler;
  T_CMD_HANDLER  cmd_handler;
} T_HW_ACI_EXT;

EXTERN void aci_ext_init(void);
EXTERN int  aci_create(T_PRIM_HANDLER prim_func, T_CMD_HANDLER cmd_func);
EXTERN void aci_delete(int handle);
EXTERN BOOL aci_check_primitive(ULONG opc, void * data);
EXTERN BOOL aci_check_command(char * cmd);
#endif
