#ifndef L1_DRP_IF_H
#define L1_DRP_IF_H

#include "sys_types.h"
#if (OP_L1_STANDALONE == 0)
#include "ffs.h"
#else
#include "ffs_simu.h"
#endif /* OP_L1_STANDALONE */


typedef struct T_MODEM_FUNC_JUMP_TABLE_TAG
{
  SYS_UWORD32 (*convert_nanosec_to_cycles)(SYS_UWORD32 time);
  void (*wait_ARM_cycles)(SYS_UWORD32 cpt_loop);
  T_FFS_FD  (*ffs_open)(const char *name, T_FFS_OPEN_FLAGS option);
  T_FFS_SIZE (*ffs_write)(T_FFS_FD fdi, void *src, T_FFS_SIZE amount);
  T_FFS_RET (*ffs_close)(T_FFS_FD fdi);
  T_FFS_SIZE (*ffs_read)(T_FFS_FD fdi, void *src, T_FFS_SIZE size);
  int (*ffs_fread)(const char *name, void *addr, int size);
  void (*TP_Enable)(SYS_UWORD16 on);
}T_MODEM_FUNC_JUMP_TABLE;

typedef struct T_MODEM_VAR_JUMP_TABLE_TAG
{
    volatile SYS_UWORD16 ** TP_ptr;
#if (L1_FF_MULTIBAND == 0)    
    SYS_UWORD8 * l1_config_std_id;
#endif
}T_MODEM_VAR_JUMP_TABLE;
#endif /* L1_DRP_IF_H */
/*==============================================================================*/

/*==============================================================================*/
