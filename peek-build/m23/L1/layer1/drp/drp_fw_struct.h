#ifndef __DRP_FW_STRUCT__
#define __DRP_FW_STRUCT__

/**********************************************************************************
drp_fw_struct.h
-- Copyright (c) 2004, Texas Instruments, Inc.
-- Author:

main project function file and global declarations
***********************************************************************************
                               Change History
************************************************************************************

Version     Date           Author(s)        Comments
-------------------------------------------------------------------------------------
Ver 0.0     22/1/2007      Sumeer B         Initial version.
************************************************************************************/

#include "sys_types.h"
#include "security/secure_types.h"
#define __L1_TYPES_H__
#include "drp_l1_if.h"

#define MAX_DRP_SCRIPT 1
#define MAX_DRP_API    1

typedef struct {
    UWORD32  add;
    UWORD16  len;
} T_DRP_SCRIPT_TABLE;

typedef struct {
    UINT32 *drp_refsw_ver_mem;
    UINT32 *drp_refsw_tag_mem;
    UINT16 nr_script;
    UINT16 dummy1;
    T_DRP_SCRIPT_TABLE script[MAX_DRP_SCRIPT];
    T_DRP_FUNC_JUMP_TABLE functptr;
} T_DRP_CERT_TABLE;

#endif /* __DRP_FW_STRUCT__*/
